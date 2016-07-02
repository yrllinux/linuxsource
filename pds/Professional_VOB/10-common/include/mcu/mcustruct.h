/*****************************************************************************
   模块名      : mcu
   文件名      : mcustruct.h
   相关文件    : 
   文件实现功能: MCU结构
   作者        : 胡昌威
   版本        : V3.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2004/09/16  3.5         胡昌威      创建
   2005/02/19  3.6         万春雷      级联修改、与3.5版本合并
******************************************************************************/
#ifndef __MCUSTRUCT_H_
#define __MCUSTRUCT_H_

#include "osp.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "kdvsys.h"
#include "mcuconst.h"
#include "kdvdef.h"
#include "vccommon.h"
#include "ummessagestruct.h"
#include "vcsstruct.h"
#include "lanman.h"


#ifdef WIN32
    #pragma comment( lib, "ws2_32.lib" ) 
    #pragma pack( push )
    #pragma pack( 1 )
    #define window( x )	x
#else
    #include <netinet/in.h>
    #define window( x )
#endif


#ifdef WIN32
#ifndef vsnprintf
#define vsnprintf   _vsnprintf
#endif
#ifndef snprintf
#define snprintf    _snprintf
#endif
#endif

#ifdef _LINUX_

#ifndef min
#define min(a,b) ((a)>(b)?(b):(a))
#endif
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

#ifndef VOID
#define VOID void 
#endif

#endif  /* end _LINUX_ */

//定义MCU结构
typedef TMt TMcu;

//终端扩展结构 (len:34)
struct TMtExt : public TMt
{
protected:
	u8    m_byManuId;          //厂商编号,参见厂商编号定义
	u8    m_byCallMode;        //呼叫终端方式：0-不呼叫终端，手动呼叫 1-呼叫一次 2-定时呼叫
	u16   m_wDialBitRate;      //呼叫终端速率（网络序）单位KBPS
	u32   m_dwCallLeftTimes;   //定时呼叫终端的剩余呼叫次数
	u32   m_dwIPAddr;          //IP地址（网络序）
	char  m_achAlias[VALIDLEN_ALIAS];  //终端别名长度为16Byte
    u8    m_byProtocolType;         // 终端的协议类型
public:
    TMtExt() : m_byManuId(0),
               m_byCallMode(0),
               m_wDialBitRate(0),
               m_dwCallLeftTimes(0),
               m_dwIPAddr(0),
               m_byProtocolType(0)
    {
        memset( m_achAlias, 0, sizeof(m_achAlias) );
    }
    void   SetManuId(u8   byManuId){ m_byManuId = byManuId;} 
    u8     GetManuId( void ) const { return m_byManuId; }
    void   SetCallMode(u8   byCallMode){ m_byCallMode = byCallMode;} 
    u8     GetCallMode( void ) const { return m_byCallMode; }
    void   SetDialBitRate(u16 wDialBitRate ){ m_wDialBitRate = htons(wDialBitRate);} 
    u16    GetDialBitRate( void ) const { return ntohs(m_wDialBitRate); }
    void   SetCallLeftTimes( u32 dwCallLeftTimes){ m_dwCallLeftTimes = dwCallLeftTimes;} 
    u32    GetCallLeftTimes( void ) const { return m_dwCallLeftTimes; }
    void   SetIPAddr(u32    dwIPAddr){ m_dwIPAddr = htonl(dwIPAddr); } 
    u32    GetIPAddr( void ) const { return ntohl(m_dwIPAddr); }
    void   SetProtocolType(u8 byProtocolType) { m_byProtocolType = byProtocolType;}
    u8     GetProtocolType(void) const { return m_byProtocolType;}
	char*  GetAlias( void ) const{ return (s8 *)m_achAlias; }
	void   SetAlias( char* pszAlias )
	{
        memset( m_achAlias, 0, sizeof( m_achAlias ) );
		if( pszAlias != NULL )
		{
			strncpy( m_achAlias, pszAlias, sizeof( m_achAlias ) - 1);
		}
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//终端扩展结构UTF8扩容 by pengguofeng (len:34)
struct TMtExtU : public TMt
{
private:
#define MEMBER_NUM   13         //成员个数
protected:
	u8    m_byManuId;          //厂商编号,参见厂商编号定义
	u8    m_byCallMode;        //呼叫终端方式：0-不呼叫终端，手动呼叫 1-呼叫一次 2-定时呼叫
	u16   m_wDialBitRate;      //呼叫终端速率（网络序）单位KBPS
	u32   m_dwCallLeftTimes;   //定时呼叫终端的剩余呼叫次数
	u32   m_dwIPAddr;          //IP地址（网络序）
    u8    m_byProtocolType;         // 终端的协议类型
	char  m_achAlias[VALIDLEN_ALIAS_UTF8];  //终端别名长度为16Byte
public:
    TMtExtU() : m_byManuId(0),
		m_byCallMode(0),
		m_wDialBitRate(0),
		m_dwCallLeftTimes(0),
		m_dwIPAddr(0),
		m_byProtocolType(0)
    {
        memset( m_achAlias, 0, sizeof(m_achAlias) );
    }
    void   SetManuId(u8   byManuId){ m_byManuId = byManuId;} 
    u8     GetManuId( void ) const { return m_byManuId; }
    void   SetCallMode(u8   byCallMode){ m_byCallMode = byCallMode;} 
    u8     GetCallMode( void ) const { return m_byCallMode; }
    void   SetDialBitRate(u16 wDialBitRate ){ m_wDialBitRate = htons(wDialBitRate);} 
    u16    GetDialBitRate( void ) const { return ntohs(m_wDialBitRate); }
    void   SetCallLeftTimes( u32 dwCallLeftTimes){ m_dwCallLeftTimes = dwCallLeftTimes;} 
    u32    GetCallLeftTimes( void ) const { return m_dwCallLeftTimes; }
    void   SetIPAddr(u32    dwIPAddr){ m_dwIPAddr = htonl(dwIPAddr); } 
    u32    GetIPAddr( void ) const { return ntohl(m_dwIPAddr); }
    void   SetProtocolType(u8 byProtocolType) { m_byProtocolType = byProtocolType;}
    u8     GetProtocolType(void) const { return m_byProtocolType;}
	char*  GetAlias( void ) const{ return (s8 *)m_achAlias; }
	void   SetAlias( char* pszAlias )
	{
        memset( m_achAlias, 0, sizeof( m_achAlias ) );
		if( pszAlias != NULL )
		{
			strncpy( m_achAlias, pszAlias, sizeof( m_achAlias ) - 1);
		}
	}

	//成员总数
	u8 GetMemNum(void)
	{
		return MEMBER_NUM;
	}

	u32 GetTotalMemLen(void)
	{
		return GetMemNum()*sizeof(u16);
	}
	//得到某成员的大小
	u16 GetMemLen(u8 byMemId )
	{
		switch ( byMemId )
		{
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 12:
			return sizeof(u8);
		case 9:
			return sizeof(u16);
		case 10:
		case 11:
			return sizeof(u32);
		case 13:
			return sizeof(m_achAlias);
		default:
			return 0;
		}
	}

	//判断某成员为字符串
	bool IsMemString(u8 byMemId )
	{
		if ( byMemId == 13 )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void PrintContent() const
	{
		StaticLog("| %5d | %5d | %5d | %5d | %5d | %5d | %s \n",
			GetType(), GetEqpType(), GetMcuIdx(), GetMtId(), GetDriId(), GetConfIdx(),
			GetAlias());
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//终端信息二次扩展结构 len:38
struct TMtExt2 : public TMt
{
public:
    TMtExt2():m_byHWVerId(0)
    {
        memset(m_achSWVerId, 0, sizeof(m_achSWVerId));
    }
    void SetMt(TMt &tMt)
    {
        memcpy(this, &tMt, sizeof(TMt));
    }
    void SetHWVerId( u8 byVerId ) { m_byHWVerId = byVerId;    }
    u8   GetHWVerId( void ) const { return m_byHWVerId;    }
    void SetSWVerId( LPCSTR lpszSWVerId )
    {
        if ( lpszSWVerId == NULL )
        {
            return;
        }
        u16 wLen = min(strlen(lpszSWVerId), sizeof(m_achSWVerId)-1);
        strncpy(m_achSWVerId, lpszSWVerId, wLen);
        m_achSWVerId[sizeof(m_achSWVerId)-1] = '\0';
    }
    LPCSTR GetSWVerId( void ) const { return m_achSWVerId;    }

    void SetProductId( LPCSTR lpszProductId )
    {
        if (NULL == lpszProductId)
        {
            return;
        }
        u16 wLen = min(strlen(lpszProductId), sizeof(m_achProductId)-1);
        strncpy(m_achProductId, lpszProductId, wLen);
        m_achProductId[sizeof(m_achProductId)-1] = '\0';
    }
    LPCSTR GetProductId( void ) const { return m_achProductId;  };

    void SetVersionId( LPCSTR lpszVersionId )
    {
        if (NULL == lpszVersionId)
        {
            return;
        }
        u16 wLen = min(strlen(lpszVersionId), sizeof(m_achVersionId)-1);
        strncpy(m_achVersionId, lpszVersionId, wLen);
        m_achVersionId[sizeof(m_achVersionId)-1] = '\0';
    }
    LPCSTR GetVersionId( void ) const { return m_achVersionId;  };

protected:
    u8  m_byHWVerId;                    //终端硬件版本号
    s8  m_achSWVerId[MAXLEN_SW_VER-1];  //终端软件版本号
    s8  m_achProductId[MAXLEN_PRODUCTID];//终端设备号
    s8  m_achVersionId[MAXLEN_VERSIONID];//终端设备版本号
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

#ifndef SETBITSTATUS
#define SETBITSTATUS(StatusValue, StatusMask, bStatus)  \
    if (bStatus)    StatusValue |= StatusMask;          \
    else            StatusValue &= ~StatusMask;

#define GETBITSTATUS(StatusValue, StatusMask)  (0 != (StatusValue&StatusMask))   
#endif
    
//向上级mcu上报本端的终端状态时所用的结构 (len:10)
struct TSMcuMtStatus : public TMt
{
    enum TSMcuMtStatusMask
    {
        Mask_Fecc    =  0x01,
        Mask_Mixing  =  0x02,
        Mask_VidLose  =  0x04,
		Mask_CallMode =  0x08,
        Mask_SendingVideo = 0x10,
        Mask_SendingAudio = 0x20,
        Mask_RcvingVideo = 0x40,
        Mask_RecvingAudio = 0x80		
    };

protected:
	u8  m_byIsStatus;     //0-bit是否能遥控摄像头,1-bit是否在混音,2-bit是否视频源丢失
	u8  m_byCurVideo;     //当前视频源(1)
	u8  m_byCurAudio;     //当前音频源(1)
	u8  m_byMtBoardType;  //终端的板卡类型(MT_BOARD_WIN-0, MT_BOARD_8010-1, MT_BOARD_8010A-2, MT_BOARD_8018-3, MT_BOARD_IMT-4,MT_BOARD_8010C-5)
	//zjj20100327
	u8	m_byMtExtInfo;   //最高位标示终端是否在发送第二路视频，余下标示终端呼叫失败原因

public:
	u8	m_byCasLevel;
	u8	m_abyMtIdentify[MAX_CASCADELEVEL]; //对应级别中终端的标识 
public:
	TSMcuMtStatus( void )
	{ 
		memset( this, 0, sizeof(TSMcuMtStatus));
		SetIsEnableFECC(FALSE);
		SetCurVideo(1);
		SetCurAudio(1);
		SetIsAutoCallMode(FALSE);
		SetMtExInfo( MTLEFT_REASON_NONE );
	}
	void SetIsEnableFECC(BOOL bCamRCEnable){ SETBITSTATUS(m_byIsStatus, Mask_Fecc, bCamRCEnable) } 
	BOOL IsEnableFECC( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_Fecc); }
    void SetIsMixing(BOOL bMixing) { SETBITSTATUS(m_byIsStatus, Mask_Mixing, bMixing) } 
    BOOL IsMixing(void) const { return GETBITSTATUS(m_byIsStatus, Mask_Mixing); }
    void SetVideoLose(BOOL bVidLose){ SETBITSTATUS(m_byIsStatus, Mask_VidLose, bVidLose) }
    BOOL IsVideoLose( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_VidLose);    }
	void SetCurVideo(u8 byCurVideo){ m_byCurVideo = byCurVideo;} 
	u8   GetCurVideo( void ) const { return m_byCurVideo; }
	void SetCurAudio(u8 byCurAudio){ m_byCurAudio = byCurAudio;} 
	u8   GetCurAudio( void ) const { return m_byCurAudio; }
	void SetMtBoardType(u8 byType) { m_byMtBoardType = byType; }
	u8   GetMtBoardType() const {return m_byMtBoardType; }
	void SetIsAutoCallMode( BOOL byAutoMode )   { SETBITSTATUS(m_byIsStatus, Mask_CallMode, byAutoMode); }
	BOOL IsAutoCallMode( void )                 { return GETBITSTATUS(m_byIsStatus, Mask_CallMode); }
    void SetSendVideo(BOOL bSend) { SETBITSTATUS(m_byIsStatus, Mask_SendingVideo, bSend) } 
    BOOL IsSendVideo( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_SendingVideo); }
    void SetSendAudio(BOOL bSend) { SETBITSTATUS(m_byIsStatus, Mask_SendingAudio, bSend) } 
    BOOL IsSendAudio( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_SendingAudio); }
    void SetRecvVideo(BOOL bRecv) { SETBITSTATUS(m_byIsStatus, Mask_RcvingVideo, bRecv) } 
    BOOL IsRecvVideo( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_RcvingVideo); }
    void SetRecvAudio(BOOL bRecv) { SETBITSTATUS(m_byIsStatus, Mask_RecvingAudio, bRecv) } 
    BOOL IsRecvAudio( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_RecvingAudio); }	
	//zjj20100327
	void SetDisconnectReason( u8 byReason ){ m_byMtExtInfo = (0x7F & byReason); }
	u8	 GetDisconnectReason( void ){ return m_byMtExtInfo & 0x7F; }
	void SetSendVideo2( BOOL bSend ){ bSend ? (m_byMtExtInfo |= 0x80) : (m_byMtExtInfo &= 0x7F);}
	BOOL IsSendVideo2( void ){ return ((m_byMtExtInfo & 0x80) == 0x80) ? TRUE :FALSE; }
	void SetMtExInfo(u8 byMtExInfo){ m_byMtExtInfo = byMtExInfo;} 
	u8   GetMtExInfo( void ) const { return m_byMtExtInfo; }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//向上级mcu上报本端的终端状态时所用的结构 (len:8)
struct TMcuToMcuMtStatus
{
    enum TMcuToMcuMtStatusMask
    {
        Mask_Fecc   =   0x01,
        Mask_Mixing =   0x02,
        Mask_VideoLose = 0x04,
		Mask_CallMode =  0x08,
        Mask_SendingVideo = 0x10,
        Mask_SendingAudio = 0x20,
        Mask_RcvingVideo = 0x40,
        Mask_RecvingAudio = 0x80
    };
protected:
	u8  m_byIsStatus;     //0-bit是否能遥控摄像头,1-bit是否在混音,2-bit是否视频源丢失
	u8  m_byCurVideo;     //当前视频源(1)
	u8  m_byCurAudio;     //当前音频源(1)
	u8  m_byMtBoardType;  //终端的板卡类型(MT_BOARD_WIN-0, MT_BOARD_8010-1, MT_BOARD_8010A-2, MT_BOARD_8018-3, MT_BOARD_IMT-4,MT_BOARD_8010C-5)
	u32 m_dwPartId;       //网络序
	u8  m_byMtExtInfo;    //最高位标示终端是否在发送第二路视频，余下标示终端呼叫失败原因

public:
	u8	m_byCasLevel;
	u8	m_abyMtIdentify[MAX_CASCADELEVEL]; //对应级别中终端的标识 
public:
	TMcuToMcuMtStatus( void )
	{ 
		memset( this, 0, sizeof(TMcuToMcuMtStatus));
		SetIsEnableFECC(FALSE);
        SetIsMixing(FALSE);
        SetIsVideoLose(FALSE);
		SetCurVideo(1);
		SetCurAudio(1);
		SetIsAutoCallMode(FALSE);
		SetMtExInfo( MTLEFT_REASON_NONE );
	}
	void SetIsEnableFECC(BOOL bCamRCEnable){ SETBITSTATUS(m_byIsStatus, Mask_Fecc, bCamRCEnable) } 
	BOOL IsEnableFECC( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_Fecc); }
    void SetIsMixing(BOOL bMixing) { SETBITSTATUS(m_byIsStatus, Mask_Mixing, bMixing) } 
    BOOL IsMixing(void) const { return GETBITSTATUS(m_byIsStatus, Mask_Mixing); }
    void SetIsVideoLose(BOOL bVideoLose) { SETBITSTATUS(m_byIsStatus, Mask_VideoLose, bVideoLose) }
    BOOL IsVideoLose(void) const { return GETBITSTATUS(m_byIsStatus, Mask_VideoLose);    }
	void SetCurVideo(u8 byCurVideo){ m_byCurVideo = byCurVideo;} 
	u8   GetCurVideo( void ) const { return m_byCurVideo; }
	void SetCurAudio(u8 byCurAudio){ m_byCurAudio = byCurAudio;} 
	u8   GetCurAudio( void ) const { return m_byCurAudio; }
	void SetMtBoardType(u8 byType) { m_byMtBoardType = byType; }
	u8   GetMtBoardType() const {return m_byMtBoardType; }
	void SetPartId(u32 dwPartId){ m_dwPartId = htonl(dwPartId);} 
	u32  GetPartId( void ) const { return ntohl(m_dwPartId); }
	void SetIsAutoCallMode( BOOL byAutoMode )   { SETBITSTATUS(m_byIsStatus, Mask_CallMode, byAutoMode); }
	BOOL IsAutoCallMode( void )                 { return GETBITSTATUS(m_byIsStatus, Mask_CallMode); }
    void SetSendVideo(BOOL bSend) { SETBITSTATUS(m_byIsStatus, Mask_SendingVideo, bSend) } 
    BOOL IsSendVideo( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_SendingVideo); }
    void SetSendAudio(BOOL bSend) { SETBITSTATUS(m_byIsStatus, Mask_SendingAudio, bSend) } 
    BOOL IsSendAudio( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_SendingAudio); }
    void SetRecvVideo(BOOL bRecv) { SETBITSTATUS(m_byIsStatus, Mask_RcvingVideo, bRecv) } 
    BOOL IsRecvVideo( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_RcvingVideo); }
    void SetRecvAudio(BOOL bRecv) { SETBITSTATUS(m_byIsStatus, Mask_RecvingAudio, bRecv) } 
    BOOL IsRecvAudio( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_RecvingAudio); }
	void SetMtExInfo(u8 byMtExInfo){ m_byMtExtInfo = byMtExInfo;} 
	u8   GetMtExInfo( void ) const { return m_byMtExtInfo; }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//用于其他适配板完成RRQ时的认证信息记录 GatekeeperID/EndpointID
//TH323TransportAddress结构应与radstack协议栈相应结构 cmTransportAddress 一致 (len:44)
struct TH323TransportAddress
{
    u16  m_wlength; /* length in bytes of route */
    u32  m_dwip;
    u16  m_wport;
    int  m_ntype;
    u32  m_adwroute[7];
    int  m_ndistribution;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//(len:310)
struct TH323EPGKIDAlias
{
protected:
	int  m_ntype;        //网络序
    u16  m_wlength;      //网络序
    char m_szAlias[256];
    int  m_npnType;      //网络序 
    TH323TransportAddress m_transport;
public:
	void SetIDtype( int ntype ){ m_ntype = htonl(ntype); }
	int  GetIDtype( void ){ return ntohl(m_ntype); }
	void SetIDlength( u16 wlength ){ m_wlength = htons(wlength); }
	u16  GetIDlength( void ){ return ntohs(m_wlength); }
	void SetIDpnType( int npnType ){ m_npnType = htonl(npnType); }
	int  GetIDpnType( void ){ return ntohl(m_npnType); }
	void SetIDAlias( char *pszAlias ){ memcpy( m_szAlias, pszAlias, 256 ); }
	char*  GetIDAlias( void ){ return m_szAlias; }
	void SetIDtransport( TH323TransportAddress *ptransport ){ memcpy( (void*)&m_transport, (void*)ptransport, sizeof(TH323TransportAddress) ); }
	TH323TransportAddress*  GetIDtransport( void ){ return &m_transport; }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//TRASInfo用于N+1备份，主板的RAS信息同步到备板(len:622)
struct TRASInfo 
{
protected:
    u32 m_dwGKIp;                   // 主板的gk地址－网络序
    u32 m_dwRRQIp;                  // 主板的呼叫发起地址，calladdress－网络序
    TH323EPGKIDAlias m_tGKIDAlias;  // 主板的GKID
    TH323EPGKIDAlias m_tEPIDAlias;  // 主板的EPID
    s8  m_achE164Num[MAXLEN_E164+1];// 主板的E164

public:
    TRASInfo() : m_dwGKIp(0),
                 m_dwRRQIp(0)
    {
        memset( &m_tGKIDAlias, 0, sizeof(m_tGKIDAlias) );
        memset( &m_tEPIDAlias, 0, sizeof(m_tEPIDAlias) );
        memset( &m_achE164Num, 0, sizeof(m_achE164Num) );
    }
    void SetGKIp(u32 dwIp){ m_dwGKIp = htonl(dwIp);    }
    u32  GetGKIp(void) { return ntohl(m_dwGKIp);    }
    void SetRRQIp(u32 dwIp){ m_dwRRQIp = htonl(dwIp);    }
    u32  GetRRQIp(void) { return ntohl(m_dwRRQIp);    }
    TH323EPGKIDAlias *GetGKID(void) { return &m_tGKIDAlias;    }
    void SetGKID(TH323EPGKIDAlias *ptH323GKID){ memcpy( &m_tGKIDAlias, ptH323GKID, sizeof(TH323EPGKIDAlias));    }
    TH323EPGKIDAlias *GetEPID(void) { return &m_tEPIDAlias;    }
    void SetEPID(TH323EPGKIDAlias *ptH323EPID){ memcpy( &m_tEPIDAlias, ptH323EPID, sizeof(TH323EPGKIDAlias));    }
    LPCSTR GetMcuE164(void) { return m_achE164Num; }
    void SetMcuE164(LPCSTR lpszE164Alias) 
    {
        if ( NULL != lpszE164Alias )
        {
            memcpy( m_achE164Num, lpszE164Alias, sizeof(m_achE164Num));    
            m_achE164Num[sizeof(m_achE164Num)-1] = '\0';
        }
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//业务消息类，最大处理28K长度消息
class CServMsg
{
protected:
	u16     m_wSerialNO;            //流水号
	u8      m_bySrcDriId;           //源DRI板号
	u8      m_bySrcMtId;            //源终端号
	u8      m_bySrcSsnId;           //源会话号
	u8      m_byDstDriId;           //目的DRI板号
    u8      m_byDstMtId;            //目的终端号
	u8      m_byMcuId;              //MCU号
	u8      m_byChnIndex;           //通道索引号
    u8      m_byConfIdx;            //会议索引号
	CConfId m_cConfId;              //会议号
	u16		m_wEventId;             //事件号
	u16		m_wTimer;               //定时
	u16		m_wErrorCode;           //错误码
	u16		m_wMsgBodyLen;          //消息体长度
    u8      m_byTotalPktNum;        //总包数（用于需要切包发送的消息）
    u8      m_byCurPktIdx;          //当前包索引（从0开始）
	u8		m_byEqpId;				//对应外设号
	u8      m_abyReserverd[11];     //保留字节
	u8      m_abyMsgBody[SERV_MSG_LEN-SERV_MSGHEAD_LEN];    //消息体
public:
	void  SetSerialNO(u16  wSerialNO){ m_wSerialNO = htons(wSerialNO);} 
    u16   GetSerialNO( void ) const { return ntohs(m_wSerialNO); }
    void  SetSrcDriId(u8   bySrcDriId){ m_bySrcDriId = bySrcDriId;} 
    u8    GetSrcDriId( void ) const { return m_bySrcDriId; }
    void  SetSrcMtId(u8   bySrcMtId){ m_bySrcMtId = bySrcMtId;} 
    u8    GetSrcMtId( void ) const { return m_bySrcMtId; }
    void  SetSrcSsnId(u8   bySrcSsnId){ m_bySrcSsnId = bySrcSsnId;} 
    u8    GetSrcSsnId( void ) const { return m_bySrcSsnId; }
    void  SetDstDriId(u8   byDstDriId){ m_byDstDriId = byDstDriId;} 
    u8    GetDstDriId( void ) const { return m_byDstDriId; }  
    void  SetDstMtId(u8   byDstMtId){ m_byDstMtId = byDstMtId;} 
    u8    GetDstMtId( void ) const { return m_byDstMtId; }
    void  SetMcuId(u8   byMcuId){ m_byMcuId = byMcuId;} 
    u8    GetMcuId( void ) const { return m_byMcuId; }
    void  SetChnIndex(u8   byChnIndex){ m_byChnIndex = byChnIndex;} 
    u8    GetChnIndex( void ) const { return m_byChnIndex; }
    void  SetConfIdx(u8   byConfIdx){ m_byConfIdx = byConfIdx;} 
    u8    GetConfIdx( void ) const { return m_byConfIdx; } 
    void  SetEventId(u16  wEventId){ m_wEventId = htons(wEventId);} 
    u16   GetEventId( void ) const { return ntohs(m_wEventId); }
    void  SetTimer(u16  wTimer){ m_wTimer = htons(wTimer);} 
    u16   GetTimer( void ) const { return ntohs(m_wTimer); }
    void  SetErrorCode(u16  wErrorCode){ m_wErrorCode = htons(wErrorCode);} 
    u16   GetErrorCode( void ) const { return ntohs(m_wErrorCode); }
    void  SetTotalPktNum(u8 byPktNum) { m_byTotalPktNum = byPktNum; }
    u8    GetTotalPktNum( void ) { return m_byTotalPktNum; }
    void  SetCurPktIdx(u8 byPktIdx) { m_byCurPktIdx = byPktIdx; }
    u8    GetCurPktIdx( void ) { return m_byCurPktIdx; }
    void  SetEqpId(u8 byEqpId) { m_byEqpId = byEqpId; }
    u8    GetEqpId( void ) { return m_byEqpId; }

	void Init( void );
	void SetNoSrc(){ SetSrcSsnId( 0 ); }
	void SetMsgBodyLen( u16  wMsgBodyLen );
	CServMsg( void );//constructor
	CServMsg( u8   * const pbyMsg, u16  wMsgLen );//constructor
	~CServMsg( void );//distructor
	void ClearHdr( void );//消息头清零
	CConfId GetConfId( void ) const;//获取会议号信息
	void SetConfId( const CConfId & cConfId );//设置会议号信息
	void SetNullConfId( void );//设置会议号信息为空
	u16  GetMsgBodyLen( void ) const;//获取消息体长度信息
	u16  GetMsgBody( u8   * pbyMsgBodyBuf, u16  wBufLen ) const;//获取消息体，由用户申请BUFFER，如果过小做截断处理
	u8   * const GetMsgBody( void ) const;//获取消息体指针，用户不需提供BUFFER
	void SetMsgBody( u8   * const pbyMsgBody = NULL, u16  wLen = 0 );//设置消息体
	void CatMsgBody( u8   * const pbyMsgBody, u16  wLen );//添加消息体
	u16  GetServMsgLen( void ) const;//获取整个消息长度
	u16  GetServMsg( u8   * pbyMsgBuf, u16  wBufLen ) const;//获取整个消息，由用户申请BUFFER，如果过小做截断处理
	u8   * const GetServMsg( void ) const;//获取整个消息指针，用户不需提供BUFFER
	void SetServMsg( u8   * const pbyMsg, u16  wLen );//设置整个消息
	const CServMsg & operator= ( const CServMsg & cServMsg );//操作符重载
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//重传结构 (len:12)
struct TPrsParam
{
public:
	TTransportAddr   m_tLocalAddr;		//为本地接收RTP地址
	TTransportAddr	 m_tRemoteAddr;		//为远端接收RTCP地址
public:
    void   SetLocalAddr(TTransportAddr tLocalAddr){ memcpy(&m_tLocalAddr,&tLocalAddr,sizeof(TTransportAddr)); } 
    TTransportAddr GetLocalAddr( void ) const { return m_tLocalAddr; }
    void   SetRemoteAddr(TTransportAddr tRemoteAddr){ memcpy(&m_tRemoteAddr,&tRemoteAddr,sizeof(TTransportAddr)); } 
    TTransportAddr GetRemoteAddr( void ) const { return m_tRemoteAddr; }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//重传时间跨度结构 (len:16)
struct TPrsTimeSpan
{
    TPrsTimeSpan()
    {
        memset(this, 0, sizeof(TPrsTimeSpan));
    }
    u16  m_wFirstTimeSpan;	  //第一个重传检测点
	u16  m_wSecondTimeSpan;   //第二个重传检测点
	u16  m_wThirdTimeSpan;    //第三个重传检测点
	u16  m_wRejectTimeSpan;   //过期丢弃的时间跨度
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//重传设备状态 (len:3)
struct TPrsPerChStatus
{
public:
	u8  byUsed;         //0--未创建，1--创建
	u8	byCount;	    //多少个反馈端
	u8  byChlReserved;  //是否为保留通道
	u8  byConfIndex;		//记录通道服务的会议Idx, zgc, 2007/04/24
public:
    void   SetUsed( void ){ byUsed = 1; }
    void   SetIdle( void ){ byUsed = 0; }
    BOOL   IsUsed( void ){ return byUsed; }
    
    u8     GetFeedNum( void ){ return byCount; }
    void   FeedNumAdd( void ){ byCount++; }
    void   FeedNumDec( void ){ byCount--; }

	void   SetReserved( BOOL bReserved ){ byChlReserved = bReserved; }
	BOOL   IsReserved( void ){ return byChlReserved; }

	//记录通道服务的会议Idx, zgc, 2007/04/24
	void   SetConfIdx( u8 byConfIdx )
	{
		if( byConfIdx > MAXNUM_MCU_CONF )
		{
			OspPrintf( TRUE, FALSE, "The conf idx %d is error!\n", byConfIdx );
			return;
		}
		byConfIndex = byConfIdx; 
	}
	u8	   GetConfIdx( void ){ return byConfIndex; }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//重传结构
struct TPrsStatus
{
public:
	TPrsPerChStatus   m_tPerChStatus[MAXNUM_PRS_CHNNL];		//本地接收RTP/RTCP包的地址
	u8 GetIdleChlNum(void)
	{
		u8 nSum = 0;
		for(u8 byLp = 0; byLp < MAXNUM_PRS_CHNNL; byLp++)
		{
			if( !m_tPerChStatus[byLp].IsReserved() )
			{
				nSum++;
			}
		}
		return nSum;
	}

	//记录通道服务的会议Idx, zgc, 2007/04/24
	void SetChnConfIdx( u8 byChnIdx, u8 byConfIdx )
	{ 
		if( byChnIdx > MAXNUM_PRS_CHNNL || byConfIdx > MAXNUM_MCU_CONF )
		{
			OspPrintf( TRUE, FALSE, "The chn idx %d or conf idx %d is error!\n", byChnIdx, byConfIdx );
			return;
		}
		m_tPerChStatus[byChnIdx].SetConfIdx(byConfIdx); 
	}
	u8   GetChnConfIdx( u8 byChnIdx ){ return m_tPerChStatus[byChnIdx].GetConfIdx(); }
	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//主备环境下，本端和对端外部模块主备同步情况(len: 2)
struct TMSSynState  
{
    //冲突类型
    enum EntityType
    {
        emNone      = 0,
        emMC        = 1,
        emMp        = 2,
        emMtAdp     = 3,
        emPeriEqp   = 4,
        emDcs       = 5,
        emMpc       = 0xFF
    };
    
protected:
    u8  m_byEntityType;       //未同步的实体类型
    u8  m_byEntityId;         //未同步的实体的ID
    
public:
    TMSSynState(void){ SetNull(); }
    
    void SetNull(void){ memset( this, 0, sizeof(TMSSynState) ); }
    
    void SetEntityType(u8 byEntityType){ m_byEntityType = byEntityType; }
    u8   GetEntityType( void ) const{ return m_byEntityType; }
    void SetEntityId(u8 byEntityId){ m_byEntityId = byEntityId; }
    u8   GetEntityId( void ) const { return m_byEntityId; }
    
    BOOL32 IsSynSucceed(void) const { return m_byEntityType == emNone ? TRUE : FALSE; }    
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//定义MCU状态结构 (len: 6+1 + 128*11 + 1+1*11 + 4 + 5 + 2 + 7 + 4 = 1449 )
struct TMcuStatus: public TMcu
{
	enum eMcuRunningStatusMask
    {
        Mask_Mp				=  0x00000004,
        Mask_Mtadp			=  0x00000008,
		Mask_HDSC			=  0x00000020,
		Mask_MDSC			=  0x00000040,
		Mask_DSC			=  0x00000080,
		Mask_McuRunOk		=  0x80000000
    };
	
	u8		m_byPeriEqpNum;	                        //外设数目
	TEqp	m_atPeriEqp[MAXNUM_OLDMCU_PERIEQP];	    //外设数组
	u8      m_byEqpOnline[MAXNUM_OLDMCU_PERIEQP];      //外设是否在线
	u32     m_dwPeriEqpIpAddr[MAXNUM_OLDMCU_PERIEQP];  //外设Ip地址
    
    u8      m_byPeriDcsNum;                         //DCS数目
    TEqp    m_atPeriDcs[MAXNUM_MCU_DCS];            //DCS数组
    u8      m_byDcsOnline[MAXNUM_MCU_DCS];          //DCS是否在线
    u32     m_dwPeriDcsIpAddr[MAXNUM_MCU_DCS];      //DCS ip地址

    // xsl [8/26/2005] 
    u8      m_byOngoingConfNum;                     //mcu上即时会议个数
    u8      m_byScheduleConfNum;                    //mcu上预约会议个数
    u16     m_wAllJoinedMtNum;                      //mcu上所有与会终端个数
    
    // 顾振华 [5/29/2006]
    u8      m_byRegedGk;                            //是否成功注册GK。如果未配置或者注册失败，则为0
	u16     m_wLicenseNum;							//当前License数
    TMSSynState m_tMSSynState;                      //当前主备情况(主备环境下有效)
    
    // guzh [9/4/2006] 
    u8      m_byRegedMpNum;                         //当前已注册的Mp数量
    u8      m_byRegedMtAdpNum;                      //当前已注册的H323 MtAdp数量

    // guzh  [12/15/2006]
    u8      m_byNPlusState;                         //备份服务器工作状态(mcuconst.h NPLUS_BAKSERVER_IDLE 等)
    u32     m_dwNPlusReplacedMcuIp;                 //如果备份服务器处于接替工作，则接替的服务器IP

	//zgc [12/21/2006]
	u8      m_byMcuIsConfiged;						//MCU是否被配置过的标识
	//zgc [07/26/2007]
	u8		m_byMcuCfgLevel;						// MCU配置信息的可靠级别
	
	//zgc [07/25/2007]
	//[31…24 23…16 15…8 7…0]
	//bit 2: no mp? ([0]no; [1]yes)
	//bit 3: no mtadp? ([0]no; [1]yes)
	//bit 4: no HDCS module?  ([0]no; [1]yes)
	//bit 5: no MDCS module?  ([0]no; [1]yes)
	//bit 7: no DSC module? ([0]no; [1]yes)
	//bit 31: MCU当前是否正常工作? ([0]no; [1]yes)
	u32		m_dwMcuRunningState;					// MCU运行状态
	

protected:
    u32     m_dwPersistantRunningTime;              // MCU运行时间（单位:s, linux上限:497day，vx上限:828day）
public:
	// xliang [11/20/2008] 
	u16		m_wAllHdiAccessMtNum;					// HDI授权接入MT的总数量,对于8000E来说，该字段表接入高清能力。
//	u16		m_wStdCriAccessMtNum;					// 标清接入能力（暂不支持，预留）
	u16	    m_wAudioMtAccessNum;					// 语音终端接入点数（替换下面的m_wAccessPCMtNum，因为该字段没有被用到） [10/18/2012 chendaiwei]
	//u16		m_wAccessPCMtNum;						// 上面预留转为PCMT接入能力 // [3/10/2010 xliang] 
protected:
	u16		m_wVcsAccessNum;

	
public:
	u16 GetVcsAccessNum( void )
	{
		return ntohs(m_wVcsAccessNum);
	}

	void SetVcsAccessNum( u16 wVcsAccessNum )
	{
		m_wVcsAccessNum = htons( wVcsAccessNum );
	}

	void SetIsExistMp( BOOL32 IsExistMp ) 
	{ 
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_Mp, IsExistMp) 
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistMp(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_Mp);
	}
	void SetIsExistMtadp( BOOL32 IsExistMtadp ) 
	{ 
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_Mtadp, IsExistMtadp)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistMtadp(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_Mtadp);
	}
	void SetIsExistDSC( BOOL32 IsExistDSC ) 
	{
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_DSC, IsExistDSC)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistDSC(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_DSC); 
	}
	void SetIsExistMDSC( BOOL32 IsExistDSC ) 
	{
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_MDSC, IsExistDSC)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistMDSC(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_MDSC); 
	}
	void SetIsExistHDSC( BOOL32 IsExistDSC ) 
	{
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_HDSC, IsExistDSC)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistHDSC(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_HDSC); 
	}
	void SetIsMcuRunOk( BOOL32 IsMcuRunOk ) 
	{
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_McuRunOk, IsMcuRunOk)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsMcuRunOk(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_McuRunOk);
	}
    void SetPersistantRunningTime(u32 dwTime) { m_dwPersistantRunningTime = htonl(dwTime);    }
    u32  GetPersistantRunningTime(void) const { return ntohl(m_dwPersistantRunningTime);    }

    void Print(void) const
    {
        StaticLog( "MCU Current Status: \n" );
        StaticLog( "\tIs Run OK:%d\n", IsMcuRunOk() );
        StaticLog( "\tExist mp: %d, Exist mtadp: %d, Exist dsc module<DSC.%d, MDSC.%d, HDSC.%d>\n",
				IsExistMp(), IsExistMtadp(), IsExistDSC(), IsExistMDSC(), IsExistHDSC());

        StaticLog( "\tConfig file state: ");
        switch(m_byMcuCfgLevel) 
        {
        case MCUCFGINFO_LEVEL_NEWEST:
            StaticLog( "Success\n");
            break;
        case MCUCFGINFO_LEVEL_PARTNEWEST:
            StaticLog( "Partly success\n");
            break;
        case MCUCFGINFO_LEVEL_LAST:
            StaticLog( "Read fail\n");
            break;
        case MCUCFGINFO_LEVEL_DEFAULT:
            StaticLog( "No cfg file\n");
            break;
        default:
            StaticLog( "level error!\n");
            break;
		}
        StaticLog( "\tIs Mcu Configed: %d\n", m_byMcuIsConfiged);

        StaticLog( "\tIsReggedGk:%d, Mp Num:%d, H323MtAdp Num:%d, PeirEqpNum:%d, DcsNum:%d\n", 
                  m_byRegedGk, m_byRegedMpNum, m_byRegedMtAdpNum, m_byPeriEqpNum, m_byPeriDcsNum);
        
        StaticLog( "\tOngoingConf:%d, ScheduleConf:%d, AllJoinedMt:%d\n", 
                  m_byOngoingConfNum, m_byScheduleConfNum, ntohs(m_wAllJoinedMtNum));

        if ( m_byNPlusState != NPLUS_NONE )
        {
            StaticLog( "\tNPlusState:%d, NPlus Replaced Mcu:0x%x\n", 
                m_byNPlusState, ntohl(m_dwNPlusReplacedMcuIp));
        }
        
        if (!m_tMSSynState.IsSynSucceed() )
        {
            StaticLog( "\tMS conflict entity ID:%d, Type:%d\n", 
                      m_tMSSynState.GetEntityId(), m_tMSSynState.GetEntityType() );
        }
        {
            u32 dwPersistantTime = GetPersistantRunningTime();

            u32 dwCutOffTime = 0;
            u32 dwDay = dwPersistantTime/(3600*24);
            dwCutOffTime += (3600*24) * dwDay;
            u32 dwHour = (dwPersistantTime - dwCutOffTime)/3600;
            dwCutOffTime += 3600 * dwHour;
            u32 dwMinute = (dwPersistantTime - dwCutOffTime)/60;
            dwCutOffTime += 60 * dwMinute;
            u32 dwSecond = dwPersistantTime - dwCutOffTime;
            if ( 0 == dwDay )
            {
                StaticLog( "\tPersistant running time: %d.h %d.m %d.s\n",
                                         dwHour, dwMinute, dwSecond );                
            }
            else
            {
                StaticLog( "\tPersistant running time: %d.day %d.h %d.m %d.s\n",
                                         dwDay, dwHour, dwMinute, dwSecond );                
            }
        }
		// xliang [11/20/2008] HDI 接入终端数量
		StaticLog( "\tHDI access Mt Num: %d\n", ntohs(m_wAllHdiAccessMtNum));
		StaticLog( "\taccess audio Mt Num: %d\n", ntohs(m_wAudioMtAccessNum));
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;



struct TMcuStatusAfterV4R6B2: public TMcu
{
	enum eMcuRunningStatusMask
    {
        Mask_Mp				=  0x00000004,
        Mask_Mtadp			=  0x00000008,
		Mask_HDSC			=  0x00000020,
		Mask_MDSC			=  0x00000040,
		Mask_DSC			=  0x00000080,
		Mask_McuRunOk		=  0x80000000
    };
	
	u8		m_byPeriEqpNum;	                        //外设数目
	TEqp	m_atPeriEqp[MAXNUM_MCU_PERIEQP];	    //外设数组
	u8      m_byEqpOnline[MAXNUM_MCU_PERIEQP];      //外设是否在线
	u32     m_dwPeriEqpIpAddr[MAXNUM_MCU_PERIEQP];  //外设Ip地址
    
    u8      m_byPeriDcsNum;                         //DCS数目
    TEqp    m_atPeriDcs[MAXNUM_MCU_DCS];            //DCS数组
    u8      m_byDcsOnline[MAXNUM_MCU_DCS];          //DCS是否在线
    u32     m_dwPeriDcsIpAddr[MAXNUM_MCU_DCS];      //DCS ip地址

    // xsl [8/26/2005] 
    u8      m_byOngoingConfNum;                     //mcu上即时会议个数
    u8      m_byScheduleConfNum;                    //mcu上预约会议个数
    u16     m_wAllJoinedMtNum;                      //mcu上所有与会终端个数
    
    // 顾振华 [5/29/2006]
    u8      m_byRegedGk;                            //是否成功注册GK。如果未配置或者注册失败，则为0
	u16     m_wLicenseNum;							//当前License数
    TMSSynState m_tMSSynState;                      //当前主备情况(主备环境下有效)
    
    // guzh [9/4/2006] 
    u8      m_byRegedMpNum;                         //当前已注册的Mp数量
    u8      m_byRegedMtAdpNum;                      //当前已注册的H323 MtAdp数量

    // guzh  [12/15/2006]
    u8      m_byNPlusState;                         //备份服务器工作状态(mcuconst.h NPLUS_BAKSERVER_IDLE 等)
    u32     m_dwNPlusReplacedMcuIp;                 //如果备份服务器处于接替工作，则接替的服务器IP

	//zgc [12/21/2006]
	u8      m_byMcuIsConfiged;						//MCU是否被配置过的标识
	//zgc [07/26/2007]
	u8		m_byMcuCfgLevel;						// MCU配置信息的可靠级别
	
	//zgc [07/25/2007]
	//[31…24 23…16 15…8 7…0]
	//bit 2: no mp? ([0]no; [1]yes)
	//bit 3: no mtadp? ([0]no; [1]yes)
	//bit 4: no HDCS module?  ([0]no; [1]yes)
	//bit 5: no MDCS module?  ([0]no; [1]yes)
	//bit 7: no DSC module? ([0]no; [1]yes)
	//bit 31: MCU当前是否正常工作? ([0]no; [1]yes)
	u32		m_dwMcuRunningState;					// MCU运行状态
	

protected:
    u32     m_dwPersistantRunningTime;              // MCU运行时间（单位:s, linux上限:497day，vx上限:828day）
public:
	// xliang [11/20/2008] 
	u16		m_wAllHdiAccessMtNum;					// HDI授权接入MT的总数量,对于8000E来说，该字段表接入高清能力。
//	u16		m_wStdCriAccessMtNum;					// 标清接入能力（暂不支持，预留）
	u16		m_wAccessPCMtNum;						// 上面预留转为PCMT接入能力 // [3/10/2010 xliang] 
public:
	u8 GetPeriEqpNum()
	{
		return m_byPeriEqpNum;
	}

	void SetIsExistMp( BOOL32 IsExistMp ) 
	{ 
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_Mp, IsExistMp) 
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistMp(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_Mp);
	}
	void SetIsExistMtadp( BOOL32 IsExistMtadp ) 
	{ 
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_Mtadp, IsExistMtadp)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistMtadp(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_Mtadp);
	}
	void SetIsExistDSC( BOOL32 IsExistDSC ) 
	{
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_DSC, IsExistDSC)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistDSC(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_DSC); 
	}
	void SetIsExistMDSC( BOOL32 IsExistDSC ) 
	{
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_MDSC, IsExistDSC)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistMDSC(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_MDSC); 
	}
	void SetIsExistHDSC( BOOL32 IsExistDSC ) 
	{
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_HDSC, IsExistDSC)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistHDSC(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_HDSC); 
	}
	void SetIsMcuRunOk( BOOL32 IsMcuRunOk ) 
	{
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_McuRunOk, IsMcuRunOk)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsMcuRunOk(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_McuRunOk);
	}
    void SetPersistantRunningTime(u32 dwTime) { m_dwPersistantRunningTime = htonl(dwTime);    }
    u32  GetPersistantRunningTime(void) const { return ntohl(m_dwPersistantRunningTime);    }

    void Print(void) const
    {
        OspPrintf(TRUE, FALSE, "MCU Current Status: \n" );
        OspPrintf(TRUE, FALSE, "\tIs Run OK:%d\n", IsMcuRunOk() );
        OspPrintf(TRUE, FALSE, "\tExist mp: %d, Exist mtadp: %d, Exist dsc module<DSC.%d, MDSC.%d, HDSC.%d>\n",
				IsExistMp(), IsExistMtadp(), IsExistDSC(), IsExistMDSC(), IsExistHDSC());

        OspPrintf(TRUE, FALSE, "\tConfig file state: ");
        switch(m_byMcuCfgLevel) 
        {
        case MCUCFGINFO_LEVEL_NEWEST:
            OspPrintf(TRUE, FALSE, "Success\n");
            break;
        case MCUCFGINFO_LEVEL_PARTNEWEST:
            OspPrintf(TRUE, FALSE, "Partly success\n");
            break;
        case MCUCFGINFO_LEVEL_LAST:
            OspPrintf(TRUE, FALSE, "Read fail\n");
            break;
        case MCUCFGINFO_LEVEL_DEFAULT:
            OspPrintf(TRUE, FALSE, "No cfg file\n");
            break;
        default:
            OspPrintf(TRUE, FALSE, "level error!\n");
            break;
		}
        OspPrintf(TRUE, FALSE, "\tIs Mcu Configed: %d\n", m_byMcuIsConfiged);

        OspPrintf(TRUE, FALSE, "\tIsReggedGk:%d, Mp Num:%d, H323MtAdp Num:%d, PeirEqpNum:%d, DcsNum:%d\n", 
                  m_byRegedGk, m_byRegedMpNum, m_byRegedMtAdpNum, m_byPeriEqpNum, m_byPeriDcsNum);
        
        OspPrintf(TRUE, FALSE, "\tOngoingConf:%d, ScheduleConf:%d, AllJoinedMt:%d\n", 
                  m_byOngoingConfNum, m_byScheduleConfNum, ntohs(m_wAllJoinedMtNum));

        if ( m_byNPlusState != NPLUS_NONE )
        {
            OspPrintf(TRUE, FALSE, "\tNPlusState:%d, NPlus Replaced Mcu:0x%x\n", 
                m_byNPlusState, ntohl(m_dwNPlusReplacedMcuIp));
        }
        
        if (!m_tMSSynState.IsSynSucceed() )
        {
            OspPrintf(TRUE, FALSE, "\tMS conflict entity ID:%d, Type:%d\n", 
                      m_tMSSynState.GetEntityId(), m_tMSSynState.GetEntityType() );
        }
        {
            u32 dwPersistantTime = GetPersistantRunningTime();

            u32 dwCutOffTime = 0;
            u32 dwDay = dwPersistantTime/(3600*24);
            dwCutOffTime += (3600*24) * dwDay;
            u32 dwHour = (dwPersistantTime - dwCutOffTime)/3600;
            dwCutOffTime += 3600 * dwHour;
            u32 dwMinute = (dwPersistantTime - dwCutOffTime)/60;
            dwCutOffTime += 60 * dwMinute;
            u32 dwSecond = dwPersistantTime - dwCutOffTime;
            if ( 0 == dwDay )
            {
                OspPrintf(TRUE, FALSE, "\tPersistant running time: %d.h %d.m %d.s\n",
                                         dwHour, dwMinute, dwSecond );                
            }
            else
            {
                OspPrintf(TRUE, FALSE, "\tPersistant running time: %d.day %d.h %d.m %d.s\n",
                                         dwDay, dwHour, dwMinute, dwSecond );                
            }
        }
		// xliang [11/20/2008] HDI 接入终端数量
		OspPrintf(TRUE, FALSE, "\tHDI access Mt Num: %d\n", ntohs(m_wAllHdiAccessMtNum));
		OspPrintf(TRUE, FALSE, "\taccess PCMt Num: %d\n", ntohs(m_wAccessPCMtNum));
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


struct TRecState 
{
protected:	
	u8      m_byRecState;  //当前终端录像状态
							//bit3-4: 00:非录像 01:录像 10:录像暂停
							//bit0:   0:实时录像 1:抽帧录像
							//bit5-6:00:录像通道idx为0，01:录像通道idx为1，10：录像通道idx为2
							//bit1:   0:不发布 1:发布
public:	
	BOOL IsNoRecording( void ) const	{ return( ( m_byRecState & 0x18 ) == 0x00 ? TRUE : FALSE ); }
	void SetNoRecording( void )	{ m_byRecState &= ~0x18; }
	BOOL IsRecording( void ) const	{ return( ( m_byRecState & 0x18 ) == 0x08 ? TRUE : FALSE ); }
	void SetRecording( void )	{ SetNoRecording(); m_byRecState |= 0x08; }
	BOOL IsRecPause( void ) const	{ return( ( m_byRecState & 0x18 ) == 0x10 ? TRUE : FALSE ); }
	void SetRecPause( void )	{ SetNoRecording(); m_byRecState |= 0x10; }
	//是否处于抽帧录像状态
	BOOL IsRecSkipFrame() const { return ( !IsNoRecording() && ( ( m_byRecState & 0x01 ) == 0x01 ) ); }
	void SetRecSkipFrame( BOOL bSkipFrame )      { if( bSkipFrame ) m_byRecState |= 0x01;else m_byRecState &= ~0x01;};
	//终端录像是否发布
	BOOL IsRecPublish() const { return ( ( m_byRecState & 0x02 ) == 0x02 ); }
	void SetRecPublish( BOOL bPublish )      { if( bPublish ) m_byRecState |= 0x02;else m_byRecState &= ~0x02;};
	
	void SetRecChannel(u8 byChannelIdx)
	{
		if (IsNoRecording())
		{
			return;
		}
		//首先清掉channelidx的位
		m_byRecState &= 0x9F;//10011111 
		//设置为channelidx对应的值
		m_byRecState |= byChannelIdx<<5; //如：channel idx:1<<5=0【01】00000 得到对应的mask，再求|
	}
	u8 GetRecChannelIdx()const
	{
		if (IsNoRecording())
		{
			return 0xFF;
		}
		else
		{
			return (m_byRecState & 0x60)>>5;//与01100000求&，取到第5-6位（从0开始）对应的channelidx
		}
		
	}

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//终端码率结构 len: 14
struct TMtBitrate : public TMt
{
protected:
    u16     m_wSendBitRate;              //终端发送码率(单位:Kbps,1K=1024)
    u16     m_wRecvBitRate;              //终端接收码率(单位:Kbps,1K=1024)
    u16     m_wH239SendBitRate;          //终端第二路视频发送码率(单位:Kbps,1K=1024)
    u16     m_wH239RecvBitRate;          //终端第二路视频接收码率(单位:Kbps,1K=1024)
    
public:
    TMtBitrate(void) { memset(this, 0, sizeof(TMtBitrate)); }
    void   SetSendBitRate(u16  wSendBitRate){ m_wSendBitRate = htons(wSendBitRate);} 
    u16    GetSendBitRate( void ) const { return ntohs(m_wSendBitRate); }
    void   SetRecvBitRate(u16  wRecvBitRate){ m_wRecvBitRate = htons(wRecvBitRate);} 
    u16    GetRecvBitRate( void ) const { return ntohs(m_wRecvBitRate); }
    void   SetH239SendBitRate(u16  wH239SendBitRate){ m_wH239SendBitRate = htons(wH239SendBitRate);} 
    u16    GetH239SendBitRate( void ) const { return ntohs(m_wH239SendBitRate); }
    void   SetH239RecvBitRate(u16  wH239RecvBitRate){ m_wH239RecvBitRate = htons(wH239RecvBitRate);} 
    u16    GetH239RecvBitRate( void ) const { return ntohs(m_wH239RecvBitRate); }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//终端状态结构(len:64)
struct TMtStatus : public TMtBitrate
{
protected:
    u8      m_byDecoderMute;             //是否解码静音
	u8      m_byCaptureMute;             //是否采集静音
    u8      m_byHasMatrix;               //是否有矩阵
	u8      m_byIsEnableFECC;            //是否能遥控摄像头
	u8      m_bySendAudio;               //是否正在传送音频
	u8      m_bySendVideo;               //是否正在传送视频
	u8      m_byReceiveVideo;            //是否正在接收视频
	u8      m_byReceiveAudio;            //是否正在接收音频
	u8      m_byRcvVideo2;				 //是否在接收第二视频
	u8      m_bySndVideo2;               //是否在发送第二视频
	u8      m_byInMixing;                //是否正在参加混音
	//zjj20100329 注掉是否冻结图像,加上终端离线原因
	//u8      m_byVideoFreeze;             //是否冻结图像
	//zjj20100327
	u8		m_byMtDisconnectReason;		 //终端呼叫失败原因
    // guzh [3/7/2007] 本字段从4.0R4开始取消，调整为是否视频源丢失
	//u8      m_bySelPolling;              //是否正在轮询选看
    u8      m_byVideoLose;               //是否视频源丢失
	u8      m_bySelByMcsDrag;            //是否是会控拖拉选看
    u8      m_byDecoderVolume;           //解码音量
	u8      m_byCaptureVolume;           //采集音量
	u8  	m_byCurVideo;	             //当前视频源(1)
	u8  	m_byCurAudio;	             //当前音频源(1)    
	u8      m_byH239VideoFormat;         //终端第二路视频分辨率
	u8      m_byMtBoardType;             //终端的板卡类型(MT_BOARD_WIN, MT_BOARD_8010 等)
    u8      m_byInTvWall;                //终端是否正在电视墙中
    u8      m_byInHdu;                   //终端是否正在HDU中   4.6 新加   jlb
    u8      m_byExVideoSourceNum;        //终端扩展视频源个数
	u8		m_byVideoMediaLoop;			 //终端远端环回状态
	u8		m_byAudioMediaLoop;			 //终端远端环回状态
    u8      m_byIsInMixGrp;              //是否在混音组内
public:
	TMt		m_tVideoMt;		             //当前接收的视频终端
	TMt		m_tAudioMt;		             //当前接收的音频终端
	TRecState m_tRecState;               //终端录像状态
protected:
    TMt     m_tLastSelectVidMt;          //选看的视频终端
	TMt	    m_tLastSelectAudMt;          //选看的音频终端
public:
	TMtStatus( void )
	{
        Clear();
    }

    void Clear()
    {
		memset( this,0,sizeof(TMtStatus));
		SetDecoderMute(FALSE);
		SetCaptureMute(FALSE);
		SetHasMatrix(TRUE);
		SetIsEnableFECC(FALSE);
		SetSendAudio(FALSE);
		SetSendVideo(FALSE);
		SetInMixing(FALSE);
		SetReceiveVideo(FALSE);
        SetReceiveAudio(FALSE);
		//SetVideoFreeze(FALSE);
		//SetSelPolling(FALSE);
        SetVideoLose(FALSE);
		SetSelByMcsDrag(FALSE);
		SetDecoderVolume(16);
		SetCaptureVolume(16);
		SetCurVideo(1);
		SetCurAudio(1);		
		SetRcvVideo2(FALSE);
		SetSndVideo2(FALSE);
        SetInTvWall(FALSE);
        SetIsInMixGrp(FALSE);
        SetMtBoardType(MT_BOARD_UNKNOW);
	}

    void   SetDecoderMute(BOOL bDecoderMute){ m_byDecoderMute = GETBBYTE(bDecoderMute);} 
    BOOL   IsDecoderMute( void ) const { return ISTRUE(m_byDecoderMute); }
    void   SetCaptureMute(BOOL bCaptureMute){ m_byCaptureMute = GETBBYTE(bCaptureMute);} 
    BOOL   IsCaptureMute( void ) const { return ISTRUE(m_byCaptureMute); }
    void   SetHasMatrix(BOOL bHasMatrix){ m_byHasMatrix = GETBBYTE(bHasMatrix);} 
    BOOL   IsHasMatrix( void ) const { return ISTRUE(m_byHasMatrix); }
    void   SetIsEnableFECC(BOOL bCamRCEnable){ m_byIsEnableFECC = GETBBYTE(bCamRCEnable);} 
    BOOL   IsEnableFECC( void ) const { return ISTRUE(m_byIsEnableFECC); }
    void   SetSendAudio(BOOL bSendAudio){ m_bySendAudio = GETBBYTE(bSendAudio);} 
    BOOL   IsSendAudio( void ) const { return ISTRUE(m_bySendAudio); }
    void   SetSendVideo(BOOL bSendVideo){ m_bySendVideo = GETBBYTE(bSendVideo);} 
    BOOL   IsSendVideo( void ) const { return ISTRUE(m_bySendVideo); }
    void   SetInMixing(BOOL bInMixing){ m_byInMixing = GETBBYTE(bInMixing);} 
    BOOL   IsInMixing( void ) const { return ISTRUE(m_byInMixing); }
    void   SetReceiveAudio(BOOL bRcv) { m_byReceiveAudio = GETBBYTE(bRcv); }
    BOOL   IsReceiveAudio() const { return ISTRUE(m_byReceiveAudio); }
	void   SetReceiveVideo(BOOL bReceiveVideo){ m_byReceiveVideo = GETBBYTE(bReceiveVideo);} 
    BOOL   IsReceiveVideo( void ) const { return ISTRUE(m_byReceiveVideo); }
    /*void   SetVideoFreeze(BOOL bVideoFreeze){ m_byVideoFreeze = GETBBYTE(bVideoFreeze);} 
    BOOL   IsVideoFreeze( void ) const { return ISTRUE(m_byVideoFreeze); }*/
    /*
    void   SetSelPolling(BOOL bSelPolling){ m_bySelPolling = GETBBYTE(bSelPolling);} 
    BOOL   IsSelPolling( void ) const { return ISTRUE(m_bySelPolling); }
    */
    void   SetVideoLose(BOOL bIsLose){ m_byVideoLose = GETBBYTE(bIsLose);} 
    BOOL   IsVideoLose( void ) const { return ISTRUE(m_byVideoLose); }

    void   SetIsInMixGrp(BOOL bDiscuss) { m_byIsInMixGrp = GETBBYTE(bDiscuss); }
    BOOL   IsInMixGrp(void) const { return ISTRUE(m_byIsInMixGrp); }    
    void   SetSelByMcsDrag( u8 bySelMediaMode ){ m_bySelByMcsDrag = bySelMediaMode;} 
    u8     GetSelByMcsDragMode( void ) const { return m_bySelByMcsDrag; }
    void   SetDecoderVolume(u8   byDecoderVolume){ m_byDecoderVolume = byDecoderVolume;} 
    u8     GetDecoderVolume( void ) const { return m_byDecoderVolume; }
    void   SetCaptureVolume(u8   byCaptureVolume){ m_byCaptureVolume = byCaptureVolume;} 
    u8     GetCaptureVolume( void ) const { return m_byCaptureVolume; }
    void   SetCurVideo(u8   byCurVideo){ m_byCurVideo = byCurVideo;} 
    u8     GetCurVideo( void ) const { return m_byCurVideo; }
    void   SetCurAudio(u8   byCurAudio){ m_byCurAudio = byCurAudio;} 
    u8     GetCurAudio( void ) const { return m_byCurAudio; }    
    void   SetH239VideoFormat(u8 byH239VideoFormat){ m_byH239VideoFormat = byH239VideoFormat;} 
    u8     GetH239VideoFormat( void ) const { return m_byH239VideoFormat; }
    void   SetMtBoardType(u8 byType) { m_byMtBoardType = byType; }
	u8     GetMtBoardType() const {return m_byMtBoardType; }
	void   SetVideoMt(TMt tVideoMt){ m_tVideoMt = tVideoMt;} 
    TMt    GetVideoMt( void ) const { return m_tVideoMt; }
    void   SetAudioMt(TMt tAudioMt){ m_tAudioMt = tAudioMt;} 
    TMt    GetAudioMt( void ) const { return m_tAudioMt; }

	//zjj20100327
	void   SetMtDisconnectReason( u8 byReason ){ m_byMtDisconnectReason = byReason; }
	u8	   GetMtDisconnectReason( void ) const { return m_byMtDisconnectReason; }	

    void	SetMediaLoop(u8 byMode, BOOL32 bOn) 
    { 
        if (MODE_VIDEO == byMode)
        {
            m_byVideoMediaLoop = GETBBYTE(bOn);
        }
        else if (MODE_AUDIO == byMode)
        {
            m_byAudioMediaLoop = GETBBYTE(bOn);
        }
    }
    BOOL    IsMediaLoop(u8 byMode)
    {
        if (MODE_VIDEO == byMode)
        {
            return ISTRUE(m_byVideoMediaLoop); 
        }
        else if (MODE_AUDIO == byMode)
        {
            return ISTRUE(m_byAudioMediaLoop); 
        }

        return FALSE;
    }
    // guzh [2/28/2007] 本变量为选看判别和恢复使用，请谨慎使用
	void   SetSelectMt(TMt tSeleteMt, u8 byMode ) 
    {
        if ( byMode == MODE_AUDIO || byMode == MODE_BOTH )
        {
            m_tLastSelectAudMt = tSeleteMt;
        }
        if ( byMode == MODE_VIDEO || byMode == MODE_BOTH )
        {
            m_tLastSelectVidMt = tSeleteMt;
        }        
    }
	TMt    GetSelectMt( u8 byMode ) const 
    { 
        if ( byMode == MODE_AUDIO )
        {
            return m_tLastSelectAudMt;
        }
        else if ( byMode == MODE_VIDEO )
        {
            return m_tLastSelectVidMt;
        }    
        else 
        {
            TMt tNullMt;
            tNullMt.SetNull();
            return tNullMt;
        }
    }

	void   SetRcvVideo2(BOOL bRcv) { m_byRcvVideo2 = GETBBYTE(bRcv);}
    BOOL   IsRcvVideo2() const {return ISTRUE(m_byRcvVideo2);}
    void   SetSndVideo2(BOOL bSend) { m_bySndVideo2 = GETBBYTE(bSend); }
	BOOL   IsSndVideo2() const { return ISTRUE(m_bySndVideo2); }
    void   SetExVideoSrcNum(u8 byNum) { m_byExVideoSourceNum = byNum; }
    u8     GetExVideoSrcNum(void) const { return m_byExVideoSourceNum; }

    void   SetInTvWall(BOOL bInTvWall){ m_byInTvWall = GETBBYTE(bInTvWall);} 
    BOOL   IsInTvWall( void ) const { return ISTRUE(m_byInTvWall); }
	//4.6 新加 jlb
	void   SetInHdu(BOOL bInHdu){ m_byInHdu = GETBBYTE(bInHdu);} 
    BOOL   IsInHdu( void ) const { return ISTRUE(m_byInHdu); }

	void  SetTMt( TMt tMt ){ memcpy( this, &tMt, sizeof(tMt) ); }
	const TMt & GetMt( void ) const; //获取终端结构

	//m_bySelByMcsDrag低四位标识选看模式
	//MODE_VIDEO		0001
	//MODE_AUDIO		0010
	//MODE_BOTH			0011
	//MODE_VIDEO2SECOND	1100
	void  AddSelByMcsMode(u8 byMode)
	{
		if (MODE_AUDIO != byMode && MODE_VIDEO != byMode && MODE_BOTH != byMode && MODE_VIDEO2SECOND != byMode)
		{
			return;
		}
		m_bySelByMcsDrag = m_bySelByMcsDrag | byMode;
	}

	BOOL32  HasModeInSelMode(u8 byMode)
	{
		if (MODE_BOTH == byMode)
		{
			return (m_bySelByMcsDrag & MODE_AUDIO) && (m_bySelByMcsDrag & MODE_VIDEO);
		}
		else if (MODE_AUDIO == byMode || MODE_VIDEO == byMode || MODE_VIDEO2SECOND == byMode)
		{
			return m_bySelByMcsDrag & byMode;
		}
		return FALSE;
	}
	
	void RemoveSelByMcsMode(u8 byMode)
	{
		if (MODE_AUDIO != byMode && MODE_VIDEO != byMode && MODE_BOTH != byMode && MODE_VIDEO2SECOND != byMode)
		{
			return;
		}

		if (m_bySelByMcsDrag == 0)
		{
			return;
		}
		
		m_bySelByMcsDrag = (~byMode) & m_bySelByMcsDrag;
	}

	void Print( void ) const
	{
		StaticLog( "Mcu%dMt%d status:\n", GetMcuId(), GetMtId() ); 
        StaticLog( "	m_byMtBoardType = %d\n", m_byMtBoardType ); 
		StaticLog( "	m_byDecoderMute = %d\n", m_byDecoderMute ); 
		StaticLog( "	m_byCaptureMute = %d\n", m_byCaptureMute );
		StaticLog( "	m_byHasMatrix = %d\n", m_byHasMatrix );
        StaticLog( "	m_byExVideoSourceNum = %d\n", m_byExVideoSourceNum );
		StaticLog( "	m_byIsEnableFECC = %d\n", m_byIsEnableFECC );
		StaticLog( "	m_bySendAudio = %d\n", m_bySendAudio );
		StaticLog( "	m_bySendVideo = %d\n", m_bySendVideo );
		StaticLog( "	m_bySendVideo2 = %d\n", m_bySndVideo2 );
        StaticLog( "   m_byRecvAudio = %d\n", m_byReceiveAudio );
        StaticLog( "   m_byRecvVideo = %d\n", m_byReceiveVideo );
		StaticLog( "   m_byRecvVideo2 = %d\n", m_byRcvVideo2 );
		StaticLog( "	m_byInMixing = %d\n", m_byInMixing );
		StaticLog( "	m_byInTvWall = %d\n", m_byInTvWall );
		StaticLog( "	m_byInHdu = %d\n",    m_byInHdu );		
        StaticLog( "	m_byIsInMixGrp = %d\n", m_byIsInMixGrp );
		StaticLog( "	m_byVideoLose = %d\n", m_byVideoLose );
		StaticLog( "	m_bySelByMcsDrag = %d\n", m_bySelByMcsDrag );
		StaticLog( "	m_byDecoderVolume = %d\n", m_byDecoderVolume );
		StaticLog( "	m_byCurVideo = %d\n", m_byCurVideo );
        StaticLog( "	m_byCurAudio = %d\n", m_byCurAudio );
		StaticLog( "	m_wSendBitRate = %d\n", ntohs(m_wSendBitRate) );
		StaticLog( "	m_wRecvBitRate = %d\n", ntohs(m_wRecvBitRate) );
		StaticLog( "	m_wH239SendBitRate = %d\n", ntohs(m_wH239SendBitRate) );
		StaticLog( "	m_wH239RecvBitRate = %d\n", ntohs(m_wH239RecvBitRate) );
		StaticLog( "	m_byH239VideoFormat = %d\n", m_byH239VideoFormat );
        StaticLog( "	m_byVideoMediaLoop = %d\n", m_byVideoMediaLoop );
        StaticLog( "	m_byAudioMediaLoop = %d\n", m_byAudioMediaLoop );
		StaticLog( "	m_byMtDisconnectReason = %d\n", m_byMtDisconnectReason );
		StaticLog("		IsNoRecoding = %d\n", m_tRecState.IsNoRecording());
		StaticLog("		RecChnnelIdx = %d\n", m_tRecState.GetRecChannelIdx());
		if( m_tVideoMt.GetType() == TYPE_MT )
		{
			StaticLog( "	SrcVideo: mcu%dmt%d\n", m_tVideoMt.GetMcuId(), m_tVideoMt.GetMtId() );
		}
		else
		{
            if (m_tVideoMt.GetEqpId() != 0)
            {
                StaticLog( "	SrcVideo: eqp%d\n", m_tVideoMt.GetEqpId() );
            }
            else
            {
                StaticLog( "	SrcVideo: NULL\n" );
            }
			
		}

		if( m_tAudioMt.GetType() == TYPE_MT )
		{
			StaticLog( "	SrcAudio: mcu%dmt%d\n", m_tAudioMt.GetMcuId(), m_tAudioMt.GetMtId() );
		}
		else
		{
            if (m_tAudioMt.GetEqpId() != 0)
            {
                StaticLog( "	SrcAudio: eqp%d\n", m_tAudioMt.GetEqpId() );
            }
            else
            {
                StaticLog( "	SrcAudio: NULL\n" );
            }			
		}

        if (!m_tLastSelectVidMt.IsNull())
        {   
            StaticLog( "	Select Video MT: mcu%dmt%d\n", 
                m_tLastSelectVidMt.GetMcuId(), m_tLastSelectVidMt.GetMtId() );
        }
        else
        {
            StaticLog( "	Select Video MT: NULL\n" );
        }
        if (!m_tLastSelectAudMt.IsNull())
        {   
            StaticLog( "	Select Audio MT: mcu%dmt%d\n", 
                   m_tLastSelectAudMt.GetMcuId(), m_tLastSelectAudMt.GetMtId() );
        }
        else
        {
            StaticLog( "	Select Audio MT: NULL\n" );
        }      
	}

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TTWMember : public TMtNoConstruct
{
    u8  byMemberType;     //成员类型
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// [5/25/2011 liuxu] 不必要再定义THduMember, 直接复用TTWMember即可
typedef TTWMember THduMember;
//HDU终端成员
// struct THduMember:public TMtNoConstruct
// {
//     u8 byMemberType;
// }
// #ifndef WIN32
// __attribute__ ( (packed) ) 
// #endif
// ;


//定义数字电视墙状态结构
struct TTvWallStatus
{
    u8        byOutputMode;  //TW_OUTPUTMODE_AUDIO : 只输出音频
                             //TW_OUTPUTMODE_VIDEO : 只输出视频
                             //TW_OUTPUTMODE_BOTH  : 同时输出音视频
    u8        byChnnlNum;    //信道数
    TTWMember atVideoMt[MAXNUM_PERIEQP_CHNNL]; //每个信道对应终端，MCU号为0表示无
}
#ifndef WIN32
__attribute__((packed))
#endif
;

/*----------------------------------------------------------------------
结构名：THduChnStatus
用途  ：
----------------------------------------------------------------------*/
struct THduChnStatus
{
    enum EHduChnStatus
    {
        eIDLE        = 0,   //空闲
        eREADY       = 2,   //准备(界面会判断状态大于1时显示HDU通道上线)
		eWAITSTART   = 3,	//等待开始
		eWAITSTOP    = 4,	//等待关闭
		eWAITCHGMODE = 5,	//等待风格切换
		eRUNNING     = 6    //表示HDU的某个通道正在编码使用
    };

public:

    void   SetStatus( u8 byStatus ) { m_byStatus = byStatus; };
    u8     GetStatus( void )  const { return m_byStatus; }
    
    BOOL32 IsNull( void ) const { return m_tHdu.IsNull(); }
    void   SetNull( void ) { m_tHdu.SetNull(); }
        
    void   SetChnIdx( u8 byChnIdx ) { m_byChnIdx = byChnIdx; }
    u8     GetChnIdx( void ) const { return m_byChnIdx; }
    
    void   SetEqp( TEqp tHdu ) { m_tHdu = tHdu; }
    TEqp   GetEqp( void ) const { return m_tHdu; }
    u8     GetEqpId( void ) const { return m_tHdu.GetEqpId(); }	

    void   SetSchemeIdx( u8 bySchemeIdx ) { m_bySchemeIdx = bySchemeIdx; };
    u8     GetSchemeIdx( void ) { return m_bySchemeIdx; }

	u8     GetVolume( void ) const { return m_byVolume; }
	void   SetVolume( u8 byVolume ){ m_byVolume = byVolume; }

	BOOL32   GetIsMute( void ) const { return m_byMute; }
	void   SetIsMute( BOOL32 byIsMute ){ m_byMute = GETBBYTE(byIsMute); }

private:
	u8 m_byStatus;	  // HduChnStatus
	u8 m_byChnIdx;
    u8 m_bySchemeIdx; // 正在被使用预案的索引号 
    u8 m_byVolume;
	u8 m_byMute;      // 是否静音
    TEqpNoConstruct m_tHdu;

} 
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;


//HDU状态结构
struct THduStatus
{
    u8        byOutputMode;  //HDU_OUTPUTMODE_AUDIO : 只输出音频
	                         //HDU_OUTPUTMODE_VIDEO : 只输出视频
	                         //HDU_OUTPUTMODE_BOTH  : 同时输出音视频
    u8        byChnnlNum;    //信道数

    THduMember atVideoMt[MAXNUM_HDU_CHANNEL]; //每个信道对应终端，MCU号为0表示无 //???

	THduChnStatus atHduChnStatus[MAXNUM_HDU_CHANNEL];
private:

	u8            m_abyChnMaxVmpMode[MAXNUM_HDU_CHANNEL];        //各个通道支持的最大子通道数
	u8            m_abyChnCurVmpMode[MAXNUM_HDU_CHANNEL];        //当前各个通道处于何种风格
	THduMember    m_atHduVmpChnMt[MAXNUM_HDU_CHANNEL][HDU_MODEFOUR_MAX_SUBCHNNUM - 1];     //新加通道终端信息
	THduChnStatus m_atHduVmpChnStatus[MAXNUM_HDU_CHANNEL][HDU_MODEFOUR_MAX_SUBCHNNUM - 1]; //新加通道状态信息

public:

	//设置HDU通道数
	void SetChnnlNum(u8 byChnnNum)
	{
		byChnnlNum = byChnnNum;
	}

	u8 GetChnnlNum()
	{
		return byChnnlNum;
	}

	// [2013/03/11 chenbing] 设置最大子通道数 
	void SetChnMaxVmpMode(u8 byChnIdx, u8 byStyle)
	{
		m_abyChnMaxVmpMode[byChnIdx] = byStyle;
	}

	// [2013/03/11 chenbing] 获取最大子通道数 
	u8 GetChnMaxVmpMode(u8 byChnIdx)
	{
		return m_abyChnMaxVmpMode[byChnIdx];
	}

	// [2013/03/11 chenbing] 设置当前通道风格 
	void SetChnCurVmpMode(u8 byChnIdx, u8 byStyle)
	{
		if (byStyle != HDUCHN_MODE_ONE && byStyle != HDUCHN_MODE_FOUR)
		{
			LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[SetChnCurVmpMode]byChnIdx(%d)  byStyle(%d) invalid param\n",byChnIdx,byStyle);
			return;
		}
		m_abyChnCurVmpMode[byChnIdx] = byStyle;
	}

	// [2013/03/11 chenbing] 获取当前通道风格
	u8 GetChnCurVmpMode(u8 byChnIdx, BOOL32 bIsOtherChnVmpMode = FALSE)
	{
		if (bIsOtherChnVmpMode)
		{
			for (u8 byIndex=0; byIndex<MAXNUM_HDU_CHANNEL; byIndex++)
			{
				if (   byIndex != byChnIdx
					&& HDUCHN_MODE_FOUR == m_abyChnCurVmpMode[byIndex]
				   )
				{
					return m_abyChnCurVmpMode[byIndex];
				}
				
			}
			return HDUCHN_MODE_ONE;
		}
		else
		{
			return m_abyChnCurVmpMode[byChnIdx];
		}
	}

	// [2013/03/11 chenbing] 设置当前通道
	void SetChnIdx(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			atHduChnStatus[byChnIdx].SetChnIdx(byChnIdx);
		} 
		else
		{
			m_atHduVmpChnStatus[byChnIdx][bySubChnIdx - 1].SetChnIdx(bySubChnIdx);
		}
	}

	// [2013/03/11 chenbing] 获取当前通道
	u8 GetChnIdx(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			return atHduChnStatus[byChnIdx].GetChnIdx();
		} 
		else
		{
			return m_atHduVmpChnStatus[byChnIdx][bySubChnIdx - 1].GetChnIdx();
		}
	}

	// [2013/03/11 chenbing] 设置当前通道成员类型
	void SetMemberType(u8 byMemberType, u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			atVideoMt[byChnIdx].byMemberType = byMemberType;
		} 
		else
		{
			m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].byMemberType = byMemberType;
		}
	}

	// [2013/03/11 chenbing] 获取当前通道成员类型
	u8 GetMemberType(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			return atVideoMt[byChnIdx].byMemberType;
		} 
		else
		{
			return m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].byMemberType;
		}
	}
	
	// [2013/03/11 chenbing] 设置当前通道Mt
	void SetChnMt(TMt tMt, u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			memcpy(&atVideoMt[byChnIdx], &tMt, sizeof(TMt));
		} 
		else
		{
			memcpy(&m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1], &tMt, sizeof(TMt));
		}
	}

	// [2013/03/11 chenbing] 获取当前通道Mt
	TMt GetChnMt(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		TMt tMt;
		if (bySubChnIdx == 0)
		{
			return atVideoMt[byChnIdx];
		} 
		else
		{
			return m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1];
		}
	}

	
	// [2013/03/11 chenbing] 获取当前通道成员
	THduMember GetHduMember(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			return atVideoMt[byChnIdx];
		} 
		else
		{
			return m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1];
		}
	}

	// [2013/03/11 chenbing] 设置当前通道状态
	void SetChnStatus(u8 byChnIdx, u8 bySubChnIdx = 0, u8 byStatus = THduChnStatus::eIDLE )
	{
		if (bySubChnIdx == 0)
		{
			atHduChnStatus[byChnIdx].SetStatus(byStatus);
		} 
		else
		{
			m_atHduVmpChnStatus[byChnIdx][bySubChnIdx - 1].SetStatus(byStatus);
		}
	}

	// [2013/03/11 chenbing] 获取当前通道状态
	u8 GetChnStatus(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			return atHduChnStatus[byChnIdx].GetStatus();
		} 
		else
		{
			return m_atHduVmpChnStatus[byChnIdx][bySubChnIdx - 1].GetStatus();
		}
	}

	void SetMcuEqp(u8 byChnIdx, u8 bySubChnIdx, u8 byMcuId, u8 byEqpId, u8 byEqpType)
	{
		if (bySubChnIdx == 0)
		{
			atVideoMt[byChnIdx].SetMcuEqp(byMcuId, byEqpId, byEqpType);
		} 
		else
		{
			m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].SetMcuEqp(byMcuId, byEqpId, byEqpType);
		}
	}

	void SetMcuId(u16 wMcuId, u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			atVideoMt[byChnIdx].SetMcuId(wMcuId);
		} 
		else
		{
			m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].SetMcuId(wMcuId);
		}
	}

	u16 GetMcuId(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			return atVideoMt[byChnIdx].GetMcuId();
		} 
		else
		{
			return m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].GetMcuId();
		}
	}

	u8 GetMtId(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			return atVideoMt[byChnIdx].GetMtId();
		} 
		else
		{
			return m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].GetMtId();
		}
	}

	u8 GetEqpId(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			return atVideoMt[byChnIdx].GetEqpId();
		} 
		else
		{
			return m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].GetEqpId();
		}
	}

	u8 GetEqpType(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			return atVideoMt[byChnIdx].GetEqpType();
		} 
		else
		{
			return m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].GetEqpType();
		}
	}

	// [2013/03/11 chenbing] 设置当前通道会议号
	void SetConfIdx(u8 byConfIdx, u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			atVideoMt[byChnIdx].SetConfIdx(byConfIdx);
		} 
		else
		{
			// 以后子通道用于不同会议可在此设置会议号
			// 目前所有子通道为同一会议号
			m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].SetConfIdx(byConfIdx);
		}
	}

	// [2013/03/11 chenbing] 获取当前通道会议号
	u8 GetConfIdx(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			return atVideoMt[byChnIdx].GetConfIdx();
		} 
		else
		{
			// 目前所有子通道为同一会议号
			return atVideoMt[byChnIdx].GetConfIdx();
		}
	}

	void SetType(u8 byChnIdx, u8 bySubChnIdx = 0, u8 byType = 0)
	{
		if (bySubChnIdx == 0)
		{
			atVideoMt[byChnIdx].SetType(byType);
		} 
		else
		{
			m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].SetType(byType);
		}
	}

	u8 GetType(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			return atVideoMt[byChnIdx].GetType();
		} 
		else
		{
			return m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].GetType();
		}
	}

	void SetEqpType(u8 byChnIdx, u8 bySubChnIdx = 0, u8 byEqpType = 0)
	{
		if (bySubChnIdx == 0)
		{
			atVideoMt[byChnIdx].SetEqpType(byEqpType);
		} 
		else
		{
			m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].SetEqpType(byEqpType);
		}
	}

	u8 SetEqpType(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			atVideoMt[byChnIdx].GetEqpType();
		} 
		else
		{
			m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].GetEqpType();
		}
	}

	// [2013/03/11 chenbing] 当前通道是否没有Mt
  	BOOL32 IsChnNull(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			return (atVideoMt[byChnIdx].IsNull());
		} 
		else
		{
			return (m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].IsNull());
		}
	}

	// [2013/03/11 chenbing] 清空当前通道中的Mt
	void SetChnNull(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			atVideoMt[byChnIdx].SetNull();
		} 
		else
		{
			m_atHduVmpChnMt[byChnIdx][bySubChnIdx - 1].SetNull();
		}
	}

	BOOL32 IsNull(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			return atHduChnStatus[byChnIdx].IsNull();
		} 
		else
		{
			return m_atHduVmpChnStatus[byChnIdx][bySubChnIdx - 1].IsNull();
		}
	}

	void SetStatusNull(u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			atHduChnStatus[byChnIdx].SetNull();
		} 
		else
		{
			m_atHduVmpChnStatus[byChnIdx][bySubChnIdx - 1].SetNull();
		}
	}

	void SetSchemeIdx(u8 bySchemeIdx, u8 byChnIdx, u8 bySubChnIdx = 0)
	{
		if (bySubChnIdx == 0)
		{
			atHduChnStatus[byChnIdx].SetSchemeIdx(bySchemeIdx);
		} 
		else
		{
			m_atHduVmpChnStatus[byChnIdx][bySubChnIdx - 1].SetSchemeIdx(bySchemeIdx);
		}
	}

	u8 GetSchemeIdx(u8 byChnIdx, u8 bySubChnIdx)
	{
		if (bySubChnIdx == 0)
		{
			return atHduChnStatus[byChnIdx].GetSchemeIdx();
		} 
		else
		{
			return m_atHduVmpChnStatus[byChnIdx][bySubChnIdx - 1].GetSchemeIdx();
		}
	}
}
#ifndef WIN32
__attribute__((packed))
#endif
;

// [9/28/2011 liuxu] 补丁程序, 用于保存和获取外设通道当前的多媒体模式
// 限制条件: 外设通道限制在MAXNUM_PERIEQP_CHNNL个, 超出的将会出错
class CPeriEqpChnnlMModeMgr
{
public:
	CPeriEqpChnnlMModeMgr() { memset(this, 0, sizeof(CPeriEqpChnnlMModeMgr)); }

	void Clear(){ memset(this, 0, sizeof(CPeriEqpChnnlMModeMgr)); } 

	// 获取指定设备号和通道号的当前多媒体模式
	u8 GetChnnlMMode( const u8 byEqpId, const u8 byChnnlIdx ) const 
	{
		if ( byEqpId == 0 || MAXNUM_MCU_PERIEQP < byEqpId || MAXNUM_PERIEQP_CHNNL <= byChnnlIdx )
		{
			LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "Err, GetChnnlMMode(%u,%u) received invalid param\n", byEqpId, byChnnlIdx);
			return MODE_NONE;
		}

		return m_abyHduChnnlMMode[byEqpId - 1][byChnnlIdx];
	}

	// 设置指定设备号和通道号的当前多媒体模式
	void SetChnnlMMode( const u8 byEqpId, const u8 byChnnlIdx, const u8 byMMode )
	{
		if ( byEqpId == 0 || MAXNUM_MCU_PERIEQP < byEqpId || MAXNUM_PERIEQP_CHNNL <= byChnnlIdx )
		{
			LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "Err, SetChnnlMMode(%u,%u) received invalid param\n", byEqpId, byChnnlIdx);
			return;
		}

		m_abyHduChnnlMMode[byEqpId - 1][byChnnlIdx] = byMMode;
	}	
	
	u16 GetChnModeData(u8 *pbyBuf)
	{
		if (NULL == pbyBuf)
		{
			return 0;
		}
		memcpy(pbyBuf, m_abyHduChnnlMMode,sizeof(m_abyHduChnnlMMode));
		return sizeof(m_abyHduChnnlMMode);
	}
				
	u16 SetChnModeData(u8 *pbyBuf)
	{
		if (NULL == pbyBuf)
		{
			return 0;
		}
		memcpy(m_abyHduChnnlMMode, pbyBuf, sizeof(m_abyHduChnnlMMode));	
		return sizeof(m_abyHduChnnlMMode);
	}

private:
	u8		m_abyHduChnnlMMode[MAXNUM_MCU_PERIEQP][MAXNUM_PERIEQP_CHNNL];
};



//双载荷
struct TDoublePayload
{
protected:
	u8  m_byRealPayload;    //原媒体格式
	u8  m_byActivePayload;  //活动媒体格式
public:
    TDoublePayload()
	{
		Reset();
	}
	void Reset()
	{
		m_byRealPayload = MEDIA_TYPE_NULL;
		m_byActivePayload = MEDIA_TYPE_NULL;
	}
	void SetRealPayLoad(u8 byRealPayload)
	{
		m_byRealPayload = byRealPayload;
	}
	u8 GetRealPayLoad()
	{
		return m_byRealPayload;
	}

	void SetActivePayload(u8 byActivePayload)
	{
		m_byActivePayload = byActivePayload;
	}

	u8 GetActivePayload()
	{
		return m_byActivePayload;
	}

	void Print() const
	{
		StaticLog( "m_byRealPayload is %d, m_byActivePayload is %d\n",
			m_byRealPayload, m_byActivePayload);
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TTvWallStartPlay
{
protected:
    TMt m_tMt;
    TTransportAddr m_tVidRtcpBackAddr; //视频RTCP回馈地址
	//TTransportAddr m_tAudRtcpBackAddr; //音频Rtcp回馈地址
    u8  m_byNeedPrs;
    TMediaEncrypt m_tVideoEncrypt;

public:
    TTvWallStartPlay(void)
    {
        Reset();
    }

    void Reset(void)
    {
		m_byNeedPrs = 0;
		m_tVidRtcpBackAddr.SetNull();
		//m_tAudRtcpBackAddr.SetNull();
        m_tMt.SetNull();
        m_tVideoEncrypt.Reset();
    }

    void SetMt(const TMt &tMt)
    {
        m_tMt  = tMt;
    }

    TMt& GetMt(void)
    {
        return m_tMt;
    }

    TMediaEncrypt& GetVideoEncrypt(void)
    {
        return m_tVideoEncrypt;
    }

    void SetVideoEncrypt(TMediaEncrypt& tMediaEncrypt)
    {
        memcpy(&m_tVideoEncrypt, &tMediaEncrypt, sizeof(tMediaEncrypt));
    }

    void SetIsNeedByPrs(BOOL bNeedPrs)
    {
        m_byNeedPrs = bNeedPrs==TRUE?1:0;
    }

    BOOL IsNeedByPrs(void)
    {
        return (m_byNeedPrs != 0);
    }

	//[liu lijiu][20100919]增加RTCP回馈地址
	//设置视频的RTCP回馈地址
	void SetVidRtcpBackAddr(u32 dwRtcpBackIp, u16 wRtcpBackPort)
    {
        m_tVidRtcpBackAddr.SetIpAddr(dwRtcpBackIp);
        m_tVidRtcpBackAddr.SetPort(wRtcpBackPort);
		return;
    }

	//获取视频的RTCP回馈地址
    void GetVidRtcpBackAddr(u32 & dwRtcpBackIp, u16 & wRtcpBackPort)
    {
        dwRtcpBackIp = m_tVidRtcpBackAddr.GetIpAddr();
        wRtcpBackPort = m_tVidRtcpBackAddr.GetPort();
        return;
    }

	//设置音频的RTCP回馈地址
	/*
	void SetAudRtcpBackAddr(u32 dwRtcpBackIp, u16 wRtcpBackPort)
    {
        m_tAudRtcpBackAddr.SetIpAddr(dwRtcpBackIp);
        m_tAudRtcpBackAddr.SetPort(wRtcpBackPort);
		return;
    }

	//获取音频的RTCP回馈地址
	void GetAudRtcpBackAddr(u32 & dwRtcpBackIp, u16 & wRtcpBackPort)
    {
        dwRtcpBackIp = m_tAudRtcpBackAddr.GetIpAddr();
        wRtcpBackPort = m_tAudRtcpBackAddr.GetPort();
        return;
    }*/
}
#ifndef WIN32
__attribute__ ((packed))
#endif
;

//4.6新加 版本 jlb
struct THduStartPlay: public TTvWallStartPlay
{
protected:
	u8 m_byMode;   // MODE_AUDIO, MODE_VIDEO, MODE_BOTH
	u8 m_byReserved;
public:
	THduStartPlay()
	{
		m_byMode = 0;
		m_byReserved = 0;
	}

	void SetMode( u8 byMode ){ m_byMode = byMode; }
    u8   GetMode( void ){ return m_byMode; }

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// [1/5/2010 xliang] T8KEMixerStart 
struct T8KEMixerStart
{
protected:
    u8 m_byMixGroupId; //混音组Id
    u8 m_byMixDepth;   //混音深度
    u8 m_byAudioMode;  //语音格式
    TMediaEncrypt m_tAudioEncrypt;	//加密参数
    u8 m_byNeedPrs;
public:
    T8KEMixerStart(void)
    {
        Reset();
    }
    void Reset(void)
    {
        m_byMixGroupId = MIXER_INVALID_GRPID;
        m_byMixDepth = 0;
        m_byAudioMode = MEDIA_TYPE_NULL;
        m_byNeedPrs = 0;
        m_tAudioEncrypt.Reset();
    }
	
    void SetMixGroupId(u8 byGroupID)
    {
        m_byMixGroupId = byGroupID;
    }
	
    u8 GetMixGroupId() const
    {
        return m_byMixGroupId;
    }
    void SetMixDepth(u8 byDepth)
    {
        m_byMixDepth = byDepth;
    }
    u8 GetMixDepth() const
    {
        return m_byMixDepth;
    }
	
    void SetAudioMode(u8 byAudioMode)
    {
        m_byAudioMode = byAudioMode;
    }
    u8  GetAudioMode() const
    {
        return m_byAudioMode;
    }
	
    TMediaEncrypt& GetAudioEncrypt()
    {
        return m_tAudioEncrypt;
    }
	
    void SetAudioEncrypt(TMediaEncrypt& tMediaEncrypt)
    {
        memcpy(&m_tAudioEncrypt, &tMediaEncrypt, sizeof(tMediaEncrypt));
    }
	
    void SetIsNeedByPrs(BOOL32 bNeedPrs)
    {
        m_byNeedPrs = (TRUE == bNeedPrs) ? 1 : 0;
    }
	
    BOOL32 IsNeedByPrs(void) const
    {
        return (m_byNeedPrs != 0);
    }

	void Print(void) const
	{
		StaticLog( "m_byMixGroupId is %d, m_byMixDepth is %d, m_byAudioMode is %d\n",
			m_byMixGroupId, m_byMixDepth, m_byAudioMode);
	}
}
#ifndef WIN32
__attribute__((packed))
#endif
;

struct TMixerStart
{
protected:
    u8 m_byMixGroupId; //混音组Id
    u8 m_byMixDepth;   //混音深度
    u8 m_byAudioMode;  //语音格式
    u8 m_byAudioMode2; //第二语音格式(若不为空则为双混音格式)
    u8 m_byIsAllMix;   //全体混音
    TMediaEncrypt m_tAudioEncrypt;	//加密参数
    u8 m_byNeedPrs;
public:
    TMixerStart(void)
    {
        Reset();
    }
    void Reset(void)
    {
        m_byMixGroupId = MIXER_INVALID_GRPID;
        m_byMixDepth = 0;
        m_byAudioMode = MEDIA_TYPE_NULL;
        m_byAudioMode2 = MEDIA_TYPE_NULL;
        m_byIsAllMix = 0;
        m_byNeedPrs = 0;
        m_tAudioEncrypt.Reset();
    }

    void SetMixGroupId(u8 byGroupID)
    {
        m_byMixGroupId = byGroupID;
    }
    u8 GetMixGroupId()
    {
        return m_byMixGroupId;
    }
    void SetMixDepth(u8 byDepth)
    {
        m_byMixDepth = byDepth;
    }
    u8 GetMixDepth()
    {
        return m_byMixDepth;
    }
    void SetAudioMode(u8 byAudioMode)
    {
        m_byAudioMode = byAudioMode;
    }
    u8  GetAudioMode()
    {
        return m_byAudioMode;
    }
    void SetSecAudioMode(u8 byAudioMode)
    {
        m_byAudioMode2 = byAudioMode;
    }
    u8  GetSecAudioMode()
    {
        return m_byAudioMode2;
    }

    void SetIsAllMix(BOOL32 bIsAllMix)
    {
        m_byIsAllMix = (bIsAllMix ? 1:0 );
    }
    BOOL32 IsAllMix()
    {
        return (m_byIsAllMix != 0);
    }
    TMediaEncrypt& GetAudioEncrypt()
    {
        return m_tAudioEncrypt;
    }

    void SetAudioEncrypt(TMediaEncrypt& tMediaEncrypt)
    {
        memcpy(&m_tAudioEncrypt, &tMediaEncrypt, sizeof(tMediaEncrypt));
    }

    void SetIsNeedByPrs(BOOL32 bNeedPrs)
    {
        m_byNeedPrs = (TRUE == bNeedPrs) ? 1 : 0;
    }

    BOOL32 IsNeedByPrs(void)
    {
        return (m_byNeedPrs != 0);
    }
}
#ifndef WIN32
__attribute__((packed))
#endif
;

//混音成员结构
struct TMixMember
{
    TMt m_tMember;
    u8  m_byVolume;                 //成员音量
    u8  m_byAudioType;              //媒体类型
    TTransportAddr m_tAddr;         //成员的传输地址
    TTransportAddr m_tRtcpBackAddr; //RTCP回馈地址
}
#ifndef WIN32
__attribute__((packed)) 
#endif
;

//混音组状态
struct TMixerGrpStatus
{
	enum EState 
	{ 
		IDLE   = 0,		//该混音组未使用 
		READY  = 1,		//该组创建,但未开始混音
		MIXING = 2,		//正在混音
        WAIT_BEGIN = 200,
        WAIT_START_SPECMIX = 201,   //等待回应状态
        WAIT_START_AUTOMIX = 202,
        WAIT_START_VAC = 203,
        WAIT_STOP = 204,
		WAIT_START_AUTOMIXANDVAC = 205,
	};

	u8       m_byGrpId;		  //混音组ID(0-4)
	u8       m_byGrpState;	  //组状态
	u8       m_byGrpMixDepth; //混音深度
    u8       m_byConfId;      //会议ID
protected:
    u8       m_abyMixMmb[MAXNUM_CONF_MT>>3];//参与竞争混音成员列表
    u8       m_abyActiveMmb[MAXNUM_MIXER_DEPTH];//被选中的混音成员列表,0表示无效成员
public:
	u8       m_byMixGrpChnNum ;//tianzhiyong 100201 EAPU下各个混音组的通道数目
	//所有状态复位
	void Reset()
	{
		m_byGrpId       = 0xff;
		m_byGrpState    = IDLE;
		m_byGrpMixDepth = 0;
		memset( m_abyMixMmb ,0 ,sizeof(m_abyMixMmb) );
		memset( m_abyActiveMmb ,0 ,sizeof(m_abyActiveMmb) );
	}
    //添加参与竞争的混音成员
	// 若该成员ID合法返回TRUE,否则返回FALSE
    BOOL  AddMmb( u8   byMmbId)
	{
		u8   byByteIdx ,byBitIdx,byMask;
		if( byMmbId > MAXNUM_CONF_MT ||byMmbId==0)
			return FALSE;
		byMmbId--;
		byByteIdx = byMmbId>>3;
		byBitIdx  = byMmbId%8;
		byMask    = 1<<byBitIdx;

		m_abyMixMmb[byByteIdx] |= byMask;

		return TRUE;
	}

	//删除参与竞争的混音成员
	// 若该成员ID合法返回TRUE,否则返回FALSE
	BOOL  RemoveMmb( u8   byMmbId)
	{

		u8   byByteIdx ,byBitIdx,byMask;
		if( byMmbId > MAXNUM_CONF_MT ||byMmbId==0)
			return FALSE;
		byMmbId--;
		byByteIdx = byMmbId>>3;
		byBitIdx  = byMmbId%8;
		byMask    = 1<<byBitIdx;

		m_abyMixMmb[byByteIdx] &= ~byMask;

		return TRUE;
	}

	//检测某成员是否存在于参与竞争的表中，
	//如果存在返回TRUE ,否则返回FALSE
	BOOL  IsContain( u8   byMmbId )
	{
		
		u8   byByteIdx ,byBitIdx,byMask;
		if( byMmbId > MAXNUM_CONF_MT ||byMmbId==0)
			return FALSE;
		byMmbId--;
		byByteIdx = byMmbId>>3;
		byBitIdx  = byMmbId%8;
		byMask    = 1<<byBitIdx;
		
		return (m_abyMixMmb[byByteIdx]&byMask)==0 ? FALSE:TRUE;
	}

	//获得正在参与竞争参与混音的成员数量
	u8    GetMmbNum()
	{
		u8   byNum=0;
		for( u8   byByteIdx=0 ;byByteIdx<(MAXNUM_CONF_MT>>3) ;byByteIdx++ )
		{
			for( u8   byBitIdx =0 ;byBitIdx<8 ;byBitIdx++ )
			{
				byNum += (m_abyMixMmb[byByteIdx]>>byBitIdx)&0x1;
			}
		}
		return byNum;
	}

	//获得正在参与竞争的混音成员ID
	//输出 - abyMmb[] 用于存储参加混音的成员ID
	//输入 - bySize abyMmb[] 的大小
	//返回 参加混音的成员数
	u8    GetMixMmb( u8   abyMmb[] ,u8   bySize)
	{
		u8   byNum=0;
		for( u8   byByteIdx=0 ;byByteIdx<(MAXNUM_CONF_MT>>3) ;byByteIdx++ )
		{
			for( u8   byBitIdx =0 ;byBitIdx<8 ;byBitIdx++ )
			{
				if((m_abyMixMmb[byByteIdx]>>byBitIdx)&0x1 )
				{
					if( bySize <= byNum )
						return bySize;
					abyMmb[byNum]= byByteIdx*8+byBitIdx+1;
					byNum++;
				}
			}
		}
		return byNum;
	}
	
	//判断指定成员是否被选中混音
	BOOL IsActive( u8   byMmbId )
	{
		if( byMmbId ==0 )return FALSE;
		for( u8   byIdx =0 ;byIdx< MAXNUM_MIXER_DEPTH ;byIdx++ )
		{
			if( m_abyActiveMmb[byIdx]== byMmbId )
				return TRUE;
		}
		return FALSE;
	}
	
	//获得被选中混音成员个数
	u8   GetActiveMmbNum()
	{
		u8   byCount=0;
		for( u8   byIdx =0 ;byIdx< MAXNUM_MIXER_DEPTH ;byIdx++ )
			if( m_abyActiveMmb[byIdx] )byCount++;
		return byCount;
	}
	
	//获得被选中混音成员
	//输出: abyMmb[] - 存储被选中混音成员ID
	//输入: bySize   - abyMmb 大小
	//返回: abyMmb中有效成员个数
	u8   GetActiveMmb( u8   abyMmb[] ,u8   bySize )
	{
		u8   byCnt=0;
    	for( u8   byIdx =0 ;byIdx< min(bySize ,MAXNUM_MIXER_DEPTH) ;byIdx++ )
			if( m_abyActiveMmb[byIdx] )
			{
				abyMmb[byIdx] = m_abyActiveMmb[byIdx];
				byCnt++;
			}
			return byCnt;		
	}

	BOOL UpdateActiveMmb( u8   abyMmb[MAXNUM_MIXER_DEPTH] )
	{
		memcpy( m_abyActiveMmb ,abyMmb ,MAXNUM_MIXER_DEPTH);
		return TRUE;
	}
	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


//混音器状态
struct TMixerStatus
{
	u8 m_byGrpNum; //混音组数量
	TMixerGrpStatus	m_atGrpStatus[MAXNUM_MIXER_GROUP];
	u8  m_bIsMultiFormat; //tianzhiyong 100201  标记该混音器是否支持多格式
	u8  m_byMixOffChnNum ;//tianzhiyong 100201 EAPU下各个混音组的偏移通道数目
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//画面合成器状态
struct TVmpStatus
{
	enum EnumUseState 
	{ 
		IDLE,		//未被占用
	    RESERVE,    //预留
        WAIT_START, //等待开始回应
		START,      //正进行画面合成
		WAIT_STOP,  //等待结束回应
	};
public:	
	u8        m_byUseState;	    //使用状态 0-未使用 1-使用
	u8        m_byChlNum;       //通道数
#ifdef _ZGCDEBUG_
	u8		  m_byEncodeNum;	//编码路数, zgc, 2007-06-20
#endif
    TVMPParam m_tVMPParam;      //合成参数(临时公开，之后变为protect)
	u8		  m_bySubType;		//vmp子类型，详见mcuconst.h中的定义// xliang [12/29/2008] 
	u8		  m_byBoardVer;		//表征MPU A板/B板
	TVMPMember  m_atVMPMember[MAXNUM_MPUSVMP_MEMBER]; //增加20个成员，超出20成员时使用
public:
	TVMPParam_25Mem GetVmpParam()
	{
		TVMPParam_25Mem tVmpParam;
		memcpy(&tVmpParam, &m_tVMPParam, sizeof(m_tVMPParam));//包含20成员的合成信息
		u8 byMaxNum = MAXNUM_MPU2VMP_MEMBER - MAXNUM_MPUSVMP_MEMBER;
		for (u8 byIdx=0; byIdx<byMaxNum; byIdx++)
		{
			tVmpParam.SetVmpMember(MAXNUM_MPUSVMP_MEMBER+byIdx, m_atVMPMember[byIdx]);
		}
		return tVmpParam;
	}
	void SetVmpParam(TVMPParam tVmpParam)
	{
		memcpy(&m_tVMPParam, &tVmpParam, sizeof(m_tVMPParam));
	}
	void SetVmpParam(TVMPParam_25Mem tVmpParam)
	{
		memcpy(&m_tVMPParam, &tVmpParam, sizeof(m_tVMPParam));
		u8 byMaxNum = MAXNUM_MPU2VMP_MEMBER - MAXNUM_MPUSVMP_MEMBER;
		for (u8 byIdx=0; byIdx<byMaxNum; byIdx++)
		{
			if (NULL != tVmpParam.GetVmpMember(MAXNUM_MPUSVMP_MEMBER+byIdx))
			{
				m_atVMPMember[byIdx] = *tVmpParam.GetVmpMember(MAXNUM_MPUSVMP_MEMBER+byIdx);
			}
		}
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// 画面合成成员结构(Mcu -- Vmp)
struct TVMPMemberEx : public TVMPMember
{
public:
	void  SetMember( TVMPMember& tMember )
	{
		memcpy(this, &tMember, sizeof(tMember));
	}

	BOOL  SetMbAlias( const s8 *pAlias ) 
    { 
        BOOL bRet = FALSE;
        if( NULL != pAlias )
        {
            memcpy(achMbAlias, pAlias, sizeof(achMbAlias));
            achMbAlias[VALIDLEN_ALIAS] = '\0';
            bRet = TRUE;
        }
#ifdef _UTF8
		// UTF8编码字节数可能为2 ~6字节，故不用下面的判断 [pengguofeng 5/23/2013]
#else
		s8 nLen = strlen(achMbAlias);
        s8 nLoop = nLen - 1;
        u8 byWideCharCount = 0;
        while ( (signed char)nLoop >= 0  && (signed char)0 > (signed char)achMbAlias[nLoop])
        {
            byWideCharCount ++;
            nLoop --;
        }
        if ( byWideCharCount % 2 == 1 )
        {
            achMbAlias[nLen-1] = '\0';
        }
#endif
        return bRet; 
    }
	const s8* GetMbAlias(void) { return achMbAlias; }

protected:
	s8   achMbAlias[VALIDLEN_ALIAS+1];
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//画面合成配置信息
struct TVmpStyleCfgInfo
{    
protected:
    u8  m_bySchemeId;                   //方案id，1－5
	u8	m_byRimEnabled;					//是否使用边框: 0-不使用(默认) 1-使用;
										//本字段目前只对方案0有效, 其他方案暂不涉及本字段的设置和判断
	u8	m_byReserved1;					//保留字段1
	u8	m_byReserved2;					//保留字段2
    u32 m_dwBackgroundColor;            //图像背景色
    u32 m_dwFrameColor;                 //图像边框色
    u32 m_dwSpeakerFrameColor;          //发言人边框色
    u32 m_dwChairFrameColor;            //主席边框色 
	u8  m_byFontType;                   // 合成主题字体
	u8  m_byFontSize;					// 合成主题字号
	u8  m_byAlignment;                  // 对齐方式
	u32  m_dwTextColor;                  // 合成主题文字颜色
	u32  m_dwTopicBkColor;               // 合成主题背景色
	u32  m_dwDiaphaneity;                // 合成主题透明度
    s8  m_achSchemeAlias[MAX_VMPSTYLE_ALIASLEN];  // 画面合成方案别名

public:
    TVmpStyleCfgInfo(void) { Clear();   }

	void	Clear(void)  { memset(this, 0, sizeof(TVmpStyleCfgInfo)); }
	void	SetNull(void) { Clear(); }
	BOOL32  IsNull(void) 
    { 
        TVmpStyleCfgInfo tInfo;
        return (0 == memcmp(this, &tInfo, sizeof(TVmpStyleCfgInfo)));
    }
    void    SetSchemeId(u8 bySchemeId) { m_bySchemeId = bySchemeId; }
    u8      GetSchemeId(void) { return m_bySchemeId; }    
    void    SetBackgroundColor(u32 dwColor) { m_dwBackgroundColor = htonl(dwColor); }
    u32     GetBackgroundColor(void) { return ntohl(m_dwBackgroundColor); }
    void    SetFrameColor(u32 dwColor) { m_dwFrameColor = htonl(dwColor); }
    u32     GetFrameColor(void) { return ntohl(m_dwFrameColor); }
    void    SetSpeakerFrameColor(u32 dwColor) { m_dwSpeakerFrameColor = htonl(dwColor); }
    u32     GetSpeakerFrameColor(void) { return ntohl(m_dwSpeakerFrameColor); }
    void    SetChairFrameColor(u32 dwColor) { m_dwChairFrameColor = htonl(dwColor); }
    u32     GetChairFrameColor(void) { return ntohl(m_dwChairFrameColor); }
	void	SetIsRimEnabled(BOOL32 bEnabled){ m_byRimEnabled = GETBBYTE(bEnabled); }
	BOOL32	GetIsRimEnabled(void) const { return m_byRimEnabled == 1 ? TRUE : FALSE; }
	void    SetFontType(u8 byFontType) { m_byFontType = byFontType;}
	u8      GetFontType(void) { return m_byFontType;}
	void    SetFontSize(u8 byFontSize ) { m_byFontSize = byFontSize;}
	u8      GetFontSize(void) { return m_byFontSize;}
	void    SetAlignment(u8 byValue ) { m_byAlignment = byValue;}
	u8      GetAlignment(void) { return m_byAlignment;}
	void    SetTextColor(u32 dwTextColor ) { m_dwTextColor = htonl(dwTextColor);}
	u32     GetTextColor(void) { return ntohl(m_dwTextColor);}
	void    SetTopicBkColor(u32 dwBkColor) { m_dwTopicBkColor = htonl(dwBkColor);}
	u32     GetTopicBkColor(void) { return ntohl(m_dwTopicBkColor);}
	void    SetDiaphaneity(u32 dwDiaphaneity ) { m_dwDiaphaneity = htonl(dwDiaphaneity); }
	u32     GetDiaphaneity(void) { return ntohl(m_dwDiaphaneity);}

    void    SetSchemeAlias(s8* pchSchemeAlias, u16 wLen = MAX_VMPSTYLE_ALIASLEN)
    {
		u32 dwCharNum = 0, dwChnNum = 0, dwIdx = 0;
		memset(m_achSchemeAlias, 0, sizeof(m_achSchemeAlias));
        memcpy(m_achSchemeAlias, pchSchemeAlias, min(sizeof(m_achSchemeAlias), wLen));
		for (dwIdx = 0; dwIdx < (MAX_VMPSTYLE_ALIASLEN - 1); dwIdx++)
		{
			if (m_achSchemeAlias[dwIdx] < 0)
			{
				dwChnNum++;
			}
			else
			{
				dwCharNum++;
			}
		}
		if (dwChnNum % 2 == 0)
		{
			m_achSchemeAlias[MAX_VMPSTYLE_ALIASLEN - 1] = '\0';
		}
		else
		{
			m_achSchemeAlias[MAX_VMPSTYLE_ALIASLEN - 2] = '\0';
		}
    }
    
    void    GetSchemeAlias(s8* pchSchemeAlias, u8 byBufferLen)
    {
        u8 byAliasLen = (byBufferLen < sizeof(m_achSchemeAlias) ) ? byBufferLen : sizeof(m_achSchemeAlias);
        memcpy(pchSchemeAlias, m_achSchemeAlias, byAliasLen);
    }
    
    void    ResetDefaultColor(void)
    {
        SetBackgroundColor(VMPSTYLE_DEFCOLOR);
        SetSpeakerFrameColor(VMPSTYLE_DEFCOLOR);
        SetChairFrameColor(VMPSTYLE_DEFCOLOR);
        SetFrameColor(VMPSTYLE_DEFCOLOR);
		SetFontType(FONT_SONG);
		SetFontSize(FONT_SIZE_24);
		SetAlignment(MIDDLE_ALIGN);
		SetTextColor(DEFAULT_TEXT_COLOR);
		SetTopicBkColor(DEFAULT_BACKGROUND_COLOR);
		SetDiaphaneity(DEFAULT_DIAPHANEITY);
    }

	inline BOOL operator == (const TVmpStyleCfgInfo& tVmpStyle )
	{
		BOOL bRet = TRUE;
		if( this == &tVmpStyle )
		{
			return bRet;
		}
		
		if ( 0 != memcmp(this, &tVmpStyle, sizeof(TVmpStyleCfgInfo)) )
		{
			bRet = FALSE;
		}
		return bRet;
	}

    void    Print(void)
    {	
		StaticLog( "VmpStyleInfo(SchemeId:%d)\n BackColor:0x%x, AudienceColor:0x%x, SpeakerColor:0x%x, ChairColor:0x%x, IsRimEnabled:%d\n",
			GetSchemeId(), GetBackgroundColor(), GetFrameColor(), GetSpeakerFrameColor(), GetChairFrameColor(), GetIsRimEnabled() );
		StaticLog( "VmpStyleInfo: \n FontType:%d, FontSize:%d, TextColor:0x%x, BkColor:0x%x, Diaphaneity:0x%x\n",
			GetFontType(), GetFontSize(), GetTextColor(), GetTopicBkColor(), GetDiaphaneity() );
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// VMP扩展配置
struct TVMPExCfgInfo
{
	u8 m_byIdleChlShowMode;  // 空闲通道显示模式 VMP_SHOW_GRAY_MODE
	
	u8  m_byIsDisplayMmbAlias; // 是否显示别名（1 为显示；0为不显示）
	u16 m_wReserved;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//画面合成参数(MCU与8000EVMP通讯用)
class C8KEVMPParam
{
public:
	u8              m_byVMPStyle;      //画面合成风格,参见mcuconst.h中画面合成风格定义
	u8    	        m_byEncType;       //图像编码类型
	u16             m_wBitRate;        //编码比特率(Kbps)
	u16   	        m_wVideoWidth;     //图像宽度(default:720)
	u16   		    m_wVideoHeight;    //图像高度(default:576)
	u8      	    m_byMemberNum;     //参加视频复合的成员数量
	u8              m_byIsDisplayMmbAlias;    //是否显示合成成员的别名, zgc, 2009-07-24// [12/21/2009 xliang] 
	u8				m_byFrameRate;	   //帧率
	
	TVMPMemberEx    m_atMtMember[MAXNUM_SDVMP_MEMBER];    //视频复合成员，按照复合方式的逻辑编号顺序
	TMediaEncrypt   m_tVideoEncrypt[MAXNUM_SDVMP_MEMBER];     //视频加密参数
	TDoublePayload  m_tDoublePayload[MAXNUM_SDVMP_MEMBER];
	
public:
	void SetBitRate( u16 wBitRate ) { m_wBitRate = htons( wBitRate); }
	u16  GetBitRate() { return ntohs(m_wBitRate); }
	void SetVideoWidth( u16 wVidWidth ) { m_wVideoWidth = htons(wVidWidth); }
	u16  GetVideoWidth() { return ntohs(m_wVideoWidth); }
	void SetVideoHeight( u16 wVidHeight ) { m_wVideoHeight = htons(wVidHeight); }
	u16	 GetVideoHeight() { return ntohs(m_wVideoHeight); }
	
    void Print(void)
    {
        StaticLog( "VmpStyle:%d, EncType:%d, Bitrate:0x%x, VidWidth:%d, VidHeight:%d, MemNum:%d\n",
			m_byVMPStyle, m_byEncType, ntohs(m_wBitRate), m_wVideoWidth, m_wVideoHeight, m_byMemberNum);
        for(u8 byIndex = 0; byIndex < MAXNUM_SDVMP_MEMBER; byIndex++)
        {
            if(0 != m_atMtMember[byIndex].GetMcuId() && 0 != m_atMtMember[byIndex].GetMtId())
            {
                StaticLog( "McuId:%d, MtId:%d, MemType:%d, MemStatus:%d, RealPayLoad:%d, ActPayLoad:%d, EncrptMode:%d\n", 
					m_atMtMember[byIndex].GetMcuId(), m_atMtMember[byIndex].GetMtId(), 
					m_atMtMember[byIndex].GetMemberType(), m_atMtMember[byIndex].GetMemStatus(),
					m_tDoublePayload[byIndex].GetRealPayLoad(), m_tDoublePayload[byIndex].GetActivePayload(),
					m_tVideoEncrypt[byIndex].GetEncryptMode());
            }            
        }        
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//画面合成参数(MCU与VMP通讯用)
class CKDVVMPParam
{
public:
	u8              m_byVMPStyle;      //画面合成风格,参见mcuconst.h中画面合成风格定义
	u8    	        m_byEncType;       //图像编码类型
	u16             m_wBitRate;        //编码比特率(Kbps)
	u16   	        m_wVideoWidth;     //图像宽度(default:720)
	u16   		    m_wVideoHeight;    //图像高度(default:576)
  	u8      	    m_byMemberNum;     //参加视频复合的成员数量
	TVMPMemberEx    m_atMtMember[MAXNUM_MPUSVMP_MEMBER];    //视频复合成员，按照复合方式的逻辑编号顺序
	TMediaEncrypt   m_tVideoEncrypt[MAXNUM_MPUSVMP_MEMBER];     //视频加密参数
	TDoublePayload  m_tDoublePayload[MAXNUM_MPUSVMP_MEMBER];	//vpu有调整过，按20成员走

public:
    void Print(void)
    {
        StaticLog( "VmpStyle:%d, EncType:%d, Bitrate:0x%x, VidWidth:%d, VidHeight:%d, MemNum:%d\n",
                  m_byVMPStyle, m_byEncType, ntohs(m_wBitRate), m_wVideoWidth, m_wVideoHeight, m_byMemberNum);
        for(u8 byIndex = 0; byIndex < MAXNUM_VMP_MEMBER; byIndex++)
        {
            if(0 != m_atMtMember[byIndex].GetMcuId() && 0 != m_atMtMember[byIndex].GetMtId())
            {
                StaticLog( "McuId:%d, MtId:%d, MemType:%d, MemStatus:%d, RealPayLoad:%d, ActPayLoad:%d, EncrptMode:%d\n", 
                        m_atMtMember[byIndex].GetMcuId(), m_atMtMember[byIndex].GetMtId(), 
                        m_atMtMember[byIndex].GetMemberType(), m_atMtMember[byIndex].GetMemStatus(),
                        m_tDoublePayload[byIndex].GetRealPayLoad(), m_tDoublePayload[byIndex].GetActivePayload(),
                        m_tVideoEncrypt[byIndex].GetEncryptMode());
            }            
        }        
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
//VMP后适配参数(MCU与VMP通讯用)
class CKDVVMPOutMember
{
private:
	u8    	        m_byEncType;		//图像编码类型
	u8    	        m_byProfileType;	//BP/HP
	u8				m_byFrameRate;		//帧率
	u16             m_wBitRate;			//编码比特率(Kbps)
	u16   	        m_wVideoWidth;		//图像宽度(default:720)
	u16   		    m_wVideoHeight;		//图像高度(default:576)
public:
	void Clear() { memset(this, 0, sizeof(CKDVVMPOutMember)); }
	void SetBitRate( u16 wBitRate ) { m_wBitRate = htons( wBitRate); }
	u16  GetBitRate() { return ntohs(m_wBitRate); }
	void SetVideoWidth( u16 wVidWidth ) { m_wVideoWidth = htons(wVidWidth); }
	u16  GetVideoWidth() { return ntohs(m_wVideoWidth); }
	void SetVideoHeight( u16 wVidHeight ) { m_wVideoHeight = htons(wVidHeight); }
	u16	 GetVideoHeight() { return ntohs(m_wVideoHeight); }
	void SetFrameRate( u8 byFrameRate ) { m_byFrameRate = byFrameRate ; }
	u8	 GetFrameRate() { return m_byFrameRate; }
	void SetEncType( u8 byEncType ) { m_byEncType = byEncType ; }
	u8	 GetEncType() { return m_byEncType; }
	void SetProfileType( u8 byProfileType ) { m_byProfileType = byProfileType ; }
	u8	 GetProfileType() { return m_byProfileType; }
	
    void Print(void)
    {
        StaticLog( "\tEncType:\t%d\n\tProfileType:\t%d\n\tFrameRate:\t%d\n\tBitrate:\t%d\n\tVidWidth:\t%d\n\tVidHeight:\t%d\n",
			m_byEncType, m_byProfileType, m_byFrameRate, GetBitRate(), GetVideoWidth(), GetVideoHeight());    
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//画面合成参数(MCU与VMP通讯用)
class CKDVNewVMPParam
{
public:
	u8              m_byVMPStyle;      //画面合成风格,参见mcuconst.h中画面合成风格定义
	u8    	        m_byEncType;       //图像编码类型
	u8      	    m_byMemberNum;     //参加视频复合的成员数量
	TVMPMemberEx    m_atMtMember[MAXNUM_MPUSVMP_MEMBER];    //视频复合成员，按照复合方式的逻辑编号顺序
	TMediaEncrypt   m_tVideoEncrypt[MAXNUM_MPUSVMP_MEMBER];     //视频加密参数
	TDoublePayload  m_tDoublePayload[MAXNUM_MPUSVMP_MEMBER];

private:
	u16             m_wBitRate;        //编码比特率(Kbps)
	u16   	        m_wVideoWidth;     //图像宽度(default:720)
	u16   		    m_wVideoHeight;    //图像高度(default:576)
	u8				m_byFrameRate;	   //帧率
public:
	void SetBitRate( u16 wBitRate ) { m_wBitRate = htons( wBitRate); }
	u16  GetBitRate() { return ntohs(m_wBitRate); }
	void SetVideoWidth( u16 wVidWidth ) { m_wVideoWidth = htons(wVidWidth); }
	u16  GetVideoWidth() { return ntohs(m_wVideoWidth); }
	void SetVideoHeight( u16 wVidHeight ) { m_wVideoHeight = htons(wVidHeight); }
	u16	 GetVideoHeight() { return ntohs(m_wVideoHeight); }
	void SetFrameRate( u8 byFrameRate ) { m_byFrameRate = byFrameRate ; }
	u8	 GetFrameRate() { return m_byFrameRate; }

    void Print(void)
    {
        StaticLog( "\tVmpStyle:\t%d\n\tEncType:\t%d\n\tBitrate:\t%d\n\tVidWidth:\t%d\n\tVidHeight:\t%d\n\tMemNum:\t\t%d\nFrameRate:\t%d\n",
			m_byVMPStyle, m_byEncType, GetBitRate(), GetVideoWidth(), GetVideoHeight(), m_byMemberNum, m_byFrameRate);

        StaticLog( "\nMem as follows:\n");

        for(u8 byIndex = 0; byIndex < MAXNUM_MPUSVMP_MEMBER; byIndex++)
        {
            if(0 != m_atMtMember[byIndex].GetMcuId() && 0 != m_atMtMember[byIndex].GetMtId())
            {
                StaticLog( "\tMemIdx[%d]:\n", byIndex);
                StaticLog( "\t\tMcuId:\t\t%d\n\t\tMtId:\t\t%d\n\t\tMemType:\t%d\n\t\tMemStatus:\t%d\n\t\tRealPayLoad:\t%d\n\t\tActPayLoad:\t%d\n\t\tEncrptMode:\t%d\n", 
					m_atMtMember[byIndex].GetMcuId(), m_atMtMember[byIndex].GetMtId(), 
					m_atMtMember[byIndex].GetMemberType(), m_atMtMember[byIndex].GetMemStatus(),
					m_tDoublePayload[byIndex].GetRealPayLoad(), m_tDoublePayload[byIndex].GetActivePayload(),
					m_tVideoEncrypt[byIndex].GetEncryptMode());
            }            
        }        
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TRecRtcpBack
{
protected:
    u32  m_tVideoIp;      //视频RTCP回馈地址
    u16  m_tVideoPort;    //视频RTCP回馈端口
    u32  m_tAudioIp;      //音频RTCP回馈地址
    u16  m_tAudioPort;    //音频RTCP回馈端口
    u32  m_tDStreamIp;    //双流RTCP回馈地址
    u16  m_tDStreamPort;  //双流RTCP回馈端口

public:
    TRecRtcpBack(void)
	{
		Reset();
	}
	void Reset(void)
	{
		m_tVideoIp = 0xFFFFFFFF;
        m_tVideoPort = 0xFFFF;
        m_tAudioIp = 0xFFFFFFFF;
        m_tAudioPort = 0xFFFF;
        m_tDStreamIp = 0xFFFFFFFF;
        m_tDStreamPort = 0xFFFF;
	}

    void SetVideoAddr(u32 dwVideoIp, u16 wVideoPort)
    {
        m_tVideoIp = htonl(dwVideoIp);
        m_tVideoPort = htons(wVideoPort);
    }

    void GetVideoAddr(u32 &dwVideoIp, u16 &wVideoPort)
    {
        dwVideoIp = ntohl(m_tVideoIp);
        wVideoPort = ntohs(m_tVideoPort);
    }

    
    void SetAudioAddr(u32 dwAudioIp, u16 wAudioPort)
    {
        m_tAudioIp = htonl(dwAudioIp);
        m_tAudioPort = htons(wAudioPort);
    }

    void GetAudioAddr(u32 &dwAudioIp, u16 &wAudioPort)
    {
        dwAudioIp = ntohl(m_tAudioIp);
        wAudioPort = ntohs(m_tAudioPort);
    }

    
    void SetDStreamAddr(u32 dwDStreamIp, u16 wDStreamPort)
    {
        m_tDStreamIp = htonl(dwDStreamIp);
        m_tDStreamPort = htons(wDStreamPort);
    }

    void GetDStreamAddr(u32 &dwDStreamIp, u16 &wDStreamPort)
    {
        dwDStreamIp = ntohl(m_tDStreamIp);
        wDStreamPort = ntohs(m_tDStreamPort);
    }
}
#ifndef WIN32
__attribute__((packed))
#endif
;

//录像机信道状态
struct TRecChnnlStatus
{
	/*通道的状态*/
	enum ERecChnnlState
	{
		STATE_IDLE       = 0,	  /*表明是一个未用通道*/

		STATE_CALLING    = 5,	  /*表明vrs新录播正在呼叫实体*/

		STATE_RECREADY   = 11,	  /*准备录像状态  */
		STATE_RECORDING  = 12,	  /*正在录像  */
		STATE_RECPAUSE   = 13,	  /*暂停录像状态*/

		STATE_PLAYREADY  = 21,	  /*准备播放状态 */
		STATE_PLAYREADYPLAY = 22,   /*作好播放准备状态*/
		STATE_PLAYING    = 23,	  /*正在或播放*/
		STATE_PLAYPAUSE  = 24,	  /*暂停播放*/
		STATE_FF         = 25,	  /*快进(仅播放通道有效)*/
		STATE_FB         = 26	  /*快退(仅播放通道有效)*/
	};

	/*通道类型定义*/
	enum ERecChnnlType
	{
		TYPE_UNUSE      =  0,	/*未始用的通道*/
		TYPE_RECORD     =  1,	/*录像通道  */   
		TYPE_PLAY       =  2	/*播放通道  */
	};

	//录像方式
	enum ERecMode
	{
		MODE_SKIPFRAME = 0,//抽帧录像
		MODE_REALTIME  = 1  //实时录像
	};
public:
	u8  		m_byType;		//通道类型：ERecChnnlType
	u8  		m_byState;		//通道状态：ERecChnnlState
	u8          m_byRecMode;    //录像方式:
	TRecProg	m_tProg;	//当前录像或放像进度
	TMtNoConstruct			m_tSrc;
protected:
	char	m_achRecordName[MAXLEN_RECORD_NAME];//记录名
	
public:
	//获取记录名
	LPCSTR GetRecordName( void ) const	{ return m_achRecordName; }
	//设置记录名
	void SetRecordName( LPCSTR lpszName ) 
	{
		strncpy( m_achRecordName, lpszName, sizeof( m_achRecordName ) );
		m_achRecordName[sizeof( m_achRecordName ) - 1] ='\0';
	}

	LPCSTR GetStatusStr( u8 eStatus )
	{
		switch(eStatus) 
		{
		case STATE_IDLE:
			return "IDLE";
			break;
		case STATE_CALLING:
			return "CALLING";
			break;
		case STATE_RECREADY:
			return "REC Ready";
			break;
		case STATE_RECORDING:
			return "Recording";
			break;
		case STATE_RECPAUSE:
			return "REC Pause";
			break;
		case STATE_PLAYREADY:
			return "PLAY Ready";
			break;
		case STATE_PLAYING:
			return "Playing";
			break;
		case STATE_PLAYPAUSE:
			return "Play Pause";
			break;
		case STATE_FF:
			return "Play FF";
			break;
		case STATE_FB:
			return "Play FB";
		default:
			return "UNKOWN";
		}
	}

	void print()
	{
        StaticLog( "[%s]:\n ChlType.%d  State.%s  byRecMode.%d  Prog.(%d/%d)  tSrc(%d,%d)\n",
			GetRecordName(), m_byType, GetStatusStr(m_byState), m_byRecMode, m_tProg.GetCurProg(), m_tProg.GetTotalTime(),
			m_tSrc.GetMcuId(), m_tSrc.GetMtId());
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//录像机状态, size = 2412
struct TRecStatus
{
protected:
	u8  	m_byRecChnnlNum;		//录像信道配置数
	u8  	m_byPlayChnnlNum;		//放像信道配置数
	u8      m_bSupportPublic;       // 是否支持发布(TRUE: 支持, FALSE:不支持)
	u8      m_byRemain;             // 保留
 	u32   	m_dwFreeSpace;        //录像机剩余磁盘空间(单位为MB)
	u32   	m_dwTotalSpace;		//录像机总磁盘空间(单位为MB)
	TRecChnnlStatus m_tChnnlStatus[MAXNUM_RECORDER_CHNNL];	//先录像信道后放像信道

public:
	//获取录像通道配置
	u8   GetRecChnnlNum( void ) const	{ return( m_byRecChnnlNum ); }
	//获取放像通道配置
	u8   GetPlayChnnlNum( void ) const	{ return( m_byPlayChnnlNum ); }
	//配置通道数，录像和放像通道总数必须不大于MAXNUM_RECORDER_CHNNL
	//失败返回FALSE
	BOOL SetChnnlNum( u8   byRecChnnlNum, u8   byPlayChnnlNum )
	{
		if( byRecChnnlNum + byPlayChnnlNum <= MAXNUM_RECORDER_CHNNL )
		{
			m_byRecChnnlNum = byRecChnnlNum;
			m_byPlayChnnlNum = byPlayChnnlNum;
			return( TRUE );
		}
		else
		{
			OspPrintf( TRUE, FALSE, "TRecStatus: Exception - Wrong recorder and play channel number: %u and %u!\n", 
				byRecChnnlNum, byPlayChnnlNum );
			return( FALSE );
		}
	}
		/*====================================================================
    功能        ：设置录像机剩余磁盘空间
    输入参数说明：u32    dwFreeSpace  - 剩余空间大小(单位 Mb)
    返回值说明  ：无
	====================================================================*/
	void SetFreeSpaceSize( u32    dwFreeSpace )
	{
		m_dwFreeSpace  = htonl(dwFreeSpace);
	}
	
	/*====================================================================
    功能        ：获取录像机剩余磁盘空间
    输入参数说明：无
    返回值说明  ：剩余空间大小(单位 MB)
	====================================================================*/
	u32    GetFreeSpaceSize( void ) const	{ return ntohl( m_dwFreeSpace ); }

		/*====================================================================
    功能        ：设置录像机总磁盘空间
    输入参数说明：u32    dwTotalSpace  - 总空间大小(单位 MB)
    返回值说明  ：无
	====================================================================*/
	void SetTotalSpaceSize( u32    dwTotalSpace )
	{
		m_dwTotalSpace  = htonl( dwTotalSpace );
	}
	// 是否支持发布
	BOOL IsSupportPublic() const { return m_bSupportPublic;}
    // 设置是否支持发布
	void SetPublicAttribute(BOOL bSupportPublic ) { m_bSupportPublic = bSupportPublic;}

	/*====================================================================
    功能        ：获取录像机总磁盘空间
    输入参数说明：无
    返回值说明  ：总空间大小(单位 MB)
	====================================================================*/
	u32    GetTotalSpaceSize( void ) const	{ return ntohl( m_dwTotalSpace ); }

	//设置录像机信道状态信息，必须先配置录放像信道数再调用此函数
	BOOL SetChnnlStatus( u8   byChnnlIndex, u8   byChnnlType, const TRecChnnlStatus * ptStatus );
	//设置录像机信道状态信息，必须先配置录放像信道数再调用此函数
	BOOL GetChnnlStatus( u8   byChnnlIndex, u8   byChnnlType, TRecChnnlStatus * ptStatus ) const;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// 包体积应比较小，否则数组过大
#define RECLIST_PACKSIZE    (u8)16
struct TRecFileListNotify
{
	u16 wListSize;//录像机中文件总数
	u16 wStartIdx;//本通知中起始文件序号
	u16 wEndIdx;  //本通知中结束文件序号
    
	s8 achFileName[RECLIST_PACKSIZE][MAXLEN_CONFNAME+MAXLEN_RECORD_NAME+1];
	u8 abyStatus[RECLIST_PACKSIZE];
public:
	TRecFileListNotify()
	{
		wListSize = 0;
		wStartIdx = 0;
		wEndIdx   = 0;
		memset ( achFileName ,0,sizeof(achFileName) );
		memset( abyStatus ,0 ,sizeof(abyStatus) );
	}

	//获取录像机记录总数
	u16 GetListSize()
	{
		return ntohs(wListSize);
	}
	
	//获取本通知中起始文件序号
	u16 GetStartIdx()
	{
		return ntohs(wStartIdx);
	}

	//获取本通知中起始文件序号
	u16 GetEndIdx()
	{
		return ntohs(wEndIdx);
	}

	//获得指定序号的文件名,如不存在返回NULL
	char* GetFileName(u16 wIdx )
	{
		if( wIdx < GetStartIdx() || wIdx > GetEndIdx() )
			return NULL;
		return achFileName[wIdx - GetStartIdx()];
	}

	//判断指定序号的文件是否发布
	BOOL IsPublic(u16 wIdx)
	{
		if( wIdx < GetStartIdx() || wIdx > GetEndIdx() )
			return FALSE;
		return abyStatus[wIdx - GetStartIdx()]&0x1 ? TRUE:FALSE;
	}

	//设置指定序号的文件发布
	BOOL Public(u16 wIdx)
	{
		if( wIdx < GetStartIdx() || wIdx > GetEndIdx() )
			return FALSE;

		abyStatus[wIdx - GetStartIdx()] |=0x1;
		return TRUE;
	}

	/////////////////////////////////////////////
	//设置录像机记录总数
	void Reset()
	{
		wListSize = 0;
	    wStartIdx =0;
	    wEndIdx =0;
	    memset( achFileName,0,sizeof(achFileName) );
		memset( abyStatus , 0,sizeof(abyStatus) );
	}
	void SetListSize( u16 wSize)
	{
		wListSize = htons(wSize);
	}
	
	//设置本通知中起始文件序号
	void SetStartIdx( u16 wIdx)
	{
		wStartIdx = htons(wIdx);
	}

	//设置本通知中起始文件序号
	void SetEndIdx( u16 wIdx)
	{
		wEndIdx = htons(wIdx);
	}

	// 添加文件
	BOOL AddFile(char* filename ,BOOL bPublic = FALSE )
	{
		if ( IsFull() )
			return FALSE;

        u16 wIdx = GetEndIdx() - GetStartIdx();

		strncpy( achFileName[wIdx], filename ,sizeof(achFileName[wIdx]) - 1);
		achFileName[wIdx][sizeof(achFileName)-1]='\0';

		if ( bPublic ) 
        {
            abyStatus[wIdx] |=0x1;
        }
		else 
        {
            abyStatus[wIdx] &=~0x01;
        }

        // 序号自动增量
        SetEndIdx( GetEndIdx() + 1 );
		return TRUE;
	}

	//判断缓存是否已满
	BOOL32 IsFull()
	{
		u16 wSize = GetEndIdx() - GetStartIdx();
		if( wSize >= RECLIST_PACKSIZE )
			return TRUE;
		return FALSE;
	}

    BOOL32 IsEmpty()
    {
        return (GetEndIdx() == GetStartIdx());
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

#define RECLIST_PACKSIZE    (u8)16
#define MAXLEN_FILENAME_LEN (u16)256
struct TRecListNewNotify
{
	u16 wListSize;//录像机中文件总数
	u16 wStartIdx;//本通知中起始文件序号
	u16 wEndIdx;  //本通知中结束文件序号
    
	s8 achFileName[RECLIST_PACKSIZE][MAXLEN_FILENAME_LEN];
	u8 abyStatus[RECLIST_PACKSIZE];
public:
	TRecListNewNotify()
	{
		wListSize = 0;
		wStartIdx = 0;
		wEndIdx   = 0;
		memset ( achFileName ,0,sizeof(achFileName) );
		memset( abyStatus ,0 ,sizeof(abyStatus) );
	}

	//获取录像机记录总数
	u16 GetListSize()
	{
		return ntohs(wListSize);
	}
	
	//获取本通知中起始文件序号
	u16 GetStartIdx()
	{
		return ntohs(wStartIdx);
	}

	//获取本通知中起始文件序号
	u16 GetEndIdx()
	{
		return ntohs(wEndIdx);
	}

	//获得指定序号的文件名,如不存在返回NULL
	char* GetFileName(u16 wIdx )
	{
		if( wIdx < GetStartIdx() || wIdx > GetEndIdx() )
			return NULL;
		return achFileName[wIdx - GetStartIdx()];
	}

	//判断指定序号的文件是否发布
	BOOL IsPublic(u16 wIdx)
	{
		if( wIdx < GetStartIdx() || wIdx > GetEndIdx() )
			return FALSE;
		return abyStatus[wIdx - GetStartIdx()]&0x1 ? TRUE:FALSE;
	}

	//设置指定序号的文件发布
	BOOL Public(u16 wIdx)
	{
		if( wIdx < GetStartIdx() || wIdx > GetEndIdx() )
			return FALSE;

		abyStatus[wIdx - GetStartIdx()] |=0x1;
		return TRUE;
	}

	/////////////////////////////////////////////
	//设置录像机记录总数
	void Reset()
	{
		wListSize = 0;
	    wStartIdx =0;
	    wEndIdx =0;
	    memset( achFileName,0,sizeof(achFileName) );
		memset( abyStatus , 0,sizeof(abyStatus) );
	}
	void SetListSize( u16 wSize)
	{
		wListSize = htons(wSize);
	}
	
	//设置本通知中起始文件序号
	void SetStartIdx( u16 wIdx)
	{
		wStartIdx = htons(wIdx);
	}

	//设置本通知中起始文件序号
	void SetEndIdx( u16 wIdx)
	{
		wEndIdx = htons(wIdx);
	}

	// 添加文件
	BOOL AddFile(char* filename ,BOOL bPublic = FALSE )
	{
		if ( IsFull() )
			return FALSE;

        u16 wIdx = GetEndIdx() - GetStartIdx();

		strncpy( achFileName[wIdx], filename ,sizeof(achFileName[wIdx]) - 1);
		achFileName[wIdx][sizeof(achFileName)-1]='\0';

		if ( bPublic ) 
        {
            abyStatus[wIdx] |=0x1;
        }
		else 
        {
            abyStatus[wIdx] &=~0x01;
        }

        // 序号自动增量
        SetEndIdx( GetEndIdx() + 1 );
		return TRUE;
	}

	//判断缓存是否已满
	BOOL32 IsFull()
	{
		u16 wSize = GetEndIdx() - GetStartIdx();
		if( wSize >= RECLIST_PACKSIZE )
			return TRUE;
		return FALSE;
	}

    BOOL32 IsEmpty()
    {
        return (GetEndIdx() == GetStartIdx());
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TPlayFileAttrib
{
protected:
    u8 m_byFileType;
    u8 m_byAudioType;
    u8 m_byVideoType;
    u8 m_byDVideoType;

public:
    TPlayFileAttrib(void)
    {
        m_byFileType = RECFILE_NORMAL;
        m_byAudioType = MEDIA_TYPE_NULL;
        m_byVideoType = MEDIA_TYPE_NULL;
        m_byDVideoType = MEDIA_TYPE_NULL;
    }

    BOOL32 IsDStreamFile(void) const
    {
        return (RECFILE_DSTREAM == m_byFileType) ? TRUE : FALSE;
    }
    u8 GetFileType(void) const
    {
        return m_byFileType;
    }
    void SetFileType(u8 byFileType)
    {
        m_byFileType = byFileType;
    }

    u8 GetAudioType(void) const
    {
        return m_byAudioType;
    }
    void SetAudioType(u8 byAudioType)
    {
        m_byAudioType = byAudioType;
    }

    u8 GetVideoType(void) const
    {
        return m_byVideoType;
    }
    void SetVideoType(u8 byVideoType)
    {
        m_byVideoType = byVideoType;
    }

    u8 GetDVideoType(void) const
    {
        return m_byDVideoType;
    }
    void SetDVideoType(u8 byDVideoType)
    {
        m_byDVideoType = byDVideoType;
    }
    
    void Print() const
    {
        StaticLog( "m_byFileType: %d\n", m_byFileType);
        StaticLog( "m_byAudioType: %d\n", m_byAudioType);
        StaticLog( "m_byVideoType: %d\n", m_byVideoType);
        StaticLog( "m_byDVideoType: %d\n", m_byDVideoType);
    }
}
#ifndef WIN32
__attribute__ ((packed))
#endif
;

// zgc, 2008-08-02, MCU的放像文件媒体信息
struct TPlayFileMediaInfo
{
    enum emStreamType
    {
        emAudio = 0,
        emVideo,
        emDVideo,
        emEnd
    };

private:
    //流的个数
    u8 m_byNum;
    //流的类型
    u8 m_abyMediaType[emEnd];
    u16	m_awWidth[emEnd];
	u16 m_awHeight[emEnd];

public:
    void clear( void )
    {
        m_byNum = 0;
        for ( u8 byLop = 0; byLop < emEnd; byLop++ )
        {
            m_abyMediaType[byLop] = MEDIA_TYPE_NULL;
            m_awWidth[byLop] = 0;
            m_awHeight[byLop] = 0;
        }
    }
    void SetStreamNum( u8 byNum ) { m_byNum = byNum; }
    u8   GetStreamNum( void ) const { return m_byNum; }
    void SetAudio( u8 byAudioType ) { m_abyMediaType[emAudio] = byAudioType; }
    u8   GetAudio( void ) const { return m_abyMediaType[emAudio]; }
    void SetVideo( u8 byVideoType, u16 wWidth, u16 wHeight ) 
    { 
        m_abyMediaType[emVideo] = byVideoType; 
        m_awWidth[emVideo] = htons(wWidth);
        m_awHeight[emVideo] = htons(wHeight);
        return;
    }
    void GetVideo( u8 &byVideoType, u16 &wWidth, u16 &wHeight ) const 
    { 
        byVideoType = m_abyMediaType[emVideo]; 
        wWidth = ntohs(m_awWidth[emVideo]);
        wHeight = ntohs(m_awHeight[emVideo]);
        return;
    }
    void SetDVideo( u8 byDVideoType, u16 wWidth, u16 wHeight ) 
    { 
        m_abyMediaType[emDVideo] = byDVideoType; 
        m_awWidth[emDVideo] = htons(wWidth);
        m_awHeight[emDVideo] = htons(wHeight);
        return;
    }
    void GetDVideo( u8 &byDVideoType, u16 &wWidth, u16 &wHeight ) const 
    { 
        byDVideoType = m_abyMediaType[emDVideo]; 
        wWidth = ntohs(m_awWidth[emDVideo]);
        wHeight = ntohs(m_awHeight[emDVideo]);
        return;
    }
}
#ifndef WIN32
__attribute__ ((packed))
#endif
;

/*----------------------------------------------------------------------
结构名：CConfVidTypeMgr
用途  ：码流接收类型管理
说明  ：接收对象目前仅包括终端和录象机
----------------------------------------------------------------------*/
/*
class CConfVidTypeMgr
{  
public:
    enum emMediaType
    {        
        emH264_1080 = 0,
        emH264_720,
        emH264_4CIF,
        emH264_CIF,
        emOther, // 主视频格式/双流同主视频格式

		emH239H264_UXGA,
        emH239H264_SXGA,
        emH239H264_XGA,
        emH239H264_SVGA,
        emH239H264_VGA,
        emDVidOther // 双流格式
    };
    
public:
    u8 m_abyMediaType[emOther+1];
    u8 m_abyDualMediaType[emDVidOther+1];     // 双流不考虑H264以外格式，Other仅作保存
    
public:
    CConfVidTypeMgr () 
    { 
        memset( m_abyMediaType, 0, sizeof(m_abyMediaType) ); 
        memset( m_abyDualMediaType, 0, sizeof(m_abyDualMediaType) );
    }

    void ConvertIn2Out( u8 emType, u8 &byOutType, u8 &byOutRes )
    {
        emMediaType emIntype = (emMediaType)emType;
        if ( emIntype == emOther ||
             emIntype == emDVidOther )
        {
            byOutType = MEDIA_TYPE_NULL;
            byOutRes = 0;
            return;
        }
        else
        {
            byOutType = MEDIA_TYPE_H264;
        }

        switch( emIntype )
        {
        case emH264_1080:
            byOutRes = VIDEO_FORMAT_HD1080;
            break;
        case emH264_720:
            byOutRes = VIDEO_FORMAT_HD720;
            break;
        case emH264_4CIF:
            byOutRes = VIDEO_FORMAT_4CIF;
            break;
        case emH264_CIF:
            byOutRes = VIDEO_FORMAT_CIF;
            break;
        case emH239H264_VGA:
            byOutRes = VIDEO_FORMAT_VGA;
            break;
        case emH239H264_SVGA:
            byOutRes = VIDEO_FORMAT_SVGA;
            break;
        case emH239H264_XGA:
            byOutRes = VIDEO_FORMAT_XGA;
            break;
        case emH239H264_SXGA:
            byOutRes = VIDEO_FORMAT_SXGA;
            break;
		case emH239H264_UXGA:
			byOutRes = VIDEO_FORMAT_UXGA;
			break;
        default:
            byOutType = MEDIA_TYPE_NULL;
            byOutRes = 0;
            break;
        }
        return;
    }
    
    u8 ConvertOut2In( u8 byOutType, u8 byOutRes, BOOL32 bDVid = FALSE )
    {
        emMediaType emIntype;
        switch( byOutType )
        {
        case MEDIA_TYPE_H264:
            {
                switch( byOutRes )
                {
                case VIDEO_FORMAT_CIF:
                    emIntype = emH264_CIF;
                    break;
                case VIDEO_FORMAT_4CIF:
                    emIntype = emH264_4CIF;
                    break;
                case VIDEO_FORMAT_HD720:
                    emIntype = emH264_720;
                    break;
                case VIDEO_FORMAT_HD1080:
                    emIntype = emH264_1080;
                    break;
                case VIDEO_FORMAT_VGA:
                    emIntype = emH239H264_VGA;
                    break;
                case VIDEO_FORMAT_SVGA:
                    emIntype = emH239H264_SVGA;
                    break;
                case VIDEO_FORMAT_XGA:
                    emIntype = emH239H264_XGA;
                    break;
                case VIDEO_FORMAT_SXGA:
                    emIntype = emH239H264_SXGA;
                    break;
				case VIDEO_FORMAT_UXGA:
					emIntype = emH239H264_UXGA;
					break;
                default:
                    emIntype = bDVid ? emDVidOther : emOther;
                    break;
                }
            }
            break;
        default:
            emIntype = bDVid ? emDVidOther : emOther;
            break;
        }
        return (u8)emIntype;
    }
    
    BOOL32 IsTypeExist( u8 emType, BOOL32 bDVid = FALSE )
    {
        if ( !bDVid )
        {
            if ( !IsVGAType(emType) )
            {
                return ( m_abyMediaType[(emMediaType)emType] > 0 );
            }
            else
            {
                return FALSE;
            }
        }
        else
        {
            return ( m_abyDualMediaType[(emMediaType)emType] > 0 );
        }
    }

    void AddType( u8 emType, BOOL32 bDVid = FALSE )
    {
        if ( !bDVid )
        {
            if ( !IsVGAType(emType) )
            {
                m_abyMediaType[(emMediaType)emType]++;
            }
        }
        else
        {
            m_abyDualMediaType[(emMediaType)emType]++;
        }
        return;
    }

    void DelType( u8 emType, BOOL32 bDVid = FALSE )
    {
        if ( IsTypeExist(emType, bDVid) )
        {
            if ( !bDVid )
            {
                m_abyMediaType[(emMediaType)emType]--;
            }
            else
            {
                m_abyDualMediaType[(emMediaType)emType]--;
            }
        }
        return;
    }
    
    u8 GetTypeNum(BOOL32 bDVid = FALSE)
    {
        u8 byNum = 0;
        if ( !bDVid )
        {          
            for( u8 byLop = 0; byLop < sizeof(m_abyMediaType); byLop++ )
            {
                if ( m_abyMediaType[(emMediaType)byLop] > 0 )
                {
                    byNum++;
                }
            }
        }
        else
        {
            for( u8 byLop = 0; byLop < sizeof(m_abyDualMediaType); byLop++ )
            {
                if ( m_abyDualMediaType[(emMediaType)byLop] > 0 )
                {
                    byNum++;
                }
            }
        }
        return byNum;
    }
    
    u8 GetH264TypeNum(BOOL32 bDVid = FALSE)
    {
        u8 byNum = 0;
        if ( !bDVid )
        {          
            for( u8 byLop = 0; byLop < sizeof(m_abyMediaType); byLop++ )
            {
                if ( m_abyMediaType[(emMediaType)byLop] > 0 && (emMediaType)byLop != emOther )
                {
                    byNum++;
                }
            }
        }
        else
        {
            for( u8 byLop = 0; byLop < sizeof(m_abyDualMediaType); byLop++ )
            {
                if ( m_abyDualMediaType[(emMediaType)byLop] > 0 &&
                     (emMediaType)byLop != emOther &&
                     (emMediaType)byLop != emDVidOther )
                {
                    byNum++;
                }
            }
        }
        return byNum;
    }

    u8 GetAdaptTypeNum( u8 bySrcOutType, u8 bySrcOutRes, BOOL32 bDVid = FALSE )
    {
        u8 emType = ConvertOut2In( bySrcOutType, bySrcOutRes, bDVid );
        u8 byRecvTypeNum = GetAdaptTypeNum( emType, bDVid );
        return byRecvTypeNum;
    }

    u8 GetAdaptTypeNum( u8 emSrcType, BOOL32 bDVid = FALSE )
    {
        u8 byRecvTypeNum = 0;
        if ( !bDVid )
        {
            for ( u8 byLop = (emMediaType)emSrcType+1; byLop < sizeof(m_abyMediaType); byLop++ )
            {
                if ( IsTypeExist(byLop, bDVid) )
                {
                    byRecvTypeNum++;
                }
            }
        }
        else
        {
            for ( u8 byLop = (emMediaType)emSrcType+1; byLop < sizeof(m_abyDualMediaType); byLop++ )
            {
                if ( IsTypeExist(byLop, bDVid) &&
                     // 适配目的应该和源的双流类型相同
                     IsVGAType(emSrcType) == IsVGAType(byLop) &&
                     byLop != emOther && byLop != emDVidOther )
                {
                    byRecvTypeNum++;
                }
                if ( IsVGAType(emSrcType) != IsVGAType(byLop) )
                {
                    break;
                }
            }
        }
        return byRecvTypeNum;
    }

    BOOL32 IsVGAType( u8 emType )
    {
        emMediaType emInType = (emMediaType)emType;
        return ( emInType <= emOther ) ? FALSE : TRUE;
    }

    u8  GetMinH264AdaptType( u8 emSrcType, BOOL32 bDVid = FALSE )
    {
        emMediaType emIntype = (emMediaType)emSrcType;
        emMediaType emMinType = emIntype;
        if ( !bDVid )
        {
            for ( u8 byLop = emIntype+1; byLop < emOther; byLop++ )
            {
                if ( IsTypeExist(byLop, bDVid) )
                {
                    emMinType = (emMediaType)byLop;
                }
            }
        }
        else
        {
            for ( u8 byLop = emIntype+1; byLop < emDVidOther; byLop++ )
            {
                if ( IsTypeExist(byLop, bDVid) && byLop != emOther )
                {
                    emMinType = (emMediaType)byLop;
                }
            }
        }

        return (u8)emMinType;
    }
	void Print( void )
	{
		u8 byType = 0;

		StaticLog( "主流待适配的群组为：\n" );
		for(byType = emH264_1080; byType <= emOther; byType ++)
		{
			if (m_abyMediaType[byType] != 0)
			{
				StaticLog( "\tType.%d, num.%d\n", byType, m_abyMediaType[byType] );
			}
		}
		StaticLog( "\n" );

		StaticLog( "双流待适配的群组为：\n" );
		for(byType = emH239H264_UXGA; byType <= emDVidOther; byType ++)
		{
			if (m_abyDualMediaType[byType] != 0)
			{
				StaticLog( "\tType.%d, num.%d\n", byType, m_abyDualMediaType[byType] );
			}
		}
		StaticLog( "\n" );
	}
}
#ifndef WIN32
__attribute__ ((packed))
#endif
;*/

// [12/23/2009 xliang] --------------------------------------------8000E
//modify bas 2
//适配器启动参数结构
/*
struct T8KEAdaptParam
{
protected:
	u8         m_byCodeType;  //适配输出类型
	u8         m_byActiveType;//适配输出动态载荷类型

	u16 	   m_wBitRate;//发送码率
	u16 	   m_wWidth; //图像宽度
	u16 	   m_wHeight;//图像宽度
	
	u8         m_byNeedPrs;

    u8         m_byFps; //发送帧率, zgc, 2009-03-16

public:
    u16  GetWidth(void) const	{ return (ntohs(m_wWidth)); };
    u16  GetHeight(void) const	{ return (ntohs(m_wHeight)); };
    u16  GetBitrate(void) const { return (ntohs(m_wBitRate));};
    void SetResolution(u16 wWidth, u16 wHeight)
    {
        m_wWidth = htons(wWidth);
        m_wHeight = htons(wHeight);
    }
	
	void SetBitRate(u16 wBitrate)
	{
	    m_wBitRate = htons(wBitrate);
	}
	
	void SetCodeType(u8 byType)
	{
		m_byCodeType = byType;
	}
	u8 GetCodeType(void){ return m_byCodeType ;}

	void SetActiveType(u8 byType)
	{
		m_byActiveType = (u8) byType;
	}
	u8 GetActiveType(void){ return m_byActiveType; }

	void SetIsNeedByPrs(BOOL32 bNeedPrs)
	{
		m_byNeedPrs = (TRUE == bNeedPrs) ? 1 : 0;
	}

	BOOL32 IsNeedbyPrs(void)
	{
		return (0 != m_byNeedPrs);
	}

    void SetFps(u8 byFps)
    {
        m_byFps = byFps;
    }
	u8 GetFps(void){ return m_byFps; }
    
    u8 GetMeidaMode(void)
    {
        u8 byMediaMode = MODE_NONE;
        switch( m_byCodeType ) 
        {
        case MEDIA_TYPE_H261:
        case MEDIA_TYPE_H262:
        case MEDIA_TYPE_H263:
        case MEDIA_TYPE_H263PLUS:
        case MEDIA_TYPE_H264:
        case MEDIA_TYPE_MP4:
            byMediaMode = MODE_VIDEO;
            break;
        case MEDIA_TYPE_PCMA:
        case MEDIA_TYPE_PCMU:
        case MEDIA_TYPE_G722:
        case MEDIA_TYPE_G7221C:
        case MEDIA_TYPE_G7231:
        case MEDIA_TYPE_G728:
        case MEDIA_TYPE_G729:
        case MEDIA_TYPE_MP3:
            byMediaMode = MODE_AUDIO;
            break;
        default:
            break;
        }
	    return byMediaMode;
    }
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;
*/


//modify bas 2
//适配器启动参数结构
struct TAdaptParam
{
    enum EVideoType
    {
        vNONE  = MEDIA_TYPE_NULL,//视频不进行适配
        vMPEG4 = MEDIA_TYPE_MP4,
        vH261  = MEDIA_TYPE_H261,
        vH263  = MEDIA_TYPE_H263,
        vH264  = MEDIA_TYPE_H264,
    };
    enum EAudioType
    {
        aNONE  = MEDIA_TYPE_NULL,//音频不进行适配
        aMP3   = MEDIA_TYPE_MP3,
        aPCMA  = MEDIA_TYPE_PCMA,
        aPCMU  = MEDIA_TYPE_PCMU,
        aG7231 = MEDIA_TYPE_G7231,
        aG728  = MEDIA_TYPE_G728,
        aG729  = MEDIA_TYPE_G729,
    };

public:
    u16  GetWidth(void) const	{ return (ntohs(m_wWidth)); };
    u16  GetHeight(void) const	{ return (ntohs(m_wHeight)); };
    u16  GetBitrate(void) const { return (ntohs(m_wBitRate));};

    void SetResolution(u16 wWidth, u16 wHeight)
    {
        m_wWidth = htons(wWidth);
        m_wHeight = htons(wHeight);
    }
	
	void SetBitRate(u16 wBitrate)
	{
	    m_wBitRate = htons(wBitrate);
	}
	
	void SetVidType(u8 eType)
	{
		m_byVidCodeType = (u8)eType;
	}
	u8 GetVidType(void){ return m_byVidCodeType ;}
	
	void SetAudType(u8 eType)
	{
		m_byAudCodeType = (u8)eType;
	}
	u8 GetAudType(void){ return m_byAudCodeType; }

	void SetVidActiveType(u8 eType)
	{
		m_byVidActiveType = (u8)eType;
	}
	u8 GetVidActiveType(void){ return m_byVidActiveType; }
	
	void SetAudActiveType(u8 eType)
	{
		m_byAudActiveType = (u8)eType;
	}
	u8 GetAudActiveType(void){ return m_byAudActiveType; }

	void SetIsNeedByPrs(BOOL32 bNeedPrs)
	{
		m_byNeedPrs = (TRUE == bNeedPrs) ? 1 : 0;
	}

	BOOL32 IsNeedbyPrs(void)
	{
		return (0 != m_byNeedPrs);
	}

    BOOL32 IsNull(void) const
    {
        return (MEDIA_TYPE_NULL == m_byAudCodeType || 0 == m_byAudCodeType ) &&
               (MEDIA_TYPE_NULL == m_byVidCodeType || 0 == m_byVidCodeType );
    }
    void Clear(void)
    {
        m_byAudCodeType = MEDIA_TYPE_NULL;
        m_byVidCodeType = MEDIA_TYPE_NULL;
        m_byAudActiveType = MEDIA_TYPE_NULL;
        m_byVidActiveType = MEDIA_TYPE_NULL;
        m_wBitRate = 0;
        m_wWidth = 0;
        m_wHeight = 0;
        m_byNeedPrs = 0;
    }

	// [11/1/2011 liuxu] 打印调整
	void PrintTitle() const 
	{
        StaticLog( "%8s %8s %8s %8s %8s %8s %8s %8s\n", 
			"AudCodeT", "VidCodeT", "AudActT", "VidActT",
			"Bitrate", "Width", "Height", "NeedPrs" );
	}

    void Print(void) const
    {
		StaticLog( "%8d %8d %8d %8d %8d %8d %8d %8d\n", 
			m_byAudCodeType, m_byVidCodeType, m_byAudActiveType, m_byVidActiveType,
			GetBitrate(), GetWidth(), GetHeight(), m_byNeedPrs );
    }

private:
    u8         m_byAudCodeType;  //适配音频输出类型
    u8         m_byVidCodeType;  //适配视频输出类型
    u8         m_byAudActiveType;//适配音频输出动态载荷类型
    u8         m_byVidActiveType;//适配视频输出动态载荷类型
    
    u16 	   m_wBitRate;//发送码率
    u16 	   m_wWidth; //图像宽度
    u16 	   m_wHeight;//图像宽度
    
    u8         m_byNeedPrs;
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

/*----------------------------------------------------------------------
结构名：THDAdaptParam
用途  ：HD适配参数
----------------------------------------------------------------------*/
struct THDAdaptParam : public TAdaptParam
{
public:
    u8 GetFrameRate( void ) { return m_byFrameRate; }
    void SetFrameRate(u8 byFrameRate) { m_byFrameRate = byFrameRate; }

	u8 GetProfileType( void ) { return m_byProfileType; }
    void SetProfileType(u8 byProfileType) { m_byProfileType = byProfileType; }

    void Reset(void)
    {
        Clear();
        m_byFrameRate = 0;
        m_byProfileType = 0;
        m_byReserve2 = 0;
        m_byReserve3 = 0;
    }

	// [11/1/2011 liuxu] 打印调整
	void PrintTitle() const 
	{
		StaticLog("--------------------------------------------------------------------------------\n");
		StaticLog( "%7s ", "FrmRate");
		StaticLog( "%7s ", "ProType");
		TAdaptParam::PrintTitle();
		StaticLog("--------------------------------------------------------------------------------\n");
	}
	
    void PrintHd(void) const
    {
        StaticLog( "%7d ", m_byFrameRate);
		StaticLog( "%7d ", m_byProfileType);
		TAdaptParam::Print();
    }

private:
    u8 m_byFrameRate;
    u8 m_byProfileType;
    u8 m_byReserve2;
    u8 m_byReserve3;
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;


//适配器通道状态结构
struct T8KEBasChnStatus : THDAdaptParam   //T8KEAdaptParam    //[03/01/2010] zjl modify bas 外设侧已统一调整为THDAdaptParam向mcu侧靠拢
{
    enum EBasStatus
    {
        IDLE   = 0, //空闲
        READY  = 1, //适配器未启动
        RUNING = 2  //适配器已运行
    };
    
protected:
    u8 m_byStatus;          // 通道状态
    u8 m_byIsReserved;      // 是否被占用
    u8 m_byChanType;        // 音频通道: 1(BAS_CHAN_AUDIO)  视频通道: 2(BAS_CHAN_VIDEO)
    CConfId m_cChnConfId;   // 会议ID, 该字段取消
#ifdef _CARRIER_
    u32 m_byConfIdx;
#else
    u8 m_byConfIdx;
#endif
    u8 m_byReservedBis1;    // 保留空间
    u16 m_wResvedBits2;     // 保留空间
    
public:
    void Init(void)
    {
        m_byStatus = IDLE;
        m_byIsReserved = 0;
        m_byChanType = 0;
        m_cChnConfId.SetNull();
        SetVidType(MEDIA_TYPE_NULL);
		SetAudType(MEDIA_TYPE_NULL);
    }
    
    void SetStatus( u8 byStatus) { m_byStatus = byStatus; }
    u8 GetStatus(void) const { return m_byStatus; }
    
    void SetIsReserved( BOOL32 bIsReserved ) { m_byIsReserved = (TRUE == bIsReserved) ? 1 : 0; }
    // 占用通道时用这个函数, zgc, 2009-03-27
#ifdef _CARRIER_
    void SetReserved( u32 byConfIdx ) 
#else
    void SetReserved( u8 byConfIdx )
#endif
    {
        SetIsReserved( TRUE );
        SetConfIdx( byConfIdx );
    }
    BOOL32 IsReserved(void) const { return (0 == m_byIsReserved) ? FALSE : TRUE ; }
    void UnReserved(void)
    {
        SetIsReserved( FALSE );
        SetConfIdx( 0 );
    }
    
    void SetChanType( u8 byBasChanType ) { m_byChanType = byBasChanType; }
    u8 GetChanType(void) const { return m_byChanType; }
    
    void SetConfId( const CConfId & cConfId ) { memcpy(&m_cChnConfId, &cConfId, sizeof(cConfId)); }
    CConfId GetConfId(void) const { return m_cChnConfId; }

#ifdef _CARRIER_
    u32 GetConfIdx( void ) const { return ntohl(m_byConfIdx); }
    void SetConfIdx( u32 dwConfIdx ) { m_byConfIdx = htonl(dwConfIdx); }
#else
    u8 GetConfIdx( void ) const { return m_byConfIdx; }
    void SetConfIdx( u8 byConfIdx ) { m_byConfIdx = byConfIdx; }
#endif
}
#ifndef WIN32
__attribute__ ((packed))
#endif
;



//适配器状态结构，MCU会议管理的最小单元为一个BAS。
struct T8KEBasStatus
{    
	u8            byChnNum;
    T8KEBasChnStatus tChnnl[MAXNUM_BAS_CHNNL];

    u8            byReserved;
#ifdef _CARRIER_
    u32           m_byConfIdx;
#else
    u8            m_byConfIdx;
#endif

public:
    void Reset(void)
    {
        UnReserved();
        byChnNum = 0;
        for ( u8 byIdx = 0; byIdx < MAXNUM_BAS_CHNNL; byIdx++ )
        {
            tChnnl[byIdx].Init();
        }
    }
#ifdef _CARRIER_
    void SetReserved( u32 byConfIdx )
#else
    void SetReserved( u8 byConfIdx )
#endif
    {
        byReserved = 1;
#ifdef _CARRIER_
        m_byConfIdx = htonl(byConfIdx);
#else
        m_byConfIdx = byConfIdx;
#endif
        
        // 目前会议以适配器为单位占用，而非以通道为单位，这里将通道也占用，
        // 为以后可能的修改兼容, zgc, 2009-03-27
        for ( u8 byIdx = 0; byIdx < MAXNUM_BAS_CHNNL; byIdx++ )
        {
            tChnnl[byIdx].SetReserved( byConfIdx );
        }
    }
    BOOL32 IsReserved() const
    {
        return ISTRUE(byReserved);
    }
    void UnReserved()
    {
        byReserved = 0; 
        m_byConfIdx = 0;
    }
#ifdef _CARRIER_
    u32   GetConfIdx()
    {
        return ntohl(m_byConfIdx);
    }
#else
    u8   GetConfIdx()
    {
        return m_byConfIdx;
    }
#endif
}
#ifndef WIN32
__attribute__((packed))
#endif
;

//混音组状态(8KE)
struct T8KEMixerGrpStatus
{
	enum EState 
	{
		IDLE   = 0,		//该混音组未使用 
		READY  = 1,		//该组创建,但未开始混音
		MIXING = 2,		//正在混音
	};

	u8       m_byGrpId;		  //混音组ID(0-4)
	u8       m_byGrpState;	  //组状态
	u8       m_byGrpMixDepth; //混音深度
#ifdef _CARRIER_
    u32      m_byConfIdx;      //会议ID
#else
    u8       m_byConfIdx;      //会议ID
#endif
    u8       m_byIsReserved;  //是否被占用
public:
	//所有状态复位
	void Reset()
	{
		m_byGrpId       = 0xff;
		m_byGrpState    = IDLE;
		m_byGrpMixDepth = 0;
        m_byIsReserved  = 0;
        m_byConfIdx = 0;
	}

    void SetReady()
    {
        m_byGrpState = READY;
    }
    BOOL32 IsReady() const
    {
        return ( m_byGrpState == READY ) ? TRUE : FALSE;
    }
    void SetMixing()
    {
        m_byGrpState = MIXING;
    }
    BOOL32 IsMixing() const
    {
        return ( m_byGrpState == MIXING ) ? TRUE : FALSE;
    }
#ifdef _CARRIER_
    void SetReserved( u32 dwConfIdx )
    {
        m_byIsReserved = 1;
        m_byConfIdx = htonl(dwConfIdx);
    }
    u32 GetConfIdx(void)
    {
        return ntohl(m_byConfIdx);
    }
#else
    void SetReserved( u8 byConfIdx )
    {
        m_byIsReserved = 1;
        m_byConfIdx = byConfIdx;
    }
    u8 GetConfIdx(void)
    {
        return m_byConfIdx;
    }
#endif

    BOOL32 IsReserved() const
    {
        return (m_byIsReserved == 0) ? FALSE : TRUE;
    }
    void UnReserved(void)
    {
        m_byIsReserved = 0;
        m_byConfIdx = 0;
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;



//混音器状态 8KE
struct T8KEMixerStatus
{
	u8 m_byGrpNum; //混音组数量
	T8KEMixerGrpStatus	m_atGrpStatus[MAXNUM_MIXER_GROUP_8KE];

public:
    void Reset(void)
    {
        m_byGrpNum = 0;
        for ( u32 dwIdx = 0; dwIdx < MAXNUM_MIXER_GROUP_8KE; dwIdx++ )
        {
            m_atGrpStatus[dwIdx].Reset();
        }
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;



//------------------------------------------


/*----------------------------------------------------------------------
结构名：THDBasChnStatus
用途  ：HDBas适配通道
----------------------------------------------------------------------*/
struct TBasBaseChnStatus
{
    enum EBasStatus
    {
        IDLE   = 0,    //空闲
        READY  = 1,    //准备适配
        RUNING = 2,    //正在适配
		WAIT_START = 3 //等待开启
    };

public:
    void   SetStatus( u8 byStatus ) { m_byStatus = byStatus; };
    u8     GetStatus( void ) { return m_byStatus; }
    
    BOOL32 IsNull( void ) const { return m_tBas.IsNull(); }
    void   SetNull( void ) { m_tBas.SetNull(); }
    
    u8     GetReservedType( void ) { return m_byIsReserved; }
    BOOL32 IsReserved ( void ) const { return ( 1 == m_byIsReserved ); }
    void   SetIsReserved ( BOOL32 bIsReserved ) { m_byIsReserved = bIsReserved ? 1 : 0; }
    BOOL32 IsTempReserved ( void ) const { return ( 2 == m_byIsReserved ); }
    void   SetIsTempReserved ( BOOL32 bIsTempReserved ) { m_byIsReserved = bIsTempReserved ? 2 : 0; } 
    void   UnReserved( void ) { m_byIsReserved = 0; }
    
    void   SetChnIdx( u8 byChnIdx ) { m_byChnIdx = byChnIdx; }
    u8     GetChnIdx( void ) const { return m_byChnIdx; }
    
    void   SetEqp( TEqp tBas ) { m_tBas = tBas; }
    TEqp   GetEqp( void ) const { return m_tBas; }
    u8     GetEqpId( void ) const { return m_tBas.GetEqpId(); }	
    void   SetType( u8 byType ) { m_byType = byType; };
    u8     GetType( void ) { return m_byType; }


    void   Print() const
    {
		// [11/4/2011 liuxu] 打印调整
		StaticLog("Bas Eqp:\n");
		TEqp::PrintTitle();
		m_tBas.PrintContent();

		StaticLog("\nChnnl Info:\n");
		StaticLog("--------------------------------------\n");
		StaticLog("%10s %8s %8s %8s\n", "EBasStatus", "Reserved", "ChnnlIdx", "BasType");
		StaticLog("--------------------------------------\n");
		StaticLog("%10d %8d %8d %8d\n", m_byStatus, m_byIsReserved, m_byChnIdx, m_byType);
    }

private:
    TEqpNoConstruct m_tBas;
    u8 m_byStatus;	// EBasStatus
    u8 m_byIsReserved; // 0 -- not reserve, 1 -- reserve, 2 -- temply reserve
	u8 m_byChnIdx;
    u8 m_byType; // zw 20081114
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

struct TAudAdaptParam 
{
private:
	u8	m_byAudCodeType;
	u8	m_byAudActiveTpye;
	u8	m_byTrackNum;
	u8  m_byIsNeedPrs;
	u8	m_byReserve1;
	u8	m_byReserve2;
	
public:
	//设置音频编码类型
	void SetAudCodeType( u8 byAudCodeType )
	{
		m_byAudCodeType = byAudCodeType;
	}
	
	u8 GetAudCodeType()
	{
		return m_byAudCodeType;
	}
	
	//设置音频编码活动载荷
	void SetAudActiveType( u8 byAudActiveType )
	{
		m_byAudActiveTpye = byAudActiveType;
	}
	
	u8 GetAudActiveType()
	{
		return m_byAudActiveTpye;
	}
	
	//设置音频编码声道数
	void SetTrackNum( u8 byTrackNum )
	{
		m_byTrackNum = byTrackNum;
	}
	
	u8 GetTrackNum( )
	{
		return m_byTrackNum;
	}
	
	
	void SetIsNeedByPrs(BOOL32 bNeedPrs)
	{
		m_byIsNeedPrs = (TRUE == bNeedPrs) ? 1 : 0;
	}
	
	BOOL32 IsNeedbyPrs(void)
	{
		return (0 != m_byIsNeedPrs);
	}
	
	
    void Clear(void)
    {
        m_byAudCodeType = MEDIA_TYPE_NULL;
		m_byAudActiveTpye = MEDIA_TYPE_NULL;
		m_byTrackNum = 0;
    }
	
	BOOL32 IsNull()
	{
		if ( m_byAudCodeType == MEDIA_TYPE_NULL)
		{
			return TRUE;
		}
		return FALSE;
	}
	
	void PrintTitle() const 
	{
		StaticLog("--------------------------------------------------------------------------------\n");
		StaticLog( "%7s ", "CodType");
		StaticLog( "%7s ", "ActType");
		StaticLog( "%7s ", "TrackNm");
		StaticLog( "%7s \n","NeedPrs");
		StaticLog("--------------------------------------------------------------------------------\n");
	}
	
    void PrintParam(void) const
    {
        StaticLog( "%7d ", m_byAudCodeType);
		StaticLog( "%7d ", m_byAudActiveTpye);
		StaticLog( "%7d",  m_byTrackNum);
		StaticLog( "%7d\n", m_byIsNeedPrs);
    }
	
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

//BAS音频通道状态
struct TAudBasChnStatus:public TBasBaseChnStatus
{
public:
    TAudAdaptParam  m_atOutPutAudParam[MAXNUM_BASOUTCHN];   //适配通道编码输出数组
public:
    TAudAdaptParam* GetOutputAudParam( u8 byChnIdx )
    {
        if ( byChnIdx < MAXNUM_BASOUTCHN )
        {
            return &m_atOutPutAudParam[byChnIdx]; 
        }
        else
        {
            return NULL;
        }
    }
    void SetOutputAudParam(TAudAdaptParam &tVidParam, u8 byChnIdx)
    {
        m_atOutPutAudParam[byChnIdx] = tVidParam;
        return;
    }
    
    void ClrOutputAudParam()
    {
        TAudAdaptParam tNullAdpParam;
        tNullAdpParam.Clear();  
        for (u8 byChnlIdx = 0; byChnlIdx < MAXNUM_BASOUTCHN; byChnlIdx++)
        {
            m_atOutPutAudParam[byChnlIdx] = tNullAdpParam;
        }
    }
    
    void PrintInfo() const
    {
        StaticLog( "\nAudChnStatus info as follows:\n");
        Print();
        
        StaticLog("\nChnnl Output Param:\n");
        m_atOutPutAudParam[0].PrintTitle();
        for(u8 byIdx = 0; byIdx < MAXNUM_BASOUTCHN; byIdx ++)
        {
            m_atOutPutAudParam[byIdx].PrintParam();
        }
        
        StaticLog("\n");
    }
}
#ifndef WIN32
__attribute__((packed))
#endif
;

//apu2-bas外设状态
struct TApu2BasStatus
{
public:
    TAudBasChnStatus m_atAudChnStatus[MAXNUM_APU2_BASCHN];  //APU2 BAS外设通道数组
public:
    void SetAudChnStatus (TAudBasChnStatus &tAudChnStatus, u8 byChnIdx)
    {
        if ( byChnIdx < MAXNUM_APU2_BASCHN )
        {
            m_atAudChnStatus[byChnIdx] = tAudChnStatus;
        }
        return;
    }
    TAudBasChnStatus * GetAudChnStatus(u8 byChnIdx)
    {
        if ( byChnIdx < MAXNUM_APU2_BASCHN )
        {
            return &m_atAudChnStatus[byChnIdx];
        }
        else
        {
            return NULL;
        }
    }
    void Print() const
    {
        StaticLog( "TApu2BasStatus is as follows:\n");
        u8 byIdx = 0;
        for(byIdx = 0; byIdx < MAXNUM_APU2_BASCHN; byIdx++)
        {
            if(!m_atAudChnStatus[byIdx].IsNull())
            {
                m_atAudChnStatus[byIdx].PrintInfo();
            }           
        }
    }
}
#ifndef WIN32
__attribute__((packed))
#endif
;


//apu2-bas外设状态
struct T8kiAudBasStatus
{
public:
    TAudBasChnStatus m_atAudChnStatus[MAXNUM_8KI_AUD_BASCHN];  //APU2 BAS外设通道数组
public:
    void SetAudChnStatus (TAudBasChnStatus &tAudChnStatus, u8 byChnIdx)
    {
        if ( byChnIdx < MAXNUM_8KI_AUD_BASCHN )
        {
            m_atAudChnStatus[byChnIdx] = tAudChnStatus;
        }
        return;
    }
    TAudBasChnStatus * GetAudChnStatus(u8 byChnIdx)
    {
        if ( byChnIdx < MAXNUM_8KI_AUD_BASCHN )
        {
            return &m_atAudChnStatus[byChnIdx];
        }
        else
        {
            return NULL;
        }
    }
    void Print() const
    {
        StaticLog( "T8kiAudBasStatus is as follows:\n");
        u8 byIdx = 0;
        for(byIdx = 0; byIdx < MAXNUM_8KI_AUD_BASCHN; byIdx++)
        {
            if(!m_atAudChnStatus[byIdx].IsNull())
            {
                m_atAudChnStatus[byIdx].PrintInfo();
            }           
        }
    }
}
#ifndef WIN32
__attribute__((packed))
#endif
;

/*----------------------------------------------------------------------
结构名：THDBasVidChnStatus
用途  ：HDBas视频通道记录状态
----------------------------------------------------------------------*/
struct THDBasVidChnStatus : public TBasBaseChnStatus //len:39
{
private:
	THDAdaptParam m_atOutputVidParam[MAXNUM_BASOUTCHN/*MAXNUM_VOUTPUT*/]; //[03/01/2010] zjl modify 

public:	
    THDAdaptParam * GetOutputVidParam( u8 byChnIdx )
    {
        if ( byChnIdx < MAXNUM_BASOUTCHN/*MAXNUM_VOUTPUT*/ ) //[03/01/2010] zjl modify 
        {
            return &m_atOutputVidParam[byChnIdx]; 
        }
        else
        {
            return NULL;
        }
    }
    void SetOutputVidParam(THDAdaptParam &tVidParam, u8 byChnIdx , BOOL32 bH263plusChn = FALSE, u8 byChnType = 0)
    {
// 		if (BAS_8KECHN_MV !=byChnType && 
// 			BAS_8KECHN_DS !=byChnType &&
// 			BAS_8KECHN_SEL !=byChnType &&
// 			byChnIdx < MAXNUM_VOUTPUT)
// 		{
// 			if ((tVidParam.GetVidType() != MEDIA_TYPE_H264 ||
// 				(tVidParam.GetVidType() == MEDIA_TYPE_H264 && 
// 				tVidParam.GetWidth() == 352 && tVidParam.GetHeight() == 288)
// 			 	)
// 		    	)
// 			{
// 				if (0 == byChnIdx && !bH263plusChn)
// 				{
// 					byChnIdx = 1;
// 					OspPrintf(TRUE, FALSE, "[SetOutputVidParam]change outputchanl to 1 automaticly\n");
// 				} 
// 			}
// 			else
// 			{
// 				if (byChnIdx == 1)
// 				{
// 					byChnIdx = 0;
// 					OspPrintf(TRUE, FALSE, "[SetOutputVidParam]change outputchanl to 0 automaticly\n");
// 				}
// 			}
// 			m_atOutputVidParam[byChnIdx] = tVidParam;
// 		}
// 		else if ((BAS_8KECHN_MV == byChnType || 
// 			      BAS_8KECHN_DS == byChnType || 
// 				  BAS_8KECHN_SEL == byChnType ) && 
// 				  byChnIdx < MAXNUM_8KEBAS_VOUTPUT)
// 		{
// 			m_atOutputVidParam[byChnIdx] = tVidParam;
// 			OspPrintf(TRUE, FALSE, "[SetOutputVidParam] SetOutputVidParam to 8kechn:%d!\n", byChnIdx);
// 		}
// 		else
// 		{
// 			OspPrintf(TRUE, FALSE, "[SetOutputVidParam] SetOutputVidParam failed! ChnType:%d, ChnIdx:%d, tVidParam<Enctype:%d, Width:%d, Height:%d>\n",
// 				byChnType, byChnIdx, 
// 				tVidParam.GetVidType(), tVidParam.GetWidth(), tVidParam.GetHeight());
// 		}
		m_atOutputVidParam[byChnIdx] = tVidParam;
        return;
    }

	void ClrOutputVidParam()
	{
 		THDAdaptParam tNullAdpParam;
		tNullAdpParam.Reset();  
		for (u8 byChnlIdx = 0; byChnlIdx < MAXNUM_BASOUTCHN/*MAXNUM_VOUTPUT*/; byChnlIdx++)
		{
			m_atOutputVidParam[byChnlIdx] = tNullAdpParam;
		}
	}

    void PrintInfo() const
    {
		// [11/1/2011 liuxu] 打印调整
        StaticLog( "\nHDBasVidChnStatus info as follows:\n");
        Print();
		
		StaticLog("\nChnnl Output Param:\n");
		m_atOutputVidParam[0].PrintTitle();
        for(u8 byIdx = 0; byIdx < MAXNUM_BASOUTCHN; byIdx ++)
        {
            m_atOutputVidParam[byIdx].PrintHd();
        }
		
		StaticLog("\n");
    }
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

typedef THDBasVidChnStatus THDBasDVidChnStatus;

/*----------------------------------------------------------------------
结构名：TMauBasStatus
用途  ：MauBas状态类，加进TPeriEqpStatus
----------------------------------------------------------------------*/
class TMauBasStatus
{
public:
    void SetVidChnStatus (THDBasVidChnStatus &tVidChnStatus, u8 byChnIdx = 0)
    {
        if ( byChnIdx < MAXNUM_MAU_VCHN )
        {
            m_atVidChnStatus[byChnIdx] = tVidChnStatus;
        }
        return;
    }
	void SetDVidChnStatus (THDBasDVidChnStatus &tDVidChnStatus, u8 byChnIdx = 0)
    {
        if ( byChnIdx < MAXNUM_MAU_DVCHN )
        {
            m_atDVidChnStatus[byChnIdx] = tDVidChnStatus;
        }
        return;
    }
	THDBasVidChnStatus * GetVidChnStatus(u8 byChnIdx = 0)
    {
        if ( byChnIdx < MAXNUM_MAU_VCHN )
        {
            return &m_atVidChnStatus[byChnIdx];
        }
        else
        {
            return NULL;
        }
    }
	THDBasDVidChnStatus * GetDVidChnStatus(u8 byChnIdx = 0)
    {
        if ( byChnIdx < MAXNUM_MAU_DVCHN )
        {
            return &m_atDVidChnStatus[byChnIdx];
        }
        else
        {
            return NULL;
        }
    }
    void Print() const
    {
        StaticLog( "TMauBasStatus is as follows:\n");
        u8 byIdx = 0;
        for(byIdx = 0; byIdx < MAXNUM_MAU_VCHN; byIdx++)
        {
            m_atVidChnStatus[byIdx].PrintInfo();
        }
        for(byIdx = 0; byIdx < MAXNUM_MAU_DVCHN; byIdx++)
        {
            m_atDVidChnStatus[byIdx].PrintInfo();
        }
    }
private:
    THDBasVidChnStatus m_atVidChnStatus[MAXNUM_MAU_VCHN];	
    THDBasDVidChnStatus m_atDVidChnStatus[MAXNUM_MAU_DVCHN];
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

/*----------------------------------------------------------------------
结构名：TMpuBasStatus
用途  ：MpuBas状态类，加进TPeriEqpStatus
----------------------------------------------------------------------*/
class TMpuBasStatus
{
public:
    void SetVidChnStatus (THDBasVidChnStatus &tVidChnStatus, u8 byChnIdx)
    {
        if ( byChnIdx < MAXNUM_MPU_CHN )
        {
            m_atVidChnStatus[byChnIdx] = tVidChnStatus;
        }
        return;
    }
	THDBasVidChnStatus * GetVidChnStatus(u8 byChnIdx)
    {
        if ( byChnIdx < MAXNUM_MPU_CHN )
        {
            return &m_atVidChnStatus[byChnIdx];
        }
		else
        {
            return NULL;
        }
    }
    void Print() const
    {
        StaticLog( "TMpuBasStatus is as follows:\n");
        u8 byIdx = 0;
        for(byIdx = 0; byIdx < MAXNUM_MPU_VCHN; byIdx++)
        {
			if(!m_atVidChnStatus[byIdx].IsNull())
			{
				m_atVidChnStatus[byIdx].PrintInfo();
			}           
        }
    }
private:
    THDBasVidChnStatus m_atVidChnStatus[MAXNUM_MPU_VCHN];
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

//[03/01/2010] zjl add 8kebasStatus
/*----------------------------------------------------------------------
结构名：T8keBasStatus
用途  ：T8keBasStatus状态类，加进TPeriEqpStatus  
----------------------------------------------------------------------*/
class T8keBasStatus
{
public:
    void SetVidChnStatus (THDBasVidChnStatus &tVidChnStatus, u8 byChnIdx)
    {
        if ( byChnIdx < MAXNUM_8KEBAS_VCHN )
        {
            m_atVidChnStatus[byChnIdx] = tVidChnStatus;
        }
        return;
    }
	THDBasVidChnStatus * GetVidChnStatus(u8 byChnIdx)
    {
        if ( byChnIdx < MAXNUM_8KEBAS_VCHN )
        {
            return &m_atVidChnStatus[byChnIdx];
        }
        else
        {
            return NULL;
        }
    }
    void Print() const
    {
        StaticLog( "T8keBasStatus is as follows:\n");
        u8 byIdx = 0;
        for(byIdx = 0; byIdx < MAXNUM_8KEBAS_VCHN; byIdx++)
        {
            m_atVidChnStatus[byIdx].PrintInfo();
        }
    }
private:
    THDBasVidChnStatus m_atVidChnStatus[MAXNUM_8KEBAS_VCHN];
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

//[03/31/2011] nzj add 8khbasStatus
/*----------------------------------------------------------------------
结构名：T8khBasStatus
用途  ：T8khBasStatus状态类，加进TPeriEqpStatus  
----------------------------------------------------------------------*/
class T8khBasStatus
{
public:
    void SetVidChnStatus (THDBasVidChnStatus &tVidChnStatus, u8 byChnIdx)
    {
        if ( byChnIdx < MAXNUM_8KHBAS_VCHN )
        {
            m_atVidChnStatus[byChnIdx] = tVidChnStatus;
        }
        return;
    }
	THDBasVidChnStatus * GetVidChnStatus(u8 byChnIdx)
    {
        if ( byChnIdx < MAXNUM_8KHBAS_VCHN )
        {
            return &m_atVidChnStatus[byChnIdx];
        }
        else
        {
            return NULL;
        }
    }
    void Print() const
    {
        OspPrintf(TRUE, FALSE, "T8khBasStatus is as follows:\n");
        u8 byIdx = 0;
        for(byIdx = 0; byIdx < MAXNUM_8KHBAS_VCHN; byIdx++)
        {
            m_atVidChnStatus[byIdx].PrintInfo();
        }
    }
private:
    THDBasVidChnStatus m_atVidChnStatus[MAXNUM_8KHBAS_VCHN];
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

class T8kiVidBasStatus
{
public:
    void SetVidChnStatus (THDBasVidChnStatus &tVidChnStatus, u8 byChnIdx)
    {
		//通道数待确认[7/30/2013 chendaiwei]
        if ( byChnIdx < 3 )
        {
            m_atVidChnStatus[byChnIdx] = tVidChnStatus;
        }
        return;
    }
	THDBasVidChnStatus * GetVidChnStatus(u8 byChnIdx)
    {
        if ( byChnIdx < 3 )
        {
            return &m_atVidChnStatus[byChnIdx];
        }
        else
        {
            return NULL;
        }
    }
    void Print() const
    {
        OspPrintf(TRUE, FALSE, "T8kiVidBasStatus is as follows:\n");
        u8 byIdx = 0;
        for(byIdx = 0; byIdx < 3; byIdx++)
        {
            m_atVidChnStatus[byIdx].PrintInfo();
        }
    }
private:
    THDBasVidChnStatus m_atVidChnStatus[3];
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;


/*----------------------------------------------------------------------
结构名：TMpu2BasStatus
用途  ：Mpu2Bas状态类，加进TPeriEqpStatus
----------------------------------------------------------------------*/
class TMpu2BasStatus
{
public:
    void SetVidChnStatus (THDBasVidChnStatus &tVidChnStatus, u8 byChnIdx)
    {
        if ( byChnIdx < MAXNUM_MPU2_EHANCED_CHN )
        {
            m_atVidChnStatus[byChnIdx] = tVidChnStatus;
        }
        return;
    }
	THDBasVidChnStatus * GetVidChnStatus(u8 byChnIdx)
    {
        if ( byChnIdx < MAXNUM_MPU2_EHANCED_CHN )
        {
            return &m_atVidChnStatus[byChnIdx];
        }
		else
        {
            return NULL;
        }
    }
    void Print() const
    {
        StaticLog( "TMpu2BasStatus is as follows:\n");
        u8 byIdx = 0;
        for(byIdx = 0; byIdx < MAXNUM_MPU2_EHANCED_CHN; byIdx++)
        {
			if(!m_atVidChnStatus[byIdx].IsNull())
			{
				m_atVidChnStatus[byIdx].PrintInfo();
			}           
        }
    }
private:
    THDBasVidChnStatus m_atVidChnStatus[MAXNUM_MPU2_EHANCED_CHN];
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;
/*----------------------------------------------------------------------
结构名：TMpuBasStatus
用途  ：MpuBas状态类，加进TPeriEqpStatus
----------------------------------------------------------------------*/
//TSpecialBasStatus该结构主要用来对MPU2模拟成MPU，连接老的MCU时使用
//因为老的union tStatus 最大为400字节(sizeof(tMpuBas));而新的由于每个通道编码路数增加，
//union tStatus最大字节达到了910(sizeof(tMpu2Bas) );导致老的MCU拿到的m_byType不对
//故通过TSpecialBasStatus来填充第401字节的m_byType，这样主控就能使用
struct TSpecialBasStatus
{
	u8  abyFillInData[400];
	u8	m_byType;
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

struct TAudBasStatus
{
public:
	union
	{
		u8 m_abyArray[910];//zjj20130922 为了和hdbas结构大小一致(要读下面的m_byType),所以增加这个结构
		TApu2BasStatus m_tApu2BasChnStatus;
		T8kiAudBasStatus m_t8KIAudBasStatus;
	}m_tAudBasStaus;

public:
    void SetEqpType(u8 byType)
    {
        if (byType != TYPE_APU2_BAS && byType != TYPE_8KI_AUD_BAS)
        {
            OspPrintf(TRUE, FALSE, "[TAudBasStatus][SetType] unexpected type.%d\n", m_byType);
            return;
        }
        m_byType = byType;
        return;
    }
    u8   GetEqpType(void) const { return m_byType; }
    
public:
    u8 m_byType;
}
#ifndef WIN32
__attribute__((packed))
#endif
;

class THdBasStatus
{
public:
    union
    {
        TMauBasStatus tMauBas;
        TMpuBasStatus tMpuBas;
		T8keBasStatus t8keBas;  //[03/01/2010] zjl add 8kebas
		T8khBasStatus t8khBas;	//[03/31/2011] nzj add 8khbas
		TMpu2BasStatus tMpu2Bas; 
		TSpecialBasStatus tSpeBas;
		T8kiVidBasStatus t8kiBas;
    }tStatus;

public:
    void SetEqpType(u8 byType)
    {
        if (byType != TYPE_MPU &&
            byType != TYPE_MAU_NORMAL &&
            byType != TYPE_MAU_H263PLUS &&
			byType != TYPE_8KE_BAS &&
			byType != TYPE_8KH_BAS &&
			byType != TYPE_MPU_H &&
			byType != TYPE_MPU2_BASIC &&
			byType != TYPE_MPU2_ENHANCED &&
			byType != TYPE_8KI_VID_BAS)
        {
            OspPrintf(TRUE, FALSE, "[THdBasStatus][SetType] unexpected type.%d\n", m_byType);
            return;
        }
        m_byType = byType;
        return;
    }
    u8   GetEqpType(void) const { return m_byType; }

public:
    u8 m_byType;
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;


//定义外设状态结构, size = 2435(TRecStatus = 2412)
struct T8KEPeriEqpStatus : public TEqp
{
public:
	u8  	m_byOnline;		//是否在线
	union UStatus
	{
		TTvWallStatus	tTvWall;
		T8KEMixerStatus	tMixer;
		TVmpStatus      tVmp;
		TRecStatus		tRecorder;
		//T8KEBasStatus		tBas;
		THdBasStatus	tHdBas;		//[03/01/2010] zjl 8000e bas外设状态向mcu靠拢
		TPrsStatus		tPrs;
	} m_tStatus;
	
protected:
	char    m_achAlias[MAXLEN_EQP_ALIAS];
public:
	//constructor
	T8KEPeriEqpStatus( void )
	{
		memset(this, 0, sizeof(T8KEPeriEqpStatus));
	}
	
	//获取外设别名
	const s8 * GetAlias() const 
	{ 
		return m_achAlias;
	}
	
	//设置外设别名
	void SetAlias(const s8 * lpszAlias)
	{ 
        if ( NULL == lpszAlias )
        {
            memset(m_achAlias, '\0', sizeof(m_achAlias) );
        }
        else
        {
            strncpy(m_achAlias, lpszAlias, sizeof(m_achAlias));
			m_achAlias[MAXLEN_EQP_ALIAS-1] = '\0';
        }
	}
	
    BOOL32 IsOnline() const
    {
        return ISTRUE(m_byOnline);
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//适配器通道状态结构
struct TBasChnStatus : TAdaptParam
{
    enum EBasStatus
    {
        IDLE   = 0,		//空闲
        READY  = 1,		//准备适配
        RUNING = 2,		//正在适配
		WAIT_START = 3  //等待开启
    };

protected:
    u8 m_byStatus;
	u8 m_byReserved;    //是否为保留通道
    u8 m_byChanType;    //音频通道: 1(BAS_CHAN_AUDIO)  视频通道: 2(BAS_CHAN_VIDEO)

public:

    void SetStatus(u8 byStatus)
    {
        m_byStatus = byStatus;
    }

    u8 GetStatus(void)
    {
        return m_byStatus;
    }

    void SetReserved(BOOL32 bReserved)
    {
        m_byReserved = (TRUE == bReserved) ? 1 : 0;
    }

    BOOL32 IsReserved(void)
    {
        return (0 != m_byReserved);
    }

    u8 GetChannelType(void)
    {
        return m_byChanType;
    }

    void SetChannelType(u8 byBasChanType)
    {
        m_byChanType = byBasChanType;
    }
}
#ifndef WIN32
__attribute__ ((packed))
#endif
;

//适配器启动的模块
struct TBasCapSet
{
public:
    u8 m_bySupportAudio;
    u8 m_bySupportVideo;
    u8 m_bySupportBitrate;

public:
    void SetAudioCapSet(BOOL32 bSupportAudio)
    {
        if (TRUE == bSupportAudio)
        {
            m_bySupportAudio = 1;
        }
        else
        {
            m_bySupportAudio = 0;
        }
    }

    BOOL32 IsSupportAudio(void)
    {
        if (0 == m_bySupportAudio)
        {
            return FALSE;
        }
        return TRUE;
    }

    void SetVideoCapSet(BOOL32 bSupportVideo)
    {
        if (TRUE == bSupportVideo)
        {
            m_bySupportVideo = 1;
        }
        else
        {
            m_bySupportVideo = 0;
        }
    }

    BOOL32 IsSupportVideo(void)
    {
        if (0 == m_bySupportVideo)
        {
            return FALSE;
        }
        return TRUE;
    }

    void SetBitrateCapSet(BOOL32 bSupportBitrate)
    {
        if (TRUE == bSupportBitrate)
        {
            m_bySupportBitrate = 1;
        }
        else
        {
            m_bySupportBitrate = 0;
        }
    }

    BOOL32 IsSupportBitrate(void)
    {
        if (0 == m_bySupportBitrate)
        {
            return FALSE;
        }
        return TRUE;
    }
}
#ifndef WIN32
__attribute__((packed))
#endif
;

//适配器通道状态结构
struct TBasStatus
{
	u8            byChnNum;
    TBasCapSet    tBasCapSet;
    TBasChnStatus tChnnl[MAXNUM_BAS_CHNNL];
}
#ifndef WIN32
__attribute__((packed))
#endif
;


//定义DCS状态结构
struct TPeriDcsStatus : public TEqp
{
public:
	u8	m_byOnline;			//是否在线
protected:
	s8	m_achDcsAlias[MAXLEN_EQP_ALIAS];
public:
	TPeriDcsStatus()
	{
		memset( this, 0, sizeof(TPeriDcsStatus) );
	}

	//获取DCS别名
	const s8 * GetDcsAlias() const
	{
		return m_achDcsAlias;
	}
	//设置外设别名
	void SetDcsAlias( const s8 * lpszAlias )
	{
		strncpy( m_achDcsAlias, lpszAlias, sizeof(m_achDcsAlias) );
		m_achDcsAlias[MAXLEN_EQP_ALIAS - 1] = '\0';
	}
	//置空
	void SetNull()
	{
		memset( this, 0, sizeof(TPeriDcsStatus) );
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//定义外设状态结构, size = 2435(TRecStatus = 2412)
struct TPeriEqpStatus : public TEqp
{
public:
	u8  	m_byOnline;		//是否在线
	union UStatus
	{
		TTvWallStatus	tTvWall;
		TMixerStatus	tMixer;
		TVmpStatus      tVmp;
		TRecStatus		tRecorder;
		TBasStatus		tBas;
		TPrsStatus		tPrs;
        THdBasStatus    tHdBas;
		THduStatus      tHdu;
		TAudBasStatus	tAudBas;
	} m_tStatus;
	
protected:
	char    m_achAlias[MAXLEN_EQP_ALIAS];
public:
	//constructor
	TPeriEqpStatus( void )
	{
		memset(this, 0, sizeof(TPeriEqpStatus));

		// [2013/03/11 chenbing]  
		// HDU通道0和通道1，默认为1风格
		m_tStatus.tHdu.SetChnCurVmpMode(0, HDUCHN_MODE_ONE);
		m_tStatus.tHdu.SetChnCurVmpMode(1, HDUCHN_MODE_ONE);
	}

	//获取外设别名
	const s8 * GetAlias() const 
	{ 
		return m_achAlias;
	}

	//设置外设别名
	void SetAlias(const s8 * lpszAlias)
	{ 
		strncpy(m_achAlias, lpszAlias, sizeof(m_achAlias));
		m_achAlias[MAXLEN_EQP_ALIAS-1] = '\0';
	}

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//定义要增加的终端信息
struct TAddMtInfo: public TMtAlias
{
	u8   m_byCallMode;
	u16  m_wDialBitRate;//（网络序）//单位Kbps
public:
	TAddMtInfo(){Init();}

	void Init()
	{
		SetCallMode(255);
		SetCallBitRate(0);
	}

	u16  GetCallBitRate() { return ntohs(m_wDialBitRate); }
	void SetCallBitRate(u16 wDialBitRate) { m_wDialBitRate = htons(wDialBitRate); }

    u8   GetCallMode() { return m_byCallMode; }
    void SetCallMode(u8 byCallMode) { m_byCallMode = byCallMode; }

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//调试版，增加终端时mcs指定的能力级信息
struct TAddMtCapInfo
{
private:
	TVideoStreamCap  m_tMainVideoCap;
	TVideoStreamCap  m_tDStreamVideoCap;
public:
	TAddMtCapInfo()	{ clear(); }
	void clear()
	{
		m_tMainVideoCap.Clear();
		m_tDStreamVideoCap.Clear();		
	}
	const TVideoStreamCap& GetMainVideoCap() const    { return m_tMainVideoCap; }
	void SetMainVideoCap(TVideoStreamCap& tVideoSteamCap) 
	{
		memcpy( &m_tMainVideoCap, &tVideoSteamCap, sizeof(TVideoStreamCap) );
	}

	const TVideoStreamCap& GetDStreamVideoCap() const { return m_tDStreamVideoCap; }
	void SetDStreamVideoCap(TVideoStreamCap& tVideoSteamCap) 
	{
		memcpy( &m_tDStreamVideoCap, &tVideoSteamCap, sizeof(TVideoStreamCap) );
	}
	
	// 主视频能力
    void	SetMainMaxBitRate(u16 wMaxRate)  {  m_tMainVideoCap.SetMaxBitRate(wMaxRate); }	
    u16		GetMainMaxBitRate() const { return m_tMainVideoCap.GetMaxBitRate(); }
	
    void	SetMainMediaType(u8 byMediaType)  {  m_tMainVideoCap.SetMediaType(byMediaType); }	
    u8		GetMainMediaType() const { return m_tMainVideoCap.GetMediaType(); }
    
    void    SetMainResolution(u8 byRes) {   m_tMainVideoCap.SetResolution(byRes);    }
    u8      GetMainResolution(void) const   {   return m_tMainVideoCap.GetResolution();    }

    void    SetMainFrameRate(u8 byFrameRate)
    {
        if ( MEDIA_TYPE_H264 == m_tMainVideoCap.GetMediaType() )
        {
            OspPrintf( TRUE, FALSE, "Function has been preciated, try SetUserDefFrameRate\n" );
            return;
        }
        m_tMainVideoCap.SetFrameRate(byFrameRate);
		return;
    }
    u8      GetMainFrameRate(void) const
    {
        if ( MEDIA_TYPE_H264 == m_tMainVideoCap.GetMediaType() )
        {
            OspPrintf( TRUE, FALSE, "Function has been preciated, try GetUserDefFrameRate\n" );
            return 0;
        }
        return m_tMainVideoCap.GetFrameRate();
    }

    // 自定义帧率
    void    SetUserDefMainFrameRate(u8 byActFrameRate) { m_tMainVideoCap.SetUserDefFrameRate(byActFrameRate); }
    BOOL32  IsMainFrameRateUserDefined() const {return m_tMainVideoCap.IsFrameRateUserDefined(); }
    u8      GetUserDefMainFrameRate(void) const { return m_tMainVideoCap.GetUserDefFrameRate(); }

	// 双流能力
    void	SetDstreamMaxBitRate(u16 wMaxRate)  {  m_tDStreamVideoCap.SetMaxBitRate(wMaxRate); }	
    u16		GetDstreamMaxBitRate() const { return m_tDStreamVideoCap.GetMaxBitRate(); }

    void	SetDstreamMediaType(u8 byMediaType)  {  m_tDStreamVideoCap.SetMediaType(byMediaType); }	
    u8		GetDstreamMediaType() const { return m_tDStreamVideoCap.GetMediaType(); }
    
    void    SetDstreamResolution(u8 byRes) {   m_tDStreamVideoCap.SetResolution(byRes);    }
    u8      GetDstreamResolution(void) const   {   return m_tDStreamVideoCap.GetResolution();    }

    void    SetDstreamFrameRate(u8 byFrameRate)
    {
        if ( MEDIA_TYPE_H264 == m_tDStreamVideoCap.GetMediaType() )
        {
            OspPrintf( TRUE, FALSE, "Function has been preciated, try SetUserDefFrameRate\n" );
            return;
        }
        m_tDStreamVideoCap.SetFrameRate(byFrameRate);
		return;
    }
    u8      GetDstreamFrameRate(void) const
    {
        if ( MEDIA_TYPE_H264 == m_tDStreamVideoCap.GetMediaType() )
        {
            OspPrintf( TRUE, FALSE, "Function has been preciated, try GetUserDefFrameRate\n" );
            return 0;
        }
        return m_tDStreamVideoCap.GetFrameRate();
    }

    // 自定义帧率
    void    SetUserDefDstreamFrameRate(u8 byActFrameRate) { m_tDStreamVideoCap.SetUserDefFrameRate(byActFrameRate); }
    BOOL32  IsDstreamFrameRateUserDefined() const {return m_tDStreamVideoCap.IsFrameRateUserDefined(); }
    u8      GetUserDefDstreamFrameRate(void) const { return m_tDStreamVideoCap.GetUserDefFrameRate(); }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//调试版增加终端的信息，包含MCS指定的部分终端能力级
struct TAddMtInfoEx: public TAddMtInfo
{
private:
	TAddMtCapInfo m_tMtCapInfo;

public:
	TAddMtInfoEx()	{ clear(); }
	void clear()
	{
		m_tMtCapInfo.clear();
	}

	const TAddMtCapInfo& GetMtCapInfo() const    { return m_tMtCapInfo; }
	void SetMtCapInfo(TAddMtCapInfo& tMtCapInfo) 
	{
		memcpy( &m_tMtCapInfo, &tMtCapInfo, sizeof(TAddMtCapInfo) );
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;



//定义会议终端结构，该结构定义了参加会议的MCU和两个终端列表（受邀终端列表和实际
//与会终端列表），后者必须是前者的子集 
struct TConfMtInfo
{
protected:   
	u16     m_wMcuIdx;          //该MC所在的Mcu的Id号
    u8      m_byConfIdx;        //会议索引号 
	u8  	m_abyMt[MAXNUM_CONF_MT/8];	        //受邀终端列表
	u8  	m_abyJoinedMt[MAXNUM_CONF_MT/8];	//与会终端列表

public:
    void  SetMcuIdx(const u16 wMcuIdx){ m_wMcuIdx = htons(wMcuIdx); } 
    u16    GetMcuIdx( void ) const { return ntohs(m_wMcuIdx); }
    void  SetConfIdx(const u8  byConfIdx){ m_byConfIdx = byConfIdx; } 
    u8    GetConfIdx( void ) const { return m_byConfIdx; }

	TConfMtInfo( void );//constructor
	BOOL MtInConf(const u8 byMtId ) const;//判断该MCU下某终端是否在受邀终端列表中
	BOOL MtJoinedConf(const u8 byMtId ) const;//判断该MCU下某终端是否在与会终端列表中
	void AddMt(const u8 byMtId );//增加该MCU下终端于会议终端列表中
	void AddJoinedMt(const u8 byMtId );//增加该MCU下终端于与会终端列表中，该函数自动将该终端也
	void RemoveMt(const u8 byMtId );//删除该MCU下终端于受邀终端列表中，该函数自动将该终端也
	void RemoveJoinedMt(const u8 byMtId );//删除该MCU下终端于与会终端列表中
	void RemoveAllMt( void );//删除该MCU下所有终端于受邀终端列表中，该函数自动将所有终端也
	void RemoveAllJoinedMt( void );//删除该MCU下所有终端于与会终端列表中
	u8   GetAllMtNum( void ) const;//得到一个结构中所有受邀终端个数
	u8   GetAllJoinedMtNum( void ) const;//得到一个结构中所有与会终端个数
	u8   GetAllUnjoinedMtNum( void ) const;//得到一个结构中所有未与会终端个数
	u8	 GetMaxMtIdInConf( void ) const;	//得到一个结构中受邀终端中MtId最大的值// xliang [12/24/2008] 

	void SetNull( void )
	{
		m_wMcuIdx = INVALID_MCUIDX;
		m_byConfIdx = 0;
		memset( &m_abyMt[0],0,sizeof(m_abyMt) );
		memset( &m_abyJoinedMt[0],0,sizeof(m_abyJoinedMt) );
	}
	
	BOOL IsNull( void ) const
	{
		if( m_wMcuIdx == INVALID_MCUIDX && m_byConfIdx == 0 )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	BOOL IsMtExists( void )
	{
		u8 byCount = sizeof(m_abyMt) / sizeof(m_abyMt[0]);
		for( u8 byIdx = 0;byIdx < byCount; ++byIdx )
		{
			if( m_abyMt[byIdx] != 0 )
			{
				return TRUE;
			}
		}
		return FALSE;
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*====================================================================
    函数名      ：IsValidMcuId
    功能        ：Mcu Id的合法性检测
    算法实现    ：[0, MAXNUM_CONFSUB_MCU) || 19200 范围内的Id都是合法Id, 否则不合法
    引用全局变量：
    输入参数说明：[in]wMcuIdx, Mcu Id索引值
    返回值说明  ：TRUE, 合法; FALSE, 不合法
	----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	10/12/30    4.6          刘旭          创建
====================================================================*/
inline BOOL32 IsValidMcuId(const u16 wMcuIdx) 
{
	if(wMcuIdx >= MAXNUM_CONFSUB_MCU && wMcuIdx != LOCAL_MCUIDX)
	{ 
		return FALSE; 
	}

	return TRUE;
}

/*====================================================================
    函数名      ：IsLocalMcuId
    功能        ：wMcuId是否是本地McuId
    算法实现    ：
    引用全局变量：
    输入参数说明：[in]wMcuId, MtId索引值
    返回值说明  ：TRUE, 合法; FALSE, 不合法
	----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	10/12/30    4.6          刘旭          创建
====================================================================*/
inline BOOL32 IsLocalMcuId(const u16 wMcuId) 
{
	return (LOCAL_MCUIDX == wMcuId);
}

/*====================================================================
    函数名      ：IsValidSubMcuId
    功能        ：Sub Mcu Id的合法性检测
    算法实现    ：[0, LOCAL_MCUID) || (LOCAL_MCUID, MAXNUM_CONFSUB_MCU)范围内的Id都是合法SUB mcuId, 否则不合法
    引用全局变量：
    输入参数说明：[in]wMcuIdx, Mcu Id索引值
    返回值说明  ：TRUE, 合法; FALSE, 不合法
	----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	10/12/30    4.6          刘旭          创建
====================================================================*/
inline BOOL32 IsValidSubMcuId(const u16 wMcuIdx) 
{
	BOOL32 bRet = IsValidMcuId(wMcuIdx) && !IsLocalMcuId(wMcuIdx);
	
	return bRet;
}

/*====================================================================
    函数名      ：IsValidMtId
    功能        ：Mt的MtId的合法性检测
    算法实现    ：(0, MAXNUM_CONF_MT)范围内的Id都是合法MtId, 否则不合法
    引用全局变量：
    输入参数说明：[in]wMtId, MtId索引值
    返回值说明  ：TRUE, 合法; FALSE, 不合法
	----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	10/12/30    4.6          刘旭          创建
====================================================================*/
inline BOOL32 IsValidMtId(const u8 byMtId) 
{
	// 终端的实际编号是[1， 192), 192有特殊用途
	if(!byMtId || byMtId >= MAXNUM_CONF_MT)
	{ 
		return FALSE; 
	}
	
	return TRUE;
}


//会议所有mcu信息 (len:1202)
struct TConfAllMcuInfo
{
private:
	u8			m_abyMcuInfo[MAXNUM_CONFSUB_MCU][MAX_SUBMCU_DEPTH];	
	u16			m_wSum;								// 级联mcu的总个数, 满足m_wSum <= MAXNUM_CONFSUB_MCU
													// 网络序, 不能直接使用
public:
    TConfAllMcuInfo( void ){ Clear(); }

	/** 
	/* @功    能 : 清空TConfAllMcuInfo中的信息
	/* @参数说明 : 
	/* @返 回 值 : 
	/* @修改日期 		@版    本            @作    者          @记    录
	/*                     1.0                周晶晶             创作
	*/
	void Clear() 
	{ 
		memset( this, 0, sizeof(TConfAllMcuInfo) ); 		
	}

	/*====================================================================
	函数名      ：IsMcuIdAdded
	功能        ：获得TConfAllMcuInfo中mcu的个数, 以便外界遍历TConfAllMcuInfo的mcu
	算法实现    ：
	引用全局变量：
	输入参数说明：
	返回值说明  ：返回mcu的数量, 包含本级
	----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	11/01/06    4.6         liuxu         创建
	====================================================================*/
	BOOL32 GetAllMcuNum() const { return (MAXNUM_CONFSUB_MCU); } 

	/*====================================================================
		函数名      ：IsMcuIdAdded
		功能        ：是否已经添加mcu信息
		算法实现    ：
		引用全局变量：
		输入参数说明：[in]pbyMcuId, 数组,
					  [in]byCount, pbyMcuId中值的个数, 其值范围为[0, MAX_SUBMCU_DEPTH]
		返回值说明  ：存在返回TRUE,不存在返回FALSE
	----------------------------------------------------------------------
		修改记录    ：
		日  期      版本        修改人        修改内容
		10/07/08    4.6         周晶晶         创建
	====================================================================*/
	BOOL32 IsMcuIdAdded( const u8* const pabyMcuId, const u8& byMcuIdCount );

	/** 
	/* @功    能 : 根据mcuid 添加一个mcu
	/* @参数说明 : [in]pabyMcuId, 数组,
				   [in]byMcuIdCount, pabyMcuId中值的个数, 其值范围为(0, MAX_SUBMCU_DEPTH]
				   [out]pwMcuIdx, 新mcu添加的index值
	/* @返 回 值 : 添加成功, 返回TRUE, 否则FALSE
	/* @修改日期 		@版    本            @作    者          @记    录
	/*                     1.0                周晶晶             创作
	/*  2010/12/15         1.1                刘  旭             修改
	*/
	BOOL32 AddMcu( const u8* const pabyMcuId, const u8& byMcuIdCount, u16* const pwMcuIdx);

	/** 
	/* @功    能 : 根据上级mcu的mcu id找到它下面所有的下级mcu的idx
	/* @参数说明 : [in]pbyMcuId, 数组,
				   [in]byCount, pbyMcuId中值的个数, 其值范围为(0, MAX_SUBMCU_DEPTH]
				   [out]pwMcuIdx, 找到的下级mcu的idx
				   [in]byMcuIdxCount, 输入的pwMcuIdx数组的长度
	/* @返 回 值 : 找到的下级mcu的数量, 没找到返回0
	/* @修改日期 		@版    本            @作    者          @记    录
	/*                     1.0                周晶晶             创作
	/*  2010/12/15         1.1                刘  旭             修改
	*/
	u8 GetMcuIdxByMcuId( const u8* pabyMcuId, 
						 const u8& byMcuIdCount, 
						 u16* const pawMcuIdx, 
						 const u8 byMcuIdxCount = MAXNUM_SUB_MCU);

	/** 
	/* @功    能 : 根据MCU index删除MCU
	/* @参数说明 : [in]pwMcuIdx, 找到的下级mcu的idx
	/* @返 回 值 : 删除成功, 返回TRUE, 否则FALSE
	/* @修改日期 		@版    本            @作    者          @记    录
	/*                     1.0                周晶晶             创作
	/*  2010/12/15         1.1                刘  旭             修改
	*/
	BOOL32 RemoveMcu(const u16& wMcuIdx );
	

	/** 
	/* @功    能 : 根据MCU id查找它的Index值
	/* @参数说明 : [in]pbyMcuId, 数组,
				   [in]byCount, pbyMcuId中值的个数, 其值范围为(0, MAX_SUBMCU_DEPTH]
				   [out]pwMcuIdx, 找到的下级mcu的idx
	/* @返 回 值 : 找到返回TRUE, 否则FALSE
	/* @修改日期 		@版    本            @作    者          @记    录
	/*                     1.0                周晶晶             创作
	/*  2010/12/15         1.1                刘  旭             修改
	*/
	BOOL32 GetIdxByMcuId( const u8* pabyMcuId, const u8& byMcuIdCount, u16* const pwMcuIdx );

	/** 
	/* @功    能 : 判断wMcuIdx表示的mcu是否是本mcu的直接下级mcu
	/* @参数说明 : [in]wMcuIdx, 找到的下级mcu的idx
	/* @返 回 值 : 是的,返回TRUE, 否则FALSE
	/* @修改日期 		@版    本            @作    者          @记    录
	/*                     1.0                周晶晶             创作
	/*  2010/12/15         1.1                刘  旭             修改
	*/
	BOOL32 IsSMcuByMcuIdx( const u16& wMcuIdx );

	/** 
	/* @功    能 : 根据mcu index找到它的mcu id信息
	/* @参数说明 : [in]wMcuIdx, 找到的下级mcu的idx
			       [out]pabyMcuId, 数组, 输出查找到的mcu id
				   [in]byMcuIdCount, pabyMcuId中值的个数, 其值范围为(0, MAX_SUBMCU_DEPTH]
	/* @返 回 值 : 找到返回TRUE, 否则FALSE
	/* @修改日期 		@版    本            @作    者          @记    录
	/*                     1.0                周晶晶             创作
	/*  2010/12/15         1.1                刘  旭             修改
	*/
	BOOL32 GetMcuIdByIdx( const u16 wMcuIdx, u8* const pabyMcuId, const u8 byMcuIdCount = MAX_SUBMCU_DEPTH );

	/** 
	/* @功    能 : 查找直接下级mcu的数量
	/* @参数说明 : 
	/* @返 回 值 : 返回实际的数量
	/* @修改日期 		@版    本            @作    者          @记    录
	/*                     1.0                周晶晶             创作
	/*  2010/12/15         1.1                付秀华             修改
	*/
	u8 GetSMcuNum( void );

	/** 
	/* @功    能 : 返回下级mcu的数量
	/* @参数说明 : 
	/* @返 回 值 : 返回实际的数量
	/* @修改日期 		@版    本            @作    者          @记    录
	/*  2010/12/15         1.1                付秀华             创作
	*/
	u16 GetMcuCount( void ) { return ntohs( m_wSum );}
	
protected:
	// CompareByMcuIds的比较结果定义
	enum  E_CapareResult
	{
		E_CMP_RESULT_UP      = 0,						// 前者为后者的直接上级mcu
		E_CMP_RESULT_EQUAL,								// 前者mcu就是后者
		E_CMP_RESULT_DOWN,								// 前者为后者的直接下级mcu
		E_CMP_RESULT_OTHER,								// 两者没有直接上下级关系
		E_CMP_RESULT_INVALID,	    					// 输入参数有误,非法比较

		E_CMP_RESULT_NUM								// 比较结果的数量, 无其它意义
	};

protected:
	/** 
	/* @功    能 : 设置当前级联中mcu的数量, 不包括本mcu
	/* @参数说明 : 
	/* @返 回 值 : 无
	/* @修改日期 		@版    本            @作    者          @记    录
	/*  2010/12/15         1.1                付秀华             创作
	*/
	void SetMcuCount( const u16& wCount )
	{
		// 最多只可能有600个下级mcu. 
		if (wCount > MAXNUM_CONFSUB_MCU)
		{
			OspPrintf(TRUE, FALSE, "SetMcuCount Error, wCount = %d is greater than MAXNUM_CONFSUB_MCU\n", wCount);
			return;
		}

		m_wSum = htons( wCount );
	}

	/** 
	/* @功    能 : 判断m_abyMcuInfo[wIndex]是否为空
	/* @参数说明 : wIndex, m_abyMcuInfo的个数索引, 范围 = [0, MAXNUM_CONFSUB_MCU)
	/* @返 回 值 : 不为空, 返回True; 否则False
	/* @创作日期 : 2010/12/15			@版    本 : 1.0                @作    者 : 刘旭
	*/
	BOOL32 IsNull(const u16& wIndex) const;

	/** 
	/* @功    能 : 将wIndex表示的mcu id序列值与pbyMcuId比较
	/* @参数说明 : [in]wIndex, m_abyMcuInfo的个数索引, 范围 = [0, MAXNUM_CONFSUB_MCU)
				   [in]pbyMcuId, 数组,
				   [in]byCount, pbyMcuId中值的个数, 其值范围为(0, MAX_SUBMCU_DEPTH]
	/* @返 回 值 : E_CMP_RESULT_UP, 索引值为wIndex的mcu为pbyMcuId表示的mcu的上级
				   E_CMP_RESULT_EQUAL，索引值为wIndex的mcu就是pbyMcuId表示的mcu
				   E_CMP_RESULT_DOWN，索引值为wIndex的mcu为pbyMcuId表示的mcu的下级
				   E_CMP_RESULT_OTHER, 索引值为wIndex的mcu与pbyMcuId表示的mcu没有直接上下级关系
				   E_CMP_RESULT_INVALID, 输入参数非法, 不能比较
	/* @创作日期 : 2010/12/15			@版    本 : 1.0                @作    者 : 刘旭
	*/
	E_CapareResult CompareByMcuIds(const u16& wIndex, const u8* const pabyMcuId, const u8& byMcuIdCount ) const;


	/** 
	/* @功    能 : 根据mcu id寻找对应的idx
	/* @参数说明 : [in]pabyMcuId, 数组,
				   [in]byMcuIdCount, pabyMcuId中值的个数, 其值范围为(0, MAX_SUBMCU_DEPTH]
				   [out]pwIdleIndex, 返回一个空闲的位置
	/* @返 回 值 : [0，MAXNUM_CONFSUB_MCU), 找到的idx值
				   [MAXNUM_CONFSUB_MCU, INVALID_MCUIDX], 没有找到
	/* @修改日期 		@版    本            @作    者          @记    录
	/* 2010/12/15         1.1                刘  旭                创作
	*/
	u16 FindMcuIdx(const u8* const pabyMcuId, const u8& byMcuIdCount, u16* const pwIdleIndex = NULL) const;


	/** 
	/* @功    能 : 根据mcu id获取它级联的深度
	/* @参数说明 : [in]pabyMcuId, 数组,
				   [in]byMcuIdCount, pabyMcuId中值的个数, 其值范围为(0, MAX_SUBMCU_DEPTH]
	/* @返 回 值 : <= 0， 非法值
				   (0，byMcuIdCount], 找到的合法的深度值
				   (byMcuIdCount, 无穷大], 非法值
	/* @修改日期 		@版    本            @作    者          @记    录
	/* 2010/12/15         1.1                刘  旭                创作
	*/
	const s8 GetDepth(const u8* const pabyMcuId, const u8& byMcuIdCount) const;

	// 检验wMcuId是否合法, 不合法直接返回FALSE
#define ISVALIDMCUID(wMcuIdx) \
	if(!IsValidMcuId(wMcuIdx)) \
	    return FALSE;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//会议所有终端信息 (len:3256)
struct TConfAllMtInfo
{
public:
	TConfMtInfo	m_tLocalMtInfo;
	
	TMt         m_tMMCU; //级联的上级MCU 

private:
	TConfMtInfo	m_atOtherMcMtInfo[MAXNUM_CONFSUB_MCU];

public:
    TConfAllMtInfo( void ){ Clear(); }

	void Clear() { 
		memset( this, 0, sizeof(TConfAllMtInfo) ); 
		m_tLocalMtInfo.SetNull();
		
		u16 wLoop = 0;
		while ( wLoop < MAXNUM_CONFSUB_MCU) {
			m_atOtherMcMtInfo[wLoop].SetNull();
			++wLoop;
		}
	}

	const u16 GetMaxMcuNum() const { return MAXNUM_CONFSUB_MCU; }	// liuxu, 返回最大的mcu数量,
																// 即m_atOtherMcMtInfo的容量

    void   SetLocalMtInfo(TConfMtInfo tLocalMtInfo){ m_tLocalMtInfo = tLocalMtInfo; } 
    TConfMtInfo  GetLocalMtInfo( void ) const { return m_tLocalMtInfo; }

	BOOL AddMt(const u8 byMtId);//会议中增加一个受邀终端  
	BOOL AddJoinedMt(const u8 byMtId);//会议中增加一个与会终端，该函数自动将该终端也
	BOOL AddJoinedMt(const TMt& tMt);//会议中增加一个与会终端，该函数自动将该终端也
	
	void RemoveMt(const u8 byMtId );//会议中删除一个终端 
	void RemoveJoinedMt(const u8 byMtId);//会议中删除一个与会终端
	void RemoveJoinedMt( const TMt&  tMt );//会议中删除一个与会终端

	void RemoveAllJoinedMt( void );//会议中删除一个MCU下所有与会终端（不删除MCU）

	BOOL MtInConf( const u8 byMtId ) const;//判断终端是否在受邀终端列表中
	BOOL MtInConf( const u16 wMcuIdx, const u8 byMtId ) const;//判断终端是否在受邀终端列表中

	// [12/30/2010 liuxu][走读] MtJoinedConf的接口有点多，只需要public一个就够了，最多两个
	BOOL MtJoinedConf( const u16  wMcuIdx, const u8  byMtId ) const;//判断终端是否在与会终端列表中
	BOOL MtJoinedConf( const TMt& tMt) const;	//可反映级联mcu下的mt是否与会 // [1/18/2010 xliang] 
	BOOL MtJoinedConf( const u8  byMtId ) const;//判断终端是否在与会终端列表中
	
	u8   GetLocalMtNum( void ) const;//得到本地所有受邀终端个数 
	u8   GetLocalJoinedMtNum( void ) const;//得到本地所有与会终端个数
	u8   GetLocalUnjoinedMtNum( void ) const;//得到本地所有未与会终端个数

    void   SetMtInfo(const TConfMtInfo& tConfMtInfo );
    TConfMtInfo  GetMtInfo( const u16 wMcuIdx ) const;
	TConfMtInfo* GetMtInfoPtr(const u16 byMcuIdx);

	u16   GetAllMtNum( void ) const;//得到会议完整结构中所有受邀终端个数 
	u32   GetAllJoinedMtNum( void ) const;//得到会议完整结构中所有与会终端个数
	u8	GetMaxMtIdInConf( void )const;// xliang [12/24/2008]

	u16	GetCascadeMcuNum( void) const;

	BOOL AddMcuInfo( const u16 wMcuIdx, const u8 m_byConfIdx );   //会议中增加一个mcu信息，可能暂时没有终端
	BOOL RemoveMcuInfo( const u16 wMcuIdx, const u8 m_byConfIdx );//会议中移除一个mcu信息，同时清空所有所属终端信息

	void Print()
	{
		StaticLog("Local Mt online.%d And MtId is: \n", m_tLocalMtInfo.GetAllJoinedMtNum());
		for (u16 wMcuIdx = 0;wMcuIdx < MAXNUM_CONFSUB_MCU;wMcuIdx++)
		{
			if ( !m_atOtherMcMtInfo[wMcuIdx].IsNull())
			{
				StaticLog("SMcu Mt online.%d\n", m_atOtherMcMtInfo[wMcuIdx].GetAllJoinedMtNum());
			}
		}
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


//定义电视墙设置结构，每个结构反映了一组终端号或MCU外设号和视频输出端口号的对应关系
struct TTVWallSetting
{
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//定义逻辑通道结构(len:47)
struct TLogicalChannel
{  
public:
	u8  m_byMediaType;      //媒体类型：MODE_VIDEO或MODE_AUDIO
	u8  m_byChannelType;    //信道类型：AUDIO_G711A、VIDEO_H261等
	u8  m_byVideoFormat;    //视频信道的分辩率
	u8  m_byH239DStream;    //信道类型是否是H239载荷
	u16 m_wFlowControl;     //信道流量(单位:Kbps,1K=1024)
    u16 m_wCurrFlowControl; //当前信道流量值
	TTransportAddr m_tRcvMediaChannel;     //接收端媒体信道
	TTransportAddr m_tRcvMediaCtrlChannel; //接收端媒体控制信道
	TTransportAddr m_tSndMediaCtrlChannel; //发送端媒体控制信道

	TMediaEncrypt  m_tMediaEncrypt;   //加密信息
	u8             m_byActivePayload; //动态载荷
	u8			   m_byFECType;		  //MCU前向纠错类型, zgc, 2007-10-10
    u8             m_byVidFPS;        //本通道码流的帧率 MODE_VIDEO下
private:
	//emProfileAttrb m_emHPAttrb;	  //[12/9/2011 chendaiwei]HP/BP属性
	u8             m_byExInfo;        //低一位标示视频HP/BP属性，剩余7位标示音频声道数
public:
	TLogicalChannel(void)
	{ 
		Clear();
	}
    void Clear(void)
    {
        memset( this, 0, sizeof(TLogicalChannel));
		m_tMediaEncrypt.Reset();
		m_byChannelType = MEDIA_TYPE_NULL;
        m_wCurrFlowControl = 0xFFFF;
		m_byExInfo = 0;
    }
    void SetMediaType(u8 byMediaType){ m_byMediaType = byMediaType;} 
    u8   GetMediaType( void ) const { return m_byMediaType; }
    void SetChannelType(u8 byChannelType){ m_byChannelType = byChannelType;} 
    u8   GetChannelType( void ) const { return m_byChannelType; }
    void SetSupportH239(BOOL32 bH239DStream){ m_byH239DStream = (bH239DStream ? 1:0);} 
    BOOL32   IsSupportH239( void ) const { return (m_byH239DStream == 0 ? FALSE : TRUE); }
    void SetVideoFormat(u8 byVideoFormat){ m_byVideoFormat = byVideoFormat;} 
    u8   GetVideoFormat( void ) const { return m_byVideoFormat; }
    void SetFlowControl(u16  wFlowControl){ m_wFlowControl = htons(wFlowControl);} 
    u16  GetFlowControl( void ) const { return ntohs(m_wFlowControl); }
    void SetCurrFlowControl(u16  wFlowControl){ m_wCurrFlowControl = htons(wFlowControl);} 
    u16  GetCurrFlowControl( void ) const { return ntohs(m_wCurrFlowControl); }
    void SetRcvMediaChannel(TTransportAddr tRcvMediaChannel){ m_tRcvMediaChannel = tRcvMediaChannel;} 
    TTransportAddr  GetRcvMediaChannel( void ) const { return m_tRcvMediaChannel; }
    void SetRcvMediaCtrlChannel(TTransportAddr tRcvMediaCtrlChannel){ m_tRcvMediaCtrlChannel = tRcvMediaCtrlChannel;} 
    TTransportAddr  GetRcvMediaCtrlChannel( void ) const { return m_tRcvMediaCtrlChannel; }
    void SetSndMediaCtrlChannel(TTransportAddr tSndMediaCtrlChannel){ m_tSndMediaCtrlChannel = tSndMediaCtrlChannel;} 
    TTransportAddr  GetSndMediaCtrlChannel( void ) const { return m_tSndMediaCtrlChannel; }
    BOOL IsOpened( void ) const { return m_byMediaType == 0 ? FALSE : TRUE; }

	
	void SetRcvMediaChnnelIp(u32 dwIp) 
	{
		m_tRcvMediaChannel.SetIpAddr(dwIp);
		m_tRcvMediaCtrlChannel.SetIpAddr(dwIp);
	}

	void SetMediaEncrypt(TMediaEncrypt& tMediaEncrypt)
	{
		m_tMediaEncrypt = tMediaEncrypt;
	}
	TMediaEncrypt &GetMediaEncrypt()
	{
		return m_tMediaEncrypt;
	}

	void SetActivePayload(u8 byActivePayload)
	{
		m_byActivePayload = byActivePayload;
	}
	u8 GetActivePayload()
	{
		return m_byActivePayload;
	}

	void SetFECType( u8 byFECType ) { m_byFECType = byFECType; }
	u8	 GetFECType( void ) const { return m_byFECType; }
    void SetChanVidFPS( u8 byFPS ) { m_byVidFPS = byFPS;    }
    u8   GetChanVidFPS( void ) const { return m_byVidFPS;    }

	// 设置/获取HP/BP属性接口 [12/9/2011 chendaiwei]
	void SetProfieAttrb(emProfileAttrb emProf)
	{
		if (emProf == emHpAttrb)
		{
			m_byExInfo |= 0x01;
		}
		else
		{
			m_byExInfo &= 0xFE;
		}
	}
	emProfileAttrb GetProfileAttrb( void )
	{
		return (emProfileAttrb)(m_byExInfo & 0x01);
	}
	void SetAudioTrackNum(u8 byAudioTrackNum)
	{
		m_byExInfo = ((m_byExInfo & 0x01) | (byAudioTrackNum << 1));
	}
	u8 GetAudioTrackNum( void ){ return ((m_byExInfo & 0xFE) >> 1); }

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//H239双流令牌拥有终端信息 (len:15)
typedef struct tagH239TokenInfo
{
    s32  m_nChannelId;   
    
    u8   m_bySymmetryBreaking;
	BOOL m_bIsResponseAck;

	//H239双流令牌拥有终端
	TMt  m_tH239TokenMt;   
    
    tagH239TokenInfo()
    {
        Clear();
    }

    void Clear()
    {
        m_nChannelId         = 0;
        m_bySymmetryBreaking = 0;
		m_bIsResponseAck     = FALSE;
        m_tH239TokenMt.SetNull();
    }

    void SetSymmetryBreaking(u8 bySymmetryBreaking)
    {
        m_bySymmetryBreaking = bySymmetryBreaking;
    }
    u8 GetSymmetryBreaking()
    {
        return m_bySymmetryBreaking;
    }

	void SetResponseAck(BOOL bIsResponseAck)
    {
        m_bIsResponseAck = bIsResponseAck;
    }
    BOOL IsResponseAck()
    {
        return m_bIsResponseAck;
    }

    void SetChannelId(s32 nChannelId)
    {
        m_nChannelId = nChannelId;
    }   
    s32 GetChannelId()
    {
        return m_nChannelId;
    }

	void SetTokenMt(TMt tH239TokenMt)
    {
        m_tH239TokenMt = tH239TokenMt;
    }   
    TMt GetTokenMt()
    {
        return m_tH239TokenMt;
    }
    
}TH239TokenInfo,*PTH239TokenInfo;
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif


//定义交换结构
struct TSwitchInfo
{
protected:
	TMt	  m_tDstMt;		  //交换目的终端
	TMt   m_tSrcMt;		  //交换源终端
	u8    m_byMode;		  //交换模式：MODE_VIDEO, MODE_AUDIO, MODE_BOTH
	u8    m_byDstChlIdx;  //目的通道索引
	u8    m_bySrcChlIdx;  //源通道索引号
public:
    void  SetDstMt(TMt tDstMt){ m_tDstMt = tDstMt;} 
    TMt   GetDstMt( void ) const { return m_tDstMt; }
    void  SetSrcMt(TMt tSrcMt){ m_tSrcMt = tSrcMt;} 
    TMt   GetSrcMt( void ) const { return m_tSrcMt; }
    void  SetMode(u8   byMode){ m_byMode = byMode;} 
    u8    GetMode( void ) const { return m_byMode; }
    void  SetDstChlIdx(u8   byDstChlIdx){ m_byDstChlIdx = byDstChlIdx;} 
    u8    GetDstChlIdx( void ) const { return m_byDstChlIdx; }
    void  SetSrcChlIdx(u8   bySrcChlIdx){ m_bySrcChlIdx = bySrcChlIdx;} 
    u8    GetSrcChlIdx( void ) const { return m_bySrcChlIdx; }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//定义电视墙交换结构
struct TTWSwitchInfo : public TSwitchInfo
{
protected:
	u8    m_bySrcMtType;  //电视墙源通道成员类型,参见mcuconst.h中电视墙成员类型定义
	u8    m_bySchemeIdx;  // 预案索引号
	u8    m_byDstSubChn;
	u8    m_byReserved[2];
public:
    TTWSwitchInfo()
	{
		memset( this, 0x0, sizeof(TTWSwitchInfo) );
	}

	void  SetMemberType(u8   bySrcMtType ){ m_bySrcMtType = bySrcMtType; } 
    u8    GetMemberType( void ) const { return m_bySrcMtType; }

	void  SetSchemeIdx(u8   bySchemeIdx ){ m_bySchemeIdx = bySchemeIdx; } 
    u8    GetSchemeIdx( void ) const { return m_bySchemeIdx; }
	void  SetDstSubChn(u8  bySubChn){ m_byDstSubChn = bySubChn;}
	u8    GetDstSubChn( void ) const { return m_byDstSubChn; }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//定义HDU交换结构
typedef TTWSwitchInfo THduSwitchInfo;   //4.6.1 新加  jlb

//设置hdu音量结构体  范围 0 -- 31
struct THduVolumeInfo:public TEqp
{
protected:
	u8  m_byChnlIdx;    // 通道索引号
	u8  m_byVolume;     // 音量大小
	u8  m_byIsMute;  // 是否静音  
	u8  m_byReserved[5];

public:
    THduVolumeInfo()
	{
        memset(this, 0x0, sizeof(THduVolumeInfo));
		m_byVolume = HDU_VOLUME_DEFAULT;
	}

	u8   GetChnlIdx( void ){ return m_byChnlIdx; }
	void SetChnlIdx( u8 byChnlIdx ){ m_byChnlIdx = byChnlIdx; }

	u8   GetVolume( void ){ return m_byVolume; }
	void SetVolume( u8 byVolume ){ m_byVolume = byVolume; }

	BOOL32   GetIsMute( void ){ return m_byIsMute; }
	void SetIsMute( BOOL32 byIsMute ){ m_byIsMute = GETBBYTE(byIsMute); }

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//定义最大不对称结构
struct TMaxSkew
{
	u8    byChan1;	//信道1：MODE_VIDEO, MODE_AUDIO
	u8    byChan2;	//信道2：MODE_VIDEO, MODE_AUDIO
protected:
	u16 	wMaxSkew;	//最大不对称值：信道2比信道1的延迟处理量，单位（ms）

public:
	u16  GetMaxSkew( void ) const;//得到最大不对称值
	void SetMaxSkew( u16  wNewMaxSkew );//设置最大不对称值
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//8000E外设注册请求消息类
struct TEqpRegReq : public TEqp
{
protected:
    u32   		m_dwIpAddr;		//外设IP地址
    s8          m_achAlias[MAXLEN_EQP_ALIAS];
    u16         m_wVersion;     //版本信息
    u32         m_dwReserve1;   //保留字段1
    u32         m_dwReserve2;   //保留字段2 
    
public:
    TEqpRegReq(void) { memset(this, 0, sizeof(TEqpRegReq)); }
    //设置外设IP地址
    void SetEqpIpAddr( const u32 &dwIpAddr ) { m_dwIpAddr = htonl(dwIpAddr); }
    //获得外设IP地址
    u32  GetEqpIpAddr(void) const { return ntohl(m_dwIpAddr); }
    
    void SetEqpAlias(const char * szAlias)
    {
        if ( NULL != szAlias )
        {
            strncpy( m_achAlias, szAlias ,sizeof(m_achAlias) );
            m_achAlias[MAXLEN_EQP_ALIAS-1] ='\0';
        }
        else
        {
            memset( m_achAlias, '\0', sizeof(m_achAlias) );
        }
        return;
    }
    
    s8 * GetEqpAlias(void) const
    {
        return (s8*)m_achAlias;
    }
    
    void SetVersion(const u16 &wVersion)
    {
        m_wVersion = htons(wVersion);
    }
    u16 GetVersion(void) const
    {
        return ntohs(m_wVersion);
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//8000E定义外设登记成功信息结构
struct TEqpRegAck
{
protected:
    u32 m_dwMcuIpAddr;	    //MCU接收地址
    u16 m_wMcuStartPort;	//MCU接收起始端口号
    u16 m_wEqpStartPort;    //EQP接收起始端口号
    u8  m_byEqpId;          //外设ID
#if defined(_8KH_)
    u8  m_byMemNum;          //8KH混音器外设通道数目
    u8  m_byReserver1;       //保留字段1
    u16 m_wReserver2;        //保留字段2
    u32 m_dwReserver3;       //保留字段3
#else
    u32 m_dwReserver1;       //保留字段1
    u32 m_dwReserver2;        //保留字段2
#endif
	
public:
    //得到上级MCU交换接收数据IP地址
    u32  GetMcuIpAddr( void ) const { return ntohl(m_dwMcuIpAddr); }   
    //设置上级MCU交换接收数据IP地址
    void SetMcuIpAddr( const u32 &dwIpAddr ) { m_dwMcuIpAddr = htonl(dwIpAddr); }
    
    //得到MCU交换接收数据端口号
    u16  GetMcuStartPort( void ) const { return ntohs(m_wMcuStartPort); }   
    //设置MCU交换接收数据端口号
    void SetMcuStartPort( const u16 &wPort ) { m_wMcuStartPort = htons(wPort); }
    
    //得到外设交换接收数据端口号
    u16  GetEqpStartPort( void ) const { return ntohs(m_wEqpStartPort); }   
    //设置外设交换接收数据端口号
    void SetEqpStartPort( const u16 &wPort ) { m_wEqpStartPort = htons(wPort); }
    
    //得到外设ID
    u8   GetEqpId( void ) const { return m_byEqpId; }
    //设置外设ID
    void SetEqpId( const u8 &byEqpId ) { m_byEqpId = byEqpId; }
#if defined(_8KH_)
    //得到该混音器配置的路数
    u8   GetMemNum( void ) const { return m_byMemNum; }
    //设置该混音器配置的路数
    void SetMemNum( const u8 &byMemNum ) { m_byMemNum = byMemNum; }
#endif
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


//外设注册请求消息类
struct TPeriEqpRegReq : public TEqp
{
protected:
	u32   		m_dwIpAddr;		//外设IP地址
	u16 		m_wStartPort;	//外设起始接收端口
	u8  		m_byChannlNum;	//外设接收信道数
	s8          m_achAlias[MAXLEN_EQP_ALIAS];
    //4.0R3扩展字段
    u16         m_wVersion;     //版本信息
    //4.5高清扩展字段
    BOOL        m_bHDEqp;       //是否高清外设

public:
    TPeriEqpRegReq(void) { memset(this, 0, sizeof(TPeriEqpRegReq)); }
    //设置外设IP地址
    //输入：IP地址(网络序)
	void SetPeriEqpIpAddr( const u32    &dwIpAddr )	{ m_dwIpAddr = dwIpAddr; }
    //获得外设IP地址
    //返回值：IP地址(网络序)
	u32    GetPeriEqpIpAddr() const	{ return m_dwIpAddr; }
	//设置外设接收起始端口
	void SetStartPort( const u16  & wStartPort )	{ m_wStartPort = htons(wStartPort); }
	//获得外设接收起始端口
	u16  GetStartPort() const	{ return ntohs( m_wStartPort ); }
	//设置外设接收信道数目
	void SetChnnlNum( const u8   & byChannlNum )	{ m_byChannlNum = byChannlNum; }
	//获得外设接收信道数目
	u8   GetChnnlNum() const	{ return m_byChannlNum; }

	void SetEqpAlias(char* szAlias)
	{
		strncpy( m_achAlias ,szAlias ,sizeof(m_achAlias) );
		m_achAlias[MAXLEN_EQP_ALIAS-1] ='\0';
	}

	LPCSTR GetEqpAlias()
	{
		return m_achAlias;
	}

    void SetVersion(u16 wVersion)
    {
        m_wVersion = htons(wVersion);
    }
    u16 GetVersion() const
    {
        return ntohs(m_wVersion);
    }
    void SetHDEqp(BOOL bHD)
    {
        m_bHDEqp = bHD;
    }

    BOOL IsHDEqp() const
    {
        return m_bHDEqp;
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//定义外设登记成功信息结构
struct TPeriEqpRegAck
{
protected:
	u32   		m_dwIpAddr;			//MCU接收地址
	u16 		m_wStartPort;		//MCU接收起始端口号
    u32         m_dwAnotherMpcIp;   //主备时：另外一块mpc板地址（用来对外设配置的备板mpc地址进行校验）
	u32         m_dwMsSSrc;         //主备时：用于标注一次系统运行，只要该值变化就说明发生过主备同时重启

public:
	//得到上级MCU交换接收数据IP地址
    u32    GetMcuIpAddr( void ) const { return ntohl(m_dwIpAddr); }

	//设置上级MCU交换接收数据IP地址
    void SetMcuIpAddr( u32    dwIpAddr ) { m_dwIpAddr = htonl(dwIpAddr); }

	//得到上级MCU交换接收数据端口号
    u16  GetMcuStartPort( void ) const { return ntohs(m_wStartPort); }

    //设置上级MCU交换接收数据端口号
    void SetMcuStartPort( u16  wPort ) { m_wStartPort = htons(wPort); }

    //另外一块mpc板地址
    u32  GetAnotherMpcIp( void ) const { return ntohl(m_dwAnotherMpcIp); }
    void SetAnotherMpcIp( u32 dwIpAddr ) { m_dwAnotherMpcIp = htonl(dwIpAddr); }

	void SetMSSsrc(u32 dwSSrc)
    {
        m_dwMsSSrc = htonl(dwSSrc);
    }
    u32 GetMSSsrc(void) const
    {
        return ntohl(m_dwMsSSrc);
    }

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//会议控制台注册信息(len:77)
struct TMcsRegInfo
{
public:
	u32   		m_dwMcsIpAddr;	      //会议控制台的Ip地址（网络序）
	u8  		m_byVideoChnNum;	  //通道数目
	u16 		m_wStartPort;		  //视频通道的起始端口号（网络序）
	s8          m_achUser[MAXLEN_PWD];//用户名
	s8          m_achPwd[MAXLEN_PWD]; //用户密码
	u32   		m_dwMcsSSRC;	      //会议控制台的连接随机值，（网络序）
	                                  //应保障同一mcs连接同一mcu的主备mpc采用同一值，
	                                  //      不同mcs连接同一mcu采用不同值
    u16         m_wMcuMcsVer;         //mcu与mcs会话版本
public:
	TMcsRegInfo( void )
    { 
        memset( this, 0, sizeof(TMcsRegInfo) );
        SetMcuMcsVer();
    }
    void  SetMcsIpAddr(u32 dwMcsIpAddr){ m_dwMcsIpAddr = htonl(dwMcsIpAddr);}
    u32   GetMcsIpAddr( void ) const { return ntohl(m_dwMcsIpAddr); }
    void  SetVideoChnNum(u8 byVideoChnNum){ m_byVideoChnNum = byVideoChnNum;}
    u8    GetVideoChnNum( void ) const { return m_byVideoChnNum; }
    void  SetStartPort(u16 wStartPort){ m_wStartPort = htons(wStartPort);}
    u16   GetStartPort( void ) const { return ntohs(m_wStartPort); }
    void  SetMcsSSRC(u32 dwMcsSSRC){ m_dwMcsSSRC = htonl(dwMcsSSRC);} 
    u32   GetMcsSSRC( void ) const { return ntohl(m_dwMcsSSRC); }
    void  SetMcuMcsVer(void) { m_wMcuMcsVer = htons(MCUMCS_VER); }
    u16   GetMcuMcsVer(void) const { return ntohs(m_wMcuMcsVer); }
    void  SetUser( LPCSTR lpszUser )
	{
		memset(m_achUser, 0, sizeof(m_achUser));
		if (NULL != lpszUser)
		{
			strncpy(m_achUser, lpszUser, sizeof(m_achUser));
			m_achUser[sizeof(m_achUser)-1] = '\0';
		}
	}
	LPCSTR GetUser( void ) const{ return m_achUser; }
	BOOL IsEqualUser( LPCSTR lpszUser ) const
	{
		s8 achBuf[MAXLEN_PWD+1] = {0};
		strncpy( achBuf, lpszUser, sizeof( achBuf ) );
		achBuf[sizeof(achBuf)-1] = '\0';
		
		if(0 == strncmp(achBuf, lpszUser, MAXLEN_PWD))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	void SetPwd( LPCSTR lpszPwd )
	{
		memset(m_achPwd, 0, sizeof(m_achPwd));
		if (NULL != lpszPwd)
		{
			strncpy(m_achPwd, lpszPwd, sizeof(m_achPwd));
			m_achPwd[sizeof(m_achPwd)-1] = '\0';
            // guzh [10/11/2007] 简单加密。在GetPwd之前要手动解密
            EncPwd();
		}
	}

	LPCSTR GetPwd( void ) const{ return m_achPwd; }
    
    // guzh [10/11/2007] 
    /*
	BOOL IsEqualPwd( LPCSTR lpszPwd ) const
	{
		s8 achBuf[MAXLEN_PWD+1] = {0};
		strncpy( achBuf, lpszPwd, sizeof( achBuf ) );
		achBuf[sizeof(achBuf)-1] = '\0';
		
		if(0 == strncmp(achBuf, lpszPwd, MAXLEN_PWD))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
    */
	
	BOOL operator ==( TMcsRegInfo &tObj ) const
	{
		if (tObj.m_dwMcsIpAddr == m_dwMcsIpAddr && 
			tObj.m_dwMcsSSRC == m_dwMcsSSRC && 
			tObj.m_byVideoChnNum == m_byVideoChnNum && 
			IsEqualUser(tObj.m_achUser) && 
			/*IsEqualPwd(tObj.m_achPwd)&&*/ 
            tObj.m_wMcuMcsVer == m_wMcuMcsVer)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
        
    void DecPwd()
    {
        EncPwd();
    }
private:
    void EncPwd()
    {
        for (u8 byLoop=0; byLoop<strlen(m_achPwd); byLoop++)
        {
            m_achPwd[byLoop] ^= 0xa7;
        }
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//会议控制台注册回应(len:157)
struct TMcsRegRsp
{
protected:
    TMcu                m_tMcu;             // MCU 结构
    TMtAlias            m_tMcuAlias;        // MCU 别名
    u32                 m_dwRemoteMpcIp;    // 主备时对端的IP
    u8                  m_byLocalActive;    // 当前本MPC是否为主用板
    u8                  m_byUsrGrpId;       // 登陆用户的组ID
    u8                  m_byMcuOsType;      // MCU的操作系统类型
	u32					m_dwSysSSrc;		// 主备标识
public:
    void   SetMcu(u8 byMcuType, u8 byMcuId)
    {
        m_tMcu.SetMcu(byMcuId);
        m_tMcu.SetMcuType(byMcuType);
    }
    TMcu   GetMcu() const
    {
        return m_tMcu;
    }
    void   SetMcuAlias(const TMtAlias &tAlias)
    {
        m_tMcuAlias = tAlias;
    }
    TMtAlias GetMcuAlias() const
    {
        return m_tMcuAlias;
    }
    void   SetRemoteMpcIp(u32 dwIp)
    {
        m_dwRemoteMpcIp = htonl(dwIp);
    }
    u32    GetRemoteMpcIp() const
    {
        return ntohl(m_dwRemoteMpcIp);
    }
    void   SetLocalActive(BOOL32 bActive)
    {
        m_byLocalActive = GETBBYTE(bActive);
    }
    BOOL32 IsLocalActive() const
    {
        return ISTRUE(m_byLocalActive);
    }
    void   SetUsrGrpId(u8 byGrpId)
    {
        m_byUsrGrpId = byGrpId;
    }
    u8     GetUsrGrpId() const
    {
        return m_byUsrGrpId;
    }
    void   SetMcuOsType(u8 byOsType)
    {
        m_byMcuOsType = byOsType;
    }
    u8     GetMcuOsType() const
    {
        return m_byMcuOsType;
    }
	void   SetSysSSrc(u32 dwSysSSrc)
	{
		m_dwSysSSrc = htonl(dwSysSSrc);
	}
	u32    GetSysSSrc() const
	{
		return ntohl(m_dwSysSSrc);
	}

    void Print() const
    {
        StaticLog( "Mcu: (%d, %d)\n", m_tMcu.GetMcuId(), m_tMcu.GetMcuType());
        StaticLog( "Alias: %s, 0x%x\n", m_tMcuAlias.m_achAlias, m_tMcuAlias.m_tTransportAddr.GetIpAddr());
        StaticLog( "Remote MpcIP: 0x%x\n", GetRemoteMpcIp());
        StaticLog( "Local Active: %d\n", IsLocalActive());
        StaticLog( "User Group Id: %d\n", GetUsrGrpId());
        StaticLog( "OS Type: %d\n", GetMcuOsType());
		StaticLog( "Local SSRC: %d\n", GetSysSSrc());
    }
    
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//会议表信息
struct TConfNameInfo
{
	CConfId		m_cConfId;          //会议号
	char		achConfName[MAXLEN_CONFNAME];   //会议名
public:
	TConfNameInfo( void )
	{
		m_cConfId.SetNull();
		memset( achConfName, 0, sizeof( achConfName ) );
	};
	void   SetConfName( LPCSTR lpszConfName )
	{
		if( lpszConfName != NULL )
		{
			strncpy( achConfName, lpszConfName, MAXLEN_CONFNAME );
		    achConfName[MAXLEN_CONFNAME - 1] = '\0';
		}
		else
		{
			memset(achConfName,0,sizeof(achConfName));
		}
	}
	LPCSTR GetConfName( void ) const { return achConfName; }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;



//定义交换通道结构 SrcIp->DisIp->RcvIp->DstIp, size=34
struct TSwitchChannel
{
    TSwitchChannel()
    {
        memset(this, 0, sizeof(TSwitchChannel));
    }
protected:
	TMt  m_tSrcMt;         //交换源终端
	u32  m_dwSrcIp;        //数据源Ip地址
	u32  m_dwDisIp;        //数据分发Ip地址
	u32  m_dwRcvIP;        //本地接收的IP地址(网络序),可以是组播(或广播)地址
	u16  m_wRcvPort;       //本地接收的端口 (机器序)
	u32  m_dwRcvBindIP;    //本地接收bind IP地址(网络序),本地地址
	u32  m_dwDstIP;        //将要发送的目标IP地址(网络序),可以是组播(或广播)地址
	u16  m_wDstPort;       //将要发送的目标端口 (机器序)
	u32  m_dwSndBindIP;    //发送目标接口IP地址.(网络序),本地地址

public:
	void SetSrcMt( TMt tMt ){ m_tSrcMt = tMt; }
	TMt  GetSrcMt( void ){ return m_tSrcMt; }
    void SetSrcIp(u32    dwSrcIp){ m_dwSrcIp = htonl(dwSrcIp);} 
    u32  GetSrcIp( void ) const { return ntohl(m_dwSrcIp); }
    void SetDisIp(u32    dwDisIp){ m_dwDisIp = htonl(dwDisIp);} 
    u32  GetDisIp( void ) const { return ntohl(m_dwDisIp); }
    void SetRcvIP(u32    dwRcvIP){ m_dwRcvIP = htonl(dwRcvIP);} 
    u32  GetRcvIP( void ) const { return ntohl(m_dwRcvIP); }
    void SetRcvPort(u16  wRcvPort){ m_wRcvPort = htons(wRcvPort);} 
    u16  GetRcvPort( void ) const { return ntohs(m_wRcvPort); }
    void SetRcvBindIP(u32    dwRcvBindIP){ m_dwRcvBindIP = htonl(dwRcvBindIP);} 
    u32  GetRcvBindIP( void ) const { return ntohl(m_dwRcvBindIP); }
    void SetDstIP(u32    dwDstIP){ m_dwDstIP = htonl(dwDstIP);} 
    u32  GetDstIP( void ) const { return ntohl(m_dwDstIP); }
    void SetDstPort(u16  wDstPort){ m_wDstPort = htons(wDstPort);} 
    u16  GetDstPort( void ) const { return ntohs(m_wDstPort); }
    void SetSndBindIP(u32    dwSndBindIP){ m_dwSndBindIP = htonl(dwSndBindIP);} 
    u32  GetSndBindIP( void ) const { return ntohl(m_dwSndBindIP); }
	BOOL IsNull( void ) const { return m_dwDstIP == 0 ? TRUE : FALSE; }
    BOOL IsSrcNull( void ) const { return m_dwSrcIp == 0 ? TRUE : FALSE; }
    BOOL IsRcvNull( void ) const { return m_dwRcvIP == 0 ? TRUE : FALSE; }
	void SetNull( ){ memset( this, 0, sizeof(TSwitchChannel) ); }
	BOOL operator ==( TSwitchChannel &tObj ) const
	{
		if( tObj.GetDstIP() == GetDstIP() && tObj.GetDstPort() == GetDstPort() )
		{
			return TRUE;
		}
        else
		{
			return FALSE;
		}
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//Mp信息
struct TMp
{
protected:
    u8   m_byMcuId;                   //MCU号
    u8   m_byMpId;                    //本Mp编号
    u8   m_byAttachMode;              //MP的附属方式
    u32  m_dwIpAddr;                  //Ip地址
    u8   m_byDoubleLink;              //双链
    u8   m_byIsMulticast;             //是否组播 0: 不组播 1：组播
    s8   m_abyMpAlias[MAXLEN_ALIAS];  //Mp别名	
	//4.0R4扩展字段
    u16  m_wVersion;			     //版本信息	ZGC	2007-10-10
public:
    void SetMcuId(u8 byMcuId) { m_byMcuId = byMcuId; }
    u8   GetMcuId(void) const { return m_byMcuId; }
    void SetMpId(u8 byMpId){ m_byMpId = byMpId; }
    u8   GetMpId(void) const { return m_byMpId; }
    void SetAttachMode(u8 byAttachMode) { m_byAttachMode = byAttachMode; }
    u8   GetAttachMode(void) const { return m_byAttachMode; }
    void SetIpAddr(u32 dwIP){ m_dwIpAddr = htonl(dwIP); }
    u32  GetIpAddr(void) { return ntohl(m_dwIpAddr); }
    void SetDoubleLink(u8 byDoubleLink) { m_byDoubleLink = byDoubleLink; }
    u8   GetDoubleLink(void) const { return m_byDoubleLink; }
    void SetMulticast(u8 byIsMulticast) { m_byIsMulticast = byIsMulticast; }
    u8   GetMulticast(void) const { return m_byIsMulticast; }
    const s8 * GetAlias(void) const { return m_abyMpAlias; }
    void SetAlias(s8 * pchAlias)
    {
        if (pchAlias != NULL)
        {
            strncpy(m_abyMpAlias, pchAlias, sizeof(m_abyMpAlias));
            m_abyMpAlias[sizeof(m_abyMpAlias) - 1] = '\0';
        }
        else
        {
            memset(m_abyMpAlias, 0, sizeof(m_abyMpAlias));
        }
    }
	u16 GetVersion(void) const { return ntohs(m_wVersion); }
	void SetVersion(u16 wVersion) { m_wVersion = htons(wVersion); }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//协议适配板配置信息(h323)
struct TMtAdpCfg 
{
protected:
    u16 m_wPartlistRefreshTime;             //级联时终端列表刷新时间间隔(s)
    u16 m_wAudInfoRefreshTime;              //级联时音频信息刷新时间间隔(s)
    u16 m_wVidInfoRefreshTime;              //级联时视频信息刷新时间间隔(s)

public:
    TMtAdpCfg(void) 
    {             
        SetPartListRefreshTime(PARTLIST_REFRESHTIME);  
        SetAudInfoRefreshTime(AUDINFO_REFRESHTIME);
        SetVidInfoRefreshTime(VIDINFO_REFRESHTIME);
    }    
    
    void    SetPartListRefreshTime(u16 wTime) { m_wPartlistRefreshTime = htons(wTime); }
    u16     GetPartListRefreshTime(void) { return ntohs(m_wPartlistRefreshTime); }
    void    SetAudInfoRefreshTime(u16 wTime) { m_wAudInfoRefreshTime = htons(wTime); }
    u16     GetAudInfoRefreshTime(void) { return ntohs(m_wAudInfoRefreshTime); }
    void    SetVidInfoRefreshTime(u16 wTime) { m_wVidInfoRefreshTime = htons(wTime); }
    u16     GetVidInfoRefreshTime(void) { return ntohs(m_wVidInfoRefreshTime); }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct  TCapExtraSet
{
protected:
    TVideoStreamCap m_tVideoCap;     //第一路最大能力
    TAudioStreamCap  m_tAudioCap;
    TDStreamCap   m_tDStreamCap;

public:

    TCapExtraSet(void){ Clear(); }
    void Clear(void)
    {
        m_tVideoCap.Clear();
        m_tAudioCap.Clear();
        m_tDStreamCap.Reset();
    }

    BOOL32 IsEmpty(void) const
    {
        if (MEDIA_TYPE_NULL == m_tVideoCap.GetMediaType() &&
            MEDIA_TYPE_NULL == m_tAudioCap.GetMediaType() &&
            MEDIA_TYPE_NULL == m_tDStreamCap.GetMediaType())
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }       
    }

    //视频
    void SetVideoStreamCap(const TVideoStreamCap &tVideoCap)
    {
        memcpy(&m_tVideoCap, &tVideoCap, sizeof(TVideoStreamCap));
    }
    TVideoStreamCap GetVideoStreamCap(void) const
    {
        return m_tVideoCap;
    }

    //音频
    void SetAudioStreamCap(const TAudioStreamCap &tAudioCap)
    {
        memcpy(&m_tAudioCap, &tAudioCap, sizeof(TAudioStreamCap));
    }
    TAudioStreamCap GetAudioStreamCap(void) const
    {
        return m_tAudioCap;
    }

    //双流
    void SetDStreamCap(const TDStreamCap &tDStreamCap)
    {
        memcpy(&m_tDStreamCap, &tDStreamCap, sizeof(TDStreamCap));
    }
    TDStreamCap GetDStreamCap(void) const
    {
        return m_tDStreamCap;
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

#define MAX_CAPEXTRASET_SIZE 1
//多能力集描述
struct TMultiCapSupport : public TCapSupport
{
protected:
    TCapExtraSet m_tCapExtraSet[MAX_CAPEXTRASET_SIZE];
    TDStreamCap m_tSecComDStream;
	u8 m_byTrackNum;

public:

    TMultiCapSupport(void){ Clear(); }

    void Clear(void)
    {
        TCapSupport::Clear();
        for (u8 byCapIndx = 0; byCapIndx < MAX_CAPEXTRASET_SIZE;  byCapIndx++)
        {
            m_tCapExtraSet[byCapIndx].Clear();
        }
        m_tSecComDStream.Reset();
		m_byTrackNum = 1;
    }

    //某一组能力组合
    BOOL32 SetCapExtraSet(const TCapExtraSet &tExtraSet, u8 bySetIndx)
    {
        if ( bySetIndx >= MAX_CAPEXTRASET_SIZE)
        {
            return FALSE;
        }

        memcpy(&m_tCapExtraSet[bySetIndx], &tExtraSet, sizeof(TCapExtraSet));

        return TRUE;

    }
    BOOL32 GetCapExtraSet(TCapExtraSet &tCapExtraSet, u8 bySetIndx) const
    {
        if( bySetIndx >= MAX_CAPEXTRASET_SIZE ||
            m_tCapExtraSet[bySetIndx].IsEmpty())
        {
            return FALSE;
        }
        memcpy(&tCapExtraSet, &m_tCapExtraSet[bySetIndx], sizeof(TCapExtraSet));

        return TRUE;
    }

    //视频
    BOOL32 SetExtraVideoCap(const TVideoStreamCap &tExtraVideo, u8 bySetIndx)
    {
        if (MAX_CAPEXTRASET_SIZE <= bySetIndx)
        {
            return FALSE;
        }
        
        m_tCapExtraSet[bySetIndx].SetVideoStreamCap(tExtraVideo);

        return TRUE;

    }
    BOOL32 GetExtraVideoCap(TVideoStreamCap &tExtraVideo, u8 bySetIndx) const
    {
        if ( bySetIndx >= MAX_CAPEXTRASET_SIZE )
        {
            return FALSE;
        }
        tExtraVideo = m_tCapExtraSet[bySetIndx].GetVideoStreamCap();

        return TRUE;
    }

    //音频
    BOOL32 SetExtraAudioCap(const TAudioStreamCap &tExtraAudio, u8 bySetIndx)
    {
        if (MAX_CAPEXTRASET_SIZE <= bySetIndx)
        {
            return FALSE;
        }
        
        m_tCapExtraSet[bySetIndx].SetAudioStreamCap(tExtraAudio);

        return TRUE;
        
    }
    BOOL32 GetExtraAudioCap(TAudioStreamCap &tExtraAudio, u8 bySetIndx) const
    {
        if (bySetIndx >= MAX_CAPEXTRASET_SIZE)
        {
            return FALSE;
        }

        tExtraAudio = m_tCapExtraSet[bySetIndx].GetAudioStreamCap();

        return TRUE;
    }

    //双流
    BOOL32 SetExtraDStreamCap(const TDStreamCap &tExtraDStream, u8 bySetIndx)
    {
        if (MAX_CAPEXTRASET_SIZE <= bySetIndx)
        {
            return FALSE;
        }

        m_tCapExtraSet[bySetIndx].SetDStreamCap(tExtraDStream);

        return TRUE;
        
    }
    BOOL32 GetExtraDStreamCap(TDStreamCap &tExtraDStream, u8 bySetIndx) const
    {
        if (bySetIndx >= MAX_CAPEXTRASET_SIZE ||
            m_tCapExtraSet[bySetIndx].IsEmpty())
        {
            return FALSE;
        }

        tExtraDStream = m_tCapExtraSet[bySetIndx].GetDStreamCap();

        return TRUE;

    }

    //设置第二共同双流能力
    TDStreamCap GetSecComDStreamCapSet( void ) const { return m_tSecComDStream; } 
	void        SetSecComDStreamCapSet( TDStreamCap &tDSCap ) { m_tSecComDStream = tDSCap; }

	void SetMainAudioTrackNum(const u8 byTrackNum) { m_byTrackNum = byTrackNum; }
	u8 GetMainAudioTrackNum( void ) const { return m_byTrackNum; }

    void Print(void) const
    {
        TCapSupport::Print();
        StaticLog( "Extra CapSet Support:\n");
        for (u8 byIndx = 0; byIndx < MAX_CAPEXTRASET_SIZE; byIndx++)
        {
            StaticLog( "\tExtraSet[%d]:", byIndx);
            if(m_tCapExtraSet[byIndx].IsEmpty())
                StaticLog( "NULL\n");
            else
                StaticLog( "\tType:%d\n\tRes:%d\n\tFps:%d\n\tHP:%d\n\taudtype:%d\n",				
                  m_tCapExtraSet[byIndx].GetVideoStreamCap().GetMediaType(),
                  m_tCapExtraSet[byIndx].GetVideoStreamCap().GetResolution(),
                  MEDIA_TYPE_H264 != m_tCapExtraSet[byIndx].GetVideoStreamCap().GetMediaType() ? m_tCapExtraSet[byIndx].GetVideoStreamCap().GetFrameRate() : m_tCapExtraSet[byIndx].GetVideoStreamCap().GetUserDefFrameRate(),
				  m_tCapExtraSet[byIndx].GetVideoStreamCap().IsSupportHP(),
				  m_tCapExtraSet[byIndx].GetAudioStreamCap().GetMediaType()
                   );

        }
        StaticLog( "Second Common DoubleStream:\n");
        StaticLog( "\tType:%d\n\tRes:%d\n\tFps:%d\n\tH239:%d\n\tHP:%d\n",
                  m_tSecComDStream.GetMediaType(),
                  m_tSecComDStream.GetResolution(),
                  MEDIA_TYPE_H264 != m_tSecComDStream.GetMediaType() ? m_tSecComDStream.GetFrameRate() : m_tSecComDStream.GetUserDefFrameRate(),
                  m_tSecComDStream.IsSupportH239(),
				  m_tSecComDStream.IsSupportHP());

		StaticLog( "m_byTrackNum:%d\n",m_byTrackNum );
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
//////////////////////////////////////////////////
//      级联消息用到的结构
//////////////////////////////////////////////////

struct TMcuMcuReq
{
	s8 m_szUserName[MAXLEN_PWD];
	s8 m_szUserPwd[MAXLEN_PWD];
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMtViewPos
{
    s32  m_nViewID;
    u8   m_bySubframeIndex;    
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMtVideoInfo
{
    s32			m_nViewCount;                   
    TMtViewPos  m_atViewPos[MAXNUM_VIEWINCONF]; //终端在哪些视图里
    s32			m_nOutputLID;                   //终端接收哪个视图
    s32			m_nOutVideoSchemeID;           //输入到终端的视频方案ID(当一个会议支持两种会议格式)                        
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMcuMcuMtInfo
{
    TMcuMcuMtInfo()
    {
        memset(this, 0, sizeof(TMcuMcuMtInfo));
    }
    TMt         m_tMt;
    s8 			m_szMtName[VALIDLEN_ALIAS_UTF8]; // 终端别名长度为16Byte // 扩为50 [pengguofeng 7/3/2013] 
    u32			m_dwMtIp; 
    s8          m_szMtDesc[VALIDLEN_ALIAS];
    u8			m_byMtType;          // 上级、下级Mcu 或Mt 
    u8          m_byManuId;          //厂商编号,参见厂商编号定义
	u8			m_byVideoIn;       
    u8			m_byVideoOut;      
    u8			m_byVideo2In;     
    u8			m_byVideo2Out;     
    u8			m_byAudioIn;       
    u8			m_byAudioOut;      
    u8		    m_byIsDataMeeting;    
    u8		    m_byIsVideoMuteIn;    
    u8			m_byIsVideoMuteOut;   
    u8			m_byIsAudioMuteIn;    
    u8			m_byIsAudioMuteOut;   
    u8          m_byIsConnected;     
    u8			m_byIsFECCEnable;     
    u8          m_byProtocolType;
    TMtVideoInfo m_tPartVideoInfo;   
	u8          m_byCasLevel; //相对与本级，终端所处级联树中的跳跃的级别数
	u8          m_abyMtIdentify[MAX_CASCADELEVEL]; //对应级别中终端的标识

public:
	BOOL32 operator == ( const TMcuMcuMtInfo& tOtherMMMtInfo ) const
	{
		const u32 dwSize = sizeof(TMcuMcuMtInfo);
		return (memcmp( this, &tOtherMMMtInfo, dwSize) == 0) ? TRUE : FALSE;
	}

	void Print()
	{
		StaticLog("TMt:<%d %d>\t Name:<%s> MtIp:%x MtDesc<%s> MtType:%d Manu:%d CasLvl:%d\n",
			m_tMt.GetMcuId(), m_tMt.GetMtId(), m_szMtName, m_dwMtIp, m_szMtDesc, m_byMtType,
			m_byManuId, m_byCasLevel);
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TCSchemeInfo
{
    s32		m_nVideoSchemeID;   //视频输出方案ID
    u32		m_dwMaxRate;        //max rate (kbps)
    u32		m_dwMinRate;        //min rate (kbps)
    BOOL	m_bCanUpdateRate;   // whether or not enable update rate
    s32     m_nFrameRate;       // frame rate (30、25)
    u8		m_byMediaType;      //
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TCViewInfo
{
    s32				m_nViewId;							//会议视图 ID
    u8              m_byRes;							//会议视图图像的分辨率
    u8              m_byCurGeoIndex;					//会议视图当前正在使用(输出)的几何布局,以在几何布局列表中的索引来标明
    u8              m_byGeoCount;						//几何布局的数目
    u8              m_abyGeoInfo[MAXNUM_GEOINVIEW];		//几何布局列表
    u8              m_byMtCount;						//会议视图的视频源的数目
    TMt             m_atMts[MAXNUM_SUBFRAMEINGEO];		//视频源列表
    u8              m_byVSchemeCount;					//可选的视频输出方案数目
    TCSchemeInfo    m_atVSchemes[MAXNUM_VSCHEMEINVIEW]; //视频输出方案列表

    // guzh [7/19/2007]
    u8              m_byAutoSwitchStatus;               //自动轮询回传
    u8              m_byDynamic;
    s32             m_nAutoSwitchTime;                  //轮询回传时间间隔
    s32             m_nVideoRole;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TCConfViewInfo
{
	u8          m_byDefViewIndex;			//默认会议视图索引
    u8          m_byViewCount;					//会议视图的数目
    TCViewInfo	m_atViewInfo[MAXNUM_VIEWINCONF];//会议视图列表
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//混音器信息，可能会有多个speaker，这里只是制定一个
struct TCMixerInfo
{
    s32    m_nMixerID;					//mixer ID
    TMt    m_tSpeaker;					//speaker Pid
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//MCU的混音器的信息
struct TCConfAudioInfo
{
    u8				m_byMixerCount;						//混音器数目
    u8				m_byDefMixerIndex;					//缺省混音器，以索引表示,它也是当前正在播音的混音器
    TCMixerInfo     m_tMixerList[MAXNUM_MIXERINCONF];	//混音器列表
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TCConfViewChangeNtf
{
	s32  m_nViewID;
    u8   m_bySubframeCount;    
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMtMediaChanStatus
{
	TMt		m_tMt;
    u8		m_byMediaMode;        //媒体模式（MODE_VIDEO,MODE_AUDIO）
    u8		m_byIsDirectionIn;    //媒体流流动方向
    BOOL	m_bMute;              //true:切断该方向的流动，false:打开该方向的媒体流动
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TCAutoSwitchReq
{
    TCAutoSwitchReq(){ memset(this, 0, sizeof(TCAutoSwitchReq)); }

    s32     m_nSwitchLayerId;              
    s32     m_nSwitchSpaceTime;     //轮训时间间隔
    s32     m_nAutoSwitchLevel;     //轮询级别,详细含义未知,1：开启，0：停止
    BOOL32  m_bSwitchOn;            //轮询启停
    
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TSetInParam
{
	TMt m_tMt;
	s32 m_nViewId;
	u8  m_bySubFrameIndex;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TCConfViewOutInfo
{    
	TMt m_tMt;
    s32 m_nOutViewID;         
    s32 m_nOutVideoSchemeID;  
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TSetOutParam 
{
    s32					m_nMtCount;           
    TCConfViewOutInfo   m_atConfViewOutInfo[MAXNUM_CONF_MT];
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


//////////////////////////////////////////////////////////////////////////
//						配置界面化所用结构
//////////////////////////////////////////////////////////////////////////

//mcu IP地址、子网掩码、网关IP、层号、槽号
struct TMcuEqpCfg
{
protected:
    u32 m_dwMcuIpAddr;          //mcu Ip地址
    u32 m_dwMcuSubnetMask;      //mcu 子网掩码
    u32 m_dwGWIpAddr;           //网关 Ip地址
    u8	m_byLayer;              //层号
    u8  m_bySlotId;             //槽id
    u8  m_byInterface;          //前后网口
	u8  m_byRemain;				// save
public:
    TMcuEqpCfg(void) { memset(this, 0, sizeof(TMcuEqpCfg)); }
    
    void    SetMcuIpAddr(u32 dwIpAddr) { m_dwMcuIpAddr = htonl(dwIpAddr); }     //host order
    u32     GetMcuIpAddr(void)  { return ntohl(m_dwMcuIpAddr); }
    void    SetMcuSubNetMask(u32 dwMask) { m_dwMcuSubnetMask = htonl(dwMask); }
    u32     GetMcuSubNetMask(void) { return ntohl(m_dwMcuSubnetMask); }
    void    SetGWIpAddr(u32 dwGWIpAddr) { m_dwGWIpAddr = htonl(dwGWIpAddr); }   //host order
    u32     GetGWIpAddr(void) { return ntohl(m_dwGWIpAddr); }
    void    SetLayer(u8 byLayer) { m_byLayer = byLayer; }
    u8      GetLayer(void) { return m_byLayer; }   
    void    SetSlot(u8 bySlotId) { m_bySlotId = bySlotId; }
    u8      GetSlot(void) { return m_bySlotId; }   
	void    SetInterface(u8 byInterface){ m_byInterface = byInterface;}
	u8      GetInterface(void) const { return m_byInterface;}
	
    void    Print(void)
    {
        StaticLog( "McuIpAddr:0x%x, McuSubnetMask:0x%x, GWIpAddr:0x%x, Layer:%d, SlotId:%d, Interface: %d\n",
                  GetMcuIpAddr(), GetMcuSubNetMask(), GetGWIpAddr(), GetLayer(), GetSlot(), GetInterface());
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMcuNetCfg
{
	TMcuEqpCfg	m_atMcuEqpCfg[MAXNUM_ETH_INTERFACE];
	u8			m_byInterface;
	
public:
	TMcuEqpCfg *GetMcuEqpCfg(u8 byIdx)
	{
		if( byIdx >= MAXNUM_ETH_INTERFACE )
		{
			return NULL;
		}
		return &m_atMcuEqpCfg[byIdx]; 
	}
	
	void SetMcuEqpCfg(u8 byIdx, TMcuEqpCfg tMcuEqpCfg)
	{
		if( byIdx >= MAXNUM_ETH_INTERFACE )
		{
			return;
		}
		m_atMcuEqpCfg[byIdx] = tMcuEqpCfg;
	}
	
	u8 GetInterface( void ) const
	{
		return m_byInterface;
	}
	void SetInterface( u8 byInterface)
	{
		m_byInterface = byInterface;
	}
	
	void Print(void)
	{
		for(u8 byLp = 0; byLp < MAXNUM_ETH_INTERFACE; byLp++)
		{
			m_atMcuEqpCfg[byLp].Print();		
		}
		StaticLog( "used interface is：%d\n", m_byInterface);
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

enum enNetworkType	// 网络地址类型，LAN or WAN 
{
	enBOTH	= 0,
	enWAN	= 1,
	enLAN	= 2
};

enum enUseModeType
{
	MODETYPE_3ETH	= 0,	//3网口模式
	MODETYPE_BAK	= 1		//备份模式
};

struct TMcu8KiNetCfg:TMcuNetCfg
{
	enUseModeType	m_enUseModeType;	// 3网口模式 || 备份模式
	enNetworkType	m_enNetworkType;    // LAN or WAN 
	BOOL32			m_bIsUseSip;		// 是否启用sip
	u8				m_bySipInEthIdx;	// SipIp在哪个网口上
	u32				m_dwSipIpAddr;		// SipIp
	u32				m_dwSipMaskAddr;	// SipMask
	u32				m_dwSipGwAddr;		// SipGw

public:

	TMcu8KiNetCfg()
    {
        Clear();
    }
	
	void Clear()
	{
		memset(this, 0, sizeof(*this));
		m_enNetworkType = enLAN;			//默认设置为私网
		m_enUseModeType = MODETYPE_3ETH;	//默认设置为3网口模式
	}

	//模式设置
	void SetUseModeType (enUseModeType enType)
	{
		m_enUseModeType = enType;	
	}
	
	enUseModeType GetUseModeType( void ) const
	{
		return  m_enUseModeType;
	}
	
	// 设置网络类型
	void SetNetworkType(enNetworkType enType) 
    {		
		m_enNetworkType = enType;		
	}
	
	// 获取网络类型
    enNetworkType GetNetworkType() const
    {
        return m_enNetworkType;
    }
	
    BOOL32 IsWan() const
    {
        return (m_enNetworkType == enWAN);
    }
	
    BOOL32 IsLan() const
    {
        return (m_enNetworkType == enLAN);
    }
	
	BOOL32 IsLanAndWan() const
    {
        return (m_enNetworkType == enBOTH);
    }
	
	void SetIsUseSip ( BOOL32 bIsUseSip )
	{
		m_bIsUseSip = bIsUseSip;
	}
	
	BOOL32 IsUseSip ( void ) const
	{
		return m_bIsUseSip;
	}

	void SetSipInEthIdx( u8 bySipInEthIdx )
	{
		m_bySipInEthIdx = bySipInEthIdx;
	}
	
	u8 GetSipInEthIdx ( void ) const
	{
		return m_bySipInEthIdx;
	}
	
	//SipIp设置
	void SetSipIpAddr(u32 dwSipIpAddr) { m_dwSipIpAddr = htonl(dwSipIpAddr); }
	u32  GetSipIpAddr()const {return ntohl(m_dwSipIpAddr); }

	void SetSipMaskAddr(u32 dwSipMaskAddr) { m_dwSipMaskAddr = htonl(dwSipMaskAddr); }
	u32  GetSipMaskAddr()const {return ntohl(m_dwSipMaskAddr); }

	void SetSipGwAddr(u32 dwSipGwAddr) { m_dwSipGwAddr = htonl(dwSipGwAddr); }
	u32  GetSipGwAddr()const {return ntohl(m_dwSipGwAddr); }

	void Print(void)
	{
		Print();		
		s8	achSipIpAddr[32] = {0};
		s8	achSipMaskAddr[32] = {0};
		s8	achSipGwAddr[32] = {0};
		u32 dwSipIpAddr = GetSipIpAddr();
		u32 dwSipMaskAddr = GetSipMaskAddr();
		u32 dwSipGwAddr = GetSipGwAddr();
		sprintf(achSipIpAddr, "%d.%d.%d.%d%c", (dwSipIpAddr>>24)&0xFF, (dwSipIpAddr>>16)&0xFF, (dwSipIpAddr>>8)&0xFF, dwSipIpAddr&0xFF, 0);
		sprintf(achSipMaskAddr, "%d.%d.%d.%d%c", (dwSipMaskAddr>>24)&0xFF, (dwSipMaskAddr>>16)&0xFF, (dwSipMaskAddr>>8)&0xFF, dwSipMaskAddr&0xFF, 0);
		sprintf(achSipGwAddr, "%d.%d.%d.%d%c", (dwSipGwAddr>>24)&0xFF, (dwSipGwAddr>>16)&0xFF, (dwSipGwAddr>>8)&0xFF, dwSipGwAddr&0xFF, 0);
		
		StaticLog("\n[TMcuNetCfg]:\nm_enUseModeType:%s\nm_enNetworkType:%s	\
				  \nm_dwSipIpAddr:%s\nm_dwSipMakAddr:%s\nm_dwSipgwAddr:%s\n",	\
				(m_enUseModeType == MODETYPE_3ETH ? "MODETYPE_3ETH":"MODETYPE_BAK"),	\
				(m_enNetworkType == enLAN ? "enLAN":(m_enNetworkType == enWAN ? "enWAN":"enBOTH")),	\
				 achSipIpAddr,achSipMaskAddr,achSipGwAddr);
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TNetParam
{
protected:   
    u32 m_dwIpAddr;		// 网络序
    u32 m_dwIpMask;		// 网络序
	
public:
    TNetParam()
    {
        SetNull();
    }
    
public:
    void SetNetParam( u32 dwIp, u32 dwIpMask )
    {
        m_dwIpAddr = htonl(dwIp);
        m_dwIpMask = htonl(dwIpMask); 
    }
	
    void SetNull(void)
    {
        m_dwIpAddr = 0;
        m_dwIpMask = 0;
    }
    
    BOOL32 IsValid() const
    {
        return ( 0 == m_dwIpAddr ) ? FALSE : TRUE ;
    }
    
    // 获取地址参数
    u32  GetIpAddr(void) const { return ntohl(m_dwIpAddr); }
    u32  GetIpMask(void) const{ return ntohl(m_dwIpMask); }
	
	BOOL   operator ==( const TNetParam & tObj ) const                  //判断是否相等
	{
		if ( GetIpAddr() == tObj.GetIpAddr() 
			&& GetIpMask() == tObj.GetIpMask() )
		{
			return TRUE;
		}
		return FALSE;
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TRoute
{
protected:
    TNetParam m_tDstNet;
    u32 m_dwGateway;
    u32 m_dwMetric;     //跃点数
	
public:
    TRoute(void) { SetNull(); }
	
    TNetParam GetDstNet(void) const { return m_tDstNet; }
    void    SetDstNet( const TNetParam &tDstNet ) { m_tDstNet = tDstNet; }
	
    u32 GetGateway(void) const { return ntohl(m_dwGateway); }
    void SetGateway( u32 dwGateway ) { m_dwGateway = htonl(dwGateway); }
	
    u32 GetMetric(void) const { return ntohl(m_dwGateway); }
    void SetMetric( u32 dwMetric ) { m_dwMetric = htonl(dwMetric); }
	
    void SetNull(void) 
    { 
        m_tDstNet.SetNull();
        m_dwGateway = 0;
        m_dwMetric = 0;
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TIpRouteCfg
{
	TNetParam	m_tNetParam; //ip,mask
    u32 m_dwGWIpAddr;        //网关 Ip地址
	
	void SetNetRouteParam(u32 dwIpAddr, u32 dwNetMask, u32 dwGWIP)
	{
		m_tNetParam.SetNetParam(dwIpAddr, dwNetMask);
		m_dwGWIpAddr = htonl(dwGWIP);
	}

	u32  GetIpAddr(void) const { return m_tNetParam.GetIpAddr(); }
    u32  GetIpMask(void) const{ return m_tNetParam.GetIpMask(); }
	u32  GetGwIp(void)	const { return ntohl(m_dwGWIpAddr); }

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TNetManuAlias
{
	s8  m_achName[MAXLEN_MULTINETALIAS];
	
	void SetAlias( LPCSTR lpzAlias )
	{
		if( lpzAlias != NULL )
		{
            memset( m_achName, 0, sizeof(m_achName));
			strncpy( m_achName, lpzAlias, MAXLEN_MULTINETALIAS-1 );
		}
	}

	const s8 *GetAlias(void) const
	{
		return m_achName;
	}

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TNetMANUInfo
{
	TIpRouteCfg		m_tIpRouteCfg;		
	TNetManuAlias	m_tNetManuAlias;	//多运营商别名

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//Multi-manufactory net access cfg
struct TMultiManuNetAccess
{
protected:
	//u8	m_byGkUsed;				//是否启用GK(1 为启用，0为不启用)
	//u8  m_byEnableMultiManuNet;		//是否启用多运营商  (1 为启用，0为不启用) (其实就是启用内置代理)
	//TMcuEthCfg8KE	m_tMultiNetCfg;	//多网段配置
	u8	m_byIpSecNum;				//条目数量
	TNetMANUInfo m_atNetManuInfo[MCU_MAXNUM_MULTINET];
		
public:

	void Init( void ){ memset(this, 0, sizeof(TMultiManuNetAccess)); }
	
	TMultiManuNetAccess() {Init();}
	
	// 传主机序
	u8	AddIpSection(u32 dwIpAddr, u32 dwNetMask, u32 dwGwIpAddr, const TNetManuAlias *pchAliasName)
	{
		if (m_byIpSecNum >= MCU_MAXNUM_MULTINET)
		{
			return MCU_MAXNUM_MULTINET;	//返回索引值为14，表fail
		}
		
		u8 byIdx = m_byIpSecNum;
		m_atNetManuInfo[byIdx].m_tIpRouteCfg.SetNetRouteParam(dwIpAddr, dwNetMask, dwGwIpAddr);
		s8* pachAlias = (s8*)(pchAliasName->m_achName);
		m_atNetManuInfo[byIdx].m_tNetManuAlias.SetAlias(pachAlias);
		m_byIpSecNum ++;
		
		return byIdx;
	}
	
	BOOL32 DelIpSection(u8 byIdx)
	{
		if(byIdx >= MCU_MAXNUM_MULTINET)
		{
			return FALSE;
		}
		
		for(u8 byLoop = byIdx; byLoop < m_byIpSecNum; byLoop ++)
		{
			
			m_atNetManuInfo[byLoop].m_tIpRouteCfg = m_atNetManuInfo[byLoop+1].m_tIpRouteCfg;

			m_atNetManuInfo[byLoop].m_tNetManuAlias.SetAlias(m_atNetManuInfo[byLoop+1].m_tNetManuAlias.GetAlias());
		}
		
		m_byIpSecNum --;
		
		return TRUE;
		
	}
	
	
	BOOL32 ModifyIpSection(u8 byIdx, u32 dwIpAddr, u32 dwNetMask, u32 dwGwIpAddr, const TNetManuAlias *pchAliasName)
	{
		if(byIdx >= MCU_MAXNUM_MULTINET)
		{
			return FALSE;
		}

		m_atNetManuInfo[byIdx].m_tIpRouteCfg.SetNetRouteParam(dwIpAddr, dwNetMask, dwGwIpAddr);
		s8* pachAlias = (s8*)(pchAliasName->m_achName);
		m_atNetManuInfo[byIdx].m_tNetManuAlias.SetAlias(pachAlias);
		
		return TRUE;
	}
	
	u8	GetIpSecNum( void ) const
	{
		return m_byIpSecNum;
	}


	//获取的均为主机序
	//获取目的IP地址
	u32 GetIpAddr(u8 byIdx) const
	{
		if( byIdx >= MCU_MAXNUM_MULTINET )
		{
			return 0;
		}
		
		return m_atNetManuInfo[byIdx].m_tIpRouteCfg.GetIpAddr();
	}
	
	u32 GetNetMask(u8 byIdx) const
	{
		if( byIdx >= MCU_MAXNUM_MULTINET )
		{
			return 0;
		}
		return m_atNetManuInfo[byIdx].m_tIpRouteCfg.GetIpMask();
	}
	
	u32 GetGWIp(u8 byIdx) const
	{
		if( byIdx >= MCU_MAXNUM_MULTINET )
		{
			return 0;
		}
		return m_atNetManuInfo[byIdx].m_tIpRouteCfg.GetGwIp();
	}
	
	BOOL32 GetAlias(u8 byIdx, TNetManuAlias *ptNetManuAlias) const
	{
		if( byIdx >= MCU_MAXNUM_MULTINET )
		{
			return FALSE;
		}
		
		memset(ptNetManuAlias->m_achName, 0, sizeof(ptNetManuAlias->m_achName));
		strncpy( ptNetManuAlias->m_achName, m_atNetManuInfo[byIdx].m_tNetManuAlias.GetAlias(), MAXLEN_MULTINETALIAS-1);
		
		return TRUE;
	}
	

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//多网口多运营商信息 支持16IP
struct TMultiEthManuNetAccess
{
protected:
	u8	m_byIpSecNum;				//条目数量
	TNetMANUInfo m_atNetManuInfo[MCU_MAXNUM_ADAPTER];
	
public:
	void Init( void ){ memset(this, 0, sizeof(TMultiEthManuNetAccess)); }
	
	TMultiEthManuNetAccess() {Init();}

	// 传主机序
	u8	AddIpSection(u32 dwIpAddr, u32 dwNetMask, u32 dwGwIpAddr, const TNetManuAlias *pchAliasName)
	{
		
		if (m_byIpSecNum >= MCU_MAXNUM_ADAPTER)
		{
			return MCU_MAXNUM_ADAPTER;
		}
		
		u8 byIdx = m_byIpSecNum;
		m_atNetManuInfo[byIdx].m_tIpRouteCfg.SetNetRouteParam(dwIpAddr, dwNetMask, dwGwIpAddr);
		s8* pachAlias = (s8*)(pchAliasName->m_achName);
		m_atNetManuInfo[byIdx].m_tNetManuAlias.SetAlias(pachAlias);
		m_byIpSecNum ++;
		
		return byIdx;
	}
	
	BOOL32 DelIpSection(u8 byIdx)
	{
		if(byIdx >= MCU_MAXNUM_ADAPTER)
		{
			return FALSE;
		}
		
		for(u8 byLoop = byIdx; byLoop < m_byIpSecNum; byLoop ++)
		{
			
			m_atNetManuInfo[byLoop].m_tIpRouteCfg = m_atNetManuInfo[byLoop+1].m_tIpRouteCfg;

			m_atNetManuInfo[byLoop].m_tNetManuAlias.SetAlias(m_atNetManuInfo[byLoop+1].m_tNetManuAlias.GetAlias());
		}
		
		m_byIpSecNum --;
		
		return TRUE;
		
	}
	
	
	BOOL32 ModifyIpSection(u8 byIdx, u32 dwIpAddr, u32 dwNetMask, u32 dwGwIpAddr, const TNetManuAlias *pchAliasName)
	{
		if(byIdx >= MCU_MAXNUM_ADAPTER)
		{
			return FALSE;
		}

		m_atNetManuInfo[byIdx].m_tIpRouteCfg.SetNetRouteParam(dwIpAddr, dwNetMask, dwGwIpAddr);
		s8* pachAlias = (s8*)(pchAliasName->m_achName);
		m_atNetManuInfo[byIdx].m_tNetManuAlias.SetAlias(pachAlias);
		
		return TRUE;
	}
	
	u8	GetIpSecNum( void ) const
	{
		return m_byIpSecNum;
	}


	//获取的均为主机序
	//获取目的IP地址
	u32 GetIpAddr(u8 byIdx) const
	{
		if( byIdx >= MCU_MAXNUM_ADAPTER )
		{
			return 0;
		}
		
		return m_atNetManuInfo[byIdx].m_tIpRouteCfg.GetIpAddr();
	}
	
	u32 GetNetMask(u8 byIdx) const
	{
		if( byIdx >= MCU_MAXNUM_ADAPTER )
		{
			return 0;
		}
		return m_atNetManuInfo[byIdx].m_tIpRouteCfg.GetIpMask();
	}
	
	u32 GetGWIp(u8 byIdx) const
	{
		if( byIdx >= MCU_MAXNUM_ADAPTER )
		{
			return 0;
		}
		return m_atNetManuInfo[byIdx].m_tIpRouteCfg.GetGwIp();
	}
	
	BOOL32 GetAlias(u8 byIdx, TNetManuAlias *ptNetManuAlias) const
	{
		if( byIdx >= MCU_MAXNUM_ADAPTER )
		{
			return FALSE;
		}
		
		memset(ptNetManuAlias->m_achName, 0, sizeof(ptNetManuAlias->m_achName));
		strncpy( ptNetManuAlias->m_achName, m_atNetManuInfo[byIdx].m_tNetManuAlias.GetAlias(), MAXLEN_MULTINETALIAS-1);
		
		return TRUE;
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMultiManuNetCfg
{
	u8  m_byEnableMultiManuNet;		//是否启用多运营商  (1 为启用，0为不启用) (其实就是启用内置代理)
	u32 m_dwGkIp;
	TMultiManuNetAccess	m_tMultiManuNetAccess;
	
	void SetGkIp(u32 dwGkIp) { m_dwGkIp = htonl(dwGkIp); }
	u32  GetGkIp(void) const { return ntohl(m_dwGkIp); }
	BOOL32 IsMultiManuNetEnable() const {return (1 == m_byEnableMultiManuNet)? TRUE: FALSE ;}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//mcu8000E某网口的 IP地址、子网掩码、网关IP
struct TMcuEthCfg8KE
{
	enum EmNetWorkMode
	{
		AUTO,		//自适应
		FULLD,			//Full Duplex
		HALFD,			//Half Duplex
		emModeEd
	};
protected:
    u32 m_dwIpAddr[MCU_MAXNUM_ADAPTER_IP];          //Ip地址
    u32 m_dwSubnetMask[MCU_MAXNUM_ADAPTER_IP];      //子网掩码
    u32 m_dwGWIpAddr[MCU_MAXNUM_ADAPTER_IP];        //网关 Ip地址
	u32 m_dwDefGWIpAddr;							//该网口默认网关ip (当某ip段未设网关，则用此网关)
	u8	m_byNetWorkMode;							//网口模式(全双工，半双工, 自动等)
	u8	m_byLinkStatus;								//启用状态(1为启用，2为禁用)
	u8	m_byIpSecNum;								//当前配置ip段的数目(1~16)
	u8	m_byReserved3;
	
public:
   
	void Init( void ){ memset(this, 0, sizeof(TMcuEthCfg8KE)); }
	
	TMcuEthCfg8KE(void) { Init(); }

	u8	AddIpSection(u32 dwIpAddr, u32 dwNetMask, u32 dwGwIpAddr)
	{
		if (m_byIpSecNum >= MCU_MAXNUM_ADAPTER_IP)
		{
			return MCU_MAXNUM_ADAPTER_IP;	//返回索引值为16，表fail
		}

		u8 byIdx = m_byIpSecNum;
		m_dwIpAddr[byIdx] = htonl(dwIpAddr);
		m_dwSubnetMask[byIdx] = htonl(dwNetMask);
		m_dwGWIpAddr[byIdx] = htonl(dwGwIpAddr);
		m_byIpSecNum ++;

		return byIdx;
	}

	BOOL32 DelIpSection(u8 byIdx)
	{
		if(byIdx >= MCU_MAXNUM_ADAPTER_IP)
		{
			return FALSE;
		}

		for(u8 byLoop = byIdx; byLoop < m_byIpSecNum - 1; byLoop ++)
		{
			m_dwIpAddr[byLoop] = m_dwIpAddr[byLoop + 1];
			m_dwSubnetMask[byLoop] = m_dwSubnetMask[byLoop +1];
			m_dwGWIpAddr[byLoop] = m_dwGWIpAddr[byLoop + 1];
		}

		m_byIpSecNum --;

		return TRUE;

	}

	BOOL32 ModifyIpSection(u8 byIdx, u32 dwIpAddr, u32 dwNetMask, u32 dwGwIpAddr)
	{
		if(byIdx >= MCU_MAXNUM_ADAPTER_IP)
		{
			return FALSE;
		}
		
		m_dwIpAddr[byIdx] = htonl(dwIpAddr);
		m_dwSubnetMask[byIdx] = htonl(dwNetMask);
		m_dwGWIpAddr[byIdx] = htonl(dwGwIpAddr);

		return TRUE;

	}

	u8	GetIpSecNum( void )
	{
		return m_byIpSecNum;
	}

public:
    void    SetMcuIpAddr(u32 dwIpAddr, u8 byIdx = 0) 
	{ 
		if( byIdx >= MCU_MAXNUM_ADAPTER_IP )
		{
			return;
		}
		m_dwIpAddr[byIdx] = htonl(dwIpAddr);

	}     
    u32     GetMcuIpAddr(u8 byIdx = 0)  
	{ 
		if( byIdx >= MCU_MAXNUM_ADAPTER_IP )
		{
			return 0;
		}
		return ntohl(m_dwIpAddr[byIdx]); 
	}

    void    SetMcuSubNetMask(u32 dwMask, u8 byIdx = 0) 
	{ 
		if( byIdx >= MCU_MAXNUM_ADAPTER_IP )
		{
			return;
		}
		m_dwSubnetMask[byIdx] = htonl(dwMask); 
	}
    u32     GetMcuSubNetMask(u8 byIdx = 0) 
	{
		if( byIdx >= MCU_MAXNUM_ADAPTER_IP )
		{
			return 0;
		}
		return ntohl(m_dwSubnetMask[byIdx]); 
	}
    
	void    SetGWIpAddr(u32 dwGWIpAddr, u8 byIdx = 0) 
	{ 
		if( byIdx >= MCU_MAXNUM_ADAPTER_IP )
		{
			return;
		}
		m_dwGWIpAddr[byIdx] = htonl(dwGWIpAddr);
	}   
    u32     GetGWIpAddr(u8 byIdx = 0) 
	{ 
		if( byIdx >= MCU_MAXNUM_ADAPTER_IP )
		{
			return 0;
		}
		return ntohl(m_dwGWIpAddr[byIdx]);
	}
	
	void	SetDefGWIpAddr(u32 dwGWIpAddr) { m_dwDefGWIpAddr = htonl(dwGWIpAddr) ;}
	u32		GetDefGWIpAddr() { return ntohl(m_dwDefGWIpAddr); }

	void	SetNetWorkMode( u8 byMode ){ m_byNetWorkMode = byMode;}
	u8		GetNetWorkMode(void) { return m_byNetWorkMode; }
	
	void	SetLinkStatus( u8 byLinkStatus){ m_byLinkStatus = byLinkStatus;}
	u8		GetLinkStatus() { return m_byLinkStatus ; }
	BOOL32	IsLinkOn(){ return (m_byLinkStatus == 1) ? TRUE: FALSE ; }
	
	void    Print(void)
    {
		for(u8 byIdx = 0; byIdx < MCU_MAXNUM_ADAPTER_IP; byIdx ++)
		{
			if(GetMcuIpAddr(byIdx) == 0)
			{
				continue;
			}
			StaticLog( "[IP%u]-IpAddr:0x%x, SubnetMask:0x%x, GWIpAddr:0x%x\n",
				byIdx, GetMcuIpAddr(byIdx), GetMcuSubNetMask(byIdx), GetGWIpAddr(byIdx));
		}
        
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


//mcu8000E 多网口配置信息
struct TMultiNetCfgInfo
{
	TMcuEthCfg8KE m_atMcuEthCfg8KE[MAXNUM_ETH_INTERFACE];
	
public:
	TMultiNetCfgInfo(void) 
	{
		for(u8 byLp = 0; byLp < MAXNUM_ETH_INTERFACE; byLp ++)
		{
			m_atMcuEthCfg8KE[byLp].Init();
		}
	}

	void SetMcuEthCfg(u8 byIdx, const TMcuEthCfg8KE &tMcuEthCfg8KE)
	{
		if( byIdx >= MAXNUM_ETH_INTERFACE )
			return;

		m_atMcuEthCfg8KE[byIdx] = tMcuEthCfg8KE;
	}

	TMcuEthCfg8KE* GetMcuEthCfg(u8 byIdx)
	{
		if( byIdx >= MAXNUM_ETH_INTERFACE )
			return  NULL;

		return &m_atMcuEthCfg8KE[byIdx];
	}

	u8	 GetLinkedOnEth()
	{
		u8 byEthIdx = MAXNUM_ETH_INTERFACE;
		for(u8 byLoop = 0; byLoop < MAXNUM_ETH_INTERFACE; byLoop ++)
		{
			if (m_atMcuEthCfg8KE[byLoop].IsLinkOn())
			{
				byEthIdx = byLoop;
				break;
			}
		}
		return byEthIdx; 
	}

	void Print( void )
	{
		for(u8 byLoop = 0; byLoop < MAXNUM_ETH_INTERFACE; byLoop ++)
		{
			StaticLog( "Eth.%u:\n---------\n", byLoop);
			m_atMcuEthCfg8KE[byLoop].Print();
		}
	}
	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


//GK配置信息
struct TGKCfgInfo
{
protected:
    u32 m_dwGkIpAddr;			//GK Ip地址 (网络序)
	u8	m_byGkUsed;				//是否启用内置GK (1 为启用，0为不启用)
	
public:
    TGKCfgInfo(void) { memset(this, 0, sizeof(TGKCfgInfo)); }
    
    void    SetGkIpAddr(u32 dwIpAddr) { m_dwGkIpAddr = htonl(dwIpAddr); }   
    u32     GetGkIpAddr(void)  { return ntohl(m_dwGkIpAddr); }
	
	void    SetGkUsed( u8 byVal) { m_byGkUsed = byVal;}
	BOOL32  IsGkUsed(void) { return (m_byGkUsed == 1); }
	
	
    void    Print(void)
    {
        StaticLog( "GkIpAddr:0x%x, GkUsed:%d\n",
			GetGkIpAddr(), IsGkUsed() );
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;



//GK,代理配置信息
struct TGKProxyCfgInfo
{
protected:
    u32 m_dwGkIpAddr;			//GK Ip地址 (网络序)
	u32 m_dwProxyIpAddr;		//proxy IP
	u16	m_byProxyPort;			//代理用的port	(网络序)
	u8	m_byGkUsed;				//是否启用内置GK (1 为启用，0为不启用)
	u8  m_byProxyUsed;			//是否启用代理(1 为启用，0为不启用)

public:
    TGKProxyCfgInfo(void) { memset(this, 0, sizeof(TGKProxyCfgInfo)); }
    
    void    SetGkIpAddr(u32 dwIpAddr) { m_dwGkIpAddr = htonl(dwIpAddr); }   
    u32     GetGkIpAddr(void)  { return ntohl(m_dwGkIpAddr); }
	
	void    SetProxyIpAddr(u32 dwIpAddr) { m_dwProxyIpAddr = htonl(dwIpAddr); }   
    u32     GetProxyIpAddr(void)  { return ntohl(m_dwProxyIpAddr); }

	void	SetProxyPort(u16 wPort) { m_byProxyPort = htons(wPort); }
	u16		GetProxyPort(){ return ntohs(m_byProxyPort); }

	void    SetGkUsed( u8 byVal) { m_byGkUsed = byVal;}
	BOOL32  IsGkUsed(void) { return (m_byGkUsed == 1); }

	void	SetProxyUsed( u8 byVal ){ m_byProxyUsed = byVal; }
	BOOL32	IsProxyUsed(void) { return (m_byProxyUsed == 1); }


    void    Print(void)
    {
        StaticLog( "GkIpAddr:0x%x, ProxyIpAddr:0x%x, Proxy Port:%d, GkUsed:%d, ProxyUsed: %d\n",
			GetGkIpAddr(), GetProxyIpAddr(), GetProxyPort(), IsGkUsed(), IsProxyUsed() );
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
/*
//mcu8000E各网口配置信息 // 暂不用
struct TMcuEqpCfg8KE
{
	TMcuEthCfg8KE m_atMcuEthCfg8KE[MAXNUM_ETH_INTERFACE];

public:
	void Print( void )
	{
		for(u8 byLoop = 0; byLoop < MAXNUM_ETH_INTERFACE; byLoop ++)
		{
			StaticLog( "Eth.%u:\t", byLoop);
			m_atMcuEthCfg8KE[byLoop].Print();
		}
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
*/

//预方案中通道所需配置资源结构   jlb
struct THduChnlCfgInfo
{
protected:
	u8   m_byChnlIdx;               //通道索引号
	u8   m_byEqpId;					//设备ID
	u8   m_byChnlVolumn;            //通道音量
	BOOL m_bIsChnlBeQuiet;          //通道静音
	s8   m_achEqpAlias[MAXLEN_EQP_ALIAS];   //外设别名
public:
	THduChnlCfgInfo(){ SetNull(); }
    void SetNull(void)
	{
        memset( this, 0, sizeof(THduChnlCfgInfo) );
	    return;
	}

	u8   GetChnlIdx(void) const { return m_byChnlIdx; }
	void SetChnlIdx(u8 val) { m_byChnlIdx = val; }

	u8   GetEqpId(void) const { return m_byEqpId; }
	void SetEqpId(u8 val) { m_byEqpId = val; }

	u8   GetChnlVolumn(void) const { return m_byChnlVolumn; }
	void SetChnlVolumn(u8 val) { m_byChnlVolumn = val; }

	BOOL GetIsChnlBeQuiet(void) const { return m_bIsChnlBeQuiet; }
	void SetIsChnlBeQuiet(BOOL val) { m_bIsChnlBeQuiet = val; }

	void SetEqpAlias(LPCSTR pchEqpAlias)
    {
		//fixme
        memcpy(m_achEqpAlias, pchEqpAlias, MAXLEN_EQP_ALIAS);
        m_achEqpAlias[sizeof(m_achEqpAlias) - 1] = '\0';
    }
	
	LPCSTR GetEqpAlias(void){ return m_achEqpAlias; }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
; 

//HDU预设方案结构   jlb
struct THduStyleCfgInfo 
{
protected:
	u8   m_byStyleIdx;          //方案Id；

	u8   m_byTotalChnlNum;      //方案需要的通道数，即界面中可配置的最大通道数
                                //根据不同的方案可为1、2、4、…、56，最大56个
	THduChnlCfgInfo m_atHduChnlCfgTable[MAXNUM_HDUCFG_CHNLNUM];  //各通道需要的配置资源
	//m_bywidth * m_byHeight <= 56
	u8   m_byWidth;             //界面配置的宽度
	u8   m_byHeight;            //界面配置的高度
    u8   m_byVolumn;            //所有通道统一音量大小
	BOOL m_bIsBeQuiet;          //所有通道是否统一静音
	s8  m_achSchemeAlias[MAX_HDUSTYLE_ALIASLEN];   //预方案别名
	
public:
    THduStyleCfgInfo(void) { SetNull(); }
	void	SetNull(void)  { memset(this, 0, sizeof(THduStyleCfgInfo)); }
	BOOL32  IsNull(void) 
    { 
        THduStyleCfgInfo tInfo;
        return (0 == memcmp(this, &tInfo, sizeof(THduStyleCfgInfo)));
    }
    
	//patHduChnlCfgTable  为数组指针，元素个数为MAXNUM_HDUCFG_CHNLNUM
	void GetHduChnlCfgTable(THduChnlCfgInfo *patHduChnlCfgTable)
	{
		memcpy(patHduChnlCfgTable, m_atHduChnlCfgTable, MAXNUM_HDUCFG_CHNLNUM*sizeof(THduChnlCfgInfo));
        return;
	}
	//patHduChnlCfgTable  为数组指针，元素个数为MAXNUM_HDUCFG_CHNLNUM
	BOOL32 SetHduChnlCfgTable(THduChnlCfgInfo *patHduChnlCfgTable)
	{
		BOOL32 bRet = TRUE;
		if (NULL == patHduChnlCfgTable)
		{
			bRet = FALSE;
			OspPrintf(TRUE, FALSE, "[mcustruct.h]:SetHduChnlCfgTable()-->patHduChnlCfgTable is NULL\n");
		}

		memcpy(m_atHduChnlCfgTable, patHduChnlCfgTable, MAXNUM_HDUCFG_CHNLNUM*sizeof(THduChnlCfgInfo));
		return bRet;
	}

	u8   GetStyleIdx(void) const { return m_byStyleIdx; }
	void SetStyleIdx(u8 val) { m_byStyleIdx = val; }

	u8   GetTotalChnlNum(void) const { return m_byTotalChnlNum; }
	void SetTotalChnlNum(u8 val) { m_byTotalChnlNum = val; }

	u8   GetWidth(void) const { return m_byWidth; }
	void SetWidth(u8 val) { m_byWidth = val; }

	u8   GetHeight(void) const { return m_byHeight; }
	void SetHeight(u8 val) { m_byHeight = val; }

	u8   GetVolumn(void) const { return m_byVolumn; }
	void SetVolumn(u8 val) { m_byVolumn = val; }

	BOOL GetIsBeQuiet(void) const { return m_bIsBeQuiet; }
	void SetIsBeQuiet(BOOL val) { m_bIsBeQuiet = val; }

	void SetSchemeAlias(LPCSTR pchSchemeAlias)
    {
        memcpy(m_achSchemeAlias, pchSchemeAlias, sizeof(m_achSchemeAlias));
        m_achSchemeAlias[MAX_HDUSTYLE_ALIASLEN-1] = '\0';
    }
    
	LPCSTR GetSchemeAlias(void){ return m_achSchemeAlias; }

	inline BOOL operator == (const THduStyleCfgInfo& tHduStyle )
	{
		BOOL bRet = TRUE;
		if( this == &tHduStyle )
		{
			return bRet;
		}
		
		if ( 0 != memcmp(this, &tHduStyle, sizeof(THduStyleCfgInfo)) )
		{
			bRet = FALSE;
		}
		return bRet;
	}

    void Print(void)
	{
		StaticLog( "[THduStyleCfgInfo]: StyleIdx:%d, TotalChnlNum:%d, Width:%d, Height:%d, Volumn:%d, IsBeQuiet:%d\n",
			      GetStyleIdx(), GetTotalChnlNum(), GetWidth(), GetHeight(), GetVolumn(), GetIsBeQuiet());
		StaticLog( "[THduStyleCfgInfo]: Hdu Scheme Alias is:%s \n",m_achSchemeAlias);
	    
		StaticLog( "[THduStyleCfgInfo]:\n");
		u16 wIndex;
		for ( wIndex=0; wIndex<GetTotalChnlNum(); wIndex++)
	    {
			StaticLog( "[%d]:  ChnlIdx:%d, EqpId:%d, ChnlVolumn:%d, IsChnlBeQuiet:%d\n",
				       m_atHduChnlCfgTable[wIndex].GetChnlIdx(),
					   m_atHduChnlCfgTable[wIndex].GetEqpId(),
					   m_atHduChnlCfgTable[wIndex].GetChnlVolumn(),
    				   m_atHduChnlCfgTable[wIndex].GetIsChnlBeQuiet());
	    }

		return;
	}

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// hdu批量轮询预案信息
struct THduPollSchemeInfo 
{
protected:
	u32  m_dwKeepTime;                       // 轮询周期
	u8   m_byCycles;                         // 轮询周期次数
	u8   m_byStatus;                         // 当前批量轮询状态
	u8   m_byReserved1;
    u8   m_byReserved2;
public:
	THduStyleCfgInfo m_tHduStyleCfgInfo;     // 批量预案预案版面信息

public:
	THduPollSchemeInfo()
	{
        SetNull();
	}
	void SetNull( void )
	{
		m_dwKeepTime = 0;
		m_byCycles = 0;
		m_byStatus = POLL_STATE_NONE;
		m_byReserved1 = 0;
		m_byReserved2 = 0;
		
		m_tHduStyleCfgInfo.SetNull();
	}
	u32   GetKeepTime( void )
	{
		return ntohl(m_dwKeepTime);
	}
	void SetKeepTime( u32 dwKeepTime )
	{
		m_dwKeepTime = htonl(dwKeepTime);
	}

	u8   GetCycles( void )
	{
		return m_byCycles;
	}
	void SetCycles( u8 byCycles )
	{
		m_byCycles = byCycles;
	}

	u8   GetStatus( void )
	{
		return m_byStatus;
	}
	void SetStatus( u8 byStatus )
	{
		m_byStatus = byStatus;
	}

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// 批量轮询前，预案中被占用通道信息
struct THduChnlInfoInUse
{
protected:
	u8    m_byConfIdx;       // 会议索引号
	u8    m_byEqpId;         // 外设ID
	u8    m_byChlIdx;        // 通道索引号
	u8    m_byResverd;       

public:
	THduChnlInfoInUse()
	{
		memset(this, 0x0, sizeof(THduChnlInfoInUse));
	}

	u8    GetConfIdx( void ){ return m_byConfIdx; }
	void  SetConfIdx( u8 byConfIdx ){ m_byConfIdx = byConfIdx; }

	u8    GetEqpId( void ){ return m_byEqpId; }
	void  SetEqpId( u8 byEqpId ){ m_byEqpId = byEqpId; }

	u8    GetChlIdx( void ){ return m_byChlIdx; }
	void  SetChlIdx( u8 byChlIdx ){ m_byChlIdx = byChlIdx; }

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//外设配置基类
struct TEqpCfgInfo
{
protected:    
    s8  m_szAlias[MAXLEN_EQP_ALIAS];		//别名
    u16 m_wMcuStartPort;					//mcu接收起始端口号
    u16 m_wEqpStartPort;					//外设接收起始端口号    
    u8  m_byEqpId;							//设备ID
    u8  m_bySwitchBrdIndex;					//交换板索引     
    u8  m_byRunningBrdIndex;				//运行板索引
    u8  m_byMapCount;						//使用的MAP个数
    u8  m_abyMapId[MAXNUM_MAP];				//Map编号数组

public:
    TEqpCfgInfo(void) { memset(this, 0, sizeof(TEqpCfgInfo)); }

    void    SetEqpId(u8 byEqpId) { m_byEqpId = byEqpId; }
    u8      GetEqpId(void) { return m_byEqpId; }
    void    SetMcuStartPort(u16 wPort) { m_wMcuStartPort = htons(wPort); }
    u16     GetMcuStartPort(void)   { return ntohs(m_wMcuStartPort); } 
    void    SetEqpStartPort(u16 wPort) { m_wEqpStartPort = htons(wPort); }
    u16     GetEqpStartPort(void) { return ntohs(m_wEqpStartPort); }
    void    SetSwitchBrdIndex(u8 byIndex) { m_bySwitchBrdIndex = byIndex; }
    u8      GetSwitchBrdIndex(void) { return m_bySwitchBrdIndex; }
    void    SetRunningBrdIndex(u8 byIndex) { m_byRunningBrdIndex = byIndex; }
    u8      GetRunningBrdIndex(void)    { return m_byRunningBrdIndex; }

    //下列方法仅供子类高清MAU特殊处理用
protected:
    void    SetMapCount(u8 byCount) { m_byMapCount = byCount; }
    u8      GetMapCount(void) const { return m_byMapCount; }

public:
    void    SetAlias(LPCSTR lpszAlias)
    {
        if(NULL != lpszAlias)
        {
            strncpy(m_szAlias, lpszAlias, sizeof(m_szAlias));
            m_szAlias[sizeof(m_szAlias) - 1] = '\0';
        }        
    }
    const s8* GetAlias(void) { return m_szAlias; }

    BOOL32    SetMapId(u8 *pbyMapId, u8 byCount)
    {
        if(NULL == pbyMapId || byCount > MAXNUM_MAP)
            return FALSE;
        
        memcpy(m_abyMapId, pbyMapId, byCount);
        m_byMapCount = byCount;
        return TRUE;
    }
    BOOL32     GetMapId(u8 *pbyMapId, u8 &byCount)
    {
        if(NULL == pbyMapId)
            return FALSE;
        
        memcpy(pbyMapId, m_abyMapId, m_byMapCount);
        byCount = m_byMapCount;
        return TRUE;
    }

    void PrintMap(void)
    {
        StaticLog( "MapCount: %d, MapId: %d,%d,%d,%d,%d\n", 
            m_byMapCount, m_abyMapId[0], m_abyMapId[1], m_abyMapId[2], m_abyMapId[3], m_abyMapId[4]);
        
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// 混音器信息
//设备id,别名,mcu起始端口,设备起始端口,交换板索引,运行板索引,每个map最大混音组数,map编号
struct TEqpMixerCfgInfo : public TEqpCfgInfo
{
protected:    
    u8  m_byMaxChnnlInGrp;					// 混音组最大通道数
   
public:
    TEqpMixerCfgInfo(void):m_byMaxChnnlInGrp(MAXNUM_MIXER_CHNNL) {}
    
    void    SetMaxChnnlInGrp(u8 byChnnlNum) { m_byMaxChnnlInGrp = byChnnlNum; }
    u8      GetMaxChnnlInGrp(void)  { return m_byMaxChnnlInGrp; } 

    void    Print(void) 
    {
        StaticLog( "\nMixer:%s\nEqpId:%d, SwitchBrd:%d, RunBrd:%d, McuPort:%d, MixerPort:%d, MaxChnnl:%d\n",
                  GetAlias(), GetEqpId(), GetSwitchBrdIndex(), GetRunningBrdIndex(), 
                  GetMcuStartPort(), GetEqpStartPort(), GetMaxChnnlInGrp());
        PrintMap();
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// 录像机信息
//设备id,别名,mcu起始端口,设备起始端口,交换板索引,录像机ip
struct TEqpRecCfgInfo : public TEqpCfgInfo
{
protected:
    u32 m_dwIpAddr;							//数字放象、录象机的Ip地址

public:
    TEqpRecCfgInfo(void):m_dwIpAddr(0) {}
  
    void    SetIpAddr(u32 dwIpAddr)  { m_dwIpAddr = htonl(dwIpAddr); }  //host order
    u32     GetIpAddr(void) { return ntohl(m_dwIpAddr); }

    void    Print(void)
    {
        StaticLog( "\nRec:%s\nEqpId:%d, SwitchBrd:%d, McuPort:%d, RecPort:%d, RecIp:0x%x\n",
                  GetAlias(), GetEqpId(), GetSwitchBrdIndex(), GetMcuStartPort(), 
                  GetEqpStartPort(), GetIpAddr());
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// 高清bas板信息
//设备id,别名,mcu起始端口,设备起始端口,交换板索引,运行板索引,高清bas板ip
struct TEqpBasHDCfgInfo : public TEqpCfgInfo
{
protected:
    u32 m_dwIpAddr;							//Ip地址

public:
    TEqpBasHDCfgInfo(void):m_dwIpAddr(0) {}
  
    void    SetIpAddr(u32 dwIpAddr)  { m_dwIpAddr = htonl(dwIpAddr); }  //host order
    u32     GetIpAddr(void) { return ntohl(m_dwIpAddr); }
    void    SetType(u8 byType) { SetMapCount(byType); }
    u8      GetType(void) const { return GetMapCount(); }

    void    Print(void)
    {
        StaticLog( "\nRec:%s\nEqpId:%d, SwitchBrd:%d, McuPort:%d, RecPort:%d, RecIp:0x%x, Type.%d\n",
                  GetAlias(), GetEqpId(), GetSwitchBrdIndex(), GetMcuStartPort(), 
                  GetEqpStartPort(), GetIpAddr(), GetType());
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// 电视墙信息
//设备id,别名,设备起始端口,交换板索引,map编号
struct TEqpTvWallCfgInfo : public TEqpCfgInfo
{     
    void    Print(void)
    {
        StaticLog( "\nTvWall:%s\nEqpId:%d, RunBrd:%d, TWPort:%d\n",
                 GetAlias(), GetEqpId(), GetRunningBrdIndex(), GetEqpStartPort());
        PrintMap();
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
struct THduChnlModePort
{
public:
	THduChnlModePort()
	{
		memset(this, 0x0, sizeof(THduChnlModePort));
	}
	u8 GetZoomRate() const { return byZoomRate; }
	void SetZoomRate(u8 val) { byZoomRate = val; }

    u8 GetOutPortType() const { return byOutPortType; }
    void SetOutPortType(u8 val) { byOutPortType = val; }
	
    u8 GetOutModeType() const { return byOutModeType; }
    void SetOutModeType(u8 val) { byOutModeType = val; }
	
	u8 GetScalingMode() const { return (reserved & 0x03);}
	void SetScalingMode(u8 byMode) { reserved = (reserved | 0x03) & (byMode | 0xFC); }

private:
    u8 byOutPortType;
    u8 byOutModeType;
    u8 byZoomRate;
	// 1)该字段低2位已使用，用于标识ScalingMode [11/29/2011 chendaiwei]
    u8 reserved;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
; 

//Hdu信息  jlb
struct TEqpHduCfgInfo : public TEqpCfgInfo 
{
protected:
	
    THduChnlModePort atHduChnlModePort[MAXNUM_HDU_CHANNEL];    
	u8	m_byStartMode;//启动模式：(0: hdu_2 1:hdu_1 2:hdu_L )
public:
	TEqpHduCfgInfo()
	{
        memset(&atHduChnlModePort, 0X0, sizeof(atHduChnlModePort));
		SetStartMode(STARTMODE_HDU_M);
	}

	void SetStartMode(u8 byStartMode){m_byStartMode = byStartMode; }
	u8   GetStartMode(){ return m_byStartMode; }

	void GetHduChnlModePort( u8 byNum, THduChnlModePort &tHduChnModePort )
	{
		if (byNum == 0 || byNum == 1)
		{
			tHduChnModePort.SetOutModeType( atHduChnlModePort[byNum].GetOutModeType() );
			tHduChnModePort.SetOutPortType( atHduChnlModePort[byNum].GetOutPortType() );
		    tHduChnModePort.SetZoomRate( atHduChnlModePort[byNum].GetZoomRate() );
			tHduChnModePort.SetScalingMode( atHduChnlModePort[byNum].GetScalingMode() );
		}
		else
		{
			OspPrintf(TRUE, FALSE, "[TEqpHduCfgInfo]:GetHduChnlModePort()'s byNum wrong!\n");
			return;			
		}
		
		return;
	}
	
    void SetHduChnlModePort( u8 byNum, THduChnlModePort &tHduChnModePort )
	{
		if (byNum == 0 || byNum == 1)
		{
			atHduChnlModePort[byNum].SetOutModeType( tHduChnModePort.GetOutModeType() );
 			atHduChnlModePort[byNum].SetOutPortType( tHduChnModePort.GetOutPortType() );
            atHduChnlModePort[byNum].SetZoomRate( tHduChnModePort.GetZoomRate() );
			atHduChnlModePort[byNum].SetScalingMode(tHduChnModePort.GetScalingMode());
		}
		else
		{
			OspPrintf(TRUE, FALSE, "[TEqpHduCfgInfo]:SetHduChnlModePort()'s byNum wrong!\n");
			return;	
		}
		
		return;
	}
	
	void Print(void)
	{
		StaticLog( "\nHdu:%s\nEqpId:%d, RunBrd:%d, HduPort:%d\n,ZoomRate[0] = %d, ZoomRate[1] = %d\n HduOutPortType[0]:%d, HduOutModeType[0]:%d\n,HduOutPortType[1]:%d, HduOutModeType[1]:%d ,HduScaleMode[0]:%d, HduScalMode[1]:%d\n",
			GetAlias(), GetEqpId(), GetRunningBrdIndex(), GetEqpStartPort(), 
			atHduChnlModePort[0].GetZoomRate(), atHduChnlModePort[1].GetZoomRate(),
			atHduChnlModePort[0].GetOutPortType(), atHduChnlModePort[0].GetOutModeType(),
			atHduChnlModePort[1].GetOutPortType(), atHduChnlModePort[1].GetOutModeType(),
			atHduChnlModePort[0].GetScalingMode(),atHduChnlModePort[1].GetScalingMode());
	}
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
;

//svmp(单vmp模式)信息   jlb
struct TEqpSvmpCfgInfo : public TEqpCfgInfo
{
	void SetVmpType (u8 byVmpSubType) { m_abyMapId[0] = byVmpSubType; } //abyMapId不使用，用来存储VMP子类型[5/11/2012 chendaiwei]
	u8	 GetVmpType ()const { return m_abyMapId[0]; }

	TEqpSvmpCfgInfo ()
	{
		m_abyMapId[0] = TYPE_MPUSVMP;//默认是mpu的vmp
	}

    void    Print(void)
    {
        StaticLog( "\nDvmp:%s\nEqpId:%d, SwitchBrd:%d, RunBrd:%d, McuPort:%d, DvmpPort:%d,VmpType:%d\n",
			GetAlias(), GetEqpId(), GetSwitchBrdIndex(), GetRunningBrdIndex(),
			GetMcuStartPort(), GetEqpStartPort(),GetVmpType());
    }
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
;

//dvmp(双vmp模式)中单个vmp的信息   jlb
struct TEqpDvmpCfgBasicInfo : public TEqpCfgInfo
{

    void    Print(void)
    {
        StaticLog( "\nDvmp:%s\nEqpId:%d, SwitchBrd:%d, RunBrd:%d, McuPort:%d, DvmpPort:%d\n",
			GetAlias(), GetEqpId(), GetSwitchBrdIndex(), GetRunningBrdIndex(),
			GetMcuStartPort(), GetEqpStartPort());
    }
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
;

//双vmp模式的配置信息   jlb
struct TEqpDvmpCfgInfo
{
public:
	TEqpDvmpCfgBasicInfo tEqpDvmpCfgBasicInfo[2];    //获得两个vmp的信息

public:
	TEqpDvmpCfgInfo()
	{
		memset( this, 0 ,sizeof(TEqpDvmpCfgInfo) );
	}
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
;

//MpuBas信息    jlb
struct TEqpMpuBasCfgInfo : public TEqpCfgInfo
{
	u8 m_byStartMode;//启动模式(0:bap_4 , 1:bap_2)

	TEqpMpuBasCfgInfo(void)
	{
		SetStartMode(TYPE_MPUBAS); //默认为TYPE_MPUBAS
	}

	void SetStartMode(u8 byStartMode){ m_byStartMode = byStartMode;}
	u8   GetStartMode(){ return m_byStartMode; }

	void Print(void)
	{
	    StaticLog( "\nBas:%s\nEqpId:%d, SwitchBrd:%d, RunBrd:%d, McuPort:%d, BasPort:%d,StartMode:%d\n",
		    GetAlias(), GetEqpId(), GetSwitchBrdIndex(), GetRunningBrdIndex(),
		    GetMcuStartPort(), GetEqpStartPort(),GetStartMode());
        PrintMap();
	}
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
;

// Ebap 信息
//设备id,别名,mcu起始端口,设备起始端口,交换板索引,运行板索引,map编号
struct TEqpEbapCfgInfo : public TEqpCfgInfo
{
    void    Print(void)
    {
        StaticLog( "\nEbap:%s\nEqpId:%d, SwitchBrd:%d, RunBrd:%d, McuPort:%d, EbapPort:%d\n",
			GetAlias(), GetEqpId(), GetSwitchBrdIndex(), GetRunningBrdIndex(),
			GetMcuStartPort(), GetEqpStartPort());
        PrintMap();
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// Evpu 信息
//设备id,别名,mcu起始端口,设备起始端口,交换板索引,运行板索引,map编号
struct TEqpEvpuCfgInfo : public TEqpCfgInfo
{
    void    Print(void)
    {
        StaticLog( "\nVmp:%s\nEqpId:%d, SwitchBrd:%d, RunBrd:%d, McuPort:%d, VmpPort:%d\n",
            GetAlias(), GetEqpId(), GetSwitchBrdIndex(), GetRunningBrdIndex(),
            GetMcuStartPort(), GetEqpStartPort());
        PrintMap();
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


// Bas 信息
//设备id,别名,mcu起始端口,设备起始端口,交换板索引,运行板索引,map编号
struct TEqpBasCfgInfo : public TEqpCfgInfo
{
    void    Print(void)
    {
        StaticLog( "\nBas:%s\nEqpId:%d, SwitchBrd:%d, RunBrd:%d, McuPort:%d, BasPort:%d\n",
                GetAlias(), GetEqpId(), GetSwitchBrdIndex(), GetRunningBrdIndex(),
                GetMcuStartPort(), GetEqpStartPort());
        PrintMap();
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// Vmp 信息
//设备id,别名,mcu起始端口,设备起始端口,交换板索引,运行板索引,map编号,同时视频编码路数
struct TEqpVmpCfgInfo : public TEqpCfgInfo
{
protected:
    u8  m_byEncodeNum;						//同时支持的视频编码路数

public:
    TEqpVmpCfgInfo(void) :m_byEncodeNum(0) {}

    void    SetEncodeNum(u8 byEncodeNum) { m_byEncodeNum = byEncodeNum; }
    u8      GetEncodeNum(void)  { return m_byEncodeNum; }

    void    Print(void)
    {
        StaticLog( "\nVmp:%s\nEqpId:%d, SwitchBrd:%d, RunBrd:%d, McuPort:%d, VmpPort:%d, EncodeNum:%d\n",
            GetAlias(), GetEqpId(), GetSwitchBrdIndex(), GetRunningBrdIndex(),
            GetMcuStartPort(), GetEqpStartPort(), GetEncodeNum());
        PrintMap();
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//Mtw复合电视墙信息
//设备id,别名,设备起始端口,交换板索引,map编号
struct TEqpMTvwallCfgInfo : public TEqpCfgInfo
{
    void    Print(void)
    {
        StaticLog( "\nMTvwall:%s\nEqpId:%d, RunBrd:%d, MTWPort:%d\n",
            GetAlias(), GetEqpId(), GetRunningBrdIndex(), GetEqpStartPort());
        PrintMap();
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// Prs信息
//设备id,别名,mcu起始端口,设备起始端口,交换板索引,运行板索引,重传检测点1,2,3,过期丢弃时间
struct TPrsCfgInfo : public TEqpCfgInfo
{
protected:
    u16 m_wFirstTimeSpan;						//第一个重传检测点(ms)
    u16 m_wSecondTimeSpan;						//第二个重传检测点(ms)
    u16 m_wThirdTimeSpan;						//第三个重传检测点(ms)
    u16 m_wLoseTimeSpan;						//过期丢弃的时间跨度(ms)

public:
    TPrsCfgInfo(void)
    {
        SetFirstTimeSpan(DEF_FIRSTTIMESPAN_PRS);
        SetSecondTimeSpan(DEF_SECONDTIMESPAN_PRS);
        SetThirdTimeSpan(DEF_THIRDTIMESPAN_PRS);
        SetLoseTimeSpan(DEF_LOSETIMESPAN_PRS);
    }

    void    SetFirstTimeSpan(u16 wTimeSpan) { m_wFirstTimeSpan = htons(wTimeSpan); }
    u16     GetFirstTimeSpan(void)  { return ntohs(m_wFirstTimeSpan); }
    void    SetSecondTimeSpan(u16 wTimeSpan) { m_wSecondTimeSpan = htons(wTimeSpan); }
    u16     GetSecondTimeSpan(void) { return ntohs(m_wSecondTimeSpan); }
    void    SetThirdTimeSpan(u16 wTimeSpan) { m_wThirdTimeSpan = htons(wTimeSpan); }
    u16     GetThirdTimeSpan(void)  { return ntohs(m_wThirdTimeSpan); }
    void    SetLoseTimeSpan(u16 wTimeSpan) { m_wLoseTimeSpan = htons(wTimeSpan); }
    u16     GetLoseTimeSpan(void)   { return ntohs(m_wLoseTimeSpan); }

    void    Print(void)
    {
        StaticLog( "\nPrs:%s\nEqpId:%d, SwitchBrd:%d, RunBrd:%d, McuPort:%d\nPrsPort:%d, FstTime:%d, SndTime:%d, ThdTime:%d, LoseTime:%d\n",
            GetAlias(), GetEqpId(), GetSwitchBrdIndex(), GetRunningBrdIndex(), 
            GetMcuStartPort(), GetEqpStartPort(), GetFirstTimeSpan(), 
            GetSecondTimeSpan(), GetThirdTimeSpan(), GetLoseTimeSpan());
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// VRS新录播信息
struct TEqpVrsRecCfgInfo
{
protected:
	u8 m_byVrsId;							// vrs新录播索引号（1-16）
	s8 m_achAlias[MAXLEN_EQP_ALIAS+1];		// vrs新录播别名
	TMtAlias m_tVrsRec;						// vrs新录播呼叫信息
public:
    TEqpVrsRecCfgInfo(void) { memset( this, 0, sizeof(TEqpVrsRecCfgInfo)); }
    void SetVrsAlias(LPCSTR lpszAlias)
    {	
		s8 achAlias[MAXLEN_EQP_ALIAS+1] = {0};
		
        if ( NULL != lpszAlias)
		{
			strncpy(achAlias, lpszAlias, strlen(lpszAlias));
			
			//1.去除字符串的前导及后续空格以及TAB键
			Trim(achAlias); 
			achAlias[sizeof(achAlias)-1] = '\0';
			
			//2.截取非法字符之前的字符串
			for(u8 byIdx = 0; byIdx < MAXLEN_EQP_ALIAS +1; byIdx++)
			{
				if(achAlias[byIdx] == ' ' || achAlias[byIdx] == ',' || achAlias[byIdx] == '\t')
				{
					achAlias[byIdx] = '\0';
					
					break;
				}
			}
			
			strncpy(m_achAlias, achAlias, sizeof(m_achAlias));
			m_achAlias[sizeof(m_achAlias)-1] = '\0';
		}
    }
    LPCSTR GetVrsAlias(void){ return m_achAlias; }
	void SetVrsCallAlias(TMtAlias tVrsRec) { m_tVrsRec =  tVrsRec;}
	TMtAlias GetVrsCallAlias(void){ return m_tVrsRec; }
	void SetVrsId(u8 byVrsId) { m_byVrsId = byVrsId; }
	u8 GetVrsId() { return m_byVrsId; }

    void    Print(void)
    {
        StaticLog( "\nVrsId.%d VrsRec:%s\n CallAliasType:%d, CallAlias:%s, VrsRecIp:0x%x\n",
			m_byVrsId, GetVrsAlias(), m_tVrsRec.m_AliasType, m_tVrsRec.m_achAlias, m_tVrsRec.m_tTransportAddr.GetIpAddr());
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//单板配置信息
struct TBrdCfgInfo
{	
protected:
	u8				m_byIndex;					    // 索引号
    u8				m_byLayer;					    // 层号
    u8              m_bySlotId;                     //槽id
    u8              m_bySlotType;                   //槽类型
    u8				m_byType;					    // 类型
    u32				m_dwBrdIp;					    // 单板Ip
    u8				m_bySelNetPort;				    // 网口选择
    u8				m_bySelMulticast;			    // 组播选择   
    u8              m_byBrdState;                   // 单板状态
    u8              m_byOsType;                     // 单板的OS类型 OS_TYPE_WIN32 ...
    s8              m_szAlias[MAXLEN_BRDALIAS];     // 商用名，如 BRD_ALIAS_TUI
public:
    TBrdCfgInfo(void) { memset(this, 0, sizeof(TBrdCfgInfo)); }

    void    SetIndex(u8 byIndex) { m_byIndex = byIndex; }
    u8      GetIndex(void) const { return m_byIndex; }
    void    SetLayer(u8 byLayer) { m_byLayer = byLayer; }
    u8      GetLayer(void) const { return m_byLayer; }    
    void    SetType(u8 byType) { m_byType = byType; }
    u8      GetType(void) const { return m_byType; }
    void    SetIpAddr(u32 dwIpAddr) { m_dwBrdIp = htonl(dwIpAddr); }    //host order
    u32     GetIpAddr(void) const { return ntohl(m_dwBrdIp); }
    void    SetSelNetPort(u8 bySel) { m_bySelNetPort = bySel; }
    u8      GetSelNetPort(void) const { return m_bySelNetPort; }
    void    SetSelMulticast(u8 bySel) { m_bySelMulticast = bySel; }
    u8      GetSelMulticast(void) const{ return m_bySelMulticast; }
    u8      GetBrdState(void) const { return m_byBrdState;}
    void    SetBrdState(u8 byBrdState) { m_byBrdState = byBrdState;}
    u8      GetOsType(void) const { return m_byOsType; }
    void    SetOsType(u8 byOsType) { m_byOsType = byOsType; }

    void SetSlot(u8 bySlotId, u8 bySlotType)
    {
        m_bySlotType = bySlotType;
        m_bySlotId  = bySlotId;      
    }

    void GetSlot(u8 &bySlotId, u8 &bySlotType) const
    { 
        bySlotId = m_bySlotId;
        bySlotType = m_bySlotType;
    }    

    void SetAlias(const s8* pszAlias)
    {
        memset( m_szAlias, 0, sizeof(m_szAlias) );
        strncpy( m_szAlias, pszAlias,  sizeof(m_szAlias) - 1);
    }
    const s8* GetAlias() const
    {
        return m_szAlias;
    }

    void Print(void) const
    {
        u8 bySlotId;
        u8 bySlotType;
        GetSlot(bySlotId, bySlotType);
        StaticLog( "Index:%d, Layer:%d, SlotId%d, SlotType%d, BrdType%d, Alias:%s, BrdIp:0x%x, EthInt:%d, Multi:%d, BrdState:%d, OsType:%d\n",
                  GetIndex(), GetLayer(), bySlotId, bySlotType, GetType(), GetAlias(), GetIpAddr(), GetSelNetPort(), GetSelMulticast(), GetBrdState(), GetOsType());
    }
}
#ifndef WIN32
__attribute__( (packed) )
#endif
;

// 单板状态
struct TBoardStatusNotify
{
public:
    TBoardStatusNotify()
    { 
        memset(this, 0, sizeof(TBoardStatusNotify));
        m_byBrdState = BOARD_OUTLINE;
    }

    u8 GetBrdLayer(void) {return m_byBrdLayer;}
    u8 GetBrdSlot(void) {return m_byBrdSlot;}
    u8 GetBrdType(void) {return m_byBrdType;}
    u8 GetBrdState(void) {return m_byBrdState;}
    u8 GetBrdOsType(void) {return m_byOsType;}

    void SetBrdLayer(u8 byLayer) { m_byBrdLayer = byLayer;}
    void SetBrdSlot(u8 bySlot) { m_byBrdSlot = bySlot;}
    void SetBrdType(u8 byType) { m_byBrdType = byType;}
    void SetBrdState(u8 byState) { m_byBrdState = byState;}
    void SetBrdOsType(u8 byType) { m_byOsType = byType; }

private: 
    u8 m_byBrdLayer;
    u8 m_byBrdSlot;
    u8 m_byBrdType;
    u8 m_byBrdState;
    u8 m_byOsType;
}
#ifndef WIN32
__attribute__( (packed) )
#endif
;

//N+1冷备份信息结构
struct TNPlusInfo
{
protected:
    u32 m_dwNPlusMcuIp;                     //若支持N+1且不是备份工作模式，需要配置N+1备份mcu的地址
    u16 m_wNPlusRtdTime;                    //N+1备份rtd时间(s)
    u16 m_wNPlusRtdNum;                     //N+1备份rtd次数
    u8  m_byNPlusRollBack;                  //是否支持故障恢复，默认支持
    u8  m_byNPlusMode;                      //是否支持N+1备份
    u8  m_byNPlusBackupMode;                //是否N+1备份工作模式
    u8  m_byReserved;                       //保留
    
public:
    TNPlusInfo() : m_dwNPlusMcuIp(0),
                   m_wNPlusRtdTime(0),
                   m_wNPlusRtdNum(0),
                   m_byNPlusRollBack(0),
                   m_byNPlusMode(0),
                   m_byNPlusBackupMode(0),
                   m_byReserved(0){}
    
    void    SetIsNPlusMode(BOOL32 bNPlus) { m_byNPlusMode = bNPlus ? 1 : 0; }   //是否支持N+1备份
    BOOL32  IsNPlusMode(void) const { return (1 == m_byNPlusMode ? TRUE : FALSE); }
    void    SetIsNPlusBackupMode(BOOL32 bBackupMode) { m_byNPlusBackupMode = bBackupMode ? 1 : 0; }
    BOOL32  IsNPlusBackupMode(void) const{ return (1 == m_byNPlusBackupMode ? TRUE : FALSE); } //是否N+1备份工作模式
    void    SetNPlusMcuIp(u32 dwIp) { m_dwNPlusMcuIp = htonl(dwIp); }   
    u32     GetNPlusMcuIp(void)const { return ntohl(m_dwNPlusMcuIp); } //N+1备份mcu的地址(host order)
    void    SetNPlusRtdTime(u16 wTime) { m_wNPlusRtdTime = htons(wTime); }
    u16     GetNPlusRtdTime(void) const{ return ntohs(m_wNPlusRtdTime); }
    void    SetNPlusRtdNum(u16 wNum) { m_wNPlusRtdNum = htons(wNum); }
    u16     GetNPlusRtdNum(void) const{ return ntohs(m_wNPlusRtdNum); }
    void    SetIsNPlusRollBack(BOOL32 bRollBack) { m_byNPlusRollBack = bRollBack ? 1 : 0; }
    BOOL32  GetIsNPlusRollBack(void) const{ return (1 == m_byNPlusRollBack ? TRUE : FALSE); }

    void    Print() const
    {
        StaticLog( "\nNPlusInfo:\nNPlusMode:%d, backupMode:%d, mcuIp:0x%x, rtd:%d, Num:%d\n",
                                IsNPlusMode(), IsNPlusBackupMode(), GetNPlusMcuIp(), 
                                GetNPlusRtdTime(), GetNPlusRtdNum());
    }
}
#ifndef WIN32
__attribute__( (packed) )
#endif
;

//mcu本地配置信息
struct TMcuLocalCfgInfo
{    
protected:
    s8  m_szMcuAlias[MAXLEN_ALIAS];			// Mcu 别名
    s8  m_szMcuE164[MAXLEN_E164];			// E164号
    s8  m_szMcuHardVer[MAXLEN_ALIAS];		// Mcu 硬件版本号   
    s8  m_szMcuSoftVer[MAXLEN_ALIAS];		// Mcu 软件版本号
    
    u16 m_wDiscCheckTime;                   //终端断链检测时间(s)
    u16 m_wDiscCheckTimes;                  //终端断链检测次数
    u16 m_wPartlistRefreshTime;             //级联时终端列表刷新时间间隔(s)
    u16 m_wAudInfoRefreshTime;              //级联时音频信息刷新时间间隔(s)
    u16 m_wVidInfoRefreshTime;              //级联时视频信息刷新时间间隔(s)
    u8  m_bySaveBandWidth;                  //是否节省带宽(0-不节省，1-节省)
    u8  m_byReserved;                       //保留

    TNPlusInfo m_tNPlusInfo;

	u8 m_byIsHoldDefaultConf;				//是否支持缺省会议召开(0-不支持，1-支持)
	u8 m_byIsShowMMcuMtList;				//是否支持显示上级mcu终端列表(0-不支持，1-支持)
	u8 m_byMaxMcsOnGoingConfNum;			//本级最多召开的mcs即时会议数
	u8 m_byAdminLevel;						//本级召开的会议的行政级别(1到4 1最大)
	s8 m_byConfNameShowType;				//在下级会议中显示的本级的会议名称的类型(1:会议名称(默认值) 2:mcu名称＋会议名称)
	u8 m_byIsMMcuSpeaker;					//是否默认将上级mcu作为发言人(0:不是 1:是

	u32 m_dwReserve;						//保留

public:
    TMcuLocalCfgInfo() : m_wDiscCheckTime(DEF_DISCCHECKTIME),
                         m_wDiscCheckTimes(DEF_DISCCHECKTIMES),
                         m_wPartlistRefreshTime(PARTLIST_REFRESHTIME),
                         m_wAudInfoRefreshTime(AUDINFO_REFRESHTIME),
                         m_wVidInfoRefreshTime(VIDINFO_REFRESHTIME),
                         m_byReserved(0),
						 m_byIsHoldDefaultConf(0),
						 m_byIsShowMMcuMtList(0),
						 m_byMaxMcsOnGoingConfNum(DEF_MCSONGOINGCONFNUM_MAX),
						 m_byAdminLevel(DEF_ADMINLVEL),
						 m_byConfNameShowType(1),
						 m_byIsMMcuSpeaker(1),
						 m_dwReserve(0)
    { 
        memset(m_szMcuAlias, 0, sizeof(m_szMcuAlias));
        memset(m_szMcuE164, 0, sizeof(m_szMcuE164));
        memset(m_szMcuHardVer, 0, sizeof(m_szMcuHardVer));
        memset(m_szMcuSoftVer, 0, sizeof(m_szMcuSoftVer));
    }
    BOOL32  IsNull(void)
    {
        TMcuLocalCfgInfo tInfo;
        return (0 == memcmp(this, &tInfo, sizeof(TMcuLocalCfgInfo)));
    }

    void    SetIsSaveBandWidth(u8 bySave) { m_bySaveBandWidth = (0 == bySave) ? 0 : 1; }
    u8      GetIsSaveBandWidth(void) const { return m_bySaveBandWidth; }
    void    SetDiscCheckTime(u16 wTime) { m_wDiscCheckTime = htons(wTime); }
    u16     GetDiscCheckTime(void) const { return ntohs(m_wDiscCheckTime); }
    void    SetDiscCheckTimes(u16 wTimes) { m_wDiscCheckTimes = htons(wTimes); }
    u16     GetDiscCheckTimes(void) const { return ntohs(m_wDiscCheckTimes); }
    void    SetPartListRefreshTime(u16 wTime) { m_wPartlistRefreshTime = htons(wTime); }
    u16     GetPartListRefreshTime(void) const { return ntohs(m_wPartlistRefreshTime); }
    void    SetAudInfoRefreshTime(u16 wTime) { m_wAudInfoRefreshTime = htons(wTime); }
    u16     GetAudInfoRefreshTime(void) const { return ntohs(m_wAudInfoRefreshTime); }
    void    SetVidInfoRefreshTime(u16 wTime) { m_wVidInfoRefreshTime = htons(wTime); }
    u16     GetVidInfoRefreshTime(void) const { return ntohs(m_wVidInfoRefreshTime); }

    void    SetNPlusInfo(TNPlusInfo &tInfo) { m_tNPlusInfo = tInfo; }
    TNPlusInfo GetNPlusInfo() const {return m_tNPlusInfo;}
    void    SetIsNPlusMode(BOOL32 bNPlus) { m_tNPlusInfo.SetIsNPlusMode( bNPlus ); }   //是否支持N+1备份
    BOOL32  IsNPlusMode(void) const{ return m_tNPlusInfo.IsNPlusMode(); }
    void    SetIsNPlusBackupMode(BOOL32 bBackupMode) { m_tNPlusInfo.SetIsNPlusBackupMode(bBackupMode); }
    BOOL32  IsNPlusBackupMode(void) const{ return m_tNPlusInfo.IsNPlusBackupMode(); } //是否N+1备份工作模式
    void    SetNPlusMcuIp(u32 dwIp) { m_tNPlusInfo.SetNPlusMcuIp(dwIp); }   
    u32     GetNPlusMcuIp(void) const{ return m_tNPlusInfo.GetNPlusMcuIp(); } //N+1备份mcu的地址(host order)
    void    SetNPlusRtdTime(u16 wTime) { m_tNPlusInfo.SetNPlusRtdTime(wTime); }
    u16     GetNPlusRtdTime(void) const{ return m_tNPlusInfo.GetNPlusRtdTime(); }
    void    SetNPlusRtdNum(u16 wNum) { m_tNPlusInfo.SetNPlusRtdNum(wNum); }
    u16     GetNPlusRtdNum(void) const{ return m_tNPlusInfo.GetNPlusRtdNum(); }
    void    SetIsNPlusRollBack(BOOL32 bRollBack) { m_tNPlusInfo.SetIsNPlusRollBack(bRollBack); }
    BOOL32  GetIsNPlusRollBack(void) const{ return m_tNPlusInfo.GetIsNPlusRollBack(); }

	void	SetIsHoldDefaultConf( u8 byIsHold ){ m_byIsHoldDefaultConf = (0 == byIsHold) ? 0 : 1; }
	u8		GetIsHoldDefaultConf( void ){ return m_byIsHoldDefaultConf; }
	void	SetIsShowMMcuMtList( u8 byIsShow ){ m_byIsShowMMcuMtList = (0 == byIsShow) ? 0 : 1;  }
	u8		GetIsShowMMcuMtList( void ){ return m_byIsShowMMcuMtList; }
	void	SetMaxMcsOnGoingConfNum( u8 byNum ){ m_byMaxMcsOnGoingConfNum = byNum; }
	u8		GetMaxMcsOnGoingConfNum( void ){ return m_byMaxMcsOnGoingConfNum; };
	void	SetAdminLevel( u8 byAdminLevel ){ m_byAdminLevel = byAdminLevel; };
	u8		GetAdminLevel( void ){ return m_byAdminLevel; }
	void	SetConfNameShowType( u8 byType ) { m_byConfNameShowType = ( 2 == byType ) ? 2 : 1; }
	u8		GetConfNameShowType(void) const{ return m_byConfNameShowType; }
	void	SetIsMMcuSpeaker( u8 byIsMMcuSpeaker ){ m_byIsMMcuSpeaker = (0 == byIsMMcuSpeaker) ? 0 : 1; }
	u8		GetIsMMcuSpeaker( ){ return m_byIsMMcuSpeaker; }

    
    void SetMcuAlias(LPCSTR lpszAlias)
    {
        if(NULL != lpszAlias)
        {
            strncpy(m_szMcuAlias, lpszAlias, sizeof(m_szMcuAlias));
            m_szMcuAlias[sizeof(m_szMcuAlias) - 1] = '\0';
        }        
    }
    const s8* GetMcuAlias(void) const{ return m_szMcuAlias; }

    void SetMcuE164(LPCSTR lpszE164)
    {
        if(NULL != lpszE164)
        {
            strncpy(m_szMcuE164, lpszE164, sizeof(m_szMcuE164));
            m_szMcuE164[sizeof(m_szMcuE164) - 1] = '\0';
        }        
    }
    const s8* GetMcuE164(void) const{ return m_szMcuE164; }

    void SetMcuHardVer(LPCSTR lpszHardVer)          //mcu 内部使用
    {
        if(NULL != lpszHardVer)
        {
            strncpy(m_szMcuHardVer, lpszHardVer, sizeof(m_szMcuHardVer));
            m_szMcuHardVer[sizeof(m_szMcuHardVer) - 1] = '\0';
        }        
    }
    const s8* GetMcuHardVer(void) const{ return m_szMcuHardVer; }

    void SetMcuSoftVer(LPCSTR lpszSoftVer)          //mcu 内部使用
    {
        if(NULL != lpszSoftVer)
        {
            strncpy(m_szMcuSoftVer, lpszSoftVer, sizeof(m_szMcuSoftVer));
            m_szMcuSoftVer[sizeof(m_szMcuSoftVer) - 1] = '\0';
        }        
    }
    const s8* GetMcuSoftVer(void) const{ return m_szMcuSoftVer; }

    void    Print(void) const
    {
        StaticLog( "\nLocalInfo:\nMcuAlias:%s, E164:%s \nHardVer:%s \nSoftVer:%s",
											GetMcuAlias(), GetMcuE164(), GetMcuHardVer(), GetMcuSoftVer());

		StaticLog("\nIsSaveBW:%d DiscTime:%d, PartList:%d, Aud:%d, Vid:%d",
											GetIsSaveBandWidth(), GetDiscCheckTime(), GetPartListRefreshTime(),
										    GetAudInfoRefreshTime(), GetVidInfoRefreshTime());

		StaticLog("\nNPlusMode:%d, backupMode:%d, mcuIp:0x%x, rtd:%d, Num:%d",
											IsNPlusMode(), IsNPlusBackupMode(), GetNPlusMcuIp(), 
										    GetNPlusRtdTime(), GetNPlusRtdNum());

		StaticLog("\nIsHoldDefaultConf:%d IsShowMMcuMtList:%d MaxMcsOnGoingConfNum:%d AdminLevel:%d LocalConfNameShowType:%d m_byIsMMcuSpeaker:%d\n",
											m_byIsHoldDefaultConf,m_byIsShowMMcuMtList,
											m_byMaxMcsOnGoingConfNum,m_byAdminLevel,
											m_byConfNameShowType,m_byIsMMcuSpeaker);
    }    
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// Trap 信息
struct TTrapCfgInfo
{
protected:
    u32  m_dwTrapIp;							// Ip    
    u16  m_wReadWritePort;						// 读写端口
    u16  m_wSendTrapPort;						// 发Trap端口
    s8   m_szReadCom[MAXLEN_TRAP_COMMUNITY];	// 读共同体
    s8   m_szWriteCom[MAXLEN_TRAP_COMMUNITY];	// 写共同体

public:
    TTrapCfgInfo(void) 
    { 
        memset(this, 0, sizeof(TTrapCfgInfo)); 
        SetReadWritePort(DEF_READWRITE_TRAPPORT);
        SetSendTrapPort(DEF_SEND_TRAPPORT);
    }

    void    SetTrapIp(u32 dwIP) { m_dwTrapIp = htonl(dwIP); }
    u32     GetTrapIp(void) { return ntohl(m_dwTrapIp); }
    void    SetReadWritePort(u16 wPort) { m_wReadWritePort = htons(wPort); }
    u16     GetReadWritePort(void)  { return ntohs(m_wReadWritePort); }
    void    SetSendTrapPort(u16 wPort) { m_wSendTrapPort = htons(wPort); }
    u16     GetSendTrapPort(void)  { return ntohs(m_wSendTrapPort); }

    void SetReadCom(LPCSTR lpszReadCom)          
    {
        if(NULL != lpszReadCom)
        {
            strncpy(m_szReadCom, lpszReadCom, sizeof(m_szReadCom));
            m_szReadCom[sizeof(m_szReadCom) - 1] = '\0';
        }        
    }
    const s8* GetReadCom(void) { return m_szReadCom; }

    void SetWriteCom(LPCSTR lpszWriteCom)          
    {
        if(NULL != lpszWriteCom)
        {
            strncpy(m_szWriteCom, lpszWriteCom, sizeof(m_szWriteCom));
            m_szWriteCom[sizeof(m_szWriteCom) - 1] = '\0';
        }        
    }
    const s8* GetWriteCom(void) { return m_szWriteCom; }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// 网络配置信息 
struct TMcuNetCfgInfo
{	
protected:
    u32 m_dwGKIpAddr;                               // Gk 地址 (若为0表示不注册gk)
    u32 m_dwRRQMtadpIpAddr;                         // 注册gk的主协议适配板地址                      
    u32 m_dwMulticastIpAddr;                        // 组播地址
    u16 m_wuMulticastPort;                          // 组播端口
    u16 m_wRcvStartPort;                            // 接受起始端口
    u16 m_w225245StartPort;                         // 245端口
    u16 m_wMaxMtNum;                                // mcu 最大终端数
	u16 m_wMTUSize;									// MTU 大小, zgc, 2007-04-02
	u8	m_byMTUSetupMode;							// MTU设置模式, zgc, 2007-04-05
    u8  m_byUseMPCTransData;                        // 是否使用Mpc传数据
    u8  m_byUseMPCStack;                            // 是否使用Mpc内置协议栈

	u8  m_byTrapListNum;                            // trap 表长度
	TTrapCfgInfo m_tTrapInfoList[MAXNUM_TRAP_LIST]; // trap表信息
    u8  m_byGkCharge;                               // 是否进行GK计费

public:
	TMcuNetCfgInfo(void) 
    { 
        memset(this, 0, sizeof(TMcuNetCfgInfo)); 
        SetMaxMtNum(MAXNUM_H225H245_MT);
    }
    BOOL32 IsNull(void)
    {
        TMcuNetCfgInfo tInfo;
        return (0 == memcmp(this, &tInfo, sizeof(TMcuNetCfgInfo)));
    }

    void SetGkIpAddr(u32 dwIpAddr) { m_dwGKIpAddr = htonl(dwIpAddr); }
    u32  GetGkIpAddr(void) { return ntohl(m_dwGKIpAddr); }
    void SetRRQMtadpIp(u32 dwIpAddr) { m_dwRRQMtadpIpAddr = htonl(dwIpAddr); }
    u32  GetRRQMtadpIp(void) { return ntohl(m_dwRRQMtadpIpAddr); }
    void SetMultiIpAddr(u32 dwIpAddr) { m_dwMulticastIpAddr = htonl(dwIpAddr); }
    u32  GetMultiIpAddr(void) { return ntohl(m_dwMulticastIpAddr); }
    void SetMultiPort(u16 wPort) { m_wuMulticastPort = htons(wPort); }
    u16  GetMultiPort(void) { return ntohs(m_wuMulticastPort); }
    void SetRcvStartPort(u16 wPort) { m_wRcvStartPort = htons(wPort); }
    u16  GetRcvStartPort(void) { return ntohs(m_wRcvStartPort); }
    void Set225245StartPort(u16 wPort) { m_w225245StartPort = htons(wPort); }
    u16  Get225245StartPort(void) { return ntohs(m_w225245StartPort); }
    void SetMaxMtNum(u16 wNum) { m_wMaxMtNum = htons(wNum); }
    u16  GetMaxMtNum(void) { return ntohs(m_wMaxMtNum); }
	void SetMTUSize(u16 wMTUSize) { m_wMTUSize = htons(wMTUSize); } // 设置MTU大小, zgc, 2007-04-02
	u16  GetMTUSize(void) {return ntohs(m_wMTUSize); } // 获得MTU大小, zgc, 2007-04-02
	void SetMTUSetupMode( u8 byMode ) { m_byMTUSetupMode = byMode; }
	u8	 GetMTUSetupMode( void ) { return m_byMTUSetupMode; }
    void SetIsUseMpcTransData(u8 byUse) { m_byUseMPCTransData = byUse; }
    u8   GetIsUseMpcTransData(void) { return m_byUseMPCTransData; }
    void SetIsUseMpcStack(u8 byUse) { m_byUseMPCStack = byUse; }
    u8   GetIsUseMpcStack(void) { return m_byUseMPCStack; }
    void   SetIsGKCharge(BOOL32 bCharge) { m_byGkCharge = bCharge ? 1 : 0;  }
    BOOL32 GetIsGKCharge(void) { return m_byGkCharge == 1 ? TRUE : FALSE;   }

    BOOL32 SetTrapList(TTrapCfgInfo *ptTrapInfo, u8 byNum)
    {
        if(NULL == ptTrapInfo || byNum > MAXNUM_TRAP_LIST)
            return FALSE;
        m_byTrapListNum = byNum;
        memcpy((void *)m_tTrapInfoList, (void *)ptTrapInfo, byNum*sizeof(TTrapCfgInfo));
        return TRUE;
    }
    BOOL32 GetTrapList(TTrapCfgInfo *ptTrapInfo, u8 &byNum)
    {
        if(NULL == ptTrapInfo)
            return FALSE;
        byNum = m_byTrapListNum;
        memcpy((void *)ptTrapInfo, (void *)m_tTrapInfoList, byNum*sizeof(TTrapCfgInfo));
        return TRUE;
    }
    
    void Print(void)
    {
        StaticLog( "\nNetCfg:\nGKIp:0x%x, RRQMtadpIp:0x%x, MultiIp:0x%x, MultiPort:%d\nRcvPort:%d, 225Port:%d, MaxMtNum:%d, IsMpcTrans:%d, IsMpcStack:%d, MTU size:%d, MTU mode:%d \n",
                                GetGkIpAddr(), GetRRQMtadpIp(), GetMultiIpAddr(), GetMultiPort(), GetRcvStartPort(),
                                Get225245StartPort(), GetMaxMtNum(), GetIsUseMpcTransData(), GetIsUseMpcStack(), GetMTUSize(), GetMTUSetupMode() );
        StaticLog( "traplist count: %d\n", m_byTrapListNum);
        for(u8 byTrap = 0; byTrap < m_byTrapListNum; byTrap++)
        {
            StaticLog( "%d. IpAddr:0x%x, ReadCom:%s, WriteCom:%s, RWPort:%u, SendPort:%u\n", byTrap,
                                    m_tTrapInfoList[byTrap].GetTrapIp(), m_tTrapInfoList[byTrap].GetReadCom(), 
                                    m_tTrapInfoList[byTrap].GetWriteCom(), m_tTrapInfoList[byTrap].GetReadWritePort(), 
                                    m_tTrapInfoList[byTrap].GetSendTrapPort());
        }
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// Qos 信息
struct TMcuQosCfgInfo
{	
protected:
    u8  m_byQosType;					// Qos 类型
    u8  m_byAudLevel;					// 音频等级
    u8  m_byVidLevel;					// 视频等级
    u8  m_byDataLevel;					// 数据等级
    u8  m_bySignalLevel;				// 信号等级
    u8  m_byIpServiceType;				// IP服务类型
	
public:
    TMcuQosCfgInfo(void) { memset(this, 0, sizeof(TMcuQosCfgInfo)); }
    BOOL32  IsNull(void) const
    {
        TMcuQosCfgInfo tInfo;
        return (0 == memcmp(this, &tInfo, sizeof(TMcuQosCfgInfo)));
    }

    void    SetQosType(u8 byType) { m_byQosType = byType; }
    u8      GetQosType(void) const { return m_byQosType; }
    void    SetAudLevel(u8 byLevel) { m_byAudLevel = byLevel; }
    u8      GetAudLevel(void) const { return m_byAudLevel; }
    void    SetVidLevel(u8 byLevel) { m_byVidLevel = byLevel; }
    u8      GetVidLevel(void) const { return m_byVidLevel; }
    void    SetDataLevel(u8 byLevel) { m_byDataLevel = byLevel; }
    u8      GetDataLevel(void) const { return m_byDataLevel; }
    void    SetSignalLevel(u8 byLevel) { m_bySignalLevel = byLevel; }
    u8      GetSignalLevel(void) const { return m_bySignalLevel; }
    void    SetIpServiceType(u8 byType) { m_byIpServiceType = byType; }
    u8      GetIpServiceType(void) const { return m_byIpServiceType; }

    void    Print(void) const
    {
        StaticLog( "QosCfg:  QosType:%d, Aud:%d, Vid:%d, Data:%d, Signal:%d, IpServciType:%d\n",
            GetQosType(), GetAudLevel(), GetVidLevel(), GetDataLevel(), 
            GetSignalLevel(), GetIpServiceType());
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// 网同步信息
struct TMcuNetSynCfgInfo
{	
protected:
    u8	m_byNetSynMode;			    	//网同步模式
    u8	m_byNetSynDTSlot;				//网同步使用的DT板号 (0-14)
    u8	m_byNetSynE1Index;				//网同步DT板上的E1号 (0-7)
	
public:
    TMcuNetSynCfgInfo(void)   { memset(this, 0, sizeof(TMcuNetSynCfgInfo)); }
    BOOL32  IsNull(void)
    {
        TMcuNetSynCfgInfo tInfo;
        return (0 == memcmp(this, &tInfo, sizeof(TMcuNetSynCfgInfo)));
    }

    void    SetNetSynMode(u8 byMode) { m_byNetSynMode = byMode; }
    u8      GetNetSynMode(void) { return m_byNetSynMode; }
    void    SetNetSynDTSlot(u8 bySlot) { m_byNetSynDTSlot = bySlot; }
    u8      GetNetSynDTSlot(void) { return m_byNetSynDTSlot; }
    void    SetNetSynE1Index(u8 byIndex) { m_byNetSynE1Index = byIndex; }
    u8      GetNetSynE1Index(void) { return m_byNetSynE1Index; }

    void    Print(void)
    {
        StaticLog( "\nNetSynCfg:\nSynMode:%d, SynDTSlot:%d, SynE1Index:%d\n",
            GetNetSynMode(), GetNetSynDTSlot(), GetNetSynE1Index());
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//数字会议服务器配置信息
struct TDCSCfgInfo
{
protected:
	u32	m_dwDcsEqpIp;					//数字会议服务器设备id

public:
    TDCSCfgInfo(void) : m_dwDcsEqpIp(0) {}
    BOOL32  IsNull(void)
    {
        TDCSCfgInfo tInfo;
        return (0 == memcmp(this, &tInfo, sizeof(TDCSCfgInfo)));
    }

    void    SetDcsEqpIp(u32 dwEqpIp) { m_dwDcsEqpIp = htonl(dwEqpIp); }
    u32      GetDcsEqpIp(void) { return ntohl(m_dwDcsEqpIp); }
    
    void    Print(void)
    {
        StaticLog( "\nDcsEqpIp:0x%x\n", GetDcsEqpIp());
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TDscUpdateReqHead
{
	TDscUpdateReqHead()
	{
		memset( this, 0, sizeof(TDscUpdateReqHead) );
	}
	~TDscUpdateReqHead(){;}
public:
	s8 m_acFileName[MAXLEN_MCU_FILEPATH];	//升级文件名
	u8  m_byMark;			//最后一帧标记，0-不是最后一帧，1-最后一帧
private:
	u32 m_dwFileSize;		//文件总长度
	u32 m_dwFrmNum;			//文件总帧数
	u32 m_dwReqFrmSN;		//当前的升级帧号
	u32 m_dwFrmSize;		//当前的数据包长度
public:
	void SetFileSize(u32 dwFileSize) { m_dwFileSize = htonl(dwFileSize); }
	u32  GetFileSize(void) { return ntohl(m_dwFileSize); }
	void SetFrmNum(u32 dwFrmNum) { m_dwFrmNum = htonl(dwFrmNum); }
	u32  GetFrmNum(void) { return ntohl(m_dwFrmNum); }
	void SetReqFrmSN(u32 dwReqFrmSN) { m_dwReqFrmSN = htonl(dwReqFrmSN); } 
	u32  GetReqFrmSN(void) { return ntohl(m_dwReqFrmSN); }
	void SetFrmSize(u32 dwFrmSize) { m_dwFrmSize = htonl(dwFrmSize); }
	u32  GetFrmSize(void) { return ntohl(m_dwFrmSize); }

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TDscUpdateRsp
{
	TDscUpdateRsp()
	{
		memset( this, 0, sizeof(TDscUpdateRsp) );
	}
	~TDscUpdateRsp(){;}
public:
	s8 m_acFileName[MAXLEN_MCU_FILEPATH];	//升级文件名

private:
	u32 m_dwRspFrmSN;		//请求的下一帧帧号

public:
	void SetRspFrmSN(u32 dwRspFrmSN) { m_dwRspFrmSN = htonl(dwRspFrmSN); }
	u32  GetRspFrmSN(void) { return ntohl(m_dwRspFrmSN); }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;



struct TMINIMCUNetParam
{
public:
    enum enNetworkType
    {
        enInvalid = 0,
            enWAN = 1,
            enLAN = 2
    };
protected:

	u32 m_dwIpAddr;		// 网络序
	u32 m_dwIpMask;		// 网络序
	u32 m_dwGatewayIp;  // 网络序
	s8  m_asServerAlias[MAXLEN_PWD]; // 运营商别名
    u8  m_byNetwordType;    // 网络地址类型，LAN or WAN    
public:
	TMINIMCUNetParam():m_dwIpAddr(0),
					   m_dwIpMask(0),
					   m_dwGatewayIp(0),
                       m_byNetwordType(enInvalid)
	{
		memset(	m_asServerAlias, 0, sizeof(m_asServerAlias) );
	}

public:
    // 设置为公网地址
    void SetWanParam( u32 dwIp, u32 dwIpMask, u32 dwGW, LPCSTR lpszAlias )
    {
        m_byNetwordType = enWAN;
        m_dwIpAddr = htonl(dwIp);
        m_dwIpMask = htonl(dwIpMask); 
        m_dwGatewayIp = htonl(dwGW);

        if (NULL != lpszAlias)
        {
            strncpy(m_asServerAlias, lpszAlias, MAXLEN_PWD-1);
            m_asServerAlias[MAXLEN_PWD-1] = 0;
        }
        else
        {
            memset(	m_asServerAlias, 0, sizeof(m_asServerAlias) );
        }
    }

    // 设置为局域网地址
    void SetLanParam( u32 dwIp, u32 dwMask, u32 dwGW )
    {
        m_byNetwordType = enLAN;
        m_dwIpAddr = htonl(dwIp);
        m_dwIpMask = htonl(dwMask);
        m_dwGatewayIp = htonl(dwGW);

        // 局域网不需要别名
        memset(	m_asServerAlias, 0, sizeof(m_asServerAlias) );
    }

    BOOL32 IsValid() const
    {
        if ( 0 == m_dwIpAddr || enInvalid == m_byNetwordType )
        {
            return FALSE;
        }
        return TRUE;
    }

    // 获取网络类型
    u8   GetNetworkType() const
    {
        return m_byNetwordType;
    }

    BOOL IsWan() const
    {
        return (m_byNetwordType == enWAN);
    }
    BOOL IsLan() const
    {
        return (m_byNetwordType == enLAN);
    }

    // 获取地址参数
    u32  GetIpAddr(void) const { return ntohl(m_dwIpAddr); }
	u32  GetIpMask(void) const{ return ntohl(m_dwIpMask); }
    u32  GetGatewayIp(void) const{ return ntohl(m_dwGatewayIp); }

    // 获取别名
    const s8 * GetServerAlias(void) const{ return m_asServerAlias; }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMINIMCUNetParamAll
{
private:
	u8 m_byParamNum;
	TMINIMCUNetParam m_atMINIMCUNetParam[ETH_IP_MAXNUM];

public:
	TMINIMCUNetParamAll()
	{
		m_byParamNum = 0;
		memset( m_atMINIMCUNetParam, 0, sizeof(m_atMINIMCUNetParam) );
	}
	BOOL32 IsValid() const
	{
		return ( m_byParamNum != 0 );
	}
	u8 GetNetParamNum(void) const
	{
		return m_byParamNum;
	}
	u8 GetLANParamNum(void) const
	{
        u8 byNum = 0;
		for ( u8 byLop = 0; byLop < m_byParamNum; byLop++ )
		{
			if ( m_atMINIMCUNetParam[byLop].IsLan() )
			{
				byNum++;
			}
		}
		return byNum;
	}
	BOOL32 AddOneNetParam( const TMINIMCUNetParam &tNetParam )
	{
		if ( !tNetParam.IsValid() )
		{
			return FALSE;
		}
		u8 byIdx = 0;
		u8 byArrayLen = sizeof(m_atMINIMCUNetParam)/sizeof(TMINIMCUNetParam);
		for ( byIdx = 0; byIdx < byArrayLen; byIdx++ )
		{
			if( !m_atMINIMCUNetParam[byIdx].IsValid() )
			{
				memcpy( m_atMINIMCUNetParam+byIdx, &tNetParam, sizeof(TMINIMCUNetParam) );
				break;
			}
		}
		if ( byIdx == byArrayLen )
		{
			return FALSE;
		}
		m_byParamNum ++;
		return TRUE;
	}
	BOOL32 GetNetParambyIdx ( u8 byIdx, TMINIMCUNetParam &tNetParam ) const
	{
		memset( &tNetParam, 0, sizeof(TMINIMCUNetParam) );
		if ( byIdx >= m_byParamNum )
		{
			return FALSE;
		}
		if ( !m_atMINIMCUNetParam[byIdx].IsValid() )
		{
			return FALSE;
		}
		memcpy( &tNetParam, &m_atMINIMCUNetParam[byIdx], sizeof(TMINIMCUNetParam) );
		return TRUE;
	}
	BOOL32 DelNetParamByIdx(u8 byIdx)
	{
        if (m_byParamNum == 0)
        {
            return FALSE;
        }
		if ( byIdx >= m_byParamNum )
		{
			return FALSE;
		}
		u8 byLop = 0;
		for ( byLop = byIdx+1; byLop < m_byParamNum; byLop++ )
		{
			memcpy( &m_atMINIMCUNetParam[byLop-1], &m_atMINIMCUNetParam[byLop], sizeof(TMINIMCUNetParam) );
		}
		memset( &m_atMINIMCUNetParam[byLop-1], 0, sizeof(TMINIMCUNetParam) );
		m_byParamNum--;
		return TRUE;
	}
	BOOL32 IsEqualTo( const TMINIMCUNetParamAll &tMINIMCUNetParamAll )
	{
		return ( 0 == memcmp( &tMINIMCUNetParamAll, this, sizeof(TMINIMCUNetParamAll) ) ) ? TRUE : FALSE;
	}
	void Clear(void)
	{
		memset( this, 0, sizeof(TMINIMCUNetParamAll) );
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//MCU8000B 配置DSC运行模块
struct T8000BDscMod
{
protected:
    u32 m_dwMcuInnerIp;          // DSC 要连接的MCU内部IP 
    u32 m_dwDscInnerIp;		     // DSC 的内部IP
    u32 m_dwInnerIpMask;         // DSC 内部IP掩码
    u16 m_wMcuListenPort;		 // DSC 要连接的MCU端口,一般是60000
	
	u8  m_byNetType;			 // MCU所在的网络类型
	u8  m_byLanMtProxyIpPos;	 // 内网终端代理服务器IP的索引位置 
	TMINIMCUNetParamAll m_tDscParamAll;	// 呼叫地址（即DSC侧IP）
	TMINIMCUNetParamAll m_tMcuParamAll;	// 会控连接地址（即MCU侧0口IP）
	
    u8 m_byStartMp;
    u8 m_byStartMtAdp;
    u8 m_byStartGk;
    u8 m_byStartProxy;
    u8 m_byStartDcs;

public:
    T8000BDscMod():m_byStartMp(0),
                     m_byStartMtAdp(0),
                     m_byStartGk(0),
                     m_byStartProxy(0),
                     m_byStartDcs(0),
                     m_wMcuListenPort(MCU_LISTEN_PORT),
                     m_dwMcuInnerIp(0),
                     m_dwDscInnerIp(0),
                     m_dwInnerIpMask(0),
					 m_byNetType(NETTYPE_INVALID),
					 m_byLanMtProxyIpPos(0)
    {
		memset( &m_tDscParamAll, 0, sizeof(m_tDscParamAll) );
		memset( &m_tMcuParamAll, 0, sizeof(m_tMcuParamAll) );
	}
    
    void   SetMcuInnerIp(u32 dwIp) { m_dwMcuInnerIp = htonl(dwIp); }
    u32    GetMcuInnerIp() const { return ntohl(m_dwMcuInnerIp); }
    void   SetMcuPort(u16 wPort) { m_wMcuListenPort = htons(wPort); }
    u16    GetMcuPort() const { return ntohs(m_wMcuListenPort); }
    void   SetDscInnerIp(u32 dwIp) { m_dwDscInnerIp = htonl(dwIp); }
    u32    GetDscInnerIp() const { return ntohl(m_dwDscInnerIp); }
    void   SetInnerIpMask(u32 dwMask) { m_dwInnerIpMask = htonl(dwMask); }
    u32    GetInnerIpMask() const {return ntohl(m_dwInnerIpMask); }
	void   SetNetType(u8 byNetType) { m_byNetType = byNetType; }
	u8	   GetNetType(void) const { return m_byNetType; }
	void   SetLanMtProxyIpPos(u8 byLanMtProxyIpPos) { m_byLanMtProxyIpPos = byLanMtProxyIpPos; }
	u8	   GetLanMtProxyIpPos(void) const { return m_byLanMtProxyIpPos; }

	void SetCallAddrAll( TMINIMCUNetParamAll &tDscParamAll)
	{
		if ( tDscParamAll.IsValid() )
		{
			memcpy( &m_tDscParamAll, &tDscParamAll, sizeof(TMINIMCUNetParamAll) );
		}
	}
	void SetMcsAccessAddrAll( TMINIMCUNetParamAll &tMcuParamAll)
	{
		if ( tMcuParamAll.IsValid() )
		{
			memcpy( &m_tMcuParamAll, &tMcuParamAll, sizeof(TMINIMCUNetParamAll) );
		}
	}
	void GetCallAddrAll(TMINIMCUNetParamAll &tDscParamAll) const 
	{ 
		memcpy( &tDscParamAll, &m_tDscParamAll, sizeof(TMINIMCUNetParamAll) ); 
	}
	void GetMcsAccessAddrAll(TMINIMCUNetParamAll &tMcuParamAll) const 
	{ 
		memcpy( &tMcuParamAll, &m_tMcuParamAll, sizeof(TMINIMCUNetParamAll) ); 
	}
	u8 GetCallAddrNum(void) const 
    { 
        return m_tDscParamAll.GetNetParamNum(); 
    }
	u8 GetMcsAccessAddrNum(void) const 
    { 
        return m_tMcuParamAll.GetNetParamNum(); 
    }
    /*
    u8 GetLanCallAddrNum(void) const
    {
        return m_tDscParamAll.GetLANParamNum();
	}
	u8 GetLanMcsAccessAddrNum(void) const
	{
        return m_tMcuParamAll.GetLANParamNum();
	}
    */
   
    void   SetStartMp(BOOL32 bStart){ m_byStartMp = bStart ? 1 : 0;    }
    BOOL32 IsStartMp() const { return m_byStartMp == 1 ? TRUE : FALSE;    }
    void   SetStartMtAdp(BOOL32 bStart){ m_byStartMtAdp = bStart ? 1 : 0;    }
    BOOL32 IsStartMtAdp() const { return m_byStartMtAdp == 1 ? TRUE : FALSE;    }
    void   SetStartGk(BOOL32 bStart) { m_byStartGk = bStart ? 1 : 0; }
    BOOL32 IsStartGk() const { return m_byStartGk == 1 ? TRUE : FALSE; }
    void   SetStartProxy(BOOL32 bStart) { m_byStartProxy = bStart ? 1 : 0; }
    BOOL32 IsStartProxy() const { return m_byStartProxy == 1 ? TRUE : FALSE; }
    void   SetStartDcs(BOOL32 bStart) { m_byStartDcs = bStart ? 1 : 0; }
    BOOL32 IsStartDcs() const { return m_byStartDcs == 1 ? TRUE : FALSE; }

    void   Print() const
    {
        u8 byLop = 0;
        StaticLog( "T8000BDscMod: DscInnerIp: 0x%x, McuInnerIp: 0x%x:%d, InnerMask: 0x%x. Module: Mp.%d, MtAdp.%d, GK.%d, Proxy.%d, Dcs.%d\n", 
            m_dwDscInnerIp, m_dwMcuInnerIp, m_wMcuListenPort, m_dwInnerIpMask,
            m_byStartMp, m_byStartMtAdp, m_byStartGk, m_byStartProxy, m_byStartDcs);
		StaticLog( "Net Type: ");
		switch( m_byNetType )
		{
		case NETTYPE_INVALID:
			StaticLog( "INVALID\n");
			break;
		case NETTYPE_LAN:
			StaticLog( "LAN\n");
			break;
		case NETTYPE_WAN:
			StaticLog( "WAN\n");
			break;
		case NETTYPE_MIXING_ALLPROXY:
			printf( "MIXING_ALLPROXY\n");
			break;
		case NETTYPE_MIXING_NOTALLPROXY:
			printf( "MIXING_NOTALLPROXY\n" );
			break;
		default:
			break;
		}
		StaticLog( "LanMtProxyIpPos: %d\n", m_byLanMtProxyIpPos );
		StaticLog( "T8000BDscMod: DSC NetParam--\n");
		TMINIMCUNetParam tTemp;
		for ( byLop = 0; byLop < m_tDscParamAll.GetNetParamNum(); byLop++ )
		{
			if ( m_tDscParamAll.GetNetParambyIdx(byLop, tTemp) )
			{
				if ( tTemp.IsValid() )
				{
					StaticLog( "NetType:%s, Ip: 0x%x\tIpMask: 0x%x\tGateway: 0x%x\tServerAlias: %s\n",
                        tTemp.IsLan() ? "LAN" : "WAN",
						tTemp.GetIpAddr(), tTemp.GetIpMask(),
						tTemp.GetGatewayIp(), tTemp.GetServerAlias() );
				}
			}
		}
		StaticLog( "T8000BDscMod: MCU NetParam--\n");
		for ( byLop = 0; byLop < ETH_IP_MAXNUM; byLop++ )
		{
			if ( m_tMcuParamAll.GetNetParambyIdx(byLop, tTemp) )
			{
				if ( tTemp.IsValid() )
				{
					StaticLog( "NetType:%s, Ip: 0x%x\tIpMask: 0x%x\tGateway: 0x%x\tServerAlias: %s\n",
                        tTemp.IsLan() ? "LAN" : "WAN",
						tTemp.GetIpAddr(), tTemp.GetIpMask(),
						tTemp.GetGatewayIp(), tTemp.GetServerAlias() );
				}
			}
		}
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//Log info
struct TLoginInfo
{
public:
	s8  m_achUser[MAXLEN_PWD+1];//用户名
	s8  m_achPwd[MAXLEN_PWD+1]; //用户密码

public:
	TLoginInfo() { memset( this, 0, sizeof(TLoginInfo) ); }

	void  SetUser( LPCSTR lpszUser )
	{
		memset(m_achUser, 0, sizeof(m_achUser));
		if (NULL != lpszUser)
		{
			strncpy(m_achUser, lpszUser, sizeof(m_achUser));
			m_achUser[sizeof(m_achUser)-1] = '\0';
		}
	}

	LPCSTR GetUser( void ) const{ return m_achUser; }

	BOOL IsEqualUser( LPCSTR lpszUser ) const
	{
		s8 achBuf[MAXLEN_PWD+1] = {0};
		strncpy( achBuf, lpszUser, sizeof( achBuf ) );
		achBuf[sizeof(achBuf)-1] = '\0';
		
		if(0 == strncmp(achBuf, lpszUser, MAXLEN_PWD))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	void SetPwd( LPCSTR lpszPwd )
	{
		memset(m_achPwd, 0, sizeof(m_achPwd));
		if (NULL != lpszPwd)
		{
			strncpy(m_achPwd, lpszPwd, sizeof(m_achPwd));
			m_achPwd[sizeof(m_achPwd)-1] = '\0';
            // 简单加密。在GetPwd之前要手动解密
            EncPwd();
		}
	}

	BOOL32 GetPwd( s8* pchPwd, u8 byLength )
	{
		if ( byLength < strlen(m_achPwd)+1 )
		{
			return FALSE;
		}
		DecPwd();
		strncpy( pchPwd, m_achPwd, byLength );
		pchPwd[byLength-1] = '\0';
		EncPwd();
		return TRUE; 
	}
        
    void DecPwd()
    {
        EncPwd();
    }

	void Print(void)
	{
		StaticLog( "Username = %s\n", GetUser() );
		s8 achPwd[MAXLEN_PWD+1];
		memset(achPwd, 0, sizeof(achPwd));
		GetPwd(achPwd, sizeof(achPwd));
		StaticLog( "Password = %s\n", achPwd );
	}

private:
    void EncPwd()
    {
        for (u8 byLoop=0; byLoop<strlen(m_achPwd); byLoop++)
        {
            m_achPwd[byLoop] ^= 0x5a;
        }
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//  [1/8/2010 pengjie] Modify 
// HDU扩展配置
struct THDUExCfgInfo
{
	u8  m_byIdleChlShowMode;  // 空闲通道显示模式 HDU_SHOW_GRAY_MODE ...
	
	u8  m_byReserved; // 保留字段
	u16 m_wReserved;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


// MCU侧保存的外设扩展配置
struct TEqpExCfgInfo
{
public:
	THDUExCfgInfo m_tHDUExCfgInfo;
	TVMPExCfgInfo m_tVMPExCfgInfo;
	
public:
	TEqpExCfgInfo() { Init(); }
	void Init( void ) { memset(this, 0, sizeof(TEqpExCfgInfo)); }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
// End Modify

//mcu general config info
struct TMcuGeneralCfg
{
    u8                  m_byVmpStyleNum;
    TVmpStyleCfgInfo    m_atVmpStyle[MAX_VMPSTYLE_NUM];
    TMcuLocalCfgInfo    m_tLocal;
    TMcuNetCfgInfo      m_tNet;
    TMcuQosCfgInfo      m_tQos;
    TMcuNetSynCfgInfo   m_tNetSyn;
    TDCSCfgInfo         m_tDcs;
    T8000BDscMod        m_tDscInfo;
	TLoginInfo			m_tLoginInfo;
	TEqpExCfgInfo       m_tEqpExCfgInfo;
	// [3/16/2010 xliang] add for 8000e 
	TMultiNetCfgInfo	m_tMultiNetCfg;
	TGKProxyCfgInfo		m_tGkProxyCfg;
	TPrsTimeSpan		m_tPrsTimeSpanCfg;
	TMultiManuNetAccess m_tMultiManuNetAccess;


public:
    void Print(void)
    {
        StaticLog( "mcu generalcfg info:\n");
        for(u8 byIndex = 0; byIndex < m_byVmpStyleNum; byIndex++)
        {
            m_atVmpStyle[byIndex].Print();
        }        
        m_tLocal.Print();
        m_tNet.Print();
        m_tQos.Print();
        m_tNetSyn.Print();
        m_tDcs.Print();
        m_tDscInfo.Print();
		m_tLoginInfo.Print();
		m_tMultiNetCfg.Print();
		m_tGkProxyCfg.Print();
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//地址簿数据结构定义
//size: 143 byte, 若增加此结构的内容注意更新每包消息的条目个数(ADDRENTRYNUM_PERPKT)
struct TMcuAddrEntry
{
protected:
    u32     m_dwEntryIdx;                                   // 条目索引
    s8	    m_szEntryName[MAXLEN_ADDR_ENTRYNAME+1];		    // 条目名字
    s8	    m_szMtAlias[MAXLEN_ADDR_323ALIAS+1];	        // 呼叫别名
    u32	    m_dwMtIpAddr;							        // IP地址(网络序)
    s8	    m_szMtE164[MAXLEN_ADDR_E164NUM+1];	    	    // 呼叫号码
    u8      m_byMtProtocolType;                             // 0:H323终端; 非0:H320终端
    s8      m_szH320Id[MAXLEN_ADDR_320ID+1];                // H320ID
    u16	    m_wCallRate;							        // 呼叫速率

public:
    TMcuAddrEntry( void ) { memset(this, 0, sizeof(TMcuAddrEntry)); }
    
    void SetEntryIdx(u32 dwIndex)  { m_dwEntryIdx = htonl(dwIndex); } 
    u32  GetEntryIdx( void ) const { return ntohl(m_dwEntryIdx); }

    void SetEntryName(LPCSTR lpszEntryName)          
    {
        if(NULL != lpszEntryName)
        {
            strncpy(m_szEntryName, lpszEntryName, sizeof(m_szEntryName));
            m_szEntryName[sizeof(m_szEntryName) - 1] = '\0';
        }        
    }
    LPCSTR GetEntryName(void) const { return m_szEntryName; }

    void SetMtAlias(LPCSTR lpszMtAlias)          
    {
        if(NULL != lpszMtAlias)
        {
            strncpy(m_szMtAlias, lpszMtAlias, sizeof(m_szMtAlias));
            m_szMtAlias[sizeof(m_szMtAlias) - 1] = '\0';
        }        
    }
    LPCSTR GetMtAlias(void) const { return m_szMtAlias; }

    void SetMtIpAddr(u32 dwIp) { m_dwMtIpAddr = htonl(dwIp); }
    u32  GetMtIpAddr( void ) const { return ntohl(m_dwMtIpAddr); }

    void SetMtE164(LPCSTR lpszMtE164)
    {
        if(NULL != lpszMtE164)
        {
            strncpy(m_szMtE164, lpszMtE164, sizeof(m_szMtE164));
            m_szMtE164[sizeof(m_szMtE164) - 1] = '\0';
        }
    }
    LPCSTR GetMtE164( void ) const { return m_szMtE164; }

    void SetMtProtocolType(u8 byType) { m_byMtProtocolType = byType; }
    u8  GetMtProtocolType( void ) const { return m_byMtProtocolType; }

    void SetH320Id(LPCSTR lpszH320Id)
    {
        if(NULL != lpszH320Id)
        {
            strncpy(m_szH320Id, lpszH320Id, sizeof(m_szH320Id));
            m_szH320Id[sizeof(m_szH320Id) - 1] = '\0';
        }
    }
    LPCSTR GetH320Id( void ) const { return m_szH320Id; }

    void SetH320Alias( u8 byLayer, u8 bySlot, u8 byChannel )
    {
        m_byMtProtocolType = 1;
        sprintf( m_szH320Id, "H.320 MT, L:%d, S:%d, Ch:%d%c", byLayer, bySlot, byChannel, 0 );
    }
    BOOL GetH320Alias ( u8 &byLayer, u8 &bySlot, u8 &byChannel ) const
    {
        BOOL bRet = FALSE;
        byLayer   = 0;
        bySlot    = 0; 
        byChannel = 0; 
        if( 1 != m_byMtProtocolType )
        {
            return bRet;
        }
        
        const s8 *pachAlias = m_szH320Id;
        u8 byIndex = 0;
        for( u16 wLoop = 0; wLoop < strlen(pachAlias); wLoop++ )
        {
            if ( pachAlias[wLoop] == ':' )
            {
                s32  nPos = 0;
                s32  nMaxCHLen = 3; //一字节的最大字串长度
                s8   achLayer[4] = {0};
                const s8 * pachAliasTemp = &pachAlias[wLoop + 1];
                if ( 0 == byIndex )
                {//层号

                    for( nPos = 0; nPos < nMaxCHLen; nPos++ )
                    {
                        if( ',' == pachAliasTemp[nPos] )
                        {
                            break;
                        }
                        achLayer[nPos] = pachAliasTemp[nPos];
                    }

                    if( 0 == nPos || nPos > nMaxCHLen )
                    {
                        return bRet;
                    }
                    achLayer[nPos+1] = '\0';
                    byLayer = atoi(achLayer);
                    byIndex++;  
                }
                else if( 1 == byIndex )
                {//槽号

                    for( nPos = 0; nPos < nMaxCHLen; nPos++ )
                    {
                        if( ',' == pachAliasTemp[nPos] )
                        {
                            break;
                        }
                        achLayer[nPos] = pachAliasTemp[nPos];
                    }
                    
                    if( 0 == nPos || nPos > nMaxCHLen )
                    {
                        return bRet;
                    }
                    achLayer[nPos+1] = '\0';
                    bySlot = atoi(achLayer);
                    byIndex++;

                }
                else if( 2 == byIndex)
                {//通道号
                    for( nPos = 0; nPos < nMaxCHLen; nPos++ )
                    {
                        achLayer[nPos] = pachAliasTemp[nPos];
                    }
                    
                    if( 0 == nPos || nPos > nMaxCHLen )
                    {
                        return bRet;
                    }
                    achLayer[nPos+1] = '\0';
                    byChannel = atoi(achLayer);
                    break;
                }

                continue;               
            }
        }

        return bRet;
	}

    void SetCallRate(u16 wCallRate) { m_wCallRate = htons(wCallRate); }
    u16  GetCallRate( void ) const { return ntohs(m_wCallRate); }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//size: 809 byte, 若增加此结构的内容注意更新每包消息的条目组个数(ADDRGROUPNUM_PERPKT)
struct TMcuAddrGroup
{
protected:
    u32 m_dwEntryIdx;								// 条目索引
    u32 m_dwEntryNum;							    // 与会终端个数
    u32 m_adwEntryIdx[MAXNUM_CONF_MT];	            // 与会终端索引
    s8  m_szGroupName[MAXLEN_ADDR_ENTRYNAME + 1];	// 组名     

public:
    TMcuAddrGroup( void ) { memset(this, 0, sizeof(TMcuAddrGroup)); }

    void SetEntryIdx(u32 dwIndex) { m_dwEntryIdx = htonl(dwIndex); } 
    u32  GetEntryIdx( void ) const { return ntohl(m_dwEntryIdx); }

    void SetEntryNum(u32 dwEntryNum) { m_dwEntryNum = htonl(dwEntryNum); }
    u32  GetEntryNum( void )const { return ntohl(m_dwEntryNum); }

    void GetAllEntryIdx(u32 *padwEntryIdx, u32 &dwNum) const
    {
        if(NULL == padwEntryIdx) return;

        if(dwNum > GetEntryNum()) 
        {
            dwNum = GetEntryNum();
        }
        
        if(dwNum > MAXNUM_CONF_MT)
        {
            dwNum = MAXNUM_CONF_MT;
        }
        
        for(u32 dwIdx=0; dwIdx < dwNum; dwIdx++)
        {
            padwEntryIdx[dwIdx] = ntohl(m_adwEntryIdx[dwIdx]);
        }        
    }
    void SetAllEntryIdx( const u32 *padwEntryIdx, u32 dwNum)
    {
        if(NULL == padwEntryIdx) return;

        if(dwNum > MAXNUM_CONF_MT)
        {
            dwNum = MAXNUM_CONF_MT;
        }

        for(u32 dwIdx=0; dwIdx < dwNum; dwIdx++)
        {
            m_adwEntryIdx[dwIdx] = htonl(padwEntryIdx[dwIdx]);
        }

        m_dwEntryNum = htonl(dwNum);
    }
    
    void SetGroupName(LPCSTR lpszName)
    {
        if(NULL != lpszName)
        {
            strncpy(m_szGroupName, lpszName, sizeof(m_szGroupName));
            m_szGroupName[sizeof(m_szGroupName)-1] = '\0';
        }
    }
    LPCSTR GetGroupName( void ) const{ return m_szGroupName; }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMcuAddrEntryV1
{
protected:
    u32     m_dwEntryIdx;                                   // 条目索引
    s8	    m_szEntryName[MAXLEN_ADDR_ENTRYNAME_V1 + 1];		    // 条目名字
    s8	    m_szMtAlias[MAXLEN_ADDR_323ALIAS_V1 + 1];	        // 呼叫别名
    u32	    m_dwMtIpAddr;							        // IP地址(网络序)
    s8	    m_szMtE164[MAXLEN_ADDR_E164NUM+1];	    	    // 呼叫号码
    u8      m_byMtProtocolType;                             // 0:H323终端; 非0:H320终端
    s8      m_szH320Id[MAXLEN_ADDR_320ID+1];                // H320ID
    u16	    m_wCallRate;							        // 呼叫速率

public:
    TMcuAddrEntryV1( void ) 
	{ 
		memset(this, 0, sizeof(TMcuAddrEntryV1));
	}

	u8 GetMemberNum(void) const
	{
		return 8;
	}

	u16 GetMemberLen(u8 byMemberId) const
	{
		switch (byMemberId)
		{
		case 0:
			return sizeof(m_dwEntryIdx);
		case 1:
			return sizeof(m_szEntryName);
		case 2:
			return sizeof(m_szMtAlias);
		case 3:
			return sizeof(m_dwMtIpAddr);
		case 4:
			return sizeof(m_szMtE164);
		case 5:
			return sizeof(m_byMtProtocolType);
		case 6:
			return sizeof(m_szH320Id);
		case 7:
			return sizeof(m_wCallRate);
		default:
			return 0;
		}
	}

	void GetTMcuAddrEntry( TMcuAddrEntry& tMcuaddr ) const
	{
		tMcuaddr.SetEntryIdx(GetEntryIdx());
		tMcuaddr.SetEntryName(GetEntryName());
		tMcuaddr.SetMtAlias(GetMtAlias());
		tMcuaddr.SetMtIpAddr(GetMtIpAddr());
		tMcuaddr.SetMtE164(GetMtE164());
		tMcuaddr.SetMtProtocolType(GetMtProtocolType());
		tMcuaddr.SetH320Id(GetH320Id());
		tMcuaddr.SetCallRate(GetCallRate());
	}

	TMcuAddrEntryV1& operator=(const TMcuAddrEntry& tAddrEntry)
	{
		SetEntryIdx(tAddrEntry.GetEntryIdx());
		SetEntryName(tAddrEntry.GetEntryName());
		SetMtAlias(tAddrEntry.GetMtAlias());
		SetMtIpAddr(tAddrEntry.GetMtIpAddr());
		SetMtE164(tAddrEntry.GetMtE164());
		SetMtProtocolType(tAddrEntry.GetMtProtocolType());
		SetH320Id(tAddrEntry.GetH320Id());
		SetCallRate(tAddrEntry.GetCallRate());	
		return *this;
	}
    
    void SetEntryIdx(u32 dwIndex) { m_dwEntryIdx = htonl(dwIndex); } 
    u32  GetEntryIdx( void )const { return ntohl(m_dwEntryIdx); }

    void SetEntryName(LPCSTR lpszEntryName)
    {
        if(NULL != lpszEntryName)
        {
            strncpy(m_szEntryName, lpszEntryName, sizeof(m_szEntryName));
            m_szEntryName[sizeof(m_szEntryName) - 1] = '\0';
        }        
    }
    LPCSTR GetEntryName(void) const { return m_szEntryName; }

    void SetMtAlias(LPCSTR lpszMtAlias)          
    {
        if(NULL != lpszMtAlias)
        {
            strncpy(m_szMtAlias, lpszMtAlias, sizeof(m_szMtAlias));
            m_szMtAlias[sizeof(m_szMtAlias) - 1] = '\0';
        }        
    }
    LPCSTR GetMtAlias(void) const { return m_szMtAlias; }

    void SetMtIpAddr(u32 dwIp) { m_dwMtIpAddr = htonl(dwIp); }
    u32  GetMtIpAddr( void ) const { return ntohl(m_dwMtIpAddr); }

    void SetMtE164(LPCSTR lpszMtE164)
    {
        if(NULL != lpszMtE164)
        {
            strncpy(m_szMtE164, lpszMtE164, sizeof(m_szMtE164));
            m_szMtE164[sizeof(m_szMtE164) - 1] = '\0';
        }
    }
    LPCSTR GetMtE164( void ) const { return m_szMtE164; }

    void SetMtProtocolType(u8 byType) { m_byMtProtocolType = byType; }
    u8  GetMtProtocolType( void ) const { return m_byMtProtocolType; }

    void SetH320Id(LPCSTR lpszH320Id)
    {
        if(NULL != lpszH320Id)
        {
            strncpy(m_szH320Id, lpszH320Id, sizeof(m_szH320Id));
            m_szH320Id[sizeof(m_szH320Id) - 1] = '\0';
        }
    }
    LPCSTR GetH320Id( void ) const { return m_szH320Id; }

    void SetH320Alias( u8 byLayer, u8 bySlot, u8 byChannel )
    {
        m_byMtProtocolType = 1;
        sprintf( m_szH320Id, "H.320 MT, L:%d, S:%d, Ch:%d%c", byLayer, bySlot, byChannel, 0);
    }
    BOOL GetH320Alias ( u8 &byLayer, u8 &bySlot, u8 &byChannel ) const
    {
        BOOL bRet = FALSE;
        byLayer   = 0;
        bySlot    = 0; 
        byChannel = 0; 
        if( 1 != m_byMtProtocolType )
        {
            return bRet;
        }
        
        const s8 *pachAlias = m_szH320Id;
        u8 byIndex = 0;
        for( u16 wLoop = 0; wLoop < strlen(pachAlias); wLoop++ )
        {
            if ( pachAlias[wLoop] == ':' )
            {
                s32  nPos = 0;
                s32  nMaxCHLen = 3; //一字节的最大字串长度
                s8   achLayer[4] = {0};
                const s8 * pachAliasTemp = &pachAlias[wLoop + 1];
                if ( 0 == byIndex )
                {//层号

                    for( nPos = 0; nPos < nMaxCHLen; nPos++ )
                    {
                        if( ',' == pachAliasTemp[nPos] )
                        {
                            break;
                        }
                        achLayer[nPos] = pachAliasTemp[nPos];
                    }

                    if( 0 == nPos || nPos > nMaxCHLen )
                    {
                        return bRet;
                    }
                    achLayer[nPos+1] = '\0';
                    byLayer = atoi(achLayer);
                    byIndex++;  
                }
                else if( 1 == byIndex )
                {//槽号

                    for( nPos = 0; nPos < nMaxCHLen; nPos++ )
                    {
                        if( ',' == pachAliasTemp[nPos] )
                        {
                            break;
                        }
                        achLayer[nPos] = pachAliasTemp[nPos];
                    }
                    
                    if( 0 == nPos || nPos > nMaxCHLen )
                    {
                        return bRet;
                    }
                    achLayer[nPos+1] = '\0';
                    bySlot = atoi(achLayer);
                    byIndex++;

                }
                else if( 2 == byIndex)
                {//通道号
                    for( nPos = 0; nPos < nMaxCHLen; nPos++ )
                    {
                        achLayer[nPos] = pachAliasTemp[nPos];
                    }
                    
                    if( 0 == nPos || nPos > nMaxCHLen )
                    {
                        return bRet;
                    }
                    achLayer[nPos+1] = '\0';
                    byChannel = atoi(achLayer);
                    break;
                }

                continue;               
            }
        }

        return bRet;
	}

    void SetCallRate(u16 wCallRate) { m_wCallRate = htons(wCallRate); }
    u16  GetCallRate( void ) const { return ntohs(m_wCallRate); }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//size: 809 byte, 若增加此结构的内容注意更新每包消息的条目组个数(ADDRGROUPNUM_PERPKT)
struct TMcuAddrGroupV1
{
protected:
    u32 m_dwEntryIdx;								// 条目索引
    u32 m_dwEntryNum;							    // 与会终端个数
    u32 m_adwEntryIdx[MAXNUM_CONF_MT];	            // 与会终端索引
    s8  m_szGroupName[MAXLEN_ADDR_ENTRYNAME_V1 + 1];	// 组名   
public:
    TMcuAddrGroupV1( void ) 
	{ 
		memset(this, 0, sizeof(TMcuAddrGroupV1)); 
	}

	u8 GetMemberNum(void)
	{
		return 4;
	}
	
	u16 GetMemberLen(u8 byMemberId)
	{
		switch (byMemberId)
		{
		case 0:
			return sizeof(m_dwEntryIdx);
		case 1:
			return sizeof(m_dwEntryNum);
		case 2:
			return sizeof(m_adwEntryIdx);
		case 3:
			return sizeof(m_szGroupName);
		default:
			return 0;
		}
	}

	void GetTMcuAddrGroup( TMcuAddrGroup& tAddrGroup ) const
	{
		tAddrGroup.SetEntryIdx( GetEntryIdx() );
		tAddrGroup.SetGroupName(  GetGroupName());
		u32 dwEntryNum = GetEntryNum();
		u32 adwEntryIdx[MAXNUM_CONF_MT];
		GetAllEntryIdx( adwEntryIdx, dwEntryNum );
		tAddrGroup.SetAllEntryIdx( adwEntryIdx, dwEntryNum );
	}
	
	TMcuAddrGroupV1& operator=( const TMcuAddrGroup& tAddrGroup )
	{
		SetEntryIdx( tAddrGroup.GetEntryIdx() );
		SetEntryNum( tAddrGroup.GetEntryNum() );
		SetGroupName( tAddrGroup.GetGroupName() );
		u32 dwEntryNum = MAXNUM_CONF_MT;
		u32 adwEntryIdx[MAXNUM_CONF_MT];
		tAddrGroup.GetAllEntryIdx( adwEntryIdx, dwEntryNum );
		SetAllEntryIdx( adwEntryIdx, dwEntryNum );
		return *this;
	}

    void SetEntryIdx(u32 dwIndex) { m_dwEntryIdx = htonl(dwIndex); } 
    u32  GetEntryIdx( void ) const{ return ntohl(m_dwEntryIdx); }

    void SetEntryNum(u32 dwEntryNum) { m_dwEntryNum = htonl(dwEntryNum); }
    u32  GetEntryNum( void ) const{ return ntohl(m_dwEntryNum); }

    void GetAllEntryIdx(u32 *padwEntryIdx, u32 &dwNum) const
    {
        if(NULL == padwEntryIdx) return;

        if(dwNum > GetEntryNum()) 
        {
            dwNum = GetEntryNum();
        }
        
        if(dwNum > MAXNUM_CONF_MT)
        {
            dwNum = MAXNUM_CONF_MT;
        }
        
        for(u32 dwIdx=0; dwIdx < dwNum; dwIdx++)
        {
            padwEntryIdx[dwIdx] = ntohl(m_adwEntryIdx[dwIdx]);
        }        
    }
    void SetAllEntryIdx( const u32 *padwEntryIdx, u32 dwNum)
    {
        if(NULL == padwEntryIdx) return;

        if(dwNum > MAXNUM_CONF_MT)
        {
            dwNum = MAXNUM_CONF_MT;
        }

        for(u32 dwIdx=0; dwIdx < dwNum; dwIdx++)
        {
            m_adwEntryIdx[dwIdx] = htonl(padwEntryIdx[dwIdx]);
        }

        m_dwEntryNum = htonl(dwNum);
    }
    
    void SetGroupName(LPCSTR lpszName)
    {
        if(NULL != lpszName)
        {
            strncpy(m_szGroupName, lpszName, sizeof(m_szGroupName));
            m_szGroupName[sizeof(m_szGroupName)-1] = '\0';
        }
    }
    LPCSTR GetGroupName( void ) const { return m_szGroupName; }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//mcu主备状态通知
struct TMcuMsStatus
{
protected:
    u8  m_byIsMsSwitchOK;

public:
    TMcuMsStatus(void) : m_byIsMsSwitchOK(0) {}

    void    SetMsSwitchOK(BOOL32 bSwitchOK) { m_byIsMsSwitchOK = bSwitchOK ? 1:0; }
    BOOL32  IsMsSwitchOK(void) { return (1==m_byIsMsSwitchOK); }

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//MCU会议计费结构 len: 146
struct TConfChargeInfo
{
protected:
    s8  m_achGKUsrName[MAX_LEN_GK_USRNAME]; //GK用户名
    s8  m_achGKPwd[MAX_LEN_GK_PWD];         //GK密码

    s8  m_achConfName[MAXLEN_CONFNAME+1];   //会议名称
    s8  m_achConfE164[MAXLEN_E164+1];       //会议E164号
    
    u16 m_wBitRate;                         //会议码率
    u16 m_wTerNum;                          //终端数
    u8  m_byMixerNum;                       //混音数
    u8  m_byVMPNum;                         //多画面数
    u8  m_byBASNum;                         //码率适配数(不包含格式适配)
    u8  m_byUseCasd;                        //是否使用级联
    u8  m_byUseEncrypt;                     //是否使用加密
    u8  m_byUseDualVideo;                   //是否使用双流
    u8  m_byUseDataConf;                    //是否召开数据会议
    u8  m_byUseStreamBroadCast;             //是否使用流媒体组播
public:
    TConfChargeInfo() :m_wBitRate(0),
                       m_wTerNum(0),
                       m_byMixerNum(0),
                       m_byVMPNum(0),
                       m_byBASNum(0),
                       m_byUseCasd(0),
                       m_byUseEncrypt(0),
                       m_byUseDualVideo(0),
                       m_byUseDataConf(0),
                       m_byUseStreamBroadCast(0)
    {
        memset( &m_achGKUsrName, 0, sizeof(m_achGKUsrName) );
        memset( &m_achGKPwd,    0, sizeof(m_achGKPwd) );
        memset( &m_achConfName, 0, sizeof(m_achConfName) );
        memset( &m_achConfE164, 0, sizeof(m_achConfE164) );
    }
    void SetBitRate(u16 wBitRate) { m_wBitRate = htons(wBitRate);    }
    u16  GetBitRate(void) const { return ntohs(m_wBitRate);    }
    void SetTerNum(u16 wNum) { m_wTerNum = htons(wNum);    }
    u16  GetTerNum(void) const { return ntohs(m_wTerNum);    }
    void SetMixerNum(u8 byNum){ m_byMixerNum = byNum;    }
    u8   GetMixerNum(void) const { return m_byMixerNum;    }
    void SetVMPNum(u8 byVMPNum) { m_byVMPNum = byVMPNum;    }
    u8   GetVMPNum(void) const { return m_byVMPNum;    }
    void SetBasNum(u8 byBasNum) { m_byBASNum = byBasNum; }
    u8   GetBasNum(void) const { return m_byBASNum;    }
    void   SetIsUseCascade(BOOL32 bUse) { m_byUseCasd = bUse ? 1 : 0;    }
    BOOL32 GetIsUseCascade(void) const { return m_byUseCasd == 1 ? TRUE : FALSE;    }
    void   SetIsUseEncrypt(BOOL32 bUse) { m_byUseEncrypt = bUse ? 1 : 0;    }
    BOOL32 GetIsUseEncrypt(void) const { return m_byUseEncrypt == 1 ? TRUE : FALSE;    }
    void   SetIsUseDualVideo(BOOL32 bUse) { m_byUseDualVideo = bUse ? 1 : 0;    }
    BOOL32 GetIsUseDualVideo(void) const { return m_byUseDualVideo == 1 ? TRUE : FALSE;    }
    void   SetIsUseDataConf(BOOL32 bUse) { m_byUseDataConf = bUse ? 1 : 0;    }
    BOOL32 GetIsUseDataConf(void) const { return m_byUseDataConf == 1 ? TRUE : FALSE;    }
    void   SetIsUseStreamBroadCast(BOOL32 bUse) { m_byUseStreamBroadCast = bUse ? 1 : 0;    }
    BOOL32 GetIsUseStreamBroadCast(void) const { return m_byUseStreamBroadCast == 1 ? TRUE : FALSE;    }
    void SetGKUsrName(LPCSTR pszUsrName)
    {
        if ( pszUsrName != NULL )
        {
            u16 wLen = min(strlen(pszUsrName), MAX_LEN_GK_USRNAME-1);
            memcpy( m_achGKUsrName, pszUsrName, wLen );
            m_achGKUsrName[wLen] = '\0';
        }
        return;
    }
    LPCSTR GetGKUsrName(void) { return m_achGKUsrName;    }

    void SetGKPwd(LPCSTR pszPwd)
    {
        if ( pszPwd != NULL )
        {
            u16 wLen = min(strlen(pszPwd), MAX_LEN_GK_PWD-1);
            strncpy( m_achGKPwd, pszPwd, wLen );
            m_achGKPwd[wLen] = '\0';
        }
        return;
    }
    LPCSTR GetGKPwd(void) { return m_achGKPwd;    }
    
    void SetConfName(LPCSTR pszConfName)
    {
        if ( pszConfName != NULL )
        {
            u16 wLen = min(strlen(pszConfName), MAXLEN_CONFNAME);
            strncpy( m_achConfName, pszConfName, wLen );
            m_achConfName[wLen] = '\0';
        }
        return;
    }
    LPCSTR GetConfName(void) { return m_achConfName;    }

    void SetConfE164(LPCSTR pszConfE164)
    {
        if ( pszConfE164 != NULL )
        {
            u16 wLen = min(strlen(pszConfE164), MAXLEN_E164);
            strncpy( m_achConfE164, pszConfE164, wLen );
            m_achConfE164[wLen] = '\0';
        }
        return;
    }
    LPCSTR GetConfE164(void) { return m_achConfE164;    }
    
    BOOL32 IsNull(void) const { return (strlen(m_achGKUsrName) == 0 ? TRUE : FALSE); }

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMpuChn
{
public:
    TMpuChn():m_dwIpAddr(0),
              m_byChnId(0)
    {
    }
    void   SetIpAddr(u32 dwIp) { m_dwIpAddr = htonl(dwIp); }
    u32    GetIpAddr(void) { return ntohl(m_dwIpAddr);  }
    void   SetChnId(u8 byId) { m_byChnId = byId; }
    u8     GetChnId(void) { return m_byChnId; }
    BOOL32 IsNull()    {    return 0 == m_dwIpAddr;    }
private:
    u32 m_dwIpAddr;
    u8 m_byChnId;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//mcu的mau使用状态
struct TMcuHdBasStatus
{
    TMcuHdBasStatus()
    {
        Clear();
    }

    //会控用
    BOOL32 GetConfMau(const CConfId &cConfId, 
                      u8 &byMauNum, u32 *pdwMau, u8 &byH263pMauNum, u32 *pdwH263pMau)
    {
        u8 byPos = 0xff;
        u8 byIdx = 0;

        if (!IsConfExist(cConfId, byPos))
        {
            return FALSE;
        }
        for(byIdx = 0; byIdx < MAXNUM_CONF_MAU; byIdx ++)
        {
            if (0 != m_aadwUsedMau[byPos][byIdx])
            {
                byMauNum++;
            }
        }

        if (!IsConfExist(cConfId, byPos))
        {
            return FALSE;
        }
        for(byIdx = 0; byIdx < MAXNUM_CONF_MAU; byIdx ++)
        {
            if (0 != m_aadwUsedH263pMau[byPos][byIdx])
            {
                byH263pMauNum++;
            }
        }
        return TRUE;
    }
    //会控用
    BOOL32 GetConfMpuChn(const CConfId &cConfId,
                         u8 &byChnNum, TMpuChn *ptMpuChn)
    {
        u8 byPos = 0xff;
        u8 byIdx = 0;
        
        if (!IsConfExist(cConfId, byPos))
        {
            return FALSE;
        }
        for(byIdx = 0; byIdx < MAXNUM_CONF_MPU; byIdx ++)
        {
            if (!m_aatUsedMpuChn[byPos][byIdx].IsNull())
            {
                byChnNum++;
            }
        }
        memcpy(ptMpuChn, m_aatUsedMpuChn, byChnNum*sizeof(TMpuChn));
        return TRUE;
    }

    //会控用
    BOOL32 GetIdleMau(u8 &byMauNum, u32 *pdwMau, u8 &byH263pMauNum, u32 *pdwH263pMau)
    {
        u8 byIdx = 0;

        for(byIdx = 0; byIdx < MAXNUM_PERIEQP; byIdx ++)
        {
            if (0 != m_adwIdleMau[byIdx])
            {
                byMauNum++;
            }
        }
        memcpy(pdwMau, &m_adwIdleMau, byMauNum);

        for(byIdx = 0; byIdx < MAXNUM_PERIEQP; byIdx ++)
        {
            if (0 != m_adwIdleH263pMau[byIdx])
            {
                byH263pMauNum++;
            }
        }
        memcpy(pdwMau, &m_adwIdleH263pMau, byH263pMauNum);

        return TRUE;
    }

    //会控用(注意: MPU的空闲资源以通道为单位描述)
    BOOL32 GetIdleMpuChn(u8 &byMpuChnNum, TMpuChn *ptMpuChn)
    {
        u8 byIdx = 0;
        
        for(byIdx = 0; byIdx < MAXNUM_PERIEQP*MAXNUM_MPU_CHN; byIdx ++)
        {
            if (!m_atIdleMpuChn[byIdx].IsNull())
            {
                byMpuChnNum++;
            }
        }
        memcpy(ptMpuChn, &m_atIdleMpuChn, byMpuChnNum);

        return TRUE;
    }

    //会控用（会议失败、模板警告/通知时 需求量这里获取）
    void GetCurMauNeeded(u8 &byMauNum, u8 &byH263pMauNum)
    {
        byMauNum = m_byCurMau;
        byH263pMauNum = m_byCurH263pMau;
        return;
    }

    //会控用（会议失败、模板警告/通知时 需求量这里获取）
    //注意: mpu资源需求程度以通道为单位描述，建议用户则以板卡为单位建议:4 chn/mpu
    void GetCurMpuChnNeeded(u8 &byChnNum)
    {
        byChnNum = m_byCurMpuChn;
        return;
    }

    //以下接口MCU使用

    void SetCurMauNeeded(u8 byMauNum, u8 byH263pMauNum)
    {
        m_byCurMau = byMauNum;
        m_byCurH263pMau = byH263pMauNum;
        return;
    }

    void SetCurMpuChnNeeded(u8 byMpuChn)
    {
        m_byCurMpuChn = byMpuChn;
        return;
    }

    void SetConfMau(const CConfId &cConfId, u32 dwMauIp, BOOL32 bH263p)
    {
        u8 byIdx = 0;
        u8 byPos = 0xff;
        u8 byFstIdlePos = 0xff;
        BOOL32 bExist = FALSE;
        BOOL32 bSet = FALSE;

        if (!bH263p)
        {
            if (IsConfExist(cConfId, byPos))
            {
                byFstIdlePos = 0xff;
                bExist = FALSE;
                for(; byIdx < MAXNUM_CONF_MAU; byIdx++)
                {
                    if (0 == m_aadwUsedMau[byPos][byIdx])
                    {
                        byFstIdlePos = byFstIdlePos == 0xff ? byIdx : byFstIdlePos;
                    }
                    if (dwMauIp == m_aadwUsedMau[byPos][byIdx])
                    {
                        bExist = TRUE;
                        break;
                    }
                }
                if (!bExist)
                {
                    if (0xff == byFstIdlePos)
                    {
                        OspPrintf(TRUE, FALSE, "[THdBasStatus][SetConfMau] no pos in ConfPos.%d\n", byPos);
                        return;
                    }
                    m_aadwUsedMau[byPos][byFstIdlePos] = dwMauIp;
                }
                return;
            }
            
            bSet = FALSE;
            for (byIdx = 0; byIdx < MAXNUM_ONGO_CONF; byIdx ++)
            {
                if (m_acConfId[byIdx].IsNull())
                {
                    m_acConfId[byIdx] = cConfId;
                    m_aadwUsedMau[byIdx][0] = dwMauIp;
                    bSet = TRUE;
                }
            }
            if (!bSet)
            {
                OspPrintf(TRUE, FALSE, "[THdBasStatus][SetConfMau] no pos for new conf!\n");
            }
            return;
        }

        //H263p相关
        if (IsConfExist(cConfId, byPos))
        {
            byFstIdlePos = 0xff;
            bExist = FALSE;
            for(; byIdx < MAXNUM_CONF_MAU; byIdx++)
            {
                if (0 == m_aadwUsedH263pMau[byPos][byIdx])
                {
                    byFstIdlePos = byFstIdlePos == 0xff ? byIdx : byFstIdlePos;
                }
                if (dwMauIp == m_aadwUsedH263pMau[byPos][byIdx])
                {
                    bExist = TRUE;
                    break;
                }
            }
            if (!bExist)
            {
                if (0xff == byFstIdlePos)
                {
                    OspPrintf(TRUE, FALSE, "[TMcuHdBasStatus][SetConfMau] no pos in ConfPos.%d(h263p)\n", byPos);
                    return;
                }
                m_aadwUsedH263pMau[byPos][byFstIdlePos] = dwMauIp;
            }
            return;
        }
        
        bSet = FALSE;
        for (byIdx = 0; byIdx < MAXNUM_ONGO_CONF; byIdx ++)
        {
            if (m_acConfId[byIdx].IsNull())
            {
                m_acConfId[byIdx] = cConfId;
                m_aadwUsedH263pMau[byIdx][0] = dwMauIp;
                bSet = TRUE;
            }
        }
        if (!bSet)
        {
            OspPrintf(TRUE, FALSE, "[TMcuHdBasStatus][SetConfMau] no pos for new conf(h263p)!\n");
        }
        return;
    }

    void SetConfMpuChn(const CConfId &cConfId, u32 dwMpuIp, u8 byChnId)
    {
        u8 byIdx = 0;
        u8 byPos = 0xff;
        u8 byFstIdlePos = 0xff;
        BOOL32 bExist = FALSE;

        if (IsConfExist(cConfId, byPos))
        {
            byFstIdlePos = 0xff;
            bExist = FALSE;
            for(; byIdx < MAXNUM_CONF_MPU; byIdx++)
            {
                if (m_aatUsedMpuChn[byPos][byIdx].IsNull())
                {
                    byFstIdlePos = byFstIdlePos == 0xff ? byIdx : byFstIdlePos;
                }
                if (dwMpuIp == m_aatUsedMpuChn[byPos][byIdx].GetIpAddr() &&
                    byChnId == m_aatUsedMpuChn[byPos][byIdx].GetChnId())
                {
                    bExist = TRUE;
                    break;
                }
            }
            if (!bExist)
            {
                if (0xff == byFstIdlePos)
                {
                    OspPrintf(TRUE, FALSE, "[TMcuHdBasStatus][SetConfMpuChn] no pos in ConfPos.%d\n", byPos);
                    return;
                }
                m_aatUsedMpuChn[byPos][byFstIdlePos].SetIpAddr(dwMpuIp);
                m_aatUsedMpuChn[byPos][byFstIdlePos].SetChnId(byChnId);
            }
            return;
        }

        BOOL32 bSet = FALSE;
        for (byIdx = 0; byIdx < MAXNUM_ONGO_CONF; byIdx ++)
        {
            if (m_acConfId[byIdx].IsNull())
            {
                m_acConfId[byIdx] = cConfId;
                m_aatUsedMpuChn[byIdx][0].SetIpAddr(dwMpuIp);
                m_aatUsedMpuChn[byIdx][0].SetChnId(byChnId);
                bSet = TRUE;
            }
        }
        if (!bSet)
        {
            OspPrintf(TRUE, FALSE, "[TMcuHdBasStatus][SetConfMpuChn] no pos for new conf!\n");
        }
        return;
    }

    void SetIdleMau(u32 dwMauIp, BOOL32 bH263p)
    {
        u8 byFstIdlePos = 0xff;
        BOOL32 bExist = FALSE;
        u8 byIdx = 0;
        if (!bH263p)
        {
            for(byIdx = 0; byIdx < MAXNUM_PERIEQP; byIdx++)
            {
                if (0 == m_adwIdleMau[byIdx])
                {
                    byFstIdlePos = byFstIdlePos == 0xff ? byIdx : byFstIdlePos;
                }
                if (dwMauIp == m_adwIdleMau[byIdx])
                {
                    bExist = TRUE;
                    break;
                }
            }
            if (!bExist)
            {
                if (0xff == byFstIdlePos)
                {
                    OspPrintf(TRUE, FALSE, "[TMcuHdBasStatus][SetIdleMau] no pos for new mau!\n");
                    return;
                }
                m_adwIdleMau[byFstIdlePos] = dwMauIp;
            }
            return;
        }
        
        byFstIdlePos = 0xff;
        bExist = FALSE;
        for(byIdx = 0; byIdx < MAXNUM_PERIEQP; byIdx++)
        {
            if (0 == m_adwIdleH263pMau[byIdx])
            {
                byFstIdlePos = byFstIdlePos == 0xff ? byIdx : byFstIdlePos;
            }
            if (dwMauIp == m_adwIdleH263pMau[byIdx])
            {
                bExist = TRUE;
                break;
            }
        }
        if (!bExist)
        {
            if (0xff == byFstIdlePos)
            {
                OspPrintf(TRUE, FALSE, "[TMcuHdBasStatus][SetIdleMau] no pos for new mau(H263p)!\n");
                return;
            }
            m_adwIdleH263pMau[byFstIdlePos] = dwMauIp;
        }
        return;
    }

    void SetIdleMpuChn(u32 dwMpuIp, u8 byChnId)
    {
        u8 byFstIdlePos = 0xff;
        BOOL32 bExist = FALSE;
        u8 byIdx = 0;

        for(byIdx = 0; byIdx < MAXNUM_PERIEQP*MAXNUM_MPU_CHN; byIdx++)
        {
            if (m_atIdleMpuChn[byIdx].IsNull())
            {
                byFstIdlePos = byFstIdlePos == 0xff ? byIdx : byFstIdlePos;
            }
            if (dwMpuIp == m_atIdleMpuChn[byIdx].GetIpAddr() &&
                byChnId == m_atIdleMpuChn[byIdx].GetChnId() )
            {
                bExist = TRUE;
                break;
            }
        }
        if (!bExist)
        {
            if (0xff == byFstIdlePos)
            {
                OspPrintf(TRUE, FALSE, "[TMcuHdBasStatus][SetIdleMpuChn] no pos for new mau!\n");
                return;
            }
            m_atIdleMpuChn[byFstIdlePos].SetIpAddr(dwMpuIp);
            m_atIdleMpuChn[byFstIdlePos].SetChnId(byChnId);
        }
        return;
    }


    void Clear(void)
    {
        memset(m_acConfId, 0, sizeof(m_acConfId));
        memset(m_aadwUsedMau, 0, sizeof(m_aadwUsedMau));
        memset(m_adwIdleMau, 0, sizeof(m_adwIdleMau));
        memset(m_aadwUsedH263pMau, 0, sizeof(m_aadwUsedH263pMau));
        memset(m_adwIdleH263pMau, 0, sizeof(m_adwIdleH263pMau));
        m_byCurMau = 0;
        m_byCurH263pMau = 0;
    }

private:
    BOOL32 IsConfExist(const CConfId &cConfId, u8 &byPos)
    {
        byPos = 0xff;
        BOOL32 bExist = FALSE;
        for(u8 byIdx = 0; byIdx < MAXNUM_ONGO_CONF; byIdx ++)
        {
            if (m_acConfId[byIdx].IsNull())
            {
                continue;
            }
            if (m_acConfId[byIdx] == cConfId)
            {
                bExist = TRUE;
                byPos = byIdx;
                break;
            }
        }
        return bExist;
    }

private:
    CConfId m_acConfId[MAXNUM_ONGO_CONF];

    u32     m_aadwUsedMau[MAXNUM_ONGO_CONF][MAXNUM_CONF_MAU];
    u32     m_aadwUsedH263pMau[MAXNUM_ONGO_CONF][MAXNUM_CONF_MAU];
    TMpuChn m_aatUsedMpuChn[MAXNUM_ONGO_CONF][MAXNUM_CONF_MPU];
    
    u32     m_adwIdleMau[MAXNUM_PERIEQP];
    u32     m_adwIdleH263pMau[MAXNUM_PERIEQP];
    TMpuChn m_atIdleMpuChn[MAXNUM_PERIEQP*MAXNUM_MPU_CHN];

    u8      m_byCurMau;
    u8      m_byCurH263pMau;
    u8      m_byCurMpuChn;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*****************************************************************
 *
 *
 *                         用户管理扩展
 *
 *
 *****************************************************************/
class CExUsrInfo: public CUserFullInfo
{
public:
    CExUsrInfo() {};
	~CExUsrInfo() {};

    CExUsrInfo(const CExUsrInfo &newInfo)
    {
        memcpy( this, &newInfo, sizeof(CUserFullInfo) );
    }

    CExUsrInfo(const CUserFullInfo &newInfo)
    {
        memcpy( this, &newInfo, sizeof(CUserFullInfo) );
    }

    const CExUsrInfo& operator=(const CExUsrInfo& newInfo)
    {
        if (this == &newInfo)
        {
            return *this;
        }

        memcpy( this, &newInfo, sizeof(CExUsrInfo) );

        return (*this);
    }

    BOOL32 operator == (const CExUsrInfo& newInfo)
    {
        return ( memcmp(this, &newInfo, sizeof(CExUsrInfo)) == 0 );
    }

    // 重载的用户描述和组Id操作。MCS必须使用这里提供的方法

	//得到用户描述
	char* GetDiscription()
	{
		return (discription+1);
	}
	
	//设置用户描述
	void SetDiscription(char* buf)
	{
		if(buf == NULL)
        {
            return;
        }
        u8 byGrpId = discription[0];
		memset(discription, 0, 2 * MAX_CHARLENGTH);

        s32 length = (strlen(buf) >= 2*MAX_CHARLENGTH - 2 ? 2*MAX_CHARLENGTH - 2: strlen(buf));
		strncpy(discription+1, buf, length);

		discription[2*MAX_CHARLENGTH-1] = '\0';
        discription[0] = byGrpId;
	}
    void SetUsrGrpId( u8 byGrdId )
    {
        discription[0] = byGrdId;
    }
    u8 GetUsrGrpId()
    {
        return discription[0];
    }

    void Print( void )
    {
        StaticLog( "name:%s actor:%d fullname:%s des:%s\n",
                  GetName(), GetActor(), GetFullName(), GetDiscription());
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


// Ip地址段，网络序
struct TIpSeg
{    
public:
	u32 dwIpStart;
	u32 dwIpEnd;

    u8  byEnabled;

public:
    TIpSeg()
    {
        SetNull();
    }

    // 输入地址要求为网络序
    void SetSeg( u32 dwStart, u32 dwEnd)    
    {
        byEnabled = 1;
        dwIpStart = dwStart;
        dwIpEnd = dwEnd;
    }
    BOOL32 IsEnabled() const
    {
        return ( byEnabled == 1 ? TRUE : FALSE);
    }

    // 返回地址为网络序
    BOOL32 GetSeg ( u32& dwStart, u32& dwEnd ) const 
    {
        dwStart = dwIpStart;
        dwEnd   = dwIpEnd;
        return IsEnabled();
    }
    void SetNull()
    {
        memset(this, 0, sizeof(TIpSeg));
    }

    // 某个Ip是否在本段内。地址要求是网络序
    BOOL32 IsIpIn(u32 dwIp) const 
    {
        u32 dwMin = dwIpStart;
        u32 dwMax = dwIpEnd;
        u32 dwTmp = 0;
        if (dwMin > dwMax)
        {
            dwTmp = dwMin;
            dwMin = dwMax;
            dwMax = dwTmp;
        }
        if (dwIp >= dwMin && dwIp <= dwMax )    
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

    // 另外一个Ip段是否在本段内
    BOOL32 IsIpSegIn( const TIpSeg tAnother ) const
    {
        return ( IsIpIn(tAnother.dwIpStart)  &&
                 IsIpIn(tAnother.dwIpEnd) );
    }
}

#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/* add by wangliang 2006-12-20 for E164  start */
/*
// E164号码段
struct TE164Seg
{
public:
    s8 szE164Start[MAXLEN_E164+1];
    s8 szE164End[MAXLEN_E164+1];
    u8 byEnabled;

public:
    TE164Seg()
    {
        SetNull();
    }

    // 取得E164号码段
    void SetSeg(LPCSTR pszStart, LPCSTR pszEnd)
    {
        byEnabled = 1;
        if(pszStart != NULL && pszEnd != NULL)
        {
            memset(szE164Start, 0, sizeof(szE164Start));
            memset(szE164End, 0, sizeof(szE164End));
            strncpy(szE164Start, pszStart, MAXLEN_E164);
            strncpy(szE164End, pszEnd, MAXLEN_E164);
        }
        return;
    }

    BOOL32 IsEnabled() const
    {
        return (byEnabled == 1 ? TRUE : FALSE);
    }

    // 返回E164号码段
    BOOL32 GetSeg(s8 *pszStart, s8 *pszEnd) const 
    {
        strncpy(pszStart, szE164Start, MAXLEN_E164);
        strncpy(pszEnd, szE164End, MAXLEN_E164);
        return IsEnabled();
    }

    // 结构置空
    void SetNull()
    {
        memset(this, 0, sizeof(TE164Seg));
    }

    // E164号码是否在本E164号码段内
    // FIXME: 如果pszE164与szE164Start或szE164End长度不同，则不能够严格判断是否在段内
    BOOL32 IsE164In(LPCSTR pszE164) const
    {
        if(strncmp(szE164Start, szE164End, MAXLEN_E164) < 0)
        {
            if(strncmp(pszE164, szE164Start, MAXLEN_E164) >= 0 
                && strncmp(pszE164, szE164End, MAXLEN_E164) <= 0)
                return TRUE;
            else
                return FALSE;
        }
        else
        {
            if(strncmp(pszE164, szE164End, MAXLEN_E164) >= 0 
                && strncmp(pszE164, szE164Start, MAXLEN_E164) <= 0)
                return TRUE;
            else
                return FALSE;
        }  
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
*/
/* add by wangliang 2006-12-20 for E164  end */

// 用户组信息
struct TUsrGrpInfo
{
public:
    TUsrGrpInfo()
    {
        SetNull();
    }

public:
    void SetNull()
    {
        memset ( this, 0, sizeof(TUsrGrpInfo) );
        SetBanned(TRUE);    // 默认是禁止
    }
    // 判断本用户组是否为空
    BOOL32 IsFree() const
    {
        return (byGrpId == 0);
    }
    // 设置/获取用户组ID
    void SetUsrGrpId(u8 byId)
    {
        byGrpId = byId;
    }
    u8 GetUsrGrpId() const
    {
        return byGrpId;
    }
    // 设置/获取用户组名
    void SetUsrGrpName(LPCSTR lpszName)
    {
        memset(szName, 0, MAX_CHARLENGTH);
        strncpy(szName, lpszName, MAX_CHARLENGTH-1);
    }
    LPCSTR GetUsrGrpName() const
    {
        return szName;
    }
    // 设置/获取用户组描述
    void SetUsrGrpDesc(LPCSTR lpszDesc)
    {
        memset(szDesc, 0, 2*MAX_CHARLENGTH);
        strncpy(szDesc, lpszDesc, 2*MAX_CHARLENGTH-1);
    }
    LPCSTR GetUsrGrpDesc() const
    {
        return szDesc;
    }
    void SetMaxUsrNum(u8 byMaxNum)
    {
        byMaxUsrNum = byMaxNum;
    }
    u8 GetMaxUsrNum() const
    {
        return byMaxUsrNum;
    }
    void SetBanned(BOOL32 bIsBanned)
    {
        bBanned = bIsBanned ? 1 : 0;
    }
    BOOL32 IsBanned() const
    {
        return bBanned ? TRUE : FALSE;
    }

    // 判断终端IP是否在合法段内，
    // 输入地址是网络序
    inline BOOL32 IsMtIpInAllowSeg(u32 dwIp) const;

    void Print() const
    {
        if ( IsFree() )
        {            
            StaticLog( "\tGroup ID: FREE\n" );
            return;
        }        
        StaticLog( "\tGroup ID: %d\n", byGrpId );
        StaticLog( "\tName: %s\n", szName);
        StaticLog( "\tDesc: %s\n", szDesc);
        StaticLog( "\tMax User: %d\n", byMaxUsrNum);

        StaticLog( "\tIP Ranges: %s\n", bBanned ? "Banned" : "Allowed");
        for (s32 nLoop = 0; nLoop < MAXNUM_IPSEG; nLoop ++)
        {
            StaticLog( "\t\tNo.%d: Enabled: %d IP: 0x%x -- 0x%x\n", 
                       nLoop,
                       atIpSeg[nLoop].IsEnabled(), 
                       atIpSeg[nLoop].dwIpStart, 
                       atIpSeg[nLoop].dwIpEnd );
        }        
    }

protected:
	u8      byGrpId;
	s8      szName[MAX_CHARLENGTH];
	s8      szDesc[2*MAX_CHARLENGTH];
	u8      byMaxUsrNum;
public:
    TIpSeg  atIpSeg[MAXNUM_IPSEG];  // 地址是网络序
    u8      bBanned;     // 0 - 表示指定段是允许, 1 - 表示是禁止
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// VCS用户信息，增加了操作调度席任务分配
class CVCSUsrInfo: public CExUsrInfo
{
public:
	CVCSUsrInfo()
	{
		SetNull();
	}

    ~CVCSUsrInfo() {};

    void SetNull()
	{
		m_wTaskNum = 0;
		memset(m_cTaskID, 0, sizeof(CConfId) * MAXNUM_MCU_VCCONF);
	}

	// 指定该用户的调度任务
	BOOL AssignTask(u16 wTaskNum, const CConfId* pConfID)
	{
		if (pConfID == NULL)
		{
			return FALSE;
		}

		SetNull();
		m_wTaskNum = htons(wTaskNum);
		memcpy(m_cTaskID, pConfID, sizeof(CConfId) * wTaskNum);
		return TRUE;
	}

	// 获取该用户的调度任务
	const CConfId* GetTask() const
	{
		return m_cTaskID;
	}
	u16 GetTaskNum() const
	{
		return ntohs(m_wTaskNum);
	}

	// 任务是否在指定任务范围类
	BOOL IsYourTask(CConfId cTaskID)
	{
		BOOL dwIsYourTask = FALSE;
		for (u16 wIndex = 0; wIndex < MAXNUM_MCU_VCCONF; wIndex++)
		{
			if (cTaskID == m_cTaskID[wIndex])
			{
				dwIsYourTask = TRUE;
				break;
			}
		}
		return dwIsYourTask;
	}

protected:
    u16     m_wTaskNum;
	CConfId m_cTaskID[ MAXNUM_MCU_VCCONF ];
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*-------------------------------------------------------------------
                               CServMsg                              
--------------------------------------------------------------------*/

/*====================================================================
    函数名      ：CServMsg
    功能        ：constructor
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/15    1.0         LI Yi         创建
====================================================================*/
inline CServMsg::CServMsg( void )
{
	Init();
}

/*====================================================================
    函数名      ：CServMsg
    功能        ：constructor
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 * const pbyMsg, 要赋值的消息指针
				  u16 wMsgLen, 要赋值的消息长度，必须大于等于 SERV_MSGHEAD_LEN
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/15    1.0         LI Yi         创建
====================================================================*/
inline CServMsg::CServMsg( u8 * const pbyMsg, u16 wMsgLen )
{
	Init();
	
	if( wMsgLen < SERV_MSGHEAD_LEN || pbyMsg == NULL )
		return;
	
	wMsgLen = min( wMsgLen, SERV_MSG_LEN );
	memcpy( this, pbyMsg, wMsgLen );
	
	//set length
	SetMsgBodyLen( wMsgLen - SERV_MSGHEAD_LEN );
}

/*====================================================================
    函数名      ：~CServMsg
    功能        ：distructor
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/15    1.0         LI Yi         创建
====================================================================*/
inline CServMsg::~CServMsg( void )
{

}

/*====================================================================
    函数名      ：Init
    功能        ：BUFFER初始化
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/15    1.0         LI Yi         创建
====================================================================*/
inline void CServMsg::Init( void )
{
	memset( this, 0, SERV_MSG_LEN );	//清零
}

/*====================================================================
    函数名      ：ClearHdr
    功能        ：消息头清零
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/15    1.0         LI Yi         创建
====================================================================*/
inline void CServMsg::ClearHdr( void )
{
	u16		wBodyLen = GetMsgBodyLen();

	memset( this, 0, SERV_MSGHEAD_LEN );	//清零
	SetMsgBodyLen( wBodyLen );
}

/*====================================================================
    函数名      ：GetConfId
    功能        ：获取会议号信息
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：会议号，全0表示创建
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/15    1.0         LI Yi         创建
    02/12/24    1.0         LI Yi         修改接口
====================================================================*/
inline CConfId CServMsg::GetConfId( void ) const
{
	return( m_cConfId );
}

/*====================================================================
    函数名      ：SetConfId
    功能        ：设置会议号信息
    算法实现    ：
    引用全局变量：
    输入参数说明：const CConfId & cConfId，会议号，全0表示创建
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/15    1.0         LI Yi         创建
    02/12/24    1.0         LI Yi         修改接口
====================================================================*/
inline void CServMsg::SetConfId( const CConfId & cConfId )
{
	m_cConfId = cConfId;
}

/*====================================================================
    函数名      ：SetNullConfId
    功能        ：设置会议号信息为空
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/06    1.0         LI Yi         创建
====================================================================*/
inline void CServMsg::SetNullConfId( void )
{
	CConfId		cConfId;
	
	cConfId.SetNull();
	SetConfId( cConfId );
}

/*====================================================================
    函数名      ：GetMsgBodyLen
    功能        ：获取消息体长度信息
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：消息体长度
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/15    1.0         LI Yi         创建
====================================================================*/
inline u16 CServMsg::GetMsgBodyLen( void ) const
{
	return( ntohs( m_wMsgBodyLen ) );
}

/*====================================================================
    函数名      ：SetMsgBodyLen
    功能        ：设置消息体长度信息
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wMsgBodyLen, 消息体长度
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/15    1.0         LI Yi         创建
====================================================================*/
inline void CServMsg::SetMsgBodyLen( u16 wMsgBodyLen )
{
	m_wMsgBodyLen = htons( wMsgBodyLen );
}

/*====================================================================
    函数名      ：GetMsgBody
    功能        ：获取消息体指针，用户不需提供BUFFER
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：u8 * const指针
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/15    1.0         LI Yi         创建
====================================================================*/
inline u8 * const CServMsg::GetMsgBody( void ) const
{
	return( ( u8 * const )m_abyMsgBody );
}

/*====================================================================
    函数名      ：GetMsgBodyLen
    功能        ：获取整个消息长度
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：整个消息长度
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/26    1.0         LI Yi         创建
====================================================================*/
inline u16 CServMsg::GetServMsgLen( void ) const
{
	return( GetMsgBodyLen() + SERV_MSGHEAD_LEN );
}

/*====================================================================
    函数名      ：GetMsgBody
    功能        ：获取消息体，由用户申请BUFFER，如果过小做截断处理
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 * pbyMsgBodyBuf, 返回的消息体
				  u16 wBufLen, BUFFER大小
    返回值说明  ：实际返回的消息体长度
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/15    1.0         LI Yi         创建
====================================================================*/
inline u16 CServMsg::GetMsgBody( u8 * pbyMsgBodyBuf, u16 wBufLen ) const
{
    u16 wActLen = min( GetMsgBodyLen(), wBufLen );
	memcpy( pbyMsgBodyBuf, m_abyMsgBody, wActLen );
	return wActLen;
}

/*====================================================================
    函数名      ：SetMsgBody
    功能        ：设置消息体
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 * const pbyMsgBody, 传入的消息体，缺省为NULL
				  u16 wLen, 传入的消息体长度，缺省为0
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/15    1.0         LI Yi         创建
====================================================================*/
inline void CServMsg::SetMsgBody( u8 * const pbyMsgBody, u16 wLen )
{
    /*
    if (pbyMsgBody == NULL || wLen == 0)
    {
        SetMsgBodyLen( 0 );
        return;
    }
    */
	wLen = min( wLen, SERV_MSG_LEN - SERV_MSGHEAD_LEN );
	memcpy( m_abyMsgBody, pbyMsgBody, wLen );
	SetMsgBodyLen( wLen );
}

/*====================================================================
    函数名      ：CatMsgBody
    功能        ：添加消息体
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 * const pbyMsgBody, 传入的消息体，缺省为NULL
				  u16 wLen, 传入的消息体长度，缺省为0
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/11/07    1.0         Liaoweijiang  创建
====================================================================*/
inline void CServMsg::CatMsgBody( u8 * const pbyMsgBody, u16 wLen )
{
    /*
    if (pbyMsgBody == NULL || wLen == 0)
    {
        return;
    }
    */
	wLen = min( wLen, SERV_MSG_LEN - SERV_MSGHEAD_LEN - GetMsgBodyLen() );
	memcpy( m_abyMsgBody + GetMsgBodyLen(), pbyMsgBody, wLen );
	SetMsgBodyLen( GetMsgBodyLen() + wLen );
}

/*====================================================================
    函数名      ：GetMsgBody
    功能        ：获取整个消息指针，用户不需提供BUFFER
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：u8 * const指针
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/26    1.0         LI Yi         创建
====================================================================*/
inline u8 * const CServMsg::GetServMsg( void ) const
{
	return( ( u8 * const )( this ) );
}

/*====================================================================
    函数名      ：GetServMsg
    功能        ：获取整个消息，由用户申请BUFFER，如果过小做截断处理
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 * pbyMsgBuf, 返回的消息
				  u16 wBufLen, BUFFER大小
    返回值说明  ：实际返回的消息长度
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/15    1.0         LI Yi         创建
====================================================================*/
inline u16 CServMsg::GetServMsg( u8 * pbyMsgBuf, u16 wBufLen ) const
{
	wBufLen = min(SERV_MSG_LEN,wBufLen);
	memcpy( pbyMsgBuf, this, wBufLen );
	return( min( GetMsgBodyLen() + SERV_MSGHEAD_LEN, wBufLen ) );
}

/*====================================================================
    函数名      ：SetServMsg
    功能        ：设置整个消息
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 * const pbyMsg, 传入的整个消息
				  u16 wMsgLen, 传入的消息长度
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/15    1.0         LI Yi         创建
====================================================================*/
inline void CServMsg::SetServMsg( u8 * const pbyMsg, u16 wMsgLen )
{
	if( wMsgLen < SERV_MSGHEAD_LEN )
	{
		OspPrintf( TRUE, FALSE, "CServMsg: SetServMsg() Exception -- invalid MsgLen!\n" );
		return;
	}

	wMsgLen = min( wMsgLen, SERV_MSG_LEN );
	memcpy( this, pbyMsg, wMsgLen );
	SetMsgBodyLen( wMsgLen - SERV_MSGHEAD_LEN );
}

/*====================================================================
    函数名      ：operator=
    功能        ：操作符重载
    算法实现    ：
    引用全局变量：
    输入参数说明：CServMsg & cServMsg, 赋值的消息引用
    返回值说明  ：CServMsg对象应用
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/26    1.0         LI Yi         创建
====================================================================*/
inline const CServMsg & CServMsg::operator= ( const CServMsg & cServMsg )
{
	u16	wLen = cServMsg.GetServMsgLen();

	memcpy( this, cServMsg.GetServMsg(), wLen );
	return( *this );
}

/*-------------------------------------------------------------------
                               TConfMtInfo                           
--------------------------------------------------------------------*/

/*====================================================================
    函数名      ：TConfMtInfo
    功能        ：constructor
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/30    1.0         LI Yi         创建
====================================================================*/
inline TConfMtInfo::TConfMtInfo( void )
{
	memset( this, 0, sizeof( TConfMtInfo ) );
	
	// [12/30/2010 liuxu] 应该还需要主动设为NULL
	SetNull();
}

/*====================================================================
    函数名      ：MtInConf
    功能        ：判断该MCU下某终端是否在受邀终端列表中
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/30    1.0         LI Yi         创建
====================================================================*/
inline BOOL TConfMtInfo::MtInConf( const u8 byMtId ) const
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT)return FALSE;
	return ( ( m_abyMt[(byMtId-1) / 8] & ( 1 << ( (byMtId-1) % 8 ) ) ) != 0 );
}

/*====================================================================
    函数名      ：MtJoinedConf
    功能        ：判断该MCU下某终端是否在与会终端列表中
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：TRUE/FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/24    1.0         LI Yi         创建
====================================================================*/
inline BOOL TConfMtInfo::MtJoinedConf( const u8 byMtId ) const
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT) return FALSE;
	return ( ( m_abyJoinedMt[(byMtId-1) / 8] & ( 1 << ( (byMtId-1) % 8 ) ) ) != 0 );
}

/*====================================================================
    函数名      ：AddMt
    功能        ：增加该MCU下终端于受邀终端列表中
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/30    1.0         LI Yi         创建
====================================================================*/
inline void TConfMtInfo::AddMt( const u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT)return;
	m_abyMt[(byMtId-1) / 8] |= 1 << ( (byMtId-1) % 8 );
}

/*====================================================================
    函数名      ：AddJoinedMt
    功能        ：增加该MCU下终端于与会终端列表中，该函数自动将该终端也
	              加入到受邀终端列表中
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/25    1.0         LI Yi         创建
====================================================================*/
inline void TConfMtInfo::AddJoinedMt( const u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT)
		return;

	m_abyJoinedMt[(byMtId-1) / 8] |= 1 << ( (byMtId-1) % 8 );
	
	m_abyMt[(byMtId-1) / 8] |= 1 << ( (byMtId-1) % 8 );
}

/*====================================================================
    函数名      ：RemoveMt
    功能        ：删除该MCU下终端于受邀终端列表中，该函数自动将该终端也
	              删除出与会终端列表中
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/30    1.0         LI Yi         创建
====================================================================*/
inline void TConfMtInfo::RemoveMt( const u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT)return;
	m_abyMt[(byMtId-1) / 8] &= ~( 1 << ( (byMtId-1) % 8 ) );
	m_abyJoinedMt[(byMtId-1) / 8] &= ~( 1 << ( (byMtId-1) % 8 ) );
}

/*====================================================================
    函数名      ：RemoveJoinedMt
    功能        ：删除该MCU下终端于与会终端列表中
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/25    1.0         LI Yi         创建
====================================================================*/
inline void TConfMtInfo::RemoveJoinedMt(const u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT)return;
	m_abyJoinedMt[(byMtId-1) / 8] &= ~( 1 << ( (byMtId-1) % 8 ) );
}

/*====================================================================
    函数名      ：RemoveAllMt
    功能        ：删除该MCU下所有终端于受邀终端列表中，该函数自动将所有终端也
	              删除出与会终端列表中
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/30    1.0         LI Yi         创建
====================================================================*/
inline void TConfMtInfo::RemoveAllMt( void )
{
	memset( m_abyMt, 0, sizeof( m_abyMt ) );
	memset( m_abyJoinedMt, 0, sizeof( m_abyMt ) );
}

/*====================================================================
    函数名      ：RemoveAllJoinedMt
    功能        ：删除该MCU下所有终端于与会终端列表中
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/25    1.0         LI Yi         创建
====================================================================*/
inline void TConfMtInfo::RemoveAllJoinedMt( void )
{
	memset( m_abyJoinedMt, 0, sizeof( m_abyMt ) );
}

/*====================================================================
    函数名      ：GetAllMtNum
    功能        ：得到一个结构中所有受邀终端个数
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：会议中受邀终端个数
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/02    1.0         LI Yi         创建
====================================================================*/
inline u8 TConfMtInfo::GetAllMtNum( void ) const
{
	u8	byMtNum = 0;

	for( u8 byLoop = 0; byLoop < MAXNUM_CONF_MT; byLoop++ )
	{
		if( MtInConf( byLoop + 1 ) )
		{
			byMtNum++;
		}
	}

	return( byMtNum );
}

/*====================================================================
    函数名      ：GetLocalJoinedMtNum
    功能        ：得到一个结构中所有与会终端个数
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：会议中与会终端个数
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/25    1.0         LI Yi         创建
====================================================================*/
inline u8 TConfMtInfo::GetAllJoinedMtNum( void ) const
{
	u8	byMtNum = 0;

	for( u8 byLoop = 0; byLoop < MAXNUM_CONF_MT; byLoop++ )
	{
		if( MtJoinedConf( byLoop + 1 ) )
		{
			byMtNum++;
		}
	}

	return( byMtNum );
}


/*====================================================================
    函数名      ：GetLocalUnjoinedMtNum
    功能        ：得到一个结构中所有未与会终端个数
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：会议中未与会终端个数
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/25    1.0         LI Yi         创建
====================================================================*/
inline u8 TConfMtInfo::GetAllUnjoinedMtNum( void ) const
{
	return( GetAllMtNum() - GetAllJoinedMtNum() );
}

/*==============================================================================
函数名    :  GetMaxMtIdInConf
功能      :  得到一个结构中受邀终端中MTId最大的Id值
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2008-12-24					薛亮							创建
==============================================================================*/
inline u8 TConfMtInfo::GetMaxMtIdInConf( void ) const
{
	u8	byMaxMtId= 0;
	
	for( u8 byLoop = 0; byLoop < MAXNUM_CONF_MT; byLoop++ )
	{
		if( MtInConf( byLoop + 1 ) )
		{
			byMaxMtId = byLoop + 1;
		}
	}
	
	return( byMaxMtId );
}

/*-------------------------------------------------------------------
                           TConfAllMcuInfo                          
--------------------------------------------------------------------*/
/*====================================================================
    函数名      ：IsMcuIdAdded
    功能        ：是否已经添加mcu信息, 本级192不能调用此接口
    算法实现    ：
    引用全局变量：
    输入参数说明：[in]pbyMcuId, 数组,
				  [in]byCount, pbyMcuId中值的个数, 其值范围为[0, MAX_SUBMCU_DEPTH]
    返回值说明  ：存在返回TRUE,不存在返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    10/07/08    4.6         周晶晶         创建
====================================================================*/
inline BOOL32 TConfAllMcuInfo::IsMcuIdAdded( const u8* const pabyMcuId, const u8& byMcuIdCount)
{
	u16 wMcuIdx = FindMcuIdx(pabyMcuId, byMcuIdCount);

	ISVALIDMCUID(wMcuIdx);

	return TRUE;
}

/*====================================================================
    函数名      ：AddMcu
    功能        ：根据mcuid 添加一个mcu
    算法实现    ：
    引用全局变量：
    输入参数说明：[in]pabyMcuId, 数组,
				  [in]byMcuIdCount, pabyMcuId中值的个数, 其值范围为(0, MAX_SUBMCU_DEPTH]
				  [out]pwMcuIdx, 新mcu添加的index值
    返回值说明  ：成功返回TRUE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    10/07/08    4.6         周晶晶         创建
	10/12/15    4.6          刘旭          走读
====================================================================*/
inline BOOL32 TConfAllMcuInfo::AddMcu( const u8* const pabyMcuId, 
									  const u8& byMcuIdCount, 
									  u16* const pwMcuIdx)
{
	// 参数验证
	if (!pabyMcuId								// pbyMcuId不能为空
		|| 0 == byMcuIdCount					// byCount不能为0
		|| byMcuIdCount > MAX_SUBMCU_DEPTH      // byCount不能超过MAX_SUBMCU_DEPTH
		|| NULL == pwMcuIdx						// pwMcuIdx非空
		)
	{
		return FALSE;
	}

	*pwMcuIdx = INVALID_MCUIDX;					// 主动将wMcuIdx初始化为非法值

	// m_abyMcuInfo中已经存满
	if (MAXNUM_CONFSUB_MCU <= GetMcuCount())
	{
		return FALSE;
	}

	u16 wFindRet = FindMcuIdx(pabyMcuId, byMcuIdCount, pwMcuIdx);
	if (wFindRet < MAXNUM_CONFSUB_MCU)
	{
		// 已经存在, 直接返回TRUE
		*pwMcuIdx = wFindRet;
		return TRUE;
	}else
	{
		// wMcuIdx为第一个空闲的位置, Add Mcu在此新位置插入
		if (*pwMcuIdx < MAXNUM_CONFSUB_MCU && !IsLocalMcuId(*pwMcuIdx))
		{
			// 先将m_abyMcuInfo[wMcuIdx]重置, 避免前次使用后遗留内容
			memset(m_abyMcuInfo[*pwMcuIdx], 0, MAX_SUBMCU_DEPTH);

			// 再拷贝数据
			memcpy(m_abyMcuInfo[*pwMcuIdx], pabyMcuId, byMcuIdCount);		// 赋值
			SetMcuCount(GetMcuCount() + 1);									// 总数加一
			return TRUE;
		}

		// m_abyMcuInfo中已经存满, 不能再添加新的下级mcu. 
		OspPrintf(TRUE, TRUE, "[Error][AddMcu] m_abyMcuInfo is full now ,but m_wSum is %d\n", GetMcuCount());
	}

	return FALSE;
}

/*====================================================================
    函数名      ：RemoveMcu
    功能        ：根据MCU index删除MCU
    算法实现    ：
    引用全局变量：
    输入参数说明：[in]pwMcuIdx, 找到的下级mcu的idx
    返回值说明  ：成功返回TRUE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    10/08/28    4.6         周晶晶         创建
	10/12/15    4.6         刘旭           走读
====================================================================*/
inline BOOL32 TConfAllMcuInfo::RemoveMcu( const u16& wMcuIdx )
{
	ISVALIDMCUID(wMcuIdx);

	// 本来就是空的, 直接返回TRUE
	if (IsNull(wMcuIdx))
	{
		return TRUE;
	}

	if (IsLocalMcuId(wMcuIdx))								// 目前TConfAllMcuInfo不支持管理本地mcu
	{
		return TRUE;
	}

	// mcu id 清空
	memset( &m_abyMcuInfo[wMcuIdx][0], 0, sizeof(m_abyMcuInfo[wMcuIdx]) );
	SetMcuCount(GetMcuCount() - 1);							// 总数减一

	return TRUE;
}


/*====================================================================
    函数名      ：GetMcuIdxByMcuId
    功能        ：根据上级mcu的mcu id找到它下面所有的下级mcu的idx
    算法实现    ：
    引用全局变量：
    输入参数说明：[in]pabyMcuId, 数组,
				  [in]byMcuIdCount, pabyMcuId中值的个数, 其值范围为[0, MAX_SUBMCU_DEPTH]
				  [out]pawMcuIdx, 数组, 找到的下级mcu的idx
				  [in]byMcuIdxCount, 输入的pawMcuIdx数组的长度
    返回值说明  ：找到的下级mcu的数量, 没找到返回0
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    10/07/08    4.6         周晶晶         创建
	10/12/15    4.6          刘旭          走读
====================================================================*/
inline u8 TConfAllMcuInfo::GetMcuIdxByMcuId( const u8* pabyMcuId, 
						 const u8& byMcuIdCount, 
						 u16* const pawMcuIdx, 
						 const u8 byMcuIdxCount/* = MAXNUM_SUB_MCU*/)
{
	// 参数验证
	if (!pabyMcuId								// pbyMcuId不能为空
		|| 0 == byMcuIdCount					// byCount不能为0
		|| byMcuIdCount > MAX_SUBMCU_DEPTH      // byCount不能超过MAX_SUBMCU_DEPTH
		|| !pawMcuIdx							// pwMcuIdx不能为空
		|| 0 == byMcuIdxCount)					// byMcuIdxCount不能为0
	{
		return 0;
	}

	// 主动将pawMcuIdx重置为非法值
	memset(pawMcuIdx, INVALID_MCUIDX, byMcuIdxCount);

	// 输入的待查找的mcu为最下级mcu, 直接返回
	if (MAX_SUBMCU_DEPTH == byMcuIdCount)
	{
		return 0;
	}

	// 从byMcuIdxCount, MAXNUM_SUB_MCU, m_wSum三者之间找出最小值
	u16 wSum = min(byMcuIdxCount, MAXNUM_SUB_MCU);
	wSum = min(wSum, GetMcuCount());

	u8 byFindNum = 0;							// 待找到的下级mcu的数量
	// 遍历级联mcu列表, 找出最多wSum个下级mcu
	for(u16 wIndex = 0; wIndex < MAXNUM_CONFSUB_MCU && byFindNum < wSum; wIndex++)
	{
		if (E_CMP_RESULT_DOWN == CompareByMcuIds(wIndex, pabyMcuId, byMcuIdCount))
		{
			pawMcuIdx[byFindNum++] = wIndex;
		}
	}

	return byFindNum;
}


/*====================================================================
    函数名      ：GetIdxByMcuId
    功能        ：通过mcuid获得索引号
    算法实现    ：
    引用全局变量：
    输入参数说明：[in]pabyMcuId, 数组,
				  [in]byMcuIdCount, pabyMcuId中值的个数, 其值范围为(0, MAX_SUBMCU_DEPTH]
				  [out]pwMcuIdx, 找到的下级mcu的idx
    返回值说明  ：找到返回TRUE, 否则FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    10/07/08    4.6         周晶晶         创建
====================================================================*/
inline BOOL32 TConfAllMcuInfo::GetIdxByMcuId(const u8* pabyMcuId, const u8& byMcuIdCount, u16* const pwMcuIdx)
{	
	if (NULL == pwMcuIdx)						// pwMcuIdx非空
	{
		return FALSE;
	}

	*pwMcuIdx = INVALID_MCUIDX;					// 主动将wMcuIdx初始化为非法值
	
	// 参数验证
	if (!pabyMcuId								// pbyMcuId不能为空
		|| 0 == byMcuIdCount					// byCount不能为0
		|| byMcuIdCount > MAX_SUBMCU_DEPTH)     // byCount不能超过MAX_SUBMCU_DEPTH
	{
		return FALSE;
	}

 	// m_abyMcuInfo为空, 返回FALSE
	if (!GetMcuCount())
	{
		return FALSE;
	}
	
	// 根据pbyMcuId和byCount寻找
	*pwMcuIdx = FindMcuIdx(pabyMcuId, byMcuIdCount);

	ISVALIDMCUID(*pwMcuIdx);	
	return TRUE;
}


/*====================================================================
    函数名      ：IsSMcuByMcuIdx
    功能        ：判断wMcuIdx表示的mcu是否是本mcu的直接下级mcu
    算法实现    ：
    引用全局变量：
    输入参数说明：[in]wMcuIdx, 找到的下级mcu的idx
    返回值说明  ：是的,返回TRUE, 否则FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    10/07/08    4.6         周晶晶         创建
	10/12/14    4.6         付秀华         走读
====================================================================*/
inline BOOL32 TConfAllMcuInfo::IsSMcuByMcuIdx(const u16& wMcuIdx )
{
	ISVALIDMCUID(wMcuIdx);
	
	if (IsLocalMcuId(wMcuIdx))								// 目前TConfAllMcuInfo不支持管理本地mcu
	{
		return FALSE;
	}

	if(m_abyMcuInfo[wMcuIdx][0] != 0 && 0 == m_abyMcuInfo[wMcuIdx][1] )
	{
		return TRUE;
	}

	return FALSE;
}

/*====================================================================
    函数名      ：GetMcuIdByIdx
    功能        ：根据mcu index找到它的mcu id信息
    算法实现    ：
    引用全局变量：
    输入参数说明：[in]wMcuIdx, 找到的下级mcu的idx
				  [out]pabyMcuId, 数组, 输出查找到的mcu id
				  [in]byMcuIdCount, pabyMcuId中值的个数, 其值范围为(0, MAX_SUBMCU_DEPTH]
    返回值说明  ：找到返回TRUE, 否则FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    10/07/08    4.6         周晶晶         创建
	10/12/15    4.6         liuxu          走读
====================================================================*/
inline BOOL32 TConfAllMcuInfo::GetMcuIdByIdx( const u16 wMcuIdx, u8* const pabyMcuId, const u8 byMcuIdCount/* = MAX_SUBMCU_DEPTH*/)
{
	// 参数验证, pbyMcuId不能为空, byCount不能为0
	if (!pabyMcuId || 0 == byMcuIdCount)
	{
		return FALSE;
	}

	memset(pabyMcuId, 0, byMcuIdCount);				// 主动将pbyMcuId的值全置为0
	if (byMcuIdCount > MAX_SUBMCU_DEPTH)			// byCount不能超过MAX_SUBMCU_DEPTH
	{
		return FALSE;
	}

	if (IsLocalMcuId(wMcuIdx))						// 目前TConfAllMcuInfo中不支持本地mcu的管理
	{
		return FALSE;
	}
	
	ISVALIDMCUID(wMcuIdx);							// wMcuIdx范围验证

	// 为空, 返回false
	if (IsNull(wMcuIdx))
	{
		return FALSE;
	}

	// 将m_abyMcuInfo[wMcuIdx]的值拷贝byCount个值到pbyMcuId中
	memcpy(pabyMcuId, m_abyMcuInfo[wMcuIdx], byMcuIdCount);
	return TRUE;
}

/*====================================================================
    函数名      ：GetSMcuNum
    功能        ：获取直连下级MCU数目
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：返回下级MCU数目
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    10/07/08    4.6         周晶晶         创建
	10/12/14    4.6         付秀华         走读
====================================================================*/
inline u8 TConfAllMcuInfo::GetSMcuNum( void )
{	
	u16 wTemp = 0;
	u8 byNum = 0;
	while(wTemp < MAXNUM_CONFSUB_MCU )
	{
		if (IsSMcuByMcuIdx(wTemp))
		{
			byNum++;
		}
		++wTemp;
	}
	
	return byNum;
}


/*====================================================================
    函数名      ：IsNull
    功能        ：判断m_abyMcuInfo[wIndex]是否为空
    算法实现    ：
    引用全局变量：
    输入参数说明：wIndex, m_abyMcuInfo的个数索引, 范围 = [0, 192) || (192, MAXNUM_CONFSUB_MCU)
    返回值说明  ：为空, 返回True; 否则False
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2010/12/15  1.0         刘旭           创建
====================================================================*/
inline BOOL32 TConfAllMcuInfo::IsNull(const u16& wIndex) const
{
	// 参数范围验证
	ISVALIDMCUID(wIndex);						// wMcuIdx范围验证
	
	// 只需要判断m_abyMcuInfo[wIndex][0]就可以了. 它的值如果为0 或者为非法值, 就是空
	if (0 == m_abyMcuInfo[wIndex][0] || m_abyMcuInfo[wIndex][0] > MAXNUM_CONF_MT)
	{
		return TRUE;
	}

	return FALSE;
}


/*====================================================================
    函数名      ：CompareByMcuIds
    功能        ：将wIndex表示的mcu id序列值与pbyMcuId比较
    算法实现    ：
    引用全局变量：
    输入参数说明：[in]wIndex, m_abyMcuInfo的个数索引, 范围 = (0, 192) || (192, MAXNUM_CONFSUB_MCU)
				  [in]pabyMcuId, 存储mcu id的数组,
				  [in]byMcuIdCount, pabyMcuId中值的个数, 其值范围为(0, MAX_SUBMCU_DEPTH]
	返回值说明  ：E_CMP_RESULT_UP, 索引值为wIndex的mcu为pbyMcuId表示的mcu的上级
				  E_CMP_RESULT_EQUAL，索引值为wIndex的mcu就是pbyMcuId表示的mcu
				  E_CMP_RESULT_DOWN，索引值为wIndex的mcu为pbyMcuId表示的mcu的下级
				  E_CMP_RESULT_OTHER, 索引值为wIndex的mcu与pbyMcuId表示的mcu没有上下级关系
				  E_CMP_RESULT_INVALID, 输入参数非法, 不能比较
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2010/12/15  1.0         刘旭           创建
====================================================================*/
inline TConfAllMcuInfo::E_CapareResult 
TConfAllMcuInfo::CompareByMcuIds(const u16& wIndex, const u8* const pabyMcuId, const u8& byMcuIdCount) const
{
	// 参数验证
	if (!pabyMcuId									// pbyMcuId不能为空
		|| 0 == byMcuIdCount						// byCount不能为0
		|| byMcuIdCount > MAX_SUBMCU_DEPTH)         // byCount不能超过MAX_SUBMCU_DEPTH		
	{
		return E_CMP_RESULT_INVALID;
	}

	// 参数验证，wIndex必须合法
	if (!IsValidMcuId(wIndex)) 
	{
		return E_CMP_RESULT_INVALID;
	}

	// 参数验证，如果wIndex为空, 则返回
	if (IsNull(wIndex))
	{
		return E_CMP_RESULT_INVALID;
	}

	// wIndex代表的Mcu Index所在级联的深度
	const s8 chDepth1 = GetDepth(m_abyMcuInfo[wIndex], MAX_SUBMCU_DEPTH);

	// pabyMcuId和byMcuIdCount代表的Mcu Index所在级联的深度
	const s8 chDapth2 = GetDepth(pabyMcuId, byMcuIdCount);

	// 必须是下级级联的mcu
	if (0 >= chDepth1 || 0 >= chDapth2)
	{
		return E_CMP_RESULT_INVALID;
	}

	// 从chDepth1和chDapth2中取最小值
	const s8 chMinDepth = min(chDepth1, chDapth2);
	// 比较chMinDepth个字节的内存
	const s32 nCmpResult = memcmp(m_abyMcuInfo[wIndex], pabyMcuId, chMinDepth);

	// 前者和后者的开始chMinDepth个字节的内存块内容相等
	if (0 == nCmpResult)
	{
		if (chDepth1 < chDapth2)					// chMinDepth = chDepth1
		{
			return E_CMP_RESULT_UP;
		}else if (chDepth1 == chDapth2)				// chDepth1 = chDapth2
		{
			return E_CMP_RESULT_EQUAL;
		}else										// chMinDepth = chDapth2
		{
			return E_CMP_RESULT_DOWN;
		}
	}else
	{
		// 前后两者它们没有共同的内存块内容， 所以它们不存在直属上下级关系
		return E_CMP_RESULT_OTHER;
	}
}


/*====================================================================
    函数名      ：FindMcuIdx
    功能        ：根据mcu id寻找对应的idx, 如果找到, 返回正确的idx, 如果失败,返回非法的idx. 
				  同时, 如果下级mcu没有占满, 会返回一个空闲可用的idx
    算法实现    ：
    引用全局变量：
    输入参数说明：[in]pbyMcuId, 数组,
				  [in]byCount, pbyMcuId中值的个数, 其值范围为[0, MAX_SUBMCU_DEPTH]
				  [out]pwIdleIndex, 如果下级mcu没有占满时, 它返回一个空闲的可用的idx
	返回值说明  ：[0，MAXNUM_CONFSUB_MCU), 找到的idx合法值
				  [MAXNUM_CONFSUB_MCU, INVALID_MCUIDX], 没有找到
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2010/12/15  1.0         刘旭           创建
====================================================================*/
inline u16 TConfAllMcuInfo::FindMcuIdx(const u8* const pabyMcuId, const u8& byMcuIdCount, u16* const pwIdleIndex/* = NULL*/) const
{
	if (pwIdleIndex)
	{
		*pwIdleIndex = INVALID_MCUIDX;					// 主动将wIdleIndex置为非法值
	}

	u16 wMcuIdx = INVALID_MCUIDX;						// 返回值, 默认返回非法值

	// 参数验证
	if (!pabyMcuId										// pbyMcuId不能为空
		|| 0 == byMcuIdCount							// byCount不能为0
		|| byMcuIdCount > MAX_SUBMCU_DEPTH)				// byCount不能超过MAX_SUBMCU_DEPTH
	{
		return wMcuIdx;
	}

	for ( u16 wIndex = 0; wIndex < MAXNUM_CONFSUB_MCU; wIndex++)
	{
		// 第一个空闲Index值赋给wIdleIndex
		if (pwIdleIndex									// pwIdleIndex非空
			&& IsNull(wIndex)							// wIndex处非空
			&& !IsValidSubMcuId(*pwIdleIndex))			// *pwIdleIndex还未赋有效值
		{
			*pwIdleIndex = wIndex;
			continue;					
		}
		
		// 内存比较, 如果memcmp返回0， 则完全相等, 否则不相等
		// 这里mcu index(即wIndex)在m_abyMcuInfo中的所有值都必须和pabyMcuId相等
		if (!IsValidSubMcuId(wMcuIdx))					// 还未找到,继续找
		{
			if (E_CMP_RESULT_EQUAL == CompareByMcuIds(wIndex, pabyMcuId, byMcuIdCount))
			{
				wMcuIdx = wIndex;
				continue;
			}
		}

		// 只有所有pwIdleIndex和wMcuIdx都被找到时, 或者已经遍历完数组时, 才停止循环
		if ((IsValidSubMcuId(wMcuIdx) && !pwIdleIndex)					// wMcuIdx找到且pwIdleIndex为空
			// wMcuIdx找到且pwIdleIndex不为空且pwIdleIndex也找到
			|| (IsValidSubMcuId(wMcuIdx) && pwIdleIndex && IsValidSubMcuId(*pwIdleIndex)))
		{
			break;
		}
	}

	return wMcuIdx;
}

/*====================================================================
    函数名      ：GetDepth
    功能        ：根据mcu id获取它级联的深度
    算法实现    ：
    引用全局变量：
    输入参数说明：[in]pabyMcuId, 数组,
				  [in]byMcuIdCount, pabyMcuId中值的个数, 其值范围为(0, MAX_SUBMCU_DEPTH]
	返回值说明  ：<= 0， 非法值
				  (0，byMcuIdCount], 找到的合法的深度值
				  (byMcuIdCount, 无穷大], 非法值
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2010/12/15  1.0         刘旭           创建
====================================================================*/
inline const s8 
TConfAllMcuInfo::GetDepth(const u8* const pabyMcuId, const u8& byMcuIdCount) const
{
	s8 chDepth = 0;						// 初始为0级，即自己
	if (NULL == pabyMcuId || 0 == byMcuIdCount)
	{
		return chDepth;
	}

	const u8 byMinCount = min(byMcuIdCount, MAX_SUBMCU_DEPTH);
	for (u8 byCounter = 0; byCounter < byMinCount; byCounter++)
	{
		if ( 0 != pabyMcuId[byCounter] )
		{
			chDepth++;
			continue;
		}else
		{
			// 由于mcu id不可能为0， 为0代表空的，即可返回
			break;
		}
	}

	return chDepth;
}

/*-------------------------------------------------------------------
                           TConfAllMtInfo                          
--------------------------------------------------------------------*/


/*====================================================================
    函数名      ：AddMt
    功能        ：会议中增加一个受邀终端
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：成功返回TRUE，否则FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/31    1.0         LI Yi         创建
====================================================================*/
inline BOOL TConfAllMtInfo::AddMt(const u8 byMtId)
{
	if (!IsValidMtId(byMtId))
	{
		return FALSE;
	}

	m_tLocalMtInfo.AddMt( byMtId );
	
	return TRUE;
}

/*====================================================================
    函数名      ：AddJoinedMt 
    功能        ：会议中增加一个与会终端，该函数自动将该终端也
	              加入到受邀终端列表中
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：成功返回TRUE，否则FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/30    1.0         LI Yi         创建
====================================================================*/
inline BOOL TConfAllMtInfo::AddJoinedMt(const u8 byMtId)
{
	if (!IsValidMtId(byMtId))
	{
		return FALSE;
	}

	m_tLocalMtInfo.AddJoinedMt( byMtId );
	m_tLocalMtInfo.AddMt( byMtId );
	
	return TRUE;
}

/*====================================================================
    函数名      ：RemoveMt
    功能        ：会议中删除一个受邀终端，该函数自动将该终端也
	              删除出与会终端列表中
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/31    1.0         LI Yi         创建
	10/12/30    4.6          liuxu         修改
====================================================================*/
inline void TConfAllMtInfo::RemoveMt(u8 byMtId )
{
	if (!IsValidMtId(byMtId))
	{
		return;
	}

	m_tLocalMtInfo.RemoveMt( byMtId );
	m_tLocalMtInfo.RemoveJoinedMt( byMtId );
}

/*====================================================================
    函数名      ：RemoveJoinedMt
    功能        ：会议中删除一个与会终端
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/30    1.0         LI Yi         创建
====================================================================*/
inline void TConfAllMtInfo::RemoveJoinedMt( const u8 byMtId )
{
	if (!IsValidMtId(byMtId))
	{
		return;
	}

	m_tLocalMtInfo.RemoveJoinedMt( byMtId );
}

/*====================================================================
    函数名      ：RemoveJoinedMt
    功能        ：会议中删除一个与会终端
    算法实现    ：
    引用全局变量：
    输入参数说明：TMt  tMt, 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/30    1.0         LI Yi         创建
	10/12/30	4.6			liuxu		  修改
====================================================================*/
inline void TConfAllMtInfo::RemoveJoinedMt( const TMt& tMt )
{
	if (!IsValidMtId(tMt.GetMtId()))
	{
		return;
	}
	
	if( tMt.IsLocal() )
	{
		m_tLocalMtInfo.RemoveJoinedMt( tMt.GetMtId() );
	}
	else
	{
		// tMt的McuId的合法性检查
		if (!IsValidMcuId(tMt.GetMcuId()))
		{
			return;
		}

		if( tMt.GetMcuId() == m_atOtherMcMtInfo[tMt.GetMcuId()].GetMcuIdx() )
		{
			m_atOtherMcMtInfo[tMt.GetMcuId()].RemoveJoinedMt( tMt.GetMtId() );
		}		
	}
}

/*====================================================================
    函数名      ：AddJoinedMt
    功能        ：会议中增加一个与会终端
    算法实现    ：
    引用全局变量：
    输入参数说明：TMt  tMt, 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/30    1.0         LI Yi         创建
	10/12/30    4.6          liuxu         修改
====================================================================*/
inline BOOL TConfAllMtInfo::AddJoinedMt(const TMt& tMt)
{
	if(!IsValidMtId(tMt.GetMtId()))
	{
		return FALSE;
	}

	if (!IsValidMcuId(tMt.GetMcuId()))
	{
		return FALSE;
	}

	// 后期将会把m_tLocalMtInfo合并到m_atOtherMcMtInfo里去
	if( tMt.IsLocal() )
	{
		m_tLocalMtInfo.AddJoinedMt( tMt.GetMtId() );
		return TRUE;
	}
	else
	{
		if( tMt.GetMcuId() == m_atOtherMcMtInfo[tMt.GetMcuId()].GetMcuIdx() )
		{
			m_atOtherMcMtInfo[tMt.GetMcuId()].AddJoinedMt( tMt.GetMtId() );
			return TRUE;
		}		

		return FALSE;
	}
}


/*====================================================================
    函数名      ：RemoveAllJoinedMt
    功能        ：会议中删除一个MCU下所有与会终端（不删除MCU）
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wMcuId, MCU号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/30    1.0         LI Yi         创建
====================================================================*/
inline void TConfAllMtInfo::RemoveAllJoinedMt( void )
{
	m_tLocalMtInfo.RemoveAllJoinedMt();
}

/*====================================================================
    函数名      ：MtInConf
    功能        ：判断终端是否在受邀终端列表中
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：TRUE/FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/01    1.0         LI Yi         创建
====================================================================*/
inline BOOL TConfAllMtInfo::MtInConf( const u8 byMtId ) const
{
	return m_tLocalMtInfo.MtInConf( byMtId );
}

/*====================================================================
    函数名      ：MtJoinedConf
    功能        ：判断终端是否在与会终端列表中
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wMcuId, MCU号
				  u8 byMtId, 终端号
    返回值说明  ：TRUE/FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/30    1.0         LI Yi         创建
====================================================================*/
inline BOOL TConfAllMtInfo::MtJoinedConf(const u8 byMtId ) const
{
	return m_tLocalMtInfo.MtJoinedConf( byMtId );
}

/*====================================================================
    函数名      ：GetAllMtNum
    功能        ：得到会议完整结构中所有受邀终端个数
    算法实现    ：
    引用全局变量：
    输入参数说明：void
    返回值说明  ：返回的会议中所有受邀终端个数
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/02    1.0         LI Yi         创建
====================================================================*/
inline u8   TConfAllMtInfo::GetLocalMtNum( void ) const
{
	return m_tLocalMtInfo.GetAllMtNum();
}

/*====================================================================
    函数名      ：GetAllJoinedMtNum
    功能        ：得到会议完整结构中所有与会终端个数
    算法实现    ：
    引用全局变量：
    输入参数说明：void
    返回值说明  ：返回的会议中所有与会终端个数
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/30    1.0         LI Yi         创建
====================================================================*/
inline u8   TConfAllMtInfo::GetLocalJoinedMtNum( void ) const
{
	return m_tLocalMtInfo.GetAllJoinedMtNum();
}

/*====================================================================
    函数名      ：GetAllUnjoinedMtNum
    功能        ：得到会议完整结构中所有未与会终端个数 
    算法实现    ：
    引用全局变量：
    输入参数说明：void
    返回值说明  ：返回的会议中所有未与会终端个数
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/30    1.0         LI Yi         创建
====================================================================*/
inline u8   TConfAllMtInfo::GetLocalUnjoinedMtNum( void ) const
{
	return m_tLocalMtInfo.GetAllUnjoinedMtNum();
}

/*====================================================================
    函数名      ：SetMtInfo
    功能        ：设置某个Mc的终端信息
    算法实现    ：
    引用全局变量：
    输入参数说明：void
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/07/29    3.0         胡昌威         创建
	10/12/30    4.6          liuxu         修改
====================================================================*/
inline void TConfAllMtInfo::SetMtInfo(const TConfMtInfo& tConfMtInfo )
{
	const u16 wMcuid = tConfMtInfo.GetMcuIdx();
	if (!IsValidMcuId(wMcuid))
	{
		return;
	}

	if (IsLocalMcuId(wMcuid))
	{
		m_tLocalMtInfo = tConfMtInfo;
	}else
	{
		m_atOtherMcMtInfo[wMcuid] = tConfMtInfo;
	}
}

/*====================================================================
    函数名      ：GetMcMtInfo
    功能        ：得到某个Mc的终端信息
    算法实现    ：
    引用全局变量：
    输入参数说明：void
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/07/29    3.0         胡昌威         创建
	10/12/30    4.6          liuxu         修改
====================================================================*/
inline TConfMtInfo  TConfAllMtInfo::GetMtInfo( const u16 wMcuIdx ) const
{
	if( IsLocalMcuId(wMcuIdx))
	{
		return m_tLocalMtInfo;
	}

	TConfMtInfo tConfMtInfo;
	tConfMtInfo.SetNull();

	if (!IsValidMcuId(wMcuIdx))
	{
		return tConfMtInfo;
	}
	
	if( m_atOtherMcMtInfo[wMcuIdx].GetMcuIdx() != wMcuIdx )
	{
		// [12/30/2010 liuxu]这是什么意思?
		tConfMtInfo.SetMcuIdx( wMcuIdx );
		return tConfMtInfo;
	}
	else
	{
		tConfMtInfo = m_atOtherMcMtInfo[wMcuIdx];		
	}

	return tConfMtInfo;
}

/*====================================================================
    函数名      ：GetMtInfoPtr
    功能        ：得到某个Mc的终端信息
    算法实现    ：
    引用全局变量：
    输入参数说明：void
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/07/29    3.0         胡昌威         创建
	10/12/29    3.0         liuxu          刘旭
====================================================================*/
inline TConfMtInfo*  TConfAllMtInfo::GetMtInfoPtr(const u16 wMcuIdx )
{
	if(wMcuIdx == m_tLocalMtInfo.GetMcuIdx())
	{
		return &m_tLocalMtInfo;
	}

	if (!IsValidMcuId(wMcuIdx))
	{
		return NULL;
	}

	if( m_atOtherMcMtInfo[wMcuIdx].GetMcuIdx() != wMcuIdx )
	{
		// [8/29/2011 liuxu] 此处不能return null, 因为给mcs汇报终端状态时, 
		// 要批量把所有list都上报上去
		/*return NULL;*/
	}
	
	return &(m_atOtherMcMtInfo[wMcuIdx]);
}

/*====================================================================
    函数名      ：MtInConf
    功能        ：判断终端是否在受邀终端列表中
    算法实现    ：
    引用全局变量：
    输入参数说明：void
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/07/29    3.0         胡昌威         创建
	10/12/30    4.6          liuxu         修改
====================================================================*/
inline BOOL TConfAllMtInfo::MtInConf( const u16  wMcuIdx, const u8  byMtId ) const
{
	if (!IsValidMcuId(wMcuIdx))
	{
		return FALSE;
	}

	TConfMtInfo tConfMtInfo = GetMtInfo( wMcuIdx );
	if( !tConfMtInfo.IsNull() )
	{
		return tConfMtInfo.MtInConf( byMtId );
	}
	else
	{
		return FALSE;
	}
}

/*====================================================================
    函数名      ：MtJoinedConf
    功能        ：判断终端是否在与会终端列表中
    算法实现    ：
    引用全局变量：
    输入参数说明：void
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/07/29    3.0         胡昌威         创建
	10/12/30    4.6          liuxu         修改
====================================================================*/
inline BOOL TConfAllMtInfo::MtJoinedConf( const u16 wMcuIdx, const u8 byMtId ) const
{
	if( m_tLocalMtInfo.GetMcuIdx() == wMcuIdx )
	{
		return MtJoinedConf( byMtId );
	}

	if (!IsValidSubMcuId(wMcuIdx))
	{
		return FALSE;
	}

	TConfMtInfo tConfMtInfo = GetMtInfo( wMcuIdx );
	if( !tConfMtInfo.IsNull() )
	{
		return tConfMtInfo.MtJoinedConf( byMtId );
	}
	else
	{
		return FALSE;
	}
}

/*==============================================================================
函数名    : MtJoinedConf 
功能      : 判断终端是否在与会终端列表中(包括下级MCU下的某终端)
算法实现  :  
参数说明  :  TMt tMt
返回值说明:  BOOL
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-9-30                   薛亮
2010-12-30                  liuxu
==============================================================================*/
inline BOOL TConfAllMtInfo::MtJoinedConf( const TMt& tMt ) const
{
	if(tMt.IsLocal())
	{
		return m_tLocalMtInfo.MtJoinedConf( tMt.GetMtId() );
	}
	else
	{
		return MtJoinedConf( tMt.GetMcuIdx(),tMt.GetMtId() );
	}
}
/*====================================================================
    函数名      ：GetAllMtNum
    功能        ：得到会议完整结构中所有受邀终端个数
    算法实现    ：
    引用全局变量：
    输入参数说明：void
    返回值说明  ：得到会议中所有受邀终端数
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/07/29    3.0         胡昌威         创建
====================================================================*/
inline u16   TConfAllMtInfo::GetAllMtNum( void ) const 
{
	// 只返回本Mc的终端
	u16 wMtNum = m_tLocalMtInfo.GetAllMtNum();

	return wMtNum;
}

/*==============================================================================
函数名    :  GetMaxMtIdInConf
功能      :  得到会议完整结构中所有受邀终端中ID最大的值
算法实现  :  
参数说明  :  
返回值说明:  u8 
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2008-12-24					薛亮							创建
==============================================================================*/
inline u8 TConfAllMtInfo::GetMaxMtIdInConf( void ) const
{
	// 只返回本Mc的
	u8 byMaxMtId = 0;
	byMaxMtId = m_tLocalMtInfo.GetMaxMtIdInConf();
	return byMaxMtId;
}

/*====================================================================
    函数名      ：GetCascadeMcuNum
    功能        ：得到级联的别的mcu个数, 请优先调用TConfAllMcuInfo的相关接口
    算法实现    ：
    引用全局变量：
    输入参数说明：void
    返回值说明  ：下级Mc的个数
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/07/29    3.0         胡昌威         创建
	10/12/30    4.6          liuxu         修改
====================================================================*/
inline u16  TConfAllMtInfo::GetCascadeMcuNum( void) const
{
	u16 wMcNum = 0;
	for( u16 wLoop=0; wLoop < MAXNUM_CONFSUB_MCU; wLoop++ )
	{
		// 192是本级mcu id
		if (IsLocalMcuId(wLoop))
		{
			continue;
		}

		if( !m_atOtherMcMtInfo[wLoop].IsNull() )
		{
			wMcNum++;
		}
	}

	return wMcNum;
}

/*=============================================================================
    函 数 名： AddMcuInfo
    功    能： 会议中增加一个mcu信息，可能暂时没有终端
    算法实现： 
    全局变量： 
    参    数： u8 byMcuId
	           u8 m_byConfIdx
    返 回 值： inline BOOL 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/04/22  3.6			万春雷                  创建
	10/12/30    4.6          liuxu         修改
=============================================================================*/
inline BOOL TConfAllMtInfo::AddMcuInfo( u16 wMcuIdx, u8 m_byConfIdx )
{   
	if(!IsValidMcuId(wMcuIdx))
	{
		return FALSE;
	}

	// 本地mcu信息暂时放在m_tLocalMtInfo中, 稍后合并到m_atOtherMcMtInfo中
	if (IsLocalMcuId(wMcuIdx))
	{
		memset( &m_tLocalMtInfo, 0, sizeof(TConfMtInfo) );
		m_tLocalMtInfo.SetMcuIdx( wMcuIdx );
		m_tLocalMtInfo.SetConfIdx( m_byConfIdx );
	}

	memset( &m_atOtherMcMtInfo[wMcuIdx], 0, sizeof(TConfMtInfo) );
	m_atOtherMcMtInfo[wMcuIdx].SetMcuIdx( wMcuIdx );
	m_atOtherMcMtInfo[wMcuIdx].SetConfIdx( m_byConfIdx );
	
	return TRUE;
}

/*=============================================================================
    函 数 名： RemoveMcuInfo
    功    能： 会议中移除一个mcu信息，同时清空所有所属终端信息
    算法实现： 
    全局变量： 
    参    数： u8 byMcuId
	           u8 m_byConfIdx
    返 回 值： inline BOOL 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/04/22  3.6			万春雷                  创建
	10/12/30    4.6          liuxu                  修改
=============================================================================*/
inline BOOL TConfAllMtInfo::RemoveMcuInfo( const u16 wMcuIdx, const u8 m_byConfIdx )
{   
	if (!IsValidSubMcuId(wMcuIdx))
	{
		return FALSE;
	}

	// 本地
	if (IsLocalMcuId(wMcuIdx))
	{
		memset( &m_tLocalMtInfo, 0, sizeof(TConfMtInfo) );
		m_tLocalMtInfo.SetNull();
		return TRUE;
	}

	// 非本地mcu
	if( m_atOtherMcMtInfo[wMcuIdx].GetMcuIdx() == wMcuIdx )
	{
		memset( &m_atOtherMcMtInfo[wMcuIdx], 0, sizeof(TConfMtInfo) );
		m_atOtherMcMtInfo[wMcuIdx].SetNull();
		return TRUE;
	}

	return FALSE;
}

/*====================================================================
    函数名      ：GetAllJoinedMtNum
    功能        ：得到会议完整结构中所有与会终端个数
    算法实现    ：
    引用全局变量：
    输入参数说明：void
    返回值说明  ：所有加入会议的终端数
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/07/29    3.0         胡昌威         创建
	10/12/30    4.6          liuxu         修改
====================================================================*/
inline u32 TConfAllMtInfo::GetAllJoinedMtNum( void ) const
{
	u16 wLoop = 0;
	
	const TConfMtInfo * ptConfMtInfo = NULL;

	u32 dwMtNum = 0;					// 所有下级mcu的终端加起来可能会超过0xFFFF
	//查找Mc号
	while( wLoop < GetMaxMcuNum() )
	{
		// 192被空出来作为本地终端. 后期优化, m_tLocalMtInfo将会合并到m_atOtherMcMtInfo中
		if (IsLocalMcuId(wLoop))
		{
			//加上本Mc的终端
			dwMtNum += m_tLocalMtInfo.GetAllJoinedMtNum();
			continue;
		}

		ptConfMtInfo = &m_atOtherMcMtInfo[wLoop];
		if( !ptConfMtInfo->IsNull() )
		{
			dwMtNum += ptConfMtInfo->GetAllJoinedMtNum();
		}

		wLoop++;
	}

	return dwMtNum;
}

/*====================================================================
    函数名      ：SetChnnlStatus
    功能        ：设置录像机信道状态信息，必须先配置录放像信道数再调用此函数
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byChnnlIndex, 信道索引
				  u8 byChnnlType, 信道种类，TRecChnnlStatus::TYPE_RECORD或TYPE_PLAY
				  const TRecChnnlStatus * ptStatus, 信道状态，内含信道种类
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/11    1.0         LI Yi         创建
====================================================================*/
inline BOOL TRecStatus::SetChnnlStatus( u8 byChnnlIndex, u8 byChnnlType, const TRecChnnlStatus * ptStatus )
{
	//record channel
	if( byChnnlType == TRecChnnlStatus::TYPE_RECORD )
	{
		if( byChnnlIndex < m_byRecChnnlNum )
		{
			m_tChnnlStatus[byChnnlIndex] = *ptStatus;
			return( TRUE );
		}
		else
		{
			OspPrintf( TRUE, FALSE, "TRecStatus: Exception - Wrong record channel index%u\n", byChnnlIndex ); 
			return( FALSE );
		}
	}
	else if( byChnnlType == TRecChnnlStatus::TYPE_PLAY )	//放像信道
	{
		if( byChnnlIndex < m_byPlayChnnlNum )
		{
			m_tChnnlStatus[byChnnlIndex + m_byRecChnnlNum] = *ptStatus;
			return( TRUE );
		}
		else
		{
			OspPrintf( TRUE, FALSE, "TRecStatus: Exception - Wrong play channel index%u\n", byChnnlIndex ); 
			return( FALSE );
		}
	}
	else
	{
		OspPrintf( TRUE, FALSE, "TRecStatus: Exception - Wrong channel type%u\n", byChnnlType ); 
		return( FALSE );
	}
}

/*====================================================================
    函数名      ：SetChnnlStatus
    功能        ：设置录像机信道状态信息，必须先配置录放像信道数再调用此函数
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byChnnlIndex, 信道索引
				  u8 byChnnlType, 信道种类，TRecChnnlStatus::TYPE_RECORD或TYPE_PLAY
				  TRecChnnlStatus * ptStatus, 信道状态，内含信道种类
    返回值说明  ：TRUE/FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/11    1.0         LI Yi         创建
====================================================================*/
inline BOOL TRecStatus::GetChnnlStatus( u8 byChnnlIndex, u8 byChnnlType, TRecChnnlStatus * ptStatus ) const
{
	//record channel
	if( byChnnlType == TRecChnnlStatus::TYPE_RECORD )
	{
		if( byChnnlIndex < m_byRecChnnlNum )
		{
			*ptStatus = m_tChnnlStatus[byChnnlIndex];
			return( TRUE );
		}
		else
		{
			OspPrintf( TRUE, FALSE, "TRecStatus: Exception - Wrong record channel index%u\n", byChnnlIndex ); 
			return( FALSE );
		}
	}
	else if( byChnnlType == TRecChnnlStatus::TYPE_PLAY )	//放像信道
	{
		if( byChnnlIndex < m_byPlayChnnlNum )
		{
			*ptStatus = m_tChnnlStatus[byChnnlIndex + m_byRecChnnlNum];
			return( TRUE );
		}
		else
		{
			OspPrintf( TRUE, FALSE, "TRecStatus: Exception - Wrong play channel index%u\n", byChnnlIndex ); 
			return( FALSE );
		}
	}
	else
	{
		//OspPrintf( TRUE, FALSE, "TRecStatus: Exception - Wrong channel type%u\n", byChnnlType ); 
		return( FALSE );
	}
}


/*====================================================================
    函数名      ：IsMtIpInAllowSeg
    功能        ：判断IP是否在合法段内
    算法实现    ：
    引用全局变量：
    输入参数说明：u32, IP 地址，网络序
    返回值说明  ：TRUE/FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    06/06/26    4.0         顾振华        创建
====================================================================*/
inline BOOL32 TUsrGrpInfo::IsMtIpInAllowSeg(u32 dwIp) const
{
/*
 *  以下代码用于 同时配置了允许和禁止段 
 *  目前只配置其中一种
 *  故暂时注释
    BOOL32 bNoAllowed = TRUE; // 是否配置了允许段

    for (u8 byLoop = 0; byLoop < MAXNUM_IPSEG; byLoop ++)
    {
        if ( atIpSegAllow[byLoop].IsEnabled() )
        {
            bNoAllowed = FALSE;
            break;
        }
    }

    // 如果没有配置允许段，检查一下所有的禁止段
    if (bNoAllowed)
    {
        for (byLoop = 0; byLoop < MAXNUM_IPSEG; byLoop ++)
        {
            if ( atIpSegBan[byLoop].IsEnabled() )
            {
                if ( atIpSegBan[byLoop].IsIpIn( dwIp ) )
                {
                    // 发现在禁止段
                    return FALSE;
                }
            }
        }
        // 不在禁止段，认为允许
        return TRUE;
    }
    
    // 如果配置了允许段，先检查是否在允许段
    for (byLoop = 0; byLoop < MAXNUM_IPSEG; byLoop ++)
    {
        if ( atIpSegAllow[byLoop].IsEnabled() )
        {
            if ( atIpSegAllow[byLoop].IsIpIn( dwIp ) )
            {
                // 在某个允许段，则再看是否用小的内部禁止段给禁止了
                for (u8 byLoop2 = 0; byLoop2 < MAXNUM_IPSEG; byLoop2 ++)
                {
                    if ( atIpSegBan[byLoop2].IsEnabled() )
                    {
                        if (atIpSegBan[byLoop2].IsIpIn( dwIp ))
                        {
                            // 确实禁止，看谁范围小谁生效
                            if ( atIpSegAllow[byLoop].IsIpSegIn( atIpSegBan[byLoop2] ) )
                            {
                                return FALSE;
                            }
                        }
                    }
                }
                // 没有禁止段规定
                return TRUE;
            }
        }
    }
*/

    if ( bBanned )
    {
        // 在地址段中查看是否在黑名单中
        for (u8 byLoop = 0; byLoop < MAXNUM_IPSEG; byLoop ++)
        {
            if ( atIpSeg[byLoop].IsEnabled() && atIpSeg[byLoop].IsIpIn( dwIp ) )
            {
                return FALSE;                    
            }
        }
        return TRUE;
    }
    else
    {
        // 在地址段中查看是否在白名单中
        for (u8 byLoop = 0; byLoop < MAXNUM_IPSEG; byLoop ++)
        {
            if ( atIpSeg[byLoop].IsEnabled() && atIpSeg[byLoop].IsIpIn( dwIp ) )
            {
                return TRUE;                    
            }
        }
        return FALSE;
    }

    return FALSE;
}

//终端视频限制（目前是分辨率，码率）
struct TMtVidLmt
{
public:
	void SetMtVidRes( u8 byRes)	{ m_byRes = byRes; }
	u8	 GetMtVidRes( void ){ return m_byRes;}	
	void SetMtVidBW( u16 wBandWidth){m_wBandWidth = wBandWidth;}
	u16	 GetMtVidBw( void ){ return m_wBandWidth;}

private:
	
	u16 m_wBandWidth;
	u8	m_byRes;
	u8  m_byReserve;

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// MCU 8000B 能力限制逻辑
// 使用静态类方法提供函数接口
// 要使用这些接口, 请包含且只包含 实现文件mcupfmlmt.h 头文件一次
// MCS 收到本结构后请自行保存,在MCU切换后调用 CMcuPfmLmt::SetFilter() 接口设置
struct TMcu8kbPfmFilter
{
    TMcu8kbPfmFilter()
    {
        byMaxNumMixSingleMp3   = MAXNUM_MIX_SINGLE_MP3;     
        byMaxNumMixMultiMp3    = MAXNUM_MIX_MULTI_MP3;
        byMaxNumMixSingleG729  = MAXNUM_MIX_SINGLE_G729;
        byMaxNumMixMultiG729   = MAXNUM_MIX_MULTI_G729;
        byMaxNumVacSingleMp3   = MAXNUM_VAC_SINGLE_MP3;
        byMaxNumVacMultiMp3    = MAXNUM_VAC_MULTI_MP3; 
        byMaxNumVacSingleG729  = MAXNUM_VAC_SINGLE_G729;
        byMaxNumVacMultiG729   = MAXNUM_VAC_MULTI_G729;

        byEnable               = 1; 
    }

    void Print() const
    {
        StaticLog( "TMcu8kbPfmFilter:\n");
        StaticLog( "\t IsEnable: %d\n", byEnable);
        StaticLog( "\t byMaxNumMixSingleMp3: %d\n", byMaxNumMixSingleMp3);
        StaticLog( "\t byMaxNumMixMultiMp3: %d\n", byMaxNumMixMultiMp3);
        StaticLog( "\t byMaxNumMixSingleG729: %d\n", byMaxNumMixSingleG729);
        StaticLog( "\t byMaxNumMixMultiG729: %d\n", byMaxNumMixMultiG729);
        StaticLog( "\t byMaxNumVacSingleMp3: %d\n", byMaxNumVacSingleMp3);
        StaticLog( "\t byMaxNumVacMultiMp3: %d\n", byMaxNumVacMultiMp3);
        StaticLog( "\t byMaxNumVacSingleG729: %d\n", byMaxNumVacSingleG729);
        StaticLog( "\t byMaxNumVacMultiG729: %d\n", byMaxNumVacMultiG729);
    }

    BOOL32 IsEnable() const
    {
        return (byEnable != 0 ? TRUE : FALSE);
    }

public:
    u8 byMaxNumMixSingleMp3; //MP3混音(定制和讨论)单开方数
    u8 byMaxNumMixMultiMp3;  //MP3混音(定制和讨论)和其他(VMP和BAS)混开方数
    u8 byMaxNumMixSingleG729;//G729混音(定制和讨论)单开方数;
    u8 byMaxNumMixMultiG729; //G729混音(定制和讨论)和其他(VMP和BAS)混开方数
    u8 byMaxNumVacSingleMp3; //MP3的VAC单开方数
    u8 byMaxNumVacMultiMp3;  //MP3的VAC和其他(BAS)混开方数
    u8 byMaxNumVacSingleG729;//G729的VAC单开方数;
    u8 byMaxNumVacMultiG729; //G729的VAC和其他(BAS)混开方数
private:
    u8 byEnable;             //是否启用

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

class CMcuPfmLmt
{
public:
    /*=============================================================================
        函 数 名： GetMaxVmpStyle
        功    能： 判断当前VMP最多支持的画面合成路数
        算法实现： 
        全局变量： 
        参    数： TConfInfo &tConfInfo         [in]  会议基本信息
                   u8 byMtNum                   [in]  会议终端数量
                   u8 byMixingMtNum             [in]  正在参与混音(智能或定制)的终端数量
                   u16 &wError                  [out] 返回的错误码
        返 回 值： BOOL32
        ----------------------------------------------------------------------
        修改记录    ：
        日  期		版本		修改人		走读人    修改内容
        2007/01/29  4.0			顾振华                  创建
    =============================================================================*/
    static u8 GetMaxOprVmpStyle( const TConfInfo &tConfInfo, u8 byMtNum, u8 byMixingMtNum, u16 &wError );
	
    /*=============================================================================
        函 数 名： GetMaxOprMixNum
        功    能： 判断当前最多能设置的混音(包括定制和智能)终端数
        算法实现： 
        全局变量： 
        参    数： TConfInfo &tConfInfo     [in]  会议信息
        返 回 值： BOOL32
        ----------------------------------------------------------------------
        修改记录    ：
        日  期		版本		修改人		走读人    修改内容
        2007/01/29  4.0			顾振华                  创建
    =============================================================================*/
    static u8 GetMaxOprMixNum( const TConfInfo &tConfInfo, u16 &wError );

    /*=============================================================================
        函 数 名： GetMaxDynVmpStyle
        功    能： 获取当前能支持的动态合成风格
        算法实现： 
        全局变量： 
        参    数： TConfInfo &tConfInfo         [in]  会议基本信息
                   u8 byMtNum                   [in]  会议终端数量
                   u8 byMixingMtNum             [in]  正在参与混音(智能或定制)的终端数量
                   u16 &wError                  [out] 返回的错误码
        返 回 值： BOOL32
        说明    ： MCS不支持使用本方法
        ----------------------------------------------------------------------
        修改记录    ：
        日  期		版本		修改人		走读人    修改内容
        2007/01/31  4.0			顾振华                  创建
    =============================================================================*/    
    static u8 GetMaxDynVmpStyle( const TConfInfo &tConfInfo, u8 byMtNum, u8 byMixingMtNum, u16 &wError );
    
    /*=============================================================================
        函 数 名： IsVmpOprSupported
        功    能： 判断当前的VMP操作能否接受
        算法实现： 
        全局变量： 
        参    数： TConfInfo &tConfInfo         [in]  会议基本信息
                   u8 byNewStyle                [in]  新的VMP操作的风格
                   u8 byMtNum                   [in]  会议终端数量
                   u8 byMixingMtNum             [in]  正在参与混音(智能或定制)的终端数量
                   u16 &wError                  [out] 返回的错误码
        返 回 值： BOOL32
        ----------------------------------------------------------------------
        修改记录    ：
        日  期		版本		修改人		走读人    修改内容
        2007/01/25  4.0			张宝卿                  创建
    =============================================================================*/    
    static BOOL32 IsVmpOprSupported( const TConfInfo &tConfInfo, u8 byNewStyle, u8 byMtNum, u8 byMixingMtNum, u16 &wError );

    /*=============================================================================
        函 数 名： IsMixOprSupported
        功    能： 判断当前的定制混音操作能否接受
        算法实现： 
        全局变量： 
        参    数： TConfInfo &tConfInfo         [in]  会议信息
                   u8 byMixingMtNum             [in]  正在参与混音(智能或定制)的终端数量
                   u8 byAddNum                  [in]  要添加进混音的终端数量
                   u16 &wError                  [out] 返回的错误码
        返 回 值： BOOL32
        ----------------------------------------------------------------------
        修改记录    ：
        日  期		版本		修改人		走读人    修改内容
        2007/01/25  4.0			张宝卿                  创建
    =============================================================================*/
    static BOOL32 IsMixOprSupported( const TConfInfo &tConfInfo, u8 byMixingNum, u8 byAddNum, u16 &wError );

    /*=============================================================================
        函 数 名： IsVacOprSupported
        功    能： 判断开启VAC是否能被接受
        算法实现： 
        全局变量： 
        参    数： TConfInfo &tConfInfo   [in]  会议信息
                   u8        byMtNum      [in]  会议当前的所有在线终端数
                   u16 &wError            [out] 返回的错误码
        返 回 值： BOOL32
        ----------------------------------------------------------------------
        修改记录    ：
        日  期		版本		修改人		走读人    修改内容
        2007/01/30  4.0			顾振华                  创建
    =============================================================================*/    
    static BOOL32 IsVacOprSupported( const TConfInfo &tConfInfo, u8 byMtNum, u16 &wError );

    /*=============================================================================
        函 数 名： IsBasOprSupported
        功    能： 判断开启BAS是否能被接受
        算法实现： 
        全局变量： 
        参    数： TConfInfo &tConfInfo   [in]  会议信息
                   u8        byMixNum     [in]  会议当前的混音终端数
                   u16 &wError            [out] 返回的错误码
        返 回 值： BOOL32
        ----------------------------------------------------------------------
        修改记录    ：
        日  期		版本		修改人		走读人    修改内容
        2007/01/30  4.0			顾振华                  创建
    =============================================================================*/   
    static BOOL32 IsBasOprSupported( const TConfInfo &tConfInfo, u8 byMixNum, u16 &wError );
    
    /*=============================================================================
        函 数 名： IsMtOprSupported
        功    能： 判断当前的增加终端是否能被接受
        算法实现： 
        全局变量： 
        参    数： TConfInfo &tConfInfo   [in]  会议信息
                   u8        byMtNum      [in]  会议当前的所有在线终端数
                   u8        byMixNum     [in]  正在混音(智能、定制)终端数
                   u16 &wError            [out] 返回的错误码
        返 回 值： BOOL32
        ----------------------------------------------------------------------
        修改记录    ：
        日  期		版本		修改人		走读人    修改内容
        2007/01/26  4.0			张宝卿                  创建
    =============================================================================*/
    static BOOL32 IsMtOprSupported( const TConfInfo &tConfInfo, u8 byMtNum, u8 byMixNum, u16 &wError );

    /*=============================================================================
        函 数 名： SetFilter
        功    能： 设置性能限制的过滤参数
        算法实现： 
        全局变量： 
        参    数： TMcu8kbPfmFilter& tNewFilter   [in]  新的参数。如果用户不配置，用缺省构造即可
        返 回 值： 
        ----------------------------------------------------------------------
        修改记录    ：
        日  期		版本		修改人		走读人    修改内容
        2007/01/30  4.0			顾振华                  创建
    =============================================================================*/
    static void SetFilter(const TMcu8kbPfmFilter& tNewFilter)
    {
        m_tFilter = tNewFilter;
    }

	/*=============================================================================
        函 数 名： GetFilter
        功    能： 设置性能限制的过滤参数
        算法实现： 
        全局变量： 
        参    数： 无
        返 回 值： TMcu8kbPfmFilter
        ----------------------------------------------------------------------
        修改记录    ：
        日  期		版本		修改人		走读人    修改内容
        2007/02/07  4.0			周广程                  创建
    =============================================================================*/
    static TMcu8kbPfmFilter GetFilter()
    {
        return m_tFilter;
    }

    /* 以下函数用于MCU8000A, 不受过滤器约束 */
	/*=============================================================================
	函 数 名： IsVmpStyleSupport
	功    能： 判断某个画面合成（包括多画面电视墙）操作的目标风格是否能支持（MCU8000A）
	算法实现： 
	全局变量： 
	参    数： const TConfInfo &tConfInfo   [in] 会议信息
			   u8 byConfIdx                 [in] 会议ConfIdx信息
			   TPeriEqpStatus *ptVmpStatus  [in] 外设状态数组
			   u16 wLen                     [in] 外设状态数组长度
			   u8 byTargetStyle             [in] 指定的目标风格
			   u8 &bySelVmpId               [out] 返回符合条件的Vmp或VmpTw
	返 回 值： BOOL32 是否能支持
	----------------------------------------------------------------------
	修改记录    ：
	日  期		版本		修改人		走读人    修改内容
	2007/7/13   4.0			周广程      顾振华      创建
	=============================================================================*/
	static BOOL32 IsVmpStyleSupport( const TConfInfo &tConfInfo, u8 byConfIdx, 
							   	     TPeriEqpStatus *ptVmpStatus, u16 wLen, 
								     u8 byTargetStyle, u8 &bySelVmpId );

	/*=============================================================================
	函 数 名： GetSupportedVmpStylesFor8KE
	功    能： 返回指定会议能够支持的所有画面合成风格（针对8000E,仅供mcs调用）
	算法实现： 
	全局变量： 
	参    数：   const TConfInfo &tConfInfo   [in] 会议信息
				 u8 byConfIdx                 [in] 会议ConfIdx信息
				 TPeriEqpStatus *ptVmpStatus  [in] 外设状态数组
				 u16 wLen                     [in] 外设状态数组长度
				 u8 *pabStyle                 [out] 返回的风格数组
				 u8 &byStyleSize              [I/O] 传入数组的大小，同时返回实际大小。
													如果实际返回值大于传入值，则说明空间不够，同时上面的数组没有进行填充
	返 回 值： void 
	----------------------------------------------------------------------
	修改记录    ：
	日  期		版本		修改人		走读人    修改内容
	2010/03/01   4.6		薛亮		          创建
	=============================================================================*/
	static void GetSupportedVmpStylesFor8KE( const TConfInfo &tConfInfo, u8 byConfIdx, 
												TPeriEqpStatus *ptVmpStatus, u16 wLen, 
												u8 *pabyStyle, u8 &byArraySize );
	/*=============================================================================
	函 数 名： GetSupportedVmpStyles
	功    能： 返回指定会议能够支持的所有画面合成风格
	算法实现： 
	全局变量： 
	参    数：   const TConfInfo &tConfInfo   [in] 会议信息
				 u8 byConfIdx                 [in] 会议ConfIdx信息
				 TPeriEqpStatus *ptVmpStatus  [in] 外设状态数组
				 u16 wLen                     [in] 外设状态数组长度
				 u8 *pabStyle                 [out] 返回的风格数组
				 u8 &byStyleSize              [I/O] 传入数组的大小，同时返回实际大小。
													如果实际返回值大于传入值，则说明空间不够，同时上面的数组没有进行填充
	返 回 值： void 
	----------------------------------------------------------------------
	修改记录    ：
	日  期		版本		修改人		走读人    修改内容
	2007/7/13   4.0			周广程      顾振华      创建
	=============================================================================*/
	static void GetSupportedVmpStyles( const TConfInfo &tConfInfo, u8 byConfIdx, 
										TPeriEqpStatus *ptVmpStatus, u16 wLen, 
										u8 *pabyStyle, u8 &byArraySize );

	/*=============================================================================
	函 数 名： GetMaxCapVMPByConfInfo
	功    能： 仅根据会议信息计算当前会议最多支持的画面合成或多画面电视墙路数（MCU8000A）		   
	算法实现： 
	全局变量： 
	参    数： const TConfInfo &tConfInfo
	返 回 值： BOOL32 
	----------------------------------------------------------------------
	修改记录    ：
	日  期		版本		修改人		走读人    修改内容
	2007/7/14   4.0			顾振华      　　　　　　创建
	=============================================================================*/
    static u8 GetMaxCapVMPByConfInfo( const TConfInfo &tConfInfo );

    /*=============================================================================
	函 数 名： IsConfFormatHD
	功    能： 本会议是否为高清会议，仅对H.264有效
	算法实现： 
	全局变量： 
    参    数： const TConfInfo &tConfInfo   [in] 会议信息
	返 回 值： BOOL32
	----------------------------------------------------------------------
	修改记录    ：
	日  期		版本		修改人		走读人    修改内容
	2007/12/19  4.0			张宝卿                  创建
	=============================================================================*/
    static BOOL32 IsConfFormatHD( const TConfInfo &tConfInfo );

    /*=============================================================================
	函 数 名： IsConfFormatOverCif
	功    能： 本会议是否为D1或者高清会议，仅对H.264有效
	算法实现： 
	全局变量： 
    参    数： const TConfInfo &tConfInfo   [in] 会议信息
	返 回 值： BOOL32 如果非H.264主格式会议均返回FALSE
	----------------------------------------------------------------------
	修改记录    ：
	日  期		版本		修改人		走读人    修改内容
	2008/03/11  4.5			顾振华                  创建
	=============================================================================*/
    static BOOL32 IsConfFormatOverCif( const TConfInfo &tConfInfo );

	/*=============================================================================
	函 数 名： IsConfPermitVmp
	功    能： 本会议是否允许画面合成
	算法实现： 
	全局变量： 
    参    数： const TConfInfo &tConfInfo   [in] 会议信息
	返 回 值： BOOL32 如果非H.264主格式会议均返回FALSE
	----------------------------------------------------------------------
	修改记录    ：
	日  期		版本		修改人		走读人    修改内容
	2008/03/11  4.5			顾振华                  创建
	=============================================================================*/
    static BOOL32 IsConfPermitVmp( const TConfInfo &tConfInfo );

    /*=============================================================================
	函 数 名： IsRollCallSupported
	功    能： 本会议是否支持会议点名
	算法实现： 
	全局变量： 
    参    数： const TConfInfo &tConfInfo   [in] 会议信息
	返 回 值： BOOL32
	----------------------------------------------------------------------
	修改记录    ：
	日  期		版本		修改人		走读人    修改内容
	2007/12/19  4.0			张宝卿                  创建
	=============================================================================*/
    static BOOL32 IsRollCallSupported( const TConfInfo &tConfInfo );

	/*=============================================================================
	函 数 名： IsVmpSupported
	功    能： 本会议是否支持画面合成器
	算法实现： 
	全局变量： 
    参    数： const TConfInfo &tConfInfo   [in] 会议信息
	返 回 值： BOOL32
	----------------------------------------------------------------------
	修改记录    ：
	日  期		版本		修改人		走读人    修改内容
	2007/12/19  4.0			张宝卿                  创建
	=============================================================================*/
    static BOOL32 IsVmpSupported( const TConfInfo &tConfInfo );
    
    /*=============================================================================
	函 数 名： IsMPWSupported
	功    能： 本会议是否支持多画面电视墙
	算法实现： 
	全局变量： 
    参    数： const TConfInfo &tConfInfo   [in] 会议信息
	返 回 值： BOOL32
	----------------------------------------------------------------------
	修改记录    ：
	日  期		版本		修改人		走读人    修改内容
	2007/12/19  4.0			张宝卿                  创建
	=============================================================================*/
    static BOOL32 IsMPWSupported( const TConfInfo &tConfInfo );
    
    /*=============================================================================
	函 数 名： IsTVWallSupported
	功    能： 本会议是否支持电视墙
	算法实现： 
	全局变量： 
    参    数： const TConfInfo &tConfInfo   [in] 会议信息
	返 回 值： BOOL32
	----------------------------------------------------------------------
	修改记录    ：
	日  期		版本		修改人		走读人    修改内容
	2007/12/19  4.0			张宝卿                  创建
	=============================================================================*/
    static BOOL32 IsTVWallSupported( const TConfInfo &tConfInfo );
    
    /*=============================================================================
	函 数 名： IsMultiVidStreamSupported
	功    能： 本会议是否支持视频多速多格式
	算法实现： 
	全局变量： 
    参    数： const TConfInfo &tConfInfo   [in] 会议信息
               u8 byMode = MODE_VIDEO       [in] 音频或视频,不支持MODE_BOTH
	返 回 值： BOOL32
	----------------------------------------------------------------------
	修改记录    ：
	日  期		版本		修改人		走读人    修改内容
	2007/12/19  4.0			张宝卿                  创建
	=============================================================================*/
    static BOOL32 IsMultiStreamSupported( const TConfInfo &tConfInfo, u8 byMode = MODE_VIDEO );

private:
	/*=============================================================================
	函 数 名： GetMaxCapVMP
	功    能： 计算当前会议最多支持的画面合成或多画面电视墙路数以及相应的设备ID（MCU8000A）			   
	算法实现： 
	全局变量： 
	参    数： const TConfInfo &tConfInfo
			   TPeriEqpStatus *ptVmpStatus : VMP/VMPTW 状态数组
			   u16 wlen : 状态数组长度
			   u8 &byVmpCapChnnlNum [OUT]: 
			   u8 &bySelVmpId[OUT]:
	返 回 值： BOOL32 
	----------------------------------------------------------------------
	修改记录    ：
	日  期		版本		修改人		走读人    修改内容
	2007/6/20   4.0			周广程      顾振华      创建
	=============================================================================*/
	static BOOL32 GetMaxCapVMP( const TConfInfo &tConfInfo, u8 byConfIdx, 
								TPeriEqpStatus *ptVmpStatus, u16 wLen, 
								u8 &byVmpCapChnnlNum, 
                                u8 &bySelVmpId, 
								u8 byTargetStyle = 0	//0表自动风格
								);

	/*=============================================================================
	函 数 名： GetSupportedVmpStyles
	功    能： 返回能够支持的所有画面合成风格（MCU8000A）
	算法实现： 
	全局变量： 
	参    数： const TConfInfo &tConfInfo
			   u8 byEqpType
			   u8 bySubType
			   u8 byCapChnnlNum             
			   u8 byVmpMaxChnnlNum
               u8 *pabyStyle                [out] 返回的风格数组
               u8 &byArraySize              [I/O] 传入数组的大小，同时返回实际大小。
                                                  如果实际返回值大于传入值，则说明空间不够，同时上面的数组没有进行填充
	返 回 值： void 
	----------------------------------------------------------------------
	修改记录    ：
	日  期		版本		修改人		走读人    修改内容
	2007/7/13   4.0			周广程      顾振华      创建
	2009/2/18	4.6			薛亮					增参数区分VMP种类；引入20风格
	=============================================================================*/
	static void GetSupportedVmpStyles( const TConfInfo &tConfInfo, u8 byEqpType, u8 bySubType,u8 byCapChnnlNum, u8 byVmpMaxChnnlNum, 
                                       u8 *pabyStyle, u8 &byArraySize );
	/*=============================================================================
	函 数 名： GetSelVmpCap
	功    能： 获取最接近会议能力的vmp id及其能力
	算法实现： 
	全局变量： 
	参    数： u8 abySelVmpIdx[]				[in] 
	TPeriEqpStatus *ptStatus		[in] 
	u8 bySelNum						[in]
	u8 byMaxChnlNumByConf			[in]
	u8 &byVmpCapChnnlNum			[out]
	u8 &bySelVmpId					[out]
	返 回 值： BOOL32
	----------------------------------------------------------------------
	修改记录    ：
	日  期		版本		修改人		走读人    修改内容
	12/9/2008				薛亮					
	=============================================================================*/
	static BOOL32 GetSelVmpCap(u8 abySelVmpIdx[], TPeriEqpStatus *ptStatus, u8 bySelNum, 
		u8 byMaxChnlNumByConf,u8 &byVmpCapChnnlNum, u8 &bySelVmpId);
	
	/*====================================================================
    函数名      : GetMaxChlNumByStyle
    功能        : 得到画面合成风格的最大成员数
    算法实现    : 
    引用全局变量: 无
    输入参数说明: byVMPStyle 画面合成风格
    返回值说明  : 无
	----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/31    3.0         胡昌威       创建
	====================================================================*/
	static u8 GetMaxChlNumByStyle( u8 byVMPStyle );

	

private:
    static TMcu8kbPfmFilter m_tFilter;
};

// xliang [3/18/2009] VMP 占高清前适配通道和需要进高清前适配通道但未占成功的终端信息
struct TVmpHdChnnlMemInfo
{
public:
	TMt	tMtInHdChnnl[MAXNUM_MPU2VMP_E20_HDCHNNL];
	TMt	tMtOutHdChnnl[MAXNUM_MPUSVMP_MEMBER];
public:
	TVmpHdChnnlMemInfo( void )
	{
		init();
	}
	void init()
	{
		memset(tMtInHdChnnl,0, sizeof(tMtInHdChnnl));
		memset(tMtOutHdChnnl,0,sizeof(tMtOutHdChnnl));
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// xliang [11/28/2008]VMP 通道成员信息
struct TChnnlMemberInfo 
{
	enum TChnnlPriMask
    {
        Mask_BeSelected		=   0x0001,
		Mask_NoneKeda		=   0x0002,
		Mask_Speaker		=   0x0004,
		Mask_Dstream		=   0x0008,
		Mask_MMcu			=	0x0010,
		Mask_BeIpc			=	0x0020
    };
public:
//	u8	m_byMtId;			//占该通道的MT ID
	TMt m_tSeizedMt;		//占该通道的MT
	u8	m_byVmpPRIAttr;		//优先级属性
	u8  m_byReserved1;		//预留
	u8  m_byReserved2;		//预留
public:
	TChnnlMemberInfo(void)
	{
		SetNull();
	}
	void SetNull()
	{
		//m_byMtId = 0;
		m_tSeizedMt.SetNull();
		m_byVmpPRIAttr = 0;
	}
	void SetAttrNull()
	{
		m_byVmpPRIAttr = 0;
	}
	void SetAttrSelected()
	{
		m_byVmpPRIAttr |= Mask_BeSelected;
	}
	void SetAttrNoneKeda()
	{
		m_byVmpPRIAttr |= Mask_NoneKeda;
	}
	void SetAttrSpeaker()
	{
		m_byVmpPRIAttr |= Mask_Speaker;
	}
	void SetAttrDstream()
	{
		m_byVmpPRIAttr |= Mask_Dstream;
	}
	void SetAttrMMcu()
	{
		m_byVmpPRIAttr |= Mask_MMcu;
	}
	void SetAttrIPC()
	{
		m_byVmpPRIAttr |= Mask_BeIpc;
	}
	BOOL32 IsAttrNull() const
	{
		return ( m_byVmpPRIAttr == 0 );
	}
	BOOL32 IsCanAdjResMt() const
	{
		// 发言人与被选看终端是可降分辨率vip，其余为不可降分辨率vip
		if (IsAttrSpeaker() || IsAttrSelected())
		{
			return TRUE;
		}
		return FALSE;
	}
	BOOL32 IsAttrMMcu() const
	{
		return ( (m_byVmpPRIAttr & Mask_MMcu) != 0 );
	}
	BOOL32 IsAttrDstream() const
	{
		return ( (m_byVmpPRIAttr & Mask_Dstream) != 0 );
	}
	BOOL32 IsAttrSpeaker() const
	{
		return ( (m_byVmpPRIAttr & Mask_Speaker) != 0 );
	}
	BOOL32 IsAttrNoneKeda() const
	{
		return ( (m_byVmpPRIAttr & Mask_NoneKeda) != 0 );
	}
	BOOL32 IsAttrSelected() const
	{
		return ( (m_byVmpPRIAttr & Mask_BeSelected) != 0 );
	}
	
	BOOL32 IsAttrBeIPC() const
	{
		return ( (m_byVmpPRIAttr & Mask_BeIpc) != 0 );
	}
	u8 GetMtId( void ) const
	{
		return m_tSeizedMt.GetMtId();
	}
	TMt GetMt(void) const
	{
		return m_tSeizedMt;
	}
	void SetMt(TMt tMt)
	{
		m_tSeizedMt = tMt;
	}
}	
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//终端执行的抢占选项操作
struct TSeizeChoice 
{
public:
//	u8	byMtId;
	TMt tSeizedMt;
	u8	byStopVmp;		//该MT是否停止VMP
	u8  byStopSelected;	//该MT是否停止被选看
	u8	byStopSpeaker;	//该MT是否取消发言人
public:
	TSeizeChoice(void)
	{
		init();
	}
	void init()
	{
		tSeizedMt.SetNull();
		byStopSelected = 0;
		byStopSpeaker  = 0;
		byStopVmp	   = 0;
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//上次被临时替代的某通道中的VMP成员信息(发言人跟随策略中用)
struct TLastVmpchnnlMemInfo
{
public:
	u8 m_byLastVmpMemInChnnl;
	u8 m_byLastMemType;
public:
	TLastVmpchnnlMemInfo( void )
	{
		Init();
	}
	void Init()
	{
		m_byLastVmpMemInChnnl = ~0;
		m_byLastMemType = 0;
	}
	void SetLastVmpMemChnnl( u8 byMemChnnl ) { m_byLastVmpMemInChnnl = byMemChnnl; }
	u8	 GetLastVmpMemChnnl( void ) { return m_byLastVmpMemInChnnl; }
	void SetLastVmpMemType( u8 byMemType ) { m_byLastMemType = byMemType; }
	u8   GetLastVmpMemType (void ) { return m_byLastMemType; } 

};

//下级MCU下的VMP成员信息
struct TVmpCasMem
{
private:

	TMt	m_tMt;	
	u8	m_byPos;	// VMP中的通道位置

public:
	TVmpCasMem( void )
	{
		Init();
	}
	void Init()
	{
		m_byPos = ~0;
		m_tMt.SetNull();
	}
	void SetMt( TMt tMt ) {m_tMt = tMt; }
	TMt  GetMt( void ) const { return m_tMt; }
	void SetPos( u8 byPos ) { m_byPos = byPos; }
	u8	 GetPos( void ) const { return m_byPos; }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TVmpCasMemInfo
{
private:
	TVmpCasMem m_atVmpCasMem[MAXNUM_CONF_MT];

public:
	TVmpCasMemInfo (void)
	{
		Init();
	}
	void Init()
	{
		u8 byLoop;
		for(byLoop = 0; byLoop < MAXNUM_CONF_MT; byLoop ++)
		{
			m_atVmpCasMem[byLoop].Init();
		}
	}
	void SetMem(u8 byId, const TVmpCasMem tVmpCasMem) 
	{ 
		if( byId == 0 )
		{
			return;
		}

		m_atVmpCasMem[byId - 1].SetMt(tVmpCasMem.GetMt());
		m_atVmpCasMem[byId - 1].SetPos(tVmpCasMem.GetPos());
	}
	TMt GetMt( u8 byId ) const { return m_atVmpCasMem[byId - 1].GetMt(); }
	u8  GetPos(u8 byId ) const { return m_atVmpCasMem[byId - 1].GetPos(); }

};

//[03/04/2010] zjl add (电视墙轮询代码合并)
//会议轮询信息 
#define POLLING_POS_START   (u8)0xFF
#define POLLING_POS_INVALID (u8)0xFE

struct TConfPollParam
{
protected:
	u8 m_byPollMtNum;  //参加轮询终端个数
	u8 m_byCurPollPos; //轮询当前位置
    u8 m_bySpecPos;    //用户强行设置的轮询位置

	// [12/21/2010 liuxu][走读]这里有必要保存一个这么大的数组？即使需要，下级终端需不需要呢？
	TMtPollParam m_atMtPollParam[MAXNUM_CONF_MT]; //参加轮询的终端参数列表
	
	TMt m_tLastPolledMt;
public:
	
    TConfPollParam()
    {
        memset(this, 0, sizeof(TConfPollParam));
        ClearSpecPos();
    }
    
	void SetPollList(u8 byMtNum, TMtPollParam* ptParam)
	{
		if ( ptParam == NULL && byMtNum != 0)
		{
			return ;
		}
		m_byPollMtNum = byMtNum > MAXNUM_CONF_MT ? MAXNUM_CONF_MT : byMtNum;
		if ( m_byPollMtNum > 0 )
		{
			memcpy( m_atMtPollParam, ptParam, m_byPollMtNum * sizeof(TMtPollParam) );
		}    
	}

    void InitPollParam(u8 byMtNum, TMtPollParam* ptParam)
	{
		SetPollList(byMtNum, ptParam) ;		
		m_byCurPollPos = 0;
        ClearSpecPos();
		m_tLastPolledMt.SetNull();
	}

	void SetPolledMtNum(u8 byMtNum){m_byPollMtNum = byMtNum;} // xliang [12/31/2008] 
    u8   GetPolledMtNum() { return m_byPollMtNum; }

    TMtPollParam* const GetPollMtByIdx(u8 byIdx)
	{
		if ( byIdx < m_byPollMtNum )
		{
			return &m_atMtPollParam[byIdx];
		}
		else
		{
			return NULL;
		}    
	}
   
    void SetCurrentIdx(u8 byIdx){ m_byCurPollPos = byIdx;}
    u8   GetCurrentIdx() { return m_byCurPollPos;}

    TMtPollParam* const GetCurrentMtPolled(){ return &m_atMtPollParam[m_byCurPollPos];}
	
    BOOL32 IsExistMt(const TMt &tMt, u8 &byIdx)
	{
		byIdx = 0xFF;
		u8 byPos = 0;
		for( ; byPos < m_byPollMtNum; byPos++ )
		{
			if ( tMt.GetMcuId() == m_atMtPollParam[byPos].GetMcuId() && 
				tMt.GetMtId() == m_atMtPollParam[byPos].GetMtId() )
			{
				byIdx = byPos;
				return TRUE;
			}
		}		
        return FALSE;
	}
    void RemoveMtFromList(const TMt &tMt)
	{
		u8 byPos = 0;
		for( ; byPos < m_byPollMtNum; byPos++ )
		{
			if ( tMt == m_atMtPollParam[byPos] )
			{            
				//byPos为找到的移除的终端所处位置
				//20100702_tzy 如果当前轮询位置在移除位置之后，则当前轮询位置向前移动
				if (m_byCurPollPos > byPos)
				{
					m_byCurPollPos--;
				}
				// 移动列表
				for( ; byPos < m_byPollMtNum-1; byPos++ )
				{
					m_atMtPollParam[byPos] = m_atMtPollParam[byPos+1];
				}
				//zjlhdupoll 最后一个遗留的除去
				memset(&m_atMtPollParam[byPos], 0, sizeof(TMtPollParam));
				m_byPollMtNum--;
				break;
			}
		}
	}
	
    void SpecPollPos(u8 byIdx)
    {
        if ( byIdx < m_byPollMtNum || byIdx == POLLING_POS_START )
        {
            m_bySpecPos = byIdx;
        }
    }
    BOOL32 IsSpecPos() const
    {
        return ( m_bySpecPos != POLLING_POS_INVALID );
    }
    u8   GetSpecPos() const
    {
        u8 byRet = m_bySpecPos;
        return byRet;
    }
    void ClearSpecPos()
    {
        m_bySpecPos = POLLING_POS_INVALID;
    }

	TMt GetLastPolledMt( void )
	{
		return m_tLastPolledMt;
	}

	void SetLastPolledMt( TMt tMt )
	{
		m_tLastPolledMt = tMt;
	}

    void Print() const
    {
        StaticLog("TConfPollParam:\n");
        StaticLog("\t m_byPollMtNum: %d(Current.%d, Spec.%d)\n", m_byPollMtNum, m_byCurPollPos, m_bySpecPos);
        for (u8 byIdx = 0; byIdx < m_byPollMtNum; byIdx ++)
        {
            StaticLog("\t\t Idx.%d Mt: <%d,%d>\n", byIdx, m_atMtPollParam[byIdx].GetMcuId(), m_atMtPollParam[byIdx].GetMtId());
        }
    }
};

//[03/04/2010] zjl add (电视墙轮询代码合并)
struct TTvWallPollParam : public TConfPollParam
{
public:
	TTvWallPollInfo m_tTWPollnfo;
public:
	TTvWallPollParam()
	{
		memset( this, 0, sizeof(TTvWallPollParam) );
	}
	
    void SetTWPollInfo(TTvWallPollInfo tTWPollInfo){ m_tTWPollnfo = tTWPollInfo;}
    TTvWallPollInfo GetTWPollInfo(){ return m_tTWPollnfo; }
	
    void SetTvWall(TEqp tTvWall) { m_tTWPollnfo.SetTvWall(tTvWall); }
    TEqp GetTvWall(void) const{ return m_tTWPollnfo.GetTvWall(); } 
    
	void SetTWChnnl(u8 byChnnl) { m_tTWPollnfo.SetTWChnnl(byChnnl); }
    u8   GetTWChnnl(void) const { return m_tTWPollnfo.GetTWChnnl(); }
	
    void SetKeepTime( u32 byKeepTime ){ m_tTWPollnfo.SetKeepTime(byKeepTime); }
	u32  GetKeepTime( void ){ return m_tTWPollnfo.GetKeepTime(); } 
	
	void SetSchemeIdx(u8 bySchemeIdx) { m_tTWPollnfo.SetSchemeIdx(bySchemeIdx); }
    u8   GetSchemeIdx(void) const { return m_tTWPollnfo.GetSchemeIdx(); }
    
	void SetPollState(u8 byPollState){ m_tTWPollnfo.SetPollState(byPollState);}
	u8   GetPollState(){ return m_tTWPollnfo.GetPollState();}
    
	void SetPollNum(u32 dwPollNum){ m_tTWPollnfo.SetPollNum(dwPollNum);}
	u32  GetPollNum(){ return m_tTWPollnfo.GetPollNum(); }
	
    void SetMediaMode(u8 byMode){m_tTWPollnfo.SetMediaMode(byMode);}
	u8   GetMediaMode(){ return m_tTWPollnfo.GetMediaMode();}
	
	void SetMtPollParam(TMtPollParam tMtPollParam){ m_tTWPollnfo.SetMtPollParam(tMtPollParam);}
    TMtPollParam GetMtPollParam() const{ return m_tTWPollnfo.GetMtPollParam(); }
	
	void SetConfIdx(u8 byConfIdx){ m_tTWPollnfo.SetConfIdx(byConfIdx); }
	u8   GetConfIdx(void) const { return m_tTWPollnfo.GetConfIdx(); }
	
	u8   GetIsStartAsPause( void ){ return m_tTWPollnfo.GetIsStartAsPause(); }                                
	void SetIsStartAsPause( u8 byIsStartAsPause ){ m_tTWPollnfo.SetIsStartAsPause(byIsStartAsPause); }	   
	u16  GetChnIID(){ return MAKEWORD( m_tTWPollnfo.GetTWChnnl(), m_tTWPollnfo.GetTvWall().GetEqpId()); }
};

//Vmp单通道轮询信息(Vmp批量轮询代码合并)(len:42)
struct TVmpPollInfo : public TPollInfo
{
public:
	u8 m_byCurPollMtNum;		//当前已进行轮询的mt数目(Vmp批量轮询用)
	u8 m_byChnlIdx;				//当前用的vmp通道(Vmp批量轮询用)
	u8 m_byFirst;				//第一版面(Vmp批量轮询用)
	u8 m_byVmpPollOver;			//vmp一次轮询结束(Vmp批量轮询用)
	//	TMtPollParamEx atMtPollParamEx[192];	//终端对应的vmp通道号 FIXME：vcinst中有个类似的
	u8	m_abyCurPollBlokMtId[MAXNUM_MPUSVMP_MEMBER];	//存放当前版面的所有MTid(Vmp批量轮询用)
    
public:
	TVmpPollInfo(void){ memset( this, 0x0, sizeof( TVmpPollInfo ) ); }
	
}
/*
#ifndef WIN32
__attribute__((packed))
#endif
*/
PACKED
;

//画面合成轮询参数(Vmp批量轮询代码合并)
struct TVmpPollParam: public TConfPollParam
{
protected:
	TVmpPollInfo m_tVmpPollnfo;

public:
	TVmpPollParam()
	{
		memset( this, 0, sizeof(TVmpPollParam) );
	}
	
	// vmp单通道轮询用,Vmp单通道轮询信息接口
    void SetVmpPollInfo(TVmpPollInfo tVmpPollInfo){ m_tVmpPollnfo = tVmpPollInfo;}
    TVmpPollInfo GetVmpPollInfo(){ return m_tVmpPollnfo; }
	
	// vmp单通道轮询用,当前被轮询广播的终端及其参数接口
	void SetMtPollParam(TMtPollParam tMtPollParam){ m_tVmpPollnfo.SetMtPollParam(tMtPollParam);}
    TMtPollParam GetMtPollParam() const{ return m_tVmpPollnfo.GetMtPollParam(); }
	
	// vmp单通道轮询用,轮询状态接口
	void SetPollState(u8 byPollState){ m_tVmpPollnfo.SetPollState(byPollState);}
	u8   GetPollState(){ return m_tVmpPollnfo.GetPollState();}
    
	// vmp单通道轮询用,终端轮询的次数接口
	void SetPollNum(u32 dwPollNum){ m_tVmpPollnfo.SetPollNum(dwPollNum);}
	u32  GetPollNum(){ return m_tVmpPollnfo.GetPollNum(); }
	
	// vmp单通道轮询用,轮询模式接口
    void SetMediaMode(u8 byMode){m_tVmpPollnfo.SetMediaMode(byMode);}
	u8   GetMediaMode(){ return m_tVmpPollnfo.GetMediaMode();}
	
	// 以下都为vmp批量轮询轮询用接口,与vmp单通道轮询无关
	u8 GetVmpPolledMtNum()	{ return m_tVmpPollnfo.m_byCurPollMtNum; }
	void SetVmpPolledMtNum(u8 byCurPollMtNum){ m_tVmpPollnfo.m_byCurPollMtNum = byCurPollMtNum ;}
	
	u8 GetVmpChnnlIdx() { return m_tVmpPollnfo.m_byChnlIdx; }
	void SetVmpChnnlIdx( u8 byChnlIdx) {m_tVmpPollnfo.m_byChnlIdx = byChnlIdx; }

	void SetIsVmpPollOver(BOOL32 bVmpPollOver){m_tVmpPollnfo.m_byVmpPollOver = (bVmpPollOver == TRUE? 1:0);}
	BOOL32 IsVmpPollOver(){return (m_tVmpPollnfo.m_byVmpPollOver == 1)?TRUE:FALSE;}

	void SetIsBatchFirst(BOOL32 bFirst){ m_tVmpPollnfo.m_byFirst = (bFirst == TRUE)?1:0;}
	BOOL32 IsBatchFirst(){return (m_tVmpPollnfo.m_byFirst == 1)?TRUE:FALSE;}

    /*==============================================================================
	函数名    :  SetCurPollBlokMtId
	功能      :  记录当前版面的MT Id
	算法实现  :  
	参数说明  :  u8 byMemberId	[in]
				 u8 byMtId		[in]	
	返回值说明:  void
	-------------------------------------------------------------------------------
	修改记录  :  
	日  期       版本          修改人          走读人          修改记录
	2008-12-24					薛亮							创建
	==============================================================================*/
	void SetCurPollBlokMtId(u8 byMemberId,u8 byMtId)
	{
		if( (byMemberId < MAXNUM_MPUSVMP_MEMBER ) 
			&& (0 < byMtId && byMtId <= MAXNUM_CONF_MT) )
		{
			m_tVmpPollnfo.m_abyCurPollBlokMtId[byMemberId] = byMtId;
		}
		else
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "Invalid member Id or mt Id.\n");
		}
	}

	/*==============================================================================
	函数名    :  IsMtInCurPollBlok
	功能      :  判断某终端是否出现在当前版面中
	算法实现  :  
	参数说明  :  u8 byMtId	[in]  要判断的终端ID	
				 u8 *byMbId	[out] 终端对应的Member ID
	返回值说明:  BOOL32 
	-------------------------------------------------------------------------------
	修改记录  :  
	日  期       版本          修改人          走读人          修改记录
	2008-12-24					薛亮							create
	==============================================================================*/
	BOOL32 IsMtInCurPollBlok(u8 byMtId,u8 *byMbId)
	{
		for(u8 byMemberId = 0; byMemberId < MAXNUM_MPUSVMP_MEMBER; byMemberId ++)
		{
			if(m_tVmpPollnfo.m_abyCurPollBlokMtId[byMemberId] == byMtId)
			{
				*byMbId = byMemberId;
				return TRUE;
			}
		}
		return FALSE;
	}
	/*==============================================================================
	函数名    :  GetMtInCurPollBlok
	功能      :  获取当前版面某通道中的MT ID
	算法实现  :  
	参数说明  :  u8 byMemberId [in]
	返回值说明:  u8 
	-------------------------------------------------------------------------------
	修改记录  :  
	日  期       版本          修改人          走读人          修改记录
	2008-12-24					薛亮							create
	==============================================================================*/
	u8 GetMtInCurPollBlok(u8 byMemberId)
	{
		return m_tVmpPollnfo.m_abyCurPollBlokMtId[byMemberId];
	}

};

struct TSpyVmpInfo //画面合成相关的多回传信息
{
//	u8		m_byRes;	//分辨率上限
	u8		m_byPos;	//通道位置
//	u8		m_byKeepOrgRes;	//表明即使下级调不了分辨率(比如非keda的)，上级还是可以让其以全分辨率进
    u8		m_byMemberType;     //画面合成成员类型,参见vccommon.h中画面合成成员类型定义
    u8		m_byMemStatus;      //成员状态(MT_STATUS_CHAIRMAN, MT_STATUS_SPEAKER, MT_STATUS_AUDIENCE)
	TEqpNoConstruct    m_tVmp;             // 画面合成外设
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TSpySwitchDstInfo
{
	//u8				m_byMcInstId;   // 某个会控的id
	u8				m_bySrcChnnl;		//源通道号（对于终端目前只填0，对于外设有多种选择）
	TTransportAddr	m_tDstVidAddr;		//目的视频的ip和port
	TTransportAddr  m_tDstAudAddr;		//目的音频的ip和port （目前Vid和Aud的Ip是相同的，仅port不同）
private:
	u32   m_dwMcIp;       //会控IP
    u32   m_dwMcSSRC;
public:
	void SetMcIp(u32 dwMcIp)
	{
		m_dwMcIp = htonl(dwMcIp);
		return;
	}
	
	u32 GetMcIp(void) const
	{
		return ntohl(m_dwMcIp);
	}

	void SetMcSSRC(u32 dwMcSSRC)
	{
		m_dwMcSSRC = htonl(dwMcSSRC);
		return;
	}
	
	u32 GetMcSSRC(void) const
	{
		return ntohl(m_dwMcSSRC);
	}
}

#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


struct TSpySwitchInfo     // 选看相关的多回传初始信息
{
	//u8    m_byMcInstId;   // 某个会控的id
	u8    m_byDstChlIdx;  // 目的通道索引
	u8    m_bySrcChlIdx;  // 源通道索引号
	TMtNoConstruct   m_tDstMt;       // 目的终端（若为会控监控，目的端为空）

private:
	u32   m_dwMcIp;       //会控IP
	u32   m_dwMcSSRC;    //会控随机数
public:
	void SetMcIp(u32 dwMcIp)
	{
		m_dwMcIp = htonl(dwMcIp);
		return;
	}

	u32 GetMcIp(void) const
	{
		return ntohl(m_dwMcIp);
	}

	void SetMcSSRC(u32 dwMcSSRC)
	{
		m_dwMcSSRC = htonl(dwMcSSRC);
		return;
	}
	
	u32 GetMcSSRC(void) const
	{
		return ntohl(m_dwMcSSRC);
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TSpyHduInfo     // 进高清电视墙相关的多回传初始信息
{
	u8    m_bySrcMtType;  // 电视墙源通道成员类型
	u8    m_bySchemeIdx;  // 预案索引号
	u8    m_byDstChlIdx;  // HDU目的通道索引
	TEqpNoConstruct  m_tHdu;         // 外设信息
private:
	u8    m_bySubChnIdx;	//HDU多画面子通道
public:
	void SetSubChnIdx(u8 bySubChnIdx)
	{
		m_bySubChnIdx = bySubChnIdx;
	}
	u8 GetSubChnIdx(void) const
	{
		return m_bySubChnIdx;
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TSpyTVWallInfo     // 进电视墙相关的多回传初始信息
{
	u8    m_bySrcMtType;  // 电视墙源通道成员类型
	u8    m_bySchemeIdx;  // 预案索引号
	u8    m_byDstChlIdx;  // TVWall目的通道索引
	TEqpNoConstruct  m_tTvWall;      // 外设信息
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TSpyPollInfo     // 轮询相关的多回传初始信息
{
	u16   m_wKeepTime;        //终端轮询的保留时间 单位:秒(s)
	u8    m_byPollingPos;        //终端轮询的次数
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TSpyRollCallInfo     // 多回传点名相关信息
{
	TMtNoConstruct m_tCaller;        // 点名人
	TMtNoConstruct m_tOldCaller;     // 老的点名人
	TMtNoConstruct m_tOldCallee;     // 老的被点名人 
	//u8			   m_byMcInstId;   // 某个会控的id
private:
	u32   m_dwMcIp;       //会控IP
	u32   m_dwMcSSRC;     //会控随机数
public:
	void SetMcIp(u32 dwMcIp)
	{
		m_dwMcIp = htonl(dwMcIp);
		return;
	}
	
	u32 GetMcIp(void) const
	{
		return ntohl(m_dwMcIp);
	}

	void SetMcSSRC(u32 dwMcSSRC)
	{
		m_dwMcSSRC = htonl(dwMcSSRC);
		return;
	}
	
	u32 GetMcSSRC(void) const
	{
		return ntohl(m_dwMcSSRC);
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TSpyRecInfo     //录像机路径
{
	TRecStartPara m_tRecPara;
	u8   m_byRecMode;                 // 0会议录像，1终端录像
	TEqpNoConstruct m_tRec;                      // 录像机外设           
	s8   m_szRecFullName[KDV_MAX_PATH];   // 录像机存放路径
	u16  m_wSerialNO;				  // 回给mcs的流水号
	//u8   m_byMcInstId;				  // 某个会控的id
private:
	u32   m_dwMcIp;       //会控IP
	u32   m_dwMcSSRC;     //会控随机数
public:
	void SetMcIp(u32 dwMcIp)
	{
		m_dwMcIp = htonl(dwMcIp);
		return;
	}
	
	u32 GetMcIp(void) const
	{
		return ntohl(m_dwMcIp);
	}

	void SetMcSSRC(u32 dwMcSSRC)
	{
		m_dwMcSSRC = htonl(dwMcSSRC);
		return;
	}
	
	u32 GetMcSSRC(void) const
	{
		return ntohl(m_dwMcSSRC);
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


union USpyInfo
{
	TSpySwitchInfo   m_tSpySwitchInfo;   // 选看的相关信息
	TSpyHduInfo      m_tSpyHduInfo;      // 进高清电视墙的相关信息
	TSpyPollInfo     m_tSpyPollInfo;     // 轮询相关信息
	TSpyTVWallInfo   m_tSpyTVWallInfo;   // 进电视墙的相关信息
	TSpyVmpInfo	     m_tSpyVmpInfo;		 // VMP相关额外信息
	TSpyRollCallInfo m_tSpyRollCallInfo; // 点名相关信息
	TSpyRecInfo		 m_tSpyRecInfo;		 //录像机
	TSpySwitchDstInfo	m_tSpySwitchDstInfo;	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMultiCacMtInfo
{
public:
	//TMt         m_tMt;
	u8          m_byCasLevel; //相对与本级，终端所处级联树中的跳跃的级别数
	u8          m_abyMtIdentify[MAX_CASCADELEVEL]; //对应级别中终端的标识 
public:
	TMultiCacMtInfo()
	{
		memset( this,0,sizeof( TMultiCacMtInfo ) );
	}

	BOOL32 IsNull()
	{
		for (u8 byCasLvl = 0; byCasLvl < m_byCasLevel;byCasLvl++)
		{
			if (m_abyMtIdentify[byCasLvl] != 0)
			{
				return FALSE;
			}
		}
		return TRUE;
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMsgHeadMsg
{
public:

	TMultiCacMtInfo m_tMsgSrc; // 消息源
	TMultiCacMtInfo m_tMsgDst;// 消息目的
public:
	TMsgHeadMsg()
	{
		memset( this,0,sizeof(TMsgHeadMsg) );
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TPreReleaseMtInfo
{
	TMt         m_tMt;
	u8          m_byCasLevel; //相对与本级，终端所处级联树中的跳跃的级别数
	u8          m_abyMtIdentify[MAX_CASCADELEVEL]; //对应级别中终端的标识 
	u8 m_byCanReleaseMode;	//可释放模式
private:
	s16 m_swCount;			//如果该mt信息非本级下的mcu信息就小于0 ,否则为0
public:
	u8 m_byIsNeedRelease;	//返回给上级mcu判断该可释放终端最终是否需要释放
	u8 m_byCanReleaseVideoDstNum;//可以释放的视频目的数
	u8 m_byCanReleaseAudioDstNum;//可以释放的音频目的数
	u8 m_byIsReuseBasChl;//是否重用可释放终端所占用的bas通道
						 //==MODE_VIDEO视频bas可复用，==MODE_AUDIO音频bas可复用，MODE_BOTH音视频bas都可复用
	
public:
	TPreReleaseMtInfo()
	{
		memset( this, 0, sizeof(TPreReleaseMtInfo) );
	}

	//传入主机序[3/7/2012 chendaiwei]
	void SetCount( s16 swCount )
	{
		m_swCount = htons(swCount);
	}

	//传出主机序
	s16 GetCount( void ) const
	{
		return ntohs(m_swCount);
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// [pengjie 2010/9/11] 多回传逻辑整理，下一步将数据成员变为私有的不再暴露给外部，外部直接调接口
struct TSpyResource         // 多回传基本信息 (len: 36)
{
public:
	TMt		       m_tSpy;		     // 回传源(Mt)
	TSimCapSet     m_tSimCapset;     // 对应回传实体的能力描述 ( 该回传源 所回传到的 回传目的端的 能力 )
	TTransportAddr m_tSpyAddr;       // 对应回传实体的回传通道地址信息
	TTransportAddr m_tVideoRtcpAddr; // [liu lijiu][20100823]记录下级接收RTCP的地址
	TTransportAddr m_tAudioRtcpAddr; // [liu lijiu][20100823]记录下级会议接收RTCP的地址
	u8		       m_bySpyMode;	     // AUDIO,VIDEO,BOTH
	u8             m_byReserved1;    // 预留字段
	u32            m_dwReserved2;    // 预留字段
	
public:
	TSpyResource()
	{
		Clear();
	}
	
	u16 GetSpyChnnl()
	{
		return ( m_tSpyAddr.GetPort()  - CASCADE_SPY_STARTPORT ) / PORTSPAN;
	}

	void Clear( void )
	{
		memset( this, 0, sizeof(TSpyResource) );
		m_tSimCapset.Clear();
	}
	
	TMt GetSpyMt( void ) const { return m_tSpy; }
	void SetSpyMt( const TMt &tSpyMt ) { m_tSpy = tSpyMt; }
	
	TSimCapSet GetSimCapset( void ) const { return m_tSimCapset; }
	void SetSimCapset( const TSimCapSet &tSimCapset ) { m_tSimCapset = tSimCapset; }
	
	TTransportAddr GetSpyAddr( void ) { return m_tSpyAddr; }
	void SetSpyAddr( const TTransportAddr &tSpyAddr ) { m_tSpyAddr = tSpyAddr; }
	
	u8 GetSpyMode( void ) { return m_bySpyMode; }
	void SetSpyMode( const u8 bySpyMode ) { m_bySpyMode = bySpyMode; }
	
	void SetVidSpyBackRtcpAddr(TTransportAddr tVideoRtcpAddr) { m_tVideoRtcpAddr = tVideoRtcpAddr; }
	TTransportAddr GetVidSpyBackRtcpAddr(void) const { return m_tVideoRtcpAddr; }

	void SetAudSpyBackRtcpAddr(TTransportAddr tAudioRtcpAddr) { m_tAudioRtcpAddr = tAudioRtcpAddr; }
	TTransportAddr GetAudSpyBackRtcpAddr(void) const { return m_tAudioRtcpAddr; }
	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
// End

// [pengjie 2010/9/11] 多回传逻辑调整
struct TPreSetInReq            // 回传初始信息
{
//protected: 下一步不再暴露成员信息
private:
	u32 m_dwEvId;             // 记录在进行回传交互前，本来应该执行的消息（如改变发言人，终端选看等）
public:
	u8  m_bySpyMode;          // 最初的回传模式
    TSpyResource m_tSpyMtInfo;    // 请求回传的终端信息
	USpyInfo    m_tSpyInfo;   // 与回传各业务相关的一些必要的信息
	TPreReleaseMtInfo m_tReleaseMtInfo;
	
public:
	TPreSetInReq()
	{
		memset( this, 0, sizeof(TPreSetInReq) );
	}

	//设置传入主机序[3/7/2012 chendaiwei]
	void SetEvId( u32 dwEvId)
	{
		m_dwEvId = htonl(dwEvId);
	}
	
	//获取返回主机序
	u32 GetEvId( void ) const
	{
		return ntohl(m_dwEvId);
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//[nizhijun 2010/12/15]多回传丢包重传相关信息
struct  TMultiRtcpInfo
{
public:
	TMt					m_tSrcMt;			//回传源信息
	u8					m_bySpyMode;		//回传模式 MODE_AUDIO/MODE_VIDEO/MODE_BOTH
	TTransportAddr		m_tVidRtcpAddr;		//视频RTCP申述信息
	TTransportAddr      m_tAudRtcpAddr;		//音频RTCP申述信息
public:
	TMultiRtcpInfo()
	{
		memset( this, 0 , sizeof(TMultiRtcpInfo) ); 
	}
}
#ifndef WIN32
__attribute__( (packed) )
#endif
;

// struct TPreSetInReq            // 回传初始信息
// {
// // [pengjie 2010/7/30] TPreSetInReq 结构整理, 主要调整回传目的端信息
// 	TMt m_tSrc;               // 回传的源
// 	u32 m_dwEvId;             // 记录在进行回传交互前，本来应该执行的消息（如改变发言人，终端选看等）
// 	u8  m_bySpyMode;          // 最初的回传模式
// 	TSimCapSet	m_tSimCapSet; // 目的端得能力
// 	USpyInfo    m_tSpyInfo;   // 与回传各业务相关的一些必要的信息
// 	TPreReleaseMtInfo m_tReleaseMtInfo;
// 
// public:
// 	TPreSetInReq()
// 	{
// 		memset( this, 0, sizeof(TPreSetInReq) );
// 	}
// }
// #ifndef WIN32
// __attribute__ ( (packed) ) 
// #endif
// ;


struct TPreSetInRsp			// PreSetIn 应答信息
{
// [pengjie 2010/7/30] TPreSetInReq 结构整理, 主要调整回传目的端信息
	TPreSetInReq m_tSetInReqInfo; // 记录上级要求的回传相关信息
	u8  m_byRspSpyMode;		// 下级真正能达到的回传模式(BOTH/V/A )
	
private:
	u32	m_dwVidSpyBW;		// 回传视频将占用多少带宽
	u32	m_dwAudSpyBW;		// 回传音频将占用多少带宽
// 	TTransportAddr m_tAudBackRtcp;//[liu lijiu][20100901]记录本级会议为下级会议分配的音频RTCP端口
// 	TTransportAddr m_tVidBackRtcp;//[liu lijiu][20100901]记录本级会议为下级会议分配的视频RTCP端口
	
public:
	TPreSetInRsp()
	{
		memset( this, 0, sizeof(TPreSetInRsp) );
	}

	void SetVidSpyRtcpAddr(TTransportAddr tVideoRtcpAddr)
	{
		m_tSetInReqInfo.m_tSpyMtInfo.SetVidSpyBackRtcpAddr( tVideoRtcpAddr );
	}
	void SetAudSpyRtcpAddr(TTransportAddr tAudioRtcpAddr)
	{
		m_tSetInReqInfo.m_tSpyMtInfo.SetAudSpyBackRtcpAddr( tAudioRtcpAddr );
	}
	TTransportAddr GetVidSpyRtcpAddr(void) const
	{
		return m_tSetInReqInfo.m_tSpyMtInfo.GetVidSpyBackRtcpAddr();
	}
	TTransportAddr GetAudSpyRtcpAddr(void) const
	{
		return m_tSetInReqInfo.m_tSpyMtInfo.GetAudSpyBackRtcpAddr();
	}

	//传入主机序参数 [3/7/2012 chendaiwei]
	void SetVidSpyBW( u32 dwSpyBW )
	{
		m_dwVidSpyBW = htonl(dwSpyBW);
	}

	//传入主机序参数
	void SetAudSpyBW( u32 dwSpyBw )
	{
		m_dwAudSpyBW = htonl(dwSpyBw);
	}

	//返回主机序  [3/7/2012 chendaiwei]
	u32 GetVidSpyBW ( void )
	{
		return ntohl(m_dwVidSpyBW);
	}

	//返回主机序
	u32 GetAudSpyBW ( void )
	{
		return ntohl(m_dwAudSpyBW);
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// struct TSpyResource         // 多回传基本信息
// {
// public:
// 	TMt		m_tSpy;		    // 可以是MT，也可能是EQP(如mixer)
//     u16     m_wSpyStartPort;    // 记录SpyChnnl, 该值 * PORTSPAN + CASCADE_SPY_STARTPORT  对应MMCU 转发板上的回传port
// 	u8		m_bySpyMode;	// AUDIO,VIDEO,BOTH
// 	
// 	u8		m_byOldSpyMode;   // 保留字段
// 	u32		m_dwReserved;
// 
// public:
// 	TSpyResource()
// 	{
// 		memset( this, 0, sizeof(TSpyResource) );
// 	}
// 
// 	u16 GetSpyChnnl()
// 	{
// 		return (m_wSpyStartPort  - CASCADE_SPY_STARTPORT ) / PORTSPAN;
// 	}
// }
// #ifndef WIN32
// __attribute__ ( (packed) ) 
// #endif
// ;

// [1/13/2010 xliang] 
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<8000E 网口配置相关>>>>>>>>>>>>>>>>>>>>>>>>>
/*=============================================================================
模板类：
类名： Link
作用： 链表节点
说明： 如有特别初始化需要，类T应自行提供相应的无参数构造函数
=============================================================================*/
template <class T>
class ArrayNode
{
private:
    T m_Data;                                   // 用于保存结点元素的内容
    u32 m_dwNextId;                             // 指向后继结点的指针

public:      
    ArrayNode() { m_dwNextId = 0; }    
    ArrayNode(const T &Data, u32 dwNextId = 0)
    {
        SetData( Data );
        SetNext( dwNextId );
    }
    
    T * GetData( void ) { return &m_Data; }    
    void SetData( const T &Data ) { m_Data = Data; }    
    u32 GetNext( void ) { return ntohl(m_dwNextId); }
    void SetNext( u32 dwNextId ) { m_dwNextId = htonl(dwNextId); }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*=============================================================================
模板类：
类名： LinkArray
作用： 构造一个单向链表
说明： 1、如有特别初始化需要，类T应自行提供相应的无参数构造函数
2、元素T应支持重载运算符 == : 该要求取消
3、2009-02-26：取消 IsExist 和 GetPos 两函数，因为这两个函数需要 类T 重载
运算符 == ，通用性不强
=============================================================================*/
template <class T, u32 dwArrayLen=1>
class LinkArray
{
private: 
    ArrayNode<T> m_atLinkArray[dwArrayLen];
    u32     m_dwHeadId;
    u32     m_dwTailId;                                  // 单链表的头、尾ID号, 索引号等于ID号减1

private: 
    u32     GetIdByLink( ArrayNode<T> * pCur )
    {
        u32 dwArraySize = sizeof(m_atLinkArray)/sizeof(m_atLinkArray[0]);
        u32 dwIdx = 0;
        for ( dwIdx = 0; dwIdx < dwArraySize; dwIdx++ )
        {
            if ( pCur == &(m_atLinkArray[dwIdx]) )
            {
                return (dwIdx+1);
            }
        } 
        return 0;
    }
    ArrayNode<T> * GetLinkById( u32 dwId )
    {
        if ( 0 == dwId || dwId > sizeof(m_atLinkArray)/sizeof(m_atLinkArray[0]) )
        {
            return NULL;
        }
        ArrayNode<T> * pCur = &(m_atLinkArray[dwId-1]);
        return pCur;
    }

    u32     GetHeadId( void ) const { return ntohl(m_dwHeadId); }
    void    SetHeadId( u32 dwHeadId ) { m_dwHeadId = htonl(dwHeadId); }

    u32     GetTailId( void ) const { return ntohl(m_dwTailId); }
    void    SetTailId( u32 dwTailId ) { m_dwTailId = htonl(dwTailId); }

    // 获得空闲存储空间ID
    u32 GetIdleId(void)
    {
        u32 dwArraySize = sizeof(m_atLinkArray)/sizeof(m_atLinkArray[0]);
        u32 dwIdx = 0;
        for ( dwIdx = 0; dwIdx < dwArraySize; dwIdx++ )
        {
            if ( NULL == m_atLinkArray[dwIdx].GetNext() &&
                GetTailId() != (dwIdx+1) )
            {
                return (dwIdx+1);
            }
        }
        return 0;
    }
     // 返回线性表指向第p个元素的指针值
    ArrayNode<T> * GetLink(const u32 &dwPos)        
    {
        if ( dwPos < 0 || dwPos >= Length() )
        {
            return NULL;
        }
        
        u32 dwCount = 0;
        ArrayNode<T> *pCur = GetLinkById( GetHeadId() );
        while ( pCur != NULL && dwCount < dwPos )
        {
            u32 dwNextId = pCur->GetNext();
            pCur = GetLinkById(dwNextId);
            dwCount++;
        }       
        return pCur;
    }   

public: 
    LinkArray() { Clear(); }         // 构造函数
    ~LinkArray() { Clear(); }        // 析构函数
    // 判断链表是否为空
    BOOL32 IsEmpty() const { return ( 0 == m_dwHeadId || 0 == m_dwTailId ); }
    // 将链表存储的内容清除，成为空表
    void   Clear()                                    
    {
        m_dwHeadId = 0;
        m_dwTailId = 0;
        u32 dwArraySize = sizeof(m_atLinkArray)/sizeof(m_atLinkArray[0]);
        u32 dwIdx = 0;
        for ( dwIdx = 0; dwIdx < dwArraySize; dwIdx++ )
        {
            m_atLinkArray[dwIdx].SetNext( 0 );
        }
    }
    // 返回此顺序表的当前实际长度
    u32 Length() 
    {
        ArrayNode<T> *pCur = GetLinkById( GetHeadId() );
        u32 dwCount = 0;
        
        while ( pCur != NULL ) 
        {
            u32 dwNextId = pCur->GetNext();
            pCur = GetLinkById(dwNextId);
            dwCount++;
        }
        return dwCount;
    }    
    // 在表尾添加一个元素value，表的长度增1
    BOOL32 Append(const T &Value)                      
    {
        u32 dwIdleId = GetIdleId();
        ArrayNode<T> *pCur = GetLinkById( dwIdleId );
        if ( NULL == pCur )
        {
            return FALSE;
        }       
        pCur->SetData( Value );
        pCur->SetNext( 0 );
        
        u32 dwOldTailId = GetTailId();
        SetTailId(dwIdleId);

        ArrayNode<T> * pTail = GetLinkById( dwOldTailId );
        if ( NULL != pTail )
        {
            pTail->SetNext( dwIdleId );
        }
        else
        {
            SetHeadId(dwIdleId);
        }        
        return TRUE;
    }
    // 在第dwPos个位置插入数据内容为Value的新结点
    BOOL32 Insert(const u32 &dwPos, const T &Value)    
    {
        ArrayNode<T> *pCur = NULL;
        ArrayNode<T> *pNext = NULL;
        
        u32 dwIdleId = GetIdleId();
        pNext = GetLinkById( dwIdleId ); 
        if ( NULL == pNext )
        {
            return FALSE;
        }
        
        if ( dwPos == 0 )
        {
            pNext->SetData( Value );
            pNext->SetNext( GetHeadId() );
            SetHeadId( dwIdleId );
            if ( 0 == GetTailId() )
            {
                // 原来的表为空
                SetTailId( dwIdleId );
            }
        }
        else 
        {
            if ( (pCur = GetLink(dwPos-1) ) == NULL) 
            {                                                     
                return FALSE;
            }
            pNext->SetData( Value );
            pNext->SetNext( pCur->GetNext() );
            pCur->SetNext( dwIdleId );
            u32 dwPreId = GetIdByLink(pCur);
            if ( dwPreId == GetTailId() )                                          
            {
                SetTailId( dwIdleId );
            }
        }    
        return TRUE;
    }
    // 删除位置i上的元素，表的长度减 1
    BOOL32 Delete(const u32 &dwPos)                    
    {
        ArrayNode<T> *pCur = NULL;
        ArrayNode<T> *pPre = NULL; 
        
        pCur = GetLink( dwPos );
        if ( NULL == pCur )
        {
            return FALSE;
        }
        if ( dwPos > 0 )
        {
            pPre = GetLink( dwPos-1 );
            if ( NULL == pPre )
            {
                return FALSE;
            }
        }

        u32 dwNext = pCur->GetNext();
        pCur->SetNext( 0 );
        if ( NULL == pPre )
        {                           
            SetHeadId( dwNext );                  
        }
        else
        {
            pPre->SetNext( dwNext );         
        }
        if ( GetTailId() == GetIdByLink(pCur) )
        {
            if ( dwNext != 0 )
            {
                SetTailId( dwNext );
            }
            else
            {
                SetTailId( GetIdByLink(pPre) );
            }
        } 
        return TRUE;
    }

    // 返回位置i的元素值 
    T * GetValue(const u32 &dwPos)            
    {
        ArrayNode<T> *p = GetLink(dwPos);
        if ( p == NULL )
        {
            return NULL;
        }
        else
        {
            return p->GetData();
        }
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TNetAdaptInfo
{
protected:
    s8  m_achAdapterName[MCU_MAX_ADAPTER_NAME_LENGTH + 4];
    s8  m_achDescription[MCU_MAX_ADAPTER_DESCRIPTION_LENGTH + 4];
    u32 m_dwMacAddressLength;
    u8  m_abyMacAddress[MCU_MAX_ADAPTER_ADDRESS_LENGTH];
    u32 m_dwIndex;
    u32 m_dwType;
	u32 m_dwDefGWIpAddr;	// [3/25/2010 xliang] 该网口默认网关ip地址
    TNetParam m_tCurrentIpAddress;

public:
    LinkArray<TNetParam, MCU_MAXNUM_ADAPTER_IP> m_tLinkIpAddr;          // 本网卡IP地址
    LinkArray<TNetParam, MCU_MAXNUM_ADAPTER_GW> m_tLinkDefaultGateway;  // 本网卡默认网关

protected:
    void SetMacAddressLength( u32 dwMacLength ) { m_dwMacAddressLength = htonl( dwMacLength ); }
public:
    void SetDefGWIpAddr(u32 dwDefIpAddr) { m_dwDefGWIpAddr = htonl(dwDefIpAddr); }
	u32  GetDefGWIpAddr() {return ntohl(m_dwDefGWIpAddr); }

public:
    
    TNetAdaptInfo(void) { Clear(); }
    ~TNetAdaptInfo(void) { Clear(); }

    void Clear( void )
    {
        memset( m_achAdapterName, '\0', sizeof(m_achAdapterName) );
        memset( m_achDescription, '\0', sizeof(m_achDescription) );
        memset( m_abyMacAddress, 0, sizeof(m_abyMacAddress) );
        m_dwMacAddressLength = 0;
        m_dwIndex = 0;
        m_dwType = 0;
        
        m_tCurrentIpAddress.SetNull();
        m_tLinkIpAddr.Clear();
        m_tLinkDefaultGateway.Clear();
    }
    void Print( void )
    {
        u32 dwIdx = 0;
        StaticLog("AdapterName: %s\n", GetAdapterName() );
		printf( "AdapterName: %s\n", GetAdapterName() );
        StaticLog("Description: %s\n", GetDescription() );
        StaticLog("MacAddress : ");
		printf("MacAddress : ");
        for ( dwIdx = 0; dwIdx < GetMacAddressLength(); dwIdx++ )
        {
            if ( dwIdx < GetMacAddressLength()-1)
            {
                StaticLog("%02x-", m_abyMacAddress[dwIdx]);
				printf("%02x-", m_abyMacAddress[dwIdx]);
            }
            else
            {
                StaticLog("%02x\n", m_abyMacAddress[dwIdx]);
				printf( "%02x\n", m_abyMacAddress[dwIdx]);
            }
        }
        StaticLog("AdaptIdx: 0x%x\n", GetAdaptIdx() );
		printf("AdaptIdx: 0x%x\n", GetAdaptIdx() );
        StaticLog("AdaptType: %d ", GetAdaptType() );
        switch ( GetAdaptType() )    //适配器类型
        {
        case MCU_MIB_IF_TYPE_OTHER:
            StaticLog( "Other\n");
            break;
        case MCU_MIB_IF_TYPE_ETHERNET:
            StaticLog( "Ethernet\n");
            break;
        case MCU_MIB_IF_TYPE_TOKENRING:
            StaticLog( "Tokenring\n");
            break;
        case MCU_MIB_IF_TYPE_FDDI:
            StaticLog( "FDDI\n");
            break;            
        case MCU_MIB_IF_TYPE_PPP:
            StaticLog( "PPP\n");
            break;
        case MCU_MIB_IF_TYPE_LOOPBACK:
            StaticLog( "LoopBack\n");
            break;
        case MCU_MIB_IF_TYPE_SLIP:
            StaticLog( "Slip\n");
            break;    
        default:
            StaticLog( "Unknow\n");
            break;
        }
        StaticLog( "CurrentIpAddress: Ip - 0x%x, Mask - 0x%x\n", 
            m_tCurrentIpAddress.GetIpAddr(),
            m_tCurrentIpAddress.GetIpMask() );
            for ( dwIdx = 0; dwIdx < m_tLinkIpAddr.Length(); dwIdx++ )
            {
                StaticLog( "NetAddress[%d]: Ip - 0x%x, Mask - 0x%x\n", dwIdx,
                m_tLinkIpAddr.GetValue( dwIdx )->GetIpAddr(), 
                m_tLinkIpAddr.GetValue( dwIdx )->GetIpMask() );

				printf("NetAddress[%d]: Ip - 0x%x, Mask - 0x%x\n", dwIdx,
					m_tLinkIpAddr.GetValue( dwIdx )->GetIpAddr(), 
					m_tLinkIpAddr.GetValue( dwIdx )->GetIpMask() );
            }
            for ( dwIdx = 0; dwIdx < m_tLinkDefaultGateway.Length(); dwIdx++ )
            {
                StaticLog( "DefaultGW[%d]: Ip - 0x%x\n", dwIdx,
                    m_tLinkDefaultGateway.GetValue( dwIdx )->GetIpAddr() );
            }
    }

    // 网络适配器别名操作
    void SetAdapterName( s8 * pchAdapterName )
    {
        if ( NULL != pchAdapterName )
        {
            strncpy( m_achAdapterName, pchAdapterName, sizeof(m_achAdapterName)-4 );
            m_achAdapterName[sizeof(m_achAdapterName)-4] = '\0';
        }
        else
        {
            memset( m_achAdapterName, '\0', sizeof(m_achAdapterName) );
        }
        return;
    }
    s8 * GetAdapterName( void )
    {
        return m_achAdapterName; 
    }

    // 网络适配器MAC地址操作
    void SetMacAddress( u8 *pbyMacAddress, u32 dwLength )
    {
        if ( NULL != pbyMacAddress )
        {
            u8 *pbyMac = pbyMacAddress;
            u32 dwLop = 0;
            for ( dwLop = 0; dwLop < dwLength && dwLop < MCU_MAX_ADAPTER_ADDRESS_LENGTH; dwLop++ )
            {
                m_abyMacAddress[dwLop] = *pbyMac;
                pbyMac++;
            }
            SetMacAddressLength( dwLop );
        }
        else
        {
            memset( m_abyMacAddress, 0, sizeof(m_abyMacAddress) );
            SetMacAddressLength( 0 );
        }
        return;
    }
    u8 * GetMacAddress( void ) { return m_abyMacAddress; }

    u32  GetMacAddressLength( void ) const { return ntohl(m_dwMacAddressLength); }

    // 网络适配器描述操作
    void SetDescription( s8 * pchDescription )
    {
        if ( NULL != pchDescription )
        {
            strncpy( m_achDescription, pchDescription, sizeof(m_achDescription)-4 );
            m_achDescription[sizeof(m_achDescription)-4] = '\0';
        }
        else
        {
            memset( m_achDescription, '\0', sizeof(m_achDescription) );
        }
        return;
    }
    s8 * GetDescription( void ) { return m_achDescription; }

    // 网络适配器索引号操作
    void SetAdaptIdx( u32 dwIdx ) {m_dwIndex = htonl(dwIdx);}
    u32  GetAdaptIdx( void ) const { return ntohl(m_dwIndex); }

    // 网络适配器类型
    void SetAdaptType( u32 dwType ) { m_dwType = htonl(dwType); }
    u32  GetAdaptType( void ) const { return ntohl(m_dwType); }

    void SetCurretnIpAddr( const TNetParam &tCurIp ) 
    {
        m_tCurrentIpAddress = tCurIp; 
    }
    TNetParam GetCurrentIpAddr(void) const 
    { 
        return m_tCurrentIpAddress; 
    }

    s32 GetIpAddrPos( u32 dwIpAddr )
    {
        u32 dwIpNum = m_tLinkIpAddr.Length();
        for ( u32 dwIdx = 0; dwIdx < dwIpNum; dwIdx++ )
        {
            TNetParam tLinkParam = *(m_tLinkIpAddr.GetValue(dwIdx) );
            if ( tLinkParam.GetIpAddr() == dwIpAddr )
            {
                return dwIdx;
            }
        }
        return -1;
    }

    s32 GetDefaultGWPos( u32 dwGWAddr )
    {
        u32 dwIpNum = m_tLinkDefaultGateway.Length();
        for ( u32 dwIdx = 0; dwIdx < dwGWAddr; dwIdx++ )
        {
            TNetParam tLinkParam = *(m_tLinkDefaultGateway.GetValue(dwIdx) );
            if ( tLinkParam.GetIpAddr() == dwGWAddr )
            {
                return dwIdx;
            }
        }
        return -1;
    }
    
    TNetAdaptInfo & operator = (TNetAdaptInfo tNetAdapt)
    {
        if ( this != &tNetAdapt )
        {
            this->Clear();
            this->SetAdapterName( tNetAdapt.GetAdapterName() );
            this->SetAdaptIdx( tNetAdapt.GetAdaptIdx() );
            this->SetAdaptType( tNetAdapt.GetAdaptType() );
            this->SetCurretnIpAddr( tNetAdapt.GetCurrentIpAddr() );
            this->SetDescription( tNetAdapt.GetDescription() );
            this->SetMacAddress( tNetAdapt.GetMacAddress(), tNetAdapt.GetMacAddressLength() );
			this->SetDefGWIpAddr(tNetAdapt.GetDefGWIpAddr());
            u32 dwLop = 0;
            u32 dwArrayLen = 0;
            // Ip array
            TNetParam *ptNetAddr = NULL;
            dwArrayLen = tNetAdapt.m_tLinkIpAddr.Length();
            for ( dwLop = 0; dwLop < dwArrayLen; dwLop++ )
            {
                ptNetAddr = tNetAdapt.m_tLinkIpAddr.GetValue(dwLop);
                if ( NULL == ptNetAddr )
                {
                    continue;
                }
                if ( !m_tLinkIpAddr.Append( *ptNetAddr ) )
                {
                    break;
                }
            }
            // GW
            TNetParam *ptGW = NULL;
            dwArrayLen = tNetAdapt.m_tLinkDefaultGateway.Length();
            for ( dwLop = 0; dwLop < dwArrayLen; dwLop++ )
            {
                ptGW = tNetAdapt.m_tLinkDefaultGateway.GetValue(dwLop);
                if ( NULL == ptGW )
                {
                    continue;
                }
                if ( !m_tLinkDefaultGateway.Append( *ptGW ) )
                {
                    break;
                }
            }
        }
        return *this;
    }

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TNetAdaptInfoAll
{
public:
    LinkArray<TNetAdaptInfo, MCU_MAXNUM_ADAPTER> m_tLinkNetAdapt;
    
public:
    TNetAdaptInfoAll(void) { Clear(); }
    ~TNetAdaptInfoAll(void) { Clear(); }
    
    void Clear(void)
    {
        u32 dwAdaptNum = m_tLinkNetAdapt.Length();
        for ( u32 dwIdx = 0; dwIdx < dwAdaptNum; dwIdx++ )
        {
            TNetAdaptInfo * ptAdaptInfo = m_tLinkNetAdapt.GetValue(dwIdx);
            ptAdaptInfo->Clear();
        }
        m_tLinkNetAdapt.Clear();
    }
    
    TNetAdaptInfoAll & operator = (TNetAdaptInfoAll & tNetAdaptInfoAll)
    {
        if ( this != &tNetAdaptInfoAll )
        {
            this->Clear();
            u32 dwLop = 0;
            u32 dwArrayLen = tNetAdaptInfoAll.m_tLinkNetAdapt.Length();
            TNetAdaptInfo * ptNetAdapt = NULL;
            for ( dwLop = 0; dwLop < dwArrayLen; dwLop++ )
            {
                ptNetAdapt = tNetAdaptInfoAll.m_tLinkNetAdapt.GetValue( dwLop );
                if ( NULL == ptNetAdapt )
                {
                    continue;
                }
                if ( !this->m_tLinkNetAdapt.Append(*ptNetAdapt) )
                {
                    break;
                }
            }
        }
        return *this;
    }
    
    BOOL32 IsValid(void)
    {
        u32 dwAdapterNum = m_tLinkNetAdapt.Length();
        u32 dwIdx = 0;
        TNetAdaptInfo * ptAdapter = NULL;
        for ( dwIdx = 0; dwIdx < dwAdapterNum; dwIdx++ )
        {
            ptAdapter = m_tLinkNetAdapt.GetValue( dwIdx );
            if ( ptAdapter->m_tLinkIpAddr.Length() == 0 )
            {
                return FALSE;
            }
        }
        return TRUE;
    }

	 // 取和ETHID相等的网卡信息 
    TNetAdaptInfo *GetAdapterByEthId( u32 dwEthId) 
    { 
        TNetAdaptInfo * ptAdapter = NULL; 
        for ( u8 byLoop = 0; byLoop < m_tLinkNetAdapt.Length(); byLoop++) 
        { 
            ptAdapter = m_tLinkNetAdapt.GetValue(byLoop); 
            if ( ptAdapter != NULL && ptAdapter->GetAdaptIdx() == dwEthId) 
            { 
                return ptAdapter; 
            } 
        } 
        return NULL; 
    } 

    
    void Print(void)
    {
        u32 dwAdapterNum = m_tLinkNetAdapt.Length();
        u32 dwIdx = 0;
        TNetAdaptInfo * ptAdapter = NULL;
        for ( dwIdx = 0; dwIdx < dwAdapterNum; dwIdx++ )
        {
            ptAdapter = m_tLinkNetAdapt.GetValue( dwIdx );
            ptAdapter->Print();
        }
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//MCU 8000E 网卡配置及路由配置
struct TMcu8KECfg
{
protected:
	enUseModeType	m_enUseModeType;	// 3网口模式 || 备份模式
	enNetworkType	m_enNetworkType;    // LAN or WAN 
	BOOL32			m_bIsUseSip;		// 是否启用sip
	u8				m_bySipInEthIdx;	// SipIp在哪个网口上
	u32				m_dwSipIpAddr;		// SipIp
	u32				m_dwSipMaskAddr;	// SipMask
	u32				m_dwSipGwAddr;		// SipGw

public:
    LinkArray<TNetAdaptInfo, MCU_MAXNUM_ADAPTER> m_tLinkNetAdap;
    LinkArray<TRoute, MCU_MAXNUM_ADAPTER_ROUTE> m_tLinkRoute;        // 本机静态路由

public:
    TMcu8KECfg(void) { Clear(); }
    ~TMcu8KECfg(void) { Clear(); }

    void Clear(void)
    {
		memset(this, 0, sizeof(*this));
		m_enNetworkType = enLAN;			//默认设置为私网
		m_enUseModeType = MODETYPE_3ETH;	//默认设置为3网口模式

        u32 dwAdaptNum = m_tLinkNetAdap.Length();
        for ( u32 dAdaptwIdx = 0; dAdaptwIdx < dwAdaptNum; dAdaptwIdx++ )
        {
            TNetAdaptInfo * ptAdaptInfo = m_tLinkNetAdap.GetValue(dAdaptwIdx);
            ptAdaptInfo->Clear();
        }
        m_tLinkNetAdap.Clear();

        u32 dwRouteNum = m_tLinkRoute.Length();
        for ( u32 dwRouteIdx = 0; dwRouteIdx < dwRouteNum; dwRouteIdx++ )
        {
            TRoute * ptRoute = m_tLinkRoute.GetValue( dwRouteIdx );
            ptRoute->SetNull();
        }
        m_tLinkRoute.Clear();
    }

	//模式设置
	void SetUseModeType (enUseModeType enType)
	{
		m_enUseModeType = enType;	
	}
	
	enUseModeType GetUseModeType( void ) const
	{
		return  m_enUseModeType;
	}
	
	// 设置网络类型
	void SetNetworkType(enNetworkType enType) 
    {		
		m_enNetworkType = enType;		
	}
	
	// 获取网络类型
    enNetworkType GetNetworkType() const
    {
        return m_enNetworkType;
    }
	
    BOOL32 IsWan() const
    {
        return (m_enNetworkType == enWAN);
    }
	
    BOOL32 IsLan() const
    {
        return (m_enNetworkType == enLAN);
    }
	
	BOOL32 IsLanAndWan() const
    {
        return (m_enNetworkType == enBOTH);
    }
	
	void SetIsUseSip ( BOOL32 bIsUseSip )
	{
		m_bIsUseSip = bIsUseSip;
	}
	
	BOOL32 IsUseSip ( void ) const
	{
		return m_bIsUseSip;
	}
	
	void SetSipInEthIdx( u8 bySipInEthIdx )
	{
		m_bySipInEthIdx = bySipInEthIdx;
	}
	
	u8 GetSipInEthIdx ( void ) const
	{
		return m_bySipInEthIdx;
	}
	
	//SipIp设置
	void SetSipIpAddr(u32 dwSipIpAddr) { m_dwSipIpAddr = htonl(dwSipIpAddr); }
	u32  GetSipIpAddr()const {return ntohl(m_dwSipIpAddr); }
	
	void SetSipMaskAddr(u32 dwSipMaskAddr) { m_dwSipMaskAddr = htonl(dwSipMaskAddr); }
	u32  GetSipMaskAddr()const {return ntohl(m_dwSipMaskAddr); }
	
	void SetSipGwAddr(u32 dwSipGwAddr) { m_dwSipGwAddr = htonl(dwSipGwAddr); }
	u32  GetSipGwAddr()const {return ntohl(m_dwSipGwAddr); }

    TMcu8KECfg & operator = (TMcu8KECfg & tMcuEqp)
    {
        if ( this != &tMcuEqp )
        {
            this->Clear();
            u32 dwLop = 0;
            u32 dwArrayLen = tMcuEqp.m_tLinkNetAdap.Length();
            TNetAdaptInfo tNetAdapt;
            for ( dwLop = 0; dwLop < dwArrayLen; dwLop++ )
            {
                tNetAdapt = *(tMcuEqp.m_tLinkNetAdap.GetValue( dwLop ));
                if ( !this->m_tLinkNetAdap.Append(tNetAdapt) )
                {
                    break;
                }
            }

            u32 dwRouteNum = tMcuEqp.m_tLinkRoute.Length();
            for ( u32 dwRouteIdx = 0; dwRouteIdx < dwRouteNum; dwRouteIdx++ )
            {
                TRoute tRoute = *(tMcuEqp.m_tLinkRoute.GetValue( dwRouteIdx ));
                if ( !this->m_tLinkRoute.Append(tRoute))
                {
                    break;
                }
            }

			this->SetUseModeType(tMcuEqp.GetUseModeType());
			this->SetNetworkType(tMcuEqp.GetNetworkType());
			this->SetIsUseSip(tMcuEqp.IsUseSip());
			this->SetSipInEthIdx(tMcuEqp.GetSipInEthIdx());
			this->SetSipIpAddr(tMcuEqp.GetSipIpAddr());
			this->SetSipMaskAddr(tMcuEqp.GetSipMaskAddr());
			this->SetSipGwAddr(tMcuEqp.GetSipGwAddr());
        }
        return *this;
    }
    
    BOOL32 IsValid(void)
    {
        u32 dwAdapterNum = m_tLinkNetAdap.Length();
        u32 dwIdx = 0;
        TNetAdaptInfo * ptAdapter = NULL;
        for ( dwIdx = 0; dwIdx < dwAdapterNum; dwIdx++ )
        {
            ptAdapter = m_tLinkNetAdap.GetValue( dwIdx );
            if ( ptAdapter->m_tLinkIpAddr.Length() == 0 )
            {
                return FALSE;
            }
        }
        return TRUE;
    }
    
    void Print(void)
    {
        u32 dwAdapterNum = m_tLinkNetAdap.Length();
        u32 dwIdx = 0;
        TNetAdaptInfo * ptAdapter = NULL;
        for ( dwIdx = 0; dwIdx < dwAdapterNum; dwIdx++ )
        {
            ptAdapter = m_tLinkNetAdap.GetValue( dwIdx );
            ptAdapter->Print();
        }

        u32 dwRouteNum = m_tLinkRoute.Length();
        for ( u32 dwRouteIdx = 0; dwRouteIdx < dwRouteNum; dwRouteIdx++ )
        {
            TRoute * ptRoute = m_tLinkRoute.GetValue( dwRouteIdx );
            StaticLog( "0x%x mask 0x%x gateway 0x%x metric %d\n", 
                                    ptRoute->GetDstNet().GetIpAddr(),
                                    ptRoute->GetDstNet().GetIpMask(),
                                    ptRoute->GetGateway(),
									ptRoute->GetMetric() );

			s8	achSipIpAddr[32] = {0};
			s8	achSipMaskAddr[32] = {0};
			s8	achSipGwAddr[32] = {0};
			u32 dwSipIpAddr = GetSipIpAddr();
			u32 dwSipMaskAddr = GetSipMaskAddr();
			u32 dwSipGwAddr = GetSipGwAddr();
			sprintf(achSipIpAddr, "%d.%d.%d.%d%c", (dwSipIpAddr>>24)&0xFF, (dwSipIpAddr>>16)&0xFF, (dwSipIpAddr>>8)&0xFF, dwSipIpAddr&0xFF, 0);
			sprintf(achSipMaskAddr, "%d.%d.%d.%d%c", (dwSipMaskAddr>>24)&0xFF, (dwSipMaskAddr>>16)&0xFF, (dwSipMaskAddr>>8)&0xFF, dwSipMaskAddr&0xFF, 0);
			sprintf(achSipGwAddr, "%d.%d.%d.%d%c", (dwSipGwAddr>>24)&0xFF, (dwSipGwAddr>>16)&0xFF, (dwSipGwAddr>>8)&0xFF, dwSipGwAddr&0xFF, 0);
			
			StaticLog("\n[TMcuNetCfg]:\nm_enUseModeType:%s\nm_enNetworkType:%s	\
				\nm_dwSipIpAddr:%s\nm_dwSipMakAddr:%s\nm_dwSipgwAddr:%s\n",	\
				(m_enUseModeType == MODETYPE_3ETH ? "MODETYPE_3ETH":"MODETYPE_BAK"),	\
				(m_enNetworkType == enLAN ? "enLAN":(m_enNetworkType == enWAN ? "enWAN":"enBOTH")),	\
				 achSipIpAddr,achSipMaskAddr,achSipGwAddr);
        }
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//add by zhanghb for 8000e 090409
struct TLicenseMsg
{
	u8      m_achLocalInfo[512];    //guard读出的local信息，包括MAC，HDD(密文)
	s8      m_achExpireDate[16];    //截至日期
	u8      m_achLicenseSN[64];      //序列号
	u16     m_wMcuAccessNum;        //mcu接入数量
	u16     m_wMcuPcmtAccNum;       //mcu中允许接入的PCMT的数量
	u16	    m_wLicenseErrorCode;	//License错误码
	BOOL32  m_bLicenseIsValid;      //License是否合法
	
public:
    TLicenseMsg()
    {
        memset( this, 0, sizeof(TLicenseMsg) );
    }
	
    TKdvTime GetExpireDate(void)
    {
        TKdvTime tDate;
        s8 * pchToken = strtok( m_achExpireDate, "-" );
        if( NULL != pchToken)
        {
            tDate.SetYear( atoi( pchToken ) );
            pchToken = strtok( NULL, "-" );
            if( NULL != pchToken )
            {
                tDate.SetMonth( atoi( pchToken ) );
                pchToken = strtok( NULL, "-" );
                if( NULL != pchToken )
                {
                    tDate.SetDay( atoi( pchToken ) );
                }
            }
        }
        return tDate;	
    }
	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMcuBrdEthParam{
    u32 dwIpAdrs;/*网络字节序*/
    u32 dwIpMask; /*网络字节序*/
    u8  byMacAdrs[6];
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
/* 以太网参数结构 */
struct TMcuBrdEthParamAll{
    u32 dwIpNum;/*有效的IP地址数*/
    TMcuBrdEthParam atBrdEthParam[MCU_MAXNUM_ADAPTER];/*存放IP地址等信息的数组*/
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TSwitchDstInfo
{
	TMt				m_tSrcMt;			//源
	u8				m_bySrcChnnl;		//源通道号（对于终端目前只填0，对于外设有多种选择）
	u8				m_byMode;			//V/A/BOTH
	TTransportAddr	m_tDstVidAddr;		//目的视频的ip和port
	TTransportAddr  m_tDstAudAddr;		//目的音频的ip和port （目前Vid和Aud的Ip是相同的，仅port不同）
	u32				m_dwReserved;		//预留
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// [9/27/2011 liuxu] 模板类静态缓冲区定义
template <class T, u32 dwBufNum >
class CStaticBuf
{
public:
	CStaticBuf( ) { ClearAll(); }
	
public:
	// 全部清空
	void ClearAll( ) { memset(this, 0, sizeof( CStaticBuf<T, dwBufNum> )); }
	
	// 缓冲区总容量
	u32 GetCapacity() const { return dwBufNum; }
	// 已使用数
	u32	GetUsedNum() const { return m_dwNum; }
	// 是否已满
	BOOL32 IsFull() const { return GetUsedNum() >= GetCapacity(); }
	
	// 获取一个元素
	BOOL32 Get( u32 dwIdx,  T& ) const ;
	// 添加一个元属. 不支持多重添加.
	BOOL Add( const T& tMt );
	// 根据元素值删除一个元素
	BOOL32 Delete( const T& TMt );
	// 删除指定位置的元素
	BOOL32 Clear( const u32 dwIdx );
	// 查找值为t的元素的位置, 若查找不到,返回dwBufNum
	u32	Find( const T& t ) const ;
	
protected:
	// 判断指定位置元素是否为空
	BOOL32	IsNull( const u32 dwIdx ) const;
	// 设置指定位置元素为空
	void	SetNull( const u32 dwIdx, const BOOL32 bNull );
	// 判断索引是否为合法值
	BOOL32	IsValidIdx( const u32 dwIdx ) const { return dwIdx < dwBufNum; }
	
private:
	T			m_Buf[dwBufNum];						// 缓冲区
	u8			m_abyFlag[(dwBufNum + 7) / 8 ];			// 是否为空的标志位
	u32			m_dwNum;								// 已使用的总数
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


template <class T, u32 dwBufNum >
BOOL32 CStaticBuf<T, dwBufNum>::Get( u32 dwIdx,  T& t ) const 
{
	// 索引非法或指定位置为空, 返回false
	if ( !IsValidIdx(dwIdx) || IsNull(dwIdx) )
	{
		return FALSE;
	}

	t = m_Buf[dwIdx];
	return TRUE;
}

template <class T, u32 dwBufNum >
BOOL CStaticBuf<T, dwBufNum>::Add( const T& t )
{
	// 不支持多重添加, 所以如果已经添加进入的, 则覆盖
	u32 dwIdx = Find(t);
	if (IsValidIdx(dwIdx))
	{
		// 已经存在, 则覆盖
		m_Buf[dwIdx] = t;
		SetNull(dwIdx, FALSE);
		return TRUE;
	}

	// 未查找到,则插入第一个空闲位置
	for(u32 dwLoop = 0; dwLoop < dwBufNum; dwLoop++)
	{
		// 插入第一个空闲的位置
		if (IsNull(dwLoop))
		{
			m_Buf[dwLoop] = t;
			SetNull(dwLoop, FALSE);
			
			if( m_dwNum < dwBufNum ) m_dwNum++;

			return TRUE;
		}
	}

	// 缓冲区已满
	return FALSE;
}


template <class T, u32 dwBufNum >
BOOL CStaticBuf<T, dwBufNum>::Delete( const T& t )
{
	// 先查找
	u32 dwIdx = Find(t);

	return Clear(dwIdx);
}

template <class T, u32 dwBufNum >
BOOL CStaticBuf<T, dwBufNum>::Clear( const u32 dwIdx )
{
	// 判断是否非法, 是否为空
	if (IsValidIdx(dwIdx) && !IsNull(dwIdx))
	{
		memset(&m_Buf[dwIdx], 0, sizeof(T) );
		SetNull(dwIdx, TRUE);

		if( m_dwNum ) m_dwNum-- ;
		
		return TRUE;
	}

	return FALSE;
}

template <class T, u32 dwBufNum >
u32 CStaticBuf<T, dwBufNum>::Find( const T& t) const 
{
	for(u32 dwLoop = 0; dwLoop < dwBufNum; dwLoop++)
	{
		// 非空且值相等
		if (!IsNull(dwLoop) && m_Buf[dwLoop] == t)
		{
			return dwLoop;
		}
	}

	return dwBufNum;
}

template <class T, u32 dwBufNum >
BOOL CStaticBuf<T, dwBufNum>::IsNull( const u32 dwIdx ) const 
{
	// 索引值合法
	if (!IsValidIdx(dwIdx))
	{
		return TRUE;
	}

	// 计算在m_abyFlag的bit位置
	const u32 dwBytePos = dwIdx / 8;		// 所在m_abyFlag中byte数
	const u8  byBitPos = (u8)(dwIdx % 8);			// 所在m_abyFlag中第dwBytePos个byte的bit
	
	return !(m_abyFlag[dwBytePos] & (1 << byBitPos));
}

template <class T, u32 dwBufNum >
void CStaticBuf<T, dwBufNum>::SetNull( const u32 dwIdx, const BOOL32 bNull )
{
	if (!IsValidIdx(dwIdx))
	{
		return;
	}

	// 计算在m_abyFlag的bit位置
	const u32 dwBytePos = dwIdx / 8;		// 所在m_abyFlag中byte数
	const u8  byBitPos = (u8)(dwIdx % 8);		// 所在m_abyFlag中第dwBytePos个byte的bit
	
	if (bNull)
	{
		m_abyFlag[dwBytePos] &= ~(1 << byBitPos);
	}else
	{
		m_abyFlag[dwBytePos] |= (1 << byBitPos);
	}
}

//static func
#include "evmcumcs.h"
/*====================================================================
    函数名      ：HandleMtListNtf
    功能        ：解析MCU_MCS_MTLIST_NOTIF
    算法实现    ：
    引用全局变量：
    输入参数说明：const CServMsg &cSerMsg, 传入的消息 
				  TMtExtU *atMtExtU, 保存终端列表
				  u8 &byMtNum,　终端数量
				  TMcu &tMcu，　所属MCU
    返回值说明  ：true: 成功　　false: 失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2013/6/27   4.7.2       彭国锋          创建
====================================================================*/
static bool HandleMtListNtf(const CServMsg &cSerMsg, TMtExtU *atMtExtU, u8 &byMtNum, TMcu &tMcu)
{
	if ( cSerMsg.GetMsgBodyLen() < sizeof(TMcu) )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[HandleMtListNtf]msg len <  sizeof TMcu\n");
		return false;
	}

	tMcu = *(TMcu *)cSerMsg.GetMsgBody();
	// by zoujunlong 原有mcu这个eqpid不会填写，所以可以区分是新老mcu，新mcu只有本地的终端才会上报扩展别名。
	byMtNum = tMcu.GetEqpId();
	u8* achAlias = NULL;

	if ( byMtNum == 0/*cSerMsg.GetMsgBodyLen() > sizeof(TMcu)*/ )
	{
		byMtNum = (cSerMsg.GetMsgBodyLen() - sizeof(TMcu) )/ sizeof(TMtExt);
	}
	else
	{
		achAlias = cSerMsg.GetMsgBody() + sizeof(TMcu) + sizeof(TMtExt) * byMtNum;
	}

	if ( byMtNum == 0 )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[HandleMtListNtf] byMtNum == 0\n");
		return false;
	}

	u8 byMtId = 0;

	TMtExt *ptMtExt = (TMtExt *)(cSerMsg.GetMsgBody() + sizeof(TMcu));
	while(byMtId < byMtNum)
	{
		if ( ptMtExt )
		{
			// 前XX个字节是完全相同的，直接拷贝 [pengguofeng 6/27/2013]
			memcpy(&atMtExtU[byMtId], ptMtExt, sizeof(TMt) + sizeof(u8)*2+sizeof(u16) + sizeof(u32)*2);
			atMtExtU[byMtId].SetProtocolType(ptMtExt->GetProtocolType());
			if ( achAlias != NULL )
			{
				atMtExtU[byMtId].SetAlias( (s8*)(&achAlias[(VALIDLEN_ALIAS + MAXLEN_CONFNAME) * byMtId]) );
			}
			else
			{
				atMtExtU[byMtId].SetAlias(ptMtExt->GetAlias());
			}
		}
		byMtId++;
		ptMtExt++;
	}

	return true;
}

// 由于第一个字节是表示成员数，所以一次只允许发一个SMCU的列表 [pengguofeng 6/27/2013]
// u8 byMtNum + u16 wMcuIdx + u8 byHasHeadInfo + [ u32 dwHeadLen + u8 byMemNum + byMemNum * u16 MemLen +] byMtNum* TMtExt_U
/*====================================================================
    函数名      ：HandleMtListNtfUtf8
    功能        ：解析MCU_MCS_GETMTLIST_NOTIF消息，赋给传入参数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CServMsg &cSerMsg, 传入的消息 
				　TMtExtU *atMtExtU,　成员列表信息
				　u8 &byMtNum，　终端数量
				　TMcu &tMcu，　所属MCU
    返回值说明  ：true: 解析成功  false: 解析失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2013/6/27   4.7.2       彭国锋          创建
====================================================================*/
static bool HandleMtListNtfUtf8(const CServMsg &cSerMsg, TMtExtU *atMtExtU, u8 &byMtNum, TMcu &tMcu)
{
	if ( cSerMsg.GetMsgBodyLen() < sizeof(u8)*2 + sizeof(u16) )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[HandleMtListNtfUtf8]msg len < 4\n");
		return false;
	}

	u8 *pMsg = cSerMsg.GetMsgBody();
	byMtNum = *(u8 *)pMsg;
	pMsg += sizeof(u8);
/*
	if ( byMtNum == 0 )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[HandleMtListNtfUtf8]byMtNum==0\n");
		// Bug00147556:下级终端数为0,也要把tMcu赋上 [pengguofeng 7/10/2013]
		return false;
	}
*/
	u16 wMcuIdx = *(u16 *)pMsg;
	pMsg += sizeof(u16);
	wMcuIdx = ntohs(wMcuIdx);

	tMcu.SetNull();
	tMcu.SetMcuIdx(wMcuIdx);

	if ( byMtNum == 0 )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[HandleMtListNtfUtf8]byMtNum==0\n");
		// Bug00147556:下级终端数为0,也要把tMcu赋上 [pengguofeng 7/10/2013]
		return false;
	}

	u8 byHasHeadInfo = *(u8 *)pMsg;
	pMsg += sizeof(u8);
	if ( byHasHeadInfo == 0 )
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CONF,"[HandleMtListNtfUtf8] no head info\n");
		return false;
	}

	if ( cSerMsg.GetMsgBodyLen() < sizeof(u8)*2 + sizeof(u16) + sizeof(u32) )
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CONF,"[HandleMtListNtfUtf8] MsgLen get invalid HeadLen\n");
		return false;
	}

	u32 dwHeadLen = *(u32 *)pMsg;
	pMsg += sizeof(u32);

	dwHeadLen = ntohl(dwHeadLen);

	if ( dwHeadLen < sizeof(u8) )
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CONF,"[HandleMtListNtfUtf8] head len is wrong:%d\n", dwHeadLen);
		return false;
	}

	if ( cSerMsg.GetMsgBodyLen() < sizeof(u8)*3 + sizeof(u16) + sizeof(u32) )
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CONF,"[HandleMtListNtfUtf8] MsgLen get invalid MemNum \n");
		return false;
	}

	u8 byMemNum = *(u8 *)pMsg; //消息结构体内的结构成员数目
	pMsg += sizeof(u8);

	if ( dwHeadLen < sizeof(u8) + byMemNum * sizeof(u16) )
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CONF,"[HandleMtListNtfUtf8] head len is wrong:%d byMemNum:%d\n", dwHeadLen, byMemNum);
		return false;
	}

	if ( cSerMsg.GetMsgBodyLen() < sizeof(u8)*3 + sizeof(u16) + sizeof(u32) + byMemNum * sizeof(u16) )
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CONF,"[HandleMtListNtfUtf8] MsgLen get invalid MemLen\n");
		return false;
	}

	u16 *pawLen = (u16 *)OspAllocMem(byMemNum * sizeof(u16)); //new u16[byMemNum]; //当前消息里各成员的长度
	memset(pawLen, 0, sizeof(u16)*byMemNum);

	u8 byIdx = 0;
	u16 wLen = 0;
	u16 wMsgStructLen = 0; //消息里的结构体总长度,要和sizeof(TMtExtU)作比较，取小
	while (byIdx < byMemNum)
	{
		wLen = *(u16 *)pMsg;
		pMsg += sizeof(u16);
		wLen = ntohs(wLen);
		wMsgStructLen += wLen;
		memcpy(&pawLen[byIdx], &wLen, sizeof(u16));
		byIdx++;
	}

	if ( cSerMsg.GetMsgBodyLen() < sizeof(u8)*3 + sizeof(u16) + sizeof(u32) + byMemNum * sizeof(u16) + byMtNum * wMsgStructLen )
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CONF,"[HandleMtListNtfUtf8] MsgLen get invalid MemStruct\n");
		return false;
	}

	u8 *pStruct = (u8*)atMtExtU;

	LogPrint(LOG_LVL_WARNING,MID_MCU_CONF,"[HandleMtListNtfUtf8] wMsgStructLen :%d sizeof(TMtExtU):%d\n",
		wMsgStructLen, sizeof(TMtExtU));

	byIdx = 0; //byidx此时表示终端数目
	u8 byStructNum = atMtExtU[0].GetMemNum();
	u16 wCopyLen = 0;
	while (byIdx < byMtNum )
	{
		for (u8 byMemId = 1; byMemId <= byStructNum; byMemId++)
		{
			if ( byMemId > byMemNum )
			{
				//wMsgStructLen < sizeof(TMtExtU): 剩下的不用拷贝了，pStruct偏移
				for ( u8 byLeftLoop = byMemId; byLeftLoop <= byStructNum; byLeftLoop++ )
				{
					pStruct += atMtExtU[byIdx].GetMemLen(byLeftLoop);
				}
				break; //拷贝下一个atMtExtU[x]
			}
			else if ( byMemId == byStructNum && byMemId < byMemNum )
			{
				//wMsgStructLen > sizeof(TMtExtU) 拷完了，但是消息里面还有额外的成员
				for ( u8 byLeftLoop2 = byMemId; byLeftLoop2 <= byMemNum; byLeftLoop2++ )
				{
					pMsg += pawLen[byLeftLoop2-1];
				}
				break; //拷贝下一段pMsg
			}
			else
			{
				//相等就走下面
			}

			wCopyLen = atMtExtU[byIdx].GetMemLen(byMemId);
			if ( wCopyLen > pawLen[byMemId-1] )
			{
				wCopyLen = (u16)pawLen[byMemId-1];
			}

			memcpy(pStruct, pMsg, wCopyLen);

			pStruct += atMtExtU[byIdx].GetMemLen(byMemId);
			pMsg += (u16)pawLen[byMemId-1];
		}

		byIdx++;
	}

	//释放
	//delete pawLen;
	OspFreeMem(pawLen);

	return true;
}

/*====================================================================
    函数名      ：UnPackMsg
    功能        ：解析消息，目前主要处理2个
    算法实现    ：
    引用全局变量：
    输入参数说明：const CServMsg &cSerMsg, 传入的消息 
				　TMtExtU *atMtExtU,　终端列表
				　u8 &byMtNum, 终端数量
				　TMcu &tMcu　　所在mcu
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2013/6/27   4.7.2       彭国锋          创建
====================================================================*/
static bool UnPackMsg(const CServMsg &cSerMsg, TMtExtU *atMtExtU, u8 &byMtNum, TMcu &tMcu)
{
	switch ( cSerMsg.GetEventId() )
	{
	case MCU_MCS_MTLIST_NOTIF: 
		return HandleMtListNtf(cSerMsg, atMtExtU, byMtNum, tMcu);
	case MCU_MCS_GETMTLIST_NOTIF: 
		return HandleMtListNtfUtf8(cSerMsg, atMtExtU, byMtNum, tMcu);
	default:
		StaticLog("[UnPackMsg]unknow msg.%d(%s) recv\n", cSerMsg.GetEventId(), OspEventDesc(cSerMsg.GetEventId()));
		return false;
	}
}

/*==============================================================================
函数名    :  CorrectUtf8Str
功能      :  修正UTF8字符串,将出错的11xx xxxx 置0,其他的不变
注意	  :  只在UTF8下有效
算法实现  :  
参数说明  :  s8 *：可修改的字符串
			 u16：字符串长度（会被修改）
返回值说明:  True: 需要修正，且已修正好
			 False: 不需修正
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2013/05/23                 彭国锋                            创建
==============================================================================*/
static BOOL32 CorrectUtf8Str(s8 * pStr, const u16 wStrLen)
{
	if ( !pStr )
	{
		return FALSE;
	}

#ifdef _UTF8
	u8 byFollowLen = 0;   // utf8整字所包含的字节，除掉领头的11xx xxxx

	for ( u16 wLoop = wStrLen; wLoop > 0; wLoop--)
	{
		if ( pStr[wLoop-1] != '\0')
		{
			if (  (pStr[wLoop-1] & 0xfe ) == 0xfc )  // 1111 110x 后面跟5个10xx xxxx，下同
			{
				byFollowLen = 5;
			}
			else if ( (pStr[wLoop-1] & 0xfc ) == 0xf8 )  // 1111 10xx: 4
			{
				byFollowLen = 4;
			}
			else if ( (pStr[wLoop-1] & 0xf8 ) == 0xf0 )  // 1111 0xxx: 3
			{
				byFollowLen = 3;
			}
			else if ( (pStr[wLoop-1] & 0xf0 ) == 0xe0 )  // 1110 xxxx: 2
			{
				byFollowLen = 2;
			}
			else if ( (pStr[wLoop-1] & 0xe0 ) == 0xc0 )  // 110x xxxx: 1
			{
				byFollowLen = 1;
			}
			else
			{
				byFollowLen = 0;
			}

			if ( wLoop + byFollowLen > wStrLen ) //超过最大长度，肯定有截断
			{
				pStr[wLoop-1] = '\0';
				return TRUE;
			}
			
			for ( u8 byLoop = 0; byLoop < byFollowLen; byLoop++)
			{
				if ( pStr[wLoop+byLoop] == '\0' || (pStr[wLoop+byLoop] & 0xc0) != 0x80 ) // 等于 0 或者不是 10xx xxxx的格式，表示有截断
				{
					pStr[wLoop-1] = '\0';
					return TRUE;
				}
			}
		}
	}
#endif

	return FALSE;
}

/*==============================================================================
函数名    :  CorrectGBKStr
功能      :  修正GBK字符串,将出错的置0,其他的不变
注意	  :  只对GBK汉字下有效
算法实现  :  
参数说明  :  s8 *：可修改的字符串
			 u16：字符串长度（会被修改）
返回值说明:  True: 需要修正，且已修正好
			 False: 不需修正
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2013/07/16                 彭国锋                            创建
==============================================================================*/
static void CorrectGBKStr(s8 *pStr, const u16 wStrLen)
{
	s8 nLoop = wStrLen - 1;
	u8 byWideCharCount = 0;
	while ( (signed char)nLoop >= 0  && (signed char)0 > (signed char)pStr[nLoop])
	{
		byWideCharCount ++;
		nLoop --;
	}
	if ( byWideCharCount % 2 == 1 )
	{
		pStr[wStrLen-1] = '\0';
    }
}

#undef SETBITSTATUS
#undef GETBITSTATUS

#ifdef WIN32
#pragma pack( pop )
#endif

#endif /* __MCUSTRUCT_H_ */




