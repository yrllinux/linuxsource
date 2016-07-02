/*****************************************************************************
模块名      : commonlib
文件名      : commonlib.cpp
相关文件    : commonlib.h
文件实现功能: 公用函数库
作者        : 周广程
版本        : V1.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/09/18  4.5         周广程        创建
******************************************************************************/

#include "commonlib.h"

// #include "macro.h" // no need [pengguofeng 9/25/2013]


#ifdef _LINUX_
//#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>
#endif

#ifdef _LINUX_
//#include <linux/route.h>			// [4/2/2010 xliang] 会与<net/if.h>中函数重定义
//#include <linux/sockios.h>
//#include <sys/ioctl.h>
//#include <linux/ethtool.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>
//#include <dirent.h>
#include <ctype.h>
#include <sys/procfs.h>
//#include <mqueue.h>
#endif



#ifdef WIN32
#include "IPHlpApi.h"
#include "Tlhelp32.h"
#endif


#ifdef WIN32
typedef DWORD (WINAPI * GETIPFORWARDTABLE)( PMIB_IPFORWARDTABLE pIpForwardTable, PULONG pdwSize, BOOL bOrder );
typedef DWORD (WINAPI * CREATEIPFORWARDENTRY)( PMIB_IPFORWARDROW pRoute );
typedef DWORD (WINAPI * DELETEIPFORWARDENTRY)( PMIB_IPFORWARDROW pRoute );
typedef DWORD (WINAPI * SETIPFORWARDENTRY)( PMIB_IPFORWARDROW pRoute );
// 网卡操作
typedef DWORD (WINAPI *ADDIPADDRESS)(DWORD IpAddress, DWORD IpMask, DWORD IfIndex, PULONG NTEContext, PULONG NTEInstance );
typedef DWORD (WINAPI *GETADAPTERSINFO)(PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen);
#endif

//////////////////////////////////////////////////////////////////////////
// TEthInfo实现 [pengguofeng 9/25/2013]
/*=============================================================================
函 数 名： SetEthId
功    能： 设置网卡ID，[主ID]
算法实现： 
全局变量： 
参    数： u8
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
void TEthInfo::SetEthId(u8 byEthId)
{
	m_byEthId = byEthId;
}

/*=============================================================================
函 数 名： GetEthId
功    能： 取本结构体对应的网卡ID，[主ID]
算法实现： 
全局变量： 
参    数： 
返 回 值： u8 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
u8 TEthInfo::GetEthId()
{
	return m_byEthId;
}

/*=============================================================================
函 数 名： IsValid
功    能： 判断此结构体是否合法
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
BOOL32 TEthInfo::IsValid()
{
	return m_byEthId == INVALID_ETHID? FALSE: TRUE;
}

/*=============================================================================
函 数 名： DelAllIpMask
功    能： 删除所有的Ip和Mask
算法实现： 
全局变量： 
参    数： 
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/10/14   4.7			彭国锋                  创建
=============================================================================*/
void	TEthInfo::DelAllIpMask()
{
	for ( u8 byId = 0;byId < MAX_ETH_IPNUM; byId++)
	{
		m_dwIp[byId] = 0;
		m_dwMask[byId] = 0;
	}
	m_dwGateway = 0;
	m_byIpNum = 0;
}

/*
 *	初始化成员变量
 */
void TEthInfo::Clear()
{
	memset(this, 0, sizeof(*this));
	m_byEthId = INVALID_ETHID;
}

