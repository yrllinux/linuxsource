/*==============================================================================
模块名  ：客户端实例
文件名  ：clientinstance.cpp  
相关文件：clientinstance.h,stdafx.h
实现功能：实现Instance通信功能
作者    ：闫日亮
版权    ：<Copyright(c) 2013-2013 Suzhou Keda Technology Co.,Ltd.All right reserved.>
--------------------------------------------------------------------------------
修改记录：
日  期        版本        修改人        走读人        修改记录
2013/08/08	  V1.0        闫日亮						创建
==============================================================================*/
#include "stdafx.h"
#include "clientinstance.h"

CClientInstance::CClientInstance()
{
	//成员变量初始化
	ClientInstanceRelease();						//释放资源
}

CClientInstance::~CClientInstance()
{
	ClientInstanceRelease();						//释放资源
}

/*==========================================================
函数名 DaemonInstanceEntry
功能 ：app守护实例入口
算法实现：<可选项>
参数说明：pMsg:消息
pcApp:该instance所属的app
返回值说明：void
============================================================*/
void CClientInstance::DaemonInstanceEntry(CMessage *const pMsg, CApp* pcApp)
{
	
}

/*==========================================================
函数名 InstanceEntry
功能 ：app实例入口
算法实现：<可选项>
参数说明：pMsg:消息
返回值说明：void
============================================================*/
void CClientInstance::InstanceEntry(CMessage *const pMsg)
{
	//判断消息是否为空
	if ( pMsg == NULL)
	{
		return;
	}

	//屏蔽继续上传消息打印
	if ( pMsg->event != STOC_UPLOADING_ACK)
	{
		OspLog(LOGPRINTLEVELM, "GET MESSAGE NO.%d!\n", pMsg->event);
	}
		
	switch ( pMsg->event )
	{
	case CTOC_CONN_REQ:											//收到连接请求的消息
		{
			ClientConnectToServer();
		}
		break;
	case STOC_CONN_ACK:											//收到连接请求应答的消息
		{
			CToSConnectAckManage(pMsg);
		}
		break;
	case CTOC_CONN_DISC:										//收到断开连接的消息
		{
			CDisconnectManage();
		}
		break;
	case CTOC_UPLOAD_REQ:										//收到上传文件请求的消息
		{
			CUploadFileReqToServer(pMsg);
		}
		break;
	case STOC_UPLOAD_ACK:										//收到上传请求应答的消息
		{
			CUploadToSAckManage();
		}
		break;
	case STOC_UPLOAD_NACK:										//收到上传请求拒绝的消息
		{
			CUploadToSNackManage(pMsg);
		}
		break;
	case STOC_UPLOADING_ACK:									//收到继续上传文件的消息
		{
			CUploadingToSAckManage();
		}
		break;
	case STOC_UPLOADING_NACK:									//收到上传文件在客户端创建失败的消息
		{
			CUploadToSNackManage(pMsg);
		}
		break;
	case CTOC_UPLOADING_PAUSE:                                  //收到上传暂停的消息
		{
			CPauseUploadFileToS();
		}
		break;		
	case CTOC_UPLOADING_RESTART:                                //收到上传重新开始的消息
		{
			CRestartUploadFileToS();
		}
		break;		
	case CTOC_FILE_REMOVE:										//收到暂停删除的消息
		{
			CPauseRemoveFileManage();
		}
		break;
	case CTOS_CONNENT_TIMER:									//超时
		{
			CDisconnectManage();
		}
		break;
	case OSP_OVERFLOW:											//服务器无闲
		{
			CDisconnectManage();
		}
		break;
	case OSP_DISCONNECT:										//断链
		{
			CDisconnectManage();
		}
		break;
	default:
		{
			OspLog(LOGPRINTLEVELH, "GET INSTANCE NONE_SENCE!\n");  
		}
		break;
	}
}

