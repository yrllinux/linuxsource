

#ifndef _PA_STRUCT_H_
#define _PA_STRUCT_H_

#include <memory.h>
#include <time.h>
#include "kdvtype.h"
#include "patype.h"
#include "pamacro.h"
#include <string.h>
#include <stdlib.h>

//2012-3-6 add-by wuhu 构造TPAAudChanCmd 结构
#define TP_MAX_EPID_LEN			64				//级联时，ID最大字符串长度
#define SEPCHAR '.'
#define TP_INVALID_INDEX		0xFFFF
#define TP_MAX_STREAMNUM		3				//会场最大发言人数量
#define TP_RET_OK	            0
#define TP_CONF_MAX_TURNNUM		64				//会议最大轮询数量

typedef struct tagTYouAreSeeing
{
	u32 m_dwVideoNum;//视频发送数量
	u32 m_dwAudioNum;//音频发送数量
	u16 m_CNSId;//CNSID
	u16 m_wMcuId;//323mcuId
	BOOL32 m_bExist;//是否存在
	s8  m_achMeetingName[PA_MAX_CONF_NAME_LEN+1];//发言人会场名
	u16 m_wSpeakerIndex;
	tagTYouAreSeeing()
	{
		memset( this, 0, sizeof(tagTYouAreSeeing) );
	}
}TYouAreSeeing,*PTYouAreSeeing;

typedef struct tagTPAIPAddress
{
public:
	u32_ip m_dwIP ;
	u16 m_wPort;

	void Clear()
	{
		m_dwIP = 0;
		m_wPort = 0;
	}
	tagTPAIPAddress()
	{
		Clear();
	}
}TPAIPAddress,*PTPAIPAddress;

typedef struct tagTPAConfStatusInd
{
	u16 m_wMeetingId;//会场ID	
	s8  m_achConfName[PA_MAX_H323ALIAS_LEN+1];

	time_t m_tBeginTime;
	s8	m_achChairName[PA_MAX_H323ALIAS_LEN+1];	

	s8	m_achSpeakName[PA_MAX_H323ALIAS_LEN+1];	
	u16	m_wChairVidSndCap;
	u16 m_wChairAudSndCap;
	BOOL32   m_bMixMotive;          // 是否启用语音激励
	BOOL32   m_bSpeakOnline;        // 发言人是否在线

	tagTPAConfStatusInd()
	{
		memset( this, 0, sizeof(tagTPAConfStatusInd) );
	}

}TPAConfStatusInd,*PTPAConfStatusInd;

typedef struct tagTPAPollNodeChange
{
	s8  m_achNodeInfo[PA_NODECHANGEINFO+1];
	tagTPAPollNodeChange()
	{
		memset( this, 0, sizeof(tagTPAPollNodeChange) );
	}
}TPAPollNodeChange, *PTPAPollNodeChange;

typedef struct tagTPACascadingSpeaker
{
	s8  m_achNodeKey[PA_NODECHANGEINFO+1];
	u16 m_wEpID;
	u16 m_wSpeaker;
	tagTPACascadingSpeaker()
	{
		memset( this, 0, sizeof(tagTPACascadingSpeaker) );
	}
}TPACascadingSpeaker;


typedef struct tagTPAPeerMuteCmd
{
	BOOL32 m_bMute;
	u16    m_wEpId;
	s8 m_achPeerMute[PA_MAX_H323ALIAS_LEN+1];
	tagTPAPeerMuteCmd()
	{
		memset( this, 0, sizeof(tagTPAPeerMuteCmd) );
	}	
}TPAPeerMuteCmd,*PTPAPeerMuteCmd;

typedef struct tagTPAPeerMuteInd
{
	BOOL32 m_bMute;
	s8 m_achPeerMute[PA_MAX_H323ALIAS_LEN+1];
	tagTPAPeerMuteInd()
	{
		memset( this, 0, sizeof(tagTPAPeerMuteInd) );
	}
}TPAPeerMuteInd,*PTPAPeerMuteInd;

typedef struct tagTHangupCascadingConf
{
	u16 m_wEpId;
	BOOL32 m_bClear;
	tagTHangupCascadingConf()
	{
		memset( this, 0, sizeof(tagTHangupCascadingConf) );
	}
}THangupCascadingConf,*PTHangupCascadingConf;


typedef struct tagTPAName
{
	s8				m_abyAlias[PA_MAX_NAME_LEN+1];
	tagTPAName()
	{
		memset( this, 0, sizeof(tagTPAName) );
	}
	void SetAlias(const s8* alias)
	{
		u16 len = strlen(alias)+1;
		len = min(len, PA_MAX_NAME_LEN);
		memcpy(m_abyAlias, alias, len);
		m_abyAlias[len] = '\0';
	}
}TPAName;

typedef struct tagTPAPollDstName
{
	s8				m_abyAlias[PA_MAX_NAME_LEN+1];
	u32             m_nScreenNum;
	tagTPAPollDstName()
	{
		memset( this, 0, sizeof(tagTPAPollDstName) );
	}
	void SetAlias(const s8* alias)
	{
		u16 len = strlen(alias)+1;
		len = min(len, PA_MAX_NAME_LEN);
		memcpy(m_abyAlias, alias, len);
		m_abyAlias[len] = '\0';
	}
}TPAPollDstName;

typedef struct tagTPAAskKeyByScreenTr
{
	u16		m_wEpID;
	u16		m_wScreenNum;
	BOOL32	m_bReqBySys;

    u32		m_dwAskType;               //请求类型  发言人、主席、选看轮询、讨论、录播、电视墙
    u32		m_dwRervse1;               //保留
	u32		m_dwRervse2;               //保留
}TPAAskKeyByScreenTr;



