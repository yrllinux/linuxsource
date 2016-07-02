/*****************************************************************************
文件名      : dbinterface.h
相关文件    : kdvtype.h
文件实现功能: 数据库访问接口              
作者        : 郑榕
模块版本    : V1.0  Copyright(C) 2002-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
日  期(y/m/d)      文件版本        创建人      修改内容
2006/07/17          1.0             郑榕         创建
******************************************************************************/

#ifndef _DB_INTERFACE_H
#define _DB_INTERFACE_H

#ifdef _MSC_VER
#include <winsock2.h>	
#include <wtypes.h>	
#endif

#ifndef NULL
#define NULL   0
#endif

#include "kdvtype.h"

//数据库类型宏定义
#define DBTYPE_SYBASE     1   //sybase数据库
#define DBTYPE_MYSQL      2	  //mysql 数据库
#define DBTYPE_ORACLE     3   //oracle数据库

#define MAX_RECORDCOL_NUM     64   //Table最大列数
#define MAX_COLDATA_WIDTH     2048  //数据项最大字节数(二进制大对象image,BLOB)
#define MAX_DB_CONNECTIONS    512  //最大连接数

//语言组定义
#define DB_LANG_ENGLISH    "us_english"
#define DB_LANG_CHINESE    "chinese"
#define DB_LANG_TCHINESE   "tchinese"  //繁体中文

//字符集定义
#define DB_CHARSET_ISO1       "iso_1"
#define DB_CHARSET_CP850      "cp850"
#define DB_CHARSET_EUCGB      "eucgb"
#define DB_CHARSET_GB18030    "gb18030"
#define DB_CHARSET_UTF8       "utf8"
#define DB_CHARSET_BIG5       "big5"   //繁体中文字符集

typedef enum
{
    EOpCode_FindRs, //发现新的记录集
	EOpCode_BindDataErr, //绑定数据失败
    EOpCode_CmdErr,   //有命令执行失败
    EOpCode_Finish, //结果集已处理完毕
    EOpCode_OtherErr  //其它错误
}EOpCode;

typedef enum
{
	ERR_DBCB_CHANNEL_INVALID = 0,  //无效连接
	ERR_DBCB_OPERATION_TIMEOUT,  //操作超时
	ERR_DBCB_UNKNOWN,      //未知错误
	ERR_DBCB_TBNOTFIND = 208,	//找不到对应的表
	ERR_DBCB_DBNOTINUSE = 921,
	ERR_DBCB_OFFLINE = 950,		//数据库不在线
	ERR_DBCB_CHANGEDB = 5701,	//数据库切换消息
	ERR_DBCB_TEMPTABLE_INUSE = 12822	//所要使用的临时表已经存在
}EDBCbErr;

//回调函数，包括服务端回调和客户端回调
typedef void (*DBCbFunc) (u16 wErrNum, LPCSTR lpSrvMsg, void *pContext);

//全局函数，初始化系统，分配上下文结构
//参数：要访问的数据库类型(目前支持sybase和mysql)
//成功返回true,失败返回false
//注意：此函数必须在任何数据库对象被创建之前调用
BOOL32 DBInit(u8 byDBType = DBTYPE_SYBASE);

//全局函数，设置本地语言组，字符集(针对sybase数据库)
//参数：lpLang 语言组, lpCharSet 字符集
//成功返回true，失败返回false
BOOL32 SetDBLocale(LPCSTR lpLang, LPCSTR lpCharSet);

//全局函数，安装CS_Lib消息处理函数，针对所有连接有效(sybase数据库)
//参数：Fuction 处理函数入口
//成功返回true，失败返回false
BOOL32 SetupLibErrFuc(DBCbFunc CbFunc, void *pContext = NULL);

//全局函数，设置操作超时时间
BOOL32 SetDBTimeOut(u8 bySec);

//全局函数，获取Buf的字符描述
LPCSTR DBGetStrFromBuf(const void *pBuf, u16 wBufLen, LPSTR lpStr, u16 wStrInLen);

//全局函数，退出系统
//成功返回true,失败返回false
BOOL32 DBExit();