/*==========================================================
函数名 ClientConnectToServer
功能 ：连接到服务器
算法实现：<可选项>
参数说明：无
返回值说明：无
============================================================*/
void CClientInstance::ClientConnectToServer()
{
	//判断状态是否空闲
	if ( CurState() != STATE_IDLE )
	{
		CB_CToSProgress(C_SEND_FILE_TO_S_COEENCT);					//状态显示已连接
		OspLog(LOGPRINTLEVELH, "ClientConnectToServer未断开结点连接!\n"); 
		return;
	}

	if ( !OspIsValidTcpNode(g_nServerNode) )							//判断一个结点是否还有效
	{
		NextState(STATE_IDLE);
		OspLog(LOGPRINTLEVELH, "GetAppID.%d GetInsID.%d\n", GetAppID(), GetInsID());
		return;
	}

	OspNodeDiscCBRegQ(g_nServerNode, GetAppID(), GetInsID());			//设置断链检查
	NextState(STATE_CONNECT);										//连接结点成功，状态置为STATE_CONNECT
	//向服务器发送连接请求的消息，让服务器查找空闲Instance应答
	OspPost( MAKEIID(SERVER_APP_ID, CInstance::PENDING), CTOS_CONN_REQ, NULL, 0,
		 g_nServerNode, MAKEIID( CLIENT_APP_ID, 1 ), SERVER_APP_ID);
	SetTimer(CTOS_CONNENT_TIMER, 5*1000); 										//进行定时操作5s超时
}

/*==========================================================
函数名 CToSDisconnectManage
功能 ：与服务器断开时的处理函数
算法实现：<可选项>
参数说明：无
返回值说明：void
============================================================*/
void CClientInstance::CDisconnectManage()
{
	NextState(STATE_IDLE);
	OspDisconnectTcpNode(g_nServerNode); 									//断开结点连接
	ClientInstanceRelease();											//释放资源
	g_nServerNode = INVALID_NODE;
	KillTimer(CTOS_CONNENT_TIMER); 
	CB_CToSProgress(C_SEND_FILE_TO_S_DICCOEENCT);						//状态显示已经断开连接
	OspLog(LOGPRINTLEVELH, "CToSDisconnectManage断开结点连接!\n");
}

/*==========================================================
函数名 CToSConnectAckManage
功能 ：收到连接应答时的处理
算法实现：<可选项>
参数说明：pMsg:文件信息
返回值说明：void
============================================================*/
void CClientInstance::CToSConnectAckManage(CMessage *const pMsg)
{
	//判断消息是否为空
	if ( pMsg == NULL)
	{
		ClientInstanceRelease();								//释放资源
		NextState(STATE_CONNECT);
		CB_CToSProgress(C_SEND_FILE_TO_S_NACKCOEENCT);			//状态显示连接失败
		return;
	}
	KillTimer(CTOS_CONNENT_TIMER); 
	m_nServerID = pMsg->srcid;                                  // 请求返回时，保存服务器ID号 
	CB_CToSProgress(C_SEND_FILE_TO_S_COEENCT);					//状态显示建立连接
	NextState(STATE_READY);
	OspLog(LOGPRINTLEVELH, "CToSConnectAckManage收到连接应答!\n");
}

/*==========================================================
函数名 CUploadFileReqToServer
功能 ：向服务器端传送文件
算法实现：<可选项>
参数说明：pMsg:文件信息
返回值说明：void
============================================================*/
void CClientInstance::CUploadFileReqToServer(CMessage *const pMsg)
{
	//判断消息是否为空
	if ( pMsg == NULL)
	{
		ClientInstanceRelease();					//释放资源
		NextState(STATE_READY);
		CB_CToSProgress(C_SEND_FILE_TO_S_FAIL);	
		OspLog(LOGPRINTLEVELH, "CUploadFileReqToServer:pMsg == NULL!\n"); 
		return;
	}
	//状态判断
	if ( CurState() != STATE_READY )
	{
		ClientInstanceRelease();					//释放资源
		CB_CToSProgress(C_SEND_FILE_TO_S_FAIL);		//上传失败
		OspLog(LOGPRINTLEVELH, "CUploadFileReqToServer:STATE ERROR.%d!\n",CurState());
		NextState(STATE_READY);		
		return;
	}
	memset(&m_tCFileInfo, 0, sizeof(CFileInfo));
	memcpy(&m_tCFileInfo, pMsg->content, pMsg->length);

	//m_ulFileSize = m_tCFileInfo.dwFileSize;			//保存文件大小为暂停上传做判断
	//计算需要发的总包数
	m_uPacketCount = m_tCFileInfo.dwFileSize / UPLOAD_PAGESIZE;
	m_uLastPacketSize = m_tCFileInfo.dwFileSize % UPLOAD_PAGESIZE;
	if ( m_uLastPacketSize > 0 )
	{
		m_uPacketCount++;								//如果包大小非整数倍，总包数量加1		
	}
	//向服务器发送上传请求信息
	post( m_nServerID, CTOS_UPLOAD_REQ, &m_tCFileInfo, sizeof(m_tCFileInfo), g_nServerNode);
	NextState(STATE_BUSY);	
}

