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
#include "eqpssn.h"
#include "mcuinnerstruct.h"
#include "rpctrl.h"

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
	TTransportAddr  m_tVideoRtcpDstAddr[MAXNUM_VMP_MEMBER];   //外设信道视频RTCP目的地址
	TTransportAddr  m_tAudioRtcpDstAddr[MAXNUM_VMP_MEMBER];   //外设信道音频RTCP目的地址

	//  [5/17/2013 guodawei] 添加外设编码类型属性 GBK/UTF8
	u8 m_byEqpCodeFormat;
};

//2、会控数据
struct TMcData
{
	BOOL32	m_bConnected;					//会控是否登记为FALSE表示未建链登记
	u8		m_byFwdChannelNum;				//MCU至会控信道数
	TLogicalChannel	m_tFwdVideoChannel;		//MCU至会控起始视频信道
	TLogicalChannel	m_tFwdAudioChannel;		//MCU至会控起始音频信道
	TLogicalChannel	m_tFwdDualChannel;		//MCU至会控起始双流信道
	TMt		m_atVidSrc[MAXNUM_MC_CHANNL];	//该会控选看视频源
	TMt		m_atAudSrc[MAXNUM_MC_CHANNL];	//该会控选看音频源
	TMt		m_atSecVidSrc[MAXNUM_MC_CHANNL];//该会控选看双流源
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
    u8   m_abyMtType[MAX_CONFIDX][MAXNUM_CONF_MT];           //会议接入终端id
    u16  m_wMtNum;                          //mtadp接入终端数
	u8   m_byRegGKConfNum;                  //注册GK的会议数
    u16  m_wQ931Port;                       //主机序(用于主适配板gk注册)    
    u16  m_wRasPort;                        //主机序
	u8   m_byMaxHDMtNum;                    //适配支持的最大HD终端数目 //  [7/27/2011 chendaiwei]
//	u8   m_abyHDMt[MAXHDLIMIT_MPC_MTADP];    //HD 终端
	TMtAdpHDChnnlInfo m_atHdChnnlInfo[MAXHDLIMIT_MPC_MTADP];	//占内嵌接入板高清资源的终端信息
	u8   m_abyMacAddr[6];					//mtadp所在接入板的Mac地址 
	u16  m_wMaxAudMtNum;					//接入板最大支持语音终端接入数  zjl 20120814
};

//6、Prs通道描述
// struct TPrsChannel
// {	
// public:
// 	TPrsChannel()
// 	{
// 		memset(this, 0, sizeof(TPrsChannel));
// 		for(u8 byLp =0; byLp < MAXNUM_PRS_CHNNL; byLp++)
// 		{
// 			m_abyPrsChannels[byLp] = EQP_CHANNO_INVALID;
// 		}
// 	}
// 
// 	u8  GetPrsId(void) const
// 	{
// 		return m_byEqpPrsId;
// 	}
// 	u8  GetChannelsNum(void) const
// 	{
// 		return m_byChannelNum;
// 	}
// 	void SetPrsId(u8 byPrsId)
// 	{
// 		m_byEqpPrsId = byPrsId;
// 	}
// 
// 	void SetChannelNum(u8 byChlsNum )
// 	{
// 		m_byChannelNum = byChlsNum;
// 	}
// 
// 	u8  m_abyPrsChannels[MAXNUM_PRS_CHNNL]; // 通道号
// private:	
// 	u8  m_byEqpPrsId;          // Prs Id
// 	u8  m_byChannelNum;        // 通道数	
// }
// #ifndef WIN32
// __attribute__ ( (packed) ) 
// #endif
// ;

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
		StaticLog("m_byPrsId = %d, m_byPrsChnlId = %d, m_byPrsMode = %d\n", m_byPrsId, m_byPrsChnlId, m_byPrsMode);
		StaticLog("m_tPrsSrc = (srcid:%d, srctype:%d), m_byPrsSrcOutChnl = %d, m_byIsBrdChn:%d,m_byIsStart:%d\n\n", 
			      m_tPrsSrc.GetMtId(), m_tPrsSrc.GetMtType(), m_byPrsSrcOutChnl, m_byIsBrdChn, m_byIsStart);
	}

	//通道所属prs
	void SetPrsId(u8 byPrsId) {m_byPrsId = byPrsId;} 
	u8 GetPrsId(void){ return m_byPrsId;}

	//通道号
	void SetPrsChnId(u8 byPrsChnId){m_byPrsChnlId = byPrsChnId;}
	u8 GetPrsChnId(void){return m_byPrsChnlId;}
	
	//通道源
	void SetPrsChnSrc(TMt tPrsSrc){ m_tPrsSrc = tPrsSrc;}
	TMt  GetPrsChnSrc(void){return m_tPrsSrc;}

	//通道源输出通道号
    void SetPrsSrcOutChn(u8 byPrsSrcOutChn){m_byPrsSrcOutChnl = byPrsSrcOutChn;}
	u8   GetPrsSrcOutChn(void){return m_byPrsSrcOutChnl;}

	//通道模式(音视频双流)
	void SetPrsMode(u8 byPrsMode){m_byPrsMode = byPrsMode;}
	u8 GetPrsMode(void){return m_byPrsMode;}
    
	//通道是否用于广播
	void SetPrsChnBrd(BOOL bBrdChn ){ m_byIsBrdChn = bBrdChn;}
	BOOL IsBrdPrsChn(void){ return m_byIsBrdChn;}

	//通道是否开启
	void   SetChnStart(BOOL bIsStart){m_byIsStart = bIsStart;}
	BOOL32 IsChnStart(void){return m_byIsStart;}
private:
	u8 m_byPrsId;
	u8 m_byPrsChnlId;
	u8 m_byPrsMode;
	TMt m_tPrsSrc;
	u8  m_byPrsSrcOutChnl;
	//zjl 增加是否广播通道 + 是否开启
	u8  m_byIsBrdChn;
	u8  m_byIsStart;
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
		for (u8 byIdx = 0; byIdx <MAXNUM_CONF_PRS_CHNNL; byIdx++)
		{
			m_tPrsChnlInfo[byIdx].SetNull();
// 			m_byIsStart[byIdx] = FALSE;
		}
	}

	u8 GetAllPrsChn(u8 *pbyPrsId, u8 *pbyPrsChnId)
	{
		if (NULL == pbyPrsId || NULL == pbyPrsChnId)
		{
			return 0;
		}
		u8 byChnNum = 0;
		for (u8 byChnIdx = 0; byChnIdx < MAXNUM_CONF_PRS_CHNNL; byChnIdx++)
		{
			if (!m_tPrsChnlInfo[byChnIdx].IsNull())
			{
				pbyPrsId[byChnNum]    = m_tPrsChnlInfo[byChnIdx].GetPrsId();
				pbyPrsChnId[byChnNum] = m_tPrsChnlInfo[byChnIdx].GetPrsChnId();
				byChnNum++;
			}
		}
		return byChnNum;
	}

	BOOL32 AssignPrsChnl(u8 byPrsId, u8 byPrsChnlId, u8 byPrsMode, BOOL bBrdChn)
	{
		for (u8 byChnlId = 0; byChnlId < MAXNUM_CONF_PRS_CHNNL; byChnlId++)
		{
			if (m_tPrsChnlInfo[byChnlId].IsNull())
			{
				m_tPrsChnlInfo[byChnlId].SetPrsId(byPrsId);
				m_tPrsChnlInfo[byChnlId].SetPrsChnId(byPrsChnlId);
				m_tPrsChnlInfo[byChnlId].SetPrsMode(byPrsMode);
				m_tPrsChnlInfo[byChnlId].SetPrsChnBrd(bBrdChn);
				return TRUE;
			}
		}
		return FALSE;

	}

	BOOL32 SetPrsChnSrc(u8 byPrsId, u8 byPrsChnId, TMt tPrsSrc, u8 bySrcOutChn)
	{
		if (tPrsSrc.IsNull() || byPrsId < PRSID_MIN || byPrsId > PRSID_MAX)
		{
			return FALSE;
		}
		for (u8 byChnIdx = 0; byChnIdx < MAXNUM_CONF_PRS_CHNNL; byChnIdx++)
		{
			if (m_tPrsChnlInfo[byChnIdx].GetPrsId() == byPrsId &&
				m_tPrsChnlInfo[byChnIdx].GetPrsChnId() == byPrsChnId)
			{
				m_tPrsChnlInfo[byChnIdx].SetPrsChnSrc(tPrsSrc);
				m_tPrsChnlInfo[byChnIdx].SetPrsSrcOutChn(bySrcOutChn);
				return TRUE;
			}	
		}
		return FALSE;
	}

	u8 GetPrsChnMediaMode(u8 byPrsId, u8 byPrsChnId)
	{
		for (u8 byChnIdx = 0; byChnIdx < MAXNUM_CONF_PRS_CHNNL; byChnIdx++)
		{
			if (!m_tPrsChnlInfo[byChnIdx].IsNull() &&
				m_tPrsChnlInfo[byChnIdx].GetPrsId() == byPrsId &&
				m_tPrsChnlInfo[byChnIdx].GetPrsChnId() == byPrsChnId)
			{
				return m_tPrsChnlInfo[byChnIdx].GetPrsMode();
			}
		}
		return MODE_NONE;
	}

	//统计指定单板在当前会议中占用的通道
	u8 GetSpecPrsIdAllChnForConf(u8 byPrsId, u8 *pbyPrsChnId)
	{
		if (NULL == pbyPrsChnId)
		{
			return FALSE;
		}
		u8 byChnNum = 0;
		for (u8 byChnIdx = 0; byChnIdx < MAXNUM_CONF_PRS_CHNNL; byChnIdx++)
		{
			if (!m_tPrsChnlInfo[byChnIdx].IsNull() &&
				m_tPrsChnlInfo[byChnIdx].GetPrsId() == byPrsId)
			{
				pbyPrsChnId[byChnNum] = m_tPrsChnlInfo[byChnIdx].GetPrsChnId();
				byChnNum++;
			}
		}
		return byChnNum;
	}

	BOOL32 FindPrsChnForBrd(u8 byMediaMode, u8 &byPrsId, u8 &byPrsChnId)
	{
		if (MODE_VIDEO     != byMediaMode &&
			MODE_AUDIO	   != byMediaMode &&
			MODE_SECVIDEO  != byMediaMode)
		{
			return FALSE;
		}
		for (u8 byChnIdx = 0; byChnIdx < MAXNUM_CONF_PRS_CHNNL; byChnIdx++)
		{
			if (!m_tPrsChnlInfo[byChnIdx].IsNull() &&
				m_tPrsChnlInfo[byChnIdx].IsBrdPrsChn() &&
				m_tPrsChnlInfo[byChnIdx].GetPrsMode() == byMediaMode)
			{
				byPrsId = m_tPrsChnlInfo[byChnIdx].GetPrsId();
				byPrsChnId = m_tPrsChnlInfo[byChnIdx].GetPrsChnId();
				return TRUE;
			}
		}
		return FALSE;
	}

	BOOL32 RemovePrsChnl(u8 byPrsId, u8 byPrsChnlId)
	{
		for (u8 byChnlId = 0; byChnlId < MAXNUM_CONF_PRS_CHNNL; byChnlId++)
		{
			if (m_tPrsChnlInfo[byChnlId].GetPrsId() == byPrsId &&
				m_tPrsChnlInfo[byChnlId].GetPrsChnId() == byPrsChnlId)
			{
				m_tPrsChnlInfo[byChnlId].SetNull();
				return TRUE;
			}
		}
		return FALSE;
	}

	BOOL32 FindPrsChnlSrc(u8 byPrsId, u8 byPrsChnlId, u8& byPrsMode, TMt& tSrc, u8& bySrcOutChnl)
	{
		if (byPrsId < PRSID_MIN || byPrsId > PRSID_MAX)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[FindPrsChnlSrc]wrong prsid%d\n", byPrsId);
			return FALSE;
		}

		for (u8 byChnlId = 0; byChnlId < MAXNUM_CONF_PRS_CHNNL; byChnlId++)
		{
			if (m_tPrsChnlInfo[byChnlId].GetPrsId() == byPrsId &&
				m_tPrsChnlInfo[byChnlId].GetPrsChnId() == byPrsChnlId)
			{
				byPrsMode    = m_tPrsChnlInfo[byChnlId].GetPrsMode();
				tSrc         = m_tPrsChnlInfo[byChnlId].GetPrsChnSrc();
				bySrcOutChnl = m_tPrsChnlInfo[byChnlId].GetPrsSrcOutChn();
				return TRUE;
			}
		}
		return	FALSE;
	}

	BOOL32 FindPrsChnForSrc(const TMt& tSrc, u8 bySrcOutChnl, u8 byMode, u8& byPrsId, u8& byPrsChnlId)
	{
		for (u8 byChnlId = 0; byChnlId < MAXNUM_CONF_PRS_CHNNL; byChnlId++)
		{
			if (m_tPrsChnlInfo[byChnlId].GetPrsChnSrc() == tSrc &&
				m_tPrsChnlInfo[byChnlId].GetPrsSrcOutChn() == bySrcOutChnl &&
				m_tPrsChnlInfo[byChnlId].GetPrsMode() == byMode)
			{
				byPrsId     = m_tPrsChnlInfo[byChnlId].GetPrsId();
				byPrsChnlId = m_tPrsChnlInfo[byChnlId].GetPrsChnId();
				return TRUE;
			}
		}
		return	FALSE;
	}

	TUsedPrsChnlInfo GetPrsChnInfo(u8 byPrsIdx)
	{
		return m_tPrsChnlInfo[byPrsIdx];
	}

	BOOL32 IsPrsChnStart(u8 byPrsId, u8 byChnId)
	{
		if (byPrsId < PRSID_MIN || byPrsId > PRSID_MAX)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[IsPrsChnStart]wrong prsid%d\n", byPrsId);
			return FALSE;
		}

		for (u8 byIdx = 0; byIdx < MAXNUM_CONF_PRS_CHNNL; byIdx++)
		{
			if (m_tPrsChnlInfo[byIdx].GetPrsId() == byPrsId &&
				m_tPrsChnlInfo[byIdx].GetPrsChnId() == byChnId)
			{
				return m_tPrsChnlInfo[byIdx].IsChnStart();
			}
		}
		return FALSE;
	}

	BOOL32 SetPrsChnStart(u8 byPrsId, u8 byChnId, BOOL32 bStart)
	{
		if (byPrsId < PRSID_MIN || byPrsId > PRSID_MAX)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[SetPrsChnStart]wrong prsid%d\n", byPrsId);
			return FALSE;
		}
		for (u8 byIdx = 0; byIdx < MAXNUM_CONF_PRS_CHNNL; byIdx++)
		{
			if (m_tPrsChnlInfo[byIdx].GetPrsId() == byPrsId &&
				m_tPrsChnlInfo[byIdx].GetPrsChnId() == byChnId)
			{
				m_tPrsChnlInfo[byIdx].SetChnStart(bStart);
				return TRUE;
			}
		}
		return FALSE;
	}

	void Print()
	{
		StaticLog("Dynamic Prs Chns as follows!\n");
		for (u8 byIdx = 0; byIdx < MAXNUM_CONF_PRS_CHNNL; byIdx++)
		{
			m_tPrsChnlInfo[byIdx].Print();
		}
	}
private:
// 	u8               m_byIsStart[MAXNUM_PRS_CHNNL];
	TUsedPrsChnlInfo m_tPrsChnlInfo[MAXNUM_CONF_PRS_CHNNL];
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TMonitorData
{
	TMt		m_tSrc;				//monitor选看视频源
	TTransportAddr m_DstAddr;	//monitor的地址
	u8		m_byMode;

public:
	u8   GetMode(void) { return m_byMode; }
	void SetMode(u8 byMode){ m_byMode = byMode; }
	
	void SetDstAddr(const TTransportAddr &tDstAddr) { m_DstAddr = tDstAddr ;}
	TTransportAddr GetDstAddr(void) { return m_DstAddr; }

	TMonitorData & operator = (const TMonitorData &tMonitorData)
	{
		m_tSrc = tMonitorData.m_tSrc;
		m_DstAddr = tMonitorData.m_DstAddr;
		m_byMode = tMonitorData.m_byMode;

		return (*this);
	}
	
	BOOL32 SetMonitorSrc(const TMt &tMt)
	{
		m_tSrc = tMt;
		
		return TRUE;
	}
	
	TMt  GetMonitorSrc(void) const
	{
		return m_tSrc;
	}
	
	BOOL32 IsVacant(void)
	{
		BOOL32 bRet = FALSE;
		
		if( GetMonitorSrc().IsNull() )
		{
			bRet = TRUE;
		}
		
		return bRet;
	}
	
};

struct TMonitorMgr
{
	TMonitorData  m_atMonitorDate[MAXNUM_MONITOR_NUM];
	
public:
	
	BOOL32 AddMonitorData(TMonitorData tMonitorData);						//添加监控数据
	BOOL32 GetMonitorSrc(u8 byMode, const TTransportAddr &tTransportAddr, TMt *ptSrc);	//获取某监控的源

	BOOL32 GetMonitorData(u16 wIdx, TMonitorData &tMonitorData);			//获取某监控的数据

private:
	
