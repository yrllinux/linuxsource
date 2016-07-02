/*==============================================================================
模块名  ：服务器APP
文件名  ：cserverospapp.cpp 
相关文件：serverospapp.h,stdafx.h
实现功能：封装服务器Instance,实现外部调用接口
作者    ：闫日亮
版权    ：<Copyright(c) 2013-2013 Suzhou Keda Technology Co.,Ltd.All right reserved.>
--------------------------------------------------------------------------------
修改记录：
日  期        版本        修改人        走读人        修改记录
2013/08/08	  V1.0        闫日亮						创建
==============================================================================*/
#include "stdafx.h"
#include "serverospapp.h"

COspApp g_zServerOspApp;						//封装zTemplate模板类

/*==========================================================
函数名 CServerOspApp
功能 ：CServerOspApp的构造函数，初始化成员
算法实现：<可选项>
参数说明：无
返回值说明：无
============================================================*/
CServerOspApp::CServerOspApp()
{
	m_nServerNode = INVALID_NODE;
}

/*==========================================================
函数名 ~CServerOspApp
功能 ：CServerOspApp的析构函数，释放资源
算法实现：<可选项>
参数说明：无
返回值说明：无
============================================================*/
CServerOspApp::~CServerOspApp()
{
	m_nServerNode = INVALID_NODE;
}

/*==========================================================
函数名 CInitServer
功能 ：初始化服务器
算法实现：<可选项>
参数说明：无
返回值说明：无
============================================================*/
void CServerOspApp::CInitServer()
{
	//初始化osp
	if ( IsOspInitd() )
	{
		//已经初始化
		return;
	}

#ifdef _DEBUG
	if ( OspInit(TRUE, 0) )					//DEBUG初始化osp，打开telnet显示
	{
#else
	if ( OspInit(FALSE, 0) )				//RELEASE初始化osp，关闭telnet显示
	{
		serversetloglevel(0);				//设置打印级别，默认release状态关闭所有打印
#endif
		OspSetPrompt("OspServer");			//显示telnet名字
		serverhelp();						//输出帮助信息
		CB_RevProgress(SERVER_INIT_OK);
		OspLog(LOGPRINTLEVELH, "初始化环境成功!\n");
		return;
	}
	CB_RevProgress(SERVER_INIT_FAIL);		
}

/*==========================================================
函数名 CCreateServerTcpNode
功能 ：创建服务器结点跟APP
算法实现：<可选项>
参数说明：无
返回值说明：无
============================================================*/
void CServerOspApp::CCreateServerTcpNode()
{
	//检查是否已经创建过结点
	if ( m_nServerNode != INVALID_NODE)
	{
		return;
	}

	//创建服务器结点
	m_nServerNode = OspCreateTcpNode(0, g_nServerPort);
	if ( m_nServerNode == INVALID_NODE )
	{
		CB_RevProgress(SERVER_INIT_FAIL);		
		OspLog(LOGPRINTLEVELH, "创建结点失败!\n");
		return;
	}
	
	//创建服务器APP
	if ( OSP_ERROR == g_zServerOspApp.CreateApp("OspServerApp", SERVER_APP_ID, APP_PRIO) )
	{
		CB_RevProgress(SERVER_INIT_FAIL);		
		OspLog(LOGPRINTLEVELH, "创建OspServerApp 失败!\n");
		return;
	}
}

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern s64 g_dwServerSpace;
/////////////////////////////Telnet接口/////////////////////////////////

#define DEMO_VER  "Vertion: 2013/08/08 DEMO_SERVER V1.0!"

/*==========================================================
函数名 setloglevel
功能 ：设置打印级别
算法实现：<可选项>
参数说明：无
返回值说明：void
============================================================*/
API void serversetloglevel(u8 byScrnLevel)
{
	OspSetScrnLogLevel(0, byScrnLevel);
	OspPrintf( TRUE,FALSE, "setloglevel %d!\n", byScrnLevel);
}

/*==========================================================
函数名 serverver
功能 ：打印当前版本信息
算法实现：<可选项>
参数说明：无
返回值说明：void
============================================================*/
API void serverver()
{
	OspPrintf( TRUE,FALSE, "%s  compile time: %s, %s\n", DEMO_VER, __TIME__, __DATE__ );
}

/*==========================================================
函数名 serverspace
功能 ：打印当前剩余空间
算法实现：<可选项>
参数说明：无
返回值说明：void
============================================================*/
API void serverspace()
{
	//获取当前磁盘大小
	ULARGE_INTEGER nFreeBytesAvailable;
	GetDiskFreeSpaceEx(_T(g_achServerBase), &nFreeBytesAvailable, 0, 0);
	ULONGLONG dwServerSpace = nFreeBytesAvailable.QuadPart / 1024 / 1024;
	OspPrintf( TRUE,FALSE, "当前剩余空间：%ld M\n", dwServerSpace );
}

/*==========================================================
函数名 serverhelp
功能   帮助信息
算法实现：<可选项>
参数说明：无
返回值说明：void
============================================================*/
API void serverhelp()
{
    OspPrintf(TRUE, FALSE, "\n");
    OspPrintf(TRUE, FALSE, "********************** 帮助 *********************\n");
    OspPrintf(TRUE, FALSE, "*                                               *\n");
    OspPrintf(TRUE, FALSE, "*你可以输入以下命令：                           *\n");
    OspPrintf(TRUE, FALSE, "*设置Log级别:      serversetloglevel            *\n");
    OspPrintf(TRUE, FALSE, "*查看版本信息:     serverversion                *\n");
	OspPrintf(TRUE, FALSE, "*查看当前剩余空间: serverspace                  *\n");
    OspPrintf(TRUE, FALSE, "*查看帮助:         serverhelp                   *\n");
    OspPrintf(TRUE, FALSE, "*设置Log级别时需带参数，例如：                  *\n");
    OspPrintf(TRUE, FALSE, "*      serversetloglevel 1                      *\n");
    OspPrintf(TRUE, FALSE, "*参数说明如下：                                 *\n");
    OspPrintf(TRUE, FALSE, "*      0 不打印任何正常LOG                      *\n");
    OspPrintf(TRUE, FALSE, "*      1 打印连接，发送，断开等LOG              *\n");
    OspPrintf(TRUE, FALSE, "*      2 打印接收消息LOG                        *\n");
    OspPrintf(TRUE, FALSE, "*************************************************\n");
}

/*==========================================================
函数名 CB_RevProgress
功能   回调函数
算法实现：<可选项>
参数说明：无
返回值说明：void
============================================================*/
void (*pCB_RevProgress)(s8 chProgress) = NULL;					//函数指针声明

void CB_RevProgress(s8 chProgress)								
{
	pCB_RevProgress(chProgress);
}

void CB_ServerCallBack(void (*pCB_ServerRevProgress)(s8 chProgress))
{
	pCB_RevProgress = pCB_ServerRevProgress;
}
#ifdef __cplusplus
}
#endif  /* __cplusplus */

//end