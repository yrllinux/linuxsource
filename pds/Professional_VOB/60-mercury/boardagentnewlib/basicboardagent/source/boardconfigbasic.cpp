/*****************************************************************************
   模块名      : Board Agent Basic
   文件名      : boardconfigbasic.cpp
   相关文件    : 
   文件实现功能: 单板代理提供给上层应用的接口实现
   作者        : 周广程
   版本        : V4.0  Copyright(C) 2001-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2007/08/20  4.0         周广程       创建
******************************************************************************/
#include "osp.h"
#include "boardconfigbasic.h"


// 构造函数
CBBoardConfig::CBBoardConfig()
{
	m_dwBrdIpAddr = 0;
    m_byBrdEthChoice = 1;  // 默认为后网口

	m_dwDstMcuNodeA = INVALID_NODE;
	m_dwDstMcuNodeB = INVALID_NODE;

	m_dwDstMcuIIdA = INVALID_INS;
	m_dwDstMcuIIdB = INVALID_INS;
    
    m_wDiscHeartBeatTime = DEF_OSPDISC_HBTIME;
    m_byDiscHeartBeatNum = DEF_OSPDISC_HBNUM;
	
	m_dwMpcIpAddrA = 0;
	m_dwMpcIpAddrB = 0;

	m_wMpcPortA = 0;
	m_wMpcPortB = 0;

	m_byBrdId = 0;
	m_dwBrdIpAddr = 0;
	memset( &m_tBoardPosition, 0, sizeof(m_tBoardPosition) );

	m_bPrintBrdLog = FALSE;
	m_bUpdateSoftBoard = TRUE;
}

// 析构函数
CBBoardConfig::~CBBoardConfig()
{
	m_dwDstMcuNodeA = 0;
	m_dwDstMcuNodeB = 0;

	m_dwDstMcuIIdA = 0;
	m_dwDstMcuIIdB = 0;
}

/*====================================================================
    函数名      ：GetBoardId
    功能        ：获取单板的ID
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：ID值，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2007/08/20  4.0         周广程       创建
====================================================================*/
u8	CBBoardConfig::GetBoardId()
{
	return m_byBrdId ;
}

/*=============================================================================
函 数 名： SetBrdId
功    能： 
算法实现： 
全局变量： 
参    数： u8 byBrdId
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/22   4.0			周广程                  创建
=============================================================================*/
void CBBoardConfig::SetBrdId(u8 byBrdId)
{
	m_byBrdId = byBrdId;
}

/*=============================================================================
函 数 名： SetBrdId
功    能： 
算法实现： 
全局变量： 
参    数： u8 byBrdLayer
           u8 byBrdSlot
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/22   4.0			周广程                  创建
=============================================================================*/
void CBBoardConfig::SetBrdId(u8 byBrdLayer, u8 byBrdSlot)
{
	m_byBrdId = (byBrdLayer<<4) | (byBrdSlot+1);
}

/*====================================================================
    函数名      ：GetBrdIpAddr
    功能        ：得到单板IP地址
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：单板IP地址(主机序)
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2007/08/20  4.0         周广程       创建
====================================================================*/
u32 CBBoardConfig::GetBrdIpAddr( )
{
	return  m_dwBrdIpAddr ;
}

/*=============================================================================
函 数 名： SetBrdIpAddr
功    能： 设置单板IP地址
算法实现： 
全局变量： 
参    数： u32 dwBrdIp(主机序)
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/22   4.0			周广程                  创建
=============================================================================*/
void CBBoardConfig::SetBrdIpAddr( u32 dwBrdIp )
{
	m_dwBrdIpAddr =  dwBrdIp ;
}