/*==========================================================
函数名 CUploadToSAckManage
功能 ：收到上传文件应答后的处理函数
算法实现：<可选项>
参数说明：无
返回值说明：void
============================================================*/
void CClientInstance::CUploadToSAckManage()
{	
	//判断是否为上传状态
	if ( (CurState() != STATE_BUSY) )
	{
		OspLog(LOGPRINTLEVELH, "CUploadToSAckManage:STATE ERROR.%d!\n",CurState());
		CReadFileErrManage();									//上传文件时失败处理		
		return;
	}
	CUploadFileToS();											//发送文件
}

/*==========================================================
函数名 CUploadFileToS
功能 ：上传文件函数
算法实现：<可选项>
参数说明：无
返回值说明：void
============================================================*/
void CClientInstance::CUploadFileToS()
{
	CB_CToSProgress(C_SEND_FILE_TO_S_UPLOADING);				//状态显示上传文件中
	m_pFile = fopen(m_tCFileInfo.achFileName, "rb+");				
	if ( m_pFile == NULL)
	{
		OspLog(LOGPRINTLEVELH, "CUploadFileToS：m_pFile == NULL!\n");		
		CReadFileErrManage();									//上传文件时失败处理
		return;
	}
	//判断fseek是否成功
	if ( fseek(m_pFile, m_uCount * UPLOAD_PAGESIZE, SEEK_SET) != 0 )
	{
		OspLog(LOGPRINTLEVELH, "CUploadFileToS：fseek error!\n");
		CReadFileErrManage();									//上传文件时失败处理
		return;
	}
	//判断fread是否成功
	if ( fread(m_tUploadFileInfo.achData, 1, UPLOAD_PAGESIZE, m_pFile) == 0 )//读取一数据包到缓存
	{
		OspLog(LOGPRINTLEVELH, "CUploadFileToS：fread error!\n");
		CReadFileErrManage();									//上传文件时失败处理
		return;
	}
	fclose(m_pFile);
	m_uCount++;

	static s8 chProgress = 0;									//记录上传进度，降低打印频率时用
	//判断上传的数据包总大小是否超过文件大小
	if ( m_uCount != m_uPacketCount )
	{
		//记录上传进度
		m_dProgress = m_uCount * 100 / m_uPacketCount;		
		//降低打印频率
		if ( chProgress != (s8)m_dProgress )
		{
			OspLog(LOGPRINTLEVELL, "已发送: %d%%\n", chProgress);
			chProgress = (s8)m_dProgress;
		}
		m_tUploadFileInfo.nDataLen = UPLOAD_PAGESIZE;			//记录上传数据上传
		CB_CToSProgress(chProgress);							//状态显示进度
		//向服务器发送一个数据包并请求继续上传
		post(m_nServerID, CTOS_UPLOADING_REQ, &m_tUploadFileInfo, sizeof(UploadFileInfo), g_nServerNode);
	}
	else
	{
		//计算最后一个包大小
		m_tUploadFileInfo.nDataLen = m_uLastPacketSize;		
		//向服务器发送一个数据包
		post(m_nServerID, CTOS_UPLOADING_REQ, &m_tUploadFileInfo, sizeof(UploadFileInfo), g_nServerNode);
		CB_CToSProgress(C_SEND_FILE_TO_S_OVER);					//状态显示已发送完毕
		ClientInstanceRelease();								//释放资源
		chProgress = 0;
		NextState(STATE_READY);
		OspLog(LOGPRINTLEVELH,"已发送完毕: 100%%!\n");
	}//if ( m_uCount != m_uPacketCount )
}

/*==========================================================
函数名 CUploadingToSAckManage
功能 ：继续上传未完成的文件请求被接受后的处理函数
算法实现：<可选项>
参数说明：无
返回值说明：void
============================================================*/
void CClientInstance::CUploadingToSAckManage()
{
	if ( CurState() == STATE_PAUSE )	//暂停状态
	{
		OspLog(LOGPRINTLEVELH,"CUploadingToSAckManage:STATE_PAUSE!\n");
		return;
	}

	CUploadToSAckManage();				//继续上传								
}

/*==========================================================
函数名 CUploadToSNackManage
功能 ：上传请求被拒绝的处理
算法实现：<可选项>
参数说明：pMsg:文件信息
返回值说明：void
============================================================*/
void CClientInstance::CUploadToSNackManage(CMessage *const pMsg)
{
	s8 eErrCode;
	memcpy(&eErrCode, pMsg->content, pMsg->length);
	CReadFileErrManage();									//上传文件时失败处理
	OspLog(LOGPRINTLEVELH, "CUploadToSNackManage：eErrCode.%d!\n", eErrCode);
}

