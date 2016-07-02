/*****************************************************************************
   模块名      : Recorder
   文件名      : chnInst.h
   相关文件    : 
   文件实现功能: Recorder channel基类定义
   作者        : 
   版本        : V3.5  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/07/04  3.0         张明义      对正在录放像文件的保护
   2006/03/21  4.0         张宝卿      代码优化
******************************************************************************/
#ifndef CHNINST_H
#define CHNINST_H

#include "stdafx.h"

#pragma warning( disable : 4706 )
#include "osp.h"
#include "ConfAgent.h"
#include "RPCtrl.h"
#include "eqpcfg.h"
#include "loguserdef.h"
#include "kdvlog.h"
//#import "WMSServerTypeLib.dll" no_namespace named_guids raw_interfaces_only
#include "wmsserver.h"

#include <vector>
 using namespace std;

#pragma once

#define REC_PLAY_RSTIME			    (u16)500        // 放像时的发送缓存时间长度ms，用于丢包重传
#define REC_CONNECTTOA			    (u8)1	        // connect to MCU.A
#define REC_CONNECTTOB			    (u8)2	        // connect to MCU.B
#define REC_CONNECTTOAB			    (u8)3	        // connect to MCU.A && MCU.B
#define REC_CONNECT_TIMEOUT		    (u16)3 * 1000
#define REC_REGISTER_TIMEOUT	    (u16)4 * 1000

#define RECPROG_PERIOD				(u16)(5*1000)	// 定时通知录像进呈
#define RECSTATUS_NOTIPERIOD		(u32)(5*60*1000)   // 正常情况 5min 进行一次录像机状态上报
#define RECSTATUS_CRINOTIPERIOD		(u16)(5*1000)      // 特殊情况 5sec 进行一次录像机状态上报
#define REC_PLAYLIST_TIMER			(u16)(60*1*1000)
#define PLAY_EMPTY_FILE				(u8)254			// 播放空文件
#define FINISH_PLAY					(u8)100			// 播放完毕
#define PLAY_RATE_GRAN              (u8)5           // 放象时间粒度(s)
#define RENAME_OK					(u8)0			// 改名成功
#define VIDEO_TWO					(u8)2			// 两路视频
#define FIRST_REGISTERACK			(u8)1			// 第一次收到注册消息
#define CHECK_IFRAME_INTERVAL       (u16)3000       // 检测关键帧时间间隔(ms)

#define DISCSPACE_LEVEL1            200             // 磁盘空间告警
#define DISCSPACE_LEVEL2            50
#define CHECK_PUBLISHPOINT_INTERVAL (u16)3000        // 检测发布点时间间隔(ms)

#define MAX_TCNAME_LEN				(u8)(32)		//

extern CDBOperate  g_cTDBOperate;

struct TConnectParam
{
    u32  m_dwIpAddr;			// 连接Ip
    u16  m_wPort;				// 连接端口
    u16  m_wConnectContext;		// 连接上下文
};

class CChnInst : public CInstance
{
public:
	CChnInst();
	~CChnInst();	
public:
	enum EDAEMONSTATE
    {
		STATE_IDLE,
		STATE_NORMAL
	};   
private:
	u32  m_dwDevice;			    	// 设备句柄
	u32  m_dwPlayIpAddr;				// 播放通道播放IP地址       
    u16  m_wPlayPort;					// 播放通道播放起始端口号 
	u32  m_dwRecIpAddr;					// 录像通道录像IP地址        
    u16  m_wRecPort;					// 录像通道录像起始端口号    
	u8   m_byChnIdx ;			        // 通道索引号,录像、放像通道分别从0开始
	u16  m_wLocalPort;			        // 放像机用于绑定的端口
	s8   m_achChnInfo[40];              // 通道信息
	TMt  m_tSrvTmt;				        // 录像的TMT
	u8   m_byPublishMode;		        // 仅对录像机有用 ，表明发布方式
	u8   m_byPublishLevel;		        // 仅对录像机有用 ，表明发布级别

	u8   m_byIsNeedPrs;                 //[刘利九][2010/11/04]
	u8   m_byIsDstream;
	
    TRecChnnlStatus m_tChnnlStatus ;	// 通道状况
    CConfId   m_cConfId;    	        // 会议号