/*==============================================================================
函数名    :  GetBrdEthChoice
功能      :  获取前网口/后网口
算法实现  :  
参数说明  :  
返回值说明:  u8
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2008-11-5
==============================================================================*/
u8 CBBoardConfig::GetBrdEthChoice()
{
	return m_byBrdEthChoice;
}
/*==============================================================================
函数名    :  SetBrdEthChoice
功能      :  设置是前网口/后网口
算法实现  :  
参数说明  :  
返回值说明:  void
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2008-11-5
==============================================================================*/
void CBBoardConfig::SetBrdEthChoice(u8 byEthChoice)
{
	m_byBrdEthChoice = byEthChoice;
}
/*=============================================================================
函 数 名： GetMpcIpA
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： u32 MPCA的IP（主机序）
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/22   4.0			周广程                  创建
=============================================================================*/
u32 CBBoardConfig::GetMpcIpA()
{
	return m_dwMpcIpAddrA;
}

/*=============================================================================
函 数 名： SetMpcIpA
功    能： 
算法实现： 
全局变量： 
参    数： u32 dwMpcIp(主机序)
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/22   4.0			周广程                  创建
=============================================================================*/
void CBBoardConfig::SetMpcIpA(u32 dwMpcIp)
{
	m_dwMpcIpAddrA = dwMpcIp;
}

/*=============================================================================
函 数 名： GetMpcIpB
功    能： 
算法实现： 
全局变量： 
参    数： void
返 回 值： u32 (主机序)
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/22   4.0			周广程                  创建
=============================================================================*/
u32 CBBoardConfig::GetMpcIpB(void)
{
    return m_dwMpcIpAddrB;
}

/*=============================================================================
函 数 名： SetMpcIpB
功    能： 
算法实现： 
全局变量： 
参    数： u32 dwMpcIp(主机序)
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/22   4.0			周广程                  创建
=============================================================================*/
void CBBoardConfig::SetMpcIpB(u32 dwMpcIp)
{
	m_dwMpcIpAddrB = dwMpcIp;
}

/*=============================================================================
函 数 名： GetMpcPortA
功    能： 
算法实现： 
全局变量： 
参    数： void
返 回 值： u16 (主机序)
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/22   4.0			周广程                  创建
=============================================================================*/
u16 CBBoardConfig::GetMpcPortA(void)
{
	return m_wMpcPortA;
}

/*=============================================================================
函 数 名： SetMpcPortA
功    能： 
算法实现： 
全局变量： 
参    数： u16 wMpcPort(主机序)
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/22   4.0			周广程                  创建
=============================================================================*/
void CBBoardConfig::SetMpcPortA(u16 wMpcPort)
{
	m_wMpcPortA =  wMpcPort;
}

/*=============================================================================
函 数 名： GetMpcPortB
功    能： 
算法实现： 
全局变量： 
参    数： void
返 回 值： u16 (主机序)
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/22   4.0			周广程                  创建
=============================================================================*/
u16 CBBoardConfig::GetMpcPortB(void)
{
    return m_wMpcPortB;
}

/*=============================================================================
函 数 名： SetMpcPortB
功    能： 
算法实现： 
全局变量： 
参    数： u16 wMpcPort(主机序)
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/22   4.0			周广程                  创建
=============================================================================*/
void CBBoardConfig::SetMpcPortB(u16 wMpcPort)
{
    m_wMpcPortB =  wMpcPort;
}

/*=============================================================================
函 数 名： GetBrdPosition
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： TBrdPos 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/27   4.0			周广程                  创建
=============================================================================*/
TBrdPos CBBoardConfig::GetBrdPosition()
{
	return m_tBoardPosition;
}

/*=============================================================================
函 数 名： SetBrdPosition
功    能： 
算法实现： 
全局变量： 
参    数： TBrdPos tBrdPosition
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/27   4.0			周广程                  创建
=============================================================================*/
void CBBoardConfig::SetBrdPosition( TBrdPos tBrdPosition )
{
	memcpy(&m_tBoardPosition, &tBrdPosition, sizeof(TBrdPos) );
}

