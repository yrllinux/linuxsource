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
#include "kdvlog.h"
#include "loguserdef.h"
#include "tvwall.h"
#include "evmcumcs.h"
#include "h460pinhole.h"

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
const u32 TIMESPACE_WAIT_CASCADE_CHECK  = 20 * 1000;    //等待级联通道打开(ms)
const u32 TIMESPACE_UPADATE_REGSTATUS   = 2 * 60 * 1000;//刷新辅接入板的实体时间间隔
const u32 TIMESPACE_WAIT_FSTVIDCHL_SUC	= 1 * 1000;		//等待第一路逻辑通道打开完成，再尝试第二路
const u32 TIMESPACE_WAIT_PLAY_SWITCHOK  = 500;          //等待放象交换建立完成后开始放象(ms)
const u32 TIMESPACE_WAIT_ALLVMPPRESETIN_ACK = 1500;//等待所有本次画面合成presetinAck的时间间隔
const u32 TIMESPACE_WAIT_VMPRSP =	8*1000;	//等待所有本次画面合成操作回响应超时间隔

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
const u16 DUMPMULTISPY		= 15;
const u16 DUMPCONFMIX		= 16;
const u16 DUMPSPEAKLIST		= 17;
const u16 DUMPPRTSPEAKLIST  = 18;
const u16 DUMPNPRTSPEAKLIST	= 19;
const u16 DUMPBASLIST		= 20;
const u16 DUMPCONFMCU		= 21;
const u16 DUMPTWINFO	    = 22;
const u16 DUMPPRSINFO	    = 23;
const u16 DUMPSMSTIMER		= 25;
const u16 DUMPTOKENOWNER    = 26;
// [12/7/2010 liuxu]添加按照功能打印
const u16 DUMPFUNCINFO		= 24;
const u16 DUMPMCOTHERTABLE	= 27;
const u16 DUMPCFGEXTRADATA	= 28;


#define IS_PTR_NULL(ptr)        ((ptr == NULL)? TRUE:FALSE)
#define MCU_SAFE_DELETE(ptr)	{if(ptr){delete ptr;ptr=NULL;}}

// 消息缓冲池大小(liuxu)
#define		MAX_MT_NUM_IN_BUF						(u32)100					
#define		MAX_MCU_MTLIST_BUF						(u32)( 1 + MAXNUM_CONFSUB_MCU)
// 消息缓冲时间(liuxu)
#define		TIMER_SPAN_SEND_MTSTATUS_MMCU			(s32)(1000)
#define		TIMER_SPAN_SEND_ALLMTLIST_ALLMCS		(s32)(1000)
#define		TIMER_SPAN_SEND_MTSTATUS_MCS			(s32)(1000)

// 单个会议的全部数据, len: 11K
class CMcuSingleConfAllInfo
{
public:
	enum _emSatReplaceType  //卫星会议卫星终端业务类型添加
	{
		emDefault = 0,            //默认
		emSpeaker,                //发言人
		emChairMan,               //主席
		emStartVmp,               //开启画面合成
		emVmpChnChange,           //画面合成通道替换
		emStartPoll,	          //开启轮训
		emStartOrChangeHdu,       //HDU相关，替换,单通道轮训
		emHduBatch,               //HDU批量轮训
		emSelect,                 //选看
		emStartOrChangeMonitor,   //监控
		emstartDs,                //双流
		emStartRecord,            //录像

		emSpeakerSrc,
		emChairSel, //主席选看
		emStopChairSel, //停止主席选看
		emPoll,
		emVmpBrd,	//Vmp广播，和下面这个区别是否考虑发言人
		emVmpNoBrd, //Vmp非广播
		emVmpStop,	//停止VMP，不用去判断当前VMP成员
		emTvWall,	//TvWall
	};
	typedef enum _emSatReplaceType emSatReplaceType;

    TConfMtTable      *m_ptMtTable;              //会议终端表      
    TConfSwitchTable  *m_ptSwitchTable;          //会议交换表
    
    TConfInfo	     m_tConf;	                 //会议完整信息
	TConfInfoEx		 m_tConfEx;					 //会议扩展信息，V4R7开始支持，存放能力集勾选[12/26/2011 chendaiwei]
	
	TConfAllMcuInfo	 m_tConfAllMcuInfo;			 //会议所有mcu信息
	TConfAllMtInfo   m_tConfAllMtInfo;           //会议所有终端信息    
    TConfInnerStatus m_tConfInStatus;            //会议内部状态标识
    TConfProtectInfo m_tConfProtectInfo;         //会议保护信息
    TConfPollParam   m_tConfPollParam;           //会议轮询参数	
    //TTvWallPollParam m_tTvWallPollParam;       //电视墙轮询参数
	//THduPollParam    m_tHduPollParam;          //hdu轮询参数
	//[03/04/2010] zjl add (电视墙多通道轮询代码合并)
    TTWMutiPollParam m_tTWMutiPollParam;         //电视墙多通道轮询信息(包含tvwall和hdu)
    TConfEqpModule   m_tConfEqpModule;           //会议设备模板

    u8               m_byConfIdx;                //会议idx
    TMt              m_tCascadeMMCU; 		     //级联的上级MCU
    u8               m_byCreateBy;               //记录会议的创建者
private:    
    TMt     m_tVidBrdSrc;                   //视频广播源
    TMt     m_tAudBrdSrc;                   //音频广播源
protected:
    TConfOtherMcTable *m_ptConfOtherMcTable;     //其它Mc终端信息
	
	//u8  	m_abyMtCountPoint[MAXNUM_CONF_MT/8];	//终端在接入侧算的点数记录
public:
    TMt     m_tDoubleStreamSrc;             //双流源 这里是为了给MpManager用，建议用public的接口代替！
protected:
	TMt		m_tSecVidBrdSrc;				//第二视频广播源
	u8		m_byNewTokenReqMtid;			//当双流源为KEDAMCU时，又有终端申请发起双流，
											//暂时记录一下该终端，等老的KEDAMCU双流源回了释放令牌通知后发起双流
    TH239TokenInfo m_tH239TokenOwnerInfo;   //H239双流令牌拥有终端信息
    
    TEqp    m_tVmpEqp;                      //画面合成外设
    TEqp    m_tVmpTwEqp;                    //复合电视墙外设
    TEqp	m_tMixEqp;                      //混音外设
    TEqp	m_tRecEqp;                      //录像外设
    TEqp	m_tPlayEqp;                     //放像外设
    TPlayEqpAttrib m_tPlayEqpAttrib;        //放像外设属性
    TPlayFileMediaInfo m_tPlayFileMediaInfo;    //放像文件媒体信息
    TRecStartPara m_tRecPara;               //录像参数
	TPlayFileHpAttrib m_tPlayFileHpAttrib;    //放像文件的视频流HP/BP信息
    
//     TEqp	m_tAudBasEqp;					//音频适配外设
//     TEqp	m_tVidBasEqp;					//视频适配外设
//     TEqp	m_tBrBasEqp;					//码率适配外设
//     TEqp    m_tCasdAudBasEqp;               //级联音频回传适配外设
//     TEqp    m_tCasdVidBasEqp;               //级联视频回传适配外设
//    TEqp    m_tPrsEqp;                      //包重传外设
    
    u8      m_byMixGrpId;					//混音组ID
    u8		m_byRecChnnl;					//录像信道号
    u8		m_byPlayChnnl;					//放像信道号
//     u8      m_byPrsChnnl;                   //包重传第一视频通道
//     u8      m_byPrsChnnl2;                  //包重传第二视频通道
//     u8      m_byPrsChnnlAud;                //包重传音频通道
//     u8      m_byPrsChnnlAudBas;             //音频适配包重传通道
//     u8      m_byPrsChnnlVidBas;             //视频适配包重传通道
//     u8      m_byPrsChnnlBrBas;              //码率适配包重传通道

// 	u8		m_byPrsChnnlVmpOut1;			//广播源1080出包重传通道
// 	u8		m_byPrsChnnlVmpOut2;				//广播源720出包重传通道
// 	u8		m_byPrsChnnlVmpOut3;			//广播源4Cif出包重传通道
// 	u8		m_byPrsChnnlVmpOut4;		//广播源Cif或other出包重传通道
// 	u8		m_byPrsChnnlDsVidBas;			//双流适配包重传通道	
	
//     u8      m_byAudBasChnnl;				//音频适配通道号
//     u8      m_byVidBasChnnl;				//视频适配通道号
//     u8      m_byBrBasChnnl;					//码率适配通道号
//     u8      m_byCasdAudBasChnnl;            //级联音频回传适配通道号
//     u8      m_byCasdVidBasChnnl;            //级联视频回传适配通道号
    u16     m_wVidBasBitrate;				//视频适配的码率    
    u16     m_wBasBitrate;					//码率适配的码率
    //u16     m_awVMPBrdBitrate[MAXNUM_MPU2_OUTCHNNL];               //单速会议VMP的广播码率
#ifdef _MINIMCU_
    u8      m_byIsDoubleMediaConf;          //是否是双媒体会议(主码率采用主格式，辅码率采用辅格式
#endif

	u32		m_dwVmpLastVCUTick;				//上一次向VMP发送请求关键帧命令
	/*u32		m_dwVmpTwLastVCUTick;			//上一次向VMPTw发送请求关键帧命令
	u32		m_dwVidBasChnnlLastVCUTick;		//上一次向BAS（不同通道）发送请求关键帧命令
	u32		m_dwAudBasChnnlLastVCUTick;
	u32		m_dwBrBasChnnlLastVCUTick;
	u32		m_dwCasdAudBasChnnlLastVCUTick;
	u32		m_dwCasdVidBasChnnlLastVCUTick;*/
    
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
    //TVMPParam	m_tLastVmpParam;			//上一次画面合成成员，用于Vmp动态分辨率恢复
    TVMPParam	m_tLastVmpTwParam;          //上一次VMPTW成员，用于Vmp动态分辨率恢复 

	u8      m_byLastMixMode;                //混音器断链前的会议混音模式

    TAcctSessionId m_tChargeSsnId;          //本会议计费SessionId

    TConfPollSwitchParam m_tPollSwitchParam;           //Radvision方式轮询回传参数
    
	//tianzhiyong 2010/03/21  原来APU下最多56个混音成员，EAPU下改为64个
    u8      m_abyMixMtId[MAXNUM_MIXER_CHNNL + 1];      //用于记录参与混音的终端的MtId 

//    u8      m_abyCasChnCheckTimes[MAXNUM_CONF_MT];     //打开级联通道的尝试次数    

    u8      m_abySerHdr[SERV_MSGHEAD_LEN];  //记录的创建会议时的消息头
    TMt     m_tRollCaller;                  //点名人
    TMt     m_tRollCallee;                  //被点名人

	u8	    m_bySpeakerVidSrcSpecType;		//发言人源是否是由会议自动指定
	u8		m_bySpeakerAudSrcSpecType;
    
    //CRcvGrp m_cMtRcvGrp;                    //适配会议终端接收群组管理
    //CBasMgr *m_pcBasMgr;                    //适配会议BAS通道和状态管理
    //CMcuBasMgr *m_pcMcuBasMgr;				//适配会议BAS通道和状态管理
	u8		m_byMtIdNotInvite;				// xliang [12/26/2008] Mt 主动呼MCU，
											//模板创会或起缺省会议情况下，
											//标记该MT预先不是受邀终端

	//****************  VMP related  ****************
public:

protected:

	//--后适配信息列表
	//TKDVVMPOutParam m_tVMPOutParam;
	//--seize related:
	//TVmpAdaptChnSeizeOpr	m_tVmpAdaptChnSeizeOpr;			//记录抢占后续所要的操作
	//TVmpChnnlInfo m_tVmpChnnlInfo;							// vmp通道信息，主要记录VMP前适配成员
	//TVMPMember	m_atVMPTmpMember[MAXNUM_MPUSVMP_MEMBER];	//抢占做记录用
	//BOOL32	m_bNewMemberSeize;								//表征此次抢占是新增了一个成员触发的抢占还是某成员升级为VIP身份触发的抢占
	u8		m_abyMtNeglectedByVmp[MAXNUM_CONF_MT/8];		//标记自动vmp时，不应参与的MT// xliang [12/17/2008] 目前不用, 仅预留
	u8 m_abyVmpEqpId[MAXNUM_CONF_VMP];	//会议占用vmp合成器索引，会议最大占用16个
	TEqp m_tModuleVmpEqp;
	
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

	CVCCStatus  m_cVCSConfStatus;            // VCS会议的独特的会议状态
	TMtListInfo m_tOrginMtList;              // VCS最初创会会议列表

    //TPrsInfoManage m_tConfPrsInfo;        // 会议使用的PRS详细信息
	
	BOOL32	m_bMMcuSpeakerValid;			// xliang [10/10/2009] 解决级联场景下，上级发双流，挂断下级再呼入，
											// 下级是否能被打开双流通道的判断通不过的时序问题

	//zjj20091102 数据模板额外信息管理(管理那些不在TConfStore结构中，需要动态增长的数据)
	//现在只是针对vcs会议中的电视墙预案
	CConfInfoFileExtraDataMgr m_cCfgInfoRileExtraDataMgr;

	TApplySpeakQue m_tApplySpeakQue;        // 会议申请发言队列

	//lukunpeng 2010/06/10 更改回传管理类的变量名及注释，以便更清楚
	//主要记录本地回传到上级的回传成员
	CConfSendSpyInfo		m_cLocalSpyMana;		// 用于记录本级会议回传到上级mcu的回传成员及占用德带宽信息
	//主要记录下级回传上来的下级终端信息
	CConfRecvSpyInfo		m_cSMcuSpyMana;			// 用于记录下级会议回传的下级成员及占用的带宽信息
	TMt     m_atMixMt[MAXNUM_MIXER_CHNNL+1];         //用于记录参与混音的终端的完整tmt信息（未被local化）
//	CMcChnMgr               m_cMcChnnlMgr;      // 记录会控监控通道信息

	u8 m_byWaitStartVmp;

	CRecAdaptMgr m_cRecAdaptMgr;		//适配录像管理
	
	u8      m_byOtherConfMtId;

	// [1/7/2011 xliang] add smscontrol
	CSmsControl m_cSmsControl;					//发送短消息控制
	
	// [9/27/2011 liuxu] 待发送终端状态的终端缓冲池
	CStaticBuf<TMt, MAX_MT_NUM_IN_BUF>		m_cMtStatusSendBuf;
	
	// [11/14/2011 liuxu] 缓冲发送AllMtInfoToAllMcs
	u8		m_byAllMtToAllMcsTimerFlag;
	CStaticBuf<u16, MAX_MCU_MTLIST_BUF>		m_cMcuMtListSendBuf;

	u8		m_byMtStatusToMMcuTimerFlag;
	CStaticBuf<u8,	MAXNUM_CONF_MT>			m_cLocalMtStatusToMMcuBuf;
	CStaticBuf<u16, MAXNUM_CONFSUB_MCU>		m_cSMcuMtStatusToMMcuBuf;

	u32 m_dwBrdFastUpdateTick;

	u8 m_byFastNeglectNum;

	u32 m_dwRealSndSpyBandWidth;

	//最低一位标示是否在等待主板发会议数据，该数据不发给备板（主板该状态应该为FALSE，备板收到instancestate后，非IDLE会议设置为TRUE，会议数据收完，置为FALSE）
	//最低第二位标示简单级联会议时是否已经有其它mcu会议呼入本会吗，呼入本会的会议在本会议中的mtid用变量m_byOtherConfMtId标示
	u8          m_byConfFlag;
	s16         m_swCurSatMtNum;       //当前剩余的可用卫星点数

private:
	CMcuBasMgr     *m_pcMcuBasMgr;				//适配会议BAS通道和状态管理
	TPrsInfoManage m_tConfPrsInfo;              // 会议使用的PRS详细信息

public:

	 u16   GetDoubleStreamVideoBitrate(u16 wDialBitrate, BOOL32 bFst = TRUE); //获取双流视频码率，默认第一路
	/*---------------------------bas管理类相关接口------------------------*/
	//开辟bas管理空间
	BOOL32 NewMcuBasMgr();
	//销毁bas管理空间
	void   DeleteMcuBasMgr();
	//获取bas管理数据区
	u32    GetBasMgrBuf(u8 *pbyBuf);
	//设置bas管理数据区
	u32    SetBasMgrBuf(u8 *pbyBuf);
	//清空bas管理数据区
	void   ClearBasMgr();

	//为广播查找可用通道
	BOOL32	FindBasChnByCapForVidBrd(TVideoStreamCap &tSimCapSet, u8 byMediaMode, TBasOutInfo &tOutInfo, BOOL32 bIsExactMatch = FALSE);
	BOOL32	FindBasChnForAudBrd(TAudioTypeDesc &tAudCap,TBasOutInfo &tOutInfo);
	//为选看查找可用通道
	BOOL32	FindBasChnByVidCapForSel(TMt &tSrc,TVideoStreamCap &tDstSimCapSet, u8 byMediaMode, TBasOutInfo &tOutInfo);
	BOOL32	FindBasChnByAudCapForSel(TMt &tSrc,TAudioTypeDesc &tDstAudCap, TBasOutInfo &tOutInfo);

	//获取广播源
	TMt	   GetBrdSrc(u8 byMediaMode);
	//1.广播部分
	//广播适配： 适配资源加入适配组
	BOOL32 AssignBasBrdChn(CBasChn **aptBasChn, TBasChnCapData *atBasChnData, u8 byChnNum,const CConfId &cConId);	
	//广播适配： 适配通道从广播组移除
	BOOL32 ReMoveBasBrdChn(const TEqp &tEqp, u8 byChnId);
	//广播适配：获得广播通道
	BOOL32 GetBasBrdChnGrp(u8 &byNum, CBasChn **pcBasChn, u8 byMediaMode);	
	//广播适配： 获取所有广播组通道
	BOOL32 GetBasAllBrdChn(u8 &byNum, CBasChn **pcBasChn);	
	//广播适配： 获取广播适配占用的不同适配器信息
	BOOL32 GetDiffBrdEqp(u8 byMediaMode, u8 &byEqpNum, TEqp *ptBas);
	//广播适配：刷新广播特殊参数，针对双other的单速会议或者音频适配
	BOOL32	RefreshSpecialCapForVidBrd(TConfInfo &tConfInfo,TVideoStreamCap &tSimCapSet, u8 byMediaMode);	
	BOOL32  RefreshSpecialCapForAudBrd(TConfInfo &tConfInfo,TConfInfoEx &tConfInfoEx, TAudioTypeDesc &tSrcAudCap);
	//广播适配：刷新广播参数
	BOOL32	RefreshBasBrdParam(u8 byMediaMode,u16 wBiteRate);

	//2.选看部分
	//选看适配：适配资源加入选看组
	BOOL32 AssignBasSelChn(CBasChn *pcBasChn,const TBasChnCapData &tBasChnIdxParam, const CConfId &cConId);
	//选看适配：适配资源从选看组删除
	BOOL32 ReMoveBasSelChn(const TEqp &tEqp, u8 byChnId);
	//选看适配：添加参数
	BOOL32 AddBasSelChnParam(const TEqp &tEqp, u8 byChnId, const  TBasChnCapData &tBasChnIdxParam);
	//选看适配：获取对应模式的通道
	BOOL32 GetBasSelChnGrp(u8 &byNum, CBasChn **pcBasChn, u8 byMediaMode);
	//选看适配：下参
	BOOL32 SetBasSelChnParam(const TEqp &tEqp, u8 byChnId, u8 byMediaMode, THDAdaptParam &tBasParam);
	BOOL32 SetBasSelAudChnParam(const TEqp &tEqp, u8 byChnId, TAudAdaptParam &tBasParam);
	//选看适配： 获取所有广播组通道
	BOOL32 GetBasAllSelChn(u8 &byNum, CBasChn **pcBasChn);	
	
	//公共部分
	//获取对应通道所属组
	u8     GetBasGrpType(const TEqp &tEqp, u8 byChnId);	
	//设置对应通道模式
	BOOL32 SetBasChnMode(const TEqp &tEqp, u8 byChnId, u8 byMediaMode);
	//获取对应通道模式
	u8     GetBasChnMode(const TEqp &tEqp, u8 byChnId);	
	//判断BAS外设是否有通道在该会议上
	BOOL32 IsExistChnOnEqp(const TEqp &tEqp);
	//设置通道源信息
	BOOL32 SetBasChnSrc(const TEqp &tEqp, u8 byChnId, TMt tSrc);
	//获取通道源信息
	TMt    GetBasChnSrc(const TEqp &tEqp, u8 byChnId);	
	//设置tick
	BOOL32 SetBasVcuTick(const TEqp &tEqp, u8 byChnId, u32 dwTick);
	//获取tick
	u32    GetBasVcuTick(const TEqp &tEqp, u8 byChnId);	
	//获取指定通道的索引(用于定时)
	u8     GetBasPosition(const TEqp &tEqp, u8 byChnId);	
	//打印
	void   PrintBas();
	
	//刷新通道某一出参数，帧率与码率刷新
	BOOL32	RefreshBasParambyOutIdx(TBasOutInfo &tBasOutInfo, THDAdaptParam tBasParam);

	//开启某个通道的适配
	BOOL32	StartBasChnAdapt(const TEqp &tEqp, u8 byChnId, TBasAdaptParamExt &tBasParamExt);

	//停止某个通道的适配
	BOOL32	StopBasChnAdapt(const TEqp &tEqp, u8 byChnId);

	//获得某个通道的输出路数
	BOOL32	GetBasChnOutputNum(const TEqp &tEqp, u8 byChnId, u8 &byOutNum, u8 &byFrontOutNum);

	//获得对应通道能力集
	BOOL32	GetBasResDataByEqp(TEqp tDiscBas, TNeedVidAdaptData *patBasResData);

	BOOL32 GetBasResAudDataByEqp(TEqp tDiscBas, TNeedAudAdaptData *patBasResData);

	/*-----------------------------------------------------------------------*/

	/*--------------------------Prs相关接口----------------------------------*/
	//分配Prs通道
	BOOL32 AssignPrsChn(u8 byPrsId, u8 byPrsChnlId, u8 byMediaMode,  BOOL bBrdChn = FALSE);
	//移除prs通道
	BOOL32 RemovePrsChn(u8 byPrsId, u8 byPrsChnId);

