/*****************************************************************************
   模块名      : MCU
   文件名      : confinfostructv4r4r5.h
   相关文件    : 
   文件实现功能: 4.0版本R4V4B2及R5兼容结构定义 (同步自11月1日的4.0每日版本)
   作者        : 张宝卿
   版本        : V4.6  Copyright(C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期           版本      修改人                 修改内容
   2007/11/02       4.0       张宝卿                   创建
   2011/04/12       4.6       苗庆松     解决TConfInfo版本兼容移至10-Common目录
******************************************************************************/
#ifndef _MASTRUCTR4R4R5_H_
#define _MASTRUCTR4R4R5_H_

#ifdef WIN32
#pragma pack( push )
#pragma pack( 1 )
#define PACKED 
#else
#define PACKED __attribute__((__packed__))	
#endif

//定义会议方式结构(会议中会改变), sizeof = 24
struct TConfModeV4R4B2
{
protected:
	u8     m_byTakeMode;        //会议举行方式: 0-预约 1-即时 2-会议模板 
	u8     m_byLockMode;        //会议锁定方式: 0-不锁定,所有会控可见可操作 1-根据密码操作此会议 2-某个会议控制台锁定
    u8     m_byCallMode;        //呼叫终端方式: 0-不呼叫终端，手动呼叫 1-呼叫一次 2-定时呼叫未与会终端
	u8     m_byVACMode;         //语音激励方式: 0-不启用语音激励控制发言(导演控制发言) 1-启用语音激励控制发言
	u8     m_byAudioMixMode;    //混音方式:     0-不混音 1-正在进行某种混音
	u8     m_byRecordMode;      //会议录像方式: (BIT:0-6)0-不录像 1-录像 2-录像暂停; BIT7 = 0 实时 1抽帧录像
	u8     m_byPlayMode;        //会议放像方式: 0-不放像 1-放像 2-放像暂停 3-快进 4-快退
	u8     m_byBasMode;         //码率适配方式: BIT0-是否RTP头适配 BIT1-是否码率适配 BIT2-是否类型适配
	u8     m_byPollMode;        //会议轮询方式: 0-不轮询广播 1-仅图像轮询广播 2-轮流发言
	u8     m_byDiscussMode;     //会议讨论模式: 0-非讨论模式(演讲模式) 1-讨论模式(定制或者智能)
	u8     m_byForceRcvSpeaker; //强制收看发言: 0-不强制收看发言人 1-强制收看发言人
	u8     m_byNoChairMode;     //会议主席模式: 0-不是无主席方式 1-无主席方式
	u8     m_byRegToGK;         //会议注册GK情况: 0-会议未在GK上注册 1-会议在GK上成功注册
	u8     m_byMixSpecMt;		//是否指定终端混音: 0-不是定制混音(智能混音) 1-指定终端混音
	u8	   m_byGkCharge;		//gk计费认证是否通过 1－通过，0－认证失败
	u8	   m_byReserved2;		//保留字段
	u32    m_dwCallInterval;    //定时呼叫时的呼叫间隔：(单位：秒，最小为5秒)（网络序）
	u32    m_dwCallTimes;       //定时呼叫时的呼叫次数（网络序）
	
public:
	TConfModeV4R4B2( void ){ memset( this, 0, sizeof(TConfModeV4R4B2) ); }
    void   SetTakeMode(u8 byTakeMode){ m_byTakeMode = byTakeMode | (m_byTakeMode&0x80); } 
    u8     GetTakeMode( void ) const { return (m_byTakeMode&0x0F); }
    void   SetTakeFromFile(BOOL32 bFromFile) // MCU 内部使用
    {
        if (bFromFile)
            m_byTakeMode |= 0x80;
        else
            m_byTakeMode &= 0x0F;
    }
    BOOL32 IsTakeFromFile(void) const
    {
        return ((m_byTakeMode&0x80)==0x80);
    }
    void   SetLockMode(u8   byLockMode){ m_byLockMode = byLockMode;} 
    u8     GetLockMode( void ) const { return m_byLockMode; }
    void   SetCallMode(u8   byCallMode){ m_byCallMode = byCallMode;} 
    u8     GetCallMode( void ) const { return m_byCallMode; }
	void   SetCallInterval(u32 dwCallInterval){ m_dwCallInterval = htonl(dwCallInterval);} 
    u32    GetCallInterval( void ) const { return ntohl(m_dwCallInterval); }
	void   SetCallTimes(u32 dwCallTimes){ m_dwCallTimes = htonl(dwCallTimes);} 
    u32    GetCallTimes( void ) const { return ntohl(m_dwCallTimes); }
    void   SetVACMode(BOOL bVACMode){ m_byVACMode = bVACMode;} 
    BOOL   IsVACMode( void ) const { return m_byVACMode == 0 ? FALSE : TRUE; }
    void   SetAudioMixMode(BOOL bAudioMixMode){ m_byAudioMixMode = bAudioMixMode;} 
    BOOL   IsAudioMixMode( void ) const { return m_byAudioMixMode == 0 ? FALSE : TRUE; }
	void   SetRegToGK( BOOL bRegToGK ){ m_byRegToGK = bRegToGK; }
	BOOL   IsRegToGK( void ) const { return m_byRegToGK == 0 ? FALSE : TRUE; }
    void   SetGkCharge( BOOL bCharge ) { m_byGkCharge = bCharge ? 1 : 0; }
    BOOL   IsGkCharge( void ) const { return (1 == m_byGkCharge ? TRUE : FALSE); }
    void   SetRecordMode(u8   byRecordMode)
	{   byRecordMode   &= 0x7f ;
		m_byRecordMode &= 0x80 ;
		m_byRecordMode |= byRecordMode;
	} 
    u8     GetRecordMode( void ) const { return m_byRecordMode&0x7f; }
	
	//判断当前是否在抽帧录像，若是返回TRUE否则返回FALSE
	BOOL   IsRecSkipFrame() const{ return m_byRecordMode&0x80 ? TRUE:FALSE;}
	//bSkipFrame = TRUE 设置当前为抽帧录像,否则实时录像
	void   SetRecSkipFrame(BOOL bSkipFrame)
	{
		if(bSkipFrame)
			m_byRecordMode |=0x80;
		else 
			m_byRecordMode &=0x7f;
	}
    void   SetPlayMode( u8  byPlayMode ){ m_byPlayMode = byPlayMode;} 
    u8     GetPlayMode( void ) const { return m_byPlayMode; }
    void   SetBasMode( u8  byBasMode, BOOL32 bHasBasMode )
	{ 
		if( bHasBasMode )
		{
			m_byBasMode |= (1<<(byBasMode-1)) ;
		}
		else
		{
			m_byBasMode &= ~(1<<(byBasMode-1));
		}
	} 
    BOOL   GetBasMode( u8  byBasMode ) const 
	{
        if ( CONF_BASMODE_NONE == byBasMode )
            return FALSE;
        else
		    return m_byBasMode & (1<<(byBasMode-1));
	}
    u8     GetBasModeValue() const
    {
        return m_byBasMode;
    }
    void   SetPollMode(u8   byPollMode){ m_byPollMode = byPollMode;} 
    u8     GetPollMode( void ) const { return m_byPollMode; }
    void   SetDiscussMode(u8   byDiscussMode){ m_byDiscussMode = byDiscussMode;} 
    BOOL   IsDiscussMode( void ) const { return m_byDiscussMode == 0 ? FALSE : TRUE; }  
    void   SetForceRcvSpeaker(u8   byForceRcvSpeaker){ m_byForceRcvSpeaker = byForceRcvSpeaker;} 
    BOOL   IsForceRcvSpeaker( void ) const { return m_byForceRcvSpeaker == 0 ? FALSE : TRUE; }
    void   SetNoChairMode(u8   byNoChairMode){ m_byNoChairMode = byNoChairMode;} 
    BOOL   IsNoChairMode( void ) const { return m_byNoChairMode == 0 ? FALSE : TRUE; }
    void   SetMixSpecMt( BOOL bMixSpecMt ){ m_byMixSpecMt = bMixSpecMt;} 
    BOOL   IsMixSpecMt( void ) const { return m_byMixSpecMt == 0 ? FALSE : TRUE; }
	void   Print( void ) const
	{
		OspPrintf( TRUE, FALSE, "\nConfMode:\n" );
		OspPrintf( TRUE, FALSE, "m_byTakeMode: %d\n", m_byTakeMode);
		OspPrintf( TRUE, FALSE, "m_byLockMode: %d\n", m_byLockMode);
		OspPrintf( TRUE, FALSE, "m_byCallMode: %d\n", m_byCallMode);
		OspPrintf( TRUE, FALSE, "m_dwCallInterval: %d\n", GetCallInterval());
		OspPrintf( TRUE, FALSE, "m_dwCallTimes: %d\n", GetCallTimes());
		OspPrintf( TRUE, FALSE, "m_byVACMode: %d\n", m_byVACMode);
		OspPrintf( TRUE, FALSE, "m_byAudioMixMode: %d\n", m_byAudioMixMode);
		OspPrintf( TRUE, FALSE, "m_byRecordMode: %d\n", m_byRecordMode);
		OspPrintf( TRUE, FALSE, "m_byPlayMode: %d\n", m_byPlayMode);
		OspPrintf( TRUE, FALSE, "m_byBasMode: %d\n", m_byBasMode);
		OspPrintf( TRUE, FALSE, "m_byPollMode: %d\n", m_byPollMode);
		OspPrintf( TRUE, FALSE, "m_byDiscussMode: %d\n", m_byDiscussMode);
		OspPrintf( TRUE, FALSE, "m_byForceRcvSpeaker: %d\n", m_byForceRcvSpeaker);
		OspPrintf( TRUE, FALSE, "m_byNoChairMode: %d\n", m_byNoChairMode);
		OspPrintf( TRUE, FALSE, "m_byRegToGK: %d\n", m_byRegToGK);
        OspPrintf( TRUE, FALSE, "m_byGkCharge: %d\n", m_byGkCharge );
		OspPrintf( TRUE, FALSE, "m_byMixSpecMt: %d\n", m_byMixSpecMt);
	}
}
/*
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
*/
PACKED
;

//zbq[11/01/2007] 混音优化: 本结构只作兼容适配用。
//定义讨论参数结构
struct TDiscussParamV4R4B2
{
public:
    u8  m_byMixMode;   //混音模式：mcuNoMix - 不混音
                       //          mcuWholeMix - 全体混音
                       //          mcuPartMix - 定制混音
                       //          mcuVacMix, - VAC 
                       //          mcuVacWholeMix - 带Vac的全体混音
    u8  m_byMemberNum; //参加讨论的成员数量(混音深度)
    TMt m_atMtMember[MAXNUM_MIXER_DEPTH];  //讨论成员
public:
    TDiscussParamV4R4B2(void)
    { 
        memset(this, 0, sizeof(TDiscussParamV4R4B2));
    }
    BOOL32 IsMtInMember(TMt tMt) const
    {
        u8 byLoop = 0;
        u8 byLoopCtrl = 0;
        while (byLoop < m_byMemberNum && byLoopCtrl < MAXNUM_MIXER_DEPTH)
        {
            if (tMt == m_atMtMember[byLoop])
            {
                return TRUE;
            }

            byLoop++;
            byLoopCtrl++;
        }

        return FALSE;
    }
}
/*
#ifndef WIN32
__attribute__ ((packed))
#endif
*/
PACKED
;

//电视墙轮询信息(len:32)
struct TTvWallPollInfoV4R4B2 : public TPollInfo
{
protected:
    TEqp    m_tTvWall;
    u8      m_byTWChnnl;
    
public:
	TTvWallPollInfoV4R4B2(void){ memset( this, 0x0, sizeof( TTvWallPollInfoV4R4B2 ) ); }
	
