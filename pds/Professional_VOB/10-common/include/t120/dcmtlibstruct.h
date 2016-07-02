/*****************************************************************************
模块名      : DcMtLib
文件名      : dcmtlibstruct.h
相关文件    : dcmtlibstruct.cpp
文件实现功能: dcmtlib结构定义
作者		: chenhongbin
版本        : V1.0
-----------------------------------------------------------------------------
修改记录:
日  期      版本     修改人             修改内容
2005/05/24  1.0      chenhongbin         创建
******************************************************************************/
#ifndef _DCMTLIBSTRUCT_H
#define _DCMTLIBSTRUCT_H

#include"kdvtype.h"
#include "ptlib.h"

#ifdef _WIN32
#pragma pack( push )
#pragma pack( 1 )
#endif

typedef u32 NodeID; //节点ID
typedef s32 ChannelId;
typedef s32 StaticChannelId;
typedef s32 DynamicChannelId;

//#define ENABLE_DCMTLIB_LOG
/*====================================================================
// 打印日志 宏定义
=====================================================================*/
#ifdef ENABLE_DCMTLIB_LOG
    #define DCMTLIB_LOG(format, para1, para2, para3)\
        { \
            CHAR anBuf[1024]; \
	        sprintf(anBuf, ".\\T120_LOG\\%ld_%ld.log", GetCurrentProcessId(), GetCurrentThreadId()); \
            HANDLE handleFile = CreateFile(anBuf, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); \
            if (handleFile != NULL) \
            { \
                SetFilePointer(handleFile, NULL, NULL, FILE_END); \
                DWORD dwRcv; \
                SYSTEMTIME tSystime; \
                GetLocalTime(&tSystime); \
                CHAR anFormat[256]; \
                sprintf(anFormat, "[%2.2d:%2.2d:%2.2d:%3.3d] DCMTLIB: %s\r\n", tSystime.wHour, tSystime.wMinute, tSystime.wSecond, tSystime.wMilliseconds, format); \
                sprintf(anBuf, anFormat, para1, para2, para3); \
                WriteFile(handleFile, anBuf, strlen(anBuf), &dwRcv, NULL); \
                CloseHandle(handleFile); \
            } \
        }

    #define DCMTLIB_LOG_DEFAULT(format)\
        { \
            CHAR anBuf[1024]; \
	        sprintf(anBuf, ".\\T120_LOG\\%ld_%ld.log", GetCurrentProcessId(), GetCurrentThreadId()); \
            HANDLE handleFile = CreateFile(anBuf, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); \
            if (handleFile != NULL) \
            { \
                SetFilePointer(handleFile, NULL, NULL, FILE_END); \
                DWORD dwRcv; \
                SYSTEMTIME tSystime; \
                GetLocalTime(&tSystime); \
                CHAR anFormat[256]; \
                sprintf(anFormat, "[%2.2d:%2.2d:%2.2d:%3.3d] DCMTLIB: %s\r\n", tSystime.wHour, tSystime.wMinute, tSystime.wSecond, tSystime.wMilliseconds, format); \
                sprintf(anBuf, anFormat); \
                WriteFile(handleFile, anBuf, strlen(anBuf), &dwRcv, NULL); \
                CloseHandle(handleFile); \
            } \
        }
#else
    #define DCMTLIB_LOG(format, para1, para2, para3)
    #define DCMTLIB_LOG_DEFAULT(format)
#endif

//申请远程控制回复
enum ERcConfirmType
{
    e_ConfirmSuccuss,          // 0 表示“确认”或“自动接收控制请求”
    e_ConfirmUnexpected,       // 1 未使用
    e_ConfirmAlwaysRefused,    // 2 表示“现在不接受控制请求”
    e_ConfirmRefused,          // 3 表示“拒绝”
    e_ConfirmForbidden         // 4 表示 Disable RC
};

//终端不能发起的应用
typedef enum
{
    e_AppShareDisable = 0x0a,        //不能发起共享
    e_AppWhiteboardDisable = 0x28,   //不能发起白板
    e_AppFileTransDisable = 0x49,    //不能发起文件传输
    e_AppChatDisable = 0x18,         //不能发起聊天
    e_AppAllAllowed = 0x08,          //能发起所有应用
    e_AppAllDisable = 0x7b,          //不能发起所有应用
}EAppDisable;

