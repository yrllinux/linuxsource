/*****************************************************************************
模块名      : DCMT控件
文件名      : DCMTDefine.h
相关文件    : 
文件实现功能:  定义DCMTOcx控件要用的常量,宏,消息号,结构等
作者        : 陈洪斌 胡小鹏
版本        : V3.0  Copyright(C) 2004-2005 KDC, All rights reserved.

-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2004/12/21  3.0         陈洪斌      创建
******************************************************************************/

#ifndef DCMTDEFINE_H
#define DCMTDEFINE_H 

//语言切换通知消息(消息体: 切换前的语言类型, 保留)
#define WM_SWITCH_LANGUAGE         WM_USER + 11

//邀请入会请求 (消息体: 邀请加入的终端名称 CString* , 请求终端标示 long )
#define WM_INVITE_REQUEST          WM_USER + 400
//申请入会请求 (消息体: 邀请加入的终端名称 CString* , 请求终端标示 long)
#define WM_JOIN_REQUEST            WM_USER + 401  
//邀请入会取消 (无消息体)
#define WM_INVITE_REQUEST_CANCEL   WM_USER + 402
//会议增加一个终端 (消息体: 终端信息 TDcMtInfo*, 保留)
#define WM_CONF_ADD_ONE_MT          WM_USER + 403
//会议删除一个终端 (消息体: 终端ID int, 保留)
#define WM_CONF_REMOVE_ONE_MT       WM_USER + 404
//创建会议成功 (消息体: 是否本地创建的会议会议 BOOL, 保留)
#define WM_CONF_CREATE_SUCCESS      WM_USER + 405
//会议结束 (无消息体)
#define WM_CONF_TERMINATE           WM_USER + 406
//挂断会议 (无消息体)
#define WM_CONF_HANGUP              WM_USER + 407
//通知会议不能发起的应用(消息体: 不能发起应用的标示 enum, 保留)
#define WM_CONF_APP_DISABLE         WM_USER + 408

//共享能力集交换成功 (无消息体)
#define WM_CAPABILITY_NEGOTIATION_SUCCESS       WM_USER + 410

//更新聊天终端通知 (消息体: 参加聊天终端总数 int, 所有终端数组 TDcMtInfo*)
#define WM_CHAT_MT_UPDATE   WM_USER + 412

//邀请终端响应 (消息体: 是否邀请成功 BOOL, 保留)
#define WM_INVITE_MT_RSP            WM_USER + 500
//加入会议响应 (消息体: 是否加入成功 BOOL, 保留)
#define WM_JOIN_CONF_RSP            WM_USER + 501
//加入会议密码错误 (消息体: 会议名称 CString* , 保留)
#define WM_JOIN_CONF_PASSWORD_ERROR WM_USER + 502
//呼叫失败，双方都在会议中 (无消息体)
#define WM_CALL_OUT_FAILED    WM_USER + 503
#define WM_CALL_BOTH_IN_CONF  WM_CALL_OUT_FAILED

//呼叫，对方有多个会议存在 (消息体: 多个会议名称列表 CStringList* , 会议名称个数)
#define WM_CALL_OUT_MORE_THAN_ONE_CONF   WM_USER + 504

//启动桌面共享 (消息体: 启动共享者ID int , 保留)
#define WM_START_DESKTOP_SHARE      WM_USER + 600
//启动应用共享 (消息体: 启动共享者ID int , 保留)
#define WM_START_APP_SHARE          WM_USER + 601
//停止共享 (消息体: 启动共享者ID int , 保留)
#define WM_STOP_APP_SHARE           WM_USER + 602

//更新创口列表 (无消息体)
#define WM_UPDATE_WINDOW_LIST       WM_USER + 610
//申请远程控制权回复 (消息体: 远程控制索引 WORD , 是否得到控制权限，或者拒绝的原因 RcConfirmType)
#define WM_CONFIRM_REMOTE_CONTROL   WM_USER + 611
//其他终端获得远程控制权 (消息体: 控制者ID int , 保留)
#define WM_OTHER_REMOTE_CONTROL     WM_USER + 612

