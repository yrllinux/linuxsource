/*****************************************************************************
   模块名      : Board Agent
   文件名      : vasinit.cpp
   相关文件    : 
   文件实现功能: 单板启动及相关普通函数定义
   作者        : jianghy
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/08/25  1.0         jianghy       创建
   2004/11/11  3.5         李 博         新接口的修改
******************************************************************************/
#include "osp.h"
#include "vasagent.h"
#include "vasguard.h"
#include "evagtsvc.h"
#include "mcuconst.h"
#include "mcuver.h"

/*====================================================================
    函数名      ：SetDriAgentDebug
    功能        ：设置MMPAGENT打印为DEBUG级别
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/01/12    1.0         jianghy       创建
====================================================================*/
API void SetVasAgentDebug()
{
	OspSetLogLevel( AID_MCU_BRDAGENT, 255, 255);	 
	OspSetTrcFlag( AID_MCU_BRDAGENT, 255, 255);

	OspSetLogLevel( AID_MCU_BRDGUARD, 255, 255);	 
	OspSetTrcFlag( AID_MCU_BRDGUARD, 255, 255);
	return;
}

/*====================================================================
    函数名      ：IptoStr
    功能        ：将IP地址(主机序)转换成字符串
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：IP字符串
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/01/12    1.0         jianghy       创建
====================================================================*/
static char* IptoStr( u32 dwIpAddr )
{
	static char achPeerIpAddr[20];
	char* pchPeerIpAddr;
	u32 dwIp = htonl( dwIpAddr );

#ifndef WIN32   /*将IP地址转换成字符串*/ 
	inet_ntoa_b(*(struct in_addr*)&dwIp, achPeerIpAddr);
#else
	pchPeerIpAddr = inet_ntoa( *(struct in_addr*)&dwIp );
	strncpy( achPeerIpAddr, pchPeerIpAddr, sizeof(achPeerIpAddr) );
	achPeerIpAddr[ sizeof(achPeerIpAddr) - 1 ] = '\0';	
#endif

	return (char*)achPeerIpAddr;
}

/*====================================================================
    函数名      ：DriAgentSelfTest
    功能        ：打印当前的配置信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/01/12    1.0         jianghy       创建
====================================================================*/
API void VasAgentSelfTest()
{
#ifndef WIN32
	//单板位置, zgc, 07/02/02
	TBrdPosition tBrdPos;
	BrdQueryPosition( &tBrdPos );
	OspPrintf( TRUE, FALSE, "Brd Layer: %d, Brd Slot: %d\n", tBrdPos.byBrdLayer, tBrdPos.byBrdSlot );
#endif
	return;
}


/*====================================================================
    函数名      ：vasagtver
    功能        ：打印当前模块的版本信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/01/12    1.0         jianghy       创建
====================================================================*/
API void vasagtver(void)
{
	OspPrintf( TRUE, FALSE, "VasAgent: %s  compile time: %s    %s\n",  
		VER_VASAGENT, __DATE__, __TIME__ );
}

/*====================================================================
    函数名      ：VasAgentAPIEnableInLinux
    功能        ：向Linux注册API函数
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/02/05    4.0         周广程         创建
====================================================================*/
void VasAgentAPIEnableInLinux()
{
#ifdef _LINUX_ 
    OspRegCommand("vasagtver", (void*)vasagtver, "vasagt version command");    
    OspRegCommand("SetVasAgentDebug",   (void*)SetVasAgentDebug,   "set vasagt debug command");  
    OspRegCommand("VasAgentSelfTest", (void*)VasAgentSelfTest, "self test debug command");
#endif
    
    return;
}

/*====================================================================
    函数名      ：InitVasAgent
    功能        ：初始化Vas单板代理
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：成功返回TRUE，反之FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/01/12    1.0         jianghy       创建
====================================================================*/
API BOOL InitVasAgent()
{

#ifdef _LINUX_
	if( ERROR == BrdInit() )
	{
		printf("[InitAgent] call BrdInit fail.\n");
		return FALSE;
	}
#endif

	//创建单板代理应用
	g_cBrdAgentApp.CreateApp( "vasAgent", AID_MCU_BRDAGENT, APPPRI_BRDAGENT );
	//创建单板守卫应用
	g_cBrdGuardApp.CreateApp( "vasGuard", AID_MCU_BRDGUARD, APPPRI_BRDGUARD);
	//单板代理开始工作
	OspPost( MAKEIID(AID_MCU_BRDAGENT,0,0), BOARD_AGENT_POWERON );
	//单板守卫模块开始工作
	OspPost( MAKEIID(AID_MCU_BRDGUARD,0,0), BOARD_GUARD_POWERON );

	OspSetLogLevel( AID_MCU_BRDAGENT, 0, 0);	 
	OspSetTrcFlag( AID_MCU_BRDAGENT, 0, 0);

	OspSetLogLevel( AID_MCU_BRDGUARD, 0, 0);	 
	OspSetTrcFlag( AID_MCU_BRDGUARD, 0, 0);

	return TRUE;
}