	//广播PRS通道是否已分配
	BOOL32 IsBrdPrsAssigned();
	//为指定Prs通道设置源信息
	BOOL32 SetPrsChnSrc(u8 byPrsId, u8 byPrsChnId, const TMt &tPrsSrc, u8 bySrcOutChn);
	//根据指定广播媒体模式获取预占的prs通道
	BOOL32 FindPrsChnForBrd(u8 byBrdMode, u8 &byPrsId, u8 &byPrsChnId);
	//为指定源寻找对应占用的prs通道
	BOOL32 FindPrsChnForSrc(const TMt& tPrsSrc, u8 bySrcOutChnl, u8 byMode, u8 &byPrsId, u8 &byPrsChnlId);
	//为指定prs通道寻找对应的源信息
	BOOL32 FindPrsChnlSrc(u8 byPrsId, u8 byPrsChnlId, u8& byMediaMode, TMt& tPrsSrc, u8& bySrcOutChnl);
	//获取指定prs单板在当前会议使用的所有通道
	u8 GetSpecPrsIdAllChnForConf(u8 byPrsId, u8 *pbyPrsChnId);
	//获取当前会议占用的所有prs通道
	u8 GetConfAllPrsChn(u8 *pbyPrsId, u8 *pbyChnId);
	//获取某个prs通道的媒体模式
	u8 GetPrsChnMediaMode(u8 byPrsId, u8 byPrsChnId);
	//当前prs通道是否已开启
	BOOL32 IsPrsChnStart(u8 byPrsId, u8 byPrsChnId);
	BOOL32 SetPrsChnStart(u8 byPrsId, u8 byPrsChnId, BOOL32 bStart);
	//获取Prs管理数据区
	u32    GetPrsMgrBuf(u8 *pbyBuf);
	//设置Prs管理数据区
	u32    SetPrsMgrBuf(u8 *pbyBuf);
	//打印
	void PrintPrs();
	/*-----------------------------------------------------------------------*/


	TMt GetVidBrdSrc();
	TMt GetSecVidBrdSrc();
	TMt GetLocalVidBrdSrc();
	void SetVidBrdSrc(TMt& tMt);
	void SetVidBrdSrcNull();
	BOOL32 IsVidBrdSrcNull();

	TMt GetAudBrdSrc();
	TMt GetLocalAudBrdSrc();
	void SetAudBrdSrc(TMt& tMt);
	void SetAudBrdSrcNull();
	BOOL32 IsAudBrdSrcNull();

    BOOL32 IsMultiCastMt( u8 byMtId ); //卫星会议专用
	BOOL32 IsSatMtSend(const TMt &tMt ); // [pengguofeng 2/18/2013]终端是不是已经在发码流

	TMt GetLocalMtFromOtherMcuMt(const TMt& tMt);
	TMt GetMcuIdxMtFromMcuIdMt( const TMt& tMt );
	TMt GetMcuIdMtFromMcuIdxMt( const TMt &tMt );
	u8  GetFstMcuIdFromMcuIdx( u16 wMcuIdx );
	u16 GetMcuIdxFromMcuId( u8 byMcuId );
	u16 GetMcuIdxFromMcuId( u8 *pMcuId,u8 byLevel = 1 );

	TMultiCacMtInfo BuildMultiCascadeMtInfo( const TMt &tMt,TMt &tNormalMt );

	TMt GetMtFromMultiCascadeMtInfo( const TMultiCacMtInfo &tMultiCacMtInfo,const TMt &tMt );

	//liuxu, 获取m_ptConfOtherMcTable
	TConfOtherMcTable * GetConfOtherMc() { return m_ptConfOtherMcTable;}

	TEqp GetVmpEqp() { return m_tVmpEqp; }

	// [9/17/2010 liuxu] 添加一个更改终端录像终端的接口
protected:
	/** 
	/* @功能说明：设置终端tMt的录像状态，此终端可以是本级终端，也可以是级联终端
	/* @参数说明：
	/*         	 tMt : 终端的TMt结构
	/*           bRecording ： 是否录像，默认是录像
	/* @返回值：无
	/* @作者 ： 刘旭       
	/* @日期： 2010-09-17
	*/
	void SetMtRecording(const TMt& tMt, BOOL32 bRecording = TRUE);

	virtual ~CMcuSingleConfAllInfo( ) { }
public:
// 	BOOL32 IsMtCountExtraPoint(u8 byMtId) const;
// 	BOOL32 AddMtExtraPoint(u8 byMtId);
// 	BOOL32 DelMtExtraPoint(u8 byMtId);

	/*====================================================================
	函数名      ：IsMSSupportCapEx
	功能        ：判断主流是否支持某扩展能力集
	引用全局变量：
	输入参数说明：[IN]byRes 分辨率
	[IN]byFps 帧率,默认为0，即不比较该项
	[IN]emProfileAttrb eProf HP/BP属性,默认为0，即不比较该项
	[IN]u16 wMaxBitRate 码率 默认为0，即不比较该项
	返回值说明  ：支持返回TRUE，否则返回FALSE。
	----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	2011/11/28  4.0         chendaiwei       创建
	====================================================================*/
	BOOL32 IsMSSupportCapEx(u8 byRes, u8 byFps = 0, emProfileAttrb eProf = emBpAttrb, u16 wMaxBitRate = 0)
	{
		BOOL32 bResult = FALSE;

		TVideoStreamCap tCap[MAX_CONF_CAP_EX_NUM];
		u8 byCapNum = MAX_CONF_CAP_EX_NUM;
		m_tConfEx.GetMainStreamCapEx(&tCap[0],byCapNum);
		
		// 只查找分辨率 [12/27/2011 chendaiwei]
		if(byFps == 0)
		{
			for(u8 byIdx = 0; byIdx < MAX_CONF_CAP_EX_NUM; byIdx++ )
			{
				if(tCap[byIdx].GetMediaType() == MEDIA_TYPE_NULL)
				{
					break;
				}
				
				if(tCap[byIdx].GetResolution() == byRes )
				{
					bResult = TRUE;
					break;
				}
			}
		}
		//忽略码率
		else if(wMaxBitRate == 0)
		{
			for(u8 byIdx = 0; byIdx < MAX_CONF_CAP_EX_NUM; byIdx++ )
			{
				if(tCap[byIdx].GetMediaType() == MEDIA_TYPE_NULL)
				{
					break;
				}
				
				if( tCap[byIdx].GetResolution() == byRes 
					&& tCap[byIdx].GetH264ProfileAttrb() == eProf 
					&& tCap[byIdx].GetUserDefFrameRate() == byFps)
				{
					bResult = TRUE;
					break;
				}
			}
		}
		//完全匹配
		else
		{
			for(u8 byIdx = 0; byIdx < MAX_CONF_CAP_EX_NUM; byIdx++ )
			{
				if(tCap[byIdx].GetMediaType() == MEDIA_TYPE_NULL)
				{
					break;
				}
				
				if( tCap[byIdx].GetResolution() == byRes 
					&& tCap[byIdx].GetH264ProfileAttrb() == eProf 
					&& tCap[byIdx].GetUserDefFrameRate() == byFps
					&& tCap[byIdx].GetMaxBitRate() == wMaxBitRate)
				{
					bResult = TRUE;
					break;
				}
			}
		}
		
		return bResult;
	}

	/*====================================================================
	函数名      ：IsDSSupportCapEx
	功能        ：判断双流是否支持某扩展能力集
	引用全局变量：
	输入参数说明：[IN]byRes 分辨率
	[IN]byFps 帧率,默认为0，即不比较该项
	[IN]emProfileAttrb eProf HP/BP属性,默认为0，即不比较该项
	[IN]u16 wMaxBitRate 码率 默认为0，即不比较该项
	返回值说明  ：支持返回TRUE，否则返回FALSE。
	----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	2011/11/28  4.0         chendaiwei       创建
	====================================================================*/
	BOOL32 IsDSSupportCapEx(u8 byRes, u8 byFps = 0, emProfileAttrb eProf = emBpAttrb, u16 wMaxBitRate = 0)
	{
		BOOL32 bResult = FALSE;
		
		u8 byCapNum = MAX_CONF_CAP_EX_NUM;
		TVideoStreamCap tCap[MAX_CONF_CAP_EX_NUM];
		m_tConfEx.GetDoubleStreamCapEx(&tCap[0],byCapNum);
		
		// 只查找分辨率 [12/27/2011 chendaiwei]
		if(byFps == 0)
		{
			for(u8 byIdx = 0; byIdx < MAX_CONF_CAP_EX_NUM; byIdx++ )
			{
				if(tCap[byIdx].GetMediaType() == MEDIA_TYPE_NULL)
				{
					break;
				}
				
				if(tCap[byIdx].GetResolution() == byRes )
				{
					bResult = TRUE;
					break;
				}
			}
		}
		//忽略码率
		else if(wMaxBitRate == 0)
		{
			for(u8 byIdx = 0; byIdx < MAX_CONF_CAP_EX_NUM; byIdx++ )
			{
				if(tCap[byIdx].GetMediaType() == MEDIA_TYPE_NULL)
				{
					break;
				}
				
				if( tCap[byIdx].GetResolution() == byRes 
					&& tCap[byIdx].GetH264ProfileAttrb() == eProf 
					&& tCap[byIdx].GetUserDefFrameRate() == byFps)
				{
					bResult = TRUE;
					break;
				}
			}
		}
		//完全匹配
		else
		{
			for(u8 byIdx = 0; byIdx < MAX_CONF_CAP_EX_NUM; byIdx++ )
			{
				if(tCap[byIdx].GetMediaType() == MEDIA_TYPE_NULL)
				{
					break;
				}
				
				if( tCap[byIdx].GetResolution() == byRes 
					&& tCap[byIdx].GetH264ProfileAttrb() == eProf 
					&& tCap[byIdx].GetUserDefFrameRate() == byFps
					&& tCap[byIdx].GetMaxBitRate() == wMaxBitRate)
				{
					bResult = TRUE;
					break;
				}
			}
		}
		
		return bResult;
	}

	/*====================================================================
	函数名      ：FindMSMatchedCapEx
	功能        ：查找小于阀值分辨率的能力集勾选
	引用全局变量：
	输入参数说明：[IN][OUT]TVideoStreamCap &tDstCapEx
				  [IN] TVideoStreamCap tSrcCap 源能力集
				  [IN]u8 byRes 阀值分辨率
					
	返回值说明  ：查找成功返回TRUE，否则返回FALSE。
	----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	2011/11/28  4.0         chendaiwei       创建
	====================================================================*/
	BOOL32 FindMSMatchedCapEx(TVideoStreamCap &tDstCapEx, TVideoStreamCap tSrcCap, u8 byRes, u8 byFps)
	{
		BOOL32 bResult = FALSE;
		TVideoStreamCap tCap[MAX_CONF_CAP_EX_NUM];
		u8 byCapNum = MAX_CONF_CAP_EX_NUM;
		m_tConfEx.GetMainStreamCapEx(tCap,byCapNum);

		for(u8 byIdx = 0; byIdx < MAX_CONF_CAP_EX_NUM; byIdx++)
		{
			if( tCap[byIdx].GetMediaType() == MEDIA_TYPE_NULL )
			{
				break;																	
			}
			/*
			//1)比阀值分辨率更小，比源帧率更小或相等，HP/BP属性相同
			//2)与阀值分辨率相同，比源帧率更小，HP/BP属性相同
			//3)源1080/30/25 勾选含有720/60特殊情况
			if( ((tCap[byIdx].GetResolution() == byRes 
				  && byRes == VIDEO_FORMAT_HD1080
				  && tCap[byIdx].GetUserDefFrameRate() < tSrcCap.GetUserDefFrameRate()  )
				  ||
				  (tCap[byIdx].IsResLowerWithOutMatched(tCap[byIdx].GetResolution(),byRes) 
				   && tCap[byIdx].GetUserDefFrameRate() <= tSrcCap.GetUserDefFrameRate() )
				  || 
				  (tCap[byIdx].GetResolution() == VIDEO_FORMAT_HD720 
				   && tCap[byIdx].GetUserDefFrameRate() >30
				   && tSrcCap.GetResolution() == VIDEO_FORMAT_HD1080
				   && tSrcCap.GetUserDefFrameRate() <= 30)
				)
				&& (tCap[byIdx].GetH264ProfileAttrb() == tSrcCap.GetH264ProfileAttrb())
			  )
			{
				tDstCapEx = tCap[byIdx];
				bResult = TRUE;
				break;
			}*/

			//1)比阀值分辨率更小，比阀值帧率更小或相等，HP/BP属性相同
			//2)与阀值分辨率相同，比阀值帧率更小，HP/BP属性相同
			if( ((tCap[byIdx].GetResolution() == byRes 
				&& tCap[byIdx].GetUserDefFrameRate() < byFps  )
				||
				(tCap[byIdx].IsResLowerWithOutMatched(tCap[byIdx].GetResolution(),byRes) 
				&& tCap[byIdx].GetUserDefFrameRate() <= tSrcCap.GetUserDefFrameRate() ))
				&& (tCap[byIdx].GetH264ProfileAttrb() == tSrcCap.GetH264ProfileAttrb())
				)
			{
				tDstCapEx = tCap[byIdx];
				bResult = TRUE;
				break;
			}
		}

		if(!bResult)
		{	
			for(u8 byIdx = 0; byIdx < MAX_CONF_CAP_EX_NUM; byIdx++)
			{
				if( tCap[byIdx].GetMediaType() == MEDIA_TYPE_NULL )
				{
					break;																	
				}
				//在大于等于阀值和小于等于源能力的基础上找最小匹配能力集
				if( !tCap[byIdx].IsResLowerWithOutMatched(tCap[byIdx].GetResolution(),byRes)
					&& !tSrcCap.IsResLowerWithOutMatched(tSrcCap.GetResolution(),tCap[byIdx].GetResolution())
					&& tCap[byIdx].GetUserDefFrameRate() <= tSrcCap.GetUserDefFrameRate()
					&& tCap[byIdx].GetUserDefFrameRate() >= byFps
					&& (tCap[byIdx].GetH264ProfileAttrb() == tSrcCap.GetH264ProfileAttrb()))
				{
					tDstCapEx = tCap[byIdx];
					bResult = TRUE;
				}
			}
		}

		return bResult;
	}