    void SetTvWall(TEqp tTvWall) { m_tTvWall = tTvWall; }
    TEqp GetTvWall(void) const{ return m_tTvWall; } 
    
	void SetTWChnnl(u8 byChnnl) { m_byTWChnnl = byChnnl; }
    u8   GetTWChnnl(void) const { return m_byTWChnnl; }
}PACKED
;

//定义画面合成参数结构 (len:136) // xliang [2/24/2009] 
struct TVMPParamV4R4B2
{
protected:
	u8      m_byVMPAuto;      //是否是自动画面合成 0-否 1-是
	u8      m_byVMPBrdst;     //合成图像是否向终端广播 0-否 1-是 
	u8      m_byVMPStyle;     //画面合成风格,参见mcuconst.h中画面合成风格定义
    u8      m_byVMPSchemeId;  //合成风格方案编号,最大支持5套方案,1-5
    u8      m_byVMPMode;      //图像复合方式: 0-不图像复合 1-会控或主席控制图像复合 2-自动图像复合(动态分屏与设置成员)
	u8		m_byRimEnabled;	  //是否使用边框: 0-不使用(默认) 1-使用;
							  //本字段目前只对方案0有效, 其他方案暂不涉及本字段的设置和判断
	//u8		m_byReserved1;	  //保留字段
	u8		m_byVMPBatchPoll; // xliang [12/18/2008] 是否是批量轮询 0-否
	u8		m_byReserved2;	  //保留字段
	TVMPMember  m_atVMPMember[MAXNUM_VMP_MEMBER_VER36/*MAXNUM_VMP_MEMBER 该宏已经被同化到20，不能再使用*/]; //画面合成成员
public:
    void   SetVMPAuto(u8   byVMPAuto){ m_byVMPAuto = byVMPAuto;} 
    BOOL   IsVMPAuto( void ) const { return m_byVMPAuto == 0 ? FALSE : TRUE; }
    void   SetVMPBrdst(u8   byVMPBrdst){ m_byVMPBrdst = byVMPBrdst;} 
    BOOL   IsVMPBrdst( void ) const { return m_byVMPBrdst == 0 ? FALSE : TRUE; }
    void   SetVMPStyle(u8   byVMPStyle){ m_byVMPStyle = byVMPStyle;} 
    u8     GetVMPStyle( void ) const { return m_byVMPStyle; }
    void   SetVMPSchemeId(u8 bySchemeId) { m_byVMPSchemeId = bySchemeId; }
    u8     GetVMPSchemeId(void) const { return m_byVMPSchemeId; }
    void   SetVMPMode(u8   byVMPMode){ m_byVMPMode = byVMPMode;} 
    u8     GetVMPMode( void ) const { return m_byVMPMode; }
	void   SetIsRimEnabled(u8 byEnabled){ m_byRimEnabled = byEnabled; }
	BOOL32 GetIsRimEnabled(void) const { return m_byRimEnabled == 1 ? TRUE : FALSE; }
	
	void	SetVMPBatchPoll(u8 byVMPBatchPoll){m_byVMPBatchPoll = byVMPBatchPoll;} // xliang [12/22/2008] 
	BOOL	IsVMPBatchPoll( void ) const { return m_byVMPBatchPoll == 0 ? FALSE : TRUE;}
	
    u8     GetMaxMemberNum( void ) const 
	{
		u8   byMaxMemNum = 1;

		switch( m_byVMPStyle ) 
		{
		case VMP_STYLE_ONE:
			byMaxMemNum = 1;
			break;
		case VMP_STYLE_VTWO:
		case VMP_STYLE_HTWO:
			byMaxMemNum = 2;
			break;
		case VMP_STYLE_THREE:
			byMaxMemNum = 3;
			break;
		case VMP_STYLE_FOUR:
			byMaxMemNum = 4;
			break;
		case VMP_STYLE_SIX:
			byMaxMemNum = 6;
			break;
		case VMP_STYLE_EIGHT:
			byMaxMemNum = 8;
			break;
		case VMP_STYLE_NINE:
			byMaxMemNum = 9;
			break;
		case VMP_STYLE_TEN:
			byMaxMemNum = 10;
			break;
		case VMP_STYLE_THIRTEEN:
			byMaxMemNum = 13;
			break;
		case VMP_STYLE_SIXTEEN:
			byMaxMemNum = 16;
			break;
		case VMP_STYLE_SPECFOUR:
			byMaxMemNum = 4;
			break;
		case VMP_STYLE_SEVEN:
			byMaxMemNum = 7;
			break;
		default:
			byMaxMemNum = 1;
			break;
		}
		return byMaxMemNum;
	}
	void   SetVmpMember( u8   byMemberId, TVMPMember tVMPMember )
	{
		if( byMemberId >= MAXNUM_VMP_MEMBER_VER36 )return;
		m_atVMPMember[byMemberId] = tVMPMember;
	}
	void   ClearVmpMember( u8 byMemberId )
	{
		if( byMemberId >= MAXNUM_VMP_MEMBER_VER36 )return;
		m_atVMPMember[byMemberId].SetNull();
        m_atVMPMember[byMemberId].SetMemberType(0);
	}
    TVMPMember *GetVmpMember( u8   byMemberId )
	{
		if( byMemberId >= MAXNUM_VMP_MEMBER_VER36 )return NULL;
		return &m_atVMPMember[byMemberId];
	}
	BOOL IsMtInMember( TMt tMt )
	{
		u8 byLoop = 0;
		u8 byLoopCtrl = 0;
		while( byLoop < GetMaxMemberNum() && byLoopCtrl < MAXNUM_VMP_MEMBER_VER36 )
		{
			if( tMt.GetMtId() == m_atVMPMember[byLoop].GetMtId() && 
				tMt.GetMcuId() == m_atVMPMember[byLoop].GetMcuId() )
			{
				return TRUE;
			}

			byLoop++;
			byLoopCtrl++;
		}

		return FALSE;
	}
    BOOL IsTypeInMember(u8 byType)
    {
        u8 byLoop = 0;
		while( byLoop < GetMaxMemberNum() )
		{
			if( byType == m_atVMPMember[byLoop].GetMemberType() )
			{
				return TRUE;
			}
			byLoop++;
		}

		return FALSE;
    }
	u8 GetChlOfMtInMember( TMt tMt )
	{
		u8 byLoop = 0;
		while( byLoop < GetMaxMemberNum() )
		{
			if( tMt.GetMtId() == m_atVMPMember[byLoop].GetMtId() )
			{
				return byLoop;
			}
			byLoop++;
		}

		return MAXNUM_VMP_MEMBER_VER36;
	}
	void   Print( void ) const
	{
		OspPrintf( TRUE, FALSE, "\nVMPParam:\n" );
		OspPrintf( TRUE, FALSE, "m_byVMPAuto: %d\n", m_byVMPAuto);
		OspPrintf( TRUE, FALSE, "m_byVMPBrdst: %d\n", m_byVMPBrdst);
		OspPrintf( TRUE, FALSE, "m_byVMPStyle: %d\n", m_byVMPStyle);
        OspPrintf( TRUE, FALSE, "m_byVmpSchemeId: %d\n", m_byVMPSchemeId );
		OspPrintf( TRUE, FALSE, "m_byRimEnabled: %d\n", m_byRimEnabled );
        OspPrintf( TRUE, FALSE, "m_byVMPMode: %d\n", m_byVMPMode );
		for( int i = 0; i < GetMaxMemberNum(); i ++ )
		{
			OspPrintf( TRUE, FALSE, "VMP member %d: MtId-%d, Type-%d\n", 
				i,  m_atVMPMember[i].GetMtId(), m_atVMPMember[i].GetMemberType() );            
		}
	}
}
/*
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
*/
PACKED
;

typedef TVMPParamV4R4B2 TVMPParamV4R5;

typedef TTvWallPollInfoV4R4B2 TTvWallPollInfoV4R5;

//定义会议状态结构, sizeof = 430
struct TConfStatusV4R4B2
{
public:
    TMt 	      m_tChairman;	      //主席终端，MCU号为0表示无主席
    TMt		      m_tSpeaker;		  //发言终端，MCU号为0表示无发言人
    TConfModeV4R4B2     m_tConfMode;        //会议方式
    TRecProg	  m_tRecProg;		  //当前录像进程,仅录像状态时有效
    TRecProg	  m_tPlayProg;	      //当前放像进程,仅放像状态时有效
    TPollInfo     m_tPollInfo;        //会议轮询参数,仅轮询时有较	
    TTvWallPollInfoV4R4B2  m_tTvWallPollInfo;  //电视墙当前轮询参数，仅轮询时有效
    TDiscussParamV4R4B2 m_tDiscussParam;    //当前混音参数，仅混音时有效
    TVMPParamV4R4B2     m_tVMPParam;        //当前视频复合参数，仅视频复合时有效
    TVMPParamV4R4B2     m_tVmpTwParam;      //当前VMPTW参数
    u8            m_byPrs;            //当前包重传状态

public:
    BOOL   HasChairman(void) const { return m_tChairman.GetMtId()==0 ? FALSE : TRUE; }	
	TMt    GetChairman(void) const { return m_tChairman; }
	void   SetNoChairman(void) { memset( &m_tChairman, 0, sizeof( TMt ) ); }
	void   SetChairman(TMt tChairman) { m_tChairman = tChairman; }
	BOOL   HasSpeaker(void) const { return m_tSpeaker.GetMtId()==0 ? FALSE : TRUE; }
    TMt    GetSpeaker(void) const { return m_tSpeaker; }
	void   SetNoSpeaker(void) { memset( &m_tSpeaker, 0, sizeof( TMt ) ); }
	void   SetSpeaker(TMt tSpeaker) { m_tSpeaker = tSpeaker; }
    void          SetConfMode(TConfModeV4R4B2 tConfMode) { m_tConfMode = tConfMode; } 
    TConfModeV4R4B2     GetConfMode(void) const { return m_tConfMode; }
    void          SetRecProg(TRecProg tRecProg){ m_tRecProg = tRecProg; } 
    TRecProg      GetRecProg(void) const { return m_tRecProg; }
    void          SetPlayProg(TRecProg tPlayProg){ m_tPlayProg = tPlayProg; } 
    TRecProg      GetPlayProg(void) const { return m_tPlayProg; }
    void          SetVmpParam(TVMPParamV4R4B2 tVMPParam){ m_tVMPParam = tVMPParam;} 
    TVMPParamV4R4B2     GetVmpParam(void) const { return m_tVMPParam; }

	//会议进行状态
	BOOL IsScheduled( void ) const { return m_tConfMode.GetTakeMode() == CONF_TAKEMODE_SCHEDULED ? TRUE : FALSE; }
	void SetScheduled( void ){ m_tConfMode.SetTakeMode( CONF_TAKEMODE_SCHEDULED ); }
	BOOL IsOngoing( void ) const { return m_tConfMode.GetTakeMode() == CONF_TAKEMODE_ONGOING ? TRUE : FALSE; }
	void SetOngoing( void ){ m_tConfMode.SetTakeMode( CONF_TAKEMODE_ONGOING ); }
	BOOL IsTemplate( void ) const { return m_tConfMode.GetTakeMode() == CONF_TAKEMODE_TEMPLATE ? TRUE : FALSE; }
	void SetTemplate( void ){ m_tConfMode.SetTakeMode( CONF_TAKEMODE_TEMPLATE ); }
	u8   GetTakeMode( void ) const{ return m_tConfMode.GetTakeMode();	}
	void SetTakeMode( u8 byTakeMode ){ m_tConfMode.SetTakeMode( byTakeMode ); }
    void SetTakeFromFile(BOOL32 bTakeFromFile) { m_tConfMode.SetTakeFromFile(bTakeFromFile); }  // MCU 内部使用
    BOOL IsTakeFromFile(void) const{ return m_tConfMode.IsTakeFromFile(); }

