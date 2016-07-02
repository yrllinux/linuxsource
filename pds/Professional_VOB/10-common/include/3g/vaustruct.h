#ifndef _VAUSTRUCT_H_
#define _VAUSTRUCT_H_

#include "osp.h"
#include "vauconst.h"
//#include "kdmconst.h"
//#include "CULib_3g.h"

#ifdef WIN32
    #pragma pack( push )
    #pragma pack( 1 )
#endif

#define MAXLEN_DEVICE_MANUFACTURER 256
#define MAX_SPL_MSG_LEN					(0x7000)
struct TSplMsg{
	u8  m_abyBufffer[MAX_SPL_MSG_LEN];
	u32 m_dwMsgLen;
	TSplMsg():m_dwMsgLen(0)
	{
	}

	u16 SetMsgBody(const void* pMsg, u32 dwLen)
	{
		if(dwLen > MAX_MSG_LEN)
		{
			return -1;
		}
		memcpy(m_abyBufffer, pMsg, dwLen);
		m_dwMsgLen += dwLen;
		return 0;
	}

	u16 CatMsgBody(const void* pMsg, u32 dwLen)
	{
		if(dwLen + m_dwMsgLen> MAX_MSG_LEN)
		{
			return -1;
		}
		memcpy(m_abyBufffer + m_dwMsgLen, pMsg, dwLen);
		m_dwMsgLen += dwLen;
		return 0;
	}

	u8* GetMsg()
	{
		return m_abyBufffer;
	}

	u32 GetMsgLen()
	{
		return m_dwMsgLen;
	}
};
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
#define OSP_MSG_MAX_LEN		(1024*28)
//该结构用于daemon实例将rtspserver消息转发给其他实例时保存一些必要的信息
struct TRtspMsg{
	u32 m_dwSrcNode;
	u32 m_dwSrcId;
	u32 m_dwDstNode;
	u32 m_dwDstId;

	u32 m_dwMsgLen;//消息长度
    u8  m_abyBuffer[OSP_MSG_MAX_LEN];		//消息Buffer，28K
	
	TRtspMsg()
	{
	}

	TRtspMsg(const CMessage* const pcMsg)
	{
		m_dwSrcNode = pcMsg->srcnode;
		m_dwSrcId   = pcMsg->srcid;
		m_dwDstNode = pcMsg->dstnode;
		m_dwDstId   = pcMsg->dstid;

		m_dwMsgLen  = pcMsg->length;
		memcpy(m_abyBuffer, pcMsg->content, pcMsg->length);
		
	}

	TRtspMsg& operator = (const CMessage* const pcMsg)
	{
		m_dwSrcNode = pcMsg->srcnode;
		m_dwSrcId   = pcMsg->srcid;
		m_dwDstNode = pcMsg->dstnode;
		m_dwDstId   = pcMsg->dstid;

		m_dwMsgLen  = pcMsg->length;
		memcpy(m_abyBuffer, pcMsg->content, pcMsg->length);
		return *this;
	}

	TRtspMsg& operator =(const TRtspMsg& other)
	{
		m_dwSrcNode = other.m_dwSrcNode;
		m_dwSrcId   = other.m_dwSrcId;
		m_dwDstNode = other.m_dwDstNode;
		m_dwDstId   = other.m_dwDstId;

		m_dwMsgLen  = other.m_dwMsgLen;
		memcpy(m_abyBuffer, other.m_abyBuffer, other.m_dwMsgLen);
		return *this;
	}
};

#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//Setup信息
struct TSetupParam
{
protected:
	u32 m_dwLocalAddr;                 //本地的IP地址
	u16 m_wLocalPort;                  //本地端口
	u32 m_dwRemoteAddr;                //远端的IP地址
	u16 m_wRemotePort;                 //远端端口

	double m_dbStartTime;              //开始时间
	double m_dbStopTime;               //结束时间

	u16 m_wServerPortA;				   //发送RTP端口
	u16 m_wServerPortB;				   //发送RTCP端口
	u16 m_wClientPortA;                //远端收数据的RTP端口
	u16 m_wClientPortB;                //与RTP端口对应的RTCP端口