	//判断会议除非双流外是否是适配会议[2/15/2012 yanghuaizhi]
	BOOL32 IsConfExcludeDDSUseAdapt()const
	{
		if (0 != m_tConf.GetSecBitRate() ||
			(MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType() &&
			0 != m_tConf.GetSecVideoMediaType()))
		{
			return TRUE;
		}
		else
		{
			if (
				(MEDIA_TYPE_H264 == m_tConf.GetMainVideoMediaType() &&
					 m_tConfEx.IsMainHasCapEx() //2)v4r7模板有勾选
				)
				||
				(m_tConf.GetSecAudioMediaType() != MEDIA_TYPE_NULL &&
				m_tConf.GetSecAudioMediaType() != 0 &&            
				m_tConf.GetSecAudioMediaType() != m_tConf.GetMainAudioMediaType())
				)
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}
};


// 业务实例(Osp功能类)
class CMcuVcInst : public CInstance,
                   public CMcuSingleConfAllInfo
{
public:
	enum 
	{ 
		STATE_IDLE,       //空闲
		STATE_WAITFOR,     // 等待外设
		STATE_SCHEDULED,  //预约
        STATE_ONGOING     //即时
	};

public:	
    CMcuVcInst();
	virtual ~CMcuVcInst();
	BOOL32 IsMtIpV6( u8 byMtId ); //mp侧需要[pengguofeng 5/10/2012] 

protected:

    /************************************************************************/
    /*                                                                      */
    /*                       一、daemon处理函数                             */
    /*                                                                      */
    /************************************************************************/

	//1. daemon实例处理函数
	void DaemonProcCommonNotif( const CMessage * pcMsg );

    void DaemonProcPowerOn( void );
    void DaemonProcMcuMSStateNtf( const CMessage * pcMsg );    

    //2. daemon 会议/模板管理
    void DaemonProcMcsMcuCreateConfReq(const CMessage * pcMsg);
    void DaemonProcMcsMcuCreateConfByTemplateReq(const CMessage * pcMsg);
    void DaemonProcMcsMcuSaveConfToTemplateReq( const CMessage * pcMsg );
    void DaemonProcCreateConfNPlus(const CMessage * pcMsg);
    void DaemonProcMcsMcuTemplateOpr(const CMessage * pcMsg);
    void DaemonProcMtMcuCreateConfReq( const CMessage * pcMsg );
    void DaemonProcMcsMcuListAllConfReq( const CMessage * pcMsg );
    void DaemonProcDcsMcuReleaseConfRsp( const CMessage * pcMsg );  //2006-01-11
    BOOL32 NtfMcsMcuCurListAllConf( CServMsg &cServMsg );

    //3. daemon GK 管理
    void DaemonProcGkRegRsp(const CMessage * pcMsg);
    void DaemonProcGKChargeRsp(const CMessage * pcMsg);
    void DaemonProcMcuReRegisterGKNtf( void );
	
    //4. daemon 外设管理
	void DaemonProBasChnnlStatusNotif( const CMessage * pcMsg );
    void DaemonProcHDBasChnnlStatusNotif( const CMessage * pcMsg );
	void DaemonProcAudBasChnnlStatusNotif(const CMessage *pcMsg);
	void DaemonProcMcsMcuGetPeriEqpStatusReq( const CMessage * pcMsg );
	void DaemonProcRecMcuProgNotif( const CMessage * pcMsg );
	void DaemonProcRecMcuChnnlStatusNotif( const CMessage * pcMsg );
    void DaemonProcHduMcuChnnlStatusNotif( const CMessage * pcMsg );

	void DaemonProcGetDsInfoAck( const CMessage * pcMsg );
    void DaemonProcMpFluxNotify( const CMessage * pcMsg );
    void DaemonProcPeriEqpMcuStatusNotif( const CMessage * pcMsg );
	void DaemonProcMtadpDiscAndRegMsg(const CMessage * pcMsg);

	void DaemonProc8KEPeriEqpMcuStatusNotif( const CMessage * pcMsg );
	
	void DaemonProcMcsMcuStopSwitchMcReq( const CMessage * pcMsg );
	void DaemonProcMcsMcuStopSwitchTWReq( const CMessage * pcMsg );
	
	void DaemonProcMcsMcuGetMcuStatusReq( const CMessage * pcMsg );
	void DaemonProcMcsMcuSetMcuCfgCmd( const CMessage * pcMsg );	

	void DaemonProcMcuMtConnectedNotif( const CMessage * pcMsg );
	void DaemonProcMcuEqpConnectedNotif( const CMessage * pcMsg );
	void DaemonProcMcuEqpDisconnectedNotif( const CMessage * pcMsg );
	void DaemonProcMcuDcsConnectedNtf( const CMessage * pcMsg );	//2005-12-15
	void DaemonProcMcuDcsDisconnectedNtf( const CMessage * pcMsg );	//2005-12-15	
	void DaemonProcMcuMcsConnectedNotif( const CMessage * pcMsg );
	void DaemonProcMcuMcsDisconnectedNotif( const CMessage * pcMsg );
	void DaemonProcMtMcuApplyJoinReq( const CMessage * pcMsg );
	void DaemonProcMtMcuApplyJoinReq(BOOL32 bLowLevleMcuCalledIn, const CConfId &cConfId, const CMessage * pcMsg);
    
    void DaemonProcNPlusRegBackupRsp( const CMessage * pcMsg );    
    void DaemonProcAppTaskRequest( const CMessage * pcMsg );
	void DaemonProcHDIFullNtf ( const CMessage *pcMsg );
    void DaemonProcAgtSvcE1BandWidthNotif( const CMessage *pcMsg ); 	
    
    //主控热备份单元测试用函数
//     void DaemonProcUnitTestMsg( void );
//     void ProcUnitTestGetTmpInfoReq( CMessage *pcMsg );
//     void ProcUnitTestGetConfInfoReq( CMessage *pcMsg );
//     void ProcUnitTestGetMtListInfoReq( CMessage *pcMsg );
//     void ProcUnitTestGetAddrbookReq( CMessage *pcMsg );
//     void ProcUnitTestGetMcuCfgReq( CMessage *pcMsg );
	
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
	void ProcMsSynRecoverConfTimer( const CMessage * pcMsg );
	void PowerOnNPlusApp(); //发送power_on消息给Nplus业务  [6/5/2012 chendaiwei]
	void ProcTimerVacChange( const CMessage * pcMsg );
    void ProcTimerAutoRec( const CMessage * pcMsg );
	//  [12/25/2009 pengjie] 刷新自动录像定时器
	void ProcTimerUpdataAutoRec( const CMessage * pcMsg );
	BOOL32 SetAutoRec( u16 &wErrCode );
    // End

	BOOL32 IsConfFileGBK(); // 判断会议文件是否为GBK [pengguofeng 7/18/2013]

	// [pengjie 2010/9/29] 检测会议是否有广播源
	void ProcTimerConfHasBrdSrc( void );
	// End

	// [pengjie 2010/2/26] 双流放像，mcu定时发令牌环
	void ProcTimer239TokenNtf( void );
	// End

	// 2011-8-5 add by pgf:检测有没有指定IP的能力存在
	BOOL32 IsCanGetSpecCapByMtIp(const s8* pIpAddr, u32 adwCapSet[] , BOOL32 bIsGetMainCapSet = TRUE );
	// 2011-8-5 add end

    void ProcWaitEqpToCreateConf(CMessage * const pcMsg);

    void CreateConfEqpInsufficientNack( u8 byCreateMtId, u8 byOldInsId, const s32 nErrCode, CServMsg &cServMsg );
    void CreateConfSendMcsNack( u8 byOldInsId, const s32 nErrCode, CServMsg &cServMsg, BOOL32 bDeleteAlias = FALSE );
    BOOL32 IsConfInfoCheckPass( const CServMsg &cSrcMsg, TConfInfo &tConfInfo, u16 &wErrCode, BOOL32 bTemplate = FALSE);
    BOOL32 IsEqpInfoCheckPass( const CServMsg &cSrcMsg, u16 &wErrCode);
    void ConfDataCoordinate(CServMsg &cMsg, u8 &byMtNum, u8 &byCreateMtId);
	void NPlusVmpParamCoordinate (CServMsg &cServMsg);
	//[12/29/2011 chendaiwei]从创会消息获得TConfInfoEx数据
	BOOL32 GetConfExInfoFromMsg(CServMsg &cServMsg, TConfInfoEx &tConfInfoEx);
    void AddSecDSCapByDebug(TConfInfo &tConfInfo);

	// 20130221 zhouyiliang 判断周期性预约会议是否需要开启即时会议
	BOOL32 IsCircleScheduleConfNeedStart( );
	TKdvTime GetNextOngoingTime(const TConfInfo& tConfInfo, const TDurationDate& tValidDuraDate,BOOL32 bAcceptCurOlder = FALSE);
	//根据starttime判断是否是周期性预约会议
	BOOL32 IsCircleScheduleConf(const TKdvTime& tStartTime);
	//保证durastart的时分秒为00:00:00,duraend(有值的话)的时分秒为23:59:59
	void   ModifyCircleScheduleConfDuraDate( TDurationDate& tDuarationDate );


	BOOL32 PrepareAllNeedBasForConf(u16* pwErrCode = NULL);
	void   RlsAllBasForConf();

    void ReleaseConf( BOOL32 bRemoveFromFile = TRUE);
    u32  PackMtInfoListIntoBuf(const TMt atMt[], const TMtAlias	atMtAlias[],
                               u8	byArraySize, u8 *pbyBuf, u32 dwBufLen );	
    void ChangeConfLockMode( CServMsg &cMsg );
    
    void SendReplyBack( CServMsg & cReplyMsg, u16 wEvent );
    BOOL32 IsMtCmdFromThisMcu( const CServMsg & cCmdReply ) const;
    BOOL32 IsMcCmdFromThisMcu( const CServMsg & cCmdReply ) const;
	BOOL32 IsMsgNeedTranslate( u16 wEventId );
	void TranslateMsg( u16 wEvent,CServMsg & cServMsg );
    BOOL32 SendMsgToMt( u8 byMtId, u16 wEvent, CServMsg & cServMsg );
    BOOL32 SendMsgToEqp( u8 byEqpId, u16 wEvent, CServMsg & cServMsg );
    BOOL32 SendMsgToMcs( u8 byMcsId, u16 wEvent, CServMsg & cServMsg );
    BOOL32 SendOneMsgToMcs( CServMsg &cServMsg, u16 wEvent );
    void SendMsgToAllMcs( u16 wEvent, CServMsg & cServMsg );
    void SendMsgToAllMp( u16 wEvent, CServMsg & cServMsg);  // libo [4/27/2005] add
	void SendMtListToMcs(u16 wMcuIdx, const BOOL32 bForce = FALSE, const BOOL32 bMMcuReq = FALSE );//(u8 byMcuId)
	void SendAllMtInfoToAllMcs( u16 wEvent, CServMsg & cServMsg, const BOOL32 bForce = FALSE );
    void SendMtAliasToMcs(TMt tMt);
    void BroadcastToAllSubMtJoinedConf( u16 wEvent, CServMsg & cServMsg );
    void BroadcastToAllMcu( u16 wEvent, CServMsg & cServMsg);
    BOOL32 SendMsgToDcsSsn( u8 byInst, CServMsg &cServMsg ); //2005-12-16
    
    void SendTrapMsg( u16 wEvent, u8 * const pbyMsg, u16 wLen );    
    
    void NotifyMtSend( u8 byDstMtId, u8 byMode = MODE_BOTH, BOOL32 bStart = TRUE );
public: //给MpManager用
    void NotifyMtReceive(  const TMt & tSrc, u8 byDstMtId );	
protected:
    void StartMtSendbyNeed( TSwitchChannel* ptSwitchChannel );
    void StopMtSendbyNeed( TSwitchChannel* ptSwitchChannel );
    void NotifyRecStartPlaybyNeed( TSwitchChannel* ptSwitchChannel ); // zgc, 2007-02-27
    void NotifyOtherMtSend( u8 byDstMtId, BOOL32 bStart );  //通知其他厂商终端是否发送视频码流        	
    BOOL32 IsOtherMtCanBeStopSend( u8 byDstMtId );
    // guzh [7/27/2007] 请求VCU，发送给终端，外设
    void NotifyFastUpdate(const TMt &tDst, u8 byChnlMode, BOOL32 bSetTimer = FALSE);
    void NotifyMtFastUpdate(u8 byMtId, u8 byChnlMode, BOOL32 bSetTimer = FALSE);
    void NotifyEqpFastUpdate(const TMt &tDst, u8 byChnlMode, BOOL32 bSetTimer = FALSE);
	void NotifyChairmanMixMode(void);

	// [8/18/2010 xliang] 通知源开始发送并编关键帧(源可以是MT或EQP)
	void NotifySrcSend(const TMt &tSrcMt, u8 byMode = MODE_BOTH, u8 bySrcChnnl = 0 , BOOL32 bFastUpdateTimer = TRUE);
    
	//[nizhijun 2011/09/09]额外MCU状态信息通告
	void SendExtMcuStatusNotify(u8 byMcsId, u8 nCurIndex,  BOOL32 bIsSendAll = FALSE);

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
	void ProcTimerRecreateDataConf( void );
    
    //3、gk 计费
    void ProcConfRegGkAck( const CMessage * pcMsg );
	void ProcConfRegGkNack( const CMessage * pcMsg );
    void ProcGKChargeRsp( const CMessage * pcMsg );
	
	//4、普通会议操作 
    void ChangeVidBrdSrc( TMt * ptNewVidBrdSrc, u16 wOldVidBrdSpyPort = SPY_CHANNL_NULL );
    void ChangeAudBrdSrc( TMt * ptNewAudBrdSrc, u16 wOldAudBrdSpyPort = SPY_CHANNL_NULL );
    void ChangeSpeaker(  TMt * ptNewSpeaker, BOOL32 bPolling = FALSE, BOOL32 bAddToVmp = TRUE,BOOL32 bNeedPreSetin = TRUE , BOOL32 bIsRecPlayChange = FALSE);
	void ChangeSecSpeaker(TMt &tNewSpeaker, BOOL32 bNeedPreSetin = TRUE);
    void ChangeChairman(  TMt * ptNewChairman, BOOL32 bAdjustSwitch = TRUE );
    void AdjustChairmanSwitch( TMt * ptOldChair = NULL );
	void AdjustChairmanVidSwitch( TMt *ptOldChair = NULL );
	void AdjustChairmanAudSwitch( void );
    
    TMt  GetLocalSpeaker( void );
    BOOL32 HasJoinedSpeaker( void );
	BOOL32 HasJoinedChairman( void );
	BOOL32 IsSelectedbyOtherMt( const TMt &tMt, u8 byMode = MODE_VIDEO);
	BOOL32 IsSelectedbyOtherMtInMultiCas( const TMt &tMt, u8 byMode = MODE_VIDEO);
	BOOL32 IsKedaMt(const TMt &tMt, BOOL32 bIsNeedLocal);	// [1/18/2010 xliang] 是否是科达的终端
	BOOL32 IsNoneKedaMtCanInMpu(const TMt & tMt);//【03/09/2011 zhouyiliang】判断非科达厂商终端是否能进画面合成
	BOOL32 IsG400IPCMt(const TMt & tMt); // 判断该终端是不是IPC前端 [pengguofeng 10/31/2012]
	BOOL32 IsPhoneMt(const TMt & tMt); // 判断该终端是不是电话终端
	

	// [8/5/2011 liuxu] 添加ptNewSpeaker参数, 录像机放像从暂停状态切换为播放状态时, 可以广播码流
	BOOL32 IsSpeakerCanBrdVid( const TMt* ptNewSpeaker = NULL );
	BOOL32 IsSpeakerCanBrdAud( const TMt* ptNewSpeaker = NULL );

	void   AdjustOldSpeakerSwitch( TMt tOldSpeaker, BOOL32 bIsHaveNewSpeaker, BOOL32 bSameInSMcu ,const TMt & tNewSpeaker);
	void   AdjustNewSpeakerSwitch( TMt tNewSpeaker, /*BOOL32 bAddToVmp, */u16 wOldSpeakerSpyPort = SPY_CHANNL_NULL );
	void   ChangeSpeakerSrc( u8 byMode, emChangeSpeakerSrcReason emReason, u16 wSpyStartPort = SPY_CHANNL_NULL);
	void   ChangeOldSpeakerSrc( u8 byMode, TMt tOldSpeaker, BOOL32 bIsHaveNewSpeaker );
	// 2011/07/06 add by peng guofeng start
	void   ChgSpeakerInMixer( TMt tOldSpeaker, TMt *ptNewSpeaker );
	BOOL32   ChgSpeakerInVmp( TMt tOldSpeaker, TMt tNewSpeaker, BOOL32 bAddToVmp );
	void   ChgSpeakerInTvWall( TMt tNewSpeaker );
	void   ChgSpeakerInHdu( TMt tNewSpeaker );
	// 2011/07/06 add end
    
    void ConfStatusChange( void );
    void ConfModeChange( void );
//  void MtStatusChange( u8 byMtId = 0, BOOL32 bForcely = FALSE );
// 	void MtStatusChange(const TMt* ptMt );
public: //给MpManager专用
    void MtStatusChange(const TMt* ptMt = NULL, BOOL32 bForcely = FALSE );
protected:
	// [9/27/2011 liuxu] 发送缓冲区内容
	void SendMtStatusInBuf( const BOOL32 bClearBuf = TRUE );
	void SendAllLocalMtStatus( const u16 wEvent = MCU_MCS_MTSTATUS_NOTIF );
	void MtSecSelSrcChange(const TMt &tMt);

    void MtInfoChange( void );
    void MtOnlineChange( TMt tMt, BOOL32 bOnline, u8 byReason );
    void NotifyMcsAlarmInfo(u8 byMcsId, u16 wErrorCode);    //byMcsId为0则通知所有mcs
    void NotifyMcsApplyList( BOOL32 bSendToChairman = FALSE );
    void SendConfInfoToChairMt(void);
    void ProcTimerRefreshMcs( void );    

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
	void ProcBatchMtSmsOpr(const CServMsg &cServMsg);
	void ProcSingleMtSmsOpr(const CServMsg &cServMsg, TMt *ptDstMt, CRollMsg *ptROLLMSG);
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
	void ProcMtMuteDumbReq(CServMsg &cServMsg);
	void ProcSingleMtMuteDumbOpr(CServMsg &cServMsg, TMt *ptDstMt, u8 byMuteOpenFlag, u8 byMuteType);
	void ProcMcsMcuPollMsg(const CMessage * pcMsg);
	void ProcPollingChangeTimerMsg(const CMessage *pcMsg);    
    void ProcStopConfPoll(BOOL32 bNeedAdjustVmp = TRUE);
    void ProcMcsMcuHduPollMsg(const CMessage *pcMsg);
	void ProcMcsMcuHduBatchPollMsg(const CMessage *pcMsg);    //  处理hdu批量轮询
    void ProcMcsMcuTWPollMsg(const CMessage *pcMsg);
    void ProcTWPollingChangeTimerMsg(const CMessage *pcMsg);
	//void ProcHduPollingChangeTimerMsg(const CMessage *pcMsg);
    void ProcHduBatchPollChangeTimerMsg(const CMessage *pcMsg);  // hdu批量定时处理
	void ProcMcsMcuMcuMediaSrcReq(const CMessage *pcMsg);
	TMtPollParam *GetNextMtPolled(u8 &byPollPos, TPollInfo& tPollInfo);
    TMtPollParam *GetMtTWPollParam(u8 &byPollPos, u8 byEqpId, u8 byChnId);
    //TMtPollParam *GetMtHduPollParam(u8 &byPollPos);
	void ProcMcsMcuVmpPollMsg(const CMessage * pcMsg);			// vmp单通道轮询
    void ProcVmpPollingChangeTimerMsg(const CMessage *pcMsg);	// vmp单通道轮询定时处理
	TMtPollParam *GetNextMtPolledForVmp(u8 &byPollPos, TPollInfo& tPollInfo);	//找下一个合适的轮询终端
	void ProcStopVmpPoll();		// 停止Vmp单通道轮询
	void ProcVmpPollingByRemoveMt( const TMt &tRemoveMt );

	void ProcMcsMcuLockSMcuReq(const CMessage *pcMsg);
	void ProcMcsMcuGetMcuLockStatusReq(const CMessage *pcMsg);
    void NotifyMtToBePolledNext( void );
    void NotiyfMtConfSpeakMode( u8 byMtId, u8 byMode );
    void NotifyMtSpeakStatus( TMt tMt, emMtSpeakerStatus emStatus );
    void PollParamChangeNotify( void );
    void ProcMcsMcuRollCallMsg( const CMessage * pcMsg );
    void RollCallStop( CServMsg &cServMsg );
    void ProcMixStart( CServMsg &cServMsg );
	BOOL32 StartLocalMix( u8 byMtNum, emMcuMixMode eMixMode,TMt *ptMixMember,u8 byEqpId,u16 &wErrorCode);
	BOOL32 StartCascadeMix( u8 byMtNum, emMcuMixMode eMixMode,TMt *ptMixMember,u8 byEqpId,u8 byReplaceMember,u16 &wErrorCode);
	BOOL32 UnPackStartMixMsg (CServMsg &cServMsg, TMixMsgInfo & tMixMsg);
	//BOOL32 UnPackLocalStartMixMsg (CServMsg &cServMsg, TMixMsgInfo & tMixMsg);
	//BOOL32 UnPackStartCascadeMixMsg (CServMsg &cServMsg, TMixMsgInfo & tMixMsg);
	//BOOL32 IsStartMixCheckPass ( TMixMsgInfo & tMixMsg);
	BOOL32 IsStartLocalMixCheckPass ( u8 byMtNum, emMcuMixMode eMixMode,TMt *ptMixMember,u8 &byEqpId, u16 &wErrorCode);
	BOOL32 IsStartCascadeMixCheckPass ( u8 byMtNum, emMcuMixMode eMixMode,TMt *ptMixMember,u8 &byEqpId, u16 &wErrorCode);
	//void StartMixDataCoordinate ( TMixMsgInfo &tMixMsg);
	void StartLocalMixDataCoordinate ( u8 byMtNum, emMcuMixMode &eMixMode, TMt *ptMixMember, u8 byEqpId);
	void StartCascadeMixDataCoordinate ( u8 byMtNum, emMcuMixMode &eMixMode, TMt *ptMixMember, u8 byEqpId);
	void SwitchMixModeVacToVacWhole( void );
	void SwitchMixModeSpecToAutoMix( void );
    void ProcMixStop( CServMsg &cServMsg, BOOL32 bRollCallStop = FALSE );
	void ProcCancelSpyMtMsg( const CMessage * pcMsg );	
	// [pengjie 2010/5/17] 会议扩展信息通知界面(下级会议是否支持多回传)
	void SendConfExtInfoToMcs( u8 bySrcSsnId = 0,u8 byMtId = 0 );
	void ProcConfPollingByRemoveMt( const TMt &tRemoveMt );
	
	//zhouyiliang 20120921 多回传点名与被点名人模式下，点下级终端，等待回响应处理
	void SetLastMultiSpyRollCallFinish(const BOOL32 bFinish);
	BOOL32 IsLastMutiSpyRollCallFinished()const
	{
		return m_tConfInStatus.IsLastMutiSpyVmpRollCallFinished();
	}
	void ProcWaitLastVmpRollCallFinishTimer();
	void ProcWaitMSConfDataTimer();

	void ProcMcsMcuGetIFrameCmd(const CMessage *pcMsg);

    //5、卫星分散会议
    u8   GetSatCastChnnlNum(u8 bySrcMtId);
//     BOOL32 IsOverSatCastChnnlNum(u8 bySrcMtId, u8 byIncNum = 1);
// 	BOOL32 IsOverSatCastChnnlNum(TMt tMt, u8 byIncNum = 1);
    BOOL32 IsOverSatCastChnnlNum(TVMPParam_25Mem &tVmpParam);
//     void ChangeSatDConfMtRcvAddr(u8 byMtId, u8 byChnnlType, BOOL32 bMulti = TRUE);
public:
    void ChangeSatDConfMtRcvAddr(u8 byMtId, u8 byChnnlType, BOOL32 bMulti = TRUE);
	BOOL32 IsCanSatMtReceiveBrdSrc(const TMt &tMt, u8 byMode = MODE_VIDEO);
protected:
    BOOL32 IsMtSendingVidToOthers(TMt tMt, BOOL32 bConfPoll/*with audio*/, BOOL32 bTwPoll, u8 bySelDstMtId);
	// 判断替换终端时，卫星回传计数是否会超过模板中配置的数值 [pengguofeng 1/23/2013]
	BOOL32 IsSatMtOverConfDCastNum(const TMt &tNewMt, emSatReplaceType emReplaceType = emDefault,
		const u8 &bySkipVcId = 0xff, const u8 &bySkipVcChnnl = 0xff,
		const u8 &bySkipVmpChnnl = 0xff, const u8 &bySkipHduId = 0xff,
		const u8 &bySkipHduChnnl = 0xff, const u8 &bySkipMtId = 0 );
	// 卫星终端会不会维持发送码流的状态 [pengguofeng 2/18/2013];参数继承上面这个接口
	BOOL32 IsMtKeepSatChnnl(const TMt &tMt, emSatReplaceType emReplaceType,
		const u8 &bySkipVcId = 0xff, const u8 &bySkipVcChnnl = 0xff,
		const u8 &bySkipVmpChnnl = 0xff, const u8 &bySkipHduId = 0xff,
		const u8 &bySkipHduChnnl = 0xff, const u8 &bySkipMtId = 0 );
	void  SetCurSatMtNum(s16  swNum){	m_swCurSatMtNum = swNum;	}
	s16   GetCurSatMtNum(){	return m_swCurSatMtNum;	}
	BOOL32 IsSatMtCanContinue(u8                byNewSatMtId      =  0, 
							  emSatReplaceType  emServiceType     =  emDefault,
							  TEqp              *ptEqp            =  NULL,
						   	  u8                byChnId           =  0xFF,
							  u8                bySubChnId        =  0xFF,
							  TVMPParam_25Mem   *pTVMPParam       =  NULL);

    //6、N+1备份相关
    void ProcNPlusConfInfoUpdate(BOOL32 bStart);
    void ProcNPlusConfMtInfoUpdate(void);
    void ProcNPlusChairmanUpdate(void);
    void ProcNPlusSpeakerUpdate(void);
    void ProcNPlusVmpUpdate(void);
	void ProcNPlusAutoMixUpdate(BOOL32 bStart);
    void ProcNPlusConfDataUpdate(const CMessage * pcMsg);
    void GetNPlusDataFromConf(TNPlusConfData &tNPlusData, TNPlusVmpParam *ptNplusVmpParam, u8 &byVmpNum ,BOOL32 bCharge = FALSE);
	u16 GetNPlusDataExFromConf(TNPlusConfExData &tNPlusDataEx);
    
    //7、主控热备份：M-S Data-Exchange
public:
    BOOL32 GetInstStateAndConfIdIdx( u8 &byState ,CConfId & cConfId,u8& byConfIdx);
    BOOL32 SetInstStateAndConfIdIdx( const u8& byState ,const CConfId& cConfId, const u8& byConfIdx);
    
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

	void   SetWaitMsConfData( BOOL32 bIsWait );
	BOOL32 IsWaitMsConfData( void );
	void   SetInOtherConf( BOOL32 bIsInOtherConf,u8 byOtherConfMtId = 0 );
	BOOL32 IsInOtherConf( void );
 
	//8、VCS会议处理
	// 1) VCS Daemon实例处理的消息
	// 保存分组消息
	void DaemonProcVcsMcuSavePackinfoReq( const CMessage * pcMsg );	
	// 获取分组信息超时
	void DaemonProcPackInfoTimer( const CMessage * pcMsg );

	// 2)VCS普通实例相关消息处理入口
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
	void ProcVcsMcuMuteReq( CServMsg& cServMsg );
	// 调度终端超时处理
	void ProcVCMTOverTime( void );
	// 组呼终端请求处理
	void ProcVcsMcuGroupCallMtReq(const CMessage * pcMsg);
	void ProcVcsMcuGroupDropMtReq(const CMessage * pcMsg);
	// 主席轮询
    void ProcVcsMcuStartChairPollReq(const CMessage *pcMsg);
    void ProcVcsMcuStopChairPollReq(const CMessage *pcMsg);
	void VcsStopChairManPoll();
    void ProcChairPollTimer( void );
	// 临时增加终端
	void ProcVcsMcuAddMtReq(const CMessage *pcMsg);
	void ProcVcsMcuDelMtReq(const CMessage *pcMsg);
	// 抢占处理, 是否同意释放终端
	void ProcVcsMcuRlsMtMsg(const CMessage * pcMsg);
	// 处理上级级联会议命令,由上级接管会议调度
	void ProcMMCUGetConfCtrlMsg();
    //处理单板上报E1终端带宽指示
    void ProcAgtSvgE1BandWidthNotif( const CMessage * pcMsg );

	// VCS会议所需外设资源的检查
	BOOL32 IsVCSEqpInfoCheckPass(u16& wErrCode);
	// VCS会议信息的整理
    void VCSConfDataCoordinate(CServMsg& cServMsg, u8 &byMtNum, u8 &byCreateMtId);
	// VCS会议中实体呼叫模式查询
	u8   VCSGetCallMode(TMt tMt);
	// 通告VCS会议分组信息
	void NotifyVCSPackInfo(CConfId& cConfId, u8 byVcsId = 0);


	// VCS呼叫挂断终端
	void VCSCallMT(CServMsg& cServMsgHdr, TMt& tMt, u8 byCallType);
	void VCSDropMT(TMt tMt);

	// VCS选看操作
	BOOL VCSConfSelMT(TSwitchInfo tSwitchInfo,BOOL32 bNeedPreSetin = TRUE );
	void VCSConfStopMTSel(TMt tDstMt, u8 byMode,BOOL32 bIsRestoreSeeBrd = TRUE );    // 取消终端选看
	void VCSConfStopMTSeled(TMt tSrcMt, u8 byMode);  // 取消终端被选看
    BOOL IsNeedSelApt(u8 bySrcMtId, u8 byDstMtId, u8 bySwitchMode);

	BOOL VCSConfMTToTW(TTWSwitchInfo tTWSwitchInfo);
	void VCSConfStopMTToTW(TMt tMt);
    // 对终端静哑音
	void VCSMTMute(TMt tMt, u8 byMuteOpenFlag, u8 byMuteType);
	u8   GetVcsHduAndTwModuleChnNum();


	void GoOnSelStep(TMt tMt, u8 byMediaType, u8 bySelDirect);	// 根据会议状态，继续进行未完成的选看步骤
	TMt  GetVCMTByAlias(TMtAlias& tMtAlias, u16 wMtDialRate, u8 byForce = FALSE); 	// 根据终端别名获取对应TMt
	void ChgCurVCMT(TMt tNewCurVCMT); 	// 根据会议状态，切换当前调度终端
	void ReLoadOrigMtList( BOOL32 bIsAdd = TRUE );            // 重新加载会议最初终端列表

	void VCSSwitchSrcToDst(const TSwitchInfo &tSwitchInfo);
	BOOL FindUsableVMP();
	u8   GetVCSAutoVmpMember(u8 byVmpId, TMt* ptVmpMember);
	void SetVCSVmpMemInChanl(TMt* pVmpMember, u8 byVmpSytle);
    void ChangeSpecMixMember(TMt* ptMixMember, u8 byMemberNum);
	void RestoreVCConf(u8 byNewMode);
	void ChangeVFormat(TMt tMt, 
					   u8 byStart = FALSE,
					   u8 byNewFormat = VIDEO_FORMAT_INVALID,
					   u8 byEqpId = 0,
					   u8 byVmpStyle = ~0,
					   u8 byPos = ~0,
					   u32 dwResW = 0,
					   u32 dwResH = 0,
					   u8 byHduChnId = 0);
	void VCSMTAbilityNotif(TMt tMt, u8 byMediaType);
	void VCSChairOffLineProc();
    void VCSConfStatusNotif();
	BOOL32 VCSChangeChairMan( void );

	TMt  VCSGetNextPollMt();
	BOOL32 SwitchToAutoMixing();

	//zjj 20090911调度下级终端时要把同级的其它终端剔出电视墙
	void FindSmcuMtInTvWallAndStop( const TMt& tNewCurVCMT, TMt *const ptDropMt, BOOL32 bIsStopSwitch = TRUE );

	//zjj 20091026
	void ProcVcsMcuMtInTwMsg( const CMessage *pMsg );
	
	//zjj20091102
	void ProcMcsVcsMcuAddPlanName( const CMessage *pMsg );
	void ProcMcsVcsMcuDelPlanName( const CMessage *pMsg );
	void ProcMcsVcsMcuModifyPlanName( const CMessage *pMsg );
	//void ProcVcsMcuSavePlanData( const CMessage *pMsg );
	void ProcMcsVcsMcuGetAllPlanData( const CMessage *pMsg );
	void ProcVcsMcuChgTvWallMgrMode( const CMessage *pMsg );
	void ProcVcsMcuClearOneTvWallChannel( const CMessage *pMsg );
	void ProcVcsMcuClearAllTvWallChannel( void );
	void ProcVcsMcuTvWallMtStartMixing( const CMessage *pMsg );
	void ProcVcsMcuMtInTvWallInReviewMode( CServMsg& cServMsg );
	void ProcVcsMcuMtInTvWallInManuMode(CServMsg& cServMsg);
	void SetSingleMtInTW(TSwitchInfo *ptSwitchInfo, CServMsg &cServMsg);
	void SetMtInTvWallAndHduInFailPresetinAndInReviewMode( TPreSetInRsp &tPreSetInRsp );
	//void ProcVcsMcuLockPlan( const CMessage *pMsg );
	//void ProcVcsMcuUnLockPlanByVcsssnInst( const CMessage *pMsg );

	void ProcVcsMcuChangeDualStatusReq( const CMessage * pcMsg );
	void ProcVcsMcuMtJoinConfRsp( const CMessage * pcMsg );
    //[5/4/2011 zhushengze]VCS控制发言人发双流
    void ProcMcsMcuChangeMtSecVidSendCmd( const CMessage * pcMsg );
    //[5/19/2011 zhushengze]组呼会议锁定状态控制
    void ProcVcsMcuGroupModeConfLockCmd( const CMessage * pcMsg );

	//[2/23/2012 zhushengze]界面、终端消息透传
    void ProcMcsMcuTransparentMsgNotify( const CMessage * pcMsg );
	
	void ShowCfgExtraData();
	void NotifyAllPlanNameToConsole( BOOL32 bNotifyVCS = TRUE);
	void NotifyOnePlanDataToConsole( s8* pbyPlanName,u16 wEvent,BOOL32 bNotiyVCS = TRUE );
	void VCSClearAllTvWallChannel( const TSwitchInfo *ptSwitchInfo = NULL, const u8 byNum = 0);
	void VCSClearTvWallChannelByMt( TMt tMt,BOOL32 bOnlyClearTvWallStatus = FALSE );
	void VCSChangeTvWallMtMixStatus( BOOL32 bIsStartMixing );
	BOOL32 VCSMtNotInTvWallCanStopMixing( TMt tMt );
	BOOL32 VCSMtInTvWallCanStopMixing( TMt tMt );
	void VCSFindMtInTvWallAndChangeSwitch( TMt tMt,BOOL32 bIsStopSwitch,u8 byMode );
	void MCSFindMtInTvWallAndChangeSwitch( TMt tMt,BOOL32 bIsStopSwitch,u8 byMode );
	void VCSClearMtDisconnectReason( TMt tMt );
	void ClearHduSchemInfo();

	void ProcVcsMcuStartMonReq( const CMessage *pcMsg );// [8/17/2010 xliang] 监控联动
	void ProcVcsMcuStopMonReq( const CMessage *pcMsg );
	void ProcVcsMcuStopAllMonCmd( const CMessage *pcMsg);
	// [chendaiwei 2010/09/11] VCS 根据本级调度席模板配置，判断下级调度席所需要的最大带宽值
	u32 GetVscSMCUMaxBW();


	//8、多回传相关
	//lukunpeng 2010/06/10 根据各event和SetInRsp判断增加源的目的数和此消息可以释放多少带宽
	void EvaluateSpyFromEvent(const TPreSetInRsp &tPreSetInRsp, u8 &byAddDestSpyNum, u32 &dwCanReleaseBW, s16 &swCanRelIndex);
	
	//zhouyiliang 20100723 针对vcs自动画面合成上传带宽已满处理
	void HandleVCSAutoVmpSpyRepalce( TPreSetInRsp &tPreSetInRsp );
	//zhouyiliang 20100809 查找vcs可替换的多回传终端
	TMt FindVCSCanReplaceSpyMt( u8 byMtInEqpType , const TMt& tSrc ,s16& swCanReplaceSpyIndex, u8& byCanReplaceChnnPos );
	//lukunpeng 2010/06/07 判断上下级是否同时支持多回传
	BOOL32 IsLocalAndSMcuSupMultSpy( u16 wMcuIdx );

	BOOL32 IsMMcuSupportMultiCascade( void );
	BOOL32 IsPreSetInRequired (const TMt &tMt);	 // 是否需要preSetIn
	BOOL32 OnMMcuPreSetIn(TPreSetInReq &tPreSetInReq);
	BOOL32 IsSupportMultCas( u16 wMcuIdx );

	void ProcMcuMcuSpyBWFull(const CMessage *pcMsg,TPreSetInRsp *ptPreSetInRsp,u16 wErrorCode = ERR_MCU_CONFSNDBANDWIDTHISFULL);
	BOOL32 JudgeMcuMcuPreSetIn(const TPreSetInReq& tPreSetInReq, const TMsgHeadMsg& tHeadMsg, TPreSetInRsp& tPreSetInRsp, u16 &wErrorCode);
	void ProcMcuMcuPreSetInReq( const CMessage *pcMsg );
	void ProcMcuMcuPreSetInAck( const CMessage *pcMsg );
	void ProcMcuMcuPreSetInNack( const CMessage *pcMsg );
	//void SendMMcuSpyNotify( const TPreSetInRsp &tPreSetInRsp );
	void SendMMcuSpyNotify( const TMt &tSpySrc, u32 dwEvId, TSimCapSet tDstCap/*, u8 byRes = VIDEO_FORMAT_INVALID*/ );
	void ProcUnfinishedCascadEvent( const TPreSetInRsp &tPreSetInRsp );
	void SendMMcuRejectSpyNotify( const TMt &tSrcMt, u32 dwSpyBW );
	BOOL32 CheckMtResAdjust( const TMt &tMt, u8 byReqRes, u16 &wErrorCode, u8 &byRealRes, u8 byStart = TRUE, u32 dwResW = 0, u32 dwResH = 0);
	//  [12/2/2009 pengjie] 得到混音成员的数目
	u8   GetCurMixerNum( void );  
	void ProcSelMtStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp );
	void ProcStartMonitorPreSetinAck( const TPreSetInRsp &tPreSetInRsp );
	void ProcVmpStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp);
	void ProcSpeakerStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp );
	// 处理级联混音未处理完的逻辑
	void ProcHduStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp );
	void ProcPollStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp );
	void ProcTWStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp );
	void ProcRollCallPreSetInAck( const TPreSetInRsp &tPreSetInRsp );
	void ProcRecStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp );
	void ProcDragStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp );
	u8 GetMixChnPos(const TMt &tSrc, BOOL32 bRemove = FALSE );
	BOOL32 IsMtInMixChn(const TMt &tMixMem);
	//void FreeSpyChannlInfoByMcuId(u8 byMcuId);
	BOOL32 FreeRecvSpy( const TMt tMt, u8 bySpyMode, BOOL32 bForce = FALSE);    // 释放本级接纳的回传终端所占有的回传资源
	void FreeAllRecvSpyByMcuIdx( u16 wMcuIdx );
	void OnMMcuBanishSpyCmd(const TMt &tSrcMt, u8 bySpyMode, u8 bySpyNoUse = MODE_BOTH );
	void ProcMcuMcuMultSpyCapNotif( const CMessage * pcMsg );
	void ProcMcuMcuSpyNotify( const CMessage *pcMsg );
	void ProcMcuMcuRejectSpyNotify( const CMessage *pcMsg );
	void ProcMcuMcuBanishSpyCmd( const CMessage *pcMsg );
	//[nizhijun 2010/12/15] 级联多回传丢包重传RTCP相关接口
	void  ProcMcuMcuMtExtInfoNotif(const CMessage *pcMsg);//上级收到下级发送的RTCP信息告知后做处理
	void  OnMMcuRtcpInfoNotif(TMt tSrc, const TSimCapSet &tDstCap,TMsgHeadMsg tHeadMsg);//下级处理组织相关RTCP信息
	void  GetSpyBasRtcpInfo(TEqp &tBas, u8 &byChnId, u8 &byFrontOutNum, u8 &byOutIdx, TTransportAddr &tRtcpAddr);
	void  GetSpyCascadeRtcpInfo(TMt tSrc, TTransportAddr &tVidRtcpAddr, TTransportAddr &tAudRtcpAddr);
	void  SendMMcuMtExtInfoNotif(TMt tSpySrc, TMsgHeadMsg tHeadMsg, u8 bySpyMode, TTransportAddr &tVidRtcpAddr, TTransportAddr &tAudRtcpAddr);
	//[nizhijun 2010/12/15] ends
