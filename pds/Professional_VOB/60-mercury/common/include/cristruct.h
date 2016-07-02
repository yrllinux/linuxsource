/*****************************************************************************
模块名      : CRISTRUCT
文件名      : cristruct.h
相关文件    : 
文件实现功能: 接入转发内部结构
作者        : 周嘉麟
版本        : V0.1  Copyright(C) 2011-2012 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期         版本        修改人        修改内容
2012/05/10     1.0         周嘉麟        创建
******************************************************************************/
#ifndef __CRISTRUCT_H_
#define __CRISTRUCT_H_

#define IPV6_NAME_LEN		(u16)64			//IPv6地址名长度
#define MAXNUM_CONFIG_GK    (u16)10         //最大配置GK数
/*Ip映射表*/
struct TIpMap
{
public:
	TIpMap():m_dwV4Ip(0)
	{
		memset(m_abyV6Ip, 0, sizeof(m_abyV6Ip));
	}

public:
	void  SetV4IP(u32 dwIp){ m_dwV4Ip = htonl(dwIp); } 
    u32   GetV4IP(void) const { return ntohl(m_dwV4Ip); }

	void  SetV6IP(const s8* pchV6Ip)
	{
		if (NULL == pchV6Ip)
		{
			return;
		}
		memcpy(m_abyV6Ip, pchV6Ip, min(sizeof(m_abyV6Ip), strlen(pchV6Ip) + 1));
		m_abyV6Ip[min(sizeof(m_abyV6Ip), strlen(pchV6Ip) + 1)] = '\0';
	}
	s8* GetV6IP(void) { return m_abyV6Ip; }

