#ifndef NMS_PUBLIC_MACRO_H_
#define NMS_PUBLIC_MACRO_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "kdvdef.h"
#include "errorid.h"
#include "mcudrvlib.h"
#include "mtsnmp.h"
#include "vccommon.h"
#include "snmpadp.h"
#include "calladdr.h"

//数据库名
#define DB_NAME_MAX_LEN     (s32)16
//用户及密码的长度
#define USER_LEN            (s32)17
#define PWD_LEN             USER_LEN
//指示灯状态长度
#define LED_STATUS_LEN		(s32)24

#define NMS_APP_NAME            _T("NmsServer 0xbe8e2ce1")
#define NMS_WATCHDOG_APPID      (u16)1
#define NMS_WATCHDOG_APP_NAME   _T("NmsWatchDog 0xbe8e2ce1")
#define NMS_WATCHDOG_PORT       (u16)7777

//看门狗和NMS互通消息
#define NMS_WATCHDOG_MSG_START      (u16)1000   //  网管启动
#define NMS_WATCHDOG_MSG_QUIT       (u16)1100   //  网管关闭

#define LANGUAGE_FILE_NAME      _T("nms.lan")

//错误码
#define ERROR_CMD_SUCCESS               0	// 交易成功
#define ERROR_DEFAULT                   ERR_SVR_BGN + 1     //缺省错误
#define ERROR_SVR_RESTARTING            ERR_SVR_BGN + 2     //服务器正在启动中
#define ERROR_DB_UNKNOWD                ERR_SVR_BGN + 3     //数据库出错
#define ERROR_CMD_IN_RUNNING            ERR_SVR_BGN + 4     //操作正在进行中

#define	ERROR_NEED_MORE_PARAM           ERR_SVR_BGN + 5     //参数不全

#define ERROR_CMD_TIMEOUT               ERR_SVR_BGN + 10    //SessionManager超时
#define ERROR_CMD_TYPE_MISMATCHED       ERR_SVR_BGN + 11    //命令类型不匹配

#define	ERROR_NO_FREEINSTANCE           ERR_SVR_BGN + 20    //CM/FM应用没有可用的实例
#define	ERROR_CLIENT_MISMATCHED         ERR_SVR_BGN + 21    //客户端不匹配

#define	ERROR_FM_GETALARMSTAMP_FAIL     ERR_SVR_BGN + 30    //获取告警信息失败
#define	ERROR_FM_GETALARMSTAMP_TIMEOUT  ERR_SVR_BGN + 31    //获取告警信息超时

#define	ERROR_FM_GETALARM_FAIL          ERR_SVR_BGN + 32    //获取告警失败
#define	ERROR_FM_GETALARM_TIMEOUT       ERR_SVR_BGN + 33    //获取告警超时

#define ERROR_CM_RESTART_UPDATING       ERR_SVR_BGN + 34    //设备文件升级中，不允许重起

#define ERROR_SNMP_SEND_FAIL            ERR_SVR_BGN + 40    //发送SNMP命令失败
#define ERROR_SNMP_RCV_FAIL             ERR_SVR_BGN + 41    //发送SNMP命令失败

#define ERROR_FTP_RUNNING               ERR_SVR_BGN + 60    //Ftp操作正在执行
#define ERROR_FTP_DELETEFILE_FAIL       ERR_SVR_BGN + 61    //FTP删除文件失败
#define ERROR_FTP_PUTFILE_FAIL          ERR_SVR_BGN + 62    //FTP上载文件失败
#define ERROR_FTP_GETFILE_FAIL          ERR_SVR_BGN + 63    //FTP获取文件失败

#define ERROR_EQUIPMENT_MISMATCHED      ERR_SVR_BGN + 70    //设备类型不匹配

#define ERROR_LOGIN                     ERR_SVR_BGN + 80    //登录失败
#define ERROR_LOGIN_NO_USER             ERR_SVR_BGN + 81    //没有这个用户
#define ERROR_LOGIN_ERR_PWD             ERR_SVR_BGN + 82    //用户密码错误
#define ERROR_LOGIN_NO_ROLE             ERR_SVR_BGN + 83    //用户不从属于角色
#define ERROR_CHANGE_PWD                ERR_SVR_BGN + 84    //修改密码失败
#define ERROR_PWD_MISMATCHED            ERR_SVR_BGN + 85    //密码不符合
#define ERROR_USER_HAS_LOGON            ERR_SVR_BGN + 86    //用户已经登录				

