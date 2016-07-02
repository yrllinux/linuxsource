/*****************************************************************************
模块名      : DcMtLib
文件名      : DcMtAdapter.h
相关文件    : DcMtAdapter.cpp 
文件实现功能: 定义CDcMtAdapter类,
              该类提供DcMtLib上层使用的接口
              实现DcMtLib和上层应用通讯的适配器的功能
作者        : 陈洪斌
模块版本    : V4.0  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      文件版本        修改人      修改内容
2005/05/24  1.0             陈洪斌      创建
******************************************************************************/

#ifndef _DCMTADAPTER_H
#define _DCMTADAPTER_H

#include"kdvtype.h"
#include<ptlib.h>

#include"dcmtlibstruct.h"

/*====================================================================
类名 ： CDcMtAdapter
描述 ： 使用DcmtLib的接口
====================================================================*/
class CDcMtAdapter
{
public:
    /*====================================================================
    函数名      ： 构造函数
    输入参数说明： szNodeIp : IP地址(ASCII编码)   nIpLen : IP地址的长度
                   szNodeAlias : 节点名称(UTF8编码)   nAliasLen : 名称的长度
                   szNodeEmail : 节点Email(Unicode编码)   nEmailLen : Email的长度
                   szNodeLocation : 节点地址(Unicode编码)   nLocationLen : 地址的长度
                   szNodePhone : 电话号码(Unicode编码)   nPhoneLen : 电话的长度
                   wNodePort : 节点端口号
                   其中，长度数据均包括结束字符（'\0'或'\0\0'）
    ====================================================================*/
    CDcMtAdapter(const char* szNodeIp, s32 nIpLen,
                 const char* szNodeAlias, s32 nAliasLen,
                 const char* szNodeEmail, s32 nEmailLen,
                 const char* szNodeLocation, s32 nLocationLen,
                 const char* szNodePhone, s32 nPhoneLen,
                 u16  wNodePort = 1503);
    /*====================================================================
    函数名      ： 析构函数
    功能        ：
    ====================================================================*/
    virtual ~CDcMtAdapter();

///////////////////////////////////////////////////////////////////////
//会议相关  -- 下行接口
///////////////////////////////////////////////////////////////////////
    /*====================================================================
    函数名      :IsInitSuccess
    功能        :是否成功初始化协议栈
    算法实现    :无
    引用全局变量:g_cApeSap
    输入参数说明:无
    返回值说明  :true : 初始化成功
                 false: 初始化失败
    ====================================================================*/
    bool IsInitSuccess();

    /*====================================================================
    函数名      :HostConference
    功能        :主持会议
    算法实现    :无
    引用全局变量:g_cApeSap
    输入参数说明:szConfName, 会议名称
                 byAppMask,	 应用掩码
                             禁止其他节点启用某些应用
                             byAppMask由若干AppMask用|运算符连接
                             AppMask参见Gcc::AppMask的定义
                 pUserData, 回调参数
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool HostConference(const char* szConfName,
                        u8 byAppMask = e_AppAllAllowed,
                        void* pUserData = NULL);
    
    /*====================================================================
    函数名      :Query
    功能        :查询其他终端会议信息
    算法实现    :无
    引用全局变量:g_cApeSap
    输入参数说明:szAddr, 呼叫的节点IP
                 pUserData, 回调参数
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool Query(const char* szAddr,
                void* pUserData = NULL);
    
    /*====================================================================
    函数名      :JoinConference
    功能        :申请加入远端的会议
    算法实现    :无
    引用全局变量:g_cApeSap
    输入参数说明:szAddr, 远端节点的IP地址
                 szConfName, 会议名称
                 szPwd, 加入会议的密码
                 pUserData, 回调参数
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool JoinConference(const char* szAddr,
                        const char* szConfName, 
                        const char* szPwd = "",
                        void* pUserData = NULL);

    /*====================================================================
    函数名      :InviteMt
    功能        :邀请节点加入会议
    算法实现    :无
    引用全局变量:g_cApeSap
    输入参数说明:szConfName,会议名
                 szAddr, 远端节点的IP地址
                 pUserData, 回调参数
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool InviteMt(const char* szConfName,
                  const char* szAddr,
                  void* pUserData = NULL);
    /*====================================================================
    函数名      :EjectUser
    功能        :踢出节点
    算法实现    :无
    引用全局变量:g_cApeSap
    输入参数说明:nNodeID,节点用户ID
                 pUserData, 回调参数
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool EjectUser(NodeID nNodeID,
                   void* pUserData = NULL);

    /*====================================================================
    函数名      :HangUp
    功能        :挂断
    算法实现    :无
    引用全局变量:g_cApeSap
    输入参数说明:
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool HangUp();

    /*====================================================================
    函数名      :JoinResponse
    功能        :加入会议请求的响应
    算法实现    :无
    引用全局变量:g_cApeSap
    输入参数说明:nTag, 远端节点标示
                 enumResult,是否允许加入会议
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool JoinResponse(u32 nTag, EConferenceResponseResult enumResult);

    /*====================================================================
    函数名      :InviteResponse
    功能        :邀请入会请求的响应
    算法实现    :无
    引用全局变量:g_cApeSap
    输入参数说明:nTag, 远端节点的标示
                 enumResult,是否接受邀请
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool InviteResponse(u32 nTag, EConferenceResponseResult enumResult);

    /*====================================================================
    函数名      :SetFlowControl
    功能        :设置码流带宽
    算法实现    :无
    引用全局变量:g_cApeSap
    输入参数说明:nFlowBandwidth 码流带宽
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool SetFlowControl(s32 nFlowBandwidth);

///////////////////////////////////////////////////////////////////////
//会议相关  -- 上行响应函数
///////////////////////////////////////////////////////////////////////    
    /*====================================================================
    函数名      :OnCreateConfirm
    功能        :创建会议回复
    算法实现    :无
    引用全局变量:无
    输入参数说明:strFrom,创建会议目标节点地址
                 enumResult,回复
                 bLocalCrate,是否在本地创建会议
                 pUserData,用户数据
    返回值说明  :无
    ====================================================================*/
    virtual void OnCreateConfirm(const PString& strFrom,
                                 EConferenceResponseResult enumResult ,
                                 bool bLocalCreate) = 0;

    /*====================================================================
    函数名      :OnQueryResponse
    功能        :呼叫远端回复会议列表
    算法实现    :无
    引用全局变量:无
    输入参数说明:strFrom,远端IP地址
                 enumResult,操作结果
                 enumENodeType,远端节点类型
                 pcConfDesc,会议结构
                 wConfNum,会议总数
                 pUserData,用户数据
    返回值说明  :无
    ====================================================================*/
    virtual void OnQueryResponse(const PString& strFrom,
                                EConferenceResponseResult enumResult,
                                ENodeType enumENodeType,
                                const PString* pstrConfName,
                                u16 wConfNum) = 0;

    /*====================================================================
    函数名      :OnJoinConfirm
    功能        :申请入会回复
    算法实现    :无
    引用全局变量:无
    输入参数说明:strFrom,会议节点地址
                 enumResult,回复
                 pUserData,用户数据
    返回值说明  :无
    ====================================================================*/
    virtual void OnJoinConfirm(const PString& strFrom ,
                                EConferenceResponseResult enumResult) = 0;

    /*====================================================================
    函数名      :OnJoinRequest
    功能        :其他节点申请入会
    算法实现    :无
    引用全局变量:无
    输入参数说明:nFromTag,申请入会节点标示
                 strMtAlias,申请操作节点别名
                 strPassword,口令
    返回值说明  :无
    ====================================================================*/
    virtual void OnJoinRequest( u32 nFromTag ,
                                const PString &strMtAlias,
                                const PString& strPassword) = 0;

    /*====================================================================
    函数名      :OnInviteConfirm
    功能        :本节点加入会议
    算法实现    :无
    引用全局变量:无
    输入参数说明:strFrom,邀请源节点地址
                 enumResult,邀请结果
                 pUserData,用户数据
    返回值说明  :无
    ====================================================================*/
    virtual void OnInviteConfirm(const PString& strFrom ,
                                EConferenceResponseResult enumResult ) = 0;

    /*====================================================================
    函数名      :OnInviteRequest
    功能        :远端邀请入会
    算法实现    :无
    引用全局变量:无
    输入参数说明:nFromTag,申请操作节点标示
                 strMtAlias,申请操作节点别名
                 cConfDesc,会议结构
    返回值说明  :无
    ====================================================================*/
    virtual void OnInviteRequest(u32 nFromTag,
                                 const PString &strMtAlias,
                                 const PString &strConfName) = 0;

    /*====================================================================
    函数名      :OnEjectUserIndication
    功能        :本节点呼叫被结束
    算法实现    :无
    引用全局变量:无
    输入参数说明:enumReason,呼叫被结束原因
    返回值说明  :无
    ====================================================================*/
    virtual void OnEjectUserIndication(EEjectUserReason enumReason) = 0;

