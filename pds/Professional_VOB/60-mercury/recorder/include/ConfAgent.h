/*****************************************************************************
   模块名      : Recorder
   文件名      : ConfAgent.h
   相关文件    : 
   文件实现功能: TCfgAgent类定义
   作者        : 
   版本        : V3.5  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
******************************************************************************/
#ifndef CONFAGENT_H
#define CONFAGENT_H

#include "osp.h"
#include "kdvtype.h"
#include "kdvsys.h"
#include "mcuconst.h"
#include "mcustruct.h"
#include "afxtempl.h"
#include "rpctrl.h"
#include "dboperate.h"

//配置文件相关宏定义
#define RECCFG_FILE						( LPCSTR )".\\conf\\reccfg.ini"
#define RECDBGCFG_FILE					( LPCSTR )".\\conf\\recDebugCfg.ini"

#define SECTION_RECSYS   				( LPCSTR )"RecorderSystem"
#define KEY_RECSYS_PERIEQPID			( LPCSTR )"Id"
#define KEY_RECSYS_PERIEQPTYPE		    ( LPCSTR )"Type"
#define KEY_RECSYS_PERIEQPALIAS		    ( LPCSTR )"Alias"
#define KEY_RECSYS_IPADDR			    ( LPCSTR )"IpAddr"
#define KEY_RECSYS_RCVPORT		        ( LPCSTR )"RecorderRcvStartPort"
#define KEY_RECSYS_PLAYPORT		        ( LPCSTR )"RecorderPlayStartPort"
#define KEY_RECSYS_RECFILEPATH		    ( LPCSTR )"RecFilePath"
#define KEY_RECSYS_MCUID				( LPCSTR )"McuId"
#define KEY_RECSYS_MCUIPADDR			( LPCSTR )"McuIpAddr"
#define KEY_RECSYS_MCUCONNPORT		    ( LPCSTR )"McuConnPort"
#define KEY_RECSYS_MCUIDB				( LPCSTR )"McuIdB"
#define KEY_RECSYS_MCUIPADDRB			( LPCSTR )"McuIpAddrB"
#define KEY_RECSYS_MCUCONNPORTB		    ( LPCSTR )"McuConnPortB"
#define KEY_RECSYS_RECCHNUM             ( LPCSTR )"RecordChannels"
#define KEY_RECSYS_PLAYCHNUM            ( LPCSTR )"PlayChannels"
#define KEY_RECSYS_TELNETPORT           ( LPCSTR )"RecorderTelnetPort"
#define KEY_RECSYS_MACHINENAME          ( LPCSTR )"MachineName"

#define KEY_RECSYS_ISPUBLISH            ( LPCSTR )"IsPublish"
#define KEY_RECSYS_DBHOSTIP             ( LPCSTR )"DBHostIP"
#define KEY_RECSYS_USERNAME             ( LPCSTR )"DBUserName"
#define KEY_RECSYS_PASSWORD             ( LPCSTR )"DBPassWord"
#define KEY_RECSYS_VODMMSPATH           ( LPCSTR )"VodMMSPath"
#define KEY_RECSYS_MMSUSERNAME          ( LPCSTR )"MMSUserName"
#define KEY_RECSYS_MMSPASSWORD          ( LPCSTR )"MMSPassWord"

#define KEY_RECSYS_MULTIINSTANCE        ( LPCSTR )"MultiInstance"
#define	SECTION_RECORDERDEBUGINFO		(LPCSTR)"RecorderDebugInfo"

enum EMNotifyType
{
	emRECORDEROK		= 1,            // 录像状态改变
	emRECORDER_FULLDISK	= 2,            // 磁盘满（50M）
    emRECORDER_SMALL_FREESPACE  = 3,    // 空闲空间不够（200M）
	emRECORDER_WRITE_FILE_ERR = 4,		// rpctrl写文件出错, 录像停止

	emPLAYEROK       	= 1000,	        // 放像状态改变 
	emPLAYEREMPTYFILE	= 1001,         // 空文件, 异常
	emPLAYFINISHTASK	= 1002,         // 完成任务
	emPLAYERPROCESS  	= 1003,         // 放像进程
};

extern u32 g_adwDevice[MAXNUM_RECORDER_CHNNL+1];

#define NOT_FIND_RECID			(u32)(-1)			// 没有找到设备号
#define LANGUAGE_CHINESE        MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED)

typedef struct tagNotify{
    u16 wChnIndex;  
    u16 wRecId;
    u16 wAlarmType;
    u32 dwPlayTime;
    tagNotify()
    {
        wAlarmType = 0;
        wChnIndex  = 0;
        wRecId = 0;
        dwPlayTime = 0;
    };
}TNotifyInfo;

u16 GetRecChnIndex( u32 dwRecId );

