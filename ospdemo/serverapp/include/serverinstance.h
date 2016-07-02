/*==============================================================================
模块名  ：服务器实例
文件名  ：serverinstance.h  
相关文件：osp.h, ospcomm.h, kdvtype.h
实现功能：继承类CInstance,实现实例相关功能
作者    ：闫日亮
版权    ：<Copyright(c) 2013-2013 Suzhou Keda Technology Co.,Ltd.All right reserved.>
--------------------------------------------------------------------------------
修改记录：
日  期        版本        修改人        走读人        修改记录
2013/08/08	  V1.0        闫日亮						创建
==============================================================================*/

#ifndef SERVERINSTANCE_H_
#define SERVERINSTANCE_H_

#include "osp.h"
#include "ospcomm.h"
#include "kdvtype.h"
/*==============================================================================
类名    ：CServerInstance
功能    ：继承OSP模板类 CInstance 的类，实现实例的通信处理功能

--------------------------------------------------------------------------------
修改记录：
日  期        版本        修改人            走读人            修改记录
2013/08/08    V1.0        闫日亮						     记录关键内容
==============================================================================*/
class CServerInstance : public CInstance
{
public:
	CServerInstance();
	~CServerInstance();

	void DaemonInstanceEntry( CMessage *const pMsg,CApp* pcApp);	//守护实例入口
	void InstanceEntry( CMessage *const pMsg);						//实例入口

private:
	void CConnectSReqManage(CMessage *const pMsg);					//客户端请求连接服务器的处理函数
	void DisConnectedCtoSManage(u32 SrcNodeID);						//断开C->S连接处理函数
	void CUploadtoSReqManage(CMessage *const pMsg);					//C->S请求上传处理函数
	void CUploadingtoSReqManage(CMessage *const pMsg);				//C->S请求继续上传处理函数
	void CUploadtoSErrManage();										//C->S上传失败处理函数
	void FileRenameManage(s8 *pFileName);							//服务器创建文件时的重名检测函数
	BOOL IsServerSpaceEnough(DWORD dwFileSize);						//服务器判断当前空间是否充足的函数
	void ServerInstanceRelease();									//服务器释放资源
	void FileNameOpt(s8 *pchName);									//服务器通过路径文件名获取文件名的函数

	s8	m_achPathFileName[MAX_NAME_LEN];			//记录文件路径名
	s32 m_nSaveClientNode;                          //保存客户端NODE号
	s32 m_nSaveClientID;                            //保存客户端ID号 
	s32 m_dwServerSpace;							//记录磁盘空间大小
	FILE		   *m_phGetFile;					//创建的文件句柄
	UploadFileInfo m_tGetFileInfo;					//接收的文件信息结构体
	CFileInfo m_tCGetFileInfo;					    //接收的文件信息结构体

	enum InsState
	{
		STATE_IDLE,									//空闲状态
		STATE_READY,								//连接状态
		STATE_BUSY									//接收文件状态
	};
};

#endif

//end
