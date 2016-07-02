/*=======================================================================
模块名      : 文件升级服务器
文件名      : filesrvconst.h
相关文件    : 无
文件实现功能: 服务器常量定义(公用常量，以及数据库常量)
作者        : 王昊
版本        : V1.0  Copyright(C) 2006-2007 KDC, All rights reserved.
-------------------------------------------------------------------------
修改记录:
日      期  版本    修改人  修改内容
2006/12/28  1.0     王昊    创建
=======================================================================*/

#ifndef _FILESRVCONST_H
#define _FILESRVCONST_H

#ifdef WIN32
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#endif//WIN32

#include "kdvtype.h"
#include "kdvdef.h"

//服务器使用过的所有版本号
enum EMFileSrvUsedVersion
{
    enumFileSrvUsedVersion1     = (u32)1,
    enumFileSrvUsedVersion2     = (u32)2
};

//服务器当前版本号，IFileSrvMsg用到，所以放入公用头文件
#define FILESRV_CUR_VERSION                         (u32)enumFileSrvUsedVersion1


/*------------------------------------------------------------------------------
  公用常量定义
------------------------------------------------------------------------------*/

//各应用ID
#define AID_SUS_MTSSN           (AID_SUS_BGN)//终端接入
#define AID_SUS_CLIENTSSN       (AID_SUS_BGN + 1)//客户端接入
//sus内部应用号
#define AID_SUS_MAINCTL         (AID_SUS_BGN + 2)//主控
#define AID_SUS_GUARD           (AID_SUS_BGN + 3)//守卫
#define AID_SUS_FILEOPR         (AID_SUS_BGN + 4)//文件处理


//Server消息头最大长度
#define MAX_FILESRV_MSG_HEAD_LEN        (s32)(40)
//Server消息体最大长度
#define MAX_FILESRV_MSG_LEN             (s32)(28 * 1024 - MAX_FILESRV_MSG_HEAD_LEN)

//设备序列号字符串最大长度
#define MAX_FILESRV_SERIALNO_LEN        (s32)(128)

//文件描述最大长度
#define MAX_FILESRV_BUG_REPORT_LEN      (s32)(512)


//一种设备需要上传的文件最大个数
#define MAX_FILESRV_DEVFILE_NUM         (s32)(4)

//文件名最大长度
#define MAX_FILESRV_FILENAME_LEN        (s32)(64)

//硬件版本号最大长度
#define MAX_FILESRV_HARDVER_LEN         (s32)(32)

//软件版本号文字最大长度
#define MAX_FILESRV_SOFTVER_LEN         (s32)(64)

//IFCSrvDev字符串最大长度
#define MAX_FILESRV_DEVSTRING_LEN       (s32)(32)
//IFCSrvDev设备类型起始位置
#define FILESRV_DEVSTRING_TYPE_BEGIN    (s32)(0)
//IFCSrvDev设备类型字符串长度
#define FILESRV_DEVSTRING_TYPE_LEN      (s32)(8)
//IFCSrvDev设备型号起始位置
#define FILESRV_DEVSTRING_MODEL_BEGIN   (s32)(8)
//IFCSrvDev设备型号字符串长度
#define FILESRV_DEVSTRING_MODEL_LEN     (s32)(8)
//IFCSrvDev操作系统类型起始位置
#define FILESRV_DEVSTRING_OS_BEGIN      (s32)(16)
//IFCSrvDev操作系统类型字符串长度
#define FILESRV_DEVSTRING_OS_LEN        (s32)(8)

//文件类型
enum EMFileSrvFileType
{
    enumFileSrvFileTypeUnknown      = (u32)0,//未知
    enumFileSrvFileTypeService      = (u32)1,//业务文件
    enumFileSrvFileTypeConsole      = (u32)2//控制台文件
};

//设备类型
enum EMFileSrvDevType
{
    enumFileSrvDevTypeUnknown     = (u8)0,
//    enumFileSrvDevTypeMCU         = (u8)1,//MCU
    enumFileSrvDevTypeMT          = (u8)2,//MT
    enumFileSrvDevTypeFileSrv     = (u8)3,//下级文件服务器
    enumFileSrvDevTypeClient      = (u8)4//客户端
};


//设备型号(MCU)
//enum EMFileSrvDevMCUModel
//{
//    enumFileSrvDevModelMCU8000A    = (u8)1,//8000
//    enumFileSrvDevModelMCU8000B    = (u8)2,//8000B
//    enumFileSrvDevModelMCUWin32		 = (u8)3//Windows
//};

