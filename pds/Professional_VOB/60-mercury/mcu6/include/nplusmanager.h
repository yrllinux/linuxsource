/*****************************************************************************
   模块名      : N+1备份管理模块
   文件名      : nplusmanager.h
   相关文件    : nplusmanager.cpp
   文件实现功能: N+1备份管理
   作者        : 许世林
   版本        : V4.0  Copyright(C) 2006-2009 KDCOM, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2006/11/21  4.0         许世林      创建
******************************************************************************/
#ifndef _NPLUS_MANAGER_H
#define _NPLUS_MANAGER_H

#include "radiusinterface.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef PACKED
#undef PACKED
#endif

#ifdef WIN32
    #pragma comment( lib, "ws2_32.lib" ) 
    #pragma pack( push )
    #pragma pack( 1 )
    #define window( x )	x
    #define PACKED 
#else
    #include <netinet/in.h>
    #define window( x )
#if defined(__ETI_linux__)
    #define PACKED
#else
    #define PACKED __attribute__((__packed__))	// 取消编译器的优化对齐
#endif
#endif

#define NPLUS_CONNECTMCU_TIMEOUT                3000
#define NPLUS_REGMCU_TIMEOUT                    3000
#define NPLUS_DATAUPDATE_TIMEOUT                3000

#define NPLUS_CONF_START                        1       //会议开始
#define NPLUS_CONF_RELEASE                      2       //开始

#define NPLUS_MAXNUM_REGNACKBYREMOTECAP         20      //20次，每次时间 NPLUS_REGMCU_TIMEOUT

#define NPLUS_MAX_BASCHN_NUM					(7*16) //目前最大BAS通道数 （16个BAS外设（MPU2-Eccard enhanced bas 7个通道） 
   
// N+1 工作状态机
enum ENPlusState
{
    MCU_NPLUS_IDLE,             //没有配置N+1模式
    MCU_NPLUS_MASTER_IDLE,      //N+1模式主mcu状态
    MCU_NPLUS_MASTER_CONNECTED, //N+1模式主mcu且已连接备份服务器状态
    MCU_NPLUS_SLAVE_IDLE,       //N+1模式备份mcu状态
    MCU_NPLUS_SLAVE_SWITCH      //N+1模式备份mcu切换后状态
};


enum emNPlusCommunicateType
{
	emAPU2MixerNum = 1,
	emMPU2BasicBasNum,
	emMPU2EnhancedBasNum,
	emMPU2BasicVmpNum,
	emMPU2EcardBasicVmpNum,
	emMPU2EnhancedVmpNum,
	em8000GBasNum,
	em8000HBasNum,
    emNplusMcuInfoEx,               // [3/18/2013 liaokang] 支持主备N+1备双备份
	emAPU2BasNum,	
	em8000IAudBasNum,				//8000I音频BAS外设数目
	em8000IVidBasNum,				//8000I视频BAS外设数目
	em8000IMixerNum,				//8000I混音器数目
};

// 如果emNPlusCommunicateType修改,则此宏要随之修改 [3/18/2013 liaokang] 支持主备N+1备双备份
// em8000IAudBasNum+em8000IVidBasNum+em8000IMixerNum[7/29/2013 chendaiwei]
#define NPLUS_PACK_EXINFO_BUF_LEN                (sizeof(u16)+sizeof(TNPlusEqpCapEx)+sizeof(TNPlusMcuInfoEx)+sizeof(u8)+sizeof(u8)+sizeof(u8)+sizeof(u8)+sizeof(u8)*3*13)

//编码能力描述
enum emEncAbility
{
	emh263Plus = 0x00000001,
	emOther = 0x00000002,
	emcifBP = 0x00000004,
	emcifHP = 0x00000008,
	em4cifBP = 0x00000010,
	em4cifHP = 0x00000020,
	em7202530BP = 0x00000040, //720 25/30帧
	em7202530HP = 0x00000080,
	em7205060BP = 0x00000100,//720 50/60帧
	em7205060HP = 0x00000200,
	emXgaBP = 0x00000400,
	emXgaHP = 0x00000800,
	emSxga30BP = 0x00001000,
	emSxga30HP = 0x00002000,
	emUxga60BP = 0x00004000,
	emUxga60HP = 0x00008000,
	em10802530BP = 0x00010000,
	em10802530HP = 0x00020000,
	em10805060BP = 0x00040000,
	em10805060HP = 0x00080000,	

	emSxga20BP = 0x00100000,	
};

//解码能力描述
enum emDecAbility
{
	emDecSD = 1,
	emDecHD30,
	emDecHD60,
	emDecHP,
};

//Bas通道类型 基本按能力从小到大排序
enum emBasChnType
{
	//视频BAS通道
	emVpuChn = 1,
	em8KGSelChn,
	em8KHSelChn,
	emMAUH263PlusChn,
	em8KIVidBasChn2, //8000I视频BAS通道2
	em8KHDSChn,
	em8KGDSChn,
	emMAUNormal,
	emMPUChn,
	em8KIVidBasChn1, //8000I视频BAS通道1
	emMPU2BasicSelChn,
	emMPU2EnhancedSelChn,
	em8KGBrdChn,
	em8KHBrdChn,
	em8KIVidBasChn0, //8000I视频BAS通道0
	emMPU_HChn,	  //MPU2_H 编码不出60帧 6出
	emMPU_H60Chn, //MPU2_H 编码出60帧 3出
	emMPU2BasicDsChn,
	emMPU2BasicBrdChn,
	emMPU2EnhancedBrdChn,
	emMPU2EnhancedDsChn,

	//音频BAS通道
	em8KIAudBasChn,
	emAPU2BasChn,
};

//BAS通道能力描述
struct TBasChnAbility
{
	u8 m_byEncNum; //一个通道编码路数
	u32 m_dwAbility[6]; //目前一个通道最大出六出（MPU_H 1进6出）
	u8 m_byDecAbility;  //解码能力
	emBasChnType m_emChnType; //bas通道类型 作为能力排序依据
	u8 m_byIsOccupy;        //标识该通道是否被占用
	u8 m_byBasMediaType;	//标识是音频还是视频BAS（MODE_VIDEO,MODE_AUDIO）
public:
	TBasChnAbility( void )
	{
		memset(this,0,sizeof(TBasChnAbility));
	}

