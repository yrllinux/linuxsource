/*=======================================================================
模块名      : 文件升级服务器
文件名      : filesrvstruct.h
相关文件    : 无
文件实现功能: 服务器公用结构定义
作者        : 王昊
版本        : V1.0  Copyright(C) 2006-2007 KDC, All rights reserved.
-------------------------------------------------------------------------
修改记录:
日      期  版本    修改人  修改内容
2006/12/28  1.0     王昊    创建
=======================================================================*/

#ifndef _FILESRVSTRUCT_H
#define _FILESRVSTRUCT_H

#ifdef WIN32
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#endif//WIN32

#include "filesrvconst.h"
#include "osp.h"

#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif

#ifdef WIN32
    #pragma comment( lib, "ws2_32.lib" )
    #pragma pack( push )
    #pragma pack( 1 )
    #define window( x )	x
#else
    #include <netinet/in.h>
    #define window( x )
#endif//WIN32

#ifdef _LINUX_

#ifndef min
#define min(a,b) ((a)>(b)?(b):(a))
#endif
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

#ifndef VOID
#define VOID void 
#endif

#endif//_LINUX_


/*------------------------------------------------------------------------------
  获取错误描述
------------------------------------------------------------------------------*/
class CErrorString
{
public:
    CErrorString() {}
    virtual ~CErrorString() {}

    /*=============================================================================
    函 数 名:MtGetErrorStr
    功    能:获取错误描述
    参    数:u32 dwErrCode              [in]    错误码
             EMFileSrvLanType emLanType [in]    语言类型
             s8 szErrStr[1024]          [out]   错误描述
    注    意:无
    返 回 值:获取成功: TURE; 获取失败: FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2007/01/11  1.0     王昊    创建
    =============================================================================*/
    static BOOL32 MtGetErrorStr(u32 dwErrCode, EMFileSrvLanType emLanType,
                                s8 szErrStr[1024])
    {
#define ONE_TEXT(code, chn, eng)                                        \
case code:                                                              \
    strcpy( szErrStr, emLanType == enumFileSrvLanChinese ? chn : eng ); \
    return TRUE;

        switch ( dwErrCode )
        {
        ONE_TEXT( SUS_ERRORCODE_MAXDBCONNECTION, "数据库已达最大连接限制！", "Maximum database thread limit!" );
        ONE_TEXT( SUS_ERRORCODE_MTALREADY_CONNECT, "该终端已连接！", "The MT had already connected!" );
        ONE_TEXT( SUS_ERRORCODE_MAX_SERVICE_SESSION, "服务器已达最大容量限制，暂时无法提供服务，请稍候再试！", "The server has reach the capacity limit, please try agin later!");
        ONE_TEXT( SUS_ERRORCODE_TIMEOUT, "操作超时！", "Operation timed out!" );
        ONE_TEXT( SUS_ERRORCODE_SUS_NOT_PREPARED, "服务器尚未启动完毕！", "The server is starting, please try again later!" );
        ONE_TEXT( SUS_ERRORCODE_SUS_DBLOADDATA_FAIL, "服务器维护中，请稍候再试！", "The server is in maintainance, please try again later!" );
        ONE_TEXT( SUS_ERRORCODE_NO_SUCH_FILE, "服务器维护中，请稍候再试！", "The server is in maintainance, please try again later!" );
        ONE_TEXT( SUS_ERRORCODE_NO_SUCH_DEV, "服务器维护中，请稍候再试！", "The server is in maintainance, please try again later!" );
        ONE_TEXT( SUS_ERRORCODE_SERVER_VERUPDATE, "服务器维护中，请稍候再试！", "The server is in maintainance, please try again later!" );
        ONE_TEXT( SUS_ERRORCODE_MT_ALREADY_NEWEST, "终端已经是最新版本，无须更新！", "The MT already running on the newest version, you do not have to update!" );
        ONE_TEXT( SUS_ERRORCODE_EXEC_DB_SQL_FAIL, "数据库操作失败！", "The Database operation failed!" );
		// fxh
        ONE_TEXT( SUS_ERRORCODE_DEV_VERTOOMUCH, "该终端在服务器中版本过多，请先删除原不使用版本再递交新版本！", "The MT has too much versions in the server, please delete the old one first, then deliever the new!" );

        default:
            return FALSE;
        }
    }