/*=============================================================================
函 数 名： GetIpNum
功    能： 取当前有多少IP
算法实现： 
全局变量： 
参    数： 
返 回 值： u8 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
u8 TEthInfo::GetIpNum()
{
	return m_byIpNum;
}

/*=============================================================================
函 数 名： GetSubIp
功    能： 取子网卡IP
算法实现： 
全局变量： 
参    数： u8
返 回 值： u32 ,主机序
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
u32	TEthInfo::GetSubIp(u8 byIdx)
{
	if ( byIdx > MAX_ETH_IPNUM -1)
	{
		return 0;//over max
	}

	return ntohl(m_dwIp[byIdx]);
}

/*=============================================================================
函 数 名： GetSubMask
功    能： 取子网卡MASK
算法实现： 
全局变量： 
参    数： u8
返 回 值： u32 ,主机序
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
u32 TEthInfo::GetSubMask(u8 byIdx)
{
	if ( byIdx > MAX_ETH_IPNUM -1 )
	{
		return 0; //over max
	}

	return ntohl(m_dwMask[byIdx]);
}

/*=============================================================================
函 数 名： AddSubIpMask
功    能： 	添加一个虚拟IP/MASK
算法实现： 
全局变量： 
参    数：	u32 dwIP，主机序
			u32 dwMask, 主机序
返 回 值： BOOL32， 成功 1 OR失败 0
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
BOOL32 TEthInfo::AddSubIpMask(u32 dwIp, u32 dwMask)
{
	u8 byIpNum = GetIpNum();
	if ( byIpNum > MAX_ETH_IPNUM -1)
	{
		return FALSE;//超过15个则不再加了
	}

	if ( GetSubIdxByIp(dwIp) != INVALID_ETHID )
	{
		return TRUE; //已经在里面的
	}

	m_dwIp[byIpNum] = htonl(dwIp);
	m_dwMask[byIpNum] = htonl(dwMask);
	AddIpNum();
	return TRUE;
}

/*=============================================================================
函 数 名： AddIpNum
功    能： 添加子IP数目，这个是连主IP一起计算的
算法实现： 
全局变量： 
参    数：	
返 回 值： void
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
void TEthInfo::AddIpNum()
{
	if ( m_byIpNum > MAX_ETH_IPNUM -1 )
	{
		return;
	}
	m_byIpNum++;
}

/*=============================================================================
函 数 名： GetSubIdxByIp
功    能： 根据IP得到它的子网卡ID
算法实现： 
全局变量： 
参    数：	u32 dwHostIp， 主机序
返 回 值： u8
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
u8	TEthInfo::GetSubIdxByIp(u32 dwIp) 
{
	if ( dwIp == 0 )
	{
		return INVALID_ETHID;
	}

	for (u8 byLoop = 0; byLoop < GetIpNum(); byLoop++)
	{
		if ( m_dwIp[byLoop] == htonl(dwIp) )
		{
			return byLoop;
		}
	}

	return INVALID_ETHID;
}

/*=============================================================================
函 数 名： DelSubIpMaskByIdx
功    能： 根据IP得到它的子网卡ID
算法实现： 
全局变量： 
参    数：	u8 byIdx,子网卡ID
返 回 值： void
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
void TEthInfo::DelSubIpMaskByIdx(u8 byIdx)
{
	if ( byIdx < MAX_ETH_IPNUM )
	{
		//整体前移，并删除最后一个 IP为0的
		for ( u8 byLoop = byIdx; byLoop < MAX_ETH_IPNUM-1; byLoop++)
		{
			if ( m_dwIp[byLoop+1] != 0 )
			{
				m_dwIp[byLoop] = m_dwIp[byLoop+1];
				m_dwMask[byLoop] = m_dwMask[byLoop+1];
			}
			else
			{
				DBG_LOG("already get to the end\n");
				break;
			}
		}
		
		m_dwIp[MAX_ETH_IPNUM-1] = m_dwMask[MAX_ETH_IPNUM-1] = 0;
		m_byIpNum--;
	}
	else
	{
		DBG_LOG("not found IP.%x\n", byIdx);
	}
}
/*=============================================================================
函 数 名： DelSubIpMask
功    能： 根据IP，删除指定的子网卡IP MASK
算法实现： 
全局变量： 
参    数：	u32 dwIP，主机序
返 回 值： void
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
void TEthInfo::DelSubIpMask(u32 dwIp)
{
	if ( dwIp == 0 )
	{
		return;
	}

	u8 byIdx = GetSubIdxByIp(dwIp);

	DelSubIpMaskByIdx(byIdx);
}

/*=============================================================================
函 数 名： SetSubIpMaskByIdx
功    能： 根据子网卡ID，删除指定的子网卡IP MASK
算法实现： 
全局变量： 
参    数：	u32 dwIP，主机序
			u32 dwMask,主机序
			u8	byIdx, 子网卡ID
返 回 值： void
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
void TEthInfo::SetSubIpMaskByIdx(u32 dwIp, u32 dwMask, u8 byIdx)
{
	if ( dwIp == 0 ||dwMask == 0 ||byIdx > MAX_ETH_IPNUM - 1)
	{
		return;
	}

	if (m_dwIp[byIdx] == 0 )
	{
		m_byIpNum++;//功效和Add类似
	}
	m_dwIp[byIdx] = htonl(dwIp);
	m_dwMask[byIdx] = htonl(dwMask);
}

/*=============================================================================
函 数 名： ModSubIpMask
功    能： 	修改指定IP的网关
算法实现： 
全局变量： 
参    数：	u32 dwIP，主机序
			u32 dwMask, 主机序
返 回 值： void
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
void TEthInfo::ModSubIpMask(u32 dwIp, u32 dwMask)
{
	if ( dwIp == 0 || dwMask == 0 )
	{
		return;
	}

	u8 byIdx = GetSubIdxByIp(dwIp);

	if ( byIdx < MAX_ETH_IPNUM )
	{
		m_dwIp[byIdx] = htonl(dwIp);
		m_dwMask[byIdx] = htonl(dwMask);
	}
}

/*=============================================================================
函 数 名： GetGateway
功    能： 取主网卡的网关
算法实现： 
全局变量： 
参    数：	
返 回 值： u32， 主机序
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
u32 TEthInfo::GetGateway()
{
	return ntohl(m_dwGateway);
}

/*=============================================================================
函 数 名： SetGateway
功    能： 设主网卡的网关
算法实现： 
全局变量： 
参    数：	u32 dwGw， 主机序
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
void TEthInfo::SetGateway(u32 dwGw)
{
	m_dwGateway = htonl(dwGw);
}

/*=============================================================================
函 数 名： DelGateway
功    能： 删除主网卡的网关
算法实现： 
全局变量： 
参    数：
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
void TEthInfo::DelGateway()
{
	m_dwGateway = 0;
}

/*=============================================================================
函 数 名： SetMacAddr
功    能： 设置MAC地址
算法实现： 
全局变量： 
参    数：	u8 *pbyMac， 16进制形式的MAC地址{AA,BB,CC,DD,EE,FF}
			u8 byMacLen
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
void TEthInfo::SetMacAddr(u8 *pbyMac, u8 byMacLen)
{
	if ( byMacLen > MAX_MACADDR_NORMAL_LEN )
	{
		byMacLen = MAX_MACADDR_NORMAL_LEN;
	}

	u8 byTmp = 0;
	m_byMacLen = 0;
	for (u8 byLoop = 0;byLoop < byMacLen; byLoop++)
	{
		byTmp = pbyMac[byLoop] >> 4;//高4位
		if ( byTmp < 10)
		{
			m_achMacAddr[m_byMacLen] = '0' + byTmp;
		}
		else // 10~15
		{
			m_achMacAddr[m_byMacLen] = 'A' + byTmp - 10;
		}

		m_byMacLen++;

		byTmp = pbyMac[byLoop] & 0x0F;//低4位
		if ( byTmp < 10)
		{
			m_achMacAddr[m_byMacLen] = '0' + byTmp;
		}
		else // 10~15
		{
			m_achMacAddr[m_byMacLen] = 'A' + byTmp - 10;
		}

		m_byMacLen++;

		if ( byLoop != byMacLen - 1)
		{
			m_achMacAddr[m_byMacLen++] = ':';
		}
	}
}

/*=============================================================================
函 数 名： SetMacAddrByStr
功    能： 设置MAC地址
算法实现： 
全局变量： 
参    数：	s8 *pachMacStr， 字符串形式的MAC地址{"AA:BB:CC:DD:EE:FF"}
			u8 byMacLen
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
void TEthInfo::SetMacAddrByStr(s8 *pachMacStr, u8 byMacLen)
{
	if ( !pachMacStr || byMacLen > MAX_MACADDR_STR_LEN -1)
	{
		return;
	}

	memcpy(m_achMacAddr, pachMacStr, byMacLen);
	m_achMacAddr[byMacLen] = 0;
	m_byMacLen = byMacLen;
}

/*=============================================================================
函 数 名： GetMacAddr
功    能： 获取MAC地址，以16进制形式保存到参数中，注意返回的都是有效值，不包含\0
			外部使用者，需要拷贝的时候，自己再拷贝！
算法实现： 
全局变量： 
参    数：	u8 *pbyMac， 存放的数组
			u8 *pbyMacLen, 返回的实际长度
返 回 值： BOOL32
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
BOOL32 TEthInfo::GetMacAddr(u8 *pbyMac, u8 *pbyMacLen)
{
	if ( !pbyMac ||!pbyMacLen)
	{
		return FALSE;
	}

	s8 abyTmp[3];
	u8 byLoop = 0;
	u8 byRetLen = 0;
	while ( byLoop < m_byMacLen )
	{
		if ( byLoop + 2 > m_byMacLen )
		{
			return FALSE;//冒号之间少于2个字符，必然格式不对
		}
		memcpy(abyTmp, &m_achMacAddr[byLoop], 2);
		abyTmp[3-1] = 0;
		sscanf(abyTmp, "%x", &pbyMac[byRetLen++]);
		byLoop += 3;
	}

	*pbyMacLen = byRetLen;
	return TRUE;
}

/*=============================================================================
函 数 名： GetMacAddrStr
功    能： 获取MAC地址,字符串形式,以\0结尾
算法实现： 
全局变量： 
参    数：	
返 回 值： s8 *
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
s8 *TEthInfo::GetMacAddrStr()
{
	return m_achMacAddr;
}

/*=============================================================================
函 数 名： GetMacAddrLen
功    能： 获取MAC地址[字符串]的长度
算法实现： 
全局变量： 
参    数：	
返 回 值： u8
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
u8 TEthInfo::GetMacAddrLen()
{
	return m_byMacLen;
}

/*=============================================================================
函 数 名： operator =
功    能： 赋值构造函数
算法实现： 
全局变量： 
参    数：	
返 回 值： TEthInfo &
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
TEthInfo & TEthInfo::operator =(TEthInfo & tEthInfo)
{
	if ( this != &tEthInfo )
	{
		this->Clear();
		this->SetEthId(tEthInfo.GetEthId());
		this->SetGateway(tEthInfo.GetGateway());
		this->SetMacAddrByStr(tEthInfo.GetMacAddrStr(), tEthInfo.GetMacAddrLen());
		for(u8 byLoop = 0; byLoop < tEthInfo.GetIpNum(); byLoop++ )
		{
			AddSubIpMask(tEthInfo.GetSubIp(byLoop), tEthInfo.GetSubMask(byLoop));
		}
	}
	return *this;
}

/*=============================================================================
函 数 名： Print
功    能： 打印函数，目前只在OspPrint，在23端口打印，则需要使用DEBUG版本的库
算法实现： 
全局变量： 
参    数：	
返 回 值： void
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
void TEthInfo::Print()
{
	OspPrintf(TRUE, FALSE, "-----------eth.%d----------\n", GetEthId());
	DBG_LOG( "-----------eth.%d----------\n", GetEthId());
	OspPrintf(TRUE, FALSE, "MAC:[%s]\n", GetMacAddrStr());
	DBG_LOG( "MAC:[%s]\n", GetMacAddrStr());
	OspPrintf(TRUE, FALSE, "Gateway:[%x]All Ip Num.%d\n", GetGateway(), GetIpNum());
	DBG_LOG("Gateway:[%x] All Ip Num.%d\n", GetGateway(), GetIpNum());
	for (u8 byLoop = 0; byLoop < GetIpNum(); byLoop++)
	{
		OspPrintf(TRUE, FALSE, "[Index:%2d] IP: %x  Mask: %x\n",byLoop, GetSubIp(byLoop), GetSubMask(byLoop));
		DBG_LOG( "[Index:%2d] IP: %x  Mask: %x\n",byLoop, GetSubIp(byLoop), GetSubMask(byLoop));
	}
}

//////////////////////////////////////////////////////////////////////////
/*=============================================================================
函 数 名： FileLog
功    能： 把字符串输出到pLogFile指向的文件中，文件指针由调用者提供，函数不关闭文件
算法实现： 
全局变量： 
参    数： FILE * pLogFile
s8 * pszFmt
...
返 回 值： API void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/4/23   4.0		    周广程                  创建
=============================================================================*/
/*lint -save -esym(530,argptr)*/
API void FileLog( FILE * pLogFile, s8 * pszFmt, ... )
{
    if ( NULL == pLogFile )
    {
        return;
    }   
    
    TKdvTime tCurTime;
    time_t tMcuTime;
    ::time( &tMcuTime );
    tCurTime.SetTime( &tMcuTime );
    
    s32 nLen = 0;
    s8 achPrintBuf[512] = {0};
    nLen = sprintf(achPrintBuf, "%4.4u-%2.2u-%2.2u %2.2u:%2.2u:%2.2u -> ", 
		tCurTime.GetYear(), tCurTime.GetMonth(), tCurTime.GetDay(),
		tCurTime.GetHour(), tCurTime.GetMinute(), tCurTime.GetSecond() );
	
    if ( nLen > 0 )
    {
        va_list argptr;
        va_start( argptr, pszFmt ); 
		
        vsprintf( achPrintBuf + nLen, pszFmt, argptr );          
        fprintf( pLogFile,"%s\n", achPrintBuf );
        fflush( pLogFile );
		
        va_end(argptr); 
    }
    
	return ;
}
/*lint -restore*/
/*=============================================================================
函 数 名： CreatDir
功    能： 创建目录(递归创建)
算法实现： 
全局变量： 
参    数： const s8* lpszDirPath
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/9/15   4.0			周广程                  创建
=============================================================================*/
API BOOL32 CreatDir(const s8* lpszDirPath)
{
    if ( NULL == lpszDirPath )
    {
        return FALSE;
    }

    BOOL32 bSuccess = TRUE;
    
#ifdef _LINUX_
    s32 nRet = 0;
    s8 achDirPath[64];
    memset(achDirPath, 0, sizeof(achDirPath));
    sprintf(achDirPath, "%s", lpszDirPath);
    s8 ch = '/';
    s8 *pchDest = NULL;
    
    pchDest = strrchr( achDirPath, ch );

    DIR* pdir = opendir(achDirPath);
    if ( pdir != NULL )
    {
        closedir(pdir);
        chmod(achDirPath, 0777);
        return TRUE;
    }
    
    if ( pchDest == NULL || pchDest == achDirPath )
    {
        nRet = mkdir( achDirPath, S_IRWXU|S_IRWXG|S_IRWXO );
        if( 0 != nRet )
        {
            printf("make diretory<%s> Error :%s\n",achDirPath,strerror(errno)); 
            return FALSE;
        }
    }
    else
    {
        *pchDest = '\0';
        if ( CreatDir(achDirPath) )
        {
            nRet = mkdir( lpszDirPath, S_IRWXU|S_IRWXG|S_IRWXO );
            if( 0 != nRet )
            {
                printf("make diretory<%s> Error :%s\n",lpszDirPath,strerror(errno));  
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }
    }		                
#endif
#ifdef WIN32
    bSuccess =  CreateDirectory( lpszDirPath, NULL );
    DWORD lastError = GetLastError();
    if ( ERROR_ALREADY_EXISTS == lastError )
    {
        bSuccess = TRUE;
    }
#endif
    return bSuccess;
}



/*=============================================================================
函 数 名： CreatFile
功    能： 创建任意路径下的文件，同时自动创建需要的目录
           如文件已存在，则直接返回TURE
算法实现： 
全局变量： 
参    数： const s8 * lpszFileFullPath  // 文件全路径，含文件名
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/10/8   4.0		    周广程                  创建
=============================================================================*/
API BOOL32 CreatFile( const s8 * lpszFileFullPath )
{
    FILE *stream = fopen( lpszFileFullPath, "r" );
    if( stream != NULL )
    {
        fclose(stream);
        return TRUE;
    }
    
    //文件不存在，创建该文件
    s8 ch = '/';
    s8 achDirPath[64];
    memset( achDirPath, 0, sizeof(achDirPath) );
    sprintf( achDirPath, "%s", lpszFileFullPath );
    s8* pchDest = strrchr( achDirPath, ch );
    if ( NULL != pchDest )
    {
        *pchDest = '\0';
    }
    BOOL32 bCreat = CreatDir( achDirPath );// 递归创建目录
    if ( !bCreat ) 
    {
        return FALSE;
    }
    
    stream = fopen( lpszFileFullPath, "w" );
    if ( stream == NULL )
    {
        return FALSE;
    }
    fclose(stream);
    
    return TRUE;
}


API u8 GetResViaWH( u16 wWidth, u16 wHeight )
{
    if ( wWidth == 128 && wHeight == 96 )
    {
        return VIDEO_FORMAT_SQCIF;
    }
    if ( wWidth == 176 && wHeight == 144 )
    {
        return VIDEO_FORMAT_QCIF;
    }
    if ( wWidth == 352 && wHeight == 288 )
    {
        return VIDEO_FORMAT_CIF;
    }
    if ( wWidth == 352 && wHeight == 576 )
    {
        return VIDEO_FORMAT_2CIF;
    }
    if ( (wWidth == 704 && wHeight == 576) ||
         (wWidth == 720 && wHeight == 576) ||
         (wWidth == 720 && wHeight == 480))
    {
        return VIDEO_FORMAT_4CIF;
    }
    if ( wWidth == 1408 && wHeight == 1152 )
    {
        return VIDEO_FORMAT_16CIF;
    }
    if ( wWidth == 352 && wHeight == 240 )
    {
        return VIDEO_FORMAT_SIF;
    }
    if ( wWidth == 352 && wHeight == 480 )
    {
        return VIDEO_FORMAT_2SIF;
    }
    if ( wWidth == 704 && wHeight == 480 )
    {
        return VIDEO_FORMAT_4SIF;
    }
    if ( wWidth == 640 && wHeight == 480 )
    {
        return VIDEO_FORMAT_VGA;
    }
    if ((wWidth == 800 && wHeight == 600) ||
        (wWidth == 800 && wHeight == 608))
    {
        return VIDEO_FORMAT_SVGA;
    }
    if ( wWidth == 1024 && wHeight == 768 )
    {
        return VIDEO_FORMAT_XGA;
    }
    if ( wWidth == 112 && wHeight == 96 )
    {
        return VIDEO_FORMAT_SQCIF_112x96;
    }
    if ( wWidth == 96 && wHeight == 80 )
    {
        return VIDEO_FORMAT_SQCIF_96x80;
    }
    if ( wWidth == 1024 && wHeight == 576 )
    {
        return VIDEO_FORMAT_W4CIF;
    }
    if ( wWidth == 1280 && wHeight == 720 )
    {
        return VIDEO_FORMAT_HD720;
    }
    if ( wWidth == 1280 && wHeight == 1024 )
    {
        return VIDEO_FORMAT_SXGA;
    }
    if ( wWidth == 1600 && wHeight == 1200 )
    {
        return VIDEO_FORMAT_UXGA;
    }
    if ( ( wWidth == 1920 && wHeight == 1088 ) ||
         ( wWidth == 1920 && wHeight == 1080 ) ||
         ( wWidth == 1920 && wHeight == 544 ) )
    {
        return VIDEO_FORMAT_HD1080;
    }

	if ( wWidth == 1440 && wHeight == 816 )
	{
		return VIDEO_FORMAT_1440x816;
	}
	
	if ( wWidth == 960 && wHeight == 544 )
	{
		return VIDEO_FORMAT_960x544;
	}
	
	if ( wWidth == 640 && wHeight == 368 )
	{
		return VIDEO_FORMAT_640x368;
	}
	
	if ( wWidth == 480 && wHeight == 272 )
	{
		return VIDEO_FORMAT_480x272;
	}
	
	if ( wWidth == 640 && wHeight == 544 )
	{
		return VIDEO_FORMAT_640x544;
	}

	if ( wWidth == 320 && wHeight == 272 )
	{
		return VIDEO_FORMAT_320x272;
	}
	
    return 0;
}


API void GetWHViaRes(u8 byRes, u16 &wWidth, u16 &wHeight)
{
    wWidth = 0;
    wHeight = 0;
    
    switch(byRes)
    {
    case VIDEO_FORMAT_SQCIF_112x96:
        wWidth = 112;
        wHeight = 96;
        break;
        
    case VIDEO_FORMAT_SQCIF_96x80:
        wWidth = 96;
        wHeight = 80;
        break;
        
    case VIDEO_FORMAT_SQCIF:
        wWidth = 128;
        wHeight = 96;
        break;
        
    case VIDEO_FORMAT_QCIF:
        wWidth = 176;
        wHeight = 144;
        break;
        
    case VIDEO_FORMAT_CIF:
        wWidth = 352;
        wHeight = 288;
        break;
        
    case VIDEO_FORMAT_2CIF:
        wWidth = 352;
        wHeight = 576;
        break;
        
    case VIDEO_FORMAT_4CIF:
        
        //基于尽可能播放考虑，这里取大值
        wWidth = 704;
        wHeight = 576;
        break;
        
    case VIDEO_FORMAT_16CIF:
        wWidth = 1048;
        wHeight = 1152;
        break;
        
    case VIDEO_FORMAT_AUTO:
		// 对于auto分辨率解析为cif
        wWidth = 352;
        wHeight = 288;      
        break;
        
    case VIDEO_FORMAT_SIF:
        wWidth = 352;
        wHeight = 240;
        break;
        
    case VIDEO_FORMAT_2SIF:
        wWidth = 352;
        wHeight = 480;
        break;
        
    case VIDEO_FORMAT_4SIF:
        wWidth = 704;
        wHeight = 480;
        break;
        
    case VIDEO_FORMAT_VGA:
        wWidth = 640;
        wHeight = 480;
        break;
        
    case VIDEO_FORMAT_SVGA:
        wWidth = 800;
        wHeight = 608;   //600-->608
        break;
        
    case VIDEO_FORMAT_XGA:
        wWidth = 1024;
        wHeight = 768;
        break;

    case VIDEO_FORMAT_W4CIF:
        wWidth = 1024;
        wHeight = 576;
        break;

    case VIDEO_FORMAT_HD720:
        wWidth = 1280;
        wHeight = 720;
        break;

    case VIDEO_FORMAT_SXGA:
        wWidth = 1280;
        wHeight = 1024;
        break;

    case VIDEO_FORMAT_UXGA:
        wWidth = 1600;
        wHeight = 1200;
        break;

    case VIDEO_FORMAT_HD1080:
        wWidth = 1920;
        wHeight = 1088;
        break;
		//非标分辨率（1080p底图）
	case VIDEO_FORMAT_1440x816:
		wWidth = 1440;
		wHeight = 816;
        break;

	case VIDEO_FORMAT_1280x720:
		wWidth = 1280;
		wHeight = 720;
		break;
	
	case VIDEO_FORMAT_960x544:
		wWidth = 960;
		wHeight = 544;
		break;

	case VIDEO_FORMAT_640x368:
		wWidth = 640;
		wHeight =368;
		break;

	case VIDEO_FORMAT_480x272:
		wWidth = 480;
		wHeight =272;
		break;
	//非标分辨率（720p底图）
	case VIDEO_FORMAT_720_960x544:
		wWidth = 960;
		wHeight =544;
		break;

	case VIDEO_FORMAT_720_864x480:
		wWidth = 864;
		wHeight =480;
		break;

	case VIDEO_FORMAT_720_640x368:
		wWidth = 640;
		wHeight =368;
		break;

	case VIDEO_FORMAT_720_432x240:
		wWidth = 432;
		wHeight =240;
		break;

	case VIDEO_FORMAT_720_320x192:
		wWidth = 320;
		wHeight =192;
		break;
	case VIDEO_FORMAT_640x544:
		wWidth = 640;
		wHeight =544;
		break;
	case VIDEO_FORMAT_320x272:
		wWidth = 320;
		wHeight =272;
		break;
    default:
        break;
    }
    if ( 0 == wHeight || 0 == wWidth )
    {
        OspPrintf( TRUE, FALSE, "[GetWHByRes] unexpected res.%d, ignore it\n", byRes );
    }
    return;
}

// 判断源端分辨率是否大于目的端分辨率,大于则返回TRUE,小于等于则返回FALSE
API BOOL32 IsResBigger(u8 bySrcRes, u8 byDstRes)
{
	if (VIDEO_FORMAT_AUTO == bySrcRes ||
		VIDEO_FORMAT_AUTO == byDstRes)
	{
		return FALSE;
	}

	//20110503 zjl 双流不区分动静态
	//     if (!IsDSResMatched(bySrcRes, byDstRes))
	//     {
	//         return FALSE;
	//     }

	const s32 s_anRes[] = { VIDEO_FORMAT_SQCIF_96x80,
		VIDEO_FORMAT_SQCIF_112x96,
		VIDEO_FORMAT_SIF,
		VIDEO_FORMAT_2SIF,
		VIDEO_FORMAT_CIF,
		VIDEO_FORMAT_2CIF,
		VIDEO_FORMAT_VGA,
		VIDEO_FORMAT_4SIF,
		VIDEO_FORMAT_4CIF,
		VIDEO_FORMAT_SVGA,
		VIDEO_FORMAT_XGA,
		VIDEO_FORMAT_HD720,
		VIDEO_FORMAT_WXGA,
		VIDEO_FORMAT_SXGA, 
		VIDEO_FORMAT_16CIF,
		VIDEO_FORMAT_UXGA,
		VIDEO_FORMAT_HD1080              
	};

	u8 bySrcResIdx = 0xff;
	u8 byDstResIdx = 0xff;
	u8 byLop = 0;

	// 记录在分辨率数组中的位置，用于比较分辨率大小
	for ( byLop = 0; byLop < sizeof(s_anRes)/sizeof(s32); byLop++ )
	{
		if ( s_anRes[byLop] == bySrcRes )
		{
			if ( bySrcResIdx == 0xff )
			{
				bySrcResIdx = byLop;    
			}
		}
		if ( s_anRes[byLop] == byDstRes )
		{
			if ( byDstResIdx == 0xff )
			{
				byDstResIdx = byLop;
			}
		}
		if ( bySrcResIdx != 0xff && byDstResIdx != 0xff )
		{
			break;
		}
	}

	if (bySrcResIdx == 0xff || byDstResIdx == 0xff)
	{
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[IsResGE] unexpected res <src.%d, dst.%d>\n", bySrcRes, byDstRes);
		return FALSE;
	}

	if (bySrcResIdx <= byDstResIdx)
	{
		return FALSE;
	}
	return TRUE;
}

API BOOL32 IsResolutionPresentation(u8 byRes)
{
	BOOL32 bPresentation = FALSE;

	switch(byRes)
	{
	case VIDEO_FORMAT_VGA:
	case VIDEO_FORMAT_SVGA:
	case VIDEO_FORMAT_XGA:
	case VIDEO_FORMAT_SXGA:
	case VIDEO_FORMAT_UXGA:
	case VIDEO_FORMAT_WXGA:
		bPresentation = TRUE;
		break;
	default:
		break;
	}
	return bPresentation;
}

API BOOL32 IsResolutionLive(u8 byRes)
{
	BOOL32 bLive = FALSE;

	switch(byRes)
	{
	case VIDEO_FORMAT_CIF:
	case VIDEO_FORMAT_2CIF:
	case VIDEO_FORMAT_4CIF:
	case VIDEO_FORMAT_16CIF:
		
	case VIDEO_FORMAT_SIF:
	case VIDEO_FORMAT_2SIF:
	case VIDEO_FORMAT_4SIF:
		
	case VIDEO_FORMAT_W4CIF:
	case VIDEO_FORMAT_HD720:
	case VIDEO_FORMAT_HD1080:
		//Auto不解析
	case VIDEO_FORMAT_AUTO:
		bLive = TRUE;
		break;
		//DS不考虑SQCIF
	case VIDEO_FORMAT_SQCIF_112x96:
	case VIDEO_FORMAT_SQCIF_96x80:
		break;
	default:
		break;
	}
	return bLive;
}

API BOOL32 IsDstSrcResMatched(u8 bySrcRes, u8 byDstRes)
{
	BOOL32 bMatched = FALSE;
	if (IsResolutionPresentation(bySrcRes) && IsResolutionPresentation(byDstRes))
	{
		bMatched = TRUE;
	}
	else if (IsResolutionLive(bySrcRes) && IsResolutionLive(byDstRes))
	{
		bMatched = TRUE;
	}
	return bMatched;
}


 
/*====================================================================
函数名      : GetMediaStr
功能        : 获取媒体类型字符串
算法实现    :（可选项）
引用全局变量: 无
输入参数说明: byMediaType 媒体类型
返回值说明  : 无
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
03/11/13    3.0         胡昌威       创建
====================================================================*/
API LPCSTR GetMediaString( u8 byMediaType )
{
    static s8 szMediaType[16];
	switch( byMediaType ) 
	{
	case MEDIA_TYPE_H261:
		sprintf( szMediaType, "%s%c", "H.261", 0 );
		break;
	case MEDIA_TYPE_H262:
		sprintf( szMediaType, "%s%c", "H.262", 0 );
		break;
	case MEDIA_TYPE_H263:
		sprintf( szMediaType, "%s%c", "H.263", 0 );
		break;
	case MEDIA_TYPE_H263PLUS:
		sprintf( szMediaType, "%s%c", "H.263+", 0 );
		break;
	case MEDIA_TYPE_H264:
		sprintf( szMediaType, "%s%c", "H.264", 0 );
		break;
	case MEDIA_TYPE_MP4:
		sprintf( szMediaType, "%s%c", "MPEG-4", 0 );
		break;
	case MEDIA_TYPE_PCMA:
		sprintf( szMediaType, "%s%c", "PCMA", 0 );
		break;
	case MEDIA_TYPE_PCMU:
		sprintf( szMediaType, "%s%c", "PCMU", 0 );
		break;
	case MEDIA_TYPE_G722:
		sprintf( szMediaType, "%s%c", "G.722", 0 );
		break;
    case MEDIA_TYPE_G7221C:
        sprintf( szMediaType, "%s%c", "G.7221", 0 );
        break;
	case MEDIA_TYPE_G7231:
		sprintf( szMediaType, "%s%c", "G.7231", 0 );
		break;
	case MEDIA_TYPE_G728:
		sprintf( szMediaType, "%s%c", "G.728", 0 );
		break;
	case MEDIA_TYPE_G729:
		sprintf( szMediaType, "%s%c", "G.729", 0 );
		break;
	case MEDIA_TYPE_MP3:
		sprintf( szMediaType, "%s%c", "MP3", 0 );
		break;
	case MEDIA_TYPE_T120:
		sprintf( szMediaType, "%s%c", "T.120", 0 );
		break;
	case MEDIA_TYPE_H224:
		sprintf( szMediaType, "%s%c", "H.224", 0 );
		break;
	case MEDIA_TYPE_H239:
		sprintf( szMediaType, "%s%c", "H.239", 0 );
		break;
	case MEDIA_TYPE_MMCU:
		sprintf( szMediaType, "%s%c", "MMCU", 0 );
		break;
	default:
		sprintf( szMediaType, "%s(%u)%c", "Unknow", byMediaType, 0 );
		break;
	}
	return szMediaType;
}

/*=============================================================================
函 数 名： strofip 
功    能： 得到Ip字符串。
算法实现： 
全局变量： 
参    数： u32    dwIp
BOOL32 bBONet -- 传入参数是否为网络序，默认为主机序
返 回 值： s8 * 
=============================================================================*/
/*lint -save -esym(1788,cSemOpt)*/
/*lint -save -esym(727,strofip_hSem)*/
API s8 * strofip(u32 dwIp, BOOL32 bBONet)
{   
    static SEMHANDLE strofip_hSem;
    if ( NULL == strofip_hSem )
    {
        // 创建信号量
        if( !OspSemBCreate(&strofip_hSem) )
        {
            OspSemDelete( strofip_hSem );
            strofip_hSem = NULL;
            OspPrintf( TRUE, FALSE, "[strofip] create handle<0x%x> failed!\n", strofip_hSem );
		}
    }
	
	OspSemTake(strofip_hSem);
//    CSemOpt cSemOpt( strofip_hSem );   // 多线程信号量保护
	
    static s8 s_astrIP[5][18];  
    static u8 byDim = 0;
	
    if (!bBONet)
    {
        dwIp = htonl(dwIp);
    }
	
	u8 *p = (u8 *)&dwIp;
	sprintf(s_astrIP[byDim],"%d.%d.%d.%d%c",p[0],p[1],p[2],p[3],0);
    u8 byDimBak = byDim;
    byDim++;
    if (byDim>=5)
    {
        byDim = 0;
    }

	OspSemGive(strofip_hSem);
	return s_astrIP[byDimBak];
}
/*lint -restore*/

/*=============================================================================
函 数 名：   AddSysDefaultGW
功    能： 
算法实现： 
全局变量： 
参    数： u32 dwGatewayIp  // 主机序
			u8  byEthId      //网卡ID
返 回 值： API void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/12/8   4.0			周广程                  创建
2013-9-17   4.7         彭国锋					支持ubuntu和多网口模式
=============================================================================*/
API void   AddSysDefaultGW( u32 dwGatewayIp, u8 byEthId)
{
    s8 sCmdBuf[256];
    memset( sCmdBuf, 0, sizeof(sCmdBuf) );
#ifdef WIN32
    sprintf( sCmdBuf, "route add 0.0.0.0 mask 0.0.0.0 %s", strofip(dwGatewayIp) );
	system( sCmdBuf );
#endif
#ifdef _LINUX_
	// 使用注意：linux可以同时存在多个默认路由，对外发包，将会使用最后一次添加的默认路由 [pengguofeng 9/18/2013]
    sprintf( sCmdBuf, "route add default gw %s dev eth%d", strofip(dwGatewayIp), byEthId);
	system( sCmdBuf );
#endif
    return;
}

/*=============================================================================
函 数 名：   DelSysDefaultGW
功    能： 
算法实现： 
全局变量： 
参    数： u8 byEthId //网卡ID
返 回 值： API void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/12/8   4.0			周广程                  创建
=============================================================================*/
API void   DelSysDefaultGW( u8 byEthId )
{
#ifdef WIN32
	system( "route delete 0.0.0.0" );
#endif
#ifdef _LINUX_
	if ( GetOsType() == OS_UBUNTU )
	{
		//do ubuntu
		s8 achCmd[128] = {0};
		// 如果有多条默认路由的情况下，只会删除第一条默认路由 [pengguofeng 9/18/2013]
		// 如果只有一条默认路由，则删除它
		sprintf(achCmd, "route del default dev eth%d", byEthId);
		system( achCmd );
		return;
	}
	system( "route delete default" );
#endif
    return;
}

/*=============================================================================
函 数 名： GetOsType
功    能： 取linux操作系统类型
算法实现： 
全局变量： 
参    数： 
返 回 值： u8
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/09/17  4.7			彭国锋                  创建
=============================================================================*/
u8 GetOsType(void)
{
#ifndef WIN32
	s8 achOsTypeStr[64] = {0};
	FILE *fp = popen(JUDGE_LINUX_OS_STR, "r");

	if ( !fp )
	{
		DBG_LOG("lsb_release not supported\n");
		return OS_WIN32;
	}

	fgets(achOsTypeStr, sizeof(achOsTypeStr), fp);

	pclose(fp);
	fp = NULL;

	DBG_LOG("get Os str is %s\n", achOsTypeStr);

	// 只比较前N个字符 [pengguofeng 9/18/2013]
	if ( strncmp(achOsTypeStr, OS_REDHAT_STR, strlen(OS_REDHAT_STR)) == 0 )
	{
		return OS_REDHAT;
	}
	else if ( strncmp(achOsTypeStr, OS_CENTOS_STR, strlen(OS_CENTOS_STR)) == 0 )
	{
		return OS_REDHAT;
	}
	else if ( strncmp(achOsTypeStr, OS_UBUNTU_STR, strlen(OS_UBUNTU_STR)) == 0 )
	{
		return OS_UBUNTU;
	}
	else
	{
		return OS_WIN32;//其他的认为是Win32即可
	}
#endif
	return OS_WIN32;
}

/*=============================================================================
函 数 名： GetNetAdapterInfoActive
功    能： 
算法实现： 
全局变量： 
参    数： TNetAdaptInfoAll * ptNetAdaptInfoAll
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/6/17   4.0			周广程                  创建
=============================================================================*/
BOOL32 GetNetAdapterInfoActive( TNetAdaptInfoAll * ptNetAdaptInfoAll )
{
    if ( NULL == ptNetAdaptInfoAll )
    {
        return FALSE;
    }
#ifdef _LINUX_
    s32 nCount = 0;
    s32 nFd = -1;
    s32 nLoop = 0;
    struct ifconf tIfc;
    struct ifreq  tCopyIfr;	
    struct ifreq atIfcreq[OSP_ETHERNET_ADAPTER_MAX_IP * OSP_ETHERNET_ADAPTER_MAX_NUM];
    struct ifreq *ptIfr = NULL;

    memset(ptNetAdaptInfoAll, 0, sizeof(TNetAdaptInfoAll));
    memset(&tIfc, 0, sizeof(tIfc));
    memset(atIfcreq, 0, OSP_ETHERNET_ADAPTER_MAX_IP * OSP_ETHERNET_ADAPTER_MAX_NUM);

    tIfc.ifc_len = sizeof( struct ifreq) * OSP_ETHERNET_ADAPTER_MAX_IP * OSP_ETHERNET_ADAPTER_MAX_NUM;
    tIfc.ifc_req = (struct ifreq *)(atIfcreq);
    
    nFd = socket(AF_INET, (int)SOCK_DGRAM, 0);
    if (nFd < 0)
    {
        OspPrintf(TRUE,FALSE,"socket error");
        return FALSE;
    }

	if (ioctl(nFd, SIOCGIFCONF, &tIfc) == -1)
	{
		close(nFd);
		OspPrintf(TRUE,FALSE,"ioctl SIOCGIFCONF error %d", errno);
		return FALSE;
	}
	
    ptIfr = tIfc.ifc_req;
    for ( ; tIfc.ifc_len - nCount >= (s32)sizeof(struct ifreq) ; nCount += (s32)sizeof(struct ifreq),  ptIfr += 1)
    {
		strcpy(tCopyIfr.ifr_name, ptIfr->ifr_name);
        s8 asIfName[32] = {0};
        s8 nIfEthIdx = GetEthIdxbyEthIfName( ptIfr->ifr_name );
        sprintf( asIfName, "eth%d", nIfEthIdx );

		//struct sockaddr_in *pSockAddr;
		//struct ethtool_value tEcmd;
		BOOL mFound = FALSE;
	
//		OspPrintf(TRUE,FALSE,"Get Interface name %s", ptIfr->ifr_name);
//         pSockAddr = (struct sockaddr_in*)(&(ptIfr->ifr_addr));
// 		dwIp = pSockAddr->sin_addr.s_addr;
		
        if (ioctl(nFd, SIOCGIFFLAGS, &tCopyIfr) < 0)
        {
			printf("ioctl SIOCGIFINDEX error %d", errno);
            continue;
        }

        // 检验是否是真实网卡
		if (tCopyIfr.ifr_flags & IFF_LOOPBACK)
		{
			continue;
		}
		
        if (ioctl(nFd, SIOCGIFINDEX, &tCopyIfr) < 0)
        {
			printf("ioctl SIOCGIFINDEX error %d", errno);
            continue;
        }
		//nIfIndex = tCopyIfr.ifr_ifindex;

        TNetAdaptInfo * ptNetAdapter = NULL;
        u32 dwAdaptNum = ptNetAdaptInfoAll->m_tLinkNetAdapt.Length();
		for (nLoop = 0; nLoop < (s32)dwAdaptNum; nLoop ++)
		{
			if ( 0 == strcmp( ptNetAdaptInfoAll->m_tLinkNetAdapt.GetValue(nLoop)->GetAdapterName(), asIfName) )
			{
                ptNetAdapter = ptNetAdaptInfoAll->m_tLinkNetAdapt.GetValue(nLoop);
				mFound = TRUE;
				break;
			}
		}
		
        ioctl(nFd,SIOCGIFNETMASK,&tCopyIfr);
        
        u32 dwIfIp = (*(struct sockaddr_in *)(void*)&ptIfr->ifr_addr).sin_addr.s_addr;
        u32 dwIfMask = (*(struct sockaddr_in *)((void*)&tCopyIfr.ifr_addr)).sin_addr.s_addr;
        TNetParam tIpParam;
        tIpParam.SetNetParam( ntohl(dwIfIp), ntohl(dwIfMask) );
					
		/* 如果不是第一次找到该接口名,则只需要更新IP地址即可,可进入下一个ifconfig*/
		if (mFound == TRUE && ptNetAdapter != NULL)
		{
            ptNetAdapter->m_tLinkIpAddr.Append( tIpParam );
			continue;
		}

        TNetAdaptInfo tNewAdapter;
        tNewAdapter.SetAdapterName( asIfName );
        s8 * ptName = tNewAdapter.GetAdapterName();
        if ( 0 != strncmp( ptName, "eth", 3 ) )
        {
            continue;
        }    
        tNewAdapter.SetAdaptIdx( nIfEthIdx );
        
        tNewAdapter.m_tLinkIpAddr.Append( tIpParam );

        u32 dwDefaultGW = 0;
        GetDefaultGW( nIfEthIdx, dwDefaultGW );
        TNetParam tDefGWParam;
        tDefGWParam.SetNetParam( dwDefaultGW, 0 );

// [3/26/2010 xliang] modify save varible
//      tNewAdapter.m_tLinkDefaultGateway.Append( tDefGWParam );
		
		tNewAdapter.SetDefGWIpAddr(dwDefaultGW);
		
		if (ioctl(nFd, SIOCGIFHWADDR, ptIfr))
        {
            OspPrintf(TRUE,FALSE,"ioctl SIOCGIFHWADDR error %d", errno);
            continue;
        }
        tNewAdapter.SetMacAddress( (u8 *)ptIfr->ifr_hwaddr.sa_data, OSP_ETHERNET_ADAPTER_MAC_LENGTH );

		/*
        BOOL32 bAdaptUp = TRUE;
		tEcmd.cmd = ETHTOOL_GLINK;
		tCopyIfr.ifr_data = (caddr_t)&tEcmd;
		if (ioctl(nFd, SIOCETHTOOL, &tCopyIfr) < 0)
		{
            printf("ioctl SIOCETHTOOL error %d", errno);
			bAdaptUp = FALSE;
        }
		else
		{
			if (!tEcmd.data)
			{
				printf("tEcmd.data is 0x%x!\n", tEcmd.data);
				bAdaptUp = FALSE;
			}
		}
		
		if ( bAdaptUp )*/
        {
            ptNetAdaptInfoAll->m_tLinkNetAdapt.Append( tNewAdapter );
        }
    }
	
    close(nFd);
    return TRUE;
#endif
#ifdef WIN32
    HMODULE hModule = LoadLibrary("Iphlpapi.dll");
    if(NULL == hModule)
    {
        OspPrintf( TRUE, FALSE, "[GetNetAdapterInfo]Cannot find \"Iphlpapi.dll\"\n");
        FreeLibrary(hModule); 
        return FALSE;
    }
    
    GETADAPTERSINFO GetAdaptInfo = (GETADAPTERSINFO)GetProcAddress(hModule, "GetAdaptersInfo");
    if(NULL == GetAdaptInfo)
    {
        OspPrintf( TRUE, FALSE, "Cannot find the function \"GetAdaptersInfo\" in \"Iphlpapi.dll\"\n");
        FreeLibrary(hModule); 
        return FALSE;
    }
    
    // 获得实际需要的BUF长度
    u32 dwBufLen = 0;
    PIP_ADAPTER_INFO ptAadpt = NULL;
    u32 dwRet = GetAdaptInfo(ptAadpt, &dwBufLen);
    if(dwRet == ERROR_BUFFER_OVERFLOW)
    {
        //OspPrintf( TRUE, FALSE, "Adapt need %d!\n", dwBufLen );
        ptAadpt = (PIP_ADAPTER_INFO)new u8[dwBufLen];
        if ( NULL == ptAadpt )
        {
            OspPrintf( TRUE, FALSE, "No enough buf for adaptinfo!\n" );
            FreeLibrary(hModule); 
            return FALSE;
        }
        dwRet = GetAdaptInfo( ptAadpt, &dwBufLen );
        if ( dwRet != ERROR_SUCCESS )
        {
            OspPrintf( TRUE, FALSE, "Get adapt info failed!\n" );
            FreeLibrary(hModule); 
            return FALSE;
        }

        PIP_ADAPTER_INFO pNext = ptAadpt;
		u8 byEthIdx = 0;
        while( pNext != NULL )
        {
            TNetAdaptInfo tNetAdapt;
            tNetAdapt.SetAdapterName( pNext->AdapterName );
			//zjj20120314 修改Index,从0计算,windows下该Index含义不同
            tNetAdapt.SetAdaptIdx( byEthIdx++ );//pNext->Index );
            tNetAdapt.SetAdaptType( pNext->Type );
            tNetAdapt.SetDescription( pNext->Description );
            tNetAdapt.SetMacAddress( pNext->Address, pNext->AddressLength );
            
            IP_ADDR_STRING * pIpNext = &(pNext->IpAddressList);
            while ( NULL != pIpNext )
            {
                u32 dwIpAddr = ntohl(INET_ADDR(pIpNext->IpAddress.String));
                u32 dwIpMask = ntohl(INET_ADDR(pIpNext->IpMask.String));
                
                TNetParam tNetParam;
                tNetParam.SetNetParam( dwIpAddr, dwIpMask );
                if ( NULL != pNext->CurrentIpAddress &&
                    ntohl(INET_ADDR(pNext->CurrentIpAddress->IpAddress.String)) == dwIpAddr )
                {
                    tNetAdapt.SetCurretnIpAddr( tNetParam );   
                }
                tNetAdapt.m_tLinkIpAddr.Append( tNetParam );
                pIpNext = pIpNext->Next;
            }
            
            IP_ADDR_STRING * pGatewayNext = &(pNext->GatewayList);
            while ( NULL != pGatewayNext )
            {
                u32 dwIpAddr = ntohl(INET_ADDR(pGatewayNext->IpAddress.String));
                u32 dwIpMask = 0xffffffff;
                TNetParam tNetParam;
                tNetParam.SetNetParam( dwIpAddr, dwIpMask );
				// [3/26/2010 xliang] modify 
// 				tNetAdapt.m_tLinkDefaultGateway.Append( tNetParam );
//              pGatewayNext = pGatewayNext->Next;
				tNetAdapt.SetDefGWIpAddr(dwIpAddr);
				break;
//				pGatewayNext = pGatewayNext->Next;
	
            }
            ptNetAdaptInfoAll->m_tLinkNetAdapt.Append( tNetAdapt );
            pNext = pNext->Next;
        }
    }
    else
    {
        FreeLibrary(hModule); 
        return FALSE;
    }
    
    FreeLibrary(hModule); 
    return TRUE;
#endif
}

/*=============================================================================
函 数 名： GetEthInfo
功    能： 
算法实现： 
全局变量： 
参    数： TEthInfo *ptEthInfo, [OUT]
			u8 &byEthNum,  [OUT] 
			const u8 byEthId,   [IN]
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/17   4.7			彭国锋                  创建
=============================================================================*/
BOOL32 GetEthInfo(TEthInfo *ptEthInfo, u8 &byEthNum, const u8 byEthId /* = INVALID_ETHID */)
{
#ifdef WIN32
	return TRUE;
#endif

	//1. input param
	if ( !ptEthInfo )
	{
		return FALSE;
	}

	//2. ostype != ubuntu
/*	if ( GetOsType() != OS_UBUNTU )
	{
		DBG_LOG("GetEthInfo only handle OS: ubuntu or redhat\n");
//		return FALSE;
	}
*/
	//3. 是否读取所有网卡信息
	BOOL32 bReadAllEth = byEthId == INVALID_ETHID? TRUE:FALSE;

#ifdef _LINUX_
	struct ifreq *ptIfReq;
	struct ifreq tIfReqTemp;
	struct ifconf ifc;
	struct ifreq atifreq[MAX_ETH_NUM*MAX_ETH_IPNUM];
	memset(atifreq, 0, sizeof(atifreq));
	ifc.ifc_len = sizeof(atifreq);
	ifc.ifc_req = (struct ifreq *)atifreq;
	u8 abyEthId[MAX_ETH_NUM] = {0};

	s32 nFd = socket(AF_INET, SOCK_DGRAM, 0);
	if ( nFd == -1 )
	{
		perror("socket SOCK_DGRAM");
		return FALSE;
	}

	//取所有配置过的IP:仅是IP地址
	if ( ioctl(nFd, SIOCGIFCONF, &ifc) < 0 )
	{
		perror("SIOCGIFCONF");
		return FALSE;
	}

	ptIfReq = ifc.ifc_req;
	u32 dwAddLen = 0;
	u8 byEthMainId = INVALID_ETHID;
	u8 byEthSubId = INVALID_ETHID;
	u32 dwIp = 0;
	u32 dwMask = 0;
	u32 dwGw = 0;
	u8 byTempIdx = 0;
	s8 achCmd[256] = {0};
	u8 byOsType = GetOsType();
	FILE *fp = NULL;
	s8 achIp[MAX_MACADDR_STR_LEN] = {0};

	//有可能是乱序的,从eth0:1 ~ eth0:17 再到eth0 !!
	for ( u32 dwAddLen = 0; dwAddLen < ifc.ifc_len; dwAddLen+=sizeof(struct ifreq),ptIfReq++)
	{
		if ( memcmp(ptIfReq->ifr_name, ETH_STR, strlen(ETH_STR)) != 0 )
		{
			continue; // skip "lo"
		}

		// ethIdx
		byEthMainId = GetEthIdxbyEthIfName(ptIfReq->ifr_name);

		if ( !bReadAllEth && byEthMainId != byEthId )
		{
			continue;
		}

 		byEthSubId = GetEthSubIdByEthName(ptIfReq->ifr_name, byEthMainId);

		if ( byEthSubId > MAX_ETH_IPNUM - 1
			&& byEthSubId != INVALID_ETHID )
		{
			continue; //超过16个的不再读
		}
		// IP
		dwIp = ((struct sockaddr_in*)&ptIfReq->ifr_addr)->sin_addr.s_addr;

		strcpy(tIfReqTemp.ifr_name, ptIfReq->ifr_name);
		
		if ( ioctl(nFd, SIOCGIFNETMASK, &tIfReqTemp) < 0 )
		{
			continue;
		}
		// netmask
		dwMask = ((struct sockaddr_in*)&tIfReqTemp.ifr_netmask)->sin_addr.s_addr;

		if ( abyEthId[byEthMainId] == 0)
		{
			abyEthId[byEthMainId] = ++byEthNum;
		}
		byTempIdx = abyEthId[byEthMainId] - 1;
		if ( ptEthInfo[byTempIdx].GetEthId() != byEthMainId )
		{
			ptEthInfo[byTempIdx].SetEthId(byEthMainId);
		}
		//由于顺序可能不固定，采用强制设置，根据byEthSubId
//		ptEthInfo[byTempIdx].AddSubIpMask(ntohl(dwIp), ntohl(dwMask));
		if ( byEthSubId == INVALID_ETHID )
		{
			ptEthInfo[byTempIdx].SetSubIpMaskByIdx(ntohl(dwIp), ntohl(dwMask), 0);
		}
		else
		{
			ptEthInfo[byTempIdx].SetSubIpMaskByIdx(ntohl(dwIp), ntohl(dwMask), byEthSubId);
		}

		// hwaddr
		if ( byEthSubId == INVALID_ETHID )
		{
			if ( ioctl(nFd, SIOCGIFHWADDR, &tIfReqTemp) < 0 )
			{
				continue;
			}
			//只有主IP才能设置MAC地址，其他设置会导致子网卡启不来
			ptEthInfo[byTempIdx].SetMacAddr((u8 *)tIfReqTemp.ifr_hwaddr.sa_data, MAX_MACADDR_NORMAL_LEN);

			//Gateway
			if ( byOsType == OS_UBUNTU )
			{
				sprintf(achCmd, GET_ETH_INFO_CMD, byEthMainId, GW_STR, IPCONF_FILE_UBUNTU);
				sprintf(achCmd, "%s | grep -v ^# |grep %s | awk '{print $2}'", achCmd, GW_STR);
			}
			else if ( byOsType == OS_REDHAT )
			{
				sprintf(achCmd, "cat %s%d |grep -v ^# |grep %s | awk -F'=' '{print $2}'", IPCONF_FILE_REDHAT, byEthMainId, GW_KEY );
			}
			else
			{
				DBG_LOG("not support yet\n");
			}
			
			fp = popen(achCmd, "r");
			if ( fp )
			{
				fgets(achIp, sizeof(achIp), fp);
				if ( strlen(achIp) > 0 && achIp[strlen(achIp)-1] == '\n')
				{
					achIp[strlen(achIp)-1] = 0;
				}
				dwGw = ntohl(inet_addr(achIp));
				ptEthInfo[byTempIdx].SetGateway(dwGw);
				pclose(fp);
			}
		}
	}

	if ( byEthNum == 0 )
	{
		return FALSE; //取得0个也返回FALSE
	}
#endif
	return TRUE;
}

/*=============================================================================
函 数 名： SetEthInfo
功    能： 
注    意： 暂时只对ubuntu/centOS/redhat作处理，其他的需要向这两种保存方式靠拢;
			此接口会写文件,尽可能同时操作多个IP的时候调用此接口!!
算法实现： 
全局变量： 
参    数： TEthInfo *ptEthInfo
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/17   4.7			彭国锋                  创建
=============================================================================*/
BOOL32 SetEthInfo(TEthInfo *ptEthInfo, const u8 byEthNum)
{
#ifdef WIN32
	return TRUE;
#endif

#ifdef _LINUX_
	//1. input param
	if ( !ptEthInfo )
	{
		return FALSE;
	}

	u8 byOsType = GetOsType();

	//2. 保证界面下下来的始终是全部的+最新的，因为这里会删除以前所有的IP，方便写文件
	s8 achCmd[256] = {0}; //按照设置一个IP所需要的最大命令长度所配置，实际长度为127
//	s8 achTempCmd[256] = {0}; //考虑每次要将achCmd压到自己中去
	s32 nRet = 0;
	/*
	 *	内容举例:
	//	for ubuntu
	//////////////////////////////////////////////////////////////////////////
	//特殊注意:system命令调用的是/bin/sh,在ubuntu下默认指向的是dash,这里面的命令和bash有差异
	//解决方法:使用fwrite!
	//////////////////////////////////////////////////////////////////////////

	 echo -e "auto eth0\\niface eth0 inet static\\n
	 address 111.111.111.111\\nnetmask 255.255.255.255\\ngateway 111.111.111.111\\n
	 pre-up ifconfig eth0 hw ether AA:BB:CC:DD:EE:FF >> /etc/network/interfaces"

	//	for centOS/redhat
	 echo -e "TYPE=ethernet\\nONBOOT=yes\\nBOOTPROTO=static\\n
	 DEVICE=eth1\\nIPADDR=172.1.0.15\\n
	 NETMASK=255.255.255.0\\nGATEWAY=172.16.1.0.254\\nMACADDR=AA:BB:CC:DD:EE:FF" > /etc/sysconfig/network-scripts/ifcfg-eth1
	 */

	// 使用fwrite,避免不同shell之间的差异 [pengguofeng 10/16/2013]
	FILE *fp = NULL;
	u32 dwRLen = 0;
	if ( byOsType == OS_UBUNTU )
	{
		fp = fopen(IPCONF_FILE_UBUNTU, "wt");
		if ( !fp)
		{
			DBG_LOG("open ubuntu file:%s failed\n", IPCONF_FILE_UBUNTU);
			return FALSE;
		}

//		sprintf(achCmd, "echo \"auto lo\\niface lo inet loopback\\n\" > %s", IPCONF_FILE_UBUNTU);
		dwRLen = sprintf(achCmd, "auto lo\niface lo inet loopback\n\n");
		
		if ( dwRLen > 0 )
		{
			fwrite(achCmd, 1, dwRLen, fp);
		}
/*		DBG_LOG("exec cmd.[%s]\n",achCmd);
		s32 nRet = system(achCmd);
		if ( nRet == -1 )
		{
			DBG_LOG("exec [%s] failed\n", achCmd);
			return FALSE;
		}
*/	}
	
	TEthInfo *pNetAdaptInfo = NULL;
	u32 dwIp = 0;
	u32 dwMask = 0;
	u8 byEthId = 0;
	s8 achTempStr[64] = {0};
	u8 byLoop = 0;
	s8 achEthFile[128] = {0}; //redhat的网卡配置文件

	//3. save to system file	
	for (u8 byEthLoop = 0;byEthLoop < byEthNum; byEthLoop++)
	{
		pNetAdaptInfo = ptEthInfo + byEthLoop;

		if ( !pNetAdaptInfo )
		{
			DBG_LOG("read %d EthInfo error\n", byEthLoop);
			return FALSE;
		}

		if ( !pNetAdaptInfo->IsValid() )
		{
			continue;//非法的IP就不用写进去了
		}

		byEthId = pNetAdaptInfo->GetEthId();
		DBG_LOG("start to write eth.%d\n", byEthId);

		for (byLoop = 0; byLoop < pNetAdaptInfo->GetIpNum();byLoop++)
		{
			memset(achCmd, 0, sizeof(achCmd));
			memset(achTempStr, 0, sizeof(achTempStr));
			memset(achEthFile, 0, sizeof(achEthFile));
			if ( byLoop == 0 )
			{
				if ( byOsType == OS_UBUNTU )
				{
				}
				else if ( byOsType == OS_REDHAT )
				{
					sprintf(achEthFile, "%s%d",IPCONF_FILE_REDHAT, byEthId);
//					sprintf(achCmd, "echo -e \"%s=eth%d\\n",DEVICE_KEY, byEthId);
				}
				sprintf(achTempStr, "eth%d", byEthId);
			}
			else
			{
				if ( byOsType == OS_UBUNTU )
				{
				}
				else if ( byOsType == OS_REDHAT )
				{
					sprintf(achEthFile, "%s%d:%d",IPCONF_FILE_REDHAT, byEthId, byLoop);
//					sprintf(achCmd, "echo -e \"%s=eth%d:%d\\n",DEVICE_KEY, byEthId, byLoop);
				}
				sprintf(achTempStr, "eth%d:%d", byEthId, byLoop);
			}

			if ( byOsType == OS_UBUNTU )
			{
				dwRLen = sprintf(achCmd, "auto %s\niface %s inet static\n", achTempStr, achTempStr);
				DBG_LOG("write %d bytes to file.%s\n", dwRLen, IPCONF_FILE_UBUNTU);
				if ( dwRLen > 0 )
				{
					fwrite(achCmd, 1, dwRLen, fp);
				}
			}
			else if ( byOsType == OS_REDHAT )
			{
				fp = fopen(achEthFile, "wt");
				if ( !fp )
				{
					DBG_LOG("open redhat file.%s failed\n", achEthFile);
					continue;
				}

				dwRLen = sprintf(achCmd, "%s=%s\n", DEVICE_KEY, achTempStr);
				DBG_LOG("write %d bytes to file.%s\n", dwRLen, achEthFile);
				if ( dwRLen > 0 )
				{
					fwrite(achCmd, 1, dwRLen, fp);
				}
			}

			dwIp = dwMask = 0;
			dwIp = pNetAdaptInfo->GetSubIp(byLoop);
			dwMask = pNetAdaptInfo->GetSubMask(byLoop);
			//strofip(dwIp, FALSE); // 保存文件的是主机序 [pengguofeng 9/18/2013]
// 			memset(achTempCmd, 0, sizeof(achTempCmd));
// 			memcpy(achTempCmd, achCmd, strlen(achCmd));
			memset(achCmd, 0, sizeof(achCmd));
			if ( byOsType == OS_UBUNTU )
			{
				dwRLen = sprintf(achCmd, "%s %s\n%s %s\n", IP_STR, strofip(dwIp, FALSE), NETMASK_STR, strofip(dwMask, FALSE));
				DBG_LOG("write %d bytes to file.%s\n", dwRLen, IPCONF_FILE_UBUNTU);
				if ( dwRLen > 0 )
				{
					fwrite(achCmd, 1, dwRLen, fp);
				}
			}
			else if ( byOsType == OS_REDHAT )
			{
				dwRLen = sprintf(achCmd, "%s=%s\n%s=%s\n", IP_KEY, strofip(dwIp, FALSE), NETMAS_KEY, strofip(dwMask, FALSE));
				DBG_LOG("write %d bytes to file.%s\n", dwRLen, achEthFile);
				if ( dwRLen > 0 )
				{
					fwrite(achCmd, 1, dwRLen, fp);
				}
			}

			if ( byLoop == 0 )
			{
				//gateway: 非0网关设置到文件中去
				dwIp = pNetAdaptInfo->GetGateway();
				if ( dwIp != 0 )
				{
// 					memset(achTempCmd, 0, sizeof(achTempCmd));
// 					memcpy(achTempCmd, achCmd, strlen(achCmd));
					memset(achCmd, 0, sizeof(achCmd));
					if ( byOsType == OS_UBUNTU )
					{
						dwRLen = sprintf(achCmd, "%s %s\n", GW_STR, strofip(dwIp, FALSE));
						DBG_LOG("write %d bytes to file.%s\n", dwRLen, IPCONF_FILE_UBUNTU);
						if ( dwRLen > 0 )
						{
							fwrite(achCmd, 1, dwRLen, fp);
						}
					}
					else if ( byOsType == OS_REDHAT )
					{
						dwRLen = sprintf(achCmd, "%s=%s\n", GW_KEY, strofip(dwIp, FALSE));
						DBG_LOG("write %d bytes to file.%s\n", dwRLen, achEthFile);
						if ( dwRLen > 0 )
						{
							fwrite(achCmd, 1, dwRLen, fp);
						}
					}
				}

				//mac addr
				if ( pNetAdaptInfo->GetMacAddrLen() > 0 ) //有MAC地址就写
				{
					s8 achHwStr[64] = {0};
// 					memset(achTempCmd, 0, sizeof(achTempCmd));
// 					memcpy(achTempCmd, achCmd, strlen(achCmd));
					memset(achCmd, 0, sizeof(achCmd));
					if ( byOsType == OS_UBUNTU )
					{
						sprintf(achHwStr, HWADDR_STR, byEthId);
						dwRLen = sprintf(achCmd, "%s %s\n", achHwStr, pNetAdaptInfo->GetMacAddrStr());
						DBG_LOG("write %d bytes to file.%s\n", dwRLen, IPCONF_FILE_UBUNTU);
						if ( dwRLen > 0 )
						{
							fwrite(achCmd, 1, dwRLen, fp);
						}
					}
					else if ( byOsType == OS_REDHAT)
					{
						dwRLen = sprintf(achCmd, "%s=%s\n", MACADDR_KEY, pNetAdaptInfo->GetMacAddrStr());
						DBG_LOG("write %d bytes to file.%s\n", dwRLen, achEthFile);
						if ( dwRLen > 0 )
						{
							fwrite(achCmd, 1, dwRLen, fp);
						}
					}
				}
			}

// 			memset(achTempCmd, 0, sizeof(achTempCmd));
// 			memcpy(achTempCmd, achCmd, strlen(achCmd));
/*			memset(achCmd, 0, sizeof(achCmd));
			if ( byOsType == OS_UBUNTU )
			{
				sprintf(achCmd, "%s\" >> %s", achTempCmd, IPCONF_FILE_UBUNTU);
			}
			else if ( byOsType == OS_REDHAT )
			{
				sprintf(achCmd, "%sTYPE=%s\\n%s=yes\\n%s=static\" > %s", achTempCmd, TYPE_KEY, ONBOOT_KEY, BOOTPR_KEY, achEthFile);
			}

			DBG_LOG("finally exec cmd.[%s]\n", achCmd);

			nRet = system(achCmd);

			if ( nRet == -1)
			{
				return FALSE;
			}
*/		
			if ( byOsType == OS_REDHAT)
			{
				memset(achCmd, 0, sizeof(achCmd));
				dwRLen = sprintf(achCmd, "TYPE=%s\n%s=yes\n%s=static\n", TYPE_KEY, ONBOOT_KEY, BOOTPR_KEY);
				DBG_LOG("write %d to file.%s\n", dwRLen, achEthFile);
				if ( dwRLen > 0 )
				{
					fwrite(achCmd, 1, dwRLen, fp);
				}
				fclose(fp);
			}
			else if ( byOsType == OS_UBUNTU )
			{
				fwrite("\n", 1, 1, fp);
				fflush(fp);
			}
		}
	}

	if ( byOsType == OS_UBUNTU )
	{
		fclose(fp);
	}
	return TRUE;
#else // ! LINUX
	return FALSE;
#endif
}

/*=============================================================================
函 数 名： DelEthInfo
功    能： 删除网卡信息，并写入配置文件
算法实现： 
全局变量： 
参    数：	u8 *pbyEthId， 待删除的网卡ID
			const u8 byEthNum，待删除的网卡个数，范围：1~MAX_ETH_NUM
返 回 值： BOOL32 成功 TRUE； 失败 FALSE
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
BOOL32 DelEthInfo(u8 *pbyEthId, const u8 byEthNum)
{
#ifdef WIN32
	return TRUE;
#endif

	if ( !pbyEthId || byEthNum == 0 )
	{
		return FALSE; //invalid param
	}

	TEthInfo tEthInfo[MAX_ETH_NUM];
	u8 byUseEthNum = 0;
	if ( !GetEthInfo(tEthInfo, byUseEthNum) )
	{
		return FALSE; //get failed
	}

	if ( byUseEthNum == 0 )
	{
		return FALSE; //get 0 eth
	}

	BOOL32 bFound = FALSE;
	for ( u8 byLoop = 0; byLoop < byUseEthNum; byLoop++)
	{
		for ( u8 byLoop2 = 0; byLoop2 < byEthNum; byLoop2++)
		{
			if ( tEthInfo[byLoop].GetEthId() == pbyEthId[byLoop2] )
			{
				tEthInfo[byLoop].Clear();
				bFound = TRUE;
			}
		}
	}

	if ( !bFound )
	{
		return FALSE;//不需要删除
	}
	else
	{
		return SetEthInfo(tEthInfo, byUseEthNum);
	}
}

/*=============================================================================
函 数 名： SetEthMainIp
功    能： 设置网卡的主IP、MASK、GATEWAY信息，并写入配置文件。
			此接口也可作为添加主IP使用
算法实现： 
全局变量： 
参    数：	u8 byEthId, 待设置的网卡ID
			u32 dwIp,   主机序
			u32 dwMask, 主机序
			u32 dwGw，  主机序
返 回 值： BOOL32 成功 TRUE； 失败 FALSE
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
BOOL32 SetEthMainIp(u8 byEthId, u32 dwIp, u32 dwMask, u32 dwGw)
{
#ifdef WIN32
	return TRUE;
#endif

	if ( byEthId > MAX_ETH_NUM - 1 )
	{
		return FALSE;
	}

	TEthInfo tEthInfo[MAX_ETH_NUM];
	u8 byEthNum = 0;
	if ( !GetEthInfo(tEthInfo, byEthNum/*, byEthId*/) ) //防止只设一个，这里要取所有网卡
	{
		return FALSE;
	}

	BOOL32 bFound = FALSE;
	u8 byAddLoop = INVALID_ETHID;
	for ( u8 byLoop = 0; byLoop < MAX_ETH_NUM/*byEthNum*/; byLoop++ )
	{
		if ( tEthInfo[byLoop].GetEthId() == byEthId )
		{
			tEthInfo[byLoop].SetSubIpMaskByIdx(dwIp, dwMask, 0);
			tEthInfo[byLoop].SetGateway(dwGw);
			bFound = TRUE;
		}
		else if ( !tEthInfo[byLoop].IsValid() ) //有空位置的
		{
			//add，只限于第一次添加主IP，辅IP就不加了
			if ( byAddLoop > byLoop )
			{
				byAddLoop = byLoop; //保存空位
			}
		}
	}

	if ( !bFound )
	{
		if ( byAddLoop == INVALID_ETHID )
		{
			return FALSE;//没有空位
		}
		else
		{
			tEthInfo[byAddLoop].SetEthId(byEthId);
			tEthInfo[byAddLoop].SetSubIpMaskByIdx(dwIp, dwMask, 0);
			tEthInfo[byAddLoop].SetGateway(dwGw);
			byEthNum++;
		}
	}
	return SetEthInfo(tEthInfo, MAX_ETH_NUM);
}