    /*====================================================================
    函数名      :OnEjectUserConfirm
    功能        :其他节点呼叫结束
    算法实现    :无
    引用全局变量:无
    输入参数说明:nNodeId,被结束呼叫节点ID
                 enumResult,结束呼叫回复
    返回值说明  :无
    ====================================================================*/
    virtual void OnEjectUserConfirm(NodeID nNodeId,
                                    EEjectUserResult enumResult) = 0;

    /*====================================================================
    函数名      :OnNodeAddIndication
    功能        :节点加入会议
    算法实现    :无
    引用全局变量:无
    输入参数说明:nNodeId,加入会议节点ID
                 strFrom,源地址
                 enumNodePos,节点逻辑位置
                 strName,节点别名
                 strEmail,节点Email地址
                 strPhone,节点电话号码
                 strLocation,节点物理位置
                 pUserData,用户数据
    返回值说明  :无
    ====================================================================*/
    virtual void OnNodeAddIndication(NodeID nNodeId ,
                                    const PString& strFrom ,
                                    EConfNodePos enumNodePos,
                                    const PString& strName ,
                                    const PString& strEmail,
                                    const PString& strPhone ,
                                    const PString& strLocation) = 0;

    /*====================================================================
    函数名      :OnNodeRemoveIndication
    功能        :节点退出会议通知（不包括本节点）
    算法实现    :无
    引用全局变量:无
    输入参数说明:nNodeId,退出节点用户ID
    返回值说明  :无
    ====================================================================*/
    virtual void OnNodeRemoveIndication( NodeID nNodeId ) = 0;

    /*====================================================================
    函数名      :OnTerminate
    功能        :本节点退出会议
    算法实现    :无
    引用全局变量:无
    输入参数说明:enumReason,退出会议原因
    返回值说明  :无
    ====================================================================*/
    virtual void OnTerminate(EConferenceTerminateReason enumReason) = 0;

///////////////////////////////////////////////////////////////////////
//应用通知  -- 上行响应函数
///////////////////////////////////////////////////////////////////////
    /*====================================================================
    函数名      :OnOtherStillImageStart
    功能        :本节点没有启动白板时，其他节点启动了白板
    算法实现    :无
    引用全局变量:无
    输入参数说明:nNodeId：启动白板的节点
    返回值说明  :无
    ====================================================================*/
    virtual void OnOtherStillImageStart(NodeID nNodeId) = 0;

    /*====================================================================
    函数名      :OnOtherMBFTStart
    功能        :本节点没有启动文件传输时，其他节点启动了文件传输
    算法实现    :无
    引用全局变量:无
    输入参数说明:nNodeId：启动文件传输的节点
    返回值说明  :无
    ====================================================================*/
    virtual void OnOtherMBFTStart(NodeID nNodeId) = 0;

    /*====================================================================
    函数名      :OnOtherMsChatStart
    功能        :本节点没有启动聊天时，其他节点启动了聊天
    算法实现    :无
    引用全局变量:无
    输入参数说明:nNodeId：启动聊天的节点
    返回值说明  :无
    ====================================================================*/
    virtual void OnOtherMsChatStart(NodeID nNodeId) = 0;

    /*====================================================================
    函数名      :OnAppDisableIndication
    功能        :应用掩码通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:byAppMask,应用掩码，用&合并不同掩码
                       判断 (byAppMask&AppMask)==AppMask
                       如果成立，说明该App是被禁止的
                       AppMask参见Gcc::AppMask的定义
    返回值说明  :无
    ====================================================================*/
    virtual void OnAppDisableIndication( const u8 byAppMask ) = 0;

    
///////////////////////////////////////////////////////////////////////
//应用程序共享应用  -- 下行接口
///////////////////////////////////////////////////////////////////////    
    
    /*====================================================================
    函数名      :SetAppShareCapability
    功能        :设置本节点能力级
    算法实现    :无
    引用全局变量:无
    输入参数说明:byBitsPerPixel,最大像素位数
                 byColorPointerCacheSize,最大非系统鼠标缓冲
                 wDesktopWidth,本节点桌面宽度
                 wDesktopHeight,本节点桌面高度
    返回值说明  :无
    ====================================================================*/
    void SetAppShareCapability(u8 byBitsPerPixel , 
                              u8 byColorPointerCacheSize , 
                              u16 wDesktopWidth , 
                              u16 wDesktopHeight);

    /*====================================================================
    函数名      :UpdateAppShareCapability
    功能        :共享中更新本节点能力级
    算法实现    :无
    引用全局变量:无
    输入参数说明:byBitsPerPixel,最大像素位数
                 byColorPointerCacheSize,最大非系统鼠标缓冲
                 wDesktopWidth,本节点桌面宽度
                 wDesktopHeight,本节点桌面高度
    返回值说明  :无
    ====================================================================*/
    void UpdateAppShareCapability(u8 byBitsPerPixel , 
                                 u8 byColorPointerCacheSize , 
                                 u16 wDesktopWidth , 
                                 u16 wDesktopHeight);