//	void ProcMMcuBanishedSpyNotify( const CMessage *pcMsg );
	BOOL32 StopSpyMtSpySwitch( TMt tSrc,u8 byMode = MODE_BOTH, u8 bySpyNoUse = MODE_BOTH/*,BOOL32 bIsNotifyMMcu = FALSE */);
	BOOL32 StopAllLocalSpySwitch();
	void StopSpeakerFollowSwitch( u8 byMode );
	BOOL32 StartSwitchToMcuByMultiSpy( TMt tSrc, u16 wSrcChnnl, u8 byDstMtId, const TSimCapSet &tDstCap, u16 &wErrorCode,
		u8 byMode = MODE_BOTH, u8 bySwitchMode = SWITCH_MODE_BROADCAST, BOOL32 bMsgStatus = TRUE);
	BOOL32 IsSupportMultiSpy( void );
	void RemoveMcu( const TMt &tMt );
	BOOL32 IsMcu( const TMt& tMt,u16 *pwMcuIdx = NULL );
	//zhouyiliang 20100826 判断两个mt的从属关系
	BOOL32 IsMtInMcu(const TMt& tMcu, const TMt& tMt, const BOOL32 bDirectlyUnder = FALSE);
	
	// [10/12/2011 liuxu] 判断终端tmt是否是tmcu的上传通道成员
	BOOL32 IsSmcuViewMtInMcu( const TMt&, const TMcu& );
	// 获取下级mcu的上传通道成员
	TMt GetSMcuViewMt( const TMcu&, const BOOL32 bDeepest = FALSE );
	// yanghuaizhi 获得终端直连mcu
	TMt GetDirectMcuFromMt( const TMt &tMt );
	// [pengjie 2010/4/2] 级联多回传请求关键帧
	void ProcMcuMcuSpyFastUpdateCmd( const CMessage *pcMsg );
	void SendMcuMcuSpyFastUpdateCmd( const TMt &tMt, u8 byMode = MODE_VIDEO );

	//lukunpeng 2010/06/10 级联多回传
	//BOOL32 FreeSpySrcBW( TMt &tMt, u32 dwSpyBW ); // 指定释放某个终端的部分带宽

	//BOOL32 FreeSpyChannlInfo( const TMt &tMt, u8 bySpyMode = MODE_BOTH, BOOL32 bForce = FALSE ); // 根据终端信息释放一个回传通道
	BOOL32 GetMcuMultiSpyBW( u16 wMcuIdx,u32 &dwMaxSpyBW, s32 &nRemainSpyBW );
	BOOL32 SetMcuSupMultSpyRemainBW( u16 wMcuIdx, s32 nRemainSpyBW );
	void   SetRealSndSpyBandWidth( u32 dwBandWidth );
	u32    GetRealSndSpyBandWidth( void );

	TSimCapSet GetMtSimCapSetByMode( u8 byMtId );
	BOOL32 GetMinSpyDstCapSet(const TMt &tMt , TSimCapSet& tReqDstCap);
	void UpdateCurSpyDstCapSet(const TMt &tMt );

	void OnPresetinFailed( const TMt &tMt );
	//zhouyiliang 20101110 判断是否双速双格式且主辅格式为h264或者MP4（主辅格式不同，只有一个是h264或者MP4）
	BOOL32 IsConfDbCapDbBitrateCapH264OrMp4();
	void  ProcMcuMcuSwitchToMcuFailNotif(const CMessage *pcMsg);//上级收到下级发送的RTCP信息告知后做处理
	//广播需要过适配的终端寻找合适的bas输出
	BOOL32 FindBasChn2BrdForMtExt(const TMt &tSrc, u8 byMtId, u8 byMediaMode,TBasOutInfo &tOutInfo);
	BOOL32 IsBasChnForRecAdapt(const TEqp& tBas,u8 byChnId);

	TVmpModule GetVmpModule( void );
	TVMPParam_25Mem GetVmpParam25MemFromModule( void );

private:
	void VCSTransOutMsgToInnerMsg(CServMsg& cOutMsg);	
	

	
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
    void procWaitOpenDSChanTimeout( void );
	void ProcPolyMCUH239Rsp( const CMessage * pcMsg );
	void ProcMtMcuStopSwitchMtReq( const CMessage * pcMsg );
	void ProcMtMcuStartSwitchMtReq( const CMessage * pcMsg );
	void ProcStartSelSwitchMt(CServMsg &cServMsg);
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
	//void ProcMcuMtFastUpdatePic( const CMessage * pcMsg );
	void ProcMtMcuMsdRsp( const CMessage * pcMsg );
	void ProcMtMcuMatrixMsg( const CMessage * pcMsg );
	void ProcMtMcuOtherMsg( const CMessage * pcMsg );
	void ProcMtMcuFastUpdatePic( const CMessage * pcMsg );
	void ProcMtMcuMixReq( const CMessage * pcMsg );
	void ProcMtMcuReleaseMtReq( const CMessage * pcMsg );
	void ProcMtMcuApplyCancelSpeakerReq( const CMessage * pcMsg );
	//  [4/1/2011 zhushengze] MCS MTC-BOX
	void ProcVcsApplyCancelSpeakerReq( const CMessage * pcMsg );
	void ProcMcsApplyCancelSpeakerReq( const CMessage * pcMsg );
	void ProcTimerInviteUnjoinedMt( const CMessage * pcMsg );
	void ProcMtAdpMcuMtListRsp( const CMessage * pcMsg ); //  [pengguofeng 7/1/2013]
	void ProcMtAdpMcuSmcuMtAliasRsp( const CMessage * pcMsg ); //  [yanghuaizhi 8/8/2013]
	void PackAndSendMtList2Mcs(u8 &byMtNum, TMtExtU *ptMtExtU, const u16 &wMcuIdx); //

    void ProcMtMcuTransParentMsgNotify( const CMessage * pcMsg );

	void ProcMcsMcuGetMtVidAliaseReq( const CMessage * pcMsg );
	void ProcMtMcuVidAliasNtf( const CMessage * pcMsg );

	u8   AddMt( TMtAlias &tMtAlias, u16 wMtDialRate = 0, u8 byCallMode = CONF_CALLMODE_TIMER, BOOL bPassiveAdd = FALSE );
	void AddJoinedMt( TMt & tMt );
	void RemoveMt( TMt & tMt, BOOL32 bMsg,BOOL32 bIsSendMsgToMcs = TRUE );
	void RemoveJoinedMt(TMt & tMt, BOOL32 bMsg, u8 byReason = MTLEFT_REASON_NORMAL,BOOL32 bNeglectMtType = FALSE);
	BOOL32 DealMtMcuEnterPwdRsp(TMt tMt, s8 *pszPwd, s32 nPwdLen);
    void ProcMtMcuBandwidthNotif(const CMessage *pcMsg);
	void ProcMMcuBandwidthNotify( const TMt& tMMcu,const u32 dwBandWidth );
	void RestoreVidSrcBitrate(TMt tMt, u8 byMediaType);
	void ProcMcsMcuSetMtVolumeCmd(const CMessage * pcMsg);
    void MtVideoSourceSwitched(CServMsg & cServMsg);
    void ProcMtMcuVideoSourceSwitched(const CMessage * pcMsg);
    void ChangeMemberVolume( const TMt & tMember, u8 byVolume );
    void SetMemberVolumeReq( const TMt & tMember, u8 byVolume );
	BOOL32 SetMtInfoByMtExtU(const u16 &wMcuIdx, const TMtExtU &tMtExtU, TMcuMcuMtInfo &tMtInfo); //根据MtExt来设置tMtInfo
	
    BOOL32 AssignMpForMt( TMt & tMt );
    void ReleaseMtRes( u8 byMtId );	
    void InviteUnjoinedMt( const CServMsg& cServMsgHdr, const TMt* ptMt = NULL, 
		                   BOOL32 bSendAlert = FALSE, BOOL32 bBrdSend = FALSE,
						   u8 byCallType = VCS_FORCECALL_REQ, BOOL bLowLevelMcuCalledIn = FALSE);

    void ProcMtCapSet( TMt tMt, TMultiCapSupport &tCapSupport, u8 bMtHasVidCap = 1);	

	BOOL32 IsMtCanCapabilityCheat( s8 *pachProductID );
	//zjl[20091208]移除会议模板中电视墙成员
	void RemoveMtInMutiTvWallMoudle(TMt tMt);
	// [2013/08/13 chenbing] 清除指定通道的成员 
	BOOL32 ClearHduChnnlInHduMoudle(u8 byHduId, u8 byHduChnId, u8 byHduSubChnId = 0, u8 byHduMode = HDUCHN_MODE_ONE);

	//lukunpeng 2010/07/23 终端申请发言/抢答代码合并
	void ProcMcsMcuChangeSpeakModeReq(const CMessage * pcMsg);
	void ProcMcsMcuGetSpeakListReq(const CMessage * pcMsg);

	// [pengjie 2010/4/16] 在删除、挂断、终端掉线等情况下清除其在外设中的信息并调整外设参数拆除相应交换
	void RemoveMtFormPeriInfo( const TMt &tMt, u8 byReason );
	// [yanghuaizhi 2012/7/12] 删除不在线终端时,清除外设中信息
	void RemoveUnjoinedMtFormPeriInfo( const TMt &tMt);
	// 为8KH，8KE构造双流能力集 [4/14/2012 chendaiwei]
	void ConstructDSCapExFor8KH8KE(const TCapSupport & tCapsupport, TVideoStreamCap *pTDSCapEx);

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
    void ProcMcuMcuAutoSwitchTimer( void );
	
	void CascadeAdjMtRes( TMt tMt,
						  u8 bNewFormat,
						  BOOL32 bStart = TRUE,
						  u8 byVmpStyle = ~0,
						  u8 byPos = ~0,
						  u32 dwResW = 0,
						  u32 dwResH = 0,
						  u8 byEqpId = 0,
						  u8 byHduChnId = 0);

	void CascadeAdjMtBitrate( TMt tMt, u16 wBitRate,BOOL32 bIsRecover = FALSE );
	// [10/18/2011 liuxu] 恢复终端的分辨率, Tvwall和Vmp通用
	void RecoverMtVFormat( const TMt&, const BOOL32 bRecoverSMcuViewMt = FALSE );
	// 降低电视墙中终端的分辨率
	void DecreaseMtVFormatInTw( const TMt& );

	void ProcMcuMcuAdjustMtResReq ( const CMessage *pcMsg );	
	void ProcMcuMcuAdjustMtResAck ( const CMessage *pcMsg );	
	void ProcMcuMcuAdjustMtResNack( const CMessage *pcMsg );

    void ProcMcuMcuCancelMeSpeakerRsp( const CMessage *pcMsg );
	void ProcMcuMcuSpeakStatusNtf( const CMessage *pcMsg );
    void ProcMcuMcuApplySpeakerRsp( const CMessage *pcMsg );
	void ProcMcuMcuMuteDumbReq( const CMessage *pcMsg );


	void ProcMcuMcuAdjustMtBitRateCmd( const CMessage *pcMsg );


	void ProcMcuMcuMtOperateRsp( const CMessage *pcMsg );
	// [pengjie 2010/4/23] 普通终端调帧率(本级)
	void SendChangeMtFps( u8 byMtId, u8 byChnnlType, u8 byFps );
	// [pengjie 2010/4/23] 级联调帧率
	void CascadeAdjMtFps( TMt tMt, BOOL32 bStart, u8 byChnnlType, u8 byFps = 0 );
	// [pengjie 2010/9/13] 普通终端调分辨率
	BOOL32 SendChgMtVidFormat( u8 byMtId, u8 byChlType, u8 byFormat, BOOL32 bMmcuCmd = FALSE, BOOL32 bStart = TRUE, u32 wResW = 0, u32 wResH = 0);
	// End

	void SendChgMtFps(const TMt& tMt, u8 byChnnlType, u8 byFps,BOOL32 bStart = TRUE );
	void ProcMcuMcuAdjustMtFpsReq ( const CMessage *pcMsg );
	void ProcMcuMcuAdjustMtFpsAck ( const CMessage *pcMsg );	
	void ProcMcuMcuAdjustMtFpsNack( const CMessage *pcMsg );
	void SendSMcuMtInfoToMMcu( u16 wMcuIdx );
	void SendAllSMcuMtInfoToMMcu( void );
	void SendSMcuMtStatusToMMcu( u16 wMcuIdx );
	void SendAllSMcuMtStatusToMMcu( void );
	void SendSMcuUpLoadMtToMMcu(void);
	// End
	void OnStartMixToSMcu(CServMsg *pcSerMsg);
	void OnStopMixToSMcu(CServMsg *pcSerMsg);
	void OnGetMixParamToSMcu(CServMsg *pcSerMsg);
	void OnAddRemoveMixToSMcu(CServMsg *pcSerMsg, BOOL32 bAdd,BOOL32 bStopMixerNoMember = TRUE);

	void OnStartDiscussToAllSMcu(CServMsg *pcSerMsg);
	void OnStopDiscussToAllSMcu(CServMsg *pcSerMsg);

	void OnGetMtStatusCmdToSMcu( u8 byDstMcuId );
	void OnNtfMtStatusToMMcu( u8 byDstMcuId, u8 byMtId = 0 );
	void OnNtfDsMtStatusToMMcu(void);
	TMt  GetConfRealDsMt(void);
	// [11/15/2011 liuxu] 采取缓冲发送本地终端状态
	void OnBufSendMtStatusToMMcu( );

	void NotifyMcuNewMt(TMt tMt);
	void NotifyMcuDelMt(TMt tMt);
	void NotifyMcuDropMt(TMt tMt);
	
	void ProcMcuMcuSendMsgReq( const CMessage * pcMsg );

	// [10/19/2011 liuxu] 获取mcu的Mc信息
	TConfMcInfo *GetMcuMcInfo( const TMt& );
	BOOL32 GetMcData( u8 byMcuId, TConfMtInfo &tConfMtInfo, TConfMcInfo &tConfOtherMcInfo );

	//TMt  GetLocalMtFromOtherMcuMt( TMt tMt );
	
	TMt  GetMcuMediaSrc(u16 wMcuIdx);
	
	void OnMMcuSetIn(TMt tMt, u8 byMcsSsnId, u8 bySwitchMode);
    void OnSetOutView(const TMt &tMcuSetInMt, u8 byMode = MODE_BOTH);

	BOOL32 IsCanSetOutView( TMt &tSetInMt, u8 byMode = MODE_BOTH );
    // guzh [5/14/2007] 获取会议音视频信息
    void GetConfViewInfo(u16 wMcuIdx, TCConfViewInfo &tInfo);
    void GetLocalAudioInfo(TCConfAudioInfo &tInfo,TMsgHeadMsg &tHeadMsg);    
	   
    // guzh [5/14/2007] 通知各个级联MCU本级媒体源
    void NotifyAllSMcuMediaInfo(u8 byTargetMcuId, u8 byMode);
    void NofityMMcuMediaInfo();

	BOOL32 IsRosterRecved( const u16 wMcuIdx );

    //尝试多次打开级联通道