	// 修改放像处理流程, zgc, 07/02/14
	TMediaEncrypt m_tVideoEncrypt;		// 视频加密参数
	TMediaEncrypt m_tAudioEncrypt;		// 音频加密参数
	TMediaInfo    m_tMediaInfo;			// 媒体信息
    TCapSupportEx m_tCapSupportEx;      // 扩展能力信息（目前只保存FEC）
	TCapSupport   m_tConfCapSupport;    //会议的能力集信息
	BOOL32 m_bNeedCapChk;               //会议是否发了能力集信息过来,发过来的话需要检查能力信息
	u8      m_byConfPlaySrcSsnId;           //会议放像时，会议的SrcSsnId，给autoplaynext用，组织消息体
	u8		m_byAudioTrackNum;
	u8		m_byAudioMediaType;
	TAudioTypeDesc m_patAudDesc[MAXNUM_CONF_AUDIOTYPE];
	u8 m_byAudNumber;

	u8		m_byConfVideoMediaType;
	u8		m_byConfSecVidMediaType;

	BOOL32 m_bFileFirstVidHp;
	BOOL32 m_bFileSecVidHp;
	u8 m_byFileAudioTrackNum;

	u8 m_byVideoNum;					// 视频通道的视频信号路数

    s8 m_achFileName[MAX_FILE_NAME_LEN];  // 当前录放像文件名，格式为 abc_255_384K.asf 

	s8 m_achPlayListName[MAX_FILE_NAME_LEN];  // 当前录放像文件名，格式为 abc_255_384K.asf 

	s8 m_achOrignalFileName[MAX_FILE_NAME_LEN]; // 文件全路径原始名     //add by jlb 081027
	s8 m_achFullName[MAX_FILE_NAME_LEN];  // 文件全文件名，格式为 E:\vod\mcuname/abc_255_384K.asf 
	s8 m_achOrignalFullName[MAX_FILE_NAME_LEN]; // 文件全路径原始名     //add by jlb 081027
	BOOL32 m_bIsNodeAConnect;
	BOOL32 m_bIsNodeBConnect;

	//[2013/9/5 zhangli]录像参数 
	TMediaEncrypt m_ptMediaEncrypt[MAX_STREAM_NUM];
	TDoublePayload m_ptDoublePayload[MAX_STREAM_NUM];
	u8 m_byAudIndex;
	u8 m_byDsIndex;
	u8 m_byVidIndex;

protected:
	BOOL32	InitRecorder( );
	// [8/25/2010 liuxu] 为初始化录像通道，在InitalData中添加了pMsg和pcApp的参数，原型不带参数
    BOOL32	InitalData(CMessage * const pMsg = NULL, CApp* pcApp = NULL);
	void	DaemonInstanceEntry( CMessage * const pMsg, CApp* pcApp);
	void	InstanceEntry( CMessage * const pcMsg );
    void	ProcConnectTimeOut(BOOL32 bIsConnectA);
	BOOL32	ConnectMcu( u32 dwMcuIp, u16 wMcuPort, u32& dwMcuNode );
    void	ProcRegisterTimeOut( CMessage* const pMsg, BOOL32 bIsRegisterA );
    void	Register( u32 dwMcuNode, BOOL32 bIsRegisterA );
	void	Disconnect( CMessage * const pcMsg );

	LPCSTR GetCurChnInfoStr();    //获得当前通道信息(字符串描述)

    void MsgReNameFile( CMessage * const pcMsg );

	// << DAEMON 处理消息函数>>
	void MsgRegAckProc( CMessage* const pMsg, CApp* pcApp );
	void MsgRegNackProc( CMessage * const pcMsg );
	void MsgChnInitProc( CMessage * const pcMsg );
	void MsgListAllRecordProc( CMessage * const pcMsg );
	void MsgGetRecStateProc(CMessage* const pMsg, CApp* pcApp);
	void MsgGetRecChnStatusProc( CMessage* const pMsg, CApp* pcApp );
	void MsgGetPlayChnStatusProc( CMessage* const pMsg, CApp* pcApp );
	void MsgDeleteRecordProc( CMessage * const pcMsg );

	void MsgDaemonPublishFileReq( CMessage * const pcMsg );
	void MsgDaemonCancelPublishFileReq( CMessage * const pcMsg );
    void MsgDaemonGetMsStatusRsp( CMessage * const pcMsg );

	void DaemonProcReconnectBCmd( CMessage * const pcMsg ); //MCU 通知会话 重联MPCB 相应实例, zgc, 2007/04/30