//别名定义
typedef struct tagTPAAlias
{
	EmPAAliasType	m_byType;     
	s8				m_abyAlias[PA_MAX_NAME_LEN+1];
	tagTPAAlias()
	{
		Clear();
	}
	void Clear()
	{
		memset( this, 0, sizeof(tagTPAAlias) );
	}
	void SetAlias(EmPAAliasType type, const s8* alias)
	{
		m_byType = type;
		strncpy(m_abyAlias, alias, PA_MAX_NAME_LEN);
		m_abyAlias[PA_MAX_NAME_LEN] = '\0';
	}
	
	
}TPAAlias, *PTPAAlias;


typedef struct tagTPAEpKeyID
{
	s8	m_szEpID[PA_MAX_EPID_LEN];
	
	tagTPAEpKeyID()
	{
		memset( this, 0, sizeof(tagTPAEpKeyID) );
	}
}TPAEpKeyID,*PTPAEpKeyID;

typedef struct tagTPAUmsVidFormat
{
	EmPAVideoFormat		m_emFormat;
	EmPAVideoResolution	m_emRes;
	EmPAProfileMask	m_emProfile;//profile
	u16				m_byFrameRate;	//帧率
	u16				m_wBitrate;
	
	tagTPAUmsVidFormat()
	{
		Clear();
	}
	void Clear()
	{
		memset(this, 0, sizeof(tagTPAUmsVidFormat));
	}
}TPAUmsVidFormat;

typedef struct tagTPAUmsAudFormat
{
	EmPAAudioFormat		m_emFormat;
	EmPAAACSampleFreq	m_emPASampleFreq;
	EmPAAACChnlCfg		m_emPAChnlCfg;
	u8				m_byReserve1;
	u8				m_byReserve2;
	
	tagTPAUmsAudFormat()
	{
		Clear();
	}
	void Clear()
	{
		memset(this, 0, sizeof(tagTPAUmsAudFormat));
	}
}TPAUmsAudFormat;

//下级向上级更新结点信息
typedef struct tagTPAConfNodeUpdateTr
{
	TPAEpKeyID m_tID;						//树状表
	u16		 m_wEpID;						//在下级中的ID
	
	u32	m_emEpType;			//结点类型
	u32	m_emCallState;		//在线状态
	u32	m_emCallReason; 
	
	TPAAlias m_tCalledAddr;					//呼叫此结点时的地址
	TPAAlias m_tConfName;					//会场名
	
	u16	m_wLevel;				//会场等级	保留
	u32 m_enRcvAlignType;
	
	BOOL32 m_bMute;
	BOOL32 m_bQuiet;
	
	//索引按照五个通道 0,1,2,3,4排序，不管本会场真实是几个编码器
	u16 m_wSpearkIndex;			//发言人通道索引
	u16 m_wDualIndex;			//发送双流通道索引

	tagTPAConfNodeUpdateTr()
	{	
		memset( this, 0, sizeof(tagTPAConfNodeUpdateTr) );
	}
}TPAConfNodeUpdateTr,*PTPAConfNodeUpdateTr;

//通知更新结点信息
typedef struct tagTPAConfNodeUpdateTrList
{
	u16		m_wNum;
	TPAConfNodeUpdateTr m_atList[PA_REFRESH_LIST_THREE];
	tagTPAConfNodeUpdateTrList()
	{
		m_wNum = 0;
	}
}TPAConfNodeUpdateTrList,*PTPAConfNodeUpdateTrList;

typedef struct tagTPAConfNodeUpdateB2NEWTr
{
	TPAEpKeyID tID;						//树状表
	u16		 wEpID;						//在下级中的ID
	BOOL32	bChanOpen;					//通道是否打开

	//B2新增
	u16	wVidSnd;		
	u16	wVidRcv;	
	u16	wAudSnd;	
	u16	wAudRcv;
	
	TPAUmsVidFormat	tVidRcv;
	TPAUmsAudFormat	tAudRcv;
	
	TPAUmsVidFormat	tVidSnd;
	TPAUmsAudFormat	tAudSnd;

	TPAUmsVidFormat	tDualVidRcv;		//接收能力
	TPAUmsAudFormat	tDualAudRcv;
	
	TPAUmsVidFormat	tDualVidSnd;
	TPAUmsAudFormat	tDualAudSnd;

	//载荷在数组中依次顺序是视频接收 视频发送 双流视频接收 双流视频发送 音频接收 音频发送 双流音频接收 双流音频发送
	u16 awPd[PA_MAX_CHAN_NUM]; 
	
	//
	tagTPAConfNodeUpdateB2NEWTr()
	{	
		memset( this, 0, sizeof(tagTPAConfNodeUpdateB2NEWTr) );
	}
	
}TPAConfNodeUpdateB2NEWTr;

typedef struct tagTPAConfNodeUpdateB2NEWTrList
{
	u16		m_wNum;
	TPAConfNodeUpdateB2NEWTr m_atList[PA_REFRESH_LIST_THREE];
	tagTPAConfNodeUpdateB2NEWTrList()
	{
		m_wNum = 0;
	}
}TPAConfNodeUpdateB2NEWTrList;

//通知删除结点
typedef struct tagTPAConfNodeDelTrList
{
	u16		m_wNum;
	TPAEpKeyID m_atList[PA_REFRESH_LIST_FIVE];
	tagTPAConfNodeDelTrList()
	{
		memset( this, 0, sizeof(tagTPAConfNodeDelTrList) );
	}
}TPAConfNodeDelTrList,*PTPAConfNodeDelTrList;


typedef struct tagTPAAudMixEpCmdTr
{
	u16	m_wEpID;
	u16 m_wLocalID;					//上级的数组索引
	TPAIPAddress m_tRtpAddr;		//上级接收EP码流
	TPAIPAddress m_tBackRtpAddr;	//混音后码流的BACKRTP地址
	tagTPAAudMixEpCmdTr()
	{
		m_tRtpAddr.Clear();
		m_tBackRtpAddr.Clear();
		m_wEpID = 0xffff;
		m_wLocalID = 0xffff;
	}
}TPAAudMixEpCmdTr,*PTPAAudMixEpCmdTr;