//     void ProcTimerReopenCascadeChannel();

    //mcu source
    void SendMcuMediaSrcNotify(BOOL32 bFource = FALSE);
    void ProcTimerMcuSrcCheck(const CMessage * pcMsg);
    
    void ProcMcuMtFastUpdateTimer( const CMessage * pcMsg );
    void ProcMcuEqpFastUpdateTimer( const CMessage * pcMsg );
    void ProcMcuMtSecVideoFastUpdateTimer( const CMessage * pcMsg );
    
    void ProcTimerChangeVmpParam(const CMessage * pcMsg);
    //[5/4/2011 zhushengze]VCS控制发言人发双流
    void ProcMcuMcuChangeMtSecVidSendCmd(const CMessage * pcMsg);
	
	//[2/23/2012 zhushengze]界面、终端消息透传
    void ProcMcuMcuTransparentMsgNotify(const CMessage * pcMsg);

    //3、双流相关
    void StopDoubleStream( BOOL32 bMsgSrcMt, BOOL32 bForceClose,BOOL32 bIsNeedAdjustVmpParam = TRUE);
    void StartDoubleStream(TMt &tMt, TLogicalChannel &tLogicChnnl);
    void StartSwitchDStreamToFirstLChannel( TMt &tDstMt );
    void StopSwitchDStreamToFirstLChannel( TMt &tDstMt );
    BOOL32 JudgeIfSwitchFirstLChannel( TMt &tSrc, u8 bySrcChnnl, TMt &tDstMt );
    void ClearH239TokenOwnerInfo(TMt *ptMt);
    void UpdateH239TokenOwnerInfo(TMt &tMt);
    void NotifyH239TokenOwnerInfo(TMt *ptMt);
    void McuMtOpenDoubleStreamChnnl(TMt &tMt, const TLogicalChannel &tLogicChnnl/*, const TCapSupport &tCapSupport*/);//打开双流通道
    void ProcSendFlowctrlToDSMtTimeout(void );
    void ProcSmcuOpenDVideoChnnlTimer(const CMessage * pcMsg);
	BOOL32 IsDStreamMtSendingVideo(TMt tMt);
	BOOL32	CheckDualMode(TMt &tMt);
    void  ShowToken();
    //4、选看相关
    //void RestoreMtSelectStatus( u8 byMtId, u8 bySelMode );       
    BOOL ChangeSelectSrc(TMt tSrcMt, TMt tDstMt, u8 byMode = MODE_BOTH);
    void StopSelectSrc(TMt tDstMt, u8 byMode = MODE_BOTH, u8 byIsRestore = TRUE,u8 byIsStopSwitch = TRUE );
	void AdjustFitstSrcMtBit(u8 byMtId,  BOOL32 bIsFirstBrdAdp, BOOL32 bIsCancelSel = FALSE);
    //选看模式和选看源、目的的能力级是否匹配
    BOOL32 IsSelModeAndCapMatched( u8               &bySwitchMode,
                                   const TSimCapSet &tSimCapSrc,
                                   const TSimCapSet &tSimCapDst,
                                   BOOL32           &bAccord2Adp);
    void ProcMtSelectdByOtherMt( u8 byMtId, BOOL32 bStartSwitch  );    

    //5、其他
    void AdjustMtVideoSrcBR( u8 byMtId, u16 wBitrate, u8 byMode = MODE_VIDEO, BOOL32 bForceFlowCtrlSrc = FALSE);
    BOOL32 IsNeedAdjustMtSndBitrate(u8 byMtId, u16 &wSndBitrate,u8 byMediaMode = MODE_VIDEO);
    //BOOL32 IsMtMatchedSrc( u8 byMtId, u8 &byChnRes, BOOL32 &bAccord2MainCap);
	void	SwitchNewVmpToSingleMt(TMt tMt);
	BOOL32 GetMtMatchedRes(u8 byMtId, u8 byChnType, u8& byChnRes);
	BOOL32 GetMtLegalCapExAccord2CallBR(u16 wCallBR,TVideoStreamCap tSrcCap,TVideoStreamCap &tMatchedCap);
	// [pengjie 2010/4/23] 判断是否是需要调整帧率的终端（如7820/7920），因为这些终端帧率可能达到50/60帧，
	//                     而有些地方是不支持这么高的帧率的（如画面合成前适配）
    BOOL32 IsNeedChangeFpsMt( u8 byMtId );
	BOOL32 IsNeedAdjustSpyFps(const TMt &tSrc, const TSimCapSet &tDstCap);
	//lukunpeng 2010/07/09 恢复下级终端的画面合成
	void RestoreSubMtInVmp(TMt& tMt);

	void RestoreMtInTvWallOrHdu( const TMt& tMt );

    //通知转发板向外网终端打洞
    void Starth460PinHoleNotify( const u32 dwLocalAddr,
                                 const u16 wLocalPort,
                                 const u32 dwRemoteAddr,
                                 const u16 wRemotePort,
                                 const u8 byPinHoleType,
                                 const u8 byPayLoad);

    //通知转发板停止向外网终端打洞
    void StopH460PinHoleNotifyByMtId( const u8 byMtId);
    void StopH460PinHole( const u8 byMpId, 
                          const u16 wLocalPort, 
                          const u32 dwRemoteAddr,
                          const u16 wRemotePort);

public:
    //TMt  GetVidBrdSrc(void);
	u16 GetLstRcvMediaResMtBr(BOOL32 bPrimary, u8 byMediaType, u8 byRes, u8 bySrcMtId = 0);
    //songkun,20110530,VMP呼叫低速没有降速，挂断低速没有升速
	u16 GetMinMtRcvBitByVmpChn(u8 byVmpId, BOOL32 bPrimary, u8 byVmpOutChnnl,u8 bySrcMtId = 0);

	BOOL32	GetSrcRealAbility(TMt &tSrc, u8 byMediaMode, TBasAdaptParamExt &tBasParamExt)
	{
		return GetBasParamExt(tSrc,byMediaMode,tBasParamExt);
	}
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
    void StartPlayReq(CServMsg & cServMsg,u8 byNackTryNum =0);
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
	void ProcRecPlayWaitMpAckTimer( void );
    
	//new bas
	//适配器响应接口统一处理
    void ProcBasMcuRsp( const CMessage * pcMsg );
	//广播适配响应处理
	void ProBasMcuBrdAdpRsp(CServMsg &cServMsg);
	//选看适配响应处理
	void ProBasMcuSelAdpRsp(CServMsg &cServMsg);

    void ProcMcuBasDisconnectedNotif( const CMessage * pcMsg );
    //void ProcMcuBasConnectedNotif( const CMessage * pcMsg );
    void ProcBasMcuCommand(const CMessage * pcMsg);
    void ProcBasTestCommand( const CMessage * pcMsg );

	//[2011/08/27/zhangli]8kebas状态通知处理
	void ProcBasStatusNotif( const CMessage * pcMsg );
	void ProcBasChnnlStatusNotif( const CMessage * pcMsg );
    void ProcHdBasChnnlStatusNotif( const CMessage * pcMsg );
	void ProcAudBasChnnlStatusNotif( const CMessage * pcMsg );
	void ProcHduMcuChnnlStatusNotif( void );

    void ProcVmpMcuRsp( const CMessage * pcMsg ); 
    void ProcVmpMcuNotif( const CMessage * pcMsg ); 
    void ProcVmpRspWaitTimer( const CMessage * pcMsg );
	void ProcVMPStopSucRsp( u8 byVmpId );
    void ProcWaitAllVMPPrestInAckTimer( const CMessage * pcMsg );
	void ProcUnfinishedChangeVmpCmd( u8 byVmpId );
    
    void ProcVmpTwMcuRsp(const CMessage * pcMsg);
    void ProcVmpTwMcuNotif(const CMessage * pcMsg);
    
    void ProcPrsConnectedNotif( const CMessage * pcMsg );
    void ProcPrsDisconnectedNotif( const CMessage * pcMsg );
    void ProcPrsMcuRsp( const CMessage * pcMsg );
	void ShowPrsInfo();

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
	void ProcMcsMcuChangeHduVmpMode( const CMessage * pcMsg );		// 切换HDU通道模式
    void ProcMcsMcuChangeHduVolume( const CMessage * pcMsg );       // 设置hdu音量
	void ProcHduConnectedNotif(const CMessage * pcMsg);             // hdu连接通知
    void ProcHduDisconnectedNotif( const CMessage * pcMsg );        // 断链通知
	//void ProcHduMcuStatusNotif( const CMessage * pcMsg );           // 状态通知
    void ProcHduMcuNeedIFrameCmd( const CMessage * pcMsg );         // hdu请求关键桢
	void StartSwitchHduReq(CServMsg & cServMsg);
	void ProcHduMcuRsp( const CMessage * pcMsg );


    //2、部分混音器管理
	//tianzhiyong 2010/03/21 增加开启混音模式参数，EAPU下感知混音模式在EAPU下混音与语音激励可单独开启，
	//以及手动开启混音器参数 byEqpId 是手动还是自动开启混音器，0 ： 自动 非0 ：混音器EQPID 默认自动
	BOOL32 StartMixing(u8 byMixMode , u8 byEqpId = 0);
	void StopMixing();
	//tianzhiyong 2010/03/21 根据EQPID判断该外设是否支持当前会议音频类型
	BOOL32 CheckMixerIsValide(u8 byMixerId);
	//tianzhiyong 2010/03/21 增加获取混音器全混通道接口，EAPU下各个混音器全混通道随各个混音器混音成员数变化而变化
	u8  GetMixerNModeChn();
	BOOL32 StartEmixerVac( u8 byEmixerId );
	void ProcMixStopSucRsp( BOOL32 bIsSendToMMcuNotify = TRUE ); // 混音器停止成功后mcu对应的业务处理
	BOOL32 AddMixMember( const TMt *ptMt, u8 byVolume = DEFAULT_MIXER_VOLUME, BOOL32 bForce = FALSE );
	BOOL32 AddMixMember( const u8 byMtNum, const TMt *ptMt, u8 byVolume = DEFAULT_MIXER_VOLUME, BOOL32 bForce = FALSE );
	void RemoveMixMember( const TMt *ptMt, BOOL32 bForce = FALSE );
	void AddRemoveMixRtcpMember(TMt tMt, BOOL32 IsAdd = FALSE);
	void AddRemoveMixRtcpMember(u8 byDstMtNum, TMt *ptDstMt, BOOL32 IsAdd = FALSE);
	void SetMemberVolume( const TMt *ptMt, u8 byVolume );
	BOOL32 NotifyMixerSendToMt( BOOL32 bSend = TRUE );
	void SetMixerSensitivity( u8 bySensVal );
    void SetMixDelayTime(u16 wDelayTime);
    void SwitchMixMember(const TMt *ptMt, BOOL32 bNMode = FALSE);
	void SwitchMixMember( BOOL32 bNMode );
    void VACChange(const TMixParam &tMixParam, u8 byExciteChn);
	void UpdateUploadMtMixingToMcs(TMt tLocalSmcuMt,CServMsg &cServMsg);
	BOOL32 IsNeedCheapMcsMtMixing(TMt tUnLocalMt);
    
	BOOL32 AddSpecMixMember(const TMt *ptMt = NULL, u8 byMtNum = 0, BOOL32 bAutoInSpec = TRUE);

	BOOL32 RemoveSpecMixMember(const TMt *ptMt = NULL, u8 byMtNum = 0, BOOL32 bCancelForceMix = FALSE, BOOL32 bStopMixNonMem = TRUE);

	//此函数将不再被外部调用，由上面两个函数AddSpecMixMember 和RemoveSpecMixMember代替
	BOOL32 AddRemoveSpecMixMember(const TMt *ptMt, u8 byMtNum, BOOL32 bAdd, BOOL32 bStopMixNonMem = TRUE, BOOL32 bAutoInSpec = TRUE, BOOL32 bCancelForceMix = FALSE, BOOL32 bIsReplace = FALSE);
    
	void SwitchDiscuss2SpecMix();

    void MixerStatusChange(void); 
    u8   GetMixMtNumInGrp(void);
    u8   GetMaxMixNum(u8 byMixerId);    //获取最大混音成员个数（目前：分散会议4个，其他56个）
//    u8   GetMixChnPos( u8 byMtId, BOOL32 bRemove = FALSE ); //获取混音成员通道位置，用于区分交换端口号
	
	//  [12/1/2009 pengjie] 多回传支持，记录的是一个未被local的tmt信息
	//u8   GetMixChnPos( TMt &tSrc, BOOL32 bRemove = FALSE );
	 
	//  [12/3/2009 pengjie] 释放某个终端所占用的混音通道，bForce = TRUE 释放所有混音通道，默认为FALSE
	//BOOL32 FreeMixChn( TMt &tSrc, BOOL32 bForce = FALSE );
	//  [12/4/2009 pengjie] 释放某个多回传终端占用的混音资源
	//BOOL32 FreeSpyMixerMember( TMt &tSrc );

    u8   GetMaxMixerDepth( u8 byMixerId ) const;
	void ChairmanAutoSpecMix();     //申请主席自动开启定制混音