//设备型号(MT)
enum EMFileSrvDevMTModel
{
    enumFileSrvDevModelMTUnknown    = (u8)0,
//    enumFileSrvDevModelMTPCMT      = (u8)1,
//    enumFileSrvDevModelMT8010      = (u8)2,
//    enumFileSrvDevModelMT8010A     = (u8)3,//8010A
//    enumFileSrvDevModelMT8010Plus  = (u8)4,
//    enumFileSrvDevModelMT8010C     = (u8)5,//8010C
//    enumFileSrvDevModelMTIMT       = (u8)6,
    enumFileSrvDevModelMTTS6610     = (u8)7,//T2 8220A
    enumFileSrvDevModelMTTS5210     = (u8)8,//T2 8220B
    enumFileSrvDevModelMTV5         = (u8)9,//T2 8220C
    enumFileSrvDevModelMTTS3210     = (u8)10,//T2 8620A
    enumFileSrvDevModelMTTS6610E    = (u8)11,
    enumFileSrvDevModelMTTS6210     = (u8)12,

    enumFileSrvDevModelMTTS5610     = (u8)13,//T2 ts5610
    enumFileSrvDevModelMTTS3610     = (u8)14,//T2 ts3610
	enumFileSrvDevModelMTTS6210E    = (u8)15,//T2 ts6210E

	enumFileSrvDevModelMTKDV7910    = (u8)16,//KDV 7910
	enumFileSrvDevModelMTKDV7810    = (u8)17,//KDV 7810
};

//操作系统类型
enum EMFileSrvOSType
{
    enumFileSrvOSTypeUnknown        = (u8)0,
    enumFileSrvOSTypeVxworksRaw     = (u8)1,//Vxworks 裸分区
    enumFileSrvOsTypeVxworksTffs    = (u8)2,//Vxworks 单分区
    enumFileSrvOsTypeWindows        = (u8)3,//Windows
    enumFileSrvOsTypeLinuxPowerPC   = (u8)4,//PowerPC Linux
    enumFileSrvOsTypeLinuxDavinci   = (u8)5,//Davinci Arm Linux
};

//语言类型
enum EMFileSrvLanType
{
    enumFileSrvLanEnglish       = (u8)0,//英语
    enumFileSrvLanChinese       = (u8)1//中文(简体)
};