/*==========================================================
函数名 CReadFileErrManage
功能 ：上传文件读错误处理
算法实现：<可选项>
参数说明：无
返回值说明：void
============================================================*/
void CClientInstance::CReadFileErrManage()
{
	//向服务器发送读文件失败信息
	post( m_nServerID, CTOS_UPLOAD_FREADERR, NULL, 0, g_nServerNode);
	ClientInstanceRelease();								//释放资源
	NextState(STATE_READY);
	CB_CToSProgress(C_SEND_FILE_TO_S_FAIL);					//状态显示上传失败
}

/*==========================================================
函数名 CPauseUploadFileToS
功能 ：暂停上传文件的处理函数
算法实现：<可选项>
参数说明：无
返回值说明：void
============================================================*/
void CClientInstance::CPauseUploadFileToS()
{
	//判断是否已经暂停
	if ( CurState() == STATE_PAUSE )
	{
		OspLog(LOGPRINTLEVELH,"CPauseUploadFileToS:STATE_PAUSE!\n");
		CB_CToSProgress(C_SEND_FILE_TO_S_PAUSE);			//暂停中
		return;
	}

	//判断是否上传状态
	if ( CurState() != STATE_BUSY )
	{
		OspLog(LOGPRINTLEVELH, "CPauseUploadFileToS:STATE ERROR.%d!\n", CurState());
		CReadFileErrManage();		
		return;
	}

	//判断是否满足暂停条件
	if ( (m_tCFileInfo.dwFileSize / 1024 / 1024) < PAUSESIZE )
	{
		CB_CToSProgress(C_SEND_FILE_TO_S_NOPAUSE);			//发送文件小于50M，不允许暂停
		OspLog(LOGPRINTLEVELH, "上传文件小于%dM,不允许暂停!\n",PAUSESIZE); 
		return;
	}

	CB_CToSProgress(C_SEND_FILE_TO_S_PAUSE);				//状态显示暂停中
	NextState(STATE_PAUSE);
	OspLog(LOGPRINTLEVELH,"注意上传已被暂停!\n");
}

/*==========================================================
函数名 CPauseRemoveFileManage
功能 ：暂停上传时删除文件的错误处理
算法实现：<可选项>
参数说明：无
返回值说明：void
============================================================*/
void CClientInstance::CPauseRemoveFileManage()
{
	//上传状态不允许删除列表文件
	if ( CurState() == STATE_BUSY )
	{
		CB_CToSProgress(C_SEND_FILE_TO_S_BUSYNODEL);		
		OspLog(LOGPRINTLEVELH, "上传状态,不能删除!\n"); 
		return;
	}

	//非暂停状态，可以删除
	if(CurState() != STATE_PAUSE)
	{
		OspLog(LOGPRINTLEVELH, "非暂停状态,可放心删除!\n"); 
		return;
	}

	//向服务器发送暂停时已删除列表文件的消息
	post( m_nServerID, CTOS_UPLOAD_FREADERR, NULL, 0, g_nServerNode);
	ClientInstanceRelease();								//释放资源
	NextState(STATE_READY);
	OspLog(LOGPRINTLEVELH, "上传文件已经删除!\n");
}

/*==========================================================
函数名 CRestartUploadFileToS
功能 ：暂停后重新上传文件的处理函数
算法实现：<可选项>
参数说明：无
返回值说明：void
============================================================*/
void CClientInstance::CRestartUploadFileToS()
{
	//判断是否暂停状态
	if ( CurState() != STATE_PAUSE )
	{
		OspLog(LOGPRINTLEVELH, "非暂停状态，动作无效STATE ERROR.%d!\n", CurState()); 
		return;
	}

	//切换状态为上传状态
	NextState(STATE_BUSY);
	OspLog(LOGPRINTLEVELH, "文件上传已经恢复!\n"); 
	//继续发送文件
	CUploadFileToS();
}

/*==========================================================
函数名 ClientInstanceRelease
功能 ：释放资源
算法实现：<可选项>
参数说明：无
返回值说明：void
============================================================*/
void CClientInstance::ClientInstanceRelease()
{
	//初始化成员
	m_pFile			= NULL;													
	m_dProgress		= 0;
	m_uCount		= 0;								
	m_uPacketCount	= 0;							
	m_uLastPacketSize=0;
	ZeroMemory(&m_tCFileInfo, sizeof(CFileInfo));	
	ZeroMemory(&m_tUploadFileInfo, sizeof(UploadFileInfo));				
}

//end