	//会议保护状态
	u8   GetProtectMode( void ) const { return m_tConfMode.GetLockMode(); }
    void SetProtectMode( u8   byProtectMode ) { m_tConfMode.SetLockMode( byProtectMode ); }
	
	//会议呼叫策略
	u8   GetCallMode( void ) const { return m_tConfMode.GetCallMode(); }
	void SetCallMode( u8   byCallMode ){ m_tConfMode.SetCallMode( byCallMode ); }
	void SetCallInterval(u32 dwCallInterval){ m_tConfMode.SetCallInterval( dwCallInterval ); } 
    u32  GetCallInterval( void ) const { return m_tConfMode.GetCallInterval( ); }
	void SetCallTimes(u32 dwCallTimes){ m_tConfMode.SetCallTimes( dwCallTimes ); } 
    u32  GetCallTimes( void ) const { return m_tConfMode.GetCallTimes( ); }

    //混音器状态(标识是否在使用混音器，无论是讨论（智能混音）、定制混音还是语音激励)
    BOOL32 IsNoMixing(void) const { return !m_tConfMode.IsAudioMixMode(); }
    void   SetNoMixing(void) { m_tConfMode.SetAudioMixMode(0); }
    BOOL32 IsMixing(void) const { return m_tConfMode.IsAudioMixMode(); }
    void   SetMixing(void) { m_tConfMode.SetAudioMixMode(1); }

    //是否会议混音（是否在混音，无论是智能混音还是定制混音）
    BOOL32 IsDiscussMode(void) const { return m_tConfMode.IsDiscussMode(); }
    void   SetDiscussMode(BOOL bDiscussMode = TRUE) { m_tConfMode.SetDiscussMode(bDiscussMode); }

    //混音方式（是智能还是定制混音）
    BOOL32 IsMixSpecMt(void) const { return m_tConfMode.IsMixSpecMt(); }
    void   SetMixSpecMt(BOOL bMixSpecMt) { m_tConfMode.SetMixSpecMt(bMixSpecMt); }

    //是否在语音激励
    BOOL32 IsVACMode(void) const { return m_tConfMode.IsVACMode(); }
    void   SetVACMode(BOOL bVACMode = TRUE) { m_tConfMode.SetVACMode(bVACMode); }

    //讨论参数（本级）
    u8     GetMixModeParam(void) const { return m_tDiscussParam.m_byMixMode; }
    void   SetMixModeParam(u8 byMixMode) { m_tDiscussParam.m_byMixMode = byMixMode; }
    u8     GetMixMemberNum(void) const { return m_tDiscussParam.m_byMemberNum; }
    void   SetDiscussMember(TDiscussParamV4R4B2 tAudMixParam)
    { memcpy(m_tDiscussParam.m_atMtMember, tAudMixParam.m_atMtMember, sizeof(tAudMixParam.m_atMtMember)); } 
    void   SetDiscussParam(TDiscussParamV4R4B2 tAudMixParam){ m_tDiscussParam = tAudMixParam; } 
    TDiscussParamV4R4B2 GetDiscussParam(void) const { return m_tDiscussParam; }

    //画面合成方式
	void   SetVMPMode(u8   byVMPMode){ m_tVMPParam.SetVMPMode(byVMPMode); } 
    u8     GetVMPMode( void ) const { return m_tVMPParam.GetVMPMode(); }
	BOOL   IsBrdstVMP( void ) const { return m_tVMPParam.GetVMPMode() != CONF_VMPMODE_NONE && m_tVMPParam.IsVMPBrdst(); }
	void   SetVmpBrdst( BOOL bBrdst ){ m_tVMPParam.SetVMPBrdst( bBrdst ); }
	u8     GetVmpStyle( void ){ return m_tVMPParam.GetVMPStyle(); }
	void   SetVmpStyle( u8 byVMPStyle ){ m_tVMPParam.SetVMPStyle( byVMPStyle ); }

    //vmp tvwall    
	void      SetVmpTwMode(u8 byVmpTwMode){ m_tVmpTwParam.SetVMPMode(byVmpTwMode); }
    u8        GetVmpTwMode(void) const { return m_tVmpTwParam.GetVMPMode(); }
    void      SetVmpTwParam(TVMPParamV4R4B2 tVmpTwParam) { m_tVmpTwParam = tVmpTwParam; }
    TVMPParamV4R4B2 GetVmpTwParam(void) const { return m_tVmpTwParam; }
    u8        GetVmpTwStyle(void) { return m_tVmpTwParam.GetVMPStyle(); }
	void      SetVmpTwStyle(u8 byVmpTwStyle) { m_tVmpTwParam.SetVMPStyle(byVmpTwStyle); }

	//强制广播(跟原来强制发言人的概念不同，接口名称暂时不修改)
	BOOL IsMustSeeSpeaker(void) const { return m_tConfMode.IsForceRcvSpeaker(); }
	void SetMustSeeSpeaker(BOOL bMustSeeSpeaker) { m_tConfMode.SetForceRcvSpeaker( bMustSeeSpeaker ); }

	//码率适配状态 -- modify bas 2
    BOOL32 IsConfAdapting(void) const { return CONF_BASMODE_NONE != m_tConfMode.GetBasModeValue(); } 
    BOOL32 IsAudAdapting(void) const { return m_tConfMode.GetBasMode(CONF_BASMODE_AUD); }
    BOOL32 IsVidAdapting(void) const { return m_tConfMode.GetBasMode(CONF_BASMODE_VID); }
    BOOL32 IsBrAdapting(void) const { return m_tConfMode.GetBasMode(CONF_BASMODE_BR); }
    BOOL32 IsCasdAudAdapting(void) const { return m_tConfMode.GetBasMode(CONF_BASMODE_CASDAUD); }
    BOOL32 IsCasdVidAdapting(void) const { return m_tConfMode.GetBasMode(CONF_BASMODE_CASDVID); }
    void SetAdaptMode(u8 byBasMode, BOOL32 bAdapt)
    { 
        m_tConfMode.SetBasMode(byBasMode, bAdapt);
    }

	//会议主席状态
	BOOL IsNoChairMode( void ) const { return m_tConfMode.IsNoChairMode(); }
    void SetNoChairMode( BOOL bNoChairMode ) { m_tConfMode.SetNoChairMode( bNoChairMode ); }

	//轮询方式
	void   SetPollMode(u8   byPollMode){ m_tConfMode.SetPollMode( byPollMode ); } 
    u8     GetPollMode(){ return m_tConfMode.GetPollMode(); }
	
	//轮询参数
    void   SetPollInfo(const TPollInfo &tPollInfo){ m_tPollInfo = tPollInfo;} 
    TPollInfo  *GetPollInfo( void ) { return &m_tPollInfo; }
	void   SetPollMedia(u8   byMediaMode){ m_tPollInfo.SetMediaMode( byMediaMode ); } 
    u8     GetPollMedia( void ) const{ return m_tPollInfo.GetMediaMode(); }
	void   SetPollState(u8   byPollState){ m_tPollInfo.SetPollState( byPollState );} 
    u8     GetPollState( void ) const { return m_tPollInfo.GetPollState(); }
    void   SetMtPollParam(TMtPollParam tMtPollParam){ m_tPollInfo.SetMtPollParam( tMtPollParam ); } 
    TMtPollParam  GetMtPollParam() const{ return m_tPollInfo.GetMtPollParam(); }

    //电视墙轮询参数
    void   SetTvWallPollInfo(const TTvWallPollInfoV4R4B2 &tPollInfo){ m_tTvWallPollInfo = tPollInfo;} 
    TTvWallPollInfoV4R4B2  *GetTvWallPollInfo( void ) { return &m_tTvWallPollInfo; }    
    void   SetTvWallPollState(u8   byPollState){ m_tTvWallPollInfo.SetPollState( byPollState );} 
    u8     GetTvWallPollState( void ) const { return m_tTvWallPollInfo.GetPollState(); }
    void   SetTvWallMtPollParam(TMtPollParam tMtPollParam){ m_tTvWallPollInfo.SetMtPollParam( tMtPollParam ); } 
    TMtPollParam  GetTvWallMtPollParam(){ return m_tTvWallPollInfo.GetMtPollParam(); }

	//会议录像状态	
	BOOL IsNoRecording( void ) const{ return m_tConfMode.GetRecordMode() == CONF_RECMODE_NONE ? TRUE : FALSE; }
	void SetNoRecording( void ){ m_tConfMode.SetRecordMode( CONF_RECMODE_NONE ); }
	BOOL IsRecording( void ) const{ return m_tConfMode.GetRecordMode() == CONF_RECMODE_REC ? TRUE : FALSE; }
	void SetRecording( void ){ m_tConfMode.SetRecordMode( CONF_RECMODE_REC ); }
	BOOL IsRecPause( void ) const{ return m_tConfMode.GetRecordMode() == CONF_RECMODE_PAUSE ? TRUE : FALSE; }
	void SetRecPause( void ){ m_tConfMode.SetRecordMode( CONF_RECMODE_PAUSE ); }

	//会议放像状态
	BOOL IsNoPlaying( void ) const{ return m_tConfMode.GetPlayMode() == CONF_PLAYMODE_NONE ? TRUE : FALSE; }
	void SetNoPlaying( void ){ m_tConfMode.SetPlayMode( CONF_PLAYMODE_NONE ); }
	BOOL IsPlaying( void ) const { return m_tConfMode.GetPlayMode() == CONF_PLAYMODE_PLAY ? TRUE : FALSE; }
	void SetPlaying( void ){ m_tConfMode.SetPlayMode( CONF_PLAYMODE_PLAY ); }
	BOOL IsPlayPause( void )const { return m_tConfMode.GetPlayMode() == CONF_PLAYMODE_PAUSE ? TRUE : FALSE; }
	void SetPlayPause( void ){ m_tConfMode.SetPlayMode( CONF_PLAYMODE_PAUSE ); }
	BOOL IsPlayFF( void )const { return m_tConfMode.GetPlayMode() == CONF_PLAYMODE_FF ? TRUE : FALSE; }
	void SetPlayFF( void ) { m_tConfMode.SetPlayMode( CONF_PLAYMODE_FF ); }
	BOOL IsPlayFB( void )const { return m_tConfMode.GetPlayMode() == CONF_PLAYMODE_FB ? TRUE : FALSE; }
	void SetPlayFB( void ){ m_tConfMode.SetPlayMode( CONF_PLAYMODE_FB ); }
	// zgc, 2007-02-27, 会议放像关键帧延迟问题
	BOOL IsPlayReady( void ) const{ return m_tConfMode.GetPlayMode() == CONF_PLAYMODE_PLAYREADY ? TRUE : FALSE; }
	void SetPlayReady( void ){ m_tConfMode.SetPlayMode( CONF_PLAYMODE_PLAYREADY ); }
	// zgc, 2007-02-27, end

	//包重传状态
	void SetPrsing( BOOL bPrs ){ m_byPrs = bPrs; }
	BOOL IsPrsing( void ){ return m_byPrs; }

	//注册GK情况
	void SetRegToGK( BOOL bRegToGK ){ m_tConfMode.SetRegToGK( bRegToGK ); }
	BOOL IsRegToGK( void ) const{ return m_tConfMode.IsRegToGK(); }

    // xsl [11/16/2006] GK计费情况
    void SetGkCharge( BOOL bCharge ) { m_tConfMode.SetGkCharge( bCharge ); }
    BOOL IsGkCharge( void ) const { return m_tConfMode.IsGkCharge(); }

