/*****************************************************************************
   模块名      : MtAdp
   文件名      : mastructr4v4b2.h
   相关文件    : 
   文件实现功能: 4.0版本R4V4B2兼容结构定义 (同步自11月1日的4.0每日版本)
   作者        : 张宝卿
   版本        : V4.0  Copyright(C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2007/11/02  4.0         张宝卿        创建
******************************************************************************/

#ifndef _MASTRUCTR4V4B2_H_
#define _MASTRUCTR4V4B2_H_

#ifdef WIN32
#pragma pack( push )
#pragma pack( 1 )
#define PACKED 
#else
#define PACKED __attribute__((__packed__))	
#endif

#ifndef SETBITSTATUS
#define SETBITSTATUS(StatusValue, StatusMask, bStatus)  \
    if (bStatus)    StatusValue |= StatusMask;          \
    else            StatusValue &= ~StatusMask;

#define GETBITSTATUS(StatusValue, StatusMask)  (0 != (StatusValue&StatusMask))   
#endif

//向上级mcu上报本端的终端状态时所用的结构 (len:8)
struct TMcuToMcuMtStatusBeforeV4R5
{
    enum TMcuToMcuMtStatusMask
    {
        Mask_Fecc   =   0x01,
        Mask_Mixing =   0x02,
        Mask_VideoLose = 0x04,
		Mask_CallMode =  0x08,
        Mask_SendingVideo = 0x10,
        Mask_SendingAudio = 0x20,
        Mask_RcvingVideo = 0x40,
        Mask_RecvingAudio = 0x80
    };

protected:
	u8  m_byIsStatus;     //0-bit是否能遥控摄像头,1-bit是否在混音,2-bit是否视频源丢失
	u8  m_byCurVideo;     //当前视频源(1)
	u8  m_byCurAudio;     //当前音频源(1)
	u8  m_byMtBoardType;  //终端的板卡类型(MT_BOARD_WIN-0, MT_BOARD_8010-1, MT_BOARD_8010A-2, MT_BOARD_8018-3, MT_BOARD_IMT-4,MT_BOARD_8010C-5)
	u32 m_dwPartId;       //网络序

public:
	TMcuToMcuMtStatusBeforeV4R5( void )
	{ 
		memset( this, 0, sizeof(TMcuToMcuMtStatusBeforeV4R5));
		SetIsEnableFECC(FALSE);
        SetIsMixing(FALSE);
        SetIsVideoLose(FALSE);
		SetCurVideo(1);
		SetCurAudio(1);
		SetIsAutoCallMode(FALSE);		
	}
	void SetIsEnableFECC(BOOL bCamRCEnable){ SETBITSTATUS(m_byIsStatus, Mask_Fecc, bCamRCEnable) } 
	BOOL IsEnableFECC( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_Fecc); }
    void SetIsMixing(BOOL bMixing) { SETBITSTATUS(m_byIsStatus, Mask_Mixing, bMixing) } 
    BOOL IsMixing(void) const { return GETBITSTATUS(m_byIsStatus, Mask_Mixing); }
    void SetIsVideoLose(BOOL bVideoLose) { SETBITSTATUS(m_byIsStatus, Mask_VideoLose, bVideoLose) }
    BOOL IsVideoLose(void) const { return GETBITSTATUS(m_byIsStatus, Mask_VideoLose);    }
	void SetCurVideo(u8 byCurVideo){ m_byCurVideo = byCurVideo;} 
	u8   GetCurVideo( void ) const { return m_byCurVideo; }
	void SetCurAudio(u8 byCurAudio){ m_byCurAudio = byCurAudio;} 
	u8   GetCurAudio( void ) const { return m_byCurAudio; }
	void SetMtBoardType(u8 byType) { m_byMtBoardType = byType; }
	u8   GetMtBoardType() const {return m_byMtBoardType; }
	void SetPartId(u32 dwPartId){ m_dwPartId = htonl(dwPartId);} 
	u32  GetPartId( void ) const { return ntohl(m_dwPartId); }
	void SetIsAutoCallMode( BOOL byAutoMode )   { SETBITSTATUS(m_byIsStatus, Mask_CallMode, byAutoMode); }
	BOOL IsAutoCallMode( void )                 { return GETBITSTATUS(m_byIsStatus, Mask_CallMode); }
    void SetSendVideo(BOOL bSend) { SETBITSTATUS(m_byIsStatus, Mask_SendingVideo, bSend) } 
    BOOL IsSendVideo( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_SendingVideo); }
    void SetSendAudio(BOOL bSend) { SETBITSTATUS(m_byIsStatus, Mask_SendingAudio, bSend) } 
    BOOL IsSendAudio( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_SendingAudio); }
    void SetRecvVideo(BOOL bRecv) { SETBITSTATUS(m_byIsStatus, Mask_RcvingVideo, bRecv) } 
    BOOL IsRecvVideo( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_RcvingVideo); }
    void SetRecvAudio(BOOL bRecv) { SETBITSTATUS(m_byIsStatus, Mask_RecvingAudio, bRecv) } 
    BOOL IsRecvAudio( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_RecvingAudio); }	
}
PACKED
;