	void SetEncAbility(u8 byOutChnIdx,emEncAbility emAbility)
	{
		m_dwAbility[byOutChnIdx] = m_dwAbility[byOutChnIdx] | (u32)emAbility;
	}

	void SetDecAbility( emDecAbility emDec)
	{
		m_byDecAbility = (u8)emDec;
	}

	BOOL32 Has60FpsEncAbility(void )
	{
		for(u8 byIdx = 0; byIdx < 6; byIdx++)
		{
			if(m_dwAbility[byIdx]&em7205060BP || m_dwAbility[byIdx]&em10805060BP)
			{
				return TRUE;
			}
		}
		
		return FALSE;
	}

}PACKED;


//BAS通道链表管理类
#define		MAXNUM_MCU_BAS_CHN				(u8)112		//16*7,16代表最大外设数，7代表mpu2enhance模式
BOOL32 GetBasCapConfNeedForNPlus(const TConfInfo &tConfInfo,const TConfInfoEx &tConfInfoEx, 
						   TNeedVidAdaptData *ptReqResource,  TNeedAudAdaptData *pReqAudResource, 
						   BOOL32 bIsChn8KH = FALSE,BOOL32 bIsChn8KE = FALSE,BOOL32 bIsChn8KI = FALSE);
struct CNPlusBasChnListMgr
{
public:
	CNPlusBasChnListMgr()
	{
		m_byRealBasChNum = 0;
	}

	//增加BAS通道
	void  InsertBasChn(CBasChn *pBasChn,u8 byIdx);
	void  SortBasChn();
	void  SetRealChnNum( u8 byRealNum ){ m_byRealBasChNum = byRealNum ;}
	//为广播查找适合BAS通道
    BOOL32  CheckBasEnoughForReq(TNeedVidAdaptData *ptReqResData,TNeedAudAdaptData *ptReqAudResource); 
private:
	//对BAS通道进行排序
	BOOL32  AddBasChnBySort(CBasChn *pcBasChn);
	
	//获得视频广播BAS通道
	BOOL32  GetBasChnsForBrd(TNeedVidAdaptData *ptReqResData);

	//获得音频BAS通道
	BOOL32  GetAudBasChnsForBrd(TNeedAudAdaptData *ptReqAudResource);
	
	CBasChn*  GetOneIdleSuitableBasChn(TNeedVidAdaptData &tLeftReqResData);

	CBasChn* GetOneIdelSuitableAudBasChn(TNeedAudAdaptData &tLeftReqResData);
	
protected:
	CBasChn	*m_apcBasChn[MAXNUM_MCU_BAS_CHN];	
	u8		m_byRealBasChNum;
};

// 终端地址信息
struct TMtInfo
{
public:
    TMtInfo( void ) { memset(this, 0, sizeof(TMtInfo)); }

    void            SetMtAddr( TTransportAddr &tAddr ) { m_dwMtAddr = tAddr; }
    TTransportAddr  GetMtAddr( void ) { return m_dwMtAddr; }
    void            SetCallBitrate( u16 wBitrate ) { m_wCallBitrate = htons(wBitrate); }
    u16             GetCallBitrate( void ) { return ntohs(m_wCallBitrate); }
    BOOL32          IsNull( void ) { return (0 == m_dwMtAddr.GetIpAddr()); }
    BOOL32          operator == (TMtInfo & tMtInfo) const
    {
        TTransportAddr tAddr = tMtInfo.GetMtAddr();
        if (memcmp(&tAddr, &m_dwMtAddr, sizeof(TTransportAddr)) == 0)
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

private:
    TTransportAddr      m_dwMtAddr;
    u16                 m_wCallBitrate;

}PACKED;

//画面合成基本参数(8bytes:N+1通信)
struct TVmpBasicParam
{
public:
	u8      m_byVMPAuto;      //是否是自动画面合成 0-否 1-是
	u8      m_byVMPBrdst;     //合成图像是否向终端广播 0-否 1-是 
	u8      m_byVMPStyle;     //画面合成风格,参见mcuconst.h中画面合成风格定义
    u8      m_byVMPSchemeId;  //合成风格方案编号,最大支持5套方案,1-5
    u8      m_byVMPMode;      //图像复合方式: 0-不图像复合 1-会控或主席控制图像复合 2-自动图像复合(动态分屏与设置成员)
	u8		m_byRimEnabled;	  //是否使用边框: 0-不使用(默认) 1-使用;
	u8		m_byVMPBatchPoll; // xliang [12/18/2008] 是否是批量轮询 0-否
	u8		m_byVMPSeeByChairman;	// 是否被主席MT选看	0-否
	u8      m_byVmpSubType;         //画面合成器子类型 MPU2_VMP_ENHACED ...

public:
	TVmpBasicParam ( void ) { memset(this,0,sizeof(*this));}

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
		case VMP_STYLE_TEN_M:
			byMaxMemNum = 10;
			break;
		case VMP_STYLE_THIRTEEN:
		case VMP_STYLE_THIRTEEN_M:
			byMaxMemNum = 13;
			break;
		case VMP_STYLE_FIFTEEN:
			byMaxMemNum = 15;
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
		case VMP_STYLE_TWENTY:
			byMaxMemNum = 20;
			break;
		case VMP_STYLE_TWENTYFIVE:
			byMaxMemNum = 25;//25风格支持
			break;
		default:
			byMaxMemNum = 1;
			break;
		}
		return byMaxMemNum;
	}

	void   Print( void ) const
	{
		StaticLog("\nVMPParam:\n");
		StaticLog("m_byVMPAuto: %d\n", m_byVMPAuto);
		StaticLog("m_byVMPBrdst: %d\n", m_byVMPBrdst);
		StaticLog("m_byVMPStyle: %d\n", m_byVMPStyle);
        StaticLog("m_byVmpSchemeId: %d\n", m_byVMPSchemeId );
		StaticLog("m_byRimEnabled: %d\n", m_byRimEnabled );
        StaticLog("m_byVMPMode: %d\n", m_byVMPMode );
		StaticLog("m_byVMPBatchPoll :%d\n", m_byVMPBatchPoll );
		StaticLog("m_byVMPSeeByChairman : %d\n", m_byVMPSeeByChairman);
		StaticLog("m_byVmpSubType : %d\n", m_byVmpSubType);
	}

}PACKED;