	void Print( void ) const
	{
		OspPrintf( TRUE, FALSE, "\nConfStatus:\n" );
        if(m_tChairman.IsNull())  
            OspPrintf( TRUE, FALSE, "Chairman: Null\n" );
        else 
            OspPrintf( TRUE, FALSE, "Chairman: Mcu%dMt%d\n", m_tChairman.GetMcuId(), m_tChairman.GetMtId() );
        if(m_tSpeaker.IsNull())
            OspPrintf( TRUE, FALSE, "Speaker: Null\n" );
        else
		    if (m_tSpeaker.GetType() == TYPE_MT)
                OspPrintf( TRUE, FALSE, "Speaker: Mcu%dMt%d\n", m_tSpeaker.GetMcuId(), m_tSpeaker.GetMtId() );
            else
                OspPrintf( TRUE, FALSE, "Speaker: Mcu%dEqp%d\n", m_tSpeaker.GetMcuId(), m_tSpeaker.GetEqpId() );
        OspPrintf( TRUE, FALSE, "VmpMode: %d\n", m_tVMPParam.GetVMPMode() );
        OspPrintf( TRUE, FALSE, "TWVmpMode: %d\n", m_tVmpTwParam.GetVMPMode() );
        OspPrintf( TRUE, FALSE, "MixMode: %d\n", GetMixModeParam() );
        /*
        if ( mcuNoMix != GetDiscussParam() )
        {
            for (...)
        }
        */
	}
}
/*
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
*/
PACKED
;


//定义会议信息结构,该结构定义了会议基本信息和状态 (len:1004)
//由于给会议文件增加了文件头，其中包括TConfInfo的结构体长度，
//因此今后给TConfInfo添加数据成员变量必须添加在所有数据成员的末尾，否则会导致读取会议文件出错
//周广程，2007-06-27
struct TConfInfoV4R4B2
{
protected:
    CConfId       m_cConfId;                        //会议号，全网唯一
    TKdvTime      m_tStartTime;                     //开始时间，控制台填0为立即开始
    u16           m_wDuration;                      //持续时间(分钟)，0表示不自动停止
    u16           m_wBitRate;                       //会议码率(单位:Kbps,1K=1024)
    u16           m_wSecBitRate;                    //双速会议的第2码率(单位:Kbps,为0表示是单速会议)
	u16			  m_wDcsBitRate;					//数据会议码率(单位:Kbps,为0表示不支持数据会议)
    u8            m_byDStreamScale;                 //双流百分比
    u8            m_byUsrGrpId;                     //用户组
    u8            m_byTalkHoldTime;                 //最小发言持续时间(单位:秒)
    u16            m_wMixDelayTime;                 //混音延时时间
    TCapSupport   m_tCapSupport;                    //会议支持的媒体
    s8            m_achConfPwd[MAXLEN_PWD+1];       //会议密码
    s8            m_achConfName[MAXLEN_CONFNAME+1]; //会议名
    s8            m_achConfE164[MAXLEN_E164+1];     //会议的E164号码
    TMtAlias      m_tChairAlias;                    //会议中主席的别名
    TMtAlias      m_tSpeakerAlias;                  //会议中发言人的别名
    TConfAttrb    m_tConfAttrb;                     //会议属性

    TMediaEncrypt m_tMediaKey;                      //第一版本所有密钥一样，不支持更新    
public:
    TConfStatusV4R4B2   m_tStatus;                        //会议状态

    // 4.0R3 版本扩展字段
protected:
    TConfAttrbEx	  m_tConfAttrbEx;                   //会议扩展属性
	TCapSupportEx	  m_tCapSupportEx;					//会议能力集扩展属性
    
public:	
    TConfInfoV4R4B2( void )
    { 
        memset( this, 0, sizeof( TConfInfoV4R4B2 ) ); 
        m_tCapSupport.Clear(); 
        m_tConfAttrbEx.Reset(); 
    }
    CConfId GetConfId( void ) const { return m_cConfId; }
    void    SetConfId( CConfId cConfId ){ m_cConfId = cConfId; }
    void    SetUsrGrpId(u8 byGrpId) { m_byUsrGrpId = byGrpId; }
    u8      GetUsrGrpId( void ) const { return m_byUsrGrpId; }
    TKdvTime GetKdvStartTime( void ) const { return( m_tStartTime ); }
    void   SetKdvStartTime( TKdvTime tStartTime ){ m_tStartTime = tStartTime; }
    void   SetDuration(u16  wDuration){ m_wDuration = htons(wDuration);} 
    u16    GetDuration( void ) const { return ntohs(m_wDuration); }
    void   SetBitRate(u16  wBitRate){ m_wBitRate = htons(wBitRate);} 
    u16    GetBitRate( void ) const { return ntohs(m_wBitRate); }
    void   SetSecBitRate(u16 wSecBitRate){ m_wSecBitRate = htons(wSecBitRate);} 
    u16    GetSecBitRate( void ) const { return ntohs(m_wSecBitRate); }
	void   SetDcsBitRate(u16 wDcsBitRate){ m_wDcsBitRate = htons(wDcsBitRate); }
	u16	   GetDcsBitRate( void ) const { return ntohs(m_wDcsBitRate); }

    void   SetMainVideoFormat(u8 byVideoFormat)
    { 
        TSimCapSet tSim = m_tCapSupport.GetMainSimCapSet();
        tSim.SetVideoResolution(byVideoFormat); 
        m_tCapSupport.SetMainSimCapSet(tSim);
    } 
    u8     GetMainVideoFormat( void ) const { return m_tCapSupport.GetMainSimCapSet().GetVideoResolution(); }
    void   SetSecVideoFormat(u8 byVideoFormat)
    { 
        TSimCapSet tSim = m_tCapSupport.GetSecondSimCapSet();
        tSim.SetVideoResolution(byVideoFormat);
        m_tCapSupport.SetSecondSimCapSet(tSim); 
    } 
    u8     GetSecVideoFormat(void) const { return m_tCapSupport.GetSecondSimCapSet().GetVideoResolution(); }
    void   SetDoubleVideoFormat(u8 byVideoFormat) { m_tCapSupport.SetDStreamResolution(byVideoFormat); } 
    u8     GetDoubleVideoFormat(void) const { return m_tCapSupport.GetDStreamResolution(); }
    u8     GetVideoFormat(u8 byVideoType, u8 byChanNo = LOGCHL_VIDEO);
    u8     GetDStreamScale(void) const 
    { 
        u8 byScale = m_byDStreamScale;
        if (byScale > MAXNUM_DSTREAM_SCALE || byScale < MINNUM_DSTREAM_SCALE)
        {
            byScale = DEF_DSTREAM_SCALE;
        }       
        
        return byScale; 
    }
    void   SetDStreamScale(u8 byDStreamScale) { m_byDStreamScale = byDStreamScale; }

    void   SetMixDelayTime(u16 wDelayTime) { m_wMixDelayTime = htons(wDelayTime); }
    u16    GetMixDelayTime(void) { return ntohs(m_wMixDelayTime); }
    void   SetTalkHoldTime(u8   byTalkHoldTime){ m_byTalkHoldTime = byTalkHoldTime;} 
    u8     GetTalkHoldTime( void ) const { return m_byTalkHoldTime; }
    void   SetCapSupport(TCapSupport tCapSupport){ m_tCapSupport = tCapSupport;} 
    TCapSupport GetCapSupport( void ) const { return m_tCapSupport; }
    void   SetConfPwd( LPCSTR lpszConfPwd );
    LPCSTR GetConfPwd( void ) const { return m_achConfPwd; }
    void   SetConfName( LPCSTR lpszConfName );
    LPCSTR GetConfName( void ) const { return m_achConfName; }
    void   SetConfE164( LPCSTR lpszConfE164 );
    LPCSTR GetConfE164( void ) const { return m_achConfE164; }
    void   SetChairAlias(TMtAlias tChairAlias){ m_tChairAlias = tChairAlias;} 
    TMtAlias  GetChairAlias( void ) const { return m_tChairAlias; }
    void   SetSpeakerAlias(TMtAlias tSpeakerAlias){ m_tSpeakerAlias = tSpeakerAlias;} 
    TMtAlias  GetSpeakerAlias( void ) const { return m_tSpeakerAlias; }
    void   SetConfAttrb(TConfAttrb tConfAttrb){ m_tConfAttrb = tConfAttrb;} 
    TConfAttrb  GetConfAttrb( void ) const { return m_tConfAttrb; }
    void   SetStatus(TConfStatusV4R4B2 tStatus){ m_tStatus = tStatus;} 
    TConfStatusV4R4B2 GetStatus( void ) const { return m_tStatus; }	

    void    SetHasVmpModule(BOOL32 bHas) { m_tConfAttrb.SetHasVmpModule(bHas); }
    void    SetHasTvwallModule(BOOL32 bHas) { m_tConfAttrb.SetHasTvWallModule(bHas); }

	TMediaEncrypt& GetMediaKey(void) { return m_tMediaKey; };
	void SetMediaKey(TMediaEncrypt& tMediaEncrypt){ memcpy(&m_tMediaKey, &tMediaEncrypt, sizeof(tMediaEncrypt));}

	void   GetVideoScale(u8 byVideoType, u16 &wVideoWidth, u16 &wVideoHeight, u8* pbyVideoResolution = NULL);
	BOOL   HasChairman( void ) const{ if(m_tStatus.m_tChairman.GetMtId()==0)return FALSE; return TRUE; }	
	TMt    GetChairman( void ) const{ return m_tStatus.m_tChairman; }
	void   SetNoChairman( void ){ memset( &m_tStatus.m_tChairman, 0, sizeof( TMt ) ); }
	void   SetChairman( TMt tChairman ){ m_tStatus.m_tChairman = tChairman; }
	BOOL   HasSpeaker( void ) const{ if(m_tStatus.m_tSpeaker.GetMtId()==0)return FALSE; return TRUE; }
    TMt    GetSpeaker( void ) const{ return m_tStatus.m_tSpeaker; }
	void   SetNoSpeaker( void ){ memset( &m_tStatus.m_tSpeaker, 0, sizeof( TMt ) ); }
	void   SetSpeaker( TMt tSpeaker ){ m_tStatus.m_tSpeaker = tSpeaker; }
	time_t GetStartTime( void ) const;	
	void   SetStartTime( time_t dwStartTime );

    u8     GetMainVideoMediaType( void ) const { return m_tCapSupport.GetMainVideoType();   }
    u8     GetMainAudioMediaType( void ) const { return m_tCapSupport.GetMainAudioType();   }
    u8     GetSecVideoMediaType( void ) const { return m_tCapSupport.GetSecVideoType(); }
    u8     GetSecAudioMediaType( void ) const { return m_tCapSupport.GetSecAudioType(); }

    void         SetConfAttrbEx(TConfAttrbEx tAttrbEx) { m_tConfAttrbEx = tAttrbEx; }
    TConfAttrbEx GetConfAttrbEx() const {return m_tConfAttrbEx; }
    void   SetSatDCastChnlNum(u8 byNum) { m_tConfAttrbEx.SetSatDCastChnlNum(byNum); }
    u8     GetSatDCastChnlNum() const { return m_tConfAttrbEx.GetSatDCastChnlNum(); }
    void   SetAutoRecAttrb(const TConfAutoRecAttrb& tAttrb ) { m_tConfAttrbEx.SetAutoRecAttrb(tAttrb); }
    TConfAutoRecAttrb GetAutoRecAttrb() const { return m_tConfAttrbEx.GetAutoRecAttrb(); }
    void   SetVmpAutoAdapt(BOOL32 bAutoAdapt) { m_tConfAttrbEx.SetVmpAutoAdapt(bAutoAdapt); }
    BOOL   IsVmpAutoAdapt() const { return m_tConfAttrbEx.IsVmpAutoAdapt(); }
    BOOL   IsSupportGkCharge(void) const { return ( strlen(m_tConfAttrbEx.GetGKUsrName()) > 0 ? TRUE : FALSE ); }
    void   SetDiscussAutoStop() { m_tConfAttrbEx.SetDiscussAutoStop(); }
    void   SetDiscussAutoSwitch2Mix() { m_tConfAttrbEx.SetDiscussAutoSwitch2Mix(); }
    BOOL   IsDiscussAutoStop() const { return m_tConfAttrbEx.IsDiscussAutoStop(); }

