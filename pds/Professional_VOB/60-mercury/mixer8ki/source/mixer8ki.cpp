/*****************************************************************************
模块名      : 8KI混音器外壳
文件名      : mixer8ki.cpp
创建时间    : 2012年 04月 25日
实现功能    : 
作者        : 田志勇
版本        : 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2012/04/25  1.0         田志勇        创建
******************************************************************************/
#include "mixer8ki.h"

#ifdef _LINUX_
s32 StartCaptureException()
{
	return init_nipdebug( 0, NULL, DIR_EXCLOG );
}
void MixerQuitFunc( int nSignalNum )
{
    g_bQuit = TRUE;
}
#endif//_LINUX_

BOOL32 procproducttest(TApu2EqpCfg &tApu2cfg);

int main()
{
#ifdef _LINUX_
	s32 nRet = StartCaptureException();
	if (0 != nRet)
	{
		printf("[main]StartCaptureException Error(%d), Ignored.\n", nRet);
	}
    else
    {
        printf("[main]StartCaptureException Succeeded!\n");
    }
#endif
	if ( ::IsOspInitd() )
    {
		printf( "[main]Osp Has Been Inited!\n" );
    }
	else
	{
		if (::OspInit( TRUE, BRD_TELNET_PORT ))
		{
			printf( "[main]OspInit Succeeded!\n" );
		}
		else
		{
			printf( "[main]OspInit Failed!\n" );
			return 0;
		}
	}
#ifdef _LINUX_
	if (OspRegQuitFunc( MixerQuitFunc )) 
	{
		printf( "[main]OspRegQuitFunc Succeeded!\n" );
	}
	else
	{
		printf( "[main]OspRegQuitFunc Failed!\n" );
	}
#endif
	//注册命令接口  待补充............
	u16 wLogRet = LogInit();//文件路径待补充...........
	if( LOG_ERR_NO_ERR == wLogRet )
	{
		printf( "[main]LogInit Succeeded\n");
	}
	else
	{
		printf( "[main]LogInit Failed(err:%d)!\n",wLogRet);
	}
	logsetport( 5676,1 );//端口确认...............
	logenablemod(MID_MCU_MIXER);
	logsetlvl(LOG_LVL_KEYSTATUS);
	TApu2EqpCfg   tApu2cfg;    
	procproducttest(tApu2cfg);
	if (!apu2Init(&tApu2cfg))
    {
        printf( "[main]apu2Init Failed!\n" );
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[main]apu2Init Failed!\n");
    }
	else
    {
        printf( "[main]apu2Init Successed!\n" );
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[main]apu2Init Successed!\n");
    }
#ifdef _LINUX_
	if (!NotfiyGuardUpdateSuccess("mixer_8ki"))
	{
		printf( "[main]NotfiyGuardUpdateSuccess Failed!\n" );
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[main]NotfiyGuardUpdateSuccess Failed!\n");
	}
	else
	{
		printf( "[main]NotfiyGuardUpdateSuccess Successed!\n" );
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[main]NotfiyGuardUpdateSuccess Successed!\n");
	}
#endif
	while( !g_bQuit )
    {
        OspDelay( 1000 );
    } 
}
/*====================================================================
函数名      : procproducttest
功能        ：
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：u32 dwNodeId
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/04/26  1.0         田志勇          创建
====================================================================*/
BOOL32 procproducttest(TApu2EqpCfg &tApu2cfg)
{
#ifdef _LINUX_
	if (BrdGetE2promTestFlag())
	{
		u32 dwBrdIpAddr = 0;
		if( ::BrdGetE2promIpAddr(&dwBrdIpAddr) == ERROR)//测试期间IP从E2PROM中读取
		{
			printf( "[procproducttest]BrdGetE2promIpAddr! so do'not do product test\n" );
			LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[IsProductTest]Read Board Ip from E2PROM error! so do'not do product test\n" );
			return FALSE;
		}
		printf( "[procproducttest]Board Ip(network sequence) while in ProductTest is: %x\n", dwBrdIpAddr);
		LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[procproducttest]Board Ip(network sequence) while in ProductTest is: %x\n", dwBrdIpAddr);
		//设置ip
		//判断设E0 or E1. (两口都连的话，以0口为准)
		u8 byId = 0;
		u8 byState = 0;
		u8 byEthId = ~0;
		STATUS wRet = ERROR;
		for(; byId < 2; byId ++)
		{
			wRet = ::BrdGetEthLinkStat(byId, &byState);
			if( OK == wRet )
			{
				printf("[procproducttest]BrdGetEthLinkStat(%u) successed!\n", byId);
				LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[procproducttest]BrdGetEthLinkStat(%u) successed!\n", byId);
			}
			else
			{
				printf("[procproducttest]BrdGetEthLinkStat(%u) Failed(%d)!\n", byId,wRet);
				LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[procproducttest]BrdGetEthLinkStat(%u) Failed(%d)!\n", byId,wRet);
				byEthId = 0;
				break;
			}
			if( byState == 1 && byEthId == (u8)~0)
			{
				byEthId = byId;
			}
			wRet = ::BrdDelEthParam(byId);//先全清掉，以防重ip设不了
			if( OK == wRet )
			{
				printf("[procproducttest]BrdDelEthParam(%u) successed!\n", byId);
				LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[procproducttest]BrdDelEthParam(%u) successed!\n", byId);
			}
			else
			{
				printf("[procproducttest]BrdDelEthParam(%u) Failed(%d)!\n", byId,wRet);
				LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[procproducttest]BrdDelEthParam(%u) Failed(%d)!\n", byId,wRet);
				byEthId = 0;
				break;
			}
		}
		
		TBrdEthParam tEtherPara;
		tEtherPara.dwIpAdrs = dwBrdIpAddr;
		tEtherPara.dwIpMask = inet_addr("255.0.0.0");
		memset( tEtherPara.byMacAdrs, 0, sizeof(tEtherPara.byMacAdrs) );
		wRet = ::BrdSetEthParam( byEthId, Brd_SET_IP_AND_MASK, &tEtherPara );
		if( OK == wRet  )
		{
			printf("[procproducttest]BrdSetEthParam Eth(%d)  IP(%d)(%x) successed!\n",byEthId,dwBrdIpAddr,dwBrdIpAddr);
			LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[procproducttest]BrdSetEthParam Eth(%d)  IP(%d)(%x) successed!\n",byEthId,dwBrdIpAddr,dwBrdIpAddr);
		}
		else
		{
			printf("[procproducttest]BrdSetEthParam Eth(%d)  IP(%d)(%x) Failed!\n",byEthId,dwBrdIpAddr,dwBrdIpAddr);
			LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[procproducttest]BrdSetEthParam Eth(%d)  IP(%d)(%x) Failed!\n",byEthId,dwBrdIpAddr,dwBrdIpAddr);
			return FALSE;
		}
		tApu2cfg.m_bIsProductTest = TRUE;
	}
#else
	BOOL32 bRet = FALSE;
	s32 sdwDefault = 0;
	s32 sdwReturnValue = 0;
	s8  achProfileName[64] = {0};
	s8  achDefStr[] = "10.1.1.1";
	s8  achReturn[MAX_VALUE_LEN + 1]  = {0};
	memset((void*)achProfileName, 0x0, sizeof(achProfileName));
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFGDEBUG_INI);
	if( ::GetRegKeyInt( achProfileName, SECTION_BoardDebug, KEY_IsAutoTest, sdwDefault, &sdwReturnValue ) 
		&& sdwReturnValue != 0 )  
	{
		tApu2cfg.m_bIsProductTest = (sdwReturnValue != 0);
	}
	if (tApu2cfg.m_bIsProductTest)
	{
		bRet = GetRegKeyString( achProfileName, SECTION_BoardDebug, KEY_ServerIpAddr, 
			achDefStr, achReturn, MAX_VALUE_LEN + 1 );
		if( bRet )  
		{
			tApu2cfg.m_acMixerCfg[0].dwConnectIP = ntohl(inet_addr(achReturn));
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[apu2start] Success profile while reading %s**%s**%s***[%s]!\n",
				achProfileName, SECTION_BoardDebug, KEY_ServerIpAddr, achReturn );
		}
		else
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[apu2start] Wrong profile while reading %s**%s**%s!\n", 
				achProfileName,SECTION_BoardDebug, KEY_ServerIpAddr );
			tApu2cfg.m_acMixerCfg[0].dwConnectIP = ntohl(inet_addr("10.1.1.1"));
		}
		tApu2cfg.m_acMixerCfg[0].wConnectPort = 60000;
		tApu2cfg.m_acMixerCfg[0].m_byMemberNum = MAXNUM_MIXER_CHNNL; 
	}
#endif
	return TRUE;
}
/*====================================================================
函数名      : BrdGetDstMcuNode
功能        ：
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：u32 dwNodeId
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/04/26  1.0         田志勇          创建
====================================================================*/
API u32 BrdGetDstMcuNode()
{
	u32 dwNode = ::OspConnectTcpNode(inet_addr("127.0.0.1"),MCU_LISTEN_PORT);
    return dwNode;
}
/*====================================================================
函数名      : BrdGetDstMcuNodeB
功能        ：
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：u32 dwNodeId
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/04/26  1.0         田志勇          创建
====================================================================*/
API u32 BrdGetDstMcuNodeB()
{
    return INVALID_NODE;
}