//单板
#define ERROR_SET_BOARD_STATUS_FAILED       ERR_SVR_BGN + 100   //设置单板状态失败
#define ERROR_SYNC_BOARD_TIME_FAILED        ERR_SVR_BGN + 101   //同步单板时间失败
#define ERROR_BOARD_BITERROR_TEST_FAILED    ERR_SVR_BGN + 102   //误码测试失败
#define ERROR_BOARD_SELF_TEST_FAILED        ERR_SVR_BGN + 103   //自测失败
#define ERROR_BOARD_E1LINK_CFG_FAILED       ERR_SVR_BGN + 104   //E1链路设置失败



#define	DEVICE_LINK_DOWN            999
#define DEVICE_POWEROFF             998
#define DEVICE_RESTART              1000


//对象类型
#define OBJECT_TYPE_UNKNOWN     0   // 未知的设备
#define OBJECT_TYPE_MCU         0x1 // MCU设备
#define OBJECT_TYPE_MT          0x2 // 终端设备
#define OBJECT_TYPE_PERI        0x3 // 外设
#define OBJECT_TYPE_SECURITY    0x4 // 安全

//Mcu 分类
#define MCU_CATALOG_UNKNOWN     0x0 //未知Mcu
#define MCU_CATALOG_8000        0x1 //Mcu8000
#define MCU_CATALOG_8000B       0x2 //Mcu8000B
#define MCU_CATALOG_8000C       0x3 //MCU 8000C
#define MCU_CATALOG_8000G       0x4 //MCU 8000G

// 终端分类
#define MT_CATALOG_UNKNOWN				MTSYSHARDWARETYPE_UNKNOWNMTMODEL	// 未知终端
#define MT_CATALOG_PC					MTSYSHARDWARETYPE_PCMT				// 桌面终端
#define MT_CATALOG_EMBED_MT8010			MTSYSHARDWARETYPE_8010				// MT8010终端
#define MT_CATALOG_EMBED_MT8010C		MTSYSHARDWARETYPE_8010C				// MT8010C终端
#define MT_CATALOG_EMBED_MT8010C1		MTSYSHARDWARETYPE_8010C1			// MT8010C终端
#define MT_CATALOG_EMBED_IMT			MTSYSHARDWARETYPE_IMT				// 嵌入终端IMT
#define MT_CATALOG_EMBED_MT8010A		MTSYSHARDWARETYPE_8010A				// MT8010A终端
#define MT_CATALOG_EMBED_MT8010APLUS    MTSYSHARDWARETYPE_8010APLUS			// MT8010A+终端
#define MT_CATALOG_EMBED_MT6610			MTSYSHARDWARETYPE_6610				// TS6610
#define MT_CATALOG_EMBED_MT6610E		MTSYSHARDWARETYPE_6610E				// TS6610E
#define MT_CATALOG_EMBED_MT6210			MTSYSHARDWARETYPE_6210				// TS6210
#define MT_CATALOG_EMBED_MT5210			MTSYSHARDWARETYPE_5210				// TS5210
#define MT_CATALOG_EMBED_MT3210			MTSYSHARDWARETYPE_3210				// TS3210
#define MT_CATALOG_EMBED_MTV5			MTSYSHARDWARETYPE_V5				// TS V5
#define MT_CATALOG_EMBED_MT8010A_2		MTSYSHARDWARETYPE_8010A_2			// 8010A 最大2M
#define MT_CATALOG_EMBED_MT8010A_4		MTSYSHARDWARETYPE_8010A_4			// 8010A 最大4M
#define MT_CATALOG_EMBED_MT8010A_8		MTSYSHARDWARETYPE_8010A_8			// 8010A 最大8M
#define MT_CATALOG_EMBED_MT7210			MTSYSHARDWARETYPE_7210				// TS7210
#define MT_CATALOG_EMBED_MT7610			MTSYSHARDWARETYPE_7610				// TS7610
#define MT_CATALOG_EMBED_MT7620_B		MTSYSHARDWARETYPE_7620_2			// TS7620_B
#define MT_CATALOG_EMBED_MT7620_A		MTSYSHARDWARETYPE_7620_4			// TS7620_A
#define MT_CATALOG_EMBED_MT5610         MTSYSHARDWARETYPE_5610              // MT5610终端
#define MT_CATALOG_EMBED_MT7810			MTSYSHARDWARETYPE_7810				// TS7810
#define MT_CATALOG_EMBED_MT7910			MTSYSHARDWARETYPE_7910				// TS7910
#define MT_CATALOG_EMBED_MT7820_A		MTSYSHARDWARETYPE_7820_A			// MT7820_A
#define MT_CATALOG_EMBED_MT7820_B		MTSYSHARDWARETYPE_7820_B			// MT7820_B
#define MT_CATALOG_EMBED_MT7920_A		MTSYSHARDWARETYPE_7920_A			// MT7920_A
#define MT_CATALOG_EMBED_MT7920_B		MTSYSHARDWARETYPE_7920_B			// MT7920_B