//日志请求主类型
enum EMFileSrvLogReqMainType
{
    enumFileSrvLogReqMainTypeAll        = (u8)0x00,//所有日志
    enumFileSrvLogReqMainTypeCategory   = (u8)0x01,//日志类型(用户/系统)
    enumFileSrvLogReqMainTypeLevel      = (u8)0x02,//日志等级(警告/重要/一般)
    enumFileSrvLogReqMainTypeTime       = (u8)0x04,//时间
    enumFileSrvLogReqMainTypeDev        = (u8)0x08,//设备标识
    enumFileSrvLogReqMainTypeResult     = (u8)0x10,//操作结果(成功/失败)
    enumFileSrvLogReqMainTypeEvent      = (u8)0x20//事件类型
};
//日志请求子类型
enum EMFileSrvLogReqSubTypeCategory//日志类型
{
//    enumFileSrvLogReqSubTypeCategoryAll     = (u16)0x0000,//所有
    enumFileSrvLogReqSubTypeCategoryUser    = (u16)0x0001,//用户
    enumFileSrvLogReqSubTypeCategorySystem  = (u16)0x0002//系统
};
enum EMFileSrvLogReqSubTypeLevel//日志等级
{
//    enumFileSrvLogReqSubTypeLevelAll        = (u16)0x0000,//所有
    enumFileSrvLogReqSubTypeLevelWarning    = (u16)0x0001,//警告
    enumFileSrvLogReqSubTypeLevelSeverity   = (u16)0x0002,//重要
    enumFileSrvLogReqSubTypeLevelCommon     = (u16)0x0004//一般
};
enum EMFileSrvLogReqSubTypeTime//时间
{
//    enumFileSrvLogReqSubTypeTimeAll         = (u16)0x0000,//所有时间
    enumFileSrvLogReqSubTypeTimeStart       = (u16)0x0001,//指定起始时间
    enumFileSrvLogReqSubTypeTimeEnd         = (u16)0x0002//指定结束时间
};
enum EMFileSrvLogReqSubTypeDev//设备标识
{
//    enumFileSrvLogReqSubTypeDevAll          = (u16)0x0000,//所有设备
    enumFileSrvLogReqSubTypeDevType         = (u16)0x0001,//指定类型
    enumFileSrvLogReqSubTypeDevModel        = (u16)0x0002,//指定型号
    enumFileSrvLogReqSubTypeDevOsType       = (u16)0x0004,//指定操作系统
    enumFileSrvLogReqSubTypeDevHardVer      = (u16)0x0008//指定硬件版本
//    enumFileSrvLogReqSubTypeDevSoftVer      = (u16)0x0010//指定(升级后的)软件版本
};
enum EMFileSrvLogReqSubTypeResult//操作结果
{
//    enumFileSrvLogReqSubTypeResultAll       = (u16)0x0000,//所有
    enumFileSrvLogReqSubTypeResultOK        = (u16)0x0001,//成功
    enumFileSrvLogReqSubTypeResultFail      = (u16)0x0002,//失败
};
enum EMFileSrvLogReqSubTypeEvent//事件
{
//    enumFileSrvLogReqSubTypeEventAll        = (u16)0x0000,//所有
    enumFileSrvLogReqSubTypeEventDevOpr     = (u16)0x0001,//设备操作
    enumFileSrvLogReqSubTypeEventClientOpr  = (u16)0x0002//客户端操作
};
enum EMFileSrvLogReqEventSubTypeDevOpr//设备操作
{
    enumFileSrvLogReqEventSubTypeDevOprAll             = (u16)0x0000,//所有
    enumFileSrvLogReqEventSubTypeDevOprLogin           = (u16)0x0001,//登录
    enumFileSrvLogReqEventSubTypeDevOprGetFile         = (u16)0x0002,//获取文件
    enumFileSrvLogReqEventSubTypeDevOprLogout          = (u16)0x0004//退出
};
enum EMFileSrvLogReqEventSubTypeClientOpr//客户端操作
{
    enumFileSrvLogReqEventSubTypeClientOprAll           = (u16)0x0000,//所有
    enumFileSrvLogReqEventSubTypeClientOprLogin         = (u16)0x0001,//登录
    enumFileSrvLogReqEventSubTypeClientOprAddUser       = (u16)0x0002,//添加用户
    enumFileSrvLogReqEventSubTypeClientOprEditUser      = (u16)0x0004,//修改用户
    enumFileSrvLogReqEventSubTypeClientOprDelUser       = (u16)0x0008,//删除用户
    enumFileSrvLogReqEventSubTypeClientOprGetLog        = (u16)0x0010,//查询日志
    enumFileSrvLogReqEventSubTypeClientOprDelLog        = (u16)0x0020,//删除日志
    enumFileSrvLogReqEventSubTypeClientOprUpdateFile    = (u16)0x0040,//上传文件
    enumFileSrvLogReqEventSubTypeClientOprLogout        = (u16)0x0080,//退出
};
//fxh最大上传版本数
#define MAX_VERSIONNUM                      (s32)(5)
#define MTTYPENUM                           (s32)(10)

//最大用户个数
#define MAX_FILESRV_USER_NUM                (s32)(16)

//客户端用户名最大长度
#define MAX_FILESRV_USERNAME_LEN            (s32)(32)
//客户端密码最大长度
#define MAX_FILESRV_PASSWORD_LEN            (s32)(32)


//FTP参数
#define MAX_SUS_FTP_USERNAME_LEN    (s32)32//FTP用户名长度
#define MAX_SUS_FTP_PASSWORD_LEN    (s32)32//FTP密码长度
#define MAX_SUS_FTP_PATH_LEN        (s32)128//FTP路径长度
#define SUS_FTP_DEFAULT_PORT        (u16)21//FTP默认端口

//本地文件路径
#define MAX_SUS_LOCAL_FILE_PATH     (s32)128