/*=============================================================================
函 数 名： SetEthSubIp
功    能： 设置子网卡的IP、MASK信息，并写入配置文件。在没有主网卡的IP时，不可作为添加子网卡的IP
算法实现： 
全局变量： 
参    数：	u8 byEthId, 待设置的网卡ID
			u8 byEthSubId, 子网卡ID
			u32 dwIp,   主机序
			u32 dwMask, 主机序
返 回 值： BOOL32 成功 TRUE； 失败 FALSE
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
BOOL32	SetEthSubIp(u8 byEthId, u8 byEthSubId, u32 dwIp, u32 dwMask)
{
#ifdef WIN32
	return TRUE;
#endif

	if ( byEthId > MAX_ETH_NUM - 1
		|| byEthSubId > MAX_ETH_IPNUM - 1)
	{
		return FALSE;
	}
	
	TEthInfo tEthInfo[MAX_ETH_NUM];
	u8 byEthNum = 0;
	if ( !GetEthInfo(tEthInfo, byEthNum/*, byEthId*/) )
	{
		return FALSE;
	}
	
	BOOL32 bFound = FALSE;
	for ( u8 byLoop = 0; byLoop < byEthNum; byLoop++ )
	{
		if ( tEthInfo[byLoop].GetEthId() == byEthId )
		{
			tEthInfo[byLoop].SetSubIpMaskByIdx(dwIp, dwMask, byEthSubId);
			bFound = TRUE;
		}
	}

	if ( !bFound )
	{
		return FALSE;
	}

	return SetEthInfo(tEthInfo, byEthNum);
}

/*=============================================================================
函 数 名： GetEthMainIp
功    能： 取主网卡的IP、MASK信息
算法实现： 
全局变量： 
参    数：	u8 byEthId, 待设置的网卡ID
			u8 byEthSubId, 子网卡ID
			u32 &dwIp,   主机序		[OUT]
			u32 &dwMask, 主机序		[OUT]
			u32 &dwGw， 主机序		[OUT]
返 回 值： BOOL32 成功 TRUE； 失败 FALSE
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
BOOL32	GetEthMainIp(u8 byEthId, u32 &dwIp, u32 &dwMask, u32 &dwGw )
{
#ifdef WIN32
	return TRUE;
#endif

	if ( byEthId > MAX_ETH_NUM - 1 )
	{
		return FALSE;
	}
	
	TEthInfo tEthInfo[MAX_ETH_NUM];
	u8 byEthNum = 0;
	if ( !GetEthInfo(tEthInfo, byEthNum, byEthId) )
	{
		return FALSE;
	}
	
	if ( byEthNum != 1 )
	{
		return FALSE;
	}

	dwIp = tEthInfo[0].GetSubIp(0);
	dwMask = tEthInfo[0].GetSubMask(0);
	dwGw = tEthInfo[0].GetGateway();
	return TRUE;
}

/*=============================================================================
函 数 名： GetEthSubIp
功    能： 取子网卡的IP、MASK信息
算法实现： 
全局变量： 
参    数：	u8 byEthId, 待设置的网卡ID
			u8 byEthSubId, 子网卡ID
			u32 &dwIp,   主机序		[OUT]
			u32 &dwMask, 主机序		[OUT]
返 回 值： BOOL32 成功 TRUE； 失败 FALSE
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
BOOL32	GetEthSubIp(u8 byEthId, u8 byEthSubId, u32 &dwIp, u32 &dwMask)
{
#ifdef WIN32
	return TRUE;
#endif

	if ( byEthId > MAX_ETH_NUM - 1
		|| byEthSubId > MAX_ETH_IPNUM - 1)
	{
		return FALSE;
	}
	
	TEthInfo tEthInfo[MAX_ETH_NUM];
	u8 byEthNum = 0;
	if ( !GetEthInfo(tEthInfo, byEthNum, byEthId) )
	{
		return FALSE;
	}
	
	if ( byEthNum != 1 )
	{
		return FALSE;
	}
	
	dwIp = tEthInfo[0].GetSubIp(byEthSubId);
	dwMask = tEthInfo[0].GetSubMask(byEthSubId);
	return TRUE;
}

/*=============================================================================
函 数 名： DelEthIp
功    能： 删除网卡的指定IP、MASK信息
算法实现： 
全局变量： 
参    数：	u8 byEthId, 待设置的网卡ID
			u32 dwIp,   主机序		
返 回 值： BOOL32 成功 TRUE； 失败 FALSE
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
BOOL32	DelEthIp(u8 byEthId, u32 dwIp)
{
#ifdef WIN32
	return TRUE;
#endif
	if ( byEthId > MAX_ETH_NUM - 1 )
	{
		return FALSE;
	}
	
	TEthInfo tEthInfo[MAX_ETH_NUM];
	u8 byEthNum = 0;
	if ( !GetEthInfo(tEthInfo, byEthNum/*, byEthId*/) )
	{
		return FALSE;
	}
	
	BOOL32 bFound = FALSE;
	for (u8 byLoop = 0; byLoop < byEthNum; byLoop++)
	{
		if ( tEthInfo[byLoop].GetEthId() == byEthId )
		{
			tEthInfo[0].DelSubIpMask(dwIp);
			bFound = TRUE;
		}
	}

	if ( !bFound )
	{
		return FALSE;
	}
	else
	{
		return SetEthInfo(tEthInfo, byEthNum);
	}
}

/*=============================================================================
函 数 名： GetNetAdapterInfoAll
功    能： 暂时保留，因为8000H还有用到这个接口，参数在mcustruct.h
算法实现： 
全局变量： 
参    数： TNetAdaptInfoAll * ptNetAdaptInfoAll
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/6/17   4.0			周广程                  创建
=============================================================================*/
BOOL32 GetNetAdapterInfoAll( TNetAdaptInfoAll * ptNetAdaptInfoAll )
{
    GetNetAdapterInfoActive(ptNetAdaptInfoAll);	//获取没有被禁用的网卡信息

    u32 dwActiveNum = ptNetAdaptInfoAll->m_tLinkNetAdapt.Length();
    TNetAdaptInfoAll tUnusedAdaptInfoAll;
    u32 dwUnusedNum = 0;

#ifdef _LINUX_

    printf( "[GetNetAdapterInfoAll] Read unused net card!\n" );

    s8 achFullDir[256] = {0};
    s8 achEthIfName[64] = {0};
    s8 achFullPathName[256] = {0};

    strncpy( achFullDir, "/etc/sysconfig/network-scripts/", sizeof(achFullDir) );

    struct dirent *ent = NULL;
    DIR *pDir = NULL;
    pDir=opendir("/etc/sysconfig/network-scripts/");
    if ( NULL == pDir )
    {
        return FALSE;
    }

    u32 dwActiveIdx = 0;
    u32 dwUnusedIdx = 0;
    while (NULL != (ent=readdir(pDir)))
    {
        //d_type：4表示为目录，8表示为文件
        if (ent->d_type== 4)
        {
			continue;
        }

        if ( !IsValidEthIfFileName(ent->d_name) )
        {
            continue;
        }

        // 网卡名
        u8 byEthIfIdx = GetEthIdxbyEthIfFileName(ent->d_name);
        memset( achEthIfName, 0, sizeof(achEthIfName) );
        sprintf( achEthIfName, "eth%d", byEthIfIdx );
			
		// 全路径名
		memset( achFullPathName, 0, sizeof(achFullPathName) );
		sprintf( achFullPathName, "%s%s", achFullDir, ent->d_name );

        // 先在active 网卡中搜索
        BOOL32 bInAcitve = FALSE;
        for ( dwActiveIdx = 0; dwActiveIdx < dwActiveNum; dwActiveIdx++ )
        {
            TNetAdaptInfo * ptNetAdapt = ptNetAdaptInfoAll->m_tLinkNetAdapt.GetValue(dwActiveIdx);
            if ( NULL == ptNetAdapt )
            {
                continue;
            }
            if( 0 == strcmp( ptNetAdapt->GetAdapterName(), achEthIfName ) )
            {
                bInAcitve = TRUE;
                break;
            }
        }
        if ( bInAcitve )
        {
            continue;
        }

        // 再在未使用网卡中寻找
        BOOL32 bInUnused = FALSE;
        TNetAdaptInfo * ptExistAdapt = NULL;
        for ( dwUnusedIdx = 0; dwUnusedIdx < dwUnusedNum; dwUnusedIdx++ )
        {
            TNetAdaptInfo * ptNetAdapt = tUnusedAdaptInfoAll.m_tLinkNetAdapt.GetValue(dwActiveIdx);
            if ( NULL == ptNetAdapt )
            {
                continue;
            }
            if( 0 == strcmp( ptNetAdapt->GetAdapterName(), achEthIfName ) )
            {
                bInUnused = TRUE;
                ptExistAdapt = ptNetAdapt;
                break;
            }
        }

        s8 achIpAddr[18] = {0};
        WGetRegKeyString( achFullPathName, IP_KEY, achIpAddr, sizeof(achIpAddr) );
        u32 dwIpAddr = ntohl( INET_ADDR( achIpAddr ) );
        s8 achIpMask[18] = {0};
        WGetRegKeyString( achFullPathName, NETMAS_KEY, achIpMask, sizeof(achIpMask) );
        u32 dwIpMask = ntohl( INET_ADDR( achIpMask ) );
        TNetParam tIp;
        tIp.SetNetParam( dwIpAddr, dwIpMask );
        if ( bInUnused && ptExistAdapt != NULL )
        {
            // 已添加网卡，则只需要添加IP地址信息即可
            ptExistAdapt->m_tLinkIpAddr.Append( tIp );    
            continue;
        }
        s8 achDefGW[18] = {0};
        WGetRegKeyString( achFullPathName, GW_KEY, achDefGW, sizeof(achDefGW) );
        u32 dwDefGW = ntohl( INET_ADDR( achDefGW ) );
        TNetParam tGW;
        tGW.SetNetParam( dwDefGW, 0 );

        TNetAdaptInfo tNewUnused;
        tNewUnused.SetAdapterName( achEthIfName );
        tNewUnused.SetAdaptIdx( byEthIfIdx );
        tNewUnused.m_tLinkIpAddr.Append( tIp );
        tNewUnused.m_tLinkDefaultGateway.Append( tGW );

        tUnusedAdaptInfoAll.m_tLinkNetAdapt.Append( tNewUnused );
        dwUnusedNum = tUnusedAdaptInfoAll.m_tLinkNetAdapt.Length();
            
    } // while (NULL != (ent=readdir(pDir)))
    closedir(pDir);

    // 插入未使用网卡信息
    for ( dwUnusedIdx = 0; dwUnusedIdx < dwUnusedNum; dwUnusedIdx++ )
    {
        TNetAdaptInfo * ptUnusedAdapt = tUnusedAdaptInfoAll.m_tLinkNetAdapt.GetValue( dwUnusedIdx );
        if ( NULL == ptUnusedAdapt )
        {
            continue;
        }
        for ( dwActiveIdx = 0; dwActiveIdx < dwActiveNum; dwActiveIdx++ )
        {
            TNetAdaptInfo * ptAcitveAdapt = ptNetAdaptInfoAll->m_tLinkNetAdapt.GetValue(dwActiveIdx);
            if ( NULL == ptAcitveAdapt )
            {
                continue;
            }
            if ( ptUnusedAdapt->GetAdaptIdx() < ptAcitveAdapt->GetAdaptIdx() )
            {
                TNetAdaptInfo tUnused = *ptUnusedAdapt;
                ptNetAdaptInfoAll->m_tLinkNetAdapt.Insert( dwActiveIdx, tUnused );
                dwActiveNum = ptNetAdaptInfoAll->m_tLinkNetAdapt.Length();
                break;
            }
        }
        if ( dwActiveIdx >= dwActiveNum )
        {
            TNetAdaptInfo tUnused = *ptUnusedAdapt;
            ptNetAdaptInfoAll->m_tLinkNetAdapt.Append( tUnused );
            dwActiveNum = ptNetAdaptInfoAll->m_tLinkNetAdapt.Length();
        }
    }
#else
    HDEVINFO hDevInfo = 0;
    
    hDevInfo = SetupDiGetClassDevs( NULL, NULL, NULL, DIGCF_PRESENT|DIGCF_ALLCLASSES );
    if ( INVALID_HANDLE_VALUE == hDevInfo )   
    {   
        printf( "[GetNetAdapterInfoAll]SetupDiGetClassDevs failed!\n" );   
        return FALSE;   
    }

    DWORD i;   
    SP_DEVINFO_DATA DeviceInfoData = {sizeof(SP_DEVINFO_DATA)};   
    
    for ( i = 0; SetupDiEnumDeviceInfo( hDevInfo, i, &DeviceInfoData ); i++)   
    {         
        HKEY   hKeyClass;   
        LONG   lRet;      
      
        //SYSTEM\CurrentControlSet\Control\Class目录下的一个子健
        if ( NULL == ( hKeyClass = SetupDiOpenClassRegKey(&DeviceInfoData.ClassGuid,KEY_READ) ) )
        {
            continue;
        }
  
        // 获得设备类型
        s8 achClassType[150] = {0};
        u32 dwLength = sizeof(achClassType);
        u32  dwType = REG_SZ; 
        lRet = RegQueryValueEx(hKeyClass, TEXT("Class"), NULL, &dwType, (LPBYTE)(achClassType), &dwLength);   
        RegCloseKey(hKeyClass);
        if (lRet != ERROR_SUCCESS)   
        {
            continue;   
        }
        // 不是Net类型，返回FALSE
        if ( 0 != strcmp( achClassType, "Net" ) )   
        {
            continue;   
        }
    
        // 第一次调用，获得实际需要的缓存大小
        s8 * pchBuffer = NULL;       
        u32 dwBufSize = 0;    
        u32 dwErrorCode = GetRegistryProperty( hDevInfo, &DeviceInfoData, SPDRP_DRIVER, pchBuffer, &dwBufSize );       
        if ( ERROR_INSUFFICIENT_BUFFER != dwErrorCode )
        {
            continue;
        }
        // 根据返回参数，重新申请空间
        pchBuffer = new s8[dwBufSize];
        if ( NULL == pchBuffer )
        {
            continue;
        }
        dwErrorCode = GetRegistryProperty( hDevInfo, &DeviceInfoData, SPDRP_DRIVER, pchBuffer, &dwBufSize );
        if ( ERROR_SUCCESS != dwErrorCode )
        {
            continue;
        }
        s8 * pchDriver = new char[dwBufSize+1];
        if ( NULL == pchDriver )
        {
            continue;
        }
        memset( pchDriver, 0, dwBufSize+1 );
        char * pchTmp = pchDriver;
        for ( u32 dwChIdx = 0; dwChIdx < dwBufSize && *(pchBuffer+dwChIdx) != '\0'; dwChIdx++ )
        {
            *pchTmp = *(pchBuffer+dwChIdx);
            pchTmp++;
            if ( *(pchBuffer+dwChIdx) == '\\' )
            {
                *pchTmp = '\\';
                pchTmp++;
            }
        }
        delete [] pchBuffer;
    
        s8  szInterfaceKey[256] = {0};
        sprintf( szInterfaceKey, "SYSTEM\\CurrentControlSet\\Control\\Class\\%s\\Ndi\\Interfaces", pchDriver );
        s8  szDriverKey[256] = {0};
        sprintf( szDriverKey, "SYSTEM\\CurrentControlSet\\Control\\Class\\%s", pchDriver );
        delete [] pchDriver;
    
        HKEY hNdiIntKey;
        lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szInterfaceKey, 0, KEY_READ, &hNdiIntKey);
        if( ERROR_SUCCESS != lRet )
        {
            continue;
        }

        s8 szData[256] = {0};
        dwBufSize = sizeof(szData);
        u32 dwDataType;
        lRet = RegQueryValueEx(hNdiIntKey, "LowerRange", 0, &dwDataType, (LPBYTE)szData, &dwBufSize);
        RegCloseKey(hNdiIntKey);
        if(ERROR_SUCCESS != lRet)
        { 
            continue;
        }
        // 判断是不是以太网卡
        if(strcmp((char*)szData, "ethernet") != 0) 
        {          
            continue;
        }   

        // 获得网卡名称
        HKEY hDriverKey;
        lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szDriverKey, 0, KEY_READ, &hDriverKey);
        if( ERROR_SUCCESS != lRet )
        {
            continue;
        }
        memset( szData, 0, sizeof(szData) );
        dwBufSize = sizeof(szData);
        lRet = RegQueryValueEx(hDriverKey, "NetCfgInstanceId", 0, &dwDataType, (LPBYTE)szData, &dwBufSize);       
        if(ERROR_SUCCESS != lRet)
        { 
            RegCloseKey(hDriverKey);
            continue;
        }

        // 先在active 网卡中搜索
        BOOL32 bInAcitve = FALSE;
        u32 dwActiveIdx;
        for ( dwActiveIdx = 0; dwActiveIdx < dwActiveNum; dwActiveIdx++ )
        {
            TNetAdaptInfo * ptNetAdapt = ptNetAdaptInfoAll->m_tLinkNetAdapt.GetValue(dwActiveIdx);
            if ( NULL == ptNetAdapt )
            {
                continue;
            }
            if( 0 == strcmp( ptNetAdapt->GetAdapterName(), (s8*)szData ) )
            {
                bInAcitve = TRUE;
                break;
            }
        }
        if ( bInAcitve )
        {
            RegCloseKey(hDriverKey);
            continue;
        }
        
        // 再在未使用网卡中寻找
        BOOL32 bInUnused = FALSE;
        u32 dwUnusedIdx;
        for ( dwUnusedIdx = 0; dwUnusedIdx < dwUnusedNum; dwUnusedIdx++ )
        {
            TNetAdaptInfo * ptNetAdapt = tUnusedAdaptInfoAll.m_tLinkNetAdapt.GetValue(dwActiveIdx);
            if ( NULL == ptNetAdapt )
            {
                continue;
            }
            if( 0 == strcmp( ptNetAdapt->GetAdapterName(), (s8*)szData ) )
            {
                bInUnused = TRUE;
                break;
            }
        }
        if ( bInUnused )
        {
            RegCloseKey(hDriverKey);
            continue;
        }

        TNetAdaptInfo tNewUnused;
        tNewUnused.SetAdapterName( (s8*)szData );

        memset( szData, 0, sizeof(szData) );
        dwBufSize = sizeof(szData);
        lRet = RegQueryValueEx(hDriverKey, "DriverDesc", 0, &dwDataType, (LPBYTE)szData, &dwBufSize);
        RegCloseKey(hDriverKey);
        if(ERROR_SUCCESS != lRet)
        { 
            continue;
        }
        tNewUnused.SetDescription( (s8*)szData );

        // 获得IP
        u32 dwIpNum = 0;
        if ( ERROR_NOT_ENOUGH_MEMORY != RegGetIpAdr( tNewUnused.GetAdapterName(), (u32*)NULL, dwIpNum ) || 0 == dwIpNum)
        {
            continue;
        }
        u32 * pdwIpSet = new u32[dwIpNum];
        if ( NULL == pdwIpSet )
        {
            continue;
        }
        if ( ERROR_SUCCESS != RegGetIpAdr( tNewUnused.GetAdapterName(), pdwIpSet, dwIpNum ) )
        {
            continue;
        }
         
        //获得掩码
        u32 dwMaskNum = 0;
        if ( ERROR_NOT_ENOUGH_MEMORY != RegGetNetMask( tNewUnused.GetAdapterName(), (u32*)NULL, dwMaskNum ) || 0 == dwMaskNum )
        {
            continue;
        }
        u32 * pdwMaskSet = new u32[dwMaskNum];
        if ( NULL == pdwMaskSet )
        {
            continue;
        }
        if ( ERROR_SUCCESS != RegGetNetMask( tNewUnused.GetAdapterName(), pdwMaskSet, dwMaskNum ) )
        {
            continue;
        }

        dwIpNum = min( dwIpNum, dwMaskNum );
        TNetParam tIpAddr;
        u32 dwIpIdx = 0;
        for ( dwIpIdx = 0; dwIpIdx < dwIpNum; dwIpIdx++ )
        {
            memset( &tIpAddr, 0, sizeof(tIpAddr) );
            tIpAddr.SetNetParam( pdwIpSet[dwIpIdx], pdwMaskSet[dwIpIdx] );
            tNewUnused.m_tLinkIpAddr.Append( tIpAddr );
        }

        //获得默认网关
        u32 dwDefGateNum = 0;
        if ( ERROR_NOT_ENOUGH_MEMORY != RegGetNetGate( tNewUnused.GetAdapterName(), (u32*)NULL, dwDefGateNum ) || 0 == dwDefGateNum )
        {
            continue;
        }
        u32 * pdwDefGateSet = new u32[dwDefGateNum];
        if ( NULL == pdwDefGateSet )
        {
            continue;
        }
        if ( ERROR_SUCCESS != RegGetNetGate( tNewUnused.GetAdapterName(), pdwDefGateSet, dwDefGateNum ) )
        {
            continue;
        }
    
        u32 dwDefGateIdx = 0;
        TNetParam tDefGate;
        for ( dwDefGateIdx = 0; dwDefGateIdx < dwDefGateNum; dwDefGateIdx++ )
        {
            memset( &tDefGate, 0, sizeof(tDefGate) );
            tDefGate.SetNetParam( pdwDefGateSet[dwDefGateIdx], 0 );
            tNewUnused.m_tLinkDefaultGateway.Append( tDefGate );
        }

        tUnusedAdaptInfoAll.m_tLinkNetAdapt.Append( tNewUnused );
    }

    // 插入未使用网卡信息
    dwUnusedNum = tUnusedAdaptInfoAll.m_tLinkNetAdapt.Length();
    u32 dwUnusedIdx = 0;
    for ( dwUnusedIdx = 0; dwUnusedIdx < dwUnusedNum; dwUnusedIdx++ )
    {
        TNetAdaptInfo * ptUnusedAdapt = tUnusedAdaptInfoAll.m_tLinkNetAdapt.GetValue( dwUnusedIdx );
        if ( NULL == ptUnusedAdapt )
        {
            continue;
        }
        TNetAdaptInfo tUnused = *ptUnusedAdapt;
        ptNetAdaptInfoAll->m_tLinkNetAdapt.Append( tUnused );
        dwActiveNum = ptNetAdaptInfoAll->m_tLinkNetAdapt.Length();
    }