//安全分类
#define SECURITY_CATALOG_UNKNOWN    0x0 //未知
#define SECURITY_CATALOG_USER       0x1 //User
#define SECURITY_CATALOG_GROUP      0x2 //Group

//前后台链路状况
#define	LINK_DOWN       0
#define LINK_ACTIVE     1
#define LINK_UNKNOWN    255

//搜索网管服务器的广播地址和端口
#define MULTICAST_IP                (LPCSTR)_T("228.1.1.224")
#define SVR_FIND_MULTICAST_PORT     (u16)7883
#define CLT_FIND_MULTICAST_PORT     (u16)7884

// 网管服务器的通讯参数
typedef struct tagTNmsEnvDataStruct
{
    tagTNmsEnvDataStruct()
    {
        ZeroMemory(this, sizeof (tagTNmsEnvDataStruct));
    }
    u32 m_dwNmsIp;  // 网管服务器地址
    u16 m_wNmsPort; // 网管服务器节点端口
} TNmsEnvDataStruct, *PTNmsEnvDataStruct;

//时间字符串的长度
const u8 TIME_LEN               = 20;

//IP地址字符串的长度
const u8 IPADDR_LEN             = 16;

//设备版本长度
const u8 DEVICE_VER_LEN         = 20;

//DB连接超时（秒）
const u8 MAX_DBCONNECT_TIME     = 30;

//软件版本长度
const u8 MAX_SOFTWARE_VER_LEN   = 255;

//灯的状态程度
const u8 MAX_LED_STATUS_LEN	    = 64;

//会议名长度
const u8 MAX_CONFNAME_LEN	    = 32;
const u8 MAX_CONFID_LEN         = MAX_CONFNAME_LEN;

const u8 PERI_STATE_LEN         = 4;    //MCU外设设备状态长度
const u8 SYNCTIME_STATE_LEN     = 3;    //MCU外设时间同步状态长度
const u8 TEST_LEN               = 4;    //MCU外设检测长度(误码/自检)
const u8 E1_LEN                 = 12;   //E1链路设置长度

//写日志级别定义
#define CRITICAL        1
#define IMPORTANT       2
#define APPLICATION     196
#define USERDEBUG       255


//Session 模块App ID定义
#define CSESSIONMANAGER_APPID       AID_SVR_BGN
#define CCMAPP_APPID                (AID_SVR_BGN + 1)		
#define CFMAPP_APPID                (AID_SVR_BGN + 2)

#define AID_INT_TEST_NMS            (AID_SVR_BGN + 3)

//文件升级定时查询间隔
#define NMS_UPDATE_QUERY_TIME       (long)2 * 1000

//文件升级分配最大内存大小
#define NMS_UPDATE_MEMORY           (int)4 * 1024