//更新鼠标形状,该消息只发生在控制其他终端的共享程序的时候
//(无消息体)
#define WM_UPDATE_CURSOR_SHAPE      WM_USER + 615
//其他终端申请请远程控制权 (无消息体)
#define WM_REQUEST_REMOTE_CONTRL    WM_USER + 620
//其他终端释放远程控制权 (无消息体)
#define WM_RELEASE_REMOTE_CONTRL    WM_USER + 621
//收到远程控制本地窗口恢复活动 (消息体: 共享（或者非共享）窗口的ID int , 保留)
#define WM_RC_RECEIVE_ACTIVE_WINDOW WM_USER + 630
//收到远程控制本地窗口激活 (消息体: 共享窗口的ID int , 保留)
#define WM_RC_ACTIVE_WINDOW         WM_USER + 640
//切换显示共享窗口
#define WM_SWITH_SHOW_SHARE_WINDOW         WM_USER + 641

//收到公聊信息 (消息体: 聊天信息结构 TChatReceiveInfo* , 保留)
#define WM_RECEIVE_CHAT_PUBLIC_TEXT        WM_USER + 650
//收到私聊信息 (消息体: 聊天信息结构 TChatReceiveInfo* , 保留)
#define WM_RECEIVE_CHAT_PRIVATE_TEXT WM_USER + 651
//其他终端启动聊天应用的通知 (消息体: 启动聊天终端ID int , 保留)
#define WM_OTHER_START_CHAT         WM_USER + 660
//其他终端启动文件传输应用的通知 (消息体: 启动文件传输终端ID int , 保留)
#define WM_OTHER_START_MBFT         WM_USER + 661
//其他终端启动白板应用的通知 (消息体: 启动白板终端ID int , 保留)
#define WM_OTHER_START_SI         WM_USER + 662

//枚举桌面窗口 (消息体: 当前枚举的窗口 HWND , 保留)
#define WM_ENUM_DESKTOP_WINDOWS			    WM_USER + 690
//枚举窗口 (消息体: 当前枚举的窗口 HWND , 保留)
#define WM_ENUM_WINDOWS			    WM_USER + 700
//加入/退出会议 (消息体: 是否在会议中（是否加入会议） BOOL , 保留)
#define WM_CONF_IN_OUT              WM_USER + 701

//开始发送文件 (无消息体)
#define WM_START_SEND_FILE          WM_USER + 740
//开始接收文件 (消息体: 发送文件的终端ID int , 文件大小 int)
#define WM_START_RECEIVE_FILE       WM_USER + 750
//开始接收文件数据 (消息体: 文件数据长度 int , 文件数据 BYTE*)
//如果发送给CDlgReceiveFile （消息体:  文件是否结束 bool,文件数据长度 int ）
#define WM_RECEIVE_FILE_DATA        WM_USER + 751
//接收的文件结束
#define WM_RECEIVE_FILE_FINISH      WM_USER + 752
//接收文件中断
#define WM_RECEIVE_FILE_TERMINATE   WM_USER + 753


//非系统鼠标数量
const BYTE POINTER_CACHE_SIZE = 25;
//会议中最大终端数
const WORD MAX_CONFERENCE_MT_NUM = 192;

//呼叫响应类型
typedef enum
{
    CallRspUserDecide = 0,  //上层用户决定是否接受呼叫
    CallRspAutoAccept,      //自动接受呼叫
    CallRspRefuse,          //拒绝接受任何呼叫(请勿打扰)
}ECallRsp;

//终端不能发起的应用
typedef enum
{
    AppShareDisable = 1,    //不能发起共享
    AppWhiteboardDisable,   //不能发起白板
    AppFileTransDisable,    //不能发起文件传输
    AppChatDisable          //不能发起聊天
}AppDisable;