	//<< 录像通道消息处理函数 >>
	void MsgStartRecordProc( CMessage * const pcMsg );
	void MsgPauseRecordProc( CMessage * const pcMsg );
	void MsgResumeRecordProc( CMessage * const pcMsg );
	void MsgStopRecordProc( CMessage * const pcMsg );
	void SendAddVidFileNotify(void);
	void MsgChangeRecModeProc( CMessage * const pcMsg );

	//<< 播放通道消息处理函数 >>
	void MsgStartPlayProc( CMessage * const pcMsg );
	void MsgPausePlayProc( CMessage * const pcMsg );
	void MsgResumePlayProc( CMessage * const pcMsg );
	void MsgFBPlayProc( CMessage * const pcMsg );
	void MsgFFPlayProc( CMessage * const pcMsg );
	void MsgStopPlayProc( CMessage * const pcMsg );
	void MsgSeekPlayProc( CMessage * const pcMsg );
    void MsgForceStopProc( CMessage * const pcMsg );
	void MsgSwitchStartNotifProc( CMessage * const pcMsg, BOOL32 bSwitchedFile = FALSE );
	void SwitchStartWaittimerOver( CMessage * const pcMsg );
	BOOL AutoPlayNextSwitchFile( void );     //自动播放下一个切换文件 add by jlb 081027
    BOOL GetNextSwitchFile(LPCSTR lpszCurFile, s8 *pszNextFile);   //得到下一个要播放的切换文件 add by jlb 081027

	//向MCU请求一个关键帧
	void  ProcTimerNeedIFrame( const CMessage *pMsg );                                  //Hdu请求Mcu发关键帧

    // <<录像设备通知>>
	void MsgDeviceNotifyProc(CMessage *const pMsg);
	void SendChnNotify();
	void SendRecProgNotify(BOOL32 bSetTimer = TRUE);
	u8   SendPlayProgNotify();
    void MsgRefreshCurChnRecFileName(CMessage * const pMsg);
	void MsgResetRtcpParam(CMessage * const pMsg);
	void MsgResetRcvParam(CMessage * const pMsg);

	void   SendRecStatusNotify( CApp* pcApp);
	void   StopAllChannel(BOOL32 bIncludePlayChnl);
	void   ReleaseChannel();
	BOOL32 SendMsgToMcu(u16 wEvent, CServMsg * const pcServMsg);	
	void   StatusShow( CApp* const pcApp);	
	u8     GetActivePayload(u8 byRealPayloadType, u8 byEncrpytMode = CONF_ENCRYPTMODE_NONE, BOOL32 bDStream = FALSE);
	void   MsgCppUnitGetEqpStatus(CMessage * const pMsg, CApp * pcApp);
    void   RestartRecorderProc();
    BOOL32 IsExistFile( s8* pchFileName );                      // 文件是否存在    

	u16 CheckFileCanPlay(s8 *pFileFullName);
    
    // 废弃
    u16    GetRecordCount(vector<CString> &vtDirName);    // 统计该MCU下录像的总数

    /* guzh [9/7/2006]
    void   SendRecordList(CServMsg &cServMsg, u16 dwTotalCount, u16 dwStartIdx, TListInfo& tListInfo, 
                          BOOL32 bConfReq = FALSE,
                          LPCSTR lpszConfName = NULL) ;
    */

    void   SendRecordList(CServMsg &cServMsg, vector<CString> &vtFiles) ;

	// void   InitPlayList();	
	// void   DelFileTapeList( s8* DelFileName );
	// BOOL32 ComparePlayList( TListInfo tTapeInfoList );          // 比较
	// void   MaintenanceList( TRecFileListNotify RecNotify );     // 维护
	// void   MsgMaintainPlayList();                               // 维护消息处理
	// void   IndexTapeList( TListInfo& tTapeInfoList );	        // 排序
    // void   CancelAllPublicFile();
    // void   PrintPlayList();
    /*void   MsgAjustBackRtcp(CMessage* const pMsg);*/   //调整BackRtcp[liu lijiu][20100727]
	BOOL32 IsPublicFile( s8* achFileName );
	
    BOOL32 GetSpaceInfo( s32& nFreeSpace, s32& nTotalSpace );
    void   ProcSetQosInfo( CMessage * const pcMsg );                // 设置Qos信息
    void   ComplexQos( u8& byValue, u8 byPrior );
    u32    GetRPBitRate( u16 wConfBitRate, u16 wPayLoadType );      //会议码率转成rp库的码率   
    void   Reclog( s8* pszFmt, ... );