typedef struct tagTPAAudMixEpAckTr
{
	u16 m_wReason;				//原因
	u16 m_wLocalID;
	TPAIPAddress m_tRtpAddr;		//接收上级混音后的码流
	TPAIPAddress m_tBackRtpAddr;	//EP码流的backrtp地址
	tagTPAAudMixEpAckTr()
	{
		m_wReason = 0;
		m_tRtpAddr.Clear();
		m_tBackRtpAddr.Clear();
		m_wLocalID = 255;
	}
}TPAAudMixEpAckTr,*PTPAAudMixEpAckTr;

typedef struct tagTDualInfo
{
	EmPAPayloadType m_emPayload;
	EmPAResolution  m_emReslution;
	u16				m_wFrameRate;
	u16             m_wBitRate;
	s32 m_nScreenNo;
	s32 m_nEpId;
	s8  m_achCascade[PA_MAX_H323ALIAS_LEN+1];
	TPAIPAddress m_tVidRtp;
	TPAIPAddress m_tVidRtcp;
	TPAIPAddress m_tVidBackRtp;

	TPAIPAddress m_tAudRtp;
	TPAIPAddress m_tAudRtcp;
	TPAIPAddress m_tAudBackRtp;

	tagTDualInfo()
	{
		memset( this, 0, sizeof(tagTDualInfo) );
	}

}TDualInfo,*PTDualInfo;

typedef struct tagTDualInfoAck
{
	BOOL32    m_bAccept;
	TDualInfo m_tDualInfo;
	tagTDualInfoAck()
	{
		memset( this, 0, sizeof(tagTDualInfoAck) );
	}
}TDualInfoAck,*PTDualInfoAck;

typedef struct tagTDualRequest
{
	s8 m_achCascade[PA_MAX_H323ALIAS_LEN+1];
	s32 m_nEpId;
	tagTDualRequest()
	{
		memset( this, 0, sizeof(tagTDualRequest) );
	}
}TDualRequest,*PTDualRequest;


typedef struct tagTPATvMonitorCmd
{
	s8				m_achCascade[PA_MAX_H323ALIAS_LEN+1];
	u16				m_wIndex;
	u16				m_wEpID;
	TPAIPAddress	m_tVidRtp;
	TPAIPAddress	m_tVidRtcp;
	TPAIPAddress	m_tVidBackRtp;
	
	TPAIPAddress	m_tAudRtp;
	TPAIPAddress	m_tAudRtcp;
	TPAIPAddress	m_tAudBackRtp;

	u16 m_wScreenNum;
	
	tagTPATvMonitorCmd()
	{
		memset( this, 0, sizeof(tagTPATvMonitorCmd) );
		m_wScreenNum = 0xffff;
	}
}TPATvMonitorCmd;

typedef struct tagTPATvMonitorInd
{
	BOOL32			m_bAccept;
	s8				m_achCascade[PA_MAX_H323ALIAS_LEN+1];
	u16				m_wIndex;
	u16				m_wEpID;
	TPAIPAddress	m_tVidRtp;
	TPAIPAddress	m_tVidRtcp;
	TPAIPAddress	m_tVidBackRtp;
	
	TPAIPAddress	m_tAudRtp;
	TPAIPAddress	m_tAudRtcp;
	TPAIPAddress	m_tAudBackRtp;

    u16 m_wScreenNum;

	tagTPATvMonitorInd()
	{
		memset( this, 0, sizeof(tagTPATvMonitorInd) );
		m_wScreenNum = 0xffff;
	}
}TPATvMonitorInd;


typedef struct tagTPANodeCapsetTr
{
	u16 m_wSpeakindex;
	tagTPANodeCapsetTr()
	{
		memset( this, 0, sizeof(tagTPANodeCapsetTr) );
	}
}TPANodeCapsetTr;

//addy-by wuhu 2011-11-1  构造TPAConfCnsList结构

typedef struct tagTPaMediaTransAddr
{
	TPAIPAddress m_tRtpAddr;
	TPAIPAddress m_tRtcpAddr;	
	TPAIPAddress m_tBackRtcpAddr;	
}TPaMediaTransAddr;


#define PA_REFRESH_LIST_FIVE	5
#define PA_MAX_ALIAS_LEN		64				//别名最大长度
#define PA_MAX_STREAMNUM		3				//会场最大发言人数量


typedef struct tagTPACnsInfo
{
	u16	m_wEpID;							//会场ID
	u16	m_wParentEpID;						//父会场ID
	u16 m_wChildEpID;
	u16 m_wBrotherEpID;
	
	EmPATPEndpointType m_emPAEpType;			//结点类型
	TPAAlias m_tCalledAddr;
	
	BOOL32	m_bOnline;			            //在线状态
	EmPACnsCallReason m_emPACallReason; 
	s8	m_achRoomName[PA_MAX_ALIAS_LEN+1];	//会场名
	u16	m_wLevel;							//会场等级	保留
	
	BOOL32 m_bMute;
	BOOL32 m_bQuiet;
	
	u16 m_wSpeakerNum;						//会场屏数	
	s8	m_achSpeakerName[PA_MAX_STREAMNUM][PA_MAX_ALIAS_LEN+1];
	
	tagTPACnsInfo()
	{	
		Clear();
	}
	
	void Clear()
	{
		m_wEpID = PA_INVALID_VALUE;
		m_wParentEpID = PA_INVALID_VALUE;
		m_wChildEpID = PA_INVALID_VALUE;
		m_wBrotherEpID = PA_INVALID_VALUE;
		m_emPAEpType = emPATPEndpointTypeUMS;
		
		m_bOnline = FALSE;
		m_achRoomName[0] = '\0';
		m_wLevel = PA_INVALID_VALUE;
		
		m_bMute = FALSE;
		m_bQuiet = FALSE;
		m_wSpeakerNum = PA_INVALID_VALUE;
		m_emPACallReason = EmPACnsCallReason_Local;
	}
	
	BOOL32 operator == (const tagTPACnsInfo& tRhs)
	{
		return this->m_wEpID == tRhs.m_wEpID;
	}
}TPACnsInfo;