class TCfgAgent
{
public:
    TCfgAgent()
    {
        Reset();
    }
    void Reset()
    {
        memset(this, 0, sizeof(TCfgAgent));
    }

public:
	u32  m_dwRecIpAddr;                  //录像通道IP地址
	s8   m_achLocalIP[32];
	u16  m_wRecStartPort;                //录像通道起始端口号
	u16  m_wPlayStartPort;               //放像通道起始端口号 //add play start port
	u8   m_byEqpId;                      //外设ID
	u8   m_byEqpType;                    //外设类型
	
	u32  m_dwMcuIpAddr;                  //MCU IP地址
	u16  m_wMcuConnPort;                 //MCU 通信端口号
	u16  m_wMcuId;                       //MCU ID号

    u32  m_dwMcuIpAddrB;                  // Mcu Ip
    u16  m_wMcuConnPortB;                 // Mcu port
    u16  m_wMcuIdB;                       // mcu id

	u8   m_byPlayChnNum;                 //播放通道个数
	u8   m_byRecChnNum;                  //录像通道个数
	u8   m_byTotalChnNum;                //总通道数
	s8   m_szAlias[MAXLEN_ALIAS];        //别名
	s8   m_szRecordPath[MAX_FILE_PATH_LEN]; //录像机文件保存路径
	s8   m_szFtpPath[MAX_FILE_PATH_LEN];    //文件发布FTP路径
    s8   m_szMachineName[32];            // 机器名
    
	BOOL m_bCppUnitTest;                 //是否测试
	u32  m_dwCppUnitIpAddr;              //测试IP地址
	u16  m_wCppUnitPort;                 //测试端口
	u16  m_wTelnetPort;                  //Telnet 调试断口

	BOOL m_bMediaServer;                 //是否用作媒体服务器
	s8   m_achMSInfo[256];               //媒体服务器信息    
	
	u32  m_dwDBHostIP;                   //发布数据库IP地址
	s8   m_achDBHostIP[32];
	
	s8   m_achDBUsername[32];
	s8   m_achDBPassword[32];

	s8   m_achVodFtpPatch[128];
	
	s8   m_achFTPUsername[32];
	s8   m_achFTPPassword[32];

    BOOL m_bMultiInst;                   //是否允许多启动
    
    BOOL32 m_bInited;
    s8     m_szCfgFilename[KDV_MAX_PATH];
	
	// 增加英文版配置文件, zgc, 2007-03-23
	LANGID  m_tLangId;  // language id
    BOOL    m_bOSEn;  // 是否英文版
    
    BOOL    m_bOpenDBSucceed;           // 数据库是否打开成功

	// [pengjie 2010/9/26] 设置录像机的最大录像文件大小（超过则会切文件）
	u32 m_dwMaxRecFileLen;

	//2012/[6/11 zhangli] 丢帧时是否请求关键帧、丢帧时是否写文件 
	BOOL32 m_bIsWriteAsfWhenLoseFrame;
	BOOL32 m_bIsNeedKeyFrameWhenLoseFrame;
	u8		m_byAudioBuffSize;	//音频buff大小(默认为16帧，每帧2048字节)
	u8		m_byVideoBuffSize;	//视频buff大小(默认为25帧，每帧524288字节)
	u8		m_byDsBuffSize;		//双流buff大小(默认为16帧，每帧524288字节)
	u8		m_byLeftBuffSize;	//buff最大剩余帧数(意味着每次读buff时，如果剩余帧数大于10，则反复读，直到剩余为10帧)
	
	//记录配置文件中帧数据大小
	TFrameSizeInfo	m_atFrameSize[MAX_AUDIO_TYPE_NUM];

	//直播文件的最小大小，用于一边录制一边直播的文件，默认512K
	u16 m_wMinSizeForBroadcast;
	// End

public:
    BOOL32 IsInited()
    {
        return m_bInited;
    }

    void SetCfgFilename(LPCSTR lpszName)
    {
        memset( m_szCfgFilename, 0, sizeof(m_szCfgFilename));
        strncpy( m_szCfgFilename, lpszName, KDV_MAX_PATH - 1 );
    }
	BOOL ReadConfigFromFile();
    BOOL WriteConfigIntoFile();
    BOOL32 IsLegalityPort(u16 wPort1, u16 wPort2);
	BOOL IsLegality( s32 nNum );	// 判断参数是否合法
	BOOL IsLegality( s8* aChar);
	void ErrorMsg(int nNum);		// 错误信息 nNum: 错误码
	void RecLog(char * fmt,...);

	//设置帧长数据信息
	BOOL32 SetFrameSizeInfo(TFrameSizeInfo &tFrameSizeInfo);
	//取帧长数据信息
	TFrameSizeInfo* GetFrameSizeInfo()
	{
		return m_atFrameSize;
	};
};

extern TCfgAgent g_tCfg;

#endif//! CONFAGENT_H