//文件升级定时查询记录
typedef struct tagUpdateFile
{
    tagUpdateFile() : m_dwClientIID(MAKEIID(INVALID_APP, INVALID_INS)),
                      m_dwClientNode(INVALID_NODE), m_dwDeviceIP(0),
                      m_dwBoardIP(0), m_dwDeviceID(0), m_nFileNum(0),
                      m_wGetSetPort(161), m_wDeviceType(0), m_byIsBoard(0),
                      m_byLayer(0), m_bySlot(0), m_byType(0)
    {
        memset( m_aszFileName, 0, sizeof (m_aszFileName) );
    }

    u32                     m_dwClientIID;  //客户端IID
    u32                     m_dwClientNode; //客户端NODE
    u32                     m_dwDeviceIP;   //设备IP, 主机序
    u32                     m_dwBoardIP;    //单板IP, 主机序
    u32                     m_dwDeviceID;   //设备ID
    s32                     m_nFileNum;     //文件个数
    u16                     m_wGetSetPort;  //设备端口, 主机序
    u16                     m_wDeviceType;  //设备类型
    u8                      m_byIsBoard;    //是否单板
    u8                      m_byLayer;      //单板层号
    u8                      m_bySlot;       //单板槽号
    u8                      m_byType;       //单板类型
    s8                      m_aszFileName[8][64];   //文件名
} TUpdateFile;

// 定义用户登录数据，同时可以用于修改用户口令
typedef struct tagTNmsLogonReq
{
    tagTNmsLogonReq()
    {
        ZeroMemory(this, sizeof (tagTNmsLogonReq));
    }
	TCHAR   m_szUsrName[USER_LEN];  // 用户登录名称
	TCHAR   m_szUsrPwd[PWD_LEN];    // 用户当前口令
	TCHAR   m_szPwdNew[PWD_LEN];    // 用户新口令（修改口令时有效）

    /*=============================================================================
    函 数 名:Print
    功    能:内容打印
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/03/30  4.0     王昊    创建
    =============================================================================*/
    void Print(void) const
    {
        ::OspPrintf( TRUE, FALSE, "User = %d, Pwd = %s, ConfirmPwd = %s\n",
                     m_szUsrName, m_szUsrPwd, m_szPwdNew );
    }
} TNmsLogonReq, *PTNmsLogonReq;

// 定义用户登录结果数据
typedef struct tagTNmsLogonRet
{
    tagTNmsLogonRet()
    {
        ZeroMemory(this, sizeof (tagTNmsLogonRet));
    }
    u32     m_dwResult;             // 登录结果
    TCHAR   m_szDbUser[USER_LEN];   // 数据库用户名
    TCHAR   m_szDbPwd[PWD_LEN];     // 数据库用户密码
    TCHAR   m_szDbAddr[IPADDR_LEN]; // 数据库地址
    TCHAR   m_szDbName[DB_NAME_MAX_LEN];    // 数据库名称
    TCHAR   m_szFtpUser[USER_LEN];  // Ftp用户 
    TCHAR   m_szFtpPwd[PWD_LEN];    // Ftp密码

    /*=============================================================================
    函 数 名:Print
    功    能:内容打印
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/03/30  4.0     王昊    创建
    =============================================================================*/
    void Print(void) const
    {
        ::OspPrintf( TRUE, FALSE, "RetCode = %d, DbUser = %s, DbPwd = %s, \
DbIp = %s, DbName = %s, FtpUser = %s, FtpPwd = %s\n", m_dwResult, m_szDbUser,
                     m_szDbPwd, m_szDbAddr, m_szDbName, m_szFtpUser, m_szFtpPwd );
    }
} TNmsLogonRet, *PTNmsLogonRet;


