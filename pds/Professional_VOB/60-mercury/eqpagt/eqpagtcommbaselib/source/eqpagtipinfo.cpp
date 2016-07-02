/*****************************************************************************
    模块名      : EqpAgt
    文件名      : eqpagtipinfo.h
    相关文件    : 
    文件实现功能: EqpAgt IP信息
    作者        : liaokang
    版本        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    修改记录:
    日  期      版本        修改人          修改内容
    2012/06/18  4.7         liaokang        创建
******************************************************************************/
#include "eqpagtipinfo.h"

#ifdef WIN32
#include "IPHlpApi.h"
#include "Tlhelp32.h"
#endif

// 检验IP地址是否有效
BOOL32 IsValidIpV4(const s8* lptrIPStr)
{
    if ( lptrIPStr == NULL ) 
    {
        return FALSE;
    }
    
    s32 nDot    = 0;                        //  已处理的字符串里的点号数
    s32 nData   = 0;                        //  已处理的字符串里的数值
    s32 nPos	= 0;						//	两点之间的间隔
    s32 nLen    = strlen( lptrIPStr );     //  字符串长度
    
    //  字符串最少7位最长15位, 且不能以'0'开头
    if ( nLen < 7 || nLen > 15 ) 
    {
        return FALSE;
    }
    
    if ( lptrIPStr[0] == '0'
        || ( lptrIPStr[0] == '2' && lptrIPStr[1] == '5'
        && lptrIPStr[2] == '5' ) )
    {
        return FALSE;
    }
    
    if ( lptrIPStr[0] == '.' || lptrIPStr[nLen - 1] == '.' )
    {
        return FALSE;
    }
    
    for ( s32 nCount = 0; nCount < nLen; ++nCount )
    {
        if ( lptrIPStr[nCount] >= '0' && lptrIPStr[nCount] <= '9' )//遇到数字
        {
            nData = 10 * nData + lptrIPStr[nCount] - '0';     //计算数值
            ++nPos;
        }
        else if ( lptrIPStr[nCount] == '.' )     //  遇到点号
        {
            nDot++;
            if ( nDot > 3 )  //  点号数大于3
            {
                return FALSE;
            }
            else if ( nPos > 3 )
            {
                return FALSE;
            }
            else if ( nData > 255 ) //数值大于255
            {
                return FALSE;
            }
            else        //  已处理的字符串合法
            {
                nData = 0;
                nPos = 0;
            }
        }
        else            //  遇到非法字符
        {
            return FALSE;
        }
    }
    
    if ( nDot != 3 )
    {
        return FALSE;
    }
    
    // 检查最后一个数字是否合法
    if ( nData > 255 ) //数值大于255
    {
        //		s_dwError = ERROR_VALUE;
        return FALSE;
    }    
    
    return TRUE;
}

