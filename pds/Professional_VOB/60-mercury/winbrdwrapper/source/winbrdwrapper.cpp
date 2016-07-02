/******************************************************************************
模块名  ： simbrdwrapper
文件名  ： BoardWrapper.h
相关文件：
文件实现功能：模拟BoardWrapper模块对外提供的主要功能
作者    ：john
版本    ：1.0.0.0.0
---------------------------------------------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
7/0/2006    0.1         john         创建
******************************************************************************/
#include <process.h>
#include "winbrd.h"
#include "mcuconst.h"

u32 g_dwPID = 0;
/*=============================================================================
  函 数 名： RegGetAdpName
  功    能： 
  算法实现： 
  全局变量： 
  参    数： s8* lpszAdpName
             u8 byLen
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 RegGetAdpName(s8* lpszAdpName, u16 wLen)
{
    if(NULL == lpszAdpName)
    {
        return FALSE;
    }

#ifdef WIN32
    HKEY hKey, hSubKey, hNdiIntKey;

    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
            "System\\CurrentControlSet\\Control\\Class\\{4d36e972-e325-11ce-bfc1-08002be10318}",
            0, KEY_READ, &hKey) != ERROR_SUCCESS)
        return FALSE;

    DWORD dwIndex = 0;
    DWORD dwBufSize = 256;
    DWORD dwDataType;
    char  szSubKey[256];
    unsigned char szData[256];
    BOOL32 bFlag = FALSE;

    while(ERROR_SUCCESS == RegEnumKeyEx(hKey, dwIndex++, szSubKey, &dwBufSize, NULL, NULL, NULL, NULL))
    {
        if(ERROR_SUCCESS == RegOpenKeyEx(hKey, szSubKey, 0, KEY_READ, &hSubKey))
        { 
            if(ERROR_SUCCESS == RegOpenKeyEx(hSubKey, "Ndi\\Interfaces", 0, KEY_READ, &hNdiIntKey))
            {
                memset(szData, '\0', sizeof(szData));
                dwBufSize = 256;
                if(ERROR_SUCCESS == RegQueryValueEx(hNdiIntKey, "LowerRange", 0, &dwDataType, szData, &dwBufSize))
                {
                    if(strcmp((char*)szData, "ethernet") == 0) // 判断是不是以太网卡
                    {
                        memset(szData, '\0', sizeof(szData));
                        dwBufSize = 256;
                        if(ERROR_SUCCESS == RegQueryValueEx(hSubKey, "DriverDesc", 0, &dwDataType, szData, &dwBufSize))
                        {
                            // szData 中便是适配器详细描述
                            memset(szData, '\0', sizeof(szData));    
                            dwBufSize = 256;
                            if(ERROR_SUCCESS == RegQueryValueEx(hSubKey, "NetCfgInstanceID", 0, &dwDataType, szData, &dwBufSize))
                            {
                                // szData 中便是适配器名称
                                wLen = (wLen < strlen((const char*)szData)) ? wLen : strlen((const char*)szData); // 长度保护
                                strncpy(lpszAdpName, (const char*)szData, wLen);
                                bFlag = TRUE;
                            }
                        }
                    }
                }
                RegCloseKey(hNdiIntKey);
            }
            RegCloseKey(hSubKey);
        }

        dwBufSize = 256;
        if(bFlag)
        {
            break; // 找到，退出while循环
        }
    } /* end of while */

    RegCloseKey(hKey);

#endif
    return TRUE;
}

/*=============================================================================
函 数 名： RegGetIpAdr
功    能： 
算法实现： 
全局变量： 
参    数： s8* lpszAdapterName
           s8* pIpAddr
           u16 wLen
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/11/30   4.0		周广程                  创建
=============================================================================*/
BOOL32 RegGetIpAdr(s8* lpszAdapterName, s8* pIpAddr, u16 wLen)
{
    if(NULL == lpszAdapterName || NULL == pIpAddr)
    {
        return FALSE;
    }
    if(0 == wLen) // ip地址不能为0
    {
        return FALSE;
    }
    
#ifdef WIN32

    HKEY hKey;
    s8 aszStrKeyName[1024] = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
    strcat(aszStrKeyName, lpszAdapterName);
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, (const s8*)aszStrKeyName, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        return FALSE;

    s8 aszIp[255];
    memset(aszIp, '\0', sizeof(aszIp));
    int nIpLen = sizeof(aszIp);

    u32 dwType = REG_MULTI_SZ;
    RegQueryValueEx(hKey, "IPAddress", 0, &dwType, (unsigned char*)aszIp, (unsigned long*)&nIpLen);
    if(wLen < nIpLen)
    {
        nIpLen = wLen;// 长度保护
    }
    memcpy(pIpAddr, aszIp, nIpLen);
    
    RegCloseKey(hKey);

#endif

    return TRUE;
}

/*=============================================================================
  函 数 名： RegGetNetMask
  功    能： 
  算法实现： 
  全局变量： 
  参    数： s8* lpszAdapterName
             s8* pNetMask
             u8 byLen
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 RegGetNetMask(s8* lpszAdapterName, s8* pNetMask, u16 wLen)
{
    if(NULL == lpszAdapterName || NULL == pNetMask)
    {
        return FALSE;
    }
    if(0 == wLen) // 掩码地址不能为0
    {
        return FALSE;
    }
    
#ifdef WIN32

    HKEY hKey;
    s8 aszStrKeyName[1024] = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
    strcat(aszStrKeyName, lpszAdapterName);
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, (const s8*)aszStrKeyName, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        return FALSE;

    s8 aszNetMask[255];
    memset(aszNetMask, '\0', sizeof(aszNetMask));
    int nMaskLen = sizeof(aszNetMask);

    u32 dwType = REG_MULTI_SZ;
    RegQueryValueEx(hKey, "SubnetMask", 0, &dwType, (unsigned char*)aszNetMask, (unsigned long*)&nMaskLen);
    if(wLen < nMaskLen)
    {
        nMaskLen = wLen;// 长度保护
    }
    memcpy(pNetMask, aszNetMask, nMaskLen);
    
    RegCloseKey(hKey);

#endif

    return TRUE;
}

/*=============================================================================
  函 数 名： RegGetNetGate
  功    能： 
  算法实现： 
  全局变量： 
  参    数： s8* lpszAdapterName
             s8* pNetGate
             u8 byLen
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 RegGetNetGate(s8* lpszAdapterName, s8* pNetGate, u16 wLen)
{
    if(NULL == lpszAdapterName || NULL == pNetGate)
    {
        return FALSE;
    }
    if(0 == wLen) // 网关地址不能为0
    {
        return FALSE;
    }

#ifdef WIN32
    
    HKEY hKey;
    s8 aszStrKeyName[1024] = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
    strcat(aszStrKeyName, lpszAdapterName);
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, (const s8*)aszStrKeyName, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        return FALSE;
    
    s8 aszNetGate[100];
    memset(aszNetGate, '\0', sizeof(aszNetGate));
    u16 wGateLen = sizeof(aszNetGate);

    u32 dwType = REG_MULTI_SZ;
    RegQueryValueEx(hKey, "DefaultGateway", 0, &dwType, (unsigned char*)aszNetGate, (unsigned long*)&wGateLen);
    if(wLen < wGateLen)
    {
        wGateLen = wLen; // 长度保护
    }
    memcpy(pNetGate, aszNetGate, wGateLen);

    RegCloseKey(hKey);

#endif
    
    return TRUE;
}

/*=============================================================================
  函 数 名： RegSetIpAdr
  功    能： 
  算法实现： 
  全局变量： 
  参    数： s8* pIpAddr
             u8 byLen
  返 回 值： BOOL 
=============================================================================*/
BOOL32 RegSetIpAdr(s8* lpszAdapterName, s8* pIpAddr, u16 wLen)
{
    if(NULL == lpszAdapterName || NULL == pIpAddr)
    {
        return FALSE;
    }
    if(0 == wLen) // 主机地址不能为0
    {
        return FALSE;
    }

#ifdef WIN32

    HKEY hKey;
    s8 aszStrKeyName[1024] = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
    strcat(aszStrKeyName, lpszAdapterName);
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, (const s8*)aszStrKeyName, 0, KEY_WRITE, &hKey) != ERROR_SUCCESS)
        return FALSE;

    s8 szIpAddress[100];
    strncpy(szIpAddress, pIpAddr, wLen);

    u8 byIpLen = strlen(szIpAddress);

    *(szIpAddress + byIpLen + 1) = 0x00; // REG_MULTI_SZ数据需要在后面再加个0
    byIpLen += 2;

    RegSetValueEx(hKey, "IPAddress", 0, REG_MULTI_SZ, (unsigned char*)szIpAddress, byIpLen);

    RegCloseKey(hKey);

#endif
    return TRUE;
}

/*=============================================================================
  函 数 名： RegSetNetMask
  功    能： 
  算法实现： 
  全局变量： 
  参    数： s8* pNetMask
             u8 byLen
  返 回 值： BOOL 
=============================================================================*/
BOOL32 RegSetNetMask(s8* lpszAdapterName, s8* pNetMask, u16 wLen)
{
    if(NULL == lpszAdapterName || NULL == pNetMask)
    {
        return FALSE;
    }
    if(0 == wLen) // 掩码地址不能为0
    {
        return FALSE;
    }

#ifdef WIN32
    
    HKEY hKey;
    s8 aszStrKeyName[1024] = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
    strcat(aszStrKeyName, lpszAdapterName);
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, (const s8*)aszStrKeyName, 0, KEY_WRITE, &hKey) != ERROR_SUCCESS)
        return FALSE;

    s8 aszNetMask[100];
    memset(aszNetMask, '\0', sizeof(aszNetMask));
    strncpy(aszNetMask, pNetMask, wLen);

    int nMaskLen = strlen(aszNetMask);

    *(aszNetMask + nMaskLen + 1) = 0x00;
    nMaskLen += 2;

    RegSetValueEx(hKey, "SubnetMask", 0, REG_MULTI_SZ, (unsigned char*)aszNetMask, nMaskLen);

    RegCloseKey(hKey);
    
#endif
    return TRUE;
}

/*=============================================================================
  函 数 名： RegSetNetGate
  功    能： 
  算法实现： 
  全局变量： 
  参    数： s8* pNetGate
             u8 byLen
  返 回 值： BOOL 
=============================================================================*/
BOOL32 RegSetNetGate(s8* lpszAdapterName, s8* pNetGate, u16 wLen)
{
    if(NULL == lpszAdapterName || NULL == pNetGate)
    {
        return FALSE;
    }
    if(0 == wLen) // 网关地址不能为0
    {
        return FALSE;
    }

#ifdef WIN32
    
    HKEY hKey;
    s8 aszStrKeyName[1024] = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
    strcat(aszStrKeyName, lpszAdapterName);
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, (const s8*)aszStrKeyName, 0, KEY_WRITE, &hKey) != ERROR_SUCCESS)
        return FALSE;
    
    char aszNetGate[100];
    strncpy(aszNetGate, pNetGate, 98);
    u8 byGateLen = strlen(aszNetGate);
    *(aszNetGate + byGateLen + 1) = 0x00;
    byGateLen += 2;

    u32 dwType = REG_MULTI_SZ;
    RegSetValueEx(hKey, "DefaultGateway", 0, dwType, (unsigned char*)aszNetGate, byGateLen);

    RegCloseKey(hKey);
    
#endif
    return TRUE;
}

/*=============================================================================
函 数 名： GetBrdIpArray
功    能： 
算法实现： 
全局变量： 
参    数：  u32 *pIpArray
           u8 &byIpNum
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/11/30   4.0		周广程                  创建
=============================================================================*/
BOOL32 GetBrdIpArray( u32 *pIpArray, u8 &byIpNum )
{
	if ( NULL == pIpArray || 0 == byIpNum )
	{
		OspPrintf(TRUE,FALSE, "[GetBrdIpArray] Get IP fail.Array point.0x%x, Num.%d\n", pIpArray, byIpNum );
		return FALSE;
	}

	s8 aszAdpName[255];
    s8 aszIp[255];
    memset(aszIp, 0, sizeof(aszIp));
    memset(aszAdpName, 0, sizeof(aszAdpName));

	BOOL32 bReturn = TRUE;

    u8 byLen = (u8)sizeof(aszAdpName);
    BOOL32 bRet = RegGetAdpName(aszAdpName, byLen);
    if(bRet)
    {
        bRet = RegGetIpAdr(aszAdpName, aszIp, sizeof(aszIp));
        if(bRet)
        {
			u8 byIpCount = 0;
			s8 achSeps[] = " \t";        // 分隔符
			s8 *pchToken = NULL;
			s8 *pchNTpos = NULL;		// 记录NULL terminal的位置
			u8 byNTnum = 0;
			// 整理
			pchToken = aszIp;
			while( byNTnum < 2 )
			{
				if ( *pchToken == '\0' )
				{
					byNTnum++;
					if ( *(pchToken+1) == '\0' )
					{
						byNTnum++;
						break;
					}
					else
					{
						byNTnum--;
						*pchToken = ' ';
					}
				}
				pchToken++;
			}
			
			pchToken = NULL;
			pchToken = strtok( aszIp, achSeps );
			while (NULL != pchToken)
			{
				if ( byIpCount >= byIpNum )
				{
					break;
				}
				*(pIpArray+byIpCount) = ntohl( INET_ADDR( pchToken ) );
				byIpCount++;
				pchToken = strtok( NULL, achSeps );
			}
			byIpNum = byIpCount;
			bReturn = TRUE;
        }
        else
        {
            byIpNum = 0;
			bReturn = FALSE;
        }
    }
    else
    {
		byIpNum = 0;
		bReturn = FALSE;
    }

	if ( !bReturn )
	{
		OspPrintf(TRUE,FALSE, "[GetBrdIpArray] Get IP fail.\n");
	}
	return bReturn;
}


/*=============================================================================
函 数 名： SetBrdIpArray
功    能： 
算法实现： 
全局变量： 
参    数：  u32 *pIpArray
           u8 &byIpNum
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/11/30   4.0		周广程                  创建
=============================================================================*/
BOOL32 SetBrdIpArray( u32 *pIpArray, u8 &byIpNum )
{
	if ( NULL == pIpArray || 0 == byIpNum )
	{
		OspPrintf(TRUE,FALSE, "[SetBrdIpArray] Set IP fail. Array point.0x%x, Num.%d\n", pIpArray, byIpNum );
		return FALSE;
	}

	s8 aszAdpName[255];
    s8 aszIp[255];
	s8 aszTemp[18];
    memset(aszIp, 0, sizeof(aszIp));
    memset(aszAdpName, 0, sizeof(aszAdpName));
	memset(aszTemp, 0, sizeof(aszTemp) );

	BOOL32 bReturn = TRUE;

    u8 byLen = (u8)sizeof(aszAdpName);
    BOOL32 bRet = RegGetAdpName(aszAdpName, byLen);
    if(bRet)
    {
		for ( u8 byIdx = 0; byIdx < byIpNum; byIdx++ )
		{
			memset(aszTemp, 0, sizeof(aszTemp) );
			sprintf( aszTemp, "%d.%d.%d.%d ", (pIpArray[byIdx]>>24)&0xff, (pIpArray[byIdx]>>16)&0xff, (pIpArray[byIdx]>>8)&0xff, pIpArray[byIdx]&0xff );
			strcat( aszIp, aszTemp );
			if ( strlen(aszIp) + sizeof("255.255.255.255") > 255 )
			{
				byIpNum = byIdx + 1;
				break;
			}
		}
		if ( RegSetIpAdr( aszAdpName, aszIp, strlen(aszIp) ) )
		{
			bReturn = TRUE;
		}
		else
		{
			byIpNum = 0;
			bReturn = FALSE;
		}
    }
    else
    {
		byIpNum = 0;
		bReturn = FALSE;
    }

	if ( !bReturn )
	{
		OspPrintf(TRUE,FALSE, "[SetBrdIpArray] Set IP fail.\n");
	}
	return bReturn;
}

/*=============================================================================
函 数 名： GetNetMaskIpArray
功    能： 
算法实现： 
全局变量： 
参    数：  u32 *pMaskArray
           u8 &byMaskNum
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/11/30   4.0		周广程                  创建
=============================================================================*/
BOOL32 GetNetMaskIpArray( u32 *pMaskArray, u8 &byMaskNum )
{
	if ( NULL == pMaskArray || 0 == byMaskNum )
	{
		OspPrintf(TRUE,FALSE, "[GetNetMaskIpArray] Get MASK fail.\n");
		return FALSE;
	}

	s8 aszAdpName[255];
    s8 aszMask[255];
    memset(aszMask, 0, sizeof(aszMask));
    memset(aszAdpName, 0, sizeof(aszAdpName));

	BOOL32 bReturn = TRUE;

    u8 byLen = (u8)sizeof(aszAdpName);
    BOOL32 bRet = RegGetAdpName(aszAdpName, byLen);
    if(bRet)
    {
        bRet = RegGetNetMask(aszAdpName, aszMask, sizeof(aszMask));
        if(bRet)
        {
			u8 byMaskCount = 0;
			s8 achSeps[] = " \t";        // 分隔符
			s8 *pchToken = NULL;
			s8 *pchNTpos = NULL;		// 记录NULL terminal的位置
			u8 byNTnum = 0;
			pchToken = aszMask;

			//整理
			while( byNTnum < 2 )
			{
				if ( *pchToken == '\0' )
				{
					byNTnum++;
					if ( *(pchToken+1) == '\0' )
					{
						byNTnum++;
						break;
					}
					else
					{
						byNTnum--;
						*pchToken = ' ';
					}
				}
				pchToken++;
			}
			
			pchToken = NULL;

			pchToken = strtok( aszMask, achSeps );
			while (NULL != pchToken)
			{
				if ( byMaskCount >= byMaskNum )
				{
					break;
				}
				*(pMaskArray+byMaskCount) = ntohl( INET_ADDR( pchToken ) );
				byMaskCount++;
				pchToken = strtok( NULL, achSeps );
			}
			byMaskNum = byMaskCount;
			bReturn = TRUE;
        }
        else
        {
            byMaskNum = 0;
			bReturn = FALSE;
        }
    }
    else
    {
		byMaskNum = 0;
		bReturn = FALSE;
    }

	if ( !bReturn )
	{
		OspPrintf(TRUE,FALSE, "[GetNetMaskIpArray] Get MASK fail.\n");
	}
	return bReturn;
}