//向上级mcu上报本端的终端状态时所用的结构 (len:8)
struct TMcuToMcuMtStatusBeforeV4R6NoMultiCascade
{
    enum TMcuToMcuMtStatusMask
    {
        Mask_Fecc   =   0x01,
        Mask_Mixing =   0x02,
        Mask_VideoLose = 0x04,
		Mask_CallMode =  0x08,
        Mask_SendingVideo = 0x10,
        Mask_SendingAudio = 0x20,
        Mask_RcvingVideo = 0x40,
        Mask_RecvingAudio = 0x80
    };

protected:
	u8  m_byIsStatus;     //0-bit是否能遥控摄像头,1-bit是否在混音,2-bit是否视频源丢失
	u8  m_byCurVideo;     //当前视频源(1)
	u8  m_byCurAudio;     //当前音频源(1)
	u8  m_byMtBoardType;  //终端的板卡类型(MT_BOARD_WIN-0, MT_BOARD_8010-1, MT_BOARD_8010A-2, MT_BOARD_8018-3, MT_BOARD_IMT-4,MT_BOARD_8010C-5)
	u32 m_dwPartId;       //网络序
	u8  m_byMtExtInfo;   //中端呼叫失败原因	
public:
	TMcuToMcuMtStatusBeforeV4R6NoMultiCascade( void )
	{ 
		memset( this, 0, sizeof(TMcuToMcuMtStatusBeforeV4R6NoMultiCascade));
		SetIsEnableFECC(FALSE);
        SetIsMixing(FALSE);
        SetIsVideoLose(FALSE);
		SetCurVideo(1);
		SetCurAudio(1);
		SetIsAutoCallMode(FALSE);
		SetMtExtInfo( MTLEFT_REASON_NONE );
	}
	void SetIsEnableFECC(BOOL bCamRCEnable){ SETBITSTATUS(m_byIsStatus, Mask_Fecc, bCamRCEnable) } 
	BOOL IsEnableFECC( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_Fecc); }
    void SetIsMixing(BOOL bMixing) { SETBITSTATUS(m_byIsStatus, Mask_Mixing, bMixing) } 
    BOOL IsMixing(void) const { return GETBITSTATUS(m_byIsStatus, Mask_Mixing); }
    void SetIsVideoLose(BOOL bVideoLose) { SETBITSTATUS(m_byIsStatus, Mask_VideoLose, bVideoLose) }
    BOOL IsVideoLose(void) const { return GETBITSTATUS(m_byIsStatus, Mask_VideoLose);    }
	void SetCurVideo(u8 byCurVideo){ m_byCurVideo = byCurVideo;} 
	u8   GetCurVideo( void ) const { return m_byCurVideo; }
	void SetCurAudio(u8 byCurAudio){ m_byCurAudio = byCurAudio;} 
	u8   GetCurAudio( void ) const { return m_byCurAudio; }
	void SetMtBoardType(u8 byType) { m_byMtBoardType = byType; }
	u8   GetMtBoardType() const {return m_byMtBoardType; }
	void SetPartId(u32 dwPartId){ m_dwPartId = htonl(dwPartId);} 
	u32  GetPartId( void ) const { return ntohl(m_dwPartId); }
	void SetIsAutoCallMode( BOOL byAutoMode )   { SETBITSTATUS(m_byIsStatus, Mask_CallMode, byAutoMode); }
	BOOL IsAutoCallMode( void )                 { return GETBITSTATUS(m_byIsStatus, Mask_CallMode); }
    void SetSendVideo(BOOL bSend) { SETBITSTATUS(m_byIsStatus, Mask_SendingVideo, bSend) } 
    BOOL IsSendVideo( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_SendingVideo); }
    void SetSendAudio(BOOL bSend) { SETBITSTATUS(m_byIsStatus, Mask_SendingAudio, bSend) } 
    BOOL IsSendAudio( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_SendingAudio); }
    void SetRecvVideo(BOOL bRecv) { SETBITSTATUS(m_byIsStatus, Mask_RcvingVideo, bRecv) } 
    BOOL IsRecvVideo( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_RcvingVideo); }
    void SetRecvAudio(BOOL bRecv) { SETBITSTATUS(m_byIsStatus, Mask_RecvingAudio, bRecv) } 
    BOOL IsRecvAudio( void ) const { return GETBITSTATUS(m_byIsStatus, Mask_RecvingAudio); }
	void SetMtExtInfo( u8 byMtExtInfo ){ m_byMtExtInfo = byMtExtInfo; }
	u8   GetMtExtInfo( void ) const { return m_byMtExtInfo;}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