//会议响应结果
typedef enum 
{
	e_CrSuccess,
	e_CrUserRejected,
	e_CrResourcesNotAvaiable,
	e_CrRejectedForSymmetryBreaking,
	e_CrLockedConferenceNotSupported,
	e_CrInvalidConference,
	e_CrInvalidPassword,
	e_CrInvalidConvenerPassword,
	e_CrChallengeResponseRequired,
	e_CrInvalidChallengeResponse
} EConferenceResponseResult;

//节点类型
typedef enum 
{
	e_NtTerminal		= 0,
	e_NtMultipointTerminal	= 1,
	e_NtMCU			= 2
} ENodeType;

//踢出用户原因
typedef enum 
{
	e_UserInitiated,
	e_HigherNodeDisconnected,
	e_NodeEjected
} EEjectUserReason;

//踢出用户结果
typedef enum 
{
	e_EuSuccess,
	e_EuInvalidRequester,
	e_EuInvalidNode
} EEjectUserResult;

//入会的节点位置
typedef enum
{
	e_CNPSelf,
	e_CNPLocal,
	e_CNPRemote
} EConfNodePos;

//会议结束原因
typedef enum 
{
	e_CtUserInitiated,
	e_CtTimedConference
} EConferenceTerminateReason;

//节点信息结构
struct TNodeInfo
{
    PString m_strNodeIp;        //节点Ip
    PString m_strNodeAlias;     //节点别名
    PString m_strNodeEmail;     //节点E-mail
    PString m_strNodeLocation;  //节点位置
    PString m_strNodePhone;     //节点电话
    u16     m_wNodePort;        //节点的端口
};

#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// 23 窗口需要在接收端标题栏中列出
#define WINDOW_FLAG_TAGGABLE    0x00800000L
// 7 窗口是否最小化
#define WINDOW_FLAG_MINIMIZED   0x00000080L
// 5 窗口是否共享应用的窗口
#define WINDOW_FLAG_HOSTED      0x00000020L
// 3 窗口是否遮盖共享应用的本地非共享应用窗口
#define WINDOW_FLAG_LOCAL       0x00000008L

/*====================================================================
结构 ： TAppShareWindow
描述 ： 应用共享窗口结构
====================================================================*/
struct TAppShareWindow
{
    //窗口ID
    s32     m_nWindowID;
    //窗口所在进程ID
    s32     m_nWindowExtra;
    //父窗口ID
    s32     m_nWindowOwner;
    //窗口屏幕坐标
    u16    m_wLeft;
    //窗口屏幕坐标
    u16    m_wTop; 
    //窗口屏幕坐标
    u16    m_wRight; 
    //窗口屏幕坐标
    u16    m_wBottom; 
    //窗口标题长度
    u16    m_wWindowTitleLength;
    //窗口标题
    char    m_chWindowTitle[50]; 
private:
    //窗口标志
    s32     m_nWindowFlags;

public:
    TAppShareWindow()
    {
        m_nWindowID = 0;
        m_nWindowExtra = 0;
        m_nWindowOwner = 0;
        m_nWindowFlags = 0;
        m_wLeft = 0;
        m_wTop = 0;
        m_wRight = 0;
        m_wBottom = 0;
        m_wWindowTitleLength = 0;
        memset( m_chWindowTitle , 0 , sizeof(m_chWindowTitle) );
    };

    void SetWindowTitle(const char* szWindowTitle,u16 wTitleLen)
    {
        if(wTitleLen > sizeof(m_chWindowTitle))
        {
            strncpy(m_chWindowTitle,szWindowTitle,sizeof(m_chWindowTitle));

            if(m_chWindowTitle[48] > 0x8F)
            {
                m_chWindowTitle[48] = '\0';
                m_wWindowTitleLength = 48;
            }
            else
            {
                m_chWindowTitle[49] = '\0';
                m_wWindowTitleLength = 49;
            }
        }
        else
        {
            strncpy(m_chWindowTitle,szWindowTitle,wTitleLen);
            m_wWindowTitleLength = wTitleLen;
        }
    }
    TAppShareWindow( const TAppShareWindow& window )
    {
        m_nWindowID = window.m_nWindowID;
        m_nWindowExtra = window.m_nWindowExtra;
        m_nWindowOwner = window.m_nWindowOwner;
        m_nWindowFlags = window.m_nWindowFlags;
        m_wLeft = window.m_wLeft;
        m_wTop = window.m_wTop;
        m_wRight = window.m_wRight;
        m_wBottom = window.m_wBottom;
        m_wWindowTitleLength = window.m_wWindowTitleLength;
        memcpy( m_chWindowTitle , window.m_chWindowTitle , sizeof(m_chWindowTitle) );
    };
    TAppShareWindow& operator= ( const TAppShareWindow& window )
    {
        if( &window != this )
        {
            m_nWindowID = window.m_nWindowID;
            m_nWindowExtra = window.m_nWindowExtra;
            m_nWindowOwner = window.m_nWindowOwner;
            m_nWindowFlags = window.m_nWindowFlags;
            m_wLeft = window.m_wLeft;
            m_wTop = window.m_wTop;
            m_wRight = window.m_wRight;
            m_wBottom = window.m_wBottom;
            m_wWindowTitleLength = window.m_wWindowTitleLength;
            memcpy( m_chWindowTitle , window.m_chWindowTitle , sizeof(m_chWindowTitle) );
        }
        return *this;
    };