/*=============================================================================
函 数 名： SetNetMaskIpArray
功    能： 
算法实现： 
全局变量： 
参    数：  u32 *pMaskArray
           u8 &byMaskNum
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/11/30   4.0		周广程                  创建
=============================================================================*/
BOOL32 SetNetMaskIpArray( u32 *pMaskArray, u8 &byMaskNum )
{
	if ( NULL == pMaskArray || 0 == byMaskNum )
	{
		OspPrintf(TRUE,FALSE, "[SetNetMaskIpArray] Set MASK fail. Array point.0x%x, Num.%d\n", pMaskArray, byMaskNum );
		return FALSE;
	}

	s8 aszAdpName[255];
    s8 aszMask[255];
	s8 aszTemp[18];
    memset(aszMask, 0, sizeof(aszMask));
    memset(aszAdpName, 0, sizeof(aszAdpName));
	memset(aszTemp, 0, sizeof(aszTemp) );

	BOOL32 bReturn = TRUE;

    u8 byLen = (u8)sizeof(aszAdpName);
    BOOL32 bRet = RegGetAdpName(aszAdpName, byLen);
    if(bRet)
    {
		for ( u8 byIdx = 0; byIdx < byMaskNum; byIdx++ )
		{
			memset(aszTemp, 0, sizeof(aszTemp) );
			sprintf( aszTemp, "%d.%d.%d.%d ", (pMaskArray[byIdx]>>24)&0xff, (pMaskArray[byIdx]>>16)&0xff, (pMaskArray[byIdx]>>8)&0xff, pMaskArray[byIdx]&0xff );
			strcat( aszMask, aszTemp );
			if ( strlen(aszMask) + sizeof("255.255.255.255") > 255 )
			{
				byMaskNum = byIdx + 1;
				break;
			}
		}
		if ( RegSetNetMask( aszAdpName, aszMask, strlen(aszMask) ) )
		{
			bReturn = TRUE;
		}
		else
		{
			byMaskNum = 0;
			bReturn = FALSE;
		}
    }
    else
    {
		byMaskNum = 0;
		bReturn = FALSE;
    }

	if ( !bReturn )
	{
		OspPrintf(TRUE,FALSE, "[SetNetMaskIpArray] Set MASK fail.\n");
	}
	return bReturn;
}
 /*=============================================================================
函 数 名： GetNetGateArray
功    能： 
算法实现： 
全局变量： 
参    数：  u32 *pNetGateArray
           u8 &byNetGateNum
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/11/30   4.0		周广程                  创建
=============================================================================*/
BOOL32 GetNetGateArray( u32 *pNetGateArray, u8 &byNetGateNum )
{
	if ( NULL == pNetGateArray || 0 == byNetGateNum )
	{
		OspPrintf(TRUE,FALSE, "[GetNetGateArray] Get NetGate fail.Array point.0x%x, Num.%d\n", pNetGateArray, byNetGateNum );
		return FALSE;
	}

	s8 aszAdpName[255];
    s8 aszNetGate[255];
    memset(aszNetGate, 0, sizeof(aszNetGate));
    memset(aszAdpName, 0, sizeof(aszAdpName));

	BOOL32 bReturn = TRUE;

    u8 byLen = (u8)sizeof(aszAdpName);
    BOOL32 bRet = RegGetAdpName(aszAdpName, byLen);
    if(bRet)
    {
        bRet = RegGetNetGate(aszAdpName, aszNetGate, sizeof(aszNetGate));
        if(bRet)
        {
			u8 byIpCount = 0;
			s8 achSeps[] = " \t";        // 分隔符
			s8 *pchToken = NULL;
			s8 *pchNTpos = NULL;		// 记录NULL terminal的位置
			u8 byNTnum = 0;
			// 整理
			pchToken = aszNetGate;
			while( byNTnum < 2 )
			{
				if ( *pchToken == '\0' )
				{
					byNTnum++;
					if ( *(pchToken+1) == '\0' )
					{
						byNTnum++;
						break;
					}
					else
					{
						byNTnum--;
						*pchToken = ' ';
					}
				}
				pchToken++;
			}
			
			pchToken = NULL;
			pchToken = strtok( aszNetGate, achSeps );
			while (NULL != pchToken)
			{
				if ( byIpCount >= byNetGateNum )
				{
					break;
				}
				*(pNetGateArray+byIpCount) = ntohl( INET_ADDR( pchToken ) );
				byIpCount++;
				pchToken = strtok( NULL, achSeps );
			}
			byNetGateNum = byIpCount;
			bReturn = TRUE;
        }
        else
        {
            byNetGateNum = 0;
			bReturn = FALSE;
        }
    }
    else
    {
		byNetGateNum = 0;
		bReturn = FALSE;
    }

	if ( !bReturn )
	{
		OspPrintf(TRUE,FALSE, "[GetNetGateArray] Get NetGate fail.\n");
	}
	return bReturn;
}

/*=============================================================================
函 数 名： SetNetGateArray
功    能： 
算法实现： 
全局变量： 
参    数：  u32 *pNetGateArray
           u8 &byNetGateNum
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/11/30   4.0		周广程                  创建
=============================================================================*/
BOOL32 SetNetGateArray( u32 *pNetGateArray, u8 &byNetGateNum )
{
	if ( NULL == pNetGateArray || 0 == byNetGateNum )
	{
		OspPrintf(TRUE,FALSE, "[SetNetGateArray] Set NetGate fail. Array point.0x%x, Num.%d\n", pNetGateArray, byNetGateNum );
		return FALSE;
	}

	s8 aszAdpName[255];
    s8 aszNetGate[255];
	s8 aszTemp[18];
    memset(aszNetGate, 0, sizeof(aszNetGate));
    memset(aszAdpName, 0, sizeof(aszAdpName));
	memset(aszTemp, 0, sizeof(aszTemp) );

	BOOL32 bReturn = TRUE;

    u8 byLen = (u8)sizeof(aszAdpName);
    BOOL32 bRet = RegGetAdpName(aszAdpName, byLen);
    if(bRet)
    {
		for ( u8 byIdx = 0; byIdx < byNetGateNum; byIdx++ )
		{
			memset(aszTemp, 0, sizeof(aszTemp) );
			sprintf( aszTemp, "%d.%d.%d.%d ", (pNetGateArray[byIdx]>>24)&0xff, (pNetGateArray[byIdx]>>16)&0xff, (pNetGateArray[byIdx]>>8)&0xff, pNetGateArray[byIdx]&0xff );
			strcat( aszNetGate, aszTemp );
			if ( strlen(aszNetGate) + sizeof("255.255.255.255") > 255 )
			{
				byNetGateNum = byIdx + 1;
				break;
			}
		}
		if ( RegSetNetGate( aszAdpName, aszNetGate, strlen(aszNetGate) ) )
		{
			bReturn = TRUE;
		}
		else
		{
			byNetGateNum = 0;
			bReturn = FALSE;
		}
    }
    else
    {
		byNetGateNum = 0;
		bReturn = FALSE;
    }

	if ( !bReturn )
	{
		OspPrintf(TRUE,FALSE, "[SetNetGateArray] Set NetGate fail.\n");
	}
	return bReturn;
}

/*=============================================================================
函 数 名： GetLocalIp
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： u32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/11/2   4.0		周广程                  创建
=============================================================================*/
u32 GetLocalIp()
{
	s8 achHostName[64];
	memset(achHostName, 0, sizeof(achHostName));
	// 取得本机名 
	if(SOCKET_ERROR == gethostname(achHostName, 64))
	{
		return 0;
	}
	// 通过主机名得到地址表
	struct hostent *pHost = gethostbyname(achHostName);
	if ( NULL == pHost )
	{
		return 0;
	}
	// 默认使用第一个IP地址 
	if ( NULL != pHost->h_addr_list[0] )
	{
		return( *(u32*)pHost->h_addr_list[0] );
	}
	else
	{
		return 0;
	}
}

/*=============================================================================
函 数 名： CreateDirectory
功    能： 
算法实现： 
全局变量： 
参    数： const s8* lpszDirPath
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/11/6   4.0		周广程                  创建
=============================================================================*/
BOOL32 CreateDirectory( const s8* lpszDirPath )
{
	if ( NULL == lpszDirPath )
	{
		return FALSE;
	}
	
    BOOL bOk = ::CreateDirectory( lpszDirPath, NULL );
    return bOk;
}

/*=============================================================================
函 数 名： CreatCfgDebugFile
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/11/2   4.0			周广程                  创建
=============================================================================*/
BOOL32 CreatCfgDebugFile()
{
    s8 achProfileName[64] = {0};
	memset(achProfileName, 0, sizeof(achProfileName));
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILENAME_CONFIG);

	FILE *stream = fopen( achProfileName, "r" );
    if( stream != NULL )
    {
		fclose(stream);
        return(	TRUE );
    }

	u8 byLop = 0;

	s8 achProfileDir[64];
	memset( achProfileDir, 0, sizeof(achProfileDir) );
	sprintf( achProfileDir, "%s", DIR_CONFIG );

    //guzh [2008/05/27]
	CreateDirectory( achProfileDir );
	stream = fopen( achProfileName, "w" );
	if ( stream == NULL )
	{
		OspPrintf(TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Creat brdcfgdebug.ini failed!\n");
		return FALSE;
	}
	fclose(stream);

	u32 adwIpArray[16] = {0};
	u8 byIpNum = 16;

	if ( !GetBrdIpArray( adwIpArray, byIpNum ) || byIpNum == 0 )
	{
		OspPrintf(TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Get local ip failed!\n");
		return FALSE;
	}
	
	u32 adwMaskIpArray[16] = {0};
	u8 byIpMaskNum = 16;

	if ( !GetNetMaskIpArray( adwMaskIpArray, byIpMaskNum ) || byIpMaskNum == 0 )
	{
		OspPrintf(TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Get local ip mask failed!\n");
		return FALSE;
	}

	u32 adwNetGateArray[16] = {0};
	u8 byNetGateNum = 16;

	if ( !GetNetGateArray( adwNetGateArray, byNetGateNum ) || byNetGateNum == 0 )
	{
		OspPrintf(TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Get local Netgate failed!\n");
		return FALSE;
	}
//	u32 dwIpAddr = ntohl( GetLocalIp() );
//	if ( 0 == dwIpAddr )
//	{
//		OspPrintf(TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Get local ip failed!\n");
//		return FALSE;
//	}
	
	// 获得进程名字
	s8 achFullPathBuf[255];   
	memset( achFullPathBuf, 0, sizeof(achFullPathBuf) );   
	GetModuleFileName( NULL, achFullPathBuf, 255 );
	s8 ch = '\\';
	s8 * pchDest = NULL;
	pchDest = strrchr( achFullPathBuf, ch );
	s8 achNameBuf[255];
	memset( achNameBuf, 0, 255 );
	if ( pchDest == NULL || pchDest == achFullPathBuf )
	{
		strncpy( achNameBuf, achFullPathBuf, strlen(achFullPathBuf) );
	}
	else
	{
		strncpy( achNameBuf, pchDest+1, strlen(pchDest) );
	}
	achNameBuf[sizeof(achNameBuf)-1] = '\0';
	strlwr( achNameBuf );

	//分析进程名字, 确定层槽号和单板类型
    u8 byMcuType = MCUTYPE_UNKOWN;
	u8 byLayer = 0;
	u8 bySlot = 0;
	u8 byType = 0;
	pchDest = NULL;
	if ( NULL != ( pchDest = strstr( achNameBuf, "mcu.exe" ) ) ||
         NULL != ( pchDest = strstr( achNameBuf, "mcu_8000b.exe" ) ) ||
         NULL != ( pchDest = strstr( achNameBuf, "mcu_8000c.exe" ) ) )
	{
        if ( NULL != ( pchDest = strstr( achNameBuf, "mcu.exe" ) ) )
        {
            byMcuType = MCUTYPE_MCU;
        }
        else
        {
            byMcuType = MCUTYPE_MINIMCU;
        }

		byLayer = 0;
		bySlot = 0;
		byType = DSL8000_BRD_MPC;
	}
	else if ( NULL != ( pchDest = strstr( achNameBuf, "cri" ) ) )
	{
		byLayer = 1;
		bySlot = 3;
		byType = DSL8000_BRD_CRI;
	}
	else if ( NULL != ( pchDest = strstr( achNameBuf, "dri" ) ) )
	{
		byLayer = 1;
		bySlot = 6;
		byType = DSL8000_BRD_DRI;
	}
	else if ( NULL != ( pchDest = strstr( achNameBuf, "dsc" ) ) )
	{
		byLayer =0;
		bySlot = 1;
		byType = DSL8000_BRD_MDSC;
	}
	else if ( NULL != ( pchDest = strstr( achNameBuf, "eqp" ) ) )
	{
		byLayer = 2;
		bySlot = 5;
		byType = DSL8000_BRD_MMP;
	}
	else
	{
		byLayer = 2;
		bySlot = 5;
		byType = DSL8000_BRD_MMP;
	}

	// layer
	s32 nValue = byLayer;
	BOOL32 bResult = SetRegKeyInt( achProfileName, SECTION_BoardConfig, KEY_Layer, nValue );
	if( FALSE == bResult )  
	{
		OspPrintf( TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Wrong profile while setting %s!\n", KEY_Layer );
	}

	// slot
	nValue = bySlot;
	bResult = SetRegKeyInt( achProfileName, SECTION_BoardConfig, KEY_Slot, nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Wrong profile while setting %s!\n", KEY_Slot );
	}

	// type
	nValue = byType;
	bResult = SetRegKeyInt( achProfileName, SECTION_BoardConfig, KEY_Type, nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Wrong profile while setting %s!\n", KEY_Type );
	}
	
	// Ip
	s8 achIpAddr[18] = {0};
	sprintf( achIpAddr, "%d.%d.%d.%d", (adwIpArray[0]>>24)&0xff, (adwIpArray[0]>>16)&0xff, (adwIpArray[0]>>8)&0xff, adwIpArray[0]&0xff );
	bResult = SetRegKeyString( achProfileName, SECTION_BoardConfig, KEY_BoardIpAddr, achIpAddr );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Wrong profile while setting %s!\n", KEY_BoardIpAddr );
	}

	s8 achEthEntry[8];
	memset( achEthEntry, 0, sizeof(achEthEntry) );
	s8 achEthIpNumEntry[16];
	memset( achEthIpNumEntry, 0, sizeof(achEthIpNumEntry) );
	s8 achEthIpAddrEntry[32];
	memset( achEthIpAddrEntry, 0, sizeof(achEthIpAddrEntry) );
	s8 achEthMacEntry[32];
	memset( achEthMacEntry, 0, sizeof(achEthMacEntry) );

    // 这里对MCU类型进行区分，因为MCS连接IP所在的网口，8000A和MINIMCU的习惯是反的
    // zgc, 2008-06-02
    u8 byEhtId = 0;
    if ( byMcuType == MCUTYPE_MCU )
    {
        byEhtId = 0;
    }
    else
    {
        byEhtId = 1;
    }

	// 这里填默认值
	sprintf( achEthEntry, "%s%d", ENTRYPART_ETHERNET, byEhtId );
	sprintf( achEthIpNumEntry, "%s%s", achEthEntry, ENTRYPART_IPNUM );
	sprintf( achEthMacEntry, "%s%s", achEthEntry, ENTRYPART_MAC );
	
	// IP num
	bResult = SetRegKeyInt( achProfileName, SECTION_IpConfig, achEthIpNumEntry, 0 );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Wrong profile while setting %s!\n", achEthIpNumEntry );
	}

	// Mac
	bResult = SetRegKeyString( achProfileName, SECTION_IpConfig, achEthMacEntry, "11:11:11:11:11:11" );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Wrong profile while setting %s!\n", achEthMacEntry );
	}

    if ( byMcuType == MCUTYPE_MCU )
    {
        byEhtId = 1;
    }
    else
    {
        byEhtId = 0;
    }

    // 这里对MCU类型进行区分，因为MCS连接IP所在的网口，8000A和MINIMCU的习惯是反的
    // zgc, 2008-06-02
	//这里填从PC上取到的值
	memset( achEthEntry, 0, sizeof(achEthEntry) );
	memset( achEthIpNumEntry, 0, sizeof(achEthIpNumEntry) );
	memset( achEthIpAddrEntry, 0, sizeof(achEthIpAddrEntry) );
	memset( achEthMacEntry, 0, sizeof(achEthMacEntry) );

	sprintf( achEthEntry, "%s%d", ENTRYPART_ETHERNET, byEhtId);
	sprintf( achEthIpNumEntry, "%s%s", achEthEntry, ENTRYPART_IPNUM );
	sprintf( achEthMacEntry, "%s%s", achEthEntry, ENTRYPART_MAC );

	// IP num
	bResult = SetRegKeyInt( achProfileName, SECTION_IpConfig, achEthIpNumEntry, byIpNum );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Wrong profile while setting %s!\n", achEthIpNumEntry );
	}

	// Ip
	s8 achIpAndMask[40] = {0};
	for ( byLop = 0; byLop < byIpNum; byLop++ )
	{
		memset( achIpAndMask, 0, sizeof(achIpAndMask) );
		sprintf( achIpAndMask, "%d.%d.%d.%d %d.%d.%d.%d", 
			(adwIpArray[byLop]>>24)&0xff, (adwIpArray[byLop]>>16)&0xff, (adwIpArray[byLop]>>8)&0xff, adwIpArray[byLop]&0xff,
			(adwMaskIpArray[byLop]>>24)&0xff, (adwMaskIpArray[byLop]>>16)&0xff, (adwMaskIpArray[byLop]>>8)&0xff, adwMaskIpArray[byLop]&0xff );
		sprintf( achEthIpAddrEntry, "%s%s%d", achEthEntry, ENTRYPART_IPADDR, byLop );
		bResult = SetRegKeyString( achProfileName, SECTION_IpConfig, achEthIpAddrEntry, achIpAndMask );
		if( bResult == FALSE )  
		{
			OspPrintf( TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Wrong profile while setting %s!ip.0x%x\n", achEthIpAddrEntry, adwIpArray[byLop]  );
		}
	}

	// Mac
	bResult = SetRegKeyString( achProfileName, SECTION_IpConfig, achEthMacEntry, "11:11:11:11:11:11" );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Wrong profile while setting %s!\n", achEthMacEntry );
	}

	// Ip route num
	bResult = SetRegKeyInt( achProfileName, SECTION_IpRoute, KEY_IpRouteNum, byNetGateNum );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Wrong profile while setting %s!\n", KEY_IpRouteNum );
	}

	s8 achIpRoute[64] = {0};
	s8 achIpRouteEntry[32] = {0};
	for ( byLop = 0; byLop < byNetGateNum; byLop++ )
	{
		memset( achIpRoute, 0, sizeof(achIpRoute) );
		sprintf( achIpRoute, "0.0.0.0 0.0.0.0 %d.%d.%d.%d", (adwNetGateArray[byLop]>>24)&0xff, (adwNetGateArray[byLop]>>16)&0xff, (adwNetGateArray[byLop]>>8)&0xff, adwNetGateArray[byLop]&0xff );
		memset( achIpRouteEntry, 0, sizeof(achIpRouteEntry) );
		sprintf( achIpRouteEntry, "%s%d", ENTRYPART_IpRoute, byLop );
		bResult = SetRegKeyString( achProfileName, SECTION_IpRoute, achIpRouteEntry, achIpRoute );
		if( bResult == FALSE )  
		{
			OspPrintf( TRUE, FALSE, "[WinBrd][CreatCfgDebugFile] Wrong profile while setting %s!\n", achIpRouteEntry );
		}
	}

	return TRUE;
}

