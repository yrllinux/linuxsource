/*==============================================================================
模块名  ：公共接口
文件名  ：Clientcommon.h   
相关文件：osp.h,ospcomm.h,kdvtype.h
实现功能：提供给外部调用的公共接口
作者    ：闫日亮
版权    ：<Copyright(c) 2013-2013 Suzhou Keda Technology Co.,Ltd.All right reserved.>
--------------------------------------------------------------------------------
修改记录：
日  期        版本        修改人        走读人        修改记录
2013/08/08	  V1.0        闫日亮						创建
==============================================================================*/
#ifndef CLIENTCOMMON_H_
#define CLIENTCOMMON_H_

#include "osp.h"
#include "ospcomm.h"
#include "kdvtype.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern s32 g_nServerNode;							//客户端获取的node

//客户端状态
typedef enum CSendFileState
{
		C_SEND_FILE_TO_S_SETERR	= 100,				//C->S设置ip有误
		C_SEND_FILE_TO_S_UPLOADING,					//C->S发送文件中
		C_SEND_FILE_TO_S_OVER,						//C->S发送完毕
		C_SEND_FILE_TO_S_FAIL,						//C->S发送失败
		C_SEND_FILE_TO_S_PAUSE,						//C->S暂停中
		C_SEND_FILE_TO_S_NOPAUSE,					//C->S发送文件小于50M，不允许暂停
		C_SEND_FILE_TO_S_BUSYNODEL,					//C->S上传时不能删除文件
		C_SEND_FILE_TO_S_COEENCTING,				//C->S正在连接中
		C_SEND_FILE_TO_S_COEENCT,					//C->S建立连接
		C_SEND_FILE_TO_S_NACKCOEENCT,				//C->S建立连接失败
		C_SEND_FILE_TO_S_DICCOEENCT,				//C->S断开连接
		C_SEND_FILE_TO_S_DELETFILE					//C->S删除文件
}CSendFileState;

//////////////////////////////第三方接口/////////////////////////////////
//对CClientOpsApp类进行封装，提供第三方接口，功能相同
void CB_CToSProgress(s8 chProgress);								//该函数由用户填充，对参数进行处理
void CB_CCallBack(void (*pCB_CToCProgress)(s8 chProgress));			//回调函声明
BOOL CCreatApp();													//创建APP
BOOL CInitClient();													//初始化客户端
void CConnectServer();												//客户端连接服务器
void CDisconnectServer();											//断开连接	
void CUploadFile(s8 *chUploadFile, DWORD dwGetFileSize);			//客户端上传文件到服务器
void CPauseUploadFile();											//客户端上传文件时暂停
void CRestartUploadFile();											//客户端暂停后重新开始上传
void CRemoveFile();													//客户端删除文件
void CSetServerIpandPort(const s8 *pchServerIp, s32 nServerPort);	//客户端设置要连接的服务器的IP跟端口号

//////////////////////////////Telnet接口/////////////////////////////////
API void clientsetloglevel(u8 byScrnLevel);
API void clienthelp();
API void clientversion();

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif