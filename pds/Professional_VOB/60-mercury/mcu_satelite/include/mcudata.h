/*****************************************************************************
   模块名      : mcu
   文件名      : mcudata.h
   相关文件    : mcudata.cpp
   文件实现功能: MCU数据模块应用类头文件
   作者        : 李屹
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/05/29  0.9         李屹          创建
   2006/01/11  4.0		   张宝卿		 增加集成T120结构
******************************************************************************/

#ifndef __MCUDATA_H_
#define __MCUDATA_H_

#include "osp.h"
#include "mcuconst.h"
#include "mcustruct.h"
#include "mcuutility.h"
#include "msmanagerssn.h"
#include "mpmanager.h"
#include "usermanage.h"
#include "vcsstruct.h"

//定义数据会议增加终端等待超时值
#define TIME_WAITFORJOIN			(u8)3		//DCS等待下级呼入的超时值
#define TIME_WAITFORINVITE			(u8)3		//DCS等待上级邀请的超时值

extern int	g_nInitUdpHandle;
extern u32  g_dwVCUInterval;

/************************************************************************/
/*                                                                      */
/*                      一、会议一般信息管理                            */
/*                                                                      */
/************************************************************************/

//1、外设数据
struct TPeriEqpData
{
	BOOL32  m_bIsValid;                          //该设备是否在配置中设置
	u8	    m_byFwdChannelNum;					 //MCU至外设信道数
	TLogicalChannel	m_tFwdVideoChannel;		     //MCU至外设起始视频信道
	TLogicalChannel	m_tFwdAudioChannel;		     //MCU至外设起始音频信道
	u8	m_byRvsChannelNum;					     //外设至MCU信道数
	TLogicalChannel	m_tRvsVideoChannel;		     //外设至MCU起始视频信道
	TLogicalChannel	m_tRvsAudioChannel;		     //外设至MCU起始音频信道
	TPeriEqpStatus	m_tPeriEqpStatus;		     //该外设状态，由外设自行上报
	TMt m_atVidSrc[MAXNUM_PERIEQP_CHNNL];        //主要为TW,VMP服务
	TMt m_atAudSrc[MAXNUM_PERIEQP_CHNNL];        //主要为TW,VMP服务
	char m_achAliase[MAXLEN_EQP_ALIAS];          //外设别名
	TTransportAddr  m_tVideoRtcpDstAddr[MAXNUM_PERIEQP_CHNNL];   //外设信道视频RTCP目的地址
	TTransportAddr  m_tAudioRtcpDstAddr[MAXNUM_PERIEQP_CHNNL];   //外设信道音频RTCP目的地址    
};

//2、会控数据
struct TMcData
{
	BOOL32	m_bConnected;					//会控是否登记为FALSE表示未建链登记
	u8		m_byFwdChannelNum;				//MCU至会控信道数
	TLogicalChannel	m_tFwdVideoChannel;		//MCU至会控起始视频信道
	TLogicalChannel	m_tFwdAudioChannel;		//MCU至会控起始音频信道
	TMt		m_atVidSrc[MAXNUM_MC_CHANNL];	//该会控选看视频源
	TMt		m_atAudSrc[MAXNUM_MC_CHANNL];	//该会控选看音频源
	TMcsRegInfo m_tMcsRegInfo;              //会控注册信息
};

//3、MP数据
struct TMpData
{
    BOOL32 m_bConnected;      //是否连接
    TMp  m_tMp;               //MP信息
	u8   m_abyMtId[MAX_CONFIDX][MAXNUM_CONF_MT];    //会议接入终端id
    u16  m_wMtNum;            //mp接入终端数
    u16  m_wNetBandAllowed;   //允许网络带宽
    u16  m_wNetBandReal;      //已经占用的网络带宽
    u32  m_dwPkNumAllowed;    //本MP允许的Pk数(一个终端的1kbps的码率转发为一个Pk，其他码率向上取整后类推) 02/09/2007-zbq
    u32  m_dwPkNumReal;       //主机序。本MP已经接入的Pk数
    u16  m_wNetBandReserved;  //预留的网络带宽（为某外设）
public:
    
    TMpData() : m_bConnected(FALSE),
                m_wMtNum(0),
                m_wNetBandAllowed(0),
                m_wNetBandReal(0),
                m_dwPkNumAllowed(0),
                m_dwPkNumReal(0),
                m_wNetBandReserved(0)
    {
        memset( &m_tMp, 0, sizeof(TMp) );
        memset( &m_abyMtId, 0, sizeof(m_abyMtId) );
    }

    BOOL32 IsThePkSupport(u32 dwAddPkNum) 
    {
        return dwAddPkNum + m_dwPkNumReal <= m_dwPkNumAllowed ? TRUE : FALSE;
    }
    // 该MP当前的繁忙指数：(Pk繁忙指数 + 码流转发繁忙指数)/2
    u8  GetCurBusyPercent(void)
    {
        //码流转发繁忙指数 > 80%, 不再负责Pk的负载承担
        if ((m_wNetBandReal+m_wNetBandReserved) * 100/m_wNetBandAllowed >= 80)
        {
            return 100;
        }
        //预留带宽和真实带宽 共同作为码流转发的负载带宽
        else
        {
            return (u8)((m_dwPkNumReal * 100)/m_dwPkNumAllowed + ((m_wNetBandReal+m_wNetBandReserved) * 100)/m_wNetBandAllowed)/2;
        }
    }
    void SetNull(void)
    {
        m_bConnected = FALSE;
        m_wMtNum = 0;
        m_wNetBandReal = 0;
        m_wNetBandAllowed = 0;
        m_dwPkNumReal = 0;
        m_dwPkNumAllowed = 0;
        m_wNetBandReserved = 0;
        memset( &m_tMp, 0, sizeof(TMp) );
        memset( &m_abyMtId, 0, sizeof(m_abyMtId) );
    }
};

//4、占内嵌接入板高清资源的终端信息// xliang [4/3/2009]  
struct TMtAdpHDChnnlInfo
{
public:
	u8 m_byHDMtId;		//HD 终端ID
	u8 m_byConfIdx;		//会议IDx
public:
	void SetNull() { m_byConfIdx = 0; m_byHDMtId = 0; }
	void SetConfIdx( u8 byConfIdx ) { m_byConfIdx = byConfIdx; }
	void SetHDMtId( u8 byHdMtId ) { m_byHDMtId = byHdMtId; } 
	u8	GetHDMtId() const { return m_byHDMtId; }
	u8	GetConfIdx() const { return m_byConfIdx; }
	BOOL   operator ==( const TMtAdpHDChnnlInfo & tObj ) const //判断是否相等
	{
		if( tObj.GetConfIdx() == GetConfIdx() 
			&& tObj.GetHDMtId() == GetHDMtId() 
			)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
};

//5、MtAdp数据
struct TMtAdpData
{
	BOOL32 m_bConnected;                      //是否连接
	u8   m_byMtAdpId;                       //本Mtadp号
	u32  m_dwIpAddr;                        //Ip地址，网络序
	u8   m_byProtocolType;                  //适配的协议类型//H323、H320、SIP
	u8   m_byMaxMtNum;                      //适配的支持最大终端数
    u8   m_byMaxSMcuNum;                    //适配的支持最大下级Mcu数
	u8   m_abyMtId[MAX_CONFIDX][MAXNUM_CONF_MT];           //会议接入终端id
    u16  m_wMtNum;                          //mtadp接入终端数
	u8   m_byRegGKConfNum;                  //注册GK的会议数
    u16  m_wQ931Port;                       //主机序(用于主适配板gk注册)    
    u16  m_wRasPort;                        //主机序
    u8   m_byIsSupportHD;                   //是否支持HD接入
//	u8   m_abyHDMt[MAXHDLIMIT_MPC_MTADP];    //HD 终端
	TMtAdpHDChnnlInfo m_atHdChnnlInfo[MAXHDLIMIT_MPC_MTADP];	//占内嵌接入板高清资源的终端信息
};

//6、Prs通道描述
struct TPrsChannel
{	
public:
	TPrsChannel()
	{
		memset(this, 0, sizeof(TPrsChannel));
		for(u8 byLp =0; byLp < MAXNUM_PRS_CHNNL; byLp++)
		{
			m_abyPrsChannels[byLp] = EQP_CHANNO_INVALID;
		}
	}

	u8  GetPrsId(void) const
	{
		return m_byEqpPrsId;
	}
	u8  GetChannelsNum(void) const
	{
		return m_byChannelNum;
	}
	void SetPrsId(u8 byPrsId)
	{
		m_byEqpPrsId = byPrsId;
	}

	void SetChannelNum(u8 byChlsNum )
	{
		m_byChannelNum = byChlsNum;
	}

	u8  m_abyPrsChannels[MAXNUM_PRS_CHNNL]; // 通道号
private:	
	u8  m_byEqpPrsId;          // Prs Id
	u8  m_byChannelNum;        // 通道数	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TUsedPrsChnlInfo 
{
public:
	TUsedPrsChnlInfo()
	{
		SetNull();
	}

	void SetNull() 
	{
		memset(this, 0, sizeof(TUsedPrsChnlInfo));
	}

	BOOL IsNull()
	{
		return (m_byPrsId < PRSID_MIN || m_byPrsId > PRSID_MAX); 
	}

	void Print()
	{
		OspPrintf(TRUE, FALSE, "m_byPrsId = %d, m_byPrsChnlId = %d, m_byPrsMode = %d\n", m_byPrsId, m_byPrsChnlId, m_byPrsMode);
		OspPrintf(TRUE, FALSE, "m_tPrsSrc = (srcid:%d, srctype:%d), m_byPrsSrcOutChnl = %d\n\n", 
			      m_tPrsSrc.GetMtId(), m_tPrsSrc.GetMtType(), m_byPrsSrcOutChnl);
	}

	u8 m_byPrsId;
	u8 m_byPrsChnlId;
	u8 m_byPrsMode;

	TMt m_tPrsSrc;
	u8  m_byPrsSrcOutChnl;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TPrsInfoManage
{
public:
	TPrsInfoManage()
	{
		SetNull();
	}
	
	void SetNull()
	{
		for (u8 byIdx = 0; byIdx <MAXNUM_PRS_CHNNL; byIdx++)
		{
			m_tPrsChnlInfo[byIdx].SetNull();
// 			m_byIsStart[byIdx] = FALSE;
		}
	}

	BOOL AddPrsChnl(u8 byPrsId, u8 byPrsChnlId, u8 byPrsMode, TMt tSrc, u8 bySrcOutChnl = 0)
	{
		for (u8 byChnlId = 0; byChnlId < MAXNUM_PRS_CHNNL; byChnlId++)
		{
			if (m_tPrsChnlInfo[byChnlId].IsNull())
			{
				m_tPrsChnlInfo[byChnlId].m_byPrsId = byPrsId;
				m_tPrsChnlInfo[byChnlId].m_byPrsChnlId = byPrsChnlId;
				m_tPrsChnlInfo[byChnlId].m_byPrsMode = byPrsMode;
				m_tPrsChnlInfo[byChnlId].m_tPrsSrc = tSrc;
				m_tPrsChnlInfo[byChnlId].m_byPrsSrcOutChnl = bySrcOutChnl;
				return TRUE;
			}
		}
		return FALSE;

	}

	void RemovePrsChnl(u8 byPrsId, u8 byPrsChnlId)
	{
		for (u8 byChnlId = 0; byChnlId < MAXNUM_PRS_CHNNL; byChnlId++)
		{
			if (m_tPrsChnlInfo[byChnlId].m_byPrsId == byPrsId &&
				m_tPrsChnlInfo[byChnlId].m_byPrsChnlId == byPrsChnlId)
			{
				m_tPrsChnlInfo[byChnlId].SetNull();
			}
		}
	}

	BOOL FindPrsChnlSrc(u8 byPrsId, u8 byPrsChnlId, u8& byPrsMode, TMt& tSrc, u8& bySrcOutChnl)
	{
		if (byPrsId < PRSID_MIN || byPrsId > PRSID_MAX)
		{
			return FALSE;
		}

		for (u8 byChnlId = 0; byChnlId < MAXNUM_PRS_CHNNL; byChnlId++)
		{
			if (m_tPrsChnlInfo[byChnlId].m_byPrsId == byPrsId &&
				m_tPrsChnlInfo[byChnlId].m_byPrsChnlId == byPrsChnlId)
			{
				byPrsMode    = m_tPrsChnlInfo[byChnlId].m_byPrsMode;
				tSrc         = m_tPrsChnlInfo[byChnlId].m_tPrsSrc;
				bySrcOutChnl = m_tPrsChnlInfo[byChnlId].m_byPrsSrcOutChnl;
				return TRUE;
			}
		}
		return	FALSE;
	}

	BOOL FindPrsForSrc(const TMt& tSrc, u8 bySrcOutChnl, u8& byPrsId, u8& byPrsChnlId)
	{
		for (u8 byChnlId = 0; byChnlId < MAXNUM_PRS_CHNNL; byChnlId++)
		{
			if (m_tPrsChnlInfo[byChnlId].m_tPrsSrc == tSrc &&
				m_tPrsChnlInfo[byChnlId].m_byPrsSrcOutChnl == bySrcOutChnl)
			{
				byPrsId     = m_tPrsChnlInfo[byChnlId].m_byPrsId;
				byPrsChnlId = m_tPrsChnlInfo[byChnlId].m_byPrsChnlId;
				return TRUE;
			}
		}
		return	FALSE;
	}

	
private:
// 	u8               m_byIsStart[MAXNUM_PRS_CHNNL];
	TUsedPrsChnlInfo m_tPrsChnlInfo[MAXNUM_PRS_CHNNL];
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
/************************************************************************/
/*                                                                      */
/*                        二、会议模板信息管理                          */
/*                                                                      */
/************************************************************************/

//会议模板信息
struct TTemplateInfo : public TConfStore
{
    u8   m_byConfIdx;           //会议id

public:
    TTemplateInfo(void) { Clear(); }
    void Clear(void) { memset(this, 0, sizeof(TTemplateInfo)); }

    BOOL32 IsEmpty(void) { return (0 == m_byConfIdx); }
}
;

struct TConfMapData
{
protected:
    u8  m_byTemIndex;           //模板数组索引
    u8  m_byInsId;              //即时会议实例id   
    
public:
    TConfMapData(void) { Clear(); }
    void Clear(void) 
    {
        m_byTemIndex = MAXNUM_MCU_TEMPLATE;
        m_byInsId = 0;
    }

    void   SetInsId(u8 byInsId) { m_byInsId = byInsId; }
    u8     GetInsId(void)  { return m_byInsId; }
    void   SetTemIndex(u8 byIndex) { m_byTemIndex = byIndex; }
    u8     GetTemIndex(void) { return m_byTemIndex; }
    
    BOOL32 IsValidConf(void) { return (m_byInsId>=MIN_CONFIDX && m_byInsId <=MAXNUM_MCU_CONF); }//是否为占用会议实例的有效会议 即时或预约
    BOOL32 IsTemUsed(void) { return (m_byTemIndex < MAXNUM_MCU_TEMPLATE); }
};


/************************************************************************/
/*                                                                      */
/*                       三、数据会议信息管理                           */
/*                                                                      */
/************************************************************************/

//1、数据会议服务器信息
struct TDcsInfo
{	
public:
	TPeriDcsStatus m_tDcsStatus;	// DCS当前的状态
	BOOL32  m_bIsValid;				// DCS是否在配置中存在
	u32		m_dwDcsIp;				// DCS IP地址
    u16		m_wDcsPort;				// DCS端口
    u16		m_wReservedPortStart;   // 预留端口范围起始端口，默认9000
    u16		m_wReservedPortRang;    // 预留端口范围大小，默认100
    u8		m_byMaxConfCount;		// 最大会议数，默认32
    u8		m_byMaxMtCount;			// 会议中最大终端数，默认64
    u8		m_byMaxDirectMtCount;   // 最大直接下级终端数，默认32
    u8		m_byMaxHeight;			// 最高级联数，默认16
	u8		m_byDcsId;				// DCS Id( MCU用 )
public:
	TDcsInfo(){  Clear(); }
	
	void Clear()
	{
		m_tDcsStatus.SetNull();
		m_bIsValid = FALSE;
		m_dwDcsIp  = 0;
		m_wDcsPort = 0;
		m_wReservedPortStart = 0;
		m_wReservedPortRang  = 0; 
		m_byMaxConfCount     = 0;
		m_byMaxMtCount       = 0;	
		m_byMaxDirectMtCount = 0;
		m_byMaxHeight        = 0;
		m_byDcsId            = 0;
	}
	BOOL32 IsNull()
	{
		if ( 0 == m_dwDcsIp || 0 == m_byDcsId )
		{
			return TRUE;
		}
		return FALSE;
	}
	void SetNull()
	{
        m_byDcsId = 0;
		m_dwDcsIp = 0;
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//2、数据会议信息
struct TDcsConfInfo
{
protected:
	s8		m_achConfName[MAXLEN_CONFNAME + 1];	
	s8		m_achConfPwd[MAXLEN_PWD + 1];
	u16		m_wBandwidth;
	BOOL32	m_bSupportJoinedMt;
public:
	TDcsConfInfo() { Clear(); }

	void Clear()
	{
		memset( &m_achConfName, 0, sizeof(m_achConfName) );
		memset( &m_achConfPwd, 0 , sizeof(m_achConfPwd) );
		m_wBandwidth = 0;
		m_bSupportJoinedMt = FALSE;
	}
	void SetConfName( LPCSTR lpszConfName )
	{
		if( NULL != lpszConfName )
		{
			strncpy( m_achConfName, lpszConfName, sizeof( m_achConfName ) );
			m_achConfName[sizeof(m_achConfName) - 1] = '\0';
		}
		else
		{
			memset( m_achConfName, 0, sizeof( m_achConfName ) );
		}
	}	
	LPCSTR GetConfName() const { return m_achConfName; }
	
	void   SetConfPwd( LPCSTR lpszConfPwd )
	{ 
		if( lpszConfPwd != NULL )
		{
			strncpy( m_achConfPwd, lpszConfPwd, sizeof( m_achConfPwd ) );
			m_achConfPwd[sizeof(m_achConfPwd) - 1] = '\0';
		}
		else
		{
			memset( m_achConfPwd, 0, sizeof( m_achConfPwd ) );
		}
	}
	LPCSTR GetConfPwd() const { return m_achConfPwd; }

	BOOL32 IsSupportJoinMt()
	{
		if ( FALSE == m_bSupportJoinedMt )
		{
			return FALSE;
		}
		return TRUE;
	}
	void SetSupportJoinMt( BOOL32 bSupport )
	{
		m_bSupportJoinedMt = bSupport;
		return;
	}

	void SetBandwidth( u16 wBandwith ) { m_wBandwidth = wBandwith;	 }
	u16	 GetBandwidth() { return m_wBandwidth;	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//3、数据会议终端信息
struct TDcsMtInfo
{
	//增加数据终端方式
	typedef enum 
	{
		emAddNone,			//none
		emBelowJoin,		//等待下级加入
		emInviteBelow,		//邀请下级加入
		emJoinAbove,		//加入上级
		emAboveInvite		//上级邀请加入
	}mtAddType;
	
	//上线或是下线终端的类型
	typedef enum
	{
		emT120None,			//none
		emT120Mt,			//T120协议下的MT
		emT120Mcu			//T120协议下的MCU
	}mtDataType;

public:
	u8  m_byMtId;
	u32 m_dwMtIp;
	u16 m_wPort;
	u16 m_wTimeOut;
	mtAddType  m_emJoinedType;
	mtDataType m_byDeviceType;	
public:
	TDcsMtInfo()
	{
		Clear();
	}
	void Clear()
	{
		m_byMtId    = 0;
		m_dwMtIp	= 0;
		m_wPort		= 0;
		m_wTimeOut	= 0;
		m_emJoinedType = emAddNone;
		m_byDeviceType = emT120None;
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


/************************************************************************/
/*                                                                      */
/*                          四、用户信息管理                            */
/*                                                                      */
/************************************************************************/

//1、用户组信息
class CUsrGrpsInfo
{
public:
    CUsrGrpsInfo()
    {
        // m_nGrpNum = 0;
        // guzh [9/7/2006] 调整策略
        // m_nGrpNum = MAXNUM_USRGRP;
        SetGrpNum( MAXNUM_USRGRP );
    }
    ~CUsrGrpsInfo() {};

public:
    TUsrGrpInfo m_atInfo[MAXNUM_USRGRP];
protected:
    s32         m_nGrpNum;  // guzh [9/7/2006] 本成员已经无效
                            // zhbq [12/29/2006] 保存为网络序
public:
    
    // 根据GroupId查找组，没找到返回False
    BOOL32 GetGrpById(u8 byGrpId, TUsrGrpInfo &tGrpInfo) const;

    // 根据名字查找组，没找到返回False
    BOOL32 GetGrpByName( const s8* szName,  TUsrGrpInfo &tGrpInfo ) const;

    // 添加组。返回组Id，已满返回 USRGRPID_INVALID
    u8 AddGrp ( const TUsrGrpInfo &tGrpInfo )  ;
    
    // 删除组操作。不进行相关其他资源释放操作。返回是否找到
    BOOL32 DelGrp ( u8 byGrpId );

    // 修改组操作，返回是否找到
    BOOL32 ChgGrp ( const TUsrGrpInfo &tGrpInfo ) ;

    // 保存到文件
    BOOL32 Save() const;
    // 读取
    BOOL32 Load();
    
    // 获取用户组个数
    s32  GetGrpNum()
    {
        return ntohl(m_nGrpNum);
    }
    // 设置用户组个数 只在构造的时候用到
    void SetGrpNum( s32 nGrpNum )
    {
        m_nGrpNum = htonl(nGrpNum);
        return;
    }

    // 打印
    void Print() const;
};

//2、用户分配的可操作会议信息
struct TUserTaskInfo
{
public:
	TUserTaskInfo()
	{
		SetNull();
	}
	
	void SetNull()
	{
		memset(m_achUserName, 0, sizeof(m_achUserName));
		memset(m_cTaskID, 0, sizeof(CConfId) * MAXNUM_MCU_VCCONF);
		m_wTaskNum = 0;
	}

	// 获取、设置用户名
	const s8* GetUserName()      { return m_achUserName; }
	BOOL      SetUserName(s8* pchUserName)
	{
		if (NULL == pchUserName)
		{
			return FALSE;
		}

		memcpy(m_achUserName, pchUserName, MAX_CHARLENGTH);	
		return TRUE;
	}

	// 获取、设置任务信息
	const CConfId* GetUserTaskInfo() const
	{
		return m_cTaskID;
	}
	u16 GetUserTaskNum() const
	{
		return m_wTaskNum;
	}

	BOOL SetUserTaskInfo(s8* pchUserName, u16 wTaskNum, const CConfId* pConfID)
	{
		if (pConfID == NULL || pchUserName == NULL || wTaskNum > MAXNUM_MCU_VCCONF)
		{
			return FALSE;
		}

		SetNull();
		memcpy(m_achUserName, pchUserName, MAX_CHARLENGTH);	
		m_wTaskNum = wTaskNum;
		memcpy(m_cTaskID, pConfID, sizeof(CConfId) * wTaskNum);
		return TRUE;
	}
	// 是否包含指定任务，若byDel为True，同时删除指定任务
	BOOL IsYourTask(const CConfId& cConfId, BOOL byDel = FALSE)
	{
		BOOL byYourTask = FALSE;
		for(u16 wIdx = 0; wIdx < m_wTaskNum; wIdx++)
		{
			if (cConfId == m_cTaskID[wIdx])
			{
				byYourTask = TRUE;
			}
			if (byYourTask && byDel)
			{
				CConfId* pcConfId = m_cTaskID;
				memcpy(pcConfId + wIdx, pcConfId + wIdx + 1, sizeof(CConfId) * (m_wTaskNum - wIdx - 1) );
				m_wTaskNum--;
				return TRUE;
			}
		}
		return byYourTask;
	}

	void Print() const
	{
		OspPrintf(TRUE, FALSE, "UserName:%s\n", m_achUserName);
		OspPrintf(TRUE, FALSE, "TaskNum :%d\n", m_wTaskNum);
		for (u16 wIndex = 0; wIndex < m_wTaskNum; wIndex++)
		{
			OspPrintf(TRUE, FALSE, "Conf%d:\n", wIndex);
			m_cTaskID[wIndex].Print();
		}
	}

protected:
	s8         m_achUserName[MAX_CHARLENGTH];
	u16        m_wTaskNum;
	CConfId    m_cTaskID[MAXNUM_MCU_VCCONF];
};

//3、用户任务描述
class CUsersTaskInfo
{
public:
    CUsersTaskInfo()
    {
        SetNULL();
    }
    ~CUsersTaskInfo() {};

	void SetNULL()
	{
		memset(m_abyUsed, 0, sizeof(TUserTaskInfo) * MAXNUM_VCSUSERNUM);
		memset(m_tUserTaskInfo, 0, sizeof(TUserTaskInfo) * MAXNUM_VCSUSERNUM);
	}

	// 添加、删除、修改用户分配的任务信息
	BOOL  AddUserTaskInfo(TUserTaskInfo& tUserTaskInfo);
	BOOL  DelUserTaskInfo(s8* pachUserName);
	BOOL  ChgUserTaskInfo(TUserTaskInfo& tUserTaskInfo);

	// 获取指定用户的任务信息
	BOOL  GetSpecUserTaskInfo(const s8* pachUserName, TUserTaskInfo& tUserTaskInfo);
	BOOL  GetSpecUserTaskInfo(u16 dwIdx, TUserTaskInfo& tUserTaskInfo);

	// 判断该用户是否有操作任务的权限
	BOOL  IsYourTask(const s8* pachUserName, const CConfId& cConfId);
	// 将所有用户中的指定任务删除
	void  DelSpecTaskInfo(CConfId cConfId, u16* const pdwChgIdx, u16& dwChgNum);


	// 保存、读取用户分配的任务信息
	BOOL  SaveUsersTaskInfo() const;
	BOOL  LoadUsersTaskInfo();

protected:
	u8             m_abyUsed[MAXNUM_VCSUSERNUM];     // 0:对应m_tUserTaskInfo无效 1:有效
	TUserTaskInfo  m_tUserTaskInfo[MAXNUM_VCSUSERNUM];

};


/************************************************************************/
/*                                                                      */
/*                          五、VCS会议特殊状态                         */
/*                                                                      */
/************************************************************************/
class CVCCStatus : public CBasicVCCStatus
{
public:
	CVCCStatus()
	{
		VCCDefaultStatus();
	}


	const TMt& GetReqVCMT()           { return m_tReqVCMT; }
	void  SetReqVCMT(TMt& tMt)        { memcpy(&m_tReqVCMT, &tMt, sizeof(TMt)); }

	u16   GetCurUseTWChanInd()           { return m_wCurUseTWChanInd; }
	void  SetCurUseTWChanInd(u16 wIndex) { m_wCurUseTWChanInd = wIndex; }

	u16   GetCurUseVMPChanInd()           { return m_wCurUseVMPChanInd; }
	void  SetCurUseVMPChanInd(u16 wIndex) { m_wCurUseVMPChanInd = wIndex; }

	u8   GetCurSrcSsnId()                 { return m_byCurSrcSsnId; }
	void  SetCurSrcSsnId(u8 byIndex)      { m_byCurSrcSsnId = byIndex; }

	// 调度模式切换时，除当前调度的Srcssn外其它状态变量需要恢复为原始值
	void  VCCRestoreStatus()
	{
		RestoreStatus();
		m_wCurUseTWChanInd  = 0;
		m_wCurUseVMPChanInd = 1;
		m_tReqVCMT.SetNull();
	}

	// 组呼组间调度模式切换
	void VCCRestoreGroupStatus()
	{
		RestoreGroupStatus();
		m_wCurUseTWChanInd  = 0;
		m_wCurUseVMPChanInd = 1;
		m_tReqVCMT.SetNull();		
	}

	// 默认状态
	void VCCDefaultStatus()
	{
		DefaultStatus();
		m_wCurUseTWChanInd  = 0;
		m_wCurUseVMPChanInd = 1;
		m_byCurSrcSsnId     = 0;
		m_tReqVCMT.SetNull();
	}

	void VCCPrint()
	{
		Print();
		OspPrintf(TRUE, FALSE, "m_tReqVCMT:mcuid－%d mtid－%d\n", m_tReqVCMT.GetMcuId(), m_tReqVCMT.GetMtId());
		OspPrintf(TRUE, FALSE, "m_wCurUseTWChanInd:%d\n", m_wCurUseTWChanInd);
		OspPrintf(TRUE, FALSE, "m_wCurUseVMPChanInd:%d\n", m_wCurUseVMPChanInd);
		OspPrintf(TRUE, FALSE, "m_byCurSrcSsnId:%d\n", m_byCurSrcSsnId);
	}

protected:
	u16 m_wCurUseTWChanInd;              // 自动模式下，当前使用的电视墙通道索引号
	u16 m_wCurUseVMPChanInd;             // 自动模式下，当前可使用的画面合成器起始通道号
	TMt m_tReqVCMT;                      // 当前请求调度的终端
	u8 m_byCurSrcSsnId;                  // 当前操作发起者VCS的实例ID号
};



/************************************************************************/
/*                                                                      */
/*                    六、会议适配资源管理 结构群                       */
/*                                                                      */
/************************************************************************/

//1、bas通道结构基本结构描述（len:8）
struct TBasChn
{
public:
    TBasChn():m_byChnId(0),
              m_byType(0)
    {
        m_tEqp.SetNull();
    }

    BOOL32 SetEqp(const TEqp &tEqp)
    {
        if (tEqp.IsNull())
        {
            return FALSE;
        }
        m_tEqp = tEqp;

        return TRUE;
    }
    TEqp GetEqp()
    {
        TEqp tEqp;
        tEqp.SetNull();
        if (m_tEqp.IsNull())
        {
            return tEqp;
        }
        return m_tEqp;
    }

    u8 GetEqpId()
    {
        return m_tEqp.IsNull() ? 0 : m_tEqp.GetEqpId();
    }
    u8 GetChnId()
    {
        return m_tEqp.IsNull() ? 0xff : m_byChnId;
    }
    void SetChnId(u8 byChnId)
    {
        m_byChnId = byChnId;
        return;
    }
    void SetType(u8 byType) { m_byType = byType; }
    u8   GetType(void) const { return m_byType; }
    
    void SetConfIdx(u8 byConfIdx) { m_tEqp.SetConfIdx(byConfIdx); }

    BOOL32 IsNull()
    {
        return m_tEqp.IsNull();
    }

protected:
    TEqp m_tEqp;
    u8   m_byChnId;
    u8   m_byType;
};

//2、选看适配：端媒体能力描述（len：8）
struct TMediaCap
{
public:
    TMediaCap():m_byType(MEDIA_TYPE_NULL),
                m_byFormat(0),
                m_byFrameRate(0),
                m_wBitRate(0)
    {
    }


private:
    u16 m_wBitRate;
    u8  m_byType;
    u8  m_byFormat;
    u8  m_byFrameRate;
    u8  m_abyReserved[3];
};

//3、选看适配：单通道描述（len：386×6＋29＝2345）
struct TSelChn
{
public:
private:
    TBasChn m_tChn;
    TMt     m_tSrc;
    TMt     m_atSDDst[MAXNUM_CONF_MT];
    TMt     m_atHDDst[MAXNUM_CONF_MT];
    u32     m_adwLastVCUTick;
    u8      m_bySelMode;        //VIDEO or SECVIDEO
    TMediaCap m_tSrcCap;
    TMediaCap m_tDstSDCap;
    TMediaCap m_tDstHDCap;
};

//4、选看适配：通道组描述（len：2345×192＝450240）
struct TSelChnGrp
{
public:
    TSelChnGrp()
    {
        return;
    }
    BOOL32 IsChnExist(const TEqp &tEqp, u8 byChnId, u8 byType)
    {
        return TRUE;
    }

    BOOL32 GetChnStatus(const TEqp &tEqp, u8 byChnId, THDBasVidChnStatus &tChn);
    BOOL32 UpdateChnStatus(const TEqp &tEqp, u8 byChnId, THDBasVidChnStatus &tChn);

    u8     GetChnPos(const TEqp &tEqp, u8 byChnId, u8 byType)
    {
        return 0;
    }

private:
    TSelChn m_atSelChn[MAXNUM_CONF_MT];
};

//5、广播适配：主流组（len：36）
struct TMVChnGrp
{
public:
    TMVChnGrp();

    //通道增加、获取、更新
    BOOL32 AddChn(const TEqp &tEqp, u8 byChnId, u8 byType);
    BOOL32 GetChn(u8 &byNum, TBasChn *ptBasChn);
    BOOL32 GetChn(u8 byIdx, THDBasVidChnStatus &tChn);
    BOOL32 UpdateChn(u8 byIdx, THDBasVidChnStatus &tChn);

    //通道VCU刷新过滤
    u32    GetChnVcuTick(const TEqp &tEqp, u8 byChnIdx);
    BOOL32 SetChnVcuTick(const TEqp &tEqp, u8 byChnIdx, u32 dwCurTicks);

    //通道鉴权
    u8     GetChnPos(const TEqp &tEqp, u8 byChnIdx, u8 byType);
    BOOL32 IsChnExist(const TEqp &tEqp, u8 byChnIdx, u8 byType);

    //查找适配输出端
    BOOL32 GetBasResource(u8 byMediaType, u8 byRes, TEqp &tHDBas, u8 &byChnId, u8 &byOutIdx);

    void Clear(void);

private:
    TBasChn m_atChn[MAXNUM_CONF_MVCHN];
    u32 m_adwLastVCUTick[MAXNUM_CONF_MVCHN];

};

//6、广播适配：双流组（len：36）
struct TDSChnGrp
{
public:
    TDSChnGrp();

    //通道增加、获取、更新
    BOOL32 AddChn(const TEqp &tEqp, u8 byChnId, u8 byType);
    BOOL32 GetChn(u8 &byNum, TBasChn *ptBasChn);
    BOOL32 GetChn(u8 byIdx, THDBasVidChnStatus &tChn);
    BOOL32 UpdateChn(u8 byIdx, THDBasVidChnStatus &tChn);

    //通道VCU刷新过滤
    u32    GetChnVcuTick(const TEqp &tEqp, u8 byChnIdx);
    BOOL32 SetChnVcuTick(const TEqp &tEqp, u8 byChnIdx, u32 dwCurTicks);

    //通道鉴权
    BOOL32 IsChnExist(const TEqp &tEqp, u8 byChnIdx, u8 byType);
    u8     GetChnPos(const TEqp &tEqp, u8 byChnIdx, u8 byType);

    //查找适配输出端
    BOOL32 GetBasResource(u8 byMediaType, u8 byRes, TEqp &tHDBas, u8 &byChnId, u8 &byOutIdx, BOOL32 bH263p = FALSE);

    BOOL32 IsGrpMpu(void);

    void Clear(void);

private:
    TBasChn m_atChn[MAXNUM_CONF_DSCHN];
    u32 m_adwLastVCUTick[MAXNUM_CONF_DSCHN];
};


//7、适配管理类：广播适配＋选看适配（len：4＋36＋36＋450240＋450240＝900556）

class CBasMgr
{
    enum emChnType
    {
        emBegin,
            emNV    = 1,    //普通适配通道
            emVGA   = 2,    //静态双流适配通道
            emH263p = 3,    //H263+双流适配通道
        emEnd
    };

    enum emMauState
    {
        emBgn,
            emOnline    = 1,
            emReserved  = 2,
        emEd
    };

    enum emDualType
    {
        emBgin,
            emDualVGA   = 1,
            emDualEqMV  = 2,
            emDualDouble= 3,
        emEnd1
    };

    enum emAdpMode
    {
        emModeBg,
            emModeMau   = 1,
            emModeMpu   = 2,
        emModeEd
    };
public:
    CBasMgr();
    ~CBasMgr();

    /*************************************************/
    /*                                               */
    /*                 广播适配相关                  */
    /*                                               */
    /*************************************************/

    //广播适配：适配资源占用
    BOOL32 OcuppyHdBasChn(const TConfInfo &tConfInfo);

    //广播适配：适配资源释放
    void   ReleaseHdBasChn(void);

    //广播适配：资源剩余预判
    BOOL32 IsHdBasSufficient(const TConfInfo &tConfInfo);

    //广播适配：会议需求预判
    void   GetNeededMau(const TConfInfo &tConfInfo, u8 &byNVChn, u8 &byH263Chn, u8 &byVGAChn);
    void   GetNeededMpu(const TConfInfo &tConfInfo, u8 &byNVChn, u8 &byDSChn);

    //广播适配：获取所有适配通道和适配模式
    void   GetChnGrp(u8 &byNum, TBasChn *ptBasChn, u8 byAdpMode);
    u8     GetChnMode(const TEqp &TEqp, u8 byChnId);

    //广播适配：获取会议的适配满足状况
    void   GetHdBasStatus(TMcuHdBasStatus &tMauStatus, TConfInfo &tCurConf);

    //广播适配：低速适配特殊支持
    BOOL32 GetLowBREqp(const TConfInfo &tConfInfo, TEqp &tEqp, u8 &byChnId);

    //广播适配：获取某群组的适配输出
    BOOL32 GetBasResource(u8    byMediaType,
                          u8    byRes,
                          TEqp &tHDBas,
                          u8   &byChnId,
                          u8   &byOutIdx,
                          BOOL32 bDual = FALSE, BOOL32 bH263p = FALSE);

    //广播适配：增加、获取、刷新通道
    void   AddSelChn(const TEqp &tEqp, u8 byChnId, u8 byChnType, BOOL32 bDual = FALSE);
    BOOL32 GetSelChn(const TEqp &tEqp, u8 byChnId, THDBasVidChnStatus &tStatus);
    BOOL32 UpdateSelChn(const TEqp &tEqp, u8 byChnId, THDBasVidChnStatus &tStatus);

    //广播适配：分配通道参数
    void   AssignBasChn(const TConfInfo &tConfInfo, u8 byMode);

    /*************************************************/
    /*                                               */
    /*                 选看适配相关                  */
    /*                                               */
    /*************************************************/


    /*************************************************/
    /*                                               */
    /*                   公共部分                    */
    /*                                               */
    /*************************************************/
    //构造、打印
    void Clear(void);
    void Print(void);
    
    //获取通道索引（全会议唯一）
    u8     GetChnId(const TEqp &tEqp, u8 byChIdx);

    //获取、更新通道状态
    BOOL32 GetChnStatus(const TEqp &tEqp, u8 byChnId, THDBasVidChnStatus &tStatus);
    BOOL32 UpdateChn(const TEqp &tEqp, u8 byChnId, THDBasVidChnStatus &tMVStatus);
    
    //适配通道Tick数获取和刷新，用于VCU
    BOOL32 SetChnVcuTick(const TEqp &tEqp, u8 byChnIdx, u32 dwCurTicks);
    u32    GetChnVcuTick(const TEqp &tEqp, u8 byChnIdx);

    //获取适配资源类型
    BOOL32 IsBrdGrpMpu(void);

private:

    //广播适配：适配资源占用
    BOOL32 OcuppyMau(const TConfInfo &tConfInfo);
    BOOL32 OcuppyMpu(const TConfInfo &tConfInfo);

    //广播适配：资源剩余预判
    BOOL32 IsMauSufficient(const TConfInfo &tConfInfo);
    BOOL32 IsMpuSufficient(const TConfInfo &tConfInfo);

    //广播适配：分配通道参数
    void   AssignMVBasChn(const TConfInfo &tConfInfo);
    void   AssignDSBasChn(const TConfInfo &tConfInfo);

    //广播适配：增加通道
    void   AddBrdChn(const TEqp &tEqp, u8 byChnId, u8 byChnType, BOOL32 bDual = FALSE);
    
    //广播适配：双流模式接口
    void   SetDualVGA(BOOL32 bVGA = TRUE) { m_emType = bVGA ? emDualVGA : m_emType;   }
    BOOL32 IsDualVGA(void) const { return (m_emType == emDualVGA);   }
    void   SetDualDouble(BOOL32 bDouble = TRUE) { m_emType = bDouble ? emDualDouble : m_emType;   }
    BOOL32 IsDualDouble(void) const { return (m_emType == emDualDouble);    }
    void   SetDualEqMv(BOOL32 bEq = TRUE) { m_emType = bEq ? emDualEqMV : m_emType;    }
    BOOL32 IsDualEqMv(void) const { return (m_emType == emDualEqMV); }
    
    //获取适配源的动态载荷
    u8     GetActivePL(const TConfInfo &tConfInfo, u8 byMediaType);

protected:

private:
    emDualType  m_emType;           //会议双流类型
    TMVChnGrp   m_tMVBrdChn;        //本会议所有的主流广播适配通道
    TDSChnGrp   m_tDSBrdChn;        //本会议所有的双流广播适配通道
    TSelChnGrp  m_tMVSelChn;        //本会议所有的主流选看适配通道
    TSelChnGrp  m_tDSSelChn;        //本会议所有的双流选看适配通道
};

//选看适配支持(过渡结构)
struct TSelChnTmp
{
public:
    
    u8   GetChnIdx(void) const { return m_byChnIdx; }
    void SetChnIdx(u8 byChnId) { m_byChnIdx = byChnId; }

    void SetBas(const TEqp &tEqp) { m_tBas = tEqp;    }
    TEqp GetBas(void) {  return m_tBas;   }

	void SetSelMode(u8 bySelMode) { m_bySelMode = bySelMode; }
	u8   GetSelMode()             { return m_bySelMode; }

    u8   GetSrcId(void) { return m_bySrcId; }
    void SetSrcId(u8 byId) { m_bySrcId = byId; };
    
    void GetDstId(u8 *pbyDstId, u8 &byLen)
    {
        byLen = min(byLen, GetDstNum());
        memcpy(pbyDstId, &m_abyDstId, byLen);
        return;
    }
    BOOL32 SetDstId(u8 byDstId)
    {
        u8 byFstIdle = 0xff;
        BOOL32 bExist = FALSE;

        for (u8 byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
        {
            if (0 == byIdx)
            {
                byFstIdle = byIdx;
            }
            if (byDstId == m_abyDstId[byIdx])
            {
                return TRUE;
            }
        }

        if (byFstIdle != 0xff)
        {
            m_abyDstId[byFstIdle] = byDstId;
            return TRUE;
        }
        OspPrintf(TRUE, FALSE, "[TSelChn::SetDstId] no pos for dst.%d!\n", byDstId);
        return FALSE;
    }

    BOOL32 RemoveDstId(u8 byDstId)
    {
        u8 byIdx = 0;
        u8 byExistPos = 0xff;
        for (byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
        {
            if (byDstId == m_abyDstId[byIdx])
            {
                byExistPos = byIdx;
                break;
            }
        }
        if (0xff != byExistPos)
        {
            //清空目的
            m_abyDstId[byExistPos] = 0;

            //整理序列
            for (byIdx = byExistPos+1; byIdx < MAXNUM_CONF_MT; byIdx++)
            {
                if (m_abyDstId[byIdx] == 0)
                {
                    break;
                }
                m_abyDstId[byIdx-1] = m_abyDstId[byIdx];
            }

            //是否清空组
            if (0 == GetDstNum())
            {
                SetNull();
            }
			return TRUE;
        }
        return  FALSE;

    }
    BOOL32 IsDstExist(u8 byDstId)
    {
        BOOL32 bExist = FALSE;
        
        for (u8 byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
        {
            if (byDstId == m_abyDstId[byIdx])
            {
                bExist = TRUE;
                break;
            }
        }
        return bExist;
    }

    void SetNull()
    {
        m_tBas.SetNull();
        m_byChnIdx = 0;
        m_bySrcId = 0;
		m_bySelMode = MODE_NONE;
        memset(&m_abyDstId, 0, sizeof(m_abyDstId));
    }

    BOOL32 IsNull()
    {
        return m_tBas.IsNull() && m_bySrcId != 0 && GetDstNum() != 0;
    }

    void Print()
    {
        OspPrintf(TRUE, FALSE, "\tBas:\t%d\n", m_tBas.GetEqpId());
        OspPrintf(TRUE, FALSE, "\tChn:\t%d\n", m_byChnIdx);
        OspPrintf(TRUE, FALSE, "\tSrc:\n");
        OspPrintf(TRUE, FALSE, "\t\tMt.%d\n", m_bySrcId);
        OspPrintf(TRUE, FALSE, "\tDst(s):\n");
        for (u8 byIdx = 0; byIdx < GetDstNum(); byIdx++)
        {
            OspPrintf(TRUE, FALSE, "\t\tMt.%d\n", m_abyDstId[byIdx]);
        }
		OspPrintf(TRUE, FALSE, "\tSelMode:\t%d\n", m_bySelMode);
    }

    u8 GetDstNum(void)
    {
        u8 byLen = 0;
        for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
        {
            if (0 == m_abyDstId[byIdx])
            {
                break;
            }
            byLen++;
        }
        return byLen;
    }

private:
    u8 m_bySrcId;
    u8 m_abyDstId[MAXNUM_CONF_MT];
    TEqp m_tBas;
    u8 m_byChnIdx;
	u8 m_bySelMode;
};

class CSelChnGrp
{
public:
    CSelChnGrp()
    {
        Clear();
    }
    BOOL32 AddSel(u8 bySrcId, u8 byDstId, u8 bySelMode, TEqp &tEqp, u8 byChnIdx)
    {
        u8 byFstIdle = 0xff;
        u8 byExistChn = 0xff;

        for(u8 byIdx = 0; byIdx < MAXNUM_PERIEQP; byIdx++)
        {
            if (m_atSelChn[byIdx].GetSrcId() == 0 &&
				0xff == byFstIdle)
            {
                byFstIdle = byIdx;
            }
            //本版本不支持同源挂多BAS的情况, 4.6.1考虑支持
            if (m_atSelChn[byIdx].GetSrcId() == bySrcId &&
				m_atSelChn[byIdx].GetSelMode())
            {
                byExistChn = byIdx;
                break;
            }
        }

        //不存在，开辟新位置
        if (0xff == byExistChn)
        {
            m_atSelChn[byFstIdle].SetBas(tEqp);
            m_atSelChn[byFstIdle].SetSrcId(bySrcId);
            m_atSelChn[byFstIdle].SetDstId(byDstId);
            m_atSelChn[byFstIdle].SetChnIdx(byChnIdx);
			m_atSelChn[byFstIdle].SetSelMode(bySelMode);
            return TRUE;
        }
        //存在，增加目的端
        else
        {
            m_atSelChn[byExistChn].SetDstId(byDstId);
            return TRUE;
        }
        return FALSE;
    }

    BOOL32 GetSelBasChn(u8 bySrcId, u8 byDstId, u8 bySelMode, TEqp &tEqp, u8 &byChnIdx, BOOL32 &bRlsChn)
    {
        for(u8 byIdx = 0; byIdx < MAXNUM_PERIEQP; byIdx++)
        {
            if (m_atSelChn[byIdx].GetSrcId() == bySrcId &&
                m_atSelChn[byIdx].IsDstExist(byDstId) &&
				m_atSelChn[byIdx].GetSelMode() == bySelMode)
            {
                tEqp = m_atSelChn[byIdx].GetBas();
                byChnIdx = m_atSelChn[byIdx].GetChnIdx();
                bRlsChn = m_atSelChn[byIdx].GetDstNum() == 1;   //只剩当前最后一个目的，外部控制释放通道
                return TRUE;
            }
        }
        return FALSE;
    }

    BOOL32 RemoveSel(u8 bySrcId, u8 byDstId, u8 bySelMode)
    {
        for(u8 byIdx = 0; byIdx < MAXNUM_PERIEQP; byIdx++)
        {
            if (m_atSelChn[byIdx].GetSrcId() == bySrcId &&
                m_atSelChn[byIdx].IsDstExist(byDstId) &&
				bySelMode == m_atSelChn[byIdx].GetSelMode())
            {
                return m_atSelChn[byIdx].RemoveDstId(byDstId);
            }
        }
        return FALSE;
    }

    BOOL32 GetSelSrc(const TEqp &tEqp, u8 byChnIdx, u8 &bySrcId)
    {
        for(u8 byIdx = 0; byIdx < MAXNUM_PERIEQP; byIdx++)
        {
            if (m_atSelChn[byIdx].GetBas() == tEqp &&
                m_atSelChn[byIdx].GetChnIdx() == byChnIdx)
            {
                bySrcId = m_atSelChn[byIdx].GetSrcId();
                return TRUE;
            }
        }
        return FALSE;
    }

    BOOL32 GetSelDst(const TEqp &tEqp, u8 byChnIdx, u8 *pbyDstId, u8 &byNum)
    {
        for(u8 byIdx = 0; byIdx < MAXNUM_PERIEQP; byIdx++)
        {
            if (m_atSelChn[byIdx].GetBas() == tEqp &&
                m_atSelChn[byIdx].GetChnIdx() == byChnIdx)
            {
                m_atSelChn[byIdx].GetDstId(pbyDstId, byNum);
                return TRUE;
            }
        }
        return FALSE;
    }

	BOOL32 FindSelSrc(u8 bySelSrcId, u8 bySelMode, u8& byAdpChnNum, u8* pbyEqpId, u8* pbyChnIdx)
	{
		byAdpChnNum = 0;
        for(u8 byIdx = 0; byIdx < MAXNUM_PERIEQP; byIdx++)
        {
			if (bySelSrcId == m_atSelChn[byIdx].GetSrcId() &&
				bySelMode == m_atSelChn[byIdx].GetSelMode())
			{
				pbyEqpId[byAdpChnNum]  = m_atSelChn[byIdx].GetBas().GetEqpId();
				pbyChnIdx[byAdpChnNum] = m_atSelChn[byIdx].GetChnIdx();
				byAdpChnNum++;
			}
		}
		if (byAdpChnNum != 0)
		{
			return TRUE;
		}
		
		return FALSE;
	}

	BOOL32 IsMtInSelAdpGroup(u8 bySelSrcId, u8 bySelDstId, TEqp* ptEqp = NULL, u8* pbyChnlIdx = NULL)
	{
		for(u8 byGroupId = 0; byGroupId < MAXNUM_PERIEQP; byGroupId++)
		{
			if (bySelSrcId == m_atSelChn[byGroupId].GetSrcId())
			{
				u8 abyDstId[MAXNUM_CONF_MT];
				u8 byDstNum = MAXNUM_CONF_MT;
				m_atSelChn[byGroupId].GetDstId(abyDstId, byDstNum);
				for (u8 byDstIdx = 0; byDstIdx < byDstNum; byDstIdx++)
				{
					if (bySelDstId == abyDstId[byDstIdx])
					{
						if(ptEqp != NULL)
						{
							*ptEqp = m_atSelChn[byGroupId].GetBas();
						}
						if (pbyChnlIdx != NULL)
						{
							*pbyChnlIdx = m_atSelChn[byGroupId].GetChnIdx();
						}	
						return TRUE;
					}
				}				
			}
		}
		return FALSE;
	}
	
	void GetUsedAdptChnlInfo(u8& byChnlNum, u8* abyBasId, u8* abyChnlIdx)
	{
		byChnlNum = 0;
		if (NULL == abyBasId || NULL == abyChnlIdx)
		{
			OspPrintf(TRUE, FALSE, "GetUsedAdptChnlInfo with wrong param\n");
			return;
		}
		for (u8 byChnIdx = 0; byChnIdx < MAXNUM_PERIEQP; byChnIdx++)
		{
			if (!m_atSelChn[byChnIdx].GetBas().IsNull())
			{
				abyBasId[byChnlNum]   = m_atSelChn[byChnIdx].GetBas().GetEqpId();
				abyChnlIdx[byChnlNum] = m_atSelChn[byChnIdx].GetChnIdx();
				byChnlNum++;
			}
		}
		return;
	}

    void Clear() {  memset(this, 0, sizeof(m_atSelChn)); }

    void Print()
    {
        OspPrintf(TRUE, FALSE, "\nSelGrp as follow:\n\n");

        for(u8 byIdx = 0; byIdx < MAXNUM_PERIEQP; byIdx++)
        {
            if (!m_atSelChn[byIdx].IsNull())
            {
                OspPrintf(TRUE, FALSE, "-----------------------------");
                OspPrintf(TRUE, FALSE, "No[%d]:\n", byIdx);
                m_atSelChn[byIdx].Print();
            }
        }
    }

private:
    TSelChnTmp m_atSelChn[MAXNUM_PERIEQP];
};



/************************************************************************/
/*                                                                      */
/*                    七、适配目的群组管理 结构群                       */
/*                                                                      */
/************************************************************************/

//1、主流接收群组（len：24*8＝192）FIXME: 未考虑主流为非264

struct TMVRcvGrp 
{
    enum emType
    {
        emBegin     = 0,
            em1080  = 1,
            em720p  = 2,
            em4Cif  = 3,
            emCif   = 4,
            emH263  = 5,
            emMp4   = 6,
        emEnd
    };
public:
    
    //构造
    TMVRcvGrp() {   Clear();    }

    //成员增、删
    void AddMem(u8 byMtId, emType type, BOOL32 bExceptEither = FALSE);
    void RemoveMem(u8 byMtId, emType type);
    
    //免适配成员增、删
    void AddExcept(u8 byMtId);
	void ClearExpt( void );

    //群组成员获取
    void GetMtList(u8 byRes, u8 &byNum, u8 *pbyMt, BOOL32 bForce = FALSE);

    //其他
    void ResIn2Out(u8 byRes, u8 &byConfRes, u8 &byConfType);
    BOOL32 GetMtMediaRes(u8 byMtId, u8 &byMediaType, u8 &byRes);
    BOOL32 IsNeedAdp(void);
    BOOL32 IsMtInExcept(u8 byMtId);

    void Clear(){   memset(this, 0, sizeof(TMVRcvGrp)); }
    void Print(void);

protected:
    BOOL32 IsGrpNull(emType type);

private:
    u8 m_aabyGrp[emEnd][24];
    u8 m_abyExcept[24];   //主流免适配区
};

//2、双流接收群组（len: 24*11=264） FIXME: 未考虑1080双流

struct TDSRcvGrp
{
    enum emType
    {
        emBegin     = 0,
            em720p  = 1,
            em4Cif  = 2,
            emCif   = 3,
            emH263plus = 4,
            emUXGA  = 5,
            emSXGA  = 6,
            emXGA   = 7,
            emSVGA  = 8,
            emVGA   = 9,
        emEnd
    };

    //构造
    TDSRcvGrp() {   Clear();    }

    //成员增、删
    void AddMem(u8 byMtId, emType type, BOOL32 bExceptEither = FALSE);
    void RemoveMem(u8 byMtId, emType type);

    //免适配成员增、删
    void AddExcept(u8 byMtId);
	void ClearExpt(void);

    //群组成员获取
    void GetMtList(u8 byRes, u8 &byNum, u8 *pbyMt);
    
    //其他
    void ResIn2Out(u8 byRes, u8 &byConfRes, u8 &byConfType);
    BOOL32 IsNeedAdp(void);
    BOOL32 IsMtInExcept(u8 byMtId);
    BOOL32 GetMtMediaRes(u8 byMtId, u8 &byMediaType, u8 &byRes);

    void Clear(){   memset(this, 0, sizeof(TDSRcvGrp));    }
    void Print(void);

protected:
    u8 m_aabyGrp[emEnd][24];
    u8 m_abyExcept[24];   //双流免适配区
};


//3、接收群组集成（len: 264*192=456）

class CRcvGrp
{
public:

    //成员增、删
    void AddMem(u8 byMtId, TSimCapSet &tSim, BOOL32 bExc = FALSE);
    void AddMem(u8 byMtId, TDStreamCap &tDCap, BOOL32 bExc = FALSE);
    void RemoveMem(u8 byMtId, TSimCapSet &tSim);
    void RemoveMem(u8 byMtId, TDStreamCap &tDCap);

    //免适配成员增、删
    void AddExcept(u8 byMtId, BOOL32 bDual = FALSE);
	void ClearExpt(BOOL32 bDual = FALSE);

    //获取内部分辨率标识
    u8   GetMVRes(u8 byType, u8 byRes);
    u8   GetDSRes(u8 byType, u8 byRes);

    //群组成员获取
    void GetMVMtList(u8 byType, u8 byRes, u8 &byNum, u8 *pbyMt, BOOL32 bForce = FALSE);
    void GetDSMtList(u8 byType, u8 byRes, u8 &byNum, u8 *pbyMt);

    //其他
    BOOL32 GetMtMediaRes(u8 byMtId, u8 &byMediaType, u8 &byRes, BOOL32 bDual = FALSE);
    BOOL32 IsMtNeedAdp(u8 byMtId, BOOL32 bMV = TRUE);
    BOOL32 IsNeedAdp(BOOL32 bMV = TRUE);

    void Clear();
    void Print();

protected:
    TMVRcvGrp m_tMVGrp;
    TDSRcvGrp m_tDSGrp;    
};



/************************************************************************/
/*                                                                      */
/*                     八、MCU 会议 全局数据类                          */
/*                                                                      */
/************************************************************************/

class CMcuVcInst;

class CMcuVcData
{
	#define USERNUM_PERPKT  32      //每包消息中用户个数
    #define ADDRENTRYNUM_PERPKT 64  //每包消息中地址簿条目个数
    #define ADDRGROUPNUM_PERPKT 16  //每包消息中地址簿条目组个数
	
	friend class CMpManager;
    friend class CMcuVcInst;

public:
    
    // 获取MCU状态
    BOOL32 GetMcuCurStatus( TMcuStatus &tMcuStatus );
    
    /************************************************************************/
    /*                                                                      */
    /*                     1、会议信息表操作函数                            */
    /*                                                                      */
    /************************************************************************/
	BOOL32 AddConf( CMcuVcInst *pConfInst, BOOL32 bRefreshHtml = TRUE );
	BOOL32 RemoveConf( u8 byConfIdx, BOOL32 bRefreshHtml = TRUE );
	u8     GetConfIdx( const CConfId & cConfId ) const;
    CConfId GetConfId( const u8 &byConfIdx ) const;
	u8     GetAllConf( TConfInfo atConfBuf[], u8 byBufSize, u8 byMode = 0 ) const;
	u8     GetConfNum( BOOL32 bIncOngoing, BOOL32 bIncSched, BOOL32 bIncTempl, u8 byConfSource = MCS_CONF ) const;    // 统计指定的会议或模板数量
	BOOL32 GetConfMtTable( u8 byConfIdx, TConfMtTable * ptMtTable ) const;
	BOOL32 GetConfProtectInfo( u8 byConfIdx, TConfProtectInfo *tConfProtectInfo ) const;
	BOOL32 GetConfAllMtInfo( u8 byConfIdx, TConfAllMtInfo * pConfAllMtInfo ) const;
	TConfMtTable     *GetConfMtTable( u8 byConfIdx ) const;
	TConfSwitchTable *GetConfSwitchTable( u8 byConfIdx ) const;
	TConfProtectInfo *GetConfProtectInfo( u8 byConfIdx ) const;
	TConfEqpModule   *GetConfEqpModule( u8 byConfIdx ) const;
	TConfAllMtInfo   *GetConfAllMtInfo( u8 byConfIdx ) const;
	CMcuVcInst       *GetConfInstHandle( u8 byConfIdx );
	TPeriEqpData     *GetEqpData( u8 byEqpId );
	BOOL32 SaveConfToFile( u8 byConfIdx, BOOL32 bTemplate, BOOL32 bDefaultConf = FALSE );
	BOOL32 RemoveConfFromFile( CConfId cConfId );
	BOOL32 GetConfFromFile(u8 byConfIdx, TPackConfStore *ptPackConfStore);
	void   RefreshHtml( void );
	BOOL32 IsConfNameRepeat( LPCSTR lpszConfName, BOOL32 IsTemple );
	BOOL32 IsConfE164Repeat( LPCSTR lpszConfE164, BOOL32 IsTemple );
	u8   GetOngoingConfIdxByE164( LPCSTR lpszConfE164 );
    u8   GetTemConfIdxByE164( LPCSTR lpszConfE164 );
    u16  GetConfRateByConfIdx( u16 wConfIdx, u16& wFirstRate, u16& wSecondRate, BOOL32 bTemplate = FALSE ); // 根据会议序号取会议速率
    u8   GetConfNameByConfId( const CConfId cConfId, LPCSTR &lpszConfName );  // 根据会议ID取会议名称
    CConfId GetConfIdByName(LPCSTR lpszConfName, BOOL32 bTemplate );  // 根据会议名称取CConfId
	CConfId GetConfIdByE164( LPCSTR lpszConfE164, BOOL32 bTemplate ); // 根据会议164号取CConfId

	void RegisterConfToGK( u8 byConfIdx, u8 byDriId = 0, BOOL32 bTemplate = FALSE, BOOL32 bUnReg = FALSE );
    void ConfChargeByGK( u8 byConfIdx, u8 byDriId, BOOL32 bStopCharge = FALSE, u8 byCreateBy = CONF_CREATE_MCS, u8 byMtNum = 0 );
    void ConfInfoMsgPack( CMcuVcInst *pcSchInst, CServMsg &cServMsg );//会议信息消息打包

    CConfId MakeConfId( u8 byConfIdx, u8 byTemplate, u8 byUsrGrpId, u8 byConfSource = MCS_CONF );
    u32     GetMakeTimesFromConfId(const CConfId& cConfId) const;
    
    void SetInsState( u8 byInsId, BOOL32 bState );
    u8   GetIdleInsId( void );       

    BOOL32 IsSavingBandwidth(void);             //是否节省带宽

    void    GetMcuEqpCapacity(TEqpCapacity& tMcuEqpCap); // get the mcu's cap of equipment
    void    GetConfEqpDemand(TConfInfo& tConfInfo, TEqpCapacity& tConfEqpDemand );
	BOOL32  AnalyEqpCapacity( TEqpCapacity& tConfEqpDemand, TEqpCapacity& tMcuSupportCap);
	void GetBasCapacity( TBasReqInfo& tBasCap ); // 取Bas能力集
	void GetPrsCapacity( TEqpReqInfo& tPrsCap ); // 取Prs能力集
    
    void GetHDBasCapacity( THDBasReqInfo& tHDBasCap ); // 取HDBas能力集, zgc, 2008-08-11
    
    u8   GetConfFECType( u8 byConfIdx, u8 byMode );  // 当前会议前向纠错
    u8   GetConfEncryptMode( u8 byConfIdx );         // 当前会议的码流加密模式
    
    /************************************************************************/
    /*                                                                      */
    /*                     2、会议模板操作函数                              */
    /*                                                                      */
    /************************************************************************/
    BOOL32 CreateTemplate( void ); 
    BOOL32 AddTemplate( TTemplateInfo &tTemInfo );
    BOOL32 ModifyTemplate( TTemplateInfo &tTemInfo, BOOL32 bSameE164AndName );
    BOOL32 DelTemplate( u8 byConfIdx );
    BOOL32 GetTemplate( u8 byConfIdx, TTemplateInfo &tTemInfo );
    BOOL32 SetConfMapInsId( u8 byConfIdx, u8 byInsId );
    u8     GetConfMapInsId( u8 byConfIdx );
    BOOL32 SetTemRegGK( u8 byConfIdx, BOOL32 bReg );
    BOOL32 IsTemRegGK( u8 byConfIdx );
    TConfMapData   GetConfMapData( u8 byConfIdx );
    u8     GetIdleConfidx( void );
    void   Msg2TemInfo( CServMsg &cMsg, TTemplateInfo &tTemInfo, 
						s8** pszUnProcInfoHead = NULL, u16* pwUnProcLen = NULL );
    void   TemInfo2Msg( TTemplateInfo &tTemInfo, CServMsg &cMsg );
    void   ShowTemplate( void );
    void   ShowConfMap( void );
    
    //卫星会议临时使用
    u32    GetExistSatCastIp();
    u16    GetExistSatCastPort();
	
    /************************************************************************/
    /*                                                                      */
    /*                     3、外设表操作函数                                */
    /*                                                                      */
    /************************************************************************/
	void   InitPeriEqpList();
	void   SetPeriEqpConnected( u8 byEqpId, BOOL32 bConnected = TRUE );
	BOOL32 IsPeriEqpConnected( u8 byEqpId );
	void   SetPeriEqpLogicChnnl( u8 byEqpId, u8 byMediaType, u8 byChnnlNum, const TLogicalChannel * ptStartChnnl, BOOL32 bForwardChnnl );
	BOOL32 GetPeriEqpLogicChnnl( u8 byEpqId, u8 byMediaType, u8 * pbyChnnlNum, TLogicalChannel * ptStartChnnl, BOOL32 bForwardChnnl );
	void   SetEqpRtcpDstAddr( u8 byEqpId, u8 byChnnl, u32 dwDstIp, u16 wDstPort, u8 byMode = MODE_VIDEO );
	BOOL32 GetEqpRtcpDstAddr( u8 byEqpId, u8 byChnnl, u32 &dwDstIp, u16 &wDstPort, u8 byMode = MODE_VIDEO );	
	void   SetPeriEqpStatus( u8 byEqpId, const TPeriEqpStatus * ptStatus );
	BOOL32 GetPeriEqpStatus( u8 byEqpId, TPeriEqpStatus * ptStatus );
	void   SendPeriEqpStatusToMcs(u8 byEqpId);
	void   SetPeriEqpSrc( u8 byEqpId, const TMt * ptSrc, u8 byChnnlNo, u8 byMode );
	BOOL32 GetPeriEqpSrc( u8 byEqpId, TMt * ptSrc, u8 byChnnlNo, u8 byMode );
	BOOL32 IsPeriEqpValid( u8 byEqpId );
	void   SetPeriEqpIsValid( u8 byEqpId );
    TEqp   GetEqp( u8 byEqpId );
	u8     GetEqpType( u8 byEqpId );
	BOOL32 GetIdleVMP( u8 * pbyVMPBuf, u8 &byNum, u8 byVMPLen );
    BOOL32 GetIdleVmpTw( u8 * pbyMPWBuf, u8 &byNum, u8 byMPWLen );
	u8     GetIdleMixGroup( u8 &byEqpId, u8 &byGrpId );
	u8     GetIdleBasChl( u8 byAdaptType, u8 &byEqpId, u8 &byChlId );
    BOOL32 GetIdleHDBasVidChl( u8 &byEqpId, u8 &byChlIdx );
	BOOL32 IsIdleHDBasVidChlExist(u8* pbyEqpId = NULL, u8* pbyChlIdx = NULL);

    void   GetIdleMau(u8 &byNVChn, u8 &byVGAChn, u8 &byH263pChn);
    void   GetIdleMpu(u8 &byChnNum);
    void   GetMpuNum(u8 &byMpuNum);

    BOOL32 GetIdleMpuChn(u8 &byEqpId, u8 &byChnId);
    BOOL32 GetIdleMauChn(u8 byChnType, u8 &byEqpId, u8 &byChnId);

    void   ReleaseHDBasChn(u8 byEqpId, u8 byChnId);
    void   ResetHDBasChn(u8 byEqpId, u8 byChnId);

    u8     GetIdlePrsChl( u8 &byEqpId, u8 &byChlId, u8 &byChlId2, u8 &byChlIdAud );
	BOOL32 GetIdlePrsChls( u8 byChannelNum, TPrsChannel& tPrsChannel);
	u8     GetMaxIdleChlsPrsId(u8& byEqpPrsId );
	BOOL32 GetIdlePrsChls(u8 byPrsId, u8 byChannelNum, TPrsChannel& tPrsChannel);
    u8     GetIdlePrsChl( u8 &byEqpId, u8 &byChlId );
    BOOL32 IsRecorderOnline (u8 byEqpId);
	void   SetEqpAlias( u8 byEqpId, LPCSTR lpszAlias );
	LPCSTR GetEqpAlias( u8 byEqpId );
    BOOL32 IsEqpH263pMau( u8 byEqpId );

    void   SetEqpIp(u8 byId, u32 &dwIp)
    {
        if (byId < MAXNUM_MCU_PERIEQP)
        {
            m_adwEqpIp[byId] = dwIp;
        }
        return;
    }
    u32    GetEqpIp(u8 byId)
    {
        if (byId >= MAXNUM_MCU_PERIEQP)
        {
            return 0;
        }
        return m_adwEqpIp[byId];
    }



    /************************************************************************/
    /*                                                                      */
    /*                     4、DCS表操作函数                                 */
    /*                                                                      */
    /************************************************************************/
	void	InitPeriDcsList();	//初始化当前DCS列表
	void	SetPeriDcsConnected( u8 byDcsId, BOOL32 bConnected = TRUE );
	void	SetPeriDcsValid( u8 byDcsId );
	BOOL32  IsPeriDcsConfiged( u32 dwDcsIp );
	BOOL32	IsPeriDcsConnected( u8 byDcsId );
	BOOL32	IsPeriDcsValid( u8 byDcsId );
	void	SetDcsAlias( u8 byDcsId );
	LPCSTR  GetDcsAlias( u8 byDcsId );
    TEqp    GetDcs( u8 byDcsId );
    BOOL32  GetPeriDcsStatus( u8 byDcsId, TPeriDcsStatus * ptStatus );
    BOOL32  SetPeriDcsStatus( u8 byDcsId, const TPeriDcsStatus * ptStatus );
	
    /************************************************************************/
    /*                                                                      */
    /*                     5、适配器状态信息的读写处理                      */
    /*                                                                      */
    /************************************************************************/
    void SetBasChanStatus( u8 byEqpId, u8 byChanNo, u8 byStatus );
    void SetBasChanReserved( u8 byEqpId, u8 byChanNo, BOOL32 bReserved );

    u8   GetTvWallOutputMode( u8 byEqpId );
	u8   GetHduOutputMode( u8 byEqpId );    //4.6.1  新加  jlb
	
    /************************************************************************/
    /*                                                                      */
    /*                     6、会议控制台表操作函数                          */
    /*                                                                      */
    /************************************************************************/
	void   SetMcConnected( u16 wMcInstId, BOOL32 bConnected = TRUE );
	BOOL32 IsMcConnected( u16 wMcInstId );
	void   SetMcLogicChnnl( u16 wMcInstId, u8 byMediaType, u8 byChnnlNum, const TLogicalChannel * ptStartChnnl );
	BOOL32 GetMcLogicChnnl( u16 wMcInstId, u8 byMediaType, u8 * pbyChnnlNum, TLogicalChannel * ptStartChnnl );
	void   SetMcSrc( u16 wMcInstId, const TMt * ptSrc, u8 byChnnlNo, u8 byMode );
	BOOL32 GetMcSrc( u16 wMcInstId, TMt * ptSrc, u8 byChnnlNo, u8 byMode );
	void   SetMcsRegInfo( u16 wMcInstId, TMcsRegInfo tMcsRegInfo );
	void   GetMcsRegInfo( u16 wMcInstId, TMcsRegInfo *ptMcsRegInfo );

    /************************************************************************/
    /*                                                                      */
    /*                     7、Mp表操作函数                                  */
    /*                                                                      */
    /************************************************************************/
	BOOL32 AddMp( TMp tMp );
	BOOL32 RemoveMp( u8 byMpId );
	BOOL32 IsMpConnected( u8 byMpId );
	u32    GetMpIpAddr( u8 byMpId );
    u8     GetMpNum( void );
	u8     FindMp( u32 dwIp );
	u32    GetAnyValidMp( void );
	u8     AssignMpIdByBurden( u8 byConfIdx, TMtAlias &tMtAlias, u8 byMtId, u16 wConfBR );  // 增加别名传入，别名可能是IP或E164+IP
	BOOL32 IncMpMtNum( u8 byMpId, u8 byConfIdx, u8 byMtId, u16 wConfBR );
	void   DecMpMtNum( u8 byMpId, u8 byConfIdx, u8 byMtId, u16 wConfBR );    
	u8     GetMpMulticast( u8 byMpId );
    BOOL32 IsMtAssignInMp( u8 byMpId, u8 byConfIdx, u8 byMtId );

    
    /************************************************************************/
    /*                                                                      */
    /*                     8、MtAdp操作函数                                 */
    /*                                                                      */
    /************************************************************************/
	void AddMtAdp( TMtAdpReg &tMtAdpReg );
	void RemoveMtAdp( u8 byMtAdpId );
	BOOL32 IsMtAdpConnected( u8 byMtAdpId );
	u32  GetMtAdpIpAddr( u8 byMtAdpId );
	u8   GetMtAdpProtocalType( u8 byMtAdpId );
	u8   GetMtAdpSupportMtNum( u8 byMtAdpId );
    u8   GetMtAdpSupportSMcuNum( u8 byMtAdpId );
	u8   GetMtAdpNum( u8 byProtocolType = PROTOCOL_TYPE_H323 );
	u8   FindMtAdp( u32 dwIp, u8 byProtocolType = PROTOCOL_TYPE_H323 );
	void IncMtAdpMtNum( u8 byDriId, u8 byConfIdx, u8 byMtId );
	void DecMtAdpMtNum( u8 byDriId, u8 byConfIdx, u8 byMtId, BOOL32 bHDMt = FALSE, BOOL32 bOnlyAlterNum = FALSE);
	void ChangeDriRegConfNum( u8 byDriId, BOOL32 bInc  );
	u8   GetRegConfDriId( void );
    u8   AssignH323MtDriId( u8 byConfIdx, TMtAlias &tMtAlias, u8 byMtId );
	u8   AssignH320MtDriId( u8 byConfIdx, TMtAlias &tMtAlias, u8 byMtId );
	void ShowDri( u8 byDriId );
    BOOL32 IsMtAssignInDri( u8 byDriId, u8 byConfIdx, u8 byMtId );

    /************************************************************************/
    /*                                                                      */
    /*                     9、端口管理                                      */
    /*                                                                      */
    /************************************************************************/

	//接收终端数据端口管理
	u16  AssignMtPort( u8 byConfIdx, u8 byMtId );
	void ReleaseMtPort( u8 byConfIdx, u8 byMtId );
    //组播地址管理
    u32  AssignMulticastIp( u8 byConfIdx );
	//组播端口管理
	u16  AssignMulticastPort( u8 byConfIdx, u8 byMtId );
    //检查组播地址是否已经被即时会议占用
    BOOL32 IsMulticastAddrOccupied(u32 dwCastIp, u16 wCastPort) ;
	void ReleaseMulticastPort( u8 byConfIdx, u8 byMtId );    
    // 检查分散会议的组播地址是否可用
    BOOL32 IsDistrConfCastAddrOccupied( u32 dwCastIp, u16 wCastPort) ;

    /************************************************************************/
    /*                                                                      */
    /*                     10、会议存储信息管理                             */
    /*                                                                      */
    /************************************************************************/
	//会议存储信息管理(重启时需自动恢复的会议Idx列表)
	BOOL32 HasConfStore( u8 byConf ){ return m_abyConfStoreInfo[byConf]; }
	void SetConfStore( u8 byConf, BOOL32 bRestore ) { m_abyConfStoreInfo[byConf] = bRestore; }
	
	void SetConfRegState( u8 byConfID, u8 byRegState ){ m_abyConfRegState[byConfID] = byRegState; } 
	u8   GetConfRegState( u8 byConfID ){ return m_abyConfRegState[byConfID]; }
	//void ClearConfRegState( void ){ memset(m_abyConfStoreInfo, 0, sizeof(m_abyConfStoreInfo)); }
    void SetConfIdMakeTimes(u32 dwTimes) { m_dwMakeConfIdTimes = dwTimes; }

    /************************************************************************/
    /*                                                                      */
    /*                     11、GK信息管理                                   */
    /*                                                                      */
    /************************************************************************/
	void SetRegGKDriId( u8 byRegGKDriId ){ m_byRegGKDriId = byRegGKDriId; } 
	u8   GetRegGKDriId( void ){ return m_byRegGKDriId; }

	void SetH323GKIDAlias( TH323EPGKIDAlias *ptH323GKIDAlias ){ memcpy( (void*)&m_tGKID, (void*)ptH323GKIDAlias, sizeof(TH323EPGKIDAlias)); } 
	TH323EPGKIDAlias *GetH323GKIDAlias( void ){ return &m_tGKID; }
	void SetH323EPIDAlias( TH323EPGKIDAlias *ptH323EPIDAlias ){ memcpy( (void*)&m_tEPID, (void*)ptH323EPIDAlias, sizeof(TH323EPGKIDAlias)); } 
	TH323EPGKIDAlias *GetH323EPIDAlias( void ){ return &m_tEPID; }

    void   SetChargeRegOK( BOOL32 bRegOK ) { m_byChargeRegOK = bRegOK ? 1 : 0;  }
    BOOL32 GetChargeRegOK( void ) { return m_byChargeRegOK == 1 ? TRUE : FALSE;    }
    
    /************************************************************************/
    /*                                                                      */
    /*                     12、Debug调试及能力开关信息                      */
    /*                                                                      */
    /************************************************************************/   
    
	// 1. 获取基本调试信息
	void   GetBaseInfoFromDebugFile( );
	u16    GetMcuTelnetPort( );
	u16    GetMcuListenPort( );
	u32    GetMcsRefreshInterval( );
	BOOL32 IsWatchDogEnable( );
    BOOL32 GetMSDetermineType( );
	u16	   GetMsSynTime(void);	//获得主备同步时间, zgc, 2007-04-02
    BOOL32 GetBitrateScale();

	// 2. 获取关于指定终端的协议适配板资源及码流转发板资源的设置
	BOOL32 GetMtCallInterfaceInfoFromDebugFile( );
	BOOL32 GetMpIdAndH323MtDriIdFromMtAlias( TMtAlias &tMtAlias, u32 &dwMtadpIpAddr, u32 &dwMpIpAddr );
	    
    
    // 3连接级联SMcu的端口（下级的侦听端口）
    BOOL32 IsMMcuSpeaker() const;
    BOOL32 IsAutoDetectMMcuDupCall() const;
    BOOL32 IsShowMMcuMtList() const;
    u8     GetCascadeAliasType() const;
    void   SetSMcuCasPort(u16 wPort);
    u16    GetSMcuCasPort() const;

    // 4. 通用参数        
	BOOL32 IsApplyChairToZxMcu() const;
    BOOL32 IsTransmitMtShortMsg() const;
    BOOL32 IsChairDisplayMtApplyInfo() const;
    BOOL32 IsSelInDoubleMediaConf() const;
    BOOL32 IsLimitAccessByMtModal() const;
    BOOL32 IsSupportSecDSCap() const;
    BOOL32 IsSendFakeCap2Polycom() const;
	BOOL32 IsSendFakeCap2Taide() const;
    BOOL32 IsSendFakeCap2TaideHD() const;
    BOOL32 IsAllowVmpMemRepeated() const;
    BOOL32 IsDistinguishHDSDMt() const;
	BOOL32 IsVidAdjustless4Polycom() const;
    BOOL32 IsSelAccord2Adp() const;
    BOOL32 IsAdpResourceCompact() const;
    BOOL32 IsSVmpOutput1080i() const;
	BOOL32 IsConfAdpManually() const;
    u8     GetBandWidthReserved4HdBas() const;
    u8     GetBandWidthReserved4HdVmp() const;

    // 5. 能力限制部分
    BOOL32 SetLicenseNum( u16 wLicenseValue );
    u16    GetLicenseNum( void );
	BOOL32 SetVCSAccessNum( u16 wAccessNum );
	u16    GetVCSAccessNum( void );
	
    void   SetMcuExpireDate( TKdvTime &tExpireDate );
    TKdvTime GetMcuExpireDate( void );
    BOOL32 IsMcuExpiredDate(void);

    BOOL32 IsMpcRunMp();
    TMcuPerfLimit& GetPerfLimit();

    BOOL32 IsMtNumOverLicense( );         //是否超过License控制的最大终端接入能力
	BOOL32 IsVCSNumOverLicense(void);     //是否超过License控制的最大VCS接入能力
    BOOL32 IsConfNumOverCap( BOOL32 bOngoing, u8 byConfSource = MCS_CONF );           //会议数量是否超限
    u8     GetMcuCasConfNum();                            //获取级联会议总数
    BOOL32 IsCasConfOverCap();                            //级联会议是否超限
    BOOL32 GetMtNumOnDri( u8 byDriId, BOOL32 bOnline, u8 &byMtNum, u8 &byMcuNum );    //获得某个DRI MtAdp上的终端或者Mcu数量，返回是否超限
    
    // 6. 卫星会议配置获取
    void   GetSatInfoFromDebugFile();

    // 7. 打印
    void ShowDebugInfo();
    void ShowBasInfo();

    
    /************************************************************************/
    /*                                                                      */
    /*                     13、用户及用户组管理                             */
    /*                                                                      */
    /************************************************************************/
	//获取当前用户所在用户组的所有信息
	BOOL32 GetMcuCurUserList(CUsrManage& cUsrManageObj, u8 byGrpId, u8 *pbyBuf, u8 &byUsrItr, u8 &byUserNumInPack );
	BOOL32 GetMCSCurUserList(u8 byGrpId, u8 *pbyBuf, u8 &byUsrItr, u8 &byUserNumInPack );
	BOOL32 GetVCSCurUserList(u8 byGrpId, u8 *pbyBuf, u8 &byUsrItr, u8 &byUserNumInPack );

	BOOL32 ReloadMcuUserList( void );
	
    // 获取该组用户当前数量和最大值
    BOOL32  GetUsrGrpUserCount(CUsrManage& cUsrManageObj, 
							   CUsrGrpsInfo& cUsrGrpObj,u8 byGrpId, 
							   u8 &byMaxNum, u8 &byNum) ;
	BOOL32  GetMCSUsrGrpUserCount(u8 byGrpId, u8 &byMaxNum, u8 &byNum);
	BOOL32  GetVCSUsrGrpUserCount(u8 byGrpId, u8 &byMaxNum, u8 &byNum);	
	

    // 添加/删除/修改用户组（包括删除后的相应的业务逻辑）
    u8      AddUserGroup( CUsrGrpsInfo &cUsrGrpObj,  const TUsrGrpInfo &tGrpInfo, u16 &wErrorNo );
	u8      AddMCSUserGroup( const TUsrGrpInfo &tGrpInfo, u16 &wErrorNo  );
	u8      AddVCSUserGroup( const TUsrGrpInfo &tGrpInfo, u16 &wErrorNo  );

    BOOL32  ChgUserGroup( CUsrGrpsInfo &cUsrGrpObj, const TUsrGrpInfo &tGrpInfo, u16 &wErrorNo );
	BOOL32  ChgMCSUserGroup(const TUsrGrpInfo &tGrpInfo, u16 &wErrorNo );
	BOOL32  ChgVCSUserGroup(const TUsrGrpInfo &tGrpInfo, u16 &wErrorNo );
	
    BOOL32  DelMCSUserGroup( u8 byGrpId, u16 &wErrorNo );
	BOOL32  DelVCSUserGroup( u8 byGrpId, u16 &wErrorNo );

    // 获取用户组的信息
    void    GetMCSUserGroupInfo ( u8 &byNum, TUsrGrpInfo **ptInfo );
    void    GetVCSUserGroupInfo ( u8 &byNum, TUsrGrpInfo **ptInfo );

    CUsrGrpsInfo * GetUserGroupInfo( void );

    // 判断终端IP是否在某个用户组的允许IP段内
    BOOL32  IsMtIpInAllowSeg ( u8 byGrpId, u32 dwIp );

    // 打印用户组
    void    PrtUserGroup();

	// 用户分配任务管理
	// 添加、删除、修改一用户的任务信息
	BOOL  AddVCSUserTaskInfo(TUserTaskInfo& tUserTaskInfo);
	BOOL  DelVCSUserTaskInfo(s8* pachUserName);
	BOOL  ChgVCSUserTaskInfo(TUserTaskInfo& tUserTaskInfo);
	// 获取指定的用户任务信息
	BOOL  GetSpecVCSUserTaskInfo(s8* pachUserName, TUserTaskInfo& tUserTaskInfo);
	// 从文件中读取所有用户任务信息
	BOOL  LoadVCSUsersTaskInfo();
	// 判定指定会议是否为指定用户的任务范围内
	BOOL  IsYourTask(const s8* pachUserName, const CConfId& cConfId);
	// 删除指定任务
	void  DelVCSSpecTaskInfo(CConfId cConfId);


public:
	BOOL32 MsgPassCheck( u16 wEvent, u8 * const pbyMsg, u16 wLen = 0 );
	void   BroadcastToAllConf( u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
	BOOL32 SendMsgToConf( u8 byConfIdx, u16 wEvent, u8 * const pbyMsg, u16 wLen );
	BOOL32 SendMsgToConf( const CConfId & cConfId, u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
	BOOL32 SendMsgToDaemonConf( u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );

	void ClearVcData();
	CMcuVcData();
	virtual ~CMcuVcData();
	
	void EqpStatusShow( u8 byEqpId );


    /************************************************************************/
    /*                                                                      */
    /*                     14、主控热备份数据倒换                           */
    /*                                                                      */
    /************************************************************************/ 
public:	
	BOOL32 GetVcDeamonEnvState( TMSSynEnvState &tMSSynEnvState );
	BOOL32 IsEqualToVcDeamonEnvState( TMSSynEnvState *ptMSSynEnvState, BOOL32 bPrintErr, TMSSynState *ptMSSynState );
	BOOL32 GetVcDeamonPeriEqpData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen );
	BOOL32 SetVcDeamonPeriEqpData( u8 *pbyBuf, u32 dwInBufLen );
	BOOL32 GetVcDeamonMCData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen );
	BOOL32 SetVcDeamonMCData( u8 *pbyBuf, u32 dwInBufLen );
	BOOL32 GetVcDeamonMpData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen );
	BOOL32 SetVcDeamonMpData( u8 *pbyBuf, u32 dwInBufLen );
	BOOL32 GetVcDeamonMtadpData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen );
	BOOL32 SetVcDeamonMtadpData( u8 *pbyBuf, u32 dwInBufLen );
	BOOL32 GetVcDeamonTemplateData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen );
	BOOL32 SetVcDeamonTemplateData( u8 *pbyBuf, u32 dwInBufLen );
	BOOL32 GetVcDeamonOtherData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen );
	BOOL32 SetVcDeamonOtherData( u8 *pbyBuf, u32 dwInBufLen );

	BOOL32 HangupAllVcInstTimer( void );
	BOOL32 ResumeAllVcInstTimer( void );

	BOOL32 GetAllVcInstState( TMSVcInstState &tVcInstState );
	BOOL32 SetAllVcInstState( TMSVcInstState &tVcInstState );
	
	u8     FindNextConfInstIDOfNotIdle( u8 byPreInstID );
    u8     GetTakeModeOfInstId( u8 byInstId ); // guzh [4/18/2007] 获取指定会议的召开类型(ongoing / schedule)
	
	BOOL32 GetOneVcInstConfMtTableData( u8 byInsID, u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen );
	BOOL32 SetOneVcInstConfMtTableData( u8 byInsID, u8 *pbyBuf, u32 dwInBufLen );
	BOOL32 GetOneVcInstConfSwitchTableData( u8 byInsID, u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen );
	BOOL32 SetOneVcInstConfSwitchTableData( u8 byInsID, u8 *pbyBuf, u32 dwInBufLen );
	BOOL32 GetOneVcInstConfOtherMcTableData( u8 byInsID, u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen );
	BOOL32 SetOneVcInstConfOtherMcTableData( u8 byInsID, u8 *pbyBuf, u32 dwInBufLen );
	BOOL32 GetOneVcInstOtherData( u8 byInsID, u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen );
	BOOL32 SetOneVcInstOtherData( u8 byInsID, u8 *pbyBuf, u32 dwInBufLen, BOOL32 bResumeTimer );
	
	BOOL32 GetCfgFileData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen, BOOL32 bPrintErr );
	BOOL32 SetCfgFileData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bPrintErr );
	BOOL32 GetDebugFileData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen, BOOL32 bPrintErr );
	BOOL32 SetDebugFileData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bPrintErr );
	BOOL32 GetAddrbookFileData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen, BOOL32 bPrintErr );
	BOOL32 SetAddrbookFileData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bPrintErr );
	BOOL32 GetConfinfoFileData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen, BOOL32 bPrintErr );
	BOOL32 SetConfinfoFileData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bPrintErr );
	BOOL32 GetLoguserFileData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen, BOOL32 bPrintErr );
	BOOL32 SetLoguserFileData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bPrintErr );
    //用户组扩展信息
	BOOL32 GetUserExFileData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen, BOOL32 bPrintErr );
	BOOL32 SetUserExFileData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bPrintErr );

	BOOL32 GetUnProcConfinfoFileData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen);
	BOOL32 SetUnProcConfinfoFileData( u8 *pbyBuf, u32 dwInBufLen);
	BOOL32 GetVCSLoguserFileData(u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen);
	BOOL32 SetVCSLoguserFileData(u8 *pbyBuf, u32 dwInBufLen);
	BOOL32 GetVCSUserTaskFileData(u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen);
	BOOL32 SetVCSUserTaskFileData(u8 *pbyBuf, u32 dwInBufLen);
	