//默认配置
#define PORT_SUS_LISTEN             (u16)60000  //服务器侦听端口
#define PORT_SUS_TELNET             (u16)2800   //服务器Telnet端口
#define SUS_DB_NAME                 (LPCSTR)"sus"//数据库名
#define SUS_DB_PORT                 (u16)5000   //数据库端口
#define SUS_WDSRV_PORT              (u16)10000  //看门狗服务端端口
#define SUS_WDSRV_CHECKCOUNT        (u8)10      //看门狗服务端测试次数
#define SUS_WDSRV_CHECKTIME         (u16)10     //看门狗服务端测试间隔(s)
#define SUS_WDAGT_CHECKCOUNT        (u32)10     //看门狗客户端测试次数
#define SUS_WDAGT_REGTIME           (u32)10     //看门狗客户端测试间隔(s)
#define SUS_DB_SUS_CONNNUM          (u32)1      //内部数据库连接线程个数
#define SUS_DB_SUC_CONNNUM          (u32)1      //客户端数据库连接线程个数
#define SUS_DB_MT_CONNNUM           (u32)16     //终端数据库连接线程个数




/*------------------------------------------------------------------------------
  错误码
------------------------------------------------------------------------------*/

#define SUS_ERRORCODE_BGN                   (u32)(20000)

#define SUS_ERRORCODE_SUCCESS               (u32)(0)
#define SUS_ERRORCODE_MAXDBCONNECTION       (SUS_ERRORCODE_BGN + 1)

//终端错误
#define SUS_ERRORCODE_MTALREADY_CONNECT     (SUS_ERRORCODE_BGN + 1000)
#define SUS_ERRORCODE_MT_ALREADY_NEWEST     (SUS_ERRORCODE_BGN + 1001)
#define SUS_ERRORCODE_NO_OTHER_VERSION      (SUS_ERRORCODE_BGN + 1002)


//服务器内部错误
#define SUS_ERRORCODE_MAX_SERVICE_SESSION   (SUS_ERRORCODE_BGN + 2000)
#define SUS_ERRORCODE_TIMEOUT               (SUS_ERRORCODE_BGN + 2001)
#define SUS_ERRORCODE_SUS_NOT_PREPARED      (SUS_ERRORCODE_BGN + 2002)
#define SUS_ERRORCODE_SUS_DBLOADDATA_FAIL   (SUS_ERRORCODE_BGN + 2003)
//#define SUS_ERRORCODE_SUS_FILE_NOT_PREPARED (SUS_ERRORCODE_BGN + 2004)
#define SUS_ERRORCODE_NO_SUCH_FILE          (SUS_ERRORCODE_BGN + 2005)
#define SUS_ERRORCODE_NO_SUCH_DEV           (SUS_ERRORCODE_BGN + 2006)
#define SUS_ERRORCODE_SERVER_VERUPDATE      (SUS_ERRORCODE_BGN + 2007)
#define SUS_ERRORCODE_NO_MEMORY             (SUS_ERRORCODE_BGN + 2008)
#define SUS_ERRORCODE_EXEC_DB_SQL_FAIL      (SUS_ERRORCODE_BGN + 2009)