//画面合成某通道成员描述(len:9)
struct TNPlusVmpChnlMember
{
public:
    TMtInfo     m_tMtInVmp;     //成员终端信息
    u8          m_byMemberType; //成员类型
public:
	TNPlusVmpChnlMember ( void ) 
	{
		memset(this,0,sizeof(*this));
	}

}PACKED;

//vmp info
struct TNPlusVmpInfo
{
public:
    TMtInfo     m_atMtInVmp[MAXNUM_MPUSVMP_MEMBER];    
    u8          m_abyMemberType[MAXNUM_MPUSVMP_MEMBER]; 

public:
    TNPlusVmpInfo( void ) { Clear();}

	void Clear ( void ) { memset(this, 0, sizeof(TNPlusVmpInfo));}

    u8      GetVmpMemType( TMtInfo tMtInfo, u8 byChnl ) 
    {
        if (byChnl >= MAXNUM_MPUSVMP_MEMBER)	// xliang [2/23/2009] 有风险
        {
            return VMP_MEMBERTYPE_MCSSPEC;
        }

        if (m_atMtInVmp[byChnl] == tMtInfo)
        {
            return m_abyMemberType[byChnl];
        }        
        return VMP_MEMBERTYPE_MCSSPEC; //default
    }

    BOOL32  IsMtInVmpMem( TMtInfo tMtInfo, u8 byChnl )
    {
        if (byChnl >= MAXNUM_MPUSVMP_MEMBER)
        {
            return FALSE;
        }

        if (m_atMtInVmp[byChnl] == tMtInfo)
        {
            return TRUE;
        }
        return FALSE;
    }

}PACKED;

//25风格后五风格vmp信息
struct TNPlusVmpInfoEx
{
public:
    TMtInfo     m_atMtInVmp[MAXNUM_MPU2VMP_MEMBER-MAXNUM_MPUSVMP_MEMBER];    
    u8          m_abyMemberType[MAXNUM_MPU2VMP_MEMBER-MAXNUM_MPUSVMP_MEMBER];
	
public:
    TNPlusVmpInfoEx( void ) { memset(this, 0, sizeof(TNPlusVmpInfoEx)); }
	
    u8      GetVmpMemType( TMtInfo tMtInfo, u8 byChnl ) 
    {
        if (byChnl >= MAXNUM_MPU2VMP_MEMBER || byChnl < MAXNUM_MPUSVMP_MEMBER)	// xliang [2/23/2009] 有风险
        {
            return VMP_MEMBERTYPE_MCSSPEC;
        }
		
        if (m_atMtInVmp[byChnl-MAXNUM_MPUSVMP_MEMBER] == tMtInfo)
        {
            return m_abyMemberType[byChnl-MAXNUM_MPUSVMP_MEMBER];
        }
        
        return VMP_MEMBERTYPE_MCSSPEC; //default
    }
	
    BOOL32  IsMtInVmpMem( TMtInfo tMtInfo, u8 byChnl )
    {
        if (byChnl >= MAXNUM_MPU2VMP_MEMBER || byChnl < MAXNUM_MPUSVMP_MEMBER)
        {
            return FALSE;
        }
		
        if (m_atMtInVmp[byChnl-MAXNUM_MPUSVMP_MEMBER] == tMtInfo)
        {
            return TRUE;
        }
        return FALSE;
    }

}PACKED;

//下级MCU的E164号和IP地址信息[11/15/2012 chendaiwei]
struct TSmcuCallnfo
{
public:
	s8 m_achAlias[MAXLEN_ALIAS];  //SMCU会议或模板E164别名字符串
    TTransportAddr m_dwMtAddr;    //SMCU的IP地址
public:
	TSmcuCallnfo()
	{
		memset(this,0,sizeof(TSmcuCallnfo));
	}

	void SetE164Alias(const s8 *pszE164Alias)
	{
		if(pszE164Alias!=NULL)
			memcpy(m_achAlias,pszE164Alias,MAXLEN_ALIAS);
	}

	void SetMcuAddr(TTransportAddr &tAddr)
	{
		m_dwMtAddr = tAddr;
	}

}PACKED;

// 备份的会议信息
struct TNPlusConfData
{
public:
    TNPlusConfData( void ) { Clear(); }
    BOOL32    IsNull( void ) { return m_tConf.GetConfId().IsNull(); }
    void      Clear( void ) { memset(this, 0, sizeof(TNPlusConfData)); }

public:
    TConfInfo       m_tConf;
    TMtInfo         m_atMtInConf[MAXNUM_CONF_MT];   
    u8              m_byMtNum;
    TNPlusVmpInfo   m_tVmpInfo;
    TAcctSessionId  m_tSsnId;
    
}PACKED;

struct TNPlusEqpCapEx //len:8
{
public:
	u8 m_byAPU2MixerNum; //APU2的混音器个数
	u8 m_byMPU2BasicBasNum;
	u8 m_byMPU2EnhancedBasNum;
	u8 m_byMPU2BasicVmpNum;	//MPU2板的vmp(basic)
	u8 m_byMPU2EcardBasicVmpNum;//MPU2-Ecard板的vmp(basic)
	u8 m_byMPU2EnhancedVmpNum;
	u8 m_by8000GBasNum;
	u8 m_by8000HBasNum;
	u8 m_byAPU2BasNum;
	u8 m_by8000IVidBasNum; //8000I视频BAS数目
	u8 m_by8000IAudBasNum; //8000I音频BAS数目
	u8 m_by8000IMixerNum;  //8000I混音器数目

public:
	TNPlusEqpCapEx()
	{
		memset(this,0,sizeof(TNPlusEqpCapEx));
	}