//    void ChangeConfRestoreMixParam();
	BOOL32 RestoreMixingByOtherMixer( u8 byMixMode );
    u16  GetCurConfMixMtNum(u16 wMcuIdx = INVALID_MCUIDX);
	void ClearSMcuAllMtMixStatusToAuto(u16 wMcuIdx = INVALID_MCUIDX);
	void GetMtListGroupBySmcu(TMt *ptMtIn, u8 byMtNum, u8 &bySmcuNum, u8 *pbyMtNum = NULL, TMt *ptMtOut = NULL);
	
	/*---------------------------适配相关-----------------------*/
	//是否需要适配
	BOOL32 IsNeedAdapt(TMt tSrc, TMt tDst, u8 byMediaMode);
	//是否需要选看适配
	BOOL32 IsNeedSelAdpt(TMt tSrc, TMt tDst, u8 byMediaMode);
	//是否需要回传适配
	BOOL32 IsNeedSpyAdpt(const TMt &tSrc, const TSimCapSet &tDstCap, u8 byMediaMode);
	//开启广播适配
	BOOL32 StartBrdAdapt(u8 byMediaMode);
	//停止广播适配
	BOOL32 StopBrdAdapt(u8 byMediaMode,BOOL32 bSelSelf = TRUE);
	//开启源到目的双流适配
	//BOOL32 StartDSAdapt(TMt tSrc, TMt tDst);
	//停止源到目的双流适配
	//BOOL32 StopDSAdapt(TMt tSrc, TMt tDst);
	//开启选看适配
	BOOL32 StartSelAdapt(TMt tSrc, TMt tDst, u8 byMediaMode, BOOL32 bInnerSelect = TRUE);
	//开启回传适配
	BOOL32 StartSpyAdapt(TMt tSrc, const TSimCapSet &tDstCap, u8 byMediaMode);
	BOOL32 StartSpyAudAdapt(TMt tSrc);
	//停止选看适配
	BOOL32 StopSelAdapt(TMt tSrc, TMt tDst, u8 byMediaMode, BOOL32 bIsStopSwitchToMt = TRUE, BOOL32 bIsStopSpySwitch = TRUE);
	//停止回传适配
	BOOL32 StopSpyAdapt(TMt tSrc, const TSimCapSet &tDstCap, u8 byMediaMode);
	//获得BAS参数额外信息
	BOOL32 GetBasParamExt(TMt &tSrc, u8 byMediaMode, TBasAdaptParamExt &tBasParamExt);
	//开启单个通道适配
	BOOL32 ChangeAdapt(const TEqp &tEqp, u8 byChnId);
	//开启bas对prs的支持
	void   StartBasPrsSupport(const TEqp &tEqp, u8 byChnId);
	//停止bas对prs的支持
	void   StopBasPrsSupport(const TEqp &tEqp, u8 byChnId);
	//开启bas对prs的支持()
	void   StartBasPrsSupportEx(u8 byMediaMode);
	//开启级联多回传bas对prs的支持(在下级使用)，建立BAS到源的RTCP交换
	//void   StartMultiSpyBasPrsSupport(const TEqp &tEqp, u8 byChnId, u8 byChnMode, TMt tBasSrc);
	//停止单个通道适配
	BOOL32 StopBasAdapt(const TEqp &tEqp, u8 byChnId,BOOL32 bSelSelf = TRUE);
	//适配录像备份恢复时停止适配，不停到外设REC的交换
	BOOL32 StopBasAdaptOnlyForRecAdp(const TEqp &tEqp, u8 byChnId);
	//建立单个通道适配交换
	BOOL32 StartBasSwitch(const TEqp &tEqp, u8 byChnId);
	//拆除单个通道适配交换
	BOOL32 StopBasSwitch(const TEqp &tEqp, u8 byChnId,BOOL32 bSelSelf = TRUE);
	//停止所有广播适配
	void   StopAllBrdAdapt();
	//停止所有选看适配
	void   StopAllSelAdapt();
	//释放单会议占用的适配资源
	void   ReleaseBasChn();
	//挂载刷新广播适配参数
	BOOL32 RefreshBasBrdAdpParam(u8 byMediaMode);
	//挂载刷新视频适配参数
	BOOL32 RefreshBasParam4VidBrd( u16 wBiteRate );
	//挂载刷新双流适配参数
	BOOL32 RefreshBasParam4DSBrd( u16 wBiteRate );
	//挂载刷新音频适配参数
	BOOL32 RefreshBasParam4AudBrd(u16 wBiteRate);
	//根据所有与会终端信息刷新广播适配参数
	BOOL32 RefreshBrdBasParam4AllMt(u8 byMediaMode);
	//根据单个终端信息刷新广播适配参数
	BOOL32 RefreshBrdBasParamForSingleMt(u8 byMtId, u8 byMediaMode, BOOL32 bUpAdjust = FALSE, BOOL32 bIncludeSelf = FALSE);
	//根据单个终端信息刷新选看适配参数
	BOOL32 RefreshSelBasParam(TMt tSrc, TMt tDst, u8 byMediaMode, BOOL32 bUpAdjust = FALSE, BOOL32 bIncludeSelf = FALSE);
	//根据单个终端信息刷新回传适配参数
	BOOL32 RefreshSpyBasParam(TMt tSrc, TSimCapSet &tDstCap, u8 byMediaMode, BOOL32 bUpAdjust = FALSE,BOOL32 bIncludeSelf = FALSE);
	//低速录像刷新适配参数
	BOOL32 RefreshBasParamForLowBrUse();
	//获取低速录像适配输出
	BOOL32 GetLowBrUseBasOutPutInfo(TEqp &tBas, u8 &byChnId, u8 &byOutNum, u8 &byOutIdx);
	//获取适配模式
	u8     GetAdaptMode(u8 byMediaMode);
	//为广播需要过适配的终端寻找合适的bas输出
	BOOL32 FindBasChn2BrdForMt(u8 byMtId,  u8 byMediaMode, TBasOutInfo &tOutInfo);
	//为选看需要过适配的终端寻找合适的bas输出
	BOOL32 FindBasChn2SelForMt(TMt tSrc,  TMt tDst,  u8 byMediaMode, TBasOutInfo &tOutInfo);
	//为选看查找通道
	BOOL32	GetBasChnForSel(const TMt &tSrc, TSimCapSet &tDstCap, u8 byMediaMode,CBasChn **pcBasChn, TBasChnCapData &tChnCapData, BOOL32 bRecursive = TRUE);
	BOOL32  GetBasChnForAudSel(const TMt &tSrc, TAudioTypeDesc &tDstAudCap,CBasChn **pcBasChn, TBasChnCapData &tChnCapData);
	//为多回传查找通道
	BOOL32	GetBasChnForMultiSpy(const TMt &tSpySrc,TSimCapSet &tDstCap,u8 byMediaMode,CBasChn **pcBasChn, TBasChnCapData &tChnCapData);

	//根据目的能力为选看需要过适配的终端寻找合适的bas输出(多回传用)
	BOOL32 FindBasChn2SpyForMt(TMt tSrc,  const TSimCapSet &tDstCap,  u8 byMediaMode, TBasOutInfo &tOutInfo);
	//通过能力集、BAS、BAS通道查找对应BAS输出
	BOOL32 FindBasOutIdxByVidCapSet(TEqp &tBas, u8 byChnId, u8 byMediaMode,TVideoStreamCap &tSimCapSet,u8 &byOutIdx, u8 &byFrontOutNum);
	//通过能力集、BAS、BAS通道查找对应BAS输出
	BOOL32 FindBasOutIdxByAudCap(TEqp &tBas, u8 byChnId,TAudioTypeDesc &tAudCap,u8 &byOutIdx, u8 &byFrontOutNum);
	//为掉链的适配通道寻找合适的替代
	BOOL32 FindBasChnForReplace(TEqp *ptDiscBas, u8 byDiscNum, CBasChn **ptRplChn, TBasChnCapData *ptBasChnCapData);
	//为掉链的适配录像通道寻找合适的替代
	BOOL32 FindBasChnForRecAptReplace(u8 byMediaMode);
	//刷新备份BAS适配通道的参数
	BOOL32 RefreshBasParamForReplace(CBasChn* pBasChn,const TBasChnCapData &tChnEncCapData,u8 byMediaMode);
	//根据源和目的信息判断是否可释放对应适配通道
	BOOL32 IsCanRlsBasChn(const TMt &tRlsSrc, const TMt &tSpySrc, const TSimCapSet &tDstCap, u8 byRlsMode);
	//根据回传源信息和要求调整的分辨率，重新调整适配交换
	BOOL32 AdjustSpyStreamFromAdp(TMt tSpySrc, u8 byRes, TMsgHeadMsg tHeadMsg);
	BOOL32 AdjustSpyStreamFromAdpForMMcu(TMt tSpySrc, u8 byRes, TMsgHeadMsg tHeadMsg);
	//根据源和目的能力找一个能给级联调分辨率强刷参数的通道
	BOOL32	FindBasChnForAdjustSpyStream(const TMt &tSpySrc, const TSimCapSet &tDstCap, u8 byMediaMode,TBasOutInfo &tOutInfo);
	//是否需要调整选看BAS参数
	BOOL32 IsNeedAdjustSelParam(const TMt &tSrc,const TMt &tDst,TBasOutInfo &tOutInfo, u8 byMediaMode,CBasChn **pcBasChn, TBasChnCapData &tChnCapData);

	// 根据bas源和请求的媒体类型获取bas的解码类型
	const BOOL32 GetBasDecPT(const TMt& tBasSrc, const u8 byMode, u8& byVidBasDecPT, u8& byAudBasDecPt, TLogicalChannel &tSrcLgcChn );

	//[nizhijun 2011/02/22]是否满足双格式不过BAS的选看
	BOOL32 IsSelInDoubleMedia(const TMt &tSrc,const TMt &tDst);	
	/*-------------------------------------------------------*/

	/*---------------------------录像相关-----------------------*/
	BOOL32	UnPackStartRecMsg(CServMsg &cServMsg, TStartRecMsgInfo &tRecMsg,u16* pwErrCode); //消息解析
	
	void	SendStartRecReq(const TStartRecMsgInfo &tStartRecMsg,const TStartRecData &tRecData,const TSpyRecInfo *ptSpyRecInfo = NULL);

	//会议录像
	BOOL32	IsConfRecCheckPass(TStartRecMsgInfo &tRecMsg,u16* pwErrCode);					//会议录像校验
	void	PrepareDataForConfRec(const TStartRecMsgInfo &tRecMsg,TStartRecData &tRecData);	//开启会议录像请求
	void	StartConfRec(const TStartRecMsgInfo &tRecMsg);									//开启会议录像

	//终端录像
	BOOL32	IsMtRecCheckPass(TStartRecMsgInfo & tRecMsg,u16* pwErrCode);					 //终端录像校验
	void	PrepareDataForMtRec(const TStartRecMsgInfo &tRecMsg, TStartRecData &tRecData);	 //开启终端录像请求			
	void	StartMtRec(const TStartRecMsgInfo &tRecMsg, BOOL32 bIsNeedPreSetIn = TRUE,const TSpyRecInfo *ptSpyRecInfo = NULL);//开启终端录像

	//vrs新录播支持
	void	StartVrsRec(const TStartRecMsgInfo &tRecMsg, u8 byRecState, BOOL32 bNeedPresetin = TRUE); //开启vrs新录播录像
	u8		GetVrsRecMtId(TMt tRecSrcMt);// 通过录像终端匹配对应新录播实体MtId
	u8		AddVrsMt(TMtAlias &tMtAlias, u8 byType, TMt tSrc, u16 wMtDialRate = 0, u8 byCallMode = CONF_CALLMODE_NONE);//添加vrs新录播成员
	void	ReleaseVrsMt(u8 byVrsMtId, u16 wErrorCode = 0, BOOL32 bMcsMsg = TRUE);// 释放Vrs新录播所有信息，状态置为IDLE
	BOOL32  CheckVrsMtCapSet(TMt tMt, TMultiCapSupport &tCapSupport);// 新录播能力比较
	BOOL32  PrepareStartRecMsgInfo(TMt tRecSrcMt, TStartRecMsgInfo &tStartVrsMsgInfo);//准备要开启vrs新录播的信息
	void	SendStartVrsRecReq(const TStartRecMsgInfo &tStartRecMsg); //开启vrs新录播录像
    void	ProcRecMcuVrsRecChnnlStatusNotif( CServMsg &cServMsg ); //vrs新录播录像状态上报处理
	void	ProcMcsMcuGetRecChnnlStatusCmd(const CMessage * pcMsg); //查询录像机状态
	void	ProcGetVrsListRecord(TMtAlias &tMtAlias, TMt tVrs, u8 byListType, u32 dwGroupId, u32 dwPageNo, u32 dwListNum);//新录播请求列表处理
	void	ProcVrsMcuListAllRecNotif(const CMessage * pcMsg);//vrs放像列表上报，包含分组列表与文件列表
	void	ProcMcsMcuStartVrsPlayReq(const CMessage * pcMsg);//开启vrs放像请求处理
	void	StartVrsPlay(TMtAlias &tMtAlias, TPlayEqpAttrib tPlayAttrib);//开启vrs放像,包含呼叫处理
    void	ProcRecMcuVrsPlayChnnlStatusNotif( const CMessage * pcMsg ); //vrs新录播放像状态上报处理
	void	PrintVrsInfo(void);//打印当前vrs信息
	BOOL32	IsVrsRecMt(const TMt &tMt);//是否是vrs新录播实体
	void	ProcMcsMcuCloseListRecordCmd(const CMessage * pcMsg); //会控关闭文件列表（用于挂断vrs新录播实体）
	BOOL32	StartSwitchToRecorder(TMt tSrc, u16 wSrcChnnl, TEqp tRec, u8 byDstChnnl, u8 byMode, u8 bySwitchMode,
								  BOOL32 bStopBeforeStart, BOOL32 bAddRtcp); //给录像机建交换，支持录像机外设与vrs新录播
	void	StopSwitchToRecorder(TEqp tRec, u8 byDstChnnl, BOOL32 bMsg, u8 byMode);//拆到录像机的交换，支持录像机外设与vrs新录播
	BOOL32	GetVrsCallAliasByVrsCfgId(u8 byVrsId, TMtAlias &tVrsAlias);


	BOOL32 IsRecNeedAdapt(u8 byMediaMode = MODE_BOTH);		// 判断是否用户自定义适配录像
	BOOL32 IsRecAdaptUseBas();								//判断用户自定义适配录像是否需要适配器
	BOOL32 InitRecAdptParam();	    						// 对自定义格式录像的各项参数进行初始化
	BOOL32 InitRecAdptParamByMediaMode(u8 byMediaMode);
	void ResetRecAdptParam(u8 byMediaMode = MODE_BOTH);     			// 对自定义格式录像的各项参数进行重置
	BOOL32 AdjustRecAdptParam(u8 byMediaMode);							// 对自定义格式参数进行调整
	void OnMtRecDStreamSrcChanged(const BOOL32 bIsStart);
	/*-------------------------------------------------------*/

	
    //void   ProcBasStartupTimeout( const CMessage * pcMsg );
    BOOL32 IsMtSrcVmp2(u8 byMtId);
    BOOL32 IsNeedMdtpAdapt( TSimCapSet &tDstSCS, TSimCapSet &tSrcSCS );
public:
    u16    GetLeastMtReqBitrate(BOOL32 bPrimary = TRUE, u8 byMediaType = MEDIA_TYPE_NULL, u8 bySrcMtId = 0);
    u16    GetLeastMtReqBR(TSimCapSet &tSrcSCS, u8 bySrtMtId = 0, BOOL32 bPrimary = TRUE);
    u8     GetLeastMtReqFramerate(u8 byMediaType = MEDIA_TYPE_NULL, u8 bySrcMtId = 0);
	BOOL32 GetMtRealSrc(u8 byMtId, u8 byMode, TMt& tMtSrc, u8& byOutChnl);
	BOOL32 GetMtCurVidSrc(u8 byMtId, TMt& tMtSrc, u8& byOutChnl);
protected:
    BOOL32 IsRecordSrcBas( u8 byType, TMt &tRecSrc, u8 &byChnId, u8 &byRecInIdx);

    //4 、高清适配相关函数
	void   GetDSBrdSrcSim(TDStreamCap &tDSim);

    BOOL32 IsDSSrcH263p( void );
    BOOL32 GetProximalGrp(TVideoStreamCap &tSimCapSet);

	void   ReleaseResbySel(TMt tSelDstMt, u8 bySelMode, BOOL32 bStopDstSwitch = TRUE);
    //old
    //BOOL32 IsRecordSrcHDBas( void );

	/*----------------------------------5.PRS相关函数---------------------------*/
	//为丢包会议预占Prs通道
	BOOL32 AssignPrsChnnl4Conf();
	//开启广播prs
	void StartBrdPrs(u8 byMediaMode);
	//停止广播prs
	void StopBrdPrs(u8 byMediaMode);
	//切换prs源
	BOOL32 ChangePrsSrc(u8 byPrsId, u8 byPrsChnId, u8 byMediaMode, TMt tPrsSrc, u8 byPrsSrcOutChn = 0);
	//获取prs源的rtcp申述地址
	BOOL32 GetRemoteRtcpAddr(TMt tRtpSrc, u8 byRtpSrcOutChn, u8 byMediaMode, TTransportAddr &tRemoteAddr);
    //vmp动态开启prs
	void StartPrsForVmp(TEqp tVmpEqp);
	//mixer动态占用prs
	void StartPrsForMixer(void);
	//vmp动态停止释放prs
	void StopPrsForVmp(TEqp tVmpEqp);
	//mixer动态停止释放prs
	void StopPrsForMixer(void);
	//建立目的终端到prs的rtcp交换
	void AddRemovePrsMember(u8 byMemId, u8 byPrsId, u8 byPrsChl, u8 byPrsMode = MODE_VIDEO, u8 bAdd = TRUE,u16 wSpyStartPort = SPY_CHANNL_NULL);
	//20110412 rtcp打包
	void AddRemovePrsMember(u8 byDstMtNum,
								TMt *pDstMt,
								u8 byPrsId,
								u8 byPrsChl,
								u8 byPrsMode = MODE_VIDEO , 
								u8 bAdd  = TRUE ,
							    u16 wSpyStartPort  = SPY_CHANNL_NULL);

	//建立以当前prs通道为目的的rtcp交换
	void BuildRtcpAndRtpSwithForPrs(u8 byPrsId, u8 byPrsChl);
	//停止单个prs通道工作
	void StopPrs(u8 byPrsId, u8 byPrsChnl, u8 byChnMode);
	//停止当前会议所有prs通道工作
	void StopAllPrsChn();
	//释放当前会议占用的所有prs通道
	void ReleaseAllPrsChn();
	void AddRemoveRtcpToDst(u32		dwDstIp,
							u16		wDstPort, 
                            TMt		tSrc,
							u8		bySrcChnnl = 0, 
		                    u8		byMode = MODE_VIDEO,
							BOOL32	bMapSrcAddr = FALSE,
							u16		wSpyStartPort = SPY_CHANNL_NULL,
							BOOL32	bAddRtcp = TRUE
							/*u8	byHduSubChnId = 0*/);

    //[liu lijiu][20100902]建立到本级会议回传通道的RTCP交换
	//void SwitchLocalMultipleSpyRtcp( TMt tSrc, u8 bySrcChnnl, TMt tDst, u8 byMode, BOOL32 bIsBuildSwitch);
	void BuildRtcpSwitchForSrcToDst(TMt tSrc, TMt tUnlocalDst, 
								 u8 byMediaMode, u8 bySrcChnnl = 0, 
								 /*u8 bySwitchMode = SWITCH_MODE_BROADCAST, */
								 u8 byDstChnnl = 0,
								 BOOL32 bAddRtcp = TRUE);
	//停到目的终端的rtcp交换
	void StopPrsMemberRtcp(TMt tDstMt, u8 byMode);
	//停止到目的实体的所有Rtcp交换
	void StopRtcpSwitchAlltoOne(const TMt &tRtcpDst, u8 byDstChn, u8 byMode);
	

	
	//BOOL32 GetMtRealSrc(u8 byMtId, u8 byMode, TMt& tMtSrc, u8& byOutChnl);
	void   OccupyPrsChnl(u8 byPrsId, u8 byPrsChnl);
	void   RlsPrsChnl(u8 byPrsId, u8 byPrsChnl);

    //6、VMP相关函数 
	void ProcMcsMcuVmpPriSeizeRsp(const CMessage *pcMsg);//vmp抢占应答
	void ProcMtSeizeVmpTimer(const CMessage *pcMsg);	//最初要抢占的MT进行抢占操作
	void ProcVmpBatchPollTimer(void);	//vmp批量poll
	void ProcMcsMcuVmpCmd(const CMessage * pcMsg);		//处理批量poll PAUSE,RESUME等命令
	void ProcStartVmpReq(CServMsg &cServMsg);			//开始画面合成
	void StartVmpBrd(TEqp tVmpEqp);							//开始MPU-VMP广播码流
	void StopVmpBrd(TEqp tVmpEqp);					//停止MPU-VMP广播码流
	void ProcChangeVmpParamReq(CServMsg &cServMsg);		//改变画面合成参数
	void ProcStartVmpBatchPollReq(CServMsg &cServMsg);	//batchpoll
	void ClearOneVmpMember(u8 byVmpId, u8 byChnnl, TVMPParam_25Mem &tVmpParam, BOOL32 bRecover = FALSE);
	void RefreshVmpChlMemalias(u8 byVmpId);//刷新vmp外设通道台标
	void GetSmcuMtAliasInVmp(u8 byVmpId); //获得画面合成中下级终端别名(别名扩容支持)
public:
	u8	GetVmpOutChnnlByRes(u8 byVmpId, u8 byRes,  u8 byMediaType = MEDIA_TYPE_H264, u8 byFrameRate=0, emProfileAttrb emAttrb=emBpAttrb);
	u8	GetResByVmpOutChnnl(u8 byVmpId, u8 &byMediaType, u8 &byFrameRate, u8 &byAttrb, u8 byChnnl);
	u8  GetVmpOutChnnlByDstMtId(u8 byDstMtId, u8 byVmpId, BOOL32 bNeedCheckSelSrc = TRUE);//追加参数，
	u8  GetVmpSpecialOutChnnlByDstMtId(u8 byDstMtId);
	u8  GetMtNumSeeVmp(u8 byVmpId, u8 byChnnl, u8 byNeglectMtId = 0);
    BOOL32 GetMtAliasToVmp(TMt tMt, s8 *pchAlias);     // vmp获得终端别名扩容，用于外设显示合成图片 [7/4/2013 liaokang]
protected:	
      
	
	BOOL32 CheckMpuMember(u8 byVmpId, TVMPParam_25Mem& tVmpParam, u16& wErrorCode, BOOL32 bChgParam = FALSE);
	u16 CheckMpuAudPollConflct(u8 byVmpId, TVMPParam_25Mem& tVmpParam, BOOL32 bChgParam = FALSE);
	u16 CheckSameMtInMultChnnl(u8 byVmpId, TVMPParam_25Mem & tVmpParam, BOOL32 bChgParam = FALSE);
	BOOL32 CheckAdpChnnlLmt(u8 byVmpId, TVMPParam_25Mem& tVmpParam, CServMsg &cServMsg, BOOL32 bChgParam = FALSE);
	BOOL32 CheckAdpChnnlAtStart(u8 byVmpId, TVMPParam_25Mem &tVmpParam, u8 byMaxHdChnnlNum, CServMsg &cServMsg);
	BOOL32 CheckAdpChnnlAtChange(u8 byVmpId, TVMPParam_25Mem &tVmpParam, u8 byMaxHdChnnlNum, CServMsg &cServMsg);
	
	// yanghuaizhi 获得终端在vmp中的所有位置,包含mcu上传终端特殊处理
	void GetChlOfMtInVmpParam(const TVMPParam_25Mem& tVmpParam, TMt tMt, u8 &byPosNum, u8 *pbyPos = NULL);

	BOOL32 CheckVmpParam(const u8 byVmpId, TVMPParam_25Mem& tVmpParam, u16& wErrorCode);
    void ChangeVmpStyle(TMt tMt, BOOL32 bAddMt/*, BOOL32 bChangeVmpParam = TRUE*/);
	// 2011-7-29 mod by peng guofeng : 移除参数
    void ChangeVmpSwitch( u8 byVmpId, u8 byState = VMP_STATE_START );
	// 2011-7-29 mod
	void CancelOneVmpFollowSwitch( u8 byMemberType, BOOL32 bNotify );
    void ChangeVmpChannelParam(TMt * ptNewSrc, u8 byMemberChnl, TMt * ptOldSrc = NULL);
    void ChangeVmpBitRate(u16 wBitRate, u8 byVmpChanNo = 1); 
    void AdjustVmpBrdBitRate(TMt *ptMt = NULL);
    void AdjustVmpSelBitRate(TEqp tVmpEqp);//调整选看vmp的指定通道码率
	u8   GetVmpChnnl( void );
	void SetVmpChnnl(u8 byVmpId, TMt tMt, u8 byChnnl, u8 byMemberType, BOOL32 bMsgMcs = FALSE );
	BOOL32 IsDynamicVmp( u8 byVmpId );
	u8   GetVmpDynStyle(u8 byVmpId, u8 byMtNum );

