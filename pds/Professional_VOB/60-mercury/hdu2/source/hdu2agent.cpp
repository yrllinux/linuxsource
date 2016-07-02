/*****************************************************************************
   模块名      : Board Agent
   文件名      : hdu2agent.cpp
   相关文件    : 
   文件实现功能: 单板代理
   作者        : 田志勇
   版本        : V4.7  Copyright(C) 2011 KDV, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   11/10/27    4.7         田志勇        创建
******************************************************************************/

#include "hdu2agent.h"
CHdu2BrdAgentApp  g_cBrdAgentApp;
static SEMHANDLE        g_semHdu;
/*=====================================================================
函数  : InstanceEntry
功能  : HDU2板代理消息主入口
输入  : CMessage * const pcMsg
输出  : 无
返回  : void
注    :
-----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
=====================================================================*/
void CHdu2BoardAgent::InstanceEntry(CMessage * const pcMsg)
{
	if ( pcMsg == NULL )
	{
		hdu2agtLog(LOG_LVL_ERROR, MID_MCU_EQP,"[CHdu2BoardAgent::InstanceEntry]pcMsg == NULL,So Return!\n" );
		return;
	}
	hdu2agtLog(LOG_LVL_DETAIL, MID_MCU_EQP,"[CHdu2BoardAgent::InstanceEntry]CHdu2BoardAgent Rcv Message %u(%s) passed!\n", pcMsg->event, ::OspEventDesc(pcMsg->event) );
	switch (pcMsg->event)
	{
	    case MPC_BOARD_GET_CONFIG_ACK:       //取配置信息确认应答
		    ProcBoardGetConfigAck( pcMsg );
		    break;
		case MPC_BOARD_GET_CONFIG_NACK:      //取配置信息否定应答
		    ProcBoardGetConfigNAck( pcMsg );
		    break;
		default:
			CBBoardAgent::InstanceEntry(pcMsg);
			break;
	}
}
/*=====================================================================
函数  : ProcBoardGetConfigAck
功能  : HDU2板代理获取板配置成功处理
输入  : CMessage * const pcMsg
输出  : 无
返回  : void
注    :
-----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
=====================================================================*/
void  CHdu2BoardAgent::ProcBoardGetConfigAck( CMessage* const pcMsg )
{
	if( pcMsg == NULL )
    {
        hdu2agtLog(LOG_LVL_ERROR, MID_MCU_EQP,"[ProcBoardGetConfigAck]pcMsg == NULL,So Return!\n" );
        return;
    }
    
	TEqpHduEntry *ptHduEntry = (TEqpHduEntry*)(pcMsg->content + sizeof(u8));
	if ( 0 != g_cBrdAgentApp.GetEqpId() && g_cBrdAgentApp.GetEqpId() != ptHduEntry->GetEqpId() )
	{
		BrdHwReset();
	}
    g_cBrdAgentApp.SetEqpId( ptHduEntry->GetEqpId() );
	g_cBrdAgentApp.SetStartMode(ptHduEntry->GetStartMode());
	THduChnlModePortAgt tHduChnModePort;
	THduModePort tModePort;
	for (u8 byChnIdx = 0 ;byChnIdx < MAXNUM_HDU_CHANNEL;byChnIdx++)
	{
		ptHduEntry->GetHduChnlModePort(byChnIdx,tHduChnModePort);
		tModePort.SetOutModeType(tHduChnModePort.GetOutModeType());
		tModePort.SetOutPortType(tHduChnModePort.GetOutPortType());
		tModePort.SetZoomRate(tHduChnModePort.GetZoomRate());
		tModePort.SetScalingMode(tHduChnModePort.GetScalingMode());
		g_cBrdAgentApp.SetChnCfg(byChnIdx,tModePort);
	}

	if ( !OspSemGive(g_semHdu) )
	{
		hdu2agtLog(LOG_LVL_ERROR, MID_MCU_EQP,"[ProcBoardGetConfigAck] OspSemGive fail! in ProcBoardGetConfigAck()\n");
	}
	KillTimer( BRDAGENT_GET_CONFIG_TIMER );
	NEXTSTATE( STATE_NORMAL );

	/*u8 abyBuf[32];
	//TODO 软件版本号待统一定义，是否用宏定义[12/13/2011 chendaiwei]
	u32 dwSoftWareVersion = 1212;
	TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();
	memcpy(abyBuf, &tBrdPos, sizeof(TBrdPos) );
	memcpy(abyBuf+sizeof(TBrdPos),&dwSoftWareVersion,sizeof(u32));
	
	//通知MPC本单板的软件版本号信息 [12/13/2011 chendaiwei]
	PostMsgToManager( BOARD_MPC_SOFTWARE_VERSION_NOTIFY, abyBuf, 
		sizeof(u32)+sizeof(TBrdPos) );
		*/
}
/*=====================================================================
函数  : ProcBoardGetConfigNAck
功能  : HDU2板代理获取板配置成功处理
输入  : CMessage * const pcMsg
输出  : 无
返回  : void
注    :
-----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
=====================================================================*/
void  CHdu2BoardAgent::ProcBoardGetConfigNAck( CMessage* const pcMsg )
{
	hdu2agtLog(LOG_LVL_WARNING, MID_MCU_EQP, "[ProcBoardGetConfigNack]The function ProcBoardGetConfigNAck()\n",
		pcMsg != NULL ? pcMsg->srcnode : 0);
}