	//会议能力集扩展属性, zgc, 2007-09-26
	BOOL32	IsSupportFEC(void) const { return !m_tCapSupportEx.IsNoSupportFEC(); }
	TCapSupportEx GetCapSupportEx(void) const { return m_tCapSupportEx; }
	void	SetCapSupportEx(TCapSupportEx tCapSupportEx) { m_tCapSupportEx = tCapSupportEx; }

	void Print( void ) const
	{
		OspPrintf( TRUE, FALSE, "\nConfInfo:\n" );
		OspPrintf( TRUE, FALSE, "m_cConfId: " );
		m_cConfId.Print();
        OspPrintf( TRUE, FALSE, "m_byUsrGrpId: %d\n", GetUsrGrpId());
		OspPrintf( TRUE, FALSE, "\nm_tStartTime: " );
        m_tStartTime.Print();
		OspPrintf( TRUE, FALSE, "\nm_wDuration: %d\n", ntohs(m_wDuration) );
		OspPrintf( TRUE, FALSE, "m_wBitRate: %d\n", ntohs(m_wBitRate) );
        OspPrintf( TRUE, FALSE, "m_wSecBitRate: %d\n", ntohs(m_wSecBitRate) );
		OspPrintf( TRUE, FALSE, "m_wDcsBitRata: %d\n", ntohs(m_wDcsBitRate) );
		OspPrintf( TRUE, FALSE, "m_byVideoFormat: %d\n", GetMainVideoFormat() );
		OspPrintf( TRUE, FALSE, "m_byTalkHoldTime: %d\n", m_byTalkHoldTime );
        OspPrintf( TRUE, FALSE, "m_byDStreamScale: %d\n", m_byDStreamScale );
		OspPrintf( TRUE, FALSE, "m_tCapSupport:\n" );
	    m_tCapSupport.Print();
		OspPrintf( TRUE, FALSE, "\nm_achConfPwd: %s\n", m_achConfPwd );
		OspPrintf( TRUE, FALSE, "m_achConfName: %s\n", m_achConfName );
		OspPrintf( TRUE, FALSE, "m_achConfE164: %s\n", m_achConfE164 );
        OspPrintf( TRUE, FALSE, "m_tChairAlias:" );
        m_tChairAlias.Print();
        OspPrintf( TRUE, FALSE, "m_tSpeakerAlias:" );
        m_tSpeakerAlias.Print();
		OspPrintf( TRUE, FALSE, "\n" );
        OspPrintf( TRUE, FALSE, "m_tConfAttrbEx: \n" );
        m_tConfAttrbEx.Print();
		OspPrintf( TRUE, FALSE, "m_tCapSupportEx: \n" );
		m_tCapSupportEx.Print();
	}
}
/*
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
*/
PACKED
;


/*-------------------------------------------------------------------
                               TConfInfo                             
--------------------------------------------------------------------*/

/*====================================================================
    函数名      ：GetVideoFormat
    功能        ：获取图像的分辨率
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byVideoType,
                  u8 byChanNo
    返回值说明  ：图像的分辨率
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/09/21    4.0         libo          创建
====================================================================*/
inline u8 TConfInfoV4R4B2::GetVideoFormat(u8 byVideoType, u8 byChanNo)
{
    if (LOGCHL_VIDEO == byChanNo)
    {
        if (m_tCapSupport.GetMainSimCapSet().GetVideoMediaType() == byVideoType)
        {
            return m_tCapSupport.GetMainSimCapSet().GetVideoResolution();
        }
        else
        {
            return m_tCapSupport.GetSecondSimCapSet().GetVideoResolution();
        }
    }
    else
    {
        return m_tCapSupport.GetDStreamResolution();
    }
}

/*====================================================================
    函数名      ：GetVideoScale
    功能        ：得到视频规模
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 &wVideoWidth 视频宽度
                  u16 &wVideoHeight 视频高度	              
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/05    3.0         胡昌威          创建             
====================================================================*/
inline void  TConfInfoV4R4B2::GetVideoScale(u8 byVideoType, u16 &wVideoWidth, u16 &wVideoHeight, u8* pbyVideoResolution/* = NULL */)
{
   
    u8 byVideoFormat;
    if (m_tCapSupport.GetMainSimCapSet().GetVideoMediaType() == byVideoType)
    {
        byVideoFormat = m_tCapSupport.GetMainSimCapSet().GetVideoResolution();
    }
    else
    {
        byVideoFormat = m_tCapSupport.GetSecondSimCapSet().GetVideoResolution();
    }
    if( NULL != pbyVideoResolution )
    {
        byVideoFormat = *pbyVideoResolution;
    }
    
    //  [4/24/2006] mpeg4 16cif 按auto处理
    if (MEDIA_TYPE_MP4 == byVideoType && VIDEO_FORMAT_16CIF == byVideoFormat)
    {
        byVideoFormat = VIDEO_FORMAT_AUTO;
    }

    switch (byVideoFormat)
	{
    case VIDEO_FORMAT_SQCIF:
		wVideoWidth = 128;
        wVideoHeight = 96;
    	break;
		
    case VIDEO_FORMAT_QCIF:
		wVideoWidth = 176;
        wVideoHeight = 144;
    	break;

    case VIDEO_FORMAT_CIF:
		wVideoWidth = 352;
        wVideoHeight = 288;
    	break;

    case VIDEO_FORMAT_2CIF:
        wVideoWidth = 352;
        wVideoHeight = 576;
        break;

	case VIDEO_FORMAT_4CIF:
		wVideoWidth = 704;
        wVideoHeight = 576;
		break;

	case VIDEO_FORMAT_16CIF:
		wVideoWidth = 1408;
        wVideoHeight = 1152;
		break;

    case VIDEO_FORMAT_SIF:
        wVideoWidth = 352;
        wVideoHeight = 240;
        break;

    case VIDEO_FORMAT_4SIF:
        wVideoWidth = 704;
        wVideoHeight = 480;
        break;

    case VIDEO_FORMAT_VGA:
        wVideoWidth = 640;
        wVideoHeight = 480;
        break;

    case VIDEO_FORMAT_SVGA:
        wVideoWidth = 800;
        wVideoHeight = 600;
        break;

    case VIDEO_FORMAT_XGA:
        wVideoWidth = 1024;
        wVideoHeight = 768;
        break;

	default:
		wVideoWidth = 352;
        wVideoHeight = 288;
		break;
    }

}

/*====================================================================
    函数名      ：GetStartTime
    功能        ：获得该会议开始时间
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：time_t类型时间（local Time），控制台填0为立即开始
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/30    1.0         LI Yi         创建
    02/08/23    1.0         LI Yi         将time_t型时间转换为字符串型保存
====================================================================*/
inline time_t TConfInfoV4R4B2::GetStartTime( void ) const
{
	time_t	dwStartTime;
	
	if( m_tStartTime.GetYear() != 0 )	//不是即时会议
		m_tStartTime.GetTime( dwStartTime );
	else
		dwStartTime = 0;

	return( dwStartTime );
}

/*====================================================================
    函数名      ：SetStartTime
    功能        ：设置该会议开始时间
    算法实现    ：
    引用全局变量：
    输入参数说明：time_t dwStartTime, 开始时间，0为立即开始
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/31    1.0         LI Yi         创建
    02/08/23    1.0         LI Yi         将time_t型时间转换为字符串型保存
====================================================================*/
inline void TConfInfoV4R4B2::SetStartTime( time_t dwStartTime )
{
	if( dwStartTime != 0 )	//不是即时会议
		m_tStartTime.SetTime( &dwStartTime );
	else
		memset( &m_tStartTime, 0, sizeof( TKdvTime ) );
}


/*====================================================================
    函数名      ：SetConfPwd
    功能        ：设置别名
    算法实现    ：
    引用全局变量：
    输入参数说明：LPCSTR lpszConfPwd, 别名
    返回值说明  ：字符串指针
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/26    1.0         LI Yi         创建
====================================================================*/
inline void TConfInfoV4R4B2::SetConfPwd( LPCSTR lpszConfPwd )
{
	if( lpszConfPwd != NULL )
	{
		strncpy( m_achConfPwd, lpszConfPwd, sizeof( m_achConfPwd ) );
		m_achConfPwd[sizeof( m_achConfPwd ) - 1] = '\0';
	}
	else
	{
		memset( m_achConfPwd, 0, sizeof( m_achConfPwd ) );
	}
}

/*====================================================================
    函数名      ：SetConfName
    功能        ：设置别名
    算法实现    ：
    引用全局变量：
    输入参数说明：LPCSTR lpszConfName, 别名
    返回值说明  ：字符串指针
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/26    1.0         LI Yi         创建
====================================================================*/
inline void TConfInfoV4R4B2::SetConfName( LPCSTR lpszConfName )
{
	if( lpszConfName != NULL )
	{
		strncpy( m_achConfName, lpszConfName, sizeof( m_achConfName ) );
		m_achConfName[sizeof( m_achConfName ) - 1] = '\0';
	}
	else
	{
		memset( m_achConfName, 0, sizeof( m_achConfName ) );
	}
}

/*====================================================================
    函数名      ：SetConfE164
    功能        ：设置别名
    算法实现    ：
    引用全局变量：
    输入参数说明：LPCSTR lpszConfE164, 别名
    返回值说明  ：字符串指针
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/26    1.0         LI Yi         创建
====================================================================*/
inline void TConfInfoV4R4B2::SetConfE164( LPCSTR lpszConfE164 )
{
	if( lpszConfE164 != NULL )
	{
		strncpy( m_achConfE164, lpszConfE164, sizeof( m_achConfE164 ) );
		m_achConfE164[sizeof( m_achConfE164 ) - 1] = '\0';
	}
	else
	{
		memset( m_achConfE164, 0, sizeof( m_achConfE164 ) );
	}
}
#define  MAXNUM_MIXING_MEMBER_BEFORE        56
struct TMixParamV4R5
{
public:
    TMixParamV4R5(void)
    {
        Clear();
    }
    void Clear(void){ memset(this, 0, sizeof(TMixParamV4R5));    }

    void SetMode(u8 byMixMode){ m_byMixMode = byMixMode;    }
    u8   GetMode(void) const { return m_byMixMode;    }

    u8   GetMemberNum() const
    {
        return m_byMtNum;
    }
    void AddMember(u8 byMtId)
    {
        if (byMtId == 0)
            return;
        
        if (m_byMtNum < MAXNUM_MIXING_MEMBER_BEFORE)
        {
            m_abyMtList[m_byMtNum++] = byMtId;
        }        
    }
    void DelMember(u8 byMtId)
    {
        if (byMtId == 0)
            return;

        for (u8 byLoop = 0; byLoop < m_byMtNum; byLoop ++ )
        {
            if (m_abyMtList[byLoop] == byMtId)
            {
                for ( u8 byMove = byLoop+1; byMove < m_byMtNum; byMove ++ )
                {
                    m_abyMtList[byMove-1] =  m_abyMtList[byMove];
                }
                m_byMtNum --;
                break;
            }
        }
    }

    void ClearAllMembers()
    {
        m_byMtNum = 0;
    }

    u8 GetMemberByIdx(u8 byIdx)
    {
        if ( byIdx < m_byMtNum )
            return m_abyMtList[byIdx];
        else
            return 0;
    }

