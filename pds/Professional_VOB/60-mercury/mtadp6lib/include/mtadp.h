/*****************************************************************************
   模块名      : MtAdp
   文件名      : mtadp.h
   相关文件    : 
   文件实现功能: 下级MT会话应用类头文件 
   作者        : 谭光
   版本        : V0.1  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/11     1.0         谭光          创建
******************************************************************************/
#ifndef _MTADP_H
#define _MTADP_H

#include "mcuconst.h"
#include "mcustruct.h"
#include "vccommon.h"

#include "cascade.h"

#ifdef WIN32
	#include "h323adapter.h"	//win32沿用v4的库
#else
	#include "h323adapter4_2.h"
#endif

#include "mastructv4r4b2.h"
#include "mcuerrcode.h"
#include "radiusinterface.h"

#include "kdvlog.h"
#include "loguserdef.h"
#include "mcuinnerstruct.h"
#include "cristruct.h"

#define CALLING_TIMEOUT				(u16)20		//maximum time to wait a calling response
#define WAITING_PWD_TIMEOUT			(u16)60		//maximum time to wait password response
#define INTERVAL_REG_GK             (u16)5 		//time interval for LW registration on GK  
#define ROUNDTRIP_TIMEOUT           (u16)2
#define URQ_RSP_TIMEOUT				(u16)20		//maximum time to wait gk of URQ response

#define TYPE_PARTLISTINFO           (u16)0		//终端列表信息
#define TYPE_AUDINFO                (u16)1		//音频信息
#define TYPE_VIDINFO                (u16)2		//视频信息

#define MAXNUM_DBGMT                (u8)10           //最大debug双流能力的终端数
#define DEFAULT_NUM_CALLOUT         (u8)10           //默认同时呼叫终端数
#define MAX_NUM_CALLOUT             (u8)20           //最大同时呼叫终端数
#define ROSTER_PARTLIST_NUM         (u16)32			 //每个花名册通知包的最大part数，超过则分为多个包发送
#define H323_POLL_INTERVAL          (u16)30			 //接收消息的协议栈定时器异常重启时间间隔
#define CONNECTING_MCU_INTERVAL     (u16)(1*1000)	 //MTADP连接MCU时间间隔
#define REGISTERING_MCU_INTERVAL    (u16)(3*1000)	 //MTADP注册MCU时间间隔
#define CONNECTING_GK_INTERVAL      (u16)(3*1000)	 //MTADP连接GK时间间隔
#define REGISTERING_GK_INTERVAL     (u16)(3*1000)	 //MTADP注册GK时间间隔
#define CONF_CHANGE_NTF_INTERVAL	(u32)(2*60*1000)//会议计费状态激活定时时间间隔 2min
#define RAS_RSP_TIMEOUT             (u16)(4000)    //等待RAS响应的超时时长定义 4s

#define REMOTE_MCU_PARTID           (u32)(0xffffffff)//对方在本MCU上的条目

// #define MTADP_TYPE_IP_NO            (u16)0
// #define MTADP_TYPE_IP_V4            (u16)1
// #define MTADP_TYPE_IP_V6            (u16)2


#define CHANGE_STATE( nextstate ) \
	do {						  \
	m_dwLastState = CurState();	  \
	DoStatistics( nextstate );	  \
	NEXTSTATE( nextstate );		  \
	} while( 0 )

// #define UNEXPECTED_MESSAGE(event) \
// 	MtAdpInfo("Unexpected message %u(%s) received in state %u. Ignore it.\n", \
// 		event, OspEventDesc(event), CurState() )