/*====================================================================
    函数名      ：ReadConnectMcuInfo
    功能        ：读取连接的MCU信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：MCU的端口号，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2007/08/20  4.0         周广程       创建
====================================================================*/
BOOL CBBoardConfig::ReadConnectMcuInfo()
{
	// 获取配置文件中MPC信息
    s8    achProfileName[32] = {0};
    BOOL  bResult;
    s8    achDefStr[] = "Cannot find the section or key";
    s8    achReturn[MAX_VALUE_LEN + 1];
    s32   sdwValue;
    s32   dwFlag;
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "brdcfg.ini");

	//BrdEthChoice
    if (!IsBrdMtHDSerial())
    {
        bResult = GetRegKeyInt(achProfileName, SECTION_IsFront, KEY_Flag, 1, &dwFlag);
        if(FALSE == bResult)
        {
            OspPrintf( TRUE, TRUE, "[BoardAgent] Wrong profile while reading %s!\n", "IsFront" );
            dwFlag = 1;
        }
        m_byBrdEthChoice = (u8)dwFlag;   // 0: front , 1: back
    }
    else
    {
        m_byBrdEthChoice = 0;
    }
	printf("[ReadConnectMcuInfo]Get BrdEthChoice success:%d!\n", m_byBrdEthChoice);

	// 获得单板位置和IP
	if ( !GetBoardInfo() )
	{
		OspPrintf( TRUE, FALSE, "[BoardAgent] Error while get boardinfo!\n");
	}
	else
	{
		printf("GetBoardInfo success!\n");
	}
	
	WriteBoardInfoToDetecteeFile();

	//MpcIp
    bResult = GetRegKeyString( achProfileName, SECTION_BoardSystem, KEY_MpcIpAddr, 
						achDefStr, achReturn, sizeof(achReturn) );
    if( bResult == FALSE )  
	{
		printf("[ReadConnectMcuInfo]Get MpcIpAddr failed!\n");		
		return FALSE;
	}
	m_dwMpcIpAddrA =  ntohl( INET_ADDR( achReturn ) );
	if( 0xffffffff == m_dwMpcIpAddrA )
	{
		m_dwMpcIpAddrA = 0;
	}
    if( 0 == m_dwMpcIpAddrA )
    {
		printf("[ReadConnectMcuInfo] The A Mpc's Ip is 0.\n");
        return FALSE;
    }
	printf("[ReadConnectMcuInfo]Get MpcIpAddrB success:%d!\n", m_dwMpcIpAddrA);
    
	//MpcIpB 
	bResult = GetRegKeyString( achProfileName, SECTION_BoardSystem, KEY_MpcIpAddrB, 
		achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )  
	{
		OspPrintf( TRUE, TRUE, "[BoardAgent] Wrong profile while reading %s!\n", KEY_MpcIpAddrB );		
	}
	m_dwMpcIpAddrB =  ntohl( INET_ADDR( achReturn ) );
	if( 0xffffffff == m_dwMpcIpAddrB )
	{
		m_dwMpcIpAddrB = 0;
	}
    if( 0 == m_dwMpcIpAddrB )
    {
        OspPrintf(TRUE, FALSE, "[BoardAgent] The B Mpc's Ip is 0.\n");
    }
 	printf("[ReadConnectMcuInfo]Get MpcIpAddrB success:%d!\n", m_dwMpcIpAddrB);

	// MpcPort
	bResult = GetRegKeyInt( achProfileName, SECTION_BoardSystem, KEY_MpcPort, MCU_LISTEN_PORT, &sdwValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, TRUE, "[BoardAgent] Wrong profile while reading %s!\n", KEY_MpcPort );
		sdwValue = MCU_LISTEN_PORT;
	}
	m_wMpcPortA = ( u16 )sdwValue;
	printf("[ReadConnectMcuInfo]Get MpcPort success:%d!\n", m_wMpcPortA);

	// MpcPort2
	bResult = GetRegKeyInt( achProfileName, SECTION_BoardSystem, KEY_MpcPortB, MCU_LISTEN_PORT, &sdwValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, TRUE, "[BoardAgent] Wrong profile while reading %s!\n", KEY_MpcPortB );
		sdwValue = MCU_LISTEN_PORT;
	}
	m_wMpcPortB = ( u16 )sdwValue;
	printf("[ReadConnectMcuInfo]Get MpcPortB success:%d!\n", m_wMpcPortB);

    // FIXME zw
    bResult = GetRegKeyString( achProfileName, SECTION_BoardSystem, (s8*)"BrdIpAddr", 
						achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )  
	{
		OspPrintf( TRUE, TRUE, "[BoardAgent] Wrong profile while reading board ip from brdcfg.ini!\n" );		
	}
    else
    {
	    m_dwBrdIpAddr =  ntohl(INET_ADDR( achReturn ));
	    if( 0xffffffff == m_dwBrdIpAddr || 0 == m_dwBrdIpAddr )
	    {
            OspPrintf( TRUE, FALSE, "[BoardAgent] The board's Ip is 0.\n" );
			printf("[BoardAgent] The board's Ip is 0.\n");
            return FALSE;
	    }
	}
	printf("[ReadConnectMcuInfo]Get BrdIpAddr success:%d!\n", m_dwBrdIpAddr);

    //读取osp断链检测参数
    GetRegKeyInt( achProfileName, SECTION_BoardSystem, KEY_HeartBeatTime, DEF_OSPDISC_HBTIME, &sdwValue );
    if (sdwValue >= 3 )
    {
        m_wDiscHeartBeatTime = (u16)sdwValue;
    }
    else
    {
        m_wDiscHeartBeatTime = DEF_OSPDISC_HBTIME;
    }
    
    GetRegKeyInt( achProfileName, SECTION_BoardSystem, KEY_HeartBeatNum, DEF_OSPDISC_HBNUM, &sdwValue );
    if (sdwValue >= 1)
    {
        m_byDiscHeartBeatNum = (u8)sdwValue;
    }   
    else
    {
        m_byDiscHeartBeatNum = DEF_OSPDISC_HBNUM;
	}
	
	return TRUE;
}