typedef struct tagTConfCnsList
{
	EmPAOprType	    m_emPAOpr;
	u16             m_wMcuId;
	u16			    m_wConfID; 
	u16		    	m_wNum;
	TPACnsInfo	    m_tConfCnsList[PA_REFRESH_LIST_FIVE];
	tagTConfCnsList()
	{
		m_emPAOpr = PA_OprType_Add;
		m_wConfID = PA_INVALID_VALUE;
		m_wNum = 0;
		m_wMcuId = 0;
	}
}TConfCnsList;

typedef struct tagTPAConfCnsList
{
    TConfCnsList tCnsList;
    tagTPAConfCnsList()
	{
		memset(this, 0, sizeof(tagTPAConfCnsList));
	}
	
}TPAConfCnsList;

typedef struct tagTPAChanSelecteCascad
{
   s32 nPeerID;
   u16 wCascadID;
}TPAChanSelecteCascad;

typedef struct tagTPAChanSelecteCascadRes
{
    u32 nRes;
	s8  achName[PA_MAX_ALIAS_LEN+1];
	tagTPAChanSelecteCascadRes()
	{
		memset(this, 0, sizeof(tagTPAChanSelecteCascadRes));
	}
}TPAChanSelecteCascadRes;

typedef struct tagTPATmpChanSelecteCascadRes
{
	s32 nPeerID;
    u32 nRes;
	s8 achName[PA_MAX_ALIAS_LEN+1];
	tagTPATmpChanSelecteCascadRes()
	{
		memset(this, 0, sizeof(tagTPATmpChanSelecteCascadRes));
	}
}TPATmpChanSelecteCascadRes;

typedef struct tagTPAChanSelecteCascadCancelRes
{
	s32 nPeerID;
    u32 nRes;
}TPAChanSelecteCascadCancelRes;

typedef struct tagTPAPollCmd
{
	TPAIPAddress m_tRtpAddr[PA_MAX_STREAMNUM];
	TPAIPAddress m_tRtcpAddr[PA_MAX_STREAMNUM];	
	TPAIPAddress m_tBackRtcpAddr[PA_MAX_STREAMNUM];
	 
	u16 m_wBitrate;				//轮询码率
	u16	m_wInterval;			//轮询时间间隔
	tagTPAPollCmd()
	{
		m_wBitrate = 0;
		m_wInterval = 0;
	}
}TPAPollCmd;

typedef struct tagTPAPollAck
{
	u16 m_wReason;
	TPAIPAddress m_tRtpAddr[PA_MAX_STREAMNUM];
	TPAIPAddress m_tRtcpAddr[PA_MAX_STREAMNUM];	
	TPAIPAddress m_tBackRtcpAddr[PA_MAX_STREAMNUM];
	tagTPAPollAck()
	{
		m_wReason = 0;
	}
}TPAPollAck;


typedef struct tagTPAViewCmd
{
	u16 m_wEpID;
	u16 m_wChanID;

	TPAIPAddress m_tRtpAddr;
	TPAIPAddress m_tRtcpAddr;	
	TPAIPAddress m_tBackRtcpAddr;

	TPAIPAddress m_tSmallRtpAddr;
	TPAIPAddress m_tSmallRtcpAddr;	
	TPAIPAddress m_tSmallBackRtcpAddr;
	
	TPAIPAddress m_tLeftRtpAddr;
	TPAIPAddress m_tLeftRtcpAddr;	
	TPAIPAddress m_tLeftBackRtcpAddr;

	TPAIPAddress m_tLeftSmallRtpAddr;
	TPAIPAddress m_tLeftSmallRtcpAddr;	
	TPAIPAddress m_tLeftSmallBackRtcpAddr;

	TPAIPAddress m_tRightRtpAddr;
	TPAIPAddress m_tRightRtcpAddr;	
	TPAIPAddress m_tRightBackRtcpAddr;

	TPAIPAddress m_tRightSmallRtpAddr;
	TPAIPAddress m_tRightSmallRtcpAddr;	
	TPAIPAddress m_tRightSmallBackRtcpAddr;

	BOOL32		 m_bMidOpen;
	BOOL32		 m_bSmallMidOpen;

	BOOL32		 m_bLeftOpen;
	BOOL32		 m_bSmallLeftOpen;

	BOOL32		 m_bRightOpen;
	BOOL32		 m_bSmallRightOpen;
	BOOL32		 m_bViewSpeaker;	//是否是仅看发言坐席

	u32          m_dwType;              //获取类型
	u16          m_wSrcScreenIndex;	    //源屏号

	//B2新增
	TPaMediaTransAddr m_atAudAddr[TP_MAX_STREAMNUM];	//音频接收地址	
	TPaMediaTransAddr m_tMixAddr;				//混音接收地址

	BOOL32			  m_bNeedBas;						//是否需要适配

	tagTPAViewCmd()
	{
		m_bMidOpen = FALSE;
		m_bSmallMidOpen = FALSE;
		
		m_bLeftOpen = FALSE;
		m_bSmallLeftOpen = FALSE;

		m_bRightOpen = FALSE;
		m_bSmallRightOpen = FALSE;
		m_bViewSpeaker = FALSE;

		m_dwType = 0;
		m_wSrcScreenIndex = -1;

		m_bNeedBas = FALSE;
	}
}TPAViewCmd;

typedef struct tagTPAAdjustFrameRate
{
	u16    m_wEpID;
	u16	 m_wFrameRate;
	tagTPAAdjustFrameRate()
	{
		m_wEpID = TP_INVALID_INDEX;
		m_wFrameRate = 0;
	}
}TPAAdjustFrameRate;