// 这是一个更为基础的接口，单纯判断有几个合成成员，出什么最接近的风格
	u8   GetVmpStylebyMtNum( u8 byMtNum ); 

	// 2011-7-29 mod by peng guofeng : 移除多余参数
    BOOL32 IsChangeVmpParam(/*TMt * ptMt*/);
	// 2011-7-29 mod end
    BOOL32 IsVMPSupportVmpStyle(u8 byStyle, u8 byEqpId, u16 &wError);
    BOOL32 IsMCUSupportVmpStyle(u8 byStyle, u8 &byEqpId, u8 byEqpType, u16 &wError );
    
	// yanghuaizhi 20111130 抢占提示消息打包
	void SendVmpPriSeizeReq(u8 byVmpId, TMt &tSeizeMt, u8 byForceSeize = 0);
	// yanghuaizhi 20111201 是否允许同一终端进多个通道
	BOOL32 IsAllowVmpMemRepeated(const u8 byVmpId);
	// yanghuaizhi 20111209 判断选择的vmp是否能备份旧的vmp
	BOOL32 IsVmpSupportBackup(TVMPParam_25Mem &tVmpParam, u8 byOldVmpId, u8 byNewVmpId);
	// yanghuaizhi 20111209 根据vmpid获得其支持最大前适配个数
	u8 GetMaxHdChnlNumByVmpId(const TConfInfo &tConfInfo, u8 byVmpId, u8 &byMaxStyleNum);
	// yanghuaizhi 20120215 (8KH移植)判断是否是8kh1080p/30fps非适配会议开启的vmp
	BOOL32 Is8khVmpInConf1080p30fps(const TConfInfo &tConfInfo, u8 byVmpId);
	// yanghuaizhi 20111201 根据模版准备vmp的后适配能力列表,并判断是否超出能力
	BOOL32 PrepareVmpOutCap(u8 byVmpId, BOOL32 bBrd = TRUE);
	// yanghuaizhi 20111201 根据模版准备vmp的后适配能力列表,并判断是否超出能力
	BOOL32 PrepareMpu2VmpOutCap(u8 byVmpId);
	// yanghuaizhi 20130801 根据模版准备8kivmp的广播后适配能力列表，8KIVMP广播与非广播编码不同
	BOOL32 Prepare8kiVmpOutCap(u8 byVmpId, BOOL32 bBrd);
	// yanghuaizhi 20111212 根据vmpoutparam获得当前vmp需要的vicp资源
	s32 GetVicpResByVmpOutParam(const TKDVVMPOutParam &tVMPOutParam);
	// yanghuaizhi 20111219 调整前适配信息,有需要占但还未占到前适配的成员,如有空余,抢占前适配
	void AdjustVmpHdChnlInfo(const TEqp &tVmpEqp);
	// yanghuaizhi 20120709 通知主席终端刷新vmpparam信息
	void SendVmpParamToChairMan();
	// yanghuaizhi 20120731 调整vmp双流跟随真实双流源,上报会控
	void UpdateVmpDStream(const TMt tMt);
	// yanghuaizhi 20130305 判断是否为正确的VmpId，移入mcuutility.h中
	//BOOL32 IsValidVmpId(u8 byVmpId);
	// yanghuaizhi 20130305 将VmpParam信息从PeriVmpParam中更新到ConfVmpParam
	void UpdateConfVmpParam(u8 byVmpId);
	// yanghuaizhi 20130305 调整旧通道成员分辨率
	void AdjustMtResInLastVmpParam(u8 byVmpId);
	// yanghuaizhi 20130320 获得vcs切换场景时的默认VmpParam信息
	TVMPParam_25Mem GetVcsRestoreConfVmpParam(void);
	// yanghuaizhi 20130320 判断当前会议占用的合成器中是否有指定UseState的合成器
	BOOL32 IsVmpStatusUseStateInVmpList(u8 byUseState);
	// yanghuaizhi 20130320 获得仅有的默认合成器
	u8 GetTheOnlyVmpIdFromVmpList(void);
	// yanghuaizhi 20130320 获得索引中占用合成器个数
	u8 GetVmpCountInVmpList();
	// yanghuaizhi 20130320 将指定vmp添加到索引
	void AddVmpIdIntoVmpList(u8 byVmpId);
	// yanghuaizhi 20130320 将指定vmp从索引删除
	void RemoveVmpIdFromVmpList(u8 byVmpId);
	// yanghuaizhi 20130321 判断会议vmpList中是否有指定VmpId
	BOOL32 IsVmpIdInVmpList(u8 byVmpId);
	// 从消息内容中获得tVmpParam信息
	BOOL32 GetVmpParamFormBuf(u8 *pbyBuf, u16 wBufLen, TVMPParam_25Mem &tVmpParam);
	// 判断是否需要调整该终端分辨率,若需要调整返回要调整的分辨率(提供接口)
	BOOL32 IsNeedAdjustMtVidFormat(const TMt &tMt, TExInfoForResFps &tExInfoForRes);
	// 判断是否需要调整该终端帧率,若需要调整返回要调整的帧率(提供接口)
	BOOL32 IsNeedAdjustMtVidFps(TMt &tMt, TExInfoForResFps &tExInfoForFps);
	// yanghuaizhi 20130325 获得终端在vmp中信息（占用通道数、最小通道分辨率、是否要调整分辨率）
	BOOL32 IsMtNeedAdjustResInConfVmp(const TMt &tMt, TExInfoForResFps &tExInfoForRes, u8 &byVmpChlRes, u8 &byChlCount);
	// yanghuaizhi 20130325 判断终端属性是否是vmp的vip
	TChnnlMemberInfo IsVipMemForVmp(const TMt &tMt, TExInfoForResFps &tExInfoForRes);
	// yanghuaizhi 20130326 调整画面合成内终端分辨率
	BOOL32  ChangeMtResFpsInVmp(u8 byVmpId, TMt tMt, TVMPParam_25Mem *ptVMPParam, BOOL32 bOccupy = TRUE, u8 byMemberType = VMP_MEMBERTYPE_NULL, u8 byPos = MAXNUM_VMP_MEMBER, BOOL32 bMsgMcs = FALSE);
	// yanghuaizhi 20130326 清某终端在vmp的所有前适配
	void ClearAdpChlInfoInAllVmp(TMt tMt);
	// yanghuaizhi 20130327 获得一个主要vmp，用于点名时有多个合成器，指定一个用作点名
	u8 GetTheMainVmpIdFromVmpList();
	// yanghuaizhi 20130325 获得终端是否在降帧率的vmp中
	BOOL32 IsMtNeedAdjustFpsInConfVmp(TMt &tMt, TExInfoForResFps &tExInfoForFps);
	// 获得vmp对应通道码率
	u16 GetVMPOutChlBitrate(TEqp tVmpEqp, u8 byIdx);
	// 设置vmp对应通道码率
	void SetVMPOutChlBitrate(TEqp tVmpEqp, u8 byIdx, u16 wBr);
	// 设置vmp对应广播模式
	void SetVMPBrdst(TEqp tVmpEqp, u8 byVMPBrdst);
	// 为N+1备份支持多画面合成提供查找适当vmp外设接口
	BOOL32 IsVmpSupportNplus(u8 byVmpSubType, u8 byVmpStyle, u8 &byEqualAbilityVmpId, u8 &bySlightlyLargerAbilityVmpId, u8 &byMinAbilityVmpId);
	// yanghuaizhi 20130412 调整画面合成内终端分辨率
	BOOL32  ChangeMtResFpsInTW(TMt tMt, BOOL32 bOccupy = TRUE);
	// yanghuaizhi 20130417 判断某MCU是否是简单级联的MCU
	BOOL32 IsNotSupportCascadeMcu(const TMt &tMt);
	// yanghuaizhi 20130417 主席终端是否在选看画面合成
	BOOL32 IsVmpSeeByChairman();
	// yanghuaizhi 20130425 vmp获得终端别名，用于外设显示合成图片
	//BOOL32 GetMtAliasInVmp(TMt tMt, s8 *pchAlias);
    BOOL32 GetMtAliasInVmp(TMt tMt, s8 *pchAlias, u8 byMaxAliasLen = VALIDLEN_ALIAS );    // 扩容 [7/4/2013 liaokang]
	// yanghuaizhi 20130425 终端开通道调整模版开启的会控指定画面合成
	void AdjustCtrlModeVmpModuleByMt(const TMt &tMt);
	// yanghuaizhi 20130527 下级挂断本级上传通道终端
	void ProcMMcuSpyMtByRemoveMt( const TMt &tRemoveMt );
	// yanghuaizhi 20130527 结束合成请求处理
	void ProcStopVmpReq(CServMsg &cServMsg);
	// yanghuaizhi 20130527 开启合成广播请求处理
	void ProcStartVmpBrdReq(CServMsg &cServMsg);
	// yanghuaizhi 20130527 停止合成广播请求处理
	void ProcStopVmpBrdReq(CServMsg &cServMsg);
	// yanghuaizhi 20130527 vmp广播冲击发言人
	void ChgSpeakerResInVmpBrd();
	// yanghuaizhi 20130527 8kh合成广播切为非广播时特殊处理
	void AdjustVmpOutChnlInChgVmpBrd(TEqp tVmpEqp, BOOL32 bStartVmpBrd);
	// yanghuaizhi 20130527 对终控台发来的合成信息进行格式化
	void FormatVmpParamFromMTCMsg(TVMPParam_25Mem &tVmpParam);
	// yanghuaizhi 20130527 画面合成开启时的各种Check
	BOOL32 CheckStartVmpReq(u8 &byVmpId, TVMPParam_25Mem &tVmpParam, BOOL32 bIsVmpModule, CServMsg &cServMsg, u16 &wError);
	// yanghuaizhi 20130527 找合适vmpid，或指定vmpid时判是否支持
	BOOL32 FindVmpSupportVmpParam(u8 &byVmpId,u8 byVmpStyle, u8 byVmpCount, u16 &wError);
	// yanghuaizhi 20130527 根据会议信息更新vmp成员
	void UpdateVmpMembersWithConfInfo(const TEqp &tVmpEqp, TVMPParam_25Mem& tVmpParam);
	// yanghuaizhi 20130527 看会控指定的成员是否有单回传mcu下的终端占用多个通道的情况
	u16 UpdateVmpMembersWithMultiSpy(const TEqp &tVmpEqp, TVMPParam_25Mem& tVmpParam);
	// yanghuaizhi 20130529 画面合成调整时的各种Check
	BOOL32 CheckChangeVmpReq(u8 &byVmpId, TVMPParam_25Mem &tVmpParam, u16 &wError);
	// yanghuaizhi 20130529 建vmp通道到各终端的交换,对终端分组建交换
	void StartVmpSwitchGrp2AllMt(const TEqp &tVmpEqp);
	// yanghuaizhi 20130529 画面合成级联多回传处理
	BOOL32 SendPreSetInReqForVmp(const TEqp &tVmpEqp, TVMPParam_25Mem *ptVmpParam, BOOL32 bStart, BOOL32 &bSendPreSetIn);
	// yanghuaizhi 20130529 准备给外设发送消息的共通数据
	TVmpCommonAttrb GetVmpCommonAttrb(u8 byVmpId, TVMPParam_25Mem *ptVMPParam, BOOL32 bStart);
	// yanghuaizhi 20130529 获得8kgvmp对应输出信息
	u8 GetResBy8KEVmpOutChnnl(u8 &byMediaType, u8 byChnnl);
	// yanghuaizhi 20130529 获得8khvmp对应输出信息
	u8 GetResBy8KHVmpOutChnnl(u8 &byMediaType, u8 &byFrameRate, u8 &byAttrb, u8 byChnnl);
	// yanghuaizhi 20130529 获得mpu对应输出信息
	u8 GetResByMpuOutChnnl(u8 &byMediaType, u8 byBoardVer, u8 byChnnl);
	// yanghuaizhi 20130529 根据分辨率获得8kgvmp对应输出通道
	u8 Get8KEVmpOutChnnlByRes(u8 byRes,  u8 byMediaType);
	// yanghuaizhi 20130529 根据分辨率获得8khvmp对应输出通道
	u8 Get8KHVmpOutChnnlByRes(const TEqp &tVmpEqp, u8 byRes,  u8 byMediaType, u8 byFrameRate, emProfileAttrb emAttrb);
	// yanghuaizhi 20130529 根据分辨率获得mpu对应输出通道
	u8 GetMpuOutChnnlByRes(u8 byRes,  u8 byMediaType, u8 byBoardVer);
	// yanghuaizhi 20130530 VMP给MCU开始工作通知
	void ProcVmpMcuStartVmpNotify(CServMsg &cServMsg);
	// yanghuaizhi 20130530 VMP上线通知
	void ProcVmpConnectedNotify(CServMsg &cServMsg);
	// yanghuaizhi 20130530 VMP掉线通知
	void ProcVmpDisconnectedNotify(CServMsg &cServMsg);
	// yanghuaizhi 20130530 停止VMP相关所有交换
	void StopVmpSwitch(const TEqp &tVmpEqp);
	// yanghuaizhi 20130530 会控指定模式建交换处理
	void ChangeCtrlModeVmpSwitch(const TEqp &tVmpEqp, u8 byState,TVMPParam_25Mem &tVMPParam);
	// yanghuaizhi 20130530 自动模式建交换处理
	void ChangeAutoModeVmpSwitch(const TEqp &tVmpEqp, u8 byState,TVMPParam_25Mem &tVMPParam);
	// yanghuaizhi 20130531 跟随通道合成成员调整判断
	BOOL32 IsVmpChannelParamChged(const TEqp &tVmpEqp, TVMPParam_25Mem &tVmpParam, TMt * ptNewSrc, u8 byMemberType, TMt * ptOldSrc);
	// yanghuaizhi 20130531 根据被抢占选项进行操作
	void ExecuteMtBeSeizedChoice(const TEqp &tVmpEqp, TVMPParam_25Mem &tVMPParam, TSeizeChoice &tSeizeChoice);
	// yanghuaizhi 20130531 对抢占终端处理
	void ExecuteMtSeizeChoice(const TEqp &tVmpEqp, TVMPParam_25Mem &tVMPParam,const TMt &tSeizeMt);
	// yanghuaizhi 20130531 终端选看画面合成Check
	BOOL32 CheckTMtSeeVmp(const TEqp &tVmpEqp, const TMt &tDstMt, BOOL32 &bIsChairManSeeVmp, u16 &wError);
	// yanghuaizhi 20130531 调整vpu广播码率
	void AdjustVpuBrdBitRate(TMt *ptMt);
	// yanghuaizhi 20130531 调整mpu广播码率
	void AdjustMpuBrdBitRate();
	// yanghuaizhi 20130531 vmp处理中对下级单回传新成员执行OnMMcuSetIn
	void OnMMcuSetInForVmp(TVMPParam_25Mem &tConfVMPParam, TVMPParam_25Mem &tPeriVmpParam);
	// yanghuaizhi 20130708 对合成成员通道排序，按发言人>不可降分辨率Mt>其余终端
	void GetVmpMemberSort(TVMPParam_25Mem &tConfVMPParam, u8 &byLen, u8 *pbyMemSort);
	// yanghuaizhi 20131105 判终端是否要按特殊方式解码
	BOOL32 IsRcvH264DependInMark(u8 byMtId);
	
	//lukunpeng 2010/07/16 成员转换调整
	u8 BatchTransVmpMemberToTVMPMemberEx(TVMPMember *ptVmpMemberArray, u8 byMaxMemberNum, TVMPMemberEx *ptVmpMemberExArray, u8 byMaxMemberExNum);
	void AdjustVmpParamMember(u8 byVmpId, TVMPParam_25Mem *ptVMPParam, u8 byVmpSubType, BOOL32 bStart);
	void	AdjustVmpParam(u8 byVmpId, TVMPParam_25Mem *ptVMPParam, BOOL32 bStart = FALSE, BOOL32 bNeedPreSetIn = TRUE);
	BOOL32  CheckSelSrcVmpAdaptChnl(TMt tSrcMt, TMt tDstMt, u8 SelMode, TEqp &tVmpEqp);
	BOOL32  ChgMtVidFormatRequired(TMt tMt, BOOL32 bNeglectVmpSubType = FALSE );
	BOOL32  ChangeMtVideoFormat( TMt   tMt, BOOL32 bConsiderVmpBrd = TRUE, BOOL32 bNonKeda = FALSE, BOOL32 bSeizePromt = FALSE);  //针对指定位置调分辨率
	void	ChgMtFormatInSdVmp( u8 byMtId, TVMPParam * ptVMPParam, BOOL32 bStart = TRUE );	//针对标清VMP的调整分辨率
	//BOOL32 GetMtFormat(const TMt &tMt, TVMPParam *ptVmpParam, u8 &byNewFormat, BOOL32 bStart = TRUE, BOOL32 bSeize = TRUE, BOOL32 bConsiderVmpBrd = TRUE, BOOL32 bNonKeda = FALSE);
	BOOL32 GetMtFormat( u8 byVmpId,
						const TMt &tMt,
						TVMPParam_25Mem *ptVmpParam,
						u8 &byNewFormat,
						BOOL32 bConsiderVmpBrd = TRUE,
						BOOL32 bNonKeda = FALSE,
						BOOL32 bForceVip = FALSE
						);
	BOOL32 GetMtFormat(const TMt &tMt, u8 &byNewFormat, BOOL32 bConsiderVmpBrd);
	
	BOOL32 GetMtFormatInSdVmp(const TMt &tMt, TVMPParam *ptVmpParam, u8 &byNewFormat, BOOL32 bStart = TRUE);
	BOOL32  ChgMtFormatInSdVmp( TMt &tMt,	
								TVMPParam *ptVmpParam,	
								BOOL32 bStart = TRUE,
								u8 byMemberType = VMP_MEMBERTYPE_NULL
								);	//新接口
	//	BOOL32  ChgMtFormatInMpu( u8 byMtId,	TVMPParam *ptVmpParam,	BOOL32 bStart = TRUE, BOOL32 bSeize = TRUE, BOOL32 bConsiderVmpBrd = TRUE, BOOL32 bNonKeda = FALSE );
	
	BOOL32 GetMtFormatInMpu(const TMt &tMt,
							TVMPParam_25Mem *ptVmpParam,
							u8 &byNewFormat,
							CVmpChgFormatResult &cVmpChgFormatRes,
							BOOL32 bStart,
							BOOL32 bConsiderVmpBrd,
							BOOL32 bNonKeda,
							BOOL32 bForceVip = FALSE,
							u8 byMemberType = VMP_MEMBERTYPE_NULL);
	
	
	
	BOOL32  ChgMtFormatInMpu (	TMt &tMt,	
								TVMPParam_25Mem *ptVmpParam,	
								BOOL32 bStart, 
								const TExInfoForInVmpRes &tExInfoForInVmpRes,
								u8 &byNewFormat
								);

	u8 GetMtFpsInVmp( const TMt &tMt, u8 byRes, BOOL32 bStart = TRUE);
		

	BOOL32  ChgMtFpsInVmp(	u8 byVmpId,
							const TMt &tMt,
							BOOL32 bStart,
							u8 byNewFormat
							);

	BOOL32 GetMtAlias(TMt tMt, s8 *pchAlias);
	BOOL32	RecoverMtResInMpu(TMt tMt);
	void	AddToVmpNeglected( u8 byMtId);
	BOOL32	IsMtNeglectedbyVmp( u8 byMtId);
	void	ProcMcsMcuStartSwitchVmpMtReq(const CMessage * pcMsg);
	void	TMtSeeVmp(CServMsg &cServMsg, BOOL32 bNeglectVmpMode = FALSE);//终端选看画面合成
	void	MCSSeeVmp(CServMsg &cServMsg, BOOL32 bNeglectVmpMode = FALSE);//20120329 yhz 会控选看画面合成处理
	void	AdjustVmpParambyMemberType(u8 byVmpId, const TMt *ptSpeaker,TVMPParam_25Mem &tVmpParam, const u8& byMembertype = VMP_MEMBERTYPE_SPEAKER,const TMt *ptOldSrc = NULL);
    void	UpdataVmpEncryptParam(TMt tMt, u8 byChnlNo, u8 byVmpEqpId = 0);
	BOOL32  VidResAdjust(u8 byVmpId, u8 byVmpStyle, u8 byPos, u8 byMtStandardFormat, u8 &byReqRes);
    BOOL32  IsDelayVidBrdVCU(void);	

	u8		GetVmpSubType( u8 byEqpId );

	void ChangeVmpParam(u8 byVmpId, TVMPParam_25Mem *ptVMPParam = NULL, BOOL32 bStart = FALSE );
	void ConstructMsgAndSendtoVmp(u8 byVmpId, CParamToVmp &cParamToVmp, BOOL32 bStart = FALSE);

    //7、vmp tvwall相关函数 
	BOOL32 IsMtInTvWall(const TMt &tMt);	
    void ChangeVmpTwChannelParam(const TMt * ptNewSrc, const u8& byMemberType);
    void ChangeVmpTwParam(TVMPParam *ptVmpParam = NULL, const BOOL32& bStart = FALSE);
	// 2011-7-29 mod by peng guofeng : 移除参数
    void ChangeVmpTwSwitch(/*u8 bySrcType = 0, */u8 byState = VMP_STATE_START);
	// 2011-7-29 mod end
    void SetVmpTwChnnl(TMt tMt, u8 byChnnl, u8 byMemberType, BOOL32 bMsgMcs = FALSE);
	// 2011-11-3 add by pgf
	u8 GetVmpChnnlNumBySpecMemberType(u8 byMemberType, TMt* ptMt = NULL);
	void GetTvWallChnnlNumBySpecMemberType(u8 byMemberType, u8 &byVidNum, u8 &byAudNum);
	void GetHduChnnlNumBySpecMemberType(u8 byMemberType, u8 &byVidNum, u8 &byAudNum);

    //8、tvall相关函数 
    void ChangeTvWallSwitch(TMt * ptSrc, u8 byTvWallId, u8 byChnlIdx,u8 byTWMemberType, u8 byState, BOOL32 bNeedPreSetin = TRUE, u8 byMode = MODE_BOTH);
	BOOL32 IsMtNotInOtherTvWallChnnl(u8 byMtId, u8 byTvWallId, u8 byChnlId);
	BOOL32 IsMtNotInOtherTvWallChnnl(TMt tMt, u8 byTvWallId, u8 byChnlId,BOOL32 bIsExceptSelf = FALSE,BOOL32 bIsFakeInTvWall = TRUE);
	BOOL32 IsHasMtInHduOrTwByMcuIdx(u16 wSmcuIdx);
    
    //9、recorder相关函数 
	//void StopAllMtRecoderToPeriEqp();
    void AdjustConfRecordSrcStream(u8 byMode, BOOL32 bIsStart);
	void AdjustMtRecSrcStream(u8 byMode, const TEqp &tEqp, u8 byRecChl, const TMt &tSrc, BOOL32 bIsStart);
	void FindMtAndAdjustMtRecSrcStream( const TMt &tMcu,const u8 byMode,const BOOL32 bIsStart );
	void AddRemoveRecRtcp(u8 byMode, const TEqp &tEqp, u8 byRecChl, const TMt &tSrc, u8 byRecChnlIdx, BOOL32 isAdd);
	
// [pengjie 2010/9/29] 调整会议录像状态
	void AdjustConfRecByBrdSrc( void );
