#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <memory.h>

typedef int		s32,BOOL32;
#ifdef _KDVP_
    typedef unsigned int u32;
#else
    typedef unsigned long   u32;
#endif
typedef unsigned char	u8;
typedef unsigned short  u16;
typedef short           s16;
typedef char            s8;

#ifdef _MSC_VER
typedef __int64			s64;
#else 
typedef long long		s64;
#endif 

#ifdef _MSC_VER
typedef unsigned __int64 u64;
#else 
typedef unsigned long long u64;
#endif

#ifndef _MSC_VER
#ifndef LPSTR
#define LPSTR   char *
#endif
#ifndef LPCSTR
#define LPCSTR  const char *
#endif
#endif

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

//ip结构 
struct TIpAddr
{
	u32 dwIp;
	u32 dwMask;
	s8 	sIpMac[18];
public:
	TIpAddr(){Clear();}
	void Clear(){memset(this, 0, sizeof(TIpAddr));}
	void Print()
	{
		struct in_addr saddr; 
		saddr.s_addr = (in_addr_t)dwIp;
		printf("ip:%s\n", inet_ntoa(saddr));
		saddr.s_addr = (in_addr_t)dwMask;
		printf("mask:%s\n", inet_ntoa(saddr));
		printf("mac:%s\n", sIpMac);
	}
};

//接收buf结构 
struct TRecvBuf
{
	s8 m_sRecvBuf[256];
public:
	TRecvBuf(){Clear();}
	void Clear(){memset(this, 0, sizeof(TRecvBuf));}
};

//根据输入指令获得相关返回值 
TRecvBuf GetLocalHostIp(const s8 *sCmd)
{
	FILE *stream;
	TRecvBuf tRecvBuf;
    stream = popen(sCmd, "r");
    fread(tRecvBuf.m_sRecvBuf, sizeof(s8), sizeof(tRecvBuf.m_sRecvBuf)-1, stream);
	pclose(stream);
	return tRecvBuf;
}

s32 main(s32 argc, u8 *argv[])
{
	s8 sCmd[256] = {'\0'}; 
	u8 byIpNum = 0;
	TRecvBuf tRecvBuf;
	
	//ipnum
	memset(sCmd, '\0', sizeof(sCmd));
	sprintf(sCmd, "ifconfig | grep -c Bcast");
	tRecvBuf.Clear();
	tRecvBuf = GetLocalHostIp(sCmd);
	printf("\nipnum:%d\n\n", atoi(tRecvBuf.m_sRecvBuf));
	byIpNum = atoi(tRecvBuf.m_sRecvBuf);
	
	TIpAddr tIpAddr;
	for(u8 byIpIdx = 1; byIpIdx <= byIpNum; byIpIdx++)
	{		
		tIpAddr.Clear();
		
		//ip
		memset(sCmd, '\0', sizeof(sCmd));
		sprintf(sCmd, "ifconfig | grep Bcast | sed -n '%dp' | awk '{print $2}' | awk -F ':' '{print $2}'", byIpIdx);
		tRecvBuf.Clear();
		tRecvBuf = GetLocalHostIp(sCmd);
		//printf("ip:%s\n", tRecvBuf.m_sRecvBuf);
		tIpAddr.dwIp = inet_addr(tRecvBuf.m_sRecvBuf);
		
		//mac
		memset(sCmd, '\0', sizeof(sCmd));
		sprintf(sCmd, "ifconfig | grep HWaddr | sed -n '%dp' | awk '{print $5}'", byIpIdx);
		tRecvBuf.Clear();
		tRecvBuf = GetLocalHostIp(sCmd);
		//printf("mac:%s\n", tRecvBuf.m_sRecvBuf);
		memcpy(tIpAddr.sIpMac, tRecvBuf.m_sRecvBuf, sizeof(tIpAddr.sIpMac));
		
		//mask
		memset(sCmd, '\0', sizeof(sCmd));
		sprintf(sCmd, "ifconfig | grep Bcast | sed -n '%dp' | awk '{print $4}' | awk -F ':' '{print $2}'", byIpIdx);
		tRecvBuf.Clear();
		tRecvBuf = GetLocalHostIp(sCmd);
		//printf("mask:%s\n", tRecvBuf.m_sRecvBuf);
		tIpAddr.dwMask = inet_addr(tRecvBuf.m_sRecvBuf);
		
		printf("ipIdx:%d\n",byIpIdx);
		tIpAddr.Print();
	}
	return 0;
}