/*=============================================================================
函 数 名： MoveToSectionStart
功    能： 将游标移到指定的段名下一行开始处
算法实现： 
全局变量： 
参    数： FILE *stream, Profile名柄   
           const s8* lpszSectionName, Profile中的段名
           BOOL32 bCreate, 如果没有该段名是否创建
返 回 值： 返回偏移量Offset，如果失败返回-1
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/7/31   4.0			周广程                  创建
=============================================================================*/
s32	MoveToSectionStart( FILE *stream, const s8* lpszSectionName, BOOL32 bCreate )
{
	char    achRow[MAX_LINE_LEN + 1];
    char    achSectionCopy[MAX_SECTIONNAME_LEN + 1];
    u32  dwLen;
    s8*   lpszReturn;

    ASSERT( strlen( lpszSectionName ) <= MAX_SECTIONNAME_LEN );

    /* move from the beginning */
    if ( fseek( stream, 0, SEEK_SET ) != 0 )
    {
        return( -1 );
    }

    /* copy the section name and change it */
    strncpy( achSectionCopy, lpszSectionName, MAX_SECTIONNAME_LEN );
    achSectionCopy[MAX_SECTIONNAME_LEN] = '\0';
    Trim( achSectionCopy );
    StrUpper( achSectionCopy );

    do
    {
        /* if error or arrive at the end of file */
		memset( achRow, 0, sizeof(achRow) );
        if( fgets( achRow, MAX_LINE_LEN, stream ) == NULL )
        {
            if( bCreate && feof( stream ) )    /* create the section */
            {
                fputs( (const s8*)STR_RETURN, stream );
                fputs( (const s8*)"[", stream );
                fputs( (const s8*)lpszSectionName, stream );
                fputs( (const s8*)"]", stream );
                fputs( (const s8*)STR_RETURN, stream );
                fseek( stream, 0, SEEK_END );
                return( ftell( stream ) );
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
        
        /* must be a section */
        if( dwLen <= 2 || achRow[0] != '[' || achRow[dwLen - 1] != ']' )
        {
            continue;
        }
        
        /* verify the section name */
        strncpy( achRow, achRow + 1, dwLen - 2 );
        achRow[dwLen - 2] = '\0';
        Trim( achRow );
        StrUpper( achRow );
        if( strcmp( achRow, achSectionCopy ) == 0 )    /* identical */
        {
            return( ftell( stream ) );
        }
    } while( TRUE );
}

/*=============================================================================
函 数 名： MoveToSectionEnd
功    能： 将游标移到指定的段名的下一段开始处或是文件结尾
算法实现： 
全局变量： 
参    数： FILE *stream, Profile名柄   
           const s8* lpszSectionName, Profile中的段名
           BOOL32 bCreate, 如果没有该段名是否创建
返 回 值： 返回偏移量Offset，如果失败返回-1
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/7/31   4.0			周广程                  创建
=============================================================================*/
s32 MoveToSectionEnd( FILE *stream, const s8* lpszSectionName, BOOL32 bCreate )
{
	char    achRow[MAX_LINE_LEN + 1];
    u32		dwLen;
    s8*		lpszReturn;

	/* get section start offset */
	s32 nStartOffset = MoveToSectionStart( stream, lpszSectionName, bCreate );
	if ( -1 == nStartOffset )
	{
		OspPrintf( TRUE, FALSE, "[MoveToSectionEnd] Move to section %s start failed!\n", lpszSectionName );
		return nStartOffset;
	}

	/* move from the beginning */
    if ( fseek( stream, nStartOffset, SEEK_SET ) != 0 )
    {
        return( -1 );
    }

	s32 nScanOffset = nStartOffset;
	do
    {
        /* if error or arrive at the end of file */
		memset( achRow, 0, sizeof(achRow) );
        if( fgets( achRow, MAX_LINE_LEN, stream ) == NULL )
        {
            if ( feof(stream) != 0 )
			{
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
        
				/* must be a section */
				if( dwLen <= 2 || achRow[0] != '[' || achRow[dwLen - 1] != ']' )
				{
					return ftell(stream);
				}
				else
				{
					return nScanOffset;
				}
			}
			else
			{
				return (-1);
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

		/* must be a section */
		if( dwLen <= 2 || achRow[0] != '[' || achRow[dwLen - 1] != ']' )
		{
			nScanOffset = ftell(stream);
			continue;
		}
		else
		{
			return nScanOffset;
		}

    } while( TRUE );
}


/*=============================================================================
函 数 名： IsMacAddrInvalid
功    能： 
算法实现： 
全局变量： 
参    数： s8* pchMacAddr
           s8 chSep
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/12/4   4.0		周广程                  创建
=============================================================================*/
BOOL32 IsMacAddrInvalid(s8* pchMacAddr, s8 chSep)
{
	if ( NULL == pchMacAddr )
	{
		return TRUE;
	}

	u32 dwlenth = strlen(pchMacAddr);
	if( dwlenth != 17 )
	{
		return TRUE;
	}

	for( u32 dwLoop=0; dwLoop<dwlenth; dwLoop++ )
	{
		if( (dwLoop+1)%3 == 0  )
		{
			if( pchMacAddr[dwLoop] != chSep )
				return TRUE;
		}
		else if( 
				!(
					( pchMacAddr[dwLoop] >= '0' && pchMacAddr[dwLoop] <= '9' ) 
					|| ( pchMacAddr[dwLoop] >= 'a' && pchMacAddr[dwLoop] <= 'f' )
					|| ( pchMacAddr[dwLoop] >= 'A' && pchMacAddr[dwLoop] <= 'F' )
				 )
			   )
		{
			return TRUE;
		}
	}

	return FALSE;
}


/*=============================================================================
函 数 名： DelSection
功    能： 
算法实现： 
全局变量： 
参    数：  FILE *stream
           const s8* lpszSectionName
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/12/4   4.0		周广程                  创建
=============================================================================*/
s32	DelSection( FILE *stream, const s8* lpszSectionName )
{
	if ( stream == NULL || lpszSectionName == NULL )
	{
		return -1;
	}
	fseek( stream, 0, SEEK_SET );
	s32 nSecStartOffset = MoveToSectionStart( stream, lpszSectionName, FALSE );
	if ( -1 == nSecStartOffset )
	{
		OspPrintf( TRUE, FALSE,"[DelSection] Move to section %s start failed!\n", lpszSectionName );
		return -1;
	}
	OspPrintf( TRUE, FALSE, "[DelSection] Sec %s start offset: %d\n", lpszSectionName, nSecStartOffset );
	s32 nSetEndOffset = MoveToSectionEnd( stream, lpszSectionName, FALSE );
	if ( -1 == nSetEndOffset )
	{
		OspPrintf( TRUE, FALSE, "[DelSection] Move to section %s end failed!\n", lpszSectionName );
		return -1;
	}
	OspPrintf( TRUE, FALSE, "[DelSection] Sec %s end offset: %d\n", lpszSectionName, nSetEndOffset );
	u32 dwLen = nSetEndOffset - nSecStartOffset;
	if ( 0 == dwLen )
	{
		return ftell(stream);
	}

	fseek( stream, 0, SEEK_END );
	u32 dwCpyLen = ftell(stream) - nSetEndOffset;
	
	s8 *lpszCpyBuf = (s8*)malloc( dwCpyLen + dwLen );
	if ( lpszCpyBuf == NULL )
	{
		return -1;
	}
	memset( lpszCpyBuf, 0, dwCpyLen + dwLen );
	fseek( stream, nSetEndOffset, SEEK_SET );
	dwCpyLen = fread( lpszCpyBuf, sizeof( char ), dwCpyLen, stream );
	fseek( stream, nSecStartOffset, SEEK_SET );
	dwCpyLen = fwrite( lpszCpyBuf, sizeof( char ), dwCpyLen+dwLen, stream );

	free(lpszCpyBuf);
	return ftell(stream);
}


/*=============================================================================
函 数 名： TrimCfgFile
功    能： 
算法实现： 
全局变量： 
参    数： const s8* lpszCfgFilePath
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/12/4   4.0		周广程                  创建
=============================================================================*/
BOOL32 TrimCfgFile( const s8* lpszCfgFilePath )
{
	FILE *stream = fopen( lpszCfgFilePath, "rb+" );
    if( stream == NULL )
    {
        return( FALSE );
    }
	fseek( stream, 0, SEEK_END );
	u32 dwFileLen = ftell( stream );
	s8 *lpszFileBuf = (s8*)malloc( dwFileLen+3 );
	memset( lpszFileBuf, 0 , dwFileLen );

	s8 *lpszTemp = lpszFileBuf;
	s8 *lpszReturn = NULL;
	u32 dwLen = 0;
	u32 dwTrimLen = 0;
	char achRow[MAX_LINE_LEN + 1];
	fseek( stream, 0, SEEK_SET );
	do 
	{
		memset( achRow, 0, sizeof(achRow) );
		if( fgets( achRow, MAX_LINE_LEN, stream ) == NULL )
		{
			if ( 0 == feof(stream) )
			{
				return FALSE;
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
		
		if ( dwLen > 0 )
		{
			sprintf( lpszTemp, "%s", achRow );
			lpszTemp += dwLen;
			sprintf( lpszTemp, "%s", STR_RETURN );
			lpszTemp += strlen(STR_RETURN);
		}
		
	} while( 0 == feof(stream) );

	dwTrimLen = lpszTemp - lpszFileBuf;

	fclose( stream );
	stream = fopen( lpszCfgFilePath, "w" );
	fwrite( lpszFileBuf, sizeof(s8), dwTrimLen, stream );

	free( lpszFileBuf );
	fclose( stream );
	
	return TRUE;
}

/*================================
函数名：BrdInit 
功能：本模块的初始化函数
算法实现：（可选项）
引用全局变量：
输入参数说明：  无
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdInit(void)
{
    g_dwPID = getpid();
	return OK;
}

/*================================
函数名：BrdQueryPosition 
功能：板身份(ID、层号、槽位)查询
算法实现：（可选项）
引用全局变量：
输入参数说明：  ptBrdPosition： 由该函数返回板身份信息的结构指针
返回值说明： 错误返回ERROR；成功返回OK和板身份信息，具体如TBrdPosition结构描述
==================================*/
STATUS BrdQueryPosition(TBrdPosition *ptBrdPosition)
{
	if ( !CreatCfgDebugFile() )
	{
		return ERROR;
	}
	
	s8 achProfileName[32];
	memset(achProfileName, 0, sizeof(achProfileName));
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILENAME_CONFIG);

	s32 nValue = 0;
	BOOL32 bResult = GetRegKeyInt( achProfileName, SECTION_BoardConfig, KEY_Layer, 0, &nValue );
	if( FALSE == bResult )  
	{
		OspPrintf( TRUE, FALSE, "[WinBrd] Wrong profile while reading %s!\n", KEY_Layer );
		return( ERROR );
	}
	ptBrdPosition->byBrdLayer = (u8)nValue;

	bResult = GetRegKeyInt( achProfileName, SECTION_BoardConfig, KEY_Slot, 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[WinBrd] Wrong profile while reading %s!\n", KEY_Slot );
		return( ERROR );
	}
	ptBrdPosition->byBrdSlot = (u8)nValue;

	bResult = GetRegKeyInt( achProfileName, SECTION_BoardConfig, KEY_Type, 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[WinBrd] Wrong profile while reading %s!\n", KEY_Type );
		return( ERROR );
	}
	ptBrdPosition->byBrdID = (u8)nValue;

	return OK;
}

/*================================
函数名：BrdGetBoardID
功能：设备身份ID查询
算法实现：（可选项）
引用全局变量：
输入参数说明：  无
返回值说明： 如：单板种类ID号宏定义
==================================*/
u8  BrdGetBoardID(void)
{
	if ( !CreatCfgDebugFile() )
	{
		return 0;
	}

	s8 achProfileName[32];
	memset(achProfileName, 0, sizeof(achProfileName));
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILENAME_CONFIG);

	s32 nValue = 0;
	BOOL32 bResult = GetRegKeyInt( achProfileName, SECTION_BoardConfig, KEY_Type, 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, FALSE, "[Brd] Wrong profile while reading %s!\n", KEY_Type );
		return 0;
	}
	return (u8)nValue;	
}

/*================================
函数名：BrdGetFuncID
功能：设备功能ID查询
算法实现：（可选项）
引用全局变量：
输入参数说明：  无
返回值说明： 单板功能ID
==================================*/
//u8  BrdGetFuncID(void);

/*================================
函数名：BrdQueryHWVersion 
功能：硬件版本号查询
算法实现：（可选项）
引用全局变量：
输入参数说明：  无
返回值说明： 硬件版本号，4个BIT位，取值范围0~16
==================================*/
u8  BrdQueryHWVersion (void)
{
	return (u8)BOARDHWVER;
}

/*================================
函数名：BrdQueryFPGAVersion 
功能：FPGA或EPLD版本号查询
算法实现：（可选项）
引用全局变量：
输入参数说明：无
返回值说明： FPGA或EPLD版本号，4个BIT位，取值范围0~16
==================================*/
u8  BrdQueryFPGAVersion (void)
{
	return (u8)BOARDFPGAVER;
}

/*================================
函数名：BrdEthPrintEnable
功能：设置是否以太网状态打印有效
算法实现：（可选项）
引用全局变量：
输入参数说明：  
返回值说明： 无。
==================================*/
void BrdEthPrintEnable(BOOL32 bEnable)
{
	return;
}

/*================================
函数名：BrdQueryOsVer
功能：提供给业务层调用的os版本号查询
算法实现：（可选项）
引用全局变量：
输入参数说明：  pchVer： 获取版本的字符串指针
                dwBufLen：该字符串的长度
                pdwVerLen：获取版本的实际字符串长度，该长度返回之前必须和dwBufLen比较是否溢出，正常时才对pVer赋值！

返回值说明： 无。如果实际字符串长度大于dwBufLen，pVerLen赋值为0
==================================*/
void  BrdQueryOsVer(s8 *pchVer, u32 dwBufLen,u32 *pdwVerLen)
{
	u32 length = strlen(OSVERSION) < dwBufLen ? strlen(OSVERSION) : dwBufLen ;

	memcpy( pchVer, OSVERSION, length );

	*pdwVerLen = length;
	return;
}

/*================================
函数名：BrdGetBSP15Speed
功能：获取BSP-15的主频
算法实现：（可选项）
引用全局变量：
输入参数说明： byDevID:BSP-15的ID号(从0开始）
返回值说明： 成功返回BSP-15的主频,失败返回ERROR
==================================*/
u8 BrdGetBSP15Speed(u8 byDevID)
{
	u8 byretspeed = 0;
	if( byDevID < (u8)5 )
	{
		byretspeed = BSP15SPEED;
	}

	return byretspeed;
}

/*================================
函数名：BrdGetBSP15SdramSize
功能：获取BSP-15的SDRAM的大小
算法实现：（可选项）
引用全局变量：
输入参数说明： byDevID:BSP-15的ID号(从0开始）
返回值说明： 成功返回BSP-15SDRAM的大小,失败返回ERROR
==================================*/
u8 BrdGetBSP15SdramSize(u8 byDevID)
{
	u8 bydramsize = 0;
	if( byDevID < 5 )
	{
		bydramsize = BSP15SPEED;
	}

	return bydramsize;
}

/*串口控制接口*/

/*================================
函数名：BrdOpenSerial
功能：提供给业务层打开Rs232 Rs422 Rs485红外串口
算法实现：（可选项）
引用全局变量：
输入参数说明：byPort串口端口号
              #define SERIAL_RS232                 0
              #define SERIAL_RS422                 1
              #define SERIAL_RS485                 2
              #define BRD_SERIAL_INFRARED          3
返回值说明： 打开串口的句柄
==================================*/
s32 BrdOpenSerial(u8 byPort)
{
	s32 nret = 0;
	switch( byPort )
	{
	case BRD_SERIAL_RS232:
		nret = 0;
		break;
	case BRD_SERIAL_RS422:
		nret = 1;
		break;
	case BRD_SERIAL_RS485:
		nret = 2;
		break;
	case BRD_SERIAL_INFRARED:
		nret = 3;
		break;
	default:
		nret = 5;
		break;
	}
	return nret;
}

/*================================
函数名：BrdCloseSerial
功能：提供给业务层关闭Rs232 Rs422 Rs485红外串口
算法实现：（可选项）
引用全局变量：
输入参数说明：  nFd：串口文件描述符，由BrdOpenSerial函数返回
返回值说明： 成功返回0，失败返回ERROR
==================================*/
s32 BrdCloseSerial(s32 nFd)
{
	return OK;
}

/*================================
函数名：BrdReadSerial
功能：提供给业务层读取Rs232 Rs422 Rs485串口
算法实现：（可选项）
引用全局变量：
输入参数说明：  nFd：串口文件描述符，由BrdOpenSerial函数返回
                pbyBuf：存放数据的缓冲区指针
                nMaxbytes：要读取的数据的最大字节数
返回值说明： 成功返回读取数据的字节数，范围1~ nMaxbytes，0表示没有数据，失败返回
            ERROR
==================================*/
s32  BrdReadSerial(s32 nFd, s8  *pbyBuf, s32 nMaxbytes)
{
	return OK;
}

/*================================
函数名：BrdWriteSerial 
功能：提供给业务层写Rs232 Rs422 Rs485红外串口
算法实现：（可选项）
引用全局变量：
输入参数说明：  nFd：串口文件描述符，由BrdOpenSerial函数返回
                pbyBuf：要写入的数据的缓冲区指针
                nBytes：要写入数据的字节数
返回值说明： 成功返回写入数据的字节数，且等于nBytes，描述符无效返回ERROR，写入值
             不等于nBytes表示写入发生错误

==================================*/
s32 BrdWriteSerial (s32 nFd, s8  *pbyBuf, s32 nBytes)
{
	return OK;
}

/*================================
函数名：BrdIoctlSerial 
功能：提供给业务层控制Rs232 Rs422 Rs485内置红外串口，同ioctl函数
算法实现：（可选项）
引用全局变量：
输入参数说明：  nFd：串口文件描述符，由BrdOpenSerial函数返回
                nFunction：操作功能码，见相关的串口操作宏定义
                nArg：参数
返回值说明： 成功返回OK，描述符无效或失败返回ERROR
==================================*/
s32 BrdIoctlSerial (s32 nFd, s32 nFunction, s32 nArg)
{
	return OK;
}

/*================================
函数名：BrdRs485QueryData
功能：Rs485查询数据，先发查询帧，后接收对端数据
算法实现：（可选项）
引用全局变量：
输入参数说明：  nFd：串口文件描述符，由BrdOpenSerial函数返回
		        ptRS485InParam：存放输入参数的结构指针；
		        ptRS485RtnData: 存放返回信息的结构指针。
返回值说明：  485状态相关宏定义 
==================================*/
s32 BrdRs485QueryData (s32 nFd, TRS485InParam *ptRS485InParam,TRS485RtnData *ptRS485RtnData)
{
	if( NULL == ptRS485InParam || NULL == ptRS485RtnData )
	{
		return ERROR;
	}
	return OK;
}

/*================================
函数名：BrdRs485TransSnd 
功能：Rs485透明发送
算法实现：（可选项）
引用全局变量：
输入参数说明：  nFd：串口文件描述符，由BrdOpenSerial函数返回
                pbyMsg：待发送的数据；
                dwMsgLen: 数据长度
返回值说明： 485状态相关宏定义
==================================*/
s32 BrdRs485TransSnd (s32 nFd, u8 *pbyMsg, u32 dwMsgLen)
{
	return OK;
}

/*================================
函数名：BrdLedStatusSet
功能：设置灯的状态
算法实现：（可选项）
引用全局变量：
输入参数说明：  byLedID:灯的ID号（见BoardWrapper.h中相关的定义）
                byState:灯的状态（见BoardWrapper.h中相关的定义）
返回值说明： 成功返回OK,失败返回ERROR
==================================*/
s32  BrdLedStatusSet(u8 byLedID, u8 byState)
{
    printf("[BrdLedStatusSet] Set Led.%d to State.%d\n", byLedID, byState);
	return OK;
}
  
/*================================
函数名：BrdQueryLedState
功能：闪灯模式查询 
算法实现：（可选项）
引用全局变量：
输入参数说明：  ptBrdLedState：存放灯的状态的结构体指针
返回值说明： 成功返回OK,失败返回ERROR
==================================*/
s32  BrdQueryLedState(TBrdLedState *ptBrdLedState)
{
	if( NULL == ptBrdLedState )
	{
		return ERROR;
	}
	return OK;
}

/*================================
函数名：BrdQueryNipState
功能：Nip状态查询 
算法实现：（可选项）
引用全局变量：
输入参数说明： 无
返回值说明： 成功返回OK,失败返回ERROR
==================================*/
s32  BrdQueryNipState(void)
{
	return OK;
}

/*================================
函数名：SysRebootEnable 
功能：允许系统出现exception时自动复位
算法实现：（可选项）
引用全局变量：
输入参数说明：  无
返回值说明： 无
==================================*/
void  SysRebootEnable (void)
{
	return;
}

/*================================
函数名：SysRebootDisable
功能：禁止系统出现exception时自动复位
算法实现：（可选项）
引用全局变量：
输入参数说明：  无
返回值说明： 无
==================================*/
void  SysRebootDisable(void)
{
	return;
}

/*================================
函数名：BrdWakeup
功能：唤醒设备
算法实现：（可选项）
引用全局变量：
输入参数说明：  无
返回值说明： 成功返回OK ；失败返回ERROR
==================================*/
STATUS  BrdWakeup(void)
{
	return OK;
}

/*================================
函数名：BrdSleep 
功能：休眠设备
算法实现：（可选项）
引用全局变量：
输入参数说明：  无
返回值说明： 成功返回OK ；失败返回ERROR
==================================*/
STATUS  BrdSleep (void)
{
	return OK;
}

/*================================
函数名：BrdHwReset
功能：系统硬件复位
算法实现：（可选项）
引用全局变量：
输入参数说明：  无
返回值说明： 无
==================================*/
void  BrdHwReset(void)
{
    ExitProcess(0);
	return;
}

/*================================
函数名：SysRebootHookAdd
功能：复位回调注册函数
算法实现：（可选项）
引用全局变量：
输入参数说明：  指向VOIDFUNCPTR类型的函数指针
返回值说明： 成功返回OK ；失败返回ERROR
==================================*/
//STATUS SysRebootHookAdd(VOIDFUNCPTR ptRebootHookFunc)
//{
//	return OK;
//}

/*================================
函数名：SysOpenWdGuard
功能：提供给业务层3.0版本的打开系统守卫函数
算法实现：（可选项）
引用全局变量：
输入参数说明：  dwNoticeTimeout：通知消息时间，如果应用程序超过这个时间还没有通知操作系统消息，系统将复位。时间单位为秒。
					如果设置为0，则认为应用程序不需要定时发送通知消息，当该应用程序的进程状态异常时系统复位。
返回值说明： 成功返回OK,失败返回ERROR
==================================*/
STATUS SysOpenWdGuard(u32 dwNoticeTimeout)
{
	return OK;
}

/*================================
函数名：SysCloseWdGuard
功能：提供给业务层3.0版本的关闭系统守卫函数
算法实现：（可选项）
引用全局变量：
输入参数说明：  无
返回值说明： 成功返回OK,失败返回ERROR
==================================*/
STATUS SysCloseWdGuard(void)
{
	return OK;
}

/*====================================================================
函数名      : SysNoticeWdGuard
功能        ：提供给业务层3.0版本的应用程序通知操作系统守卫运行状态，同时闪烁运行灯。当打开系统守卫后，
			  应用程序就必须在注册所设定的时间内调用这个接口，否则操作系统将复位系统。
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：无
返回值说明  ：出错返回ERROR；成功返回OK。
====================================================================*/
STATUS SysNoticeWdGuard(void)
{
	return OK;
}

/*====================================================================
函数名      : SysWdGuardIsOpened
功能        ：提供给业务层3.0版本的获取系统守卫状态。
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：无
返回值说明  ：打开返回TRUE；关闭返回FALSE。
====================================================================*/
BOOL32 SysWdGuardIsOpened(void)
{
	return TRUE;
}

/*====================================================================
函数名      : BrdSetWatchdogMode
功能        ：设置看门狗工作模式
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：byMode:如下看门狗宏定义 
                #define WATCHDOG_USE_CLK            0x00   时钟硬件喂狗 
                #define WATCHDOG_USE_SOFT           0x01    软件喂狗 
                #define WATCHDOG_STOP                0x02   停止喂狗，设备会立即重启
返回值说明  ：无
说明：由于在VxWorks上由业务程序喂狗频繁出现问题，因此，在Linux上做了调整，喂狗这一部分
统一调用上面提供的系统守卫的一套函数接口，该接口在Linux上不再提供。
====================================================================*/
void   BrdSetWatchdogMode(u8 byMode)
{
	return;
}

/*====================================================================
函数名      : BrdFeedDog
功能        ：喂狗函数,1.6秒内至少调一次，否则系统会重启，调用该函数的任务的优先级必须很高，建议为最高优先级
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：无
返回值说明  ：无
说明：由于在VxWorks上由业务程序喂狗频繁出现问题，因此，在Linux上做了调整，喂狗这一部分
统一调用上面提供的系统守卫的一套函数接口，该接口在Linux上不再提供。
====================================================================*/
void   BrdFeedDog(void)
{
	return;
}

/*====================================================================
函数名      : BrdAlarmStateScan
功能        ：底层告警信息扫描
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：ptBrdAlarmState:返回的告警信息
返回值说明  ：打开返回TRUE；关闭返回FALSE。
====================================================================*/
STATUS BrdAlarmStateScan(TBrdAlarmState *ptBrdAlarmState)
{
	return OK;
}

/*====================================================================
函数名      : BrdSetFanState
功能        ：设置系统风扇运行状态
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：byRunState：模式
                #define BRD_FAN_RUN                  0       风扇转动
                #define BRD_FAN_STOP                 1       风扇停转
返回值说明  ：成功返回OK ；失败返回ERROR
====================================================================*/
STATUS BrdSetFanState(u8 byRunState)
{
	if ( BRD_FAN_RUN != byRunState && BRD_FAN_STOP != byRunState )
	{
		return ERROR;
	}
	return OK;
}

/*====================================================================
函数名      : BrdFanIsStopped
功能        ：查询指定风扇是否停转
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：无
返回值说明  ：TRUE=风扇停转 / FALSE=风扇正常
====================================================================*/
BOOL32   BrdFanIsStopped(u8 byFanId)
{
	return FALSE;
}

/*====================================================================
函数名      : BrdExtModuleIdentify
功能        ：终端外挂模块识别函数
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：无
返回值说明  ：外挂模块ID号：
            0为E1模块，1为4E1模块，2为V35DTE模块，3为V35DCE模块，4为网桥模块，5为2E1模块,
            6为KDV8000B模块, 0xc为DSC模块,目前MDSC也是这个ID号，下一版的MDSC将改变为0xd,0xe为HDSC模块，
            0xf 没有外挂模块
====================================================================*/
u8   BrdExtModuleIdentify(void)
{
	return 0xd; // mdsc;
}

/*====================================================================
函数名      : BrdVideoMatrixSet
功能        ：视频交换矩阵设置
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：注：选择的端口范围视矩阵板类型而定，16x16矩阵输入输出端口取值范围：1~16
            由于硬件面积无法全部使用，只使用了15个）；8x8取值范围：1~8。
            videoInSelect = 视频输入源选择： 1~16/8输入端口， 0xfb = 关相应的输出；
            0xfc = 开相应的输出，默认所有的输出口都是打开的，如果业务程序关闭了某
            个输出口，则必须先打开这个输出口，然后在设置交换（分两步做）
            videoOutSelect = 视频输出端口选择：1~16/8视频输出接口
            说明：在KDV8010A上，设备面板上的6个输入/数出口对应的输入/输出端口号是：2~7
返回值说明  ：无
====================================================================*/
void BrdVideoMatrixSet(u8 byInSelect, u8 byOutSelect)
{
	return;
}

/*====================================================================
函数名      : BrdTimeGet
功能        ：取系统时间（不会从RTC中获取时间）
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：ptGettm：存放返回时间的结构指针；tm为系统的数据结构
返回值说明  ：错误返回ERROR；成功返回OK
说明：业务程序对本接口返回的tm结构中的年和月必须进行了处理之后才能传给用户，
具体处理要求是：tm中的年要加上1900；tm中的月要将加上1
====================================================================*/
STATUS BrdTimeGet( struct tm* ptGettm )
{
	time_t time_c;
	time( &time_c );
	*ptGettm = *localtime( &time_c );
	return OK;
}

/*====================================================================
函数名      : BrdTimeSet
功能        ：设置系统时间，如果有RTC则同步设置
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：ptGettm：存放要设置的时间的结构指针；tm为系统的数据结构
返回值说明  ：错误返回ERROR；成功返回OK
说明：业务程序对tm结构中的年和月必须进行了处理之后才能传给该接口，
具体处理要求是：tm中的年要将用户设置的数值减去1900；tm中的月要将用户设置的数值减去1
====================================================================*/
STATUS BrdTimeSet( struct tm* ptGettm )
{
	return OK;
}

/*====================================================================
函数名      : BrdGetAlarmInput
功能        ：获取现场告警(KDM2400/2500专用)
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：byPort: 告警输入端口号
              pbyState: 存放告警输入值状态的指针（值定义：0:告警；1:正常）
返回值说明  ：错误返回ERROR；成功返回OK
====================================================================*/
STATUS BrdGetAlarmInput(u8 byPort, u8* pbyState)
{
	if( NULL == pbyState )
	{
		return ERROR;
	}
	return OK;
}

/*====================================================================
函数名      : BrdSetAlarmOutput
功能        ：输出现场告警
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：byPort: 告警输出端口号（0,1,2）
              pbyState: 0:产生报警;1:报警恢复
返回值说明  ：错误返回ERROR；成功返回OK
====================================================================*/
STATUS BrdSetAlarmOutput(u8 byPort, u8  byState)
{
	return OK;
}

/*====================================================================
函数名      : BrdGetSdramMaxSize
功能        ：取板子内存最大值,以MByte为单位
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：无
返回值说明  ：内存最大值,以MByte为单位
====================================================================*/
//u32 BrdGetSdramMaxSize(void)
//{
//	struct sysinfo info;
//	s32 nret = sysinfo( &info );
//	return info.totalram;
//}

/*====================================================================
函数名      : BrdGetAllDiskInfo
功能        ：获取flash分区信息
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：ptBrdAllDiskInfo ：指向TbrdAllDiskInfo结构的指针
返回值说明  ：错误返回ERROR；成功返回OK
====================================================================*/
STATUS BrdGetAllDiskInfo(TBrdAllDiskInfo *ptBrdAllDiskInfo)
{
	if( NULL == ptBrdAllDiskInfo )
	{ return ERROR; }
	return OK;
}

/*====================================================================
函数名      : BrdGetFullFileName
功能        ：转换文件名为带全路径的文件名
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：  byPutDiskId:盘的Id； 
                pchInFileName:输入的文件名 ，不带盘符；
                pchRtnFileName:返回的加上Id号盘符全路径的文件名
返回值说明  ：返回的加上Id号盘符的文件名的长度，以字节为单位，0为错误
====================================================================*/
u32 BrdGetFullFileName(u8 byPutDiskId, s8 *pchInFileName, s8 *pchRtnFileName)
{
	if( pchInFileName == pchRtnFileName )
	{
		return 0;
	}
	return OK;
}

/*====================================================================
函数名      : BrdCopyFile
功能        ：拷贝文件
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：  pchSrcFile：源文件，必须是全路经
                pchDesFile：目标文件，必须是全路经
返回值说明  ：成功返回OK ；失败返回ERROR
====================================================================*/
STATUS BrdCopyFile (s8 *pchSrcFile, s8 *pchDesFile)
{
	return OK;
}

/*====================================================================
函数名      : BrdBackupFile
功能        ：备份文件
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：  pchSrcFile：源文件，必须是全路经
                pchDesFile：目标文件，必须是全路经
返回值说明  ：成功返回OK ；失败返回ERROR
====================================================================*/
STATUS BrdBackupFile (s8 *pchSrcFile, s8 *pchDesFile)
{
	return OK;
}

/*====================================================================
函数名      : BrdRestoreFile 
功能        ：还原文件，如果目标文件存在且只读，将其强行覆盖
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：  pchSrcFile：源文件，必须是全路经
                pchDesFile：目标文件，必须是全路经
返回值说明  ：成功返回OK ；失败返回ERROR
====================================================================*/
STATUS BrdRestoreFile(s8 *pchSrcFile, s8 *pchDesFile)
{
	return OK;
}

/*====================================================================
函数名      : SysSetUsrAppParam
功能        ：单个用户程序启动参数设置，为了兼容vx上的接口
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：  pbyFileName:应用文件名
                byFileType :有两套宏定义，含义相同，为了兼容以前vx上
                #define LOCAL_UNCOMPRESS_FILE   0 本地未压缩的文件
                #define LOCAL_COMPRESS_FILE  1 本地压缩的文件
                #define REMOTE_UNCOMPRESS_FILE 2 远端未压缩的文件
                #define REMOTE_COMPRESS_FILE  3 远端压缩的文件
                #define LOCAL_ZIP_FILE  4 本地zip压缩包中的文件
                pAutoRunFunc:为了兼容以前vx上接口而设，本函数无效
返回值说明  ：成功返回OK ；失败返回ERROR
====================================================================*/
STATUS SysSetUsrAppParam(s8 *pbyFileName, u8 byFileType, s8 *pAutoRunFunc)
{
	return OK;
}

/*====================================================================
函数名      : SysSetMultiAppParam
功能        ：多个用户程序启动参数设置，Linux上新增接口
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：  ptAppLoadInf:指向应用配置信息的数据结构的指针
返回值说明  ：成功返回OK ；失败返回ERROR
====================================================================*/
STATUS SysSetMultiAppParam(TAppLoadInf *ptAppLoadInf)
{
	return OK;
}
/*====================================================================
函数名      : BrdGetMemInfo
功能        ：查询当前内存使用信息
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：  pdwByteFree: 存放当前空闲的内存大小数据的指针，字节为单位
                pdwByteAlloc: 存放当前已分配的内存大小数据的指针，字节为单位
返回值说明  ：成功返回OK ；失败返回ERROR
====================================================================*/
//STATUS BrdGetMemInfo(u32 *pdwByteFree, u32 *pdwByteAlloc)
//{
//	struct sysinfo info;
//	s32 nret = sysinfo( &info );
//	*pdwByteFree = info.freeram/(1024*1024);
//	*pdwByteAlloc = (info.totalram - info.freeram)/(1024*1024);
//	return (0 == nret) ? OK : ERROR;
//}
/*====================================================================
函数名      : SysGetIdlePercent
功能        ：查询当前cpu的空闲百分比
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：  无
返回值说明  ：当前cpu的空闲百分比
====================================================================*/
u8 SysGetIdlePercent(void)
{
	return 50;
}

/*====================================================================
函数名      : BrdGetDeviceInfo
功能        ：查询当前设备的CPU类型、主频、u-boot和内核版本信息的接口
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：  ptBrdDeviceInfo:指向TBrdDeviceInfo结构体的指针
返回值说明  ：OK/ERROR
====================================================================*/
STATUS BrdGetDeviceInfo (TBrdDeviceInfo* ptBrdDeviceInfo)
{
	if( NULL == ptBrdDeviceInfo )
		return ERROR;
	return OK;
}

/*====================================================================
函数名      : BrdAddUser
功能        ：增加系统用户
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：  pchUserName：要增加的登陆用户名
		        pchPassword：要增加的用户的登录密码
                tUserType：要增加的用户类型，见相关的结构体定义
返回值说明  ：OK/ERROR
====================================================================*/
STATUS BrdAddUser (s8* pchUserName,s8* pchPassword, TUserType tUserType)
{
	if( NULL == pchUserName )
		return ERROR;
	return OK;
}

/*====================================================================
函数名      : BrdDelUser
功能        ：删除系统用户
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：  pchUserName：要删除的登陆用户名
		        pchPassword：要删除的用户的登录密码
                tUserType：要删除的用户类型，见相关的结构体定义
返回值说明  ：OK/ERROR
====================================================================*/
STATUS BrdDelUser (s8* pchUserName,s8* pchPassword, TUserType tUserType)
{
	if( NULL == pchUserName )
		return ERROR;
	return OK;
}

/*====================================================================
函数名      : BrdUpdateAppFile
功能        ：将用户指定的单个文件更新到APP区(/usr/bin目录下)的接口
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：  pchSrcFile：要更新的源文件名，包含绝对路径，不支持“./”等相对路径符号
		        pchDstFile：要更新的目的文件名，包含绝对路径，不支持“./”等相对路径符号
返回值说明  ：OK/ERROR
示例：BrdUpdateAppFile（”/ramdisk/mp8000b”,”/usr/bin/mp8000b”）
	将/ramdisk/目录下的mp8000b程序更新到只读的APP区(/usr/bin)并命名为mp8000b
====================================================================*/
STATUS BrdUpdateAppFile (s8* pchSrcFile,s8* pchDstFile)
{
	return OK;
}

/*====================================================================
函数名      : BrdUpdateAppImage
功能        ：将用户指定的使用mkfs.jffs2制作的包含所有上层业务程序的Image文件更新到APP区(/usr/bin目录下)的接口
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：  pImageFile：要更新的Image文件名，包含绝对路径，不支持“./”等相对路径符号
返回值说明  ：OK/ERROR
示例：BrdUpdateAppImage（”/ramdisk/mdsc.jffs2”）
	将/ramdisk/目录下使用mkfs.jff2制作的的包含mdsc上所有的业务程序、配置文件的mdsc.jffs2镜像文件更新到只读的APP区(/usr/bin)
注意：调用完该函数后，设备必须重启新更新APP分区才能生效	
====================================================================*/
STATUS BrdUpdateAppImage (s8* pImageFile)
{
	return OK;
}

/*================================
函数名：BrdGetBSP15CapturePort
功能：BSP15图像采集端口查询
算法实现：（可选项）
引用全局变量：
输入参数说明： byBSP15ID:BSP-15的ID号(从0开始）
返回值说明： BSP15图像采集端口宏定义
==================================*/
u8  BrdGetBSP15CapturePort(u8 byBSP15ID)
{
	return 0;
}

/*================================
函数名：BrdGetSAA7114OutPort
功能：BSP15图像输出接口查询
算法实现：（可选项）
引用全局变量：
输入参数说明： byBSP15ID:BSP-15的ID号(从0开始）
返回值说明： SAA7114图像数据输出端口宏定义
==================================*/
u8  BrdGetSAA7114OutPort(u8 byBSP15ID)
{
	return 0;
}

/*================================
函数名：BrdGetCaptureChipType
功能：BSP15图像采集芯片查询
算法实现：（可选项）
引用全局变量：
输入参数说明： byBSP15ID:BSP-15的ID号(从0开始）
返回值说明： 图像采集芯片型号
==================================*/
u8  BrdGetCaptureChipType(u8 byBSP15ID)
{
	return 0;
}

/*================================
函数名：BrdGetAudCapChipType
功能：提供给codec上层查询BSP15音频采集芯片的类型
算法实现：（可选项）
引用全局变量：
输入参数说明： byBSP15ID:BSP-15的ID号(从0开始）
返回值说明： 音频采集芯片型号
==================================*/
u8  BrdGetAudCapChipType(u8 byBSP15ID)
{
	return 0;
}

/*================================
函数名：BrdBsp15GpdpIsUsed
功能：查询哪些BSP15的gpdp端口的互相连接
算法实现：（可选项）
引用全局变量：
输入参数说明：无
返回值说明： bsp15使用gpdp互联状况的掩码，每个bit位代表一个bsp15，0=不使用/1=使用，
低位为0号map， 最大支持32个map互联
==================================*/
u32 BrdBsp15GpdpIsUsed(void)
{
	return 0;
}

/*================================
函数名：BrdGetBSP15VGAConf
功能：获取指定bsp15的vga配置
算法实现：（可选项）
引用全局变量：
输入参数说明：无
返回值说明： byDevId：bsp15的设备号
低位为0号map， 见VGA配置宏定义
==================================*/
u8  BrdGetBSP15VGAConf(u8 byDevId)
{
	return 0;
}

/*================================
函数名：BrdStopVGACap
功能：停止采集vga(8083)数据(控制fpga)
算法实现：（可选项）
引用全局变量：
输入参数说明：无
返回值说明： 无
==================================*/
void BrdStopVGACap(void)
{
	return;
}

/*================================
函数名：BrdStartVGACap
功能：开始采集vga(8083)数据(控制fpga)
算法实现：（可选项）
引用全局变量：
输入参数说明：byMode:健上面的VGA采集模式宏定义
返回值说明： 无
==================================*/
void BrdStartVGACap(u8 byMode)
{
	return;
}

/*================================
函数名：BrdSetVGACapMode
功能：开始采集vga(8083)数据(控制fpga)
算法实现：（可选项）
引用全局变量：
输入参数说明：byMode:健上面的VGA采集模式宏定义
返回值说明： 无
==================================*/
void BrdSetVGACapMode (u8 byMode)
{
	return;
}

/*================================
函数名：BrdCloseVGA
功能：关闭并禁止vga(8083)采集芯片工作
算法实现：（可选项）
引用全局变量：
输入参数说明：无
返回值说明： 无
==================================*/
void BrdCloseVGA(void)
{
	return;
}

/*================================
函数名：BrdOpenVGA
功能：打开并开始vga(8083)采集芯片工作
算法实现：（可选项）
引用全局变量：
输入参数说明：无
返回值说明： 无
==================================*/
void BrdOpenVGA(void)
{
	return;
}

/*================================
函数名：BrdSetBsp15GPIOMode
功能：GPIO总线切换控制信号,仅对第11版kdv8010a有效
算法实现：（可选项）
引用全局变量：
输入参数说明：byMode:模式，见相关的宏定义
返回值说明： 无
==================================*/
void BrdSetBsp15GPIOMode(u8 byMode)
{
	return;
}

/*================================
函数名：BrdMapDevOpenPreInit
功能：在map启动时对相关硬件的初始化
算法实现：（可选项）
引用全局变量：
输入参数说明：byBSP15ID：bsp15的设备号
返回值说明：无
==================================*/
void BrdMapDevOpenPreInit(u8 byBSP15ID)
{
	return;
}

/*================================
函数名：BrdMapDevClosePreInit
功能：在map关闭时对相关硬件的复位等操作
算法实现：（可选项）
引用全局变量：
输入参数说明：byBSP15ID：bsp15的设备号
返回值说明：无
==================================*/
void BrdMapDevClosePreInit(u8 byBSP15ID)
{
	return;
}

/*====================================================================
函数名      : BrdAudioMatrixSet
功能        ：音频交换矩阵设置
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：注：byAudioInSelect = 输入源选择：0~6 = 7个输入接口，0号输入硬件内部
          已接至解码后的图像，对外屏蔽。0xb = 关相应的输出；0xc = 开相应的输出；
          byAudioOutSelect = 视频输出端口选择：0~6 = 7个视频输出接口，
          0号输出硬件内部，已接至编码数据源，对外屏蔽。其余保留。
返回值说明  ：无
====================================================================*/
void  BrdAudioMatrixSet(u8 byAudioInSelect, u8 byAdioOutSelect)
{
	return;
}

/*====================================================================
函数名      : BrdGetMatrixType
功能        ：矩阵板类型查询
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：无
返回值说明  ：矩阵板类型，具体如矩阵板型号宏定义
====================================================================*/
u8 BrdGetMatrixType(void)
{
	return 0;
}

/*====================================================================
函数名      : BrdGetPowerVoltage
功能        ：获取当前电压值，以毫伏为单位
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：dwSampleTimes 采样次数，0表示使用默认值8
返回值说明  ：成功返回电压值，以毫伏为单位；无效返回ERROR
====================================================================*/
s32 BrdGetPowerVoltage(u32 dwSampleTimes)
{
	return 0;
}

/*====================================================================
函数名      : BrdShutoffPower
功能        ：关闭电源供应
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：无
返回值说明  ：成功返回OK；失败返回ERROR
====================================================================*/
STATUS BrdShutoffPower(void)
{
	return OK;
}

/*====================================================================
函数名      : BrdGetSwitchSel
功能        ：获取开关选择的状态
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：无
返回值说明  ：0 or 1
====================================================================*/
u8 BrdGetSwitchSel(void)
{
	return 0;
}
/*================================
函数名：BrdGetEthParam
功能：读取以太网参数，含ip、mask、mac
算法实现：（可选项）
引用全局变量：
输入参数说明：  u8 byEthId --- 以太网ID号（0~x）;
                TBrdEthParam *ptBrdEthParam --- 存放以太网信息的结构指针。
返回值说明： 出错返回ERROR；成功返回OK。
==================================*/
STATUS BrdGetEthParam(u8 byEthId, TBrdEthParam *ptBrdEthParam)
{
	if( NULL == ptBrdEthParam )
		return ERROR;

	if ( !CreatCfgDebugFile() )
	{
		return ERROR;
	}

	s8 achProfileName[64];
	memset(achProfileName, 0, sizeof(achProfileName));
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILENAME_CONFIG);

	u32 dwIpNum = 0;
	s8 achEthEntry[8];
	memset( achEthEntry, 0, sizeof(achEthEntry) );
	s8 achEthIpNumEntry[16];
	memset( achEthIpNumEntry, 0, sizeof(achEthIpNumEntry) );
	s8 achEthIpAddrEntry[32];
	memset( achEthIpAddrEntry, 0, sizeof(achEthIpAddrEntry) );
	s8 achEthMacEntry[32];
	memset( achEthMacEntry, 0, sizeof(achEthMacEntry) );

	sprintf( achEthEntry, "%s%d", ENTRYPART_ETHERNET, byEthId );
	sprintf( achEthIpNumEntry, "%s%s", achEthEntry, ENTRYPART_IPNUM );

	BOOL32 bRet = ::GetRegKeyInt( achProfileName, SECTION_IpConfig, achEthIpNumEntry, 0, (s32*)&dwIpNum );
	if ( !bRet || 0 == dwIpNum )
	{
		return ERROR;
	}

	sprintf( achEthMacEntry, "%s%s", achEthEntry, ENTRYPART_MAC );
	sprintf( achEthIpAddrEntry, "%s%s0", achEthEntry, ENTRYPART_IPADDR );
	
	s8 achIpAddr[64];
	memset( achIpAddr, 0, sizeof(achIpAddr) );
	s8 achMac[18];
	memset( achMac, 0, sizeof(achMac) );

	bRet = ::GetRegKeyString( achProfileName, SECTION_IpConfig, achEthMacEntry, "00:00:00:00:00:00", achMac, sizeof(achMac) );
	if ( !bRet )
	{
		return ERROR;
	}
	// Mac addr
	if ( IsMacAddrInvalid(achMac, ':') )
	{
		if ( 0 != strlen(achMac) )
		{
			return ERROR;
		}
	}
    else
	{
		u8 byCount = 0;
		s8* pMacAddr = achMac;
		s8* pchToken = strtok( pMacAddr, ":" );
		while( NULL != pchToken)
		{
			BOOL32 bErr = FALSE;
			u8 byTmp = 0;
			s8 ch = 0;
			for (u8 byIndex = 0; byIndex < strlen(pchToken); byIndex ++ )
			{
				ch = pchToken[byIndex];
				if (ch >= '0' && ch <= '9' )
				{
					byTmp = (byTmp << 4) + (ch - '0');
				}
				else if (ch >= 'A' && ch <= 'F' )
				{
					byTmp =  (byTmp << 4) + (ch - 'A') + 10;
				}
				else if (ch >= 'a' && ch <= 'f' )
				{
					byTmp =  (byTmp << 4) + (ch - 'a') + 10;
				}
				else
				{
					bErr = TRUE;
					break;
				}
			}

			if (bErr)
			{
				break;
			}

			ptBrdEthParam->byMacAdrs[byCount] = byTmp;
			byCount ++;
			if (byCount == 6)
			{
				break;
			}

			pchToken = strtok( NULL, ":" );
		}

		if ( byCount != 6 )
		{
			return ERROR;
		}
	}		

	
	bRet = ::GetRegKeyString( achProfileName, SECTION_IpConfig, achEthIpAddrEntry, "127.0.0.1 255.255.255.0", achIpAddr, sizeof(achIpAddr) );
	if ( !bRet )
	{
		return ERROR;
	}

	s8 achSeps[] = " \t";        // 分隔符
    s8 *pchToken = NULL;

	// IpAddr
	pchToken = strtok( achIpAddr, achSeps );
    if (NULL == pchToken)
    {
        return ERROR;
    }
    else
    {
		ptBrdEthParam->dwIpAdrs = INET_ADDR(pchToken);
    }

	// IpMask
	pchToken = strtok( NULL, achSeps );
    if (NULL == pchToken)
    {
        ptBrdEthParam->dwIpMask = htonl(0xffffff00);
    }
    else
    {
		ptBrdEthParam->dwIpMask = INET_ADDR(pchToken);
    }
	
	return OK;
}

/*================================
函数名：BrdGetEthParamAll
功能：获取一个网口上所有的以太网参数，多个IP地址的情况
算法实现：（可选项）
引用全局变量：
输入参数说明：  u8 byEthId --- 以太网ID号（0~x）;
                BrdEthParamAll *ptBrdEthParamAll --- 存放以太网信息的结构指针。
返回值说明： 出错返回ERROR；成功返回OK。
==================================*/
STATUS BrdGetEthParamAll(u8 byEthId, TBrdEthParamAll *ptBrdEthParamAll)
{
	if( NULL == ptBrdEthParamAll )
        return ERROR;

	if ( !CreatCfgDebugFile() )
	{
		return ERROR;
	}

	s8 achProfileName[64];
	memset(achProfileName, 0, sizeof(achProfileName));
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILENAME_CONFIG);
    
    u32 dwIpNum = 0;
	s8 achEthEntry[8];
	memset( achEthEntry, 0, sizeof(achEthEntry) );
	s8 achEthIpNumEntry[16];
	memset( achEthIpNumEntry, 0, sizeof(achEthIpNumEntry) );
	s8 achEthIpAddrEntry[32];
	memset( achEthIpAddrEntry, 0, sizeof(achEthIpAddrEntry) );
	s8 achEthMacEntry[32];
	memset( achEthMacEntry, 0, sizeof(achEthMacEntry) );
	s8 achIpAddr[64];
	memset( achIpAddr, 0, sizeof(achIpAddr) );
	s8 achMac[18];
	memset( achMac, 0, sizeof(achMac) );
	

	sprintf( achEthEntry, "%s%d", ENTRYPART_ETHERNET, byEthId );
	sprintf( achEthIpNumEntry, "%s%s", achEthEntry, ENTRYPART_IPNUM );
	sprintf( achEthMacEntry, "%s%s", achEthEntry, ENTRYPART_MAC );

	BOOL32 bRet = ::GetRegKeyInt( achProfileName, SECTION_IpConfig, achEthIpNumEntry, 0, (s32*)&dwIpNum );
	if ( !bRet || 0 == dwIpNum )
	{
		return ERROR;
	}
	else
	{
		ptBrdEthParamAll->dwIpNum = dwIpNum;
	}

	// Mac addr
	u8 abyMacAddr[6];
	memset( abyMacAddr, 0, sizeof(abyMacAddr) );
	memset( achMac, 0, sizeof(achMac) );
	bRet = ::GetRegKeyString( achProfileName, SECTION_IpConfig, achEthMacEntry, "00:00:00:00:00:00", achMac, sizeof(achMac) );
	if ( !bRet )
	{
		return ERROR;
	}
	if ( IsMacAddrInvalid(achMac, ':') )
	{
		if ( 0 != strlen(achMac) )
		{
			return ERROR;
		}
	}
	else
	{
		s8* pchToken;
		u8 byCount = 0;
		s8* pMacAddr = achMac;
		pchToken = strtok( pMacAddr, ":" );
		while( NULL != pchToken)
		{
			BOOL32 bErr = FALSE;
			u8 byTmp = 0;
			s8 ch = 0;
			for (u8 byIndex = 0; byIndex < strlen(pchToken); byIndex ++ )
			{
				ch = pchToken[byIndex];
				if (ch >= '0' && ch <= '9' )
				{
					byTmp = (byTmp << 4) + (ch - '0');
				}
				else if (ch >= 'A' && ch <= 'F' )
				{
					byTmp =  (byTmp << 4) + (ch - 'A') + 10;
				}
				else if (ch >= 'a' && ch <= 'f' )
				{
					byTmp =  (byTmp << 4) + (ch - 'a') + 10;
				}
				else
				{
					bErr = TRUE;
					break;
				}
			}

			if (bErr)
			{
				break;
			}

			abyMacAddr[byCount] = byTmp;
			byCount ++;
			if (byCount == 6)
			{
				break;
			}

			pchToken = strtok( NULL, ":" );
		}

		if ( byCount != 6 )
		{
			return ERROR;
		}
	}		

	for ( u32 dwIndex = 0; dwIndex < dwIpNum; dwIndex++ )
	{	
		memset( achEthIpAddrEntry, 0, sizeof(achEthIpAddrEntry) );
		sprintf( achEthIpAddrEntry, "%s%s%d", achEthEntry, ENTRYPART_IPADDR, dwIndex );

		memset( achIpAddr, 0, sizeof(achIpAddr) );
		bRet = ::GetRegKeyString( achProfileName, SECTION_IpConfig, achEthIpAddrEntry, "127.0.0.1 255.255.255.0", achIpAddr, sizeof(achIpAddr) );
		if ( !bRet )
		{
			return ERROR;
		}

		s8 achSeps[] = " \t";        // 分隔符
		s8 *pchToken = NULL;

		// IpAddr
		pchToken = strtok( achIpAddr, achSeps );
		if (NULL == pchToken)
		{
			return ERROR;
		}
		else
		{
			ptBrdEthParamAll->atBrdEthParam[dwIndex].dwIpAdrs = INET_ADDR(pchToken);
		}

		// IpMask
		pchToken = strtok( NULL, achSeps );
		if (NULL == pchToken)
		{
			return ERROR;
		}
		else
		{
			ptBrdEthParamAll->atBrdEthParam[dwIndex].dwIpMask = INET_ADDR(pchToken);
		}

		memcpy( ptBrdEthParamAll->atBrdEthParam[dwIndex].byMacAdrs, abyMacAddr, sizeof(abyMacAddr) );
	}
    
	return OK;
}

/*=============================
函数名：BrdEthParam2Str
功能：将TBrdEthParam转换成配置文件中的字串
算法实现：（可选项）
引用全局变量：
输入参数说明：  s8 *achStr : 存放转换后的字串的数组
                TBrdEthParam tBrdEthParam : 需要转换的结构
返回值说明： 出错返回ERROR；成功返回OK。
=============================*/
BOOL32 BrdEthParam2Str( s8 *achIpAddrStr, u8 byIpStrLen, s8*achMacStr, u8 byMacStrLen, TBrdEthParam *ptBrdEthParam )
{
	if ( NULL == achIpAddrStr || NULL == achMacStr
		|| 0 == byIpStrLen || 0 == byMacStrLen )
	{
		return FALSE;
	}

	s8 achIpAddr[64];
	memset( achIpAddr, 0, sizeof(achIpAddr) );
	s8 achIp[17];
	memset( achIp, 0, sizeof(achIp) );
	s8 achMask[17];
	memset( achMask, 0, sizeof(achMask) );
	s8 achMacAddr[18];
	memset( achMacAddr, 0, sizeof(achMacAddr) );

	u32 dwIp = htonl(ptBrdEthParam->dwIpAdrs);
	sprintf( achIp, "%d.%d.%d.%d", (dwIp>>24)&0xFF, (dwIp>>16)&0xFF, (dwIp>>8)&0xFF, dwIp&0xFF );
	dwIp = htonl(ptBrdEthParam->dwIpMask);
	sprintf( achMask, "%d.%d.%d.%d", (dwIp>>24)&0xFF, (dwIp>>16)&0xFF, (dwIp>>8)&0xFF, dwIp&0xFF );
	u32 dwMacAddrSum = 0;
	for (u8 byIndex = 0; byIndex < 6; byIndex ++ )
	{
		dwMacAddrSum += ptBrdEthParam->byMacAdrs[byIndex];
	}
	if ( dwMacAddrSum != 0 )
	{
		s8 *pTemp = achMacAddr;
		for ( u8 byLop = 0; byLop < 6; byLop++ )
		{
			sprintf( pTemp, "%d%d", (ptBrdEthParam->byMacAdrs[byLop]>>4)&0x0F, ptBrdEthParam->byMacAdrs[byLop]&0x0F );
			pTemp += 2;
			if ( byLop < 5 )
			{
				sprintf( pTemp, ":" );
				pTemp++;
			}
		}
	}
	else
	{
		memset( achMacStr, 0, byMacStrLen );
	}

	sprintf( achIpAddr, "%s\t%s", achIp, achMask );
	strncpy( achIpAddrStr, achIpAddr, byIpStrLen );
	strncpy( achMacStr, achMacAddr, byMacStrLen );

	return TRUE;
}

/*================================
函数名：BrdSetEthParam
功能：设置主以太网参数(主IP和掩码)
算法实现：（可选项）
引用全局变量：
输入参数说明：  byEthId：以太网的编号（0 ~ X）X视不同板子而定；
                byIpOrMac：为设置命令选择(见BoardWrapper.h中相关的宏定义)

                ptBrdEthParam：指向存放设定值的TbrdEthParam数据结构的指针
返回值说明： 出错返回ERROR；成功返回OK。
==================================*/
STATUS BrdSetEthParam(u8 byEthId, u8 byIpOrMac, TBrdEthParam *ptBrdEthParam)
{
	if( NULL == ptBrdEthParam )
		return ERROR;

	if ( !CreatCfgDebugFile() )
	{
		return ERROR;
	}

	s8 achProfileName[64];
	memset(achProfileName, 0, sizeof(achProfileName));
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILENAME_CONFIG);

	s8 achEthEntry[8];
	memset( achEthEntry, 0, sizeof(achEthEntry) );
	s8 achEthIpNumEntry[16];
	memset( achEthIpNumEntry, 0, sizeof(achEthIpNumEntry) );
	s8 achEthIpAddrEntry[32];
	memset( achEthIpAddrEntry, 0, sizeof(achEthIpAddrEntry) );
	s8 achEthMacEntry[32];
	memset( achEthMacEntry, 0, sizeof(achEthMacEntry) );

	sprintf( achEthEntry, "%s%d", ENTRYPART_ETHERNET, byEthId );
	sprintf( achEthIpNumEntry, "%s%s", achEthEntry, ENTRYPART_IPNUM );
	sprintf( achEthIpAddrEntry, "%s%s0", achEthEntry, ENTRYPART_IPADDR );
	sprintf( achEthMacEntry, "%s%s", achEthEntry, ENTRYPART_MAC );

	u32 dwIpNum = 0;
	BOOL32 bRet = ::GetRegKeyInt( achProfileName, SECTION_IpConfig, achEthIpNumEntry, 0, (s32*)&dwIpNum );
	if ( !bRet )
	{
		return ERROR;
	}

	// 取当前的Param进行修改
	TBrdEthParam tCurBrdEthParam;
	memset( &tCurBrdEthParam, 0, sizeof(tCurBrdEthParam) );
	if ( dwIpNum > 0 && ERROR == BrdGetEthParam( byEthId, &tCurBrdEthParam ) )
	{
		return ERROR;
	}
	
	switch( byIpOrMac )
	{
	case Brd_SET_IP_AND_MASK:
		tCurBrdEthParam.dwIpAdrs = ptBrdEthParam->dwIpAdrs;
		tCurBrdEthParam.dwIpMask = ptBrdEthParam->dwIpMask;
		break;
	case Brd_SET_MAC_ADDR:
		memcpy( tCurBrdEthParam.byMacAdrs, ptBrdEthParam->byMacAdrs, sizeof(tCurBrdEthParam.byMacAdrs) );
		break;
	case Brd_SET_ETH_ALL_PARAM:
		tCurBrdEthParam.dwIpAdrs = ptBrdEthParam->dwIpAdrs;
		tCurBrdEthParam.dwIpMask = ptBrdEthParam->dwIpMask;
		memcpy( tCurBrdEthParam.byMacAdrs, ptBrdEthParam->byMacAdrs, sizeof(tCurBrdEthParam.byMacAdrs) );
		break;
	default:
		return ERROR;
		break;
	}

	s8 achIpAddr[64];
	memset( achIpAddr, 0, sizeof(achIpAddr) );
	s8 achMac[18];
	memset( achMac, 0, sizeof(achMac) );

	BrdEthParam2Str( achIpAddr, sizeof(achIpAddr), achMac, sizeof(achMac), &tCurBrdEthParam );

	bRet = ::SetRegKeyString( achProfileName, SECTION_IpConfig, achEthIpAddrEntry, achIpAddr );
	if ( !bRet )
	{
		return ERROR;
	}
	bRet = ::SetRegKeyString( achProfileName, SECTION_IpConfig, achEthMacEntry, achMac );
	if ( !bRet )
	{
		return ERROR;
	}

	if ( 0 == dwIpNum )
	{
		dwIpNum++;
		bRet = ::SetRegKeyInt( achProfileName, SECTION_IpConfig, achEthIpNumEntry, dwIpNum );
		if ( !bRet )
		{
			return ERROR;
		}
	}
	
	return OK;
}

