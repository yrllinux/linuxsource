/*==============================================================================
模块名  ：服务器实例
文件名  ：serverinstance.cpp 
相关文件：serverospapp.h,stdafx.h
实现功能：实现Instance通信功能
作者    ：闫日亮
版权    ：<Copyright(c) 2013-2013 Suzhou Keda Technology Co.,Ltd.All right reserved.>
--------------------------------------------------------------------------------
修改记录：
日  期        版本        修改人        走读人        修改记录
2013/08/08	  V1.0        闫日亮						创建
==============================================================================*/

#include "stdafx.h"
#include "serverospapp.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern s8 g_achServerBase[MAX_NAME_LEN];			//记录文件存放的目录

#ifdef __cplusplus
}
#endif  /* __cplusplus */

/*==========================================================
函数名 CServerInstance
功能 ：CServerInstance的构造函数，初始化成员
算法实现：<可选项>
参数说明：无
返回值说明：无
============================================================*/
CServerInstance::CServerInstance()						
{
	//成员变量初始化																
	ServerInstanceRelease();
	m_nSaveClientNode   = 0;
	m_nSaveClientID		= 0;
	m_dwServerSpace		= 0;
}

/*==========================================================
函数名 ~CServerInstance
功能 ：CServerInstance的析构函数，释放资源
算法实现：<可选项>
参数说明：无
返回值说明：无
============================================================*/
CServerInstance::~CServerInstance()
{
	ServerInstanceRelease();					//释放资源
	m_nSaveClientNode   = 0;
	m_nSaveClientID		= 0;
}

/*==========================================================
函数名 DaemonInstanceEntry
功能 ：app命令收发主入口，收命令，并做相应处理
算法实现：<可选项>
参数说明：pMsg:消息
		  pcApp:该instance所属的app
返回值说明：void
============================================================*/
void CServerInstance::DaemonInstanceEntry(CMessage *const pMsg,CApp* pcApp)
{

}

/*==========================================================
函数名 DaemonInstanceEntry
功能 ：app命令收发主入口，收命令，并做相应处理
算法实现：<可选项>
参数说明：pMsg:消息
返回值说明：void
============================================================*/
void CServerInstance::InstanceEntry(CMessage *const pMsg)
{
	//判断消息是否为空
	if ( pMsg == NULL )
	{
		return;
	}

	//屏蔽继续上传消息打印
	if ( pMsg->event != CTOS_UPLOADING_REQ)
	{
		OspLog(LOGPRINTLEVELM, "GET MESSAGE NO.%d!\n", pMsg->event);
	}

	switch ( pMsg->event )
	{
	case CTOS_CONN_REQ:								//连接请求信息
		{
			CConnectSReqManage(pMsg);
		}
		break;
	case CTOS_UPLOAD_REQ:							//上传请求信息						
		{
			CUploadtoSReqManage(pMsg);
		}
		break;
	case CTOS_UPLOAD_FREADERR:						//上传文件时异常请求信息						
		{
			CUploadtoSErrManage();
		}
		break;
	case CTOS_UPLOADING_REQ:						//继续上传文件请求信息
		{
			CUploadingtoSReqManage(pMsg);
		}
		break;
	case OSP_DISCONNECT:							//断链
		{
			DisConnectedCtoSManage(m_nSaveClientNode);
		}
		break;
	case S_WAIT_UPLOAD_TIMER:						//连接到上传的定时
		{
			DisConnectedCtoSManage(m_nSaveClientNode);
		}
		break;
	case S_WAIT_UPLOAD_AGAIN_TIMER:					//上传完成到再次上传的定时
		{
			DisConnectedCtoSManage(m_nSaveClientNode);
		}
		break;
	default :
		{
			OspLog(LOGPRINTLEVELH,"GET INSTANCE NONE_SENCE!\n");  
		}
		break;
	}	
}

/*==========================================================
函数名 CConnectSReqManage
功能 ：收到连接要求时的处理函数
算法实现：<可选项>
参数说明：pMsg:消息 
返回值说明：void
============================================================*/
void CServerInstance::CConnectSReqManage(CMessage *const pMsg)
{
	//检查消息是否为空
	if ( pMsg == NULL)
	{
		NextState(STATE_IDLE);
		return;
	}

	m_nSaveClientNode = pMsg->srcnode;                              // 请求连接时，保存客户端NODE号
	m_nSaveClientID = pMsg->srcid;                                  // 请求连接时，保存客户端ID号 

	OspNodeDiscCBRegQ(m_nSaveClientNode, GetAppID(), GetInsID());	//设置断链检查
	//向客户端发送连接应答
	OspPost(m_nSaveClientID, STOC_CONN_ACK, NULL, 0, m_nSaveClientNode,
		MAKEIID(SERVER_APP_ID, GetInsID()), 0);
	NextState(STATE_READY);
	SetTimer(S_WAIT_UPLOAD_TIMER, 3*60*1000); 					//进行定时操作，等待客户端上传3分钟超时
}