	BOOL32 GetMonitorSrc(u16 wIdx, TMt *ptSrc);								//获取某监控的源
	BOOL32 SetMonitorData(u16 wIdx, TMonitorData tMonitorData);				//设置某监控的数据
	u16 GetMonitorIdx( u8 byMode, const TTransportAddr &tTransportAddr);	//获取监控索引（根据dst地址）	
	u16 AssignMonitorIdx(void);												//分配监控索引
};

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

    BOOL32 IsEmpty(void) const { return (0 == m_byConfIdx); }
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

//zjj20091102 数据模板额外信息管理(管理那些不在TConfStore结构中，需要动态增长的数据)
//现在只是针对vcs会议中的电视墙预案
class CConfInfoFileExtraDataMgr
{
protected:
	u8 *m_pPlanData;
	u8 m_byTvWallPlanNum;
	u8 m_byIsReadPlan;
	u16 m_wBufLen;
	//TConfStore m_tConfStore;

public:
	CConfInfoFileExtraDataMgr() 
	{ 
		//m_tConfStore.m_tConfInfo.Reset();
		m_pPlanData = NULL;m_byTvWallPlanNum = m_byIsReadPlan = 0;
		m_wBufLen = 0; 
	}
	~CConfInfoFileExtraDataMgr(){ Clear(); }
	void Clear()
	{
		if( m_pPlanData )
		{
			delete []m_pPlanData;	
			m_pPlanData = NULL;
		}
		m_byTvWallPlanNum = m_byIsReadPlan = 0;
		m_wBufLen = 0;
	}

	void GetAllPlanName( s8 **pPlanData,u8 &byPlanNum );

	BOOL32 GetMtPlanAliasByPlanName( s8 *pPlanName, 
							TMtVCSPlanAlias *pMtVCSPlanAlias,
							u8 &byMtAliasNum );
	
	BOOL32 CheckPlanData( void );

	BOOL32 AddPlanName( s8 *pPlanName, u8 byConfIdx, BOOL32 bIsDefaultConf = FALSE );

	BOOL32 DelPlanName( s8 *pPlanName, u8 byConfIdx, BOOL32 bIsDefaultConf = FALSE );

	BOOL32 ModifyPlanName( s8 *pPlanOldName,s8 *pPlanNewName, u8 byConfIdx,BOOL32 bIsDefaultConf = FALSE );

	BOOL32 IsHasPlanName( s8 *pPlanName );

	BOOL32 SaveMtPlanAliasByPlanName( s8 *pPlanName, 
							TMtVCSPlanAlias *pMtVCSPlanAlias,
							u8 byMtAliasNum,
							u8 byConfIdx,
							BOOL32 bIsDefaultConf = FALSE);

	// 添加参数：是否要再转为GBK，用于读MCS老版本预案 [pengguofeng 7/24/2013]
	BOOL32 ReadExtraDataFromConfInfoFile(/* TConfStore tConfStore */u8 byConfIdx, BOOL32 bIsDefaultConf = FALSE,
		BOOL32 bNeedChg2GBK = FALSE);

//	BOOL32 SaveExtraDataToConfInfoFile( TConfStore tConfStore );
	void   TransEncoding2Utf8();  // 将预案中的名称转为UTF8 [pengguofeng 7/17/2013]

	BOOL32 IsReadPlan();

	u8 GetMtPlanAliasNumByPlanName( s8 *pPlanName );

	u8 GetMtPlanNum();

	u16 GetMSData(u8 *pbyBuf);

	u16 SetMSData(u8 *pbyBuf);
	
	u16 GetMSDataLen();
	
	BOOL32 GetDefaultConfStore( TConfStore & tConfStore );

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
    s32  GetGrpNum() const
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
		StaticLog("UserName:%s\n", m_achUserName);
		StaticLog("TaskNum :%d\n", m_wTaskNum);
		for (u16 wIndex = 0; wIndex < m_wTaskNum; wIndex++)
		{
			StaticLog("Conf%d:\n", wIndex);
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

	// 打印 [pengguofeng 5/15/2013]
	void  Print(void)
	{
		for ( u8 byUserNum = 0; byUserNum < MAXNUM_VCSUSERNUM; byUserNum++)
		{
			if ( m_abyUsed[byUserNum] == 1)
			{
				m_tUserTaskInfo[byUserNum].Print();
			}
		}
	}

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

	u8	GetCurVCMTMcIdx()				  { return m_byVCMTMcIdx;}
	void SetCurVCMTMcIdx( u8 byIndex )	  { m_byVCMTMcIdx = byIndex; }
	BOOL32 GetIsTvWallOperating(){ return ( 1 == m_byIsTvWallOperating); }
	void SetIsTvWallOperating( BOOL32 bIsOperating )
	{
		if( bIsOperating )
		{
			m_byIsTvWallOperating = 1;
		}
		else
		{
			m_byIsTvWallOperating = 0;
		}
	}

	BOOL32 GetIsPlanDataOperation(){ return 1 == m_byIsPlanDataOperation; }
	void SetIsPlanDataOperation( BOOL32 bIsOperating )
	{
		if( bIsOperating )
		{
			m_byIsPlanDataOperation = 1;
		}
		else
		{
			m_byIsPlanDataOperation = 0;
		}
	}

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
		m_byIsTvWallOperating = 0;
		m_byIsPlanDataOperation = 0;
		memset( m_achLockedPlanName,0,sizeof( m_achLockedPlanName ) );
		memset( m_abyLockedVcsSsnId,0,sizeof( m_abyLockedVcsSsnId ) );
		//memset( m_abyVmpMemReplcePrio,0,sizeof( m_abyVmpMemReplcePrio ) );
	}

	BOOL32 IsPlanLockedByOtherVcs( u8 *pbyPlanName,u8 byStrLen,u8 bySsnId )
	{
		for( u8 byIdx = 0;byIdx < VCS_MAXNUM_PLAN;byIdx++ )
		{
			if( bySsnId == m_abyLockedVcsSsnId[byIdx] &&
				0 == memcmp( &m_achLockedPlanName[byIdx][0],pbyPlanName,byStrLen) &&
				strlen( &m_achLockedPlanName[byIdx][0] ) == byStrLen 
				)
			{
				return FALSE;
			}		
		}
		return TRUE;
	}

	BOOL32 LockPlan( u8 *pbyPlanName,u8 byStrLen,u8 bySsnId )
	{
		for( u8 byIdx = 0;byIdx < VCS_MAXNUM_PLAN;byIdx++ )
		{
			if( 0 == m_abyLockedVcsSsnId[byIdx] )
			{
				m_abyLockedVcsSsnId[byIdx] = bySsnId;
				memcpy( &m_achLockedPlanName[byIdx][0],pbyPlanName,byStrLen );
				m_achLockedPlanName[byIdx][byStrLen] = '\0';
				return TRUE;
			}		
		}
		return FALSE;
	}

	
	BOOL32 UnlockPlan( u8 *pbyPlanName,u8 byStrLen,u8 bySrcSsnId )
	{
		for( u8 byIdx = 0;byIdx < VCS_MAXNUM_PLAN;byIdx++ )
		{
			if( bySrcSsnId == m_abyLockedVcsSsnId[byIdx] &&
				0 == memcmp( &m_achLockedPlanName[byIdx][0],pbyPlanName,byStrLen) &&
				strlen( &m_achLockedPlanName[byIdx][0] ) == byStrLen
				)
			{
				m_abyLockedVcsSsnId[byIdx] = 0;
				memset( &m_achLockedPlanName[byIdx][0],0,VCS_MAXLEN_ALIAS );
				return TRUE;
			}		
		}
		return FALSE;
	}

	void UnlockPlan( u8 bySrcSsnId )
	{
		if( 0 == bySrcSsnId )
		{
			return;
		}
		
		for( u8 byIdx = 0;byIdx < VCS_MAXNUM_PLAN;byIdx++ )
		{
			if( bySrcSsnId == m_abyLockedVcsSsnId[byIdx] )
			{
				m_abyLockedVcsSsnId[byIdx] = 0;
				memset( &m_achLockedPlanName[byIdx][0],0,VCS_MAXLEN_ALIAS );
				//return TRUE;
			}
		}
		return;
	}

	/*void SetVmpMemberReplacePrio(u8 byVmpChnIndex,u8 byMemprio)
	{
		if ( byVmpChnIndex >= MAXNUM_VMP_MEMBER )//非法参数
		{
			return ;
		}
		//m_abyVmpMemReplcePrio[byVmpChnIndex] = byMemprio;
		
	}

	u8	GetVmpMemberReplacePrio( u8 byVmpChnIndex )
	{
		if (byVmpChnIndex >= MAXNUM_VMP_MEMBER)//非法参数
		{
			return 0;
		}
		return m_abyVmpMemReplcePrio[byVmpChnIndex];
	}*/

	void VCCPrint()
	{
		Print();
		StaticLog("m_tReqVCMT:mcuid－%d mtid－%d\n", m_tReqVCMT.GetMcuId(), m_tReqVCMT.GetMtId());
		StaticLog("m_wCurUseTWChanInd:%d\n", m_wCurUseTWChanInd);
		StaticLog("m_wCurUseVMPChanInd:%d\n", m_wCurUseVMPChanInd);
		StaticLog("m_byCurSrcSsnId:%d\n", m_byCurSrcSsnId);
		StaticLog("m_byCurVCMTMcIdx:%d\n", m_byVCMTMcIdx);
		StaticLog("m_byIsTvWallOperating:%d\n", m_byIsTvWallOperating);
		StaticLog("m_byIsPlanDataOperation:%d\n", m_byIsPlanDataOperation);
		for( u8 byIdx = 0;byIdx < VCS_MAXNUM_PLAN;byIdx++ )
		{
			if( 0 != m_abyLockedVcsSsnId[byIdx] )
			{
				StaticLog("LockedPlan(%d):%s(SrcSsnId:%d)\n",
							byIdx+1,
							&m_achLockedPlanName[byIdx][0],
							m_abyLockedVcsSsnId[byIdx]
							);
			}
		}
	}
	void SetVcsBackupChairMan(const TMt& tBackChairMan ){  m_tVCSBackChairMan =  tBackChairMan;  }
	TMt  GetVcsBackupChairMan( void ){ return m_tVCSBackChairMan; }

protected:
	u16 m_wCurUseTWChanInd;              // 自动模式下，当前使用的电视墙通道索引号
	u16 m_wCurUseVMPChanInd;             // 自动模式下，当前可使用的画面合成器起始通道号
	TMt m_tReqVCMT;                      // 当前请求调度的终端
	u8 m_byCurSrcSsnId;                  // 当前操作发起者VCS的实例ID号
	u8 m_byVCMTMcIdx;					 // 当前调度终端进入的监控通道号

	

	s8	m_achLockedPlanName[VCS_MAXNUM_PLAN][VCS_MAXLEN_ALIAS];
	u8	m_abyLockedVcsSsnId[VCS_MAXNUM_PLAN];

	//zjj20091102 是否正在对电视墙操作，包括建拆交换，置外设状态等
	u8 m_byIsTvWallOperating;//0-否 1-是

	//zjj20091102 是否正在对预案进行操作
	u8 m_byIsPlanDataOperation;//0-否 1-是

	//u8 m_abyVmpMemReplcePrio[MAXNUM_VMP_MEMBER];//zhouyiliang 20100721画面合成成员的替换优先级，0为最低，值越大越高
	TMt  m_tVCSBackChairMan;
	
};



/************************************************************************/
/*                                                                      */
/*                    六、会议适配资源管理 结构群                       */
/*                                                                      */
/************************************************************************/
/*--------------------------new bas----------------------------*/
enum BasChnType
{
  ILLEGAL_BAS_CHN =0,
  OLD_BAS_CHN,
  MPU2_BAS_CHN,
  APU2_BAS_CHN,
  X868KI_AUDBAS_CHN
};

//BAS通道基类
class CBasChn
{
public:	
	//解码能力
	enum DecAbility
	{
		emDecAud = 0,
		emDecSD,
		emDecHD,
		emDecHighFps,
		emDecH261,//mpu不支持h261
		emDecHp,
		emDecEnd,
	};

	CBasChn()
	{
		Clear();
	}

	virtual ~CBasChn()
	{

	}
	
public:
	void Clear()
	{
		m_tBas.SetNull();
		m_tSrc.SetNull();
		m_byChnId		 = 0;	
		m_byEncNum		 = 0;
		m_byDecAbility    = 0;
		m_byMediaMode	 = MODE_NONE;
		m_bIsReserved	 = FALSE;
		m_bIsOnLine		 = FALSE;
	}
	//设置外设
	void SetBas(TEqp tEqp){ m_tBas = tEqp; }
	TEqp GetBas()const{ return m_tBas; }
	
	//设置源
	void SetSrc(TMt tSrc)
	{
		m_tSrc = tSrc; 
	}
	
	TMt  GetSrc()const{ return m_tSrc; }
	
	//设置入通道
	void SetChnId(u8 byChnId){ m_byChnId = byChnId; }
	u8   GetChnId()	{ return m_byChnId; }

	//设置媒体模式
	void SetMediaMode(u8 byMediaMode){ m_byMediaMode = byMediaMode;}
	u8   GetMediaMode(){ return m_byMediaMode;}
	
	//设置解码能力
	void SetDecAbility(u8 byDecAbility){ m_byDecAbility |= (u8)1 <<byDecAbility; }
	//是否支持解码能力
	BOOL32 IsSupportDecParam(u8 byDecAbility)
	{
		u8 byTempDecAbility = m_byDecAbility;
		return byDecAbility &= byTempDecAbility; 
	}
	
	u16	GetMaxDecAbility()
	{
		return m_byDecAbility;
	}

	
	//设置状态占用
	void SetIsReserved(BOOL32 bIsReserved)
	{ 
		m_bIsReserved = bIsReserved;
		//释放占用，同时也恢复通道能力
		if ( FALSE == bIsReserved )
		{
			RestoreEncArray();
		}
	}

	BOOL32 IsReserved(){ return m_bIsReserved;}
	
	//设置在线
	void SetIsOnLine(BOOL32 bIsOnLine)
	{
		m_bIsOnLine = bIsOnLine;
	}
	BOOL32 IsOnLine(){ return m_bIsOnLine;} 
	
	//设置输出通道数
	void SetEncNum(u8 byEncNum)
	{
		if ( byEncNum >MAXNUM_BASOUTCHN )
		{
			OspPrintf(TRUE,FALSE,"[SetEncNum]byEncNum:%d is over\n",byEncNum);
			m_byEncNum = MAXNUM_BASOUTCHN;
		}
		m_byEncNum = byEncNum;
	}
	u8   GetEncNum(){ return m_byEncNum;}	

	void SendMsgToBas( u16 wEvent, CServMsg & cServMsg, const CConfId &cConId)
	{
		if( m_tBas.GetEqpId() == 0 )
		{
			LogPrint( LOG_LVL_ERROR, MID_MCU_BAS, "[SendMsgToBas] ERROR: Send Message.%d<%s> To Eqp %d not exist.\n",
					  wEvent, OspEventDesc(wEvent), m_tBas.GetEqpId() 
					 );
			return ;
		}
		else
		{
			cServMsg.SetConfId( cConId );
			cServMsg.SetConfIdx( m_tBas.GetConfIdx() );
			cServMsg.SetMcuId( LOCAL_MCUID );       
			g_cEqpSsnApp.SendMsgToPeriEqpSsn( m_tBas.GetEqpId(), wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );		 
		}	
	}

	
	//设置tick
	virtual void SetVcuTick(u32 dwTick)
	{ 
		return;
	}

	virtual u32  GetVcuTick()
	{ 
		return 0;
	}

	//BAS通道是否满足需求
	virtual BOOL32 IsBasChnSupportNeeds(TNeedVidAdaptData &tNeedBasData, TVideoStreamCap *ptOccpiedResData)
	{
		return FALSE;
	}

	virtual BOOL32 IsBasChnSupportAudNeeds(TNeedAudAdaptData &tNeedAudData, TAudioTypeDesc *ptOccpiedResData = NULL, BOOL32 bIsCheckChn = TRUE)
	{
		return FALSE;
	}
	
	//获得最大编码能力
	virtual u16	   GetMaxEncAbility()
	{
		return 0;
	}
	
	//打印
	virtual void   Print()
	{
		return;
	}

	//恢复
	virtual void   RestoreEncArray()
	{
		return;
	}
	
	//通道设置参数接口
	virtual	BOOL32	SetAdaptParam(THDAdaptParam &tAdaptParam)
	{
		return FALSE;
	}
		
	virtual BOOL32 SetAudAdaptParam(TAudAdaptParam &tAudParam)
	{
		return FALSE;
	}


	//强制给某一路输出下参
	virtual BOOL32	SetAdaptParamForcely(THDAdaptParam &tAdaptParam, u8 byOutIdx)
	{
		return FALSE;
	}

	virtual BOOL32 SetAudAdaptParamForcely(TAudAdaptParam &tAudParam, u8 byOutIdx)
	{
		return FALSE;
	}

	//开启适配
	virtual BOOL32  StartAdapt( const TBasAdaptParamExt &tBasParamExt,CConfId &cConfId)
	{
		return FALSE;
	}

	//停止适配
	virtual	BOOL32  StopAdapt(CConfId &cConfId)
	{
		return FALSE;
	}

	//某一出编码是否支持某一分辨率
	virtual BOOL32 IsSupportEncCap(const TVideoStreamCap &tSimCap, u8 byEncIdx)
	{
		return FALSE;
	}

private:
	TEqp  		m_tBas;								//外设信息
	u8    		m_byChnId;							//通道ID
	u8    	    m_byMediaMode;						//媒体类型（视频/音频）
	TMt			m_tSrc;							    //源
	u8			m_byEncNum;							//编码通道数
	u8			m_byDecAbility;						//解码能力			
	BOOL32      m_bIsReserved;						//是否占用
	BOOL32      m_bIsOnLine;						//是否在线		

};