/*================================
函数名：BrdSetSecondEthParam
功能：设置从以太网参数(从IP和掩码)
算法实现：（可选项）
引用全局变量：
输入参数说明：  byEthId：以太网的编号（0 ~ X）X视不同板子而定；
                byIpOrMac：为设置命令选择(见BoardWrapper.h中相关的宏定义)
                ptBrdEthParam：指向存放设定值的TbrdEthParam数据结构的指针
返回值说明： 出错返回ERROR；成功返回OK。
注意：在Linux下，一个网口可以配置多个IP地址，一个网口最多可以配置5个IP地址。
==================================*/
STATUS BrdSetSecondEthParam(u8 byEthId, u8 byIpOrMac, TBrdEthParam *ptBrdEthParam)
{
	if( NULL == ptBrdEthParam )
		return ERROR;

	if ( !CreatCfgDebugFile() )
	{
		return ERROR;
	}

	s8 achProfileName[64];
	memset(achProfileName, 0, sizeof(achProfileName));
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILENAME_CONFIG);

	s8 achEthEntry[8];
	memset( achEthEntry, 0, sizeof(achEthEntry) );
	s8 achEthIpNumEntry[16];
	memset( achEthIpNumEntry, 0, sizeof(achEthIpNumEntry) );
	s8 achEthIpAddrEntry[32];
	memset( achEthIpAddrEntry, 0, sizeof(achEthIpAddrEntry) );
	s8 achIpAddr[64];
	memset( achIpAddr, 0, sizeof(achIpAddr) );
	s8 achMac[18];
	memset( achMac, 0, sizeof(achMac) );

	sprintf( achEthEntry, "%s%d", ENTRYPART_ETHERNET, byEthId );
	sprintf( achEthIpNumEntry, "%s%s", achEthEntry, ENTRYPART_IPNUM );
	
	// 取当前IP配置
	TBrdEthParamAll tBrdEthParamAll;
	memset( &tBrdEthParamAll, 0, sizeof(tBrdEthParamAll) );
	if ( ERROR == BrdGetEthParamAll( byEthId, &tBrdEthParamAll ) )
	{
		return ERROR;
	}
	if ( 0 == tBrdEthParamAll.dwIpNum )
	{
		return ERROR;
	}

	BOOL32 bRet;
	if ( Brd_DEL_SEC_IP_AND_MASK == byIpOrMac )
	{
		// 删除IpConfig段
		FILE *stream = fopen( achProfileName, "rb+" );
		if( stream == NULL )
		{
			return ERROR;
		}
		s32 nOffset = DelSection( stream, SECTION_IpConfig );
		fclose( stream );
		if ( nOffset == -1 )
		{
			return ERROR;
		}
		else
		{
			if ( !TrimCfgFile( achProfileName ) )
			{
				return ERROR;
			}
		}

		// 删掉
		u32 dwLop = 0;
		for ( dwLop = 1; dwLop < tBrdEthParamAll.dwIpNum; dwLop++ )
		{
			if ( ptBrdEthParam->dwIpAdrs == tBrdEthParamAll.atBrdEthParam[dwLop].dwIpAdrs 
				&& ptBrdEthParam->dwIpMask == tBrdEthParamAll.atBrdEthParam[dwLop].dwIpMask )
			{
				memset( &tBrdEthParamAll.atBrdEthParam[dwLop], 0, sizeof(tBrdEthParamAll.atBrdEthParam[dwLop]) );
				break;
			}
		}
		if ( dwLop == tBrdEthParamAll.dwIpNum )
		{
			return ERROR;
		}

		if ( ERROR == BrdSetEthParam(byEthId, Brd_SET_IP_AND_MASK, &tBrdEthParamAll.atBrdEthParam[0]) )
		{
			return ERROR;
		}
		u32 dwSecIdx = 1;
		for ( dwLop = 1; dwLop < tBrdEthParamAll.dwIpNum; dwLop++ )
		{			
			if ( 0 == tBrdEthParamAll.atBrdEthParam[dwLop].dwIpAdrs )
			{
				continue;
			}
			memset( achIpAddr, 0, sizeof(achIpAddr) );
			memset( achMac, 0, sizeof(achMac) );
			BrdEthParam2Str( achIpAddr, sizeof(achIpAddr), achMac, sizeof(achMac), &tBrdEthParamAll.atBrdEthParam[dwLop] );

			memset( achEthIpAddrEntry, 0, sizeof(achEthIpAddrEntry) );
			sprintf( achEthIpAddrEntry, "%s%s%d", achEthEntry, ENTRYPART_IPADDR, dwSecIdx );

			bRet = ::SetRegKeyString( achProfileName, SECTION_IpConfig, achEthIpAddrEntry, achIpAddr );
			if ( !bRet )
			{
				return ERROR;
			}
			dwSecIdx++;
		}
		tBrdEthParamAll.dwIpNum--;
		bRet = ::SetRegKeyInt( achProfileName, SECTION_IpConfig, achEthIpNumEntry, tBrdEthParamAll.dwIpNum );
		if ( !bRet )
		{
			return ERROR;
		}
	} // if ( Brd_DEL_SEC_IP_AND_MASK == byIpOrMac )

	if ( Brd_SET_SEC_IP_AND_MASK == byIpOrMac )
	{
		u32 dwLop = 0;
		for ( dwLop = 1; dwLop < tBrdEthParamAll.dwIpNum; dwLop++ )
		{
			if ( ptBrdEthParam->dwIpAdrs == tBrdEthParamAll.atBrdEthParam[dwLop].dwIpAdrs 
				|| (ptBrdEthParam->dwIpAdrs&ptBrdEthParam->dwIpMask) == (tBrdEthParamAll.atBrdEthParam[dwLop].dwIpAdrs&ptBrdEthParam->dwIpMask)
				|| (ptBrdEthParam->dwIpAdrs&tBrdEthParamAll.atBrdEthParam[dwLop].dwIpMask) == (tBrdEthParamAll.atBrdEthParam[dwLop].dwIpAdrs&tBrdEthParamAll.atBrdEthParam[dwLop].dwIpMask)  
			   )
			{
				return ERROR;
			}
		}
		memset( achIpAddr, 0, sizeof(achIpAddr) );
		memset( achMac, 0, sizeof(achMac) );
		BrdEthParam2Str( achIpAddr, sizeof(achIpAddr), achMac, sizeof(achMac), ptBrdEthParam );

		memset( achEthIpAddrEntry, 0, sizeof(achEthIpAddrEntry) );
		sprintf( achEthIpAddrEntry, "%s%s%d", achEthEntry, ENTRYPART_IPADDR, tBrdEthParamAll.dwIpNum );

		BOOL32 bRet = ::SetRegKeyString( achProfileName, SECTION_IpConfig, achEthIpAddrEntry, achIpAddr );
		if ( !bRet )
		{
			return ERROR;
		}
		tBrdEthParamAll.dwIpNum++;
		bRet = ::SetRegKeyInt( achProfileName, SECTION_IpConfig, achEthIpNumEntry, tBrdEthParamAll.dwIpNum );
		if ( !bRet )
		{
			return ERROR;
		}
	} // if ( Brd_SET_SEC_IP_AND_MASK == byIpOrMac )

	if ( Brd_SET_SEC_ETH_ALL_PARAM == byIpOrMac )
	{
		u32 dwLop = 0;
		for ( dwLop = 1; dwLop < tBrdEthParamAll.dwIpNum; dwLop++ )
		{
			if ( ptBrdEthParam->dwIpAdrs == tBrdEthParamAll.atBrdEthParam[dwLop].dwIpAdrs 
				|| (ptBrdEthParam->dwIpAdrs&ptBrdEthParam->dwIpMask) == (tBrdEthParamAll.atBrdEthParam[dwLop].dwIpAdrs&ptBrdEthParam->dwIpMask)
				|| (ptBrdEthParam->dwIpAdrs&tBrdEthParamAll.atBrdEthParam[dwLop].dwIpMask) 
					== (tBrdEthParamAll.atBrdEthParam[dwLop].dwIpAdrs&tBrdEthParamAll.atBrdEthParam[dwLop].dwIpMask)  
			   )
			{
				return ERROR;
			}
		}
		memset( achIpAddr, 0, sizeof(achIpAddr) );
		memset( achMac, 0, sizeof(achMac) );
		BrdEthParam2Str( achIpAddr, sizeof(achIpAddr), achMac, sizeof(achMac), ptBrdEthParam );

		memset( achEthIpAddrEntry, 0, sizeof(achEthIpAddrEntry) );
		sprintf( achEthIpAddrEntry, "%s%s%d", achEthEntry, ENTRYPART_IPADDR, tBrdEthParamAll.dwIpNum );

		BOOL32 bRet = ::SetRegKeyString( achProfileName, SECTION_IpConfig, achEthIpAddrEntry, achIpAddr );
		if ( !bRet )
		{
			return ERROR;
		}
		tBrdEthParamAll.dwIpNum++;
		bRet = ::SetRegKeyInt( achProfileName, SECTION_IpConfig, achEthIpNumEntry, tBrdEthParamAll.dwIpNum );
		if ( !bRet )
		{
			return ERROR;
		}
	}
	
	return OK;
}

