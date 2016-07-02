/*****************************************************************************
   模块名      : mcu
   文件名      : mcuvc.h
   相关文件    : mcuvc.cpp
   文件实现功能: MCU业务应用类头文件
   作者        : 李屹
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2002/07/25  0.9         李屹        创建
   2002/07/25  0.9         李屹        创建
   2005/01/25  3.6         万春雷      MCU集成测试接口完善
   2005/02/19  3.6         万春雷      级联修改、与3.5版本合并
   2007/06/28  4.0         顾振华      类定义整理，优化
   2008/11/18  4.5         张宝卿      代码优化整理、高清HD集成 
******************************************************************************/

#ifndef __MCUVC_H_
#define __MCUVC_H_

#include "mcudata.h"
#include "agentinterface.h"
#include "nplusmanager.h"
#include "evnms.h"

//定时长度(毫秒)
const u32 TIMESPACE_SCHEDULED_CHECK		= 60 * 1000;	//定时检查预约会议是否到时
const u32 TIMESPACE_ONGOING_CHECK		= 60 * 1000;	//定时检查即时会议是否结束
const u32 TIMESPACE_INVITE_UNJOINEDMT	= 30 * 1000;	//定时邀请未与会终端
const u32 TIMERSPACE_SEND_FASTUPDATEPIC = 2  * 1000;    //定时自动更新图像
const u32 TIMESPACE_SWITCHDELAY			= 500;			//切换延时
const u32 TIMESPACE_WAIT_AUTOINVITE		= 50;			//等待MT邀请应答(秒)
const u32 TIMESPACE_WAIT_MPREG  		= 5 * 1000;	    //等待MP注册
const u32 TIMESPACE_MCUSRC_CHECK        = 2 * 1000;     //MCU源刷新
const u32 TIMESPACE_CHANGEVMPPARAM      = 1 * 1000;     //等待改变画面复合通道参数的时间
const u32 TIMESPACE_RECREATE_DATACONF   = 5 * 1000;		//重新创建数据会议(ms)
const u32 TIMESPACE_WAIT_CASCADE_CHECK  = 10 * 1000;    //等待级联通道打开(ms)
const u32 TIMESPACE_UPADATE_REGSTATUS   = 2 * 60 * 1000;//刷新辅接入板的实体时间间隔
const u32 TIMESPACE_WAIT_FSTVIDCHL_SUC	= 1 * 1000;		//等待第一路逻辑通道打开完成，再尝试第二路
const u32 TIMESPACE_APPLYFREQUENCE_CHECK= 10 * 1000;    //等待卫星会议申请频率的超时时间
const u32 TIMESPACE_SAT_MULTICAST_CHECK = 1500;			//等待卫星会议刷新超时时间间隔(ms)


//Daemon打印信息入口参数
const u16 DUMPMCINFO		= 1;
const u16 DUMPSRC			= 2;
const u16 DUMPEQPINFO		= 3;
const u16 DUMPEQPSTATUS		= 4;
const u16 DUMPTEMPLATE		= 5;
const u16 DUMPCONFMAP		= 6;

//会议打印信息入口参数
const u16 DUMPCONFALL       = 1;
const u16 DUMPCONFINFO      = 2;
const u16 DUMPCONFSWITCH    = 3;
const u16 DUMPCONFMT        = 4;
const u16 DUMPCONFEQP       = 5;
const u16 DUMPMTSTAT        = 6;
const u16 DUMPMTMONITOR     = 7;
const u16 DUMPBASINFO		= 8;
//const u16 DUMPADPGRPINFO	= 9;
//const u16 DUMPBASCHAN       = 10;
const u16 DUMPRLSCONF       = 11;
const u16 DUMPRLSBAS        = 12;
const u16 DUMPCHGMTRES      = 13;
const u16 DUMPVMPADAPT		= 14;


#define MCU_SAFE_DELETE(ptr)	{if(ptr){delete ptr;ptr=NULL;}}


// 单个会议的全部数据, len: 11K
class CMcuSingleConfAllInfo
{
public:

    TConfMtTable      *m_ptMtTable;              //会议终端表      
    TConfSwitchTable  *m_ptSwitchTable;          //会议交换表
    TConfOtherMcTable *m_ptConfOtherMcTable;     //其它Mc终端信息

    
    TConfInfo	     m_tConf;	                 //会议完整信息
	TConfAllMtInfo   m_tConfAllMtInfo;           //会议所有终端信息    
    TConfInnerStatus m_tConfInStatus;            //会议内部状态标识
    TConfProtectInfo m_tConfProtectInfo;         //会议保护信息
    TConfPollParam   m_tConfPollParam;           //会议轮询参数	
    TTvWallPollParam m_tTvWallPollParam;         //电视墙轮询参数
	THduPollParam    m_tHduPollParam;            //hdu轮询参数
    TConfEqpModule   m_tConfEqpModule;           //会议设备模板

    u8               m_byConfIdx;                //会议idx
    TMt              m_tCascadeMMCU; 		     //级联的上级MCU
    u8               m_byCreateBy;               //记录会议的创建者
protected:    

    TMt     m_tVidBrdSrc;                   //视频广播源
    TMt     m_tAudBrdSrc;                   //音频广播源
    TMt     m_tDoubleStreamSrc;             //双流源 
	u8		m_byLastDsSrcMtId;				//上次双流源MT Id
	TLogicalChannel m_tLogicChnnl;			// 记录lc (为了双流抢发)
    TH239TokenInfo m_tH239TokenOwnerInfo;   //H239双流令牌拥有终端信息
    
    TEqp    m_tVmpEqp;                      //画面合成外设
    TEqp    m_tVmpTwEqp;                    //复合电视墙外设
    TEqp	m_tMixEqp;                      //混音外设
    TEqp	m_tRecEqp;                      //录像外设
    TEqp	m_tPlayEqp;                     //放像外设
    TPlayEqpAttrib m_tPlayEqpAttrib;        //放像外设属性
    TPlayFileMediaInfo m_tPlayFileMediaInfo;    //放像文件媒体信息
    TRecStartPara m_tRecPara;               //录像参数
    
    TEqp	m_tAudBasEqp;					//音频适配外设
    TEqp	m_tVidBasEqp;					//视频适配外设
    TEqp	m_tBrBasEqp;					//码率适配外设
    TEqp    m_tCasdAudBasEqp;               //级联音频回传适配外设
    TEqp    m_tCasdVidBasEqp;               //级联视频回传适配外设
    TEqp    m_tPrsEqp;                      //包重传外设
    
    u8      m_byMixGrpId;					//混音组ID
    u8		m_byRecChnnl;					//录像信道号
    u8		m_byPlayChnnl;					//放像信道号
    u8      m_byPrsChnnl;                   //包重传第一视频通道
    u8      m_byPrsChnnl2;                  //包重传第二视频通道
    u8      m_byPrsChnnlAud;                //包重传音频通道
    u8      m_byPrsChnnlAudBas;             //音频适配包重传通道
    u8      m_byPrsChnnlVidBas;             //视频适配包重传通道
    u8      m_byPrsChnnlBrBas;              //码率适配包重传通道

	u8		m_byPrsChnnlVmpOut1;			//广播源1080出包重传通道
	u8		m_byPrsChnnlVmpOut2;			//广播源720出包重传通道
	u8		m_byPrsChnnlVmpOut3;			//广播源4Cif出包重传通道
	u8		m_byPrsChnnlVmpOut4;			//广播源Cif或other出包重传通道
	u8		m_byPrsChnnlDsVidBas;			//双流适配包重传通道	
	
    u8      m_byAudBasChnnl;				//音频适配通道号
    u8      m_byVidBasChnnl;				//视频适配通道号
    u8      m_byBrBasChnnl;					//码率适配通道号
    u8      m_byCasdAudBasChnnl;            //级联音频回传适配通道号
    u8      m_byCasdVidBasChnnl;            //级联视频回传适配通道号
    u16     m_wVidBasBitrate;				//视频适配的码率    
    u16     m_wBasBitrate;					//码率适配的码率
    u16     m_awVMPBrdBitrate[MAXNUM_MPU_OUTCHNNL];               //单速会议VMP的广播码率
#ifdef _MINIMCU_
    u8      m_byIsDoubleMediaConf;          //是否是双媒体会议(主码率采用主格式，辅码率采用辅格式
#endif

	u32		m_dwVmpLastVCUTick;				//上一次向VMP发送请求关键帧命令
	u32		m_dwVmpTwLastVCUTick;			//上一次向VMPTw发送请求关键帧命令
	u32		m_dwVidBasChnnlLastVCUTick;		//上一次向BAS（不同通道）发送请求关键帧命令
	u32		m_dwAudBasChnnlLastVCUTick;
	u32		m_dwBrBasChnnlLastVCUTick;
	u32		m_dwCasdAudBasChnnlLastVCUTick;
	u32		m_dwCasdVidBasChnnlLastVCUTick;
    
    //TBasCapSet m_tBasCapSet;              //码率适配的能力集    
    //u8      m_byRegGKDriId;               //本会议注册GK的MtAdp(其实现在所有会议都通过主适配注册，该地段意义不大）
        
    TRefreshParam m_tRefreshParam;          //Mcs刷新信息参数   

    u32     m_dwSpeakerViewId;				//用于级联时的发言人视频源
    u32     m_dwVmpViewId;					//用于级联时的视频复合视频源
    u32     m_dwSpeakerVideoId;				//用于级联时的发言人视频源的视频方案ID
    u32     m_dwVmpVideoId;					//用于级联时的视频复合视频源的视频方案ID
    u32     m_dwSpeakerAudioId;				//用于级联时的发言人的音频ID
    u32     m_dwMixerAudioId;				//用于级联时的混音器的音频ID
    TMt     m_tLastSpyMt;                   //上一次回传通道的终端，若为空则看自己(for h320 mcu cascade select)       
        
    u8		m_byDcsIdx;						//数据会议服务器id索引(Dcs会话的实例号)

    u16		m_wRestoreTimes;		        //会议恢复尝试次数	
                  
    TMt     m_tVacLastSpeaker;              //最后的语音激励成员
    TMt     m_tLastSpeaker;                 //上一次的发言人 
    TVMPParam	m_tLastVmpParam;			//上一次画面合成成员，用于Vmp动态分辨率恢复
    TVMPParam	m_tLastVmpTwParam;          //上一次VMPTW成员，用于Vmp动态分辨率恢复 

    TMixParam m_tLastMixParam;              //混音器断链前的状态

    TAcctSessionId m_tChargeSsnId;          //本会议计费SessionId

    TConfPollSwitchParam m_tPollSwitchParam;           //Radvision方式轮询回传参数
    u8      m_abyMixMtId[MAXNUM_MIXER_CHNNL+1];        //用于记录参与混音的终端的MtId
    u8      m_abyCasChnCheckTimes[MAXNUM_CONF_MT];     //打开级联通道的尝试次数    