//BAS通道具体子类
class CMpu2BasChn: public CBasChn
{
private:
	u16		m_wMaxEncAbility;		//最大编码能力
	u8		m_byEncOtherNum;		//编码other路数
	u16		m_awEncArray[MPU2_BASCHN_ENCARRAY_NUM];		//适应媒控，对VICP分段
	u32     m_adwLastVCUTick;					//tick数	
private:
	//设置编解码能力
	void	SetChnAbility(u8 byChnId, u8 byEqpType);

	//设置BAS编码总能力和分段能力
	void	SetEncAbility( u16 wEncAbility );


	//设置other编码输出路数
	void SetOtherEncNum(u8 byEncNum){ m_byEncOtherNum = byEncNum;}
	u8   GetOtherEncNum()const{ return m_byEncOtherNum;}

	//检查能力集是否能用VICP计算
	BOOL32 IsNeedVicpOccupy(const TVideoStreamCap &tCapSet);

	//计算通道剩余能力
	BOOL32  CalculateLeftEncAbility(u16 &wLeftEncAbility, u8 &wLeftEncNum, u8 &wOtherLeftEncNum);

	//按照某个VICP总和占用通道
	u16	OccupyCapByEncVal(u16 wEncVal, TNeedVidAdaptData &tNeedBasData, u8 &byLeftEncNum, u8 &byLeftEncOtherNum,u8 *abyRecord);


public:
	CMpu2BasChn(const TEqp &tEqp, u8 byChnId, u8 byEqpType )
	{
		m_adwLastVCUTick = 0;
		SetBas(tEqp);
		SetChnId(byChnId);
		SetChnAbility(byChnId,byEqpType);
	}

	//该构造专门给N+1备份使用
	CMpu2BasChn(){	m_adwLastVCUTick = 0;}
	void SetMpu2BasAttr(u8 byChnId, u8 byChnType) 
	{
		SetChnAbility(byChnId,byChnType);
	}

	~CMpu2BasChn()
	{

	}
	
	void SetVcuTick(u32 dwTick)
	{
		m_adwLastVCUTick = dwTick;
	}
	u32  GetVcuTick()
	{ 
		return m_adwLastVCUTick;
	}

	//是否支持编解码
	BOOL32 IsBasChnSupportNeeds(TNeedVidAdaptData &tNeedBasData, TVideoStreamCap *ptOccpiedResData);
	//获取最大编码能力
	u16	  GetMaxEncAbility();
	//打印
	void Print();	

	//通道普通刷参
	BOOL32	SetAdaptParam(THDAdaptParam &tAdaptParam);
	//给通道某一出强行刷参
	BOOL32	SetAdaptParamForcely(THDAdaptParam &tAdaptParam, u8 byOutIdx);
	//通道开启适配
	BOOL32  StartAdapt(const TBasAdaptParamExt &tBasParamExt,CConfId &cConfId);
	//通道停止适配
	BOOL32  StopAdapt(CConfId &cConfId);
	//恢复BAS 分段能力
	void RestoreEncArray();
	//是否支持某一路编码
	BOOL32 IsSupportEncCap(const TSimCapSet &tSimCap, u8 byEncIdx)
	{
		return TRUE;
	}

	//是否支持音频能力
	BOOL32 IsBasChnSupportAudNeeds(TNeedAudAdaptData &tNeedAudData, TAudioTypeDesc *ptOccpiedResData= NULL, BOOL32 bIsCheckChn = TRUE)
	{
		return FALSE;
	}

	BOOL32 SetAudAdaptParam(TAudAdaptParam &tAudParam)
	{
		return FALSE;
	}

	BOOL32 SetAudAdaptParamForcely(TAudAdaptParam &tAudParam, u8 byOutIdx)
	{
		return FALSE;
	}
};

class COldBasChn:public CBasChn
{
private:
	//编码能力
	enum EncAbility
	{
			emAudType  = 0,
		//	emH263plus,
		//	emDSCif,
		//	emDS4Cif,
		//	emDS720p,
			emXGA,
			emOther,			
			emSXGA,			
			emCif,	
			em4Cif,				
			emHD720p,
			emHD720pHighFps,
			emHD720pHighProfile,
			emHD1080p ,		
			emHD1080pHighFps,	
			emHD1080pHighProfile,
	};
	u16 m_awEncAbility[MAXNUM_BASOUTCHN];
	
	enum ChnEqpType
	{
		em8KHType  = 0,
		em8KGType   = 1,
		emVpuAudType = 2,
		em8KIType = 3,
		emOtherType = 4,
	};

	u8	m_byEqpType;		//标注通道的外设类型，避免一些宏定义判断

	u32     m_adwLastVCUTick;					//tick数
private:
	//通道构造
	void SetChnAbility(u8 byEqpType);
	void SetChnAbilityByChnTpye(u8 byChnType);
	void SetEncAbility(u8 byEncIdx, u16 wEncAbility);

	BOOL32 ChangeCapToEncAbility(const TVideoStreamCap *aptSimCapSet, u8 byCapSetNum, u16 &wEncAbility,u8 *pbyEncNum, u8 *pbyH264EncIdx);

	BOOL32 ChangeEncAbilityToCap(u16 wEncAbility,  u8 *pbyEncIdx, u8 *pbyEncNum,u8 byOccupiedOtherNum, TNeedVidAdaptData &tNeedBasData);

	BOOL32 GetOtherCap(TVideoStreamCap *patSimCap, u8 byNum, TVideoStreamCap *patOtherCap, u8 byOtherNum);

	//是否支持编码能力
	BOOL32 IsSupportEncParam(u8 byEncIdx, u16 wEncAbility)
	{
		if (byEncIdx >= MAXNUM_BASOUTCHN)
		{
			OspPrintf(TRUE, FALSE, "[IsSupportEncParam] byEncIdx:%d is unexpected!\n",byEncIdx);
			return FALSE;
		}
		
		if (wEncAbility > emHD1080pHighProfile)
		{
			OspPrintf(TRUE, FALSE, "[IsSupportEncParam] wEncAbility:%d is Wrong!\n",wEncAbility);
			return FALSE;
		}
		
		u16 wTempEncAbility = m_awEncAbility[byEncIdx];
		return wTempEncAbility &= 1 << wEncAbility;
	}
public:
	COldBasChn(const TEqp &tEqp, u8 byChnId, TPeriEqpStatus &tBasStatus);

	//该构造专门给N+1备份使用
	COldBasChn(){	m_adwLastVCUTick = 0; };
	void NPlusSetAttr(u8 byChnType, BOOL32 bIsHDBas,BOOL32 bIsVPU = FALSE);	

	void SetVcuTick(u32 dwTick)
	{
		m_adwLastVCUTick = dwTick;
	}
	u32  GetVcuTick()
	{ 
		return m_adwLastVCUTick;
	}

	//是否支持编解码
	BOOL32 IsBasChnSupportNeeds(TNeedVidAdaptData &tNeedBasData, TVideoStreamCap *ptOccpiedResData);
	//获取最大编码能力(排序用)
	u16  GetMaxEncAbility();
	//打印
	void Print();

	//通道普通刷参
	BOOL32	SetAdaptParam(THDAdaptParam &tAdaptParam);
	//通道某一出强行刷参
	BOOL32	SetAdaptParamForcely(THDAdaptParam &tAdaptParam, u8 byOutIdx);
	//通道开启适配
	BOOL32  StartAdapt(const TBasAdaptParamExt &tBasParamExt,CConfId &cConfId);
	//通道停止适配
	BOOL32  StopAdapt(CConfId &cConfId);
	//恢复
	void   RestoreEncArray()
	{
		return;
	}
	//是否支持某一路编码
	BOOL32 IsSupportEncCap(const TVideoStreamCap &tSimCap, u8 byEncIdx);

	//是否支持音频能力
	BOOL32 IsBasChnSupportAudNeeds(TNeedAudAdaptData &tNeedAudData, TAudioTypeDesc *ptOccpiedResData= NULL, BOOL32 bIsCheckChn = TRUE);

	BOOL32 SetAudAdaptParam(TAudAdaptParam &tAudParam);

	BOOL32 SetAudAdaptParamForcely(TAudAdaptParam &tAudParam, u8 byOutIdx);
};


enum emAudAbility
{
	emMp3=0,
	emG722,
	emG711ULAW,
	emG711ALAW,
	emG729,
	emG728,
	emG7221POLYCOM,
	emG719,
	emAACLC,
	emAACLD,
	emAACLCDouble,
	emAACLDDouble,
	emHigh,
};



class CAudBasChn:public CBasChn
{
protected:
	u32 m_dwAudDecAbility;
	u32 m_dwAudEncAbility;
protected:
	//设置解码能力
	void SetDecAbility(u8 byDecAbility);
	//设置编码能力
	void SetEncAbility(u8 byEncAbility);
	//是否支持所需能力
	BOOL32 IsSupportDecAndEnc(u8 *pabyAbility, u8 byNum);
	//将能力改成枚举描述
	void ChangeAudCapToAbility(const TAudioTypeDesc *aptAudTypeDesc, u8 &byRealNum, u8 *pabyAbility);
	//获得剩余的编码输出路数
	u8 GetLeftEncNum();

public:

	CAudBasChn(){}
	CAudBasChn(const TEqp &tEqp, u8 byChnId)
	{
		SetBas(tEqp);
		SetChnId(byChnId);
		//SetChnAbility();
	}

	//该构造专门给N+1备份使用
	
	/*void SetApu2BasAttr() 
	{
		SetChnAbility();
	}*/

	~CAudBasChn(){};
public:
    //判断通道是否支持适配
    BOOL32 IsBasChnSupportAudNeeds(TNeedAudAdaptData &tNeedBasData, TAudioTypeDesc *ptOccpiedResData= NULL, BOOL32 bIsCheckChn = TRUE);
	
    //给通道下参
    BOOL32 SetAudAdaptParam(TAudAdaptParam  &tAdaptParam);
	
    //给通道某一路强行下参
    BOOL32 SetAudAdaptParamForcely(TAudAdaptParam &tAudParam, u8 byOutIdx);
	
    //通道开启适配
    BOOL32  StartAdapt(const TBasAdaptParamExt &tBasParamExt,CConfId &cConfId);
    
    //通道停止适配
    BOOL32  StopAdapt(CConfId &cConfId);

	//打印
	virtual void Print() = 0;
};


class CApu2BasChn : public CAudBasChn
{
protected:
	void SetChnAbility();
public:
	CApu2BasChn(const TEqp &tEqp, u8 byChnId) : CAudBasChn(tEqp,byChnId)
	{		
		SetChnAbility();
	}

	//该构造专门给N+1备份使用
	CApu2BasChn() : CAudBasChn(){}
	void SetApu2BasAttr() 
	{
		SetChnAbility();
	}
	~CApu2BasChn(){};

	void Print();

};

class C8KIAudBasChn : public CAudBasChn
{
protected:
	void SetChnAbility();
public:
	C8KIAudBasChn(const TEqp &tEqp, u8 byChnId) : CAudBasChn(tEqp,byChnId)
	{		
		SetChnAbility();
	}

	//该构造专门给N+1备份使用
	C8KIAudBasChn() : CAudBasChn(){}
	void Set8KIAudBasAttr() 
	{
		SetChnAbility();
	}
	~C8KIAudBasChn(){};

	void Print();

};


//适配通道结点
struct CBasChnNode
{  
	CBasChnNode()
	{
		m_ptData = NULL;
		m_ptLast = NULL;
		m_ptNext = NULL;
	}
public:
	void SetBasChnData(CBasChn *ptBasChnData){ m_ptData = ptBasChnData;}
	CBasChn *GetBasChnData(){ return m_ptData;}
	
	void SetLast(CBasChnNode *ptBasChnNode){ m_ptLast = ptBasChnNode;}
	CBasChnNode *GetLast(){ return m_ptLast;}
	
	void SetNext(CBasChnNode *ptBasChnNode){ m_ptNext = ptBasChnNode;}
	CBasChnNode *GetNext(){ return m_ptNext;}
	
	void Print()
	{
		if(GetBasChnData())
		{
			GetBasChnData()->Print();
		}		
	}
private:
	CBasChn  *m_ptData;						 //通道信息
	CBasChnNode  *m_ptLast;                      //前一个节点
    CBasChnNode  *m_ptNext;						 //下一个节点
};

//适配通道状态
enum emBASCHNSTATUS
{
	BASCHN_STATE_INVALIED  = 0,
    BASCHN_STATE_READY     = 1,
	BASCHN_STATE_WAITSTART = 2,         
	BASCHN_STATE_RUNNING   = 3
} ;  


struct TVideoCommonAttr
{
    u16     m_wMaxBitRate;  //包含了HP和码率
    u8      m_byResolution; 
    u8      m_byFrameRate; 
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

struct TBasChnCap
{
private:
    u8                           m_byMediaType;      //媒体类型
    union UCommonCap
    {
        TVideoCommonAttr		 m_tVidCap;          //视频通用属性
        u8                       m_byAudioTrackNum;  //音频通道数
    }m_tCommonCap;
public:
    TBasChnCap()
	{
		Clear();
	}
    ~TBasChnCap()
	{
		Clear();
	}

    void Clear()
	{
		m_byMediaType = MEDIA_TYPE_NULL;
	}
	
	BOOL32 IsNull() const
	{
		if ( m_byMediaType == MEDIA_TYPE_NULL )
		{
			return TRUE;
		}

		return FALSE;
	}

    //设置视频能力
    void SetVidepCapSet(const TVideoStreamCap &tVidCap)
	{
		if ( tVidCap.GetMediaType() != MEDIA_TYPE_NULL &&
			tVidCap.GetMediaType() != 0
			)
		{
			m_byMediaType = tVidCap.GetMediaType();
			m_tCommonCap.m_tVidCap.m_byFrameRate = tVidCap.GetUserDefFrameRate();
			m_tCommonCap.m_tVidCap.m_byResolution = tVidCap.GetResolution();
			u16 wTempBitRate = tVidCap.GetMaxBitRate();
			if ( tVidCap.IsSupportHP() )
			{
				wTempBitRate |= 0x8000;
			}
			else
			{
				wTempBitRate &=0x7FFF;
			}
			m_tCommonCap.m_tVidCap.m_wMaxBitRate = wTempBitRate;
		}
		return;
	}

    BOOL32 GetVideoCapSet(TVideoStreamCap &tVidCap)
	{
		if ( m_byMediaType == MEDIA_TYPE_NULL || m_byMediaType == 0 )
		{
			return FALSE;
		}
		
		tVidCap.SetMediaType( m_byMediaType );
		tVidCap.SetUserDefFrameRate( m_tCommonCap.m_tVidCap.m_byFrameRate );
		tVidCap.SetResolution( m_tCommonCap.m_tVidCap.m_byResolution );
		tVidCap.SetMaxBitRate( m_tCommonCap.m_tVidCap.m_wMaxBitRate );

		if ( m_tCommonCap.m_tVidCap.m_wMaxBitRate & 0x8000 )
		{
			tVidCap.SetH264ProfileAttrb( emHpAttrb );
		}
		else
		{
			tVidCap.SetH264ProfileAttrb( emBpAttrb );
		}
		
		return TRUE;
	}
	
    //设置音频能力
    void SetAudCapSet(const TAudioTypeDesc &tAudCap)
	{
		if ( tAudCap.GetAudioMediaType() != MEDIA_TYPE_NULL )
		{
			m_byMediaType = tAudCap.GetAudioMediaType();
			m_tCommonCap.m_byAudioTrackNum = tAudCap.GetAudioTrackNum();
		}

		return;
	}

    BOOL32 GetAudCapSet(TAudioTypeDesc &tAudCap)
	{
		if ( m_byMediaType == MEDIA_TYPE_NULL )
		{
			return FALSE;
		}

		tAudCap.SetAudioMediaType( m_byMediaType );
		tAudCap.SetAudioTrackNum( m_tCommonCap.m_byAudioTrackNum );
		
		return TRUE;
	}

	//获得媒体类型
	u8 GetMediaType() const
	{
		return m_byMediaType;
	}

	//获得视频分辨率
	u8 GetVideoResolution() const
	{
		return m_tCommonCap.m_tVidCap.m_byResolution;
	}

	//获得视频帧率
	u8 GetFrameRate() const
	{
		return m_tCommonCap.m_tVidCap.m_byFrameRate;
	}

	//获得视频码率
	u16		GetVideoMaxBitRate() const 
	{ 
		u16 wTmpBitRate = m_tCommonCap.m_tVidCap.m_wMaxBitRate;
		wTmpBitRate = wTmpBitRate & 0x7FFF;
		return wTmpBitRate; 
	}

	//获得视频profiletype
	emProfileAttrb GetVideoProfileType ( void ) const
	{
		u16 wTmpBitRate = m_tCommonCap.m_tVidCap.m_wMaxBitRate;
		if(wTmpBitRate & 0x8000)
		{
			return emHpAttrb;
		}
		else
		{
			return emBpAttrb;
		}
	}

	//获得音频声道数
	u8 GetTrackNum() const
	{
		return m_tCommonCap.m_byAudioTrackNum;
	}
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;


//单个通道对应的编码能力集结构
struct TBasChnCapData
{
	TBasChnCap	atBasChnCap[MAX_CONF_BAS_ADAPT_NUM];
};



//BAS通道链表管理类
struct CBasChnListMgr
{
public:
    CBasChnListMgr(void)
	{
		m_pHeadNode = NULL;
	}
	
