/*==============================================================================
模块名  ：客户端APP
文件名  ：clientospapp.h.cpp 
相关文件：clientospapp.h,stdafx.h
实现功能：实现与外部及客户端的交互
作者    ：闫日亮
版权    ：<Copyright(c) 2013-2013 Suzhou Keda Technology Co.,Ltd.All right reserved.>
--------------------------------------------------------------------------------
修改记录：
日  期        版本        修改人        走读人        修改记录
2013/08/08	  V1.0        闫日亮						创建
==============================================================================*/
#include "stdafx.h"
#include "clientospapp.h"

COspApp g_zClientOspApp;
s32 g_nServerNode = INVALID_NODE;							//客户端获取的node
/*==========================================================
函数名 CClientOspApp
功能 ：CClientOspApp的构造函数，初始化成员
算法实现：<可选项>
参数说明：无
返回值说明：无
============================================================*/
CClientOspApp::CClientOspApp()
{
	//成员变量初始化
}

/*==========================================================
函数名：~CClientOspApp
功能：CClientOpsApp的析构函数，释放资源
算法实现：<可选项>
参数说明：无
返回值说明：无
============================================================*/
CClientOspApp::~CClientOspApp()
{
	
}

/*==========================================================
函数名：CInitClient
功能：初始化服务器
算法实现：<可选项>
参数说明：无
返回值说明：TRUE 初始化成功  FALSE 失败
============================================================*/	
BOOL CClientOspApp::InitClient()
{
	//判断是否初始化
	if ( IsOspInitd() )
	{
		OspLog(LOGPRINTLEVELH, "已经初始化!\n"); 
		return TRUE;
	}
	else
	{
		//未初始化
#ifdef _DEBUG
		if ( OspInit(TRUE, 0, "OspClient") )					//初始化
		{
#else
		if ( OspInit(FALSE, 0, "OspClient") )
		{
			clientsetloglevel(0);								//Release模式默认关闭打印
#endif		
			OspSetPrompt("OspClient");							//显示telnet名字
			clienthelp();										//显示打印信息
			OspLog(LOGPRINTLEVELH, "初始化成功!\n");
			return TRUE;
		}
		else
		{
			OspLog(LOGPRINTLEVELH, "初始化失败!\n");
			return FALSE;
		}
	}
}	

/*==========================================================
函数名：CCreatApp
功能：创建APP
算法实现：<可选项>
参数说明：无
返回值说明：TRUE 创建APP成功  FALSE 失败
============================================================*/	
BOOL CClientOspApp::CreatClientApp()
{
	//创建客户端APP
	if ( g_zClientOspApp.CreateApp("OspClientApp", CLIENT_APP_ID, APP_PRIO) == OSP_ERROR )
	{
		OspLog(LOGPRINTLEVELH, "CreatClientApp创建失败!\n"); 
		return FALSE;
	}

	//连接服务器结点
	g_nServerNode = OspConnectTcpNode(inet_addr(m_pchServerIp), m_nServerPort);
	if ( g_nServerNode == INVALID_NODE )
	{
		OspLog(LOGPRINTLEVELH, "OspConnectTcpNode连接服务器结点失败!\n");
		return FALSE;
	}
	return TRUE;
}

/*==========================================================
函数名：CConnectServer
功能：连接服务器
算法实现：<可选项>
参数说明：无
返回值说明：无
============================================================*/	
void CClientOspApp::ClientConnectServer()
{
	//向客户端Instance发送连接消息C->C
	OspPost( MAKEIID( CLIENT_APP_ID, 1), CTOC_CONN_REQ, NULL, 0);
}

/*==========================================================
函数名：CDisconnectServer
功能：断开服务器连接
算法实现：<可选项>
参数说明：无
返回值说明：无
============================================================*/	
void CClientOspApp::ClientDisconnectServer()
{
	//向客户端Instance发送断开消息C->C
	OspPost( MAKEIID( CLIENT_APP_ID, 1), CTOC_CONN_DISC, NULL, 0);
}

/*==========================================================
函数名：CUploadFile
功能：上传文件
算法实现：<可选项>
参数说明：chUploadFile：上传的文件名
返回值说明：无
============================================================*/	
void CClientOspApp::ClientUploadFile(s8 *chUploadFile, DWORD dwGetFileSize)
{
	//保存上传的文件信息
	CFileInfo m_tCFileInfo;
	memset(&m_tCFileInfo,0,sizeof(CFileInfo));
	memcpy(m_tCFileInfo.achFileName,chUploadFile,MAX_PATH);
	m_tCFileInfo.dwFileSize = dwGetFileSize;
	//向客户端Instance发送上传文件消息C->C
	OspPost( MAKEIID( CLIENT_APP_ID, 1), CTOC_UPLOAD_REQ, &m_tCFileInfo, sizeof(CFileInfo));
}

/*==========================================================
函数名：CPauseUploadFile
功能：暂停上传
算法实现：<可选项>
参数说明：无
返回值说明：无
============================================================*/	
void CClientOspApp::ClientPauseUploadFile()
{
	//向客户端Instance发送暂停上传消息C->C
	OspPost( MAKEIID( CLIENT_APP_ID, 1), CTOC_UPLOADING_PAUSE, NULL, 0);
}

/*==========================================================
函数名：CRestartUploadFile
功能：上传暂停后重新开始
算法实现：<可选项>
参数说明：无
返回值说明：无
============================================================*/	
void CClientOspApp::ClientRestartUploadFile()
{
	//向客户端Instance发送继续上传消息C->C
	OspPost( MAKEIID( CLIENT_APP_ID, 1), CTOC_UPLOADING_RESTART, NULL, 0);
}

/*==========================================================
函数名：CRemoveFile
功能：删除列表文件
算法实现：<可选项>
参数说明：无
返回值说明：无
============================================================*/	
void CClientOspApp::ClientRemoveFile()
{
	//向客户端Instance发送删除列表文件消息C->C
	OspPost( MAKEIID( CLIENT_APP_ID, 1), CTOC_FILE_REMOVE, NULL, 0);
}

/*==========================================================
函数名：CGetServerIpandPort
功能：获得用户设置的服务器IP根端口号
算法实现：<可选项>
参数说明：无
返回值说明：无
============================================================*/	
void CClientOspApp::ClientSetServerIpandPort(const s8 *pchServerIp, s32 nServerPort)
{
	memset(&m_pchServerIp, 0, MAX_NAME_LEN);

	//判断是否非法
	if ( strlen(pchServerIp) == 0 )
	{
		CB_CToSProgress(C_SEND_FILE_TO_S_SETERR);					//设置有误回调
		return;
	}
	//保存IP
	memcpy(m_pchServerIp, pchServerIp, MAX_NAME_LEN);		
	//保存端口号
	m_nServerPort = nServerPort; 
}

//end