    /*====================================================================
    函数名      :StartAppShare
    功能        :启动应用程序共享
    算法实现    :无
    引用全局变量:无
    输入参数说明:wNumberApplications,共享进程数
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool StartAppShare( u16 wNumberApplications );

    /*====================================================================
    函数名      :StartDesktopAppShare
    功能        :启动桌面共享
    算法实现    :无
    引用全局变量:无
    输入参数说明:无
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool StartDesktopAppShare( );

    /*====================================================================
    函数名      :StopAppShare
    功能        :停止共享
    算法实现    :无
    引用全局变量:无
    输入参数说明:无
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool StopAppShare( );

    /*====================================================================
    函数名      :UpdateWindowList
    功能        :更新窗口列表
    算法实现    :无
    引用全局变量:无
    输入参数说明:wListTime,事件相对时间（毫秒）
                 ptWindow,窗口列表信息
                 wWndNumber,窗口总数
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool UpdateWindowList(u16 wListTime , 
                         TAppShareWindow* ptWindow ,
                         u16 wWndNumber);

    /*====================================================================
    函数名      :ActivateHostedWindow,ActivateLocalWindow
    功能        :更新活动窗口
    算法实现    :无
    引用全局变量:无
    输入参数说明:nWindowID,活动窗口ID
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool ActivateHostedWindow( s32 nWindowID );
    bool ActivateLocalWindow( );

    /*====================================================================
    函数名      :UpdatePalette
    功能        :更新调色板
    算法实现    :无
    引用全局变量:无
    输入参数说明:pbyPalette,调色板信息（每一索引4u8，按BLUE-GREEN-RED-RESERVERD排列）
                 wColorNumber,调色板索引总数
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool UpdatePalette(u8* pbyPalette ,
                      u16 wColorNumber );

    /*====================================================================
    函数名      :UpdateBitmap
    功能        :更新图象
    算法实现    :无
    引用全局变量:无
    输入参数说明:wDestLeft,目标矩形左上角X坐标
                 wDestTop,目标矩形左上角Y坐标
                 wDestRight,目标矩形右下角X坐标
                 wDestBottom,目标矩形右下角Y坐标
                 wWidth ,图象缓冲宽度
                 wHeight ,图象缓冲高度
                 byBitPerPixel ,每像素颜色位数（8为256色，24真彩24位色）
                 pbyBmpData,图象缓冲
                 wBmpDataLength,图象缓冲长度
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool UpdateBitmap(u16 wDestLeft ,
                     u16 wDestTop ,
                     u16 wDestRight ,
                     u16 wDestBottom ,
                     u16 wWidth ,
                     u16 wHeight ,
                     u8 byBitPerPixel ,
                     u8* pbyBmpData,
                     u16 wBmpDataLength);

    /*====================================================================
    函数名      :UpdateSystemPointer
    功能        :更新系统鼠标
    算法实现    :无
    引用全局变量:无
    输入参数说明:dwSystemPointerType,系统鼠标类型（0无鼠标，0x7f00系统默认鼠标）
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool UpdateSystemPointer( u32 dwSystemPointerType );

    /*====================================================================
    函数名      :UpdateColorPointer
    功能        :更新非系统鼠标，在鼠标缓存中写入新的鼠标形状
    算法实现    :无
    引用全局变量:无
    输入参数说明:ptColorPointer,非系统鼠标形状信息
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool UpdateColorPointer(TAppShareColorPointer* ptColorPointer );

    /*====================================================================
    函数名      :UpdateCachePointer
    功能        :更新鼠标形状(索引)
    算法实现    :无
    引用全局变量:无
    输入参数说明:wCacheIndex,非系统鼠标缓冲索引
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool UpdateCachePointer(u16 wCacheIndex);

    /*====================================================================
    函数名      :UpdatePointerPosition
    功能        :更新鼠标位置
    算法实现    :无
    引用全局变量:无
    输入参数说明:wPositionX,鼠标位置X坐标
                 wPositionY,鼠标位置Y坐标
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool UpdatePointerPosition(u16 wPositionX ,
                              u16 wPositionY );

    /*====================================================================
    函数名      :EnableRC
    功能        :远程控制许可
    算法实现    :无
    引用全局变量:无
    输入参数说明:bEnable,是否许可远程控制
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool EnableRC( bool bEnable = false );

    /*====================================================================
    函数名      :ConfirmRC
    功能        :远程控制申请回复
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,申请节点ID
                 wRequestIndex,请求索引
                 wConfirmIndex,同意远程控制索引
                 eConfirmType,是否许可远程控制（及拒绝原因）
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool ConfirmRC(NodeID nInitiator,
                  u16 wRequestIndex,
                  u16 wConfirmIndex,
                  ERcConfirmType eConfirmType );


    /*====================================================================
    函数名      :RequestRC
    功能        :申请远程控制
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,开始共享的节点ID
                 wRequestIndex,申请远程控制索引
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool RequestRC(NodeID nInitiator,
                     u16 wRequestIndex);

    /*====================================================================
    函数名      :ReleaseRC
    功能        :申请放弃远程控制
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,开始共享的节点ID
                 wRequestIndex,申请索引
                 wControlIndex,远程控制索引
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool ReleaseRC(NodeID nInitiator,
                  u16 wRequestIndex,
                  u16 wControlIndex);

    /*====================================================================
    函数名      :MouseMove
                 LMouseDown
                 LMouseUp
                 LMouseDoubleClick
                 RMouseDown
                 RMouseUp
                 RMouseDoubleClick
                 MiddleMouseDown
                 MiddleMouseUp
                 MiddleMouseDoubleClick
    功能        :远程控制鼠标操作
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,开始共享的节点ID
                 nEventTime,事件相对时间（毫秒）
                 nPointX,鼠标位置横坐标
                 nPointY,鼠标位置纵坐标
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool MouseMove(NodeID nInitiator,
                    s32 nEventTime,
                    s32 nPointX,
                    s32 nPointY );
    bool LMouseDown(NodeID nInitiator,
                    s32 nEventTime,
                    s32 nPointX,
                    s32 nPointY );
    bool LMouseUp(NodeID nInitiator,
                    s32 nEventTime,
                    s32 nPointX,
                    s32 nPointY );
    bool LMouseDoubleClick(NodeID nInitiator,
                    s32 nEventTime,
                    s32 nPointX,
                    s32 nPointY );
    bool RMouseDown(NodeID nInitiator,
                    s32 nEventTime,
                    s32 nPointX,
                    s32 nPointY );
    bool RMouseUp(NodeID nInitiator,
                    s32 nEventTime,
                    s32 nPointX,
                    s32 nPointY );
    bool RMouseDoubleClick(NodeID nInitiator,
                    s32 nEventTime,
                    s32 nPointX,
                    s32 nPointY );
    bool MiddleMouseDown(NodeID nInitiator,
                    s32 nEventTime,
                    s32 nPointX,
                    s32 nPointY );
    bool MiddleMouseUp(NodeID nInitiator,
                    s32 nEventTime,
                    s32 nPointX,
                    s32 nPointY );
    bool MiddleMouseDoubleClick(NodeID nInitiator,
                    s32 nEventTime,
                    s32 nPointX,
                    s32 nPointY );

    /*====================================================================
    函数名      :KeyDown
                 KeyUp
    功能        :远程控制键盘操作
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,开始共享的节点ID
                 nEventTime,事件相对时间（毫秒）
                 wCode,键盘操作虚拟键
                 byCodeType     -- 按键编码方案：1：ASCII字符，2：键盘虚拟键
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool KeyDown(NodeID nInitiator,
                 s32 nEventTime,
                 u16 wCode,
                 BYTE byCodeType = 2);
    bool KeyUp(NodeID nInitiator,
               s32 nEventTime,
               u16 wCode,
               BYTE byCodeType = 2);

    /*====================================================================
    函数名      :RCActivateHostedWindow
    功能        :远程激活窗口
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,开始共享的节点ID
                 nWindowID,窗口ID
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool RCActivateHostedWindow( NodeID nInitiator , s32 nWindowID );

    /*====================================================================
    函数名      :RCActivateHostedWindow
    功能        :远程恢复窗口
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,开始共享的节点ID
                 nWindowID,窗口ID
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool RCRestoreHostedWindow( NodeID nInitiator , s32 nWindowID );

///////////////////////////////////////////////////////////////////////
//应用程序共享应用  -- 上行响应函数
///////////////////////////////////////////////////////////////////////  
    /*====================================================================
    函数名      :OnAppShareCapabilityNegotiationSuccess
    功能        :能力级交换成功
    算法实现    :无
    引用全局变量:无
    输入参数说明:byBitsPerPixel,最大像素位数
                （小于24只能发送256色图象数据，大于24可以发送真彩24位色图象数据）
                 byMacColorPointerCache,最大非系统鼠标缓冲数
    返回值说明  :无
    ====================================================================*/
    virtual void OnAppShareCapabilityNegotiationSuccess( u8 byBitsPerPixel ,
                                                         u8 byMaxColorPointerCache ) = 0;
          
    /*====================================================================
    函数名      :OnStartAppShareNotif
    功能        :开始共享
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,开始共享应用的节点ID
                 strNodeName,开始共享应用的终端别名
                 wdesktopWidth,桌面宽度
                 wdesktopHeight,桌面高度
    返回值说明  :无
    ====================================================================*/
    virtual void OnStartAppShareNotif( NodeID nInitiator ,
                                        const PString& strNodeName ,
                                        u16 wdesktopWidth ,
                                        u16 wdesktopHeight ) = 0;

    /*====================================================================
    函数名      :OnStartDesktopShareNotif
    功能        :开始桌面共享
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,开始共享桌面的节点ID
                 strNodeName,开始共享桌面的终端别名
                 wdesktopWidth,桌面宽度
                 wdesktopHeight,桌面高度
    返回值说明  :无
    ====================================================================*/
    virtual void OnStartDesktopShareNotif( NodeID nInitiator ,
                                           const PString& strNodeName ,
                                           u16 wdesktopWidth ,
                                           u16 wdesktopHeight ) = 0;

    /*====================================================================
    函数名      :OnStopAppShareNotif
    功能        :结束共享
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,开始共享的节点ID
    返回值说明  :无
    ====================================================================*/
    virtual void OnStopAppShareNotif( NodeID nInitiator ) = 0;

    /*====================================================================
    函数名      :OnUpdateWindowList
    功能        :共享窗口列表变化
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,开始共享的节点ID
                 ptWindow,窗口列表信息
                 wWndNumber,窗口总数
    返回值说明  :无
    ====================================================================*/
    virtual void OnUpdateWindowList( NodeID nInitiator ,
                                    TAppShareWindow* ptWindow ,
                                    u16 wWndNumber) = 0;

    /*====================================================================
    函数名      :OnActivateHostedWindow,OnActivateLocalWindow
    功能        :远端终端更新活动窗口通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,开始共享的节点ID
                 nWindowID,活动窗口ID
    返回值说明  :无
    ====================================================================*/
    virtual void OnActivateHostedWindow(NodeID nInitiator,
                                        s32 nWindowID) = 0;
    virtual void OnActivateLocalWindow(NodeID nInitiator) = 0;

    /*====================================================================
    函数名      :OnUpdatePalette
    功能        :调色板变化
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,开始共享的节点ID
                 byPalette,调色板信息（每一索引4u8，按BLUE-GREEN-RED-RESERVERD排列）
                 wColorNumber,调色板索引总数
    返回值说明  :无
    ====================================================================*/
    virtual void OnUpdatePalette( NodeID nInitiator ,
                                  u8* pbyPalette ,
                                  u16 nNumberColor ) = 0;
    
    /*====================================================================
    函数名      :OnUpdateBitmap
    功能        :更新图象
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,开始共享的节点ID
                 wDestLeft,目标矩形左上角X坐标
                 wDestTop,目标矩形左上角Y坐标
                 wDestRight,目标矩形右下角X坐标
                 wDestBottom,目标矩形右下角Y坐标
                 wWidth ,图象缓冲宽度
                 wHeight ,图象缓冲高度
                 byBitPerPixel ,每像素颜色位数（8为256色，24真彩24位色）
                 pbyBmpData,图象缓冲
                 wBmpDataLength,图象缓冲长度
    返回值说明  :无
    ====================================================================*/
    virtual void OnUpdateBitmap(NodeID nInitiator ,
                                u16 wDestLeft ,
                                u16 wDestTop ,
                                u16 wDestRight ,
                                u16 wDestBottom ,
                                u16 wWidth ,
                                u16 wHeight ,
                                u8 byBitPerPixel ,
                                u8* pbyBmpData,
                                u16 wBmpDataLength) = 0;

    /*====================================================================
    函数名      :OnUpdateCapability
    功能        :共享启动者更新能力级
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,开始共享桌面的节点ID
                 strNodeName,开始共享桌面的终端别名
                 wdesktopWidth,桌面宽度
                 wdesktopHeight,桌面高度
    返回值说明  :无
    ====================================================================*/
    virtual void OnUpdateCapability( NodeID nInitiator ,
                                    const PString& strNodeName , 
                                    u16 wdesktopWidth ,
                                    u16 wdesktopHeight ) = 0;