#endif

    return TRUE;
}


/*=============================================================================
函 数 名： GetNetAdapterInfo
功    能： 
算法实现： 
全局变量： 
参    数： TNetAdaptInfoAll * ptNetAdaptInfoAll
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/4/1   4.0		周广程                  创建
=============================================================================*/
BOOL32 GetNetAdapterInfo( TNetAdaptInfoAll * ptNetAdaptInfoAll, BOOL32 bActive )
{
    if ( bActive )
    {
        return GetNetAdapterInfoActive( ptNetAdaptInfoAll );
    }
    else
    {
        return GetNetAdapterInfoAll( ptNetAdaptInfoAll );
    }
}


/*=============================================================================
函 数 名： GetAdapterIp
功    能： 获取指定网卡的IP配置
算法实现： 
全局变量： 
参    数： [IN]u32 dwIfIndex:网口号
           [IN/OUT]TBrdEthParamAll* ptAllEthParm:
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2010/4/12   4.0		    付秀华                  创建
2010/4/12	4.6			薛亮					modify
=============================================================================*/
BOOL32 GetAdapterIp( u32 dwIfIndex, TMcuBrdEthParamAll* ptAllEthParm)
{

	if ( dwIfIndex > MCU_MAXNUM_ADAPTER-1  ||
		 NULL == ptAllEthParm)
	{
		return FALSE;
	}

	BOOL32 bRet = FALSE;
	memset(ptAllEthParm, 0, sizeof(TMcuBrdEthParamAll));

	TNetAdaptInfoAll tNetAdapterInfoAll;
    if ( GetNetAdapterInfo( &tNetAdapterInfoAll ) )
	{
		TNetAdaptInfo *ptNetAdapt = tNetAdapterInfoAll.GetAdapterByEthId(dwIfIndex);
		if (ptNetAdapt != NULL)
		{
			 printf("---------------------------------\n"); 
             ptNetAdapt->Print(); 
             printf("---------------------------------\n"); 
			u32 dwIpNum= 0;
			for(; dwIpNum < MCU_MAXNUM_ADAPTER_IP; dwIpNum ++)
			{
				TNetParam *ptNetParam =  ptNetAdapt->m_tLinkIpAddr.GetValue(dwIpNum);
				if(ptNetParam == NULL)
				{
					break;
				}
				ptAllEthParm->atBrdEthParam[dwIpNum].dwIpAdrs = htonl(ptNetParam->GetIpAddr());
				ptAllEthParm->atBrdEthParam[dwIpNum].dwIpMask = htonl(ptNetParam->GetIpMask());
				memcpy(ptAllEthParm->atBrdEthParam[dwIpNum].byMacAdrs, ptNetAdapt->GetMacAddress(), sizeof(ptAllEthParm->atBrdEthParam[dwIpNum].byMacAdrs));
			}
			ptAllEthParm->dwIpNum = dwIpNum;
			
			bRet = TRUE;
		} 
        else 
        { 
           printf("no match active NetAdapter for eth%d\n", dwIfIndex); 
        } 


	}

	return bRet;
/*
	// 指定网卡
	s8 achPathName[64] ={0};
    s8 achFileName[64] = {0};
#ifdef _LINUX_
    sprintf( achPathName, "/etc/sysconfig/network-scripts/");
	sprintf( achFileName, "ifcfg-eth%d", dwIfIndex );

    DIR *pDir = NULL;
    pDir = opendir(achPathName);
    if ( NULL == pDir )
    {
        return FALSE;
    }

	struct dirent *ent = NULL;
    u8 byFileNameLen = strlen( achFileName );
	s8 asIpAddrStr[32] = {0};
	s8 asNetMaskStr[32] = {0};
	s8 asMacStr[32] = {0};
	u32 dwIpNum = 0;
    while (NULL != (ent=readdir(pDir)))
    {
		//判断是否为相对应的网口配置文件
        //d_type：4表示为目录，8表示为文件
        if (ent->d_type== 4)
        {
			continue;
        }

        u8 byLocalLen = strlen(ent->d_name);
        
        if ( byLocalLen < byFileNameLen )
        {
            continue;
        }

        if ( memcmp(achFileName, ent->d_name, byFileNameLen) != 0 )
        {
			// 非同一网卡文件，跳过
            continue;
        }
        else if ( byLocalLen > byFileNameLen )
        {
            s8 * pch = ent->d_name+byFileNameLen;
            if ( *pch != ':' )
            {
				// 非合法网卡配置文件，跳过
                continue;
            }
            // 从':'后面开始检查
            pch++;
            u8 byChIdx = 0;
            for ( byChIdx = 0; byChIdx < byLocalLen-byFileNameLen-1; byChIdx++ )
            {
                if ( *(pch+byChIdx) < '0' || *(pch+byChIdx) > '9' )
                {
                    // 文件名不合法，退出
                    break;
                }
            }   
            if ( byChIdx < byLocalLen-byFileNameLen-1 )
            {
                continue;
            }
        }


		// 全路径名
		memset( achFullPathName, 0, sizeof(achFullPathName) );
		sprintf( achFullPathName, "%s%s", achPathName, ent->d_name );

        memset( asIpAddrStr, 0, sizeof(asIpAddrStr) );
		WGetRegKeyString(achFullPathName, IP_KEY, asIpAddrStr, sizeof(asIpAddrStr));
        memset( asNetMaskStr, 0, sizeof(asNetMaskStr) );
		WGetRegKeyString(achFullPathName, NETMAS_KEY, asNetMaskStr, sizeof(asNetMaskStr));
        memset( asMacStr, 0, sizeof(asMacStr) );
		WGetRegKeyString(achFullPathName, HWADDR_KEY, asMacStr, sizeof(asMacStr));

		if (dwIpNum >= IP_ADDR_MAX_NUM)
		{
			break;
		}
		ptAllEthParm->atBrdEthParam[dwIpNum].dwIpAdrs = INET_ADDR(asIpAddrStr);
		ptAllEthParm->atBrdEthParam[dwIpNum].dwIpMask = INET_ADDR(asNetMaskStr);
		memcpy(ptAllEthParm->atBrdEthParam[dwIpNum].byMacAdrs, asMacStr, min(sizeof(ptAllEthParm->atBrdEthParam[dwIpNum].byMacAdrs), sizeof(asMacStr)));

	}

	ptAllEthParm->dwIpNum = dwIpNum;
#endif
	
	return TRUE;
*/
}


//////////////仅在win32下用////////////////
#ifdef WIN32
/*=============================================================================
函 数 名： RegEnableDHCP
功    能： 
算法实现： 
全局变量： 
参    数：  s8* lpszAdpName
           BOOL32 bEnable
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/5/12   4.0		周广程                  创建
=============================================================================*/
BOOL32 RegEnableDHCP( const s8* lpszAdpName, BOOL32 bEnable )
{
    if( NULL == lpszAdpName )
    {
        return FALSE;
    }
    
    HKEY hKey;
    s8 aszStrKeyName[1024] = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
    strcat(aszStrKeyName, lpszAdpName);
    if( ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, (const s8*)aszStrKeyName, 0, KEY_WRITE, &hKey) != ERROR_SUCCESS)
    {
        return FALSE;
    }
    
    u32 dwEnable = bEnable ? 1 : 0;

    if ( ERROR_SUCCESS != RegSetValueEx(hKey, "EnableDHCP", 0, REG_DWORD, (unsigned char*)&dwEnable, sizeof(dwEnable)) )
    {
        RegCloseKey(hKey);
        return FALSE;
    }
    
    RegCloseKey(hKey);
    
    return TRUE;
}

/*=============================================================================
函 数 名： RegIsEnableDHCP
功    能： 
算法实现： 
全局变量： 
参    数： s8* lpszAdpName
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/5/12   4.0		周广程                  创建
=============================================================================*/
BOOL32 RegIsEnableDHCP( const s8* lpszAdpName )
{
    if( NULL == lpszAdpName )
    {
        return FALSE;
    }
    
    HKEY hKey;
    s8 aszStrKeyName[1024] = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
    strcat(aszStrKeyName, lpszAdpName);
    if( ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, (const s8*)aszStrKeyName, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
    {
        return FALSE;
    }
    
    DWORD dwEnable;
    DWORD dwlen = sizeof(dwEnable);

    DWORD dwType = REG_DWORD;
    if ( ERROR_SUCCESS != RegQueryValueEx(hKey, "EnableDHCP", 0, &dwType, (unsigned char *)&dwEnable, &dwlen ) )
    {
        RegCloseKey(hKey);
        return FALSE;
    }
    
    BOOL32 bEnable = ( dwEnable == 1 ) ? TRUE : FALSE; 

    RegCloseKey(hKey);
        
    return bEnable;
}

/*=============================================================================
  函 数 名： RegGetAdpName
  功    能： 
  算法实现： 
  全局变量： 
  参    数： s8* lpszAdpName
             u8 byLen
  返 回 值： BOOL32 
=============================================================================*/
// BOOL32 RegGetAdpName(s8* lpszAdpName, u16 wLen)
// {
//     if(NULL == lpszAdpName)
//     {
//         return FALSE;
//     }
// 
//     HKEY hKey, hSubKey, hNdiIntKey;
// 
//     if( ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE,
//             "System\\CurrentControlSet\\Control\\Class\\{4d36e972-e325-11ce-bfc1-08002be10318}",
//             0, KEY_READ, &hKey) != ERROR_SUCCESS)
//         return FALSE;
// 
//     DWORD dwIndex = 0;
//     DWORD dwBufSize = 256;
//     DWORD dwDataType;
//     char  szSubKey[256];
//     unsigned char szData[256];
//     BOOL32 bFlag = FALSE;
// 
//     while(ERROR_SUCCESS == RegEnumKeyEx(hKey, dwIndex++, szSubKey, &dwBufSize, NULL, NULL, NULL, NULL))
//     {
//         if(ERROR_SUCCESS == RegOpenKeyEx(hKey, szSubKey, 0, KEY_READ, &hSubKey))
//         { 
//             if(ERROR_SUCCESS == RegOpenKeyEx(hSubKey, "Ndi\\Interfaces", 0, KEY_READ, &hNdiIntKey))
//             {
//                 memset(szData, '\0', sizeof(szData));
//                 dwBufSize = 256;
//                 if(ERROR_SUCCESS == RegQueryValueEx(hNdiIntKey, "LowerRange", 0, &dwDataType, szData, &dwBufSize))
//                 {
//                     if(strcmp((char*)szData, "ethernet") == 0) // 判断是不是以太网卡
//                     {
//                         memset(szData, '\0', sizeof(szData));
//                         dwBufSize = 256;
//                         if(ERROR_SUCCESS == RegQueryValueEx(hSubKey, "DriverDesc", 0, &dwDataType, szData, &dwBufSize))
//                         {
//                             // szData 中便是适配器详细描述
//                             memset(szData, '\0', sizeof(szData));    
//                             dwBufSize = 256;
//                             if(ERROR_SUCCESS == RegQueryValueEx(hSubKey, "NetCfgInstanceID", 0, &dwDataType, szData, &dwBufSize))
//                             {
//                                 // szData 中便是适配器名称
//                                 wLen = (wLen < strlen((const char*)szData)) ? wLen : (u16)strlen((const char*)szData); // 长度保护
//                                 strncpy(lpszAdpName, (const char*)szData, wLen);
//                                 bFlag = TRUE;
//                             }
//                         }
//                     }
//                 }
//                 if ( ERROR_SUCCESS != RegCloseKey(hNdiIntKey) )
//                 {
//                     return FALSE;
//                 }
//             }
//             if ( ERROR_SUCCESS != RegCloseKey(hSubKey) )
//             {
//                 return FALSE;
//             }
//         }
// 
//         dwBufSize = 256;
//         if(bFlag)
//         {
//             break; // 找到，退出while循环
//         }
//     } /* end of while */
// 
//     RegCloseKey(hKey);
// 
//     return TRUE;
// }

/*=============================================================================
函 数 名： RegGetIpAdr
功    能： 
算法实现： 
全局变量： 
参    数： s8* lpszAdapterName
           s8* pIpAddr
           u32 &dwLen
返 回 值： u32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/11/30   4.0		周广程                  创建
=============================================================================*/
u32 RegGetIpAdr(s8* lpszAdapterName, s8* pIpAddr, u32 &dwLen)
{
    u32 dwRet = ERROR_SUCCESS;

    if( NULL == lpszAdapterName )
    {
        return ERROR_INVALID_PARAMETER;
    }    

    HKEY hKey;
    s8 aszStrKeyName[1024] = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
    strcat(aszStrKeyName, lpszAdapterName);
    dwRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, (const s8*)aszStrKeyName, 0, KEY_READ, &hKey);
    if( ERROR_SUCCESS != dwRet )
    {
        return dwRet;
    }

    u32 dwType = REG_MULTI_SZ;
    u32 dwIpLen = 0;
    dwRet = RegQueryValueEx(hKey, "IPAddress", 0, &dwType, NULL, (unsigned long*)&dwIpLen);
    if ( ERROR_SUCCESS != dwRet )
    {
        RegCloseKey(hKey);
        return dwRet;
    }
    if ( pIpAddr == NULL || dwLen < dwIpLen )
    {      
        dwLen = dwIpLen;
        return ERROR_NOT_ENOUGH_MEMORY;
    }

    s8 * psIpSet = new s8[dwIpLen];
    if ( NULL == psIpSet )
    {
        RegCloseKey(hKey);
        return ERROR_OUTOFMEMORY;
    }

    dwRet = RegQueryValueEx(hKey, "IPAddress", 0, &dwType, (LPBYTE)psIpSet, (unsigned long*)&dwIpLen);
    RegCloseKey(hKey);
    if ( ERROR_SUCCESS != dwRet )
    {       
        delete [] psIpSet;
        return dwRet;
    }

    memcpy(pIpAddr, psIpSet, dwIpLen);
    dwLen = dwIpLen;
    delete [] psIpSet;

    return ERROR_SUCCESS;
}

/*=============================================================================
函 数 名： RegGetIpAdr
功    能： 
算法实现： 
全局变量： 
参    数： s8* lpszAdapterName
           u32* pdwIpAddr 
           u32 &dwLen
返 回 值： u16 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/6/24   4.0			周广程                  创建
=============================================================================*/
u32 RegGetIpAdr(s8* lpszAdapterName, u32* pdwIpAddr, u32 &dwLen)
{
    u32 dwBufLen = 0;
    u32 dwRet = ERROR_SUCCESS;
    
    dwRet = RegGetIpAdr( lpszAdapterName, (s8*)NULL, dwBufLen );
    if ( dwRet != ERROR_NOT_ENOUGH_MEMORY )
    {
        return dwRet;
    }

    s8 * psIpSet = new s8[dwBufLen];
    if ( NULL == psIpSet )
    {
        return ERROR_OUTOFMEMORY;
    }
    
    dwRet = RegGetIpAdr( lpszAdapterName, psIpSet, dwBufLen );
    if ( ERROR_SUCCESS != dwRet )
    {
        delete [] psIpSet;
        return dwRet;
    }

    u32 dwIpNum = 0;
    u32 dwIdx = 0;
    for ( dwIdx = 0; dwIdx < dwBufLen; dwIdx++ )
    {
        if ( *(psIpSet+dwIdx) == '\0' )
        {
            dwIpNum++;
            if ( *(psIpSet+dwIdx+1) == '\0' )
            {
                break;
            }
        }
    }

    if ( NULL == pdwIpAddr || dwLen < dwIpNum )
    {
        delete [] psIpSet;
        dwLen = dwIpNum;
        return ERROR_NOT_ENOUGH_MEMORY;
    }

    dwLen = dwIpNum;
    s8 * pRecIp = NULL;
    u32 * pdwTmpIp = NULL;
    for ( dwIdx = 0, pRecIp = psIpSet, pdwTmpIp = pdwIpAddr; dwIdx < dwBufLen; dwIdx++ )
    {
        if ( *(psIpSet+dwIdx) == '\0' )
        {
            *pdwTmpIp = ntohl(INET_ADDR(pRecIp));
            if ( *(psIpSet+dwIdx+1) == '\0' )
            {
                break;
            }
            pRecIp += dwIdx+1;
            pdwTmpIp++;
        }
    }

    delete [] psIpSet;
    return ERROR_SUCCESS;
}

/*=============================================================================
  函 数 名： RegGetNetMask
  功    能： 
  算法实现： 
  全局变量： 
  参    数： s8* lpszAdapterName
             s8* pNetMask
             u32 &dwLen
  返 回 值： u16 
=============================================================================*/
u32 RegGetNetMask(s8* lpszAdapterName, s8* pNetMask, u32 &dwLen)
{
    u32 dwRet = ERROR_SUCCESS;
    
    if( NULL == lpszAdapterName )
    {
        return ERROR_INVALID_PARAMETER;
    }    
    
    HKEY hKey;
    s8 aszStrKeyName[1024] = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
    strcat(aszStrKeyName, lpszAdapterName);
    dwRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, (const s8*)aszStrKeyName, 0, KEY_READ, &hKey);
    if( ERROR_SUCCESS != dwRet )
    {
        return dwRet;
    }
    
    u32 dwType = REG_MULTI_SZ;
    u32 dwBufLen = 0;
    dwRet = RegQueryValueEx(hKey, "SubnetMask", 0, &dwType, NULL, &dwBufLen);
    if ( ERROR_SUCCESS != dwRet )
    {
        RegCloseKey(hKey);
        return dwRet;
    }
    if ( pNetMask == NULL || dwLen < dwBufLen )
    {      
        dwLen = dwBufLen;
        return ERROR_NOT_ENOUGH_MEMORY;
    }
    
    s8 * psMaskSet = new s8[dwBufLen];
    if ( NULL == psMaskSet )
    {
        RegCloseKey(hKey);
        return ERROR_OUTOFMEMORY;
    }
    
    dwRet = RegQueryValueEx(hKey, "SubnetMask", 0, &dwType, (LPBYTE)psMaskSet, &dwBufLen);
    RegCloseKey(hKey);
    if ( ERROR_SUCCESS != dwRet )
    {       
        delete [] psMaskSet;
        return dwRet;
    }
    
    memcpy(pNetMask, psMaskSet, dwBufLen);
    dwLen = dwBufLen;
    delete [] psMaskSet;
    
    return ERROR_SUCCESS;
}

/*=============================================================================
函 数 名： RegGetNetMask
功    能： 
算法实现： 
全局变量： 
参    数： s8* lpszAdapterName
           u32* pdwNetMask
           u32 &dwLen
返 回 值： u16 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/6/24   4.0			周广程                  创建
=============================================================================*/
u32 RegGetNetMask(s8* lpszAdapterName, u32* pdwNetMask, u32 &dwLen)
{
    u32 dwBufLen = 0;
    u32 dwRet = ERROR_SUCCESS;
    
    dwRet = RegGetNetMask( lpszAdapterName, (s8*)NULL, dwBufLen );
    if ( dwRet != ERROR_NOT_ENOUGH_MEMORY )
    {
        return dwRet;
    }
    
    s8 * psMaskSet = new s8[dwBufLen];
    if ( NULL == psMaskSet )
    {
        return ERROR_OUTOFMEMORY;
    }
    
    dwRet = RegGetNetMask( lpszAdapterName, psMaskSet, dwBufLen );
    if ( ERROR_SUCCESS != dwRet )
    {
        delete [] psMaskSet;
        return dwRet;
    }
    
    u32 dwMaskNum = 0;
    u32 dwIdx = 0;
    for ( dwIdx = 0; dwIdx < dwBufLen; dwIdx++ )
    {
        if ( *(psMaskSet+dwIdx) == '\0' )
        {
            dwMaskNum++;
            if ( *(psMaskSet+dwIdx+1) == '\0' )
            {
                break;
            }
        }
    }
    
    if ( NULL == pdwNetMask || dwLen < dwMaskNum )
    {
        delete [] psMaskSet;
        dwLen = dwMaskNum;
        return ERROR_NOT_ENOUGH_MEMORY;
    }
    
    dwLen = dwMaskNum;
    s8 * pRecMask = NULL;
    u32 * pdwTmpMask = NULL;
    for ( dwIdx = 0, pRecMask = psMaskSet, pdwTmpMask = pdwNetMask; dwIdx < dwBufLen; dwIdx++ )
    {
        if ( *(psMaskSet+dwIdx) == '\0' )
        {
            *pdwTmpMask = ntohl(INET_ADDR(pRecMask));
            if ( *(psMaskSet+dwIdx+1) == '\0' )
            {
                break;
            }
            pRecMask += dwIdx+1;
            pdwTmpMask++;
        }
    }
    
    delete [] psMaskSet;
    return ERROR_SUCCESS;
}

/*=============================================================================
函 数 名： RegGetNetGate
功    能： 
算法实现： 
全局变量： 
参    数： s8* lpszAdapterName
           s8* pNetGate
           u32 &dwLen
返 回 值： u32 
=============================================================================*/
u32 RegGetNetGate(s8* lpszAdapterName, s8* pNetGate, u32 &dwLen)
{
    u32 dwRet = ERROR_SUCCESS;
    
    if( NULL == lpszAdapterName )
    {
        return ERROR_INVALID_PARAMETER;
    }    
    
    HKEY hKey;
    s8 aszStrKeyName[1024] = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
    strcat(aszStrKeyName, lpszAdapterName);
    dwRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, (const s8*)aszStrKeyName, 0, KEY_READ, &hKey);
    if( ERROR_SUCCESS != dwRet )
    {
        return dwRet;
    }
    
    u32 dwType = REG_MULTI_SZ;
    u32 dwBufLen = 0;
    dwRet = RegQueryValueEx(hKey, "DefaultGateway", 0, &dwType, NULL, &dwBufLen);
    if ( ERROR_SUCCESS != dwRet )
    {
        RegCloseKey(hKey);
        return dwRet;
    }
    if ( pNetGate == NULL || dwLen < dwBufLen )
    {      
        dwLen = dwBufLen;
        return ERROR_NOT_ENOUGH_MEMORY;
    }
    
    s8 * psNegGateSet = new s8[dwBufLen];
    if ( NULL == psNegGateSet )
    {
        RegCloseKey(hKey);
        return ERROR_OUTOFMEMORY;
    }
    
    dwRet = RegQueryValueEx(hKey, "DefaultGateway", 0, &dwType, (LPBYTE)psNegGateSet, &dwBufLen);
    RegCloseKey(hKey);
    if ( ERROR_SUCCESS != dwRet )
    {       
        delete [] psNegGateSet;
        return dwRet;
    }
    
    memcpy(pNetGate, psNegGateSet, dwBufLen);
    dwLen = dwBufLen;
    delete [] psNegGateSet;
    
    return ERROR_SUCCESS;
}

/*=============================================================================
函 数 名： RegGetNetGate
功    能： 
算法实现： 
全局变量： 
参    数： s8* lpszAdapterName
           u32* pdwNetGate
           u32 &dwLen
返 回 值： u32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/6/24   4.0			周广程                  创建
=============================================================================*/
u32 RegGetNetGate(s8* lpszAdapterName, u32* pdwNetGate, u32 &dwLen)
{
    u32 dwBufLen = 0;
    u32 dwRet = ERROR_SUCCESS;
    
    dwRet = RegGetNetGate( lpszAdapterName, (s8*)NULL, dwBufLen );
    if ( dwRet != ERROR_NOT_ENOUGH_MEMORY )
    {
        return dwRet;
    }
    
    s8 * psNetGateSet = new s8[dwBufLen];
    if ( NULL == psNetGateSet )
    {
        return ERROR_OUTOFMEMORY;
    }
    
    dwRet = RegGetNetGate( lpszAdapterName, psNetGateSet, dwBufLen );
    if ( ERROR_SUCCESS != dwRet )
    {
        delete [] psNetGateSet;
        return dwRet;
    }
    
    u32 dwNetGateNum = 0;
    u32 dwIdx = 0;
    for ( dwIdx = 0; dwIdx < dwBufLen; dwIdx++ )
    {
        if ( *(psNetGateSet+dwIdx) == '\0' )
        {
            dwNetGateNum++;
            if ( *(psNetGateSet+dwIdx+1) == '\0' )
            {
                break;
            }
        }
    }
    
    if ( NULL == pdwNetGate || dwLen < dwNetGateNum )
    {
        delete [] psNetGateSet;
        dwLen = dwNetGateNum;
        return ERROR_NOT_ENOUGH_MEMORY;
    }
    
    dwLen = dwNetGateNum;
    s8 * pRecNetGate = NULL;
    u32 * pdwTmpNetGate = NULL;
    for ( dwIdx = 0, pRecNetGate = psNetGateSet, pdwTmpNetGate = pdwNetGate; dwIdx < dwBufLen; dwIdx++ )
    {
        if ( *(psNetGateSet+dwIdx) == '\0' )
        {
            *pdwTmpNetGate = ntohl(INET_ADDR(pRecNetGate));
            if ( *(psNetGateSet+dwIdx+1) == '\0' )
            {
                break;
            }
            pRecNetGate += dwIdx+1;
            pdwTmpNetGate++;
        }
    }
    
    delete [] psNetGateSet;
    return ERROR_SUCCESS;
}

/*=============================================================================
函 数 名： RegSetIpAdr
功    能： 
算法实现： 
全局变量： 
参    数： s8* lpszAdapterName —— 网卡名（不是描述）
           s8* pIpAddr —— IP地址组（可有多个IP，以'\0'分割）
           u32 dwLen —— IP地址组长度（包括最后的连续两个'\0'）
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/3/31   4.0		周广程                  创建
=============================================================================*/
u32 RegSetIpAdr(s8* lpszAdapterName, s8* pIpAddr, u32 dwLen)
{
    if( NULL == lpszAdapterName || NULL == pIpAddr || 0 == dwLen )
    {
        return ERROR_INVALID_PARAMETER;
    }

    HKEY hKey;
    s8 aszStrKeyName[1024] = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
    strcat(aszStrKeyName, lpszAdapterName);
    u32 dwRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, (const s8*)aszStrKeyName, 0, KEY_WRITE, &hKey);
    if( ERROR_SUCCESS != dwRet)
    {
        return dwRet;
    }

    dwRet = RegSetValueEx(hKey, "IPAddress", 0, REG_MULTI_SZ, (unsigned char*)pIpAddr, dwLen);
    RegCloseKey(hKey);
    if ( ERROR_SUCCESS != dwRet )
    {        
        return dwRet;
    }

    return ERROR_SUCCESS;
}