	~CBasChnListMgr(void)
	{
		m_pHeadNode = NULL;
	}

public:
	//增加BAS通道
	BOOL32  InsertBasChn(const TEqp &tEqp, u8 byChnId);		
	//通道是否存在
	BOOL32  IsNodeExist(const TEqp &tEqp, u8 byChnId, BOOL32 bIsCanSetOnlineState = FALSE );	
	//设置在线
	void    SetBasOnLine(u8 byEqpId, BOOL32 bOnLine);	
	//为广播查找适合BAS通道
    BOOL32  CheckBasEnoughForReq(TNeedVidAdaptData *ptReqResData,TNeedAudAdaptData *pReqAudResource, CBasChn **ptConfNeedChns,  TBasChnCapData*ptBasChnCapData, u16 &wErrorCode); 
	//为选看查找适合的BAS通道
	BOOL32  GetSuitableBasChnForVidSel(TNeedVidAdaptData &tNeedBasData, CBasChn **ppcBasChnData,  TBasChnCapData *ptBasChnCapData);
	BOOL32  GetSuitableBasChnForAudSel(TNeedAudAdaptData &tNeedBasData, CBasChn **ppcBasChnData);
	//占用结点
	BOOL32  OcuppyBasChn(u8 byEqpId, u8 byChnId);			
	//释放结点
	BOOL32  ReleaseBasChn(u8 byEqpId, u8 byChnId);	
	//获得某个通道在该通道所在外设上，前面通道的输出总数
	BOOL32  GetBasChnFrontOutPutNum(const TEqp &tEqp, u8 byChnId, u8 &byOutNum,  u8 &byFrontOutNum);
	
	//主备时获得BAS数据信息
	u32 GetBasListBuf(u8 *pbyBuf);
	u32 SetBasListBuf(u8 *pbyBuf);	

	CBasChn* GetBasChnAddr(const TEqp &tEqp, u8 byChnId);
	//打印
	void	Print();
private:
	//对BAS通道进行排序
	BOOL32  AddBasChnBySort(CBasChn *pcBasChn);

	//获得视频广播BAS通道
	BOOL32  GetBasChnsForVidBrd(TNeedVidAdaptData *ptReqResData, CBasChn **ptBasConfNeed,  TBasChnCapData *ptBasChnCapData, u8 &byRetNum);

	CBasChn*  GetOneIdleSuitableBasChn(TNeedVidAdaptData &tLeftReqResData,TVideoStreamCap *ptOccpiedResData, u8 byResDataNum);

	//获得音频广播BAS通道
	BOOL32 GetBasChnsForAudBrd(TNeedAudAdaptData *ptReqAudData, CBasChn **ptBasConfNeed, TBasChnCapData *ptBasChnCapData, u8 &byRetNum);

	CBasChn* GetOneIdleSuitableAudBasChn(TNeedAudAdaptData &tLeftReqResData, TAudioTypeDesc *ptOccpiedResData);

protected:
	CBasChnNode  *m_pHeadNode;										
};


class CConfBasChnData
{
private:
	CBasChn		*m_pcBasChn;			//通道指针
	TBasChnCapData	m_tBasChnCapData;		//通道参数
	CConfId		m_cConfId;
	//[2012/03/29 nizhijun]BAS主备使用
	TEqp		m_tBas;				
	u8			m_byChnId;			
public:
	CConfBasChnData()
	{
		m_pcBasChn = NULL;
	}

	CConfBasChnData(CBasChn * const pBasChn, const  TBasChnCapData&tBasChnIdxParam, CConfId cConfId)
	{
		m_pcBasChn = pBasChn;
		m_cConfId = cConfId;
		m_tBas = pBasChn->GetBas();
		m_byChnId = pBasChn->GetChnId();
		SetBasChnCapParam(tBasChnIdxParam);
	}
	
	void Clear()
	{
		m_pcBasChn = NULL;
	}

	CBasChn * GetBasChn()
	{
		return m_pcBasChn;
	}
	
	CConfId	GetConfId()
	{
		return m_cConfId;
	}
	

	void SetBasChnCapParam(const  TBasChnCapData &tBasChnParam)
	{
		memcpy( &m_tBasChnCapData,&tBasChnParam,sizeof(TBasChnCapData) );
	}


	TBasChnCapData GetBasChnCapParam()
	{
		return m_tBasChnCapData;
	}


	BOOL32 IsBasChnExist(const TEqp &tEqp, u8 byChnId)
	{
		if ( NULL == m_pcBasChn )
		{
			return FALSE;
		}
		
		if ( m_pcBasChn->GetBas() == tEqp &&
			 m_pcBasChn->GetChnId() == byChnId	 
		   )
		{
			return TRUE;
		}

		return FALSE;
	}

	BOOL32	StartAdapt(const TBasAdaptParamExt &tBasParamExt)
	{
		if ( NULL == m_pcBasChn   )
		{
			return FALSE;
		}
		
		if ( !m_pcBasChn->StartAdapt(tBasParamExt,m_cConfId) )
		{
			return FALSE;
		}
		return TRUE;
	}


	BOOL32	StopAdapt()
	{
		if ( NULL == m_pcBasChn   )
		{
			return FALSE;
		}
		
		return m_pcBasChn->StopAdapt(m_cConfId);
	}

	void ResignBasChnAddr();
};

struct TBasOutInfo
{
	TEqp	m_tBasEqp;
	u8		m_byChnId;
	u8		m_byOutIdx;
	u8		m_byOutNum;
	u8		m_byFrontOutNum;
	TBasOutInfo()
	{
		clear();
	}
	
	void clear()
	{
		m_tBasEqp.SetNull();
		m_byChnId = 0;
		m_byOutIdx = 0;
		m_byOutNum = 0;
		m_byFrontOutNum = 0;
	}
};


//适配通道组基础结构
struct TMcuBasChnGrp
{
public:
	//1.通用接口：

	//组内是否存在该通道
	BOOL32 IsChnExist(const TEqp &tEqp, u8 byChnId, u8 &byArrayIdx);
	//组内增加适配通道
	BOOL32 AddChn(CBasChn * const pcBasChn, const  TBasChnCapData &tBasChnIdxParam, const CConfId &cConId);
	//组内移除适配通道
	BOOL32 ReMoveChn(const TEqp &tEqp, u8 byChnId);
	//获取匹配模式的适配通道
	BOOL32 GetChn(u8 &byNum, CBasChn **pcChn, u8 byMediaMode);
	//获取所有适配通道
	BOOL32 GetAllChn(u8 &byNum, CBasChn **pcBasChn);
	
	//设置通道模式
	BOOL32 SetChnMode(const TEqp &tEqp, u8 byChnId, u8 byMediaMode, u8 byIdx);
	//获取通道模式
	u8	   GetChnMode(const TEqp &tEqp, u8 byChnId, u8 byIdx);
	
	//设置通道源信息
	BOOL32 SetChnSrc(const TEqp &tEqp, u8 byChnId, TMt tSrc, u8 byIdx);
	//获取通道源信息
	TMt    GetChnSrc(const TEqp &tEqp, u8 byChnId, u8 byIdx);
	
	//设置tick
	BOOL32 SetVcuTick(const TEqp &tEqp, u8 byChnId, u32 dwTick, u8 byIdx);
	//获取tick
	u32    GetVcuTick(const TEqp &tEqp, u8 byChnId, u8 byIdx);
	
	//打印
	void   Print();

	//获得对应通道能力集
	BOOL32	GetBasResDataByEqp(TEqp tDiscBas, TNeedVidAdaptData *patBasResData);
	BOOL32	GetBasResAudDataByEqp(TEqp tDiscBas, TNeedAudAdaptData *patBasResData);

	//广播适配：设置广播适配参数
	//BOOL32 SetOutPutParam(const TConfInfo& tConfInfo, THDAdaptParam tParam, u8 byMediaMode);
	//广播适配：清空广播适配参数
	BOOL32 ClearAdaptParam(u8 byMediaMode);
	//对某一个通道的某一出强制刷参
	BOOL32	RefreshBasParambyOut(TBasOutInfo &tBasOutInfo, THDAdaptParam &tBasParam);

	//广播专用
	//对广播通道进行特殊刷参
	BOOL32 RefreshSpecialVidCap(TConfInfo &tConfInfo, TVideoStreamCap &tSimCapSet, u8 byMediaMode);
	BOOL32 RefreshSpecialAudCap(TConfInfo &tConfInfo, TConfInfoEx &tConfInfoEx,TAudioTypeDesc &tAudSrcCap);
	//广播通道参数刷新
	BOOL32 RefreshAllChnsParam(TConfInfo &tConfInfo, u8 byMediaMode,u16 wBiteRate);
	//寻找广播可用通道
	BOOL32	FindBasChnForVidBrd(const TVideoStreamCap &tSimCapSet,TBasOutInfo &tOutInfo,u8 byMediaMode,BOOL32 bIsExactMatch = FALSE);
	BOOL32  FindBasChnForAudBrd( const TAudioTypeDesc &tAudCap, TBasOutInfo &tOutInfo );

	//选看专用
	//刷新选看通道参数
	//开启选看通道适配
	BOOL32	StartAdapt(const TEqp &tEqp, u8 byChnId,TBasAdaptParamExt &tBasParamExt, u8 byIdx);
	//开启选看通道适配
	BOOL32	StopAdapt(const TEqp &tEqp, u8 byChnId,BOOL32 bIsSelChn = FALSE);
	//寻找选看可用通道
	BOOL32	FindBasChnForVidSel(TMt &tSrc,const TVideoStreamCap &tSimCapSet,TBasOutInfo &tOutInfo,u8 byMediaMode);
	BOOL32  FindBasChnForAudSel(TMt &tSrc,const TAudioTypeDesc &tAudCap,TBasOutInfo &tOutInfo);
	//寻找广播可用通道
	BOOL32	SetSelChnParam(const TEqp &tEqp, u8 byChnId, u8 byMediaMode, THDAdaptParam &tBasParam);
	BOOL32	SetSelAudChnParam(const TEqp &tEqp, u8 byChnId, TAudAdaptParam &tBasParam);
	//添加参数记录
	BOOL32 AddBasSelChnParam(const TEqp &tEqp, u8 byChnId, const TBasChnCapData &tBasChnIdxParam);
	
	//主备使用
	void ResignBasChn();

protected:
	CConfBasChnData     m_atConfBasChn[MAXNUM_PERIEQP];//通道组

};



//保存单个会议bas管理信息
class CMcuBasMgr
{
public:
	CMcuBasMgr(){ Clear();}
	~CMcuBasMgr(){Clear();};
public:
	void   Clear(){ memset(this, 0, sizeof(CMcuBasMgr));} 


	/*----------------广播适配---------------*/
	//广播适配： 适配资源加入适配组
	BOOL32 AssignBrdChn(CBasChn **aptBasChn, TBasChnCapData *patBasChnCapData, u8 byChnNum, const CConfId &cConId);	
	//广播适配： 适配通道从广播组移除
	BOOL32 ReMoveBrdChn(const TEqp &tEqp, u8 byChnId);
	//广播适配： 设置广播适配参数
	//BOOL32 SetBrdAdaptParam(const TConfInfo& tConfInfo, THDAdaptParam tParam, u8 byMediaMode);
	//广播适配： 清空广播适配参数
	BOOL32 ClearBrdAdaptParam(u8 byMediaMode);	
	//广播适配： 获取对应模式的通道
	BOOL32 GetBrdChnGrp(u8 &byNum, CBasChn **pcBasChn, u8 byMediaMode);	
	//广播适配： 获取所有广播组通道
	BOOL32 GetAllBrdChn(u8 &byNum, CBasChn **pcBasChn);	
	//广播适配： 获取广播适配占用的不同适配器信息
	BOOL32 GetDiffBrdEqp(u8 byMediaMode, u8 &byEqpNum, TEqp *ptBas);
	//广播适配：广播刷特殊参数
	BOOL32	RefreshSpecialCapForVidBrd(TConfInfo &tConfInfo,TVideoStreamCap &tSimCapSet, u8 byMediaMode);
	BOOL32  RefreshSpecialCapForAudBrd(TConfInfo &tConfInfo, TConfInfoEx &tConfInfoEx,TAudioTypeDesc &tAudSrcCap);
	//广播适配：广播刷参
	BOOL32	RefreshBrdParam(TConfInfo &tConfInfo, u8 byMediaMode,u16 wBiteRate);
	//查找可用广播通道
	BOOL32 FindBasChnByCapForVidBrd(const TVideoStreamCap &tSimCapSet, u8 byMediaMode, TBasOutInfo &tBasChnInfo, BOOL32 bIsExactMatch = FALSE);
	BOOL32 FindBasChnByCapForAudBrd(const TAudioTypeDesc &tAudCap, TBasOutInfo &tBasChnInfo);


	/*--------------选看适配-----------------*/
	//选看适配：适配资源加入选看组
	BOOL32 AssignSelChn(CBasChn *pcBasChn, const TBasChnCapData &tBasChnIdxParam,const CConfId &cConId);
	//选看适配：适配资源从选看组删除
	BOOL32 ReMoveSelChn(const TEqp &tEqp, u8 byChnId);
	//选看适配：添加参数
	BOOL32 AddBasSelChnParam(const TEqp &tEqp, u8 byChnId, const  TBasChnCapData &tBasChnIdxParam);
	//选看适配：获取对应模式的通道
	BOOL32 GetSelChnGrp(u8 &byNum, CBasChn **pcBasChn, u8 byMediaMode);
	//选看适配：查找可用选看通道
	BOOL32 FindBasChnByVidCapForSel(TMt &tSrc,const TVideoStreamCap &tSimCapSet, u8 byMediaMode, TBasOutInfo &tBasChnInfo);
	BOOL32 FindBasChnByAudCapForSel(TMt &tSrc,const TAudioTypeDesc &tAudCap, TBasOutInfo &tBasChnInfo);
	//选看适配：下参
	BOOL32 SetSelChnParam(const TEqp &tEqp, u8 byChnId, u8 byMediaMode, THDAdaptParam &tBasParam);
	BOOL32 SetSelAudChnParam(const TEqp &tEqp, u8 byChnId, TAudAdaptParam &tBasParam);
	//选看适配： 获取所有选看组通道
	BOOL32 GetAllSelChn(u8 &byNum, CBasChn **pcBasChn);	

	/*--------------公共接口------------------*/
	//获取对应通道所属组
	u8     GetGrpType(const TEqp &tEqp, u8 byChnId);	
	//设置对应通道模式
	BOOL32 SetChnMode(const TEqp &tEqp, u8 byChnId, u8 byMediaMode);
	//获取对应通道模式
	u8     GetChnMode(const TEqp &tEqp, u8 byChnId);	
	//设置通道源信息
	BOOL32 SetChnSrc(const TEqp &tEqp, u8 byChnId, TMt tSrc);
	//获取通道源信息
	TMt    GetChnSrc(const TEqp &tEqp, u8 byChnId);	
	//设置tick
	BOOL32 SetVcuTick(const TEqp &tEqp, u8 byChnId, u32 dwTick);
	//获取tick
	u32	   GetVcuTick(const TEqp &tEqp, u8 byChnId);	
	//获取指定通道的索引(用于定时)
	u8     GetPosition(const TEqp &tEqp, u8 byChnId);	
	//打印
	void   Print();
	//刷新通道某一出参数，帧率与码率刷新
	BOOL32	RefreshBasParambyOut(TBasOutInfo &tBasOutInfo, THDAdaptParam &tBasParam);
	//开启某个通道适配
	BOOL32	StartBasAdapt(const TEqp &tEqp,u8 byChnId, TBasAdaptParamExt &tBasParamExt);
	//停止某个通道适配
	BOOL32	StopBasAdapt(const TEqp &tEqp, u8 byChnId);	
	//获得对应通道能力集
	BOOL32	GetBasResDataByEqp(TEqp tDiscBas, TNeedVidAdaptData *patBasResData);
	BOOL32	GetBasResAudDataByEqp(TEqp tDiscBas, TNeedAudAdaptData *patBasResData);

	//主备使用
	void ResignBasChn();
private:
	TMcuBasChnGrp	  m_tBrdChn;          //单会议广播适配组
	TMcuBasChnGrp     m_tSelChn;          //单会议选看适配组
};
/*------------------------------------------------------------------*/

/************************************************************************/
/*                                                                      */
/*                    七、会议合成信息管理 结构群                       */
/*                                                                      */
/************************************************************************/
/*--------------------------new vmp----------------------------*/
//合成器抢占相关信息 len:(8+1+2+15+1)=27
struct TVmpPriSeizeInfo
{
public:
	TVMPMember m_tPriSeizeMember;	//要抢占的成员
	u8 m_byChlIdx;					//要抢占成员的通道
	u16 m_wEventId;					//消息id
	TSwitchInfo m_tSwitchInfo;		//交换信息(用于选看)
	u8 m_bySrcSsnId;				//源ssnid(用于选看刷图标)
public:
	TVmpPriSeizeInfo()
	{
		Clear();
	}
	void Clear()
	{
		m_tPriSeizeMember.SetNull();
		m_byChlIdx = MAXNUM_VMP_MEMBER;
		m_wEventId = 0;
		memset(&m_tSwitchInfo, 0, sizeof(m_tSwitchInfo));
		m_bySrcSsnId = 0;
	}
};

//合成相关会议信息
struct TVmpInfo
{
	TVMPParam_25Mem m_tConfVMPParam;
	TVMPParam_25Mem m_tLastVmpParam;
	TKDVVMPOutParam m_tVMPOutParam;
	TVmpChnnlInfo m_tVmpChnnlInfo;
	TVmpPriSeizeInfo m_tVmpPriSeizeInfo;
public:
	TVmpInfo()
	{
		Clear();
	}
	void Clear()
	{
		m_tConfVMPParam.Clear();
		m_tLastVmpParam.Clear();
		m_tVMPOutParam.Initialize();
		m_tVmpChnnlInfo.clear();
		m_tVmpPriSeizeInfo.Clear();
	}
};

//N+1画面合成模板信息
struct TNplusVmpModule
{
public:
	u8 m_byConfIdx;
	u8 m_byEqpId;
	u8 m_byMtId[MAXNUM_VMP_MEMBER];
	u8 m_byMemberType[MAXNUM_VMP_MEMBER];

public:
	TNplusVmpModule ( void ) 
	{ 
		Clear();
	}