	void Print()
	{
		if (0 != m_dwV4Ip)
		{
			OspPrintf(TRUE, FALSE, "\t\t V4Ip.%x(host:%u， net:%u),\n", m_dwV4Ip, GetV4IP(), m_dwV4Ip);
			OspPrintf(TRUE, FALSE, "\t\t V6Ip.%s\n", m_abyV6Ip);
		}
	}

private:
	u32 m_dwV4Ip;
	s8  m_abyV6Ip[IPV6_NAME_LEN];
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*终端IP映射表*/
struct TMtIpMapTab
{
public:
	TIpMap m_atIpMap[MAXNUM_CONF_MT];
public:
	void Print()
	{
		for (u8 byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
		{
			m_atIpMap[byIdx].Print();
		}
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*接入板Ip映射表*/
struct TCriIpMapTab
{
public:
	TIpMap m_atIpMap[MAXNUM_DRI];
public:
	void Print()
	{
		for (u8 byIdx = 0; byIdx < MAXNUM_DRI; byIdx++)
		{
			m_atIpMap[byIdx].Print();
		}
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*GK Ip映射表*/
struct TGKIpMapTab 
{
public:
	TIpMap m_atIpMap[MAXNUM_CONFIG_GK];
public:
	void Print()
	{
		for (u8 byIdx = 0; byIdx < MAXNUM_CONFIG_GK; byIdx++)
		{
			m_atIpMap[byIdx].Print();
		}
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TSwitchChannelV6
{
    TSwitchChannelV6()
    {
        memset(this, 0, sizeof(TSwitchChannelV6));
    }
protected:
	TMt  m_tSrcMt;							//交换源终端
	s8   m_achSrcIp[IPV6_NAME_LEN];         //数据源Ip地址
	s8   m_achDisIp[IPV6_NAME_LEN];			//数据分发Ip地址
	s8   m_achRcvIP[IPV6_NAME_LEN];			//本地接收的IP地址(网络序),可以是组播(或广播)地址
	u16  m_wRcvPort;						//本地接收的端口 (机器序)
	s8   m_achRcvBindIP[IPV6_NAME_LEN];		//本地接收bind IP地址(网络序),本地地址
	s8   m_achDstIP[IPV6_NAME_LEN];			//将要发送的目标IP地址(网络序),可以是组播(或广播)地址
	u16  m_wDstPort;						//将要发送的目标端口 (机器序)
	s8   m_achSndBindIP[IPV6_NAME_LEN];		//发送目标接口IP地址.(网络序),本地地址
	u8   m_byRcvSockType;
	u8   m_bySndSockType;
	
public:
	void SetSrcMt( TMt tMt ){ m_tSrcMt = tMt; }
	TMt  GetSrcMt( void ){ return m_tSrcMt; }

    void SetSrcIp(s8* pchSrcIp)
	{
		if (NULL != pchSrcIp)
		{
			memcpy(m_achSrcIp, pchSrcIp, min(strlen(pchSrcIp)+1, sizeof(m_achSrcIp)));
			m_achSrcIp[min(strlen(pchSrcIp)+1, sizeof(m_achSrcIp))] = '\0';
		}
	} 
	s8*  GetSrcIp(void){ return m_achSrcIp; }

	void SetDisIp(s8* pchDisIp)
	{
		if (NULL != pchDisIp)
		{
			memcpy(m_achDisIp, pchDisIp, min(strlen(pchDisIp)+1, sizeof(m_achDisIp)));
			m_achDisIp[min(strlen(pchDisIp)+1, sizeof(m_achDisIp))] = '\0';
		}
	} 
    s8*  GetDisIp(void) { return m_achDisIp; }

 
	void SetRcvIp(s8* pchRcvIp)
	{
		if (NULL != pchRcvIp)
		{
			memcpy(m_achRcvIP, pchRcvIp, min(strlen(pchRcvIp)+1, sizeof(m_achRcvIP)));
			m_achRcvIP[min(strlen(pchRcvIp)+1, sizeof(m_achRcvIP))] = '\0';
		}
	} 
	s8*  GetRcvIp(void){ return m_achRcvIP; }

    void SetRcvPort(u16  wRcvPort){ m_wRcvPort = htons(wRcvPort);} 
    u16  GetRcvPort( void ) const { return ntohs(m_wRcvPort); }

	void SetRcvBindIp(s8* pchRcvBindIp)
	{
		if (NULL != pchRcvBindIp)
		{
			memcpy(m_achRcvBindIP, pchRcvBindIp, min(strlen(pchRcvBindIp)+1, sizeof(m_achRcvBindIP)));
			m_achRcvBindIP[min(strlen(pchRcvBindIp)+1, sizeof(m_achRcvBindIP))] = '\0';
		}
	} 
	s8*  GetRcvBindIp(void){ return m_achRcvBindIP; }

   	void SetDstIp(s8* pchDstIp)
	{
		if (NULL != pchDstIp)
		{
			memcpy(m_achDstIP, pchDstIp, min(strlen(pchDstIp)+1, sizeof(m_achDstIP)));
			m_achDstIP[min(strlen(pchDstIp)+1, sizeof(m_achDstIP))] = '\0';
		}
	} 
	s8*  GetDstIp(void){ return m_achDstIP; }


    void SetDstPort(u16  wDstPort){ m_wDstPort = htons(wDstPort);} 
    u16  GetDstPort( void ) const { return ntohs(m_wDstPort); }

	void SetSndBindIp(s8* pchSndBindIp)
	{
		if (NULL != pchSndBindIp)
		{
			memcpy(m_achSndBindIP, pchSndBindIp, min(strlen(pchSndBindIp)+1, sizeof(m_achSndBindIP)));
			m_achSndBindIP[min(strlen(pchSndBindIp)+1, sizeof(m_achSndBindIP))] = '\0';
		}
	} 
	s8*  GetSndBindIp(void){ return m_achSndBindIP; }

	void SetRcvSockType(u8 bySockType){ m_byRcvSockType = bySockType;}
	u8   GetRcvSockType()const{ return m_byRcvSockType;}

	void SetSndSockType(u8 bySockType){ m_bySndSockType = bySockType;}
	u8   GetSndSockType()const{ return m_bySndSockType;}

 	BOOL32 IsNull(void) { return 0 == strlen(m_achDstIP);}
    BOOL32 IsSrcNull( void ) const { return 0 == strlen(m_achSrcIp); }
	BOOL32 IsRcvNull( void ) const { return 0 == strlen(m_achRcvIP); }
 	void   SetNull( ){ memset( this, 0, sizeof(TSwitchChannelV6) ); }
 	BOOL32 operator ==( TSwitchChannelV6 &tObj )
	{
		if(0 == memcmp(tObj.GetDstIp(), GetDstIp(), max(strlen(tObj.GetDstIp()), strlen(GetDstIp()))) && 
			tObj.GetDstPort() == GetDstPort() )
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
#endif