/*=============================================================================
函 数 名： RegSetIpAdr
功    能： 
算法实现： 
全局变量： 
参    数： s8* lpszAdapterName
           u32 * pdwIpAddr
           u32 dwLen
返 回 值： u32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/6/24   4.0			周广程                  创建
=============================================================================*/
u32 RegSetIpAdr(s8* lpszAdapterName, u32 * pdwIpAddr, u32 dwLen)
{
    if ( NULL == lpszAdapterName || NULL == pdwIpAddr || 0 == dwLen )
    {
        return ERROR_INVALID_PARAMETER;
    }
    
    // 一个IP字符串最大长度为15，加上一个分隔符'\0', 加上最后两个终止符'\0'
    s8 * pszIpAddr = new s8[16*dwLen+2];
    if ( NULL == pszIpAddr )
    {
        return ERROR_OUTOFMEMORY;
    }
    
    s8 aszTemp[17] = {0};
    s8 * pchSetPos = NULL;
    u32 dwIpStrLen = 0;
    u32 dwIdx = 0;
    for ( dwIdx = 0, pchSetPos = pszIpAddr; dwIdx < dwLen; dwIdx++ )
    {
        memset(aszTemp, 0, sizeof(aszTemp) );
        sprintf( aszTemp, "%d.%d.%d.%d", (pdwIpAddr[dwIdx]>>24)&0xff, (pdwIpAddr[dwIdx]>>16)&0xff, (pdwIpAddr[dwIdx]>>8)&0xff, pdwIpAddr[dwIdx]&0xff );
        strcpy( pchSetPos, aszTemp );
        *(pchSetPos + strlen(aszTemp)) = '\0';
        pchSetPos += strlen(aszTemp)+1;
        dwIpStrLen += strlen(aszTemp)+1;
    }
    *pchSetPos = '\0';
    dwIpStrLen += 1;

    u32 dwRet = RegSetIpAdr( lpszAdapterName, pszIpAddr, dwIpStrLen );
    
    delete [] pszIpAddr;
    return dwRet;
}

/*=============================================================================
函 数 名： RegSetNetMask
功    能： 
算法实现： 
全局变量： 
参    数： s8* lpszAdapterName —— 网卡名（不是描述）
           s8* pNetMask —— 掩码组（可有多个掩码，以'\0'分割）
           u32 dwLen —— 掩码组长度（包括最后的连续两个'\0'）
返 回 值： u32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/3/31   4.0		周广程                  创建
=============================================================================*/
u32 RegSetNetMask(s8* lpszAdapterName, s8* pNetMask, u32 dwLen)
{
    if( NULL == lpszAdapterName || NULL == pNetMask || 0 == dwLen )
    {
        return ERROR_INVALID_PARAMETER;
    }
    
    HKEY hKey;
    s8 aszStrKeyName[1024] = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
    strcat(aszStrKeyName, lpszAdapterName);
    u32 dwRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, (const s8*)aszStrKeyName, 0, KEY_WRITE, &hKey);
    if( ERROR_SUCCESS != dwRet)
    {
        return dwRet;
    }
    
    dwRet = RegSetValueEx(hKey, "SubnetMask", 0, REG_MULTI_SZ, (unsigned char*)pNetMask, dwLen);
    RegCloseKey(hKey);
    if ( ERROR_SUCCESS != dwRet )
    {        
        return dwRet;
    }
    
    return ERROR_SUCCESS;
}

/*=============================================================================
函 数 名： RegSetNetMask
功    能： 
算法实现： 
全局变量： 
参    数： s8* lpszAdapterName
           u32* pdwNetMask
           u32 dwLen
返 回 值： u32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/6/24   4.0			周广程                  创建
=============================================================================*/
u32 RegSetNetMask(s8* lpszAdapterName, u32* pdwNetMask, u32 dwLen)
{
    if ( NULL == lpszAdapterName || NULL == pdwNetMask || 0 == dwLen )
    {
        return ERROR_INVALID_PARAMETER;
    }
    
    // 一个IP字符串最大长度为15，加上一个分隔符'\0', 加上最后两个终止符'\0'
    s8 * pszMaskAddr = new s8[16*dwLen+2];
    if ( NULL == pszMaskAddr )
    {
        return ERROR_OUTOFMEMORY;
    }
    
    s8 aszTemp[17] = {0};
    s8 * pchSetPos = NULL;
    u32 dwMaskStrLen = 0;
    u32 dwIdx = 0;
    for ( dwIdx = 0, pchSetPos = pszMaskAddr; dwIdx < dwLen; dwIdx++ )
    {
        memset( aszTemp, 0, sizeof(aszTemp) );
        sprintf( aszTemp, "%d.%d.%d.%d", (pdwNetMask[dwIdx]>>24)&0xff, (pdwNetMask[dwIdx]>>16)&0xff, (pdwNetMask[dwIdx]>>8)&0xff, pdwNetMask[dwIdx]&0xff );
        strcpy( pchSetPos, aszTemp );
        *(pchSetPos + strlen(aszTemp)) = '\0';
        pchSetPos += strlen(aszTemp)+1;
        dwMaskStrLen += strlen(aszTemp)+1;
    }
    *pchSetPos = '\0';
    dwMaskStrLen += 1;
    
    u32 dwRet = RegSetIpAdr( lpszAdapterName, pszMaskAddr, dwMaskStrLen );
    
    delete [] pszMaskAddr;
    return dwRet;
}

/*=============================================================================
函 数 名： RegSetNetGate
功    能： 
算法实现： 
全局变量： 
参    数： const s8* lpszAdapterName
           s8* pNetGate
           u32 dwLen
返 回 值： u32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/6/24   4.0			周广程                  创建
=============================================================================*/
u32 RegSetNetGate(const s8* lpszAdapterName, s8* pNetGate, u32 dwLen)
{
    if( NULL == lpszAdapterName || NULL == pNetGate || 0 == dwLen )
    {
        return ERROR_INVALID_PARAMETER;
    }
    
    HKEY hKey;
    s8 aszStrKeyName[1024] = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
    strcat(aszStrKeyName, lpszAdapterName);
    u32 dwRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, (const s8*)aszStrKeyName, 0, KEY_WRITE, &hKey);
    if( ERROR_SUCCESS != dwRet)
    {
        return dwRet;
    }
    
    dwRet = RegSetValueEx(hKey, "DefaultGateway", 0, REG_MULTI_SZ, (unsigned char*)pNetGate, dwLen);
    RegCloseKey(hKey);
    if ( ERROR_SUCCESS != dwRet )
    {        
        return dwRet;
    }
    
    return ERROR_SUCCESS;
}

/*=============================================================================
函 数 名： RegSetNetGate
功    能： 
算法实现： 
全局变量： 
参    数： const s8* lpszAdapterName
           u32* pdwNetGate
           u32 dwLen
返 回 值： u32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/6/24   4.0			周广程                  创建
=============================================================================*/
u32 RegSetNetGate(const s8* lpszAdapterName, u32* pdwNetGate, u32 dwLen)
{
    if ( NULL == lpszAdapterName || NULL == pdwNetGate || 0 == dwLen )
    {
        return ERROR_INVALID_PARAMETER;
    }
    
    // 一个IP字符串最大长度为15，加上一个分隔符'\0', 加上最后两个终止符'\0'
    s8 * pszNetGateAddr = new s8[16*dwLen+2];
    if ( NULL == pszNetGateAddr )
    {
        return ERROR_OUTOFMEMORY;
    }
    
    s8 aszTemp[17] = {0};
    s8 * pchSetPos = NULL;
    u32 dwNetGateStrLen = 0;
    u32 dwIdx = 0;
    for ( dwIdx = 0, pchSetPos = pszNetGateAddr; dwIdx < dwLen; dwIdx++ )
    {
        memset( aszTemp, 0, sizeof(aszTemp) );
        sprintf( aszTemp, "%d.%d.%d.%d", (pdwNetGate[dwIdx]>>24)&0xff, (pdwNetGate[dwIdx]>>16)&0xff, (pdwNetGate[dwIdx]>>8)&0xff, pdwNetGate[dwIdx]&0xff );
        strcpy( pchSetPos, aszTemp );
        *(pchSetPos + strlen(aszTemp)) = '\0';
        pchSetPos += strlen(aszTemp)+1;
        dwNetGateStrLen += strlen(aszTemp)+1;
    }
    *pchSetPos = '\0';
    dwNetGateStrLen += 1;
    
    u32 dwRet = RegSetNetGate( lpszAdapterName, pszNetGateAddr, dwNetGateStrLen );
    
    delete [] pszNetGateAddr;
    return dwRet;
}

/*=============================================================================
函 数 名： RegGetRouteNum
功    能： 
算法实现： 
全局变量： 
参    数： void
返 回 值： u32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/4/9   4.0		周广程                  创建
=============================================================================*/
u32 RegGetRouteNum(void)
{
    HKEY hKey;
    s8 aszStrKeyName[1024] = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\PersistentRoutes";
    if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, (const s8*)aszStrKeyName, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
    {
        return FALSE;
    }
    
    s8 achRouteBuf[256];
    memset(achRouteBuf, '\0', sizeof(achRouteBuf));
    u32 dwRouteBufLen = sizeof(achRouteBuf);
    
    u32 dwIndex = 0;
    u32 dwRet = 0;
    u32 dwType = REG_SZ;
    while( ERROR_NO_MORE_ITEMS != RegEnumValue( hKey, dwIndex, achRouteBuf, &dwRouteBufLen, NULL, &dwType,NULL,NULL ) )
    {       
        memset(achRouteBuf, '\0', sizeof(achRouteBuf));
        dwIndex++;
    }
    
    RegCloseKey(hKey);
    
    return dwIndex;
}

/*=============================================================================
函 数 名： RegGetRoute
功    能： 
算法实现： 
全局变量： 
参    数： 
           TRoute * ptRoute
           u16 wLen
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/4/9   4.0		周广程                  创建
=============================================================================*/
BOOL32 RegGetRoute(TRoute * ptRoute, u32 dwLen)
{
    if(NULL == ptRoute || 0 == dwLen )
    {
        return FALSE;
    }
    
    HKEY hKey;
    s8 aszStrKeyName[1024] = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\PersistentRoutes";
    if( ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, (const s8*)aszStrKeyName, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
    {
        return FALSE;
    }
    
    s8 achRouteBuf[256];
    memset(achRouteBuf, '\0', sizeof(achRouteBuf));
    u32 dwRouteBufLen = sizeof(achRouteBuf);
    
    u32 dwIndex = 0;
    s8  achSeps[] = ","; 
    s8  *pchToken = NULL;
    TRoute * ptTmp = NULL;
    while( ERROR_NO_MORE_ITEMS != RegEnumValue( hKey, dwIndex, achRouteBuf, &dwRouteBufLen, NULL, NULL,NULL,NULL ) &&
           dwIndex < dwLen )
    {
        ptTmp = ptRoute+dwIndex;
        // dst ip, dst mask
        TNetParam tDstParam;

        pchToken = strtok( achRouteBuf, achSeps );
        u32 dwDstIp = ntohl(INET_ADDR( pchToken ));

        pchToken = strtok( NULL, achSeps );
        u32 dwDstMask = ntohl(INET_ADDR( pchToken ) );

        tDstParam.SetNetParam( dwDstIp, dwDstMask );
        ptTmp->SetDstNet( tDstParam );
        
        // gateway
        pchToken = strtok( NULL, achSeps );
        ptTmp->SetGateway( ntohl( INET_ADDR(pchToken) ) );

        // metric
        pchToken = strtok( NULL, achSeps );
        ptTmp->SetMetric( atoi(pchToken) );
        
        memset(achRouteBuf, '\0', sizeof(achRouteBuf));
        dwRouteBufLen = sizeof(achRouteBuf);
        dwIndex++;
    }
    
    RegCloseKey(hKey);
    
    return TRUE;
}

/*=============================================================================
函 数 名： RegAddRoute
功    能： 
算法实现： 
全局变量： 
参    数： 
           TRoute * ptRoute
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/4/9    4.0		    周广程                  创建
=============================================================================*/
BOOL32 RegAddRoute(TRoute * ptRoute )
{
    if ( NULL == ptRoute )
    {
        return FALSE;
    }

    HKEY hKey;
    s8 aszStrKeyName[1024] = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\PersistentRoutes";
    if( ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, (const s8*)aszStrKeyName, 0, KEY_WRITE, &hKey) != ERROR_SUCCESS)
    {
        return FALSE;
    }

    s8 achRouteBuf[256];
    memset(achRouteBuf, '\0', sizeof(achRouteBuf));
    u32 dwRouteBufLen = 0;

    TNetParam tDstParam = ptRoute->GetDstNet();
    u32 dwDstIp = tDstParam.GetIpAddr();
    u32 dwDstMask = tDstParam.GetIpMask();
    u32 dwGateWay = ptRoute->GetGateway();
    u32 dwMetric = ptRoute->GetMetric();

    s8 achIp[18];    
    // dst ip
    memset( achIp, '\0', sizeof(achIp) );    
    sprintf( achIp, "%d.%d.%d.%d,", (dwDstIp>>24)&0xff, (dwDstIp>>16)&0xff, (dwDstIp>>8)&0xff, dwDstIp&0xff );
    strcat( achRouteBuf, achIp );
    dwRouteBufLen += strlen(achIp);

    // dst mask
    memset( achIp, '\0', sizeof(achIp) );
    sprintf( achIp, "%d.%d.%d.%d,", (dwDstMask>>24)&0xff, (dwDstMask>>16)&0xff, (dwDstMask>>8)&0xff, dwDstMask&0xff );
    strcat( achRouteBuf, achIp );
    dwRouteBufLen += strlen(achIp);

    // gateway
    memset( achIp, '\0', sizeof(achIp) );
    sprintf( achIp, "%d.%d.%d.%d,", (dwGateWay>>24)&0xff, (dwGateWay>>16)&0xff, (dwGateWay>>8)&0xff, dwGateWay&0xff );
    strcat( achRouteBuf, achIp );
    dwRouteBufLen += strlen(achIp);

    // metric
    memset( achIp, '\0', sizeof(achIp) );
    sprintf( achIp, "%d", dwMetric );
    strcat( achRouteBuf, achIp );
    dwRouteBufLen += strlen(achIp);

    *(achRouteBuf+dwRouteBufLen) = '\0';

    s8 achData[3] = {0};
    if ( ERROR_SUCCESS != RegSetValueEx(hKey, achRouteBuf, 0, REG_MULTI_SZ, (unsigned char*)achData, strlen(achData)+1) )
    {
        RegCloseKey(hKey);
        return FALSE;
    }

    RegCloseKey(hKey);

    return TRUE;
}

/*=============================================================================
函 数 名： RegDelRoute
功    能： 
算法实现： 
全局变量： 
参    数： TRoute * ptRoute
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/4/9   4.0		周广程                  创建
=============================================================================*/
BOOL32 RegDelRoute( TRoute * ptRoute )
{
    if ( NULL == ptRoute )
    {
        return FALSE;
    }
    
    HKEY hKey;
    s8 aszStrKeyName[1024] = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\PersistentRoutes";
    if( ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, (const s8*)aszStrKeyName, 0, KEY_WRITE, &hKey) != ERROR_SUCCESS)
    {
        return FALSE;
    }
    
    s8 achRouteBuf[256];
    memset(achRouteBuf, '\0', sizeof(achRouteBuf));
    u32 dwRouteBufLen = 0;
    
    TNetParam tDstParam = ptRoute->GetDstNet();
    u32 dwDstIp = tDstParam.GetIpAddr();
    u32 dwDstMask = tDstParam.GetIpMask();
    u32 dwGateWay = ptRoute->GetGateway();
    u32 dwMetric = ptRoute->GetMetric();
    
    s8 achIp[18];    
    // dst ip
    memset( achIp, '\0', sizeof(achIp) );    
    sprintf( achIp, "%d.%d.%d.%d,", (dwDstIp>>24)&0xff, (dwDstIp>>16)&0xff, (dwDstIp>>8)&0xff, dwDstIp&0xff );
    strcat( achRouteBuf, achIp );
    dwRouteBufLen += strlen(achIp);
    
    // dst mask
    memset( achIp, '\0', sizeof(achIp) );
    sprintf( achIp, "%d.%d.%d.%d,", (dwDstMask>>24)&0xff, (dwDstMask>>16)&0xff, (dwDstMask>>8)&0xff, dwDstMask&0xff );
    strcat( achRouteBuf, achIp );
    dwRouteBufLen += strlen(achIp);
    
    // gateway
    memset( achIp, '\0', sizeof(achIp) );
    sprintf( achIp, "%d.%d.%d.%d,", (dwGateWay>>24)&0xff, (dwGateWay>>16)&0xff, (dwGateWay>>8)&0xff, dwGateWay&0xff );
    strcat( achRouteBuf, achIp );
    dwRouteBufLen += strlen(achIp);
    
    // metric
    memset( achIp, '\0', sizeof(achIp) );
    sprintf( achIp, "%d", dwMetric );
    strcat( achRouteBuf, achIp );
    dwRouteBufLen += strlen(achIp);
    
    *(achRouteBuf+dwRouteBufLen) = '\0';
    
    if ( ERROR_SUCCESS != RegDeleteValue(hKey, achRouteBuf) )
    {
        RegCloseKey(hKey);
        return FALSE;
    }
    
    RegCloseKey(hKey);

    return TRUE;
}

/*=============================================================================
函 数 名： RegDelRouteAll
功    能： 
算法实现： 
全局变量： 
参    数： void
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/4/9   4.0		周广程                  创建
=============================================================================*/
BOOL32 RegDelRouteAll(void)
{
    u32 dwRouteNum = RegGetRouteNum();
    TRoute * ptRouteArray = NULL;
    if ( 0 == dwRouteNum )
    {
        return TRUE;
    }

    ptRouteArray = new TRoute[dwRouteNum];
    if ( NULL == ptRouteArray )
    {
        return FALSE;
    }
        
    if ( !RegGetRoute( ptRouteArray, dwRouteNum ) )
    {
        return FALSE;
    }

    BOOL32 bDelAll = TRUE;
    TRoute * ptTmp = ptRouteArray;
    for ( u32 dwIdx = 0; dwIdx < dwRouteNum; dwIdx++ )
    {
        bDelAll &= RegDelRoute( ptTmp );
        ptTmp++;
    }

    return bDelAll;
}

/*=============================================================================
函 数 名： GetLocalIp
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： u32 [主机序]
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/11/2   4.0		周广程                  创建
=============================================================================*/
// u32 GetLocalIp()
// {
// 	s8 achHostName[64];
// 	memset(achHostName, 0, sizeof(achHostName));
// 
// 	// 取得本机名 
// 	if(SOCKET_ERROR == gethostname(achHostName, 64))
// 	{
// 		return 0;
// 	}
// 	// 通过主机名得到地址表
// 	struct hostent *pHost = gethostbyname(achHostName);
// 	if ( NULL == pHost )
// 	{
// 		return 0;
// 	}
// 	// 默认使用第一个IP地址 
//     u32 dwIp = 0;
// 	if ( NULL != pHost->h_addr_list[0] )
// 	{
//         dwIp += (u32)((u8)pHost->h_addr_list[0][0]);
//         dwIp = dwIp << 8;
//         dwIp += (u32)((u8)pHost->h_addr_list[0][1]);
//         dwIp = dwIp << 8;
//         dwIp += (u32)((u8)pHost->h_addr_list[0][2]);
//         dwIp = dwIp << 8;
//         dwIp += (u32)((u8)pHost->h_addr_list[0][3]);
// 
// 		return( ntohl(dwIp) );
// 	}
//     
//     return 0;
// }

/*=============================================================================
函 数 名： AddAdapterIp
功    能： 
算法实现： 
全局变量： 
参    数：  u32 dwIpAddr
           u32 dwIpMask
           s8 * pchAdapterName
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/3/31   4.0		周广程                  创建
=============================================================================*/
BOOL32 AddAdapterIp( u32 dwIpAddr, u32 dwIpMask, s8 * pchAdapterName )
{
    if ( 0 == dwIpAddr || 0 == dwIpMask || NULL == pchAdapterName )
    {
        OspPrintf( TRUE, FALSE, "[AddAdapterIp] param(Ip=%d, Mask=%d, AdapterName=0x%x) is error!\n", dwIpAddr, dwIpMask, pchAdapterName );
        return FALSE;
    }
    /*
    HMODULE hModule = LoadLibrary("Iphlpapi.dll");
    if(NULL == hModule)
    {
        OspPrintf( TRUE, FALSE, "[AddAdapterIp]Cannot find \"Iphlpapi.dll\"\n");
        FreeLibrary(hModule); 
        return FALSE;
    }
    
    ADDIPADDRESS AddIpAddress = (ADDIPADDRESS)GetProcAddress(hModule, "AddIPAddress");
    if(NULL == hModule)
    {
        OspPrintf( TRUE, FALSE, "[AddAdapterIp]Cannot find the function \"AddIPAddress\" in \"Iphlpapi.dll\"\n");
        FreeLibrary(hModule); 
        return FALSE;
    }

    ULONG NTEContext = 0;
    ULONG NTEInstance = 0;
    DWORD dwRet = AddIpAddress( htonl(dwIpAddr), htonl(dwIpMask), ptAdapt->GetAdaptIdx(), &NTEContext, &NTEInstance );
    if ( dwRet != ERROR_SUCCESS )
    {
        OspPrintf( TRUE, FALSE, "[AddAdapterIp]Add 0x%x:0x%x on netadapt<%s> failed!\n", dwIpAddr, dwIpMask, ptAdapt->GetDescription() );
        FreeLibrary(hModule); 
        return FALSE;
    }
    */

    u16 wIdx = 0;

    IP_ADDRESS_STRING IpAddr;
    sprintf( IpAddr.String, "%d.%d.%d.%d\0", (dwIpAddr>>24)&0xff, (dwIpAddr>>16)&0xff, (dwIpAddr>>8)&0xff, dwIpAddr&0xff );

    IP_MASK_STRING IpMask;
    sprintf( IpMask.String, "%d.%d.%d.%d\0", (dwIpMask>>24)&0xff, (dwIpMask>>16)&0xff, (dwIpMask>>8)&0xff, dwIpMask&0xff );

    s8 sRegIp[ETH_IP_MAXNUM*16+1] = {0};
    u32 dwBufLen = sizeof(sRegIp);
    RegGetIpAdr( pchAdapterName, sRegIp, dwBufLen );
    s8 * pRegIp = (s8*)sRegIp;
    for ( wIdx = 0; wIdx < dwBufLen-1; wIdx++ )
    {
        if ( sRegIp[wIdx] == '\0' )
        {
            if ( wIdx < 7 ) // 最小的x.x.x.x形式的字符串长度
            {
                wIdx = 0;
                break;
            }
            else if ( strcmp(pRegIp, (s8*)IpAddr.String) == 0 )
            {
                // 不允许配置同样的IP
                return FALSE;
            }
        }
        if ( sRegIp[wIdx] == '\0' && sRegIp[wIdx+1] == '\0' )
        {
            break;
        }
        pRegIp = &(sRegIp[wIdx+1]);
    }
    u16 wIpLen = wIdx;

    s8 sRegMask[ETH_IP_MAXNUM*16+1] = {0};
    dwBufLen = sizeof(sRegMask);
    RegGetNetMask( pchAdapterName, sRegMask, dwBufLen );
    for ( wIdx = 0; wIdx < dwBufLen-1; wIdx++ )
    {
        if ( sRegMask[wIdx] == '\0' && sRegMask[wIdx+1] == '\0' )
        {
            break;
        }
    }
    u16 wMaskLen = wIdx;

    
    
    wIpLen += 1;
    memcpy( sRegIp+wIpLen, IpAddr.String, strlen(IpAddr.String)+1 );
    wIpLen += strlen(IpAddr.String)+1;
    RegSetIpAdr( pchAdapterName, sRegIp, wIpLen+1 );

    wMaskLen += 1;
    memcpy( sRegMask+wMaskLen, IpMask.String, strlen(IpMask.String)+1 );
    wMaskLen += strlen(IpMask.String)+1;
    RegSetNetMask( pchAdapterName, sRegMask, wMaskLen+1 );

    //NotifyIPChange( ptAdapt->GetAdapterName(), 3, IpAddr.String, IpMask.String );
	
    //FreeLibrary(hModule); 
    return TRUE;
}

/*=============================================================================
函 数 名： SetAdapterIp
功    能： 
算法实现： 
全局变量： 
参    数：  TNetParam * ptNetParam
           u16 wParamNum
           s8 * pchAdapterName
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/3/31   4.0		周广程                  创建
=============================================================================*/
BOOL32 SetAdapterIp( TNetParam * ptNetParam, u16 wParamNum, s8 * pchAdapterName )
{
    if ( NULL == ptNetParam || 0 == wParamNum || NULL == pchAdapterName || wParamNum > ETH_IP_MAXNUM )
    {
        OspPrintf( TRUE, FALSE, "[SetAdapterIp] param(ptNetParam=0x%x, wParamNum=%d, AdapterName=0x%x) is error!\n", ptNetParam, wParamNum, pchAdapterName );
        return FALSE;
    }

    u16 wIdx = 0;
    
    s8 sRegIp[ETH_IP_MAXNUM*16+1] = {0};
    s8 sRegMask[ETH_IP_MAXNUM*16+1] = {0};
    IP_ADDRESS_STRING IpAddr;
    IP_MASK_STRING IpMask;
    u32 dwIpAddr = 0;
    u32 dwIpMask = 0;

    s8 * pchRegIp = sRegIp;
    s8 * pchRegMask = sRegMask;
    u16 wRegIpLen = 0;
    u16 wRegMaskLen = 0;
    for ( wIdx = 0; wIdx < wParamNum; wIdx++ )
    {
        TNetParam tParam = *(ptNetParam+wIdx);
        dwIpAddr = tParam.GetIpAddr();
        dwIpMask = tParam.GetIpMask();
        sprintf( IpAddr.String, "%d.%d.%d.%d\0", (dwIpAddr>>24)&0xff, (dwIpAddr>>16)&0xff, (dwIpAddr>>8)&0xff, dwIpAddr&0xff );
        sprintf( IpMask.String, "%d.%d.%d.%d\0", (dwIpMask>>24)&0xff, (dwIpMask>>16)&0xff, (dwIpMask>>8)&0xff, dwIpMask&0xff );

        memcpy( pchRegIp, IpAddr.String, strlen(IpAddr.String)+1 );
        pchRegIp += strlen(IpAddr.String)+1;
        wRegIpLen += strlen(IpAddr.String)+1;

        memcpy( pchRegMask, IpMask.String, strlen(IpMask.String)+1 );
        pchRegMask += strlen(IpMask.String)+1;
        wRegMaskLen += strlen(IpMask.String)+1;
    }
    
    RegSetIpAdr( pchAdapterName, sRegIp, wRegIpLen+1 );
    RegSetNetMask( pchAdapterName, sRegMask, wRegMaskLen+1 );
    
    //NotifyIPChange( ptAdapt->GetAdapterName(), 3, IpAddr.String, IpMask.String );
    
    //FreeLibrary(hModule); 
    return TRUE;
}

/*=============================================================================
函 数 名： AddDefaultGW
功    能： 
算法实现： 
全局变量： 
参    数： u32 dwGWAddr
           s8 * pchAdapterName
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/3/31   4.0		周广程                  创建
=============================================================================*/
BOOL32 AddDefaultGW( u32 dwGWAddr, s8 * pchAdapterName )
{
    if ( 0 == dwGWAddr || NULL == pchAdapterName )
    {
        OspPrintf( TRUE, FALSE, "[AddDefaultGW] param(GW=%d, AdapterName=0x%x) is error!\n", dwGWAddr, pchAdapterName );
        return FALSE;
    }

    u16 wIdx = 0;

    s8 sRegGW[ETH_IP_MAXNUM*16+1] = {0};
    u32 dwBufLen = sizeof(sRegGW);
    RegGetNetGate( pchAdapterName, sRegGW, dwBufLen );
    for ( wIdx = 0; wIdx < sizeof(sRegGW)-1; wIdx++ )
    {
        if ( sRegGW[wIdx] == '\0' && sRegGW[wIdx+1] == '\0' )
        {
            break;
        }
    }
    u16 wGWLen = wIdx;

    IP_ADDRESS_STRING GWAddr;
    
    wGWLen += 1;
    sprintf( GWAddr.String, "%d.%d.%d.%d\0", (dwGWAddr>>24)&0xff, (dwGWAddr>>16)&0xff, (dwGWAddr>>8)&0xff, dwGWAddr&0xff );
    memcpy( sRegGW+wGWLen, GWAddr.String, strlen(GWAddr.String)+1 );
    wGWLen += strlen(GWAddr.String);
    wGWLen += 2;    // 最后两个连续的'\0'也要算上
    RegSetNetGate( pchAdapterName, sRegGW, wGWLen );

    return TRUE;
}

/*=============================================================================
函 数 名： DelDefaultGW
功    能： 
算法实现： 
全局变量： 
参    数： s8 * pchAdapterName
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/5/13   4.0		周广程                  创建
=============================================================================*/
BOOL32 DelDefaultGW( const s8 * pchAdapterName )
{
    if ( NULL == pchAdapterName )
    {
        OspPrintf( TRUE, FALSE, "[SetAdapterIp] param(AdapterName=0x%x) is error!\n", pchAdapterName );
        return FALSE;
    }
    
    u16 wIdx = 0;
    
    s8 sRegGW[ETH_IP_MAXNUM*16+1] = {0};
    u16 wRegGWLen = 0;
    
    RegSetNetGate( pchAdapterName, sRegGW, wRegGWLen );
    return TRUE;
}

/*=============================================================================
函 数 名： SetDefaultGW
功    能： 
算法实现： 
全局变量： 
参    数： u32 *pdwGWAddr
           u16 wGWNum
           s8 * pchAdapterName
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/3/31   4.0		    周广程                  创建
=============================================================================*/
BOOL32 SetDefaultGW( u32 *pdwGWAddr, u16 wGWNum, s8 * pchAdapterName )
{
    if ( NULL == pdwGWAddr || 0 == wGWNum || NULL == pchAdapterName || wGWNum > ETH_IP_MAXNUM )
    {
        OspPrintf( TRUE, FALSE, "[SetAdapterIp] param(pdwGWAddr=0x%x, wGWNum=%d, AdapterName=0x%x) is error!\n", pdwGWAddr, wGWNum, pchAdapterName );
        return FALSE;
    }
    
    u16 wIdx = 0;
    
    s8 sRegGW[ETH_IP_MAXNUM*16+1] = {0};
    IP_ADDRESS_STRING GWAddr;
    u32 dwGWAddr = 0;
    
    s8 * pchRegGW = sRegGW;
    u16 wRegGWLen = 0;
    for ( wIdx = 0; wIdx < wGWNum; wIdx++ )
    {
        dwGWAddr = *(pdwGWAddr+wIdx);
        sprintf( GWAddr.String, "%d.%d.%d.%d\0", (dwGWAddr>>24)&0xff, (dwGWAddr>>16)&0xff, (dwGWAddr>>8)&0xff, dwGWAddr&0xff );
             
        memcpy( pchRegGW, GWAddr.String, strlen(GWAddr.String)+1 );
        pchRegGW += strlen(GWAddr.String)+1;
        wRegGWLen += strlen(GWAddr.String)+1;
    }
    
    wRegGWLen += 1;     // 最后以连续两个'\0'结尾, 所以长度需要再加1
    RegSetNetGate( pchAdapterName, sRegGW, wRegGWLen );
    return TRUE;
}