	void Clear ( void )
	{
		memset(this,0,sizeof(*this));
		m_byConfIdx = 0xFF;
	}

	BOOL32 IsNull ( void )
	{
		if( m_byEqpId == 0 && m_byConfIdx == 0xFF )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	BOOL32 HasVmpMember ( void )
	{
		for( u8 byChIdx = 0; byChIdx < MAXNUM_VMP_MEMBER; byChIdx++ )
		{
			if(m_byMtId[byChIdx]!= 0 || m_byMemberType[byChIdx]!=0)
			{
				return TRUE;
			}
		}

		return FALSE;
	}

	void Print ( void )
	{
		if( m_byConfIdx!= 0xFF && m_byEqpId != 0)
		{
			StaticLog("[N+1]Vmp Module ConfIdx.%d EqpId.%d\n",m_byConfIdx,m_byEqpId);
			for (u8 byChidx = 0; byChidx < MAXNUM_VMP_MEMBER; byChidx++ )
			{
				if(m_byMemberType[byChidx]!=0 || m_byMtId[byChidx]!= 0)
				{
					StaticLog("    ChnIndex[%d] MemberType[%d] MtId[%d]\n",byChidx,m_byMemberType[byChidx],m_byMtId[byChidx]);
				}
			}
		}
	}
};

//会议合成信息管理类
class CVmpInfoMgr
{
public:
	// 获得指定vmp会议合成信息
	TVMPParam_25Mem GetConfVmpParam(TEqp tVmpEqp)
	{
		TVMPParam_25Mem tConfVmpParam;
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			tConfVmpParam = m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tConfVMPParam;
		}
		return tConfVmpParam;
	}
	// 设置指定vmp会议合成信息
	void SetConfVmpParam(TEqp tVmpEqp, TVMPParam_25Mem tConfVmpParam)
	{
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tConfVMPParam = tConfVmpParam;
		}
	}
	// 获得指定vmp的Last合成信息
	TVMPParam_25Mem GetLastVmpParam(TEqp tVmpEqp)
	{
		TVMPParam_25Mem tLastVmpParam;
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			tLastVmpParam = m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tLastVmpParam;
		}
		return tLastVmpParam;
	}
	// 设置指定vmp的Last合成信息
	void SetLastVmpParam(TEqp tVmpEqp, TVMPParam_25Mem tLastVmpParam)
	{
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tLastVmpParam = tLastVmpParam;
		}
	}
	// 获得指定vmp后向通道信息
	TKDVVMPOutParam GetVMPOutParam(TEqp tVmpEqp)
	{
		TKDVVMPOutParam tVMPOutParam;
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			tVMPOutParam = m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tVMPOutParam;
		}
		return tVMPOutParam;
	}
	// 设置指定vmp后向通道信息
	void SetVMPOutParam(TEqp tVmpEqp, TKDVVMPOutParam tVMPOutParam)
	{
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tVMPOutParam = tVMPOutParam;
		}
	}
	// 获得指定vmp前适配相关信息
	TVmpChnnlInfo GetVmpChnnlInfo(TEqp tVmpEqp)
	{
		TVmpChnnlInfo tVmpChnnlInfo;
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			tVmpChnnlInfo = m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tVmpChnnlInfo;
		}
		return tVmpChnnlInfo;
	}
	// 设置指定vmp前适配相关信息
	void SetVmpChnnlInfo(TEqp tVmpEqp, TVmpChnnlInfo tVmpChnnlInfo)
	{
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tVmpChnnlInfo = tVmpChnnlInfo;
		}
	}
	// 获得指定vmp前适配抢占相关信息
	TVmpPriSeizeInfo GetVmpPriSeizeInfo(TEqp tVmpEqp)
	{
		TVmpPriSeizeInfo tVmpPriSeizeInfo;
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			tVmpPriSeizeInfo = m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tVmpPriSeizeInfo;
		}
		return tVmpPriSeizeInfo;
	}
	// 设置指定vmp前适配抢占相关信息
	void SetVmpPriSeizeInfo(TEqp tVmpEqp, TVmpPriSeizeInfo tVmpPriSeizeInfo)
	{
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tVmpPriSeizeInfo = tVmpPriSeizeInfo;
		}
	}

	void SetConfVmpMode(TEqp tVmpEqp, u8 byVMPMode)
	{
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tConfVMPParam.SetVMPMode(byVMPMode);
		}
	}
	
	u16 GetVMPOutChlBitrate(TEqp tVmpEqp, u8 byIdx)
	{
		u16 wBr = 0;
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX && byIdx < MAXNUM_MPU2_OUTCHNNL)
		{
			wBr = m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tVMPOutParam.GetVmpOutCapIdx(byIdx).GetMaxBitRate();
		}
		return wBr;
	}

	void SetVMPOutChlBitrate(TEqp tVmpEqp, u8 byIdx, u16 wBr)
	{
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX && byIdx < MAXNUM_MPU2_OUTCHNNL)
		{
			TVideoStreamCap tVidStrCap = m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tVMPOutParam.GetVmpOutCapIdx(byIdx);
			tVidStrCap.SetMaxBitRate(wBr);
			m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tVMPOutParam.UpdateVmpOutMember(byIdx, tVidStrCap);
		}
	}
	
	void SetVMPBrdst(TEqp tVmpEqp, u8 byVMPBrdst)
	{
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tConfVMPParam.SetVMPBrdst(byVMPBrdst);
			m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tLastVmpParam.SetVMPBrdst(byVMPBrdst);
		}
	}

	void ClearVmpInfo(TEqp tVmpEqp)
	{
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].Clear();
		}
	}

	void ClearVmpAdpHdChl(TEqp tVmpEqp)
	{
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tVmpChnnlInfo.clearHdChnnl();
		}
	}

	u8 GetVmpAdpMaxStyleNum(TEqp tVmpEqp)
	{
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			return m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tVmpChnnlInfo.GetMaxStyleNum();
		}
		return 0;
	}

	u8 GetVmpAdpHDChlNum(TEqp tVmpEqp)
	{
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			return m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tVmpChnnlInfo.GetHDChnnlNum();
		}
		return 0;
	}

	u8 GetVmpAdpMaxNumHdChl(TEqp tVmpEqp)
	{
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			return m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tVmpChnnlInfo.GetMaxNumHdChnnl();
		}
		return 0;
	}

	u8 GetVmpAdpChnlCountByMt(TEqp tVmpEqp, TMt tMt)
	{
		if (tVmpEqp.GetEqpType() == EQP_TYPE_VMP && tVmpEqp.GetEqpId() >= VMPID_MIN && tVmpEqp.GetEqpId() <= VMPID_MAX)
		{
			return m_atVmpInfo[tVmpEqp.GetEqpId()-VMPID_MIN].m_tVmpChnnlInfo.GetChnlCountByMt(tMt);
		}
		return 0;
	}

private:
	TVmpInfo m_atVmpInfo[MAXNUM_PERIEQP];//合成器最大16个外设
};
/*------------------------------------------------------------------*/

/************************************************************************/
/*                                                                      */
/*                     八、适配录像管理类                         */
/*                                                                      */
/************************************************************************/
struct TRecBasChn
{
	TEqp	m_tBas;
	u8		m_byChnId;
	TRecBasChn()
	{
		Clear();
	}
	~TRecBasChn()
	{
		Clear();
	}

	void Clear()
	{
		m_tBas.SetNull();
		m_byChnId = 0XFF;
	}
};

#define  MAX_RECBASCHN_NUM	3
class CRecAdaptMgr
{
public:
	CRecAdaptMgr();
	~CRecAdaptMgr();

	void Clear(u8 byMediaMode = MODE_BOTH);

	void	SetRecVideoCapSet(u8 byMediaMode, const TVideoStreamCap &tVideoCap);
	BOOL32  GetRecVideoCapSet(u8 byMediaMode, TVideoStreamCap &tVideoCap);

	void	SetRecAudCapSet(const TAudioTypeDesc &tAudCap);
	BOOL32  GetRecAudCapSet(TAudioTypeDesc &tAudCap);

	void	AddRecBasChn(u8 byMediaMode, const TRecBasChn &tRecBasChn);
	BOOL32  IsExistRecBasChn(u8 byMediaMode,TRecBasChn &tRecBasChn);
	
	BOOL32  IsExistMediaModeForRecAdpat(const TRecBasChn &tRecBasChn,u8& byMediaMode);
	void   ClearBasChnForAud();

	void Print();

private:
	enum emAdaptMode
	{
		RECBAS_VIDEO = 0,
		RECBAS_SECVIDEO,
		RECBAS_AUDIO
	};

	TVideoStreamCap		m_tRecMainParam;
	TVideoStreamCap		m_tRecDSParam;
	TAudioTypeDesc		m_tRecAudParam;
	TRecBasChn			m_atRecBasChn[MAX_RECBASCHN_NUM];
}
#ifndef WIN32
__attribute__( (packed) )

#endif

;

enum emNetCapState
{
	emNetCapIdle =  0,
	emNetCapStart  = 1,
};


struct TNetCapMgr
{
public:
	TNetCapMgr()
	{
		Clear();
	}
	~TNetCapMgr()
	{
		Clear();
	}
	void Clear()
	{
		memset(this,0,sizeof(TNetCapMgr));
	}
	void SetNetCapStatus( const emNetCapState& emNetCapStatus)
	{
		m_byNetCapStatus = (u8)emNetCapStatus;
	}
	emNetCapState GetNetCapStatus()const
	{
		return (emNetCapState)m_byNetCapStatus;
	}
	void SetOprNetCapMcsInstID(const u8& byMcsInstId)
	{
		m_byOprNetCapMcsInstID = byMcsInstId;
	}
	u8 GetOprNetCapMcsInstID()const
	{
		return m_byOprNetCapMcsInstID;
	}

	void StartNetCap()
	{
#if defined(_8KE_) || defined(_8KH_)
		FILE* pCapChoice = fopen("/opt/mcu/pcap/CapChoice","w");
#else
		FILE* pCapChoice = fopen("/usr/mcu/temp/CapChoice","w");
#endif
		if (NULL != pCapChoice)
		{
			fwrite("1",1,1,pCapChoice);
			fclose(pCapChoice);
		}
	
	}
	void StopNetCap()
	{
		s8 szStartCapCmd[256] = {0};
#if defined(_8KE_) || defined(_8KH_)
		sprintf(szStartCapCmd,"for file in /opt/mcu/pcap/UsrNetCap* ; do \n mv -f $file $file.pcap \n  done;  killall tcpdump");
#else
		sprintf(szStartCapCmd,"for file in /usr/mcu/temp/UsrNetCap* ; do \n mv -f $file $file.pcap \n  done;  killall tcpdump");
#endif
		system(szStartCapCmd);

// 		FILE* pCapChoice = fopen("/opt/mcu/pcap/CapChoice","w");
// 		if (NULL != pCapChoice)
// 		{
// 			fwrite("0",1,1,pCapChoice);
// 			fclose(pCapChoice);
// 		}
	}
private:
	u8 m_byNetCapStatus;          //当前抓包状态
	u8 m_byOprNetCapMcsInstID;    //当前正在操作抓包的mcs instid

}
PACKED
;

struct TStartRecMsgInfo
{
	TMt m_tRecordMt;		
	TEqp m_tRecEqp;
	TRecStartPara m_tRecPara;
	s8 m_aszRecName[MAX_FILE_NAME_LEN];
	BOOL32 bIsRecAdaptConf;
	u16    m_wSerialNO;	//多回传使用
	u8	   m_bySrcSsnId;//多回传使用
	TMtAlias m_tVrsRecAlias;//新录播呼叫信息
	TStartRecMsgInfo()
	{
		m_tRecordMt.SetNull();
		m_tRecEqp.SetNull();
		m_tRecPara.Reset();
		memset(m_aszRecName,0,sizeof(m_aszRecName));
		bIsRecAdaptConf = FALSE;
		m_tVrsRecAlias.SetNull();
	}
}
PACKED
;

struct TStartRecData
{
	TDoublePayload m_tDVPayload;
	TDoublePayload m_tDAPayload;
	TDoublePayload m_tDDVPayload;
	TRecRtcpBack   m_tRtcpBack;
	TStartRecData()
	{
		m_tDVPayload.Reset();
		m_tDAPayload.Reset();
		m_tDVPayload.Reset();
		m_tRtcpBack.Reset();
	}
}
PACKED
;

/************************************************************************/
/*                                                                      */
/*                     九、级联多回传相关管理类                         */
/*                                                                      */
/************************************************************************/
/*==============================================================================
类名    :  CMultiSpyMgr
功能    :  级联多回传管理
主要接口:  
备注    :  
-------------------------------------------------------------------------------
修改记录:  
日  期     版本          修改人          走读人          修改记录
2009-9-28                 薛亮
==============================================================================*/
class CMultiSpyMgr
{
/*lint -save -sem(CMultiSpyMgr::Init,initializer)*/
public:

	CMultiSpyMgr(void);
    ~CMultiSpyMgr(void);

	BOOL32 AssignSpyChnnl ( u16 &wChnnlIdx );  // 分配mcu空闲的回传通道

	BOOL32 ReuseSpyChnnl( u16 wChnnlIdx );
	void   ReleaseSpyChnnl( u16 wChnnlIdx );  // 释放回传通道
		
	u16    GetMSMultiSpy(u8 *pbyBuf);
	u16    SetMSMultiSpy(u8 *pbyBuf);

protected:
	void Init( void );

private:

	u8	    m_byIsMultiSpy[MAXNUM_ACCOMMODATE_SPY];		//数组的索引即是spyChnnl，数组的元素记录该通道(通道号:索引值 * PORTSPAN + SPY_MCU_STARTPORT)是否已被占用
	SEMHANDLE	m_hSpyRW;
};

struct TSpyPrsResource
{
protected:
	TEqp	m_tPrsEqp;//prs设备
	u8		m_byChannel;//占用的prs通道号

public:
	TSpyPrsResource(){ Clear(); }

	void Clear( void )
	{
		m_tPrsEqp.SetNull();
		m_byChannel = 0;
	}

	void SetPrsInfo( TEqp tPrsEqp,u8 byChannel )
	{
		m_tPrsEqp = tPrsEqp;
		m_byChannel = byChannel;
	}

	void GetPrsInfo( TEqp &tPrsEqp,u8 &byChannel )
	{
		tPrsEqp = m_tPrsEqp;
		byChannel = m_byChannel;
	}	

	TEqp GetPrsEqp( void )
	{
		return m_tPrsEqp;
	}

	u8 GetPrsChlIdx( void )
	{
		return m_byChannel;
	}

	BOOL32 IsNull( void )
	{
		return m_tPrsEqp.IsNull();
	}
}
PACKED
;
//多回传下级终端占用bas的信息
struct TSpyBasResource
{
protected:
	TEqp	m_tVideoBas;//视频适配用的bas
	u8		m_byVideoChlIndex;//视频或码率视频使用的bas通道号
	TEqp	m_tAudioBas;//音频适配用的bas
	u8		m_byAudioChlIndex;//音频视频使用的bas通道号
	u8		m_byMode;

public:
	TSpyBasResource( void )	{ Clear(); }

	void Clear(){ 
		m_tVideoBas.SetNull();
		m_tAudioBas.SetNull();
		m_byVideoChlIndex=m_byAudioChlIndex =0;
		m_byMode = MODE_NONE;	
	}

	void Clear( u8 byMode )
	{
		switch( byMode )
		{
			case MODE_VIDEO:
				m_tVideoBas.SetNull();
				m_byVideoChlIndex = 0;
				if( MODE_BOTH == m_byMode )
				{
					m_byMode = MODE_AUDIO;
				}
				break;
			case MODE_AUDIO:
				m_tAudioBas.SetNull();
				m_byAudioChlIndex = 0;
				if( MODE_BOTH == m_byMode )
				{
					m_byMode = MODE_VIDEO;
				}
				break;
			case MODE_BOTH:
				Clear();
				break;
			default:
				break;
		}
	}

	void SetBasInfo( TEqp tBas,u8 byChlIndex,u8 byMode )
	{
		switch( byMode )
		{
		case MODE_VIDEO:
			m_tVideoBas = tBas;
			m_byVideoChlIndex = byChlIndex;			
			break;
		case MODE_AUDIO:
			m_tAudioBas = tBas;
			m_byAudioChlIndex = byChlIndex;	
			break;
		default:
			break;
		}
		SetMode( byMode );
	}