    TNPlusEqpCapEx operator + (const TNPlusEqpCapEx &tObj) const
    {
        TNPlusEqpCapEx tCap;
		tCap.m_byAPU2MixerNum = tObj.m_byAPU2MixerNum + m_byAPU2MixerNum;
		tCap.m_byMPU2BasicBasNum = tObj.m_byMPU2BasicBasNum + m_byMPU2BasicBasNum;
		tCap.m_byMPU2EnhancedBasNum = tObj.m_byMPU2EnhancedBasNum + m_byMPU2EnhancedBasNum;
		tCap.m_byMPU2BasicVmpNum = tObj.m_byMPU2BasicVmpNum + m_byMPU2BasicVmpNum;	//MPU2板的vmp(basic)
		tCap.m_byMPU2EcardBasicVmpNum = tObj.m_byMPU2EcardBasicVmpNum + m_byMPU2EcardBasicVmpNum;//MPU2-Ecard板的vmp(basic)
		tCap.m_byMPU2EnhancedVmpNum = tObj.m_byMPU2EnhancedVmpNum + m_byMPU2EnhancedVmpNum;
		tCap.m_by8000GBasNum = tObj.m_by8000GBasNum + m_by8000GBasNum;
		tCap.m_by8000HBasNum = tObj.m_by8000HBasNum + m_by8000HBasNum;
		tCap.m_byAPU2BasNum = tObj.m_byAPU2BasNum + m_byAPU2BasNum;
		tCap.m_by8000IVidBasNum =  tObj.m_by8000IVidBasNum + m_by8000IVidBasNum; //8000I视频BAS数目
		tCap.m_by8000IAudBasNum =  tObj.m_by8000IAudBasNum + m_by8000IAudBasNum; //8000I音频BAS数目
		tCap.m_by8000IMixerNum =   tObj.m_by8000IMixerNum + m_by8000IMixerNum;	 //8000I混音器数目
		
		return tCap;
    }

	// 只比较除BAS以外的外设能力[1/12/2012 chendaiwei]
    BOOL32 operator < (const TNPlusEqpCapEx &tObj) const
    {
		u8 byCanReplaceMixerNum = 0;
		if( tObj.m_byAPU2MixerNum > m_byAPU2MixerNum )
		{
			byCanReplaceMixerNum = tObj.m_byAPU2MixerNum - m_byAPU2MixerNum;
		}
		
		if(   m_byAPU2MixerNum < tObj.m_byAPU2MixerNum
		    && (m_by8000IMixerNum <tObj.m_by8000IMixerNum+byCanReplaceMixerNum))
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS, "operator < (const TNPlusEqpCapEx &tObj) const = TRUE due to:\n" );
            LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS, "\tAPU2<%d vs %d>  8000IMixer<%d vs %d>\n", m_byAPU2MixerNum, tObj.m_byAPU2MixerNum,m_by8000IMixerNum,tObj.m_by8000IMixerNum );
			// 			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS, "\tVMP(MPU2 MPU2_Ecard)<%d vs %d>\n", m_byMPU2BasicVmpNum + m_byMPU2EcardBasicVmpNum + m_byMPU2EnhancedVmpNum, tObj.m_byMPU2BasicVmpNum + tObj.m_byMPU2EcardBasicVmpNum + tObj.m_byMPU2EnhancedVmpNum );
			// 			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS, "\tVMP(MPU2 Basic + MPU2Ecard Enhanced)<%d vs %d>\n", m_byMPU2BasicVmpNum + m_byMPU2EnhancedVmpNum, tObj.m_byMPU2BasicVmpNum + tObj.m_byMPU2EnhancedVmpNum);
			return TRUE;
		}
		
        return FALSE;
    }

	void Print ( void )
	{
		StaticLog("\tAPU2<%d>\n", m_byAPU2MixerNum);

		StaticLog("\tVMP(MPU2)<%d>\n", m_byMPU2BasicVmpNum);
		StaticLog("\tVMP(MPU2_Ecard basic)<%d>\n",m_byMPU2EcardBasicVmpNum );
		StaticLog("\tVMP(MPU2Ecard Enhanced)<%d>\n", m_byMPU2EnhancedVmpNum);

		StaticLog("\t8000GBasNum<%d>\n", m_by8000GBasNum);
		StaticLog("\t8000HBasNum<%d>\n", m_by8000HBasNum);
		StaticLog("\tMPU2BasicBasNum<%d>\n", m_byMPU2BasicBasNum);
		StaticLog("\tMPU2EnhancedBasNum<%d>\n", m_byMPU2EnhancedBasNum);
		StaticLog("\tAPU2BasNum<%d>\n", m_byAPU2BasNum);

		StaticLog("\t8000IMixer<%d>\n", m_by8000IMixerNum);
		StaticLog("\t8000IVidBasNum<%d>\n", m_by8000IVidBasNum);
		StaticLog("\t8000IAudBasNum<%d>\n", m_by8000IAudBasNum);
	}

}PACKED;

struct TNPlusEqpCap //len:26
{
public:
    u8      m_byMtAdpNum;
    u8      m_byMpNum;
    u8      m_byVmpNum;
	//[chendaiwei 2010/09/26]增加混音器数目
	u8      m_byMixerNum;
    u8      m_byBasAudChnlNum;    
    u8      m_byBasVidChnlNum;    
    u8      m_byPrsChnlNum;
    u8      m_byGKCharge;
    u8      m_byDCSNum;
    u8      m_abyVMPChnNum[MAXNUM_PERIEQP];
    u8      m_byMAUNum;
    u8      m_byMAUH263pNum;
	u8      m_byHduNum;
    u8      m_byMpuBasNum;
	u8      m_byBap2BasNum;
    
public:
    TNPlusEqpCap( void ) { SetNull(); }    
    
    void    SetNull( void )
    {
        m_byMtAdpNum = 0;
        m_byMpNum = 0;
        m_byVmpNum = 0;
		m_byMixerNum = 0;
        m_byBasAudChnlNum = 0;  
        m_byBasVidChnlNum = 0;
        m_byPrsChnlNum = 0;
        m_byGKCharge = 0;
        m_byDCSNum = 0;
        memset( m_abyVMPChnNum, 0, sizeof(m_abyVMPChnNum) );
        m_byMAUNum = 0;
        m_byMAUH263pNum = 0;
        m_byMpuBasNum = 0;
		m_byHduNum = 0;
		m_byBap2BasNum = 0;
    }