/*================================
函数名：BrdDelEthParam
功能：删除指定的以太网接口
算法实现：（可选项）
引用全局变量：
输入参数说明：  byEthId：以太网的编号（0 ~ X）X视不同板子而定；
返回值说明： 出错返回ERROR；成功返回OK。
注意：在Linux下，一个网口可以配置多个IP地址，该接口会删除该接口上的所有IP地址。
==================================*/
STATUS BrdDelEthParam(u8 byEthId)
{
	if ( !CreatCfgDebugFile() )
	{
		return ERROR;
	}
	
	if ( byEthId > 1 )
	{
		return ERROR;
	}

	s8 achProfileName[64];
	memset(achProfileName, 0, sizeof(achProfileName));
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILENAME_CONFIG);

	s8 achEthEntry[8];
	memset( achEthEntry, 0, sizeof(achEthEntry) );
	s8 achEthIpNumEntry[16];
	memset( achEthIpNumEntry, 0, sizeof(achEthIpNumEntry) );
	s8 achEthIpAddrEntry[32];
	memset( achEthIpAddrEntry, 0, sizeof(achEthIpAddrEntry) );
	s8 achIpAddr[64];
	memset( achIpAddr, 0, sizeof(achIpAddr) );
	s8 achMac[18];
	memset( achMac, 0, sizeof(achMac) );

	sprintf( achEthEntry, "%s%d", ENTRYPART_ETHERNET, byEthId );
	sprintf( achEthIpNumEntry, "%s%s", achEthEntry, ENTRYPART_IPNUM );

	s8 achEthMacEntry[32];
	memset( achEthMacEntry, 0, sizeof(achEthMacEntry) );
	sprintf( achEthMacEntry, "%s%s", achEthEntry, ENTRYPART_MAC );

	// 取当前IP配置
	TBrdEthParamAll tBrdEthParamAll;
	memset( &tBrdEthParamAll, 0, sizeof(tBrdEthParamAll) );
	if ( ERROR == BrdGetEthParamAll( byEthId, &tBrdEthParamAll ) )
	{
		return ERROR;
	}
	if ( 0 == tBrdEthParamAll.dwIpNum )
	{
		return OK;
	}

	// 删除IpConfig段
	FILE *stream = fopen( achProfileName, "rb+" );
	if( stream == NULL )
	{
		return ERROR;
	}
	s32 nOffset = DelSection( stream, SECTION_IpConfig );
	fclose( stream );
	if ( nOffset == -1 )
	{
		return ERROR;
	}
	else
	{
		if ( !TrimCfgFile( achProfileName ) )
		{
			return ERROR;
		}
	}
	
	u16 wSum = 0;
	BOOL32 bRet;
	for ( u8 byIdx = 0; byIdx < 6; byIdx++ )
	{
		wSum += tBrdEthParamAll.atBrdEthParam[0].byMacAdrs[byIdx];
	}
	if ( wSum != 0 )
	{
		memset( achIpAddr, 0, sizeof(achIpAddr) );
		memset( achMac, 0, sizeof(achMac) );
		if ( BrdEthParam2Str( achIpAddr, sizeof(achIpAddr), achMac, sizeof(achMac), &tBrdEthParamAll.atBrdEthParam[0] ) )
		{
			bRet = ::SetRegKeyString( achProfileName, SECTION_IpConfig, achEthMacEntry, achMac );
			if ( !bRet )
			{
				return ERROR;
			}
		}
		else
		{
			return ERROR;
		}
	}

	bRet = ::SetRegKeyInt( achProfileName, SECTION_IpConfig, achEthIpNumEntry, 0 );
	if ( !bRet )
	{
		return ERROR;
	}

	return OK;
}