	TEqp GetVideoBas( void ){return m_tVideoBas;}
	TEqp GetAudioBas( void ){return m_tAudioBas;}
	//void SetBas( TEqp tBas ){m_tBas = tBas;}

	u8 GetVideoChlIndex(void){ return m_byVideoChlIndex; }
	u8 GetAudioChlIndex(void){ return m_byAudioChlIndex; }
	//void SetChlIndex( u8 byChlIndex ){ m_byChlIndex = byChlIndex; }

	u8 GetMode( void ){ return m_byMode; }
	void SetMode( u8 byMode )
	{ 
		if( MODE_NONE == m_byMode )
		{			
			m_byMode = byMode; 
		}
		else if( MODE_VIDEO == m_byMode )
		{
			if( MODE_AUDIO == byMode )
			{
				m_byMode = MODE_BOTH;
			}
		}
		else if( MODE_AUDIO == m_byMode )
		{
			if( MODE_VIDEO == byMode )
			{
				m_byMode = MODE_BOTH;
			}
		}
		
	}

	BOOL32 IsNull()
	{
		return ( m_tVideoBas.IsNull() && m_tAudioBas.IsNull() );
	}


}
PACKED
;

typedef TSpyResource CSendSpy;


// 记录本级会议中成员被回传到上级的信息
class CConfSendSpyInfo
{
public:

	CConfSendSpyInfo();
	void Clear( void );

    BOOL32 GetSpyChannlInfo( TMt &tMt, CSendSpy &tSrcSpyInfo );  // 根据终端信息，得到本级回传到上级的回传成员信息
	BOOL32 AddSpyChannlInfo( CSendSpy &tSrcSpyInfo );  // 根据终端信息，设置相应的回传通道信息
	//lukunpeng 2010/06/10 不需要管理本级的上传还有多少带宽可用，完全交给上级控制
//     u32    GetConfMaxSpyBW( void );  
// 	void   SetConfMaxSpyBW( u32 dwMaxSpyBW );  
//     u32    GetConfRemainSpyBW( void );  
// 	void   SetConfRemainSpyBW( u32 dwRemainSpyBW );
	BOOL32 FreeSpyChannlInfo(const TMt &tMt, u8 bySpyMode); // 根据终端信息释放一个回传通道	
	u8		GetSpyMode(const TMt &tMt );		// 获取spy mode （Video, audio, both）
	//BOOL32 SetSpyMode( TMt &tMt, u8 byMode );   // 记录当前的重传模式
//	BOOL32 SetSpySimCap(const TMt &tMt, TSimCapSet &tDstSimCap );  // 设置回传成员的的目的端得能力
//	BOOL32 GetSpySimCap(const TMt &tMt, TSimCapSet &tDstSimCap );  // 得到回传成员的的目的端得能力
	//BOOL32 IsMtInSendSpyMember( TMt &tMt );  // 是否在回传成员列表
	//BOOL32 SetOldMode( TMt &tMt, u8 byOldMode );   // 记录老的重传模式
	//u8     GetOldMode( TMt &tMt );

	CSendSpy* GetSendSpy(u16 wIndex);
//	u16 GetSendSpyNum();
	
	//zhouyiliang 20100728 查找是否还有除tmt外还在用wSpyPort的回传终端 
	BOOL32 IsOtherSpyMtUsePort( const TMt & tMt, const u16 wSpyPort );
	
	BOOL32 IsRepeatedSpy( CSendSpy &tSrcSpyInfo, BOOL32 bNeglectMode = FALSE, BOOL32 bNeglectSpyPort = FALSE);

    BOOL32 SaveSpySimCap( const TMt &tSpyMt, const TSimCapSet &tSimCap );
	
	
private:

    CSendSpy m_cSpyMember[MAXNUM_CONF_SPY];      // 当前会议中回传到上级会议的成员
//	u16 m_wSpyNum;								 // 20110531 zjl 目前这个变量没意义
	//lukunpeng 2010/06/10 不需要管理本级的上传还有多少带宽可用，完全交给上级控制
// 	u32			   m_dwMaxSpyBW;		               // 会议配置的最大用于回传到上级会议的回传带宽
// 	u32            m_dwRemainSpyBW;                    // 当前会议剩余的回传到上级会议的回传带宽
};
struct TSpyStatus
{
	enum
	{
		IDLE = 0,	//空闲
		NORMAL,		//正在使用
		WAIT_FREE,	//等待释放
		WAIT_USE	//预占用，等待真正使用
	};
}
PACKED
;


class CRecvSpy:public TSpyResource
{
public:

	CRecvSpy()
	{
        Clear();
	}

	void Clear()
	{
		memset( this, 0, sizeof(CRecvSpy) );
	}

	void GetSpyResource( TSpyResource &tSpySrc )
	{
		tSpySrc.m_tSpy = m_tSpy;
		tSpySrc.m_tSimCapset = m_tSimCapset;
		tSpySrc.m_tSpyAddr = m_tSpyAddr;
		tSpySrc.m_tVideoRtcpAddr = m_tVideoRtcpAddr;
		tSpySrc.m_tAudioRtcpAddr = m_tAudioRtcpAddr;
		tSpySrc.m_bySpyMode = m_bySpyMode;
		tSpySrc.m_byReserved1 = m_byReserved1;
		tSpySrc.m_dwReserved2 = m_dwReserved2;
	}

public:
	u32	m_dwVidBW;		// 该Spy回传视频占用带宽
	u32 m_dwAudBW;		// 该Spy回传音频占用带宽
	u8 m_byVSpyDstNum;	// 记录回传源之目的数目(视频)
	u8 m_byASpyDstNum;  // 记录回传源之目的数目(音频)
	u8 m_byUseState;	// 记录是否预占用 是-1 否-0
	u8 m_byPreAddMode;	// 记录预占用模式
	s8 m_byPreAddDstNum;//记录预占用增加目的数
};

//  [11/27/2009 pengjie] 级联多回传支持，记录监控的通道和源的映射关系
class CMcChnnlInfo
{
	/*lint -save -sem(CMcChnnlInfo::Clear,initializer)*/
public:

	CMcChnnlInfo();
	void   Clear( void );
    BOOL32 SetMcChnnlMode( u8 byMode );
	u8     GetMcChnnlMode( void );
	BOOL32 SetMcSrc( TMt &tmt );
	TMt    GetMcSrc( void );

private:

	u8 m_byMode;    // 监控的模式 MODE_VIDEO,MODE_AUDIO ...
	TMt m_tSrc;     // 进入监控的源终端
};

class CMcChnMgr  // 会控监控通道管理类
{
public:

	CMcChnMgr();
	void Clear( void );

	//BOOL32 SetMcChnnlInfo( u8 byChnnl, CMcChnnlInfo &cMcChnnlInfo );
	//BOOL32 GetMcChnnlInfo( u8 byChnnl, CMcChnnlInfo &cMcChnnlInfo );
	BOOL32 FreeMcChnnl( u8 byChnnl );

private:

	CMcChnnlInfo m_cMcChnnl[MAXNUM_MC_CHANNL];
};

// 记录本级会议所接纳的下级回传源信息
class CConfRecvSpyInfo
{
public:
	CConfRecvSpyInfo();
	void   Clear( CMultiSpyMgr *pcMultiSpyMgr = NULL );
	BOOL32 IsMtInSpyMember(const TMt &tMt, u8 bySpyMode = MODE_NONE);	// 某终端是否在回传成员中,外加判断回传模式
	//BOOL32 IsMtInSpyMember(const TMt &tMt );                 // 某终端是否在回传成员中
	//BOOL32 IsMcuMtInSpyMember(const u8 byMcuId );              // 判断某个下级mcu是下是否还有终端在回传
	//BOOL32 IsUsedSpyMode(const TMt &tMt, u8 bySpyMode = MODE_BOTH ); // 判断是否能够释放一个回传通道(或者它的某种回传模式)

	void LeftOnceToFree(const TMt &tMt, u8 bySpyMode, u32 &dwCanReleaseBW, s16& swCanRelIndex);

	void IsCanFree( const TMt &tMt, u8 bySpyMode, u8 byVideoDstNum, u8 byAudioDstNum, u32 &dwCanReleaseBW, s16& swCanRelIndex);
	//BOOL32 AddSpyMode(const TMt &tMt, u8 bySpyMode, u32 dwSpyBW = 0, u8 byAddDstNum = 0);  // 根据终端信息，设置相应的回传通道信息, 如果终端未在回传成员中，返回False
	//BOOL32 FreeSpyChannlInfo(const TMt &tMt, u8 bySpyMode = MODE_BOTH, BOOL32 bForce = FALSE ); // 根据终端信息释放一个回传通道

	BOOL32 GetRecvSpy(const TMt &tMt, CRecvSpy &tSrcSpyInfo );      // 根据终端，得到相应的回传信息 (CRecvSpy存储了回传源的音视频目的数目，所以再些了一个) 
//	BOOL32 GetSpyMtBySpyPort( u16 wSpyPort,TMt &tSrc );
//	void   FreeSpyChannlInfoByMcuId( u16 wMcuIdx, CMultiSpyMgr *pcMultiSpyMgr );

	//s16 FindSpyMt(const TMt &tMt);

	s16 FindSpyMt(const TMt &tMt, u8 bySpyMode = MODE_NONE);

//	BOOL32 IncSpyModeDst(const TMt &tMt, u8 bySpyMode, u8 byDstNum = 1, BOOL32 bIsPreAdd = FALSE);
//	BOOL32 DecSpyModeDst(const TMt &tMt, u8 bySpyMode, u8 byDstNum = 1);

	//BOOL32 AddSpyMode(const TMt &tMt, u8 bySpyMode,	BOOL32 bIsPreAdd = FALSE );
	BOOL32 RemoveSpyMode(const TMt &tMt, u8 bySpyMode, u32 &dwReleaseBW, s16& swChnnlID, u8& byRelSpyMode);   // 移除回传通道的模式
	BOOL32 AddSpyChnnlInfo(const TMt &tMt, u16 SpyStartPort, s16 &swIndex,BOOL32 bIsPreAdd = FALSE );         // 为某个终端配置端口

	BOOL32 SaveSpySimCap( const TMt &tSpyMt, const TSimCapSet &tSimCap );

	BOOL32 GetSpySimCap( const TMt &tSpyMt, TSimCapSet &tSimCap) const;

	//根据回传源目数，调整回传模式
	void AdjustSpyMode(const TMt &tMt, u32 &dwReleaseBW, s16& swChnnlID, u8& byRelSpyMode);

	BOOL32 DecAndAdjustSpyMode(const TMt &tMt, u8 bySpyMode, u32& dwReleaseBW, s16& swChnnlID, u8& byRelSpyMode);

	const CRecvSpy* GetSpyMemberInfo( u16 wIndex );//得到一个回传成员的信息

//	u16 GetSpyNum();

	void ReplaceSpy(s16 swSpyIndex, const TMt &tMt);

//private:

	BOOL32 GetRecvSpy(s16 swSpyIndex, CRecvSpy &tSrcSpyInfo );

	void   AdjustSpyMode(s16 swSpyIndex, u32 &dwReleaseBW, s16& swChnnlID, u8& byRelSpyMode);
	
	BOOL32 AddSpyMode(s16 swSpyIndex, u8 bySpyMode,BOOL32 bIsPreAdd = FALSE);

	BOOL32 IncSpyModeDst(s16 swSpyIndex, u8 bySpyMode, s16 swDstNum = 1, BOOL32 bIsPreAdd = FALSE);

	BOOL32 DecSpyModeDst(s16 swSpyIndex, u8 bySpyMode, s16 swDstNum = 1);

	BOOL32 ModifySpyModeBW(s16 swSpyIndex, u8 bySpyMode, u32 dwVidBW, u32 dwAudBW);

	BOOL32 FreeSpyModeBW(s16 swSpyIndex, u8 bySpyMode, u32 &dwReleaseBW);

	BOOL32 ClearSpyModeDst(s16 swSpyIndex, u8 bySpyMode);
	BOOL32 ReleasePreAddRes( const TMt &tMt,u32 &dwReleaseBW,s16& swChnnlID );

	BOOL32 ModifyUseState( const TMt &tMt,u8 byState );

	BOOL32 ModifyUseState( s16 swSpyIndex,u8 byState );

	BOOL32 GetFstSpyMtByMcuIdx( u16 wMcuIdx,TMt &tMt );
	
	void SetSpyBackVidRtcpAddr(s16 swSpyIndex, u8 bySpyMode, TTransportAddr tVideoRtcpAddr);
	void SetSpyBackAudRtcpAddr(s16 swSpyIndex, u8 bySpyMode, TTransportAddr tAudRtcpAddr);
	void GetSpyBackVidRtcpAddr(s16 swSpyIndex, u8 bySpyMode, TTransportAddr& tVideoRtcpAddr);
	void GetSpyBackAudRtcpAddr(s16 swSpyIndex, u8 bySpyMode, TTransportAddr& tAudRtcpAddr);

private:

	CRecvSpy m_cSpyMember[MAXNUM_CONF_SPY];     // 当前会议接纳的回传成员
//	u16 m_wSpyNum;								// 20110531 zjl 目前这个变量没意义
};


/************************************************************************/
/*                                                                      */
/*                      十、MCU 会议 全局数据类                          */
/*                                                                      */
/************************************************************************/

class CMcuVcInst;

class CMcuVcData : public CPeriEqpChnnlMModeMgr
{
	/*lint -save -sem(CMcuVcData::ClearVcData,initializer)*/
	
	#define USERNUM_PERPKT  32      //每包消息中用户个数
    #define ADDRENTRYNUM_PERPKT 64  //每包消息中地址簿条目个数
    #define ADDRGROUPNUM_PERPKT 16  //每包消息中地址簿条目组个数
	

	friend class CMpManager;
    friend class CMcuVcInst;

protected:
	CMcuVcInst      *m_apConfInst[MAXNUM_MCU_CONF];         //会议信息表
	TPeriEqpData     m_atPeriEqpTable[MAXNUM_MCU_PERIEQP];  //外设信息表，外设号减1为数组下标

	TDcsInfo		 m_atPeriDcsTable[MAXNUM_MCU_DCS];		//DCS信息表, DCS id号减1为数组下标 2005-12-14
	TMcData          m_atMcTable[MAXNUM_MCU_MC + MAXNUM_MCU_VC];            //会控信息表，会控实例号减1为数组下标
	TMonitorMgr		 m_atMonitorTable[MAXNUM_MCU_MC + MAXNUM_MCU_VC];		//监控联动信息
	TMpData          m_atMpData[MAXNUM_DRI];                //Mp信息表
	TMtAdpData       m_atMtAdpData[MAXNUM_DRI];             //MtAdp信息表
	
	//zjj 20121225 上级mcu不算点数,但是会占后向端口点,所以端口点数应该多算1个,否则呼上来的终端后向通道就分配不到成为0
	TRecvMtPort      m_atRecvMtPort[MAXNUM_MCU_MT+1];
//	TIpAddrRes       m_atMulticastIp[MAXNUM_MCU_CONF];
	TRecvMtPort      m_atMulticastPort[MAXNUM_MCU_MT+1];

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
	u8               m_abyTempEditer[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE]; // 编辑指定模板的用户信息
	CVmpMemVidLmt    m_cVmpMemVidLmt;			// mpu-vmp 各通道能力限制(主要是分辨率)

    u32              m_adwEqpIp[MAXNUM_MCU_PERIEQP];
	BOOL32           m_bRRQDriTransferred;      //主接入板 迁移标识

	CMultiSpyMgr	 m_cMultiSpyMgr;	//级联多回传管理
	
	u32				 m_dwMcuStartTick;	//系统出示tick数
	CBasChnListMgr   m_cBasChnListMgr;
	
	BOOL32           m_bUSBKEYFlag;     //[liu lijiu][2010/10/28]USBKEY授权数据是否验证成功仅限于8000G使用
	u16              m_wUSBKEYErrorCode; //返回USBKEY验证的错误码

	u16				 m_wCurrentHDMtNum;  //MPC已经接入的高清终端数 //  [7/28/2011 chendaiwei]
	u8				 m_abyHDMtData[MAX_CONFIDX][MAXNUM_CONF_MT/8]; //记录终端是否占用高清接入点 //  [7/27/2011 chendaiwei]
	TDri2E1Cfg       m_atDri2E1CfgTable[MAXNUM_SUB_MCU];            //对于Dri2的E1级联配置
	u16              m_wCurrentAudMtNum; //当前已经接入的语音终端数     
	u8				 m_abyAudMtData[MAX_CONFIDX][MAXNUM_CONF_MT/8]; //记录终端是否占用语音接入点 zjl 20120815
	CVmpInfoMgr      m_cVmpInfoMgr;		// 画面合成信息管理
#if defined(_8KH_) ||  defined(_8KE_) || defined(_8KI_)
	TNetCapMgr       m_tNetCapMgr;
#endif

	TNplusVmpModule	m_tNplusVmpModule[MAXNUM_PERIEQP];
	
public:
    
    // 获取MCU状态
    BOOL32 GetMcuCurStatus( TMcuStatus &tMcuStatus );
	//获得MCU ext 状态
	BOOL32	GetMcuExtCurStatus( u8 byCurEqpId, u8 &byPeriEqpNum, TEqp *patPeriEqp, u8 *pbyEqpOnline, u32 *pdwPeriEqpIpAddr );
	