//Ftp文件命令的消息结构
typedef struct tagTCmdCmFtpFile
{
    tagTCmdCmFtpFile()
    {
        ZeroMemory(this, sizeof (tagTCmdCmFtpFile));
    }

    TCHAR   m_szDeviceFileName[MAX_PATH + 1];   //远端文件全名
    TCHAR   m_szNmcFileName[MAX_PATH + 1];      //客户端文件全名
//    u32     m_dwDeviceIP;                       //远端设备IP(网络序)
    u8      m_byMainType;                       //远端设备主类型(MCU/MT)
    u8      m_bySubType;                        //远端设备子类型(板类型/终端类型)
    u8      m_byOsType;                         //操作系统类型

    //zw[08/12/2008]
    void ChangeName(TCHAR *pstrFilePath)
    {
        if (pstrFilePath == NULL)
        {
			return;
        }

		s8 oldName[256] = {0};
        s8 newName[256] = {0};

		STRCAT(oldName, pstrFilePath);
		STRCAT(newName, pstrFilePath);

        if ( !strcmp("index.htm", m_szNmcFileName) )
        {
			STRCAT(oldName, "\\index.htm");
			STRCAT(newName, "\\index.htm.unknown");

            remove( newName );
            rename( oldName, newName );

            STRCAT( &m_szNmcFileName[9], ".unknown\0");
            STRCAT( &m_szDeviceFileName[9], ".unknown\0");
			return;
        }
        if ( !strcmp("player.htm", m_szNmcFileName) )
        {
			STRCAT(oldName, "\\player.htm");
			STRCAT(newName, "\\player.htm.unknown");
            remove( newName );
            rename( oldName, newName );

            STRCAT( &m_szNmcFileName[10], ".unknown\0");
            STRCAT( &m_szDeviceFileName[10], ".unknown\0");
			return;

        }
        if ( !strcmp("mcucfg.ini", m_szNmcFileName) )
        {
			STRCAT(oldName, "\\mcucfg.ini");
			STRCAT(newName, "\\mcucfg.ini.unknown");
            remove( newName );
            rename( oldName, newName );

            STRCAT( &m_szNmcFileName[10], ".unknown\0");
            STRCAT( &m_szDeviceFileName[10], ".unknown\0");
			return;
        }

		if ( !strcmp("brdcfg.ini", m_szNmcFileName) )
        {
			STRCAT(oldName, "\\brdcfg.ini");
			STRCAT(newName, "\\brdcfg.ini.unknown");
            remove( newName );
            rename( oldName, newName );
			
            STRCAT( &m_szNmcFileName[10], ".unknown\0");
            STRCAT( &m_szDeviceFileName[10], ".unknown\0");
			return;
        }

        if ( !strcmp("mtcfg.ini", m_szNmcFileName) )
        {
			STRCAT(oldName, "\\mtcfg.ini");
			STRCAT(newName, "\\mtcfg.ini.unknown");
			remove( newName );
            rename( oldName, newName );

            STRCAT( &m_szNmcFileName[9], ".unknown\0");
            STRCAT( &m_szDeviceFileName[9], ".unknown\0");
			return;
        }
    }

    /*=============================================================================
    函 数 名:Print
    功    能:内容打印
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/04/03  4.0     王昊    创建
    =============================================================================*/
    void Print(void) const
    {
        ::OspPrintf( TRUE, FALSE, "MainType = %d, SubType = %d, \
OsType = %d\nDeviceFile = %s, NmcFile = %s\n",//IP = %s
//                     CCallAddr::GetIpStr( ntohl(m_dwDeviceIP) ),
                     m_byMainType,
                     m_bySubType, m_byOsType,
                     m_szDeviceFileName,
                     m_szNmcFileName );
    }
} ICmdCmFtpFile, *PICmdCmFtpFile;


// 文件传输进度数据结构
typedef struct tagTFtpProcSize
{
    tagTFtpProcSize() : m_dwEventId(0), m_dwCurProcSize(0) {}

    u32 m_dwEventId;        // 传输进度事件，包括FTP_BEGIN：开始传输，
                            //      FTP_TRANSFER：文件正在传输
                            //      FTP_END：文件传输结束
                            //      FTP_ERROR：文件传输出错
    u32 m_dwCurProcSize;    // 对应事件的内容，当事件为FTP_BEGIN时，表示文件的总长度
                            //      当事件为FTP_TRANSFER时，表示当前已经传输了的大小
                            //      当事件为FTP_END时，不表示
                            //      当事件为FTP_ERROR时，表示错误码

    /*=============================================================================
    函 数 名:Print
    功    能:内容打印
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/03/30  4.0     王昊    创建
    =============================================================================*/
    void Print(void) const
    {
        ::OspPrintf( TRUE, FALSE, "Status = %d, CurSize = %d\n", m_dwEventId,
                     m_dwCurProcSize );
    }
} TFtpProcSize, *PTFtpProcSize;