    bool operator==(const TAppShareWindow& window) const
    {
        if( m_nWindowID   != window.m_nWindowID)
//        || m_nWindowExtra != window.m_nWindowExtra
//        || m_nWindowOwner != window.m_nWindowOwner
//        || m_nWindowFlags != window.m_nWindowFlags
//        || m_wLeft        != window.m_wLeft
//        || m_wTop         != window.m_wTop
//        || m_wRight       != window.m_wRight
//        || m_wBottom      != window.m_wBottom
//        || m_wWindowTitleLength != window.m_wWindowTitleLength)
        {
            return false;
        }

        return true;
    };

    //设置查询窗口是否需要列出
    void SetWindowTaggable( )
    {
        m_nWindowFlags |= WINDOW_FLAG_TAGGABLE;
    };
    void ResetWindowTaggable( )
    {
        m_nWindowFlags &= ~WINDOW_FLAG_TAGGABLE;
    };
    bool IsWindowTaggable( )
    {
        return ( ( m_nWindowFlags & WINDOW_FLAG_TAGGABLE ) != 0 ? true : false );
    };

    //设置查询窗口是否最小化
    void SetWindowMinimized( )
    {
        m_nWindowFlags |= WINDOW_FLAG_MINIMIZED;
    };
    void ResetWindowMinimized( )
    {
        m_nWindowFlags &= ~WINDOW_FLAG_MINIMIZED;
    };
    bool IsWindowMinimized( )
    {
        return ( ( m_nWindowFlags & WINDOW_FLAG_MINIMIZED ) != 0 ? true : false );
    };

    //设置查询窗口是否共享窗口
    void SetWindowHosted( )
    {
        m_nWindowFlags |= WINDOW_FLAG_HOSTED;
    };
    void ResetWindowHosted( )
    {
        m_nWindowFlags &= ~WINDOW_FLAG_HOSTED;
    };
    bool IsWindowHosted( )
    {
        return ( ( m_nWindowFlags & WINDOW_FLAG_HOSTED ) != 0 ? true : false );
    };

    //设置查询窗口是否本地非共享窗口
    void SetWindowLocal( )
    {
        m_nWindowFlags |= WINDOW_FLAG_LOCAL;
    };
    void ResetWindowLocal( )
    {
        m_nWindowFlags &= ~WINDOW_FLAG_LOCAL;
    };
    bool IsWindowLocal( )
    {
        return ( ( m_nWindowFlags & WINDOW_FLAG_LOCAL ) != 0 ? true : false );
    };
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*====================================================================
结构 ： TAppShareColorPointer
描述 ： 共享非系统鼠标结构
====================================================================*/
struct TAppShareColorPointer
{
    //非系统鼠标缓存索引
    u16 m_wCacheIndex; 
    //非系统缓存鼠标热点坐标
    u16 m_wHotSpotX; 
    //非系统缓存鼠标热点坐标
    u16 m_wHotSpotY; 
    //非系统缓存鼠标宽度
    u16 m_wWidth; 
    //非系统缓存鼠标高度
    u16 m_wHeight; 
    u16 m_wAndMaskLength;
    u16 m_wXorMaskLength;
    u8* m_pbyMask;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*====================================================================
结构 ： TStillImageColorRGB
描述 ： 电子白板颜色
====================================================================*/
struct TStillImageColorRGB
{
    u8 m_byRed;
    u8 m_byGreen;
    u8 m_byBlue;

    TStillImageColorRGB()
    {
        m_byRed = 0;
        m_byGreen = 0;
        m_byBlue = 255;
    };
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*====================================================================
结构 ： TStillImagePoint
描述 ： 电子白板指示器
====================================================================*/
struct TStillImagePoint
{
    s32 m_nPointX;
    s32 m_nPointY;