    // guzh [12/13/2006] 小于只比较外设能力(含DCS)和GK计费能力，接入和转发不考虑
	// 修改小于符号的意义，只比较除BAS以外的外设能力[1/12/2012 chendaiwei]
	// 修改小于符号的意义，不比较vmp外设能力[1/12/2012 chendaiwei]
    BOOL32 operator < (const TNPlusEqpCap &tObj) const
    {
        if ( /*m_byVmpNum < tObj.m_byVmpNum ||*/
			 m_byMixerNum < tObj.m_byMixerNum ||
             m_byBasAudChnlNum < tObj.m_byBasAudChnlNum ||
             /*m_byBasVidChnlNum < tObj.m_byBasVidChnlNum ||*/
             m_byPrsChnlNum < tObj.m_byPrsChnlNum ||
             m_byGKCharge < tObj.m_byGKCharge ||
             m_byDCSNum < tObj.m_byDCSNum ||
             /*m_byMAUNum < tObj.m_byMAUNum ||
             m_byMAUH263pNum < tObj.m_byMAUH263pNum ||*/
			 m_byHduNum < tObj.m_byHduNum 
             /*m_byMpuBasNum < tObj.m_byMpuBasNum ||
			 m_byBap2BasNum < tObj.m_byBap2BasNum ||*/
             /*IsLocalVMPCapInferior( tObj )*/ )
        {
            LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS, "operator < (const TNPlusEqpCap &tObj) const = TRUE due to:\n" );
            //LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS, "\tVMP<%d vs %d>\n", m_byVmpNum, tObj.m_byVmpNum );
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS, "\tMIXER<%d vs %d>\n", m_byMixerNum, tObj.m_byMixerNum );
            LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS, "\tBAC<%d vs %d>\n", m_byBasAudChnlNum, tObj.m_byBasAudChnlNum );
            /*LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS, "\tBVC<%d vs %d>\n", m_byBasVidChnlNum, tObj.m_byBasVidChnlNum );*/
            LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS, "\tPRS<%d vs %d>\n", m_byPrsChnlNum, tObj.m_byPrsChnlNum );
            LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS, "\tGKC<%d vs %d>\n", m_byGKCharge, tObj.m_byGKCharge );
            LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS, "\tDCS<%d vs %d>\n", m_byDCSNum, tObj.m_byDCSNum );
            /*LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS, "\tMAU<%d vs %d>\n", m_byMAUNum, tObj.m_byMAUNum );*/
            /*LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS, "\tMAU(h263p)<%d vs %d>\n", m_byMAUH263pNum, tObj.m_byMAUH263pNum );*/
            /*LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS, "\tMPU<%d vs %d>\n", m_byMpuBasNum, tObj.m_byMpuBasNum );*/
			/*LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS, "\tBAP2<%d vs %d>\n", m_byBap2BasNum, tObj.m_byBap2BasNum );*/
            LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS, "\tHDU<%d vs %d>\n", m_byHduNum, tObj.m_byHduNum );
            return TRUE;
        }
        return FALSE;
    }
	
	void Print( void )
	{
		StaticLog("\tVmpNum<%d>\n", m_byVmpNum);
		StaticLog("\tMixerNum<%d>\n", m_byMixerNum);
        StaticLog("\tBasVidChnlNum<%d>\n", m_byBasVidChnlNum);
		StaticLog("\tBasAudChnlNum<%d>\n", m_byBasAudChnlNum);
		StaticLog("\tPrsChnlNum<%d>\n", m_byPrsChnlNum);
		StaticLog("\tGKCharge<%d>\n", m_byGKCharge);
		StaticLog("\tDCSNum<%d>\n", m_byDCSNum);
		StaticLog("\tMAU<%d>\n", m_byMAUNum);
		StaticLog("\tMAU(h263p)<%d>\n", m_byMAUH263pNum);
		StaticLog("\tMPU<%d>\n", m_byMpuBasNum);
		StaticLog("\tBAP2<%d>\n", m_byBap2BasNum);
        StaticLog("\tHduNum<%d>\n", m_byHduNum);
	}

    // guzh [12/13/2006]  +运算只考虑外设能力(含DCS)和GK计费能力，接入和转发不考虑
    TNPlusEqpCap operator + (const TNPlusEqpCap &tObj) const
    {
        TNPlusEqpCap tCap;
        tCap.m_byBasAudChnlNum = tObj.m_byBasAudChnlNum + m_byBasAudChnlNum;
        tCap.m_byBasVidChnlNum = tObj.m_byBasVidChnlNum + m_byBasVidChnlNum;
        tCap.m_byPrsChnlNum = tObj.m_byPrsChnlNum + m_byPrsChnlNum;
        tCap.m_byVmpNum = tObj.m_byVmpNum + m_byVmpNum;
		tCap.m_byMixerNum = tObj.m_byMixerNum + m_byMixerNum;
        tCap.m_byHduNum = tObj.m_byHduNum + m_byHduNum;


        if ( tCap.m_byGKCharge == 0 )
        {
            tCap.m_byGKCharge = tObj.m_byGKCharge + m_byGKCharge;
        }
        tCap.m_byDCSNum = tObj.m_byDCSNum + m_byDCSNum;

        AdjustChnNumOrder( (u8*)&tCap.m_abyVMPChnNum, tCap.m_byVmpNum );
        AdjustChnNumOrder( (u8*)&tObj.m_abyVMPChnNum, tObj.m_byVmpNum );

        u8 byVMPNum = m_byVmpNum > tObj.m_byVmpNum ? m_byVmpNum : tObj.m_byVmpNum;
        for ( u8 byIdx = 0; byIdx < byVMPNum; byIdx ++ )
        {
            if ( tObj.m_abyVMPChnNum[byVMPNum] > tCap.m_abyVMPChnNum[byVMPNum] )
            {
                tCap.m_abyVMPChnNum[byVMPNum] = tObj.m_abyVMPChnNum[byVMPNum];
            }
        }
        tCap.m_byMAUNum = tObj.m_byMAUNum + m_byMAUNum;
        tCap.m_byMAUH263pNum = tObj.m_byMAUH263pNum + m_byMAUH263pNum;
        tCap.m_byMpuBasNum = tObj.m_byMpuBasNum + m_byMpuBasNum;
		tCap.m_byBap2BasNum = tObj.m_byBap2BasNum + m_byBap2BasNum;
        
        return tCap;
    }

    //VMP能力具体到MAP数 [12/28/2006-zbq]
    BOOL32 IsLocalVMPCapInferior( const TNPlusEqpCap &tObj ) const
    {
        AdjustChnNumOrder( (u8*)&m_abyVMPChnNum, m_byVmpNum );
        AdjustChnNumOrder( (u8*)&tObj.m_abyVMPChnNum, tObj.m_byVmpNum );

        u8 byVMPNum = m_byVmpNum < tObj.m_byVmpNum ? m_byVmpNum : tObj.m_byVmpNum;

        if ( byVMPNum > MAXNUM_PERIEQP )
        {
            LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS, "byVMPNum > MAXNUM_PERIEQP !\n" );
            return TRUE;
        }
        for ( u8 byIdx = 0; byIdx < byVMPNum; byIdx ++ )
        {
            if ( m_abyVMPChnNum[byIdx] < tObj.m_abyVMPChnNum[byIdx] )
            {
                LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS, "m_abyVMPChnNum[%d].%d < tObj.m_abyVMPChnNum[%d].%d !\n",
                                         byIdx, m_abyVMPChnNum[byIdx], byIdx,  tObj.m_abyVMPChnNum[byIdx] );
                return TRUE;
            }
        }
        return FALSE;
    }

    //将通道能力冒大个泡再比较，防止 田忌赛马式 误比 [12/28/2006-zbq]
    void AdjustChnNumOrder( u8 *pbyChn, u8 byNum ) const
    {
        if ( NULL == pbyChn || byNum > MAXNUM_PERIEQP )
        {
            return;
        }
        u8 abyChn[MAXNUM_PERIEQP] = { 0 };
        memcpy( abyChn, pbyChn, byNum );

        for( u8 byIdx = 1; byIdx < byNum; byIdx ++ )
        {
            BOOL32 bAdjusted = FALSE;
            for( u8 byPos = byNum-1; byPos >= byIdx; byPos-- )
            {
                if ( abyChn[byPos] > abyChn[byPos-1] )
                {
                    u8 byTmp = abyChn[byPos];
                    abyChn[byPos] = abyChn[byPos-1];
                    abyChn[byPos-1] = byTmp;

                    bAdjusted = TRUE;
                }
            }
            if ( !bAdjusted )
            {
                break;
            }            
        }
        memcpy( pbyChn, abyChn, byNum );
        return;
    }

    BOOL32 HasMtAdp() const
    {
        return (m_byMtAdpNum > 0);
    }

    BOOL32 HasMp() const
    {
        return (m_byMpNum > 0);
    }

    void SetIsGKCharge( BOOL32 bCharge )
    {
        m_byGKCharge = bCharge ? 1 : 0;
    }
    
}PACKED;