// 主机序
BOOL32 IsValidIpV4(u32 dwIP)
{
    u8 byPos1 = HIBYTE( HIWORD( dwIP ) );
    if ( byPos1 == 0 || byPos1 == 255 )
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

BOOL32 IsValidIpMask(const s8* lptrIpMask)
{
    BOOL32 bRet = IsValidIpV4( lptrIpMask );
    if ( !bRet )
    {
        return FALSE;
    }
    
    u32 dwAddr = ntohl( inet_addr( lptrIpMask ) ); 
    bRet = IsValidIpMask( dwAddr );
    return bRet;
}

// 主机序
BOOL32 IsValidIpMask(u32 dwIpMask)
{
    if ( dwIpMask == 0 )
    {
        return FALSE;
    }
    
    u32 dwTemp1 = ~dwIpMask;
    u32 dwTemp2 = dwIpMask - dwTemp1;
    return ( (dwTemp1 & dwTemp2) == 1 );
}

#ifdef WIN32
// 网卡操作
typedef DWORD (WINAPI *GETADAPTERSINFO)( PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen );
typedef DWORD (__stdcall *type_NhGetInterfaceNameFromDeviceGuid)(GUID* guid, char* buf, DWORD* buflen, DWORD unknown1/*=0*/, DWORD unknown2/*=1*/);
#endif

void GUIDFormString(const char*pszGuid,GUID &guid)
{
    // GUID结构
//     typedef struct _GUID
//     {
//         unsigned long Data1;
//         unsigned short Data2;
//         unsigned short Data3;
//         unsigned char Data4[8];
//     } GUID;


    int  temp[8] = {'\0'};
    sscanf(pszGuid,"{%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
        &(guid.Data1), &(guid.Data2), &(guid.Data3),
        &temp[0], &temp[1], &temp[2], &temp[3], 
        &temp[4], &temp[5], &temp[6], &temp[7] );

    guid.Data4[0] = (unsigned char)temp[0];
    guid.Data4[1] = (unsigned char)temp[1];
    guid.Data4[2] = (unsigned char)temp[2];
    guid.Data4[3] = (unsigned char)temp[3];
    guid.Data4[4] = (unsigned char)temp[4];
    guid.Data4[5] = (unsigned char)temp[5];
    guid.Data4[6] = (unsigned char)temp[6];
    guid.Data4[7] = (unsigned char)temp[7];
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
    // 此段代码可以根据GUID得到接口名称
    // 但是根据NhGetInterfaceNameFromDeviceGuid得到的buf
    // 英文的显示为一个字符一个0间隔
    // 中文的显示为乱码，没有规律
    // 后续修改后支持
    // 放于此仅仅只是做测试用
//     HMODULE hDll = LoadLibrary("IPHlpApi.dll");
//     type_NhGetInterfaceNameFromDeviceGuid NhGetInterfaceNameFromDeviceGuid = (type_NhGetInterfaceNameFromDeviceGuid)GetProcAddress(hDll, "NhGetInterfaceNameFromDeviceGuid");
//     GUID guid;
//     GUIDFormString( "{2BEDDD6F-AAD4-44FE-86BF-6B5DD05CED45}", guid ); 
//     s8 achTemp[1024] = {0};
//     s8 achBuf[512] = {0};
//     u32 dwLen = sizeof(achTemp);
//     u32 ret = NhGetInterfaceNameFromDeviceGuid(&guid, (char*)achTemp, &dwLen, 0, 1);
//     if(ret == 0)
//     {
//         for ( s32 nLoop = 0; nLoop < sizeof(achBuf); nLoop++)
//         {
//             achBuf[nLoop] = achTemp[2*nLoop];
//         }
//         OspPrintf( TRUE, FALSE,"%s\n", achBuf);
//         // 成功，buf中保存的内容为字符串“本地连接”
//     }
//     FreeLibrary(hDll);





    if ( NULL == ptNetAdaptInfoAll )
    {
        return FALSE;
    }
#ifdef _LINUX_
    s32 nCount = 0;
    s32 nFd = -1;
    s32 nLoop = 0;
    s32 nEthernetAdapterNum = 0;
    struct ifconf tIfc;
    struct ifreq  tCopyIfr;	
    struct ifreq atIfcreq[OSP_ETHERNET_ADAPTER_MAX_IP * OSP_ETHERNET_ADAPTER_MAX_NUM];
    struct ifreq *ptIfr = NULL;

    memset(ptNetAdaptInfoAll, 0, sizeof(TNetAdaptInfoAll));
    memset(&tIfc, 0, sizeof(tIfc));
    memset(atIfcreq, 0, OSP_ETHERNET_ADAPTER_MAX_IP * OSP_ETHERNET_ADAPTER_MAX_NUM);

    tIfc.ifc_len = sizeof( struct ifreq) * OSP_ETHERNET_ADAPTER_MAX_IP * OSP_ETHERNET_ADAPTER_MAX_NUM;
    tIfc.ifc_req = (struct ifreq *)(&atIfcreq);
    
    nFd = socket(AF_INET, SOCK_DGRAM, 0);
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
    for ( ; tIfc.ifc_len - nCount >= sizeof(struct ifreq) ; nCount += sizeof(struct ifreq),  ptIfr += 1)
    {
		strcpy(tCopyIfr.ifr_name, ptIfr->ifr_name);
        s8 asIfName[32] = {0};
        s8 nIfEthIdx = GetEthIdxbyEthIfName( ptIfr->ifr_name );
        sprintf( asIfName, "eth%d", nIfEthIdx );

    	s32 nIfIndex;
		struct sockaddr_in *pSockAddr;
		struct ethtool_value tEcmd;
		s32 nIpNum = 0;
		u32 dwIp = 0;
		BOOL mFound = FALSE;

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
		nIfIndex = tCopyIfr.ifr_ifindex;

        TNetAdaptInfo * ptNetAdapter = NULL;
        u32 dwAdaptNum = ptNetAdaptInfoAll->m_tLinkNetAdapt.Length();
		for (nLoop = 0; nLoop < dwAdaptNum; nLoop ++)
		{
			if ( 0 == strcmp( ptNetAdaptInfoAll->m_tLinkNetAdapt.GetValue(nLoop)->GetAdapterName(), asIfName) )
			{
                ptNetAdapter = ptNetAdaptInfoAll->m_tLinkNetAdapt.GetValue(nLoop);
				mFound = TRUE;
				break;
			}
		}
		
        ioctl(nFd,SIOCGIFNETMASK,&tCopyIfr);
        
        u32 dwIfIp = (*(struct sockaddr_in *)&ptIfr->ifr_addr).sin_addr.s_addr;
        u32 dwIfMask = (*(struct sockaddr_in *)(&tCopyIfr.ifr_addr)).sin_addr.s_addr;
        TNetParam tIpParam;
        tIpParam.SetNetParam( ntohl(dwIfIp), ntohl(dwIfMask) );
					
		/* 如果不是第一次找到该接口名,则只需要更新IP地址即可,可进入下一个ifconfig*/

    if (mFound == TRUE)
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

		
		tNewAdapter.SetDefGWIpAddr(dwDefaultGW);
		
		if (ioctl(nFd, SIOCGIFHWADDR, ptIfr))
        {
            OspPrintf(TRUE,FALSE,"ioctl SIOCGIFHWADDR error %d", errno);
            continue;
        }
        tNewAdapter.SetMacAddress( (u8 *)ptIfr->ifr_hwaddr.sa_data, OSP_ETHERNET_ADAPTER_MAC_LENGTH );
        
        ptNetAdaptInfoAll->m_tLinkNetAdapt.Append( tNewAdapter );

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
        while( NULL != pNext )
        {
            TNetAdaptInfo tNetAdapt;
            memset( &tNetAdapt, 0, sizeof(tNetAdapt));
            tNetAdapt.SetAdapterName( pNext->AdapterName );
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
                memset( &tNetParam, 0, sizeof(tNetParam));
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
				tNetAdapt.SetDefGWIpAddr(dwIpAddr);
				break;	
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
函 数 名： GetNetAdapterInfoAll
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
	s32 nCharPos = 0;

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
        if ( bInUnused )
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
BOOL32 GetAdapterIp( u32 dwIfIdx, TAllEthParam* ptAllEthParm)
{

	if ( dwIfIdx > EQPAGT_MAX_ADAPTER_NUM - 1  ||
		 NULL == ptAllEthParm)
	{
		return FALSE;
	}

	BOOL32 bRet = FALSE;
	memset(ptAllEthParm, 0, sizeof(TAllEthParam));

	TNetAdaptInfoAll tNetAdapterInfoAll;
    if ( GetNetAdapterInfo( &tNetAdapterInfoAll ) )
	{
		TNetAdaptInfo *ptNetAdapt = tNetAdapterInfoAll.GetAdapterByEthId( dwIfIdx );
		if (ptNetAdapt != NULL)
		{
			 printf("---------------------------------\n"); 
             ptNetAdapt->Print(); 
             printf("---------------------------------\n"); 
			u32 dwIpNum= 0;
			for(; dwIpNum < EQPAGT_MAX_ADAPTER_NUM; dwIpNum ++)
			{
				TNetParam *ptNetParam =  ptNetAdapt->m_tLinkIpAddr.GetValue(dwIpNum);
				if(ptNetParam == NULL)
				{
					break;
				}
				ptAllEthParm->atEthParam[dwIpNum].dwIpAdrs = htonl(ptNetParam->GetIpAddr());
				ptAllEthParm->atEthParam[dwIpNum].dwIpMask = htonl(ptNetParam->GetIpMask());
				memcpy(ptAllEthParm->atEthParam[dwIpNum].byMacAdrs, ptNetAdapt->GetMacAddress(), min(sizeof(ptAllEthParm->atEthParam[dwIpNum].byMacAdrs), EQPAGT_MAX_ADAPTER_ADDRESS_LEN));
			}
			ptAllEthParm->dwIpNum = dwIpNum;
			
			bRet = TRUE;
		} 
        else 
        { 
           printf("no match active NetAdapter for eth%d\n", dwIfIdx ); 
        } 
	}
	return bRet;
}




#ifdef WIN32

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
函 数 名： GetRegistryProperty
功    能： 
算法实现： 
全局变量： 
参    数： HDEVINFO DeviceInfoSet
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

#endif