	u32	   GetMcuStartTick();
	void   SetMcuStartTick(u32 dwMcuStartTick);

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
	//zjj20091102 加一个参数，会议老的TConfStore指针，如果是修改模板就不为空，新增模板时为空
	//为了区分是修改和新增，因为修改时要把会议信息文件中的动态增长的数据拿出来(现在仅仅时vcs会议中的预案)
	BOOL32 SaveConfToFile( u8 byConfIdx, BOOL32 bTemplate, BOOL32 bDefaultConf = FALSE,TConfStore *ptOldConfStore = NULL );
	BOOL32 RemoveConfFromFile( CConfId cConfId );
	BOOL32 GetConfFromFile(u8 byConfIdx, TPackConfStore *ptPackConfStore);
	void   RefreshHtml( void );
	BOOL32 IsConfNameRepeat( LPCSTR lpszConfName, BOOL32 IsTemple ,BOOL32 bIsOngoingConf );
	BOOL32 IsConfE164Repeat( LPCSTR lpszConfE164, BOOL32 IsTemple ,BOOL32 bIsOngoingConf );
	BOOL32 IsE164Repeated( LPCSTR lpszE164, BOOL32 IsTemple,BOOL32 bIsOngoingConf  );
	BOOL32 IsConfOnGoing(CConfId cConfId);
	u8   GetOngoingConfIdxByE164( LPCSTR lpszConfE164 );
    u8   GetTemConfIdxByE164( LPCSTR lpszConfE164 );
    u16  GetConfRateByConfIdx( u16 wConfIdx, u16& wFirstRate, u16& wSecondRate, BOOL32 bTemplate = FALSE ); // 根据会议序号取会议速率
    u8   GetConfNameByConfId( const CConfId cConfId, LPCSTR &lpszConfName );  // 根据会议ID取会议名称
    CConfId GetConfIdByName(LPCSTR lpszConfName, BOOL32 bTemplate  ,BOOL32 bIsOngoingConf);  // 根据会议名称取CConfId
	CConfId GetConfIdByE164( LPCSTR lpszConfE164, BOOL32 bTemplate ,BOOL32 bIsOngoingConf); // 根据会议164号取CConfId

	void RegisterConfToGK( u8 byConfIdx, u8 byDriId = 0, BOOL32 bTemplate = FALSE, BOOL32 bUnReg = FALSE );
    void ConfChargeByGK( u8 byConfIdx, u8 byDriId, BOOL32 bStopCharge = FALSE, u8 byCreateBy = CONF_CREATE_MCS, u8 byMtNum = 0 );
    void ConfInfoMsgPack( CMcuVcInst *pcSchInst, CServMsg &cServMsg );//会议信息消息打包

    CConfId MakeConfId( u8 byConfIdx, u8 byTemplate, u8 byUsrGrpId, u8 byConfSource = MCS_CONF );
    u32     GetMakeTimesFromConfId(const CConfId& cConfId) const;
    
    void SetInsState( u8 byInsId, BOOL32 bState );
    u8   GetIdleInsId( void );       

    BOOL32 IsSavingBandwidth(void);             //是否节省带宽

	BOOL32 IsHoldDefaultConf(void);				//是否支持召开缺省会议

	void    GetMcuEqpCapacity(TEqpCapacity& tMcuEqpCap); // get the mcu's cap of equipment
    void    GetConfEqpDemand(TConfInfo& tConfInfo, TEqpCapacity& tConfEqpDemand );
	BOOL32  AnalyEqpCapacity( const TConfInfo &tConfInfo, const TConfInfoEx &tConfInfoEx,TEqpCapacity& tConfEqpDemand, TEqpCapacity& tMcuSupportCap);
	void GetBasCapacity( TBasReqInfo& tBasCap ); // 取Bas能力集
	void GetPrsCapacity( TEqpReqInfo& tPrsCap ); // 取Prs能力集    
    u8   GetConfFECType( u8 byConfIdx, u8 byMode );  // 当前会议前向纠错
    u8   GetConfEncryptMode( u8 byConfIdx );         // 当前会议的码流加密模式

	//级联多回传
	CMultiSpyMgr* GetCMultiSpyMgr( void );
    
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
    void   Msg2TemInfo( CServMsg &cMsg, TTemplateInfo &tTemInfo );
    void   TemInfo2Msg( TTemplateInfo &tTemInfo, CServMsg &cMsg );
    void   ShowTemplate( void );
    void   ShowConfMap( void );
	
    u8     GetTempEditerInfo(u8 byConfPos );
	void   SetTempEditerInfo(u8 byConfPos, u8 byInstId = 0);
	//更新模板信息中的扩展信息
	void UpdateExInfoInTemplate(TTemplateInfo &  tTemp,const emCommunicateType& byExenumType,const u16& wInfoLen,u8* pbyBuf);

    /************************************************************************/
    /*                                                                      */
    /*                     3、外设表操作函数                                */
    /*                                                                      */
    /************************************************************************/
	void   InitPeriEqpList();
	void   SetPeriEqpConnected( u8 byEqpId, BOOL32 bConnected = TRUE, BOOL32 bIsValid = TRUE );
	BOOL32 IsPeriEqpConnected( u8 byEqpId );
	void   SetPeriEqpLogicChnnl( u8 byEqpId, u8 byMediaType, u8 byChnnlNum, const TLogicalChannel * ptStartChnnl, BOOL32 bForwardChnnl );
	BOOL32 GetPeriEqpLogicChnnl( u8 byEpqId, u8 byMediaType, u8 * pbyChnnlNum, TLogicalChannel * ptStartChnnl, BOOL32 bForwardChnnl );
	void   SetEqpRtcpDstAddr( u8 byEqpId, u8 byChnnl, u32 dwDstIp, u16 wDstPort, u8 byMode = MODE_VIDEO );
	BOOL32 GetEqpRtcpDstAddr( u8 byEqpId, u8 byChnnl, u32 &dwDstIp, u16 &wDstPort, u8 byMode = MODE_VIDEO );	
	u32    GetEqpIpAddr(u8 byEqpId) const;	// [12/18/2009 xliang] 8000E用
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
	//20100708_tzy 增加获取APU混音器和EAPU混音器,增加最小能力限制,即获取一个能力比该参数大的混音器
	u8     GetIdleEapuMixer(u8 byMinMemberNum,u8 *pbyMixMemberNum = NULL);
	u8     GetIdleApuMixer(u8 byMinMemberNum,u8 *pbyMixMemberNum = NULL);
	u8     GetIdleMixer(u8 byMinMemberNum, u8 byAudCapNum, TAudioTypeDesc *ptAudioTypeDesc); 
	u8     GetIdleApu2Mixer(u8 byMinMemberNum,u8 *pbyMixMemberNum = NULL );
	u8     GetIdle8kxMixer(u8 byMinMemberNum);
	//u8     GetIdleBasChl( u8 byAdaptType, u8 &byEqpId, u8 &byChlId );
    //BOOL32 GetIdleHDBasVidChl( u8 &byEqpId, u8 &byChlIdx );
	//BOOL32 IsIdleHDBasVidChlExist(u8* pbyEqpId = NULL, u8* pbyChlIdx = NULL);
	//BOOL32 IsIdleBasVidChlExist( u8 byAdaptType,u8* pbyEqpId = NULL, u8* pbyChlIdx = NULL );

//    void   GetIdleMau(u8 &byNVChn, u8 &byVGAChn, u8 &byH263pChn);
//    void   GetIdleMpu(u8 &byChnNum);
    void   GetMpuNum(u8 &byMpuNum);
//	void   GetIdle8keBas(u8 &byMvChnNum, u8 &byDsChnNum);//[03/01/2010] zjl add 

	BOOL32 IsMultiModeHdu(const u8 byHduId);

	//  [5/17/2013 guodawei]
	u8 GetEqpCodeFormat(const u8 byEqpId);
	BOOL32 SetEqpCodeFormat(u8 byEqpId, u8 byCodeFormat);

/*------------------------new bas-------------------------------*/
	//统计bas资源
	void   AddMcuBasChnlInfo(const TEqp &tEqp, u8 byChnId);								
	//bas资源是否满足会议需求
	BOOL32 IsBasChnlEnoughForConf(const TConfInfo &tConfInfo, const TConfInfoEx &tConfInfoEx,  
								  CBasChn **ptBasChnConfNeed,  TBasChnCapData *ptBasChnCapData,u16 &wErrorCode);	
	//获取会议的编解码能力
	BOOL32 GetBasCapConfNeed(const TConfInfo &tConfInfo, const TConfInfoEx &tConfInfoEx, 
							 TNeedVidAdaptData *ptReqResource, TNeedAudAdaptData *pReqAudResource);	
	//广播占用bas通道(置外设表reserved 和链表 reserved)
	BOOL32 OcuppyBasChn(CBasChn **aptBasChn, u8 byChnNum );	
	//单个通道占用(置外设表reserved 和链表 reserved)
	BOOL32 OcuppyBasChn(u8 byEqpId, u8 byChnId);
	//当前适配通道是否正在适配
	emBASCHNSTATUS GetBasChnStatus(TEqp tBas, u8 byChnId);
	//更新适配通道状态
    BOOL32 UpdateBasChnStatus(const TEqp &tEqp, u8 byChnId, emBASCHNSTATUS byState);
	//根据源和目的能力获取空闲适配通道
	BOOL32 GetIdleBasChn(TSimCapSet &tSrcCap, TSimCapSet &tDstCap, u8 byMediaMode, CBasChn **ppcBasChnData,  TBasChnCapData *ptBasChnCapData);
	BOOL32 GetIdleAudBasChn(TAudioTypeDesc &tAudSrcCap, TAudioTypeDesc&tAudDstCap,CBasChn  **ppcBasChnData);
	//根据源和目的能力获取空闲适配通道, 如不足则通过降低编码性能循序尝试
	BOOL32 GetIdleBasChnPossible(TSimCapSet tSrcCap, TSimCapSet &tDstCap, u8 byMediaMode, CBasChn **ppcBasChnData,  TBasChnCapData *ptBasChnCapData);
	//根据编解码能力获取相应空闲适配通道
	BOOL32 IsBasChnEnoughForReq(TNeedVidAdaptData *ptReqResData, TNeedAudAdaptData *pReqAudResource, CBasChn **ptNeedChns,  TBasChnCapData*ptBasChnCapData);
	//获得BAS通道地址
	CBasChn* GetBasChnAddr(const TEqp &tEqp, u8 byChnId);
	//设置适配器在线状态
	void   SetBasOnLine(u8 byEqpId, BOOL32 bOnLine);
	//释放BAS通道
	void   ReleaseBasChn(const TEqp &tEqp, u8 byChnId);
	//获得高清BAS通道状态
	BOOL32 GetHDBasChnStatus(const TEqp &tEqp, u8 byChnId, THDBasVidChnStatus &tChnStatus);
	//获得音频BAS通道状态
	BOOL32 GetAudBasChnStatus(const TEqp &tEqp, u8 byChnId, TAudBasChnStatus &tChnStatus);

	//获得某个BAS通道在该通道所在外设上的输出偏移
	BOOL32 GetBasChnFrontOutPutNum(const TEqp &tEqp, u8 byChnId,  u8 &byOutNum, u8 &byFrontOutNum);
/*--------------------------------------------------------------*/

/*------------------------new vmp-------------------------------*/
	TVMPParam_25Mem GetConfVmpParam(TEqp tVmpEqp) { return m_cVmpInfoMgr.GetConfVmpParam(tVmpEqp); }
	void SetConfVmpParam(TEqp tVmpEqp, TVMPParam_25Mem tConfVmpParam) { m_cVmpInfoMgr.SetConfVmpParam(tVmpEqp, tConfVmpParam); }
	TVMPParam_25Mem GetLastVmpParam(TEqp tVmpEqp) { return m_cVmpInfoMgr.GetLastVmpParam(tVmpEqp); }
	void SetLastVmpParam(TEqp tVmpEqp, TVMPParam_25Mem tLastVmpParam) { m_cVmpInfoMgr.SetLastVmpParam(tVmpEqp, tLastVmpParam); }
	TKDVVMPOutParam GetVMPOutParam(TEqp tVmpEqp) { return m_cVmpInfoMgr.GetVMPOutParam(tVmpEqp); }
	void SetVMPOutParam(TEqp tVmpEqp, TKDVVMPOutParam tVMPOutParam) { m_cVmpInfoMgr.SetVMPOutParam(tVmpEqp, tVMPOutParam); }
	TVmpChnnlInfo GetVmpChnnlInfo(TEqp tVmpEqp) { return m_cVmpInfoMgr.GetVmpChnnlInfo(tVmpEqp); }
	void SetVmpChnnlInfo(TEqp tVmpEqp, TVmpChnnlInfo tVmpChnnlInfo) { m_cVmpInfoMgr.SetVmpChnnlInfo(tVmpEqp, tVmpChnnlInfo); }
	TVmpPriSeizeInfo GetVmpPriSeizeInfo(TEqp tVmpEqp) { return m_cVmpInfoMgr.GetVmpPriSeizeInfo(tVmpEqp); }
	void SetVmpPriSeizeInfo(TEqp tVmpEqp, TVmpPriSeizeInfo tVmpPriSeizeInfo) { m_cVmpInfoMgr.SetVmpPriSeizeInfo(tVmpEqp, tVmpPriSeizeInfo); }
	BOOL32 IsBrdstVMP(TEqp tVmpEqp) { return m_cVmpInfoMgr.GetConfVmpParam(tVmpEqp).GetVMPMode() != CONF_VMPMODE_NONE && m_cVmpInfoMgr.GetConfVmpParam(tVmpEqp).IsVMPBrdst(); }
	void SetVMPBrdst(TEqp tVmpEqp, u8 byVMPBrdst) { m_cVmpInfoMgr.SetVMPBrdst(tVmpEqp, byVMPBrdst); }
	u8 GetVMPMode(TEqp tVmpEqp) { return m_cVmpInfoMgr.GetConfVmpParam(tVmpEqp).GetVMPMode(); }
	void SetVMPMode(TEqp tVmpEqp, u8 byMode) { m_cVmpInfoMgr.SetConfVmpMode(tVmpEqp, byMode); }
	void ClearVmpInfo(TEqp tVmpEqp) { m_cVmpInfoMgr.ClearVmpInfo(tVmpEqp); }
	u16 GetVMPOutChlBitrate(TEqp tVmpEqp, u8 byIdx) {return m_cVmpInfoMgr.GetVMPOutChlBitrate(tVmpEqp, byIdx);}
	void SetVMPOutChlBitrate(TEqp tVmpEqp, u8 byIdx, u16 wBr) { m_cVmpInfoMgr.SetVMPOutChlBitrate(tVmpEqp, byIdx, wBr); }
	void ClearVmpAdpHdChl(TEqp tVmpEqp) { m_cVmpInfoMgr.ClearVmpAdpHdChl(tVmpEqp); }
	u8 GetVmpAdpMaxStyleNum(TEqp tVmpEqp) { return m_cVmpInfoMgr.GetVmpAdpMaxStyleNum(tVmpEqp); }
	u8 GetVmpAdpHDChlNum(TEqp tVmpEqp) { return m_cVmpInfoMgr.GetVmpAdpHDChlNum(tVmpEqp); }
	u8 GetVmpAdpMaxNumHdChl(TEqp tVmpEqp) { return m_cVmpInfoMgr.GetVmpAdpMaxNumHdChl(tVmpEqp); }
	u8 GetVmpAdpChnlCountByMt(TEqp tVmpEqp, TMt tMt) { return m_cVmpInfoMgr.GetVmpAdpChnlCountByMt(tVmpEqp, tMt); }
/*--------------------------------------------------------------*/

//    u8     GetIdlePrsChl( u8 &byEqpId, u8 &byChlId, u8 &byChlId2, u8 &byChlIdAud );
//	BOOL32 GetIdlePrsChls( u8 byChannelNum, TPrsChannel& tPrsChannel);
	//prs整合
	BOOL32 GetIdlePrsChls(u8 byNeedChnNum, u8 *pbyPrsId = NULL, u8 *pbyPrsChnId = NULL);
	u8     GetAllIdlePrsChnsNum();

	u8     GetMaxIdleChlsPrsId(u8& byEqpPrsId );
//	BOOL32 GetIdlePrsChls(u8 byPrsId, u8 byChannelNum, TPrsChannel& tPrsChannel);
    u8     GetIdlePrsChl( u8 &byEqpId, u8 &byChlId );
    BOOL32 IsRecorderOnline (u8 byEqpId);
	void   SetEqpAlias( u8 byEqpId, LPCSTR lpszAlias );
	LPCSTR GetEqpAlias( u8 byEqpId );
//    BOOL32 IsEqpH263pMau( u8 byEqpId );

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

	
	void UpdateAgentEqpStat(u8 byEqpType);
	void UpdateAgentAuthMtNum();


	//外设类型操作
	u8 GetHduChnNumAcd2Eqp(const TEqp tEqp);
	BOOL32 Set8kxMixerCfg(TEqpRegAck &tRegAck);
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

