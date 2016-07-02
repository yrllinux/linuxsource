/*****************************************************************************
模块名      : updatelib
文件名      : vccommon_old.h
相关文件    : 
文件实现功能: 提取被更换数据信息的结构头文件
作者        : 张宝卿
版本        : V1.0  Copyright(C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人		走读人      修改内容
2006/02/22  1.0         张宝卿					创建
******************************************************************************/
#ifndef _VCCOMMON_040602_OLD_H_
#define _VCCOMMON_040602_OLD_H_

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "kdvtype.h"
#include "kdvdef.h"


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

#define MAXNUM_PERIEQP_CHNNL_OLD		16
#define MAXNUM_CONF_MT_OLD				192

//定义简化MCU、终端或外设结构 (len:6)
struct TMtOld
{
protected:
    u8          m_byMainType;   //设备主类
    u8  		m_bySubType;	//设备子类
	u8  		m_byMcuId;		//MCU号
	u8  		m_byEqpId;	    //设备号	
	u8          m_byConfDriId;  //DRI板号
    u8          m_byConfIdx;    //会议索引号，即会议实例号 1 - MAXNUM_MCU_CONF

public:
	//操作所有类型函数
	void   SetType( u8   byType ){ m_byMainType = byType; }         //设置结构主类
	u8     GetType( void ) const { return m_byMainType; }           //得到结构主类
	void   SetMcuId( u8   byMcuId ){ m_byMcuId = byMcuId; }         //设置McuId
	u8     GetMcuId( void ) const { return m_byMcuId; }             //得到McuId
	BOOL   operator ==( const TMtOld & tObj ) const;                  //判断是否相等

	//操作MCU函数
	void   SetMcu( u8   byMcuId );                         //设置MCU
	void   SetMcuType( u8  byMcuType ){ m_byMainType = TYPE_MCU; m_bySubType = byMcuType; }
	u8     GetMcuType( void ) const { return m_bySubType; }

	//操作外设函数
	void   SetMcuEqp( u8   byMcuId, u8   byEqpId, u8   byEqpType );            //设置MCU外设
	void   SetEqpType( u8   byEqpType ){ m_byMainType = TYPE_MCUPERI; m_bySubType = byEqpType; }                                       //设置MCU外设类型
	u8     GetEqpType( void ) const { return m_bySubType; }
	void   SetEqpId( u8   byEqpId ){ m_byMainType = TYPE_MCUPERI; m_byEqpId = byEqpId; }
	u8     GetEqpId( void ) const { return m_byEqpId; }
	