    BOOL32 IsMtInMember(u8 byMtId) const
    {
        for (u8 byLoop = 0; byLoop < m_byMtNum; byLoop ++ )
        {
            if (m_abyMtList[byLoop] == byMtId)
            {
                return TRUE;
            }
        }
        return FALSE;
    }
protected:
    u8  m_byMixMode;    //混音模式：mcuNoMix - 不混音
                        //          mcuWholeMix - 全体混音
                        //          mcuPartMix - 定制混音
                        //          mcuVacMix, - VAC 
                        //          mcuVacWholeMix - 带Vac的全体混音
    u8  m_byMtNum;
    u8  m_abyMtList[MAXNUM_MIXING_MEMBER_BEFORE];
private:
    u8  m_abyReserved[4];
}
PACKED
;
// xliang [2/24/2009] 4.5 TConfStatus
struct TConfStatusV4R5
{
public:
    TMt 	      m_tChairman;	      //主席终端，MCU号为0表示无主席
    TMt		      m_tSpeaker;		  //发言终端，MCU号为0表示无发言人
    TConfMode     m_tConfMode;        //会议方式
    TRecProg	  m_tRecProg;		  //当前录像进程,仅录像状态时有效
    TRecProg	  m_tPlayProg;	      //当前放像进程,仅放像状态时有效
    TPollInfo     m_tPollInfo;        //会议轮询参数,仅轮询时有较
    TTvWallPollInfoV4R5 m_tTvWallPollInfo;  //电视墙当前轮询参数，仅轮询时有效
    TMixParamV4R5     m_tMixParam;        //当前混音参数
    TVMPParamV4R5     m_tVMPParam;        //当前视频复合参数，仅视频复合时有效
    TVMPParamV4R5     m_tVmpTwParam;      //当前VMPTW参数
    u8            m_byPrs;            //当前包重传状态

public:
    BOOL   HasChairman(void) const { return m_tChairman.GetMtId()==0 ? FALSE : TRUE; }	
	TMt    GetChairman(void) const { return m_tChairman; }
	void   SetNoChairman(void) { memset( &m_tChairman, 0, sizeof( TMt ) ); }
	void   SetChairman(TMt tChairman) { m_tChairman = tChairman; }
	BOOL   HasSpeaker(void) const { return m_tSpeaker.GetMtId()==0 ? FALSE : TRUE; }
    TMt    GetSpeaker(void) const { return m_tSpeaker; }
	void   SetNoSpeaker(void) { memset( &m_tSpeaker, 0, sizeof( TMt ) ); }
	void   SetSpeaker(TMt tSpeaker) { m_tSpeaker = tSpeaker; }
    void          SetConfMode(TConfMode tConfMode) { m_tConfMode = tConfMode; } 
    TConfMode     GetConfMode(void) const { return m_tConfMode; }
    void          SetRecProg(TRecProg tRecProg){ m_tRecProg = tRecProg; } 
    TRecProg      GetRecProg(void) const { return m_tRecProg; }
    void          SetPlayProg(TRecProg tPlayProg){ m_tPlayProg = tPlayProg; } 
    TRecProg      GetPlayProg(void) const { return m_tPlayProg; }
    void          SetVmpParam(TVMPParamV4R5 tVMPParam){ m_tVMPParam = tVMPParam;} 
    TVMPParamV4R5     GetVmpParam(void) const { return m_tVMPParam; }

	//会议进行状态
	BOOL IsScheduled( void ) const { return m_tConfMode.GetTakeMode() == CONF_TAKEMODE_SCHEDULED ? TRUE : FALSE; }
	void SetScheduled( void ){ m_tConfMode.SetTakeMode( CONF_TAKEMODE_SCHEDULED ); }
	BOOL IsOngoing( void ) const { return m_tConfMode.GetTakeMode() == CONF_TAKEMODE_ONGOING ? TRUE : FALSE; }
	void SetOngoing( void ){ m_tConfMode.SetTakeMode( CONF_TAKEMODE_ONGOING ); }
	BOOL IsTemplate( void ) const { return m_tConfMode.GetTakeMode() == CONF_TAKEMODE_TEMPLATE ? TRUE : FALSE; }
	void SetTemplate( void ){ m_tConfMode.SetTakeMode( CONF_TAKEMODE_TEMPLATE ); }
	u8   GetTakeMode( void ) const{ return m_tConfMode.GetTakeMode();	}
	void SetTakeMode( u8 byTakeMode ){ m_tConfMode.SetTakeMode( byTakeMode ); }
    void SetTakeFromFile(BOOL32 bTakeFromFile) { m_tConfMode.SetTakeFromFile(bTakeFromFile); }  // MCU 内部使用
    BOOL IsTakeFromFile(void) const{ return m_tConfMode.IsTakeFromFile(); }

	//会议保护状态
	u8   GetProtectMode( void ) const { return m_tConfMode.GetLockMode(); }
    void SetProtectMode( u8   byProtectMode ) { m_tConfMode.SetLockMode( byProtectMode ); }
	
	//会议呼叫策略
	u8   GetCallMode( void ) const { return m_tConfMode.GetCallMode(); }
	void SetCallMode( u8   byCallMode ){ m_tConfMode.SetCallMode( byCallMode ); }
	void SetCallInterval(u32 dwCallInterval){ m_tConfMode.SetCallInterval( dwCallInterval ); } 
    u32  GetCallInterval( void ) const { return m_tConfMode.GetCallInterval( ); }
	void SetCallTimes(u32 dwCallTimes){ m_tConfMode.SetCallTimes( dwCallTimes ); } 
    u32  GetCallTimes( void ) const { return m_tConfMode.GetCallTimes( ); }

    //会议点名模式
    u8   GetRollCallMode( void ) const { return m_tConfMode.GetRollCallMode();    }
    void SetRollCallMode( u8 byMode ) { m_tConfMode.SetRollCallMode(byMode);    }
    
    //混音器状态(标识是否在使用混音器，无论是智能混音、定制混音还是语音激励)    

    //直接获取混音器状态参数
    u8     GetMixerMode(void) const { return m_tMixParam.GetMode(); }
    void   SetMixerMode(u8 byMixMode) { m_tMixParam.SetMode(byMixMode); }
    
    TMixParamV4R5 GetMixerParam(void) const { return m_tMixParam; }
    void   SetMixerParam(TMixParamV4R5 tAudMixParam){ m_tMixParam = tAudMixParam; }     
    
    //是否在混音(智能或定制)
    BOOL32 IsNoMixing(void) const { return mcuNoMix == GetMixerMode() || mcuVacMix == GetMixerMode(); }
    BOOL32 IsMixing(void) const { return !IsNoMixing(); }
    void   SetNoMixing(void) { SetMixerMode( IsVACing() ? mcuVacMix :mcuNoMix );  }

    //是否智能混音
    BOOL32 IsAutoMixing(void) const 
    {
        return (mcuVacWholeMix == GetMixerMode() || mcuWholeMix == GetMixerMode());
    }
    void   SetAutoMixing(BOOL bAutoMix = TRUE)
    {
        u8 byMixMode = mcuNoMix;
        if ( bAutoMix )
        {
            byMixMode = IsVACing() ? mcuVacWholeMix : mcuWholeMix;
        }
        else
        {
            byMixMode = IsVACing() ? mcuVacMix : mcuNoMix;
        }
        SetMixerMode(byMixMode);
        return;
    }
    
    //是否在定制混音
    BOOL32 IsSpecMixing(void) const
    {
        return (mcuVacPartMix == GetMixerMode() || mcuPartMix == GetMixerMode());
    }
    void   SetSpecMixing(BOOL bMixSpecMt = TRUE)
    {
        u8 byMixMode = mcuNoMix;
        if ( bMixSpecMt )
        {
            byMixMode = IsVACing() ? mcuVacPartMix : mcuPartMix;
        }
        else
        {
            byMixMode = IsVACing() ? mcuVacMix : mcuNoMix;
        }
        SetMixerMode(byMixMode);
    }
    
    //是否在语音激励
    BOOL32 IsVACing(void) const
    {
        return ( mcuVacMix == GetMixerMode() ||
                 mcuVacPartMix == GetMixerMode() || 
                 mcuVacWholeMix == GetMixerMode() );
    }
    void   SetVACing(BOOL bVACMode = TRUE)
    {
        u8 byMixMode = mcuNoMix;
        if ( bVACMode )
        {
            switch(GetMixerMode())
            {
            case mcuNoMix:      byMixMode = mcuVacMix;      break;
            case mcuPartMix:    byMixMode = mcuVacPartMix;  break;
            case mcuWholeMix:   byMixMode = mcuVacWholeMix; break;
            default:            byMixMode = GetMixerMode();   break;
            }
        }
        else
        {
            switch(GetMixerMode())
            {
            case mcuVacMix:         byMixMode = mcuNoMix;       break;
            case mcuVacPartMix:     byMixMode = mcuPartMix;     break;
            case mcuVacWholeMix:    byMixMode = mcuWholeMix;    break;
            default:                byMixMode = GetMixerMode();   break;
            }
        }
        SetMixerMode(byMixMode);
        return;
    }
    
    //画面合成方式
	void   SetVMPMode(u8   byVMPMode){ m_tVMPParam.SetVMPMode(byVMPMode); } 
    u8     GetVMPMode( void ) const { return m_tVMPParam.GetVMPMode(); }
	BOOL   IsBrdstVMP( void ) const { return m_tVMPParam.GetVMPMode() != CONF_VMPMODE_NONE && m_tVMPParam.IsVMPBrdst(); }
	void   SetVmpBrdst( BOOL bBrdst ){ m_tVMPParam.SetVMPBrdst( bBrdst ); }
	u8     GetVmpStyle( void ){ return m_tVMPParam.GetVMPStyle(); }
	void   SetVmpStyle( u8 byVMPStyle ){ m_tVMPParam.SetVMPStyle( byVMPStyle ); }

    //vmp tvwall    
	void      SetVmpTwMode(u8 byVmpTwMode){ m_tVmpTwParam.SetVMPMode(byVmpTwMode); }
    u8        GetVmpTwMode(void) const { return m_tVmpTwParam.GetVMPMode(); }
    void      SetVmpTwParam(TVMPParamV4R5 tVmpTwParam) { m_tVmpTwParam = tVmpTwParam; }
    TVMPParamV4R5 GetVmpTwParam(void) const { return m_tVmpTwParam; }
    u8        GetVmpTwStyle(void) { return m_tVmpTwParam.GetVMPStyle(); }
	void      SetVmpTwStyle(u8 byVmpTwStyle) { m_tVmpTwParam.SetVMPStyle(byVmpTwStyle); }

	//强制广播(跟原来强制发言人的概念不同，接口名称暂时不修改)
	BOOL IsMustSeeSpeaker(void) const { return m_tConfMode.IsForceRcvSpeaker(); }
	void SetMustSeeSpeaker(BOOL bMustSeeSpeaker) { m_tConfMode.SetForceRcvSpeaker( bMustSeeSpeaker ); }