/*====================================================================
    函数名      ：WriteBoardInfoToDetecteeFile
    功能        ：写单板信息到单板搜索进程读取信息的文件中
				  （通过文件与单板搜索进程通信）
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2013/04/19  4.7         chendaiwei       创建
====================================================================*/
BOOL32 CBBoardConfig::WriteBoardInfoToDetecteeFile ( void )
{
	s8 achDeviceName[64] = {0};
	s8 achDeviceType[64] ={0};

	switch ( m_tBoardPosition.byBrdID)
	{
	case BRD_TYPE_MPC:
		strcpy(achDeviceType,BRD_ALIAS_MPC);
		break;
	case BRD_TYPE_APU:
		strcpy(achDeviceType,BRD_ALIAS_APU);
		break;
	case BRD_TYPE_DRI:
		strcpy(achDeviceType,BRD_ALIAS_DRI);
		break;
	case BRD_TYPE_CRI:
		strcpy(achDeviceType,BRD_ALIAS_CRI);
		break;
	default:
		printf("[WriteBoardInfoToDetecteeFile] No need to write file!\n");
		return TRUE;

		break;
	}

	u8 byBrdTypeStrLen = strlen(achDeviceType);
	memcpy(&achDeviceName[0],achDeviceType,byBrdTypeStrLen);	

	if(    m_tBoardPosition.byBrdID == BRD_TYPE_HDU2
		|| m_tBoardPosition.byBrdID == BRD_TYPE_HDU2_L
		|| m_tBoardPosition.byBrdID == BRD_TYPE_HDU2_S
		|| m_tBoardPosition.byBrdID == BRD_TYPE_HDU
		|| m_tBoardPosition.byBrdID == BRD_TYPE_HDU_L)
	{
		//层号取配置文件
		s8    achProfileName[64] = {0};
		memset((void*)achProfileName, 0x0, sizeof(achProfileName));
		sprintf(achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFGDEBUG_INI);
		//Layer
		s32  sdwLayer = 0;
		if(!GetRegKeyInt( achProfileName, SECTION_BoardConfig, KEY_Layer, 0, &sdwLayer ))
		{
			printf( "[WriteBoardInfoToDetecteeFile] Wrong profile while reading %s%s!\n", SECTION_BoardConfig, KEY_Layer );
			return FALSE;
		}
		
		sprintf(&achDeviceName[byBrdTypeStrLen],"(%d层%d槽)",(u8)sdwLayer,m_tBoardPosition.byBrdSlot);
	}
	else
	{
		sprintf(&achDeviceName[byBrdTypeStrLen],"(%d层%d槽)",m_tBoardPosition.byBrdLayer,m_tBoardPosition.byBrdSlot);
	}

	printf("[WriteBoardInfoToDetecteeFile] device name:%s\n",achDeviceName);

	s8 achConfigFilePath[64]={0};


//方案1：
// 	sprintf(achConfigFilePath,"%s/%s",DIR_CONFIG,"detectee.ini");
//  
// 	FILE *hFile = fopen(achConfigFilePath,"w+");
// 	if( hFile == NULL)
// 	{
// 	 	printf("[WriteBoardInfoToDetecteeFile]open or create detectee.ini failed!\n");
// 
// 		return FALSE;
// 	}
// 
// 	if(fputs(achDeviceType,hFile) == EOF )
// 	{
// 		printf("[WriteBoardInfoToDetecteeFile] write Device Type Failed!\n");
// 
// 		fclose(hFile);
// 		
// 		return FALSE;
// 	}
// 
// 	if(fputs(achDeviceName,hFile) == EOF )
// 	{
// 		printf("[WriteBoardInfoToDetecteeFile] write Device name Failed!\n");
// 		
// 		fclose(hFile);
// 		
// 		return FALSE;
// 	}
// 
// 	fclose(hFile);

//方案二：

	sprintf(achConfigFilePath,"%s/%s",DIR_CONFIG,"kdvipdt.ini");
	
	FILE *hFile = fopen(achConfigFilePath,"a+");
	if( hFile == NULL)
	{
		printf("[WriteBoardInfoToDetecteeFile]open or create kdvipdt.ini failed!\n");
		
		return FALSE;
	}
	else
	{
		fclose(hFile);
	}
	
	if(!SetRegKeyString(achConfigFilePath,"Dev","TypeAlias",achDeviceType))
	{
		printf("[WriteBoardInfoToDetecteeFile]set type alis failed!\n");
		
		return FALSE;
	}
	
	if(!SetRegKeyString(achConfigFilePath,"Dev","Alias",achDeviceName))
	{
		printf("[WriteBoardInfoToDetecteeFile]set device alis failed!\n");
		
		return FALSE;
	}

#if defined(_LINUX_)

	u32 dwGw = BrdGetDefGateway();
	printf("[WriteBoardInfoToDetecteeFile]default GW 0x%x!\n",dwGw);
	if(!SetRegKeyInt(achConfigFilePath,"Dev","GW",(s32)dwGw))
	{
		printf("[WriteBoardInfoToDetecteeFile]set device GW failed!\n");
		
		return FALSE;
	}

#endif

//end:方案二

	return TRUE;
};