/*====================================================================
    函数名      ：ProcGetVersionReq
    功能        ：MPC发送给单板的获取版本信息消息处理
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    11/12/14    4.7         陈代伟        创建
====================================================================*/
/*void CHdu2BoardAgent::ProcGetVersionReq( CMessage* const pcMsg )
{
	s8 abyBuf[MAX_SOFT_VER_LEN+1+sizeof(TBrdPos)] = {0};
	switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:
		//取版本信息
		{
		memset(abyBuf, 0, sizeof(abyBuf) );

		TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();
		memcpy(abyBuf, &tBrdPos,sizeof(tBrdPos) );
		
		//TODO 软件版本号待统一定义，是否用宏定义[12/13/2011 chendaiwei]
		sprintf(abyBuf+sizeof(tBrdPos),"softwareversin:hdu2[4.7]");

		//发送给MANAGER响应
		PostMsgToManager( BOARD_MPC_GET_VERSION_ACK, (u8*)abyBuf, sizeof(abyBuf) );
		break;
		}
		
	default:
		LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}*/

/*=====================================================================
函数  : ProcBoardGetConfigNack
功能  : HDU2板代理获取板配置成功处理
输入  : CMessage * const pcMsg
输出  : 无
返回  : void
注    :
-----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
=====================================================================*/
BOOL32 CHdu2BoardConfig::ReadConfig()
{
	BOOL32 bIsTest = FALSE;
	s32 sdwReturnValue = 0;
	BOOL32 bRet = FALSE;
	s8    achProfileName[64] = {0};
	memset((void*)achProfileName, 0x0, sizeof(achProfileName));
	sprintf(achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFGDEBUG_INI);
#ifdef _LINUX_
	bIsTest = BrdGetE2promTestFlag();
#else
	bRet = GetRegKeyInt( achProfileName, SECTION_BoardDebug, "IsTest", 0, &sdwReturnValue );
	if( !bRet )  
	{
		printf( "[BrdAgent] Wrong profile while reading [%s][%s]!\n", SECTION_BoardDebug, "IsTest" );
	}
	bIsTest = (sdwReturnValue != 0);
#endif
	g_cBrdAgentApp.SetIsTest( bIsTest );
	if (bIsTest)
	{
		GetBoardInfo();//生产测试时要区分是HDU2板还是HDU2_L板
		printf("[ReadConfig]bIsTest,So Return!\n");
		return TRUE;
	}
    bRet = CBBoardConfig::ReadConnectMcuInfo();
    if ( !bRet )
    {
		printf("[ReadConfig]CBBoardConfig::ReadConnectMcuInfo Failed,So Return!\n");
        return bRet;
    }

	TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();
	bRet = GetRegKeyInt( achProfileName, SECTION_BoardConfig, KEY_Layer, 0, &sdwReturnValue );
	if( !bRet )  
	{
		printf( "[BrdAgent] Wrong profile while reading [%s][%s]!\n", SECTION_BoardConfig, KEY_Layer );
		return FALSE;
	}
	tBrdPos.byBrdLayer = (u8)sdwReturnValue;
	BOOL32 bIsSimuHdu = FALSE;
	bRet = GetRegKeyInt( achProfileName, SECTION_BoardConfig, "IsSimuHdu", 0, &sdwReturnValue );
	if( !bRet )  
	{
		printf( "[BrdAgent] Wrong profile while reading [%s][%s]!\n", SECTION_BoardConfig, "IsSimuHdu" );
	}
	else
	{
		bIsSimuHdu = (sdwReturnValue != 0);
		if (bIsSimuHdu)
		{
			if (tBrdPos.byBrdID == BRD_TYPE_HDU2)
			{
				tBrdPos.byBrdID = BRD_TYPE_HDU;
				//获取配置输出接口类型 其中YPbPr:1 VGA:2 DVI:3 HDMI:4
				//没有配置或其他非法值则表示不按照配置进行设置，而是依靠界面进行设置
				bRet = GetRegKeyInt( achProfileName, SECTION_BoardConfig, "OutPortType0", 0, &sdwReturnValue );
				g_cBrdAgentApp.SetOutPortTypeInCfg(0,0);
				g_cBrdAgentApp.SetOutPortTypeInCfg(1,0);
				if(bRet)
				{
					if ((u8)sdwReturnValue <= 4)
					{
						g_cBrdAgentApp.SetOutPortTypeInCfg(0,(u8)sdwReturnValue);
					}
				}
				bRet = GetRegKeyInt( achProfileName, SECTION_BoardConfig, "OutPortType1", 0, &sdwReturnValue );
				if(bRet)
				{
					if ((u8)sdwReturnValue <= 4)
					{
						g_cBrdAgentApp.SetOutPortTypeInCfg(1,(u8)sdwReturnValue);
					}
				}
			}
			else
			{
				printf( "[ReadConfig]tBrdPos.byBrdID(%d) Can't SimuHDU(%d),So Return!!!!\n",tBrdPos.byBrdID, BRD_TYPE_HDU );
				return FALSE;
			}
		}
	}
	g_cBrdAgentApp.SetBrdPosition(tBrdPos);
    return TRUE;
}
/*=====================================================================
函数  : InitBrdAgent
功能  : HDU2板代理初始化
输入  : void
输出  : 无
返回  : BOOL32
注    :
-----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
=====================================================================*/
API BOOL32 InitBrdAgent(void)
{
	static u16 wFlag = 0;
	if( wFlag != 0 )
	{
		printf("[InitBrdAgent]Hdu2BrdAgent Already Inited! So Return TURE!\n" );
		return TRUE;
	}
   	wFlag++;

#ifndef _VXWORKS_
    if( ERROR == BrdInit() )
    {
        printf("[InitBrdAgent]BrdInit failed,So Return FALSE!\n");
        return FALSE;
    }
#endif

    if ( !OspInit(TRUE, BRD_TELNET_PORT) )
    {
        printf("[InitBrdAgent]OspInit failed,So Return FALSE!\n");
        return FALSE;
    }

#ifndef WIN32
    OspTelAuthor( MCU_TEL_USRNAME, MCU_TEL_PWD );
#endif

	//得到配置信息
    if( !g_cBrdAgentApp.ReadConfig() )
    {
        printf("[InitBrdAgent] ReadConfig failed.\n");
        return FALSE;
    }

	if(g_cBrdAgentApp.GetIsTest())		//走生产测试路线
	{
		u32 dwBrdIpAddr = 0;

#ifdef _LINUX_
		BrdGetE2promIpAddr(&dwBrdIpAddr);//测试期间IP从E2PROM中读取
		if(dwBrdIpAddr == 0)
		{
			printf( "[InitBrdAgent]Read Board Ip from E2PROM error!\n" );
			return FALSE;
		}
		printf("[InitBrdAgent]Board Ip(network sequence): %x\n", dwBrdIpAddr);
        TBrdEthParam tEtherPara;
        tEtherPara.dwIpAdrs = dwBrdIpAddr;
        tEtherPara.dwIpMask = inet_addr("255.0.0.0");
        memset( tEtherPara.byMacAdrs, 0, sizeof(tEtherPara.byMacAdrs) );
        if( ERROR == BrdSetEthParam( 0, Brd_SET_IP_AND_MASK, &tEtherPara ) )
        {
            printf("[InitBrdAgent]Set Eth 0 IP failed\n");
        }
#endif

		g_cBrdAgentApp.SetBrdIpAddr(dwBrdIpAddr);
		return TRUE;
	}

    if ( !OspSemBCreate(&g_semHdu) )
	{
        printf("[InitBrdAgent]OspSemBCreate failed,So Return FALSE!\n");
		return FALSE;
	}
	
    if ( !OspSemTake( g_semHdu ) )
    {
        printf("[InitBrdAgent]OspSemTake failed,So Return FALSE!\n");
		return FALSE;
    }

	//创建单板代理应用
	g_cBrdAgentApp.CreateApp( "Hdu2Agent", AID_MCU_BRDAGENT, APPPRI_BRDAGENT );
	//创建单板守卫应用
	g_cBrdGuardApp.CreateApp( "Hdu2Guard", AID_MCU_BRDGUARD, APPPRI_BRDGUARD);
	//单板代理开始工作
	CBBoardConfig *pBBoardConfig = (CBBoardConfig*)&g_cBrdAgentApp;
	OspPost( MAKEIID(AID_MCU_BRDAGENT, 1), OSP_POWERON, &pBBoardConfig, sizeof(pBBoardConfig) );
	//单板守卫模块开始工作
	// 单板守卫保存单板类型信息，方便根据不同单板类型有不同告警 [12/1/2011 chendaiwei]
	u8 byBrdId = pBBoardConfig->GetBrdPosition().byBrdID;
	OspPost( MAKEIID(AID_MCU_BRDGUARD,0,0), BOARD_GUARD_POWERON,&byBrdId,sizeof(u8));
	//wait here
    if ( !OspSemTake( g_semHdu) )
    {
        printf("[InitBrdAgent] OspSemTake failed,So Return FALSE!\n");
		return FALSE;
    }
	OspSetLogLevel( AID_MCU_BRDAGENT, 0, 0);	 
	OspSetTrcFlag( AID_MCU_BRDAGENT, 0, 0);
	return TRUE;
}
/*=====================================================================
函数  : BrdGetDstMcuNode
功能  : 获取连接主MCU的NODE节点号
输入  : void
输出  : 无
返回  : u32
注    :
-----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
=====================================================================*/
u32 BrdGetDstMcuNode(void)
{
    return g_cBrdAgentApp.m_dwDstMcuNodeA;
}
/*=====================================================================
函数  : BrdGetDstMcuNodeB
功能  : 获取连接备MCU的NODE节点号
输入  : void
输出  : 无
返回  : u32
注    :
-----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
=====================================================================*/
u32 BrdGetDstMcuNodeB(void)
{
    return g_cBrdAgentApp.m_dwDstMcuNodeB;
}