/*================================
函数名：BrdGetAllIpRoute
功能：读取所有路由信息
算法实现：（可选项）
引用全局变量：
输入参数说明：ptBrdAllIpRouteInfo：指向存放返回路由信息的TBrdAllIpRouteInfo结构指针
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdGetAllIpRoute(TBrdAllIpRouteInfo *ptBrdAllIpRouteInfo)
{
	if ( !CreatCfgDebugFile() )
	{
		return ERROR;
	}

	s8 achProfileName[64];
	memset(achProfileName, 0, sizeof(achProfileName));
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILENAME_CONFIG);

	u32 dwIpRouteNum = 0;
	BOOL32 bRet = ::GetRegKeyInt( achProfileName, SECTION_IpRoute, KEY_IpRouteNum, 0, (s32*)&dwIpRouteNum );
	if ( !bRet )
	{
		ptBrdAllIpRouteInfo->dwIpRouteNum = 0;
		return ERROR;
	}

	if ( ptBrdAllIpRouteInfo->dwIpRouteNum == 0 )
	{
		return OK;
	}

	ptBrdAllIpRouteInfo->dwIpRouteNum = dwIpRouteNum;
	
	s8 achIpRouteEntry[32];
	s8 achIpRoute[64];

	u8 byIdx = 0;
	for ( byIdx = 0; byIdx < dwIpRouteNum; byIdx++ )
	{
		memset( achIpRouteEntry, 0, sizeof(achIpRouteEntry) );
		sprintf( achIpRouteEntry, "%s%d", ENTRYPART_IpRoute, byIdx );

		memset( achIpRoute, 0, sizeof(achIpRoute) );
		bRet = ::GetRegKeyString( achProfileName, SECTION_IpRoute, achIpRouteEntry, "ReadError", achIpRoute, sizeof(achIpRoute) );
		if ( !bRet || strcmp( "ReadError", achIpRoute) == 0 )
		{
			ptBrdAllIpRouteInfo->dwIpRouteNum = 0;
			return ERROR;
		}

		s8 achSeps[] = " \t";        // 分隔符
		s8 *pchToken = NULL;

		// IpNet
		pchToken = strtok( achIpRoute, achSeps );
		if (NULL == pchToken)
		{
			ptBrdAllIpRouteInfo->dwIpRouteNum = 0;
			return ERROR;
		}
		else
		{
			ptBrdAllIpRouteInfo->tBrdIpRouteParam[byIdx].dwDesIpNet = INET_ADDR(pchToken);
		}

		// IpMask
		pchToken = strtok( NULL, achSeps );
		if (NULL == pchToken)
		{
			ptBrdAllIpRouteInfo->dwIpRouteNum = 0;
			return ERROR;
		}
		else
		{
			ptBrdAllIpRouteInfo->tBrdIpRouteParam[byIdx].dwDesIpMask = INET_ADDR(pchToken);
		}

		// GWIp
		pchToken = strtok( NULL, achSeps );
		if ( NULL == pchToken )
		{
			ptBrdAllIpRouteInfo->dwIpRouteNum = 0;
			return ERROR;
		}
		else
		{
			ptBrdAllIpRouteInfo->tBrdIpRouteParam[byIdx].dwGwIpAdrs = INET_ADDR(pchToken);
		}

		// RoutePri
		pchToken = strtok( NULL, achSeps );
		if ( NULL == pchToken )
		{
			ptBrdAllIpRouteInfo->tBrdIpRouteParam[byIdx].dwRoutePri = 0;
		}
		else
		{
			ptBrdAllIpRouteInfo->tBrdIpRouteParam[byIdx].dwRoutePri = atol(pchToken);
		}
	}

	return OK;
}

/*================================
函数名：BrdAddOneIpRoute
功能：增加一条路
算法实现：（可选项）
引用全局变量：
输入参数说明：ptBrdIpRouteParam：指向存放路由信息的TBrdIpRouteParam结构指针
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdAddOneIpRoute(TBrdIpRouteParam *ptBrdIpRouteParam)
{
	if ( !CreatCfgDebugFile() )
	{
		return ERROR;
	}

	s8 achProfileName[64];
	memset(achProfileName, 0, sizeof(achProfileName));
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILENAME_CONFIG);

	TBrdAllIpRouteInfo tBrdAllIpRouteInfo;
	if ( ERROR == BrdGetAllIpRoute(&tBrdAllIpRouteInfo) )
	{
		return ERROR;
	}

	u32 dwIdx, dwIpRouteNum;
	dwIpRouteNum = tBrdAllIpRouteInfo.dwIpRouteNum;
	for ( dwIdx = 0; dwIdx < dwIpRouteNum; dwIdx++ )
	{
		if ( ptBrdIpRouteParam->dwDesIpNet == tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwDesIpNet 
			&& ptBrdIpRouteParam->dwDesIpMask == tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwDesIpMask
			&& ptBrdIpRouteParam->dwGwIpAdrs == tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwGwIpAdrs
		   )
		{
			return OK;	
		}
	}

	// 增加路由
	s8 achIpRouteEntry[32];
	s8 achIpRoute[64];

	memset( achIpRouteEntry, 0, sizeof(achIpRouteEntry) );
	sprintf( achIpRouteEntry, "%s%d", ENTRYPART_IpRoute, dwIpRouteNum+1 );

	memset( achIpRoute, 0, sizeof(achIpRoute) );
	u32 dwIpNet = ntohl(ptBrdIpRouteParam->dwDesIpNet);
	u32 dwIpMask = ntohl(ptBrdIpRouteParam->dwDesIpMask);
	u32 dwGWAddr = ntohl(ptBrdIpRouteParam->dwGwIpAdrs);
	u32 dwGWPeri = ptBrdIpRouteParam->dwRoutePri;
	sprintf( achIpRoute, "%s.%s.%s.%s\t%s.%s.%s.%s\t%s.%s.%s.%s\t%d",
				(dwIpNet>>24)&0xff, (dwIpNet>>16)&0xff, (dwIpNet>>8)&0xff, dwIpNet&0xff,
				(dwIpMask>>24)&0xff, (dwIpMask>>16)&0xff, (dwIpMask>>8)&0xff, dwIpMask&0xff,
				(dwGWAddr>>24)&0xff, (dwGWAddr>>16)&0xff, (dwGWAddr>>8)&0xff, dwGWAddr&0xff,
				dwGWPeri );
	BOOL32 bRet = ::SetRegKeyString( achProfileName, SECTION_IpRoute, achIpRouteEntry, achIpRoute );
	if ( !bRet )
	{
		return ERROR;
	}

	dwIpRouteNum++;

	bRet = ::SetRegKeyInt( achProfileName, SECTION_IpRoute, KEY_IpRouteNum, dwIpRouteNum );
	if ( !bRet )
	{
		return ERROR;
	}

	return OK;
}

/*================================
函数名：BrdDelOneIpRoute
功能：删除一条路
算法实现：（可选项）
引用全局变量：
输入参数说明：ptBrdIpRouteParam：指向存放路由信息的TBrdIpRouteParam结构指针
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdDelOneIpRoute(TBrdIpRouteParam *ptBrdIpRouteParam)
{
	if ( !CreatCfgDebugFile() )
	{
		return ERROR;
	}

	s8 achProfileName[64];
	memset(achProfileName, 0, sizeof(achProfileName));
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILENAME_CONFIG);

	TBrdAllIpRouteInfo tBrdAllIpRouteInfo;
	if ( ERROR == BrdGetAllIpRoute(&tBrdAllIpRouteInfo) )
	{
		return ERROR;
	}

	u32 dwIdx, dwIpRouteNum;
	dwIpRouteNum = tBrdAllIpRouteInfo.dwIpRouteNum;
	for ( dwIdx = 0; dwIdx < dwIpRouteNum; dwIdx++ )
	{
		if ( ptBrdIpRouteParam->dwDesIpNet == tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwDesIpNet 
			&& ptBrdIpRouteParam->dwDesIpMask == tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwDesIpMask
			&& ptBrdIpRouteParam->dwGwIpAdrs == tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwGwIpAdrs
		   )
		{
			memset( &tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx], 0, sizeof(TBrdIpRouteParam) );
			break;	
		}
	}

	if ( dwIdx == dwIpRouteNum )
	{
		return ERROR;
	}

	// 删除IpRoute段
	FILE *stream = fopen( achProfileName, "rb+" );
	if( stream == NULL )
	{
		return ERROR;
	}
	s32 nOffset = DelSection( stream, SECTION_IpRoute );
	fclose( stream );
	if ( nOffset == -1 )
	{
		return ERROR;
	}
	else
	{
		if ( !TrimCfgFile( achProfileName ) )
		{
			return ERROR;
		}
	}

	for ( dwIdx = 0; dwIdx < dwIpRouteNum; dwIdx++ )
	{
		if ( tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwDesIpNet != 0 
			|| tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwDesIpMask != 0
			|| tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwGwIpAdrs != 0 )
		{
			if ( ERROR == BrdAddOneIpRoute( &tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx] ) )
			{
				return ERROR;
			}		
		}
	}
	return OK;
}

/*================================
函数名：BrdGetDefGateway
功能：获取默认网关ip
算法实现：（可选项）
引用全局变量：
输入参数说明： 无
返回值说明： 错误返回0；成功返回长整型的ip地址，网络序
==================================*/
u32 BrdGetDefGateway(void)
{
	if ( !CreatCfgDebugFile() )
	{
		return ERROR;
	}
	
	TBrdAllIpRouteInfo tBrdAllIpRouteInfo;
	if ( ERROR == BrdGetAllIpRoute(&tBrdAllIpRouteInfo) )
	{
		return 0;
	}

	u32 dwIpRouteNum = tBrdAllIpRouteInfo.dwIpRouteNum;

	u8 dwIdx = 0;
	u32 dwGWIpAdrs = 0;
	s32 nRoutePeri = -1;
	for ( dwIdx = 0; dwIdx < dwIpRouteNum; dwIdx++ )
	{
		if ( 0 == tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwDesIpNet
			&& 0 == tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwDesIpMask
			&& 0 != tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwGwIpAdrs )
		{
			if ( (s32)tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwRoutePri > nRoutePeri )
			{
				dwGWIpAdrs = tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwGwIpAdrs;
				nRoutePeri = tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwRoutePri;
			}
		}
	}

	return dwGWIpAdrs;
}