/*====================================================================
    函数名      GetBrdPosByConfig
    功能        ：通过配置文件获取单板的TBrdPosition和IP地址
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：成功返回TRUE，反之返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2007/08/20  4.0         周广程       创建
====================================================================*/
BOOL32 CBBoardConfig::GetBrdPosByConfig(TBrdPos &tBrdPos)
{
	s8   achProfileName[KDV_MAX_PATH] = {0};
    BOOL bResult;
    s32  nValue;

	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "brdcfgfile.ini");

	bResult = GetRegKeyInt( achProfileName, "BoardPos", "BrdID", 0, &nValue );
	tBrdPos.byBrdID = (nValue >= 0) ? nValue : 0;
	bResult = (bResult && GetRegKeyInt( achProfileName, "BoardPos", "BrdLayer", 0, &nValue ));
	tBrdPos.byBrdLayer = (nValue >= 0) ? nValue : 0;
	bResult = (bResult && GetRegKeyInt( achProfileName, "BoardPos", "BrdSlot", 0, &nValue ));
	tBrdPos.byBrdSlot = (nValue >= 0) ? nValue : 0;

	printf( "[Brd_Debug][GetBrdPosByConfig] bResult = %d!\n", bResult );

	return bResult;
}

/*====================================================================
    函数名      ：GetBoardInfo
    功能        ：获取单板的TBrdPosition和IP地址
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：成功返回TRUE，反之返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2007/08/20  4.0         周广程       创建
====================================================================*/
BOOL32 CBBoardConfig::GetBoardInfo()
{
    s8   achProfileName[32];
    BOOL bResult;
    s32  nValue;

	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "brdcfgdebug.ini");
	
	// 在测试时要建立节点
	bResult = GetRegKeyInt( achProfileName, "BoardDebug", "IsTest", 0, &nValue );
	if( bResult == TRUE )  
	{
		m_bIsTest = (nValue != 0);
		if(m_bIsTest)
        {
			OspCreateTcpNode( 0, 4400 );
        }
	}
	STATUS nRet;
    u8 byBrdID = BrdGetBoardID();