    /*====================================================================
    函数名      :OnUpdateSystemPointer
    功能        :更新系统鼠标
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,开始共享的节点ID
                 dwSystemPointerType,系统鼠标类型（0无鼠标，0x7f00系统默认鼠标）
    返回值说明  :无
    ====================================================================*/
    virtual void OnUpdateSystemPointer( NodeID nInitiator ,
                                        u32 dwSystemPointerType ) = 0;

    /*====================================================================
    函数名      :OnUpdateColorPointer
    功能        :更新非系统鼠标，在鼠标缓存中写入新的鼠标形状
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,开始共享的节点ID
                 ptColorPointer,非系统鼠标形状信息
    返回值说明  :无
    ====================================================================*/
    virtual void OnUpdateColorPointer( NodeID nInitiator ,
                                       TAppShareColorPointer* ptColorPointer ) = 0;
    
    /*====================================================================
    函数名      :OnUpdateCachePointer
    功能        :更新非系统缓冲鼠标，表示使用该索引的鼠标
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,开始共享的节点ID
                 wCacheIndex,非系统缓冲鼠标索引
    返回值说明  :无
    ====================================================================*/
    virtual void OnUpdateCachePointer( NodeID nInitiator ,
                                       u16 wCacheIndex ) = 0;

    /*====================================================================
    函数名      :OnUpdatePointerPosition
    功能        :更新缓冲位置
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,开始共享的节点ID
                 wPositionX,鼠标横坐标
                 wPositionY,鼠标纵坐标
    返回值说明  :无
    ====================================================================*/
    virtual void OnUpdatePointerPosition( NodeID nInitiator ,
                                          u16 wPositionX ,
                                          u16 wPositionY ) = 0;
    
    /*====================================================================
    函数名      :OnEnableRC
    功能        :远程控制许可
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,开始共享的节点ID
                 bEnalbe,远程控制许可
    返回值说明  :无
    ====================================================================*/
    virtual void OnEnableRC( NodeID nInitiator ,
                             bool bEnable ) = 0;

    /*====================================================================
    函数名      :OnOtherRCing
    功能        :其他终端已经远程控制指定共享
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,开始共享的节点ID
                 nControlId,远程控制共享的节点ID
                 strName,当前控制终端别名
    返回值说明  :无
    ====================================================================*/
    virtual void OnOtherRCing( NodeID nInitiator ,
                               NodeID nControlId , 
                               const PString& strName ) = 0;

    /*====================================================================
    函数名      :OnRequestRC
    功能        :远端终端申请远程控制（立刻返回）
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,申请远程控制节点ID
                 strName,申请控制终端别名
                 wRequestIndex,申请终端索引
    返回值说明  :无
    ====================================================================*/
    virtual void OnRequestRC(NodeID nInitiator,
                             const PString& strNodeName ,
                             u16 wRequestIndex ) = 0;
    /*====================================================================
    函数名      :OnConfirmRC
    功能        :申请远程控制回复
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,开始共享的节点ID
                 wRequestIndex,请求远程控制的索引
                 wConfirmIndex,如果得到控制权限为远程控制索引（申请放弃控制权限需要）
                 enumConfirmType,回复结果
    返回值说明  :无
    ====================================================================*/
    virtual void OnConfirmRC( NodeID nInitiator ,
                              u16 wRequestIndex ,
                              u16 wConfirmIndex ,
                              ERcConfirmType enumConfirmType ) = 0;
    /*====================================================================
    函数名      :OnReleaseRC
    功能        :远端终端申请放弃远程控制（立刻返回）
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,申请远程控制节点ID
                 wRequestIndex,申请终端索引
                 wConfirmIndex,申请成功时得到的同意远程控制索引
    返回值说明  :无
    ====================================================================*/
    virtual void OnReleaseRC(NodeID nInitiator,
                            u16 wRequestIndex,
                            u16 wConfirmIndex) = 0;

    /*====================================================================
    函数名      :OnMouseMove
                 OnLMouseDown
                 OnLMouseUp
                 OnLMouseDoubleClick
                 OnRMouseDown
                 OnRMouseUp
                 OnRMouseDoubleClick
                 OnMiddleMouseDown
                 OnMiddleMouseUp
                 OnMiddleMouseDoubleClick
    功能        :远程控制鼠标操作
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,远程控制节点ID
                 nEventTime,事件相对时间（毫秒）
                 nPointX,鼠标位置逻辑横坐标
                 nPointY,鼠标位置逻辑纵坐标
    返回值说明  :无
    ====================================================================*/
    virtual void OnMouseMove(NodeID nInitiator,
                            s32 nEventTime,
                            s32 nPointX,
                            s32 nPointY ) = 0;
    virtual void OnLMouseDown(NodeID nInitiator,
                              s32 nEventTime,
                              s32 nPointX,
                              s32 nPointY ) = 0;
    virtual void OnLMouseUp(NodeID nInitiator,
                            s32 nEventTime,
                            s32 nPointX,
                            s32 nPointY ) = 0;
    virtual void OnLMouseDoubleClick(NodeID nInitiator,
                                    s32 nEventTime,
                                    s32 nPointX,
                                    s32 nPointY ) = 0;
    virtual void OnRMouseDown(NodeID nInitiator,
                                s32 nEventTime,
                                s32 nPointX,
                                s32 nPointY ) = 0;
    virtual void OnRMouseUp(NodeID nInitiator,
                            s32 nEventTime,
                            s32 nPointX,
                            s32 nPointY ) = 0;
    virtual void OnRMouseDoubleClick(NodeID nInitiator,
                                    s32 nEventTime,
                                    s32 nPointX,
                                    s32 nPointY ) = 0;
    virtual void OnMiddleMouseDown(NodeID nInitiator,
                                    s32 nEventTime,
                                    s32 nPointX,
                                    s32 nPointY ) = 0;
    virtual void OnMiddleMouseUp(NodeID nInitiator,
                                s32 nEventTime,
                                s32 nPointX,
                                s32 nPointY ) = 0;
    virtual void OnMiddleMouseDoubleClick(NodeID nInitiator,
                                            s32 nEventTime,
                                            s32 nPointX,
                                            s32 nPointY ) = 0;

    /*====================================================================
    函数名      :OnKeyDown
                 OnKeyUp
    功能        :远程控制键盘操作
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,远程控制节点ID
                 nEventTime,事件相对时间（毫秒）
                 byCodeType,按键编码方案：1：ASCII字符，2：键盘虚拟键
                 wCode,键值
    返回值说明  :无
    ====================================================================*/
    virtual void OnKeyDown(NodeID nInitiator,
                            s32 nEventTime,
                            u8 byCodeType,
                            u16 wCode) = 0;
    virtual void OnKeyUp(NodeID nInitiator,
                        s32 nEventTime,
                        u8 byCodeType,
                        u16 wCode) = 0;

    /*====================================================================
    函数名      :OnRCActivateHostedWindow
    功能        :远程控制切换Hosted活动窗口
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,开始共享的节点ID
                 nWindowID,活动窗口ID
    返回值说明  :无
    ====================================================================*/
    virtual void OnRCActivateHostedWindow( NodeID nInitiator,
                                           s32 nWindowID ) = 0;

    /*====================================================================
    函数名      :OnRCRestoreHostedWindow
    功能        :申请从标题栏中最大化Hosted窗口
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,开始共享的节点ID
                 nWindowID,活动窗口ID
    返回值说明  :无
    ====================================================================*/
    virtual void OnRCRestoreHostedWindow(NodeID nInitiator,
                                         s32 nWindowID ) = 0;
    

///////////////////////////////////////////////////////////////////////
//电子白板应用  -- 下行接口函数
///////////////////////////////////////////////////////////////////////  

