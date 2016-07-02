/*****************************************************************************
模块名      : 数据会议系统各模块
文件名      : dcstrc.h
相关文件    : 无
文件实现功能: 数据会议系统各模块共用的结构定义和实现
作者        : 赖齐
版本        : 4.0
-----------------------------------------------------------------------------
修改记录:
日期(D/M/Y)     版本     修改人      修改内容
16/08/2005      0.1      赖齐        从V3.6移植
******************************************************************************/

#ifndef _DCSTRC_H
#define _DCSTRC_H

#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef WIN32
#include "ptlib.h"
#endif

#include "dccommon.h"
#include <vector>

#ifdef WIN32
    #pragma pack( push )
    #pragma pack( 1 )
#else
    #include<sys/types.h>
    #include<sys/socket.h>
    #include <math.h>
    #define LPSTR   char *
    #define LPCSTR  const char *
#endif

typedef enum
{
    enum_MtInvited,
    enum_MtCallIn,
    enum_MtRemote
}enum_ConfMtType;


typedef enum
{
    enum_Mcu,
    enum_Mt
}enum_DeviceType;


#ifndef _KDC_INTE_TEST_

//会议号结构
struct CConfId
{
protected:
	u8  	m_abyConfId[16];

public:
/*====================================================================
    函数名      ：GetConfId
    功能        ：获得会议号数组
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 * pbyConfId, 传入的BUFFER数组指针，用来返回会议号
			      u8 byBufLen, BUFFER大小
    返回值说明  ：实际返回数组大小
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/31    1.0         LI Yi         创建
====================================================================*/
    u8   CConfId::GetConfId( u8   * pbyConfId, u8   byBufLen ) const
    {
		u16 wTempLen = sizeof(m_abyConfId);
		u8 byLen = wTempLen < byBufLen ? (u8)wTempLen : byBufLen ;
        memcpy( pbyConfId, m_abyConfId, byLen );
        
        return( byLen );
    }

/*====================================================================
    函数名      ：GetConfIdString
    功能        ：获得会议号字符串
    算法实现    ：
    引用全局变量：
    输入参数说明：LPSTR lpszConfId, 传入的字符串BUFFER数组指针，用来
	                   返回0结尾会议号字符串
			      u8 byBufLen, BUFFER大小
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/31    1.0         LI Yi         创建
====================================================================*/
    void GetConfIdString( LPSTR lpszConfId, u8   byBufLen ) const
    {
        u8	byLoop;
        
        for( byLoop = 0; byLoop < sizeof( m_abyConfId ) && byLoop < ( byBufLen - 1 ) / 2; byLoop++ )
        {
            sprintf( lpszConfId + byLoop * 2, "%.2x", m_abyConfId[byLoop] );
        }
        lpszConfId[byLoop * 2] = '\0';
    }

/*====================================================================
    函数名      ：CreateConfId
    功能        ：创建会议号
    算法实现    ：根据当前时间和随机值
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/06    1.0         LI Yi         创建
====================================================================*/
    void CConfId::CreateConfId( void )
    {
        struct tm	*ptmCurTime;
        time_t		tCurTime = time( NULL );
        u8	byTemp;
        u16	wTemp;
        
        memset( m_abyConfId, 0, sizeof( m_abyConfId ) );
        
        //get current time
        ptmCurTime = localtime( &tCurTime );
        //year
        wTemp = (u16)(ptmCurTime->tm_year + 1900);
        memcpy( m_abyConfId, &wTemp, sizeof( u16 ) );
        //month
        byTemp = (u8)(ptmCurTime->tm_mon + 1);
        memcpy( m_abyConfId + 2, &byTemp, sizeof( u8 ) );
        //day
        byTemp = (u8)(ptmCurTime->tm_mday);
        memcpy( m_abyConfId + 3, &byTemp, sizeof( u8 ) );
        //hour
        byTemp = (u8)(ptmCurTime->tm_hour);
        memcpy( m_abyConfId + 4, &byTemp, sizeof( u8 ) );
        //minute
        byTemp = (u8)(ptmCurTime->tm_min);
        memcpy( m_abyConfId + 5, &byTemp, sizeof( u8 ) );
        //second
        byTemp = (u8)(ptmCurTime->tm_sec);
        memcpy( m_abyConfId + 6, &byTemp, sizeof( u8 ) );
        //msec
        wTemp = 0;
        memcpy( m_abyConfId + 7, &wTemp, sizeof( u16 ) );
        
        //rand
        wTemp = (u16)rand();
        memcpy( m_abyConfId + 9, &wTemp, sizeof( u16 ) );
    }
    
/*====================================================================
    函数名      ：SetConfId
    功能        ：设置会议号数组
    算法实现    ：
    引用全局变量：
    输入参数说明：const CConfId & cConfId, 输入会议号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/31    1.0         LI Yi         创建
====================================================================*/
    void SetConfId( const CConfId & cConfId )
    {
        cConfId.GetConfId( m_abyConfId, sizeof( m_abyConfId ) );
    }    

/*====================================================================
    函数名      ：SetConfId
    功能        ：设置会议号数组
    算法实现    ：
    引用全局变量：
    输入参数说明：const u8 * pbyConfId, 传入的数组指针
			      u8 byBufLen, BUFFER大小
    返回值说明  ：实际设置数组大小
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/31    1.0         LI Yi         创建
====================================================================*/
    u8   SetConfId( const u8   * pbyConfId, u8   byBufLen )
    {
		u16 wTempLen = sizeof( m_abyConfId );
    	u8 bySize = wTempLen < byBufLen ? (u8)wTempLen : byBufLen ;
        
        memset( m_abyConfId, 0, sizeof( m_abyConfId ) );
        memcpy( m_abyConfId, pbyConfId, bySize );
        
        return( bySize );
    }

/*====================================================================
    函数名      ：SetConfId
    功能        ：设置会议号数组
    算法实现    ：
    引用全局变量：
    输入参数说明：LPCSTR lpszConfId, 传入的会议号字符串
    返回值说明  ：实际设置数组大小
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/31    1.0         LI Yi         创建
====================================================================*/
    u8   SetConfId( LPCSTR lpszConfId )
    {
        LPCSTR	lpszTemp = lpszConfId;
        char	achTemp[3], *lpszStop;
        u8	byCount = 0;
        
        memset( m_abyConfId, 0, sizeof( m_abyConfId ) );
        while( lpszTemp != NULL && lpszTemp + 1 != NULL && byCount < sizeof( m_abyConfId ) )
        {
            memcpy( achTemp, lpszTemp, 2 );
            achTemp[2] = '\0';
            m_abyConfId[byCount] = ( u8 )strtoul( achTemp, &lpszStop, 16 );
            byCount++;
            lpszTemp += 2;
        }
        
        return( byCount );
    }    
    
/*====================================================================
    函数名      ：IsNull
    功能        ：判断会议号是否为空
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：TRUE/FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/03/31    1.0         LI Yi         创建
====================================================================*/
    BOOL IsNull( void ) const
    {
        u8	abyNull[sizeof( m_abyConfId )];
        
        memset( abyNull, 0, sizeof( abyNull ) );
        if( memcmp( m_abyConfId, abyNull, sizeof( m_abyConfId ) ) == 0 )
            return( TRUE );
        else
            return( FALSE );
    }

/*====================================================================
    函数名      ：SetNull
    功能        ：设置会议号为空
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/03/31    1.0         LI Yi         创建
====================================================================*/
    void SetNull( void )
    {
        memset( m_abyConfId, 0, sizeof( m_abyConfId ) );
    }


/*====================================================================
    函数名      ：operator==
    功能        ：操作符重载
    算法实现    ：
    引用全局变量：
    输入参数说明：const CConfId & cConfId, 输入会议号
    返回值说明  ：相等返回TRUE，否则返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/31    1.0         LI Yi         创建
====================================================================*/
    BOOL operator == ( const CConfId & cConfId ) const         
    {
        u8	abyTemp[sizeof( m_abyConfId )];
        
        cConfId.GetConfId( abyTemp, sizeof( abyTemp ) );
        if( memcmp( abyTemp, m_abyConfId, sizeof( m_abyConfId ) ) == 0 )
            return( TRUE );
        else
            return( FALSE );
    }
    