//节点(这里指终端)类型
typedef enum
{
	CNPSelf,                //终端自身
	CNPLocal,               //本地终端
	CNPRemote               //远程终端
} ConfNodePos;


//终端信息结构
struct TDcMtInfo
{
    int     nMtID;          //终端号
    ConfNodePos NodePos;    //节点位置
    CString strMtName;      //终端名称
    CString strMtEmail;     //终端E-mail
    CString strMtPhone;     //终端电话
    CString strMtLocation;  //终端位置
    CString strFrom;        //源地址
};

//共享基本信息结构
struct TAppShareBaseData
{
	int		nShareID;               //共享ID
	CString strShareName;           //共享终端名称
	WORD    wDesktopWidth;          //共享者桌面宽度
	WORD    wDesktopHeight;         //共享者桌面高度
};

//光标类型
enum EPointerType
{
    POINTER_NULL,           //没有光标
    POINTER_SYSTEM,         //系统光标
    POINTER_NO_SYSTEM,      //非系统光标
};

//位图信息结构
struct TBitmapData
{
    BITMAPINFOHEADER m_BitmapInfoHeader;    //位图信息头
    BYTE    m_byPalette[4*256];             //调色板数据
    BYTE*   m_pbyBitmapData;                //位图数据
    TBitmapData::TBitmapData()
    {
        m_BitmapInfoHeader.biSize = 0x00000028;
        m_BitmapInfoHeader.biWidth = 0x00000000;
        m_BitmapInfoHeader.biHeight = 0x00000000;
        m_BitmapInfoHeader.biPlanes = 0x0001;
        m_BitmapInfoHeader.biBitCount = 0x0000;
        m_BitmapInfoHeader.biCompression = 0x00000000;
        m_BitmapInfoHeader.biSizeImage = 0x00000000;
        m_BitmapInfoHeader.biXPelsPerMeter = 0x00000000;
        m_BitmapInfoHeader.biYPelsPerMeter = 0x00000000;
        m_BitmapInfoHeader.biClrUsed = 0x00000000;
        m_BitmapInfoHeader.biClrImportant = 0x00000000;
    
        memset( m_byPalette , 0xf8 , 4*256);
        m_pbyBitmapData = NULL;
    };

    TBitmapData::~TBitmapData()
    {
        if(m_pbyBitmapData != NULL)
        {
            delete [] m_pbyBitmapData;
            m_pbyBitmapData = NULL;
        }
    };
};


//接收聊天信息结构
struct TChatReceiveInfo
{
    TChatReceiveInfo::TChatReceiveInfo()
    {
        nUserID = 0;
        nTextLen = 0;
        strText = _T("");
    };
    int nUserID;        //终端ID
    int nTextLen;       //聊天信息长度
    CString strText;    //聊天信息;

};


//////////////////////////////////////////////////////////////////////////
// 以下是测试所用到的定义

#define DCMT_TEST
#ifdef DCMT_TEST

#define DCMT_TEST_TELNET_PORT 2300
#define DCMT_TEST_LISTEN_PORT 2301

#define DCMT_TEST_APP_ID 235 