    // 媒体信息存储结构转换, zgc, 2008-08-02
    void   MediaInfoRec2Mcu( TMediaInfo tMediaInfo, TPlayFileMediaInfo &tPlayFileMediaInfo );

	//[2012/4/13 zhangli]增加数据库操作接口，如果操作失败，连接数据库再次操作，如果仍然失败则返回
	//录像机启动时连接数据库，中途有可能数据库连接断掉，录像机侧和DB侧都不感知，导致操作失败
	u16 PublishOneFile(s8 *szFullFileName, BOOL32 isWriteFileInfo = TRUE);
	u16 UpdatePublishOneFile(char *szFullFileName);
	u16 AddOneLiveSource();
	u16 DeleteOneLiveSource();

	BOOL32 InitCOMServer();
	BOOL32 CreateBrdPubPoint();
	void DeleteBrdPubPoint();
	void ProcTimerPpFailed(CMessage * const pcMsg);
	void ProcRecorderQuit(CMessage * const pcMsg);

	BOOL32 CreatePlayList();
	void DeletePlayList();
	void AddToPlayList();

public:			// 录像机服务器接口添加
	/** 
	/* 功能：判断此实例是否已经连接上了终端
	/* 参数： 无
	/* 返回值： True已经连接上，False没有连接上
	*/
    BOOL32 IsMtConnect(){return m_bIsMtConnect;}

	/** 
	/* 功能：设置此实例是否已经连接上了终端
	/* 参数： bFlag ： True已经连接上，False没有连接上
	/* 返回值： 无
	*/
    void SetMtConnect(BOOL32 bFlag)
	{
		m_bIsMtConnect = bFlag;

		// [9/28/2010 liuxu] 自动重置
		if (!m_bIsMtConnect)
		{
			SetMTNodeInfo(0, 0);
		}
	}

	/** 
	/* 功能：Daemon实例处理开始录像请求
	/* 参数： CMessage , CApp
	/* 返回值： 无
	*/
    void OnDaemStartRecReq(CMessage* const pMsg, CApp* pcApp);
	
private:		// 录像机服务器接口添加
	
				/** 
				/* 功能：Daemon实例响应终端的注册请求
	*/
    void OnMtRegReq(CMessage * const pMsg, CApp * pcApp);

	/** 
	/* 功能：普通响应终端断链的处理
	*/
    void OnMtDisconnect(CMessage * const pMsg);

	/** 
	/* 功能：记录终端的节点信息
	/* 参数： 
	/*			dwNodeId : 节点号
	/*			dwInstId ：全局号
	/* 返回值： 无
	*/
    void SetMTNodeInfo(u32 dwNodeId, u32 dwInstId);

	/** 
	/* 功能：将消息发给录像机服务器（终端）
	/* 参数：
	/*        wEvent    : 事件号
	/*        pcServMsg : 事件内容，CServMsg结构
	/* 返回值： 发送成功，返回True，否则false
	*/
	BOOL32 SendMsgToRecSevr(u16 wEvent, CServMsg * const pcServMsg);

	/** 
	/* 功能：将消息发给录像机服务器（终端）的Daemon实例
	/* 参数：
	/*        wEvent    : 事件号
	/*        pcServMsg : 事件内容，CServMsg结构
	/* 返回值： 发送成功，返回True，否则false
	*/
	BOOL32 SendMsgToRecSevrDaem(u16 wEvent, CServMsg * const pcServMsg);
    
private:

    u32 m_dwMtNodeId;							// 终端的节点号
    u32 m_dwMtInstId;							// 终端的Instance号
    BOOL32 m_bIsMtConnect;						// 是否连接上了终端
	BOOL32 m_bIsMtMsg;							// 消息是不是要发给终端

	IWMSPublishingPoint *m_pPubPoint;
	IWMSBroadcastPublishingPoint *m_pBCPubPoint;
};

class CRecCfg
{
public:
    CRecCfg();
    ~CRecCfg();
public:
    u32    m_dwMcuNode;         // MCU.A 的节点号码
    u32    m_dwMcuIId;          // MCU.A APPID和Instance ID
    u32    m_dwMcuNodeB;        // MCU.A 的节点号码
    u32    m_dwMcuIIdB;         // MCU.A APPID和Instance ID
    u8     m_byRegAckNum;       // 第一次注册成功
	u32    m_dwMpcSSrc;         // MPC的会话参数
    u16    m_wRegTimes;         // 注册超时：1－reg to MCU.A; 2-reg to MCU.B; 3-both A and B
    u16    m_wDevCount;         // 设备计数器
    TEqp   m_tEqp;              // 本外设基本信息