//注册消息(len:29)
struct TNPlusMcuRegInfo
{
public:
    void            SetMcuIpAddr( u32 dwIpAddr ) { m_dwMcuIpAddr = htonl(dwIpAddr); }
    u32             GetMcuIpAddr( void ) { return ntohl(m_dwMcuIpAddr); }
    void            SetMcuEqpCap( const TNPlusEqpCap &tEqpCap ) { m_tEqpCap = tEqpCap; }
    TNPlusEqpCap    GetMcuEqpCap( void ) { return m_tEqpCap; }
    void            SetMcuType( u8 byType ) { m_byMcuType = byType;    }
    u8              GetMcuType( void ){ return m_byMcuType;    }

private:
    u32             m_dwMcuIpAddr;  //net order
    TNPlusEqpCap    m_tEqpCap;      //外设基本能力
    u8              m_byMcuType;    //8000 or 8000B or WIN32

}PACKED;

//注册MCU扩展信息 [3/18/2013 liaokang] 支持主备N+1备双备份
struct TNPlusMcuInfoEx
{
public:
    TNPlusMcuInfoEx( void )
    {
        memset(this,0,sizeof(TNPlusMcuInfoEx));
    }
    /*BOOL32 IsNull( void ) const{ return ( 0 == m_dwAnotherMcuIp || MCU_MSSTATE_OFFLINE == m_emCurMSState ); }*/
    //当前主备用状态
    emMCUMSState GetCurMSState(void) const { return m_emCurMSState; }
    void   SetCurMSState(emMCUMSState emCurMSState) { m_emCurMSState = emCurMSState; }
    //另外一块mpc板地址
    u32  GetAnotherMpcIp( void ) const { return ntohl(m_dwAnotherMcuIp); }
    void SetAnotherMpcIp( u32 dwIpAddr ) { m_dwAnotherMcuIp = htonl(dwIpAddr); }
    //当前编码方式
    emenCodingForm GetMcuEncodingForm() const { return m_emMcuEncodingForm; }
    void   SetMcuEncodingForm(emenCodingForm emMcuEncodingForm)  { m_emMcuEncodingForm = emMcuEncodingForm; }
    
private:
    emMCUMSState    m_emCurMSState;
    u32             m_dwAnotherMcuIp;  //net order
    emenCodingForm  m_emMcuEncodingForm;     //[5/7/2013 liaokang] 增加编码方式
}PACKED;

//N+1画面合成成员信息(len:225bytes)
struct TNPlusVmpMember 
{
public:
	TNPlusVmpChnlMember m_tVmpChnnlInfo[MAXNUM_VMP_MEMBER];
}PACKED;

struct TNPlusConfExData
{
public:
	u8 m_byConInfoExBuf[CONFINFO_EX_BUFFER_LENGTH]; //会议扩展信息Buffer[1/4/2012 chendaiwei]
	TSmcuCallnfo  m_atSmcuCallInfo[MAXNUM_SUB_MCU];	//下级MCU呼叫信息

public:
	TNPlusConfExData( void )
	{
		Clear();
	}
	
	void Clear( void )
	{
		memset(this,0,sizeof(TNPlusConfExData));
	}

}PACKED;

//画面合成信息(len:232)
struct TNPlusVmpParam
{
public:
	TVmpBasicParam	m_tVmpBaiscParam;   //vmp基本参数数组
	TNPlusVmpMember m_tVmpMemer; //vmp成员信息数组
	u8 m_byConfIdx;

public:
	TNPlusVmpParam ( void )
	{
		Clear();
	}

	void Clear( void )
	{
		memset(this,0,sizeof(*this));
		m_byConfIdx = 0xFF;
	}

	BOOL32 IsNull ( void )
	{
		if(    m_tVmpBaiscParam.m_byVMPAuto == 0
			&& m_tVmpBaiscParam.m_byVMPBrdst == 0
			&& m_tVmpBaiscParam.m_byVMPStyle == 0
			&& m_tVmpBaiscParam.m_byVMPSchemeId == 0
			&& m_tVmpBaiscParam.m_byVMPMode == 0
			&& m_tVmpBaiscParam.m_byRimEnabled == 0
			&& m_tVmpBaiscParam.m_byVMPBatchPoll == 0
			&& m_tVmpBaiscParam.m_byVMPSeeByChairman == 0
			&& m_tVmpBaiscParam.m_byVmpSubType == 0)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
}PACKED;

class CNPlusInst : public CInstance
{
    //状态机
    enum
    {
        STATE_IDLE,
        STATE_NORMAL,
        STATE_SWITCHED
    };

public:
    CNPlusInst( void );
    virtual ~CNPlusInst( void );