//查询是否与会
#define EV_DCMT_TEST_IS_INCONF_QUERY            EV_TEST_DCMT_BGN + 1
//查询是否启动了桌面共享
#define EV_DCMT_TEST_IS_START_DESKTOP_SHARE     EV_TEST_DCMT_BGN + 2
//查询是否启动了应用程序共享
#define EV_DCMT_TEST_IS_START_APP_SHARE         EV_TEST_DCMT_BGN + 3
//查询终端接收到的共享总数
#define EV_DCMT_TEST_GET_RECEIVE_SHARE_COUNT    EV_TEST_DCMT_BGN + 4
//第一个接收共享ID
#define EV_DCMT_TEST_GET_FIRST_RECEIVE_SHARE_ID EV_TEST_DCMT_BGN + 5
//查询是否允许远程控制
#define EV_DCMT_TEST_IS_ENABLE_REMOTE_CONTROL   EV_TEST_DCMT_BGN + 6
//获取发送绘图对象测试数据消息
#define EV_DCMT_TEST_GET_DRAWOBJECT_SEND        EV_TEST_DCMT_BGN + 7
//获取收到绘图对象测试数据消息
#define EV_DCMT_TEST_GET_DRAWOBJECT_RECEIVE     EV_TEST_DCMT_BGN + 8
//远程启动桌面共享
#define EV_DCMT_TEST_START_DESKTOP_SHARE        EV_TEST_DCMT_BGN + 9
//远程启动第一个应用程序共享
#define EV_DCMT_TEST_START_FIRST_APP_SHARE      EV_TEST_DCMT_BGN + 10
//远程结束所有的共享
#define EV_DCMT_TEST_STOP_ALL_SHARE             EV_TEST_DCMT_BGN + 11
//允许控制
#define EV_DCMT_TEST_SET_ALLOW_REMOTE_CONTROL   EV_TEST_DCMT_BGN + 12

//查询是否启动了白板
#define EV_DCMT_TEST_IS_START_WHITEBOARD        EV_TEST_DCMT_BGN + 20
//获取当前工作区绘图对象个数消息
#define EV_DCMT_TEST_GET_DRAWOBJECT_COUNT       EV_TEST_DCMT_BGN + 22
//查询白板当前工作区顶层画图对象的类型
#define EV_DCMT_TEST_GET_DRAWOBJECT_TYPE_TOP    EV_TEST_DCMT_BGN + 23
//查询白板当前工作区底层画图对象的类型
#define EV_DCMT_TEST_GET_DRAWOBJECT_TYPE_BOTTOM    EV_TEST_DCMT_BGN + 24
//查询白板当前工作区顶层画图对象的颜色
#define EV_DCMT_TEST_GET_DRAWOBJECT_COLOR_TOP    EV_TEST_DCMT_BGN + 25
//查询白板当前工作区顶层画图对象的笔宽
#define EV_DCMT_TEST_GET_DRAWOBJECT_PENWIDTH_TOP    EV_TEST_DCMT_BGN + 26
//查询获取白板当前工作区顶层画图对象的起始点(左上角坐标)
#define EV_DCMT_TEST_GET_DRAWOBJECT_ANCHOR_POINT_TOP    EV_TEST_DCMT_BGN + 27

//获取当前工作区绘图选中对象个数消息
#define EV_DCMT_TEST_GET_SELECTOBJECT_COUNT       EV_TEST_DCMT_BGN + 28
//查询白板当前工作区顶层选中画图对象的类型
#define EV_DCMT_TEST_GET_SELECTOBJECT_TYPE_TOP    EV_TEST_DCMT_BGN + 29
//查询白板当前工作区顶层选中画图对象的颜色
#define EV_DCMT_TEST_GET_SELECTOBJECT_COLOR_TOP    EV_TEST_DCMT_BGN + 30
//查询白板当前工作区顶层选中画图对象的笔宽
#define EV_DCMT_TEST_GET_SELECTOBJECT_PENWIDTH_TOP    EV_TEST_DCMT_BGN + 31
////查询白板本地指示器是否显示
//#define EV_DCMT_TEST_IS_LOCAL_POINTER_SHOW    EV_TEST_DCMT_BGN + 32
//查询白板显示指示器的总数
#define EV_DCMT_TEST_GET_SHOW_POINTER_NUMBER    EV_TEST_DCMT_BGN + 33
//获取指示器位置消息
#define EV_DCMT_TEST_GET_POINTER_POSITION       EV_TEST_DCMT_BGN + 34

//获取工作区数量
#define EV_DCMT_TEST_GET_WORKSPACE_COUNT        EV_TEST_DCMT_BGN + 40
//获取工作区索引消息
#define EV_DCMT_TEST_GET_WORKSPACE_INDEX        EV_TEST_DCMT_BGN + 41

#endif

//////////////////////////////////////////////////////////////////////////

#endif DCMTDEFINE_H