	//适配整合
	//获取适配器输入通道数
	u8     GetBasInPutChnNum(const TEqp &tEqp);
	//设置对应适配通道参数
// 	BOOL32 SetBasAdaptParam(const TConfInfo& tConfInfo,
// 							const TEqp &tEqp, u8 byChnId, 
// 							THDAdaptParam tHDParm, BOOL32 bForceSet = FALSE, BOOL32 bAdjRes = FALSE);
	//清空对应适配通道参数
	BOOL32 ClearAdaptParam(const TEqp &tEqp, u8 byChnId, u8 byMediaMode);
	//获取对应适配通道参数
	BOOL32 GetBasOutPutParam(const TEqp &tEqp, u8 byChnId, u8 &byOutNum,  THDAdaptParam *ptParam);
	BOOL32 GetAudBasOutPutParam(const TEqp &tEqp, u8 byChnId, u8 &byOutNum,  TAudAdaptParam *ptParam);
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
	BOOL32 GetMcsRegInfo( u16 wMcInstId, TMcsRegInfo *ptMcsRegInfo );
	u16    GetMcIns(u32 dwMcIp, u32 dwMcSSrc, u8 byConfSource = MCS_CONF);

	/************************************************************************/
    /*                                                                      */
    /*                     6B、监控表操作函数								*/
    /*                                                                      */
    /************************************************************************/
	BOOL32 GetMonitorSrc( u16 wMcInstId, u8 byMode, const TTransportAddr &tTransportAddr, TMt * ptSrc );
	BOOL32 SetMonitorSrc( u16 wMcInstId, u8 byMode, const TTransportAddr &tTransportAddr, const TMt &tSrc);
	BOOL32 GetMonitorData( u16 wMcInstId, u16 wIdx, TMonitorData &tMonitorData);
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
	void	ReAssignMpForEqp(u8 byEqpId);
	void	AssignNewMpToReplace(u8 byOldMpId);
	u8		GetNextValidMp(u8 byMpId);
	void	ChkAndRefreshMpForEqp(u8 byEqpId);
    
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
//	void IncMtAdpMtNum( u8 byDriId, u8 byConfIdx, u8 byMtId, u16 wIncExtraNum = 0);
    void IncMtAdpMtNum( u8 byDriId, u8 byConfIdx, u8 byMtId, u8 byMtType = TYPE_MT);
	void DecMtAdpMtNum( u8 byDriId, u8 byConfIdx, u8 byMtId, BOOL32 bHDMt = FALSE
		                /*BOOL32 bOnlyAlterNum = FALSE, BOOL32 bIsMcu = FALSE */);

	BOOL32 IncAudMtAdpNum(u8 byConfIdx, u8 byMtId, u8 byMtHasVidCap, u16& wErrorCode);
	BOOL32 DecAudMtAdpNum(u8 byConfIdx, u8 byMtId);

	void ChangeDriRegConfNum( u8 byDriId, BOOL32 bInc  );
	u8   GetRegConfDriId( void );
    u8   AssignH323MtDriId( u8 byConfIdx, TMtAlias &tMtAlias, u8 byMtId );
	u8   AssignH320MtDriId( u8 byConfIdx, TMtAlias &tMtAlias, u8 byMtId );
#if	defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	u8   AssignH323MtDriIdFor8KE( u8 byConfIdx, TMtAlias &tMtAlias, u8 byMtId );
#endif
	void ShowDri( u8 byDriId );
    BOOL32 IsMtAssignInDri( u8 byDriId, u8 byConfIdx, u8 byMtId );

	u16	 GetMpcHDAccessNum();

	BOOL32 IncMpcCurrentHDMtNum (u8 byConIdx, u8 byMtId,BOOL32 bIsMcu = FALSE );
	BOOL32 DecMpcCurrentHDMtNum (u8 byConIdx, u8 byMtId,BOOL32 bIsMcu = FALSE );
	BOOL32 IsOccupyHDAccessPoint (u8 byConIdx, u8 byMtId);
	BOOL32 IsOccupyAudAccessPoint(u8 byConIdx, u8 byMtId);

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
  
	void   SetRRQDriTransed( BOOL32 bTrans ) { m_bRRQDriTransferred = bTrans;   }
    BOOL32 GetRRQDriTransed( void ) { return m_bRRQDriTransferred;   }
   
    /************************************************************************/
    /*                                                                      */
    /*                     12、Debug调试及能力开关信息                      */
    /*                                                                      */
    /************************************************************************/   
    
	// 1. 获取基本调试信息
	void   GetBaseInfoFromDebugFile( );
	BOOL32 IsTelnetEnable();
	u16    GetMcuTelnetPort( );
	u16    GetMcuListenPort( );
	u32    GetMcsRefreshInterval( );

	u32	   GetRefreshMtInfoInterval( ) const { return m_tMcuDebugVal.m_dwRefressMtInfoInterval; }
	u32	   GetRefreshSMcuMtInterval( ) const { return m_tMcuDebugVal.m_dwRefreshSMcuMtInterval; }
	u32	   GetRefreshMtAdpSMcuListInterval() const { return m_tMcuDebugVal.m_dwRefreshMtAdpSMcuList; }
	
	u8	   GetMcuMcsMtStatusBufFlag( ) const { return m_tMcuDebugVal.m_byEnableMcuMcsMtStatusBuf; }
	u8	   GetMcuMcuMtStatusBufFlag( ) const { return m_tMcuDebugVal.m_byEnableMcuMcuMtStatusBuf; }
	u8	   GetMtAdpSMcuListBufFlag( ) const { return m_tMcuDebugVal.m_byEnableMtAdpSMcuListBuf; }
	u8	   GetMcuMcsMtInfoBufFlag( ) const { 
		// 终端状态必须在终端信息和列表之后发送! 所以如果没有启用终端列表的缓冲发送, 则不能启用状态信息和列表的缓冲发送
		u8 byFlagRet = m_tMcuDebugVal.m_byEnableMcuMcsMtInfoBuf 
			& m_tMcuDebugVal.m_byEnableMcuMcsMtStatusBuf;
		return byFlagRet; 
	}
	
	BOOL32 IsWatchDogEnable( );
    BOOL32 GetMSDetermineType( );
	u16	   GetMsSynTime(void);	//获得主备同步时间, zgc, 2007-04-02
    BOOL32 GetBitrateScale();

    BOOL32 GetDri2E1CfgTableFromFile();
    BOOL32 GetDri2E1CfgTable( TDri2E1Cfg* tDri2E1CfgTable );
	void SaveRealBandwidth(u16 wRealBandWidth, u8 byIdx, u8 byE1Num);
    u32 GetCfgIpByIdx( u8 byIndx );
	BOOL32 GetRealBandwidthByIp(u32 dwIp, u16 &wRealBandwidth);

	
	BOOL32 GetFakeCapParamByMtInfo(u32 dwIp, s8* pszProductId,u8 byManuId,TSimCapSet &tMainCapSet, TDStreamCap &tDSCapSet,TAudioTypeDesc & tAudioDesc, u8& byFakeMask);
	BOOL32 GetFakeCapIdByMtInfo(u32 dwIp, s8* pszProductId,u8 byManuId, u8 &byMainCapId, u8 &byDualCapId,u8 & byAudioCapId);

    //获取打洞时间间隔
    u32 GetPinHoleInterval(void){ return m_tMcuDebugVal.m_dwPinHoleInterval; }
   
    // 2. 获取关于指定终端的协议适配板资源及码流转发板资源的设置
	BOOL32 GetMtCallInterfaceInfoFromDebugFile( );
	BOOL32 GetMpIdAndH323MtDriIdFromMtAlias( TMtAlias &tMtAlias, u32 &dwMtadpIpAddr, u32 &dwMpIpAddr );
	    
    
    // 3连接级联SMcu的端口（下级的侦听端口）
    BOOL32 IsMMcuSpeaker() const;
    BOOL32 IsAutoDetectMMcuDupCall() const;
    BOOL32 IsShowMMcuMtList() const;
	BOOL32 IsStartSwitchToMMcuWithOutCasBas() const;//[03/04/2010] zjl modify (若无适配资源，直接交换码流到上级mcu)
    u8     GetCascadeAliasType() const;
	u8     GetCascadeAdminLevel( ) const;
    void   SetSMcuCasPort(u16 wPort);
    u16    GetSMcuCasPort() const;

    // 4. 通用参数        
	BOOL32 IsApplyChairToZxMcu() const;
    BOOL32 IsTransmitMtShortMsg() const;
    BOOL32 IsChairDisplayMtApplyInfo() const;
    BOOL32 IsSelInDoubleMediaConf() const;
    BOOL32 IsLimitAccessByMtModal() const;
	u8	   GetDbgConfFPS() const;
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
    u8     GetBandWidthReserved4HdBas() const;
    u8     GetBandWidthReserved4HdVmp() const;    
	BOOL32 IsSupportChgLGCResAcdBR() const;
	u16    Get1080P6050FpsBpLeastBR() const;
	u16    Get1080P3025FpsBpLeastBR() const;
	u16	   Get720P6050FpsBpLeastBR() const;
	u16    Get720P3025FpsBpLeastBR() const;
	u16    Get1080P6050FpsHpLeastBR() const;
	u16    Get1080P3025FpsHpLeastBR() const;
	u16    Get720P6050FpsHpLeastBR() const;
	u16    Get720P3025FpsHpLeastBR() const;
    u16    Get4CifLeastBR() const;
    u16    GetCifLeastBR() const;
	BOOL32 GetFakeCap2HuaweiSDEp( const u32 dwMtIp,u8 &byVideoFormat ) const;
	
	u8     GetVcsMtOverTimeInterval() const;//获取vcs会议呼叫超时时间
	BOOL32 IsVASimultaneous() const;
	
	u16		GetMmcuOlcTimerOut() const;		//获取开级联逻辑通道超时时间
	u8		GetFastUpdateToSpeakerNum() const;
    // 5. 能力限制部分
    BOOL32 SetLicenseNum( u16 wLicenseValue );
    u16    GetLicenseNum( void );

	BOOL32 SetAudMtLicenseNum(u16 wAudLicenseValue);
	u16    GetAudMtLicenseNum(void);
	u16    GetMpcCurrentAudMtAdpNum(void);

	BOOL32 SetVCSAccessNum( u16 wAccessNum );
	u16    GetVCSAccessNum( void );
	u8     GetMaxMcsOngoingConfNum(void) const;
	
	BOOL32	SetLicenseHDMtNum(u16 wLicenseValue);
	u16		GetLicenseHDMtNum(void);
	BOOL32	SetLicensePCMtNum(u16 wLicenseValue);
	u16		GetLicensePCMtNum(void);
	
    void   SetMcuExpireDate( TKdvTime &tExpireDate );
    TKdvTime GetMcuExpireDate( void );
    BOOL32 IsMcuExpiredDate(void);

    BOOL32 IsMpcRunMp();
    TMcuPerfLimit& GetPerfLimit();
    u8 GetFpsAdp();
	BOOL32 IsChairAutoSpecMix();

	//[add][liulijiu][2010.07.15]
	BOOL32 ReadLicenseFromUSBKEY(u16 &wErrorCode);    //验证USBKEY里的授权数据是否正确
	void   SetLicesenUSBKEYFlag(void);                //设置USBKEY flag
	BOOL32 GetLicesenUSBKEYFlag(u16 &wErrorCode);     //得到USBKEY flag
	//[add][liulijiu][2010.07.15] end

    BOOL32 IsMtNumOverLicense( );         //是否超过License控制的最大终端接入能力
	BOOL32 IsVCSNumOverLicense(void);     //是否超过License控制的最大VCS接入能力
    BOOL32 IsConfNumOverCap( BOOL32 bOngoing, u8 byConfSource = MCS_CONF );           //会议数量是否超限
    u8     GetMcuCasConfNum();                            //获取级联会议总数
    BOOL32 IsCasConfOverCap();                            //级联会议是否超限
	//  [12/22/2010 chendaiwei]Fix me: 该函数应该只用于返回某个DRI MtAdp上的终端或者Mcu数量
    BOOL32 GetMtNumOnDri( u8 byDriId, BOOL32 bOnline, u8 &byMtNum, u8 &byMcuNum );    //获得某个DRI MtAdp上的终端或者Mcu数量，返回是否超限
	//  [12/22/2010 chendaiwei] 获得某个DRI MtAdp上的终端或者Mcu数量，返回是否超限
	BOOL32 IsMtNumOverDriLimit( u8 byDriId, BOOL32 bOnline, u8 &byMtNum, u8 &byMcuNum );    
	u16	   GetPcmtAccessPointNum(void);
	u16	   GetAccessPointNum( void );
	BOOL32 IsMtNumOverMcuLicense(u8 byConfIdx, TMt tMt, const TCapSupport &tCapSupport, u16 &wErrorCode);
	BOOL32 IsHdMtNumOverMcuLicense( u8 byConfIdx, const TMt &tMt, const TCapSupport &tCapSupport, u16 &wErrorCode);
	u8 GetMixerWaitRspTimeout(void);
	u8 GetDelayReleaseConfTime(void);
	u8 GetMtFastUpdateInterval( void ) const;
	u8 GetMtFastUpdateNeglectNum( void ) const;
	BOOL32 IsPolyComMtCanSendDoubleStream( void ) const;
	BOOL32 IsChaoRanMtAllowInVmpChnnl( void ) const;
	BOOL32 IsSendStaticText( void ) const;

	
	BOOL32 IsNeedMapDstIpAddr(u32 dwDstIpAddr)const; //交换的目的ip是否需要（作假）设置mapip和端口
	BOOL32 IsNeedSendSelfMutePack(u8 byManuId,const s8* pProductId);//对应的manuid和productid是否需要给自己发哑音包
	u8 GetSendMutePackNum ( void ) const;
	u8 GetSendMutePackInterval ( void ) const;
	BOOL32 IsRcvH264DependInMark(u8 byManuId,const s8* pProductId);//对应的manuid和productid是否需要按特殊方式解码
	BOOL32 IsReverseG7221c(u8 byManuId,const s8* pProductId);//对应的manuid和productid是否需要对g7221.c音频格式码流内容奇偶对换

	u16 GetCurrentAudMtNum (void);

    BOOL32 IsMultiCastMtIp(u32 dwIpAddr)const; //是否接收组播码流

    // 6. 打印
    void ShowDebugInfo();
    void ShowBasInfo();
	void ShowBasList();
	void ShowTW(u8 byHduId);
	void ShowPrsInfo();
	BOOL32 IsCanConfCapabilityCheat( void );
	
	BOOL32 IsMtBoardTypeCanCapabilityCheat( s8* pachProductID );

	BOOL32 IsHDMt( s8* pachProductID );

	BOOL32 IsLowCalledMtChlOpenMp4( void );
	BOOL32 IsLowCalledMtChlOpenH264( void );
    
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
    //sgx
	BOOL32 VcsReloadMcuUserList( void );  
	
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

	//抓包相关
#if defined(_8KH_) ||  defined(_8KE_) || defined(_8KI_)

	void SetNetCapStatus( const emNetCapState& emNetCapStatus);
	emNetCapState GetNetCapStatus()const;

	void SetOprNetCapMcsInstID(const u8& byMcsInstId);

	u8 GetOprNetCapMcsInstID()const;

	BOOL32 StartNetCap(const u8& byMcsInstId);

	BOOL32 StopNetCap();
#endif

public:
	BOOL32 MsgPassCheck( u16 wEvent, u8 * const pbyMsg, u16 wLen = 0 );
	void   BroadcastToAllConf( u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
	BOOL32 SendMsgToConf( u8 byConfIdx, u16 wEvent, u8 * const pbyMsg, u16 wLen );
	BOOL32 SendMsgToConf( const CConfId & cConfId, u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
	BOOL32 SendMsgToDaemonConf( u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
	BOOL32 SendMsgToEqp( u8 byEqpId,  u16 wEvent, u8 * const pbyMsg, u16 wLen );
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
	BOOL32 GetVcDeamonOtherData( u8 *pbyBuf, u32 &dwOutBufLen );
	BOOL32 SetVcDeamonOtherData( u8 *pbyBuf );

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
    BOOL32 GetAddrbookFileDataUtf8( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen, BOOL32 bPrintErr );
	BOOL32 SetAddrbookFileDataUtf8( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bPrintErr );
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

	void ConstructCriMacInfoMsg(CServMsg & cMsg); //构造接入板MAC地址信息消息体[5/16/2012 chendaiwei]
	BOOL32 FindMatchedMpIpForMt(u32 dwMtIp, u32 &dwMpIp); //查找Mt所挂转发板的IP地址（该IP地址应与Mt IP同网段，针对8000H/G/I）[5/25/2012 chendaiwei]
	
private: 
	u32  OprVcDeamonOtherData( u8 *pbyBuf, BOOL32 bGet );
//	u32  GetVcDeamonOtherDataLen( void );


    /************************************************************************/
    /*                                                                      */
    /*                     15.N+1相关管理
    /*                                                                      */
    /************************************************************************/ 
public:
	BOOL32 NplusSaveVmpModuleByConfIdx ( u8 byConfIdx, TNplusVmpModule *ptVmpMod, u8 byModNum);
	void NplusRemoveVmpModuleByConfIdx ( u8 byConfIdx );
	void NplusRemoveVmpModuleEqpId ( u8 byEqpId );
	BOOL32 NPlusIsMtInVmpModule(u8 byLoopChIx, u8 byMtId, u8 byConfIdx, u8 byEqpId, u8 &byMember);
	void NplusRemoveVmpMember( u8 byMtId,u8 byConfIdx,u8 byEqpId = 0);
	BOOL32 NPlusIsVmpOccupyByConf (u8 byEqpId,u8 byConfIdx);
	void GetNPlusVmpMember(u8 byEqpId,u8 byChIdx,u8 &byMtId,u8 &byMemberType);
};
#endif

//END OF FILE