//客户端错误
#define SUS_ERRORCODE_LOGIN_INVALID_PWD     (SUS_ERRORCODE_BGN + 3000)
#define SUS_ERRORCODE_LOGIN_NO_USERNAME     (SUS_ERRORCODE_BGN + 3001)
#define SUS_ERRORCODE_NO_RIGHT              (SUS_ERRORCODE_BGN + 3002)
#define SUS_ERRORCODE_ADDUSER_EXIST         (SUS_ERRORCODE_BGN + 3003)
#define SUS_ERRORCODE_BUSY_LAST_REQ         (SUS_ERRORCODE_BGN + 3004)
#define SUS_ERRORCODE_EDITUSER_NO_EXIST     (SUS_ERRORCODE_BGN + 3005)
#define SUS_ERRORCODE_DELUSER_NO_EXIST      (SUS_ERRORCODE_BGN + 3006)
#define SUS_ERRORCODE_EDITUSER_UP_RIGHT     (SUS_ERRORCODE_BGN + 3007)
#define SUS_ERRORCODE_EDITUSER_DW_ADMIN_RT  (SUS_ERRORCODE_BGN + 3008)
#define SUS_ERRORCODE_EDITUSER_USER_LOGIN   (SUS_ERRORCODE_BGN + 3009)
#define SUS_ERRORCODE_EDITUSER_NO_CHG_NAME  (SUS_ERRORCODE_BGN + 3010)
#define SUS_ERRORCODE_DELUSER_NO_DEL_ADMIN  (SUS_ERRORCODE_BGN + 3011)
#define SUS_ERRORCODE_DELUSER_USER_LOGIN    (SUS_ERRORCODE_BGN + 3012)
#define SUS_ERRORCODE_EDITVER_NO_DEV        (SUS_ERRORCODE_BGN + 3013)
#define SUS_ERRORCODE_SETREC_NO_NEW_FILE    (SUS_ERRORCODE_BGN + 3014)
#define SUS_ERRORCODE_SETREC_SAME_FILE      (SUS_ERRORCODE_BGN + 3015)
#define SUS_ERRORCODE_DELVER_NO_RECOMMEND   (SUS_ERRORCODE_BGN + 3016)
#define SUS_ERRORCODE_DELVER_NO_FILE        (SUS_ERRORCODE_BGN + 3017)
#define SUS_ERRORCODE_UPDATEFILE_IN_DOING   (SUS_ERRORCODE_BGN + 3018)
#define SUS_ERRORCODE_UPDATEFILE_VER_EXIST  (SUS_ERRORCODE_BGN + 3019)
#define SUS_ERRORCODE_DEV_HARDVER_CONFLICT  (SUS_ERRORCODE_BGN + 3020)
#define SUS_ERRORCODE_UPDATE_LAST_UNFINNISH (SUS_ERRORCODE_BGN + 3021)
#define SUS_ERRORCODE_UPDATE_FILE_NOTEXIST  (SUS_ERRORCODE_BGN + 3022)
#define SUS_ERRORCODE_UPDATE_FILEWRITE_FAIL (SUS_ERRORCODE_BGN + 3023)
#define SUS_ERRORCODE_UPDATE_FILE_TOTALPK   (SUS_ERRORCODE_BGN + 3024)
#define SUS_ERRORCODE_UPDATE_FILE_CURPK     (SUS_ERRORCODE_BGN + 3025)
#define SUS_ERRORCODE_UPDATE_FILE_PKSIZE    (SUS_ERRORCODE_BGN + 3026)
#define SUS_ERRORCODE_UPDATE_FILE_TYPE      (SUS_ERRORCODE_BGN + 3027)
#define SUS_ERRORCODE_UPDATE_FILE_SIZE      (SUS_ERRORCODE_BGN + 3028)
#define SUS_ERRORCODE_UPDATE_FILE_CURPK_SN  (SUS_ERRORCODE_BGN + 3029)
#define SUS_ERRORCODE_UPDATE_FILENAME       (SUS_ERRORCODE_BGN + 3030)
#define SUS_ERRORCODE_UPDATE_FILETYPE       (SUS_ERRORCODE_BGN + 3031)
#define SUS_ERRORCODE_UPDATE_PACK_SIZE      (SUS_ERRORCODE_BGN + 3032)
#define SUS_ERRORCODE_RESET_MAINTENANCE     (SUS_ERRORCODE_BGN + 3033)
#define SUS_ERRORCODE_RESET_MTLOGIN         (SUS_ERRORCODE_BGN + 3034)
//fxh
#define SUS_ERRORCODE_DEV_VERTOOMUCH        (SUS_ERRORCODE_BGN + 3035)
/*------------------------------------------------------------------------------
  服务器专有常量
------------------------------------------------------------------------------*/
//文件传输最大长度(传输文件按照5K切包)
#define MAX_FILESRV_DEV_FILE_LEN            (s32)(5 * 1024)



/*------------------------------------------------------------------------------
  客户端专有常量
------------------------------------------------------------------------------*/
//客户端配置通知
enum EMFileSrvCfgNotifType
{
    enumFileSrvCfgNotifTypeReq  = (u8)1,//客户端请求配置
    enumFileSrvCfgNotifTypeAdd  = (u8)2,//新增配置
    enumFileSrvCfgNotifTypeEdit = (u8)3//修改配置
};

//数据库常量
#define MAX_DB_NAME_LEN                 (s32)64//数据库名
#define MAX_DB_USERNAME_LEN             (s32)64//数据库用户名
#define MAX_DB_USER_PWD_LEN             (s32)32//数据库密码

//数据库类型
enum EMDBEngineType
{
    enumDBEngineTypeSybase      = (u8)0,
    enumDBEngineTypeMySql       = (u8)1,
    enumDBEngineTypeOracle      = (u8)2
};

//文件传输最大长度(传输文件按照20K切包)
#define MAX_FILESRV_CLIENT_FILE_LEN             (s32)(20 * 1024)

#endif//_FILESRVCONST_H