    u8      m_abySerHdr[SERV_MSGHEAD_LEN];  //记录的创建会议时的消息头
    TMt     m_tRollCaller;                  //点名人
    TMt     m_tRollCallee;                  //被点名人

	u8	    m_bySpeakerVidSrcSpecType;		//发言人源是否是由会议自动指定
	u8		m_bySpeakerAudSrcSpecType;
    
    CRcvGrp m_cMtRcvGrp;                    //适配会议终端接收群组管理
    CBasMgr m_cBasMgr;                      //适配会议BAS通道和状态管理

	u8		m_byMtIdNotInvite;				// xliang [12/26/2008] Mt 主动呼MCU，
											//模板创会或起缺省会议情况下，
											//标记该MT预先不是受邀终端

	//****************  VMP related  ****************
	//--seize related:
	TVmpChnnlInfo   m_tVmpChnnlInfo;						//vmp通道信息，主要记录VMP前适配成员
	TVMPMember	    m_atVMPTmpMember[MAXNUM_MPUSVMP_MEMBER];//抢占做记录用
	BOOL32	m_bNewMemberSeize;								//表征此次抢占是新增了一个成员触发的抢占还是某成员升级为VIP身份触发的抢占
	u8		m_abyMtNeglectedByVmp[MAXNUM_CONF_MT/8];		//标记自动vmp时，不应参与的MT// xliang [12/17/2008] 目前不用, 仅预留
	
	//--speaker follow related:
	TLastVmpchnnlMemInfo m_tLastVmpChnnlMemInfo;			//用于发言人跟随，记录前次发言人占的通道成员以便恢复
	
	//--Batch poll related:
	TVmpBatchPollInfo	m_tVmpBatchPollInfo;				//vmp批量轮询信息 // xliang [12/18/2008] 
	TVmpPollParam		m_tVmpPollParam;					//vmp批量轮询参数 // xliang [12/18/2008]

	//--system performance related:
	u8		m_byVmpOperating;								//标记VMP正在操作	

	//u8	m_abyMtVmpChnnl[MAXNUM_CONF_MT];				//tVMPMember对应的VMP chnnl，抢占成功后,置通道派用场
	//TVmpCasMemInfo m_tVmpCasMemInfo;						//VMP 级联成员信息记录（级联的SrcMt不能做参考）	

	//********************** vmp related END *********************

    THduBatchPollInfo   m_tHduBatchPollInfo;    //hdu批量轮询信息
	THduPollSchemeInfo  m_tHduPollSchemeInfo;   //hdu批量轮询预案信息

	CVCCStatus m_cVCSConfStatus;                // VCS会议的独特的会议状态	
    CSelChnGrp m_cSelChnGrp;                    // 选看适配群组描述


    TPrsInfoManage m_tConfPrsInfo;              // 会议使用的PRS详细信息,目前仅针对HDBAS

    //高清平台集成卫星会议

    //网管信息保存
    u32		  m_dwMultiFreqSeq;                 //发送网管请求的序列号，用于区分回应的内容
    u32		  m_dwReceiveFreqSeq;               //发送网管请求的序列号，用于区分回应的内容
	u32		  m_dwSeq;                          //序列号

    //临时增加以下变量以接入卫星会议；未加入主备；考虑逐步
    BOOL32    m_bSchedStart;
    u32       m_dwOldDuration;
    u32       m_dwOldStart;
    u16       m_wOpMtId;
    u8        m_byConfSPortNum;

	u16		  m_wTicks;							//向终端多播信息,时隙
	u16		  m_wTimes;							//向终端多播信息,时间
	u8        m_bySendBitMap;					//时隙检测后要发送内容的表格

	u16       m_wMtTopoNum;						//保存发送到TOPO的MT号，因为可能分多个包
	u16		  m_wMtKeyNum;						//保存发送到Key的MT号，因为可能分多个包
};


// 业务实例(Osp功能类)
class CMcuVcInst : public CInstance,
                   public CMcuSingleConfAllInfo
{
public:
	enum 
	{ 
		STATE_IDLE,       //空闲
		STATE_WAITEQP,    //等待外设
        STATE_WAITAUTH,   //卫星会议等待网管频率申请应答
		STATE_PERIOK,	  //会议相关的外部条件准备ok
		STATE_ONGOING,    //即时
		STATE_SCHEDULED,  //预约
        STATE_MASTER,
        STATE_SLAVE
	};

public:	
    CMcuVcInst();
	virtual ~CMcuVcInst();

protected:

    /************************************************************************/
    /*                                                                      */
    /*                       一、daemon处理函数                             */
    /*                                                                      */
    /************************************************************************/

	//1. daemon实例处理函数
	void DaemonProcCommonNotif( const CMessage * pcMsg );

    void DaemonProcPowerOn( const CMessage * pcMsg );
    void DaemonProcMcuMSStateNtf( const CMessage * pcMsg );    

    //2. daemon 会议/模板管理
    void DaemonProcMcsMcuCreateConfReq(const CMessage * pcMsg);
    void DaemonProcMcsMcuCreateConfByTemplateReq(const CMessage * pcMsg);
    void DaemonProcMcsMcuSaveConfToTemplateReq( const CMessage * pcMsg );
    void DaemonProcCreateConfNPlus(const CMessage * pcMsg);
    void DaemonProcMcsMcuTemplateOpr(const CMessage * pcMsg);
    void DaemonProcMtMcuCreateConfReq( const CMessage * pcMsg );
    void DaemonProcMcsMcuListAllConfReq( const CMessage * pcMsg, CApp* pcApp );
    void DaemonProcDcsMcuReleaseConfRsp( const CMessage * pcMsg );  //2006-01-11
    BOOL32 NtfMcsMcuCurListAllConf( CServMsg &cServMsg );

    //3. daemon GK 管理
    void DaemonProcGkRegRsp(const CMessage * pcMsg);
    void DaemonProcGKChargeRsp(const CMessage * pcMsg);
    void DaemonProcMcuReRegisterGKNtf( const CMessage * pcMsg );
	
    //4. daemon 外设管理
	void ProcBasChnnlStatusNotif( const CMessage * pcMsg );
    void DaemonProcHDBasChnnlStatusNotif( const CMessage * pcMsg );
	void DaemonProcMcsMcuGetPeriEqpStatusReq( const CMessage * pcMsg );
	void DaemonProcRecMcuProgNotif( const CMessage * pcMsg );
	void DaemonProcRecMcuChnnlStatusNotif( const CMessage * pcMsg );
    void DaemonProcHduMcuChnnlStatusNotif( const CMessage * pcMsg );

    void DaemonProcMpFluxNotify( const CMessage * pcMsg );
    void DaemonProcPeriEqpMcuStatusNotif( const CMessage * pcMsg );
	void DaemonProcMcsMcuStopSwitchMcReq( const CMessage * pcMsg );
	void DaemonProcMcsMcuStopSwitchTWReq( const CMessage * pcMsg );
	
	void DaemonProcMcsMcuGetMcuStatusReq( const CMessage * pcMsg, CApp* pcApp );
	void DaemonProcMcsMcuSetMcuCfgCmd( const CMessage * pcMsg );	

	void DaemonProcMcuMtConnectedNotif( const CMessage * pcMsg );
	void DaemonProcMcuEqpConnectedNotif( const CMessage * pcMsg );
	void DaemonProcMcuEqpDisconnectedNotif( const CMessage * pcMsg );
	void DaemonProcMcuDcsConnectedNtf( const CMessage * pcMsg );	//2005-12-15
	void DaemonProcMcuDcsDisconnectedNtf( const CMessage * pcMsg );	//2005-12-15	
	void DaemonProcMcuMcsConnectedNotif( const CMessage * pcMsg );
	void DaemonProcMcuMcsDisconnectedNotif( const CMessage * pcMsg );
	void DaemonProcMtMcuApplyJoinReq( const CMessage * pcMsg );
    
    void DaemonProcNPlusRegBackupRsp( const CMessage * pcMsg );    
    void DaemonProcAppTaskRequest( const CMessage * pcMsg );
	void DaemonProcHDIFullNtf ( const CMessage *pcMsg );
    
    //5、Test
	void DaemonProcTestMcuGetAllMasterConfIdCmd( const CMessage* pcMsg );
	void DaemonProcTestMcuGetConfFullInfoCmd( const CMessage* pcMsg );	
    //主控热备份单元测试用函数
    void DaemonProcUnitTestMsg( CMessage *pcMsg );
    void ProcUnitTestGetTmpInfoReq( CMessage *pcMsg );
    void ProcUnitTestGetConfInfoReq( CMessage *pcMsg );
    void ProcUnitTestGetMtListInfoReq( CMessage *pcMsg );
    void ProcUnitTestGetAddrbookReq( CMessage *pcMsg );
    void ProcUnitTestGetMcuCfgReq( CMessage *pcMsg );
	
protected:
    
    /************************************************************************/
    /*                                                                      */
    /*                    二、会议管理及操作                                */
    /*                                                                      */
    /************************************************************************/

    //1、普通会议管理
	void ProcMcsMcuCreateConfReq( const CMessage * pcMsg );	
    void ProcMMcuMcuCreateConfByTemplateReq( const CMessage * pcMsg );	
	void ProcMcsMcuReleaseConfReq( const CMessage * pcMsg );
	void ProcMcsMcuModifyConfReq( const CMessage * pcMsg );
	void ProcMcsMcuSaveConfReq(  const CMessage * pcMsg );
	void ProcMcsMcuChangeLockModeConfReq( const CMessage * pcMsg );
	void ProcMcsMcuChangeConfPwdReq( const CMessage * pcMsg );
	void ProcMcsMcuEnterPwdRsp( const CMessage * pcMsg );
	void ProcMcsMcuDelayConfReq( const CMessage * pcMsg );
	void ProcMcsMcuChangeVacHoldTimeReq( const CMessage * pcMsg );
	void ProcTimerOngoingCheck( const CMessage * pcMsg );
	void ProcTimerScheduledCheck( const CMessage * pcMsg );
	void ProcTimerWaitMpRegCheck( const CMessage * pcMsg );
	void ProcTimerVacChange( const CMessage * pcMsg );
    void ProcTimerAutoRec( const CMessage * pcMsg );
    void ProcWaitEqpToCreateConf(CMessage * const pcMsg);

    void CreateConfEqpInsufficientNack( u8 byCreateMtId, u8 byOldInsId, const s32 nErrCode, CServMsg &cServMsg );
    void CreateConfSendMcsNack( u8 byOldInsId, const s32 nErrCode, CServMsg &cServMsg, BOOL32 bDeleteAlias = FALSE );
    BOOL32 IsConfInfoCheckPass( const CServMsg &cSrcMsg, TConfInfo &tConfInfo, u16 &wErrCode, BOOL32 bTemplate = FALSE);
    BOOL32 IsEqpInfoCheckPass( const CServMsg &cSrcMsg, u16 &wErrCode);
    void ConfDataCoordinate(CServMsg &cMsg, u8 &byMtNum, u8 &byCreateMtId);
    void AddSecDSCapByDebug(TConfInfo &tConfInfo);

	BOOL32 PrepareAllNeedBasForConf(u16* pwErrCode = NULL);
	void   RlsAllBasForConf();

    void ReleaseConf( BOOL32 bRemoveFromFile = TRUE);
    u32  PackMtInfoListIntoBuf(const TMt atMt[], const TMtAlias	atMtAlias[],
                               u8	byArraySize, u8 *pbyBuf, u32 dwBufLen );	
    void ChangeConfLockMode( CServMsg &cMsg );
    
    void SendReplyBack( CServMsg & cReplyMsg, u16 wEvent );
    BOOL32 IsMtCmdFromThisMcu( const CServMsg & cCmdReply ) const;
    BOOL32 IsMcCmdFromThisMcu( const CServMsg & cCmdReply ) const;
    BOOL32 SendMsgToMt( u8 byMtId, u16 wEvent, CServMsg & cServMsg );
    BOOL32 SendMsgToEqp( u8 byEqpId, u16 wEvent, CServMsg & cServMsg );
    BOOL32 SendMsgToMcs( u8 byMcsId, u16 wEvent, CServMsg & cServMsg );
    BOOL32 SendOneMsgToMcs( CServMsg &cServMsg, u16 wEvent );
    void SendMsgToAllMcs( u16 wEvent, CServMsg & cServMsg );
    void SendMsgToAllMp( u16 wEvent, CServMsg & cServMsg);  // libo [4/27/2005] add
    void SendMtListToMcs(u8 buMcuId);
    void SendMtAliasToMcs(TMt tMt);
    void BroadcastToAllSubMtJoinedConf( u16 wEvent, CServMsg & cServMsg );
    void BroadcastToAllMcu( u16 wEvent, CServMsg & cServMsg, u8 byExceptMc = 0 );
    BOOL32 SendMsgToDcsSsn( u8 byInst, CServMsg &cServMsg ); //2005-12-16
    
    void SendTrapMsg( u16 wEvent, u8 * const pbyMsg, u16 wLen );    
    
    void NotifyMtSend( u8 byDstMtId, u8 byMode = MODE_BOTH, BOOL32 bStart = TRUE );
	void NotifyEqpSend( u8 byDstEqpId, u8 byMode = MODE_BOTH, BOOL32 bStart = TRUE );
    void NotifyMtReceive(  const TMt & tSrc, u8 byDstMtId );	
    void StartMtSendbyNeed( TSwitchChannel* ptSwitchChannel );
    void StopMtSendbyNeed( TSwitchChannel* ptSwitchChannel );
    void NotifyRecStartPlaybyNeed( TSwitchChannel* ptSwitchChannel ); // zgc, 2007-02-27
    void NotifyOtherMtSend( u8 byDstMtId, BOOL32 bStart );  //通知其他厂商终端是否发送视频码流        	
    // guzh [7/27/2007] 请求VCU，发送给终端，外设
    void NotifyFastUpdate(const TMt &tDst, u8 byChnlMode, BOOL32 bSetTimer = FALSE);
    void NotifyMtFastUpdate(u8 byMtId, u8 byChnlMode, BOOL32 bSetTimer = FALSE);
    void NotifyEqpFastUpdate(const TMt &tDst, u8 byChnlMode, BOOL32 bSetTimer = FALSE);
    

	//2、数据会议管理及操作
	void SendMcuDcsCreateConfReq();
	void SendMcuDcsReleaseConfReq();
	void ProcDcsMcuCreateConfRsp( const CMessage * pcMsg );
	BOOL32 FindOnlineDcs();
	void ResetInstDataStatus();

	void SendMcuDcsAddMtReq( u8 byMtId, u32 dwMtIp = 0, u16 wPort = 0 );
	void SendMcuDcsDelMtReq( u8 byMtId );
	void ReInviteHasBeenInDataConfMt();
	void ProcDcsMcuAddMtRsp( const CMessage * pcMsg );
	void ProcDcsMcuDelMtRsp( const CMessage * pcMsg );
	void ProcDcsMcuMtJoinedNtf( const CMessage * pcMsg );
	void ProcDcsMcuMtLeftNtf( const CMessage * pcMsg );
	void ProcDcsMcuStatusNotif( const CMessage * pcMsg );
	void ProcTimerRecreateDataConf( const CMessage * pcMsg );
    
    //3、gk 计费
    void ProcConfRegGkAck( const CMessage * pcMsg );
	void ProcConfRegGkNack( const CMessage * pcMsg );
    void ProcGKChargeRsp( const CMessage * pcMsg );
	
	//4、普通会议操作 
    void ChangeVidBrdSrc( TMt * ptNewVidBrdSrc );
    void ChangeAudBrdSrc( TMt * ptNewAudBrdSrc );
    void ChangeSpeaker(  TMt * ptNewSpeaker, BOOL32 bPolling = FALSE, BOOL32 bAddToVmp = TRUE );
    void ChangeChairman(  TMt * ptNewChairman );
    void AdjustChairmanSwitch( TMt * ptOldChair = NULL );
	void AdjustChairmanVidSwitch( TMt *ptOldChair = NULL );
	void AdjustChairmanAudSwitch( void );
    
    TMt  GetLocalSpeaker( void );
    BOOL32 HasJoinedSpeaker( void );
	BOOL32 HasJoinedChairman( void );
	BOOL32 IsSelectedbyOtherMt( u8 bySrcMtId,u8 byMode = MODE_VIDEO);
	BOOL32 IsSpeakerCanBrdVid(void);
	BOOL32 IsSpeakerCanBrdAud(void);
	void   AdjustOldSpeakerSwitch( TMt tOldSpeaker, BOOL32 bIsHaveNewSpeaker );
	void   AdjustNewSpeakerSwitch( TMt tNewSpeaker, BOOL32 bAddToVmp );
	void   ChangeSpeakerSrc( u8 byMode, emChangeSpeakerSrcReason emReason);
	void   ChangeOldSpeakerSrc( u8 byMode, TMt tOldSpeaker, BOOL32 bIsHaveNewSpeaker );
    
    void ConfStatusChange( void );
    void ConfModeChange( void );
    void MtStatusChange( u8 byMtId = 0, BOOL32 bForcely = FALSE );
    void MtInfoChange( void );
    void MtOnlineChange( TMt tMt, BOOL32 bOnline, u8 byReason );
    void NotifyMcsAlarmInfo(u8 byMcsId, u16 wErrorCode);    //byMcsId为0则通知所有mcs
    void SendConfInfoToChairMt(void);
    void ProcTimerRefreshMcs(const CMessage * pcMsg);    

	void ProcMcsMcuSpecChairmanReq( const CMessage * pcMsg );
	void ProcMcsMcuCancelChairmanReq( const CMessage * pcMsg );
	void ProcMcsMcuSetConfChairMode( const CMessage * pcMsg );
	void ProcMcsMcuSpecSpeakerReq( const CMessage * pcMsg );
	void ProcMcsMcuCancelSpeakerReq( const CMessage * pcMsg );
	void ProcMcsMcuSeeSpeakerCmd( const CMessage * pcMsg );
    void ProcMcsMcuSpecOutViewReq( const CMessage * pcMsg );
	void ProcMcsMcuAddMtReq( const CMessage * pcMsg );
	void ProcMcsMcuAddMtExReq( const CMessage * pcMsg );
	void ProcMcsMcuDelMtReq( const CMessage * pcMsg );
	void ProcMcsMcuSendMsgReq( const CMessage * pcMsg );
	void ProcMcsMcuGetMtListReq( const CMessage * pcMsg );
	void ProcMcsMcuRefreshMcuCmd( const CMessage * pcMsg); 
	void ProcMcsMcuGetAllMtAliasReq( const CMessage * pcMsg );
	void ProcMcsMcuGetConfInfoReq( const CMessage * pcMsg );
	void ProcMcsMcuVMPReq(const CMessage * pcMsg);
    void VmpCommonReq(CServMsg & cServMsg);
    void ProcMcsMcuVmpTwReq(const CMessage * pcMsg);
    void VmpTwCommonReq(CServMsg & cServMsg);
	void ProcMcsMcuGetConfStatusReq( const CMessage * pcMsg );
	void ProcMcsMcuGetLockInfoReq( const CMessage * pcMsg );
	void ProcMcsMcuVACReq(const CMessage * pcMsg);
    void MixerVACReq(CServMsg & cServMsg);
	void ProcMcsMcuMixReq(const CMessage * pcMsg);
    void ProcMcsMcuChgMixDelayReq(const CMessage * pcMsg);
    //void MixerCommonReq(CServMsg & cServMsg);
	void ProcMcsMcuCamCtrlCmd( const CMessage * pcMsg );
	void ProcMMcuMcuCamCtrlCmd( const CMessage * pcMsg );
	void ProcMcsMcuMtOperCmd( const CMessage * pcMsg );
	void ProcMcsMcuMtOperReq( const CMessage * pcMsg );
	void ProcMcsMcuPollMsg(const CMessage * pcMsg);
	void ProcPollingChangeTimerMsg(const CMessage *pcMsg);    
    void ProcStopConfPoll();
    void ProcMcsMcuHduPollMsg(const CMessage *pcMsg);
	void ProcMcsMcuHduBatchPollMsg(const CMessage *pcMsg);    //  处理hdu批量轮询
    void ProcMcsMcuTWPollMsg(const CMessage *pcMsg);
    void ProcTWPollingChangeTimerMsg(const CMessage *pcMsg);
	void ProcHduPollingChangeTimerMsg(const CMessage *pcMsg);
    void ProcHduBatchPollChangeTimerMsg(const CMessage *pcMsg);  // hdu批量定时处理
	void ProcMcsMcuMcuMediaSrcReq(const CMessage *pcMsg);
	TMtPollParam *GetNextMtPolled(u8 &byPollPos, TPollInfo& tPollInfo);
    TMtPollParam *GetMtTWPollParam(u8 &byPollPos);
    TMtPollParam *GetMtHduPollParam(u8 &byPollPos);

	void ProcMcsMcuLockSMcuReq(const CMessage *pcMsg);
	void ProcMcsMcuGetMcuLockStatusReq(const CMessage *pcMsg);
    void NoifyMtToBePolledNext( void );
    void PollParamChangeNotify( void );
    void ProcMcsMcuRollCallMsg( const CMessage * pcMsg );
    void RollCallStop( CServMsg &cServMsg );
    void ProcMixStart( CServMsg &cServMsg );
    void ProcMixStop( CServMsg &cServMsg, BOOL32 bRollCallStop = FALSE );
	// xliang [12/12/2008] mpu-vmp related
	void ProcMcsMcuVmpPriSeizeRsp(const CMessage *pcMsg);//vmp抢占应答
	void ProcMtSeizeVmpTimer(const CMessage *pcMsg);	//最初要抢占的MT进行抢占操作
	void ProcVmpBatchPollTimer(const CMessage *pcMsg);	//vmp批量poll
	void ProcMcsMcuVmpCmd(const CMessage * pcMsg);		//处理批量poll PAUSE,RESUME等命令
	void ProcChangeVmpParamReq(CServMsg &cServMsg);		//改变画面合成参数
	void ProcStartVmpReq(CServMsg &cServMsg);			//开始画面合成
	void ClearOneVmpMember(u8 byChnnl, TVMPParam &tVmpParam, BOOL32 bRecover = FALSE);
    //5、卫星分散会议
    u8   GetSatCastChnnlNum(u8 bySrcMtId);
    BOOL32 IsOverSatCastChnnlNum(u8 bySrcMtId, u8 byIncNum = 1);
    BOOL32 IsOverSatCastChnnlNum(TVMPParam &tVmpParam);
    void ChangeSatDConfMtRcvAddr(u8 byMtId, u8 byChnnlType, BOOL32 bMulti = TRUE);
    BOOL32 IsMtSendingVidToOthers(TMt tMt, BOOL32 bConfPoll/*with audio*/, BOOL32 bTwPoll, u8 bySelDstMtId);


    //6、N+1备份相关
    void ProcNPlusConfInfoUpdate(BOOL32 bStart);
    void ProcNPlusConfMtInfoUpdate(void);
    void ProcNPlusChairmanUpdate(void);
    void ProcNPlusSpeakerUpdate(void);
    void ProcNPlusVmpUpdate(void);
    void ProcNPlusConfDataUpdate(const CMessage * pcMsg);
    void GetNPlusDataFromConf(TNPlusConfData &tNPlusData, BOOL32 bCharge = FALSE);
    
    //7、主控热备份：M-S Data-Exchange
public:
    BOOL32 GetInstState( u8 &byState );
    BOOL32 SetInstState( u8 byState );
    
    BOOL32 HangupInstTimer( void );
    BOOL32 ResumeInstTimer( void );
    
    BOOL32 GetConfMtTableData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen );
    BOOL32 SetConfMtTableData( u8 *pbyBuf, u32 dwInBufLen );
    BOOL32 GetConfSwitchTableData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen );
    BOOL32 SetConfSwitchTableData( u8 *pbyBuf, u32 dwInBufLen );
    BOOL32 GetConfOtherMcTableData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen );
    BOOL32 SetConfOtherMcTableData( u8 *pbyBuf, u32 dwInBufLen );
    BOOL32 GetInstOtherData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen );
    BOOL32 SetInstOtherData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bResumeTimer );
 
	//8、VCS会议处理
	// VCS相关消息处理入口
	void ProcVcsMcuMsg( const CMessage * pcMsg );
	// 创建vcs会议
	void ProcVcsMcuCreateConfReq( const CMessage * pcMsg );
	// 终端调度请求处理
    void ProcVcsMcuVCMTReq( const CMessage * pcMsg );
	// 当前会议的调度模式请求处理
	void ProcVcsMcuVCModeReq( const CMessage * pcMsg );
	// 会议模式切换请求
	void ProcVcsMcuVCModeChgReq( const CMessage * pcMsg );
	// 会议静哑音请求处理
	void ProcVcsMcuMuteReq( const CMessage * pcMsg );
	// 调度终端超时处理
	void ProcVCMTOverTime(const CMessage * pcMsg);
	// 组呼终端请求处理
	void ProcVcsMcuGroupCallMtReq(const CMessage * pcMsg);
	void ProcVcsMcuGroupDropMtReq(const CMessage * pcMsg);
	// 主席轮询
    void ProcVcsMcuStartChairPollReq(const CMessage *pcMsg);
    void ProcVcsMcuStopChairPollReq(const CMessage *pcMsg);
    void ProcChairPollTimer(const CMessage *pcMsg);
	// 临时增加终端
	void ProcVcsMcuAddMtReq(const CMessage *pcMsg);
	void ProcVcsMcuDelMtReq(const CMessage *pcMsg);
	// 抢占处理, 是否同意释放终端
	void ProcVcsMcuRlsMtMsg(const CMessage * pcMsg);


	// VCS会议所需外设资源的检查
	BOOL32 IsVCSEqpInfoCheckPass(u16& wErrCode);
	// VCS会议信息的整理
    void VCSConfDataCoordinate(CServMsg& cServMsg, u8 &byMtNum, u8 &byCreateMtId);
	// VCS会议中实体呼叫模式查询
	u8   VCSGetCallMode(TMt tMt);

	// VCS呼叫挂断终端
	void VCSCallMT(CServMsg& cServMsgHdr, TMt& tMt, u8 byCallType);
	void VCSDropMT(TMt tMt);

	// VCS选看操作
	BOOL VCSConfSelMT(TSwitchInfo tSwitchInfo);
	void VCSConfStopMTSel(TMt tDstMt, u8 byMode);    // 取消终端选看
	void VCSConfStopMTSeled(TMt tSrcMt, u8 byMode);  // 取消终端被选看
    BOOL IsNeedSelApt(u8 bySrcMtId, u8 byDstMtId, u8 bySwitchMode);

	BOOL VCSConfMTToTW(TTWSwitchInfo tTWSwitchInfo);
	void VCSConfStopMTToTW(TMt tMt);
    // 对终端静哑音
	void VCSMTMute(TMt tMt, u8 byMuteOpenFlag, u8 byMuteType);


	// 根据会议状态，继续进行未完成的选看步骤
	void GoOnSelStep(TMt tMt, u8 byMediaType, u8 bySelDirect);
	// 根据会议状态，切换当前调度终端
	void ChgCurVCMT(TMt tNewCurVCMT);

	void VCSSwitchSrcToDst(const TSwitchInfo &tSwitchInfo);
	u16  FindUsableTWChan(u16 wCurUseChanIndex, TMt* ptDropOutMT = NULL, TEqp* tEqp = NULL, u8* byChanIdx = NULL, TMt* ptInMT = NULL);
	BOOL FindUsableVMP();
	u8   GetVCSAutoVmpMember(TMt* ptVmpMember);
	void SetVCSVmpMemInChanl(TMt* pVmpMember, u8 byVmpSytle);
    void ChangeSpecMixMember(TMt* ptMixMember, u8 byMemberNum);
	void RestoreVCConf(u8 byNewMode);
	void ChangeVFormat(TMt tMt, u8 byNewFormat);
	void VCSMTAbilityNotif(TMt tMt, u8 byMediaType);
	void VCSChairOffLineProc();
    void VCSConfStatusNotif();

	TMt  VCSGetNextPollMt();
	BOOL32 SwitchToAutoMixing();

	