	//操作终端函数
	void   SetMt( u8   byMcuId, u8   byMtId, u8   m_byDriId = 0, u8   m_byConfIdx = 0);
	void   SetMt( TMtOld tMt);
	void   SetMtId( u8   byMtId ){ m_byMainType = TYPE_MT; m_byEqpId = byMtId; }
	u8     GetMtId( void ) const { return m_byEqpId; }//设置终端
	void   SetMtType( u8   byMtType ) { m_byMainType = TYPE_MT; m_bySubType = byMtType; }
	u8     GetMtType( void ) const { return m_bySubType; }  	
    void   SetDriId(u8   byDriId){ m_byConfDriId = byDriId; }
    u8     GetDriId( void ) const { return m_byConfDriId; }
    void   SetConfIdx(u8   byConfIdx){ m_byConfIdx = byConfIdx; } 
    u8     GetConfIdx( void ) const { return m_byConfIdx; }
	void   SetNull( void ){ m_byMcuId = 0; m_byEqpId = 0; m_byConfDriId = 0; }
	BOOL   IsNull( void ){ return m_byMcuId == 0 && m_byEqpId == 0; }
	BOOL   IsLocal() const {	return (m_byMcuId == LOCAL_MCUID );	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//定义终端结构
typedef TMtOld TEqpOld;

//终端轮询参数 (len:8)
struct TMtPollParamOld: public TMtOld
{
protected:
	u16   m_wKeepTime;        //终端轮询的保留时间 单位:秒(s)
    u32   m_dwPollNum;        //终端轮询的次数
public:
	void  SetKeepTime(u16 wKeepTime) { m_wKeepTime = htons(wKeepTime); } 
	u16   GetKeepTime(void) const { return ntohs(m_wKeepTime); }
    void  SetPollNum(u32 dwPollNum) { m_dwPollNum = htonl(dwPollNum); } 
	u32   GetPollNum(void) const { return ntohl(m_dwPollNum); }
	TMtOld   GetTMt(void) const { return *(TMtOld *)this; }
	void  SetTMt(TMtOld tMt) { memcpy( this, &tMt, sizeof(tMt) ); }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//定义画面合成成员结构 (len:7)
struct TVMPMemberOld: public TMtOld
{
protected:
    u8   m_byMemberType;     //画面合成成员类型,参见mcuconst.h中画面合成成员类型定义
    u8   m_byMemStatus;      //成员状态(MT_STATUS_CHAIRMAN, MT_STATUS_SPEAKER, MT_STATUS_AUDIENCE)

public:
    void  SetMemberType(u8   byMemberType){ m_byMemberType = byMemberType;} 
    u8    GetMemberType( void ) const { return m_byMemberType; }
    void  SetMemStatus(u8 byStatus) { m_byMemStatus = byStatus; }
    u8    GetMemStatus(void) const { return m_byMemStatus; }
    void  SetMemberTMt( TMtOld tMt ){ memcpy( this, &tMt, sizeof(TMt) ); }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//定义画面合成参数结构 (len:121)
struct TVMPParamOld
{
protected:
	u8      m_byVMPAuto;      //是否是自动画面合成 0-否 1-是
	u8      m_byVMPBrdst;     //合成图像是否向终端广播 0-否 1-是 
	u8      m_byVMPStyle;     //画面合成风格,参见mcuconst.h中画面合成风格定义
    u8      m_byVMPSchemeId;  //合成风格方案编号,最大支持5套方案,1-5
	TVMPMemberOld  m_atVMPMember[MAXNUM_VMP_MEMBER]; //画面合成成员
    u8      m_byVMPMode;      //图像复合方式: 0-不图像复合 1-会控或主席控制图像复合 2-自动图像复合(动态分屏与设置成员)
	u8		m_byRimEnabled;	  //是否使用边框: 0-不使用(默认) 1-使用;
							  //本字段目前只对方案0有效, 其他方案暂不涉及本字段的设置和判断
	u8		m_byReserved1;	  //保留字段
	u8		m_byReserved2;	  //保留字段
public:
    void   SetVMPAuto(u8   byVMPAuto){ m_byVMPAuto = byVMPAuto;} 
    BOOL   IsVMPAuto( void ) const { return m_byVMPAuto == 0 ? FALSE : TRUE; }
    void   SetVMPBrdst(u8   byVMPBrdst){ m_byVMPBrdst = byVMPBrdst;} 
    BOOL   IsVMPBrdst( void ) const { return m_byVMPBrdst == 0 ? FALSE : TRUE; }
    void   SetVMPStyle(u8   byVMPStyle){ m_byVMPStyle = byVMPStyle;} 
    u8     GetVMPStyle( void ) const { return m_byVMPStyle; }
    void   SetVMPSchemeId(u8 bySchemeId) { m_byVMPSchemeId = bySchemeId; }
    u8     GetVMPSchemeId(void) const { return m_byVMPSchemeId; }
    void   SetVMPMode(u8   byVMPMode){ m_byVMPMode = byVMPMode;} 
    u8     GetVMPMode( void ) const { return m_byVMPMode; }
	void   SetIsRimEnabled(u8 byEnabled){ m_byRimEnabled = byEnabled; }
	BOOL32 GetIsRimEnabled(void) const { return m_byRimEnabled == 1 ? TRUE : FALSE; }
    u8     GetMaxMemberNum( void ) const 
	{
		u8   byMaxMemNum = 1;

		switch( m_byVMPStyle ) 
		{
		case VMP_STYLE_ONE:
			byMaxMemNum = 1;
			break;
		case VMP_STYLE_VTWO:
		case VMP_STYLE_HTWO:
			byMaxMemNum = 2;
			break;
		case VMP_STYLE_THREE:
			byMaxMemNum = 3;
			break;
		case VMP_STYLE_FOUR:
			byMaxMemNum = 4;
			break;
		case VMP_STYLE_SIX:
			byMaxMemNum = 6;
			break;
		case VMP_STYLE_EIGHT:
			byMaxMemNum = 8;
			break;
		case VMP_STYLE_NINE:
			byMaxMemNum = 9;
			break;
		case VMP_STYLE_TEN:
			byMaxMemNum = 10;
			break;
		case VMP_STYLE_THIRTEEN:
			byMaxMemNum = 13;
			break;
		case VMP_STYLE_SIXTEEN:
			byMaxMemNum = 16;
			break;
		case VMP_STYLE_SPECFOUR:
			byMaxMemNum = 4;
			break;
		case VMP_STYLE_SEVEN:
			byMaxMemNum = 7;
			break;
		default:
			byMaxMemNum = 1;
			break;
		}
		return byMaxMemNum;
	}
	void   SetVmpMember( u8   byMemberId, TVMPMemberOld tVMPMember )
	{
		if( byMemberId >= MAXNUM_VMP_MEMBER )return;
		m_atVMPMember[byMemberId] = tVMPMember;
	}
	void   ClearVmpMember( u8 byMemberId )
	{
		if( byMemberId >= MAXNUM_VMP_MEMBER )return;
		m_atVMPMember[byMemberId].SetNull();
        m_atVMPMember[byMemberId].SetMemberType(0);
	}
    TVMPMemberOld *GetVmpMember( u8   byMemberId )
	{
		if( byMemberId >= MAXNUM_VMP_MEMBER )return NULL;
		return &m_atVMPMember[byMemberId];
	}
	BOOL IsMtInMember( TMtOld tMt )
	{
		u8 byLoop = 0;
		u8 byLoopCtrl = 0;
		while( byLoop < GetMaxMemberNum() && byLoopCtrl < MAXNUM_VMP_MEMBER )
		{
			if( tMt.GetMtId() == m_atVMPMember[byLoop].GetMtId() && 
				tMt.GetMcuId() == m_atVMPMember[byLoop].GetMcuId() )
			{
				return TRUE;
			}

			byLoop++;
			byLoopCtrl++;
		}

		return FALSE;
	}
	u8 GetChlOfMtInMember( TMtOld tMt )
	{
		u8 byLoop = 0;
		while( byLoop < GetMaxMemberNum() )
		{
			if( tMt.GetMtId() == m_atVMPMember[byLoop].GetMtId() )
			{
				return byLoop;
			}

			byLoop++;
		}

		return MAXNUM_VMP_MEMBER;
	}
	void   Print( void )
	{
		OspPrintf( TRUE, FALSE, "\nVMPParam:\n" );
		OspPrintf( TRUE, FALSE, "m_byVMPAuto: %d\n", m_byVMPAuto);
		OspPrintf( TRUE, FALSE, "m_byVMPBrdst: %d\n", m_byVMPBrdst);
		OspPrintf( TRUE, FALSE, "m_byVMPStyle: %d\n", m_byVMPStyle);
        OspPrintf( TRUE, FALSE, "m_byVmpSchemeId: %d\n", m_byVMPSchemeId );
		OspPrintf( TRUE, FALSE, "m_byRimEnabled: %d\n", m_byRimEnabled );
        OspPrintf( TRUE, FALSE, "m_byVMPMode: %d\n", m_byVMPMode );
		for( int i=0;i<GetMaxMemberNum();i++ )
		{
			OspPrintf( TRUE, FALSE, "VMP member %d: MtId-%d, Type-%d\n", 
				i,  m_atVMPMember[i].GetMtId(), m_atVMPMember[i].GetMemberType() );
		}
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//会议号结构 (len:16)
struct CConfIdOld
{
protected:
	u8  	m_abyConfId[MAXLEN_CONFID];

public:
	u8   CConfIdOld::GetConfId( u8   * pbyConfId, u8   byBufLen ) const; //获得会议号数组
	void CConfIdOld::CreateConfId( void );									//创建会议号
	void GetConfIdString( LPSTR lpszConfId, u8   byBufLen ) const;//获得会议号字符串
	void SetConfId( const CConfIdOld & cConfId );						//设置会议号
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

//定义传输地址结构(len:6)
struct TTransportAddrOld
{
public:
	u32   		m_dwIpAddr;		//IP地址, 网络序
	u16 		m_wPort;		//端口号, 网络序
public:
    void   SetNetSeqIpAddr(u32    dwIpAddr){ m_dwIpAddr = dwIpAddr; } 
    u32    GetNetSeqIpAddr( void ) const { return m_dwIpAddr; }
    void   SetIpAddr(u32    dwIpAddr){ m_dwIpAddr = htonl(dwIpAddr); } 
    u32    GetIpAddr( void ) const { return ntohl(m_dwIpAddr); }
    void   SetPort(u16  wPort){ m_wPort = htons(wPort); } 
    u16    GetPort( void ) const { return ntohs(m_wPort); }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//(len:22)
struct TMediaEncryptOld
{
protected:
	u8  m_byEncryptMode;    //加密模式:CONF_ENCRYPTMODE_NONE,CONF_ENCRYPTMODE_DES, CONF_ENCRYPT_AES
    s32 m_nKeyLen;          //加密key的长度
	u8  m_abyEncKey[MAXLEN_KEY]; //加密key
	u8  m_byReserve;//保留
public:
	TMediaEncryptOld()
	{
		Reset();
	}
	void Reset()
	{
		m_byEncryptMode = CONF_ENCRYPTMODE_NONE;
		m_nKeyLen = 0;
	}

	void SetEncryptMode(u8 byEncMode)
	{
		m_byEncryptMode = byEncMode;
	}
    u8  GetEncryptMode()
	{
		return m_byEncryptMode;
	}
	void SetEncryptKey(u8 *pbyKey, s32 nLen)
	{
		m_nKeyLen = (nLen > MAXLEN_KEY ? MAXLEN_KEY : nLen);
		if(m_nKeyLen > 0)
			memcpy(m_abyEncKey, pbyKey, m_nKeyLen); 
		m_nKeyLen = htonl(m_nKeyLen);
	}
	
	void GetEncryptKey(u8 *pbyKey, s32* pnLen)
	{
		if(pnLen != NULL) *pnLen = ntohl(m_nKeyLen);
		if(pbyKey != NULL) memcpy(pbyKey, m_abyEncKey, ntohl(m_nKeyLen));
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//记录录像或放像进度(单位:秒)
struct  TRecProgOld
{
protected:
	u32   		m_dwCurProg;	//当前进度
	u32   		m_dwTotalTime;	//总长度，仅在放像时有效

public:
	//设置当前进度位置（单位：秒）
	void SetCurProg( u32    dwCurProg )	{ m_dwCurProg = htonl( dwCurProg ); }
	//获取当前进度位置（单位：秒）
	u32    GetCurProg( void ) const	{ return ntohl( m_dwCurProg ); }
	//设置总长度，仅对放像有效（单位：秒）
	void SetTotalTime( u32    dwTotalTime )	{ m_dwTotalTime = htonl( dwTotalTime ); }
	//获取总长度，仅对放像有效（单位：秒）
	u32    GetTotalTime( void ) const	{ return ntohl( m_dwTotalTime ); }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//时间结构重新定义
struct TKdvTimeOld
{
protected:
	u16 		m_wYear;//年
	u8  		m_byMonth;//月
	u8  		m_byMDay;//日
	u8  		m_byHour;//时
	u8  		m_byMinute;//分
	u8  		m_bySecond;//秒

public:
	void SetTime( const time_t * ptTime );//设置时间
	void GetTime( time_t & tTime ) const;//得到时间结构
	BOOL operator == ( const TKdvTimeOld & tTime );//重载

	u16  GetYear( void ) const	{ return( ntohs( m_wYear ) ); }
	u8   GetMonth( void ) const	{ return( m_byMonth ); }
	u8   GetDay( void ) const	{ return( m_byMDay ); }
	u8   GetHour( void ) const	{ return( m_byHour ); }
	u8   GetMinute( void ) const	{ return( m_byMinute ); }
	u8   GetSecond( void ) const	{ return( m_bySecond ); }
	void SetYear( u16  wYear )	{ m_wYear = htons( wYear ); }
	void SetMonth( u8   byMonth )	{ m_byMonth = byMonth; }
	void SetDay( u8   byDay )	{ m_byMDay = byDay; }
	void SetHour( u8   byHour )	{ m_byHour = byHour; }
	void SetMinute( u8   byMinute )	{ m_byMinute = byMinute; }
	void SetSecond( u8   bySecond )	{ m_bySecond = bySecond; }
	void Print( void )
	{
		OspPrintf( TRUE, FALSE, "%d-%d-%d %d:%d:%d", ntohs(m_wYear),m_byMonth,m_byMDay,m_byHour,m_byMinute,m_bySecond );
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//轮询信息
struct TPollInfoOld
{
protected:
    u8  m_byMediaMode;           //轮询模式 MODE_VIDEO 图像  MODE_BOTH 语音图像
    u8  m_byPollState;           //轮询状态，参见轮询状态定义
    u32 m_dwPollNum;             //终端轮询的次数
    TMtPollParamOld m_tMtPollParam; //当前被轮询广播的终端及其参数
public:
    void SetMediaMode(u8 byMediaMode) { m_byMediaMode = byMediaMode; }
    u8   GetMediaMode(void) { return m_byMediaMode; }
    void SetPollState(u8 byPollState) { m_byPollState = byPollState; }
    u8   GetPollState(void) const { return m_byPollState; }
    void SetPollNum(u32 dwPollNum) { m_dwPollNum = htonl(dwPollNum); }
    u32  GetPollNum(void) const { return ntohl(m_dwPollNum); }
    void SetMtPollParam(TMtPollParamOld tMtPollParam){ m_tMtPollParam = tMtPollParam; }
    TMtPollParamOld GetMtPollParam() { return m_tMtPollParam; }
    void Print(void)
    {
        OspPrintf(TRUE, FALSE, "\nPollInfo:\n");
        OspPrintf(TRUE, FALSE, "m_byMediaMode: %d\n", m_byMediaMode);
        OspPrintf(TRUE, FALSE, "m_byPollState: %d\n", m_byPollState);
        OspPrintf(TRUE, FALSE, "m_dwPollNum: %d\n", m_dwPollNum);
        OspPrintf(TRUE, FALSE, "CurPollMtId: %d\n", m_tMtPollParam.GetMtId());
    }
}
#ifndef WIN32
__attribute__((packed))
#endif
;

//电视墙轮询信息
struct TTvWallPollInfoOld : public TPollInfoOld
{
protected:
    TEqpOld    m_tTvWall;
    u8      m_byTWChnnl;
    
public:
    void SetTvWall(TEqpOld tTvWall) { m_tTvWall = tTvWall; }
    TEqpOld GetTvWall(void) { return m_tTvWall; } 
    void SetTWChnnl(u8 byChnnl) { m_byTWChnnl = byChnnl; }
    u8   GetTWChnnl(void) { return m_byTWChnnl; }
}
#ifndef WIN32
__attribute__((packed))
#endif
;

//定义终端别名结构(len:135)
struct TMtAliasOld
{
public:
    u8			m_AliasType;               //别名类型
    s8			m_achAlias[MAXLEN_ALIAS];  //别名字符串
    TTransportAddrOld	m_tTransportAddr;          //传输地址
public:
	TMtAliasOld( void ){ memset( this, 0, sizeof(TMtAliasOld) ); }
	BOOL operator ==( const TMtAliasOld & tObj ) const;
	BOOL IsNull( void ){ if( m_AliasType == 0 )return TRUE; return FALSE; }
	void SetNull( void ){ memset(this, 0, sizeof(TMtAliasOld)); }
	void SetE164Alias( LPCSTR lpzAlias )
	{
		if( lpzAlias != NULL )
		{
			m_AliasType = mtAliasTypeE164;
			memcpy( m_achAlias, lpzAlias, MAXLEN_ALIAS );
			m_achAlias[MAXLEN_ALIAS-1] = '\0';
		}
	}
	void SetH323Alias( LPCSTR lpzAlias )
	{
		if( lpzAlias != NULL )
		{
			m_AliasType = mtAliasTypeH323ID;
			memcpy( m_achAlias, lpzAlias, MAXLEN_ALIAS );
			m_achAlias[MAXLEN_ALIAS-1] = '\0';
		}
	}
	void SetH320Alias( u8 byLayer, u8 bySlot, u8 byChannel )
	{
		m_AliasType = mtAliasTypeH320ID;
		sprintf( m_achAlias, "层%d-槽%d-通道%d%c", byLayer, bySlot, byChannel, 0 );
	}
	BOOL GetH320Alias( u8 &byLayer, u8 &bySlot, u8 &byChannel )
	{
		BOOL bRet = FALSE;
		byLayer   = 0;
		bySlot    = 0; 
		byChannel = 0; 
		if( mtAliasTypeH320ID != m_AliasType )
		{
			return bRet;
		}

		s8 *pachLayerPrefix   = "层";
		s8 *pachSlotPrefix    = "-槽";
		s8 *pachChannelPrefix = "-通道";
		s8 *pachAlias = m_achAlias;

		if( 0 == memcmp( pachAlias, pachLayerPrefix, strlen("层") ) )
		{
			s32  nPos = 0;
			s32  nMaxCHLen = 3; //一字节的最大字串长度
			s8 achLayer[4] = {0};

			//提取层号
			pachAlias += strlen("层");
			for( nPos = 0; nPos < nMaxCHLen; nPos++ )
			{
				if( '-' == pachAlias[nPos] )
				{
					break;
				}
				achLayer[nPos] = pachAlias[nPos];
			}
			if( 0 == nPos || nPos >= nMaxCHLen )
			{
				return bRet;
			}
			achLayer[nPos+1] = '\0';
			byLayer = atoi(achLayer);

			//提取槽号
			pachAlias += nPos;
			if( 0 == memcmp( pachAlias, pachSlotPrefix, strlen("-槽") ) )
			{
				pachAlias += strlen("-槽");
				for( nPos = 0; nPos < nMaxCHLen; nPos++ )
				{
					if( '-' == pachAlias[nPos] )
					{
						break;
					}
					achLayer[nPos] = pachAlias[nPos];
				}
				if( 0 == nPos || nPos >= nMaxCHLen )
				{
					return bRet;
				}
				achLayer[nPos+1] = '\0';
				bySlot = atoi(achLayer);
			
				//提取通道号
				pachAlias += nPos;
				if( 0 == memcmp( pachAlias, pachChannelPrefix, strlen("-通道") ) )
				{
					pachAlias += strlen("-通道");
					memcpy( achLayer, pachAlias, nMaxCHLen );
					achLayer[nMaxCHLen] = '\0';
					byChannel = atoi(achLayer);
					bRet = TRUE;
				}
			}
		}
		return bRet;
	}
	void Print( void )
	{
		if( m_AliasType == 0 )
		{
			OspPrintf( TRUE, FALSE, "null aliase!" );
		}
		else if( m_AliasType == mtAliasTypeTransportAddress )
		{
			OspPrintf( TRUE, FALSE, "IP:0x%x, port:%d", 
				     m_tTransportAddr.GetIpAddr(), m_tTransportAddr.GetPort() );
		}		
		else if( m_AliasType == mtAliasTypeE164 )
		{
			OspPrintf( TRUE, FALSE, "E164: %s", m_achAlias ); 
		}
		else if( m_AliasType == mtAliasTypeH323ID )
		{
			OspPrintf( TRUE, FALSE, "H323ID: %s", m_achAlias ); 
		}
		else if( m_AliasType == mtAliasTypeH320ID )
		{
			OspPrintf( TRUE, FALSE, "H320ID: %s", m_achAlias ); 
		}
        else if( m_AliasType == mtAliasTypeH320Alias )
        {
            OspPrintf( TRUE, FALSE, "H320Alias: %s", m_achAlias );
        }
		else
		{
			OspPrintf( TRUE, FALSE, "Other type aliase!" ); 
		}
		OspPrintf( TRUE, FALSE, "\n" );
	}
} 
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


//定义讨论参数结构
struct TDiscussParamOld
{
public:
    u8  m_byMixMode;   //混音模式：mcuNoMix - 不混音
                       //          mcuWholeMix - 全体混音
                       //          mcuPartMix - 定制混音
    u8  m_byMemberNum; //参加讨论的成员数量(混音深度)
    TMtOld m_atMtMember[MAXNUM_MIXER_DEPTH];  //讨论成员
public:
    TDiscussParamOld(void)
    { 
        memset(this, 0, sizeof(TDiscussParamOld));
    }
    BOOL32 IsMtInMember(TMtOld tMt)
    {
        u8 byLoop = 0;
        u8 byLoopCtrl = 0;
        while (byLoop < m_byMemberNum && byLoopCtrl < MAXNUM_MIXER_DEPTH)
        {
            if (tMt == m_atMtMember[byLoop])
            {
                return TRUE;
            }

            byLoop++;
            byLoopCtrl++;
        }

        return FALSE;
    }
}
#ifndef WIN32
__attribute__ ((packed))
#endif
;

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
	void   Print( void )
	{
		OspPrintf( TRUE, FALSE, "\nConfAttrb:\n" );
		OspPrintf( TRUE, FALSE, "m_byOpenMode: %d\n", m_byOpenMode);
		OspPrintf( TRUE, FALSE, "m_byEncryptMode: %d\n", m_byEncryptMode);
		OspPrintf( TRUE, FALSE, "m_byMulticastMode: %d\n", m_byMulticastMode);
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

/*电视墙模板结构和画面合成模板结构说明:
目前创会时的消息体: TConfInfoOld + 2 byte(TMtAliasOld数组打包长度,即下面的n)
                  + n byte(TMtAliasOld数组打包) + TMultiTvWallModuleOld(可选) + TVmpModuleOld(可选) */
//电视墙模板结构
struct TTvWallModuleOld
{
	TEqpOld m_tTvWall;                                     //会控指定的电视墙
    u8   m_abyTvWallMember[MAXNUM_PERIEQP_CHNNL_OLD];       //会控指定的电视墙成员索引（即创会时TMtAliasOld数组索引+1）
  
public:
    TTvWallModuleOld(void) { memset(this, 0, sizeof(TTvWallModuleOld)); }

    TEqpOld GetTvEqp(void)
    {
        return m_tTvWall;
    }
    
    void SetTvEqp(TEqpOld tTvWall)
    {
        m_tTvWall = tTvWall;
    }
    
    void EmptyTvMember(void)
    {
        memset((void*)m_abyTvWallMember, 0, sizeof(m_abyTvWallMember));
    }

    void SetTvChannel(u8 byTvChl, u8 byMtIdx) // 设置指定通道
    {
        if(byTvChl < MAXNUM_PERIEQP_CHNNL_OLD)
        {
            m_abyTvWallMember[byTvChl] = byMtIdx;
        }
    }
    
    void RemoveTvMember(u8 byTvChl) // 移除电视墙指定通道
    {
        if(byTvChl < MAXNUM_PERIEQP_CHNNL_OLD)
        {
            m_abyTvWallMember[byTvChl] = 0;
        }
    }
    
    void RemoveMtInTvMember(u8 byMtIdx) // 移除电视墙中所有该终端
    {
        for(u8 byTvChlLp = 0; byTvChlLp < MAXNUM_PERIEQP_CHNNL_OLD; byTvChlLp++)
        {
            if(m_abyTvWallMember[byTvChlLp] == byMtIdx)
            {
                m_abyTvWallMember[byTvChlLp] = 0;
            }
        }
    }

    void Clear(void)
    {
        memset(&m_tTvWall, 0, sizeof(m_tTvWall));
        memset(m_abyTvWallMember, 0, sizeof(m_abyTvWallMember) );
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// 会议模板中支持多电视墙结构
struct TMultiTvWallModuleOld
{
    u8 m_byTvModuleNum; // 电视墙数目
    TTvWallModuleOld  m_atTvWallModule[MAXNUM_PERIEQP_CHNNL_OLD];

public:

    TMultiTvWallModuleOld(void){ memset(this, 0, sizeof(TMultiTvWallModuleOld));}

    void SetTvModuleNum( u8 byTvModuleNum ) { m_byTvModuleNum = (byTvModuleNum <= MAXNUM_PERIEQP_CHNNL_OLD) ? byTvModuleNum : m_byTvModuleNum;}
    u8     GetTvModuleNum( void ) { return m_byTvModuleNum; }
    BOOL32 SetTvModule(u8 byTvModueIdx, TTvWallModuleOld m_tTvWallModule)
    {
        BOOL32 bRet = TRUE;
        if( byTvModueIdx < MAXNUM_PERIEQP_CHNNL_OLD )
        {
            m_atTvWallModule[byTvModueIdx] = m_tTvWallModule;
        }
        else
        {
            bRet = FALSE;
        }
        return bRet;
    }

    BOOL32  GetTvModuleByIdx(u8 byTvIdx, TTvWallModuleOld& tTvWallModule) // 按序号取电视墙模板
    {
        BOOL32 bRet = TRUE;
        if( 0 != m_byTvModuleNum && byTvIdx < MAXNUM_PERIEQP_CHNNL_OLD)
        {
            tTvWallModule = m_atTvWallModule[byTvIdx];
        }
        else
        {
            bRet = FALSE;
        }
        return bRet;
    }

    BOOL32  GetTvModuleByTvId(u8 byTvId, TTvWallModuleOld& tTvWallModule)
    {
        BOOL32 bRet = TRUE;
        if( 0 != m_byTvModuleNum )
        {
            for(u8 byTvLp = 0; byTvLp < MAXNUM_PERIEQP_CHNNL_OLD; byTvLp++)
            {
                if( byTvId == m_atTvWallModule[byTvLp].m_tTvWall.GetEqpId() )
                {
                    tTvWallModule = m_atTvWallModule[byTvLp];
                    break;
                }
            }
        }
        else
        {
            bRet = FALSE;
        }
        return bRet;
    }

    void SetTvWallMember(u8 byTvId, u8 byTvChl, u8 byMtIdx)
    {
        for(u8 byTvLp = 0; byTvLp < MAXNUM_PERIEQP_CHNNL_OLD; byTvLp++)
        {
            if(m_atTvWallModule[byTvLp].GetTvEqp().GetEqpId() == byTvId)
            {
                m_atTvWallModule[byTvLp].SetTvChannel(byTvChl, byMtIdx);
                break;
            }
        }
    }

    void RemoveMtByTvId(u8 byTvId, u8 byMtIdx )
    {
        for(u8 byTvLp = 0; byTvLp < MAXNUM_PERIEQP_CHNNL_OLD; byTvLp++)
        {
            if(m_atTvWallModule[byTvLp].GetTvEqp().GetEqpId() == byTvId)
            {
                m_atTvWallModule[byTvLp].RemoveMtInTvMember(byMtIdx);
                break;
            }
        }
    }

    void Clear(void)
    {
        m_byTvModuleNum = 0;
        memset(m_atTvWallModule, 0, sizeof(m_atTvWallModule) );
    }

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//画面合成模板结构 
struct TVmpModuleOld
{                        
	TVMPParamOld m_tVMPParam;                              //会控指定的画面合成参数
	u8        m_abyVmpMember[MAXNUM_VMP_MEMBER];        //会控指定的画面合成成员索引（即创会时TMtAliasOld数组索引+1）

    TVmpModuleOld(void) { memset(this, 0, sizeof(TVmpModuleOld)); }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


//定义会议方式结构(会议中会改变)
struct TConfModeOld
{
protected:
	u8     m_byTakeMode;        //会议举行方式: 0-预约 1-即时 2-会议模板 
	u8     m_byLockMode;        //会议锁定方式: 0-不锁定,所有会控可见可操作 1-根据密码操作此会议 2-某个会议控制台锁定
    u8     m_byCallMode;        //呼叫终端方式: 0-不呼叫终端，手动呼叫 1-呼叫一次 2-定时呼叫未与会终端
	u8     m_byVACMode;         //语音激励方式: 0-不启用语音激励控制发言(导演控制发言) 1-启用语音激励控制发言
	u8     m_byAudioMixMode;    //混音方式:     0-不混音 1-智能混音
	u8     m_byRecordMode;      //会议录像方式: (BIT:0-6)0-不录像 1-录像 2-录像暂停; BIT7 = 0 实时 1抽帧录像
	u8     m_byPlayMode;        //会议放像方式: 0-不放像 1-放像 2-放像暂停 3-快进 4-快退
	u8     m_byBasMode;         //码率适配方式: BIT0-是否RTP头适配 BIT1-是否码率适配 BIT2-是否类型适配
	u8     m_byPollMode;        //会议轮询方式: 0-不轮询广播 1-仅图像轮询广播 2-轮流发言
	u8     m_byDiscussMode;     //会议讨论模式: 0-非讨论模式(演讲模式) 1-讨论模式
	u8     m_byForceRcvSpeaker; //强制收看发言: 0-不强制收看发言人 1-强制收看发言人
	u8     m_byNoChairMode;     //会议主席模式: 0-不是无主席方式 1-无主席方式
	u8     m_byRegToGK;         //会议注册GK情况: 0-会议未在GK上注册 1-会议在GK上成功注册
	u8     m_byMixSpecMt;		//是否指定终端混音: 0-不是指定终端混音(所有终端参加混音) 1-指定终端混音
	u8	   m_byReserved1;		//保留字段
	u8	   m_byReserved2;		//保留字段
	u32    m_dwCallInterval;    //定时呼叫时的呼叫间隔：(单位：秒，最小为5秒)（网络序）
	u32    m_dwCallTimes;       //定时呼叫时的呼叫次数（网络序）
public:
	TConfModeOld( void ){ memset( this, 0, sizeof(TConfModeOld) ); }
    void   SetTakeMode(u8 byTakeMode){ m_byTakeMode = byTakeMode;} 
    u8     GetTakeMode( void ) const { return m_byTakeMode; }
    void   SetLockMode(u8   byLockMode){ m_byLockMode = byLockMode;} 
    u8     GetLockMode( void ) const { return m_byLockMode; }
    void   SetCallMode(u8   byCallMode){ m_byCallMode = byCallMode;} 
    u8     GetCallMode( void ) const { return m_byCallMode; }
	void   SetCallInterval(u32 dwCallInterval){ m_dwCallInterval = htonl(dwCallInterval);} 
    u32    GetCallInterval( void ) const { return ntohl(m_dwCallInterval); }
	void   SetCallTimes(u32 dwCallTimes){ m_dwCallTimes = htonl(dwCallTimes);} 
    u32    GetCallTimes( void ) const { return ntohl(m_dwCallTimes); }
    void   SetVACMode(BOOL bVACMode){ m_byVACMode = bVACMode;} 
    BOOL   IsVACMode( void ) const { return m_byVACMode == 0 ? FALSE : TRUE; }
    void   SetAudioMixMode(BOOL bAudioMixMode){ m_byAudioMixMode = bAudioMixMode;} 
    BOOL   IsAudioMixMode( void ) const { return m_byAudioMixMode == 0 ? FALSE : TRUE; }
	void   SetRegToGK( BOOL bRegToGK ){ m_byRegToGK = bRegToGK; }
	BOOL   IsRegToGK( void ){ return m_byRegToGK == 0 ? FALSE : TRUE; }
    void   SetRecordMode(u8   byRecordMode)
	{   byRecordMode   &= 0x7f ;
		m_byRecordMode &= 0x80 ;
		m_byRecordMode |= byRecordMode;
	} 
    u8     GetRecordMode( void ) const { return m_byRecordMode&0x7f; }
	
	//判断当前是否在抽帧录像，若是返回TRUE否则返回FALSE
	BOOL   IsRecSkipFrame(){ return m_byRecordMode&0x80 ? TRUE:FALSE;}
	//bSkipFrame = TRUE 设置当前为抽帧录像,否则实时录像
	void   SetRecSkipFrame(BOOL bSkipFrame)
	{
		if(bSkipFrame)
			m_byRecordMode |=0x80;
		else 
			m_byRecordMode &=0x7f;
	}
    void   SetPlayMode( u8  byPlayMode ){ m_byPlayMode = byPlayMode;} 
    u8     GetPlayMode( void ) const { return m_byPlayMode; }
    void   SetBasMode( u8  byBasMode, BOOL32 bHasBasMode )
	{ 
		if( bHasBasMode )
		{
			m_byBasMode |= (1<<(byBasMode-1)) ;
		}
		else
		{
			m_byBasMode &= ~(1<<(byBasMode-1));
		}
	} 
    BOOL   GetBasMode( u8  byBasMode ) const 
	{ 
		return m_byBasMode & (1<<(byBasMode-1));
	}
    void   SetPollMode(u8   byPollMode){ m_byPollMode = byPollMode;} 
    u8     GetPollMode( void ) const { return m_byPollMode; }
    void   SetDiscussMode(u8   byDiscussMode){ m_byDiscussMode = byDiscussMode;} 
    BOOL   IsDiscussMode( void ) const { return m_byDiscussMode == 0 ? FALSE : TRUE; }  
    void   SetForceRcvSpeaker(u8   byForceRcvSpeaker){ m_byForceRcvSpeaker = byForceRcvSpeaker;} 
    BOOL   IsForceRcvSpeaker( void ) const { return m_byForceRcvSpeaker == 0 ? FALSE : TRUE; }
    void   SetNoChairMode(u8   byNoChairMode){ m_byNoChairMode = byNoChairMode;} 
    BOOL   IsNoChairMode( void ) const { return m_byNoChairMode == 0 ? FALSE : TRUE; }
    void   SetMixSpecMt( BOOL bMixSpecMt ){ m_byMixSpecMt = bMixSpecMt;} 
    BOOL   IsMixSpecMt( void ) const { return m_byMixSpecMt == 0 ? FALSE : TRUE; }
	void   Print( void )
	{
		OspPrintf( TRUE, FALSE, "\nConfMode:\n" );
		OspPrintf( TRUE, FALSE, "m_byTakeMode: %d\n", m_byTakeMode);
		OspPrintf( TRUE, FALSE, "m_byLockMode: %d\n", m_byLockMode);
		OspPrintf( TRUE, FALSE, "m_byCallMode: %d\n", m_byCallMode);
		OspPrintf( TRUE, FALSE, "m_dwCallInterval: %d\n", GetCallInterval());
		OspPrintf( TRUE, FALSE, "m_dwCallTimes: %d\n", GetCallInterval());
		OspPrintf( TRUE, FALSE, "m_byVACMode: %d\n", m_byVACMode);
		OspPrintf( TRUE, FALSE, "m_byAudioMixMode: %d\n", m_byAudioMixMode);
		OspPrintf( TRUE, FALSE, "m_byRecordMode: %d\n", m_byRecordMode);
		OspPrintf( TRUE, FALSE, "m_byPlayMode: %d\n", m_byPlayMode);
		OspPrintf( TRUE, FALSE, "m_byBasMode: %d\n", m_byBasMode);
		OspPrintf( TRUE, FALSE, "m_byPollMode: %d\n", m_byPollMode);
		OspPrintf( TRUE, FALSE, "m_byDiscussMode: %d\n", m_byDiscussMode);
		OspPrintf( TRUE, FALSE, "m_byForceRcvSpeaker: %d\n", m_byForceRcvSpeaker);
		OspPrintf( TRUE, FALSE, "m_byNoChairMode: %d\n", m_byNoChairMode);
		OspPrintf( TRUE, FALSE, "m_byRegToGK: %d\n", m_byRegToGK);
		OspPrintf( TRUE, FALSE, "m_byMixSpecMt: %d\n", m_byMixSpecMt);
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//定义会议状态结构
struct TConfStatusOld
{
public:
    TMtOld 	      m_tChairman;	      //主席终端，MCU号为0表示无主席
    TMtOld		      m_tSpeaker;		  //发言终端，MCU号为0表示无发言人
    TConfModeOld     m_tConfMode;        //会议方式
    TRecProgOld	  m_tRecProg;		  //当前录像进程,仅录像状态时有效
    TRecProgOld	  m_tPlayProg;	      //当前放像进程,仅放像状态时有效
    TPollInfoOld     m_tPollInfo;        //会议轮询参数,仅轮询时有较
    TTvWallPollInfoOld     m_tTvWallPollInfo;  //电视墙当前轮询参数，仅轮询时有效
    TDiscussParamOld m_tDiscussParam;    //当前混音参数，仅混音时有效
    TVMPParamOld     m_tVMPParam;        //当前视频复合参数，仅视频复合时有效
    TVMPParamOld     m_tVmpTwParam;
    u8            m_byPrs;            //当前包重传状态

public:
    BOOL   HasChairman(void) const { return m_tChairman.GetMtId()==0 ? FALSE : TRUE; }	
	TMtOld    GetChairman(void) const { return m_tChairman; }
	void   SetNoChairman(void) { memset( &m_tChairman, 0, sizeof( TMtOld ) ); }
	void   SetChairman(TMtOld tChairman) { m_tChairman = tChairman; }
	BOOL   HasSpeaker(void) const { return m_tSpeaker.GetMtId()==0 ? FALSE : TRUE; }
    TMtOld    GetSpeaker(void) const { return m_tSpeaker; }
	void   SetNoSpeaker(void) { memset( &m_tSpeaker, 0, sizeof( TMtOld ) ); }
	void   SetSpeaker(TMtOld tSpeaker) { m_tSpeaker = tSpeaker; }
    void          SetConfMode(TConfModeOld tConfMode) { m_tConfMode = tConfMode; } 
    TConfModeOld     GetConfMode(void) const { return m_tConfMode; }
    void          SetRecProg(TRecProgOld tRecProg){ m_tRecProg = tRecProg; } 
    TRecProgOld      GetRecProg(void) const { return m_tRecProg; }
    void          SetPlayProg(TRecProgOld tPlayProg){ m_tPlayProg = tPlayProg; } 
    TRecProgOld      GetPlayProg(void) const { return m_tPlayProg; }
    void          SetVmpParam(TVMPParamOld tVMPParam){ m_tVMPParam = tVMPParam;} 
    TVMPParamOld     GetVmpParam(void) const { return m_tVMPParam; }

	//会议进行状态
	BOOL IsScheduled( void ) const { return m_tConfMode.GetTakeMode() == CONF_TAKEMODE_SCHEDULED ? TRUE : FALSE; }
	void SetScheduled( void ){ m_tConfMode.SetTakeMode( CONF_TAKEMODE_SCHEDULED ); }
	BOOL IsOngoing( void ) const { return m_tConfMode.GetTakeMode() == CONF_TAKEMODE_ONGOING ? TRUE : FALSE; }
	void SetOngoing( void ){ m_tConfMode.SetTakeMode( CONF_TAKEMODE_ONGOING ); }
	BOOL IsTemplate( void ) const { return m_tConfMode.GetTakeMode() == CONF_TAKEMODE_TEMPLATE ? TRUE : FALSE; }
	void SetTemplate( void ){ m_tConfMode.SetTakeMode( CONF_TAKEMODE_TEMPLATE ); }
	u8   GetTakeMode( void ){ return m_tConfMode.GetTakeMode();	}
	void SetTakeMode( u8 byTakeMode ){ m_tConfMode.SetTakeMode( byTakeMode ); }

	//会议保护状态
	u8   GetProtectMode( void ) const { return m_tConfMode.GetLockMode(); }
    void SetProtectMode( u8   byProtectMode ) { m_tConfMode.SetLockMode( byProtectMode ); }
	
	//会议呼叫策略
	u8   GetCallMode( void ) const { return m_tConfMode.GetCallMode(); }
	void SetCallMode( u8   byCallMode ){ m_tConfMode.SetCallMode( byCallMode ); }
	void SetCallInterval(u32 dwCallInterval){ m_tConfMode.SetCallInterval( dwCallInterval ); } 
    u32  GetCallInterval( void ) const { return m_tConfMode.GetCallInterval( ); }
	void SetCallTimes(u32 dwCallTimes){ m_tConfMode.SetCallTimes( dwCallTimes ); } 
    u32  GetCallTimes( void ) const { return m_tConfMode.GetCallTimes( ); }

    //会议混音状态
    BOOL32 IsNoMixing(void) const { return !m_tConfMode.IsAudioMixMode(); }
    void   SetNoMixing(void) { m_tConfMode.SetAudioMixMode(0); }
    BOOL32 IsMixing(void) const { return m_tConfMode.IsAudioMixMode(); }
    void   SetMixing(void) { m_tConfMode.SetAudioMixMode(1); }

    //混音方式 
    BOOL32 IsMixSpecMt(void) const { return m_tConfMode.IsMixSpecMt(); }
    void   SetMixSpecMt(BOOL bMixSpecMt) { m_tConfMode.SetMixSpecMt(bMixSpecMt); }

    //会议讨论方式
    BOOL32 IsDiscussMode(void) const { return m_tConfMode.IsDiscussMode(); }
    void   SetDiscussMode(BOOL bDiscussMode = TRUE) { m_tConfMode.SetDiscussMode(bDiscussMode); }

    //语音激励方式
    BOOL32 IsVACMode(void) const { return m_tConfMode.IsVACMode(); }
    void   SetVACMode(BOOL bVACMode = TRUE) { m_tConfMode.SetVACMode(bVACMode); }

    //讨论参数
    BOOL32 GetMixModeParam(void) const { return m_tDiscussParam.m_byMixMode; }
    void   SetMixModeParam(u8 byMixMode) { m_tDiscussParam.m_byMixMode = byMixMode; }
    void   SetDiscussMember(TDiscussParamOld tAudMixParam)
    { memcpy(m_tDiscussParam.m_atMtMember, tAudMixParam.m_atMtMember, sizeof(tAudMixParam.m_atMtMember)); } 
    void   SetDiscussParam(TDiscussParamOld tAudMixParam){ m_tDiscussParam = tAudMixParam; } 
    TDiscussParamOld GetDiscussParam(void) const { return m_tDiscussParam; }

    //画面合成方式
	void   SetVMPMode(u8   byVMPMode){ m_tVMPParam.SetVMPMode(byVMPMode); } 
    u8     GetVMPMode( void ) const { return m_tVMPParam.GetVMPMode(); }
	BOOL   IsBrdstVMP( void ) const { return m_tVMPParam.GetVMPMode() != CONF_VMPMODE_NONE && m_tVMPParam.IsVMPBrdst(); }
	void   SetVmpBrdst( BOOL bBrdst ){ m_tVMPParam.SetVMPBrdst( bBrdst ); }
	u8     GetVmpStyle( void ){ return m_tVMPParam.GetVMPStyle(); }
	void   SetVmpStyle( u8 byVMPStyle ){ m_tVMPParam.SetVMPStyle( byVMPStyle ); }

    //vmp tvwall    
	void      SetVmpTwMode(u8 byVmpTwMode){ m_tVmpTwParam.SetVMPMode(byVmpTwMode); }
    u8        GetVmpTwMode(void) const { return m_tVmpTwParam.GetVMPMode(); }
    void      SetVmpTwParam(TVMPParamOld tVmpTwParam) { m_tVmpTwParam = tVmpTwParam; }
    TVMPParamOld GetVmpTwParam(void) const { return m_tVmpTwParam; }
    u8        GetVmpTwStyle(void) { return m_tVmpTwParam.GetVMPStyle(); }
	void      SetVmpTwStyle(u8 byVmpTwStyle) { m_tVmpTwParam.SetVMPStyle(byVmpTwStyle); }

	//强制广播(跟原来强制发言人的概念不同，接口名称暂时不修改)
	BOOL IsMustSeeSpeaker(void) const { return m_tConfMode.IsForceRcvSpeaker(); }
	void SetMustSeeSpeaker(BOOL bMustSeeSpeaker) { m_tConfMode.SetForceRcvSpeaker( bMustSeeSpeaker ); }

	//码率适配状态 -- modify bas 2
    BOOL32 IsAudAdapting(void) const { return m_tConfMode.GetBasMode(CONF_BASMODE_AUD); }
    BOOL32 IsVidAdapting(void) const { return m_tConfMode.GetBasMode(CONF_BASMODE_VID); }
    BOOL32 IsBrAdapting(void) const { return m_tConfMode.GetBasMode(CONF_BASMODE_BR); }
    BOOL32 IsCasdAudAdapting(void) const { return m_tConfMode.GetBasMode(CONF_BASMODE_CASDAUD); }
    BOOL32 IsCasdVidAdapting(void) const { return m_tConfMode.GetBasMode(CONF_BASMODE_CASDVID); }
    void SetAdaptMode(u8 byBasMode, BOOL32 bAdapt)
    { 
        m_tConfMode.SetBasMode(byBasMode, bAdapt);
    }

	//会议主席状态
	BOOL IsNoChairMode( void ) const { return m_tConfMode.IsNoChairMode(); }
    void SetNoChairMode( BOOL bNoChairMode ) { m_tConfMode.SetNoChairMode( bNoChairMode ); }

	//轮询方式
	void   SetPollMode(u8   byPollMode){ m_tConfMode.SetPollMode( byPollMode ); } 
    u8     GetPollMode(){ return m_tConfMode.GetPollMode(); }
	
	//轮询参数
    void   SetPollInfo(TPollInfoOld tPollInfo){ m_tPollInfo = tPollInfo;} 
    TPollInfoOld  *GetPollInfo( void ) { return &m_tPollInfo; }
	void   SetPollMedia(u8   byMediaMode){ m_tPollInfo.SetMediaMode( byMediaMode ); } 
    u8     GetPollMedia( void ){ return m_tPollInfo.GetMediaMode(); }
	void   SetPollState(u8   byPollState){ m_tPollInfo.SetPollState( byPollState );} 
    u8     GetPollState( void ) const { return m_tPollInfo.GetPollState(); }
    void   SetMtPollParam(TMtPollParamOld tMtPollParam){ m_tPollInfo.SetMtPollParam( tMtPollParam ); } 
    TMtPollParamOld  GetMtPollParam(){ return m_tPollInfo.GetMtPollParam(); }

    //电视墙轮询参数
    void   SetTvWallPollInfo(TTvWallPollInfoOld tPollInfo){ m_tTvWallPollInfo = tPollInfo;} 
    TTvWallPollInfoOld  *GetTvWallPollInfo( void ) { return &m_tTvWallPollInfo; }    
    void   SetTvWallPollState(u8   byPollState){ m_tTvWallPollInfo.SetPollState( byPollState );} 
    u8     GetTvWallPollState( void ) const { return m_tTvWallPollInfo.GetPollState(); }
    void   SetTvWallMtPollParam(TMtPollParamOld tMtPollParam){ m_tTvWallPollInfo.SetMtPollParam( tMtPollParam ); } 
    TMtPollParamOld  GetTvWallMtPollParam(){ return m_tTvWallPollInfo.GetMtPollParam(); }

	//会议录像状态	
	BOOL IsNoRecording( void ) const{ return m_tConfMode.GetRecordMode() == CONF_RECMODE_NONE ? TRUE : FALSE; }
	void SetNoRecording( void ){ m_tConfMode.SetRecordMode( CONF_RECMODE_NONE ); }
	BOOL IsRecording( void ) const{ return m_tConfMode.GetRecordMode() == CONF_RECMODE_REC ? TRUE : FALSE; }
	void SetRecording( void ){ m_tConfMode.SetRecordMode( CONF_RECMODE_REC ); }
	BOOL IsRecPause( void ) const{ return m_tConfMode.GetRecordMode() == CONF_RECMODE_PAUSE ? TRUE : FALSE; }
	void SetRecPause( void ){ m_tConfMode.SetRecordMode( CONF_RECMODE_PAUSE ); }

	//会议放像状态
	BOOL IsNoPlaying( void ) const{ return m_tConfMode.GetPlayMode() == CONF_PLAYMODE_NONE ? TRUE : FALSE; }
	void SetNoPlaying( void ){ m_tConfMode.SetPlayMode( CONF_PLAYMODE_NONE ); }
	BOOL IsPlaying( void ) const { return m_tConfMode.GetPlayMode() == CONF_PLAYMODE_PLAY ? TRUE : FALSE; }
	void SetPlaying( void ){ m_tConfMode.SetPlayMode( CONF_PLAYMODE_PLAY ); }
	BOOL IsPlayPause( void )const { return m_tConfMode.GetPlayMode() == CONF_PLAYMODE_PAUSE ? TRUE : FALSE; }
	void SetPlayPause( void ){ m_tConfMode.SetPlayMode( CONF_PLAYMODE_PAUSE ); }
	BOOL IsPlayFF( void )const { return m_tConfMode.GetPlayMode() == CONF_PLAYMODE_FF ? TRUE : FALSE; }
	void SetPlayFF( void ) { m_tConfMode.SetPlayMode( CONF_PLAYMODE_FF ); }
	BOOL IsPlayFB( void )const { return m_tConfMode.GetPlayMode() == CONF_PLAYMODE_FB ? TRUE : FALSE; }
	void SetPlayFB( void ){ m_tConfMode.SetPlayMode( CONF_PLAYMODE_FB ); }

	//包重传状态
	void SetPrsing( BOOL bPrs ){ m_byPrs = bPrs; }
	BOOL IsPrsing( void ){ return m_byPrs; }

	//注册GK情况
	void SetRegToGK( BOOL bRegToGK ){ m_tConfMode.SetRegToGK( bRegToGK ); }
	BOOL IsRegToGK( void ){ return m_tConfMode.IsRegToGK(); }

	void Print( void )
	{
		OspPrintf( TRUE, FALSE, "\nConfStatus:\n" );
        if(m_tChairman.IsNull())  
            OspPrintf( TRUE, FALSE, "Chairman: \n" );
        else 
            OspPrintf( TRUE, FALSE, "Chairman: Mcu%dMt%d\n", m_tChairman.GetMcuId(), m_tChairman.GetMtId() );
        if(m_tSpeaker.IsNull())
            OspPrintf( TRUE, FALSE, "Speaker: \n" );
        else
		    OspPrintf( TRUE, FALSE, "Speaker: Mcu%dMt%d\n", m_tSpeaker.GetMcuId(), m_tSpeaker.GetMtId() );
        OspPrintf( TRUE, FALSE, "VmpMode: %d\n", m_tVMPParam.GetVMPMode() );
        OspPrintf( TRUE, FALSE, "TWVmpMode: %d\n", m_tVmpTwParam.GetVMPMode() );
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//定义视频媒体格式的属性(len:5)
struct TVideoStreamCapOld
{
protected:
    u16		m_wMaxBitRate;   
    u8		m_byMediaType;   
    u8      m_byResolution;	//分辨率 VIDEO_FORMAT_AUTO, ...
	u8      m_byFrameRate;  //帧率 MPI
    
public:
    TVideoStreamCapOld() { Clear(); }
    
    void    Clear(void)
    {
        m_wMaxBitRate = 0;
        m_byFrameRate = 0;
        m_byMediaType = MEDIA_TYPE_NULL;
        m_byResolution = VIDEO_FORMAT_CIF;
    }   
    
    void	SetMediaType(u8 byMediaType)  {  m_byMediaType = byMediaType; }	
    u8		GetMediaType() const { return m_byMediaType; }
    
    void    SetResolution(u8 byRes) { m_byResolution = byRes; }
    u8      GetResolution(void) const { return m_byResolution; }

    void    SetFrameRate(u8 byFrameRate) { m_byFrameRate = byFrameRate; }
    u8      GetFrameRate(void) const { 	return m_byFrameRate;	}
    
    void	SetMaxBitRate(u16 wMaxRate) {m_wMaxBitRate = htons(wMaxRate); }
    u16		GetMaxBitRate() const { return ntohs(m_wMaxBitRate); }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//双流能力类型(len:6)
struct TDStreamCapOld : public TVideoStreamCapOld
{
protected:
    u8  m_byIsH239Type;    

public:
    TDStreamCapOld() { Reset(); }

    void SetSupportH239(BOOL32 bSupport) { m_byIsH239Type = (bSupport ? 1:0); }
    BOOL32   IsSupportH239(void) { return (0 != m_byIsH239Type); }

    void Reset(void)
    {
        Clear();
        m_byIsH239Type = 0;
    }
};

//同时能力集结构(len:6)
struct TSimCapSetOld
{
protected:
	TVideoStreamCapOld  m_tVideoCap;
	u8  m_byAudioMediaType;  

public:
    TSimCapSetOld( void )	{ Clear(); }

	void SetVideoMediaType( u8 byMediaType ) { m_tVideoCap.SetMediaType(byMediaType); }
    u8   GetVideoMediaType( void ) const { return m_tVideoCap.GetMediaType(); }

	void SetVideoResolution( u8 byRes ) { m_tVideoCap.SetResolution(byRes); }
    u8   GetVideoResolution( void ) const { return m_tVideoCap.GetResolution(); }

	void SetVideoMaxBitRate( u16 wMaxRate ) { m_tVideoCap.SetMaxBitRate(wMaxRate); }
	u16  GetVideoMaxBitRate( void ) const { return m_tVideoCap.GetMaxBitRate(); }

    void SetVideoFrameRate(u8 byFrameRate) { m_tVideoCap.SetFrameRate(byFrameRate); }
    u8   GetVideoFrameRate(void) const { return m_tVideoCap.GetFrameRate(); }
    
    void SetAudioMediaType( u8 byAudioMediaType ) { m_byAudioMediaType = byAudioMediaType;}
    u8   GetAudioMediaType( void ) const { return m_byAudioMediaType; }

    void SetVideoCap( TVideoStreamCapOld &tVidCap ){ memcpy(&m_tVideoCap, &tVidCap, sizeof(TVideoStreamCapOld)); }
    TVideoStreamCapOld   GetVideoCap( void ) const { return m_tVideoCap; }

	BOOL32   operator ==( const TSimCapSet & tSimCapSet ) const  //判断是否相等(只判断语音图像)
	{
		if( GetVideoMediaType() == tSimCapSet.GetVideoMediaType() && 
			m_byAudioMediaType == tSimCapSet.GetAudioMediaType() &&
            GetVideoMediaType() != MEDIA_TYPE_NULL && 
            m_byAudioMediaType != MEDIA_TYPE_NULL)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	BOOL32   IsLike(  const TSimCapSet & tSimCapSet )  const  //判断是否相似(只判断语音图像)
	{
		if( *this == tSimCapSet )
		{
			return TRUE;
		}
		
		if( ( GetVideoMediaType() == MEDIA_TYPE_NULL && m_byAudioMediaType != MEDIA_TYPE_NULL && 
			  m_byAudioMediaType == tSimCapSet.GetAudioMediaType() ) || 
			( m_byAudioMediaType == MEDIA_TYPE_NULL && GetVideoMediaType() != MEDIA_TYPE_NULL && 
			  GetVideoMediaType() == tSimCapSet.GetVideoMediaType() ) || 
			( tSimCapSet.GetAudioMediaType() == MEDIA_TYPE_NULL && tSimCapSet.GetVideoMediaType() != MEDIA_TYPE_NULL && 
			  tSimCapSet.GetVideoMediaType() == GetVideoMediaType() ) || 
			( tSimCapSet.GetVideoMediaType() == MEDIA_TYPE_NULL && tSimCapSet.GetAudioMediaType() != MEDIA_TYPE_NULL && 
			  tSimCapSet.GetAudioMediaType() == m_byAudioMediaType ) )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	BOOL32   IsNull( void )
	{
		if(	GetVideoMediaType() == MEDIA_TYPE_NULL && m_byAudioMediaType == MEDIA_TYPE_NULL )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	void  Clear( void )
	{
        m_tVideoCap.Clear();
        m_byAudioMediaType = MEDIA_TYPE_NULL;        
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//能力集结构(len:23)
struct TCapSupportOld
{
protected:		
	u8          m_bySupportMMcu;        //是否支持合并级联
    u8          m_bySupportH224;	    //是否支持H224
    u8          m_bySupportT120;	    //是否支持T120  
    u8          m_byEncryptMode;        //加密模式CONF_ENCRYPTMODE_NONE, CONF_ENCRYPTMODE_DES, CONF_ENCRYPTMODE_AES
    u8          m_byDStreamType;        //VIDEO_DSTREAM_H263PLUS,...

    //双流能力
    TDStreamCapOld   m_tDStreamCap; 

	//同时能力集
	TSimCapSetOld  m_tMainSimCapSet;    //主的同时能力集
	TSimCapSetOld  m_tSecondSimCapSet;  //辅的同时能力集

public:
	//构造函数
    TCapSupportOld( void ) { Clear(); }
           
	void    SetSupportMMcu( BOOL32 bMMcu) { m_bySupportMMcu = (bMMcu ? 1:0); }
	BOOL32  IsSupportMMcu() const { return (0 != m_bySupportMMcu); }

    void    SetEncryptMode(u8 byMode) { m_byEncryptMode = byMode; }
    u8      GetEncryptMode(void) { return m_byEncryptMode; }
    
    void    SetSupportH224(BOOL32  bSupportH224){ m_bySupportH224 = (bSupportH224 ? 1:0); } 
    BOOL32  IsSupportH224( void ) { return (0 != m_bySupportH224); }  
    
    void    SetSupportT120(BOOL32   bSupportT120) { m_bySupportT120 = (bSupportT120 ? 1:0);} 
    BOOL32  IsSupportT120(void) { return (0 != m_bySupportT120); }   

	//第二路视频（双流）能力集设置
	void    SetDStreamResolution(u8 byRes) { m_tDStreamCap.SetResolution(byRes); }
	u8      GetDStreamResolution(void) const { return m_tDStreamCap.GetResolution(); }
	
	void    SetDStreamMaxBitRate(u16 wMaxRate) { m_tDStreamCap.SetMaxBitRate(wMaxRate); }
	u16     GetDStreamMaxBitRate(void) const { return m_tDStreamCap.GetMaxBitRate(); }

    void    SetDStreamFrameRate(u8 byFrameRate) { m_tDStreamCap.SetFrameRate(byFrameRate); }
    u8      GetDStreamFrameRate(void) const { return m_tDStreamCap.GetFrameRate(); }

    void    SetDStreamType( u8 byDStreamType ); //仅会控使用    
    u8      GetDStreamType( void ) const { return m_byDStreamType; } //仅会控使用

	void    SetDStreamSupportH239( BOOL32 bSupport ) { m_tDStreamCap.SetSupportH239(bSupport); } //仅MCU内部使用
    BOOL32  IsDStreamSupportH239( void ) { return m_tDStreamCap.IsSupportH239(); } //第二路视频是否支持H239

    void    SetDStreamMediaType( u8 byMediaType ) { m_tDStreamCap.SetMediaType(byMediaType); } //仅MCU内部使用
    u8      GetDStreamMediaType( void ) const { return m_tDStreamCap.GetMediaType(); }	

    TDStreamCapOld GetDStreamCapSet( void ) const { return m_tDStreamCap; } 

	//第一路音视频能力集设置
    void    SetMainSimCapSet( TSimCapSetOld &tSimCapSet ) { memcpy(&m_tMainSimCapSet, &tSimCapSet, sizeof(TSimCapSetOld)); }    
    TSimCapSetOld  GetMainSimCapSet( void ) const { return m_tMainSimCapSet; }

    void    SetSecondSimCapSet( TSimCapSetOld &tSimCapSet ) { memcpy(&m_tSecondSimCapSet, &tSimCapSet, sizeof(TSimCapSetOld)); }
    TSimCapSetOld  GetSecondSimCapSet( void ) const { return m_tSecondSimCapSet; }
   
	BOOL32  IsSupportMediaType( u8 byMediaType ); //第一路音视频是否支持某种媒体类型	

	void Clear( void )
	{
        m_bySupportMMcu = 0;
        m_bySupportH224 = 0;
        m_bySupportT120 = 0;
        m_byEncryptMode = CONF_ENCRYPTMODE_NONE;
        m_byDStreamType = VIDEO_DSTREAM_MAIN;
        m_tMainSimCapSet.Clear(); 
        m_tSecondSimCapSet.Clear(); 
        m_tDStreamCap.Reset();
	}
	
	void Print( void )
	{	
		OspPrintf( TRUE, FALSE, "main encrypt mode:%d\n", m_byEncryptMode);
		OspPrintf( TRUE, FALSE, "main simul support:\n");
		OspPrintf( TRUE, FALSE, " VideoType :%d, Resolution :%d, Fps :%d\n", m_tMainSimCapSet.GetVideoMediaType(),
                   m_tMainSimCapSet.GetVideoResolution(), m_tMainSimCapSet.GetVideoFrameRate() );
		OspPrintf( TRUE, FALSE, " AudioType :%d\n", m_tMainSimCapSet.GetAudioMediaType() );
		OspPrintf( TRUE, FALSE, "second simul support:\n");
		OspPrintf( TRUE, FALSE, " VideoType :%d, Resolution :%d, Fps :%d\n", m_tSecondSimCapSet.GetVideoMediaType(),
                   m_tSecondSimCapSet.GetVideoResolution(), m_tSecondSimCapSet.GetVideoFrameRate());
		OspPrintf( TRUE, FALSE, " AudioType :%d\n", m_tSecondSimCapSet.GetAudioMediaType() );
        OspPrintf( TRUE, FALSE, "double stream support:\n" );
        OspPrintf( TRUE, FALSE, " VideoType :%d, Resolution :%d, Fps :%d\n", m_tDStreamCap.GetMediaType(),
                   m_tDStreamCap.GetResolution(), m_tDStreamCap.GetFrameRate());
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
    CConfIdOld       m_cConfId;                        //会议号，全网唯一
    TKdvTimeOld      m_tStartTime;                     //开始时间，控制台填0为立即开始
    u16			     m_wDuration;                      //持续时间(分钟)，0表示不自动停止
    u16              m_wBitRate;                       //会议码率(单位:Kbps,1K=1024)
    u16              m_wSecBitRate;                    //双速会议的第2码率(单位:Kbps,为0表示是单速会议)
	u16			     m_wDcsBitRate;					   //数据会议码率(单位:Kbps,为0表示不支持数据会议)
    u16              m_wReserved;                      //保留
    u8               m_byTalkHoldTime;                 //最小发言持续时间(单位:秒)
    u16              m_wMixDelayTime;                  //混音延时时间
    TCapSupportOld   m_tCapSupport;                    //会议支持的媒体
    s8               m_achConfPwd[MAXLEN_PWD+1];       //会议密码
    s8               m_achConfName[MAXLEN_CONFNAME+1]; //会议名
    s8               m_achConfE164[MAXLEN_E164+1];     //会议的E164号码
    TMtAliasOld      m_tChairAlias;                    //会议中主席的别名
    TMtAliasOld      m_tSpeakerAlias;                  //会议中发言人的别名
    TConfAttrbOld    m_tConfAttrb;                     //会议属性

    TMediaEncryptOld m_tMediaKey;                      //第一版本所有密钥一样，不支持更新

public:
    TConfStatusOld   m_tStatus;                        //会议状态

public:	
    TConfInfoOld( void ){ memset( this, 0, sizeof( TConfInfoOld ) ); m_tCapSupport.Clear(); }
    CConfIdOld GetConfId( void ) const { return m_cConfId; }
    void    SetConfId( CConfIdOld cConfId ){ m_cConfId = cConfId; }
    TKdvTimeOld GetKdvStartTime( void ) const { return( m_tStartTime ); }
    void   SetKdvStartTime( TKdvTimeOld tStartTime ){ m_tStartTime = tStartTime; }
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
        TSimCapSetOld tSim = m_tCapSupport.GetMainSimCapSet();
        tSim.SetVideoResolution(byVideoFormat); 
        m_tCapSupport.SetMainSimCapSet(tSim);
    } 
    u8     GetMainVideoFormat( void ) const { return m_tCapSupport.GetMainSimCapSet().GetVideoResolution(); }
    void   SetSecVideoFormat(u8 byVideoFormat)
    { 
        TSimCapSetOld tSim = m_tCapSupport.GetSecondSimCapSet();
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
    void   SetCapSupport(TCapSupportOld tCapSupport){ m_tCapSupport = tCapSupport;} 
    TCapSupportOld GetCapSupport( void ) const { return m_tCapSupport; }
    void   SetConfPwd( LPCSTR lpszConfPwd );
    LPCSTR GetConfPwd( void ) const { return m_achConfPwd; }
    void   SetConfName( LPCSTR lpszConfName );
    LPCSTR GetConfName( void ) const { return m_achConfName; }
    void   SetConfE164( LPCSTR lpszConfE164 );
    LPCSTR GetConfE164( void ) const { return m_achConfE164; }
    void   SetChairAlias(TMtAliasOld tChairAlias){ m_tChairAlias = tChairAlias;} 
    TMtAliasOld  GetChairAlias( void ) const { return m_tChairAlias; }
    void   SetSpeakerAlias(TMtAliasOld tSpeakerAlias){ m_tSpeakerAlias = tSpeakerAlias;} 
    TMtAliasOld  GetSpeakerAlias( void ) const { return m_tSpeakerAlias; }
    void   SetConfAttrb(TConfAttrbOld tConfAttrb){ m_tConfAttrb = tConfAttrb;} 
    TConfAttrbOld  GetConfAttrb( void ) const { return m_tConfAttrb; }
    void   SetStatus(TConfStatusOld tStatus){ m_tStatus = tStatus;} 
    TConfStatusOld  GetStatus( void ) const { return m_tStatus; }	

	TMediaEncryptOld& GetMediaKey(void) { return m_tMediaKey; };
	void SetMediaKey(TMediaEncryptOld& tMediaEncrypt){ memcpy(&m_tMediaKey, &tMediaEncrypt, sizeof(tMediaEncrypt));}

	void   GetVideoScale(u8 byVideoType, u16 &wVideoWidth, u16 &wVideoHeight, u8* pbyVideoResolution = NULL);
	BOOL   HasChairman( void ) const{ if(m_tStatus.m_tChairman.GetMtId()==0)return FALSE; return TRUE; }	
	TMtOld    GetChairman( void ) const{ return m_tStatus.m_tChairman; }
	void   SetNoChairman( void ){ memset( &m_tStatus.m_tChairman, 0, sizeof( TMtOld ) ); }
	void   SetChairman( TMtOld tChairman ){ m_tStatus.m_tChairman = tChairman; }
	BOOL   HasSpeaker( void ) const{ if(m_tStatus.m_tSpeaker.GetMtId()==0)return FALSE; return TRUE; }
    TMtOld    GetSpeaker( void ) const{ return m_tStatus.m_tSpeaker; }
	void   SetNoSpeaker( void ){ memset( &m_tStatus.m_tSpeaker, 0, sizeof( TMtOld ) ); }
	void   SetSpeaker( TMtOld tSpeaker ){ m_tStatus.m_tSpeaker = tSpeaker; }
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

/*-------------------------------------------------------------------
                               TMtOld                                  
--------------------------------------------------------------------*/

/*====================================================================
    函数名      ：operator==
    功能        ：根据ID判断是否相等
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMtOld & tObj, 判断对象
    返回值说明  ：TRUE/FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/01/02    1.0         LI Yi         创建
	04/06/28    3.0         胡昌威        修改
====================================================================*/
inline BOOL TMtOld::operator ==( const TMtOld & tObj ) const
{
	if( tObj.GetType() != GetType() )
	{
		return FALSE;
	}

	switch( tObj.GetType() ) 
	{
	case TYPE_MCU:
		if( tObj.GetMcuId() == GetMcuId() ) 
		{
			return TRUE;
		}
		break;
	case TYPE_MCUPERI:
		if( tObj.GetMcuId() == GetMcuId() && tObj.GetEqpType() == GetEqpType() 
			&& tObj.GetEqpId() ==  GetEqpId() )
		{
			return TRUE;
		}
		break;
	case TYPE_MT:
        if( tObj.GetMcuId() == GetMcuId() && tObj.GetMtId() ==  GetMtId() 
			&& tObj.GetConfIdx() == GetConfIdx() )
		{
			return TRUE;
		}
		break;
	default:
		break;
	}

	return FALSE;
}

/*====================================================================
    函数名      ：SetMcu
    功能        ：设置MCU
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMcuId, MCU号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2002/07/26  1.0         LI Yi         创建
    2003/10/28  3.0         胡昌威        修改  
====================================================================*/
inline void TMtOld::SetMcu( u8 byMcuId )
{
	m_byMainType = TYPE_MCU;
	m_bySubType = 0;
	m_byMcuId = byMcuId;
	m_byEqpId = 0;	
	m_byConfDriId = 0;  
    m_byConfIdx = 0;    
}

/*====================================================================
    函数名      ：SetMcuEqp
    功能        ：设置MCU外设
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMcuId, MCU号
				  u8 byEqpId, 外设号
				  u8 byEqpType, 外设类型
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2002/07/26  1.0         LI Yi         创建
    2003/06/06  1.0         LI Yi         增加外设类型参数
    2003/10/28  3.0         胡昌威        修改  
====================================================================*/
inline void TMtOld::SetMcuEqp( u8 byMcuId, u8 byEqpId, u8 byEqpType )
{
	m_byMainType = TYPE_MCUPERI;
	m_bySubType = byEqpType;
	m_byMcuId = byMcuId;
	m_byEqpId = byEqpId;	
	m_byConfDriId = 0;  
    m_byConfIdx = 0; 
}

/*====================================================================
    函数名      ：SetMt
    功能        ：设置终端
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMcuId, MCU号
				  u8 byMtId, 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2002/07/26  1.0         LI Yi         创建
    2003/10/28  3.0         胡昌威        修改  
====================================================================*/
inline void TMtOld::SetMt( u8 byMcuId, u8 byMtId, u8 byDriId, u8 byConfIdx )
{
	m_byMainType = TYPE_MT;
	m_bySubType = 0;
	m_byMcuId = byMcuId;
	m_byEqpId = byMtId;	
	m_byConfDriId = byDriId;  
    m_byConfIdx = byConfIdx; 
}

/*====================================================================
    函数名      ：SetMt
    功能        ：设置终端
    算法实现    ：
    引用全局变量：
    输入参数说明：TMtOld tMt 
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2005/01/24  3.6         魏治兵        创建
====================================================================*/
inline void TMtOld::SetMt( TMtOld tMt )
{
	SetMt(tMt.GetMcuId(), tMt.GetMtId(), tMt.GetDriId(), tMt.GetConfIdx());
}

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

/*-------------------------------------------------------------------
                               TCapSupportOld                             
--------------------------------------------------------------------*/
/*=============================================================================
  函 数 名： SetDStreamType
  功    能： 设置双流参数
  算法实现： 
  全局变量： 
  参    数： u8 byDStreamType
  返 回 值： inline void 
=============================================================================*/
inline void TCapSupportOld::SetDStreamType( u8 byDStreamType ) 
{ 
    m_byDStreamType = byDStreamType;
    
    switch(byDStreamType)
    {
    case VIDEO_DSTREAM_MAIN:
        m_tDStreamCap.SetMediaType(m_tMainSimCapSet.GetVideoMediaType());
        m_tDStreamCap.SetSupportH239(FALSE);
        break;
    case VIDEO_DSTREAM_H263PLUS:
        m_tDStreamCap.SetMediaType(MEDIA_TYPE_H263PLUS);
        m_tDStreamCap.SetSupportH239(FALSE);
        break;
    case VIDEO_DSTREAM_H263PLUS_H239:
        m_tDStreamCap.SetMediaType(MEDIA_TYPE_H263PLUS);
        m_tDStreamCap.SetSupportH239(TRUE);
        break;
    case VIDEO_DSTREAM_H263_H239:
        m_tDStreamCap.SetMediaType(MEDIA_TYPE_H263);
        m_tDStreamCap.SetSupportH239(TRUE);
        break;
    case VIDEO_DSTREAM_H264_H239:
        m_tDStreamCap.SetMediaType(MEDIA_TYPE_H264);
        m_tDStreamCap.SetSupportH239(TRUE);
        break;
    default:
        break;
    }
}

/*====================================================================
    函数名      ：IsSupportMediaType
    功能        ：判断第一路音视频是否支持某种媒体类型
    算法实现    ：
    引用全局变量：
    输入参数说明：u8   byMediaType 媒体类型       
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/18    3.0         胡昌威          创建             
====================================================================*/
inline BOOL32  TCapSupportOld::IsSupportMediaType( u8 byMediaType )
{
    BOOL32  bSupport = FALSE;
	switch( byMediaType ) 
	{
	case MEDIA_TYPE_H261:		
	case MEDIA_TYPE_H262:		
	case MEDIA_TYPE_H263:
	case MEDIA_TYPE_H264:		
	case MEDIA_TYPE_MP4:
//  case MEDIA_TYPE_H263PLUS:	
        bSupport = (m_tMainSimCapSet.GetVideoMediaType() == byMediaType ||
                    m_tSecondSimCapSet.GetVideoMediaType() == byMediaType);
        break;   
		
	case MEDIA_TYPE_PCMA:	
	case MEDIA_TYPE_PCMU:	
	case MEDIA_TYPE_G722:	
	case MEDIA_TYPE_G7231:	
	case MEDIA_TYPE_G728:	
	case MEDIA_TYPE_G729:	
	case MEDIA_TYPE_MP3:
        bSupport = (m_tMainSimCapSet.GetAudioMediaType() == byMediaType ||
                    m_tSecondSimCapSet.GetAudioMediaType() == byMediaType);
        break;
    default:
        break;	
	}

	return bSupport;
}

/*-------------------------------------------------------------------
                               TConfInfoOld                             
--------------------------------------------------------------------*/

/*====================================================================
    函数名      ：GetVideoFormat
    功能        ：获取图像的分辨率
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byVideoType,
                  u8 byChanNo
    返回值说明  ：图像的分辨率
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/09/21    4.0         libo          创建
====================================================================*/
inline u8 TConfInfoOld::GetVideoFormat(u8 byVideoType, u8 byChanNo)
{
    if (LOGCHL_VIDEO == byChanNo)
    {
        if (m_tCapSupport.GetMainSimCapSet().GetVideoMediaType() == byVideoType)
        {
            return m_tCapSupport.GetMainSimCapSet().GetVideoResolution();
        }
        else
        {
            return m_tCapSupport.GetSecondSimCapSet().GetVideoResolution();
        }
    }
    else
    {
        return m_tCapSupport.GetDStreamResolution();
    }
}

/*====================================================================
    函数名      ：GetVideoScale
    功能        ：得到视频规模
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 &wVideoWidth 视频宽度
                  u16 &wVideoHeight 视频高度	              
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/05    3.0         胡昌威          创建             
====================================================================*/
inline void  TConfInfoOld::GetVideoScale(u8 byVideoType, u16 &wVideoWidth, u16 &wVideoHeight, u8* pbyVideoResolution/* = NULL */)
{
   
    u8 byVideoFormat;
    if (m_tCapSupport.GetMainSimCapSet().GetVideoMediaType() == byVideoType)
    {
        byVideoFormat = m_tCapSupport.GetMainSimCapSet().GetVideoResolution();
    }
    else
    {
        byVideoFormat = m_tCapSupport.GetSecondSimCapSet().GetVideoResolution();
    }
    if( NULL != pbyVideoResolution )
    {
        byVideoFormat = *pbyVideoResolution;
    }

    switch (byVideoFormat)
	{
    case VIDEO_FORMAT_SQCIF:
		wVideoWidth = 128;
        wVideoHeight = 96;
    	break;
		
    case VIDEO_FORMAT_QCIF:
		wVideoWidth = 176;
        wVideoHeight = 144;
    	break;

    case VIDEO_FORMAT_CIF:
		wVideoWidth = 352;
        wVideoHeight = 288;
    	break;

    case VIDEO_FORMAT_2CIF:
        wVideoWidth = 352;
        wVideoHeight = 576;
        break;

	case VIDEO_FORMAT_4CIF:
		wVideoWidth = 704;
        wVideoHeight = 576;
		break;

	case VIDEO_FORMAT_16CIF:
		wVideoWidth = 1408;
        wVideoHeight = 1152;
		break;

    case VIDEO_FORMAT_SIF:
        wVideoWidth = 352;
        wVideoHeight = 240;
        break;

    case VIDEO_FORMAT_4SIF:
        wVideoWidth = 704;
        wVideoHeight = 480;
        break;

    case VIDEO_FORMAT_VGA:
        wVideoWidth = 640;
        wVideoHeight = 480;
        break;

    case VIDEO_FORMAT_SVGA:
        wVideoWidth = 800;
        wVideoHeight = 600;
        break;

    case VIDEO_FORMAT_XGA:
        wVideoWidth = 1024;
        wVideoHeight = 768;
        break;

	default:
		wVideoWidth = 352;
        wVideoHeight = 288;
		break;
    }

}

/*====================================================================
    函数名      ：GetStartTime
    功能        ：获得该会议开始时间
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：time_t类型时间（local Time），控制台填0为立即开始
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/30    1.0         LI Yi         创建
    02/08/23    1.0         LI Yi         将time_t型时间转换为字符串型保存
====================================================================*/
inline time_t TConfInfoOld::GetStartTime( void ) const
{
	time_t	dwStartTime;
	
	if( m_tStartTime.GetYear() != 0 )	//不是即时会议
		m_tStartTime.GetTime( dwStartTime );
	else
		dwStartTime = 0;

	return( dwStartTime );
}

/*====================================================================
    函数名      ：SetStartTime
    功能        ：设置该会议开始时间
    算法实现    ：
    引用全局变量：
    输入参数说明：time_t dwStartTime, 开始时间，0为立即开始
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/31    1.0         LI Yi         创建
    02/08/23    1.0         LI Yi         将time_t型时间转换为字符串型保存
====================================================================*/
inline void TConfInfoOld::SetStartTime( time_t dwStartTime )
{
	if( dwStartTime != 0 )	//不是即时会议
		m_tStartTime.SetTime( &dwStartTime );
	else
		memset( &m_tStartTime, 0, sizeof( TKdvTimeOld ) );
}


/*====================================================================
    函数名      ：SetConfPwd
    功能        ：设置别名
    算法实现    ：
    引用全局变量：
    输入参数说明：LPCSTR lpszConfPwd, 别名
    返回值说明  ：字符串指针
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/26    1.0         LI Yi         创建
====================================================================*/
inline void TConfInfoOld::SetConfPwd( LPCSTR lpszConfPwd )
{
	if( lpszConfPwd != NULL )
	{
		strncpy( m_achConfPwd, lpszConfPwd, sizeof( m_achConfPwd ) );
		m_achConfPwd[sizeof( m_achConfPwd ) - 1] = '\0';
	}
	else
	{
		memset( m_achConfPwd, 0, sizeof( m_achConfPwd ) );
	}
}

/*====================================================================
    函数名      ：SetConfName
    功能        ：设置别名
    算法实现    ：
    引用全局变量：
    输入参数说明：LPCSTR lpszConfName, 别名
    返回值说明  ：字符串指针
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/26    1.0         LI Yi         创建
====================================================================*/
inline void TConfInfoOld::SetConfName( LPCSTR lpszConfName )
{
	if( lpszConfName != NULL )
	{
		strncpy( m_achConfName, lpszConfName, sizeof( m_achConfName ) );
		m_achConfName[sizeof( m_achConfName ) - 1] = '\0';
	}
	else
	{
		memset( m_achConfName, 0, sizeof( m_achConfName ) );
	}
}

/*====================================================================
    函数名      ：SetConfE164
    功能        ：设置别名
    算法实现    ：
    引用全局变量：
    输入参数说明：LPCSTR lpszConfE164, 别名
    返回值说明  ：字符串指针
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/26    1.0         LI Yi         创建
====================================================================*/
inline void TConfInfoOld::SetConfE164( LPCSTR lpszConfE164 )
{
	if( lpszConfE164 != NULL )
	{
		strncpy( m_achConfE164, lpszConfE164, sizeof( m_achConfE164 ) );
		m_achConfE164[sizeof( m_achConfE164 ) - 1] = '\0';
	}
	else
	{
		memset( m_achConfE164, 0, sizeof( m_achConfE164 ) );
	}
}

/*-------------------------------------------------------------------
                               TMtAliasOld                             
--------------------------------------------------------------------*/
/*====================================================================
    函数名      ：operator ==
    功能        ：比较两个终端别名是否相等 
    算法实现    ：
    引用全局变量：
    输入参数说明：void
    返回值说明  ：TRUE - 相等 FALSE - 不相等
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2003/11/13  3.0         胡昌威         创建
====================================================================*/
inline  BOOL   TMtAliasOld::operator ==( const TMtAliasOld & tObj ) const
{
    if( tObj.m_AliasType == 0 || tObj.m_AliasType != m_AliasType ) 
	{
		return FALSE ;
	}

	if( tObj.m_AliasType == mtAliasTypeTransportAddress )
	{
		if( (tObj.m_tTransportAddr.GetIpAddr() == m_tTransportAddr.GetIpAddr()) && 
			(tObj.m_tTransportAddr.GetPort() == m_tTransportAddr.GetPort()) && 
			(tObj.m_tTransportAddr.GetIpAddr() != 0 ))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		if( (memcmp( tObj.m_achAlias, m_achAlias, sizeof( m_achAlias ) ) == 0) && 
			(m_achAlias[0] != 0) )
		{
            return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
}

/*====================================================================
    函数名      ：SetTime
    功能        ：设置时间
    算法实现    ：
    引用全局变量：
    输入参数说明：const time_t * ptTime, 要设置的时间
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/02/07    1.0         秦重军         创建
====================================================================*/
inline void TKdvTimeOld::SetTime( const time_t * ptTime )
{
	tm *ptLocalTime;
	
	ptLocalTime = ::localtime( ptTime );

	m_wYear = htons( (u16)ptLocalTime->tm_year + 1900 );
	m_byMonth = (u8)ptLocalTime->tm_mon + 1;
	m_byMDay = (u8)ptLocalTime->tm_mday;
	m_byHour = (u8)ptLocalTime->tm_hour;
	m_byMinute = (u8)ptLocalTime->tm_min;
	m_bySecond = (u8)ptLocalTime->tm_sec;
}

/*====================================================================
    函数名      ：GetTime
    功能        ：得到时间结构
    算法实现    ：
    引用全局变量：
    输入参数说明：time_t & tTime, 返回的时间
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/02/07    1.0         秦重军         创建
====================================================================*/
inline void TKdvTimeOld::GetTime( time_t & tTime ) const
{
	tm tTmCurTime;

	tTmCurTime.tm_year = ntohs( m_wYear ) - 1900;
	tTmCurTime.tm_mon = m_byMonth - 1;
	tTmCurTime.tm_mday = m_byMDay;
	tTmCurTime.tm_hour = m_byHour;
	tTmCurTime.tm_min = m_byMinute;
	tTmCurTime.tm_sec = m_bySecond;

	tTime = ::mktime( &tTmCurTime );
}

/*====================================================================
    函数名      ：operator ==
    功能        ：==重载
    算法实现    ：
    引用全局变量：
    输入参数说明：const TKdvTimeOld & tTime, 比较的时间
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/07    1.0         李屹          创建
====================================================================*/
inline BOOL TKdvTimeOld::operator == ( const TKdvTimeOld & tTime )
{
	if( memcmp( this, &tTime, sizeof( TKdvTimeOld ) ) == 0 )
		return( TRUE );
	else
		return( FALSE );
}


//经过Pack处理后的会议存储信息 
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

//会议存储信息 
struct TConfStoreOld : public TPackConfStoreOld
{
    TMtAliasOld		m_atMtAlias[MAXNUM_CONF_MT_OLD];
	u16				m_awMtDialBitRate[MAXNUM_CONF_MT_OLD];
    TMultiTvWallModuleOld m_tMultiTvWallModule;
	TVmpModuleOld	m_atVmpModule;
public:
    
    void EmptyAllTvMember(void)
    {
        for(u8 byTvLp = 0; byTvLp < MAXNUM_PERIEQP_CHNNL_OLD; byTvLp++)
        {
            m_tMultiTvWallModule.m_atTvWallModule[byTvLp].EmptyTvMember();
        }    
    }
	
    void EmptyAllVmpMember(void)
    {
        memset((void*)m_atVmpModule.m_abyVmpMember, 0, sizeof(m_atVmpModule.m_abyVmpMember));
    }
	
    void SetMtInTvChannel(u8 byTvId, u8 byChannel, u8 byMtIdx)
    {
        for(u8 byTvLp = 0; byTvLp < MAXNUM_PERIEQP_CHNNL_OLD; byTvLp++)
        {
            if(m_tMultiTvWallModule.m_atTvWallModule[byTvLp].m_tTvWall.GetEqpId() == byTvId)
            {
                m_tMultiTvWallModule.m_atTvWallModule[byTvLp].SetTvChannel(byTvLp, byMtIdx);
                break;
            }
        }
    }
    
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

#ifdef WIN32
#pragma pack( pop )
#endif

#endif //_VCCOMMON_040602_OLD_H_