private: 
	u32  OprVcDeamonOtherData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bGet );
	u32  GetVcDeamonOtherDataLen( void );
    
    
    /************************************************************************/
    /*                                                                      */
    /*                     15、卫星会议支持相关配置                         */
    /*                                                                      */
    /************************************************************************/ 
public:
    //获取频率
    void GetApplyFreqInfo(u32 &dwSatIp, u16 &wSatPort);

    //频率响应端口
    u16  GetApplyRcvPort( void ) const;
    
    //是否得到频率
    void SetConfGetSendFreq( u8 byConfIdx , BOOL bTrue );
    void SetConfGetReceiveFreq( u8 byConfIdx , BOOL bTrue );
    
    BOOL IsConfGetAllFreq( u8 byConfIdx );
    BOOL IsConfGetSendFreq( u8 byConfIdx );
	BOOL IsConfGetReceiveFreq( u8 byConfIdx );

	//会议频率管理
    void SetConfBitRate( u8 byConfIdx, u32 dwSend, u32 dwRecv );
    u32  GetConfRcvBitRate( u8 byConfIdx );
    u32  GetConfSndBitRate( u8 byConfIdx );

    void SetConfFreq( u8 byConfIdx, u32 dwSend, u8 byPos,u32 dwRecv );
    u32  GetConfRcvFreq( u8 byConfIdx , u8 byPos );
    u32  GetConfSndFreq( u8 byConfIdx );
    
    void SetConfInfo( u8 byConfIdx );
    void ReleaseConfInfo( u8 byConfIdx );
    
    u8   GetConfRcvNum( u8 byConfIdx );
	void SetConfRcvNum( u8 byConfIdx , u8 byTotal );

    u16  GetRcvMtSignalPort( void ) const;
    u32  GetTimeRefreshIpAddr( void ) const;
    u16  GetTimeRefreshPort( void ) const;

    //MODEM操作函数
    void SetMcuModemConnected( u16 wModemId, BOOL bConnected );
    u8   GetIdleMcuModemForSnd();
    u8   GetIdleMcuModemForRcv();
    void ReleaseSndMcuModem( u8 byModemId );
    void ReleaseRcvMcuModem( u8 byModemId );
    
    void SetMcuModemSndData( u8 byModemId, u8 byConfIdx, u32 dwSndFreq, u32 dwSndBit );
    void SetMcuModemRcvData( u8 byModemId, u8 byConfIdx, u32 dwRevFreq, u32 dwRevBit );
    void GetMcuModemRcvData( u8 byModemId, u32 &dwRcvFreq, u32 &dwRcvBit );
	void GetMcuModemSndData( u8 byModemId, u32 &dwSndFreq, u32 &dwSndBit );

    void SetModemSportNum( u8 byModemId, u8 byNum );
	u8   GetModemSportNum( u8 byModemId );

	u8   GetConfRcvModem( u8 byConfIdx );
	u8   GetConfSndModem( u8 byConfIdx );


	void SetMtOrEqpUseMcuModem( TMt tMt, u8 byModemId, BOOL bRcv, BOOL bUsed );
	u8	 GetConfMtUsedMcuModem( u8 byConfIdx, TMt tMt, BOOL bRcv );

	u8   GetConfIdleMcuModem( u8 byConfIdx, BOOL bRcv );
	u8   ReleaseConfUsedMcuModem( u8 byConfIdx, TMt tMt, BOOL bRcv );

    void DisplayModemInfo( void );
	void ShowConfFreqInfo( void );
	void ShowSatInfo( void );

    void SetMtModemConnected( u16 wMtId, BOOL bConnected);
    BOOL IsMtModemConnected( u16 wMtId );
    void SetMtModemStatus( u16 wMtId, BOOL bError );

	u32  GetMcuMulticastDataIpAddr( void );
	u16  GetMcuMulticastDataPort( void );

	u32  GetMcuSecMulticastIpAddr( void );

	u16  GetMcuRcvMtMediaStartPort( void );
	u16  GetMtRcvMcuMediaPort( void );

	u8   GetConfRefreshTimes( u8 byConfIdx );
	void SetConfRefreshTimes( u8 byConfIdx, u8 byTimes );
	BOOL IsConfRefreshNeeded( void );