typedef struct tagTPAAdjustVidRes
{
	u16    m_wEpID;
	u32	 m_dwVidRes;
	tagTPAAdjustVidRes()
	{
		m_wEpID = TP_INVALID_INDEX;
		m_dwVidRes = 0;
	}
}TPAAdjustVidRes;


typedef struct tagTPAViewAck
{
	u16 m_wReason;
	u16 m_wChanID;

	TPAIPAddress m_tRtpAddr;
	TPAIPAddress m_tRtcpAddr;	
	TPAIPAddress m_tBackRtcpAddr;

	TPAIPAddress m_tSmallRtpAddr;
	TPAIPAddress m_tSmallRtcpAddr;	
	TPAIPAddress m_tSmallBackRtcpAddr;
	
	TPAIPAddress m_tLeftRtpAddr;
	TPAIPAddress m_tLeftRtcpAddr;	
	TPAIPAddress m_tLeftBackRtcpAddr;
	
	TPAIPAddress m_tLeftSmallRtpAddr;
	TPAIPAddress m_tLeftSmallRtcpAddr;	
	TPAIPAddress m_tLeftSmallBackRtcpAddr;
	
	TPAIPAddress m_tRightRtpAddr;
	TPAIPAddress m_tRightRtcpAddr;	
	TPAIPAddress m_tRightBackRtcpAddr;
	
	TPAIPAddress m_tRightSmallRtpAddr;
	TPAIPAddress m_tRightSmallRtcpAddr;	
	TPAIPAddress m_tRightSmallBackRtcpAddr;

	BOOL32		 m_bMidOpen;
	BOOL32		 m_bSmallMidOpen;
	
	BOOL32		 m_bLeftOpen;
	BOOL32		 m_bSmallLeftOpen;
	
	BOOL32		 m_bRightOpen;
	BOOL32		 m_bSmallRightOpen;

	//B2新增
	TPaMediaTransAddr m_atAudAddr[TP_MAX_STREAMNUM];	//音频接收地址	
	TPaMediaTransAddr m_tMixAddr;			      //混音接收地址
		
	tagTPAViewAck()
	{
		m_wReason = 0;

		m_bMidOpen = FALSE;
		m_bSmallMidOpen = FALSE;
		
		m_bLeftOpen = FALSE;
		m_bSmallLeftOpen = FALSE;

		m_bRightOpen = FALSE;
		m_bSmallRightOpen = FALSE;
	}
}TPAViewAck;

typedef struct tagTPACascadeOpenChanCmd
{
	u16 m_wEpID;
	
	TPaMediaTransAddr m_atVidAddr[TP_MAX_STREAMNUM];	//接收地址
	TPaMediaTransAddr m_atAudAddr[TP_MAX_STREAMNUM];	//接收地址
	TPaMediaTransAddr m_atSmallAddr[TP_MAX_STREAMNUM];	//接收地址
	
	BOOL32		 m_abVidOpen[TP_MAX_STREAMNUM];
	BOOL32		 m_abAudOpen[TP_MAX_STREAMNUM];
	BOOL32		 m_abSmallOpen[TP_MAX_STREAMNUM];
	
	tagTPACascadeOpenChanCmd()
	{
		m_wEpID = TP_INVALID_INDEX;
		
		memset(m_abVidOpen, 0, sizeof(m_abVidOpen));
		memset(m_abSmallOpen, 0, sizeof(m_abSmallOpen));
		memset(m_abAudOpen, 0, sizeof(m_abVidOpen));
	}
	
}TPACascadeOpenChanCmd;

typedef struct tagTPACascadeOpenChanAck
{
	u16 m_wEpID;
	
	TPaMediaTransAddr m_atVidAddr[TP_MAX_STREAMNUM];	//接收地址
	TPaMediaTransAddr m_atAudAddr[TP_MAX_STREAMNUM];	//接收地址
	TPaMediaTransAddr m_atSmallAddr[TP_MAX_STREAMNUM];	//接收地址
	
	BOOL32		 m_abVidOpen[TP_MAX_STREAMNUM];
	BOOL32		 m_abSmallOpen[TP_MAX_STREAMNUM];
	BOOL32		 m_abAudOpen[TP_MAX_STREAMNUM];
	
	tagTPACascadeOpenChanAck()
	{
		m_wEpID = TP_INVALID_INDEX;
		
		memset(m_abVidOpen, 0, sizeof(m_abVidOpen));
		memset(m_abSmallOpen, 0, sizeof(m_abSmallOpen));
		memset(m_abAudOpen, 0, sizeof(m_abVidOpen));
	}
	
}TPACascadeOpenChanAck;


typedef struct tagTPAUmsReasonInd
{
    u16	m_wConfID;     //必填
    u16 m_wEpID;       //根据 实际 情况使用
    u32 m_dwReason;    //必填 tperror.h中的原因
    
    u32 m_dwReserve1;  //保留
    u32 m_dwReserve2;
    u32 m_dwReserve3;
    
    tagTPAUmsReasonInd()
    {
        m_wConfID = TP_INVALID_INDEX;
        m_wEpID   = TP_INVALID_INDEX;
        
        m_dwReason = 0;
        
        m_dwReserve1 = 0;
        m_dwReserve2 = 0;
        m_dwReserve3 = 0;
    }
}TPAUmsReasonInd;



#define PA_TP_REC_FILE_LEN  64  //录播文件名最大长度

typedef struct tagTPAUmsUpRecPlayState
{
    BOOL32		m_bIsPlayIng;	//是否在放像
    BOOL32		m_bIsDualPlayIng;	//是否放双流
    s8			m_achFileName[PA_TP_REC_FILE_LEN];	//文件名
    
    tagTPAUmsUpRecPlayState()
    {
        Clear();
    }
    
    void Clear()
    {
        m_bIsPlayIng = FALSE;
        m_bIsDualPlayIng = FALSE;
        memset(m_achFileName, 0, PA_TP_REC_FILE_LEN);
    }
}TPAUmsUpRecPlayState;


