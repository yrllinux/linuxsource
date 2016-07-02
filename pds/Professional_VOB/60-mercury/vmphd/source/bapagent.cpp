/*****************************************************************************
   模块名      : Board Agent
   文件名      : bapagent.cpp
   相关文件    : 
   文件实现功能: 单板代理
   作者        : guzh
   版本        : V4.5  Copyright(C) 2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
******************************************************************************/

#include "bapagent.h"

CBrdAgentApp	g_cBrdAgentApp;	//单板代理应用实例

BOOL32 CBoardConfig::ReadConfig()
{
    BOOL32 bRet = CBBoardConfig::ReadConnectMcuInfo();
    if (!bRet)
    {
        return bRet;
    }

    s8    achProfileName[64] = {0};
    s32   sdwValue;
    
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFG_INI);

    // EqpId
	bRet = GetRegKeyInt( achProfileName, SECTION_Eqp, KEY_Id, BASID_MIN, &sdwValue );
	if( !bRet )  
	{
		OspPrintf( TRUE, TRUE, "[BrdAgent] Wrong profile while reading %s%s!\n", SECTION_Eqp, KEY_Id );
		return FALSE;
	}
    m_byEqpId = ( u8 )sdwValue;
    return TRUE;
}


void AgentAPIEnableInLinux()
{
#ifdef _LUNUX_
    
#endif
}

API BOOL InitBrdAgent()
{
    BOOL bResult;

	static u16 wFlag;
	if( wFlag != 0 )
	{
		printf("[BrdAgent] Already Inited!\n" );
		return TRUE;
	}
    
#ifndef _VXWORKS_
    if( ERROR == BrdInit() )
    {
        printf("[BrdAgent] BrdInit fail.\n");
        return FALSE;
    }
#endif

    ::OspInit(TRUE, BRD_TELNET_PORT);
    
    //Osp telnet 初始授权 [11/28/2006-zbq]
#ifndef WIN32
    OspTelAuthor( MCU_TEL_USRNAME, MCU_TEL_PWD );
#endif

#undef OSPEVENT
#define OSPEVENT( x, y ) OspAddEventDesc( #x, y )
    
#ifdef _EV_AGTSVC_H_
#undef _EV_AGTSVC_H_
#include "evagtsvc.h"
#define _EV_AGTSVC_H_
#else
#include "evagtsvc.h"
#undef _EV_AGTSVC_H_
#endif    

	// 增加API
	AgentAPIEnableInLinux();

	wFlag++;


	//得到配置信息
    bResult = g_cBrdAgentApp.ReadConfig();
    if( bResult != TRUE )
    {
        printf("[BrdAgent] ReadConfig failed.\n");
        return FALSE;
    }

	//创建单板代理应用
	g_cBrdAgentApp.CreateApp( "BapAgent", AID_MCU_BRDAGENT, APPPRI_BRDAGENT );

	//单板代理开始工作
	CBBoardConfig *pBBoardConfig = (CBBoardConfig*)&g_cBrdAgentApp;
	OspPost( MAKEIID(AID_MCU_BRDAGENT, 1), OSP_POWERON, &pBBoardConfig, sizeof(pBBoardConfig) );

	OspSetLogLevel( AID_MCU_BRDAGENT, 0, 0);	 
	OspSetTrcFlag( AID_MCU_BRDAGENT, 0, 0);

	return TRUE;
}


API u32 BrdGetDstMcuNode()
{
    return g_cBrdAgentApp.m_dwDstMcuNodeA;
}

API u32 BrdGetDstMcuNodeB()
{
    return g_cBrdAgentApp.m_dwDstMcuNodeB;
}