protected:
    CMcuVcInst      *m_apConfInst[MAXNUM_MCU_CONF];         //会议信息表
    TPeriEqpData     m_atPeriEqpTable[MAXNUM_MCU_PERIEQP];  //外设信息表，外设号减1为数组下标	
    TDcsInfo		 m_atPeriDcsTable[MAXNUM_MCU_DCS];		//DCS信息表, DCS id号减1为数组下标 2005-12-14
    TMcData          m_atMcTable[MAXNUM_MCU_MC + MAXNUM_MCU_VC];            //会控信息表，会控实例号减1为数组下标
    TMpData          m_atMpData[MAXNUM_DRI];                //Mp信息表
    TMtAdpData       m_atMtAdpData[MAXNUM_DRI];             //MtAdp信息表
    
    TRecvMtPort      m_atRecvMtPort[MAXNUM_MCU_MT];
    //	TIpAddrRes       m_atMulticastIp[MAXNUM_MCU_CONF];
    TRecvMtPort      m_atMulticastPort[MAXNUM_MCU_MT];
    
    u8               m_abyConfStoreInfo[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE];   //重启时需自动恢复的会议Idx列表
    
    u8               m_abyConfRegState[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];  //state of mcu/conf gk registration
    u8               m_byRegGKDriId; //多适配板时用于GK注册的板ID
    TH323EPGKIDAlias m_tGKID;        //GatekeeperID，用于其他适配板完成RRQ时的认证信息记录
    TH323EPGKIDAlias m_tEPID;        //EndpointID，用于其他适配板完成RRQ时的认证信息记录
    u8               m_byChargeRegOK;//计费链路已经建立
    
    u32              m_dwMakeConfIdTimes;   //会议号产生次数,用于MakeConfId产生会议号
    TMcuDebugVal     m_tMcuDebugVal;
    
    u32              m_dwMtCallInterfaceNum;
    TMtCallInterface m_atMtCallInterface[MAXNUM_MCU_MT];  //指定终端的协议适配板资源及码流转发板资源的设置
    
    TTemplateInfo   *m_ptTemplateInfo;                    //模板信息
    TConfMapData     m_atConfMapData[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE];
    
    CUsrGrpsInfo     m_cUsrGrpInfo;    // MCS用户组信息 
    CUsrGrpsInfo     m_cVCSUsrGrpInfo; // VCS用户组信息
    CUsersTaskInfo   m_cVCSUsrTaskInfo;// VCS用户调度席分配信息	
    CVmpMemVidLmt    m_cVmpMemVidLmt;			// mpu-vmp 各通道能力限制(主要是分辨率)
    
    u32              m_adwEqpIp[MAXNUM_MCU_PERIEQP];


    u8               m_abyMtModemConnected[MAXNUM_MCU_MT];      //终端Modem状态(448)
    u8               m_abyMtModemStatus[MAXNUM_MCU_MT];         //终端Modem状态(448)


protected:
    TMcuSatInfo      m_tSatInfo;                                 //卫星会议的相关配置信息
    TConfFreqBitRate m_atConfFreq[MAXNUM_ONGO_CONF]; 			 //会议上下星带宽记录信息
    u8				 m_abySendConfFreq[MAX_CONFIDX];			 //记录各个会议是否分配发送频率
    u8				 m_abyReceiveConfFreq[MAX_CONFIDX];			 //记录各个会议是否分配接收频率
    TModemData       m_atModemData[MAXNUM_MCU_MODEM];            //MCU侧16个MODEM状态
	u8               m_abyConfRefreshTimes[MAX_CONFIDX];		 //临时解决拨号 和 消息发送的时序问题

};

#endif

//END OF FILE