/*=============================================================================
函 数 名： IsIpAddrIn
功    能： 
算法实现： 
全局变量： 
参    数：  u32 dwIpAddr
           s8 * pchAdapterName
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/3/31   4.0		周广程                  创建
=============================================================================*/
BOOL32 IsIpAddrIn( u32 dwIpAddr, s8 * pchAdapterName )
{
    if ( 0 == dwIpAddr || NULL == pchAdapterName )
    {
        OspPrintf( TRUE, FALSE, "[IsIpAddrIn] param(Ip=%d, AdapterName=0x%x) is error!\n", dwIpAddr, pchAdapterName );
        return FALSE;
    }

    u16 wIdx = 0;
    
    s8 sRegIp[ETH_IP_MAXNUM*16+1] = {0};
    u32 dwBufLen = sizeof(sRegIp);
    RegGetIpAdr( pchAdapterName, sRegIp, dwBufLen );
    s8 * pchIpTmp = sRegIp;
    u32 dwRegIp = 0;
    for ( wIdx = 0; wIdx < sizeof(sRegIp)-1; wIdx++ )
    {
        if ( sRegIp[wIdx] == '\0' && wIdx == 0 )
        {
            break;
        }
        if ( sRegIp[wIdx] == '\0' )
        {
            dwRegIp = ntohl( INET_ADDR(pchIpTmp) );
            if ( dwRegIp == dwIpAddr )
            {
                return TRUE;
            }
            pchIpTmp = &(sRegIp[wIdx+1]);
        }
        if ( sRegIp[wIdx] == '\0' && sRegIp[wIdx+1] == '\0' )
        {
            break;
        }
    }
    return FALSE;    
}

/*=============================================================================
函 数 名： DelAdapterIp
功    能： 
算法实现： 
全局变量： 
参    数：  u32 dwIpAddr
           s8 * pchAdapterName
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/3/31   4.0		周广程                  创建
=============================================================================*/
BOOL32 DelAdapterIp( u32 dwIpAddr, s8 * pchAdapterName )
{
    if ( 0 == dwIpAddr || NULL == pchAdapterName )
    {
        OspPrintf( TRUE, FALSE, "[DelAdapterIp] param(Ip=%d, AdapterName=0x%x) is error!\n", dwIpAddr, pchAdapterName );
        return FALSE;
    }
    
    u16 wIdx = 0;
    
    s8 sRegIp[ETH_IP_MAXNUM*16+1] = {0};
    s8 sTmpIp[ETH_IP_MAXNUM*16+1] = {0};
    u32 dwBufLen = sizeof(sRegIp);
    RegGetIpAdr( pchAdapterName, sRegIp, dwBufLen ); 
    BOOL32 bFind = FALSE;
    s8 * pchRegIp = sRegIp;
    s8 * pchTmpIp = sTmpIp;
    u32 dwRegIp = 0;
    u16 wTmpIpLen = 0;
    for ( wIdx = 0; wIdx < sizeof(sRegIp)-1; wIdx++ )
    {
        if ( sRegIp[wIdx] == '\0' && wIdx == 0 )
        {
            break;
        }
        if ( sRegIp[wIdx] == '\0' )
        {
            dwRegIp = ntohl( INET_ADDR(pchRegIp) );            
            if ( dwRegIp == dwIpAddr )
            {
                bFind = TRUE;
            }
            else
            {
                strncpy( pchTmpIp, pchRegIp, strlen(pchRegIp) );
                *( pchTmpIp+strlen(pchRegIp) ) = '\0';
                pchTmpIp += strlen(pchRegIp)+1;
                wTmpIpLen += strlen(pchRegIp)+1;
            }  
            pchRegIp = &(sRegIp[wIdx+1]);
        }
        if ( sRegIp[wIdx] == '\0' && sRegIp[wIdx+1] == '\0' )
        {
            *pchTmpIp = '\0';
            wTmpIpLen -= 1;
            break;
        }
    }
    
    memcpy( sRegIp, sTmpIp, sizeof(sTmpIp) );
    
    RegSetIpAdr( pchAdapterName, sRegIp, wTmpIpLen );
    
    return bFind;  
}

/*=============================================================================
函 数 名： DelAdapterAllIp
功    能： 删除某网口的所有ip信息
算法实现： 
全局变量： 
参    数：  TNetAdaptInfo * ptNetAdapt 
           
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/4/17   4.6		    薛亮                  创建
=============================================================================*/
BOOL32 DelAdapterAllIp(TNetAdaptInfo * ptNetAdapt )
{
	BOOL32 bRet = TRUE;

	if( ptNetAdapt == NULL)
	{
		return FALSE;
	}

	s8 *pchAdapterName = ptNetAdapt->GetAdapterName();
	for(u8 byIpSecIdx = 0; byIpSecIdx < MCU_MAXNUM_ADAPTER_IP; byIpSecIdx ++)
	{
		TNetParam *ptNetParam = ptNetAdapt->m_tLinkIpAddr.GetValue(byIpSecIdx);
		if( ptNetParam != NULL)
		{
			u32 dwIpAddr = ptNetParam->GetIpAddr();
			if( 0 == dwIpAddr || NULL == pchAdapterName)
			{
				OspPrintf( TRUE, FALSE, "[DelAdapterIp] param(Ip=%d, AdapterName=0x%x) is error!\n", dwIpAddr, pchAdapterName );
				return FALSE;
			}
			bRet &= DelAdapterIp(dwIpAddr, pchAdapterName);
		}
	}

	return bRet;
}

/*==============================================================
函 数 名： NotifyIPChange
功    能： 
算法实现： 
全局变量： 
参    数： LPCTSTR lpszAdapterName
           s32 nIndex
           LPCTSTR pIPAddress
           LPCTSTR pNetMask
返 回 值： BOOL 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/3/31   4.0		周广程                  创建
=============================================================================*/
BOOL32 NotifyIPChange(LPCTSTR lpszAdapterName, s32 nIndex, LPCTSTR pIPAddress, LPCTSTR pNetMask)
{
    BOOL			bResult = FALSE;
    HINSTANCE		hDhcpDll;
    BDHCPNOTIFYPROC	pDhcpNotifyProc;
    WCHAR wcAdapterName[256];
    
    MultiByteToWideChar(CP_ACP, 0, lpszAdapterName, -1, wcAdapterName,256);
    
    if((hDhcpDll = LoadLibrary("dhcpcsvc")) == NULL)
    {
        return FALSE;
    }
    
    if((pDhcpNotifyProc = (BDHCPNOTIFYPROC)GetProcAddress(hDhcpDll, "DhcpNotifyConfigChange")) != NULL)
    {
        if((pDhcpNotifyProc)(NULL, wcAdapterName, TRUE, nIndex, inet_addr(pIPAddress), inet_addr(pNetMask), 0) == ERROR_SUCCESS)
        {
            bResult = TRUE;
        }
    }
    
    FreeLibrary(hDhcpDll);
    return bResult;
}

/*=============================================================================
函 数 名： SetAdapterRoute
功    能： 
算法实现： 
全局变量： 
参    数：  TRoute * ptRoute
           u32 wRouteNum
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/4/9   4.0		周广程                  创建
=============================================================================*/
BOOL32 SetAdapterRoute( TRoute * ptRoute, u32 dwRouteNum )
{
    if ( NULL == ptRoute && 0 != dwRouteNum )
    {
        return FALSE;
    }

    if ( !RegDelRouteAll() )
    {
        return FALSE;
    }     
    
    TRoute * ptTmp = ptRoute;
    BOOL32 bAddSuccess = TRUE;
    for ( u32 dwIdx = 0; dwIdx < dwRouteNum; dwIdx++ )
    {
        bAddSuccess &= RegAddRoute( ptTmp );
        ptTmp++;
    }

    return bAddSuccess;
}

/*=============================================================================
函 数 名： GetAdapterRoute
功    能： 
算法实现： 
全局变量： 
参    数：  TRoute * ptRoute
           u32 dwRouteNum
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/4/9   4.0		周广程                  创建
=============================================================================*/
BOOL32 GetAdapterRoute( TRoute * ptRoute, u32 dwRouteNum )
{
    if ( NULL == ptRoute || 0 == dwRouteNum )
    {
        return FALSE;
    }

    if ( !RegGetRoute( ptRoute, dwRouteNum ) )
    {
        return FALSE;
    }

    return TRUE;
}

/*=============================================================================
函 数 名： StateChange
功    能： 
算法实现： 
全局变量： 
参    数：  DWORD NewState
           DWORD SelectedItem
           HDEVINFO hDevInfo
返 回 值： BOOL 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/6/23   4.0			周广程                  创建
=============================================================================*/
BOOL StateChange( DWORD NewState, DWORD SelectedItem, HDEVINFO hDevInfo )
{
    SP_PROPCHANGE_PARAMS PropChangeParams = {sizeof(SP_CLASSINSTALL_HEADER)};   
    SP_DEVINFO_DATA DeviceInfoData = {sizeof(SP_DEVINFO_DATA)};   
    
    //   
    //   Get   a   handle   to   the   Selected   Item.   
    //   
    if ( !SetupDiEnumDeviceInfo(hDevInfo,SelectedItem,&DeviceInfoData) )   
    {   
        printf("SetupDiEnumDeviceInfo failed!\n");   
        return   FALSE;   
    }   
    
    //   
    //   Set   the   PropChangeParams   structure.   
    //   
    PropChangeParams.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;   
    PropChangeParams.Scope = DICS_FLAG_CONFIGGENERAL;   
    PropChangeParams.StateChange = NewState; 
    PropChangeParams.HwProfile = 0;
    
    if (!SetupDiSetClassInstallParams(hDevInfo,   
        &DeviceInfoData,   
        (SP_CLASSINSTALL_HEADER   *)&PropChangeParams,   
        sizeof(PropChangeParams)))   
    {   
        printf("SetupDiSetClassInstallParams failed!\n");   
        return FALSE;   
    }   
    
    //   
    //   Call   the   ClassInstaller   and   perform   the   change.   
    //   
    if ( !SetupDiCallClassInstaller( DIF_PROPERTYCHANGE,   
        hDevInfo,   
        &DeviceInfoData))   
    {   
        printf("SetupDiCallClassInstaller failed!\n");    
        return   TRUE;   
    }   
    
    return   TRUE;
}

/*=============================================================================
函 数 名： GetRegistryProperty
功    能： 
算法实现： 
全局变量： 
参    数：  HDEVINFO DeviceInfoSet
           PSP_DEVINFO_DATA DeviceInfoData
           ULONG Property
           PVOID Buffer
           PULONG Length
返 回 值： u32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/6/23   4.0			周广程                  创建
=============================================================================*/
u32 GetRegistryProperty( HDEVINFO hDevInfo, PSP_DEVINFO_DATA DeviceInfoData, ULONG Property, PVOID Buffer, PULONG Length )
{
    SetupDiGetDeviceRegistryProperty(hDevInfo, DeviceInfoData, 
        Property, NULL, (BYTE*)Buffer, *Length, Length);       
    
    u32 dwErrorCode = GetLastError();
    return dwErrorCode;   
}

/*=============================================================================
函 数 名： IsSpecNetCard
功    能： 
算法实现： 
全局变量： 
参    数： PSP_DEVINFO_DATA DeviceInfoData
返 回 值： BOOL 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/6/23   4.0			周广程                  创建
=============================================================================*/
BOOL IsSpecNetCard( HDEVINFO hDevInfo, PSP_DEVINFO_DATA DeviceInfoData, const s8 * pchAdapterName  )
{     
    HKEY   hKeyClass;   
    LONG   lRet;      
      
    //SYSTEM\CurrentControlSet\Control\Class目录下的一个子健
    if ( NULL == ( hKeyClass = SetupDiOpenClassRegKey(&DeviceInfoData->ClassGuid,KEY_READ) ) )
    {
        return FALSE;
    }
  
    // 获得设备类型
    s8 achClassType[150] = {0};
    u32 dwLength = sizeof(achClassType);
    u32  dwType = REG_SZ; 
    lRet = RegQueryValueEx(hKeyClass, TEXT("Class"), NULL, &dwType, (LPBYTE)(achClassType), &dwLength);   
    RegCloseKey(hKeyClass);
    if (lRet != ERROR_SUCCESS)   
    {
        return FALSE;   
    }
    // 不是Net类型，返回FALSE
    if ( 0 != strcmp( achClassType, "Net" ) )   
    {
        return FALSE;   
    }
    
    // 第一次调用，获得实际需要的缓存大小
    s8 * pchBuffer = NULL;       
    u32 dwBufSize = 0;    
    u32 dwErrorCode = GetRegistryProperty( hDevInfo, DeviceInfoData, SPDRP_DRIVER, pchBuffer, &dwBufSize );       
    if ( ERROR_INSUFFICIENT_BUFFER != dwErrorCode )
    {
        return FALSE;
    }
    // 根据返回参数，重新申请空间
    pchBuffer = new s8[dwBufSize];
    if ( NULL == pchBuffer )
    {
        printf( "malloc memory failed!\n" );
        return FALSE;
    }
    dwErrorCode = GetRegistryProperty( hDevInfo, DeviceInfoData, SPDRP_DRIVER, pchBuffer, &dwBufSize );
    if ( ERROR_SUCCESS != dwErrorCode )
    {
        printf( "GetRegistryProperty failed!\n" );
        return FALSE;
    }
    s8 * pchDriver = new char[dwBufSize+1];
    if ( NULL == pchDriver )
    {
        printf( "malloc driver memory failed!\n" );
        return FALSE;
    }
    memset( pchDriver, 0, dwBufSize+1 );
    char * pchTmp = pchDriver;
    for ( u32 dwChIdx = 0; dwChIdx < dwBufSize && *(pchBuffer+dwChIdx) != '\0'; dwChIdx++ )
    {
        *pchTmp = *(pchBuffer+dwChIdx);
        pchTmp++;
        if ( *(pchBuffer+dwChIdx) == '\\' )
        {
            *pchTmp = '\\';
            pchTmp++;
        }
    }
    delete [] pchBuffer;
    
    s8  szInterfaceKey[256] = {0};
    sprintf( szInterfaceKey, "SYSTEM\\CurrentControlSet\\Control\\Class\\%s\\Ndi\\Interfaces", pchDriver );
    s8  szDriverKey[256] = {0};
    sprintf( szDriverKey, "SYSTEM\\CurrentControlSet\\Control\\Class\\%s", pchDriver );
    delete [] pchDriver;
    
    HKEY hNdiIntKey;
    lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szInterfaceKey, 0, KEY_READ, &hNdiIntKey);
    if( ERROR_SUCCESS != lRet )
    {
        return FALSE;
    }

    s8 szData[256] = {0};
    dwBufSize = sizeof(szData);
    u32 dwDataType;
    lRet = RegQueryValueEx(hNdiIntKey, "LowerRange", 0, &dwDataType, (LPBYTE)szData, &dwBufSize);
    RegCloseKey(hNdiIntKey);
    if(ERROR_SUCCESS != lRet)
    { 
        return FALSE;
    }
    // 判断是不是以太网卡
    if(strcmp((char*)szData, "ethernet") != 0) 
    {          
        return FALSE;
    }   

    // 获得网卡名称
    HKEY hDriverKey;
    lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szDriverKey, 0, KEY_READ, &hDriverKey);
    if( ERROR_SUCCESS != lRet )
    {
        return FALSE;
    }
    memset( szData, 0, sizeof(szData) );
    dwBufSize = sizeof(szData);
    lRet = RegQueryValueEx(hDriverKey, "NetCfgInstanceId", 0, &dwDataType, (LPBYTE)szData, &dwBufSize);
    RegCloseKey(hDriverKey);
    if(ERROR_SUCCESS != lRet)
    { 
        return FALSE;
    }
    // 判断是否是指定网卡
    if ( strcmp( (s8*)szData, pchAdapterName ) != 0 )
    {
        return FALSE;
    }

    
    return TRUE;
}

/*=============================================================================
函 数 名： NetCardOper
功    能： 
算法实现： 
全局变量： 
参    数：  const s8 * pchDescription
           BOOL32 bStart
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/6/23   4.0			周广程                  创建
=============================================================================*/
BOOL32 NetCardOper( const s8 * pchAdapterName, BOOL32 bStart )
{       
    HDEVINFO hDevInfo = 0;

    hDevInfo = SetupDiGetClassDevs( NULL, NULL, NULL, DIGCF_PRESENT|DIGCF_ALLCLASSES );
    if ( INVALID_HANDLE_VALUE == hDevInfo )   
    {   
        printf( "SetupDiGetClassDevs failed!\n" );   
        return FALSE;   
    }   
    
    DWORD i;   
    SP_DEVINFO_DATA DeviceInfoData = {sizeof(SP_DEVINFO_DATA)};   
      
    for ( i = 0; SetupDiEnumDeviceInfo( hDevInfo, i, &DeviceInfoData ); i++)   
    {         
        if ( IsSpecNetCard( hDevInfo, &DeviceInfoData, pchAdapterName ) )   
        {          
            //DICS_DISABLE-->禁用网络   
            //DICS_ENABLE--->恢复网络 
            if ( !bStart )
            {
                if ( StateChange( DICS_DISABLE, i, hDevInfo ) )  
                {
                    printf("Netcard stopped\n");   
                }
            }
            else
            {
                if ( StateChange( DICS_ENABLE, i, hDevInfo ) )  
                {
                    printf("Netcard started\n");   
                }
            }
        }   
    }  
        
    return TRUE;   
}
#endif //WIN32


//                         (仅在LINUX平台上使用)
/*=============================================================================*/

#ifdef _LINUX_

s32 WWriteKeyValueString( FILE *stream, 
                s32 sdwOffsetCurLine, s32 sdwOffsetNextLine, 
                const s8* lpszKeyName, const s8* lpszKeyValue )
{
    u32  dwLen;
    s8*   lpszBuf = NULL ;

    ASSERT( lpszKeyName != NULL );

    /* copy the remaining content in buffer */
    fseek( stream, 0, SEEK_END );
    dwLen = ftell( stream ) - sdwOffsetNextLine;
    if( dwLen != 0 )
    {
        lpszBuf = (s8*)malloc( dwLen );
        ASSERT( lpszBuf != NULL );
        fseek( stream, sdwOffsetNextLine, SEEK_SET );
		if( lpszBuf != NULL )
		{
			dwLen = fread( lpszBuf, sizeof( char ), dwLen, stream );
		}
    }
    
    /* write the new value or delete it */
    fseek( stream, sdwOffsetCurLine, SEEK_SET );
    if( lpszKeyValue != NULL )
    {
        fputs( (const s8*)lpszKeyName, stream );
        fputs( (const s8*)"=", stream );
        fputs( (const s8*)lpszKeyValue, stream );
        fputs( (const s8*)STR_RETURN, stream );
    }

    /* write the remaining content in the buffer back */
    if( dwLen != 0 && lpszBuf != NULL )
    {
        fwrite( lpszBuf, sizeof( char ), dwLen, stream );
    }

    if( lpszBuf )
    {
        free( lpszBuf );
    }
    return( ftell( stream ) );
}
/*lint -save -e506*/
s32 WReadWriteKeyValueString( FILE *stream, const s8* lpszKeyName, 
                        s8* lpszKeyValue, u32 dwBufSize, BOOL32 bWrite )
{
    char    achRow[MAX_LINE_LEN + 1];
    char    achRowKeyName[MAX_KEYNAME_LEN + 1];
    char    achKeyNameCopy[MAX_KEYNAME_LEN + 1];
    s8*   lpszEqualPos;
    s8*   lpszReturn;
    u32  dwLen;
    s32  sdwOffsetCurLine;
    s32  sdwOffsetNextLine;

    ASSERT( lpszKeyName != NULL && strlen( lpszKeyName ) <= MAX_KEYNAME_LEN );
    ASSERT( !bWrite || ( strlen( lpszKeyValue ) <= MAX_VALUE_LEN ) );

    /* copy the Key name and change it */
    strncpy( achKeyNameCopy, lpszKeyName, MAX_KEYNAME_LEN );
    achKeyNameCopy[MAX_KEYNAME_LEN] = '\0';
    Trim( achKeyNameCopy );
    StrUpper( achKeyNameCopy );

    fseek( stream, 0, SEEK_SET );
    do
    {
        /* record the offset in every line */
        sdwOffsetCurLine = ftell( stream );

        if( fgets( achRow, MAX_LINE_LEN, stream ) == NULL )
        {                                           /* the end */
            if( bWrite && lpszKeyValue != NULL && feof( stream ) )       
                                                    /* create the Key */
            {
                sdwOffsetNextLine = sdwOffsetCurLine;
                return( WWriteKeyValueString( stream, sdwOffsetCurLine, 
                    sdwOffsetNextLine, lpszKeyName, lpszKeyValue ) );
            }
            else
            {
                return( -1 );
            }
        }

        /* eliminate the return key */
        if( ( lpszReturn = strstr( achRow, "\r\n" ) ) != NULL )
        {
            lpszReturn[0] = '\0';
        }
        else if( ( lpszReturn = strstr( achRow, "\n" ) ) != NULL )
        {
            lpszReturn[0] = '\0';
        }
        Trim( achRow );
        dwLen = strlen( achRow );
        
        if( dwLen == 0 )    continue;
        /* if arrive at a comment or cannot find a '=', continue */
        if( strstr( achRow, PROFILE_COMMENT1 ) == achRow ||
            strstr( achRow, PROFILE_COMMENT2 ) == achRow ||
            strstr( achRow, PROFILE_COMMENT3 ) == achRow ||
            strstr( achRow, PROFILE_COMMENT4 ) == achRow ||
            ( lpszEqualPos = strchr( achRow, '=' ) ) == NULL )
        {
            continue;
        }

        /* verify the key name */
        memcpy( achRowKeyName, achRow, lpszEqualPos - achRow );
        achRowKeyName[lpszEqualPos - achRow] = '\0';
        Trim( achRowKeyName );
        StrUpper( achRowKeyName );
        if( strcmp( achRowKeyName, achKeyNameCopy ) != 0 )  /* not identical */
        {
            continue;
        }

        /* return or set the key value string */
        if( bWrite )
        {
            sdwOffsetNextLine = ftell( stream );
            return( WWriteKeyValueString( stream, sdwOffsetCurLine, 
                sdwOffsetNextLine, lpszKeyName, lpszKeyValue ) );
        }
        else
        {
            Trim( lpszEqualPos + 1 );
            strncpy( lpszKeyValue, lpszEqualPos + 1, dwBufSize - 1 );
            lpszKeyValue[dwBufSize - 1] = '\0';
            return( ftell( stream ) );
        }

    } while( TRUE );
}
/*lint -restore*/

BOOL32 WGetRegKeyString2( FILE *stream,
                      const s8* lpszKeyName,  
                      s8*  lpszReturnValue, 
                      u32 dwBufSize )
{
    ASSERT( lpszKeyName != NULL && lpszReturnValue != NULL );

    ASSERT( stream != NULL);

    /* continue to find the right key */
    if( WReadWriteKeyValueString( stream, lpszKeyName, lpszReturnValue, dwBufSize, FALSE ) == -1 )
    {
        return( FALSE );
    }
    
    return( TRUE );
} 
    
BOOL32 WGetRegKeyString( const s8* lpszProfileName,
                      const s8* lpszKeyName, 
                      s8*  lpszReturnValue, 
                      u32 dwBufSize )
{
    FILE    *stream;
    BOOL32    bResult;

    stream = fopen( lpszProfileName, "rb" );
    if( stream == NULL )
    {
        /*printf( "Open file %s failed!\n", lpszProfileName );*/
        return( FALSE );
    }

    bResult = WGetRegKeyString2( stream, lpszKeyName, lpszReturnValue, dwBufSize );
    
    fclose( stream );

    return( bResult );
}  

BOOL32 WSetRegKeyString( const s8* lpszProfileName,  const s8* lpszKeyName, const s8* lpszValue)
{
    s32  sdwOffset;
    FILE    *stream;
    s8*    lpszBuf = NULL;

    stream = fopen( lpszProfileName, "rb+" );
    if( stream == NULL )
    {
        /*printf( "Open file %s failed!\n", lpszProfileName );*/
        return( FALSE );
    }
    /* continue to find the right key and write the value */
    if( ( sdwOffset = WReadWriteKeyValueString( stream, lpszKeyName, (s8*)lpszValue, 0, TRUE ) ) == -1 )
    {
        fclose( stream );
        return( FALSE );
    }
    
    /* record the whole new file */
    lpszBuf = (s8*)malloc( sdwOffset );
    ASSERT( lpszBuf );
    fseek( stream, 0, SEEK_SET );
	if( lpszBuf != NULL )
	{
		sdwOffset = fread( lpszBuf, sizeof( char ), sdwOffset, stream );
	}

    fclose( stream );

    /* rewrite the file */
    stream = fopen( lpszProfileName, "wb" );
    ASSERT( stream != NULL );
	if( lpszBuf != NULL && stream != NULL )
	{
		fwrite( lpszBuf, sizeof( char ), sdwOffset, stream );
	}

	if(stream != NULL)
	{
		fclose( stream );
	}

    free( lpszBuf );
    return( TRUE );
}

BOOL32 WDeleteRegKey( const s8* lpszProfileName, const s8* lpszKeyName )
{
    BOOL32    bResult;

    bResult = WSetRegKeyString( lpszProfileName, lpszKeyName, NULL );
    
    return( bResult );
}

/*=============================================================================
函 数 名： GetDefaultGW
功    能： 获得默认网关
算法实现： 
全局变量： 
参    数： u32 &dwDefaultGW
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/4/2   4.0		周广程                  创建
=============================================================================*/
BOOL32 GetDefaultGW( u32 dwIfIndex, u32 &dwDefaultGW ) 
{
	if ( GetOsType() == OS_UBUNTU )
	{
	//  [pengguofeng 9/18/2013]取两个字符串之间的行内容：
	//  每个网卡的默认网关有且只有一个，因此在auto ethX和gateway之间取内容即可
		s8 achCmd[256] = {0};
		sprintf(achCmd, GET_ETH_INFO_CMD, dwIfIndex, GW_STR, IPCONF_FILE_UBUNTU);

		sprintf(achCmd, "%s |grep %s | grep -v ^# | awk '{print $2}'", achCmd, GW_STR);

		DBG_LOG("exec cmd is [%s]\n", achCmd);
		FILE *fp = popen(achCmd, "r");
		if ( !fp )
		{
			DBG_LOG("cmd result null\n");
			return FALSE;
		}

		s8 achGw[32] = {0};
		fgets(achGw, sizeof(achGw), fp); //只是取到第一行,且只有一行内容
		pclose(fp);
		fp = NULL;
		if ( strlen(achGw) > 0 && achGw[strlen(achGw)-1] == '\n')
		{
			achGw[strlen(achGw)-1] = 0; // 字符串从stdout取出来的，自带\n [pengguofeng 9/18/2013]
		}
		else if ( strlen(achGw) == 0 )
		{
			return FALSE;
		}

		DBG_LOG("get gw str.[%s]\n", achGw);
		dwDefaultGW = INET_ADDR(achGw);
		if ( dwDefaultGW == 0 )
		{
			return FALSE;
		}
		
		dwDefaultGW = ntohl(dwDefaultGW);
		return TRUE;
	}

    // 指定网卡
    s8 achIfName[64] = {0};
    sprintf( achIfName, "/etc/sysconfig/network-scripts/ifcfg-eth%d", dwIfIndex );
    achIfName[sizeof(achIfName)-1] = '\0';
    s8 asIpAddrStr[32] = {0};
    
    BOOL32 bRet = WGetRegKeyString( achIfName, GW_KEY, asIpAddrStr, sizeof(asIpAddrStr) );
    if ( !bRet )
    {
        dwDefaultGW = 0;
        return FALSE;
    }

    dwDefaultGW = ntohl(INET_ADDR(asIpAddrStr));
	return  TRUE;   
} 

/*=============================================================================
函 数 名： SetDefaultGW
功    能： 
算法实现： 
全局变量： 
参    数：  u32 dwIfIndex
           u32 dwDefaultGW
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/4/2   4.0		周广程                  创建
=============================================================================*/
BOOL32 SetDefaultGW( u32 dwIfIndex, u32 dwDefaultGW )
{
	//  [pengguofeng 9/22/2013]
	if ( GetOsType() == OS_UBUNTU )
	{
		//1 get default gw of ethX
		u32 dwGwIp = 0;
		GetDefaultGW(dwIfIndex, dwGwIp);

		s8 achCmd[256] = {0};
		if ( dwGwIp == 0 )
		{
			//add Gw to file
			sprintf(achCmd, ADD_ETH_INFO_CMD, dwIfIndex, GW_STR, strofip(dwDefaultGW), IPCONF_FILE_UBUNTU);
			DBG_LOG("exec add default gw cmd.[%s]\n", achCmd);
		}
		else
		{
			//modify Gw to file
			sprintf(achCmd, MOD_ETH_INFO_CMD, GW_STR,strofip(dwGwIp), GW_STR, strofip(dwDefaultGW), IPCONF_FILE_UBUNTU);
			DBG_LOG("exec modify default gw cmd.[%s]\n", achCmd);
		}

		s32 nRet = system(achCmd);

		if ( nRet == -1 )
		{
			return FALSE;
		}
		return TRUE;
	}
    // 指定网卡
    s8 achIfName[64] = {0};
    sprintf( achIfName, "/etc/sysconfig/network-scripts/ifcfg-eth%d", dwIfIndex );
    achIfName[sizeof(achIfName)-1] = '\0';
	s8 asIpAddrStr[32] = {0};
    if( 0 != dwDefaultGW )
    {
        sprintf(asIpAddrStr, "%d.%d.%d.%d%c", (dwDefaultGW>>24)&0xFF, (dwDefaultGW>>16)&0xFF, (dwDefaultGW>>8)&0xFF, dwDefaultGW&0xFF, 0);
    }
    else
    {
        memcpy(asIpAddrStr, "0", sizeof("0"));
    }
	WSetRegKeyString(achIfName,GW_KEY,asIpAddrStr);


	return   TRUE;   
}