	u8 m_byTransportType;              //传输类型: 0-UDP 1-ReliableUDP 2-TCP
	u8 m_byTransportMode;              //传输模式: 1-Multicast 2-Unicast

    u16 m_wBitRate;                    //码率//+by lxx @20080324
	u8  m_byIndex;					   //第几路码流
		
public:
	TSetupParam()
	{
	}
	TSetupParam(const TSetupParam& other)
	{
		m_dwLocalAddr  = other.m_dwLocalAddr;
		m_wLocalPort   = other.m_wLocalPort;
		m_dwRemoteAddr = other.m_dwRemoteAddr;
		m_wRemotePort  = other.m_wRemotePort;

		m_dbStartTime  = other.m_dbStartTime;
		m_dbStopTime   = other.m_dbStopTime;
		m_wServerPortA = other.m_wServerPortA;
		m_wServerPortB = other.m_wServerPortB;
		m_wClientPortA = other.m_wClientPortA;
		m_wClientPortB = other.m_wClientPortB;

		m_byTransportType = other.m_byTransportType;
		m_byTransportMode = other.m_byTransportMode;

		m_wBitRate = other.m_wBitRate;
		m_byIndex  = other.m_byIndex;
	}

	TSetupParam& operator = (const TSetupParam& other)	
	{
		m_dwLocalAddr  = other.m_dwLocalAddr;
		m_wLocalPort   = other.m_wLocalPort;
		m_dwRemoteAddr = other.m_dwRemoteAddr;
		m_wRemotePort  = other.m_wRemotePort;

		m_dbStartTime  = other.m_dbStartTime;
		m_dbStopTime   = other.m_dbStopTime;
		m_wServerPortA = other.m_wServerPortA;
		m_wServerPortB = other.m_wServerPortB;
		m_wClientPortA = other.m_wClientPortA;
		m_wClientPortB = other.m_wClientPortB;

		m_byTransportType = other.m_byTransportType;
		m_byTransportMode = other.m_byTransportMode;

		m_wBitRate = other.m_wBitRate;
		m_byIndex  = other.m_byIndex;
		return *this;
	}
	
    void    SetLocalIpAddr( u32    dwLocalIP){ m_dwLocalAddr = (dwLocalIP);}
	u32     GetLocalIpAddr( )const{ return (m_dwLocalAddr); }
    void    SetLocalPort( u16    wLocalPort){ m_wLocalPort = wLocalPort;}
	u16     GetLocalPort( )const{ return m_wLocalPort; }	
    void    SetRemoteIpAddr( u32  dwRemoteIP){ m_dwRemoteAddr = (dwRemoteIP);}
	u32     GetRemoteIpAddr( )const{ return (m_dwRemoteAddr); }
    void    SetRemotePort( u16  wRemotePort){ m_wRemotePort = wRemotePort;}
	u16     GetRemotePort( )const{ return m_wRemotePort; }	
    void    SetStartTime( double  dbStartTime ){ m_dbStartTime = dbStartTime;}
	double  GetStartTime( )const{ return m_dbStartTime; }
    void    SetStopTime( double  dbStopTime ){ m_dbStopTime = dbStopTime;}
	double  GetStopTime( )const{ return m_dbStopTime; }
	void	SetServerPortA( u16	   wServerPortA){ m_wServerPortA = htons(wServerPortA);}
	u16		GetServerPortA( )const{ return ntohs(m_wServerPortA); }
	void	SetServerPortB( u16	   wServerPortB){ m_wServerPortB = htons(wServerPortB);}
	u16		GetServerPortB( )const{ return ntohs(m_wServerPortB); }
    void    SetClientPortA( u16    wClientPortA){ m_wClientPortA = htons(wClientPortA);}
	u16     GetClientPortA( )const{ return ntohs(m_wClientPortA); }	
    void    SetClientPortB( u16    wClientPortB){ m_wClientPortB = htons(wClientPortB);}
	u16     GetClientPortB( )const{ return ntohs(m_wClientPortB); }	
    void    SetTransportType( u8  byTransportType){ m_byTransportType = byTransportType;}
	u8      GetTransportType( )const{ return m_byTransportType; }
    void    SetTransportMode( u8  byTransportMode){ m_byTransportMode = byTransportMode;}
	u8      GetTransportMode( )const{ return m_byTransportMode; }
    void    SetBitRate(u16 wBitRate) {m_wBitRate = wBitRate;}//+by lxx @20080324
    u16     GetBitRate() const{return m_wBitRate;}//+by lxx @20080324
	void	SetIndex( u8	byIndex) {m_byIndex = byIndex;}
	u8		GetIndex() const { return m_byIndex;}//by zyq20090313
};
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

