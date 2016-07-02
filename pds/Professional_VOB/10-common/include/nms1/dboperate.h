/********************************************************************
模块名:      数据库操作函数库
文件名:      DBOperate.h
相关文件:    
文件实现功能 操作接口实现头文件
作者：       
版本：       4.0
------------------------------------------------------------------------
修改记录:
日		期	版本	修改人	走读人	修改内容
2005/08/22	4.0		任厚平

*********************************************************************/

#if !defined(AFX_DBOPERATE_H__C01764B0_66E9_471B_9931_62802DDD3E83__INCLUDED_)
#define AFX_DBOPERATE_H__C01764B0_66E9_471B_9931_62802DDD3E83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable : 4146)

#import "C:\Program Files\Common Files\System\ADO\msado15.dll"\
	no_namespace \
rename("EOF","ISEOF")


#include <winsock2.h>
#pragma comment(lib, "ws2_32")
#include "kdvtype.h"

//一些宏定义

#define  VOD_OK                 (u16)0    //VOD库操作成功 
#define  VODERR_BASE_CODE		(u16)20000//错误码基准值


//登录时，身份验证失败
#define VODERR_DB_INDENTIFY_METHOD			(u16)(VODERR_BASE_CODE + 1) 
//检索类组信息失败
#define VODERR_DB_LOADCLASSINFO_METHOD		(u16)(VODERR_BASE_CODE + 2) 
//检索类组中的文件信息失败
#define VODERR_DB_LOADFILEINFO_METHOD		(u16)(VODERR_BASE_CODE + 3) 
//检索类类别的权限信息
#define VODERR_DB_LOADPOPINFO_METHOD		(u16)(VODERR_BASE_CODE + 4) 
//更改用户登陆密码
#define VODERR_DB_CHANGEPWD_METHOD			(u16)(VODERR_BASE_CODE + 5) 
//更新组的权限集
#define VODERR_DB_UPDATEPOPS_METHOD			(u16)(VODERR_BASE_CODE + 6) 
//检索日志信息失败
#define VODERR_DB_LOADLOGINFO_METHOD		(u16)(VODERR_BASE_CODE + 7) 
//增加一条点播记录
#define VODERR_DB_ADDONELOG_METHOD			(u16)(VODERR_BASE_CODE + 8) 
//删除一条点播记录
#define VODERR_DB_DELONELOG_METHOD			(u16)(VODERR_BASE_CODE + 9) 
//增加一条文件发布记录
#define VODERR_DB_ADDONEPUB_METHOD			(u16)(VODERR_BASE_CODE + 10)
//更新一条文件发布记录
#define VODERR_DB_UPDATEONEPUB_METHOD		(u16)(VODERR_BASE_CODE + 11) 
//删除一条文件发布记录
#define VODERR_DB_DELONEPUB_METHOD			(u16)(VODERR_BASE_CODE + 12) 
//增加一条文件柜记录
#define VODERR_DB_ADDONECLASS_METHOD		(u16)(VODERR_BASE_CODE + 13)
//更新一条文件柜记录
#define VODERR_DB_UPDATEONECLASS_METHOD		(u16)(VODERR_BASE_CODE + 14) 
//删除一条文件柜记录
#define VODERR_DB_DELONECLASS_METHOD		(u16)(VODERR_BASE_CODE + 15) 
//检索用户信息失败
#define VODERR_DB_LOADUSERINFO_METHOD		(u16)(VODERR_BASE_CODE + 16) 
//增加一条用户记录
#define VODERR_DB_ADDONEUSER_METHOD			(u16)(VODERR_BASE_CODE + 17)
//更新一条用户记录
#define VODERR_DB_UPDATEONEUSER_METHOD		(u16)(VODERR_BASE_CODE + 18) 
//删除一条用户记录
#define VODERR_DB_DELONEUSER_METHOD			(u16)(VODERR_BASE_CODE + 19) 
//更新一条用户的停用、启用记录标记
#define VODERR_DB_SETONEUSERFLAG_METHOD		(u16)(VODERR_BASE_CODE + 20)
//更新一条点播记录
#define VODERR_DB_UPDATEONEPLAYLOG_METHOD   (u16)(VODERR_BASE_CODE + 21)
//查询当前点播人数
#define VODERR_DB_QUERYNUMOFPLAYING_METHOD	(u16)(VODERR_BASE_CODE + 22);


//设置文件发布的数据库操作参数无效
#define VODERR_OPEN_DB_PARAM				(u16)(VODERR_BASE_CODE + 47) 
//打开数据库连接的操作失败
#define VODERR_OPEN_DB						(u16)(VODERR_BASE_CODE + 48) 
//数据库没有进行连接操作，仍处于关闭状态
#define VODERR_DB_CLOSE_STATUS				(u16)(VODERR_BASE_CODE + 49) 


//发布文件操作失败
#define VODERR_DB_INSERT_METHOD				(u16)(VODERR_BASE_CODE + 50) 
//取消发布文件操作失败
#define VODERR_DB_CANCEL_METHOD				(u16)(VODERR_BASE_CODE + 51) 
//查询文件发布状态操作失败
#define VODERR_DB_QUERY_METHOD				(u16)(VODERR_BASE_CODE + 52) 
//更新文件发布状态操作失败
#define VODERR_DB_UPDATE_METHOD				(u16)(VODERR_BASE_CODE + 53)
//更新数据库操作失败
#define VODERR_DB_UPDATEDB_METHOD			(u16)(VODERR_BASE_CODE + 54)