/*==========================================================
函数名 DisConnectedCtoSManage
功能 ：超时断开连接
算法实现：<可选项>
参数说明：无
返回值说明：void
============================================================*/
void CServerInstance::DisConnectedCtoSManage(u32 SrcNodeID)
{	
	//断链时查看是否有未接收完的文件，删除之
	if ( strlen(m_achPathFileName) != 0 )
	{
		FILE *pFile = fopen(m_achPathFileName, "rb+");
		fseek(pFile, 0, SEEK_END);
		DWORD dwFileLength = ftell(pFile);
		fclose(pFile);
		if (m_tCGetFileInfo.dwFileSize > dwFileLength)
		{
			//删除接收未完成的文件
			remove(m_achPathFileName);										
		}
	}
	OspDisconnectTcpNode(SrcNodeID);		//断开连接
	ServerInstanceRelease();				//释放资源
	m_nSaveClientNode   = 0;
	m_nSaveClientID		= 0;
	KillTimer(S_WAIT_UPLOAD_TIMER); 							//收到上传请求，取消Timer
	KillTimer(S_WAIT_UPLOAD_AGAIN_TIMER); 						//收到上传请求，取消Timer
	NextState(STATE_IDLE);
	OspLog(LOGPRINTLEVELH, "断开结点连接!\n");
}

/*==========================================================
函数名 CUploadtoSReqManage
功能 ：收到数据上传要求时的处理函数
算法实现：<可选项>
参数说明：pMsg:消息
返回值说明：void
============================================================*/
void CServerInstance::CUploadtoSReqManage(CMessage *const pMsg)
{
	KillTimer(S_WAIT_UPLOAD_TIMER); 							//收到上传请求，取消Timer
	KillTimer(S_WAIT_UPLOAD_AGAIN_TIMER); 						//收到上传请求，取消Timer

	s8 achFileName[MAX_NAME_LEN] = {0};							//临时放置文件名

	//判断是否为空
	if ( pMsg == NULL )
	{
		s8 eErrCode = STOC_FILE_CREATEERR;
		//向客户端发送拒绝上传信息
		post(m_nSaveClientID, STOC_UPLOAD_NACK, &eErrCode, sizeof(s8), m_nSaveClientNode);
		CUploadtoSErrManage();
		OspLog(LOGPRINTLEVELH, "CUploadtoSReqManage：pMsg == NULL!\n"); 
		return;
	}

	//判断当前状态
	if ( CurState() != STATE_READY )
	{
		s8 eErrCode = STOC_FILE_CREATEERR;
		//向客户端发送拒绝上传信息
		post(m_nSaveClientID, STOC_UPLOAD_NACK, &eErrCode, sizeof(s8), m_nSaveClientNode);
		OspLog(LOGPRINTLEVELH, "CUploadtoSReqManage：STATE ERROR.%d!\n", CurState()); 
		CUploadtoSErrManage();
		return;
	}

	memset(&m_tCGetFileInfo, 0, sizeof(CFileInfo));
	memcpy(&m_tCGetFileInfo, pMsg->content, pMsg->length);	
	memcpy(achFileName, m_tCGetFileInfo.achFileName, MAX_NAME_LEN);

	FileNameOpt(achFileName);											//获取上传的文件名
	if ( strlen(achFileName) == 0 )
	{
		s8 eErrCode = STOC_FILE_CREATEERR;
		//向客户端发送拒绝上传信息
		post(m_nSaveClientID, STOC_UPLOAD_NACK, &eErrCode, sizeof(s8), m_nSaveClientNode);
		CUploadtoSErrManage();
		OspLog(LOGPRINTLEVELH, "CUploadtoSReqManage：文件名为空!\n");
		return;
	}

	//组合成存放路径文件名
	memset(m_achPathFileName, 0, MAX_NAME_LEN);
	memcpy(m_achPathFileName, g_achServerBase, MAX_NAME_LEN);
	strcat(m_achPathFileName, "\\");
	strcat(m_achPathFileName, achFileName);
	FileRenameManage(m_achPathFileName);						 		//判断是否文件重名

	//判断空间是否充足
	if ( !IsServerSpaceEnough(m_tCGetFileInfo.dwFileSize) )	
	{
		s8 eErrCode = STOC_SERVER_NOSPACE;
		OspLog(LOGPRINTLEVELH,"CUploadtoSReqManage:空间不足!\n");
		//向客户端发送文件创建失败信息
		post(m_nSaveClientID, STOC_UPLOAD_NACK, &eErrCode, sizeof(s8), m_nSaveClientNode);
		CUploadtoSErrManage();	
		CB_RevProgress(SERVER_NOSPACE);								//空间不足
		return;
	}	
	
	m_phGetFile = fopen(m_achPathFileName, "wb+");						//得到文件句柄,并创建文件
	if ( m_phGetFile == NULL )
	{
		s8 eErrCode = STOC_FILE_CREATEERR;
		OspLog(LOGPRINTLEVELH,"CUploadtoSReqManage:m_phGetFile == NULL!\n");
		post(m_nSaveClientID, STOC_UPLOAD_NACK, &eErrCode, sizeof(s8), m_nSaveClientNode);
		CUploadtoSErrManage();
		return;
	}	

	//向客户端发送上传应答
	post(m_nSaveClientID, STOC_UPLOAD_ACK, &m_tCGetFileInfo, sizeof(CFileInfo), m_nSaveClientNode);
	fclose(m_phGetFile);
	m_phGetFile = NULL;
	NextState(STATE_BUSY);													
}