    u16    m_dwMcuStartRcvPort; // mcu 侧开始接受端口
    
    // 正在使用的文件名,每个实例操作对应于像一应序号数组
    // 例如实例1正在操作的文件名为 m_achUsingFile[1]
    // 若该实例为操作文件则相应的内容为空
    s8     m_achUsingFile[MAXNUM_RECORDER_CHNNL+4][MAX_FILE_NAME_LEN];    
    
    TPrsTimeSpan       m_PrsParam;       // 重传时间跨度
    
    s8      m_szMcuAlias[MAX_ALIAS_LEN]; // 连接的Mcu别名
    s8      m_szWorkPath[MAX_FILE_PATH_LEN];    // 本次的工作路路径，如 E:\vod\McuName
    s8      m_achPublishPath[MAX_FILE_PATH_LEN]; // 发布路径 mms://usr:pas@KEDACOM/vod

	// [8/31/2010 liuxu] 添加终端的工作路径.每个终端独有一个工作路径
	s8		m_szMTWorkPath[MAXNUM_RECORDER_CHNNL][MAX_FILE_PATH_LEN];	// 终端的工作路径
public:
    // 返回工作路径
    LPCSTR  GetWorkingPath() ; 

	// [8/31/2010 liuxu] 返回终端的工作路径
	LPCSTR  GetMtWorkingPath(const u32&) ; 
	void FileTimeToTime_t(FILETIME cFileTime, time_t *pcTime);
    // 预处理文件名，拼接上VOD路径和MCU名称信息
    // lpszFullName [out] 需要自己预分配空间
    // 如把 conf1 和 a.asf 替换成 e:\vod\mcuname\conf1/a.asf
    
    u16 PreProcFileame(LPSTR lpszFullName, LPSTR lpszOldName, LPSTR lpszDirname) ;
    
    /**
	/*  预处理文件名，拼接上VOD路径和MCU名称信息
    /*  lpszFullName [out] 需要自己预分配空间
	/*	如把 conf1 和 a.asf 替换成 e:\vod\mcuname\conf1/a.asf
	*/
    u16 PreProcMtFileName(LPSTR lpszFullName, LPSTR lpszOldName, LPSTR lpszDirname, u32 dwMtId) ;

    // guzh [7/17/2007] 处理成RP库需要的Pattern名(不带路径）
    void    PreProcPatternName(LPSTR lpszPatternName, 
                               LPCSTR lpszOldNameWithoutDotAsf,
                               u8 byOwnerGrp, u16 wBitrate) ;

    // 反处理， 从完整文件名中分割目录和文件名，要求以 / 区分
    // 如 e:\vod\mcuname\conf/abc.asf
    // 分割为 e:\vod\mcuname\conf/ 和 abc.asf
    // 用户自己需要给出完整的 memset 0 过的空间
    void SplitPath(LPCSTR lpszFull, LPSTR lpszDir, LPSTR lpszName, BOOL32 bWithSlash) ;

    // 把完整路径切割成符合MMS要求的路径和名称
    void Path2MMS(LPCSTR lpszFullName, LPSTR lpszMMS) ;
    
    // 根据完整文件名获取纯文件名、会议码率和组ID
    BOOL32 GetParamFromFilename(LPCSTR lpszFullname, u8 &byGrpId, u16 &wBitrate, CString& cstrPureName);

	// [8/1/2011 liuxu] 用户自定义名称检验
	u16 CheckPureName( const s8* );

	void GetWHByRes(u8 byRes, u16 &wWidth, u16 &wHeight);
	void GetResByWH(u16 wWidth, u16 wHeight, u8 &byRes);
	
	BOOL32 IsConfHD(const TCapSupport &tCap);

	u16 OpenDB();
};

typedef zTemplate< CChnInst, MAXNUM_RECORDER_CHNNL, CRecCfg > CRecApp;
extern CRecApp g_cRecApp;
extern u32 g_adwRecDuration[MAXNUM_RECORDER_CHNNL+1];

extern IWMSServer *g_pServer;
extern IWMSPublishingPoints *g_pPubPoints;
//录像机退出信号量
extern SEMHANDLE g_hSemQuit;

#endif //!CHNINST_H