#define UNEXPECTED_MESSAGE( event ) \
	do {	\
	u16 wId = event; \
	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Unexpected message %u(%s) received in state %u. Ignore it.\n", \
		wId, OspEventDesc(wId), CurState() );\
	} while( 0 )
	
#define TIME_SHOW           \
    time_t  tCurTime;       \
    struct tm   *ptTime;    \
    s8 achBuf[32];          \
    memset(achBuf, '\0', sizeof(achBuf));   \
    tCurTime = time( 0 );                   \
    ptTime = localtime( &tCurTime );        \
    StaticLog( "%2.2u:%2.2u:%2.2u ",   \
                            ptTime->tm_hour, ptTime->tm_min, ptTime->tm_sec );

//print ip address as ascii string, avoiding the thread-unsafe inet_ntoa
#define QUADADDR(ip) ((u8 *)&(ip))[0], ((u8 *)&(ip))[1], ((u8 *)&(ip))[2], ((u8 *)&(ip))[3]

#define IS_MT   (m_emEndpointType == emEndpointTypeMT)

const u8	DEBUG_CRITICAL	=	LOG_LVL_ERROR;
const u8	DEBUG_ERROR		=	LOG_LVL_ERROR;
const u8	DEBUG_WARNING	=	LOG_LVL_WARNING;
const u8	DEBUG_INFO		=	LOG_LVL_KEYSTATUS;
const u8	DEBUG_VERBOSE	=	LOG_LVL_DETAIL;
const u8    DEBUG_DETAIL    =   LOG_LVL_DETAIL;  

#define OTHERBREAK  { if(m_byVendorId != MT_MANU_KDC)	break; }
#define OTHERRETURN { if(m_byVendorId != MT_MANU_KDC)   return;}


enum CALLDIRECT 
{ 
	CALL_OUTGOING,	                   
	CALL_INCOMING, 	
	CALL_INVALID 
};

enum CHANDIRECT 
{ 
	CHAN_OUTGOING,	
	CHAN_INCOMING,	
	CHAN_INVALID 
};

enum CHANSTATE
{ 
	CHAN_IDLE = 0,
	CHAN_OPENING, 
	CHAN_CONNECTED 
};

enum emMtVer
{
    emMtVerBegin,
        emMtVer36,          //KDC MT Version 3.6
        emMtVer40R4,        //KDC MT Version 4.0 R1-R4
        emMtVer40R5,        //KDC MT Version 4.0 R5+
		emMtVer40R6,		//KDC MT Version 4.0 R6
    emVerEnd
};

// 级联扩容结构体 [pengguofeng 6/25/2013]
struct TMtList
{
private:
	u16 m_wTotalLen; //总长度, 只用于计算,目测范围192*50
	u8  m_abyMtId[MAXNUM_CONF_MT]; //表示某终端别名有没有存储: 0表示未存储　非0值表示存储
	s8  m_achMtName[MAXNUM_CONF_MT][STR_LEN/*_UTF8*/];
	s8  m_achMcuName[MAXLEN_ALIAS]; //本级Mcu的名字，最大32个汉字
public:
	void Init()
	{
		memset(this, 0, sizeof(TMtList));
	}
	
	//  [pengguofeng 7/11/2013]
	bool AddMcuName(s8 *pName)
	{
		if ( !pName )
		{
			return false;
		}

		u32 dwLen = strlen(pName);
		if ( dwLen > MAXLEN_ALIAS -1)
		{
			dwLen = MAXLEN_ALIAS -1;
		}

		memcpy(m_achMcuName, pName, dwLen);
		m_wTotalLen += dwLen;
		m_achMcuName[dwLen] = 0;
		return true;
	}

	u16  GetTotalNameLen(void)
	{
		return m_wTotalLen;
	}
	
	u32  GetTotalMsgLen(void)
	{
		//涉及到上报给MCU的消息体，所有成员的bymtid+ byNameLen + 
		return GetTotalMtNum() *(sizeof(u8) + sizeof(u16) ) + GetTotalNameLen();
	}

	u8   GetTotalMtNum(void)
	{
		u8 byMtNum = 0;
		for (u8 byIdx = 0; byIdx < MAXNUM_CONF_MT;byIdx++)
		{
			if (m_abyMtId[byIdx] != 0 )
			{
				byMtNum++;
			}
		}

		byMtNum++; //mcu本身

		return byMtNum;
	}

	bool IsValidMt(const u8 byMtId)
	{
		if ( byMtId > 0 && byMtId <= MAXNUM_CONF_MT)
		{
			if ( m_abyMtId[byMtId-1] != 0 )
			{
				return true;
			}
		}
		
		return false;
	}
	
	bool AddMtName(u8 byMtId, s8 *pszMtName)
	{
		u16 dwLen = strlen(pszMtName);
		if ( /*IsValidMt(byMtId)*/ byMtId > 0 && byMtId <= MAXNUM_CONF_MT )
		{			
			if ( dwLen > STR_LEN/*_UTF8*/-1 )
			{
				dwLen = STR_LEN/*_UTF8*/ - 1;
			}
			memcpy(m_achMtName[byMtId-1], pszMtName,dwLen);
			m_achMtName[byMtId-1][dwLen] = 0;
			m_wTotalLen += dwLen;

			m_abyMtId[byMtId-1] = 1;
			return true;
		}
		else
		{
/*			if ( byMtId == 0 )
			{
				if ( dwLen > MAXLEN_ALIAS -1 )
				{
					dwLen = MAXLEN_ALIAS -1;
				}
				memcpy(m_achMcuName, pszMtName,dwLen);
				m_wTotalLen += dwLen;
			}
*/			return false;
		}
	}
	
	bool DelMtName(u8 byMtId)
	{
		if ( IsValidMt(byMtId) )
		{
			m_abyMtId[byMtId-1] = 0;
			m_wTotalLen -= strlen(m_achMtName[byMtId-1]);
			return true;
		}
		else
		{
			return false;
		}
	}
	
	s8 * GetMtName(const u8 byMtId)
	{
		if ( IsValidMt(byMtId))
		{
			return m_achMtName[byMtId-1];
		}
		else
		{
			return NULL;
		}
	}
	
	//由使用者保证pBuf足够长：193*(u8 + u16) + 50*192=10176 = 10K
	//拼接格式: [byMtId + wStrLen + s8 * ] x GetTotalMtNum()
	bool MakeMtName2Buf(u8 *pBuf, u16 &dwWLen )
	{
		if ( !pBuf )
		{
			return false;
		}

		dwWLen = 0;
		u16 wNameLen = 0;
		u16 wCopyLen = 0;
		//1.拼McuName
		u8 byMcuId = 0;
		memcpy(pBuf+dwWLen, &byMcuId, sizeof(u8));
		dwWLen += sizeof(u8);

		wCopyLen = strlen(m_achMcuName);
		wNameLen = htons(wCopyLen);

		memcpy(pBuf+dwWLen, &wNameLen, sizeof(u16));
		dwWLen += sizeof(u16);

		memcpy(pBuf+dwWLen, m_achMcuName, wCopyLen);
		dwWLen += wCopyLen;
				
		//2. 拼下级MT
		for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT;byMtId++)
		{
			if ( IsValidMt(byMtId) )
			{
				memcpy(pBuf+dwWLen, &byMtId, sizeof(u8));
				dwWLen += sizeof(u8);

				wCopyLen = strlen(m_achMtName[byMtId-1]);
				wNameLen = htons(wCopyLen);
				memcpy(pBuf+dwWLen, &wNameLen, sizeof(u16));
				dwWLen += sizeof(u16);

				memcpy(pBuf+dwWLen, m_achMtName[byMtId-1], wCopyLen);
				dwWLen += wCopyLen;
			}
		}

		return true;
	}
	void Print()
	{
		StaticLog("\t\t[MtList] Total Length:%d McuName:{%s }\n", m_wTotalLen, m_achMcuName);
		for ( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
		{
			if ( IsValidMt(byMtId))
			{
				StaticLog("\t\tIdx:%d Name:{%s }\n",byMtId, m_achMtName[byMtId-1]);
			}
		}
	}
};

struct TMcuList
{
private:
#define INVALID_IDX     (u8)0xFF
	u8 m_abyMcuMtId[MAXNUM_SUB_MCU+1]; //包括自己在内一共25个，值对应这些MCU在二级的表示
	TMtList m_tMtList[MAXNUM_SUB_MCU+1];
public:
	void Init()
	{
		memset(m_abyMcuMtId, INVALID_IDX, sizeof(m_abyMcuMtId)); //初值为全255，是因为0~24是有效值
		for ( u8 byIdx = 0; byIdx < MAXNUM_SUB_MCU + 1;byIdx++ )
		{
			m_tMtList[byIdx].Init();
		}
	}

	bool IsValidMcu( const u8 &byMcuMtId, u8 &byMtListIdx)
	{
		byMtListIdx = INVALID_IDX;
		for ( u8 byIdx = 1; byIdx <= MAXNUM_SUB_MCU + 1; byIdx++)
		{
			if ( m_abyMcuMtId[byIdx-1] == byMcuMtId )
			{
				byMtListIdx = byIdx;
				return true;
			}
		}

		return false;
	}

	bool GetIdleIdx(u8 &byMtListIdx)
	{
		byMtListIdx = INVALID_IDX;
		for ( u8 byIdx = 1; byIdx <= MAXNUM_SUB_MCU + 1;byIdx++)
		{
			if ( m_abyMcuMtId[byIdx-1] == INVALID_IDX )
			{
				byMtListIdx = byIdx;
				return true;
			}
		}

		return false;
	}

	void DelMtList(const u8 &byMcuMtId)
	{
		u8 byIdx = 0xFF;
		if ( IsValidMcu(byMcuMtId, byIdx) )
		{
			m_abyMcuMtId[byIdx-1] = INVALID_IDX;
		}
	}
	bool AddMtList(const u8 &byMcuMtId, const TMtList &tMtList)
	{
		u8 byIdx = INVALID_IDX;
		if ( IsValidMcu(byMcuMtId, byIdx) 
			|| GetIdleIdx(byIdx))
		{
			memcpy(&m_tMtList[byIdx-1], &tMtList, sizeof(TMtList));
			m_abyMcuMtId[byIdx-1] = byMcuMtId;
			return true;
		}
		
		return false;
	}

	TMtList *GetMtList(const u8 &byMcuMtId)
	{
		u8 byIdx = INVALID_IDX;
		if ( IsValidMcu(byMcuMtId, byIdx) )
		{
			return &m_tMtList[byIdx-1];
		}
		else
		{
			return NULL;
		}
	}

	void Print()
	{
		StaticLog("\t[McuList] as follow:\n");
		for ( u8 byLoop = 1;byLoop<= MAXNUM_SUB_MCU+1;byLoop++)
		{
			if ( m_abyMcuMtId[byLoop-1] != INVALID_IDX )
			{
				StaticLog("\tSMcu MtId:%d\n", m_abyMcuMtId[byLoop-1]);
				m_tMtList[byLoop-1].Print();
			}
		}
	}
};

struct TMtadpMcuList
{
private:
#define INVALID_IDX     (u8)0xFF
		u8 m_abyIdleMcuIdx[MAXLIMIT_CRI_MTADP_SMCU];  //当前剩下的索引,　没用过的用255表示，用过的置0
		u8 m_abyMtadpInsId[MAXNUM_CONF_MT]; //每个实体对应的m_tMcuList下标,取值范围：0~27，初始为255
		TMcuList m_tMcuList[MAXLIMIT_CRI_MTADP_SMCU];
public:
	void Init()
	{
		memset(m_abyMtadpInsId, INVALID_IDX, sizeof(m_abyMtadpInsId));
		memset(m_abyIdleMcuIdx, INVALID_IDX, sizeof(m_abyIdleMcuIdx));
		for ( u8 byIdx = 0; byIdx < MAXLIMIT_CRI_MTADP_SMCU;byIdx++)
		{
			m_tMcuList[byIdx].Init();
		}
	}

	bool IsValidInsId(const u8 &byInsId, u8 &byMcuIdx)
	{
		if ( byInsId <= MAXNUM_CONF_MT 
			&& byInsId >= 1
			&& m_abyMtadpInsId[byInsId-1] < MAXLIMIT_CRI_MTADP_SMCU )
		{
			byMcuIdx = m_abyMtadpInsId[byInsId-1];
			return true;
		}

		byMcuIdx = INVALID_IDX;
		return false;
	}

	bool IsIdleIdx(const u8 &byInsId)
	{
		if ( byInsId <= MAXNUM_CONF_MT 
			&& byInsId >= 1
			&& m_abyMtadpInsId[byInsId-1] == INVALID_IDX )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	//取得0~25中的最小可用索引
	bool GetIdleIdx(u8 &byInsId, u8 &byMcuIdx)
	{
		if ( m_abyMtadpInsId[byInsId-1] != INVALID_IDX )
		{
			byMcuIdx = m_abyMtadpInsId[byInsId-1];
			return true;
		}
		else
		{
			//分配一个
			byMcuIdx = AssignNewMcuIdx();
			if ( byMcuIdx == INVALID_IDX )
			{
				return false;
			}
			else
			{
				return true;
			}
		}
	}

	u8 AssignNewMcuIdx()
	{
		for ( u8 byMcuIdx = 0;byMcuIdx < MAXLIMIT_CRI_MTADP_SMCU; byMcuIdx++)
		{
			if (m_abyIdleMcuIdx[byMcuIdx] == INVALID_IDX )
			{
				return byMcuIdx;
			}
		}

		return INVALID_IDX;
	}

	bool AddMcuList(u8 byInsId, const TMcuList &tMcuList)
	{
		u8 byMcuIdx = INVALID_IDX;
		if ( IsValidInsId(byInsId, byMcuIdx) )
		{
			//找到已经存在的
		}
		else if ( GetIdleIdx(byInsId, byMcuIdx))
		{
			//找到空闲的，分配即可
		}
		else
		{
			//找不到的情况
		}

		if ( byMcuIdx != INVALID_IDX )
		{
			memcpy(&m_tMcuList[byMcuIdx], &tMcuList, sizeof(TMcuList));
			m_abyMtadpInsId[byInsId-1] = byMcuIdx; //保存此InstId对应的Mcu索引
			m_abyIdleMcuIdx[byMcuIdx] = 0;         //此Mcu索引被用掉了
			LogPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Add McuList of Ins:%d used McuIdx:%d\n", byInsId, byMcuIdx);
			return true;
		}
		else
		{
			return false;
		}
	}

	bool DelMcuList( const u8 &byInsId)
	{
		u8 byMcuIdx = INVALID_IDX;
		if ( IsValidInsId(byInsId, byMcuIdx))
		{
			m_abyMtadpInsId[byInsId-1] = INVALID_IDX;
			m_abyIdleMcuIdx[byMcuIdx] = INVALID_IDX;
			return true;
		}

		return false;
	}

	TMcuList *GetMcuList(const u8 &byInsId)
	{
		u8 byMcuIdx = INVALID_IDX;
		if ( IsValidInsId(byInsId, byMcuIdx))
		{
			return &m_tMcuList[byMcuIdx];
		}
		else
		{
			return NULL;
		}
	}

	void Print()
	{
		StaticLog("[Mtadp List]:\n");
		u8 byMcuIdx = INVALID_IDX;
		for ( u8 byInsId = 1; byInsId <= MAXNUM_CONF_MT;byInsId++)
		{
			if ( IsValidInsId(byInsId, byMcuIdx) )
			{
				StaticLog("MtadpInst:%d\n", byInsId);
				m_tMcuList[byMcuIdx].Print();
			}
		}
	}
};
// 定义END [pengguofeng 6/25/2013]

struct TChannel
{
	TLogicalChannel	tLogicChan;
	HCHAN			hsChan;			//NULL means free
	u32             dwLastVCUTick;  //last video-fast-update send-time-tick
	u32             dwLastFCTick;   //last flowcontrol-command send-time-tick
	u16				wCallRate;
	u8				byState;
	u8				byDirect;		//calling direction：CHAN_OUTGOING / CHAN_INCOMING

    TChannel(void)
    {
        Clear();
    }

    void Clear(void)
    {
        hsChan    = NULL;
        byState   = CHAN_IDLE;
        byDirect  = 0;
        wCallRate = 0;
        dwLastVCUTick = 0;
        dwLastFCTick  = 0;
        tLogicChan.Clear();
    }
		
	BOOL32 CheckVCUTimeInterval()
    { 
		u32 dwTimeInterval = 1 * OspClkRateGet();
		u32 dwCurTick = OspTickGet();	
		if( (dwCurTick - dwLastVCUTick) > dwTimeInterval )
		{
			dwLastVCUTick = dwCurTick;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	BOOL32 CheckFCTimeInterval( u16 wCurCallRate )
	{
        u32 dwTimeInterval = 1 * OspClkRateGet();
		u32 dwCurTick = OspTickGet();
		if( (wCallRate != wCurCallRate) || 
			(dwCurTick - dwLastFCTick) > dwTimeInterval )
		{
			wCallRate = wCurCallRate;
			dwLastFCTick = dwCurTick;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
};

//终端呼叫信息(len: 321)
struct TMtCallInfo
{
public:
    TMtCallInfo() { SetNull(); }
    void   SetNull( void ){ memset(this, 0, sizeof(TMtCallInfo)); }
    BOOL32 IsNull( void ) { return m_byConfIdx == 0 || m_tMt.GetMtId() == 0 ? TRUE : FALSE; }
    
public:
    CConfId  m_cConfId;
    TMt      m_tMt;
    u8       m_byEncrypt;
    u8       m_byConfIdx;
    u16      m_wCallRate;
    TMtAlias m_tMtAlias;
    TMtAlias m_tConfAlias;
    TCapSupport m_tCap;
 	u16         m_wForceCallInfo;        // low byte: conflevel; high byte:forcecalltype
	u8			m_byAdminLevel;
	TCapSupportEx m_tCapEx;

	// 新增主流扩展能力集勾选和双流扩展能力集勾选 [12/8/2011 chendaiwei]
	TVideoStreamCap m_atMainStreamCapEx[MAX_CONF_CAP_EX_NUM];
	TVideoStreamCap m_atDoubleStreamCapEx[MAX_CONF_CAP_EX_NUM];
	TMtAlias m_tConfE164Alias;
	u8 m_byNPlusSmcuAliasFlag; //Flag为1，标识N+1会议下级会议别名类型是E164号，但实质是IP呼叫.[11/16/2012 chendaiwei]
	TAudioTypeDesc m_atAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];//会议音频能力集
};

//终端呼叫队列
struct TMtQueue
{
public:
    TMtQueue();
    ~TMtQueue();

    void   Init( u8 byLen );
    void   Quit( void );
    BOOL32 ProcQueueInfo( TMtCallInfo &tCallInfo, BOOL32 bInc = TRUE, BOOL32 bDel = TRUE );
    BOOL32 IsQueueFull( void );
    BOOL32 IsQueueNull( void );
    BOOL32 IsMtInQueue( u8 byConfIdx, u8 byMtId, u8* pbyPos = NULL );
    void   ClearQueue( void );
    void   ShowQueue( void );
    void   PrtQueueInfo( void );
	void   QueueLog( s8* pszStr, ... );
    
protected:
    TMtCallInfo    *m_ptCallInfo;
    u8              m_byLen;
    s32             m_nHead;
    s32             m_nTail;
    BOOL32          m_bPrtChgInfo; //是否实时打印呼叫队列的出入列信息
};

//终端双流debug能力描述
struct TMtDSCap
{
public:
    TMtDSCap():m_byRes(0),
		m_byFps(0)
    {
        memset(m_achHWVer, 0, sizeof(m_achHWVer));
    };
	
    void SetRes(u8 byRes) { m_byRes = byRes;    }
    u8   GetRes(void) const { return m_byRes;   }
    void SetFps(u8 byFps) { m_byFps = byFps;    }
    u8   GetFps(void) const { return m_byFps;   }
    void SetHWVer(LPCSTR lpHWVer){ memcpy(m_achHWVer, lpHWVer, sizeof(m_achHWVer)-1); }
    LPCSTR GetHWVer(void) const { return m_achHWVer; }
    void SetNull()
    {
        m_byRes = 0;
        m_byFps = 0;
        memset(m_achHWVer, 0, sizeof(m_achHWVer));
    }
    BOOL32 IsNull()
    {
        return 0 == strlen(m_achHWVer);
    }
    void Print(void)
    {
        StaticLog("Mt.%s with ds cap<Res.%d, Fps.%d>\n", m_achHWVer, m_byRes, m_byFps);
    }
	
private:
    u8 m_byRes;
    u8 m_byFps;
    s8 m_achHWVer[14];      //版本字符串只匹配前14个,够用
};

struct TMtAdpDSCaps
{
public:
    TMtAdpDSCaps():m_byNum(0){};
    BOOL32 AddMtCap(LPCSTR lpHWVer, u8 byRes, u8 byFps)
    {
        if (MAXNUM_DBGMT == m_byNum)
        {
            StaticLog( "[TMtAdpDSCaps] dbg caps has no room for new cap!\n");
            return FALSE;
        }
        if (IsMtExist(lpHWVer))
        {
            return TRUE;
        }
        for(u8 byIdx = 0; byIdx < MAXNUM_DBGMT; byIdx++)
        {
            if (m_atDSCap[byIdx].IsNull())
            {
                m_atDSCap[byIdx].SetHWVer(lpHWVer);
                m_atDSCap[byIdx].SetFps(byFps);
                m_atDSCap[byIdx].SetRes(byRes);
                m_byNum ++;
                break;
            }
        }
        return TRUE;
    }
	
    BOOL32 GetMtCap(s8 *lpHWVer, u8 &byRes, u8 &byFps)
    {
        if (!IsMtExist(lpHWVer))
        {
            return FALSE;
        }
        for(u8 byIdx = 0; byIdx < m_byNum; byIdx++)
        {
            if (0 == strcmp(lpHWVer, m_atDSCap[byIdx].GetHWVer()))
            {
                byRes = m_atDSCap[byIdx].GetRes();
                byFps = m_atDSCap[byIdx].GetFps();
                break;
            }
        }
        return TRUE;
    }
    BOOL32 IsMtExist(LPCSTR lpHWVer)
    {
        BOOL32 bExist = FALSE;
        for(u8 byIdx = 0; byIdx < m_byNum; byIdx++)
        {
            if (0 == strcmp(lpHWVer, m_atDSCap[byIdx].GetHWVer()))
            {
                bExist = TRUE;
                break;
            }
        }
        return bExist;
    }
    void Print(void)
    {
        StaticLog("Ds cap num.%d\n", m_byNum);
        for(u8 byIdx = 0; byIdx < m_byNum; byIdx++)
        {
            m_atDSCap[byIdx].Print();
        }
    }
private:
    u8 m_byNum;
    TMtDSCap m_atDSCap[MAXNUM_DBGMT];
};

//IPV6接入支持
// #define  MTADP_NAME_LEN_IPV6    46
// struct TIpMap
// {
// private:
//     u32 m_dwIpV4;
//     u8  m_abyIpV6[MTADP_NAME_LEN_IPV6];
// 
// public:
//     TIpMap(){ memcpy( this, 0, sizeof(TIpMap) ); }
// 
//     ~TIpMap(){ }
// 
//     u32 GetIpV4( void ){ return ntohl( m_dwIpV4 ); }
//     void SetIpV4( u32 dwIpV4 ){ m_dwIpV4 = htonl( dwIpV4 ); }
// 
//     u8* GetIpV6( void ){ return m_abyIpV6; }
//     void SetIpV6( u8* pabyIpV6 )
//     { 
//         if (pabyIpV6 != NULL)
//         {
//             memcpy( m_abyIpV6, pabyIpV6, sizeof(m_abyIpV6) );
//         }
//         else
//         {
//             StaticLog("[TIpMap::SetIpV6] pointer param is null!\n");
//         }
//     }
// 
// };

class CMtAdpInst;

struct TPartIdTable
{
	u8  byMtId;
	u32 dwPartId;
};

struct TMtadpMultiCascadeMsg : TPartID
{
	u8 m_byCasLevel;
	u8 m_abyMtIdentify[MAX_CASCADELEVEL]; //对应级别中终端的标识 
};

struct TMtadpHeadMsg
{
	TMtadpMultiCascadeMsg m_tMsgSrc;
	TMtadpMultiCascadeMsg m_tMsgDst;
public:
	TMtadpHeadMsg()
	{
		memset( this,0,sizeof(TMtadpHeadMsg) );
	}
};

//H460 RTP发送地址
struct RTPSndAddress 
{
public:
    u32  m_dwIP;			// RTP地址
    u16  m_wPort;			// RTP端口
    u8   m_byChlIndex;		// 对应通道索引

public:
    void SetIp( u32 dwIp){ m_dwIP = htonl(dwIp); }
    u32  GetIp(){ return ntohl(m_dwIP); }

    void SetPort( u16 wPort ){ m_wPort = htons(wPort); }
    u16  GetPort(){ return ntohs(m_wPort); }

    void SetChlIndex( u8 byChlIndex ) { m_byChlIndex = byChlIndex; }
    u8   GetChlIndex(){ return m_byChlIndex; }
};

class CMtAdpInst : public CInstance
{
public:	
	enum 
	{ 
		STATE_IDLE	= 0,	//idle
		STATE_GKCALLING,	//calling GK		
		STATE_CALLING,		//h.225 calling (MCU invites Mt or MT applys join conf)
		STATE_NORMAL,		//h.225 calling completed
	};
		
	CMtAdpInst();
	virtual ~CMtAdpInst();

	void ProcRadEvNewCall( CMessage * const pcMsg );
	void ProcRadEvNewChannel( CMessage * const pcMsg );
	void ProcRadEvCallCtrl( CMessage * const pcMsg );
	void ProcRadEvChanCtrl( CMessage * const pcMsg );
	void ProcRadEvConfCtrl( CMessage * const pcMsg );
	void ProcRadEvFeccCtrl( CMessage * const pcMsg );
	void ProcRadEvMmcuCtrl( CMessage * const pcMsg );
	
public:
	u32			m_dwLastState;	         //previous state
	HCALL		m_hsCall;		         //stack handle for this call
	u8			m_byDirect;		         //calling direction	
	u16			m_wCallRate;	         //call bandwidth in Kbps
 	u16         m_wForceCallInfo;        // low byte: conflevel; high byte:forcecalltype
	TCALLPARAM  m_tIncomingCallParams;   //all info about an incoming call
	u16			m_wAppliedCallRate;
	u8			m_byFirstVideoChanIdx;
	u8			m_byMtId;		         //terminal number, assigned from MMCU or to Mt
	
	s8    		m_achtMtAlias[LEN_TERID];//terminal alias (H323ID/E164/IP string)
	TMtAlias    m_tMtAlias;
	CConfId		m_cConfId;				 //conference ID the terminal belongs to
	u8			m_byConfIdx;			 //conference instance id in McuVc
										 //<m_byConfIdx, m_byMtId> singly determins an mtadp instance
	u32			m_dwMtIpAddr;			 //mt IP address in network byte order
	u16			m_wMtPort;				 //mt port for Q931 calling
	u8			m_byVendorId;			 //vendor of mt or mmcu
	emEndpointType m_emEndpointType;     //MT, MCU, or GK

    emenCodingForm m_emEndpointEncoding; //[4/8/2013 liaokang] MT, MCU 编码方式
	
	BOOL32		m_bH245Connected;        //whether h245 connection is setup
	BOOL32		m_bHasJoinedConf;        //whether the TCS & MSD have completed

	HRAS		m_hsRas;		         //ras handle for this call
	s32			m_nIrrFrequency;	     //IRR frequency if required
	
	cmTerminalLabel m_tTerminalLabel;	 //TerminalLabel assigned by upper MCU
	
	TChannel	m_atChannel[MAXNUM_CALL_CHANNEL]; //channel info of this call
	u8			m_byTimeoutTimes;		 //roundTripDelayRsp time out times
	BOOL32		m_bEverGotRTDResponse;	 //ever getting RTD rsp means mt supports it

	u16			 m_wSavedEvent;		     //save the last event sent to terminal	
	TCapSet		*m_ptRemoteCapSet;	     //remote cap set
	TCapSupport	 m_tLocalCapSupport;     //local cap set of no 3.6mt's
// 	TCapSupport  m_tCommonCap;		     //common cap supported by both local and remote EP's
    TMultiCapSupport m_tCommonCap;
	TCapSupportEx m_tLocalCapEx;	     // local capex set, zgc, 2007-09-27 
	TCapSupportEx m_tCommonCapEx;		 // common capex supported by both local and remote EP's, zgc, 2007-09-30
 
	BOOL32       m_bIsRecvDRQFromGK;     //whether DRQ is received from GK
	BOOL32       m_bIsRecvARJFromGK;     //whether ARJ is received from GK
	TMtAlias     m_tConfAlias;

	u8      m_byEncrypt;
	BOOL32  m_bMaster;
    BOOL32  m_bMsdOK;
    u8      m_byMsdFailTimes;
    
//	u8	   *m_pbyNonStandConfInfoData;	 //非标数据指针(动态分配)
//	s32		m_nNonStandConfInfoDataLen;	 //非标数据长度
//	u8	   *m_pbyNonStandConfInfoData36; //3.6终端的非标数据指针(动态分配)
//	s32		m_nNonStandConfInfoDataLen36;//3.6终端的非标数据长度

	TPartIdTable m_atPartIdTable[MAXNUM_CONF_MT];
	u32          m_dwPartIdNum;
	TConfMtInfo  m_tMtInfo;	
	u32		m_dwMMcuReqId;   
	u32		m_dwPeerReqId;	
	u8		m_abyPassword[LEN_H243PWD]; 
	s8		m_achConfPwd [LEN_H243PWD];
	s32		m_nPasswordLen;

	u8      m_byMtVer;					 //emMtVer
	u8		m_byAdminLevel;				//20100828新增 mcu行政级别
	u8      m_dwSmcuMixMemBeforeStart[MAXNUM_CONF_MT>>3];//保存预添加的下级混音成员，针对以前版本R3_FULL
	//[4/9/2011 zhushengze]IMAX模板帧率适配
	//u8		m_byAdpFps;
	
	// [11/21/2011 liuxu] 下级终端列表缓冲发送给McuVc
	u8		m_bySMcuMtListBufTimerStart;
	CStaticBuf<TMcuMcuMtInfo, MAXNUM_CONF_MT>	m_cSMcuMtListSendBuf;
	
	//主双流扩展能力集勾选 [12/2/2011 chendaiwei]
	TVideoStreamCap m_atMainStreamCapEX[MAX_CONF_CAP_EX_NUM];	
	TVideoStreamCap m_atDoubleStreamCapEX[MAX_CONF_CAP_EX_NUM];

	//u8 m_byAudioTrackNum;		//音频通道数 单声道 双声道等,目前主要用于AACLC AACLD[3/16/2012 chendaiwei]

    u8 m_abyMtIpV6[IPV6_STR_LEN];    //终端IPV6地址
    u8 m_byMtIpType;                        //终端实际IP类型（IPV4/IPV6）
	TMtAlias     m_tConfE164Alias; 
	
	TAudioTypeDesc m_atLocalAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];//会议音频能力集

    RTPSndAddress   m_atAudSndRtpAddr;       //远端发送通道的RTP音频发送地址
    RTPSndAddress   m_atVidSndRtpAddr;       //远端发送通道的RTP视频发送地址
    RTPSndAddress   m_atSecSndRtpAddr;       //远端发送通道的RTP双流发送地址
	RTPSndAddress   m_atH224SndRtpAddr;      //远端发送通道的RTP数据信道发送地址
	BOOL32			m_bIsNeedPinHold;		 //对端是否需要打洞
	
    TMt      m_tRecSrcMt;             //新录播录像源支持
	u8		 m_byRecType;			  //新录播录放像Type

public:
    BOOL32 H323Init();

	//-------------Daemon Instance ---------------
	void DaemonInstanceEntry( CMessage * const pcMsg, CApp * pcApp );
	void DaemonInstanceDump ( u32 dwParam = 0 );
	void DaemonProcPowerOn  ( void );
	void DaemonProcAppTaskRequest( CMessage * const pcMsg );	
	void DaemonProcMcuMtQosSetCmd( CMessage * const pcMsg );    //qos set to stack
	void DaemonProcMtAdpCfgCmd( CMessage * const pcMsg );       //mcu cfg set to mtadp
	void DaemonProcConnectMcu    ( BOOL32 bMcuA );				//connect to mcu vc
	void DaemonProcRegisterMcu   ( BOOL32 bMcuA );				//register itself to mcu vc
	void DaemonProcMcuRegisterRsp( CMessage * const pcMsg ); 	//proc the registration rsp
	void DaemonProcMcuDisconnect ( CMessage * const pcMsg, CApp* pcApp );	// mcu disconnected
	void DaemonProcInviteMtReq ( CMessage * const pcMsg, CApp* pcApp );
	void DaemonProcJoinMmcuReq ( CMessage * const pcMsg, CApp* pcApp );
	void DaemonProcMcuMtGeneral( CMessage * const pcMsg, CApp* pcApp );	
	void DaemonProcGkMcuGeneral( CMessage * const pcMsg, CApp* pcApp );
	void DaemonProcMtMcuGeneral( CMessage * const pcMsg, CApp* pcApp );
	void DaemonProcMtMcuConfMsg( CMessage * const pcMsg );
    void DaemonProcGetMsStatusRsp( CMessage * const pcMsg, CApp* pcApp );

    void DaemonProcConnectToGK();
    void DaemonDisconnectFromGK();    
    void DaemonProcRegToGK( /*CMessage * const pcMsg*/ );
    void DaemonProcRegGKRsp( CMessage * const pcMsg );
	void DaemonProcMcuMtChargeStopReq( CMessage * const pcMsg );
    void DaemonProcGKMcuChargeStopRsp( CMessage * const pcMsg );
    void DaemonProcMcuMtChargeStartReq( CMessage * const pcMsg );
    void DaemonProcGKMcuChargeStartRsp( CMessage * const pcMsg );
	void DaemonProcMcuGKConfChargeStatusNtf( /*CMessage * const pcMsg*/ );
	void DaemonProcGKMcuConfChargeStatusNtf( CMessage * const pcMsg );
    void DaemonProcNPlusUnregGKCmd( CMessage * const pcMsg );
    void DaemonProcNPlusStopChargeCmd( CMessage * const pcMsg );
    void DaemonProcMcuE164RestoreNtf( CMessage * const pcMsg );
    void DaemonProcCallNextMtNtf( void );
    void DaemonCallNextFromQueue( TMtCallInfo &tCallInfo );
    void DaemonDelMtFromQueue( u8 byConfIdx, u8 byMtId );
    void DaemonProcURQTimeOut( void );
	void DaemonPronRRQTimeOut( void );
    void DaemonProcGetStackResFailed( void );
	void DaemonProcMMcuConfNameShowTypeCmd( CMessage * const pcMsg );
	void DaemonProcMMcuChangeAdminLevelCmd( CMessage * const pcMsg );

public:
	//-----------------RAS--------------------
	BOOL32 DaemonProcSendRRQ( CMessage * constpcMsg );
	BOOL32 DaemonProcSendURQ( CMessage * constpcMsg );	
	BOOL32 DaemonProcDisengageOnGK();	
//	BOOL32 DaemonProcResendRRQReq( CMessage * const pcMsg );
	BOOL32 DaemonProcUpdateRRQNtf( CMessage * const pcMsg );
	BOOL32 DaemonProcUpdateGKANDEPIDNtf( CMessage * const pcMsg );
	BOOL32 DaemonProcUpdateGkAddrCmd( /*CMessage * const pcMsg, */CApp* pcApp );
	BOOL32 ProcSendARQ( TMtAlias &tSrcAddress, TMtAlias &tDstAddress, u16 wBandwidth );	
	BOOL32 ProcSendIRR();
	BOOL32 ProcSendBRQ( s32 nRate, u8 byDirect );
	void   ProcOnRecvDRQ( TDRQINFO *ptDRQINF );
    void   SendRRQUpdateMcuE164( TRASInfo * ptRASInfo = NULL );

private:
	//----------- Instance Entry --------------
	void InstanceEntry( CMessage * const  pcMsg );
	void InstanceDump ( u32 param = 0 ); 

	void ProcStdEvMcuMt( CMessage * const pcMsg );
	void ProcEvInterMcu( CMessage * const pcMsg );
//	void ProcRasEvMcuMt( CMessage * const pcMsg );
	void ProcKdvCustomEvMcuMt( CMessage * const pcMsg );
	void ProcVCMsgMcuMcu(CMessage * const pcMsg);

	void ProcMcuCreateConfReq( CMessage * const  pcMsg );
	void ProcMcuCreateConfRsp( CMessage * const  pcMsg );
	void ProcIncomingCall(BOOL32 bLowerMcuCallingIn = FALSE);
	
	void ProcMcuMtInviteMtReq( CMessage * const  pcMsg ); //invite mt
	void CallOutByTransportAddr( BOOL32 bIsIgnoreVcIpCheck = FALSE );	
	void CallProceed( cmTransportAddress *ptTransportAddr, s32 nApprovedBandwidth, s32 nRrrFrequency = 0 );
	
	void FlowControlProceed( s32 nRate );
	void ProcMcuMtOpenLogicChannelReq ( CMessage * const  pcMsg ); 
	void ProcMcuMtCloseLogicChannelCmd( CMessage * const  pcMsg ); 
	void ProcMcuMtOpenLogicChannelRsp ( CMessage * const  pcMsg ); 
	void ProcMcuMtJoinedMtInfoRsp ( CMessage * const  pcMsg );
	void ProcMcuMtEnterPasswordReq( CMessage * const  pcMsg );
	void ProcMcuMtFeccCmd( CMessage * const  pcMsg );
	void ProcMcuMtH239TokenGeneralInd( CMessage * const  pcMsg );
    void ProcMcuMtMediaLoopOnRsp( CMessage * const pcMsg );

	void ProcMcuMtGeneralCmd( CMessage * const  pcMsg );    //handle general MCU->MT cmd's
	void ProcMcuMtGeneralInd( CMessage * const  pcMsg );	//handle general MCU->MT ind's
	void ProcMcuMtGeneralRsp( CMessage * const  pcMsg );    //handle general MCU->MT rsp's

    void ProcMtAdpReRegisterMcu( /*CMessage * const pcMsg,*/ CApp* pcApp );
    void CallNextFromQueue( u8 byConfIdx, u8 byMtId );
	
	void ProcMcuVrsStartRecReq( CMessage * const  pcMsg );		//开启录像请求
	void ProcMcuVrsListAllRecordReq( CMessage * const  pcMsg );	//获取文件列表
	void ProcMcuVrsStartPlayReq( CMessage * const  pcMsg );		//开启放像请求
	void ProcMcuVrsSeekReq( CMessage * const  pcMsg );			//放像进度拖动
	//void ProcMcuVrsGetVrsInfo( CMessage * const  pcMsg );		//获得Vrs相关信息，状态或进度等
	void ProcMcuSendMsgtoVrs(u16 wEventId, u8* pBuf = NULL, u16 nBufLen = 0);//给Vrs发非标消息
	void ProcMcuGetMsgFormVrs(CServMsg& cMsg, u16 wEventId, u16 wBufLen, u8* pbyBuf);//收到Vrs发来的消息
	void ProcVrsRspTimeout( void );//等待vrs应答超时
	u8	 GetRecStateByVrsState(u8 byVrsState, u8 byRecType);//转换vrs录播服务器上报的状态

	//---------- H323 MT --> MCU ------------
	void ProcMtMcuMsdRsp( u8 byMsdResult );
			
	//---------- Link Maintenance------------
	void ProcMcuMtRoundTripDelayReq( CMessage * const  pcMsg );
	void ProcTimerExpired          ( CMessage * const  pcMsg );


private:
	//------------ Utilities ----------------
	BOOL32 SendMsgToMcuInst( const CServMsg &cServMsg );
	void SendMsgToMcu( CServMsg &cServMsg );
	void SendMsgToMcuDaemon( u16 wEvent, u8 * const pbyMsg, u16 wLen );
    void SendMsgToGK( u16 wEvent, u8 * const pbyMsg, u16 wLen );
	void BuildAndSendMsgToMcu ( u16 wEvent, u8* const pbyMsg  = NULL, u16 wLen  = 0 );	
	void BuildAndSendMsgToMcu2( u16 wEvent, u8* const pbyMsg1 = NULL, u16 wLen1 = 0,
										    u8* const pbyMsg2 = NULL, u16 wLen2 = 0 );
	s32  FindChannel( HCHAN hsChan );
	s32  FindChannelByPayloadType( u8 byChanDirect, u8 byPayloadType, u8 byMode );
	s32	 FindChannelByMediaType  ( u8 byChanDirect, u8 byMediaType ); 
	s32	 GetFreeChannel();
	HCHAN GetMMcuChannel();
	void FreeChannel( s32 nChanIdx );
	void FreeChannel( HCHAN hsChan );

    emMtVer GetPeerMtVer(HCALL hsCall);
    BOOL32 IsPeerMtSerialTS(HCALL hsCall);
    BOOL32 IsPeerMtPcmt(HCALL hsCall);
    BOOL32 IsLocalMcu8000C();
    BOOL32 IsCallingSupported(HCALL hsCall);

    BOOL IsChanG7221CSupport();
    BOOL GetCapVideoParam(TVideoCap &tVidCap, BOOL32 bDual, u8 &byRes, u8 &byFps);
    BOOL GetCapVideoParam(TH264VideoCap &tVidCap, BOOL32 bDual, u8 &byRes, u8 &byFps);   
    
    //开始 GK计费
    void GkChargeStart( TConfChargeInfo &tChargeInfo, u8 byConfIdx );
	void ClearChargeData( u8 byConfIdx = 0 );

    void MAPrint ( const u8 byLevel, const u16 wModule, const s8* pszFormat, ...);
// 	void MtAdpPrintf ( u8 byLevel, BOOL32 bToScreen, BOOL32 bToFile, s8* pchPrintStr );
//     void MtAdpDetail ( s8* pszFmt, ... );
// 	void MtAdpVerbose( s8* pszFmt, ... );
// 	void MtAdpInfo   ( s8* pszFmt, ... );
// 	void MtAdpWarning( s8* pszFmt, ... );	
// 	void MtAdpException( s8* pszFmt, ... );
// 	void MtAdpCritical ( s8* pszFmt, ... );
	void DoStatistics( u32 dwState );
	
    // guzh [5/9/2007] 支持通报对端挂断原因。这里的byCause两边共用
	void ClearInst( u16 wReportEvent = 0, u8 byCause = 0, u8 byMsgRemote = 0 );
    void ClearInstMtFromQueue( void );
	
	// [11/21/2011 liuxu] 发送SMcuMtList Buffer
	void SendSMcuMtListInBuf( const BOOL32 byLastPack );
private:
	//mcu-mcu send message
	void OnSendMultiCascadeReqMsg( CServMsg &cServMsg, HCHAN hsChan );	
	void OnSendMultiCascadeRspMsg( CServMsg &cServMsg, HCHAN hsChan );
	void OnSendMultiCascadeNtfMsg( CServMsg &cServMsg, HCHAN hsChan );
	void OnSendRosterNotify ( const CServMsg &cServMsg, HCHAN hsChan );
	void OnSendMtListAck    ( const CServMsg &cServMsg, HCHAN hsChan );
	void OnSendVideoInfoAck ( const CServMsg &cServMsg, HCHAN hsChan );
	void OnSendAudioInfoAck ( const CServMsg &cServMsg, HCHAN hsChan );
	void OnSendInviteMtReq  ( const CServMsg &cServMsg, HCHAN hsChan, const TReq &tReq, const TPartID &tPardId );
	void OnSendReInviteMtReq( const CServMsg &cServMsg, HCHAN hsChan, TReq tReq, TPartID tPardId );
	
	void OnSendCallAlertMtNotify( const CServMsg &cServMsg, HCHAN hsChan );
	void OnSendNewMtNotify ( CServMsg &cServMsg, HCHAN hsChan );
	void OnSendSetOutReq   ( CServMsg &cServMsg, HCHAN hsChan );
	void OnSendSetOutNotify( CServMsg &cServMsg, HCHAN hsChan );
	void OnSendDropMtReq   ( CServMsg &cServMsg, HCHAN hsChan, TReq tReq, TPartID tPardId );
	void OnSendDelMtReq    ( CServMsg &cServMsg, HCHAN hsChan, TReq tReq, TPartID tPardId );
	void OnSendSetMtChanReq( CServMsg &cServMsg, HCHAN hsChan );
	void OnSendSetInReq    ( CServMsg &cServMsg, HCHAN hsChan, TReq tReq, TPartID tPardId );
    void OnSendAutoSwitchReq( CServMsg &cServMsg, HCHAN hsChan );

	void OnSendStartMixerCmd  ( CServMsg &cServMsg, HCHAN hsChan );
	void OnSendStartMixerNotif( CServMsg &cServMsg, HCHAN hsChan );
	void OnSendStopMixerCmd   ( CServMsg &cServMsg, HCHAN hsChan );
	void OnSendStopMixerNotif ( CServMsg &cServMsg, HCHAN hsChan );
	void OnSendGetMixerParamReq ( CServMsg &cServMsg, HCHAN hsChan );
	//void OnSendGetMixerParamAck ( CServMsg &cServMsg, HCHAN hsChan );
	void OnSendGetMixerParamNack( CServMsg &cServMsg, HCHAN hsChan );
	void OnSendMixerParamNotif  ( CServMsg &cServMsg, HCHAN hsChan );
	void OnSendAddMixerMemberCmd( CServMsg &cServMsg, HCHAN hsChan );
	void OnSendRemoveMixerMemberCmd( CServMsg &cServMsg, HCHAN hsChan );

	void OnSendLockMcuReq ( CServMsg &cServMsg, HCHAN hsChan );
	void OnSendLockMcuAck ( CServMsg &cServMsg, HCHAN hsChan );
	void OnSendLockMcuNack( CServMsg &cServMsg, HCHAN hsChan );
	void OnSendMtStatusCmd( CServMsg &cServMsg, HCHAN hsChan );
	void OnSendMtStatusNtf( CServMsg &cServMsg, HCHAN hsChan );

	void OnSendMsgNtf( CServMsg &cServMsg, HCHAN hsChan );

	void OnSendAdjustMtResReq( CServMsg &cServMsg, HCHAN hsChan );
	void OnSendAdjustMtResAck( CServMsg &cServMsg, HCHAN hsChan );
	void OnSendAdjustMtResNack( CServMsg &cServMsg, HCHAN hsChan );

	// [pengjie 2010/4/23] 级联降帧率
	void OnSendAdjustMtFpsReq( CServMsg &cServMsg, HCHAN hsChan );
	void OnSendAdjustMtFpsAck( CServMsg &cServMsg, HCHAN hsChan );
	void OnSendAdjustMtFpsNack( CServMsg &cServMsg, HCHAN hsChan );
	// End

	void OnSendAdjustMtBitrateCmd( CServMsg &cServMsg, HCHAN hsChan );

	void OnSendAudMuteReq( CServMsg &cServMsg, HCHAN hsChan );
	//void OnSendAudMuteAck( CServMsg &cServMsg, HCHAN hsChan );
	//void OnSendAudMuteNack( CServMsg &cServMsg, HCHAN hsChan );
	//void OnSendVcsMuteReq( CServMsg &cServMsg, HCHAN hsChan );

	// [pengjie 2010/8/12] 级联远摇
	void OnSendMMcuFeccCmd( CServMsg &cServMsg, HCHAN hsChan );
	// End

	void OnSendApplySpeakerReq( CServMsg &cServMsg, HCHAN hsChan );
    void OnSendApplySpeakerAck( CServMsg &cServMsg, HCHAN hsChan );
    void OnSendApplySpeakerNack( CServMsg &cServMsg, HCHAN hsChan );
    void OnSendApplySpeakerNotif( CServMsg &cServMsg, HCHAN hsChan );

	void OnSendCancelMeSpeakerReq( const CServMsg &cServMsg, HCHAN hsChan );
    void OnSendCancelMeSpeakerAck( const CServMsg &cServMsg, HCHAN hsChan );
    void OnSendCancelMeSpeakerNack( const CServMsg &cServMsg, HCHAN hsChan );

    //[5/4/2011 zhushengze]VCS控制发言人发双流
    void OnSendMsgChangeMtSecVidSendCmd( const CServMsg &cServMsg, HCHAN hsChan );

    //[2/23/2012 zhushengze]界面、终端消息透传
    void OnSendMsgTransparentMsgNtf( const CServMsg &cServMsg, HCHAN hsChan );

	//mcu-mcu receive message
	//void ProcRegunregReq( CServMsg &cMsg, u8* pbyBuf, s32 nBufLen );
	void ProcNewRosterNotify( CServMsg &cMsg, u8* pbyBuf, s32 nBufLen );
	
	void ProcPartListReq( CServMsg &cMsg, u8* pbyBuf, s32 nBufLen );
	void ProcPartListRsp( CServMsg &cMsg, u8* pbyBuf, s32 nBufLen );
	void ProcInviteMtReq( CServMsg &cMsg, u8* pbyBuf, s32 nBufLen );
	void ProcInviteMtRsp( CServMsg &cMsg, u8* pbyBuf, s32 nBufLen );	
	void ProcNewMtNotify( CServMsg &cMsg, u8* pbyBuf, s32 nBufLen );
	void ProcReInviteMtReq( CServMsg &cMsg, u8* pbyBuf, s32 nBufLen );
	
	void ProcAudioInfoRsp( CServMsg &cMsg, u8* pbyBuf, s32 nBufLen );
	void ProcVideoInfoRsp( CServMsg &cMsg, u8* pbyBuf, s32 nBufLen );
	
	void ProcCallAlertMtNotify( CServMsg &cMsg, u8* pbyBuf, s32 nBufLen );	
	void ProcSetOutReq( CServMsg &cMsg, u8* pbyBuf, s32 nBufLen );
	void ProcSetOutNtf( CServMsg &cMsg, u8* pbyBuf, s32 nBufLen );
	void ProcSetInReq ( CServMsg &cMsg, u8* pbyBuf, s32 nBufLen );

	void ProcDropMtReq( CServMsg &cMsg, u8* pbyBuf, s32 nBufLen );
	void ProcDelMtReq ( CServMsg &cMsg, u8* pbyBuf/*, s32 nBufLen*/ );
	
	void ProcSetPartChanReq( CServMsg &cMsg, u8* pbyBuf, s32 nBufLen );
	void ProcNonStandardMsg( CServMsg &cMsg, u8* pbyBuf, s32 nBufLen );

	void ProcMcuMcuStartMixerCmd  ( const CServMsg &cServMsg, CServMsg &cMsg );
	void ProcMcuMcuStartMixerNotif( const CServMsg &cServMsg, CServMsg &cMsg );
	void ProcMcuMcuStopMixerCmd   ( const CServMsg &cServMsg, CServMsg &cMsg );
	void ProcMcuMcuStopMixerNotif ( const CServMsg &cServMsg, CServMsg &cMsg );

	void ProcMcuMcuGetMixerParamReq  ( const CServMsg &cServMsg, CServMsg &cMsg );
	void ProcMcuMcuGetMixerParamAck  ( const CServMsg &cServMsg, CServMsg &cMsg );	
	void ProcMcuMcuGetMixerParamNack ( const CServMsg &cServMsg, CServMsg &cMsg );
	void ProcMcuMcuGetMixerParamNotif( const CServMsg &cServMsg, CServMsg &cMsg );
	void ProcMcuMcuAddMixerMemeberCmd( const CServMsg &cServMsg, CServMsg &cMsg );
	void ProcMcuMcuRemoveMixerMemeberCmd( const CServMsg &cServMsg, CServMsg &cMsg );

	void ProcMcuMcuLockReq ( const CServMsg &cServMsg, CServMsg &cMsg );
	void ProcMcuMcuLockAck ( const CServMsg &cServMsg, CServMsg &cMsg );
	void ProcMcuMcuLockNack( const CServMsg &cServMsg, CServMsg &cMsg );

	void ProcMcuMcuMtStatusCmd( const CServMsg &cServMsg, CServMsg &cMsg );
	void ProcMcuMcuMtStatusNtf( const CServMsg &cServMsg, CServMsg &cMsg );

	void ProcMcuMcuMsgNtf( const CServMsg &cServMsg, CServMsg &cMsg );

	void ProcMcuMcuSpeakStatusNtf( CServMsg &cServMsg, CServMsg &cMsg );

    void ProcMcuMcuApplySpeakerReq( CServMsg &cServMsg, CServMsg &cMsg );
    void ProcMcuMcuApplySpeakerRsp( CServMsg &cServMsg, CServMsg &cMsg );
    void ProcMcuMcuApplySpeakerNtf( CServMsg &cServMsg, CServMsg &cMsg );
	void OnSendSpeakStatusNotify( CServMsg &cServMsg,HCHAN hsChan );
	void OnSendMcuMcuSpeakModeNotify( CServMsg &cServMsg,HCHAN hsChan );
    void ProcMcuMcuCancelMeSpeakerReq( CServMsg &cServMsg, CServMsg &cMsg );
    void ProcMcuMcuCancelMeSpeakerRsp( CServMsg &cServMsg, CServMsg &cMsg );
	void ProcMcuMcuSpeakModeNotify( CServMsg &cServMsg, CServMsg &cMsg );

	void ProcMcuMcuAdjMtResReq ( const CServMsg &cServMsg, CServMsg &cMsg );
	void ProcMcuMcuAdjMtResRsp ( const CServMsg &cServMsg, CServMsg &cMsg );

	// [pengjie 2010/4/23] 级联调帧率
	void ProcMcuMcuAdjMtFpsReq ( const CServMsg &cServMsg, CServMsg &cMsg );
	void ProcMcuMcuAdjMtFpsRsp ( const CServMsg &cServMsg, CServMsg &cMsg );
	// End

	void ProcMcuMcuAdjMtBitrateCmd( const CServMsg &cServMsg, CServMsg &cMsg );

	void ProcMcuMcuMuteDumbReq ( const CServMsg &cServMsg, CServMsg &cMsg );
	void ProcMcuMcuMuteDumbRsp ( CServMsg &cServMsg, CServMsg &cMsg );

	// [pengjie 2010/8/12] 级联远摇
	void ProcMMcuFeccCmd( const CServMsg &cServMsg, CServMsg &cMsg );
	// End
    //[5/4/2011 zhushengze]VCS控制发言人发双流
    void ProcMMcuChangeMtSecVidSendCmd(const CServMsg &cServMsg, CServMsg &cMsg );

	//[2/23/2012 zhushengze]界面、终端消息透传
    void ProcMMcuTransparentMsgNtf( const CServMsg &cServMsg, CServMsg &cMsg );

	//  [pengguofeng 7/1/2013]
	void ProcMcuMtadpRequestMtList(u8 bMMcuReq, u8 byMcuMtId = 0xFF);
	
	// [yanghuaizhi 8/7/2013]
	void ProcMcuMtadpRequestMtAlias(const CServMsg &cServMsg);

	void SetPeerReqId( u32 dwPeerReqId );
	u32  GetPeerReqId();

	void MtInfo2Part( TPart *ptPart, TMcuMcuMtInfo *ptInfo );
	void Part2MtInfo( TPart *ptPart, TMcuMcuMtInfo *ptInfo );

	void VideoInfoIn2Out( TCConfViewInfo*  ptInInfo, TConfVideoInfo* ptOutInfo );
	void VideoInfoOut2In( TCConfViewInfo*  ptInInfo, TConfVideoInfo* ptOutInfo );
	void AudioInfoIn2Out( TCConfAudioInfo* ptInInfo, TConfAudioInfo* ptOutInfo );
	void AudioInfoOut2In( TCConfAudioInfo* ptInInfo, TConfAudioInfo* ptOutInfo );

	void VideoSchemeIn2Out( TOutPutVideoSchemeInfo* ptOInfo, TCSchemeInfo* ptIInfo );
	void VideoSchemeOut2In( TOutPutVideoSchemeInfo* ptOInfo, TCSchemeInfo* ptIInfo );
	
	TMt		GetMtFromPartId( u32 dwPartId, BOOL32 bLocal = FALSE );
	u32		GetPartIdFromMt( TMt tMt,      BOOL32 *pbLocal );
	BOOL32	AddPartIdTable( u32 dwPartId );
	void    PartIdUpdate  ( u32 dwOldPartId, u32 dwNewPartId, BOOL32 bLocal );
	void    DelPartIdFromTable( u32 dwPartId );

    BOOL32  SetGKRasAddress(cmTransportAddress	&tGKAddr);
	
	BOOL32  IsR2WJMCU();

	BOOL32  IsMtHasVidCap();

    //[4/8/2013 liaokang] 终端编码方式    
    void    SetEndPointEncoding(emenCodingForm emEPCodingForm);
    emenCodingForm GetEndPointEncoding();
	// 添加参数：bOut：表示出还是入 [pengguofeng 5/20/2013]
	BOOL32	TransEncoding(const s8 *pSrc, s8 *pDst, s32 nlen, BOOL32 bOut = TRUE);

	//zjj20100201 多回传代码合并
	//  pengjie[9/28/2009] 级联多回传
	//  能力互探	
	void OnSendMultSpyCapNotif( CServMsg &cServMsg, HCHAN hsChan );
	void ProcMcuMcuMultSpyCapNotif( const CServMsg &cServMsg, CServMsg &cMsg );

    //   进行多级联准备（双方握手）
    void OnSendPreSetinReq( CServMsg &cServMsg, HCHAN hsChan );
	void OnSendPreSetinAck( CServMsg &cServMsg, HCHAN hsChan );
	void OnSendPreSetinNack( CServMsg &cServMsg, HCHAN hsChan );

    void ProcMcuMcuPreSetinReq( const CServMsg &cServMsg, CServMsg &cMsg );
	void ProcMcuMcuPreSetinAck( const CServMsg &cServMsg, CServMsg &cMsg );
	void ProcMcuMcuPreSetinNack( const CServMsg &cServMsg, CServMsg &cMsg );

	//   收发关键帧
	void OnSendSpyFastUpdateCmd( CServMsg &cServMsg, HCHAN hsChan );
	void ProcMcuMcuSpyFastUpdateCmd( CServMsg &cServMsg, CServMsg &cMsg );

	//   通知下级mcu，上级可以接受其回传码流的带宽
	void OnSendSpyChnnlNotif( CServMsg &cServMsg, HCHAN hsChan );
	void ProcMcuMcuSpyChnnlNotif( const CServMsg &cServMsg, CServMsg &cMsg );

	//通知上级mcu，下级建交换到上级mcu失败
	void OnSendSwitchToMMcuFailNotif(CServMsg &cServMsg, HCHAN hsChan);
	void ProcMcuMcuSwitchToMMcuFailNotif(const CServMsg &cServMsg, CServMsg &cMsg);

	//   通知下级mcu，上级mcu带宽不足，不能进行级联多回传
	void OnSendRejectSpyNotif( CServMsg &cServMsg, HCHAN hsChan );
    void ProcMcuMcuRejectSpyNotif( const CServMsg &cServMsg, CServMsg &cMsg );

	//   上级mcu发给下级的拆除相应的交换，回收回传通道
    void OnSendBanishSpyCmd( CServMsg &cServMsg, HCHAN hsChan );
    void ProcMcuMcuBanishSpyCmd( const CServMsg &cServMsg, CServMsg &cMsg );
	
	//   [nizhijun 2010/12/15]下级MCU发给上级MCU的RTCP信息通告
    void OnSendMtExtInfoNotify( CServMsg &cServMsg, HCHAN hsChan );
    void ProcMcuMcuMtExtInfoNotify( const CServMsg &cServMsg, CServMsg &cMsg );

	//   下级mcu发给上级的释放一路音、视频回传通道的通知，上级收到后释放自己相应的音、视频回传通道资源
	void OnSendBanishSpyNotif( CServMsg &cServMsg, HCHAN hsChan );
	void ProcMcuMcuBanishSpyNotif( const CServMsg &cServMsg, CServMsg &cMsg );
	void HeadMsgToMtadpHeadMsg( const TMsgHeadMsg &tHeadMsg ,TMtadpHeadMsg &tMtadpHeadMsg );
	void MtadpHeadMsgToHeadMsg( const TMtadpHeadMsg &tHeadMsg ,TMsgHeadMsg &tMtadpHeadMsg );
};

enum GKREG_STATE
{
	STATE_NULL, 
	STATE_REG_INPROGRESS, 	
	STATE_REG_OK
};

struct TMaStaticInfo
{
public:
    TMaStaticInfo( void ) { memset(this, 0, sizeof(TMaStaticInfo)); }

    u32	m_dwIncomingCalls;	//total number of incoming calls since the system's startup
    u32	m_dwOutgoingCalls;	//total number of outgoing calls since the system's startup
    
    u32 m_dwSuccessfulCalls;//total number of successful calls since the system's startup
    u32 m_dwFailedCalls;	//total number of failed calls since the system's startup
    
    u8	m_byCurNumOnline;	//current number of h245-connected calls
    u8	m_byCurNumCalling;	//current number of h225 callings
    
    u8	m_byMaxNumOnline;	//maximum number of simultaneous online terminals
    u8	m_byMaxNumCalling;	//maximum number of simultaneous callingterminals
    
    BOOL32 m_bH323InitFailed;   //是否323stack初始化失败
    
    u32 m_dwSndMtListReqNum;
    u32 m_dwRcvMtListReqNum;
    u32 m_dwSndAudInfoReqNum;
    u32 m_dwRcvAudInfoReqNum;
    u32 m_dwSndVidInfoReqNum;
    u32 m_dwRcvVidInfoReqNum;
};

class CMtAdpData
{
public: 	
	CMtAdpData();
	virtual ~CMtAdpData();
	
public: 
	u32     m_dwMcuNode;			        //Mcu node id
	u32     m_dwMcuIId;				        //IId of the APP on MCU we talk to
	u32     m_dwVcIpAddr;                   //Mcu IP (net order) get from the registration response of MCU
	s8      m_achMcuAlias[MAXLEN_ALIAS];	//Mcu Alias get from the registration response of MCU
    u8      m_byCasAliasType;               //Mcu Alias display style
    u8      m_byCascadeLevel;	

	u32		m_dwMpcSSrc;      // guzh [6/12/2007] 业务侧会话校验值
	
	//used when doublelinking
	u32     m_dwMcuNodeB;			        //Mcu node id
	u32     m_dwMcuIIdB;				    //IId of the APP on MCU we talk to
	u32     m_dwVcIpAddrB;                  //Mcu IP (net order) get from the registration response of MCU	

	//conference & MT are numbered starting from 1 
	u8      m_Msg2InstMap[MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1][MAXNUM_CONF_MT + 1];
	BOOL32  m_bMtAdpInited;
	BOOL32  m_bH323PollWatchdog;

	BOOL32  m_bAttachedToVc;		        //MtAdp attaching mode ( 1-MCU, other-independent )
	u8      m_byDriId;				        //MtAdp Dri number
	s8      m_achMtAdpAlias[16];	        //MtAdp alias
	u32     m_dwMtAdpIpAddr;                //MtAdp Ip address (net order) 

	u16     m_wMcuNetId;			        //Mcu number
	u32     m_dwMcuIpAddr;			        //Mcu IP (net order) to connect to - no use
	u16     m_wMcuPort;				        //Mcu Port (host order) to connect to - no use
	u32     m_dwMcuIpAddrB;			        //Mcu IP (net order) to connect to (used when doublelinking) - no use
	u16     m_wMcuPortB;			        //Mcu Port (host order) to connect to (used when doublelinking) - no use
	BOOL32  m_bDoubleLink;                  //Mcu Linking mode ( 1-DoubleLink, other-SingleLink )

	u16     m_wQ931Port;                    //Q.931 port
	u16     m_wRasPort;                     //Ras port
	u16     m_wH225H245Port;                //H225H245 port
	u16     m_wH225H245MtNum;               //H225H245 max mt limit num 
	BOOL32  m_bIsRespDRQFromGK ;            //  [12/11/2009 pengjie] 是否响应gk的DRQ

	u16     m_wMaxRTDInterval;              //interval of sending RTD probing package
	u8      m_byMaxRTDFailTimes;            //if rtd failed m_byMaxRTDFailTimes times, we drop the call
	BOOL32	m_byDebugLevel;       
	HAPP    m_hApp;

    u8      m_byMaxCallNum;                 //max num of mt to be call out together
    BOOL32  m_bUseCallingMatch;             //whether use calling-match when call out or call in
    
    emenCodingForm m_emMcuEncoding;         //[4/8/2013 liaokang] Mcu编码方式
    emenCodingForm m_emGkEncoding;          //[4/8/2013 liaokang] GK 编码方式
	u8      m_byStackInitUtf8;				// 323协议栈 [pengguofeng 8/29/2013]

	BOOL32  m_bSupportSlice;				//whether support slice

	BOOL32	m_bSupportTaideDsCap;			//zjj 2009-08-27泰德终端呼叫时如果没有双流能力，是否作弊加入双流能力

	u8		m_abyInvitedMsgInOspMt[MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE][MAXNUM_CONF_MT / 8 + 1];//zjj 2011-03-15 呼叫消息是否已经投入osp队列
	u8		m_abyNeglectInviteMsgMt[MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE][MAXNUM_CONF_MT / 8 + 1];//zjj 2011-03-15 是否需要忽略邀请消息

    //------------------------- NPLUS -------------------------
    BOOL32      m_bURQForNPlus;             //whether the ucf is come from the urq for nplus
    TMtAlias	m_tMcuAliasBak;             //backup MCU's alias in N+1 mode

    //---------------------- CONF CHARGE ----------------------
    BOOL32  m_bIsGKCharge;                  // whether the conferene going to be charged which RRQ by this mtadp
    u32     m_dwGKNode;                     // GK node id which used to trans the conf charge info
    u32     m_dwGKIId;                      // GK Inst id which used to trans the conf charge info
    BOOL32  m_bGKReged;                     // whether the master mtadp is registered to gk successfully
    TAcctSessionId  m_atChargeSsnId[MAX_CONFIDX];   // Session id to identify the right charged conf
    TConfChargeInfo m_atChargeInfo[MAX_CONFIDX];    // charge info belong to the right charged conf
    u8      m_abyRestart[MAX_CONFIDX];              // whether the conference is restart charge by this mtadp
    
    //---------------------- RAS ------------------------------

	cmTransportAddress	m_tGKAddr;	        //GK address, net order
    cmTransportAddress	m_tGKAddrOld;
    
	BOOL32      m_bGkAddrSet;
	BOOL32      m_bGkRegistered;	
	BOOL32      m_bGotRRJOrURQFromGK;       //this means we should try a new RRQ
	BOOL32      m_bMasterMtAdp;             //是否为主适配板 TRUE: 主，由适配板完成RRQ；FALSE, 辅，由其他适配板完成
	BOOL32      m_bAlreadyNtfGKEPID;        //是否已通知由自身完成完成RRQ时的GKID/EPID认证信息
	TH323EPGKIDAlias m_tGKID;               //GatekeeperID，用于其他适配板完成RRQ时的认证信息记录
	TH323EPGKIDAlias m_tEPID;               //EndpointID，用于其他适配板完成RRQ时的认证信息记录
    BOOL32      m_bWaitingURQRsp;           //主备链路同时不信任发起的URQ标识
                                            //FIXME: 本标识适应当前的VC逻辑,若URQ的逻辑变更，则本标识对应的逻辑均需作出调整
    u8          m_byGetStackResFailedTimes; //获取协议栈资源失败次数记录


	//item 0 for mcu alias, items 1..(MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE)
	//for conf aliases, item (MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1) for
	//lightweight registration handle, the last one for re-registration handle
	HRAS		m_hsRas[MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 3];
	TMtAlias    m_tMcuH323Id;
	TMtAlias	m_atMcuAlias[MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1];     //entry 0 is reserved for MCU ID
	GKREG_STATE m_gkConfRegState[MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1]; //state of conf gk registration
	GKREG_STATE m_gkRegState;               //state of gk registration of mcu alias & confs as a whole
    TTransportAddr  m_atMtadpAddr[MAXNUM_DRI];      //多适配板地址向gk注册
		
	//circular queue to searialize the handling of RRQ's from conf and template
	u8			m_abyRRQFifoQueue[MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1];
	s32			m_nReqHead;
	s32			m_nReqTail;
	
	HRAS		m_hsCurrentRRQRas; //最近一次RRQ的RAS句柄,超时该句柄不再更新（记录第一次超时的句柄），直到收到该
	//RRQ的RRJ（来自协议栈超时）[12/26/2012 chendaiwei]
	u8			m_byCurrentRRQConIdx; //最近一次RRQ的会议idx,该idx随m_hsCurrentRRQRas设置[12/26/2012 chendaiwei]

    //--------------------- STATISTICS ------------------------
	TMaStaticInfo   m_tMaStaticInfo;
    TMtQueue        m_tCallQueue;
    TMtQueue        m_tWaitQueue;
	TKdvTime		m_tExpireDate;		
	u16				m_wMaxNumConntMt;	//最大终端接入数
	u16				m_wCurNumConntMt;	//当前终端接入数// xliang [1/4/2009] 
	u8				m_byMaxHDMtNum;     //高清接入点数目 	//  [7/27/2011 chendaiwei]
	u16             m_wMaxNumConnAudMt; //最大接入语音终端  // zjl 20120814
	u8				m_abyConnMcuMtList[MAXNUM_CONF_MT];//记录接入能力分配的是Mcu或Mt列表 xliang [1/4/2009] 
	u8				m_byLicenseErrCode;
	TMtAdpDSCaps    m_tDSCaps;          //最大暂支持对10种类型终端的双流Debug处理
    u32 m_dwRestrictManuTable;                //限制呼叫的厂商(每一位代表一个厂商，低位起算) [20120206 zhushz]
	s8				m_achFakedProductId[MAX_ProductIDSize];
	u8              m_abyCRIMacAddr[6]; //Mtadp所属接入板(CRI，CRI2，IS2.2)MaC地址
#ifdef _IS22_
	u16				m_wDiscHeartBeatTime;
	u8				m_byDiscHeartBeatNum;
#endif
	
	u8				m_byIsGkUseRRQPwd;				//是否GK启用密码发送RRQ
	s8				m_achRRQPassword[REG_PWD_LEN];	//注册密钥
	
	u32				m_dwPinHoleInterval;			//打洞时间间隔

    //--------------------- H323 STACK ------------------------
	TH323CONFIG m_tH323Config;
	
	// ---------------------Mcu List ------------- [pengguofeng 6/26/2013]
	TMtadpMcuList	m_tAllSMcuList;
	//--------------------- Mtatp Buffer Send SMcuMtList--------
	u8				m_byEnableBufSendSMcuMtList;
	u32				m_dwBufSendSMcuMtListInterval;

    //IPV6接入支持
    u8              m_abyMtadpIpV6[IPV6_STR_LEN];    //本地接入板IPV6地址
    s16             m_swScopeId;                            //scopeId
    TIpMap          m_atMtMapTable[MAXNUM_CONF_MT];         //终端地址映射表
    TIpMap          m_atGkMapTable[MAXNUM_CONFIG_GK];         //GK地址映射表
    TIpMap          m_atMtadpMapTable[MAXNUM_DRI];          //接入板地址映射表
	// 此策略用于多块接入板向同一GK注册,Mtadp这张表对应的值是CriTable.[pengguofeng 6/7/2012]


    BOOL32 SetMtIpMapTable( TIpMap* ptMtMapTable );  
    BOOL32 SetGkIpMapTable(TIpMap* ptGkMapTable );  
    BOOL32 SetMtadpIpMapTable(TIpMap* ptMtadpMapTable );  
    
    //是否有与IPV4对应的IPV6地址
    BOOL32 IsIpV6Mt( const u32 dwIpV4, u32 &dwIndex);
    BOOL32 IsIpV6Gk( const u32 dwIpV4, u32 &dwIndex);
    BOOL32 IsIpV6Mtadp( const u32 dwIpV4, u32 &dwIndex);

    //是否有与IPV6对应的IPV4地址
    BOOL32 IsIpV4Mt( /*const */s8* pbyIpV4, u32 &dwIndex);
    BOOL32 IsIpV4Gk( /*const*/ s8* pbyIpV4, u32 &dwIndex);
    BOOL32 IsIpV4Mtadp( /*const*/ s8* pbyIpV4, u32 &dwIndex);

//     u8* GetMtIpV6ByIndex( const u32 dwIndex );
//     u32 GetMtIpV4ByIndex( const u32 dwIndex );
// 
//     u8* GetGkIpV6ByIndex( const u32 dwIndex );
//     u32 GetGkIpV4ByIndex( const u32 dwIndex );
// 
//     u8* GetGkIpV6ByIndex( const u32 dwIndex );
//     u32 GetGkIpV4ByIndex( const u32 dwIndex );



	s32  ProcH323CallBack( const CServMsg * pcServMsg, u16 wDstInst );

	//---------------------- RAS ------------------------------
	BOOL32 ProcEnqueueRRQ( const u8 byConfIdx ); //utility to searialize RRQ's
	BOOL32 ProcDequeueRRQ( u8 *pbyConfIdx );
    BOOL32 IsConfInQueue( const u8 byConfIdx );
    void   ClearMtAdpData();
    BOOL32 GetCallDataFromDebugFile( u16& wRasPort, u16& wQ931Port, u8& byMaxCall );// get call data from debug file for linux
	BOOL32 GetMtDSDbgInfoFromDbgFile( void );
	BOOL32 GetMtSliceInfoFromDbgFile( void );
	BOOL32 GetLicenseDataFromFile( s8* pchPath );//read license
	void   SetExpireDate( TKdvTime &tExpireDate );//set expired date
	BOOL32 IsExpiredDate(const TKdvTime &tDate);
	void   SetLicenseErrorCode(u8 byErrorCode);
	u8	   GetLicenseErrorCode(void);

	//----------

    //[4/8/2013 liaokang] MCU编码方式    
    void    SetMcuEncoding(emenCodingForm emMcuEncoding);
    emenCodingForm GetMcuEncoding();
    void    SetGkEncoding(emenCodingForm emGkEncoding);
    emenCodingForm GetGkEncoding();
	void	SetStackInitUtf8( BOOL32 bUtf8);
	u8      GetStackInitUtf8(void);

	void	AddToConnMcuList(u16 wInstId);
	void	AddToConnMtList(u16 wInstId);
	BOOL32	IsInConnMcuList(u16 wInstId);
	BOOL32  IsInConnMtList(u16 wInstId);
	void	ClearConnMcuMtList(u16 wInstId);

	BOOL32 GetTaideDsSupport( void );
	BOOL32 GetIsRespDRQFormGK( void );

	void SetMtInviteMsgInOsp( u8 byConfIdx,u8 byMtId,BOOL32 bIsMsgIsOsp = TRUE );
	BOOL32 IsMtInviteMsgInOsp( u8 byConfIdx,u8 byMtId );
	
	void SetNeglectInviteMsg( u8 byConfIdx,u8 byMtId,BOOL32 bIsNeglect = TRUE );
	BOOL32 IsNeglectInviteMsg( u8 byConfIdx,u8 byMtId );

    BOOL32 GetRestrictManu( void );
    BOOL32 AddManuToRestrictTable( u8 byManuId );
    BOOL32 IsManuInRestrictTable( u8 byManuId );
	void ReadFakedProductId( void );
	void GetPinHoleTime(void);
	LPCSTR GetFakedProductId( void );

#ifdef _IS22_
	void ReadDiscHeartBeatParam( void );
	u8 GetDiscHeartBeatNum( void );
	u16 GetDiscHeartBeatTime( void );
#endif

};

// 支持IPV6


void   MtAdpAPIEnableInLinux();

s8* PszCallCtrlTypeName( u16 wType );
s8* PszChanCtrlTypeName( u16 wType );
s8* PszConfCtrlTypeName( u16 wType );
s8* PszRasCtrlTypeName ( u16 wType );
s8* PszCascadeCtrlTypeName( u16 wType );

BOOL32 BODY_LEN_EQ( CServMsg &cServMsg, u16 wLen );
BOOL32 BODY_LEN_GE( CServMsg &cServMsg, u16 wLen );

typedef zTemplate< CMtAdpInst, MAXNUM_DRI_MT, CMtAdpData, 0 > CMtAdpApp;
extern  CMtAdpApp  g_cMtAdpApp;

#endif