// 单板告警的告警内容数据结构
typedef struct tagTBoard
{
    tagTBoard()
    {
        ZeroMemory(this, sizeof (tagTBoard));
    }
    u32 m_dwIPAddr;                                     //IP地址,网络序
    u32 m_dwSuperID;                                    //上级ID地址
    u8  m_byLayerID;                                    //层号,对于外设表示外设类型
    u8  m_bySlotID;                                     //槽号,对于外设表示外设ID
    u8  m_byType;                                       //类型
    u8  m_byStatus;                                     //状态
    u8  m_byOsType;                                     //操作系统类型
    TCHAR   m_szSoftwareVersion[MAX_SOFTWARE_VER_LEN];  //软件版本
    TCHAR   m_szPanelLed[MAX_LED_STATUS_LEN];           //板卡Led灯

    /*=============================================================================
    函 数 名:Print
    功    能:内容打印
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/04/03  4.0     王昊    创建
    =============================================================================*/
    void Print(void) const
    {
        ::OspPrintf( TRUE, FALSE, "IP = %s, MasterID = %d, LayerID = %d, SlotID = %d, \
Type = %d, Status = %d, OsType = %d, SoftwareVer = %s, LED = %s\n",
                     CCallAddr::GetIpStr( ntohl( m_dwIPAddr ) ), m_dwSuperID,
                     m_byLayerID, m_bySlotID, m_byType, m_byStatus, m_byOsType,
                     m_szSoftwareVersion, m_szPanelLed );
    }
} TBoard, *PTBoard;


//DSI板
typedef struct tagTDSIBoard : public TBoard
{
    tagTDSIBoard() : m_byE1LinkCfgCount(0), m_byE1LinkStatus(0)
    {
        m_byType = DSL8000_BRD_DSI;
    }

    u8  m_byE1LinkCfgCount;         //E1链路状态设置数量
    u8  m_byE1LinkStatus;           //E1链路状态

    /*=============================================================================
    函 数 名:Print
    功    能:内容打印
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/04/03  4.0     王昊    创建
    =============================================================================*/
    void Print(void) const
    {
        ::OspPrintf( TRUE, FALSE, "Count = %d, E1Status = %d",
                     m_byE1LinkCfgCount, m_byE1LinkStatus );
        dynamic_cast<const TBoard*>(this)->Print();
    }
} TDSIBoard, *PTDSIBoard;


//Mcu会议状态告警内容数据结构
typedef struct tagTMcuConfState
{
    tagTMcuConfState()
    {
        ZeroMemory( this, sizeof (tagTMcuConfState) );
    }
    u8      m_byMcuConfState;                   //会议状态
    TCHAR   m_szMcuAlias[32];                   //Mcu别名
    TCHAR   m_szMcuConfName[MAX_CONFNAME_LEN];  //会议名
    TCHAR   m_szMcuConfId[MAX_CONFID_LEN];      //会议Id
    TCHAR   m_szMcuConfTime[TIME_LEN];          // 告警产生时间

    /*=============================================================================
    函 数 名:Print
    功    能:内容打印
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/04/03  4.0     王昊    创建
    =============================================================================*/
    void Print(void) const
    {
        ::OspPrintf( TRUE, FALSE,
                     "State = %d, Alias = %s, Name = %s, ConfID = %s, Time = %s\n",
                     m_byMcuConfState, m_szMcuAlias, m_szMcuConfName,
                     m_szMcuConfId, m_szMcuConfTime );
    }
} TMcuConfState, *PTMcuConfState;