typedef struct tagPartNoMultiCascade:public TPartID
{
    s8			m_aszPartName[STR_LEN];     //part name
    s8			m_aszDialStr[STR_LEN];      //dial string
    u32			m_dwIP;            //part ip
    emEPType	m_emEPType;        //part type
    s8          m_aszEPDesc[STR_LEN];       //part des(product id)
    PayloadType m_emVideoIn;       //video in format (H263、G711.....None)
    PayloadType m_emVideoOut;      //.....
    PayloadType m_emVideo2In;      //.....
    PayloadType m_emVideo2Out;     //..... 
    PayloadType m_emAudioIn;       //.....
    PayloadType m_emAudioOut;      //..... 
    BOOL32      m_bDataMeeting;    //data meeting  
    BOOL32      m_bVideoMuteIn;    //whether or not video in mute(no camera)
    BOOL32      m_bVideoMuteOut;   //....(no dispaly)
    BOOL32      m_bAudioMuteIn;    //....(no mic)
	BOOL32      m_bAudioMuteOut;   //....(no audio player)
    BOOL32      m_bInvited;        //whether or not is Invited
    BOOL32      m_bMaster;         //whether or not Master ternial
    TTimeSpec   m_tStartTime;      //call start time
    emConnState m_emConnState;     //connect state
    BOOL32      m_bFECCEnable;     //whether or not enable FECC
    TPartVideoInfo m_tPartVideoInfo;  //视频输出输入信息
    TPartAVStatis  m_tAVStatis;    //视频音频输入输出统计信息
	

    tagPartNoMultiCascade()
    {
        memset(this, 0, sizeof(*this));
    }
    inline void SetVideoAttr(PayloadType emVideoIn, PayloadType emVideoOut, 
                                PayloadType emVideo2In, PayloadType emVideo2Out, 
                                BOOL32 bVideoMuteIn, BOOL32 bVideoMuteOut)
    {
         m_emVideoIn      = emVideoIn;      
         m_emVideoOut     = emVideoOut; 
         m_emVideo2In      = emVideo2In;      
         m_emVideo2Out     = emVideo2Out; 
         m_bVideoMuteIn   = bVideoMuteIn;    
         m_bVideoMuteOut  = bVideoMuteOut;   
    }

    inline void GetVideoAttr(PayloadType *pemVideoIn, PayloadType *pemVideoOut, 
                                PayloadType *pemVideo2In, PayloadType *pemVideo2Out,
                                BOOL32 *pbVideoMuteIn, BOOL32 *pbVideoMuteOut)
    {
        if(pemVideoIn)
        { 
            *pemVideoIn = m_emVideoIn;
        }
        if(pemVideoOut)
        { 
            *pemVideoOut = m_emVideoOut; 
        }
        if(pemVideo2In)
        { 
            *pemVideo2In = m_emVideo2In;
        }
        if(pemVideo2Out)
        { 
            *pemVideo2Out = m_emVideo2Out; 
        }
        if(pbVideoMuteIn)
        { 
            *pbVideoMuteIn = m_bVideoMuteIn;
        }
        if(pbVideoMuteOut)
        { 
            *pbVideoMuteOut = m_bVideoMuteOut;
        }
        return;
    }
    
    inline void SetViewInAndOutAttr(TPartVideoInfo &tPartVideoInfo)
    {
        memcpy(&m_tPartVideoInfo, &tPartVideoInfo,sizeof(TPartVideoInfo));
    }

    inline TPartVideoInfo* GetViewInAndOutAttr()
    {
        return &m_tPartVideoInfo;
    }
    
    inline void SetAudioAttr(PayloadType emAudioIn, PayloadType emAudioOut, BOOL32 bAudioMuteIn, BOOL32 bAudioMuteOut)
    {
        m_emAudioIn      = emAudioIn;      
        m_emAudioOut     = emAudioOut;     
        m_bAudioMuteIn   = bAudioMuteIn;    
        m_bAudioMuteOut  = bAudioMuteOut;   
    }

    inline void GetAudioAttr(PayloadType *pemAudioIn, PayloadType *pemAudioOut, BOOL32 *pbAudioMuteIn, BOOL32 *pbAudioMuteOut)
    {
        if(pemAudioIn)
        { 
            *pemAudioIn = m_emAudioIn;
        }
        if(pemAudioOut)
        { 
            *pemAudioOut = m_emAudioOut; 
        }
        if(pbAudioMuteIn)
        { 
            *pbAudioMuteIn = m_bAudioMuteIn;
        }
        if(pbAudioMuteOut)
        { 
            *pbAudioMuteOut = m_bAudioMuteOut;
        }
        return;
    }

    inline void SetNameAttr(const s8 *pszName, s32 nNameLen, const s8 *pszDescr, s32 nDescrLen)
    {
        // assert(pszName != NULL);
        s32 nStrLen = (nNameLen >= STR_LEN ? STR_LEN -1 : nNameLen );
        strncpy(m_aszPartName, pszName, nStrLen);
        m_aszPartName[nStrLen] = '\0';

        if(pszDescr != NULL && nDescrLen > 0)
        {
            nStrLen = (nDescrLen >= STR_LEN ? STR_LEN -1 : nDescrLen );
            strncpy(m_aszDialStr, pszDescr, nStrLen);
            m_aszDialStr[nStrLen] = '\0';
        }
        else
        {
            m_aszDialStr[0] = '\0';
        }
    }

    inline void GetNameAttr(s8 *pszName, s32 nNameLen, s8 *pszDescr, s32 nDescrLen)
    {
        if(pszName != NULL && nNameLen > 0)
        {
            s32 nStrLen = (nNameLen >= STR_LEN ? STR_LEN -1 : nNameLen );
            strncpy(pszName, m_aszPartName, nStrLen);
            pszName[nStrLen] = '\0';  
        }

        if(pszDescr != NULL && nDescrLen > 0)
        {
            s32 nStrLen = (nDescrLen >= STR_LEN ? STR_LEN -1 : nDescrLen);
            strncpy(pszDescr, m_aszDialStr, nStrLen);     
            pszDescr[nStrLen] = '\0';
        }
    }

    inline void SetDialStr(const s8 *pszDialStr, s32 nLen)
    {
       if(pszDialStr !=NULL && nLen >0)
       {
           s32 nStrLen = (nLen >= STR_LEN ? STR_LEN -1 : nLen);
           strncpy(m_aszDialStr, pszDialStr, nStrLen);
           m_aszDialStr[nStrLen] = '\0';
       }
    }

    inline s8 * GetDialStr() { return this->m_aszDialStr; }

    inline void SetEntityAttr(emConnState emState, emEPType emEntityType, BOOL32 bFECCEnable, BOOL32 bMaster, BOOL32 bInvited)
    {
        m_emConnState    = emState;  
        m_emEPType       = emEntityType;
        m_bFECCEnable    = bFECCEnable;     
        m_bMaster        = bMaster;       
        m_bInvited       = bInvited;       
    }
    inline BOOL32 IsFECCEnable(){ return m_bFECCEnable;}
    inline BOOL32 IsMaster() { return m_bMaster;}
    inline BOOL32 IsInvited() { return m_bInvited;}
    inline emConnState GetConnState() { return m_emConnState;}
    inline void   SetConnState(emConnState emState) { m_emConnState = emState;}
    inline void   StopAsVideoSrc() { m_tPartVideoInfo.m_nViewCount = 0;}
    inline BOOL32 IsVideoSrc() { return (m_tPartVideoInfo.m_nViewCount > 0);}
    inline u32    GetTotalBandWith() { return m_tAVStatis.m_dwTotalInBandWidth + m_tAVStatis.m_dwTotalOutBandWidth;}

    inline void GetEntityAttr(emConnState *pemState, BOOL32 *pbFECCEnable, BOOL32 *pbMaster, BOOL32 *pbInvited)
    {
        if(pemState)
        { 
            *pemState = m_emConnState;
        }
        if(pbFECCEnable)
        { 
            *pbFECCEnable = m_bFECCEnable; 
        }
        if(pbMaster)
        { 
            *pbMaster = m_bMaster;
        }
        if(pbInvited)
        { 
            *pbInvited = m_bInvited;
        }
        return;
    }
    inline void GetVideoScheme(s32 *pnViewId, s32 *pnVideoSchemeId)
    {
        m_tPartVideoInfo.GetVideoSpec(pnViewId,pnVideoSchemeId);
    }
    inline void SetVideoScheme(s32 nViewId, s32 nVideoSchemeId)
    {
        m_tPartVideoInfo.SetVideoSpec(nViewId, nVideoSchemeId, NULL, NULL);
    }
}TPartNoMultiCascade,*PTPartNoMultiCascade;

#ifdef WIN32
#pragma pack( pop )
#endif

#endif  // _MASTRUCTR4V4B2_H_
