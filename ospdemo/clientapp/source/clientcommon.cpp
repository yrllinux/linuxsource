/*==============================================================================
模块名  ：公共接口
文件名  ：clientcommon.cpp  
相关文件：clientospapp.h,stdafx.h
实现功能：封装公共接口
作者    ：闫日亮
版权    ：<Copyright(c) 2013-2013 Suzhou Keda Technology Co.,Ltd.All right reserved.>
--------------------------------------------------------------------------------
修改记录：
日  期        版本        修改人        走读人        修改记录
2013/08/08	  V1.0        闫日亮						创建
==============================================================================*/
#include "stdafx.h"
#include "clientospapp.h"

/////////////////////////////封装接口，给外部调用///////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*==========================================================
函数名：CInitClient
功能：初始化服务器
算法实现：<可选项>
参数说明：无
返回值说明：TRUE 初始化、创建APP成功  FALSE 失败
============================================================*/	
BOOL CInitClient()
{
	//判断是否初始化
	return g_zClientOspApp.InitClient();
}	

/*==========================================================
函数名：CCreatApp
功能：创建APP
算法实现：<可选项>
参数说明：无
返回值说明：TRUE 创建APP成功  FALSE 失败
============================================================*/	
BOOL CCreatApp()
{
	//创建客户端APP是否成功
	return g_zClientOspApp.CreatClientApp();
}

/*==========================================================
函数名：CConnectServer
功能：连接服务器
算法实现：<可选项>
参数说明：无
返回值说明：无
============================================================*/
void CConnectServer()
{
	g_zClientOspApp.ClientConnectServer();
}

/*==========================================================
函数名：CDisconnectServer
功能：断开服务器连接
算法实现：<可选项>
参数说明：无
返回值说明：无
============================================================*/	
void CDisconnectServer()
{
	g_zClientOspApp.ClientDisconnectServer();
}

/*==========================================================
函数名：CUploadFile
功能：上传文件
算法实现：<可选项>
参数说明：chUploadFile：上传的文件名
返回值说明：无
============================================================*/	
void CUploadFile(s8 *chUploadFile, DWORD dwGetFileSize)
{
	if ( strlen(chUploadFile) == 0 )
	{
		return;
	}

	g_zClientOspApp.ClientUploadFile(chUploadFile, dwGetFileSize);
}

/*==========================================================
函数名：CPauseUploadFile
功能：暂停上传
算法实现：<可选项>
参数说明：无
返回值说明：无
============================================================*/
void CPauseUploadFile()
{
	g_zClientOspApp.ClientPauseUploadFile();
}

/*==========================================================
函数名：CRestartUploadFile
功能：上传暂停后重新开始
算法实现：<可选项>
参数说明：无
返回值说明：无
============================================================*/	
void CRestartUploadFile()
{
	g_zClientOspApp.ClientRestartUploadFile();
}

/*==========================================================
函数名：CRemoveFile
功能：删除列表文件
算法实现：<可选项>
参数说明：无
返回值说明：无
============================================================*/
void CRemoveFile()
{
	g_zClientOspApp.ClientRemoveFile();
}

/*==========================================================
函数名：CGetServerIpandPort
功能：获得用户设置的服务器IP根端口号
算法实现：<可选项>
参数说明：无
返回值说明：无
============================================================*/	
void CSetServerIpandPort(const s8 *pchServerIp, s32 nServerPort)
{
	if ( strlen(pchServerIp) == 0 )
	{
		CB_CToSProgress(C_SEND_FILE_TO_S_SETERR);					//设置有误回调
		return;
	}
	
	g_zClientOspApp.ClientSetServerIpandPort(pchServerIp, nServerPort);	
}

//////////////////////////////Telnet接口/////////////////////////////////

#define DEMO_VER  "Vertion: 2013/08/08 DEMO_CLIENT V1.0!"

/*==========================================================
函数名 clientsetloglevel
功能 ：设置打印级别
算法实现：<可选项>
参数说明：无
返回值说明：void
============================================================*/
API void clientsetloglevel(u8 byScrnLevel)
{
	OspSetScrnLogLevel(0, byScrnLevel);
	OspPrintf( TRUE, FALSE, "setloglevel %d!\n", byScrnLevel);
}

/*==========================================================
函数名 clientversion
功能 ：打印当前版本信息
算法实现：<可选项>
参数说明：无
返回值说明：void
============================================================*/
API void clientversion()
{
	OspPrintf( TRUE, FALSE, "%s  compile time: %s, %s\n", DEMO_VER, __TIME__, __DATE__ );
}

/*==========================================================
函数名 clienthelp
功能   帮助信息
算法实现：<可选项>
参数说明：无
返回值说明：void
============================================================*/
API void clienthelp()
{
    OspPrintf(TRUE, FALSE, "\n");
    OspPrintf(TRUE, FALSE, "********************** 帮助 *********************\n");
    OspPrintf(TRUE, FALSE, "*                                               *\n");
    OspPrintf(TRUE, FALSE, "*你可以输入以下命令：                           *\n");
    OspPrintf(TRUE, FALSE, "*设置Log级别:      clientsetloglevel            *\n");
    OspPrintf(TRUE, FALSE, "*查看版本信息:     clientversion                *\n");
    OspPrintf(TRUE, FALSE, "*查看帮助:         clienthelp                   *\n");
    OspPrintf(TRUE, FALSE, "*设置Log级别时需带参数，例如：                  *\n");
    OspPrintf(TRUE, FALSE, "*      clientsetloglevel 1                      *\n");
    OspPrintf(TRUE, FALSE, "*参数说明如下：                                 *\n");
    OspPrintf(TRUE, FALSE, "*      0 不打印任何正常LOG                      *\n");
    OspPrintf(TRUE, FALSE, "*      1 打印连接，发送，断开等LOG              *\n");
    OspPrintf(TRUE, FALSE, "*      2 打印发送消息LOG                        *\n");
    OspPrintf(TRUE, FALSE, "*      3 打印传送百分比LOG                      *\n");
    OspPrintf(TRUE, FALSE, "*************************************************\n");
}

/*==========================================================
函数名 CB_CToSProgress
功能   回调函数
算法实现：<可选项>
参数说明：无
返回值说明：void
============================================================*/
void (*pCB_CToSProgress)(s8 chProgress) = NULL;					//函数指针声明

void CB_CToSProgress(s8 chProgress)								
{
	pCB_CToSProgress(chProgress);
}

void CB_CCallBack(void (*pCB_CToCProgress)(s8 chProgress))
{
	pCB_CToSProgress = pCB_CToCProgress;
}

#ifdef __cplusplus
}
#endif  /* __cplusplus */

//end