/*****************************************************************************
  模块名      : libapu2.a
  文件名      : apu2cfg.h
  相关文件    : 
  文件实现功能: apu2配置拓展
  作者        : 周嘉麟
  版本        : V1.0  Copyright(C) 2009-2010 KDC, All rights reserved.
-----------------------------------------------------------------------------*/
#ifndef _APU2_CFG_H_
#define _APU2_CFG_H_

#include "eqpbase.h"

#define  MAXNUM_APU2_BAS		(u8)1				//APU2最大配置适配器				
#define  MIXER_APU2_CHN_MINNUM  (u8)2				//APU2最小配置通道数
#define  BRD_APU2_SNDPORT       (u16)10000          //APU2本地起始发送端口

	/************************************************************************/
	/*							CApu2Cfg配置类定义							*/
	/*																		*/
	/*        说明：一个Apu2拆成2个混音器，每个混音器配置对应CApu2Cfg		*/
	/************************************************************************/
class CApu2MixerCfg:public CEqpCfg
{
public:
	CApu2MixerCfg():m_byMemberNum(0),
					m_bySndOff(0),
					m_byRcvOff(0),
					m_byIsSimuApu(0),
					m_wSimApuVer(0),
					m_byQualityLvl(0)
	{
	
	};
	~CApu2MixerCfg()
	{	
		Clear();
	};
public:

	void Clear()
	{
		m_byMemberNum     = 0;
		m_bySndOff        = 0;
		m_byRcvOff        = 0;
		m_byIsSimuApu     = 0; 
		m_wSimApuVer      = 0;
		m_byQualityLvl	  = 0;
	}

	void Copy (const CApu2MixerCfg* const pcCfg)
	{
		if (NULL == pcCfg)
		{
			return;
		}
		
		m_byMemberNum      = pcCfg->m_byMemberNum;
		m_bySndOff         = pcCfg->m_bySndOff;
		m_byRcvOff		   = pcCfg->m_byRcvOff;
		m_byIsSimuApu	   = pcCfg->m_byIsSimuApu;
		m_wSimApuVer       = pcCfg->m_wSimApuVer;
		m_byQualityLvl	   = pcCfg->m_byQualityLvl;

		m_dwMcuNodeA       = pcCfg->m_dwMcuNodeA;
		m_dwMcuIIdA        = pcCfg->m_dwMcuIIdA;
		m_dwMcuNodeB       = pcCfg->m_dwMcuNodeB;
		m_dwMcuIIdB		   = pcCfg->m_dwMcuIIdB;
		m_dwServerNode     = pcCfg->m_dwServerNode;
		m_dwServerIId      = pcCfg->m_dwServerIId;
		m_dwMpcSSrc		   = pcCfg->m_dwMpcSSrc;
		m_dwMcuRcvIp       = pcCfg->m_dwMcuRcvIp;
		m_wMcuRcvStartPort = pcCfg->m_wMcuRcvStartPort;
		m_wEqpVer		   = pcCfg->m_wEqpVer;
		m_byRegAckNum      = pcCfg->m_byRegAckNum;
		memcpy(&m_tPrsTimeSpan, &pcCfg->m_tPrsTimeSpan, sizeof(m_tPrsTimeSpan));
		memcpy(&m_tQosInfo, &pcCfg->m_tQosInfo, sizeof(m_tQosInfo));

		
		byEqpId			   = pcCfg->byEqpId;
		byEqpType          = pcCfg->byEqpType;
		dwLocalIP		   = pcCfg->dwLocalIP;
		dwConnectIP		   = pcCfg->dwConnectIP;
		wConnectPort	   = pcCfg->wConnectPort;
		wMcuId			   = pcCfg->wMcuId;
		wRcvStartPort	   = pcCfg->wRcvStartPort;
		wMAPCount		   = pcCfg->wMAPCount;
		dwConnectIpB	   = pcCfg->dwConnectIpB;
		wConnectPortB	   = pcCfg->wConnectPortB;
		wRcvStartPortB     = pcCfg->wRcvStartPortB;
		memcpy(achAlias, pcCfg->achAlias, MAXLEN_ALIAS);
		memcpy(m_atMap, pcCfg->m_atMap, sizeof(m_atMap));
		return;
	}