	void Print( void )
	{
		char achBuf[33];
		achBuf[32] = 0;
		GetConfIdString( achBuf, 33 );
		OspPrintf( TRUE, FALSE, "%s", achBuf);
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

#ifdef SERV_MSGHEAD_LEN
    #undef SERV_MSGHEAD_LEN
#endif
#define      SERV_MSGHEAD_LEN             (u16)0x30    //消息头长度

#endif

//业务消息接口常量定义
const u16 SERV_MSGHEAD_SRIALNO_OFFSET	= 0;	//流水号偏移量
const u16 SERV_MSGHEAD_SRIALNO_LEN		= 2;	//流水号字节数
const u16 SERV_MSGHEAD_SRCMCUID_OFFSET	= SERV_MSGHEAD_SRIALNO_OFFSET + SERV_MSGHEAD_SRIALNO_LEN;	//源MCU号偏移量
const u16 SERV_MSGHEAD_SRCMCUID_LEN		= 2;	//源MCU号字节数
const u16 SERV_MSGHEAD_SRCMTID_OFFSET	= SERV_MSGHEAD_SRCMCUID_OFFSET + SERV_MSGHEAD_SRCMCUID_LEN;	//源MT号偏移量
const u16 SERV_MSGHEAD_SRCMTID_LEN		= 1;	//源MT号字节数
const u16 SERV_MSGHEAD_SRCSSNID_OFFSET	= SERV_MSGHEAD_SRCMTID_OFFSET + SERV_MSGHEAD_SRCMTID_LEN;	//源SESSION号偏移量
const u16 SERV_MSGHEAD_SRCSSNID_LEN	= 1;		//源SESSION号字节数
const u16 SERV_MSGHEAD_DSTMCUID_OFFSET	= SERV_MSGHEAD_SRCSSNID_OFFSET + SERV_MSGHEAD_SRCSSNID_LEN;	//目的MCU号偏移量
const u16 SERV_MSGHEAD_DSTMCUID_LEN		= 2;	//MCU号字节数
const u16 SERV_MSGHEAD_DSTMTID_OFFSET	= SERV_MSGHEAD_DSTMCUID_OFFSET + SERV_MSGHEAD_DSTMCUID_LEN;	//目的MT号偏移量
const u16 SERV_MSGHEAD_DSTMTID_LEN		= 1;	//MT号字节数
const u16 SERV_MSGHEAD_CHNINDEX_OFFSET	= SERV_MSGHEAD_DSTMTID_OFFSET + SERV_MSGHEAD_DSTMTID_LEN;	//视频通道索引偏移量
const u16 SERV_MSGHEAD_CHNINDEX_LEN		= 1;	//视频通道索引字节数
const u16 SERV_MSGHEAD_CONFID_OFFSET	= SERV_MSGHEAD_CHNINDEX_OFFSET + SERV_MSGHEAD_CHNINDEX_LEN;	//会议号偏移量
const u16 SERV_MSGHEAD_CONFID_LEN		= 16;	//会议号字节数
const u16 SERV_MSGHEAD_EVENTID_OFFSET	= SERV_MSGHEAD_CONFID_OFFSET + SERV_MSGHEAD_CONFID_LEN;		//事件号偏移量
const u16 SERV_MSGHEAD_EVENTID_LEN		= 2;	//事件号字节数
const u16 SERV_MSGHEAD_TIMER_OFFSET		= SERV_MSGHEAD_EVENTID_OFFSET + SERV_MSGHEAD_EVENTID_LEN;	//定时偏移量
const u16 SERV_MSGHEAD_TIMER_LEN		= 2;	//定时字节数
const u16 SERV_MSGHEAD_ERRORCODE_OFFSET	= SERV_MSGHEAD_TIMER_OFFSET + SERV_MSGHEAD_TIMER_LEN;		//错误码偏移量
const u16 SERV_MSGHEAD_ERRORCODE_LEN	= 2;	//错误码字节数
const u16 SERV_MSGHEAD_BODYLEN_OFFSET	= SERV_MSGHEAD_ERRORCODE_OFFSET + SERV_MSGHEAD_ERRORCODE_LEN;	//消息体长度偏移量
const u16 SERV_MSGHEAD_BODYLEN_LEN		= 2;	//消息体长度字节数
const u16 SERV_MSGBODY_OFFSET			= SERV_MSGHEAD_LEN;	//消息体偏移量


//业务消息类，最大处理4K长度消息
class C16KServMsg
{
protected:
	u8	m_abyBuffer[0x4000];		//消息Buffer，4K

/*====================================================================
    功能        ：初始化
    输入参数说明：无
    返回值说明  ：无
====================================================================*/
	void Init( void )
    {
        memset( m_abyBuffer, 0, sizeof( m_abyBuffer ) );	//清零
    }

/*====================================================================
    功能        ：设置消息体长度
    输入参数说明：wMsgBodyLen－－消息体长度
    返回值说明  ：无
====================================================================*/
	void SetMsgBodyLen( u16 wMsgBodyLen )
    {
        wMsgBodyLen = htons( wMsgBodyLen );
        memcpy( m_abyBuffer + SERV_MSGHEAD_BODYLEN_OFFSET, &wMsgBodyLen, SERV_MSGHEAD_BODYLEN_LEN );
    }

public:
/*====================================================================
    功能        ：constructor
    输入参数说明：无
    返回值说明  ：
====================================================================*/
	C16KServMsg( void )
    {
        Init();
    }

/*====================================================================
    功能        ：constructor
    输入参数说明：u8 * const pbyMsg, 要赋值的消息指针
				  u16 wMsgLen, 要赋值的消息长度，必须大于等于16
    返回值说明  ：
====================================================================*/
	C16KServMsg( u8 * const pbyMsg, u16 wMsgLen )
    {
        Init();
        
        if( wMsgLen < SERV_MSGHEAD_LEN || pbyMsg == NULL )
            return;
        
		u16 wTempLen = sizeof( m_abyBuffer ) ;
        wMsgLen = wTempLen < wMsgLen ? wTempLen : wMsgLen ;
        memcpy( m_abyBuffer, pbyMsg, wMsgLen );
        //set length
        SetMsgBodyLen( (u16)(wMsgLen - SERV_MSGHEAD_LEN) );
    }

/*====================================================================
    功能        ：distructor
    输入参数说明：
    返回值说明  ：
====================================================================*/
	~C16KServMsg( void )
    {
    }

/*====================================================================
    功能        ：消息头清零
    输入参数说明：
    返回值说明  ：
====================================================================*/
	void ClearHdr( void )
    {
        u16		wBodyLen = GetMsgBodyLen();
        
        memset( m_abyBuffer, 0, SERV_MSGHEAD_LEN );	//清零
        SetMsgBodyLen( wBodyLen );
    }

/*====================================================================
    功能        ：获取流水号信息
    输入参数说明：无
    返回值说明  ：流水号
====================================================================*/
	u16 GetSerialNo( void ) const
    {
        u16		wTemp;
        
        memcpy( &wTemp, m_abyBuffer + SERV_MSGHEAD_SRIALNO_OFFSET, SERV_MSGHEAD_SRIALNO_LEN );
        return( ntohs( wTemp ) );
    }

/*====================================================================
    功能        ：设置流水号信息
    输入参数说明：u16 wSerialNo, 流水号
    返回值说明  ：无
====================================================================*/
	void SetSerialNo( u16 wSerialNo )
    {
        wSerialNo = htons( wSerialNo );
        memcpy( m_abyBuffer + SERV_MSGHEAD_SRIALNO_OFFSET, &wSerialNo, SERV_MSGHEAD_SRIALNO_LEN );
    }

/*====================================================================
    功能        ：获取源MCU号信息
    输入参数说明：无
    返回值说明  ：MCU号
====================================================================*/
	u16 GetSrcMcuId( void ) const
    {
        u16		wTemp;
        
        memcpy( &wTemp, m_abyBuffer + SERV_MSGHEAD_SRCMCUID_OFFSET, SERV_MSGHEAD_SRCMCUID_LEN );
        return( ntohs( wTemp ) );
    }

/*====================================================================
    功能        ：获取源MT号信息
    输入参数说明：无
    返回值说明  ：MT号
====================================================================*/
	u8 GetSrcMtId( void ) const
    {
        return( *( u8 * )( m_abyBuffer + SERV_MSGHEAD_SRCMTID_OFFSET ) );
    }

/*====================================================================
    功能        ：设置源MCU
    输入参数说明：u16 wSrcMcuId, MCU号
    返回值说明  ：无
====================================================================*/
	void SetSrcMcu( u16 wSrcMcuId )
    {
        wSrcMcuId = htons( wSrcMcuId );
        memcpy( m_abyBuffer + SERV_MSGHEAD_SRCMCUID_OFFSET, &wSrcMcuId, SERV_MSGHEAD_SRCMCUID_LEN );
        memset( m_abyBuffer + SERV_MSGHEAD_SRCMTID_OFFSET, 0, SERV_MSGHEAD_SRCMTID_LEN );
    }

/*====================================================================
    功能        ：设置源MT
    输入参数说明：u16 wSrcMcuId, MCU号
				  u8 bySrcMtId, MT号
    返回值说明  ：无
====================================================================*/
	void SetSrcMt( u16 wSrcMcuId, u8 bySrcMtId )
    {
        wSrcMcuId = htons( wSrcMcuId );
        memcpy( m_abyBuffer + SERV_MSGHEAD_SRCMCUID_OFFSET, &wSrcMcuId, SERV_MSGHEAD_SRCMCUID_LEN );
        memcpy( m_abyBuffer + SERV_MSGHEAD_SRCMTID_OFFSET, &bySrcMtId, SERV_MSGHEAD_SRCMTID_LEN );
    }

/*====================================================================
    功能        ：获取会话号信息
    输入参数说明：无
    返回值说明  ：Session号，0表示建立链接
====================================================================*/
	u8 GetSrcSsnId( void ) const
    {
        return( *( u8 * )( m_abyBuffer + SERV_MSGHEAD_SRCSSNID_OFFSET ) );
    }

/*====================================================================
    功能        ：设置会话号信息
    输入参数说明：u8 bySrcSsnId, Session号，0表示建立链接
    返回值说明  ：无
====================================================================*/
	void SetSrcSsnId( u8 bySrcSsnId )
    {
        memcpy( m_abyBuffer + SERV_MSGHEAD_SRCSSNID_OFFSET, &bySrcSsnId, SERV_MSGHEAD_SRCSSNID_LEN );
    }

/*====================================================================
    功能        ：设置无源，不需应答
    输入参数说明：无
    返回值说明  ：无
====================================================================*/
	void SetNoSrc( void )
    {
        SetSrcMcu( 0 );
        SetSrcSsnId( 0 );
    }

/*====================================================================
    功能        ：获取目的MCU号信息
    输入参数说明：无
    返回值说明  ：MCU号
====================================================================*/
	u16 GetDstMcuId( void ) const
    {
        u16		wTemp;
        
        memcpy( &wTemp, m_abyBuffer + SERV_MSGHEAD_DSTMCUID_OFFSET, SERV_MSGHEAD_DSTMCUID_LEN );
        return( ntohs( wTemp  ) );
    }

/*====================================================================
    功能        ：获取目的MT号信息
    输入参数说明：无
    返回值说明  ：MT号
====================================================================*/
	u8 GetDstMtId( void ) const
    {
        return( *( u8 * )( m_abyBuffer + SERV_MSGHEAD_DSTMTID_OFFSET ) );
    }

/*====================================================================
    功能        ：设置目的MCU
    输入参数说明：u16 wDstMcuId, MCU号
    返回值说明  ：无
====================================================================*/
	void SetDstMcu( u16 wDstMcuId )
    {
        wDstMcuId = htons( wDstMcuId );
        memcpy( m_abyBuffer + SERV_MSGHEAD_DSTMCUID_OFFSET, &wDstMcuId, SERV_MSGHEAD_DSTMCUID_LEN );
        memset( m_abyBuffer + SERV_MSGHEAD_DSTMTID_OFFSET, 0, SERV_MSGHEAD_DSTMTID_LEN );
    }

/*====================================================================
    功能        ：设置目的MT
    输入参数说明：u16 wDstMcuId, MCU号
				  u8 byDstMtId, MT号
    返回值说明  ：无
====================================================================*/
	void SetDstMt( u16 wDstMcuId, u8 byDstMtId )
    {
        wDstMcuId = htons( wDstMcuId );
        memcpy( m_abyBuffer + SERV_MSGHEAD_DSTMCUID_OFFSET, &wDstMcuId, SERV_MSGHEAD_DSTMCUID_LEN );
        memcpy( m_abyBuffer + SERV_MSGHEAD_DSTMTID_OFFSET, &byDstMtId, SERV_MSGHEAD_DSTMTID_LEN );
    }

/*====================================================================
    功能        ：设置无目的，不需应答
    输入参数说明：无
    返回值说明  ：无
====================================================================*/
	void SetNoDst( void )
    {
        SetDstMcu( 0 );
    }

/*====================================================================
    功能        ：获取会议号信息
    输入参数说明：无
    返回值说明  ：会议号，0表示创建
====================================================================*/
	CConfId GetConfId( void ) const
    {
        CConfId	cConfId;
        
        cConfId.SetConfId( m_abyBuffer + SERV_MSGHEAD_CONFID_OFFSET, SERV_MSGHEAD_CONFID_LEN );
        
        return( cConfId );
    }

/*====================================================================
    功能        ：设置会议号信息
    输入参数说明：const CConfId & cConfId，会议号，全0表示创建
    返回值说明  ：无
====================================================================*/
	void SetConfId( const CConfId & cConfId )
    {
        cConfId.GetConfId( m_abyBuffer + SERV_MSGHEAD_CONFID_OFFSET, SERV_MSGHEAD_CONFID_LEN );
    }

/*====================================================================
    功能        ：设置会议号信息为空
    输入参数说明：无
    返回值说明  ：无
====================================================================*/
	void SetNullConfId( void )
    {
        CConfId		cConfId;
        
        cConfId.SetNull();
        SetConfId( cConfId );
    }

/*====================================================================
    功能        ：获取事件号信息
    输入参数说明：无
    返回值说明  ：事件号
====================================================================*/
	u16 GetEventId( void ) const
    {
        u16	wTemp = 0;
        
        memcpy( &wTemp, m_abyBuffer + SERV_MSGHEAD_EVENTID_OFFSET, SERV_MSGHEAD_EVENTID_LEN );
        return( ntohs( wTemp ) );
    }

/*====================================================================
    功能        ：设置事件号信息
    输入参数说明：u16 wEventId, 事件号
    返回值说明  ：无
====================================================================*/
	void SetEventId( u16 wEventId )
    {
        wEventId = htons( wEventId );
        memcpy( m_abyBuffer + SERV_MSGHEAD_EVENTID_OFFSET, &wEventId, SERV_MSGHEAD_EVENTID_LEN );
    }

/*====================================================================
    功能        ：获取定时器信息
    输入参数说明：无
    返回值说明  ：定时器时长，0表示无应答
====================================================================*/
	u16 GetTimer( void ) const
    {
        u16	wTemp = 0;
        
        memcpy( &wTemp, m_abyBuffer + SERV_MSGHEAD_TIMER_OFFSET, SERV_MSGHEAD_TIMER_LEN );
        return( ntohs( wTemp ) );
    }

/*====================================================================
    功能        ：设置定时器信息
    输入参数说明：u16 wTimer, 定时器时长，0表示无应答
    返回值说明  ：无
====================================================================*/
	void SetTimer( u16 wTimer )
    {
        wTimer = htons( wTimer );
        memcpy( m_abyBuffer + SERV_MSGHEAD_TIMER_OFFSET, &wTimer, SERV_MSGHEAD_TIMER_LEN );
    }

/*====================================================================
    功能        ：获取错误码信息（NACK消息用）
    输入参数说明：无
    返回值说明  ：错误码
====================================================================*/
	u16 GetErrorCode( void ) const
    {
        u16	wTemp = 0;
        
        memcpy( &wTemp, m_abyBuffer + SERV_MSGHEAD_ERRORCODE_OFFSET, SERV_MSGHEAD_ERRORCODE_LEN );
        return( ntohs( wTemp ) );
    }

/*====================================================================
    功能        ：设置错误码信息（NACK消息用）
    输入参数说明：u16 wErrorCode, 错误码
    返回值说明  ：无
====================================================================*/
	void SetErrorCode( u16 wErrorCode )
    {
        wErrorCode = htons( wErrorCode );
        memcpy( m_abyBuffer + SERV_MSGHEAD_ERRORCODE_OFFSET, &wErrorCode, SERV_MSGHEAD_ERRORCODE_LEN );
    }

/*====================================================================
    功能        ：获取视频通道索引
    输入参数说明：无
    返回值说明  ：视频通道索引
====================================================================*/
	u8 GetChnIndex( void ) const
    {
        u8 byTemp = 0;;
        
        memcpy( &byTemp, m_abyBuffer + SERV_MSGHEAD_CHNINDEX_OFFSET, SERV_MSGHEAD_CHNINDEX_LEN );
        return( byTemp );
    }

/*====================================================================
    功能        ：设置视频通道索引
    输入参数说明：u8 byIndex, 视频通道索引
    返回值说明  ：无
====================================================================*/
	void SetChnIndex( u8 byIndex )
    {
        memcpy( m_abyBuffer + SERV_MSGHEAD_CHNINDEX_OFFSET, &byIndex, SERV_MSGHEAD_CHNINDEX_LEN );
    }

/*====================================================================
    功能        ：获取消息体长度信息
    输入参数说明：无
    返回值说明  ：消息体长度
====================================================================*/
	u16 GetMsgBodyLen( void ) const
    {
        u16 wTemp = 0;
        
        memcpy( &wTemp, m_abyBuffer + SERV_MSGHEAD_BODYLEN_OFFSET, SERV_MSGHEAD_BODYLEN_LEN );
        return( ntohs( wTemp ) );
    }

/*====================================================================
    功能        ：获取消息体，由用户申请BUFFER，如果过小做截断处理
    输入参数说明：u8 * pbyMsgBodyBuf, 返回的消息体
				  u16 wBufLen, BUFFER大小
    返回值说明  ：实际返回的消息体长度
====================================================================*/
	u16 GetMsgBody( u8 * pbyMsgBodyBuf, u16 wBufLen ) const
    {
        memcpy( pbyMsgBodyBuf, m_abyBuffer + SERV_MSGBODY_OFFSET, wBufLen );
        
		u16 wTempLen = GetMsgBodyLen();
        return wTempLen < wBufLen ? wTempLen : wBufLen;
    }

/*====================================================================
    功能        ：获取消息体指针，用户不需提供BUFFER
    输入参数说明：无
    返回值说明  ：u8 * const指针
====================================================================*/
	u8 * const GetMsgBody( void ) const
    {
        return( ( u8 * const )( m_abyBuffer + SERV_MSGBODY_OFFSET ) );
    }

/*====================================================================
    功能        ：设置消息体
    输入参数说明：u8 * const pbyMsgBody, 传入的消息体，缺省为NULL
				  u16 wLen, 传入的消息体长度，缺省为NULL
    返回值说明  ：无
====================================================================*/
	void SetMsgBody( u8 * const pbyMsgBody = NULL, u16 wLen = 0 )
    {
		u16 wTempLen = sizeof(m_abyBuffer) - SERV_MSGHEAD_LEN;
    	wLen = wLen < wTempLen ? wLen : wTempLen;
        
        memcpy( m_abyBuffer + SERV_MSGBODY_OFFSET, pbyMsgBody, wLen );
        SetMsgBodyLen( wLen );
    }

/*====================================================================
    功能        ：添加消息体
    输入参数说明：u8 * const pbyMsgBody, 传入的消息体，缺省为NULL
				  u16 wLen, 传入的消息体长度，缺省为NULL
    返回值说明  ：无
====================================================================*/
	void CatMsgBody( u8 * const pbyMsgBody, u16 wLen )
    {
    	u16 wTempLen = u16( sizeof( m_abyBuffer ) - SERV_MSGHEAD_LEN - GetMsgBodyLen() );
    	wLen = wLen < wTempLen ? wLen : wTempLen;
        
        memcpy( m_abyBuffer + SERV_MSGBODY_OFFSET + GetMsgBodyLen(), pbyMsgBody, wLen );
        SetMsgBodyLen( (u16)(GetMsgBodyLen() + wLen) );
    }

/*====================================================================
    功能        ：获取整个消息长度
    输入参数说明：无
    返回值说明  ：整个消息长度
====================================================================*/
	u16 GetServMsgLen( void ) const
    {
        return( (u16)(GetMsgBodyLen() + SERV_MSGHEAD_LEN) );
    }

/*====================================================================
    功能        ：获取整个消息，由用户申请BUFFER，如果过小做截断处理
    输入参数说明：u8 * pbyMsgBuf, 返回的消息
				  u16 wBufLen, BUFFER大小
    返回值说明  ：实际返回的消息长度
====================================================================*/
	u16 GetServMsg( u8 * pbyMsgBuf, u16 wBufLen ) const
    {
        memcpy( pbyMsgBuf, m_abyBuffer, wBufLen );
        
        u16 wTempLen = (u16)( GetMsgBodyLen() + SERV_MSGHEAD_LEN );
        return wTempLen < wBufLen ? wTempLen : wBufLen;
    }

/*====================================================================
    功能        ：获取整个消息指针，用户不需提供BUFFER
    输入参数说明：无
    返回值说明  ：u8 * const指针
====================================================================*/
	u8 * const GetServMsg( void ) const
    {
        return( ( u8 * const )( m_abyBuffer ) );
    }

/*====================================================================
    功能        ：设置整个消息
    输入参数说明：u8 * const pbyMsg, 传入的整个消息
				  u16 wLen, 传入的消息长度
    返回值说明  ：无
====================================================================*/
	void SetServMsg( u8 * const pbyMsg, u16 wMsgLen )
    {
        if( wMsgLen < SERV_MSGHEAD_LEN )
        {
            printf( "CServMsg: Exception -- invalid service message!\n" );
            return;
        }
        
        u16 wTempLen = sizeof(m_abyBuffer);
        wMsgLen = wTempLen < wMsgLen ? wTempLen : wMsgLen;
        
        memcpy( m_abyBuffer, pbyMsg, wMsgLen );
        SetMsgBodyLen( (u16)(wMsgLen - SERV_MSGHEAD_LEN) );
    }

/*====================================================================
    功能        ：操作符重载
    输入参数说明：CServMsg & cServMsg, 赋值的消息引用
    返回值说明  ：CServMsg对象应用
====================================================================*/
	const C16KServMsg & operator= ( const C16KServMsg & cServMsg )
    {
        u16	wLen = cServMsg.GetServMsgLen();
        
        memcpy( m_abyBuffer, cServMsg.GetServMsg(), wLen );
        return( *this );
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//DCServer信息
struct TDCServerInfo
{
protected:
	u32	    m_dwIpAddr;						    //服务器地址,网络序
	char	m_achAlias[MAX_ALIAS_LENGTH];	    //服务器别名
    u32     m_dwMcuIpAddr;                      //MCU地址，网络序
	u32		m_dwBackupMcuIpAddr;				//备用MCU地址，网络序
    u16	    m_wMcuPort;					        //MCU端口,网络序
    u16     m_wBackupMcuPort;                   //备用MCU端口，网络序
    u32     m_dwGKIpAddr;                       //GK地址，网络序
    u16	    m_wGKPort;					        //GK端口,网络序
    char    m_achGKRegNum[16];                  //GK注册E164号码
	u16	    m_wListenPort;					    //服务器侦听端口,网络序
    u16     m_wTelnetPort;                      //服务器Telnet端口，网络序
    u16	    m_wT120Port;					    //服务器T120端口,网络序
    u16     m_wRasPort;                         //服务器RAS端口,网络序
    u16     m_wH323Port;                        //服务器H323端口,网络序
    char	m_achEmail[MAX_EMAIL_LENGTH];	    //服务器Email
    char	m_achLocation[MAX_NODELOCATION_LENGTH];	//服务器位置
    char	m_achPhoneNum[MAX_ALIAS_LENGTH];	//服务器电话号码
	
public:
/*====================================================================
    功能        ：构造函数
    输入参数说明：无
    返回值说明  ：无
====================================================================*/
	TDCServerInfo( void )
	{
		m_dwIpAddr = 0;
        memset( m_achAlias , 0, sizeof(m_achAlias) );
		//m_dwGWIpAddr = 0;
        m_dwGKIpAddr = 0;
        m_wGKPort = htons(1719);

        memset( m_achGKRegNum , 0, sizeof(m_achGKRegNum) );
        memcpy( m_achGKRegNum , "1234", sizeof(m_achGKRegNum) );
        
        m_wListenPort = htons(PORT_DCS);
        m_wTelnetPort = htons(2600);
        m_wT120Port = htons(DCS_DEFAULT_PORT);
        memset( m_achEmail , 0, sizeof(m_achEmail) );
        memset( m_achLocation , 0, sizeof(m_achLocation) );
        memset( m_achPhoneNum , 0, sizeof(m_achPhoneNum) );
	}
	
/*====================================================================
    功能        ：得到IP地址
    输入参数说明：无
    返回值说明  ：得到的Ip地址
====================================================================*/
	u32	GetIpAddr( void ) const
	{
		return ntohl(m_dwIpAddr);
	}

/*====================================================================
    功能        ：设置服务器地址
    输入参数说明：dwIpAddr－－Ip地址
    返回值说明  ：无
====================================================================*/
	void SetIpAddr( u32 dwIpAddr )
	{
		m_dwIpAddr = htonl(dwIpAddr);
	}
	
/*====================================================================
    功能        ：得到服务器别名
    输入参数说明：无
    返回值说明  ：得到的服务器别名
====================================================================*/
	LPCSTR GetAlias( void ) const
	{
		return m_achAlias;
	}
    
/*====================================================================
    功能        ：设置服务器别名
    输入参数说明：lpszAlias－－服务器别名
    返回值说明  ：无
====================================================================*/
    void SetAlias( LPCSTR lpszAlias )
	{
        if (lpszAlias == NULL)
        {
		    m_achAlias[0] = '\0';
        }
        else
        {
		    strncpy( m_achAlias, lpszAlias, sizeof(m_achAlias) );
		    m_achAlias[sizeof(m_achAlias) - 1] = '\0';
        }
	}
	
/*====================================================================
    功能        ：得到服务器MCU1地址
    输入参数说明：无
    返回值说明  ：服务器MCU1地址
====================================================================*/
	u32	GetMcuIpAddr( void ) const
	{
		return ntohl(m_dwMcuIpAddr);
	}

/*====================================================================
    功能        ：得到服务器MCU2地址
    输入参数说明：无
    返回值说明  ：服务器MCU2地址
====================================================================*/
	u32	GetBackupMcuIpAddr( void ) const
	{
		return ntohl(m_dwBackupMcuIpAddr);
	}

/*====================================================================
    功能        ：设置服务器MCU1地址
    输入参数说明：dwMcuIpAddr－－MCU1 Ip地址
    返回值说明  ：无
====================================================================*/
	void SetMcuIpAddr( u32 dwMcuIpAddr )
	{
		m_dwMcuIpAddr = htonl(dwMcuIpAddr);
	}
    
/*====================================================================
    功能        ：设置服务器MCU2地址
    输入参数说明：dwMcuIpAddr－－MCU2 Ip地址
    返回值说明  ：无
====================================================================*/
	void SetBackupMcuIpAddr( u32 dwMcuIpAddr )
	{
		m_dwBackupMcuIpAddr = htonl(dwMcuIpAddr);
	}	


/*====================================================================
    功能        ：得到服务器MCU1端口
    输入参数说明：无
    返回值说明  ：服务器MCU端口
====================================================================*/
	u16	GetMcuPort( void ) const
	{
		return ntohs( m_wMcuPort );
	}

/*====================================================================
    功能        ：得到服务器MCU2端口
    输入参数说明：无
    返回值说明  ：服务器MCU端口
====================================================================*/
	u16	GetBackupMcuPort( void ) const
	{
		return ntohs( m_wBackupMcuPort );
	}

/*====================================================================
    功能        ：设置服务器MCU1端口
    输入参数说明：wMcuPort－－MCU端口
    返回值说明  ：无
====================================================================*/
	void SetMcuPort( u16 wMcuPort )
	{
		m_wMcuPort = htons( wMcuPort );
	}

/*====================================================================
    功能        ：设置服务器MCU2端口
    输入参数说明：wMcuPort－－MCU端口
    返回值说明  ：无
====================================================================*/
	void SetBackupMcuPort( u16 wMcuPort )
	{
		m_wBackupMcuPort = htons( wMcuPort );
	}

/*====================================================================
    功能        ：得到服务器GK地址
    输入参数说明：无
    返回值说明  ：服务器GK地址
====================================================================*/
	u32	GetGKIpAddr( void ) const
	{
		return ntohl(m_dwGKIpAddr);
	}

/*====================================================================
    功能        ：设置服务器GK地址
    输入参数说明：dwGKIpAddr－－GKIp地址
    返回值说明  ：无
====================================================================*/
	void SetGKIpAddr( u32 dwGKIpAddr )
	{
		m_dwGKIpAddr = htonl(dwGKIpAddr);
	}
    
/*====================================================================
    功能        ：得到服务器GK端口
    输入参数说明：无
    返回值说明  ：服务器GK端口
====================================================================*/
	u16	GetGKPort( void ) const
	{
		return ntohs( m_wGKPort );
	}

/*====================================================================
    功能        ：设置服务器GK端口
    输入参数说明：wGKPort－－GK端口
    返回值说明  ：无
====================================================================*/
	void SetGKPort( u16 wGKPort )
	{
		m_wGKPort = htons( wGKPort );
	}

/*====================================================================
    功能        ：得到GK注册号码
    输入参数说明：无
    返回值说明  ：GK注册号码
====================================================================*/
	LPCSTR GetGKRegNum( void ) const
	{
		return m_achGKRegNum;
	}
    
/*====================================================================
    功能        ：设置GK注册号码
    输入参数说明：lpszGKRegNum－－GK注册号码
    返回值说明  ：无
====================================================================*/
    void SetGKRegNum( LPCSTR lpszGKRegNum )
	{
        if (lpszGKRegNum == NULL)
        {
		    m_achGKRegNum[0] = '\0';
        }
        else
        {
		    strncpy( m_achGKRegNum, lpszGKRegNum, sizeof(m_achGKRegNum) );
		    m_achGKRegNum[sizeof(m_achGKRegNum) - 1] = '\0';
        }
	}

/*====================================================================
    功能        ：得到服务器侦听端口
    输入参数说明：无
    返回值说明  ：服务器侦听端口
====================================================================*/
	u16	GetListenPort( void ) const
	{
		return ntohs( m_wListenPort );
	}

/*====================================================================
    功能        ：设置服务器侦听端口
    输入参数说明：wListenPort－－侦听端口
    返回值说明  ：无
====================================================================*/
	void SetListenPort( u16 wListenPort )
	{
		m_wListenPort = htons( wListenPort );
	}

/*====================================================================
    功能        ：得到服务器Telnet端口
    输入参数说明：无
    返回值说明  ：服务器Telnet端口
====================================================================*/
	u16	GetTelnetPort( void ) const
	{
		return ntohs( m_wTelnetPort );
	}

/*====================================================================
    功能        ：设置服务器Telnet端口
    输入参数说明：wTelnetPort－－Telnet端口
    返回值说明  ：无
====================================================================*/
	void SetTelnetPort( u16 wTelnetPort )
	{
		m_wTelnetPort = htons( wTelnetPort );
	}

/*====================================================================
    功能        ：得到服务器T120端口
    输入参数说明：无
    返回值说明  ：服务器T120端口
====================================================================*/
	u16	GetT120Port( void ) const
	{
		return ntohs( m_wT120Port );
	}

/*====================================================================
    功能        ：设置服务器T120端口
    输入参数说明：wT120Port－－T120端口
    返回值说明  ：无
====================================================================*/
	void SetT120Port( u16 wT120Port )
	{
		m_wT120Port = htons( wT120Port );
	}

/*====================================================================
    功能        ：得到服务器RAS端口
    输入参数说明：无
    返回值说明  ：服务器RAS端口
====================================================================*/
	u16	GetRasPort( void ) const
	{
		return ntohs( m_wRasPort );
	}

/*====================================================================
    功能        ：设置服务器RAS端口
    输入参数说明：wRasPort－－RAS端口
    返回值说明  ：无
====================================================================*/
	void SetRasPort( u16 wRasPort )
	{
		m_wRasPort = htons( wRasPort );
	}

/*====================================================================
    功能        ：得到服务器H323端口
    输入参数说明：无
    返回值说明  ：服务器H323端口
====================================================================*/
	u16	GetH323Port( void ) const
	{
		return ntohs( m_wH323Port );
	}

/*====================================================================
    功能        ：设置服务器H323端口
    输入参数说明：wH323Port－－H323端口
    返回值说明  ：无
====================================================================*/
	void SetH323Port( u16 wH323Port )
	{
		m_wH323Port = htons( wH323Port );
	}

/*====================================================================
    功能        ：得到服务器Email
    输入参数说明：无
    返回值说明  ：得到的服务器Email
====================================================================*/
	LPCSTR GetEmail( void ) const
	{
		return m_achEmail;
	}
    
/*====================================================================
    功能        ：设置服务器Email
    输入参数说明：lpszEmail－－服务器Email
    返回值说明  ：无
====================================================================*/
    void SetEmail( LPCSTR lpszEmail )
	{
        if (lpszEmail == NULL)
        {
		    m_achEmail[0] = '\0';
        }
        else
        {
		    strncpy( m_achEmail, lpszEmail, sizeof(m_achEmail) );
		    m_achEmail[sizeof(m_achEmail) - 1] = '\0';
        }
	}

/*====================================================================
    功能        ：得到服务器位置
    输入参数说明：无
    返回值说明  ：得到的服务器位置
====================================================================*/
	LPCSTR GetLocation( void ) const
	{
		return m_achLocation;
	}
    
/*====================================================================
    功能        ：设置服务器位置
    输入参数说明：lpszLocation－－服务器位置
    返回值说明  ：无
====================================================================*/
    void SetLocation( LPCSTR lpszLocation )
	{
        if (lpszLocation == NULL)
        {
		    m_achLocation[0] = '\0';
        }
        else
        {
		    strncpy( m_achLocation, lpszLocation, sizeof(m_achLocation) );
		    m_achLocation[sizeof(m_achLocation) - 1] = '\0';
        }
	}

/*====================================================================
    功能        ：得到服务器PhoneNum
    输入参数说明：无
    返回值说明  ：得到的服务器PhoneNum
====================================================================*/
	LPCSTR GetPhoneNum( void ) const
	{
		return m_achPhoneNum;
	}
    
/*====================================================================
    功能        ：设置服务器PhoneNum
    输入参数说明：lpszPhoneNum－－服务器PhoneNum
    返回值说明  ：无
====================================================================*/
    void SetPhoneNum( LPCSTR lpszPhoneNum )
	{
        if (lpszPhoneNum == NULL)
        {
		    m_achPhoneNum[0] = '\0';
        }
        else
        {
		    strncpy( m_achPhoneNum, lpszPhoneNum, sizeof(m_achPhoneNum) );
		    m_achPhoneNum[sizeof(m_achPhoneNum) - 1] = '\0';
        }
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//服务器会议信息
struct TDCSConfInfo
{
protected:
    u8      m_byIsH323Call;                             //是否通过H323呼叫 默认为0
    char    m_achE164Num[MAX_E164NUM_LENGTH];           //服务器E164号码
	u32	    m_dwIpAddr;						            //服务器地址,网络序
    u16	    m_wT120Port;					            //服务器T120端口,网络序
    u8      m_byE164Valid;                              //默认为0
    char	m_achConfNameNum[MAX_CONFNAME_NUM_LENGTH];	//服务器会议名－数字段
    char	m_achConfNameTxt[MAX_CONFNAME_TXT_LENGTH];	//服务器会议名－文本段
    char	m_achConfPasswd[MAX_DC_PASSWORD_LENGTH];	//会议密码
	
public:
/*====================================================================
    功能        ：构造函数
    输入参数说明：无
    返回值说明  ：无
====================================================================*/
	TDCSConfInfo( void )
	{
        m_byIsH323Call = 0;
		m_dwIpAddr = 0;
        m_wT120Port = 0;
        m_byE164Valid = 0;
        memset( m_achE164Num , 0, sizeof(m_achE164Num) );
        memset( m_achConfNameNum , 0, sizeof(m_achConfNameNum) );
        memset( m_achConfNameTxt , 0, sizeof(m_achConfNameTxt) );
        memset( m_achConfPasswd , 0, sizeof(m_achConfPasswd) );
	}
	
/*====================================================================
    功能        ：得到是否通过H323呼叫
    输入参数说明：无
    返回值说明  ：是否通过H323呼叫
====================================================================*/
	bool IsH323Call(void) const
	{
        return m_byIsH323Call? true:false;
	}

/*====================================================================
    功能        ：设置是否通过H323呼叫
    输入参数说明：wT120Port－－是否通过H323呼叫
    返回值说明  ：无
====================================================================*/
	void SetH323Call( bool bH323Call )
	{
        m_byIsH323Call = u8(bH323Call? 1:0);
	}

/*====================================================================
    功能        ：得到服务器E164号码
    输入参数说明：无
    返回值说明  ：得到的服务器E164号码
====================================================================*/
	LPCSTR GetE164Num( void ) const
	{
		return m_achE164Num;
	}
    
/*====================================================================
    功能        ：设置服务器E164号码
    输入参数说明：lpszE164Num－－服务器E164号码
    返回值说明  ：无
====================================================================*/
    void SetE164Num( LPCSTR lpszE164Num )
	{
		strncpy( m_achE164Num, lpszE164Num, sizeof(m_achE164Num) );
		m_achE164Num[sizeof(m_achE164Num) - 1] = '\0';
	}

/*====================================================================
    功能        ：得到IP地址
    输入参数说明：无
    返回值说明  ：得到的Ip地址
====================================================================*/
	u32	GetIpAddr( void ) const
	{
		return ntohl(m_dwIpAddr);
	}

/*====================================================================
    功能        ：设置服务器地址
    输入参数说明：dwIpAddr－－Ip地址
    返回值说明  ：无
====================================================================*/
	void SetIpAddr( u32 dwIpAddr )
	{
		m_dwIpAddr = htonl(dwIpAddr);
	}
	
/*====================================================================
    功能        ：得到服务器T120端口
    输入参数说明：无
    返回值说明  ：服务器T120端口
====================================================================*/
	u16	GetT120Port( void ) const
	{
		return ntohs( m_wT120Port );
	}

/*====================================================================
    功能        ：设置服务器T120端口
    输入参数说明：wT120Port－－T120端口
    返回值说明  ：无
====================================================================*/
	void SetT120Port( u16 wT120Port )
	{
		m_wT120Port = htons( wT120Port );
	}

/*====================================================================
    功能        ：得到E164号码是否有效
    输入参数说明：无
    返回值说明  ：E164号码是否有效
====================================================================*/
	bool IsE164Valid(void) const
	{
        return m_byE164Valid? true:false;
	}

/*====================================================================
    功能        ：设置E164号码是否有效
    输入参数说明：wT120Port－－E164号码是否有效
    返回值说明  ：无
====================================================================*/
	void SetE164Valid( bool bE164Valid )
	{
        m_byE164Valid = (u8)(bE164Valid? 1:0);
	}

/*====================================================================
    功能        ：得到会议名数字段
    输入参数说明：无
    返回值说明  ：得到的会议名数字段
====================================================================*/
	LPCSTR GetConfNameNum( void ) const
	{
		return m_achConfNameNum;
	}
    
/*====================================================================
    功能        ：设置会议名数字段
    输入参数说明：lpszConfNameNum－－会议名数字段
    返回值说明  ：无
====================================================================*/
    void SetConfNameNum( LPCSTR lpszConfNameNum )
	{
		strncpy( m_achConfNameNum, lpszConfNameNum, sizeof(m_achConfNameNum) );
		m_achConfNameNum[sizeof(m_achConfNameNum) - 1] = '\0';
	}
    
/*====================================================================
    功能        ：得到会议名文本段
    输入参数说明：无
    返回值说明  ：得到的会议名文本段
====================================================================*/
	LPCSTR GetConfNameTxt( void ) const
	{
		return m_achConfNameTxt;
	}
    
/*====================================================================
    功能        ：设置会议名文本段
    输入参数说明：lpszConfNameTxt－－会议名文本段
    返回值说明  ：无
====================================================================*/
    void SetConfNameTxt( LPCSTR lpszConfNameTxt )
	{
		strncpy( m_achConfNameTxt, lpszConfNameTxt, sizeof(m_achConfNameTxt) );
		m_achConfNameTxt[sizeof(m_achConfNameTxt) - 1] = '\0';
	}

/*====================================================================
    功能        ：得到会议密码
    输入参数说明：无
    返回值说明  ：得到的会议密码
====================================================================*/
	LPCSTR GetConfPasswd( void ) const
	{
		return m_achConfPasswd;
	}
    
/*====================================================================
    功能        ：设置会议密码
    输入参数说明：lpszConfPasswd－－会议密码
    返回值说明  ：无
====================================================================*/
    void SetConfPasswd( LPCSTR lpszConfPasswd )
	{
		strncpy( m_achConfPasswd, lpszConfPasswd, sizeof(m_achConfPasswd) );
		m_achConfPasswd[sizeof(m_achConfPasswd) - 1] = '\0';
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//用户信息
struct TDCUser
{
protected:
	char	m_achUserName[MAX_USER_LENGTH];         //用户名
	char	m_achPassword[MAX_PASSWORD_LENGTH];		//用户密码
	u8      m_byLevel;                              //用户等级( 0:只读用户;1:可操作用户;2:超级用户 )
    u8      m_byOnline;                             //用户是否在线
public:
	TDCUser( void )
	{
		memset( m_achUserName , 0, sizeof(m_achUserName) );
		memset( m_achPassword , 0, sizeof(m_achPassword) );
		m_byLevel = 0;
		m_byOnline = 0;
	}
	
	~TDCUser()
	{
	}
	
/*====================================================================
    功能        ：得到用户名
    输入参数说明：无
    返回值说明  ：用户名
====================================================================*/
	LPCSTR GetUserName( void ) const
    {
        return m_achUserName;
    }

/*====================================================================
    功能        ：设置用户名
    输入参数说明：achUsername－－用户名
    返回值说明  ：无
====================================================================*/
	void SetUserName( LPCSTR achUsername )
    {
        strncpy( m_achUserName, achUsername, sizeof(m_achUserName) );
		m_achUserName[sizeof(m_achUserName) - 1] = '\0';
    }
	
/*====================================================================
    功能        ：得到用户密码
    输入参数说明：无
    返回值说明  ：用户密码
====================================================================*/
	LPCSTR GetPassword( void ) const
    {
        return m_achPassword;
    }

/*====================================================================
    功能        ：设置用户密码
    输入参数说明：achPassword－－用户密码
    返回值说明  ：无
====================================================================*/
	void SetPassword( LPCSTR achPassword )
    {
        strncpy( m_achPassword, achPassword, sizeof(m_achPassword) );
		m_achPassword[sizeof(m_achPassword) - 1] = '\0';
    }
	
/*====================================================================
    功能        ：得到用户等级
    输入参数说明：无
    返回值说明  ：用户等级
====================================================================*/
    u8 GetLevel(void) const
    {
        return m_byLevel;
    }

/*====================================================================
    功能        ：设置用户等级
    输入参数说明：level－－用户等级
    返回值说明  ：无
====================================================================*/
    void SetLevel(u8 level)
    {
        m_byLevel = level;
    }
	
/*====================================================================
    功能        ：得到用户是否在线
    输入参数说明：无
    返回值说明  ：  true－－在线
                    false－－不在线
====================================================================*/
    bool IsOnline(void) const
    {
        return m_byOnline? true:false;
    }

/*====================================================================
    功能        ：设置用户是否在线
    输入参数说明：bOnline－－是否在线
    返回值说明  ：无
====================================================================*/
    void SetOnline( bool bOnline = false )
    {
        m_byOnline = (u8)(bOnline? 1:0);
    }	

/*====================================================================
    功能        ：判断用户是否相等
    输入参数说明：tDcUser－－另一用户信息
    返回值说明  ：true－－相等
                  false－－不相等
====================================================================*/
	bool IsEqual(const TDCUser& tDcUser)
	{
        return (strcmp(GetUserName(), tDcUser.GetUserName())==0)? true:false;
	}
    
/*====================================================================
    功能        ：赋值操作符重载
    输入参数说明：TDCUser& tDcUser, 赋值的消息引用
    返回值说明  ：TDCUser对象引用
====================================================================*/
	TDCUser& operator=(const TDCUser& tDcUser) 
	{
		SetUserName(tDcUser.GetUserName());
		SetPassword(tDcUser.GetPassword());
		SetLevel(tDcUser.GetLevel());
		SetOnline(tDcUser.IsOnline());
		return *this;
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//终端简单信息
struct TDCBaseMt
{
private:
	char	m_achAlias[MAX_ALIAS_LENGTH];		//终端别名--索引
    u16     m_wMtId;                            //终端ID
    char    m_achE164Num[MAX_E164NUM_LENGTH];   //E164号码
	u32	    m_dwIpAddr;						    //终端地址,网络序
	u16	    m_wChannelPort;						//信令端口,网络序
    u8      m_byE164Valid;                      //E164是否有效(1:E164有效  0:Ip有效  默认为1)
   	char    m_achMtInfo[MAX_MT_INFO_LENGTH];    //备注信息
	
public:
	TDCBaseMt( void )
	{
		memset( m_achAlias , 0, sizeof(m_achAlias) );
        m_wMtId = 0;
        memset( m_achE164Num , 0, sizeof(m_achE164Num) );
		m_dwIpAddr = 0;
		m_wChannelPort = htons(DCS_DEFAULT_PORT);	
        m_byE164Valid = 1;
		memset( m_achMtInfo , 0, sizeof(m_achMtInfo) );
	}

/*====================================================================
    功能        ：设置终端别名
    输入参数说明：lpszAlias－－终端别名
    返回值说明  ：无
====================================================================*/   
	void SetAlias( LPCSTR lpszAlias )
	{
        if (lpszAlias == NULL)
        {
            m_achAlias[0] = '\0';
        }
        else
        {
		    strncpy( m_achAlias, lpszAlias, sizeof(m_achAlias) );
		    m_achAlias[sizeof(m_achAlias) - 1] = '\0';
        }
	}
    
/*====================================================================
    功能        ：得到终端别名
    输入参数说明：无
    返回值说明  ：终端别名
====================================================================*/	
	LPCSTR GetAlias( void ) const
	{
		return m_achAlias;
	}

/*====================================================================
    功能        ：得到终端ID
    输入参数说明：无
    返回值说明  ：终端ID
====================================================================*/	
	u16	GetMtId( void ) const
	{
		return ntohs( m_wMtId );
	}

/*====================================================================
    功能        ：设置终端ID
    输入参数说明：wMtId－－终端ID
    返回值说明  ：无
====================================================================*/   
	void SetMtId( u16 wMtId )
	{
		m_wMtId = htons( wMtId );
	}

/*====================================================================
    功能        ：设置E164号码
    输入参数说明：lpszE164Num－－E164号码
    返回值说明  ：无
====================================================================*/   
	void SetE164Num( LPCSTR lpszE164Num )
	{
        if (lpszE164Num == NULL)
        {
            m_achE164Num[0] = '\0';
        }
        else
        {
		    strncpy( m_achE164Num, lpszE164Num, sizeof(m_achE164Num) );
		    m_achE164Num[sizeof(m_achE164Num) - 1] = '\0';
        }
	}
    
/*====================================================================
    功能        ：得到E164号码
    输入参数说明：无
    返回值说明  ：E164号码
====================================================================*/	
	LPCSTR GetE164Num( void ) const
	{
		return m_achE164Num;
	}

/*====================================================================
    功能        ：得到终端地址
    输入参数说明：无
    返回值说明  ：终端地址
====================================================================*/	
	u32	GetIpAddr( void ) const
	{
		return ntohl(m_dwIpAddr);
	}

/*====================================================================
    功能        ：设置终端地址
    输入参数说明：dwIpAddr－－终端地址
    返回值说明  ：无
====================================================================*/   
	void SetIpAddr( u32 dwIpAddr )
	{
		m_dwIpAddr = htonl(dwIpAddr);
	}
	
/*====================================================================
    功能        ：得到信令端口
    输入参数说明：无
    返回值说明  ：信令端口
====================================================================*/	
	u16	GetChannelPort( void ) const
	{
		return ntohs( m_wChannelPort );
	}

/*====================================================================
    功能        ：设置信令端口
    输入参数说明：wChannelPort－－信令端口
    返回值说明  ：无
====================================================================*/   
	void SetChannelPort( u16 wChannelPort )
	{
		m_wChannelPort = htons( wChannelPort );
	}

/*====================================================================
    功能        ：设置E164号码是否有效
    输入参数说明：bE164Valid－－E164号码是否有效
    返回值说明  ：无
====================================================================*/   
	void SetE164Valid( bool bE164Valid )
	{
        m_byE164Valid = bE164Valid? 1:0;
	}

/*====================================================================
    功能        ：得到E164号码是否有效
    输入参数说明：无
    返回值说明  ：E164号码是否有效
====================================================================*/	
	bool IsE164Valid( void ) const
	{
        return m_byE164Valid? true:false;
	}
	
/*====================================================================
    功能        ：得到终端描述
    输入参数说明：无
    返回值说明  ：终端描述
====================================================================*/
	LPCSTR GetMtInfo( void ) const
	{
		return m_achMtInfo;
	}

/*====================================================================
    功能        ：设置终端描述
    输入参数说明：lpszMtInfo－－终端描述
    返回值说明  ：无
====================================================================*/
	void SetMtInfo( LPCSTR lpszMtInfo )
	{
        if (lpszMtInfo == NULL)
        {
		    m_achMtInfo[0] = '\0';
        }
        else
        {
		    strncpy( m_achMtInfo, lpszMtInfo, sizeof(m_achMtInfo) );
		    m_achMtInfo[sizeof(m_achMtInfo) - 1] = '\0';
        }
	}

/*====================================================================
    功能        ：判断终端是否相等
    输入参数说明：tDCBaseMt－－另一终端信息
    返回值说明  ：  true－－相等
                    false－－不相等
====================================================================*/
	bool IsEqual(const TDCBaseMt& tDCBaseMt)
	{
        return (strcmp(GetAlias(), tDCBaseMt.GetAlias())==0)? true:false;
	}

/*====================================================================
    功能        ：赋值操作符重载
    输入参数说明：TDCBaseMt& tDCBaseMt, 赋值的消息引用
    返回值说明  ：TDCBaseMt对象引用
====================================================================*/
	TDCBaseMt& operator=(const TDCBaseMt& tDCBaseMt) 
	{
		SetAlias(tDCBaseMt.GetAlias());
        SetE164Num(tDCBaseMt.GetE164Num());
		SetIpAddr(tDCBaseMt.GetIpAddr());
		SetChannelPort(tDCBaseMt.GetChannelPort());
        SetE164Valid(tDCBaseMt.IsE164Valid());
        SetMtInfo( tDCBaseMt.GetMtInfo() );
		return *this;
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//终端详细信息
struct TDCMt : public TDCBaseMt
{
protected:
    u16     m_wGKCallingPort;                           //GK呼叫端口
    enum_ConfMtType m_enumConfMtType;                   //会议中的终端类型
    enum_DeviceType m_enumDeviceType;                   //会议中的终端设备类型
	char	m_achEmail[MAX_EMAIL_LENGTH];			    //终端Email地址
	u8		m_byOnline;								    //终端是否与会
    u16     m_byChildCount;                             //直接子节点个数
    char    m_achNodeName[MAX_NODENAME_LENGTH];         //节点名
    char    m_achNodePhone[MAX_NODEPHONE_LENGTH];       //电话号码
    char    m_achNodeLocation[MAX_NODELOCATION_LENGTH]; //节点位置
public:
	TDCMt( void )
	{
        m_wGKCallingPort = htons(1720);
        m_enumConfMtType = enum_MtInvited;
        m_enumDeviceType = enum_Mt;
		memset( m_achEmail , 0, sizeof(m_achEmail) );
		memset( m_achNodeName , 0, sizeof(m_achNodeName) );
		memset( m_achNodePhone , 0, sizeof(m_achNodePhone) );
		memset( m_achNodeLocation , 0, sizeof(m_achNodeLocation) );
		m_byOnline = 0;
        m_byChildCount = 0;
	}

/*====================================================================
    功能        ：得到GK 呼叫端口
    输入参数说明：无
    返回值说明  ：GK 呼叫端口
====================================================================*/	
    u16	GetGKCallingPort( void ) const
    {
        return ntohs( m_wGKCallingPort );
    }
    
/*====================================================================
    功能        ：设置GK 呼叫端口
    输入参数说明：wGKCallingPort－－GK 呼叫端口
    返回值说明  ：无
====================================================================*/   
    void SetGKCallingPort( u16 wGKCallingPort )
    {
        m_wGKCallingPort = htons( wGKCallingPort );
	}

/*====================================================================
    功能        ：得到会议终端类型
    输入参数说明：无
    返回值说明  ：会议终端类型
====================================================================*/	
	enum_ConfMtType GetConfMtType( void ) const
	{
		return (enum_ConfMtType)ntohl(m_enumConfMtType);
	}

/*====================================================================
    功能        ：设置会议终端类型
    输入参数说明：byConfMtType－－会议终端类型
    返回值说明  ：无
====================================================================*/   
	void SetConfMtType( enum_ConfMtType enumConfMtType )
	{
		m_enumConfMtType = (enum_ConfMtType)htonl(enumConfMtType);
	}

/*====================================================================
    功能        ：得到终端设备类型
    输入参数说明：无
    返回值说明  ：终端设备类型
====================================================================*/	
	enum_DeviceType GetDeviceType( void ) const
	{
		return (enum_DeviceType)ntohl(m_enumDeviceType);
	}

/*====================================================================
    功能        ：设置终端设备类型
    输入参数说明：byConfMtType－－终端设备类型
    返回值说明  ：无
====================================================================*/   
	void SetDeviceType( enum_DeviceType enumDeviceType )
	{
		m_enumDeviceType = (enum_DeviceType)htonl(enumDeviceType);
	}

/*====================================================================
    功能        ：得到终端是否在线
    输入参数说明：无
    返回值说明  ：  true－－在线
                    false－－不在线
====================================================================*/
	bool IsOnline( void ) const
	{
		return m_byOnline ? true : false;
	}

/*====================================================================
    功能        ：设置终端是否在线
    输入参数说明：bOnline－－终端是否在线
    返回值说明  ：无
====================================================================*/
	void SetOnline( bool bOnline = false )
	{
		m_byOnline = (u8)(bOnline? 1:0);
	}

/*====================================================================
    功能        ：得到终端的直接子节点个数
    输入参数说明：无
    返回值说明  ： 直接子节点个数
====================================================================*/
	u16 GetChildCount( void ) const
	{
		return ntohs(m_byChildCount);
	}

/*====================================================================
    功能        ：设置终端的直接子节点个数
    输入参数说明：直接子节点个数
    返回值说明  ：无
====================================================================*/
	void SetChildCount( u16 nChildCount )
	{
		m_byChildCount = htons(nChildCount);
	}

/*====================================================================
    功能        ：得到终端Email地址
    输入参数说明：无
    返回值说明  ：终端Email地址
====================================================================*/
	LPCSTR GetEmail( void ) const
	{
		return m_achEmail;
	}

/*====================================================================
    功能        ：设置终端Email地址
    输入参数说明：lpszEmail－－终端Email地址
    返回值说明  ：无
====================================================================*/
	void SetEmail( LPCSTR lpszEmail )
	{
        if (lpszEmail == NULL)
        {
            memset(m_achEmail, 0, sizeof(m_achEmail));
        }
        else
        {
            if (lpszEmail[0] != 0)
            {
                memcpy( m_achEmail, lpszEmail, sizeof(m_achEmail) );
                m_achEmail[sizeof(m_achEmail) - 1] = '\0';
            }
            else
            {
                memset(m_achEmail, 0, sizeof(m_achEmail));
            }
        }
	}
	
/*====================================================================
    功能        ：得到节点名
    输入参数说明：无
    返回值说明  ：节点名
====================================================================*/
    LPCSTR GetNodeName( void ) const
    {
        return m_achNodeName;
    }
	
/*====================================================================
    功能        ：设置节点名
    输入参数说明：lpszNodeName－－节点名
    返回值说明  ：无
====================================================================*/
    void SetNodeName( LPCSTR lpszNodeName )
    {
        if (lpszNodeName == NULL)
        {
            memset(m_achNodeName, 0, sizeof(m_achNodeName));
        }
        else
        {
            if(lpszNodeName[0] != 0)
            {
                memcpy( m_achNodeName, lpszNodeName, sizeof(m_achNodeName) );
		        m_achNodeName[sizeof(m_achNodeName) - 1] = '\0';
            }
            else
            {
                memset(m_achNodeName, 0, sizeof(m_achNodeName));
            }
        }
    }
	
/*====================================================================
    功能        ：得到电话号码
    输入参数说明：无
    返回值说明  ：电话号码
====================================================================*/
    LPCSTR GetNodePhone( void ) const
    {
        return m_achNodePhone;
    }
	
/*====================================================================
    功能        ：设置电话号码
    输入参数说明：lpszNodePhone－－电话号码
    返回值说明  ：无
====================================================================*/
    void SetNodePhone( LPCSTR lpszNodePhone )
    {
        if (lpszNodePhone == NULL)
        {
            memset(m_achNodePhone, 0, sizeof(m_achNodePhone));
        }
        else
        {
            if(lpszNodePhone[0] != 0)
            {
                memcpy( m_achNodePhone, lpszNodePhone, sizeof(m_achNodePhone) );
                m_achNodePhone[sizeof(m_achNodePhone) - 1] = '\0';
            }
            else
            {
                memset(m_achNodePhone, 0, sizeof(m_achNodePhone));
            }
        }
    }
	
/*====================================================================
    功能        ：得到节点位置
    输入参数说明：无
    返回值说明  ：节点位置
====================================================================*/
    LPCSTR GetNodeLocation( void ) const
    {
        return m_achNodeLocation;
    }
	
/*====================================================================
    功能        ：设置节点位置
    输入参数说明：lpszNodeLocation－－节点位置
    返回值说明  ：无
====================================================================*/
    void SetNodeLocation( LPCSTR lpszNodeLocation )
    {
        if (lpszNodeLocation == NULL)
        {
            memset(m_achNodeLocation, 0, sizeof(m_achNodeLocation));
        }
        else
        {
            if(lpszNodeLocation[0] != 0)
            {
                memcpy( m_achNodeLocation, lpszNodeLocation, sizeof(m_achNodeLocation) );
		        m_achNodeLocation[sizeof(m_achNodeLocation) - 1] = '\0';
            }
            else
            {
                memset(m_achNodeLocation, 0, sizeof(m_achNodeLocation));
            }
        }
    }

/*====================================================================
    功能        ：判断终端是否相等
    输入参数说明：tMt－－另一终端信息
    返回值说明  ：true－－相等
                  false－－不相等
====================================================================*/
	bool IsEqual(const TDCMt& tMt)
	{
        return ( (strcmp(GetAlias(), tMt.GetAlias())==0) &&
                (GetConfMtType()==tMt.GetConfMtType()) )? true:false;
	}

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//会议信息
struct TDataConfInfo
{
private:
	CConfId	m_cConfId;						            //会议号
	char	m_achConfName[MAX_DC_CONFNAME_LENGTH];		//会议名称
	char	m_achConfPassword[MAX_DC_PASSWORD_LENGTH];	//会议密码
	
	u8	    m_byConfProperty;   //会议属性参数,共8位,各位意义如下:(从低到高)
	                            //第1位:是否接受终端主动呼入    (0:不支持,1:支持)
	                            //第2位:是否自动邀请断线终端    (0:不支持,1:支持)
	                            //第3位:是否locked              (0:否    ,1:是  )
	                            //第4位:会议终止类型            (0:auto  ,1:manual)
	                            //第5位:会议是否支持主席        (0:不支持,1:支持)
	                            //第6位:会议是否有密码          (0:没有  ,1:有)
	
	u8	    m_byConfAppPara;    //会议应用参数,共8位,各位意义如下:(从低到高)
	                            //第1位:是否支持应用程序共享    (0:不支持,1:支持)
	                            //第2位:是否支持电子白板        (0:不支持,1:支持)
	                            //第3位:是否支持文件传输        (0:不支持,1:支持)
	                            //第4位:是否支持聊天            (0:不支持,1:支持)
	
	TDCMt	m_tChairman;	    //会议主席终端
	u32	    m_dwBandwidth;		//会议最高带宽限制
	u16	    m_wDuration;		//会议持续时间( 分钟为单位,0表示不自动结束 ),网络序

    u8      m_byIsLocalConf;    //是否为本地会议(0:不是   1:是  默认为1)
    u8      m_byIsH323Call;     //是否通过H323呼叫(0:不是 1：是 默认为0);
	
public:
	TDataConfInfo( void )
	{
		memset( m_achConfName , 0, sizeof(m_achConfName) );
		memset( m_achConfPassword , 0, sizeof(m_achConfPassword) );
		m_byConfAppPara = 0;
		m_byConfProperty = 0;
		m_dwBandwidth = htonl(1024);	
		m_wDuration = 0;
        m_byIsLocalConf = 1;
        m_byIsH323Call = 0;
	}
	
/*====================================================================
    功能        ：得到会议号
    输入参数说明：无
    返回值说明  ：会议号
====================================================================*/
	const CConfId & GetConfId( void ) const
	{
		return m_cConfId;
	}
	
/*====================================================================
    功能        ：设置会议号
    输入参数说明：cConfId－－会议号
    返回值说明  ：无
====================================================================*/
	void SetConfId( const CConfId & cConfId )
	{
		m_cConfId = cConfId;
	}
	
/*====================================================================
    功能        ：得到会议名
    输入参数说明：无
    返回值说明  ：会议名
====================================================================*/
	LPCSTR GetConfName( void ) const
	{
		return m_achConfName;
	}

/*====================================================================
    功能        ：设置会议名
    输入参数说明：lpszConfName－－会议名
    返回值说明  ：无
====================================================================*/
	void SetConfName( LPCSTR lpszConfName )
	{
        if (lpszConfName == NULL)
        {
            m_achConfName[0] = '\0';
        }
        else
        {
		    strncpy( m_achConfName, lpszConfName, sizeof(m_achConfName) );
		    m_achConfName[sizeof(m_achConfName) - 1] = '\0';
        }
	}
	
/*====================================================================
    功能        ：会议属性参数
    输入参数说明：无
    返回值说明  ：会议属性参数
====================================================================*/
    u8 GetConfProperty() const
    {
        return m_byConfProperty;
    }
    
/*====================================================================
    功能        ：设置会议属性参数
    输入参数说明：byConfProperty－－会议属性参数
    返回值说明  ：无
====================================================================*/
    void SetConfProperty( u8 byConfProperty )
    {
        m_byConfProperty = byConfProperty;
    }
	
/*====================================================================
    功能        ：得到会议应用参数
    输入参数说明：无
    返回值说明  ：会议应用参数
====================================================================*/
    u8 GetConfAppPara() const
    {
        return m_byConfAppPara;
    }
    
/*====================================================================
    功能        ：设置会议应用参数
    输入参数说明：byConfPara－－会议应用参数
    返回值说明  ：无
====================================================================*/
    void SetConfAppPara( u8 byConfPara )
    {
        m_byConfAppPara = byConfPara;
    }

/*====================================================================
    功能        ：得到会议是否支持应用共享
    输入参数说明：无
    返回值说明  ：  true－－支持
                    false－－不支持
====================================================================*/
	bool IsEnableAppSharing( void ) const
	{
		return ( (m_byConfAppPara & CONF_ENABLE_APPSHARING) == CONF_ENABLE_APPSHARING ) ? true : false;
	}

/*====================================================================
    功能        ：设置会议是否支持应用共享
    输入参数说明：bEnable－－会议是否支持应用共享
    返回值说明  ：无
====================================================================*/
	void SetEnableAppSharing( bool bEnable = true )
	{
		if(bEnable)
			m_byConfAppPara |= CONF_ENABLE_APPSHARING;
		else
			m_byConfAppPara &= ~CONF_ENABLE_APPSHARING;
	}
	
/*====================================================================
    功能        ：得到会议是否支持电子白板
    输入参数说明：无
    返回值说明  ：  true－－支持
                    false－－不支持
====================================================================*/
	bool IsEnableWhiteboard( void ) const
	{
		return ((m_byConfAppPara & CONF_ENABLE_WHITEBOARD) == CONF_ENABLE_WHITEBOARD) ? true : false;
	}

/*====================================================================
    功能        ：设置会议是否支持电子白板
    输入参数说明：bEnable－－会议是否支持电子白板
    返回值说明  ：无
====================================================================*/
	void SetEnableWhiteboard( bool bEnable = true )
	{
		if(bEnable)
			m_byConfAppPara |= CONF_ENABLE_WHITEBOARD;
		else
			m_byConfAppPara &= ~CONF_ENABLE_WHITEBOARD;
	}
	
/*====================================================================
    功能        ：得到会议是否支持文件传输
    输入参数说明：无
    返回值说明  ：  true－－支持
                    false－－不支持
====================================================================*/
	bool IsEnableFileTransfer( void ) const
	{
		return ((m_byConfAppPara & CONF_ENABLE_FILETRANSFER) == CONF_ENABLE_FILETRANSFER) ? true : false;
	}

/*====================================================================
    功能        ：设置会议是否支持文件传输
    输入参数说明：bEnable－－会议是否支持文件传输
    返回值说明  ：无
====================================================================*/
	void SetEnableFileTransfer( bool bEnable = true )
	{
		if(bEnable)
			m_byConfAppPara |= CONF_ENABLE_FILETRANSFER;
		else
			m_byConfAppPara &= ~CONF_ENABLE_FILETRANSFER;
	}
	
/*====================================================================
    功能        ：得到会议是否支持聊天
    输入参数说明：无
    返回值说明  ：  true－－支持
                    false－－不支持
====================================================================*/
	bool IsEnableChat( void ) const
	{
		return ((m_byConfAppPara & CONF_ENABLE_CHAT) == CONF_ENABLE_CHAT) ? true : false;
	}

/*====================================================================
    功能        ：设置会议是否支持聊天
    输入参数说明：bEnable－－会议是否支持聊天
    返回值说明  ：无
====================================================================*/
	void SetEnableChat( bool bEnable = true )
	{
		if(bEnable)
			m_byConfAppPara |= CONF_ENABLE_CHAT;
		else
			m_byConfAppPara &= ~CONF_ENABLE_CHAT;
	}
	
/*====================================================================
    功能        ：得到会议是否支持终端主动呼入
    输入参数说明：无
    返回值说明  ：  true－－支持
                    false－－不支持
====================================================================*/
	bool IsAcceptMtCall( void ) const
	{
		return ((m_byConfProperty & CONF_ENABLE_ACCEPTMTCALL) == CONF_ENABLE_ACCEPTMTCALL) ? true : false;
	}

/*====================================================================
    功能        ：设置会议是否支持终端主动呼入
    输入参数说明：bEnable－－会议是否支持终端主动呼入
    返回值说明  ：无
====================================================================*/
	void SetAcceptMtCall( bool bEnable = true )
	{
		if(bEnable)
			m_byConfProperty |= CONF_ENABLE_ACCEPTMTCALL;
		else
			m_byConfProperty &= ~CONF_ENABLE_ACCEPTMTCALL;
	}
	
/*====================================================================
    功能        ：得到会议是否支持自动重邀断线终端
    输入参数说明：无
    返回值说明  ：true－－支持
                  false－－不支持
====================================================================*/
	bool IsAutoInviteMt( void ) const
	{
		return ((m_byConfProperty & CONF_ENABLE_AUTOINVITEMT) == CONF_ENABLE_AUTOINVITEMT) ? true : false;
	}

/*====================================================================
    功能        ：设置会议是否支持自动重邀断线终端
    输入参数说明：bEnable－－会议是否支持自动重邀断线终端
    返回值说明  ：无
====================================================================*/
	void SetAutoInviteMt( bool bEnable = true )
	{
		if(bEnable)
			m_byConfProperty |= CONF_ENABLE_AUTOINVITEMT;
		else
			m_byConfProperty &= ~CONF_ENABLE_AUTOINVITEMT;
	}
	
/*====================================================================
    功能        ：得到会议是否锁定
    输入参数说明：无
    返回值说明  ：  true－－锁定
                    false－－不锁定
====================================================================*/
	bool IsConfLocked( void ) const
	{
		return ((m_byConfProperty & CONF_ENABLE_LOCKED) == CONF_ENABLE_LOCKED) ? true : false;
	}

/*====================================================================
    功能        ：设置会议是否锁定
    输入参数说明：bEnable－－会议是否锁定
    返回值说明  ：无
====================================================================*/
	void SetConfLocked( bool bEnable = true )
	{
		if(bEnable)
			m_byConfProperty |= CONF_ENABLE_LOCKED;
		else
			m_byConfProperty &= ~CONF_ENABLE_LOCKED;
	}
	
/*====================================================================
    功能        ：得到会议是否手动终止
    输入参数说明：无
    返回值说明  ：  true－－手动终止
                    false－－不手动终止
====================================================================*/
	bool IsConfTmManual( void ) const
	{
		return ((m_byConfProperty & CONF_ENABLE_TMMANUAL) == CONF_ENABLE_TMMANUAL) ? true : false;
	}

/*====================================================================
    功能        ：设置会议是否手动终止
    输入参数说明：bEnable－－会议是否手动终止
    返回值说明  ：无
====================================================================*/
	void SetConfTmManual( bool bEnable = true )
	{
		if(bEnable)
			m_byConfProperty |= CONF_ENABLE_TMMANUAL;
		else
			m_byConfProperty &= ~CONF_ENABLE_TMMANUAL;
	}
	
/*====================================================================
    功能        ：得到会议是否支持主席
    输入参数说明：无
    返回值说明  ：  true－－支持
                    false－－不支持
====================================================================*/
	bool IsConfSupportChairman( void ) const
	{
		return ((m_byConfProperty & CONF_SUPPORT_CHAIRMAN) == CONF_SUPPORT_CHAIRMAN) ? true : false;
	}

/*====================================================================
    功能        ：设置会议是否支持主席
    输入参数说明：bEnable－－会议是否支持主席
    返回值说明  ：无
====================================================================*/
	void SetConfSupportChairman( bool bEnable = true )
	{
		if(bEnable)
			m_byConfProperty |= CONF_SUPPORT_CHAIRMAN;
		else
			m_byConfProperty &= ~CONF_SUPPORT_CHAIRMAN;
	}
	
/*====================================================================
    功能        ：得到会议是否需要密码才能加入
    输入参数说明：无
    返回值说明  ：  true－－需要
                    false－－不需要
====================================================================*/
	bool IsNeedPassword( void ) const
	{
        return ((m_byConfProperty & CONF_NEED_PASSWORD) == CONF_NEED_PASSWORD) ? true : false;
	}

/*====================================================================
    功能        ：设置会议是否需要密码才能加入
    输入参数说明：bEnable－－会议是否需要密码才能加入
    返回值说明  ：无
====================================================================*/
	void SetNeedPassword( bool bEnable = true )
	{
		if(bEnable)
			m_byConfProperty |= CONF_NEED_PASSWORD;
		else
			m_byConfProperty &= ~CONF_NEED_PASSWORD;
	}
	
/*====================================================================
    功能        ：得到会议密码
    输入参数说明：无
    返回值说明  ：会议密码
====================================================================*/
	LPCSTR GetConfPassword( void ) const
	{
		return m_achConfPassword;
	}

/*====================================================================
    功能        ：设置会议密码
    输入参数说明：lpszConfPassword－－会议密码
    返回值说明  ：无
====================================================================*/
	void SetConfPassword( LPCSTR lpszConfPassword )
	{
        if (lpszConfPassword == NULL)
        {
		    m_achConfPassword[0] = '\0';
        }
        else
        {
		    strncpy( m_achConfPassword, lpszConfPassword, sizeof(m_achConfPassword) );
		    m_achConfPassword[sizeof(m_achConfPassword) - 1] = '\0';
        }
	}
	
/*====================================================================
    功能        ：得到主席终端信息
    输入参数说明：无
    返回值说明  ：主席终端信息
====================================================================*/
	TDCMt & GetChairman( void ) const
	{
		return (TDCMt&)m_tChairman;
	}
	
/*====================================================================
    功能        ：设置会议主席终端
    输入参数说明：tChairman－－会议主席终端
    返回值说明  ：无
====================================================================*/
	void SetChairman( const TDCMt& tChairman )
	{
		m_tChairman = tChairman;
	}
	
/*====================================================================
    功能        ：得到会议最大带宽限制
    输入参数说明：无
    返回值说明  ：会议最大带宽限制
====================================================================*/
	u32	GetBandwidth( void ) const
	{
		return ntohl( m_dwBandwidth );
	}

/*====================================================================
    功能        ：设置会议最大带宽限制
    输入参数说明：dwBandwidth－－会议最大带宽限制
    返回值说明  ：无
====================================================================*/
	void SetBandwidth( u32 dwBandwidth )
	{
		m_dwBandwidth = htonl( dwBandwidth );
	}
	
/*====================================================================
    功能        ：得到会议持续时间
    输入参数说明：无
    返回值说明  ：会议持续时间
====================================================================*/
	u16	GetDuration( void ) const
	{
		return ntohs( m_wDuration );
	}

/*====================================================================
    功能        ：设置会议持续时间
    输入参数说明：wDuration－－会议持续时间
    返回值说明  ：无
====================================================================*/
	void SetDuration( u16 wDuration )
	{
		m_wDuration = htons( wDuration );
	}

/*====================================================================
    功能：        是否为本地会议
    输入参数说明：无
    返回值说明  ：true－－是
                  false－－否
====================================================================*/
    bool IsLocalConf() const
    {
        return m_byIsLocalConf? true:false;
    }
    
/*====================================================================
    功能        ：设置是否为本地会议
    输入参数说明：bIsLocalConf－－是否为本地会议
    返回值说明  ：无
====================================================================*/
    void SetLocalConf( bool bIsLocalConf )
    {
        m_byIsLocalConf = (u8)(bIsLocalConf? 1:0);
    }

/*====================================================================
    功能：        是否通过H323呼叫
    输入参数说明：无
    返回值说明  ：true－－是
                  false－－否
====================================================================*/
    bool IsH323Call() const
    {
        return m_byIsH323Call? true:false;
    }
    
/*====================================================================
    功能        ：设置是否通过H323呼叫
    输入参数说明：bIsH323Call－－是否通过H323呼叫
    返回值说明  ：无
====================================================================*/
    void SetH323Call( bool bIsH323Call )
    {
        m_byIsH323Call = (u8)(bIsH323Call? 1:0);
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//会议详细信息
struct TDataConfFullInfo : public TDataConfInfo
{
protected:
	u8	    m_byMtCount;					//当前会议受邀终端总数
	TDCMt	m_tDCMt[MAX_MT_IN_DATACONF];	//数据会议终端列表
public:
	TDataConfFullInfo( void )
	{
		m_byMtCount = 0;
        memset( m_tDCMt, 0, sizeof(m_tDCMt) );
	}
/*====================================================================
    功能        ：得到会议终端总数
    输入参数说明：无
    返回值说明  ：会议终端总数
====================================================================*/
	u8 GetMtCount( void ) const
	{
		return m_byMtCount;
	}

/*====================================================================
    功能        ：设置会议终端总数
    输入参数说明：byMtCount－－会议终端总数
    返回值说明  ：无
====================================================================*/
    void SetMtCount( u8 byMtCount )
    {
        m_byMtCount = byMtCount;
    }

/*====================================================================
    功能        ：得到会议中终端
    输入参数说明：byIndex－－终端索引
    返回值说明  ：终端指针
====================================================================*/
	const TDCMt* GetTDCMt( u8 byIndex ) const
	{
		if( byIndex <= (m_byMtCount-1) )
		{
		    return &(m_tDCMt[byIndex]);
		}
		else
		{
			return NULL;
		}
	}

/*====================================================================
    功能        ：得到会议中终端
    输入参数说明：byIndex－－终端索引
    返回值说明  ：终端指针
====================================================================*/
	const TDCMt* GetTDCMt( const TDCMt& tDCMt )
	{
        for( int i=0 ; i<m_byMtCount ; i++ )
        {
            if( m_tDCMt[i].IsEqual(tDCMt) )
                return &(m_tDCMt[i]);
        }
        return NULL;
	}
	
/*====================================================================
    功能        ：  得到会议中所有终端
    输入参数说明：  ptDCMtBuf－－缓冲区指针
                    byBufsize－－终端数量
    返回值说明  ：  实际终端数量
====================================================================*/
	u8 GetAllMt( TDCMt* ptDCMtBuf , u8 byBufsize )
	{
		byBufsize = (byBufsize<m_byMtCount) ? byBufsize : m_byMtCount;
		
		memcpy( ptDCMtBuf, m_tDCMt, sizeof(TDCMt)*byBufsize );
		return byBufsize;
	}
	
/*====================================================================
    功能        ：  在会议中加入终端
    输入参数说明：  tDCMt－－终端信息
    返回值说明  ：  true－－成功
                    false－－失败
====================================================================*/
	bool AddMt( const TDCMt & tDCMt )
	{
		if( m_byMtCount >= MAX_MT_IN_DATACONF )
			return false;
		else
		{
			m_tDCMt[m_byMtCount] = tDCMt;
			m_byMtCount++;
			return true;
		}
	}
	
/*====================================================================
    功能        ：  在会议中退出终端
    输入参数说明：  tDCMt－－终端信息
    返回值说明  ：  true－－成功
                    false－－失败
====================================================================*/
	void RemoveMt( const TDCMt & tDCMt )
	{
        if(GetChairman().IsEqual(tDCMt))
        {
            TDCMt tMt;
            SetChairman(tMt);
        }
        
		for( int i = 0 ; i < m_byMtCount ; i++ )
		{
			if( m_tDCMt[i].IsEqual(tDCMt) )
			{
				for( int j = i + 1; j < m_byMtCount; j++ )
				{
					m_tDCMt[j-1] = m_tDCMt[j];
				}
				m_byMtCount--;
				return;
			}	
        }
	}
	
/*====================================================================
    功能        ：  在会议中退出所有终端
    输入参数说明：  无
    返回值说明  ：  true－－成功
                    false－－失败
====================================================================*/
	void RemoveAllMt( void )
	{
		m_byMtCount = 0;
        memset( m_tDCMt, 0, sizeof(m_tDCMt) );
	}
	
/*====================================================================
    功能        ：  判断终端是否受邀
    输入参数说明：  tMt－－终端信息
    返回值说明  ：  true－－受邀
                    false－－不受邀
====================================================================*/
	bool MtInConf( const TDCMt & tMt )
	{
		for( int i = 0; i < m_byMtCount; i++ )
		{
			if( m_tDCMt[i].IsEqual(tMt) )
			{
				return true;
			}
		}
		return false;
	}
    
/*====================================================================
    功能        ：  判断终端是否在线
    输入参数说明：  tMt－－终端信息
    返回值说明  ：  true－－在线
                    false－－不在线
====================================================================*/
    bool MtJoinedConf( const TDCMt & tMt )
    {
        for( int i = 0; i < m_byMtCount; i++ )
        {
            if( m_tDCMt[i].IsEqual(tMt) )
            {
                return (m_tDCMt[i].IsOnline());
            }
        }
        return false;
    }
	
/*====================================================================
    功能        ：  修改终端状态
    输入参数说明：  tMt－－终端信息
    返回值说明  ：  无
====================================================================*/
	void ModifyMt( const TDCMt & tDCMt )
	{
		for( int i = 0; i < m_byMtCount; i++ )
		{
			if( m_tDCMt[i].IsEqual(tDCMt) )
			{
				m_tDCMt[i] = tDCMt;
				return;
			}
		}
	}

/*====================================================================
    功能        ：  设置所有终端的上线或下线状态
    输入参数说明：  bOnline－－上线或下线
    返回值说明  ：  无
====================================================================*/
	void SetAllMtsOnline(bool bOnline)
	{
		for( int i = 0; i < m_byMtCount; i++ )
		{
			m_tDCMt[i].SetOnline(bOnline);
		}
	}
	
/*====================================================================
    功能        ：赋值操作符重载
    输入参数说明：TDataConfFullInfo& tDcConf, 赋值的消息引用
    返回值说明  ：TDataConfFullInfo对象引用
====================================================================*/
	TDataConfFullInfo& operator=(const TDataConfFullInfo& tDCConf) 
	{
		m_byMtCount = tDCConf.GetMtCount();
		for(u8 i = 0; i < GetMtCount(); i++)
		{
			const TDCMt *pMt = tDCConf.GetTDCMt(i);
			if( pMt!=NULL )
			{
				m_tDCMt[i] = *pMt;
			}
		}
		
		SetConfId(tDCConf.GetConfId());
		SetConfName(tDCConf.GetConfName());
		SetConfPassword(tDCConf.GetConfPassword());
		
		SetConfAppPara(tDCConf.GetConfAppPara());
		SetConfProperty(tDCConf.GetConfProperty());
		SetChairman(tDCConf.GetChairman());
		SetBandwidth(tDCConf.GetBandwidth());
		SetDuration(tDCConf.GetDuration());
        SetLocalConf(tDCConf.IsLocalConf());
        SetH323Call(tDCConf.IsH323Call());
		return *this;
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//模板信息
struct TDataConfTemplate : public TDataConfInfo
{
protected:
	CConfId	m_cConfTemplateId;						        //模板号
	char	m_achConfTemplateName[MAX_DC_CONFNAME_LENGTH];	//模板名

    u8	        m_byBaseMtCount;					    //当前模板受邀终端总数
    TDCBaseMt	m_tDCBaseMt[MAX_MT_IN_DATACONF];	    //模板终端列表

public:
    TDataConfTemplate( void )
	{
        m_cConfTemplateId.SetNull();
		memset( m_achConfTemplateName , 0, sizeof(m_achConfTemplateName) );

        m_byBaseMtCount = 0;
        memset( m_tDCBaseMt, 0, sizeof(m_tDCBaseMt) );
	}

/*====================================================================
    功能        ：  得到模板号
    输入参数说明：  无
    返回值说明  ：  模板号
====================================================================*/
	const CConfId & GetConfTemplateId( void ) const
	{
		return m_cConfTemplateId;
	}

/*====================================================================
    功能        ：  设置模板号
    输入参数说明：  cConfId－－模板号
    返回值说明  ：  无
====================================================================*/
	void SetConfTemplateId( const CConfId & cConfId )
	{
		m_cConfTemplateId = cConfId;
	}
	
/*====================================================================
    功能        ：  得到模板名
    输入参数说明：  无
    返回值说明  ：  模板名
====================================================================*/
	LPCSTR GetConfTemplateName( void ) const
	{
		return m_achConfTemplateName;
	}

/*====================================================================
    功能        ：  设置模板名
    输入参数说明：  lpszConfTemplateName－－会议模板名
    返回值说明  ：  无
====================================================================*/
	void SetConfTemplateName( LPCSTR lpszConfTemplateName )
	{
		strncpy( m_achConfTemplateName, lpszConfTemplateName, sizeof(m_achConfTemplateName) );
		m_achConfTemplateName[sizeof(m_achConfTemplateName) - 1] = '\0';
	}	

/*====================================================================
    功能        ：得到模板终端总数
    输入参数说明：无
    返回值说明  ：会议终端总数
====================================================================*/
	u8 GetBaseMtCount( void ) const
	{
		return m_byBaseMtCount;
	}

/*====================================================================
    功能        ：设置模板终端总数
    输入参数说明：byMtCount－－会议终端总数
    返回值说明  ：无
====================================================================*/
    void SetBaseMtCount( u8 byBaseMtCount )
    {
        m_byBaseMtCount = byBaseMtCount;
    }

/*====================================================================
    功能        ：得到模板中终端
    输入参数说明：byIndex－－终端索引
    返回值说明  ：终端指针
====================================================================*/
	const TDCBaseMt *GetTDCBaseMt( u8 byIndex ) const
	{
		if( byIndex <= (m_byBaseMtCount-1) )
		{
		    return &(m_tDCBaseMt[byIndex]);
		}
		else
		{
			return NULL;
		}
	}
	
/*====================================================================
    功能        ：  得到模板中所有终端
    输入参数说明：  ptDCBaseMtBuf－－缓冲区指针
                    byBufsize－－终端数量
    返回值说明  ：  实际终端数量
====================================================================*/
	u8 GetAllBaseMt( TDCBaseMt* ptDCBaseMtBuf , u8 byBufsize )
	{
		byBufsize = (byBufsize<m_byBaseMtCount) ? byBufsize : m_byBaseMtCount;
		
		memcpy( ptDCBaseMtBuf, m_tDCBaseMt, sizeof(TDCBaseMt)*byBufsize );
		return byBufsize;
	}
	
/*====================================================================
    功能        ：  在模板中加入终端
    输入参数说明：  tMt－－终端信息
    返回值说明  ：  true－－成功
                    false－－失败
====================================================================*/
	bool AddBaseMt( const TDCBaseMt & tMt )
	{
		if( m_byBaseMtCount >= MAX_MT_IN_DATACONF )
			return false;
		else
		{
			m_tDCBaseMt[m_byBaseMtCount] = tMt;
			m_byBaseMtCount++;
			return true;
		}
	}
	
///*====================================================================
//    功能        ：  在模板中删除终端
//    输入参数说明：  tMt－－终端信息
//    返回值说明  ：  true－－成功
//                    false－－失败
//====================================================================*/
//	void RemoveBaseMt( const TDCBaseMt & tMt )
//	{
//		for( int i = 0 ; i < m_byBaseMtCount ; i++ )
//		{
//			if( m_tDCBaseMt[i].IsEqual(tMt) )
//			{
//				for( int j = i + 1; j < m_byBaseMtCount; j++ )
//				{
//					m_tDCBaseMt[j-1] = m_tDCBaseMt[j];
//				}
//				m_byBaseMtCount--;
//				break;
//			}
//        }
//	}
	
/*====================================================================
    功能        ：  在模板中删除所有终端
    输入参数说明：  无
    返回值说明  ：  true－－成功
                    false－－失败
====================================================================*/
	void RemoveAllBaseMt( void )
	{
		m_byBaseMtCount = 0;
        memset( m_tDCBaseMt, 0, sizeof(m_tDCBaseMt) );
	}
	
/*====================================================================
    功能        ：  判断终端是否在模板中
    输入参数说明：  tMt－－终端信息
    返回值说明  ：  true－－受邀
                    false－－不受邀
====================================================================*/
	bool BaseMtInTemplate( const TDCBaseMt & tMt )
	{
		for( int i = 0; i < m_byBaseMtCount; i++ )
		{
			if( m_tDCBaseMt[i].IsEqual(tMt) )
			{
				return true;
			}
		}
		return false;
	}
    
///*====================================================================
//    功能        ：  修改终端状态
//    输入参数说明：  tMt－－终端信息
//    返回值说明  ：  无
//====================================================================*/
//	void ModifyBaseMt( const TDCBaseMt & tMt )
//	{
//		for( int i = 0; i < m_byBaseMtCount; i++ )
//		{
//			if( m_tDCBaseMt[i].IsEqual(tMt) )
//			{
//				m_tDCBaseMt[i] = tMt;
//				return;
//			}
//		}
//	}

/*====================================================================
    功能        ：赋值操作符重载
    输入参数说明：TDataConfTemplate& tDcTeml, 赋值的消息引用
    返回值说明  ：TDataConfTemplate对象引用
====================================================================*/
	TDataConfTemplate& operator=(const TDataConfTemplate& tTemplate) 
	{
		SetConfTemplateId(tTemplate.GetConfTemplateId());
		SetConfTemplateName(tTemplate.GetConfTemplateName());
		
		m_byBaseMtCount = tTemplate.GetBaseMtCount();
		for(u8 i = 0; i < m_byBaseMtCount; i++)
		{
			const TDCBaseMt *pMt = tTemplate.GetTDCBaseMt(i);
			if( pMt!=NULL )
			{
				m_tDCBaseMt[i] = *pMt;
			}
		}
		
		SetConfId(tTemplate.GetConfId());
		SetConfName(tTemplate.GetConfName());
		SetConfPassword(tTemplate.GetConfPassword());
		
		SetConfAppPara(tTemplate.GetConfAppPara());
		SetConfProperty(tTemplate.GetConfProperty());
		SetChairman(tTemplate.GetChairman());
		SetBandwidth(tTemplate.GetBandwidth());
		SetDuration(tTemplate.GetDuration());
        SetLocalConf(tTemplate.IsLocalConf());
        SetH323Call(tTemplate.IsH323Call());

		return *this;
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

#ifdef _WIN32
//临界区定义
class CSection
{
private:
	CRITICAL_SECTION* m_pSection;
public:
	CSection(CRITICAL_SECTION* pSection)
	{
        m_pSection=pSection;
        EnterCriticalSection(m_pSection);
	}

	~CSection()
	{
		if(m_pSection != NULL)
			LeaveCriticalSection(m_pSection);
	}
};
#else
class PMutex;
#endif


//地址薄
class CAddrBook_DC  
{
public:
	CAddrBook_DC()
	{
#ifdef _WIN32
        //临界区初始化
        InitializeCriticalSection(&m_Section_MtEntry);
        InitializeCriticalSection(&m_Section_ConfEntry);
#endif
	}
	~CAddrBook_DC()
	{
#ifdef _WIN32
        //临界区析构
        DeleteCriticalSection(&m_Section_MtEntry);
        DeleteCriticalSection(&m_Section_ConfEntry);
#endif
	}
	
protected:

#ifdef _WIN32
    CRITICAL_SECTION m_Section_MtEntry;     //Windows下终端列表访问临界区
    CRITICAL_SECTION m_Section_ConfEntry;   //Windows下会议模板列表访问临界区
#else
    PMutex  m_mutex_MtEntry;        //Linux下终端列表访问互斥量
    PMutex  m_mutex_ConfEntry;      //Linux下会议模板列表访问互斥量
#endif
    
    std::vector<TDCBaseMt>	m_vtDCBaseMt;						//终端列表
	std::vector<TDataConfTemplate>	m_vtDCTemplate;				//会议模板列表
	typedef std::vector<TDCBaseMt>::iterator itorMt;             //终端列表索引
	typedef std::vector<TDataConfTemplate>::iterator itorConf;   //会议模板列表索引
	
public:
    
/*====================================================================
    功能        ：  从列表中得到终端
    输入参数说明：  tDCBaseMt－－终端新信息
                    tDCBaseMtOld－－终端原信息
    返回值说明  ：  true－－成功
                    false－－失败
====================================================================*/
    bool GetMtEntry( const TDCBaseMt tDCBaseMt, TDCBaseMt & tDCBaseMtOld )
    {
#ifdef _WIN32
        CSection lock(&m_Section_MtEntry);
#else
        PWaitAndSignal lock(m_mutex_MtEntry);
#endif

        for( itorMt i=m_vtDCBaseMt.begin(); i<m_vtDCBaseMt.end(); i++ )
        {
            if( (*i).IsEqual(tDCBaseMt) )
            {
                tDCBaseMtOld = (*i);
                return true;
            }
        }
        return false;
    }
	
/*====================================================================
    功能        ：  从列表中得到会议模板
    输入参数说明：  cConfId－－会议模板号
                    tDCTmplOld－－会议模板原信息
    返回值说明  ：  true－－成功
                    false－－失败
====================================================================*/
    bool GetConfEntryById( const CConfId cConfId, TDataConfTemplate & tDCTmplOld )
    {
#ifdef _WIN32
        CSection lock(&m_Section_ConfEntry);
#else
        PWaitAndSignal lock(m_mutex_ConfEntry);
#endif

        for( itorConf i=m_vtDCTemplate.begin(); i<m_vtDCTemplate.end(); i++ )
        {
            if( (*i).GetConfTemplateId() == cConfId )
            {
                tDCTmplOld = (*i);
                return true;
            }
        }
        return false;
    }
    
/*====================================================================
    功能        ：  通过模板名从列表中得到会议模板
    输入参数说明：  lpszTemplateName－－会议模板名
                    tDCTmplOld－－会议模板原信息
    返回值说明  ：  true－－成功
                    false－－失败
====================================================================*/
    bool GetConfEntryByName( LPCSTR lpszTemplateName, TDataConfTemplate & tDCTmplOld )
    {
#ifdef _WIN32
        CSection lock(&m_Section_ConfEntry);
#else
        PWaitAndSignal lock(m_mutex_ConfEntry);
#endif

        for( itorConf i=m_vtDCTemplate.begin(); i<m_vtDCTemplate.end(); i++ )
        {
            if( strcmp((*i).GetConfTemplateName(), lpszTemplateName)==0 )
            {
                tDCTmplOld = (*i);
                return true;
            }
        }
        return false;
    }

/*====================================================================
    功能        ：  判断终端数是否已满
    输入参数说明：  
    返回值说明  ：  true－－已满
                    false－－不满
====================================================================*/
    bool IsMtEntryFull()
    {
#ifdef _WIN32
        CSection lock(&m_Section_MtEntry);
#else
        PWaitAndSignal lock(m_mutex_MtEntry);
#endif
    
        return (m_vtDCBaseMt.size()>=MAX_MTENTRY_NUM);
    }
    
/*====================================================================
    功能        ：  从列表中添加终端条目
    输入参数说明：  tDCBaseMt－－终端信息
    返回值说明  ：  true－－成功
                    false－－失败
====================================================================*/
	bool AddMtEntry( const TDCBaseMt& tDCBaseMt )
	{
#ifdef _WIN32
        CSection lock(&m_Section_MtEntry);
#else
        PWaitAndSignal lock(m_mutex_MtEntry);
#endif
            
        if( m_vtDCBaseMt.size()>=MAX_MTENTRY_NUM )
        {
            return false;
        }
        
		for( itorMt i=m_vtDCBaseMt.begin(); i<m_vtDCBaseMt.end(); i++ )
		{
			if( (*i).IsEqual(tDCBaseMt) )
			{
				return false;
			}
		}		
		m_vtDCBaseMt.push_back( tDCBaseMt );
		return true;
	}

/*====================================================================
    功能        ：  从列表中删除终端条目
    输入参数说明：  tDCBaseMt－－终端信息
    返回值说明  ：  true－－成功
                    false－－失败
====================================================================*/
	bool RemoveMtEntry( TDCBaseMt tDCBaseMt )
	{
#ifdef _WIN32
        CSection lock(&m_Section_MtEntry);
#else
        PWaitAndSignal lock(m_mutex_MtEntry);
#endif
            
		for( itorMt i=m_vtDCBaseMt.begin(); i<m_vtDCBaseMt.end(); i++ )
		{
			if( (*i).IsEqual(tDCBaseMt) )
			{
				m_vtDCBaseMt.erase(i);
				return true;
			}
		}
		return true;
	}

/*====================================================================
    功能        ：  从列表中修改终端条目
    输入参数说明：  tDCBaseMt－－终端信息
    返回值说明  ：  true－－成功
                    false－－失败
====================================================================*/
	bool ModifyMtEntry( const TDCBaseMt& tDCBaseMt )
	{
#ifdef _WIN32
        CSection lock(&m_Section_MtEntry);
#else
        PWaitAndSignal lock(m_mutex_MtEntry);
#endif

		for( itorMt i=m_vtDCBaseMt.begin(); i<m_vtDCBaseMt.end(); i++ )
		{
			if( (*i).IsEqual(tDCBaseMt) )
			{
				(*i) = tDCBaseMt;
				return true;
			}
		}		
		return false;
	}

/*====================================================================
    功能        ：  判断模板数是否已满
    输入参数说明：  
    返回值说明  ：  true－－已满
                    false－－不满
====================================================================*/
    bool IsConfEntryFull()
    {
#ifdef _WIN32
        CSection lock(&m_Section_ConfEntry);
#else
        PWaitAndSignal lock(m_mutex_ConfEntry);
#endif
    
        return (m_vtDCTemplate.size()>=MAX_CONFENTRY_NUM); 
    }
	
/*====================================================================
    功能        ：  从列表中添加会议模板
    输入参数说明：  tDCTemplate－－会议模板
    返回值说明  ：  true－－成功
                    false－－失败
====================================================================*/
	bool AddConfEntry( const TDataConfTemplate& tDCTemplate )
	{
#ifdef _WIN32
        CSection lock(&m_Section_ConfEntry);
#else
        PWaitAndSignal lock(m_mutex_ConfEntry);
#endif

		for( itorConf i=m_vtDCTemplate.begin(); i<m_vtDCTemplate.end(); i++ )
		{
			if( (*i).GetConfTemplateId() == tDCTemplate.GetConfTemplateId() )
			{
				return false;
			}
		}		
		m_vtDCTemplate.push_back( tDCTemplate );
		return true;
	}

/*====================================================================
    功能        ：  从列表中删除会议模板
    输入参数说明：  cConfId－－会议模板号
    返回值说明  ：  true－－成功
                    false－－失败
====================================================================*/
    bool RemoveConfEntry( const CConfId& cConfId )
	{
#ifdef _WIN32
        CSection lock(&m_Section_ConfEntry);
#else
        PWaitAndSignal lock(m_mutex_ConfEntry);
#endif

		for( itorConf i=m_vtDCTemplate.begin(); i<m_vtDCTemplate.end(); i++ )
		{
			if( (*i).GetConfTemplateId() == cConfId )
			{
				m_vtDCTemplate.erase(i);
				return true;
			}
		}
		return true;
	}

/*====================================================================
    功能        ：  从列表中修改会议模板
    输入参数说明：  tDCTemplate－－会议模板
    返回值说明  ：  true－－成功
                    false－－失败
====================================================================*/
	bool ModifyConfEntry( const TDataConfTemplate& tDCTemplate )
	{
#ifdef _WIN32
        CSection lock(&m_Section_ConfEntry);
#else
        PWaitAndSignal lock(m_mutex_ConfEntry);
#endif

		for( itorConf i=m_vtDCTemplate.begin(); i<m_vtDCTemplate.end(); i++ )
		{
			if( (*i).GetConfTemplateId() == tDCTemplate.GetConfTemplateId() )
			{
				(*i) = tDCTemplate;
				return true;
			}
		}		
		return false;
	}
	
/*====================================================================
    功能        ：  得到终端列表
    输入参数说明：  无
    返回值说明  ：  终端列表
====================================================================*/
    std::vector<TDCBaseMt> & GetTDCBaseMtVector()
    {
        return m_vtDCBaseMt;
    }
	
/*====================================================================
    功能        ：  得到会议模板列表
    输入参数说明：  无
    返回值说明  ：  会议模板列表
====================================================================*/
    std::vector<TDataConfTemplate> & GetTDCConfTemplate()
    {
        return m_vtDCTemplate;
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

#ifdef WIN32
#pragma pack( pop )
#endif

#endif // _DCSTRC_H
