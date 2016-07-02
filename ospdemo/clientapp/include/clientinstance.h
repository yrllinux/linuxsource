/*==============================================================================
模块名  ：客户端实例
文件名  ：ClientInstance.h   
相关文件：
实现功能：继承类CInstance,实现实例相关功能
作者    ：闫日亮
版权    ：<Copyright(c) 2013-2013 Suzhou Keda Technology Co.,Ltd.All right reserved.>
--------------------------------------------------------------------------------
修改记录：
日  期        版本        修改人        走读人        修改记录
2013/08/08	  V1.0        闫日亮						创建
==============================================================================*/
#ifndef CLIENTINSTANCE_H_
#define CLIENTINSTANCE_H_

#include "clientcommon.h"

/*==============================================================================
类名    ：CClientInstance
功能    ：继承OSP模板类 CInstance 的类，实现实例的通信处理功能

--------------------------------------------------------------------------------
修改记录：
日  期        版本        修改人            走读人            修改记录
2013/08/08    V1.0        闫日亮						     记录关键内容
==============================================================================*/
class CClientInstance : public CInstance 
{

public:

	CClientInstance();
	~CClientInstance();

	void DaemonInstanceEntry( CMessage *const pMsg, CApp* pcApp);	//实例守护入口
	void InstanceEntry( CMessage *const pMsg);						//实例入口

private:
	void ClientConnectToServer();									//C->S客户端连接到服务器
	void CDisconnectManage();									//C->C客户端断开连接处理函数
	void CToSConnectAckManage(CMessage *const pMsg);				//S->C客户端收到服务器连接应答处理函数
	void CUploadFileReqToServer(CMessage *const pMsg);				//C->S客户端请求上传文件到服务器
	void CUploadFileToS();											//C->S客户端上传文件函数
	void CUploadToSAckManage();										//S->C客户端收到上传文件应答函数
	void CUploadToSNackManage(CMessage *const pMsg);				//S->C客户端收到上传请求拒绝的处理函数
	void CUploadingToSAckManage();									//S->C客户端收到继续上传文件应答的处理函数
	void CPauseUploadFileToS();										//C->C客户端上传文件过程中暂停
	void CRestartUploadFileToS();									//C->C客户端暂停文件上传后重新开始上传函数
	void CPauseRemoveFileManage();									//C->C客户端暂停时删除列表文件时的出错处理
	void CReadFileErrManage();										//客户端上传文件失败时的处理
	void ClientInstanceRelease();									//客户端释放资源
private:
	FILE *m_pFile;								//传输的文件指针
	s32 m_nServerID;							//服务器ID
	s32 m_nServerNode;							//服务器node
	u32 m_uCount;								//记录上传文件包数
	u32 m_uPacketCount;							//记录上传文件分割总包数
	DWORD m_uLastPacketSize;					//记录最后一个数据包的大小
	u32 m_dProgress;							//记录传输进度

	CFileInfo m_tCFileInfo;						//记录上传文件的信息结构体	
	UploadFileInfo m_tUploadFileInfo;			//记录上传文件的信息结构体

	enum InsState
	{
		STATE_IDLE,								//空闲状态
		STATE_CONNECT,							//连接结点成功状态
		STATE_READY,							//建立连接状态
		STATE_BUSY,								//上传状态
		STATE_PAUSE								//暂停状态
	};
};

#endif