    //用户信息
    BOOL32  GetGrpUsrCount( u8 byGrpId, u8 &byMaxNum, u8 &byNum );
    BOOL32  GetGrpUserList( u8 byGrpId, u8 *pbyBuf, u8 &byUsrItr, u8 &byUserNumInPack );
    CExUsrInfo*   GetUserPtr();
    CUsrGrpsInfo* GetUsrGrpInfo() ;
    //当前编码方式
    emenCodingForm GetMcuEncodingForm( void ) const { return m_emMcuEncodingForm; }

    void ClearInst( void );

protected:
    //普通实例入口
    //备份Server模式下每个实例对应一个mcu
	void InstanceEntry( CMessage * const pcMsg );  
    void InstanceDump( u32 dwParam = 0 );

    void ProcRegNPlusMcuReq( const CMessage * pcMsg );
    void ProcMcuDataUpdateReq( const CMessage * pcMsg );
    void ProcMcuEqpCapNotif( const CMessage * pcMsg );
    void ProcConfRollback( const CMessage * pcMsg );
    
    void ProcRtdRsp( void );
    void ProcRtdTimeOut( void );
    void ProcDisconnect( const CMessage * pcMsg );

    // 强制停止服务
    void ProcReset( void );

    // 开始接替工作
    void RestoreMcuConf( void );
    
    //Daemon 实例入口
    //备份Server模式下负责实例消息的分发，备份Client模式下demon实例对应本地mcu
    void DaemonInstanceEntry( CMessage* const pcMsg, CApp* pcApp );    
       
    void DaemonPowerOn( void );
    void DaemonReset( void );
    void DaemonDisconnect( void  );

    // 用户请求回滚
    void DaemonMcsRollBack( const CMessage * pcMsg, CApp* pcApp  );
    
    void DaemonConnectNPlusMcuTimeOut( void );
    void DaemonRegNPlusMcuTimeOut();
    
    void DaemonRegNPlusMcuReq( const CMessage * pcMsg, CApp* pcApp  );
    void DaemonRegNPlusMcuRsp( const CMessage * pcMsg );

    void DaemonVcNPlusMsgNotif( const CMessage * pcMsg );
    void DaemonDataUpdateRsp( const CMessage * pcMsg );
    void DaemonRtdReq( void );
    //会议回滚
    void DaemonConfRollbackReq( const CMessage * pcMsg );
    void DaemonUsrRollbackReq( void );
    void DaemonGrpRollbackReq( void );
    
    //消息发送
    BOOL32 MsgSndPassCheck(u16 wEvent);
    void   SndMsg2NplusMcu(CServMsg &cServMsg,u32 dwMcuIId = 0,u32 dwMcuNode = 0);
    void   PostReplyBack( u16 wEvent, u16 wErrorCode = 0, u8 *const pbyMsg = NULL, u16 wMsgLen = 0,u32 dwMcuIId = INVALID_NODE,u32 dwMcuNode = INVALID_NODE);
    void   DaemonRegNack( const CMessage * pcMsg, u8 byReason );
	
    //获取IP信息
    u32  GetMcuIpAddr( void ) const { return m_dwMcuIpAddr; }
    u32  GetMcuIpAddrB( void ) const { return m_dwMcuIpAddrB; }

    //内部处理函数
    BOOL32  SetConfData( TNPlusConfData *ptConfData );
	BOOL32  SetConfExData(CConfId cConfId, u8 *pbybuff,u16 wBufLen );
    BOOL32  SetConfInfo( TConfInfo *ptConfInfo, BOOL32 bStart = TRUE );
    BOOL32  SetConfMtInfo( CConfId &cConfId, TMtInfo *ptMtInfo, u8 byMtNum );
	BOOL32  SetConfSmcuCallInfo( CConfId &cConfId, TSmcuCallnfo *ptSmcuCallInfo, u8 bySmcuNum );
    BOOL32  SetConfVmpInfo( CConfId &cConfId, TNPlusVmpInfo *ptVmpInfo, TVMPParam *ptParam );
	BOOL32  SetConfMultiVmpParam( CConfId &cConfId, u8 *ptNplusVmpInfoBuf, u16 &wPackbufLen);
	BOOL32  UnPackVmpBufToVmpParam( u8 *ptNplusVmpInfoBuf, TNPlusVmpParam *ptVmpParam, u8 &byVmpNum, u16 &wPackbufLen);
	BOOL32  GetConfMultiVmpParam( u8 byConfIdx, TNPlusVmpParam *ptParam, u8 &byVmpNum);
	void	ClearVmpParamByConfIdx ( u8 byConIdx );
	BOOL32  SetConfAutoMix( CConfId &cConfId, BOOL32 bStart );
    BOOL32  SetChairman( CConfId &cConfId, TMtAlias *ptMtAlias );
    BOOL32  SetSpeaker( CConfId &cConfId, TMtAlias *ptMtAlias );
    BOOL32  SetUsrGrpInfo( CUsrGrpsInfo *pcUsrGrpInfo );
    BOOL32  SetUsrInfo( CExUsrInfo *pcUsrInfo, u8 byNum, BOOL32 bAdd = FALSE );

    u8      GetConfIdxByConfId( const CConfId &cConfId );    
    u8      GetEmptyConfIdx( void );
    