//Mt会议状态告警内容数据结构
typedef struct tagTMtConfState
{
    tagTMtConfState()
    {
        ZeroMemory( this, sizeof (tagTMtConfState) );
    }
    u8      m_dwMtState;                    //Mt状态
    TCHAR   m_szConfId[MAX_CONFID_LEN];     //会议Id
    TCHAR   m_szMtAlias[MAXLEN_ALIAS];      //Mt别名
    TCHAR   m_szConfTime[TIME_LEN];         //告警产生时间

    /*=============================================================================
    函 数 名:Print
    功    能:内容打印
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/04/03  4.0     王昊    创建
    =============================================================================*/
    void Print(void) const
    {
        ::OspPrintf( TRUE, FALSE,
                     "State = %d, Alias = %s, ConfID = %s, Time = %s\n",
                     m_dwMtState, m_szMtAlias, m_szConfId, m_szConfTime );
    }
} TMtConfState, *PTMtConfState;


//定义服务器配置变量
typedef struct tagSvrConfig
{
    tagSvrConfig()
    {
        ZeroMemory(this, sizeof (tagSvrConfig));
    }
    TCHAR   m_szIPAddr[IPADDR_LEN];         //服务器ip地址
    TCHAR   m_aszReadCommunity[MAX_COMMUNITY_LEN];  //读共同体名
    TCHAR   m_aszWriteCommunity[MAX_COMMUNITY_LEN]; //写共同体名
    u16     m_wTrapPort;                    //服务器接收trap的端口
    u16     m_wOspConnPort;                 //服务器Osp侦听端口
    u16     m_wOspTelnetPort;               //服务器Telnet端口
    u16     m_wCmSyncPeriod;                //配置轮询的定时时间
    u16     m_wFmSyncPeriod;                //告警轮询的定时时间
    u8      m_bySnmpTimeout;                //Snmp超时

    TCHAR   m_szDBServer[IPADDR_LEN];       //服务器连接的数据库地址
    TCHAR   m_szDBName[DB_NAME_MAX_LEN];    //服务器连接的数据库名
    TCHAR   m_szDBUser[USER_LEN];           //服务器连接的用户名
    TCHAR   m_szDBPwd[PWD_LEN];             //服务器连接的用户密码

    TCHAR   m_szRemoteFtpUser[USER_LEN];    //服务器连接agent的ftp用户名
    TCHAR   m_szRemoteFtpPwd[PWD_LEN];      //服务器连接agent的ftp用户密码
    TCHAR   m_szLocalFtpUser[USER_LEN];     //服务器本地的Ftp用户名
    TCHAR   m_szLocalFtpPwd[PWD_LEN];       //服务器本地的Ftp用户密码
    TCHAR   m_szFtpLocalPath[MAX_PATH];     //服务器本地的ftp路径

    u8      m_byPrintToScreenLevel;         //服务器打印屏幕的等级
    u8      m_byPrintToFileLevel;           //服务器日志文件记录登记
    TCHAR   m_szLogFile[MAX_PATH];          //服务器日志文件名

    /*=============================================================================
    函 数 名:Print
    功    能:内容打印
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/03/30  4.0     王昊    创建
    =============================================================================*/
    void Print(void) const
    {
        ::OspPrintf( TRUE, FALSE, "SvrIp = %s, TrapPort = %d, Read = %s, Write = %s, \
OspPort = %d, TelnetPort = %d, CmPoll = %d, FmPoll = %d, Timeout = %d, DbIp = %s, \
DbName = %s, DbUser = %s, DbPwd = %s, FtpRemoteUser = %s, FtpRemotePwd = %s, \
FtpLocalUser = %s, FtpLocalPwd = %s, FtpLocalPath = %s, Screen = %d, File = %d, \
LogPath = %s\n",
                     m_szIPAddr, m_wTrapPort, m_aszReadCommunity, m_aszWriteCommunity,
                     m_wOspConnPort, m_wOspTelnetPort, m_wCmSyncPeriod, m_wFmSyncPeriod,
                     m_bySnmpTimeout, m_szDBServer, m_szDBName, m_szDBUser,
                     m_szDBPwd, m_szRemoteFtpUser, m_szRemoteFtpPwd,
                     m_szLocalFtpUser, m_szLocalFtpPwd, m_szFtpLocalPath,
                     m_byPrintToScreenLevel, m_byPrintToFileLevel, m_szLogFile );
    }
} TSvrConfig;

#endif