    TStillImagePoint()
    {
        m_nPointX = 0;
        m_nPointY = 0;
    };
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*====================================================================
结构 ： TStillImageBitmap
描述 ： 电子白板指图片
====================================================================*/
struct TStillImageBitmap
{
    //BMP句柄(RegisterAllocateHandle分配)
    s32     m_nBitmapHandle;
    //工作间句柄(RegisterAllocateHandle分配)
    s32     m_nWorkspaceHandle;
    //平面ID(值为1)
    u8    m_byPlaneID;

    //视图状态(是否被选择)
    enum ViewState
    {
        e_unselected,
        e_selected,
        e_hidden
    };
    ViewState m_enumViewState;

    //在指定平面的Z轴相对位置
    enum ZOrder
    {
        e_front,
        e_back
    };
    ZOrder m_enumZOrder;

    //BMP左上角坐标点
    TStillImagePoint  m_tAnchorPoint;

    //BMP宽度
    s32     m_nBitmapWidth;
    //BMP高度
    s32     m_nBitmapHeight;
    
    //是否有后续BMP数据
    bool    m_bMoreToFollow;

    //BMP数据及长度
    s32     m_nBitmapDataLength;
    u8*   m_pbyBitmapData;

    TStillImageBitmap()
    {
        m_enumViewState = e_selected;
        m_enumZOrder = e_front;
        m_pbyBitmapData = NULL;
        m_nBitmapDataLength = 0;
    };
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*====================================================================
结构 ： TStillImageDrawing
描述 ： 电子白板指画图对象
====================================================================*/
struct TStillImageDrawing
{
    //图形句柄(RegisterAllocateHandle分配)
    s32     m_nDrawingHandle;
    //工作间句柄(RegisterAllocateHandle分配)
    s32     m_nWorkspaceHandle;
    //平面ID(值为1)
    u8    m_byPlaneID;
    
    //图形形状
    enum DrawingType
    {
        e_point,        // 点
        e_openPolyLine, // 开放曲线
        e_closedPolyLine, //封闭曲线
        e_rectangle,    //矩形
        e_ellipse       //椭圆
    };
    DrawingType    m_enumDrawingType;

    //Pen颜色
    TStillImageColorRGB m_tPenColor;
    //是否需要有填充颜色
    bool    m_bFilled;
    //填充颜色
    TStillImageColorRGB m_tFillColor;
    //Pen宽度(3--16)
    u8    m_byPenThickness;

    //Pen笔尖形状
    enum PenNib
    {
        e_circular,
        e_square
    };
    PenNib  m_enumPenNib;

    //线形状
    enum LineStyle
    {
        e_solid,
        e_dashed,
        e_dotted,
        e_dash_dot,
        e_dash_dot_dot,
        e_two_tone
    };
    LineStyle   m_enumLineStyle;

    //是否高亮显示
    bool    m_bHighlight;

    //视图状态(是否被选择)
    enum ViewState
    {
        e_unselected,
        e_selected,
        e_hidden
    };
    ViewState   m_enumViewState;

    //在指定平面的Z轴相对位置
    enum ZOrder
    {
        e_front,
        e_back
    };
    ZOrder  m_enumZOrder;

    //图形第一个点的坐标
    TStillImagePoint  m_tAnchorPoint;

    //图形点集及点总数
    s32 m_nDrawingPointsNum;
    TStillImagePoint* m_ptDrawingPoints;

    TStillImageDrawing()
    {
        m_enumDrawingType = e_openPolyLine;
        m_byPenThickness = 3;
        m_enumPenNib = e_circular;
        m_enumLineStyle = e_solid;
        m_bHighlight = false;
        m_enumViewState = e_selected;
        m_enumZOrder = e_front;
        m_bFilled = false;
		m_nDrawingPointsNum = 0;
		m_ptDrawingPoints = NULL;
    };
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

#define SI_LF_FACESIZE 32

/*====================================================================
结构 ： TStillImageLogfont
描述 ： 电子白板字体,对应于Windows字体结构LOGFONT
====================================================================*/
struct TStillImageLogfont
{
    long m_lfHeight; 
    long m_lfWidth; 
    long m_lfEscapement; 
    long m_lfOrientation; 
    long m_lfWeight; 
    u8 m_lfItalic; 
    u8 m_lfUnderline; 
    u8 m_lfStrikeOut; 
    u8 m_lfCharSet; 
    u8 m_lfOutPrecision; 
    u8 m_lfClipPrecision; 
    u8 m_lfQuality; 
    u8 m_lfPitchAndFamily; 
    char m_lfFaceName[SI_LF_FACESIZE];
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
/*====================================================================
结构 ： TStillImageText
描述 ： 电子白板文本
====================================================================*/
struct TStillImageText
{
    //文字句柄(RegisterAllocateHandle分配)
    s32     m_nTextHandle;
    //工作间句柄
    s32     m_nWorkspaceHandle;
    //平面ID(值为1)
    u8    m_byPlaneID;