/*==========================================================
函数名 CUploadingtoSReqManage
功能 ：接收到继续上传未完成的文件要求时的处理函数
算法实现：<可选项>
参数说明：pMsg:消息
返回值说明：void
============================================================*/
void CServerInstance::CUploadingtoSReqManage(CMessage *const pMsg)
{
	//判断是否空
	if ( pMsg == NULL )
	{
		s8 eErrCode = STOC_FILE_WRITEERR;
		//向客户端发送写失败信息
		post(m_nSaveClientID, STOC_UPLOADING_NACK, &eErrCode, sizeof(s8), m_nSaveClientNode);
		CUploadtoSErrManage();
		OspLog(LOGPRINTLEVELH, "CUploadingtoSReqManage:pMsg == NULL!\n"); 
		return;
	}

	if ( (CurState() != STATE_BUSY) )
	{
		s8 eErrCode = STOC_FILE_WRITEERR;
		//向客户端发送写失败信息
		post(m_nSaveClientID, STOC_UPLOADING_NACK, &eErrCode, sizeof(s8), m_nSaveClientNode);
		OspLog(LOGPRINTLEVELH, "CUploadingtoSReqManage:STATE ERROR.%d!\n", CurState());
		CUploadtoSErrManage();
		return;
	}

	memset(&m_tGetFileInfo, 0 , sizeof(UploadFileInfo));
	memcpy(&m_tGetFileInfo, pMsg->content, pMsg->length);

	m_phGetFile = fopen(m_achPathFileName, "ab+");					//得到文件句柄,并在文件末尾追加数据
	if ( m_phGetFile == NULL )
	{
		s8 eErrCode = STOC_FILE_WRITEERR;
		//向客户端发送写失败信息
		post(m_nSaveClientID, STOC_UPLOADING_NACK, &eErrCode, sizeof(s8), m_nSaveClientNode);
		CUploadtoSErrManage();
		OspLog(LOGPRINTLEVELH, "CUploadingtoSReqManage:m_phGetFile == NULL!\n"); 
		return;
	}

	if ( fwrite(m_tGetFileInfo.achData, 1, m_tGetFileInfo.nDataLen, m_phGetFile) == 0 )	//写数据到文件
	{
		s8 eErrCode = STOC_FILE_WRITEERR;
		post(m_nSaveClientID, STOC_UPLOADING_NACK, &eErrCode, sizeof(s8), m_nSaveClientNode);
		CUploadtoSErrManage();
		OspLog(LOGPRINTLEVELH, "CUploadingtoSReqManage:fwrite error!\n"); 
		return;
	}
	fflush(m_phGetFile); 
	fseek(m_phGetFile, 0, SEEK_END);
	DWORD dwFileLength = ftell(m_phGetFile);
	fclose(m_phGetFile);//关闭文件句柄

	//判断当前是否已经写完
	if ( dwFileLength == m_tCGetFileInfo.dwFileSize )
	{
		SetTimer(S_WAIT_UPLOAD_AGAIN_TIMER, 3*60*1000); 				//等待下次上传3分钟超时
		ServerInstanceRelease();										//释放资源
		NextState(STATE_READY);
		OspLog(LOGPRINTLEVELH, "CUploadingtoSReqManage:write done!\n"); 
		return;
	}

	//向客户端发送继续上传的信息
	post(m_nSaveClientID, STOC_UPLOADING_ACK, NULL, 0, m_nSaveClientNode);			
}