typedef struct tagTPAEpPathKey
{
	s8	m_szEpID[TP_MAX_EPID_LEN+1];
	tagTPAEpPathKey()
	{
		Clear();
	}
	void Clear()
	{
		m_szEpID[0] = '\0';
	}
	
	BOOL32 IsRoot() const
	{
		if (0 == memcmp(m_szEpID, "root", 4))
		{
			return TRUE;
		}
		return FALSE;
	}
	
	BOOL32 IsValid() const
	{
		u8 byLast = SEPCHAR;
		u16 wIndex = 0;
		
		if (IsRoot())
		{
			return TRUE;
		}
		while (m_szEpID[wIndex] != '\0')
		{
			if (m_szEpID[wIndex] != SEPCHAR && (m_szEpID[wIndex] < '0' || m_szEpID[wIndex] > '9'))
			{
				return FALSE;
			}
			
			if (m_szEpID[wIndex] == SEPCHAR && m_szEpID[wIndex] == byLast)
			{
				return FALSE;
			}
			
			byLast = m_szEpID[wIndex];
			
			wIndex++;
			
			if (wIndex > TP_MAX_EPID_LEN)
			{
				return FALSE;
			}
		}
		return TRUE;
	}
	
	BOOL32 GetKeyPath(u16 awEpID[], u16 wMaxNum, u16& wRealNum) const 
	{
		wRealNum = 0;
		if (IsRoot())
		{
			return TRUE;
		}
		
		if (!IsValid())
		{
			return FALSE;
		}
		s8	szEpID[TP_MAX_EPID_LEN+1];
		memcpy(szEpID, m_szEpID, TP_MAX_EPID_LEN+1);
		
		s8* pChar = szEpID;
		u16 wIndex = 0;
		while (*(pChar+wIndex) != '\0')
		{
			if (*(pChar+wIndex) == SEPCHAR)
			{
				if (wIndex == 0)
				{
					return FALSE;
				}
				*(pChar+wIndex) = '\0';
				awEpID[wRealNum] = atoi(pChar);
				++wRealNum;
				if (wRealNum >= wMaxNum)
				{
					return FALSE;
				}
				pChar = pChar+wIndex+1;
				wIndex = 0;
				continue;
			}
			wIndex++;
		}
		return TRUE;
	}
}TPAEpPathKey;

typedef struct tagTPAEpKey
{
	u16 m_wEpID;			//上级通知下级时为下级ID
							//下级通知上级时为本地ID即下级ID
	TPAEpPathKey m_tKey;
	tagTPAEpKey()
	{
		m_wEpID = TP_INVALID_INDEX;
		m_tKey.Clear();
	}
	void Clear()
	{
		m_wEpID = TP_INVALID_INDEX;
		m_tKey.Clear();
	}

}TPAEpKey;


typedef struct tagTPAFlowControl
{
	TPAEpKey	m_tEpKey;
	u16	m_wEncNo;	//-1针对所有屏
	u16	m_wBand;
	u32 m_enOpr;
	tagTPAFlowControl()
	{
		m_tEpKey.Clear();
		m_wEncNo = -1;
		m_wBand = 0;
		m_enOpr = 0;
	}
}TPAFlowControl;

typedef struct tagTPAAudChanCmd
{
	TPAEpKey		m_tEpKey;
	TPAIPAddress	m_atRtpAddr[TP_MAX_STREAMNUM];
	TPAIPAddress	m_tMixRtpAddr;
	tagTPAAudChanCmd()
	{
		m_tEpKey.Clear();
		memset(m_atRtpAddr, 0, sizeof(m_atRtpAddr));
		memset(&m_tMixRtpAddr, 0, sizeof(m_tMixRtpAddr) );
	}
	
}TPAAudChanCmd;

typedef struct tagTPAAudChanAck
{
	u16		  	    m_wReason;					//原因
	TPAEpKey		m_tEpKey;
	TPAIPAddress    m_atBackRtcpAddr[TP_MAX_STREAMNUM];
	TPAIPAddress    m_tMixBackRtcpAddr;
	tagTPAAudChanAck()
	{
		m_tEpKey.Clear();
		m_wReason = TP_RET_OK;
		
		memset( m_atBackRtcpAddr, 0, sizeof(m_atBackRtcpAddr) );
		memset( &m_tMixBackRtcpAddr, 0, sizeof(m_tMixBackRtcpAddr) );
	}
	
}TPAAudChanAck;


typedef struct tagTPAVidUpAddr
{
	TPAIPAddress		m_tAddr;
	TPAIPAddress		m_tSmallAddr;
	tagTPAVidUpAddr()
	{
		Clear();
	}
	
	void Clear()
	{
		m_tAddr.Clear();
		m_tSmallAddr.Clear();
	}
}TPAVidUpAddr;

typedef struct tagTPAVidDownAddr
{
	TPAIPAddress	m_tAddr;
	TPAIPAddress	m_tSmallAddrL;
	TPAIPAddress	m_tSmallAddrR;
	tagTPAVidDownAddr()
	{
		Clear();
	}
	
	void Clear()
	{
		m_tAddr.Clear();
		m_tSmallAddrL.Clear();
		m_tSmallAddrR.Clear();
	}
}TPAVidDownAddr;

typedef struct tagTPAVidChanCmd
{
	TPAEpKey				m_tEpKey;
	TPAVidUpAddr			m_atUpRtp[TP_MAX_STREAMNUM];	//上传的Rtp端口
	TPAVidDownAddr		m_atDownBackRtcp[TP_MAX_STREAMNUM]; //下传的BackRtcp端口
	TPAIPAddress		m_atAudBackRtcp[TP_MAX_STREAMNUM];
	
	tagTPAVidChanCmd()
	{
		m_tEpKey.Clear();
		memset(m_atUpRtp, 0, sizeof(m_atUpRtp));
		memset(m_atDownBackRtcp, 0, sizeof(m_atDownBackRtcp) );
		memset(m_atAudBackRtcp, 0, sizeof(m_atAudBackRtcp) );
	}
}TPAVidChanCmd;