/*=============================================================================
函 数 名： DelDefaultGW
功    能： 
算法实现： 
全局变量： 
参    数：  u32 dwIfIndex
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/4/3   4.0		周广程                  创建
=============================================================================*/
BOOL32 DelDefaultGW( u32 dwIfIndex )
{
	//  [pengguofeng 9/22/2013]
	if ( GetOsType() == OS_UBUNTU )
	{
		//1. get default gw of ethX
		u32 dwGwIp = 0;
		GetDefaultGW(dwIfIndex, dwGwIp);
		if ( dwGwIp != 0 )
		{
			s8 achCmd[256] = {0};
			sprintf(achCmd, DEL_ETH_INFO_CMD, GW_STR, strofip(dwGwIp), IPCONF_FILE_UBUNTU );

			DBG_LOG("exec cmd is [%s]\n", achCmd);

			s32 nRet = system(achCmd);
			if ( nRet == -1 )
			{
				return FALSE;
			}
		}

		return TRUE;
	}
    
    // 指定网卡
    s8 achIfName[64] = {0};
    sprintf( achIfName, "/etc/sysconfig/network-scripts/ifcfg-eth%d", dwIfIndex );
    achIfName[sizeof(achIfName)-1] = '\0';	
	WDeleteRegKey(achIfName,GW_KEY);
    
    return   TRUE;
}

/*=============================================================================
函 数 名： SetAdapterIp
功    能： 清空以前的所有IP，只保留本次IP
算法实现： 
全局变量： 
参    数： u32 dwIfIndex
           TNetParam tIp
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/4/2   4.0		    周广程                  创建
=============================================================================*/
BOOL32 SetAdapterIp( u32 dwIfIndex, TNetParam tIp )
{
	if ( dwIfIndex > MCU_MAXNUM_ADAPTER-1 || 
		tIp.GetIpMask() == 0 || 
		tIp.GetIpAddr() == 0 )
	{
		return FALSE;
	}

	// 指定网卡
	s8 achPathName[64] ={0};
    s8 achFileName[64] = {0};
	s8 asIpAddrStr[32] = {0};
	s8 asNetMaskStr[32] = {0};
	s8 asMacStr[32] = {0};
	s8 achFullPathName[256] = {0};

    struct dirent *ent = NULL;
    DIR *pDir = NULL;

    //s8 achdir[512];
    sprintf( achPathName, "/etc/sysconfig/network-scripts/");
	sprintf( achFileName, "ifcfg-eth%d", dwIfIndex );
    u8 byFileNameLen = strlen( achFileName );

    pDir=opendir(achPathName);
    if ( NULL == pDir )
    {
        return FALSE;
    }

    while (NULL != (ent=readdir(pDir)))
    {
        //d_type：4表示为目录，8表示为文件
        if (ent->d_type== 4)
        {
			continue;
        }

        u8 byLocalLen = strlen(ent->d_name);
        
        if ( byLocalLen < byFileNameLen )
        {
            continue;
        }

        if ( memcmp(achFileName, ent->d_name, byFileNameLen) != 0 )
        {
			// 非同一网卡文件，跳过
            continue;
        }
        else if ( byLocalLen > byFileNameLen )
        {
            s8 * pch = ent->d_name+byFileNameLen;
            if ( *pch != ':' )
            {
				// 非合法网卡配置文件，跳过
                continue;
            }
            // 从':'后面开始检查
            pch++;
            u8 byChIdx = 0;
            for ( byChIdx = 0; byChIdx < byLocalLen-byFileNameLen-1; byChIdx++ )
            {
                if ( *(pch+byChIdx) < '0' || *(pch+byChIdx) > '9' )
                {
                    // 文件名不合法，退出
                    break;
                }
            }   
            if ( byChIdx < byLocalLen-byFileNameLen-1 )
            {
                continue;
            }
        }
			
		// 全路径名
		memset( achFullPathName, 0, sizeof(achFullPathName) );
		sprintf( achFullPathName, "%s%s", achPathName, ent->d_name );

        memset( asIpAddrStr, 0, sizeof(asIpAddrStr) );
		WGetRegKeyString(achFullPathName,IP_KEY,asIpAddrStr,sizeof(asIpAddrStr));
        memset( asNetMaskStr, 0, sizeof(asNetMaskStr) );
		WGetRegKeyString(achFullPathName,NETMAS_KEY,asNetMaskStr,sizeof(asNetMaskStr));


		if ( strlen(asMacStr) == 0 )
		{           
			WGetRegKeyString(achFullPathName,HWADDR_KEY,asMacStr,sizeof(asMacStr));
		}

		// 删除文件
		remove( achFullPathName );

    } // while (NULL != (ent=readdir(pDir)))
    closedir(pDir);


    // 获得设备名
    s8 asDeviceName[64] = {0};
    strncpy( (s8*)asDeviceName, (s8*)(achFileName+6), sizeof(asDeviceName) );

    memset( achFullPathName, 0, sizeof(achFullPathName) );
    sprintf( achFullPathName, "%s%s", achPathName, achFileName );
    	
	//创建文件
	FILE* hWrite = fopen( achFullPathName, "wb+" ); // create configure file
	if( NULL == hWrite )
    {
		return FALSE;	
	}
	fclose(hWrite);

    u32 dwTmpIp = tIp.GetIpAddr();
	memset(asIpAddrStr,0,sizeof(asIpAddrStr));
	memset(asNetMaskStr,0,sizeof(asNetMaskStr));
    sprintf(asIpAddrStr, "%d.%d.%d.%d", (dwTmpIp>>24)&0xFF, (dwTmpIp>>16)&0xFF, (dwTmpIp>>8)&0xFF, dwTmpIp&0xFF);
	dwTmpIp = tIp.GetIpMask();
	sprintf(asNetMaskStr, "%d.%d.%d.%d", (dwTmpIp>>24)&0xFF, (dwTmpIp>>16)&0xFF, (dwTmpIp>>8)&0xFF, dwTmpIp&0xFF);
	
	u16 wRet = 0;
	wRet |= WSetRegKeyString(achFullPathName, DEVICE_KEY, asDeviceName);
    wRet |= WSetRegKeyString(achFullPathName, HWADDR_KEY, asMacStr);
	wRet |= WSetRegKeyString(achFullPathName, TYPE_KEY,   "Ethernet");
	wRet |= WSetRegKeyString(achFullPathName, ONBOOT_KEY, "yes");
	wRet |= WSetRegKeyString(achFullPathName, BOOTPR_KEY, "static");
	wRet |= WSetRegKeyString(achFullPathName, IP_KEY,     asIpAddrStr);
	wRet |= WSetRegKeyString(achFullPathName, NETMAS_KEY, asNetMaskStr);

    return ( wRet == 0 ) ? TRUE : FALSE;
}

/*=============================================================================
函 数 名： AddEthIp
功    能： 给网卡添加IP、MASK、Gateway信息，并写入配置文件。
			如果是只添加子网卡，那么dwGw一定不要填值
算法实现： 
全局变量： 
参    数：	u8 byEthId, 待设置的网卡ID
			u8 byEthSubId, 子网卡ID
			u32 dwIp,   主机序
			u32 dwMask, 主机序
返 回 值： BOOL32 成功 TRUE； 失败 FALSE
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
BOOL32 AddEthIp(u8 byEthId, u32 dwIp, u32 dwMask, u32 dwGw /*= 0*/ )
{
#ifdef _LINUX_
	TEthInfo tEthInfo[MAX_ETH_NUM];
	u8 byEthNum = 0;
	if ( !GetEthInfo(tEthInfo, byEthNum) )
	{
		DBG_LOG("get all eth failed\n");
		return FALSE;
	}
	u32 dwTmpIp = 0;
	u32 dwTmpMask = 0;
	BOOL32 bModify = FALSE;
	for (u8 byLoop = 0; byLoop <byEthNum; byLoop++)
	{
		if ( !tEthInfo[byLoop].IsValid() 
			|| tEthInfo[byLoop].GetEthId() != byEthId)
		{
			continue;
		}
		
		for ( u8 bySubIdx = 0; bySubIdx < tEthInfo[byLoop].GetIpNum(); bySubIdx++ )
		{
			dwTmpIp = tEthInfo[byLoop].GetSubIp(bySubIdx);
			dwTmpMask = tEthInfo[byLoop].GetSubMask(bySubIdx);
			if ( dwTmpIp == dwIp )
			{
				//modify
				tEthInfo[byLoop].ModSubIpMask(dwIp, dwMask);
				bModify = TRUE;
			}
		}
		
		if ( !bModify )
		{
			if ( !tEthInfo[byLoop].AddSubIpMask(dwIp, dwMask) )
			{
				DBG_LOG("[%s@%d]add ip%x mask%x failed\n", __func__, __LINE__, dwIp, dwMask);
				return FALSE;
			}
		}
		
		if ( dwGw != 0 ) //非0才允许设置
		{
			tEthInfo[byLoop].SetGateway(dwGw);
		}
		else
		{
			DBG_LOG("dwGw == 0\n");
		}
#ifdef _DEBUG
		tEthInfo[byLoop].Print();
#endif
	}
	
	return SetEthInfo(tEthInfo, byEthNum);
	
#endif

#ifdef WIN32
	return TRUE;
#endif

	return TRUE;
}
/*=============================================================================
函 数 名： AddAdapterIp
功    能： 
算法实现： 
全局变量： 
参    数：  u32 dwIfIndex
           TNetParam tIp
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/5/26   4.0			周广程                  创建
=============================================================================*/
BOOL32 AddAdapterIp( u32 dwIfIndex, TNetParam tIp )
{
	if ( dwIfIndex > MCU_MAXNUM_ADAPTER-1 || 
		tIp.GetIpMask() == 0 || 
		tIp.GetIpAddr() == 0 )
	{
		return FALSE;
	}
	
	// for ubuntu [pengguofeng 9/23/2013]
	if ( GetOsType() == OS_UBUNTU )
	{
		return AddEthIp((u8)dwIfIndex, tIp.GetIpAddr(), tIp.GetIpMask());
	}

	// 下面是非ubuntu的处理 [pengguofeng 9/23/2013]
	// 指定网卡
	s8 achPathName[64] ={0};
    s8 achFileName[64] = {0};
	s8 asIpAddrStr[32] = {0};
	s8 asNetMaskStr[32] = {0};
	s8 asMacStr[32] = {0};
	u32 dwOldIp = 0;
	u32 dwOldMask = 0;
	s8 achFullPathName[256] = {0};

    struct dirent *ent = NULL;
    DIR *pDir = NULL;

    //s8 achdir[512];
    sprintf( achPathName, "/etc/sysconfig/network-scripts/");
	sprintf( achFileName, "ifcfg-eth%d", dwIfIndex );
    u8 byFileNameLen = strlen( achFileName );


    pDir=opendir(achPathName);
    if ( NULL == pDir )
    {
        return FALSE;
    }

    s16 wMaxIpIdx = -1;
    u16 wIpIdx = 0;
    u16 wIpNum = 0;
    while (NULL != (ent=readdir(pDir)))
    {
        //d_type：4表示为目录，8表示为文件
        if (ent->d_type== 4)
        {
			continue;
        }

        u8 byLocalLen = strlen(ent->d_name);
        
        if ( byLocalLen < byFileNameLen )
        {
            continue;
        }

        if ( memcmp(achFileName, ent->d_name, byFileNameLen) != 0 )
        {
			// 非同一网卡文件，跳过
            continue;
        }
        else if ( byLocalLen > byFileNameLen )
        {
            s8 * pch = ent->d_name+byFileNameLen;
            if ( *pch != ':' )
            {
				// 非合法网卡配置文件，跳过
                continue;
            }
            // 从':'后面开始检查
            pch++;
            u8 byChIdx = 0;
            for ( byChIdx = 0; byChIdx < byLocalLen-byFileNameLen-1; byChIdx++ )
            {
                if ( *(pch+byChIdx) < '0' || *(pch+byChIdx) > '9' )
                {
                    // 文件名不合法，退出
                    break;
                }
            }   
            if ( byChIdx < byLocalLen-byFileNameLen-1 )
            {
                continue;
            }
        }
			
		// 全路径名
		memset( achFullPathName, 0, sizeof(achFullPathName) );
		sprintf( achFullPathName, "%s%s", achPathName, ent->d_name );

        memset( asIpAddrStr, 0, sizeof(asIpAddrStr) );
		WGetRegKeyString(achFullPathName,IP_KEY,asIpAddrStr,sizeof(asIpAddrStr));
        memset( asNetMaskStr, 0, sizeof(asNetMaskStr) );
		WGetRegKeyString(achFullPathName,NETMAS_KEY,asNetMaskStr,sizeof(asNetMaskStr));


		dwOldIp = ntohl(INET_ADDR(asIpAddrStr));
		dwOldMask = ntohl(INET_ADDR(asNetMaskStr));
		if ((tIp.GetIpAddr() == dwOldIp) &&
			(tIp.GetIpMask() == dwOldMask) )
		{
            // 已存在，直接返回
			return FALSE;
		}

		// 符合ifcfg-ethx类型的文件，获取MAC地址
		if ( byLocalLen == byFileNameLen )
		{
			if ( strlen(asMacStr) == 0 )
			{           
				WGetRegKeyString(achFullPathName,HWADDR_KEY,asMacStr,sizeof(asMacStr));
			}
		}

        // 获得IP索引号        
        if ( byLocalLen == byFileNameLen )
        {
            wIpIdx = 0;
        }
        else
        {
            s8 * pch = strrchr( ent->d_name, ':' );
            if ( NULL != pch )
            {
                pch++;
                wIpIdx = atoi( pch );
            }
			else
			{
				continue;
			}
        }
        // 找到最大的IP索引号
        if ( wIpIdx > wMaxIpIdx )
        {
            wMaxIpIdx = wIpIdx;
        }

        wIpNum++;
    } // while (NULL != (ent=readdir(pDir)))
    closedir(pDir);

    // 有重复名称的配置文件
    if ( wIpNum > wMaxIpIdx+1 )
    {
        return FALSE;
    }

    // 获得新增加的IP索引号
    if ( wMaxIpIdx >= 0 )
    {
        if ( wIpNum == wMaxIpIdx+1 )
        {
            wIpIdx = wMaxIpIdx+1;
            memset( achFileName, 0, sizeof(achFileName) );
            sprintf( achFileName, "ifcfg-eth%d:%d", dwIfIndex, wIpIdx );
        }
        else
        {
            for ( u16 wIdx = 0; wIdx <= wMaxIpIdx; wIdx++ )
            {
                memset( achFileName, 0, sizeof(achFileName) );
                if ( wIdx == 0 )
                {
                    sprintf( achFileName, "ifcfg-eth%d", dwIfIndex );
                }
                else
                {
                    sprintf( achFileName, "ifcfg-eth%d:%d", dwIfIndex, wIdx );
                }
				FILE * fpTmp = fopen( (s8*)achFileName, "rb" );
                if ( NULL == fpTmp )
                {
                    break;
                }
				fclose( fpTmp );
            }
        }
    }
    else
    {
        memset( achFileName, 0, sizeof(achFileName) );
        sprintf( achFileName, "ifcfg-eth%d", dwIfIndex );
    }

    // 获得设备名: ethx或ethx:y
    s8 asDeviceName[64] = {0};
    strncpy( (s8*)asDeviceName, (s8*)(achFileName+6), sizeof(asDeviceName) );

    memset( achFullPathName, 0, sizeof(achFullPathName) );
    sprintf( achFullPathName, "%s%s", achPathName, achFileName );
    	
	//创建文件
	FILE* hWrite = fopen( achFullPathName, "wb+" ); // create configure file
	if( NULL == hWrite )
    {
		return FALSE;	
	}
	fclose(hWrite);

    u32 dwTmpIp = tIp.GetIpAddr();
	memset(asIpAddrStr,0,sizeof(asIpAddrStr));
	memset(asNetMaskStr,0,sizeof(asNetMaskStr));
    sprintf(asIpAddrStr, "%d.%d.%d.%d", (dwTmpIp>>24)&0xFF, (dwTmpIp>>16)&0xFF, (dwTmpIp>>8)&0xFF, dwTmpIp&0xFF);
	dwTmpIp = tIp.GetIpMask();
	sprintf(asNetMaskStr, "%d.%d.%d.%d", (dwTmpIp>>24)&0xFF, (dwTmpIp>>16)&0xFF, (dwTmpIp>>8)&0xFF, dwTmpIp&0xFF);
	
	u16 wRet = 0;
	wRet |= WSetRegKeyString(achFullPathName, DEVICE_KEY, asDeviceName);
	if ( strlen(asMacStr) > 0 )
    {
		wRet |= WSetRegKeyString(achFullPathName, HWADDR_KEY, asMacStr);
	}
	wRet |= WSetRegKeyString(achFullPathName, TYPE_KEY,   "Ethernet");
	wRet |= WSetRegKeyString(achFullPathName, ONBOOT_KEY, "yes");
	wRet |= WSetRegKeyString(achFullPathName, BOOTPR_KEY, "static");
	wRet |= WSetRegKeyString(achFullPathName, IP_KEY,     asIpAddrStr);
	wRet |= WSetRegKeyString(achFullPathName, NETMAS_KEY, asNetMaskStr);

	// [3/25/2010 xliang] 让新加的ip立即生效
// 	s8   chCmdline[256] =  {0}; 
// 	sprintf(chCmdline, "ifconfig %s %s netmask %s", asDeviceName, asIpAddrStr, asNetMaskStr);
// 	system(chCmdline);

    return ( wRet == 0 ) ? TRUE : FALSE;
}

/*=============================================================================
函 数 名： DelAdapterIp
功    能： 
算法实现： 
全局变量： 
参    数：  u32 dwIfIndex
           u32 dwIp
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/4/3    4.0		    周广程                  创建
=============================================================================*/
BOOL32 DelAdapterIp( u32 dwIfIndex, u32 dwIp )
{
    if ( dwIfIndex > MCU_MAXNUM_ADAPTER-1 || dwIp == 0 )
	{
		return FALSE;
	}
    printf( "[DelAdapterIp] Param ip = 0x%x\n", dwIp );
    
	// ubuntu处理 [pengguofeng 9/23/2013]
	if ( GetOsType() == OS_UBUNTU )
	{
		s8 achCmd[256] = {0};
		sprintf(achCmd, DEL_ETH_INFO_CMD, IP_STR, strofip(dwIp), IPCONF_FILE_UBUNTU);
		DBG_LOG("exec del cmd.[%s]\n", achCmd);

		s32 nRet = system(achCmd);
		if ( nRet == -1 )
		{
			return FALSE;
		}
		return TRUE;
	}

	// 指定网卡
	s8 achPathName[64] ={0};
    s8 achFileName[64] = {0};
	s8 asIpAddrStr[32] = {0};
	s8 asMacStr[32] = {0};
	u32 dwOldIp = 0;
	s8 achFullPathName[256] = {0};

    struct dirent *ent = NULL;
    DIR *pDir = NULL;

    //s8 achdir[512];
    sprintf( achPathName, "/etc/sysconfig/network-scripts/");
	sprintf( achFileName, "ifcfg-eth%d", dwIfIndex );
    u8 byFileNameLen = strlen( achFileName );

    printf( "[DelAdapterIp] eth file name is %s\n", achFileName );

	// 遍历
    pDir=opendir(achPathName);
    if ( NULL == pDir )
    {
        printf( "[DelAdapterIp] Open directory %s failed!\n", achPathName );
        return FALSE;
    }
    while ( NULL != (ent=readdir(pDir)) )
    {
        //d_type：4表示为目录，8表示为文件
        if (ent->d_type== 4)
        {
            continue;
        }

        u8 byLocalLen = strlen(ent->d_name);
        if ( byLocalLen < byFileNameLen )
        {
            continue;
        }
        
        if ( memcmp(achFileName, ent->d_name, byFileNameLen) != 0 )
        {
            continue;
        }
       
        BOOL32 bMainCfg = FALSE;
        if ( byLocalLen == byFileNameLen )
        {
            bMainCfg = TRUE;
        }

        // 校验是否合法文件
        if ( !bMainCfg )
        {
            s8 * pch = ent->d_name+byFileNameLen;
            if ( *pch != ':' )
            {
                // 文件名不合法
                continue;
            }
            // 从':'后面开始检查
            pch++;
            u8 byChIdx = 0;
            for ( byChIdx = 0; byChIdx < byLocalLen-byFileNameLen-1; byChIdx++ )
            {
                if ( *(pch+byChIdx) < '0' || *(pch+byChIdx) > '9' )
                {
                    // 文件名不合法
                    break;
                }
            }
            if ( byChIdx < byLocalLen-byFileNameLen-1 )
            {
                continue;
            }
        }

        printf("[DelAdapterIp] Get file.%s\n", ent->d_name );

		memset( achFullPathName, 0, sizeof(achFullPathName) );
		sprintf( achFullPathName, "%s%s", achPathName, ent->d_name );

        printf("[DelAdapterIp] Get file full name is %s\n", achFullPathName );

        // 获得MAC地址
        if ( bMainCfg )
        {           
            WGetRegKeyString(achFullPathName,HWADDR_KEY,asMacStr,sizeof(asMacStr));
        }

        memset( asIpAddrStr, 0, sizeof(asIpAddrStr) );
		WGetRegKeyString(achFullPathName,IP_KEY,asIpAddrStr,sizeof(asIpAddrStr));

        printf("[DelAdapterIp] Get ip.%s\n", asIpAddrStr );
		dwOldIp = ntohl(INET_ADDR(asIpAddrStr));
		if ( dwIp != dwOldIp )
		{  
            continue;
		}

        if ( bMainCfg )
        {           
/*            bMainDel = TRUE;*/
        }
        // 删除文件
        remove( achFullPathName ); 
        printf( "[DelAdapterIp] remove %s\n", achFullPathName );
        
    } // while (NULL != (ent=readdir(pDir)))
    closedir(pDir);

    // 如果主网卡文件被删除，则选择一个序号最大的网卡文件修改为主网卡文件
#if 0
    s32 nMaxIpIdx = -1;
    s32 nSelIdx = -1;
    if ( bMainDel )
    {
        pDir=opendir(achPathName);
        if ( NULL == pDir )
        {
            return FALSE;
        }
        while ( NULL != (ent=readdir(pDir)) )
        {
            //d_type：4表示为目录，8表示为文件
            if (ent->d_type== 4)
            {
                continue;
            }

            u8 byLocalLen = strlen(ent->d_name);
            if ( byLocalLen <= byFileNameLen )
            {
                continue;
            }
        
            if ( memcmp(achFileName, ent->d_name, byFileNameLen) != 0 )
            {
                continue;
            }

            // 校验是否合法文件
            s8 * pch = ent->d_name+byFileNameLen;
            if ( *pch != ':' )
            {
                // 文件名不合法
                continue;
            }
            // 从':'后面开始检查
            pch++;
            u8 byChIdx = 0;
            for ( byChIdx = 0; byChIdx < byLocalLen-byFileNameLen-1; byChIdx++ )
            {
                if ( *(pch+byChIdx) < '0' || *(pch+byChIdx) > '9' )
                {
                    // 文件名不合法
                    break;
                }
            }
            if ( byChIdx < byLocalLen-byFileNameLen-1 )
            {
                continue;
            }

            nSelIdx = atoi(pch);

            if ( nSelIdx > nMaxIpIdx )
            {
                nMaxIpIdx = nSelIdx;
            }                 
        } // while (NULL != (ent=readdir(pDir)))
        closedir(pDir);

        if ( nMaxIpIdx > -1 )
        {
            memset( achFullPathName, 0, sizeof(achFullPathName) );
		    sprintf( achFullPathName, "%s%s%d:%d", achPathName, "ifcfg-eth", dwIfIndex, nMaxIpIdx );
            strncpy( achSaveFullPathName, achFullPathName, sizeof(achSaveFullPathName) );
            s8 * pchTmp = strrchr( achSaveFullPathName, ':' );
            pchTmp = '\0';
            rename( (s8*)achFullPathName, (s8*)achSaveFullPathName );

            WSetRegKeyString(achSaveFullPathName, DEVICE_KEY, achFileName);
            WSetRegKeyString(achSaveFullPathName, HWADDR_KEY, asMacStr);
        }
        else
        {
            return ( FALSE );
        }
    }
#endif
    return TRUE;
}

/*=============================================================================
函 数 名： GetEthHWaddr
功    能： 获取网卡的MAC地址。注意：长度不包含\0
算法实现： 
全局变量： 
参    数：	u8 byEthId, 待设置的网卡ID
			s8 *pMacAddrStr,   字符串形式的MAC地址	
			u8 *pbyLen， 返回MAC地址的实际长度
返 回 值： BOOL32 成功 TRUE； 失败 FALSE
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
BOOL32 GetEthHWaddr(u8 byEthId, s8 *pMacAddrStr, u8 *pbyLen)
{
#ifdef WIN32
	return TRUE;
#endif

	if ( byEthId > MAX_ETH_NUM -1 )
	{
		return FALSE;
	}

	u8 byOsType = GetOsType();
	BOOL32 bRet = FALSE;
	s8 achCmd[256] = {0};//命令
	s8 achTemStr[128] = {0};//临时过渡
	s8 achMac[MAX_MACADDR_STR_LEN] = {0};
	switch ( byOsType)
	{
	case OS_UBUNTU:
		sprintf(achTemStr, HWADDR_STR, byEthId);
		sprintf(achCmd, GET_ETH_INFO_CMD, byEthId, achTemStr, IPCONF_FILE_UBUNTU);
		sprintf(achCmd, "%s | grep -v ^# |grep eth%d |grep hw | awk '{print $6}'", achCmd, byEthId);
		bRet = TRUE;
		break;
	case OS_REDHAT:
		sprintf(achTemStr, "%s%d", IPCONF_FILE_REDHAT, byEthId);
		sprintf(achCmd, "cat %s | grep -v ^# | grep %s | awk -F'=' '{print $2}'", achTemStr, MACADDR_KEY);
		bRet = TRUE;
		break;
	default:
		bRet = FALSE;
		break;
	}

	if ( bRet )
	{
		DBG_LOG("exec cmd[%s]\n", achCmd);
		FILE *fp = popen(achCmd, "r");
		if ( !fp)
		{
			bRet = FALSE;
		}
		else
		{
			fgets(achMac, sizeof(achMac), fp);
			pclose(fp);
			fp = NULL;
			bRet = TRUE;
			// trin \n [pengguofeng 9/27/2013]
			if ( strlen(achMac) > 0 && achMac[strlen(achMac) - 1] == '\n')
			{
				achMac[strlen(achMac) - 1] = 0;
			}
			else if ( strlen(achMac) == 0 )
			{
				return FALSE;// 长度为0 返回FALSE [pengguofeng 10/8/2013]
			}
			*pbyLen = strlen(achMac);

			memcpy(pMacAddrStr, achMac, *pbyLen);
		}
	}

	return bRet;
}

/*=============================================================================
函 数 名： SetEthHWaddr
功    能： 设置网卡的MAC地址，写入系统配置文件
算法实现： 
全局变量： 
参    数：	u8 byEthId, 待设置的网卡ID
			s8 *pMacAddrStr,   字符串形式的MAC地址		
返 回 值： BOOL32 成功 TRUE； 失败 FALSE
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
BOOL32 SetEthHWaddr(u8 byEthId, s8 *pMacAddrStr)
{
#ifdef WIN32
	return TRUE;
#endif

	if ( byEthId > MAX_ETH_NUM -1)
	{
		return FALSE;
	}
	// ubuntu处理 [pengguofeng 9/23/2013]
	u8 byOsType = GetOsType();
	if (byOsType == OS_UBUNTU )
	{
		s8 achCmd[256] = {0};
		s8 achTempStr[128] = {0};
		//1.find if exist
		sprintf(achCmd, ".*eth%d.*hw", byEthId);
		sprintf(achTempStr, GET_ETH_INFO_CMD, byEthId, achCmd, IPCONF_FILE_UBUNTU);
		memset(achCmd, 0, sizeof(achCmd));
		sprintf(achCmd, "%s | grep hw |grep eth%d | grep -v ^# | awk '{print $6}'", achTempStr, byEthId);
		DBG_LOG("exec cmd[%s]\n", achCmd);
		s8 achOldMac[64] = {0};
		FILE *fp = popen(achCmd, "r");
		if ( !fp )
		{
			//none
		}
		else
		{
			fgets(achOldMac, sizeof(achOldMac), fp);
			if ( strlen(achOldMac) > 0 && achOldMac[strlen(achOldMac)-1] == '\n')
			{
				achOldMac[strlen(achOldMac)-1] = 0;
			}
			pclose(fp);
			fp = NULL;
		}
		
		memset(achCmd, 0, sizeof(achCmd));
		memset(achTempStr, 0, sizeof(achTempStr));

		if ( strlen(achOldMac) == 0 )
		{
			//add
			sprintf(achTempStr, HWADDR_STR, byEthId);
			sprintf(achCmd, ADD_ETH_INFO_CMD, byEthId, achTempStr, pMacAddrStr, IPCONF_FILE_UBUNTU);
		}
		else
		{
			//modify
			//trim \n
			sprintf(achTempStr, HWADDR_STR, byEthId);
			sprintf(achCmd, MOD_ETH_INFO_CMD, achTempStr, achOldMac, achTempStr, pMacAddrStr, IPCONF_FILE_UBUNTU);
		}
		
		DBG_LOG("exec add hw cmd[%s]\n", achCmd );
		s32 nRet = system(achCmd);
		if ( nRet == -1 )
		{
			return FALSE;
		}
		return TRUE;
	}
	else if(byOsType == OS_REDHAT)
	{
		return SetHWaddrToAdapter(byEthId, pMacAddrStr);
	}
	else
	{
		return TRUE;//win32
	}
}

/*=============================================================================
函 数 名： DelEthHWaddr
功    能： 删除网卡的MAC地址，写入系统配置文件。注意：长度不包含\0
算法实现： 
全局变量： 
参    数：	u8 byEthId, 待设置的网卡ID
返 回 值： BOOL32 成功 TRUE； 失败 FALSE
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/27   4.7			彭国锋                  创建
=============================================================================*/
BOOL32 DelEthHWaddr(u8 byEthId)
{
#ifdef WIN32
	return TRUE;
#endif

	u8 byOsType = GetOsType();
	s8 achCmd[256] = {0};
	s8 achTemp[128] = {0};
	BOOL32 bRet = FALSE;
	s8 achMac[MAX_MACADDR_STR_LEN] = {0};
	u8 byLen = 0;
	bRet = GetEthHWaddr(byEthId, achMac, &byLen);
	if ( !bRet )
	{
		DBG_LOG("no Mac Addr found of eth.%d\n", byEthId);
		return FALSE;
	}
	switch ( byOsType )
	{
	case OS_UBUNTU:
		sprintf(achTemp, HWADDR_STR, byEthId);
		sprintf(achCmd, DEL_ETH_INFO_CMD, achTemp, achMac, IPCONF_FILE_UBUNTU);
		bRet = TRUE;
		break;
	case OS_REDHAT:
		sprintf(achCmd, "sed -i '/%s=%s/d' %s%d", HWADDR_KEY, achMac, IPCONF_FILE_REDHAT, byEthId);
		bRet = TRUE;
		break;
	default:
		bRet = FALSE;
		break;
	}

	if ( bRet )
	{
		DBG_LOG("exec cmd.[%s]\n", achCmd);
		if ( system(achCmd) == -1 )
		{
			bRet = FALSE;
		}
	}

	return bRet;
}
/*=============================================================================
函 数 名： SetHWaddrToAdapter
功    能： 
算法实现： 
全局变量： 
参    数：  u32 dwIfIndex
           
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
20100326    4.6		    薛亮                  创建
=============================================================================*/
BOOL32 SetHWaddrToAdapter( u32 dwIfIndex,  s8 *pHwAddrStr )
{
	if ( dwIfIndex > MCU_MAXNUM_ADAPTER-1 )
	{
		return FALSE;
	}
   
	// 指定网卡
	s8 achPathName[64] ={0};
    s8 achFileName[64] = {0};
	s8 asMacStr[32] = {0};
	s8 achFullPathName[256] = {0};

    struct dirent *ent = NULL;
    DIR *pDir = NULL;

    //s8 achdir[512];
    sprintf( achPathName, "/etc/sysconfig/network-scripts/");
	sprintf( achFileName, "ifcfg-eth%d", dwIfIndex );
    u8 byFileNameLen = strlen( achFileName );

    printf( "[SetHWaddrToAdapter] eth file name is %s\n", achFileName );

	// 遍历
    pDir=opendir(achPathName);
    if ( NULL == pDir )
    {
        printf( "[DelAdapterIp] Open directory %s failed!\n", achPathName );
        return FALSE;
    }
    while ( NULL != (ent=readdir(pDir)) )
    {
        //d_type：4表示为目录，8表示为文件
        if (ent->d_type== 4)
        {
            continue;
        }

        u8 byLocalLen = strlen(ent->d_name);
        if ( byLocalLen < byFileNameLen )
        {
            continue;
        }
        
        if ( memcmp(achFileName, ent->d_name, byFileNameLen) != 0 )
        {
            continue;
        }
       
        BOOL32 bMainCfg = FALSE;
        if ( byLocalLen == byFileNameLen )
        {
            bMainCfg = TRUE;
        }

        // 校验是否合法文件
        if ( !bMainCfg )
        {
            s8 * pch = ent->d_name+byFileNameLen;
            if ( *pch != ':' )
            {
                // 文件名不合法
                continue;
            }
            // 从':'后面开始检查
            pch++;
            u8 byChIdx = 0;
            for ( byChIdx = 0; byChIdx < byLocalLen-byFileNameLen-1; byChIdx++ )
            {
                if ( *(pch+byChIdx) < '0' || *(pch+byChIdx) > '9' )
                {
                    // 文件名不合法
                    break;
                }
            }
            if ( byChIdx < byLocalLen-byFileNameLen-1 )
            {
                continue;
            }
        }

		memset( achFullPathName, 0, sizeof(achFullPathName) );
		sprintf( achFullPathName, "%s%s", achPathName, ent->d_name );

        printf("[SetHWaddrToAdapter] Get file full name is %s\n", achFullPathName );

		WGetRegKeyString(achFullPathName, HWADDR_KEY, asMacStr, sizeof(asMacStr));
		
		if(strcmp(asMacStr, pHwAddrStr) != 0)
		{
			printf("[SetHWaddrToAdapter] Set HwAddr to file:%s\n", achFullPathName);
			WSetRegKeyString(achFullPathName, HWADDR_KEY, pHwAddrStr);
		}
		else
		{
			printf("[SetHWaddrToAdapter] HwAddr already exist, ignore!\n");
		}
        
    } // while (NULL != (ent=readdir(pDir)))
    closedir(pDir);

    return TRUE;
}

