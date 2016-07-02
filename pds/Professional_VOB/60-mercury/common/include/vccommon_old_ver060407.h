/*****************************************************************************
模块名      : updatelib
文件名      : vccommon_old.h
相关文件    : 
文件实现功能: 提取被更换数据信息的结构头文件
作者        : 顾振华
版本        : V1.0  Copyright(C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人		走读人      修改内容
2006/04/07  1.0         顾振华					创建
******************************************************************************/


#ifndef _VCCOMMON_OLD_060407_H_
#define _VCCOMMON_OLD_060407_H_


#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "kdvtype.h"
#include "kdvdef.h"
// 包含新版本的定义 
#include "vccommon.h"

#ifdef _LINUX_

#ifndef min
#define min(a,b) ((a)>(b)?(b):(a))
#endif
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

#endif//!_LINUX_



#ifdef WIN32
    #pragma comment( lib, "ws2_32.lib" ) 
    #pragma pack( push )
    #pragma pack( 1 )
    #define window( x )	x
    #define PACKED 
#else
    #include <netinet/in.h>
    #define window( x )
    #define PACKED __attribute__((__packed__))	// 取消编译器的优化对齐
#endif


//会议号结构 (len:16)
struct CConfIdOld
{
protected:
	u8  	m_abyConfId[MAXLEN_CONFID];

public:
	u8   GetConfId( u8   * pbyConfId, u8   byBufLen ) const; //获得会议号数组
	void CreateConfId( void );                         //创建会议号
	void GetConfIdString( LPSTR lpszConfId, u8   byBufLen ) const;//获得会议号字符串
	void SetConfId( const CConfIdOld & cConfId );                  //设置会议号
	u8   SetConfId( const u8   * pbyConfId, u8   byBufLen );          //设置会议号数组
	u8   SetConfId( LPCSTR lpszConfId );                          //设置会议号数组	
	BOOL IsNull( void ) const;                                  //判断会议号是否为空
	void SetNull( void );                                       //设置会议号为空
	BOOL operator == ( const CConfIdOld & cConfId ) const;         //操作符重载
	void Print( void )
	{
		s8 achBuf[MAXLEN_CONFID*2+1];
		memset(achBuf, 0, sizeof(achBuf));
		GetConfIdString( achBuf, sizeof(achBuf) );
		OspPrintf( TRUE, FALSE, "%s\n", achBuf);
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
/*-------------------------------------------------------------------
                               CConfIdOld                                  
--------------------------------------------------------------------*/

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
inline u8 CConfIdOld::GetConfId( u8 * pbyConfId, u8 byBufLen ) const
{
	u8	byLen = min( sizeof( m_abyConfId ), byBufLen );
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
inline void CConfIdOld::GetConfIdString( LPSTR lpszConfId, u8 byBufLen ) const
{
	u8	byLoop;

	for( byLoop = 0; byLoop < sizeof( m_abyConfId ) && byLoop < ( byBufLen - 1 ) / 2; byLoop++ )
	{
		sprintf( lpszConfId + byLoop * 2, "%.2x", m_abyConfId[byLoop] );
	}
	lpszConfId[byLoop * 2] = '\0';
}

/*====================================================================
    函数名      ：SetConfId
    功能        ：设置会议号数组
    算法实现    ：
    引用全局变量：
    输入参数说明：const CConfIdOld & cConfId, 输入会议号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/31    1.0         LI Yi         创建
====================================================================*/
inline void CConfIdOld::SetConfId( const CConfIdOld & cConfId )
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
inline u8 CConfIdOld::SetConfId( const u8 * pbyConfId, u8 byBufLen )
{
	u8	bySize = min( byBufLen, sizeof( m_abyConfId ) );
	
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
inline u8 CConfIdOld::SetConfId( LPCSTR lpszConfId )
{
	LPCSTR	lpszTemp = lpszConfId;
	s8	achTemp[3], *lpszStop;
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
inline void CConfIdOld::CreateConfId( void )
{
	struct tm	*ptmCurTime;
	time_t		tCurTime = time( NULL );
	u8	byTemp;
	u16	wTemp;

	memset( m_abyConfId, 0, sizeof( m_abyConfId ) );
	
	//get current time
	ptmCurTime = localtime( &tCurTime );
	//year
	wTemp = ptmCurTime->tm_year + 1900;
	memcpy( m_abyConfId, &wTemp, sizeof( u16 ) );
	//month
	byTemp = ptmCurTime->tm_mon + 1;
	memcpy( m_abyConfId + 2, &byTemp, sizeof( u8 ) );
	//day
	byTemp = ptmCurTime->tm_mday;
	memcpy( m_abyConfId + 3, &byTemp, sizeof( u8 ) );
	//hour
	byTemp = ptmCurTime->tm_hour;
	memcpy( m_abyConfId + 4, &byTemp, sizeof( u8 ) );
	//minute
	byTemp = ptmCurTime->tm_min;
	memcpy( m_abyConfId + 5, &byTemp, sizeof( u8 ) );
	//second
	byTemp = ptmCurTime->tm_sec;
	memcpy( m_abyConfId + 6, &byTemp, sizeof( u8 ) );
	//msec
	wTemp = 0;
	memcpy( m_abyConfId + 7, &wTemp, sizeof( u16 ) );

	//rand
	wTemp = rand();
	memcpy( m_abyConfId + 9, &wTemp, sizeof( u16 ) );
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
inline BOOL CConfIdOld::IsNull( void ) const
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
inline void CConfIdOld::SetNull( void )
{
	memset( m_abyConfId, 0, sizeof( m_abyConfId ) );
}

/*====================================================================
    函数名      ：operator==
    功能        ：操作符重载
    算法实现    ：
    引用全局变量：
    输入参数说明：const CConfIdOld & cConfId, 输入会议号
    返回值说明  ：相等返回TRUE，否则返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/31    1.0         LI Yi         创建
====================================================================*/
inline BOOL CConfIdOld::operator == ( const CConfIdOld & cConfId ) const
{
	u8	abyTemp[sizeof( m_abyConfId )];

	cConfId.GetConfId( abyTemp, sizeof( abyTemp ) );
	if( memcmp( abyTemp, m_abyConfId, sizeof( m_abyConfId ) ) == 0 )
		return( TRUE );
	else
		return( FALSE );
}


//定义会议属性结构(会议中不会改变)
struct TConfAttrbOld
{
protected:
	u8     m_byOpenMode;        //会议开放方式: 0-不开放,拒绝列表以外的终端 1-根据密码加入 2-完全开放
	u8     m_byEncryptMode;     //会议加密模式: 0-不加密, 1-des加密,2-aes加密
	u8     m_byMulticastMode;   //组播会议方式: 0-不是组播会议 1-组播会议
	u8     m_byDataMode;        //数据会议方式: 0-不包含数据的视音频会议 1-包含数据的视音频会议 2-只有数据的会议
	u8     m_byReleaseMode;     //会议结束方式: 0-不会自动结束 1-无终端时自动结束
    u8     m_byVideoMode;       //会议视频模式: 0-速度优先 1-画质优先
	u8     m_byCascadeMode;     //会议级连方式: 0-不支持合并级联, 1-支持合并级联
	u8     m_byPrsMode;         //丢包重传方式: 0-不重传 1-重
    u8     m_byHasTvWallModule; //电视墙模板:   0-无电视墙模板  1-有电视墙模板
	u8     m_byHasVmpModule;    //画面合成模板: 0-无画面合成模板  1-有画面合成模板
	u8     m_byDiscussConf;     //是否讨论会议: 0-不是讨论会议(演讲会议) 1-讨论会议 (这一字段仅用来控制会议开始后是否启动混音)
	u8     m_byUseAdapter;		//是否启用适配: 0-不启用任何适配 2-按需要进行(码流，码率)适配 
	u8     m_bySpeakerSrc;		//发言人的源:   0-看自己 1-看主席 2-看上一次发言人 
	u8     m_bySpeakerSrcMode;  //发言人的源的方式: 0-无效值 1-仅视频 2-仅音频 3-视音频
	u8     m_byMulcastLowStream;//是否组播低速码流: 0-不是组播低速码流(组播高速码流) 1-组播低速码流
	u8     m_byAllInitDumb;     //终端入会后是否初始哑音 0-不哑音 1-哑音
	u8	   m_byDualMode;		//会议的双流发起方式: 0-发言人 1-任意终端
	u8     m_byUniformMode;     //码流转发时是否支持归一化整理方式: CONF_UNIFORMMODE_NONE-不重整 CONF_UNIFORMMODE_valid-重整，
	                            //这个处理方式与丢包重传方式互斥
	u8	   m_byReserve1;		//保留字段
	u8	   m_byReserve2;		//保留字段
    
//    u32    m_dwMulticastIp;     //会议组播地址；如果配置是组播会议，需要填写组播地址
//    u16    m_wMulticastPort;    //会议组播起始端口；如果配置是组播会议，需要填写组播地址
//	u8	   m_byReserve3;		//保留字段
//	u8	   m_byReserve4;		//保留字段

public:
	TConfAttrbOld( void )
	{ 
		memset( this, 0, sizeof(TConfAttrbOld) );
		m_byOpenMode = 2;
	}

    void   SetOpenMode(u8   byOpenMode){ m_byOpenMode = byOpenMode;} 
    u8     GetOpenMode( void ) const { return m_byOpenMode; }
	void   SetEncryptMode(u8   byEncryptMode){ m_byEncryptMode = byEncryptMode;} 
    u8     GetEncryptMode( void ) const { return m_byEncryptMode; }
    void   SetMulticastMode(BOOL bMulticastMode){ m_byMulticastMode = bMulticastMode;} 
    BOOL   IsMulticastMode( void ) const { return m_byMulticastMode == 0 ? FALSE : TRUE; }
    void   SetDataMode(u8   byDataMode){ m_byDataMode = byDataMode;} 
    u8     GetDataMode( void ) const { return m_byDataMode; }
    void   SetReleaseMode(BOOL bReleaseMode){ m_byReleaseMode = bReleaseMode;} 
    BOOL   IsReleaseNoMt( void ) const { return m_byReleaseMode == 0 ? FALSE : TRUE; }
    void   SetQualityPri(BOOL bQualityPri){ m_byVideoMode = bQualityPri;} 
    BOOL   IsQualityPri( void ) const { return m_byVideoMode == 0 ? FALSE : TRUE; }
	void   SetSupportCascade(BOOL bCascadeMode){ m_byCascadeMode = (bCascadeMode==TRUE?1:0); }
	BOOL   IsSupportCascade( void ) const { return (m_byCascadeMode != 0 );}
	void   SetPrsMode( BOOL bResendPack ){ m_byPrsMode = bResendPack; }
	BOOL   IsResendLosePack( void ){ return m_byPrsMode == 0 ? FALSE : TRUE; }
	void   SetHasTvWallModule( BOOL bHasTvWallModule ){ m_byHasTvWallModule = bHasTvWallModule; }
	BOOL   IsHasTvWallModule( void ){ return m_byHasTvWallModule == 0 ? FALSE : TRUE; }
	void   SetHasVmpModule( BOOL bHasVmpModule ){ m_byHasVmpModule = bHasVmpModule; }
	BOOL   IsHasVmpModule( void ){ return m_byHasVmpModule == 0 ? FALSE : TRUE; }
	void   SetDiscussConf( BOOL bDiscussConf ){ m_byDiscussConf = bDiscussConf; }
	BOOL   IsDiscussConf( void ){ return m_byDiscussConf == 0 ? FALSE : TRUE; }
    void   SetUseAdapter( BOOL bUseAdapter ){ m_byUseAdapter = bUseAdapter; } 
    BOOL   IsUseAdapter( void ) const { return m_byUseAdapter == 0 ? FALSE : TRUE; }
    void   SetSpeakerSrc( u8 bySpeakerSrc ){ m_bySpeakerSrc = bySpeakerSrc;} 
    u8     GetSpeakerSrc( void ) const { return m_bySpeakerSrc; }
    void   SetSpeakerSrcMode( u8 bySpeakerSrcMode ){ m_bySpeakerSrcMode = bySpeakerSrcMode;} 
    u8     GetSpeakerSrcMode( void ) const { return m_bySpeakerSrcMode; }
	void   SetMulcastLowStream( BOOL bMulcastLowStream ){ m_byMulcastLowStream = bMulcastLowStream;} 
    BOOL   IsMulcastLowStream( void ) const { return m_byMulcastLowStream == 0 ? FALSE : TRUE; }
	void   SetAllInitDumb( BOOL bAllInitDumb ){ m_byAllInitDumb = bAllInitDumb;} 
    BOOL   IsAllInitDumb( void ) const { return m_byAllInitDumb == 0 ? FALSE : TRUE; }
	void   SetUniformMode( BOOL bUniformMode ){ m_byUniformMode = bUniformMode; }
	u8     IsAdjustUniformPack( void ){ return m_byUniformMode; }
	void   SetDualMode(	u8 byDualMode ){ m_byDualMode = byDualMode;	}
	u8	   GetDualMode( void ) const { return m_byDualMode;	}
//    void   SetMulticastIp (u32 dwIp) { m_dwMulticastIp = htonl(dwIp); }
//    u32    GetMulticastIp ( void ) const { return ntohl(m_dwMulticastIp); }
//    void   SetMulticastPort ( u16 wPort) { m_wMulticastPort = wPort; }
//    u16    GetMulticastPort ( void ) const { return m_wMulticastPort; }
	void   Print( void )
	{
		OspPrintf( TRUE, FALSE, "\nConfAttrb:\n" );
		OspPrintf( TRUE, FALSE, "m_byOpenMode: %d\n", m_byOpenMode);
		OspPrintf( TRUE, FALSE, "m_byEncryptMode: %d\n", m_byEncryptMode);
		OspPrintf( TRUE, FALSE, "m_byMulticastMode: %d\n", m_byMulticastMode);
//        OspPrintf( TRUE, FALSE, "m_byMulticastIp: 0x%x\n", GetMulticastIp());
//        OspPrintf( TRUE, FALSE, "m_byMulticastPort: %d\n", m_wMulticastPort);
		OspPrintf( TRUE, FALSE, "m_byDataMode: %d\n", m_byDataMode);
		OspPrintf( TRUE, FALSE, "m_byReleaseMode: %d\n", m_byReleaseMode);
		OspPrintf( TRUE, FALSE, "m_byVideoMode: %d\n", m_byVideoMode);
		OspPrintf( TRUE, FALSE, "m_byCascadeMode: %d\n", m_byCascadeMode);
		OspPrintf( TRUE, FALSE, "m_byPrsMode: %d\n", m_byPrsMode);
		OspPrintf( TRUE, FALSE, "m_byHasTvWallModule: %d\n", m_byHasTvWallModule);
		OspPrintf( TRUE, FALSE, "m_byHasVmpModule: %d\n", m_byHasVmpModule);	
		OspPrintf( TRUE, FALSE, "m_byDiscussConf: %d\n", m_byDiscussConf);
		OspPrintf( TRUE, FALSE, "m_byUseAdapter: %d\n", m_byUseAdapter);
		OspPrintf( TRUE, FALSE, "m_bySpeakerSrc: %d\n", m_bySpeakerSrc);
		OspPrintf( TRUE, FALSE, "m_bySpeakerSrcMode: %d\n", m_bySpeakerSrcMode);
		OspPrintf( TRUE, FALSE, "m_byMulcastLowStream: %d\n", m_byMulcastLowStream);
		OspPrintf( TRUE, FALSE, "m_byAllInitDumb: %d\n", m_byAllInitDumb);
		OspPrintf( TRUE, FALSE, "m_byUniformMode: %d\n", m_byUniformMode);
		OspPrintf( TRUE, FALSE, "m_byDualMode: %d\n", m_byDualMode);
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


//定义会议信息结构,该结构定义了会议基本信息和状态 (len:725)
struct TConfInfoOld
{
protected:
    CConfIdOld    m_cConfId;                        //会议号，全网唯一
    TKdvTime      m_tStartTime;                     //开始时间，控制台填0为立即开始
    u16           m_wDuration;                      //持续时间(分钟)，0表示不自动停止
    u16           m_wBitRate;                       //会议码率(单位:Kbps,1K=1024)
    u16           m_wSecBitRate;                    //双速会议的第2码率(单位:Kbps,为0表示是单速会议)
	u16			  m_wDcsBitRate;					//数据会议码率(单位:Kbps,为0表示不支持数据会议)
    u16           m_wReserved;                      //保留
    u8            m_byTalkHoldTime;                 //最小发言持续时间(单位:秒)
    u16            m_wMixDelayTime;                 //混音延时时间
    TCapSupport   m_tCapSupport;                    //会议支持的媒体
    s8            m_achConfPwd[MAXLEN_PWD+1];       //会议密码
    s8            m_achConfName[MAXLEN_CONFNAME+1]; //会议名
    s8            m_achConfE164[MAXLEN_E164+1];     //会议的E164号码
    TMtAlias      m_tChairAlias;                    //会议中主席的别名
    TMtAlias      m_tSpeakerAlias;                  //会议中发言人的别名
    TConfAttrbOld m_tConfAttrb;                     //会议属性

    TMediaEncrypt m_tMediaKey;                      //第一版本所有密钥一样，不支持更新

public:
    TConfStatus   m_tStatus;                        //会议状态

public:	
    TConfInfoOld( void ){ memset( this, 0, sizeof( this ) ); m_tCapSupport.Clear(); }
    CConfIdOld GetConfId( void ) const { return m_cConfId; }
    void    SetConfId( CConfIdOld cConfId ){ m_cConfId = cConfId; }
    TKdvTime GetKdvStartTime( void ) const { return( m_tStartTime ); }
    void   SetKdvStartTime( TKdvTime tStartTime ){ m_tStartTime = tStartTime; }
    void   SetDuration(u16  wDuration){ m_wDuration = htons(wDuration);} 
    u16    GetDuration( void ) const { return ntohs(m_wDuration); }
    void   SetBitRate(u16  wBitRate){ m_wBitRate = htons(wBitRate);} 
    u16    GetBitRate( void ) const { return ntohs(m_wBitRate); }
    void   SetSecBitRate(u16 wSecBitRate){ m_wSecBitRate = htons(wSecBitRate);} 
    u16    GetSecBitRate( void ) const { return ntohs(m_wSecBitRate); }
	void   SetDcsBitRate(u16 wDcsBitRate){ m_wDcsBitRate = htons(wDcsBitRate); }
	u16	   GetDcsBitRate( void ) const { return ntohs(m_wDcsBitRate); }

    void   SetMainVideoFormat(u8 byVideoFormat)
    { 
        TSimCapSet tSim = m_tCapSupport.GetMainSimCapSet();
        tSim.SetVideoResolution(byVideoFormat); 
        m_tCapSupport.SetMainSimCapSet(tSim);
    } 
    u8     GetMainVideoFormat( void ) const { return m_tCapSupport.GetMainSimCapSet().GetVideoResolution(); }
    void   SetSecVideoFormat(u8 byVideoFormat)
    { 
        TSimCapSet tSim = m_tCapSupport.GetSecondSimCapSet();
        tSim.SetVideoResolution(byVideoFormat);
        m_tCapSupport.SetSecondSimCapSet(tSim); 
    } 
    u8     GetSecVideoFormat(void) const { return m_tCapSupport.GetSecondSimCapSet().GetVideoResolution(); }
    void   SetDoubleVideoFormat(u8 byVideoFormat) { m_tCapSupport.SetDStreamResolution(byVideoFormat); } 
    u8     GetDoubleVideoFormat(void) const { return m_tCapSupport.GetDStreamResolution(); }
    u8     GetVideoFormat(u8 byVideoType, u8 byChanNo = LOGCHL_VIDEO);

    void   SetMixDelayTime(u16 wDelayTime) { m_wMixDelayTime = htons(wDelayTime); }
    u16     GetMixDelayTime(void) { return ntohs(m_wMixDelayTime); }
    void   SetTalkHoldTime(u8   byTalkHoldTime){ m_byTalkHoldTime = byTalkHoldTime;} 
    u8     GetTalkHoldTime( void ) const { return m_byTalkHoldTime; }
    void   SetCapSupport(TCapSupport tCapSupport){ m_tCapSupport = tCapSupport;} 
    TCapSupport GetCapSupport( void ) const { return m_tCapSupport; }
    void   SetConfPwd( LPCSTR lpszConfPwd );
    LPCSTR GetConfPwd( void ) const { return m_achConfPwd; }
    void   SetConfName( LPCSTR lpszConfName );
    LPCSTR GetConfName( void ) const { return m_achConfName; }
    void   SetConfE164( LPCSTR lpszConfE164 );
    LPCSTR GetConfE164( void ) const { return m_achConfE164; }
    void   SetChairAlias(TMtAlias tChairAlias){ m_tChairAlias = tChairAlias;} 
    TMtAlias  GetChairAlias( void ) const { return m_tChairAlias; }
    void   SetSpeakerAlias(TMtAlias tSpeakerAlias){ m_tSpeakerAlias = tSpeakerAlias;} 
    TMtAlias  GetSpeakerAlias( void ) const { return m_tSpeakerAlias; }
    void   SetConfAttrb(TConfAttrbOld tConfAttrb){ m_tConfAttrb = tConfAttrb;} 
    TConfAttrbOld  GetConfAttrb( void ) const { return m_tConfAttrb; }
    void   SetStatus(TConfStatus tStatus){ m_tStatus = tStatus;} 
    TConfStatus  GetStatus( void ) const { return m_tStatus; }	

	TMediaEncrypt& GetMediaKey(void) { return m_tMediaKey; };
	void SetMediaKey(TMediaEncrypt& tMediaEncrypt){ memcpy(&m_tMediaKey, &tMediaEncrypt, sizeof(tMediaEncrypt));}

	void   GetVideoScale(u8 byVideoType, u16 &wVideoWidth, u16 &wVideoHeight, u8* pbyVideoResolution = NULL);
	BOOL   HasChairman( void ) const{ if(m_tStatus.m_tChairman.GetMtId()==0)return FALSE; return TRUE; }	
	TMt    GetChairman( void ) const{ return m_tStatus.m_tChairman; }
	void   SetNoChairman( void ){ memset( &m_tStatus.m_tChairman, 0, sizeof( TMt ) ); }
	void   SetChairman( TMt tChairman ){ m_tStatus.m_tChairman = tChairman; }
	BOOL   HasSpeaker( void ) const{ if(m_tStatus.m_tSpeaker.GetMtId()==0)return FALSE; return TRUE; }
    TMt    GetSpeaker( void ) const{ return m_tStatus.m_tSpeaker; }
	void   SetNoSpeaker( void ){ memset( &m_tStatus.m_tSpeaker, 0, sizeof( TMt ) ); }
	void   SetSpeaker( TMt tSpeaker ){ m_tStatus.m_tSpeaker = tSpeaker; }
	time_t GetStartTime( void ) const;	
	void SetStartTime( time_t dwStartTime );

	void Print( void )
	{
		OspPrintf( TRUE, FALSE, "\nConfInfo:\n" );
		OspPrintf( TRUE, FALSE, "m_cConfId: " );
		m_cConfId.Print();
		OspPrintf( TRUE, FALSE, "\nm_tStartTime: " );
        m_tStartTime.Print();
		OspPrintf( TRUE, FALSE, "\nm_wDuration: %d\n", ntohs(m_wDuration) );
		OspPrintf( TRUE, FALSE, "m_wBitRate: %d\n", ntohs(m_wBitRate) );
		OspPrintf( TRUE, FALSE, "m_wDcsBitRata: %d\n", ntohs(m_wDcsBitRate) );
		OspPrintf( TRUE, FALSE, "m_byVideoFormat: %d\n", GetMainVideoFormat() );
		OspPrintf( TRUE, FALSE, "m_byTalkHoldTime: %d\n", m_byTalkHoldTime );
		OspPrintf( TRUE, FALSE, "m_tCapSupport:" );
	    m_tCapSupport.Print();
		OspPrintf( TRUE, FALSE, "\nm_achConfPwd: %s\n", m_achConfPwd );
		OspPrintf( TRUE, FALSE, "m_achConfName: %s\n", m_achConfName );
		OspPrintf( TRUE, FALSE, "m_achConfE164: %s\n", m_achConfE164 );
		OspPrintf( TRUE, FALSE, "\n" );
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// 经过Pack处理后的会议存储信息 
struct TPackConfStoreOld
{
    TConfInfoOld m_tConfInfo;
	u8			 m_byMtNum;
	u16			 m_wAliasBufLen;  //(主机序 存储于文件中)进行Pack时指定后面的 m_pbyAliasBuf 字节长度
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


#ifdef WIN32
#pragma pack( pop )
#endif




#endif _VCCOMMON_OLD_060407_H_