typedef struct tagAddr{
	u32 m_dwAddr;//地址，网络序
	u16 m_wPort;//端口，主机序
	tagAddr()
	{
	}
	tagAddr(const sockaddr_in& addrin)
	{
		m_dwAddr = *(u32*)&(addrin.sin_addr);
		m_wPort  = ntohs(addrin.sin_port);
	}
	tagAddr& operator = (const tagAddr& other)
	{
		m_dwAddr = other.m_dwAddr;
		m_wPort  = other.m_wPort;
		return *this;
	}
	
	bool operator == (const tagAddr& other)
	{
		return m_dwAddr == other.m_dwAddr && m_wPort == other.m_wPort;
	}

}TAddr;
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TAsfInfo{
	u32 m_dwMaxBitRate;
	u32 m_dwMaxPackSize;
	u32 m_dwPackCnt;
	u32 m_dwbyType;
	u32 m_dwDuration;
	u32 m_dwPreroll;
	u32 m_dwFileSize;
	u32 m_dwStreamCnt;
};
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
#define VAU_FMTP_MAX_SIZE (256)
#define VAU_RAW_HEADER_LEN (2048)


struct TSdpInfo
{
public:
	u8 m_byVideoType;
	u8 m_byAudioType;

	u16 m_wVideoBitRate;
	u32 m_wAudioBitRate;
	u32 m_wAudioSmpRate;

	u16 m_wVideoWidth;
	u16 m_wVideoHeight;

	u8	m_byAudioAlign;
	u8  m_bySecVideoType;
	u16 m_wSecVideoWidth;
	u16 m_wSecVideoHeight;
	s8  m_aszVideoCfg[VAU_FMTP_MAX_SIZE];
	s8	m_aszAudioCfg[VAU_FMTP_MAX_SIZE];
	s8  m_aszSecVidCfg[VAU_FMTP_MAX_SIZE];
	u32 m_dwVidCfgLen;
	u32 m_dwAudCfgLen;
	u32 m_dwSecVidCfgLen;

	//新加成员，发送asf包时使用
	s8  m_aszFileHeader[VAU_RAW_HEADER_LEN];
	TAsfInfo m_tAsfInfo;

};

#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
struct TStreamInfo{
	u8	 m_byVideo;
	u8   m_byMediaType;
	u16  m_wWtdth;
	u16  m_wHeight;
	u32  m_dwDuration;
	u32  m_dwBitRate;
};
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
#define VAU_MAX_STREAM_NUM (3)
struct TSdpMediaInfo
{
	//流的个数
	u32 m_byNum;
	//流的类型
	TStreamInfo m_atStreamInfo[VAU_MAX_STREAM_NUM];
};
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
#define MAXLEN_NAME 256
#define MAXLEN_PWD 256
struct TLoginReq
{
	char m_szUserName[MAXLEN_NAME + 1];
	char m_szPassword[MAXLEN_PWD + 1];

	bool operator == (const TLoginReq& other)
	{
		return strcmp(m_szUserName, other.m_szUserName) == 0;
	}

	TLoginReq()
	{
		m_szUserName[0] = 0;
		m_szPassword[0] = 0;
	}

	TLoginReq(const TLoginReq& other)
	{
		strcpy(m_szUserName, other.m_szUserName);
		strcpy(m_szPassword, other.m_szPassword);
	}

	TLoginReq& operator = (const TLoginReq& other)
	{
		strcpy(m_szUserName, other.m_szUserName);
		strcpy(m_szPassword, other.m_szPassword);
		return *this;
	}
};


#ifdef WIN32
#pragma pack( pop )
#endif

#endif // _VAUSTRUCT_H_