    /*====================================================================
    函数名      :StartStillImage
    功能        :会议中启动电子白板应用
    算法实现    :无
    引用全局变量:无
    输入参数说明:bSupportText,是否支持文字
                 bSupport24Bitmap,是否支持24位真彩色图象
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool StartStillImage( bool bSupportText = true ,
                         bool bSupport24Bitmap = true );

    /*====================================================================
    函数名      :StopStillImage
    功能        :会议中停止电子白板应用
    算法实现    :无
    引用全局变量:无
    输入参数说明:
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool StopStillImage( );

    /*====================================================================
    函数名      :RefreshWorkspaceStatus
    功能        :刷新工作间状态
    算法实现    :无
    引用全局变量:无
    输入参数说明:bRefresh,一般为true
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool RefreshWorkspaceStatus( bool bRefresh = true );

    /*====================================================================
    函数名      :CreateWorkspace
    功能        :新建工作间
    算法实现    :无
    引用全局变量:无
    输入参数说明:tWorkspace,工作间结构信息
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool CreateWorkspace( const TStillImageWorkspace& tWorkspace );

    /*====================================================================
    函数名      :DeleteWorkspace
    功能        :删除工作间
    算法实现    :无
    引用全局变量:无
    输入参数说明:nWorkspaceHandle,工作间句柄
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool DeleteWorkspace( s32 nWorkspaceHandle );

    /*====================================================================
    函数名      :EditWorkspaceViewViewState
    功能        :修改工作间视图状态
    算法实现    :无
    引用全局变量:无
    输入参数说明:nWorkspaceHandle,工作间句柄
                 nViewHandle,视图句柄
                 enumViewState,视图状态
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool EditWorkspaceViewViewState( s32 nWorkspaceHandle ,
                                    s32 nViewHandle ,
                                    TStillImageViewParameter::ViewState enumViewState );

    /*====================================================================
    函数名      :EditWorkspaceViewUpdateEnabled
    功能        :修改工作间视图锁定状态
    算法实现    :无
    引用全局变量:无
    输入参数说明:nWorkspaceHandle,工作间句柄
                 nViewHandle,视图句柄
                 bUpdateEnabled,视图锁定状态
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool EditWorkspaceViewUpdateEnabled( s32 nWorkspaceHandle ,
                                        s32 nViewHandle ,
                                        bool bUpdateEnabled );

    /*====================================================================
    函数名      :CreateDrawing
    功能        :新建图形
    算法实现    :无
    引用全局变量:无
    输入参数说明:tDrawing,图形结构信息
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool CreateDrawing( const TStillImageDrawing& tDrawing );

    /*====================================================================
    函数名      :DeleteDrawing
    功能        :删除图形
    算法实现    :无
    引用全局变量:无
    输入参数说明:nDrawingHandle,图形句柄
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool DeleteDrawing( s32 nDrawingHandle );

    /*====================================================================
    函数名      :EditDrawingPenColor
    功能        :编辑图形笔颜色
    算法实现    :无
    引用全局变量:无
    输入参数说明:nDrawingHandle,图形句柄
                 tPenColor,笔颜色
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool EditDrawingPenColor( s32 nDrawingHandle ,
                             const TStillImageColorRGB& tPenColor );

    /*====================================================================
    函数名      :EditDrawingFillColor
    功能        :编辑图形填充色
    算法实现    :无
    引用全局变量:无
    输入参数说明:nDrawingHandle,图形句柄
                 tFillColor,填充色
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool EditDrawingFillColor( s32 nDrawingHandle ,
                              const TStillImageColorRGB& tFillColor );

    /*====================================================================
    函数名      :EditDrawingPenThickness
    功能        :编辑图形笔宽度
    算法实现    :无
    引用全局变量:无
    输入参数说明:nDrawingHandle,图形句柄
                 byPenThickness,笔宽度
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool EditDrawingPenThickness( s32 nDrawingHandle ,
                                 u8 byPenThickness );

    /*====================================================================
    函数名      :EditDrawingPenNib
    功能        :编辑图形笔形状
    算法实现    :无
    引用全局变量:无
    输入参数说明:nDrawingHandle,图形句柄
                 enumPenNib,笔形状
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool EditDrawingPenNib( s32 nDrawingHandle ,
                           TStillImageDrawing::PenNib enumPenNib );

    /*====================================================================
    函数名      :EditDrawingLineStyle
    功能        :编辑图形线形
    算法实现    :无
    引用全局变量:无
    输入参数说明:nDrawingHandle,图形句柄
                 enumLineStyle,线形
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool EditDrawingLineStyle( s32 nDrawingHandle ,
                              TStillImageDrawing::LineStyle enumLineStyle );

    /*====================================================================
    函数名      :EditDrawingHighlight
    功能        :编辑图形高亮显示
    算法实现    :无
    引用全局变量:
                 
    输入参数说明:nDrawingHandle,图形句柄
                 bHighlight,是否高亮显示
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool EditDrawingHighlight( s32 nDrawingHandle ,
                              bool bHighlight );

    /*====================================================================
    函数名      :EditDrawingViewState
    功能        :编辑图形视图状态
    算法实现    :无
    引用全局变量:
                 
    输入参数说明:nDrawingHandle,图形句柄
                 enumViewState,视图状态
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool EditDrawingViewState( s32 nDrawingHandle ,
                              TStillImageDrawing::ViewState enumViewState );

    /*====================================================================
    函数名      :EditDrawingZOrder
    功能        :编辑图形Z轴相对位置
    算法实现    :无
    引用全局变量:无
    输入参数说明:nDrawingHandle,图形句柄
                 enumZOrder,Z轴相对位置
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool EditDrawingZOrder( s32 nDrawingHandle ,
                           TStillImageDrawing::ZOrder enumZOrder );

    /*====================================================================
    函数名      :EditDrawingAnchorPoint
    功能        :移动图形
    算法实现    :无
    引用全局变量:无
    输入参数说明:nDrawingHandle,图形句柄
                 tAnchorPoint,图形基点坐标
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool EditDrawingAnchorPoint( s32 nDrawingHandle ,
                                const TStillImagePoint& tAnchorPoint );

    /*====================================================================
    函数名      :EditDrawingSubPoints
    功能        :编辑图形形状
    算法实现    :无
    引用全局变量:无
    输入参数说明:nDrawingHandle,图形句柄
                 nInitialIndex,初始索引
                 tInitialPoint,初始点坐标
                 nSubPointsNum,图形组合点数目
                 ptPoints,图形组合点集合
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool EditDrawingSubPoints( s32 nDrawingHandle ,
                              s32 nInitialIndex ,
                              const TStillImagePoint& tInitialPoint ,
                              s32 nSubPointsNum ,
                              const TStillImagePoint* ptPoints );

    /*====================================================================
    函数名      :CreateBitmap
    功能        :新建图片
    算法实现    :无
    引用全局变量:无
    输入参数说明:tBmp,图片结构信息
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool CreateBitmap( const TStillImageBitmap& tBmp );

    /*====================================================================
    函数名      :CreateBitmapPointer
    功能        :新建远程指示
    算法实现    :无
    引用全局变量:
                 
    输入参数说明:tBmp,图片结构信息
                 nTransparencyMaskLength,掩码长度
                 pbyTransparencyMask,掩码数据
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool CreateBitmapPointer( const TStillImageBitmap& tBmp ,
                            s32 nTransparencyMaskLength ,
                            const u8* pbyTransparencyMask );

    /*====================================================================
    函数名      :CreateBitmapContinue
    功能        :新建图片后续数据
    算法实现    :无
    引用全局变量:无
    输入参数说明:nBitmapHandle,图片句柄
                 bMoreToFollow,是否还有后续数据(为false表示图片数据已完整)
                 nBitmapDataLength,图片数据长度
                 pbyBitmapData,图片数据
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool CreateBitmapContinue( s32 nBitmapHandle ,
                              bool bMoreToFollow ,
                              s32 nBitmapDataLength ,
                              const u8* pbyBitmapData );

    /*====================================================================
    函数名      :DeleteBitmap
    功能        :删除图片
    算法实现    :无
    引用全局变量:无
    输入参数说明:nBitmapHandle,图片句柄
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool DeleteBitmap( s32 nBitmapHandle );

    /*====================================================================
    函数名      :EditBitmapViewState
    功能        :编辑图片视图状态
    算法实现    :无
    引用全局变量:无
    输入参数说明:nBitmapHandle,图片句柄
                 enumViewState,视图状态
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool EditBitmapViewState( s32 nBitmapHandle ,
                             TStillImageBitmap::ViewState enumViewState );

    /*====================================================================
    函数名      :EditBitmapZOrder
    功能        :编辑图片Z轴相对位置
    算法实现    :无
    引用全局变量:无
    输入参数说明:nBitmapHandle,图片句柄
                 enumZOrder,Z轴相对位置
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool EditBitmapZOrder( s32 nBitmapHandle ,
                          TStillImageBitmap::ZOrder enumZOrder );

    /*====================================================================
    函数名      :EditBitmapAnchorPoint
    功能        :移动图片位置
    算法实现    :无
    引用全局变量:无
    输入参数说明:nBitmapHandle,图片句柄
                 tAnchorPoint,图片基点坐标
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool EditBitmapAnchorPoint( s32 nBitmapHandle ,
                               const TStillImagePoint& tAnchorPoint );

    /*====================================================================
    函数名      :CreateText
    功能        :新建文字
    算法实现    :无
    引用全局变量:无
    输入参数说明:tText,文字结构信息
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool CreateText( const TStillImageText& tText );

    /*====================================================================
    函数名      :DeleteText
    功能        :删除文字
    算法实现    :无
    引用全局变量:无
    输入参数说明:nTextHandle,文字句柄
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool DeleteText( s32 nTextHandle );

    /*====================================================================
    函数名      :EditTextTextData
    功能        :改变文字内容
    算法实现    :无
    引用全局变量:无
    输入参数说明:nTextHandle,文字句柄
                 nTextDataLength,文字长度
                 pbyTextData,文字内容
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool EditTextTextData( s32 nTextHandle ,
                          s32 nTextDataLength ,
                          const u8* pbyTextData );

    /*====================================================================
    函数名      :EditTextTextStyle
    功能        :改变文字格式（编辑状态，但是非编辑状态也可使用）
    算法实现    :无
    引用全局变量:无
    输入参数说明:nTextHandle,文字句柄
                 tTextColor,文字颜色
                 tLogfont,文字字体
                 nTextDataLength,文字长度
                 pbyTextData,文字内容
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool EditTextTextStyle( s32 nTextHandle ,
                           const TStillImageColorRGB& tTextColor ,
                           const TStillImageLogfont& tLogfont ,
                           s32 nTextDataLength ,
                           const u8* pbyTextData );

    /*====================================================================
    函数名      :EditTextTextColor
    功能        :改变文字颜色（编辑状态，但是非编辑状态也可使用）
    算法实现    :无
    引用全局变量:无
    输入参数说明:nTextHandle,文字句柄
                 tTextColor,文字颜色
                 nTextDataLength,文字长度
                 pbyTextData,文字内容
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool EditTextTextColor( s32 nTextHandle ,
                           const TStillImageColorRGB& tTextColor ,
                           s32 nTextDataLength ,
                           const u8* pbyTextData );

    /*====================================================================
    函数名      :EditTextViewState
    功能        :编辑文字视图状态
    算法实现    :无
    引用全局变量:无
    输入参数说明:nTextHandle,文字句柄
                 enumViewState,视图状态
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool EditTextViewState( s32 nTextHandle ,
                           TStillImageText::ViewState enumViewState );

    /*====================================================================
    函数名      :EditTextZOrder
    功能        :编辑文字Z轴相对位置
    算法实现    :无
    引用全局变量:无
    输入参数说明:nTextHandle,文字句柄
                 enumZOrder,Z轴相对位置
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool EditTextZOrder( s32 nTextHandle ,
                        TStillImageText::ZOrder enumZOrder );

    /*====================================================================
    函数名      :EditTextAnchorPoint
    功能        :移动文字
    算法实现    :无
    引用全局变量:无
    输入参数说明:nTextHandle,文字句柄
                 tAnchorPoint,文字基点
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool EditTextAnchorPoint( s32 nTextHandle ,
                             const TStillImagePoint& tAnchorPoint );

///////////////////////////////////////////////////////////////////////
//电子白板应用  -- 上行响应函数
///////////////////////////////////////////////////////////////////////  
    /*====================================================================
    函数名      :OnStillImageCapabilityNegotiationSuccess
    功能        :能力级交换成功通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nParticipantsNum,会议中开启白板应用的终端数
                 pnParticipantsNodeList,开启白板应用的终端列表，一维数组
                 bSupportText,终端白板应用是否都支持文字
                 bSupport24Bitmap,终端白板应用是否都支持24位色真彩图片
    返回值说明  :无
    ====================================================================*/
    virtual void OnStillImageCapabilityNegotiationSuccess(s32 nParticipantsNum ,
                                                          NodeID *pnParticipantsNodeList,
                                                          bool bSupportText ,
                                                          bool bSupport24Bitmap ) = 0;
    