/*=============================================================================
函 数 名： DelAdapterAllIp
功    能： 
算法实现： 
全局变量： 
参    数：  u32 dwIfIndex
           
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
20100326    4.6		    薛亮                  创建
=============================================================================*/
BOOL32 DelAdapterAllIp( u32 dwIfIndex )
{
    if ( dwIfIndex > MCU_MAXNUM_ADAPTER-1 )
	{
		return FALSE;
	}
    
	// 处理ubuntu [pengguofeng 9/23/2013]
	if ( GetOsType() == OS_UBUNTU )
	{
		TEthInfo tEthInfo[MAX_ETH_NUM];
		u8 byEthNum = 0;
		if ( !GetEthInfo(tEthInfo, byEthNum) )
		{
			DBG_LOG("get all eth info failed\n");
			return FALSE;
		}

		if ( byEthNum == 0 )
		{
			DBG_LOG("get all eth info failed\n");
			return FALSE;
		}

		for ( u8 byLoop = 0; byLoop < byEthNum; byLoop++)
		{
			if ( tEthInfo[byLoop].IsValid()
				&& tEthInfo[byLoop].GetEthId() == (u8)dwIfIndex)
			{
				tEthInfo[byLoop].Clear();
			}
		}

		return SetEthInfo(tEthInfo, byEthNum);
	}

	// 指定网卡
	s8 achPathName[64] ={0};
    s8 achFileName[64] = {0};
	s8 achCmd[256] = {0};
/*
    struct dirent *ent = NULL;
    DIR *pDir = NULL;
*/
    //s8 achdir[512];
    sprintf( achPathName, "/etc/sysconfig/network-scripts/");
	sprintf( achFileName, "ifcfg-eth%d", dwIfIndex );
	sprintf(achCmd, "rm -f %s%s*", achPathName, achFileName);
	s32 nRet = system(achCmd);
	if ( nRet == -1 )
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
/*    u8 byFileNameLen = strlen( achFileName );

    printf( "[DelAdapterIp] eth file name is %s\n", achFileName );

	// 遍历
    pDir=opendir(achPathName);
    if ( NULL == pDir )
    {
        printf( "[DelAdapterIp] Open directory %s failed!\n", achPathName );
        return FALSE;
    }
    while ( NULL != (ent=readdir(pDir)) )
    {
        //d_type：4表示为目录，8表示为文件
        if (ent->d_type== 4)
        {
            continue;
        }

        u8 byLocalLen = strlen(ent->d_name);
        if ( byLocalLen < byFileNameLen )
        {
            continue;
        }
        
        if ( memcmp(achFileName, ent->d_name, byFileNameLen) != 0 )
        {
            continue;
        }
       
        BOOL32 bMainCfg = FALSE;
        if ( byLocalLen == byFileNameLen )
        {
            bMainCfg = TRUE;
        }

        // 校验是否合法文件
        if ( !bMainCfg )
        {
            s8 * pch = ent->d_name+byFileNameLen;
            if ( *pch != ':' )
            {
                // 文件名不合法
                continue;
            }
            // 从':'后面开始检查
            pch++;
            u8 byChIdx = 0;
            for ( byChIdx = 0; byChIdx < byLocalLen-byFileNameLen-1; byChIdx++ )
            {
                if ( *(pch+byChIdx) < '0' || *(pch+byChIdx) > '9' )
                {
                    // 文件名不合法
                    break;
                }
            }
            if ( byChIdx < byLocalLen-byFileNameLen-1 )
            {
                continue;
            }
        }

        printf("[DelAdapterIp] Get file.%s\n", ent->d_name );

		memset( achFullPathName, 0, sizeof(achFullPathName) );
		sprintf( achFullPathName, "%s%s", achPathName, ent->d_name );

        printf("[DelAdapterIp] Get file full name is %s\n", achFullPathName );

        
        if ( bMainCfg )
        {           
            //bMainDel = TRUE; //该标志目前无用
        }
        // 删除文件
        remove( achFullPathName ); 
        printf( "[DelAdapterIp] remove %s\n", achFullPathName );
        
    } // while (NULL != (ent=readdir(pDir)))
    closedir(pDir);

    return TRUE;
	*/
}

/*=============================================================================
函 数 名： NetCardOper
功    能： 
算法实现： 
全局变量： 
参    数：  u32 dwIfIndex
           BOOL32 bStart
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/6/16   4.0			周广程                  创建
=============================================================================*/
BOOL32 NetCardOper( u32 dwIfIndex, BOOL32 bStart )
{
    if ( dwIfIndex > MCU_MAXNUM_ADAPTER-1 )
	{
		return FALSE;
	}
    
	// 指定网卡
	s8 achPathName[64] ={0};
    s8 achFileName[64] = {0};
	s8 achFullPathName[256] = {0};

    struct dirent *ent = NULL;
    DIR *pDir = NULL;

    //s8 achdir[512];
    sprintf( achPathName, "/etc/sysconfig/network-scripts/");
	sprintf( achFileName, "ifcfg-eth%d", dwIfIndex );
    u8 byFileNameLen = strlen( achFileName );

    sprintf( achFullPathName, "%s%s", achPathName, achFileName );

    printf( "[NetCardOper] eth file name is %s\n", achFileName );

	// 遍历
    pDir=opendir(achPathName);
    if ( NULL == pDir )
    {
        printf( "[NetCardOper] Open directory %s failed!\n", achPathName );
        return FALSE;
    }

    while ( NULL != (ent=readdir(pDir)) )
    {
        //d_type：4表示为目录，8表示为文件
        if (ent->d_type== 4)
        {
            continue;
        }

        u8 byLocalLen = strlen(ent->d_name);
        if ( byLocalLen < byFileNameLen )
        {
            continue;
        }
        
        if ( memcmp(achFileName, ent->d_name, byFileNameLen) != 0 )
        {
            continue;
        }
       
        if ( byLocalLen > byFileNameLen )
        {
            continue;
        }
        
        printf( "[NetCardOper]Find file!\n" );

        BOOL32 bOperSuccess = FALSE;
        if ( bStart )
        {
            bOperSuccess = WSetRegKeyString(achFullPathName,ONBOOT_KEY,"yes");
        }
        else
        {
            bOperSuccess = WSetRegKeyString(achFullPathName,ONBOOT_KEY,"no");
        }

        printf( "[NetCardOper] OperSuccess = %d\n", bOperSuccess );

        return bOperSuccess;
    }

    return FALSE;
}

/*=============================================================================
函 数 名： IsValidEthIfFileName
功    能： 
算法实现： 
全局变量： 
参    数： const s8 * pchEthIfFileName
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/6/17   4.0			周广程                  创建
=============================================================================*/
BOOL32 IsValidEthIfFileName( const s8 * pchEthIfFileName )
{
    if ( NULL == pchEthIfFileName )
    {
        return FALSE;
    }

    s8 achFileName[64] = {0};
    u32 dwCharPos = 0;
    
    sprintf( achFileName, "ifcfg-eth" );

    u8 byFileNameLen = strlen( achFileName );
    u8 byEthIfFileNameLen = strlen( pchEthIfFileName );

    // 长度不足
    if ( byEthIfFileNameLen <= byFileNameLen )
    {
        return FALSE;
    }

    // 文件名前缀错误
    if ( 0 != strncmp( (s8*)achFileName, pchEthIfFileName, byFileNameLen ) )
    {
        return FALSE;
    }

    s8 achEthIfFileName[256] = {0};
    strncpy( achEthIfFileName, pchEthIfFileName, sizeof(achEthIfFileName) );

    s8 * pchPos = &achEthIfFileName[byFileNameLen];
    dwCharPos = byFileNameLen;
    BOOL32 bFirstIsZero = FALSE;    //文件名前缀后的第一个数字为0的标志
    BOOL32 bFirstIsZeroAfterColon = FALSE;  //第一个冒号后的第一个数字为0的标志
    u32 dwFirstColonPos = 0;    //第一个冒号的位置
    u32 dwColonNum = 0;         //冒号的数量
    while( *pchPos != '\0' && dwCharPos < byEthIfFileNameLen )
    {
        if ( *pchPos != ':' && ( *pchPos < '0' || *pchPos > '9' ) )
        {
            return FALSE;
        }

        if ( *pchPos == ':' )
        {
            dwColonNum++;
            if ( dwFirstColonPos == 0 )
            {
                // 记录第一个冒号的位置
                dwFirstColonPos = dwCharPos;
            }
            if ( dwCharPos == byFileNameLen )
            {
                // 冒号不能直接跟在文件名前缀后
                return FALSE;
            }
            if ( dwColonNum > 1 )
            {
                // 不能出现两个冒号
                return FALSE;
            }
            if ( dwCharPos == byEthIfFileNameLen-1 )
            {
                //冒号后不能为空
                return FALSE;
            }
        }

        if ( *pchPos >= '0' && *pchPos <= '9' )
        {
            if ( *pchPos == '0' && dwCharPos == byFileNameLen )
            {
                // 第一个数字为0
                bFirstIsZero = TRUE;
            }
            if ( *pchPos == '0' && dwFirstColonPos != 0 && dwCharPos == dwFirstColonPos+1 )
            {
                // 第一个冒号后的第一个数字为0
                bFirstIsZeroAfterColon = TRUE;
            }
            if ( bFirstIsZero &&
                 dwCharPos > byFileNameLen && 
                 dwFirstColonPos == 0 )
            {
                //网口号除0网口外不能以0开头
                return FALSE;
            }
            if ( bFirstIsZeroAfterColon &&
                 dwCharPos > dwFirstColonPos+1 )
            {
                //IP号除0外不能以0开头
                return FALSE;
            }
        }

        // 递增
        pchPos++;
        dwCharPos++;
    }
    
    return TRUE;
}

/*=============================================================================
函 数 名： GetEthIdxbyEthIfFileName
功    能： 
算法实现： 
全局变量： 
参    数： const s8 * pchEthIfFileName
返 回 值： s8  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/6/18   4.0			周广程                  创建
=============================================================================*/
s8  GetEthIdxbyEthIfFileName( const s8 * pchEthIfFileName )
{
    if ( !IsValidEthIfFileName( pchEthIfFileName ) )
    {
        return -1;
    }

    u8 byPreNameLen = strlen("ifcfg-eth");
    
    s8 achEthIfFileName[256] = {0};
    strncpy( achEthIfFileName, pchEthIfFileName, sizeof(achEthIfFileName) );
    u8 byEthIfFileNameLen = strlen(achEthIfFileName);
    
    s8 * pchPos = &achEthIfFileName[byPreNameLen];
    u32 dwCharPos = byPreNameLen;
    while( *pchPos != '\0' && dwCharPos < byEthIfFileNameLen )
    {       
        if ( *pchPos == ':' )
        {
            // 截断
            *pchPos = '\0';
            break;
        }        
        // 递增
        pchPos++;
        dwCharPos++;
    }

    return atoi( (s8*)&achEthIfFileName[byPreNameLen] );
}

/*=============================================================================
函 数 名： IsValidEthIfName
功    能： 
算法实现： 
全局变量： 
参    数： const s8 * pchEthIfName
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/6/25   4.0			周广程                  创建
=============================================================================*/
BOOL32 IsValidEthIfName( const s8 * pchEthIfName )
{
    if ( NULL == pchEthIfName )
    {
        return FALSE;
    }
    
    s8 achIfName[64] = {0};
    u32 dwCharPos = 0;
    
    sprintf( achIfName, "eth" );
    
    u8 byIfNameLen = strlen( achIfName );
    u8 byEthIfNameLen = strlen( pchEthIfName );
    
    // 长度不足
    if ( byEthIfNameLen <= byIfNameLen )
    {
        return FALSE;
    }
    
    // 文件名前缀错误
    if ( 0 != strncmp( (s8*)achIfName, pchEthIfName, byIfNameLen ) )
    {
        return FALSE;
    }
    
    s8 achEthIfName[256] = {0};
    strncpy( achEthIfName, pchEthIfName, sizeof(achEthIfName) );
    
    s8 * pchPos = &achEthIfName[byIfNameLen];
    dwCharPos = byIfNameLen;
    BOOL32 bFirstIsZero = FALSE;    //文件名前缀后的第一个数字为0的标志
    BOOL32 bFirstIsZeroAfterColon = FALSE;  //第一个冒号后的第一个数字为0的标志
    u32 dwFirstColonPos = 0;    //第一个冒号的位置
    u32 dwColonNum = 0;         //冒号的数量
    while( *pchPos != '\0' && dwCharPos < byEthIfNameLen )
    {
        if ( *pchPos != ':' && ( *pchPos < '0' || *pchPos > '9' ) )
        {
            return FALSE;
        }
        
        if ( *pchPos == ':' )
        {
            dwColonNum++;
            if ( dwFirstColonPos == 0 )
            {
                // 记录第一个冒号的位置
                dwFirstColonPos = dwCharPos;
            }
            if ( dwCharPos == byIfNameLen )
            {
                // 冒号不能直接跟在文件名前缀后
                return FALSE;
            }
            if ( dwColonNum > 1 )
            {
                // 不能出现两个冒号
                return FALSE;
            }
            if ( dwCharPos == byEthIfNameLen-1 )
            {
                //冒号后不能为空
                return FALSE;
            }
        }
        
        if ( *pchPos >= '0' && *pchPos <= '9' )
        {
            if ( *pchPos == '0' && dwCharPos == byIfNameLen )
            {
                // 第一个数字为0
                bFirstIsZero = TRUE;
            }
            if ( *pchPos == '0' && dwFirstColonPos != 0 && dwCharPos == dwFirstColonPos+1 )
            {
                // 第一个冒号后的第一个数字为0
                bFirstIsZeroAfterColon = TRUE;
            }
            if ( bFirstIsZero &&
                dwCharPos > byIfNameLen && 
                dwFirstColonPos == 0 )
            {
                //网口号除0网口外不能以0开头
                return FALSE;
            }
            if ( bFirstIsZeroAfterColon &&
                dwCharPos > dwFirstColonPos+1 )
            {
                //IP号除0外不能以0开头
                return FALSE;
            }
        }
        
        // 递增
        pchPos++;
        dwCharPos++;
    }
    
    return TRUE;
}

/*=============================================================================
函 数 名： GetEthIdxbyEthIfName
功    能： 
算法实现： 
全局变量： 
参    数： const s8 * pchEthIfName
返 回 值： s8  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/6/25   4.0			周广程                  创建
=============================================================================*/
s8  GetEthIdxbyEthIfName( const s8 * pchEthIfName )
{
    if ( !IsValidEthIfName( pchEthIfName ) )
    {
        return -1;
    }
    
    u8 byPreNameLen = strlen("eth");
    
    s8 achEthIfName[256] = {0};
    strncpy( achEthIfName, pchEthIfName, sizeof(achEthIfName) );
    u8 byEthIfNameLen = strlen(achEthIfName);
    
    s8 * pchPos = &achEthIfName[byPreNameLen];
    u32 dwCharPos = byPreNameLen;
    while( *pchPos != '\0' && dwCharPos < byEthIfNameLen )
    {       
        if ( *pchPos == ':' )
        {
            // 截断
            *pchPos = '\0';
            break;
        }        
        // 递增
        pchPos++;
        dwCharPos++;
    }
    
    return atoi( (s8*)&achEthIfName[byPreNameLen] );
}

/*=============================================================================
函 数 名： GetEthSubIdByEthName
功    能： 对形如 ethX 或 ethX:Y 的网口名，取得子网口号如，Y 或 INVALID_ETHID
算法实现： 
全局变量： 
参    数： const s8 * pchEthIfName
返 回 值： s8  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/25   4.7			彭国锋                  创建
=============================================================================*/
u8 GetEthSubIdByEthName(const s8 *pachEthName, u8 byEthMainId)
{
	if ( !pachEthName || byEthMainId > MAX_ETH_IPNUM - 1)
	{
		return INVALID_ETHID;
	}

	s8 achTemp[MAX_MACADDR_STR_LEN] = {0};
	sprintf(achTemp, "eth%d", byEthMainId);

	if ( strlen(pachEthName) == strlen(achTemp) )
	{
		DBG_LOG("only for ethX:Y, now is [%s]\n",achTemp);
		return INVALID_ETHID;
	}
	u8 byStartPos = strlen(achTemp) + strlen(":");
	
	return atoi(pachEthName+byStartPos);
}

s32 GetLinkStat(s8 *ifname)
{
	s32 nRet = 0;
	s32 skfd = -1;
	struct ifreq ifr;
	struct ethtool_value edata;
	
	if (( skfd = socket( AF_INET, (int)SOCK_DGRAM, 0 ) ) < 0 )
	{
		printf("[GetLinkStat]socket error\n");
		perror("GetLinkStat");
		nRet = -1;
	}
	
	if( nRet == 0 )
	{
		memset(&ifr, 0, sizeof(ifr));
		edata.cmd = ETHTOOL_GLINK;
		strncpy(ifr.ifr_name, ifname, 16-1);
		ifr.ifr_data = (char *) &edata;
		if (ioctl(skfd, SIOCETHTOOL, &ifr) == -1) 
		{
			perror("GETLinkStat");
			nRet = -1;
		}
		
		if( nRet == 0)
		{
			nRet = (edata.data)? 1 : 0;
		}

		if(close(skfd) == -1)
// 		if( shutdown(skfd, SHUT_RDWR) == -1 ) //用shutdown并没有释放资源
		{
			printf("[GetLinkStat] close skfd(%d) faild, errno is: %s\n", skfd, strerror(errno));
		}

	}

	return nRet;
}


s32 GetEthEnStat(s8 *ifname)
{
	int skfd = -1;
	struct ifreq ifr;
	if (( skfd = socket( AF_INET, (int)SOCK_DGRAM, 0 ) ) < 0 )
		return -1;

	memset(&ifr, 0, sizeof(struct ifreq));
	strcpy(ifr.ifr_name, ifname);
	if (ioctl(skfd, SIOCGIFHWADDR, &ifr))
	{
		printf("[GetEthEnStat] call ioctl fail: %s\n", strerror(errno));
		return -1;
	}
	
	if( close(skfd) == -1 )
	{
		printf("[GetEthEnStat] shutdown skfd(%d) faild, errno is: %s\n", skfd, strerror(errno));
	}

	return ifr.ifr_flags & IFF_UP;
	
}

/*=============================================================================
函 数 名： NotfiyGuardUpdateSuccess
功    能： 通知升级守卫进程已经正常启动，升级成功
算法实现： 
全局变量： 
参    数： const s8 * lpszLocalProcessName 本进程的名字
返 回 值： s8  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2012/05/10  4.7			chendaiwei            create
=============================================================================*/
BOOL32 NotfiyGuardUpdateSuccess( const s8* lpszLocalProcessName )
{
	if(lpszLocalProcessName == NULL)
	{
		printf("[NotfiyGuardUpdateSuccess] processName == NULL!\n");

		return FALSE;
	}
	
	s8 aszNamedPipe[MAX_VALUE_LEN+1] = {0};
	sprintf(aszNamedPipe,"/tmp/%s_pipe",lpszLocalProcessName);
	printf("[NotfiyGuardUpdateSuccess] Name is %s!\n",aszNamedPipe);

	int ret = mkfifo(aszNamedPipe, S_IRUSR | S_IWUSR);
	if( ret == EOF)
	{
		printf("[NotfiyGuardUpdateSuccess] mkfifo[%s] failed,errno is %d!\n",aszNamedPipe,errno);
		perror("Erro:");

		if(errno != EEXIST)
		{
			return FALSE;
		}
		else
		{
			if(unlink(aszNamedPipe) == EOF)
			{
				printf("[NotfiyGuardUpdateSuccess] unlink existed [%s] failed,errno is %d!\n",aszNamedPipe,errno);
			}

			if(mkfifo(aszNamedPipe, S_IRUSR | S_IWUSR) == EOF)
			{
				printf("[NotfiyGuardUpdateSuccess] re-mkfifo[%s] failed,errno is %d!\n",aszNamedPipe,errno);

				return FALSE;
			}
		}
	}
	
	FILE *hNamedPipe = fopen(aszNamedPipe,"w");
	if(hNamedPipe == NULL)
	{
		printf("[NotfiyGuardUpdateSuccess]open named Pipe %s failed",aszNamedPipe);

		return FALSE;
	}
	else
	{
		if( EOF == fputs(aszNamedPipe,hNamedPipe))
		{
			printf("[NotfiyGuardUpdateSuccess]write named Pipe %s failed",aszNamedPipe);

			fclose(hNamedPipe);
			
			return FALSE;
		}
		else
		{
			fclose(hNamedPipe);
		}
	}

	return TRUE;
}

/*=============================================================================
函 数 名： ExecShellCmd
功    能： 执行shell脚本，不会继承父进程的文件描述符(文件、socket等等)
算法实现： 类同于system函数
全局变量： 
参    数： 
注    意： 
返 回 值： int: 0表示正常，其他值表示出错
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/11/18   4.7	    彭国锋                  创建
=============================================================================*/
static int ExecShellCmd(const char * cmdstring)
{
	if ( !cmdstring )
	{
		return -1;
	}

	pid_t pid;
	int status;
	struct rlimit rlim;
	int max_fd = 0;
	if ( getrlimit(RLIMIT_NOFILE, &rlim) == 0 )
	{
		max_fd = rlim.rlim_cur;
	}
	else
	{
		return -1;
	}
	
	if((pid = vfork())<0)		
	{
		status = -1;		
	}	
	else if(pid == 0)		
	{
		for ( int i = 0; i< max_fd; i++)
		{
			close(i);
		}
		execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);		
		_exit(127);		
	}	
	else		
	{
		while(waitpid(pid, &status, 0) < 0)			
		{
			if(errno != EINTR)
			{
				status = -1;				
				break;				
			}			
		}		
	}
	
	return status;
}
/*=============================================================================
函 数 名： SetSshUp
功    能： 启用SSH服务
算法实现： Shell命令
全局变量： 
参    数： 
注    意： 
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/16   4.7		    彭国锋                  创建
=============================================================================*/
BOOL32 SetSshUp(void)
{
	s32 nRet = system("service ssh restart");
	DBG_LOG("SetSshUp ret.%d\n", nRet);
	return nRet == -1 ? FALSE:TRUE;
}

/*=============================================================================
函 数 名： SetSshDown
功    能： 禁用SSH服务
算法实现： Shell命令
全局变量： 
参    数： 
注    意： 
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/16   4.7		    彭国锋                  创建
=============================================================================*/
BOOL32  SetSshDown(void)
{
	s32 nRet = system("service ssh stop; kill -15 `ps -ef | grep \"sshd:.*\" | grep -v grep | awk '{print $2}'`");
	DBG_LOG("SetSshDown ret.%d\n", nRet);
	return nRet == -1 ? FALSE:TRUE;
}

/*=============================================================================
函 数 名： SetFtpUp
功    能： 启用Ftp服务
算法实现： Shell命令
全局变量： 
参    数： 
注    意： 
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/16   4.7		    彭国锋                  创建
=============================================================================*/
BOOL32	SetFtpUp(void)
{
	s32 nRet = system("service vsftpd restart");
	DBG_LOG("SetFtpUp ret.%d\n", nRet);
	return nRet == -1 ? FALSE:TRUE;
}

/*=============================================================================
函 数 名： SetFtpDown
功    能： 禁用Ftp服务
算法实现： Shell命令
全局变量： 
参    数： 
注    意： 
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/16   4.7		    彭国锋                  创建
=============================================================================*/
BOOL32	SetFtpDown(void)
{
	s32 nRet = system("service vsftpd stop");
	DBG_LOG("SetFtpDown ret.%d\n", nRet);
	return nRet == -1 ? FALSE:TRUE;
}

/*=============================================================================
函 数 名： SetTelnetUp
功    能： 启用Telnet服务
算法实现： Shell命令
全局变量： 
参    数： 
注    意： 这是禁止外部给本机的2500~2600端口发包;以后如果还有其他iptables规则,请将iptables -F;去掉
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/16   4.7		    彭国锋                  创建
=============================================================================*/
BOOL32	SetTelnetUp(void)
{
	s32 nRet = system("iptables -F;iptables -I INPUT -p tcp --dport 2500:2600 -j ACCEPT");
	DBG_LOG("SetTelnetUp ret.%d\n", nRet);
	return nRet == -1 ? FALSE:TRUE;
}

/*=============================================================================
函 数 名： SetTelnetDown
功    能： 禁用Telnet服务
算法实现： Shell命令
全局变量： 
参    数： 
注    意： 注意事项同SetTelnetUp
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/16   4.7		    彭国锋                  创建
=============================================================================*/
BOOL32	SetTelnetDown(void)
{
	s32 nRet = system("iptables -F;iptables -I INPUT -p tcp --dport 2500:2600 -j REJECT");
	DBG_LOG("SetTelnetDown ret.%d\n", nRet);
	return nRet == -1 ? FALSE:TRUE;
}

/*=============================================================================
函 数 名： AddSysRoute
功    能： 添加一条系统路由，立即生效
算法实现： Shell命令
全局变量： 
参    数：	u32 dwIp
			u32 dwMask
			u32 dwGw
			u8  byEthId
注    意： 主机序是必须的
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/16   4.7		    彭国锋                  创建
=============================================================================*/
BOOL32	AddSysRoute(u8 byEthId, u32 dwIp, u32 dwMask, u32 dwGw)
{
#ifdef _LINUX_
	s8 achCmd[128] = {0};
	sprintf(achCmd, ADD_STATIC_ROUTE_CMD, strofip(dwIp), strofip(dwMask), strofip(dwGw), byEthId);

	DBG_LOG("exec cmd.[%s]\n", achCmd);
	s32 nRet = system(achCmd);
	return nRet == -1? FALSE:TRUE;
#endif

#ifdef WIN32
	return TRUE;
#endif
}

/*=============================================================================
函 数 名： DelSysRoute
功    能： 删除一条系统路由，立即生效
算法实现： Shell命令
全局变量： 
参    数： u32 dwIp
			u32 dwMask
			u32 dwGw
			u8  byEthId
注    意： 
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/9/16   4.7		    彭国锋                  创建
=============================================================================*/
BOOL32	DelSysRoute(u8 byEthId, u32 dwIp, u32 dwMask, u32 dwGw)
{
#ifdef _LINUX_
	s8 achCmd[128] = {0};
	sprintf(achCmd, DEL_STATIC_ROUTE_CMD, strofip(dwIp), strofip(dwMask), strofip(dwGw), byEthId);
	
	DBG_LOG("exec cmd.[%s]\n", achCmd);
	s32 nRet = system(achCmd);
	return nRet == -1? FALSE:TRUE;
#endif

#ifdef WIN32
	return TRUE;
#endif
}

#endif