/*================================
函数名：BrdDelDefGateway
功能：删除默认网关ip
算法实现：（可选项）
引用全局变量：
输入参数说明： 无
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdDelDefGateway(void)
{
	if ( !CreatCfgDebugFile() )
	{
		return ERROR;
	}

	TBrdAllIpRouteInfo tBrdAllIpRouteInfo;
	if ( ERROR == BrdGetAllIpRoute(&tBrdAllIpRouteInfo) )
	{
		return OK;
	}

	u32 dwIpRouteNum = tBrdAllIpRouteInfo.dwIpRouteNum;

	u8 dwIdx = 0;
	u32 dwGWIpAdrs = 0;
	s32 nRoutePeri = -1;
	for ( dwIdx = 0; dwIdx < dwIpRouteNum; dwIdx++ )
	{
		if ( 0 == tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwDesIpNet
			&& 0 == tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwDesIpMask
			&& 0 != tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx].dwGwIpAdrs )
		{
			if ( ERROR == BrdDelOneIpRoute(&tBrdAllIpRouteInfo.tBrdIpRouteParam[dwIdx]) )
			{
				return ERROR;
			}
		}
	}

	return OK;
}

/*================================
函数名：BrdSetDefGateway
功能：设置默认网关ip
算法实现：（可选项）
引用全局变量：
输入参数说明：  dwIpAdrs：默认网关ip地址，长整型，网络序
返回值说明： 出错返回ERROR；成功返回OK。
==================================*/
STATUS BrdSetDefGateway(u32 dwIpAdrs)
{
	if ( !CreatCfgDebugFile() )
	{
		return ERROR;
	}

	TBrdIpRouteParam tBrdIpRouteParam;
	tBrdIpRouteParam.dwDesIpMask = 0;
	tBrdIpRouteParam.dwDesIpNet = 0;
	tBrdIpRouteParam.dwGwIpAdrs = dwIpAdrs;
	if ( ERROR == BrdAddOneIpRoute( &tBrdIpRouteParam ) )
	{
		return ERROR;
	}

	return OK;
}
/*================================
函数名：BrdGetNextHopIpAddr
功能：获取通往指定ip的第一跳路由ip地址
算法实现：（可选项）
引用全局变量：
输入参数说明：dwDstIpAddr：目的ip地址，长整型，网络序
		    dwDstMask：目的掩码，长整型，网络序
返回值说明： 错误返回0；成功返回第一跳路由ip地址，长整型
==================================*/
u32 BrdGetNextHopIpAddr(u32 dwDstIpAddr,u32 dwDstMask)
{
	return OK;
}

/*================================
函数名：BrdGetRouteWayBandwidth
功能：读取俄E1捆绑线路的带宽值，以Kbps为单位
算法实现：（可选项）
引用全局变量：
输入参数说明：无
返回值说明： 线路带宽值，以Kbps为单位
==================================*/
u32 BrdGetRouteWayBandwidth(void)
{
	return OK;
}





/*================================
函数名：BrdChkOneIpStatus
功能：检查指定ip在配置中的状态
算法实现：（可选项）
引用全局变量：
输入参数说明：dwIpAdrs：待检查的ip地址，网络序
返回值说明： 返回指定ip在配置中的状态：
#define IP_SET_AND_UP	        1  ---address set and up 
#define IP_SET_AND_DOWN	        2  ---address set and down
#define IP_NOT_SET	            3  ---address not set
==================================*/
u32 BrdChkOneIpStatus(u32 dwIpAdrs)
{
	return IP_SET_AND_UP;
}

/*================================
函数名：BrdSaveNipConfig
功能：保存nip的网络配置文件
算法实现：（可选项）
引用全局变量：
输入参数说明：无
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdSaveNipConfig(void)
{
	return OK;
}

/*================================
函数名：BrdIpConflictCallBackReg
功能：注册ip地址冲突时回调函数,当设备的IP地址和外部设备冲突时，会调用该函数通知业务程序
算法实现：（可选项）
引用全局变量：
输入参数说明：ptFunc：执行业务程序注册的回调函数指针
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdIpConflictCallBackReg (TIpConflictCallBack  ptFunc)
{
	return OK;
}

/*================================
函数名：BrdIpOnceConflicted
功能：查询系统是否发生过ip地址冲突
算法实现：（可选项）
引用全局变量：
输入参数说明：无
返回值说明： TRUE or FALSE
==================================*/
BOOL32 BrdIpOnceConflicted (void)
{
	return OK;
}

/*================================
函数名：BrdPing
功能：以太网ping接口
算法实现：（可选项）
引用全局变量：
输入参数说明：pchDestIP:Ping的目的IP地址
              ptPingOpt：Ping的参数结构体指针
              nUserID:用户ID标志，用户区分不同的用户调用
              ptCallBackFunc:Ping结果的回调函数
返回值说明： TRUE--传入的参数合法，该函数调用成功，但是否ping通需要用户注册的回调函数来判断
             FALSE--传入的参数非法，该函数调用失败
==================================*/
BOOL32 BrdPing(s8* pchDestIP,TPingOpt* ptPingOpt,s32 nUserID,TPingCallBack ptCallBackFunc)
{
	return TRUE;
}

/*================================
函数名：BrdGetEthActLnkStat
功能：真实的以太网连接状态检测
算法实现：（可选项）
引用全局变量：
输入参数说明：byEthID：要查询的以太网索引号
返回值说明： 0=down/1=up
==================================*/
u8 BrdGetEthActLnkStat(u8 byEthID)
{
	return 1;
}

/*================================
函数名：BrdSetV35Param  
功能：设置v35接口参数
算法实现：（可选项）
引用全局变量：
输入参数说明：  dwDevId：v35接口的编号（0 ~ X）X视不同板子而定；
                dwIpAdrs：为ip地址
                dwIpMask：为掩码地址
返回值说明： 出错返回ERROR；成功返回OK。
==================================*/
STATUS BrdSetV35Param(u32 dwDevId, u32 dwIpAdrs, u32 dwIpMask)
{
	return OK;
}

/*================================
函数名：BrdGetV35Param
功能：获取v35接口参数
算法实现：（可选项）
引用全局变量：
输入参数说明：  dwDevId：v35接口的编号（0 ~ X）X视不同板子而定；
pdwIpAdrs：指向存放返回ip地址的指针
pdwIpMask: 指向存放返回掩码地址的指针
返回值说明： 出错返回ERROR；成功返回OK。
==================================*/
STATUS BrdGetV35Param(u32 dwDevId, u32 *pdwIpAdrs, u32 *pdwIpMask)
{
	return OK;
}

/*================================
函数名：BrdDelV35Param
功能：删除指定的v35接口地址
算法实现：（可选项）
引用全局变量：
输入参数说明：  dwDevId：v35接口的编号（0 ~ X）X视不同板子而定
返回值说明： 出错返回ERROR；成功返回OK。
==================================*/
STATUS BrdDelV35Param(u32 dwDevId)
{
	return OK;
}

/*================================
函数名：BrdLineIsLoop
功能：检查当前E1线路自环状态(如果有多个E1线路自环，则找到第一个自环的E1线路并返回IP地址)
算法实现：（可选项）
引用全局变量：
输入参数说明：  pdwIpAddr：存放接口ip地址的指针，仅在返回值为TRUE时有效
返回值说明： 自环则返回TRUE ；没有自环则返回FALSE
==================================*/
BOOL32   BrdLineIsLoop(u32 *pdwIpAddr)
{
	return OK;
}

/*================================
函数名：BrdGetE1MaxNum
功能：查询设备最大可配置的e1个数
算法实现：（可选项）
引用全局变量：
输入参数说明：无
返回值说明： 最大配置的e1个数
==================================*/
u8 BrdGetE1MaxNum(void)
{
	return OK;
}

/*================================
函数名：BrdSetE1SyncClkOutputState
功能：目前仅对DRI、DSI有效，主要设置从E1线路上提取出的网同步参考时钟是否输出给MPC板进行同步，
      同一时刻绝对禁止不同的板子同时输出时钟，否则MPC无法同步
算法实现：（可选项）
引用全局变量：
输入参数说明：byMode：网同步参考时钟输出模式，定义如下：
                #define E1_ CLK_OUTPUT_DISABLE   ((u8)0)   高阻禁止板上时钟信号输出到MPC板上
                #define E1_ CLK_OUTPUT_ENABLE    ((u8)1)   允许板上时钟信号输出到MPC板上
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdSetE1SyncClkOutputState (u8 byMode)
{
	return OK;
}

/*================================
函数名：BrdSetE1RelayLoopMode
功能：目前仅对DRI、DSI、KDV8010有效，主要设置E1链路继电器环回（正常、内环、外环）
算法实现：（可选项）
引用全局变量：
输入参数说明：byE1ID：E1链路的ID号，最大值和相关设备相关，具体如各单板最大E1个数宏定义;
            byMode：继电器工作模式设置，定义如下：
            #define BRD_RELAY_MODE_NORMAL     0    继电器连通
            #define BRD_RELAY_MODE_LOCLOOP    1    继电器自环
            #define BRD_RELAY_MODE_OUTLOOP    2    继电器外环
            说明：
            1：对DRI来说，只有正常、内环设置；
            2：对DSI来说，只有正常、外环设置；
            3：对MT来说，只有正常、内环设置，且和所插的模块相关。
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdSetE1RelayLoopMode(u8 byE1ID,u8 byMode)
{
	return OK;
}

/*================================
函数名：BrdSelectE1NetSyncClk
功能：目前仅对DRI、DSI有效，主要设置网同步参考时钟输出选择。
注意：该功能必须在BrdSetE1SyncClkOutputState设置了同步参考时钟输出允许的条件下才生效
算法实现：（可选项）
引用全局变量：
输入参数说明：byE1ID：E1链路的ID号，最大值和相关设备相关，具体如各单板最大E1个数宏定义
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdSelectE1NetSyncClk(u8 byE1ID)
{
	return OK;
}

/*================================
函数名：BrdQueryE1Imp
功能：目前仅对DRI、DSI、KDV8010有效，主要获取指定e1阻抗值
算法实现：（可选项）
引用全局变量：
输入参数说明：byE1ID：E1链路的ID号，最大值和相关设备相关，具体如各单板最大E1个数宏定义
返回值说明： 错误返回0xff；成功返回阻抗值宏定义
==================================*/
u8  BrdQueryE1Imp(u8 byE1ID)
{
	return OK;
}

/*================================
函数名：BrdGetE1AlmState
功能：获取指定E1线路的告警状态
算法实现：（可选项）
引用全局变量：
输入参数说明：byE1ID：E1链路的ID号，最大值和相关设备相关，具体如各单板最大E1个数宏定义
返回值说明： 返回0为正常，非0有告警
==================================*/
u8  BrdGetE1AlmState(u8 byE1Id)
{
	return OK;
}


/*================================
函数名：E1TransGetMaxE1Num
功能：获取设备实际支持的E1个数
算法实现：（可选项）
引用全局变量：
输入参数说明：无
返回值说明： 错误返回0并打印出错信息；成功, 返回设备实际支持的E1个数
==================================*/
/*
u8 E1TransGetMaxE1Num(void)
{
	return 1;
}
*/

/*================================
函数名：E1TransChanOpen
功能：打开一个E1透明传输通道，初始化相应的mcc_channel
算法实现：（可选项）
引用全局变量：
输入参数说明：byE1ChanID：打开的通道号,必须小于 E1_TRANS_CHAN_MAX_NUM
		      ptChanParam：通道的参数设置
              ptE1MsgCallBack：注册的回调函数
返回值说明： E1_TRANS_FAILURE/E1_TRANS_SUCCESS
==================================*/
/*
s32 E1TransChanOpen( u8 byE1ChanID,TE1TransChanParam *ptChanParam,TE1MsgCallBack  ptE1MsgCallBack )
{
	return E1_TRANS_SUCCESS;
}
*/

/*================================
函数名：E1TransChanClose
功能：关闭mcc控制器一个E1透明传输通道
算法实现：（可选项）
引用全局变量：
输入参数说明：byE1ChanID：打开的通道号,必须小于 E1_TRANS_CHAN_MAX_NUM
返回值说明： E1_TRANS_FAILURE/E1_TRANS_SUCCESS
==================================*/
/*
s32 E1TransChanClose( u8 byE1ChanID)
{
	return E1_TRANS_SUCCESS;
}
*/

/*================================
函数名：E1TransChanMsgSnd
功能：拷贝模式发送数据包
算法实现：（可选项）
引用全局变量：
输入参数说明：byE1ChanID:设备号
		      tMsgParam:消息数据信息指针
返回值说明： 见E1透明传输部分的返回值宏定义
==================================*/
/*
s32 E1TransChanMsgSnd(u8 byE1ChanID, TE1TransMsgParam *ptMsgParam)
{
	return E1_TRANS_SUCCESS;
}
*/

/*================================
函数名：E1TransTxPacketNumGet
功能：读取指定通道发送缓冲区队列中待发送的数据包的个数
算法实现：（可选项）
引用全局变量：
输入参数说明：byE1ChanID:设备号
返回值说明： 出错返回E1_TRANS_FAILURE；成功返回队列中待发送的数据包的个数
==================================*/
/*
s32 E1TransTxPacketNumGet(u8 byE1ChanID)
{
	return 1;
}
*/

/*================================
函数名：E1TransBufQHeadPtrInc
功能：移动读指针
算法实现：（可选项）
引用全局变量：
输入参数说明：byE1ChanID:设备号
              dwOffset：地址偏移
返回值说明： 见E1透明传输部分的返回值宏定义
==================================*/
/*
s32 E1TransBufQHeadPtrInc(u8 byE1ChanID, u32 dwOffset)
{
	return E1_TRANS_SUCCESS;
}
*/

/*================================
函数名：E1TransBufMsgCopy
功能：从中读取指定长度的数据到指定缓冲区，不移动读指针
算法实现：（可选项）
引用全局变量：
输入参数说明：byE1ChanID:设备号
              pbyDstBuf:读出数据保存区
              dwSize:期望读出长度
返回值说明： 错误返回0；成功返回读出的字节数
==================================*/
/*
s32 E1TransBufMsgCopy(u8 byE1ChanID, u8 *pbyDstBuf, u32 dwSize)
{
	return dwSize;
}
*/

/*================================
函数名：E1TransBufMsgLenGet
功能：查询当前可读数据的长度
算法实现：（可选项）
引用全局变量：
输入参数说明：byE1ChanID:设备号
返回值说明： 错误返回0；成功返回可读数据长度
==================================*/
/*
s32 E1TransBufMsgLenGet(u8 byE1ChanID)
{
	return 1;
}
*/

/*================================
函数名：E1TransBufMsgByteRead
功能：读取当前读指针开始指定偏移的一个字符，不移动读指针
算法实现：（可选项）
引用全局变量：
输入参数说明：byE1ChanID:设备号
              dwOffset:地址偏移
              pbyRtnByte:存字节数据的指针
返回值说明： 见E1透明传输部分的返回值宏定义
==================================*/
/*
s32 E1TransBufMsgByteRead(u8 byE1ChanID, u32 dwOffset, u8 *pbyRtnByte)
{
	if ( NULL == pbyRtnByte )
		return E1_TRANS_NO_BUF;

	return E1_TRANS_SUCCESS;
}
*/

/*================================
函数名：E1TransChanLocalLoopSet
功能：将指定通道物理线路自环
算法实现：（可选项）
引用全局变量：
输入参数说明：byE1ChanID:设备号
              bIsLoop：是否将该链路设置为自环模式
返回值说明： 见E1透明传输部分的返回值宏定义
==================================*/
/*
s32 E1TransChanLocalLoopSet(u8 byE1ChanID, BOOL32 bIsLoop)
{
	return E1_TRANS_SUCCESS;
}
*/

/*================================
函数名：E1TransChanInfoGet
功能：获取指定通道数据收发的统计信息
算法实现：（可选项）
引用全局变量：
输入参数说明：byE1ChanID:设备号
              ptInfo:存统计信息的指针
返回值说明： 见E1透明传输部分的返回值宏定义
==================================*/
/*
s32 E1TransChanInfoGet(u8 byE1ChanID, TE1TransChanStat *ptInfo)
{
	if ( NULL == ptInfo )
		return E1_TRANS_NO_BUF;

	return E1_TRANS_SUCCESS;
}
*/

/*================================
函数名：BrdOpenE1SingleLinkChan
功能：创建一个E1单链路连接通道
算法实现：（可选项）
引用全局变量：
输入参数说明：dwChanID: 单链路连接通道号，范围0~ E1_SINGLE_LINK_CHAN_MAX_NUM -1，该ID具有唯一性，用来标识每个单链路连接通道，不可冲突。一个通道不可重复打开，必须先关闭
              ptChanParam:单链路连接通道参数结构指针
返回值说明： E1_RETURN_OK/ERRCODE
==================================*/
STATUS BrdOpenE1SingleLinkChan(u32 dwChanID, TBrdE1SingleLinkChanInfo *ptChanParam)
{
	if ( NULL == ptChanParam )
		return E1_ERR_PARAM_EXCEPTION;

	return E1_RETURN_OK;
}

/*====================================================================
函数名      : BrdCloseE1SingleLinkChan
功能        ：关闭指定的E1单链路连接通道
算法实现    ：。
引用全局变量：无
输入参数说明：dwChanID: 单链路连接通道号，范围0~ E1_SINGLE_LINK_CHAN_MAX_NUM -1，
            该ID具有唯一性，用来标识每个单链路连接通道，不可冲突。
            该通道必须是已经打开成功的，如果未打开则返回E1_ERR_CHAN_NOT_CONF;
返回值说明  ：E1_RETURN_OK/ERRCODE。
====================================================================*/
STATUS BrdCloseE1SingleLinkChan(u32 dwChanID)
{
	return E1_RETURN_OK;
}

/*====================================================================
函数名      : BrdGetE1SingleLinkChanInfo
功能        ：读取指定的E1单链路连接通道信息
算法实现    ：调用路由组提供的nip模块的接口。
引用全局变量：无
输入参数说明：dwChanID: 单链路连接通道号，范围0~ E1_SINGLE_LINK_CHAN_MAX_NUM -1，
            该ID具有唯一性，用来标识每个单链路连接通道，不可冲突。
            该通道必须是已经打开成功的，如果未打开则返回E1_ERR_CHAN_NOT_CONF;
                ptChanInfo:单链路连接通道参数结构指针.
返回值说明  ：E1_RETURN_OK/ERRCODE。
====================================================================*/
STATUS BrdGetE1SingleLinkChanInfo(u32 dwChanID, TBrdE1SingleLinkChanInfo *ptChanInfo)
{
	if ( NULL == ptChanInfo )
		return E1_ERR_PARAM_EXCEPTION;

	return E1_RETURN_OK;
}