    void StopConfChargeOfSwitchedMcu( void );
	BOOL32 IsLocalEqpCapLowerThanRemote(TNPlusEqpCap &tLocalCap, TNPlusEqpCapEx &tlocalCapEx, TNPlusEqpCap &tRemoteCap, TNPlusEqpCapEx &tRemoteCapEx);
	BOOL32 IsVmpLowerThanOther( TNPlusEqpCap tLocalCap, TNPlusEqpCap tOtherCap, TNPlusEqpCapEx tLocalCapEx, TNPlusEqpCapEx tOtherCapEx);
	BOOL32 IsBasChnLowerThanOther(TBasChnAbility *ptLocalBasChn,TBasChnAbility *ptRemoteBasChn,u8 byLocalChNum, u8 byRemoteChnNum,BOOL32 bVideoBAs = TRUE);
	void GetAllBasChnAbilityArray(TNPlusEqpCap tLocalCap, TNPlusEqpCapEx tlocalCapEx ,TBasChnAbility *ptLocalBasChn, u8 &byLocalBasChnNum);
	void GetVideoBasChnAbilityArray(TNPlusEqpCap tCap, TNPlusEqpCapEx tCapEx,TBasChnAbility *ptBasChn, u8 &byBasChnNum);
	void GetAudioBasChnAbilityArray(TNPlusEqpCap tCap, TNPlusEqpCapEx tCapEx,TBasChnAbility *ptBasChn, u8 &byBasChnNum);
	void GetBasChnAbilityArrayByEqpType( u8 byEqpType, u8 byEqpNum, TBasChnAbility *ptBasChnnlArry, u8 &byArraySize);
    void UnRegAllInfoOfSwitchedMcu( void );
    void SetRASInfo( TRASInfo *ptRASInfo ){ memcpy( &m_tRASInfo, ptRASInfo, sizeof(TRASInfo) );    }
    TRASInfo *GetRASInfo( void ){ return &m_tRASInfo;   }
	BOOL32 IsBasSupportRollBack(TBasChnAbility *patBasChnAbility,u8 byChnNum);

    // 转码
    void TranslateEncodingForm( void );
        
private:
    u32             m_dwMcuNode;        
    u32             m_dwMcuIId;
    u32             m_dwMcuIpAddr;                  //mcu地址信息(host order)
    u32             m_dwMcuNodeB;        
    u32             m_dwMcuIIdB;
    u32             m_dwMcuIpAddrB;                 //mcu地址信息(host order)
    emenCodingForm  m_emMcuEncodingForm;            //[5/7/2013 liaokang] 增加编码方式

    TNPlusConfData  m_atConfData[MAXNUM_ONGO_CONF]; //mcu上会议信息
    CUsrGrpsInfo    m_cUsrGrpsInfo;                 //mcu上用户组信息
    CExUsrInfo      m_acUsrInfo[MAXNUM_USRNUM];     //用户信息
    u8              m_byUsrNum;                     //用户个数
    u16             m_wRtdFailTimes;                //rtd失败次数
    u16             m_wRegNackByCapTimes;           //由于对端能力不够，无法回滚时，拒绝其注册的次数
    TRASInfo        m_tRASInfo;                     //rasinfo, 用于备份MCU伪装注销掉主MCU上的实体信息
    TConfChargeInfo m_atChargeInfo[MAXNUM_ONGO_CONF];//mcu上会议的计费信息, 用于备份MCU伪装结束主MCU的计费会议
	TNPlusConfExData m_atConfExData[MAXNUM_ONGO_CONF];//会议扩展信息[1/10/2012 chendaiwei]
	TNPlusVmpParam   m_atNplusParam[MAX_PRIEQP_NUM];  //MCU vmp参数信息
};

//实例全局数据
class CNPlusData
{       
public:
    CNPlusData( void );
    virtual ~CNPlusData( void );
    
    BOOL32          InitNPlusState(void);               // 初始化本地mcu工作状态机，返回是否是N+1模式
    u8              GetMcuSwitchedInsId( void );
    u32             GetMcuSwitchedIp( void );
    void            SetMcuSwitchedInsId( u8 byInsId, u32 dwIpAddr );
    ENPlusState     GetLocalNPlusState( void );         // 获取本地 mcu N+1状态机
    void            SetLocalNPlusState( ENPlusState emState );  // 设置本地 mcu N+1状态机
    u16             GetRtdTime( void );
    u16             GetRtdNum( void );
    void            SetNPlusSynOk( BOOL32 bSynOk );
    BOOL32          GetNPlusSynOk( void );
    TNPlusEqpCap    GetMcuEqpCap( void );
	TNPlusEqpCapEx  GetMcuEqpCapEx( void );
	void			GetEqpCapFromConf( const TConfInfo &tConf, TNPlusEqpCap &tCap, TNPlusEqpCapEx &tCapEx, TBasChnAbility *ptBasAbility, u8& byBasChnSize);
    void            SetMcuType( u8 byType );
    u8              GetMcuType( void );

    void            PostMsgToNPlusDaemon( u16 wEvent, u8 *const pbyMsg = NULL, u16 wMsgLen = 0 );

    //消息打包/解压
    void            PackNplusExInfo(const TNPlusEqpCapEx &tEqpCapEx,u8 * pbyOutbuf, u16 &wPackEqpCapExLen,const TNPlusMcuInfoEx *ptMcuRegInfoEx = NULL);
    void            UnPackNplusExInfo(TNPlusEqpCapEx & tNplusEqpCap,const u8 *pbyInBuf, u16 &wUnPackNplusEqpCapLen, BOOL32 &bExistUnknowInfo,TNPlusMcuInfoEx *PtMcuRegInfoEx = NULL);
  
	TBasChnAbility  GetBasChnAbilityByBasChnType( emBasChnType eBasChnType);

private:
    ENPlusState     m_emLocalNPlusState;
    u8              m_byNPlusSwitchInsId;               //发生模式切换的实例id
    u32             m_dwNPlusSwitchMcuIp;               //发生模式切换的mcu地址
    u16             m_wRtdTime;                         //rtd 时间间隔
    u16             m_wRtdNum;                          //rtd 次数
    u8              m_byNPlusSynOk;                     //是否数据备份成功
    u8              m_byLocalMcuType;                   //本地MCU的类型

};

typedef zTemplate< CNPlusInst, MAXNUM_NPLUS_MCU, CNPlusData > CNPlusApp;

extern  CNPlusApp           g_cNPlusApp;    

#define  TRANSENCODING_UTF8_GBK    (u8)0
#define  TRANSENCODING_GBK_UTF8    (u8)1

inline void TransEncodingOfUsrGrps(CUsrGrpsInfo &cUsrGrpsInfo, u8 byTransEncodingType);
inline void TransEncodingOfUsrInfo(CExUsrInfo &cExUsrInfo, u8 byTransEncodingType);
/*inline*/ void TransEncodingOfNPlusConfData(TNPlusConfData &tConfData, u8 byTransEncodingType);

#ifdef WIN32
#pragma pack( pop )
#endif

#endif //_NPLUS_MANAGER_H