private: 
    u32  OprInstOtherData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bGet );
	u32  GetInstOtherDataLen( void );  


    /************************************************************************/
    /*                                                                      */
    /*                       三、终端管理及操作                             */
    /*                                                                      */
    /************************************************************************/

protected:	

    //1、普通操作
	void ProcMtMcuFlowControlCmd( const CMessage * pcMsg, BOOL32 bNtfPhyBandWidth = FALSE );
	void ProcPolycomMcuFlowctrlCmd(u8 byPolyMCUId, TLogicalChannel &tLogicChnnl);
	void ProcMtMcuFlowControlIndication( const CMessage * pcMsg );
	void ProcMtMcuCapSetNotif( const CMessage * pcMsg );
	void ProcMtMcuMtAliasNotif( const CMessage * pcMsg );
	void ProcMtMcuEnterPwdReq( const CMessage * pcMsg ); 
    void ProcMtMcuEnterPwdRsp( const CMessage * pcMsg );
    void ProcMtMcuOpenLogicChnnlReq( const CMessage * pcMsg );
	void ProcMtMcuOpenLogicChnnlRsp( const CMessage * pcMsg );
	void ProcMtMcuCloseLogicChnnlNotify( const CMessage * pcMsg );
	void ProcMtMcuMediaLoopOpr( const CMessage *pcMsg );
	void ProcMtMcuGetH239TokenReq( const CMessage * pcMsg );
	void ProcMtMcuOwnH239TokenNotify( const CMessage * pcMsg );
	void ProcMtMcuReleaseH239TokenNotify( const CMessage * pcMsg );
	void ProcPolyMCUH239Rsp( const CMessage * pcMsg );
	void ProcMtMcuStopSwitchMtReq( const CMessage * pcMsg );
	void ProcMtMcuStartSwitchMtReq( const CMessage * pcMsg );
	void ProcMtMcuSendMcMsgReq( const CMessage * pcMsg );
	void ProcMtMcuApplyJoinReq( const CMessage * pcMsg );
	void ProcMtMcuApplyChairmanReq( const CMessage * pcMsg );
	void ProcMtMcuApplySpeakerNotif( const CMessage * pcMsg );
	void ProcMtMcuGetMtAliasReq( const CMessage * pcMsg );
	void ProcMtMcuJoinedMtListReq( const CMessage * pcMsg );
	void ProcMtMcuJoinedMtListIdReq( const CMessage * pcMsg );
	void ProcMcsMcuCallMtReq( const CMessage * pcMsg );
	void ProcMcsMcuDropMtReq( const CMessage * pcMsg );
	void ProcMcsMcuSetCallMtModeReq( const CMessage * pcMsg );
	void ProcCommonOperNack( const CMessage * pcMsg );
	void ProcCommonOperAck( const CMessage * pcMsg );
	void ProcMtMcuMtStatusNotif( const CMessage * pcMsg );
	void ProcMcsMcuGetMtStatusReq( const CMessage * pcMsg );
	void ProcMcsMcuGetAllMtStatusReq( const CMessage * pcMsg );
    void ProcMcsMcuGetMtBitrateReq( const CMessage *pcMsg );
    void ProcMcsMcuGetMtExtInfoReq( const CMessage *pcMsg );
	void ProcMtMcuInviteMtNack( const CMessage * pcMsg );
	void ProcMtMcuInviteMtAck( const CMessage * pcMsg );
	void ProcMtMcuDisconnectedNotif( const CMessage * pcMsg );
	void ProcMtMcuConnectedNotif( const CMessage * pcMsg );
	void ProcMtMcuMtJoinNotif( const CMessage * pcMsg );
	void ProcMtMcuMtTypeNotif( const CMessage * pcMsg );
	void CallFailureNotify(CServMsg	&cServMsg);
	void ProcMtMcuCallFailureNotify(const CMessage * pcMsg);
	void ProcMtMcuGetChairmanReq( const CMessage * pcMsg );
	void ProcMcuMtFastUpdatePic( const CMessage * pcMsg );
	void ProcMtMcuMsdRsp( const CMessage * pcMsg );
	void ProcMtMcuMatrixMsg( const CMessage * pcMsg );
	void ProcMtMcuOtherMsg( const CMessage * pcMsg );
	void ProcMtMcuFastUpdatePic( const CMessage * pcMsg );
	void ProcMtMcuMixReq( const CMessage * pcMsg );
	void ProcMtMcuReleaseMtReq( const CMessage * pcMsg );
	void ProcTimerInviteUnjoinedMt( const CMessage * pcMsg );
	u8   AddMt( TMtAlias &tMtAlias, u16 wMtDialRate = 0, u8 byCallMode = CONF_CALLMODE_TIMER, BOOL bPassiveAdd = FALSE );
	void AddJoinedMt( TMt & tMt );
	void RemoveMt( TMt & tMt, BOOL32 bMsg );
	void RemoveJoinedMt(TMt & tMt, BOOL32 bMsg, u8 byReason = MTLEFT_REASON_NORMAL,BOOL32 bNeglectMtType = FALSE);
	BOOL32 DealMtMcuEnterPwdRsp(TMt tMt, s8 *pszPwd, s32 nPwdLen);
    void ProcMtMcuBandwidthNotif(const CMessage *pcMsg);
	void RestoreVidSrcBitrate(TMt tMt, u8 byMediaType);
	void ProcMcsMcuSetMtVolumeCmd(const CMessage * pcMsg);
    void MtVideoSourceSwitched(CServMsg & cServMsg);
    void ProcMtMcuVideoSourceSwitched(const CMessage * pcMsg);
    void ChangeMemberVolume( const TMt & tMember, u8 byVolume );
    void SetMemberVolumeReq( const TMt & tMember, u8 byVolume );

    BOOL32 AssignMpForMt( TMt & tMt );
    void ReleaseMtRes( u8 byMtId, BOOL32 bForced = FALSE );	
    void InviteUnjoinedMt( CServMsg& cServMsgHdr, const TMt* ptMt = NULL, 
		                   BOOL32 bSendAlert = FALSE, BOOL32 bBrdSend = FALSE,
						   u8 byCallType = VCS_FORCECALL_REQ);

    void ProcMtCapSet( TMt tMt, TCapSupport &tCapSupport, const TCapSupportEx &tCapSupportEx );	

	//2、级联MCU-MCU
	void ProcMcuMcuRegisterNotify( const CMessage * pcMsg );
	void ProcMcuMcuRosterNotify( const CMessage * pcMsg );
	void ProcMcuMcuMtListReq( const CMessage * pcMsg );
	void ProcMcuMcuMtListAck( const CMessage * pcMsg );
	void ProcMcuMcuVideoInfoReq( const CMessage * pcMsg );
	void ProcMcuMcuVideoInfoAck( const CMessage * pcMsg );
	void ProcMcuMcuConfViewChgNtf( const CMessage * pcMsg );
	void ProcMcuMcuAudioInfoReq( const CMessage * pcMsg );
	void ProcMcuMcuAudioInfoAck( const CMessage * pcMsg );
	void ProcMcuMcuAddMtReq( const CMessage * pcMsg );
	void ProcMcuMcuDropMtNotify( const CMessage * pcMsg );
	void ProcMcuMcuDelMtNotify( const CMessage * pcMsg );
	void ProcMcuMcuSetMtChanReq( const CMessage * pcMsg);
	void ProcMcuMcuSetMtChanNotify( const CMessage * pcMsg);
	void ProcMcuMcuNewMtNotify( const CMessage * pcMsg);
	void ProcMcuMcuCallAlertMtNotify( const CMessage * pcMsg);

	void ProcMcuMcuSetInReq( const CMessage * pcMsg);
	void ProcMcuMcuSetOutReq( const CMessage * pcMsg);
	void ProcMcuMcuSetOutNotify( const CMessage * pcMsg);

	void ProcMcuMcuStartMixerCmd( const CMessage * pcMsg);
	void ProcMcuMcuStartMixerNotif( const CMessage * pcMsg);
	void ProcMcuMcuStopMixerCmd( const CMessage * pcMsg);
	void ProcMcuMcuStopMixerNotif( const CMessage * pcMsg);
	void ProcMcuMcuGetMixerParamReq( const CMessage * pcMsg);
	void ProcMcuMcuGetMixerParamAck( const CMessage * pcMsg);
	void ProcMcuMcuGetMixerParamNack( const CMessage * pcMsg);
	void ProcMcuMcuMixerParamNotif( const CMessage * pcMsg);
	void ProcMcuMcuAddMixerMemberCmd( const CMessage * pcMsg);
	void ProcMcuMcuRemoveMixerMemberCmd( const CMessage * pcMsg);

	void ProcMcuMcuLockReq( const CMessage * pcMsg);
	void ProcMcuMcuLockAck( const CMessage * pcMsg);
	void ProcMcuMcuLockNack( const CMessage * pcMsg);
	
	void ProcMcuMcuMtStatusCmd( const CMessage *pcMsg );
	void ProcMcuMcuMtStatusNotif( const CMessage *pcMsg );
    
    void ProcMcuMcuAutoSwitchReq( const CMessage *pcMsg );
    void ProcMcuMcuAutoSwitchRsp( const CMessage *pcMsg );
    void ProcMcuMcuAutoSwitchTimer( const CMessage *pcMsg );
	
	void CascadeAdjMtRes( TMt tMt, u8 bNewFormat, BOOL32 bStart = TRUE, u8 byVmpStyle = ~0, u8 byPos = ~0);
	void ProcMcuMcuAdjustMtResReq ( const CMessage *pcMsg );	
	void ProcMcuMcuAdjustMtResAck ( const CMessage *pcMsg );	

	void OnStartMixToSMcu(CServMsg *pcSerMsg);
	void OnStopMixToSMcu(CServMsg *pcSerMsg);
	void OnGetMixParamToSMcu(CServMsg *pcSerMsg);
	void OnAddRemoveMixToSMcu(CServMsg *pcSerMsg, BOOL32 bAdd);

	void OnStartDiscussToAllSMcu(CServMsg *pcSerMsg);
	void OnStopDiscussToAllSMcu(CServMsg *pcSerMsg);

	void OnGetMtStatusCmdToSMcu( u8 byDstMcuId );
	void OnNtfMtStatusToMMcu( u8 byDstMcuId, u8 byMtId = 0 );

	void NotifyMcuNewMt(TMt tMt);
	void NotifyMcuDelMt(TMt tMt);
	void NotifyMcuDropMt(TMt tMt);
	
	void ProcMcuMcuSendMsgReq( const CMessage * pcMsg );

	TConfMcInfo *GetConfMcInfo( u8 byMcuId );
	BOOL32 GetMcData( u8 byMcuId, TConfMtInfo &tConfMtInfo, TConfMcInfo &tConfOtherMcInfo );

	TMt  GetLocalMtFromOtherMcuMt( TMt tMt );
	
	TMt  GetMcuMediaSrc(u8 byMcuId);
	
	void OnMMcuSetIn(TMt &tMt, u8 byMcsSsnId, u8 bySwitchMode, BOOL32 bPolling = FALSE);
    void OnSetOutView(TMt &tSetInMt);

    // guzh [5/14/2007] 获取会议音视频信息
    void GetConfViewInfo(u8 byMcuId, TCConfViewInfo &tInfo);
    void GetLocalAudioInfo(TCConfAudioInfo &tInfo);    
	   
    // guzh [5/14/2007] 通知各个级联MCU本级媒体源
    void NotifyAllSMcuMediaInfo(u8 byTargetMcuId, u8 byMode);
    void NofityMMcuMediaInfo();

    //尝试多次打开级联通道
    void ProcTimerReopenCascadeChannel();

    //mcu source
    void SendMcuMediaSrcNotify(BOOL32 bFource = FALSE);
    void ProcTimerMcuSrcCheck(const CMessage * pcMsg);
    
    void ProcMcuMtFastUpdateTimer( const CMessage * pcMsg );
    void ProcMcuEqpFastUpdateTimer( const CMessage * pcMsg );
    void ProcMcuMtSecVideoFastUpdateTimer( const CMessage * pcMsg );
    
    void ProcTimerChangeVmpParam(const CMessage * pcMsg);

    //3、双流相关
    void StopDoubleStream( BOOL32 bMsgSrcMt, BOOL32 bForceClose );
    void StartDoubleStream(TMt &tMt, TLogicalChannel &tLogicChnnl);
    void StartSwitchDStreamToFirstLChannel( TMt &tDstMt );
    void StopSwitchDStreamToFirstLChannel( TMt &tDstMt );
    BOOL32 JudgeIfSwitchFirstLChannel( TMt &tSrc, u8 bySrcChnnl, TMt &tDstMt );
    void ClearH239TokenOwnerInfo(TMt *ptMt);
    void UpdateH239TokenOwnerInfo(TMt &tMt);
    void NotifyH239TokenOwnerInfo(TMt *ptMt);
    void McuMtOpenDoubleStreamChnnl(TMt &tMt, const TLogicalChannel &tLogicChnnl, const TCapSupport &tCapSupport);//打开双流通道
    void ProcSendFlowctrlToDSMtTimeout(const CMessage * pcMsg );
    void ProcSmcuOpenDVideoChnnlTimer(const CMessage * pcMsg);
    u16    GetDoubleStreamVideoBitrate(u16 wDialBitrate, BOOL32 bFst = TRUE); //获取双流视频码率，默认第一路
	BOOL32 IsDStreamMtSendingVideo(TMt tMt);
    
    //4、选看相关
    void RestoreMtSelectStatus( u8 byMtId, u8 bySelMode );       
    BOOL ChangeSelectSrc(TMt tSrcMt, TMt tDstMt, u8 byMode = MODE_BOTH);
    void StopSelectSrc(TMt tDstMt, u8 byMode = MODE_BOTH, u8 byIsRestore = TRUE );
    //选看模式和选看源、目的的能力级是否匹配
    BOOL32 IsSelModeAndCapMatched( u8               &bySwitchMode,
                                   const TSimCapSet &tSimCapSrc,
                                   const TSimCapSet &tSimCapDst,
                                   BOOL32           &bAccord2Adp);
    void ProcMtSelectdByOtherMt( u8 byMtId, BOOL32 bStartSwitch  );    

    //5、其他
    void AdjustMtVideoSrcBR( u8 byMtId, u16 wBitrate, u8 byMode = MODE_VIDEO );
    BOOL32 IsNeedAdjustMtSndBitrate(u8 byMtId, u16 &wSndBitrate);
    BOOL32 IsMtMatchedSrc( u8 byMtId, u8 &byChnRes, BOOL32 &bAccord2MainCap);
	void	SwitchNewVmpToSingleMt(TMt tMt);
	BOOL32 GetMtMatchedRes(u8 byMtId, u8 byChnType, u8& byChnRes);
	BOOL32 FindAcceptCodeChnl(u8 byMtId, u8 byMode, u8 byEqpId, u8 byChnIdx, u8& byOutChnlIdx);
	BOOL32 IsNeedNewSelAdp(u8 bySrcId, u8 byDstId,u8 bySelMode,u8* pbyEqpId = NULL, u8* pbyOutChnlIdx = NULL);