/*====================================================================
函数名      : BrdOpenE1MultiLinkChan
功能        ：创建一个E1多链路捆绑连接通道
算法实现    ：调用路由组提供的nip模块的接口。
引用全局变量：无
输入参数说明：dwChanID: 多链路捆绑连接通道号，范围0~ dwMultiLinkChanNum -1，
            该ID具有唯一性，用来标识每个单链路连接通道，不可冲突。一个通道不可重复打开，
            必须先关闭;
             ptChanParam:多链路捆绑连接通道参数结构指针.
返回值说明  ：E1_RETURN_OK/ERRCODE。
====================================================================*/
STATUS BrdOpenE1MultiLinkChan(u32 dwChanID, TBrdE1MultiLinkChanInfo *ptChanParam)
{
	if ( NULL == ptChanParam )
		return E1_ERR_PARAM_EXCEPTION;

	return E1_RETURN_OK;
}

/*====================================================================
函数名      : BrdCloseE1MultiLinkChan
功能        ：关闭指定的E1多链路捆绑连接通道
算法实现    ：。
引用全局变量：无
输入参数说明：dwChanID: 多链路捆绑连接通道号，范围0~ dwMultiLinkChanNum -1，
            该ID具有唯一性，用来标识每个多链路捆绑连接通道，不可冲突。
            该通道必须是已经打开成功的，如果未打开则返回E1_ERR_CHAN_NOT_CONF;
返回值说明  ：E1_RETURN_OK/ERRCODE。
====================================================================*/
STATUS BrdCloseE1MultiLinkChan(u32 dwChanID)
{
	return E1_RETURN_OK;
}
/*====================================================================
函数名      : BrdGetE1MultiLinkChanInfo
功能        ：读取指定的E1多链路捆绑连接通道信息
算法实现    ：调用路由组提供的nip模块的接口。
引用全局变量：无
输入参数说明：dwChanID: 多链路捆绑连接通道号，范围0~ dwMultiLinkChanNum -1，
            该ID具有唯一性，用来标识每个单链路连接通道，不可冲突。
            该通道必须是已经打开成功的，如果未打开则返回E1_ERR_CHAN_NOT_CONF;
            ptChanInfo:多链路捆绑连接通道参数结构指针.
返回值说明  ：E1_RETURN_OK/ERRCODE。
====================================================================*/
STATUS BrdGetE1MultiLinkChanInfo(u32 dwChanID, TBrdE1MultiLinkChanInfo *ptChanInfo)
{
	if ( NULL == ptChanInfo )
		return E1_ERR_PARAM_EXCEPTION;

	return E1_RETURN_OK;
}

/*RawFlash相关的函数声明*/

/*================================
函数名： BrdRawFlashIsUsed
功能：查询是否采用rawflash管理
算法实现：（可选项）
引用全局变量：无
输入参数说明：  无
返回值说明： TRUE:使用/FALSE:不使用
==================================*/
BOOL32   BrdRawFlashIsUsed(void)
{
	return TRUE;
}

/*================================
函数名：BrdGetFullRamDiskFileName
功能：转换相对路径的文件名为ramdisk中带绝对路径的文件名
算法实现：（可选项）
引用全局变量：无
输入参数说明： pInFileName:相对路径的文件名,如"webfiles/doc/www.html"; 
               pRtnFileName:存放返回绝对路径文件名的地址指针。
               上面的例子会输出:"/ramdisk/webfiles/doc/www.html"
返回值说明： 带绝对路径文件名的长度。
==================================*/
u32 BrdGetFullRamDiskFileName(s8 *pInFileName, s8 *pRtnFileName)
{
	if ( NULL == pInFileName || NULL == pRtnFileName )
		return 0;

	memcpy( pRtnFileName, "/ramdisk/", sizeof("/ramdisk/") );
	strcat( pRtnFileName, pInFileName );
	return strlen(pRtnFileName); 
}

/*================================
函数名：BrdFpUnzipFile
功能：解压复合分区中指定的文件，并输出到指定的文件。
算法实现：（可选项）
引用全局变量：无
输入参数说明： pUnzipFileName:待解压的文件名，可含相对路径，如"/webfiles/doc/aaa.html"。
               pOuputFileName:存放解压后数据的文件名，目前必须解压到/ramdisk/中，如"/ramdisk/bbb.html"
返回值说明  ：OK/ERROR。
==================================*/
STATUS BrdFpUnzipFile(s8 *pUnzipFileName, s8 *pOuputFileName)
{
	return OK;
}

/*================================
函数名：BrdFpUnzipFileIsExist
功能：查询复合分区压缩包中指定的压缩文件是否存在。
算法实现：（可选项）
引用全局变量：无
输入参数说明： pUnzipFileName:待解压的文件名
返回值说明： TRUE/FALSE
==================================*/
BOOL32   BrdFpUnzipFileIsExist(s8 *pUnzipFileName)
{
	if ( NULL == pUnzipFileName )
		return FALSE;

	return TRUE;
}

/*================================
函数名：BrdFpPartition
功能：分区。
算法实现：（可选项）
引用全局变量：无
输入参数说明： ptParam：指向TFlPartnParam结构缓冲区的指针。
返回值说明： OK/ERROR。
==================================*/
STATUS BrdFpPartition(TFlPartnParam *ptParam)
{
	if ( NULL == ptParam )
		return ERROR;

	return OK;
}

/*================================
函数名：BrdFpGetFPtnInfo
功能：读分区信息
算法实现：（可选项）
引用全局变量：无
输入参数说明： ptParam：指向TFlPartnParam结构缓冲区的指针。
返回值说明： OK/ERROR。
==================================*/
STATUS BrdFpGetFPtnInfo(TFlPartnParam *ptParam)
{
	if ( NULL == ptParam )
		return ERROR;

	return OK;
}

/*================================
函数名：BrdFpGetExecDataInfo
功能：读取特殊可执行程序的类型、地址和大小 
算法实现：（可选项）
引用全局变量：无
输入参数说明： pdwExecAdrs:存放特殊可执行数据相对地址的缓冲区的指针；
                pdwExeCodeType:存放特殊可执行数据类型的缓冲区的指针。
返回值说明： -1出错/其它值为特殊可执行数据的大小。
==================================*/
s32  BrdFpGetExecDataInfo(u32 *pdwExecAdrs, u32 *pdwExeCodeType)
{
	if ( NULL == pdwExecAdrs || NULL == pdwExeCodeType )
		return -1;

	return 1024;
}

/*================================
函数名：BrdFpReadExecData
功能：读取特殊可执行程序数据
算法实现：（可选项）
引用全局变量：无
输入参数说明： pbyDesBuf:存放特殊可执行数据的缓冲区的指针;
              dwLen: 要读取的数据的长度。
返回值说明： -1出错/其它值为读取特殊可执行程序数据的长度。
==================================*/
s32  BrdFpReadExecData(u8 *pbyDesBuf, u32 dwLen)
{
	if ( NULL == pbyDesBuf )
		return -1;

	return dwLen;
}

/*================================
函数名：BrdFpUpdateExecData
功能：更新特殊可执行程序数据
算法实现：（可选项）
引用全局变量：无
输入参数说明： pFile:FPGA文件名字
返回值说明： OK/ERROR。
==================================*/
STATUS BrdFpUpdateExecData(s8* pFile)
{
	if (NULL == pFile)
		return ERROR;

	return OK;
}

/*================================
函数名：BrdFpWriteExecData
功能：擦除ExeCode特殊可执行程序数据
算法实现：（可选项）
引用全局变量：无
输入参数说明： 无
返回值说明： OK/ERROR。
==================================*/
STATUS BrdFpEraseExeCode(void)
{
	return OK;
}

/*================================
函数名：BrdFpUpdateAuxData
功能：更新IOS分区中的数据(IOS分区中的数据组成：kernel+ramdisk，其中，kernel和ramdisk是路由组发布的--update.linux)
算法实现：（可选项）
引用全局变量：无
输入参数说明： pFile:update.linux文件名字
返回值说明： OK/ERROR。
==================================*/
STATUS BrdFpUpdateAuxData(s8* pFile)
{
	if ( NULL == pFile )
		return ERROR;
	
	return OK;
}

/*================================
函数名      : BrdFpEraseAuxData
功能        ：读取用户数据分区的个数。
算法实现    ：（可选项）
引用全局变量：
输入参数说明：无。
返回值说明  ：用户分区的个数
==================================*/
u8  BrdFpGetUsrFpnNum (void)
{
	return 1;
}

/*================================
函数名      : BrdFpWriteDataToUsrFpn
功能        ：写指定缓冲区数据到指定的用户数据分区
算法实现    ：（可选项）
引用全局变量：
输入参数说明：byIndex：第几个用户分区（从0开始）
              pbyData：要写入的数据指针
              dwLen：要写入数据的长度
返回值说明  ：OK/ERROR。
==================================*/
STATUS BrdFpWriteDataToUsrFpn(u8 byIndex, u8 *pbyData, u32 dwLen)
{
	if ( NULL == pbyData )
		return ERROR;

	return OK;
}

/*================================
函数名      : BrdFpWriteDataToUsrFpn
功能        ：读取指定分区内指定段指定长度的有效数据到缓冲区
算法实现    ：（可选项）
引用全局变量：
输入参数说明：byIndex：第几个用户分区（从0开始）
              pbyData：要读出的数据指针
              dwLen：要读出数据的长度
返回值说明  ：实际读取的数据长度
==================================*/
s32  BrdFpReadDataFromUsrFpn(u8 bySection, u8 *pbyDesBuf, u32 dwLen)
{
	if ( NULL == pbyDesBuf )
		return ERROR;

	return OK;
}

/*================================
函数名      : BrdMPCQueryAnotherMPCState
功能        ：对端主处理机板是否在位查询
算法实现    ：（可选项）
引用全局变量：
输入参数说明：无
返回值说明  ：宏定义：
		  #define BRD_MPC_OUTOF_POSITION      ((u8)0)   //对端主处理机板不在位
          #define BRD_MPC_IN_POSITION               ((u8)1)   //对端主处理机板在位
==================================*/
u8 BrdMPCQueryAnotherMPCState(void)
{
	return BRD_MPC_IN_POSITION;
}

/*================================
函数名      : BrdMPCQueryLocalMSState
功能        ：当前主处理机板主备用状态查询
算法实现    ：（可选项）
引用全局变量：
输入参数说明：无
返回值说明  ：宏定义：
		  #define BRD_MPC_RUN_MASTER      ((u8)0)   // 当前主处理机板主用
          #define BRD_MPC_RUN_SLAVE               ((u8)1)   // 当前主处理机板备用 
==================================*/
u8 BrdMPCQueryLocalMSState (void)
{
	return BRD_MPC_RUN_MASTER;
}

/*================================
函数名      : BrdMPCSetLocalMSState
功能        ：设置当前主处理机板主备用状态
算法实现    ：（可选项）
引用全局变量：
输入参数说明：byState:
              #define BRD_MPC_RUN_MASTER          0      // 当前主处理机板主用
              #define BRD_MPC_RUN_SLAVE           1      // 当前主处理机板备用 
返回值说明  ：OK/ERROR
==================================*/
STATUS BrdMPCSetLocalMSState (u8 byState)
{
	return OK;
}

/*================================
函数名      : BrdMPCQuerySDHType
功能        ：光模块种类查询
算法实现    ：（可选项）
引用全局变量：
输入参数说明：无
返回值说明  ：宏定义：
		#define BRD_MPC_SDHMODULE_NONE      ((u8)0x07)   // 当前主处理机板没有插模块 
        #define BRD_MPC_SDHMODULE_SOI1      ((u8)0x00)   // 当前主处理机板插模块SOI1 
        #define BRD_MPC_SDHMODULE_SOI4      ((u8)0x01)   // 当前主处理机板插模块SOI4  
==================================*/
u8 BrdMPCQuerySDHType (void)
{
	return BRD_MPC_SDHMODULE_SOI1;
}

/*================================
函数名      : BrdMPCQueryNetSyncMode
功能        ：锁相环工作模式查询
算法实现    ：（可选项）
引用全局变量：
输入参数说明：无
返回值说明  ：宏定义：
		#define SYNC_MODE_FREERUN           ((u8)0)   // 自由振荡，对于MCU应设置为该模式，所有下级线路时钟与该MCU同步
        #define SYNC_MODE_TRACK_SDH8K       ((u8)1)   // 跟踪模式，作为下级MCU应设置为该模式，跟踪SDH来的8K时钟
        #define SYNC_MODE_TRACK_SDH2M       ((u8)2)   // 跟踪模式，作为下级MCU应设置为该模式，跟踪SDH来的2M时钟 
        #define SYNC_MODE_TRACK_DT2M        ((u8)4)   // 跟踪模式，作为下级MCU应设置为该模式，跟踪DT来的2M时钟 
        #define SYNC_MODE_UNKNOWN           ((u8)0xff)// 未知或错误的模式 
==================================*/
u8 BrdMPCQueryNetSyncMode (void)
{
	return SYNC_MODE_FREERUN;
}

/*================================
函数名      : BrdMPCSetNetSyncMode
功能        ：锁相环网同步模式选择设置，不支持SDH部分
算法实现    ：（可选项）
引用全局变量：
输入参数说明：byMode：锁相环网同步模式，有如下定义：
        #define SYNC_MODE_FREERUN           ((u8)0)   // 自由振荡，对于主MCU应设置为该模式，所有下级线路时钟与该MCU同步 
        #define SYNC_MODE_TRACK_SDH8K       ((u8)1)   //跟踪模式，作为下级MCU应设置为该模式，跟踪SDH来的8K时钟 
        #define SYNC_MODE_TRACK_SDH2M       ((u8)2)   // 跟踪模式，作为下级MCU应设置为该模式，跟踪SDH来的2M时钟 
        #define SYNC_MODE_TRACK_DT2M        ((u8)4)   // 跟踪模式，作为下级MCU应设置为该模式，跟踪DT来的2M时钟
返回值说明  ：错误返回ERROR；成功返回OK
==================================*/
STATUS BrdMPCSetNetSyncMode(u8 byMode)
{
	return OK;
}

/*================================
函数名      : BrdMPCSetAllNetSyncMode
功能        ：锁相环网同步模式选择设置,增加sdh部分
算法实现    ：（可选项）
引用全局变量：
输入参数说明：byMode：锁相环网同步模式，有如下定义：
        #define SYNC_MODE_FREERUN           ((u8)0)   // 自由振荡，对于主MCU应设置为该模式，所有下级线路时钟与该MCU同步 
        #define SYNC_MODE_TRACK_SDH8K       ((u8)1)   // 跟踪模式，作为下级MCU应设置为该模式，跟踪SDH来的8K时钟 
        #define SYNC_MODE_TRACK_SDH2M       ((u8)2)   // 跟踪模式，作为下级MCU应设置为该模式，跟踪SDH来的2M时钟 
        #define SYNC_MODE_TRACK_DT2M        ((u8)4)   // 跟踪模式，作为下级MCU应设置为该模式，跟踪DT来的2M时钟
        dwSdhE1Id：对于同步模式为SYNC_MODE_TRACK_SDH8K和SYNC_MODE_TRACK_SDH2M时必须指定sdh对应的e1号，如果是SOI-1模
        块范围为241-301；如果是SIO-4模块时范围为241-484
返回值说明  ：错误返回ERROR；成功返回OK
==================================*/
STATUS BrdMPCSetAllNetSyncMode(u8 byMode, u32 dwSdhE1Id)
{
	return OK;
}

/*================================
函数名      : BrdMPCResetSDH
功能        ：光模块复位
算法实现    ：（可选项）
引用全局变量：
输入参数说明：无
返回值说明  ：错误返回ERROR；成功返回OK
==================================*/
STATUS BrdMPCResetSDH (void)
{
	return OK;
}

/*================================
函数名      : BrdMPCReSetAnotherMPC
功能        ：复位对端主处理机
算法实现    ：（可选项）
引用全局变量：
输入参数说明：无
返回值说明  ：错误返回ERROR；成功返回OK
==================================*/
STATUS BrdMPCReSetAnotherMPC (void)
{
	return OK;
}

/*================================
函数名      : BrdMPCOppReSetDisable
功能        ：禁止对端主处理机复位本主处理机
算法实现    ：（可选项）
引用全局变量：
输入参数说明：无
返回值说明  ：错误返回ERROR；成功返回OK
==================================*/
STATUS BrdMPCOppReSetDisable (void)
{
	return OK;
}

/*================================
函数名      : BrdMPCOppReSetEnable 
功能        ：允许对端主处理机复位本主处理机
算法实现    ：（可选项）
引用全局变量：
输入参数说明：无
返回值说明  ：错误返回ERROR；成功返回OK
==================================*/
STATUS BrdMPCOppReSetEnable (void)
{
	return OK;
}

/*================================
函数名      : BrdMPCLedBoardSpeakerSet
功能        ：设置灯板上扬声器蜂鸣的状态
算法实现    ：（可选项）
引用全局变量：
输入参数说明：byState：扬声器状态，如下宏定义
        #define LED_BOARD_SPK_ON                ((u8)0x01)   //开启扬声器
        #define LED_BOARD_SPK_OFF               ((u8)0x00)   // 关闭扬声器
返回值说明  ：错误返回ERROR；成功返回OK
==================================*/
STATUS BrdMPCLedBoardSpeakerSet(u8 byState)
{
	return OK;
}

/*================================
函数名      : BrdFastLoadEqtFileIsEn
功能        ：是否允许bsp15快速启动
算法实现    ：（可选项）
引用全局变量：
输入参数说明：无
返回值说明  ：0=禁止快速启动bsp15，从flash启动，速度较慢;
              1=允许快速启动bsp15，从内存镜像启动，速度较快，但占用较多内存;
==================================*/
u8 BrdFastLoadEqtFileIsEn(void)
{
	return 1;
}

/*================================
函数名      : AtaApiMountPartion
功能        ：挂载磁盘分区---Linux上新增的磁盘操作接口
算法实现    ：（可选项）
引用全局变量：
输入参数说明：pchPartionName:要挂载的磁盘分区名称，如"/dev/hda0"
              pMntPath:要挂载的目的路径，如"/ramdisk/hda0"
返回值说明  ：错误返回如ata驱动模块错误码描述错误码；成功, 返回ATA_SUCCESS.
==================================*/
//s32 AtaApiMountPartion(s8* pchPartionName,s8* pMntPath)
//{
//	return ATA_SUCCESS;
//}

/*================================
函数名      : BrdNipErase
功能        ：擦除NIP配置信息
算法实现    ：擦除配置；请在console模式下调用该函数；
              telnet上调用该函数会导致telnet无法正常工作
              （由于nip中IP地址被擦除后，telnet无法正常工作）；
引用全局变量：
输入参数说明：无
返回值说明  ：错误返回ERROR；成功返回OK
==================================*/
STATUS BrdNipErase(void)
{
	return OK;
}

/*================================
函数名      : BrdMicAdjustIsSupport
功能        ：查询当前mic调节是否支持
算法实现    ：
引用全局变量：
输入参数说明：无
返回值说明  ：错误返回FALSE；成功返回TRUE
==================================*/
BOOL32   BrdMicAdjustIsSupport(void)
{
	return TRUE;
}

/*================================
函数名      : BrdMicVolumeSet
功能        ：设置mic输入音量
算法实现    ：
引用全局变量：
输入参数说明：byTapPosition:音阶，范围：0-127,对音量为正逻辑
返回值说明  ：错误返回ERROR；成功返回OK
==================================*/
STATUS BrdMicVolumeSet(u8  byTapPosition)
{
	if ( 0 > byTapPosition || 127 < byTapPosition )
		return ERROR;

	return OK;
}

/*================================
函数名      : BrdMicVolumeGet
功能        ：查询当前mic输入音量 
算法实现    ：
引用全局变量：
输入参数说明：无
返回值说明  ：当前mic输入音量(0-127)/0xff=错误
==================================*/
u8 BrdMicVolumeGet(void)
{
	return OK;
}