#ifndef _LINUX12_
    if ( byBrdID == BRD_TYPE_DSC/*KDV8000B_MODULE*/ || 
         byBrdID == BRD_TYPE_MDSC/*DSL8000_BRD_MDSC*/ ||
         byBrdID == BRD_TYPE_HDSC/*DSL8000_BRD_HDSC*/ )
    {
        m_tBoardPosition.byBrdLayer = 0;
        m_tBoardPosition.byBrdSlot = 0;
	    m_tBoardPosition.byBrdID = byBrdID;
    }    
    else 
#endif
    if ( IsBrdMtHDSerial() )
    {
        m_tBoardPosition.byBrdLayer = BRD_LAYER_CUSTOM;
        m_tBoardPosition.byBrdSlot = 0;
        m_tBoardPosition.byBrdID = byBrdID;
    }    
    else
    {
		TBrdPosition tBrdPosition; 
        nRet = BrdQueryPosition( &tBrdPosition );
#ifdef _LINUX12_
		m_tBoardPosition.byBrdID = (u8)tBrdPosition.byBrdID;
		m_tBoardPosition.byBrdLayer = tBrdPosition.byBrdLayer;
		m_tBoardPosition.byBrdSlot = tBrdPosition.byBrdSlot;

		TBrdE2PromInfo tBrdE2PromInfo;
		memset( &tBrdE2PromInfo, 0x0, sizeof(tBrdE2PromInfo) );
		nRet = BrdGetE2PromInfo(&tBrdE2PromInfo); 
		if (ERROR == nRet)
		{
			printf("[BoardAgent][GetBoardInfo] BrdGetE2PromInfo error!\n");
			return FALSE;
		}
		byBrdID = TransPid2BrdType(tBrdE2PromInfo.dwProductId);
		if (UNKNOWN_BOARD != byBrdID)
		{
			printf("[BoardAgent][GetBoardInfo] byBrdID change from %d to %d!\n", m_tBoardPosition.byBrdID, byBrdID);
			m_tBoardPosition.byBrdID = byBrdID;			
		}	
		printf("[BoardAgent][GetBoardInfo] dwProductId 0x%x!\n", tBrdE2PromInfo.dwProductId);


#else
		m_tBoardPosition.byBrdID = tBrdPosition.byBrdID;
        m_tBoardPosition.byBrdLayer = tBrdPosition.byBrdLayer;
		m_tBoardPosition.byBrdSlot = tBrdPosition.byBrdSlot;
#endif
        printf("[BoardAgent][GetBoardInfo] Lay.%d, Slot.%d, Type.%d, nRet.%d!\n",
            m_tBoardPosition.byBrdLayer, m_tBoardPosition.byBrdSlot, m_tBoardPosition.byBrdID, nRet);
		if(m_tBoardPosition.byBrdID == BRD_TYPE_IS21)
		{
			m_tBoardPosition.byBrdLayer = 0;
			m_tBoardPosition.byBrdSlot = 7;
			printf("=============Fake Is2.1 0 layer 7 slot!===============\n");
		}

		if ( ERROR == nRet )
        {
            OspPrintf( TRUE, FALSE, "[BoardAgent][GetBoardInfo] BrdQueryPosition error!\n");
            return FALSE;
	    }	
     }
    m_byBrdId = (m_tBoardPosition.byBrdLayer<<4) | (m_tBoardPosition.byBrdSlot+1);

	TBrdEthParam tBrdEthParam;       
    nRet = BrdGetEthParam(m_byBrdEthChoice, &tBrdEthParam);// 取单板Ip
	if ( ERROR == nRet )
	{
		printf( "[BoardAgent][GetBoardInfo] BrdGetEthParam error!\n");
		return FALSE;
	}
	m_dwBrdIpAddr = ntohl(tBrdEthParam.dwIpAdrs);
    
	return TRUE;
}