public:
    TMt  GetVidBrdSrc(void);

protected:
    void AdjustKedaMcuAndTaideMuteInfo( TMt * ptMt, BOOL32 bDecodeAud, BOOL32 bOpenFlag );    
    void AdjustKedaMcuAndTaideAudioSwitch( u8 byDstMtId, BOOL32 bDecodeAud, BOOL32 bOpenFlag );

    u8 	GetSpeakerSrcSpecType(u8 byMode);
    void SetSpeakerSrcSpecType(u8 byMode, u8 bySpecType);
    void ProcPolycomSecVidChnnl(CMessage * const pcMsg);

    //立案系统特殊消息处理函数
    void ProcBuildCaseSpecialMessage(const CMessage * pcMsg);


    /************************************************************************/
    /*                                                                      */
    /*                       四、外设管理及操作                             */
    /*                                                                      */
    /************************************************************************/

    //1、未整理
    void ProcMixerMcuRsp( const CMessage * pcMsg );
    void ProcMixerMcuNotif( const CMessage * pcMsg );
    void ProcMixerRspWaitTimer( const CMessage * pcMsg );
    void ProcMcsMcuListAllRecordReq( const CMessage * pcMsg );
    void ProcRecMcuProgNotif( const CMessage * pcMsg );
    void ProcRecMcuListAllRecNotif( const CMessage * pcMsg );
    void ProcRecMcuPlayChnnlStatusNotif( const CMessage * pcMsg );	
    void ProcRecMcuRecChnnlStatusNotif( const CMessage * pcMsg );
    void ProcRecMcuResp(const CMessage * pcMsg);
	void ProcRecMcuNeedIFrameCmd(const CMessage * pcMsg);     //增加录像机关键帧请求  jlb  2008/12/18
    void ProcMcsMcuSeekReq( const CMessage * pcMsg );
    void ProcMcsMcuStopPlayReq( const CMessage * pcMsg );
    void ProcMcsMcuResumePlayReq( const CMessage * pcMsg );
    void ProcMcsMcuPausePlayReq( const CMessage * pcMsg );
    void ProcMcsMcuStartPlayReq( const CMessage * pcMsg );
    void StartPlayReq(CServMsg & cServMsg);
    void ProcMcsMcuStopRecReq( const CMessage * pcMsg );
    void ProcMcsMcuResumeRecReq( const CMessage * pcMsg );
    void ProcMcsMcuPauseRecReq( const CMessage * pcMsg );
    void ProcMcsMcuStartRecReq( const CMessage * pcMsg );
    void ProcMcsMcuChangeRecModeReq( const CMessage * pcMsg );
    void ProcMcsMcuDeleteRecordReq( const CMessage * pcMsg );
    void ProcMcsMcuRenameRecordReq( const CMessage * pcMsg );
    void ProcMcsMcuPublishRecReq( const CMessage * pcMsg );
    void ProcMcsMcuCancelPublishRecReq( const CMessage * pcMsg );
    void ProcMcuRecConnectedNotif(const CMessage * pcMsg);
    void ProcMcuRecDisconnectedNotif( const CMessage * pcMsg );
    void ProcMcsMcuGetRecPlayProgCmd( const CMessage * pcMsg );
	void ProcRecPlayWaitMpAckTimer( const CMessage * pcMsg);
    
    void ProcBasMcuRsp( const CMessage * pcMsg );
    void ProcMcuBasDisconnectedNotif( const CMessage * pcMsg );
    void ProcMcuBasConnectedNotif( const CMessage * pcMsg );
    void ProcBasMcuCommand(const CMessage * pcMsg);
    void ProcBasTestCommand( const CMessage * pcMsg );

    void ProcHdBasMcuRsp( const CMessage * pcMsg );
    void ProcHdBasMcuBrdAdpRsp( CServMsg &cServMsg );
    void ProcHdBasMcuSelRsp( CServMsg &cServMsg );
    void ProcHdBasChnnlStatusNotif( const CMessage * pcMsg );
	void ProcHduMcuChnnlStatusNotif( const CMessage * pcMsg );

    void ProcVmpMcuRsp( const CMessage * pcMsg ); 
    void ProcVmpMcuNotif( const CMessage * pcMsg ); 
    void ProcVmpRspWaitTimer( const CMessage * pcMsg );
	void ProcVMPStopSucRsp(void);
    
    void ProcVmpTwMcuRsp(const CMessage * pcMsg);
    void ProcVmpTwMcuNotif(const CMessage * pcMsg);
    
    void ProcPrsConnectedNotif( const CMessage * pcMsg );
    void ProcPrsDisconnectedNotif( const CMessage * pcMsg );
    void ProcPrsMcuRsp( const CMessage * pcMsg );
    void ProcMcStopSwitch( const CMessage * pcMsg );
    void ProcMcsMcuStopSwitchTWReq( const CMessage * pcMsg );
    void ProcMcsMcuStartSwitchTWReq( const CMessage * pcMsg );
    void StartSwitchTWReq(CServMsg & cServMsg);
    void ProcTvwallConnectedNotif(const CMessage * pcMsg);
    void ProcTvwallDisconnectedNotif( const CMessage * pcMsg );
    void ProcMcsMcuAddMixMemberCmd( const CMessage * pcMsg );
    void ProcMcsMcuRemoveMixMemberCmd( const CMessage * pcMsg );
    void ProcMcsMcuReplaceMixMemberCmd( const CMessage * pcMsg );
	void ProcMtMcuApplyMixNotify( const CMessage * pcMsg );
    void ProcMcsMcuStopSwitchHduReq( const CMessage * pcMsg );      // 停止hdu交换
    void ProcMcsMcuStartSwitchHduReq( const CMessage * pcMsg );     // 开始hdu交换
    void ProcMcsMcuChangeHduVolume( const CMessage * pcMsg );       // 设置hdu音量
	void ProcHduConnectedNotif(const CMessage * pcMsg);             // hdu连接通知
    void ProcHduDisconnectedNotif( const CMessage * pcMsg );        // 断链通知
	void ProcHduMcuStatusNotif( const CMessage * pcMsg );           // 状态通知
    void ProcHduMcuNeedIFrameCmd( const CMessage * pcMsg );         // hdu请求关键桢	

    
    //2、部分混音器管理
	BOOL32 StartMixing( u8 byMixMode );
	void StopMixing( void );
	void ProcMixStopSucRsp( void ); // 混音器停止成功后mcu对应的业务处理
	BOOL32 AddMixMember( const TMt *ptMt, u8 byVolume = DEFAULT_MIXER_VOLUME, BOOL32 bForce = FALSE );
	void RemoveMixMember( const TMt *ptMt, BOOL32 bForce = FALSE );
	void SetMemberVolume( const TMt *ptMt, u8 byVolume );
	BOOL32 NotifyMixerSendToMt( BOOL32 bSend = TRUE );
	void SetMixerSensitivity( u8 bySensVal );
    void SetMixDelayTime(u16 wDelayTime);
    void SwitchMixMember(const TMt *ptMt, BOOL32 bNMode = FALSE);
    void VACChange(const TMixParam &tMixParam, u8 byExciteChn);
    
	void AddRemoveSpecMixMember( TMt *ptMt = NULL, u8 byMtNum = 0, BOOL32 bAdd = TRUE, BOOL32 bStopMixNonMem = FALSE );
    void SwitchDiscuss2SpecMix();

    void MixerStatusChange(void); 
    u8   GetMixMtNumInGrp(void);
    u8   GetMaxMixNum(void);    //获取最大混音成员个数（目前：分散会议4个，其他56个）
    u8   GetMixChnPos( u8 byMtId, BOOL32 bRemove = FALSE ); //获取混音成员通道位置，用于区分交换端口号
    u8   GetMaxMixerDepth() const;

	//3、标清适配相关函数  
    BOOL32 StartAdapt( u8 byAdaptType, u16 wBitRate, TSimCapSet *pDstSCS = NULL, TSimCapSet *pSrcSCS = NULL );
    BOOL32 StartBasAdapt( TAdaptParam* ptParam , u8 byAdaptType, TSimCapSet *pSrcSCS );
    BOOL32 ChangeAdapt(u8 byAdaptType, u16 wBitRate, TSimCapSet *pDstSCS = NULL, TSimCapSet *pSrcSCS = NULL);
    BOOL32 ChangeBasAdaptParam(TAdaptParam *ptParam, u8 byAdaptType, TSimCapSet *pSrcSCS = NULL);
    void   ProcBasStartupTimeout( const CMessage * pcMsg );
    BOOL32 StopBasAdapt( u8 byAdaptType );
    BOOL32 StopBasSwitch( u8 byAdaptType );
    BOOL32 IsMtNeedAdapt(u8 byAdaptType, u8 byMtId, TMt *ptSrcMt = NULL);
    BOOL32 IsMtSrcBas(u8 byMtId, u8 byMode, u8 &byAdpType);
    BOOL32 IsMtSrcVmp2(u8 byMtId);
    BOOL32 IsNeedMdtpAdapt( TSimCapSet &tDstSCS, TSimCapSet &tSrcSCS );
	u16	   GetLstRcvGrpMtBr(BOOL32 bPrimary, u8 byMediaType, u8 byRes, u8 bySrcMtId = 0);
    u16    GetLeastMtReqBitrate(BOOL32 bPrimary = TRUE, u8 byMediaType = MEDIA_TYPE_NULL, u8 bySrcMtId = 0);
    u16    GetLeastMtReqBR(TSimCapSet &tSrcSCS, u8 bySrtMtId = 0, BOOL32 bPrimary = TRUE);
    u8     GetLeastMtReqFramerate(u8 byMediaType = MEDIA_TYPE_NULL, u8 bySrcMtId = 0);
    BOOL32 IsNeedAudAdapt(TSimCapSet &tDstSCS, TSimCapSet &tSrcSCS, TMt *ptSrcMt = NULL);
    BOOL32 IsNeedVidAdapt(TSimCapSet &tDstSCS, TSimCapSet &tSrcSCS, u16 &wAdaptBitRate, TMt *ptSrcMt = NULL);
    BOOL32 IsNeedBrAdapt(TSimCapSet &tDstSCS, TSimCapSet &tSrcSCS, u16 &wAdaptBitRate, TMt *ptSrcMt = NULL);
    BOOL32 IsNeedCifAdp( void );
    BOOL32 IsRecordSrcBas( u8 byType, TMt &tRecSrc, u8 byRecChnlIdx);
    
    u32	   GetBasLastVCUTick( u8 byBasChnnl );
	void   SetBasLastVCUTick( u8 byBasChnnl, u32 dwTick );

    //4 、高清适配相关函数

    //new
    BOOL32 StartHDMVBrdAdapt(void);
    BOOL32 StopHDMVBrdAdapt(void);

    BOOL32 StartHDDSBrdAdapt(void);
    BOOL32 StopHDDSBrdAdapt(void);

    BOOL32 ChangeHDAdapt(const TEqp &tEqp, u8 byChnId, BOOL32 bStart = FALSE);
    BOOL32 StopHDAdapt(const TEqp &tEqp, u8 byChnId);

    void   GetMVBrdSrcSim(TSimCapSet &tSim);
    void   GetDSBrdSrcSim(TDStreamCap &tDSim);
    void   GetSelSrcSim(const TEqp &tEqp, u8 byChnId, TSimCapSet &tSim);
    void   GetSelSrcSim(const TEqp &tEqp, u8 byChnId, TDStreamCap &tDSim);


    void   RefreshRcvGrp(u8 byMtId = 0);
    BOOL32 RefreshBasMVAdpParam(u8 byMtId, BOOL32 bSet = FALSE);
    BOOL32 RefreshBasDSAdpParam(u8 byMtId, BOOL32 bSet = FALSE);
    BOOL32 RefreshBasParam(u8 byMtId, BOOL32 bSet = FALSE, BOOL32 bDual = FALSE);
    BOOL32 RefreshBasParam4AllMt(BOOL32 bSet = FALSE, BOOL32 bDual = FALSE);
    BOOL32 RefreshBasParam4MVBrd( void );
    BOOL32 RefreshBasParam4DSBrd( void );

    BOOL32 RefreshBasMVSelAdpParam(const TEqp &tEqp, u8 byChnId);
    BOOL32 RefreshBasDSSelAdpParam(const TEqp &tEqp, u8 byChnId);

    BOOL32 IsDSSrcH263p( void );
    BOOL32 GetProximalGrp(u8 byMediaType, u8 byRes, u8 &byGrpType, u8 &byGrpRes);

    BOOL32 StartHDCascadeAdp( TMt *ptMt = NULL );
    BOOL32 StopHDCascaseAdp( void );

    BOOL32 StartHdVidSelAdp(TMt tSrc, TMt tDst, u8 byMode = MODE_VIDEO);
    BOOL32 StopHdVidSelAdp(TMt tSrc, TMt tDst, u8 byMode = MODE_VIDEO);
	void   ReleaseResbySel(TMt tSelDstMt, u8 bySelMode);

	BOOL32 StartAudSelAdp(TMt tSrc, TMt tDst);
	BOOL32 StopAudSelAdp(TMt tSrc, TMt tDst);


    //old
    BOOL32 IsRecordSrcHDBas( void );

	//5、丢包重传相关函数 
	void ChangePrsSrc( TMt tMt, u8 byPrsChanMode, BOOL32 bSrcBas = FALSE );
    BOOL ChangePrsSrc(u8 byPrsId, u8 byPrsChnlPos, const TMt& tEqp, u8 byEqpOutChnl = 0, u8 byEqpInChnl = 0);
	void AddRemovePrsMember( TMt tMt, BOOL32 bAdd, u8 byPrsChanMode  );
	void AddRemovePrsMember(u8 byMemId, u8 byPrsId, u8 byPrsChl, u8 byPrsMode = MODE_VIDEO, u8 bAdd = TRUE);
	void BuildRtcpAndRtpSwithForPrs(u8 byPrsId, u8 byPrsChl);
	void StopPrs( u8 byPrsChanMode, BOOL32 bNotify = FALSE );
	void StopPrs(u8 byPrsId, u8 byPrsChnl);
	void SwitchVideoRtcpToDst(u32 dwDstIp, u16 wDstPort, 
                              TMt tSrc, u8 bySrcChnnl = 0, 
		                      u8 byMode = MODE_VIDEO,
                              u8 bySwitchMode = SWITCH_MODE_BROADCAST, BOOL32 bMapSrcAddr = FALSE);
	BOOL32 AssignPrsChnnl4HdConf( );
	BOOL32 AssignPrsChnnl4SdConf( );
	BOOL32 AssignPrsChnnl(void);
	BOOL32 GetMtRealSrc(u8 byMtId, u8 byMode, TMt& tMtSrc, u8& byOutChnl);
	void   OccupyPrsChnl(u8 byPrsId, u8 byPrsChnl);
	void   RlsPrsChnl(u8 byPrsId, u8 byPrsChnl);

    //6、VMP相关函数 
	BOOL32 CheckVmpParam(TVMPParam& tVmpParam, u16& wErrorCode);
	BOOL32 CheckMpuMember(TVMPParam& tVmpParam, u16& wErrorCode, BOOL32 bChgParam = FALSE);
	void CheckMpuAudPollConflct(TVMPParam& tVmpParam, BOOL32 bChgParam = FALSE);
	void CheckSameMtInMultChnnl(TVMPParam & tVmpParam);
	void CheckAdpChnnlLmt(u8 byVmpId, TVMPParam& tVmpParam, CServMsg &cServMsg, BOOL32 bChgParam = FALSE);
	void CheckAdpChnnlAtStart(TVMPParam &tVmpParam, u8 byMaxHdChnnlNum, CServMsg &cServMsg);
	void CheckAdpChnnlAtChange(TVMPParam &tVmpParam, u8 byMaxHdChnnlNum, CServMsg &cServMsg);
    void ChangeVmpStyle(TMt tMt, BOOL32 bAddMt, BOOL32 bChangeVmpParam = TRUE);
    void ChangeVmpSwitch( u8 bySrcType = 0, u8 byState = VMP_STATE_START );
	void CancelOneVmpFollowSwitch( u8 byMemberType, BOOL32 bNotify );
    void ChangeVmpChannelParam(TMt * ptNewSrc, u8 byMemberChnl, TMt * ptOldSrc = NULL);
    void ChangeVmpParam( TVMPParam *ptVMPParam = NULL, BOOL32 bStart = FALSE );
    void ChangeVmpBitRate(u16 wBitRate, u8 byVmpChanNo = 1); 
    u8   AdjustVmpBrdBitRate( TMt *ptMt = NULL);
	u8   GetVmpChnnl( void );
	void SetVmpChnnl( TMt tMt, u8 byChnnl, u8 byMemberType, BOOL32 bMsgMcs = FALSE );
	BOOL32 IsDynamicVmp( void );
	u8   GetVmpDynStyle( u8 byMtNum );
    BOOL32 IsChangeVmpParam(TMt * ptMt);
    BOOL32 IsVMPSupportVmpStyle(u8 byStyle, u8 byEqpId, u16 &wError);
    BOOL32 IsMCUSupportVmpStyle(u8 byStyle, u8 &byEqpId, u8 byEqpType, u16 &wError );
    

	void	AdjustVmpParam(TVMPParam *ptVMPParam, BOOL32 bStart = FALSE);
	BOOL32  ChangeMtVideoFormat( TMt   tMt, TVMPParam * ptVMPParam, BOOL32 bStart = TRUE, BOOL32 bSeize = TRUE, BOOL32 bConsiderVmpBrd = TRUE );  //支持级联的画面合成分辨率调整
	void	ChgMtFormatInSdVmp( u8 byMtId, TVMPParam * ptVMPParam, BOOL32 bStart = TRUE );	//针对标清VMP的调整分辨率
	BOOL32  ChgMtFormatInMpu( u8 byMtId,	TVMPParam *ptVmpParam,	BOOL32 bStart = TRUE, BOOL32 bSeize = TRUE, BOOL32 bConsiderVmpBrd = TRUE );
	BOOL32	RecoverMtResInMpu(TMt tMt);
	void	AddToVmpNeglected( u8 byMtId);
	BOOL32	IsMtNeglectedbyVmp( u8 byMtId);
	void	ChangeNewVmpParam(TVMPParam *ptVMPParam, BOOL32 bStart = FALSE);
	void	ProcMcsMcuStartSwitchVmpMtReq(const CMessage * pcMsg);
	void	ChairmanSeeVmp(TMt tDstMt, CServMsg &cServMsg );
	void	AdjustVmpParambyMemberType(TMt *ptSpeaker,TVMPParam &tVmpParam, u8 byMembertype = VMP_MEMBERTYPE_SPEAKER,TMt *ptOldSrc = NULL);
    void	UpdataVmpEncryptParam(TMt tMt, u8 byChnlNo);
	BOOL32  VidResAdjust(u8 byVmpStyle, u8 byPos, u8 byMtStandardFormat, u8 &byReqRes);
    BOOL32  IsDelayVidBrdVCU(void);	

    //7、vmp tvwall相关函数 
    void ChangeVmpTwChannelParam(TMt * ptNewSrc, u8 byMemberType);
    void ChangeVmpTwParam(TVMPParam *ptVmpParam = NULL, BOOL32 bStart = FALSE);
    void ChangeVmpTwSwitch(u8 bySrcType = 0, u8 byState = VMP_STATE_START);
    void SetVmpTwChnnl(TMt tMt, u8 byChnnl, u8 byMemberType, BOOL32 bMsgMcs = FALSE);

    //8、tvall相关函数 
    void ChangeTvWallSwitch(TMt * ptSrc, u8 byTvWallId, u8 byChnlIdx,u8 byTWMemberType, u8 byState, BOOL32 bEqpStop = TRUE);
	BOOL32 IsMtNotInOtherTvWallChnnl(u8 byMtId, u8 byTvWallId, u8 byChnlId);
    
    //9、recorder相关函数 
	void StopAllMtRecoderToPeriEqp();
    void AdjustRecordSrcStream( u8 byMode );   
	
	//10、Hdu相关函数
	void ChangeHduSwitch(TMt * ptSrc, u8 byHduId, u8 byChnlIdx,u8 byHduMemberType, u8 byState, u8 byMode = MODE_BOTH, BOOL32 bBatchPoll = FALSE);
    BOOL32 IsMtNotInOtherHduChnnl(u8 byMtId, u8 byHduId, u8 byChnlId);
	BOOL32 HduBatchPollOfOneCycle( BOOL32 bFirstCycle );            // hdu一个周期的批量轮询
	void   StopHduBatchPollSwitch( BOOL32 bStopPlay = FALSE );

    /************************************************************************/
    /*                                                                      */
    /*                        五、mp/mtadp管理                              */
    /*                                                                      */
    /************************************************************************/
    void ProcMpMessage( CMessage * const pcMsg );
    void ProcMpDissconnected( CMessage * const pcMsg );
	void ProcMtAdpDissconnected( CMessage * const pcMsg );	


    /************************************************************************/
    /*                                                                      */
    /*                       六、交换管理及操作                             */
    /*                                                                      */
    /************************************************************************/
	
    //1、终端交换
	BOOL32 StartSwitchToSubMt(TMt tSrc, 
                              u8 bySrcChnnl,
                              u8 byDstMtId,
                              u8 byMode = MODE_BOTH,
                              u8 bySwitchMode = SWITCH_MODE_BROADCAST,
                              BOOL32 bMsgStatus = TRUE,
                              BOOL32 bSndFlowCtrl = TRUE,
                              BOOL32 bIsSrcBrd = FALSE );

	void   StopSwitchToSubMt( u8 byDstMtId,
                              u8 byMode = MODE_BOTH,
                              u8 bySwitchMode = SWITCH_MODE_BROADCAST,
                              BOOL32 bMsgStatus = TRUE,
                              BOOL32 bStopSelByMcs = TRUE,
                              BOOL32 bSelSelf = TRUE );	

	BOOL32 StartSwitchToMcu( TMt tSrc,
                             u8 bySrcChnnl,
                             u8 byDstMtId,
                             u8 byMode = MODE_BOTH,
                             u8 bySwitchMode = SWITCH_MODE_BROADCAST,
                             BOOL32 bMsgStatus = TRUE );

	BOOL32 RestoreAllSubMtJoinedConfWatchingSrcMt(TMt tSrc, u8 byMode = MODE_BOTH );
	
    void   StopSwitchToAllSubMtJoinedConfNeedAdapt( BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH, u8 byAdaptType = ADAPT_TYPE_VID );
	BOOL32 StartSwitchToAllSubMtJoinedConfNeedAdapt( TMt tSrc, u8 bySrcChnnl, u8 byMode = MODE_BOTH, u8 byAdaptType = ADAPT_TYPE_VID );
    
    BOOL32 StartSwitchToSubMtNeedAdp(const TEqp &tEqp, u8 byChnId);
    BOOL32 StopSwitchToSubMtNeedAdp(const TEqp &tEqp, u8 byChnId);
    
    BOOL32 StartSwitchToSubMtFromAdp(u8 byMtId, BOOL32 bDual = FALSE);
    BOOL32 StopSwitchToSubMtFromAdp(u8 byMtId, BOOL32 bDual = FALSE);

    BOOL32 StartHdBasSwitch(const TEqp &tEqp, u8 byChnId);
    BOOL32 StopHdBasSwitch(const TEqp &tEqp, u8 byChnId);

    BOOL32 StartAllHdBasSwitch(BOOL32 bDual = FALSE);
    BOOL32 StopAllHdBasSwitch(BOOL32 bDual = FALSE);

    BOOL32 CheckHdBasChnMode(u8 byMode, u8 &byMediaMode, u8 &bySwitchMode);

    // 唇音同步
    BOOL32 StartSwitchAud2MtNeedAdp(u8 byDstMtId, u8 bySwitchMode = SWITCH_MODE_BROADCAST);

    // guzh [3/20/2007]终端广播交换相关
    BOOL32 StartSwitchFromBrd(const TMt &tSrc, u8 bySrcChnnl, u8 byDstMtNum, const TMt* const ptDst);
    BOOL32 StartSwitchToAllSubMtJoinedConf( const TMt &tSrc, u8 bySrcChnnl );     // 音频
    BOOL32 StartSwitchToAllSubMtJoinedConf( const TMt &tSrc, u8 bySrcChnnl, const TMt &tOldSrc, BOOL32 bForce ); // 视频
    void   RestoreRcvMediaBrdSrc( u8 byMtId, u8 byMode = MODE_BOTH, BOOL32 bMsgMtStutas = TRUE );
    void   SwitchSrcToDst( const TSwitchInfo & tSwitchInfo, const CServMsg & cServMsgHdr, BOOL32 bBatchPollChg = FALSE );
    BOOL32 SwitchSrcToDstAccord2Adp( const TSwitchInfo & tSwitchInfo, const CServMsg & cServMsgHdr );
    void   StopSwitchToAllSubMtJoinedConf( u8 byMode = MODE_BOTH );
    void   ProcVidBrdSrcChanged( CMessage * const pcMsg );
    BOOL32 CanMtRecvVideo(const TMt &tDst, const TMt &tSrc);

	//2、外设交换
	BOOL32 StartSwitchToPeriEqp(TMt tSrc,
                                u8  bySrcChnnl,
                                u8  byEqpId,
                                u16 wDstChnnl           = 0,
                                u8  byMode              = MODE_BOTH,
                                u8  bySwitchMode        = SWITCH_MODE_BROADCAST,
                                BOOL32 bDstHDBas        = FALSE,
                                BOOL32 bStopBeforeStart = TRUE,
                                BOOL32 bBatchPollChg    = FALSE);

	void StopSwitchToPeriEqp( u8  byEqpId,
							  u16 wDstChnnl		  = 0,
							  BOOL32 bMsg		  = TRUE,
							  u8     byMode		  = MODE_BOTH,
							  u8     bySwitchMode = SWITCH_MODE_BROADCAST); 

	void StopSwitchToAllPeriEqpWatchingSrcMt( TMt tSrc, BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH );
	void StopSwitchToAllPeriEqpWatchingSrcMcu(  u16 wMcuId, BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH );
	void StopSwitchToAllPeriEqpInConf( BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH );
    BOOL32 StartSwitchToRecNeedHDVidAdapt( void );

	//3、会控交换
	BOOL32 StartSwitchToMc( TMt tSrc, u8 bySrcChnnl, u16 wMcInstId, u8 byDstChnnl = 0, u8 byMode = MODE_BOTH );
	void StopSwitchToMc( u16 wMcInstId,  u8 byDstChnnl = 0, BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH, BOOL32 bFilterConn = TRUE );
	void StopSwitchToAllMcInConf( BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH );
	void StopSwitchToAllMcWatchingSrcMt( TMt tSrc, BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH );
	void StopSwitchToAllMcWatchingSrcMcu(  u16 wMcuId, BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH );
    BOOL32 IsMtInMcSrc(u8 byMtId, u8 byMode);

    //4、桥交换
    BOOL32 AdjustSwitchBridge(u8 byMpId, BOOL32 bInsert);


    /************************************************************************/
    /*                                                                      */
    /*                          七、实例一般函数                            */
    /*                                                                      */
    /************************************************************************/