typedef struct tagTPAVidChanAck
{
	u16					m_wReason;	//原因
	TPAEpKey				m_tEpKey;
	TPAVidUpAddr			m_atUpBackRtcp[TP_MAX_STREAMNUM];	//上传的BackRtcp端口
	TPAVidDownAddr		m_atDownRtp[TP_MAX_STREAMNUM];		    //下传的Rtp端口
	TPAIPAddress		m_atAudRtp[TP_MAX_STREAMNUM];

	tagTPAVidChanAck()
	{
		m_wReason = TP_RET_OK;
		m_tEpKey.Clear();
		memset(m_atUpBackRtcp, 0, sizeof(m_atUpBackRtcp));
		memset(m_atDownRtp, 0, sizeof(m_atDownRtp));
		memset( m_atAudRtp, 0, sizeof(m_atAudRtp) );
	}
}TPAVidChanAck;


#define TP_CONF_MAX_AUDMIXNUM	4			//最大混音参与数量
typedef struct tagTPAAuxMixList
{
	u16 m_wSpeakerIndex;					//下面数组的索引
	u16	m_awList[TP_CONF_MAX_AUDMIXNUM];	//混音ID
	tagTPAAuxMixList()
	{
		memset(this, 0, sizeof(tagTPAAuxMixList) );
	}
}TPAAuxMixList;

typedef struct tagTPAConfAuxMixInfo
{
	u16			m_wConfID;
	BOOL32		m_bStart;
	TPAAuxMixList	m_atAuxMixList;
	tagTPAConfAuxMixInfo()
	{
		m_wConfID = TP_INVALID_INDEX;
		m_bStart = FALSE;
	}
}TPAConfAuxMixInfo;

typedef struct tagTPAConfEpID
{
	u16	m_wConfID;
	u16	m_wEpID;
	tagTPAConfEpID()
	{
		Clear();
	}
	void Clear()
	{
		m_wConfID = TP_INVALID_INDEX;
		m_wEpID = TP_INVALID_INDEX;
	}
}TPAConfEpID;


typedef struct tagTPAConfTurnList
{
	u16 m_wInterval;						//轮询间隔，单位秒
	u16	m_wNum;								//参与轮询数量
	u16	m_awList[TP_CONF_MAX_TURNNUM];
	tagTPAConfTurnList()
	{
		Clear();
	}
	void Clear()
	{
		m_wInterval = 20;
		m_wNum = 0;
	}
}TPAConfTurnList;

typedef struct tagTPAConfTurnInfo
{
	u16				m_wConfID;
	TPAConfTurnList	m_atTurnList;
	tagTPAConfTurnInfo()
	{
		m_wConfID = TP_INVALID_INDEX;
	}
}TPAConfTurnInfo;


typedef struct tagTPAConfCallEpAddr
{
	u16	m_wConfID;
	TPAAlias	m_tAlias;
	tagTPAConfCallEpAddr()
	{
		m_wConfID = TP_INVALID_INDEX;
	}
}TPAConfCallEpAddr;

typedef struct tagTPAChairConfInfo
{
	u16			m_wConfID;
	time_t		m_tBeginTime;		//会议开始时间
	TPAAlias	m_tConfName;		//会议名称
	TPAAlias	m_tChairName;		//主席名称
	tagTPAChairConfInfo()
	{
		m_wConfID = TP_INVALID_INDEX;
		m_tBeginTime = 0;
	}
}TPAChairConfInfo;


typedef struct tagTPAAudMixStat
{
	u16		m_wConfID;
	u16		m_wEpID;
	BOOL32	m_bIsStart;
	EmPATpMixStatus	m_emMixStatus;
	tagTPAAudMixStat()
	{
		m_wConfID = TP_INVALID_INDEX;
		m_wEpID = TP_INVALID_INDEX;
		m_bIsStart = FALSE;
		m_emMixStatus = emPAAudMixIdle;
	}
}TPAAudMixStat;

typedef struct tagTPADisScreenStat
{
	BOOL32 m_bInDiscard;//该会场是否在讨论中
	u16 m_wEpIDDst;//UMS级联用
	
	u16 m_awScreenNum[TP_MAX_STREAMNUM]; //三个屏对应会场的屏数
	u16 m_awSpeakerIndx[TP_MAX_STREAMNUM];//三个屏对应会场的对应的发言坐席索引
	TPAAlias m_atAlias[TP_MAX_STREAMNUM];//三个屏对应的会场别名，无效值代表此处会场为空需看静态图片，单屏为中间
	
	tagTPADisScreenStat(){Clear();}
	
	void Clear()
	{
		m_bInDiscard = FALSE;
		m_wEpIDDst = TP_INVALID_INDEX;
		for ( u16 wIndex = 0; wIndex < TP_MAX_STREAMNUM; wIndex ++ )
		{
			m_atAlias[wIndex].Clear();
			m_awScreenNum[wIndex] = TP_INVALID_INDEX;
			m_awSpeakerIndx[wIndex] = TP_INVALID_INDEX;
		}
	}
	
	void SetByScreenID( u16 wScreenID, TPAAlias tAlias, u16 wScreenNum, u16 wSpeakerIndex )
	{
		if ( wScreenID >= TP_MAX_STREAMNUM )
		{
			return ;
		}
		
		m_atAlias[wScreenID].SetAlias( tAlias.m_byType, tAlias.m_abyAlias);
		m_awSpeakerIndx[wScreenID] = wSpeakerIndex;
		m_awScreenNum[wScreenID] = wScreenNum;
		
		return ;
	}
	
}TPADisScreenStat;



