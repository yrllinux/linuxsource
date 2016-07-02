/*==============================================================================
模块名  ：服务器APP
文件名  ：cserverospapp.h 
相关文件：serverinstance.h
实现功能：封装服务器Instance,实现外部调用接口
作者    ：闫日亮
版权    ：<Copyright(c) 2013-2013 Suzhou Keda Technology Co.,Ltd.All right reserved.>
--------------------------------------------------------------------------------
修改记录：
日  期        版本        修改人        走读人        修改记录
2013/08/08	  V1.0        闫日亮						创建
==============================================================================*/

#ifndef CCLIENTOPSAPP_H_
#define CCLIENTOPSAPP_H_

#include "serverinstance.h"

/*==============================================================================
类名    ：CServerOspApp
功能    ：封装OSP模板类 zTemplate 的类，用于跟外部调用的交互

--------------------------------------------------------------------------------
修改记录：
日  期        版本        修改人            走读人            修改记录
2013/08/08    V1.0        闫日亮						     记录关键内容
==============================================================================*/
class CServerOspApp
{
public:
	CServerOspApp();
	~CServerOspApp();
	
	void CInitServer();							//初始化服务器
	void CCreateServerTcpNode();				//创建服务器结点跟app

private:
	s32		m_nServerNode;						//记录服务器NODE
};

typedef zTemplate<CServerInstance, SERVER_MAX_INS,CServerOspApp, 20> COspApp;
extern COspApp g_zServerOspApp;					//封装zTemplate模板类


//服务器状态
typedef enum ServerState
{
		SERVER_INIT_OK,							//服务器初始化成功
		SERVER_INIT_FAIL,						//服务器初始化失败
		SERVER_NOSPACE							//服务器空间不足
}ServerState;


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern s32 g_nServerPort;						//记录设置的端口号
extern s8 g_achServerBase[MAX_NAME_LEN];		//记录文件存放的目录
			
//////////////////////////////第三方接口/////////////////////////////////
//对CClientOspApp类进行封装，提供第三方接口，功能相同
void CB_RevProgress(s8 chProgress);										//该函数由用户填充，对参数进行处理
void CB_ServerCallBack(void (*pCB_ServerRevProgress)(s8 chProgress));	//回调函声明

//////////////////////////////Telnet接口/////////////////////////////////

API void serversetloglevel(u8 byScrnLevel);							
API void serverhelp();
API void serverversion();
API void serverspace();
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif

//end