private:
    void ClearVcInst( void );
    u8   AssignIdleConfInsID(u8 byStartInsId = 1);
    
	void InstanceDump( u32 param = 0 );
	void InstanceEntry( CMessage * const pcMsg );
	void DaemonInstanceEntry( CMessage * const pMsg, CApp * pcApp );
	void DaemonInstanceDump( u32 param = 0 );

    /************************************************************************/
    /*                                                                      */
    /*                          八、卫星会议操作函数                        */
    /*                                                                      */
    /************************************************************************/
private:

    //1、会议开关，卫星频率控制

    void DaemonProcSendMsgToNms(const CMessage* pcMsg);
    void ProcGetFreqFromNMS(const CMessage * pcMsg);
    void ApplySatFrequence();
    void ReleaseSatFrequence();
    u32  GetSerialId();
    void ReleaseConfFreq();

    //2、网管消息处理及控制
    u32  SendCmdToNms( u16 wCmd , u8 byBrdNum, u32 dwBrdBit, u16 wNum, u32 dwBitRate, u32 dwFreq[5], u32 dwBrdFreq );
    void ProcApplyFreqTimeOut( const CMessage * pcMsg );
    void ProcNmsMcuReshedconfNotif(  const CMessage*pcMsg );
    void ProcNmsMcuSchedConf( const CMessage*pcMsg );
    void ProcDelayTimeOut( const CMessage* pcMsg );	
    void ProcNmsAddMt(const CMessage* pcMsg );
    void ProcNmsDelMt(const CMessage* pcMsg );
    void ProcNmsAddMtOut( const CMessage* pcMsg );
    void ProcNmsDelMtOut( const CMessage* pcMsg );
    void ProcSchedStartNow( const CMessage* pcMsg );

    //3、MODEM控制
    void ProcModemConfRegRsp( const CMessage * pcMsg );
    void ProcModemMcuStatusNotif( const CMessage * pcMsg );
    BOOL StartConfModem(u8 byNum);
    BOOL StopConfModem( u8 byNum );
    void DaemonProcModemReg(const CMessage* pcMsg);

    //4、会议和终端控制
    void ProcCreateConfAccordNms( const CMessage * pcMsg );
    void RefreshConfState( void );
	void RefreshConfState( BOOL32 bHdGrp );
	void RefreshMtBitRate( void );
	BOOL RefreshConfMtKey( void );
	void DaemonProcTimerMultiCast( const CMessage * pcMsg, CApp * pcApp );
    
    //5、其他卫星会议控制函数
    void ProcSchedConfTimeOut( const CMessage * pcMsg );
	u8   GetMtSrc( u8 byMtId, u8 byMode );

    /************************************************************************/
    /*                                                                      */
    /*                          九、打印相关                                */
    /*                                                                      */
    /************************************************************************/