typedef struct tagTPADisAskKeyFrame
{
	u16 m_wEpID;//关键帧请求的源会场ID
	u16 m_wScreenNo;//源会场的哪个屏请求
	BOOL32 m_bReqBySys;
	tagTPADisAskKeyFrame(){Clear();}
	
	void Clear()
	{
		m_wEpID = TP_INVALID_INDEX;
		m_wScreenNo = TP_INVALID_INDEX;
		m_bReqBySys = FALSE;
	}
	
}TPADisAskKeyFrame; 


typedef struct tagTPADisListOpr
{
	u16 m_wConfID;
	u16 m_wEpID;
	u16 m_wIndex;//在CNC界面的位置 0-3
	EmPADisListOprType m_emOprType;
	EmPADisListOprReslt m_emOprReslt; //回复CNC时使用
	tagTPADisListOpr()
	{
		m_wConfID = TP_INVALID_INDEX;
		m_wEpID = TP_INVALID_INDEX;
		m_wIndex = TP_INVALID_INDEX;
		m_emOprType = emPA_DisListOpr_Add;
		m_emOprReslt = emPA_DisListOpr_Sucess;
	}
}TPADisListOpr;

// added by ganxiebin 20120814 >>> 

// 最大终端数量, 对应MAX_CONFMT_NUM
#define PA_MAX_CONFMT_NUM		192	

// 终端编号参数
// 对应modulcallstruct.h中的TMDLTERLABEL结构体
typedef struct tagTPATerminalLabel 
{
	u8 m_byMcuNo;
	u8 m_byTerNo;
	
	tagTPATerminalLabel()
	{
		Clear();		
	}
	
	void Clear()
	{
		m_byMcuNo = 0xff;
		m_byTerNo = 0xff;
	}

	BOOL32 isValid()
	{
		if( m_byTerNo <= PA_MAX_CONFMT_NUM && m_byMcuNo != 0xff )
			return TRUE;

		return FALSE;
	}

	BOOL32 operator == (const tagTPATerminalLabel& tTerLabel)
	{
      if(memcmp(this, (void*)&tTerLabel, sizeof(tTerLabel)) == 0)
			  return TRUE;
		
		  return FALSE;
	}

}TPATerminalLabel,*PTPATerminalLabel;
			

// 终端编号列表
// 对应modulcallstruct.h中的TMDLTERLABELLIST结构体
typedef struct tagTPATerLabelList
{
	u8 m_byNum;
	TPATerminalLabel m_atTerLabelList[PA_MAX_CONFMT_NUM];
	
	tagTPATerLabelList()
	{
		Clear();
	}
	
	void Clear()
	{
		m_byNum = 0;
		memset( m_atTerLabelList, 0, sizeof(m_atTerLabelList) );
	}
}TPATerLabelList;

// 终端别名长度, 对应MAX_TERMINALID_LEN
#define PA_MAX_TERMINALID_LEN		128	

//终端信息(会控用), 对应TMDLTERMINALINFO
typedef struct tagTPATerminalInfo
{
	u8 m_byMcuNo;   
	u8 m_byTerNo;
	u8 m_byConfNo;   //会议号码
	s8 m_achTerminalId[PA_MAX_TERMINALID_LEN+1];    //TerminalID
	BOOL32 m_bIsChair;   //是否是主席
	BOOL32 m_bIsFloor;   //是否是发言人
	
	BOOL32 m_bLastPack;  // 是否是最后一包
	BOOL32 m_bOnline;    // 是否在线
	
	TPATerminalLabel m_tTerYouAreView;    //正在选看谁
	u32 m_anSeenByOtherListMask[6];   //6个int表示192个位，来表示192个终端，谁选看了这个终端

	tagTPATerminalInfo()
	{
		Clear();
	}
	
	void Clear()
	{
		m_byConfNo = 0xff;
		m_byMcuNo = 0xff;
		m_byTerNo = 0xff;
		memset( m_achTerminalId, 0, sizeof(m_achTerminalId) );
		m_bIsChair = FALSE;
		m_bIsFloor = FALSE;
		
		m_bLastPack = FALSE;
		m_bOnline   = FALSE;
		m_tTerYouAreView.Clear();
		
		memset( m_anSeenByOtherListMask, 0, sizeof(m_anSeenByOtherListMask) );
	}
	
	void AddSeenByOtherList( u8 terNo )
	{
		if( terNo >= 192 )
			return;

		u8 i = terNo/(sizeof(u32)*8);
		u8 j = terNo%(sizeof(u32)*8);
		
		u32 mask = 0x00000001;
		mask <<= j;
		
		m_anSeenByOtherListMask[i] |= mask;
	}

	void DelSeenByOtherList( u8 terNo )
	{
		if( terNo >= 192 )
			return;

		u8 i = terNo/(sizeof(u32)*8);
		u8 j = terNo%(sizeof(u32)*8);

		u32 mask = 0x00000001;
		mask <<= j;

		m_anSeenByOtherListMask[i] &= ~mask;
	}

	BOOL32 IsSeenByOtherListEmpty( )
	{
		u32 temp[6];
		memset( temp, 0, sizeof(temp) );
		return !memcmp( temp, m_anSeenByOtherListMask, sizeof(temp) );
	}

	void ResetSeenByOtherList( )
	{
		memset( m_anSeenByOtherListMask, 0, sizeof(m_anSeenByOtherListMask) );
	}

	u8 GetSeenByOtherList( u8* list )  //这个数组最大可能192个u8
	{ 
		u8 count = 0;

		for( u8 i = 0; i < 6; i++ )
		{
			if( m_anSeenByOtherListMask[i] != 0 )
			{
				for( u8 j = 0; j < sizeof(u32)*8; j++ )
				{
					u32 mask = 0x00000001;
					mask <<= j;

					if( m_anSeenByOtherListMask[i] & mask )
					{
						list[count++] = (u8)(sizeof(u32)*8*i + j);
					}
				}
			}
		}
		return count;
	}
	
}TPATerminalInfo,*PTPATerminalInfo;

// added by ganxiebin 20120814 <<<
#endif








