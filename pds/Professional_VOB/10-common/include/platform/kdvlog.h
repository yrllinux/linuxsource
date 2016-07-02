/*****************************************************************************
   模块名      : kdvlog
   文件名      : kdvlog.h
   相关文件    : 
   文件实现功能: kdvlog.lib对外头文件
   作者        : 刘旭
   版本        : V4.6  Copyright(C) 2001-2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2011/04/5   4.6          刘旭        创建
******************************************************************************/


#ifndef KDV_LOG_LIB_H
#define KDV_LOG_LIB_H

#include "kdvtype.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef DllExport
#ifdef WIN32
#define DllExport __declspec(dllexport)
#else     /* VxWorks*/
#define DllExport
#endif
#endif

// 常量定义
#define		LOG_DEF_USER_NAME				(s8*)"KdvLog"		// 默认的用户名
#define		LOG_MAX_LENGTH_NAME				(u8)108				// 用户名的最大长度
#define		LOG_MAX_LENGTH_PATH				(u8)128				// 文件完整路径的最大长度			
#define		LOG_MSG_MAX_LENGTH				(u8)255				// 每条log消息的最大长度

// 默认的模块名索引
#define		LOG_DEF_MODULE_INDEX			(u16)0xFFFF			// 默认log中不输出模块名

// log等级定义
#define		LOG_LVL_ERROR					(u8)1				// 程序运行错误(逻辑或业务出错),必须输出
#define		LOG_LVL_WARNING					(u8)2				// 告警信息, 可能正确, 也可能错误
#define		LOG_LVL_KEYSTATUS				(u8)3				// 程序运行到一个关键状态时的信息输出
#define		LOG_LVL_DETAIL					(u8)4				// 普通信息, 最好不要写进log文件中

// 输出设备控制, 通过位操作符组合输出, 默认输出到文件
#define		LOG_DST_NULL					(u8)0				// 不输出Log
#define		LOG_DST_FILE					(u8)0x01			// Log输出到文件
#define		LOG_DST_SERIAL					(u8)0x02			// Log输出到串口
#define		LOG_DST_TELNET					(u8)0x04			// Log输出到Telnet
#define		LOG_DST_DEFAULT					LOG_DST_FILE		// Log输出到Telnet

// 模块名定义
#define MODULE_DEF( x, y)		const u16 x = (y)

// 错误码
#define Err_t					u16								// 错误码类型
#define	LOG_ERR_NO_ERR						(Err_t)(0)			// 无错误
#define	LOG_ERR_INIT_GDATA_FAILED			(Err_t)(1)			// 全局变量初始化失败
#define	LOG_ERR_CFGPATH_TOO_LONG			(Err_t)(2)			// 配置文件路径过长
#define	LOG_ERR_OPEN_CFGFILE_FAILED			(Err_t)(3)			// 创建或打开配置文件失败,请确认是否被其它程序占用
#define	LOG_ERR_WIN_SOCK_INIT_ERR			(Err_t)(4)			// WSAStartup调用失败
#define	LOG_ERR_SOCKET_CREATE_ERR			(Err_t)(5)			// 创建soeckt失败
#define	LOG_ERR_SET_NET_BUF_FAILED			(Err_t)(6)			// 设置soeckt的缓冲区失败
#define	LOG_ERR_INIT_PRINTER_FAILED			(Err_t)(7)			// 初始化打印失败
#define	LOG_ERR_CREATE_SEM_FAILED			(Err_t)(8)			// 创建信号量失败
#define	LOG_ERR_CREATE_RECVTASK_ERR			(Err_t)(9)			// 创建接收线程失败
#define	LOG_ERR_CLIENT_CONNECT_ERR			(Err_t)(10)			// 客户端connect失败
#define	LOG_ERR_CLIENT_BIND_ERR				(Err_t)(11)			// 客户端绑定端口失败

/*====================================================================
函数名      ：LogInit
功能        ：KdvLog模块初始化
算法实现    ：内部会去读配置文件, 读不到就按默认设置log模块.
			  配置文件包含了是否启用log, 缓冲区大小, 服务器地址等信息
引用全局变量：
输入参数说明：[in] pszCfgFileFullPath, log配置文件的文件名, 完整路径. 不能为空
返回值说明  ：Err_t, 参照错误码定义
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
11/03/13    1.0         liuxu           创建
====================================================================*/
DllExport Err_t LogInit( const s8* pszCfgFileFullPath = 0);

/*====================================================================
函数名      ：LogPrint, LogDstPrint
功能        ：LogPrint, 默认打印接口, 默认输出到文件;
LogDstPrint, 可控输出到各种设备,
算法实现    ：
引用全局变量：
输入参数说明：[in] byLevel, 打印级别, 默认是最低级别
			  [in] wModuleId, 模块索引, 默认是通用索引
			  [in] pszFormat, 打印内容, 最大有效长度是108
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
11/03/13    1.0         liuxu           创建
====================================================================*/
DllExport void LogPrint( const u8 byLevel, const u16 wModule, const s8* pszFormat, ...);

/*====================================================================
函数名      ：logflush
功能        ：将缓冲区数据写入文件.某些非常重要的信息需要确保被写入文件时,请调用此接口
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
11/03/13    1.0         liuxu           创建
====================================================================*/
DllExport void logflush();

/*====================================================================
函数名      ：logsetdst
功能        ：设置Log输出方向
算法实现    ：
引用全局变量：
输入参数说明：[in] byDst, 参考输出设备定义. 支持"与"操作
			  [in] bOpen, true为打开; false,关闭
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
11/03/13    1.0         liuxu           创建
====================================================================*/
DllExport void logsetdst( const u8 byDst );

/*====================================================================
函数名      ：logsetlvl
功能        ：设置log的优先级, 一般调试用
算法实现    ：支持对全局log level或单独的某个模块的log level进行设置
对全局level设置时, 会更新每个模块的log level.
引用全局变量：
输入参数说明：[in] byLevel, 优先级. 1， error; 2, warning; 3, key status; 4, common
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
11/03/13    1.0         liuxu           创建
====================================================================*/
DllExport void logsetlvl( u8 byLevel = LOG_LVL_KEYSTATUS );

/*====================================================================
函数名      ：logdisablemod
功能        ：不打印byModuleId代表的模块的log信息, 调试用
算法实现    ：
引用全局变量：
输入参数说明：[in] wModuleId, 要被禁止打印的模块索引
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
11/03/13    1.0         liuxu           创建
====================================================================*/
DllExport void logdisablemod(const u16 wModuleId);

/*====================================================================
函数名      ：logsetport
功能        ：设置客户端端口绑定的范围, 如果不设置,就是25020~25100
算法实现    ：
引用全局变量：
输入参数说明：[in] wStartPort, 起始端口
			  [in] wPortNum, 端口范围
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
11/03/13    1.0         liuxu           创建
====================================================================*/
DllExport void logsetport(const u16 wStartPort, const u16 wPortNum);

/*====================================================================
函数名      ：logenablemod
功能        ：恢复打印wModuleId代表的模块的log信息, 调试用
算法实现    ：
引用全局变量：
输入参数说明：[in] wModuleId, 需要打开打印的模块的id, 默认全部打开
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
11/03/13    1.0         liuxu           创建
====================================================================*/
DllExport void logenablemod(const u16 wModuleId = LOG_DEF_MODULE_INDEX);

/*====================================================================
函数名      ：loghelp
功能        ：KdvLog的telnet帮助函数
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
11/03/13    1.0         liuxu           创建
====================================================================*/
DllExport void loghelp( );

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
//end of file