	void Print()
	{
		CEqpCfg::Print();
		StaticLog("\nMix: \n");
		StaticLog("\t MemberNum		: %d\n", m_byMemberNum);
		StaticLog("\t m_bySndOff		: %d\n", m_bySndOff);
		StaticLog("\t m_byRcvOff		: %d\n", m_byRcvOff);
		StaticLog("\t m_byIsSimuApu		: %d\n", m_byIsSimuApu);
		StaticLog("\t m_wSimApuVer		: %d\n", m_wSimApuVer);
		StaticLog("\t m_byQualityLvl		: %d\n", m_byQualityLvl);
	}
public:	
	u8     m_byMemberNum;		//配置成员个数
	u8     m_bySndOff;			//发送对象偏移
	u8	   m_byRcvOff;			//接收对象偏移
	u8     m_byIsSimuApu;       //是否模拟Apu
	u16    m_wSimApuVer;		//模拟Apu版本
	u8	   m_byQualityLvl;		//音质等级
	
};

class CApu2BasCfg : public CEqpCfg
{
public:
	CApu2BasCfg():m_byQualityLvl(0)
	{
		
	};
	~CApu2BasCfg()
	{	
		Clear();
	};
public:
	
	void Clear()
	{
		m_byQualityLvl	  = 0;
	}
	
	void Copy (const CApu2BasCfg* const pcCfg)
	{
		if (NULL == pcCfg)
		{
			return;
		}
		
		m_byQualityLvl	   = pcCfg->m_byQualityLvl;
		
		m_dwMcuNodeA       = pcCfg->m_dwMcuNodeA;
		m_dwMcuIIdA        = pcCfg->m_dwMcuIIdA;
		m_dwMcuNodeB       = pcCfg->m_dwMcuNodeB;
		m_dwMcuIIdB		   = pcCfg->m_dwMcuIIdB;
		m_dwServerNode     = pcCfg->m_dwServerNode;
		m_dwServerIId      = pcCfg->m_dwServerIId;
		m_dwMpcSSrc		   = pcCfg->m_dwMpcSSrc;
		m_dwMcuRcvIp       = pcCfg->m_dwMcuRcvIp;
		m_wMcuRcvStartPort = pcCfg->m_wMcuRcvStartPort;
		m_wEqpVer		   = pcCfg->m_wEqpVer;
		m_byRegAckNum      = pcCfg->m_byRegAckNum;
		memcpy(&m_tPrsTimeSpan, &pcCfg->m_tPrsTimeSpan, sizeof(m_tPrsTimeSpan));
		memcpy(&m_tQosInfo, &pcCfg->m_tQosInfo, sizeof(m_tQosInfo));
		
		
		byEqpId			   = pcCfg->byEqpId;
		byEqpType          = pcCfg->byEqpType;
		dwLocalIP		   = pcCfg->dwLocalIP;
		dwConnectIP		   = pcCfg->dwConnectIP;
		wConnectPort	   = pcCfg->wConnectPort;
		wMcuId			   = pcCfg->wMcuId;
		wRcvStartPort	   = pcCfg->wRcvStartPort;
		wMAPCount		   = pcCfg->wMAPCount;
		dwConnectIpB	   = pcCfg->dwConnectIpB;
		wConnectPortB	   = pcCfg->wConnectPortB;
		wRcvStartPortB     = pcCfg->wRcvStartPortB;
		memcpy(achAlias, pcCfg->achAlias, MAXLEN_ALIAS);
		memcpy(m_atMap, pcCfg->m_atMap, sizeof(m_atMap));
		return;
	}
	
	void Print()
	{
		CEqpCfg::Print();
		StaticLog("\nBas: \n");
		StaticLog("\t m_byQualityLvl		: %d", m_byQualityLvl);
	}
public:	
	u8	   m_byQualityLvl;		//音质等级
};

	/************************************************************************/
	/*							CApu2Cfg配置类定义							*/
	/*																		*/
	/*					说明：一个Apu2设备对应TApu2EqpCfg					*/
	/************************************************************************/
struct TApu2EqpCfg
{
public:
	BOOL32        m_bIsProductTest;					   //是否生产测试
	u8		      m_byMixerNum;						   //Apu2设备配了几个混音器
	CApu2MixerCfg m_acMixerCfg[MAXNUM_APU2_MIXER];	   //每个混音器配置
	u8			  m_byBasNum;						   //APU2设备配了几个适配器
	CApu2BasCfg	  m_acBasCfg[MAXNUM_APU2_BAS];		   //每个适配器配置
public:
	TApu2EqpCfg()
	{
		m_bIsProductTest = FALSE;
		m_byMixerNum     = 0;
	}
};

#endif