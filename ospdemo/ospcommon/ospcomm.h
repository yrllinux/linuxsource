/*****************************************************************************
   模块名      : OSPCOMM 
   文件名      : ospcomm.h 
   文件实现功能: 通信消息宏定义
   版本        : V3.0  Copyright(C) 2001-2002 KDC, All rights reserved.
******************************************************************************/
#ifndef _OSPCOMM_H_
#define _OSPCOMM_H_

#include "osp.h"
#include "io.h"

//定义默认值
#define	  PAUSESIZE				  50						//上传暂停需要文件大于50M
#define	  BASEDOCUMENT			  "D:\\tmp"					//服务器默认根目录
#define	  SERVERIP				  "127.0.0.1"				//服务器默认IP
#define	  SERVERNODE			  "6682"					//服务器默认NODE
#define   LOGPRINTLEVELH		  1							//设置控制log打印级别最高
#define   LOGPRINTLEVELM		  2							//设置控制log打印级别居中
#define   LOGPRINTLEVELL		  3							//设置控制log打印级别全部

//定义常用的参数
const u16 CLIENT_APP_ID			= 100;						//客户端APP号
const u16 SERVER_APP_ID			= 150;						//服务器端APP号
const u16 APP_PRIO				= 100;						//服务器和客户端APP优先级
const u32 UPLOAD_PAGESIZE		= 10240;					//文件传输每包字节数
const u32 SERVER_MAX_INS		= 20;						//服务端最大例数 
const u16 SERVER_LISTEN_PORT	= 6682;					    //服务端监听端口
const long RETRY_TIME			= 3 * 1000;					//重新连接服务器间隔时间
const u16 CHECK_PERIOD			= 10;						//链路检测周期
const u8 PING_NUM				= 3;						//ping次数
const s8  CLIENT_PRI			= 100;						// 客户端优先级
const u16 MAX_NAME_LEN			= 260;						//文件名最大长度

/**********************************************************************************
							客户端跟服务器之间的消息定义
**********************************************************************************/
const u16 USR_EV_BASE			= (OSP_USEREVENT_BASE + 20);//用户事件的起始地址定义
//(客户端到客户端)   (消息体为空)
const u16 CTOC_CONN_REQ			= (USR_EV_BASE + 1);		//请求连接信息
//(客户端到服务端)   (消息体为空)					
const u16 CTOS_CONN_REQ			= (USR_EV_BASE + 2);		//请求连接信息	
//(服务端到客户端)   (消息体为空)				
const u16 STOC_CONN_ACK			= (USR_EV_BASE + 3);		//接受连接请求	
//(服务端到客户端)	 该消息未使用		
const u16 STOC_CONN_NACK		= (USR_EV_BASE + 4);		//拒绝连接请求	
//(客户端到客户端)   (消息体为空)				
const u16 CTOC_CONN_DISC		= (USR_EV_BASE + 5);		//断开连接请求					

//(客户端到客户端)   (消息体为CFileInfo)
const u16 CTOC_UPLOAD_REQ		= (USR_EV_BASE + 6);		//请求上传文件
//(客户端到服务端)   (消息体为CFileInfo)					
const u16 CTOS_UPLOAD_REQ		= (USR_EV_BASE + 7);		//请求上传文件	
//(服务端到客户端)   (消息体为空)				
const u16 STOC_UPLOAD_ACK		= (USR_EV_BASE + 8);		//接受上传文件请求				
//(服务端到客户端)   (消息体为eErrCode)
const u16 STOC_UPLOAD_NACK		= (USR_EV_BASE + 9);		//拒绝上传文件请求	
//(客户端到服务端)   (消息体为空)			
const u16 CTOS_UPLOAD_FREADERR	= (USR_EV_BASE + 10);		//上传过程读文件错误			
//(客户端到服务端)   (消息体为UploadFileInfo)
const u16 CTOS_UPLOADING_REQ	= (USR_EV_BASE + 11);		//继续上传未完成的文件请求	
//(服务端到客户端)   (消息体为空)	
const u16 STOC_UPLOADING_ACK	= (USR_EV_BASE + 12);		//接受继续上传未完成的文件请求	
//(客户端到服务端)   该消息未使用
const u16 CTOS_UPLOADING_OVER	= (USR_EV_BASE + 13);		//文件上传完毕					
//(客户端到客户端)   (消息体为空)
const u16 CTOC_UPLOADING_PAUSE	= (USR_EV_BASE + 14);		//文件上传暂停		
//(客户端到客户端)   (消息体为空)			
const u16 CTOC_UPLOADING_RESTART= (USR_EV_BASE + 15);		//文件上传暂停后开始			
//(服务端到客户端)   (消息体为eErrCode)
const u16 STOC_UPLOADING_NACK	= (USR_EV_BASE + 16);		//服务器端创建文件错误信息		
//(客户端到客户端)	 (消息体为空)
const u16 CTOC_FILE_REMOVE		= (USR_EV_BASE + 17);		//文件上传暂停时删除			
//(客户端到客户端)   该消息未使用
const u16 CTOC_SERVER_IP_PROT	= (USR_EV_BASE + 18);		//客户端设置的服务器IP跟端口信息
//(客户端到客户端)	 (消息体为空)
const u16 CTOS_CONNENT_TIMER	= (USR_EV_BASE + 19);		//客户端请求连接时的超时定时器	
//(服务器到服务器)	 (消息体为空)
const u16 S_WAIT_UPLOAD_TIMER	= (USR_EV_BASE + 20);		//服务器收到连接请求后到收到上传请求间的超时定时器
//(服务器到服务器)	 (消息体为空)	
const u16 S_WAIT_UPLOAD_AGAIN_TIMER	= (USR_EV_BASE + 21);	//服务器接收完成到在收到上传请求的超时定时器		

typedef enum ErrCode
{
	STOC_FILE_CREATEERR = 1,						//服务器创建文件错误
	STOC_FILE_WRITEERR,								//服务器写文件错误
	STOC_SERVER_NOSPACE								//服务器空间不足
}ErrCode;

//文件信息结构体,记录文件名跟大小
typedef struct CFileInfo
{
	s8			achFileName[MAX_PATH];				//文件名
	DWORD		dwFileSize;							//文件大小
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
CFileInfo,*pCFileInfo;

//文件信息结构体,记录上传文件每包数据跟长度
typedef struct UploadFileInfo
{
	s8			achData[UPLOAD_PAGESIZE];			//文件数据
	s32			nDataLen;							//有效数据长度
}
#ifndef WIN32
__attribute__ ( (packed) )
#endif
UploadFileInfo,*pUploadFileInfo;

#endif  /* _OSPCOMM_H_ */

/* end of file ospcomm.h */