/*=============================================================================
函 数 名： IsBrdMtHDSerial
功    能： 是否是由 KDV7810、7910转成的BAS-HD
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/7/31   4.5			顾振华                  创建
=============================================================================*/
BOOL CBBoardConfig::IsBrdMtHDSerial() const
{
#if defined(_LINUX12_) || defined(WIN32)
    u8 byBrdID = BrdGetBoardID();
    if ( byBrdID == BRD_KDV7810 ||    
         byBrdID == BRD_KDV7910 ||
         byBrdID == BRD_HWID_KDV7820 )
    {
        return TRUE;
    }
#endif
    return FALSE;
}

/*=============================================================================
函 数 名： NetAToNetB
功    能： A配置转到B，A清0
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/27   4.0			周广程                  创建
=============================================================================*/
void CBBoardConfig::NetAToNetB(void)
{
	m_dwDstMcuNodeB = m_dwDstMcuNodeA;
	m_dwDstMcuNodeA = INVALID_NODE;

	m_dwDstMcuIIdB = m_dwDstMcuIIdA;
	m_dwDstMcuIIdA = INVALID_INS;

	m_dwMpcIpAddrB = m_dwMpcIpAddrA;
	m_dwMpcIpAddrA = 0;

	m_wMpcPortB = m_wMpcPortA;
	m_wMpcPortA = 0;
}

/*=============================================================================
函 数 名： NetBToNetA
功    能： B配置转到A，B清0
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/27   4.0			周广程                  创建
=============================================================================*/
void CBBoardConfig::NetBToNetA(void)
{
	m_dwDstMcuNodeA = m_dwDstMcuNodeB;
	m_dwDstMcuNodeB = INVALID_NODE;

	m_dwDstMcuIIdA = m_dwDstMcuIIdB;
	m_dwDstMcuIIdB = INVALID_INS;

	m_dwMpcIpAddrA = m_dwMpcIpAddrB;
	m_dwMpcIpAddrB = 0;

	m_wMpcPortA = m_wMpcPortB;
	m_wMpcPortB = 0;
}

/*=============================================================================
函 数 名： SwitchNetAB
功    能： A、B配置倒换
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/27   4.0			周广程                  创建
=============================================================================*/
void CBBoardConfig::SwitchNetAB(void)
{
	u32 dwDstMcuNode = 0;
	u32 dwDstMcuIId = 0;
	u32 dwMpcIpAddr = 0;
	u16 wMpcPort = 0;
	
	dwDstMcuNode = m_dwDstMcuNodeA;
	m_dwDstMcuNodeA = m_dwDstMcuNodeB;
	m_dwDstMcuNodeB = dwDstMcuNode;

	dwDstMcuIId = m_dwDstMcuIIdA;
	m_dwDstMcuIIdA = m_dwDstMcuIIdB;
	m_dwDstMcuIIdB = dwDstMcuIId;

	dwMpcIpAddr = m_dwMpcIpAddrA;
	m_dwMpcIpAddrA = m_dwMpcIpAddrB;
	m_dwMpcIpAddrB = dwMpcIpAddr;

	wMpcPort = m_wMpcPortA;
	m_wMpcPortA = m_wMpcPortB;
	m_wMpcPortB = wMpcPort;
}

