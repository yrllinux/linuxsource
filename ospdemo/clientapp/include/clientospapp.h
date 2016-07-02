/*==============================================================================
模块名  ：客户端APP
文件名  ：cclientospapp.h 
相关文件：clientinstance.h
实现功能：封装客户端Instance,实现外部调用接口
作者    ：闫日亮
版权    ：<Copyright(c) 2013-2013 Suzhou Keda Technology Co.,Ltd.All right reserved.>
--------------------------------------------------------------------------------
修改记录：
日  期        版本        修改人        走读人        修改记录
2013/08/08	  V1.0        闫日亮						创建
==============================================================================*/

#ifndef CCLIENTOSPAPP_H_
#define CCLIENTOSPAPP_H_

#include "clientinstance.h"

/*==============================================================================
类名    ：CClientOspApp
功能    ：封装OSP类，用于跟外部调用的交互

--------------------------------------------------------------------------------
修改记录：
日  期        版本        修改人            走读人            修改记录
2013/08/08    V1.0        闫日亮						     记录关键内容
==============================================================================*/
class CClientOspApp
{
public:
	CClientOspApp();
	~CClientOspApp();
	
	BOOL InitClient();								//初始化客户端
	BOOL CreatClientApp();							//创建APP
	void ClientConnectServer();						//客户端连接服务器
	void ClientDisconnectServer();					//断开连接					
	void ClientUploadFile(s8 *chUploadFile, DWORD dwGetFileSize);//客户端上传文件到服务器
	void ClientPauseUploadFile();					//客户端上传文件时暂停
	void ClientRestartUploadFile();					//客户端暂停后重新开始上传
	void ClientRemoveFile();						//客户端删除文件
	void ClientSetServerIpandPort(const s8 *pchServerIp, s32 nServerPort);//客户端设置要连接的服务器的IP跟端口号

private:
	s8 m_pchServerIp[MAX_NAME_LEN];					//设置的服务器IP
	s32 m_nServerPort;								//设置的服务器PORT
};

typedef zTemplate<CClientInstance, 1, CClientOspApp, 20> COspApp;
extern COspApp g_zClientOspApp;

#endif