/*==========================================================
函数名 CUploadtoSErrManage
功能 ：文件上传过程中，客户端读错误的处理函数
算法实现：<可选项>
参数说明：无
返回值说明：void
============================================================*/
void CServerInstance::CUploadtoSErrManage()
{
	//查看是否有未接收完的文件，删除之
	if ( strlen(m_achPathFileName) != 0 )
	{
		FILE *pFile = fopen(m_achPathFileName, "rb+");
		fseek(pFile, 0, SEEK_END);
		DWORD dwFileLength = ftell(pFile);
		fclose(pFile);
		if (m_tCGetFileInfo.dwFileSize > dwFileLength)
		{
			//删除接收未完成的文件
			remove(m_achPathFileName);											
		}
	}
	//释放资源
	ServerInstanceRelease();													
	NextState(STATE_READY);
}

/*==========================================================
函数名 FileNameOpt
功能 ：获取上传文件的文件名
算法实现：<可选项>
参数说明：pName：完整的路径（在客户端）和文件名
返回值说明：
============================================================*/
void CServerInstance::FileNameOpt(char *pchName)
{
	//判断是否空
	if ( pchName == NULL )
	{
		return;
	}

	s8 achName[MAX_NAME_LEN] = {0};
	strcpy(achName, pchName);
	//获取路径文件中的文件名
	for ( int i = strlen(achName); i > 0; i-- )
	{
		if ( pchName[i-1] == '\\' )
		{
			strcpy(pchName, &achName[i]);
		}
	}
}

/*==========================================================
函数名 FileRenameManage
功能 ：服务器端获取上传文件时存在同名文件的处理函数
算法实现：文件查找/匹配算法
参数说明：pFileName:文件名称
返回值说明：void
============================================================*/
void CServerInstance::FileRenameManage(s8* pFileName)
{
	s8 achReName[MAX_NAME_LEN] = {0};							//记录文件
	s32 nNum = 1;
	BOOL bfilesame = FALSE;
	strcpy(achReName, pFileName);								//保持源，防止被改变后还有重名
	do 
	{
		//匹配目标目录下的文件名跟接收的文件名
		bfilesame = access(pFileName, 0) == 0;
		if ( bfilesame )
		{
			strcpy(pFileName, achReName);
			s8 cfileback[MAX_NAME_LEN] = {0};					//记录扩展名前面部分
			s8 cfilefront[MAX_NAME_LEN] = {0};					//记录扩展名
			for ( s32 i = strlen(pFileName); i > 0; i-- )
			{
				//如果查到文件名带有后缀，将点前的部分加num跟后缀组合
				if ( pFileName[i-1] == '.' )
				{
					strncpy(cfilefront, pFileName, i-1);
					strncpy(cfileback, &pFileName[i], strlen(pFileName) - i);
					sprintf(pFileName, "%s%d.%s", cfilefront, nNum, cfileback);
					break;
				}

				//如果文件名不带后缀
				if ( i == 1 )
				{
					sprintf(pFileName, "%s%d", pFileName, nNum);
					break;
				}

			}//for()循环结束
			OspLog(LOGPRINTLEVELH, "同名文件改为: %s\n", pFileName);			
			nNum++;//rand()可以尝试
		}
	} while (bfilesame);
	OspLog(LOGPRINTLEVELH,"不存在同名文件: %s\n",pFileName);
}

/*==========================================================
函数名 IsServerSpaceEnough
功能 ：判断磁盘分区上是否有足够空间接收上传文件
算法实现：<可选项>
参数说明: dwFileSize:上传的文件总大小
返回值说明：BOOL：如果空间足够，则返回TRUE；否则返回FALSE
============================================================*/
BOOL CServerInstance::IsServerSpaceEnough(DWORD dwFileSize)
{
	//获取当前磁盘大小
	ULARGE_INTEGER nFreeBytesAvailable;
	GetDiskFreeSpaceEx(_T(g_achServerBase), &nFreeBytesAvailable, 0, 0);
	ULONGLONG dwServerSpace = nFreeBytesAvailable.QuadPart / 1024 / 1024;
	DWORD dwSize = dwFileSize / 1024 / 1024 + 1;	//B转换为M,加1防止溢出

	//判断服务器剩余空间大小
	if ( dwServerSpace > dwSize )
	{
		OspLog(LOGPRINTLEVELH, "空间充足，可以上传，剩余%d M!\n", dwServerSpace);
		return TRUE;
	}

	OspLog(LOGPRINTLEVELH,"空间不足，只剩: %d M!\n",dwServerSpace);
	return FALSE;
}

/*==========================================================
函数名 ServerInstanceRelease
功能 ：释放资源
算法实现：<可选项>
参数说明：无
返回值说明：void
============================================================*/
void CServerInstance::ServerInstanceRelease()
{
	//释放资源		
	m_phGetFile			= NULL;	                           
	ZeroMemory(m_achPathFileName, MAX_NAME_LEN);
	ZeroMemory(&m_tGetFileInfo, sizeof(UploadFileInfo));
	ZeroMemory(&m_tCGetFileInfo, sizeof(CFileInfo));
}

//end


