//设置DBI日志文件存放位置,路径长度不能大于500字节,不设置则默认路径为./errorlog
BOOL32 SetDBILogDir(LPCSTR lpcLogDir);

//错误日志写开关(如需要修改默认日志存放路径,则应先修改日志存放路径,默认状态下不写日志)
BOOL32 SetFileLog(BOOL32 dwSwitch);

class CDBRs;
class CDBHandlerImp;
class CDBHandler
{
// Constructors/Destruction
public:
    CDBHandler();
    ~CDBHandler();

public:
/*====================================================================
函数名      : ConnecttoSrv
功能        : 建立到数据库服务器的连接
输入参数说明: szUsrName 登陆用户名
              szPwd 密码
			  szDBName 数据库名
              szSrvIP 数据库服务器地址(字符串IP)
              wPort 端口
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 ConnecttoSrv(LPCSTR szUsrName,
                      LPCSTR szPwd,
					  LPCSTR szDBName,
                      LPCSTR szSrvIP,
                      u16 wPort = 5000);

/*====================================================================
函数名      : CloseConnection
功能        : 断开到数据库服务器的连接
输入参数说明: 无
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 CloseConnection();

/*====================================================================
函数名      : SetupCtErrFuc(sybase,mysql数据库都有效)
功能        : 安装客户端错误消息处理函数
输入参数说明: Function 函数入口指针
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 SetupCtErrFuc(DBCbFunc CbFunc, void *pContext = NULL);

/*====================================================================
函数名      : SetupSrvErrFuc(sybase数据库有效)
功能        : 安装服务器端错误消息处理函数
输入参数说明: Function 函数入口指针
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 SetupSrvErrFuc(DBCbFunc CbFunc, void *pContext = NULL);

/*====================================================================
函数名      : RemoveCallBack(sybase数据库有效)
功能        : 移除该连接上的回调信息
输入参数说明: 无
返回值说明  : 成功true,失败false
====================================================================*/    
    void RemoveCallBack();
	
/*====================================================================
函数名      : UseDB
功能        : 打开制定数据库
输入参数说明: szDBName目标数据库名
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 UseDB(LPCSTR szDBName);

/*====================================================================
函数名      : ExecSql
功能        : 执行命令类sql语句,多语句执行时最后一条语句末尾不能带封号
输入参数说明: szsql sql字串
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 ExecSql(LPCSTR szsql);

/*====================================================================
函数名      : ExecSql
功能        : 执行有记录集返回的sql语句,多语句执行时最后一条语句末尾不能带封号
输入参数说明: szsql sql字串
              pcRecordSet 返回结果集指针
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 ExecSql(LPCSTR szsql, CDBRs *pcRecordSet);

// /*====================================================================
// 函数名      : OpenCursor
// 功能        : 打开游标操作
// 输入参数说明: lpCursorSql 和游标相关联的查询语句
//               pcRecordSet 游标返回的记录集指针
// 返回值说明  : 成功true,失败false
// ====================================================================*/    
// 	BOOL32 OpenCursor(LPCSTR lpCursorSql, CDBRs *pcRecordSet);
// 
// /*====================================================================
// 函数名      : UpdateCurRow
// 功能        : 更新游标当前行
// 输入参数说明: lpUpdateSql 更新当前行的sql语句
// 			  lpTableName 和游标相关联的表名 
// 返回值说明  : 成功true,失败false
// ====================================================================*/    
// 	BOOL32 UpdateCurRow(LPCSTR lpUpdateSql, LPCSTR lpTableName);
// 
// /*====================================================================
// 函数名      : DelCurRow
// 功能        : 删除游标当前行
// 输入参数说明: lpUpdateSql 更新当前行的sql语句
// 			  lpTableName 和游标相关联的表名
// 			  特别注意: 当游标涉及多个表时，删除将失败
// 返回值说明  : 成功true,失败false
// ====================================================================*/    
// 	BOOL32 DelCurRow(LPCSTR lpTableName);
	
/*====================================================================
函数名      : InsertBuf(sybase数据库有效)
功能        : 插入用户自定义的非结构化数据到数据库表中
输入参数说明: lpszSql 获取页指针的Sql语句
		      pcRecordSet 获取返回指针时使用
			  pBuf  数据块首地址
              wBufLen 数据块长度
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 InsertBuf(LPCSTR lpszSql, CDBRs *pcRecordSet, const void *pBuf, u16 wBufLen);
	
/*====================================================================
函数名      : GoOnProcRes
功能        : 继续处理结果集(适用于处理多个记录集返回)
输入参数说明: 无
返回值说明  : EOpCode_FindRs 发现新的记录集
              EOpCode_Finish 结果集已处理完毕
			  EOpCode_BindDataErr 数据邦定失败
              EOpCode_CmdErr 发现某条命令执行失败
====================================================================*/    
    EOpCode GoOnProcRes(); 

/*====================================================================
函数名      : GetAffectedRowCount
功能        : 获取本次命令作用的记录条数
输入参数说明: 无
返回值说明  : 记录条数
====================================================================*/    
	u32 GetAffectedRowCount();

/*====================================================================
函数名      : GetHandleAddr
功能        : 获取数据库句柄地址
输入参数说明: 无
返回值说明  : 数据库句柄地址
====================================================================*/   
	CDBHandlerImp* GetHandleAddr();
protected:
	CDBHandlerImp *m_pcDBHandlerImp;
};