protected:
	void ShowConfMt( u8 byMtId );
	void ShowConfSwitch( void );
	void ShowConfEqp( void );
	void ShowMtStat( void );
	void ShowConfMod( void );
	void ShowMtMonitor( void );
	void ShowBasInfo( u8 byBasId = 0 );
	void ConfLog( BOOL32 bFile, s8 * fmt, ... );    
	void ShowVmpAdaptMember( void );
};

typedef zTemplate< CMcuVcInst, MAXNUM_MCU_CONF, CMcuVcData, sizeof( CConfId ) > CMcuVcApp;	//Daemon实例用于处理非会议命令

extern  CMcuVcApp	        g_cMcuVcApp;	//MCU业务应用实例
extern  CAgentInterface     g_cMcuAgent;    
extern  CMSManagerSsnApp    g_cMSSsnApp;
extern  CMpManager          g_cMpManager;   //MP管理类全局变量

#define STATECHECK \
if( CurState() != STATE_ONGOING )\
{\
	ConfLog( FALSE, "Wrong message %u(%s) received in state %u at Line.%u!\n", \
		pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), __LINE__ );\
	return;\
}

#define ISGROUPMODE(byMode) \
((byMode >= VCS_GROUPSPEAK_MODE && byMode <= VCS_GROUPVMP_MODE) ? TRUE : FALSE)


#endif

//END OF FILE