/*=============================================================================
函 数 名： TransPid2BrdType
功    能： pid转换成brdtype
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2010/06/01   4.0		周嘉麟                  创建
=============================================================================*/
u8 CBBoardConfig::TransPid2BrdType(u32 dwPID)
{
	u8 byBrdID = BRD_TYPE_UNKNOW;
	switch (dwPID)
	{
	case BRD_PID_KDM200_HDU:
		byBrdID = BRD_TYPE_HDU;
		break;
	case BRD_PID_KDM200_HDU_L:
		byBrdID = BRD_TYPE_HDU_L;
		break;
	case BRD_PID_KDV_EAPU:
		byBrdID = BRD_TYPE_EAPU;
		break;		
	case BRD_PID_KDV_MPU:
	case 0x043b:
		byBrdID = BRD_TYPE_MPU;
		break;
	case BRD_PID_KDV_MAU:
		break;
	case BRD_PID_KDV8000A_MPC2:
		//byBrdID = BRD_TYPE_MPC;
	//  [1/21/2011 chendaiwei] 支持MPC2单板类型上报
		byBrdID = BRD_TYPE_MPC2;
		break;
	//  [1/21/2011 chendaiwei] 支持CRI2单板类型上报
	case BRD_PID_KDV8000A_CRI2:
		//byBrdID = BRD_TYPE_CRI;
		byBrdID = BRD_TYPE_CRI2;
		break;
	//  [1/21/2011 chendaiwei] 支持DRI2单板类型上报
	case BRD_PID_KDV8000A_DRI2:
		byBrdID = BRD_TYPE_DRI2;
		break;
	//[2011/01/25 zhushz]IS2.x单板支持
	case BRD_PID_KDV8000A_IS21:
		byBrdID = BRD_TYPE_IS21;
		break;
	case BRD_PID_KDV8000A_IS22:
		byBrdID = BRD_TYPE_IS22;
		break;
	case BRD_PID_KDV8000A_HDU2:
		byBrdID = BRD_TYPE_HDU2;
		break;
	case BRD_PID_KDV8000A_HDU2_L:
		byBrdID = BRD_TYPE_HDU2_L;
		break;
	case BRD_PID_KDV8000A_MPU2:
		//TBD 判断是否有扩展子卡  [2/2/2012 chendaiwei]
		{
#if defined (_LINUX12_) && defined(_ARM_DM81_)		
			s32 swResult = BrdMpu2HasEcard();

			if(swResult == 1)
			{
				byBrdID = BRD_TYPE_MPU2ECARD;
			}
			else if (swResult == 0)
			{
				byBrdID = BRD_TYPE_MPU2;
			}
			else if (swResult == -1)
			{
				byBrdID = BRD_TYPE_UNKNOW;
				LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS,"Failed to invoke BrdMpu2HasEcard!\n");
			}
#endif
		}
		break;
	case BRD_PID_KDV8000A_APU2:
		byBrdID = BRD_TYPE_APU2;
		break;
	case BRD_PID_KDV8000A_HDU2_S:
		byBrdID = BRD_TYPE_HDU2_S;
		break;
	default:
		break;
	}
	return byBrdID;
}
/*=============================================================================
函 数 名： brdlog
功    能： 
算法实现： 
全局变量： 
参    数： char * fmt
           ...
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/28   4.0			周广程                  创建
=============================================================================*/
void CBBoardConfig::brdlog(char * fmt, va_list argptr)
{
	char PrintBuf[255];
    memset(PrintBuf, 0, sizeof(PrintBuf));
    int BufLen = 0;
	if ( m_bPrintBrdLog )
	{
		BufLen = sprintf(PrintBuf, "[BBoardAgent]:");
		BufLen += vsprintf(PrintBuf+BufLen, fmt, argptr);
		BufLen += sprintf(PrintBuf+BufLen, "\n");
		OspPrintf(TRUE, FALSE, PrintBuf);
	}
    return;
}