	//码率适配状态 -- modify bas 2
    BOOL32 IsConfAdapting(void) const { return CONF_BASMODE_NONE != m_tConfMode.GetBasModeValue(); } 
    BOOL32 IsAudAdapting(void) const { return m_tConfMode.GetBasMode(CONF_BASMODE_AUD); }
    BOOL32 IsVidAdapting(void) const { return m_tConfMode.GetBasMode(CONF_BASMODE_VID); }
    BOOL32 IsBrAdapting(void) const { return m_tConfMode.GetBasMode(CONF_BASMODE_BR); }
    BOOL32 IsCasdAudAdapting(void) const { return m_tConfMode.GetBasMode(CONF_BASMODE_CASDAUD); }
    BOOL32 IsCasdVidAdapting(void) const { return m_tConfMode.GetBasMode(CONF_BASMODE_CASDVID); }
    void SetAdaptMode(u8 byBasMode, BOOL32 bAdapt)
    { 
        m_tConfMode.SetBasMode(byBasMode, bAdapt);
    }
    // 高清适配状态
	BOOL32 IsConfHDAdapting(void) const { return CONF_HDBASMODE_NONE != m_tConfMode.GetHDBasModeValue(); }
    BOOL32 IsHdVidAdapting(void) const { return m_tConfMode.GetHDBasMode(CONF_HDBASMODE_VID); }
    BOOL32 IsHDDoubleVidAdapting(void) const { return m_tConfMode.GetHDBasMode( CONF_HDBASMODE_DOUBLEVID ) ;}
    BOOL32 IsHdCasdVidAdapting(void) const { return m_tConfMode.GetHDBasMode(CONF_HDBASMODE_CASDVID); }
    void SetHDAdaptMode(u8 byHDBasMode, BOOL32 bAdapt)
    { 
        m_tConfMode.SetHDBasMode(byHDBasMode, bAdapt);
    }

	//会议主席状态
	BOOL IsNoChairMode( void ) const { return m_tConfMode.IsNoChairMode(); }
    void SetNoChairMode( BOOL bNoChairMode ) { m_tConfMode.SetNoChairMode( bNoChairMode ); }

	//轮询方式
	void   SetPollMode(u8   byPollMode){ m_tConfMode.SetPollMode( byPollMode ); } 
    u8     GetPollMode(){ return m_tConfMode.GetPollMode(); }
	
	//轮询参数
    void   SetPollInfo(const TPollInfo &tPollInfo){ m_tPollInfo = tPollInfo;} 
    TPollInfo  *GetPollInfo( void ) { return &m_tPollInfo; }
	void   SetPollMedia(u8   byMediaMode){ m_tPollInfo.SetMediaMode( byMediaMode ); } 
    u8     GetPollMedia( void ) const{ return m_tPollInfo.GetMediaMode(); }
	void   SetPollState(u8   byPollState){ m_tPollInfo.SetPollState( byPollState );} 
    u8     GetPollState( void ) const { return m_tPollInfo.GetPollState(); }
    void   SetMtPollParam(TMtPollParam tMtPollParam){ m_tPollInfo.SetMtPollParam( tMtPollParam ); } 
    TMtPollParam  GetMtPollParam() const{ return m_tPollInfo.GetMtPollParam(); }

    //电视墙轮询参数
    void   SetTvWallPollInfo(const TTvWallPollInfoV4R5 &tPollInfo){ m_tTvWallPollInfo = tPollInfo;} 
    TTvWallPollInfoV4R5  *GetTvWallPollInfo( void ) { return &m_tTvWallPollInfo; }    
    void   SetTvWallPollState(u8   byPollState){ m_tTvWallPollInfo.SetPollState( byPollState );} 
    u8     GetTvWallPollState( void ) const { return m_tTvWallPollInfo.GetPollState(); }
    void   SetTvWallMtPollParam(TMtPollParam tMtPollParam){ m_tTvWallPollInfo.SetMtPollParam( tMtPollParam ); } 
    TMtPollParam  GetTvWallMtPollParam(){ return m_tTvWallPollInfo.GetMtPollParam(); }

	//会议录像状态	
	BOOL IsNoRecording( void ) const{ return m_tConfMode.GetRecordMode() == CONF_RECMODE_NONE ? TRUE : FALSE; }
	void SetNoRecording( void ){ m_tConfMode.SetRecordMode( CONF_RECMODE_NONE ); }
	BOOL IsRecording( void ) const{ return m_tConfMode.GetRecordMode() == CONF_RECMODE_REC ? TRUE : FALSE; }
	void SetRecording( void ){ m_tConfMode.SetRecordMode( CONF_RECMODE_REC ); }
	BOOL IsRecPause( void ) const{ return m_tConfMode.GetRecordMode() == CONF_RECMODE_PAUSE ? TRUE : FALSE; }
	void SetRecPause( void ){ m_tConfMode.SetRecordMode( CONF_RECMODE_PAUSE ); }

	//会议放像状态
	BOOL IsNoPlaying( void ) const{ return m_tConfMode.GetPlayMode() == CONF_PLAYMODE_NONE ? TRUE : FALSE; }
	void SetNoPlaying( void ){ m_tConfMode.SetPlayMode( CONF_PLAYMODE_NONE ); }
	BOOL IsPlaying( void ) const { return m_tConfMode.GetPlayMode() == CONF_PLAYMODE_PLAY ? TRUE : FALSE; }
	void SetPlaying( void ){ m_tConfMode.SetPlayMode( CONF_PLAYMODE_PLAY ); }
	BOOL IsPlayPause( void )const { return m_tConfMode.GetPlayMode() == CONF_PLAYMODE_PAUSE ? TRUE : FALSE; }
	void SetPlayPause( void ){ m_tConfMode.SetPlayMode( CONF_PLAYMODE_PAUSE ); }
	BOOL IsPlayFF( void )const { return m_tConfMode.GetPlayMode() == CONF_PLAYMODE_FF ? TRUE : FALSE; }
	void SetPlayFF( void ) { m_tConfMode.SetPlayMode( CONF_PLAYMODE_FF ); }
	BOOL IsPlayFB( void )const { return m_tConfMode.GetPlayMode() == CONF_PLAYMODE_FB ? TRUE : FALSE; }
	void SetPlayFB( void ){ m_tConfMode.SetPlayMode( CONF_PLAYMODE_FB ); }
	// zgc, 2007-02-27, 会议放像关键帧延迟问题
	BOOL IsPlayReady( void ) const{ return m_tConfMode.GetPlayMode() == CONF_PLAYMODE_PLAYREADY ? TRUE : FALSE; }
	void SetPlayReady( void ){ m_tConfMode.SetPlayMode( CONF_PLAYMODE_PLAYREADY ); }
	// zgc, 2007-02-27, end

	//包重传状态
	void SetPrsing( BOOL bPrs ){ m_byPrs = bPrs; }
	BOOL IsPrsing( void ){ return m_byPrs; }

	//注册GK情况
	void SetRegToGK( BOOL bRegToGK ){ m_tConfMode.SetRegToGK( bRegToGK ); }
	BOOL IsRegToGK( void ) const{ return m_tConfMode.IsRegToGK(); }

    // xsl [11/16/2006] GK计费情况
    void SetGkCharge( BOOL bCharge ) { m_tConfMode.SetGkCharge( bCharge ); }
    BOOL IsGkCharge( void ) const { return m_tConfMode.IsGkCharge(); }

	void Print( void ) const
	{
		OspPrintf( TRUE, FALSE, "\nConfStatus:\n" );
        if(m_tChairman.IsNull())  
            OspPrintf( TRUE, FALSE, "Chairman: Null\n" );
        else 
            OspPrintf( TRUE, FALSE, "Chairman: Mcu%dMt%d\n", m_tChairman.GetMcuId(), m_tChairman.GetMtId() );
        if(m_tSpeaker.IsNull())
            OspPrintf( TRUE, FALSE, "Speaker: Null\n" );
        else
		    if (m_tSpeaker.GetType() == TYPE_MT)
                OspPrintf( TRUE, FALSE, "Speaker: Mcu%dMt%d\n", m_tSpeaker.GetMcuId(), m_tSpeaker.GetMtId() );
            else
                OspPrintf( TRUE, FALSE, "Speaker: Mcu%dEqp%d\n", m_tSpeaker.GetMcuId(), m_tSpeaker.GetEqpId() );
        OspPrintf( TRUE, FALSE, "VmpMode: %d\n", m_tVMPParam.GetVMPMode() );
        OspPrintf( TRUE, FALSE, "TWVmpMode: %d\n", m_tVmpTwParam.GetVMPMode() );
        OspPrintf( TRUE, FALSE, "MixerMode: %d\n", GetMixerMode() );
	}
}
/*
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
*/
PACKED
;

// xliang [2/24/2009] V4R5 ConfInfo 
struct TConfInfoV4R5
{
protected:
    CConfId       m_cConfId;                        //会议号，全网唯一
    TKdvTime      m_tStartTime;                     //开始时间，控制台填0为立即开始
    u16           m_wDuration;                      //持续时间(分钟)，0表示不自动停止
    u16           m_wBitRate;                       //会议码率(单位:Kbps,1K=1024)
    u16           m_wSecBitRate;                    //双速会议的第2码率(单位:Kbps,为0表示是单速会议)
	u16			  m_wDcsBitRate;					//数据会议码率(单位:Kbps,为0表示不支持数据会议)
    u8            m_byDStreamScale;                 //双流百分比
    u8            m_byUsrGrpId;                     //用户组
    u8            m_byTalkHoldTime;                 //最小发言持续时间(单位:秒)
    u16            m_wMixDelayTime;                 //混音延时时间
    TCapSupport   m_tCapSupport;                    //会议支持的媒体
    s8            m_achConfPwd[MAXLEN_PWD+1];       //会议密码
    s8            m_achConfName[MAXLEN_CONFNAME+1]; //会议名
    s8            m_achConfE164[MAXLEN_E164+1];     //会议的E164号码
    TMtAlias      m_tChairAlias;                    //会议中主席的别名
    TMtAlias      m_tSpeakerAlias;                  //会议中发言人的别名
    TConfAttrb    m_tConfAttrb;                     //会议属性

    TMediaEncrypt m_tMediaKey;                      //第一版本所有密钥一样，不支持更新    
public:
    TConfStatusV4R5   m_tStatus;                        //会议状态

    // 4.0R3 版本扩展字段
protected:
    TConfAttrbEx  m_tConfAttrbEx;                   //会议扩展属性
	TCapSupportEx m_tCapSupportEx;					//会议能力集扩展属性
    
public:	
    TConfInfoV4R5( void )
    { 
        memset( this, 0, sizeof( TConfInfoV4R5 ) ); 
        m_tCapSupport.Clear(); 
        m_tConfAttrbEx.Reset(); 
    }
    CConfId GetConfId( void ) const { return m_cConfId; }
    void    SetConfId( CConfId cConfId ){ m_cConfId = cConfId; }
    void    SetUsrGrpId(u8 byGrpId) { m_byUsrGrpId = byGrpId; }
    u8      GetUsrGrpId( void ) const { return m_byUsrGrpId; }
    TKdvTime GetKdvStartTime( void ) const { return( m_tStartTime ); }
    void   SetKdvStartTime( TKdvTime tStartTime ){ m_tStartTime = tStartTime; }
    void   SetDuration(u16  wDuration){ m_wDuration = htons(wDuration);} 
    u16    GetDuration( void ) const { return ntohs(m_wDuration); }
    void   SetBitRate(u16  wBitRate){ m_wBitRate = htons(wBitRate);} 
    u16    GetBitRate( void ) const { return ntohs(m_wBitRate); }
    void   SetSecBitRate(u16 wSecBitRate){ m_wSecBitRate = htons(wSecBitRate);} 
    u16    GetSecBitRate( void ) const { return ntohs(m_wSecBitRate); }
	void   SetDcsBitRate(u16 wDcsBitRate){ m_wDcsBitRate = htons(wDcsBitRate); }
	u16	   GetDcsBitRate( void ) const { return ntohs(m_wDcsBitRate); }