//添加一个直播源失败
#define VODERR_DB_ADDONELIVE_METHOD			(u16)(VODERR_BASE_CODE + 55)
#define VODERR_DB_DELONELIVE_METHOD			(u16)(VODERR_BASE_CODE + 56)
#define VODERR_DB_UPDATEONELIVE_METHOD		(u16)(VODERR_BASE_CODE + 57)



#define MAX_LIST_PAGE_SIZE		10


enum emDbType
{
	emDbTypeMySQL,
	emDbTypeMSSQL,
	emDBTypeNone = 255
};

#define PORT_MYSQL	(u16)3306
#define PORT_MSSQL	(u16)1433

//DBOperate类定义

class CDBOperate  
{
public:
	CDBOperate();
	virtual ~CDBOperate();

public:
	emDbType JudgeDBType(char *IP);
	
	/*=============================================================================
	函 数 名：OpenDB
	功    能：打开数据库
	参    数：szHostIP
			  szUserName
			  szPwd
			  wPort = 1433          
	返 回 值：WORD      : 返回VOD_OK 表示操作成功，
	=============================================================================*/
	virtual u16 OpenDB(CString &szHostIP, CString &szUserName, CString &szPwd, u16 wPort = 3066, emDbType dbType = emDbTypeMySQL );
	/*=============================================================================
	函 数 名：CloseDB
	功    能：关闭数据库
	参    数：无	           
	返 回 值：WORD
	=============================================================================*/
	virtual u16 CloseDB();
	/*=============================================================================
	函 数 名：GetErrorDescript
	功    能： 
	参    数：无	           
	返 回 值：CString
	=============================================================================*/
	 virtual CString GetErrorDescript();
	/*=============================================================================
	函 数 名：PublishOneFile
	功    能：
	参    数：szFilePath
	          szFileName
			  bSecret
	返 回 值：WORD
	=============================================================================*/
	
	virtual u16 PublishOneFile(char *szFilePath, char *szFileName, u32 dwFileSize,
							   char *szPublishTime, BOOL bSecret, u32 dwProgramLen);
	/*=============================================================================
	函 数 名：CancelOnePublishedFile
	功    能：
	参    数：szFilePath
			  szFileName
	返 回 值：WORD
	=============================================================================*/
	
	virtual u16 UpdatePublishOneFile(char *szFilePath, char *szFileName, u32 dwFileSize, u32 dwProgramLen);
	/*=============================================================================
	函 数 名：UpdateOnePublishedFile
	功    能：
	参    数：szFilePath
			  szFileName
	返 回 值：WORD
	=============================================================================*/

	virtual u16 CancelOnePublishedFile(char *szFilePath, char *szFileName);
	/*=============================================================================
	函 数 名：QueryOneFileStauts
	功    能：
	参    数：szFilePath
			  szFileName
			  bPublished
	返 回 值：WORD
	=============================================================================*/
	virtual u16 QueryOneFileStauts(char *szFilePath, char *szFileName, BOOL &bPublished);	 

	/*=============================================================================
	函 数 名：UpdateDataBase
	功    能：增量升级数据库 判断新增的表、字段是否存在，不存在的话，自动创建
	参    数：
	返 回 值：WORD
	修改记录：
	日期			版本		修改人		修改记录
	2012/07/26		4.7			邹俊龙		创建
	=============================================================================*/
	virtual u16 UpdateDataBase();

	/*=============================================================================
	函 数 名：UpdateTBLvod_stream
	功    能：更新vod_stream表
	参    数：
	返 回 值：WORD
	修改记录：
	日期			版本		修改人		修改记录
	2013/01/08		4.7			沈钦		创建
	=============================================================================*/
	virtual u16 UpdateTBLvod_stream();
	/*=============================================================================
	函 数 名：AddOneLiveSource
	功    能：增加一个直播发布源
	参    数：bMulticast true是组播直播  false windowsmedia server上的直播
	返 回 值：WORD
	修改记录：
	日期			版本		修改人		修改记录
	2013/01/08		4.7			沈钦		创建
	=============================================================================*/
	virtual WORD AddOneLiveSource( CString &szStreamSource,CString &szIPAddress, bool bMulticast=true);
	
	/*=============================================================================
	函 数 名：DeleteOneLiveSource
	功    能：删除一个直播源
	参    数：
	返 回 值：WORD
	修改记录：
	日期			版本		修改人		修改记录
	2013/01/08		4.7			沈钦		创建
	=============================================================================*/
	virtual WORD DeleteOneLiveSource(CString strSourceName,CString strIPAddress);
	/*=============================================================================
	函 数 名：UpdateOneLiveSource
	功    能：更新直播源
	参    数：
	返 回 值：WORD
	修改记录：
	日期			版本		修改人		修改记录
	2013/01/08		4.7			沈钦		创建
	=============================================================================*/
	virtual WORD UpdateOneLiveSource(CString strOldStreamName,CString strOldIPAddress,CString strNewSourceName,CString strNewIPAddress);

private:
	bool	InitSocket();

protected:
	_ConnectionPtr m_pCon;		//数据库连接变量

	CString m_szErrorInfo;

	CRITICAL_SECTION  m_csdbLock;

	BOOL m_bWSAInited;

	emDbType m_emDbType;
};

#endif // !defined(AFX_DBOPERATE_H__C01764B0_66E9_471B_9931_62802DDD3E83__INCLUDED_)