    /*=============================================================================
    函 数 名:ClientGetErrorStr
    功    能:获取错误描述
    参    数:u32 dwErrCode              [in]    错误码
             EMFileSrvLanType emLanType [in]    语言类型
             s8 szErrStr[1024]          [out]   错误描述
    注    意:无
    返 回 值:获取成功: TURE; 获取失败: FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2007/01/11  1.0     王昊    创建
    =============================================================================*/
    static BOOL32 ClientGetErrorStr(u32 dwErrCode, EMFileSrvLanType emLanType,
                                    s8 szErrStr[1024])
    {
#define ONE_TEXT(code, chn, eng)                                        \
case code:                                                              \
    strcpy( szErrStr, emLanType == enumFileSrvLanChinese ? chn : eng ); \
    return TRUE;

        switch ( dwErrCode )
        {
        ONE_TEXT( SUS_ERRORCODE_MAXDBCONNECTION, "数据库已达最大连接限制！", "Maximum database thread limit!" );
        ONE_TEXT( SUS_ERRORCODE_MTALREADY_CONNECT, "该终端已连接！", "The MT had already connected!" );
        ONE_TEXT( SUS_ERRORCODE_MAX_SERVICE_SESSION, "服务器已达最大容量限制，暂时无法提供服务，请稍候再试！", "The server has reach the capacity limit, please try agin later!");
        ONE_TEXT( SUS_ERRORCODE_TIMEOUT, "操作超时！", "Operation timed out!" );
        ONE_TEXT( SUS_ERRORCODE_SUS_NOT_PREPARED, "服务器尚未启动完毕！", "The server is starting, please try again later!" );
        ONE_TEXT( SUS_ERRORCODE_SUS_DBLOADDATA_FAIL, "初始化获取数据库信息失败！", "Load data from DB when starting failed!" );
        ONE_TEXT( SUS_ERRORCODE_NO_SUCH_FILE, "指定文件不存在！", "The file does not exist!" );
        ONE_TEXT( SUS_ERRORCODE_NO_SUCH_DEV, "找不到该设备的版本信息！", "can not find version information of the device" );
        ONE_TEXT( SUS_ERRORCODE_MT_ALREADY_NEWEST, "终端已经是最新版本，无须更新！", "The MT already running on the newest version, you do not have to update!" );
        ONE_TEXT( SUS_ERRORCODE_LOGIN_INVALID_PWD, "登录密码错误！", "Invalid login password!" );
        ONE_TEXT( SUS_ERRORCODE_LOGIN_NO_USERNAME, "登录用户不存在！", "Invalid login user!" );
        ONE_TEXT( SUS_ERRORCODE_NO_RIGHT, "您不具备该权限！", "You have no right to do that operation!" );
        ONE_TEXT( SUS_ERRORCODE_ADDUSER_EXIST, "您要添加的用户已存在！", "The user has already exist that you can not add again!" );
        ONE_TEXT( SUS_ERRORCODE_BUSY_LAST_REQ, "服务器正在处理上一条请求消息！", "The server is busy operating last request!" );
        ONE_TEXT( SUS_ERRORCODE_EDITUSER_NO_EXIST, "您要修改的用户并不存在！", "The user you want to modify does not exist!" );
        ONE_TEXT( SUS_ERRORCODE_DELUSER_NO_EXIST, "您要删除的用户并不存在！", "The user you want to delete does not exist!" );
        ONE_TEXT( SUS_ERRORCODE_EDITUSER_UP_RIGHT, "您无权提升自己的权限！", "You can not promote your right!" );
        ONE_TEXT( SUS_ERRORCODE_EDITUSER_DW_ADMIN_RT, "您无权修改超级管理员权限！", "You can not change the right of the super administrator" );
        ONE_TEXT( SUS_ERRORCODE_EDITUSER_USER_LOGIN, "该用户已登录，无法修改其权限！", "You can not change the right of the logging user!" );
        ONE_TEXT( SUS_ERRORCODE_EDITUSER_NO_CHG_NAME, "您不能修改用户名！", "You can not change the username!" );
        ONE_TEXT( SUS_ERRORCODE_DELUSER_NO_DEL_ADMIN, "您无权删除超级管理员！", "You can not delete the super administrator" );
        ONE_TEXT( SUS_ERRORCODE_DELUSER_USER_LOGIN, "改用户已登录，无法删除！", "You can not delete the logging user!" );
        ONE_TEXT( SUS_ERRORCODE_EDITVER_NO_DEV, "找不到指定设备！", "Can not find the specified device!" );
        ONE_TEXT( SUS_ERRORCODE_SERVER_VERUPDATE, "服务器正在更新版本，无法执行指定操作！", "The server is in updating!" );
        ONE_TEXT( SUS_ERRORCODE_SETREC_NO_NEW_FILE, "新的推荐版本文件缺失！", "The new recommended file lost!" );
        ONE_TEXT( SUS_ERRORCODE_SETREC_SAME_FILE, "指定版本已经是推荐版本！", "The specified version have been set as recommended already!" );
        ONE_TEXT( SUS_ERRORCODE_NO_MEMORY, "内存不足！", "Not enough memory!" );
        ONE_TEXT( SUS_ERRORCODE_DELVER_NO_RECOMMEND, "不允许删除推荐版本文件！", "You have not got the right to delete the recommended version!" );
        ONE_TEXT( SUS_ERRORCODE_DELVER_NO_FILE, "需要删除的版本不存在！", "Can not find the version you want to delete!" );
        ONE_TEXT( SUS_ERRORCODE_EXEC_DB_SQL_FAIL, "数据库操作失败！", "The Database operation failed!" );
        ONE_TEXT( SUS_ERRORCODE_UPDATEFILE_IN_DOING, "上一次版本更新尚未结束，无法再次更新", "The last updating has in progress, that you can not start a new one now!" );
        ONE_TEXT( SUS_ERRORCODE_UPDATEFILE_VER_EXIST, "该版本已存在，请删除后重新上传！", "The version has already exist, please delete the old one and update again!" );
        ONE_TEXT( SUS_ERRORCODE_DEV_HARDVER_CONFLICT, "部分硬件版本号已存在！", "Part of the specified hardware version had already exist!" );
        ONE_TEXT( SUS_ERRORCODE_UPDATE_LAST_UNFINNISH, "上一文件尚未传输完毕，无法进行此次传输！", "The last file has not finnished receiving yet!" );
        ONE_TEXT( SUS_ERRORCODE_UPDATE_FILE_NOTEXIST, "此次上传的文件资料缺失，无法继续！", "The specified file does not prepared!" );
        ONE_TEXT( SUS_ERRORCODE_UPDATE_FILEWRITE_FAIL, "文件打开失败！", "Failed to open new file!" );
        ONE_TEXT( SUS_ERRORCODE_UPDATE_FILE_TOTALPK, "错误的文件总包数！", "Invalid total package number parameter!" );
        ONE_TEXT( SUS_ERRORCODE_UPDATE_FILE_CURPK, "文件当前包数大于总包数！", "Invalid current package number!" );
        ONE_TEXT( SUS_ERRORCODE_UPDATE_FILE_PKSIZE, "文件传输包过大！", "The transferring file package is too large!" );
        ONE_TEXT( SUS_ERRORCODE_UPDATE_FILE_TYPE, "非法文件类型！", "Illegal file type!" );
        ONE_TEXT( SUS_ERRORCODE_UPDATE_FILE_SIZE, "文件大小超标！", "The file size is too large!" );
        ONE_TEXT( SUS_ERRORCODE_UPDATE_FILE_CURPK_SN, "当前文件包序号错误！", "Invalid current package number!" );
        ONE_TEXT( SUS_ERRORCODE_UPDATE_FILENAME, "文件名已存在！", "The filename had already existed!" );
        ONE_TEXT( SUS_ERRORCODE_UPDATE_FILETYPE, "文件类型已存在！", "The file type had already existed!" );
        ONE_TEXT( SUS_ERRORCODE_UPDATE_PACK_SIZE, "当前传输包大小与标称不符！", "The package size is not the same to the real transferring ones!" );
        ONE_TEXT( SUS_ERRORCODE_RESET_MAINTENANCE, "版本维护中，不允许重启！", "You can not reset the system when the server is in maintenance state!" );
        ONE_TEXT( SUS_ERRORCODE_RESET_MTLOGIN, "当前已有终端登录，不建议重启！", "You can not reset the system when some of the MTs in upgrading!" );
		//fxh
		ONE_TEXT( SUS_ERRORCODE_DEV_VERTOOMUCH, "该终端在服务器上版本数超过最大允许值，请删除不使用的版本，再上传需要的新版本!", "The MT has too much verison in the server, please delete the unused one first!")
		ONE_TEXT( SUS_ERRORCODE_NO_OTHER_VERSION, "服务器上没有配置该终端的其它可用版本!", "There is no other verison available for this type of MT!")
      
		default:
            return FALSE;
        }
    }
};

#ifdef WIN32
#pragma pack( pop )
#endif//WIN32

#endif//_FILESRVSTRUCT_H