// End
	
	//10、Hdu相关函数
	BOOL32 ChangeHduSwitch(TMt/*const*/ * ptSrc, 
		const u8 byHduId, 
		const u8 byChnlIdx, 
		const u8 byHduSubChnId,
		const u8 byHduMemberType, 
		const u8 byState, 
		const u8 byMode = MODE_BOTH, 
		const BOOL32 bBatchPoll = FALSE,
		const BOOL32 bCanSendStopPlayMsg = TRUE,
		const BOOL32 bNeedPreSetin = TRUE,
		const u8 byMulitModeHdu = HDUCHN_MODE_ONE
		);

	// [9/21/2011 liuxu] 获取hdu通道的实际音视频模式
	u8 GetHduChnnlRealMode( const u8 byHduId, const u8 byChidx);

    BOOL32 IsMtNotInOtherHduChnnl(const u8 byMtId, const u8 byHduId, const u8 byChnlId, const u8 bySubChnId = 0);
	BOOL32 IsMtNotInOtherHduChnnl(const TMt tMt, const u8 byHduId, const u8 byChnlId,BOOL32 bIsExceptSelf = FALSE, BOOL32 bIsFakeInHdu = TRUE);
	BOOL32 IsHduChnnlMcsSpec(const TMt tMt, const u8 byHduId, const u8 byChnlId, const u8 bySubChnId = 0);
	BOOL32 HduBatchPollOfOneCycle( const BOOL32 bFirstCycle );            // hdu一个周期的批量轮询
	void   StopHduBatchPollSwitch( const BOOL32 bStopPlay = FALSE );
	BOOL32 CheckHduAbility(TMt tSrc, const u8 byHduId, const u8 byChnId, u8 *pbyVmpOutChl = NULL,BOOL32 bIsSecVideo = FALSE);
	BOOL32 CheckTwAbility(TMt tSrc, const u8 byTwId, const u8 byChnId);

	BOOL32 ChangeMtResFpsInHduVmp(TMt tMt,
								  u8 byHduId, 
								  u8 byHduChnId,
								  u8 byHduSubChnId,
								  BOOL32& bIsStartSwitch,
								  BOOL32 bIsStart = TRUE,
								  BOOL32 bForceAdjust = FALSE
								  );
	// [2013/03/11 chenbing]
	void ChangeHduVmpMode(CServMsg cServMsg);
	void SendChangeHduModeReq(CServMsg cServMsg);

	// [2013/04/11 chenbing]
	BOOL32 IsOldViewMtAndNewViewMtSame(TMt tNewtMt, TMt tOldtMt);
	BOOL32 IsCanAdjustMtRes(TMt tMt);
	BOOL32 IsCanAdjustMtFps(TMt tMt);
	BOOL32 IsViewMtInHduVmp(TMt tMt, TMt *ptOldTMt = NULL, BOOL32 bIsNewViewMt = TRUE);
	BOOL32 IsMtInHduVmp(TMt tMt,
						u8 byHduId = 0,
						u8 byHduChnId = 0,
						u8 byHduSubChnId = 0,
						BOOL32 bIsIgnoreCurChn = FALSE,
						BOOL32 bIsIgnoreCurEqpId = FALSE,
						BOOL32 bIsIgnoreCurState = FALSE
						);
	BOOL32 IsBatchPollMtInHduVmp( TMt tMt,BOOL32 bIsIgnoreCurState = FALSE );
	TSimCapSet GetHduChnSupportCap(TMt tSrc, u8 byHduId = 0, u8 byMode = MODE_BOTH);
	void   StopViewToAllHduVmpSwitch(TMt tMt);

	// [5/25/2011 liuxu] 添加Vcs电视墙操作的相关接口
	// 根据索引(会议中配置的hdu通道的索引)获取hdu通道状态
	BOOL32 GetConfHduChnnlInfo( const u8 byChnnlIdx, THduChnStatus& tHduChnnStatus, THduMember& tHduMember);
	
	// 根据会议配置的hdu通道号获取对应的通道(hdu)
	BOOL32 GetConfHduChnnl( const u8 byChnnlIdx, CConfTvwChnnl& cChnnl);
	// 根据会议配置的普通电视墙通道号获取对应的通道(tvwall)
	BOOL32 GetConfTvwChnnl( const u8 byChnnlIdx, CConfTvwChnnl& cChnnl);
	// 根据会议配置的电视墙编号获取对应的电视墙通道(hdu or tvwall)
	BOOL32 GetCfgTvwChnnl( const u8 byChnnlIdx, CConfTvwChnnl& cChnnl);

	// 根据设备号和通道号找该电视墙通道在会议中对应的通道号
	u8 GetConfCfgedTvwIdx( const u8 byEqpId, const u8 byTvwIdx );

	// 寻找为某个下级mcu服务的hdu通道. 下一步将会把以下几个接口统一为FindTvwChnnlByMcu
	u8 FindHduChnnlByMcu( const TMt& tSubMt, CConfTvwChnnl* pacChnnl = NULL, const u8 byChnnlNum = 0);
	// 寻找为某个下级mcu服务的tvwall通道. 下一步将会把以下几个接口统一为FindTvwChnnlByMcu
	u8 FindTvwChnnlByMcu( const TMt& tSubMt, CConfTvwChnnl* pacChnnl = NULL, const u8 byChnnlNum = 0);
	// 寻找为某个下级mcu服务的电视墙通道, 搜索顺序是hdu->tvwall->hdtvall
	u8 FindAllTvwChnnlByMcu( const TMt& tSubMt, CConfTvwChnnl* pacChnnl = NULL, const u8 byChnnlNum = 0);

	// 寻找源为tSrc的HDU电视墙通道
	u8 FindHduChnnl( const TMt& tSrc, CConfTvwChnnl* pacChnnl, const u8 byChnnlNum );
	// 寻找源为tSrc的Tvwall电视墙通道
	u8 FindTvwChnnl( const TMt& tSrc, CConfTvwChnnl* pacChnnl, const u8 byChnnlNum );
	// 寻找源为tSrc的所有电视墙通道
	u8 FindAllTvwChnnl( const TMt& tSrc, CConfTvwChnnl* pacChnnl, const u8 byChnnlNum );

	// 获取会议中配置的所有电视墙通道
	u8 GetAllCfgedTvwChnnl( CConfTvwChnnl* pacChnnl, const u8 byChnnlNum );

	// 根据设备号和通道号获取通道
	BOOL32 GetTvwChnnl( const u8 byTvwEqpId, const u8 byChnnlIdx, CTvwChnnl& cChnnl );
	
	// 终端是否需要改变分辨率(终端进入或退出普通 Tvwall设备时用)
	BOOL32 NeedChangeVFormat( const TMt& tMt );

	// 寻找下一个电视墙通道
	u16 FindNextTvwChnnl( const u16 wCurChnnlIdx, 
		const TMt* ptNextMt = NULL, 
		CConfTvwChnnl* pcNextChnnl = NULL,
		const BOOL32 bUseSingleSpyFirst = TRUE);
	// [5/25/2011 liuxu] VCS电视墙相关接口添加完毕

	// [pengjie 2011/9/21] 刷新某终端的电视墙状态，支持刷新各类电视墙
	void RefreshMtStatusInTw( TMt tMt, BOOL32 bInTw, BOOL32 bTypeHdu, u8 byHduMemberType = TW_MEMBERTYPE_NULL );
	// [10/12/2011 liuxu] 判断下级终端是否是电视墙通道的mcu成员的上传成员
	BOOL32 IsSMcuViewMtAndInTw( const TMt&, const BOOL32 IsHdu );
	
	// 刷新其它电视墙通道状态, 单回传时需要将其它同级终端所在电视墙通道更新
	void UpdateTvwOnSMcuViewMtChg( const TMt& tNewSMcuViewMt, const BOOL32 bHduEqp, const TMt* ptOldViewMt = NULL );
	
	// 上传通道终端改变时更新监控通道终端信息
	void UpdateMontiorOnSMcuViewMtChg( const TMt& tNewSMcuViewMt);
	// [7/21/2011 liuxu] 与hdu/tvwall相关的与任何会议模式相关的的操作定义
	// 按照通道成员类型或成员来停止到所在的HDU通道的交换
	void FindConfHduHduChnnlAndStop( TMt* const ptTvwMember = NULL, 
		const u8 byTvwMemberType = TW_MEMBERTYPE_NULL,
		const BOOL32 bStopRunning = FALSE );

	// 遍历所有电视墙通道,如果其成员符合ptOldTvwMember或byTvwMemberType,则更改交换
	void FindConfHduChnnlAndReplace( TMt* const ptNewTvwMember,
		TMt* const ptOldTvwMember = NULL, 		
		const u8 byTvwMemberType = TW_MEMBERTYPE_NULL,
		const u8 byMediaMode = MODE_BOTH );
	// [7/21/2011 liuxu] Over

	void FindConfTvWallChnnlAndStop( TMt* const ptTvwMember = NULL, 
		const u8 byTvwMemberType = TW_MEMBERTYPE_NULL,
		const BOOL32 bStopRunning = FALSE );

	// [pengjie 2010/8/26] 三级电视墙轮询
	BOOL32 ProcHduBatchPollSwitch( TMt tFstMt, TMt tNextMt, 
		const THduChnlPollInfo &tHduChnlPollInfo, 
		const BOOL32 bFirstCycle,
		BOOL32 bIsPoll = FALSE);

	BOOL32 GetNextNeedPollMt( const TMt &tCurrMt, TMt &tNextMt );
	// End

	// 修改电视墙批量轮询Bug新增接口
	BOOL32 FindBatchPollMt(TMt& tCurrt, BOOL32 bIsFindNext = FALSE);

	BOOL32 HasUnLocalMemberInEqp(u8 byEqpId, u8 byMemberType = 0); 

	// 11、SMS
	void ProcSmsPackTimer( void );

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
// 	BOOL32 StartSwitchToSubMt(TMt tSrc, 
//                               u8 bySrcChnnl,
//                               u8 byDstMtId,
//                               u8 byMode = MODE_BOTH,
//                               u8 bySwitchMode = SWITCH_MODE_BROADCAST,
//                               BOOL32 bMsgStatus = TRUE,
//                               BOOL32 bSndFlowCtrl = TRUE,
//                               BOOL32 bIsSrcBrd = FALSE,
// 							  u16 wSpyPort = SPY_CHANNL_NULL );
    //建立1对n Rtp交换
	BOOL32 StartSwitchToAll( TMt tSrc, 
							const u8 bySwitchGrpNum,
							TSwitchGrp *pSwitchGrp,
							u8 byMode,
							u8 bySwitchMode = SWITCH_MODE_BROADCAST,
							BOOL32 bMsgStatus = TRUE,
							BOOL32 bSndFlowCtrl = TRUE,
							u16 wSpyStartPort = SPY_CHANNL_NULL,
							BOOL32 bInnerSelect = TRUE);
	//zyl 20121116 处理选看的mcsdragmod
	void AddRemoveMcsDragMod(const TMt& tUnlocalSrc,
							const u8 &byDstMtId,
							const u8& byMediaMode, 
							u8 bySwitchMode = SWITCH_MODE_BROADCAST,
							u16 wSpyStartPort = SPY_CHANNL_NULL,
							BOOL32 bMsgStatus = TRUE,
							BOOL32 bInnerSelect = TRUE);

	
// 	void   StopSwitchToSubMt( u8 byDstMtId, 
// 							  u8 byMode = MODE_BOTH,
// 							  u8 bySwitchMode = SWITCH_MODE_BROADCAST,
// 							  BOOL32 bMsgStatus = TRUE, 
// 							  BOOL32 bStopSelByMcs = TRUE,
// 							  BOOL32 bSelSelf = TRUE,
// 							  u16 wSpyPort = SPY_CHANNL_NULL );	

	void   StopSwitchToSubMt(u8 byMtNum, 
							TMt *ptDstMt,
							u8 byMode, 
		                    BOOL32 bMsgStatus = TRUE,
							BOOL32 bStopSelByMcs = TRUE, 
							BOOL32 bSelSelf = TRUE, 
							u16    wSpyPort = SPY_CHANNL_NULL);

	BOOL32 StartSwitchToMcu(TMt tSrc, u8 bySrcChnnl, u8 byDstMtId, u8 byMode = MODE_BOTH,u8 bySwitchMode = SWITCH_MODE_BROADCAST,BOOL32 bMsgStatus = TRUE );
	BOOL32 RestoreAllSubMtJoinedConfWatchingSrcMt(TMt tSrc, u8 byMode = MODE_BOTH,BOOL32 bIsStopSel = TRUE );	
    BOOL32 StopSpyMtCascaseSwitch( u8 byMode = MODE_BOTH );

	//建交换到需要适配到的终端
    BOOL32 StartSwitchToSubMtNeedAdp(const TEqp &tEqp, u8 byChnId, BOOL32 bIsConsiderBiteRate = FALSE);
	//拆除到需要适配的终端交换
    BOOL32 StopSwitchToSubMtNeedAdp(const TEqp &tEqp, u8 byChnId,BOOL32 bSelSelf = TRUE);
    //根据终端接收能力建bas到它的交换
    BOOL32 StartSwitchToSubMtFromAdp(u8 byMtId, u8 byMediaMode);
	//建立对应模式的广播适配交换
	BOOL32 StartAllBasSwitch(u8 byMediaMode);
	//拆除对应模式的广播适配交换
	BOOL32 StopAllBasSwitch(u8 byMediaMode);


    // 唇音同步
	BOOL32 StartSwitchAud2Perieqp(TMt tSrc);
    BOOL32 StartSwitchAud2MtNeedAdp(TMt tSrc, TMt tDst, u8 bySwitchMode = SWITCH_MODE_BROADCAST);
	BOOL32 GetAudBufOutPutInfo(TEqp &tBas, u8 &byChnId, u8 &byOutNum, u8 &byOutIdx);

	//zjj20090925 补建唇音同步状态下bas到各个终端的音频交换
	BOOL32 StartSwitchAud3MtNeedAdp( u8 bySwitchMode = SWITCH_MODE_BROADCAST );


    // guzh [3/20/2007]终端广播交换相关
    BOOL32 StartSwitchFromBrd(const TMt &tSrc, u8 bySrcChnnl, u8 byDstMtNum, const TMt* const ptDstBase, u16 wSpyStartPort = SPY_CHANNL_NULL);
    BOOL32 StartSwitchToAllSubMtJoinedConf( const TMt &tSrc, u8 bySrcChnnl, u16 wSpyStartPort = SPY_CHANNL_NULL );     // 音频
    BOOL32 StartSwitchToAllSubMtJoinedConf( const TMt &tSrc, u8 bySrcChnnl, const TMt &tOldSrc, BOOL32 bForce, u16 wSpyStartPort = SPY_CHANNL_NULL ); // 视频   
	BOOL32 StartSwitchToAllNeedAdpWhenSrcChanged(TMt tSrc, u8 byDstNum, TMt *ptDstMt, u8 byMode);

	void RestoreRcvMediaBrdSrc( u8 byMtId, u8 byMode = MODE_BOTH, BOOL32 bMsgMtStutas = TRUE, u16 wSpyStartPort = SPY_CHANNL_NULL );
	void RestoreRcvMediaBrdSrc( u8 byMtNum, TMt *ptDstMt, u8 byMode = MODE_BOTH, 
		                      BOOL32 bMsgMtStutas = TRUE, u16 wSpyStartPort = SPY_CHANNL_NULL );
	void RestoreRcvSecMediaBrdSrc(u8 byMtNum, TMt *ptDstMt, u8 byMode = MODE_VIDEO2SECOND, BOOL32 bMsgMtStutas = TRUE, u16 wSpyStartPort = SPY_CHANNL_NULL);
	
	void SwitchSrcToDst(
		const TSwitchInfo & tSwitchInfo,
		const CServMsg & cServMsgHdr,
		BOOL32 bBatchPollChg = FALSE,
		const THduSwitchInfo *ptHduSwitchInfo = NULL,
		const u8 byMulitModeHdu = HDUCHN_MODE_ONE);
 
	void StopSwitchToAllSubMtJoinedConf( u8 byMode = MODE_BOTH );
    void ProcVidBrdSrcChanged( CMessage * const pcMsg );
    BOOL32 CanMtRecvVideo(const TMt &tDst, const TMt &tSrc);

	//2010/07/13 lukunpeng 根据录像外设状态获取终端相应的外设信息
	BOOL32 GetMtRecordInfo( const TMt &tMt, TEqp& tRecEqp, u8& byRecChannel );
	BOOL32 RemoveMtFromRec(const TMt* ptMt = NULL, const BOOL32 bStopAll = FALSE );
public: //给MpManager可用
	BOOL32 GetMtStatus(const TMt& tMt, TMtStatus& tMtStatus);

	void SetMtStatus(const TMt& tMt,  const TMtStatus& tMtStatus);
protected:
	//2、外设交换
	BOOL32 StartSwitchToPeriEqp(TMt tSrc,
                                u16 wSrcChnnl,
                                u8  byEqpId,
                                u8  byDstChnnl           = 0,
                                u8  byMode              = MODE_BOTH,
                                u8  bySwitchMode        = SWITCH_MODE_BROADCAST,
                                BOOL32 bDstHDBas        = FALSE,
                                BOOL32 bStopBeforeStart = TRUE,
                                BOOL32 bBatchPollChg    = FALSE,
								BOOL32 bAddRtcp			= TRUE,
								BOOL32 bSendMsgToEqp    = TRUE,
								const u8 byHduSubChnId  = 0,
								const u8 byMulitModeHdu = HDUCHN_MODE_ONE);

	void StopSwitchToPeriEqp( u8 byEqpId, u8 byDstChnnl = 0, BOOL32 bMsg = TRUE,
			u8 byMode = MODE_BOTH,u8 bySwitchMode = SWITCH_MODE_BROADCAST, u8 byHduSubChnId = 0); 
	void StopSwitchToAllPeriEqpWatchingSrcMt( TMt tSrc, BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH );
	//void StopSwitchToAllPeriEqpWatchingSrcMcu(  u16 wMcuId, BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH );
	void StopSwitchToAllPeriEqpInConf( BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH );
	void GetHduMemberRtcpAddrInfo(const TMt tSrc, 
									const u8 bySrcChl,
								  const u8 byMode, 
								  u32 &dwVidIpAddr, 
								  u16 &wVidPort, 
								  u32 &dwAudIpAddr, 
								  u16 &wAudPort);

	//3、会控交换
	BOOL32 StartSwitchToMc( TMt tSrc, u8 bySrcChnnl, u16 wMcInstId, u8 byDstChnnl = 0, u8 byMode = MODE_BOTH, u16 wSpyPort = SPY_CHANNL_NULL );
	void StopSwitchToMc( u16 wMcInstId,  u8 byDstChnnl = 0, BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH, BOOL32 bFilterConn = TRUE );
	void StopSwitchToAllMcInConf( BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH );
	void AdjustSwitchToAllMcWatchingSrcMt( TMt tSrc, BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH,BOOL32 bIsStop=TRUE,u8 byChl = 0xFF );
	void StopSwitchToAllMcWatchingSrcMcu(  u16 wMcuId, BOOL32 bMsg = TRUE, u8 byMode = MODE_BOTH );
    BOOL32 IsMtInMcSrc(u8 byMtId, u8 byMode);
	u8 GetCanRlsNumInVcsMc(TMt &tMt, u8 byMode/*, u8 byChnl = VCS_VCMT_MCCHLIDX*/);

	BOOL32	StartSwitchToMonitor(TSwitchDstInfo &tSwitchDstInfo, u16 wMcInstId);
	void	NotifyUIStartMonitor(TSwitchDstInfo &tSwitchDstInfo, u16 wMcInstId);
	BOOL32  StopSwitchToMonitor(TSwitchDstInfo &tSwitchDstInfo, u16 wMcInstId, BOOL32 bIsOnlySwitch = FALSE);

	void    StopSwitchToAllMonitor(BOOL32 bConf = FALSE);
	BOOL32  StopSwitchToMonitor(u16 wMcInstId, BOOL32 bConf = FALSE);
	void	AdjustSwitchToMonitorWatchingSrc(const TMt &tSrc,u8 byMode = MODE_NONE,BOOL32 bIsStop = TRUE,BOOL32 bIsOnlySwitch = FALSE);
    
	//4、桥交换
    BOOL32 AdjustSwitchBridge(u8 byMpId, BOOL32 bInsert);
	void   Proc8khmMultiSwitch(); // 8000H-M特殊处理：随时切换组播地址 [pengguofeng 6/6/2013]

	//5、多回传
	BOOL32 AddSpyChnnlInfo(const TMt &tMt, s16 &swIndex,BOOL32 bIsPreAdd = FALSE);

	void NotifyMcsCriMacAddrInfo( void ); //通知MCS所有接入功能单板Mac地址信息
	void DaemonProcMcsMcuGetCriMacAddrInfoReq(  const CMessage * pcMsg ); //处理MCS获取所有接入功能单板Mac地址信息的请求

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
    /*                          八、打印相关                                */
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
	void ConfPrint( const u8 byLevel, const u16 wModule, const s8* pszFormat, ...);
	void ShowVmpAdaptMember( void );

	void ShowConfMix( void );

	void ShowConfMultiSpy( void );
// 	void ShowMultiSpyMMcuInfo( void );
// 	void ShowMultiSpySMcuInfo( void );

	void ShowConfMcu( void );
	void ShowConfMc(void);//zhouyiliang 20101216打印会议所有mc
	void ShowMcOtherTable(void);

};

typedef zTemplate< CMcuVcInst, MAXNUM_MCU_CONF, CMcuVcData, sizeof( CConfId ) > CMcuVcApp;	//Daemon实例用于处理非会议命令

extern  CMcuVcApp	        g_cMcuVcApp;	//MCU业务应用实例
extern  CAgentInterface     g_cMcuAgent;    
extern  CMSManagerSsnApp    g_cMSSsnApp;
extern  CMpManager          g_cMpManager;   //MP管理类全局变量

#define STATECHECK \
if( CurState() != STATE_ONGOING )\
{\
	ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Wrong message %u(%s) received in state %u at Line.%u!\n", \
		       pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), __LINE__ );\
	return;\
}

// 消除pclint对ISGROUPMODE的错误号为666的检查--byMode如果传入的是一个等式时,会报宏的副作用的warning
/*lint -save -esym(666, ISGROUPMODE)*/
#define ISGROUPMODE(byMode) \
((byMode >= VCS_GROUPSPEAK_MODE && byMode <= VCS_GROUPROLLCALL_MODE) ? TRUE : FALSE)

// [8/8/2011 liuxu] 创建会议时对会议new指针进行检查, 是空则nack
#ifndef START_CONF_CHECK_MEMORY
#define START_CONF_CHECK_MEMORY( p, m, c )	\
	do{										\
	if(!p){								\
	m.SetErrorCode( ERR_MCU_CONFOVERFLOW );				\
	SendReplyBack( m, m.GetEventId() + 2 );		\
	printf("Conference %s failure because no memory for %s\n", c.GetConfName(), #p);			\
	ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s failure because No memory for %s!\n", c.GetConfName(), #p );		\
	return;			\
	}\
	}while(0)
#endif

#endif

//END OF FILE