    void   SetMainVideoFormat(u8 byVideoFormat)
    { 
        TSimCapSet tSim = m_tCapSupport.GetMainSimCapSet();
        tSim.SetVideoResolution(byVideoFormat); 
        m_tCapSupport.SetMainSimCapSet(tSim);
    } 
    u8     GetMainVideoFormat( void ) const { return m_tCapSupport.GetMainSimCapSet().GetVideoResolution(); }
    void   SetSecVideoFormat(u8 byVideoFormat)
    { 
        TSimCapSet tSim = m_tCapSupport.GetSecondSimCapSet();
        tSim.SetVideoResolution(byVideoFormat);
        m_tCapSupport.SetSecondSimCapSet(tSim); 
    } 
    u8     GetSecVideoFormat(void) const { return m_tCapSupport.GetSecondSimCapSet().GetVideoResolution(); }
    void   SetDoubleVideoFormat(u8 byVideoFormat) { m_tCapSupport.SetDStreamResolution(byVideoFormat); } 
    u8     GetDoubleVideoFormat(void) const { return m_tCapSupport.GetDStreamResolution(); }
    u8     GetVideoFormat(u8 byVideoType, u8 byChanNo = LOGCHL_VIDEO);
    u8     GetDStreamScale(void) const 
    { 
        u8 byScale = m_byDStreamScale;
        if (byScale > MAXNUM_DSTREAM_SCALE || byScale < MINNUM_DSTREAM_SCALE)
        {
            byScale = DEF_DSTREAM_SCALE;
        }       
        
        return byScale; 
    }
    void   SetDStreamScale(u8 byDStreamScale) { m_byDStreamScale = byDStreamScale; }

    void   SetMixDelayTime(u16 wDelayTime) { m_wMixDelayTime = htons(wDelayTime); }
    u16    GetMixDelayTime(void) { return ntohs(m_wMixDelayTime); }
    void   SetTalkHoldTime(u8   byTalkHoldTime){ m_byTalkHoldTime = byTalkHoldTime;} 
    u8     GetTalkHoldTime( void ) const { return m_byTalkHoldTime; }
    void   SetCapSupport(TCapSupport tCapSupport){ m_tCapSupport = tCapSupport;} 
    TCapSupport GetCapSupport( void ) const { return m_tCapSupport; }
    void   SetConfPwd( LPCSTR lpszConfPwd );
    LPCSTR GetConfPwd( void ) const { return m_achConfPwd; }
    void   SetConfName( LPCSTR lpszConfName );
    LPCSTR GetConfName( void ) const { return m_achConfName; }
    void   SetConfE164( LPCSTR lpszConfE164 );
    LPCSTR GetConfE164( void ) const { return m_achConfE164; }
    void   SetChairAlias(TMtAlias tChairAlias){ m_tChairAlias = tChairAlias;} 
    TMtAlias  GetChairAlias( void ) const { return m_tChairAlias; }
    void   SetSpeakerAlias(TMtAlias tSpeakerAlias){ m_tSpeakerAlias = tSpeakerAlias;} 
    TMtAlias  GetSpeakerAlias( void ) const { return m_tSpeakerAlias; }
    void   SetConfAttrb(TConfAttrb tConfAttrb){ m_tConfAttrb = tConfAttrb;} 
    TConfAttrb  GetConfAttrb( void ) const { return m_tConfAttrb; }
    void   SetStatus(TConfStatusV4R5 tStatus){ m_tStatus = tStatus;} 
    TConfStatusV4R5  GetStatus( void ) const { return m_tStatus; }	

    void    SetHasVmpModule(BOOL32 bHas) { m_tConfAttrb.SetHasVmpModule(bHas); }
    void    SetHasTvwallModule(BOOL32 bHas) { m_tConfAttrb.SetHasTvWallModule(bHas); }

	TMediaEncrypt& GetMediaKey(void) { return m_tMediaKey; };
	void SetMediaKey(TMediaEncrypt& tMediaEncrypt){ memcpy(&m_tMediaKey, &tMediaEncrypt, sizeof(tMediaEncrypt));}

	void   GetVideoScale(u8 byVideoType, u16 &wVideoWidth, u16 &wVideoHeight, u8* pbyVideoResolution = NULL);
	BOOL   HasChairman( void ) const{ if(m_tStatus.m_tChairman.GetMtId()==0)return FALSE; return TRUE; }	
	TMt    GetChairman( void ) const{ return m_tStatus.m_tChairman; }
	void   SetNoChairman( void ){ memset( &m_tStatus.m_tChairman, 0, sizeof( TMt ) ); }
	void   SetChairman( TMt tChairman ){ m_tStatus.m_tChairman = tChairman; }
	BOOL   HasSpeaker( void ) const{ if(m_tStatus.m_tSpeaker.GetMtId()==0)return FALSE; return TRUE; }
    TMt    GetSpeaker( void ) const{ return m_tStatus.m_tSpeaker; }
	void   SetNoSpeaker( void ){ memset( &m_tStatus.m_tSpeaker, 0, sizeof( TMt ) ); }
	void   SetSpeaker( TMt tSpeaker ){ m_tStatus.m_tSpeaker = tSpeaker; }
	time_t GetStartTime( void ) const;	
	void   SetStartTime( time_t dwStartTime );

    u8     GetMainVideoMediaType( void ) const { return m_tCapSupport.GetMainVideoType();   }
    u8     GetMainAudioMediaType( void ) const { return m_tCapSupport.GetMainAudioType();   }
    u8     GetSecVideoMediaType( void ) const { return m_tCapSupport.GetSecVideoType(); }
    u8     GetSecAudioMediaType( void ) const { return m_tCapSupport.GetSecAudioType(); }
    u8     GetDStreamMediaType( void ) const { return m_tCapSupport.GetDStreamMediaType(); }

    //帧率
    void   SetMainVidFrameRate(u8 byMPI) { m_tCapSupport.SetMainVidFrmRate(byMPI);    }
    u8     GetMainVidFrameRate(void) const { return m_tCapSupport.GetMainVidFrmRate();    }
    void   SetSecVidFrameRate(u8 byMPI) { m_tCapSupport.SetSecVidFrmRate(byMPI);    }
    u8     GetSecVidFrameRate(void) const { return m_tCapSupport.GetSecVidFrmRate();    }
    void   SetDStreamFrameRate(u8 byMPI) { m_tCapSupport.SetDStreamFrmRate(byMPI);    }
    u8     GetDStreamFrameRate(void) const { return m_tCapSupport.GetDStreamFrameRate();    }

    //H264 自定义帧率
    void   SetMainVidUsrDefFPS(u8 byFPS) { m_tCapSupport.SetMainVidUsrDefFPS(byFPS);    }
    u8     GetMainVidUsrDefFPS(void) const { return m_tCapSupport.GetMainVidUsrDefFPS();    }
    BOOL32 IsMainVidUsrDefFPS(void) const { return m_tCapSupport.IsMainVidUsrDefFPS();    }
    void   SetSecVidUsrDefFPS(u8 byFPS) { m_tCapSupport.SetSecVidUsrDefFPS(byFPS);    }
    u8     GetSecVidUsrDefFPS(void) const { return m_tCapSupport.GetSecVidUsrDefFPS();    }
    BOOL32 IsSecVidUsrDefFPS(void) const { return m_tCapSupport.IsSecVidUsrDefFPS();    }
    void   SetDStreamUsrDefFPS(u8 byFPS) { m_tCapSupport.SetDStreamUsrDefFPS(byFPS);    }
    u8     GetDStreamUsrDefFPS(void) const { return m_tCapSupport.GetDStreamUsrDefFPS();    }
    BOOL32 IsDStreamUsrDefFPS(void) const { return m_tCapSupport.IsDStreamUsrDefFPS();    }

    void         SetConfAttrbEx(TConfAttrbEx tAttrbEx) { m_tConfAttrbEx = tAttrbEx; }
    TConfAttrbEx GetConfAttrbEx() const {return m_tConfAttrbEx; }
    void   SetSatDCastChnlNum(u8 byNum) { m_tConfAttrbEx.SetSatDCastChnlNum(byNum); }
    u8     GetSatDCastChnlNum() const { return m_tConfAttrbEx.GetSatDCastChnlNum(); }
    void   SetAutoRecAttrb(const TConfAutoRecAttrb& tAttrb ) { m_tConfAttrbEx.SetAutoRecAttrb(tAttrb); }
    TConfAutoRecAttrb GetAutoRecAttrb() const { return m_tConfAttrbEx.GetAutoRecAttrb(); }
    void   SetVmpAutoAdapt(BOOL32 bAutoAdapt) { m_tConfAttrbEx.SetVmpAutoAdapt(bAutoAdapt); }
    BOOL   IsVmpAutoAdapt() const { return m_tConfAttrbEx.IsVmpAutoAdapt(); }
    BOOL   IsSupportGkCharge(void) const { return ( strlen(m_tConfAttrbEx.GetGKUsrName()) > 0 ? TRUE : FALSE ); }
    void   SetDiscussAutoStop() { m_tConfAttrbEx.SetDiscussAutoStop(); }
    void   SetDiscussAutoSwitch2Mix() { m_tConfAttrbEx.SetDiscussAutoSwitch2Mix(); }
    BOOL   IsDiscussAutoStop() const { return m_tConfAttrbEx.IsDiscussAutoStop(); }

	//会议能力集扩展属性, zgc, 2007-09-26
	BOOL32	IsSupportFEC(void) const { return !m_tCapSupportEx.IsNoSupportFEC(); }
	TCapSupportEx GetCapSupportEx(void) const { return m_tCapSupportEx; }
	void	SetCapSupportEx(TCapSupportEx tCapSupportEx) { m_tCapSupportEx = tCapSupportEx; }

	void Print( void ) const
	{
		OspPrintf( TRUE, FALSE, "\nConfInfo:\n" );
		OspPrintf( TRUE, FALSE, "m_cConfId: " );
		m_cConfId.Print();
        OspPrintf( TRUE, FALSE, "m_byUsrGrpId: %d\n", GetUsrGrpId());
		OspPrintf( TRUE, FALSE, "\nm_tStartTime: " );
        m_tStartTime.Print();
		OspPrintf( TRUE, FALSE, "\nm_wDuration: %d\n", ntohs(m_wDuration) );
		OspPrintf( TRUE, FALSE, "m_wBitRate: %d\n", ntohs(m_wBitRate) );
        OspPrintf( TRUE, FALSE, "m_wSecBitRate: %d\n", ntohs(m_wSecBitRate) );
		OspPrintf( TRUE, FALSE, "m_wDcsBitRate: %d\n", ntohs(m_wDcsBitRate) );
		OspPrintf( TRUE, FALSE, "m_byVideoFormat: %d\n", GetMainVideoFormat() );
		OspPrintf( TRUE, FALSE, "m_byTalkHoldTime: %d\n", m_byTalkHoldTime );
        OspPrintf( TRUE, FALSE, "m_byDStreamScale: %d\n", m_byDStreamScale );
		OspPrintf( TRUE, FALSE, "m_tCapSupport:\n" );
	    m_tCapSupport.Print();
        OspPrintf( TRUE, FALSE, "m_tCapSupportEx: \n" );
        m_tCapSupportEx.Print();
		OspPrintf( TRUE, FALSE, "m_achConfName: %s\n", m_achConfName );
		OspPrintf( TRUE, FALSE, "m_achConfE164: %s\n", m_achConfE164 );
        OspPrintf( TRUE, FALSE, "m_achConfPwd: %s\n", m_achConfPwd );
        OspPrintf( TRUE, FALSE, "m_tChairAlias:" );
        m_tChairAlias.Print();
        OspPrintf( TRUE, FALSE, "m_tSpeakerAlias:" );
        m_tSpeakerAlias.Print();
		OspPrintf( TRUE, FALSE, "\n" );
        OspPrintf( TRUE, FALSE, "m_tConfAttrbEx: \n" );
        m_tConfAttrbEx.Print();
	}
}
/*
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
*/
PACKED
;

#ifdef WIN32
#pragma pack( pop )
#endif

#endif  // _MASTRUCTR4V4B2_H_
