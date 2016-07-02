/*****************************************************************************
模块名      : Mcu Agent
文件名      : ConfigureAgent.cpp
相关文件    : CCfgAgent.h
文件实现功能: 配置文件的读取
作者        : liuhuiyun
版本        : V4.0  Copyright( C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2005/08/17  1.0         liuhuiyun       创建
2006/04/30  4.0		   liuhuiyun	   优化
2006/11/02  4.0         张宝卿          数据存取算法优化
******************************************************************************/
#include "configureagent.h"
//#include "agentmsgcenter.h"
//#include "brdmanager.h"
#include "brdguard.h"

//#include <string.h>
//#include <stdlib.h>
//#include <time.h>

#ifdef _VXWORKS_
#include <dirent.h>
#endif

//#ifdef _LINUX_
//#include <sys/types.h>
//#include <dirent.h>
//#endif 

#ifdef WIN32
#undef _WINDOWS_ //vc6.0此文件需要afx.h，windows.h需undef
#include <afx.h>
#include <windows.h>
#include <conio.h>
#include <stdio.h>
#endif

void        AgtAPIEnableInLinux();
CCfgAgent	g_cCfgParse;  // 配置处理类
TMcuAgentDebugInfo g_tAgentDebugInfo;   // Debug信息

SEMHANDLE   g_hSync;	  // 全局同步信号量，用于进程之间同步操作

u8 g_byPrintLvl = LOG_ERROR;  // 打印级别

/*lint -save -e666*/
/*lint -save -e765*/
/*lint -save -e785*/
/*lint -save -e429*/
/*lint -save -e1536*/
/*lint -save -e506*/
/*lint -save -e845*/
/*lint -save -e1551*/
/*lint -save -esym(1788,cSemOpt)*/
/*lint -save -esym(714,g_byEthBak)*/
/*lint -save -esym(530,argptr)*/
/*lint -save -esym(552,g_byEthBak)*/

u8 g_byEthBak	= 1;		  // 是否启用网口备份 	
/*=============================================================================
函 数 名： McuAgentInit
功    能： 初始化代理
算法实现： 
全局变量： 
参    数： u8 byMcuType： MCU_TYPE_KDV8000, MCU_TYPE_KDV8000B, MCU_TYPE_KDV8000C (暂时没有使用)
返 回 值： API BOOL32 
=============================================================================*/
/*lint -save -e537*/
BOOL32 McuAgentInit(u8 byMcuType, BOOL32 bEnabled = TRUE)
{  
#undef OSPEVENT 
#define OSPEVENT( x, y ) OspAddEventDesc( #x, y )
#ifdef _EV_AGTSVC_H_
#undef _EV_AGTSVC_H_
#include "evagtsvc.h"
#define _EV_AGTSVC_H_
#else
#include "evagtsvc.h"
#undef  _EV_AGTSVC_H_
#undef _EV_AGTSVC_H_
#endif
	
	AgtAPIEnableInLinux();
	
	// 先创建全局信号量，便于进程间同步
	if( !OspSemBCreate( &g_hSync ) )
	{
		OspSemDelete( g_hSync );
		g_hSync = NULL;
		printf("[McuAgentInit] create g_hSync failed!\n" );
		return FALSE;
	}
	
    printf("[Mcu] Mcu Running on Board: %s\n", GetBoardIDStr());
    
	// 创建代理APP
	g_cMsgProcApp.CreateApp("McuAgent", AID_MCU_AGENT, APPPRI_MCUAGT, 200);
	
	// 单板管理
	g_cBrdManagerApp.CreateApp("BoardManagerApp", AID_MCU_BRDMGR, APPPRI_MCUBRDMANAGER, 100);
	
    // 单板守卫
    g_cBrdGuardApp.CreateApp("McuBrdGuard", AID_MCU_BRDGUARD, APPPRI_MCUBRDGUARD, 50);
	
    // app创建成功，分别发送对应的初始化消息
    u16 wRet = 0;
    wRet = OspPost(MAKEIID( AID_MCU_AGENT, 1), EV_MSGCENTER_POWER_ON);
    if(OSP_OK != wRet)
    {
        printf("[McuAgentInit] Fail to send power on message to MsgCenter\n");
		return FALSE;
    }
    wRet = OspPost(MAKEIID(AID_MCU_BRDMGR, CInstance::DAEMON), BOARD_AGENT_POWERON);
    if (OSP_OK != wRet)
    {
        printf("[McuAgentInit] Fail to send power on message to Brdmanager\n");
        return FALSE;
    }
	wRet = OspPost(MAKEIID(AID_MCU_BRDGUARD, 1), BOARD_GUARD_POWERON);
    if (OSP_OK != wRet)
    {
        printf("[McuAgentInit] Fail to send power on message to Brdguard\n");
        return FALSE;
    }
    
	// 设置主备板
    g_cCfgParse.SetMpcActive(bEnabled);
	
	OspSemTake( g_hSync );   // 进程同步操作
	OspSemGive( g_hSync );
	
	g_cCfgParse.PreTreatMpcNIP();  // 检查MPC的NIP是否设置了IP, zgc, 2007-07-30
	
    BOOL32 bResult = FALSE;
    bResult = g_cCfgParse.AgentInit( byMcuType );  // 代理初始化，mqs, 2011-02-26
	
	
	
    if( !bResult )       
	{
		printf("[Mcu] Agent read configuration file fully failed!\n" );
	}
	else
	{
		switch( g_cCfgParse.GetMcuCfgInfoLevel() ) 
		{
		case MCUCFGINFO_LEVEL_NEWEST:
			printf("[Mcu] Agent read configuration file succeeded!\n" );
			break;
		case MCUCFGINFO_LEVEL_LAST:
		case MCUCFGINFO_LEVEL_DEFAULT:
		case MCUCFGINFO_LEVEL_PARTNEWEST:
			printf("[Mcu] Agent read configuration file partly unsuccessful.\n");
			break;
		default:
			break;
		}
		
	}

	//单板搜索写入MPC IP，主要针对8000H/E等多IP场景
    s8 achConfigFilePath[64]={0};
	sprintf(achConfigFilePath,"%s/%s",DIR_CONFIG,"kdvipdt.ini");
	
	FILE *hFile = fopen(achConfigFilePath,"a+");
	if( hFile == NULL)
	{
		printf("[Mcuagentinit]open or create kdvipdt.ini failed!\n");
	}
	else
	{
		fclose(hFile);
	}

	u32 dwGw = g_cCfgParse.GetMpcGateway();
	//dwGw = htonl(dwGw);

	if(!SetRegKeyInt(achConfigFilePath,"Dev","GW",(s32)dwGw))
	{
		printf("[Mcuagentinit]set mcu GW failed!\n");
	}

#if defined (_8KH_) || defined (_8KE_) || defined(_8KI_)
	u32 wMpcIp = g_cCfgParse.GetLocalIp(); //网络序
	if(!SetRegKeyInt(achConfigFilePath,"Dev","IP",(s32)wMpcIp))
	{
		printf("[Mcuagentinit]set mpc ip failed!\n");
	}
	
	s8    achProfileNamePxy[MAXLEN_MCU_FILEPATH] = {0};
	sprintf(achProfileNamePxy, "%s/%s", DIR_CONFIG, PXYCFG_INI_FILENAME);
	s8 achIpAddr[32];
	memset( achIpAddr, '\0', sizeof(achIpAddr));
	g_cCfgParse.GetIpFromU32(achIpAddr, wMpcIp);
	if(SUCCESS_AGENT != g_cCfgParse.WriteStringToFile( achProfileNamePxy, "LocalInfo", "IpAddr", achIpAddr))
	{
		printf("[Mcuagentinit]write pxySercfg.ini IpAddr failed!\n");
	}

#endif
	
	return bResult;
}

/*=============================================================================
函 数 名： McuAgentQuit
功    能： 退出代理
算法实现： 
全局变量： 
参    数： void
BOOL32 bRebootBrd 是否重启所有单板
返 回 值： API BOOL32 
=============================================================================*/
BOOL32 McuAgentQuit( BOOL32 bRebootBrd )
{  
#ifndef WIN32
	
	TBrdPosition tBrdPos;
    TBoardInfo tBrdInfo;
    
	// 将连接的单板复位
    if(bRebootBrd)
    {
        for( u8 byId = 1; byId <= MAX_BOARD_NUM; byId++ )
        {
            g_cCfgParse.GetBrdCfgById(byId, &tBrdInfo);
            if( BOARD_STATUS_INLINE == tBrdInfo.GetState() )
            {
                tBrdPos.byBrdLayer = tBrdInfo.GetLayer();
                tBrdPos.byBrdSlot  = tBrdInfo.GetSlot();
                tBrdPos.byBrdID    = tBrdInfo.GetType();
                g_cCfgParse.PostMsgToBrdMgr(tBrdPos, MPC_BOARD_RESET_CMD, NULL, 0);
            }
        }
    }	
	OspDelay( 100 );// 让Osp有足够时间来分发消息
	
#endif
	
	// 发送关机TRAP
    OspPost( MAKEIID( AID_MCU_AGENT, 1), EV_MSGCENTER_POWER_OFF);
	
    g_cCfgParse.FreeAllSemHandle();
	
	return TRUE;	
}

CCfgAgent::CCfgAgent() : 
m_dwBasEntryNum(0),
m_dwMixEntryNum(0),
m_dwMpwEntryNum(0),
m_dwPrsEntryNum(0),
m_dwVMPEntryNum(0),
m_dwRecEntryNum(0),
m_dwTVWallEntryNum(0),
m_dwBrdCfgEntryNum(0),
m_dwHduEntryNum(0), 
m_dwEbapEntryNum(0),
m_dwEvpuEntryNum(0),
m_dwHduSchemeNum(0),
m_dwSvmpEntryNum(0),
m_dwDvmpEntryNum(0),
m_dwMpuBasEntryNum(0),
m_dwVmpAttachNum(0),
m_dwTrapRcvEntryNum(0),
m_byTempPortKind(0)
{
    memset( m_achCfgName, '\0', sizeof(m_achCfgName));
    memset( m_achCfgBakName, '\0', sizeof(m_achCfgBakName));
	memset( &m_tMcuPfmInfo, 0, sizeof(m_tMcuPfmInfo));
	memset( m_achCompileTime, '\0', sizeof(m_achCompileTime));
	m_byIsMcu800L = 0;
	m_byIsMcu8000H_M = 0;
	m_byEncoding = emenCoding_Utf8; // 新版本默认UTF8 [pengguofeng 4/12/2013]

	memset(&m_tProxyDMZInfo,0,sizeof(m_tProxyDMZInfo));
}

CCfgAgent::~CCfgAgent()
{
	FreeSemHandle( m_hMcuAllConfig );
}

/*=============================================================================
函 数 名： FREE_TABLE_MEM
功    能： 释放指定表的内存
算法实现： 
全局变量： 
参    数：  void **ppMem
u32 dwEntryNum
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::TableMemoryFree( void **ppMem, u32 dwEntryNum )
{
    if( NULL == ppMem)
    {
        return FALSE;
    }
    for( u32 dwLoop = 0; dwLoop < dwEntryNum; dwLoop++ )
    {
        if( NULL != ppMem[dwLoop] )
        {
            delete [] (s8*)ppMem[dwLoop];
            ppMem[dwLoop] = NULL;
        }
    }
    delete [] (s8*)ppMem;
    ppMem = NULL;
	return TRUE;
}


#define FREE_TABLE_MEM(pp, v)									\
	do{															\
		if(NULL == pp) break;									\
		for (s32 nLoop = 0; nLoop < (s32)v; nLoop++ )			\
		{														\
			if( NULL != pp[nLoop] )								\
				delete []pp[nLoop];								\
			pp[nLoop] = NULL;							\
		}														\
		delete []pp;											\
		pp = NULL;												\
	} while (0)													


/*=============================================================================
函 数 名： AgentInit
功    能： 初始化操作
算法实现： 
全局变量： 
参    数： u8 byMcuType：Mcu 类型(暂时没有使用)
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::AgentInit( u8 byMcuType )
{
    printf("CfgAgent::AgentInit 1 McuType.%d\n",byMcuType);
    TEqpBrdCfgEntry tBrdCfg;
    memset((s8*)&tBrdCfg, 0, sizeof(tBrdCfg) );
    
    if( !CreateSemHandle(m_hMcuAllConfig) ) // 创建信号量 
    {
        printf("[AgentInit] Fail to create cfg semHandle\n");
        FreeSemHandle( m_hMcuAllConfig );
		return FALSE;
    }
	
	sprintf( m_achCfgName, "%s/%s", DIR_CONFIG, MCUCFGFILENAME );
	// 备份文件名, zgc, 2007-03-20
	sprintf( m_achCfgBakName, "%s/%s", DIR_CONFIG, MCUCFGBAKFILENAME );
	
    
    FILE* hOpen = NULL;
	s32 nFopenErr = 0;
	BOOL32 bValidCfgFile = FALSE;	
    if( NULL != (hOpen = fopen(m_achCfgName, "r")) ) // exist
    {
		
		 fseek(hOpen,0,SEEK_END);
		u32 dwfilelen  = ftell(hOpen);
		if (dwfilelen > 0)
		{
			bValidCfgFile = TRUE;
		}
        fclose(hOpen); 
    }

	if ( FALSE == bValidCfgFile )
	{
#ifdef _LINUX_
		nFopenErr = errno;	
#endif
		
#ifdef WIN32
		nFopenErr = GetLastError(); 
#endif
		//记录打开mcucfg.ini失败的时间
		WriteFailedTimeForOpenMcuCfg(nFopenErr);

		BOOL32 bCreatDefaultCfgfile = TRUE;
		// 检查备份文件是否存在，如存在，首先用备份文件刷配置文件, zgc, 2007-03-20
		if( NULL != (hOpen = fopen(m_achCfgBakName, "r") ) )
		{
			fclose(hOpen);
			
			if( !CopyFileBetweenCfgAndBak( m_achCfgBakName, m_achCfgName ) )
			{
				printf("[AgentInit] Agent copy bak file to cfg file failed!\n");
			}
			else
			{
				bCreatDefaultCfgfile = FALSE;
			}
		}
		
		if( TRUE == bCreatDefaultCfgfile )
		{
			// 生成默认配置文件
			if(SUCCESS_AGENT == PretreatConfigureFile(m_achCfgName, 0) )
			{
                m_tMPCInfo.SetLocalConfigVer(VERSION_CONFIGURE40); // 4.0 版本
				printf("[AgentInit] create default config file succeed\n!");				
			}
			else
			{
				printf("[AgentInit] create default config file failed\n!");
				FreeAllSemHandle();
				return FALSE;
			}
		}
    }
	
    // guzh [4/30/2007] 首先读取Debug 文件
    AgentReadDebugInfo();
	
    // 读入配置文件
	BOOL32 bRet = FALSE;
	if( !AgentReadConfig() )  
	{
		printf("[AgentInit] AgentReadConfig failed!\n" );
		bRet = FALSE;
		
		// 设置成默认参数
		SetDefaultConfig();
        // guzh [4/30/2007] 不修改配置文件
        /*
		if( SUCCESS_AGENT != WriteAllCfgInfoToCfgfile() )
		{
		printf("[AgentInit] Agent save default cfg info to cfg file failed!\n");
		}
        */
		OspPost( MAKEIID( AID_MCU_AGENT, 1), SVC_AGT_CONFIG_ERROR );
	}
	// 增加对配置文件的备份功能, zgc, 2007-03-16
	else
	{
		//写入MPC板的配置信息（层号依赖于界面配置，不依赖于驱动接口）[8/7/2013 chendaiwei]
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_) && defined(_LINUX_) && !defined(_LINUX12_)
		s8 achMcuType[64] ={0};
		s8 achMcuAlias[64] = {0};
		strcpy(achMcuType,"MPC");
		u8 byBrdTypeStrLen = strlen(achMcuType);
		memcpy(&achMcuAlias[0],achMcuType,byBrdTypeStrLen);	
		sprintf(&achMcuAlias[byBrdTypeStrLen],"(%d层%d槽)",m_tMPCInfo.GetLocalLayer(),m_tMPCInfo.GetLocalSlot());
		
		s8 achConfigFilePath[64]={0};
		sprintf(achConfigFilePath,"%s/%s",DIR_CONFIG,"kdvipdt.ini");
		
		FILE *hFile = fopen(achConfigFilePath,"a+");
		if( hFile == NULL)
		{
			printf("[Agentinit]open or create kdvipdt.ini failed!\n");
			
			return FALSE;
		}
		else
		{
			fclose(hFile);
		}
		
		if(!SetRegKeyString(achConfigFilePath,"Dev","TypeAlias",achMcuType))
		{
			printf("[Agentinit]set type alis failed!\n");
			
			return FALSE;
		}
		
		if(!SetRegKeyString(achConfigFilePath,"Dev","Alias",achMcuAlias))
		{
			printf("[WriteBoardInfoToDetecteeFile]set device alis failed!\n");
			
			return FALSE;
		}
#endif

#if defined(_8KE_) || defined(_8KH_)
		TMultiManuNetAccess tMultiManuNetAccess;
		//GetMultiManuNetAccess(tMultiManuNetAccess);
		WriteMultiManuNetAccess(tMultiManuNetAccess);

		LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AgentInit] write multimanu from multimanucfg.ini to mcucfg.ini\n");
#endif

#ifdef _8KI_
		WritePxyMultiNetInfo();
#endif

		bRet = TRUE;
		if( MCUCFGINFO_LEVEL_NEWEST == m_tMcuSystem.GetMcuCfgInfoLevel() )
		{
#ifdef _MINIMCU_
#ifndef _VXWORKS_
			u8 byLop0, byLop1;
			TMINIMCUNetParamAll tMINIMCUNetParamAll;
			TMINIMCUNetParam tNetParam;
			TBrdEthParamAll tEthParamAll, tAgtCfgParamAll;
			memset( &tEthParamAll, 0, sizeof(tEthParamAll) );
			memset( &tAgtCfgParamAll, 0, sizeof(tAgtCfgParamAll) );
			
			// 检验IP
			AgtGetBrdEthParamAll( 0, &tEthParamAll );
			m_tMcuDscInfo.GetMcsAccessAddrAll( tMINIMCUNetParamAll );
			tAgtCfgParamAll.dwIpNum = tMINIMCUNetParamAll.GetNetParamNum();
			for ( byLop0 = 0; byLop0 < tAgtCfgParamAll.dwIpNum ; byLop0++ )
			{
				memset( &tNetParam, 0, sizeof(tNetParam) );
				tMINIMCUNetParamAll.GetNetParambyIdx( byLop0, tNetParam );
				tAgtCfgParamAll.atBrdEthParam[byLop0].dwIpAdrs = htonl( tNetParam.GetIpAddr() );
				tAgtCfgParamAll.atBrdEthParam[byLop0].dwIpMask = htonl( tNetParam.GetIpMask() );
			}
			if ( tEthParamAll.dwIpNum != tAgtCfgParamAll.dwIpNum )
			{
				printf("[AgentInit] ip num.%d in cfg file is not equal to NIP num.%d!\n", tAgtCfgParamAll.dwIpNum, tEthParamAll.dwIpNum);
				m_tMcuDscInfo.cosPrint();
				if ( !SaveDscLocalInfoToNip( &m_tMcuDscInfo ) )
				{
					printf("[AgentInit] Save cfg ip failed!\n");
				}
			}
			else
			{
				BOOL32 bEqual = TRUE;
				for ( byLop0 = 0; byLop0 < tEthParamAll.dwIpNum; byLop0++ )
				{
					for ( byLop1 = 0; byLop1 < tAgtCfgParamAll.dwIpNum; byLop1++ )
					{
						if ( tEthParamAll.atBrdEthParam[byLop0].dwIpAdrs == tAgtCfgParamAll.atBrdEthParam[byLop1].dwIpAdrs 
							&& tEthParamAll.atBrdEthParam[byLop0].dwIpMask == tAgtCfgParamAll.atBrdEthParam[byLop1].dwIpMask )
						{
							break;
						}
					}
					if ( byLop1 == tAgtCfgParamAll.dwIpNum )
					{
						bEqual = FALSE;
						break;
					}
				}
				if ( bEqual == FALSE )
				{
					printf("[AgentInit] ip in cfg file is not equal to NIP!\n");
					m_tMcuDscInfo.cosPrint();
					if ( !SaveDscLocalInfoToNip( &m_tMcuDscInfo ) )
					{
						printf("[AgentInit] Save cfg ip failed!\n");
					}
				}
			}// if ( tEthParamAll.dwIpNum != tAgtCfgParamAll.dwIpNum ) else
			
			// 检验路由
			TBrdAllIpRouteInfo tBrdAllIpRouteInfo;
			BrdGetAllIpRoute( &tBrdAllIpRouteInfo );
			BOOL32 bFirstWan = TRUE;
			for ( byLop0 = 0; byLop0 < tMINIMCUNetParamAll.GetNetParamNum(); byLop0++ )
			{
				memset( &tNetParam, 0, sizeof(tNetParam) );
				tMINIMCUNetParamAll.GetNetParambyIdx( byLop0, tNetParam );
				if ( tNetParam.IsLan() )
				{
					if (NETTYPE_LAN == m_tMcuDscInfo.GetNetType() 
						|| NETTYPE_MIXING_ALLPROXY == m_tMcuDscInfo.GetNetType()
						|| NETTYPE_MIXING_NOTALLPROXY == m_tMcuDscInfo.GetNetType())
					{
						if (  ( 0 == BrdGetDefGateway() )
							|| ( 0 != BrdGetDefGateway() && htonl(tNetParam.GetGatewayIp()) != BrdGetDefGateway() )
							)
						{
							if ( !SaveRouteToNipByDscInfo( &m_tMcuDscInfo ) )
							{
								printf("[AgentInit] Save cfg route failed!\n");
							}
							break;
						}
					}
					else
					{
						OspPrintf(TRUE, FALSE, "[AgentInit] NetParam.%d nettype.%d is not fit to Dsc nettype.%d!\n",
							byLop0, tNetParam.GetNetworkType(), m_tMcuDscInfo.GetNetType() );
					}
				}
				else if ( tNetParam.IsWan() && bFirstWan == TRUE )
				{
					if (NETTYPE_WAN == m_tMcuDscInfo.GetNetType() 
						|| NETTYPE_MIXING_ALLPROXY == m_tMcuDscInfo.GetNetType()
						|| NETTYPE_MIXING_NOTALLPROXY == m_tMcuDscInfo.GetNetType())
					{						
						if (  ( 0 == BrdGetDefGateway() )
							|| ( 0 != BrdGetDefGateway() && htonl(tNetParam.GetGatewayIp()) != BrdGetDefGateway() )
							)
						{
							if ( !SaveRouteToNipByDscInfo( &m_tMcuDscInfo ) )
							{
								printf("[AgentInit] Save cfg route failed!\n");
							}
							break;
						}
						bFirstWan = FALSE;
					}
					else
					{
						OspPrintf(TRUE, FALSE, "[AgentInit] NetParam.%d nettype.%d is not fit to Dsc nettype.%d!\n",
							byLop0, tNetParam.GetNetworkType(), m_tMcuDscInfo.GetNetType() );
					}
				}
				else
				{
					OspPrintf(TRUE, FALSE, "[AgentInit] The netparam nettype is invalid, it's impossible!\n");
				}// if ( bEqual == FALSE ...
			}// if ( tEthParamAll.dwIpNum != tAgtCfgParamAll.dwIpNum ) else
			//#endif
#else 
			u8 byLop;
			TMINIMCUNetParamAll tMINIMCUNetParamAll;
			TMINIMCUNetParam tNetParam;
			TBrdEthParam tBrdEthParam;
			memset( &tBrdEthParam, 0, sizeof(tBrdEthParam) );
			memset( &tNetParam, 0, sizeof(tNetParam) );
			
			AgtGetBrdEthParam( 0, &tBrdEthParam );
			m_tMcuDscInfo.GetMcsAccessAddrAll( tMINIMCUNetParamAll );
			if ( tMINIMCUNetParamAll.GetNetParambyIdx( 0, tNetParam ) )
			{
				if ( tBrdEthParam.dwIpAdrs != htonl(tNetParam.GetIpAddr()) 
					|| tBrdEthParam.dwIpMask != htonl(tNetParam.GetIpMask()) )
				{
					printf("[AgentInit] ip in cfg file is not equal to NIP!\n");
					if ( !SaveDscLocalInfoToNip( &m_tMcuDscInfo ) )
					{
						printf("[AgentInit] Save cfg ip failed!\n");
					}
				}
			}
#endif
#endif //#ifdef _MINIMCU_
			if( !CopyFileBetweenCfgAndBak( m_achCfgName , m_achCfgBakName ) )
			{
				printf("[AgentInit] Agent save cfg bak file failed!\n");
			}
		}
		else
		{
			if( SUCCESS_AGENT != WriteAllCfgInfoToCfgfile() )
			{
				printf("[AgentInit] Agent save cfg info to cfg file failed!\n");
			}
		}
	}
	
	
	printf("CfgAgent::AgentInit 2\n");
    
	// 启动snmp服务
    OspPost( MAKEIID( AID_MCU_AGENT, 1), EV_TOMSGCENTER_INITAL_SNMPINFO ); 
	
	// 创建FTP目录
	AgentDirCreate( DIR_FTP );
	
	// 发送开机TRAP
	printf("AgentInit- 发送开机TRAP\n");
	OspSemTake( g_hSync );
	OspPost( MAKEIID( AID_MCU_AGENT, 1), EV_AGENT_COLD_RESTART );
	OspSemGive( g_hSync );
	
	// 所有配置的单板按不在线, 发送TRAP
	u16 wLoop;
	TBrdStatus tBrdStatus;
	for( wLoop = 0; wLoop < m_dwBrdCfgEntryNum; wLoop++ )
	{
		//  [1/21/2011 chendaiwei]支持MPC2
		if( m_atBrdCfgTable[wLoop].GetType() != BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ &&
			m_atBrdCfgTable[wLoop].GetType() != BRD_TYPE_MPC2 ) /*不是MPC*/
		{
			// 通知前台代理，要告警！
			tBrdStatus.byLayer = m_atBrdCfgTable[wLoop].GetLayer();
			tBrdStatus.bySlot = m_atBrdCfgTable[wLoop].GetSlot();
			tBrdStatus.byType = m_atBrdCfgTable[wLoop].GetType();
			tBrdStatus.byStatus = BOARD_STATUS_DISCONNECT;
            tBrdStatus.byOsType = 0;
			
			OspPost( MAKEIID( AID_MCU_AGENT, 1), 
				MCU_AGT_BOARD_STATUSCHANGE, (u8*)&tBrdStatus, sizeof(tBrdStatus) );
		}
		else
		{
			TBrdLedState tBrdLedState = {0};
			TBrdPosition tPos = {0};
            // [20120718 liaokang] 防止主备倒换后，备板转为主用板的情况下，NMS获取LED值不对
//          if( m_tMPCInfo.GetIsLocalMaster() )
//          {
            AgtQueryBrdPosition( &tPos );
            
            if(
  	        #ifdef _LINUX12_
                tPos.byBrdID == BRD_HWID_MPC2 &&					
			#else
                tPos.byBrdID == m_atBrdCfgTable[wLoop].GetType() &&
			#endif
                tPos.byBrdLayer == m_atBrdCfgTable[wLoop].GetLayer() &&
                tPos.byBrdSlot == m_atBrdCfgTable[wLoop].GetSlot() )
            {
                if (OK != BrdQueryLedState(&tBrdLedState)) 
                {
                    printf("[AgentInit] BrdQueryLedState failed !\n");
                }
            #ifdef _LINUX12_ 
                m_atBrdCfgTable[wLoop].SetPanelLed( (s8*)&tBrdLedState.nlunion.tBrdMPC2LedState );
            #else
                m_atBrdCfgTable[wLoop].SetPanelLed( (s8*)&tBrdLedState.nlunion.tBrdMPCLedState );
            #endif
            }
//          }
//          else
//          {
// 				if (OK != BrdQueryLedState(&tBrdLedState)) 
// 				{
// 				 	printf("[AgentInit] BrdQueryLedState failed !\n");
// 				}
// 				#ifdef _LINUX12_ 
// 					m_atBrdCfgTable[wLoop].SetPanelLed( (s8*)&tBrdLedState.nlunion.tBrdMPC2LedState );
// 				#else  
// 					m_atBrdCfgTable[wLoop].SetPanelLed( (s8*)&tBrdLedState.nlunion.tBrdMPCLedState );
// 				#endif	
// 			}
		}
	}
	
	// 点亮MPC板上NMS灯 miaoqingsong 20101118 add
	OspSemTake( g_hSync );
	OspPost( MAKEIID( AID_MCU_AGENT, 1), MCUAGENT_OPENORCLOSE_MPCLED_TIMER );
    OspSemGive( g_hSync );
	
#if (defined(_8KE_) || defined(_8KH_)) && defined(_LINUX_)
	//apache server config
	s8 achFileName[MAXLEN_MCU_FILEPATH] = {0};
	sprintf(achFileName, "%s", APACHE_CONFIG_SCRIPT );
	if (0 == access (achFileName, F_OK))
	{
		
		chmod(achFileName, S_IREAD|S_IWRITE|S_IEXEC);
		s8    achFile[MAXLEN_MCU_FILEPATH] = {0};
		sprintf(achFile, "cd /opt/mcu; %s", achFileName);
		s32 nRetsystem = system(achFile);
		printf("system(%s) res: %d\n", achFile, nRetsystem);
	}
	else
	{
		printf("%s doesn't exist!\n", achFileName);
	}
	
#endif
	
	printf("AgentInit--end\n");
	return bRet;
}

/*=============================================================================
函 数 名： CreateSemHandle
功    能： 创建信号量
算法实现： 
全局变量： 
参    数： SEMHANDLE &hSem
返 回 值： void 
=============================================================================*/
BOOL32 CCfgAgent::CreateSemHandle( SEMHANDLE &hSem )
{
    BOOL32 bRet = TRUE;
	
	// 创建信号量
	if( !OspSemBCreate(&hSem) )
	{
		OspSemDelete( hSem );
		hSem = NULL;
        printf("[CreateSemHandle] create handle failed!\n" );
		bRet = FALSE;
	}
    return bRet;
}

/*=============================================================================
函 数 名： FreeSemHandle
功    能： 释放信号量
算法实现： 
全局变量： 
参    数： SEMHANDLE &hSem
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::FreeSemHandle( SEMHANDLE &hSem )
{
    // 删除信号量
	if( NULL != hSem )
	{
		OspSemDelete( hSem );
		hSem = NULL;
	}
    return TRUE;
}

/*=============================================================================
函 数 名： FreeAllSemHandle
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/20  4.0			周广程                  创建
=============================================================================*/
BOOL32 CCfgAgent::FreeAllSemHandle()
{
	FreeSemHandle( m_hMcuAllConfig );
	return TRUE;
}

/*=============================================================================
函 数 名： GetBoardNum
功    能： 得到单板数目
算法实现： 
全局变量： 
参    数： void
返 回 值： u32 
=============================================================================*/
u32 CCfgAgent::GetBoardNum(void)
{
    return m_dwBrdCfgEntryNum;
}

/*=============================================================================
函 数 名： HasSnmpNms
功    能： 
算法实现： 
全局变量： 
参    数： void
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::HasSnmpNms( void ) const
{
	return ( 0 != m_dwTrapRcvEntryNum );
}

/*=============================================================================
函 数 名： GetSnmpParam
功    能： 
算法实现： 
全局变量： 
参    数： TSnmpAdpParam& tParam
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::GetSnmpParam( TSnmpAdpParam& tParam ) const
{
	BOOL32 bRet = FALSE;
    memset( &tParam, 0, sizeof(tParam) );
	
	if( 0 != m_dwTrapRcvEntryNum )
	{
        tParam.dwLocalIp = htonl(m_atTrapRcvTable[0].GetTrapIp());
		
		TTrapInfo tTrapInfo;
		memcpy( &tTrapInfo, &m_atTrapRcvTable[0], sizeof(TTrapInfo) );
		
        strncpy( tParam.achReadCommunity, tTrapInfo.GetReadCom(), MAX_COMMUNITY_LEN );
		
        tParam.achReadCommunity[MAX_COMMUNITY_LEN-1] = '\0';
		
        strncpy( tParam.achWriteCommunity, tTrapInfo.GetWriteCom(), MAX_COMMUNITY_LEN );
		
		tParam.achWriteCommunity[MAX_COMMUNITY_LEN-1] = '\0';
		
        tParam.wGetSetPort = m_atTrapRcvTable[0].GetGSPort();
        tParam.wTrapPort = m_atTrapRcvTable[0].GetTrapPort();
		
		printf("GetSnmpParam succeed!\n");
		
		bRet = TRUE;
	}
	else
	{
		memset(&tParam, 0, sizeof(tParam));
	}
	
	return bRet;
}

/*=============================================================================
函 数 名： SetDefaultConfig
功    能： 设置为默认配置(存在参数错误时)
算法实现： 
全局变量： 
参    数： 
返 回 值：
==============================================================================*/
void CCfgAgent::SetDefaultConfig(void)
{
	s8 achTime[MAX_CONFIGURE_LENGTH];
	memset(achTime, 0, sizeof(achTime));
	
	s8 achProfileName[KDV_MAX_PATH];
	memset(achProfileName, 0, sizeof(KDV_MAX_PATH));
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, MCUCFGFILENAME);
	
	m_dwTrapRcvEntryNum = 0;
	m_dwTVWallEntryNum = 0;	
	m_dwMixEntryNum = 0;	
	m_dwRecEntryNum = 0;	
	m_dwBasEntryNum = 0;
	m_dwVMPEntryNum = 0;	
	m_dwHduEntryNum = 0;
	m_dwHduSchemeNum = 0;
	m_dwSvmpEntryNum = 0;
	m_dwDvmpEntryNum = 0;
	m_dwEvpuEntryNum = 0;
	m_dwEbapEntryNum = 0;
	m_dwBrdCfgEntryNum = 0;	
	
	
	// 不能配置版本号为当前时间，防止RTC时钟坏导致错误　
	sprintf( achTime, "%s Basic Config", MCUAGT_SYS_CFGVER );
    m_tMcuSystem.SetConfigVer( achTime );
	
	// 本地信息
	m_tMcuLocalInfo.SetMcuId( 192 );
    m_tMcuLocalInfo.SetAlias( "KEDA_MCU" );
    m_tMcuLocalInfo.SetE164Num( "8651264273989" );
	
	// 网络组
	m_tMcuNetwork.SetGKIp( 0 );
    m_tMcuNetwork.SetIsGKCharge( FALSE );
    m_tMcuNetwork.SetRRQMtadpIp( 0 );
	m_tMcuNetwork.SetCastIp( 0 );
	m_tMcuNetwork.SetCastPort( MULTICAST_DEFPORT );
	m_tMcuNetwork.SetRecvStartPort( MT_MCU_STARTPORT );
	m_tMcuNetwork.SetMpcTransData( 0 ); // guzh [1/16/2007] 默认不支持
	m_tMcuNetwork.SetMpcStack( TRUE );
    m_tMcuNetwork.Set225245StartPort( MCU_H225245_STARTPORT );
	m_tMcuNetwork.SetMTUSize( DEFAULT_MTU_SIZE ); // MTU长度, zgc, 2007-04-05

	m_tMcuNetwork.SetGkRRQUsePwdFlag(0);
	m_tMcuNetwork.SetGkRRQPassword(NULL);
	
    // 单板配置
	m_dwBrdCfgEntryNum = 1;
	m_atBrdCfgTable[0].SetBrdId( 1 );
	
    TBrdPosition tMpcPosition;
    
	//#ifndef WIN32 // vx linux	
	if (OK != AgtQueryBrdPosition(&tMpcPosition)) 
    {
        printf("[SetDefaultConfig] AgtQueryBrdPosition failed !\n");
	}
    else
    {
        Agtlog(LOG_INFORM, "[SetDefaultConfig] The Mpc Layer: %d, Slot:%d, Id: %d, Active:%d\n", 
			tMpcPosition.byBrdLayer, tMpcPosition.byBrdSlot, tMpcPosition.byBrdID, IsMpcActive());
    }
	
#ifdef _MINIMCU_
    m_tMPCInfo.SetLocalPortChoice(0);   // 前网口
#else
    // 对于创建默认配置文件的情况，由于不知道用户希望的MPC的IP，
	// 因此只能优先选择eth1，在eth1没有配置IP的情况下，再检查eth0, zgc, 2007-07-12
#ifndef _VXWORKS_
	// 先查eth1
	STATUS nRet = 0;
	TBrdEthParamAll tBrdEthParamAll;
	nRet = AgtGetBrdEthParamAll( 1, &tBrdEthParamAll ); 
	if ( nRet == ERROR )
	{
		printf( "[SetDefaultConfig] Get eth1 params failed!\n" );
	}
	else if ( tBrdEthParamAll.dwIpNum > 0 && tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs !=0 )
	{
		m_tMPCInfo.SetLocalPortChoice( 1 );
	}
	
	if ( nRet == ERROR 
		|| tBrdEthParamAll.dwIpNum == 0 
		|| (tBrdEthParamAll.dwIpNum > 0 && tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs == 0) )
	{
		// 再查eth0
		memset( &tBrdEthParamAll, 0, sizeof(tBrdEthParamAll) );
		nRet = AgtGetBrdEthParamAll( 0, &tBrdEthParamAll ); 
		if ( nRet == ERROR )
		{
			printf( "[SetDefaultConfig] Get eth0 params failed!\n" );
		}
		else if ( tBrdEthParamAll.dwIpNum > 0 && tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs !=0 )
		{
			m_tMPCInfo.SetLocalPortChoice( 0 );
		}
		
		if ( nRet == ERROR 
			|| tBrdEthParamAll.dwIpNum == 0 
			|| (tBrdEthParamAll.dwIpNum > 0 && tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs == 0) )
		{
			printf( "[SetDefaultConfig] MPC ip is not correct!\n" );
			m_tMPCInfo.SetLocalPortChoice( 1 );
		}
	}
#else
	//#ifdef _VXWORKS_
	// 先查eth1
	s32 nRet;
	TBrdEthParam tOneParam;
	memset(&tOneParam, 0, sizeof(tOneParam));
	nRet = AgtGetBrdEthParam( 1, &tOneParam );
	if ( nRet == ERROR )
	{
		printf( "[SetDefaultConfig] Get eth0 param failed!\n" );
	}
	else if ( tOneParam.dwIpAdrs != 0 )
	{
		m_tMPCInfo.SetLocalPortChoice( 1 );
	}
	
	if ( nRet == ERROR || tOneParam.dwIpAdrs == 0 ) 
	{
		//再查eth0
		memset(&tOneParam, 0, sizeof(tOneParam));
		nRet = AgtGetBrdEthParam( 0, &tOneParam );
		if ( nRet == ERROR )
		{
			printf( "[SetDefaultConfig] Get eth0 param failed!\n" );
		}
		else if ( tOneParam.dwIpAdrs != 0 )
		{
			m_tMPCInfo.SetLocalPortChoice( 0 );
		}
		
		if ( nRet == ERROR || tOneParam.dwIpAdrs == 0 ) 
		{
			printf( "[SetDefaultConfig] MPC ip is not correct!\n" );
			m_tMPCInfo.SetLocalPortChoice( 1 );
		}
	}
#endif
	//	#ifdef WIN32
	//		m_tMPCInfo.SetLocalPortChoice( 1 );
	//	#endif
#endif
	
	if(tMpcPosition.byBrdLayer >= 4)
	{
		printf("[SetDefaultConfig] tMpcPosition's layer.%d change to 0!\n ", tMpcPosition.byBrdLayer);
		tMpcPosition.byBrdLayer = 0;
	}
	m_atBrdCfgTable[0].SetBrdIp( ntohl(GetLocalIp()) );
	m_atBrdCfgTable[0].SetLayer(tMpcPosition.byBrdLayer);
	m_atBrdCfgTable[0].SetSlot(tMpcPosition.byBrdSlot);
	//#ifndef WIN32
	
	//  [1/21/2011 chendaiwei]支持MPC2
	u32 dwBoardID = BrdGetBoardID();
#ifdef _LINUX12_
	if( dwBoardID == BRD_HWID_MPC2 )
	{
		dwBoardID = BRD_TYPE_MPC2/*DSL8000_BRD_MPC2*/;
	}
#endif
	
	m_atBrdCfgTable[0].SetType(u8(dwBoardID));
	//#else
	//	m_atBrdCfgTable[0].SetType(DSL8000_BRD_MPC);
	//#endif
	
	m_atBrdCfgTable[0].SetPortChoice( m_tMPCInfo.GetLocalPortChoice() );
	m_atBrdCfgTable[0].SetState( BOARD_STATUS_INLINE );
	
    m_atBrdCfgTable[0].SetVersion( GetMcuSoftVersion());
	if(BRD_TYPE_MPC2 == dwBoardID)
	{
		m_atBrdCfgTable[0].SetAlias(BRD_ALIAS_MPC2);
	}
	else
	{
		m_atBrdCfgTable[0].SetAlias(BRD_ALIAS_MPC);
	}
	
    m_tNetSync.SetMode( NETSYNTYPE_VIBRATION );
	
    ///FIXME: DSC MODULE Info ?
#ifdef _MINIMCU_
	m_tMcuDscInfo.SetStartMp( TRUE );
	m_tMcuDscInfo.SetStartMtAdp( TRUE );
	m_tMcuDscInfo.SetStartProxy( TRUE );
	m_tMcuDscInfo.SetStartDcs( TRUE );
	m_tMcuDscInfo.SetStartGk( TRUE );
	m_tMcuDscInfo.SetDscInnerIp( ntohl(INET_ADDR(DEFAULT_DSC_INNERIP)) );
#endif
	
    return;
}
/*=============================================================================
函 数 名： AgentReadDebugInfo
功    能： 代理读取Debug文件
算法实现： 
全局变量： 
参    数： 
返 回 值： 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2007/04/30  4.0         顾振华       创建
=============================================================================*/
void CCfgAgent::AgentReadDebugInfo(void)
{
    s8  achDebugFileName[64];    
    s32 nValue;
	
    sprintf(achDebugFileName, "%s/%s", DIR_CONFIG, MCUDEBUGFILENAME);    
	
    GetRegKeyInt( achDebugFileName, "mcuNetWork", "mcuListenPort", MCU_LISTEN_PORT, &nValue );
    g_tAgentDebugInfo.SetListenPort((u16)nValue);
	
    GetRegKeyInt( achDebugFileName, "mcuNetWork", "mcuBrdHeartBeatTime", DEF_OSPDISC_HBTIME, &nValue );
    // 至少3秒
    if ( nValue >= 3 )
    {
        g_tAgentDebugInfo.SetBoardDiscTimeSpan((u16)nValue);
    }
    
    GetRegKeyInt( achDebugFileName, "mcuNetWork", "mcuBrdHeartBeatNum", DEF_OSPDISC_HBNUM, &nValue );
    // 至少1次
    if (nValue >= 1 )
    {
        g_tAgentDebugInfo.SetBoardDiscTimes((u8)nValue);
    }    
	
#ifndef WIN32
	
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_) || defined(_MINIMCU_)
	GetRegKeyInt(achDebugFileName, "mcuAbilityParam", "IsMpcRunMp", 1, &nValue);
#else
	GetRegKeyInt(achDebugFileName, "mcuAbilityParam", "IsMpcRunMp", 0, &nValue);
#endif
	BOOL32 bRunMp = (nValue!=0)?TRUE:FALSE;
    g_tAgentDebugInfo.SetMpcRunMp(bRunMp);
	
#else
	GetRegKeyInt(achDebugFileName, "mcuAbilityParam", "IsMpcRunMp", 1, &nValue);
	BOOL32 bRunMp =(nValue!=0)?TRUE:FALSE;
    g_tAgentDebugInfo.SetMpcRunMp(bRunMp );
#endif
    
#ifndef WIN32
	
#if defined(_MINIMCU_) || defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	GetRegKeyInt(achDebugFileName, "mcuAbilityParam", "IsMpcRunMtAdp", 1, &nValue);
#else	
	GetRegKeyInt(achDebugFileName, "mcuAbilityParam", "IsMpcRunMtAdp", 0, &nValue);
#endif
    g_tAgentDebugInfo.SetMpcRunMtAdp( (nValue!=0)?TRUE:FALSE );
	
#else
	GetRegKeyInt(achDebugFileName, "mcuAbilityParam", "IsMpcRunMtAdp", 1, &nValue);
    g_tAgentDebugInfo.SetMpcRunMtAdp( nValue!=0 );
	
#endif
}


/*=============================================================================
函 数 名： AgentReadConfig
功    能： 代理读取配置
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::AgentReadConfig(void)
{
	//    s8   achProfileName[256];
	//	memset( achProfileName, 0, sizeof(achProfileName) );
	//   sprintf( achProfileName, "%s/%s", DIR_CONFIG, MCUCFGFILENAME );
	
	ENTER( m_hMcuAllConfig );
	
	// 多国语言 [pengguofeng 4/12/2013]
	u8 byEncodingType = 0;
	u16 wRet = ReadMcuEncodingType(byEncodingType);
	if ( wRet == SUCCESS_AGENT )
	{
		StaticLog("set mcu encoding:%d from file\n", byEncodingType);
		SetEncodingType(byEncodingType);
	}
	else
	{
		StaticLog("set mcu encoding:GBK\n");
		SetEncodingType(emenCoding_GBK);
	}

	BOOL32 bResult = TRUE;
    bResult = AgentGetSystemInfo( m_achCfgName );
	//增加使用备份文件进行保护, zgc, 2007-03-20
	bResult &= AgentGetBrdCfgTable( m_achCfgName );
	if( !bResult )
	{
		bResult = PreReadCfgfile( m_achCfgBakName );
		if( bResult )
		{
			bResult = CopyCfgSrcfileToDstfile( m_achCfgBakName, m_achCfgName );
			if( bResult )
			{
				bResult = AgentGetSystemInfo( m_achCfgName );
				bResult &= AgentGetBrdCfgTable( m_achCfgName );
				if( !bResult )
				{
					printf( "[AgentReadConfig] Read cfg file entirely failed! Use default config!\n" );
					m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_DEFAULT );
					return FALSE;
				}
				printf( "[AgentReadConfig] Read cfg file failed! Use last file config!\n" );
				m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_LAST );
			}
			else
			{
				printf( "[AgentReadConfig] Read cfg file entirely failed! Use default config!\n" );
				m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_DEFAULT );
				return FALSE;
			}
		}
		else
		{
			printf( "[AgentReadConfig] Read cfg file entirely failed! Use default config!\n" );
			m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_DEFAULT );
			return FALSE;
		}
	}
	
	bResult = AgentGetLocalInfo( m_achCfgName );
	//增加使用备份文件进行保护, zgc, 2007-03-20
	if( !bResult )
	{
		bResult = AgentGetLocalInfo( m_achCfgBakName );
		//zjj20100116 以下函数注掉,因为以上函数读文件若无法读到某个项时已经给出默认值
		/*
		if( !bResult )
		{
		SetLocalInfoByDefault();
		}
		*/
		printf("[AgentReadConfig] Read localinfo failed!\n");
		m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_PARTNEWEST );
	}
	
	bResult = AgentGetEqpExCfgInfo( m_achCfgName );
	if( !bResult )
	{
		bResult = AgentGetEqpExCfgInfo( m_achCfgBakName );
		if( !bResult )
		{
			SetEqpExCfgInfoByDefault();
		}
		printf("[AgentReadConfig] Read EqpExCfgInfo failed!\n");
		m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_PARTNEWEST );
	}
	
	bResult = AgentGetNetworkInfo( m_achCfgName );
	//增加使用备份文件进行保护, zgc, 2007-03-20
	if( !bResult )
	{
		bResult = AgentGetNetworkInfo( m_achCfgBakName );
		if( !bResult )
		{
			SetNetWorkInfoByDefault();
		}
		printf("[AgentReadConfig] Read networkinfo failed!\n");
		m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_PARTNEWEST );
	}
	
    bResult = AgentGetTrapRcvTable( m_achCfgName );
	//增加使用备份文件进行保护, zgc, 2007-03-20
	if( !bResult )
	{
		bResult = AgentGetTrapRcvTable( m_achCfgBakName );
		if( !bResult )
		{
			SetTrapRcvTableByDefault();
		}
		printf("[AgentReadConfig] Read traprcvtable failed!\n");
		m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_PARTNEWEST );
	}
	
	bResult = AgentGetMixerTable( m_achCfgName );
	//增加使用备份文件进行保护, zgc, 2007-03-20
	if( !bResult )
	{
		bResult = AgentGetMixerTable( m_achCfgBakName );
		if( !bResult )
		{
			SetMixerTableByDefault();
		}
		printf("[AgentReadConfig] Read mixertable failed!\n");
		m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_PARTNEWEST );
	}
	
	bResult = AgentGetRecorderTable( m_achCfgName );
	//增加使用备份文件进行保护, zgc, 2007-03-20
	if( !bResult )
	{
		bResult = AgentGetRecorderTable( m_achCfgBakName );
		if( !bResult )
		{
			SetRecorderTableByDefault();
		}
		printf("[AgentReadConfig] Read recordertable failed!\n");
		m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_PARTNEWEST );
	}
	
	bResult = AgentGetTVWallTable( m_achCfgName );
	//增加使用备份文件进行保护, zgc, 2007-03-20
	if( !bResult )
	{
		bResult = AgentGetTVWallTable( m_achCfgBakName );
		if( !bResult )
		{
			SetTVWallTableByDefault();
		}
		printf("[AgentReadConfig] Read tvwalltable failed!\n");
		m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_PARTNEWEST );
	}
	
	bResult = AgentGetBasTable( m_achCfgName );
	//增加使用备份文件进行保护, zgc, 2007-03-20
	if( !bResult )
	{
		bResult = AgentGetBasTable( m_achCfgBakName );
		if( !bResult )
		{
			SetBasTableByDefault();
		}
		printf("[AgentReadConfig] Read bastable failed!\n");
		m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_PARTNEWEST );
	}
	
    //zw[08/07/2008]
    bResult = AgentGetBasHDTable( m_achCfgName );
	//增加使用备份文件进行保护, zgc, 2007-03-20
	if( !bResult )
	{
		bResult = AgentGetBasHDTable( m_achCfgBakName );
		if( !bResult )
		{
			SetBasHDTableByDefault();
		}
		printf("[AgentReadConfig] Read basHDtable failed!\n");
		m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_PARTNEWEST );
	}
	
	bResult = AgentGetVMPTable( m_achCfgName );
	//增加使用备份文件进行保护, zgc, 2007-03-20
	if( !bResult )
	{
		bResult = AgentGetVMPTable( m_achCfgBakName );
		if( !bResult )
		{
			SetVMPTableByDefault();
		}
		printf("[AgentReadConfig] Read vmptable failed!\n");
		m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_PARTNEWEST );
	}
	//4.6版本新加 jlb  ---------  start     ----------
	bResult = AgentGetHduTable( m_achCfgName );
	//增加使用备份文件进行保护, zgc, 2007-03-20
	if( !bResult )
	{
		bResult = AgentGetHduTable( m_achCfgBakName );
		if( !bResult )
		{
			SetHduTableByDefault();
		}
		printf("[AgentReadConfig] Read hdu table failed!\n");
		m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_PARTNEWEST );
	}
	
	bResult = AgentGetSvmpTable( m_achCfgName );
	//增加使用备份文件进行保护, zgc, 2007-03-20
	if( !bResult )
	{
		bResult = AgentGetSvmpTable( m_achCfgBakName );
		if( !bResult )
		{
			SetVMPTableByDefault();
		}
		printf("[AgentReadConfig] Read Svmp table failed!\n");
		m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_PARTNEWEST );
	}
	
	//bResult = AgentGetDvmpTable( m_achCfgName );
	//增加使用备份文件进行保护, zgc, 2007-03-20
	// 	if( !bResult )
	// 	{
	// 		bResult = AgentGetDvmpTable( m_achCfgBakName );
	// 		if( !bResult )
	// 		{
	// 			SetVMPTableByDefault();
	// 		}
	// 		printf("[AgentReadConfig] Read Dvmp table failed!\n");
	// 		m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_PARTNEWEST );
	// 	}
	
	bResult = AgentGetMpuBasTable( m_achCfgName );
	//增加使用备份文件进行保护, zgc, 2007-03-20
	if( !bResult )
	{
		bResult = AgentGetMpuBasTable( m_achCfgBakName );
		if( !bResult )
		{
			SetBasTableByDefault();
		}
		printf("[AgentReadConfig] Read MpuBas table failed!\n");
		m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_PARTNEWEST );
	}
	
	//	bResult = AgentGetEbapTable( m_achCfgName );
	//增加使用备份文件进行保护, zgc, 2007-03-20
	// 	if( !bResult )
	// 	{
	// 		bResult = AgentGetEbapTable( m_achCfgBakName );
	// 		if( !bResult )
	// 		{
	// 			SetEbapTableByDefault();
	// 		}
	// 		printf("[AgentReadConfig] Read Ebap table failed!\n");
	// 		m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_PARTNEWEST );
	// 	}
	
	//	bResult = AgentGetEvpuTable( m_achCfgName );
	//增加使用备份文件进行保护, zgc, 2007-03-20
	// 	if( !bResult )
	// 	{
	// 		bResult = AgentGetEvpuTable( m_achCfgBakName );
	// 		if( !bResult )
	// 		{
	// 			SetEvpuTableByDefault();
	// 		}
	// 		printf("[AgentReadConfig] Read Evpu table failed!\n");
	// 		m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_PARTNEWEST );
	// 	}
	
	bResult = AgentGetHduSchemeTable( m_achCfgName );
	if ( !bResult )
	{
		bResult = AgentGetHduTable( m_achCfgBakName );
		if (!bResult)
		{
			SetHduTableByDefault();
		}
		printf("[AgentReadConfig] Read Evpu table failed!\n");
		m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_PARTNEWEST );
	}
    //--------------------    4.6新加 end   ------------- 
	
    bResult = AgentGetMpwTable(m_achCfgName);
	//增加使用备份文件进行保护, zgc, 2007-03-20
	if( !bResult )
	{
		bResult = AgentGetMpwTable( m_achCfgBakName );
		if( !bResult )
		{
			SetMpwTableByDefault();
		}
		printf("[AgentReadConfig] Read mpwtable failed!\n");
		m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_PARTNEWEST );
	}
	
	bResult = AgentGetEqpDcs( m_achCfgName );
	//增加使用备份文件进行保护, zgc, 2007-03-20
	if( !bResult )
	{
		bResult = AgentGetEqpDcs( m_achCfgBakName );
		if( !bResult )
		{
			SetEqpDcsByDefault();
		}
		printf("[AgentReadConfig] Read eqpdcs failed!\n");
		m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_PARTNEWEST );
	}
	
    bResult = AgentGetVmpAttachTable(m_achCfgName);
	//增加使用备份文件进行保护, zgc, 2007-03-20
	if( !bResult )
	{
		bResult = AgentGetVmpAttachTable( m_achCfgBakName );
		if( !bResult )
		{
			SetVmpAttachTableByDefault();
		}
		printf("[AgentReadConfig] Read vmpattachtable failed!\n");
		m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_PARTNEWEST );
	}
	
	bResult = AgentGetPrsInfo( m_achCfgName );
	//增加使用备份文件进行保护, zgc, 2007-03-20
	if( !bResult )
	{
		bResult = AgentGetPrsInfo( m_achCfgBakName );
		if( !bResult )
		{
			SetPrsInfoByDefault();
		}
		printf("[AgentReadConfig] Read prsinfo failed!\n");
		m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_PARTNEWEST );
	}
	
	bResult = AgentGetNecSync( m_achCfgName );
	//增加使用备份文件进行保护, zgc, 2007-03-20
	if( !bResult )
	{
		bResult = AgentGetNecSync( m_achCfgBakName );
		if( !bResult )
		{
			SetNecSyncByDefault();
		}
		printf("[AgentReadConfig] Read necsync failed!\n");
		m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_PARTNEWEST );
	}
	
    bResult = AgentGetQosInfo( m_achCfgName );
	//增加使用备份文件进行保护, zgc, 2007-03-20
	if( !bResult )
	{
		bResult = AgentGetQosInfo( m_achCfgBakName );
		if( !bResult )
		{
			SetQosInfoByDefault();
		}
		printf("[AgentReadConfig] Read Qosinfo failed!\n");
		m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_PARTNEWEST );
	}
	
	bResult = AgentGetVrsRecorderTable( m_achCfgName );
	//增加使用备份文件进行保护
	if( !bResult )
	{
		bResult = AgentGetVrsRecorderTable( m_achCfgBakName );
		if( !bResult )
		{
			SetRecorderTableByDefault();
		}
		printf("[AgentReadConfig] Read vrsrecordertable failed!\n");
		m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_PARTNEWEST );
	}
	
#ifdef _MINIMCU_
    bResult = AgentGetDSCInfo( m_achCfgName );
	//增加使用备份文件进行保护, zgc, 2007-03-20
	if( !bResult )
	{
		bResult = AgentGetDSCInfo( m_achCfgBakName );
		if( !bResult )
		{
			SetDscInfoByDefault();
		}
		printf("[AgentReadConfig] Read Dscinfo failed!\n");
		m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_PARTNEWEST );
	}
#endif
	
	// VCS
	AgentGetVCSSoftName( m_achCfgName );
	
	
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_) 
//#if defined(_8KE_) || defined(_8KH_)
	// [1/14/2010 xliang] 8000E 各网卡信息读取保存(该信息并不会保存到ini文件，只是保存到成员变量中)
	TNetAdaptInfoAll tNetAdapterInfoAll;
    if ( GetNetAdapterInfo( &tNetAdapterInfoAll ) ) //获取活动的网卡信息
    {
        SetMcuEqpCfg( &tNetAdapterInfoAll );
    }
//#endif
#if defined(_LINUX_) && defined(_8KE_)
	TMcu8KECfg tMcu8KeCfg;
	GetMcuEqpCfg(&tMcu8KeCfg);
	for( u8 byIdx = 0; byIdx < MAXNUM_ETH_INTERFACE; byIdx ++)
	{
		TNetAdaptInfo *ptNetAdapt = tMcu8KeCfg.m_tLinkNetAdap.GetValue(byIdx); //当前启用的eth，无论0或1均从结点0开始累计
		if( ptNetAdapt != NULL && ptNetAdapt->GetAdaptIdx() == 0)
		{
			u8 *pbyMacAddr = ptNetAdapt->GetMacAddress();
			s8 achMacAddr[32] = {0};
			sprintf(achMacAddr, "%02X:%02X:%02X:%02X:%02X:%02X", 
				pbyMacAddr[0],
				pbyMacAddr[1],
				pbyMacAddr[2],
				pbyMacAddr[3],
				pbyMacAddr[4],
				pbyMacAddr[5]
				);
			printf("If.%d's achMacAddr is： %s\n", ptNetAdapt->GetAdaptIdx(), achMacAddr);
			
			SetHWaddrToAdapter(ptNetAdapt->GetAdaptIdx(), achMacAddr);
			
			break;
		}
	}
#endif
	//获取路由信息并进行配置
	SetRouteToSys();
	
	// [3/16/2010 xliang] 获取 gk, proxy 配置
	bResult = AgentGetGkProxyCfgInfo( m_achCfgName );
	if( !bResult )
	{
		bResult = AgentGetGkProxyCfgInfo( m_achCfgBakName );
		if( !bResult )
		{
			SetGkProxyCfgInfoByDefault();
		}
		printf("[AgentReadConfig] Read GkProxyCfgInfo failed!\n");
		m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_PARTNEWEST );
	}
	
	// [3/16/2010 xliang] 获取prs span配置 
	bResult = AgentGetPrsTimeSpanCfgInfo( m_achCfgName );
	if( !bResult )
	{
		bResult = AgentGetPrsTimeSpanCfgInfo( m_achCfgBakName );
		if( !bResult )
		{
			SetPrsTimeSpanCfgInfoByDefault();
		}
		printf("[AgentReadConfig] Read PrsSpanCfgInfo failed!\n");
		m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_PARTNEWEST );
	}
#endif

#ifdef _8KI_
	//yrl20130930获取8KI网口配置 
	bResult = AgentGetNewNetCfgInfo( m_achCfgName );
	if( !bResult )
	{
		bResult = AgentGetNewNetCfgInfo( m_achCfgBakName );
		if( !bResult )
		{
			SetNewNetCfgInfoByDefault();
		}
		printf("[AgentReadConfig] Read NewNetCfgInfo failed!\n");
		m_tMcuSystem.SetMcuCfgInfoLevel( MCUCFGINFO_LEVEL_PARTNEWEST );
	}

	SwitchNewNetCfgToMcuEqpCfg(m_tNewNetCfg, m_tMcuEqpCfg);
#endif
    return TRUE;
}

#ifdef _LINUX_
u32 CCfgAgent::EthIdxSys2Hard( u32 dwSysIdx )
{
	u32 dwHardIdx = 0xffffffff;
	
	switch( dwSysIdx )
	{
	case 0:
		dwHardIdx = G_ETH1;
		break;
	case 1:
		dwHardIdx = G_ETH2;
		break;
	case 2:
		dwHardIdx = ETH;
		break;
	default:
		OspPrintf( TRUE, FALSE, "[EthIdxSys2Hard] Input sysidx(0x%x) is error", dwSysIdx );
		break;
	}
	
	return dwHardIdx;
}

/*=============================================================================
函 数 名： EthIdxHard2Sys
功    能： 
算法实现： 
全局变量： 
参    数： u32 dwHardIdx
返 回 值： u32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/5/31   4.0			周广程                  创建
=============================================================================*/
u32 CCfgAgent::EthIdxHard2Sys( u32 dwHardIdx )
{
	u32 dwSysIdx = 0xffffffff;
	
	switch( dwHardIdx )
	{
	case G_ETH1:
		dwSysIdx = 0;
		break;
	case G_ETH2:
		dwSysIdx = 1;
		break;
	case ETH:
		dwSysIdx = 2;
		break;
	default:
		OspPrintf( TRUE, FALSE, "[EthIdxHard2Sys] Input hardidx(0x%x) is error", dwHardIdx );
		break;
	}
	
	return dwSysIdx;
}
#endif


/*=============================================================================
函 数 名： AgentGetSystemInfo
功    能： 取系统信息
算法实现： 
全局变量： 
参    数： const s8* lpszProfileName
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::AgentGetSystemInfo( const s8* lpszProfileName )
{
	if( NULL == lpszProfileName )
	{
		printf("[AgentGetSystemInfo] error input parameter.\n");
		return FALSE;
	}
	
    s8   achReturn[1024];
    memset(achReturn, '\0', sizeof(achReturn));
	s8   achDefStr[] = "Error string";
    s8   achDataTmp[] = "2000";
	
    BOOL32 bSucceedRead = TRUE;
	
    // KEY_mcusysConfigVersion 
    BOOL32 bResult = GetRegKeyString( lpszProfileName, SECTION_mcuSystem, KEY_mcusysConfigVersion, 
		achDefStr, achReturn, MAX_CONFIGURE_LENGTH );
	
	m_tMcuSystem.SetConfigVer( bResult ? achReturn : (s8*)MCUAGT_SYS_CFGVER );
	
    if( !bResult )
	{
		printf("[AgentGetSystemInfo] Wrong profile while reading %s!\n", KEY_mcusysConfigVersion );
        bSucceedRead = FALSE;
		
		//从下面<if ( !bSucceedRead )>括号中移到这里
		//for支持MCU导航配置 zgc06-12-21
		// 取配置文件的年份
        memset(achDataTmp, '\0', sizeof(achDataTmp));
        strncpy(achDataTmp, m_tMcuSystem.GetConfigVer(), 4);
        u16 wNewYear = (u16)atoi(achDataTmp);
		
        // 取配置文件的月份
        memset(achDataTmp, '\0', sizeof(achDataTmp));
        strncpy(achDataTmp, m_tMcuSystem.GetConfigVer()+4, 2);
        u16 wNewMonth = (u16)atoi(achDataTmp);
        if( wNewYear < CONF_YEAR_OLD && wNewMonth < CONF_MONTH_OLD)
        {
            printf("[AgentGetSystemInfo]\n\n-----The configure file is too old, please update it!\n\n");
            m_tMPCInfo.SetLocalConfigVer(VERSION_CONFIGURE36);
        }
	}
    
	//增加读MCU是否配置过的标识
	//for支持MCU导航配置 zgc06-12-21
	s32  nValue = 0;
    bResult = GetRegKeyInt( lpszProfileName, 
		SECTION_mcuSystem, 
		KEY_mcuIsConfiged, 
		0, &nValue );
    if( !bResult )
    {
		printf("[AgentGetIsMcuConfiged] Wrong profile while reading %s!\n",                    
			KEY_mcuIsConfiged );
        m_tMcuSystem.SetIsMcuConfiged( FALSE );
	}
    else
    {
		m_tMcuSystem.SetIsMcuConfiged( (nValue == 0) ? FALSE: TRUE );
    }    
	
    if ( !bSucceedRead )
    {
        g_cAlarmProc.ProcReadError( ERR_AGENT_READSYSCFG );
    }
	
	m_tMcuSystem.SetState( MCU_STATE_RUNNING );
    
    //return TRUE;
	return bSucceedRead;
}

/*=============================================================================
函 数 名： AgentGetLocalInfo
功    能： 取本地信息
算法实现： 
全局变量： 
参    数：  const s8* lpszProfileName
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::AgentGetLocalInfo( const s8* lpszProfileName )
{
    if( NULL == lpszProfileName )
	{
		printf("[AgentGetLocalInfo] error input parameter.\n");
		return FALSE;
	}
    s8    achReturn[MAX_VALUE_LEN + 1];
    memset(achReturn, '\0', sizeof(achReturn));
	
	s8   achDefStr[] = "Error string";
    s32  nValue = 0;
    BOOL32 bSucceedRead = TRUE;
	
	// mcu编号 
    BOOL32 bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuLocalInfo, KEY_mucId, 0, &nValue );
    if( !bResult )
	{
		printf("[AgentGetLocalInfo] Wrong profile while reading %s!\n", KEY_mucId );
		bSucceedRead = FALSE;
	}
	if( nValue == 0 || nValue > LOCAL_MCUID )
	{
		printf("[AgentGetLocalInfo] Wrong Mcu Id value: %s = %d!\n", KEY_mucId, nValue);
        nValue = LOCAL_MCUID;
	}
    m_tMcuLocalInfo.SetMcuId( bSucceedRead ? (u8)nValue : LOCAL_MCUID );
	
    // mcu别名( 最大长度: 汉字:15; 英文:31 )
    bResult = GetRegKeyString( lpszProfileName, SECTION_mcuLocalInfo, KEY_mcuAlias, 
		achDefStr, achReturn, MAXLEN_ALIAS );
    if( !bResult )
	{
		printf("[AgentGetLocalInfo] Wrong profile while reading %s!\n", KEY_mcuAlias );
        bSucceedRead = FALSE;
	}
	if ( IsUtf8Encoding() )
	{
		printf("[AgentGetLocalInfo]Mcu Alias :UTF8\n"); // 不需要转 [pengguofeng 4/12/2013]
		m_tMcuLocalInfo.SetAlias( bSucceedRead ? achReturn : "KEDA MCU" );
	}
	else
	{
		if ( bSucceedRead )
		{
			printf("[AgentGetLocalInfo]Mcu Alias :GBK\n"); //需要转！
			char achAlias[MAXLEN_ALIAS+1];
			memset(achAlias, 0 , sizeof(achAlias));
			gb2312_to_utf8(achReturn, achAlias, MAXLEN_ALIAS);
			m_tMcuLocalInfo.SetAlias( achAlias );
		}
		else
		{
			m_tMcuLocalInfo.SetAlias( "KEDA MCU" );
		}
	}
	
    // E164号码 (数字字符串，最长8位)
    bResult = GetRegKeyString( lpszProfileName, SECTION_mcuLocalInfo, KEY_mcuE164Number, 
		achDefStr, achReturn, MAXLEN_E164 );
    if( !bResult )
	{
		printf("[AgentGetLocalInfo] Wrong profile while reading %s!\n", KEY_mcuE164Number );
        bSucceedRead = FALSE;
	}
    else
    {
        // 判断E164是否合法
        for(u8 byLoop = 0; byLoop < strlen(achReturn); byLoop++)
        {
            if( 0 == strchr("#*,0123456789", achReturn[byLoop]) )
            {
                printf("[AgentGetLocalInfo] Wrong profile while reading %s!\n", KEY_mcuE164Number );
                bSucceedRead = FALSE;
            }
        }
    }
    m_tMcuLocalInfo.SetE164Num( bSucceedRead ? achReturn : "8651264273989" );
    
    // 终端H.323断链检测时间间隔
    bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuLocalInfo, Key_mcuCheckLinkTime, 
		0, &nValue );
    if( !bResult )
	{
		printf("[AgentGetLocalInfo] Wrong profile while reading %s!\n", Key_mcuCheckLinkTime );
        bSucceedRead = FALSE;
	}
    m_tMcuLocalInfo.SetCheckTime( bSucceedRead ? (u16)nValue : DEF_DISCCHECKTIME );
    
    // 终端H.323断链检测次数
    bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuLocalInfo, KEY_mcuCheckMtLinkTimes, 
		0, &nValue );
    if( !bResult )
	{
		printf("[AgentGetLocalInfo] Wrong profile while reading %s!\n", KEY_mcuCheckMtLinkTimes );
        bSucceedRead = FALSE;
	}
    m_tMcuLocalInfo.SetCheckTimes( bSucceedRead ? (u16)nValue : DEF_DISCCHECKTIMES );
	
    // 级连时终端列表刷新时间
    bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuLocalInfo, Key_mcuRefreshListTime, 
		0, &nValue );
    if( !bResult )
	{
		printf("[AgentGetLocalInfo] Wrong profile while reading %s!\n", Key_mcuRefreshListTime );
        bSucceedRead = FALSE;
	}
    m_tMcuLocalInfo.SetListRefreshTime( bSucceedRead ? (u16)nValue : PARTLIST_REFRESHTIME );
	
    // 级连时音频信息刷新时间
    bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuLocalInfo, Key_mcuAudioRefreshTime, 
		0, &nValue );
    if( !bResult)
	{
		printf("[AgentGetLocalInfo] Wrong profile while reading %s!\n", Key_mcuAudioRefreshTime );
        bSucceedRead = FALSE;
	}
    m_tMcuLocalInfo.SetAudioRefreshTime( bSucceedRead ? (u16)nValue : AUDINFO_REFRESHTIME);
	
    // 级连时视频信息刷新时间
    bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuLocalInfo, Key_mcuVideoRefreshTime, 
		0, &nValue );
    if( !bResult )
	{
		printf("[AgentGetLocalInfo] Wrong profile while reading %s!\n", Key_mcuVideoRefreshTime );
        bSucceedRead = FALSE;
	}
    m_tMcuLocalInfo.SetVideoRefreshTime( bSucceedRead ? (u16)nValue : VIDINFO_REFRESHTIME );
	
    // 是否节省带宽(0:不节省 1:节省)
    bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuLocalInfo, Key_mcuIsSaveBand, 
		1, &nValue );
    if( !bResult )
	{
		printf("[AgentGetLocalInfo] Wrong profile while reading %s!\n", Key_mcuIsSaveBand );
        bSucceedRead = FALSE;
	}
    m_tMcuLocalInfo.SetIsSaveBand( (u8)nValue );
	
    // 是否支持N+1 模式(0:不 1:是)
    bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuLocalInfo, Key_mcuIsNPlusMode, 
		0, &nValue );
    if( !bResult )
	{
		printf("[AgentGetLocalInfo] Wrong profile while reading %s!\n", Key_mcuIsNPlusMode );
		bSucceedRead = FALSE;
	}
    m_tMcuLocalInfo.SetIsNPlusMode( nValue > 0 ? TRUE : FALSE );
	
    // 是否N+1 备份模式(0:不 1:是)
    bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuLocalInfo, Key_mcuIsNPlusBackupMode, 
		0, &nValue );
    if( !bResult )
	{
		printf("[AgentGetLocalInfo] Wrong profile while reading %s!\n", Key_mcuIsNPlusBackupMode );
		bSucceedRead = FALSE;
	}
    m_tMcuLocalInfo.SetIsNPlusBackupMode( nValue > 0 ? TRUE : FALSE );
	
    // N+1 备份mcu ip
    bResult = GetRegKeyString( lpszProfileName, SECTION_mcuLocalInfo, Key_mcuNPlusMcuIp, 
		achDefStr, achReturn, MAXLEN_ALIAS );
    if( !bResult )
	{
		printf("[AgentGetLocalInfo] Wrong profile while reading %s!\n", Key_mcuNPlusMcuIp );
        bSucceedRead = FALSE;
	}
    m_tMcuLocalInfo.SetNPlusMcuIp( ntohl(INET_ADDR(achReturn)) );
	
    //rtd time
    bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuLocalInfo, Key_mcuNPlusRtdTime, 
		DEF_NPLUSRTD_TIME, &nValue );
    if( !bResult )
	{
		printf("[AgentGetLocalInfo] Wrong profile while reading %s!\n", Key_mcuNPlusRtdTime );
		bSucceedRead = FALSE;
	}
    m_tMcuLocalInfo.SetNPlusRtdTime( (u16)nValue );
	
    //rtd times
    bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuLocalInfo, Key_mcuNPlusRtdNum, 
		DEF_NPLUSRTD_NUM, &nValue );
    if( !bResult )
	{
		printf("[AgentGetLocalInfo] Wrong profile while reading %s!\n", Key_mcuNPlusRtdNum );
		bSucceedRead = FALSE;
	}
    m_tMcuLocalInfo.SetNPlusRtdNum( (u16)nValue );
	
    //rollback
    bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuLocalInfo, Key_mcuIsNPlusRollBack, 
		1, &nValue );
    if( !bResult )
	{
		printf("[AgentGetLocalInfo] Wrong profile while reading %s!\n", Key_mcuIsNPlusRollBack );
		bSucceedRead = FALSE;
	}
    m_tMcuLocalInfo.SetIsNPlusRollBack( (u8)nValue );
	
	//zjj20100107 增加配置
	//是否支持召开缺省会议
	bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuLocalInfo, key_mcuIsHoldDefaultConf, 
		0, &nValue );
    if( !bResult )
	{
		printf("[AgentGetLocalInfo] Wrong profile while reading %s!\n", key_mcuIsHoldDefaultConf );
		bSucceedRead = FALSE;
	}
    m_tMcuLocalInfo.SetIsHoldDefaultConf( (u8)nValue );
	
	//是否支持显示上级mcu终端列表
	bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuLocalInfo, key_mcuIsShowMMcuMtList, 
		0, &nValue );
    if( !bResult )
	{
		printf("[AgentGetLocalInfo] Wrong profile while reading %s!\n", key_mcuIsShowMMcuMtList );
		bSucceedRead = FALSE;
	}
    m_tMcuLocalInfo.SetIsShowMMcuMtList( (u8)nValue );
	
	//本级最多召开的MCS即时会议数
	bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuLocalInfo, key_mcuMaxMcsOnGoingConfNum, 
		DEF_MCSONGOINGCONFNUM_MAX, &nValue );
    if( !bResult )
	{
		printf("[AgentGetLocalInfo] Wrong profile while reading %s!\n", key_mcuMaxMcsOnGoingConfNum );
		bSucceedRead = FALSE;
	}
    m_tMcuLocalInfo.SetMaxMcsOnGoingConfNum( (u8)nValue );
	
	//本级召开的会议的行政级别(1到4 1最大)
	bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuLocalInfo, key_mcuAdminLevel, 
		DEF_ADMINLVEL, &nValue );
    if( !bResult )
	{
		printf("[AgentGetLocalInfo] Wrong profile while reading %s!\n", key_mcuAdminLevel );
		bSucceedRead = FALSE;
	}
    m_tMcuLocalInfo.SetAdminLevel( (u8)nValue );
	
	//在下级会议中显示的本级的会议名称显示类型
	bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuLocalInfo, key_mcuLocalConfNameShowType, 
		1, &nValue );
    if( !bResult )
	{
		printf("[AgentGetLocalInfo] Wrong profile while reading %s!\n", key_mcuLocalConfNameShowType );
		bSucceedRead = FALSE;
	}
    m_tMcuLocalInfo.SetConfNameShowType( (u8)nValue );
	
	//在下级会议中显示的本级的会议名称显示类型
	bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuLocalInfo, key_mcuIsMMcuSpeaker, 
		1, &nValue );
    if( !bResult )
	{
		printf("[AgentGetLocalInfo] Wrong profile while reading %s!\n", key_mcuIsMMcuSpeaker );
		bSucceedRead = FALSE;
	}
    m_tMcuLocalInfo.SetIsMMcuSpeaker( (u8)nValue );
	
	
	if ( !bSucceedRead )
    {
        g_cAlarmProc.ProcReadError( ERR_AGENT_READLOCALCFG );
    }
	//return TRUE;
	return bSucceedRead;
}

// [3/16/2010 xliang] 8000E prs time span config 
BOOL32 CCfgAgent::AgentGetPrsTimeSpanCfgInfo(const s8* lpszProfileName )
{
	if( NULL == lpszProfileName )
	{
		printf("[AgentGetGkProxyCfgInfo] error input parameter.\n");
		return FALSE;
	}
	
	s8    achDefStr[] = "Cannot find the section or key";
    s8    achReturn[MAX_VALUE_LEN + 1];
    s32   nValue = 0;
    memset(achReturn, '\0', sizeof(achReturn));
    BOOL32 bSuccedRead = TRUE;
	
	
	BOOL32 bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuPrsTimeSpan, KEY_mcuPrsSpan1, 0, &nValue );
    if( !bResult )  
    {
        printf("[AgentGetNetworkInfo] Wrong profile while reading %s!\n", KEY_mcuPrsSpan1 );
        bSuccedRead = FALSE;
    }
	m_tPrsTimeSpan.m_wFirstTimeSpan = (u16)nValue;
	
	
	bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuPrsTimeSpan, KEY_mcuPrsSpan2, 0, &nValue );
    if( !bResult )  
    {
        printf("[AgentGetNetworkInfo] Wrong profile while reading %s!\n", KEY_mcuPrsSpan2 );
        bSuccedRead = FALSE;
    }
	m_tPrsTimeSpan.m_wSecondTimeSpan = (u16)nValue;
	
	bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuPrsTimeSpan, KEY_mcuPrsSpan3, 0, &nValue );
    if( !bResult )  
    {
        printf("[AgentGetNetworkInfo] Wrong profile while reading %s!\n", KEY_mcuPrsSpan3 );
        bSuccedRead = FALSE;
    }
	m_tPrsTimeSpan.m_wThirdTimeSpan = (u16)nValue;
	
	bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuPrsTimeSpan, KEY_mcuPrsSpan4, 0, &nValue );
    if( !bResult )  
    {
        printf("[AgentGetNetworkInfo] Wrong profile while reading %s!\n", KEY_mcuPrsSpan4 );
        bSuccedRead = FALSE;
    }
	m_tPrsTimeSpan.m_wRejectTimeSpan = (u16)nValue;
	
	
	return bSuccedRead;
}


BOOL32 CCfgAgent::AgentGetGkProxyCfgInfo( const s8* lpszProfileName)
{
	if( NULL == lpszProfileName )
	{
		printf("[AgentGetGkProxyCfgInfo] error input parameter.\n");
		return FALSE;
	}
	
	s8    achDefStr[] = "Cannot find the section or key";
    s8    achReturn[MAX_VALUE_LEN + 1];
    s32   nValue = 0;
    memset(achReturn, '\0', sizeof(achReturn));
    BOOL32 bSuccedRead = TRUE;
	
	// GK的IP地址, 0表示不使用 
    BOOL32  bResult = GetRegKeyString( lpszProfileName, SECTION_mcuGkProxyCfg, KEY_mcuGkIpAddr, 
		achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( !bResult )  
    {
        printf("[AgentGetGkProxyCfgInfo] Wrong profile while reading %s!\n", KEY_mcuGkIpAddr );
        bSuccedRead = FALSE;
    }
	m_tGkProxyCfgInfo.SetGkIpAddr(ntohl(INET_ADDR(achReturn)));
	
	//proxy ip
	// 	bResult = GetRegKeyString( lpszProfileName, SECTION_mcuGkProxyCfg, KEY_mcuProxyIpAddr, 
	// 		achDefStr, achReturn, MAX_VALUE_LEN + 1 );
	//     if( !bResult )  
	//     {
	//         printf("[AgentGetGkProxyCfgInfo] Wrong profile while reading %s!\n", KEY_mcuProxyIpAddr );
	//         bSuccedRead = FALSE;
	//     }
	// 	m_tGkProxyCfgInfo.SetProxyIpAddr(ntohl(INET_ADDR(achReturn)));
	
	
	//proxy port
	// 	bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuGkProxyCfg, KEY_mcuProxyPort, 0, &nValue );
	// 	if( !bResult )  
	// 	{
	// 		printf("[AgentGetNetworkInfo] Wrong profile while reading %s!\n", KEY_mcuProxyPort );
	//         bSuccedRead = FALSE;
	// 	}
	// --端口保护 FIXME
	//	m_tGkProxyCfgInfo.SetProxyPort( (u16)nValue );
	
	//if GK used
	bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuGkProxyCfg, KEY_mcuGkUsed, 0, &nValue );
    if( !bResult )  
    {
        printf("[AgentGetNetworkInfo] Wrong profile while reading %s!\n", KEY_mcuGkUsed );
        bSuccedRead = FALSE;
    }
	m_tGkProxyCfgInfo.SetGkUsed( (u8)nValue);
	
	//if proxy used
    bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuGkProxyCfg, KEY_mcuProxyUsed, 0, &nValue );
    if( !bResult )  
    {
        printf("[AgentGetNetworkInfo] Wrong profile while reading %s!\n", KEY_mcuProxyUsed );
        bSuccedRead = FALSE;
    }
	m_tGkProxyCfgInfo.SetProxyUsed( (u8)nValue);

	//读取DMZ配置信息
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	s8    achProfileNamePxy[MAXLEN_MCU_FILEPATH] = {0};
	sprintf(achProfileNamePxy, "%s/%s", DIR_CONFIG, PXYCFG_INI_FILENAME);
	
	bResult = GetRegKeyInt(achProfileNamePxy,SECTION_ProxyLocalInfo,KEY_mcuUseDMZ,0,&nValue);
    if( !bResult )  
    {
        printf("[AgentGetNetworkInfo] Wrong profile while reading %s!\n", KEY_mcuUseDMZ );
        bSuccedRead = FALSE;
    }
	m_tProxyDMZInfo.SetIsUseDMZ((u8)nValue);

	memset(achReturn,0,sizeof(achReturn));
    bResult = GetRegKeyString( achProfileNamePxy, SECTION_ProxyLocalInfo, KEY_mcuIpDmzAddr, 
		achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( !bResult )  
    {
        printf("[AgentGetGkProxyCfgInfo] Wrong profile while reading %s!\n", KEY_mcuIpDmzAddr );
        bSuccedRead = FALSE;
    }
	m_tProxyDMZInfo.SetDMZIpAddr(ntohl(INET_ADDR(achReturn)));
#endif
	
	return bSuccedRead;
	
	
}

#ifdef _8KI_
/*=============================================================================
函 数 名： AgentGetNewNetCfgInfo
功    能： 取8KI网络信息
算法实现： 
全局变量： 
参    数：  const s8* lpszProfileName
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::AgentGetNewNetCfgInfo( const s8* lpszProfileName )
{
	if( NULL == lpszProfileName )
	{
		printf("[AgentGetNetworkInfo] error input parameter.\n");
		return FALSE;
	}

	#define MAX_KEY_LEN 10
	#define MAX_IP_LEN 32
	
	s8	achDmzIpAddr[MAX_IP_LEN] = {0};
	s8	achNotesInfo[MAXLEN_NOTES] = {0};
	s8	achKeyIpAddr[MAX_KEY_LEN] = {0};
	s8	achKeyMask[MAX_KEY_LEN] = {0};
	s8	achKeyGw[MAX_KEY_LEN] = {0};
	s8	achKeyNotes[MAX_KEY_LEN] = {0};
	s8	achIpAddr[MAX_IP_LEN] = {0};
	s8	achMask[MAX_IP_LEN] = {0};	
	s8	achGw[MAX_IP_LEN] = {0};
	s8	achSecName[MAX_KEY_LEN] = {0};
	s8  achReturn[MAX_VALUE_LEN + 1] = {0};				
		
	FILE *hFile = NULL;
    if(NULL == (hFile = fopen(lpszProfileName, "r"))) // exist
	{
		Agtlog(LOG_ERROR,"%s does not exist!\n", lpszProfileName);
		return FALSE;
	}
	
	fclose( hFile );
	//读取模式
	s32 nUseModeType = 0;
	BOOL32  bResult = GetRegKeyInt( lpszProfileName, "NewNetCfgInfo", "EthModeType", MODETYPE_3ETH, &nUseModeType);
	m_tNewNetCfg.SetUseModeType((enUseModeType)nUseModeType);
	
	//读取网络类型
	s32 nNetworkType = 0;
	GetRegKeyInt( lpszProfileName, "NewNetCfgInfo", "NetworkType", enLAN, &nNetworkType);
	m_tNewNetCfg.SetNetworkType((enNetworkType)nNetworkType);

	//读是否启用sip
	s32 nUseSip = 0;
	GetRegKeyInt( lpszProfileName, "NewNetCfgInfo", "UseSip", 0, &nUseSip);
	m_tNewNetCfg.SetIsUseSip(nUseSip);

	//读取SipIp地址
	memset(achReturn, '\0', sizeof(achReturn));				
	GetRegKeyString( lpszProfileName, "NewNetCfgInfo", "SipIpAddr", 0, achReturn, sizeof(achReturn));			
	m_tNewNetCfg.SetSipIpAddr(ntohl(INET_ADDR(achReturn)));

	//读取SipMask地址
	memset(achReturn, '\0', sizeof(achReturn));				
	GetRegKeyString( lpszProfileName, "NewNetCfgInfo", "SipMaskAddr", 0, achReturn, sizeof(achReturn));			
	m_tNewNetCfg.SetSipMaskAddr(ntohl(INET_ADDR(achReturn)));

	//读取SipGw地址
	memset(achReturn, '\0', sizeof(achReturn));				
	GetRegKeyString( lpszProfileName, "NewNetCfgInfo", "SipGwAddr", 0, achReturn, sizeof(achReturn));			
	m_tNewNetCfg.SetSipGwAddr(ntohl(INET_ADDR(achReturn)));
	
	TEthCfg tEthCfg;
	TNetCfg tNetCfg;
	
	for(u8 byEthIdx = 0; byEthIdx < MAXNUM_ETH_INTERFACE; byEthIdx ++)
	{
		m_tNewNetCfg.GetEthCfg(byEthIdx, tEthCfg);
		sprintf( achSecName, "8KI-eth%d", byEthIdx);
		
		//从文件读取DMZ地址
		memset(achReturn, '\0', sizeof(achReturn));				
		GetRegKeyString( lpszProfileName, achSecName, "DmzIp", 0, achReturn, sizeof(achReturn));			
		tEthCfg.SetDMZIpAddr(ntohl(INET_ADDR(achReturn)));
		
		//从文件读取网口下有效IP数目
		s32 nUseIpNum = 0;
		GetRegKeyInt( lpszProfileName, achSecName, "UseIpNum", 0, &nUseIpNum);

		for( u8 byIpSecIdx = 0; byIpSecIdx < nUseIpNum; byIpSecIdx ++)
		{
			tEthCfg.GetNetCfg(byIpSecIdx, tNetCfg);
			
			memset(achKeyIpAddr, 0, sizeof(achKeyIpAddr));
			memset(achKeyMask, 0, sizeof(achKeyMask));
			memset(achKeyGw, 0, sizeof(achKeyGw));
			memset(achKeyNotes, 0, sizeof(achKeyNotes));
			
			sprintf(achKeyIpAddr, "Ip%d", byIpSecIdx);
			sprintf(achKeyMask, "Mask%d", byIpSecIdx);					
			sprintf(achKeyGw, "Gw%d", byIpSecIdx);
			sprintf(achKeyNotes, "Notes%d", byIpSecIdx);
			
			memset(achIpAddr, 0, sizeof(achIpAddr));
			memset(achMask, 0, sizeof(achMask));
			memset(achGw, 0, sizeof(achGw));
			
			GetRegKeyString( lpszProfileName, achSecName, achKeyIpAddr, 0, achIpAddr, sizeof(achIpAddr) );
			u32	dwIpAddr	= ntohl(INET_ADDR(achIpAddr));
			
			if (dwIpAddr == 0)
			{
				continue;
			}
			
			GetRegKeyString( lpszProfileName, achSecName, achKeyMask, 0, achMask, sizeof(achMask) );
			GetRegKeyString( lpszProfileName, achSecName, achKeyGw, 0, achGw, sizeof(achGw) );
			GetRegKeyString( lpszProfileName, achSecName, achKeyNotes, "", achNotesInfo, sizeof(achNotesInfo) );
			
			u32	dwMask		= ntohl(INET_ADDR(achMask));
			u32 dwGwIp		= ntohl(INET_ADDR(achGw));
			
			tNetCfg.SetNetRouteParam(dwIpAddr, dwMask, dwGwIp);
			tNetCfg.SetNotes(achNotesInfo);
			
			tEthCfg.SetNetCfg(byIpSecIdx, tNetCfg);	//信息保存
			
		}//for( u8 byIpSecIdx = 0; byIpSecIdx < MCU_MAXNUM_ADAPTER_IP; byIpSecIdx ++)		
		
		m_tNewNetCfg.SetEthCfg(byEthIdx, tEthCfg);	//信息保存
		
	}//for(u8 byEthIdx = 0; byEthIdx < MAXNUM_ETH_INTERFACE; byEthIdx ++)

	return bResult;
}

/*=============================================================================
函 数 名： SwitchNewNetCfgToMcuEqpCfg
功    能： TNewNetCfg转换为TMcu8KECfg
算法实现： 
全局变量： 
参    数： IN：const TNewNetCfg &tNewNetCfg 
		   OUT：TMcu8KECfg &tMcuEqpCfg
返 回 值： void 
=============================================================================*/
void CCfgAgent::SwitchNewNetCfgToMcuEqpCfg( const TNewNetCfg &tNewNetCfg, TMcu8KECfg &tMcuEqpCfg )
{
	m_tMcuEqpCfg.SetUseModeType(m_tNewNetCfg.GetUseModeType());
	m_tMcuEqpCfg.SetNetworkType(m_tNewNetCfg.GetNetworkType());
	m_tMcuEqpCfg.SetIsUseSip(m_tNewNetCfg.IsUseSip());
	m_tMcuEqpCfg.SetSipInEthIdx(m_tNewNetCfg.GetSipInEthIdx());
	m_tMcuEqpCfg.SetSipIpAddr(m_tNewNetCfg.GetSipIpAddr());
	m_tMcuEqpCfg.SetSipMaskAddr(m_tNewNetCfg.GetSipMaskAddr());
	m_tMcuEqpCfg.SetSipGwAddr(m_tNewNetCfg.GetSipGwAddr());
	
	TNetParam achtPxyNetParam[MCU_MAXNUM_ADAPTER];
	TNetParam achtPxyGwParam[MCU_MAXNUM_ADAPTER];
	
	TEthCfg tEthCfg;
	TNetCfg tNetCfg;

	u32 dwAdptNum = tMcuEqpCfg.m_tLinkNetAdap.Length();

	for(u8 bylop = 0; bylop < dwAdptNum; bylop ++)
	{
		memset( achtPxyNetParam, 0, sizeof(achtPxyNetParam));
		memset( achtPxyGwParam, 0, sizeof(achtPxyGwParam));
		
		m_tNewNetCfg.GetEthCfg(bylop, tEthCfg);
		
		TNetAdaptInfo *ptNetAdaptInfo = m_tMcuEqpCfg.m_tLinkNetAdap.GetValue(bylop);
		
		if(ptNetAdaptInfo != NULL)
		{	
			ptNetAdaptInfo->m_tLinkIpAddr.Clear();
			
			for(u8 byloop = 0; byloop < MCU_MAXNUM_ADAPTER; byloop ++)
			{		
				tEthCfg.GetNetCfg(byloop, tNetCfg);
				
				u32 dwIp = tNetCfg.GetIpAddr();
				if(dwIp == 0)
				{
					continue;
				}
				u32 dwMask = tNetCfg.GetIpMask();
				u32 dwGw = tNetCfg.GetGwIp();
				
				achtPxyNetParam[byloop].SetNetParam(dwIp, dwMask);
				achtPxyGwParam[byloop].SetNetParam(dwGw,0);
				
				if( achtPxyNetParam[byloop].IsValid())
				{
					ptNetAdaptInfo->m_tLinkIpAddr.Append(achtPxyNetParam[byloop]);
					ptNetAdaptInfo->m_tLinkDefaultGateway.Append(achtPxyGwParam[byloop]);
				}
			}			
		}
	}
}
#endif

/*=============================================================================
函 数 名： AgentGetNetworkInfo
功    能： 取网络信息
算法实现： 
全局变量： 
参    数：  const s8* lpszProfileName
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::AgentGetNetworkInfo( const s8* lpszProfileName )
{
    if( NULL == lpszProfileName )
	{
		printf("[AgentGetNetworkInfo] error input parameter.\n");
		return FALSE;
	}
	
	s8    achDefStr[] = "Cannot find the section or key";
    s8    achReturn[MAX_VALUE_LEN + 1];
    s32   nValue = 0;
    memset(achReturn, '\0', sizeof(achReturn));
    BOOL32 bSuccedRead = TRUE;
	
	// GK的IP地址, 0表示不使用 
    BOOL32  bResult = GetRegKeyString( lpszProfileName, SECTION_mcuNetwork, KEY_mcunetGKIpAddr, 
		achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( !bResult )  
    {
        printf("[AgentGetNetworkInfo] Wrong profile while reading %s!\n", KEY_mcunetGKIpAddr );
        bSuccedRead = FALSE;
    }
    m_tMcuNetwork.SetGKIp( ntohl(INET_ADDR(achReturn)) );
	
    // GK计费
    bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuNetwork, KEY_mcunetGKCharge, 0, &nValue );
    if( !bResult )  
    {
        printf("[AgentGetNetworkInfo] Wrong profile while reading %s!\n", KEY_mcunetGKCharge );
        bSuccedRead = FALSE;
    }
    m_tMcuNetwork.SetIsGKCharge( nValue == 1 ? TRUE : FALSE );
    
    // 主协议适配板ip
    bResult = GetRegKeyString( lpszProfileName, SECTION_mcuNetwork, KEY_mcunetRRQMtadpIp,
		achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if ( !bResult )
    {
        printf("[AgentGetNetworkInfo] Wrong profile while reading %s!\n", KEY_mcunetRRQMtadpIp );
        bSuccedRead = FALSE;
    }
    m_tMcuNetwork.SetRRQMtadpIp( ntohl(INET_ADDR(achReturn)) );
	
	// MCU组播IP地址(225.0.0.0与239.255.255.255之间) 
    bResult = GetRegKeyString( lpszProfileName, SECTION_mcuNetwork, KEY_mcunetMulticastIpAddr, 
		achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( !bResult )  
	{
		printf("[AgentGetNetworkInfo] Wrong profile while reading %s!\n", KEY_mcunetMulticastIpAddr );
        bSuccedRead = FALSE;
	}
    m_tMcuNetwork.SetCastIp( ntohl(INET_ADDR(achReturn)) );
	
	// MCU组播起始端口(必须是4的整数倍)
	bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuNetwork, KEY_mcunetMulticastPort, 0, &nValue );
	if( !bResult )  
	{
		printf("[AgentGetNetworkInfo] Wrong profile while reading %s!\n", KEY_mcunetMulticastPort );
        bSuccedRead = FALSE;
	}
	m_tMcuNetwork.SetCastPort( (u16)nValue );
	
	// MCU起始接收端口号
	bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuNetwork, KEY_mcunetRecvStartPort, 0, &nValue );
	if( !bResult )  
	{
		printf("[AgentGetNetworkInfo] Wrong profile while reading %s!\n", KEY_mcunetRecvStartPort );
        bSuccedRead = FALSE;
	}
	// 保护端口不小于终端MCU侧起始端口, zgc, 2007-06-05
	if( MT_MCU_STARTPORT > nValue )
	{
		printf("[AgentGetNetworkInfo] The mcu net recv start port cfg value(%d) is changed to default value\n", nValue);
		nValue = MT_MCU_STARTPORT;
	}
	m_tMcuNetwork.SetRecvStartPort( (u16)nValue );
	
    // MCU的225/245接收起始端口号
	bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuNetwork, KEY_mcunet225245StartPort, 0, &nValue );
	if( !bResult )  
	{
		printf("[AgentGetNetworkInfo] Wrong profile while reading %s!\n", KEY_mcunet225245StartPort );
        bSuccedRead = FALSE;
	}
	m_tMcuNetwork.Set225245StartPort( (u16)nValue );
	
    // 终端数 
	bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuNetwork, KEY_mcunet225245MtNum, 0, &nValue );
	if( !bResult )  
	{
		printf("[AgentGetNetworkInfo] Wrong profile while reading %s!\n", KEY_mcunet225245MtNum);
        bSuccedRead = FALSE;
	}
    if(nValue > MAXNUM_CONF_MT || nValue <= 0)
    {
        printf("[AgentGetNetworkInfo] The Mt Num is not suit for the rule\n");
        nValue = MAXNUM_CONF_MT;
    }
	m_tMcuNetwork.Set225245MtNum( (u16)nValue );
	
	// MTU大小, zgc, 2007-04-02
	bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuNetwork, KEY_mcunetMTUSize, DEFAULT_MTU_SIZE, &nValue );
	if( !bResult || nValue > MAXNUM_MTU_SIZE || nValue < MINNUM_MTU_SIZE )
	{
		nValue = DEFAULT_MTU_SIZE;
		printf("[AgentGetNetworkInfo] Wrong profile while reading %s!\n", KEY_mcunetMTUSize);
        bSuccedRead = FALSE;
	}
	m_tMcuNetwork.SetMTUSize( (u16)nValue );
	// end, zgc, 2007-04-02


    // GK RRQ username
	memset(achReturn, '\0', sizeof(achReturn));
	s8 achDefUsreName[GK_RRQ_NAME_LEN]={0};

	s32 dwIsUseGkRRQPwd = 0;
    bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuNetwork, KEY_mcunetIsGKRRQUsePwd,
		0, &dwIsUseGkRRQPwd);
    if ( !bResult )
    {
        printf("[AgentGetNetworkInfo] Wrong profile while reading %s!\n", KEY_mcunetIsGKRRQUsePwd );
        //bSuccedRead = FALSE;
    }
    m_tMcuNetwork.SetGkRRQUsePwdFlag((u8)dwIsUseGkRRQPwd);

	// GK RRQ password
	memset(achReturn, '\0', sizeof(achReturn));
	s8 achDefPassword[GK_RRQ_NAME_LEN]={0};
    bResult = GetRegKeyString( lpszProfileName, SECTION_mcuNetwork, KEY_mcunetGKRRQPwd,
		achDefPassword, achReturn, MAX_VALUE_LEN + 1 );
    if ( !bResult )
    {
        printf("[AgentGetNetworkInfo] Wrong profile while reading %s!\n", KEY_mcunetGKRRQPwd );
        //bSuccedRead = FALSE;
    }
    m_tMcuNetwork.SetGkRRQPassword(achReturn);

	// 是否使用MPC转发数据 1 是, 0 否  
    // guzh [1/16/2007] 不再从mcucfg.ini 里面确定,通过debug文件确定
    m_tMcuNetwork.SetMpcTransData( g_tAgentDebugInfo.IsMpcRunMp() );
	
	// 是否使用MPC内置协议栈 1 是, 0 否 
	// [12/30/2009 xliang] 8000A/B/C 统一成通过配置文件读取
	// [2/24/2011 xliang] 8000G 始终启用内置接入
#if defined (_MINIMCU_)	
	// 8000B始终不起内置接入，占用太多内存资源 [5/28/2012 chendaiwei]
	m_tMcuNetwork.SetMpcStack( FALSE );
#else
	#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
		m_tMcuNetwork.SetMpcStack( TRUE );	
	#else
		bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuNetwork, KEY_mcunetUseMPCStack, 0, &nValue );
		if( !bResult )  
		{
			printf("[AgentGetNetworkInfo] Wrong profile while reading %s!\n", KEY_mcunetUseMPCStack );
			bSuccedRead = FALSE;
		}
		m_tMcuNetwork.SetMpcStack( nValue > 0 ? TRUE : FALSE);
	#endif
#endif
	
	
    if ( !bSuccedRead )
    {
        g_cAlarmProc.ProcReadError( ERR_AGENT_READNETCFG );
    }
	return bSuccedRead;
}

/*=============================================================================
函 数 名： AgentGetTrapRcvTable
功    能： 取Trap信息
算法实现： 
全局变量： 
参    数：  const s8* lpszProfileName
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::AgentGetTrapRcvTable( const s8* lpszProfileName )
{
	if( NULL == lpszProfileName )
	{
		printf("[AgentGetTrapRcvTable] Error input param.\n");
		return FALSE;
	}
	
    s8    achSeps[] = " \t";       // 分隔符
    s8    *pchToken = NULL;
    s32    nMemEntryNum;
    s8     achPortStr[32];        // 临时存放字符串
	memset(achPortStr, '\0', sizeof(achPortStr));
	
    BOOL32 bSucceedRead = TRUE;
	
    // get the number of entry
    BOOL32 bResult = GetRegKeyInt( lpszProfileName, SECTION_mcunetTrapRcvTable, 
		STR_ENTRY_NUM, 0, &nMemEntryNum );
	if( !bResult ) 
	{
		printf("[AgentGetTrapRcvTable] Wrong profile while reading %s %s!\n", 
			SECTION_mcunetTrapRcvTable, STR_ENTRY_NUM );
		bSucceedRead = FALSE;
	}
	
    // 接收TRAP陷阱的服务器表项数为0，则可直接退出
    if( 0 == nMemEntryNum ) 
    {
        m_dwTrapRcvEntryNum = 0;
    }
    else
    {
		// 分配空间
         s8*   *ppszTable = new s8*[(u32)nMemEntryNum];
		if( NULL == ppszTable )
		{
			printf("[AgentGetTrapRcvTable] Error to alloc memory.\n");
			return FALSE;
		}
		
        u32  dwLoop = 0;
        for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
        {
            ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
			if( NULL == ppszTable[dwLoop] )
			{
				FREE_TABLE_MEM( ppszTable, dwLoop );
				return FALSE;
			}
        }
		
		// 取整个表
        m_dwTrapRcvEntryNum = nMemEntryNum;
        bResult = AgtGetRegKeyStringTable( lpszProfileName, SECTION_mcunetTrapRcvTable,
			"fail", ppszTable, &m_dwTrapRcvEntryNum, MAX_VALUE_LEN + 1 );
        if( !bResult)
        {
			printf( "[AgentGetTrapRcvTable] Wrong profile while reading %s! table\n", 
				SECTION_mcunetTrapRcvTable );
            FREE_TABLE_MEM( ppszTable, nMemEntryNum );
            m_dwTrapRcvEntryNum = 0;
            g_cAlarmProc.ProcReadError( ERR_AGENT_READTRAPCFG );
            return FALSE;
        }
		
        // 分析字串
        for( dwLoop = 0; dwLoop < m_dwTrapRcvEntryNum; dwLoop++ )
        {
            m_atTrapRcvTable[dwLoop].SetIndex((u8)dwLoop);
			
			// IpAddr 
            pchToken = strtok( ppszTable[dwLoop], achSeps );
            if( NULL == pchToken)
            {
                printf("[AgentGetTrapRcvTable] Wrong profile while reading %s!\n", 
					FIELD_mcunetTrapRcvEntIpAddr );
                bSucceedRead = FALSE;
                continue;
            }
            else
            {
                m_atTrapRcvTable[dwLoop].SetTrapIp( ntohl(INET_ADDR( pchToken )) );
            }
			
            // 读共同体
            pchToken = strtok( NULL, achSeps );
            if( NULL == pchToken )
            {
                printf("[AgentGetTrapRcvTable] Wrong profile while reading %s!\n", 
					FIELD_mcunetTrapRcvEntRCommunity );
                bSucceedRead = FALSE;
                continue;
            }
            else
            {
				if ( IsUtf8Encoding() )
				{
					printf("[AgentGetTrapRcvTable]No need to Trans to Utf8\n");
					m_atTrapRcvTable[dwLoop].SetReadCom( pchToken );
				}
				else
				{
					printf("[AgentGetTrapRcvTable]need to Trans to Utf8\n");
					s8 achTrapName[MAXLEN_TRAP_COMMUNITY+1];
					memset(achTrapName, 0, sizeof(achTrapName));
					gb2312_to_utf8(pchToken, achTrapName, MAXLEN_TRAP_COMMUNITY);
					m_atTrapRcvTable[dwLoop].SetReadCom( achTrapName );
				}
            }
			
            // 写共同体
            pchToken = strtok(NULL, achSeps);
            if(NULL == pchToken)
            {
                printf("[AgentGetTrapRcvTable] Wrong profile while reading %s\n", 
					FIELD_mcunetTrapRevEntWCommunity);
				bSucceedRead = FALSE;
                continue;
            }
            else
            {
				if ( IsUtf8Encoding() )
				{
					printf("[AgentGetTrapRcvTable]No need to Trans to Utf8\n");
					m_atTrapRcvTable[dwLoop].SetWriteCom( pchToken );
				}
				else
				{
					printf("[AgentGetTrapRcvTable]need to Trans to Utf8\n");
					s8 achTrapName[MAXLEN_TRAP_COMMUNITY+1];
					memset(achTrapName, 0, sizeof(achTrapName));
					gb2312_to_utf8(pchToken, achTrapName, MAXLEN_TRAP_COMMUNITY);
					m_atTrapRcvTable[dwLoop].SetWriteCom( achTrapName );
				}
            }
			
            // 取、设置端口
            pchToken = strtok(NULL, achSeps);
            if(NULL == pchToken)
            {
                printf("[AgentGetTrapRcvTable] Wrong profile while reanding %s\n", 
					FIELD_mcunetTrapGetSetPort);
                m_atTrapRcvTable[dwLoop].SetGSPort( SNMP_PORT );
                bSucceedRead = FALSE;
                continue;
            }
            else
            {
                strncpy( achPortStr, pchToken, MAX_PORT_LENGTH);
                m_atTrapRcvTable[dwLoop].SetGSPort( atoi(achPortStr) );
            }
			
            // Trap 端口
            memset(achPortStr, '\0', MAX_PORT_LENGTH);
            pchToken = strtok(NULL, achSeps);
            if(NULL == pchToken)
            {
                printf("[AgentGetTrapRcvTable] Wrong profile while reading %s\n", 
					FIELD_mcunetTrapSendTrapPort);
                m_atTrapRcvTable[dwLoop].SetTrapPort( TRAP_PORT );
                bSucceedRead = FALSE;
                continue;
            }
            else
            {
                strncpy(achPortStr, pchToken, MAX_PORT_LENGTH);
                m_atTrapRcvTable[dwLoop].SetTrapPort( atoi(achPortStr) );
            }
        }
		
        // 释放临时空间
        FREE_TABLE_MEM( ppszTable, nMemEntryNum );
    }
    if ( !bSucceedRead )
    {
        g_cAlarmProc.ProcReadError( ERR_AGENT_READTRAPCFG );
    }
    
    //return TRUE;
	return bSucceedRead;
}

/*=============================================================================
函 数 名： GetSlotThroughName
功    能： 有单板槽号得到单板名称
算法实现： 
全局变量： 
参    数： const s8* pbyName
u8* pbySlot
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::GetSlotThroughName(const s8* lpszName, u8* pbySlot)
{
    BOOL32 bRet = TRUE;
	if( (0 == memcmp( lpszName, "EX", 2 )) || (0 == memcmp( lpszName, "MC", 2 )) )
	{
		*pbySlot = atoi( (s8*)lpszName + 2 );
	}
	else if (0 == memcmp(lpszName, "TVSEX", 5) )
	{
		*pbySlot = atoi( (s8*)lpszName + 5 );
		return TRUE;
	}
	//[2011/01/25 zhushz]IS2.x单板支持
	else if (0 == memcmp(lpszName, "IS", 2) )
	{
		*pbySlot = atoi( (s8*)lpszName + 2 );
		return TRUE;
	}
	else
	{
		*pbySlot = 0;
		bRet = FALSE;
	}
	return bRet;
}

/*=============================================================================
函 数 名： GetTypeThroughName
功    能： 由单板类型得到单板名称
算法实现： 
全局变量： 
参    数：  const s8* pbyName
u8* pbyType
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::GetTypeThroughName( const s8* lpzName, u8* pbyType )
{
    if( 0 == strcmp( lpzName, BOARD_TYPE_MPC ) )
	{
		*pbyType = BRD_TYPE_MPC/*DSL8000_BRD_MPC*/;
		return( TRUE );
	}
	//  [1/21/2011 chendaiwei]MPC2支持
	else if( 0 == strcmp( lpzName, BOARD_TYPE_MPC2 ) )
	{
		*pbyType = BRD_TYPE_MPC2;
		return( TRUE ); 
	}	
	else if( 0 == strcmp( lpzName, BOARD_TYPE_CRI ) )
	{
		*pbyType = BRD_TYPE_CRI/*DSL8000_BRD_CRI*/;
		return( TRUE ); 
	}
	//  [1/21/2011 chendaiwei]CRI2支持
	else if( 0 == strcmp( lpzName, BOARD_TYPE_CRI2 ) )
	{
		*pbyType = BRD_TYPE_CRI2;
		return( TRUE ); 
	}	
	else if( 0 == strcmp( lpzName, BOARD_TYPE_DTI ) )
	{
		*pbyType = BRD_TYPE_DTI/*DSL8000_BRD_DTI*/;
		return( TRUE ); 
	}
	else if( 0 == strcmp( lpzName, BOARD_TYPE_DIC ) )
	{
		*pbyType = BRD_TYPE_DIC/*DSL8000_BRD_DIC*/;
		return( TRUE ); 
	}
	else if( 0 == strcmp( lpzName, BOARD_TYPE_DRI ) )
	{
		*pbyType = BRD_TYPE_DRI/*DSL8000_BRD_DRI*/;
		return( TRUE );
	}
	//  [1/21/2011 chendaiwei]支持DRI2
	else if( 0 == strcmp( lpzName, BOARD_TYPE_DRI2 ) )
	{
		*pbyType = BRD_TYPE_DRI2/*DSL8000_BRD_DRI2*/;
		return( TRUE );
	}
	else if( 0 == strcmp( lpzName, BOARD_TYPE_MMP ))
	{
		*pbyType = BRD_TYPE_MMP/*DSL8000_BRD_MMP*/;
		return( TRUE );
	}
	else if( 0 == strcmp( lpzName, BOARD_TYPE_VPU ))
	{
		*pbyType = BRD_TYPE_VPU/*DSL8000_BRD_VPU*/;
		return( TRUE );
	}
	else if( 0 == strcmp( lpzName, BOARD_TYPE_DEC5 ))
	{
		*pbyType = BRD_TYPE_DEC5/*DSL8000_BRD_DEC5*/;
		return( TRUE );
	}
	else if( 0 == strcmp( lpzName, BOARD_TYPE_VAS ))
	{
		*pbyType = BRD_TYPE_VAS/*DSL8000_BRD_VAS*/;
		return( TRUE );
	}
	else if( 0 == strcmp( lpzName, BOARD_TYPE_IMT ))
	{
		*pbyType = BRD_TYPE_IMT/*DSL8000_BRD_IMT*/;
		return( TRUE );
	}
    else if( 0 == strcmp( lpzName, BOARD_TYPE_APU ))
    {
        *pbyType = BRD_TYPE_APU/*DSL8000_BRD_APU*/;
        return( TRUE );
    }
	//tianzhiyong 100205  EAPU板支持
	else if( 0 == strcmp( lpzName, BOARD_TYPE_EAPU ))
    {
        *pbyType = BRD_TYPE_EAPU;
        return( TRUE );
    }
    else if( 0 == strcmp( lpzName, BOARD_TYPE_DSI ) )
    {
        *pbyType = BRD_TYPE_DSI/*DSL8000_BRD_DSI*/;
        return( TRUE );
    }
    else if( 0 == strcmp( lpzName, BOARD_TYPE_MPCB ))
    {
        *pbyType = KDV8000B_BOARD;
        return( TRUE );
    }
#ifdef _LINUX_
#ifndef _LINUX12_
    else if( 0 == strcmp( lpzName, BOARD_TYPE_MPCC ))
    {
        *pbyType = KDV8000C_BOARD;
        return( TRUE );
	}    
#endif
#endif
	else if( 0 == strcmp( lpzName, BOARD_TYPE_MDSC))
	{
		*pbyType = BRD_TYPE_MDSC/*DSL8000_BRD_MDSC*/;
		return TRUE;
	}
    else if( 0 == strcmp(lpzName, BOARD_TYPE_HDSC) )
    {
        *pbyType = BRD_TYPE_HDSC/*DSL8000_BRD_HDSC*/;
        return TRUE;
    }
	else if( 0 == strcmp(lpzName, BOARD_TYPE_DSC) )
	{
		*pbyType = BRD_TYPE_DSC/*KDV8000B_MODULE*/;
		return TRUE;
	}
	//4.6
	else if( 0 == strcmp(lpzName, BOARD_TYPE_MPU) )
	{
		*pbyType = BRD_TYPE_MPU/*BRD_HWID_DSL8000_MPU*/;
		return TRUE;
	}
	else if( 0 == strcmp(lpzName, BOARD_TYPE_MPU2ECARD) )
	{
		*pbyType = BRD_TYPE_MPU2ECARD;
		return TRUE;
	}
	else if( 0 == strcmp(lpzName, BOARD_TYPE_MPU2) )
	{
		*pbyType = BRD_TYPE_MPU2;
		return TRUE;
	}
	else if( 0 == strcmp(lpzName, BOARD_TYPE_APU2) )
	{
		*pbyType = BRD_TYPE_APU2;
		return TRUE;
	}
	else if( 0 == strcmp(lpzName, BOARD_TYPE_HDU) )
	{
		*pbyType = BRD_TYPE_HDU/*BRD_HWID_KDM200_HDU*/;
		return TRUE;
	}
	else if( 0 == strcmp(lpzName, BOARD_TYPE_HDU_L) )
	{
		*pbyType = BRD_TYPE_HDU_L;
		return TRUE;
	}
	else if( 0 == strcmp(lpzName, BOARD_TYPE_HDU2) )
	{
		*pbyType = BRD_TYPE_HDU2;
		return TRUE;
	}
	else if( 0 == strcmp(lpzName, BOARD_TYPE_HDU2_L) )
	{
		*pbyType = BRD_TYPE_HDU2_L;
		return TRUE;
	}
	else if( 0 == strcmp(lpzName, BOARD_TYPE_HDU2_S) )
	{
		*pbyType = BRD_TYPE_HDU2_S;
		return TRUE;
	}
	else if( 0 == strcmp(lpzName, BOARD_TYPE_EBAP) )
	{
		*pbyType = BRD_HWID_EBAP;
		return TRUE;
	}
	else if( 0 == strcmp(lpzName, BOARD_TYPE_EVPU) )
	{
		*pbyType = BRD_HWID_EVPU;
		return TRUE;
	}
	else if( 0 == strcmp(lpzName, BOARD_TYPE_IS21) )
	{
		*pbyType = BRD_TYPE_IS21;
		return TRUE;
	}
	// [pengjie 2010/12/9] IS22
	else if( 0 == strcmp(lpzName, BOARD_TYPE_IS22) )
	{
		*pbyType = BRD_TYPE_IS22;
		return TRUE;
	}
    // End
	else 
	{
		return FALSE;
	}
}

/*=============================================================================
函 数 名： AgentGetBrdCfgTable
功    能： 取单板配置信息
算法实现： 
全局变量： 
参    数：  const s8* lpszProfileName
TEqpBrdCfgEntry ***mcueqpBrdCfgTable
u32 *dwmcueqpBrdCfgEntryNum
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::AgentGetBrdCfgTable( const s8* lpszProfileName  )
{
    if( NULL == lpszProfileName )
	{
		printf("[AgentGetBrdCfgTable] Error input param.\n");
		return FALSE;
	}
	
	s8*   *ppszTable = NULL;        // 临时空间
    s8    achSeps[] = " \t";        // 分隔符
    s8    *pchToken = NULL;
    u32   dwLoop;
    s32   nMemEntryNum;
    u8    byBrdType = 0;
    BOOL32  bResult = TRUE;
    BOOL32  bSucceedRead = TRUE;
	
	TBrdPosition tPos;
    memset( &tPos, 0, sizeof(tPos) );
	
    BOOL32 bIsValidBrd = TRUE;
    BOOL32 bError = FALSE;
    u8 byEntryIndex = 0;
	
    // 单板数
    bResult = GetRegKeyInt( lpszProfileName, SECTION_mcueqpBoardConfig, 
		STR_ENTRY_NUM, 0, &nMemEntryNum );
    if( !bResult ) 
    {
		printf("[AgentGetBrdCfgTable] Wrong profile while reading %s %s!\n", 
			SECTION_mcueqpBoardConfig, STR_ENTRY_NUM );
    }
    if( 0 == nMemEntryNum )
    {
        printf("The Board Num cannot be 0 \n");
        return FALSE;
    }
	
	// alloc memory
    ppszTable = new s8*[(u32)nMemEntryNum];
    if( NULL == ppszTable )
    {
        Agtlog(LOG_INFORM, "[AgentGetBrdCfgTable] Fail to malloc memory \n");
        return  FALSE;
    }
	
    for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
    {
        ppszTable[dwLoop] = new s8[MAX_VALUE_LEN+1];
        if(NULL == ppszTable[dwLoop])
        {
            printf("[AgentGetBrdCfgTable] Fail to malloc memory for board table\n");
            FREE_TABLE_MEM( ppszTable, dwLoop );
			return  FALSE;
        }
    }
	
    m_dwBrdCfgEntryNum = nMemEntryNum;
    bResult = AgtGetRegKeyStringTable( lpszProfileName, SECTION_mcueqpBoardConfig,
		"fail", ppszTable, &m_dwBrdCfgEntryNum, MAX_VALUE_LEN + 1 );
    if( !bResult ) 
    {
		printf("[AgentGetBrdCfgTable] Wrong profile while reading %s table!\n", 
			SECTION_mcueqpBoardConfig );

		LogPrint(LOG_LVL_ERROR,  MID_PUB_ALWAYS, "[AgentGetBrdCfgTable] Wrong profile while reading %s table!\n", 
			SECTION_mcueqpBoardConfig );

		FREE_TABLE_MEM( ppszTable, nMemEntryNum );  // 释放空间
		m_dwBrdCfgEntryNum = 0;
		g_cAlarmProc.ProcReadError( ERR_AGENT_READBOARDCFG );
		return FALSE;
    }
	
	// zgc, 2008-05-14
	u8 byLocalMpcIdx = MAX_BOARD_NUM;
	u8 byOtherMpcIdx = MAX_BOARD_NUM;
	TEqpBrdCfgEntry tLocalMpcCfg;
	TEqpBrdCfgEntry tOtherMpcCfg;	
	
    // analyze entry strings 
    for( dwLoop = 0; dwLoop < m_dwBrdCfgEntryNum; dwLoop++ )
    {
        // BoardId 
        pchToken = strtok( ppszTable[dwLoop], achSeps );
        if (NULL == pchToken)
        {
            Agtlog(LOG_WARN, "[AgentGetBrdCfgTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpBoardId );

			LogPrint(LOG_LVL_ERROR,  MID_PUB_ALWAYS, "[AgentGetBrdCfgTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpBoardId );

            bResult = FALSE;
            continue;
        }
        else
        {
            m_atBrdCfgTable[dwLoop].SetBrdId( atoi(pchToken) );
        }
		
		// Layer
        pchToken = strtok( NULL, achSeps );
        if (NULL == pchToken)
        {
            printf("[AgentGetBrdCfgTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpBoardLayer );

			LogPrint(LOG_LVL_ERROR,  MID_PUB_ALWAYS, "[AgentGetBrdCfgTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpBoardLayer );

            bResult = FALSE;
            continue;
        }
        else
        {
			m_atBrdCfgTable[dwLoop].SetLayer(atoi(pchToken) );
        }
		
		// Slot 
        pchToken = strtok( NULL, achSeps );
        if (NULL == pchToken)
        {
            printf("[AgentGetBrdCfgTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpBoardSlot );

			LogPrint(LOG_LVL_ERROR,  MID_PUB_ALWAYS, "[AgentGetBrdCfgTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpBoardSlot );

            bResult = FALSE;
            continue;
        }
        else
        {
            u8 bySlot = 0;
			StrUpper( pchToken );
			
			if( !GetSlotThroughName( pchToken, &bySlot ) )
			{
				printf("[AgentGetBrdCfgTable] Wrong Value %s = %s !\n", 
					FIELD_mcueqpBoardSlot, pchToken );

				LogPrint(LOG_LVL_ERROR,  MID_PUB_ALWAYS, "[AgentGetBrdCfgTable] Wrong Value %s = %s !\n", 
					FIELD_mcueqpBoardSlot, pchToken );

				bResult = FALSE;
			}
            m_atBrdCfgTable[dwLoop].SetSlot( bySlot );
        }
		
		// Type 
        pchToken = strtok( NULL, achSeps );
        if (NULL == pchToken)
        {
            printf("[AgentGetBrdCfgTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpBoardType );

			LogPrint(LOG_LVL_ERROR,  MID_PUB_ALWAYS, "[AgentGetBrdCfgTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpBoardType );

            bResult = FALSE;
            continue;
        }
        else
        {
			StrUpper( pchToken );
			if( !GetTypeThroughName( pchToken, &byBrdType ) )
			{
				printf("[AgentGetBrdCfgTable] Wrong Value %s = %s !\n", 
					FIELD_mcueqpBoardType, pchToken );

				LogPrint(LOG_LVL_ERROR,  MID_PUB_ALWAYS, "[AgentGetBrdCfgTable] Wrong Value %s = %s !\n", 
					FIELD_mcueqpBoardType, pchToken );

				bResult = FALSE;
			}
            m_atBrdCfgTable[dwLoop].SetType(byBrdType);
            // 暂时用单板类型作为别名，如果用户后面配置了别名，则用别名替换
            m_atBrdCfgTable[dwLoop].SetAlias( pchToken );
        }
        
		// IpAddr
        pchToken = strtok( NULL, achSeps );
        if (NULL == pchToken)
        {
            printf("[AgentGetBrdCfgTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpBoardIpAddr );

			LogPrint(LOG_LVL_ERROR,  MID_PUB_ALWAYS, "[AgentGetBrdCfgTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpBoardIpAddr );

            bResult = FALSE;
            continue;
        }
        else
        {
			m_atBrdCfgTable[dwLoop].SetBrdIp( ntohl(INET_ADDR(pchToken)) );
        }
		
        // 前后网口，可以没有此字段
		
        pchToken = strtok(NULL, achSeps);
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
		// [4/13/2010 xliang] 8ke mpc不参考配置文件, hdu参考配置文件
		u8 byPortChoice = 0;
		if( byBrdType == BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ ||
			BRD_TYPE_MPC2 == byBrdType )
		{
			// [3/4/2011 xliang] confirm before in Ethinit(), don't change it
			byPortChoice = m_tMPCInfo.GetLocalPortChoice();
			//byPortChoice = Search8KEPortChoice();
			//m_tMPCInfo.SetLocalPortChoice( byPortChoice );
		}
		else
		{
			if(NULL == pchToken)
			{
				byPortChoice = 1;
			}
			else
			{
				byPortChoice = atoi(pchToken);
			}
		}
		m_atBrdCfgTable[dwLoop].SetPortChoice( byPortChoice );
		
#else
		//  [1/21/2011 chendaiwei]支持MPC2
		if (BRD_TYPE_MPC == byBrdType || BRD_TYPE_MPC2 == byBrdType)
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[AgentGetBrdCfgTable] byBrdType is Mpc or Mpc2!\n");
			u8 byMpcPortChoice = SearchPortChoice( m_atBrdCfgTable[dwLoop].GetBrdIp() );
			if ( byMpcPortChoice == 0xff )
			{
#ifdef _MINIMCU_
				byMpcPortChoice = 0;
#else
				byMpcPortChoice = 1;
#endif
			}
			m_atBrdCfgTable[dwLoop].SetPortChoice( byMpcPortChoice );
			if (OspIsLocalHost( htonl(m_atBrdCfgTable[dwLoop].GetBrdIp()) ) )
			{
				LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[AgentGetBrdCfgTable] OspIsLocalHost, so SetLocalPortChoice.%d!\n", byMpcPortChoice);
				m_tMPCInfo.SetLocalPortChoice( byMpcPortChoice );
			}
			else
			{
				LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[AgentGetBrdCfgTable] !OspIsLocalHost!\n");
			}
		}
		else
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[AgentGetBrdCfgTable] byBrdType is not Mpc or Mpc2!\n");
			if ( NULL == pchToken )
			{
				u8 byPortChoice = SearchPortChoice( m_atBrdCfgTable[dwLoop].GetBrdIp() );
				if ( byPortChoice == 0xff )
				{
#ifdef _MINIMCU_
					byPortChoice = 0;
#else
					byPortChoice = 1;
#endif
				}
				m_atBrdCfgTable[dwLoop].SetPortChoice( byPortChoice );
				
			}
			else
			{
				m_atBrdCfgTable[dwLoop].SetPortChoice( atoi(pchToken) );
			}
		}
#endif
        // 是否组播，可以没有此字段
        pchToken = strtok(NULL, achSeps);
        if( NULL == pchToken)
        {
            m_atBrdCfgTable[dwLoop].SetCastChoice( 0 );
        }
        else
        {
			//  [1/21/2011 chendaiwei]新增CRI2，DRI2，MPC2支持
            if( byBrdType == BRD_TYPE_DRI/*DSL8000_BRD_DRI*/ || byBrdType == BRD_TYPE_CRI/*DSL8000_BRD_CRI*/ || byBrdType == BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ ||
				byBrdType == BRD_TYPE_16E1/*DSL8000_BRD_16E1*/ || byBrdType == BRD_TYPE_CRI2 || byBrdType == BRD_TYPE_DRI2 || byBrdType == BRD_TYPE_MPC2
#ifdef _MINIMCU_
				|| byBrdType == BRD_TYPE_MDSC/*DSL8000_BRD_MDSC*/ || byBrdType == BRD_TYPE_HDSC/*DSL8000_BRD_HDSC*/ || byBrdType ==  BRD_TYPE_DSC/*KDV8000B_MODULE*/ 
#endif
				)
				// cri dri mpc dsc mdsc hdsc cri2
            {
                u8 byCastChoice = atoi( pchToken );
                if( byCastChoice == 0 || byCastChoice == 1)
                {
                    m_atBrdCfgTable[dwLoop].SetCastChoice( byCastChoice );
                }
                else
                {
                    printf("[AgentGetBrdCfgTable] The board's cast choice is not legality\n");
                    m_atBrdCfgTable[dwLoop].SetCastChoice( 0 );
                }
            }
        }
        
        // 商用名
        pchToken = strtok(NULL, achSeps);
        if( NULL != pchToken)
        {
            m_atBrdCfgTable[dwLoop].SetAlias( pchToken );
        }
        else
        {
            // 采用前面默认别名            
        }
		
		// init other item 
		m_atBrdCfgTable[dwLoop].SetState( BOARD_STATUS_DISCONNECT );
		m_atBrdCfgTable[dwLoop].SetEntParamNull();
        m_atBrdCfgTable[dwLoop].SetPeriIdNull();

		// 判断配置和实际是否一致, 并写入 TMPCInfo
		//  [1/21/2011 chendaiwei]支持MPC2
		if( BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ == m_atBrdCfgTable[dwLoop].GetType() || BRD_TYPE_MPC2 == m_atBrdCfgTable[dwLoop].GetType())
		{
			
			//#ifndef WIN32  // vx linux
#ifndef _MINIMCU_
			if( OK == AgtQueryBrdPosition(&tPos) )
            {
                printf("[AgentGetBrdCfgTable] Layer= %d, Slot= %d, Id= %d\n", 
					tPos.byBrdLayer, tPos.byBrdSlot, tPos.byBrdID );                 
                
				LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[AgentGetBrdCfgTable] Layer= %d, Slot= %d, Id= %d\n", 
					tPos.byBrdLayer, tPos.byBrdSlot, tPos.byBrdID );      

				// 槽位号一致, LocalMPC
				if( 
#ifdef _LINUX12_
					tPos.byBrdID == BRD_HWID_MPC2 &&					
#else
					tPos.byBrdID == BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ &&					
#endif
#ifndef WIN32 
					tPos.byBrdSlot == m_atBrdCfgTable[dwLoop].GetSlot()
#else
					OspIsLocalHost( htonl(m_atBrdCfgTable[dwLoop].GetBrdIp()))
#endif
					)
				{
					// zgc, 2008-05-14
					byLocalMpcIdx = (u8)dwLoop;
					memcpy( &tLocalMpcCfg, &m_atBrdCfgTable[dwLoop], sizeof(tLocalMpcCfg) );
				}
                // 跟本机槽号不一致, OtherMPC
				else if (
#ifdef _LINUX12_
					tPos.byBrdID == BRD_HWID_MPC2 					
#else
					tPos.byBrdID == BRD_TYPE_MPC/*DSL8000_BRD_MPC*/					
#endif 
					)
				{	
                    u8  byTmpSlot = (tPos.byBrdSlot == 0) ? 1 : 0;      // 判断另一块Mpc板应该在哪一槽位

                    printf("[AgentGetBrdCfgTable] Other mpc's slot= %d\n", byTmpSlot);
                    LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS,  "[AgentGetBrdCfgTable] Other mpc's slot= %d\n", byTmpSlot);

					// if( byTmpSlot == m_atBrdCfgTable[dwLoop].GetSlot() ) // 保护， 防乱配置Mpc板
					if( 
#ifndef WIN32
						byTmpSlot == m_atBrdCfgTable[dwLoop].GetSlot()
#else
						!OspIsLocalHost( htonl(m_atBrdCfgTable[dwLoop].GetBrdIp()))
#endif					
						) 
                    {
						// zgc, 2008-05-14
						byOtherMpcIdx = (u8)dwLoop;
						memcpy( &tOtherMpcCfg, &m_atBrdCfgTable[dwLoop], sizeof(tOtherMpcCfg) );
                    }
                    else
                    {
                        printf( "[AgentGetBrdCfgTable] Configed Mpc( Layer=%d, Slot=%d ) Conflict with Actual Mpc( Layer=%d, Slot=%d)!\n",
																		m_atBrdCfgTable[dwLoop].GetLayer(), 
																		m_atBrdCfgTable[dwLoop].GetSlot(),
																		tPos.byBrdLayer, tPos.byBrdSlot );

						LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS,  "[AgentGetBrdCfgTable] Configed Mpc( Layer=%d, Slot=%d ) Conflict with Actual Mpc( Layer=%d, Slot=%d)!\n",
																		m_atBrdCfgTable[dwLoop].GetLayer(), 
																		m_atBrdCfgTable[dwLoop].GetSlot(),
																		tPos.byBrdLayer, tPos.byBrdSlot );
                    }
                }
			}			
#else
			// mcu8000b 
			// zgc, 2008-05-14
			byLocalMpcIdx = (u8)dwLoop;
			memcpy( &tLocalMpcCfg, &m_atBrdCfgTable[dwLoop], sizeof(tLocalMpcCfg) );			

#endif  // _MINIMCU_	 
		}        
    }
	
	BOOL32 bSaveMpcSuccess = FALSE;
	LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[AgentGetBrdCfgTable] byLocalMpcIdx.%d, byOtherMpcIdx.%d!\n",
								byLocalMpcIdx, byOtherMpcIdx);
	// [pengjie 2011/6/13] 保存mpc配置调整
	if( (byLocalMpcIdx != MAX_BOARD_NUM && byOtherMpcIdx == MAX_BOARD_NUM) )
	{
		bSaveMpcSuccess = SaveMpcInfo( byLocalMpcIdx, &tLocalMpcCfg );
	}
	else if( byLocalMpcIdx == MAX_BOARD_NUM && byOtherMpcIdx != MAX_BOARD_NUM )
	{
		tOtherMpcCfg.SetSlot( tPos.byBrdSlot );
		bSaveMpcSuccess = SaveMpcInfo( byOtherMpcIdx, &tOtherMpcCfg );
	}
	else if( byLocalMpcIdx != MAX_BOARD_NUM && byOtherMpcIdx != MAX_BOARD_NUM )
	{
		bSaveMpcSuccess = SaveMpcInfo( byLocalMpcIdx, &tLocalMpcCfg, byOtherMpcIdx, &tOtherMpcCfg );
	}
	else
	{
		printf( "[AgentGetBrdCfgTable] SaveMpcInfo LocalMpcIdx and OtherMpcIdx == MAX_BOARD_NUM!\n" );
	}
	
	// 	if ( byOtherMpcIdx == MAXNUM_BOARD )
	// 	{
	// 		bSaveMpcSuccess = SaveMpcInfo( byLocalMpcIdx, &tLocalMpcCfg );
	// 	}
	// 	else
	// 	{
	// 		bSaveMpcSuccess = SaveMpcInfo( byLocalMpcIdx, &tLocalMpcCfg, byOtherMpcIdx, &tOtherMpcCfg );
	// 	}
	
	if ( !bSaveMpcSuccess )
	{
		printf( "[AgentGetBrdCfgTable] SaveMpcInfo failed!\n" );
	}
	
	/*
    if( bError )
    {
	TBoardInfo tBoardInfo;
	tBoardInfo.SetBrdId( m_atBrdCfgTable[byEntryIndex].GetBrdId() );
	tBoardInfo.SetLayer( m_atBrdCfgTable[byEntryIndex].GetLayer() );
	tBoardInfo.SetSlot( m_atBrdCfgTable[byEntryIndex].GetSlot() );
	tBoardInfo.SetType( m_atBrdCfgTable[byEntryIndex].GetType() );
	tBoardInfo.SetCastChoice( m_atBrdCfgTable[byEntryIndex].GetCastChoice() );
	tBoardInfo.SetPortChoice( m_atBrdCfgTable[byEntryIndex].GetPortChoice() );
	tBoardInfo.SetBrdIp( m_atBrdCfgTable[byEntryIndex].GetBrdIp() );
	tBoardInfo.SetAlias( m_atBrdCfgTable[byEntryIndex].GetAlias() );
	
	  WriteBoardTable( byEntryIndex, &tBoardInfo, FALSE );
	  }
	*/
	
    if( !bResult)
    {
        g_cAlarmProc.ProcReadError(ERR_AGENT_READBOARDCFG);
    }
	
    FREE_TABLE_MEM( ppszTable, nMemEntryNum );
    
    //return TRUE;	
	return bResult;
}

/*=============================================================================
函 数 名： AgentGetMixerTable
功    能： 取Mixer配置信息
算法实现： 
全局变量： 
参    数：  const s8* lpszProfileName
TEqpMixerEntry ***ptmcuEqpMixerTable
u32 *dwmcueqpMixerEntryNum
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::AgentGetMixerTable( const s8* lpszProfileName )
{
    if( NULL == lpszProfileName )
	{
		printf("[AgentGetMixerTable] Error input param.\n");
		return FALSE;
	}
	
	s8*   *ppszTable = NULL;
    s8    achSeps[] = " \t";       // 
    s8    *pchToken = NULL;
    u32   dwLoop;
	u32	  dwBrdLoop;
    s32   nMemEntryNum;
	
	BOOL32 bIsPrintError = FALSE;
	
    // get the number of entry
    BOOL32 bResult = GetRegKeyInt( lpszProfileName, SECTION_mcueqpMixerTable, 
		STR_ENTRY_NUM, 0, &nMemEntryNum );
	if( !bResult ) 
	{
		printf( "[AgentGetMixerTable] Wrong profile while reading %s %s!\n", 
			SECTION_mcueqpMixerTable, STR_ENTRY_NUM );
        g_cAlarmProc.ProcReadError(ERR_AGENT_READMIXCFG);
		return FALSE;
	}
    
    // 没有配置混音器
    if(0 == nMemEntryNum )
    {
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
		m_atMixTable[0].SetEqpId(MIXERID_MIN);
		m_atMixTable[0].SetMcuRecvPort(MIXER_MCU_STARTPORT);
		m_atMixTable[0].SetSwitchBrdId(m_atBrdCfgTable[0].GetBrdId());
		m_atMixTable[0].SetAlias((LPCSTR)"MIXER");
		m_atMixTable[0].SetRunBrdId(m_atBrdCfgTable[0].GetBrdId());
		m_atMixTable[0].SetEqpRecvPort(MIXER_EQP_STARTPORT);
		m_atMixTable[0].SetMaxChnInGrp(MAXNUM_MIXER_CHNNL);
		m_dwMixEntryNum = 1;
#else
        m_dwMixEntryNum = 0;
#endif
        return TRUE;
    }
	
	// alloc memory
    ppszTable = new s8*[(u32)nMemEntryNum];
    if( NULL == ppszTable )
    {
		printf("[AgentGetMixerTable] Fail to malloc the memory for mixer table\n");
		return FALSE;
    }
    for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
    {
        ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1 ];
        if( NULL == ppszTable[dwLoop] )
        {
            printf("[AgentGetMixerTable] Fail to malloc the memory for mixer table\n");
            FREE_TABLE_MEM( ppszTable, dwLoop );
			return FALSE;
        }
    }
	
	// get the mixer table
    m_dwMixEntryNum = nMemEntryNum;
    bResult = AgtGetRegKeyStringTable( lpszProfileName, SECTION_mcueqpMixerTable,
		"fail", ppszTable, &m_dwMixEntryNum, MAX_VALUE_LEN + 1 );
    if( !bResult )
    {
		printf( "[AgentGetMixerTable] Wrong profile while reading %s table!\n", 
			SECTION_mcueqpMixerTable );
		
        FREE_TABLE_MEM( ppszTable, nMemEntryNum );
        m_dwMixEntryNum = 0;
        g_cAlarmProc.ProcReadError(ERR_AGENT_READMIXCFG);
        return FALSE;
    }
	
    for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
    {		
        // MixerId
        pchToken = strtok( ppszTable[dwLoop], achSeps );
        if( NULL == pchToken)
        {
            printf("[AgentGetMixerTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpMixerEntId );
            bResult = FALSE;
			bIsPrintError = TRUE;
        }
        else
        {
            m_atMixTable[dwLoop].SetEqpId( atoi(pchToken) );
        }
		
		// Port
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken)
        {
            printf("[AgentGetMixerTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpMixerEntPort );
            bResult = FALSE;
			bIsPrintError = TRUE;
            continue;
        }
        else
        {
            m_atMixTable[dwLoop].SetMcuRecvPort( atoi(pchToken) );
        }
		
		// SwitchBrdId
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetMixerTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpMixerEntSwitchBrdId );
            bResult = FALSE;
			bIsPrintError = TRUE;
            continue;
        }
        else
        {
            m_atMixTable[dwLoop].SetSwitchBrdId( atoi(pchToken) );
        }
		
		// Alias
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken)
        {
            printf("[AgentGetMixerTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpMixerEntAlias );
            bResult = FALSE;
			bIsPrintError = TRUE;
            continue;
        }
        else
        {
			if ( IsUtf8Encoding())
			{
				printf("[AgentGetMixerTable]Mixer Alias:%d is UTF8\n", dwLoop);
				m_atMixTable[dwLoop].SetAlias(pchToken);
			}
			else
			{
				printf("[AgentGetMixerTable]Mixer Alias:%d is GBK\n", dwLoop);
				char achAlias[MAXLEN_EQP_ALIAS+1];
				memset(achAlias, 0 , sizeof(achAlias));
				gb2312_to_utf8(pchToken, achAlias, MAXLEN_EQP_ALIAS);
				m_atMixTable[dwLoop].SetAlias(achAlias);
			}
        }
		
		// RunningBrdId
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken)
        {
            printf("[AgentGetMixerTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpMixerEntRunningBrdId );
            bResult = FALSE;
			bIsPrintError = TRUE;
            continue;
        }
        else
        {
            m_atMixTable[dwLoop].SetRunBrdId( atoi(pchToken) );
			for( dwBrdLoop = 0; dwBrdLoop < m_dwBrdCfgEntryNum; dwBrdLoop++)
			{
				if( m_atBrdCfgTable[dwBrdLoop].GetBrdId() == m_atMixTable[dwLoop].GetRunBrdId() )
				{
					m_atMixTable[dwLoop].SetIpAddr( m_atBrdCfgTable[dwBrdLoop].GetBrdIp() );
					bResult = m_atBrdCfgTable[dwBrdLoop].SetPeriId( m_atMixTable[dwLoop].GetEqpId() );
					break;
				}
			}
			if( dwBrdLoop >= m_dwBrdCfgEntryNum )
			{
				printf("[AgentGetMixerTable] Wrong Mixer Run Board Value %s = %s!\n", 
					FIELD_mcueqpMixerEntRunningBrdId, pchToken);
				bResult = FALSE;
				bIsPrintError = TRUE;
			}
        }
		
		// MixStartPort
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken)
        {
            printf("[AgentGetMixerTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpMixerEntMixStartPort );
            bResult = FALSE;
			bIsPrintError = TRUE;
            continue;
        }
        else
        {
            m_atMixTable[dwLoop].SetEqpRecvPort( atoi(pchToken) );
        }
		
		// MaxChnInGrp
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken)
        {
            printf("[AgentGetMixerTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpMixerEntMaxChannelInGrp );
            bResult = FALSE;
			bIsPrintError = TRUE;
            continue;
        }
        else
        {
			m_atMixTable[dwLoop].SetMaxChnInGrp( atoi(pchToken) );
        }
		// 每个MAP现在只能为1
		m_atMixTable[dwLoop].SetMaxMixGrpNum( 1 );
		
		// 取Mixer使用的Map号
		pchToken = strtok( NULL, achSeps );
		s8 achTemSeps[] = ",";
		s8 *pchTemToken = NULL;
		s8 achMapId[MAX_VALUE_LEN+1] = {0};
		if ( NULL == pchToken )
		{
			printf("[AgentGetMixerTable] pchToken, Wrong profile while reading %s!\n", 
				FIELD_mcueqpMixerEntMaxChannelInGrp );
            bResult = FALSE;
			bIsPrintError = TRUE;
            continue;
		}
		else
		{						
			strncpy( achMapId, pchToken, strlen(pchToken) );
			achMapId[strlen(pchToken)] = '\0';
		}
		
		// 设置mapid
		pchTemToken = strtok( achMapId, achTemSeps );
		if ( NULL == pchTemToken )
		{
			printf("[AgentGetMixerTable] pchTemToken, Wrong profile while reading %s!\n", 
				FIELD_mcueqpMixerEntMaxChannelInGrp );
			bResult = FALSE;
			bIsPrintError = TRUE;
			continue;
		}
		else
		{
			m_atMixTable[dwLoop].SetUsedMapId( atoi(pchTemToken) );
			while( ( pchTemToken = strtok( NULL, achTemSeps ) ) != NULL )
			{			
				m_atMixTable[dwLoop].SetUsedMapId( atoi(pchTemToken) );
			}
		}
		if( m_atMixTable[dwLoop].GetUsedMapNum() == 0 )
		{
            printf("[AgentGetMixerTable] GetUsedMapNum = 0, Wrong profile while reading %s!\n", 
				FIELD_mcueqpMixerEntMapId );
			bResult = FALSE;
			bIsPrintError = TRUE;
			continue;
		}	
		
        //tianzhiyong  2010/03/09 按照板子的ID查找相应的板类型，初始化其他参数 
		for ( u8 byBrdNum = 0 ; byBrdNum < MAX_BOARD_NUM ;byBrdNum++)
		{
			if (m_atBrdCfgTable[byBrdNum].GetBrdId() == m_atMixTable[dwLoop].GetRunBrdId())
			{
				m_atMixTable[dwLoop].SetDefaultEntParam( EQP_TYPE_MIXER, ntohl(GetLocalIp()) );

				break;
			}
		}
	}

    if( bIsPrintError )
    {
        g_cAlarmProc.ProcReadError(ERR_AGENT_READMIXCFG);
    }
    FREE_TABLE_MEM( ppszTable, nMemEntryNum );
    
    //return TRUE;
	return bResult;
}

/*=============================================================================
函 数 名： AgentGetRecorderTable
功    能： 取Recorder配置信息
算法实现： 
全局变量： 
参    数：  const s8* lpszProfileName
TEqpRecEntry ***mcueqpRecorderTable
u32 *dwmcueqpRecorderEntryNum
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::AgentGetRecorderTable( const s8* lpszProfileName )
{
	if( NULL == lpszProfileName )
	{
		printf("[AgentGetRecorderTable] Error input param \n");
		return FALSE;
	}	
	
    s8*   *ppszTable = NULL;
    s8    achSeps[] = " \t";       // 分隔符
    s8    *pchToken = NULL;
    u32   dwLoop;
    BOOL32  bResult = TRUE;
    s32   nMemEntryNum;
    
    // 取录像机数目
    bResult = GetRegKeyInt( lpszProfileName, SECTION_mcueqpRecorderTable, 
		STR_ENTRY_NUM, 0, &nMemEntryNum );
	if( !bResult) 
	{
		printf( "[AgentGetRecorderTable] Wrong profile while reading %s %s!\n", 
			SECTION_mcueqpRecorderTable, STR_ENTRY_NUM );
        g_cAlarmProc.ProcReadError(ERR_AGENT_READRECORDERCFG);
		return FALSE;
	}
	
    // 没有配置录像机
    if( 0 == nMemEntryNum)
    {
        m_dwRecEntryNum = 0;
        return TRUE;
    }
	
	// 临时空间
    ppszTable = new s8*[(u32)nMemEntryNum];
    if(NULL == ppszTable )
    {
        printf("[AgentGetRecorderTable] Fail to malloc memory for recorder table\n");
        return FALSE;
    }
    for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
    {
        ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
        if(NULL == ppszTable )
        {
            printf("[AgentGetRecorderTable] Fail to malloc memory for recorder table\n");
            FREE_TABLE_MEM( ppszTable, dwLoop );
			return FALSE;
        }
    }
	
    m_dwRecEntryNum = nMemEntryNum;
	
    bResult = AgtGetRegKeyStringTable( lpszProfileName, SECTION_mcueqpRecorderTable,
		"fail", ppszTable, &m_dwRecEntryNum, MAX_VALUE_LEN + 1 );
    if( !bResult )
    {
		printf("[AgentGetRecorderTable] Wrong profile while reading %s table!\n", 
			SECTION_mcueqpRecorderTable);
		
        FREE_TABLE_MEM( ppszTable, nMemEntryNum );
        m_dwRecEntryNum = 0;
        g_cAlarmProc.ProcReadError(ERR_AGENT_READRECORDERCFG);
        return FALSE;
    }
	
    for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
    {
        // Id 
        pchToken = strtok( ppszTable[dwLoop], achSeps );
        if( NULL == pchToken)
        {
            printf("[AgentGetRecorderTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpRecorderEntId);
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atRecTable[dwLoop].SetEqpId( atoi(pchToken) );
        }
		
        // Port 
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetRecorderTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpRecorderEntPort);
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atRecTable[dwLoop].SetMcuRecvPort( atoi(pchToken) );
        }
		
		// SwitchBrdId 
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken)
        {
            printf("[AgentGetRecorderTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpRecorderEntSwitchBrdId );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atRecTable[dwLoop].SetSwitchBrdId( atoi(pchToken) );
        }
		
        // Alias
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetRecorderTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpRecorderEntAlias );
            bResult = FALSE;
            continue;
        }
        else
        {
			if ( IsUtf8Encoding())
			{
				printf("[AgentGetRecorderTable]Rec Alias:%d is UTF8\n", dwLoop);
				m_atRecTable[dwLoop].SetAlias( pchToken );
			}
			else
			{
				printf("[AgentGetRecorderTable]Rec Alias:%d is GBK\n", dwLoop);
				char achAlias[MAXLEN_EQP_ALIAS+1];
				memset(achAlias, 0 , sizeof(achAlias));
				gb2312_to_utf8(pchToken, achAlias, MAXLEN_EQP_ALIAS);
				m_atRecTable[dwLoop].SetAlias( achAlias );
			}
        }
		
		// IpAddr
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetRecorderTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpRecorderEntIpAddr );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atRecTable[dwLoop].SetIpAddr( ntohl(INET_ADDR(pchToken)) );
        }
		
		// RecvStartPort
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken)
        {
            printf("[AgentGetRecorderTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpRecorderEntRecvStartPort );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atRecTable[dwLoop].SetEqpRecvPort( atoi(pchToken) );
        }
        // 初始化其他参数
        m_atRecTable[dwLoop].SetDefaultEntParam( EQP_TYPE_RECORDER, ntohl(GetLocalIp()) );
	}

    if( !bResult )
    {
        g_cAlarmProc.ProcReadError(ERR_AGENT_READRECORDERCFG);
    }
	// free memory
    FREE_TABLE_MEM( ppszTable, nMemEntryNum );
    
    //return TRUE;
	return bResult;
}

/*=============================================================================
函 数 名： AgentGetVrsRecorderTable
功    能： 读取Vrs新录播信息
算法实现： 
全局变量： 
参    数：  const s8* lpszProfileName
			TEqpRecEntry ***mcueqpVrsRecorderTable
			u32 *dwmcueqpVrsRecorderEntryNum
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::AgentGetVrsRecorderTable( const s8* lpszProfileName )
{
	if( NULL == lpszProfileName )
	{
		printf("[AgentGetVrsRecorderTable] Error input param \n");
		return FALSE;
	}	
	
    s8*   *ppszTable = NULL;
    s8    achSeps[] = " \t";       // 分隔符
    s8    *pchToken = NULL;
    u32   dwLoop;
    BOOL32  bResult = TRUE;
    s32   nMemEntryNum;
    
    // 取录像机数目
    bResult = GetRegKeyInt( lpszProfileName, SECTION_mcueqpVrsRecorderTable, 
		STR_ENTRY_NUM, 0, &nMemEntryNum );
	if( !bResult) 
	{
		printf( "[AgentGetVrsRecorderTable] Wrong profile while reading %s %s!\n", 
			SECTION_mcueqpVrsRecorderTable, STR_ENTRY_NUM );
        g_cAlarmProc.ProcReadError(ERR_AGENT_READRECORDERCFG);
		return FALSE;
	}
	
    // 没有配置录像机
    if( 0 == nMemEntryNum)
    {
        m_dwVrsRecEntryNum = 0;
        return TRUE;
    }
	
	// 临时空间
    ppszTable = new s8*[(u32)nMemEntryNum];
    if(NULL == ppszTable )
    {
        printf("[AgentGetVrsRecorderTable] Fail to malloc memory for vrsrecorder table\n");
        return FALSE;
    }
    for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
    {
        ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
        if(NULL == ppszTable )
        {
            printf("[AgentGetVrsRecorderTable] Fail to malloc memory for vrsrecorder table\n");
            FREE_TABLE_MEM( ppszTable, dwLoop );
			return FALSE;
        }
    }
	
    m_dwVrsRecEntryNum = nMemEntryNum;
	
    bResult = AgtGetRegKeyStringTable( lpszProfileName, SECTION_mcueqpVrsRecorderTable,
		"fail", ppszTable, &m_dwVrsRecEntryNum, MAX_VALUE_LEN + 1 );
    if( !bResult )
    {
		printf("[AgentGetVrsRecorderTable] Wrong profile while reading %s table!\n", 
			SECTION_mcueqpVrsRecorderTable);
		
        FREE_TABLE_MEM( ppszTable, nMemEntryNum );
        m_dwVrsRecEntryNum = 0;
        g_cAlarmProc.ProcReadError(ERR_AGENT_READRECORDERCFG);
        return FALSE;
    }
	
	u8 byVrsId;
	TMtAlias tVrsCallAlias;
	u8 byVrsCallAliasType;
	TTransportAddr tTransportAddr;
    for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
    {
		byVrsId = 0;
		tVrsCallAlias.SetNull();
		byVrsCallAliasType = mtAliasTypeBegin;
		tTransportAddr.SetNull();

        // Vrs索引id 
        pchToken = strtok( ppszTable[dwLoop], achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetVrsRecorderTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpVrsRecorderEntId);
            bResult = FALSE;
            continue;
        }
        else
        {
			byVrsId = atoi(pchToken);
			m_atVrsRecTable[dwLoop].SetVrsId(byVrsId);
        }
		
        // Vrs新录播显示别名
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken)
        {
            printf("[AgentGetVrsRecorderTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpVrsRecorderEntAlias);
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atVrsRecTable[dwLoop].SetVrsAlias( pchToken );
        }
		
        // 呼叫类型 
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetVrsRecorderTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpVrsRecorderEntCallAliasType);
            bResult = FALSE;
            continue;
        }
        else
        {
			byVrsCallAliasType = atoi(pchToken);
        }
		
		// ip地址或e164号
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken)
        {
			if (mtAliasTypeTransportAddress == byVrsCallAliasType)
			{
				printf("[AgentGetVrsRecorderTable] Wrong profile while reading %s!\n", 
					FIELD_mcueqpVrsRecorderEntIpAddr );
			}
			else if (mtAliasTypeE164 == byVrsCallAliasType)
			{
				printf("[AgentGetVrsRecorderTable] Wrong profile while reading %s!\n", 
					FIELD_mcueqpVrsRecorderEntE164Number );
			}
            bResult = FALSE;
            continue;
        }
        else
        {
			if (mtAliasTypeTransportAddress == byVrsCallAliasType)
			{
				tTransportAddr.SetIpAddr(ntohl(INET_ADDR(pchToken)));
				tVrsCallAlias.m_AliasType = mtAliasTypeTransportAddress;
				tVrsCallAlias.m_tTransportAddr = tTransportAddr;
			}
			else if (mtAliasTypeE164 == byVrsCallAliasType)
			{
				tVrsCallAlias.SetE164Alias(pchToken);
			}
            m_atVrsRecTable[dwLoop].SetVrsCallAlias(tVrsCallAlias);
        }
	}

    if( !bResult )
    {
        g_cAlarmProc.ProcReadError(ERR_AGENT_READRECORDERCFG);
    }
	// free memory
    FREE_TABLE_MEM( ppszTable, nMemEntryNum );
    
    //return TRUE;
	return bResult;
}

/*=============================================================================
函 数 名： AgentGetTVWallTable
功    能： 取TvWall配置信息
算法实现： 
全局变量： 
参    数：  const s8* lpszProfileName
TEqpTVWallEntry ***mcueqpTVWallTable
u32 *dwmcueqpTVWallEntryNum
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::AgentGetTVWallTable( const s8* lpszProfileName )
{
    if( NULL == lpszProfileName )
	{
		printf("[AgentGetTVWallTable] Error input parameter (AgentGetTVWallTable)\n");
		return FALSE;
	}
	
    //guzh [2008/03/27]
#ifdef _MINIMCU_    
    if ( IsMcu8000BHD() )
    {
        printf("[McuAgent] No Tvwall Support in MCU8000B-HD\n");
        m_dwTVWallEntryNum = 0;
        return TRUE;
    }
#endif
	
	s8 **ppszTable = NULL;
    s8   achSeps[] = " ,\t";       /* space or tab as seperators */
    s8 * pchToken = NULL;
	u32	 dwBrdLoop;
    u32  dwLoop;
    s32  nMemEntryNum;
	
    // get the number of entry
    BOOL32 bResult = GetRegKeyInt( lpszProfileName, SECTION_mcueqpTVWallTable, 
		STR_ENTRY_NUM, 0, &nMemEntryNum );
	if( !bResult) 
	{
		printf("[AgentGetTVWallTable] Wrong profile while reading %s %s!\n", 
			SECTION_mcueqpTVWallTable, STR_ENTRY_NUM );
        g_cAlarmProc.ProcReadError(ERR_AGENT_READTVCFG);
		return FALSE;
	}
	
    // no TvWall
    if(0 == nMemEntryNum)
    {
        m_dwTVWallEntryNum = 0;
        return TRUE;
    }
	
	// alloc memory
    ppszTable = new s8*[(u32)nMemEntryNum];
    if(NULL == ppszTable)
    {
        printf("[AgentGetTVWallTable] Fail to malloc memory for tv wall table\n");
        return FALSE;
    }
	
    for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
    {
        ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
        if(NULL == ppszTable[dwLoop])
        {
            printf("[AgentGetTVWallTable] Fail to malloc memory for tv wall table\n");
			FREE_TABLE_MEM( ppszTable, dwLoop );
            return FALSE;
        }
    }
	
	// get the tvwall table
    m_dwTVWallEntryNum = nMemEntryNum;
    bResult = AgtGetRegKeyStringTable( lpszProfileName, SECTION_mcueqpTVWallTable,
		"fail", ppszTable, &m_dwTVWallEntryNum, MAX_VALUE_LEN + 1 );
    if( !bResult)
    {
		printf("[AgentGetTVWallTable] Wrong profile while reading %s table!\n", 
			SECTION_mcueqpTVWallTable );
        FREE_TABLE_MEM( ppszTable, nMemEntryNum );
        g_cAlarmProc.ProcReadError( ERR_AGENT_READPRSCFG );
        m_dwTVWallEntryNum = 0;
        return FALSE;
    }
	
    for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
    {
        // TVWallEntId 
        pchToken = strtok( ppszTable[dwLoop], achSeps );
        if( NULL == pchToken)
        {
            printf("[AgentGetTVWallTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpTVWallEntId );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atTVWallTable[dwLoop].SetEqpId( atoi(pchToken) );
        }
		
		// mcueqpTVWallEntAlias 
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetTVWallTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpTVWallEntAlias );
            bResult = FALSE;
            continue;
        }
        else
        {
			if ( IsUtf8Encoding())
			{
				printf("[AgentGetRecorderTable]Rec Alias:%d is UTF8\n", dwLoop);
				m_atTVWallTable[dwLoop].SetAlias( pchToken );
			}
			else
			{
				printf("[AgentGetRecorderTable]Rec Alias:%d is GBK\n", dwLoop);
				char achAlias[MAXLEN_EQP_ALIAS+1];
				memset(achAlias, 0 , sizeof(achAlias));
				gb2312_to_utf8(pchToken, achAlias, MAXLEN_EQP_ALIAS);
				m_atTVWallTable[dwLoop].SetAlias( achAlias );
			}
        }
        
		// RunningBrdId 
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetTVWallTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpTVWallEntRunningBrdId );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atTVWallTable[dwLoop].SetRunBrdId( atoi(pchToken) );
			for( dwBrdLoop = 0; dwBrdLoop < m_dwBrdCfgEntryNum; dwBrdLoop++)
			{
				if( m_atBrdCfgTable[dwBrdLoop].GetBrdId() == m_atTVWallTable[dwLoop].GetRunBrdId() )
				{
					m_atTVWallTable[dwLoop].SetIpAddr( m_atBrdCfgTable[dwBrdLoop].GetBrdIp() );
					bResult = m_atBrdCfgTable[dwBrdLoop].SetPeriId( m_atTVWallTable[dwLoop].GetEqpId() );
					break;
				}
			}
			if( dwBrdLoop >= m_dwBrdCfgEntryNum )
			{
				printf("[AgentGetTVWallTable] Wrong Tv Wall Run Board Value %s = %s!\n", 
					FIELD_mcueqpTVWallEntRunningBrdId, pchToken);
				bResult = FALSE;
			}
        }
		
		// RecvStartPort
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken)
        {
            printf("[AgentGetTVWallTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpTVWallEntRecvStartPort );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atTVWallTable[dwLoop].SetEqpRecvPort( atoi(pchToken) );
        }
        
        if( VERSION_CONFIGURE36 == m_tMPCInfo.GetLocalConfigVer() )
        {
            pchToken = strtok( NULL, achSeps );
            if( pchToken == NULL )
            {
                printf("[AgentGetTVWallTable] Wrong profile while reading %s!\n", 
					FIELD_mcueqpTVWallEntDivStyle );
            }
            else
            {
                u8 byMcueqpTVWallEntDivStyle = atoi( pchToken );  // 分割方式
            }
			
            pchToken = strtok( NULL, achSeps );
            if( pchToken == NULL )
            {
                printf( "[AgentGetTVWallTable] Wrong profile while reading %s!\n", 
					FIELD_mcueqpTVWallEntDivNum );
            }
            else
            {
                u8 byMcueqpTVWallEntDivNum = atoi( pchToken );  // 分割画面数
            }
        }
		
		// MapId
		while( ( pchToken = strtok( NULL, achSeps ) ) != NULL )
		{
            m_atTVWallTable[dwLoop].SetUsedMapId( atoi(pchToken) );
		}
		
		if( 0 == m_atTVWallTable[dwLoop].GetUsedMapNum() )
		{
            printf("[AgentGetTVWallTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpTVWallEntMapId );
			bResult = FALSE;
		}
        // 初始化其他参数
        m_atTVWallTable[dwLoop].SetDefaultEntParam( EQP_TYPE_TVWALL, ntohl(GetLocalIp()) );
	}  

    if( !bResult)
    {
        g_cAlarmProc.ProcReadError(ERR_AGENT_READTVCFG);
    }
	// free memory
    FREE_TABLE_MEM( ppszTable, nMemEntryNum );
    
    //return TRUE;
	return bResult;
}

/*=============================================================================
函 数 名： AgentGetBasTable
功    能： 取Bas配置信息
算法实现： 
全局变量： 
参    数：  const s8* lpszProfileName
TEqpBasEntry ***mcueqpBasTable
u32 *dwmcueqpBasEntryNum
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::AgentGetBasTable( const s8* lpszProfileName )
{
    if( NULL == lpszProfileName )
	{
		printf("[AgentGetBasTable] error input parameter . (AgentGetBasTable)! \n");
		return FALSE;
	}
	
	//zbq[06/20/2008] BAS的是否启用基于会议判断
	/*
    //guzh [2008/03/27]
	#ifdef _MINIMCU_    
    if ( IsMcu8000BHD() )
    {
	printf("[McuAgent] No BAS Support in MCU8000B-HD\n");
	m_dwBasEntryNum = 0;
	return TRUE;
    }
	#endif
	*/
	
	s8*   *ppszTable = NULL;
    s8    achSeps[] = " ,\t";       /* space or tab as seperators */
    s8    *pchToken = NULL;
    u32   dwLoop;
	u32	  dwBrdLoop;
    s32   nMemEntryNum;
	
    // get the number of entry 
    BOOL32 bResult = GetRegKeyInt( lpszProfileName, SECTION_mcueqpBasTable, 
		STR_ENTRY_NUM, 0, &nMemEntryNum );
	if( !bResult ) 
	{
		printf("[AgentGetBasTable] Wrong profile while reading %s %s!\n", 
			SECTION_mcueqpBasTable, STR_ENTRY_NUM );
        g_cAlarmProc.ProcReadError(ERR_AGENT_READBASCFG);
		return FALSE;
	}
	
    if ( nMemEntryNum == 0 )
    {
        m_dwBasEntryNum = 0;
        return TRUE;
    }
	// alloc memory
    ppszTable = new s8*[(u32)nMemEntryNum];
    if( NULL == ppszTable )
    {
        printf("[AgentGetBasTable] Fail to malloc memory for Bas table\n");
        return FALSE;
    }
    for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
    {
        ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
        if(NULL == ppszTable[dwLoop])
        {
            printf("[AgentGetBasTable] Fail to malloc memory for Bas table\n");
            FREE_TABLE_MEM( ppszTable, dwLoop );
			return FALSE;
        }
    }
	
    m_dwBasEntryNum = nMemEntryNum;
    bResult = AgtGetRegKeyStringTable( lpszProfileName, SECTION_mcueqpBasTable,
		"fail", ppszTable, &m_dwBasEntryNum, MAX_VALUE_LEN + 1);
    if( !bResult )
    {
		printf("[AgentGetBasTable] Wrong profile while reading %s table!\n", 
			SECTION_mcueqpBasTable);
		
        FREE_TABLE_MEM( ppszTable, nMemEntryNum );
        m_dwBasEntryNum = 0;
        g_cAlarmProc.ProcReadError(ERR_AGENT_READBASCFG);
		return FALSE;
    }
	
    for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
    {
        // Id
        pchToken = strtok( ppszTable[dwLoop], achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetBasTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpBasEntId );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atBasTable[dwLoop].SetEqpId( atoi(pchToken) );
        }
		
        // BasEntPort
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetBasTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpBasEntPort );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atBasTable[dwLoop].SetMcuRecvPort( atoi(pchToken) );
        }
		
        // SwitchBrdId
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetBasTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpBasEntSwitchBrdId );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atBasTable[dwLoop].SetSwitchBrdId( atoi(pchToken) );
        }
		
		// BasEntAlias
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetBasTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpBasEntAlias );
            bResult = FALSE;
            continue;
        }
        else
        {
			if ( IsUtf8Encoding())
			{
				printf("[AgentGetBasTable]Bas Alias:%d is UTF8\n", dwLoop);
				m_atBasTable[dwLoop].SetAlias( pchToken );
			}
			else
			{
				printf("[AgentGetBasTable]Bas Alias:%d is GBK\n", dwLoop);
				char achAlias[MAXLEN_EQP_ALIAS+1];
				memset(achAlias, 0 , sizeof(achAlias));
				gb2312_to_utf8(pchToken, achAlias, MAXLEN_EQP_ALIAS);
				m_atBasTable[dwLoop].SetAlias( achAlias );
			}
        }
		
		// RunningBrdId 
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetBasTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpBasEntRunningBrdId );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atBasTable[dwLoop].SetRunBrdId( atoi(pchToken) );
			for( dwBrdLoop = 0; dwBrdLoop < m_dwBrdCfgEntryNum; dwBrdLoop++)
			{
				if( m_atBrdCfgTable[dwBrdLoop].GetBrdId() == m_atBasTable[dwLoop].GetRunBrdId() )
				{
					m_atBasTable[dwLoop].SetIpAddr( m_atBrdCfgTable[dwBrdLoop].GetBrdIp() );
					bResult = m_atBrdCfgTable[dwBrdLoop].SetPeriId( m_atBasTable[dwLoop].GetEqpId() );
					break;
				}
			}
			if( dwBrdLoop >= m_dwBrdCfgEntryNum )
			{
				printf("[AgentGetBasTable] Wrong Value %s = %s!\n", 
					FIELD_mcueqpBasEntRunningBrdId, pchToken);
				bResult = FALSE;
			}
        }
		
		// RecvStartPort
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetBasTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpBasEntRecvStartPort );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atBasTable[dwLoop].SetEqpRecvPort( atoi(pchToken) );
        }
		
		// MapId
		while( ( pchToken = strtok( NULL, achSeps ) ) != NULL )
		{
            m_atBasTable[dwLoop].SetUsedMapId( atoi(pchToken) );
		}                
		
		// MaxAdpChannel
		m_atBasTable[dwLoop].SetAdpChnNum( 0 );  // 暂时不用该参数
        
        // 初始化其他参数
        m_atBasTable[dwLoop].SetDefaultEntParam( EQP_TYPE_BAS, ntohl(GetLocalIp()) );
	}

    if( !bResult )
    {
        g_cAlarmProc.ProcReadError(ERR_AGENT_READBASCFG);
    }
    
    FREE_TABLE_MEM( ppszTable, nMemEntryNum );
    
    //return TRUE;
	return bResult;
}

/*=============================================================================
函 数 名： AgentGetBasHDTable
功    能： 取BasHD配置信息
算法实现： 
全局变量： 
参    数：  const s8* lpszProfileName
TEqpBasHDEntry ***mcueqpBasHDTable
u32 *dwmcueqpBasHDEntryNum
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::AgentGetBasHDTable( const s8* lpszProfileName )
{
    if( NULL == lpszProfileName )
	{
		printf("[AgentGetBasHDTable] error input parameter . (AgentGetBasHDTable)! \n");
		return FALSE;
	}
	
	s8*   *ppszTable = NULL;
    s8    achSeps[] = " ,\t";       /* space or tab as seperators */
    s8    *pchToken = NULL;
    u32   dwLoop;
    s32   nMemEntryNum;
	
    // get the number of entry 
    BOOL32 bResult = GetRegKeyInt( lpszProfileName, SECTION_mcueqpBasHDTable, 
		STR_ENTRY_NUM, 0, &nMemEntryNum );
	if( !bResult ) 
	{
		printf("[AgentGetBasHDTable] Wrong profile while reading %s %s!\n", 
			SECTION_mcueqpBasHDTable, STR_ENTRY_NUM );
        g_cAlarmProc.ProcReadError(ERR_AGENT_READBASHDCFG);
		return FALSE;
	}
	
    if ( nMemEntryNum == 0 )
    {
        m_dwBasHDEntryNum = 0;
        return TRUE;
    }
	// alloc memory
    ppszTable = new s8*[(u32)nMemEntryNum];
    if( NULL == ppszTable )
    {
        printf("[AgentGetBasHDTable] Fail to malloc memory for BasHD table\n");
        return FALSE;
    }
    for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
    {
        ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
        if(NULL == ppszTable[dwLoop])
        {
            printf("[AgentGetBasHDTable] Fail to malloc memory for BasHD table\n");
            FREE_TABLE_MEM( ppszTable, dwLoop );
			return FALSE;
        }
    }
	
    m_dwBasHDEntryNum = nMemEntryNum;
    bResult = AgtGetRegKeyStringTable( lpszProfileName, SECTION_mcueqpBasHDTable,
		"fail", ppszTable, &m_dwBasHDEntryNum, MAX_VALUE_LEN + 1);
    if( !bResult )
    {
		printf("[AgentGetBasHDTable] Wrong profile while reading %s table!\n", 
			SECTION_mcueqpBasHDTable);
		
        FREE_TABLE_MEM( ppszTable, nMemEntryNum );
        m_dwBasHDEntryNum = 0;
        g_cAlarmProc.ProcReadError(ERR_AGENT_READBASHDCFG);
		return FALSE;
    }
	
    for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
    {
        // Id
        pchToken = strtok( ppszTable[dwLoop], achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetBasHDTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpBasEntId );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atBasHDTable[dwLoop].SetEqpId( atoi(pchToken) );
        }
		
        // BasEntPort
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetBasHDTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpBasEntPort );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atBasHDTable[dwLoop].SetMcuRecvPort( atoi(pchToken) );
        }
		
        // SwitchBrdId
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetBasHDTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpBasEntSwitchBrdId );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atBasHDTable[dwLoop].SetSwitchBrdId( atoi(pchToken) );
        }
		
		// BasEntAlias
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetBasHDTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpBasEntAlias );
            bResult = FALSE;
            continue;
        }
        else
        {
			if ( IsUtf8Encoding())
			{
				printf("[AgentGetBasHDTable]HdBas Alias:%d is UTF8\n", dwLoop);
				m_atBasHDTable[dwLoop].SetAlias( pchToken );
			}
			else
			{
				printf("[AgentGetBasHDTable]HdBas Alias:%d is GBK\n", dwLoop);
				char achAlias[MAXLEN_EQP_ALIAS+1];
				memset(achAlias, 0 , sizeof(achAlias));
				gb2312_to_utf8(pchToken, achAlias, MAXLEN_EQP_ALIAS);
				m_atBasHDTable[dwLoop].SetAlias( achAlias );
			}
        }
		
        // IpAddr
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetBasHDTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpBasEntIpAddr );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atBasHDTable[dwLoop].SetIpAddr( ntohl(INET_ADDR(pchToken)) );
        }
		
		// RecvStartPort
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetBasHDTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpBasEntRecvStartPort );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atBasHDTable[dwLoop].SetEqpRecvPort( atoi(pchToken) );
        }     
        
        //Type
        pchToken = strtok( NULL, achSeps );
        if ( NULL == pchToken )
        {
            printf("[AgentGetBasHDTable] Wrong profile while reading %s!\n",
				"mcueqpBasEntType");
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atBasHDTable[dwLoop].SetHDBasType( atoi(pchToken) );
        }
		
		// MaxAdpChannel
		m_atBasHDTable[dwLoop].SetAdpChnNum( 0 );  // 暂时不用该参数
        
        // 初始化其他参数
        m_atBasHDTable[dwLoop].SetDefaultEntParam( EQP_TYPE_BAS, ntohl(GetLocalIp()) );
	} 

    if( !bResult )
    {
        g_cAlarmProc.ProcReadError(ERR_AGENT_READBASCFG);
    }
    
    FREE_TABLE_MEM( ppszTable, nMemEntryNum );
    
    //return TRUE;
	return bResult;
}

/*=============================================================================
函 数 名： AgentGetVMPTable
功    能： 取Vmp配置信息
算法实现： 
全局变量： 
参    数：  const s8* lpszProfileName
TEqpVMPEntry ***mcueqpVMPTable
u32 *dwmcueqpVMPEntryNum
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::AgentGetVMPTable( const s8* lpszProfileName )
{
    if( NULL == lpszProfileName )
	{
		printf("[AgentGetVMPTable] Error input parameter!\n");
		return FALSE;
	}
	
	//zbq[06/20/2008] VMP的是否启用基于会议判断
	/*
	#ifdef _MINIMCU_    
    if ( IsMcu8000BHD() )
    {
	printf("[McuAgent] No VMP Support in MCU8000B-HD\n");
	m_dwVMPEntryNum = 0;
	return TRUE;
    }
	#endif
	*/
	
	s8*   *ppszTable = NULL;
    s8    achSeps[] = " ,\t";       /* space or tab as seperators */
    s8    *pchToken = NULL;
	u32	  dwBrdLoop;
    u32   dwLoop;
    s32   nMemEntryNum;
	
    //  get the number of entry 
    BOOL32 bResult = GetRegKeyInt( lpszProfileName, SECTION_mcueqpVMPTable, 
		STR_ENTRY_NUM, 0, &nMemEntryNum );
	if( !bResult) 
	{
		printf("[AgentGetVMPTable] Wrong profile while reading %s %s!\n", 
			SECTION_mcueqpVMPTable, STR_ENTRY_NUM );
        g_cAlarmProc.ProcReadError(ERR_AGENT_READVMPCFG);
		return FALSE;
	}
	
    // 没有配置Vmp
    if(0 == nMemEntryNum)
    {
        m_dwVMPEntryNum = 0;
        return TRUE;
    }
	
	// alloc memory
    ppszTable = new s8*[(u32)nMemEntryNum];
    if( NULL == ppszTable )
    {
		printf("[AgentGetVMPTable] Fail to malloc memory for Vmp table\n");
		return FALSE;
    }
    for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
    {
        ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
        if(NULL == ppszTable[dwLoop] )
        {
            printf("[AgentGetVMPTable] Fail to malloc memory for Vmp table\n");
            FREE_TABLE_MEM( ppszTable, dwLoop);
			return FALSE;
        }
    }
	
	// get the vmp table
    m_dwVMPEntryNum = nMemEntryNum;
    bResult = AgtGetRegKeyStringTable( lpszProfileName, SECTION_mcueqpVMPTable,
		"fail", ppszTable, &m_dwVMPEntryNum, MAX_VALUE_LEN + 1 );
    if( !bResult )
    {
		printf("[AgentGetVMPTable] Wrong profile while reading %s table!\n", 
			SECTION_mcueqpVMPTable );
        FREE_TABLE_MEM( ppszTable, nMemEntryNum);
        m_dwVMPEntryNum = 0;
        g_cAlarmProc.ProcReadError(ERR_AGENT_READVMPCFG);
		return FALSE;
    }
	
    for( dwLoop = 0; dwLoop < m_dwVMPEntryNum; dwLoop++ )
    {
        // VMPEntId
        pchToken = strtok( ppszTable[dwLoop], achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetVMPTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpVMPEntId );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atVMPTable[dwLoop].SetEqpId( atoi(pchToken) );
        }
		
        // VMPEntPort
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetVMPTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpVMPEntPort );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atVMPTable[dwLoop].SetMcuRecvPort( atoi(pchToken) );
        }
		
        // SwitchBrdId 
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken)
        {
            printf("[AgentGetVMPTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpVMPEntSwitchBrdId );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atVMPTable[dwLoop].SetSwitchBrdId( atoi(pchToken) );
        }
		
		// Alias
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetVMPTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpVMPEntAlias );
            bResult = FALSE;
            continue;
        }
        else
        {
			if ( IsUtf8Encoding())
			{
				printf("[AgentGetVMPTable]Vmp Alias:%d is UTF8\n", dwLoop);
				m_atVMPTable[dwLoop].SetAlias( pchToken );
			}
			else
			{
				printf("[AgentGetVMPTable]Vmp Alias:%d is GBK\n", dwLoop);
				char achAlias[MAXLEN_EQP_ALIAS+1];
				memset(achAlias, 0 , sizeof(achAlias));
				gb2312_to_utf8(pchToken, achAlias, MAXLEN_EQP_ALIAS);
				m_atVMPTable[dwLoop].SetAlias( achAlias );
			}
        }
		
		// RunningBrdId
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetVMPTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpVMPEntRunningBrdId );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atVMPTable[dwLoop].SetRunBrdId( atoi(pchToken) );
			for( dwBrdLoop = 0; dwBrdLoop < m_dwBrdCfgEntryNum; dwBrdLoop++)
			{
				if( m_atBrdCfgTable[dwBrdLoop].GetBrdId() == m_atVMPTable[dwLoop].GetRunBrdId() )
				{
					m_atVMPTable[dwLoop].SetIpAddr( m_atBrdCfgTable[dwBrdLoop].GetBrdIp() );
					bResult = m_atBrdCfgTable[dwBrdLoop].SetPeriId( m_atVMPTable[dwLoop].GetEqpId() );
					break;
				}
			}
			if( dwBrdLoop >= m_dwBrdCfgEntryNum )
			{
				printf("[AgentGetVMPTable] Wrong Value %s = %s!\n", 
					FIELD_mcueqpVMPEntRunningBrdId, pchToken);
				bResult = FALSE;
			}
			
        }
		
		// RecvStartPort
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken)
        {
            printf("[AgentGetVMPTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpVMPEntRecvStartPort );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atVMPTable[dwLoop].SetEqpRecvPort( atoi(pchToken) );
        }
		
		// EncNumber
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken)
        {
            printf("[AgentGetVMPTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpVMPEntEncNumber );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atVMPTable[dwLoop].SetEncodeNum( atoi(pchToken) );
        }
		
		// MapId
		while( ( pchToken = strtok( NULL, achSeps ) ) != NULL )
		{
            m_atVMPTable[dwLoop].SetUsedMapId( atoi(pchToken) );
		}
		if( m_atVMPTable[dwLoop].GetUsedMapNum() == 0 )
		{
            printf("[AgentGetVMPTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpVMPEntMapId );
			bResult = FALSE;
		}
		
        // 初始化其他参数
        m_atVMPTable[dwLoop].SetDefaultEntParam( EQP_TYPE_VMP, ntohl(GetLocalIp()) );
	}   

    if( !bResult )
    {
        g_cAlarmProc.ProcReadError(ERR_AGENT_READVMPCFG);
    }
	// free memory 
    FREE_TABLE_MEM( ppszTable, nMemEntryNum );
    
    //return TRUE;
	return bResult;
}

/*=============================================================================
函 数 名： AgentGetMpwTable
功    能： 读复合电视墙
算法实现： 
全局变量： 
参    数： const s8* lpszProfileName
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::AgentGetMpwTable(const s8* lpszProfileName)
{
    if(NULL == lpszProfileName)
    {
        printf("[AgentGetMpwTable] The pointer cannot be Null.\n");
        return FALSE;
    }
	
	//guzh [2008/03/27]
#ifdef _MINIMCU_    
    if ( IsMcu8000BHD() )
    {
        printf("[McuAgent] No MPW Support in MCU8000B-HD\n");
        m_dwMpwEntryNum = 0;
        return TRUE;
    }
#endif
	
    s8*   *ppszTable = NULL;
    s8    achSeps[] = " ,\t";       /* space or tab as seperators */
    s8    *pchToken = NULL;
	u32	  dwBrdLoop;
    u32   dwLoop;
    BOOL32  bResult = TRUE;
    s32   nMemEntryNum;
	
    //  get the number of entry 
    bResult = GetRegKeyInt( lpszProfileName, SECTION_mcueqpMpwTable, 
		STR_ENTRY_NUM, 0, &nMemEntryNum );
	if( !bResult ) 
	{
		printf("[AgentGetMpwTable] Wrong profile while reading %s %s!\n", 
			SECTION_mcueqpMpwTable, STR_ENTRY_NUM );
        g_cAlarmProc.ProcReadError(ERR_AGENT_READMPWCFG);
		return FALSE;
	}
	
    // 没有配置Mpw
    if(0 == nMemEntryNum)
    {
        m_dwMpwEntryNum = 0;
        return TRUE;
    }
	
	// alloc memory
    ppszTable = new s8*[(u32)nMemEntryNum];
    if(NULL == ppszTable )
    {
		printf("[AgentGetMpwTable] Fail to malloc memory for Mpw table\n");
		return FALSE;
    }
	
    for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
    {
        ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
        if(NULL == ppszTable[dwLoop] )
        {
            printf("[AgentGetMpwTable] Fail to malloc memory for Mpw table1\n");
            FREE_TABLE_MEM( ppszTable, dwLoop);
			return FALSE;
        }
    }
	
	// get the Mpw table
    m_dwMpwEntryNum = (u32)nMemEntryNum;
	
    bResult = AgtGetRegKeyStringTable( lpszProfileName, SECTION_mcueqpMpwTable,
		"fail", ppszTable, (u32*)&nMemEntryNum, MAX_VALUE_LEN + 1 );
    if( !bResult )
    {
		printf("[AgentGetMpwTable] Wrong profile while reading %s table!\n", 
			SECTION_mcueqpMpwTable);
        FREE_TABLE_MEM( ppszTable, nMemEntryNum);
        m_dwMpwEntryNum = 0;
        g_cAlarmProc.ProcReadError(ERR_AGENT_READMPWCFG);
		return FALSE;
    }
	
    for( dwLoop = 0; dwLoop < m_dwMpwEntryNum; dwLoop++ )
    {
        // Mpw Id
        pchToken = strtok( ppszTable[dwLoop], achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetMpwTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpMpwEntId );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atMpwTable[dwLoop].SetEqpId( atoi(pchToken) );
        }
		
		// Alias
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetMpwTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpMpwEntAlias );
            bResult = FALSE;
            continue; 
        }
        else
        {
			if ( IsUtf8Encoding())
			{
				printf("[AgentGetMpwTable]Mpw Alias:%d is UTF8\n", dwLoop);
				m_atMpwTable[dwLoop].SetAlias( pchToken );
			}
			else
			{
				printf("[AgentGetMpwTable]Mpw Alias:%d is GBK\n", dwLoop);
				char achAlias[MAXLEN_EQP_ALIAS+1];
				memset(achAlias, 0 , sizeof(achAlias));
				gb2312_to_utf8(pchToken, achAlias, MAXLEN_EQP_ALIAS);
				m_atMpwTable[dwLoop].SetAlias( achAlias );
			}
        }
		
		// RunningBrdId
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetMpwTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpMpwEntRunningBrdId );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atMpwTable[dwLoop].SetRunBrdId( atoi(pchToken) );
			for( dwBrdLoop = 0; dwBrdLoop < m_dwBrdCfgEntryNum; dwBrdLoop++)
			{
				if( m_atBrdCfgTable[dwBrdLoop].GetBrdId() == m_atMpwTable[dwLoop].GetRunBrdId() )
				{
					m_atMpwTable[dwLoop].SetIpAddr( m_atBrdCfgTable[dwBrdLoop].GetBrdIp() );
					bResult = m_atBrdCfgTable[dwBrdLoop].SetPeriId( m_atMpwTable[dwLoop].GetEqpId() );
                    break;
				}
			}
			if( dwBrdLoop >= m_dwBrdCfgEntryNum )
			{
				printf("[AgentGetMpwTable] Wrong Value %s = %s!\n", 
					FIELD_mcueqpMpwEntRunningBrdId, pchToken);
				bResult = FALSE;
			}
			
        }
		
		// RecvStartPort
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetMpwTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpMpwEntRecvStartPort );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atMpwTable[dwLoop].SetEqpRecvPort( atoi(pchToken) );
        }
		
		// MapId
		while( ( pchToken = strtok( NULL, achSeps ) ) != NULL )
		{
            m_atMpwTable[dwLoop].SetUsedMapId( atoi(pchToken) );
		}
		
		if( m_atMpwTable[dwLoop].GetUsedMapNum() == 0 )
		{
            printf("[AgentGetMpwTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpMpwEntMapId );
			bResult = FALSE;
		}
        // 初始化其他参数
        m_atMpwTable[dwLoop].SetDefaultEntParam( EQP_TYPE_VMPTW, ntohl(GetLocalIp()) );
	}   
    if( !bResult )
    {
        g_cAlarmProc.ProcReadError(ERR_AGENT_READMPWCFG);
    }
	// free memory 
    FREE_TABLE_MEM( ppszTable, nMemEntryNum );
    
    //return TRUE;
	return bResult;
}
//4.6版本新加 jlb
/*=============================================================================
函 数 名： AgentGetHduTable
功    能： 读Hdu信息
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
BOOL32 CCfgAgent::AgentGetHduTable( const s8* lpszProfileName )     
{
    if( NULL == lpszProfileName )
	{
		printf("[AgentGetHduTable] Error input parameter (AgentGetHduTable)\n");
		return FALSE;
	}
	
	s8 **ppszTable = NULL;
    s8   achSeps[] = " ,\t";       /* space or tab as seperators */
    s8 * pchToken = NULL;
	u32	 dwBrdLoop;
    u32  dwLoop;
    s32  nMemEntryNum;
	
    // get the number of entry
    BOOL32 bResult = GetRegKeyInt( lpszProfileName, SECTION_mcueqpHduTable, 
		STR_ENTRY_NUM, 0, &nMemEntryNum );
	if( !bResult) 
	{
		printf("[AgentGetHduTable] Wrong profile while reading %s %s!\n", 
			SECTION_mcueqpHduTable, STR_ENTRY_NUM );
        g_cAlarmProc.ProcReadError(ERR_AGENT_READHDUCFG);
		return FALSE;
	}
	
    // no Hdu
    if(0 == nMemEntryNum)
    {
        m_dwHduEntryNum = 0;
        return TRUE;
    }
	
	// alloc memory
    ppszTable = new s8*[(u32)nMemEntryNum];
    if(NULL == ppszTable)
    {
        printf("[AgentGetHduTable] Fail to malloc memory for Hdu table\n");
        return FALSE;
    }
	
    for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
    {
        ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
        if(NULL == ppszTable[dwLoop])
        {
            printf("[AgentGetHduTable] Fail to malloc memory for hdu table\n");
			FREE_TABLE_MEM( ppszTable, dwLoop );
            return FALSE;
        }
    }
	
	// get the hdu table
    m_dwHduEntryNum = nMemEntryNum;   
    bResult = AgtGetRegKeyStringTable( lpszProfileName, SECTION_mcueqpHduTable,
		"fail", ppszTable, &m_dwHduEntryNum, MAX_VALUE_LEN + 1 );
    if( !bResult)
    {
		printf("[AgentGetHduTable] Wrong profile while reading %s table!\n", 
			SECTION_mcueqpHduTable );
        FREE_TABLE_MEM( ppszTable, nMemEntryNum );
        g_cAlarmProc.ProcReadError( ERR_AGENT_READHDUCFG );
        m_dwHduEntryNum = 0;
        return FALSE;
    }
	
    for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
    {
        // HduEntId 
        pchToken = strtok( ppszTable[dwLoop], achSeps );
        if( NULL == pchToken)
        {
            printf("[AgentGetHduTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpHduEntId );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atHduTable[dwLoop].SetEqpId( atoi(pchToken) );
        }
		
		// mcueqpHduEntAlias 
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetHduTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpHduEntAlias );
            bResult = FALSE;
            continue;
        }
        else
        {
			if ( IsUtf8Encoding())
			{
				printf("[AgentGetHduTable]Hdu Alias:%d is UTF8\n", dwLoop);
				m_atHduTable[dwLoop].SetAlias( pchToken );
			}
			else
			{
				printf("[AgentGetHduTable]Hdu Alias:%d is GBK\n", dwLoop);
				char achAlias[MAXLEN_EQP_ALIAS+1];
				memset(achAlias, 0 , sizeof(achAlias));
				gb2312_to_utf8(pchToken, achAlias, MAXLEN_EQP_ALIAS);
				m_atHduTable[dwLoop].SetAlias( achAlias );
			}
        }
        
		// RunningBrdId 
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetHduTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpHduEntRunningBrdId );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atHduTable[dwLoop].SetRunBrdId( atoi(pchToken) );
			for( dwBrdLoop = 0; dwBrdLoop < m_dwBrdCfgEntryNum; dwBrdLoop++)
			{
				if( m_atBrdCfgTable[dwBrdLoop].GetBrdId() == m_atHduTable[dwLoop].GetRunBrdId() )
				{
					m_atHduTable[dwLoop].SetIpAddr( m_atBrdCfgTable[dwBrdLoop].GetBrdIp() );
					bResult = m_atBrdCfgTable[dwBrdLoop].SetPeriId( m_atHduTable[dwLoop].GetEqpId() );
					break;
				}
			}
			if( dwBrdLoop >= m_dwBrdCfgEntryNum )
			{
				printf("[AgentGetHduTable] Wrong hdu Run Board Value %s = %s!\n", 
					FIELD_mcueqpHduEntRunningBrdId, pchToken);
				bResult = FALSE;
			}
        }
		
		// RecvStartPort
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken)
        {
            printf("[AgentGetHduTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpHduEntRecvStartPort );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atHduTable[dwLoop].SetEqpRecvPort( atoi(pchToken) );
        }
        
		THduChnlModePortAgt tHduChnModePort;
		memset(&tHduChnModePort, 0x0, sizeof(tHduChnModePort));
		
		//ZoomRate NO.0 chnl
		pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken)
        {
            printf("[AgentGetHduTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpHduEntZoomRate1 );
            bResult = FALSE;
            continue;
        }
        else
        {
			tHduChnModePort.SetZoomRate( atoi(pchToken) );
        }
		
		// ModeType NO.0 Chnl
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken)
        {
            printf("[AgentGetHduTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpHduEntOutModeType1 );
            bResult = FALSE;
            continue;
        }
        else
        {
			tHduChnModePort.SetOutModeType( atoi(pchToken) );
        }
		
		//PortType  NO.0 Chnl
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken)
        {
            printf("[AgentGetHduTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpHduEntOutPortType1 );
            bResult = FALSE;
            continue;
        }
        else
        {
            tHduChnModePort.SetOutPortType( atoi( pchToken ) );
        }
		m_atHduTable[dwLoop].SetHduChnlModePort(0, tHduChnModePort);
		
		//ZoomRate NO.1 chnl
		pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken)
        {
            printf("[AgentGetHduTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpHduEntZoomRate2 );
            bResult = FALSE;
            continue;
        }
        else
        {
			tHduChnModePort.SetZoomRate( atoi(pchToken) );
        }
		
		// ModeType NO.1 Chnl
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken)
        {
            printf("[AgentGetHduTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpHduEntOutModeType2 );
            bResult = FALSE;
            continue;
        }
        else
        {
			tHduChnModePort.SetOutModeType( atoi(pchToken) );
        }
		
		//PortType  NO.1 Chnl
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken)
        {
            printf("[AgentGetHduTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpHduEntOutPortType2 );
            bResult = FALSE;
            continue;
        }
        else
        {
            tHduChnModePort.SetOutPortType( atoi(pchToken) );
        }
        m_atHduTable[dwLoop].SetHduChnlModePort(1, tHduChnModePort);
		
        //StartMode  zjl20100426
        pchToken = strtok( NULL, achSeps );
		if (NULL == pchToken)
		{
			m_atHduTable[dwLoop].SetStartMode(STARTMODE_HDU_M);
			OspPrintf(TRUE, FALSE, "[AgentGetHduTable] Set default hdu StartMode.%d because mcucfg not find it!\n", STARTMODE_HDU_M);
		}
		else
		{
			m_atHduTable[dwLoop].SetStartMode(atoi(pchToken));
			OspPrintf(TRUE, FALSE, "[AgentGetHduTable] Read hdu StartMode.%d success!\n", atoi(pchToken));
		}

		if(STARTMODE_HDU_H == m_atHduTable[dwLoop].GetStartMode() || 
		   STARTMODE_HDU_M == m_atHduTable[dwLoop].GetStartMode() ||
		   STARTMODE_HDU_L == m_atHduTable[dwLoop].GetStartMode() ||
		   STARTMODE_HDU2 == m_atHduTable[dwLoop].GetStartMode()  ||
		   STARTMODE_HDU2_L == m_atHduTable[dwLoop].GetStartMode() ||
		   STARTMODE_HDU2_S == m_atHduTable[dwLoop].GetStartMode())
		{
			m_atHduTable[dwLoop].SetDefaultEntParam( EQP_TYPE_HDU, ntohl(GetLocalIp()) );
		}
		else
		{
			printf("[AgentGetHduTable]unexpected m_atHduTable %d StartMode %d!\n",dwLoop,  m_atHduTable[dwLoop].GetStartMode());
		}

// 		// 初始化其他参数
// 		if (STARTMODE_HDU_H == m_atHduTable[dwLoop].GetStartMode())
// 		{
// 			m_atHduTable[dwLoop].SetDefaultEntParam( EQP_TYPE_HDU_H, ntohl(GetLocalIp()) );
// 		}
// 		else if (STARTMODE_HDU_M == m_atHduTable[dwLoop].GetStartMode())
// 		{
// 			m_atHduTable[dwLoop].SetDefaultEntParam( EQP_TYPE_HDU, ntohl(GetLocalIp()) );
// 		}
// 		else if(STARTMODE_HDU_L == m_atHduTable[dwLoop].GetStartMode())
// 		{
// 			m_atHduTable[dwLoop].SetDefaultEntParam( EQP_TYPE_HDU_L, ntohl(GetLocalIp()) );
// 		}
// 		else if(STARTMODE_HDU2 == m_atHduTable[dwLoop].GetStartMode())
// 		{
// 			m_atHduTable[dwLoop].SetDefaultEntParam( EQP_TYPE_HDU2, ntohl(GetLocalIp()) );
// 		}
// 		else if(STARTMODE_HDU2_L == m_atHduTable[dwLoop].GetStartMode())
// 		{
// 			m_atHduTable[dwLoop].SetDefaultEntParam( EQP_TYPE_HDU2_L, ntohl(GetLocalIp()) );
// 		} 		
// 		else
// 		{
// 			printf("[AgentGetHduTable]unexpected m_atHduTable %d StartMode %d!\n",dwLoop,  m_atHduTable[dwLoop].GetStartMode());
// 		}
		
		// ScaleMode只针对HDU2板。非HDU2板无该配置项。 [11/30/2011 chendaiwei]
		TBoardInfo tBrdInfo;
		GetBrdCfgById(m_atHduTable[dwLoop].GetRunBrdId(),&tBrdInfo);
		if(tBrdInfo.GetType() != BRD_TYPE_HDU2 && tBrdInfo.GetType() != BRD_TYPE_HDU2_L && tBrdInfo.GetType() != BRD_TYPE_HDU2_S)
		{
			continue;
		}

		//scaling mode  NO.0 Chnl
		pchToken = strtok( NULL, achSeps );
		if( NULL == pchToken)
		{
			printf("[AgentGetHduTable] Wrong profile while reading %s!\n", 
				"mcueqpHduEntScaleMode1"/*FIELD_mcueqpHduEntScaleMode1*/ ); //TODO 该宏的定义文件需要由网管提交 [11/30/2011 chendaiwei]
			bResult = FALSE;
			continue;
		}
		else
		{
			m_atHduTable[dwLoop].GetHduChnlModePort(0,tHduChnModePort);
			tHduChnModePort.SetScalingMode( atoi(pchToken) );
		}
		m_atHduTable[dwLoop].SetHduChnlModePort(0, tHduChnModePort);		//ScalingMode NO.0 Chnl
		
		//scaling mode  NO.1 Chnl
		pchToken = strtok( NULL, achSeps );
		if( NULL == pchToken)
		{
			printf("[AgentGetHduTable] Wrong profile while reading %s!\n", 
				"mcueqpHduEntScaleMode2"/*FIELD_mcueqpHduEntScaleMode2*/ ); //TODO 该宏的定义文件需要由网管提交 [11/30/2011 chendaiwei]
			bResult = FALSE;
			continue;
		}
		else
		{
			m_atHduTable[dwLoop].GetHduChnlModePort(1,tHduChnModePort);
			tHduChnModePort.SetScalingMode( atoi(pchToken) );
		}
        m_atHduTable[dwLoop].SetHduChnlModePort(1, tHduChnModePort);       //ScalingMode NO.1 Chnl
	}  
	
    if( !bResult)
    {
        g_cAlarmProc.ProcReadError(ERR_AGENT_READHDUCFG);
    }
	// free memory
    FREE_TABLE_MEM( ppszTable, nMemEntryNum );
    
    //return TRUE;
	return bResult;
}

/*=============================================================================
函 数 名： AgentGetSvmpTable
功    能： 读Svmp信息
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
BOOL32 CCfgAgent::AgentGetSvmpTable( const s8* lpszProfileName ) 
{
    if( NULL == lpszProfileName )
	{
		printf("[AgentGetSvmpTable] Error input parameter!\n");
		return FALSE;
	}
	
	s8*   *ppszTable = NULL;
    s8    achSeps[] = " ,\t";       /* space or tab as seperators */
    s8    *pchToken = NULL;
	u32	  dwBrdLoop;
    u32   dwLoop;
    s32   nMemEntryNum;
	
    //  get the number of entry 
    BOOL32 bResult = GetRegKeyInt( lpszProfileName, SECTION_mcueqpSvmpTable, 
		STR_ENTRY_NUM, 0, &nMemEntryNum );
	if( !bResult) 
	{
		printf("[AgentGetSvmpTable] Wrong profile while reading %s %s!\n", 
			SECTION_mcueqpSvmpTable, STR_ENTRY_NUM );
        g_cAlarmProc.ProcReadError(ERR_AGENT_READVMPCFG);
		return FALSE;
	}
	
    // 没有配置Vmp
    if(0 == nMemEntryNum)
    {
        m_dwSvmpEntryNum = 0;
        return TRUE;
    }
	
	// alloc memory
    ppszTable = new s8*[(u32)nMemEntryNum];
    if( NULL == ppszTable )
    {
		printf("[AgentGetSvmpTable] Fail to malloc memory for Svmp table\n");
		return FALSE;
    }
    for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
    {
        ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
        if(NULL == ppszTable[dwLoop] )
        {
            printf("[AgentGetSvmpTable] Fail to malloc memory for Svmp table\n");
            FREE_TABLE_MEM( ppszTable, dwLoop);
			return FALSE;
        }
    }
	
	// get the vmp table
    m_dwSvmpEntryNum = nMemEntryNum;
    bResult = AgtGetRegKeyStringTable( lpszProfileName, SECTION_mcueqpSvmpTable,
		"fail", ppszTable, &m_dwSvmpEntryNum, MAX_VALUE_LEN + 1 );
    if( !bResult )
    {
		printf("[AgentGetSvmpTable] Wrong profile while reading %s table!\n", 
			SECTION_mcueqpSvmpTable );
        FREE_TABLE_MEM( ppszTable, nMemEntryNum);
        m_dwSvmpEntryNum = 0;
        g_cAlarmProc.ProcReadError(ERR_AGENT_READVMPCFG);
		return FALSE;
    }
	
    for( dwLoop = 0; dwLoop < m_dwSvmpEntryNum; dwLoop++ )
    {
        // VMPEntId
        pchToken = strtok( ppszTable[dwLoop], achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetSvmpTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpSvmpEntId );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atSvmpTable[dwLoop].SetEqpId( atoi(pchToken) );
        }
		
        // VMPEntPort
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetSvmpTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpSvmpEntPort );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atSvmpTable[dwLoop].SetMcuRecvPort( atoi(pchToken) );
        }
		
        // SwitchBrdId 
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken)
        {
            printf("[AgentGetSvmpTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpSvmpEntSwitchBrdId );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atSvmpTable[dwLoop].SetSwitchBrdId( atoi(pchToken) );
        }
		
		// Alias
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetSvmpTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpSvmpEntAlias );
            bResult = FALSE;
            continue;
        }
        else
        {
			if ( IsUtf8Encoding())
			{
				printf("[AgentGetSvmpTable]Svmp Alias:%d is UTF8\n", dwLoop);
				m_atSvmpTable[dwLoop].SetAlias( pchToken );
			}
			else
			{
				printf("[AgentGetSvmpTable]Svmp Alias:%d is GBK\n", dwLoop);
				char achAlias[MAXLEN_EQP_ALIAS+1];
				memset(achAlias, 0 , sizeof(achAlias));
				gb2312_to_utf8(pchToken, achAlias, MAXLEN_EQP_ALIAS);
				m_atSvmpTable[dwLoop].SetAlias( achAlias );
			}
        }
		
		// RunningBrdId
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetSvmpTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpSvmpEntRunningBrdId );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atSvmpTable[dwLoop].SetRunBrdId( atoi(pchToken) );
			for( dwBrdLoop = 0; dwBrdLoop < m_dwBrdCfgEntryNum; dwBrdLoop++)
			{
				if( m_atBrdCfgTable[dwBrdLoop].GetBrdId() == m_atSvmpTable[dwLoop].GetRunBrdId() )
				{
					m_atSvmpTable[dwLoop].SetIpAddr( m_atBrdCfgTable[dwBrdLoop].GetBrdIp() );
					bResult = m_atBrdCfgTable[dwBrdLoop].SetPeriId( m_atSvmpTable[dwLoop].GetEqpId() );
					break;
				}
			}
			if( dwBrdLoop >= m_dwBrdCfgEntryNum )
			{
				printf("[AgentGetSvmpTable] Wrong Value %s = %s!\n", 
					FIELD_mcueqpSvmpEntRunningBrdId, pchToken);
				bResult = FALSE;
			}
			
        }
		
		// RecvStartPort
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken)
        {
            printf("[AgentGetSvmpTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpSvmpEntRecvStartPort );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atSvmpTable[dwLoop].SetEqpRecvPort( atoi(pchToken) );
        }
		
		// MapId
		// 		while( ( pchToken = strtok( NULL, achSeps ) ) != NULL )
		// 		{
		//             m_atVMPTable[dwLoop].SetUsedMapId( atoi(pchToken) );
		// 		}
		// 		if( m_atVMPTable[dwLoop].GetUsedMapNum() == 0 )
		// 		{
		//             printf("[AgentGetSvmpTable] Wrong profile while reading %s!\n", 
		//                                                 FIELD_mcueqpSvmpEntMapId );
		// 			bResult = FALSE;
		// 		}
		
        // 初始化其他参数
        m_atSvmpTable[dwLoop].SetDefaultEntParam( EQP_TYPE_VMP, ntohl(GetLocalIp()) );

		// SubVmpType字段由V4R7及以后版本支持，老版本MPU配置文件中可能无此配置项，跳过[11/30/2011 chendaiwei]
		TBoardInfo tBrdInfo;
		GetBrdCfgById(m_atSvmpTable[dwLoop].GetRunBrdId(),&tBrdInfo);
		if(tBrdInfo.GetType() == BRD_TYPE_MPU)
		{
			m_atSvmpTable[dwLoop].SetVmpType( TYPE_MPUSVMP );
			continue;
		}

		// subVmptype
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken)
        {
            printf("[AgentGetSvmpTable] Wrong profile while reading %s!\n", 
				"mcueqpSvmpSubVmpType" );//TBD FIELD_mcueqpSvmpSubVmpType  该宏的定义文件需要由网管提交 [11/30/2011 chendaiwei]
			
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atSvmpTable[dwLoop].SetVmpType( atoi(pchToken) );
        }
	}   

    if( !bResult )
    {
        g_cAlarmProc.ProcReadError(ERR_AGENT_READVMPCFG);
    }
	// free memory 
    FREE_TABLE_MEM( ppszTable, nMemEntryNum );
    
    //return TRUE;
	return bResult;
}

/*=============================================================================
函 数 名： AgentGetDvmpTable
功    能： 读Dvmp信息
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
/*
BOOL32 CCfgAgent::AgentGetDvmpTable( const s8* lpszProfileName ) 
{
if( NULL == lpszProfileName )
{
printf("[AgentGetDvmpTable] Error input parameter!\n");
return FALSE;
}

  s8*   *ppszTable = NULL;
  s8    achSeps[] = " ,\t";        //space or tab as seperators 
  s8    *pchToken = NULL;
  u32	  dwBrdLoop;
  u32   dwLoop;
  s32   nMemEntryNum;
  
    //  get the number of entry 
    BOOL32 bResult = GetRegKeyInt( lpszProfileName, SECTION_mcueqpDvmpTable, 
	STR_ENTRY_NUM, 0, &nMemEntryNum );
	if( !bResult) 
	{
	printf("[AgentGetDvmpTable] Wrong profile while reading %s %s!\n", 
	SECTION_mcueqpDvmpTable, STR_ENTRY_NUM );
	g_cAlarmProc.ProcReadError(ERR_AGENT_READVMPCFG);
	return FALSE;
	}
	
	  // 没有配置Vmp
	  if(0 == nMemEntryNum)
	  {
	  m_dwDvmpEntryNum = 0;
	  return TRUE;
	  }
	  
		// alloc memory
		ppszTable = new s8*[nMemEntryNum];
		if( NULL == ppszTable )
		{
		printf("[AgentGetDvmpTable] Fail to malloc memory for Dvmp table\n");
		return FALSE;
		}
		for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
		{
        ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
        if(NULL == ppszTable[dwLoop] )
        {
		printf("[AgentGetDvmpTable] Fail to malloc memory for Dvmp table\n");
		FREE_TABLE_MEM( ppszTable, dwLoop);
		return FALSE;
        }
		}
		
		  // get the vmp table
		  m_dwDvmpEntryNum = nMemEntryNum;
		  bResult = AgtGetRegKeyStringTable( lpszProfileName, SECTION_mcueqpDvmpTable,
		  "fail", ppszTable, &m_dwVMPEntryNum, MAX_VALUE_LEN + 1 );
		  if( !bResult )
		  {
		  printf("[AgentGetDvmpTable] Wrong profile while reading %s table!\n", 
		  SECTION_mcueqpDvmpTable );
		  FREE_TABLE_MEM( ppszTable, nMemEntryNum);
		  m_dwDvmpEntryNum = 0;
		  g_cAlarmProc.ProcReadError(ERR_AGENT_READVMPCFG);
		  return FALSE;
		  }
		  
			for( dwLoop = 0; dwLoop < m_dwDvmpEntryNum; dwLoop++ )
			{
			// VMPEntId
			pchToken = strtok( ppszTable[dwLoop], achSeps );
			if( NULL == pchToken )
			{
            printf("[AgentGetDvmpTable] Wrong profile while reading %s!\n", 
			FIELD_mcueqpDvmpEntId );
            bResult = FALSE;
            continue;
			}
			else
			{
            m_atDvmpBasicTable[dwLoop].SetEqpId( atoi(pchToken) );
			}
			
			  // VMPEntPort
			  pchToken = strtok( NULL, achSeps );
			  if( NULL == pchToken )
			  {
			  printf("[AgentGetDvmpTable] Wrong profile while reading %s!\n", 
			  FIELD_mcueqpDvmpEntPort );
			  bResult = FALSE;
			  continue;
			  }
			  else
			  {
			  m_atDvmpBasicTable[dwLoop].SetMcuRecvPort( atoi(pchToken) );
			  }
			  
				// SwitchBrdId 
				pchToken = strtok( NULL, achSeps );
				if( NULL == pchToken)
				{
				printf("[AgentGetDvmpTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpDvmpEntSwitchBrdId );
				bResult = FALSE;
				continue;
				}
				else
				{
				m_atDvmpBasicTable[dwLoop].SetSwitchBrdId( atoi(pchToken) );
				}
				
				  // Alias
				  pchToken = strtok( NULL, achSeps );
				  if( NULL == pchToken )
				  {
				  printf("[AgentGetDvmpTable] Wrong profile while reading %s!\n", 
				  FIELD_mcueqpDvmpEntAlias );
				  bResult = FALSE;
				  continue;
				  }
				  else
				  {
				  m_atDvmpBasicTable[dwLoop].SetAlias( pchToken );
				  }
				  
					// RunningBrdId
					pchToken = strtok( NULL, achSeps );
					if( NULL == pchToken )
					{
					printf("[AgentGetDvmpTable] Wrong profile while reading %s!\n", 
					FIELD_mcueqpDvmpEntRunningBrdId );
					bResult = FALSE;
					continue;
					}
					else
					{
					m_atDvmpBasicTable[dwLoop].SetRunBrdId( atoi(pchToken) );
					for( dwBrdLoop = 0; dwBrdLoop < m_dwBrdCfgEntryNum; dwBrdLoop++)
					{
					if( m_atBrdCfgTable[dwBrdLoop].GetBrdId() == m_atDvmpBasicTable[dwLoop].GetRunBrdId() )
					{
					m_atDvmpBasicTable[dwLoop].SetIpAddr( m_atBrdCfgTable[dwBrdLoop].GetBrdIp() );
					bResult = m_atBrdCfgTable[dwBrdLoop].SetPeriId( m_atDvmpBasicTable[dwLoop].GetEqpId() );
					break;
					}
					}
					if( dwBrdLoop >= m_dwBrdCfgEntryNum )
					{
					printf("[AgentGetDvmpTable] Wrong Value %s = %s!\n", 
					FIELD_mcueqpDvmpEntRunningBrdId, pchToken);
					bResult = FALSE;
					}
					
					  }
					  
						// RecvStartPort
						pchToken = strtok( NULL, achSeps );
						if( NULL == pchToken)
						{
						printf("[AgentGetDvmpTable] Wrong profile while reading %s!\n", 
						FIELD_mcueqpDvmpEntRecvStartPort );
						bResult = FALSE;
						continue;
						}
						else
						{
						m_atDvmpBasicTable[dwLoop].SetEqpRecvPort( atoi(pchToken) );
						}
						
						  // 		// MapId
						  // 		while( ( pchToken = strtok( NULL, achSeps ) ) != NULL )
						  // 		{
						  //             m_atVMPTable[dwLoop].SetUsedMapId( atoi(pchToken) );
						  // 		}
						  // 		if( m_atVMPTable[dwLoop].GetUsedMapNum() == 0 )
						  // 		{
						  //             printf("[AgentGetDvmpTable] Wrong profile while reading %s!\n", 
						  //                                                 FIELD_mcueqpDvmpEntMapId );
						  // 			bResult = FALSE;
						  // 		}
						  
							// 初始化其他参数
							m_atDvmpBasicTable[dwLoop].SetDefaultEntParam( EQP_TYPE_VMP, ntohl(GetLocalIp()) );
							}   
							if( !bResult )
							{
							g_cAlarmProc.ProcReadError(ERR_AGENT_READVMPCFG);
							}
							// free memory 
							FREE_TABLE_MEM( ppszTable, nMemEntryNum );
							
							  //return TRUE;
							  return bResult;
							  }
*/
/*=============================================================================
函 数 名： AgentGetDvmpTable
功    能： 读MpuBas信息
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/

BOOL32 CCfgAgent::AgentGetMpuBasTable( const s8* lpszProfileName ) 
{
    if( NULL == lpszProfileName )
	{
		printf("[AgentGetMpuBasTable] error input parameter . (AgentGetMpuBasTable)! \n");
		return FALSE;
	}
	
	s8*   *ppszTable = NULL;
    s8    achSeps[] = " ,\t";       
    s8    *pchToken = NULL;
    u32   dwLoop;
	u32	  dwBrdLoop;
    s32   nMemEntryNum;
	
    // get the number of entry 
    BOOL32 bResult = GetRegKeyInt( lpszProfileName, SECTION_mcueqpMpuBasTable, 
		STR_ENTRY_NUM, 0, &nMemEntryNum );
	if( !bResult ) 
	{
		printf("[AgentGetMpuBasTable] Wrong profile while reading %s %s!\n", 
			SECTION_mcueqpMpuBasTable, STR_ENTRY_NUM );
        g_cAlarmProc.ProcReadError(ERR_AGENT_READBASCFG);
		return FALSE;
	}
	
    if ( nMemEntryNum == 0 )
    {
        m_dwMpuBasEntryNum = 0;
        return TRUE;
    }
	
	// alloc memory
    ppszTable = new s8*[(u32)nMemEntryNum];
    if( NULL == ppszTable )
    {
        printf("[AgentGetMpuBasTable] Fail to malloc memory for MpuBas table\n");
        return FALSE;
    }
    for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
    {
        ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
        if(NULL == ppszTable[dwLoop])
        {
            printf("[AgentGetMpuBasTable] Fail to malloc memory for Bas table\n");
            FREE_TABLE_MEM( ppszTable, dwLoop );
			return FALSE;
        }
    }
	
    m_dwMpuBasEntryNum = nMemEntryNum;
    bResult = AgtGetRegKeyStringTable( lpszProfileName, SECTION_mcueqpMpuBasTable,
		"fail", ppszTable, &m_dwMpuBasEntryNum, MAX_VALUE_LEN + 1);
    if( !bResult )
    {
		printf("[AgentGetMpuBasTable] Wrong profile while reading %s table!\n", 
			SECTION_mcueqpMpuBasTable);
		
        FREE_TABLE_MEM( ppszTable, nMemEntryNum );
        m_dwMpuBasEntryNum = 0;
        g_cAlarmProc.ProcReadError(ERR_AGENT_READBASCFG);
		return FALSE;
    }
	
    for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
    {
        // Id
        pchToken = strtok( ppszTable[dwLoop], achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetMpuBasTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpMpuBasEntId );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atMpuBasTable[dwLoop].SetEqpId( atoi(pchToken) );
        }
		
        // BasEntPort
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetMpuBasTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpMpuBasEntPort );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atMpuBasTable[dwLoop].SetMcuRecvPort( atoi(pchToken) );
        }
		
        // SwitchBrdId
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetMpuBasTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpMpuBasEntSwitchBrdId );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atMpuBasTable[dwLoop].SetSwitchBrdId( atoi(pchToken) );
        }
		
		// MpuBasEntAlias
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetMpuBasTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpMpuBasEntAlias );
            bResult = FALSE;
            continue;
        }
        else
        {
			if ( IsUtf8Encoding())
			{
				printf("[AgentGetMpuBasTable]MpuBas Alias:%d is UTF8\n", dwLoop);
				m_atMpuBasTable[dwLoop].SetAlias( pchToken );
			}
			else
			{
				printf("[AgentGetMpuBasTable]MpuBas Alias:%d is GBK\n", dwLoop);
				char achAlias[MAXLEN_EQP_ALIAS+1];
				memset(achAlias, 0 , sizeof(achAlias));
				gb2312_to_utf8(pchToken, achAlias, MAXLEN_EQP_ALIAS);
				m_atMpuBasTable[dwLoop].SetAlias( achAlias );
			}
        }
		
		// RunningBrdId 
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetMpuBasTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpMpuBasEntRunningBrdId );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atMpuBasTable[dwLoop].SetRunBrdId( atoi(pchToken) );
			for( dwBrdLoop = 0; dwBrdLoop < m_dwBrdCfgEntryNum; dwBrdLoop++)
			{
				if( m_atBrdCfgTable[dwBrdLoop].GetBrdId() == m_atMpuBasTable[dwLoop].GetRunBrdId() )
				{
					m_atMpuBasTable[dwLoop].SetIpAddr( m_atBrdCfgTable[dwBrdLoop].GetBrdIp() );
					bResult = m_atBrdCfgTable[dwBrdLoop].SetPeriId( m_atMpuBasTable[dwLoop].GetEqpId() );
					break;
				}
			}
			if( dwBrdLoop >= m_dwBrdCfgEntryNum )
			{
				printf("[AgentGetMpuBasTable] Wrong Value %s = %s!\n", 
					FIELD_mcueqpMpuBasEntRunningBrdId, pchToken);
				bResult = FALSE;
			}
        }
		
		// RecvStartPort
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetMpuBasTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpMpuBasEntRecvStartPort );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atMpuBasTable[dwLoop].SetEqpRecvPort( atoi(pchToken) );
        }
		
		
		//StartMode zjl20100426
		pchToken = strtok( NULL, achSeps );
		if (NULL == pchToken)
		{
			m_atMpuBasTable[dwLoop].SetStartMode(TYPE_MPUBAS);
			OspPrintf(TRUE, FALSE, "[AgentGetMpuBasTable] Set default mpu StartMode.%d because mcucfg not find it!\n", TYPE_MPUBAS);
		}
		else
		{
			m_atMpuBasTable[dwLoop].SetStartMode(atoi(pchToken));
			OspPrintf(TRUE, FALSE, "[AgentGetMpuBasTable] Read mpu StartMode.%d success!\n", atoi(pchToken));
		}
		
		// MapId
		// 		while( ( pchToken = strtok( NULL, achSeps ) ) != NULL )
		// 		{
		//             m_atBasTable[dwLoop].SetUsedMapId( atoi(pchToken) );
		// 		}                
		
		// MaxAdpChannel
		//		m_atMpuBasTable[dwLoop].SetAdpChnNum( 0 );  // 暂时不用该参数
        
        // 初始化其他参数
        m_atMpuBasTable[dwLoop].SetDefaultEntParam( EQP_TYPE_BAS, ntohl(GetLocalIp()) );
	}

    if( !bResult )
    {
        g_cAlarmProc.ProcReadError(ERR_AGENT_READBASCFG);
    }
    
    FREE_TABLE_MEM( ppszTable, nMemEntryNum );
    
	return bResult;
	
}

/*=============================================================================
函 数 名： AgentGetEbapTable
功    能： 读Ebap信息
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
/*
BOOL32 CCfgAgent::AgentGetEbapTable( const s8* lpszProfileName )  
{
if( NULL == lpszProfileName )
{
printf("[AgentGetEbapTable] error input parameter . (AgentGetEbapTable)! \n");
return FALSE;
}

  s8*   *ppszTable = NULL;
  s8    achSeps[] = " ,\t";       // space or tab as seperators 
  s8    *pchToken = NULL;
  u32   dwLoop;
  u32	  dwBrdLoop;
  s32   nMemEntryNum;
  
    // get the number of entry 
    BOOL32 bResult = GetRegKeyInt( lpszProfileName, SECTION_mcueqpEbapTable, 
	STR_ENTRY_NUM, 0, &nMemEntryNum );
	if( !bResult ) 
	{
	printf("[AgentGetEbapTable] Wrong profile while reading %s %s!\n", 
	SECTION_mcueqpEbapTable, STR_ENTRY_NUM );
	g_cAlarmProc.ProcReadError(ERR_AGENT_READBASCFG);
	return FALSE;
	}
	
	  if ( nMemEntryNum == 0 )
	  {
	  m_dwEbapEntryNum = 0;
	  return TRUE;
	  }
	  
		// alloc memory
		ppszTable = new s8*[nMemEntryNum];
		if( NULL == ppszTable )
		{
        printf("[AgentGetEbapTable] Fail to malloc memory for Ebap table\n");
        return FALSE;
		}
		for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
		{
        ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
        if(NULL == ppszTable[dwLoop])
        {
		printf("[AgentGetEbapTable] Fail to malloc memory for Ebap table\n");
		FREE_TABLE_MEM( ppszTable, dwLoop );
		return FALSE;
        }
		}
		
		  m_dwEbapEntryNum = nMemEntryNum;
		  bResult = AgtGetRegKeyStringTable( lpszProfileName, SECTION_mcueqpEbapTable,
		  "fail", ppszTable, &m_dwEbapEntryNum, MAX_VALUE_LEN + 1);
		  if( !bResult )
		  {
		  printf("[AgentGetEbapTable] Wrong profile while reading %s table!\n", 
		  SECTION_mcueqpEbapTable);
		  
			FREE_TABLE_MEM( ppszTable, nMemEntryNum );
			m_dwEbapEntryNum = 0;
			g_cAlarmProc.ProcReadError(ERR_AGENT_READBASCFG);
			return FALSE;
			}
			
			  for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
			  {
			  // Id
			  pchToken = strtok( ppszTable[dwLoop], achSeps );
			  if( NULL == pchToken )
			  {
			  printf("[AgentGetEbapTable] Wrong profile while reading %s!\n", 
			  FIELD_mcueqpEbapEntId );
			  bResult = FALSE;
			  continue;
			  }
			  else
			  {
			  m_atEbapTable[dwLoop].SetEqpId( atoi(pchToken) );
			  }
			  
				// EbapEntPort
				pchToken = strtok( NULL, achSeps );
				if( NULL == pchToken )
				{
				printf("[AgentGetEbapTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpEbapEntPort );
				bResult = FALSE;
				continue;
				}
				else
				{
				m_atEbapTable[dwLoop].SetMcuRecvPort( atoi(pchToken) );
				}
				
				  // SwitchBrdId
				  pchToken = strtok( NULL, achSeps );
				  if( NULL == pchToken )
				  {
				  printf("[AgentGetEbapTable] Wrong profile while reading %s!\n", 
				  FIELD_mcueqpEbapEntSwitchBrdId );
				  bResult = FALSE;
				  continue;
				  }
				  else
				  {
				  m_atEbapTable[dwLoop].SetSwitchBrdId( atoi(pchToken) );
				  }
				  
					// EbapEntAlias
					pchToken = strtok( NULL, achSeps );
					if( NULL == pchToken )
					{
					printf("[AgentGetMEbapTable] Wrong profile while reading %s!\n", 
					FIELD_mcueqpEbapEntAlias );
					bResult = FALSE;
					continue;
					}
					else
					{
					m_atEbapTable[dwLoop].SetAlias( pchToken );
					}
					
					  // RunningBrdId 
					  pchToken = strtok( NULL, achSeps );
					  if( NULL == pchToken )
					  {
					  printf("[AgentGetEbapTable] Wrong profile while reading %s!\n", 
					  FIELD_mcueqpEbapEntRunningBrdId );
					  bResult = FALSE;
					  continue;
					  }
					  else
					  {
					  m_atEbapTable[dwLoop].SetRunBrdId( atoi(pchToken) );
					  for( dwBrdLoop = 0; dwBrdLoop < m_dwBrdCfgEntryNum; dwBrdLoop++)
					  {
					  if( m_atBrdCfgTable[dwBrdLoop].GetBrdId() == m_atEbapTable[dwLoop].GetRunBrdId() )
					  {
					  m_atEbapTable[dwLoop].SetIpAddr( m_atBrdCfgTable[dwBrdLoop].GetBrdIp() );
					  bResult = m_atBrdCfgTable[dwBrdLoop].SetPeriId( m_atEbapTable[dwLoop].GetEqpId() );
					  break;
					  }
					  }
					  if( dwBrdLoop >= m_dwBrdCfgEntryNum )
					  {
					  printf("[AgentGetEbapTable] Wrong Value %s = %s!\n", 
					  FIELD_mcueqpEbapEntRunningBrdId, pchToken);
					  bResult = FALSE;
					  }
					  }
					  
						// RecvStartPort
						pchToken = strtok( NULL, achSeps );
						if( NULL == pchToken )
						{
						printf("[AgentGetEbapTable] Wrong profile while reading %s!\n", 
						FIELD_mcueqpEbapEntRecvStartPort );
						bResult = FALSE;
						continue;
						}
						else
						{
						m_atEbapTable[dwLoop].SetEqpRecvPort( atoi(pchToken) );
						}
						
						  // MapId
						  // 		while( ( pchToken = strtok( NULL, achSeps ) ) != NULL )
						  // 		{
						  //             m_atEbapTable[dwLoop].SetUsedMapId( atoi(pchToken) );
						  // 		}                
						  
							// MaxAdpChannel
							//		m_atEbapTable[dwLoop].SetAdpChnNum( 0 );  // 暂时不用该参数
							
							  // 初始化其他参数
							  m_atEbapTable[dwLoop].SetDefaultEntParam( EQP_TYPE_BAS, ntohl(GetLocalIp()) );
							  } 
							  
								if( !bResult )
								{
								g_cAlarmProc.ProcReadError(ERR_AGENT_READBASCFG);
								}
								
								  FREE_TABLE_MEM( ppszTable, nMemEntryNum );
								  
									//return TRUE;
									return bResult;
									}
*/
/*=============================================================================
函 数 名： AgentGetEvpuTable
功    能： 读Evpu信息
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
/*
BOOL32 CCfgAgent::AgentGetEvpuTable( const s8* lpszProfileName )   
{
if( NULL == lpszProfileName )
{
printf("[AgentGetEvpuTable] Error input parameter!\n");
return FALSE;
}

  s8*   *ppszTable = NULL;
  s8    achSeps[] = " ,\t";       // space or tab as seperators 
  s8    *pchToken = NULL;
  u32	  dwBrdLoop;
  u32   dwLoop;
  s32   nMemEntryNum;
  
    //  get the number of entry 
    BOOL32 bResult = GetRegKeyInt( lpszProfileName, SECTION_mcueqpEvpuTable, 
	STR_ENTRY_NUM, 0, &nMemEntryNum );
	if( !bResult) 
	{
	printf("[AgentGetEvpuTable] Wrong profile while reading %s %s!\n", 
	SECTION_mcueqpEvpuTable, STR_ENTRY_NUM );
	g_cAlarmProc.ProcReadError(ERR_AGENT_READVMPCFG);
	return FALSE;
	}
	
	  // 没有配置Vmp
	  if(0 == nMemEntryNum)
	  {
	  m_dwEvpuEntryNum = 0;
	  return TRUE;
	  }
	  
		// alloc memory
		ppszTable = new s8*[nMemEntryNum];
		if( NULL == ppszTable )
		{
		printf("[AgentGetEvpuTable] Fail to malloc memory for Evpu table\n");
		return FALSE;
		}
		for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
		{
        ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
        if(NULL == ppszTable[dwLoop] )
        {
		printf("[AgentGetEvpuTable] Fail to malloc memory for Evpu table\n");
		FREE_TABLE_MEM( ppszTable, dwLoop);
		return FALSE;
        }
		}
		
		  // get the vmp table
		  m_dwEvpuEntryNum = nMemEntryNum;
		  bResult = AgtGetRegKeyStringTable( lpszProfileName, SECTION_mcueqpEvpuTable,
		  "fail", ppszTable, &m_dwEvpuEntryNum, MAX_VALUE_LEN + 1 );
		  if( !bResult )
		  {
		  printf("[AgentGetEvpuTable] Wrong profile while reading %s table!\n", 
		  SECTION_mcueqpEvpuTable );
		  FREE_TABLE_MEM( ppszTable, nMemEntryNum);
		  m_dwEvpuEntryNum = 0;
		  g_cAlarmProc.ProcReadError(ERR_AGENT_READVMPCFG);
		  return FALSE;
		  }
		  
			for( dwLoop = 0; dwLoop < m_dwEvpuEntryNum; dwLoop++ )
			{
			// VMPEntId
			pchToken = strtok( ppszTable[dwLoop], achSeps );
			if( NULL == pchToken )
			{
            printf("[AgentGetEvpuTable] Wrong profile while reading %s!\n", 
			FIELD_mcueqpEvpuEntId );
            bResult = FALSE;
            continue;
			}
			else
			{
            m_atEvpuTable[dwLoop].SetEqpId( atoi(pchToken) );
			}
			
			  // VMPEntPort
			  pchToken = strtok( NULL, achSeps );
			  if( NULL == pchToken )
			  {
			  printf("[AgentGetEvpuTable] Wrong profile while reading %s!\n", 
			  FIELD_mcueqpEvpuEntPort );
			  bResult = FALSE;
			  continue;
			  }
			  else
			  {
			  m_atEvpuTable[dwLoop].SetMcuRecvPort( atoi(pchToken) );
			  }
			  
				// SwitchBrdId 
				pchToken = strtok( NULL, achSeps );
				if( NULL == pchToken)
				{
				printf("[AgentGetEvpuTable] Wrong profile while reading %s!\n", 
				FIELD_mcueqpEvpuEntSwitchBrdId );
				bResult = FALSE;
				continue;
				}
				else
				{
				m_atEvpuTable[dwLoop].SetSwitchBrdId( atoi(pchToken) );
				}
				
				  // Alias
				  pchToken = strtok( NULL, achSeps );
				  if( NULL == pchToken )
				  {
				  printf("[AgentGetEvpuTable] Wrong profile while reading %s!\n", 
				  FIELD_mcueqpEvpuEntAlias );
				  bResult = FALSE;
				  continue;
				  }
				  else
				  {
				  m_atEvpuTable[dwLoop].SetAlias( pchToken );
				  }
				  
					// RunningBrdId
					pchToken = strtok( NULL, achSeps );
					if( NULL == pchToken )
					{
					printf("[AgentGetEvpuTable] Wrong profile while reading %s!\n", 
					FIELD_mcueqpEvpuEntRunningBrdId );
					bResult = FALSE;
					continue;
					}
					else
					{
					m_atEvpuTable[dwLoop].SetRunBrdId( atoi(pchToken) );
					for( dwBrdLoop = 0; dwBrdLoop < m_dwBrdCfgEntryNum; dwBrdLoop++)
					{
					if( m_atBrdCfgTable[dwBrdLoop].GetBrdId() == m_atEvpuTable[dwLoop].GetRunBrdId() )
					{
					m_atEvpuTable[dwLoop].SetIpAddr( m_atBrdCfgTable[dwBrdLoop].GetBrdIp() );
					bResult = m_atBrdCfgTable[dwBrdLoop].SetPeriId( m_atEvpuTable[dwLoop].GetEqpId() );
					break;
					}
					}
					if( dwBrdLoop >= m_dwBrdCfgEntryNum )
					{
					printf("[AgentGetEvpuTable] Wrong Value %s = %s!\n", 
					FIELD_mcueqpEvpuEntRunningBrdId, pchToken);
					bResult = FALSE;
					}
					
					  }
					  
						// RecvStartPort
						pchToken = strtok( NULL, achSeps );
						if( NULL == pchToken)
						{
						printf("[AgentGetEvpuTable] Wrong profile while reading %s!\n", 
						FIELD_mcueqpEvpuEntRecvStartPort );
						bResult = FALSE;
						continue;
						}
						else
						{
						m_atEvpuTable[dwLoop].SetEqpRecvPort( atoi(pchToken) );
						}
						
						  // 初始化其他参数
						  m_atEvpuTable[dwLoop].SetDefaultEntParam( EQP_TYPE_VMP, ntohl(GetLocalIp()) );
						  }   
						  if( !bResult )
						  {
						  g_cAlarmProc.ProcReadError(ERR_AGENT_READVMPCFG);
						  }
						  // free memory 
						  FREE_TABLE_MEM( ppszTable, nMemEntryNum );
						  
							//return TRUE;
							return bResult;
							}
*/
/*=============================================================================
函 数 名： AgentGetHduSchemeTable
功    能： 读取HDU预案
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
BOOL32 CCfgAgent::AgentGetHduSchemeTable( const s8* lpszProfileName )
{
    if( NULL == lpszProfileName  )
	{
		printf("[AgentGetHduSchemeTable] Error param.pProName\n" );
		return FALSE;
	}
	
	s8	**ppszTable = NULL;
    s8    achSeps[] = " ,\t";       /* space or tab as seperators */
    s8    *pchToken = NULL;
    u32   dwLoop = 0;
    u32   dwMemEntryNum = 0;
	
    // get the number of entry 
    BOOL32  bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuHduSchemeTable, 
		STR_ENTRY_NUM, 0, (s32*)&dwMemEntryNum );
	if( !bResult ) 
	{
		printf("[AgentGetHduSchemeTable] Wrong profile while reading %s %s!\n", 
			SECTION_mcuHduSchemeTable, STR_ENTRY_NUM );
        g_cAlarmProc.ProcReadError(ERR_AGENT_READHDUSCHEMECFG);
		return FALSE;
	}
    
    m_dwHduSchemeNum = dwMemEntryNum;
    if(0 == dwMemEntryNum)
    {
        return TRUE;
    }
	
    ppszTable = new s8*[dwMemEntryNum];
    if(NULL == ppszTable)
    {
        printf("[AgentGetHduSchemeTable] Fail to malloc memory for HDU attach table\n");
        return  FALSE;
    }
    
    for( dwLoop = 0; dwLoop < dwMemEntryNum; dwLoop++ )
    {
        ppszTable[dwLoop] = new s8[ MAX_LINE_LEN + 1 ];
        if(NULL == ppszTable[dwLoop])
        {
            printf("[AgentGetHduSchemeTable] Fail to malloc memory for hdu attach table..\n");
            FREE_TABLE_MEM( ppszTable, dwLoop );
			return FALSE;
        }
    }
	
    bResult = AgtGetRegKeyStringTable(lpszProfileName, SECTION_mcuHduSchemeTable, 
		"Error", ppszTable, &dwMemEntryNum, MAX_VALUE_LEN + 1);
    if( !bResult )
    {
        printf("[AgentGetHduSchemeTable] Fail to read hdu attach table.\n");
        m_dwHduSchemeNum = 0;
        FREE_TABLE_MEM( ppszTable, dwMemEntryNum );  // 释放空间
        g_cAlarmProc.ProcReadError(ERR_AGENT_READHDUSCHEMECFG);
        return FALSE;
    }
	
    // read table
    for(dwLoop = 0; dwLoop < m_dwHduSchemeNum; dwLoop++)
    {
		// height
        pchToken = strtok( ppszTable[dwLoop], achSeps );
        if(NULL == pchToken)
        {
            printf("[AgentGetHduSchemeTable] Fail to read the hdu height value\n");
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atHduSchemeTable[dwLoop].SetHeight( atoi(pchToken) ); 
        }
        // width
        pchToken = strtok( NULL, achSeps );
        if(NULL == pchToken)
        {
            printf("[AgentGetHduSchemeTable] Fail to read the hdu width value\n");
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atHduSchemeTable[dwLoop].SetWidth( atoi(pchToken) ); 
        }
		
        //Is beQuiet
        pchToken = strtok( NULL, achSeps );
        if(NULL == pchToken)
        {
            printf("[AgentGetHduSchemeTable] Fail to read the hdu beQuiet value\n");
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atHduSchemeTable[dwLoop].SetIsBeQuiet( atoi(pchToken) );
        }
		
        // style idx
        pchToken = strtok( NULL, achSeps );
        if(NULL == pchToken)
        {
            printf("[AgentGetHduSchemeTable] Fail to read the hdu style idx value\n");
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atHduSchemeTable[dwLoop].SetStyleIdx( atoi(pchToken) );
        }
		
		// total chnl num
		pchToken = strtok( NULL, achSeps );
        if(NULL == pchToken)
        {
            printf("[AgentGetHduSchemeTable] Fail to read the hdu total chnl num value\n");
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atHduSchemeTable[dwLoop].SetTotalChnlNum( atoi(pchToken) );
        }
		
		// Volumn
		pchToken = strtok( NULL, achSeps );
        if(NULL == pchToken)
        {
            printf("[AgentGetHduSchemeTable] Fail to read the hdu Volumn value\n");
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atHduSchemeTable[dwLoop].SetVolumn( atoi(pchToken) );
        }
		
		// Alias
		pchToken = strtok( NULL, achSeps );
        if(NULL == pchToken)
        {
            printf("[AgentGetHduSchemeTable] Fail to read the hdu Alias value\n");
            m_atHduSchemeTable[dwLoop].SetSchemeAlias( "Unnamed" );
            bResult = FALSE;
            continue;
        }
        else
        {
			if ( IsUtf8Encoding())
			{
				printf("[AgentGetHduSchemeTable]HduScheme Alias:%d is UTF8\n", dwLoop);
				m_atHduSchemeTable[dwLoop].SetSchemeAlias( pchToken );
			}
			else
			{
				printf("[AgentGetHduSchemeTable]HduScheme Alias:%d is GBK\n", dwLoop);
				char achAlias[MAX_HDUSTYLE_ALIASLEN];
				memset(achAlias, 0 , sizeof(achAlias));
				gb2312_to_utf8(pchToken, achAlias, MAX_HDUSTYLE_ALIASLEN-1);
				m_atHduSchemeTable[dwLoop].SetSchemeAlias( achAlias );
			}
        }
		
		//read ChnTable
        u32 totalNum = m_atHduSchemeTable[dwLoop].GetTotalChnlNum();	
        if ( 0 == totalNum )
        {
			FREE_TABLE_MEM( ppszTable, dwLoop );
			return  TRUE; 
        }
		
		s8 **ppszChnTable = new s8*[totalNum];
		if(NULL == ppszChnTable)
		{
			printf("[AgentGetHduSchemeTable] Fail to malloc memory for HDU Chn  table\n");
			FREE_TABLE_MEM( ppszTable, dwLoop );
			return  FALSE;
		}
		u8 dwLoop2;
		for( dwLoop2 = 0; dwLoop2 < totalNum; dwLoop2++ )
		{
			ppszChnTable[dwLoop2] = new s8[ MAX_VALUE_LEN + 1 ];
			if(NULL == ppszChnTable[dwLoop2])
			{
				printf("[AgentGetHduSchemeTable] Fail to malloc memory for hdu attach table..\n");
				FREE_TABLE_MEM( ppszChnTable, dwLoop2 );
				FREE_TABLE_MEM( ppszTable, dwLoop );
				return FALSE;
			}
		}
		
		THduChnlInfo atHduChnlTable[MAXNUM_HDUCFG_CHNLNUM];
		memset(atHduChnlTable, 0x0, sizeof(atHduChnlTable));
        s8 achSchemeChnTableName[256];
		memset(achSchemeChnTableName, 0x0, sizeof(achSchemeChnTableName));
		switch (dwLoop)
		{
		case 0:
			memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable1, strlen(SECTION_mcuHduSchemeChnTable1) );
			break;
			
		case 1:
			memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable2, strlen(SECTION_mcuHduSchemeChnTable2) );
			break;
			
		case 2:
			memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable3, strlen(SECTION_mcuHduSchemeChnTable3) );
			break;
			
		case 3:
			memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable4, strlen(SECTION_mcuHduSchemeChnTable4) );
			break;
			
		case 4:
			memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable5, strlen(SECTION_mcuHduSchemeChnTable5) );
			break;
			
		case 5:
			memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable6, strlen(SECTION_mcuHduSchemeChnTable6) );
			break;
			
		case 6:
			memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable7, strlen(SECTION_mcuHduSchemeChnTable7) );
			break;
			
		case 7:
			memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable8, strlen(SECTION_mcuHduSchemeChnTable8) );
			break;
			
		case 8:
			memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable9, strlen(SECTION_mcuHduSchemeChnTable9) );
			break;
			
		case 9:
			memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable10, strlen(SECTION_mcuHduSchemeChnTable10) );
			break;
			
		case 10:
			memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable11, strlen(SECTION_mcuHduSchemeChnTable11) );
			break;
			
		case 11:
			memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable12, strlen(SECTION_mcuHduSchemeChnTable12) );
			break;
			
		case 12:
			memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable13, strlen(SECTION_mcuHduSchemeChnTable13) );
			break;
			
		case 13:
			memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable14, strlen(SECTION_mcuHduSchemeChnTable14) );
			break;
			
		case 14:
			memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable15, strlen(SECTION_mcuHduSchemeChnTable15) );
			break;
			
		case 15:
			memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable16, strlen(SECTION_mcuHduSchemeChnTable16) );
			break;
			
		default:
			OspPrintf(TRUE, FALSE, "[WriteHduSchemeTable] hdu Scheme's Index is beyond 15 !\n");
		}
		bResult = AgtGetRegKeyStringTable(lpszProfileName, achSchemeChnTableName, 
			"Error", ppszChnTable, &totalNum, MAX_VALUE_LEN + 1);
		if( !bResult )
		{
			printf("[AgentGetHduSchemeTable] Fail to read hdu attach table.\n");
			FREE_TABLE_MEM( ppszChnTable, totalNum );  // 释放空间
			FREE_TABLE_MEM( ppszTable, dwLoop );
			g_cAlarmProc.ProcReadError(ERR_AGENT_READHDUSCHEMECFG);
			return FALSE;
		}
		// chnl table
        s8    achChnSeps[] = " ,\t";       /* space or tab as seperators */
		s16  wIdx = -1;
		for ( u8 byIndex2=0; byIndex2<totalNum; byIndex2++)
		{
			//idx
			pchToken = strtok( ppszChnTable[byIndex2], achChnSeps );
			if(NULL == pchToken)
			{
				printf("[AgentGetHduSchemeTable] Fail to read the hdu chnl idx%d value\n", byIndex2+1);
				bResult = FALSE;
				continue;
			}
			else
			{
                wIdx =  atoi(pchToken); 
			}
			
			//Chnl idx
			pchToken = strtok( NULL, achChnSeps );
			if(NULL == pchToken)
			{
				printf("[AgentGetHduSchemeTable] Fail to read the hdu chnl idx%d value\n", byIndex2+1);
				atHduChnlTable[wIdx].SetChnlIdx(0);
				bResult = FALSE;
				continue;
			}
			else
			{
				atHduChnlTable[wIdx].SetChnlIdx( atoi(pchToken) );
			}
			
			//eqpId
			pchToken = strtok( NULL, achChnSeps );
			if(NULL == pchToken)
			{
				printf("[AgentGetHduSchemeTable] Fail to read the hdu chnl eqpId%d value\n", byIndex2+1);
				atHduChnlTable[wIdx].SetEqpId( 0 );
				bResult = FALSE;
				continue;
			}
			else
			{
				atHduChnlTable[wIdx].SetEqpId( atoi(pchToken) );
			}
			
			//volumn
			pchToken = strtok( NULL, achChnSeps );
			if(NULL == pchToken)
			{
				printf("[AgentGetHduSchemeTable] Fail to read the hdu chnl volumn%d value\n", byIndex2+1);
				atHduChnlTable[wIdx].SetChnlVolumn( 0 );
				bResult = FALSE;
				continue;
			}
			else
			{
				atHduChnlTable[wIdx].SetChnlVolumn( atoi(pchToken) );
			}
			
			//be Quiet
			pchToken = strtok( NULL, achChnSeps );
			if(NULL == pchToken)
			{
				printf("[AgentGetHduSchemeTable] Fail to read the hdu chnl Quiet%d value\n", byIndex2+1);
				atHduChnlTable[wIdx].SetIsChnlBeQuiet(TRUE);
				bResult = FALSE;
				continue;
			}
			else
			{
				atHduChnlTable[wIdx].SetIsChnlBeQuiet( atoi(pchToken) );
			}
            
			// alias
			pchToken = strtok( NULL, achChnSeps );
			if(NULL == pchToken)
			{
				printf("[AgentGetHduSchemeTable] Fail to read the hdu chnl alias%d value\n", byIndex2+1);
				atHduChnlTable[wIdx].SetSchemeAlias("UnNamed");
				bResult = FALSE;
				continue;
			}
			else
			{
				if ( IsUtf8Encoding())
				{
					printf("[AgentGetHduSchemeTable]HduScheme Alias:%d is UTF8\n", dwLoop);
					atHduChnlTable[wIdx].SetSchemeAlias( pchToken );
				}
				else
				{
					printf("[AgentGetHduSchemeTable]HduScheme Alias:%d is GBK\n", dwLoop);
					char achAlias[MAXLEN_EQP_ALIAS];
					memset(achAlias, 0 , sizeof(achAlias));
					gb2312_to_utf8(pchToken, achAlias, MAXLEN_EQP_ALIAS-1);
					atHduChnlTable[wIdx].SetSchemeAlias( achAlias );
				}
			}
		}
        m_atHduSchemeTable[dwLoop].SetHduChnlTable( atHduChnlTable );
		
		FREE_TABLE_MEM( ppszChnTable, totalNum );// 释放内存
		
    }
    if ( !bResult )
    {
        g_cAlarmProc.ProcReadError( ERR_AGENT_READHDUSCHEMECFG );
    }
	
	FREE_TABLE_MEM( ppszTable, dwMemEntryNum );// 释放内存
    return TRUE;
	
}

/*=============================================================================
函 数 名： AgentGetEqpDcs
功    能： 取Dcs配置信息
算法实现： 
全局变量： 
参    数：  const s8* lpszProfileName
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::AgentGetEqpDcs( const s8* lpszProfileName )
{
    if( NULL == lpszProfileName )
	{
		printf("[AgentGetEqpDcs] error input parameter.\n");
		return FALSE;
	}
	
	BOOL32 bResult;
    s8     achReturn[32];
    memset(achReturn, '\0', sizeof(achReturn));
	
	// DcsId
    if( VERSION_CONFIGURE36 == m_tMPCInfo.GetLocalConfigVer() )
    {
        bResult = GetRegKeyString( lpszProfileName, SECTION_mcueqpDataConfServer, 
			"mcueqpDcsId", "0.0.0.0", achReturn, sizeof(achReturn) );
    }
    else
    {
        bResult = GetRegKeyString( lpszProfileName, SECTION_mcueqpDataConfServer, 
			KEY_mcueqpDcsIp, "0.0.0.0", achReturn, sizeof(achReturn) );
    }
	
	if( !bResult)  
	{
		printf("[AgentGetMpwTable] Wrong profile while reading %s!\n", KEY_mcueqpDcsIp );
		g_cAlarmProc.ProcReadError(ERR_AGENT_READDATACONFCFG);
        m_tDCS.m_dwDcsIp = 0;
	}
    else
    {
        m_tDCS.m_dwDcsIp = ntohl(INET_ADDR(achReturn));
    }
	
	//return TRUE;
	return bResult;
}

/*=============================================================================
函 数 名： AgentGetPrsInfo
功    能： 取Prs配置信息
算法实现： 
全局变量： 
参    数：  const s8* lpszProfileName
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::AgentGetPrsInfo( const s8* lpszProfileName )
{
    if( NULL == lpszProfileName )
	{
		printf("[AgentGetPrsInfo] Error input parameter!\n");
		return FALSE;
	}
	s8*   *ppszTable = NULL;
    s8    achSeps[] = " ,\t";       /* space or tab as seperators */
    s8    *pchToken = NULL;
    u32   dwLoop;
	u32	  dwBrdLoop;
    BOOL32  bResult = TRUE;
    s32   nMemEntryNum;
	
    // get the number of entry 
    bResult = GetRegKeyInt( lpszProfileName, SECTION_mcueqpPrsTable, 
		STR_ENTRY_NUM, 0, &nMemEntryNum );
	if( !bResult ) 
	{
		printf("[AgentGetPrsInfo] Wrong profile while reading %s %s!\n", 
			SECTION_mcueqpPrsTable, STR_ENTRY_NUM );
        g_cAlarmProc.ProcReadError(ERR_AGENT_READBASCFG);
		return FALSE;
	}
	
	// alloc memory
    ppszTable = new s8*[(u32)nMemEntryNum];
    if(NULL == ppszTable)
    {
        printf("[AgentGetPrsInfo] Fail to malloc memory for Bas table\n");
        return FALSE;
    }
    for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
    {
        ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
        if(NULL == ppszTable[dwLoop])
        {
            printf("[AgentGetPrsInfo] Fail to malloc memory for Bas table\n");
            FREE_TABLE_MEM( ppszTable, dwLoop );
			return FALSE;
        }
    }
	
    m_dwPrsEntryNum = nMemEntryNum;
    bResult = AgtGetRegKeyStringTable( lpszProfileName, SECTION_mcueqpPrsTable,
		"fail", ppszTable, &m_dwPrsEntryNum, MAX_VALUE_LEN + 1 );
    if( !bResult )
    {
		printf("[AgentGetPrsInfo] Wrong profile while reading %s table!\n", 
			SECTION_mcueqpPrsTable);
		
        FREE_TABLE_MEM( ppszTable, nMemEntryNum );
        m_dwPrsEntryNum = 0;
        g_cAlarmProc.ProcReadError( ERR_AGENT_READBASCFG );
		return FALSE;
    }
	
    memset(m_atPrsTable, 0, sizeof(m_atPrsTable));
    for( dwLoop = 0; dwLoop < (u32)nMemEntryNum; dwLoop++ )
    {
        // Id
        pchToken = strtok( ppszTable[dwLoop], achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetPrsInfo] Wrong profile while reading %s!\n", FIELD_mcueqpPrsId );
            bResult = FALSE;
            m_atPrsTable[dwLoop].SetEqpId( 0 );
            continue;
        }
        else
        {
            u8 byPrsId = atoi( pchToken );
            if( 0 == byPrsId )
            {
                m_atPrsTable[dwLoop].SetEqpId( 0 );
                continue;
            }
            else if( VERSION_CONFIGURE36 == m_tMPCInfo.GetLocalConfigVer() )
            {
                byPrsId += 16;  // 跳过多画面复合电视墙
            }
            m_atPrsTable[dwLoop].SetEqpId( byPrsId );
        }
		
        // BasEntPort
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetPrsInfo] Wrong profile while reading %s!\n", 
				FIELD_mcueqpPrsMCUStartPort );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atPrsTable[dwLoop].SetMcuRecvPort( atoi(pchToken) );
        }
		
        // SwitchBrdId
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetPrsInfo] Wrong profile while reading %s!\n", 
				FIELD_mcueqpPrsSwitchBrdId );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atPrsTable[dwLoop].SetSwitchBrdId( atoi(pchToken) );
        }
		
		// BasEntAlias
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetPrsInfo] Wrong profile while reading %s!\n", 
				FIELD_mcueqpPrsAlias );
            bResult = FALSE;
            continue;
        }
        else
        {
			if ( IsUtf8Encoding())
			{
				printf("[AgentGetPrsInfo]Prs Alias:%d is UTF8\n", dwLoop);
				m_atPrsTable[dwLoop].SetAlias( pchToken );
			}
			else
			{
				printf("[AgentGetPrsInfo]Prs Alias:%d is GBK\n", dwLoop);
				char achAlias[MAXLEN_EQP_ALIAS+1];
				memset(achAlias, 0 , sizeof(achAlias));
				gb2312_to_utf8(pchToken, achAlias, MAXLEN_EQP_ALIAS);
				m_atPrsTable[dwLoop].SetAlias( achAlias );
			}
        }
		
		// RunningBrdId 
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetPrsInfo] Wrong profile while reading %s!\n", 
				FIELD_mcueqpBasEntRunningBrdId );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atPrsTable[dwLoop].SetRunBrdId( atoi(pchToken) );
			for( dwBrdLoop = 0; dwBrdLoop < m_dwBrdCfgEntryNum; dwBrdLoop++)
			{
				if( m_atBrdCfgTable[dwBrdLoop].GetBrdId() == m_atPrsTable[dwLoop].GetRunBrdId() )
				{
					m_atPrsTable[dwLoop].SetIpAddr( m_atBrdCfgTable[dwBrdLoop].GetBrdIp() );
					bResult = m_atBrdCfgTable[dwBrdLoop].SetPeriId( m_atPrsTable[dwLoop].GetEqpId() );
					break;
				}
			}
			if( dwBrdLoop >= m_dwBrdCfgEntryNum )
			{
				printf("[AgentGetPrsInfo] Wrong Value %s = %s!\n", 
					FIELD_mcueqpPrsRunningBrdId, pchToken);
				bResult = FALSE;
			}
        }
		
		// RecvStartPort
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetPrsInfo] Wrong profile while reading %s!\n", 
				FIELD_mcueqpPrsStartPort );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atPrsTable[dwLoop].SetEqpRecvPort( atoi(pchToken) );
        }
		
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetPrsInfo] Wrong profile while reading %s!\n", 
				FIELD_mcueqpPrsFirstTimeSpan );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atPrsTable[dwLoop].SetFirstTimeSpan( atoi(pchToken) );
        }
		
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetPrsInfo] Wrong profile while reading %s!\n", 
				FIELD_mcueqpPrsSecondTimeSpan );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atPrsTable[dwLoop].SetSecondTimeSpan( atoi(pchToken) );
        }
		
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetPrsInfo] Wrong profile while reading %s!\n", 
				FIELD_mcueqpPrsThirdTimeSpan );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atPrsTable[dwLoop].SetThirdTimeSpan( atoi(pchToken) );
        }
		
        pchToken = strtok( NULL, achSeps );
        if( NULL == pchToken )
        {
            printf("[AgentGetPrsInfo] Wrong profile while reading %s!\n", 
				FIELD_mcueqpPrsRejectTimeSpan );
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atPrsTable[dwLoop].SetRejectTimeSpan( atoi(pchToken) );
        }
        // 初始化其他参数
        m_atPrsTable[dwLoop].SetDefaultEntParam( EQP_TYPE_PRS, ntohl(GetLocalIp()) );
	}

    if( !bResult )
    {
        g_cAlarmProc.ProcReadError(ERR_AGENT_READPRSCFG);
    }
    
	// free memory 
    FREE_TABLE_MEM( ppszTable, nMemEntryNum );
    
    //return TRUE;
	return bResult;
}

/*=============================================================================
函 数 名： AgentGetNecSync
功    能： 取NetSync配置信息
算法实现： 
全局变量： 
参    数：  const s8* lpszProfileName
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::AgentGetNecSync( const s8* lpszProfileName )
{
    if( NULL == lpszProfileName )
	{
		printf("[AgentGetNecSync] error input parameter.\n");
		return FALSE;
	}
	
	// 防止windows下误用网步参数
#ifndef WIN32
	
#ifdef _MINIMCU_    
    m_tNetSync.SetMode( NETSYNTYPE_VIBRATION );
    m_tNetSync.SetE1Index( 0 );
    m_tNetSync.SetDTSlot( 0 );
	return TRUE;
#else
    s32  nValue = 0;
    BOOL32 bSuccRead = TRUE;
	
	// NetSyncMode
    BOOL32 bResult = GetRegKeyInt( lpszProfileName, SECTION_mcueqpNetSync, 
		KEY_mcueqpNetSyncMode, 0, &nValue );
	bSuccRead &= bResult;
	if( !bResult )  
	{
		printf("[AgentGetNecSync] Wrong profile while reading %s!\n", KEY_mcueqpNetSyncMode );
	}
    m_tNetSync.SetMode( bResult ? (u8)nValue : NETSYNTYPE_VIBRATION );
	
	
	if( NETSYNTYPE_VIBRATION == m_tNetSync.GetMode() ) // 自由振荡
	{
		BrdMPCSetNetSyncMode(SYNC_MODE_FREERUN);
	}
	else if( NETSYNTYPE_TRACE == m_tNetSync.GetMode() ) // 跟踪模式
	{
		BrdMPCSetNetSyncMode(SYNC_MODE_TRACK_DT2M);
	}
	else
	{
		printf("[AgentGetNecSync] Wrong mode value: %s = %d!\n", KEY_mcueqpNetSyncMode ,nValue);
		return FALSE;		
	}
    
	// DTSlot
    bResult = GetRegKeyInt( lpszProfileName, SECTION_mcueqpNetSync, 
		KEY_mcueqpNetSyncDTSlot, 0, &nValue );
	bSuccRead &= bResult;
	if( !bResult ) 
	{
		printf("[AgentGetNecSync] Wrong profile while reading %s!\n", KEY_mcueqpNetSyncDTSlot );
	}
    if ( bResult )
    {
        if( nValue > NETSYN_MAX_DTNUM || nValue < 0 )
        {
            printf("[AgentGetNecSync] Wrong value: %s = %d!\n", KEY_mcueqpNetSyncDTSlot, nValue);
            bResult = FALSE;
			bSuccRead &= bResult;
        }        
    }
    m_tNetSync.SetDTSlot( bResult ? (u8)nValue : 10 );
	
	// E1Index
    bResult = GetRegKeyInt( lpszProfileName, SECTION_mcueqpNetSync, 
		KEY_mcueqpNetSyncE1Index, 0, &nValue );
	bSuccRead &= bResult;
	if( !bResult ) 
	{
		printf("[AgentGetNecSync] Wrong profile while reading %s!\n", KEY_mcueqpNetSyncE1Index );
	}
    if ( bResult )
    {
        if( nValue > NETSYN_MAX_E1NUM || nValue < 0)
        {
            printf("[AgentGetNecSync] Wrong value: %s = %d!\n", KEY_mcueqpNetSyncE1Index, nValue);
            bResult = FALSE;
			bSuccRead &= bResult;
        }	        
    }
    m_tNetSync.SetE1Index( bResult ? (u8)nValue : 0 );
	
    if ( !bSuccRead )
    {
        g_cAlarmProc.ProcReadError( ERR_AGENT_READSYSCFG );
    }
    return bSuccRead;    
#endif  // minimcu
    
#else
    m_tNetSync.SetMode( NETSYNTYPE_VIBRATION );
    m_tNetSync.SetE1Index( 0 );
    m_tNetSync.SetDTSlot( 0 );
	
	return TRUE;
#endif	
}

/*=============================================================================
函 数 名： AgentGetQosInfo
功    能： 取Qos配置信息
算法实现： 
全局变量： 
参    数： const s8* lpszProfileName
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::AgentGetQosInfo(const s8* lpszProfileName )
{
    if( NULL == lpszProfileName )
	{
		printf("[AgentGetQosInfo] error input parameter.\n");
		return FALSE;
	}
	
    BOOL32 bSucceedRead = TRUE;
    
	s32  nValue = 0;
    // Qos type
    BOOL32 bResult = GetRegKeyInt(lpszProfileName, SECTION_mcuQosInfo, KEY_mcuQosType, 0, &nValue);
	bSucceedRead &= bResult;
    if( !bResult)
    {
        printf("[AgentGetQosInfo] Wrong profile while read %s\n", KEY_mcuQosType);
    }
    bSucceedRead &= m_tMcuQosInfo.SetQosType( bResult ? nValue : QOSTYPE_IP_PRIORITY );
	
	bResult = GetRegKeyInt(  lpszProfileName, SECTION_mcuQosInfo, KEY_mcuAudioLevel, 0, &nValue );
	bSucceedRead &= bResult;
    if( !bResult )
    {
        printf("[AgentGetQosInfo] Wrong profile while read %s\n", KEY_mcuAudioLevel);
    }
    bSucceedRead &= m_tMcuQosInfo.SetAudioLevel( bResult ? nValue : AUDIO_LVL_IP );   
	
	bResult = GetRegKeyInt(lpszProfileName, SECTION_mcuQosInfo, KEY_mcuVideoLevel, 0, &nValue);
	bSucceedRead &= bResult;
    if( !bResult )
    {
        printf("[AgentGetQosInfo] Wrong profile while read %s\n", KEY_mcuVideoLevel);
    }
    bSucceedRead &= m_tMcuQosInfo.SetVideoLevel( bResult ? nValue : VIDEO_LVL_IP );
	
    // data level
    bResult &= GetRegKeyInt(lpszProfileName, SECTION_mcuQosInfo, KEY_mcuDataLevel, 0, &nValue);
	bSucceedRead &= bResult;
    if( !bResult )
    {
        printf("[AgentGetQosInfo] Wrong profile when read: %s\n", KEY_mcuDataLevel);
    }
    bSucceedRead &= m_tMcuQosInfo.SetDataLevel( bResult ? nValue : DATA_LVL_IP );
	
    // singanl level 
    bResult = GetRegKeyInt(lpszProfileName, SECTION_mcuQosInfo, KEY_mcuSignalLevel, 0, &nValue);
	bSucceedRead &= bResult;
    if( !bResult )
    {
        printf("[AgentGetQosInfo] Wrong profile when read: %s\n", KEY_mcuSignalLevel);
    }
    bSucceedRead &= m_tMcuQosInfo.SetSignalLevel( bResult ? nValue : SIGNAL_LVL_IP );
    
    u8 achQosType[16];
    memset(achQosType, 0, sizeof(achQosType));
    
    // Ip service type
	// 非IP优先的话不需要读写mcuIpServiceType, zgc, 2008-04-21
	if ( QOSTYPE_IP_PRIORITY == m_tMcuQosInfo.GetQosType() )
    {
		bResult = GetRegKeyString(lpszProfileName, SECTION_mcuQosInfo, KEY_mcuIpServiceType, "", 
			(s8*)achQosType, sizeof(achQosType));
		bSucceedRead &= bResult;
		if( !bResult )
		{
			printf("[AgentGetQosInfo] Wrong profile when read: %s\n", KEY_mcuIpServiceType);
			bSucceedRead &= m_tMcuQosInfo.SetIpServiceType(0);
		}
		else
		{
			BOOL32 bRet = IsValidQosIpTypeStr(achQosType);
			if( bRet )
			{
				nValue = 0;
				bSucceedRead &= m_tMcuQosInfo.SetIpServiceType((u8)nValue);
				return bSucceedRead;
			}
			nValue = GetQosIpType(achQosType);
			if(ERR_AGENT_GETQOSIPTYPE != nValue)
			{
				bSucceedRead &= m_tMcuQosInfo.SetIpServiceType((u8)nValue);
			}
			else
			{
				bSucceedRead &= m_tMcuQosInfo.SetIpServiceType( IPSERVICETYPE_NONE );
			}
		}
	}
    if ( !bSucceedRead )
    {
        g_cAlarmProc.ProcReadError( ERR_AGENT_READQOSCFG );
    }
    //return TRUE;
	return bSucceedRead;
}
/*=============================================================================
函 数 名： AgentGetVCSSoftName
功    能： 
算法实现： 
全局变量： 
参    数： const s8* pachSoftName
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::AgentGetVCSSoftName( const s8* lpszProfileName )
{
    if( NULL == lpszProfileName )
    {
        printf("[AgentGetVCSSoftName] error input parameter.\n");
        return FALSE;
    }
	
    s8 achReturn[MAX_VCSSOFTNAME_LEN];   
    memset(achReturn, 0, sizeof(achReturn));
	
    BOOL32 bResult = GetRegKeyString(lpszProfileName, SECTION_mcuVCS, KEY_mcuVCSSoftName,
		VCS_DEFAULTSOFTNAME, achReturn, MAX_VCSSOFTNAME_LEN);
    if( !bResult)
    {
        printf("[AgentGetVCSSoftName] Wrong profile while read %s\n", KEY_mcuVCSSoftName);
    }
	
	if ( IsUtf8Encoding())
	{
		printf("[AgentGetVCSSoftName]Vcs def Title is UTF8\n");
		memcpy(m_achVCSSoftName, achReturn, MAX_VCSSOFTNAME_LEN);
	}
	else
	{
		printf("[AgentGetVCSSoftName]Vcs def Title is GBK\n");
		char achAlias[MAX_VCSSOFTNAME_LEN];
		memset(achAlias, 0 , sizeof(achAlias));
		u32 dwLen = gb2312_to_utf8(achReturn, achAlias, MAX_VCSSOFTNAME_LEN-1);
		dwLen = min(dwLen, MAX_VCSSOFTNAME_LEN);
		memcpy(m_achVCSSoftName, achAlias, dwLen);
	}
	return TRUE;
}

#ifdef _MINIMCU_
/*=============================================================================
函 数 名： AgentGetDSCInfo
功    能： 读取MCU8000B DSC特殊配置信息
算法实现： 
全局变量： 
参    数： const s8* lpszProfileName
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::AgentGetDSCInfo( const s8* lpszProfileName )
{
#ifdef _MINIMCU_
    if( NULL == lpszProfileName )
    {
        printf("[GetMcu8000B DSC Info] error input parameter.\n");
        return FALSE;
    }
	
    s8 achReturn [32];   
    s32  nValue = 0;
	u32 dwLop = 0;
	
    // start mp
    BOOL32 bResult = GetRegKeyInt(lpszProfileName, SECTION_DscModule, KEY_DscMp, TRUE, &nValue);
    if( !bResult)
    {
        printf("[Get8000BDSCInfo] Wrong profile while read %s\n", KEY_DscMp);
    }
    m_tMcuDscInfo.SetStartMp( bResult ? nValue : TRUE );
	
    // start mtadp
    bResult = GetRegKeyInt(lpszProfileName, SECTION_DscModule, KEY_DscMtAdp, TRUE, &nValue);
    if (!bResult)
    {
        printf("[Get8000BDSCInfo] Wrong profile while read %s\n", KEY_DscMtAdp);
    }
    m_tMcuDscInfo.SetStartMtAdp( bResult ? nValue : TRUE );
	
    // start gk
    bResult = GetRegKeyInt(lpszProfileName, SECTION_DscModule, KEY_DscGk, TRUE, &nValue);
    if (!bResult)
    {
        printf("[Get8000BDSCInfo] Wrong profile while read %s\n", KEY_DscGk);
    }
    m_tMcuDscInfo.SetStartGk( bResult ? nValue : TRUE );
	
    // start Proxy
    bResult = GetRegKeyInt(lpszProfileName, SECTION_DscModule, KEY_DscProxy, TRUE, &nValue);
    if (!bResult)
    {
        printf("[Get8000BDSCInfo] Wrong profile while read %s\n", KEY_DscProxy);
    }
    m_tMcuDscInfo.SetStartProxy( bResult ? nValue : TRUE );
    
    // start Dcs
    bResult = GetRegKeyInt(lpszProfileName, SECTION_DscModule, KEY_DscDcs, TRUE, &nValue);
    if (!bResult)
    {
        printf("[Get8000BDSCInfo] Wrong profile while read %s\n", KEY_DscDcs);
    }
    m_tMcuDscInfo.SetStartDcs( bResult ? nValue : TRUE );
    
    // 对端IP，先从配置文件里面读取，方便用户在MDSC不在线时配置。但是还是以连接上来的为准
    bResult = GetRegKeyString( lpszProfileName, SECTION_DscModule, KEY_DscInnerIp,
		DEFAULT_DSC_INNERIP, achReturn, sizeof(achReturn) );
    
    if( !bResult)  
    {
        printf("[Get8000BDSCInfo] Wrong profile while reading %s!\n", KEY_DscInnerIp );
        m_tMcuDscInfo.SetDscInnerIp( ntohl(INET_ADDR(DEFAULT_DSC_INNERIP)) );
    }
    else
    {
        m_tMcuDscInfo.SetDscInnerIp( ntohl(INET_ADDR(achReturn)) );
    } 
	
	// Net Type
	bResult = GetRegKeyInt(lpszProfileName, SECTION_DscModule, KEY_NetType, NETTYPE_LAN, &nValue);
	if (!bResult)
    {
        printf("[Get8000BDSCInfo] Wrong profile while read %s\n", KEY_NetType);
    }
	m_tMcuDscInfo.SetNetType( (u8)nValue );
	
	// LanMtProxyIpPos
	bResult = GetRegKeyInt(lpszProfileName, SECTION_DscModule, KEY_LanMtProxyIpPos, 0, &nValue);
	if (!bResult)
    {
        printf("[Get8000BDSCInfo] Wrong profile while read %s\n", KEY_LanMtProxyIpPos);
    }
	m_tMcuDscInfo.SetLanMtProxyIpPos( (u8)nValue );
	
	// MCU IP 和端口则自行获取
    // 尝试内网口
#ifndef _VXWORKS_
	TBrdEthParamAll tBrdEthParamAll;
	memset( &tBrdEthParamAll, 0 ,sizeof(TBrdEthParamAll) );
	STATUS nRet = AgtGetBrdEthParamAll( 1, &tBrdEthParamAll );
	u32 dwIpNum = tBrdEthParamAll.dwIpNum;
	u32 dwMpcInnerIpMask = 0;
	u32 dwDscInnerIp = 0;
	if ( dwIpNum > 0)
	{
		for( dwLop = 0; dwLop < dwIpNum; dwLop++ )
		{
			dwMpcInnerIpMask = tBrdEthParamAll.atBrdEthParam[dwLop].dwIpMask;
			dwDscInnerIp = htonl(m_tMcuDscInfo.GetDscInnerIp());
			if ( (dwDscInnerIp&dwMpcInnerIpMask) == (tBrdEthParamAll.atBrdEthParam[dwLop].dwIpAdrs&dwMpcInnerIpMask) )
			{
				m_tMcuDscInfo.SetMcuInnerIp(ntohl(tBrdEthParamAll.atBrdEthParam[dwLop].dwIpAdrs));
				m_tMcuDscInfo.SetInnerIpMask(ntohl(dwMpcInnerIpMask));
				break;
			}
		}
		if ( dwLop == dwIpNum )
		{
			m_tMcuDscInfo.SetMcuInnerIp( ntohl(INET_ADDR(DEFAULT_MCU_INNERIP)) );
			m_tMcuDscInfo.SetInnerIpMask( ntohl(INET_ADDR(DEFAULT_INNERIPMASK)) );
		}
	}
	else
	{
		// 没有配置内网口，使用默认的内部IP, zgc, 2007-05-30
		m_tMcuDscInfo.SetMcuInnerIp( ntohl(INET_ADDR(DEFAULT_MCU_INNERIP)) );
		m_tMcuDscInfo.SetInnerIpMask( ntohl(INET_ADDR(DEFAULT_INNERIPMASK)) );
	}
#else
	//#ifdef _VXWORKS_
    TBrdEthParam tEthParam;
    STATUS nRet = AgtGetBrdEthParam( 1, &tEthParam );
    if ( nRet == OK && tEthParam.dwIpAdrs != 0 )
    {
        m_tMcuDscInfo.SetMcuInnerIp( ntohl(tEthParam.dwIpAdrs) );
        m_tMcuDscInfo.SetInnerIpMask( ntohl(tEthParam.dwIpMask) );
    }
    else
    {
		// 没有配置内网口，使用默认的内部IP, zgc, 2007-05-30
		m_tMcuDscInfo.SetMcuInnerIp( ntohl(INET_ADDR(DEFAULT_MCU_INNERIP)) );
		m_tMcuDscInfo.SetInnerIpMask( ntohl(INET_ADDR(DEFAULT_INNERIPMASK)) );
    }
#endif
	//#ifdef WIN32
	//    m_tMcuDscInfo.SetMcuInnerIp( ntohl(INET_ADDR(DEFAULT_MCU_INNERIP)) );
	//	m_tMcuDscInfo.SetInnerIpMask( ntohl(INET_ADDR(DEFAULT_INNERIPMASK)) );
	//#endif
	
    // 端口
    m_tMcuDscInfo.SetMcuPort( g_tAgentDebugInfo.GetListenPort() );
	
	u8 byIpNum = 0;
	u8 byLop = 0;
	s8 asEntryStr[32];
	s8 asMcsAccessAddrStr[64];
	s8 asCallAddrStr[128];
	
	s8 achSeps[] = " \t";        // 分隔符
    s8 *pchToken = NULL;
	TMINIMCUNetParamAll tNetParamAll;
	TMINIMCUNetParam tNetParam;
	
	// 如果MiniMcuNetParam没有设，则通知会控弹配置向导, zgc, 2007-08-24
	BOOL32 bReadMiniMcuNetParam = TRUE;
	
	// 读McsAccessAddr
	memset( &tNetParamAll, 0, sizeof(tNetParamAll) );
	bResult = GetRegKeyInt(lpszProfileName, SECTION_DscModule, KEY_MCSAccessAddrNum, 0, &nValue);
    if (!bResult)
    {
        //FIXME: 是否是Fatal error?能否从NIP里面获取一下？
        /*
        printf("[Get8000BDSCInfo] Wrong profile while read %s\n", KEY_MCSAccessAddrNum);
        */
        byIpNum = 0;
    }
	else
	{
		byIpNum = nValue;
	}
	if ( 0 == byIpNum )
	{
		bReadMiniMcuNetParam = FALSE;
	}
	
    u8  byNetType;
    u32 dwIp;
    u32 dwMask;
    u32 dwGW;
    s8* szAlias = NULL;
    
	for ( byLop = 0; byLop < byIpNum; byLop++ )
	{
		szAlias = NULL;
		memset( &tNetParam, 0, sizeof(tNetParam) );
		
		memset( asEntryStr, 0, sizeof(asEntryStr) );
		sprintf(asEntryStr, "%s%d", ENTRY_MCSAccessAddr, byLop);
		
		memset( asMcsAccessAddrStr, 0, sizeof(asMcsAccessAddrStr) );
		GetRegKeyString( lpszProfileName, SECTION_DscModule, asEntryStr,
			"fail", asMcsAccessAddrStr, sizeof(asMcsAccessAddrStr) );
		
        // Network Type
        pchToken = strtok( asMcsAccessAddrStr, achSeps );
        if (NULL == pchToken)
        {
            printf( "[Get8000BDSCInfo] Wrong profile while reading %s NetworkType!\n", asEntryStr );
            continue;
        }
        else
        {
            byNetType = atoi(pchToken);
        }
        
		// IpAddr
		pchToken = strtok( NULL, achSeps );
        if (NULL == pchToken)
        {
            printf( "[Get8000BDSCInfo] Wrong profile while reading %s Ip!\n", asEntryStr );
            continue;
        }
        else
        {
			dwIp = ntohl(INET_ADDR(pchToken));
        }
		
		// IpMask
		pchToken = strtok( NULL, achSeps );
        if (NULL == pchToken)
        {
            printf( "[Get8000BDSCInfo] Wrong profile while reading %s IpMask!\n", asEntryStr );
            continue;
        }
        else
        {
			dwMask = ntohl(INET_ADDR(pchToken));
        }
		
		// GWIpAddr
        pchToken = strtok( NULL, achSeps );
        if (NULL == pchToken)
        {
            printf("[Get8000BDSCInfo] Wrong profile while reading %s GWIp!\n", asEntryStr );
            continue;
        }
        else
        {
			dwGW = ntohl(INET_ADDR(pchToken));
        }
		
		// ServerAlias
        pchToken = strtok( NULL, achSeps );
        if (NULL != pchToken)
        {
            szAlias = pchToken;
        }
		
        if ( byNetType == (u8)TMINIMCUNetParam::enLAN )
        {
            tNetParam.SetLanParam( dwIp, dwMask, dwGW );
        }
        else if ( byNetType == (u8)TMINIMCUNetParam::enWAN )
        {
            tNetParam.SetWanParam( dwIp, dwMask, dwGW, szAlias );
        }
        else
        {
            printf("[Get8000BDSCInfo] Wrong profile line while reading %s!\n", asEntryStr );
            continue;
        }
		tNetParamAll.AddOneNetParam( tNetParam );
	}
	m_tMcuDscInfo.SetMcsAccessAddrAll( tNetParamAll );
	
	// 读CallAddr
	memset( &tNetParamAll, 0, sizeof(tNetParamAll) );
	bResult = GetRegKeyInt(lpszProfileName, SECTION_DscModule, KEY_CallAddrNum, 0, &nValue);
    if (!bResult)
    {
        //FIXME: 是否是Fatal Error? 能否从DSC上报的里面获取一下？
        byIpNum = 0;
        /*
        printf("[Get8000BDSCInfo] Wrong profile while read %s\n", KEY_CallAddrNum);
        */
    }
	else
	{
		byIpNum = nValue;
	}
	if ( 0 == byIpNum )
	{
		bReadMiniMcuNetParam = FALSE;
	}
	
	for ( byLop = 0; byLop < byIpNum; byLop++ )
	{
		szAlias = NULL;
		memset( &tNetParam, 0, sizeof(tNetParam) );
		
		memset( asEntryStr, 0, sizeof(asEntryStr) );
		sprintf(asEntryStr, "%s%d", ENTRY_CallAddr, byLop);
		
		memset( asCallAddrStr, 0, sizeof(asCallAddrStr) );
		GetRegKeyString( lpszProfileName, SECTION_DscModule, asEntryStr,
			"fail", asCallAddrStr, sizeof(asCallAddrStr) );
		
        // Network Type
        pchToken = strtok( asCallAddrStr, achSeps );
        if (NULL == pchToken)
        {
            printf( "[Get8000BDSCInfo] Wrong profile while reading %s NetworkType!\n", asEntryStr );
            continue;
        }
        else
        {
            byNetType = atoi(pchToken);
        }
		
		// IpAddr
		pchToken = strtok( NULL, achSeps );
        if (NULL == pchToken)
        {
            printf( "[Get8000BDSCInfo] Wrong profile while reading %s!\n", asEntryStr );
            continue;
        }
        else
        {
			dwIp = ntohl(INET_ADDR(pchToken));
        }
		
		// IpMask
		pchToken = strtok( NULL, achSeps );
        if (NULL == pchToken)
        {
            printf( "[Get8000BDSCInfo] Wrong profile while reading %s!\n", asEntryStr );
            continue;
        }
        else
        {
			dwMask = ntohl(INET_ADDR(pchToken));
        }
		
		// GWIpAddr
        pchToken = strtok( NULL, achSeps );
        if (NULL == pchToken)
        {
            printf("[Get8000BDSCInfo] Wrong profile while reading %s!\n", asEntryStr );
            continue;
        }
        else
        {
			dwGW = ntohl(INET_ADDR(pchToken));
        }
		
		// ServerAlias
        pchToken = strtok( NULL, achSeps );
        if (NULL != pchToken)
        {
            szAlias = pchToken;
        }
		
        if ( byNetType == (u8)TMINIMCUNetParam::enLAN )
        {
            tNetParam.SetLanParam( dwIp, dwMask, dwGW );
        }
        else if ( byNetType == (u8)TMINIMCUNetParam::enWAN )
        {
            tNetParam.SetWanParam( dwIp, dwMask, dwGW, szAlias );
        }
        else
        {
            printf("[Get8000BDSCInfo] Wrong profile line while reading %s!\n", asEntryStr );
            continue;
        }
		tNetParamAll.AddOneNetParam( tNetParam );
	}
	m_tMcuDscInfo.SetCallAddrAll( tNetParamAll );
	
    if ( !bReadMiniMcuNetParam )
    {
        g_cAlarmProc.ProcReadError( ERR_AGENT_READ8000BINFO );
		m_tMcuSystem.SetIsMcuConfiged( FALSE );
    }
	
	m_tLastMcuDscInfo = m_tMcuDscInfo;
	
#else
    memset(&m_tMcuDscInfo, 0, sizeof(m_tMcuDscInfo) );
#endif
	
#ifdef _MINIMCU_
	return TRUE;
#else
    return TRUE;
#endif	
}
#endif

/*=============================================================================
函 数 名： IsValidQosIpTypeStr
功    能： 检测Qos Ip字符串
算法实现： 
全局变量： 
参    数： u8* pchIpTypeStr
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::IsValidQosIpTypeStr(u8* pchIpTypeStr)
{
    if(NULL == pchIpTypeStr)
    {
        return FALSE;
    }
	
    BOOL32 bRet = TRUE;
    s32 nCount1 = 0;
    s32 nCount2 = 0;
    s32 nCount3 = 0;
    s32 nCount4 = 0;
    s32 nLoop = 0;
    while('\0' != pchIpTypeStr[nLoop++])
    {
        switch(pchIpTypeStr[nLoop]) {
        case '1':
            nCount1++;
			break;
        case '2':
            nCount2++;
            break;
        case '3':
            nCount3++;
            break;
			
        case '4':
            nCount4++;
            break;
			
        case ',':
        case ' ':
            break;
			
        default:
            bRet = FALSE;
            break;
        }
    }
	
    if(nCount1 > 1 || nCount2 > 1 || nCount3 > 1 || nCount4 > 1 || FALSE == bRet)
    {
        bRet = FALSE;
    }
    return bRet;
}

/*=============================================================================
函 数 名： GetQosIpType
功    能： 取QosIp字符串
算法实现： 
全局变量： 
参    数： u8* pchIpType
u8 byLen
返 回 值： u8 
=============================================================================*/
u8 CCfgAgent::GetQosIpType(u8* pchIpType)
{
    u8  byRetType = ERR_AGENT_GETQOSIPTYPE;
    u8  byCount = 0; // 计数
    s8* pchToken = NULL;
    u8  achSteps[] = ",\t";
	
	if (!pchIpType) return ERR_AGENT_GETQOSIPTYPE;
	
    Trim((s8*)pchIpType);
    pchToken = strtok((s8*)pchIpType, (const s8*)achSteps);
	if (!pchToken) return ERR_AGENT_GETQOSIPTYPE;
	
    u8 byTmpData = atoi(pchToken);
    
    // 取类型值, 检查是否在范围之内
    if ( byTmpData > MAX_IPSERVICE_LVL )
    {
        return byRetType;
    }
	
    if(0 == byTmpData)
    {
        byRetType = 0;
        return byRetType;
    }
    else
    {
        byRetType = 0;
        byRetType = CombineQosValue(byRetType, byTmpData);
    }
    
    while( NULL != ( pchToken = strtok( NULL, (const s8*)achSteps ) ))
	{
        byTmpData = atoi(pchToken);
        if(0 != byTmpData)
        {
            byRetType = CombineQosValue(byRetType, byTmpData);
        }
        else
        {
            byRetType = 0;
            break;
        }
	}    
    return byRetType;
}

/*=============================================================================
函 数 名： CombineQosValue
功    能： 合成Qos值
算法实现： 
全局变量： 
参    数： u8 byCombineValue
u8 byType
返 回 值： u8 
=============================================================================*/
u8 CCfgAgent::CombineQosValue(u8 byCombineValue, u8 byType)
{
    switch(byType) {
    case QOS_IP_LEASTDELAY:
        byCombineValue += (u8)IPSERVICETYPE_LOW_DELAY;
        break;
        
    case QOS_IP_THRUPUT:
        byCombineValue += (u8)IPSERVICETYPE_HIGH_THROUGHPUT;
        break;
		
    case QOS_IP_RELIABLITY:
        byCombineValue += (u8)IPSERVICETYPE_HIGH_RELIABILITY;
        break;
		
    case QOS_IP_EXPENSES:
        byCombineValue += (u8)IPSERVICETYPE_LOW_EXPENSE;
        break;
    default:
        break;
    }
    return byCombineValue;
}
/*=============================================================================
函 数 名： AgentGetVmpAttachInfo
功    能： 取Vmp附属信息
算法实现： 
全局变量： 
参    数： const s8* lpszProfileName
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::AgentGetVmpAttachTable(const s8* lpszProfileName)
{
    if( NULL == lpszProfileName  )
	{
		printf("[AgentGetVmpAttachTable] Error param.pProName\n" );
		return FALSE;
	}
	
	s8*   *ppszTable = NULL;
    s8    achSeps[] = " ,\t";       /* space or tab as seperators */
    s8    *pchToken = NULL;
    u32   dwLoop = 0;
    u32   dwMemEntryNum = 0;
	
    // get the number of entry 
    BOOL32  bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuVmpAttachTable, 
		STR_ENTRY_NUM, 0, (s32*)&dwMemEntryNum );
	if( !bResult ) 
	{
		printf("[AgentGetVmpAttachTable] Wrong profile while reading %s %s!\n", 
			SECTION_mcuVmpAttachTable, STR_ENTRY_NUM );
        g_cAlarmProc.ProcReadError(ERR_AGENT_READVMPATTACHCFG);
		return FALSE;
	}
    
    m_dwVmpAttachNum = dwMemEntryNum;
    if(0 == dwMemEntryNum)
    {
        return TRUE;
    }
	
    ppszTable = new s8*[dwMemEntryNum];
    if(NULL == ppszTable)
    {
        printf("[AgentGetVmpAttachTable] Fail to malloc memory for vmp attach table\n");
        return  FALSE;
    }
    
    for( dwLoop = 0; dwLoop < dwMemEntryNum; dwLoop++ )
    {
        ppszTable[dwLoop] = new s8[ MAX_VALUE_LEN + 1 ];
        if(NULL == ppszTable[dwLoop])
        {
            printf("[AgentGetVmpAttachTable] Fail to malloc memory for vmp attach table..\n");
            FREE_TABLE_MEM( ppszTable, dwLoop );
			return FALSE;
        }
    }
	
    bResult = AgtGetRegKeyStringTable(lpszProfileName, SECTION_mcuVmpAttachTable, 
		"Error", ppszTable, &dwMemEntryNum, MAX_VALUE_LEN + 1);
    if( !bResult )
    {
        printf("[AgentGetVmpAttachTable] Fail to read vmp attach table.\n");
        m_dwVmpAttachNum = 0;
        FREE_TABLE_MEM( ppszTable, dwMemEntryNum );  // 释放空间
        g_cAlarmProc.ProcReadError(ERR_AGENT_READVMPATTACHCFG);
        return FALSE;
    }
	
    // read table
    for(dwLoop = 0; dwLoop < m_dwVmpAttachNum; dwLoop++)
    {
        // index
        pchToken = strtok( ppszTable[dwLoop], achSeps );
        if(NULL == pchToken)
        {
            printf("[AgentGetVmpAttachTable] Fail to read the vmp attach index.\n");
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atVmpAttachCfg[dwLoop].SetIndex( atoi(pchToken) );
        }
        // blackgroud color
        pchToken = strtok( NULL, achSeps );
        if(NULL == pchToken)
        {
            printf("[AgentGetVmpAttachTable] Fail to read the vmp attach blackgroud color value\n");
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atVmpAttachCfg[dwLoop].SetBGDColor( atoi(pchToken) );
        }
        // frame color
        pchToken = strtok( NULL, achSeps );
        if(NULL == pchToken)
        {
            printf("[AgentGetVmpAttachTable] Fail to read the vmp attach frame color value\n");
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atVmpAttachCfg[dwLoop].SetFrameColor( atoi(pchToken) );
        }
        // speaker color
        pchToken = strtok( NULL, achSeps );
        if(NULL == pchToken)
        {
            printf("[AgentGetVmpAttachTable] Fail to read the vmp attach speaker color value\n");
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atVmpAttachCfg[dwLoop].SetSpeakerFrameColor( atoi(pchToken) );
        }
        // chair color
        pchToken = strtok( NULL, achSeps );
        if(NULL == pchToken)
        {
            printf("[AgentGetVmpAttachTable] Fail to read the vmp attach chair color value\n");
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atVmpAttachCfg[dwLoop].SetChairFrameColor( atoi(pchToken) );
        }
		// font type
		pchToken = strtok( NULL, achSeps );
        if(NULL == pchToken)
        {
            printf("[AgentGetVmpAttachTable] Fail to read the vmp attach font type value\n");
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atVmpAttachCfg[dwLoop].SetFontType( atoi(pchToken) );
        }
		// font size
		pchToken = strtok( NULL, achSeps );
        if(NULL == pchToken)
        {
            printf("[AgentGetVmpAttachTable] Fail to read the vmp attach font size value\n");
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atVmpAttachCfg[dwLoop].SetFontSize( atoi(pchToken) );
        }
		// alignment 
		pchToken = strtok( NULL, achSeps );
        if(NULL == pchToken)
        {
            printf("[AgentGetVmpAttachTable] Fail to read the vmp attach font size value\n");
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atVmpAttachCfg[dwLoop].SetAlignType( atoi(pchToken) );
        }
		// text color
		pchToken = strtok( NULL, achSeps );
        if(NULL == pchToken)
        {
            printf("[AgentGetVmpAttachTable] Fail to read the vmp attach text color value\n");
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atVmpAttachCfg[dwLoop].SetTextColor( atoi(pchToken) );
        }
		// back color
		pchToken = strtok( NULL, achSeps );
        if(NULL == pchToken)
        {
            printf("[AgentGetVmpAttachTable] Fail to read the vmp attach back color value\n");
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atVmpAttachCfg[dwLoop].SetTopicBGDColor( atoi(pchToken) );
        }
		// diaphaneity
		pchToken = strtok( NULL, achSeps );
        if(NULL == pchToken)
        {
            printf("[AgentGetVmpAttachTable] Fail to read the vmp attach diaphaneity value\n");
            bResult = FALSE;
            continue;
        }
        else
        {
            m_atVmpAttachCfg[dwLoop].SetDiaphaneity( atoi(pchToken) );
        }		
        // alias
        pchToken = strtok(NULL, achSeps);
        if(NULL == pchToken)
        {
            printf("[AgentGetVmpAttachTable] Fail to read the vmp style alias\n");
            m_atVmpAttachCfg[dwLoop].SetAlias( "Unnamed" );
            /*
            bResult = FALSE;
            continue;
            */
        }
        else
        {
			if ( IsUtf8Encoding())
			{
				printf("[AgentGetVmpAttachTable]VmpScheme Alias:%d is UTF8\n", dwLoop);
				m_atVmpAttachCfg[dwLoop].SetAlias( pchToken );
			}
			else
			{
				printf("[AgentGetVmpAttachTable]VmpScheme Alias:%d is GBK\n", dwLoop);
				char achAlias[MAX_VMPSTYLE_ALIASLEN];
				memset(achAlias, 0 , sizeof(achAlias));
				gb2312_to_utf8(pchToken, achAlias, MAX_VMPSTYLE_ALIASLEN-1);
				m_atVmpAttachCfg[dwLoop].SetAlias( achAlias );
			}
        }
    }
    if ( !bResult )
    {
        g_cAlarmProc.ProcReadError( ERR_AGENT_READVMPATTACHCFG );
    }
	
	FREE_TABLE_MEM( ppszTable, dwMemEntryNum );// 释放内存，zgc，2007-03-07
    return TRUE;
}

/*=============================================================================
函 数 名： GetVmpAttachInfo
功    能： 取画面合成附属信息
算法实现： 
全局变量： 
参    数： TVmpAttachCfg* ptVmpAttachCfg
返 回 值： u16 
=============================================================================*/
u16 CCfgAgent::ReadVmpAttachTable(u8* pbyVmpProjectNum, TVmpAttachCfg* ptVmpAttachTable)
{
    if(NULL == pbyVmpProjectNum || NULL == ptVmpAttachTable )
    {
        Agtlog(LOG_ERROR, "[ReadVmpAttachTable] param error: pNum.0x%x, pTable.0x%x \n", 
			pbyVmpProjectNum, ptVmpAttachTable );
        return ERR_AGENT_NULLPARAM;
    }
    *pbyVmpProjectNum = (u8)m_dwVmpAttachNum;
    
    u16 wRet = SUCCESS_AGENT;
    for(u8 byIndex = 0; byIndex < m_dwVmpAttachNum; byIndex++)
    {
        if( NULL != &ptVmpAttachTable[byIndex] )
        {
            memcpy( &ptVmpAttachTable[byIndex], &m_atVmpAttachCfg[byIndex], sizeof(TVmpAttachCfg) );
        }
        else
        {
            wRet = (u16)ERR_AGENT_NULLPARAM;
        }
    }
    return wRet;
}

/*=============================================================================
函 数 名： WriteVmpAttachTable
功    能： 设置画面合成附属信息
算法实现： 
全局变量： 
参    数： TVmpAttachCfg& tVmpAttachCfg
返 回 值： u16 
=============================================================================*/
u16 CCfgAgent::WriteVmpAttachTable(u8 byVmpSchemeNum, TVmpAttachCfg* ptVmpAttachTable)
{
    if(NULL == ptVmpAttachTable && 0 != byVmpSchemeNum )
    {
        Agtlog( LOG_ERROR, "[WriteVmpAttachTable] param err: ptTable.0x%x !\n", ptVmpAttachTable );
        return ERR_AGENT_SETNODEVALUE;
    }
    m_dwVmpAttachNum = byVmpSchemeNum;
	
    s8   achEntryStr[32];
    s8   achVmpStyleCfgInfo[512];
	
    u16 wRet = SUCCESS_AGENT;
    wRet = WriteTableEntryNum(m_achCfgName, (s8*)SECTION_mcuVmpAttachTable, 
		(s8*)ENTRY_NUM, byVmpSchemeNum );
    if( SUCCESS_AGENT != wRet )
    {
        printf("[WriteVmpAttachTable] Fail to write profile in %s\n", SECTION_mcuVmpAttachTable);
        return wRet;
    }    
	
    for( u8 byIndex = 0; byIndex < byVmpSchemeNum; byIndex++ )
    {
        if(ptVmpAttachTable /*&& NULL != &ptVmpAttachTable[byIndex]*/)
        {
            memcpy( &m_atVmpAttachCfg[byIndex], &ptVmpAttachTable[byIndex], sizeof(TVmpAttachCfg) );
			
            memset(achVmpStyleCfgInfo, '\0', sizeof(achVmpStyleCfgInfo));
            memset(achEntryStr, '\0', sizeof(achEntryStr));
            
            GetEntryString(achEntryStr, byIndex);
            sprintf(achVmpStyleCfgInfo, "  %d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s", 
				ptVmpAttachTable[byIndex].GetIndex(),
				ptVmpAttachTable[byIndex].GetBGDColor(),
				ptVmpAttachTable[byIndex].GetFrameColor(),
				ptVmpAttachTable[byIndex].GetSpeakerFrameColor(),
				ptVmpAttachTable[byIndex].GetChairFrameColor(),
				ptVmpAttachTable[byIndex].GetFontType(),
				ptVmpAttachTable[byIndex].GetFontSize(),
				ptVmpAttachTable[byIndex].GetAlignType(),
				ptVmpAttachTable[byIndex].GetTextColor(), 
				ptVmpAttachTable[byIndex].GetTopicBGDColor(),
				ptVmpAttachTable[byIndex].GetDiaphaneity(),
				ptVmpAttachTable[byIndex].GetAlias() );
            
            wRet = WriteTableEntry( m_achCfgName, (s8*)SECTION_mcuVmpAttachTable, 
				achEntryStr, achVmpStyleCfgInfo );
            if( SUCCESS_AGENT != wRet )
            {
                printf("[WriteVmpAttachTable] Fail to write the configure file\n");
                break;
            }
        }
    }    
    return wRet;
}

// 多国语言 [pengguofeng 5/10/2013]
u16 CCfgAgent::ReadMcuEncodingType(u8 &byEncoding)
{
	s32 nEncoding = 0;
	// 判断返回值 [pengguofeng 5/20/2013]
	if ( GetRegKeyInt(m_achCfgName, SECTION_McuEncode, KEY_encoding, 0, &nEncoding) )
	{
		byEncoding = (u8)nEncoding;	
		return SUCCESS_AGENT;
	}
	else
	{
		return ERR_AGENT_GETNODEVALUE;
	}
}

BOOL32 CCfgAgent::WriteMcuEncodingType(const u8 *pbyEncoding)
{
	if ( !pbyEncoding)
	{
		return FALSE;
	}
	s32 nEncoding = s32(*pbyEncoding);
	
	u16 wRet = SetRegKeyInt(m_achCfgName, SECTION_McuEncode, KEY_encoding, nEncoding);
	
	return (wRet == SUCCESS_AGENT? TRUE: FALSE);
}
// end [pengguofeng 5/10/2013]

/*=============================================================================
函 数 名： GetEntryString
功    能： 得到Entry字符串
算法实现： 
全局变量： 
参    数： s8* achStr
u8 byIdx
返 回 值： void 
=============================================================================*/
void CCfgAgent::GetEntryString(s8* achStr, u8 byIdx)
{
    if( NULL != achStr )
    {
        sprintf(achStr, "%s%d", ENTRY_KEY, byIdx);
    }
    return;
}

/*=============================================================================
函 数 名： GetIpFromU32
功    能： 由U32得到Ip字符串
算法实现： 
全局变量： 
参    数： s8* achDstStr
u32 dwIpAddr: 网络序
返 回 值： void 
=============================================================================*/
void CCfgAgent::GetIpFromU32(s8* achDstStr, u32 dwIpAddr)
{
    if( NULL != achDstStr )    
    {
        u32 dwTmpIp = ntohl(dwIpAddr);
        if( 0 != dwTmpIp )
        {
            sprintf(achDstStr, "%d.%d.%d.%d%c", (dwTmpIp>>24)&0xFF, (dwTmpIp>>16)&0xFF, (dwTmpIp>>8)&0xFF, dwTmpIp&0xFF, 0);
        }
        else
        {
            memcpy(achDstStr, "0", sizeof("0"));
        }
    }    
    return;
}

/*=============================================================================
函 数 名： ChangeBrdRunEqpNum
功    能： 修改单板上运行外设数目
算法实现： 
全局变量： 
参    数： u8 byBrdId：单板号
u8 byEqpId：外设号
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::ChangeBrdRunEqpNum(u8 byBrdIdx, u8 byEqpId)
{
    u8 byIndex = 0;
    BOOL32 bRet = FALSE;
	
    for( ; byIndex < m_dwBrdCfgEntryNum; byIndex++ )
    {
        if(m_atBrdCfgTable[byIndex].GetBrdId() == byBrdIdx)
        {
            bRet = m_atBrdCfgTable[byIndex].SetPeriId( byEqpId );
            break;
        }
    }
    if( !bRet )
    {
        printf("[ChangeBrdRunEqpNum] The board ID is not exist or the eqp num is full !\n");
    }
    return bRet;
}

/*=============================================================================
函 数 名： GetLocalIp
功    能： 得到Mpc板Ip
算法实现： 
全局变量： 
参    数： 
返 回 值： u32  
=============================================================================*/
u32  CCfgAgent::GetLocalIp()
{
/*    
#if defined(WIN32)
u32 dwIpAddr;

  // 从单板列表得到
  dwIpAddr = GetBrdIpAddr( MCU_BOARD_MPC );
  // 得不到或得到的不是本机IP,使用本机列表中的第一个IP地址
  if( OspIsLocalHost( dwIpAddr ) )
  {
		return dwIpAddr;
		}
		else
		{
		// 2005-10-26 modify
		dwIpAddr = GetBrdIpAddr(MCU_BOARD_MPCD);
        if (OspIsLocalHost(dwIpAddr))
		{
		return dwIpAddr;
		}
		Agtlog(LOG_ERROR, "[GetMpcIpAddr] The Mpc's Ip is not the local Ip\n");
		
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
		  
			return 0;
			
			  #else
	*/
	//[chendaiwei 2010/10/15] OspIsLocalHost接口没有正常工作
	if(htonl(m_tMPCInfo.GetLocalIp()) != 0)
		//if( OspIsLocalHost( htonl(m_tMPCInfo.GetLocalIp()) ) )
	{
		return htonl(m_tMPCInfo.GetLocalIp());
	}
	
	// 直接读接口的IP地址
	TBrdEthParam tEtherPara = {0};
	
	// 默认使用第0个IP地址 (8000b)
	if ( ERROR == AgtGetBrdEthParam( m_tMPCInfo.GetLocalPortChoice(), &tEtherPara ) )
	{
		return 0;
	}

	return  tEtherPara.dwIpAdrs;  // 网络序	
								  /*    
								  #endif
	*/
}

/*=============================================================================
函 数 名： GetBrdIpAddr
功    能： 由单板ID得到指定单板IP
算法实现： 
全局变量： 
参    数： u8 byBrdID
返 回 值： u32 IP(网络序)
=============================================================================*/
u32 CCfgAgent::GetBrdIpAddr(u8 byBrdPosId)
{
	u32 dwIpAddr;
	u8 byLayer = 0;
    u8 bySlot = 0;
	
    // 示例： ID: 14: 第一层, 第四槽
    // 2005-10-26 modify
    if (MCU_BOARD_MPC != byBrdPosId && MCU_BOARD_MPCD != byBrdPosId )
	{
		byLayer = byBrdPosId >> 4;
		bySlot = (byBrdPosId & 0x0f);
	}
	
	u32 dwLoop = 0;
	for( ; dwLoop < m_dwBrdCfgEntryNum; dwLoop++ )
	{
		// MPC
		//  [1/21/2011 chendaiwei]支持CRI2，MPC2
		if( BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ == m_atBrdCfgTable[dwLoop].GetType() || BRD_TYPE_MPC2 == m_atBrdCfgTable[dwLoop].GetType() )
		{
			// 2005-10-26 modify
			if ((MCU_BOARD_MPC == byBrdPosId && 0 == m_atBrdCfgTable[dwLoop].GetSlot()) ||
                (MCU_BOARD_MPCD == byBrdPosId && 1 == m_atBrdCfgTable[dwLoop].GetSlot()) )
			{
				dwIpAddr = htonl( m_atBrdCfgTable[dwLoop].GetBrdIp() );
				return dwIpAddr;
			}
			continue;
		}
		// EX板
        else
		{
			if( m_atBrdCfgTable[dwLoop].GetSlot() == (bySlot - 1) && 
				m_atBrdCfgTable[dwLoop].GetLayer() == byLayer )
			{
				dwIpAddr = htonl( m_atBrdCfgTable[dwLoop].GetBrdIp() );
				return dwIpAddr;
			}
		}
	}
	if( dwLoop >= m_dwBrdCfgEntryNum )
    {
        Agtlog(LOG_WARN, "[GetBrdIpAddr] The board Id.%d is not exist while getting its IP.\n", byBrdPosId);
    }
	return 0;
}

/*=============================================================================
函 数 名： GetBrdIpByIndex
功    能： 
算法实现： 
全局变量： 
参    数： u8 byBrdIndex
返 回 值： u32 (主机序)
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/22  4.0			周广程                  创建
=============================================================================*/
u32 CCfgAgent::GetBrdIpByAgtId( u8 byAgtId )
{
/*
u32 dwBrdIp = 0;
if( byBrdIndex > m_dwBrdCfgEntryNum || 0 == byBrdIndex )
{
Agtlog( LOG_ERROR, "The board index is error, return 0!\n");
dwBrdIp = 0;
}
dwBrdIp = m_atBrdCfgTable[byBrdIndex-1].GetBrdIp();

  return dwBrdIp;
	*/
	u32 dwIpAddr = 0;
	u32 dwLoop = 0;
	for( ; dwLoop < m_dwBrdCfgEntryNum; dwLoop++ )
	{
		if (m_atBrdCfgTable[dwLoop].GetBrdId() == byAgtId)
		{
			dwIpAddr = m_atBrdCfgTable[dwLoop].GetBrdIp();
			break;
		}
	}
	return dwIpAddr;
}


/*=============================================================================
函 数 名： GetCriDriIsCast
功    能： 取Cri、Dri板是否组播
算法实现： 
全局变量： 
参    数： u8 byBrdId
返 回 值： －1：错误的单板ID， 0：不组播，1： 组播 
=============================================================================*/
u8 CCfgAgent::GetCriDriIsCast( u8 byBrdId )
{
    u8 byResult = 0;
    u8 byLayer = 0;  // mpc 板不一定在0层
    u8 bySlot  = 0;
    
    // 2005-10-26 modify
    if (MCU_BOARD_MPC != byBrdId && MCU_BOARD_MPCD != byBrdId )
    {
        byLayer = byBrdId >> 4;
        bySlot  = (byBrdId & 0x0f) - 1;
    }
    else
    {
        // 如果是MPC板
        if(MCU_BOARD_MPCD == byBrdId)
        {
            byLayer = m_tMPCInfo.GetLocalLayer();
            bySlot = 1;
        }
        else
        {
            byLayer = m_tMPCInfo.GetLocalLayer();
            bySlot = 0;
        }
    }
    
    u8 byIndex = 0;
    for( ; byIndex < m_dwBrdCfgEntryNum; byIndex++ )
    {
        if( byLayer == m_atBrdCfgTable[byIndex].GetLayer() && 
            bySlot  == m_atBrdCfgTable[byIndex].GetSlot() )
        {
            byResult = m_atBrdCfgTable[byIndex].GetCastChoice();
            break;
        }
    }
    
    if(byIndex >= m_dwBrdCfgEntryNum)
    {
        Agtlog(LOG_ERROR, "[GetCriDriIsCast] The board ID is not exist while checking cast.\n");
        byResult = 0;
    }
    return byResult;
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
BOOL32 CCfgAgent::RegSetIpAdr(s8* lpszAdapterName, s8* pIpAddr, u16 wLen)
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
BOOL32 CCfgAgent::RegSetNetMask(s8* lpszAdapterName, s8* pNetMask, u16 wLen)
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
BOOL32 CCfgAgent::RegSetNetGate(s8* lpszAdapterName, s8* pNetGate, u16 wLen)
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
函 数 名： RegGetAdpName
功    能： 
算法实现： 
全局变量： 
参    数： s8* lpszAdpName
u8 byLen
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::RegGetAdpName(s8* lpszAdpName, u16 wLen)
{
    if(NULL == lpszAdpName)
    {
        return FALSE;
    }

	Agtlog(LOG_LVL_DETAIL,"[RegGetAdpName] AdpName.%s Len.%d\n",lpszAdpName,wLen);

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
u8 byLen
返 回 值： BOOL32 
=============================================================================*/
// BOOL32 CCfgAgent::RegGetIpAdr(s8* lpszAdapterName, s8* pIpAddr, u16 wLen)
// {
//     return TRUE;
// }

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
BOOL32 CCfgAgent::RegGetNetMask(s8* lpszAdapterName, s8* pNetMask, u16 wLen)
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
	
    s8 aszNetMask[100];
    memset(aszNetMask, '\0', sizeof(aszNetMask));
    int nMaskLen = sizeof(aszNetMask);
	
    u32 dwType = REG_MULTI_SZ;
    RegQueryValueEx(hKey, "SubnetMask", 0, &dwType, (unsigned char*)aszNetMask, (unsigned long*)&nMaskLen);
    if(wLen < nMaskLen)
    {
        nMaskLen = wLen;// 长度保护
    }
    strncpy(pNetMask, aszNetMask, nMaskLen);
    
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
BOOL32 CCfgAgent::RegGetNetGate(s8* lpszAdapterName, s8* pNetGate, u16 wLen)
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
    strncpy(pNetGate, aszNetGate, wGateLen);
	
    RegCloseKey(hKey);
	
#endif
    
    return TRUE;
}

/*=============================================================================
函 数 名： GetMpcMaskIp
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： u32  
=============================================================================*/
u32  CCfgAgent::GetMpcMaskIp()   // 子网掩码
{
    u32 dwMaskIp = 0;
#ifndef WIN32
    TBrdEthParam tBrdEthParam;
    u8 byEthId = m_tMPCInfo.GetLocalPortChoice(); // 前后网口选择
    u8 byRet = AgtGetBrdEthParam(byEthId, &tBrdEthParam);
    dwMaskIp = tBrdEthParam.dwIpMask; // mask ip
	dwMaskIp = ntohl(dwMaskIp);
#else // win32
    s8 aszAdpName[255];
    s8 aszNetMaskIp[64];
    memset(aszNetMaskIp, 0, sizeof(aszNetMaskIp));
    memset(aszAdpName, 0, sizeof(aszAdpName));
	
    u8 byLen = (u8)sizeof(aszAdpName);
    BOOL32 bRet = RegGetAdpName(aszAdpName, byLen);
    if(bRet)
    {
        bRet = RegGetNetMask(aszAdpName, aszNetMaskIp, sizeof(aszNetMaskIp));
        if(bRet)
        {
            dwMaskIp = INET_ADDR(aszNetMaskIp); // 网络序
        }
        else
        {
            dwMaskIp = 0;
        }
    }
    else
    {
        Agtlog(LOG_ERROR, "[GetMpcMaskIp] Get adp name fail.\n");
    }
    
#endif
    return dwMaskIp;
}

/*=============================================================================
函 数 名： SetMpcMaskIp
功    能： 
算法实现： 
全局变量： 
参    数： u32 dwIp
返 回 值： u8  
=============================================================================*/
u16   CCfgAgent::SetMpcMaskIp(u32 dwIp, u8 byInterface)
{   
    u16 wRet = SUCCESS_AGENT;
    dwIp = ntohl(dwIp);
	m_tMPCInfo.SetLocalPortChoice( byInterface );
	
#ifndef WIN32
    TBrdEthParam tBrdEthParam;
    memset(&tBrdEthParam, 0, sizeof(TBrdEthParam));
	
    Agtlog(LOG_INFORM, "[GetMpcMaskIp] The Mask Ip you want to set: %0x\n", dwIp);
	
    u8 byEthId = m_tMPCInfo.GetLocalPortChoice();          // 前后网口选择
    u8 byRet = AgtGetBrdEthParam(byEthId, &tBrdEthParam);     // 先取原有信息
    Agtlog(LOG_INFORM, "[GetMpcMaskIp] Src Ip: %0x, Mask: %0x, %d\n", tBrdEthParam.dwIpAdrs, tBrdEthParam.dwIpMask, byRet);   
    
	tBrdEthParam.dwIpMask = dwIp;               // 更改掩码地址
    byRet = BrdSetEthParam(byEthId, Brd_SET_IP_AND_MASK, &tBrdEthParam); 
    Agtlog(LOG_INFORM, "[GetMpcMaskIp] Dst Ip: %0x, Mask: %0x, byEthId= %d, return value: %d\n", 
		tBrdEthParam.dwIpAdrs, tBrdEthParam.dwIpMask, byEthId, byRet);
	
    byRet = BrdSaveNipConfig(); // 保存Ip
	
    Agtlog(LOG_INFORM, "[SetMpcMaskIp] Save the Mask Ip, return value: %d\n",byRet);
	
#else // win32
    s8 aszAdpName[255];
    s8 aszNetMaskIp[64];
    memset(aszNetMaskIp, 0, sizeof(aszNetMaskIp));
    memset(aszAdpName, 0, sizeof(aszAdpName));
    GetIpFromU32(aszNetMaskIp, htonl(dwIp)); // 取Ip地址字符串
	
    u8 byLen = (u8)sizeof(aszAdpName);
    BOOL32 bRet = RegGetAdpName(aszAdpName, byLen);
    if( bRet )
    {
        bRet = RegSetNetMask(aszAdpName, aszNetMaskIp, sizeof(aszNetMaskIp));
    }
    else
    {
        Agtlog(LOG_ERROR, "[GetMpcMaskIp] Get adp name fail.\n");
    }
#endif
    return wRet;
}

/*=============================================================================
函 数 名： SetLocalIp
功    能： 
算法实现： 
全局变量： 
参    数： u32 dwIp
返 回 值： u8  
=============================================================================*/
u16   CCfgAgent::SetLocalIp(u32 dwIp, u8 byInterface)
{
    dwIp = ntohl(dwIp);
    u16 wRet = SUCCESS_AGENT;
	m_tMPCInfo.SetLocalPortChoice( byInterface );
#ifndef WIN32
    TBrdEthParam tBrdEthParam;
    memset(&tBrdEthParam, 0, sizeof(TBrdEthParam));
    
    Agtlog(LOG_INFORM, "[SetLocalIp] The Mpc Ip you want to set: %0x\n", dwIp);
    
    u8 byEthId = m_tMPCInfo.GetLocalPortChoice();           // 前后网口选择
    u8 byRet = AgtGetBrdEthParam(byEthId, &tBrdEthParam);      // 先取原有信息
    Agtlog(LOG_INFORM, "[SetLocalIp] Src Get Mpc Ip: %0x, byEthId= %d, return value: %d\n", tBrdEthParam.dwIpAdrs, byEthId, byRet);
    
    tBrdEthParam.dwIpAdrs = dwIp;                           // 更改Ip地址
    byRet |= BrdSetEthParam(byEthId, Brd_SET_IP_AND_MASK, &tBrdEthParam);
    Agtlog(LOG_INFORM, "[SetLocalIp] Dst Get Mpc Ip: %0x, return value: %d\n", tBrdEthParam.dwIpAdrs, byRet);
	
    byRet |= BrdSaveNipConfig();                             // 保存Ip
    Agtlog(LOG_INFORM, "[SetLocalIp] Save the ip, return value: %d\n",byRet);
	
#else //  WIN32
    s8 aszAdpName[255];
    s8 aszMpcIp[64];
    memset(aszMpcIp, 0, sizeof(aszMpcIp));
    memset(aszAdpName, 0, sizeof(aszAdpName));
    GetIpFromU32(aszMpcIp, htonl(dwIp));// 取Ip地址字符串
	
    u8 byLen = (u8)sizeof(aszAdpName);
    BOOL32 bRet = RegGetAdpName(aszAdpName, byLen);
    if( bRet )
    {
        bRet = RegSetIpAdr(aszAdpName, aszMpcIp, sizeof(aszMpcIp));
    }
    else
    {
        Agtlog(LOG_INFORM, "[SetLocalIp] Get adp name fail.\n");
    }
#endif
    return wRet;
}


u8 CCfgAgent::GetMac(s8 *pszBuffer)
{
	if(pszBuffer == NULL)
	{
		return 0;
	}
	
#ifndef WIN32
    TBrdEthParam tBrdEthParam;
    memset(&tBrdEthParam, 0, sizeof(TBrdEthParam));
    
    u8 byEthId = m_tMPCInfo.GetLocalPortChoice();           // 前后网口选择
    u8 byRet = AgtGetBrdEthParam(byEthId, &tBrdEthParam);      // 先取原有信息
	
	memcpy(pszBuffer, tBrdEthParam.byMacAdrs, sizeof(tBrdEthParam.byMacAdrs));
    Agtlog(LOG_INFORM, "[GetMac] Src Get Mpc Mac: %s, byEthId= %d, return value: %d\n", tBrdEthParam.byMacAdrs, byEthId, byRet);
	
	return sizeof(tBrdEthParam.byMacAdrs);
#else //  WIN32
	return 0;
#endif
}


/*=============================================================================
函 数 名： SetMpcActive
功    能： 
算法实现： 
全局变量： 
参    数： BOOL32 bEnabled
返 回 值： void 
=============================================================================*/
void CCfgAgent::SetMpcActive(BOOL32 bEnabled)
{
    m_tMPCInfo.SetIsLocalMaster(bEnabled);
    return;
}

/*=============================================================================
函 数 名： IsMpcActive
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::IsMpcActive()
{
    return m_tMPCInfo.GetIsLocalMaster();
}

/*=============================================================================
函 数 名： GetMpcGateway
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： u32  
=============================================================================*/
u32  CCfgAgent::GetMpcGateway()  // 网关Ip
{
    u32 dwGateWayIp = 0;
#ifndef WIN32
	u8 byEthId = m_tMPCInfo.GetLocalPortChoice(); // 前后网口选择
	dwGateWayIp = AgtGetMpcGateway( byEthId );
	dwGateWayIp = ntohl(dwGateWayIp);
#else //  WIN32
    s8 aszAdpName[255];
    s8 aszNetGateIp[64];
    memset(aszNetGateIp, 0, sizeof(aszNetGateIp));
    memset(aszAdpName, 0, sizeof(aszAdpName));
	
    u8 byLen = (u8)sizeof(aszAdpName);
    BOOL32 bRet = RegGetAdpName(aszAdpName, byLen);
    if(bRet)
    {
        bRet = RegGetNetGate(aszAdpName, aszNetGateIp, sizeof(aszNetGateIp));
        if(bRet)
        {
            dwGateWayIp = ntohl(INET_ADDR(aszNetGateIp)); // 主机序
        }
        else
        {
            dwGateWayIp = 0;
        }
    }
    else
    {
        Agtlog(LOG_ERROR, "[GetMpcGateway] Get adp name fail.\n");
    }
#endif
    return dwGateWayIp;
}

/*=============================================================================
函 数 名： SetMpcGateway
功    能： 
算法实现： 
全局变量： 
参    数： u32 dwIp
返 回 值： u8  
=============================================================================*/
u16   CCfgAgent::SetMpcGateway(u32 dwIp)
{
    u16 wRet = SUCCESS_AGENT;
    dwIp = ntohl(dwIp);
#ifndef WIN32
    u8 byRet = BrdSetDefGateway(dwIp);
    Agtlog(LOG_INFORM, "[SetMpcGateway] Set GateWay:%0x ,return value: %d\n", dwIp, byRet);
    byRet = BrdSaveNipConfig(); // 保存Ip
    Agtlog(LOG_INFORM, "[SetMpcGateway] Save the GateWay, return value: %d\n", byRet);
	
#else //  WIN32
	
    s8 aszAdpName[255];
    s8 aszGWIp[64];
    memset(aszGWIp, 0, sizeof(aszGWIp));
    memset(aszAdpName, 0, sizeof(aszAdpName));
    GetIpFromU32(aszGWIp, htonl(dwIp));// 取Ip地址字符串
	
    u8 byLen = (u8)sizeof(aszAdpName);
    BOOL32 bRet = RegGetAdpName(aszAdpName, byLen);
    if( bRet )
    {
        bRet = RegSetNetGate(aszAdpName, aszGWIp, sizeof(aszGWIp));
    }
    else
    {
        Agtlog(LOG_ERROR, "[SetMpcGateway] Get adp name fail.\n");
    }
#endif
    return wRet;
}

/*=============================================================================
函 数 名： GetMpcHWVersion
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： u8  
=============================================================================*/
/*lint -save -e527*/
u8   CCfgAgent::GetMpcHWVersion() // 硬件版本号查询
{
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	return VERSION_CONFIGURE60;
#endif
	
    u8 byVersion = 0;
#ifndef WIN32
    byVersion = BrdQueryHWVersion();
    if (byVersion == 0)
		// 保护    
    {
        byVersion = VERSION_CONFIGURE40;
    }
	
#else // WIN32
    byVersion = VERSION_CONFIGURE40; // version 
#endif
	
    return byVersion;
}
/*lint -restore*/

/*=============================================================================
函 数 名： SetEqpRecCfgById
功    能： 据外设ID设录像机信息
算法实现： 
全局变量： 
参    数：  u8 byId
TmcueqpRecorderEntry * ptRecCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::SetEqpRecCfgById( u8 byId, TEqpRecInfo* ptRecCfg )
{
    u16 wRet = ERR_AGENT_SETNODEVALUE;
	
    if( NULL == ptRecCfg || byId < RECORDERID_MIN || byId > RECORDERID_MAX )
    {
        Agtlog(LOG_ERROR, "[SetEqpRecorderCfg] param err: byId: %d, ptBasCfg: 0x%x !\n", byId, ptRecCfg);
        return ERR_AGENT_SETNODEVALUE;
    }
	
    for(u8 byRow = 1; byRow <= m_dwRecEntryNum; byRow++)
    {
        if( byId == m_atRecTable[byRow-1].GetEqpId() )
        {
            wRet = SetEqpRecCfgByRow( byRow, ptRecCfg );
            break;
        }
    }
    if( SUCCESS_AGENT != wRet )
        Agtlog(LOG_ERROR, "[SetEqpRecCfgById] set rec<%d> config failed !\n", byId);
	
    return wRet;
}

/*=============================================================================
函 数 名： SetEqpRecCfgById
功    能： 据配置行号设录像机信息
算法实现： 
全局变量： 
参    数： u8 byRow
		   TEqpRecInfo * ptRecCfg
		   BOOL32 bIsNeedUpdatePortInTable 标识是否需要更新内存表中端口
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::SetEqpRecCfgByRow( u8 byRow, TEqpRecInfo * ptRecCfg, BOOL32 bIsNeedUpdatePortInTable )
{
    BOOL32 bFlag = FALSE;
    u16 wRet = SUCCESS_AGENT;
    
    if ( CheckEqpRow( EQP_TYPE_RECORDER, byRow ) )
    {
		m_atRecTable[byRow-1].SetEqpId( ptRecCfg->GetEqpId() );
		//如果是通过界面重新分配端口信息，把重新分配后的端口信息写入文件，但不记录在内存中。以防止用户不重启导致已工作
		//外设端口变化，无法正常工作[12/15/2011 chendaiwei]
		if(bIsNeedUpdatePortInTable)
		{
			m_atRecTable[byRow-1].SetMcuRecvPort( ptRecCfg->GetMcuRecvPort() );
		}
        m_atRecTable[byRow-1].SetSwitchBrdId( ptRecCfg->GetSwitchBrdId() );
        m_atRecTable[byRow-1].SetIpAddr( ptRecCfg->GetIpAddr() );
		
        m_atRecTable[byRow-1].SetEqpRecvPort( ptRecCfg->GetEqpRecvPort() );
        m_atRecTable[byRow-1].SetAlias( ptRecCfg->GetAlias() );
		
        // 为便于以后扩展功能，在初次读配置文件时分配置空间时应该预留部分空间。
        // 该记录必须存在 ? ?
		
        s8   achEntryStr[32];
        s8   achRecInfo[512];
        s8   achRecIpAddr[32];
        memset(achEntryStr, '\0', sizeof(achEntryStr));
        memset(achRecInfo, '\0', sizeof(achRecInfo));
        memset(achRecIpAddr, '\0', sizeof(achRecIpAddr));
		
        GetEntryString(achEntryStr, byRow-1);
        GetIpFromU32(achRecIpAddr, htonl(m_atRecTable[byRow-1].GetIpAddr()));
        
        // question
        sprintf(achRecInfo, "   %d", m_atRecTable[byRow-1].GetEqpId());   // ID 前4个空格
        sprintf(achRecInfo, "%s\t%d", achRecInfo, /*m_atRecTable[byRow-1].GetMcuRecvPort()*/ptRecCfg->GetMcuRecvPort());
        sprintf(achRecInfo, "%s\t%d", achRecInfo, m_atRecTable[byRow-1].GetSwitchBrdId());
        sprintf(achRecInfo, "%s\t%s", achRecInfo, m_atRecTable[byRow-1].GetAlias());
        sprintf(achRecInfo, "%s\t%s", achRecInfo, achRecIpAddr);  // ip address 
        sprintf(achRecInfo, "%s\t%d", achRecInfo, m_atRecTable[byRow-1].GetEqpRecvPort());
        
        wRet = WriteTableEntry( m_achCfgName, SECTION_mcueqpRecorderTable, 
			achEntryStr, achRecInfo );
		if ( SUCCESS_AGENT != wRet )
		{
			printf("[SetEqpRecCfgByRow] Fail to write the configure file\n");
		}
    }
    else
        wRet = ERR_AGENT_SETNODEVALUE;
	
    return wRet;
}

/*=============================================================================
函 数 名： SetEqpVrsRecCfgByRow
功    能： 据配置行号设Vrs新录播信息
算法实现： 
全局变量： 
参    数： u8 byRow
		   TEqpVrsRecCfgInfo * ptVrsRecCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2013/10/09  4.7         杨怀志       创建
=============================================================================*/
u16 CCfgAgent::SetEqpVrsRecCfgByRow( u8 byRow, TEqpVrsRecCfgInfo * ptVrsRecCfg )
{
    BOOL32 bFlag = FALSE;
    u16 wRet = SUCCESS_AGENT;
    
    if (byRow != 0 && byRow <= m_dwVrsRecEntryNum)
    {
		TMtAlias tVrsCallAlias = ptVrsRecCfg->GetVrsCallAlias();
		m_atVrsRecTable[byRow-1].SetVrsId(ptVrsRecCfg->GetVrsId());
		m_atVrsRecTable[byRow-1].SetVrsAlias(ptVrsRecCfg->GetVrsAlias());
		m_atVrsRecTable[byRow-1].SetVrsCallAlias(tVrsCallAlias);
        // 为便于以后扩展功能，在初次读配置文件时分配置空间时应该预留部分空间。
        // 该记录必须存在 ? ?
		
        s8   achEntryStr[32];
        s8   achRecInfo[512];
        s8   achRecIpAddr[32];
        memset(achEntryStr, '\0', sizeof(achEntryStr));
        memset(achRecInfo, '\0', sizeof(achRecInfo));
        memset(achRecIpAddr, '\0', sizeof(achRecIpAddr));
		
        GetEntryString(achEntryStr, byRow-1);
        GetIpFromU32(achRecIpAddr, htonl(tVrsCallAlias.m_tTransportAddr.GetIpAddr()));
        
        sprintf(achRecInfo, "%d", m_atVrsRecTable[byRow-1].GetVrsId());		// ID 前4个空格
        sprintf(achRecInfo, "%s\t%s", achRecInfo, m_atVrsRecTable[byRow-1].GetVrsAlias());
        sprintf(achRecInfo, "%s\t%d", achRecInfo, tVrsCallAlias.m_AliasType);
		if (mtAliasTypeTransportAddress == tVrsCallAlias.m_AliasType)
		{
			sprintf(achRecInfo, "%s\t%s", achRecInfo, achRecIpAddr);  // ip address 
		}
		else if (mtAliasTypeE164 == tVrsCallAlias.m_AliasType)
		{
			sprintf(achRecInfo, "%s\t%s", achRecInfo, tVrsCallAlias.m_achAlias);
		}
		else
		{
			Agtlog(LOG_ERROR, "[SetEqpRecCfgByRow] VrsCallAliasType.%d is not ip address or e164.\n", tVrsCallAlias.m_AliasType);
			return ERR_AGENT_SETNODEVALUE;
		}
        
        wRet = WriteTableEntry( m_achCfgName, SECTION_mcueqpVrsRecorderTable, 
			achEntryStr, achRecInfo );
		if ( SUCCESS_AGENT != wRet )
		{
			printf("[SetEqpRecCfgByRow] Fail to write the configure file\n");
		}
    }
    else
        wRet = ERR_AGENT_SETNODEVALUE;
	
    return wRet;
}

/*=============================================================================
函 数 名： SetEqpTVWallCfg
功    能： 据外设ID设电视墙信息
算法实现： 
全局变量： 
参    数： u8 byId
TEqpTVWallEntry * ptTWCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::SetEqpTVWallCfgById( u8 byId, TEqpTVWallInfo * ptTWCfg )
{
    u16 wRet = ERR_AGENT_SETNODEVALUE;
	
    if( NULL == ptTWCfg || byId < TVWALLID_MIN || byId > TVWALLID_MAX )
    {
        Agtlog(LOG_ERROR, "[SetEqpTVWallCfg] param err: byId: %d, ptCfg: 0x%x !\n", byId, ptTWCfg);
        return wRet;
    }
	
    for(u8 byRow = 1; byRow <= m_dwTVWallEntryNum; byRow++)
    {
        if( byId == m_atTVWallTable[byRow-1].GetEqpId() )
        {
            wRet = SetEqpTVWallCfgByRow( byRow, ptTWCfg );
            break;
        }
    }
    if( SUCCESS_AGENT != wRet )
        Agtlog(LOG_ERROR, "[SetEqpTVWallCfgById] set tvwall<%d> config failed !\n", byId);
	
    return wRet;
}

/*=============================================================================
函 数 名： SetEqpTVWallCfgByRow
功    能： 据外设ID设电视墙信息
算法实现： 
全局变量： 
参    数： u8 byRow
TEqpTVWallEntry * ptTWCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::SetEqpTVWallCfgByRow( u8 byRow, TEqpTVWallInfo * ptTWCfg )
{
    BOOL32 bFlag = FALSE;
    u16 wRet = SUCCESS_AGENT;
	
    if ( CheckEqpRow( EQP_TYPE_TVWALL, byRow) )
    {
		m_atTVWallTable[byRow-1].SetEqpId( ptTWCfg->GetEqpId() );
        m_atTVWallTable[byRow-1].SetRunBrdId( ptTWCfg->GetRunBrdId() );
		
        // zbq [2007/03/02] 外设配置重新写入时需再次从单板表里获取该外设的最新Ip
        m_atTVWallTable[byRow-1].SetIpAddr( GetBrdIpByAgtId(ptTWCfg->GetRunBrdId()) );
		
        m_atTVWallTable[byRow-1].SetEqpRecvPort( ptTWCfg->GetEqpRecvPort() );
        m_atTVWallTable[byRow-1].SetAlias( ptTWCfg->GetAlias() );
        
        bFlag = ChangeBrdRunEqpNum( ptTWCfg->GetRunBrdId(), ptTWCfg->GetEqpId() );
        if ( !bFlag ) 
            wRet = ERR_AGENT_SETNODEVALUE;
		
        u8 byMapNum = 0;
        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        ptTWCfg->GetUsedMap( abyMapId, byMapNum );
        m_atTVWallTable[byRow-1].SetUsedMap( abyMapId, byMapNum );
        
        if( SUCCESS_AGENT == wRet )
        {
            s8   achEntryStr[32];
            s8   achUseMapStr[32];
            s8   achTvCfgInfo[512];
            memset(achEntryStr, '\0', sizeof(achEntryStr));
            memset(achUseMapStr, '\0', sizeof(achUseMapStr));
            memset(achTvCfgInfo, '\0', sizeof(achTvCfgInfo));
			
            GetEntryString( achEntryStr, byRow-1);
            m_atTVWallTable[byRow-1].GetUsedMapStr( achUseMapStr );
            sprintf( achTvCfgInfo, "%d\t%s\t%d\t%d\t%s", 
				m_atTVWallTable[byRow-1].GetEqpId(),
				m_atTVWallTable[byRow-1].GetAlias(),
				m_atTVWallTable[byRow-1].GetRunBrdId(),
				m_atTVWallTable[byRow-1].GetEqpRecvPort(),
				achUseMapStr);
            
            wRet = WriteTableEntry( m_achCfgName, SECTION_mcueqpTVWallTable, 
				achEntryStr, achTvCfgInfo );
            if ( SUCCESS_AGENT != wRet )
            {
                printf("[SetEqpTVWallCfg] Fail to write the configure file\n");
                wRet = ERR_AGENT_SETNODEVALUE;
            }
        }
    }
    else
        wRet = ERR_AGENT_SETNODEVALUE;
	
    return wRet;    
}

/*=============================================================================
函 数 名： SetEqpMixerCfgById
功    能： 据外设ID设混音器信息
算法实现： 
全局变量： 
参    数： u8 byId
TEqpMixerEntry * ptMixerCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::SetEqpMixerCfgById( u8 byId, TEqpMixerInfo tMixerCfg )
{
    u16  wRet  = ERR_AGENT_SETNODEVALUE;
	
    if( byId < MIXERID_MIN || byId > MIXERID_MAX)
    {
        Agtlog(LOG_ERROR, "[SetEqpMixerCfg] The Mix ID is not ligality\n");
        return wRet;
    }   
    
    for( u8 byRow = 1; byRow <= m_dwMixEntryNum; byRow++)
    {
        if( byId == m_atMixTable[byRow-1].GetEqpId() )
        {
            wRet = SetEqpMixerCfgByRow( byRow, tMixerCfg );
            break;
        }
    }
    if( SUCCESS_AGENT != wRet )
        Agtlog(LOG_ERROR, "[SetEqpMixerCfgById] set mixer<%d> config failed !\n", byId);
	
    return wRet;
}

/*=============================================================================
函 数 名： SetEqpMixerCfgByRow
功    能： 据配置行号设混音器信息
算法实现： 
全局变量： 
参    数： u8 byId
           TEqpMixerEntry * ptMixerCfg
		   BOOL32 bIsNeedUpdatePortInTable 标识是否需要更新内存表中端口
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::SetEqpMixerCfgByRow( u8 byRow, TEqpMixerInfo tMixerCfg,BOOL32 bIsNeedUpdatePortInTable )
{
    BOOL32 bFlag = FALSE;
    u16 wRet = SUCCESS_AGENT;
	
	//tianzhiyong 增加一种混音器外设类型
    if ( CheckEqpRow( EQP_TYPE_MIXER, byRow ) )
    {
        m_atMixTable[byRow-1].SetEqpId(tMixerCfg.GetEqpId());
        m_atMixTable[byRow-1].SetSwitchBrdId( tMixerCfg.GetSwitchBrdId() );

		//如果是通过界面重新分配端口信息，把重新分配后的端口信息写入文件，但不记录在内存中。以防止用户不重启导致已工作
		//外设端口变化，无法正常工作[12/15/2011 chendaiwei]
		if(bIsNeedUpdatePortInTable)
		{
			m_atMixTable[byRow-1].SetMcuRecvPort( tMixerCfg.GetMcuRecvPort() );
		}

        m_atMixTable[byRow-1].SetRunBrdId( tMixerCfg.GetRunBrdId() );
		
        // zbq [2007/03/02] 外设配置重新写入时需再次从单板表里获取该外设的最新Ip
        m_atMixTable[byRow-1].SetIpAddr( GetBrdIpByAgtId(tMixerCfg.GetRunBrdId()) );
		
        m_atMixTable[byRow-1].SetEqpRecvPort( tMixerCfg.GetEqpRecvPort() );
        m_atMixTable[byRow-1].SetMaxChnInGrp( tMixerCfg.GetMaxChnInGrp() );
        m_atMixTable[byRow-1].SetAlias( tMixerCfg.GetAlias() );
        
        bFlag = ChangeBrdRunEqpNum( tMixerCfg.GetRunBrdId(), tMixerCfg.GetEqpId() );
        if ( !bFlag ) 
            wRet = ERR_AGENT_SETNODEVALUE;
        
        u8 byMapNum = 0;
        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        tMixerCfg.GetUsedMap( abyMapId, byMapNum );
        m_atMixTable[byRow-1].SetUsedMap( abyMapId, byMapNum );
		
		m_atMixTable[byRow-1].SetIsSendRedundancy( tMixerCfg.IsSendRedundancy() );

        if ( SUCCESS_AGENT == wRet )
        {
            s8   achMixCfgInfo[512];
            s8   achEntryStr[32];
            s8   achUsedMapStr[32];
            memset(achEntryStr, '\0', sizeof(achEntryStr));
            memset(achUsedMapStr, '\0', sizeof(achUsedMapStr));
            memset(achMixCfgInfo, '\0', sizeof(achMixCfgInfo));
            
            GetEntryString(achEntryStr, byRow-1);
            m_atMixTable[byRow-1].GetUsedMapStr( achUsedMapStr );
            sprintf( achMixCfgInfo, "%d\t%d\t%d\t%s\t%d\t%d\t%d\t%s\t%d", 
				m_atMixTable[byRow-1].GetEqpId(),
				/*m_atMixTable[byRow-1].GetMcuRecvPort()*/tMixerCfg.GetMcuRecvPort(), 
				m_atMixTable[byRow-1].GetSwitchBrdId(),
				m_atMixTable[byRow-1].GetAlias(),
				m_atMixTable[byRow-1].GetRunBrdId(),
				m_atMixTable[byRow-1].GetEqpRecvPort(),
				m_atMixTable[byRow-1].GetMaxChnInGrp(),
				achUsedMapStr,
				m_atMixTable[byRow-1].IsSendRedundancy());

            wRet = WriteTableEntry( m_achCfgName, SECTION_mcueqpMixerTable, 
				achEntryStr, achMixCfgInfo );
            if ( SUCCESS_AGENT != wRet )
            {
                Agtlog(LOG_ERROR, "[SetEqpMixerCfgByRow] Fail to write the configure file\n");
                wRet = ERR_AGENT_SETNODEVALUE;
            }
        }
    }
    else
        wRet = ERR_AGENT_SETNODEVALUE;
    
    return wRet;
}

/*=============================================================================
函 数 名： SetEqpBasCfgById
功    能： 据外设ID设Bas信息
算法实现： 
全局变量： 
参    数： u8 byId
TEqpBasEntry * ptBasCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::SetEqpBasCfgById( u8 byId, TEqpBasInfo * ptBasCfg )
{
    u16 wRet = ERR_AGENT_SETNODEVALUE;
	
    if( NULL == ptBasCfg || byId < BASID_MIN || byId > BASID_MAX )
    {
        Agtlog(LOG_ERROR, "[SetEqpBasCfgById] param err: byId: %d, ptBasCfg: 0x%x !\n", byId, ptBasCfg);
        return wRet;
    }
    
    for( u8 byRow = 1; byRow <= m_dwBasEntryNum; byRow ++ )
    {
        if( byId == m_atBasTable[byRow-1].GetEqpId() )
        {
            wRet = SetEqpBasCfgByRow( byRow, ptBasCfg );
            break;
        }
    }
    if( SUCCESS_AGENT != wRet )
        Agtlog(LOG_ERROR, "[SetEqpBasCfgById] set bas<%d> config failed !\n", byId);
	
    return wRet;
}

/*=============================================================================
函 数 名： SetEqpBasCfgByRow
功    能： 据配置行号设BAS信息
算法实现： 
全局变量： 
参    数： u8 byId
TEqpBasInfo * ptBasCfg
		   BOOL32 bIsNeedUpdatePortInTable 标识是否需要更新内存表中端口
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::SetEqpBasCfgByRow( u8 byRow, TEqpBasInfo * ptBasCfg, BOOL32 bIsNeedUpdatePortInTable )
{
    BOOL32 bFlag = FALSE;
    u16 wRet = SUCCESS_AGENT;
    
    if ( CheckEqpRow( EQP_TYPE_BAS, byRow ) )
    {
        m_atBasTable[byRow-1].SetEqpId( ptBasCfg->GetEqpId() );

		//如果是通过界面重新分配端口信息，把重新分配后的端口信息写入文件，但不记录在内存中。以防止用户不重启导致已工作
		//外设端口变化，无法正常工作[12/15/2011 chendaiwei]
		if(bIsNeedUpdatePortInTable)
		{
			m_atBasTable[byRow-1].SetMcuRecvPort( ptBasCfg->GetMcuRecvPort() );
		}

        m_atBasTable[byRow-1].SetSwitchBrdId( ptBasCfg->GetSwitchBrdId() );
        m_atBasTable[byRow-1].SetEqpRecvPort( ptBasCfg->GetEqpRecvPort() );
        m_atBasTable[byRow-1].SetRunBrdId( ptBasCfg->GetRunBrdId() );
		
        // zbq [2007/03/02] 外设配置重新写入时需再次从单板表里获取该外设的最新Ip
        m_atBasTable[byRow-1].SetIpAddr( GetBrdIpByAgtId(ptBasCfg->GetRunBrdId()) );
		
        m_atBasTable[byRow-1].SetAlias( ptBasCfg->GetAlias() );
        
        u8 byMapNum = 0;
        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        ptBasCfg->GetUsedMap( abyMapId, byMapNum );
        m_atBasTable[byRow-1].SetUsedMap( abyMapId, byMapNum );
        
        bFlag = ChangeBrdRunEqpNum( ptBasCfg->GetRunBrdId(), ptBasCfg->GetEqpId() );
        if ( !bFlag )
            wRet = ERR_AGENT_SETNODEVALUE;
        
        if ( SUCCESS_AGENT == wRet )
        {
            s8 achBasCfgInfo[512];
            s8 achEntryStr[32];
            s8 achMapStr[32];
            memset( achMapStr, 0, sizeof(achMapStr) );
            memset(achEntryStr, '\0', sizeof(achEntryStr));
            memset(achBasCfgInfo, '\0', sizeof(achBasCfgInfo));
			
            m_atBasTable[byRow-1].GetUsedMapStr( achMapStr );
            GetEntryString(achEntryStr, byRow-1);
            sprintf(achBasCfgInfo, "   %d\t%d\t%d\t%s\t%d\t%d\t%s", 
				m_atBasTable[byRow-1].GetEqpId(),
				/*m_atBasTable[byRow-1].GetMcuRecvPort(),*/ptBasCfg->GetMcuRecvPort(),
				m_atBasTable[byRow-1].GetSwitchBrdId(),
				m_atBasTable[byRow-1].GetAlias(),
				m_atBasTable[byRow-1].GetRunBrdId(),
				m_atBasTable[byRow-1].GetEqpRecvPort(),
				achMapStr);
            
            wRet = WriteTableEntry( m_achCfgName, SECTION_mcueqpBasTable, 
				achEntryStr, achBasCfgInfo );
            if ( SUCCESS_AGENT != wRet )
            {
                Agtlog(LOG_ERROR, "[SetEqpBasCfg] Fail to write the profile in %s\n", 
					SECTION_mcueqpBasTable );
                wRet = ERR_AGENT_SETNODEVALUE;
            }
        }
    }
    else
        wRet = ERR_AGENT_SETNODEVALUE;
    
    return wRet;
}

/*=============================================================================
函 数 名： SetEqpBasHDCfgByRow
功    能： 据配置行号设BASHD信息
算法实现： 
全局变量： 
参    数： u8 byId
TEqpBasHDInfo * ptBasHDCfg
 BOOL32 bIsNeedUpdatePortInTable 标识是否需要更新内存表中端口
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/08/07  4.0         周文        创建
=============================================================================*/
u16 CCfgAgent::SetEqpBasHDCfgByRow( u8 byRow, TEqpBasHDInfo * ptBasHDCfg, BOOL32 bIsNeedUpdatePortInTable )
{
    BOOL32 bFlag = FALSE;
    u16 wRet = SUCCESS_AGENT;
    
    if ( CheckEqpRow( EQP_TYPE_BAS, byRow, TRUE ) )
    {
        m_atBasHDTable[byRow-1].SetEqpId( ptBasHDCfg->GetEqpId() );
		
		//如果是通过界面重新分配端口信息，把重新分配后的端口信息写入文件，但不记录在内存中。以防止用户不重启导致已工作
		//外设端口变化，无法正常工作[12/15/2011 chendaiwei]
		if(bIsNeedUpdatePortInTable)
		{
			m_atBasHDTable[byRow-1].SetMcuRecvPort( ptBasHDCfg->GetMcuRecvPort() );
		}

        m_atBasHDTable[byRow-1].SetSwitchBrdId( ptBasHDCfg->GetSwitchBrdId() );
        m_atBasHDTable[byRow-1].SetEqpRecvPort( ptBasHDCfg->GetEqpRecvPort() );
        m_atBasHDTable[byRow-1].SetIpAddr( ptBasHDCfg->GetIpAddr() );
        m_atBasHDTable[byRow-1].SetAlias( ptBasHDCfg->GetAlias() );
        m_atBasHDTable[byRow-1].SetHDBasType( ptBasHDCfg->GetHDBasType() );
        
        if ( SUCCESS_AGENT == wRet )
        {
            s8   achEntryStr[32];
            s8   achBasHDCfgInfo[512];//fix me 512?
            s8   achBasHDIpAddr[32];
            memset(achEntryStr, '\0', sizeof(achEntryStr));
            memset(achBasHDCfgInfo, '\0', sizeof(achBasHDCfgInfo));
            memset(achBasHDIpAddr, '\0', sizeof(achBasHDIpAddr));
			
            GetEntryString(achEntryStr, byRow-1);
            GetIpFromU32(achBasHDIpAddr, htonl(m_atBasHDTable[byRow-1].GetIpAddr()));
			
            sprintf(achBasHDCfgInfo, "   %d", m_atBasHDTable[byRow-1].GetEqpId());   // ID 前4个空格
            sprintf(achBasHDCfgInfo, "%s\t%d", achBasHDCfgInfo, /*m_atBasHDTable[byRow-1].GetMcuRecvPort()*/ptBasHDCfg->GetMcuRecvPort());
            sprintf(achBasHDCfgInfo, "%s\t%d", achBasHDCfgInfo, m_atBasHDTable[byRow-1].GetSwitchBrdId());
            sprintf(achBasHDCfgInfo, "%s\t%s", achBasHDCfgInfo, m_atBasHDTable[byRow-1].GetAlias());
            sprintf(achBasHDCfgInfo, "%s\t%s", achBasHDCfgInfo, achBasHDIpAddr);  // ip address 
            sprintf(achBasHDCfgInfo, "%s\t%d", achBasHDCfgInfo, m_atBasHDTable[byRow-1].GetEqpRecvPort());
            sprintf(achBasHDCfgInfo, "%s\t%d", achBasHDCfgInfo, m_atBasHDTable[byRow-1].GetHDBasType());
            
            wRet = WriteTableEntry( m_achCfgName, SECTION_mcueqpBasHDTable, 
				achEntryStr, achBasHDCfgInfo );
            if ( SUCCESS_AGENT != wRet )
            {
                Agtlog(LOG_ERROR, "[SetEqpBasHDCfg] Fail to write the profile in %s\n", 
					SECTION_mcueqpBasHDTable );
                wRet = ERR_AGENT_SETNODEVALUE;
            }
        }
    }
    else
        wRet = ERR_AGENT_SETNODEVALUE;
    
    return wRet;
}

/*=============================================================================
函 数 名： SetEqpVMPCfgById
功    能： 据外设ID设Vmp信息
算法实现： 
全局变量： 
参    数： u8 byId
TEqpVMPEntry * ptVMPCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::SetEqpVMPCfgById( u8 byId, TEqpVMPInfo * ptVMPCfg )
{
    u16 wRet = ERR_AGENT_SETNODEVALUE;
	
	if( NULL == ptVMPCfg || byId < VMPID_MIN || byId > VMPID_MAX )
    {
        Agtlog(LOG_ERROR, "[SetEqpVMPCfg] param err: byId: %d, ptBasCfg: 0x%x !\n", byId, ptVMPCfg);
        return wRet;
    }
    
    for( u8 byRow = 1; byRow <= m_dwVMPEntryNum; byRow ++ )
    {
        if( byId == m_atVMPTable[byRow-1].GetEqpId() )
        {
            wRet = SetEqpVMPCfgByRow( byRow, ptVMPCfg );
            break;
        }
    }
    if( SUCCESS_AGENT != wRet )
        Agtlog(LOG_ERROR, "[SetEqpVMPCfgById] set vmp<%d> config failed !\n", byId);
	
    return wRet;
}

/*=============================================================================
函 数 名： SetEqpVMPCfgById
功    能： 据配置行号设Vmp信息
算法实现： 
全局变量： 
参    数： u8 byId
TEqpVMPEntry * ptVMPCfg
 BOOL32 bIsNeedUpdatePortInTable 标识是否需要更新内存表中端口
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::SetEqpVMPCfgByRow( u8 byRow, TEqpVMPInfo * ptVMPCfg,BOOL32 bIsNeedUpdatePortInTable )
{
    BOOL32 bFlag = FALSE;
    u16 wRet = SUCCESS_AGENT;
    
    if ( CheckEqpRow( EQP_TYPE_VMP, byRow ) )
    {
        m_atVMPTable[byRow-1].SetEqpId( ptVMPCfg->GetEqpId() );
		//如果是通过界面重新分配端口信息，把重新分配后的端口信息写入文件，但不记录在内存中。以防止用户不重启导致已工作
		//外设端口变化，无法正常工作[12/15/2011 chendaiwei]
		if(bIsNeedUpdatePortInTable)
		{
			m_atVMPTable[byRow-1].SetMcuRecvPort( ptVMPCfg->GetMcuRecvPort() );
		}
        m_atVMPTable[byRow-1].SetSwitchBrdId( ptVMPCfg->GetSwitchBrdId() );
        m_atVMPTable[byRow-1].SetRunBrdId( ptVMPCfg->GetRunBrdId() );
		
        // zbq [2007/03/02] 外设配置重新写入时需再次从单板表里获取该外设的最新Ip
        m_atVMPTable[byRow-1].SetIpAddr( GetBrdIpByAgtId(ptVMPCfg->GetRunBrdId()) );
		
        m_atVMPTable[byRow-1].SetEqpRecvPort( ptVMPCfg->GetEqpRecvPort() );
        m_atVMPTable[byRow-1].SetEncodeNum( ptVMPCfg->GetEncodeNum() );
        m_atVMPTable[byRow-1].SetAlias( ptVMPCfg->GetAlias() );
        
        bFlag = ChangeBrdRunEqpNum( ptVMPCfg->GetRunBrdId(), ptVMPCfg->GetEqpId());
        if ( !bFlag )
            wRet = ERR_AGENT_SETNODEVALUE;
        
        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        u8 byMapNum = 0;
        ptVMPCfg->GetUsedMap( abyMapId, byMapNum );
        m_atVMPTable[byRow-1].SetUsedMap( abyMapId, byMapNum );
		
        if( SUCCESS_AGENT == wRet )
        {
            s8 achUsedMap[32];
            s8 achEntryStr[32];
            s8 achVmpCfgInfo[512];
            memset(achUsedMap, '\0', sizeof(achUsedMap));
            memset(achEntryStr, '\0', sizeof(achEntryStr));
            memset(achVmpCfgInfo, '\0', sizeof(achVmpCfgInfo));
			
            GetEntryString( achEntryStr, byRow-1);
            m_atVMPTable[byRow-1].GetUsedMapStr( achUsedMap );
            sprintf(achVmpCfgInfo, "%d\t%d\t%d\t%s\t%d\t%d\t%d\t%s", 
				m_atVMPTable[byRow-1].GetEqpId(), 
				/*m_atVMPTable[byRow-1].GetMcuRecvPort(),*/ptVMPCfg->GetMcuRecvPort(),
				m_atVMPTable[byRow-1].GetSwitchBrdId(), 
				m_atVMPTable[byRow-1].GetAlias(), 
				m_atVMPTable[byRow-1].GetRunBrdId(),
				m_atVMPTable[byRow-1].GetEqpRecvPort(),
				m_atVMPTable[byRow-1].GetEncodeNum(), 
				achUsedMap );
            
            wRet = WriteTableEntry( m_achCfgName,SECTION_mcueqpVMPTable, achEntryStr, achVmpCfgInfo );
            if ( SUCCESS_AGENT != wRet )
            {
                Agtlog(LOG_ERROR, "[SetEqpVMPCfg] Fail to write the profile in Vmp cfg \n");
                wRet = ERR_AGENT_SETNODEVALUE;
            }
        }
    }
    else
        wRet = ERR_AGENT_SETNODEVALUE;
    
    return wRet;
}

/*=============================================================================
函 数 名： SetEqpMpwCfgById
功    能： 据外设ID设Mpw信息
算法实现： 
全局变量： 
参    数： u8 byId
TEqpMpwInfo* ptMpwCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::SetEqpMpwCfgById(u8 byId, TEqpMpwInfo* ptMpwCfg)
{
    u16 wRet = ERR_AGENT_SETNODEVALUE;
	
	if( NULL == ptMpwCfg || byId < VMPTWID_MIN || byId > VMPTWID_MAX )
    {
        Agtlog(LOG_ERROR, "[SetEqpMpwCfgById] param err: byId: %d, ptMpwCfg: 0x%x !\n", byId, ptMpwCfg);
        return wRet;
    }
    
    for(u8 byRow = 1; byRow <= m_dwMpwEntryNum; byRow++)
    {
        if( byId == m_atMpwTable[byRow-1].GetEqpId() )
        {
            wRet = SetEqpMpwCfgByRow( byRow, ptMpwCfg );
            break;
        }
    }
    if( SUCCESS_AGENT != wRet )
        Agtlog(LOG_ERROR, "[SetEqpMpwCfg] The Mpw ID is not exist or map count is 0\n");
    
    return wRet;
}

/*=============================================================================
函 数 名： SetEqpMpwCfgByRow
功    能： 据配置行号设Mpw信息
算法实现： 
全局变量： 
参    数： u8 byId
TEqpMpwInfo* ptMpwCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::SetEqpMpwCfgByRow( u8 byRow, TEqpMpwInfo * ptMpwCfg )
{
    BOOL32 bFlag = FALSE;
    u16 wRet = SUCCESS_AGENT;
	
    if ( CheckEqpRow( EQP_TYPE_VMPTW, byRow )) 
    {
        m_atMpwTable[byRow-1].SetEqpId( ptMpwCfg->GetEqpId() );
        m_atMpwTable[byRow-1].SetRunBrdId( ptMpwCfg->GetRunBrdId() );
        m_atMpwTable[byRow-1].SetEqpRecvPort( ptMpwCfg->GetEqpRecvPort() );
        m_atMpwTable[byRow-1].SetAlias( ptMpwCfg->GetAlias() );
		
        // zbq [2007/03/02] 外设配置重新写入时需再次从单板表里获取该外设的最新Ip
        m_atMpwTable[byRow-1].SetIpAddr( GetBrdIpByAgtId(ptMpwCfg->GetRunBrdId()) );
        
        bFlag = ChangeBrdRunEqpNum( ptMpwCfg->GetRunBrdId(), ptMpwCfg->GetEqpId());
        if ( !bFlag )
            wRet = ERR_AGENT_SETNODEVALUE;
		
        u8 byMapNum = 0;
        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        ptMpwCfg->GetUsedMap( abyMapId, byMapNum );
        m_atMpwTable[byRow-1].SetUsedMap( abyMapId, byMapNum );
        
        if ( SUCCESS_AGENT == wRet )
        {
            s8 achUsedMap[32];
            s8 achEntryStr[32];
            s8 achMpwCfgInfo[512];
            memset(achUsedMap, '\0', sizeof(achUsedMap)); 
            memset(achEntryStr, '\0', sizeof(achEntryStr));
            memset(achMpwCfgInfo, '\0', sizeof(achMpwCfgInfo));
            
            GetEntryString( achEntryStr, byRow-1 );
            m_atMpwTable[byRow-1].GetUsedMapStr( achUsedMap );
            sprintf(achMpwCfgInfo, "%d\t%s\t%d\t%d\t%s", 
				m_atMpwTable[byRow-1].GetEqpId(),
				m_atMpwTable[byRow-1].GetAlias(),
				m_atMpwTable[byRow-1].GetRunBrdId(),
				m_atMpwTable[byRow-1].GetEqpRecvPort(),  
				achUsedMap );
			
            wRet = WriteTableEntry( m_achCfgName, SECTION_mcueqpMpwTable, achEntryStr, achMpwCfgInfo);
            if ( SUCCESS_AGENT != wRet )
            {
                Agtlog(LOG_ERROR, "[SetEqpMpwCfg] Fail to write the profile in Vmp cfg \n");
                wRet = ERR_AGENT_SETNODEVALUE;
            }
        }
    }
    else
        wRet = ERR_AGENT_SETNODEVALUE;
	
    return wRet;
}

/*=============================================================================
函 数 名： SetEqpPrsCfgById
功    能： 据外设ID设PRS信息
算法实现： 
全局变量： 
参    数： u8 byPrsId
TEqpPrsInfo *ptPrsCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::SetEqpPrsCfgById(u8 byPrsId, TEqpPrsInfo *ptPrsCfg)
{
    u16 wRet = ERR_AGENT_SETNODEVALUE;
	
    for(u8 byRow = 1; byRow <= m_dwPrsEntryNum; byRow++)
    {
        if(m_atPrsTable[byRow-1].GetEqpId() == byPrsId)
        {
            wRet = SetEqpPrsCfgByRow( byRow, ptPrsCfg );
            break;
        }
    }
    return wRet;
}

/*=============================================================================
函 数 名： SetEqpPrsCfg
功    能： 据配置行号设PRS信息
算法实现： 
全局变量： 
参    数： u8 byRow
TEqpPrsInfo *ptPrsCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::SetEqpPrsCfgByRow( u8 byRow, TEqpPrsInfo * ptPrsCfg )
{
    u16 wRet = SUCCESS_AGENT;
	
    if ( CheckEqpRow( EQP_TYPE_PRS, byRow ) )
    {
		m_atPrsTable[byRow-1].SetEqpId( ptPrsCfg->GetEqpId() );
        m_atPrsTable[byRow-1].SetRunBrdId( ptPrsCfg->GetRunBrdId() );
        m_atPrsTable[byRow-1].SetSwitchBrdId( ptPrsCfg->GetSwitchBrdId() );
        
        // zbq [2007/03/02] 外设配置重新写入时需再次从单板表里获取该外设的最新Ip
        m_atPrsTable[byRow-1].SetIpAddr( GetBrdIpByAgtId(ptPrsCfg->GetRunBrdId()) );
        
        m_atPrsTable[byRow-1].SetMcuRecvPort( ptPrsCfg->GetMcuRecvPort() );
        m_atPrsTable[byRow-1].SetEqpRecvPort( ptPrsCfg->GetEqpRecvPort() );
        m_atPrsTable[byRow-1].SetFirstTimeSpan( ptPrsCfg->GetFirstTimeSpan() );
        m_atPrsTable[byRow-1].SetSecondTimeSpan( ptPrsCfg->GetSecondTimeSpan() );
        m_atPrsTable[byRow-1].SetThirdTimeSpan( ptPrsCfg->GetThirdTimeSpan() );
        m_atPrsTable[byRow-1].SetRejectTimeSpan( ptPrsCfg->GetRejectTimeSpan() );
        m_atPrsTable[byRow-1].SetAlias( ptPrsCfg->GetAlias() );
        
        s8  achEntryStr[32];
        s8  achPrsCfgInfo[512];
        memset(achEntryStr, '\0', sizeof(achEntryStr));
        memset(achPrsCfgInfo, '\0', sizeof(achPrsCfgInfo));
		
        GetEntryString(achEntryStr, byRow-1);
        sprintf(achPrsCfgInfo, "  %d\t%d\t%d\t%s\t%d\t%d\t%d\t%d\t%d\t%d",
			m_atPrsTable[byRow-1].GetEqpId(), m_atPrsTable[byRow-1].GetMcuRecvPort(),
			m_atPrsTable[byRow-1].GetSwitchBrdId(), m_atPrsTable[byRow-1].GetAlias(),
			m_atPrsTable[byRow-1].GetRunBrdId(), m_atPrsTable[byRow-1].GetEqpRecvPort(),
			m_atPrsTable[byRow-1].GetFirstTimeSpan(), m_atPrsTable[byRow-1].GetSecondTimeSpan(),
			m_atPrsTable[byRow-1].GetThirdTimeSpan(), m_atPrsTable[byRow-1].GetRejectTimeSpan());
		
        wRet = WriteTableEntry( m_achCfgName, (s8*)SECTION_mcueqpPrsTable, 
			achEntryStr, achPrsCfgInfo );
    }
    else
        wRet = ERR_AGENT_SETNODEVALUE;
	
    if( SUCCESS_AGENT != wRet )
    {
        Agtlog(LOG_ERROR, "[SetEqpPrsCfg] Fail to write the configure file(Prs table)\n");
    }
    return wRet;
}

/*=============================================================================
函 数 名： SetTrapInfo
功    能： 设置Trap服务器
算法实现： 
全局变量： 
参    数：  TTrapInfo* ptTrapInfo
u8 bySnmpNum：Trap服务器序号(从 1 开始)
返 回 值： BOOL32 
=============================================================================*/
u16 CCfgAgent::SetTrapInfo( u8 byRow, TTrapInfo * ptTrapInfo) 
{
	memcpy( &m_atTrapRcvTable[byRow-1], ptTrapInfo, sizeof(TTrapInfo) );
	
	s8  achSnmpIp[32];
    s8  achEntryStr[32];
	s8  achSnmpInfo[256];
	memset(achSnmpIp, '\0', sizeof(achSnmpIp));
    memset(achEntryStr, '\0', sizeof(achEntryStr));    
    memset(achSnmpInfo, '\0', sizeof(achSnmpInfo));
	
    GetEntryString(achEntryStr, byRow-1);
    GetIpFromU32( achSnmpIp, htonl(ptTrapInfo->GetTrapIp()) );
    
    sprintf(achSnmpInfo, "%s\t%s\t%s\t%d\t%d", achSnmpIp,
		ptTrapInfo->GetReadCom(),  ptTrapInfo->GetWriteCom(),
		ptTrapInfo->GetGSPort(), ptTrapInfo->GetTrapPort() );
	
    u16 wRet = WriteTableEntry(m_achCfgName, (s8*)SECTION_mcunetTrapRcvTable, 
		achEntryStr, achSnmpInfo );
    if( SUCCESS_AGENT != wRet )
    {
        Agtlog(LOG_ERROR, "[SetBrdCfg] Fail to write profile in %s\n", SECTION_mcunetTrapRcvTable);
    }
    return wRet;
}

/*=============================================================================
函 数 名： GetTrapInfo
功    能： 取单个Snmp服务器信息
算法实现： 
全局变量： 
参    数： u8 bySnmpIdx： 序号（从1开始）
TTrapInfo* ptTrapInfo
返 回 值： BOOL32 
=============================================================================*/
u16 CCfgAgent::GetTrapInfo(u8 byRow, TTrapInfo* ptTrapInfo)
{
	if( NULL == ptTrapInfo ||  byRow > (u8)m_dwTrapRcvEntryNum || byRow < 1 )
    {
        Agtlog(LOG_ERROR, "[GetTrapInfo] Error param: ptTrap.0x%x, byRow.%d > m_tTrapNum.%d\n",
												ptTrapInfo, byRow, m_dwTrapRcvEntryNum );
        return ERR_AGENT_GETTABLEVALUE;
    }	
	memcpy( ptTrapInfo, &m_atTrapRcvTable[byRow-1], sizeof(TTrapInfo) );
    return SUCCESS_AGENT;
}

/*=============================================================================
函 数 名： GetTrapInfo
功    能： 取整个Trap信息
算法实现： 
全局变量： 
参    数： TTrapInfo* ptTrapInfo
返 回 值： BOOL32 
=============================================================================*/
u16 CCfgAgent::GetTrapInfo( TTrapInfo* ptTrapInfo )
{
    if( NULL == ptTrapInfo )
    {
        Agtlog(LOG_ERROR, "[GetTrapInfo] The Trap pointer can not be Null\n");
        return ERR_AGENT_GETTABLEVALUE;
    }
    
    for( u8 byIndex = 0; byIndex < m_dwTrapRcvEntryNum; byIndex++)
    {
		memcpy( &ptTrapInfo[byIndex], &m_atTrapRcvTable[byIndex], sizeof(TTrapInfo) );
    }
    return SUCCESS_AGENT;
}

/*=============================================================================
函 数 名： WriteIntToFile
功    能： 写注册表文件
算法实现： 
全局变量： 
参    数： 
const s8* plszFileName
const s8* pszSectionName
const s8* pszKeyName
const s32 dwValue
返 回 值： u16 
=============================================================================*/
u16 CCfgAgent::WriteIntToFile(const s8* plszFileName, const s8* pszSectionName, const s8* pszKeyName, const s32 dwValue)
{
    u16 wRet = SUCCESS_AGENT;
	ENTER( m_hMcuAllConfig );
	
	FILE* hWrite = fopen(plszFileName, "a+"); // create configure file
	if( NULL == hWrite )
    {
		return ERR_AGENT_CREATECONFFILE;	
	}
	fclose(hWrite);
	hWrite = NULL;
	
    BOOL32 bResult = SetRegKeyInt(plszFileName, pszSectionName, pszKeyName, dwValue);
    if ( !bResult)
    {
        wRet = ERR_AGENT_WRITEFILE;
        printf("[WriteIntToFile] Wrong profile while setting %s!\n", pszKeyName);
    }
    return wRet;
}

/*=============================================================================
函 数 名： WriteIntToFile
功    能： 写注册表文件
算法实现： 
全局变量： 
参    数： const s8* pszSectionName
const s8* pszKeyName
const s32 dwValue
返 回 值： u16 
=============================================================================*/
u16 CCfgAgent::WriteIntToFile(const s8* pszSectionName, const s8* pszKeyName, const s32 dwValue)
{
    u16 wRet = SUCCESS_AGENT;
	ENTER( m_hMcuAllConfig );
	
    BOOL32 bResult = SetRegKeyInt(m_achCfgName, pszSectionName, pszKeyName, dwValue);
    if ( !bResult)
    {
        wRet = ERR_AGENT_WRITEFILE;
        printf("[WriteIntToFile] Wrong profile while setting %s!\n", pszKeyName);
    }
    return wRet;
}

/*=============================================================================
函 数 名： WriteStringToFile
功    能： 写字符串到指定的文件
算法实现： 
全局变量： 
参    数： const s8* plszFileName
const s8* pszSectionName
const s8* pszKeyName
const s8* pszValue
返 回 值： u16 
=============================================================================*/
u16 CCfgAgent::WriteStringToFile(const s8* plszFileName, const s8* pszSectionName, const s8* pszKeyName, const s8* pszValue)
{
    u16 wRet = SUCCESS_AGENT;
	ENTER( m_hMcuAllConfig );
	FILE* hWrite = fopen(plszFileName, "a+"); // create configure file
	if( NULL == hWrite )
    {
		return ERR_AGENT_CREATECONFFILE;	
	}

	fclose(hWrite);
	hWrite = NULL;

    BOOL32 bResult = SetRegKeyString(plszFileName, pszSectionName, pszKeyName, pszValue);
    if ( !bResult)
    {
        wRet = ERR_AGENT_WRITEFILE;
        printf("[WriteStringToFile] Wrong profile while setting %s!\n", pszKeyName);
    }
    return wRet;
}

/*=============================================================================
函 数 名： SetStringToFile
功    能： 写字符串到文件
算法实现： 
全局变量： 
参    数： const s8* pszSectionName
const s8* pszKeyName
const s8* pszValue
返 回 值： BOOL32 
=============================================================================*/
u16 CCfgAgent::WriteStringToFile(const s8* pszSectionName, const s8* pszKeyName, const s8* pszValue)
{
    u16 wRet = SUCCESS_AGENT;
	ENTER( m_hMcuAllConfig );
	
    BOOL32 bResult = SetRegKeyString(m_achCfgName, pszSectionName, pszKeyName, pszValue);
    if ( !bResult)
    {
        wRet = ERR_AGENT_WRITEFILE;
        printf("[WriteStringToFile] Wrong profile while setting %s!\n", pszKeyName);
    }
    return wRet;
}

/*=============================================================================
函 数 名： GetEqpRecCfgbyId
功    能： 据外设ID取录像机信息
算法实现： 
全局变量： 
参    数： u8 byId
TEqpRecEntry * ptRecCfg
返 回 值： u16
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/14  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::GetEqpRecCfgById( u8 byId, TEqpRecInfo * ptRecCfg )
{
    if( NULL == ptRecCfg || byId < RECORDERID_MIN || byId > RECORDERID_MAX )
    {
        Agtlog(LOG_ERROR, "[GetEqpRecCfgbyId] param err: byId<%d>, ptCfg: 0x%x\n", byId, ptRecCfg);
        return ERR_AGENT_GETTABLEVALUE;
    }
    u16 wRet = ERR_AGENT_EQPNOTEXIST;
	
    for( u8 byRow = 1; byRow <= m_dwRecEntryNum; byRow ++ )
    {
        if( m_atRecTable[byRow-1].GetEqpId() == byId )
        {
            wRet = GetEqpRecCfgByRow( byRow, ptRecCfg );
            break;
        }
    }
    if( SUCCESS_AGENT != wRet )
        Agtlog(LOG_ERROR, "[GetEqpRecorderCfg] The recorder ID.%d is not exist,please check it\n", byId);
    
    return wRet;
}

/*=============================================================================
函 数 名： GetEqpRecCfgByRow
功    能： 据配置行号取录像机信息
算法实现： 
全局变量： 
参    数： u8 byRow
TEqpRecEntry * ptRecCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::GetEqpRecCfgByRow( u8 byRow, TEqpRecInfo * ptRecCfg )
{
    if ( NULL == ptRecCfg )
        return ERR_AGENT_GETTABLEVALUE;
	
    if ( CheckEqpRow( EQP_TYPE_RECORDER, byRow) )
    {
        ptRecCfg->SetEqpId( m_atRecTable[byRow-1].GetEqpId() );
        ptRecCfg->SetMcuRecvPort( m_atRecTable[byRow-1].GetMcuRecvPort() );
        ptRecCfg->SetSwitchBrdId( m_atRecTable[byRow-1].GetSwitchBrdId() );
        ptRecCfg->SetIpAddr( m_atRecTable[byRow-1].GetIpAddr() );
        ptRecCfg->SetEqpRecvPort( m_atRecTable[byRow-1].GetEqpRecvPort() );
        ptRecCfg->SetAlias( m_atRecTable[byRow-1].GetAlias() );
        return SUCCESS_AGENT;
    }
    return ERR_AGENT_EQPNOTEXIST;    
}

/*=============================================================================
函 数 名： GetEqpVrsRecCfgByRow
功    能： 据配置行号取Vrs新录播信息
算法实现： 
全局变量： 
参    数： u8 byRow
		   TEqpVrsRecCfgInfo * ptVrsRecCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2013/10/09  4.7     yanghuaizhi       创建
=============================================================================*/
u16 CCfgAgent::GetEqpVrsRecCfgByRow( u8 byRow, TEqpVrsRecCfgInfo * ptVrsRecCfg )
{
    if ( NULL == ptVrsRecCfg )
        return ERR_AGENT_GETTABLEVALUE;
	
	if (byRow != 0 && byRow <= m_dwVrsRecEntryNum)
    {
		ptVrsRecCfg->SetVrsId(m_atVrsRecTable[byRow-1].GetVrsId());
		ptVrsRecCfg->SetVrsAlias(m_atVrsRecTable[byRow-1].GetVrsAlias());
		ptVrsRecCfg->SetVrsCallAlias(m_atVrsRecTable[byRow-1].GetVrsCallAlias());
        return SUCCESS_AGENT;
    }
    return ERR_AGENT_EQPNOTEXIST;
}

/*=============================================================================
函 数 名： GetEqpTVWallCfgbyId
功    能： 据外设ID取电视墙信息
算法实现： 
全局变量： 
参    数： u8 byId,
TEqpTVWallEntry * ptTWCfg
返 回 值： u16
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::GetEqpTVWallCfgById( u8 byId, TEqpTVWallInfo * ptTWCfg )
{
    if( NULL == ptTWCfg || byId < TVWALLID_MIN || byId > TVWALLID_MAX )
    {
        Agtlog(LOG_ERROR, "[GetEqpTVWallCfg] param err: byId<%d>, ptCfg: 0x%x\n", byId, ptTWCfg);
        return ERR_AGENT_GETTABLEVALUE;
    }
    u16 wRet = ERR_AGENT_EQPNOTEXIST;
	
    for( u8 byRow = 1; byRow <= m_dwTVWallEntryNum; byRow ++ )
    {
        if( m_atTVWallTable[byRow-1].GetEqpId() == byId )
        {
            wRet = GetEqpTVWallCfgByRow( byRow, ptTWCfg);
            break;
        }
    }
    if( SUCCESS_AGENT != wRet )
        Agtlog(LOG_ERROR, "[GetEqpTVWallCfg] The Tv ID.%d is not exist,please check it\n", byId);
	
    return wRet;
}

/*=============================================================================
函 数 名： GetEqpTVWallCfgByRow
功    能： 据配置行号取电视墙信息
算法实现： 
全局变量： 
参    数： u8 byRow
TEqpTVWallInfo * ptTWCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::GetEqpTVWallCfgByRow( u8 byRow, TEqpTVWallInfo * ptTWCfg )
{
    if ( NULL == ptTWCfg )
        return ERR_AGENT_GETTABLEVALUE;
	
    if ( CheckEqpRow( EQP_TYPE_TVWALL, byRow) )
    {
        ptTWCfg->SetEqpId( m_atTVWallTable[byRow-1].GetEqpId() );
        ptTWCfg->SetRunBrdId( m_atTVWallTable[byRow-1].GetRunBrdId() );
        ptTWCfg->SetEqpRecvPort( m_atTVWallTable[byRow-1].GetEqpRecvPort() );
        ptTWCfg->SetAlias( m_atTVWallTable[byRow-1].GetAlias() );
        
        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        u8 byMapNum = 0;
        m_atTVWallTable[byRow-1].GetUsedMap( abyMapId, byMapNum );
        ptTWCfg->SetUsedMap( abyMapId, byMapNum );
		
        return SUCCESS_AGENT;
    }
    return ERR_AGENT_EQPNOTEXIST; 
}

/*=============================================================================
函 数 名： GetEqpMixerCfgById
功    能： 据外设ID取混音器信息
算法实现： 
全局变量： 
参    数： u8 byId,
TEqpMixerEntry * ptMixerCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::GetEqpMixerCfgById( u8 byId, TEqpMixerInfo * ptMixerCfg )
{
    if( NULL == ptMixerCfg  || byId < MIXERID_MIN || byId > MIXERID_MAX )
    {
        Agtlog(LOG_ERROR, "[GetEqpMixerCfg] param err: byId<%d>, ptCfg: 0x%x\n", byId, ptMixerCfg);
        return ERR_AGENT_GETTABLEVALUE;
    }
    
    u16 wRet = ERR_AGENT_EQPNOTEXIST;

    for( u8 byRow = 1; byRow <= m_dwMixEntryNum; byRow ++ )
    {
        if ( byId == m_atMixTable[byRow-1].GetEqpId() )
        {
            wRet = GetEqpMixerCfgByRow( byRow, ptMixerCfg );
            break;
        }
    }

    if ( SUCCESS_AGENT != wRet )
	{
        Agtlog(LOG_ERROR, "[GetEqpMixerCfg] The mixer ID.%d is not exist,please check it\n", byId);
		return ERR_AGENT_EQPNOTEXIST;
	}

    return SUCCESS_AGENT;
}

/*=============================================================================
函 数 名： GetEqpMixerCfgbyRow
功    能： 据配置行号取混音器信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpMixerEntry * ptMixerCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::GetEqpMixerCfgByRow( u8 byRow, TEqpMixerInfo * ptMixerCfg )
{
    if ( NULL == ptMixerCfg )
        return ERR_AGENT_GETTABLEVALUE;
    //tianzhiyong 20100312
    if ( CheckEqpRow( EQP_TYPE_MIXER, byRow))
    {
        ptMixerCfg->SetEqpId( m_atMixTable[byRow-1].GetEqpId() );
        ptMixerCfg->SetMcuRecvPort( m_atMixTable[byRow-1].GetMcuRecvPort() );
        ptMixerCfg->SetSwitchBrdId( m_atMixTable[byRow-1].GetSwitchBrdId() );
        ptMixerCfg->SetEqpRecvPort( m_atMixTable[byRow-1].GetEqpRecvPort() );
        ptMixerCfg->SetRunBrdId( m_atMixTable[byRow-1].GetRunBrdId() );
        ptMixerCfg->SetMaxChnInGrp( m_atMixTable[byRow-1].GetMaxChnInGrp() );
		ptMixerCfg->SetAlias( m_atMixTable[byRow-1].GetAlias() );
        
        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        u8 byMapNum = 0;
        m_atMixTable[byRow-1].GetUsedMap( abyMapId, byMapNum );
        ptMixerCfg->SetUsedMap( abyMapId, byMapNum ); 
		
		ptMixerCfg->SetIsSendRedundancy( m_atMixTable[byRow-1].IsSendRedundancy() );
		
        return SUCCESS_AGENT;
    }
    return ERR_AGENT_EQPNOTEXIST; 
}

/*=============================================================================
函 数 名： GetEqpBasCfgbyId
功    能： 据外设ID取Bas信息
算法实现： 
全局变量： 
参    数：  u8 byId,
TEqpBasEntry * ptBasCfg
返 回 值： u16
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::GetEqpBasCfgById( u8 byId, TEqpBasInfo * ptBasCfg )
{
    if( NULL == ptBasCfg || byId < BASID_MIN || byId > BASID_MAX )
    {
        Agtlog(LOG_ERROR, "[GetEqpBasCfg] param err: byId<%d>, ptCfg: 0x%x\n", byId, ptBasCfg);
        return ERR_AGENT_GETTABLEVALUE;
    }
	
    // 高清BAS直接返回错误码, zgc, 2008-08-22
    if ( IsEqpBasHD(byId) )
    {
        Agtlog(LOG_VERBOSE, "[GetEqpBasCfg] Bas<%d> is HD-Bas\n", byId );
        return ERR_AGENT_GETTABLEVALUE;
    }
	
    u16 wRet = ERR_AGENT_EQPNOTEXIST;
    for( u8 byRow = 1; byRow <= (u8)m_dwBasEntryNum; byRow++)
    {
        if( m_atBasTable[byRow-1].GetEqpId() == byId )
        {   
            wRet = GetEqpBasCfgByRow( byRow, ptBasCfg );
            break;
        }
    }
    
    if ( SUCCESS_AGENT != wRet )
        Agtlog(LOG_ERROR, "[GetEqpBasCfg] The bas ID.%d is not exist,please check it\n", byId);
    
    return wRet;
}

/*=============================================================================
函 数 名： IsEqpBasHD
功    能： 
算法实现： 
全局变量： 
参    数： u8 byId,
返 回 值： BOOL32
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/08/07  4.0         周文        创建
=============================================================================*/
BOOL32 CCfgAgent::IsEqpBasHD( u8 byId )
{
#ifdef _8KI_
	return TRUE;
#endif

    BOOL32 wRet = FALSE;
    u8 byRow = 1;
    for( byRow = 1; byRow <= (u8)m_dwBasHDEntryNum; byRow++)
    {
        if( byId == m_atBasHDTable[byRow-1].GetEqpId() )
        {   
            wRet = TRUE;
            break;
        }
    }
    if (!wRet)
    {
        for( byRow = 1; byRow <= (u8)m_dwMpuBasEntryNum; byRow++)
        {
			//APU2 BAS在表中，但实际不属于HD BAS[3/22/2013 chendaiwei]
            if( byId == m_atMpuBasTable[byRow-1].GetEqpId() 
				&& m_atMpuBasTable[byRow-1].GetStartMode() != TYPE_APU2BAS)
            {   
                wRet = TRUE;
                break;
            }
        }
    }
    return wRet;
}

BOOL32 CCfgAgent::IsEqpBasAud(u8 byEqpId)
{
	BOOL32 wRet = FALSE;

    for( u8 byRow = 1; byRow <= (u8)m_dwMpuBasEntryNum; byRow++)
    {
        if( byEqpId == m_atMpuBasTable[byRow-1].GetEqpId()
			&& m_atMpuBasTable[byRow-1].GetStartMode() == TYPE_APU2BAS)
        {   
            wRet = TRUE;
            break;
        }
    }

    return wRet;
}

/*=============================================================================
函 数 名： GetMpuBasCfgById
功    能： 据外设ID取BasHD信息
算法实现： 
全局变量： 
参    数：  u8 byId,
TEqpBasHDInfo * ptBasHDCfg
返 回 值： u16
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2012/11/08  4.7          倪志俊        创建
=============================================================================*/
u16 CCfgAgent::GetMpuBasCfgById( u8 byId,TEqpMpuBasInfo* ptEqpMpuBasInfo )
{
	u16 wRet = ERR_AGENT_EQPNOTEXIST;

	if( NULL == ptEqpMpuBasInfo || byId < BASID_MIN || byId > BASID_MAX )
    {
        Agtlog(LOG_ERROR, "[GetMpuBasCfgById] param err: byId<%d>, ptCfg: 0x%x\n", byId, ptEqpMpuBasInfo);
        return ERR_AGENT_GETTABLEVALUE;
    }

	u8 byRow = 0;
	for( byRow = 1; byRow <= (u8)m_dwMpuBasEntryNum; byRow++)
	{
		if( m_atMpuBasTable[byRow-1].GetEqpId() == byId )
		{   
			wRet = GetEqpMpuBasCfgByRow( byRow, ptEqpMpuBasInfo );
			break;
		}
    }

	return wRet;
}

/*=============================================================================
函 数 名： GetEqpBasHDCfgbyId
功    能： 据外设ID取BasHD信息
算法实现： 
全局变量： 
参    数：  u8 byId,
TEqpBasHDEntry * ptBasHDCfg
返 回 值： u16
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/08/07  4.0         周文        创建
=============================================================================*/
u16 CCfgAgent::GetEqpBasHDCfgById( u8 byId, TEqpBasHDInfo * ptBasHDCfg )
{
    if( NULL == ptBasHDCfg || byId < BASID_MIN || byId > BASID_MAX )
    {
        Agtlog(LOG_ERROR, "[GetEqpBasHDCfg] param err: byId<%d>, ptCfg: 0x%x\n", byId, ptBasHDCfg);
        return ERR_AGENT_GETTABLEVALUE;
    }
	
    u16 wRet = ERR_AGENT_EQPNOTEXIST;
    u8 byRow = 0;
    for( byRow = 1; byRow <= (u8)m_dwBasHDEntryNum; byRow++)
    {
        if( m_atBasHDTable[byRow-1].GetEqpId() == byId )
        {   
            wRet = GetEqpBasHDCfgByRow( byRow, ptBasHDCfg );
            break;
        }
    }
    if (SUCCESS_AGENT == wRet)
    {
        return wRet;
    }
    
    //尝试mpu－bas
	//尝试mpu2-bas[1/4/2012 chendaiwei]
    TEqpMpuBasInfo tMpuBasCfg;
	
    if (SUCCESS_AGENT != wRet)
    {
        for( byRow = 1; byRow <= (u8)m_dwMpuBasEntryNum; byRow++)
        {
            if( m_atMpuBasTable[byRow-1].GetEqpId() == byId )
            {   
                wRet = GetEqpMpuBasCfgByRow( byRow, &tMpuBasCfg );
                break;
            }
        }
    }
    if (SUCCESS_AGENT == wRet)
    {
        GetHdBasCfgFromMpuBas(*ptBasHDCfg, tMpuBasCfg);
    }
	
    if ( SUCCESS_AGENT != wRet )
        Agtlog(LOG_ERROR, "[GetEqpBasHDCfg] The basHD ID.%d is not exist,please check it\n", byId);
    
    return wRet;
}

/*=============================================================================
函 数 名： GetEqpBasCfgbyRow
功    能： 据配置行号取BAS信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpBasInfo * ptBasCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::GetEqpBasCfgByRow( u8 byRow, TEqpBasInfo * ptBasCfg )
{
    if ( NULL == ptBasCfg )
        return ERR_AGENT_GETTABLEVALUE;
    
    if ( CheckEqpRow(EQP_TYPE_BAS, byRow) )
    {
        ptBasCfg->SetEqpId( m_atBasTable[byRow-1].GetEqpId() );
        ptBasCfg->SetMcuRecvPort( m_atBasTable[byRow-1].GetMcuRecvPort() );
        ptBasCfg->SetSwitchBrdId( m_atBasTable[byRow-1].GetSwitchBrdId() );
        ptBasCfg->SetEqpRecvPort( m_atBasTable[byRow-1].GetEqpRecvPort() );
        ptBasCfg->SetRunBrdId( m_atBasTable[byRow-1].GetRunBrdId() );
        ptBasCfg->SetAlias( m_atBasTable[byRow-1].GetAlias() );
        
        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        u8 byMapNum = 0;
        m_atBasTable[byRow-1].GetUsedMap( abyMapId, byMapNum );
        ptBasCfg->SetUsedMap( abyMapId, byMapNum );
		
        return SUCCESS_AGENT;
    }
    return ERR_AGENT_EQPNOTEXIST; 
}

/*=============================================================================
函 数 名： GetEqpBasHDCfgbyRow
功    能： 据配置行号取BASHD信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpBasHDInfo * ptBasHDCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/08/07  4.0         周文        创建
=============================================================================*/
u16 CCfgAgent::GetEqpBasHDCfgByRow( u8 byRow, TEqpBasHDInfo * ptBasHDCfg )
{
    if ( NULL == ptBasHDCfg )
        return ERR_AGENT_GETTABLEVALUE;
    
    if ( CheckEqpRow(EQP_TYPE_BAS, byRow, TRUE) )
    {
        ptBasHDCfg->SetEqpId( m_atBasHDTable[byRow-1].GetEqpId() );
        ptBasHDCfg->SetMcuRecvPort( m_atBasHDTable[byRow-1].GetMcuRecvPort() );
        ptBasHDCfg->SetSwitchBrdId( m_atBasHDTable[byRow-1].GetSwitchBrdId() );
        ptBasHDCfg->SetEqpRecvPort( m_atBasHDTable[byRow-1].GetEqpRecvPort() );
        ptBasHDCfg->SetAlias( m_atBasHDTable[byRow-1].GetAlias() );
        ptBasHDCfg->SetIpAddr( m_atBasHDTable[byRow-1].GetIpAddr() );
        ptBasHDCfg->SetHDBasType( m_atBasHDTable[byRow - 1].GetHDBasType() );
		
        return SUCCESS_AGENT;
    }
    return ERR_AGENT_EQPNOTEXIST; 
}

/*=============================================================================
函 数 名： GetEqpVMPCfg
功    能： 据配置行号取VMP信息
算法实现： 
全局变量： 
参    数： u8 byId,
TEqpVMPEntry * ptVMPCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::GetEqpVMPCfgById( u8 byId, TEqpVMPInfo * ptVMPCfg )
{
    if( NULL == ptVMPCfg || byId < VMPID_MIN || byId > VMPID_MAX )
    {
        Agtlog(LOG_ERROR, "[GetEqpBasCfg] param err: byId<%d>, ptCfg: 0x%x\n", byId, ptVMPCfg);
        return ERR_AGENT_GETTABLEVALUE;
    }
    u16 wRet = ERR_AGENT_EQPNOTEXIST;
	
    for( u8 byRow = 1; byRow <= m_dwVMPEntryNum; byRow++)
    {
        if( m_atVMPTable[byRow-1].GetEqpId() == byId )
        {   
            wRet = GetEqpVMPCfgByRow( byRow, ptVMPCfg );
            break;
        }
    }
    if( SUCCESS_AGENT != wRet )
        Agtlog(LOG_ERROR, "[GetEqpVMPCfg] The vmp ID.%d is not exist,please check it\n", byId);
	
    return wRet;
}

/*=============================================================================
函 数 名： GetEqpVMPCfgByRow
功    能： 据配置行号取VMP信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpVMPInfo * ptVMPCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::GetEqpVMPCfgByRow( u8 byRow, TEqpVMPInfo * ptVMPCfg )
{
    if ( NULL == ptVMPCfg )
        return ERR_AGENT_GETTABLEVALUE;
    
    if ( CheckEqpRow(EQP_TYPE_VMP, byRow) )
    {
        ptVMPCfg->SetEqpId( m_atVMPTable[byRow-1].GetEqpId() );
        ptVMPCfg->SetMcuRecvPort( m_atVMPTable[byRow-1].GetMcuRecvPort() );
        ptVMPCfg->SetSwitchBrdId( m_atVMPTable[byRow-1].GetSwitchBrdId() );
        ptVMPCfg->SetRunBrdId( m_atVMPTable[byRow-1].GetRunBrdId() );
        ptVMPCfg->SetEqpRecvPort( m_atVMPTable[byRow-1].GetEqpRecvPort() );
        ptVMPCfg->SetEncodeNum( m_atVMPTable[byRow-1].GetEncodeNum() );
        ptVMPCfg->SetAlias( m_atVMPTable[byRow-1].GetAlias() );
        
        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        u8 byMapNum = 0;
        m_atVMPTable[byRow-1].GetUsedMap( abyMapId, byMapNum );
        ptVMPCfg->SetUsedMap( abyMapId, byMapNum );
		
        return SUCCESS_AGENT;
    }
    return ERR_AGENT_EQPNOTEXIST; 
}

/*=============================================================================
函 数 名： GetEqpMpwCfg
功    能： 据外设ID取Mpw信息
算法实现： 
全局变量： 
参    数： u8 byId
TEqpMpwInfo* ptMpwCfg
返 回 值： u16
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::GetEqpMpwCfgById(u8 byId, TEqpMpwInfo* ptMpwCfg)
{
    if( NULL == ptMpwCfg || byId < VMPTWID_MIN || byId > VMPTWID_MAX )
    {
        Agtlog(LOG_ERROR, "[GetEqpMpwCfg] param err: byId<%d>, ptCfg: 0x%x\n", byId, ptMpwCfg);
        return ERR_AGENT_GETTABLEVALUE;
    }
	
    u16 wRet = ERR_AGENT_EQPNOTEXIST;
    for( u8 byRow = 1; byRow <= m_dwMpwEntryNum; byRow++)
    {
        if( m_atMpwTable[byRow-1].GetEqpId() == byId )
        {
            wRet = GetEqpMpwCfgByRow( byRow, ptMpwCfg );
            break;
        }
    }
    if( SUCCESS_AGENT != wRet )
    {
        Agtlog(LOG_ERROR, "[GetEqpMpwCfg] The Mpw ID.%d is not exist,please check it\n", byId);
    }
    return wRet;
}

/*=============================================================================
函 数 名： GetEqpMpwCfgByRow
功    能： 据配置行号取Mpw信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpVMPInfo * ptVMPCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::GetEqpMpwCfgByRow( u8 byRow, TEqpMpwInfo * ptMpwCfg )
{
    if ( NULL == ptMpwCfg )
        return ERR_AGENT_GETTABLEVALUE;
    
    if ( CheckEqpRow( EQP_TYPE_VMPTW, byRow) )
    {
        ptMpwCfg->SetEqpId( m_atMpwTable[byRow-1].GetEqpId() );
        ptMpwCfg->SetRunBrdId( m_atMpwTable[byRow-1].GetRunBrdId() );
        ptMpwCfg->SetEqpRecvPort( m_atMpwTable[byRow-1].GetEqpRecvPort() );
        ptMpwCfg->SetAlias( m_atMpwTable[byRow-1].GetAlias() );
        
        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        u8 byMapNum = 0;
        m_atMpwTable[byRow-1].GetUsedMap( abyMapId, byMapNum );
        ptMpwCfg->SetUsedMap( abyMapId, byMapNum );
        
        return SUCCESS_AGENT;
    }
    return ERR_AGENT_EQPNOTEXIST; 
}

/*=============================================================================
函 数 名： GetEqpPrsCfgById
功    能： 据外设ID取PRS信息
算法实现： 
全局变量： 
参    数： u8 byPrsId
TEqpPrsInfo* ptPrsCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::GetEqpPrsCfgById(u8 byId, TEqpPrsInfo * ptPrsCfg)
{
    if( byId < PRSID_MIN || byId > PRSID_MAX || NULL == ptPrsCfg)
    {
        Agtlog(LOG_ERROR, "[GetEqpPrsCfg] param err: byId<%d> or ptPrsCfg is null \n", byId);
        return ERR_AGENT_GETTABLEVALUE;
    }
    
    u16 wRet = ERR_AGENT_GETTABLEVALUE;
	
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	TPrsTimeSpan tPrsCfg;
	if (SUCCESS_AGENT == GetPrsTimeSpanCfgInfo(tPrsCfg))
	{
		ptPrsCfg->SetAlias("prs");
		ptPrsCfg->SetEqpId(PRSID_MIN);
		ptPrsCfg->SetIpAddr(ntohl(GetLocalIp()));
		ptPrsCfg->SetEqpRecvPort(PRS_EQP_STARTPORT);
		ptPrsCfg->SetMcuRecvPort(PRS_MCU_STARTPORT);
		ptPrsCfg->SetRunBrdId(1);
		ptPrsCfg->SetSwitchBrdId(1);
		ptPrsCfg->SetFirstTimeSpan(tPrsCfg.m_wFirstTimeSpan);
		ptPrsCfg->SetSecondTimeSpan(tPrsCfg.m_wSecondTimeSpan);
		ptPrsCfg->SetThirdTimeSpan(tPrsCfg.m_wThirdTimeSpan);
		ptPrsCfg->SetRejectTimeSpan(tPrsCfg.m_wRejectTimeSpan);
		wRet = SUCCESS_AGENT;
	}
	else
	{
		OspPrintf(TRUE, FALSE, "[GetEqpPrsCfgById]Fail to get prscfg");
	}
#else
    
    for(u8 byRow = 1; byRow <= m_dwPrsEntryNum; byRow++)
    {
        if(m_atPrsTable[byRow-1].GetEqpId() == byId)
        {
            wRet = GetEqpPrsCfgByRow( byRow, ptPrsCfg );
            break;
        }
    }
    if ( SUCCESS_AGENT != wRet )
        Agtlog(LOG_ERROR, "[GetEqpPrsCfg] The prs ID.%d is not exist,please check it\n", byId);
#endif
	
    return wRet;
}

/*=============================================================================
函 数 名： GetEqpPrsCfgByRow
功    能： 据配置行号取PRS信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpPrsInfo * tPrsInfo
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::GetEqpPrsCfgByRow( u8 byRow, TEqpPrsInfo * ptPrsInfo )
{
    if ( NULL == ptPrsInfo )
        return ERR_AGENT_GETTABLEVALUE;
    
    if ( CheckEqpRow( EQP_TYPE_PRS, byRow) )
    {
        ptPrsInfo->SetEqpId( m_atPrsTable[byRow-1].GetEqpId() );
        ptPrsInfo->SetRunBrdId( m_atPrsTable[byRow-1].GetRunBrdId() );
        ptPrsInfo->SetSwitchBrdId( m_atPrsTable[byRow-1].GetSwitchBrdId() );
        ptPrsInfo->SetIpAddr( m_atPrsTable[byRow-1].GetIpAddr() );
        ptPrsInfo->SetMcuRecvPort( m_atPrsTable[byRow-1].GetMcuRecvPort() );
        ptPrsInfo->SetEqpRecvPort( m_atPrsTable[byRow-1].GetEqpRecvPort() );
        ptPrsInfo->SetFirstTimeSpan( m_atPrsTable[byRow-1].GetFirstTimeSpan() );
        ptPrsInfo->SetRejectTimeSpan( m_atPrsTable[byRow-1].GetRejectTimeSpan() );
        ptPrsInfo->SetSecondTimeSpan( m_atPrsTable[byRow-1].GetSecondTimeSpan() );
        ptPrsInfo->SetThirdTimeSpan( m_atPrsTable[byRow-1].GetThirdTimeSpan() );
        ptPrsInfo->SetAlias( m_atPrsTable[byRow-1].GetAlias() );
        
        return SUCCESS_AGENT;
    }
    return ERR_AGENT_EQPNOTEXIST; 
}

//4.6版本 新加外设 jlb
/*=============================================================================
函 数 名： GetEqpHduCfgByRow
功    能： 据配置行号获取Hdu信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpHduInfo * ptHduCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
u16 CCfgAgent::GetEqpHduCfgByRow( u8 byRow, TEqpHduInfo * ptHduCfg ) 
{
	if ( NULL == ptHduCfg )
        return ERR_AGENT_GETTABLEVALUE;
    
    if ( CheckEqpRow( EQP_TYPE_HDU, byRow) )
    {
        ptHduCfg->SetEqpId( m_atHduTable[byRow-1].GetEqpId() );
        ptHduCfg->SetRunBrdId( m_atHduTable[byRow-1].GetRunBrdId() );
        ptHduCfg->SetIpAddr( m_atHduTable[byRow-1].GetIpAddr() );
        ptHduCfg->SetEqpRecvPort( m_atHduTable[byRow-1].GetEqpRecvPort() );        
        ptHduCfg->SetAlias( m_atHduTable[byRow-1].GetAlias() );
		ptHduCfg->SetStartMode(m_atHduTable[byRow-1].GetStartMode());
        THduChnlModePortAgt tHduCHnModePort;
		m_atHduTable[byRow-1].GetHduChnlModePort(0, tHduCHnModePort);
		ptHduCfg->SetHduChnlModePort(0, tHduCHnModePort);
        m_atHduTable[byRow-1].GetHduChnlModePort(1, tHduCHnModePort);
		ptHduCfg->SetHduChnlModePort(1, tHduCHnModePort);
		
        return SUCCESS_AGENT;
    }
	
    return ERR_AGENT_EQPNOTEXIST; 
}
/*=============================================================================
函 数 名： SetEqpHduCfgByRow
功    能：  据配置行号设置Hdu信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpHduInfo * ptHduCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
u16 CCfgAgent::SetEqpHduCfgByRow( u8 byRow, TEqpHduInfo * ptHduCfg ) 
{
    BOOL32 bFlag = FALSE;
    u16 wRet = SUCCESS_AGENT;
	
    if ( CheckEqpRow( EQP_TYPE_HDU, byRow) )
    {
		m_atHduTable[byRow-1].SetEqpId( ptHduCfg->GetEqpId() );
        m_atHduTable[byRow-1].SetRunBrdId( ptHduCfg->GetRunBrdId() );
        // 外设配置重新写入时需再次从单板表里获取该外设的最新Ip
        
        for (s32 nIndex=0; nIndex < (s32)m_dwBrdCfgEntryNum; nIndex++)
        {
			if (ptHduCfg->GetRunBrdId() == m_atBrdCfgTable[nIndex].GetBrdId())
			{
                m_atHduTable[byRow-1].SetIpAddr( GetBrdIpByAgtId(m_atBrdCfgTable[nIndex].GetBrdId()) );
				break;
			}
        }
		
		m_atHduTable[byRow-1].SetEqpRecvPort( ptHduCfg->GetEqpRecvPort() );
        m_atHduTable[byRow-1].SetAlias( ptHduCfg->GetAlias() );
		m_atHduTable[byRow-1].SetStartMode(ptHduCfg->GetStartMode());
		
		THduChnlModePortAgt tHduChnModePortAgt1, tHduChnModePortAgt2;
        ptHduCfg->GetHduChnlModePort(0, tHduChnModePortAgt1);
		m_atHduTable[byRow-1].SetHduChnlModePort( 0, tHduChnModePortAgt1);
        ptHduCfg->GetHduChnlModePort( 1, tHduChnModePortAgt2 );
		m_atHduTable[byRow-1].SetHduChnlModePort( 1, tHduChnModePortAgt2 );
		
        bFlag = ChangeBrdRunEqpNum( ptHduCfg->GetRunBrdId(), ptHduCfg->GetEqpId() );
        if ( !bFlag ) 
            wRet = ERR_AGENT_SETNODEVALUE;
		
		
        if( SUCCESS_AGENT == wRet )
        {
            s8   achEntryStr[32];
			
            s8   achHduCfgInfo[512];
            memset(achEntryStr, '\0', sizeof(achEntryStr));
            memset(achHduCfgInfo, '\0', sizeof(achHduCfgInfo));	
			
            GetEntryString( achEntryStr, byRow-1);
            sprintf( achHduCfgInfo, "%d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d", 
				m_atHduTable[byRow-1].GetEqpId(),
				m_atHduTable[byRow-1].GetAlias(),
				m_atHduTable[byRow-1].GetRunBrdId(),
				m_atHduTable[byRow-1].GetEqpRecvPort(),
				tHduChnModePortAgt1.GetZoomRate(),
				tHduChnModePortAgt1.GetOutModeType(),
                tHduChnModePortAgt1.GetOutPortType(),
                tHduChnModePortAgt2.GetZoomRate(),
				tHduChnModePortAgt2.GetOutModeType(),
                tHduChnModePortAgt2.GetOutPortType(),
				m_atHduTable[byRow-1].GetStartMode(),
				tHduChnModePortAgt1.GetScalingMode(),
                tHduChnModePortAgt2.GetScalingMode()
                );
            
            wRet = WriteTableEntry( m_achCfgName, SECTION_mcueqpHduTable, 
				achEntryStr, achHduCfgInfo );
            if ( SUCCESS_AGENT != wRet )
            {
                printf("[SetEqpHduCfgByRow] Fail to write the configure file\n");
                wRet = ERR_AGENT_SETNODEVALUE;
            }
        }
    }
    else
        wRet = ERR_AGENT_SETNODEVALUE;
	
    return wRet;   
} 

/*=============================================================================
函 数 名： GetEqpSvmpCfgByRow
功    能： 据配置行号获取Svmp信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpSvmpInfo * ptSvmpCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/   
u16 CCfgAgent::GetEqpSvmpCfgByRow( u8 byRow, TEqpSvmpInfo * ptSvmpCfg )
{
    if ( NULL == ptSvmpCfg )
        return ERR_AGENT_GETTABLEVALUE;
    
    if ( byRow <= m_dwSvmpEntryNum )
    {
        ptSvmpCfg->SetEqpId( m_atSvmpTable[byRow-1].GetEqpId() );
        ptSvmpCfg->SetMcuRecvPort( m_atSvmpTable[byRow-1].GetMcuRecvPort() );
        ptSvmpCfg->SetSwitchBrdId( m_atSvmpTable[byRow-1].GetSwitchBrdId() );
        ptSvmpCfg->SetRunBrdId( m_atSvmpTable[byRow-1].GetRunBrdId() );
        ptSvmpCfg->SetEqpRecvPort( m_atSvmpTable[byRow-1].GetEqpRecvPort() );
        ptSvmpCfg->SetAlias( m_atSvmpTable[byRow-1].GetAlias() );
        ptSvmpCfg->SetVmpType( m_atSvmpTable[byRow-1].GetVmpType() );

        return SUCCESS_AGENT;
	}
	
	return ERR_AGENT_EQPNOTEXIST; 
}     

/*=============================================================================
函 数 名： SetEqpSvmpCfgByRow
功    能： 据配置行号设置Svmp信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpSvmpInfo * ptSvmpCfg
 BOOL32 bIsNeedUpdatePortInTable 标识是否需要更新内存表中端口
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/ 
u16 CCfgAgent::SetEqpSvmpCfgByRow( u8 byRow, TEqpSvmpInfo * ptSvmpCfg,BOOL32 bIsNeedUpdatePortInTable )  
{
    BOOL32 bFlag = FALSE;
    u16 wRet = SUCCESS_AGENT;
    
    if ( byRow <= m_dwSvmpEntryNum )
    {
        m_atSvmpTable[byRow-1].SetEqpId( ptSvmpCfg->GetEqpId() );
		//如果是通过界面重新分配端口信息，把重新分配后的端口信息写入文件，但不记录在内存中。以防止用户不重启导致已工作
		//外设端口变化，无法正常工作[12/15/2011 chendaiwei]
		if(bIsNeedUpdatePortInTable)
		{
			m_atSvmpTable[byRow-1].SetMcuRecvPort( ptSvmpCfg->GetMcuRecvPort() );
		}
        m_atSvmpTable[byRow-1].SetSwitchBrdId( ptSvmpCfg->GetSwitchBrdId() );
        m_atSvmpTable[byRow-1].SetRunBrdId( ptSvmpCfg->GetRunBrdId() );
		
        // 外设配置重新写入时需再次从单板表里获取该外设的最新Ip
        m_atSvmpTable[byRow-1].SetIpAddr( GetBrdIpByAgtId(ptSvmpCfg->GetRunBrdId()) );		
        m_atSvmpTable[byRow-1].SetEqpRecvPort( ptSvmpCfg->GetEqpRecvPort() );
        m_atSvmpTable[byRow-1].SetAlias( ptSvmpCfg->GetAlias() );
		m_atSvmpTable[byRow-1].SetVmpType(ptSvmpCfg->GetVmpType());
        
        bFlag = ChangeBrdRunEqpNum( ptSvmpCfg->GetRunBrdId(), ptSvmpCfg->GetEqpId());
        if ( !bFlag )
            wRet = ERR_AGENT_SETNODEVALUE;
		
        if( SUCCESS_AGENT == wRet )
        {
            s8 achEntryStr[32];
            s8 achSvmpCfgInfo[512];
            memset(achEntryStr, '\0', sizeof(achEntryStr));
            memset(achSvmpCfgInfo, '\0', sizeof(achSvmpCfgInfo));
			
            GetEntryString( achEntryStr, byRow-1);
            sprintf(achSvmpCfgInfo, "%d\t%d\t%d\t%s\t%d\t%d\t%d", 
				m_atSvmpTable[byRow-1].GetEqpId(), 
				/*m_atSvmpTable[byRow-1].GetMcuRecvPort(),*/ptSvmpCfg->GetMcuRecvPort(),
				m_atSvmpTable[byRow-1].GetSwitchBrdId(), 
				m_atSvmpTable[byRow-1].GetAlias(), 
				m_atSvmpTable[byRow-1].GetRunBrdId(),
				m_atSvmpTable[byRow-1].GetEqpRecvPort(),
				m_atSvmpTable[byRow-1].GetVmpType());
            
            wRet = WriteTableEntry( m_achCfgName,SECTION_mcueqpSvmpTable, achEntryStr, achSvmpCfgInfo );
            if ( SUCCESS_AGENT != wRet )
            {
                Agtlog(LOG_ERROR, "[SetEqpSvmpCfgByRow] Fail to write the profile in Vmp cfg \n");
                wRet = ERR_AGENT_SETNODEVALUE;
            }
        }
	}
	else
		wRet = ERR_AGENT_SETNODEVALUE;
    
    return wRet;
}    

/*=============================================================================
函 数 名： GetEqpDvmpCfgByRow
功    能： 据配置行号获取Dvmp信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpDvmpBasicInfo * ptDvmpCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
// u16 CCfgAgent::GetEqpDvmpCfgByRow( u8 byRow, TEqpDvmpBasicInfo * ptDvmpCfg )
// {
//     if ( NULL == ptDvmpCfg )
//         return ERR_AGENT_GETTABLEVALUE;
//     
//     if ( byRow <= m_dwDvmpEntryNum )
//     {
//         ptDvmpCfg->SetEqpId( m_atDvmpBasicTable[byRow-1].GetEqpId() );
//         ptDvmpCfg->SetMcuRecvPort( m_atDvmpBasicTable[byRow-1].GetMcuRecvPort() );
//         ptDvmpCfg->SetSwitchBrdId( m_atDvmpBasicTable[byRow-1].GetSwitchBrdId() );
//         ptDvmpCfg->SetRunBrdId( m_atDvmpBasicTable[byRow-1].GetRunBrdId() );
//         ptDvmpCfg->SetEqpRecvPort( m_atDvmpBasicTable[byRow-1].GetEqpRecvPort() );
//         ptDvmpCfg->SetAlias( m_atDvmpBasicTable[byRow-1].GetAlias() );
//         
//         return SUCCESS_AGENT;
//     }
// 
//     return ERR_AGENT_EQPNOTEXIST; 
// }     

/*=============================================================================
函 数 名： SetEqpDvmpCfgByRow
功    能： 据配置行号设置Dvmp信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpDvmpBasicInfo * ptDvmpCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
// u16 CCfgAgent::SetEqpDvmpCfgByRow( u8 byRow, TEqpDvmpBasicInfo * ptDvmpCfg )  
// {
// 	BOOL32 bFlag = FALSE;
//     u16 wRet = SUCCESS_AGENT;
//     
//     if ( byRow <= m_dwDvmpEntryNum )
//     {
//         m_atDvmpBasicTable[byRow-1].SetEqpId( ptDvmpCfg->GetEqpId() );
//         m_atDvmpBasicTable[byRow-1].SetMcuRecvPort( ptDvmpCfg->GetMcuRecvPort() );
//         m_atDvmpBasicTable[byRow-1].SetSwitchBrdId( ptDvmpCfg->GetSwitchBrdId() );
//         m_atDvmpBasicTable[byRow-1].SetRunBrdId( ptDvmpCfg->GetRunBrdId() );
// 		
//         // 外设配置重新写入时需再次从单板表里获取该外设的最新Ip
//         m_atDvmpBasicTable[byRow-1].SetIpAddr( GetBrdIpByAgtId(ptDvmpCfg->GetRunBrdId()) );		
//         m_atDvmpBasicTable[byRow-1].SetEqpRecvPort( ptDvmpCfg->GetEqpRecvPort() );
//         m_atDvmpBasicTable[byRow-1].SetAlias( ptDvmpCfg->GetAlias() );
//         
//         bFlag = ChangeBrdRunEqpNum( ptDvmpCfg->GetRunBrdId(), ptDvmpCfg->GetEqpId());
//         if ( !bFlag )
//             wRet = ERR_AGENT_SETNODEVALUE;
//         
//         if( SUCCESS_AGENT == wRet )
//         {
//             s8 achUsedMap[32];
//             s8 achEntryStr[32];
//             s8 achDvmpCfgInfo[512];
//             memset(achUsedMap, '\0', sizeof(achUsedMap));
//             memset(achEntryStr, '\0', sizeof(achEntryStr));
//             memset(achDvmpCfgInfo, '\0', sizeof(achDvmpCfgInfo));
// 			
//             GetEntryString( achEntryStr, byRow-1);
//             sprintf(achDvmpCfgInfo, "%d\t%d\t%d\t%s\t%d\t%d", 
// 				m_atDvmpBasicTable[byRow-1].GetEqpId(), 
// 				m_atDvmpBasicTable[byRow-1].GetMcuRecvPort(),
// 				m_atDvmpBasicTable[byRow-1].GetSwitchBrdId(), 
// 				m_atDvmpBasicTable[byRow-1].GetAlias(), 
// 				m_atDvmpBasicTable[byRow-1].GetRunBrdId(),
// 				m_atDvmpBasicTable[byRow-1].GetEqpRecvPort());
//             
//             wRet = WriteTableEntry( m_achCfgName,SECTION_mcueqpDvmpTable, achEntryStr, achDvmpCfgInfo );
//             if ( SUCCESS_AGENT != wRet )
//             {
//                 Agtlog(LOG_ERROR, "[SetEqpDvmpCfgByRow] Fail to write the profile in Vmp cfg \n");
//                 wRet = ERR_AGENT_SETNODEVALUE;
//             }
//         }
//     }
//     else
//         wRet = ERR_AGENT_SETNODEVALUE;
//     
//     return wRet;
// }  

/*=============================================================================
函 数 名： GetEqpMpuBasCfgByRow
功    能： 据配置行号获取MpuBas信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpMpuBasInfo * ptMpuBasCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
u16 CCfgAgent::GetEqpMpuBasCfgByRow( u8 byRow, TEqpMpuBasInfo * ptMpuBasCfg ) 
{
    if ( NULL == ptMpuBasCfg )
        return ERR_AGENT_GETTABLEVALUE;
    
    if ( byRow <= m_dwMpuBasEntryNum && byRow >= 1)
    {
        ptMpuBasCfg->SetEqpId( m_atMpuBasTable[byRow-1].GetEqpId() );
        ptMpuBasCfg->SetMcuRecvPort( m_atMpuBasTable[byRow-1].GetMcuRecvPort() );
        ptMpuBasCfg->SetSwitchBrdId( m_atMpuBasTable[byRow-1].GetSwitchBrdId() );
        ptMpuBasCfg->SetEqpRecvPort( m_atMpuBasTable[byRow-1].GetEqpRecvPort() );
        ptMpuBasCfg->SetRunBrdId( m_atMpuBasTable[byRow-1].GetRunBrdId() );
        ptMpuBasCfg->SetAlias( m_atMpuBasTable[byRow-1].GetAlias() );
		ptMpuBasCfg->SetStartMode(m_atMpuBasTable[byRow-1].GetStartMode());
        
        return SUCCESS_AGENT;
    }
    return ERR_AGENT_EQPNOTEXIST; 
}     

/*=============================================================================
函 数 名： SetEqpMpuBasCfgByRow
功    能： 据配置行号设置MpuBas信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpMpuBasInfo * ptMpuBasCfg
 BOOL32 bIsNeedUpdatePortInTable 标识是否需要更新内存表中端口
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
u16 CCfgAgent::SetEqpMpuBasCfgByRow( u8 byRow, TEqpMpuBasInfo * ptMpuBasCfg, BOOL32 bIsNeedUpdatePortInTable ) 
{
    BOOL32 bFlag = FALSE;
    u16 wRet = SUCCESS_AGENT;
    
    if ( byRow <= m_dwMpuBasEntryNum && byRow >= 1 )
    {
        m_atMpuBasTable[byRow-1].SetEqpId( ptMpuBasCfg->GetEqpId() );
		//如果是通过界面重新分配端口信息，把重新分配后的端口信息写入文件，但不记录在内存中。以防止用户不重启导致已工作
		//外设端口变化，无法正常工作[12/15/2011 chendaiwei]
		if(bIsNeedUpdatePortInTable)
		{
			m_atMpuBasTable[byRow-1].SetMcuRecvPort( ptMpuBasCfg->GetMcuRecvPort() );
		}
        m_atMpuBasTable[byRow-1].SetSwitchBrdId( ptMpuBasCfg->GetSwitchBrdId() );
        m_atMpuBasTable[byRow-1].SetEqpRecvPort( ptMpuBasCfg->GetEqpRecvPort() );
        m_atMpuBasTable[byRow-1].SetRunBrdId( ptMpuBasCfg->GetRunBrdId() );
		m_atMpuBasTable[byRow-1].SetStartMode(ptMpuBasCfg->GetStartMode());
		
        // 外设配置重新写入时需再次从单板表里获取该外设的最新Ip
        m_atMpuBasTable[byRow-1].SetIpAddr( GetBrdIpByAgtId(ptMpuBasCfg->GetRunBrdId()) );
        m_atMpuBasTable[byRow-1].SetAlias( ptMpuBasCfg->GetAlias() );
		
		bFlag = ChangeBrdRunEqpNum( ptMpuBasCfg->GetRunBrdId(), ptMpuBasCfg->GetEqpId() );
        if ( !bFlag )
            wRet = ERR_AGENT_SETNODEVALUE;
        
        if ( SUCCESS_AGENT == wRet )
        {
            s8 achMpuBasCfgInfo[512];
            s8 achEntryStr[32];
            s8 achMapStr[32];
            memset( achMapStr, 0, sizeof(achMapStr) );
            memset(achEntryStr, '\0', sizeof(achEntryStr));
            memset(achMpuBasCfgInfo, '\0', sizeof(achMpuBasCfgInfo));
			
			m_atMpuBasTable[byRow-1].GetUsedMapStr( achMapStr );
            GetEntryString(achEntryStr, byRow-1);
            sprintf(achMpuBasCfgInfo, "   %d\t%d\t%d\t%s\t%d\t%d\t%d", 
				m_atMpuBasTable[byRow-1].GetEqpId(),
				/*m_atMpuBasTable[byRow-1].GetMcuRecvPort(),*/ptMpuBasCfg->GetMcuRecvPort(),
				m_atMpuBasTable[byRow-1].GetSwitchBrdId(),
				m_atMpuBasTable[byRow-1].GetAlias(),
				m_atMpuBasTable[byRow-1].GetRunBrdId(),
				m_atMpuBasTable[byRow-1].GetEqpRecvPort(),
				m_atMpuBasTable[byRow-1].GetStartMode());
            
            wRet = WriteTableEntry( m_achCfgName, SECTION_mcueqpMpuBasTable, 
				achEntryStr, achMpuBasCfgInfo );
            if ( SUCCESS_AGENT != wRet )
            {
                Agtlog(LOG_ERROR, "[SetEqpMpuBasCfgByRow] Fail to write the profile in %s\n", 
					SECTION_mcueqpMpuBasTable );
                wRet = ERR_AGENT_SETNODEVALUE;
            }
        }
    }
    else
        wRet = ERR_AGENT_SETNODEVALUE;
    
    return wRet;
	
}      

/*=============================================================================
函 数 名： GetEqpEbapCfgByRow
功    能： 据配置行号获取Ebap信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpEbapInfo * ptEbapCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
// u16 CCfgAgent::GetEqpEbapCfgByRow( u8 byRow, TEqpEbapInfo * ptEbapCfg ) 
// {
//     if ( NULL == ptEbapCfg )
//         return ERR_AGENT_GETTABLEVALUE;
//     
//     if ( byRow <= m_dwEbapEntryNum )
//     {
//         ptEbapCfg->SetEqpId( m_atEbapTable[byRow-1].GetEqpId() );
//         ptEbapCfg->SetMcuRecvPort( m_atEbapTable[byRow-1].GetMcuRecvPort() );
//         ptEbapCfg->SetSwitchBrdId( m_atEbapTable[byRow-1].GetSwitchBrdId() );
//         ptEbapCfg->SetEqpRecvPort( m_atEbapTable[byRow-1].GetEqpRecvPort() );
//         ptEbapCfg->SetRunBrdId( m_atEbapTable[byRow-1].GetRunBrdId() );
//         ptEbapCfg->SetAlias( m_atEbapTable[byRow-1].GetAlias() );
//         
//         return SUCCESS_AGENT;
//     }
// 
//     return ERR_AGENT_EQPNOTEXIST; 
// }  

/*=============================================================================
函 数 名： SetEqpEbapCfgByRow
功    能： 据配置行号设置Ebap信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpEbapInfo * ptEbapCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/   
// u16 CCfgAgent::SetEqpEbapCfgByRow( u8 byRow, TEqpEbapInfo * ptEbapCfg ) 
// {
//     BOOL32 bFlag = FALSE;
//     u16 wRet = SUCCESS_AGENT;
//     
//     if ( byRow <= m_dwEbapEntryNum )
//     {
//         m_atEbapTable[byRow-1].SetEqpId( ptEbapCfg->GetEqpId() );
//         m_atEbapTable[byRow-1].SetMcuRecvPort( ptEbapCfg->GetMcuRecvPort() );
//         m_atEbapTable[byRow-1].SetSwitchBrdId( ptEbapCfg->GetSwitchBrdId() );
//         m_atEbapTable[byRow-1].SetEqpRecvPort( ptEbapCfg->GetEqpRecvPort() );
//         m_atEbapTable[byRow-1].SetRunBrdId( ptEbapCfg->GetRunBrdId() );
// 		
//         // 外设配置重新写入时需再次从单板表里获取该外设的最新Ip
//         m_atEbapTable[byRow-1].SetIpAddr( GetBrdIpByAgtId(ptEbapCfg->GetRunBrdId()) );		
//         m_atEbapTable[byRow-1].SetAlias( ptEbapCfg->GetAlias() );
//         
//         bFlag = ChangeBrdRunEqpNum( ptEbapCfg->GetRunBrdId(), ptEbapCfg->GetEqpId() );
//         if ( !bFlag )
//             wRet = ERR_AGENT_SETNODEVALUE;
//         
//         if ( SUCCESS_AGENT == wRet )
//         {
//             s8 achEbapCfgInfo[512];
//             s8 achEntryStr[32];
//             s8 achMapStr[32];
//             memset( achMapStr, 0, sizeof(achMapStr) );
//             memset(achEntryStr, '\0', sizeof(achEntryStr));
//             memset(achEbapCfgInfo, '\0', sizeof(achEbapCfgInfo));
// 			
//             m_atEbapTable[byRow-1].GetUsedMapStr( achMapStr );
//             GetEntryString(achEntryStr, byRow-1);
//             sprintf(achEbapCfgInfo, "   %d\t%d\t%d\t%s\t%d\t%d", 
// 				m_atEbapTable[byRow-1].GetEqpId(),
// 				m_atEbapTable[byRow-1].GetMcuRecvPort(),
// 				m_atEbapTable[byRow-1].GetSwitchBrdId(),
// 				m_atEbapTable[byRow-1].GetAlias(),
// 				m_atEbapTable[byRow-1].GetRunBrdId(),
// 				m_atEbapTable[byRow-1].GetEqpRecvPort());
//             
//             wRet = WriteTableEntry( m_achCfgName, SECTION_mcueqpEbapTable, 
// 				achEntryStr, achEbapCfgInfo );
//             if ( SUCCESS_AGENT != wRet )
//             {
//                 Agtlog(LOG_ERROR, "[SetEqpMpuBasCfgByRow] Fail to write the profile in %s\n", 
// 					SECTION_mcueqpEbapTable );
//                 wRet = ERR_AGENT_SETNODEVALUE;
//             }
//         }
//     }
//     else
//         wRet = ERR_AGENT_SETNODEVALUE;
//     
//     return wRet;
// }     

/*=============================================================================
函 数 名： GetEqpEvpuCfgByRow
功    能： 据配置行号获取Evpu信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpEvpuInfo * ptEvpuCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/  
// u16 CCfgAgent::GetEqpEvpuCfgByRow( u8 byRow, TEqpEvpuInfo * ptEvpuCfg )  
// {
//     if ( NULL == ptEvpuCfg )
//         return ERR_AGENT_GETTABLEVALUE;
//     
//     if ( byRow <= m_dwEvpuEntryNum )
//     {
//         ptEvpuCfg->SetEqpId( m_atEvpuTable[byRow-1].GetEqpId() );
//         ptEvpuCfg->SetMcuRecvPort( m_atEvpuTable[byRow-1].GetMcuRecvPort() );
//         ptEvpuCfg->SetSwitchBrdId( m_atEvpuTable[byRow-1].GetSwitchBrdId() );
//         ptEvpuCfg->SetRunBrdId( m_atEvpuTable[byRow-1].GetRunBrdId() );
//         ptEvpuCfg->SetEqpRecvPort( m_atEvpuTable[byRow-1].GetEqpRecvPort() );
//         ptEvpuCfg->SetAlias( m_atEvpuTable[byRow-1].GetAlias() );
// 
// 		
//         return SUCCESS_AGENT;
//     }
// 
//     return ERR_AGENT_EQPNOTEXIST; 
// }   

/*=============================================================================
函 数 名： SetEqpEvpuCfgByRow
功    能： 据配置行号设置Evpu信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpEvpuInfo * ptEvpuCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/ 
// u16 CCfgAgent::SetEqpEvpuCfgByRow( u8 byRow, TEqpEvpuInfo * ptEvpuCfg )  
// {
//     BOOL32 bFlag = FALSE;
//     u16 wRet = SUCCESS_AGENT;
//     
//     if ( byRow <= m_dwEvpuEntryNum )
//     {
//         m_atEvpuTable[byRow-1].SetEqpId( ptEvpuCfg->GetEqpId() );
//         m_atEvpuTable[byRow-1].SetMcuRecvPort( ptEvpuCfg->GetMcuRecvPort() );
//         m_atEvpuTable[byRow-1].SetSwitchBrdId( ptEvpuCfg->GetSwitchBrdId() );
//         m_atEvpuTable[byRow-1].SetRunBrdId( ptEvpuCfg->GetRunBrdId() );
// 		
//         // 外设配置重新写入时需再次从单板表里获取该外设的最新Ip
//         m_atEvpuTable[byRow-1].SetIpAddr( GetBrdIpByAgtId(ptEvpuCfg->GetRunBrdId()) );		
//         m_atEvpuTable[byRow-1].SetEqpRecvPort( ptEvpuCfg->GetEqpRecvPort() );
//         m_atEvpuTable[byRow-1].SetAlias( ptEvpuCfg->GetAlias() );
//         
//         bFlag = ChangeBrdRunEqpNum( ptEvpuCfg->GetRunBrdId(), ptEvpuCfg->GetEqpId());
//         if ( !bFlag )
//             wRet = ERR_AGENT_SETNODEVALUE;
// 
// 		
//         if( SUCCESS_AGENT == wRet )
//         {
//             s8 achUsedMap[32];
//             s8 achEntryStr[32];
//             s8 achEvpuCfgInfo[512];
//             memset(achUsedMap, '\0', sizeof(achUsedMap));
//             memset(achEntryStr, '\0', sizeof(achEntryStr));
//             memset(achEvpuCfgInfo, '\0', sizeof(achEvpuCfgInfo));
// 			
//             GetEntryString( achEntryStr, byRow-1);
//             sprintf(achEvpuCfgInfo, "%d\t%d\t%d\t%s\t%d\t%d", 
// 				m_atEvpuTable[byRow-1].GetEqpId(), 
// 				m_atEvpuTable[byRow-1].GetMcuRecvPort(),
// 				m_atEvpuTable[byRow-1].GetSwitchBrdId(), 
// 				m_atEvpuTable[byRow-1].GetAlias(), 
// 				m_atEvpuTable[byRow-1].GetRunBrdId(),
// 				m_atEvpuTable[byRow-1].GetEqpRecvPort() );
//             
//             wRet = WriteTableEntry( m_achCfgName,SECTION_mcueqpEvpuTable, achEntryStr, achEvpuCfgInfo );
//             if ( SUCCESS_AGENT != wRet )
//             {
//                 Agtlog(LOG_ERROR, "[SetEqpEvpuCfgByRow] Fail to write the profile in Vmp cfg \n");
//                 wRet = ERR_AGENT_SETNODEVALUE;
//             }
//         }
//     }
//     else
//         wRet = ERR_AGENT_SETNODEVALUE;
//     
//     return wRet;
// }   

//4.6版本 新加外设  jlb
/*=============================================================================
函 数 名： GetEqpHduCfgById
功    能： 据ID号获取Hdu信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpHduInfo * ptHduCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
u16 CCfgAgent::GetEqpHduCfgById( u8 byId, TEqpHduInfo * ptHduCfg )
{
    if( NULL == ptHduCfg || byId < HDUID_MIN || byId > HDUID_MAX )
    {
        Agtlog(LOG_ERROR, "[GetEqpHduCfgById] param err: byId<%d>, ptHduCfg: 0x%x\n", byId, ptHduCfg);
        return ERR_AGENT_GETTABLEVALUE;
    }
    u16 wRet = ERR_AGENT_EQPNOTEXIST;
	
    for( u8 byRow = 1; byRow <= m_dwHduEntryNum; byRow ++ )
    {
        if( m_atHduTable[byRow-1].GetEqpId() == byId )
        {
            wRet = GetEqpHduCfgByRow( byRow, ptHduCfg);
            break;
        }
    }
    if( SUCCESS_AGENT != wRet )
        Agtlog(LOG_ERROR, "[GetEqpHduCfgById] The Tv ID.%d is not exist,please check it\n", byId);
	
    return wRet;
}

/*=============================================================================
函 数 名： SetEqpHduCfgById
功    能： 据ID号设置Hdu信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpHduInfo * ptHduCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
u16 CCfgAgent::SetEqpHduCfgById( u8 byId, TEqpHduInfo * ptHduCfg )
{
    u16 wRet = ERR_AGENT_SETNODEVALUE;
	
    if( NULL == ptHduCfg || byId < HDUID_MIN || byId > HDUID_MAX )
    {
        Agtlog(LOG_ERROR, "[SetEqpTVWallCfg] param err: byId: %d, ptHduCfg: 0x%x !\n", byId, ptHduCfg);
        return wRet;
    }
	
    for(u8 byRow = 1; byRow <= m_dwHduEntryNum; byRow++)
    {
        if( byId == m_atHduTable[byRow-1].GetEqpId() )
        {
            wRet = SetEqpHduCfgByRow( byRow, ptHduCfg );
            break;
        }
    }
    if( SUCCESS_AGENT != wRet )
        Agtlog(LOG_ERROR, "[SetEqpHduCfgById] set tvwall<%d> config failed !\n", byId);
	
    return wRet;
}

/*=============================================================================
函 数 名： GetEqpSvmpCfgById
功    能： 据ID号获取Svmp信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpSvmpInfo * ptSvmpCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
u16 CCfgAgent::GetEqpSvmpCfgById( u8 byId, TEqpSvmpInfo * ptSvmpCfg )
{
    if( NULL == ptSvmpCfg || byId < VMPID_MIN || byId > VMPID_MAX )
    {
        Agtlog(LOG_ERROR, "[GetEqpBasCfg] param err: byId<%d>, ptCfg: 0x%x\n", byId, ptSvmpCfg);
        return ERR_AGENT_GETTABLEVALUE;
    }
    u16 wRet = ERR_AGENT_EQPNOTEXIST;
	
    for( u8 byRow = 1; byRow <= m_dwSvmpEntryNum; byRow++)
    {
        if( m_atSvmpTable[byRow-1].GetEqpId() == byId )
        {   
            wRet = GetEqpSvmpCfgByRow( byRow, ptSvmpCfg );
            break;
        }
    }
    if( SUCCESS_AGENT != wRet )
        Agtlog(LOG_ERROR, "[GetEqpSvmpCfgById] The vmp ID.%d is not exist,please check it\n", byId);
	
    return wRet;
}

/*=============================================================================
函 数 名： SetEqpSvmpCfgById
功    能： 据ID号设置Svmp信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpSvmpInfo * ptSvmpCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
u16 CCfgAgent::SetEqpSvmpCfgById( u8 byId, TEqpSvmpInfo * ptSvmpCfg )
{
    u16 wRet = ERR_AGENT_SETNODEVALUE;
	
	if( NULL == ptSvmpCfg || byId < VMPID_MIN || byId > VMPID_MAX )
    {
        Agtlog(LOG_ERROR, "[SetEqpSvmpCfgById] param err: byId: %d, ptBasCfg: 0x%x !\n", byId, ptSvmpCfg);
        return wRet;
    }
    
    for( u8 byRow = 1; byRow <= m_dwSvmpEntryNum; byRow ++ )
    {
        if( byId == m_atSvmpTable[byRow-1].GetEqpId() )
        {
            wRet = SetEqpSvmpCfgByRow( byRow, ptSvmpCfg );
            break;
        }
    }
    if( SUCCESS_AGENT != wRet )
        Agtlog(LOG_ERROR, "[SetEqpSvmpCfgById] set vmp<%d> config failed !\n", byId);
	
    return wRet;
}

/*=============================================================================
函 数 名： GetEqpDvmpCfgById
功    能： 据ID号获取pDvmp信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpDvmpBasicInfo * ptDvmpCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
// u16 CCfgAgent::GetEqpDvmpCfgById( u8 byId, TEqpDvmpBasicInfo * ptDvmpCfg )
// {
//     if( NULL == ptDvmpCfg || byId < VMPID_MIN || byId > VMPID_MAX )
//     {
//         Agtlog(LOG_ERROR, "[GetEqpDvmpCfgById] param err: byId<%d>, ptDvmpCfg: 0x%x\n", byId, ptDvmpCfg);
//         return ERR_AGENT_GETTABLEVALUE;
//     }
//     u16 wRet = ERR_AGENT_EQPNOTEXIST;
// 	
//     for( u8 byRow = 1; byRow <= m_dwDvmpEntryNum; byRow++)
//     {
//         if( m_atDvmpBasicTable[byRow-1].GetEqpId() == byId )
//         {   
//             wRet = GetEqpDvmpCfgByRow( byRow, ptDvmpCfg );
//             break;
//         }
//     }
//     if( SUCCESS_AGENT != wRet )
//         Agtlog(LOG_ERROR, "[GetEqpDvmpCfgById] The vmp ID.%d is not exist,please check it\n", byId);
// 	
//     return wRet;
// }

/*=============================================================================
函 数 名： SetEqpDvmpCfgById
功    能： 据ID号设置Dvmp信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpDvmpBasicInfo * ptDvmpCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
// u16 CCfgAgent::SetEqpDvmpCfgById( u8 byId, TEqpDvmpBasicInfo * ptDvmpCfg )
// {
//     u16 wRet = ERR_AGENT_SETNODEVALUE;
// 	
// 	if( NULL == ptDvmpCfg || byId < VMPID_MIN || byId > VMPID_MAX )
//     {
//         Agtlog(LOG_ERROR, "[SetEqpDvmpCfgById] param err: byId: %d, ptDvmpCfg: 0x%x !\n", byId, ptDvmpCfg);
//         return wRet;
//     }
//     
//     for( u8 byRow = 1; byRow <= m_dwDvmpEntryNum; byRow ++ )
//     {
//         if( byId == m_atDvmpBasicTable[byRow-1].GetEqpId() )
//         {
//             wRet = SetEqpDvmpCfgByRow( byRow, ptDvmpCfg );
//             break;
//         }
//     }
//     if( SUCCESS_AGENT != wRet )
//         Agtlog(LOG_ERROR, "[SetEqpDvmpCfgById] set vmp<%d> config failed !\n", byId);
// 	
//     return wRet;
// }

/*=============================================================================
函 数 名： GetEqpMpuBasCfgById
功    能： 据ID号获取MpuBas信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpMpuBasInfo * ptMpuBasCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
u16 CCfgAgent::GetEqpMpuBasCfgById( u8 byId, TEqpMpuBasInfo * ptMpuBasCfg )
{
    if( NULL == ptMpuBasCfg || byId < BASID_MIN || byId > BASID_MAX )
    {
        Agtlog(LOG_ERROR, "[GetEqpMpuBasCfgById] param err: byId<%d>, ptMpuBasCfg: 0x%x\n", byId, ptMpuBasCfg);
        return ERR_AGENT_GETTABLEVALUE;
    }
	
    u16 wRet = ERR_AGENT_EQPNOTEXIST;
    for( u8 byRow = 1; byRow <= (u8)m_dwMpuBasEntryNum; byRow++)
    {
        if( m_atMpuBasTable[byRow-1].GetEqpId() == byId )
        {   
            wRet = GetEqpMpuBasCfgByRow( byRow, ptMpuBasCfg );
            break;
        }
    }
    
    if ( SUCCESS_AGENT != wRet )
        Agtlog(LOG_ERROR, "[GetEqpMpuBasCfgById] The bas ID.%d is not exist,please check it\n", byId);
    
    return wRet;
}

/*=============================================================================
函 数 名： SetEqpMpuBasCfgById
功    能： 据ID号设置MpuBas信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpMpuBasInfo * ptMpuBasCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
u16 CCfgAgent::SetEqpMpuBasCfgById( u8 byId, TEqpMpuBasInfo * ptMpuBasCfg )
{
    u16 wRet = ERR_AGENT_SETNODEVALUE;
	
    if( NULL == ptMpuBasCfg || byId < BASID_MIN || byId > BASID_MAX )
    {
        Agtlog(LOG_ERROR, "[SetEqpMpuBasCfgById] param err: byId: %d, ptMpuBasCfg: 0x%x !\n", byId, ptMpuBasCfg);
        return wRet;
    }
    
    for( u8 byRow = 1; byRow <= m_dwMpuBasEntryNum; byRow ++ )
    {
        if( byId == m_atMpuBasTable[byRow-1].GetEqpId() )
        {
            wRet = SetEqpMpuBasCfgByRow( byRow, ptMpuBasCfg );
            break;
        }
    }
    if( SUCCESS_AGENT != wRet )
        Agtlog(LOG_ERROR, "[SetEqpMpuBasCfgById] set bas<%d> config failed !\n", byId);
	
    return wRet;
}

/*=============================================================================
函 数 名： GetEqpEbapCfgById
功    能： 据ID号获取Ebap信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpMpuBasInfo * ptMpuBasCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
// u16 CCfgAgent::GetEqpEbapCfgById( u8 byId, TEqpEbapInfo * ptEbapCfg )
// {
//     if( NULL == ptEbapCfg || byId < BASID_MIN || byId > BASID_MAX )
//     {
//         Agtlog(LOG_ERROR, "[GetEqpEbapCfgById] param err: byId<%d>, ptMpuBasCfg: 0x%x\n", byId, ptEbapCfg);
//         return ERR_AGENT_GETTABLEVALUE;
//     }
// 	
//     u16 wRet = ERR_AGENT_EQPNOTEXIST;
//     for( u8 byRow = 1; byRow <= (u8)m_dwEbapEntryNum; byRow++)
//     {
//         if( m_atEbapTable[byRow-1].GetEqpId() == byId )
//         {   
//             wRet = GetEqpEbapCfgByRow( byRow, ptEbapCfg );
//             break;
//         }
//     }
//     
//     if ( SUCCESS_AGENT != wRet )
//         Agtlog(LOG_ERROR, "[GetEqpEbapCfgById] The bas ID.%d is not exist,please check it\n", byId);
//     
//     return wRet;
// }

/*=============================================================================
函 数 名： SetEqpEbapCfgById
功    能： 据ID号设置Ebap信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpEbapInfo * ptEbapCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
// u16 CCfgAgent::SetEqpEbapCfgById( u8 byId, TEqpEbapInfo * ptEbapCfg )
// {
//     u16 wRet = ERR_AGENT_SETNODEVALUE;
// 	
//     if( NULL == ptEbapCfg || byId < BASID_MIN || byId > BASID_MAX )
//     {
//         Agtlog(LOG_ERROR, "[SetEqpEbapCfgById] param err: byId: %d, ptEbapCfg: 0x%x !\n", byId, ptEbapCfg);
//         return wRet;
//     }
//     
//     for( u8 byRow = 1; byRow <= m_dwEbapEntryNum; byRow ++ )
//     {
//         if( byId == m_atEbapTable[byRow-1].GetEqpId() )
//         {
//             wRet = SetEqpEbapCfgByRow( byRow, ptEbapCfg );
//             break;
//         }
//     }
//     if( SUCCESS_AGENT != wRet )
//         Agtlog(LOG_ERROR, "[SetEqpEbapCfgById] set bas<%d> config failed !\n", byId);
// 	
//     return wRet;
// }

/*=============================================================================
函 数 名： GetEqpEvpuCfgById
功    能： 据ID号获取Evpu信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpEvpuInfo * ptEvpuCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
// u16 CCfgAgent::GetEqpEvpuCfgById( u8 byId, TEqpEvpuInfo * ptEvpuCfg )
// {
// 	if( NULL == ptEvpuCfg || byId < VMPID_MIN || byId > VMPID_MAX )
//     {
//         Agtlog(LOG_ERROR, "[GetEqpEvpuCfgById] param err: byId<%d>, ptEvpuCfg: 0x%x\n", byId, ptEvpuCfg);
//         return ERR_AGENT_GETTABLEVALUE;
//     }
//     u16 wRet = ERR_AGENT_EQPNOTEXIST;
// 	
//     for( u8 byRow = 1; byRow <= m_dwEvpuEntryNum; byRow++)
//     {
//         if( m_atEvpuTable[byRow-1].GetEqpId() == byId )
//         {   
//             wRet = GetEqpEvpuCfgByRow( byRow, ptEvpuCfg );
//             break;
//         }
//     }
//     if( SUCCESS_AGENT != wRet )
//         Agtlog(LOG_ERROR, "[GetEqpEvpuCfgById] The vmp ID.%d is not exist,please check it\n", byId);
// 	
//     return wRet;
// }

/*=============================================================================
函 数 名： SetEqpEvpuCfgById
功    能： 据ID号设置Evpu信息
算法实现： 
全局变量： 
参    数： u8 byRow,
TEqpEvpuInfo * ptEvpuCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
// u16 CCfgAgent::SetEqpEvpuCfgById( u8 byId, TEqpEvpuInfo * ptEvpuCfg )
// {
//     u16 wRet = ERR_AGENT_SETNODEVALUE;
// 	
// 	if( NULL == ptEvpuCfg || byId < VMPID_MIN || byId > VMPID_MAX )
//     {
//         Agtlog(LOG_ERROR, "[SetEqpEvpuCfgById] param err: byId: %d, ptEvpuCfg: 0x%x !\n", byId, ptEvpuCfg);
//         return wRet;
//     }
//     
//     for( u8 byRow = 1; byRow <= m_dwEvpuEntryNum; byRow ++ )
//     {
//         if( byId == m_atEvpuTable[byRow-1].GetEqpId() )
//         {
//             wRet = SetEqpEvpuCfgByRow( byRow, ptEvpuCfg );
//             break;
//         }
//     }
//     if( SUCCESS_AGENT != wRet )
//         Agtlog(LOG_ERROR, "[SetEqpEvpuCfgById] set vmp<%d> config failed !\n", byId);
// 	
//     return wRet;
// }

/*=============================================================================
函 数 名： GetBrdCfgById
功    能： 
算法实现： 
全局变量： 
参    数： u8 byRo
TBoardInfo* ptBoardInfo
返 回 值： u16 
=============================================================================*/
u16 CCfgAgent::GetBrdCfgById(u8 byBrdId, TBoardInfo* ptBoardInfo)
{
    if( NULL == ptBoardInfo || byBrdId == 0 )
    {
        Agtlog(LOG_ERROR, "[GetBrdCfgById] param err: byId.%d, ptInfo.0x%x \n", byBrdId, ptBoardInfo );
        return ERR_AGENT_GETTABLEVALUE;
    }
	u16 wRet = ERR_AGENT_EQPNOTEXIST;
	
    for( u8 byIdx = 0; byIdx < m_dwBrdCfgEntryNum; byIdx++)
    {
        if( m_atBrdCfgTable[byIdx].GetBrdId() == byBrdId )
        {
            ptBoardInfo->SetBrdId( m_atBrdCfgTable[byIdx].GetBrdId() );
            ptBoardInfo->SetLayer( m_atBrdCfgTable[byIdx].GetLayer() );
            ptBoardInfo->SetSlot( m_atBrdCfgTable[byIdx].GetSlot() );
            ptBoardInfo->SetType( m_atBrdCfgTable[byIdx].GetType() );
            ptBoardInfo->SetBrdIp( m_atBrdCfgTable[byIdx].GetBrdIp() );
            ptBoardInfo->SetPortChoice( m_atBrdCfgTable[byIdx].GetPortChoice() );
            ptBoardInfo->SetCastChoice( m_atBrdCfgTable[byIdx].GetCastChoice() );
            ptBoardInfo->SetState( m_atBrdCfgTable[byIdx].GetState() );
            ptBoardInfo->SetOSType( m_atBrdCfgTable[byIdx].GetOSType() );
            ptBoardInfo->SetAlias( m_atBrdCfgTable[byIdx].GetAlias() );
            
            u8 byPeriNum = 0;
            u8 abyPeriId[MAXNUM_BRD_EQP];
            memset( abyPeriId, 0, sizeof(abyPeriId) );
            m_atBrdCfgTable[byIdx].GetPeriId( abyPeriId, &byPeriNum );
            ptBoardInfo->SetPeriId( abyPeriId, byPeriNum );
            
            wRet = SUCCESS_AGENT;
            break;
        }
    }
    if( SUCCESS_AGENT != wRet )
    {
        Agtlog(LOG_ERROR, "[GetBrdCfgById] The board ID:%d is not exist,please check it\n", byBrdId);
    }
    return wRet;
}


/*=============================================================================
函 数 名： SetBrdCfgById
功    能： 
算法实现： 
全局变量： 
参    数： u8 byIndex
TBoardInfo * ptBoardInfo
返 回 值： u16 
=============================================================================*/
u16 CCfgAgent::SetBrdCfgById( u8 byBrdId, TBoardInfo * ptBrdInfo )
{
    if( byBrdId == 0 )
    {
        Agtlog(LOG_ERROR, "[SetBrdCfgById] param err: byId.%d \n", byBrdId );
        return ERR_AGENT_GETTABLEVALUE;
    }
    u16 wRet = ERR_AGENT_EQPNOTEXIST;
	
    for( u8 byIdx = 0; byIdx < m_dwBrdCfgEntryNum; byIdx++)
    {
        if( m_atBrdCfgTable[byIdx].GetBrdId() == byBrdId )
        {           
            // 清除在线状态
            m_atBrdCfgTable[byIdx].SetState( BOARD_STATUS_DISCONNECT );
            
            m_atBrdCfgTable[byIdx].SetBrdId( ptBrdInfo->GetBrdId() );
            m_atBrdCfgTable[byIdx].SetLayer( ptBrdInfo->GetLayer() );
            m_atBrdCfgTable[byIdx].SetSlot( ptBrdInfo->GetSlot() );
            m_atBrdCfgTable[byIdx].SetType( ptBrdInfo->GetType() );
            m_atBrdCfgTable[byIdx].SetCastChoice( ptBrdInfo->GetCastChoice() );
            m_atBrdCfgTable[byIdx].SetPortChoice( ptBrdInfo->GetPortChoice() );
            m_atBrdCfgTable[byIdx].SetBrdIp( ptBrdInfo->GetBrdIp() );			
            m_atBrdCfgTable[byIdx].SetOSType( ptBrdInfo->GetOSType() );
            m_atBrdCfgTable[byIdx].SetAlias( ptBrdInfo->GetAlias() );
			
            u8 byPeriNum = 0;
            u8 abyPeriId[MAXNUM_BRD_EQP];
            memset( abyPeriId, 0, sizeof(abyPeriId) );
            ptBrdInfo->GetPeriId( abyPeriId, &byPeriNum );
            m_atBrdCfgTable[byIdx].SetPeriId( abyPeriId, byPeriNum );
			
            // 重新取单板状态：是否在线
            TBrdPosition tBrdPosition;
            tBrdPosition.byBrdLayer = ptBrdInfo->GetLayer();
            tBrdPosition.byBrdSlot  = ptBrdInfo->GetSlot();
            tBrdPosition.byBrdID    = ptBrdInfo->GetType();
            
			//  [1/21/2011 chendaiwei]支持MPC2
            if ( m_atBrdCfgTable[byIdx].GetType() == BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ || m_atBrdCfgTable[byIdx].GetType() == BRD_TYPE_MPC2 )
            {
                if ( m_tMPCInfo.GetLocalLayer() == m_atBrdCfgTable[byIdx].GetLayer() &&
					m_tMPCInfo.GetLocalSlot() == m_atBrdCfgTable[byIdx].GetSlot() )
                {
                    m_atBrdCfgTable[byIdx].SetState( BOARD_STATUS_INLINE );
                }
                else if ( m_tMPCInfo.GetOtherMpcLayer() == m_atBrdCfgTable[byIdx].GetLayer() &&
					m_tMPCInfo.GetOtherMpcSlot() == m_atBrdCfgTable[byIdx].GetSlot() )
                {
                    m_atBrdCfgTable[byIdx].SetState( m_tMPCInfo.GetOtherMpcStatus() );
                }
            }
            else
            {
                BOOL32 bRegedBrd =  g_cBrdManagerApp.IsRegedBoard(tBrdPosition);
                m_atBrdCfgTable[byIdx].SetState( bRegedBrd ? BOARD_STATUS_INLINE : BOARD_STATUS_DISCONNECT );
            }
			
            s8  achBrdSlot[32];
            s8  achEntryStr[32];
            s8  achBrdIpStr[32];
            s8  achBrdCfgInfo[512];
            memset(achBrdSlot, '\0', sizeof(achBrdSlot));
            memset(achEntryStr, '\0', sizeof(achEntryStr));
            memset(achBrdIpStr, '\0', sizeof(achBrdIpStr));
            memset(achBrdCfgInfo, '\0', sizeof(achBrdCfgInfo));
            
            GetEntryString(achEntryStr, byIdx);
            GetBrdSlotString(ptBrdInfo->GetSlot(), ptBrdInfo->GetType(), achBrdSlot);
            GetIpFromU32(achBrdIpStr, htonl(m_atBrdCfgTable[byIdx].GetBrdIp()));
            
            sprintf(achBrdCfgInfo, "%d\t%d\t%s\t%s\t%s\t%d\t%d\t%s", 
				m_atBrdCfgTable[byIdx].GetBrdId(),
				m_atBrdCfgTable[byIdx].GetLayer(),
				achBrdSlot,
				GetBrdTypeStr(m_atBrdCfgTable[byIdx].GetType()),
				achBrdIpStr,
				m_atBrdCfgTable[byIdx].GetPortChoice(), 
				m_atBrdCfgTable[byIdx].GetCastChoice(),
				m_atBrdCfgTable[byIdx].GetAlias());
            
            wRet = WriteTableEntry( m_achCfgName, (s8*)SECTION_mcueqpBoardConfig, 
				achEntryStr, achBrdCfgInfo );
            break;
        }
    }
    if( SUCCESS_AGENT != wRet )
    {
        Agtlog(LOG_ERROR, "[SetBrdCfgById] The board ID:%d is not exist,please check it\n", byBrdId);
    }
    return wRet;
}

/*=============================================================================
函 数 名： WriteBoardCfg
功    能： 
算法实现： 
全局变量： 
参    数： u8 byIdx: 表项索引
TBoardInfo * ptBrdInfo
BOOL32 bSemTake
返 回 值： u16
=============================================================================*/
u16 CCfgAgent::WriteBoardTable( u8 byIdx, TBoardInfo * ptBrdInfo, BOOL32 bSemTake )
{
    u16  wRet = SUCCESS_AGENT;
    
    m_atBrdCfgTable[byIdx].SetEntParamNull();
    m_atBrdCfgTable[byIdx].SetPeriIdNull();
    
    // 清除在线状态
    m_atBrdCfgTable[byIdx].SetState( BOARD_STATUS_DISCONNECT );
    m_atBrdCfgTable[byIdx].SetBrdId( ptBrdInfo->GetBrdId() );
    m_atBrdCfgTable[byIdx].SetLayer( ptBrdInfo->GetLayer() );
    m_atBrdCfgTable[byIdx].SetSlot( ptBrdInfo->GetSlot() );
    m_atBrdCfgTable[byIdx].SetType( ptBrdInfo->GetType() );
    m_atBrdCfgTable[byIdx].SetCastChoice( ptBrdInfo->GetCastChoice() );
    m_atBrdCfgTable[byIdx].SetPortChoice( ptBrdInfo->GetPortChoice() );
    m_atBrdCfgTable[byIdx].SetBrdIp( ptBrdInfo->GetBrdIp() );
    m_atBrdCfgTable[byIdx].SetAlias( ptBrdInfo->GetAlias() );
    
    //此处未写入OS类型，因为上层不感知
    
    u8 byPeriNum = 0;
    u8 abyPeriId[MAXNUM_BRD_EQP];
    memset( abyPeriId, 0, sizeof(abyPeriId) );
    ptBrdInfo->GetPeriId( abyPeriId, &byPeriNum );
    m_atBrdCfgTable[byIdx].SetPeriId( abyPeriId, byPeriNum );
    
    // 重新取单板状态：是否在线
    TBrdPosition tBrdPosition;
    tBrdPosition.byBrdLayer = ptBrdInfo->GetLayer();
    tBrdPosition.byBrdSlot  = ptBrdInfo->GetSlot();
    tBrdPosition.byBrdID    = ptBrdInfo->GetType();
    
	//  [1/21/2011 chendaiwei]支持MPC2
    if ( m_atBrdCfgTable[byIdx].GetType() == BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ || m_atBrdCfgTable[byIdx].GetType() == BRD_TYPE_MPC2 )
    {
        if ( m_tMPCInfo.GetLocalLayer() == m_atBrdCfgTable[byIdx].GetLayer() && 
			m_tMPCInfo.GetLocalSlot() == m_atBrdCfgTable[byIdx].GetSlot() )
        {
            m_atBrdCfgTable[byIdx].SetState( BOARD_STATUS_INLINE );
        }
        else if ( m_tMPCInfo.GetOtherMpcLayer() == m_atBrdCfgTable[byIdx].GetLayer() &&
			m_tMPCInfo.GetOtherMpcSlot() == m_atBrdCfgTable[byIdx].GetSlot() )
        {
            m_atBrdCfgTable[byIdx].SetState( m_tMPCInfo.GetOtherMpcStatus() );
        }
    }
    else
    {
        BOOL32 bRegedBrd =  g_cBrdManagerApp.IsRegedBoard(tBrdPosition);
        m_atBrdCfgTable[byIdx].SetState( bRegedBrd ? BOARD_STATUS_INLINE : BOARD_STATUS_DISCONNECT );
    }
    s8  achBrdSlot[32];
    s8  achEntryStr[32];
    s8  achBrdIpStr[32];
    s8  achBrdCfgInfo[512];
    memset(achBrdSlot, '\0', sizeof(achBrdSlot));
    memset(achEntryStr, '\0', sizeof(achEntryStr));
    memset(achBrdIpStr, '\0', sizeof(achBrdIpStr));
    memset(achBrdCfgInfo, '\0', sizeof(achBrdCfgInfo));
    
    GetEntryString(achEntryStr, byIdx);
    GetBrdSlotString(ptBrdInfo->GetSlot(), ptBrdInfo->GetType(), achBrdSlot);
    GetIpFromU32(achBrdIpStr, htonl(m_atBrdCfgTable[byIdx].GetBrdIp()));
    
    // guzh [1/25/2007] 写入单板别名
    sprintf(achBrdCfgInfo, "%d\t%d\t%s\t%s\t%s\t%d\t%d\t%s", 
		m_atBrdCfgTable[byIdx].GetBrdId(),
		m_atBrdCfgTable[byIdx].GetLayer(),
		achBrdSlot,
		GetBrdTypeStr(m_atBrdCfgTable[byIdx].GetType()),
		achBrdIpStr,
		m_atBrdCfgTable[byIdx].GetPortChoice(), 
		m_atBrdCfgTable[byIdx].GetCastChoice(),
		m_atBrdCfgTable[byIdx].GetAlias());
    
    wRet = WriteTableEntry( m_achCfgName, (s8*)SECTION_mcueqpBoardConfig, 
		achEntryStr, achBrdCfgInfo, bSemTake );

    return wRet;
}

/*=============================================================================
函 数 名： OrganizeBoardTable
功    能： 保存单板信息并组织到ptInfTobeWrited，合并多个board信息一起写入文件
算法实现： 
全局变量： 
参    数： u8 byIdx: 表项索引
TBoardInfo * ptBrdInfo
BOOL32 bSemTake
返 回 值： u16
=============================================================================*/
u16 CCfgAgent::OrganizeBoardTable( u8 byIdx, TBoardInfo * ptBrdInfo, s8* ptInfTobeWrited, BOOL32 bSemTake )
{
	if (MAX_BOARD_NUM <= byIdx || ptInfTobeWrited == NULL || NULL == ptBrdInfo )
	{
	   StaticLog("[OrganizeBoardTable]organize error,byIdx:%d,ptInfoTobeWrite Null?:%d,ptBrdInfo Null?:%d,bSemTake.%d\n",
		   byIdx,(ptInfTobeWrited == NULL)?1:0,(NULL == ptBrdInfo)?1:0,bSemTake);
	   return ERR_AGENT_WRITEFILE;
	}

	u16  wRet = SUCCESS_AGENT;
    
    m_atBrdCfgTable[byIdx].SetEntParamNull();
    m_atBrdCfgTable[byIdx].SetPeriIdNull();
    
    // 清除在线状态
    m_atBrdCfgTable[byIdx].SetState( BOARD_STATUS_DISCONNECT );
    m_atBrdCfgTable[byIdx].SetBrdId( ptBrdInfo->GetBrdId() );
    m_atBrdCfgTable[byIdx].SetLayer( ptBrdInfo->GetLayer() );
    m_atBrdCfgTable[byIdx].SetSlot( ptBrdInfo->GetSlot() );
    m_atBrdCfgTable[byIdx].SetType( ptBrdInfo->GetType() );
    m_atBrdCfgTable[byIdx].SetCastChoice( ptBrdInfo->GetCastChoice() );
    m_atBrdCfgTable[byIdx].SetPortChoice( ptBrdInfo->GetPortChoice() );
    m_atBrdCfgTable[byIdx].SetBrdIp( ptBrdInfo->GetBrdIp() );
    m_atBrdCfgTable[byIdx].SetAlias( ptBrdInfo->GetAlias() );
    
    //此处未写入OS类型，因为上层不感知
    
    u8 byPeriNum = 0;
    u8 abyPeriId[MAXNUM_BRD_EQP];
    memset( abyPeriId, 0, sizeof(abyPeriId) );
    ptBrdInfo->GetPeriId( abyPeriId, &byPeriNum );
    m_atBrdCfgTable[byIdx].SetPeriId( abyPeriId, byPeriNum );
    
    // 重新取单板状态：是否在线
    TBrdPosition tBrdPosition;
    tBrdPosition.byBrdLayer = ptBrdInfo->GetLayer();
    tBrdPosition.byBrdSlot  = ptBrdInfo->GetSlot();
    tBrdPosition.byBrdID    = ptBrdInfo->GetType();
    
	//  [1/21/2011 chendaiwei]支持MPC2
    if ( m_atBrdCfgTable[byIdx].GetType() == BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ || m_atBrdCfgTable[byIdx].GetType() == BRD_TYPE_MPC2 )
    {
        if ( m_tMPCInfo.GetLocalLayer() == m_atBrdCfgTable[byIdx].GetLayer() &&  
			m_tMPCInfo.GetLocalSlot() == m_atBrdCfgTable[byIdx].GetSlot() )
        {
            m_atBrdCfgTable[byIdx].SetState( BOARD_STATUS_INLINE );
        }
        else if ( m_tMPCInfo.GetOtherMpcLayer() == m_atBrdCfgTable[byIdx].GetLayer() &&
			m_tMPCInfo.GetOtherMpcSlot() == m_atBrdCfgTable[byIdx].GetSlot() )
        {
            m_atBrdCfgTable[byIdx].SetState( m_tMPCInfo.GetOtherMpcStatus() );
        }
    }
    else
    {
        BOOL32 bRegedBrd =  g_cBrdManagerApp.IsRegedBoard(tBrdPosition);
        m_atBrdCfgTable[byIdx].SetState( bRegedBrd ? BOARD_STATUS_INLINE : BOARD_STATUS_DISCONNECT );
    }
    s8  achBrdSlot[32];
    s8  achEntryStr[32];
    s8  achBrdIpStr[32];
    memset(achBrdSlot, '\0', sizeof(achBrdSlot));
    memset(achEntryStr, '\0', sizeof(achEntryStr));
    memset(achBrdIpStr, '\0', sizeof(achBrdIpStr));
    
    GetEntryString(achEntryStr, byIdx);
    GetBrdSlotString(ptBrdInfo->GetSlot(), ptBrdInfo->GetType(), achBrdSlot);
    GetIpFromU32(achBrdIpStr, htonl(m_atBrdCfgTable[byIdx].GetBrdIp()));
    
    // guzh [1/25/2007] 写入单板别名
    sprintf(ptInfTobeWrited, "%s = %d\t%d\t%s\t%s\t%s\t%d\t%d\t%s%s", 
		achEntryStr,
		m_atBrdCfgTable[byIdx].GetBrdId(),
		m_atBrdCfgTable[byIdx].GetLayer(),
		achBrdSlot,
		GetBrdTypeStr(m_atBrdCfgTable[byIdx].GetType()),
		achBrdIpStr,
		m_atBrdCfgTable[byIdx].GetPortChoice(), 
		m_atBrdCfgTable[byIdx].GetCastChoice(),
		m_atBrdCfgTable[byIdx].GetAlias(),
		STR_RETURN);
    
    return wRet;
	
	
	
	
	
}

/*=============================================================================
函 数 名： GetBoardRow
功    能： 取单板行号
算法实现： 
全局变量： 
参    数： u8 byLayer：层号
u8 bySlot：槽号
u8 byType：类型
返 回 值： u16  
=============================================================================*/
u8  CCfgAgent::GetBoardRow(u8 byLayer, u8 bySlot, u8 byType)
{
    u8 byRow = ERR_AGENT_BRDNOTEXIST;   // 单板不存在
    for(u32 dwLop = 0; dwLop < m_dwBrdCfgEntryNum; dwLop++)
    {
        if( m_atBrdCfgTable[dwLop].GetLayer() == byLayer &&
            m_atBrdCfgTable[dwLop].GetSlot() == bySlot &&
            m_atBrdCfgTable[dwLop].GetType() == byType)
        {
            byRow = (u8)(dwLop+1);
            break;
        }
    }
    return byRow; 
}

/*=============================================================================
函 数 名： JustConfigedBrdDRI16
功    能： 整对mcs还没有对dri16做出调整的情况下，临时修改配置
算法实现： 
全局变量： 
参    数： u8 byLayer：层号
u8 bySlot：槽号
u8 byType：类型
返 回 值： BOOL32 
=============================================================================*/
void CCfgAgent::AdjustConfigedBrdDRI16(TBrdPosition tBrdPosition)
{
	if (tBrdPosition.byBrdID == BRD_TYPE_16E1/*DSL8000_BRD_16E1*/)
	{
		//调整界面配置DRI为DRI16
		for(u32 dwLoop = 0; dwLoop < m_dwBrdCfgEntryNum; ++dwLoop)
		{
			//  [1/21/2011 chendaiwei]新增DRI2支持
			if ( (m_atBrdCfgTable[dwLoop].GetType() == BRD_TYPE_DRI/*DSL8000_BRD_DRI*/ ||  m_atBrdCfgTable[dwLoop].GetType() == BRD_TYPE_DRI2) &&
				m_atBrdCfgTable[dwLoop].GetSlot() == tBrdPosition.byBrdSlot &&
				m_atBrdCfgTable[dwLoop].GetLayer() == tBrdPosition.byBrdLayer)
			{
				m_atBrdCfgTable[dwLoop].SetType(BRD_TYPE_16E1/*DSL8000_BRD_16E1*/);
				break;
			}
		}
	}
}

/*=============================================================================
函 数 名： IsConfigedBrd
功    能： 是否配置了该单板
算法实现： 
全局变量： 
参    数： u8 byLayer：层号
u8 bySlot：槽号
u8 byType：类型
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::IsConfigedBrd(TBrdPosition tBrdPosition)  // byBrdId 
{
	BOOL32 bRet = FALSE;
	for( u32 dwLoop = 0; dwLoop < m_dwBrdCfgEntryNum; dwLoop++ )
	{
		if( m_atBrdCfgTable[dwLoop].GetType() == tBrdPosition.byBrdID &&
            m_atBrdCfgTable[dwLoop].GetSlot() == tBrdPosition.byBrdSlot &&
            m_atBrdCfgTable[dwLoop].GetLayer() == tBrdPosition.byBrdLayer )
		{
			bRet = TRUE;
			break;
		}
	}
	
    return bRet;	    
}

/*=============================================================================
函 数 名： GetBrdId
功    能： 取单板Id
算法实现： 
全局变量： 
参    数：  TBrdPosition tBrdPositon
返 回 值： u8
=============================================================================*/
u8 CCfgAgent::GetBrdId( TBrdPosition tBrdPos )
{
    u8  byBrdId = 0;
	
    for(u32 dwLoop = 0; dwLoop < m_dwBrdCfgEntryNum; dwLoop++ )
	{
		if( m_atBrdCfgTable[dwLoop].GetLayer() == tBrdPos.byBrdLayer &&
            m_atBrdCfgTable[dwLoop].GetSlot() == tBrdPos.byBrdSlot &&
            m_atBrdCfgTable[dwLoop].GetType() == tBrdPos.byBrdID )
		{
			byBrdId = m_atBrdCfgTable[dwLoop].GetBrdId();
			break;
		}
	}
    if ( 0 == byBrdId )
    {
        Agtlog( LOG_ERROR, "[GetBrdId] Brd<%d,%d:%s> unexisted !\n", 
			tBrdPos.byBrdLayer, tBrdPos.byBrdSlot, GetBrdTypeStr(tBrdPos.byBrdID));
    }
    return byBrdId;	
}

/*=============================================================================
函 数 名： GetPriInfo
功    能： 取外设信息
算法实现： 
全局变量： 
参    数： u8 byEqpId：外设Id
u32* pdwEqpIp：外设Ip
u8* pbyEqpType：类型
返 回 值： BOOL32 
=============================================================================*/
u16 CCfgAgent::GetPriInfo(u8 byEqpId, u32* pdwEqpIp, u8* pbyEqpType)
{
    u8 byRow = 0;
    if(NULL == pbyEqpType || NULL == pdwEqpIp)
    {
        return ERR_AGENT_NULLPARAM;
    }
	*pdwEqpIp = 0;
	*pbyEqpType = 0;
	
    if( byEqpId >= MIXERID_MIN && byEqpId <= MIXERID_MAX )
    {
        for(byRow = 0; byRow < m_dwMixEntryNum; byRow++ )
        {
            if(m_atMixTable[byRow].GetEqpId() == byEqpId)
            {
				*pdwEqpIp = htonl(m_atMixTable[byRow].GetIpAddr());
				*pbyEqpType = m_atMixTable[byRow].GetType();
                return SUCCESS_AGENT;
            }
        }
    }
	
    if( byEqpId >= RECORDERID_MIN && byEqpId <= RECORDERID_MAX )
    {
        for(byRow = 0; byRow < m_dwRecEntryNum; byRow++)
        {
            if( byEqpId == m_atRecTable[byRow].GetEqpId() )
            {
                *pdwEqpIp = htonl(m_atRecTable[byRow].GetIpAddr());
                *pbyEqpType = EQP_TYPE_RECORDER;
                return SUCCESS_AGENT;
            }
        }
    }
	
    if( byEqpId >= TVWALLID_MIN && byEqpId <= TVWALLID_MAX )
    {
        for(byRow = 0; byRow < m_dwTVWallEntryNum; byRow++)
        {
            if(m_atTVWallTable[byRow].GetEqpId() == byEqpId)
            {
                *pbyEqpType = EQP_TYPE_TVWALL;
                *pdwEqpIp = htonl(m_atTVWallTable[byRow].GetIpAddr());
                return SUCCESS_AGENT;
            }
        }
    }
	
    if( byEqpId >= BASID_MIN && byEqpId <= BASID_MAX )
    {
		for(byRow = 0; byRow < (u8)m_dwBasEntryNum; byRow++)
		{
			if(m_atBasTable[byRow].GetEqpId() == byEqpId)
			{
                *pdwEqpIp = htonl(m_atBasTable[byRow].GetIpAddr());
                *pbyEqpType = EQP_TYPE_BAS;
                return SUCCESS_AGENT;
			}
		}
		
		//zw 2008/08/08
		for(byRow = 0; byRow < (u8)m_dwBasHDEntryNum; byRow++)
		{
			if(m_atBasHDTable[byRow].GetEqpId() == byEqpId)
			{
                *pdwEqpIp = htonl(m_atBasHDTable[byRow].GetIpAddr());
                *pbyEqpType = EQP_TYPE_BAS;
                return SUCCESS_AGENT;
			}
		}
    }
    
    if(byEqpId >= VMPID_MIN && byEqpId <= VMPID_MAX )
    {
        for(byRow = 0; byRow < m_dwVMPEntryNum; byRow++)
        {
            if(m_atVMPTable[byRow].GetEqpId() == byEqpId)
            {
                *pdwEqpIp = htonl(m_atVMPTable[byRow].GetIpAddr());
                *pbyEqpType = EQP_TYPE_VMP;
                return SUCCESS_AGENT;
            }
        }
    }
	
    if(byEqpId >= VMPTWID_MIN && byEqpId <= VMPTWID_MAX )
    {
        for(byRow = 0; byRow < m_dwMpwEntryNum; byRow++)
        {
            if(m_atMpwTable[byRow].GetEqpId() == byEqpId)
            {
                *pdwEqpIp = htonl(m_atMpwTable[byRow].GetIpAddr());
                *pbyEqpType = EQP_TYPE_VMPTW;
                return SUCCESS_AGENT;
            }
        }
    }
    
    if( byEqpId >= PRSID_MIN && byEqpId <= PRSID_MAX )
    {
        for(byRow = 0; byRow < m_dwPrsEntryNum; byRow++)
        {
            if( m_atPrsTable[byRow].GetEqpId() == byEqpId )
            {
                *pdwEqpIp = htonl(m_atPrsTable[byRow].GetIpAddr());
                *pbyEqpType = EQP_TYPE_PRS;
                return SUCCESS_AGENT;
            }
        }
    }
	
	//4.6新加外设 jlb
    //Hdu
    if( byEqpId >= HDUID_MIN && byEqpId <= HDUID_MAX )
    {
		for(byRow = 0; byRow < m_dwHduEntryNum; byRow++)
		{
			if(m_atHduTable[byRow].GetEqpId() == byEqpId)
            {
				*pbyEqpType = EQP_TYPE_HDU;
				/*if (STARTMODE_HDU_H == m_atHduTable[byRow].GetStartMode()) //zjl20100426
				{
					*pbyEqpType = EQP_TYPE_HDU_H;
				}
				else if(STARTMODE_HDU_M == m_atHduTable[byRow].GetStartMode())
				{
					*pbyEqpType = EQP_TYPE_HDU;
				}
				else if(STARTMODE_HDU_L == m_atHduTable[byRow].GetStartMode())
				{
					*pbyEqpType = EQP_TYPE_HDU_L;
				}
				else if(STARTMODE_HDU2 == m_atHduTable[byRow].GetStartMode())
				{
					*pbyEqpType = EQP_TYPE_HDU2;
				}
				else if(STARTMODE_HDU2_L == m_atHduTable[byRow].GetStartMode())
				{
					*pbyEqpType = EQP_TYPE_HDU2_L;
				}
				else
				{
					*pbyEqpType = EQP_TYPE_HDU;
					printf("[GetPriInfo] warning: Set default hdu EqpType:%d because m_atHduTable GetS unexpected StartMode:%d!\n",
						EQP_TYPE_HDU, m_atHduTable[byRow].GetStartMode());
				}*/
                *pdwEqpIp = htonl(m_atHduTable[byRow].GetIpAddr()); //3786150060;    
                return SUCCESS_AGENT;
            }
		}
    }
	
	//svmp
	if( byEqpId >= VMPID_MIN && byEqpId <= VMPID_MAX )
    {
        for(byRow = 0; byRow < m_dwSvmpEntryNum; byRow++)
        {
            if(m_atSvmpTable[byRow].GetEqpId() == byEqpId)
            {
                *pbyEqpType = EQP_TYPE_VMP;
                *pdwEqpIp = htonl(m_atSvmpTable[byRow].GetIpAddr());
                return SUCCESS_AGENT;
            }
        }
    }
	
	//dvmp
	// 	if( byEqpId >= VMPID_MIN && byEqpId <= VMPID_MAX )
	//     {
	//         for(byRow = 0; byRow < m_dwDvmpEntryNum; byRow++)
	//         {
	//             if(m_atDvmpBasicTable[byRow].GetEqpId() == byEqpId)
	//             {
	//                 *pbyEqpType = EQP_TYPE_VMP;
	//                 *pdwEqpIp = htonl(m_atDvmpBasicTable[byRow].GetIpAddr());
	//                 return SUCCESS_AGENT;
	//             }
	//         }
	//     }    
	
	//mpuBas
	if( byEqpId >= BASID_MIN && byEqpId <= BASID_MAX )
    {
        for(byRow = 0; byRow < m_dwMpuBasEntryNum; byRow++)
        {
            if(m_atMpuBasTable[byRow].GetEqpId() == byEqpId)
            {
                *pbyEqpType = EQP_TYPE_BAS;
                *pdwEqpIp = htonl(m_atMpuBasTable[byRow].GetIpAddr());
                return SUCCESS_AGENT;
            }
        }
    }
	
	//ebap
	// 	if( byEqpId >= BASID_MIN && byEqpId <= BASID_MAX )
	//     {
	//         for(byRow = 0; byRow < m_dwEbapEntryNum; byRow++)
	//         {
	//             if(m_atEbapTable[byRow].GetEqpId() == byEqpId)
	//             {
	//                 *pbyEqpType = EQP_TYPE_BAS;
	//                 *pdwEqpIp = htonl(m_atEbapTable[byRow].GetIpAddr());
	//                 return SUCCESS_AGENT;
	//             }
	//         }
	//     }
	
	//evpu
	// 	if( byEqpId >= VMPID_MIN && byEqpId <= VMPID_MAX )
	//     {
	//         for(byRow = 0; byRow < m_dwEvpuEntryNum; byRow++)
	//         {
	//             if(m_atEvpuTable[byRow].GetEqpId() == byEqpId)
	//             {
	//                 *pbyEqpType = EQP_TYPE_VMP;
	//                 *pdwEqpIp = htonl(m_atEvpuTable[byRow].GetIpAddr());
	//                 return SUCCESS_AGENT;
	//             }
	//         }
	//     }
	
    return ERR_AGENT_EQPNOTEXIST;    
}

/*=============================================================================
函 数 名： GetRecCfgToBrd
功    能： 取录像机配置（单板管理）
算法实现： 
全局变量： 
参    数：  u8 byId
TEqpRecEntry* ptRecCfg
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::GetRecCfgToBrd( u8 byId, TEqpRecEntry* ptRecEnt)
{
	BOOL32 bRet = FALSE;
    if( NULL == ptRecEnt || byId < RECORDERID_MIN || byId > RECORDERID_MAX )
    {
        Agtlog(LOG_ERROR, "[GetRecCfgToBrd] param err: byId<%d>, ptCfg: 0x%x\n", byId, ptRecEnt);
        return bRet;
    }
	
    for( u8 byRow = 1; byRow <= m_dwRecEntryNum; byRow++)
    {
        if( m_atRecTable[byRow-1].GetEqpId() == byId )
        {
            ptRecEnt->SetEqpId( m_atRecTable[byRow-1].GetEqpId() );
            ptRecEnt->SetMcuRecvPort( m_atRecTable[byRow-1].GetMcuRecvPort() );
            ptRecEnt->SetSwitchBrdId( m_atRecTable[byRow-1].GetSwitchBrdId() );
            ptRecEnt->SetIpAddr( m_atRecTable[byRow-1].GetIpAddr() );
            ptRecEnt->SetEqpRecvPort( m_atRecTable[byRow-1].GetEqpRecvPort() );
			
            ptRecEnt->SetMcuId( m_atRecTable[byRow-1].GetMcuId() );
            ptRecEnt->SetMcuPort( m_atRecTable[byRow-1].GetMcuPort() );
            ptRecEnt->SetMcuIp( m_atRecTable[byRow-1].GetMcuIp() );
            ptRecEnt->SetType( m_atRecTable[byRow-1].GetType() );
            ptRecEnt->SetAlias( m_atRecTable[byRow-1].GetAlias() );
			
			bRet = TRUE;
            break;
        }
    }
    if( !bRet )
    {
        Agtlog(LOG_ERROR, "[GetRecCfgToBrd] The bas ID.%d is not exist,please check it\n", byId);
    }
    return bRet;
}

/*=============================================================================
函 数 名： GetTvCfgToBrd
功    能： 取Tv配置（单板管理）
算法实现： 
全局变量： 
参    数：  u8 byId
TEqpTVWallEntry* ptTWCfg
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::GetTvCfgToBrd( u8 byId, TEqpTVWallEntry* ptTWEnt)
{
	if( NULL == ptTWEnt || byId < TVWALLID_MIN || byId > TVWALLID_MAX )
    {
        Agtlog(LOG_ERROR, "[GetTvCfgToBrd] param err: byId<%d>, ptCfg: 0x%x\n", byId, ptTWEnt);
        return FALSE;
    }
	
    BOOL32 bRet = FALSE;
	
    for( u8 byRow = 1; byRow <= m_dwTVWallEntryNum; byRow++)
    {
        if( m_atTVWallTable[byRow-1].GetEqpId() == byId )
        {
            ptTWEnt->SetEqpId( m_atTVWallTable[byRow-1].GetEqpId() );
            ptTWEnt->SetRunBrdId( m_atTVWallTable[byRow-1].GetRunBrdId() );
            ptTWEnt->SetEqpRecvPort( m_atTVWallTable[byRow-1].GetEqpRecvPort() );
			
            ptTWEnt->SetIpAddr( m_atTVWallTable[byRow-1].GetIpAddr() );
            ptTWEnt->SetMcuId( m_atTVWallTable[byRow-1].GetMcuId() );
            ptTWEnt->SetMcuIp( m_atTVWallTable[byRow-1].GetMcuIp() );
            ptTWEnt->SetMcuPort( m_atTVWallTable[byRow-1].GetMcuPort() );           
            ptTWEnt->SetType( m_atTVWallTable[byRow-1].GetType() );
            ptTWEnt->SetAlias( m_atTVWallTable[byRow-1].GetAlias() );
            
            u8 abyMapId[MAXNUM_MAP] = { 0 };
            u8 byMapNum = 0;
            m_atTVWallTable[byRow-1].GetUsedMap( abyMapId, byMapNum );
            ptTWEnt->SetUsedMap( abyMapId, byMapNum );
			
            bRet = TRUE;
            break;
        }
    }
	
    if( !bRet )
    {
        Agtlog(LOG_ERROR, "[GetTvCfgToBrd] The Tv ID.%d is not exist,please check it\n", byId);
    }
	
    return bRet;
}

/*=============================================================================
函 数 名： GetMixCfgToBrd
功    能： 取混音器信息（单板管理）
算法实现： 
全局变量： 
参    数：  u8 byId
TEqpMixerEntry* ptMixerCfg
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::GetMixCfgToBrd( u8 byId, TEqpMixerEntry* ptMixerEnt)
{
	if( NULL == ptMixerEnt || byId < MIXERID_MIN || byId > MIXERID_MAX )
    {
        Agtlog(LOG_ERROR, "[GetMixCfgToBrd] param err: byId<%d>, ptCfg: 0x%x\n", byId, ptMixerEnt);
        return FALSE;
    }
	
    BOOL32 bRet = FALSE;
    for( u8 byRow = 1; byRow <= m_dwMixEntryNum; byRow++)
    {
        if( m_atMixTable[byRow-1].GetEqpId() == byId )
        {
            ptMixerEnt->SetEqpId( m_atMixTable[byRow-1].GetEqpId() );
            ptMixerEnt->SetMcuRecvPort( m_atMixTable[byRow-1].GetMcuRecvPort() );
            ptMixerEnt->SetSwitchBrdId( m_atMixTable[byRow-1].GetSwitchBrdId() );
            ptMixerEnt->SetEqpRecvPort( m_atMixTable[byRow-1].GetEqpRecvPort() );
            ptMixerEnt->SetRunBrdId( m_atMixTable[byRow-1].GetRunBrdId() );
            ptMixerEnt->SetMaxChnInGrp( m_atMixTable[byRow-1].GetMaxChnInGrp() );
            
            ptMixerEnt->SetIpAddr( m_atMixTable[byRow-1].GetIpAddr() );
            ptMixerEnt->SetMcuPort( m_atMixTable[byRow-1].GetMcuPort() );
            ptMixerEnt->SetMcuId( m_atMixTable[byRow-1].GetMcuId() );
            ptMixerEnt->SetMcuIp( m_atMixTable[byRow-1].GetMcuIp() );
            ptMixerEnt->SetType( m_atMixTable[byRow-1].GetType() );
            ptMixerEnt->SetAlias( m_atMixTable[byRow-1].GetAlias() );
			
            u8 abyMapId[MAXNUM_MAP] = { 0 };
            u8 byMapNum = 0;
            m_atMixTable[byRow-1].GetUsedMap( abyMapId, byMapNum );
            ptMixerEnt->SetUsedMap( abyMapId, byMapNum );
			
			ptMixerEnt->SetIsSendRedundancy( m_atMixTable[byRow-1].IsSendRedundancy() );
			
            bRet = TRUE;
            break;
        }
    }
    if( !bRet )
    {
        Agtlog(LOG_ERROR, "[GetMixCfgToBrd] The Mix ID.%d is not exist,please check it\n", byId);
    }
    return bRet;
}

/*=============================================================================
函 数 名： GetBasCfgToBrd
功    能： 取Bas配置（单板管理）
算法实现： 
全局变量： 
参    数：  u8 byId
TEqpBasEntry* ptBasCfg
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::GetBasCfgToBrd( u8 byId, TEqpBasEntry* ptBasEnt)
{
	if( NULL == ptBasEnt || byId < BASID_MIN || byId > BASID_MAX )
    {
        Agtlog(LOG_ERROR, "[GetBasCfgToBrd] param err: byId<%d>, ptCfg: 0x%x\n", byId, ptBasEnt);
        return FALSE;
    }
	
    BOOL32 bRet = FALSE;
    for( u8 byRow = 1; byRow <= m_dwBasEntryNum; byRow++)
    {
        if( m_atBasTable[byRow-1].GetEqpId() == byId )
        {
            ptBasEnt->SetEqpId( m_atBasTable[byRow-1].GetEqpId() );
            ptBasEnt->SetMcuRecvPort( m_atBasTable[byRow-1].GetMcuRecvPort() );
            ptBasEnt->SetSwitchBrdId( m_atBasTable[byRow-1].GetSwitchBrdId() );
            ptBasEnt->SetEqpRecvPort( m_atBasTable[byRow-1].GetEqpRecvPort() );
            ptBasEnt->SetRunBrdId( m_atBasTable[byRow-1].GetRunBrdId() );
            
            ptBasEnt->SetIpAddr( m_atBasTable[byRow-1].GetIpAddr() );
            ptBasEnt->SetMcuId( m_atBasTable[byRow-1].GetMcuId() );
            ptBasEnt->SetMcuIp( m_atBasTable[byRow-1].GetMcuIp() );
            ptBasEnt->SetMcuPort( m_atBasTable[byRow-1].GetMcuPort() );
            ptBasEnt->SetType( m_atBasTable[byRow-1].GetType() );
            ptBasEnt->SetAlias( m_atBasTable[byRow-1].GetAlias() );
			
            u8 abyMapId[MAXNUM_MAP];
            memset(abyMapId, 0, sizeof(abyMapId));
            u8 byMapNum = 0;
            m_atBasTable[byRow-1].GetUsedMap( abyMapId, byMapNum );
            ptBasEnt->SetUsedMap( abyMapId, byMapNum );
            
            bRet = TRUE;
            break;
        }
    }
	
    if( !bRet )
    {
        Agtlog(LOG_ERROR, "[GetBasCfgToBrd] The bas ID.%d is not exist,please check it\n", byId);
    }
    return bRet;
}

/*=============================================================================
函 数 名： GetVmpCfgToBrd
功    能： 取Vmp配置（单板管理）
算法实现： 
全局变量： 
参    数：  u8 byId
TEqpVMPEntry* ptVMPCfg
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::GetVmpCfgToBrd( u8 byId, TEqpVMPEntry* ptVMPEnt )
{
	if( NULL == ptVMPEnt || byId < VMPID_MIN || byId > VMPID_MAX )
    {
        Agtlog(LOG_ERROR, "[GetVmpCfgToBrd] param err: byId<%d>, ptCfg: 0x%x\n", byId, ptVMPEnt);
        return FALSE;
    }
	
    BOOL32 bRet = FALSE;
	
    for( u8 byRow = 1; byRow <= m_dwVMPEntryNum; byRow++)
    {
        if( m_atVMPTable[byRow-1].GetEqpId() == byId )
        {
            ptVMPEnt->SetEqpId( m_atVMPTable[byRow-1].GetEqpId() );
            ptVMPEnt->SetMcuRecvPort( m_atVMPTable[byRow-1].GetMcuRecvPort() );
            ptVMPEnt->SetSwitchBrdId( m_atVMPTable[byRow-1].GetSwitchBrdId() );
            
            ptVMPEnt->SetIpAddr( m_atVMPTable[byRow-1].GetIpAddr() );
            ptVMPEnt->SetRunBrdId( m_atVMPTable[byRow-1].GetRunBrdId() );
            ptVMPEnt->SetEqpRecvPort( m_atVMPTable[byRow-1].GetEqpRecvPort() );
            ptVMPEnt->SetEncodeNum( m_atVMPTable[byRow-1].GetEncodeNum() );
			
            ptVMPEnt->SetMcuId( m_atVMPTable[byRow-1].GetMcuId() );
            ptVMPEnt->SetMcuIp( m_atVMPTable[byRow-1].GetMcuIp() );
            ptVMPEnt->SetMcuPort( m_atVMPTable[byRow-1].GetMcuPort() );
            ptVMPEnt->SetType( m_atVMPTable[byRow-1].GetType() );
            ptVMPEnt->SetAlias( m_atVMPTable[byRow-1].GetAlias() );
			
            u8 abyMapId[MAXNUM_MAP];
            memset(abyMapId, 0, sizeof(abyMapId));
            u8 byMapNum = 0;
            m_atVMPTable[byRow-1].GetUsedMap( abyMapId, byMapNum );
            ptVMPEnt->SetUsedMap( abyMapId, byMapNum );
			
            bRet = TRUE;
            break;
        }
    }
	
    if( !bRet )
    {
        Agtlog(LOG_ERROR, "[GetVmpCfgToBrd] The vmp ID.%d is not exist,please check it\n", byId);
    }
    return bRet;
}

/*=============================================================================
函 数 名： GetMpwCfgToBrd
功    能： 取Mpw配置信息（单板管理）
算法实现： 
全局变量： 
参    数： u8 byId
TEqpMPWEntry* ptMpwCfg
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::GetMpwCfgToBrd(u8 byId, TEqpMPWEntry* ptMpwEnt)
{
	if( NULL == ptMpwEnt || byId < VMPTWID_MIN || byId > VMPTWID_MAX )
    {
        Agtlog(LOG_ERROR, "[GetBasCfgToBrd] param err: byId<%d>, ptCfg: 0x%x\n", byId, ptMpwEnt);
        return FALSE;
    }
    
	BOOL32 bRet = FALSE;
    for( u8 byRow = 1; byRow <= m_dwMpwEntryNum; byRow++)
    {
        if( m_atMpwTable[byRow-1].GetEqpId() == byId )
        {
            ptMpwEnt->SetEqpId( m_atMpwTable[byRow-1].GetEqpId() );
            ptMpwEnt->SetIpAddr( m_atMpwTable[byRow-1].GetIpAddr() );
            ptMpwEnt->SetRunBrdId( m_atMpwTable[byRow-1].GetRunBrdId() );
            ptMpwEnt->SetEqpRecvPort( m_atMpwTable[byRow-1].GetEqpRecvPort() );
			
            ptMpwEnt->SetMcuId( m_atMpwTable[byRow-1].GetMcuId() );
            ptMpwEnt->SetMcuIp( m_atMpwTable[byRow-1].GetMcuIp() );
            ptMpwEnt->SetMcuPort( m_atMpwTable[byRow-1].GetMcuPort() );
            ptMpwEnt->SetType( m_atMpwTable[byRow-1].GetType() );
            ptMpwEnt->SetAlias( m_atMpwTable[byRow-1].GetAlias() );
			
            u8 abyMapId[MAXNUM_MAP];
            memset(abyMapId, 0, sizeof(abyMapId));
            u8 byMapNum = 0;
            m_atMpwTable[byRow-1].GetUsedMap( abyMapId, byMapNum );
            ptMpwEnt->SetUsedMap( abyMapId, byMapNum );
			
            bRet = TRUE;
            break;
        }
    }
    if( !bRet )
    {
        Agtlog(LOG_ERROR, "[GetMpwCfgToBrd] The vmp ID.%d is not exist,please check it\n", byId);
    }
    return bRet;
}
/*=============================================================================
函 数 名： GetPrsCfgToBrd
功    能： 取Prs配置
算法实现： 
全局变量： 
参    数：  u8 byId
TEqpPrsEntry* ptPrsEnt
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::GetPrsCfgToBrd( u8 byId, TEqpPrsEntry* ptPrsEnt)
{
    if( NULL == ptPrsEnt || byId < PRSID_MIN || byId > PRSID_MAX )
    {
        Agtlog(LOG_ERROR, "[GetPrsCfgToBrd] param err: byId<%d>, ptCfg: 0x%x\n", byId, ptPrsEnt);
        return FALSE;
    }
	
    BOOL32 bRet = FALSE;
    for( u8 byRow = 1; byRow <= m_dwPrsEntryNum; byRow++)
    {
        if(byId == m_atPrsTable[byRow-1].GetEqpId())
        {
            ptPrsEnt->SetEqpId( m_atPrsTable[byRow-1].GetEqpId() );
            ptPrsEnt->SetIpAddr( m_atPrsTable[byRow-1].GetIpAddr() );
            ptPrsEnt->SetMcuRecvPort( m_atPrsTable[byRow-1].GetMcuRecvPort() );
            ptPrsEnt->SetEqpRecvPort( m_atPrsTable[byRow-1].GetEqpRecvPort() );
            ptPrsEnt->SetRunBrdId( m_atPrsTable[byRow-1].GetRunBrdId() );
            ptPrsEnt->SetSwitchBrdId( m_atPrsTable[byRow-1].GetSwitchBrdId() );
            ptPrsEnt->SetFirstTimeSpan( m_atPrsTable[byRow-1].GetFirstTimeSpan() );
            ptPrsEnt->SetSecondTimeSpan( m_atPrsTable[byRow-1].GetSecondTimeSpan() );
            ptPrsEnt->SetThirdTimeSpan( m_atPrsTable[byRow-1].GetThirdTimeSpan() );
            ptPrsEnt->SetRejectTimeSpan( m_atPrsTable[byRow-1].GetRejectTimeSpan() );
            ptPrsEnt->SetAlias( m_atPrsTable[byRow-1].GetAlias() );
            
            ptPrsEnt->SetMcuIp( m_atPrsTable[byRow-1].GetMcuIp() );
            ptPrsEnt->SetMcuId( m_atPrsTable[byRow-1].GetMcuId() );
            ptPrsEnt->SetMcuPort( m_atPrsTable[byRow-1].GetMcuPort() );
            ptPrsEnt->SetType( m_atPrsTable[byRow-1].GetType() );
			
            bRet = TRUE;
            break;
        }
    }
    if( !bRet )
    {
        Agtlog(LOG_ERROR, "[GetPrsCfgToBrd] The prs ID.%d is not exist,please check it\n", byId);
    }
    return bRet;
}

//4.6版本新加 jlb 
/*=============================================================================
函 数 名： GetHduCfgToBrd
功    能： 取Hdu信息
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/30  4.6         江乐斌       创建
=============================================================================*/ 
BOOL32 CCfgAgent::GetHduCfgToBrd( u8 byId, TEqpHduEntry * ptHduCfg )
{
	if( NULL == ptHduCfg || byId < HDUID_MIN || byId > HDUID_MAX )
    {
        Agtlog(LOG_ERROR, "[GetHduCfgToBrd] param err: byId<%d>, ptHduCfg: 0x%x\n", byId, ptHduCfg);
        return FALSE;
    }
	
    BOOL32 bRet = FALSE;
	
    for( u8 byRow = 1; byRow <= m_dwHduEntryNum; byRow++)
    {
        if( m_atHduTable[byRow-1].GetEqpId() == byId )
        {
            ptHduCfg->SetEqpId( m_atHduTable[byRow-1].GetEqpId() );
            ptHduCfg->SetRunBrdId( m_atHduTable[byRow-1].GetRunBrdId() );
            ptHduCfg->SetEqpRecvPort( m_atHduTable[byRow-1].GetEqpRecvPort() );
			ptHduCfg->SetStartMode(m_atHduTable[byRow-1].GetStartMode());
			
            ptHduCfg->SetIpAddr( m_atHduTable[byRow-1].GetIpAddr() );
            ptHduCfg->SetMcuId( m_atHduTable[byRow-1].GetMcuId() );
            ptHduCfg->SetMcuIp( m_atHduTable[byRow-1].GetMcuIp() );
            ptHduCfg->SetMcuPort( m_atHduTable[byRow-1].GetMcuPort() );           
            ptHduCfg->SetType( m_atHduTable[byRow-1].GetType() );
            ptHduCfg->SetAlias( m_atHduTable[byRow-1].GetAlias() );
            THduChnlModePortAgt tHduChnModePort;
			memset(&tHduChnModePort, 0x0, sizeof(tHduChnModePort));
			m_atHduTable[byRow-1].GetHduChnlModePort(0, tHduChnModePort);
			ptHduCfg->SetHduChnlModePort(0, tHduChnModePort);
			m_atHduTable[byRow-1].GetHduChnlModePort(1, tHduChnModePort);
			ptHduCfg->SetHduChnlModePort(1, tHduChnModePort);
			
			
			// 			ptHduCfg->SetOutModeType(m_atHduTable[byRow-1].GetOutModeType());
			// 			ptHduCfg->SetOutModeType(m_atHduTable[byRow-1].GetOutPortType());
			
            
            bRet = TRUE;
            break;
        }
    }
	
    if( !bRet )
    {
        Agtlog(LOG_ERROR, "[GetHduCfgToBrd] The Hdu ID.%d is not exist,please check it\n", byId);
    }
	
    return bRet;
}

/*=============================================================================
函 数 名： GetSvmpCfgToBrd
功    能： 取Svmp信息
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/30  4.6         江乐斌       创建
=============================================================================*/
BOOL32 CCfgAgent::GetSvmpCfgToBrd( u8 byId, TEqpSvmpEntry * ptSvmpCfg )
{
	if( NULL == ptSvmpCfg || byId < VMPID_MIN || byId > VMPID_MAX )
    {
        Agtlog(LOG_ERROR, "[GetSvmpCfgToBrd] param err: byId<%d>, ptCfg: 0x%x\n", byId, ptSvmpCfg);
        return FALSE;
    }
	
    BOOL32 bRet = FALSE;
	
    for( u8 byRow = 1; byRow <= m_dwSvmpEntryNum; byRow++)
    {
        if( m_atVMPTable[byRow-1].GetEqpId() == byId )
        {
            ptSvmpCfg->SetEqpId( m_atSvmpTable[byRow-1].GetEqpId() );
            ptSvmpCfg->SetMcuRecvPort( m_atSvmpTable[byRow-1].GetMcuRecvPort() );
            ptSvmpCfg->SetSwitchBrdId( m_atSvmpTable[byRow-1].GetSwitchBrdId() );
            
            ptSvmpCfg->SetIpAddr( m_atSvmpTable[byRow-1].GetIpAddr() );
            ptSvmpCfg->SetRunBrdId( m_atSvmpTable[byRow-1].GetRunBrdId() );
            ptSvmpCfg->SetEqpRecvPort( m_atSvmpTable[byRow-1].GetEqpRecvPort() );
			
            ptSvmpCfg->SetMcuId( m_atSvmpTable[byRow-1].GetMcuId() );
            ptSvmpCfg->SetMcuIp( m_atSvmpTable[byRow-1].GetMcuIp() );
            ptSvmpCfg->SetMcuPort( m_atSvmpTable[byRow-1].GetMcuPort() );
            ptSvmpCfg->SetType( m_atSvmpTable[byRow-1].GetType() );
            ptSvmpCfg->SetAlias( m_atSvmpTable[byRow-1].GetAlias() );
			
            bRet = TRUE;
            break;
        }
    }
	
    if( !bRet )
    {
        Agtlog(LOG_ERROR, "[GetSvmpCfgToBrd] The vmp ID.%d is not exist,please check it\n", byId);
    }
    return bRet;
}      

/*=============================================================================
函 数 名： GetDvmpCfgToBrd
功    能： 取Dvmp信息
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/30  4.6         江乐斌       创建
=============================================================================*/
// BOOL32 CCfgAgent::GetDvmpCfgToBrd( u8 byId, TEqpDvmpBasicEntry * ptDvmpBasicCfg )   
// {
// 	if( NULL == ptDvmpBasicCfg || byId < VMPID_MIN || byId > VMPID_MAX )
//     {
//         Agtlog(LOG_ERROR, "[GetDvmpCfgToBrd] param err: byId<%d>, ptDvmpBasicCfg: 0x%x\n", byId, ptDvmpBasicCfg);
//         return FALSE;
//     }
// 	
//     BOOL32 bRet = FALSE;
// 	
//     for( u8 byRow = 1; byRow <= m_dwDvmpEntryNum; byRow++)
//     {
//         if( m_atVMPTable[byRow-1].GetEqpId() == byId )
//         {
//             ptDvmpBasicCfg->SetEqpId( m_atDvmpBasicTable[byRow-1].GetEqpId() );
//             ptDvmpBasicCfg->SetMcuRecvPort( m_atDvmpBasicTable[byRow-1].GetMcuRecvPort() );
//             ptDvmpBasicCfg->SetSwitchBrdId( m_atDvmpBasicTable[byRow-1].GetSwitchBrdId() );
//             
//             ptDvmpBasicCfg->SetIpAddr( m_atDvmpBasicTable[byRow-1].GetIpAddr() );
//             ptDvmpBasicCfg->SetRunBrdId( m_atDvmpBasicTable[byRow-1].GetRunBrdId() );
//             ptDvmpBasicCfg->SetEqpRecvPort( m_atDvmpBasicTable[byRow-1].GetEqpRecvPort() );
// 			
//             ptDvmpBasicCfg->SetMcuId( m_atDvmpBasicTable[byRow-1].GetMcuId() );
//             ptDvmpBasicCfg->SetMcuIp( m_atDvmpBasicTable[byRow-1].GetMcuIp() );
//             ptDvmpBasicCfg->SetMcuPort( m_atDvmpBasicTable[byRow-1].GetMcuPort() );
//             ptDvmpBasicCfg->SetType( m_atDvmpBasicTable[byRow-1].GetType() );
//             ptDvmpBasicCfg->SetAlias( m_atDvmpBasicTable[byRow-1].GetAlias() );
// 		
//             bRet = TRUE;
//             break;
//         }
//     }
// 	
//     if( !bRet )
//     {
//         Agtlog(LOG_ERROR, "[GetDvmpCfgToBrd] The vmp ID.%d is not exist,please check it\n", byId);
//     }
//     return bRet;
// }  
/*=============================================================================
函 数 名： GetMpuBasCfgToBrd
功    能： 取MpuBas信息
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/30  4.6         江乐斌       创建
=============================================================================*/
BOOL32 CCfgAgent::GetMpuBasCfgToBrd( u8 byId, TEqpMpuBasEntry * ptMpuBasCfg )
{
	if( NULL == ptMpuBasCfg || byId < BASID_MIN || byId > BASID_MAX )
    {
        Agtlog(LOG_ERROR, "[GetMpuBasCfgToBrd] param err: byId<%d>, ptMpuBasCfg: 0x%x\n", byId, ptMpuBasCfg);
        return FALSE;
    }
	
    BOOL32 bRet = FALSE;
    for( u8 byRow = 1; byRow <= m_dwBasEntryNum; byRow++)
    {
        if( m_atBasTable[byRow-1].GetEqpId() == byId )
        {
            ptMpuBasCfg->SetEqpId( m_atMpuBasTable[byRow-1].GetEqpId() );
            ptMpuBasCfg->SetMcuRecvPort( m_atMpuBasTable[byRow-1].GetMcuRecvPort() );
            ptMpuBasCfg->SetSwitchBrdId( m_atMpuBasTable[byRow-1].GetSwitchBrdId() );
            ptMpuBasCfg->SetEqpRecvPort( m_atMpuBasTable[byRow-1].GetEqpRecvPort() );
            ptMpuBasCfg->SetRunBrdId( m_atMpuBasTable[byRow-1].GetRunBrdId() );
            
            ptMpuBasCfg->SetIpAddr( m_atMpuBasTable[byRow-1].GetIpAddr() );
            ptMpuBasCfg->SetMcuId( m_atMpuBasTable[byRow-1].GetMcuId() );
            ptMpuBasCfg->SetMcuIp( m_atMpuBasTable[byRow-1].GetMcuIp() );
            ptMpuBasCfg->SetMcuPort( m_atMpuBasTable[byRow-1].GetMcuPort() );
            ptMpuBasCfg->SetType( m_atMpuBasTable[byRow-1].GetType() );
            ptMpuBasCfg->SetAlias( m_atMpuBasTable[byRow-1].GetAlias() );
			
            bRet = TRUE;
            break;
        }
    }
	
    if( !bRet )
    {
        Agtlog(LOG_ERROR, "[GetMpuBasCfgToBrd] The bas ID.%d is not exist,please check it\n", byId);
    }
    return bRet;
}    

/*=============================================================================
函 数 名： GetEbapCfgToBrd
功    能： 取Ebap信息
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/30  4.6         江乐斌       创建
=============================================================================*/
// BOOL32 CCfgAgent::GetEbapCfgToBrd( u8 byId, TEqpEbapEntry * ptEbapCfg )
// {
// 	if( NULL == ptEbapCfg || byId < BASID_MIN || byId > BASID_MAX )
//     {
//         Agtlog(LOG_ERROR, "[GetEbapCfgToBrd] param err: byId<%d>, ptEbapCfg: 0x%x\n", byId, ptEbapCfg);
//         return FALSE;
//     }
// 	
//     BOOL32 bRet = FALSE;
//     for( u8 byRow = 1; byRow <= m_dwEbapEntryNum; byRow++)
//     {
//         if( m_atEbapTable[byRow-1].GetEqpId() == byId )
//         {
//             ptEbapCfg->SetEqpId( m_atEbapTable[byRow-1].GetEqpId() );
//             ptEbapCfg->SetMcuRecvPort( m_atEbapTable[byRow-1].GetMcuRecvPort() );
//             ptEbapCfg->SetSwitchBrdId( m_atEbapTable[byRow-1].GetSwitchBrdId() );
//             ptEbapCfg->SetEqpRecvPort( m_atEbapTable[byRow-1].GetEqpRecvPort() );
//             ptEbapCfg->SetRunBrdId( m_atEbapTable[byRow-1].GetRunBrdId() );
//             
//             ptEbapCfg->SetIpAddr( m_atEbapTable[byRow-1].GetIpAddr() );
//             ptEbapCfg->SetMcuId( m_atEbapTable[byRow-1].GetMcuId() );
//             ptEbapCfg->SetMcuIp( m_atEbapTable[byRow-1].GetMcuIp() );
//             ptEbapCfg->SetMcuPort( m_atEbapTable[byRow-1].GetMcuPort() );
//             ptEbapCfg->SetType( m_atEbapTable[byRow-1].GetType() );
//             ptEbapCfg->SetAlias( m_atEbapTable[byRow-1].GetAlias() );
//             
//             bRet = TRUE;
//             break;
//         }
//     }
// 	
//     if( !bRet )
//     {
//         Agtlog(LOG_ERROR, "[GetEbapCfgToBrd] The bas ID.%d is not exist,please check it\n", byId);
//     }
//     return bRet;
// }    

/*=============================================================================
函 数 名： GetEvpuCfgToBrd
功    能： 取Evpu信息
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/30  4.6         江乐斌       创建
=============================================================================*/
// BOOL32 CCfgAgent::GetEvpuCfgToBrd( u8 byId, TEqpEvpuEntry * ptEvpuCfg )
// {
// 	if( NULL == ptEvpuCfg || byId < VMPID_MIN || byId > VMPID_MAX )
//     {
//         Agtlog(LOG_ERROR, "[GetEvpuCfgToBrd] param err: byId<%d>, ptEvpuCfg: 0x%x\n", byId, ptEvpuCfg);
//         return FALSE;
//     }
// 	
//     BOOL32 bRet = FALSE;
// 	
//     for( u8 byRow = 1; byRow <= m_dwEvpuEntryNum; byRow++)
//     {
//         if( m_atEvpuTable[byRow-1].GetEqpId() == byId )
//         {
//             ptEvpuCfg->SetEqpId( m_atEvpuTable[byRow-1].GetEqpId() );
//             ptEvpuCfg->SetMcuRecvPort( m_atEvpuTable[byRow-1].GetMcuRecvPort() );
//             ptEvpuCfg->SetSwitchBrdId( m_atEvpuTable[byRow-1].GetSwitchBrdId() );
//             
//             ptEvpuCfg->SetIpAddr( m_atEvpuTable[byRow-1].GetIpAddr() );
//             ptEvpuCfg->SetRunBrdId( m_atEvpuTable[byRow-1].GetRunBrdId() );
//             ptEvpuCfg->SetEqpRecvPort( m_atEvpuTable[byRow-1].GetEqpRecvPort() );
// 			
//             ptEvpuCfg->SetMcuId( m_atEvpuTable[byRow-1].GetMcuId() );
//             ptEvpuCfg->SetMcuIp( m_atEvpuTable[byRow-1].GetMcuIp() );
//             ptEvpuCfg->SetMcuPort( m_atEvpuTable[byRow-1].GetMcuPort() );
//             ptEvpuCfg->SetType( m_atEvpuTable[byRow-1].GetType() );
//             ptEvpuCfg->SetAlias( m_atEvpuTable[byRow-1].GetAlias() );
// 		
//             bRet = TRUE;
//             break;
//         }
//     }
// 	
//     if( !bRet )
//     {
//         Agtlog(LOG_ERROR, "[GetEvpuCfgToBrd] The vmp ID.%d is not exist,please check it\n", byId);
//     }
//     return bRet;
// }    


/*=============================================================================
函 数 名： GetBrdSlotString
功    能： 取单板槽字符串
算法实现： 
全局变量： 
参    数： u8 bySlot
s8* pchStr
返 回 值： void 
=============================================================================*/
void CCfgAgent::GetBrdSlotString(u8 bySlot, u8 byType, s8* pszStr)
{
    if( NULL == pszStr)
    {
        return;
    }
    if (BRD_TYPE_HDU/*BRD_HWID_KDM200_HDU*/ == byType ||
		BRD_TYPE_HDU_L == byType || //4.6 新加
		BRD_TYPE_HDU2 == byType ||
		BRD_TYPE_HDU2_L == byType ||
		BRD_TYPE_HDU2_S == byType
		)     
    {
		if (bySlot > 6)
		{
			return;
		}
    }
	else if( bySlot > MAX_BRD_NUM) // 最多16槽
    {
        return;
    }
	
	//  [1/21/2011 chendaiwei]支持MPC2
    if( BRD_TYPE_MPC == byType || BRD_TYPE_MPC2 == byType)   //BRD_TYPE_MPC
    {
        sprintf(pszStr, "mc%d", bySlot);
    }
	else if ( BRD_TYPE_HDU/*BRD_HWID_KDM200_HDU*/ == byType
		|| BRD_TYPE_HDU_L == byType
		|| BRD_TYPE_HDU2 == byType
		|| BRD_TYPE_HDU2_L == byType
		|| BRD_TYPE_HDU2_S == byType)
	{
        sprintf(pszStr, "tvsex%d", bySlot);
	}
	//[2011/01/25 zhushz]IS2.x单板支持
	else if (BRD_TYPE_IS21 == byType ||
		BRD_TYPE_IS22 == byType)
	{
		sprintf(pszStr, "is%d", bySlot);
	}
    else
    {
        sprintf(pszStr, "ex%d", bySlot);
    }
    return;
}

/*=============================================================================
函 数 名： CheckEqpRow
功    能： 检测外设行号是否正确
算法实现： 
全局变量： 
参    数： u8 byRow
返 回 值： BOOL32 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
BOOL32 CCfgAgent::CheckEqpRow( u8 byType, u8 byRow, BOOL32 bHDFlag )
{
    BOOL32 bRet = FALSE;
	
    if ( 0 == byRow )        
        return bRet;
	
    switch( byType )
    {
    case EQP_TYPE_MIXER:    if (byRow <= m_dwMixEntryNum)   bRet = TRUE;    break;
		//tianzhiyong  20100310  增加一种混音器外设类型
	//case EQP_TYPE_EMIXER:   if (byRow <= m_dwMixEntryNum)   bRet = TRUE;    break;
	// 4.7版本 apu2 [1/31/2012 chendaiwei]
	//case EQP_TYPE_APU2_MIXER:   if (byRow <= m_dwMixEntryNum)   bRet = TRUE;    break;
    case EQP_TYPE_VMP:      if (byRow <= m_dwVMPEntryNum)   bRet = TRUE;    break;
    case EQP_TYPE_RECORDER: if (byRow <= m_dwRecEntryNum)   bRet = TRUE;    break;
    case EQP_TYPE_BAS: 
		if ( !bHDFlag )
		{
			if (byRow <= m_dwBasEntryNum)   bRet = TRUE;    break;
		}
		else
		{
			if (byRow <= m_dwBasHDEntryNum) bRet = TRUE;    break;
		}
    case EQP_TYPE_TVWALL:   if (byRow <= m_dwTVWallEntryNum) bRet = TRUE;   break;
    case EQP_TYPE_PRS:      if (byRow <= m_dwPrsEntryNum)   bRet = TRUE;    break;
    case EQP_TYPE_VMPTW:    if (byRow <= m_dwMpwEntryNum)   bRet = TRUE;    break;
		//4.6版本 新加外设 jlb
    case EQP_TYPE_HDU:      if (byRow <= m_dwHduEntryNum) bRet = TRUE;      break;
    case EQP_TYPE_FILEENC:
    case EQP_TYPE_DCS:
    default:
        Agtlog(LOG_ERROR, "[CheckEqpRow] unexpected eqp type<%d> recieved, ignore it !\n", byType);
        break;
    }
    if ( !bRet )
        Agtlog(LOG_ERROR, "[CheckEqpRow] Eqp<%d>'s row<%d> unexist, check failed !\n", byType, byRow);
	
    return bRet;
}

/*=============================================================================
函 数 名： GetDcsIp
功    能： 取DcsIp
算法实现： 
全局变量： 
参    数： void
返 回 值： u32  
=============================================================================*/
u32  CCfgAgent::GetDcsIp( void )
{
    return htonl(m_tDCS.m_dwDcsIp);
}

/*=============================================================================
函 数 名： SetDcsIp
功    能： 设置DcsIp
算法实现： 
全局变量： 
参    数： u32 dwDcsIp
返 回 值： BOOL32 
=============================================================================*/
u16 CCfgAgent::SetDcsIp( u32 dwDcsIp )
{
    BOOL32 bFlag = FALSE;
    u8 achIpStr[32];
    memset((s8*)achIpStr, 0, sizeof(achIpStr));
    m_tDCS.m_dwDcsIp = ntohl(dwDcsIp);
    GetIpFromU32((s8*)achIpStr, dwDcsIp);
    u16 wRet = WriteStringToFile(SECTION_mcueqpDataConfServer, KEY_mcueqpDcsIp, (s8*)achIpStr);
	
    if ( SUCCESS_AGENT != wRet )
        printf("[SetDcsIp] failed, wRet.%d \n", wRet );
    return wRet;
}

/*=============================================================================
函 数 名： AgentDirCreate
功    能： 建立目录
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
=============================================================================*/
void CCfgAgent::AgentDirCreate( const s8* pPathName )
{
#ifdef WIN32
	CreateDirectory( ( LPCTSTR )pPathName, NULL );
#elif defined( _VXWORKS_ )
	mkdir( ( s8* )pPathName );
#endif
#ifdef _LINUX_
    DIR* pdir = opendir(pPathName);
    if (pdir == NULL)
    {
        // 不存在则创建
		s32 nRet = mkdir( ( s8* )pPathName, 0777 );
		if( 0 != nRet )
		{
			printf("make diretory Error :%s\n",strerror(errno)); 
            return;
		}        
    }
    else
    {
        closedir(pdir);
    }    
    chmod(pPathName, 0777);
#endif
    
	return;
}

/*=============================================================================
函 数 名： GetQosInfo
功    能： 取Qos信息
算法实现： 
全局变量： 
参    数： TQosInfo* ptQosInfo
返 回 值： BOOL32 
=============================================================================*/
u16 CCfgAgent::GetQosInfo( TQosInfo* ptQosInfo)
{
    u16 wRet = ERR_AGENT_GETNODEVALUE;
    if( NULL != ptQosInfo)    
    {
		*ptQosInfo = m_tMcuQosInfo;
        wRet = SUCCESS_AGENT;
    }
    return wRet;
}

/*=============================================================================
函 数 名： GetNetWorkInfo
功    能： 取网络信息
算法实现： 
全局变量： 
参    数： TNetWorkInfo* ptNetWorkInfo
返 回 值： BOOL32 
=============================================================================*/
u16 CCfgAgent::GetNetWorkInfo( TNetWorkInfo* ptNetWorkInfo)
{
	memcpy( ptNetWorkInfo, &m_tMcuNetwork, sizeof(TNetWorkInfo));
    return SUCCESS_AGENT;
}

/*=============================================================================
函 数 名： GetLocalInfo
功    能： 取本地信息
算法实现： 
全局变量： 
参    数： TLocalInfo* ptLocalInfo
返 回 值： BOOL32 
=============================================================================*/
u16 CCfgAgent::GetLocalInfo( TLocalInfo* ptLocalInfo )
{
    u16 wRet = ERR_AGENT_GETTABLEVALUE;
    if( NULL != ptLocalInfo )    
    {
		*ptLocalInfo = m_tMcuLocalInfo;
        wRet = SUCCESS_AGENT;
    }
    return wRet;
}

/*=============================================================================
函 数 名： GetBrdTypeStr
功    能： 取单板类型字符串
算法实现： 
全局变量： 
参    数： u8 byType
返 回 值： LPCSTR 
=============================================================================*/
LPCSTR CCfgAgent::GetBrdTypeStr(u8 byType)
{
    LPCSTR pszBrdType = NULL;
	
    switch( byType )
    {
    case BRD_TYPE_MPC/*DSL8000_BRD_MPC*/:       pszBrdType = BOARD_TYPE_MPC;        break;
		//  [1/21/2011 chendaiwei]支持MPC2
	case BRD_TYPE_MPC2:							pszBrdType = BOARD_TYPE_MPC2;		break;
    case BRD_TYPE_DTI/*DSL8000_BRD_DTI*/:       pszBrdType = BOARD_TYPE_DTI;        break;
    case BRD_TYPE_CRI/*DSL8000_BRD_CRI*/:       pszBrdType = BOARD_TYPE_CRI;        break;
		//  [1/21/2011 chendaiwei]支持CRI2
    case BRD_TYPE_CRI2:							pszBrdType = BOARD_TYPE_CRI2;       break;
    case BRD_TYPE_DIC/*DSL8000_BRD_DIC*/:       pszBrdType = BOARD_TYPE_DIC;        break;
    case BRD_TYPE_DRI/*DSL8000_BRD_DRI*/:       pszBrdType = BOARD_TYPE_DRI;        break;
		//  [1/21/2011 chendaiwei]支持DRI2	
    case BRD_TYPE_DRI2/*DSL8000_BRD_DRI2*/:     pszBrdType = BOARD_TYPE_DRI2;       break;
    case BRD_TYPE_MMP/*DSL8000_BRD_MMP*/:       pszBrdType = BOARD_TYPE_MMP;        break;
    case BRD_TYPE_VPU/*DSL8000_BRD_VPU*/:       pszBrdType = BOARD_TYPE_VPU;        break;
    case BRD_TYPE_DEC5/*DSL8000_BRD_DEC5*/:      pszBrdType = BOARD_TYPE_DEC5;       break;
    case BRD_TYPE_VAS/*DSL8000_BRD_VAS*/:       pszBrdType = BOARD_TYPE_VAS;        break;
    case BRD_TYPE_IMT/*DSL8000_BRD_IMT*/:       pszBrdType = BOARD_TYPE_IMT;        break;
    case BRD_TYPE_APU/*DSL8000_BRD_APU*/:       pszBrdType = BOARD_TYPE_APU;        break;
	case BRD_TYPE_EAPU:         pszBrdType = BOARD_TYPE_EAPU;        break;//tianzhiyong 100205 EAPU
    case BRD_TYPE_DSI/*DSL8000_BRD_DSI*/:       pszBrdType = BOARD_TYPE_DSI;        break;
	case BRD_TYPE_MDSC/*DSL8000_BRD_MDSC*/:      pszBrdType = BOARD_TYPE_MDSC;       break;
    case BRD_TYPE_HDSC/*DSL8000_BRD_HDSC*/:      pszBrdType = BOARD_TYPE_HDSC;       break;    
	case BRD_TYPE_DSC/*KDV8000B_MODULE*/:       pszBrdType = BOARD_TYPE_DSC;        break;
		//4.6版本 新加 jlb
	case BRD_TYPE_MPU/*BRD_HWID_DSL8000_MPU*/:   pszBrdType = BOARD_TYPE_MPU;        break;
	case BRD_HWID_EBAP:          pszBrdType = BOARD_TYPE_EBAP;       break;
	case BRD_HWID_EVPU:          pszBrdType = BOARD_TYPE_EVPU;       break;
	case BRD_TYPE_HDU/*BRD_HWID_KDM200_HDU*/:    pszBrdType = BOARD_TYPE_HDU;        break;
	case BRD_TYPE_HDU_L/*BRD_PID_KDM200_HDU_L*/:   pszBrdType = BOARD_TYPE_HDU_L;     break;
	case BRD_TYPE_HDU2:   pszBrdType = BOARD_TYPE_HDU2;     break;
	case BRD_TYPE_HDU2_L:  pszBrdType = BOARD_TYPE_HDU2_L;   break;
	case BRD_TYPE_HDU2_S:  pszBrdType = BOARD_TYPE_HDU2_S;   break;
		//  [12/5/2010 chendaiwei]增加IS2.2类型字符串
		//[2011/01/25 zhushz]IS2.x单板支持
	case BRD_TYPE_IS21:			pszBrdType = BOARD_TYPE_IS21;       break;
	case BRD_TYPE_IS22:	        pszBrdType = BOARD_TYPE_IS22;       break;
	// 4.7版本 [1/31/2012 chendaiwei]
	case BRD_TYPE_MPU2:					pszBrdType = BOARD_TYPE_MPU2;   break;
	case BRD_TYPE_MPU2ECARD:			pszBrdType = BOARD_TYPE_MPU2ECARD;   break;
	case BRD_TYPE_APU2:					pszBrdType = BOARD_TYPE_APU2;   break;

		// guzh [4/27/2007] 目前暂时没有8000C Mobule
#ifdef _LINUX_
		// [pengjie 2010/3/13] 目前mpc2还没有KDV8000C_MODULE 等定义
#ifndef _LINUX12_
    case KDV8000C_MODULE:       pszBrdType = BOARD_TYPE_DSC;        break;
#endif
#endif
    case KDV8000B_BOARD:        pszBrdType = BOARD_TYPE_MPCB;       break;
#ifdef _LINUX_
#ifndef _LINUX12_
    case KDV8000C_BOARD:        pszBrdType = BOARD_TYPE_MPCC;       break;
#endif
#endif
    default:                    pszBrdType = BOARD_TYPE_UNKNOW;     break;
    }
    return pszBrdType;
}

/*=============================================================================
函 数 名： GetImageFileName
功    能： 通过单板类型获得单板image文件名
算法实现： 
全局变量： 
参    数： u8 byBrdType
s8 *pszFileName
返 回 值： s8*
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/09/20  4.0         张宝卿       创建
=============================================================================*/
BOOL32 CCfgAgent::GetImageFileName( u8 byBrdType, s8 *pszFileName )
{
    BOOL32 bRet = TRUE;
	
    switch( byBrdType )
    {
    case BRD_TYPE_MPC/*DSL8000_BRD_MPC*/:       
        memcpy(pszFileName, "/ramdisk/mcu.image",    sizeof("/ramdisk/mcu.image"));
        break;
		//  [1/21/2011 chendaiwei]Fix me:确认是MPC2否有image文件
    case BRD_TYPE_MPC2/*DSL8000_BRD_MPC2*/:       
        memcpy(pszFileName, "/ramdisk/mcu2.image",    sizeof("/ramdisk/mcu2.image"));
        break;
    case BRD_TYPE_APU/*DSL8000_BRD_APU*/:
        memcpy(pszFileName, "/ramdisk/kdvapu.image", sizeof("/ramdisk/kdvapu.image"));
        break;
    case BRD_TYPE_VPU/*DSL8000_BRD_VPU*/:
        memcpy(pszFileName, "/ramdisk/kdvvpu.image", sizeof("/ramdisk/kdvvpu.image"));
        break;
    case BRD_TYPE_CRI/*DSL8000_BRD_CRI*/:
        memcpy(pszFileName, "/ramdisk/kdvcri.image", sizeof("/ramdisk/kdvcri.image"));
        break;
		//  [1/21/2011 chendaiwei]Fix me:确认是CRI2否有image文件
	case BRD_TYPE_CRI2:
        memcpy(pszFileName, "/ramdisk/kdvcri2.image", sizeof("/ramdisk/kdvcri2.image"));
        break;
    case BRD_TYPE_DRI/*DSL8000_BRD_DRI*/:
        memcpy(pszFileName, "/ramdisk/kdvdri.image", sizeof("/ramdisk/kdvdri.image"));
        break; 
		//  [1/21/2011 chendaiwei]Fix me:确认是DRI2否有image文件
	case BRD_TYPE_DRI2:
        memcpy(pszFileName, "/ramdisk/kdvdri2.image", sizeof("/ramdisk/kdvdri2.image"));
        break;
		//4.6版本 新加 jlb
    case BRD_TYPE_MPU/*BRD_HWID_DSL8000_MPU*/:
        memcpy(pszFileName, "/ramdisk/kdvmpu.image", sizeof("/ramdisk/kdvmpu.image"));
        break;
    case BRD_TYPE_HDU/*BRD_HWID_KDM200_HDU*/:
	case BRD_TYPE_HDU_L:
        memcpy(pszFileName, "/ramdisk/kdvhdu.image", sizeof("/ramdisk/kdvhdu.image"));
        break;
	case BRD_TYPE_HDU2:
	case BRD_TYPE_HDU2_L:
        memcpy(pszFileName, "/ramdisk/kdvhdu2.image", sizeof("/ramdisk/kdvhdu2.image"));
        break;
	case BRD_TYPE_HDU2_S:
 		memcpy(pszFileName, "/ramdisk/kdvhdu2_s.image", sizeof("/ramdisk/kdvhdu2_s.image"));
        break;
    case BRD_HWID_EBAP:
        memcpy(pszFileName, "/ramdisk/kdvebap.image", sizeof("/ramdisk/kdvebap.image"));
        break;
    case BRD_HWID_EVPU:
        memcpy(pszFileName, "/ramdisk/kdvevpu.image", sizeof("/ramdisk/kdvevpu.image"));
        break;
		
    case BRD_TYPE_DSC/*KDV8000B_MODULE*/:
	
	// 4.7版本 [1/31/2012 chendaiwei]
    case BRD_TYPE_MPU2ECARD:
	case BRD_TYPE_MPU2:
        memcpy(pszFileName, "/ramdisk/kdvmpu2.image", sizeof("/ramdisk/kdvmpu2.image"));
        break;
	case BRD_TYPE_APU2:
        memcpy(pszFileName, "/ramdisk/kdvapu2.image", sizeof("/ramdisk/kdvapu2.image"));
        break;

		// guzh [4/27/2007] 目前没有8000CModule
#ifdef _LINUX_
#ifndef _LINUX12_
    case KDV8000C_MODULE:
#endif
#endif
    case BRD_TYPE_HDSC/*DSL8000_BRD_HDSC*/:  
    case BRD_TYPE_MDSC/*DSL8000_BRD_MDSC*/:
        memcpy(pszFileName, "/ramdisk/mdsc.image",   sizeof("/ramdisk/mdsc.image"));
        break;
    case BRD_TYPE_DEC5/*DSL8000_BRD_DEC5*/:
        memcpy(pszFileName, "/ramdisk/kdvtvwall.image", sizeof("/ramdisk/kdvtvwall.image"));
        break;
		
		//EAPU没有相关的IMAGE文件
    case BRD_TYPE_EAPU:
		memcpy(pszFileName, "/ramdisk/kdveapu.image", sizeof("/ramdisk/kdveapu.image"));
        bRet = FALSE;
        break;
		
    case BRD_TYPE_DIC/*DSL8000_BRD_DIC*/:
    case BRD_TYPE_MMP/*DSL8000_BRD_MMP*/:
    case BRD_TYPE_VAS/*DSL8000_BRD_VAS*/:
    case BRD_TYPE_IMT/*DSL8000_BRD_IMT*/:
    case BRD_TYPE_DSI/*DSL8000_BRD_DSI*/:
    case KDV8000B_BOARD:
#ifdef _LINUX_
#ifndef _LINUX12_
    case KDV8000C_BOARD:
#endif
#endif
    case BRD_TYPE_DTI/*DSL8000_BRD_DTI*/:
    default:
		memcpy(pszFileName, "/ramdisk/.image", sizeof("/ramdisk/.image"));
        bRet = FALSE;
        Agtlog( LOG_ERROR, "[GetImageFileName] unexpected brd type.%d received !\n", byBrdType);
        break;
    }
    return bRet;
}



/*=============================================================================
函 数 名： GetBinFileName
功    能： 通过单板类型获得单板Bin文件名
算法实现： 
全局变量： 
参    数： u8 byBrdType
s8 *pszFileName
返 回 值： s8*
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2010/07/27  4.6         陆昆朋       创建
=============================================================================*/
BOOL32 CCfgAgent::GetBinFileName( u8 byBrdType, s8 *pszFileName )
{
	BOOL32 bRet = TRUE;
	
    switch( byBrdType )
    {
    case BRD_TYPE_EAPU:
        memcpy(pszFileName, "/ramdisk/kdveapu.bin",    sizeof("/ramdisk/kdveapu.bin"));
        break;
	default:
		memcpy(pszFileName, "/ramdisk/.bin",    sizeof("/ramdisk/.bin"));
		bRet = FALSE;
		break;
	}
	return bRet;
}

/*=============================================================================
函 数 名： SetQosInfo
功    能： 设置Qos信息
算法实现： 
全局变量： 
参    数： TQosInfo * ptQosInfo
返 回 值： BOOL32 
=============================================================================*/
u16 CCfgAgent::SetQosInfo( TQosInfo * ptQosInfo )
{
    u16 wRet = SUCCESS_AGENT;
    
    memcpy( &m_tMcuQosInfo, ptQosInfo, sizeof(TQosInfo) );
    
    wRet |= WriteIntToFile(SECTION_mcuQosInfo, KEY_mcuQosType, (s32)ptQosInfo->GetQosType());
    wRet |= WriteIntToFile(SECTION_mcuQosInfo, KEY_mcuAudioLevel, (s32)ptQosInfo->GetAudioLevel());
    wRet |= WriteIntToFile(SECTION_mcuQosInfo, KEY_mcuVideoLevel, (s32)ptQosInfo->GetVideoLevel());
    wRet |= WriteIntToFile(SECTION_mcuQosInfo, KEY_mcuDataLevel, (s32)ptQosInfo->GetDataLevel());
    wRet |= WriteIntToFile(SECTION_mcuQosInfo, KEY_mcuSignalLevel, (s32)ptQosInfo->GetSignalLevel());
	
    if( SUCCESS_AGENT == wRet )
    {
        if( QOSTYPE_IP_PRIORITY == ptQosInfo->GetQosType() )
        {
            u8 abyIpServiceStr[16];
            memset(abyIpServiceStr, 0, sizeof(abyIpServiceStr));
            u8 byType = ptQosInfo->GetIpServiceType();
            if( 0 != byType)
            {
                GetQosStrFromNum( byType, abyIpServiceStr, sizeof(abyIpServiceStr) );
            }
            else
            {
                memcpy(abyIpServiceStr, "0", sizeof("0"));
            }
            wRet = WriteStringToFile(SECTION_mcuQosInfo, KEY_mcuIpServiceType, (const char*)abyIpServiceStr);
        }
    }
    return wRet;
}

#ifdef _MINIMCU_
/*=============================================================================
函 数 名： SetDSCInfo
功    能： 设置mcu8000B特殊配置信息
算法实现： 
全局变量： 
参    数： TDSCInfo * ptMcuInfo, BOOL32 bToFile
返 回 值： u16 
=============================================================================*/
u16 CCfgAgent::SetDSCInfo( TDSCModuleInfo * ptMcuInfo, BOOL32 bToFile )
{
    u16 wRet = SUCCESS_AGENT;
	
#ifdef _MINIMCU_  
    memcpy( &m_tMcuDscInfo, ptMcuInfo, sizeof(TDSCModuleInfo) );
	
	if (!bToFile)
	{
		return SUCCESS_AGENT;
	}
	
	wRet |= WriteIntToFile(SECTION_DscModule, KEY_DscMp, (s32)ptMcuInfo->IsStartMp());
	wRet |= WriteIntToFile(SECTION_DscModule, KEY_DscMtAdp, (s32)ptMcuInfo->IsStartMtAdp());
	wRet |= WriteIntToFile(SECTION_DscModule, KEY_DscGk, (s32)ptMcuInfo->IsStartGk());
	wRet |= WriteIntToFile(SECTION_DscModule, KEY_DscProxy, (s32)ptMcuInfo->IsStartProxy());
	wRet |= WriteIntToFile(SECTION_DscModule, KEY_DscDcs, (s32)ptMcuInfo->IsStartDcs()); 
	
	s8 achIpAddr[32] = {0};
	
	GetIpFromU32(achIpAddr, htonl(ptMcuInfo->GetDscInnerIp()) );
	
	wRet |= WriteStringToFile(SECTION_DscModule, KEY_DscInnerIp, achIpAddr); 
	
	wRet |= WriteIntToFile(SECTION_DscModule, KEY_NetType, (s32)ptMcuInfo->GetNetType());
	
	wRet |= WriteIntToFile(SECTION_DscModule, KEY_LanMtProxyIpPos, (s32)ptMcuInfo->GetLanMtProxyIpPos());
	
	u8 byLop = 0;
	u8 byIpNum = 0;
    u8 byNetType = 0;
	u32 dwIpAddr = 0;
	s8 asIpAddrStr[32];
	u32 dwIpMask = 0;
	s8 asIpMaskStr[32];
	u32 dwGWIpAddr = 0;
	s8 asGWIpAddrStr[32];
	s8 asEntryStr[32];
	
	s8 asAlias[MAXLEN_PWD];
	s8 asMcsAccessAddrStr[128];
	s8 asCallAddrStr[128];
	
	TMINIMCUNetParam tNetParam;
	TMINIMCUNetParamAll tNetParamAll;
	memset( &tNetParam, 0, sizeof(tNetParam) );
	memset( &tNetParamAll, 0, sizeof(tNetParamAll) );
	
	// 写MCSAccessAddr
	byIpNum = ptMcuInfo->GetMcsAccessAddrNum();
	ptMcuInfo->GetMcsAccessAddrAll( tNetParamAll );
	wRet |= WriteIntToFile(SECTION_DscModule, KEY_MCSAccessAddrNum, (s32)byIpNum); 
	for ( byLop = 0; byLop < byIpNum; byLop++ )
	{
		memset( asEntryStr, 0, sizeof(asEntryStr) );
		sprintf(asEntryStr, "%s%d", ENTRY_MCSAccessAddr, byLop);
		
		if ( !tNetParamAll.GetNetParambyIdx( byLop, tNetParam) )
		{
			printf("[MCUAGT][SetDSCInfo]McsAccessAddr loop %d error!\n", byLop);
			Agtlog(LOG_ERROR, "[SetDSCInfo]McsAccessAddr loop %d error!\n", byLop);
			continue;
		}		
		
        byNetType = tNetParam.GetNetworkType();
        dwIpAddr = tNetParam.GetIpAddr();
        dwIpMask = tNetParam.GetIpMask();
		dwGWIpAddr = tNetParam.GetGatewayIp();
		
        GetIpFromU32(asIpAddrStr, htonl(dwIpAddr));
        GetIpFromU32(asIpMaskStr, htonl(dwIpMask));
        GetIpFromU32(asGWIpAddrStr, htonl(dwGWIpAddr));
		
		memset( asAlias, 0, sizeof(asAlias) );
		strncpy( asAlias, tNetParam.GetServerAlias(), MAXLEN_PWD );
		
		memset( asMcsAccessAddrStr, 0, sizeof(asMcsAccessAddrStr) );
		sprintf( asMcsAccessAddrStr, "%d\t%s\t%s\t%s\t%s", byNetType, asIpAddrStr, asIpMaskStr, asGWIpAddrStr, asAlias );
		
		wRet |= WriteStringToFile(SECTION_DscModule, asEntryStr, asMcsAccessAddrStr); 
	}
	
	// 写CallAddr
	memset( &tNetParam, 0, sizeof(tNetParam) );
	memset( &tNetParamAll, 0, sizeof(tNetParamAll) );
	
	byIpNum = ptMcuInfo->GetCallAddrNum();
	ptMcuInfo->GetCallAddrAll( tNetParamAll );
	wRet |= WriteIntToFile(SECTION_DscModule, KEY_CallAddrNum, (s32)byIpNum); 
	for ( byLop = 0; byLop < byIpNum; byLop++ )
	{
		memset( asEntryStr, 0, sizeof(asEntryStr) );
		sprintf(asEntryStr, "%s%d", ENTRY_CallAddr, byLop);
		
		if ( !tNetParamAll.GetNetParambyIdx(byLop, tNetParam) )
		{
			printf("[MCUAGT][SetDSCInfo]McsAccessAddr loop %d error!\n", byLop);
			Agtlog(LOG_ERROR, "[SetDSCInfo]McsAccessAddr loop %d error!\n", byLop);
			continue;
		}
		
        byNetType = tNetParam.GetNetworkType();
        dwIpAddr = tNetParam.GetIpAddr();
        dwIpMask = tNetParam.GetIpMask();
		dwGWIpAddr = tNetParam.GetGatewayIp();
		
		GetIpFromU32(asIpAddrStr, htonl(dwIpAddr));
        GetIpFromU32(asIpMaskStr, htonl(dwIpMask));
        GetIpFromU32(asGWIpAddrStr, htonl(dwGWIpAddr));
		
        memset( asAlias, 0, sizeof(asAlias) );
        strncpy( asAlias, tNetParam.GetServerAlias(), MAXLEN_PWD );
        
        memset( asCallAddrStr, 0, sizeof(asCallAddrStr) );
        sprintf( asCallAddrStr, "%d\t%s\t%s\t%s\t%s", byNetType, asIpAddrStr, asIpMaskStr, asGWIpAddrStr, asAlias );
		
		wRet |= WriteStringToFile(SECTION_DscModule, asEntryStr, asCallAddrStr); 
	}
#endif        
    return wRet;
}

/*=============================================================================
函 数 名： SaveDscLocalInfoToNip
功    能： 
算法实现： 
全局变量： 
参    数： TDSCModuleInfo * ptDscInfo
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/7/17   4.0			周广程                  创建
=============================================================================*/
BOOL32 CCfgAgent::SaveDscLocalInfoToNip( TDSCModuleInfo * ptDscInfo )
{
#ifdef _MINIMCU_  
	u32 dwLop = 0;
	TDSCModuleInfo tNewDscInfo;
	memcpy( &tNewDscInfo, ptDscInfo, sizeof(TDSCModuleInfo) );
	u8 byMcsAccessAddrNum = tNewDscInfo.GetMcsAccessAddrNum();
	if ( 0 == byMcsAccessAddrNum )
	{
		Agtlog( LOG_ERROR, "[SaveDscInfoToNip] The mpc ip num can't be 0!\n");
		return FALSE;
	}
	
	TMINIMCUNetParamAll tNetParamAll;
	TMINIMCUNetParam tNetParam;
	tNewDscInfo.GetMcsAccessAddrAll( tNetParamAll );
	
#ifdef _LINUX_
	// 设置IP
	TBrdEthParamAll tBrdEthParamAll;
	STATUS nRet = AgtGetBrdEthParamAll( 0, &tBrdEthParamAll ); 
	if ( ERROR == nRet )
	{
		Agtlog(LOG_ERROR, "[SaveDscInfoToNip] AgtGetBrdEthParamAll() failed, wRet.%d \n", nRet );
		return FALSE;
	}
	u32 dwIpNum = tBrdEthParamAll.dwIpNum;
	if ( dwIpNum > 1 )
	{
		// 删掉所有辅IP
		for ( dwLop = dwIpNum-1; dwLop > 0; dwLop-- )
		{
			printf("[SaveDscInfoToNip] Del sec ip.0x%x, mask.0x%x \n", 
				ntohl(tBrdEthParamAll.atBrdEthParam[dwLop].dwIpAdrs), 
				ntohl(tBrdEthParamAll.atBrdEthParam[dwLop].dwIpMask));
			nRet = BrdSetSecondEthParam(0, Brd_DEL_SEC_IP_AND_MASK, &tBrdEthParamAll.atBrdEthParam[dwLop]);
			if ( ERROR == nRet )
			{
				Agtlog(LOG_ERROR, "[SaveDscInfoToNip] Del sec ip.0x%x, mask.0x%x failed, wRet.%d \n", 
					ntohl(tBrdEthParamAll.atBrdEthParam[dwLop].dwIpAdrs), 
					ntohl(tBrdEthParamAll.atBrdEthParam[dwLop].dwIpMask), 
					nRet );
				return FALSE;
			}
		}
	}
	tNetParamAll.GetNetParambyIdx(0, tNetParam);
	TBrdEthParam tOneParam;
	tOneParam.dwIpAdrs = htonl( tNetParam.GetIpAddr() );
	tOneParam.dwIpMask = htonl( tNetParam.GetIpMask() );
	printf("[SaveDscInfoToNip] BrdSetEthParam() , ip.0x%x, mask.0x%x \n", 
		tNetParam.GetIpAddr(), tNetParam.GetIpMask());
	nRet = BrdSetEthParam(0, Brd_SET_IP_AND_MASK, &tOneParam);
	if ( ERROR == nRet )
	{
		Agtlog(LOG_ERROR, "[SaveDscInfoToNip] BrdSetEthParam() failed, ip.0x%x, mask.0x%x, wRet.%d \n", 
			tNetParam.GetIpAddr(), tNetParam.GetIpMask(), nRet );
		return FALSE;
	}
	for ( dwLop = 1; dwLop < byMcsAccessAddrNum; dwLop++ )
	{
		memset( &tNetParam, 0, sizeof(tNetParam) );
		tNetParamAll.GetNetParambyIdx(dwLop, tNetParam);
		tOneParam.dwIpAdrs = htonl( tNetParam.GetIpAddr() );
		tOneParam.dwIpMask = htonl( tNetParam.GetIpMask() );
		printf( "[SaveDscInfoToNip] Set sec ip.0x%x, mask.0x%x \n", 
			tNetParam.GetIpAddr(), tNetParam.GetIpMask());
		nRet = BrdSetSecondEthParam(0, Brd_SET_SEC_IP_AND_MASK, &tOneParam);
		if ( ERROR == nRet )
		{
			Agtlog(LOG_ERROR, "[SaveDscInfoToNip] Set sec ip.0x%x, mask.0x%x failed, wRet.%d \n", 
				tNetParam.GetIpAddr(), tNetParam.GetIpMask(), nRet );
			return FALSE;
		}
	}
	BrdSaveNipConfig();
	
#endif
#endif // _MINIMCU_
	
	return TRUE;
}

/*=============================================================================
函 数 名： SaveRouteToNipByDscInfo
功    能： 
算法实现： 
全局变量： 
参    数： TDSCModuleInfo *ptDscInfo
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/9/21   4.0			周广程                  创建
=============================================================================*/
BOOL32 CCfgAgent::SaveRouteToNipByDscInfo( TDSCModuleInfo *ptDscInfo )
{
#ifdef _MINIMCU_ 
	u32 dwLop = 0;
	TDSCModuleInfo tNewDscInfo;
	memcpy( &tNewDscInfo, ptDscInfo, sizeof(TDSCModuleInfo) );
	u8 byMcsAccessAddrNum = tNewDscInfo.GetMcsAccessAddrNum();
	if ( 0 == byMcsAccessAddrNum )
	{
		Agtlog( LOG_ERROR, "[SaveRouteToNipByDscInfo] The mpc ip num can't be 0!\n");
		return FALSE;
	}
	
	TMINIMCUNetParamAll tNetParamAll;
	TMINIMCUNetParam tNetParam;
	tNewDscInfo.GetMcsAccessAddrAll( tNetParamAll );
	u32 dwIpNum = tNetParamAll.GetNetParamNum();
	
#ifdef _LINUX_
	STATUS nRet = OK;
	// 设置路由
	// 删除默认路由
	if ( 0 != BrdGetDefGateway() )
	{
		nRet = BrdDelDefGateway();
		if ( ERROR == nRet )
		{
			OspPrintf(TRUE, FALSE, "[SaveRouteToNipByDscInfo] Del DefGateway error!\n");
		}
	}
	BrdSaveNipConfig();
	
	// 删除所有普通路由
	TBrdAllIpRouteInfo tBrdAllIpRouteInfo;
	nRet = BrdGetAllIpRoute( &tBrdAllIpRouteInfo );
	if ( ERROR == nRet )
	{
		OspPrintf(TRUE, FALSE, "[SaveRouteToNipByDscInfo] Get Ip routes error!\n" );
	}
	u32 dwIpRouteNum = tBrdAllIpRouteInfo.dwIpRouteNum;
	for ( dwLop = 0; dwLop < dwIpRouteNum ; dwLop++ )
	{
		nRet = BrdDelOneIpRoute( &tBrdAllIpRouteInfo.tBrdIpRouteParam[dwLop] );
		if ( ERROR == nRet )
		{
			OspPrintf(TRUE, FALSE, "[SaveRouteToNipByDscInfo] Del Ip routes error! DesIpNet.0x%x, DesIpMask.0x%x, GwIp.0x%x\n",
				tBrdAllIpRouteInfo.tBrdIpRouteParam[dwLop].dwDesIpNet,
				tBrdAllIpRouteInfo.tBrdIpRouteParam[dwLop].dwDesIpMask,
				tBrdAllIpRouteInfo.tBrdIpRouteParam[dwLop].dwGwIpAdrs);
		}
	}
	// 添加路由
	TBrdIpRouteParam tBrdIpRouteParam;
	BOOL32 bFirst = TRUE;
	for ( dwLop = 0; dwLop < dwIpNum; dwLop++ )
	{
		memset( &tBrdIpRouteParam, 0, sizeof(TBrdIpRouteParam) );
		memset( &tNetParam, 0, sizeof(tNetParam) );
		tNetParamAll.GetNetParambyIdx(dwLop, tNetParam);
		if ( bFirst )
		{
			nRet = BrdSetDefGateway( htonl(tNetParam.GetGatewayIp()) );
			if ( ERROR == nRet )
			{
				printf( "[SaveRouteToNipByDscInfo] Add Ip routes error! GwIp.0x%x\n", 
					tNetParam.GetGatewayIp());
			}
			else
			{
				bFirst = FALSE;
			}
		}
	}
	BrdSaveNipConfig();
	
#endif
#endif // _MINIMCU_
	
	return TRUE;
}

/*=============================================================================
函 数 名： GetLastDscInfo
功    能： 
算法实现： 
全局变量： 
参    数： TDSCModuleInfo * ptDscInfo
返 回 值： u16 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/6/28   4.0			周广程                  创建
=============================================================================*/
u16 CCfgAgent::GetLastDscInfo( TDSCModuleInfo * ptDscInfo )
{
	memcpy( ptDscInfo, &m_tLastMcuDscInfo, sizeof(TDSCModuleInfo) );
    return SUCCESS_AGENT;
}

/*=============================================================================
函 数 名： SetLastDscInfo
功    能： 
算法实现： 
全局变量： 
参    数： TDSCModuleInfo * ptDscInfo
返 回 值： u16 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/6/28   4.0			周广程                  创建
=============================================================================*/
u16 CCfgAgent::SetLastDscInfo( TDSCModuleInfo * ptDscInfo )
{
	memcpy( &m_tLastMcuDscInfo, ptDscInfo, sizeof(TDSCModuleInfo) );
    return SUCCESS_AGENT;
}


/*=============================================================================
函 数 名： SetDscInnerIp
功    能： 设置DSC的内部通信IP
算法实现： 
全局变量： 
参    数： 
返 回 值：  
=============================================================================*/
void CCfgAgent::SetDscInnerIp(u32 dwIp)
{
    m_tMcuDscInfo.SetDscInnerIp( dwIp );
	
    s8   achIpAddr[32] = {0};
    GetIpFromU32(achIpAddr, m_tMcuDscInfo.GetDscInnerIp() );
    WriteStringToFile(SECTION_DscModule, KEY_DscInnerIp, achIpAddr); 
	
    return;
}

/*=============================================================================
函 数 名： GetDSCInfo
功    能： 获取mcu8000B特殊配置信息
算法实现： 
全局变量： 
参    数： TDSCInfo * ptMcuInfo
返 回 值： u16 
=============================================================================*/
u16 CCfgAgent::GetDSCInfo( TDSCModuleInfo * ptMcuInfo )
{
    memcpy( ptMcuInfo, &m_tMcuDscInfo, sizeof(TDSCModuleInfo) );
    return SUCCESS_AGENT;
}

/*=============================================================================
函 数 名： GetConfigedDscType
功    能： 
算法实现： 
全局变量： 
参    数： void
返 回 值： u8(如果配置过DSC，则返回DSC的类型，否则返回0) 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/2/28  4.0			周广程                  创建
2007/3/05  4.0			周广程					修改
=============================================================================*/
u8 CCfgAgent::GetConfigedDscType( void )
{
	u32 dwBrdNum = GetBoardNum();
	u32 dwLoop = 0;
	u8 byBrdType = 0;
	for( dwLoop = 0; dwLoop < dwBrdNum; dwLoop++ )
	{
		byBrdType = m_atBrdCfgTable[dwLoop].GetType();
		if( byBrdType == BRD_TYPE_DSC/*KDV8000B_MODULE*/ || 
			byBrdType == BRD_TYPE_MDSC/*DSL8000_BRD_MDSC*/ || 
			byBrdType == BRD_TYPE_HDSC/*DSL8000_BRD_HDSC*/ )
		{
			return byBrdType;
		}
	}
	return 0;
}

/*=============================================================================
函 数 名： IsDscReged
功    能： 
算法实现： 
全局变量： 
参    数： 无
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/7/3	4.0			周广程                  创建
2008/6/21	4.5			张宝卿					返回具体的DSC类型
=============================================================================*/
BOOL32 CCfgAgent::IsDscReged( u8 &byDscType )
{
	u8 byBrdType = GetConfigedDscType();
	byDscType = byBrdType;
	if ( 0 != byBrdType )
	{
		TBrdPosition tBrdPos;
		tBrdPos.byBrdID = byBrdType ;
		tBrdPos.byBrdLayer = 0 ;
		tBrdPos.byBrdSlot = 0 ;
		return g_cBrdManagerApp.IsRegedBoard( tBrdPos );
	}
	else
	{
		return FALSE;
	}
}
#endif

/*=============================================================================
函 数 名： GetQosStrFromNum
功    能： 
算法实现： 
全局变量： 
参    数： u8 byType
u8* pchQosIpType
返 回 值： void 
=============================================================================*/
BOOL32 CCfgAgent::GetQosStrFromNum(u8 byType, u8* pchQosIpType, u8 byLen)
{
    if(NULL == pchQosIpType || byLen < MIN_IPSERVICE_STR_LEN)
    {
        return FALSE;
    }
	
    u8 byCount = 0;
    if(0 != (byType & IPSERVICETYPE_LOW_DELAY))
    {
        pchQosIpType[byCount] = '1';
        byCount++;
        pchQosIpType[byCount] = ',';
        byCount++;
    }
	
    if(0 != (byType & IPSERVICETYPE_HIGH_THROUGHPUT))
    {
        pchQosIpType[byCount] = '2';
        byCount++;
        pchQosIpType[byCount] = ',';
        byCount++;
    }
	
    if(0 != (byType & IPSERVICETYPE_HIGH_RELIABILITY))
    {
        pchQosIpType[byCount] = '3';
        byCount++;
        pchQosIpType[byCount] = ',';
        byCount++;
    }
	
    if(0 != (byType & IPSERVICETYPE_LOW_EXPENSE))
    {
        pchQosIpType[byCount] = '4';
        byCount++;
        pchQosIpType[byCount] = ',';
        byCount++;
    }
    return TRUE;
}

/*=============================================================================
函 数 名： PostMsgToBrdMgr
功    能： 发消息给单板管理
算法实现： 
全局变量： 
参    数： const char* pchDstAlias
u8 byAliasLen
u16 wEvent
const void* pvContent
u16 wLen
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
06/09/15    4.0         顾振华        修改
=============================================================================*/
BOOL32 CCfgAgent::PostMsgToBrdMgr(const TBrdPosition &tBrdPosition, u16 wEvent, const void* pvContent, u16 wLen)
{
    u8  byInstId = GetBrdInstIdByPos(tBrdPosition);
    s32 nRet = OspPost( MAKEIID(AID_MCU_BRDMGR, byInstId), wEvent, pvContent, wLen );
	if (OSP_OK != nRet)
    {
        Agtlog( LOG_ERROR, "[PostMsgToBrdMgr] msg <%d><%s> Post FAILED to Inst.%d !\n", 
			wEvent, OspEventDesc(wEvent), byInstId );
        return FALSE;
    }
    return TRUE;
}

/*=============================================================================
函 数 名： SetPowerOffMsgDst
功    能： 设置待机消息号
算法实现： 
全局变量： 
参    数： u16 wEvent
u32 dwDstId
返 回 值： void 
=============================================================================*/
void CCfgAgent::SetPowerOffMsgDst(u16 wEvent, u32 dwDstId)
{
    m_dwPowerOffDstId = dwDstId;
    m_wPowerOffEvent  = wEvent;
    return;
}

/*=============================================================================
函 数 名： SetRebootMsgDst
功    能： 设置重启消息号
算法实现： 
全局变量： 
参    数：  u16 wEvent
u32 dwDstId
返 回 值： void 
=============================================================================*/
void CCfgAgent::SetRebootMsgDst( u16 wEvent, u32 dwDstId)
{
    m_dwRebootDstId = dwDstId;
    m_wRebootEvent  = wEvent;
    return;
}

/*=============================================================================
函 数 名： SetRunningMsgDst
功    能： 设置运行消息号
算法实现： 
全局变量： 
参    数：  u16 wEvent
u32 dwDstId
返 回 值： void 
=============================================================================*/
void CCfgAgent::SetRunningMsgDst( u16 wEvent, u32 dwDstId)
{
    m_dwRunningDstId = dwDstId;
    m_wRunningEvent  = wEvent;
    return;
}

/*=============================================================================
函 数 名： SnmpRebootBoard
功    能： 响应网管系统管理命令
算法实现： 
全局变量： 
参    数： TBrdPosition tBrdPostion
返 回 值： void 
=============================================================================*/
void CCfgAgent::SnmpRebootBoard(TBrdPosition tPos)
{
#ifndef WIN32
	//  [1/21/2011 chendaiwei]支持MPC2
    if( BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ == tPos.byBrdID || BRD_TYPE_MPC2 == tPos.byBrdID )
	{
		Agtlog(LOG_INFORM, "[SystemManager] dst board:<%d,%d:%s>.\n",
			tPos.byBrdLayer, tPos.byBrdSlot, GetBrdTypeStr(tPos.byBrdID) );
		
		Agtlog(LOG_INFORM, "[SystemManager] Local MPC<%d, %d>, Other MPC<%d, %d>.\n",
			m_tMPCInfo.GetLocalLayer(), m_tMPCInfo.GetLocalSlot(),
			m_tMPCInfo.GetOtherMpcLayer(), m_tMPCInfo.GetOtherMpcSlot() );
		
		
		if( m_tMPCInfo.GetLocalLayer() == tPos.byBrdLayer &&
			m_tMPCInfo.GetLocalSlot()  == tPos.byBrdSlot )
		{
			OspPost( MAKEIID(AID_MCU_BRDGUARD, 1), m_wRebootEvent );
			printf("[SnmpRebootBoard]snmp req reboot\n");
			return;
		}
		if( m_tMPCInfo.GetOtherMpcLayer() == tPos.byBrdLayer &&
			m_tMPCInfo.GetOtherMpcSlot()  == tPos.byBrdSlot )
		{
            OspPost( MAKEIID(AID_MCU_MSMANAGERSSN, 1), EV_AGENT_MCU_MCU_RESTART );
			return;
		}
	}
#endif
    // 重启远端板
    RebootBoard( tPos.byBrdLayer, tPos.byBrdSlot, tPos.byBrdID ); 
    return;
}

/*=============================================================================
函 数 名： SetSystemTime
功    能： 设置系统时间
算法实现： 
全局变量： 
参    数： time_t tTime
返 回 值： u8 
=============================================================================*/
u16 CCfgAgent::SetSystemTime(time_t tTime)
{
    u16 wRet = SUCCESS_AGENT;
#ifndef WIN32
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	
	TKdvTime tKdvTime;
	tKdvTime.SetTime( &tTime );
	
    Agtlog( LOG_INFORM,  "[SetSystemTime] time: %d-%d-%d %d:%d:%d", tKdvTime.GetYear(),tKdvTime.GetMonth(),tKdvTime.GetDay(),
		tKdvTime.GetHour(),tKdvTime.GetMinute(),tKdvTime.GetSecond() );
    stime((long*)&tTime); 
	
#else
    u8 byRet = BrdTimeSet(gmtime(&tTime));
    if ( OK !=  byRet )
    {
        wRet = ERR_AGENT_SETNODEVALUE;
        Agtlog(LOG_ERROR, "[SetSystemTime] Set System time failed, return: %d\n", byRet);
    }
	
#endif
#else
	
	
    SYSTEMTIME tSysTime;
	CTime cSysTime(tTime);
	cSysTime.GetAsSystemTime(tSysTime);	
    BOOL bRet = ::SetLocalTime(&tSysTime);
    if( !bRet )
    {
        wRet = ERR_AGENT_SETNODEVALUE;
        Agtlog(LOG_ERROR, "[SetSystemTime] Set System time failed \n");
    }
	
	
#endif
    return wRet;
}

/*=============================================================================
函 数 名： UpdateSoftware
功    能： 更新单板软件
算法实现： 
全局变量： 
参    数： TBrdPosition tBrdPostion
s8* pchFileName
u8 byLen
返 回 值： void 
=============================================================================*/
void CCfgAgent::UpdateBrdSoftware(TBrdPosition tBrdPostion, CServMsg &cServMsg)
{
    BOOL32 bSentOK = FALSE;
    u8 byBrdNum = (u8)m_dwBrdCfgEntryNum; 
	
    // 在会控升级的情况下，升级消息只能由主板发起，避免升级两次
    // 只针对会控是因为会控面对主备环境建立的是双链，而网管面对的则是独立的MCU客户端 2006-09-27
    if ( UPDATE_MCS == cServMsg.GetChnIndex() && !IsMpcActive() )
    {
        Agtlog(LOG_WARN, "[UpdateBrdSoftware] should not be<%d, bMST:%d>, ignore it\n",
			cServMsg.GetChnIndex(), IsMpcActive() );
        return;
    }
	
    for(u8 byRow = 0; byRow < byBrdNum; byRow++ )
    {
        if( m_atBrdCfgTable[byRow].GetLayer() == tBrdPostion.byBrdLayer &&
            m_atBrdCfgTable[byRow].GetSlot() == tBrdPostion.byBrdSlot &&
            m_atBrdCfgTable[byRow].GetType() == tBrdPostion.byBrdID)
        {            
            PostMsgToBrdMgr( tBrdPostion,
				MPC_BOARD_UPDATE_SOFTWARE_CMD, 
				cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
            bSentOK = TRUE;
            break;
        }
    }
    if ( !bSentOK )
    {
        Agtlog(LOG_ERROR, "[UpdateBrdSoftware] brd<%d,%d:%s> unexist, update failed !\n",
			tBrdPostion.byBrdLayer, tBrdPostion.byBrdSlot,
			GetBrdTypeStr(tBrdPostion.byBrdID) );
        return;
    }
	
    TBoardInfo tBrdInfo;
    GetBrdCfgById( GetBrdId(tBrdPostion), &tBrdInfo );
    if ( UPDATE_NMS != cServMsg.GetChnIndex() ||
		OS_TYPE_LINUX != tBrdInfo.GetOSType() )
    {
        return;
    }
    //以下标记单板的状态为正在升级，等待网管查询
    u8 byBrdId = *(u8*)cServMsg.GetMsgBody();
    u8 byNum   = *(u8*)(cServMsg.GetMsgBody()+sizeof(u8));
	
    s8 achFileName[256];
    s8 *lpMsgBody = (s8*)(cServMsg.GetMsgBody()+sizeof(u8)*2);
    u16 wMsgLen = cServMsg.GetMsgBodyLen() - 2;
    u8  byLen = 0;
    u16 wOffSet = 0;  
	
    while( byNum-- > 0 )
    {
        memset( achFileName, 0, sizeof(achFileName) );
        byLen = *lpMsgBody;
		
        if (byLen + wOffSet > wMsgLen)
        {
            Agtlog(LOG_ERROR, "[UpdateBrdSoftware] invalid msgbody,len:%d,wOffSet:%d,wMsgLen:%d \n",
				byLen, wOffSet, wMsgLen);
            break;
        }
        else
        {
            Agtlog(LOG_INFORM, "[UpdateBrdSoftware] valid msgbody,len:%d,wOffSet:%d,wMsgLen:%d \n",
				byLen, wOffSet, wMsgLen);
        }
        
        memcpy(achFileName, lpMsgBody+sizeof(u8), byLen);
        wOffSet = sizeof(byLen) + byLen;
        lpMsgBody += wOffSet;
        Agtlog(LOG_ERROR, "[UpdateBrdSoftware] filename: %s, wOffSet: %d\n", achFileName, wOffSet);
		
        //判断是否image文件
        s32 nNameOffBin = strlen(achFileName) - strlen(LINUXAPP_POSTFIX_BIN);
        s32 nNameOffImage = strlen(achFileName) - strlen(LINUXAPP_POSTFIX);
        s32 nNameOffLinux = 0;
		
		if ( nNameOffBin > 0 && 0 == strcmp(achFileName+nNameOffBin, LINUXAPP_POSTFIX_BIN) )
		{
            g_cBrdManagerApp.SetBinFileStatus(tBrdPostion, SNMP_FILE_UPDATE_INPROCESS);
		}
        else if (nNameOffImage > 0 && 0 == strcmp(achFileName+nNameOffImage, LINUXAPP_POSTFIX) )
        {
            g_cBrdManagerApp.SetImageFileStatus(tBrdPostion, SNMP_FILE_UPDATE_INPROCESS);
        }
        else
        {
            //判断是否os
            nNameOffLinux = strlen(achFileName) - strlen(LINUXOS_POSTFIX);
            if (nNameOffLinux > 0 && 0 == strcmp(achFileName+nNameOffLinux, LINUXOS_POSTFIX) )
            {
                g_cBrdManagerApp.SetOSFileStatus(tBrdPostion, SNMP_FILE_UPDATE_INPROCESS);
            }   
            else
            {
                Agtlog(LOG_ERROR, "[UpdateBrdSoftware] not supported file type.\n");
            }
        }
    }
    return;
}


/*=============================================================================
函 数 名： Update8KXMpcSoftware
功    能： 8K,X86系列mcu更新Mpc板软件
算法实现： 
全局变量： 
参    数： s8* lpszMsg:  要更新的文件序列 u8(文件一长度) + s8[](文件一名) + u8(文件二长度) + s8[](文件二名) ... ...
u16 wMsgLen:  总的文件序列长度
u8  byFileNum: 要更新的文件总个数
s8* pRet: 各个文件更新的结果
返 回 值： u16  
=============================================================================*/
BOOL32 CCfgAgent::Update8KXMpcSoftware( const s8* szFileName,TBrdPosition tMpcPosition,BOOL32 &bIsCheckFileError  )
{
	s8   chCmdline[256] =  {0};  
	s8  achFileName[256];
	memcpy( achFileName,szFileName,sizeof(achFileName) );
	achFileName[sizeof(achFileName)-1] = '\0';
#ifdef _LINUX_

#ifdef _8KI_
	sprintf(chCmdline,"/usr/mcu/%s",achFileName );
	strcpy( achFileName,chCmdline );
#endif

	s32 nNameOff = strlen(achFileName) - strlen(LINUXAPP_POSTFIX_BIN);
		
	if (nNameOff > 0 && 0 == strcmp(achFileName+nNameOff, LINUXAPP_POSTFIX_BIN) )
	{
#ifdef _8KI_
		FILE *pFile = fopen( achFileName,"r" );
		if( pFile == NULL )
		{
			Agtlog(LOG_ERROR, "[Update8KXMpcSoftware] can't openfile.%s,fail to update\n", achFileName);
			return FALSE;
		}

		u8 byLoop = 0;
		BOOL32 bIsCanUpdateFile = FALSE;
		while( fgets( chCmdline,sizeof(chCmdline),pFile ) )
		{
			Agtlog(LOG_ERROR, "[Update8KXMpcSoftware] file.%s is not correct update file\n", chCmdline);
			if( byLoop > 5 )
			{
				break;
			}
			if( 0 == memcmp( KDV8KI_UPDATE_MAGICNUMBER,chCmdline,strlen(KDV8KI_UPDATE_MAGICNUMBER) ) )
			{
				bIsCanUpdateFile = TRUE;
				break;
			}
			++byLoop;
		}
		fclose(pFile);


		if( !bIsCanUpdateFile )
		{
			bIsCheckFileError = TRUE;
			Agtlog(LOG_ERROR, "[Update8KXMpcSoftware] file.%s is not correct update file\n", achFileName);
			return FALSE;
		}
#endif

		//可以开始升级
		// ------current dir: opt/mcu/    // [3/8/2010 xliang] need to confirm
		//1, 将原有业务程序(包括库)统统打包并压缩(包中不含文件夹), 生成有行权限的mcuorg.tar.gz			
		
		s8   szExcludeFile[]=" --exclude conf --exclude data --exclude log --exclude pcap";
		sprintf(chCmdline,  "cd /opt/mcu; rm %s -f; tar czvf %s ./* --exclude ftp %s; chmod a+x %s", 
			"mcuorg.tar.gz", "mcuorg.tar.gz",szExcludeFile, "mcuorg.tar.gz");   //在opt/mcu下生成mcuorg.tar.gz
		system(chCmdline);	

		//2, 升级
		chdir((const char*)DIR_FTP);

#ifdef _8KI_			
		sprintf(chCmdline,"cp %s %s/%s",achFileName,DIR_FTP,KDV8KI_UPDATE_FILENAME );	
		system( chCmdline );			

		Agtlog(LOG_INFORM, "[Update8KXMpcSoftware] updating file cmd :%s\n", chCmdline);

		sprintf( achFileName,"%s/%s",DIR_FTP,KDV8KI_UPDATE_FILENAME );
#endif

		if (0 == access (achFileName, F_OK))
		{
			//s8    achFile[MAXLEN_MCU_FILEPATH] = {0};
			//sprintf(achFile, "./%s", achFileName);
			chmod(achFileName, S_IREAD|S_IWRITE|S_IEXEC);
			g_cBrdManagerApp.SetImageFileStatus(tMpcPosition, 
				SNMP_FILE_UPDATE_INPROCESS);
			system(achFileName);
			g_cBrdManagerApp.SetOSFileStatus(tMpcPosition, 
				SNMP_FILE_UPDATE_SUCCEED);
			//zhouyiliang 20120208 写mpc升级的标志位,0表示升级，1表示正常启动（与老版本保持一致，规避升级为老版本问题）
			SetMpcUpateStatus(0);

			return TRUE;
		}			
	}
#ifdef _8KI_
	nNameOff = strlen(achFileName) - strlen(KEY_FILENAME);		 

	if(  nNameOff > 0 && 0 == strcmp(achFileName+nNameOff, KEY_FILENAME) )
	{
		if (0 == access (achFileName, F_OK))
		{				
			chmod(achFileName, S_IREAD|S_IWRITE|S_IEXEC);
		}
		sprintf( chCmdline,"cp %s %s",achFileName,DIR_DATA );		
		system( chCmdline );

		Agtlog(LOG_INFORM, "[Update8KXMpcSoftware] updating file cmd :%s\n", chCmdline);
		return TRUE;
	}		

	nNameOff = strlen(achFileName) - strlen("index.htm");
	if( nNameOff > 0 && 0 == strcmp(achFileName+nNameOff, "index.htm") )
	{
		if (0 == access (achFileName, F_OK))
		{				
			chmod(achFileName, S_IREAD|S_IWRITE|S_IEXEC);
		}
		sprintf( chCmdline,"cp %s %s",achFileName,DIR_WEB );		
		system( chCmdline );
		Agtlog(LOG_INFORM, "[Update8KXMpcSoftware] updating file cmd :%s\n", chCmdline);
		return TRUE;
	}

	nNameOff = strlen(achFileName) - strlen("mcs.cab");
	if( nNameOff > 0 && 0 == strcmp(achFileName+nNameOff, "mcs.cab") )
	{
		if (0 == access (achFileName, F_OK))
		{				
			chmod(achFileName, S_IREAD|S_IWRITE|S_IEXEC);
		}
		sprintf( chCmdline,"cp %s %s",achFileName,DIR_WEB );		
		system( chCmdline );
		Agtlog(LOG_INFORM, "[Update8KXMpcSoftware] updating file cmd :%s\n", chCmdline);
		return TRUE;
	}

	nNameOff = strlen(achFileName) - strlen("player.htm");
	if( nNameOff > 0 && 0 == strcmp(achFileName+nNameOff, "player.htm") )
	{
		if (0 == access (achFileName, F_OK))
		{				
			chmod(achFileName, S_IREAD|S_IWRITE|S_IEXEC);
		}
		sprintf( chCmdline,"cp %s %s",achFileName,DIR_WEB );		
		system( chCmdline );
		Agtlog(LOG_INFORM, "[Update8KXMpcSoftware] updating file cmd :%s\n", chCmdline);
		return TRUE;
	}

	nNameOff = strlen(achFileName) - strlen("stream.cab");
	if( nNameOff > 0 && 0 == strcmp(achFileName+nNameOff, "stream.cab") )
	{
		if (0 == access (achFileName, F_OK))
		{				
			chmod(achFileName, S_IREAD|S_IWRITE|S_IEXEC);
		}
		sprintf( chCmdline,"cp %s %s",achFileName,DIR_WEB );		
		system( chCmdline );
		Agtlog(LOG_INFORM, "[Update8KXMpcSoftware] updating file cmd :%s\n", chCmdline);
		return TRUE;
	}

	nNameOff = strlen(achFileName) - strlen(MCUCFGFILENAME);
	if( nNameOff > 0 && 0 == strcmp(achFileName+nNameOff, MCUCFGFILENAME) )
	{
		if (0 == access (achFileName, F_OK))
		{				
			chmod(achFileName, S_IREAD|S_IWRITE);
		}
		sprintf( chCmdline,"cp %s %s",achFileName,DIR_CONFIG );		
		system( chCmdline );
		Agtlog(LOG_INFORM, "[Update8KXMpcSoftware] updating file cmd :%s\n", chCmdline);
		return TRUE;
	}
	 

	nNameOff = strlen(achFileName) - strlen("FILE_USR_PATH_BMP_4v3.bmp");
	if( nNameOff > 0 && 0 == strcmp(achFileName+nNameOff, "FILE_USR_PATH_BMP_4v3.bmp") )
	{
		if (0 == access (achFileName, F_OK))
		{				
			chmod(achFileName, S_IREAD|S_IWRITE);
		}
		sprintf( chCmdline,"cp %s %s",achFileName,DIR_FTP );		
		system( chCmdline );
		Agtlog(LOG_INFORM, "[Update8KXMpcSoftware] updating file cmd :%s\n", chCmdline);
		return TRUE;
	}

	nNameOff = strlen(achFileName) - strlen("FILE_USR_PATH_BMP_16v9.bmp");
	if( nNameOff > 0 && 0 == strcmp(achFileName+nNameOff, "FILE_USR_PATH_BMP_16v9.bmp") )
	{
		if (0 == access (achFileName, F_OK))
		{				
			chmod(achFileName, S_IREAD|S_IWRITE);
		}
		sprintf( chCmdline,"cp %s %s",achFileName,DIR_FTP );		
		system( chCmdline );
		Agtlog(LOG_INFORM, "[Update8KXMpcSoftware] updating file cmd :%s\n", chCmdline);
		return TRUE;
	}


		 
#endif
	return FALSE;


#endif//#ifdef _LINUX_

	return TRUE;
	
}

	


/*=============================================================================
函 数 名： UpdateMpcSoftware
功    能： 更新Mpc板软件
算法实现： 
全局变量： 
参    数： s8* lpszMsg:  要更新的文件序列 u8(文件一长度) + s8[](文件一名) + u8(文件二长度) + s8[](文件二名) ... ...
u16 wMsgLen:  总的文件序列长度
u8  byFileNum: 要更新的文件总个数
s8* pRet: 各个文件更新的结果
返 回 值： u16  
=============================================================================*/
BOOL32 CCfgAgent::UpdateMpcSoftware( s8* lpszMsg, u16 wMsgLen, u8 byFileNum, u8 *pbyRet, BOOL32 bNMS )
{
    if ( lpszMsg == NULL )
    {
        Agtlog(LOG_ERROR, "[UpdateMpcSoftware] input lpszMsg should not be NULL!\n");
        return FALSE;
    }
    if ( pbyRet == NULL )
    {
        Agtlog(LOG_ERROR, "[UpdateMpcSoftware] result pointer should not be NULL!\n");
        return FALSE;
    }
	
    u8  byLen = 0;
    u16 wOffSet = 0;
    s8  achFileName[256];
    u8  byUpdateFile = 0;
    u8  byTmpNum = byFileNum;
	
#ifdef _LINUX_
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)

	
	
	

		


	
	
#endif
#endif
    
    while ( byFileNum -- > 0 )
    {
        memset( achFileName, 0, sizeof(achFileName) );
        byLen = *lpszMsg;
        if (byLen + wOffSet > wMsgLen)
        {
            Agtlog(LOG_ERROR, "[UpdateMpcSoftware] invalid msgbody, byLen: %d, wOffSet: %d, wMsgLen: %d!\n",
				byLen, wOffSet, wMsgLen );
            pbyRet[byUpdateFile] = 0;
            break;
        }
        else
        {
            Agtlog(LOG_INFORM, "[UpdateMpcSoftware] msgbody valid, byLen: %d, wOffSet: %d, wMsgLen: %d!\n",
				byLen, wOffSet, wMsgLen );
        }
		
        s8 *pszFileName = lpszMsg+sizeof(u8);
        memcpy(achFileName, pszFileName, byLen+1);
        achFileName[byLen] = '\0';
        wOffSet = sizeof(byLen) + byLen;
        lpszMsg += wOffSet;
        
        Agtlog(LOG_INFORM, "[UpdateMpcSoftware] filename: %s, wOffSet: %d\n", 
			achFileName, wOffSet);
#ifdef _VXWORKS_   
		
        s8  achTmpName[256];
        
        // 保存上传文件名
        memcpy(achTmpName, achFileName, sizeof(achFileName));
        
        // 判断是否有此文件名
        FILE *hR = fopen( achTmpName, "r" );
        if( NULL == hR )
        {
            pbyRet[byUpdateFile] = 0;
            continue;
        }
        else
        {
            fclose( hR );
        }
		
        //去掉后缀
        achFileName[strlen(achFileName) - strlen("upd")] = 0;
        Agtlog(LOG_INFORM, "[UpdateMpcSoftware] updating file name is :%s\n", achFileName);
		
        //将原来的文件改名
        sprintf(achTmpName, "%sold", achFileName);
        rename(achFileName, achTmpName);
		
        //新文件改名
        rename(pszFileName, achFileName);
		
        //删除原来的文件
        remove(achTmpName);
		
#elif defined _LINUX_
		
		TBrdPosition tMpcPosition;
		tMpcPosition.byBrdID = BRD_TYPE_MPC/*DSL8000_BRD_MPC*/;
		tMpcPosition.byBrdLayer = m_tMPCInfo.GetLocalLayer();
		tMpcPosition.byBrdSlot  = m_tMPCInfo.GetLocalSlot();
		
		Agtlog(LOG_INFORM, "[UpdateMpcSoftware] updating file name is :%s\n", achFileName);
		
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
		

		
		pbyRet[byUpdateFile] = 0;
		BOOL32 bIsCheckFileError = FALSE;
		if( Update8KXMpcSoftware(achFileName,tMpcPosition,bIsCheckFileError) )
		{
			pbyRet[byUpdateFile] = 1;
		}
		else
		{
			if( bIsCheckFileError )
			{
				pbyRet[byUpdateFile] = 3;
			}
		}
		
		

#elif defined _LINUX12_ 
		//[chendaiwei 20101116]新的MPC2板会通过bin包的形式升级,相应升级接口BrdSysUpdate只可在Linux 1.2下调用
		//判断是否bin文件
		//  [1/21/2011 chendaiwei]支持MPC2
		tMpcPosition.byBrdID = BRD_TYPE_MPC2/*DSL8000_BRD_MPC2*/;
		
		s32 nNameOff = strlen(achFileName) - strlen(LINUXAPP_POSTFIX_BIN);
		if ( nNameOff > 0 && 0 == strcmp(achFileName+nNameOff, LINUXAPP_POSTFIX_BIN) )
		{
			printf("[UpdateMpcSoftware] update ram file.%s as bin\n", achFileName);
			
			STATUS nRetCode = BrdCheckUpdatePackage(achFileName);
			if( nRetCode != OK )
			{
				Agtlog(LOG_ERROR, "[UpdateMpcSoftware] check package[%s] failed,error.%d!\n",achFileName,nRetCode );
				pbyRet[byUpdateFile] = 0;
				if( CHK_PID_FAILURE == nRetCode )
				{
					pbyRet[byUpdateFile] = 2;
				}
				return FALSE;
			}

			if ( SYSTEM_UPDATE_SUCCESS != BrdSysUpdate(achFileName) )
			{
				pbyRet[byUpdateFile] = 0;
				g_cBrdManagerApp.SetBinFileStatus(tMpcPosition, SNMP_FILE_UPDATE_FAILED);
				Agtlog(LOG_ERROR, "[UpdateMpcSoftware] BrdUpdateAppBin failed.\n");
			}
			else
			{
				g_cBrdManagerApp.SetBinFileStatus(tMpcPosition, SNMP_FILE_UPDATE_SUCCEED);
				Agtlog(LOG_INFORM, "[UpdateMpcSoftware] BrdUpdateAppBin success.\n");
			}
		}
#else
		
		//判断是否image文件
		s32 nNameOff = strlen(achFileName) - strlen(LINUXAPP_POSTFIX);
		
		if (nNameOff > 0 && 0 == strcmp(achFileName+nNameOff, LINUXAPP_POSTFIX) )
		{
			g_cBrdManagerApp.SetImageFileStatus(tMpcPosition, 
				SNMP_FILE_UPDATE_INPROCESS);
			if ( OK != BrdUpdateAppImage(achFileName) )
			{
				pbyRet[byUpdateFile] = 0;
				g_cBrdManagerApp.SetImageFileStatus(tMpcPosition, 
					SNMP_FILE_UPDATE_FAILED);
				Agtlog(LOG_ERROR, "[UpdateMpcSoftware] BrdUpdateAppImage failed.\n");
			}
			else
			{
				g_cBrdManagerApp.SetImageFileStatus(tMpcPosition, 
					SNMP_FILE_UPDATE_SUCCEED);
				Agtlog(LOG_ERROR, "[UpdateMpcSoftware] BrdUpdateAppImage success.\n");
			}
		}
		else
		{
			//判断是否os
			nNameOff = strlen(achFileName) - strlen(LINUXOS_POSTFIX);
			if (nNameOff > 0 && 0 == strcmp(achFileName+nNameOff, LINUXOS_POSTFIX) )
			{
				g_cBrdManagerApp.SetOSFileStatus(tMpcPosition, 
					SNMP_FILE_UPDATE_INPROCESS);
				
				if ( OK != BrdFpUpdateAuxData(achFileName) )
				{
					pbyRet[byUpdateFile] = 0;
					g_cBrdManagerApp.SetOSFileStatus(tMpcPosition, 
						SNMP_FILE_UPDATE_FAILED);
					Agtlog(LOG_ERROR, "[UpdateMpcSoftware] BrdFpUpdateAuxData failed.\n");
				}
				else
				{
					g_cBrdManagerApp.SetOSFileStatus(tMpcPosition, 
						SNMP_FILE_UPDATE_SUCCEED);
					Agtlog(LOG_ERROR, "[UpdateMpcSoftware] BrdFpUpdateAuxData success.\n");
				}
			}   
			else
			{
				Agtlog(LOG_ERROR, "[UpdateMpcSoftware] not supported file type.\n");
			}
		}
#endif
		
#endif
        byUpdateFile ++;
    }
	
    s32 nIndex = 0;
    for( ; nIndex < byTmpNum; nIndex ++ )
    {
        if ( pbyRet[nIndex] == 0 )
        {
            Agtlog(LOG_ERROR, "[UpdateMpcSoftware] file.%d update failed !\n", nIndex);
            return FALSE;
        }
    }
	
    if ( TRUE == bNMS )
    {
        // [zw] [09/10/2008] 只有升级成功才会重启。 
        OspPost( MAKEIID(AID_MCU_BRDGUARD, 1 ), m_wRebootEvent );
		printf("[UpdateMpcSoftware]update suc req reboot\n");
    }
	
    return TRUE;
}


/*=============================================================================
函 数 名： SmUpdateMpcSoftware
功    能： 更新Mpc板软件
算法实现： 
全局变量： 
参    数： 

  返 回 值： u16  
  =============================================================================*/
BOOL32 CCfgAgent::SmUpdateMpcSoftware()
{
	s8  achFileName[KDV_MAX_PATH];
	u8  byUpdateFile = 0;
	
	TMcuUpdateInfo* ptUpdateInfo = g_cCfgParse.GetMcuUpdateInfo();
	
	if (ptUpdateInfo == NULL)
	{
		return FALSE;
	}
	
	u8 byFileNum = ptUpdateInfo->GetFileNum();
	
	TUpdateFileInfo* ptUpdateFile = NULL;
	
#ifdef _LINUX_
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	
#endif
#endif
	
	while ( byFileNum > 0 )
	{
		byFileNum--;
		memset( achFileName, 0, sizeof(achFileName) );
		
		memcpy(achFileName, ptUpdateInfo->GetLocalUpdateFileName(byFileNum), KDV_MAX_PATH);
		Agtlog(LOG_INFORM, "[SmUpdateMpcSoftware] filename: %s\n", achFileName);
		
#ifdef _VXWORKS_   
		
		s8  achTmpName[256];
		
		// 保存上传文件名
		memcpy(achTmpName, achFileName, sizeof(achFileName));
		
		// 判断是否有此文件名
		FILE *hR = fopen( achTmpName, "r" );
		if( NULL == hR )
		{
			continue;
		}
		else
		{
			fclose( hR );
		}
		
		//去掉后缀
		achFileName[strlen(achFileName) - strlen("upd")] = 0;
		Agtlog(LOG_INFORM, "[SmUpdateMpcSoftware] updating file name is :%s\n", achFileName);
		
		//将原来的文件改名
		sprintf(achTmpName, "%sold", achFileName);
		rename(achFileName, achTmpName);
		
		//新文件改名
		rename(ptUpdateInfo->GetLocalUpdateFileName(byFileNum), achFileName);
		
		//删除原来的文件
		remove(achTmpName);
		
#elif defined _LINUX_
		
		TBrdPosition tMpcPosition;
		tMpcPosition.byBrdID = BRD_TYPE_MPC/*DSL8000_BRD_MPC*/;
		tMpcPosition.byBrdLayer = m_tMPCInfo.GetLocalLayer();
		tMpcPosition.byBrdSlot  = m_tMPCInfo.GetLocalSlot();
		
		Agtlog(LOG_INFORM, "[UpdateMpcSoftware] updating file name is :%s\n", achFileName);
		
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
		
	
		BOOL32 bIsCheckFileError = FALSE;
		if( !Update8KXMpcSoftware(achFileName,tMpcPosition,bIsCheckFileError) )
		{
			ptUpdateInfo->SetUpdateState(SNMP_FILE_UPDATE_FAILED);
			continue;
		}
		
#else
		
		//判断是否image文件
		s32 nNameOff = strlen(achFileName) - strlen(LINUXAPP_POSTFIX);
		
		if (nNameOff > 0 && 0 == strcmp(achFileName+nNameOff, LINUXAPP_POSTFIX) )
		{
			g_cBrdManagerApp.SetImageFileStatus(tMpcPosition, 
				SNMP_FILE_UPDATE_INPROCESS);
			if ( OK != BrdUpdateAppImage(achFileName) )
			{
				g_cBrdManagerApp.SetImageFileStatus(tMpcPosition, 
					SNMP_FILE_UPDATE_FAILED);
				ptUpdateInfo->SetUpdateState(SNMP_FILE_UPDATE_FAILED);
				Agtlog(LOG_ERROR, "[UpdateMpcSoftware] BrdUpdateAppImage failed.\n");
			}
			else
			{
				g_cBrdManagerApp.SetImageFileStatus(tMpcPosition, 
					SNMP_FILE_UPDATE_SUCCEED);
				Agtlog(LOG_ERROR, "[UpdateMpcSoftware] BrdUpdateAppImage success.\n");
			}
		}
		else
		{
			//判断是否os
			nNameOff = strlen(achFileName) - strlen(LINUXOS_POSTFIX);
			if (nNameOff > 0 && 0 == strcmp(achFileName+nNameOff, LINUXOS_POSTFIX) )
			{
				g_cBrdManagerApp.SetOSFileStatus(tMpcPosition, 
					SNMP_FILE_UPDATE_INPROCESS);
				
				if ( OK != BrdFpUpdateAuxData(achFileName) )
				{
					g_cBrdManagerApp.SetOSFileStatus(tMpcPosition, 
						SNMP_FILE_UPDATE_FAILED);
					ptUpdateInfo->SetUpdateState(SNMP_FILE_UPDATE_FAILED);
					Agtlog(LOG_ERROR, "[UpdateMpcSoftware] BrdFpUpdateAuxData failed.\n");
				}
				else
				{
					g_cBrdManagerApp.SetOSFileStatus(tMpcPosition, 
						SNMP_FILE_UPDATE_SUCCEED);
					Agtlog(LOG_ERROR, "[UpdateMpcSoftware] BrdFpUpdateAuxData success.\n");
				}
			}   
			else
			{
				Agtlog(LOG_ERROR, "[UpdateMpcSoftware] not supported file type.\n");
			}
		}
#endif
#endif
		byUpdateFile ++;
    }
	
	if (ptUpdateInfo->GetUpdateState() != SNMP_FILE_UPDATE_FAILED)
	{
		ptUpdateInfo->SetUpdateState(SNMP_FILE_UPDATE_SUCCEED);
	}
	
    return TRUE;
}


/*=============================================================================
函 数 名： BitErrorTest
功    能： Bit测试
算法实现： 
全局变量： 
参    数： TBrdPosition tBrdPostion
返 回 值： void 
=============================================================================*/
void CCfgAgent::BitErrorTest(TBrdPosition tBrdPostion)
{
    u8 byBrdNum = (u8)m_dwBrdCfgEntryNum;
    for(u8 byRow = 0; byRow < byBrdNum; byRow++ )
    {
        if( m_atBrdCfgTable[byRow].GetLayer() == tBrdPostion.byBrdLayer &&
            m_atBrdCfgTable[byRow].GetSlot() == tBrdPostion.byBrdSlot &&
            m_atBrdCfgTable[byRow].GetType() == tBrdPostion.byBrdID)
        {
            PostMsgToBrdMgr( tBrdPostion, MPC_BOARD_BIT_ERROR_TEST_CMD, NULL, 0);
            break;
        }
    }
    return;
}


/*=============================================================================
函 数 名： SyncSystemTime
功    能： 同步MPC板时间
算法实现： 
全局变量： 
参    数： s8* pchTime
u16* wLength
返 回 值： void 
=============================================================================*/
void CCfgAgent::SyncSystemTime(const s8* pszTime)
{
    if( NULL == pszTime )
    {        
        return;
    }
    s8 achTimeStr[32];
    u8 byLen = ( strlen(pszTime) > sizeof(achTimeStr)) ? sizeof(achTimeStr) : strlen(pszTime);
    
    memset( achTimeStr, '\0', sizeof(achTimeStr) );
    memcpy( achTimeStr, pszTime, byLen );
	
    s8* pszTimeStr = (s8*)achTimeStr;
    s8 achYear[5];
    s8 achMon[3];
    s8 achDay[3];
    s8 achHour[3];
    s8 achMin[3];
    s8 achSec[3];
    
    strncpy( (s8*)achYear, pszTimeStr, 4 );
    achYear[4] = '\0';
    strncpy( (s8*)achMon,  pszTimeStr + 4, 2 );
    achMon[2] = '\0';
    strncpy( (s8*)achDay,  pszTimeStr + 6, 2 );
    achDay[2] = '\0';
    strncpy( (s8*)achHour, pszTimeStr + 8, 2 );
    achHour[2] = '\0';
    strncpy( (s8*)achMin,  pszTimeStr + 10, 2 );
    achMin[2] = '\0';
    strncpy( (s8*)achSec,  pszTimeStr + 12, 2 );
    achSec[2] = '\0';
    
    u16 wYear    = atoi(achYear);
    u8  byMonth  = atoi(achMon);
    u8  byDay    = atoi(achDay);
    u8  byHour   = atoi(achHour);
    u8  byMinute = atoi(achMin);
    u8  bySecond = atoi(achSec);
	
#ifndef WIN32
    // 1、同步MPC时间
    tm tTime;
    tTime.tm_year = wYear - 1900;
    tTime.tm_mon  = byMonth - 1;
    tTime.tm_mday = byDay;
    tTime.tm_hour = byHour;
    tTime.tm_min  = byMinute;
    tTime.tm_sec  = bySecond;
    tTime.tm_wday = 0;
    tTime.tm_yday = 0;
    tTime.tm_isdst = 0;
    
    if (OK == BrdTimeSet(&tTime)) 
    {
        Agtlog(LOG_INFORM, "[SysMpcTime] set time %d%d%d%d%d%d to MPC board succeed!\n",
			wYear, byMonth, byDay, byHour, byMinute, bySecond );
    }
    else 
    {
        Agtlog(LOG_ERROR, "[SysMpcTime] set time %d%d%d%d%d%d to MPC board failed!\n",
			wYear, byMonth, byDay, byHour, byMinute, bySecond );
    }
#endif

    // 2、同步外设单板时间

    // 2.1 取MPC本地时间
    time_t      tCurTime;
    struct tm  *ptTime;
    s8 achBuf[32];
    memset(achBuf, '\0', sizeof(achBuf));   
    tCurTime = time( 0 );
    ptTime = localtime( &tCurTime );
    sprintf( achBuf, "%4.4u%2.2u%2.2u%2.2u%2.2u%2.2u", 
        ptTime->tm_year + 1900, ptTime->tm_mon + 1,
        ptTime->tm_mday, ptTime->tm_hour, ptTime->tm_min, ptTime->tm_sec );

    // 2.2 取所有已配置的单板TBrdPosition，并发送单板时间同步命令
    TBrdPosition tBrdPos;
    for(u16 wLoop = 0; wLoop < m_dwBrdCfgEntryNum; wLoop++)
    {
        tBrdPos.byBrdLayer = m_atBrdCfgTable[wLoop].GetLayer();
        tBrdPos.byBrdSlot = m_atBrdCfgTable[wLoop].GetSlot();
		tBrdPos.byBrdID = m_atBrdCfgTable[wLoop].GetBrdId();
        BOOL32 bRet = PostMsgToBrdMgr( tBrdPos, 
            MPC_BOARD_TIME_SYNC_CMD, (s8*)achBuf, sizeof(achBuf));
        if ( bRet )
        {
            Agtlog(LOG_INFORM, "[SyncBoardTime] send time %s to Board<%d.%d.%d> succeed !\n", 
                achBuf, tBrdPos.byBrdLayer, tBrdPos.byBrdSlot, tBrdPos.byBrdID );
        }     
    }
    return;
}

/*=============================================================================
函 数 名： SyncBoardTime
功    能： 设置某单板系统时间同步
算法实现： NOTE: 主板和备板之间不能作相互时间同步操作
全局变量： 
参    数： u8* pBuf
返 回 值： void 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/11/27  4.0         张宝卿      时间同步策略整改
=============================================================================*/
void CCfgAgent::SyncBoardTime(u8* pbyBuf)
{
    TBrdPosition tBrdPos;
    tBrdPos.byBrdLayer = *(u8*)pbyBuf;
    tBrdPos.byBrdSlot  = *(u8*)(pbyBuf + 1);
	
	// [20110316 miaoqingsong modify] 添加网管侧到MCU侧单板类型转换
	//tBrdPos.byBrdID    = *(u8*)(pbyBuf + 2);
    tBrdPos.byBrdID    = g_cCfgParse.GetMcuBrdTypeFromSnmp(*(u8*)(pbyBuf + 2));
    
    //从网管本地同步MPC板时间
	//  [1/21/2011 chendaiwei]支持MPC2
    if ( (BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ == tBrdPos.byBrdID ||
		BRD_TYPE_MPC2 == tBrdPos.byBrdID ) &&
		tBrdPos.byBrdLayer == m_tMPCInfo.GetLocalLayer() &&
		tBrdPos.byBrdSlot  == m_tMPCInfo.GetLocalSlot() ) 
    {
        SyncSystemTime((s8*)(pbyBuf+3));
    }
    //从MPC同步其他单板的时间
    else if ( g_cBrdManagerApp.IsRegedBoard(tBrdPos) ) 
    {
        //取MPC本地时间
        time_t      tCurTime;
        struct tm  *ptTime;
        s8 achBuf[32];
        memset(achBuf, '\0', sizeof(achBuf));
		
        tCurTime = time( 0 );
        ptTime = localtime( &tCurTime );
        sprintf( achBuf, "%4.4u%2.2u%2.2u%2.2u%2.2u%2.2u", 
			ptTime->tm_year + 1900, ptTime->tm_mon + 1,
			ptTime->tm_mday, ptTime->tm_hour, ptTime->tm_min, ptTime->tm_sec );
		
        BOOL32 bRet = PostMsgToBrdMgr( tBrdPos, 
			MPC_BOARD_TIME_SYNC_CMD, (s8*)achBuf, sizeof(achBuf));
        if ( bRet )
        {
            Agtlog(LOG_INFORM, "[SyncBoardTime] send time %s to Board<%d.%d.%d> succeed !\n", 
				achBuf, tBrdPos.byBrdLayer, tBrdPos.byBrdSlot, tBrdPos.byBrdID );
        } 
		
    }
    else
    {
        Agtlog( LOG_ERROR, "[SyncBoardTime] Board <%d.%d.%d> to be sync UNEXIST !\n",
			tBrdPos.byBrdLayer, tBrdPos.byBrdSlot, tBrdPos.byBrdID);
    }
    return;
}

/*=============================================================================
函 数 名： BoardSelfTest
功    能： 自测
算法实现： 
全局变量： 
参    数： TBrdPosition tBrdPostion
返 回 值： void 
=============================================================================*/
void CCfgAgent::BoardSelfTest(TBrdPosition tBrdPostion)
{
    u8 byBrdNum = (u8)m_dwBrdCfgEntryNum;
    for(u8 byRow = 0; byRow < byBrdNum; byRow++ )
    {
        if( m_atBrdCfgTable[byRow].GetLayer() == tBrdPostion.byBrdLayer &&
            m_atBrdCfgTable[byRow].GetSlot() == tBrdPostion.byBrdSlot &&
            m_atBrdCfgTable[byRow].GetType() == tBrdPostion.byBrdID)
        {
            PostMsgToBrdMgr( tBrdPostion, MPC_BOARD_SELF_TEST_CMD, NULL, 0);
            break;
        }
    }
    return;
}

/*=============================================================================
函 数 名： SetSystemState
功    能： 系统状态设置
算法实现： 
全局变量： 
参    数： u8 byOpt
返 回 值： void 
=============================================================================*/
void CCfgAgent::SetSystemState( u8 byState )
{
    if( MCU_STATE_RUNNING == byState )
    {
        if( m_tMcuSystem.GetState() == MCU_STATE_STANDY)
        {
            OspPost( m_dwRunningDstId, m_wRunningEvent );
            m_tMcuSystem.SetState( MCU_STATE_RUNNING );
        }
    }
    else if( MCU_STATE_REBOOT == byState )
    {
        if( m_dwRebootDstId != 0 ) // 为0， 不告诉业务, 非0 ,通知业务
        {
            Agtlog( LOG_INFORM, "\nm_dwRebootDstId= %d, m_wRebootEvent= %d\n", 
				m_dwRebootDstId, m_wRebootEvent );
            
            OspPost( MAKEIID(AID_MCU_BRDMGR, CInstance::EACH), MPC_BOARD_RESET_CMD);
            OspPost( MAKEIID(AID_MCU_MSMANAGERSSN, 1), EV_AGENT_MCU_MCU_RESTART );
            OspDelay(2000);

			// [02/13/2012_mqs] 网管重启设备直接重启，避免多线程操作引起的时序问题
            //OspPost( MAKEIID(AID_MCU_BRDGUARD, 1 ), m_wRebootEvent );
#if ( defined(_8KE_) || defined(_8KH_) ) && defined(_LINUX_)			
			McuAgentQuit(FALSE);
			//OspQuit();
			OspDelay(100); 
			
			s8   chCmdline[256] =  {0};   
			sprintf(chCmdline,  "reboot");
			system(chCmdline);			
#elif defined(_LINUX_)
			printf("[SetSystemState] Reboot the mcu!\n");
			
			OspQuit();
			OspDelay( 1000 );	
			BrdHwReset(); // restart	
#else	
			OspPrintf(TRUE, FALSE, "[SetSystemState] Recv reboot message!\n");	
#endif
			printf("[SetSystemState]now system state req reboot\n");
        }
    }
    else if( MCU_STATE_STANDY == byState )
    {
        if( m_tMcuSystem.GetState() == MCU_STATE_RUNNING)
        {
            OspPost( m_dwPowerOffDstId, m_wPowerOffEvent );
            m_tMcuSystem.SetState( MCU_STATE_STANDY );
        }
    }
    
    return;
}

/*====================================================================
函数名      : SetIsMcuConfiged
功能        ：设置Mcu配置标识
算法实现    ：
引用全局变量：
输入参数说明：BOOL32 bIsConfiged
返回值说明  ：u16
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
06/12/25    4.0         周广程         创建
====================================================================*/
u16 CCfgAgent::SetIsMcuConfiged( BOOL32 bIsConfiged)
{
	m_tMcuSystem.SetIsMcuConfiged( bIsConfiged );
	
	s32 nValue = bIsConfiged ? 1 : 0 ;
    u16 wRet = WriteIntToFile( SECTION_mcuSystem, KEY_mcuIsConfiged, nValue );
	
    if( SUCCESS_AGENT != wRet )
	{
		Agtlog(LOG_ERROR, "[SetIsMcuConfiged] Wrong profile while reading %s!\n", KEY_mcuIsConfiged );
		return ERR_AGENT_WRITECFGFILE;
	}
    return SUCCESS_AGENT;
}

/*====================================================================
函数名      : IsMcuConfiged
功能        ：判断Mcu配置表示
算法实现    ：
引用全局变量：
输入参数说明：无
返回值说明  ：BOOL32
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
06/12/25    4.0         周广程          创建
====================================================================*/
BOOL32 CCfgAgent::IsMcuConfiged()
{
	return m_tMcuSystem.IsMcuConfiged();
}

/*=============================================================================
函 数 名： SearchPortChoice
功    能： 
算法实现： 
全局变量： 
参    数： u32 dwIpAddr = 0 : 主机序
返 回 值： u8 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/5/14   4.0			周广程                  创建
=============================================================================*/
u8 CCfgAgent::SearchPortChoice( u32 dwIpAddr )
{
	u8 byPortChoice = 0xff;
	
	//支持8000E
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	// [3/4/2011 xliang] don't use Search8KEPortChoice() which is not correct all the circumstances
	//byPortChoice = Search8KEPortChoice();
	byPortChoice = m_tMPCInfo.GetLocalPortChoice();
	LogPrint(LOG_LVL_ERROR,  MID_PUB_ALWAYS, "[SearchPortChoice] The board ip is (0x%x)\n", dwIpAddr );
#else
	
	if ( dwIpAddr != 0 && !OspIsLocalHost( htonl(dwIpAddr) ) )
	{
		printf( "[SearchMpcPortChoiceByCfg] The board ip(0x%x) is not local host\n", dwIpAddr );

		LogPrint(LOG_LVL_ERROR,  MID_PUB_ALWAYS, "[SearchMpcPortChoiceByCfg] The board ip(0x%x) is not local host\n", dwIpAddr );
	
		return byPortChoice;
	}
	
#ifdef _MINIMCU_ 
	byPortChoice = 0;
#else
#ifndef _VXWORKS_
	// 先查eth1
	STATUS nRet = 0;
	TBrdEthParamAll tBrdEthParamAll;
	u8 byLop = 0;
	nRet = AgtGetBrdEthParamAll( 1, &tBrdEthParamAll ); 

	LogPrint(LOG_LVL_KEYSTATUS,  MID_PUB_ALWAYS,"[SearchMpcPortChoiceByCfg] PortChoice.1->IpNum.%d\n", tBrdEthParamAll.dwIpNum);

	if (tBrdEthParamAll.dwIpNum >0)
	{
		for (u8 byIdx = 0; byIdx < tBrdEthParamAll.dwIpNum; byIdx++)
		{
			LogPrint(LOG_LVL_KEYSTATUS,  MID_PUB_ALWAYS, "[SearchMpcPortChoiceByCfg] PortChoice.1-><IpIndex.%d, ip.0x%x>!\n",
										 byIdx, tBrdEthParamAll.atBrdEthParam[byIdx].dwIpAdrs);
		}
	}
	if ( nRet == ERROR )
	{
		printf( "[SearchMpcPortChoiceByCfg] Get eth1 params failed, search eth0!\n" );
		LogPrint(LOG_LVL_ERROR,  MID_PUB_ALWAYS, "[SearchMpcPortChoiceByCfg] Get eth1 params failed, search eth0!\n" );
	}
	else if( tBrdEthParamAll.dwIpNum > 0 && tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs != 0 )
	{
		if ( dwIpAddr == 0 )
		{
			byPortChoice = 1;
		}
		else
		{
			for ( byLop = 0; byLop < tBrdEthParamAll.dwIpNum; byLop++ )
			{
				if ( htonl(dwIpAddr) == tBrdEthParamAll.atBrdEthParam[byLop].dwIpAdrs )
				{
					byPortChoice = 1;
					break;
				}
			}
		}
	}
	
	//再查eth0
	if ( nRet == ERROR 
		|| tBrdEthParamAll.dwIpNum == 0 
		|| (tBrdEthParamAll.dwIpNum > 0 && tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs == 0)
		|| (dwIpAddr != 0 && byLop == tBrdEthParamAll.dwIpNum) )
	{
		memset( &tBrdEthParamAll, 0, sizeof(tBrdEthParamAll) );
		nRet = AgtGetBrdEthParamAll( 0, &tBrdEthParamAll ); 

		LogPrint(LOG_LVL_KEYSTATUS,  MID_PUB_ALWAYS,"[SearchMpcPortChoiceByCfg] PortChoice.0->IpNum.%d\n", tBrdEthParamAll.dwIpNum);
		
		if (tBrdEthParamAll.dwIpNum >0)
		{
			for (u8 byIdx = 0; byIdx < tBrdEthParamAll.dwIpNum; byIdx++)
			{
				LogPrint(LOG_LVL_KEYSTATUS,  MID_PUB_ALWAYS, "[SearchMpcPortChoiceByCfg] PortChoice.0-><IpIndex.%d, ip.0x%x>!\n",
					byIdx, tBrdEthParamAll.atBrdEthParam[byIdx].dwIpAdrs);
			}
		}

		if ( nRet == ERROR )
		{
			printf( "[SearchMpcPortChoiceByCfg] Get eth0 params failed, use default(eth1)!\n" );
			LogPrint(LOG_LVL_KEYSTATUS,  MID_PUB_ALWAYS, "[SearchMpcPortChoiceByCfg] Get eth0 params failed, use default(eth1)!\n" );
		}
		else if ( tBrdEthParamAll.dwIpNum > 0 && tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs != 0 )
		{
			if ( dwIpAddr == 0 )
			{
				byPortChoice = 0;
			}
			else
			{
				for ( byLop = 0; byLop < tBrdEthParamAll.dwIpNum; byLop++ )
				{
					if ( htonl(dwIpAddr) == tBrdEthParamAll.atBrdEthParam[byLop].dwIpAdrs )
					{
						byPortChoice = 0;
						break;
					}
				}
			}					
		}
		
		if ( nRet == ERROR 
			|| tBrdEthParamAll.dwIpNum == 0 
			|| (tBrdEthParamAll.dwIpNum > 0 && tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs == 0)
			|| ( dwIpAddr != 0 && byLop == tBrdEthParamAll.dwIpNum) )
		{
			byPortChoice = 1;
		}
	}
	
#else
	//#ifdef _VXWORKS_
	// 先查eth1
	s32 nRet;
	TBrdEthParam tOneParam;
	memset(&tOneParam, 0, sizeof(tOneParam));
	nRet = AgtGetBrdEthParam( 1, &tOneParam );
	if ( nRet == ERROR )
	{
		printf( "[SearchMpcPortChoiceByCfg] Get eth1 param failed, search eth0!\n" );
	}
	else if ( tOneParam.dwIpAdrs != 0 )
	{
		if ( dwIpAddr == 0 )
		{
			byPortChoice = 1;
		}
		else if ( htonl(dwIpAddr) == tOneParam.dwIpAdrs )
		{
			byPortChoice = 1;
		}
	}
	
	if ( nRet == ERROR || 
		tOneParam.dwIpAdrs == 0 ||
		(dwIpAddr != 0 && htonl(dwIpAddr) != tOneParam.dwIpAdrs) ) 
	{
		//再查eth0
		memset(&tOneParam, 0, sizeof(tOneParam));
		nRet = AgtGetBrdEthParam( 0, &tOneParam );
		if ( nRet == ERROR )
		{
			printf( "[SearchMpcPortChoiceByCfg] Get eth0 param failed, use default(eth1)!\n" );
		}
		else if ( tOneParam.dwIpAdrs != 0 )
		{
			if ( dwIpAddr == 0)
			{
				byPortChoice = 0;
			}
			else if ( htonl(dwIpAddr) == tOneParam.dwIpAdrs )
			{
				byPortChoice = 0;
			}
		}
		
		if ( nRet == ERROR || 
			tOneParam.dwIpAdrs == 0 ||
			(dwIpAddr != 0 && htonl(dwIpAddr) != tOneParam.dwIpAdrs) ) 
		{
			byPortChoice = 1;
		}
	}
#endif
#endif
#endif			
	return byPortChoice;
}


/*=============================================================================
函 数 名： SaveMpcInfo
功    能： 
算法实现： 对于配置信息和实际情况不一致的问题，作以下保护
1、（只针对8000A的主备环境）首先考虑是否是由于槽号颠倒导致不一致，
如果是，则交换除槽号以外的其它配置信息
2、如果不是第一种情况，则取本地NIP配置的第一个IP为MPC的IP
全局变量： 
参    数：  u8 byLocalMpcIdx
TEqpBrdCfgEntry *ptLocalMpc
u8 byOtherMpcIdx
TEqpBrdCfgEntry *ptOtherMpc
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/5/14   4.0		周广程                  创建
=============================================================================*/
BOOL32 CCfgAgent::SaveMpcInfo( u8 byLocalMpcIdx, TEqpBrdCfgEntry *ptLocalMpc, u8 byOtherMpcIdx, TEqpBrdCfgEntry *ptOtherMpc, BOOL32 bSemTake )
{
	//检测输入参数的有效性
	if ( byLocalMpcIdx >= MAX_BOARD_NUM || ptLocalMpc == NULL || byOtherMpcIdx > MAX_BOARD_NUM )
	{
		printf( "[SaveMpcInfo] LocalMpcIdx(%d), point of localmpccfg(%d), othermpcidx(%d), error!\n",
			byLocalMpcIdx, (s32)ptLocalMpc, byOtherMpcIdx );

		LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS,"[SaveMpcInfo] LocalMpcIdx(%d), point of localmpccfg(%d), othermpcidx(%d), error!\n",
												byLocalMpcIdx, (s32)ptLocalMpc, byOtherMpcIdx );
		return FALSE;
	}
	
	//  [1/21/2011 chendaiwei]支持MPC2
	if ( ptLocalMpc->GetType() != BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ && ptLocalMpc->GetType() != BRD_TYPE_MPC2 )
	{
		printf( "[SaveMpcInfo] Local cfg is not MPC, type is %d, error!\n", ptLocalMpc->GetType() );

		LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS, "[SaveMpcInfo] Local cfg is not MPC, type is %d, error!\n", ptLocalMpc->GetType() );

		return FALSE;
	}
	
	if ( ptOtherMpc != NULL && ptOtherMpc->GetType() != BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ && ptOtherMpc->GetType() !=BRD_TYPE_MPC2)
	{
		printf( "[SaveMpcInfo] Other cfg is not MPC, type is %d, error!\n", ptOtherMpc->GetType() );

		LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS, "[SaveMpcInfo] Other cfg is not MPC, type is %d, error!\n", ptOtherMpc->GetType() );
		return FALSE;
	}
	
	// 是否主备环境
	BOOL32 bIsDouble = FALSE;
	if ( ptOtherMpc != NULL )
	{
		bIsDouble = TRUE;	
#ifdef _MINIMCU_
		printf( "[SaveMpcInfo] Mpc8000b in ms enviroment, it's impossible!\n" );
		LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS,"[SaveMpcInfo] Mpc8000b in ms enviroment, it's impossible!\n" );
		return FALSE;
#endif
	}
	
	u32 dwOrgMpcIp = ptLocalMpc->GetBrdIp();
	printf("[SaveMpcInfo] original Local config info: ip = 0x%x, slot = %d\n", 
													  ptLocalMpc->GetBrdIp(), ptLocalMpc->GetSlot() );

	LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS,"[SaveMpcInfo] original Local config info: ip = 0x%x, slot = %d\n", 
													  ptLocalMpc->GetBrdIp(), ptLocalMpc->GetSlot() );

	if ( bIsDouble && ptOtherMpc )
	{
		printf("[SaveMpcInfo] original Other config info: ip = 0x%x, slot = %d\n",
									ptOtherMpc->GetBrdIp(), ptOtherMpc->GetSlot() );

		LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[SaveMpcInfo] original Other config info: ip = 0x%x, slot = %d\n",
									ptOtherMpc->GetBrdIp(), ptOtherMpc->GetSlot() );
	}
	
	
	BOOL32 bError = FALSE;
	
	TBrdEthParamAll tMpcEthParamAll;
    memset( &tMpcEthParamAll, 0, sizeof(tMpcEthParamAll) );
#ifndef _VXWORKS_
	AgtGetBrdEthParamAll( ptLocalMpc->GetPortChoice(), &tMpcEthParamAll);
#else
	TBrdEthParam tMpcEthParam;
    memset( &tMpcEthParam, 0, sizeof(tMpcEthParam) );
	AgtGetBrdEthParam( ptLocalMpc->GetPortChoice(), &tMpcEthParam );
	tMpcEthParamAll.dwIpNum = 1;
	tMpcEthParamAll.atBrdEthParam = tMpcEthParam;	
#endif
	
	u32 dwLocalFirstIp = 0;
	u32 dwLop = 0;
	// 对于ipnum 等于0的情况，由于没有参照物，认为配置正确，不做特殊处理
	if ( tMpcEthParamAll.dwIpNum > 0 )
	{
		printf("[SaveMpcInfo]Local tMpcEthParamAll.dwIpNum.%d!\n", tMpcEthParamAll.dwIpNum);

		LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[SaveMpcInfo]Local tMpcEthParamAll.dwIpNum.%d!\n", tMpcEthParamAll.dwIpNum);

		for ( dwLop = 0; dwLop < tMpcEthParamAll.dwIpNum; dwLop++ )
		{
			printf("[SaveMpcInfo]Local tMpcEthParamAll.Ip[%d].0x%x!\n", 
										dwLop, ntohl( tMpcEthParamAll.atBrdEthParam[dwLop].dwIpAdrs ));
			
			LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS,"[SaveMpcInfo]Local tMpcEthParamAll.Ip[%d].0x%x!\n",
										dwLop, ntohl( tMpcEthParamAll.atBrdEthParam[dwLop].dwIpAdrs ));

			if ( ptLocalMpc->GetBrdIp() == ntohl( tMpcEthParamAll.atBrdEthParam[dwLop].dwIpAdrs ) )
			{
				break;
			}
		}
		if ( dwLop >= tMpcEthParamAll.dwIpNum )
		{
			printf( "[SaveMpcInfo] Local Mpc Ip(0x%x) in profile not exist, port choice: %d !\n", 
												      ptLocalMpc->GetBrdIp(), ptLocalMpc->GetPortChoice() );

			LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[SaveMpcInfo] Local Mpc Ip(0x%x) in profile not exist, port choice: %d !\n", 
												      ptLocalMpc->GetBrdIp(), ptLocalMpc->GetPortChoice() );
			bError = TRUE;
			dwLocalFirstIp = ntohl(tMpcEthParamAll.atBrdEthParam[0].dwIpAdrs);
		}
	}
	
	
	// 本地MPC配置出错 
	if ( bError)
	{
		//尝试另外一个MPC配置是否符合
		BOOL32 byAdoptFirstIp = TRUE;
		if (bIsDouble && ptOtherMpc)
		{
			memset(&tMpcEthParamAll, 0, sizeof(tMpcEthParamAll));
#ifndef _VXWORKS_
			AgtGetBrdEthParamAll( ptOtherMpc->GetPortChoice(), &tMpcEthParamAll);
#else
			TBrdEthParam tMpcEthParam;
			memset( &tMpcEthParam, 0, sizeof(tMpcEthParam) );
			AgtGetBrdEthParam( ptOtherMpc->GetPortChoice(), &tMpcEthParam );
			tMpcEthParamAll.dwIpNum = 1;
			tMpcEthParamAll.atBrdEthParam = tMpcEthParam;	
#endif
			if (tMpcEthParamAll.dwIpNum > 0)
			{
				printf("[SaveMpcInfo]Other tMpcEthParamAll.dwIpNum.%d!\n", tMpcEthParamAll.dwIpNum);
				
				LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[SaveMpcInfo]Other tMpcEthParamAll.dwIpNum.%d!\n", tMpcEthParamAll.dwIpNum);
			
				for ( dwLop = 0; dwLop < tMpcEthParamAll.dwIpNum; dwLop++ )
				{
					if ( ptOtherMpc->GetBrdIp() == ntohl( tMpcEthParamAll.atBrdEthParam[dwLop].dwIpAdrs ) )
					{
						// 交换两个MPC的配置信息，除槽号以外
						printf( "[SaveMpcInfo] Change two mpc cfgs info.\n" );
						
						TEqpBrdCfgEntry tTmpCfg;
						memcpy( &tTmpCfg, ptLocalMpc, sizeof(TEqpBrdCfgEntry) );
						memcpy( ptLocalMpc, ptOtherMpc, sizeof(TEqpBrdCfgEntry) );
						memcpy( ptOtherMpc, &tTmpCfg, sizeof(TEqpBrdCfgEntry) );
						
						u8 byTmpSlot = ptLocalMpc->GetSlot();
						ptLocalMpc->SetSlot( ptOtherMpc->GetSlot() );
						ptOtherMpc->SetSlot( byTmpSlot );
						
						u8 byTmpIdx = byLocalMpcIdx;
						byLocalMpcIdx = byOtherMpcIdx;
						byOtherMpcIdx = byTmpIdx;
						
						printf("[SaveMpcInfo] Local: ip = 0x%x, slot = %d\n", ptLocalMpc->GetBrdIp(), ptLocalMpc->GetSlot() );
						printf("[SaveMpcInfo] Other: ip = 0x%x, slot = %d\n", ptOtherMpc->GetBrdIp(), ptOtherMpc->GetSlot() );

						LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[SaveMpcInfo] Local: ip = 0x%x, slot = %d\n", ptLocalMpc->GetBrdIp(), ptLocalMpc->GetSlot() );
						LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[SaveMpcInfo] Other: ip = 0x%x, slot = %d\n", ptOtherMpc->GetBrdIp(), ptOtherMpc->GetSlot() );
						
						byAdoptFirstIp = FALSE;
						break;
					}
				}
			}
		}
		
		if (byAdoptFirstIp)
		{
			//尝试失败保存原网口第一个ip
			printf( "[SaveMpcInfo] Set local cfg ip(0x%x).\n", dwLocalFirstIp );
			LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[SaveMpcInfo] Set local cfg ip(0x%x).\n", dwLocalFirstIp );

			ptLocalMpc->SetBrdIp( dwLocalFirstIp );
		}
	}
	
	// xsl [8/18/2006] 主备时采用同样的操作系统
#ifdef WIN32
	ptLocalMpc->SetOSType( OS_TYPE_WIN32 );
	if ( bIsDouble && ptOtherMpc )
	{
		if ( ptOtherMpc != NULL )
		{
			ptOtherMpc->SetOSType( OS_TYPE_WIN32 );
		}
	}
#elif defined _LINUX_
	ptLocalMpc->SetOSType( OS_TYPE_LINUX );
	if ( bIsDouble )
	{
		if( NULL != ptOtherMpc )
		{
			ptOtherMpc->SetOSType( OS_TYPE_LINUX );
		}
	}
#else
	ptLocalMpc->SetOSType( OS_TYPE_VXWORKS );
	if ( bIsDouble )
	{
		ptOtherMpc->SetOSType( OS_TYPE_VXWORKS );
	}
#endif
	
	//更新内存信息
	m_tMPCInfo.SetLocalIp( ptLocalMpc->GetBrdIp() );
	m_tMPCInfo.SetLocalPortChoice( ptLocalMpc->GetPortChoice() );
	m_tMPCInfo.SetLocalLayer( ptLocalMpc->GetLayer() ); 
	m_tMPCInfo.SetLocalSlot( ptLocalMpc->GetSlot() );
	m_tMPCInfo.SetOSType(ptLocalMpc->GetOSType());
	
	memcpy( &m_atBrdCfgTable[byLocalMpcIdx], ptLocalMpc, sizeof(TEqpBrdCfgEntry) );
	m_atBrdCfgTable[byLocalMpcIdx].SetState( BOARD_STATUS_INLINE );
    m_atBrdCfgTable[byLocalMpcIdx].SetVersion( GetMcuSoftVersion() );
	
	//仅主板能读取层号
	if( m_tMPCInfo.GetIsLocalMaster() )
	{	
		TBrdPosition tPos;
		if ( OK != AgtQueryBrdPosition(&tPos) )
		{
			printf( "[SaveMpcInfo] Get board TBrdPosition failed!\n" );
			LogPrint(LOG_LVL_KEYSTATUS,  MID_PUB_ALWAYS, "[SaveMpcInfo] Get board TBrdPosition failed!\n" );
			return FALSE;
		}

		printf("[SaveMpcInfo] Local real info: Layer= %d, Slot= %d, Id= %d\n", 
														tPos.byBrdLayer, tPos.byBrdSlot, tPos.byBrdID );
		
		LogPrint(LOG_LVL_KEYSTATUS,  MID_PUB_ALWAYS, "[SaveMpcInfo] Local real info: Layer= %d, Slot= %d, Id= %d\n", 
														tPos.byBrdLayer, tPos.byBrdSlot, tPos.byBrdID );

		//m_tMPCInfo.SetLocalLayer(tPos.byBrdLayer); // 记录本机
		if (m_atBrdCfgTable[byLocalMpcIdx].GetLayer() != tPos.byBrdLayer &&
			tPos.byBrdLayer < 4)
		{
			bError = TRUE;
			m_tMPCInfo.SetLocalLayer(tPos.byBrdLayer);
			m_atBrdCfgTable[byLocalMpcIdx].SetLayer( tPos.byBrdLayer );
			m_atBrdCfgTable[byLocalMpcIdx].SetSlot( tPos.byBrdSlot );
		}
	}
	
	if ( bIsDouble && ptOtherMpc)
	{
		m_tMPCInfo.SetOtherMpcLayer( ptOtherMpc->GetLayer() );
		m_tMPCInfo.SetOtherMpcSlot( ptOtherMpc->GetSlot() );
		m_tMPCInfo.SetOtherMpcIp( ptOtherMpc->GetBrdIp() );
		
		u8 byRet = BrdMPCQueryAnotherMPCState();
		m_tMPCInfo.SetOtherMpcStatus( BrdStatusHW2Agt(byRet) );
		BOOL32 bHasOtherMpc = ( BRD_MPC_IN_POSITION == byRet )? TRUE:FALSE;
		m_tMPCInfo.SetIsHaveOtherMpc( bHasOtherMpc );
        m_tMPCInfo.SetOtherMpcPort( MCU_LISTEN_PORT );
		
		memcpy( &m_atBrdCfgTable[byOtherMpcIdx], ptOtherMpc, sizeof(TEqpBrdCfgEntry) );
		m_atBrdCfgTable[byOtherMpcIdx].SetState( BrdStatusHW2Agt(byRet) );		
	}
	
	// 修改配置文件
	if( bError )
    {
		printf( "[SaveMpcInfo] Modify brd.%d cfg entry.\n", byLocalMpcIdx );

		LogPrint(LOG_LVL_KEYSTATUS,  MID_PUB_ALWAYS, "[SaveMpcInfo] Modify brd.%d cfg entry.\n", byLocalMpcIdx );

        TBoardInfo tBoardInfo;
        tBoardInfo.SetBrdId( m_atBrdCfgTable[byLocalMpcIdx].GetBrdId() );
        tBoardInfo.SetLayer( m_atBrdCfgTable[byLocalMpcIdx].GetLayer() );
        tBoardInfo.SetSlot( m_atBrdCfgTable[byLocalMpcIdx].GetSlot() );
        tBoardInfo.SetType( m_atBrdCfgTable[byLocalMpcIdx].GetType() );
        tBoardInfo.SetCastChoice( m_atBrdCfgTable[byLocalMpcIdx].GetCastChoice() );
        tBoardInfo.SetPortChoice( m_atBrdCfgTable[byLocalMpcIdx].GetPortChoice() );
        tBoardInfo.SetBrdIp( m_atBrdCfgTable[byLocalMpcIdx].GetBrdIp() );
        tBoardInfo.SetAlias( m_atBrdCfgTable[byLocalMpcIdx].GetAlias() );
        
        WriteBoardTable( byLocalMpcIdx, &tBoardInfo, bSemTake );

		//NIP IP地址与配置文件不一致，修改配置文件[6/5/2012 chendaiwei]
#ifdef _MINIMCU_
		BOOL32 bRet = AgentGetDSCInfo( m_achCfgName );
		//增加使用备份文件进行保护, zgc, 2007-03-20
		if( !bRet )
		{
			bRet = AgentGetDSCInfo( m_achCfgBakName );
			if( !bRet )
			{
				SetDscInfoByDefault();
			}
		}
		
		TMINIMCUNetParamAll tMINIMCUNetParamAll;
		TMINIMCUNetParam tNetParam;
		m_tMcuDscInfo.GetMcsAccessAddrAll( tMINIMCUNetParamAll );

		for( u8 byIdx = 0; byIdx < tMINIMCUNetParamAll.GetNetParamNum(); byIdx++ )
		{
			tMINIMCUNetParamAll.GetNetParambyIdx( byIdx, tNetParam );
			//修改配置文件IP为NIP IP[6/5/2012 chendaiwei]
			
			printf("tNetParam.GetIpAddr() == 0x%x\n",tNetParam.GetIpAddr());

			if( tNetParam.GetIpAddr() == dwOrgMpcIp )
			{
				if(tNetParam.GetNetworkType() == (u8)tNetParam.enLAN)
				{
					tNetParam.SetLanParam(tBoardInfo.GetBrdIp(),tNetParam.GetIpMask(),tNetParam.GetGatewayIp());
				}
				else
				{
					s8 asServerAlias[MAXLEN_PWD] = {0};
					memcpy(asServerAlias,tNetParam.GetServerAlias(),strlen(tNetParam.GetServerAlias()));
					tNetParam.SetWanParam(tBoardInfo.GetBrdIp(),tNetParam.GetIpMask(),tNetParam.GetGatewayIp(),asServerAlias);
				}
				
				tMINIMCUNetParamAll.DelNetParamByIdx(byIdx);
				tMINIMCUNetParamAll.AddOneNetParam(tNetParam);
				m_tMcuDscInfo.SetMcsAccessAddrAll(tMINIMCUNetParamAll);
				
				SetDSCInfo(&m_tMcuDscInfo,TRUE);
				printf("[SaveMpcInfo] dcs inf changed as follows:\n");
				m_tMcuDscInfo.cosPrint();
			}
		}
#endif
		
		if ( bIsDouble )
		{
			printf( "[SaveMpcInfo] Modify brd.%d cfg entry.\n", byOtherMpcIdx );
			LogPrint(LOG_LVL_KEYSTATUS,  MID_PUB_ALWAYS, "[SaveMpcInfo] Modify brd.%d cfg entry.\n", byOtherMpcIdx );
			memset( &tBoardInfo, 0, sizeof(tBoardInfo) );
			tBoardInfo.SetBrdId( m_atBrdCfgTable[byOtherMpcIdx].GetBrdId() );
			tBoardInfo.SetLayer( m_atBrdCfgTable[byOtherMpcIdx].GetLayer() );
			tBoardInfo.SetSlot( m_atBrdCfgTable[byOtherMpcIdx].GetSlot() );
			tBoardInfo.SetType( m_atBrdCfgTable[byOtherMpcIdx].GetType() );
			tBoardInfo.SetCastChoice( m_atBrdCfgTable[byOtherMpcIdx].GetCastChoice() );
			tBoardInfo.SetPortChoice( m_atBrdCfgTable[byOtherMpcIdx].GetPortChoice() );
			tBoardInfo.SetBrdIp( m_atBrdCfgTable[byOtherMpcIdx].GetBrdIp() );
			tBoardInfo.SetAlias( m_atBrdCfgTable[byOtherMpcIdx].GetAlias() );
			
			WriteBoardTable( byOtherMpcIdx, &tBoardInfo, bSemTake );
		}
    } 
	
	return TRUE;
}

/*=============================================================================
函 数 名： SetLocalInfo
功    能： 设置本地信息
算法实现： 
全局变量： 
参    数： TLocalInfo * ptLocalInfo
返 回 值： BOOL32 
=============================================================================*/
u16 CCfgAgent::SetLocalInfo(TLocalInfo * ptLocalInfo,u32 dwNodeValue)
{
    memcpy( &m_tMcuLocalInfo, ptLocalInfo, sizeof(TLocalInfo) );
	
    u16 wRet = SUCCESS_AGENT; 
	if(dwNodeValue == 0 )
	{
		wRet |= WriteStringToFile(SECTION_mcuLocalInfo, KEY_mcuAlias, m_tMcuLocalInfo.GetAlias());
		wRet |= WriteStringToFile(SECTION_mcuLocalInfo, KEY_mcuE164Number, m_tMcuLocalInfo.GetE164Num());
		wRet |= WriteIntToFile(SECTION_mcuLocalInfo, KEY_mucId, (s32)m_tMcuLocalInfo.GetMcuId()); 
		wRet |= WriteIntToFile(SECTION_mcuLocalInfo, Key_mcuCheckLinkTime, (s32)m_tMcuLocalInfo.GetCheckTime());
		wRet |= WriteIntToFile(SECTION_mcuLocalInfo, KEY_mcuCheckMtLinkTimes, (s32)m_tMcuLocalInfo.GetCheckTimes());
		wRet |= WriteIntToFile(SECTION_mcuLocalInfo, Key_mcuRefreshListTime, (s32)m_tMcuLocalInfo.GetListRefreshTime());
		wRet |= WriteIntToFile(SECTION_mcuLocalInfo, Key_mcuAudioRefreshTime, (s32)m_tMcuLocalInfo.GetAudioRefreshTime());
		wRet |= WriteIntToFile(SECTION_mcuLocalInfo, Key_mcuVideoRefreshTime, (s32)m_tMcuLocalInfo.GetVideoRefreshTime());
		wRet |= WriteIntToFile(SECTION_mcuLocalInfo, Key_mcuIsSaveBand, (s32)m_tMcuLocalInfo.GetIsSaveBand());
		wRet |= WriteIntToFile(SECTION_mcuLocalInfo, Key_mcuIsNPlusMode, (s32)m_tMcuLocalInfo.IsNPlusMode());
		wRet |= WriteIntToFile(SECTION_mcuLocalInfo, Key_mcuIsNPlusBackupMode, (s32)m_tMcuLocalInfo.IsNPlusBackupMode());
		
		
		wRet |= WriteIntToFile(SECTION_mcuLocalInfo, key_mcuIsHoldDefaultConf, (s32)m_tMcuLocalInfo.GetIsHoldDefaultConf());
		wRet |= WriteIntToFile(SECTION_mcuLocalInfo, key_mcuIsShowMMcuMtList, (s32)m_tMcuLocalInfo.GetIsShowMMcuMtList());
		wRet |= WriteIntToFile(SECTION_mcuLocalInfo, key_mcuMaxMcsOnGoingConfNum, (s32)m_tMcuLocalInfo.GetMaxMcsOnGoingConfNum());
		wRet |= WriteIntToFile(SECTION_mcuLocalInfo, key_mcuAdminLevel, (s32)m_tMcuLocalInfo.GetAdminLevel());
		wRet |= WriteIntToFile(SECTION_mcuLocalInfo, key_mcuLocalConfNameShowType, m_tMcuLocalInfo.GetConfNameShowType());
		wRet |= WriteIntToFile(SECTION_mcuLocalInfo, key_mcuIsMMcuSpeaker, m_tMcuLocalInfo.GetIsMMcuSpeaker());
		
		
		s8   achIpAddr[32];
		memset( achIpAddr, '\0', sizeof(achIpAddr));    
		GetIpFromU32(achIpAddr, htonl(m_tMcuLocalInfo.GetNPlusMcuIp()) );
		wRet |= WriteStringToFile(SECTION_mcuLocalInfo, Key_mcuNPlusMcuIp, achIpAddr);
		
		wRet |= WriteIntToFile(SECTION_mcuLocalInfo, Key_mcuNPlusRtdTime, (s32)m_tMcuLocalInfo.GetNPlusRtdTime());
		wRet |= WriteIntToFile(SECTION_mcuLocalInfo, Key_mcuNPlusRtdNum, (s32)m_tMcuLocalInfo.GetNPlusRtdNum());
		wRet |= WriteIntToFile(SECTION_mcuLocalInfo, Key_mcuIsNPlusRollBack, (s32)m_tMcuLocalInfo.GetIsNPlusRollBack());
	}
	else
	{
		//目前网管支持写入NODE[2/21/2013 chendaiwei]
		switch(dwNodeValue)
		{
		case NODE_MCUID:
			wRet |= WriteIntToFile(SECTION_mcuLocalInfo, KEY_mucId, (s32)m_tMcuLocalInfo.GetMcuId()); 
			break;
		case NODE_MCUALIAS:
			wRet |= WriteStringToFile(SECTION_mcuLocalInfo, KEY_mcuAlias, m_tMcuLocalInfo.GetAlias());
			break;
		case NODE_MCUE164NUMBER:
			wRet |= WriteStringToFile(SECTION_mcuLocalInfo, KEY_mcuE164Number, m_tMcuLocalInfo.GetE164Num());
			break;
		default:
			break;
		}
	}
	
    if ( SUCCESS_AGENT != wRet )
        printf("[SetLocalInfo] failed, wRet.%d \n", wRet );
	
    return wRet;
}

/*=============================================================================
函 数 名： SetNetWorkInfo
功    能： 设置网络信息
算法实现： 
全局变量： 
参    数： TNetWorkInfo * ptNetWorkInfo
		   u32 dwNodeValue 网管标识具体修改的结点
返 回 值： BOOL32 
=============================================================================*/
u16 CCfgAgent::SetNetWorkInfo(TNetWorkInfo* ptNetWorkInfo,u32 dwNodeValue )
{
    BOOL32 bFlag = TRUE;
    s8   achIpAddr[32];
    memset( achIpAddr, '\0', sizeof(achIpAddr));
	memcpy( &m_tMcuNetwork, ptNetWorkInfo, sizeof(TNetWorkInfo) );

	u16 wRet = SUCCESS_AGENT;
	if( dwNodeValue == 0) //全修改
	{
		GetIpFromU32(achIpAddr, htonl(m_tMcuNetwork.GetGkIp()) );
		wRet = WriteStringToFile(SECTION_mcuNetwork, KEY_mcunetGKIpAddr, achIpAddr);
		
		wRet |= WriteIntToFile(SECTION_mcuNetwork, KEY_mcunetGKCharge, (s32)m_tMcuNetwork.GetIsGKCharge() );
		
		memset( achIpAddr, '\0', sizeof(achIpAddr) );
		GetIpFromU32(achIpAddr, htonl(m_tMcuNetwork.GetRRQMtadpIp()) );
		wRet |= WriteStringToFile(SECTION_mcuNetwork, KEY_mcunetRRQMtadpIp, achIpAddr);
		
		memset( achIpAddr, '\0', sizeof(achIpAddr));
		GetIpFromU32(achIpAddr, htonl(m_tMcuNetwork.GetCastIp()) );
		wRet |= WriteStringToFile(SECTION_mcuNetwork, KEY_mcunetMulticastIpAddr, achIpAddr);
		
		wRet |= WriteIntToFile(SECTION_mcuNetwork, KEY_mcunetMulticastPort, (s32)m_tMcuNetwork.GetCastPort());
		wRet |= WriteIntToFile(SECTION_mcuNetwork, KEY_mcunetRecvStartPort, (s32)m_tMcuNetwork.GetRecvStartPort()); 
		wRet |= WriteIntToFile(SECTION_mcuNetwork, KEY_mcunet225245StartPort, (s32)m_tMcuNetwork.Get225245StartPort());
		wRet |= WriteIntToFile(SECTION_mcuNetwork, KEY_mcunet225245MtNum, (s32)m_tMcuNetwork.Get225245MtNum()); 
		wRet |= WriteIntToFile(SECTION_mcuNetwork, KEY_mcunetUseMPCTransData, (s32)m_tMcuNetwork.GetMpcTransData());
		wRet |= WriteIntToFile(SECTION_mcuNetwork, KEY_mcunetUseMPCStack, (s32)m_tMcuNetwork.GetMpcStack());
		// 设置MTU大小, zgc, 2007-04-02
		wRet |= WriteIntToFile(SECTION_mcuNetwork, KEY_mcunetMTUSize, (s32)m_tMcuNetwork.GetMTUSize() );
		
		//TODO:网管是否要支持
		wRet |= WriteIntToFile(SECTION_mcuNetwork, KEY_mcunetIsGKRRQUsePwd, (s32)m_tMcuNetwork.GetGkRRQUsePwdFlag());
		wRet |= WriteStringToFile(SECTION_mcuNetwork, KEY_mcunetGKRRQPwd, m_tMcuNetwork.GetGkRRQPassword());
	}
	else
	{
		switch(dwNodeValue)
		{
		case NODE_MCUNETGKIPADDR:
			GetIpFromU32(achIpAddr, htonl(m_tMcuNetwork.GetGkIp()) );
			wRet = WriteStringToFile(SECTION_mcuNetwork, KEY_mcunetGKIpAddr, achIpAddr);
			break;
		case NODE_MCUNETMULTICASTIPADDR:
			memset( achIpAddr, '\0', sizeof(achIpAddr));
			GetIpFromU32(achIpAddr, htonl(m_tMcuNetwork.GetCastIp()) );
			wRet |= WriteStringToFile(SECTION_mcuNetwork, KEY_mcunetMulticastIpAddr, achIpAddr);
			break;
		case NODE_MCUNETMULTICASTPORT:
			wRet |= WriteIntToFile(SECTION_mcuNetwork, KEY_mcunetMulticastPort, (s32)m_tMcuNetwork.GetCastPort());
			break;
		case NODE_MCUNETSTARTRECVPORT:
			wRet |= WriteIntToFile(SECTION_mcuNetwork, KEY_mcunetRecvStartPort, (s32)m_tMcuNetwork.GetRecvStartPort()); 
			break;
		case NODE_MCUNET225245STARTPORT:
			wRet |= WriteIntToFile(SECTION_mcuNetwork, KEY_mcunet225245StartPort, (s32)m_tMcuNetwork.Get225245StartPort());
			break;
		case NODE_MCUNET225245MAXMTNUM:
			wRet |= WriteIntToFile(SECTION_mcuNetwork, KEY_mcunet225245MtNum, (s32)m_tMcuNetwork.Get225245MtNum()); 
			break;
		case NODE_MCUNETUSEMPCTRANSDATA:
			wRet |= WriteIntToFile(SECTION_mcuNetwork, KEY_mcunetUseMPCTransData, (s32)m_tMcuNetwork.GetMpcTransData());
			break;
		case NODE_MCUNETUSEMPCSTACK:
			wRet |= WriteIntToFile(SECTION_mcuNetwork, KEY_mcunetUseMPCStack, (s32)m_tMcuNetwork.GetMpcStack());
			break;
		case NODE_MCUNETGKUSED:
			wRet |= WriteIntToFile(SECTION_mcuNetwork, KEY_mcunetGKCharge, (s32)m_tMcuNetwork.GetIsGKCharge() );
			break;
		}
	}
		
    if ( SUCCESS_AGENT != wRet )
        printf("[SetLocalInfo] failed, wRet.%d \n", wRet );
    
    return wRet;
}

/*=============================================================================
函 数 名： GetNetSyncInfo
功    能： 取网同步信息
算法实现： 
全局变量： 
参    数： TNetSyncInfo* ptNetSyncInfo
返 回 值： BOOL32 
=============================================================================*/
u16 CCfgAgent::GetNetSyncInfo(TNetSyncInfo* ptNetSyncInfo)
{
	memcpy( ptNetSyncInfo, &m_tNetSync, sizeof(TNetSyncInfo) );
    return SUCCESS_AGENT;
}

/*=============================================================================
函 数 名： SetNetSyncInfo
功    能： 设置网同步信息
算法实现： 
全局变量： 
参    数： TNetSyncInfo * ptNetSyncInfo
返 回 值： BOOL32 
=============================================================================*/
u16 CCfgAgent::SetNetSyncInfo(TNetSyncInfo * ptNetSyncInfo)
{
	u16 wRet = SUCCESS_AGENT;
	
	u8 byMode = ptNetSyncInfo->GetMode();
    if( NETSYNTYPE_VIBRATION == byMode || NETSYNTYPE_TRACE == byMode || 0 == byMode )
	{
		m_tNetSync.SetMode( byMode );
		wRet |= WriteIntToFile(SECTION_mcueqpNetSync, KEY_mcueqpNetSyncMode, (s32)byMode);
	}
    else
    {
        wRet = ERR_AGENT_VALUEBESET;
    }
	
	u8 byDTSlot = ptNetSyncInfo->GetDTSlot();
	if( byDTSlot <= NETSYN_MAX_DTNUM )
	{
		m_tNetSync.SetDTSlot( byDTSlot );
		wRet |= WriteIntToFile(SECTION_mcueqpNetSync, KEY_mcueqpNetSyncDTSlot, (s32)byDTSlot);
	}
    else
    {
        wRet = ERR_AGENT_VALUEBESET;
    }
	
	u8 byE1Index = ptNetSyncInfo->GetE1Index();
	if( byE1Index <= NETSYN_MAX_E1NUM)
	{
		m_tNetSync.SetE1Index( byE1Index );
		wRet |= WriteIntToFile(SECTION_mcueqpNetSync, KEY_mcueqpNetSyncE1Index, (s32)byE1Index);
	}
    else
    {
        wRet = ERR_AGENT_VALUEBESET;
    }
    if ( SUCCESS_AGENT != wRet )
        printf("[SetNetSyncInfo] failed, wRet.%d \n", wRet );
	
    return wRet;
}

/*=============================================================================
函 数 名： SetLoginInfo
功    能： 
算法实现： 
全局变量： 
参    数： TLoginInfo *ptLoginInfo
返 回 值： u16 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/10/19   4.0		周广程                  创建
=============================================================================*/
u16 CCfgAgent::SetLoginInfo( TLoginInfo *ptLoginInfo )
{
	if ( NULL == ptLoginInfo )
	{
		OspPrintf( TRUE, FALSE, "[SetLoginInfo] param is null! Error!\n" );
		return ERR_AGENT_VALUEBESET;
	}
	
	s8 achPwd[MAXLEN_PWD+1];
	memset(achPwd, 0, sizeof(achPwd));
	ptLoginInfo->GetPwd(achPwd, sizeof(achPwd));
	if ( NULL == ptLoginInfo->GetUser() || 0 == strlen(achPwd) )
	{
		OspPrintf( TRUE, FALSE, "[SetLoginInfo] Login name or password is null! Error!\n" );
		return ERR_AGENT_VALUEBESET;
	}
	
#ifdef _LINUX_
	ptLoginInfo->Print();
	TUserType emUserType = USER_TYPE_TELNET_SUPER;
	STATUS bRet = BrdAddUser( (s8*)ptLoginInfo->GetUser(), achPwd, emUserType );
	if ( ERROR == bRet )
	{
		OspPrintf( TRUE, FALSE, "[SetLoginInfo] Set login info failed!\n" );
		return ERR_AGENT_SETLOGININFO;
	}
	else
	{
		OspPrintf( TRUE, FALSE, "[SetLoginInfo] Set login info success!\n" );
	}
#endif
	return SUCCESS_AGENT;
}

/*=============================================================================
函 数 名： GetLoginInfo
功    能： 
算法实现： 
全局变量： 
参    数： TLoginInfo *ptLoginInfo
返 回 值： u16 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/10/19   4.0		周广程                  创建
=============================================================================*/
u16 CCfgAgent::GetLoginInfo( TLoginInfo *ptLoginInfo )
{
	if ( NULL == ptLoginInfo )
	{
		printf( "[SetLoginInfo] Param is null! Error!\n" );
		return ERR_AGENT_VALUEBESET;
	}
#ifndef WIN32
	// 等待NIP接口
#endif
	return SUCCESS_AGENT;
}

/*=============================================================================
函 数 名： SetSystemInfo
功    能： 设置MCU系统信息
算法实现： 
全局变量： 
参    数： TMcuSystem * ptMcuSysInfo
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人       修改内容
2006/11/02  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::SetSystemInfo( TMcuSystem * ptMcuSysInfo )
{
	// [02/13/2012-mqs] 将设备重启操作放在写配置文件之后做，以避免正在写配置文件
	//            时，网管让设备重启导致配置文件清空问题。
    //SetSystemState( ptMcuSysInfo->GetState() );       // STATE
	
    // [11/27/2006-zbq] 此处不能写入MPC板时间，会造成系统时间拖延
    //SyncSystemTime( ptMcuSysInfo->GetTime() );      // TIME
	
    SetSystemCfgVer( ptMcuSysInfo->GetConfigVer() );  // ConfigVer
    memcpy( &m_tMcuSystem, ptMcuSysInfo, sizeof(TMcuSystem) );

	SetSystemState( ptMcuSysInfo->GetState() );       // STATE
	
    return SUCCESS_AGENT;
}

/*=============================================================================
函 数 名： GetSystemInfo
功    能： 取MCU系统信息
算法实现： 
全局变量： 
参    数： TMcuSystem tMcuSysInfo
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人       修改内容
2006/11/02  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::GetSystemInfo( TMcuSystem *ptMcuSysInfo )
{
    time_t  tCurTime;
    struct tm   *ptTime;
    s8 achBuf[32];
    memset(achBuf, '\0', sizeof(achBuf));
    tCurTime = time( 0 );
    ptTime = localtime( &tCurTime );
    sprintf( achBuf, "%4.4u%2.2u%2.2u%2.2u%2.2u%2.2u", ptTime->tm_year + 1900, 			
		ptTime->tm_mon + 1, ptTime->tm_mday, ptTime->tm_hour, ptTime->tm_min, ptTime->tm_sec );
    m_tMcuSystem.SetTime( achBuf );
	
    memcpy( ptMcuSysInfo, &m_tMcuSystem, sizeof(TMcuSystem) );
    return SUCCESS_AGENT;
}



/*=============================================================================
函 数 名： SetMcuPfmInfo
功    能： 设置MCU系统信息
算法实现： 
全局变量： 
参    数： TMcuSystem * ptMcuSysInfo
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人       修改内容
2006/11/02  4.0         张宝卿       创建
=============================================================================*/
// u16 CCfgAgent::SetMcuPfmInfo( TMcuPfmInfo * ptMcuPfmInfo )
// {
// 	if (ptMcuPfmInfo == NULL)
// 	{
// 		return !SUCCESS_AGENT;
// 	}
// 	
//     memcpy( &m_tMcuPfmInfo, ptMcuPfmInfo, sizeof(TMcuPfmInfo) );
// 	
//     return SUCCESS_AGENT;
// }

/*=============================================================================
函 数 名： GetMcuPfmInfo
功    能： 取MCU 性能数据信息
算法实现： 
全局变量： 
参    数： TMcuPfmInfo ptMcuPfmInfo
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人       修改内容
2006/11/02  4.0         陆昆朋       创建
=============================================================================*/
u16 CCfgAgent::GetMcuPfmInfo( TMcuPfmInfo * ptMcuPfmInfo )
{
	if (ptMcuPfmInfo == NULL)
	{
		return !SUCCESS_AGENT;
	}
	
    memcpy( ptMcuPfmInfo, &m_tMcuPfmInfo, sizeof(TMcuPfmInfo) );
    return SUCCESS_AGENT;
}

TMcuPfmInfo* CCfgAgent::GetMcuPfmInfo()
{
	return &m_tMcuPfmInfo;
}

u16 CCfgAgent::GetMcuUpdateInfo( TMcuUpdateInfo * ptMcuUpdateInfo )
{
	if (ptMcuUpdateInfo == NULL)
	{
		return !SUCCESS_AGENT;
	}
	
    memcpy( ptMcuUpdateInfo, &m_tMcuUpdateInfo, sizeof(TMcuUpdateInfo) );
    return SUCCESS_AGENT;
}

u16 CCfgAgent::SetMcuUpdateInfo( TMcuUpdateInfo * ptMcuUpdateInfo )
{
	if (ptMcuUpdateInfo == NULL)
	{
		return !SUCCESS_AGENT;
	}
	
    memcpy( &m_tMcuUpdateInfo, ptMcuUpdateInfo, sizeof(TMcuUpdateInfo) );
	
    return SUCCESS_AGENT;
}

TMcuUpdateInfo* CCfgAgent::GetMcuUpdateInfo()
{
	return &m_tMcuUpdateInfo;
}

/*=============================================================================
函 数 名： ReadTrapTable
功    能： 读Trap表
算法实现： 
全局变量： 
参    数： u8* pbyTrapNum
TTrapInfo* ptTrapTable
返 回 值： BOOL32 
=============================================================================*/
u16 CCfgAgent::ReadTrapTable(u8* pbyTrapNum, TTrapInfo* ptTrapTable )
{
    if(NULL == pbyTrapNum || NULL == ptTrapTable)
    {
        return ERR_AGENT_NULLPARAM;
    }
	
    u16 wRet = SUCCESS_AGENT;
    *pbyTrapNum = (u8)m_dwTrapRcvEntryNum;
    for(u8 byRow = 1; byRow <= m_dwTrapRcvEntryNum; byRow++)
    {
        if( NULL != &ptTrapTable[byRow-1] )
        {
            wRet = GetTrapInfo(byRow, &ptTrapTable[byRow-1]);
            if(SUCCESS_AGENT != wRet)
            {
                break;
            }
        }
        else
        {
            wRet = ERR_AGENT_NULLPARAM;
        }
    }
    return wRet;
}

/*=============================================================================
函 数 名： WriteTrapTable
功    能： 写Trap表
算法实现： 
全局变量： 
参    数： u8 byTrapNum
TTrapInfo* ptTrapTable
返 回 值： u16  
=============================================================================*/
u16  CCfgAgent::WriteTrapTable(u8 byTrapNum, TTrapInfo* ptTrapTable)
{
    if( NULL == ptTrapTable && 0 != byTrapNum )
    {
        return ERR_AGENT_NULLPARAM;
    }
    m_dwTrapRcvEntryNum = byTrapNum;
	
    u16 wRet = SUCCESS_AGENT;
    wRet = WriteTableEntryNum( m_achCfgName, (s8*)SECTION_mcunetTrapRcvTable,
		(s8*)ENTRY_NUM, byTrapNum );
    if( SUCCESS_AGENT != wRet )
    {
        printf( "[WriteTrapTable] Fail to write profile in %s\n", SECTION_mcunetTrapRcvTable );
        return wRet;
    }
	
    memset(m_atTrapRcvTable, 0, sizeof(m_atTrapRcvTable));
    
    for(u8 byRow = 1; byRow <= byTrapNum && ptTrapTable; byRow++)
    {
        wRet = SetTrapInfo(byRow, &ptTrapTable[byRow-1]);
        if( SUCCESS_AGENT != wRet )
        {
            break;
        }
    }
    return wRet;
}

/*=============================================================================
函 数 名： ReadBrdTable
功    能： 读单板表
算法实现： 
全局变量： 
参    数： u8* pbyBrdNum
TBoardInfo* ptBoardTable
返 回 值： BOOL32 
=============================================================================*/
u16 CCfgAgent::ReadBrdTable(u8* pbyBrdNum, TBoardInfo* ptBoardTable)
{
    if(NULL == pbyBrdNum || NULL == ptBoardTable)
    {
        Agtlog( LOG_ERROR, "[ReadBrdTable] param err: pbyNum.0x%x, ptTable.0x%x !\n", pbyBrdNum, ptBoardTable );
        return ERR_AGENT_NULLPARAM;
    }
    *pbyBrdNum = (u8)m_dwBrdCfgEntryNum;
    
    u16 wRet = SUCCESS_AGENT;
	
	for(u8 byIndex = 0; byIndex < m_dwBrdCfgEntryNum; byIndex++)
    {
        if(NULL != &ptBoardTable[byIndex])
        {
            TBoardInfo tBrdInfo;
            GetBrdCfgById( m_atBrdCfgTable[byIndex].GetBrdId(), &tBrdInfo );
            memcpy( &ptBoardTable[byIndex], &tBrdInfo, sizeof(TBoardInfo) );
        }
        else
        {
            wRet = ERR_AGENT_NULLPARAM;
            break;
        }    
    }
    return wRet;
}

/*=============================================================================
函 数 名： GetBrdInfoByRow
功    能： 读单板全信息
算法实现： 
全局变量： 
参    数： u8 byRow
TEqpBrdCfgEntry tBrdCfgEnt
BOOL32 bSnmp: 区分SNMP的调用和普通调用，设置对应的打印级别
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/31  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::GetBrdInfoByRow( u8 byRow, TEqpBrdCfgEntry *ptBrdCfgEnt, BOOL32 bSnmp )
{
    u16 wRet = ERR_AGENT_NULLPARAM;

    if ( NULL == ptBrdCfgEnt || byRow == 0 || byRow > m_dwBrdCfgEntryNum )
    {
        if ( bSnmp )
		{
            Agtlog( LOG_ERROR, "[GetBrdInfoByRow] param err: ptEnt.0x%x, Row.%d, bSnmp.%d!\n", ptBrdCfgEnt, byRow, bSnmp );
		}
        else
		{
            Agtlog( LOG_ERROR, "[GetBrdInfoByRow] param err: ptEnt.0x%x, Row.%d, bSnmp.%d!\n", ptBrdCfgEnt, byRow, bSnmp );
		}
		
        return wRet;
    }

    if ( 0 != m_atBrdCfgTable[byRow-1].GetBrdId() ) 
    {
        memcpy( ptBrdCfgEnt, &m_atBrdCfgTable[byRow-1], sizeof(TEqpBrdCfgEntry) );
        wRet = SUCCESS_AGENT;
    }

    return wRet;
}

/*=============================================================================
函 数 名： SetBrdInfoByRow
功    能： 写入单板全信息
算法实现： 
全局变量： 
参    数： u8 byRow
TEqpBrdCfgEntry *ptBrdCfgEnt
返 回 值： BOOL32 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/31  4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::SetBrdInfoByRow( u8 byRow, TEqpBrdCfgEntry *ptBrdCfgEnt )
{
    u16 wRet = ERR_AGENT_NULLPARAM;
    if ( byRow == 0 || byRow > m_dwBrdCfgEntryNum )
    {
        Agtlog( LOG_ERROR, "[GetBrdInfoByRow] param err: Row.%d !\n", byRow );
        return wRet;
    }
    if ( 0 != m_atBrdCfgTable[byRow-1].GetBrdId() ) 
    {
		if ( m_atBrdCfgTable[byRow-1].GetOSType() != ptBrdCfgEnt->GetOSType())
		{
			LogPrint(LOG_LVL_WARNING,MID_MCU_CFG,"=============[SetBrdInfoByRow] Brd<%d,%d> m_atBrdCfgTable osType:%d ptBrdCfgEnt->GetOSType():%d,unmatched!error!\n",ptBrdCfgEnt->GetLayer(),ptBrdCfgEnt->GetSlot(),m_atBrdCfgTable[byRow-1].GetOSType(),ptBrdCfgEnt->GetOSType());
		}

        memcpy(  &m_atBrdCfgTable[byRow-1], ptBrdCfgEnt, sizeof(TEqpBrdCfgEntry) );
        wRet = SUCCESS_AGENT;
    }
    return wRet;
}

/*=============================================================================
函 数 名： GetBrdInfoById
功    能： 读单板全信息
算法实现： 
全局变量： 
参    数： u8 byId
TEqpBrdCfgEntry tBrdCfgEnt
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/11/2   4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::GetBrdInfoById( u8 byBrdId, TEqpBrdCfgEntry* ptBrdCfgEnt )
{
    u16 wRet = ERR_AGENT_GETNODEVALUE;
    if ( NULL == ptBrdCfgEnt || 0 == byBrdId )
    {
        Agtlog( LOG_ERROR, "[GetBrdInfoById] param err: byId.%d, ptEnt.0x%x !\n", byBrdId, ptBrdCfgEnt );
        return wRet;
    }
    for ( u8 byIndex = 0; byIndex < m_dwBrdCfgEntryNum; byIndex ++ )
    {
        if ( m_atBrdCfgTable[byIndex].GetBrdId() == byBrdId )
        {
            memcpy( ptBrdCfgEnt, &m_atBrdCfgTable[byIndex], sizeof(TEqpBrdCfgEntry) );
            wRet = SUCCESS_AGENT;
        }
    }
    return wRet;
}

/*=============================================================================
函 数 名： SetBrdInfoById
功    能： 写入单板全信息
算法实现： 
全局变量： 
参    数： u8 byId
TEqpBrdCfgEntry *ptBrdCfgEnt
返 回 值： BOOL32 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/11/2   4.0         张宝卿       创建
=============================================================================*/
u16 CCfgAgent::SetBrdInfoById( u8 byBrdId, TEqpBrdCfgEntry *ptBrdCfgEnt )
{
    u16 wRet = ERR_AGENT_GETNODEVALUE;
    if ( 0 == byBrdId )
    {
        Agtlog( LOG_ERROR, "[GetBrdInfoById] param err: byId.%d!\n", byBrdId );
        return wRet;
    }
    for ( u8 byIndex = 0; byIndex < m_dwBrdCfgEntryNum; byIndex ++ )
    {
        if ( m_atBrdCfgTable[byIndex].GetBrdId() == byBrdId )
        {
            memcpy( &m_atBrdCfgTable[byIndex], ptBrdCfgEnt, sizeof(TEqpBrdCfgEntry) );
            wRet = SUCCESS_AGENT;
        }
    }
    return wRet;
}

/*=============================================================================
函 数 名： WriteBrdTable
功    能： 写单板表
算法实现： 
全局变量： 
参    数： u8 byBrdNum
TBoardInfo* ptBoardTable
返 回 值： u16  
=============================================================================*/
u16  CCfgAgent::WriteBrdTable(u8 byBrdNum, TBoardInfo* ptBoardTable )
{
    if( NULL == ptBoardTable && 0 != byBrdNum )
	{
		return ERR_AGENT_NULLPARAM;
	}
    m_dwBrdCfgEntryNum = byBrdNum;
	
    u16 wRet = SUCCESS_AGENT;
    wRet = WriteTableEntryNum( m_achCfgName, (s8*)SECTION_mcueqpBoardConfig, 
		(s8*)ENTRY_NUM, byBrdNum );
    if( SUCCESS_AGENT != wRet )
    {
        printf( "[WriteBrdTable] Fail to write profile in %s\n", SECTION_mcueqpBoardConfig );
        return wRet;
    }
	s8 szBoardSectionInfo[MAX_WRITE_SECTIONONECE_LEN] = {0};
	s8 szRowBoardInfo[MAX_VALUE_LEN + 1 ]={0};	
	u32 dwSectionOffSet = 0;
    for(u8 byIdx = 0; byIdx < byBrdNum && ptBoardTable; byIdx ++ )
    {
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)//zhouyiliang 201203028kg,8kh一条一条写没问题
			wRet = WriteBoardTable( byIdx, &ptBoardTable[byIdx] );

#else
			//zhouyiliang 20120227 先将单板信息存在buffer中，然后一起写	
			memset(szRowBoardInfo,0,sizeof(szRowBoardInfo));
			wRet = OrganizeBoardTable( byIdx, &ptBoardTable[byIdx],szRowBoardInfo );
			if (strlen(szRowBoardInfo) > MAX_VALUE_LEN  )
			{
				szRowBoardInfo[MAX_VALUE_LEN] = '\0';
			}
			//写的单板内容多于2k，分为多块写
			if ( (strlen(szBoardSectionInfo) + strlen(szRowBoardInfo)) > MAX_WRITE_SECTIONONECE_LEN  )
			{
				if ( !SetRegKeySection(m_achCfgName,(s8*)SECTION_mcueqpBoardConfig,szBoardSectionInfo,(u16)dwSectionOffSet,ENTRY_NUM) )
				{
					StaticLog("[WriteBrdTable]SetRegKeySection error!\n");
					wRet = ERR_AGENT_WRITEFILE;
				}
				dwSectionOffSet += strlen(szBoardSectionInfo);
				memset(szBoardSectionInfo,0,sizeof(szBoardSectionInfo));
				
			}		
			sprintf(szBoardSectionInfo,"%s%s",szBoardSectionInfo,szRowBoardInfo);
#endif		

			if(SUCCESS_AGENT != wRet)
			{
				break;
		
			}
    }
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)

	if ( strlen(szBoardSectionInfo) > 0 )
	{
		if ( !SetRegKeySection(m_achCfgName,(s8*)SECTION_mcueqpBoardConfig,szBoardSectionInfo,(u16)dwSectionOffSet,ENTRY_NUM) )
		{
			StaticLog("[WriteBrdTable]SetRegKeySection error!\n");
			wRet = ERR_AGENT_WRITEFILE;
		}
		
	}
#endif
    return wRet;
}

/*=============================================================================
函 数 名： ReadMixerTable
功    能： 读Mixer表
算法实现： 
全局变量： 
参    数： u8* pbyMixNum
TEqpMixerInfo* ptMixerTable
返 回 值： BOOL32 
=============================================================================*/
u16 CCfgAgent::ReadMixerTable(u8* pbyMixNum, TEqpMixerInfo* ptMixerTable)
{
    if( NULL == pbyMixNum || NULL == ptMixerTable )
    {
        return ERR_AGENT_NULLPARAM;
    }
    *pbyMixNum = (u8)m_dwMixEntryNum;
    
	u16 wRet = SUCCESS_AGENT;
    for( u8 byRow = 1; byRow <= *pbyMixNum; byRow ++ )
    {
        wRet = GetEqpMixerCfgByRow( byRow, &ptMixerTable[byRow-1] );
        if ( SUCCESS_AGENT != wRet )    break;
    }
    return wRet;
}

/*=============================================================================
函 数 名： WriteMixerTable
功    能： 写Mixer表
算法实现： 
全局变量： 
参    数： u8 byMixNum
		   TEqpMixerInfo* ptMixerTable
		   BOOL32 bIsNeedUpdatePortInTable 标识是否需要更新内存中的端口信息
返 回 值： u16  
=============================================================================*/
u16  CCfgAgent::WriteMixerTable(u8 byMixNum, TEqpMixerInfo* ptMixerTable,BOOL32 bIsNeedUpdatePortInTable)
{
	if( NULL == ptMixerTable && byMixNum != 0 )
	{
		return ERR_AGENT_NULLPARAM;
	}

	//保存MCU侧分配端口[2/20/2012 chendaiwei]
	u16 awMcuRecvPort[MAX_PRIEQP_NUM] = {0};
	for( u8 byIdx = 0; byIdx < m_dwMixEntryNum; byIdx++ )
	{
		awMcuRecvPort[m_atMixTable[byIdx].GetEqpId()-MIXERID_MIN] = m_atMixTable[byIdx].GetMcuRecvPort();
	}

    m_dwMixEntryNum = (u32)byMixNum;
    
    u16 wRet = SUCCESS_AGENT;
    wRet = WriteTableEntryNum( m_achCfgName, (s8*)SECTION_mcueqpMixerTable, 
		(s8*)ENTRY_NUM, byMixNum );
    if( SUCCESS_AGENT != wRet )
    {
        printf("[WriteMixerTable] Fail to write profile in %s\n", SECTION_mcueqpMixerTable);
        return wRet;
    }

    memset(m_atMixTable, 0, sizeof(m_atMixTable));
	
    for( u8 byRow = 1; byRow <= byMixNum && ptMixerTable; byRow++ )
    {
        wRet = SetEqpMixerCfgByRow( byRow, ptMixerTable[byRow-1],bIsNeedUpdatePortInTable );
		m_atMixTable[byRow-1].SetDefaultEntParam( EQP_TYPE_MIXER, ntohl(GetLocalIp()), 3 );

		//界面保存配置信息，内存外设表中MCU侧端口应该保持不变，避免用户不重启，该外设无法正常使用 [2/20/2012 chendaiwei]
		if( !bIsNeedUpdatePortInTable )
		{
			u16 wMcuPort = awMcuRecvPort[ptMixerTable[byRow-1].GetEqpId() - MIXERID_MIN];
			m_atMixTable[byRow-1].SetMcuRecvPort(wMcuPort);
		}

        if ( SUCCESS_AGENT != wRet )    break;
    }
    return wRet;
}


/*=============================================================================
函 数 名： ReadTvTable
功    能： 读tvwall 表
算法实现： 
全局变量： 
参    数： u8* pbyTvNum
TEqpTVWallInfo* ptTvTable
返 回 值： BOOL32 
=============================================================================*/
u16 CCfgAgent::ReadTvTable(u8* pbyTvNum, TEqpTVWallInfo* ptTvTable)
{
    if(NULL == pbyTvNum || NULL == ptTvTable)
    {
        return ERR_AGENT_NULLPARAM;
    }
    *pbyTvNum = (u8)m_dwTVWallEntryNum;
	
	u16 wRet = SUCCESS_AGENT;
    for( u8 byRow = 1; byRow <= m_dwTVWallEntryNum; byRow ++ )
    {
        wRet = GetEqpTVWallCfgByRow( byRow, &ptTvTable[byRow-1] );
        if ( SUCCESS_AGENT != wRet )    break;
    }
    return wRet;
}

/*=============================================================================
函 数 名： WriteTvTable
功    能： 写TvwalL表
算法实现： 
全局变量： 
参    数： u8 byTvNum
TEqpTVWallInfo* ptTvTable
返 回 值： u16  
=============================================================================*/
u16  CCfgAgent::WriteTvTable(u8 byTvNum, TEqpTVWallInfo* ptTvTable)
{
    if( NULL == ptTvTable  && byTvNum != 0)
	{
		return ERR_AGENT_NULLPARAM;
	}
    m_dwTVWallEntryNum = (u32)byTvNum;
	
    u16 wRet = SUCCESS_AGENT;
    wRet = WriteTableEntryNum(m_achCfgName, (s8*)SECTION_mcueqpTVWallTable, 
		(s8*)ENTRY_NUM, byTvNum );
    if( SUCCESS_AGENT != wRet )
    {
        printf("[WriteTvTable] Fail to write profile in %s\n", SECTION_mcueqpTVWallTable);
        return wRet;
    }
	
    memset( m_atTVWallTable, 0, sizeof(m_atTVWallTable) );
    
    for( u8 byRow = 1; byRow <= byTvNum && ptTvTable; byRow++)
    {
        wRet = SetEqpTVWallCfgByRow( byRow, &ptTvTable[byRow-1] );
        m_atTVWallTable[byRow-1].SetDefaultEntParam( EQP_TYPE_TVWALL, ntohl(GetLocalIp()), 3 );
		
        if ( SUCCESS_AGENT != wRet )    break;
    }
    return wRet;
}

/*=============================================================================
函 数 名： ReadRecTable
功    能： 读Rcorder表
算法实现： 
全局变量： 
参    数： u8* pbyRecNum
TEqpRecEntry* ptRecTable
返 回 值： BOOL32 
=============================================================================*/
u16 CCfgAgent::ReadRecTable(u8* pbyRecNum, TEqpRecInfo* ptRecTable)
{
    if(NULL == pbyRecNum || NULL == ptRecTable)
    {
        return ERR_AGENT_NULLPARAM;
    }
    *pbyRecNum = (u8)m_dwRecEntryNum;
	
	u16 wRet = SUCCESS_AGENT;
    for( u8 byRow = 1; byRow <= m_dwRecEntryNum; byRow ++ )
    {
        wRet = GetEqpRecCfgByRow( byRow, &ptRecTable[byRow-1] );
        if ( SUCCESS_AGENT != wRet )    break;
    }
    return wRet;
}

/*=============================================================================
函 数 名： WriteRecTable
功    能： 写Recorder表
算法实现： 
全局变量： 
参    数： u8 byRecNum
		   TEqpRecEntry* ptRecTable
		   BOOL32 bIsNeedUpdatePortInTable 标识是否需要更新内存中的端口信息
返 回 值： u16  
=============================================================================*/
u16  CCfgAgent::WriteRecTable(u8 byRecNum, TEqpRecInfo* ptRecTable, BOOL32 bIsNeedUpdatePortInTable)
{
	if( NULL == ptRecTable && byRecNum != 0)
	{
		return ERR_AGENT_NULLPARAM;
	}

	//保存MCU侧分配端口[2/20/2012 chendaiwei]
	u16 awMcuRecvPort[MAX_PRIEQP_NUM] = {0};
	for( u8 byIdx = 0; byIdx < m_dwRecEntryNum; byIdx++ )
	{
		awMcuRecvPort[m_atRecTable[byIdx].GetEqpId()-RECORDERID_MIN] = m_atRecTable[byIdx].GetMcuRecvPort();
	}

    m_dwRecEntryNum = (u32)byRecNum;
    
    u16 wRet = SUCCESS_AGENT;
    wRet = WriteTableEntryNum(m_achCfgName, (s8*)SECTION_mcueqpRecorderTable,
		(s8*)ENTRY_NUM, byRecNum );
    if( SUCCESS_AGENT != wRet )
    {
        printf("[WriteRecTable] Fail to write profile in %s\n", SECTION_mcueqpRecorderTable);
        return wRet;
    }
    
    memset(m_atRecTable, 0, sizeof(m_atRecTable));
	
    for( u8 byRow = 1; byRow <= byRecNum && ptRecTable; byRow++ )
    {
        wRet = SetEqpRecCfgByRow( byRow, &ptRecTable[byRow-1],bIsNeedUpdatePortInTable );
        m_atRecTable[byRow-1].SetDefaultEntParam( EQP_TYPE_RECORDER, ntohl(GetLocalIp()), 3 );
		
		//界面保存配置信息，内存外设表中MCU侧端口应该保持不变，避免用户不重启，该外设无法正常使用 [2/20/2012 chendaiwei]
		if( !bIsNeedUpdatePortInTable )
		{
			u16 wMcuPort = awMcuRecvPort[ptRecTable[byRow-1].GetEqpId() - RECORDERID_MIN];
			m_atRecTable[byRow-1].SetMcuRecvPort(wMcuPort);
		}
		
        if ( SUCCESS_AGENT != wRet )    break;
    }
    return wRet;
}

/*=============================================================================
函 数 名： ReadVrsRecTable
功    能： 读Vrs新录播表
算法实现： 
全局变量： 
参    数： u8* byVrsRecNum
		   TEqpVrsRecCfgInfo* ptVrsRecTable
返 回 值： u16 
=============================================================================*/
u16 CCfgAgent::ReadVrsRecTable( u8* pbyVrsRecNum, TEqpVrsRecCfgInfo* ptVrsRecTable )
{
    if(NULL == pbyVrsRecNum || NULL == ptVrsRecTable)
    {
        return ERR_AGENT_NULLPARAM;
    }
    *pbyVrsRecNum = (u8)m_dwVrsRecEntryNum;
	
	u16 wRet = SUCCESS_AGENT;
    for( u8 byRow = 1; byRow <= m_dwVrsRecEntryNum; byRow ++ )
    {
        wRet = GetEqpVrsRecCfgByRow( byRow, &ptVrsRecTable[byRow-1] );
        if ( SUCCESS_AGENT != wRet )    break;
    }
    return wRet;
}

/*=============================================================================
函 数 名： WriteVrsRecTable
功    能： 写Vrs新录播表
算法实现： 
全局变量： 
参    数： u8 byVrsRecNum
		   TEqpVrsRecCfgInfo* ptVrsRecTable
返 回 值： u16  
=============================================================================*/
u16  CCfgAgent::WriteVrsRecTable(u8 byVrsRecNum, TEqpVrsRecCfgInfo* ptVrsRecTable)
{
	if( NULL == ptVrsRecTable && byVrsRecNum != 0)
	{
		return ERR_AGENT_NULLPARAM;
	}

    m_dwVrsRecEntryNum = (u32)byVrsRecNum;
    
    u16 wRet = SUCCESS_AGENT;
    wRet = WriteTableEntryNum(m_achCfgName, (s8*)SECTION_mcueqpVrsRecorderTable,
		(s8*)ENTRY_NUM, byVrsRecNum );
    if( SUCCESS_AGENT != wRet )
    {
        printf("[WriteVrsRecTable] Fail to write profile in %s\n", SECTION_mcueqpVrsRecorderTable);
        return wRet;
    }
    
    memset(m_atVrsRecTable, 0, sizeof(m_atVrsRecTable));
	
    for( u8 byRow = 1; byRow <= byVrsRecNum && ptVrsRecTable; byRow++ )
    {
        wRet = SetEqpVrsRecCfgByRow( byRow, &ptVrsRecTable[byRow-1] );
		
        if ( SUCCESS_AGENT != wRet )    break;
    }
    return wRet;
}

/*=============================================================================
函 数 名： ReadBasTable
功    能： 读Bas表
算法实现： 
全局变量： 
参    数： u8* pbyBasNum
TEqpBasInfo* ptBasTable
返 回 值： BOOL32 
=============================================================================*/
u16 CCfgAgent::ReadBasTable(u8* pbyBasNum, TEqpBasInfo* ptBasTable)
{
    if(NULL == pbyBasNum || NULL == ptBasTable)
    {
        return ERR_AGENT_NULLPARAM;
    }
    
	u16 wRet = SUCCESS_AGENT;
    *pbyBasNum = (u8)m_dwBasEntryNum;
    for( u8 byRow = 1; byRow <= *pbyBasNum; byRow ++ )
    {
        wRet = GetEqpBasCfgByRow( byRow, &ptBasTable[byRow-1] );
        if ( SUCCESS_AGENT != wRet )    break;
    }
    return wRet;
}

/*=============================================================================
函 数 名： ReadBasHDTable
功    能： 读BasHD表
算法实现： 
全局变量： 
参    数： u8* pbyBasHDNum
TEqpBasHDInfo* ptBasHDTable
返 回 值： BOOL32 
=============================================================================*/
u16 CCfgAgent::ReadBasHDTable(u8* pbyBasHDNum, TEqpBasHDInfo* ptBasHDTable)
{
    if(NULL == pbyBasHDNum || NULL == ptBasHDTable)
    {
        return ERR_AGENT_NULLPARAM;
    }
    
	u16 wRet = SUCCESS_AGENT;
    *pbyBasHDNum = (u8)m_dwBasHDEntryNum;
    for( u8 byRow = 1; byRow <= *pbyBasHDNum; byRow ++ )
    {
        wRet = GetEqpBasHDCfgByRow( byRow, &ptBasHDTable[byRow-1] );
        if ( SUCCESS_AGENT != wRet )    break;
    }
    return wRet;
}

/*=============================================================================
函 数 名： WriteBasTable
功    能： 写Bas表
算法实现： 
全局变量： 
参    数： u8 byBasNum
		   TEqpBasInfo* ptBasTable
		   BOOL32 bIsNeedUpdatePortInTable 标识是否需要更新内存中的端口信息
返 回 值： u16  
=============================================================================*/
u16  CCfgAgent::WriteBasTable(u8 byBasNum, TEqpBasInfo* ptBasTable, BOOL32 bIsNeedUpdatePortInTable)
{
    if( NULL == ptBasTable && byBasNum != 0 )
	{
		return ERR_AGENT_NULLPARAM;
	}

	//保存MCU侧分配端口[2/20/2012 chendaiwei]
	u16 awMcuRecvPort[MAX_PRIEQP_NUM] = {0};
	for( u8 byIdx = 0; byIdx < m_dwBasEntryNum; byIdx++ )
	{
		awMcuRecvPort[m_atBasTable[byIdx].GetEqpId()-BASID_MIN] = m_atBasTable[byIdx].GetMcuRecvPort();
	}

    m_dwBasEntryNum = byBasNum;
    u16 wRet = SUCCESS_AGENT;
    wRet = WriteTableEntryNum( m_achCfgName, (s8*)SECTION_mcueqpBasTable, 
		(s8*)ENTRY_NUM, byBasNum );
    if( SUCCESS_AGENT != wRet )
    {
        printf("[WriteBasTable] Fail to write profile in %s\n", SECTION_mcueqpBasTable);
        return wRet;
    }
	
    memset( m_atBasTable, 0, sizeof(m_atBasTable) );
    
    for( u8 byRow = 1; byRow <= byBasNum && ptBasTable; byRow++)
    {
        wRet = SetEqpBasCfgByRow( byRow, &ptBasTable[byRow-1],bIsNeedUpdatePortInTable );
        m_atBasTable[byRow-1].SetDefaultEntParam( EQP_TYPE_BAS, ntohl(GetLocalIp()), 3 );
		
		//界面保存配置信息，内存外设表中MCU侧端口应该保持不变，避免用户不重启，该外设无法正常使用 [2/20/2012 chendaiwei]
		if( !bIsNeedUpdatePortInTable )
		{
			u16 wMcuPort = awMcuRecvPort[ptBasTable[byRow-1].GetEqpId() - BASID_MIN];
			m_atBasTable[byRow-1].SetMcuRecvPort(wMcuPort);
		}

        if ( SUCCESS_AGENT != wRet )    break;
    }
    return wRet;
}

/*=============================================================================
函 数 名： WriteBasHDTable
功    能： 写BasHD表
算法实现： 
全局变量： 
参    数： u8 byBasHDNum
TEqpBasHDInfo* ptBasHDTable
		   BOOL32 bIsNeedUpdatePortInTable 标识是否需要更新内存中的端口信息
返 回 值： u16  
=============================================================================*/
u16  CCfgAgent::WriteBasHDTable(u8 byBasHDNum, TEqpBasHDInfo* ptBasHDTable, BOOL32 bIsNeedUpdatePortInTable)
{
    if( NULL == ptBasHDTable && byBasHDNum != 0 )
	{
		return ERR_AGENT_NULLPARAM;
	}

	//保存MCU侧分配端口[2/20/2012 chendaiwei]
	u16 awMcuRecvPort[MAX_PRIEQP_NUM] = {0};
	for( u8 byIdx = 0; byIdx < m_dwBasHDEntryNum; byIdx++ )
	{
		awMcuRecvPort[m_atBasHDTable[byIdx].GetEqpId()-BASID_MIN] = m_atBasHDTable[byIdx].GetMcuRecvPort();
	}

    m_dwBasHDEntryNum = byBasHDNum;
    u16 wRet = SUCCESS_AGENT;
    wRet = WriteTableEntryNum( m_achCfgName, (s8*)SECTION_mcueqpBasHDTable, 
		(s8*)ENTRY_NUM, byBasHDNum );
    if( SUCCESS_AGENT != wRet )
    {
        printf("[WriteBasHDTable] Fail to write profile in %s\n", SECTION_mcueqpBasHDTable);
        return wRet;
    }
	
    memset( m_atBasHDTable, 0, sizeof(m_atBasHDTable) );
    
    for( u8 byRow = 1; byRow <= byBasHDNum && ptBasHDTable; byRow++)
    {
        wRet = SetEqpBasHDCfgByRow( byRow, &ptBasHDTable[byRow-1],bIsNeedUpdatePortInTable );
        m_atBasHDTable[byRow-1].SetDefaultEntParam( EQP_TYPE_BAS, ntohl(GetLocalIp()), 3 );
		
		//界面保存配置信息，内存外设表中MCU侧端口应该保持不变，避免用户不重启，该外设无法正常使用 [2/20/2012 chendaiwei]
		if( !bIsNeedUpdatePortInTable )
		{
			u16 wMcuPort = awMcuRecvPort[ptBasHDTable[byRow-1].GetEqpId() - BASID_MIN];
			m_atBasHDTable[byRow-1].SetMcuRecvPort(wMcuPort);
		}

        if ( SUCCESS_AGENT != wRet )    
            break;
    }
    return wRet;
}

/*=============================================================================
函 数 名： ReadVmpTable
功    能： 读vmp 表
算法实现： 
全局变量： 
参    数：  u8* pbyVmpNum
TEqpVMPInfo* ptVmpTable
返 回 值： BOOL32 
=============================================================================*/
u16 CCfgAgent::ReadVmpTable( u8* pbyVmpNum, TEqpVMPInfo* ptVmpTable)
{
    if(NULL == pbyVmpNum || NULL == ptVmpTable )
    {
        return ERR_AGENT_NULLPARAM;
    }
    *pbyVmpNum = (u8)m_dwVMPEntryNum;
	
	u16 wRet = SUCCESS_AGENT;
    for( u8 byRow = 1; byRow <= *pbyVmpNum; byRow ++ )
    {
        wRet = GetEqpVMPCfgByRow( byRow, &ptVmpTable[byRow-1] );
        if ( SUCCESS_AGENT != wRet )    break;
    }
    return wRet;
}

/*=============================================================================
函 数 名： WriteVmpTable
功    能： 写Vmp表
算法实现： 
全局变量： 
参    数： u8 byVmpNum
TEqpVMPInfo* ptVmpTable
BOOL32 bIsNeedUpdatePortInTable 标识是否需要更新内存中的端口信息
返 回 值： u16  
=============================================================================*/
u16  CCfgAgent::WriteVmpTable(u8 byVmpNum, TEqpVMPInfo* ptVmpTable,BOOL32 bIsNeedUpdatePortInTable)
{
    if( NULL == ptVmpTable && byVmpNum != 0)
	{
		return ERR_AGENT_NULLPARAM;
	}

	//保存MCU侧分配端口[2/20/2012 chendaiwei]
	u16 awMcuRecvPort[MAX_PRIEQP_NUM] = {0};
	for( u8 byIdx = 0; byIdx < m_dwVMPEntryNum; byIdx++ )
	{
		awMcuRecvPort[m_atVMPTable[byIdx].GetEqpId()-VMPID_MIN] = m_atVMPTable[byIdx].GetMcuRecvPort();
	}

    m_dwVMPEntryNum = (u32)byVmpNum;
	
    u16 wRet = SUCCESS_AGENT;
    wRet = WriteTableEntryNum( m_achCfgName, (s8*)SECTION_mcueqpVMPTable, 
		(s8*)ENTRY_NUM, byVmpNum );
    if( SUCCESS_AGENT != wRet )
    {
        printf("[WriteVmpTable] Fail to write profile in %s\n", SECTION_mcueqpVMPTable);
        return wRet;
    }
	
    memset(m_atVMPTable, 0, sizeof(m_atVMPTable));
    
    for( u8 byRow = 1; byRow <= byVmpNum && ptVmpTable; byRow++)
    {
        wRet = SetEqpVMPCfgByRow( byRow, &ptVmpTable[byRow-1],bIsNeedUpdatePortInTable );
        m_atVMPTable[byRow-1].SetDefaultEntParam( EQP_TYPE_VMP, ntohl(GetLocalIp()), 3 );
		
		//界面保存配置信息，内存外设表中MCU侧端口应该保持不变，避免用户不重启，该外设无法正常使用 [2/20/2012 chendaiwei]
		if( !bIsNeedUpdatePortInTable )
		{
			u16 wMcuPort = awMcuRecvPort[ptVmpTable[byRow-1].GetEqpId() - VMPID_MIN];
			m_atVMPTable[byRow-1].SetMcuRecvPort(wMcuPort);
		}

        if ( SUCCESS_AGENT != wRet )    break;
    }
    return wRet;
}

/*=============================================================================
函 数 名： ReadMpwTable
功    能： 取Mpw表信息
算法实现： 
全局变量： 
参    数： u8* pbyMpwNum
TEqpMpwInfo* ptMpwTable
返 回 值： BOOL32 
=============================================================================*/
u16 CCfgAgent::ReadMpwTable(u8* pbyMpwNum, TEqpMpwInfo* ptMpwTable)
{
    if(NULL == ptMpwTable || NULL == pbyMpwNum )
    {
        return ERR_AGENT_NULLPARAM;
    }
    *pbyMpwNum = (u8)m_dwMpwEntryNum;
	
	u16 wRet = SUCCESS_AGENT;
    for( u8 byRow = 1; byRow <= *pbyMpwNum; byRow ++ )
    {
        wRet = GetEqpMpwCfgByRow( byRow, &ptMpwTable[byRow-1] );
        if ( SUCCESS_AGENT != wRet )    break;
    }
    return wRet;
}

/*=============================================================================
函 数 名： WriteMpwTable
功    能： 写Mpw表
算法实现： 
全局变量： 
参    数： u8 byMpwNum
TEqpMpwInfo* ptMpwTable
返 回 值： u16 
=============================================================================*/
u16 CCfgAgent::WriteMpwTable(u8 byMpwNum, TEqpMpwInfo* ptMpwTable)
{
    if( NULL == ptMpwTable && byMpwNum != 0)
	{
		return ERR_AGENT_NULLPARAM;
	}
    m_dwMpwEntryNum = (u32)byMpwNum;
	
    u16 wRet = SUCCESS_AGENT;
    wRet = WriteTableEntryNum( m_achCfgName, (s8*)SECTION_mcueqpMpwTable, 
		(s8*)ENTRY_NUM, byMpwNum );
    if( SUCCESS_AGENT != wRet )
    {
        printf("[WriteMpwTable] Fail to write profile in %s\n", SECTION_mcueqpMpwTable);
        return wRet;
    }
	
    memset(m_atMpwTable, 0, sizeof(m_atMpwTable));
	
    for( u8 byRow = 1; byRow <= byMpwNum && ptMpwTable; byRow++)
    {
        wRet = SetEqpMpwCfgByRow( byRow, &ptMpwTable[byRow-1] );
        m_atMpwTable[byRow-1].SetDefaultEntParam( EQP_TYPE_VMPTW, ntohl(GetLocalIp()), 3 );
		
        if ( SUCCESS_AGENT != wRet )    break;
    }
    return wRet;
}


/*=============================================================================
函 数 名： ReadPrsTable
功    能： 读表
算法实现： 
全局变量： 
参    数： u8* pbyPrsNum
TEqpPrsInfo* ptPrsTable
返 回 值： u16 
=============================================================================*/
u16 CCfgAgent::ReadPrsTable(u8* pbyPrsNum, TEqpPrsInfo* ptPrsTable)
{
    if(NULL == pbyPrsNum || NULL == ptPrsTable)
    {
        return ERR_AGENT_NULLPARAM;
    }
    *pbyPrsNum = (u8)m_dwPrsEntryNum;
    
	u16 wRet = SUCCESS_AGENT;
    for( u8 byRow = 1; byRow <= *pbyPrsNum; byRow ++ )
    {
        wRet = GetEqpPrsCfgByRow( byRow, &ptPrsTable[byRow-1] );
        if ( SUCCESS_AGENT != wRet )    break;
    }
    return wRet;
}

/*=============================================================================
函 数 名： WritePrsTable
功    能： 写表
算法实现： 
全局变量： 
参    数： u8 byPrsNum
TEqpPrsInfo* ptPrsTable
返 回 值： u16 
=============================================================================*/
u16 CCfgAgent::WritePrsTable(u8 byPrsNum, TEqpPrsInfo* ptPrsTable)
{
    if( NULL == ptPrsTable && byPrsNum != 0)
    {
        return ERR_AGENT_NULLPARAM;
    }
    m_dwPrsEntryNum = (u32)byPrsNum;
	
    u16 wRet = SUCCESS_AGENT;
    wRet = WriteTableEntryNum( m_achCfgName, (s8*)SECTION_mcueqpPrsTable, 
		(s8*)ENTRY_NUM, byPrsNum );
    if( SUCCESS_AGENT != wRet )
    {
        printf( "[WritePrsTable] Fail to write profile in %s\n", SECTION_mcueqpPrsTable);
        return wRet;
    }
	
    memset(m_atPrsTable, 0, sizeof(m_atPrsTable));
	
    for( u8 byRow = 1; byRow <= byPrsNum && ptPrsTable; byRow++)
    {
        wRet = SetEqpPrsCfgByRow( byRow, &ptPrsTable[byRow-1] );
        m_atPrsTable[byRow-1].SetDefaultEntParam( EQP_TYPE_PRS, ntohl(GetLocalIp()), 3 );
		
        if ( SUCCESS_AGENT != wRet )    break;
    }
    return wRet;
}

/*=============================================================================
函 数 名： IsSVmp
功    能： 
算法实现： 
全局变量： 
参    数： u8 byPrsNum
TEqpPrsInfo* ptPrsTable
返 回 值： u16 
=============================================================================*/
BOOL32 CCfgAgent::IsSVmp( u8 byEqpId )
{
    for ( int byLoop = 0; byLoop < MAX_PRIEQP_NUM; byLoop++ )
    {
        if ( byEqpId == m_atSvmpTable[byLoop].GetEqpId() )
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*=============================================================================
函 数 名： GetVmpEqpVersion
功    能： 获取当前VMP的硬件版本
算法实现： 
全局变量： 
参    数： u8 byEqpId
返 回 值： u8 
------------------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
=============================================================================*/
u16 CCfgAgent::GetVmpEqpVersion(u8 byEqpId)
{
	u16 wVersion = 0;
	
	for ( int byLoop = 0; byLoop < MAX_PRIEQP_NUM; byLoop++ )
	{
		if ( byEqpId == m_atSvmpTable[byLoop].GetEqpId() )
		{
			switch( m_atSvmpTable[byLoop].GetVmpType() )
			{
			case TYPE_MPUSVMP:
				wVersion =	DEVVER_MPU;
				break;
			case TYPE_MPU2VMP_BASIC:
			case TYPE_MPU2VMP_ENHANCED:
				wVersion =	DEVVER_MPU2;
				break;
			default:
				break;
			}
		}
	}

	return wVersion;
}

/*=============================================================================
函 数 名： IsSVmp
功    能： 
算法实现： 
全局变量： 
参    数： u8 byPrsNum
TEqpPrsInfo* ptPrsTable
返 回 值： u16 
=============================================================================*/
// BOOL32 CCfgAgent::IsDVmp( u8 byEqpId )
// {
//     for ( int byLoop = 0; byLoop < MAX_PRIEQP_NUM; byLoop++ )
//     {
//         if ( byEqpId == m_atDvmpBasicTable[byLoop].GetEqpId() )
//         {
//             return TRUE;
//         }
//     }
//     return FALSE;
// }

/*=============================================================================
函 数 名： IsSVmp
功    能： 
算法实现： 
全局变量： 
参    数： u8 byPrsNum
TEqpPrsInfo* ptPrsTable
返 回 值： u16 
=============================================================================*/
// BOOL32 CCfgAgent::IsEVpu( u8 byEqpId )
// {
//     for ( int byLoop = 0; byLoop < MAX_PRIEQP_NUM; byLoop++ )
//     {
//         if ( byEqpId == m_atEvpuTable[byLoop].GetEqpId() )
//         {
//             return TRUE;
//         }
//     }
//     return FALSE;
// }

/*=============================================================================
函 数 名： IsSVmp
功    能： 
算法实现： 
全局变量： 
参    数： u8 byPrsNum
TEqpPrsInfo* ptPrsTable
返 回 值： u16 
=============================================================================*/
BOOL32 CCfgAgent::IsMpuBas( u8 byEqpId )
{
    for ( int byLoop = 0; byLoop < MAX_PRIEQP_NUM; byLoop++ )
    {
        if ( byEqpId == m_atMpuBasTable[byLoop].GetEqpId() )
        {
            return TRUE;
        }
    }
    return FALSE;
}


/*=============================================================================
函 数 名： GetBasEqpVersion
功    能： 获取当前Bas的硬件版本
算法实现： 
全局变量： 
参    数： u8 byEqpId
返 回 值： u8 
------------------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
=============================================================================*/
u16 CCfgAgent::GetBasEqpVersion(u8 byEqpId)
{
	u16 wVersion = 0;
	
	for( int byLoop = 0; byLoop < MAX_PRIEQP_NUM; byLoop++ )
	{
		if ( byEqpId == m_atMpuBasTable[byLoop].GetEqpId() )
		{
			switch( m_atMpuBasTable[byLoop].GetStartMode() )
			{
			case TYPE_MPUBAS:
			case TYPE_MPUBAS_H:
				wVersion =	DEVVER_MPU;
				break;
			case TYPE_MPU2BAS_ENHANCED:
			case TYPE_MPU2BAS_BASIC:
				wVersion =	DEVVER_MPU2;
				break;
			case TYPE_APU2BAS:
				wVersion = DEVVER_APU2;
				break;
			default:
				break;
			}
		

		}
	}
	return wVersion;
}

/*=============================================================================
函 数 名： GetHDUEqpVersion
功    能： 获取当前HDU的硬件版本
算法实现： 
全局变量： 
参    数： u8 byEqpId
返 回 值： u8 
------------------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/02/20  4.7         chendaiei     创建
=============================================================================*/
u16 CCfgAgent::GetHDUEqpVersion(u8 byEqpId)
{
	u16 wVersion = 0;
	
	for( int byLoop = 0; byLoop < MAX_PERIHDU_NUM; byLoop++ )
	{
		if ( byEqpId == m_atHduTable[byLoop].GetEqpId() )
		{
			switch( m_atHduTable[byLoop].GetStartMode() )
			{
			case STARTMODE_HDU_M://HDU 
			case STARTMODE_HDU_H:
			case STARTMODE_HDU_L:
				wVersion = DEVVER_HDU;
				break;
			case STARTMODE_HDU2:
				wVersion = DEVVER_HDU2;
				break;
			case STARTMODE_HDU2_L:
				wVersion = DEVVER_HDU2_L;
				break;
			case STARTMODE_HDU2_S:
				wVersion = DEVVER_HDU2_S;
				break;
			default:
				break;
			}
		}
	}
	return wVersion;
}

/*=============================================================================
函 数 名： GetMpuBasWorkMode
功    能： 
算法实现： 
全局变量： 
参    数： u8 byEqpId, u8 &byWorkMode
返 回 值： true or false 
=============================================================================*/
BOOL32 CCfgAgent::GetMpuBasEntry( u8 byEqpId, TEqpMpuBasEntry &tMpuBasEntry)
{
	memset(&tMpuBasEntry, 0, sizeof(TEqpMpuBasEntry));
	for ( int byLoop = 0; byLoop < MAX_PRIEQP_NUM; byLoop++ )
    {
        if ( byEqpId == m_atMpuBasTable[byLoop].GetEqpId())
        {
			tMpuBasEntry = m_atMpuBasTable[byLoop];
            return TRUE;
        }
    }
    return FALSE;
}

/*=============================================================================
函 数 名： GetIS22BrdIndexByPos
功    能： 根据IS2.2层槽号获取特定的IS2.2单板序号
算法实现： 
全局变量： 
参    数： u8 byLayer 层号 u8 bySlot槽号
返 回 值： u8 单板序号  
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
11/09/15    4.0         chendaiwei       创建
====================================================================*/
u8 CCfgAgent::GetIS22BrdIndexByPos(u8 byLayer, u8 bySlot)
{
	u8 byBrdIdx = 0;
	if(bySlot == 7)
	{
		if(byLayer == 0)
		{
			byBrdIdx = 93;
		}
		else if (byLayer == 1)
		{
			byBrdIdx = 94;
		}
		else if( byLayer == 2)
		{
			byBrdIdx = 95;
		}
		else if( byLayer == 3)
		{
			byBrdIdx = 96;
		}
	}
	
	return byBrdIdx;
}

/*=============================================================================
函 数 名： GetIdlePRSEqpId
功    能： 获取当前最小空闲可用的PRS外设ID
算法实现： 
全局变量： 
参    数： 
返 回 值： u8 Prs Eqp Id 
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
11/09/15    4.0         chendaiwei       创建
====================================================================*/
u8 CCfgAgent::GetIdlePRSEqpId()
{	
	// 最大单板数是否要做调整+4 [9/15/2011 chendaiwei]
	
	u8 abyIndex[MAX_PRIEQP_NUM];
	u8 byIndex = 0;
	u8 byInerIdx = 0;
	u8 byTempPRSId = 0;
	u8 byFlag = 0;
	
	// 当前没有PRS记录，直接返回最小PRS ID [9/15/2011 chendaiwei]
	if( m_dwPrsEntryNum == 0 )
	{
		return PRSID_MIN;
	}
	
	// 初始化排序数组 [9/15/2011 chendaiwei]
	for(byIndex = 0; byIndex < MAX_PRIEQP_NUM; byIndex ++)
	{
		abyIndex[byIndex] = m_atPrsTable[byIndex].GetEqpId();
	}
	
	// 现有PRS Eqp ID排序 [9/15/2011 chendaiwei]
	for( byIndex = 0; byIndex < m_dwPrsEntryNum -1  ; byIndex++)
    {
		byFlag = 1;
		for(byInerIdx = 0; byInerIdx < m_dwPrsEntryNum - byIndex -1;byInerIdx++)
		{
			if(abyIndex[byInerIdx] >abyIndex[byInerIdx+1])
			{
				byTempPRSId =abyIndex[byInerIdx];
				abyIndex[byInerIdx] = abyIndex[byInerIdx+1];
				abyIndex[byInerIdx+1] = byTempPRSId;
				
				byFlag = 0;
			}
		}
		
		if(byFlag == 1)
		{
			break;
		}
	}
	
	// 获取当前最小空闲可用的PRS外设ID[9/15/2011 chendaiwei]
	u8 byIdlePRSId = PRSID_MIN;
	for(byIndex = 0; byIndex < m_dwPrsEntryNum; byIndex++)
	{
		if( byIdlePRSId == abyIndex[byIndex])
		{
			byIdlePRSId ++;
		}
		else if( abyIndex[byIndex] != 0 )
		{
			break;
		}
	}
	
	if(byIdlePRSId > PRSID_MAX)
	{
		OspPrintf(TRUE,FALSE,"[CCfgAgent::GetIdlePRSEqpId] no idle PRS EQP id can be userd!\n");
		
		byIdlePRSId = 0;
	}
	
	return byIdlePRSId;
}

/*=============================================================================
函 数 名： IsSVmp
功    能： 
算法实现： 
全局变量： 
参    数： u8 byPrsNum
TEqpPrsInfo* ptPrsTable
返 回 值： u16 
=============================================================================*/
// BOOL32 CCfgAgent::IsEBap( u8 byEqpId )
// {
//     for ( int byLoop = 0; byLoop < MAX_PRIEQP_NUM; byLoop++ )
//     {
//         if ( byEqpId == m_atEbapTable[byLoop].GetEqpId() )
//         {
//             return TRUE;
//         }
//     }
//     return FALSE;
// }

//4.6版本 新加外设   jlb
/*=============================================================================
函 数 名： ReadHduTable
功    能： 读表
算法实现： 
全局变量： 
参    数： u8* pbyHduNum
TEqpHduInfo* ptHduTable
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
u16    CCfgAgent::ReadHduTable( u8* pbyHduNum, TEqpHduInfo* ptHduTable )
{
    if(NULL == pbyHduNum || NULL == ptHduTable)
    {
        return ERR_AGENT_NULLPARAM;
    }
    *pbyHduNum = (u8)m_dwHduEntryNum;
	
	u16 wRet = SUCCESS_AGENT;
    for( u8 byRow = 1; byRow <= m_dwHduEntryNum; byRow ++ )
    {
        wRet = GetEqpHduCfgByRow( byRow, &ptHduTable[byRow-1] );
        if ( SUCCESS_AGENT != wRet )    break;
    }
    return wRet;
}

/*=============================================================================
函 数 名： WriteHduTable
功    能： 写表
算法实现： 
全局变量： 
参    数： u8 byHduNum
TEqpHduInfo* ptHduTable
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
u16    CCfgAgent::WriteHduTable( u8 byHduNum,  TEqpHduInfo* ptHduTable )
{
    if( NULL == ptHduTable  && byHduNum != 0)
	{
		return ERR_AGENT_NULLPARAM;
	}
    m_dwHduEntryNum = (u32)byHduNum;
	
    u16 wRet = SUCCESS_AGENT;
    wRet = WriteTableEntryNum(m_achCfgName, (s8*)SECTION_mcueqpHduTable, 
		(s8*)ENTRY_NUM, byHduNum );
    if( SUCCESS_AGENT != wRet )
    {
        printf("[WriteHduTable] Fail to write profile in %s\n", SECTION_mcueqpHduTable);
        return wRet;
    }
	
    memset( m_atHduTable, 0, sizeof(m_atHduTable) );
    
    for( u8 byRow = 1; byRow <= byHduNum&&ptHduTable; byRow++)
    {
        wRet = SetEqpHduCfgByRow( byRow, &ptHduTable[byRow-1] );

		if(STARTMODE_HDU_H == ptHduTable[byRow-1].GetStartMode() ||
		   STARTMODE_HDU_M == ptHduTable[byRow-1].GetStartMode() ||
		   STARTMODE_HDU_L == ptHduTable[byRow-1].GetStartMode() ||
		   STARTMODE_HDU2 == ptHduTable[byRow-1].GetStartMode()  ||
		   STARTMODE_HDU2_L == ptHduTable[byRow-1].GetStartMode()  ||
		   STARTMODE_HDU2_S == ptHduTable[byRow-1].GetStartMode())
		{
			m_atHduTable[byRow-1].SetDefaultEntParam( EQP_TYPE_HDU, ntohl(GetLocalIp()), 3 );
		}
		else
		{
			printf("[WriteHduTable]Error: unexpected StartMode<Id:%d, Mode:%d>!\n", byRow-1,  ptHduTable[byRow-1].GetStartMode());
		}

		/*if (STARTMODE_HDU_H == ptHduTable[byRow-1].GetStartMode())
		{
			m_atHduTable[byRow-1].SetDefaultEntParam( EQP_TYPE_HDU_H, ntohl(GetLocalIp()), 3 );
		}
		else if(STARTMODE_HDU_M == ptHduTable[byRow-1].GetStartMode())
		{
			m_atHduTable[byRow-1].SetDefaultEntParam( EQP_TYPE_HDU, ntohl(GetLocalIp()), 3 );
		}
		else if(STARTMODE_HDU_L == ptHduTable[byRow-1].GetStartMode())
		{
			m_atHduTable[byRow-1].SetDefaultEntParam( EQP_TYPE_HDU_L, ntohl(GetLocalIp()), 3 );
		}
		else if(STARTMODE_HDU2 == ptHduTable[byRow-1].GetStartMode())
		{
			m_atHduTable[byRow-1].SetDefaultEntParam( EQP_TYPE_HDU2, ntohl(GetLocalIp()), 3 );
		}
		else if(STARTMODE_HDU2_L == ptHduTable[byRow-1].GetStartMode())
		{
			m_atHduTable[byRow-1].SetDefaultEntParam( EQP_TYPE_HDU2_L, ntohl(GetLocalIp()), 3 );
		}
		else
		{
			printf("[WriteHduTable]Error: unexpected StartMode<Id:%d, Mode:%d>!\n", byRow-1,  ptHduTable[byRow-1].GetStartMode());
		}*/

        if ( SUCCESS_AGENT != wRet )    break;
    }
    return wRet;
}

/*=============================================================================
函 数 名： ReadSvmpTable
功    能： 读表
算法实现： 
全局变量： 
参    数： u8* pbySvmpNum
TEqpSvmpInfo* ptSvmpTable
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
u16    CCfgAgent::ReadSvmpTable( u8* pbySvmpNum, TEqpSvmpInfo* ptSvmpTable )
{
    if(NULL == pbySvmpNum || NULL == ptSvmpTable )
    {
        return ERR_AGENT_NULLPARAM;
    }
    *pbySvmpNum = (u8)m_dwSvmpEntryNum;
	
	u16 wRet = SUCCESS_AGENT;
    for( u8 byRow = 1; byRow <= *pbySvmpNum; byRow ++ )
    {
        wRet = GetEqpSvmpCfgByRow( byRow, &ptSvmpTable[byRow-1] );
        if ( SUCCESS_AGENT != wRet )    break;
    }
    return wRet;
}

/*=============================================================================
函 数 名： WriteSvmpTable
功    能： 写表
算法实现： 
全局变量： 
参    数： u8 bySvmpNum
TEqpSvmpInfo* ptSvmpTable
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
u16    CCfgAgent::WriteSvmpTable( u8 bySvmpNum,  TEqpSvmpInfo* ptSvmpTable,BOOL32 bIsNeedUpdatePortInTable)
{
    if( NULL == ptSvmpTable && bySvmpNum != 0)
	{
		return ERR_AGENT_NULLPARAM;
	}

	//保存MCU侧分配端口[2/20/2012 chendaiwei]
	u16 awMcuRecvPort[MAX_PRIEQP_NUM] = {0};
	for( u8 byIdx = 0; byIdx < m_dwSvmpEntryNum; byIdx++ )
	{
		awMcuRecvPort[m_atSvmpTable[byIdx].GetEqpId()-VMPID_MIN] = m_atSvmpTable[byIdx].GetMcuRecvPort();
	}

    m_dwSvmpEntryNum = (u32)bySvmpNum;
	
    u16 wRet = SUCCESS_AGENT;
    wRet = WriteTableEntryNum( m_achCfgName, (s8*)SECTION_mcueqpSvmpTable, 
		(s8*)ENTRY_NUM, bySvmpNum );
    if( SUCCESS_AGENT != wRet )
    {
        printf("[WriteSvmpTable] Fail to write profile in %s\n", SECTION_mcueqpSvmpTable);
        return wRet;
    }
	
    memset(m_atSvmpTable, 0, sizeof(m_atSvmpTable));
    
    for( u8 byRow = 1; byRow <= bySvmpNum && ptSvmpTable; byRow++)
    {
        wRet = SetEqpSvmpCfgByRow( byRow, &ptSvmpTable[byRow-1],bIsNeedUpdatePortInTable );
        m_atSvmpTable[byRow-1].SetDefaultEntParam( EQP_TYPE_VMP, ntohl(GetLocalIp()), 3 );
		
		//界面保存配置信息，内存外设表中MCU侧端口应该保持不变，避免用户不重启，该外设无法正常使用 [2/20/2012 chendaiwei]
		if( !bIsNeedUpdatePortInTable )
		{
			u16 wMcuPort = awMcuRecvPort[ptSvmpTable[byRow-1].GetEqpId() - VMPID_MIN];
			m_atSvmpTable[byRow-1].SetMcuRecvPort(wMcuPort);
		}

        if ( SUCCESS_AGENT != wRet )    break;
    }
    return wRet;
}

/*=============================================================================
函 数 名： ReadDvmpTable
功    能： 读表
算法实现： 
全局变量： 
参    数： u8* pbyDvmpNum
TEqpDvmpBasicInfo* ptHduTable
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
// u16    CCfgAgent::ReadDvmpTable( u8* pbyDvmpNum, TEqpDvmpBasicInfo* ptDvmpTable )
// {
//     if(NULL == pbyDvmpNum || NULL == ptDvmpTable )
//     {
//         return ERR_AGENT_NULLPARAM;
//     }
//     *pbyDvmpNum = (u8)m_dwDvmpEntryNum;     //fixme
// 	
// 	u16 wRet = SUCCESS_AGENT;
//     for( u8 byRow = 1; byRow <= *pbyDvmpNum; byRow ++ )
//     {
//         wRet = GetEqpDvmpCfgByRow( byRow, &ptDvmpTable[byRow-1] );
//         if ( SUCCESS_AGENT != wRet )    break;
//     }
//     return wRet;
// }

/*=============================================================================
函 数 名： WriteDvmpTable
功    能： 写表
算法实现： 
全局变量： 
参    数： u8 byDvmpNum
TEqpDvmpBasicInfo* ptHduTable
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
// u16    CCfgAgent::WriteDvmpTable( u8 byDvmpNum,  TEqpDvmpBasicInfo* ptDvmpTable )
// {
//     if( NULL == ptDvmpTable && byDvmpNum != 0)
// 	{
// 		return ERR_AGENT_NULLPARAM;
// 	}
//     m_dwDvmpEntryNum = (u32)byDvmpNum;    //fixme
// 	
//     u16 wRet = SUCCESS_AGENT;
//     wRet = WriteTableEntryNum( m_achCfgName, (s8*)SECTION_mcueqpDvmpTable, 
// 		(s8*)ENTRY_NUM, byDvmpNum );
//     if( SUCCESS_AGENT != wRet )
//     {
//         printf("[WriteDvmpTable] Fail to write profile in %s\n", SECTION_mcueqpDvmpTable);
//         return wRet;
//     }
// 	
//     memset(m_atDvmpBasicTable, 0, sizeof(m_atDvmpBasicTable));
//     
//     for( u8 byRow = 1; byRow <= byDvmpNum; byRow++)
//     {
//         wRet = SetEqpDvmpCfgByRow( byRow, &ptDvmpTable[byRow-1] );
//         m_atDvmpBasicTable[byRow-1].SetDefaultEntParam( EQP_TYPE_VMP, ntohl(GetLocalIp()), 3 );
// 		
//         if ( SUCCESS_AGENT != wRet )    break;
//     }
//     return wRet;
// }

/*=============================================================================
函 数 名： ReadMpuBasTable
功    能： 读表
算法实现： 
全局变量： 
参    数： u8* pbyMpuBasNum
TEqpMpuBasInfo* ptMpuBasTable
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
u16    CCfgAgent::ReadMpuBasTable( u8* pbyMpuBasNum, TEqpMpuBasInfo* ptMpuBasTable )
{
    if(NULL == pbyMpuBasNum || NULL == ptMpuBasTable)
    {
        return ERR_AGENT_NULLPARAM;
    }
    
	u16 wRet = SUCCESS_AGENT;
    *pbyMpuBasNum = (u8)m_dwMpuBasEntryNum;
    for( u8 byRow = 1; byRow <= *pbyMpuBasNum; byRow ++ )
    {
        wRet = GetEqpMpuBasCfgByRow( byRow, &ptMpuBasTable[byRow-1] );
        if ( SUCCESS_AGENT != wRet )    break;
    }
    return wRet;
}

/*=============================================================================
函 数 名： WriteMpuBasTable
功    能： 写表
算法实现： 
全局变量： 
参    数： u8 byMpuBasNum
TEqpMpuBasInfo* ptMpuBasTable 
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
u16    CCfgAgent::WriteMpuBasTable( u8 byMpuBasNum,  TEqpMpuBasInfo* ptMpuBasTable,BOOL32 bIsNeedUpdatePortInTable )
{
    if( NULL == ptMpuBasTable && byMpuBasNum != 0 )
	{
		return ERR_AGENT_NULLPARAM;
	}

	//保存MCU侧分配端口[2/20/2012 chendaiwei]
	u16 awMcuRecvPort[MAX_PRIEQP_NUM] = {0};
	for( u8 byIdx = 0; byIdx < m_dwMpuBasEntryNum; byIdx++ )
	{
		awMcuRecvPort[m_atMpuBasTable[byIdx].GetEqpId()-BASID_MIN] = m_atMpuBasTable[byIdx].GetMcuRecvPort();
	}

    m_dwMpuBasEntryNum = byMpuBasNum;
    u16 wRet = SUCCESS_AGENT;
    wRet = WriteTableEntryNum( m_achCfgName, (s8*)SECTION_mcueqpMpuBasTable, 
		(s8*)ENTRY_NUM, byMpuBasNum );
    if( SUCCESS_AGENT != wRet )
    {
        printf("[WriteMpuBasTable] Fail to write profile in %s\n", SECTION_mcueqpMpuBasTable);
        return wRet;
    }
	
    memset( m_atMpuBasTable, 0, sizeof(m_atMpuBasTable) );
    
    for( u8 byRow = 1; byRow <= byMpuBasNum && ptMpuBasTable; byRow++)
    {
        wRet = SetEqpMpuBasCfgByRow( byRow, &ptMpuBasTable[byRow-1],bIsNeedUpdatePortInTable );
        m_atMpuBasTable[byRow-1].SetDefaultEntParam( EQP_TYPE_BAS, ntohl(GetLocalIp()), 3 );
		
		//界面保存配置信息，内存外设表中MCU侧端口应该保持不变，避免用户不重启，该外设无法正常使用 [2/20/2012 chendaiwei]
		if( !bIsNeedUpdatePortInTable )
		{
			u16 wMcuPort = awMcuRecvPort[ptMpuBasTable[byRow-1].GetEqpId() - BASID_MIN];
			m_atMpuBasTable[byRow-1].SetMcuRecvPort(wMcuPort);
		}

        if ( SUCCESS_AGENT != wRet )    break;
    }
    return wRet;
}

/*=============================================================================
函 数 名： ReadEbapTable
功    能： 读表
算法实现： 
全局变量： 
参    数： u8* pbyEbapNum, TEqpEbapInfo* ptEbapTable
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
// u16    CCfgAgent::ReadEbapTable( u8* pbyEbapNum, TEqpEbapInfo* ptEbapTable )
// {
// 	if(NULL == pbyEbapNum || NULL == ptEbapTable)
//     {
//         return ERR_AGENT_NULLPARAM;
//     }
//     
// 	u16 wRet = SUCCESS_AGENT;
//     *pbyEbapNum = (u8)m_dwEbapEntryNum;
//     for( u8 byRow = 1; byRow <= *pbyEbapNum; byRow ++ )
//     {
//         wRet = GetEqpEbapCfgByRow( byRow, &ptEbapTable[byRow-1] );
//         if ( SUCCESS_AGENT != wRet )    break;
//     }
// 
//     return wRet;
// 
// }

/*=============================================================================
函 数 名： WriteEbapTable
功    能： 写表
算法实现： 
全局变量： 
参    数： u8 byEbapNum,  TEqpEbapInfo* ptEbapTable
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
// u16    CCfgAgent::WriteEbapTable( u8 byEbapNum,  TEqpEbapInfo* ptEbapTable )
// {
//     if( NULL == ptEbapTable && byEbapNum != 0 )
// 	{
// 		return ERR_AGENT_NULLPARAM;
// 	}
//     m_dwEbapEntryNum = byEbapNum;
//     u16 wRet = SUCCESS_AGENT;
//     wRet = WriteTableEntryNum( m_achCfgName, (s8*)SECTION_mcueqpEbapTable, 
// 		(s8*)ENTRY_NUM, byEbapNum );
//     if( SUCCESS_AGENT != wRet )
//     {
//         printf("[CCfgAgent::WriteEbapTable] Fail to write profile in %s\n", SECTION_mcueqpEbapTable);
//         return wRet;
//     }
// 	
//     memset( m_atEbapTable, 0, sizeof(m_atEbapTable) );
//     
//     for( u8 byRow = 1; byRow <= byEbapNum; byRow++)
//     {
//         wRet = SetEqpEbapCfgByRow( byRow, &ptEbapTable[byRow-1] );
//         m_atEbapTable[byRow-1].SetDefaultEntParam( EQP_TYPE_BAS, ntohl(GetLocalIp()), 3 );
// 		
//         if ( SUCCESS_AGENT != wRet )    break;
//     }
//     return wRet;
// }

/*=============================================================================
函 数 名： ReadEvpuTable
功    能： 读表
算法实现： 
全局变量： 
参    数： u8* pbyEvpuNum, TEqpEvpuInfo* ptEvpuTable
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
==============================================================================*/
// u16    CCfgAgent::ReadEvpuTable( u8* pbyEvpuNum, TEqpEvpuInfo* ptEvpuTable )
// {
// 	if(NULL == pbyEvpuNum || NULL == ptEvpuTable)
//     {
//         return ERR_AGENT_NULLPARAM;
//     }
//     
// 	u16 wRet = SUCCESS_AGENT;
//     *pbyEvpuNum = (u8)m_dwEvpuEntryNum;
//     for( u8 byRow = 1; byRow <= *pbyEvpuNum; byRow ++ )
//     {
//         wRet = GetEqpEvpuCfgByRow( byRow, &ptEvpuTable[byRow-1] );
//         if ( SUCCESS_AGENT != wRet )    break;
//     }
// 	
//     return wRet;
// 
// }

/*=============================================================================
函 数 名： WriteEvpuTable
功    能： 写表
算法实现： 
全局变量： 
参    数： u8 byEvpuNum,  TEqpEvpuInfo* ptEvpuTable
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
// u16    CCfgAgent::WriteEvpuTable( u8 byEvpuNum,  TEqpEvpuInfo* ptEvpuTable )
// {
//     if( NULL == ptEvpuTable && byEvpuNum != 0 )
// 	{
// 		return ERR_AGENT_NULLPARAM;
// 	}
//     m_dwEvpuEntryNum = byEvpuNum;
//     u16 wRet = SUCCESS_AGENT;
//     wRet = WriteTableEntryNum( m_achCfgName, (s8*)SECTION_mcueqpEvpuTable, 
// 		(s8*)ENTRY_NUM, byEvpuNum );
//     if( SUCCESS_AGENT != wRet )
//     {
//         printf("[CCfgAgent::WriteEvpuTable] Fail to write profile in %s\n", SECTION_mcueqpEvpuTable);
//         return wRet;
//     }
// 	
//     memset( m_atEvpuTable, 0, sizeof(m_atEvpuTable) );
//     
//     for( u8 byRow = 1; byRow <= byEvpuNum; byRow++)
//     {
//         wRet = SetEqpEvpuCfgByRow( byRow, &ptEvpuTable[byRow-1] );
//         m_atEvpuTable[byRow-1].SetDefaultEntParam( EQP_TYPE_VMP, ntohl(GetLocalIp()), 3 );
// 		
//         if ( SUCCESS_AGENT != wRet )    break;
//     }
//     return wRet;
// }

/*=============================================================================
函 数 名： ReadHduSchemeTable
功    能： 读表
算法实现： 
全局变量： 
参    数： u8* pbyNum, THduStyleInfo* ptCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
==============================================================================*/
u16    CCfgAgent::ReadHduSchemeTable( u8* pbyNum, THduStyleInfo* ptCfg )
{
    if(NULL == pbyNum || NULL == ptCfg )
    {
        Agtlog(LOG_ERROR, "[ReadHduSchemeTable] param error: pNum.0x%x, pTable.0x%x \n", 
			pbyNum, ptCfg );
        return ERR_AGENT_NULLPARAM;
    }
	
    *pbyNum = (u8)m_dwHduSchemeNum;
    
    u16 wRet = SUCCESS_AGENT;
    for(u8 byIndex = 0; byIndex < m_dwHduSchemeNum; byIndex++)
    {
        if( NULL != &ptCfg[byIndex] )
        {
            memcpy( &ptCfg[byIndex], &m_atHduSchemeTable[byIndex], sizeof(THduStyleInfo) );
        }
        else
        {
            wRet = (u16)ERR_AGENT_NULLPARAM;
        }
    }
	
    return wRet;
	
}

/*=============================================================================
函 数 名： WriteHduSchemeTable   
功    能： 写表                    
算法实现： 
全局变量： 
参    数： u8 byNum,  THduStyleInfo* ptCfg
返 回 值： u16 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
u16    CCfgAgent::WriteHduSchemeTable( u8 byNum,  THduStyleInfo* ptCfg )
{
	if(NULL == ptCfg && 0 != byNum )
	{
		Agtlog( LOG_ERROR, "[WriteHduSchemeTable] param err: ptTable.0x%x !\n", ptCfg );
		return ERR_AGENT_SETNODEVALUE;
	}
    m_dwHduSchemeNum = byNum;
	
    u16 wRet = SUCCESS_AGENT;
    wRet = WriteTableEntryNum(m_achCfgName, (s8*)SECTION_mcuHduSchemeTable, 
		(s8*)ENTRY_NUM, byNum );
    if( SUCCESS_AGENT != wRet )
    {
        printf("[WriteHduSchemeTable] Fail to write profile in %s\n", SECTION_mcuHduSchemeTable);
        return wRet;
    }    
	
    s8   achEntryStr[32];
    s8   achHduStyleCfgInfo[512];
	//s8   achHduStyleChnTable[512];  
    for( u8 byIndex = 0; byIndex < byNum; byIndex++ )
    {
        if(ptCfg /*NULL != &ptCfg[byIndex]*/)
        {
            memcpy( &m_atHduSchemeTable[byIndex], &ptCfg[byIndex], sizeof(THduStyleInfo) );
			
            memset(achHduStyleCfgInfo, '\0', sizeof(achHduStyleCfgInfo));
            memset(achEntryStr, '\0', sizeof(achEntryStr));
            
            GetEntryString(achEntryStr, byIndex);
            sprintf(achHduStyleCfgInfo, "  %d\t%d\t%d\t%d\t%d\t%d\t%s", 
				ptCfg[byIndex].GetHeight(),
				ptCfg[byIndex].GetWidth(),
				ptCfg[byIndex].GetIsBeQuiet(),
				ptCfg[byIndex].GetStyleIdx(),
				ptCfg[byIndex].GetTotalChnlNum(),
				ptCfg[byIndex].GetVolumn(),
				ptCfg[byIndex].GetSchemeAlias() );            
            wRet = WriteTableEntry( m_achCfgName, (s8*)SECTION_mcuHduSchemeTable, 
				achEntryStr, achHduStyleCfgInfo );
            if( SUCCESS_AGENT != wRet )
            {
                printf("[WriteHduSchemeTable] Fail to write the configure file\n");
                break;
            }
			
			s8 achSchemeChnTableName[256];
			memset(achSchemeChnTableName, 0x0, sizeof(achSchemeChnTableName));
			switch (byIndex)
			{
			case 0:
                memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable1, strlen(SECTION_mcuHduSchemeChnTable1) );
                break;
				
			case 1:
                memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable2, strlen(SECTION_mcuHduSchemeChnTable2) );
                break;
				
			case 2:
                memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable3, strlen(SECTION_mcuHduSchemeChnTable3) );
                break;
				
			case 3:
                memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable4, strlen(SECTION_mcuHduSchemeChnTable4) );
                break;
				
			case 4:
                memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable5, strlen(SECTION_mcuHduSchemeChnTable5) );
                break;
				
			case 5:
                memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable6, strlen(SECTION_mcuHduSchemeChnTable6) );
                break;
				
			case 6:
                memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable7, strlen(SECTION_mcuHduSchemeChnTable7) );
                break;
				
			case 7:
                memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable8, strlen(SECTION_mcuHduSchemeChnTable8) );
                break;
				
			case 8:
                memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable9, strlen(SECTION_mcuHduSchemeChnTable9) );
                break;
				
			case 9:
                memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable10, strlen(SECTION_mcuHduSchemeChnTable10) );
                break;
				
			case 10:
                memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable11, strlen(SECTION_mcuHduSchemeChnTable11) );
                break;
				
			case 11:
                memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable12, strlen(SECTION_mcuHduSchemeChnTable12) );
                break;
				
			case 12:
                memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable13, strlen(SECTION_mcuHduSchemeChnTable13) );
                break;
				
			case 13:
                memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable14, strlen(SECTION_mcuHduSchemeChnTable14) );
                break;
				
			case 14:
                memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable15, strlen(SECTION_mcuHduSchemeChnTable15) );
                break;
				
			case 15:
                memcpy(achSchemeChnTableName, SECTION_mcuHduSchemeChnTable16, strlen(SECTION_mcuHduSchemeChnTable16) );
                break;
				
			default:
				OspPrintf(TRUE, FALSE, "[WriteHduSchemeTable] hdu Scheme's Index is beyond 15 !\n");
			}
			
			u8 bySchemeChnNum = ptCfg[byIndex].GetTotalChnlNum();
			wRet = WriteTableEntryNum(m_achCfgName, achSchemeChnTableName, 
				(s8*)ENTRY_NUM, bySchemeChnNum );
			if( SUCCESS_AGENT != wRet )
			{
				printf("[WriteHduSchemeChnTable] Fail to write profile in %s\n", achSchemeChnTableName);
				return wRet;
			}    
			
			u8 byEntry = 0;
			THduChnlInfo atHduChnlInfo[MAXNUM_HDUCFG_CHNLNUM];
			memset(atHduChnlInfo, '\0', sizeof(atHduChnlInfo));
			ptCfg[byIndex].GetHduChnlTable(atHduChnlInfo);

			s8 szHduSectionInfo[MAX_WRITE_SECTIONONECE_LEN] = {0};
			s8 szRowHduInfo[MAX_VALUE_LEN + 1 ]={0};	
			u32 dwSectionOffSet = 0;

			for ( u8 byIndex2=0; byIndex2<MAXNUM_HDUCFG_CHNLNUM; byIndex2++ )
			{
				if ( 0!= atHduChnlInfo[byIndex2].GetEqpId())
				{
					memset(szRowHduInfo, '\0', sizeof(szRowHduInfo));
					memset(achEntryStr, '\0', sizeof(achEntryStr));
					GetEntryString(achEntryStr, byEntry);
					byEntry++;
					sprintf(szRowHduInfo, "%s = %d\t%d\t%d\t%d\t%d\t%s%s",
						achEntryStr,
						byIndex2,
						atHduChnlInfo[byIndex2].GetChnlIdx(),
						atHduChnlInfo[byIndex2].GetEqpId(),
						atHduChnlInfo[byIndex2].GetChnlVolumn(),
						atHduChnlInfo[byIndex2].GetIsChnlBeQuiet(),
						atHduChnlInfo[byIndex2].GetSchemeAlias(),
						STR_RETURN
						);
					
					if (strlen(szRowHduInfo) > MAX_VALUE_LEN  )
					{
						szRowHduInfo[MAX_VALUE_LEN ] = '\0';
					}

					//写的单板内容多于2k，分为多块写
					if ( (strlen(szHduSectionInfo) + strlen(szRowHduInfo)) > MAX_WRITE_SECTIONONECE_LEN )
					{
						if ( !SetRegKeySection(m_achCfgName,(s8*)achSchemeChnTableName,szHduSectionInfo,(u16)dwSectionOffSet,ENTRY_NUM) ) 
						{
							StaticLog("[WriteHduSchemeChnTable]SetRegKeySection error!\n");
							wRet = ERR_AGENT_WRITEFILE;
						}
						dwSectionOffSet += strlen(szHduSectionInfo);
						memset(szHduSectionInfo,0,sizeof(szHduSectionInfo));
						
					}	 
					sprintf(szHduSectionInfo,"%s%s",szHduSectionInfo,szRowHduInfo);

					//wRet = WriteTableEntry( m_achCfgName, (s8*)achSchemeChnTableName, 
					//	achEntryStr, achHduStyleChnTable );
					if( SUCCESS_AGENT != wRet )
					{
						printf("[WriteHduSchemeChnTable] Fail to write the configure file\n");
						break;
					}
				}
				else
				{
					continue;
				}				
			}
			
			if ( strlen(szHduSectionInfo) > 0 ) 
			{ 
				if ( !SetRegKeySection(m_achCfgName,(s8*)achSchemeChnTableName,szHduSectionInfo,(u16)dwSectionOffSet,ENTRY_NUM) )
				{
					StaticLog("[WriteHduSchemeChnTable]SetRegKeySection error!\n");
					wRet = ERR_AGENT_WRITEFILE;
				}
			}
        }
    }    
	
    return wRet;
	
}

/*=============================================================================
函 数 名： ChangeBoardStatus
功    能： 记录单板状态
算法实现： 
全局变量： 
参    数：TBrdPosition tBrdPosition
u8 byStatus
返 回 值：void 
=============================================================================*/
void CCfgAgent::ChangeBoardStatus(TBrdPosition tBrdPosition, u8 byStatus)
{
    for(u8 byIndex = 0; byIndex < m_dwBrdCfgEntryNum; byIndex++)
    {
        if( m_atBrdCfgTable[byIndex].GetLayer() == tBrdPosition.byBrdLayer && 
            m_atBrdCfgTable[byIndex].GetSlot()  == tBrdPosition.byBrdSlot && 
            m_atBrdCfgTable[byIndex].GetType()  == tBrdPosition.byBrdID )
        {
			Agtlog(LOG_INFORM, "ChangeBoardStatus: The Board<%d,%d:%s> eth choice is different.\n",
				tBrdPosition.byBrdLayer, tBrdPosition.byBrdSlot,
				GetBrdTypeStr(tBrdPosition.byBrdID) );
			
            m_atBrdCfgTable[byIndex].SetState( byStatus );
            break;
        }
    }
    return;
}

#ifdef _LINUX_
/*=============================================================================
函 数 名： _Get8KECpuInfo
功    能： 获取MCU8000G的Cpu使用率
算法实现： 
全局变量： 
参    数： T8KECpuInfo* pt8KECpuInfo
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录：
日  期		   版本		    修改人		走读人      修改内容
2010/11/30     5.0			苗庆松                    创建
=============================================================================*/
static BOOL32 _Get8KECpuInfo( T8KECpuInfo* pt8KECpuInfo )
{
	if( NULL == pt8KECpuInfo )
	{
		return FALSE;
	}
    
	static BOOL32 bFirstOperate = TRUE;
	static u8     byIdleTimePrev = 100;
    static u32    dwLastTotalTick = 0;
	static u32    dwLastIdleTick = 0;
	
    FILE* ptCpuFile = fopen( "/proc/stat" , "r" );
	
	if( NULL == ptCpuFile )
	{
		return FALSE;
	}
	
	rewind( ptCpuFile );
	
	s8       achBuffer[256] = {0};
	BOOL32   bFind = FALSE;
	u32      dwCurrentUserTick = 0;
	u32      dwCurrentNiceTick = 0;
	u32      dwCurrentSysTick = 0;
	u32      dwCurrentIdleTick = 0;
	u32      dwCurrentTotalTick = 0;
	
	while( NULL != fgets( achBuffer , 256 , ptCpuFile ) )
	{
		if( sscanf( achBuffer , "cpu %d %d %d %d" ,
			&dwCurrentUserTick ,
			&dwCurrentNiceTick ,
			&dwCurrentSysTick ,
			&dwCurrentIdleTick	) > 0 )
		{
			bFind = TRUE;
			break;
		}
	}
	
	if( FALSE == bFind )
	{
		fclose( ptCpuFile );
		return FALSE;
	}
	else if( bFirstOperate )
	{
		bFirstOperate = FALSE;
		pt8KECpuInfo->m_by8KEIdlePercent = byIdleTimePrev;
		dwLastTotalTick = dwCurrentUserTick+dwCurrentNiceTick+dwCurrentSysTick+dwCurrentIdleTick;
		dwLastIdleTick = dwCurrentIdleTick;
	}
	else
	{
		dwCurrentTotalTick = dwCurrentUserTick+dwCurrentNiceTick+dwCurrentSysTick+dwCurrentIdleTick;
		if( dwLastTotalTick == dwCurrentTotalTick )
		{
			pt8KECpuInfo->m_by8KEIdlePercent = byIdleTimePrev;
		}
		else
		{
			byIdleTimePrev = ( dwCurrentIdleTick - dwLastIdleTick )*100/( dwCurrentTotalTick - dwLastTotalTick);
			pt8KECpuInfo->m_by8KEIdlePercent = byIdleTimePrev;
		}
		dwLastTotalTick = dwCurrentTotalTick;
		dwLastIdleTick = dwCurrentIdleTick;
	}
	
	fclose( ptCpuFile );
	
	return TRUE;
}

/*=============================================================================
函 数 名： _Gey8KEMemoryInfo
功    能： 获取MCU8000G的内存占用率
算法实现： 
全局变量： 
参    数： T8KEMemInfo* pt8KEMemInfo
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录：
日  期		   版本		    修改人		走读人      修改内容
2010/12/01     5.0			苗庆松                    创建
=============================================================================*/
static BOOL32 _Gey8KEMemoryInfo( T8KEMemInfo* pt8KEMemInfo)
{
	if( NULL == pt8KEMemInfo )
	{
		return FALSE;
	}
	
	FILE* ptMemFile = fopen( "/proc/meminfo" , "r" );
	if( NULL == ptMemFile )
	{
		return FALSE;
	}
	
	char achBuffer[256] = {0};
	BOOL32 bFind = FALSE;
	u32 dwMemTotal = 0;
	u32 dwMemFree = 0;
	
	rewind( ptMemFile );
	while( NULL != fgets( achBuffer , 256 , ptMemFile ) )
	{
		if( sscanf( achBuffer , "MemTotal: %d kB" , &dwMemTotal ) > 0 )
		{
			bFind = TRUE;
			break;
		}
	}
    if( FALSE == bFind )
	{
		fclose( ptMemFile );
		return FALSE;
	}
	
    bFind = FALSE;
	
	rewind( ptMemFile );
	while( NULL != fgets( achBuffer , 256 , ptMemFile ) )
	{
		if( sscanf( achBuffer , "MemFree: %d kB" , &dwMemFree ) > 0 )
		{
			bFind = TRUE;
			break;
		}
	}
    
	if( FALSE == bFind )
	{
		pt8KEMemInfo->m_dw8KEPhysicsSize = 0;
		pt8KEMemInfo->m_dw8KEAllocSize = 0;
		pt8KEMemInfo->m_dw8KEFreeSize = 0;
	}
	else
	{
		pt8KEMemInfo->m_dw8KEPhysicsSize = dwMemTotal;
		pt8KEMemInfo->m_dw8KEAllocSize = dwMemTotal - dwMemFree;
		pt8KEMemInfo->m_dw8KEFreeSize = dwMemFree;
	}
	
	fclose( ptMemFile );
	
	return bFind;
}

/*====================================================================
函数名：Get8KECpuInfo
功能：  获取MCU8000G的Cpu使用率
算法实现：
引用全局变量：
输入参数说明：pt8KECpuInfo，用户CPU信息结构
返回值说明：为TRUE时表示获取信息成功，否则表示失败
----------------------------------------------------------------------
修改记录：
日  期		   版本		    修改人		走读人      修改内容
2010/11/30     5.0			苗庆松                    创建
====================================================================*/
BOOL32 CCfgAgent::Get8KECpuInfo( T8KECpuInfo* pt8KECpuInfo )
{
	BOOL32 bRet ;
	
	bRet = _Get8KECpuInfo(pt8KECpuInfo);
	
	return bRet;
}

/*====================================================================
函数名：Get8KEMemoryInfo
功能：  获取MCU8000G的Memory使用率
算法实现：
引用全局变量：
输入参数说明：pt8KEMemInfo，用户Memory信息结构
返回值说明：为TRUE时表示获取信息成功，否则表示失败
----------------------------------------------------------------------
修改记录：
日  期		   版本		    修改人		走读人      修改内容
2010/12/01     5.0			苗庆松                    创建
====================================================================*/
BOOL32 CCfgAgent::Get8KEMemoryInfo( T8KEMemInfo* pt8KEMemInfo)
{
	BOOL32 bRet;
	
	bRet = _Gey8KEMemoryInfo(pt8KEMemInfo);
	
	return bRet;
}
#endif

/*=================================================================================
函 数 名： GetCpuRate
功    能： 取Cpu 占有率
算法实现： 
全局变量： 
参    数： 
返 回 值： u8 
-----------------------------------------------------------------------------------
修改记录：
日  期		   版本		    修改人		走读人         修改内容
2010/11/30         5.0			苗庆松               添加MCU8000GCpu占用率统计
====================================================================================*/
/*lint -save -e527*/
u8  CCfgAgent::GetCpuRate()
{
	TOspCpuInfo tCpuInfo;
    memset( &tCpuInfo, 0, sizeof(TOspCpuInfo) );
#ifdef _LINUX_	
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
    // [20101130 miaoqingsong] 获取MCU8000G的Cpu占用率
	T8KECpuInfo t8KECpuInfo;
	memset( &t8KECpuInfo, 0, sizeof(T8KECpuInfo) );
	
    if (!Get8KECpuInfo(&t8KECpuInfo))
    {
        Agtlog(LOG_ERROR, "[Get8KECpuRate] Error Get Cpu Percentage.\n");
        return 0;
    }
	return (100 - t8KECpuInfo.m_by8KEIdlePercent);
#endif
#endif
    
    if (!OspGetCpuInfo(&tCpuInfo))
    {
        Agtlog(LOG_ERROR, "[GetMpcCpuRate] Error Get Cpu Percentage.\n");
        return 0;
    }
    return (100 - tCpuInfo.m_byIdlePercent);
}
/*lint -restore*/
/*================================================================================
函 数 名： GetMemoryRate
功    能： 取Memory占有率
算法实现： 
全局变量： 
参    数： 
返 回 值： u8  
---------------------------------------------------------------------------------
修改记录：
日  期		   版本		    修改人		走读人          修改内容
2010/12/01         5.0			苗庆松               添加MCU8000G内存使用率统计
=================================================================================*/
/*lint -save -e527*/
u32  CCfgAgent::GetMemoryRate()
{
	TOspMemInfo tMemInfo;
    memset( &tMemInfo, 0, sizeof(TOspMemInfo) );
#ifdef _LINUX_
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
    // [20101201 miaoqingsong] 获取MCU8000G的内存使用信息
	T8KEMemInfo t8KEMemInfo;
	memset( &t8KEMemInfo, 0,sizeof(T8KEMemInfo) );
	
    if (!Get8KEMemoryInfo(&t8KEMemInfo))
    {
        Agtlog(LOG_ERROR, "[Get8KEMemoryRate] Error Get Memory Percentage.\n");
        return 0;
    }
	
    return MAKEDWORD(t8KEMemInfo.m_dw8KEAllocSize / 1000, t8KEMemInfo.m_dw8KEPhysicsSize / 1000);
#endif
#endif
    
    if (!OspGetMemInfo(&tMemInfo))
    {
        Agtlog(LOG_ERROR, "[GetMemoryRate] Error Get Memory Percentage.\n");
        return 0;
    }
	
	return MAKEDWORD(tMemInfo.m_dwAllocSize / 1000, tMemInfo.m_dwPhysicsSize / 1000);
}
/*lint -restore*/


void CCfgAgent::SetMcuCompileTime(s8 *pszCompileTime)
{
	if (pszCompileTime == NULL)
	{
		return;
	}
	memcpy(m_achCompileTime, pszCompileTime, min(sizeof(m_achCompileTime), strlen(pszCompileTime)));
}

s8* CCfgAgent::GetMcuCompileTime(u16 &wLen)
{
	wLen = strlen(m_achCompileTime);
	return m_achCompileTime;
}

void CCfgAgent::SetTempPortKind(u8 byPortKind)
{
	m_byTempPortKind = byPortKind;
}
u8 CCfgAgent::GetTempPortKind()
{
	return m_byTempPortKind;
}


/*=============================================================================
函 数 名： RebootBoard
功    能： 重启指定单板
算法实现： 
全局变量： 
参    数： TBrdPosition tBrdPos
返 回 值： u16 
=============================================================================*/
u16 CCfgAgent::RebootBoard(u8 byLayer, u8 bySlot, u8 byType)
{
    TBrdPosition tBrdPos;
    tBrdPos.byBrdLayer = byLayer;
    tBrdPos.byBrdSlot  = bySlot;
    tBrdPos.byBrdID    = byType;
	
    u16 wRet = SNMP_GENERAL_ERROR;
    for(u8 byIndex = 0; byIndex < m_dwBrdCfgEntryNum; byIndex++)
    {
        if( m_atBrdCfgTable[byIndex].GetLayer() == byLayer &&
            m_atBrdCfgTable[byIndex].GetSlot()  == bySlot &&
            m_atBrdCfgTable[byIndex].GetType()  == byType )
        {  
            PostMsgToBrdMgr( tBrdPos, MPC_BOARD_RESET_CMD, NULL, 0);
            wRet = SUCCESS_AGENT;
            break;
        }
    }
    return wRet;
}

/*=============================================================================
函 数 名： PretreatConfigureFile
功    能： 判断文件是否存在，不存在，生成默认配置文件
算法实现： 
全局变量： 
参    数： s8* achFileName
u8 byMpcType      暂没有使用
返 回 值： void 
=============================================================================*/
u16 CCfgAgent::PretreatConfigureFile(s8* achFileName, u8 byMpcType)
{
    AgentDirCreate(DIR_CONFIG);
	
    FILE* hWrite = fopen(achFileName, "w"); // create configure file
	if( NULL == hWrite )
    {
		return ERR_AGENT_CREATECONFFILE;	
	}
	
	fclose(hWrite);
	
    u16 wRet = SUCCESS_AGENT;
	
    // 版本信息
    wRet |= SetSystemCfgVer(MCUAGT_SYS_CFGVER);
	
	// Mcu配置标识置0, 支持MCU导航配置, zgc 06-12-25
	wRet |= SetIsMcuConfiged( FALSE );
    
    srand((unsigned)time(NULL));
	
    TLocalInfo tLocalInfo;
    tLocalInfo.SetMcuId( LOCAL_MCUID );
    
    s8 achAlias[MAXLEN_ALIAS];
    memset( achAlias, 0, sizeof(achAlias) );
    u16 wRandNum = (u16)rand();
    sprintf( achAlias, "KedaMCU%d", wRandNum );
    tLocalInfo.SetAlias(achAlias);
	
    s8 achE164Num[MAXLEN_E164];
    memset( achE164Num, 0, sizeof(achE164Num) );
    wRandNum = (u16)rand();
    sprintf(achE164Num, "164#%d", wRandNum);
    tLocalInfo.SetE164Num(achE164Num);
	
    tLocalInfo.SetCheckTimes( DEF_DISCCHECKTIMES );             // 终端断链检测次数
    tLocalInfo.SetCheckTime( DEF_DISCCHECKTIME );               // 断链检测时间
    tLocalInfo.SetListRefreshTime( PARTLIST_REFRESHTIME );      // 级连时终端列表刷新时间
    tLocalInfo.SetAudioRefreshTime( AUDINFO_REFRESHTIME );      // 级连时音频信息刷新时间
    tLocalInfo.SetVideoRefreshTime( VIDINFO_REFRESHTIME );      // 级连时视频信息刷新时间
    tLocalInfo.SetIsSaveBand( 1 );                              // 是否节省带宽(0:不节省 1:节省)
	
	tLocalInfo.SetIsHoldDefaultConf( 0 );						// 是否支持缺省会议召开(0-不支持，1-支持)
	tLocalInfo.SetIsShowMMcuMtList( 0 );						//是否支持显示上级mcu终端列表(0-不支持，1-支持)
	tLocalInfo.SetMaxMcsOnGoingConfNum( DEF_MCSONGOINGCONFNUM_MAX );//本级最多召开的mcs即时会议数
	tLocalInfo.SetAdminLevel( DEF_ADMINLVEL );					//本级召开的会议的行政级别(1到4 1最大)
	tLocalInfo.SetConfNameShowType( 1 );						//在下级会议中显示的本级的会议名称的类型(1:会议名称(默认值) 2:mcu名称＋会议名称)	
	tLocalInfo.SetIsMMcuSpeaker( 1 );							//是否默认将上级mcu作为发言人(0:不是 1:是)
    
    wRet |= SetLocalInfo( &tLocalInfo );
	
    // network info
    TNetWorkInfo  tNetWorkInfo;
    tNetWorkInfo.SetGKIp( 0 );							// Gk 地址
    tNetWorkInfo.SetIsGKCharge( FALSE );                // 默认不支持计费
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
    tNetWorkInfo.SetRRQMtadpIp( ntohl(GetLocalIp())  );	// 主适配板地址
#else
    tNetWorkInfo.SetRRQMtadpIp( 0 );					// 主适配板地址
#endif
    tNetWorkInfo.SetCastIp( 0 );						// 组播地址
    tNetWorkInfo.SetCastPort( MULTICAST_DEFPORT );      // 组播端口
    tNetWorkInfo.SetRecvStartPort( MT_MCU_STARTPORT );  // 接受起始端口
    tNetWorkInfo.Set225245StartPort( MCU_H225245_STARTPORT ); // 245端口
    tNetWorkInfo.Set225245MtNum( MAXNUM_H225H245_MT );	// 245 Mt数
    tNetWorkInfo.SetMpcTransData( 0 );					// 是否使用Mpc传数据
    tNetWorkInfo.SetMpcStack( TRUE );					// 是否使用Mpc内置协议栈
	tNetWorkInfo.SetMTUSize( DEFAULT_MTU_SIZE );        // MTU长度, zgc, 2007-04-05
    wRet |= SetNetWorkInfo( &tNetWorkInfo );
    
    wRet |= WriteTrapTable(0, NULL);
    
    TBrdPosition tMpcPosition;
    memset(&tMpcPosition, 0, sizeof(TBrdPosition));
	
	//#ifndef WIN32
	AgtQueryBrdPosition(&tMpcPosition);
	
    printf("[Agt Pretreat]The Mpc Layer: %d, Slot:%d, Id: %d, Enable:%d, MpcType.%d\n", 
        tMpcPosition.byBrdLayer, tMpcPosition.byBrdSlot,
		tMpcPosition.byBrdID, IsMpcActive(),byMpcType);

	//20110825 zjl 对层号255错误进行保护
	if (tMpcPosition.byBrdLayer >= 4)
	{
		printf("[Agt Pretreat]The Mpc Layer:%d change to 0!\n ", tMpcPosition.byBrdLayer);
		tMpcPosition.byBrdLayer = 0;		
	}
	//#endif  // linux && win32
	
	u8 byPortChoice = SearchPortChoice();
	if ( byPortChoice == 0xff )
	{
#ifdef _MINIMCU_
		byPortChoice = 0;
#else
		byPortChoice = 1;
#endif
	}
	m_tMPCInfo.SetLocalPortChoice( byPortChoice );
	
    //单板表写入本mpc板信息，其他空
    TBoardInfo tBoardTable[1];
    tBoardTable[0].SetBrdId( 1 );                       // 序号
#ifdef _LINUX12_
	tBoardTable[0].SetType(BRD_TYPE_MPC2);
	tBoardTable[0].SetAlias(GetBrdTypeStr(BRD_TYPE_MPC2/*DSL8000_BRD_MPC*/)); //别名
#else
	tBoardTable[0].SetType(BRD_TYPE_MPC/*DSL8000_BRD_MPC*/);				 // 类型
	tBoardTable[0].SetAlias(GetBrdTypeStr(BRD_TYPE_MPC/*DSL8000_BRD_MPC*/)); //别名
#endif
    tBoardTable[0].SetLayer(tMpcPosition.byBrdLayer);   // 层号
    tBoardTable[0].SetSlot(tMpcPosition.byBrdSlot);     // 槽号
    tBoardTable[0].SetPortChoice(m_tMPCInfo.GetLocalPortChoice());    // 网口选择
    tBoardTable[0].SetCastChoice(0);                    // 组播选择
    tBoardTable[0].SetBrdIp( ntohl(GetLocalIp()) );     // 单板Ip
	
    wRet |= WriteBrdTable( 1, tBoardTable );
    
	wRet |= WriteMixerTable(0, NULL);
    wRet |= WriteRecTable(0, NULL);
    wRet |= WriteTvTable( 0, NULL);
    wRet |= WriteBasTable(0, NULL);
    wRet |= WriteBasHDTable(0, NULL);
    wRet |= WriteVmpTable(0, NULL);
    wRet |= WriteMpwTable(0, NULL);
    wRet |= SetDcsIp(0);
    wRet |= WriteVmpAttachTable(0, NULL);
    wRet |= WritePrsTable(0, NULL);
	//4.6新加版本 jlb
    wRet |= WriteHduTable(0, NULL);
	wRet |= WriteHduSchemeTable(0, NULL);
    wRet |= WriteSvmpTable(0, NULL);
	//    wRet |= WriteDvmpTable(0, NULL);
    wRet |= WriteMpuBasTable(0, NULL);
	//    wRet |= WriteEbapTable(0, NULL);
	//    wRet |= WriteEvpuTable(0, NULL);
    wRet |= WriteVrsRecTable(0, NULL);
	
    
    // net sync
    TNetSyncInfo tNetSyncInfo;
    tNetSyncInfo.SetMode(NETSYNTYPE_VIBRATION);	// 网同步模式
    tNetSyncInfo.SetDTSlot(10);		            // 网同步使用的DT板号
    tNetSyncInfo.SetE1Index(0);	
    wRet |= SetNetSyncInfo( &tNetSyncInfo );
	
    // Qos
    TQosInfo tQosInfo;
    tQosInfo.SetQosType(QOSTYPE_IP_PRIORITY);   // Qos 类型
    tQosInfo.SetAudioLevel(AUDIO_LVL_IP);		// 音频等级
    tQosInfo.SetVideoLevel(VIDEO_LVL_IP);		// 视频等级
    tQosInfo.SetDataLevel(DATA_LVL_IP);			// 数据等级
    tQosInfo.SetSignalLevel(SIGNAL_LVL_IP);		// 信号等级
    tQosInfo.SetIpServiceType(IPSERVICETYPE_LOW_DELAY); // IP服务等级
    wRet |= SetQosInfo( &tQosInfo );    
	
	// TEpqExCfg
	TEqpExCfgInfo tEqpExCfgInfo;
	wRet |= SetEqpExCfgInfo( tEqpExCfgInfo );                   //设置外设扩展信息  
	
	// DSC info
#ifdef _MINIMCU_
	TDSCModuleInfo tDscInfo;
	tDscInfo.SetStartMp( TRUE );
	tDscInfo.SetStartMtAdp( TRUE );
	tDscInfo.SetStartProxy( TRUE );
	tDscInfo.SetStartDcs( TRUE );
	tDscInfo.SetStartGk( TRUE );
	tDscInfo.SetDscInnerIp( ntohl(INET_ADDR(DEFAULT_DSC_INNERIP)) );
	
	//#ifndef WIN32
    // MCU IP 和端口则自行获取
    // 首先尝试内网口
    TBrdEthParam tEthParam;
    STATUS nRet = AgtGetBrdEthParam( 1, &tEthParam );
    if ( nRet == OK && tEthParam.dwIpAdrs != 0 )
    {
        tDscInfo.SetMcuInnerIp( ntohl(tEthParam.dwIpAdrs) );
        tDscInfo.SetInnerIpMask( ntohl(tEthParam.dwIpMask) );
    }
    else
    {
		tDscInfo.SetMcuInnerIp( ntohl(INET_ADDR(DEFAULT_MCU_INNERIP)) );
		tDscInfo.SetInnerIpMask( ntohl(INET_ADDR(DEFAULT_INNERIPMASK)) );
    }
	//#else
	//    m_tMcuDscInfo.SetMcuInnerIp( ntohl(INET_ADDR(DEFAULT_MCU_INNERIP)) );
	//    m_tMcuDscInfo.SetInnerIpMask( ntohl(INET_ADDR(DEFAULT_INNERIPMASK)) );
	//#endif	
	
	TMINIMCUNetParamAll tParamAddrAll;
	TMINIMCUNetParam	tParamAddr;
	memset( &tParamAddrAll, 0, sizeof(tParamAddrAll) );
#ifndef _VXWORKS_
	// MCU侧会控接入IP根据NIP设置
	TBrdEthParamAll tBrdEthParamAll;
	AgtGetBrdEthParamAll( 0, &tBrdEthParamAll );
	u8 byLop = 0;
	for ( byLop = 0; byLop < tBrdEthParamAll.dwIpNum; byLop++ )
	{
		memset( &tParamAddr, 0, sizeof(tParamAddr) );
		tParamAddr.SetLanParam( ntohl(tBrdEthParamAll.atBrdEthParam[byLop].dwIpAdrs),
			ntohl(tBrdEthParamAll.atBrdEthParam[byLop].dwIpMask),
			0);
		tParamAddrAll.AddOneNetParam( tParamAddr );
	}
	tDscInfo.SetMcsAccessAddrAll( tParamAddrAll );
	
	// DSC侧暂是设置为默认值，等DSC注册上来后再修改
	memset( &tParamAddrAll, 0, sizeof(tParamAddrAll) );
	memset( &tParamAddr, 0, sizeof(tParamAddr) );
	tParamAddr.SetLanParam( ntohl(INET_ADDR(DEFAULT_DSC_IP)), ntohl(INET_ADDR(DEFAULT_IPMASK)), 0);
	tParamAddrAll.AddOneNetParam( tParamAddr );
	tDscInfo.SetCallAddrAll( tParamAddrAll );
#endif
	//#ifdef WIN32
	//	memset( &tParamAddr, 0, sizeof(tParamAddr) );
	//	tParamAddr.SetLanParam( ntohl(GetLocalIp()), ntohl(INET_ADDR(DEFAULT_IPMASK)), ntohl(GetMpcGateway()) );
	//	tParamAddrAll.AddOneNetParam( tParamAddr );
	//	tDscInfo.SetMcsAccessAddrAll( tParamAddrAll );
	//
	//	memset( &tParamAddrAll, 0, sizeof(tParamAddrAll) );
	//	memset( &tParamAddr, 0, sizeof(tParamAddr) );
	//	tParamAddr.SetLanParam( ntohl(INET_ADDR(DEFAULT_DSC_IP)), ntohl(INET_ADDR(DEFAULT_IPMASK)), 0 );
	//	tParamAddrAll.AddOneNetParam( tParamAddr );
	//	tDscInfo.SetCallAddrAll( tParamAddrAll );
	//#endif
	wRet |= SetDSCInfo( &tDscInfo, TRUE );
#endif
	
	wRet |= SetVCSSoftName( VCS_DEFAULTSOFTNAME );
	
	//支持8000E
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	//8000E不支持配置向导
#if defined(_8KE_)
	wRet |= SetIsMcuConfiged(TRUE);
#endif
	TGKProxyCfgInfo tGkProxyCfgInfo;
	tGkProxyCfgInfo.SetGkIpAddr(0);
	tGkProxyCfgInfo.SetGkUsed(0);
	tGkProxyCfgInfo.SetProxyUsed(0);
	tGkProxyCfgInfo.SetProxyIpAddr(0);
	tGkProxyCfgInfo.SetProxyPort(0);
	wRet |= SetGkProxyCfgInfo(tGkProxyCfgInfo);
	
	TPrsTimeSpan tPrsTimeSpan;
	tPrsTimeSpan.m_wFirstTimeSpan  = 40;
	tPrsTimeSpan.m_wSecondTimeSpan = 120;
	tPrsTimeSpan.m_wThirdTimeSpan  = 240;
	tPrsTimeSpan.m_wRejectTimeSpan = 480;
	wRet |= SetPrsTimeSpanCfgInfo(tPrsTimeSpan);
#endif
    return wRet;
}
/*=============================================================================
函 数 名： SetVCSSoftName
功    能： 设置VCS软件名称
算法实现： 
全局变量： 
参    数： s8* pachSoftName
返 回 值： u16 
=============================================================================*/
u16 CCfgAgent::SetVCSSoftName(const s8* pachSoftName)
{
    ENTER( m_hMcuAllConfig );
	
    BOOL32 bResult = SetRegKeyString( m_achCfgName, SECTION_mcuVCS, 
		KEY_mcuVCSSoftName, pachSoftName);
    if( !bResult )
	{
		Agtlog(LOG_ERROR, "[SetVCSSoftName] Wrong profile while setting %s!\n", KEY_mcuVCSSoftName );
		return ERR_AGENT_WRITECFGFILE;
	}
	else
	{
		const s32 nSrcLen = strlen(pachSoftName) + 1;
		const s32 nDstLen = sizeof(m_achVCSSoftName);
		const s32 nActlen = min(nSrcLen, nDstLen);
		memcpy( m_achVCSSoftName, pachSoftName, nActlen );
	}
    return SUCCESS_AGENT;
}
/*=============================================================================
函 数 名： GetVCSSoftName
功    能： 取VCS软件名称信息
算法实现： 
全局变量： 
参    数： s8* pachSoftName(64字节)
返 回 值： void 
=============================================================================*/
void CCfgAgent::GetVCSSoftName(s8* pachSoftName)
{
    memcpy( pachSoftName, m_achVCSSoftName, sizeof(m_achVCSSoftName) );
    return;
}

/*=============================================================================
函 数 名： SetEqpExCfgInfo
功    能： 设外设扩展信息
算法实现： 
全局变量： 
参    数： IN TEqpExCfgInfo &tEqpExCfgInfo
返 回 值： u16 
=============================================================================*/
u16 CCfgAgent::SetEqpExCfgInfo( TEqpExCfgInfo &tEqpExCfgInfo )
{
	m_tEqpExCfgInfo = tEqpExCfgInfo;
	
	u16 wRet = SUCCESS_AGENT;
	wRet |= WriteIntToFile(SECTION_EqpExCfgInfo, KEY_HduIdleChlShowMode, (s32)m_tEqpExCfgInfo.m_tHDUExCfgInfo.m_byIdleChlShowMode);
    wRet |= WriteIntToFile(SECTION_EqpExCfgInfo, KEY_VmpIdleChlShowMode, (s32)m_tEqpExCfgInfo.m_tVMPExCfgInfo.m_byIdleChlShowMode);
	wRet |= WriteIntToFile(SECTION_EqpExCfgInfo, KEY_VmpIsDisplayMmbAlias, (s32)m_tEqpExCfgInfo.m_tVMPExCfgInfo.m_byIsDisplayMmbAlias);
    if ( SUCCESS_AGENT != wRet )
        printf("[SetEqpExCfgInfo] failed, wRet.%d \n", wRet );
	
    return wRet;
}

/*=============================================================================
函 数 名： GetEqpExCfgInfo
功    能： 得到外设扩展信息
算法实现： 
全局变量： 
参    数： OUT TEqpExCfgInfo &tEqpExCfgInfo
返 回 值： u16 
=============================================================================*/
u16 CCfgAgent::GetEqpExCfgInfo( TEqpExCfgInfo &tEqpExCfgInfo )
{
    u16 wRet = SUCCESS_AGENT;  
	
	tEqpExCfgInfo = m_tEqpExCfgInfo;
	
    return wRet;
}

/*=============================================================================
函 数 名： AgentGetEqpExCfgInfo
功    能： 得到外设扩展信息从文件中得到
算法实现： 
全局变量： 
参    数： OUT TEqpExCfgInfo &tEqpExCfgInfo
返 回 值： u16 
=============================================================================*/
BOOL32 CCfgAgent::AgentGetEqpExCfgInfo( const s8* lpszProfileName )
{
    if( NULL == lpszProfileName )
	{
		printf("[AgentGetLocalInfo] error input parameter.\n");
		return FALSE;
	}
	
	BOOL32 bSucceedRead = TRUE;
    s32  nValue = 0;
	BOOL32 bResult = 0;
    bResult = GetRegKeyInt( lpszProfileName, SECTION_EqpExCfgInfo, KEY_HduIdleChlShowMode, 0, &nValue );
    if( !bResult )
	{
		printf("[AgentGetEqpExCfgInfo] Wrong profile while reading %s!\n", KEY_mucId );
		bSucceedRead = FALSE;
	}
	m_tEqpExCfgInfo.m_tHDUExCfgInfo.m_byIdleChlShowMode = nValue;
	
	bResult = GetRegKeyInt( lpszProfileName, SECTION_EqpExCfgInfo, KEY_VmpIdleChlShowMode, 0, &nValue );
    if( !bResult )
	{
		printf("[AgentGetEqpExCfgInfo] Wrong profile while reading %s!\n", KEY_mucId );
		bSucceedRead = FALSE;
	}
	m_tEqpExCfgInfo.m_tVMPExCfgInfo.m_byIdleChlShowMode = nValue;
	
	bResult = GetRegKeyInt( lpszProfileName, SECTION_EqpExCfgInfo, KEY_VmpIsDisplayMmbAlias, 0, &nValue );
    if( !bResult )
	{
		printf("[AgentGetEqpExCfgInfo] Wrong profile while reading %s!\n", KEY_VmpIsDisplayMmbAlias );
		bSucceedRead = FALSE;
	}
	m_tEqpExCfgInfo.m_tVMPExCfgInfo.m_byIsDisplayMmbAlias = nValue;
	
	return bSucceedRead;
}

/*=============================================================================
函 数 名： SetConfigureVer
功    能： 设置配置文件版本信息
算法实现： 
全局变量： 
参    数： s8* achVersion
返 回 值： u16 
=============================================================================*/
u16 CCfgAgent::SetSystemCfgVer(s8* achVersion)
{
    ENTER( m_hMcuAllConfig );
    BOOL32 bResult = SetRegKeyString( m_achCfgName, SECTION_mcuSystem, 
		KEY_mcusysConfigVersion, achVersion);
    if( !bResult )
	{
		Agtlog(LOG_ERROR, "[SetConfigureVer] Wrong profile while reading %s!\n", KEY_mcusysConfigVersion );
		return ERR_AGENT_WRITECFGFILE;
	}
    return SUCCESS_AGENT;
}

/*=============================================================================
函 数 名： GetTrapServerNum
功    能： 取Trap服务器数
算法实现： 
全局变量： 
参    数： void
返 回 值： u8 
=============================================================================*/
u8 CCfgAgent::GetTrapServerNum(void)
{
    return (u8)m_dwTrapRcvEntryNum;
}

/*=============================================================================
函 数 名： GetTrapTarget
功    能： 取Trap信息
算法实现： 
全局变量： 
参    数： u8 byIndex
TTarget& tTrapTarget
返 回 值： void 
=============================================================================*/
void CCfgAgent::GetTrapTarget(u8 byIndex, TTarget& tTrapTarget)
{
    tTrapTarget.dwIp = m_atTrapRcvTable[byIndex].GetTrapIp();
    tTrapTarget.wPort = m_atTrapRcvTable[byIndex].GetTrapPort();
    memcpy( tTrapTarget.achCommunity, 
		m_atTrapRcvTable[byIndex].GetReadCom(), 
		strlen(m_atTrapRcvTable[byIndex].GetReadCom()) );
    return;
	
}

/*=============================================================================
函 数 名： SetMPCInfo
功    能： 设置MPC信息
算法实现： 
全局变量： 
参    数： TMPCInfo tMPCInfo
返 回 值： void 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
void CCfgAgent::SetMPCInfo( TMPCInfo tMPCInfo )
{
    memcpy( &m_tMPCInfo, &tMPCInfo, sizeof(TMPCInfo) );
    return;
}

/*=============================================================================
函 数 名： GetMPCInfo
功    能： 取MPC信息
算法实现： 
全局变量： 
参    数： TMPCInfo *ptMPCInfo
返 回 值： void 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/10/15  4.0         张宝卿       创建
=============================================================================*/
void CCfgAgent::GetMPCInfo( TMPCInfo *ptMPCInfo )
{
    memcpy( ptMPCInfo, &m_tMPCInfo, sizeof(TMPCInfo) );
    return;
}

/*=============================================================================
函 数 名： BrdStatusMcuVc2Agt
功    能： 单板状态值 业务转到代理
算法实现： 
全局变量： 
参    数： u8 byStatusIn
返 回 值： void 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/11/02  4.0         张宝卿       创建
=============================================================================*/
u8 CCfgAgent::BrdStatusMcuVc2Agt( u8 byStatusIn )
{
    u8 byStatusOut = 0;
    switch( byStatusIn )
    {
    case BOARD_INLINE:      byStatusOut = BOARD_STATUS_INLINE;  	break;
    case BOARD_OUTLINE:     byStatusOut = BOARD_STATUS_DISCONNECT;  break;
    case BOARD_CFGCONFLICT: byStatusOut = BOARD_STATUS_CFGCONFLICT; break;
    case BOARD_UNKNOW:      byStatusOut = BOARD_STATUS_UNKNOW;      break;
    default:                byStatusOut = BOARD_STATUS_UNKNOW;      break;
    }
    return byStatusOut;
}

/*=============================================================================
函 数 名： McuSubTypeAgt2Snmp
功    能： 
算法实现： 
全局变量： 
参    数： u8 byStatusIn
返 回 值： void 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/04/03  4.0         张宝卿       创建
=============================================================================*/
u8 CCfgAgent::McuSubTypeAgt2Snmp( u8 byMcuSubType )
{
    u8 bySnmpSubType = SNMP_MCUSUBTYPE_8000A;
	
    switch( byMcuSubType )
    {
		//  [1/21/2011 chendaiwei]支持MPC2
    case BRD_TYPE_MPC/*DSL8000_BRD_MPC*/:   
	case BRD_TYPE_MPC2:
		bySnmpSubType = SNMP_MCUSUBTYPE_8000A;  break;
    case KDV8000B_BOARD:    bySnmpSubType = SNMP_MCUSUBTYPE_8000B;  break;
		
		//8000g没有硬件ID
	case SNMP_MCUSUBTYPE_8000G:    bySnmpSubType = SNMP_MCUSUBTYPE_8000G;  break;

	case SNMP_MCUSUBTYPE_8000H:		bySnmpSubType = SNMP_MCUSUBTYPE_8000H;  break;
	case SNMP_MCUSUBTYPE_800L:     bySnmpSubType = SNMP_MCUSUBTYPE_800L;  break;
	case SNMP_MCUSUBTYPE_8000I:    bySnmpSubType = SNMP_MCUSUBTYPE_8000I;  break;
	case SNMP_MCUSUBTYPE_8000H_M:  bySnmpSubType = SNMP_MCUSUBTYPE_8000H_M; break;

#ifdef _LINUX_
#ifdef _LINUX12_
	case BRD_KDV8000BHD:  bySnmpSubType = SNMP_MCUSUBTYPE_8000B;  break;
#else
	case KDV8000C_BOARD:    bySnmpSubType = SNMP_MCUSUBTYPE_8000C;  break;
	case KDV8000BHD_BOARD:  bySnmpSubType = SNMP_MCUSUBTYPE_8000B;  break;
#endif
#endif
		
    default:                bySnmpSubType = SNMP_MCUSUBTYPE_8000A;  break;
    }
    return bySnmpSubType;
}

/*=============================================================================
函 数 名： GetSnmpBrdTypeFromHW
功    能： 
算法实现： 
全局变量： 
参    数： u8 byDSLType
返 回 值： u8 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/06/21  4.5         张宝卿       创建
=============================================================================*/
u8 CCfgAgent::GetSnmpBrdTypeFromHW(u8 byHWType)
{
	u8 byNmsBrdType = BRD_TYPE_UNKNOW;
	
	switch(byHWType)
	{
    case BRD_TYPE_MPC/*DSL8000_BRD_MPC*/:	byNmsBrdType = NMS_BRD_TYPE_MPC;		break;
    case BRD_TYPE_MPC2/*DSL8000_BRD_MPC*/:	byNmsBrdType = NMS_BRD_TYPE_MPC2;		break;
    case BRD_TYPE_DTI/*DSL8000_BRD_DTI*/:	byNmsBrdType = NMS_BRD_TYPE_DTI;		break;
    case BRD_TYPE_DIC/*DSL8000_BRD_DIC*/:	byNmsBrdType = NMS_BRD_TYPE_DIC;		break;
    case BRD_TYPE_CRI/*DSL8000_BRD_CRI*/:	byNmsBrdType = NMS_BRD_TYPE_CRI;		break;
	case BRD_TYPE_CRI2/*DSL8000_BRD_CRI*/:	byNmsBrdType = NMS_BRD_TYPE_CRI2;		break;
    case BRD_TYPE_VAS/*DSL8000_BRD_VAS*/:	byNmsBrdType = NMS_BRD_TYPE_VAS;		break;
    case BRD_TYPE_MMP/*DSL8000_BRD_MMP*/:	byNmsBrdType = NMS_BRD_TYPE_MMP;		break;
    case BRD_TYPE_DRI/*DSL8000_BRD_DRI*/:	byNmsBrdType = NMS_BRD_TYPE_DRI;		break;
	case BRD_TYPE_DRI2/*DSL8000_BRD_DRI*/:	byNmsBrdType = NMS_BRD_TYPE_DRI2;		break;
    case BRD_TYPE_IMT/*DSL8000_BRD_IMT*/:	byNmsBrdType = NMS_BRD_TYPE_IMT;		break;
    case BRD_TYPE_APU/*DSL8000_BRD_APU*/:	byNmsBrdType = NMS_BRD_TYPE_APU;		break;
    case BRD_TYPE_DSI/*DSL8000_BRD_DSI*/:	byNmsBrdType = NMS_BRD_TYPE_DSI;		break;
    case BRD_TYPE_VPU/*DSL8000_BRD_VPU*/:	byNmsBrdType = NMS_BRD_TYPE_VPU;		break;
    case BRD_TYPE_DEC5/*DSL8000_BRD_DEC5*/:	byNmsBrdType = NMS_BRD_TYPE_DEC5;		break;
	case BRD_TYPE_DRI16/*DSL8000_BRD_DRI16*/:	byNmsBrdType = NMS_BRD_TYPE_DRI16;  break;
    case BRD_TYPE_MDSC/*DSL8000_BRD_MDSC*/:	byNmsBrdType = NMS_BRD_TYPE_MDSC;		break;
    case BRD_TYPE_16E1/*DSL8000_BRD_16E1*/:	byNmsBrdType = NMS_BRD_TYPE_16E1;		break;
	case BRD_TYPE_HDSC/*DSL8000_BRD_HDSC*/:	byNmsBrdType = NMS_BRD_TYPE_HDSC;		break;
		//4.6 新增单板类型
    case BRD_TYPE_HDU/*BRD_HWID_KDM200_HDU*/:  byNmsBrdType = NMS_BRD_TYPE_HDU;		break;
    case BRD_TYPE_MPU/*BRD_HWID_DSL8000_MPU*/:	byNmsBrdType = NMS_BRD_TYPE_MPU;	break;
	case BRD_HWID_EBAP:		byNmsBrdType = NMS_BRD_TYPE_EBAP;		break;
	case BRD_HWID_EVPU:		byNmsBrdType = NMS_BRD_TYPE_EVPU;		break;
		
		//lukunpeng 由于目前BRD_HWID_EAPU大于u8，EAPU注册时用的0x19即BRD_TYPE_EAPU注册
	case BRD_TYPE_EAPU:		byNmsBrdType = NMS_BRD_TYPE_EAPU;		break;
	case BRD_TYPE_HDU_L:	byNmsBrdType = NMS_BRD_TYPE_HDU_L;      break;
	case BRD_TYPE_IS21:     byNmsBrdType = NMS_BRD_TYPE_IS21;       break;
	case BRD_TYPE_IS22:     byNmsBrdType = NMS_BRD_TYPE_IS22;       break;
	
	// [1/31/2012 chendaiwei]4.7版本
	case BRD_TYPE_HDU2:						   byNmsBrdType = NMS_BRD_TYPE_HDU2;	break;
	case BRD_TYPE_HDU2_L:					   byNmsBrdType = NMS_BRD_TYPE_HDU2_L;	break;
	case BRD_TYPE_HDU2_S:					   byNmsBrdType = NMS_BRD_TYPE_HDU2_S;	break;
	case BRD_TYPE_MPU2:						   byNmsBrdType = NMS_BRD_TYPE_MPU2;	break;
	case BRD_TYPE_MPU2ECARD:	               byNmsBrdType = NMS_BRD_TYPE_MPU2_ECARD;	break;
	case BRD_TYPE_APU2:						   byNmsBrdType = NMS_BRD_TYPE_APU2;	break;

	default:
		byNmsBrdType = BRD_TYPE_UNKNOW;
		Agtlog(LOG_ERROR, "[GetSnmpBrdTypeFromHW] unexpected brd dsl type.%d, ignore it\n", byHWType);
		break;
	}
	
	return byNmsBrdType;
}

u8 CCfgAgent::GetHWBrdTypeFromSnmp(u8 bySnmpType)
{
	u8 byHWBrdType = BRD_TYPE_UNKNOW;
	
	switch(bySnmpType)
	{
    case NMS_BRD_TYPE_MPC:	byHWBrdType = BRD_TYPE_MPC/*DSL8000_BRD_MPC*/;		break;
	case NMS_BRD_TYPE_MPC2:	byHWBrdType = BRD_TYPE_MPC2/*DSL8000_BRD_MPC*/;		break;
    case NMS_BRD_TYPE_DTI:	byHWBrdType = BRD_TYPE_DTI/*DSL8000_BRD_DTI*/;		break;
    case NMS_BRD_TYPE_DIC:	byHWBrdType = BRD_TYPE_DIC/*DSL8000_BRD_DIC*/;		break;
    case NMS_BRD_TYPE_CRI:	byHWBrdType = BRD_TYPE_CRI/*DSL8000_BRD_CRI*/;		break;
	case NMS_BRD_TYPE_CRI2:	byHWBrdType = BRD_TYPE_CRI2/*DSL8000_BRD_CRI*/;		break;
    case NMS_BRD_TYPE_VAS:	byHWBrdType = BRD_TYPE_VAS/*DSL8000_BRD_VAS*/;		break;
    case NMS_BRD_TYPE_MMP:	byHWBrdType = BRD_TYPE_MMP/*DSL8000_BRD_MMP*/;		break;
    case NMS_BRD_TYPE_DRI:	byHWBrdType = BRD_TYPE_DRI/*DSL8000_BRD_DRI*/;		break;
	case NMS_BRD_TYPE_DRI2:	byHWBrdType = BRD_TYPE_DRI2/*DSL8000_BRD_DRI*/;		break;
    case NMS_BRD_TYPE_IMT:	byHWBrdType = BRD_TYPE_IMT/*DSL8000_BRD_IMT*/;		break;
    case NMS_BRD_TYPE_APU:	byHWBrdType = BRD_TYPE_APU/*DSL8000_BRD_APU*/;		break;
    case NMS_BRD_TYPE_DSI:	byHWBrdType = BRD_TYPE_DSI/*DSL8000_BRD_DSI*/;		break;
    case NMS_BRD_TYPE_VPU:	byHWBrdType = BRD_TYPE_VPU/*DSL8000_BRD_VPU*/;		break;
    case NMS_BRD_TYPE_DEC5:	byHWBrdType = BRD_TYPE_DEC5/*DSL8000_BRD_DEC5*/;		break;
	case NMS_BRD_TYPE_DRI16:byHWBrdType = BRD_TYPE_DRI16/*DSL8000_BRD_DRI*/;		break;
    case NMS_BRD_TYPE_MDSC:	byHWBrdType = BRD_TYPE_MDSC/*DSL8000_BRD_MDSC*/;		break;
    case NMS_BRD_TYPE_16E1:	byHWBrdType = BRD_TYPE_16E1/*DSL8000_BRD_16E1*/;		break;
	case NMS_BRD_TYPE_HDSC:	byHWBrdType = BRD_TYPE_HDSC/*DSL8000_BRD_HDSC*/;		break;
		//4.6 新增单板类型
    case NMS_BRD_TYPE_HDU:  byHWBrdType = BRD_TYPE_HDU/*BRD_HWID_KDM200_HDU*/;	break;

    case NMS_BRD_TYPE_MPU:	byHWBrdType = BRD_TYPE_MPU/*BRD_HWID_DSL8000_MPU*/;	break;
	case NMS_BRD_TYPE_EBAP:	byHWBrdType = BRD_HWID_EBAP;		break;
	case NMS_BRD_TYPE_EVPU:	byHWBrdType = BRD_HWID_EVPU;		break;
		
		//lukunpeng 由于目前BRD_HWID_EAPU大于u8，EAPU注册时用的0x19即BRD_TYPE_EAPU注册
	case NMS_BRD_TYPE_EAPU:	byHWBrdType = BRD_TYPE_EAPU;		break;
	case NMS_BRD_TYPE_HDU_L: byHWBrdType = BRD_TYPE_HDU_L;      break;
	case NMS_BRD_TYPE_IS21: byHWBrdType = BRD_TYPE_IS21;       break;
	case NMS_BRD_TYPE_IS22: byHWBrdType = BRD_TYPE_IS22;       break;

	// 4.7版本 [1/31/2012 chendaiwei]
	case NMS_BRD_TYPE_HDU2: byHWBrdType = BRD_TYPE_HDU2;						break;
	case NMS_BRD_TYPE_HDU2_L: byHWBrdType = BRD_TYPE_HDU2_L;	break;
	case NMS_BRD_TYPE_HDU2_S: byHWBrdType = BRD_TYPE_HDU2_S;	break;
	case NMS_BRD_TYPE_MPU2:	byHWBrdType = BRD_TYPE_MPU2;	break;
	case NMS_BRD_TYPE_MPU2_ECARD:	byHWBrdType = BRD_TYPE_MPU2ECARD;	break;
	case NMS_BRD_TYPE_APU2:	byHWBrdType = BRD_TYPE_APU2;	break;
	default:
		byHWBrdType = BRD_TYPE_UNKNOW;
		Agtlog(LOG_ERROR, "[GetHWBrdTypeFromSnmp] unexpected brd dsl type.%d, ignore it\n", bySnmpType);
		break;
	}
	
	return byHWBrdType;
}

u8 CCfgAgent::GetMcuBrdTypeFromSnmp(u8 byNmsType)
{
	u8 byMcuBrdType = BRD_TYPE_UNKNOW;
	
	switch(byNmsType)
	{
    case NMS_BRD_TYPE_MPC:	byMcuBrdType = BRD_TYPE_MPC;		break;
	case NMS_BRD_TYPE_MPC2: byMcuBrdType = BRD_TYPE_MPC2;       break;
    case NMS_BRD_TYPE_DTI:	byMcuBrdType = BRD_TYPE_DTI;		break;
    case NMS_BRD_TYPE_DIC:	byMcuBrdType = BRD_TYPE_DIC;		break;
    case NMS_BRD_TYPE_CRI:	byMcuBrdType = BRD_TYPE_CRI;		break;
	case NMS_BRD_TYPE_CRI2:	byMcuBrdType = BRD_TYPE_CRI2;		break;
    case NMS_BRD_TYPE_VAS:	byMcuBrdType = BRD_TYPE_VAS;		break;
    case NMS_BRD_TYPE_MMP:	byMcuBrdType = BRD_TYPE_MMP;		break;
    case NMS_BRD_TYPE_DRI:	byMcuBrdType = BRD_TYPE_DRI;		break;
	case NMS_BRD_TYPE_DRI2:	byMcuBrdType = BRD_TYPE_DRI2;		break;
    case NMS_BRD_TYPE_IMT:	byMcuBrdType = BRD_TYPE_IMT;		break;
    case NMS_BRD_TYPE_APU:	byMcuBrdType = BRD_TYPE_APU;		break;
    case NMS_BRD_TYPE_DSI:	byMcuBrdType = BRD_TYPE_DSI;		break;
    case NMS_BRD_TYPE_VPU:	byMcuBrdType = BRD_TYPE_VPU;		break;
    case NMS_BRD_TYPE_DEC5:	byMcuBrdType = BRD_TYPE_DEC5;		break;
	case NMS_BRD_TYPE_DRI16:byMcuBrdType = BRD_TYPE_DRI16;		break;
    case NMS_BRD_TYPE_MDSC:	byMcuBrdType = BRD_TYPE_MDSC;		break;
    case NMS_BRD_TYPE_16E1:	byMcuBrdType = BRD_TYPE_16E1;		break;
	case NMS_BRD_TYPE_HDSC:	byMcuBrdType = BRD_TYPE_HDSC;		break;
		//4.6 新增单板类型
    case NMS_BRD_TYPE_HDU:  byMcuBrdType = BRD_TYPE_HDU;		break;
    case NMS_BRD_TYPE_MPU:	byMcuBrdType = BRD_TYPE_MPU;		break;
	case NMS_BRD_TYPE_EBAP:	byMcuBrdType = BRD_TYPE_EBAP;		break;
	case NMS_BRD_TYPE_EVPU:	byMcuBrdType = BRD_TYPE_EVPU;		break;
	case NMS_BRD_TYPE_EAPU:	byMcuBrdType = BRD_TYPE_EAPU;		break;
	case NMS_BRD_TYPE_HDU_L:  byMcuBrdType = BRD_TYPE_HDU_L;	break;
	case NMS_BRD_TYPE_IS21: byMcuBrdType = BRD_TYPE_IS21;       break;
	case NMS_BRD_TYPE_IS22: byMcuBrdType = BRD_TYPE_IS22;       break;

	// 4.7版本 [1/31/2012 chendaiwei]
	case NMS_BRD_TYPE_MPU2:	byMcuBrdType = BRD_TYPE_MPU2;		break;
	case NMS_BRD_TYPE_MPU2_ECARD:	byMcuBrdType = BRD_TYPE_MPU2ECARD;		break;
	case NMS_BRD_TYPE_HDU2: byMcuBrdType = BRD_TYPE_HDU2;		break;
	case NMS_BRD_TYPE_HDU2_L: byMcuBrdType = BRD_TYPE_HDU2_L;	break;
	case NMS_BRD_TYPE_HDU2_S: byMcuBrdType = BRD_TYPE_HDU2_S;	break;
	case NMS_BRD_TYPE_APU2: byMcuBrdType = BRD_TYPE_APU2;		break;

	default:
		byMcuBrdType = BRD_TYPE_UNKNOW;
		Agtlog(LOG_ERROR, "[GetMcuBrdTypeFromSnmp] unexpected brd dsl type.%d, ignore it\n", byNmsType);
		break;
	}
	
	return byMcuBrdType;
}

/*=============================================================================
函 数 名： GetSnmpBrdTypeFromMcu
功    能： 
算法实现： 
全局变量： 
参    数： u8 byVCType
返 回 值： u8 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2009/05/12  4.5         张宝卿       创建
=============================================================================*/
u8 CCfgAgent::GetSnmpBrdTypeFromMcu(u8 byMcuType)
{
    u8 byNmsBrdType = UNKNOWN_BOARD;
    
    switch(byMcuType)
    {
    case BRD_TYPE_MPC:	byNmsBrdType = NMS_BRD_TYPE_MPC;		break;
	case BRD_TYPE_MPC2: byNmsBrdType = NMS_BRD_TYPE_MPC2;       break;
    case BRD_TYPE_DTI:	byNmsBrdType = NMS_BRD_TYPE_DTI;		break;
    case BRD_TYPE_DIC:	byNmsBrdType = NMS_BRD_TYPE_DIC;		break;
    case BRD_TYPE_CRI:	byNmsBrdType = NMS_BRD_TYPE_CRI;		break;
    case BRD_TYPE_CRI2:	byNmsBrdType = NMS_BRD_TYPE_CRI2;		break;
    case BRD_TYPE_VAS:	byNmsBrdType = NMS_BRD_TYPE_VAS;		break;
    case BRD_TYPE_MMP:	byNmsBrdType = NMS_BRD_TYPE_MMP;		break;
    case BRD_TYPE_DRI:	byNmsBrdType = NMS_BRD_TYPE_DRI;		break;
	case BRD_TYPE_DRI2:	byNmsBrdType = NMS_BRD_TYPE_DRI2;		break;
    case BRD_TYPE_IMT:	byNmsBrdType = NMS_BRD_TYPE_IMT;		break;
    case BRD_TYPE_APU:	byNmsBrdType = NMS_BRD_TYPE_APU;		break;
    case BRD_TYPE_DSI:	byNmsBrdType = NMS_BRD_TYPE_DSI;		break;
    case BRD_TYPE_VPU:	byNmsBrdType = NMS_BRD_TYPE_VPU;		break;
    case BRD_TYPE_DEC5:	byNmsBrdType = NMS_BRD_TYPE_DEC5;	    break;
    case BRD_TYPE_MDSC:	byNmsBrdType = NMS_BRD_TYPE_MDSC;	    break;
    case BRD_TYPE_16E1:	byNmsBrdType = NMS_BRD_TYPE_16E1;		break;
    case BRD_TYPE_HDSC:	byNmsBrdType = NMS_BRD_TYPE_HDSC;	    break;
		
    case BRD_TYPE_MPU:  byNmsBrdType = NMS_BRD_TYPE_MPU;		break;
    case BRD_TYPE_EBAP: byNmsBrdType = NMS_BRD_TYPE_EBAP;       break;
    case BRD_TYPE_EVPU: byNmsBrdType = NMS_BRD_TYPE_EVPU;       break;
	case BRD_TYPE_EAPU: byNmsBrdType = NMS_BRD_TYPE_EAPU;       break;
    case BRD_TYPE_HDU:  byNmsBrdType = NMS_BRD_TYPE_HDU;		break;
	case BRD_TYPE_HDU_L:  byNmsBrdType = NMS_BRD_TYPE_HDU_L;    break;
	case BRD_TYPE_IS21: byNmsBrdType = NMS_BRD_TYPE_IS21;       break;
	case BRD_TYPE_IS22: byNmsBrdType = NMS_BRD_TYPE_IS22;       break;
	
	//4.7版本[1/31/2012 chendaiwei]
	case BRD_TYPE_MPU2:  byNmsBrdType = NMS_BRD_TYPE_MPU2;		break;
	case BRD_TYPE_MPU2ECARD:  byNmsBrdType = NMS_BRD_TYPE_MPU2_ECARD;		break;
    case BRD_TYPE_HDU2: byNmsBrdType = NMS_BRD_TYPE_HDU2;		break;
	case BRD_TYPE_HDU2_L: byNmsBrdType = NMS_BRD_TYPE_HDU2_L;	break;
	case BRD_TYPE_HDU2_S: byNmsBrdType = NMS_BRD_TYPE_HDU2_S;	break;

    case BRD_TYPE_APU2: byNmsBrdType = NMS_BRD_TYPE_APU2;		break;

    default:
        Agtlog(LOG_VERBOSE, "[GetSnmpBrdTypeFromMcu] unexpected brd dsl type.%d, ignore it\n", byMcuType);
        byNmsBrdType = BRD_TYPE_UNKNOW;
        break;
    }
    
	return byNmsBrdType;
}

/*=============================================================================
函 数 名： BrdStatusAgt2McuVc
功    能： 单板状态值 代理转到业务
算法实现： 
全局变量： 
参    数： u8 byStatusIn
返 回 值： void 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/11/02  4.0         张宝卿       创建
=============================================================================*/
u8 CCfgAgent::BrdStatusAgt2McuVc( u8 byStatusIn )
{
    u8 byStatusOut = 0;
    switch( byStatusIn )
    {
    case BOARD_STATUS_INLINE:       byStatusOut = BOARD_INLINE;    	    break;
    case BOARD_STATUS_DISCONNECT:   byStatusOut = BOARD_OUTLINE;   	    break;
    case BOARD_STATUS_CFGCONFLICT:  byStatusOut = BOARD_CFGCONFLICT;   	break;
    case BOARD_STATUS_UNKNOW:       byStatusOut = BOARD_UNKNOW;         break;
    default:                        byStatusOut = BOARD_UNKNOW;         break;
    }
    return byStatusOut;
}

/*=============================================================================
函 数 名： BrdStatusHW2Agt
功    能： 单板状态值 底层转到代理
算法实现： 
全局变量： 
参    数： u8 byStatusIn
返 回 值： void 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/11/02  4.0         张宝卿       创建
=============================================================================*/
u8 CCfgAgent::BrdStatusHW2Agt( u8 byStatusIn )
{
    u8 byStatusOut = 0;
    switch( byStatusIn )
    {
    case BRD_MPC_IN_POSITION:    byStatusOut = BOARD_STATUS_INLINE;  	break;
    case BRD_MPC_OUTOF_POSITION: byStatusOut = BOARD_STATUS_DISCONNECT;  break;
    default:                     byStatusOut = BOARD_STATUS_DISCONNECT;  break;
    }
    return byStatusOut;
}

/*=============================================================================
函 数 名： BrdStatusAgt2NMS
功    能： 单板状态值 代理转到NMS
算法实现： 
全局变量： 
参    数： u8 byStatusIn
返 回 值： void 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/11/24  4.0         张宝卿       创建
=============================================================================*/
u8 CCfgAgent::BrdStatusAgt2NMS( u8 byStatusIn )
{
    u8 byStatusOut = 0;
    switch( byStatusIn )
    {
    case BOARD_STATUS_INLINE:       byStatusOut = NMS_BOARD_ONLINE;     break;
    case BOARD_STATUS_DISCONNECT:   byStatusOut = NMS_BOARD_OFFLINE;    break;
    case BOARD_STATUS_CFGCONFLICT:  byStatusOut = NMS_BOARD_OFFLINE;    break;
    case BOARD_STATUS_UNKNOW:       byStatusOut = NMS_BOARD_OFFLINE;    break;
    default:                        byStatusOut = NMS_BOARD_OFFLINE;    break;
    }
    return byStatusOut;
}

/*=============================================================================
函 数 名： PanelLedStatusAgt2NMS
功    能： 灯状态值 代理转到NMS(只读)
算法实现： 
全局变量： 
参    数： TEqpBrdCfgEntry& tBrdCfg 单板配置表项
           s8* pszLedStr            网口状态
返 回 值： u8 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2012/05/09  4.7         liaokang      创建
=============================================================================*/
u8 CCfgAgent::PanelLedStatusAgt2NMS( TEqpBrdCfgEntry& tBrdCfg, s8* pszLedStr )
{
    if(NULL == pszLedStr)
    {
        return 0;
    }

    u8 byLedCount = 0;
    u8 byLoop = 0;
    s8 achTempled[ MAX_BOARD_LED_NUM + 1 ];
    s8 achExistled[ MAX_BOARD_LED_NUM + 1 ];
    memset( achTempled, 0, sizeof(achTempled) );
    memset( achExistled, 0, sizeof(achExistled) );
    memcpy( achExistled, tBrdCfg.GetPanelLed(), sizeof(achExistled) );

    if ( 0 == strlen(achExistled) )
    {
        memcpy( pszLedStr, achExistled, sizeof(achExistled) );
        return byLedCount;
    }
    
    byLedCount = strlen( achExistled );

    switch( tBrdCfg.GetType() )
    {
    case BRD_TYPE_IS22:
        // 重新排序
        byLedCount = 32;
        /*achExistled不包括5v,12v. 
        0-2 ALM;MLNK;G2  (IS2.2 8548)
        3-9:00-06;        (IS2.2 8313)
        10-16:08-14;      (IS2.2 8313)
        17:run;           (IS2.2 8313)
        18-19:mc0-mc1;    (IS2.2 8313)
        20-27:e0-e7;      (IS2.2 8313)
        28-29:G0-G1       (IS2.2 8313)*/
//      achTempled[0] = 2;  5V led 置灭          achTempled[1] = 2;  12V led 置灭
//      achTempled[2] = achExistled[3];     00   achTempled[3] = achExistled[17];  run
//      achTempled[4] = achExistled[4];     01   achTempled[5] = achExistled[18];  mc0
//      achTempled[6] = achExistled[5];     02   achTempled[7] = achExistled[19];  mc1      
//      achTempled[8] = achExistled[6];     03   achTempled[9] = achExistled[20];  E0
//      achTempled[10] = achExistled[7];    04   achTempled[11] = achExistled[21]; E1
//      achTempled[12] = achExistled[8];    05   achTempled[13] = achExistled[22]; E2
//      achTempled[14] = achExistled[9];    06   achTempled[15] = achExistled[23]; E3
//      achTempled[16] = achExistled[10];   08   achTempled[17] = achExistled[24]; E4
//      achTempled[18] = achExistled[11];   09   achTempled[19] = achExistled[25]; E5
//      achTempled[20] = achExistled[12];   10   achTempled[21] = achExistled[26]; E6
//      achTempled[22] = achExistled[13];   11   achTempled[23] = achExistled[27]; E7
//      achTempled[24] = achExistled[14];   12   achTempled[25] = achExistled[28]; G0
//      achTempled[26] = achExistled[15];   13   achTempled[27] = achExistled[29]; G1
//      achTempled[28] = achExistled[16];   14   achTempled[29] = achExistled[2];  G2
//      achTempled[30] = achExistled[1];    MLNK achTempled[31] = achExistled[0];  ALM
        // 5v 12v
        for ( byLoop = 0; byLoop < 2; byLoop++)
        {
            achTempled[byLoop] = 2;
        }
        // 00-06 08-14
        for ( byLoop = 0; byLoop < 14; byLoop++)
        {
            achTempled[byLoop*2 + 2] = achExistled[byLoop + 3];
        }
        // run mc0-mc1 e0-e7 G0-G1
        for ( byLoop = 0; byLoop < 13; byLoop++)
        {
            achTempled[byLoop*2 + 3] = achExistled[byLoop + 17];
        }
        // G2 MLNK ALM
        for ( byLoop = 0; byLoop < 3; byLoop++)
        {
            achTempled[byLoop + 29] = achExistled[ 2 - byLoop];
        }
//         OspPrintf(TRUE,FALSE,"\n\nexist led 2:%d,led 1:%d,led 0:%d\n\n\n",
//             achExistled[2],achExistled[1],achExistled[0]);
        memcpy( pszLedStr, achTempled, byLedCount);
        break;      
    default: 
		memcpy( pszLedStr, achExistled, byLedCount );
        break;
    }
    
    return byLedCount; 
}

/*=============================================================================
函 数 名： EthPortStatusAgt2NMS
功    能： 网口状态值 代理转到NMS(只读)
算法实现： 
全局变量： 
参    数： TEqpBrdCfgEntry& tBrdCfg 单板配置表项
           s8* pszEthPortStr        网口状态
返 回 值： u8 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2012/05/09  4.7         liaokang      创建
=============================================================================*/
u8 CCfgAgent::EthPortStatusAgt2NMS( TEqpBrdCfgEntry& tBrdCfg, s8* pszEthPortStr )
{
    if(NULL == pszEthPortStr)
    {
        return 0;
    }

    u8 byEthPortBufSize = 0;
    
    s8 achTempEthPort[ MAX_BOARD_ETHPORT_NUM+1 ];
    s8 achExistEthPort[ MAX_BOARD_ETHPORT_NUM+1 ];
    memset( achTempEthPort, 0, sizeof(achTempEthPort) );
    memset( achExistEthPort, 0, sizeof(achExistEthPort) );
    memcpy( achExistEthPort, tBrdCfg.GetEthPort(), sizeof(achExistEthPort) );
    
    if ( 0 == strlen(achExistEthPort) )
    {
        memcpy( pszEthPortStr, achExistEthPort, sizeof(achExistEthPort) );
        return byEthPortBufSize;
    }
    
    switch( tBrdCfg.GetType() )
    {
    case BRD_TYPE_IS22:
        {
            // 分隔符'~'(s8) + '~'(s8) + 状态类型(u8) +前网口数目(u8) + 前网口状态(3个) + 后网口数目(u8) + 后网口状态(8个)
            // NMS侧 双工 表示：1 半双工 2 全双工 3 未知
            byEthPortBufSize = 16;
            achTempEthPort[0]  = '~';   // 分隔符
            achTempEthPort[1]  = '~';   // 分隔符
            achTempEthPort[2]  = 1;     // 状态类型
            achTempEthPort[3]  = 3;     // 前网口数目
            achTempEthPort[4]  = achExistEthPort[0];     // 前网口 G0
            achTempEthPort[5]  = achExistEthPort[1];     // 前网口 G1
            achTempEthPort[6]  = achExistEthPort[2];     // 前网口 G2
            achTempEthPort[7]  = 8;     // 后网口数目
            achTempEthPort[8]  = achExistEthPort[3];     // 后网口 LNK0
            achTempEthPort[9]  = achExistEthPort[4];     // 后网口 LNK1
            achTempEthPort[10] = achExistEthPort[5];     // 后网口 LNK2
            achTempEthPort[11] = achExistEthPort[6];     // 后网口 LNK3
            achTempEthPort[12] = achExistEthPort[7];     // 后网口 LNK4
            achTempEthPort[13] = achExistEthPort[8];     // 后网口 LNK5
            achTempEthPort[14] = achExistEthPort[9];     // 后网口 LNK6
            achTempEthPort[15] = achExistEthPort[10];    // 后网口 LNK7
            memcpy( pszEthPortStr, achTempEthPort, sizeof(achTempEthPort) );
        }
        break;      
    default:
        break;
    }
    
    return byEthPortBufSize;
}   

/*=============================================================================
函 数 名： GetHdBasCfgFromMpuBas
功    能： 
算法实现： 
全局变量： 
参    数： u8 byStatusIn
返 回 值： void 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/11/24  4.0         张宝卿       创建
=============================================================================*/
void CCfgAgent::GetHdBasCfgFromMpuBas(TEqpBasHDInfo &tHdBasInfo,
                                      TEqpMpuBasInfo &tMpuBasInfo)
{
    tHdBasInfo.SetAlias(tMpuBasInfo.GetAlias());
    tHdBasInfo.SetEqpId(tMpuBasInfo.GetEqpId());
    tHdBasInfo.SetEqpRecvPort(tMpuBasInfo.GetEqpRecvPort());
    if (TYPE_MPUBAS == tMpuBasInfo.GetStartMode())
	{
		tHdBasInfo.SetHDBasType(TYPE_MPU);
	}
	else if (TYPE_MPUBAS_H == tMpuBasInfo.GetStartMode())
	{
		tHdBasInfo.SetHDBasType(TYPE_MPU_H);
	}
	else if (TYPE_MPU2BAS_BASIC == tMpuBasInfo.GetStartMode())
	{
		tHdBasInfo.SetHDBasType(TYPE_MPU2_BASIC);
	}
	else if (TYPE_MPU2BAS_ENHANCED == tMpuBasInfo.GetStartMode())
	{
		tHdBasInfo.SetHDBasType(TYPE_MPU2_ENHANCED);
	}
	else if (TYPE_APU2BAS == tMpuBasInfo.GetStartMode())
	{
		tHdBasInfo.SetHDBasType(TYPE_APU2_BAS);
	}
	else 
	{
		OspPrintf(TRUE, FALSE, "[GetHdBasCfgFromMpuBas] unexpected mpu<EqpId:%d> startmode:%d!\n", tMpuBasInfo.GetEqpId(),
			tMpuBasInfo.GetStartMode());
	}
    tHdBasInfo.SetIpAddr(tMpuBasInfo.GetIpAddr());
    tHdBasInfo.SetMcuRecvPort(tMpuBasInfo.GetMcuRecvPort());
    tHdBasInfo.SetSwitchBrdId(tMpuBasInfo.GetSwitchBrdId());
	
    return;
}

/*=============================================================================
函 数 名： WriteTableEntry
功    能： 写表中某一项
算法实现： 
全局变量： 
参    数： s8* achFileName：文件名
s8* achTableName：表名
s8* achEntry：行号
s8* achConfInfo：值
返 回 值： u16 
=============================================================================*/
u16 CCfgAgent::WriteTableEntry(const s8* achFileName, const s8* achTableName, const s8* achEntry, const s8* achConfInfo, BOOL32 bSemTake /* = TRUE */)
{
    u16  wRet  = SUCCESS_AGENT;
    BOOL32 bTakeSucceed = TRUE;
	
    if( bSemTake )
    {
        if( OspSemTakeByTime( m_hMcuAllConfig, WAIT_SEM_TIMEOUT ) != TRUE )
        {
            bTakeSucceed = FALSE;
            Agtlog( LOG_ERROR, "[WriteTableEntry] m_hMcuAllConfig take failed !\n" );
        }
    }
	
    BOOL32 bRet = SetRegKeyString( achFileName, achTableName, achEntry, achConfInfo);
    
    if( bSemTake && bTakeSucceed )
    {
        OspSemGive( m_hMcuAllConfig );
    }
    
    if( !bRet )
    {
        wRet = ERR_AGENT_WRITEFILE;
    }
    return wRet;
}

/*=============================================================================
函 数 名： WriteTableEntryNum
功    能： 写表的EntryNum项
算法实现： 
全局变量： 
参    数： s8* achFileName：文件名
s8* achTableName：表名
s8* achEntryNumStr：行号
u8 byEntryNum：值
返 回 值： u16 
=============================================================================*/
u16 CCfgAgent::WriteTableEntryNum(s8* achFileName, s8* achTableName, s8* achEntryNumStr, u8 byEntryNum)
{
    u16 wRet = SUCCESS_AGENT;
    ENTER( m_hMcuAllConfig );
	
    BOOL32 bRet = SetRegKeyInt( achFileName, achTableName, achEntryNumStr, (s32)byEntryNum );
    if ( !bRet ) 
    {
        wRet = ERR_AGENT_WRITEFILE;
    }
    return wRet;
}

/*=============================================================================
函 数 名： GetMcuLayerCount
功    能： 取mcu 中的总的层数
算法实现： 
全局变量： 
参    数： void
返 回 值： u8 
=============================================================================*/
u8 CCfgAgent::GetMcuLayerCount()
{
    u8 byLayerCount = 0;
    for(u8 byIndex = 0; byIndex < m_dwBrdCfgEntryNum; byIndex++)
    {
        u8 byBoardLayer = m_atBrdCfgTable[byIndex].GetLayer();
        byLayerCount = (byLayerCount < byBoardLayer) ? byBoardLayer : byLayerCount;
    }
    byLayerCount = byLayerCount + 1; // 层号从0开始
    Agtlog(LOG_VERBOSE, "The Mcu Layer: %d\n", byLayerCount);
    return byLayerCount;
}

/*=============================================================================
函 数 名： GetBrdInstIdByPos
功    能： 根据板位置算出其所在实例ID
算法实现： 
全局变量： 
参    数： TBrdPosition &tBrdPos
返 回 值： u8 
=============================================================================*/
u8 CCfgAgent::GetBrdInstIdByPos(const TBrdPosition tBrdPos)
{
    if ( BRD_TYPE_HDU/*BRD_HWID_KDM200_HDU*/ != tBrdPos.byBrdID 
		&& BRD_TYPE_HDU_L/*BRD_PID_KDM200_HDU_L*/ != tBrdPos.byBrdID
		&& BRD_TYPE_HDU2 != tBrdPos.byBrdID
		&& BRD_TYPE_HDU2_L != tBrdPos.byBrdID 	// 4.6 jlb
		&& BRD_TYPE_HDU2_S != tBrdPos.byBrdID)
    {
		return ( tBrdPos.byBrdLayer<<4 | (tBrdPos.byBrdSlot+1) );
    }
    else
    {
#ifndef _MINIMCU_
        return ( 65 + tBrdPos.byBrdLayer * 7 + tBrdPos.byBrdSlot );
#else
		return ( 16 + tBrdPos.byBrdLayer * 7 + tBrdPos.byBrdSlot );
#endif
        
    }
}

/*=============================================================================
函 数 名： GetRunBrdTypeByIdx
功    能： 通过板的层槽组合索引查询运行板类型
算法实现： 
全局变量： 
参    数： u8 byBrdIdx
返 回 值： u8 
=============================================================================*/
u8 CCfgAgent::GetRunBrdTypeByIdx( u8 byBrdId)
{
    u8 byLayer = 0;
    u8 bySlot = 0;
    
    // 示例： ID: 14: 第一层, 第四槽
    // 2005-10-26 modify
    if (MCU_BOARD_MPC != byBrdId && MCU_BOARD_MPCD != byBrdId )
    {
        byLayer = byBrdId >> 4;
        bySlot = (byBrdId & 0x0f);
    }
    
    u32 dwLoop = 0;
    for( ; dwLoop < m_dwBrdCfgEntryNum; dwLoop++ )
    {
        // MPC
		//  [1/21/2011 chendaiwei]支持MPC2
        if( BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ == m_atBrdCfgTable[dwLoop].GetType() ||
			BRD_TYPE_MPC2 == m_atBrdCfgTable[dwLoop].GetType() )
        {
            // 2005-10-26 modify
            if ((MCU_BOARD_MPC == byBrdId && 0 == m_atBrdCfgTable[dwLoop].GetSlot()) ||
                (MCU_BOARD_MPCD == byBrdId && 1 == m_atBrdCfgTable[dwLoop].GetSlot()) )
            {
                //return BRD_TYPE_MPC/*DSL8000_BRD_MPC*/;
				return m_atBrdCfgTable[dwLoop].GetType();
            }
            continue;
        }
        // EX板
        else
        {
            if( m_atBrdCfgTable[dwLoop].GetSlot() == (bySlot - 1) && 
                m_atBrdCfgTable[dwLoop].GetLayer() == byLayer )
            {
                return m_atBrdCfgTable[dwLoop].GetType();
            }
        }
    }
	
    if( dwLoop >= m_dwBrdCfgEntryNum )
    {
        Agtlog(LOG_WARN, "[GetRunBrdTypeByIdx] The board Id.%d is not exist while getting its Type.\n", byBrdId);
    }
	
	return 0xFF;
}

/*=============================================================================
函 数 名： IsMcuPdtBrdMatch
功    能： 检查业务与MPC板类型是否匹配
算法实现： 
全局变量： 
参    数： 
返 回 值： 
=============================================================================*/
/*lint -save -e527*/
BOOL32 CCfgAgent::IsMcuPdtBrdMatch(u8 byPdtType) const
{
	
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	BOOL32 bRet = (byPdtType == MCU_TYPE_KDV8000E || byPdtType == MCU_TYPE_KDV8000H || byPdtType == MCU_TYPE_KDV800L || byPdtType == MCU_TYPE_KDV8000H_M || byPdtType == MCU_TYPE_KDV8000I) ? TRUE: FALSE;
	printf("pdtType.%d IsMcuPdtBrdMatch: %d\n",  byPdtType, bRet);
	return bRet;
#endif
	
#ifndef WIN32    
    u32 dwBrdId = BrdGetBoardID();
    switch ( dwBrdId )  
    {
#ifdef _LINUX12_
    case BRD_HWID_MPC2:		
#else
    case BRD_TYPE_MPC/*DSL8000_BRD_MPC*/:		
#endif
		if ( byPdtType != MCU_TYPE_KDV8000 )
		{
			return FALSE;
		}
		break;
    case KDV8000B_BOARD:
        if ( byPdtType != MCU_TYPE_KDV8000B )
        {
            return FALSE;
        }
        break;
#ifndef _VXWORKS_
#ifdef _LINUX12_
	case BRD_KDV8000BHD:
#else
	case KDV8000BHD_BOARD:
#endif
		if ( byPdtType != MCU_TYPE_KDV8000B_HD )
		{
			return FALSE;
		}
		break;
#endif
#ifdef _LINUX_
#ifndef _LINUX12_
    case KDV8000C_BOARD:
        if ( byPdtType != MCU_TYPE_KDV8000C )
        {
            return FALSE;
        }
        break;
#endif
#endif
    default:
        return FALSE;
    }
#endif
    
    return TRUE;
}
/*lint -restore*/

/*=============================================================================
函 数 名： PreTreatMpcNIP
功    能： 
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/7/30   4.0			周广程                  创建
=============================================================================*/
void CCfgAgent::PreTreatMpcNIP(void)
{
	
#ifdef _MINIMCU_
	//#ifndef WIN32 
	STATUS nRet = OK;
	TBrdEthParam tEthParam;
	memset( &tEthParam, 0, sizeof(tEthParam) );
	AgtGetBrdEthParam( 0, &tEthParam );
	printf( "[PreTreatMpcNIP]Get eth0, Ip=0x%x, Mask=0x%x\n",tEthParam.dwIpAdrs, tEthParam.dwIpMask);
	if ( 0 == tEthParam.dwIpAdrs )
	{
		memset( &tEthParam, 0, sizeof(tEthParam) );
		tEthParam.dwIpAdrs = INET_ADDR(DEFAULT_MCU_IP);
		tEthParam.dwIpMask = INET_ADDR(DEFAULT_IPMASK);
		printf("[PreTreatMpcNIP]Set eth0, Ip=0x%x, Mask=0x%x\n",tEthParam.dwIpAdrs, tEthParam.dwIpMask);
		nRet = BrdSetEthParam( 0, Brd_SET_IP_AND_MASK, &tEthParam );
		if ( ERROR == nRet )
		{
			printf("[PreTreatMpcNIP]Set eth0 Ip failed!\n");
		}
	}
	memset( &tEthParam, 0, sizeof(tEthParam) );
	AgtGetBrdEthParam( 1, &tEthParam );
	printf( "[PreTreatMpcNIP]Get eth1, Ip=0x%x, Mask=0x%x\n",tEthParam.dwIpAdrs, tEthParam.dwIpMask);
	if ( 0 == tEthParam.dwIpAdrs )
	{
		memset( &tEthParam, 0, sizeof(tEthParam) );
		tEthParam.dwIpAdrs = INET_ADDR(DEFAULT_MCU_INNERIP);
		tEthParam.dwIpMask = INET_ADDR(DEFAULT_INNERIPMASK);
		printf( "[PreTreatMpcNIP]Set eth1, Ip=0x%x, Mask=0x%x\n",tEthParam.dwIpAdrs, tEthParam.dwIpMask);
		nRet = BrdSetEthParam( 1, Brd_SET_IP_AND_MASK, &tEthParam );
		if ( ERROR == nRet )
		{
			printf("[PreTreatMpcNIP]Set eth1 Ip failed!\n");
		}
	}
	return; 
	//#endif
#elif defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	// [4/13/2010 xliang] FIXME:
#ifdef _LINUX_
	EthInit(); //初始化网口状态，优先用eth0
#endif
#else
	//#ifndef WIN32
	STATUS nRet = OK;
	TBrdEthParam tEthParam;
	memset( &tEthParam, 0, sizeof(tEthParam) );
	AgtGetBrdEthParam( 0, &tEthParam );
	BOOL32 bIsSetIpEth0 = ( 0 == tEthParam.dwIpAdrs ) ? FALSE : TRUE;
	memset( &tEthParam, 0, sizeof(tEthParam) );
	AgtGetBrdEthParam( 1, &tEthParam );
	BOOL32 bIsSetIpEth1 = ( 0 == tEthParam.dwIpAdrs ) ? FALSE : TRUE;
	if ( bIsSetIpEth0 || bIsSetIpEth1 )
	{
		return;
	}
	else
	{
		memset( &tEthParam, 0, sizeof(tEthParam) );
		tEthParam.dwIpAdrs = INET_ADDR(DEFAULT_MCU_IP);
		tEthParam.dwIpMask = INET_ADDR(DEFAULT_IPMASK);
        // 默认配后网口地址
		nRet = BrdSetEthParam( 1, Brd_SET_IP_AND_MASK, &tEthParam );
		if ( ERROR == nRet )
		{
			printf("[PreTreatMpcNIP]Set eth1 Ip failed!\n");
		}
		return;
	}
	//#endif
#endif
}

/*=============================================================================
函 数 名： AgtGetRegKeyStringTable
功    能： 
算法实现： 
全局变量： 
参    数： const s8* lpszProfileName,    
const s8* lpszSectionName
const s8* lpszDefault
s8* *lpszEntryArray
u32 *dwEntryNum
u32 dwBufSize
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/2/27  4.0			周广程                  创建
=============================================================================*/
BOOL32 CCfgAgent::AgtGetRegKeyStringTable( const s8* lpszProfileName,    
										  const s8* lpszSectionName,      
										  const s8* lpszDefault,     
										  s8* *const lpszEntryArray, 
										  u32 *pdwEntryNum, 
										  u32 dwBufSize 
										  )
{
	u32 dwEntryNum = *pdwEntryNum;
	BOOL32 bResult = GetRegKeyStringTable( lpszProfileName,    
		lpszSectionName,      
		lpszDefault,     
		lpszEntryArray, 
		pdwEntryNum, 
		dwBufSize 
		);
	bResult &= ( dwEntryNum == *pdwEntryNum ) ? TRUE : FALSE;
	return bResult;
}

/*=============================================================================
函 数 名： BrdGetDstMcuNode
功    能： 取本地Mcu结点号(用于8000b启动内置mp/mix/bas/vmp)
算法实现： 
全局变量： 
参    数： void
返 回 值： API u32 
=============================================================================*/
API u32 BrdGetDstMcuNode( void )
{
    return 0;
}

/*=============================================================================
函 数 名： BrdGetDstMcuNodeB
功    能： 取本地Mcu结点号(用于8000b启动内置mp/mix/bas/vmp)
算法实现： 
全局变量： 
参    数： void
返 回 值： API u32 
=============================================================================*/
API u32 BrdGetDstMcuNodeB(void)
{
    return 0;
}

/*=============================================================================
函 数 名： ShowRecInfo
功    能： 显示录像机信息
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
=============================================================================*/
void CCfgAgent::ShowRecInfo(void)             
{
	if( m_dwRecEntryNum != 0)
	{
		OspPrintf(TRUE, FALSE, "[m_atRecTable]\n");
		for( u8 byIndex = 0; byIndex < (u8)m_dwRecEntryNum; byIndex++)
		{
			OspPrintf(TRUE, FALSE, "%d\t%d\t%d\t%s \t%x\t%d\n", 
				m_atRecTable[byIndex].GetEqpId(),
				m_atRecTable[byIndex].GetMcuRecvPort(),
				m_atRecTable[byIndex].GetSwitchBrdId(),
				m_atRecTable[byIndex].GetAlias(),
				m_atRecTable[byIndex].GetIpAddr(),
				m_atRecTable[byIndex].GetEqpRecvPort() );
		}
		OspPrintf(TRUE,FALSE,"\n");
	}

	if (m_dwVrsRecEntryNum != 0)
	{
		OspPrintf(TRUE, FALSE, "[m_atVrsRecTable]\n");
		for( u8 byIndex = 0; byIndex < (u8)m_dwVrsRecEntryNum; byIndex++)
		{
			m_atVrsRecTable[byIndex].Print();
		}
		OspPrintf(TRUE,FALSE,"\n");
	}

    return;
}

/*=============================================================================
函 数 名： ShowNetsyncInfo
功    能： 显示网同步信息
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
=============================================================================*/
void CCfgAgent::ShowNetsyncInfo(void)
{
    OspPrintf(TRUE, FALSE, "mode: %d\t DTSlot: %d\tE1Index: %d\n", 
		m_tNetSync.GetMode(), 
		m_tNetSync.GetDTSlot(), 
		m_tNetSync.GetE1Index());
    return;
}    

/*=============================================================================
函 数 名： ShowPrsInfo
功    能： 显示Prs信息
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
=============================================================================*/
void CCfgAgent::ShowPrsInfo(void)
{
    OspPrintf(TRUE, FALSE, "\t\tThe Prs information\n");
    OspPrintf(TRUE, FALSE, "  id\tmcu port  switch id  alias  run id  prs port first second third reject\n");
    for(u8 byIndex = 0; byIndex < m_dwPrsEntryNum; byIndex++)
    {
        OspPrintf(TRUE, FALSE, "  %d   %d      %d      %s      %d      %d      %d    %d     %d     %d\n", 
			m_atPrsTable[byIndex].GetEqpId(),
			m_atPrsTable[byIndex].GetMcuRecvPort(),
			m_atPrsTable[byIndex].GetSwitchBrdId(),
			m_atPrsTable[byIndex].GetAlias(),
			m_atPrsTable[byIndex].GetRunBrdId(),
			m_atPrsTable[byIndex].GetEqpRecvPort(),
			m_atPrsTable[byIndex].GetFirstTimeSpan(),
			m_atPrsTable[byIndex].GetSecondTimeSpan(),
			m_atPrsTable[byIndex].GetThirdTimeSpan(),
			m_atPrsTable[byIndex].GetRejectTimeSpan() );
    }
    return;
}

//4.6版本新加 jlb   ------------   start -------------------
/*=============================================================================
函 数 名： ShowHduInfo
功    能： 显示Hdu信息
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
void   CCfgAgent::ShowHduInfo( void )  
{
    OspPrintf(TRUE, FALSE, "[ShowHduInfo] The Hdu number is: %d\n", m_dwHduEntryNum);
    OspPrintf(TRUE, FALSE, "ID\t Alias \t Run Id\tStart port\n");
	THduChnlModePortAgt tHduChnModePort1, tHduChnModePort2;
	memset(&tHduChnModePort1, 0x0, sizeof(tHduChnModePort1));
	memset(&tHduChnModePort2, 0x0, sizeof(tHduChnModePort2));
	
    for(u8 byIndex = 0; byIndex < (u8)m_dwHduEntryNum; byIndex++)
    {
        m_atHduTable[byIndex].GetHduChnlModePort(0, tHduChnModePort1);
		m_atHduTable[byIndex].GetHduChnlModePort(1, tHduChnModePort2);
        OspPrintf(TRUE, FALSE, "%d\t%s \t%d\t%d\t%d\t%d\t%d\t%d\t%d", 
			m_atHduTable[byIndex].GetEqpId(),
			m_atHduTable[byIndex].GetAlias(),
			m_atHduTable[byIndex].GetRunBrdId(),
			m_atHduTable[byIndex].GetEqpRecvPort(),
			tHduChnModePort1.GetOutModeType(),
			tHduChnModePort1.GetOutPortType(),
			tHduChnModePort2.GetOutModeType(),
			tHduChnModePort2.GetOutPortType(),
			m_atHduTable[byIndex].GetStartMode()
			);
    }
    return;
}

/*=============================================================================
函 数 名： ShowSvmpInfo
功    能： 显示Svmp信息
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
void   CCfgAgent::ShowSvmpInfo( void )
{
    OspPrintf(TRUE, FALSE, "[ShowSvmpInfo] The Svmp number is: %d\n", m_dwVMPEntryNum);
	
    OspPrintf(TRUE, FALSE, "ID    Port   Switch Id  Alias  Run Id  Start port  \n");
	
    for(u8 byIndex = 0; byIndex < (u8)m_dwVMPEntryNum; byIndex++)
    {
        OspPrintf(TRUE, FALSE, "%d\t%d\t%d\t%s \t%d\t%d\n", 
			m_atSvmpTable[byIndex].GetEqpId(),
			m_atSvmpTable[byIndex].GetMcuRecvPort(),
			m_atSvmpTable[byIndex].GetSwitchBrdId(),
			m_atSvmpTable[byIndex].GetAlias(),
			m_atSvmpTable[byIndex].GetRunBrdId(),
			m_atSvmpTable[byIndex].GetEqpRecvPort() );
    }
    return;
}



/*=============================================================================
函 数 名： ShowMpuBasInfo
功    能： 显示MpuBas信息
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/12/11  4.6         江乐斌       创建
=============================================================================*/
void   CCfgAgent::ShowMpuBasInfo( void )
{
    OspPrintf(TRUE, FALSE, "[ShowMpuBasInfo] The MpuBas number is: %d\n", m_dwMpuBasEntryNum);
    OspPrintf(TRUE, FALSE, "ID   Port\tSwitch Id    Alias  Run Id  Start port\n");
    for(u8 byIndex = 0; byIndex < (u8)m_dwMpuBasEntryNum; byIndex++)
    {
        OspPrintf(TRUE, FALSE, "%d\t%d\t%d\t%s \t%d\t%d\n", 
			m_atMpuBasTable[byIndex].GetEqpId(),
			m_atMpuBasTable[byIndex].GetMcuRecvPort(),
			m_atMpuBasTable[byIndex].GetSwitchBrdId(),
			m_atMpuBasTable[byIndex].GetAlias(),
			m_atMpuBasTable[byIndex].GetRunBrdId(),
			m_atMpuBasTable[byIndex].GetEqpRecvPort() );
    }
    return;
}

/*=============================================================================
函 数 名： ShowMauBasInfo
功    能： 显示MauBas信息
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2009/05/12  4.6         张宝卿       创建
=============================================================================*/
void CCfgAgent::ShowMauBasInfo( void )
{
    OspPrintf(TRUE, FALSE, "[ShowMauBasInfo] The MpuBas number is: %d\n", m_dwBasHDEntryNum);
    OspPrintf(TRUE, FALSE, "ID   Port\tSwitch Id    Alias  Run Id  Start port\n");
    for(u8 byIndex = 0; byIndex < (u8)m_dwBasHDEntryNum; byIndex++)
    {
        OspPrintf(TRUE, FALSE, "%d\t%d\t%d\t%s \t%d\t%d\n", 
			m_atBasHDTable[byIndex].GetEqpId(),
			m_atBasHDTable[byIndex].GetMcuRecvPort(),
			m_atBasHDTable[byIndex].GetSwitchBrdId(),
			m_atBasHDTable[byIndex].GetAlias(),
			0,
			m_atBasHDTable[byIndex].GetEqpRecvPort() );
    }
    return;
}


//---------------------    4.6新加  end  ----------------------
/*=============================================================================
函 数 名： ShowNetInfo
功    能： 显示网络信息
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
=============================================================================*/
void CCfgAgent::ShowNetInfo(void)
{
    OspPrintf(TRUE, FALSE, "MPC m_byMpcPortChoice: %d, MPC m_bMpcEnabled: %d\n", 
		m_tMPCInfo.GetLocalPortChoice(), m_tMPCInfo.GetIsLocalMaster());
	m_tMcuNetwork.Print();
    return;
}
/*=============================================================================
函 数 名： ShowBoardInfo
功    能： 显示Board信息
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
=============================================================================*/
void CCfgAgent::ShowBoardInfo(void)    //fixme    增加新加板子的信息显示hdu, mpu, ebap, evpu
{
    OspPrintf(TRUE, FALSE, "[ShowBoardInfo] The board number is: %d\n", m_dwBrdCfgEntryNum);
	
    s8 achTypeStr[100][12] = {"mpc", "dti", "dic", "cri", "vas", "mmp", "dri","imt", "apu", "dsi", 
		"vpu", "dec5", "vri","dsc", "8005", "dri16", "mdsc", "16e1", "8003", "", "hdsc"};
    
    strncpy(&achTypeStr[70][0], "hdu", sizeof("hdu"));
    strncpy(&achTypeStr[22][0], "mpu", sizeof("mpu"));
	
	// 增加新板卡显示 [12/6/2011 chendaiwei]
	strncpy(&achTypeStr[37][0], "hdu2", sizeof("hdu2"));
	strncpy(&achTypeStr[38][0], "mpu2", sizeof("mpu2"));
	strncpy(&achTypeStr[39][0], "mpu2_ecard", sizeof("mpu2_ecard"));

	strncpy(&achTypeStr[40][0], "apu2", sizeof("apu2"));
	strncpy(&achTypeStr[41][0], "hdu2-L", sizeof("hdu2-L"));

    OspPrintf(TRUE, FALSE, "BrdId\tL/S\tType \tIp \t\tEthInt/Cast\tOSType\tStatus\tAlias\n");
    for(u8 byIndex = 0; byIndex < (u8)m_dwBrdCfgEntryNum; byIndex++)
    {
        OspPrintf(TRUE, FALSE, "%d\t%d/%d\t%s\t%x\t%d/%d\t\t%d\t%d\t%s \n", 
			m_atBrdCfgTable[byIndex].GetBrdId(),
			m_atBrdCfgTable[byIndex].GetLayer(),
			m_atBrdCfgTable[byIndex].GetSlot(),
			achTypeStr[m_atBrdCfgTable[byIndex].GetType()], 
			m_atBrdCfgTable[byIndex].GetBrdIp(),
			m_atBrdCfgTable[byIndex].GetPortChoice(),
			m_atBrdCfgTable[byIndex].GetCastChoice(),
			m_atBrdCfgTable[byIndex].GetOSType(),
			m_atBrdCfgTable[byIndex].GetState(),
			m_atBrdCfgTable[byIndex].GetAlias());
    }

    return;
}
/*=============================================================================
函 数 名： ShowVmpInfo
功    能： 显示Vmp信息
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
=============================================================================*/
void CCfgAgent::ShowVmpInfo(void)
{
	if(m_dwVMPEntryNum != 0)
	{
		OspPrintf(TRUE, FALSE, "[m_atVMPTable]\n");
		OspPrintf(TRUE, FALSE, "ID    Port   Switch Id           Alias        Run Id  Start port  Encode number\n");
		
		for(u8 byIndex = 0; byIndex < (u8)m_dwVMPEntryNum; byIndex++)
		{
			OspPrintf(TRUE, FALSE, "%d   %d\t%d\t%17s \t%d\t%d\t\t%d\t%d\n", 
				m_atVMPTable[byIndex].GetEqpId(),
				m_atVMPTable[byIndex].GetMcuRecvPort(),
				m_atVMPTable[byIndex].GetSwitchBrdId(),
				m_atVMPTable[byIndex].GetAlias(),
				m_atVMPTable[byIndex].GetRunBrdId(),
				m_atVMPTable[byIndex].GetEqpRecvPort(),
				m_atVMPTable[byIndex].GetEncodeNum(),
				m_atVMPTable[byIndex].GetIpAddr());
		}
		OspPrintf(TRUE, FALSE, "\n");
	}

	if(m_dwSvmpEntryNum != 0)
	{
		OspPrintf(TRUE, FALSE, "[m_atSvmpTable]\n");
		OspPrintf(TRUE, FALSE, "ID    Port   Switch Id           Alias       Run Id  Start port\n");
		for(u8 byIdx = 0; byIdx < (u8)m_dwSvmpEntryNum; byIdx++)
		{
			OspPrintf(TRUE, FALSE, "%d   %d\t%d\t%17s \t%d\t%d\n", 
				m_atSvmpTable[byIdx].GetEqpId(),
				m_atSvmpTable[byIdx].GetMcuRecvPort(),
				m_atSvmpTable[byIdx].GetSwitchBrdId(),
				m_atSvmpTable[byIdx].GetAlias(),
				m_atSvmpTable[byIdx].GetRunBrdId(),
				m_atSvmpTable[byIdx].GetEqpRecvPort() );
		}
		OspPrintf(TRUE, FALSE, "\n");
	}

    return;
}

/*=============================================================================
函 数 名： ShowMpwInfo
功    能： 显示Mpw信息
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
=============================================================================*/
void CCfgAgent::ShowMpwInfo(void)
{
    OspPrintf(TRUE, FALSE, "[ShowBoardInfo] The Mpw number is: %d\n", m_dwMpwEntryNum);
    OspPrintf(TRUE, FALSE, "ID   Alias    Run Id   Start Port\n");
    for(u8 byIndex = 0; byIndex < (u8)m_dwMpwEntryNum; byIndex++)
    {
        OspPrintf(TRUE, FALSE, "%d\t%s \t%d\t%d\n", 
			m_atMpwTable[byIndex].GetEqpId(),
			m_atMpwTable[byIndex].GetAlias(),
			m_atMpwTable[byIndex].GetRunBrdId(),
			m_atMpwTable[byIndex].GetEqpRecvPort() );
    }
    return;
}
/*=============================================================================
函 数 名： ShowTvInfo
功    能： 显示TvWall信息
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
=============================================================================*/
void CCfgAgent::ShowTvInfo(void)            
{
    OspPrintf(TRUE, FALSE, "[ShowBoardInfo] The Tv Wall number is: %d\n", m_dwTVWallEntryNum);
    OspPrintf(TRUE, FALSE, "ID\t Alias \t Run Id\tStart port\n");
    for(u8 byIndex = 0; byIndex < (u8)m_dwTVWallEntryNum; byIndex++)
    {
        OspPrintf(TRUE, FALSE, "%d\t%s \t%d\t%d\n", 
			m_atTVWallTable[byIndex].GetEqpId(),
			m_atTVWallTable[byIndex].GetAlias(),
			m_atTVWallTable[byIndex].GetRunBrdId(),
			m_atTVWallTable[byIndex].GetEqpRecvPort() );
    }
    return;
}

/*=============================================================================
函 数 名： ShowMixerInfo
功    能： 显示Mixer信息
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
=============================================================================*/
void CCfgAgent::ShowMixerInfo(void)        
{
	if(m_dwMixEntryNum != 0)
	{
		OspPrintf(TRUE, FALSE, "[m_atMixTable]\n");
		OspPrintf(TRUE, FALSE, "ID   Port\tSwitch Id         Alias      Run Id  Start port  MixChnl   IsRedund\n");
		for(u8 byIndex = 0; byIndex < (u8)m_dwMixEntryNum; byIndex++)
		{
			OspPrintf(TRUE, FALSE, "%d    %d\t%d\t%17s \t%d\t%d\t  %d\t%d\t%d\n", 
				m_atMixTable[byIndex].GetEqpId(),
				m_atMixTable[byIndex].GetMcuRecvPort(),
				m_atMixTable[byIndex].GetSwitchBrdId(),
				m_atMixTable[byIndex].GetAlias(),
				m_atMixTable[byIndex].GetRunBrdId(),
				m_atMixTable[byIndex].GetEqpRecvPort(),
				m_atMixTable[byIndex].GetMaxChnInGrp(),
				m_atMixTable[byIndex].IsSendRedundancy(),
				m_atMixTable[byIndex].GetIpAddr());
		}
		OspPrintf(TRUE,FALSE,"\n");
	}

    return;
}

/*=============================================================================
函 数 名： ShowBasInfo
功    能： 显示Bas信息
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
=============================================================================*/
void CCfgAgent::ShowBasInfo(void)           
{
	u8 byIndex = 0;
	if(m_dwBasEntryNum != 0 )
	{
		OspPrintf(TRUE, FALSE, "[m_atBasTable]\n");
		OspPrintf(TRUE, FALSE, "ID   Port\tSwitch Id           Alias   Run Id  Start port\n");

		for( byIndex= 0; byIndex < (u8)m_dwBasEntryNum; byIndex++)
		{
			OspPrintf(TRUE, FALSE, "%d\t%d\t%d\t%17s \t%d\t%d\n", 
				m_atBasTable[byIndex].GetEqpId(),
				m_atBasTable[byIndex].GetMcuRecvPort(),
				m_atBasTable[byIndex].GetSwitchBrdId(),
				m_atBasTable[byIndex].GetAlias(),
				m_atBasTable[byIndex].GetRunBrdId(),
				m_atBasTable[byIndex].GetEqpRecvPort() );
		}
		OspPrintf(TRUE,FALSE,"\n");
	}

	if(m_dwBasHDEntryNum != 0)
	{
		OspPrintf(TRUE, FALSE, "[m_atBasHDTable]\n");
		OspPrintf(TRUE, FALSE, "ID      Port\tSwitch Id             Alias   Run Id  Start port\n");
		for( byIndex = 0; byIndex < (u8)m_dwBasHDEntryNum; byIndex++)
		{
			OspPrintf(TRUE, FALSE, "%d\t%d\t%d\t%17s \t%d\n", 
				m_atBasHDTable[byIndex].GetEqpId(),
				m_atBasHDTable[byIndex].GetMcuRecvPort(),
				m_atBasHDTable[byIndex].GetSwitchBrdId(),
				m_atBasHDTable[byIndex].GetAlias(),
				m_atBasHDTable[byIndex].GetEqpRecvPort() );
		}
		OspPrintf(TRUE,FALSE,"\n");
	}

	if(m_dwMpuBasEntryNum != 0)
	{
		OspPrintf(TRUE, FALSE, "[m_atMpuBasTable]\n");
		OspPrintf(TRUE, FALSE, "ID      Port\tSwitch Id            Alias    Run Id  Start port\n");
		for( byIndex = 0; byIndex < (u8)m_dwMpuBasEntryNum; byIndex++)
		{
			OspPrintf(TRUE, FALSE, "%d\t%d\t%d\t%17s \t%d\t%d\n", 
				m_atMpuBasTable[byIndex].GetEqpId(),
				m_atMpuBasTable[byIndex].GetMcuRecvPort(),
				m_atMpuBasTable[byIndex].GetSwitchBrdId(),
				m_atMpuBasTable[byIndex].GetAlias(),
				m_atMpuBasTable[byIndex].GetRunBrdId(),
				m_atMpuBasTable[byIndex].GetEqpRecvPort() );
		}
		OspPrintf(TRUE,FALSE,"\n");
	}

    return;
}

/*=============================================================================
函 数 名： ShowDcsInfo
功    能： 显示DcsIp 
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
=============================================================================*/
void CCfgAgent::ShowDcsInfo(void)
{
    OspPrintf(TRUE, FALSE, "The Dcs Ip: %0x\n", m_tDCS.m_dwDcsIp);
    return;
}

/*=============================================================================
函 数 名： ShowLocalInfo
功    能： 显示本地信息
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
=============================================================================*/
void CCfgAgent::ShowLocalInfo(void)
{
    m_tMcuLocalInfo.Print();
    m_tMPCInfo.Print();
	OspPrintf( TRUE, FALSE, "[CfgInfoLevel] %d\n", m_tMcuSystem.GetMcuCfgInfoLevel() );
	OspPrintf( TRUE, FALSE, "[mcuEncodingType] %d\n", GetEncodingType() );
	OspPrintf( TRUE, FALSE, "[Mcu IsUtf8Encoding] %d\n", IsUtf8Encoding() );
    return;
}

/*=============================================================================
函 数 名： ShowQosInfo
功    能： 显示Qos信息
算法实现： 
全局变量： 
参    数： 
返 回 值： void 
=============================================================================*/
void CCfgAgent::ShowQosInfo(void)
{
    m_tMcuQosInfo.Print();
    return;
}

/*=============================================================================
函 数 名： ShowVmpAttachInfo
功    能： 显示VmpAttach信息
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
=============================================================================*/
void CCfgAgent::ShowVmpAttachInfo(void)
{
    OspPrintf(TRUE, FALSE, "\t The Vmp Attach info, Num: %d\n", m_dwVmpAttachNum);
    OspPrintf(TRUE, FALSE, "\t Index BlackGroup Frame  Speaker  ChairFrame");
    OspPrintf(TRUE, FALSE, " Font Size Text TopicBG Diaphaneity Alias\n");
    for(u8 byIndex = 0; byIndex < m_dwVmpAttachNum; byIndex++)
    {
        OspPrintf(TRUE, FALSE, "\t  %d    %d   %d   %d   %d", 
			m_atVmpAttachCfg[byIndex].GetIndex(),
			m_atVmpAttachCfg[byIndex].GetBGDColor(),
			m_atVmpAttachCfg[byIndex].GetFrameColor(),
			m_atVmpAttachCfg[byIndex].GetSpeakerFrameColor(),
			m_atVmpAttachCfg[byIndex].GetChairFrameColor());
		OspPrintf(TRUE, FALSE, "  %d  %d    %d    %d    %d  %s \n", 
			m_atVmpAttachCfg[byIndex].GetFontType(),
			m_atVmpAttachCfg[byIndex].GetFontSize() ,
			m_atVmpAttachCfg[byIndex].GetTextColor() ,
			m_atVmpAttachCfg[byIndex].GetTopicBGDColor() ,
			m_atVmpAttachCfg[byIndex].GetDiaphaneity(),
			m_atVmpAttachCfg[byIndex].GetAlias() );
    }
    return;
}

/*=============================================================================
函 数 名： ShowDscInfo
功    能： 显示8000B DCS板信息
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
=============================================================================*/
void CCfgAgent::ShowDscInfo(void)
{
    m_tMcuDscInfo.Print();
    return;
}

/*=============================================================================
函 数 名： ShowSemHandleInfo
功    能： 显示信号量句柄值
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
=============================================================================*/
void CCfgAgent::ShowSemHandleInfo(void)
{
    OspPrintf( TRUE, FALSE, "Config Info Handle: 0x%x \n", m_hMcuAllConfig );
    g_cAlarmProc.ShowSemHandle();
    g_cBrdManagerApp.ShowSemHandle();
    return;
}

// mcu start optimize, zgc, 2007-02-27
/*=============================================================================
函 数 名： CopyCfgSrcfileToDstfile
功    能： 
算法实现： 
全局变量： 
参    数：  const s8* lpszSrcfilename
const s8* lpszDstfilename
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/16  4.0			周广程                  创建
=============================================================================*/
BOOL32 CCfgAgent::CopyCfgSrcfileToDstfile( const s8* lpszSrcfilename, const s8* lpszDstfilename )
{
	printf( "[McuAgt] Copy file %s to %s!\n", lpszSrcfilename, lpszDstfilename );
	
	u8  abyCfgContent[MAXLEN_MCUCFG_INI];
    u32 dwCfgFileLen = MAXLEN_MCUCFG_INI;
	
	memset( abyCfgContent, 0, sizeof(abyCfgContent) );
	
	// 读配置文件
	FILE *hSrcfile  = fopen(lpszSrcfilename, "r");
	s32 nFileLen = 0;
	s32 nlen = 0;
	if (NULL == hSrcfile)
	{
		printf( "[McuAgt][CopySrcfileToDstfile] The srcfile open failed!\n" );
		return FALSE;
	}
	else
	{
		fseek(hSrcfile, 0, SEEK_END);
		nFileLen = ftell(hSrcfile);
		if (nFileLen > (s32)dwCfgFileLen)
		{
			printf( "[McuAgt][CopySrcfileToDstfile] The srcfile size if over limit!\n" );
			return FALSE;
		}
		if (nFileLen > 0)
		{
			fseek(hSrcfile, 0, SEEK_SET);
			nlen = fread(abyCfgContent, 1, nFileLen, hSrcfile);
			if( nlen < nFileLen && !feof(hSrcfile) )
			{
				printf( "[McuAgt][CopySrcfileToDstfile] The srcfile read failed!\n" );
				return FALSE;
			}
		}
		fclose(hSrcfile);
		hSrcfile = NULL;		
	}
	
	// 写备份文件
	nlen = 0;
	FILE *hDstfile  = fopen(lpszDstfilename, "w");
	if (NULL == hDstfile)
	{
		printf( "[McuAgt][CopySrcfileToDstfile] The dstfile open failed!\n" );
		return FALSE;
	}
	else
	{
		if (nFileLen > 0)
		{
			fseek(hDstfile, 0, SEEK_SET);
			nlen = fwrite(abyCfgContent, 1, nFileLen, hDstfile);
			if( nlen < nFileLen && !feof(hDstfile) )
			{
				printf( "[McuAgt][CopySrcfileToDstfile] The dstfile write failed!\n" );
				return FALSE;
			}
		}
		fclose(hDstfile);
		hDstfile = NULL;
	}
	
	return TRUE;
}

/*=============================================================================
函 数 名： CopyFileBetweenCfgAndBak
功    能： 
算法实现： 
全局变量： 
参    数：  const s8* lpszSrcfilename
const s8* lpszDstfilename
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/20  4.0			周广程                  创建
=============================================================================*/
BOOL32 CCfgAgent::CopyFileBetweenCfgAndBak( const s8* lpszSrcfilename, const s8* lpszDstfilename )
{
	BOOL32 bRet = TRUE;
	
	OspSemTakeByTime( m_hMcuAllConfig, WAIT_SEM_TIMEOUT );
	
	bRet = CopyCfgSrcfileToDstfile( lpszSrcfilename, lpszDstfilename );			
	if( !bRet )
	{
		OspPrintf( TRUE, FALSE, "[AgentInit] Agent copy file failed!\n" );
	}
	
	OspSemGive( m_hMcuAllConfig );
	
	return bRet;
}
/*=============================================================================
函 数 名： PreReadCfgfile
功    能： 
算法实现： 
全局变量： 
参    数： const s8* lpszCfgfilename
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/16  4.0			周广程                  创建
=============================================================================*/
BOOL32 CCfgAgent::PreReadCfgfile( const s8* lpszCfgfilename )
{
	if( NULL == lpszCfgfilename )
	{
		printf("[PreReadCfgfile] error input parameter.\n");
		return FALSE;
	}
	
	FILE* hOpen = NULL;
	if(NULL == (hOpen = fopen(m_achCfgName, "r"))) 
    {
        printf("[PreReadCfgfile] Pre read file is not exist.\n");
		return FALSE;
    }
	else
	{
		fclose( hOpen );
	}
	
	BOOL32 bSucceedRead = TRUE;
	BOOL32 bResult = TRUE;
	s8   achReturn[1024];
    memset(achReturn, '\0', sizeof(achReturn));
	s8   achDefStr[] = "Error string";
	s32  nValue = 0;
	
	// system info
	bSucceedRead &= GetRegKeyString( lpszCfgfilename, SECTION_mcuSystem, KEY_mcusysConfigVersion, achDefStr, achReturn, MAX_CONFIGURE_LENGTH );
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcuSystem, KEY_mcuIsConfiged, 0, &nValue );
	if( !bSucceedRead )
	{
		printf("[McuAgt][PreReadCfgfile] The system info is error!\n");
		return bSucceedRead;
	}
	
	// local info
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcuLocalInfo, KEY_mucId, 0, &nValue );
	bSucceedRead &= GetRegKeyString( lpszCfgfilename, SECTION_mcuLocalInfo, KEY_mcuAlias, achDefStr, achReturn, MAXLEN_ALIAS );
	bSucceedRead &= GetRegKeyString( lpszCfgfilename, SECTION_mcuLocalInfo, KEY_mcuE164Number, achDefStr, achReturn, MAXLEN_E164 );
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcuLocalInfo, Key_mcuCheckLinkTime, 0, &nValue );
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcuLocalInfo, KEY_mcuCheckMtLinkTimes, 0, &nValue );
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcuLocalInfo, Key_mcuRefreshListTime, 0, &nValue );
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcuLocalInfo, Key_mcuAudioRefreshTime, 0, &nValue );
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcuLocalInfo, Key_mcuVideoRefreshTime, 0, &nValue );
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcuLocalInfo, Key_mcuIsSaveBand, 0, &nValue );
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcuLocalInfo, Key_mcuIsNPlusMode, 0, &nValue );
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcuLocalInfo, Key_mcuIsNPlusBackupMode, 0, &nValue );
	bSucceedRead &= GetRegKeyString( lpszCfgfilename, SECTION_mcuLocalInfo, Key_mcuNPlusMcuIp, achDefStr, achReturn, MAXLEN_ALIAS );
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcuLocalInfo, Key_mcuNPlusRtdTime, DEF_NPLUSRTD_TIME, &nValue );
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcuLocalInfo, Key_mcuNPlusRtdNum, DEF_NPLUSRTD_NUM, &nValue );
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcuLocalInfo, Key_mcuIsNPlusRollBack, 1, &nValue );	
	if( !bSucceedRead )
	{
		printf("[McuAgt][PreReadCfgfile] The local info is error!\n");
		return bSucceedRead;
	}
	
	// net work info
	bSucceedRead &= GetRegKeyString( lpszCfgfilename, SECTION_mcuNetwork, KEY_mcunetGKIpAddr, achDefStr, achReturn, MAX_VALUE_LEN + 1 );
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcuNetwork, KEY_mcunetGKCharge, 0, &nValue );
	bSucceedRead &= GetRegKeyString( lpszCfgfilename, SECTION_mcuNetwork, KEY_mcunetRRQMtadpIp, achDefStr, achReturn, MAX_VALUE_LEN + 1 );
	bSucceedRead &= GetRegKeyString( lpszCfgfilename, SECTION_mcuNetwork, KEY_mcunetMulticastIpAddr, achDefStr, achReturn, MAX_VALUE_LEN + 1 );
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcuNetwork, KEY_mcunetMulticastPort, 0, &nValue );
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcuNetwork, KEY_mcunetRecvStartPort, 0, &nValue );
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcuNetwork, KEY_mcunet225245StartPort, 0, &nValue );
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcuNetwork, KEY_mcunet225245MtNum, 0, &nValue );
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcuNetwork, KEY_mcunetMTUSize, DEFAULT_MTU_SIZE, &nValue ); // 读MTU大小, zgc, 2007-04-02
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcuNetwork, KEY_mcunetUseMPCStack, 0, &nValue );
	if( !bSucceedRead )
	{
		printf("[McuAgt][PreReadCfgfile] The net work info is error!\n");
		return bSucceedRead;
	}
	
	// trap rcv table
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcunetTrapRcvTable, STR_ENTRY_NUM, 0, &nValue );
	if( nValue > 0)
	{
		s8 **ppszTable= new s8*[(u32)nValue];
		if( NULL == ppszTable )
		{
			printf("[PreReadCfgfile] Error to alloc memory.\n");
			return FALSE;
		}
        for( u32 dwLoop = 0; dwLoop < (u32)nValue; dwLoop++ )
        {
            ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
			if( NULL == ppszTable[dwLoop] )
			{
				FREE_TABLE_MEM( ppszTable, dwLoop );
				return FALSE;
			}
        }
		bSucceedRead &= AgtGetRegKeyStringTable( lpszCfgfilename, SECTION_mcunetTrapRcvTable, "fail", ppszTable, (u32*)&nValue, MAX_VALUE_LEN + 1 );
		FREE_TABLE_MEM( ppszTable, nValue );
	}
	if( !bSucceedRead )
	{
		printf("[McuAgt][PreReadCfgfile] The trap rcv table is error!\n");
		return bSucceedRead;
	}
	
	// brd cfg table
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcueqpBoardConfig, STR_ENTRY_NUM, 0, &nValue );
	if( nValue > 0 )
	{
		s8 **ppszTable= new s8*[(u32)nValue];
		if( NULL == ppszTable )
		{
			printf("[PreReadCfgfile] Error to alloc memory.\n");
			return FALSE;
		}
        for( u32 dwLoop = 0; dwLoop < (u32)nValue; dwLoop++ )
        {
            ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
			if( NULL == ppszTable[dwLoop] )
			{
				FREE_TABLE_MEM( ppszTable, dwLoop );
				return FALSE;
			}
        }
		bSucceedRead &= AgtGetRegKeyStringTable( lpszCfgfilename, SECTION_mcueqpBoardConfig, "fail", ppszTable, (u32*)&nValue, MAX_VALUE_LEN + 1 );
		FREE_TABLE_MEM( ppszTable, nValue );
	}
	if( !bSucceedRead )
	{
		printf("[McuAgt][PreReadCfgfile] The brd cfg table is error!\n");
		return bSucceedRead;
	}
	
	// mixer table
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcueqpMixerTable, STR_ENTRY_NUM, 0, &nValue );
	if( nValue > 0 )
	{
		s8 **ppszTable= new s8*[(u32)nValue];
		if( NULL == ppszTable )
		{
			printf("[PreReadCfgfile] Error to alloc memory.\n");
			return FALSE;
		}
        for( u32 dwLoop = 0; dwLoop < (u32)nValue; dwLoop++ )
        {
            ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
			if( NULL == ppszTable[dwLoop] )
			{
				FREE_TABLE_MEM( ppszTable, dwLoop );
				return FALSE;
			}
        }
		bSucceedRead &= AgtGetRegKeyStringTable( lpszCfgfilename, SECTION_mcueqpMixerTable, "fail", ppszTable, (u32*)&nValue, MAX_VALUE_LEN + 1 );
		FREE_TABLE_MEM( ppszTable, nValue );
	}
	if( !bSucceedRead )
	{
		printf("[McuAgt][PreReadCfgfile] The mixer table is error!\n");
		return bSucceedRead;
	}
	
	// recorder table
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcueqpRecorderTable, STR_ENTRY_NUM, 0, &nValue );
	if( nValue > 0 )
	{
		s8 **ppszTable= new s8*[(u32)nValue];
		if( NULL == ppszTable )
		{
			printf("[PreReadCfgfile] Error to alloc memory.\n");
			return FALSE;
		}
        for( u32 dwLoop = 0; dwLoop < (u32)nValue; dwLoop++ )
        {
            ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
			if( NULL == ppszTable[dwLoop] )
			{
				FREE_TABLE_MEM( ppszTable, dwLoop );
				return FALSE;
			}
        }
		bSucceedRead &= AgtGetRegKeyStringTable( lpszCfgfilename, SECTION_mcueqpRecorderTable, "fail", ppszTable, (u32*)&nValue, MAX_VALUE_LEN + 1 );
		FREE_TABLE_MEM( ppszTable, nValue );
	}
	if( !bSucceedRead )
	{
		printf("[McuAgt][PreReadCfgfile] The recorder table is error!\n");
		return bSucceedRead;
	}
	
	// tvwall table
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcueqpTVWallTable, STR_ENTRY_NUM, 0, &nValue );
	if( nValue > 0 )
	{
		s8 **ppszTable= new s8*[(u32)nValue];
		if( NULL == ppszTable )
		{
			printf("[PreReadCfgfile] Error to alloc memory.\n");
			return FALSE;
		}
        for( u32 dwLoop = 0; dwLoop < (u32)nValue; dwLoop++ )
        {
            ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
			if( NULL == ppszTable[dwLoop] )
			{
				FREE_TABLE_MEM( ppszTable, dwLoop );
				return FALSE;
			}
        }
		bSucceedRead &= AgtGetRegKeyStringTable( lpszCfgfilename, SECTION_mcueqpTVWallTable, "fail", ppszTable, (u32*)&nValue, MAX_VALUE_LEN + 1 );
		FREE_TABLE_MEM( ppszTable, nValue );
	}
	if( !bSucceedRead )
	{
		printf("[McuAgt][PreReadCfgfile] The tvwall table is error!\n");
		return bSucceedRead;
	}
	
	// bas table
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcueqpBasTable, STR_ENTRY_NUM, 0, &nValue );
	if( nValue > 0 )
	{
		s8 **ppszTable= new s8*[(u32)nValue];
		if( NULL == ppszTable )
		{
			printf("[PreReadCfgfile] Error to alloc memory.\n");
			return FALSE;
		}
        for( u32 dwLoop = 0; dwLoop < (u32)nValue; dwLoop++ )
        {
            ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
			if( NULL == ppszTable[dwLoop] )
			{
				FREE_TABLE_MEM( ppszTable, dwLoop );
				return FALSE;
			}
        }
		bSucceedRead &= AgtGetRegKeyStringTable( lpszCfgfilename, SECTION_mcueqpBasTable, "fail", ppszTable, (u32*)&nValue, MAX_VALUE_LEN + 1 );
		FREE_TABLE_MEM( ppszTable, nValue );
	}
	if( !bSucceedRead )
	{
		printf("[McuAgt][PreReadCfgfile] The bas table is error!\n");
		return bSucceedRead;
	}
	
	// vmp table
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcueqpVMPTable, STR_ENTRY_NUM, 0, &nValue );
	if( nValue > 0 )
	{
		s8 **ppszTable= new s8*[(u32)nValue];
		if( NULL == ppszTable )
		{
			printf("[PreReadCfgfile] Error to alloc memory.\n");
			return FALSE;
		}
        for( u32 dwLoop = 0; dwLoop < (u32)nValue; dwLoop++ )
        {
            ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
			if( NULL == ppszTable[dwLoop] )
			{
				FREE_TABLE_MEM( ppszTable, dwLoop );
				return FALSE;
			}
        }
		bSucceedRead &= AgtGetRegKeyStringTable( lpszCfgfilename, SECTION_mcueqpVMPTable, "fail", ppszTable, (u32*)&nValue, MAX_VALUE_LEN + 1 );
		FREE_TABLE_MEM( ppszTable, nValue );
	}
	if( !bSucceedRead )
	{
		printf("[McuAgt][PreReadCfgfile] The vmp table is error!\n");
		return bSucceedRead;
	}
	
	// mpw table
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcueqpMpwTable, STR_ENTRY_NUM, 0, &nValue );
	if( nValue > 0 )
	{
		s8 **ppszTable= new s8*[(u32)nValue];
		if( NULL == ppszTable )
		{
			printf("[PreReadCfgfile] Error to alloc memory.\n");
			return FALSE;
		}
        for( u32 dwLoop = 0; dwLoop < (u32)nValue; dwLoop++ )
        {
            ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
			if( NULL == ppszTable[dwLoop] )
			{
				FREE_TABLE_MEM( ppszTable, dwLoop );
				return FALSE;
			}
        }
		bSucceedRead &= AgtGetRegKeyStringTable( lpszCfgfilename, SECTION_mcueqpMpwTable, "fail", ppszTable, (u32*)&nValue, MAX_VALUE_LEN + 1 );
		FREE_TABLE_MEM( ppszTable, nValue );
	}
	if( !bSucceedRead )
	{
		printf("[McuAgt][PreReadCfgfile] The mpw table is error!\n");
		return bSucceedRead;
	}
	
	//4.6新加版本 jlb 
	// hdu table
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcueqpHduTable, STR_ENTRY_NUM, 0, &nValue );
	if( nValue > 0 )
	{
		s8 **ppszTable= new s8*[(u32)nValue];
		if( NULL == ppszTable )
		{
			printf("[PreReadCfgfile] Error to alloc memory.\n");
			return FALSE;
		}
        for( u32 dwLoop = 0; dwLoop < (u32)nValue; dwLoop++ )
        {
            ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
			if( NULL == ppszTable[dwLoop] )
			{
				FREE_TABLE_MEM( ppszTable, dwLoop );
				return FALSE;
			}
        }
		bSucceedRead &= AgtGetRegKeyStringTable( lpszCfgfilename, SECTION_mcueqpHduTable, "fail", ppszTable, (u32*)&nValue, MAX_VALUE_LEN + 1 );
		FREE_TABLE_MEM( ppszTable, nValue );
	}
	if( !bSucceedRead )
	{
		printf("[McuAgt][PreReadCfgfile] The hdu table is error!\n");
		return bSucceedRead;
	}
	
	// svmp table
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcueqpSvmpTable, STR_ENTRY_NUM, 0, &nValue );
	if( nValue > 0 )
	{
		s8 **ppszTable= new s8*[(u32)nValue];
		if( NULL == ppszTable )
		{
			printf("[PreReadCfgfile] Error to alloc memory.\n");
			return FALSE;
		}
        for( u32 dwLoop = 0; dwLoop < (u32)nValue; dwLoop++ )
        {
            ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
			if( NULL == ppszTable[dwLoop] )
			{
				FREE_TABLE_MEM( ppszTable, dwLoop );
				return FALSE;
			}
        }
		bSucceedRead &= AgtGetRegKeyStringTable( lpszCfgfilename, SECTION_mcueqpSvmpTable, "fail", ppszTable, (u32*)&nValue, MAX_VALUE_LEN + 1 );
		FREE_TABLE_MEM( ppszTable, nValue );
	}
	if( !bSucceedRead )
	{
		printf("[McuAgt][PreReadCfgfile] The svmp table is error!\n");
		return bSucceedRead;
	}
	
	// dvmp table
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcueqpDvmpTable, STR_ENTRY_NUM, 0, &nValue );
	if( nValue > 0 )
	{
		s8 **ppszTable= new s8*[(u32)nValue];
		if( NULL == ppszTable )
		{
			printf("[PreReadCfgfile] Error to alloc memory.\n");
			return FALSE;
		}
        for( u32 dwLoop = 0; dwLoop < (u32)nValue; dwLoop++ )
        {
            ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
			if( NULL == ppszTable[dwLoop] )
			{
				FREE_TABLE_MEM( ppszTable, dwLoop );
				return FALSE;
			}
        }
		bSucceedRead &= AgtGetRegKeyStringTable( lpszCfgfilename, SECTION_mcueqpDvmpTable, "fail", ppszTable, (u32*)&nValue, MAX_VALUE_LEN + 1 );
		FREE_TABLE_MEM( ppszTable, nValue );
	}
	if( !bSucceedRead )
	{
		printf("[McuAgt][PreReadCfgfile] The dvmp table is error!\n");
		return bSucceedRead;
	}
	
	// mpubas table
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcueqpMpuBasTable, STR_ENTRY_NUM, 0, &nValue );
	if( nValue > 0 )
	{
		s8 **ppszTable= new s8*[(u32)nValue];
		if( NULL == ppszTable )
		{
			printf("[PreReadCfgfile] Error to alloc memory.\n");
			return FALSE;
		}
        for( u32 dwLoop = 0; dwLoop < (u32)nValue; dwLoop++ )
        {
            ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
			if( NULL == ppszTable[dwLoop] )
			{
				FREE_TABLE_MEM( ppszTable, dwLoop );
				return FALSE;
			}
        }
		bSucceedRead &= AgtGetRegKeyStringTable( lpszCfgfilename, SECTION_mcueqpMpuBasTable, "fail", ppszTable, (u32*)&nValue, MAX_VALUE_LEN + 1 );
		FREE_TABLE_MEM( ppszTable, nValue );
	}
	if( !bSucceedRead )
	{
		printf("[McuAgt][PreReadCfgfile] The Mpubas table is error!\n");
		return bSucceedRead;
	}
	
	// ebap table
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcueqpEbapTable, STR_ENTRY_NUM, 0, &nValue );
	if( nValue > 0 )
	{
		s8 **ppszTable= new s8*[(u32)nValue];
		if( NULL == ppszTable )
		{
			printf("[PreReadCfgfile] Error to alloc memory.\n");
			return FALSE;
		}
        for( u32 dwLoop = 0; dwLoop < (u32)nValue; dwLoop++ )
        {
            ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
			if( NULL == ppszTable[dwLoop] )
			{
				FREE_TABLE_MEM( ppszTable, dwLoop );
				return FALSE;
			}
        }
		bSucceedRead &= AgtGetRegKeyStringTable( lpszCfgfilename, SECTION_mcueqpEbapTable, "fail", ppszTable, (u32*)&nValue, MAX_VALUE_LEN + 1 );
		FREE_TABLE_MEM( ppszTable, nValue );
	}
	if( !bSucceedRead )
	{
		printf("[McuAgt][PreReadCfgfile] The ebap table is error!\n");
		return bSucceedRead;
	}
	
	// evpu table
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcueqpEvpuTable, STR_ENTRY_NUM, 0, &nValue );
	if( nValue > 0 )
	{
		s8 **ppszTable= new s8*[(u32)nValue];
		if( NULL == ppszTable )
		{
			printf("[PreReadCfgfile] Error to alloc memory.\n");
			return FALSE;
		}
        for( u32 dwLoop = 0; dwLoop < (u32)nValue; dwLoop++ )
        {
            ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
			if( NULL == ppszTable[dwLoop] )
			{
				FREE_TABLE_MEM( ppszTable, dwLoop );
				return FALSE;
			}
        }
		bSucceedRead &= AgtGetRegKeyStringTable( lpszCfgfilename, SECTION_mcueqpEvpuTable, "fail", ppszTable, (u32*)&nValue, MAX_VALUE_LEN + 1 );
		FREE_TABLE_MEM( ppszTable, nValue );
	}
	if( !bSucceedRead )
	{
		printf("[McuAgt][PreReadCfgfile] The evpu table is error!\n");
		return bSucceedRead;
	}
	
	
	// eqp dcs
	bResult = GetRegKeyString( lpszCfgfilename, SECTION_mcueqpDataConfServer, KEY_mcueqpDcsIp, "0.0.0.0", achReturn, sizeof(achReturn) );
	if( !bResult ) // VERSION_CONFIGURE36
	{
		bResult = GetRegKeyString( lpszCfgfilename, SECTION_mcueqpDataConfServer, "mcueqpDcsId", "0.0.0.0", achReturn, sizeof(achReturn) );
	}
	bSucceedRead &= bResult;
	
	// prs info table
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcueqpPrsTable, STR_ENTRY_NUM, 0, &nValue );
	if( nValue > 0 )
	{
		s8 **ppszTable= new s8*[(u32)nValue];
		if( NULL == ppszTable )
		{
			printf("[PreReadCfgfile] Error to alloc memory.\n");
			return FALSE;
		}
        for( u32 dwLoop = 0; dwLoop < (u32)nValue; dwLoop++ )
        {
            ppszTable[dwLoop] = new s8[MAX_VALUE_LEN + 1];
			if( NULL == ppszTable[dwLoop] )
			{
				FREE_TABLE_MEM( ppszTable, dwLoop );
				return FALSE;
			}
        }
		bSucceedRead &= AgtGetRegKeyStringTable( lpszCfgfilename, SECTION_mcueqpPrsTable, "fail", ppszTable, (u32*)&nValue, MAX_VALUE_LEN + 1 );
		FREE_TABLE_MEM( ppszTable, nValue );
	}
	if( !bSucceedRead )
	{
		printf("[McuAgt][PreReadCfgfile] The prs info table is error!\n");
		return bSucceedRead;
	}
	
#ifndef WIN32
	// nec sync
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcueqpNetSync, KEY_mcueqpNetSyncMode, 0, &nValue );
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcueqpNetSync, KEY_mcueqpNetSyncDTSlot, 0, &nValue );
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcueqpNetSync, KEY_mcueqpNetSyncE1Index, 0, &nValue );
#endif
	
	// Qos info
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcuQosInfo, KEY_mcuQosType, 0, &nValue );
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcuQosInfo, KEY_mcuAudioLevel, 0, &nValue );
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcuQosInfo, KEY_mcuVideoLevel, 0, &nValue );
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcuQosInfo, KEY_mcuDataLevel, 0, &nValue );
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_mcuQosInfo, KEY_mcuSignalLevel, 0, &nValue );
	bSucceedRead &= GetRegKeyString( lpszCfgfilename, SECTION_mcuQosInfo, KEY_mcuIpServiceType, "", (s8*)achReturn, sizeof(achReturn));
	
#ifdef _MINIMCU_
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_DscModule, KEY_DscMp, TRUE, &nValue );
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_DscModule, KEY_DscMtAdp, TRUE, &nValue );
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_DscModule, KEY_DscGk, TRUE, &nValue );
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_DscModule, KEY_DscProxy, TRUE, &nValue );
	bSucceedRead &= GetRegKeyInt( lpszCfgfilename, SECTION_DscModule, KEY_DscDcs, TRUE, &nValue );
	bSucceedRead &= GetRegKeyString( lpszCfgfilename, SECTION_DscModule, KEY_DscInnerIp, DEFAULT_DSC_INNERIP, achReturn, sizeof(achReturn) );
#endif
	
	return bSucceedRead;
	
}
/*=============================================================================
函 数 名： SetSystemInfoByDefault
功    能： 暂时无需调用
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/8    4.0			周广程                  创建
=============================================================================*/
void CCfgAgent::SetSystemInfoByDefault()
{
	time_t  tCurTime;
	struct tm   *ptTime = NULL;
	s8 achTime[MAX_CONFIGURE_LENGTH];
	memset(achTime, 0, sizeof(achTime));
	// 不能配置版本号为当前时间，防止RTC时钟坏导致读取配置文件错误
	tCurTime = time( 0 );
	ptTime = localtime( &tCurTime );
	sprintf( achTime, "%s Default Config", MCUAGT_SYS_CFGVER );
    m_tMcuSystem.SetConfigVer( achTime );
	m_tMcuSystem.SetIsMcuConfiged( FALSE );
	
}

/*=============================================================================
函 数 名： SetLocalInfoByDefault
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/8    4.0			周广程                  创建
=============================================================================*/
void CCfgAgent::SetLocalInfoByDefault()
{
	m_tMcuLocalInfo.SetMcuId( LOCAL_MCUID );
    m_tMcuLocalInfo.SetAlias( "KEDA_MCU" );
    m_tMcuLocalInfo.SetE164Num( "8651264273989" );
	
}

/*=============================================================================
函 数 名： SetGkProxyCfgInfoByDefault
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2010/03/16  4.6			薛亮                  创建
=============================================================================*/
void CCfgAgent::SetGkProxyCfgInfoByDefault()
{
	m_tGkProxyCfgInfo.SetGkIpAddr(0);
	m_tGkProxyCfgInfo.SetGkUsed(0);
	m_tGkProxyCfgInfo.SetProxyIpAddr(0);
	m_tGkProxyCfgInfo.SetProxyPort(0);
	m_tGkProxyCfgInfo.SetProxyUsed(0);

	m_tProxyDMZInfo.SetIsUseDMZ(0);
	m_tProxyDMZInfo.SetDMZIpAddr(0);
	// 	SetGkProxyCfgInfo(m_tGkProxyCfgInfo);
}
/*=============================================================================
函 数 名： SetPrsTimeSpanCfgInfoByDefault
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2010/03/16  4.6			薛亮                  创建
=============================================================================*/
void CCfgAgent::SetPrsTimeSpanCfgInfoByDefault()
{
	m_tPrsTimeSpan.m_wFirstTimeSpan = 40;
	m_tPrsTimeSpan.m_wSecondTimeSpan = 120;
	m_tPrsTimeSpan.m_wThirdTimeSpan = 240;
	m_tPrsTimeSpan.m_wRejectTimeSpan = 480;
	// 	SetPrsTimeSpanCfgInfo(m_tPrsTimeSpan);
}

#ifdef _8KI_
/*=============================================================================
函 数 名： SetNewNetCfgInfoByDefault
功    能： 设置8KI默认网口信息
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
20130930    8KI			闫日亮                  创建
=============================================================================*/
void CCfgAgent::SetNewNetCfgInfoByDefault()
{
	m_tNewNetCfg.Clear();
#ifdef _LINUX_
	TEthInfo tEthInfo[MAXNUM_ETH_INTERFACE];
	u8 byEthNum = 0;
	GetEthInfo(tEthInfo, byEthNum);
	TEthCfg tEthCfg;
	TNetCfg tNetCfg;
	for(u8 byEthIdx = 0; byEthIdx < byEthNum; byEthIdx++)
	{
		m_tNewNetCfg.GetEthCfg(byEthIdx, tEthCfg);
		u8 byIpNum = tEthInfo[byEthIdx].GetIpNum();
		for(u8 byIpIndx = 0; byIpIndx < byIpNum; byIpIndx++)
		{
			tEthCfg.GetNetCfg(byIpIndx, tNetCfg);
			u32 dwIp = tEthInfo[byEthIdx].GetSubIp(byIpIndx);
			u32 dwMask = tEthInfo[byEthIdx].GetSubMask(byIpIndx);
			u32 dwGw = 0;
			if(byIpIndx == 0)
			{
				dwGw = tEthInfo[byEthIdx].GetGateway();	
			}			
			tNetCfg.SetNetRouteParam(dwIp, dwMask, dwGw);
			tEthCfg.SetNetCfg(byIpIndx, tNetCfg);
		}		
		m_tNewNetCfg.SetEthCfg(byEthIdx, tEthCfg);
	}
#endif
}
#endif

/*=============================================================================
函 数 名： SetNetworkInfoByDefault
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/8  4.0			周广程                  创建
=============================================================================*/
void CCfgAgent::SetNetWorkInfoByDefault()
{
	m_tMcuNetwork.SetGKIp( 0 );
    m_tMcuNetwork.SetIsGKCharge( FALSE );
    m_tMcuNetwork.SetRRQMtadpIp( 0 );
	m_tMcuNetwork.SetCastIp( 0 );
	m_tMcuNetwork.SetCastPort( MULTICAST_DEFPORT );
	m_tMcuNetwork.SetRecvStartPort( MT_MCU_STARTPORT );
	m_tMcuNetwork.SetMpcTransData( 0 ); // guzh [1/16/2007] 默认不支持
	m_tMcuNetwork.SetMpcStack( TRUE );
    m_tMcuNetwork.Set225245StartPort( MCU_H225245_STARTPORT );
	m_tMcuNetwork.SetMTUSize( DEFAULT_MTU_SIZE ); // 设置MTU大小, zgc, 2007-04-02

	m_tMcuNetwork.SetGkRRQUsePwdFlag(0);
	m_tMcuNetwork.SetGkRRQPassword(NULL);
}

/*=============================================================================
函 数 名： SetTrapRcvTableByDefault
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/8  4.0			周广程                  创建
=============================================================================*/
void CCfgAgent::SetTrapRcvTableByDefault()
{
	m_dwTrapRcvEntryNum = 0;
}

/*=============================================================================
函 数 名： SetBrdCfgTableByDefault
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值：  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/8  4.0			周广程                  创建
=============================================================================*/
void CCfgAgent::SetBrdCfgTableByDefault()
{
	m_dwBrdCfgEntryNum = 1;
	m_atBrdCfgTable[0].SetBrdId( 1 );
	
    TBrdPosition tMpcPosition;
    
	if (OK != AgtQueryBrdPosition(&tMpcPosition)) 
	{
		printf("[SetDefaultConfig] AgtQueryBrdPosition failed !\n");
	}
	else
	{
		Agtlog(LOG_INFORM, "[SetDefaultConfig] The Mpc Layer: %d, Slot:%d, Id: %d, Active:%d\n", 
			tMpcPosition.byBrdLayer, tMpcPosition.byBrdSlot, tMpcPosition.byBrdID, IsMpcActive());
	}
	
	
#ifdef _MINIMCU_
    m_tMPCInfo.SetLocalPortChoice(0);   // 前网口
#else
    // 对于创建默认配置文件的情况，由于不知道用户希望的MPC的IP，
	// 因此只能优先选择eth1，在eth1没有配置IP的情况下，再检查eth0, zgc, 2007-07-12
#ifndef _VXWORKS_
	// 先查eth1
	STATUS nRet = 0;
	TBrdEthParamAll tBrdEthParamAll;
	nRet = AgtGetBrdEthParamAll( 1, &tBrdEthParamAll ); 
	if ( nRet == ERROR )
	{
		printf( "[SetDefaultConfig] Get eth1 params failed!\n" );
	}
	else if ( tBrdEthParamAll.dwIpNum > 0 && tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs !=0 )
	{
		m_tMPCInfo.SetLocalPortChoice( 1 );
	}
	
	if ( nRet == ERROR 
		|| tBrdEthParamAll.dwIpNum == 0 
		|| (tBrdEthParamAll.dwIpNum > 0 && tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs == 0) )
	{
		// 再查eth0
		memset( &tBrdEthParamAll, 0, sizeof(tBrdEthParamAll) );
		nRet = AgtGetBrdEthParamAll( 0, &tBrdEthParamAll ); 
		if ( nRet == ERROR )
		{
			printf( "[SetDefaultConfig] Get eth0 params failed!\n" );
		}
		else if ( tBrdEthParamAll.dwIpNum > 0 && tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs !=0 )
		{
			m_tMPCInfo.SetLocalPortChoice( 0 );
		}
		
		if ( nRet == ERROR 
			|| tBrdEthParamAll.dwIpNum == 0 
			|| (tBrdEthParamAll.dwIpNum > 0 && tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs == 0) )
		{
			printf( "[SetDefaultConfig] MPC ip is not correct!\n" );
			m_tMPCInfo.SetLocalPortChoice( 1 );
		}
	}
#else
	//#ifdef _VXWORKS_
	// 先查eth1
	s32 nRet;
	TBrdEthParam tOneParam;
	memset(&tOneParam, 0, sizeof(tOneParam));
	nRet = AgtGetBrdEthParam( 1, &tOneParam );
	if ( nRet == ERROR )
	{
		printf( "[SetDefaultConfig] Get eth0 param failed!\n" );
	}
	else if ( tOneParam.dwIpAdrs != 0 )
	{
		m_tMPCInfo.SetLocalPortChoice( 1 );
	}
	
	if ( nRet == ERROR || tOneParam.dwIpAdrs == 0 ) 
	{
		//再查eth0
		memset(&tOneParam, 0, sizeof(tOneParam));
		nRet = AgtGetBrdEthParam( 0, &tOneParam );
		if ( nRet == ERROR )
		{
			printf( "[SetDefaultConfig] Get eth0 param failed!\n" );
		}
		else if ( tOneParam.dwIpAdrs != 0 )
		{
			m_tMPCInfo.SetLocalPortChoice( 0 );
		}
		
		if ( nRet == ERROR || tOneParam.dwIpAdrs == 0 ) 
		{
			printf( "[SetDefaultConfig] MPC ip is not correct!\n" );
			m_tMPCInfo.SetLocalPortChoice( 1 );
		}
	}
#endif
	//	#ifdef WIN32
	//		m_tMPCInfo.SetLocalPortChoice( 1 );
	//	#endif
#endif
	
	m_atBrdCfgTable[0].SetBrdIp( ntohl(GetLocalIp()) );
	m_atBrdCfgTable[0].SetLayer(tMpcPosition.byBrdLayer);
	m_atBrdCfgTable[0].SetSlot(tMpcPosition.byBrdSlot);
	//  [1/21/2011 chendaiwei]支持MPC2
#ifdef _LINUX12_
	m_atBrdCfgTable[0].SetType(BRD_TYPE_MPC2/*DSL8000_BRD_MPC2*/);
	m_atBrdCfgTable[0].SetAlias(BRD_ALIAS_MPC2);
#else
	m_atBrdCfgTable[0].SetType(BRD_TYPE_MPC/*DSL8000_BRD_MPC*/);
	m_atBrdCfgTable[0].SetAlias(BRD_ALIAS_MPC);
#endif
	m_atBrdCfgTable[0].SetPortChoice( m_tMPCInfo.GetLocalPortChoice() );
	
	
}

/*=============================================================================
函 数 名： SetEqpDcsByDefault
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/8  4.0			周广程                  创建
=============================================================================*/
void CCfgAgent::SetEqpDcsByDefault()
{
	m_tDCS.m_dwDcsIp = 0;
}

/*=============================================================================
函 数 名： SetPrsInfoByDefault
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/8  4.0			周广程                  创建
=============================================================================*/
void CCfgAgent::SetPrsInfoByDefault()
{
	m_dwPrsEntryNum = 0;
}

/*=============================================================================
函 数 名： SetNecSyncByDefault
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值：  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/8  4.0			周广程                  创建
=============================================================================*/
void CCfgAgent::SetNecSyncByDefault()
{
	m_tNetSync.SetMode( NETSYNTYPE_VIBRATION );
}

/*=============================================================================
函 数 名： SetMixerTableByDefault
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/8  4.0			周广程                  创建
=============================================================================*/
void CCfgAgent::SetMixerTableByDefault()
{
	m_dwMixEntryNum = 0;
}

/*=============================================================================
函 数 名： SetRecorderTableByDefault
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/8  4.0			周广程                  创建
=============================================================================*/
void CCfgAgent::SetRecorderTableByDefault()
{
	m_dwRecEntryNum = 0;
}

/*=============================================================================
函 数 名： SetVrsRecorderTableByDefault
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/10/09  4.0			杨怀志                  创建
=============================================================================*/
void CCfgAgent::SetVrsRecorderTableByDefault()
{
	m_dwVrsRecEntryNum = 0;
}

/*=============================================================================
函 数 名： SetTVWallTableByDefault
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值：  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/8  4.0			周广程                  创建
=============================================================================*/
void CCfgAgent::SetTVWallTableByDefault()
{
	m_dwTVWallEntryNum = 0;
}

/*=============================================================================
函 数 名： SetBasTableByDefault
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值：  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/8  4.0			周广程                  创建
=============================================================================*/
void CCfgAgent::SetBasTableByDefault()
{
	m_dwBasEntryNum = 0;
}

/*=============================================================================
函 数 名： SetBasHDTableByDefault
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值：  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/08/07  4.0			周文                  创建
=============================================================================*/
void CCfgAgent::SetBasHDTableByDefault()
{
	m_dwBasHDEntryNum = 0;
}

//4.6版本新加  jlb
/*=============================================================================
函 数 名： SetHduTableByDefault
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值：  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/12/31  4.6			江乐斌                  创建
=============================================================================*/
void CCfgAgent::SetHduTableByDefault()
{
    m_dwHduEntryNum = 0;    
}

/*=============================================================================
函 数 名： SetSvmpTableByDefault
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值：  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/12/31  4.6			江乐斌                  创建
=============================================================================*/
void CCfgAgent::SetSvmpTableByDefault()
{
	m_dwSvmpEntryNum = 0;
}

/*=============================================================================
函 数 名： SetDvmpTableByDefault
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值：  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/12/31  4.6			江乐斌                  创建
=============================================================================*/
void CCfgAgent::SetDvmpTableByDefault()
{
	m_dwDvmpEntryNum = 0;
}

/*=============================================================================
函 数 名： SetMpuBasTableByDefault
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值：  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/12/31  4.6			江乐斌                  创建
=============================================================================*/
void CCfgAgent::SetMpuBasTableByDefault()
{
	m_dwMpuBasEntryNum = 0;
}

/*=============================================================================
函 数 名： SetEbapTableByDefault
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值：  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/12/31  4.6			江乐斌                  创建
=============================================================================*/
void CCfgAgent::SetEbapTableByDefault()
{
    m_dwEbapEntryNum = 0;
}

/*=============================================================================
函 数 名： SetEvpuTableByDefault
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值：  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/12/31  4.6			江乐斌                  创建
=============================================================================*/
void CCfgAgent::SetEvpuTableByDefault()
{
    m_dwEvpuEntryNum = 0;
}

/*=============================================================================
函 数 名： SetVMPTableByDefault
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值：  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/8  4.0			周广程                  创建
=============================================================================*/
void CCfgAgent::SetVMPTableByDefault()
{
	m_dwVMPEntryNum = 0;
}

/*=============================================================================
函 数 名： SetQosInfoByDefault
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/8  4.0			周广程                  创建
=============================================================================*/
void CCfgAgent::SetQosInfoByDefault()
{
    m_tMcuQosInfo.SetQosType(QOSTYPE_IP_PRIORITY);   // Qos 类型
    m_tMcuQosInfo.SetAudioLevel(AUDIO_LVL_IP);		// 音频等级
    m_tMcuQosInfo.SetVideoLevel(VIDEO_LVL_IP);		// 视频等级
    m_tMcuQosInfo.SetDataLevel(DATA_LVL_IP);			// 数据等级
    m_tMcuQosInfo.SetSignalLevel(SIGNAL_LVL_IP);		// 信号等级
    m_tMcuQosInfo.SetIpServiceType(IPSERVICETYPE_LOW_DELAY); // IP服务等级
}

/*=============================================================================
函 数 名： SetVmpAttachTableByDefault
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/8  4.0			周广程                  创建
=============================================================================*/
void CCfgAgent::SetVmpAttachTableByDefault()
{
	m_dwVmpAttachNum = 0;
}

/*=============================================================================
函 数 名： SetMpwTableByDefault
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值：  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/8  4.0			周广程                  创建
=============================================================================*/
void CCfgAgent::SetMpwTableByDefault()
{
	m_dwMpwEntryNum = 0;
}

/*=============================================================================
函 数 名： SetMpwTableByDefault
功    能： 外设扩展配置信息
算法实现： 
全局变量： 
参    数： 
返 回 值：  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
20100111    4.6			pengjie                create
=============================================================================*/
void CCfgAgent::SetEqpExCfgInfoByDefault()
{
	m_tEqpExCfgInfo.m_tHDUExCfgInfo.m_byIdleChlShowMode = 0;
	m_tEqpExCfgInfo.m_tVMPExCfgInfo.m_byIdleChlShowMode = 0;
}

/*=============================================================================
函 数 名： SetDscInfoByDefault
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/16  4.0			周广程                  创建
=============================================================================*/
#ifdef _MINIMCU_
void CCfgAgent::SetDscInfoByDefault()
{
#ifdef _MINIMCU_
	TDSCModuleInfo tDscInfo;
	tDscInfo.SetStartMp( TRUE );
	tDscInfo.SetStartMtAdp( TRUE );
	tDscInfo.SetStartProxy( TRUE );
	tDscInfo.SetStartDcs( TRUE );
	tDscInfo.SetStartGk( TRUE );
	tDscInfo.SetDscInnerIp( ntohl(INET_ADDR(DEFAULT_DSC_INNERIP)) );
	
	//#ifndef WIN32
    // MCU IP 和端口则自行获取
    // 首先尝试内网口
    TBrdEthParam tEthParam;
    STATUS nRet = AgtGetBrdEthParam( 1, &tEthParam );
    if ( nRet == OK && tEthParam.dwIpAdrs != 0 )
    {
        m_tMcuDscInfo.SetMcuInnerIp( ntohl(tEthParam.dwIpAdrs) );
        m_tMcuDscInfo.SetInnerIpMask( ntohl(tEthParam.dwIpMask) );
    }
    else
    {
		m_tMcuDscInfo.SetMcuInnerIp( ntohl(INET_ADDR(DEFAULT_MCU_INNERIP)) );
		m_tMcuDscInfo.SetInnerIpMask( ntohl(INET_ADDR(DEFAULT_INNERIPMASK)) );
    }
	//#else
	//    m_tMcuDscInfo.SetMcuInnerIp( ntohl(INET_ADDR(DEFAULT_MCU_INNERIP)) );
	//    m_tMcuDscInfo.SetInnerIpMask( ntohl(INET_ADDR(DEFAULT_INNERIPMASK)) );
	//#endif	
	
	TMINIMCUNetParamAll tParamAddrAll;
	TMINIMCUNetParam	tParamAddr;
	memset( &tParamAddrAll, 0, sizeof(tParamAddrAll) );
#ifndef _VXWORKS_
	// MCU侧会控接入IP根据NIP设置
	TBrdEthParamAll tBrdEthParamAll;
	AgtGetBrdEthParamAll( 0, &tBrdEthParamAll );
	u8 byLop = 0;
	for ( byLop = 0; byLop < tBrdEthParamAll.dwIpNum; byLop++ )
	{
		memset( &tParamAddr, 0, sizeof(tParamAddr) );
		tParamAddr.SetLanParam( ntohl(tBrdEthParamAll.atBrdEthParam[byLop].dwIpAdrs),
			ntohl(tBrdEthParamAll.atBrdEthParam[byLop].dwIpMask),
			0);
		tParamAddrAll.AddOneNetParam( tParamAddr );
	}
	tDscInfo.SetMcsAccessAddrAll( tParamAddrAll );
	
	// DSC侧暂是设置为默认值，等DSC注册上来后再修改
	memset( &tParamAddrAll, 0, sizeof(tParamAddrAll) );
	memset( &tParamAddr, 0, sizeof(tParamAddr) );
	tParamAddr.SetLanParam( ntohl(INET_ADDR(DEFAULT_DSC_IP)), ntohl(INET_ADDR(DEFAULT_IPMASK)), 0);
	tParamAddrAll.AddOneNetParam( tParamAddr );
	tDscInfo.SetCallAddrAll( tParamAddrAll );
#endif
	//#ifdef WIN32
	//	memset( &tParamAddr, 0, sizeof(tParamAddr) );
	//	tParamAddr.SetLanParam( ntohl(GetLocalIp()), ntohl(INET_ADDR(DEFAULT_IPMASK)), ntohl(GetMpcGateway()) );
	//	tParamAddrAll.AddOneNetParam( tParamAddr );
	//	tDscInfo.SetMcsAccessAddrAll( tParamAddrAll );
	//
	//	memset( &tParamAddrAll, 0, sizeof(tParamAddrAll) );
	//	memset( &tParamAddr, 0, sizeof(tParamAddr) );
	//	tParamAddr.SetLanParam( ntohl(INET_ADDR(DEFAULT_DSC_IP)), ntohl(INET_ADDR(DEFAULT_IPMASK)), 0 );
	//	tParamAddrAll.AddOneNetParam( tParamAddr );
	//	tDscInfo.SetCallAddrAll( tParamAddrAll );
	//#endif
	SetDSCInfo( &tDscInfo, TRUE );
#endif //#ifdef _MINIMCU_
}
#endif

/*=============================================================================
函 数 名： SetMcuCfgInfoLevel
功    能： 
算法实现： 
全局变量： 
参    数： u8 byLevel
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/16  4.0			周广程                  创建
=============================================================================*/
void CCfgAgent::SetMcuCfgInfoLevel( u8 byLevel )
{
	m_tMcuSystem.SetMcuCfgInfoLevel( byLevel );
}

/*=============================================================================
函 数 名： GetMcuCfgInfoLevel
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： u8 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/16  4.0			周广程                  创建
=============================================================================*/
u8 CCfgAgent::GetMcuCfgInfoLevel()
{
	return m_tMcuSystem.GetMcuCfgInfoLevel();
}

/*=============================================================================
函 数 名： WriteAllCfgInfoToCfgfile
功    能： 
算法实现： 
全局变量： 
参    数： const s8* lpszCfgfilename
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/19  4.0			周广程                  创建
=============================================================================*/
BOOL32 CCfgAgent::WriteAllCfgInfoToCfgfile()
{
	printf("[McuAgt][WriteAllCfgInfoToCfgfile] Write all cfg info to %s\n!", m_achCfgName );
	
	//ENTER( m_hMcuAllConfig );
	
	if( NULL == m_achCfgName )
	{
		printf( "[McuAge][WriteAllCfgInfoToCfgfile] The cfgfile name is not exist!\n" );
		return FALSE;
	}
	
	FILE *hfile  = fopen(m_achCfgName, "w");
	if (NULL == hfile)
	{
		printf( "[McuAgt][WriteAllCfgInfoToCfgfile] The cfg file open failed!\n" );
		return FALSE;
	}
	else
	{
		fclose(hfile);
		hfile = NULL;
	}
	
	u16 bResult = SUCCESS_AGENT;
	s32 dwEntryNum = 0;
	u8 dwLop = 0;
	// system info
	TMcuSystem tMcuSystem;
	GetSystemInfo( &tMcuSystem );
    bResult = SetSystemInfo( &tMcuSystem );
	
	bResult |= SetIsMcuConfiged( IsMcuConfiged() );
	
	// local info
	TLocalInfo tLocalInfo;
	GetLocalInfo( &tLocalInfo );
	bResult |= SetLocalInfo( &tLocalInfo );
	
	// net work
	TNetWorkInfo tNetWorkInfo;
	GetNetWorkInfo( &tNetWorkInfo );
	bResult |= SetNetWorkInfo( &tNetWorkInfo );
	
	// TrapRcvTable
	TTrapInfo atTrapRcvTable[MAXNUM_TRAP_LIST];
	dwEntryNum = m_dwTrapRcvEntryNum;
	if( dwEntryNum > 0)
	{
		memcpy( atTrapRcvTable, m_atTrapRcvTable, sizeof(TTrapInfo)*m_dwTrapRcvEntryNum );
	}
	bResult |= WriteTrapTable( dwEntryNum, atTrapRcvTable );
	
	// BoardConfig
	TBoardInfo	atBrdTable[MAX_BOARD_NUM];	
	dwEntryNum = m_dwBrdCfgEntryNum;
	if( dwEntryNum > 0)
	{
		for( dwLop = 0; dwLop < dwEntryNum; dwLop++ )
		{
			atBrdTable[dwLop].SetBrdId( m_atBrdCfgTable[dwLop].GetBrdId() );
			atBrdTable[dwLop].SetLayer( m_atBrdCfgTable[dwLop].GetLayer() );
			atBrdTable[dwLop].SetSlot( m_atBrdCfgTable[dwLop].GetSlot() );
			atBrdTable[dwLop].SetType( m_atBrdCfgTable[dwLop].GetType() );
			atBrdTable[dwLop].SetCastChoice( m_atBrdCfgTable[dwLop].GetCastChoice() );
			atBrdTable[dwLop].SetPortChoice( m_atBrdCfgTable[dwLop].GetPortChoice() );
			atBrdTable[dwLop].SetBrdIp( m_atBrdCfgTable[dwLop].GetBrdIp() );
			atBrdTable[dwLop].SetAlias( m_atBrdCfgTable[dwLop].GetAlias() );
			// 运行外设情况
            u8  byPeriNum = 0;			    // 运行的外设个数
            u8  abyPeriId[MAXNUM_BRD_EQP] = {0}; // 运行在该单板上的外设ID
            m_atBrdCfgTable[dwLop].GetPeriId( abyPeriId, &byPeriNum );
            atBrdTable[dwLop].SetPeriId( abyPeriId, byPeriNum );
		}
	}
	bResult |= WriteBrdTable( dwEntryNum, atBrdTable );
	
	// MixerTable
	TEqpMixerInfo atMixTable[MAX_PRIEQP_NUM];
	dwEntryNum = m_dwMixEntryNum;
	if( dwEntryNum > 0)
	{
		for( dwLop = 0; dwLop < dwEntryNum; dwLop++ )
		{
			GetEqpMixerCfgByRow( dwLop+1, atMixTable+dwLop );
		}
	}
	bResult |= WriteMixerTable( dwEntryNum, atMixTable );
	
	// RecorderTable
	TEqpRecInfo atRecTable[MAX_PRIEQP_NUM];
	dwEntryNum = m_dwRecEntryNum;
	if( dwEntryNum > 0)
	{
		for( dwLop = 0; dwLop < dwEntryNum; dwLop++ )
		{
			GetEqpRecCfgByRow( dwLop+1, atRecTable+dwLop );
		}
	}
	bResult |= WriteRecTable( dwEntryNum,  atRecTable );
	
	// TVWallTable
	TEqpTVWallInfo atTVWallTable[MAX_PRIEQP_NUM];
	dwEntryNum = m_dwTVWallEntryNum;
	if( dwEntryNum > 0)
	{
		for( dwLop = 0; dwLop < dwEntryNum; dwLop++ )
		{
			GetEqpTVWallCfgByRow( dwLop+1, atTVWallTable+dwLop );
		}
	}
	bResult |= WriteTvTable( dwEntryNum, atTVWallTable );
	
	// BasTable
	TEqpBasInfo atBasTable[MAX_PRIEQP_NUM];
	dwEntryNum = m_dwBasEntryNum;
	if( dwEntryNum > 0)
	{
		for( dwLop = 0; dwLop < dwEntryNum; dwLop++ )
		{
			GetEqpBasCfgByRow( dwLop+1, atBasTable+dwLop );
		}
	}
	bResult |= WriteBasTable( dwEntryNum, atBasTable );
	
    //zw[08/09/2008]
    //BasHDTable
	TEqpBasHDInfo atBasHDTable[MAX_PRIEQP_NUM];
	dwEntryNum = m_dwBasHDEntryNum;
	if( dwEntryNum > 0)
	{
		for( dwLop = 0; dwLop < dwEntryNum; dwLop++ )
		{
			GetEqpBasHDCfgByRow( dwLop+1, atBasHDTable+dwLop );
		}
	}
	bResult |= WriteBasHDTable( dwEntryNum, atBasHDTable );
	
	// VMPTable
	TEqpVMPInfo atVMPTable[MAX_PRIEQP_NUM];
	dwEntryNum = m_dwVMPEntryNum;
	if( dwEntryNum > 0)
	{
		for( dwLop = 0; dwLop < dwEntryNum; dwLop++ )
		{
			GetEqpVMPCfgByRow( dwLop+1, atVMPTable+dwLop );
		}
	}
	bResult |= WriteVmpTable( dwEntryNum, atVMPTable );
	
	// MpwTable
	TEqpMpwInfo atMpwTable[MAX_PRIEQP_NUM];
	dwEntryNum = m_dwMpwEntryNum;
	if( dwEntryNum > 0)
	{
		for( dwLop = 0; dwLop < dwEntryNum; dwLop++ )
		{
			GetEqpMpwCfgByRow( dwLop+1, atMpwTable+dwLop );
		}
	}
	bResult |= WriteMpwTable( dwEntryNum, atMpwTable );
	//4.6新加版本 jlb
	
	// HduTable
	TEqpHduInfo atHduTable[MAX_PERIHDU_NUM];
	dwEntryNum = m_dwHduEntryNum;
	if( dwEntryNum > 0)
	{
		for( dwLop = 0; dwLop < dwEntryNum; dwLop++ )
		{
			GetEqpHduCfgByRow( dwLop+1, atHduTable+dwLop );
		}
	}
	bResult |= WriteHduTable( dwEntryNum, atHduTable );
	
	// svmpTable
	TEqpSvmpInfo atSvmpTable[MAX_PRIEQP_NUM];
	dwEntryNum = m_dwSvmpEntryNum;
	if( dwEntryNum > 0)
	{
		for( dwLop = 0; dwLop < dwEntryNum; dwLop++ )
		{
			GetEqpSvmpCfgByRow( dwLop+1, atSvmpTable+dwLop );
		}
	}
	bResult |= WriteSvmpTable( dwEntryNum, atSvmpTable );
	
	// dvmpTable
	// 	TEqpDvmpBasicInfo atDvmpTable[MAX_PRIEQP_NUM];
	// 	dwEntryNum = m_dwDvmpEntryNum;
	// 	if( dwEntryNum > 0)
	// 	{
	// 		for( dwLop = 0; dwLop < dwEntryNum; dwLop++ )
	// 		{
	// 			GetEqpDvmpCfgByRow( dwLop+1, atDvmpTable+dwLop );
	// 		}
	// 	}
	// 	bResult |= WriteDvmpTable( dwEntryNum, atDvmpTable );
	
	// MpuBasTable
	TEqpMpuBasInfo atMpuBasTable[MAX_PRIEQP_NUM];
	dwEntryNum = m_dwMpuBasEntryNum;
	if( dwEntryNum > 0)
	{
		for( dwLop = 0; dwLop < dwEntryNum; dwLop++ )
		{
			GetEqpMpuBasCfgByRow( dwLop+1, atMpuBasTable+dwLop );
		}
	}
	bResult |= WriteMpuBasTable( dwEntryNum, atMpuBasTable );
	
	// EbapTable
	// 	TEqpEbapInfo atEbapTable[MAX_PRIEQP_NUM];
	// 	dwEntryNum = m_dwEbapEntryNum;
	// 	if( dwEntryNum > 0)
	// 	{
	// 		for( dwLop = 0; dwLop < dwEntryNum; dwLop++ )
	// 		{
	// 			GetEqpEbapCfgByRow( dwLop+1, atEbapTable+dwLop );
	// 		}
	// 	}
	// 	bResult |= WriteEbapTable( dwEntryNum, atEbapTable );
	
	// EvpuTable
	// 	TEqpEvpuInfo atEvpuTable[MAX_PRIEQP_NUM];
	// 	dwEntryNum = m_dwEvpuEntryNum;
	// 	if( dwEntryNum > 0)
	// 	{
	// 		for( dwLop = 0; dwLop < dwEntryNum; dwLop++ )
	// 		{
	// 			GetEqpEvpuCfgByRow( dwLop+1, atEvpuTable+dwLop );
	// 		}
	// 	}
	// 	bResult |= WriteEvpuTable( dwEntryNum, atEvpuTable );
	
	// DataConfServer
	u32 dwDcsIp = GetDcsIp();
	bResult |= SetDcsIp( dwDcsIp );
	
	// VmpAttachTable
	TVmpAttachCfg atVmpAttachCfg[MAX_VMPSTYLE_NUM];
	dwEntryNum = m_dwVmpAttachNum;
	if( dwEntryNum > 0)
	{
		memcpy( atVmpAttachCfg, m_atVmpAttachCfg, sizeof(TVmpAttachCfg)*m_dwVmpAttachNum );
	}
	bResult |= WriteVmpAttachTable( dwEntryNum, atVmpAttachCfg );
	
	// PrsTable
	TEqpPrsInfo atPrsTable[MAX_PRIEQP_NUM];
	dwEntryNum = m_dwPrsEntryNum;
	if( dwEntryNum > 0)
	{
		for( dwLop = 0; dwLop < dwEntryNum; dwLop++ )
		{
			GetEqpPrsCfgByRow( dwLop+1, atPrsTable+dwLop );
		}
	}
	bResult |= WritePrsTable( dwEntryNum, atPrsTable );
	
	// VrsRecorderTable
	TEqpVrsRecCfgInfo atVrsRecTable[MAX_PRIEQP_NUM];
	dwEntryNum = m_dwVrsRecEntryNum;
	if( dwEntryNum > 0)
	{
		for( dwLop = 0; dwLop < dwEntryNum; dwLop++ )
		{
			GetEqpVrsRecCfgByRow( dwLop+1, atVrsRecTable+dwLop );
		}
	}
	bResult |= WriteVrsRecTable( dwEntryNum,  atVrsRecTable );
	
	// NetSync
	TNetSyncInfo tNetSyncInfo;
	GetNetSyncInfo( &tNetSyncInfo );
	bResult |= SetNetSyncInfo( &tNetSyncInfo ); 
	
	// QosConfigure
	TQosInfo tQosInfo;
	GetQosInfo( &tQosInfo );
	bResult |= SetQosInfo( &tQosInfo );
	
	TEqpExCfgInfo tEqpExCfgInfo;
	GetEqpExCfgInfo( tEqpExCfgInfo );
	bResult |= SetEqpExCfgInfo( tEqpExCfgInfo );
	
	// DscInfo
#ifdef _MINIMCU_
	TDSCModuleInfo tDscInfo;
	GetDSCInfo( &tDscInfo );
    bResult |= SetDSCInfo( &tDscInfo, TRUE );
#endif
	
	if( SUCCESS_AGENT != bResult )
	{
		printf( "[McuAge][WriteAllCfgInfoToCfgfile] Write file failed!\n" );
	}
	
	return bResult;
}

/*=============================================================================
函 数 名： GetBrdIdFrmIdx
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值：
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/19  4.0			付秀华                 创建
=============================================================================*/
u8 CCfgAgent::GetBrdIdFrmIdx(u8 byBrdIdx)
{ 
	for (u8 byIdx = 0; byIdx < MAX_BOARD_NUM; byIdx++)
	{
		if (m_atBrdCfgTable[byIdx].GetBrdId() == byBrdIdx)
		{
			u8 byLayer = m_atBrdCfgTable[byIdx].GetLayer();
			u8 bySlot  = m_atBrdCfgTable[byIdx].GetSlot();
			
			u8 byBrdId;
			if (DSL8000_BRD_MPC == m_atBrdCfgTable[byIdx].GetType() &&
				0 == m_atBrdCfgTable[byIdx].GetSlot())
			{
				byBrdId = MCU_BOARD_MPC;
			}
			else if (DSL8000_BRD_MPC == m_atBrdCfgTable[byIdx].GetType() &&
				1 == m_atBrdCfgTable[byIdx].GetSlot())
			{
				byBrdId = MCU_BOARD_MPCD;
			}
			else
			{
				byBrdId = (byLayer << 4) | (bySlot + 1);
			}
			
			return byBrdId;
		}
	}
	return 0;
}
/*=============================================================================
函 数 名： GetBrdIdxFrmId
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值：
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/19  4.0			付秀华                 创建
=============================================================================*/
u8 CCfgAgent::GetBrdIdxFrmId(u8 byBrdId)
{
	u8 byLayer  = 0;
	u8 bySlot   = 0;
	u8 byTempBrdId = 0;
	for (u8 byIdx = 0; byIdx < MAX_BOARD_NUM; byIdx++)
	{
		if( BRD_TYPE_MPC == m_atBrdCfgTable[byIdx].GetType() || BRD_TYPE_MPC2 == m_atBrdCfgTable[byIdx].GetType() )
		{
			if (MCU_BOARD_MPC == byBrdId)
			{
				if (///*DSL8000_BRD_MPC*/BRD_TYPE_MPC == m_atBrdCfgTable[byIdx].GetType() &&
					0 == m_atBrdCfgTable[byIdx].GetSlot())
				{
					return m_atBrdCfgTable[byIdx].GetBrdId();
				}
			}
			else if (MCU_BOARD_MPCD == byBrdId)
			{
				if (///*DSL8000_BRD_MPC*/BRD_TYPE_MPC == m_atBrdCfgTable[byIdx].GetType() &&
					1 == m_atBrdCfgTable[byIdx].GetSlot())
				{
					return m_atBrdCfgTable[byIdx].GetBrdId();
				}
			}
		}	
		else
		{
			byLayer = m_atBrdCfgTable[byIdx].GetLayer();
			bySlot  = m_atBrdCfgTable[byIdx].GetSlot();
			byTempBrdId = (byLayer << 4) | (bySlot + 1);
			if (byTempBrdId == byBrdId)
			{
				return m_atBrdCfgTable[byIdx].GetBrdId();
			}
		}
		
	}
	return 0;
}

/*=============================================================================
函 数 名： GetEqpSwitchBrdId
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值：
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/19  4.0			付秀华                 创建
=============================================================================*/
u8 CCfgAgent::GetEqpSwitchBrdId(u8 byEqpId)
{
	u32 dwLoop = 0;
	
	// 待完善，目前只实现以下混音器 mpu部分
    for( dwLoop = 0; dwLoop < m_dwMixEntryNum; dwLoop++ )
    {
		if (m_atMixTable[dwLoop].GetEqpId() == byEqpId)
		{
			return GetBrdIdFrmIdx(m_atMixTable[dwLoop].GetSwitchBrdId());
		}
	}
	
    for( dwLoop = 0; dwLoop < m_dwSvmpEntryNum; dwLoop++ )
    {
		if (m_atSvmpTable[dwLoop].GetEqpId() == byEqpId)
		{
			return GetBrdIdFrmIdx(m_atSvmpTable[dwLoop].GetSwitchBrdId());
		}
	}
	
    for( dwLoop = 0; dwLoop < m_dwMpuBasEntryNum; dwLoop++ )
    {
		if (m_atMpuBasTable[dwLoop].GetEqpId() == byEqpId)
		{
			return GetBrdIdFrmIdx(m_atMpuBasTable[dwLoop].GetSwitchBrdId());
		}
	}
	
	return (MAXNUM_DRI + 1);
}
/*=============================================================================
函 数 名： SetEqpSwitchBrdId
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/19  4.0			付秀华                  创建
=============================================================================*/
BOOL32 CCfgAgent::SetEqpSwitchBrdId(u8 byEqpId, u8 bySwitchBrdId)
{
	u32 dwLoop = 0;
	// 待完善，目前只实现以下混音器 mpu部分, 且未保存到文件中
    for( dwLoop = 0; dwLoop < m_dwMixEntryNum; dwLoop++ )
    {
		if (m_atMixTable[dwLoop].GetEqpId() == byEqpId)
		{
			m_atMixTable[dwLoop].SetSwitchBrdId(GetBrdIdxFrmId(bySwitchBrdId));
			return TRUE;
		}
	}
	
    for( dwLoop = 0; dwLoop < m_dwSvmpEntryNum; dwLoop++ )
    {
		if (m_atSvmpTable[dwLoop].GetEqpId() == byEqpId)
		{
			m_atSvmpTable[dwLoop].SetSwitchBrdId(GetBrdIdxFrmId(bySwitchBrdId));
			return TRUE;
		}
	}
	
    for( dwLoop = 0; dwLoop < m_dwMpuBasEntryNum; dwLoop++ )
    {
		if (m_atMpuBasTable[dwLoop].GetEqpId() == byEqpId)
		{
			m_atMpuBasTable[dwLoop].SetSwitchBrdId(GetBrdIdxFrmId(bySwitchBrdId));
			return TRUE;
		}
	}
	
	return FALSE;
}

/*=============================================================================
函 数 名： GetWatchDogOption
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： u8 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/9/18   4.0			周广程                  创建
=============================================================================*/
u8 CCfgAgent::GetWatchDogOption()
{
	s8   achFullName[64];
	u32  dwWDEnable;
	sprintf( achFullName, "%s/%s", DIR_CONFIG, MCUDEBUGFILENAME );
	GetRegKeyInt( achFullName, "mcuWatchDog", "mcuWatchDogEnable", 1, ( s32* )&dwWDEnable );
	
	return (u8) dwWDEnable;
}
// end, zgc, 2007-02-27

/*=============================================================================
函 数 名： IsMcu8000BHD
功    能： 是否是 MCU-8000B-HD
算法实现： 
全局变量： 
参    数： 
返 回 值： u8 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/03/27  4.5			顾振华                  创建
=============================================================================*/
#ifdef _MINIMCU_
BOOL32 CCfgAgent::IsMcu8000BHD() const
{	
    u32 dwBrdType = BrdGetBoardID();
#ifdef _VXWORKS_	
    return FALSE;
	
#elif defined(WIN32) 
	return TRUE;
	
#else
#ifdef _LINUX12_
	return ( dwBrdType == BRD_KDV8000BHD );
#else
	return ( dwBrdType == KDV8000BHD_BOARD );
#endif
#endif
	
}

#endif


/*=============================================================================
函 数 名： Agtlog
功    能： 打印实现
算法实现： 
全局变量： 
参    数： 
返 回 值： API void 
=============================================================================*/
void Agtlog( u8 byPrintLvl, s8* pszFmt, ... )
{
    s8  achPrintBuf[512];
    s32 nBufLen = 0;
    va_list argptr;
	if ( g_byPrintLvl >= byPrintLvl )
	{
		nBufLen = sprintf(achPrintBuf, "[McuAgt]: ");
		va_start(argptr, pszFmt);
		nBufLen += vsprintf(achPrintBuf+nBufLen, pszFmt, argptr);
		va_end(argptr);
		//OspPrintf(TRUE, FALSE, achPrintBuf);
		LogPrint(byPrintLvl, MID_MCULIB_MCUAGT, achPrintBuf);
	}
    return;
}

/*=============================================================================
函 数 名： showeqprecinfo
功    能： 显示录像机信息
算法实现： 
全局变量： 
参    数： 
返 回 值： API void 
=============================================================================*/
API void showrecinfo(void)
{
    g_cCfgParse.ShowRecInfo();
    return;
}

/*=============================================================================
函 数 名： showbasinfo
功    能： 显示Bas信息
算法实现： 
全局变量： 
参    数： 
返 回 值： API void 
=============================================================================*/
API void showbasinfo(void)
{
    g_cCfgParse.ShowBasInfo();
    return;
}

/*=============================================================================
函 数 名： showmixerinfo
功    能： 显示混音器信息
算法实现： 
全局变量： 
参    数： 
返 回 值： API void 
=============================================================================*/
API void showmixerinfo(void)
{
    g_cCfgParse.ShowMixerInfo();
    return;
}

/*=============================================================================
函 数 名： showtvinfo
功    能： 显示TvWall信息
算法实现： 
全局变量： 
参    数： 
返 回 值： API void 
=============================================================================*/
API void showtvinfo(void)
{
    g_cCfgParse.ShowTvInfo();
    return;
}

/*=============================================================================
函 数 名： showvmpinfo
功    能： 显示Vmp信息
算法实现： 
全局变量： 
参    数： 
返 回 值： API void 
=============================================================================*/
API void showvmpinfo(void)
{
    g_cCfgParse.ShowVmpInfo();
    return;
}

/*=============================================================================
函 数 名： showmpwinfo
功    能： 显示多画面电视墙信息
算法实现： 
全局变量： 
参    数： void
返 回 值： API void 
=============================================================================*/
API void showmpwinfo(void)
{
    g_cCfgParse.ShowMpwInfo();
    return;
}

//   0: "mpc", 1: "dti", 2: "dic", 3: "cri",   4: "vas",  5: "mmp", 6: "dri",
//   7:"imt",  8:"apu",  9: "dsi", 10: "vpu", 11: "dec5" , 107: "8000b"
/*=============================================================================
函 数 名： showboardinfo
功    能： 显示单板信息
算法实现： 
全局变量： 
参    数： 
返 回 值： API void 
=============================================================================*/
API void showboardinfo(void)
{
    g_cCfgParse.ShowBoardInfo();
    return;
}

/*=============================================================================
函 数 名： showvmpattachinfo
功    能： 显示VmpAttach信息
算法实现： 
全局变量： 
参    数： void
返 回 值： API void 
=============================================================================*/
API void showvmpattachinfo(void)
{
    g_cCfgParse.ShowVmpAttachInfo();
    return;
}

/*=============================================================================
函 数 名： showqosinfo
功    能： 显示Qos信息
算法实现： 
全局变量： 
参    数： void
返 回 值： API void 
=============================================================================*/
API void showqosinfo(void)
{
    g_cCfgParse.ShowQosInfo();
    return;
}

/*=============================================================================
函 数 名： showlocalinfo
功    能： 显示本地信息
算法实现： 
全局变量： 
参    数： void
返 回 值： API void 
=============================================================================*/
API void showlocalinfo(void)
{
    g_cCfgParse.ShowLocalInfo();
    return;
}

/*=============================================================================
函 数 名： shownetinfo
功    能： 显示网络信息
算法实现： 
全局变量： 
参    数： 
返 回 值： API void 
=============================================================================*/
API void shownetinfo(void)
{
    g_cCfgParse.ShowNetInfo();
    return;
}

/*=============================================================================
函 数 名： showprsinfo
功    能： 显示Prs信息
算法实现： 
全局变量： 
参    数： void
返 回 值： API void 
=============================================================================*/
API void showprsinfo(void)
{
    g_cCfgParse.ShowPrsInfo();
    return;
}

/*=============================================================================
函 数 名： showdcsinfo
功    能： 显示Dcs信息
算法实现： 
全局变量： 
参    数： void
返 回 值： API void 
=============================================================================*/
API void showdcsinfo(void)
{
    g_cCfgParse.ShowDcsInfo();
    return;
}

/*=============================================================================
函 数 名： shownetsyncinfo
功    能： 显示网同步信息
算法实现： 
全局变量： 
参    数： 
返 回 值： API void 
=============================================================================*/
API void shownetsyncinfo(void)
{
    g_cCfgParse.ShowNetsyncInfo();
    return;
}

/*=============================================================================
函 数 名： showdscinfo
功    能： 显示8000B DSC 信息
算法实现： 
全局变量： 
参    数： void
返 回 值： API void 
=============================================================================*/
API void showdscinfo(void)
{
    g_cCfgParse.ShowDscInfo();
    return;
}


/*=============================================================================
函 数 名： showregboard
功    能： 显示当前已经注册单板
算法实现： 
全局变量： 
参    数： void
返 回 值： API void 
=============================================================================*/
API void showregboard(void)
{
    g_cBrdManagerApp.ShowRegedBoard();
    return;
}

/*=============================================================================
函 数 名： showalarm
功    能： 显示当前告警
算法实现： 
全局变量： 
参    数： void
返 回 值： API void 
=============================================================================*/
API void showalarm(void)
{
	g_cAlarmProc.ShowCurAlarm();
    return;
}

/*=============================================================================
函 数 名： showsemhandle
功    能： 显示代理当前所有的互斥量句柄
算法实现： 
全局变量： 
参    数： void
返 回 值： API void 
=============================================================================*/
API void showsemhandle(void)
{
    g_cCfgParse.ShowSemHandleInfo();
    return;
}

/*=============================================================================
函 数 名： dumpinst
功    能： 显示实例信息
算法实现： 
全局变量： 
参    数： void
返 回 值： API void 
=============================================================================*/
API void dumpinst(void)
{
	::OspPost( MAKEIID(AID_MCU_BRDMGR, CInstance::DAEMON),
		TEST_BOARDMANAGERCMD_DUMPINST, NULL, 0 );
    return;
}

/*=============================================================================
函 数 名： showerrorstring
功    能： 显示读配置错误信息
算法实现： 
全局变量： 
参    数： void
返 回 值： API void 
=============================================================================*/
API void showerrorstring(void)
{
    g_cAlarmProc.ShowErrorString();
    return;
}


API void showhduinfo(void)
{
    g_cCfgParse.ShowHduInfo();
}

API void showsvmpinfo(void)
{
    g_cCfgParse.ShowSvmpInfo();
}


API void showmaubasinfo(void)
{
    g_cCfgParse.ShowMauBasInfo();
}

API void showmpubasinfo(void)
{
    g_cCfgParse.ShowMpuBasInfo();
}

API void showeqpinfo( void )
{
	g_cCfgParse.ShowEqpInfo();
}

/*=============================================================================
函 数 名： setagtlog
功    能： 设置打印级别
算法实现： 
全局变量： 
参    数： void
返 回 值： API void 
=============================================================================*/
API void setagtlog(u8 byLevel)
{
    if (byLevel > LOG_VERBOSE) 
    {
        g_byPrintLvl = LOG_VERBOSE;
    }
    else
    {
        g_byPrintLvl = byLevel;
    }

	if( g_byPrintLvl > 0 ) 
	{
		logenablemod(MID_MCULIB_MCUAGT);
	}
	else
	{
		logdisablemod(MID_MCULIB_MCUAGT);
	}
    return;
}

/*=============================================================================
函 数 名： setethbak
功    能： 设置是否进行网口备份
算法实现： 
全局变量： 
参    数： u8 byEnable (值为0 表不进行网口备份)
返 回 值： API void 
=============================================================================*/
API void setethbak(u8 byEnble)
{
#if defined(_LINUX_) && ( defined(_8KE_) || defined(_8KH_) || defined(_8KI_))
	g_byEthBak = byEnble;
#else
	OspPrintf(TRUE, FALSE, "this command is only used in 8000G system!Enable.%d\n",byEnble);
#endif
}

/*=============================================================================
函 数 名： showboxfanalarm
功    能： 机箱风扇告警测试命令
算法实现： 
全局变量： 
参    数： u8   byFanId             风扇ID 机箱有8个风扇 ID号为 0 - 7；
u16  wFanSpeed           风扇速度 大于等于2000为正常；小于2000为异常；
u8   m_byBoxFanStatus    测试中控制发告警的状态值
返 回 值： void 
-----------------------------------------------------------------------------
修改记录:
日  期        版本       修改人            修改内容
2010/12/16    5.0        苗庆松              创建
=============================================================================*/
API void showboxfanalarm( u8 byFanId, u16 wFanSpeed, u8 m_byBoxFanStatus )
{
	TBoxFanStatus       tBoxFanStatus;              
	
	for ( ; byFanId < MCU8000ABOXFAN_NUM; byFanId++ )
	{
		if ( (wFanSpeed < MIN_MCU8000ABOXFAN_SPEED) && ((((unsigned int)1<<byFanId) & m_byBoxFanStatus) == 0) )
		{
			tBoxFanStatus.bySlot = byFanId / 2;
			tBoxFanStatus.byFanPos = byFanId % 2 + 1;
			tBoxFanStatus.byStatus = 1;
			
			OspPost( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_BOX_FAN_STATUS, &tBoxFanStatus, sizeof(tBoxFanStatus) );
			OspPrintf( TRUE, FALSE, "[SVC_AGT_BOX_FAN_STATUS] Alarm generate Message post succeed!\n" );
			
			m_byBoxFanStatus |= (unsigned int)1 << byFanId;
		}
		else
		{
			if ( 0 < (((unsigned int)1<<byFanId) & m_byBoxFanStatus) ) 
				
			{
				tBoxFanStatus.bySlot = byFanId / 2;
				tBoxFanStatus.byFanPos = byFanId % 2 + 1;
				tBoxFanStatus.byStatus = 0;
				
				OspPost( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_BOX_FAN_STATUS, &tBoxFanStatus, sizeof(tBoxFanStatus) );
				OspPrintf( TRUE, FALSE, "[SVC_AGT_BOX_FAN_STATUS] Alarm restore Message post succeed!\n" );
				
				m_byBoxFanStatus ^= (unsigned int)1 << byFanId;      
			}
		}
	}
}

/*=============================================================================
函 数 名： showmpccpualaem
功    能： MPC板CPU占用率告警
算法实现： 
全局变量： 
参    数： u8 byMpcCpuAllocRate  Mpc板Cpu占用率 大于等于85%为异常；
u8 m_byMPCCpuStatus   测试中控制发告警的状态值
返 回 值： void 
-----------------------------------------------------------------------------
修改记录:
日  期        版本       修改人            修改内容
2010/12/16    5.0        苗庆松              创建
=============================================================================*/
API void showmpccpualarm( u8 byMpcCpuAllocRate, u8 m_byMPCCpuStatus )
{
	TMPCCpuStatus  tMpcCpuStatus;
	
	tMpcCpuStatus.byCpuAllocRate = byMpcCpuAllocRate;
	
	if ( (byMpcCpuAllocRate >= 85) && (m_byMPCCpuStatus == 0) )    // Cpu占用率超过85%
	{		
		tMpcCpuStatus.byCpuStatus = 1;
		
		OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_CPU_STATUS, &tMpcCpuStatus, sizeof(tMpcCpuStatus));
        OspPrintf( TRUE, FALSE, "[SVC_AGT_CPU_STATUS] Alarm generate Message post succeed!\n");
		
		m_byMPCCpuStatus = 1;
	}
	
	if ( (tMpcCpuStatus.byCpuAllocRate < 85) && ( m_byMPCCpuStatus == 1) )
	{
		tMpcCpuStatus.byCpuStatus = 0;
		
		OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_CPU_STATUS, &tMpcCpuStatus,sizeof(tMpcCpuStatus));
        OspPrintf( TRUE, FALSE, "[SVC_AGT_CPU_STATUS] Alarm restore Message post succeed!\n");
		
		m_byMPCCpuStatus = 0;
	}       
}

/*=============================================================================
函 数 名： showmpcmemoryalarm
功    能： MPC板Memory使用率告警
算法实现： 
全局变量： 
参    数： u8 byMpcMemoryAllocRate     Mpc板Memory占用率 大于等于85%为异常；
u8 m_byMPCMemoryStatus      测试中控制发告警的状态值.
返 回 值： void 
-----------------------------------------------------------------------------
修改记录:
日  期        版本       修改人            修改内容
2010/12/16    5.0        苗庆松              创建
=============================================================================*/
API void showmpcmemoryalarm( u8 byMpcMemoryAllocRate,u8 m_byMPCMemoryStatus )
{
	TMPCMemoryStatus tMpcMemoryStatus;
	
	tMpcMemoryStatus.byMemoryAllocRate = byMpcMemoryAllocRate;
	
	if ( (byMpcMemoryAllocRate >= 85) && (m_byMPCMemoryStatus == 0) )    // 内存使用率超过85%
	{	
		tMpcMemoryStatus.byMemoryStatus = 1;
		
		OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_MPCMEMORY_STATUS, &tMpcMemoryStatus, sizeof(tMpcMemoryStatus));
        OspPrintf( TRUE, FALSE, "[SVC_AGT_MPCMEMORY_STATUS] Alarm generate Message post succeed!\n");
		
		m_byMPCMemoryStatus = 1;
	}
	
	if ( (tMpcMemoryStatus.byMemoryAllocRate < 85) && (m_byMPCMemoryStatus == 1) )
	{
		tMpcMemoryStatus.byMemoryStatus = 0;
		
		OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_MPCMEMORY_STATUS, &tMpcMemoryStatus, sizeof(tMpcMemoryStatus));
        OspPrintf( TRUE, FALSE, "[SVC_AGT_MPCMEMORY_STATUS] Alarm restore Message post succeed!\n");
		
		m_byMPCMemoryStatus = 0;
	}
}

/*=============================================================================
函 数 名： showpowertempalarm
功    能： 电源板温度告警测试命令
算法实现： 
全局变量： 
参    数： u8 byPowerBrdTemp           电源板温度：0x01位表示左电源状态，0，温度正常；1，温度异常
0x02位表示右电源状态，0，温度正常；1，温度异常
u8 m_byPowerTempStatus      测试中控制发告警的状态值.
返 回 值： void 
-----------------------------------------------------------------------------
修改记录:
日  期        版本       修改人            修改内容
2010/12/16    5.0        苗庆松              创建
=============================================================================*/
API void showpowertempalarm( u8 byPowerBrdTemp, u8 m_byPowerTempStatus )
{
	TPowerBrdTempStatus    tPowerBrdTempStatus;
	
	if( 1 == (byPowerBrdTemp & 0x01) )              
	{
		if ( m_byPowerTempStatus == 0 )
		{
			tPowerBrdTempStatus.bySlot = POWER_LEFT;
			tPowerBrdTempStatus.byPowerBrdTempStatus = 1;
			
			OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_POWERTEMP_STATUS, &tPowerBrdTempStatus, sizeof(tPowerBrdTempStatus) );
            OspPrintf( TRUE, FALSE, "[SVC_AGT_POWERTEMP_STATUS] Alarm generate Message post succeed!\n");
			
			m_byPowerTempStatus = 1;
		}
		
	}
	else
	{
		if ( m_byPowerTempStatus == 1 )
		{
			tPowerBrdTempStatus.bySlot = POWER_LEFT;
			tPowerBrdTempStatus.byPowerBrdTempStatus = 0;
			
			OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_POWERTEMP_STATUS, &tPowerBrdTempStatus, sizeof(tPowerBrdTempStatus) );
			OspPrintf( TRUE, FALSE, "[SVC_AGT_POWERTEMP_STATUS] Alarm restore Message post succeed!\n");
			
			m_byPowerTempStatus = 0;
		}
	}
	
	if ( 2 == (byPowerBrdTemp & 0x02) )              
	{
		if ( m_byPowerTempStatus == 0 )
		{
			tPowerBrdTempStatus.bySlot = POWER_RIGHT;
			tPowerBrdTempStatus.byPowerBrdTempStatus = 1;
			
			OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_POWERTEMP_STATUS, &tPowerBrdTempStatus, sizeof(tPowerBrdTempStatus) );
			OspPrintf( TRUE, FALSE, "[SVC_AGT_POWERTEMP_STATUS] Alarm generate Message post succeed!\n");
			
			m_byPowerTempStatus = 1;
		}
		
	}
	else
	{
		if ( m_byPowerTempStatus == 1 )
		{
			tPowerBrdTempStatus.bySlot = POWER_RIGHT;
			tPowerBrdTempStatus.byPowerBrdTempStatus = 0;
			
			OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_POWERTEMP_STATUS, &tPowerBrdTempStatus, sizeof(tPowerBrdTempStatus) );
			OspPrintf( TRUE, FALSE, "[SVC_AGT_POWERTEMP_STATUS] Alarm restore Message post succeed!\n");
			
			m_byPowerTempStatus = 0;
		}
	}
}

/*=============================================================================
函 数 名： showmpc2tempalarm
功    能： MPC2板温度异常告警测试命令
算法实现： 
全局变量： 
参    数： u8 m_byMpc2Tempvalue       Mpc2板实时温度值 大于等于50%为异常；
u8 m_byMPC2TempStatus      测试中控制发告警的状态值. 
返 回 值： void 
-----------------------------------------------------------------------------
修改记录:
日  期        版本       修改人            修改内容
2011/01/13    5.0        苗庆松              创建
=============================================================================*/
API void showmpc2tempalarm( u8 byMpc2Tempvalue, u8 byMPC2TempStatus )
{
	TMPC2TempStatus tMpc2TempStatus;
	
	tMpc2TempStatus.dwMpc2Temp = byMpc2Tempvalue;
	
	if ( (byMpc2Tempvalue >= 50) && (byMPC2TempStatus == 0) )    // Mpc2板温度超过50度
	{	
		tMpc2TempStatus.byMpc2TempStatus = 1;
		
		OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_MPC2TEMP_STATUS, &tMpc2TempStatus, sizeof(tMpc2TempStatus));
        OspPrintf( TRUE, FALSE, "[SVC_AGT_MPC2TEMP_STATUS] Alarm generate Message post succeed!\n");
		
		byMPC2TempStatus = 1;
	}
	
	if ( (byMpc2Tempvalue < 50) && (byMPC2TempStatus == 1) )
	{
		tMpc2TempStatus.byMpc2TempStatus = 0;
		
		OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_MPC2TEMP_STATUS, &tMpc2TempStatus, sizeof(tMpc2TempStatus));
        OspPrintf( TRUE, FALSE, "[SVC_AGT_MPC2TEMP_STATUS] Alarm restore Message post succeed!\n");
		
		byMPC2TempStatus = 0;
	}
}

/*=============================================================================
函 数 名： showe1linkalarm
功    能： E1线路异常告警测试命令
算法实现： 
全局变量： 
参    数： u8  byBrdLayer   层号
u8  byBrdSlot    槽号
u8  byBrdType    板类型
u8  byE1Number   E1号
u8  byStatus     状态   bit0  E1失同步   bit1 E1失载波       bit2 E1远端告警	
bit3  E1全1      bit4 CRC复帧错误    bit5 随路复帧错误
返 回 值： void 
-----------------------------------------------------------------------------
修改记录:
日  期        版本       修改人            修改内容
2011/02/21    5.0        苗庆松              创建
=============================================================================*/
API void showe1linkalarm( u8 byBrdLayer, u8 byBrdSlot, u8 byBrdType, u8 byE1Number, u8 byStatus )
{
	TLinkStatus tLinkStatus;
	
	tLinkStatus.byLayer  =  byBrdLayer;
	tLinkStatus.bySlot   =  byBrdSlot;
	tLinkStatus.byType   =  byBrdType;
	tLinkStatus.byE1Num  =  byE1Number;
	tLinkStatus.byStatus =  byStatus;
	
	OspPost( MAKEIID( AID_MCU_AGENT, 1 ), MCU_AGT_LINK_STATUSCHANGE, &tLinkStatus, sizeof( tLinkStatus ) );			
}

/*=============================================================================
函 数 名： showbrdcpualarm
功    能： 单板cpu异常告警测试命令
算法实现： 
全局变量： 
参    数： u8  byBrdLayer   层号
u8  byBrdSlot    槽号
u8  byBrdType    板类型
u8  byStatus     状态   1异常 0正常
返 回 值： void 
-----------------------------------------------------------------------------
修改记录:
日  期        版本       修改人            修改内容
2011/10/26    5.0        陈代伟          创建
=============================================================================*/
API void showbrdcpualarm( u8 byBrdLayer, u8 byBrdSlot, u8 byBrdType,u8 byStatus )
{
	TBrdStatus tBrdStatus;
	memset(&tBrdStatus,0,sizeof(tBrdStatus));
	tBrdStatus.byLayer  =  byBrdLayer;
	tBrdStatus.bySlot   =  byBrdSlot;
	tBrdStatus.byType   =  byBrdType;
	tBrdStatus.byStatus =  byStatus;
	
	OspPost( MAKEIID( AID_MCU_AGENT, 1 ), SVC_AGT_BRD_CPU_STATUS_NOTIF, &tBrdStatus, sizeof( tBrdStatus ) );			
}

/*=============================================================================
函 数 名： showbrdtempalarm
功    能： 单板温度异常告警测试命令
算法实现： 
全局变量： 
参    数： u8  byBrdLayer   层号
u8  byBrdSlot    槽号
u8  byBrdType    板类型
u8  byStatus     状态   1异常 0正常
返 回 值： void 
-----------------------------------------------------------------------------
修改记录:
日  期        版本       修改人            修改内容
2011/10/26    5.0        陈代伟          创建
=============================================================================*/
API void showbrdtempalarm( u8 byBrdLayer, u8 byBrdSlot, u8 byBrdType,u8 byStatus )
{
	TBrdStatus tBrdStatus;
	memset(&tBrdStatus,0,sizeof(tBrdStatus));
	tBrdStatus.byLayer  =  byBrdLayer;
	tBrdStatus.bySlot   =  byBrdSlot;
	tBrdStatus.byType   =  byBrdType;
	tBrdStatus.byStatus =  byStatus;
	
	OspPost( MAKEIID( AID_MCU_AGENT, 1 ), SVC_AGT_BRD_TEMPERATURE_STATUS_NOTIFY, &tBrdStatus, sizeof( tBrdStatus ) );			
}

/*=============================================================================
函 数 名： showsynsrcalarm
功    能： 同步源异常告警测试命令
算法实现： 
全局变量： 
参    数： u8 byStatus    0  正常态; 1 异常态
返 回 值： void 
-----------------------------------------------------------------------------
修改记录:
日  期        版本       修改人            修改内容
2011/02/21    5.0        苗庆松              创建
=============================================================================*/
API void showsyncsrcalarm( u8 byStatus )
{
	u8 tSyncsrcStatus = 0;
	tSyncsrcStatus = byStatus;
	
	OspPost( MAKEIID( AID_MCU_AGENT, 1 ), MCU_AGT_SYNCSOURCE_STATUSCHANGE, &tSyncsrcStatus, sizeof( tSyncsrcStatus ) );	
}

/*=============================================================================
函 数 名： showfilesystemalarm
功    能： 文件系统出错告警测试命令
算法实现： 
全局变量： 
参    数：  u8  byBrdLayer    层号
u8  byBrdSlot     槽号
u8  byBrdType     板类型
u8  byFileStatus  状态      0 正常态；1 异常态
返 回 值： void 
-----------------------------------------------------------------------------
修改记录:
日  期        版本       修改人            修改内容
2011/02/21    5.0        苗庆松              创建
=============================================================================*/
API void showfilesystemalarm( u8 byBrdLayer, u8 byBrdSlot, u8 byBrdType, u8 byFileStatus )
{
	TSoftwareStatus tSoftStatus;
	
	tSoftStatus.byLayer  = byBrdLayer;
	tSoftStatus.bySlot   = byBrdSlot;
	tSoftStatus.byType   = byBrdType;
	tSoftStatus.byStatus = byFileStatus;
	
	OspPost( MAKEIID( AID_MCU_AGENT, 1 ), SVC_AGT_FILESYSTEM_STATUS, &tSoftStatus, sizeof( tSoftStatus ) );		
}

/*=============================================================================
函 数 名： showpoweralarm
功    能： 电源板异常告警测试命令
算法实现： 
全局变量： 
参    数：  u8  byPowerSlot     电源槽号  1  左电源；2  右电源
u8  byPowerType     电源类型  1  5V电源；2  48V电源
u8  byPowerStatus   电源状态  0  正常态；1  异常态
返 回 值：  void 
-----------------------------------------------------------------------------
修改记录:
日  期        版本       修改人            修改内容
2011/04/26    5.0        苗庆松              创建
=============================================================================*/
API void showpoweralarm( u8 byPowerSlot, u8 byPowerType, u8 byPowerStatus )
{
	TPowerStatus tPowerStatus;
	
	tPowerStatus.bySlot   = byPowerSlot;
	tPowerStatus.byType   = byPowerType;
	tPowerStatus.byStatus = byPowerStatus;
	
	OspPost( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_POWER_STATUS, &tPowerStatus, sizeof(tPowerStatus) );
}

/*=============================================================================
函 数 名： showpowerfanalarm
功    能： 电源风扇停转告警测试命令
算法实现： 
全局变量： 
参    数：  u8  byPowerFanSlot     电源槽号  1  左电源；2  右电源
u8  byPowerFanType     电源类型  1  左风扇；2  右风扇
u8  byPowerFanStatus   电源状态  0  正常态；1  异常态
返 回 值：  void 
-----------------------------------------------------------------------------
修改记录:
日  期        版本       修改人            修改内容
2011/04/26    5.0        苗庆松              创建
=============================================================================*/
API void showpowerfanalarm( u8 byPowerFanSlot, u8 byPowerFanType, u8 byPowerFanStatus )
{
	TPowerFanStatus     tPowerFanStatus;
	
	tPowerFanStatus.bySlot = byPowerFanSlot;
	tPowerFanStatus.byFanPos = byPowerFanType;
	tPowerFanStatus.byStatus = byPowerFanStatus;
	
	OspPost( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_POWER_FAN_STATUS, &tPowerFanStatus, sizeof(tPowerFanStatus) );
}

/*=============================================================================
函 数 名： showfilesystemalarm
功    能： 单板异常 /单板配置冲突告警测试命令
算法实现： 
全局变量： 
参    数：  u8  byBrdLayer    层号
u8  byBrdSlot     槽号
u8  byBrdType     板类型
u8  byBrdStatus   状态    0 正常；1 断链；2 配置冲突
返 回 值：  void 
-----------------------------------------------------------------------------
修改记录:
日  期        版本       修改人            修改内容
2011/02/21    5.0        苗庆松              创建
=============================================================================*/
API void showboardalarm( u8  byBrdLayer, u8  byBrdSlot, u8  byBrdType, u8 byBrdStatus )
{
	TBrdStatus tBrdStatus;
	
	tBrdStatus.byLayer  = byBrdLayer;
	tBrdStatus.bySlot   = byBrdSlot;
	tBrdStatus.byType   = byBrdType;
	tBrdStatus.byStatus = byBrdStatus;
	
	OspPost( MAKEIID(AID_MCU_AGENT, 1), MCU_AGT_BOARD_STATUSCHANGE, (u8*)&tBrdStatus, sizeof(tBrdStatus) );
}

/*=============================================================================
函 数 名： showdriverinterface
功    能： 会议状态通告Trap测试命令
算法实现： 
全局变量： 
参    数： 
返 回 值： void 
-----------------------------------------------------------------------------
修改记录:
日  期        版本       修改人            修改内容
2011/06/21    5.0        苗庆松              创建
=============================================================================*/
API void showdriverinterface()
{ 
#ifdef _LINUX12_
	lm75 tSensor;
	u32 dwReturn = BrdGetSensorStat( &tSensor );
	if ( OK == dwReturn )
	{
		printf( "[BrdGetSensorStat] Success Get Mpc2 temperature!\n");
	} 
	else
	{
		printf( "[BrdGetSensorStat] Error Get Mpc2 temperature.\n");
	}
	
	u8 byPowerBrdTemp = 0;
	u32 dwRet = BrdMPCLedBrdTempCheck( &byPowerBrdTemp );
	if ( OK == dwRet )
	{
		printf( "[BrdMPCLedBrdTempCheck] Success Get Power's board temperature!\n" );
	} 
	else
	{
		printf( "[BrdMPCLedBrdTempCheck] Error Get Power's board temperature!\n" );
	}
	
	TBrdLedState tBrdLedState;
    if (OK != BrdQueryLedState(&tBrdLedState)) 
    {
        printf( "[ProcGuardStateScan]  Error BrdQueryLedState !\n" );
    }
    else
    { 
       	printf( "[ProcGuardStateScan] Success BrdQueryLedState !\n" );        
    }
#endif
}

/*=============================================================================
函 数 名： showconfstatustrap
功    能： 会议状态通告Trap测试命令
算法实现： 
全局变量： 
参    数： 
返 回 值： void 
-----------------------------------------------------------------------------
修改记录:
日  期        版本       修改人            修改内容
2010/12/17    5.0        苗庆松              创建
=============================================================================*/
API void showconfstatus( s8* abyConfId, s8* abyConfName)
{
	// [10/25/2011 liuxu] 非空判断, 否则会出错
	if ( !abyConfId || !abyConfName )
	{
		StaticLog("[showconfstatus] param invalid\n");
		return;
	}

    TConfNotify tConfNotify;
	memset( &tConfNotify, 0, sizeof(tConfNotify));
    
	memcpy( tConfNotify.m_abyConfId, abyConfId, sizeof(abyConfId) );
	memcpy( tConfNotify.m_abyConfName, abyConfName, sizeof(abyConfName) );
	
	OspPost( MAKEIID( AID_MCU_AGENT, 1 ), SVC_AGT_CONFERENCE_START, (u8*)&tConfNotify, sizeof(tConfNotify) );
	OspPrintf( TRUE, FALSE,"[SVC_AGT_CONFERENCE_START] Message send succeed!\n");
}

/*=============================================================================
函 数 名： show8kiledstatus
功    能： 8kirun灯和alarm灯告警测试
算法实现： 
全局变量： 
参    数： 
返 回 值： void 
-----------------------------------------------------------------------------
修改记录:
日  期        版本       修改人            修改内容
2013/11/18    5.0        周晶晶              创建
=============================================================================*/
API void show8kiledstatus( u8 byRunLed,u8 byAlarmLed )
{
	TBrdLedState tBrdLedState;
#ifdef _8KI_
	if (byRunLed > 0 && byAlarmLed > 0)
	{
		tBrdLedState.dwLedNum = sizeof(TBrdMPCLedStateDesc);
		memset(&tBrdLedState.nlunion.tBrdMPCLedState, BRD_LED_OFF, sizeof(tBrdLedState.nlunion.tBrdMPCLedState));//初始化为灯灭
		tBrdLedState.nlunion.tBrdMPCLedState.byLedRUN = byRunLed;
		tBrdLedState.nlunion.tBrdMPCLedState.byLedALM = byAlarmLed;
	}
	else
	{
		StaticLog("[show8kiledstatus] byRunLed and byAlarmLed Must be greater than 0.\n");
	}
#endif
	OspPost( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_LED_STATUS, &tBrdLedState, sizeof(tBrdLedState) );
}

/*=============================================================================
函 数 名： show8kicputmpandcpufan
功    能： 8kicpu温度和风扇转速设置
算法实现： 
全局变量： 
参    数： 
返 回 值： void 
-----------------------------------------------------------------------------
修改记录:
日  期        版本       修改人            修改内容
2013/11/18    5.0        周晶晶              创建
=============================================================================*/
API void  show8kicputmpandcpufan(u8 byCpuTemp, u32 dwCpuFan )
{
#ifdef _8KI_
	u8 byCpuTempState = dwCpuFan <= 85 ? 0 : 1;//cpu温度小于等于85度为正常值
	u8 byCpuFanState = dwCpuFan >= 100 ? 0 : 1;//风扇转速大于等于100为正常值
	OspPost( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_CPU_FAN_STATUS, &byCpuFanState, sizeof(byCpuFanState));
	OspPost( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_CPUTEMP_STATUS, &byCpuTempState, sizeof(byCpuTempState));
#endif
}

/*=============================================================================
函 数 名： setethlog
功    能： 设置底层以太网测试情况打印级别
算法实现： 
全局变量： 
参    数： void
返 回 值： API void 
=============================================================================*/
API void setethlog(u8 byLevel)
{
#ifdef _LINUX_
    BrdEthPrintEnable( 0 == byLevel ? FALSE : TRUE );
#endif
    return;
}

/*=============================================================================
函 数 名： mcuagtver
功    能：  打印版本
算法实现： 
全局变量： 
参    数： void
返 回 值： API void 
=============================================================================*/
API void mcuagtver(void)
{
	OspPrintf( TRUE, FALSE, "McuAgent: %s  compile time: %s    %s\n",  
		VER_MCUAGENT, __DATE__, __TIME__ );
}

const s8* GetBoardIDStr()
{
    static s8 szDescStr[16];
#ifndef WIN32
    u32 dwBrdId = BrdGetBoardID();
    switch (dwBrdId)
    {
#ifdef _LINUX12_
    case BRD_HWID_MPC2:		
#else
    case BRD_TYPE_MPC/*DSL8000_BRD_MPC*/:		
#endif
        strcpy( szDescStr, "8000MPC");
        break;
    case KDV8000B_BOARD:
        strcpy( szDescStr, "8000B");
        break;
#ifndef _VXWORKS_
#ifndef _LINUX12_
	case KDV8000BHD_BOARD:
		strcpy( szDescStr, "8000B-HD");
		break;
    case KDV8000C_BOARD:
        strcpy( szDescStr, "8000C");
        break;
#else
	case BRD_KDV8000BHD:
		strcpy( szDescStr, "8000B-HD");
		break;
#endif
#endif
    default:
        sprintf( szDescStr, "Unknown(%d)", dwBrdId);
        break;
    }
#else //WIN32
    strcpy( szDescStr, "Windows");
#endif
    
    return szDescStr;
}

const s8* GetMcuSoftVersion()
{
    static s8 ms_VersionBuf[128] = {0};
    if ( strlen(ms_VersionBuf) != 0 )
    {
        return ms_VersionBuf;
    }
	
    strcpy(ms_VersionBuf, KDV_MCU_PREFIX);
    
    s8 achMon[16] = {0};
    u32 byDay = 0;
    u32 byMonth = 0;
    u32 wYear = 0;
    static s8 achFullDate[24] = {0};
    
    s8 achDate[32] = {0};
    sprintf(achDate, "%s", __DATE__);
    StrUpper(achDate);
    
    sscanf(achDate, "%s %d %d", achMon, &byDay, &wYear );
    
    if ( 0 == strcmp( achMon, "JAN") )		 
        byMonth = 1;
    else if ( 0 == strcmp( achMon, "FEB") )
        byMonth = 2;
    else if ( 0 == strcmp( achMon, "MAR") )
        byMonth = 3;
    else if ( 0 == strcmp( achMon, "APR") )		 
        byMonth = 4;
    else if ( 0 == strcmp( achMon, "MAY") )
        byMonth = 5;
    else if ( 0 == strcmp( achMon, "JUN") )
        byMonth = 6;
    else if ( 0 == strcmp( achMon, "JUL") )
        byMonth = 7;
    else if ( 0 == strcmp( achMon, "AUG") )
        byMonth = 8;
    else if ( 0 == strcmp( achMon, "SEP") )		 
        byMonth = 9;
    else if ( 0 == strcmp( achMon, "OCT") )
        byMonth = 10;
    else if ( 0 == strcmp( achMon, "NOV") )
        byMonth = 11;
    else if ( 0 == strcmp( achMon, "DEC") )
        byMonth = 12;
    else
        byMonth = 0;
    
    if ( byMonth != 0 )
    {
        sprintf(achFullDate, "%04d%02d%02d", wYear, byMonth, byDay);
        sprintf(ms_VersionBuf, "%s%s", KDV_MCU_PREFIX, achFullDate);        
    }
    else
    {
        sprintf(ms_VersionBuf, "%s%s", KDV_MCU_PREFIX, achFullDate);        
    }
    
    return ms_VersionBuf;
}

API void mcuagenthelp(void)
{
    mcuagtver();
	
    OspPrintf(TRUE, FALSE, "Mcu Running on Board: %s\n", GetBoardIDStr());
    OspPrintf(TRUE, FALSE, "Mcu Agent Help Menu as follows:\n");
	
    OspPrintf(TRUE, FALSE, "  \tsetagtlog(u8 byLvl)  ----> Set agent print Level: 0.err;1.warning;2.info;3.verbose.\n");
    OspPrintf(TRUE, FALSE, "  \tmcuagtver            ----> Show agent version.\n");
	OspPrintf(TRUE, FALSE, "  \tdumpinst        ----> Show All Inst info.\n\n");
	
    OspPrintf(TRUE, FALSE, "  \tshowlocalinfo   ----> Show Local info.\n");
    OspPrintf(TRUE, FALSE, "  \tshowqosinfo     ----> Show Qos   info.\n");	
    OspPrintf(TRUE, FALSE, "  \tshownetinfo     ----> Show Network info.\n");
    OspPrintf(TRUE, FALSE, "  \tshownetsyncinfo ----> Show Netsync info.\n\n");
	
    OspPrintf(TRUE, FALSE, "  \tshowboardinfo   ----> Show All board info.\n");
    OspPrintf(TRUE, FALSE, "  \tshowregboard    ----> Show Reg board info.\n\n");
	
	OspPrintf(TRUE, FALSE, "  \tshowbasinfo     ----> Show Bas info.\n");
    OspPrintf(TRUE, FALSE, "  \tshowrecinfo     ----> Show Rec info.\n");
	OspPrintf(TRUE, FALSE, "  \tshowmpwinfo     ----> Show Mpw info.\n");
    OspPrintf(TRUE, FALSE, "  \tshowvmpinfo     ----> Show Vmp info.\n");
    OspPrintf(TRUE, FALSE, "  \tshowprsinfo     ----> Show Prs info.\n");
    OspPrintf(TRUE, FALSE, "  \tshowdcsinfo     ----> Show Dcs info.\n");
    OspPrintf(TRUE, FALSE, "  \tshowmixerinfo   ----> Show Mixer info.\n");
	
    OspPrintf(TRUE, FALSE, "  \tshowhduinfo     ----> Show hdu info.\n");
	
    OspPrintf(TRUE, FALSE, "  \tshowmaubasinfo  ----> Show mpu-Bas info.\n");
    OspPrintf(TRUE, FALSE, "  \tshowmpubasinfo  ----> Show mau-Bas info.\n");
	
    OspPrintf(TRUE, FALSE, "  \tshowsvmpinfo     ----> Show svmp info.\n");
	
	
    OspPrintf(TRUE, FALSE, "  \tshowtvinfo      ----> Show TvWall info.\n");
    OspPrintf(TRUE, FALSE, "  \tshowdscinfo      ----> Show 8000B DSC Board Module info.\n");
    OspPrintf(TRUE, FALSE, "  \tshowvmpattachinfo ----> Show Vmp attach info.\n\n");    
	
    OspPrintf(TRUE, FALSE, "  \tshowerrorstring ----> Show Error string.\n");
    OspPrintf(TRUE, FALSE, "  \tshowalarm       ----> Show Alarm table.\n");
    OspPrintf(TRUE, FALSE, "  \tshowsemhandle   ----> Show Sem handle.\n");
    OspPrintf(TRUE, FALSE, "  \tsetethlog       ----> Set eth info print: 0.nonprint, >=1.print.\n");    
	OspPrintf(TRUE, FALSE, "  \tsetethbak       ----> [only used in 8000G system] Set eth bak enalbe or not: 0.disable, >=1.enable.\n");  
    return;
}
/*lint -save -e611*/
void AgtAPIEnableInLinux()
{       
#ifdef _LINUX_
	OspRegCommand("mcuagenthelp",		(void*)mcuagenthelp,		"show help info");
	OspRegCommand("mcuagtver",			(void*)mcuagtver,			"show agent ver info");
    OspRegCommand("setagtlog",          (void*)setagtlog,			"set print level");
    OspRegCommand("showlocalinfo",		(void*)showlocalinfo,       "show local info");
    OspRegCommand("showqosinfo",		(void*)showqosinfo,		    "show qos   info");
    OspRegCommand("shownetinfo",		(void*)shownetinfo,		    "show net   info");
    OspRegCommand("shownetsyncinfo",    (void*)shownetsyncinfo,     "show netsync info");
    OspRegCommand("showboardinfo",		(void*)showboardinfo,       "show board info");
    OspRegCommand("showregboard",		(void*)showregboard,		"show reged board");
	OspRegCommand("showbasinfo",		(void*)showbasinfo,		    "show bas   info");
	OspRegCommand("showdcsinfo",		(void*)showdcsinfo,		    "show dcs   info");
	OspRegCommand("showmixerinfo",		(void*)showmixerinfo,       "show mixer info");
	OspRegCommand("showmpwinfo",		(void*)showmpwinfo,		    "show mpw   info");
	OspRegCommand("showprsinfo",		(void*)showprsinfo,		    "show prs   info");
	OspRegCommand("showrecinfo",		(void*)showrecinfo,		    "show rec   info");
    OspRegCommand("showvmpinfo",		(void*)showvmpinfo,		    "show vmp   info");
	OspRegCommand("showtvinfo",		    (void*)showtvinfo,			"show tvwall info");
    OspRegCommand("showdscinfo",		(void*)showdscinfo,			"Show 8000B DSC Board Module info");
	OspRegCommand("showvmpattachinfo",  (void*)showvmpattachinfo,   "show vmpattach info");
	
    OspRegCommand("showhduinfo",        (void*)showhduinfo,         "show hdu info");
	
    OspRegCommand("showmaubasinfo",     (void*)showmaubasinfo,      "show mau bas info");
    OspRegCommand("showmpubasinfo",     (void*)showmpubasinfo,      "show mpu bas info");
    OspRegCommand("showsvmpinfo",       (void*)showsvmpinfo,        "show svmp info");
	OspRegCommand("showeqpinfo",       (void*)showeqpinfo,			"show eqp info");
	
	OspRegCommand("showalarm",			(void*)showalarm,			"show alarm info ");
	OspRegCommand("showerrorstring",    (void*)showerrorstring,     "show errorstring");
    OspRegCommand("showsemhandle",      (void*)showsemhandle,       "show all the sem handle");
	OspRegCommand("dumpinst",			(void*)dumpinst,			"dumpinst");
    OspRegCommand("setethlog",          (void*)setethlog,           "set eth print log");
	OspRegCommand("setethbak",			(void*)setethbak,			"set eth bak enable/disable");
	
	// [20101216 miaoqingsong add]     告警功能测试命令
    OspRegCommand("showboxfanalarm",    (void*)showboxfanalarm,     "show box fan alarm info");
	OspRegCommand("showmpccpualarm",    (void*)showmpccpualarm,     "show mpc cpu alarm info");
	OspRegCommand("showmpcmemoryalarm", (void*)showmpcmemoryalarm,  "show mpc memory alarm info");
	OspRegCommand("showpowertempalarm", (void*)showpowertempalarm,  "show power temperature alarm info");
	OspRegCommand("showmpc2tempalarm",  (void*)showmpc2tempalarm,   "show mpc2 temperature alarm info");
	OspRegCommand("showe1linkalarm",    (void*)showe1linkalarm,     "show e1 link status alarm info");
	OspRegCommand("showsyncsrcalarm",   (void*)showsyncsrcalarm,    "show syn source alarm info");
	OspRegCommand("showfilesystemalarm",(void*)showfilesystemalarm, "show file system alarm info");
	OspRegCommand("showpoweralarm",     (void*)showpoweralarm,      "show power status alarm info");
	OspRegCommand("showpowerfanalarm",  (void*)showpowerfanalarm,   "show power fan status alarm info");
	OspRegCommand("showboardalarm",     (void*)showboardalarm,      "show board status alarm info");

	OspRegCommand("showbrdcpualarm",     (void*)showbrdcpualarm,      "show board cpu alarm info");
	OspRegCommand("showbrdtempalarm",    (void*)showbrdtempalarm,     "show board temperature alarm info");
	

	
	
	// [20101217 miaoqingsong add]     会议状态Trap上报测试命令
	OspRegCommand("showconfstatus",     (void*)showconfstatus,      "show conference status");
	
	// [20110621 miaoqingsong add]     调用底层驱动接口是否正确测试命令
	OspRegCommand("showdriverinterface",(void*)showdriverinterface, "show driver interface status info");

	OspRegCommand("show8kiledstatus",      (void*)show8kiledstatus,      "show 8ki led status");
	OspRegCommand("show8kicputmpandcpufan",(void*)show8kicputmpandcpufan,      "show 8ki cpu tmp and cpu fan");
#endif


	
    return;
}

BOOL32 TMcuUpdateInfo::FtpGet()
{
#ifdef _LINUX_
	if (m_byFtpGetEnd >= m_byFileNum)
	{
		return FALSE;
	}
	
	TUpdateFileInfo *ptUpdateInfo = &m_tUpdateFile[m_byFtpGetEnd];
	
	++m_byFtpGetEnd;
	
	s8 aszIpAddr[IP_ADDR_MAX_NUM] = {0};
	
	u32 dwIPAddr = ptUpdateInfo->GetSvrIp();
	u8 *p = (u8 *)&dwIPAddr;
	sprintf(aszIpAddr,"%d.%d.%d.%d%c",p[0],p[1],p[2],p[3],0);
	
	Agtlog(LOG_INFORM, "FtpGet: SvrIp:%s, Port:%d\n", aszIpAddr, ptUpdateInfo->GetSvrPort());
	
	if(FTPC_OK != FTPCReset()) 
	{
		OspPrintf(TRUE, FALSE, "FTPCReset 初始化失败\n");
		return FALSE;
    }
	
	if ( FTPC_OK !=FTPCSetServAddr(aszIpAddr, ptUpdateInfo->GetSvrPort())) 
	{ 
		OspPrintf(TRUE, FALSE, "设置服务器地址失败\n");
		return FALSE;
	}
	
	Agtlog(LOG_INFORM, "FtpGet: User:%s, Password:%s\n", ptUpdateInfo->GetSvrUser(), ptUpdateInfo->GetSvrPassword());
	
	if ( FTPC_OK !=FTPCSetLogId(ptUpdateInfo->GetSvrUser(),ptUpdateInfo->GetSvrPassword())) 
	{ 
		OspPrintf(TRUE, FALSE, "设置服务器账号密码失败\n");
		return FALSE;
	}
	
	s8 aszLocalFile[KDV_MAX_PATH] = {0};
	
	s8 aszFilePath[KDV_MAX_PATH] = {0};
	
	s8 aszFileName[KDV_MAX_PATH] = {0};
	
	memcpy(aszFilePath, ptUpdateInfo->GetSvrFilePath(), KDV_MAX_PATH);
	
	char *name_start = NULL;
	
	int sep = '/';
	
	name_start = strrchr(aszFilePath, sep);
	
	if (name_start == NULL)
	{
		OspPrintf(TRUE, FALSE, "FtpGet file name is null: %s", aszFilePath);
		return FALSE;
	}
	
	name_start += 1;
	
	memcpy(aszFileName, name_start, min(KDV_MAX_PATH, strlen(name_start)));
	
	*name_start = '\0';
	
	sprintf(aszLocalFile, "/ramdisk/%s", aszFileName);
	
	ptUpdateInfo->SetLocalFilePath(aszLocalFile);
	
	remove(aszLocalFile);
	
	Agtlog(LOG_INFORM, "FtpGet: SvrPath:%s, SvrFileName:%s, LocalPathFile:%s\n", aszFilePath, aszFileName, aszLocalFile);
	
	s32 dwError= FTPCGetFileinFlash(aszFilePath, aszFileName, aszLocalFile, ptUpdateInfo);
	
	if ( FTPC_OK != dwError) 
	{ 
		OspPrintf(TRUE, FALSE, "传送错误码是%d\n",dwError);
		return FALSE;
	}
#endif
	
	return TRUE;
}


#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
/*=============================================================================
函 数 名： SetRouteCfg
功    能： 路由配置
算法实现： 写路由配置信息，并设置系统路由(即时生效，不需重启)
全局变量： 
参    数： TMultiNetCfgInfo &tMultiNetCfgInfo
返 回 值： u16 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2010/04/19  4.6			薛亮                  创建
=============================================================================*/
u16 CCfgAgent::SetRouteCfg(TMultiNetCfgInfo &tMultiNetCfgInfo)
{
	u16 wRet = SUCCESS_AGENT;
	
	const u8 byNameLen = 10;
	s8	achProfileName[MAXLEN_MCU_FILEPATH] = {0};
	sprintf(achProfileName, "%s/%s", DIR_CONFIG, MULTINETCFG_INI_FILENAME);
	
	FILE* hOpen = NULL;
    if(NULL != (hOpen = fopen(achProfileName, "r"))) // exist
    {
		// fclose(hOpen); 
    }
    else
	{	
		AgentDirCreate(DIR_CONFIG);
		hOpen = fopen(achProfileName, "w"); // create configure file
	}
	
	if( NULL != hOpen)
	{
		fclose(hOpen);
		hOpen = NULL;
	}
	for(u8 byEthIdx = 0; byEthIdx < MAXNUM_ETH_INTERFACE; byEthIdx ++)
	{
		TMcuEthCfg8KE *ptMcuEthCfg8KE = tMultiNetCfgInfo.GetMcuEthCfg(byEthIdx);
		if( ptMcuEthCfg8KE == NULL )
		{
			continue;
		}
		
		s32 dwRealNumOfIp = 0;
		s8		achSecName[byNameLen] = {0};
		sprintf(achSecName, "eth%d", byEthIdx);
		
		// 某网口的ip段
		u8 byIpSecNum = ptMcuEthCfg8KE->GetIpSecNum();
		wRet |= WriteIntToFile( achProfileName, achSecName, "Num", byIpSecNum);
		
		for( u8 byIpSecIdx = 0; byIpSecIdx < byIpSecNum; byIpSecIdx ++)
		{
			u32 dwHIp	= ptMcuEthCfg8KE->GetMcuIpAddr(byIpSecIdx);
			u32 dwHMask = ptMcuEthCfg8KE->GetMcuSubNetMask(byIpSecIdx);
			u32 dwHGWIp	= ptMcuEthCfg8KE->GetGWIpAddr(byIpSecIdx);
			
			if( dwHIp == 0 )
			{
				continue;
			}
			
			s8		achKeyIpAddr[byNameLen] = {0};
			sprintf(achKeyIpAddr, "Ip%d", byIpSecIdx);
			s8		achKeyMask[byNameLen] = {0};
			sprintf(achKeyMask, "Mask%d", byIpSecIdx);
			s8		achKeyGw[byNameLen] = {0};
			sprintf(achKeyGw, "Gw%d", byIpSecIdx);
			
			
			s8		achIpAddr[32] = {0};
			GetIpFromU32(achIpAddr, htonl(dwHIp) );
			wRet = WriteStringToFile( achProfileName, achSecName, achKeyIpAddr, achIpAddr);
			
			s8		achMask[32] = {0};
			GetIpFromU32(achMask, htonl(dwHMask) );
			wRet |= WriteStringToFile( achProfileName, achSecName, achKeyMask, achMask);
			
			s8		achGw[32] = {0};
			GetIpFromU32( achGw, htonl(dwHGWIp) );		
			wRet |= WriteStringToFile( achProfileName, achSecName, achKeyGw, achGw);
			
#ifdef _LINUX_	
			s8 achCmdLine[256] = {0};
			//printf("route del -net %s netmask %s dev eth%d\n", achIpAddr, achMask, byEthIdx);
			//sprintf(achCmdLine, "route del -net %s netmask %s dev eth%d", achIpAddr, achMask, byEthIdx);
			//system(achCmdLine);
			//memset(achCmdLine, 0, sizeof(achCmdLine));
			printf("route add -net %s netmask %s gw %s dev eth%d\n", achIpAddr, achMask, achGw, byEthIdx);
			sprintf(achCmdLine, "route add -net %s netmask %s gw %s dev eth%d", achIpAddr, achMask, achGw, byEthIdx);
			system(achCmdLine);
	
#endif
			
		}
	}
	
	return wRet;
}

/*=============================================================================
函 数 名： SetMultiManuNetCfg
功    能： 多运营商网络配置
算法实现： 系统IP配置，写路由信息到配置文件 (要生效需重启)
全局变量： 
参    数： const TMultiManuNetCfg &tMultiManuNetCfg
返 回 值： u16 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2011/04/01  4.6			薛亮                  创建
=============================================================================*/
u16 CCfgAgent::SetMultiManuNetCfg(const TMultiManuNetCfg &tMultiManuNetCfg, const TMultiEthManuNetAccess *tMultiEthManuNetAccess, const u8 byEthNum)
{
	u16 wRet = SUCCESS_AGENT;


// 	for( u8 byLoop = 0;byLoop < tMultiManuNetCfg.m_tMultiManuNetAccess.GetIpSecNum();++byLoop )
// 	{
// 		if( 0 == tMultiManuNetCfg.m_tMultiManuNetAccess.GetGWIp( byLoop ) )
// 		{
// 			return ERR_AGENT_SETGWIP;
// 		}
// 	}
	
	//1,配置系统IP
	TMcu8KECfg tMcu8KECfg;
	GetMcuEqpCfg(&tMcu8KECfg);

	tMcu8KECfg.Print();

	TNetParam tNetParam;	//MPC IP
	TNetParam tGkNetParam;	
	TNetParam atPxyNetParam[MCU_MAXNUM_MULTINET];
	TNetParam atPxyGwParam[MCU_MAXNUM_MULTINET];
	memset( atPxyNetParam, 0, sizeof(atPxyNetParam));
	memset( atPxyGwParam, 0, sizeof(atPxyGwParam));

	TNetParam *ptNetParam = NULL;
	TNetAdaptInfo *ptNetAdapt = tMcu8KECfg.m_tLinkNetAdap.GetValue(0);

#ifdef _8KH_
	TMPCInfo tMpcInfo;
	GetMPCInfo(&tMpcInfo);
	u8 byEthId = tMpcInfo.GetLocalPortChoice();
	Agtlog(LOG_LVL_KEYSTATUS,"[SetMultiManuNetCfg] ethid.%d info\n",byEthId );
	if( ptNetAdapt != NULL && ptNetAdapt->GetAdaptIdx() != byEthId )
	{
		ptNetAdapt = tMcu8KECfg.m_tLinkNetAdap.GetValue(1);
		if( ptNetAdapt != NULL && ptNetAdapt->GetAdaptIdx() != byEthId )
		{
			Agtlog(LOG_LVL_ERROR,"[SetMultiManuNetCfg] Fail to get ethid.%d info\n",byEthId );
		}
	}
#endif
	
	if( ptNetAdapt != NULL )
	{
		u32 dwMask = 0;
		u32 dwIp   = 0;
		u32 dwGw   = 0;
		ptNetParam =  ptNetAdapt->m_tLinkIpAddr.GetValue(0);
		if( ptNetParam != NULL)
		{
			tNetParam = *ptNetParam;

			dwMask	= ptNetParam->GetIpMask();
			dwIp	= tMultiManuNetCfg.GetGkIp();
			tGkNetParam.SetNetParam(dwIp, dwMask);
			
			if ( tMultiManuNetCfg.IsMultiManuNetEnable())
			{
				TMultiManuNetAccess tMultiManuNetAccess = tMultiManuNetCfg.m_tMultiManuNetAccess;

				for(u8 byLp = 0; byLp < tMultiManuNetAccess.GetIpSecNum(); byLp ++)
				{
					if ( tMultiManuNetAccess.GetIpAddr(byLp) == 0 )
					{
						continue;
					}
					dwIp = tMultiManuNetAccess.GetIpAddr(byLp);
					dwMask = tMultiManuNetAccess.GetNetMask(byLp);
					dwGw = tMultiManuNetAccess.GetGWIp(byLp);
					
					atPxyNetParam[byLp].SetNetParam(dwIp, dwMask);
					atPxyGwParam[byLp].SetNetParam(dwIp, dwMask);
				}

			}

		}
		
	}

#ifdef _8KI_ 
	#define ETH_MAX_NUM	1
#else
	#define ETH_MAX_NUM	2
#endif

	for(u8 bylop = 0; bylop < ETH_MAX_NUM; bylop ++)
	{
		TNetAdaptInfo *ptNetAdaptInfo = tMcu8KECfg.m_tLinkNetAdap.GetValue(bylop);
		
		if(ptNetAdaptInfo != NULL)
		{
#ifdef _8KH_
			if( Is8000HmMcu() && ptNetAdaptInfo->GetAdaptIdx() != byEthId )
			{
				continue;
			}
#endif

			ptNetAdaptInfo->m_tLinkIpAddr.Clear();
			if( tNetParam.IsValid())
			{
				ptNetAdaptInfo->m_tLinkIpAddr.Append(tNetParam);
			}
			
			if( tGkNetParam.IsValid())
			{
				ptNetAdaptInfo->m_tLinkIpAddr.Append(tGkNetParam);
			}

			for(u8 byloop = 0; byloop < MCU_MAXNUM_MULTINET; byloop ++)
			{	
				if( atPxyNetParam[byloop].IsValid())
				{
					ptNetAdaptInfo->m_tLinkIpAddr.Append(atPxyNetParam[byloop]);
					ptNetAdaptInfo->m_tLinkDefaultGateway.Append(atPxyGwParam[byloop]);
				}
			}			
		}
	}

#ifdef _8KI_
	//写网口1跟网口2信息
	TNetParam achtPxyNetParam[MCU_MAXNUM_ADAPTER];
	TNetParam achtPxyGwParam[MCU_MAXNUM_ADAPTER];

	for(bylop = 0; bylop < byEthNum; bylop ++)
	{
		memset( achtPxyNetParam, 0, sizeof(achtPxyNetParam));
		memset( achtPxyGwParam, 0, sizeof(achtPxyGwParam));

		TNetAdaptInfo *ptNetAdaptInfo = tMcu8KECfg.m_tLinkNetAdap.GetValue(bylop+1);
		
		if(ptNetAdaptInfo != NULL)
		{	
			ptNetAdaptInfo->m_tLinkIpAddr.Clear();
			
			for(u8 byloop = 0; byloop < tMultiEthManuNetAccess[bylop].GetIpSecNum(); byloop ++)
			{		
				if ( tMultiEthManuNetAccess[bylop].GetIpAddr(byloop) == 0 )
				{
					continue;
				}

				u32 dwIp = tMultiEthManuNetAccess[bylop].GetIpAddr(byloop);
				u32 dwMask = tMultiEthManuNetAccess[bylop].GetNetMask(byloop);
				u32 dwGw = tMultiEthManuNetAccess[bylop].GetGWIp(byloop);

				achtPxyNetParam[byloop].SetNetParam(dwIp, dwMask);
				achtPxyGwParam[byloop].SetNetParam(dwGw,dwMask);

				if( achtPxyNetParam[byloop].IsValid())
				{
					ptNetAdaptInfo->m_tLinkIpAddr.Append(achtPxyNetParam[byloop]);
					ptNetAdaptInfo->m_tLinkDefaultGateway.Append(achtPxyGwParam[byloop]);
				}
			}			
		}
	}
#endif

	wRet |= Save8KENetCfgToSys(&tMcu8KECfg);

	//2, write route to cfg file(主要是记录IP，mask和GW信息)
	//写多网段配置信息到代理配置文件[11/22/2012 chendaiwei]
	wRet |= WriteMultiManuNetAccess(tMultiManuNetCfg.m_tMultiManuNetAccess, tMultiEthManuNetAccess, byEthNum);


	if( 0!= atPxyNetParam[0].GetIpAddr() )
	{
		wRet |= WriteProxyIp( atPxyNetParam[0] );
	}
	
	
	return wRet;
}

u16	CCfgAgent::WriteProxyIp( TNetParam tNtParam )
{
	u16 wRet = SUCCESS_AGENT;

	s8    achProfileNamePxy[MAXLEN_MCU_FILEPATH] = {0};
	sprintf(achProfileNamePxy, "%s/%s", DIR_CONFIG, PXYCFG_INI_FILENAME);	
	

	s8		achIpAddr[32] = {0};
	memset( achIpAddr, '\0', sizeof(achIpAddr));

    GetIpFromU32(achIpAddr, htonl(tNtParam.GetIpAddr()) );
	wRet |= WriteStringToFile( achProfileNamePxy, "LocalInfo", "IpAddr", achIpAddr);

	return wRet;
}

u16 CCfgAgent::WriteMultiManuNetAccess(const TMultiManuNetAccess &tMultiManuNetAccess, const TMultiEthManuNetAccess *tMultiEthManuNetAccess, const u8 byEthNum)
{
	u16 wRet = SUCCESS_AGENT;

	const u8 byNameLen = 32;
	s8	achProfileName[MAXLEN_MCU_FILEPATH] = {0};
	sprintf(achProfileName, "%s/%s", DIR_CONFIG, MULTIMANUNETCFG_FILENAME);

	s8	achMcuCfgFileName[MAXLEN_MCU_FILEPATH] = {0};
	sprintf(achMcuCfgFileName, "%s/%s", DIR_CONFIG, MCUCFGFILENAME);
	
	FILE* hOpen = NULL;
    if(NULL != (hOpen = fopen(achProfileName, "r"))) // exist
    {
		// fclose(hOpen); 
    }
    else
	{	
		AgentDirCreate(DIR_CONFIG);
		hOpen = fopen(achProfileName, "w"); // create configure file
	}
	
	if( NULL != hOpen)
	{
		fclose(hOpen);
		hOpen = NULL;
	}
	
	s8		achSecName[byNameLen] = {0};
	sprintf(achSecName, "MultiManuNetRoute");
	u8 byNum = tMultiManuNetAccess.GetIpSecNum();
	wRet = WriteIntToFile( achProfileName, achSecName, "Num", byNum);
	wRet |= WriteIntToFile( achMcuCfgFileName, achSecName, "Num", byNum);

	//多运营商Eth数目-1
	wRet |= WriteIntToFile( achProfileName, achSecName, "EthNum", byEthNum);
	wRet |= WriteIntToFile( achMcuCfgFileName, achSecName, "EthNum", byEthNum);

	if ( wRet != SUCCESS_AGENT )
		return wRet;


	s8	achPxyCfgName[MAXLEN_MCU_FILEPATH] = {0};
	sprintf(achPxyCfgName, "%s/%s", DIR_CONFIG, PXYCFG_INI_FILENAME);
	
	if(-1==DelSection(achPxyCfgName,"DefaultGW"))
	{
		Agtlog(LOG_LVL_ERROR,"[WriteMultiManuNetAccess]delete %s  section[DefaultGW]  failed!\n",achPxyCfgName);
	}

	for( u8 byEthIdx = 0; byEthIdx < byEthNum + 1; byEthIdx ++)
	{

		u32 dwHIp = 0;
		u32 dwHMask = 0;
		u32 dwHGWIp	= 0;

		if(byEthIdx != 0)
		{
			sprintf(achSecName, "MultiManuNetRoute%d", byEthIdx);
			byNum = tMultiEthManuNetAccess[byEthIdx-1].GetIpSecNum();
			wRet = WriteIntToFile( achProfileName, achSecName, "Num", byNum);
			wRet |= WriteIntToFile( achMcuCfgFileName, achSecName, "Num", byNum);
		}

		for( u8 byIpSecIdx = 0; byIpSecIdx < byNum; byIpSecIdx ++)
		{
			if(byEthIdx == 0)
			{
				dwHIp = tMultiManuNetAccess.GetIpAddr(byIpSecIdx);
				dwHMask = tMultiManuNetAccess.GetNetMask(byIpSecIdx);
				dwHGWIp	= tMultiManuNetAccess.GetGWIp(byIpSecIdx);
			}
			else
			{
				dwHIp = tMultiEthManuNetAccess[byEthIdx-1].GetIpAddr(byIpSecIdx);
				dwHMask = tMultiEthManuNetAccess[byEthIdx-1].GetNetMask(byIpSecIdx);				
				dwHGWIp	= tMultiEthManuNetAccess[byEthIdx-1].GetGWIp(byIpSecIdx);
			}
			
			if (dwHIp == 0 || dwHGWIp == 0)
			{
				continue;
			}

			//------key----------
			//Route info
			s8		achKeyIpAddr[byNameLen] = {0};
			sprintf(achKeyIpAddr, "Ip%d", byIpSecIdx);
			s8		achKeyMask[byNameLen] = {0};
			sprintf(achKeyMask, "Mask%d", byIpSecIdx);
			s8		achKeyGw[byNameLen] = {0};
			sprintf(achKeyGw, "Gw%d", byIpSecIdx);

			//Alias info
			s8		achKeyAlias[byNameLen] = {0};
			sprintf(achKeyAlias, "Alias%d", byIpSecIdx);
			
			//-----value-----------
			s8		achIpAddr[32] = {0};
			GetIpFromU32(achIpAddr, htonl(dwHIp) );
			wRet = WriteStringToFile( achProfileName, achSecName, achKeyIpAddr, achIpAddr);
			wRet |= WriteStringToFile( achMcuCfgFileName, achSecName, achKeyIpAddr, achIpAddr);
		
			s8		achMask[32] = {0};
			GetIpFromU32(achMask, htonl(dwHMask) );
			wRet |= WriteStringToFile( achProfileName, achSecName, achKeyMask, achMask);
			wRet |= WriteStringToFile( achMcuCfgFileName, achSecName, achKeyMask, achMask);
			
			s8		achGw[32] = {0};
			GetIpFromU32( achGw, htonl(dwHGWIp) );		
			wRet |= WriteStringToFile( achProfileName, achSecName, achKeyGw, achGw);
			wRet |= WriteStringToFile( achMcuCfgFileName, achSecName, achKeyGw, achGw);

			TNetManuAlias tNetManuAlias;
			if(byEthIdx == 0)
			{
				tMultiManuNetAccess.GetAlias(byIpSecIdx, &tNetManuAlias);
			}
			else
			{
				tMultiEthManuNetAccess[byEthIdx-1].GetAlias(byIpSecIdx, &tNetManuAlias);
			}
			wRet |= WriteStringToFile( achProfileName, achSecName, achKeyAlias, tNetManuAlias.GetAlias());
			wRet |= WriteStringToFile( achMcuCfgFileName, achSecName, achKeyAlias, tNetManuAlias.GetAlias());


			//写多网段地址到代理配置文件中 [11/22/2012 chendaiwei]
			//[DefaultGW]
			//172.16.229.8 = 172.16.229.108 第一个字段表示本地ip 第二个字段表示下一条地址 也就是网关
			//168.168.168.9 = 168.168.168.108
			if(!SetRegKeyString(achPxyCfgName,"DefaultGW",achIpAddr,achGw))
			{
				wRet = ERR_AGENT_WRITEFILE;
				Agtlog(LOG_LVL_ERROR,"[WriteMultiManuNetAccess]set DefalultGw to.%s failed!\n",PXYCFG_INI_FILENAME);
				
				return wRet;
			}

			s8	achVirtulaIpAddr[MAXLEN_MCU_FILEPATH] = {0};
			sprintf(achVirtulaIpAddr, "VirtualIPAddr%d",byIpSecIdx);
			if(!SetRegKeyString(achPxyCfgName,"LocalInfo",achVirtulaIpAddr,achIpAddr))
			{
				wRet = ERR_AGENT_WRITEFILE;
				Agtlog(LOG_LVL_ERROR,"[WriteMultiManuNetAccess]set VirtualIPAddr to.%s failed!\n",PXYCFG_INI_FILENAME);
				
				return wRet;
			}
		}

	}

	if(!SetRegKeyInt(achPxyCfgName,"LocalInfo","VirtualIPNum",byNum))
	{
		wRet = ERR_AGENT_WRITEFILE;
		Agtlog(LOG_LVL_ERROR,"[WriteMultiManuNetAccess]set VirtualIPNum to.%s failed!\n",PXYCFG_INI_FILENAME);
		
		return wRet;
	}

	return wRet;
}

u16 CCfgAgent::GetMultiManuNetAccess(TMultiManuNetAccess &tMultiManuNetAccess,BOOL32 bFromMcuCfgFile, 
									 TMultiEthManuNetAccess *tMultiEthManuNetAccess, u8 *byEthNum)
{
	u16 wRet = SUCCESS_AGENT;
	
	const u8 byNameLen = 32;
	s8	achProfileName[MAXLEN_MCU_FILEPATH] = {0};
	//从mcucfg.ini中获取多运营商信息
	if(bFromMcuCfgFile)
	{
		sprintf(achProfileName, "%s/%s", DIR_CONFIG, MCUCFGFILENAME);
	}
	//从multimanucfg.ini中获取多运营商信息
	else
	{
		sprintf(achProfileName, "%s/%s", DIR_CONFIG, MULTIMANUNETCFG_FILENAME);
	}
	
	FILE* hOpen = NULL;
    if(NULL != (hOpen = fopen(achProfileName, "r"))) // exist
	{
		
		s8  achSecName[byNameLen] = {0};
		
		for( u8 byEthIdx = 0; byEthIdx < MAXNUM_ETH_INTERFACE; byEthIdx ++)
		{
			if(byEthIdx != 0)
			{
				sprintf(achSecName, "MultiManuNetRoute%d", byEthIdx);
			}
			else
			{
				sprintf(achSecName, "MultiManuNetRoute");
			}
						
			s32 nNum = 0;
			GetRegKeyInt( achProfileName, achSecName, "Num", 0, &nNum);

			s32 nEthNum = 0;
			GetRegKeyInt( achProfileName, achSecName, "EthNum", 0, &nEthNum);
			*byEthNum = nEthNum;

			BOOL32 bUtf8 = IsUtf8Encoding();// lang [pengguofeng 4/12/2013]
			char achAlias[MAXLEN_MULTINETALIAS];

			for( u8 byIpSecIdx = 0; byIpSecIdx < nNum; byIpSecIdx ++)
			{
				// Create Key
				s8		achKeyIpAddr[byNameLen] = {0};
				sprintf(achKeyIpAddr, "Ip%d", byIpSecIdx);
				
				s8		achKeyMask[byNameLen] = {0};
				sprintf(achKeyMask, "Mask%d", byIpSecIdx);
				
				s8		achKeyGw[byNameLen] = {0};
				sprintf(achKeyGw, "Gw%d", byIpSecIdx);
				
				s8		achKeyAlias[byNameLen] = {0};
				sprintf(achKeyAlias, "Alias%d", byIpSecIdx);
				
				// Get Key Value
				s8		achIpAddr[32] = {0};
				GetRegKeyString( achProfileName, achSecName, achKeyIpAddr, "0.0.0.0", achIpAddr, sizeof(achIpAddr) );
				
				s8		achMask[32] = {0};
				GetRegKeyString( achProfileName, achSecName, achKeyMask, "0.0.0.0", achMask, sizeof(achMask) );
				
				s8		achGw[32] = {0};
				GetRegKeyString( achProfileName, achSecName, achKeyGw, "0.0.0.0", achGw, sizeof(achGw) );
				
				
				u32	dwHIpAddr	= ntohl(INET_ADDR(achIpAddr));
				u32	dwMask		= ntohl(INET_ADDR(achMask));
				u32 dwGw		= ntohl(INET_ADDR(achGw));
				
				s8		achName[32] = {0};
				GetRegKeyString( achProfileName, achSecName, achKeyAlias, " ", achName, sizeof(achName) );
				TNetManuAlias tNetManuAlias;
				if ( IsUtf8Encoding())
				{
					printf("[GetMultiManuNetAccess]Alias:%d is UTF8\n", byIpSecIdx);
					tNetManuAlias.SetAlias(achName);
				}
				else
				{
					printf("[GetMultiManuNetAccess]Alias:%d is GBK\n", byIpSecIdx);
					memset(achAlias, 0 , sizeof(achAlias));
					gb2312_to_utf8(achName, achAlias, sizeof(achAlias)-1);
					tNetManuAlias.SetAlias(achAlias);
				}

				if(byEthIdx == 0)
				{
					tMultiManuNetAccess.AddIpSection(
						dwHIpAddr, dwMask, dwGw, 
						&tNetManuAlias
					);
				}
				else
				{
					tMultiEthManuNetAccess[byEthIdx-1].AddIpSection(
						dwHIpAddr, dwMask, dwGw, 
						&tNetManuAlias
					);
				}				
			}
		}
	}
	else
	{
		printf("%s does not exist!\n", achProfileName);
	}
	
	if( NULL != hOpen )
	{
		fclose( hOpen );
	}
	
	return wRet;
}

u16 CCfgAgent::GetMultiNetCfgInfo(TMultiNetCfgInfo &tMultiNetCfgInfo)
{
	u16 wRet = SUCCESS_AGENT;
	
	const u8 byNameLen = 10;
	s8	achProfileName[MAXLEN_MCU_FILEPATH] = {0};
	sprintf(achProfileName, "%s/%s", DIR_CONFIG, MULTINETCFG_INI_FILENAME);
	
	FILE* hOpen = NULL;
    if(NULL != (hOpen = fopen(achProfileName, "r"))) // exist
	{
		
		s8  achSecName[byNameLen] = {0};
		u8 byEthIdx = 0;
		for( ; byEthIdx < MAXNUM_ETH_INTERFACE; byEthIdx ++)
		{
			sprintf( achSecName, "eth%d", byEthIdx);
			
			s32 nNum = 0;
			GetRegKeyInt( achProfileName, achSecName, "Num", 0, &nNum);
			for( u8 byIpSecIdx = 0; byIpSecIdx < nNum; byIpSecIdx ++)
			{
				s8		achKeyIpAddr[byNameLen] = {0};
				sprintf(achKeyIpAddr, "Ip%d", byIpSecIdx);
				
				s8		achKeyMask[byNameLen] = {0};
				sprintf(achKeyMask, "Mask%d", byIpSecIdx);
				
				s8		achKeyGw[byNameLen] = {0};
				sprintf(achKeyGw, "Gw%d", byIpSecIdx);
				
				s8		achIpAddr[32] = {0};
				GetRegKeyString( achProfileName, achSecName, achKeyIpAddr, "0.0.0.0", achIpAddr, sizeof(achIpAddr) );
				
				s8		achMask[32] = {0};
				GetRegKeyString( achProfileName, achSecName, achKeyMask, "0.0.0.0", achMask, sizeof(achMask) );
				
				s8		achGw[32] = {0};
				GetRegKeyString( achProfileName, achSecName, achKeyGw, "0.0.0.0", achGw, sizeof(achGw) );
				
				u32	dwHIpAddr	= ntohl(INET_ADDR(achIpAddr));
				u32	dwMask		= ntohl(INET_ADDR(achMask));
				u32 dwGw		= ntohl(INET_ADDR(achGw));
				
				tMultiNetCfgInfo.m_atMcuEthCfg8KE[byEthIdx].AddIpSection(dwHIpAddr, dwMask, dwGw);
				
			}
			
		}
	}
	else
	{
		printf("%s does not exist!\n", achProfileName);
	}
	
	if( NULL != hOpen )
	{
		fclose( hOpen );
	}
	
	return wRet;
	
	
}

#ifdef _8KI_
/*=============================================================================
函 数 名： WritePxyMultiNetInfo
功    能： 设置代理多网段信息
算法实现： 
全局变量： 
参    数： void
返 回 值： 返回SUCCESS_AGENT表明写文件成功；否则失败
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/10/17	4.7.3		周晶晶                 创建
=============================================================================*/
u16	CCfgAgent::WritePxyMultiNetInfo( void )
{
	s8	achPxyCfgName[MAXLEN_MCU_FILEPATH] = {0};
	sprintf(achPxyCfgName, "%s/%s", DIR_CONFIG, PXYCFG_INI_FILENAME);

	FILE* hOpen = NULL;
    if(NULL != (hOpen = fopen(achPxyCfgName, "r"))) // exist
    {
		fclose(hOpen); 
    }
    else
	{	
		return ERR_AGENT_WRITEFILE;
	}
	

	if(-1==DelSection(achPxyCfgName,"DefaultGW"))
	{
		Agtlog(LOG_LVL_ERROR,"[WriteMultiManuNetAccess]delete %s  section[DefaultGW]  failed!\n",achPxyCfgName);
	}

	s8		achIpAddr[32] = {0};
	s8		achGw[32] = {0};
	s8		achVirtulaIpAddr[MAXLEN_MCU_FILEPATH] = {0};
		
	u8		byNum = 0;

	TNewNetCfg tNewNetCfg;
	GetNewNetCfgInfo( tNewNetCfg );
	TEthCfg tEthCfg;
	TNetCfg tNetCfg;
	u32 dwIp = 0,dwMask = 0,dwGwIp = 0;

	for( u8 byIdx = 0;byIdx < MAXNUM_ETH_INTERFACE;++byIdx )
	{
		tEthCfg.Clear();
		
		tNewNetCfg.GetEthCfg( byIdx,tEthCfg );
		for( u8 byIpIdx = 0;byIpIdx < MCU_MAXNUM_ADAPTER_IP;++byIpIdx )
		{
			tNetCfg.Clear();
			tEthCfg.GetNetCfg( byIpIdx,tNetCfg );	
			
			dwIp   = tNetCfg.GetIpAddr();
			dwMask = tNetCfg.GetIpMask();
			dwGwIp = tNetCfg.GetGwIp();

			if( 0 == byIdx && 0 == byIpIdx )
			{
				continue;
			}

			if( 0 == dwIp || 0 == dwMask || 0 == dwGwIp )
			{
				continue;
			}

			if( 0 == byIdx && 0 == byIpIdx )
			{
				DelSysDefaultGW(0);
				AddSysDefaultGW(dwGwIp, 0);
			}

			GetIpFromU32( achGw, htonl(dwGwIp) );
			GetIpFromU32( achIpAddr, htonl(dwIp) );

			if(!SetRegKeyString(achPxyCfgName,"DefaultGW",achIpAddr,achGw))
			{				
				Agtlog(LOG_LVL_ERROR,"[WritePxyMultiNetInfo]set DefalultGw to.%s failed!\n",PXYCFG_INI_FILENAME);
				
				return ERR_AGENT_WRITEFILE;
			}
			
			sprintf(achVirtulaIpAddr, "VirtualIPAddr%d",byNum);
			if(!SetRegKeyString(achPxyCfgName,"LocalInfo",achVirtulaIpAddr,achIpAddr))
			{				
				Agtlog(LOG_LVL_ERROR,"[WritePxyMultiNetInfo]set VirtualIPAddr to.%s failed!\n",PXYCFG_INI_FILENAME);
				
				return ERR_AGENT_WRITEFILE;
			}
			++byNum;
		}
	}

	if(!SetRegKeyInt(achPxyCfgName,"LocalInfo","VirtualIPNum",byNum))
	{		
		Agtlog(LOG_LVL_ERROR,"[WriteMultiManuNetAccess]set VirtualIPNum to.%s failed!\n",PXYCFG_INI_FILENAME);
		
		return ERR_AGENT_WRITEFILE;
	}

	return SUCCESS_AGENT;
}
/*=============================================================================
函 数 名： SetNewNetCfgInfo
功    能： 设置8KI网络配置信息，支持3网口；
算法实现： 写NET信息、网口MAC地址、ETH模式、SIP、DMZ、网络类型到配置文件 (要生效需重启)
全局变量： 
参    数： const TNewNetCfg &tNewNetCfg
返 回 值： 返回SUCCESS_AGENT表明写文件成功；否则失败
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/09/22	8KI			闫日亮                 创建
=============================================================================*/
u16 CCfgAgent::SetNewNetCfgInfo(const TNewNetCfg &tNewNetCfg, const s8 bySecEthIndx/* = -1*/) 
{
	u16 wRet = SUCCESS_AGENT;

	#define MAX_KEY_LEN 10
	#define MAX_IP_LEN 32

	s8	achDmzIpAddr[MAX_IP_LEN] = {0};			//DMZ地址
	s8	achNotesInfo[MAXLEN_NOTES] = {0};		//备注信息
	s8	achKeyIpAddr[MAX_KEY_LEN] = {0};		//IP字段
	s8	achKeyMask[MAX_KEY_LEN] = {0};			//MASK字段
	s8	achKeyGw[MAX_KEY_LEN] = {0};			//网关字段
	s8	achKeyNotes[MAX_KEY_LEN] = {0};			//备注字段
	s8	achIpAddr[MAX_IP_LEN] = {0};
	s8	achMask[MAX_IP_LEN] = {0};	
	s8	achGw[MAX_IP_LEN] = {0};
	s8  achReturn[MAX_VALUE_LEN + 1] = {0};				
	s8	achSecName[MAXLEN_MCU_FILEPATH] = {0};
	s8  achMacAddr[MAX_MACADDR_STR_LEN] = {0};
	sprintf(achSecName, "NewNetCfgInfo");

	s8 achMcuCfgFileName[MAXLEN_MCU_FILEPATH] = {0};
	sprintf(achMcuCfgFileName, "%s/%s", DIR_CONFIG, MCUCFGFILENAME);

	//写ETH模式
	u8 byUseModeType = tNewNetCfg.GetUseModeType();
	wRet |= WriteIntToFile( achMcuCfgFileName, achSecName, "EthModeType", (s32)byUseModeType);

	//写网络类型
	u8 byNetworkType = tNewNetCfg.GetNetworkType();
	wRet |= WriteIntToFile( achMcuCfgFileName, achSecName, "NetworkType", (s32)byNetworkType);

	//写是否启用SipIp
	u8 byUseSip = tNewNetCfg.IsUseSip();
	wRet |= WriteIntToFile( achMcuCfgFileName, achSecName, "UseSip", (s32)byUseSip);

	//写SipIp在哪个网口上
	u8 bySipInEthIdx = tNewNetCfg.GetSipInEthIdx();
	wRet |= WriteIntToFile( achMcuCfgFileName, achSecName, "SipInEthIdx", (s32)bySipInEthIdx);

	//写SipIp地址
	s8	achSipIpAddr[32] = {0};
	u32 dwSipIp = tNewNetCfg.GetSipIpAddr();
	GetIpFromU32(achSipIpAddr, htonl(dwSipIp));
	wRet |= WriteStringToFile( achMcuCfgFileName, achSecName, "SipIpAddr", achSipIpAddr);

	//写SipMask
	s8	achSipMaskAddr[32] = {0};
	u32 dwSipMask = tNewNetCfg.GetSipMaskAddr();
	GetIpFromU32(achSipMaskAddr, htonl(dwSipMask));
	wRet |= WriteStringToFile( achMcuCfgFileName, achSecName, "SipMaskAddr", achSipMaskAddr);

	//写SipGw
	s8	achSipGwAddr[32] = {0};
	u32 dwSipGw = tNewNetCfg.GetSipGwAddr();
	GetIpFromU32(achSipGwAddr, htonl(dwSipGw));
	wRet |= WriteStringToFile( achMcuCfgFileName, achSecName, "SipGwAddr", achSipGwAddr);

	TEthCfg tEthCfg;
	TNetCfg tNetCfg;

#ifdef _LINUX_	
	TEthInfo tEthInfo[MAXNUM_ETH_INTERFACE];
	u8 byEthNum = 0;
	GetEthInfo(tEthInfo, byEthNum);
	if(byUseModeType == MODETYPE_BAK)
	{
		//获取MAC1地址
		memset(achMacAddr, '\0', sizeof(achMacAddr));		
		memcpy(achMacAddr, tEthInfo[1].GetMacAddrStr(), sizeof(achMacAddr));
		//写网口1的MAC地址到mcucfg.ini文件
		wRet |= WriteStringToFile( achMcuCfgFileName, "Eth1-Mac", "MacIpAddr", achMacAddr);

		//获取MAC0地址
		memset(achMacAddr, '\0', sizeof(achMacAddr));
		memcpy(achMacAddr, tEthInfo[0].GetMacAddrStr(), sizeof(achMacAddr));
	}
	else
	{
		//从文件读取mac1地址
		memset(achMacAddr, '\0', sizeof(achMacAddr));			
		wRet |= GetRegKeyString( achMcuCfgFileName, "Eth1-Mac", "MacIpAddr", 0, achMacAddr, sizeof(achMacAddr));			
	}

	//设置1网口MAC地址
	tEthInfo[1].SetMacAddrByStr(achMacAddr, strlen(achMacAddr));
#endif

	//写ETH、DMZ信息
	for(u8 byEthIdx = 0; byEthIdx < MAXNUM_ETH_INTERFACE; byEthIdx++)
	{
		if(bySecEthIndx != -1)
		{
			byEthIdx = bySecEthIndx;
		}

		tNewNetCfg.GetEthCfg(byEthIdx, tEthCfg);	//保存Eth网口信息		
		sprintf( achSecName, "8KI-eth%d", byEthIdx);			

#ifdef _LINUX_
		tEthInfo[byEthIdx].SetEthId(byEthIdx);

		//删除网口对应所有IP
		tEthInfo[byEthIdx].DelAllIpMask();
#endif

		if(byEthIdx == 1 && byUseModeType == MODETYPE_BAK)
		{
#ifdef _LINUX_
			tEthInfo[1] = tEthInfo[0];	//备份模式，网口1跟网口0一致
			tEthInfo[1].SetEthId(1);
#endif
			break;
		}

		//写DMZ地址信息到文件mcucfg.ini
		u32 dwDmzIp = tEthCfg.GetDMZIpAddr();
		memset(achDmzIpAddr, 0, sizeof(achDmzIpAddr));
		GetIpFromU32(achDmzIpAddr, htonl(dwDmzIp));
		wRet |= WriteStringToFile( achMcuCfgFileName, achSecName, "DmzIp", achDmzIpAddr);

		if(byEthIdx == 0)
		{
			//写DMZ地址信息到文件pxysrvCfg.ini
			s8    achProfileNamePxy[MAXLEN_MCU_FILEPATH] = {0};
			sprintf(achProfileNamePxy, "%s/%s", DIR_CONFIG, PXYCFG_INI_FILENAME);		
			wRet |= WriteIntToFile(achProfileNamePxy,SECTION_ProxyLocalInfo,KEY_mcuUseDMZ,(s32)tEthCfg.IsUseDMZ());		
			wRet |= WriteStringToFile(achProfileNamePxy,SECTION_ProxyLocalInfo,KEY_mcuIpDmzAddr,achDmzIpAddr);		
		}
		
		u8 byUseIpNum = 0;
		for( u8 byIpSecIdx = 0; byIpSecIdx < MCU_MAXNUM_ADAPTER_IP; byIpSecIdx ++)
		{	
			tEthCfg.GetNetCfg(byIpSecIdx, tNetCfg);

			u32 dwHIp = tNetCfg.GetIpAddr();
			u32 dwHMask = tNetCfg.GetIpMask();			
			u32 dwHGWIp	= tNetCfg.GetGwIp();
					
			//屏蔽掉IP地址为空的IP段
			if (dwHIp == 0)
			{
				continue;
			}

			byUseIpNum++;

			//获取备注信息
			memset(achNotesInfo, 0, sizeof(achNotesInfo));
			memcpy(achNotesInfo, tNetCfg.GetNotes(), sizeof(achNotesInfo));	
			
			//------key----------
			memset(achKeyIpAddr, 0, sizeof(achKeyIpAddr));
			memset(achKeyMask, 0, sizeof(achKeyMask));
			memset(achKeyGw, 0, sizeof(achKeyGw));
			memset(achKeyNotes, 0, sizeof(achKeyNotes));

			sprintf(achKeyIpAddr, "Ip%d", byIpSecIdx);
			sprintf(achKeyMask, "Mask%d", byIpSecIdx);
			sprintf(achKeyGw, "Gw%d", byIpSecIdx);
			sprintf(achKeyNotes, "Notes%d", byIpSecIdx);
							
			//-----value-----------
			memset(achIpAddr, 0, sizeof(achIpAddr));
			memset(achMask, 0, sizeof(achMask));
			memset(achGw, 0, sizeof(achGw));

			GetIpFromU32( achIpAddr, htonl(dwHIp) );
			wRet |= WriteStringToFile( achMcuCfgFileName, achSecName, achKeyIpAddr, achIpAddr);
							
			GetIpFromU32( achMask, htonl(dwHMask) );
			wRet |= WriteStringToFile( achMcuCfgFileName, achSecName, achKeyMask, achMask);
							
			GetIpFromU32( achGw, htonl(dwHGWIp) );		
			wRet |= WriteStringToFile( achMcuCfgFileName, achSecName, achKeyGw, achGw);
			
			//写备注信息
			wRet |= WriteStringToFile( achMcuCfgFileName, achSecName, achKeyNotes, achNotesInfo);

			if(byEthIdx == 0)
			{
				LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CFG, "[SetNewNetCfgInfo] receive eth0-%d Ip:%s,Mask:%s,Gw:%s!\n",
					byIpSecIdx, achIpAddr, achMask, achGw);
			}

#ifdef _LINUX_						
			//增加对第i个网口设置网关
			if( dwHGWIp != 0 )
			{
				if(byIpSecIdx == 0)
				{
					//第0网口的第0个网关设为系统默认网关
					if(byEthIdx == 0)
					{
						DelSysDefaultGW(0);
						AddSysDefaultGW(dwHGWIp, 0);
						tEthInfo[byEthIdx].SetGateway(dwHGWIp);
					}
					else
					{
						tEthInfo[byEthIdx].SetGateway(0);
					}
				}
				else
				{
					//添加静态路由信息
					AddSysRoute(byEthIdx, dwHIp, dwHMask, dwHGWIp);	
				}
			}	

			tEthInfo[byEthIdx].SetSubIpMaskByIdx(dwHIp, dwHMask, byIpSecIdx);
#endif
		}//for( u8 byIpSecIdx = 0; byIpSecIdx < MCU_MAXNUM_ADAPTER_IP; byIpSecIdx ++)

		//写每个网口配置的有效IP数目到文件mcucfg.ini
		wRet |= WriteIntToFile( achMcuCfgFileName, achSecName, "UseIpNum", (s32)byUseIpNum);

		if(bySecEthIndx != -1)
		{
			break;
		}

	}//for(u8 byEthIdx = 0; byEthIdx < MAXNUM_ETH_INTERFACE; byEthIdx++)

#ifdef _LINUX_
	if(byUseModeType == MODETYPE_BAK)
	{
		SetEthInfo(tEthInfo, MAXNUM_ETH_INTERFACE-1);
	}
	else
	{
		SetEthInfo(tEthInfo, MAXNUM_ETH_INTERFACE);
	}		
#endif

	memcpy( &m_tNewNetCfg, &tNewNetCfg, sizeof(m_tNewNetCfg) );

	WritePxyMultiNetInfo();

	return wRet;
}

/*=============================================================================
函 数 名： GetNewNetCfgInfo
功    能： 获取8KI网络配置信息，支持3网口
算法实现： 从文件读取NET信息、ETH模式、SIP、DMZ、网络类型
全局变量： 
参    数： TNewNetCfg &tNewNetCfg：读取的数据保存到该参数中
返 回 值： 返回SUCCESS_AGENT表明读文件成功；否则失败
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/09/22	8KI			闫日亮                 创建
=============================================================================*/
u16 CCfgAgent::GetNewNetCfgInfo(TNewNetCfg &tNewNetCfg)
{
 	memcpy( &tNewNetCfg, &m_tNewNetCfg, sizeof(tNewNetCfg));
	return SUCCESS_AGENT;	
}

/*=============================================================================
函 数 名： SwitchMcuEqpCfgToNewNetCfg
功    能： TMcu8KECfg转换为TNewNetCfg 
算法实现： 
全局变量： 
参    数： IN：const TMcu8KECfg &tMcuEqpCfg 
		   OUT：TNewNetCfg &tNewNetCfg
返 回 值： void 
=============================================================================*/
void CCfgAgent::SwitchMcuEqpCfgToNewNetCfg( TMcu8KECfg &tMcuEqpCfg, TNewNetCfg &tNewNetCfg)
{
	tNewNetCfg.SetUseModeType(tMcuEqpCfg.GetUseModeType());
	tNewNetCfg.SetNetworkType(tMcuEqpCfg.GetNetworkType());
	tNewNetCfg.SetIsUseSip(tMcuEqpCfg.IsUseSip());
	tNewNetCfg.SetSipInEthIdx(tMcuEqpCfg.GetSipInEthIdx());
	tNewNetCfg.SetSipIpAddr(tMcuEqpCfg.GetSipIpAddr());
	tNewNetCfg.SetSipMaskAddr(tMcuEqpCfg.GetSipMaskAddr());
	tNewNetCfg.SetSipGwAddr(tMcuEqpCfg.GetSipGwAddr());
	
	TEthCfg tEthCfg;
    TNetCfg tNetCfg;
	
	// 读取多网卡
	u32 dwAdptNum = tMcuEqpCfg.m_tLinkNetAdap.Length();
	
	for(u32 dwLoop = 0; dwLoop < dwAdptNum; dwLoop ++)
	{
		tNewNetCfg.GetEthCfg(dwLoop, tEthCfg);
		
		TNetAdaptInfo * ptNetAdapt = tMcuEqpCfg.m_tLinkNetAdap.GetValue(dwLoop);		
		if( ptNetAdapt == NULL)
		{
			continue;
		}
		
		for(u8 byIpSecIdx = 0; byIpSecIdx < MCU_MAXNUM_ADAPTER_IP; byIpSecIdx ++)
		{
			tEthCfg.GetNetCfg(byIpSecIdx, tNetCfg);
			
			TNetParam *ptNetParam = ptNetAdapt->m_tLinkIpAddr.GetValue(byIpSecIdx);			//host ip, mask
			TNetParam *ptGWParam  = ptNetAdapt->m_tLinkDefaultGateway.GetValue(byIpSecIdx);	//网关ip
			
			if( ptNetParam == NULL)
			{
				continue;
			}
			
			if( !(ptNetParam->IsValid()) )
			{
				continue;		
			}
			
			if(ptGWParam != NULL)
			{
				tNetCfg.SetNetRouteParam(ptNetParam -> GetIpAddr(), ptNetParam -> GetIpMask(), ptGWParam -> GetIpAddr());
			}
			else
			{
				tNetCfg.SetNetRouteParam(ptNetParam -> GetIpAddr(), ptNetParam -> GetIpMask(), 0);
			}
			
			tEthCfg.SetNetCfg(byIpSecIdx, tNetCfg);	//信息保存
		}
		
		tNewNetCfg.SetEthCfg(dwLoop, tEthCfg);	//网口信息保存
	}
}
#endif
/*=============================================================================
函 数 名： Make8KINetCfgEffect
功    能： 实际写入内存中保存的8KI网络配置信息，使新网络配置生效
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2010/01/21  8000E	    薛亮                  创建
=============================================================================*/
/*void CCfgAgent::Make8KINetCfgEffect( void )
{
	TMcu8KECfg tMcuEqpCfg = m_tMcuEqpCfg;
	u16 wRet = SUCCESS_AGENT;
	
	// 读取多网卡
	u32 dwAdaptNum = tMcuEqpCfg.m_tLinkNetAdap.Length();
	u32 dwLop = 0;
	for(; dwLop < dwAdaptNum; dwLop ++)
	{
		// 设置多ip
		TNetAdaptInfo * ptNetAdapt = tMcuEqpCfg.m_tLinkNetAdap.GetValue(dwLop);		
		if( ptNetAdapt == NULL)
		{
			continue;
		}
		
		u32 dwEthIdx = ptNetAdapt->GetAdaptIdx();
		
		BrdDelEthParam((u8)dwEthIdx); //TODO 待确认是否需要删除
		
		for(u8 byIpSecIdx = 0; byIpSecIdx < MCU_MAXNUM_ADAPTER_IP; byIpSecIdx ++)
		{					
			TNetParam *ptNetParam = ptNetAdapt->m_tLinkIpAddr.GetValue(byIpSecIdx);			//host ip, mask
			TNetParam *ptGWParam  = ptNetAdapt->m_tLinkDefaultGateway.GetValue(byIpSecIdx);	//网关ip
			
			if( ptNetParam == NULL)
			{
				continue;
			}
			if( !(ptNetParam->IsValid()) )
			{
				continue;		
			}
			
			//TODO 针对8000I 需要驱动提供多IP支持  [5/7/2012 chendaiwei]
			//主IP[5/7/2012 chendaiwei]
			if( 0 == byIpSecIdx)
			{
				g_cCfgParse.SetLocalIp(ptNetParam->GetIpAddr(),(u8)dwEthIdx);
				g_cCfgParse.SetMpcMaskIp(ptNetParam->GetIpMask(),(u8)dwEthIdx);
			}
		}
		
		u32 dwDefGWIpAddr = ptNetAdapt->GetDefGWIpAddr();
		g_cCfgParse.SetMpcGateway(dwDefGWIpAddr);
	}
	
    return;
}*/

/*=============================================================================
函 数 名： Save8KENetCfgToSys
功    能： 刷新系统配置
算法实现： 
全局变量： 
参    数： TMcu8KECfg * ptMcuEqpCfg
返 回 值： u16 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2010/01/21  8000E	    薛亮                  创建
=============================================================================*/
u16 CCfgAgent::Save8KENetCfgToSys( TMcu8KECfg * ptMcuEqpCfg)
{
#ifdef 	_8KI_
	TNewNetCfg tNewNetCfg;
	SwitchMcuEqpCfgToNewNetCfg(*ptMcuEqpCfg, tNewNetCfg);

	//保存网口信息到文件
	g_cCfgParse.SetNewNetCfgInfo(tNewNetCfg);
	
	m_tMcuEqpCfg = *ptMcuEqpCfg;

	return SUCCESS_AGENT;
#endif

	TMcu8KECfg tMcuEqpCfg = *ptMcuEqpCfg;
	u16 wRet = SUCCESS_AGENT;


	TMPCInfo tMpcInfo;
	GetMPCInfo(&tMpcInfo);
	u8 byEthId = tMpcInfo.GetLocalPortChoice();

	// 读取多网卡
	u32 dwAdaptNum = tMcuEqpCfg.m_tLinkNetAdap.Length();
	u32 dwLop = 0;
	for(; dwLop < dwAdaptNum; dwLop ++)
	{
		
#ifdef WIN32
		if( dwLop == 1)
		{
			break;
		}
#endif
		
		// 设置多ip
		TNetAdaptInfo * ptNetAdapt = tMcuEqpCfg.m_tLinkNetAdap.GetValue(dwLop);		
		if( ptNetAdapt == NULL)
		{
			continue;
		}

		
		u32 dwEthIdx = ptNetAdapt->GetAdaptIdx();
#ifdef _8KH_
		if ( Is8000HmMcu() && dwEthIdx != dwEthIdx )
		{
			continue;
		}
#endif
		
		// [9/11/2010 xliang] 把以前的ip全删掉. 注： 网口id并非就是dwLop
#ifdef _LINUX_ 
		DelAdapterAllIp(dwEthIdx); // dangerous
#elif defined(WIN32)
		
								   /*		// dangerous
								   TNetAdaptInfo * ptSingleNetAdapt = m_tMcuEqpCfg.m_tLinkNetAdap.GetValue(dwLop);
								   if( ptSingleNetAdapt != NULL)
								   {
								   DelAdapterAllIp(ptSingleNetAdapt);
								   }
		*/		
#endif
		
		
		// 		s32 dwRealNumOfIp = 0;
		// 		s8		achSecName[byNameLen] = {0};
		// 		sprintf(achSecName, "eth%d", dwLop);
		
		for(u8 byIpSecIdx = 0; byIpSecIdx < MCU_MAXNUM_ADAPTER_IP; byIpSecIdx ++)
		{					
			TNetParam *ptNetParam = ptNetAdapt->m_tLinkIpAddr.GetValue(byIpSecIdx);			//host ip, mask
			TNetParam *ptGWParam  = ptNetAdapt->m_tLinkDefaultGateway.GetValue(byIpSecIdx);	//网关ip
			
			if( ptNetParam == NULL)
			{
				continue;
			}
			if( !(ptNetParam->IsValid()) )
			{
				continue;		
			}
			
#ifdef WIN32            
			AddAdapterIp( ptNetParam->GetIpAddr(), ptNetParam->GetIpMask(), (s8*)ptNetAdapt->GetAdapterName() );
#endif	//WIN32
			
#ifdef _LINUX_
			// 此处不需要id转换
			TNetParam tParam;
			tParam.SetNetParam( ptNetParam->GetIpAddr(), ptNetParam->GetIpMask() );
			AddAdapterIp( dwEthIdx, tParam );
#endif	//LINUX
			
			// 设置多网段路由：
			// 静态路由采取命令实现，不和系统的配置文件打交道，配置文件用自定义的。重启后根据自定义的配置文件进行命令操作
			// 静态路由：需ip, mask, GW 
			/*
			dwRealNumOfIp ++;
			
			  s8		achKeyIpAddr[byNameLen] = {0};
			  sprintf(achKeyIpAddr, "Ip%d", byIpSecIdx);
			  s8		achKeyMask[byNameLen] = {0};
			  sprintf(achKeyMask, "Mask%d", byIpSecIdx);
			  s8		achKeyGw[byNameLen] = {0};
			  sprintf(achKeyGw, "Gw%d", byIpSecIdx);
			  
				
				  s8		achIpAddr[32] = {0};
				  GetIpFromU32(achIpAddr, htonl(ptNetParam->GetIpAddr()) );
				  wRet = WriteStringToFile( achProfileName, achSecName, achKeyIpAddr, achIpAddr);
				  
					s8		achMask[32] = {0};
					GetIpFromU32(achMask, htonl(ptNetParam->GetIpMask()) );
					wRet |= WriteStringToFile( achProfileName, achSecName, achKeyMask, achMask);
					
					  if( ptGWParam == NULL )
					  {
					  continue;
					  }
					  if( !ptGWParam->IsValid())
					  {
					  continue;
					  }
					  
						s8		achGw[32] = {0};
						GetIpFromU32( achGw, htonl(ptGWParam->GetIpAddr()) );		
						wRet |= WriteStringToFile( achProfileName, achSecName, achKeyGw, achGw);
						#ifdef _LINUX_
						s8 achCmdLine[256] = {0};
						sprintf(achCmdLine, "route del -net %s netmask %s dev eth%d", achIpAddr, achMask, dwLop);
						system(achCmdLine);
						memset(achCmdLine, 0, sizeof(achCmdLine));
						sprintf(achCmdLine, "route add -net %s netmask %s gw %s dev eth%d", achIpAddr, achMask, achGw, dwLop);
						system(achCmdLine);
						#endif
			*/
		}
		
		//		wRet |= WriteIntToFile( achProfileName, achSecName, "Num", dwRealNumOfIp);
		
		//设置某网口的默认网关
		u32 dwDefGWIpAddr = ptNetAdapt->GetDefGWIpAddr();
	
	#ifdef WIN32
		SetDefaultGW( &dwDefGWIpAddr, 1, (s8*)ptNetAdapt->GetAdapterName() );
	#endif	//WIN32			
		
	#ifdef _LINUX_
		SetDefaultGW( dwEthIdx, dwDefGWIpAddr );
	#endif
		
		//设置为系统默认网关(参考第0块，或第1块网卡)
		if( dwLop == 0 )
		{
			DelSysDefaultGW(0);
			AddSysDefaultGW( dwDefGWIpAddr, 0);
		}
	}

	m_tMcuEqpCfg = *ptMcuEqpCfg;
	
    return wRet;
}

BOOL32  CCfgAgent::EthInit()
{
#ifdef _LINUX_
  /*#if defined (_8KI_)

	u8 byCurEthUse = 0;
	u8 byState0 = 0;
	u8 byState1 = 0;

	if( ERROR == BrdGetEthLinkStat(0,&byState0))
	{
		printf("[EthInit]BrdGetEthLinkStat 0 failed!\n");
	}
	
	if(ERROR == BrdGetEthLinkStat(1,&byState1))
	{
		printf("[EthInit]BrdGetEthLinkStat 1 failed!\n");
	}

	printf("[EthInit]Eth0 status is :%d\n", byState0);
	printf("[EthInit]Eth1 status is :%d\n", byState1);

	if( byState0 == 1)
	{
		//记录当前使用eth0
		byCurEthUse = 0;
	}
	else if (byState1 == 1)
	{
		//记录当前使用eth1
		byCurEthUse = 1;
	}
	else
	{
		//eth0 eth1均无连接，默认down掉eth1,启用eth0
		byCurEthUse = 0;
	}
	printf("now system is using eth%d!\n", byCurEthUse);
	
	TBrdEthParam tEthParam0;
	TBrdEthParam tEthParam1;
	memset( &tEthParam0, 0, sizeof(tEthParam0) );
	AgtGetBrdEthParam( 0, &tEthParam0 );
	BOOL32 bIsSetIpEth0 = ( 0 == tEthParam0.dwIpAdrs ) ? FALSE : TRUE;
	memset( &tEthParam1, 0, sizeof(tEthParam1) );
	AgtGetBrdEthParam( 1, &tEthParam1 );
	BOOL32 bIsSetIpEth1 = ( 0 == tEthParam1.dwIpAdrs ) ? FALSE : TRUE;
	STATUS nRet = OK;
	
	//1.down掉网口eth0 eth1
	for( u8 byEthIdx = 0; byEthIdx < 2; byEthIdx++ )
	{
		//TODO 网口名待确认down掉网口  [5/7/2012 chendaiwei]
		if(ERROR == BrdInterfaceShutdown("fastethnet",byEthIdx))
		{
			printf("[EthInit]Failed to shutdown eth%d!\n",byEthIdx);
			Agtlog(LOG_WARN,"[EthInit]Failed to shutdown eth%d!\n",byEthIdx);
		}
	}

	// 2.无IP配置，设置默认IP [5/7/2012 chendaiwei]
	if ( !(bIsSetIpEth0 && bIsSetIpEth1 ))
	{
		TBrdEthParam tEthParam;
		memset( &tEthParam, 0, sizeof(tEthParam) );
		tEthParam.dwIpAdrs = INET_ADDR(DEFAULT_MCU_IP);
		tEthParam.dwIpMask = INET_ADDR(DEFAULT_IPMASK);

		for( u8 byEthIdx = 0; byEthIdx < 2; byEthIdx++ )
		{
			nRet = BrdSetEthParam( byEthIdx, Brd_SET_IP_AND_MASK, &tEthParam );
			if ( ERROR == nRet )
			{
				printf("[EthInit]Set eth%dIp failed!\n",byEthIdx);
			}
		}
	}
	//3.eth1无IP配置,用eth0参数设置eth1
	else if( bIsSetIpEth0 && !bIsSetIpEth1)
	{
		nRet = BrdSetEthParam( 1, Brd_SET_IP_AND_MASK, &tEthParam0 );
		if ( ERROR == nRet )
		{
			printf("[EthInit]IP Eth1 unset.Set eth1 failed!\n");
		}
	}
	//4.eth0无IP配置,用eth1参数设置eth0
	else if( !bIsSetIpEth0 && bIsSetIpEth1 )
	{
		nRet = BrdSetEthParam( 0, Brd_SET_IP_AND_MASK, &tEthParam1 );
		if ( ERROR == nRet )
		{
			printf("[EthInit]IP Eth0 unset.Set eth0 failed!\n");
		}
	}
	//5.eth0 eth1参数不一致
	else if( 0 != memcmp(&tEthParam0,&tEthParam1,sizeof(TBrdEthParam)))
	{
		if(byCurEthUse == 0)
		{
			nRet = BrdSetEthParam( 1, Brd_SET_IP_AND_MASK, &tEthParam0 );
			if ( ERROR == nRet )
			{
				printf("[EthInit]Eth0 Eth1 unmatched,chose eth0!\n");
			}
		}
		else
		{
			nRet = BrdSetEthParam( 0, Brd_SET_IP_AND_MASK, &tEthParam1 );
			if ( ERROR == nRet )
			{
				printf("[EthInit]Eth0 Eth1 unmatched,chose eth1!\n");
			}
		}
	}

	nRet = BrdSaveNipConfig();
	if ( ERROR == nRet )
	{
		printf("[EthInit]Failed to BrdSaveNipConfig!\n");
	}

	//TODO 待确认网口名 6.up byCurUseEth
	if(ERROR == BrdInterfaceNoShutdown("fastethernet",byCurEthUse))
	{
		printf("[EthInit]Failed to up eth%d!\n",byCurEthUse);
	}
	
	TMPCInfo tMPCInfo;
    GetMPCInfo( &tMPCInfo );
    tMPCInfo.SetLocalPortChoice( byCurEthUse );
	SetMPCInfo(tMPCInfo);

	OspDelay(6000);
	OspPost(MAKEIID(AID_MCU_BRDMGR, CInstance::DAEMON), EV_ETH_STATDETECT_TIME);

  #else */
	u8 byCurEthUse = 0;
	
	BOOL32 bPreOk = FALSE;	
	s8 abyBuf[2] = {0};
	s8 achFileName[MAXLEN_MCU_FILEPATH] = {0};
	sprintf(achFileName, "%s", "/opt/ethchoice" );
	if (0 == access (achFileName, F_OK))
	{
		FILE *hFile  = fopen(achFileName, "rb");
		if ( NULL != hFile )
		{
			s32 nFileLen = 0;
			fseek(hFile, 0, SEEK_END);
			nFileLen = ftell(hFile);
			//printf ("fileLen is %d\n", nFileLen);
			if (nFileLen > 0 && (u32)nFileLen < sizeof(abyBuf))
			{
				fseek(hFile, 0, SEEK_SET);
				fread(abyBuf, nFileLen, 1, hFile);
				
				byCurEthUse = atoi(abyBuf);
				bPreOk = TRUE;
				printf("bPreOk is %d, CurEthuse: %d\n", bPreOk, byCurEthUse);
			}
			
			fclose(hFile);
			hFile = NULL;
			
		}
	}
	
	
	if ( !bPreOk )
	{
		s32 nStat0 = GetLinkStat("eth0");
		s32 nStat1 = GetLinkStat("eth1");
		printf("[EthInit]Eth0 status is :%d\n", nStat0);
		printf("[EthInit]Eth1 status is :%d\n", nStat1);
		
		
		if( nStat0 == 1)
		{
			
			//eth0 可用，则禁用eth1和eth2
			//s8 achCmdLine[256] = {0};
			//sprintf( achCmdLine, "ifdown eth1");  
			//system(achCmdLine);
			//memset( achCmdLine, 0, sizeof(achCmdLine));
			//sprintf( achCmdLine, "ifdown eth2");
			//system (achCmdLine);
			
			//记录当前使用eth0
			byCurEthUse = 0;
			
		}
		else 
		{
			//禁eth2
			//s8 achCmdLine[256] = {0};
			//sprintf( achCmdLine, "ifdown eth0");  //因为可能存在ip冲突问题，虽然nStat0为0，这里也要把eth0禁掉
			//system(achCmdLine);
			
			//memset( achCmdLine, 0, sizeof(achCmdLine));
			//sprintf( achCmdLine, "ifdown eth2");
			//system (achCmdLine);
			byCurEthUse = 1;
			
		}
	}

	printf("now system is using eth%d!\n", byCurEthUse);
	
	TMPCInfo tMPCInfo;
    GetMPCInfo( &tMPCInfo );
    tMPCInfo.SetLocalPortChoice( byCurEthUse );
	SetMPCInfo(tMPCInfo);
	
	//#endif
#endif
	
	return TRUE;
} 

u16 CCfgAgent::GetMcuEqpCfg( TMcu8KECfg * ptMcuEqpCfg )
{
    u16 wRet = SUCCESS_AGENT;
    
    ptMcuEqpCfg->Clear();
    *ptMcuEqpCfg = m_tMcuEqpCfg;
	
    return wRet;
}

/*=============================================================================
函 数 名： SetMcu8KIEqpCfg
功    能： 设置8KI网络配置
算法实现： 
全局变量： 
参    数： TMcu8KECfg * ptMcuEqpCfg
返 回 值： u16 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2012/05/07	4.7		    chendaiwei            创建
=============================================================================*/
u16 CCfgAgent::SetMcu8KIEqpCfg( TMcu8KECfg * ptMcuEqpCfg )
{
	m_tMcuEqpCfg = *ptMcuEqpCfg;

	return SUCCESS_AGENT;
}

/*=============================================================================
函 数 名： SetMcuEqpCfg
功    能： 
算法实现： 
全局变量： 
参    数： TNetAdaptInfoAll * ptNetAdaptInfoAll
返 回 值： u16 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/4/1   4.0			周广程                  创建
=============================================================================*/
u16 CCfgAgent::SetMcuEqpCfg( TNetAdaptInfoAll * ptNetAdaptInfoAll )
{
	
	
    u16 wRet = SUCCESS_AGENT;
    
	m_tMcuEqpCfg.Clear();
	
	
    if ( NULL != ptNetAdaptInfoAll )
    {
		u32 dwHardIdx = 0;
        u32 dwIdx = 0;
        u32 dwNetAdapterNum = ptNetAdaptInfoAll->m_tLinkNetAdapt.Length();
        for ( dwIdx = 0; dwIdx < dwNetAdapterNum; dwIdx++ )
        {
            
            TNetAdaptInfo tMcuAdaptInfo = *(ptNetAdaptInfoAll->m_tLinkNetAdapt.GetValue(dwIdx));
#ifdef _LINUX_	
			if ( IsValidEthIfName( tMcuAdaptInfo.GetAdapterName()) )
			{
				dwHardIdx = GetEthIdxbyEthIfName( tMcuAdaptInfo.GetAdapterName());       
				//            dwHardIdx = g_cMcuAgent.EthIdxSys2Hard( dwHardIdx );		
			}
#endif
			tMcuAdaptInfo.SetAdaptIdx(dwHardIdx);
            m_tMcuEqpCfg.m_tLinkNetAdap.Append( tMcuAdaptInfo );
        }
		
		
#ifdef _LINUX_
		//eth0 与eth1 保持同步 
		TNetAdaptInfo *ptNetAdapt = m_tMcuEqpCfg.m_tLinkNetAdap.GetValue(0); //当前启用的eth，无论0或1均从结点0开始累计
		BOOL32 bNeedChg = TRUE;
#ifdef _8KH_
		// 如果是8000H-M则不需要 [pengguofeng 11/28/2012]
		bNeedChg = !Is8000HmMcu();
#endif
		if( bNeedChg && ptNetAdapt != NULL)
		{
			//网口信息
			// [4/19/2010 xliang] FIXME: ethid
			TNetAdaptInfo tNetAdapt = *ptNetAdapt;
			u32 dwBakIdx = (dwHardIdx == 0)? 1 : 0;
			tNetAdapt.SetAdaptIdx(dwBakIdx);
			m_tMcuEqpCfg.m_tLinkNetAdap.Append(tNetAdapt);
		}
#endif
		
    }
    else
    {
        wRet = ERR_AGENT_VALUEBESET;
    }
	
	printf("[SetMcuEqpCfg]print m_tMcuEqpCfg at end:------\n");
	m_tMcuEqpCfg.Print();
	/*
	
	  #if 0	// [3/29/2010 xliang] 
	  
		const u8 byNameLen = 10;
		s8	achProfileName[MAXLEN_MCU_FILEPATH] = {0};
		sprintf(achProfileName, "%s/%s", DIR_CONFIG, MULTINETCFG_INI_FILENAME);
		
		  FILE* hOpen = NULL;
		  if(NULL != (hOpen = fopen(achProfileName, "r"))) // exist
		  {
		  
			s8  achSecName[byNameLen] = {0};
			u8 byEthIdx = 0;
			for( ; byEthIdx < MAXNUM_ETH_INTERFACE; byEthIdx ++)
			{
			sprintf( achSecName, "eth%d", byEthIdx);
			TNetAdaptInfo *ptNetAdapt = m_tMcuEqpCfg.m_tLinkNetAdap.GetValue(byEthIdx);
			
			  if (ptNetAdapt == NULL)
			  {
			  continue;
			  }
			  // 		u32 dwDefGwIpAddr = ptNetAdapt->GetDefGWIpAddr();
			  ptNetAdapt->m_tLinkIpAddr.Clear();
			  ptNetAdapt->m_tLinkDefaultGateway.Clear();
			  s32 nNum = 0;
			  GetRegKeyInt( achProfileName, achSecName, "Num", 0, &nNum);
			  for( u8 byIpSecIdx = 0; byIpSecIdx < nNum; byIpSecIdx ++)
			  {
			  s8		achKeyIpAddr[byNameLen] = {0};
			  sprintf(achKeyIpAddr, "Ip%d", byIpSecIdx);
			  s8		achKeyMask[byNameLen] = {0};
			  sprintf(achKeyMask, "Mask%d", byIpSecIdx);
			  s8		achKeyGw[byNameLen] = {0};
			  sprintf(achKeyGw, "Gw%d", byIpSecIdx);
			  
				s8		achIpAddr[32] = {0};
				GetRegKeyString( achProfileName, achSecName, achKeyIpAddr, "0.0.0.0", achIpAddr, sizeof(achIpAddr) );
				
				  s8		achMask[32] = {0};
				  GetRegKeyString( achProfileName, achSecName, achKeyMask, "0.0.0.0", achMask, sizeof(achMask) );
				  
					s8		achGw[32] = {0};
					GetRegKeyString( achProfileName, achSecName, achKeyGw, "0.0.0.0", achGw, sizeof(achGw) );
					
					  u32	dwHIpAddr	= ntohl(INET_ADDR(achIpAddr));
					  u32	dwMask		= ntohl(INET_ADDR(achMask));
					  u32 dwGw		= ntohl(INET_ADDR(achGw));
					  
						TNetParam tNetParam;
						tNetParam.SetNetParam(dwHIpAddr, dwMask);
						ptNetAdapt->m_tLinkIpAddr.Append(tNetParam);
						
						  tNetParam.SetNetParam(dwGw, dwMask);
						  ptNetAdapt->m_tLinkDefaultGateway.Append(tNetParam);
						  
							//设静态路由
							#ifdef _LINUX_
							if(dwGw != 0)
							{
							s8 achCmdLine[256] = {0};
							sprintf(achCmdLine, "route del -net %s netmask %s dev eth%d", achIpAddr, achMask, byEthIdx);
							printf("route del -net %s netmask %s dev eth%d\n", achIpAddr, achMask, byEthIdx);
							system(achCmdLine);
							
							  memset(achCmdLine, 0, sizeof(achCmdLine));
							  printf("route add -net %s netmask %s gw %s dev eth%d\n", achIpAddr, achMask, achGw, byEthIdx);
							  sprintf(achCmdLine, "route add -net %s netmask %s gw %s dev eth%d", achIpAddr, achMask, achGw, byEthIdx);
							  system(achCmdLine);
							  }
							  #endif
							  }
							  
								}
								}
								#endif
								
	*/
	
	return wRet;
}

/*=============================================================================
函 数 名： SetRouteToSys
功    能： 对系统进行配置路由
算法实现： 
全局变量： 
参    数： 
返 回 值： u8:当前可用网口号 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2010/04/19	4.6			薛亮					create
=============================================================================*/
void CCfgAgent::SetRouteToSys( void )
{
	const u8 byNameLen = 32;
	s8	achProfileName[MAXLEN_MCU_FILEPATH] = {0};
	sprintf(achProfileName, "%s/%s", DIR_CONFIG, MULTINETCFG_INI_FILENAME);
	
	FILE* hOpen = NULL;
    if(NULL != (hOpen = fopen(achProfileName, "r"))) // exist
	{
		
		s8  achSecName[byNameLen] = {0};
		u8 byEthIdx = 0;
		for( ; byEthIdx < MAXNUM_ETH_INTERFACE; byEthIdx ++)
		{
			sprintf( achSecName, "eth%d", byEthIdx);
			
			s32 nNum = 0;
			GetRegKeyInt( achProfileName, achSecName, "Num", 0, &nNum);
			for( u8 byIpSecIdx = 0; byIpSecIdx < nNum; byIpSecIdx ++)
			{
				s8		achKeyIpAddr[byNameLen] = {0};
				sprintf(achKeyIpAddr, "Ip%d", byIpSecIdx);
				
				s8		achKeyMask[byNameLen] = {0};
				sprintf(achKeyMask, "Mask%d", byIpSecIdx);
				
				s8		achKeyGw[byNameLen] = {0};
				sprintf(achKeyGw, "Gw%d", byIpSecIdx);
				
				s8		achIpAddr[32] = {0};
				GetRegKeyString( achProfileName, achSecName, achKeyIpAddr, "0.0.0.0", achIpAddr, sizeof(achIpAddr) );
				
				s8		achMask[32] = {0};
				GetRegKeyString( achProfileName, achSecName, achKeyMask, "0.0.0.0", achMask, sizeof(achMask) );
				
				s8		achGw[32] = {0};
				GetRegKeyString( achProfileName, achSecName, achKeyGw, "0.0.0.0", achGw, sizeof(achGw) );
				
				u32	dwHIpAddr	= ntohl(INET_ADDR(achIpAddr));
				u32	dwMask		= ntohl(INET_ADDR(achMask));
				u32 dwGw		= ntohl(INET_ADDR(achGw));
				
				//设静态路由
#ifdef _LINUX_
				if(dwHIpAddr != 0 && dwMask !=0 && dwGw != 0 )
				{
					s8 achCmdLine[256] = {0};
					//sprintf(achCmdLine, "route del -net %s netmask %s dev eth%d", achIpAddr, achMask, byEthIdx);
					//printf("route del -net %s netmask %s dev eth%d\n", achIpAddr, achMask, byEthIdx);
					//system(achCmdLine);
					
					//memset(achCmdLine, 0, sizeof(achCmdLine));
					printf("route add -net %s netmask %s gw %s dev eth%d\n", achIpAddr, achMask, achGw, byEthIdx);
					sprintf(achCmdLine, "route add -net %s netmask %s gw %s dev eth%d", achIpAddr, achMask, achGw, byEthIdx);
					system(achCmdLine);
				}
#endif
			}
			
		}
	}
	else
	{
		printf("%s does not exist! so no route be set at present!\n", achProfileName);
	}
	
	if( NULL != hOpen )
	{
		fclose( hOpen );
		hOpen = NULL;
	}
	
	// [4/1/2011 xliang] multiManuNetCfg
	// 	sprintf(achProfileName, "%s/%s", DIR_CONFIG, MULTIMANUNETCFG_FILENAME);
	// 	
	// 	FILE* hOpenManu = NULL;
	//     if(NULL != (hOpenManu = fopen(achProfileName, "r"))) // exist
	// 	{
	// 		
	// 		s8  achSecName[byNameLen] = {0};
	// 		sprintf( achSecName, "MultiManuNetRoute");
	// 		
	// 		s32 nNum = 0;
	// 		GetRegKeyInt( achProfileName, achSecName, "Num", 0, &nNum);
	// 		for( u8 byIpSecIdx = 0; byIpSecIdx < nNum; byIpSecIdx ++)
	// 		{
	// 			s8		achKeyIpAddr[byNameLen] = {0};
	// 			sprintf(achKeyIpAddr, "Ip%d", byIpSecIdx);
	// 			
	// 			s8		achKeyMask[byNameLen] = {0};
	// 			sprintf(achKeyMask, "Mask%d", byIpSecIdx);
	// 			
	// 			s8		achKeyGw[byNameLen] = {0};
	// 			sprintf(achKeyGw, "Gw%d", byIpSecIdx);
	// 			
	// 			s8		achIpAddr[32] = {0};
	// 			GetRegKeyString( achProfileName, achSecName, achKeyIpAddr, "0.0.0.0", achIpAddr, sizeof(achIpAddr) );
	// 			
	// 			s8		achMask[32] = {0};
	// 			GetRegKeyString( achProfileName, achSecName, achKeyMask, "0.0.0.0", achMask, sizeof(achMask) );
	// 			
	// 			s8		achGw[32] = {0};
	// 			GetRegKeyString( achProfileName, achSecName, achKeyGw, "0.0.0.0", achGw, sizeof(achGw) );
	// 			
	// 			u32	dwHIpAddr	= ntohl(INET_ADDR(achIpAddr));
	// 			u32	dwMask		= ntohl(INET_ADDR(achMask));
	// 			u32 dwGw		= ntohl(INET_ADDR(achGw));
	// 			
	// 
	// 			//设静态路由
	// #ifdef _LINUX_
	// 
	// 			TMPCInfo tMPCInfo;
	// 			GetMPCInfo( &tMPCInfo );
	// 			u8 byEthIdx = tMPCInfo.GetLocalPortChoice();
	// 			
	// 			if(dwHIpAddr != 0 && dwMask !=0 && dwGw != 0 )
	// 			{
	// 				//获取网段IP
	// 				u32 dwHNetIp = (dwHIpAddr & dwMask);
	// 				s8		achNetAddr[32] = {0};
	// 				GetIpFromU32(achNetAddr, htonl(dwHNetIp) );
	// 
	// 				s8 achCmdLine[256] = {0};
	// 				//sprintf(achCmdLine, "route del -net %s netmask %s dev eth%d", achNetAddr, achMask, byEthIdx);
	// 				//printf("route del -net %s netmask %s dev eth%d\n", achNetAddr, achMask, byEthIdx);
	// 				//system(achCmdLine);
	// 				
	// 				//memset(achCmdLine, 0, sizeof(achCmdLine));
	// 				printf("route add -net %s netmask %s gw %s dev eth%d\n", achNetAddr, achMask, achGw, byEthIdx);
	// 				sprintf(achCmdLine, "route add -net %s netmask %s gw %s dev eth%d", achNetAddr, achMask, achGw, byEthIdx);
	// 				system(achCmdLine);
	// 			}
	// #endif
	// 		}
	// 		
	// 	}
	// 	else
	// 	{
	// 		printf("%s does not exist! so no route be set at present!\n", achProfileName);
	// 	}
	// 
	// 	if( NULL != hOpenManu )
	// 	{
	// 		fclose( hOpenManu );
	// 		hOpenManu = NULL;
	// 	}
	
	
}
/*=============================================================================
函 数 名： Search8KEPortChoice
功    能： 获取8KE当前可用的网口号，当网口均不可用时获取第一个ip配置非空的网口号
算法实现： 
全局变量： 
参    数： 
返 回 值： u8:当前可用网口号 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/5/14   4.0			周广程                  创建
2010/04/13	4.6			薛亮					modify
=============================================================================*/
/*
u8 CCfgAgent::Search8KEPortChoice()
{
u8 byNetPort = 0;
#ifdef _LINUX_
s8	achEthName[5] = {0};
for(u8 byEthIdx = 0; byEthIdx < MAXNUM_ETH_INTERFACE; byEthIdx ++)
{
sprintf(achEthName, "eth%d", byEthIdx);
if ( GetEthEnStat(achEthName) == 1 )
{
byNetPort = byEthIdx;
break;
}
}
#endif
return byNetPort;

  }
  */
  /*=============================================================================
  函 数 名： SetGkProxyCfgInfo
  功    能： 将GK Proxy配置信息写入文件
  算法实现： 
  全局变量： 
  参    数： [IN]TGKProxyCfgInfo &tGKProxyCfgInfo 
  返 回 值： u16 
  =============================================================================*/
  u16 CCfgAgent::SetGkProxyCfgInfo( TGKProxyCfgInfo &tGKProxyCfgInfo )
  {
	  memcpy( &m_tGkProxyCfgInfo, &tGKProxyCfgInfo, sizeof(tGKProxyCfgInfo) );
	  
	  u16 wRet = SUCCESS_AGENT;
	  
	  wRet = WriteIntToFile(SECTION_mcuGkProxyCfg, KEY_mcuGkUsed, (s32)m_tGkProxyCfgInfo.IsGkUsed());
	  
	  wRet |= WriteIntToFile(SECTION_mcuGkProxyCfg, KEY_mcuProxyUsed, (s32)m_tGkProxyCfgInfo.IsProxyUsed());
	  
	  s8   achIpAddr[32];
	  memset( achIpAddr, '\0', sizeof(achIpAddr));
	  GetIpFromU32(achIpAddr, htonl(m_tGkProxyCfgInfo.GetGkIpAddr()) );
	  wRet |= WriteStringToFile(SECTION_mcuGkProxyCfg, KEY_mcuGkIpAddr, achIpAddr);
	  
	  //	memset( achIpAddr, '\0', sizeof(achIpAddr));
	  //	GetIpFromU32(achIpAddr, htonl(m_tGkProxyCfgInfo.GetProxyIpAddr()) );
	  //  wRet |= WriteStringToFile(SECTION_mcuGkProxyCfg, KEY_mcuProxyIpAddr, achIpAddr);
	  
	  //	wRet |= WriteIntToFile(SECTION_mcuGkProxyCfg, KEY_mcuProxyPort, (s32)m_tGkProxyCfgInfo.GetProxyPort());
	  
	  // [4/1/2011 xliang] 添加IP的动作在之后多运营商接入那块进行
	  /*
	  //添加ip
	  TMcu8KECfg tMcu8KECfg;
	  GetMcuEqpCfg(&tMcu8KECfg);
	  
		TNetParam tGkNetParam;
		TNetParam tPxyNetParam;
		TNetParam tNetParam;
		TNetParam *ptNetParam = NULL;
		TNetAdaptInfo *ptNetAdapt = tMcu8KECfg.m_tLinkNetAdap.GetValue(0);
		
		  if( ptNetAdapt != NULL )
		  {
		  u32 dwMask = 0;
		  u32 dwIp   = 0;
		  ptNetParam =  ptNetAdapt->m_tLinkIpAddr.GetValue(0);
		  if( ptNetParam != NULL)
		  {
		  tNetParam = *ptNetParam;
		  dwMask	= ptNetParam->GetIpMask();
		  dwIp	= m_tGkProxyCfgInfo.GetGkIpAddr();
		  tGkNetParam.SetNetParam(dwIp, dwMask);
		  
			dwIp	= m_tGkProxyCfgInfo.GetProxyIpAddr();
			tPxyNetParam.SetNetParam(dwIp, dwMask);
			}
			
			  }
			  for(u8 bylop = 0; bylop < 2; bylop ++)
			  {
			  TNetAdaptInfo *ptNetAdaptInfo = tMcu8KECfg.m_tLinkNetAdap.GetValue(bylop);
			  if(ptNetAdaptInfo != NULL)
			  {
			  ptNetAdaptInfo->m_tLinkIpAddr.Clear();
			  if( tNetParam.IsValid())
			  {
			  ptNetAdaptInfo->m_tLinkIpAddr.Append(tNetParam);
			  }
			  
				if( tGkNetParam.IsValid())
				{
				ptNetAdaptInfo->m_tLinkIpAddr.Append(tGkNetParam);
				}
				
				  if( tPxyNetParam.IsValid())
				  {
				  ptNetAdaptInfo->m_tLinkIpAddr.Append(tPxyNetParam);
				  }
				  }
				  }
				  
					Save8KENetCfgToSys(&tMcu8KECfg);
	  */
	  
	  // [3/27/2010 xliang] 刷新GK侧配置文件
	  s8    achProfileNameGk[MAXLEN_MCU_FILEPATH] = {0};
	  sprintf(achProfileNameGk, "%s/%s", DIR_CONFIG, GKCFG_INI_FILENAME);
	  
	  wRet |= WriteIntToFile( achProfileNameGk, "StarInfo", "StartGK", (s32)m_tGkProxyCfgInfo.IsGkUsed());
	  
	  s32	nIs323UserConstIP = 1;
	  wRet |= WriteIntToFile( achProfileNameGk, "LocalInfo", "Is323UserConstIP", nIs323UserConstIP);
	  
	  memset( achIpAddr, '\0', sizeof(achIpAddr));
	  GetIpFromU32(achIpAddr, htonl(m_tGkProxyCfgInfo.GetGkIpAddr()) );
	  wRet |= WriteStringToFile( achProfileNameGk, "LocalInfo", "IpAddr", achIpAddr);
	  
	  // [3/27/2010 xliang] 刷新Proxy侧配置文件
	  s8    achProfileNamePxy[MAXLEN_MCU_FILEPATH] = {0};
	  sprintf(achProfileNamePxy, "%s/%s", DIR_CONFIG, PXYCFG_INI_FILENAME);
	  
	  //1
	  s32 nStartMp = 0;
	  wRet |= WriteIntToFile( achProfileNamePxy, "StarInfo", "StartPxy", (s32)m_tGkProxyCfgInfo.IsProxyUsed());
	  wRet |= WriteIntToFile( achProfileNamePxy, "StarInfo", "StartMp", nStartMp);
	  
	  //2
	  u16 wRasPort = 1819;
	  u16 wH245Port = 1820;
	  u16 wOspPort = 2776;
	  u16 wRtpPort = 2776;
	  u16 wRtcpPort = 2777;
	  u16 wMEDIABASEPORT = 48000;
	  u16 wGkRasPort = 1719;
	  
	  //wRet |= WriteIntToFile( achProfileNamePxy, "LocalInfo", "Is323UserConstIP", nIs323UserConstIP);
	  memset( achIpAddr, '\0', sizeof(achIpAddr));
	  GetIpFromU32(achIpAddr, GetLocalIp());
	  wRet |= WriteStringToFile( achProfileNamePxy, "LocalInfo", "IpAddr", achIpAddr);

	  //memset( achIpAddr, '\0', sizeof(achIpAddr));
	  //GetIpFromU32(achIpAddr, htonl(m_tGkProxyCfgInfo.GetProxyIpAddr()) );
	  //wRet |= WriteStringToFile( achProfileNamePxy, "LocalInfo", "IpAddr", achIpAddr);
	  wRet |= WriteIntToFile(achProfileNamePxy, "LocalInfo", "RasPort", (s32)wRasPort);
	  wRet |= WriteIntToFile(achProfileNamePxy, "LocalInfo", "H245Port", (s32)wH245Port);
	  wRet |= WriteIntToFile(achProfileNamePxy,"LocalInfo", "OSPPORT", (s32)wOspPort);
	  wRet |= WriteIntToFile(achProfileNamePxy,"LocalInfo", "RTPPORT", (s32)wRtpPort);
	  wRet |= WriteIntToFile(achProfileNamePxy,"LocalInfo", "RTCPPORT", (s32)wRtcpPort);
	  wRet |= WriteIntToFile(achProfileNamePxy,"LocalInfo", "MEDIABASEPORT", (s32)wMEDIABASEPORT);
	  
	  //3 GKInfo
	  memset( achIpAddr, '\0', sizeof(achIpAddr));
	  GetIpFromU32( achIpAddr, htonl(m_tGkProxyCfgInfo.GetGkIpAddr()) );
	  wRet |= WriteStringToFile( achProfileNamePxy, "GKInfo", "IpAddr", achIpAddr);
	  wRet |= WriteIntToFile(achProfileNamePxy,"GKInfo", "RasPort", (s32)wGkRasPort);
	  
	  if ( SUCCESS_AGENT != wRet )
		  printf("[SetGkProxyCfgInfo] failed, wRet.%d \n", wRet );
	  
	  return wRet;
	  
}

//(主机序)写注册GKIP到文件iwCfg.ini
u16 CCfgAgent::SetSipRegGkInfo( u32 dwGKIp )
{
	u16 wRet = SUCCESS_AGENT;

	#ifdef WIN32
	#define DIR_CONFIG_NEW               ( LPCSTR )"./conf"	
	#else
	#define DIR_CONFIG_NEW               ( LPCSTR )"/usr/conf"	
	#endif

	s8 achFName[KDV_MAX_PATH] = {0};
	sprintf(achFName, "%s/%s", DIR_CONFIG_NEW, "iwCfg.ini");

	FILE* hOpen = NULL;
    if(NULL == (hOpen = fopen(achFName, "r")))	// no exist
	{	
		AgentDirCreate(DIR_CONFIG_NEW);
		hOpen = fopen(achFName, "w");			// create configure file
	}
	
	if(NULL != hOpen)
	{
		fclose(hOpen);
		hOpen = NULL;
	}

	s8   achIpAddr[32];
	memset( achIpAddr, '\0', sizeof(achIpAddr));
	GetIpFromU32(achIpAddr, htonl(dwGKIp));
	wRet |= WriteStringToFile(achFName, "GKInfo", "IpAddr", achIpAddr);
	
	if ( SUCCESS_AGENT != wRet )
	{
		printf("[SetSipRegGkInfo] failed, wRet.%d \n", wRet );
	}

	return wRet;
}

u16 CCfgAgent::SetProxyDMZInfo( TProxyDMZInfo &tProxyDMZInfo )
{
	memcpy( &m_tProxyDMZInfo, &tProxyDMZInfo, sizeof(tProxyDMZInfo) );

	u16 wRet = SUCCESS_AGENT;
	s8    achProfileNamePxy[MAXLEN_MCU_FILEPATH] = {0};
	sprintf(achProfileNamePxy, "%s/%s", DIR_CONFIG, PXYCFG_INI_FILENAME);

	wRet |= WriteIntToFile(achProfileNamePxy,SECTION_ProxyLocalInfo,KEY_mcuUseDMZ,(s32)tProxyDMZInfo.IsUseDMZ());

	s8   achIpAddr[32] = {0};
	GetIpFromU32(achIpAddr, htonl(tProxyDMZInfo.GetDMZIpAddr()) );
	wRet |= WriteStringToFile(achProfileNamePxy,SECTION_ProxyLocalInfo,KEY_mcuIpDmzAddr,achIpAddr);
	
	//写配置文件[5/14/2013 chendaiwei]
	s8    achMcuCfgName[MAXLEN_MCU_FILEPATH] = {0};
	sprintf(achMcuCfgName, "%s/%s", DIR_CONFIG, MCUCFGFILENAME);
	wRet |= WriteIntToFile(achMcuCfgName,SECTION_ProxyLocalInfo,KEY_mcuUseDMZ,(s32)tProxyDMZInfo.IsUseDMZ());
	wRet |= WriteStringToFile(achMcuCfgName,SECTION_ProxyLocalInfo,KEY_mcuIpDmzAddr,achIpAddr);

	if ( SUCCESS_AGENT != wRet )
	printf("[SetProxyDMZInfo] failed, wRet.%d \n", wRet );
	  
	return wRet;
}

u16 CCfgAgent::GetProxyDMZInfoFromMcuCfgFile( TProxyDMZInfo &tProxyDMZInfo )
{
	s8    achMcuCfgName[MAXLEN_MCU_FILEPATH] = {0};
	sprintf(achMcuCfgName, "%s/%s", DIR_CONFIG, MCUCFGFILENAME);
	s32   nValue = 0;
	s32   nDmzIndx = 0;

	BOOL32 wRet = GetRegKeyInt(achMcuCfgName,SECTION_ProxyLocalInfo,KEY_mcuUseDMZ,0,&nValue);
	tProxyDMZInfo.SetIsUseDMZ((u8)nValue);

	wRet |= GetRegKeyInt(achMcuCfgName,SECTION_ProxyLocalInfo,"DmzEthIndx",0,&nDmzIndx);
	tProxyDMZInfo.SetDMZEthIndx((u8)nDmzIndx);

	s8    achDefStr[] = "Cannot find the section or key";
    s8    achReturn[MAX_VALUE_LEN + 1];

    memset(achReturn, '\0', sizeof(achReturn));

    wRet |= GetRegKeyString( achMcuCfgName, SECTION_ProxyLocalInfo, KEY_mcuIpDmzAddr, 
		achDefStr, achReturn, MAX_VALUE_LEN + 1 );

	tProxyDMZInfo.SetDMZIpAddr(ntohl(INET_ADDR(achReturn)));

	if ( SUCCESS_AGENT != wRet )
		printf("[GetProxyDMZInfoFromMcuCfgFile] failed, wRet.%d \n", wRet );
	
	return wRet;
}

u16 CCfgAgent::GetProxyDMZInfo( TProxyDMZInfo &tProxyDMZInfo )
{
	u16 wRet = SUCCESS_AGENT;
	tProxyDMZInfo = m_tProxyDMZInfo;
	
    return wRet;

/*	s8    achProfileNamePxy[MAXLEN_MCU_FILEPATH] = {0};
	sprintf(achProfileNamePxy, "%s/%s", DIR_CONFIG, PXYCFG_INI_FILENAME);
	s32   nValue = 0;
	
	wRet |= GetRegKeyInt(achProfileNamePxy,SECTION_ProxyLocalInfo,KEY_mcuUseDMZ,0,&nValue);
	tProxyDMZInfo.SetIsUseDMZ((u8)nValue);
	
	s8    achDefStr[] = "Cannot find the section or key";
    s8    achReturn[MAX_VALUE_LEN + 1];

    memset(achReturn, '\0', sizeof(achReturn));

    wRet |= GetRegKeyString( achProfileNamePxy, SECTION_ProxyLocalInfo, KEY_mcuIpDmzAddr, 
		achDefStr, achReturn, MAX_VALUE_LEN + 1 );

	tProxyDMZInfo.SetDMZIpAddr(ntohl(INET_ADDR(achReturn)));

	if ( SUCCESS_AGENT != wRet )
		printf("[GetProxyDMZInfo] failed, wRet.%d \n", wRet );
	
	return wRet;
	*/
}

/*=============================================================================
函 数 名： GetGkProxyCfgInfo
功    能： 得到GK,代理配置信息
算法实现： 
全局变量： 
参    数： TGKProxyCfgInfo &tGKProxyCfgInfo  [o]
返 回 值： u16 
=============================================================================*/
u16 CCfgAgent::GetGkProxyCfgInfo( TGKProxyCfgInfo &tGKProxyCfgInfo )
{
    u16 wRet = SUCCESS_AGENT;  
	
	tGKProxyCfgInfo = m_tGkProxyCfgInfo;
	
    return wRet;
}

/*=============================================================================
函 数 名： SetPrsTimeSpanCfgInfo
功    能： 将Prs配置信息写入文件
算法实现： 
全局变量： 
参    数： [IN]TPrsTimeSpan &tPrsTimeSpan 
返 回 值： u16 
=============================================================================*/
u16 CCfgAgent::SetPrsTimeSpanCfgInfo( TPrsTimeSpan &tPrsTimeSpan )
{
	m_tPrsTimeSpan = tPrsTimeSpan;
	
	u16 wRet = SUCCESS_AGENT;
	wRet |= WriteIntToFile(SECTION_mcuPrsTimeSpan, KEY_mcuPrsSpan1, (s32)m_tPrsTimeSpan.m_wFirstTimeSpan);
	wRet |= WriteIntToFile(SECTION_mcuPrsTimeSpan, KEY_mcuPrsSpan2, (s32)m_tPrsTimeSpan.m_wSecondTimeSpan);
	wRet |= WriteIntToFile(SECTION_mcuPrsTimeSpan, KEY_mcuPrsSpan3, (s32)m_tPrsTimeSpan.m_wThirdTimeSpan);
	wRet |= WriteIntToFile(SECTION_mcuPrsTimeSpan, KEY_mcuPrsSpan4, (s32)m_tPrsTimeSpan.m_wRejectTimeSpan);
	
    if ( SUCCESS_AGENT != wRet )
        printf("[SetPrsTimeSpanCfgInfo] failed, wRet.%d \n", wRet );
	
    return wRet;
	
}

/*=============================================================================
函 数 名： GetPrsTimeSpanCfgInfo
功    能： 得到prs time span配置信息
算法实现： 
全局变量： 
参    数： TPrsTimeSpan &tPrsTimeSpan  [o]
返 回 值： u16 
=============================================================================*/
u16 CCfgAgent::GetPrsTimeSpanCfgInfo( TPrsTimeSpan &tPrsTimeSpan )
{
    u16 wRet = SUCCESS_AGENT;  
	
	tPrsTimeSpan = m_tPrsTimeSpan;
	
    return wRet;
}

#endif
//[2011/02/11 zhushz] mcs修改mcu ip
/*=============================================================================
函 数 名： GetNewMpcNetCfg
功    能： 得到mcs设置的新网络配置
算法实现： 
全局变量： 
参    数： TMcuNewNetCfg& tMcuNewNetInfo
返 回 值： void 
=============================================================================*/
void CCfgAgent::GetNewMpcNetCfg(TMcuNewNetCfg& tMcuNewNetInfo)
{
	tMcuNewNetInfo = m_tMcuNewNetCfg;
	return;
}
/*=============================================================================
函 数 名： SetNewMpcNetCfg
功    能： 设置mcs设置的新网络配置
算法实现： 
全局变量： 
参    数： const TMcuNewNetCfg& tMcuNewNetInfo
返 回 值： void 
=============================================================================*/
void CCfgAgent::SetNewMpcNetCfg(const TMcuNewNetCfg& tMcuNewNetInfo)
{
	m_tMcuNewNetCfg = tMcuNewNetInfo;
	return;
}
/*=============================================================================
函 数 名： SetIsNetCfgBeModifed
功    能： 设置是否mcs设置的新网络配置标志
算法实现： 
全局变量： 
参    数： BOOL32 bIsNetCfgModify
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::SetIsNetCfgBeModifed(BOOL32 bIsNetCfgModify)
{
	m_bIsNetCfgBeModifedByMcs = bIsNetCfgModify;
	return m_bIsNetCfgBeModifedByMcs;
}
/*=============================================================================
函 数 名： IsMpcNetCfgBeModifedByMcs
功    能： 网络配置是否被mcs修改
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CCfgAgent::IsMpcNetCfgBeModifedByMcs()
{
	return m_bIsNetCfgBeModifedByMcs;
}


/*=============================================================================
函 数 名： WriteFailedTimeForOpenMcuCfg
功    能： 记录fopen mcucfg.ini失败的时间
算法实现： 
全局变量： 
参    数：	
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2011/09/16   4.6		倪志俊                  创建
=============================================================================*/
void CCfgAgent::WriteFailedTimeForOpenMcuCfg(s32 nErrno)
{
	FILE *hFile = NULL;
	s8  achFileName[64] = {0};
	sprintf(achFileName, "%s/%s", DIR_CONFIG,"HduCfg.ini");		
	hFile = fopen(achFileName, "a+b");
	if (NULL == hFile)
	{
		OspPrintf( TRUE, FALSE, "[WriteFailedTimeForOpenMcuCfg]Waring:Open File(%s) Failed,So Return!\n",achFileName);
		return;
	}
	s8 achCfgInfo[255] = {0};
	time_t tiCurTime = ::time(NULL);
	s32 nLen = 0;
	nLen = sprintf(achCfgInfo, "error:%s,time:%s\n", strerror(nErrno),ctime(&tiCurTime));
	fwrite((u8*)achCfgInfo, nLen, 1, hFile);
	fclose(hFile);
}



/*=============================================================================
函 数 名： SetMpcUpateStatus
功    能： 设置mpc升级标志，如果mcs升级mcu或者网管升级mcu，则将升级标志设为1
算法实现： 
全局变量： 
参    数： s32 :0表示未升级，1表示已经升级
返 回 值：  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2012/02/08   4.7		周翼亮                  创建
=============================================================================*/
void CCfgAgent::SetMpcUpateStatus(const s32 dwUpateSt)
{
	if ( dwUpateSt != 0 && dwUpateSt != 1 )
	{
		OspPrintf(TRUE,FALSE,"[SetMpcUpateStatus]invalid mpcupdatestatus:%d\n",dwUpateSt);
		return;
	}
	#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	s8    achProfileName[MAXLEN_MCU_FILEPATH] = {0};
	s32  dwCurUpateSt = 0;

#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	sprintf(achProfileName, "%s/%s", DIR_DATA, RUNSTATUS_8KE_CHKFILENAME);	
	BOOL32 bRet = GetRegKeyInt( achProfileName, SECTION_RUNSTATUS, KEY_MCU8KE, 0, &dwCurUpateSt );
#endif
	
	
	if( !bRet )  
	{
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
		OspPrintf( TRUE, FALSE, "[SetRunStatus] profile \" %s \" doesn't exist! So create automatically!\n", RUNSTATUS_8KE_CHKFILENAME);		
		printf( "[SetRunStatus] profile\" %s \" doesn't exist! So create automatically!\n", RUNSTATUS_8KE_CHKFILENAME);
#endif

		// create new directory and file
#if defined(WIN32)
		CreateDirectory(DIR_DATA, NULL);
#elif defined(_VXWORKS_)
		mkdir(DIR_DATA);
#elif defined(_LINUX_)
		mkdir(DIR_DATA, 0777);
		chmod(DIR_DATA, 0777);
#endif
		char achFullName[MAXLEN_MCU_FILEPATH] = {0};		
		char achBuf[MAXLEN_MCU_FILEPATH] = {0};
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
		sprintf(achFullName, "%s/%s", DIR_DATA, RUNSTATUS_8KE_CHKFILENAME);		
		sprintf(achBuf, "[%s]\n%s = %d\n", SECTION_RUNSTATUS, KEY_MCU8KE,dwUpateSt);
#endif

		
		int dwInBufLen = strlen(achBuf) /* + 1*/ ;	// 补充'\0'没有必要，在linux上用shell脚本对该文件操作反而会有问题。

		FILE *pFile = fopen(achFullName, "w");
		if (NULL != pFile)
		{
			if (0 != dwInBufLen)
			{
				fwrite(achBuf, 1, dwInBufLen, pFile);
#ifdef WIN32    //不作多余的 fflush 操作
				fflush(pFile);
#endif
			}
			fclose(pFile);
			pFile = NULL;
		}
		else
		{
			printf("Create file brdcfgdebug.ini failed\n");			
		}
	}
	else
	{
		if(dwUpateSt != dwCurUpateSt)
		{
		
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
			bRet = SetRegKeyInt( achProfileName, SECTION_RUNSTATUS, KEY_MCU8KE, dwUpateSt );
#endif

		}
	}
#endif
}

/*=============================================================================
函 数 名： ShowEqpInfo
功    能： 显示外设信息（主要显示端口分配信息）
算法实现： 
全局变量： 
参    数： 
返 回 值：  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2012/02/16   4.7		陈代伟                  创建
=============================================================================*/
void CCfgAgent::ShowEqpInfo( void )
{
	ShowVmpInfo();
	ShowBasInfo();
	ShowMixerInfo();
	ShowRecInfo();

	return;
}

/*=============================================================================
函 数 名： AddRegedMcsIp
功    能： 添加在线MCS的IP信息
算法实现： 
全局变量： 
参    数： u32 dwMcsIpAddr 
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2012/4/24   4.7			liaokang                创建
=============================================================================*/
BOOL32 CCfgAgent::AddRegedMcsIp( u32 dwMcsIpAddr )
{
    u16 wLoop;
    // 找到一个空表项
    for(wLoop = 0; wLoop < MAXNUM_MCU_MC; wLoop++ )
    {
        if( !m_tRegedMcsIpInfo[wLoop].GetMcsIpAddr() )
        {
            break;
        }
    }

    if( wLoop >= MAXNUM_MCU_MC )
    {
        Agtlog( LOG_ERROR, "[AddRegedMcsIp] can't find a blank item !\n" );
        return FALSE;
    }
    
    m_tRegedMcsIpInfo[wLoop].SetMcsIpAddr( dwMcsIpAddr );
    
    Agtlog( LOG_VERBOSE, "[AddRegedMcsIp] Add Mcs Ip:%0x succeceed!\n", dwMcsIpAddr );

    return TRUE;
}

/*=============================================================================
函 数 名： DeleteRegedMcsIp
功    能： 删除在线MCS的IP信息
算法实现： 
全局变量： 
参    数： u32 dwMcsIpAddr 
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2012/4/24   4.7			liaokang                创建
=============================================================================*/
BOOL32 CCfgAgent::DeleteRegedMcsIp( u32 dwMcsIpAddr )
{
    u16 wLoop;
    // 找到对应表项
    for(wLoop = 0; wLoop < MAXNUM_MCU_MC; wLoop++ )
    {
        if( dwMcsIpAddr == m_tRegedMcsIpInfo[wLoop].GetMcsIpAddr() )
        {
            break;
        }
    }
    
    if( wLoop >= MAXNUM_MCU_MC )
    {
        Agtlog( LOG_ERROR, "[DeleteRegedMcsIp] can't find Mcs Ip item: %0x !\n", dwMcsIpAddr);
        return FALSE;
    }
    
    m_tRegedMcsIpInfo[wLoop].SetMcsIpAddr( 0 );
    
    Agtlog( LOG_VERBOSE, "[DeleteRegedMcsIp] Delete Mcs Ip:%0x succeceed !\n", dwMcsIpAddr );

    return TRUE;
}

/*=============================================================================
函 数 名： GetRecombinedRegedMcsIpInfo
功    能： 获取重组的在线MCS的IP信息
算法实现： 
全局变量： 
参    数： [out] s8 *pchRegedMcsIpInfo
           [out] u16 &wBufLen
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2012/4/24   4.7			liaokang                创建
=============================================================================*/
BOOL32 CCfgAgent::GetRecombinedRegedMcsIpInfo(s8 *pchRegedMcsIpInfo, u16 &wBufLen)
{
// 结构：前2个字节代表连接mcu的mcs个数（网络序），其他每4个字节代表一个IP（网络序）

    u16  wLoop;
    u16  wRegedMcsNum = 0;
    u32  dwTempIp = 0;
    s8 abyRegedMcsIpInfo[256] = {0};
    abyRegedMcsIpInfo[255] = '\0';

    s8 abyBuf[256] = {0};
    abyBuf[255] = '\0';
    s8 *pBuff = abyBuf;
    pBuff += sprintf(pBuff, "[GetRecombinedRegedMcsIpInfo] Reged Mcs Ip Info:\n");

    // 找到对应表项
    for(wLoop = 0; wLoop < MAXNUM_MCU_MC; wLoop++ )
    {
        dwTempIp = m_tRegedMcsIpInfo[wLoop].GetMcsIpAddr();
        if( dwTempIp )
        { 
            wRegedMcsNum += 1;
            pBuff += sprintf(pBuff, "Reged Mcs No : %d, Ip: 0x%x \n", wRegedMcsNum, dwTempIp);
            dwTempIp = htonl(dwTempIp);
            memcpy( abyRegedMcsIpInfo + 2 + (wRegedMcsNum-1) * 4, &dwTempIp, 4 ); 
        }
    }
    
    if ( !wRegedMcsNum )
    {
        Agtlog( LOG_ERROR, "[GetRecombinedRegedMcsIpInfo] no Reged Mcs Ip item!\n");
        return FALSE;
    }

    wBufLen = 2 + wRegedMcsNum * 4;
    u16 wRegedMcsNumTemp = htons(wRegedMcsNum);
    memcpy( abyRegedMcsIpInfo, &wRegedMcsNumTemp, sizeof(u16) ); 
    memcpy( pchRegedMcsIpInfo, abyRegedMcsIpInfo, wBufLen);
    pBuff += sprintf(pBuff, "total Reged Mcs num :%d\n", wRegedMcsNum );
    Agtlog(LOG_VERBOSE, abyBuf);

    return TRUE;
}

/*====================================================================
    函数名        GetBrdIdbyIpAddr
    功能		  通过IP获取单板索引
    算法实现    ：
    引用全局变量：
    输入参数说明：u32 IP 主机序
    返回值说明  ：u8 单板ID
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	12/02/13    4.7         陈代伟           创建
====================================================================*/
u8 CCfgAgent::GetBrdIdbyIpAddr(u32 dwIpAddr)
{
	for( u8 byIdx = 0; byIdx < m_dwBrdCfgEntryNum; byIdx++ )
	{
		if(m_atBrdCfgTable[byIdx].GetBrdIp() == dwIpAddr)
		{
			return m_atBrdCfgTable[byIdx].GetBrdId();
		}
	}

	Agtlog(LOG_LVL_KEYSTATUS,"[GetBrdIdbyIpAddr] Brd<IP:%D> no exists!\n",dwIpAddr);

	return 0;
}
// 多国语言 [pengguofeng 4/12/2013]
/*====================================================================
函数名        SetEncodingType
功能		  设置当前文件的编码方式
算法实现    ：
引用全局变量：
输入参数说明：　所有新发的Mcs默认将认为是UTF8编码
返回值说明  ：u8
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
13/04/23    4.7        pgf          创建
====================================================================*/
void CCfgAgent::SetEncodingType(u8 byEncoding)
{
	m_byEncoding = byEncoding;
}

/*====================================================================
函数名        GetEncodingType
功能		  获取当前文件的编码方式
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：u8
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
13/04/23    4.7        pgf          创建
====================================================================*/

u8 CCfgAgent::GetEncodingType()
{
	return m_byEncoding;
}

/*====================================================================
函数名        IsUtf8Encoding
功能		  判断当前文件是否Utf8编码
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：True/False
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
13/04/23    4.7        pgf          创建
====================================================================*/
BOOL32 CCfgAgent::IsUtf8Encoding()
{
#ifdef _UTF8
	return GetEncodingType() == emenCoding_Utf8;
#else
	return TRUE; //  [pengguofeng 4/26/2013]老版本返回TRUE,为的是读xxx.ini时不作转码操作
#endif
}

/*====================================================================
    函数名        RefreshBrdOsType
    功能		  更新某个外设板卡的OSTYPE值
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：u8 byBrdId, u8 byOsType
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	12/08/23    4.7         田志勇          创建
====================================================================*/
BOOL32 CCfgAgent::RefreshBrdOsType(u8 byBrdId, u8 byOsType)
{
	for( u8 byIdx = 0; byIdx < m_dwBrdCfgEntryNum; byIdx++ )
	{
		if(m_atBrdCfgTable[byIdx].GetBrdId() == byBrdId)
		{
			m_atBrdCfgTable[byIdx].SetOSType(byOsType);
			return TRUE;
		}
	}
	return FALSE;
}
#ifdef _8KH_
/*====================================================================
    函数名        SetMcuType800L
    功能		  标记本MCU为800L类型（方便和8000I区分）  
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	12/06/18    4.7         陈代伟           创建
====================================================================*/
void CCfgAgent::SetMcuType800L( void )
{
	m_byIsMcu800L = 1;
}

/*====================================================================
    函数名        SetMcuType8000Hm
    功能		  标记本MCU为8000H-M类型
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	12/06/18    4.7         陈代伟           创建
====================================================================*/
void CCfgAgent::SetMcuType8000Hm( void )
{
	m_byIsMcu8000H_M = 1;
}

/*====================================================================
    函数名        Is800LMcu
    功能		  判断是否是800L型号的MCU  
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：BOOL32 是返回TRUE 否则返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	12/06/18    4.7         陈代伟           创建
====================================================================*/
BOOL32 CCfgAgent::Is800LMcu( void )
{
	return (m_byIsMcu800L == 0)? FALSE:TRUE;
}

/*====================================================================
    函数名        Is8000HmMcu
    功能		  判断是否是8000H-M型号的MCU  
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：BOOL32 是返回TRUE 否则返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	12/06/18    4.7         陈代伟           创建
====================================================================*/
BOOL32 CCfgAgent::Is8000HmMcu( void )
{
	return (m_byIsMcu8000H_M == 0)? FALSE:TRUE;
}

#endif



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
2012/11/23  4.7          chendaiwei           创建
=============================================================================*/
s32	CCfgAgent::MoveToSectionStart( FILE *stream, const s8* lpszSectionName, BOOL32 bCreate )
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
2012/11/23   4.7		chendaiwei                 创建
=============================================================================*/
s32	CCfgAgent::MoveToSectionEnd( FILE *stream, const s8* lpszSectionName, BOOL32 bCreate )
{
	char    achRow[MAX_LINE_LEN + 1];
    u32		dwLen;
    s8*		lpszReturn;

	/* get section start offset */
	s32 nStartOffset = MoveToSectionStart( stream, lpszSectionName, bCreate );
	if ( -1 == nStartOffset )
	{
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
函 数 名： DelSection
功    能： 删除指定段名下的内容
算法实现： 
全局变量： 
参    数： FILE *stream, Profile名柄   
const s8* lpszSectionName, Profile中的段名
返 回 值： 返回偏移量Offset，如果失败返回-1
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2012/12/23   4.7	    chendaiwei                 创建
=============================================================================*/
s32	CCfgAgent::DelSection(s8 *fileName, const s8* lpszSectionName )
{
	FILE *stream = NULL;
    if(NULL == (stream = fopen(fileName, "rb+"))) 
    {
		return -1;
    }

	if ( stream == NULL || lpszSectionName == NULL )
	{
		return -1;
	}

	fseek( stream, 0, SEEK_SET );
	s32 nSecStartOffset = MoveToSectionStart( stream, lpszSectionName, FALSE );
	if ( -1 == nSecStartOffset )
	{
		return -1;
	}

	s32 nSetEndOffset = MoveToSectionEnd( stream, lpszSectionName, FALSE );
	if ( -1 == nSetEndOffset )
	{
		return -1;
	}

	u32 dwLen = nSetEndOffset - nSecStartOffset;
	if ( 0 == dwLen )
	{
		return ftell(stream);
	}
	
	fseek( stream, 0, SEEK_END );
	u32 dwCpyLen = ftell(stream) - nSetEndOffset;
	
	u32 dwBufBeforeSection = (u32)nSecStartOffset;
	s8 *lpszBufBeforeSection = (s8*)malloc( dwBufBeforeSection);
	s8 *lpszCpyBuf = (s8*)malloc( dwCpyLen);
	if ( lpszCpyBuf == NULL || lpszBufBeforeSection == NULL)
	{
		return -1;
	}
	memset( lpszCpyBuf, 0, dwCpyLen);
	memset( lpszBufBeforeSection,0,dwBufBeforeSection);
	fseek( stream, 0, SEEK_SET );
	dwBufBeforeSection = fread( lpszBufBeforeSection, sizeof( char ), dwBufBeforeSection, stream );
	fseek( stream, nSetEndOffset, SEEK_SET );
	dwCpyLen = fread( lpszCpyBuf, sizeof( char ), dwCpyLen, stream );
	
	fclose(stream);
	stream = NULL;
	
	if(NULL == (stream = fopen(fileName, "wb+"))) 
    {
		return -1;
    }
	
	if ( stream == NULL || lpszSectionName == NULL )
	{
		return -1;
	}
	
	fwrite(lpszBufBeforeSection,1,dwBufBeforeSection,stream);
	fwrite(lpszCpyBuf,1,dwCpyLen,stream);
	fclose(stream);
	stream = NULL;
	
	free(lpszCpyBuf);
	free(lpszBufBeforeSection);
	
	return 0;
}


/*lint -restore*/
// END OF FILE