    //文字颜色
    TStillImageColorRGB m_tTextColor;
    //文字后景色
    TStillImageColorRGB m_tTextBackColor;

    //视图状态(是否被选择)
    enum ViewState
    {
        e_unselected,
        e_selected,
        e_hidden
    };
    ViewState   m_enumViewState;

    //在指定平面的Z轴相对位置
    enum ZOrder
    {
        e_front,
        e_back
    };
    ZOrder  m_enumZOrder;

    //文字左上角坐标点
    TStillImagePoint m_tAnchorPoint;

    //文字字体
    TStillImageLogfont m_tLogFont;

    //文字内容及长度
    s32 m_nTextDataLength;
    u8* m_pbyTextData;

    TStillImageText()
    {
        m_nTextHandle = 0;
        m_nWorkspaceHandle = 0;
        m_byPlaneID = 1;
        m_tTextColor.m_byRed = 0;
        m_tTextColor.m_byGreen = 0;
        m_tTextColor.m_byBlue = 0;
        m_tTextBackColor.m_byRed = 0xff;
        m_tTextBackColor.m_byGreen = 0xff;
        m_tTextBackColor.m_byBlue = 0xff;
        m_enumViewState = e_selected;
        m_enumZOrder = e_front;
        m_pbyTextData = NULL;
    };
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*====================================================================
结构 ： TStillImagePlaneParameter
描述 ： 电子白板平台结构
====================================================================*/
struct TStillImagePlaneParameter
{
    //平面ID(值为0,1,2)
    u8    m_byPlaneID;
    //是否可编辑
    bool    m_bEditable;
    //是否支持注释
    bool    m_bAnnotation;
    //是否支持图象
    bool    m_bImage;
    //是否令牌保护
    bool    m_bProtected;

    TStillImagePlaneParameter()
    {
        m_byPlaneID = 1;
        m_bEditable = true;
        m_bAnnotation = true;
        m_bImage = true;
        m_bProtected = false;
    };
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
/*====================================================================
结构 ： TStillImageViewParameter
描述 ： 电子白板视图结构
====================================================================*/
struct TStillImageViewParameter
{
    //视图句柄(RegisterAllocateHandle分配)
    s32     m_nViewHandle;

    //视图状态(是否焦点视图)
    enum ViewState
    {
        e_hidden,
        e_background,
        e_foreground,
        e_focus
    };
    ViewState   m_enumViewState;

    //视图是否被锁定(false表示被琐定)
    bool    m_bUpdatesEnabled;

    TStillImageViewParameter()
    {
        m_nViewHandle = 0;
        m_enumViewState = e_foreground;
        m_bUpdatesEnabled = true;
    };
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
/*====================================================================
结构 ： TStillImageWorkspace
描述 ： 电子白板工作区结构
====================================================================*/
struct TStillImageWorkspace
{
    //工作间句柄(RegisterAllocateHandle分配)
    s32     m_nWorkspaceHandle;

    u16     m_wAppRosterInstance;

    //工作间是否同步
    bool    m_bSynchronized;

    //工作间是否接收键盘事件
    bool    m_bAcceptKeyboardEvents;

    //工作间是否接收鼠标事件
    bool    m_bAcceptPointingDeviceEvents;

    //工作间宽度
    s32     m_nWorkspaceWidth;
    //工作间高度
    s32     m_nWorkspaceHeight;

    //平面参数
    TStillImagePlaneParameter   m_atPlaneParameters[3];

    //视图参数(Handle为0表示不存在视图)
    TStillImageViewParameter    m_tViewParameter;

    TStillImageWorkspace()
    {
        m_nWorkspaceHandle = 0;
        m_wAppRosterInstance = 0;
        m_bSynchronized = true;
        m_bAcceptKeyboardEvents = false;
        m_bAcceptPointingDeviceEvents = false;
        m_nWorkspaceWidth = 0;
        m_nWorkspaceHeight = 0;
    };
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

#ifdef WIN32
#pragma pack( pop )
#endif

#endif