    /*====================================================================
    函数名      :OnWorkspaceRefreshStatus
    功能        :工作间状态刷新通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 bRefresh,一般为true
                 bLocalOperate,是否本终端发起的操作(如果为true,则可视为有新终端加入应用
                                上层需要将所有工作间及工作间内视图、元素等重新发送一遍)
    返回值说明  :无
    ====================================================================*/
    virtual void OnWorkspaceRefreshStatus( NodeID nInitiator ,
                                            bool bRefresh ,
                                            bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnWorkspaceCreate
    功能        :工作间新建通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 tWorkspace,工作间结构信息
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnWorkspaceCreate( NodeID nInitiator ,
                                    const TStillImageWorkspace& tWorkspace ,
                                    bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnWorkspaceDelete
    功能        :工作间删除通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 nWorkspaceHandle,工作间句柄
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnWorkspaceDelete( NodeID nInitiator ,
                                    s32 nWorkspaceHandle ,
                                    bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnWorkspaceEditCreateView
    功能        :工作间视图创建通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 nWorkspaceHandle,工作间句柄
                 tViewParameter,工作间视图结构信息
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnWorkspaceEditCreateView( NodeID nInitiator ,
                                            s32 nWorkspaceHandle ,
                                            const TStillImageViewParameter& tViewParameter ,
                                            bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnWorkspaceEditViewState
    功能        :工作间视图状态变化通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 nWorkspaceHandle,工作间句柄
                 nViewHandle,工作间视图句柄
                 enumViewState,工作间视图状态
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnWorkspaceEditViewState( NodeID nInitiator ,
                                           s32 nWorkspaceHandle ,
                                           s32 nViewHandle,
                                           TStillImageViewParameter::ViewState enumViewState ,
                                           bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnWorkspaceEditViewUpdateEnabled
    功能        :工作间视图锁定状态变化通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 nWorkspaceHandle,工作间句柄
                 nViewHandle,工作间视图句柄
                 bUpdatesEnabled,是否被锁定(为true,表示可刷新,未被锁定;为false,表示不可刷新,被锁定)
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnWorkspaceEditViewUpdateEnabled( NodeID nInitiator ,
                                                   s32 nWorkspaceHandle ,
                                                   s32 nViewHandle,
                                                   bool bUpdatesEnabled,
                                                   bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnDrawingCreate
    功能        :图形新建通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 tDrawing,图形结构信息
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnDrawingCreate( NodeID nInitiator ,
                                  const TStillImageDrawing& tDrawing ,
                                  bool bLocalOperate ) = 0;
    
    /*====================================================================
    函数名      :OnDrawingDelete
    功能        :图形删除通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 nDrawingHandle,图形句柄
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnDrawingDelete( NodeID nInitiator ,
                                  s32 nDrawingHandle ,
                                  bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnDrawingEditPenColor
    功能        :图形笔颜色变化通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 nDrawingHandle,图形句柄
                 tPenColor,笔颜色
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnDrawingEditPenColor( NodeID nInitiator ,
                                        s32 nDrawingHandle ,
                                        const TStillImageColorRGB& tPenColor ,
                                        bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnDrawingEditFillColor
    功能        :图形填充色变化通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 nDrawingHandle,图形句柄
                 tFillColor,填充色
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnDrawingEditFillColor( NodeID nInitiator ,
                                        s32 nDrawingHandle ,
                                        const TStillImageColorRGB& tFillColor ,
                                        bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnDrawingEditPenThickness
    功能        :图形笔宽度变化通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 nDrawingHandle,图形句柄
                 byPenThickness,笔宽度
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnDrawingEditPenThickness( NodeID nInitiator ,
                                            s32 nDrawingHandle ,
                                            u8 byPenThickness ,
                                            bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnDrawingEditPenNib
    功能        :图形笔尖形状变化通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 nDrawingHandle,图形句柄
                 enumPenNib,笔尖形状
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnDrawingEditPenNib( NodeID nInitiator ,
                                      s32 nDrawingHandle ,
                                      TStillImageDrawing::PenNib enumPenNib ,
                                      bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnDrawingEditLineStyle
    功能        :图形线形变化通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 nDrawingHandle,图形句柄
                 enumLineStyle,线形
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnDrawingEditLineStyle( NodeID nInitiator ,
                                        s32 nDrawingHandle ,
                                        TStillImageDrawing::LineStyle enumLineStyle ,
                                        bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnDrawingEditHighlight
    功能        :图形高亮显示变化通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 nDrawingHandle,图形句柄
                 bHighlight,是否高亮显示
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnDrawingEditHighlight( NodeID nInitiator ,
                                        s32 nDrawingHandle ,
                                        bool bHighlight ,
                                        bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnDrawingEditViewState
    功能        :图形视图状态变化通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 nDrawingHandle,图形句柄
                 enumViewState,图形视图状态
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnDrawingEditViewState( NodeID nInitiator ,
                                        s32 nDrawingHandle ,
                                        TStillImageDrawing::ViewState enumViewState ,
                                        bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnDrawingEditZOrder
    功能        :图形Z轴相对位置变化通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 nDrawingHandle,图形句柄
                 enumZOrder,图形Z轴相对位置
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnDrawingEditZOrder( NodeID nInitiator ,
                                      s32 nDrawingHandle ,
                                      TStillImageDrawing::ZOrder enumZOrder ,
                                      bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnDrawingEditAnchorPoint
    功能        :图形移动通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 nDrawingHandle,图形句柄
                 tAnchorPoint,图形第一个点的新座标
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnDrawingEditAnchorPoint( NodeID nInitiator ,
                                           s32 nDrawingHandle ,
                                           const TStillImagePoint& tAnchorPoint ,
                                           bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnDrawingEditSubPoints
    功能        :图形形状编辑通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 nDrawingHandle,图形句柄
                 nInitialIndex,初始索引
                 tInitialPoint,初始点坐标：是相对创建时m_tAnchorPoint的相对座标
                 nSubPointsNum,图形差值点数目
                 ptPoints,图形差值点集合：每一个点是相对于前一个点的相对座标
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnDrawingEditSubPoints( NodeID nInitiator ,
                                        s32 nDrawingHandle ,
                                        s32 nInitialIndex ,
                                        const TStillImagePoint& tInitialPoint ,
                                        s32 nSubPointsNum ,
                                        const TStillImagePoint* ptPoints ,
                                        bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnBitmapCreate
    功能        :图片新建通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 tBmp,图片结构信息
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnBitmapCreate( NodeID nInitiator ,
                                const TStillImageBitmap& tBmp ,
                                bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnBitmapPointerCreate
    功能        :远程指示新建通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 tBmp,图片结构信息
                 nTransparencyMaskLength,掩码长度
                 pbyTransparencyMask,掩码数据
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnBitmapPointerCreate( NodeID nInitiator ,
                                        const TStillImageBitmap& tBmp ,
                                        s32 nTransparencyMaskLength ,
                                        const u8* pbyTransparencyMask ,
                                        bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnBitmapContinue
    功能        :新建图片后续数据
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 nBitmapHandle,图片句柄
                 bMoreToFollow,是否还有后续数据(为false表示图片数据已完整)
                 nBitmapDataLength,图片数据长度
                 pbyBitmapData,图片数据
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnBitmapContinue( NodeID nInitiator ,
                                    s32 nBitmapHandle ,
                                    bool bMoreToFollow ,
                                    s32 nBitmapDataLength ,
                                    const u8* pbyBitmapData ,
                                    bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnBitmapDelete
    功能        :图片删除通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 nBitmapHandle,图片句柄
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnBitmapDelete( NodeID nInitiator ,
                                s32 nBitmapHandle ,
                                bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnBitmapEditViewState
    功能        :图片视图状态变化通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 nBitmapHandle,图片句柄
                 enumViewState,图片视图状态
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnBitmapEditViewState( NodeID nInitiator ,
                                        s32 nBitmapHandle ,
                                        TStillImageBitmap::ViewState enumViewState ,
                                        bool bLocalOperate ) = 0;
    
    /*====================================================================
    函数名      :OnBitmapEditZOrder
    功能        :图片Z轴相对位置变化通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 nBitmapHandle,图片句柄
                 enumZOrder,Z轴相对位置
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnBitmapEditZOrder( NodeID nInitiator ,
                                    s32 nBitmapHandle ,
                                    TStillImageBitmap::ZOrder enumZOrder ,
                                    bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnBitmapEditAnchorPoint
    功能        :图片移动通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 nBitmapHandle,图片句柄
                 tAnchorPoint,基点坐标
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnBitmapEditAnchorPoint( NodeID nInitiator ,
                                          s32 nBitmapHandle ,
                                          const TStillImagePoint& tAnchorPoint ,
                                          bool bLocalOperate ) = 0;
    
    /*====================================================================
    函数名      :OnTextCreate
    功能        :文字新建通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 tText,文字结构信息
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnTextCreate( NodeID nInitiator ,
                               const TStillImageText& tText ,
                               bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnTextDelete
    功能        :文字删除通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 nTextHandle,文字句柄
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnTextDelete( NodeID nInitiator ,
                               s32 nTextHandle ,
                               bool bLocalOperate ) = 0;
    
    /*====================================================================
    函数名      :OnTextEditTextData
    功能        :文字内容改变通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 nTextHandle,文字句柄
                 nTextDataLength,文字内容长度
                 pbyTextData,文字内容
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnTextEditTextData( NodeID nInitiator ,
                                    s32 nTextHandle ,
                                    s32 nTextDataLength ,
                                    const u8* pbyTextData ,
                                    bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnTextEditingTextStyle
    功能        :文字格式改变通知（编辑状态中）
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 nTextHandle,文字句柄
                 tTextColor,文字颜色
                 tLogfont,文字字体结构
                 nTextDataLength,文字内容长度
                 pbyTextData,文字内容
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnTextEditingTextStyle( NodeID nInitiator ,
                                        s32 nTextHandle ,
                                        const TStillImageColorRGB& tTextColor ,
                                        const TStillImageLogfont& tLogfont ,
                                        s32 nTextDataLength ,
                                        const u8* pbyTextData ,
                                        bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnTextEditingTextColor
    功能        :文字颜色改变通知（编辑状态中）
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 nTextHandle,文字句柄
                 tTextColor,文字颜色
                 nTextDataLength,文字内容长度
                 pbyTextData,文字内容
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnTextEditingTextColor( NodeID nInitiator ,
                                        s32 nTextHandle ,
                                        const TStillImageColorRGB& tTextColor ,
                                        s32 nTextDataLength ,
                                        const u8* pbyTextData ,
                                        bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnTextEditTextStyle
    功能        :文字格式改变通知（非编辑状态）
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 nTextHandle,文字句柄
                 tTextColor,文字颜色
                 tLogfont,文字字体（字体结构可能是全0，此时表示字体结构无效，仅颜色有效）
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnTextEditTextStyle( NodeID nInitiator ,
                                     s32 nTextHandle ,
                                     const TStillImageColorRGB& tTextColor ,
                                     const TStillImageLogfont& tLogfont ,
                                     bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnTextEditTextColor
    功能        :文字颜色改变通知（非编辑状态）
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 nTextHandle,文字句柄
                 tTextColor,文字颜色
                 tLogfont,文字字体
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnTextEditTextColor( NodeID nInitiator ,
                                      s32 nTextHandle ,
                                      const TStillImageColorRGB& tTextColor ,
                                      bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnTextEditViewState
    功能        :文字视图状态变化通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 nTextHandle,文字句柄
                 enumViewState,视图状态
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnTextEditViewState( NodeID nInitiator ,
                                      s32 nTextHandle ,
                                      TStillImageText::ViewState enumViewState ,
                                      bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnTextEditZOrder
    功能        :文字Z轴相对位置变化通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 nTextHandle,文字句柄
                 enumZOrder,Z轴相对位置
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnTextEditZOrder( NodeID nInitiator ,
                                   s32 nTextHandle ,
                                   TStillImageText::ZOrder enumZOrder ,
                                   bool bLocalOperate ) = 0;

    /*====================================================================
    函数名      :OnTextEditAnchorPoint
    功能        :文字移动通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发起操作的终端白板应用ID
                 nTextHandle,文字句柄
                 tAnchorPoint,文字基点
                 bLocalOperate,是否本终端发起的操作
    返回值说明  :无
    ====================================================================*/
    virtual void OnTextEditAnchorPoint( NodeID nInitiator ,
                                        s32 nTextHandle ,
                                        const TStillImagePoint& tAnchorPoint ,
                                        bool bLocalOperate ) = 0;
///////////////////////////////////////////////////////////////////////
// 文件传输应用 -- 下行接口函数
///////////////////////////////////////////////////////////////////////  

    /*====================================================================
    函数名      :StartMBFT
    功能        :会议中启动文件传输应用
    算法实现    :无
    引用全局变量:无
    输入参数说明:无
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool StartMBFT( );

    /*====================================================================
    函数名      :StopMBFT
    功能        :会议中停止文件传输应用
    算法实现    :无
    引用全局变量:
                 
    输入参数说明:无
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool StopMBFT( );

    /*====================================================================
    函数名      :ResponseToReceiveFile
    功能        :同意/拒绝接收别终端文件
    算法实现    :无
    引用全局变量:
                 
    输入参数说明:nInitiator,发送文件的终端ID
                 bConfirm,是否同意接收文件
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool ResponseToReceiveFile( NodeID nInitiator ,
                                bool bConfirm = true );

    /*====================================================================
    函数名      :AcceptFile
    功能        :能够/不能够接收别终端文件
    算法实现    :无
    引用全局变量:
                 
    输入参数说明:nInitiator,发送文件的终端ID
                 bConfirm,是否能够接收文件
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool AcceptFile( NodeID nInitiator ,
                     bool bConfirm = true );

    /*====================================================================
    函数名      :RejectFile
    功能        :中途停止接收别终端传输的文件
    算法实现    :无
    引用全局变量:无                 
    输入参数说明:nInitiator,发送文件的终端ID
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool RejectFile( NodeID nInitiator );

    /*====================================================================
    函数名      :InviteToReceiveFile
    功能        :邀请别终端接收文件
    算法实现    :无
    引用全局变量:无
    输入参数说明:pnMBFTUserIds,邀请接收文件的终端ID
                 nMBFTUserIds,邀请接收文件的终端总数
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool InviteToReceiveFile( NodeID* pnMBFTUserIds ,
                              s32 nMBFTUserIds );

    /*====================================================================
    函数名      :OfferFile
    功能        :提供文件格式
    算法实现    :无
    引用全局变量:无                 
    输入参数说明:pchFileName,文件名
                 tTimeOfLastModification,最近文件修改时间
                 nFileSize,文件长度
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool OfferFile( const char* pchFileName ,
                    const PTime& tTimeOfLastModification ,
                    s32 nFileSize );

    /*====================================================================
    函数名      :StartTransferFile
    功能        :开始传输文件
    算法实现    :无
    引用全局变量:无
    输入参数说明:pchFileName,文件名
                 tTimeOfLastModification,最近文件修改时间
                 nFileSize,文件长度
                 bFileEnd,文件传输是否完成
                 nFileDataLength,本次传输文件长度
                 pbyFileData,本次传输文件内容
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool StartTransferFile( const char* pchFileName ,
                            const PTime& tTimeOfLastModification ,
                            s32 nFileSize ,
                            bool bFileEnd ,
                            s32 nFileDataLength ,
                            u8* pbyFileData );

    /*====================================================================
    函数名      :TransferFileContinue
    功能        :继续传输文件
    算法实现    :无
    引用全局变量:无
    输入参数说明:bFileEnd,文件传输是否完成
                 nFileDataLength,本次传输文件长度
                 pbyFileData,本次传输文件内容
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool TransferFileContinue( bool bFileEnd ,
                               s32 nFileDataLength ,
                               u8* pbyFileData );

    /*====================================================================
    函数名      :StopTransferFile
    功能        :中途停止传输文件
    算法实现    :无
    引用全局变量:无
    输入参数说明:无
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool StopTransferFile();

    /*====================================================================
    函数名      :SetMbftTimeOut
    功能        :设置传输文件传输的超时时间
    算法实现    :无
    引用全局变量:无
    输入参数说明:dwTimeOut:超时时间（毫秒）
                 必须大于1000，否则返回失败
                 默认超时时间为30秒
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool SetMbftTimeOut(DWORD dwTimeOut);
/////////////////////////////////////////////////////////////////////////////////////
// 文件传输应用  -- 上行响应函数
/////////////////////////////////////////////////////////////////////////////////////

    /*====================================================================
    函数名      :OnMBFTCapabilityNegotiationSuccess
    功能        :能力级交换成功通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:pnMBFTUserIds,可以发送文件的终端ID(包括本终端)
                 nMBFTUserNum,可以发送文件的终端总数
    返回值说明  :无
    ====================================================================*/
    virtual void OnMBFTCapabilityNegotiationSuccess( NodeID *pnMBFTUserIds ,
                                                     s32 nMBFTUserNum ) = 0;

    /*====================================================================
    函数名      :OnInviteToReceiveFile
    功能        :别终端邀请接收文件
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发送文件的终端ID
    返回值说明  :无
    ====================================================================*/
    virtual void OnInviteToReceiveFile( NodeID nInitiator ) = 0;

    /*====================================================================
    函数名      :OnOfferFile
    功能        :别终端提供文件格式
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发送文件的终端ID
                 strFileName,文件名
                 tTimeOfLastModification,最近文件修改时间
                 nFileSize,文件长度
    返回值说明  :无
    ====================================================================*/
    virtual void OnOfferFile( NodeID nInitiator ,
                              const PString& strFileName ,
                              const PTime& tTimeOfLastModification ,
                              s32 nFileSize ) = 0;

    /*====================================================================
    函数名      :OnStartTransferFile
    功能        :别终端开始传输文件
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发送文件的终端ID
                 strFileName,文件名
                 tTimeOfLastModification,最近文件修改时间
                 nFileSize,文件长度
                 bFileEnd,传输文件是否完成
                 nFileDataLength,本次传输数据长度
                 pbyFileData,本传输数据
    返回值说明  :无
    ====================================================================*/
    virtual void OnStartTransferFile( NodeID nInitiator ,
                                      const PString& strFileName ,
                                      const PTime& tTimeLastModification ,
                                      s32 nFileSize ,
                                      bool bFileEnd ,
                                      s32 nFileDataLength ,
                                      u8* pbyFileData ) = 0;

    /*====================================================================
    函数名      :OnTransferFileContinue
    功能        :别终端继续传输文件
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发送文件的终端ID
                 bFileEnd,传输文件是否完成
                 nFileDataLength,本次传输数据长度
                 pbyFileData,本传输数据
    返回值说明  :无
    ====================================================================*/
    virtual void OnTransferFileContinue( NodeID nInitiator ,
                                        bool bFileEnd ,
                                        s32 nFileDataLength ,
                                        u8* pbyFileData ) = 0;

    /*====================================================================
    函数名      :OnStopTransferFile
    功能        :别终端中途停止传输文件
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发送文件的终端ID
    返回值说明  :无
    ====================================================================*/
    virtual void OnStopTransferFile( NodeID nInitiator ) = 0;

    /*====================================================================
    函数名      :OnResponseToReceiveFile
    功能        :已收到被邀请接收文件终端的回复
    算法实现    :无
    引用全局变量:无
    输入参数说明:nResponseUserNum,当前接受文件的终端总数(为0表示已无终端接受)
    返回值说明  :无
    ====================================================================*/
    virtual void OnResponseToReceiveFile( s32 nResponseUserNum ) = 0;

    /*====================================================================
    函数名      :OnAcceptFile
    功能        :别终端文件格式回复
    算法实现    :无
    引用全局变量:无
    输入参数说明:nAcceptFileUserNum,当前接受文件的终端总数(为0表示已无终端接受)
    返回值说明  :无
    ====================================================================*/
    virtual void OnAcceptFile( s32 nAcceptFileUserNum ) = 0;

    /*====================================================================
    函数名      :OnRejectFile
    功能        :传输文件中途终端停止接收文件
    算法实现    :无
    引用全局变量:无
    输入参数说明:nReceivingFileUserNum,当前接受文件的终端总数(为0表示已无终端接受)
    返回值说明  :无
    ====================================================================*/
    virtual void OnRejectFile( s32 nReceivingFileUserNum ) = 0;

    /*====================================================================
    函数名      :OnTransferFileEnd
    功能        :传输文件完成
    算法实现    :无
    引用全局变量:无
    输入参数说明:无
    返回值说明  :无
    ====================================================================*/
    virtual void OnTransferFileEnd() = 0;

///////////////////////////////////////////////////////////////////////
// 聊天应用  -- 下行接口函数
///////////////////////////////////////////////////////////////////////  
 
    /*====================================================================
    函数名      :StartChat
    功能        :会议中启动聊天应用
    算法实现    :无
    引用全局变量:无
    输入参数说明:无
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool StartChat( );

    /*====================================================================
    函数名      :StopChat
    功能        :会议中停止聊天应用
    算法实现    :无
    引用全局变量:无
    输入参数说明:无
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool StopChat( );

    /*====================================================================
    函数名      :SendText
    功能        :发送公聊信息
    算法实现    :无
    引用全局变量:无
    输入参数说明:nTextLength,聊天信息长度
                 pbyText,聊天信息内容
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool SendText( s32 nTextLength , u8* pbyText );

    /*====================================================================
    函数名      :SendPrivateText
    功能        :发送私聊信息
    算法实现    :无
    引用全局变量:无
    输入参数说明:nPrivateUserId,目的终端ID
                 nTextLength,聊天信息长度
                 pbyText,聊天信息内容
    返回值说明  :true : 命令执行成功
                 false: 命令执行失败
    ====================================================================*/
    bool SendPrivateText( NodeID nPrivateUserId ,
                          s32 nTextLength ,
                          u8* pbyText );

/////////////////////////////////////////////////////////////////////////////////////
// 聊天应用  -- 上行响应函数
/////////////////////////////////////////////////////////////////////////////////////

    /*====================================================================
    函数名      :OnChatCapabilityNegotiationSuccess
    功能        :能力级交换成功通知
    算法实现    :无
    引用全局变量:无
    输入参数说明:pnChatUserIds,可以聊天的终端ID(包括本终端)
                 nChatUserNum,可以聊天的终端总数
    返回值说明  :无
    ====================================================================*/
    virtual void OnChatCapabilityNegotiationSuccess( NodeID *pnChatUserIds ,
                                                    s32 nChatUserNum ) = 0;

    /*====================================================================
    函数名      :OnReceivedText
    功能        :接收到公聊消息
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发送聊天信息的终端ID
                 nTextLength,聊天信息长度
                 pbyText,聊天信息内容
    返回值说明  :无
    ====================================================================*/
	virtual void OnReceivedText( NodeID nInitiator ,
                                 s32 nTextLength ,
                                 u8* pbyText ) = 0;

    /*====================================================================
    函数名      :OnReceivedPrivateText
    功能        :接收到私聊消息
    算法实现    :无
    引用全局变量:无
    输入参数说明:nInitiator,发送聊天信息的终端ID
                 nTextLength,聊天信息长度
                 pbyText,聊天信息内容
    返回值说明  :无
    ====================================================================*/
	virtual void OnReceivedPrivateText( NodeID nInitiator ,
                                        s32 nTextLength ,
                                        u8* pbyText ) = 0;


///////////////////////////////////////////////////////////////////////
// 测试查询  -- 测试打印内部信息
///////////////////////////////////////////////////////////////////////  
 
    /*====================================================================
    函数名      :PrintStatus
    功能        :打印当前DcMtLib的状态
    算法实现    :无
    引用全局变量:无
    输入参数说明:无
    返回值说明  :无
    ====================================================================*/
    bool PrintStatus();
};

#endif//_DCMTADAPTER_H