class CDBRsImp;
class CDBRs
{
// Constructors/Destruction
public:
    CDBRs();
    ~CDBRs();

public:    
/*====================================================================
函数名      : MoveNext
功能        : 将指针指向结果集下一条记录
输入参数说明: 无
返回值说明  : 成功读出当前行返回true,否则返回false
====================================================================*/    
    BOOL32 MoveNext();

/*====================================================================
函数名      : IsEof
功能        : 记录集是否到达末尾
输入参数说明: 无
返回值说明  : 到达末尾返回true,否则false
====================================================================*/    
    BOOL32 IsEof();

/*====================================================================
函数名      : GetFieldCount
功能        : 获取结果集列数
输入参数说明: 无
返回值说明  : 结果集列数
====================================================================*/    
    const u16 GetFieldCount();

/*====================================================================
函数名      : GetFieldValue
功能        : 获取某列值
输入参数说明: IN lpColName 列名
              OUT Value 该列值  
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 GetFieldValue(LPCSTR lpColName, double& Value);

/*====================================================================
函数名      : GetFieldValue
功能        : 获取某列值
输入参数说明: IN lpColName 列名
              OUT Value 该列值  
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 GetFieldValue(LPCSTR lpColName, float& Value);

/*====================================================================
函数名      : GetFieldValue
功能        : 获取某列值
输入参数说明: IN lpColName 列名
              OUT Value 该列值  
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 GetFieldValue(LPCSTR lpColName, s64& Value);

/*====================================================================
函数名      : GetFieldValue
功能        : 获取某列值
输入参数说明: IN lpColName 列名
              OUT Value 该列值  
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 GetFieldValue(LPCSTR lpColName, u64& Value);

/*====================================================================
函数名      : GetFieldValue
功能        : 获取某列值
输入参数说明: IN lpColName 列名
              OUT Value 该列值  
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 GetFieldValue(LPCSTR lpColName, s32& Value);

/*====================================================================
函数名      : GetFieldValue
功能        : 获取某列值
输入参数说明: IN lpColName 列名
              OUT Value 该列值  
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 GetFieldValue(LPCSTR lpColName, u32& Value);

/*====================================================================
函数名      : GetFieldValue
功能        : 获取某列值
输入参数说明: IN lpColName 列名
              OUT Value 该列值  
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 GetFieldValue(LPCSTR lpColName, s16& Value);

/*====================================================================
函数名      : GetFieldValue
功能        : 获取某列值
输入参数说明: IN lpColName 列名
              OUT Value 该列值  
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 GetFieldValue(LPCSTR lpColName, u16& Value);
	
/*====================================================================
函数名      : GetFieldValue
功能        : 获取某列值
输入参数说明: IN lpColName 列名
              OUT Value 该列值  
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 GetFieldValue(LPCSTR lpColName, s8& Value);

/*====================================================================
函数名      : GetFieldValue
功能        : 获取某列值
输入参数说明: IN lpColName 列名
              OUT Value 该列值  
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 GetFieldValue(LPCSTR lpColName, u8& Value);

/*====================================================================
函数名      : GetFieldValue
功能        : 获取某列值
输入参数说明: IN lpColName 列名
              IN wBufLen 缓存大小
              OUT pstr 该列字串值
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 GetFieldValue(LPCSTR lpColName, LPSTR pstr, u16 wBufLen);

/*====================================================================
函数名      : GetFieldValue
功能        : 获取某列值(用户自定义数据块)
输入参数说明: IN lpColName 列名
              IN wBufLen 缓存大小
              OUT pBuf 缓存首地址
			  OUT wOutLen 实际取出数据块的大小
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 GetFieldValue(LPCSTR lpColName, void *pBuf, u16 wBufLen, u16 &wOutLen);

/*====================================================================
函数名      : GetFieldValue
功能        : 获取某列值
输入参数说明: IN wIndex 列索引
              OUT Value 该列值  
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 GetFieldValue(u8 byIndex, double& Value);

/*====================================================================
函数名      : GetFieldValue
功能        : 获取某列值
输入参数说明: IN wIndex 列索引
              OUT Value 该列值  
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 GetFieldValue(u8 byIndex, float& Value);

/*====================================================================
函数名      : GetFieldValue
功能        : 获取某列值
输入参数说明: IN wIndex 列索引
              OUT Value 该列值  
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 GetFieldValue(u8 byIndex, s64& Value);

/*====================================================================
函数名      : GetFieldValue
功能        : 获取某列值
输入参数说明: IN wIndex 列索引
              OUT Value 该列值  
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 GetFieldValue(u8 byIndex, u64& Value);

/*====================================================================
函数名      : GetFieldValue
功能        : 获取某列值
输入参数说明: IN wIndex 列索引
              OUT Value 该列值  
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 GetFieldValue(u8 byIndex, s32& Value);

/*====================================================================
函数名      : GetFieldValue
功能        : 获取某列值
输入参数说明: IN wIndex 列索引
              OUT Value 该列值  
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 GetFieldValue(u8 byIndex, u32& Value);

/*====================================================================
函数名      : GetFieldValue
功能        : 获取某列值
输入参数说明: IN wIndex 列索引
              OUT Value 该列值  
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 GetFieldValue(u8 byIndex, s16& Value);

/*====================================================================
函数名      : GetFieldValue
功能        : 获取某列值
输入参数说明: IN wIndex 列索引
              OUT Value 该列值  
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 GetFieldValue(u8 byIndex, u16& Value);

/*====================================================================
函数名      : GetFieldValue
功能        : 获取某列值
输入参数说明: IN wIndex 列索引
              OUT Value 该列值  
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 GetFieldValue(u8 byIndex, s8& Value);

/*====================================================================
函数名      : GetFieldValue
功能        : 获取某列值
输入参数说明: IN wIndex 列索引
              OUT Value 该列值  
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 GetFieldValue(u8 byIndex, u8& Value);

/*====================================================================
函数名      : GetFieldValue
功能        : 获取某列值
输入参数说明: IN wIndex 列索引
              IN wBufLen 缓存大小
              OUT pstr 该列字串值
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 GetFieldValue(u8 byIndex, LPSTR pstr, u16 wBufLen);

/*====================================================================
函数名      : GetFieldValue
功能        : 获取某列值(用户自定义数据块)
输入参数说明: IN wIndex 列索引
              IN wBufLen 缓存大小
              OUT pBuf 缓存首地址
			  OUT wOutLen 实际取出数据块的大小
返回值说明  : 成功true,失败false
====================================================================*/    
    BOOL32 GetFieldValue(u8 byIndex, void *pBuf, u16 wBufLen, u16 &wOutLen);

	CDBRsImp* GetRsImp();

protected:
	CDBRsImp *m_pcDBRsImp;
};

#endif
