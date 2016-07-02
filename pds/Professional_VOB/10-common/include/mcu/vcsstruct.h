/*****************************************************************************
   模块名      : mcu
   文件名      : vcsstruct.h
   相关文件    : 
   文件实现功能: 与VCS通信所用结构体
   作者        : 付秀华
   版本        : 
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2008/11/20              付秀华      创建
******************************************************************************/
#ifndef __VCSSTRUCT_H_
#define __VCSSTRUCT_H_

#include "osp.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "kdvsys.h"
#include "mcuconst.h"
#include "kdvdef.h"
#include "vccommon.h"
#include "ummessagestruct.h"
#include "mcuerrcode.h"
#include "mcustruct.h"

#define MAX_HDTWDEVNUM      (u8)100        // 最多可配置的高清电视墙模块数
#define MAX_HDTWCHANNUM     (u8)1          // 每个高清电视墙模块支持的通道数

//VCS会议中的电视墙管理模式
#define VCS_TVWALLMANAGE_MANUAL_MODE	(u8)1//手动模式
#define VCS_TVWALLMANAGE_AUTO_MODE		(u8)2//自动模式
#define VCS_TVWALLMANAGE_REVIEW_MODE	(u8)3//预览模式

// 调度模式
#define VCS_SINGLE_MODE     (u8)1      // 单方调度模式
#define VCS_MULTW_MODE      (u8)2      // 多方电视墙调度模式
#define VCS_MULVMP_MODE     (u8)3      // 多方画面合成调度模式
#define VCS_GROUPSPEAK_MODE (u8)4      // 组呼发言人模式
#define VCS_GROUPTW_MODE    (u8)5      // 组呼电视墙模式
#define VCS_GROUPVMP_MODE   (u8)6      // 组呼画面合成模式
#define VCS_GROUPCHAIRMAN_MODE (u8)7	//组呼主席模式
#define VCS_GROUPROLLCALL_MODE (u8)8	//组呼点名模式

// 调度终端描述类型
#define VCS_DTYPE_MTID      (u8)1       // TMt
#define VCS_DTYPE_MTALIAS   (u8)2       // 别名

// 静音哑音类型
#define VCS_AUDPROC_MUTE    (u8)0      // 哑音
#define VCS_AUDPROC_SILENCE (u8)1      // 静音

// 操作对象类型
#define VCS_OPR_LOCAL       (u8)0      // 操作本地
#define VCS_OPR_REMOTE      (u8)1      // 操作远端

// VCS组条目类别
#define VCS_ENTRY_MT_MT             (u8)1
#define VCS_ENTRY_MT_MCU            (u8)6
#define VCS_ENTRY_MT_GROUP          (u8)7
#define VCS_ENTRY_MT_CALL_GROUP     (u8)8

// VCS最大临时可增加的呼叫终端数
#define VCS_MAXNEWADDMT_NUM        (u8)192
// VCS最大支持的组呼数
#define VCS_MAXGROUPCALL_NUM       (u8)64
// VCS最长组呼名长度
#define MAX_VCSGROUPNAMELEN        (u8)32
// VCS分组信息每包最大字节数25k
#define MAX_VCSPACKIFNOLEN         (u16)0x6000

// VCS调度操作类型
#define VCS_VCTYPE_DEFAULT         (u8)0
#define VCS_VCTYPE_DROPMT          (u8)1

// VCS调度强拆类型
#define VCS_FORCECALL_REQ          (u8)0
#define VCS_FORCECALL_CMD          (u8)1
#define VCS_FORCECALL_CANCELCMD	   (u8)2

// VCS主席轮询状态
#define VCS_POLL_START             (u8)1
#define VCS_POLL_PAUSE             (u8)2
#define VCS_POLL_STOP              (u8)3

#define VCS_MAXLEN_ALIAS			(u8)80

#define VCS_MAXNUM_PLAN		(u8)16

typedef TMtAlias TSMCUAlias;            // 下级MCU配置信息
typedef TMtAliasNoConstruct THDTWModuleAlias;      // 高清电视墙别名


//20091102为电视墙管理中的预案名称结构
struct TMtVcsPlanNames
{
	u8			m_byPlanNum;
	s8			m_achAlias[VCS_MAXNUM_PLAN][MAX_VCSGROUPNAMELEN];
}
PACKED
;

//20091102为电视墙管理中的预案新建的结构
//因为要在网络上传输所以新定义一个TMtVCSAlias，与TMtAlias唯一的区别就是m_achAlias字段从192字节减少到32字节
//定义终端别名结构(len:39)
struct TMtVCSPlanAlias
{
public:
    u8				m_AliasType;               //别名类型
    s8				m_achAlias[VCS_MAXLEN_ALIAS];  //别名字符串
    TTransportAddr	m_tTransportAddr;          //传输地址
	u8				m_byReserve;
public:
	TMtVCSPlanAlias( void ){ memset( this, 0, sizeof(TMtVCSPlanAlias) ); }
	BOOL operator ==( const TMtVCSPlanAlias & tObj ) const;
	BOOL IsNull( void ) const { if( m_AliasType == 0 )return TRUE; return FALSE; }
	void SetNull( void ){ memset(this, 0, sizeof(TMtVCSPlanAlias)); }
	BOOL IsAliasNull( void ) const { return 0 == strlen(m_achAlias); }
	void SetE164Alias( LPCSTR lpzAlias )
	{
		if( lpzAlias != NULL )
		{
			m_AliasType = mtAliasTypeE164;
            memset( m_achAlias, 0, sizeof(m_achAlias));
			strncpy( m_achAlias, lpzAlias, VCS_MAXLEN_ALIAS-1 );
		}
	}
	void SetH323Alias( LPCSTR lpzAlias )
	{
		if( lpzAlias != NULL )
		{
			m_AliasType = mtAliasTypeH323ID;
            memset( m_achAlias, 0, sizeof(m_achAlias));
			strncpy( m_achAlias, lpzAlias, VCS_MAXLEN_ALIAS-1 );
		}
	}
	void SetH320Alias( u8 byLayer, u8 bySlot, u8 byChannel )
	{
		m_AliasType = mtAliasTypeH320ID;
		sprintf( m_achAlias, "层%d-槽%d-通道%d%c", byLayer, bySlot, byChannel, 0 );
	}
	BOOL GetH320Alias ( u8 &byLayer, u8 &bySlot, u8 &byChannel )
	{
		BOOL bRet = FALSE;
		byLayer   = 0;
		bySlot    = 0; 
		byChannel = 0; 
		if( mtAliasTypeH320ID != m_AliasType )
		{
			return bRet;
		}

		s8 *pachLayerPrefix   = "层";
		s8 *pachSlotPrefix    = "-槽";
		s8 *pachChannelPrefix = "-通道";
		s8 *pachAlias = m_achAlias;

		if( 0 == memcmp( pachAlias, pachLayerPrefix, strlen("层") ) )
		{
			s32  nPos = 0;
			s32  nMaxCHLen = 3; //一字节的最大字串长度
			s8 achLayer[4] = {0};

			//提取层号
			pachAlias += strlen("层");
			for( nPos = 0; nPos < nMaxCHLen; nPos++ )
			{
				if( '-' == pachAlias[nPos] )
				{
					break;
				}
				achLayer[nPos] = pachAlias[nPos];
			}
			if( 0 == nPos || nPos >= nMaxCHLen )
			{
				return bRet;
			}
			achLayer[nPos+1] = '\0';
			byLayer = atoi(achLayer);

			//提取槽号
			pachAlias += nPos;
			if( 0 == memcmp( pachAlias, pachSlotPrefix, strlen("-槽") ) )
			{
				pachAlias += strlen("-槽");
				for( nPos = 0; nPos < nMaxCHLen; nPos++ )
				{
					if( '-' == pachAlias[nPos] )
					{
						break;
					}
					achLayer[nPos] = pachAlias[nPos];
				}
				if( 0 == nPos || nPos >= nMaxCHLen )
				{
					return bRet;
				}
				achLayer[nPos+1] = '\0';
				bySlot = atoi(achLayer);
			
				//提取通道号
				pachAlias += nPos;
				if( 0 == memcmp( pachAlias, pachChannelPrefix, strlen("-通道") ) )
				{
					pachAlias += strlen("-通道");
					memcpy( achLayer, pachAlias, nMaxCHLen );
					achLayer[nMaxCHLen] = '\0';
					byChannel = atoi(achLayer);
					bRet = TRUE;
				}
			}
		}
		return bRet;
	}
	void Print( void ) const
	{
		if( m_AliasType == 0 )
		{
			StaticLog("null alias" );
		}
		else if( m_AliasType == mtAliasTypeTransportAddress )
		{
			StaticLog("IP: 0x%x:%d", 
				     m_tTransportAddr.GetIpAddr(), m_tTransportAddr.GetPort() );
		}		
		else if( m_AliasType == mtAliasTypeE164 )
		{
			StaticLog("E164: %s", m_achAlias ); 
		}
		else if( m_AliasType == mtAliasTypeH323ID )
		{
			StaticLog("H323ID: %s", m_achAlias ); 
		}
		else if( m_AliasType == mtAliasTypeH320ID )
		{
			StaticLog("H320ID: %s", m_achAlias ); 
		}
        else if( m_AliasType == mtAliasTypeH320Alias )
        {
            StaticLog("H320Alias: %s", m_achAlias );
        }
		else
		{
			StaticLog("Other type alias!" ); 
		}
		StaticLog("\n" );
	}
}
/* 
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
*/
PACKED
;

inline  BOOL   TMtVCSPlanAlias::operator ==( const TMtVCSPlanAlias & tObj ) const
{
    if( tObj.m_AliasType == 0 || tObj.m_AliasType != m_AliasType ) 
	{
		return FALSE ;
	}

	if( tObj.m_AliasType == mtAliasTypeTransportAddress )
	{
		if( (tObj.m_tTransportAddr.GetIpAddr() == m_tTransportAddr.GetIpAddr()) && 
			(tObj.m_tTransportAddr.GetPort() == m_tTransportAddr.GetPort()) && 
			(tObj.m_tTransportAddr.GetIpAddr() != 0 ))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		if( (memcmp( tObj.m_achAlias, m_achAlias, sizeof( m_achAlias ) ) == 0) && 
			(m_achAlias[0] != 0) )
		{
            return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
}

// 会议高清电视墙组配置,由若干高清终端组成
struct THDTvWall
{
public:
	void SetNull()
	{
		m_byHDTWDevNum = 0;
		m_byReserve    = 0;
		memset(m_awHDTWDialBitRate, 0, sizeof(m_awHDTWDialBitRate));
		memset(m_atHDTWModuleAlias, 0, sizeof(m_atHDTWModuleAlias));
		// 成员类型为TW_MEMBERTYPE_NULL，表明指定的通道不可用
		memset(m_abyMemberType, 0, sizeof(m_abyTvWallMember));
		for (u8 byDevID = 0; byDevID < MAX_HDTWDEVNUM; byDevID++)
		{
			for (u8 byChanID = 0; byChanID < MAX_HDTWCHANNUM; byChanID++)
			{
				// 初始化将成员索引设置成为非法的索引号
				m_abyTvWallMember[byDevID][byChanID] = 0xFF;
			}
		}
	}


public:
	BOOL SetHDTWCfg(u8 byHDTWDevNum, THDTWModuleAlias* const patModuelAlias, u16* const paDialBitRate = NULL)
	{
		if (NULL == patModuelAlias)
		{
			return FALSE;
		}
		THDTWModuleAlias* ptHDTWModuelAlias = patModuelAlias;
		u16* pwHDTWDialBitRate = paDialBitRate;

		m_byHDTWDevNum = byHDTWDevNum;
		for (u8 byDevID = 0; byDevID < byHDTWDevNum; byDevID++)
		{
			m_atHDTWModuleAlias[byDevID] = *ptHDTWModuelAlias++;
			if (pwHDTWDialBitRate != NULL)
			{
				m_awHDTWDialBitRate[byDevID] = *pwHDTWDialBitRate++;
			}

		}
		return TRUE;
	}

	BOOL SetHDTwWallMember(u8 byHDTWDevID, u8 byHDTWChanID, u8 byMTIndex, u8 byMemberType)
	{
		if (byHDTWDevID >= m_byHDTWDevNum || byHDTWChanID >= MAX_HDTWCHANNUM )
		{
			return FALSE;
		}

		m_abyTvWallMember[byHDTWDevID][byHDTWChanID] = byMTIndex;
		m_abyMemberType[byHDTWDevID][byHDTWChanID] = byMemberType;
		return TRUE;
	}

	BOOL GetHDTwWallMember(u8 byHDTWDevID, u8 byHDTWChanID, u8& byMTIndex, u8& byMemberType) const
	{
		if (byHDTWDevID >= m_byHDTWDevNum || byHDTWChanID >= MAX_HDTWCHANNUM)
		{
			return FALSE;
		}

		byMTIndex = m_abyTvWallMember[byHDTWDevID][byHDTWChanID];
		byMemberType = m_abyMemberType[byHDTWDevID][byHDTWChanID];
		return TRUE;
	}
	
	// 启动电视墙自动工作模式，指定可用的通道数, 否则自动模式下通道均不可用
	BOOL StartAutoModel(u8 byUsableChanNum)
	{
		// 是否配置足够的电视墙设备支持指定的通道数
		if (byUsableChanNum > m_byHDTWDevNum * MAX_HDTWCHANNUM)
		{
			return FALSE;
		}
		u8 byCurUsableChanNum = 0;
		for (u8 byDevID = 0; byDevID < m_byHDTWDevNum; byDevID++)
		{
			for (u8 byChanID = 0; byChanID < MAX_HDTWCHANNUM; byChanID++)
			{
				if (byCurUsableChanNum >= byUsableChanNum)
				{
					return TRUE;
				}
				m_abyMemberType[byDevID][byChanID] = TW_MEMBERTYPE_VCSAUTOSPEC;
				byCurUsableChanNum++;				
			}
		}
		return TRUE;
	}

	// 是否配置了高清电视墙
	BOOL IsHDTWCfg()  { return m_byHDTWDevNum != 0; }

	u8               m_byHDTWDevNum;
	u8               m_byReserve;
	u16              m_awHDTWDialBitRate[MAX_HDTWDEVNUM];          //呼上高清电视墙设备使用的码率
	THDTWModuleAlias m_atHDTWModuleAlias[MAX_HDTWDEVNUM];
    u8   m_abyTvWallMember[MAX_HDTWDEVNUM][MAX_HDTWCHANNUM];       //电视墙成员索引（即创会时TMtAlias数组索引+1）
    u8   m_abyMemberType  [MAX_HDTWDEVNUM][MAX_HDTWCHANNUM];       //电视墙成员跟随类型
}
PACKED
;

// VCS下级MCU配置
struct TVCSSMCUCfg
{
public:
	TVCSSMCUCfg()
	{
 		SetNull();
	}
	void SetNull()
	{
		m_wSMCUNum = 0;
		memset(m_atSMCUAlias, 0, sizeof(m_atSMCUAlias));
		memset(m_awSMCUDialBitRate, 0, sizeof(m_awSMCUDialBitRate));
	}

	// 是否配置了VCS下级mcu
	BOOL IsVCSMCUCfg() { return m_wSMCUNum != 0; }

	u8         m_wSMCUNum;
	TSMCUAlias m_atSMCUAlias[MAXLIMIT_CONF_SMCU];
	u16        m_awSMCUDialBitRate[MAXLIMIT_CONF_SMCU];
}
PACKED
;

// 终端分组信息
struct TVCSGroupInfo
{
	s8  achGroupName[MAX_VCSGROUPNAMELEN];    // 该分组名称
	u16 dwEntryNum;                           // 该分组实体个数
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
struct TVCSEntryInfo
{
    TVCSEntryInfo()
    {
        memset( this, 0, sizeof(TVCSEntryInfo) );
    }

	s8 achEntryName[VCS_MAXLEN_ALIAS];   // 分组中包含的各实体名称(可为终端的164号/IP地址/H323ID或者子文件夹名称)
	u8 byEntryType;                         // 条目类型（MT/MCU/组）
    u8 byAddrType;                          // 地址类型（H323/E164/IP）
  	u16 wDialBitRate;                       //主机序
	s8 achUIName[VCS_MAXLEN_ALIAS];      // 界面显示的名称

	BOOL32 operator == ( const TVCSEntryInfo &tEntry ) const
	{
		if ( byEntryType != tEntry.byEntryType )
			return FALSE;
		if( byAddrType != tEntry.byAddrType )
			return FALSE;
		if ( strcmp( achEntryName, tEntry.achEntryName ) != 0 )
			return FALSE;
		return TRUE;
	}

	//重载<以便在map中使用  sq101210   由于除wDialBitRate4个字段可以确定唯一 所以不考虑wDialBitRate
	BOOL operator < ( const TVCSEntryInfo &tEntry ) const
	{
		if( this->byEntryType > tEntry.byEntryType  )
		{
			return FALSE;
		}
		else if(this->byEntryType == tEntry.byEntryType)
		{
			if( this-> byAddrType > tEntry.byAddrType )
			{
				return FALSE;
			}
			else  if( this-> byAddrType == tEntry.byAddrType )
			{
				//return FALSE;
				int nRes = strcmp(this->achEntryName, tEntry.achEntryName);
				if( nRes >0 )
				{
					return FALSE;
				}
				else if( ( nRes == 0 ))
				{
					nRes = strcmp(this->achUIName, tEntry.achUIName);
					if( nRes >0 )
					{
						return FALSE;
					}
					else if ( nRes == 0 )
					{
						return FALSE;
					}
					else
					{
						return TRUE;
					}
				}
				else
				{
					return TRUE;
				}	
			}
			else
			{
				return TRUE;
			}
		}
		else
		{
			return TRUE;
		}
		
		return FALSE;
	}
}
PACKED
;
//会议发言模式
#define     CONF_SPEAKMODE_NORMAL            0  //会控指定 或 一般申请（带队列，非自动出列）
#define     CONF_SPEAKMODE_ANSWERINSTANTLY   1  //抢答（带队列，自动出列）
//会议双流的发起使能状态
#define     CONF_DUALSTATUS_DISABLE          0  //会议双流关闭
#define     CONF_DUALSTATUS_ENABLE           1  //会议双流开启

class CBasicVCCStatus
{
    enum emMaskEx
    {
        emIsGroupLock = 0x01,
    };
public:
	CBasicVCCStatus()
	{
		DefaultStatus();
	}

    u8   GetCurVCMode()               { return m_byVCMode; }
	void SetCurVCMode(u8 byMode)      { m_byVCMode = byMode; }
	
	const TMt& GetCurVCMT()           { return m_tCurVCMT; }
	void  SetCurVCMT(TMt& tMt)        
	{ 
//		m_tLastVcMt = m_tCurVCMT;
		memcpy(&m_tCurVCMT, &tMt, sizeof(TMt)); 
	}

    BOOL IsLocMute()                  { return (m_byLocalMuteType & (1 << VCS_AUDPROC_MUTE)) != 0; }
	BOOL IsLocSilence()               { return (m_byLocalMuteType & (1 << VCS_AUDPROC_SILENCE)) != 0;}
	void SetLocAudProcType(u8 byOpenFlag, u8 byMuteType)
	{ 
		m_byLocalMuteType = (byOpenFlag ? (m_byLocalMuteType | (1 << byMuteType)) : (m_byLocalMuteType & (~(1 << byMuteType))));
	}

    BOOL IsRemMute()                  { return (m_byRemoteMuteType & (1 << VCS_AUDPROC_MUTE)) != 0; }
	BOOL IsRemSilence()               { return (m_byRemoteMuteType & (1 << VCS_AUDPROC_SILENCE)) != 0;}
	void SetRemAudProcType(u8 byOpenFlag, u8 byMuteType)
	{ 
		m_byRemoteMuteType = (byOpenFlag ? (m_byRemoteMuteType | (1 << byMuteType)) : (m_byRemoteMuteType & (~(1 << byMuteType))));
	}

	// 获取在线组呼列表
	const s8* GetGroupCallList()
	{
		return (s8*)m_achOnLineGroup;
	}
	
	// 增减在线组呼列表
    BOOL32 OprGroupCallList(s8* pachGroupName, u8 byNameLen, u8 byAdd, u16 wErrCode)
	{
		wErrCode = 0;

		if (NULL == pachGroupName)
		{
			StaticLog("[OprGroupCallList] NULL == pachGroupName\n");
			return FALSE;
		}
		
		u8 byExistPos = VCS_MAXGROUPCALL_NUM;
		u8 byEmptPos  = VCS_MAXGROUPCALL_NUM;
		for (u8 byIdx = 0; byIdx < VCS_MAXGROUPCALL_NUM; byIdx++)
		{
			if (!memcmp(pachGroupName, m_achOnLineGroup[byIdx], min(byNameLen, MAX_VCSGROUPNAMELEN)) &&
				min(byNameLen, MAX_VCSGROUPNAMELEN) >= strlen(m_achOnLineGroup[byIdx]))
			{
				byExistPos = byIdx;
				break;
			}
			if (!memcmp(m_achOnLineGroup[byIdx], "", sizeof("")) && VCS_MAXGROUPCALL_NUM == byExistPos)
			{
				byEmptPos = byIdx;
			}
		}

		if (byAdd)
		{ 
			if (byExistPos != VCS_MAXGROUPCALL_NUM)
			{
				StaticLog("[OprGroupCallList] %s has on line\n", m_achOnLineGroup[byExistPos]);
				return TRUE;
			}

			if (VCS_MAXGROUPCALL_NUM == byEmptPos)
			{
				wErrCode = ERR_MCU_VCS_OVERGROUPNUM;
				return FALSE;
			}
			else
			{
				memcpy(m_achOnLineGroup[byEmptPos], pachGroupName, min(byNameLen, MAX_VCSGROUPNAMELEN));
				m_achOnLineGroup[byEmptPos][MAX_VCSGROUPNAMELEN - 1] = 0;
			}
		}
		else
		{
			if (byExistPos != VCS_MAXGROUPCALL_NUM)
			{
				memset(m_achOnLineGroup[byExistPos], 0, sizeof(m_achOnLineGroup[byExistPos]));
			}

		}

		return TRUE;
	}

	// 是否超过可支持的临时增加数
	BOOL32 IsAddEnable()
	{
		for (u8 byIdx = 0; byIdx < VCS_MAXNEWADDMT_NUM; byIdx++)
		{
			if (m_atNewAddMt[byIdx].IsNull())
			{
				return TRUE;
			}
		}
		
		return FALSE;
	}
	
	// 获取临时呼叫终端列表
	const TMt* GetNewMtList()
	{
		return m_atNewAddMt;
	}

	// 增减临时呼叫的终端列表
	BOOL32 OprNewMt(const TMt& tMt, u8 byAdd)
	{
		u8 byExistPos = VCS_MAXNEWADDMT_NUM;
		u8 byEmptyPos = VCS_MAXNEWADDMT_NUM;
		for (u8 byIdx = 0; byIdx < VCS_MAXNEWADDMT_NUM; byIdx++)
		{
			if (m_atNewAddMt[byIdx] == tMt)
			{
				byExistPos = byIdx;
				break;
			}
			if (m_atNewAddMt[byIdx].IsNull() && VCS_MAXNEWADDMT_NUM == byEmptyPos)
			{
				byEmptyPos = byIdx;
			}
		}
		if (byAdd)
		{
			if (byExistPos != VCS_MAXNEWADDMT_NUM)
			{
				return TRUE;
			}
			if (VCS_MAXNEWADDMT_NUM == byEmptyPos)
			{
				return FALSE;
			}
			else
			{
				m_atNewAddMt[byEmptyPos] = tMt;
				return TRUE;
			}
		}
		else
		{
			if (byExistPos != VCS_MAXNEWADDMT_NUM)
			{
				m_atNewAddMt[byExistPos].SetNull();
				return TRUE;
			}

		}
		return FALSE;

	}

	// 获取当前的主席轮询终端
	const TMt& GetCurChairPollMt()
	{
		return m_tMtPolling;
	}
	// 设置当前的主席轮询终端
	void SetCurChairPollMt(const TMt& tMt)
	{
		m_tMtPolling = tMt;
	}
	// 获取主席轮询状态
	u8 GetChairPollState()
	{
		return m_byPollState;
	}
	void SetChairPollState(u8 byPollState)
	{
		m_byPollState = byPollState;
	}
	// 获取主席轮询间隔
	u8 GetPollIntval()
	{
		return m_byPollIntval;
	}
	void SetPollIntval(u8 byIntval)
	{
		m_byPollIntval = byIntval;
	}

	// 调度模式切换时，除调度模式静哑音状态外其它状态变量需要恢复为原始值
	void  RestoreStatus()
	{
		m_tCurVCMT.SetNull();
//		m_tLastVcMt.SetNull();
// 		m_byLocalMuteType  = 0;
// 		m_byRemoteMuteType = 0; 
		memset(m_achOnLineGroup, 0, sizeof(m_achOnLineGroup));
		m_tMtPolling.SetNull();
		m_byPollIntval = 0;
		m_byPollState  = VCS_POLL_STOP;
		m_byTvWallManageMode = VCS_TVWALLMANAGE_MANUAL_MODE;
        m_bySpeakMode = CONF_SPEAKMODE_NORMAL;
        m_byDualEnable = CONF_DUALSTATUS_DISABLE;
//        m_byDualing = 0;
        SetGroupModeLock( TRUE );
	}

	// 组呼中模式切换,不更改轮询状态及组呼组状态
	void RestoreGroupStatus()
	{
		m_tCurVCMT.SetNull();
        //[9/13/2011 zhushengze]恢复双流使能状态
        SetConfDualEnable(CONF_DUALSTATUS_DISABLE);
//		m_tLastVcMt.SetNull();
// 		m_byLocalMuteType  = 0;
// 		m_byRemoteMuteType = 0; 
	}

	// 默认状态
	void DefaultStatus()
	{
		// 默认为单方调度模式
		m_byVCMode = VCS_SINGLE_MODE;
		m_byTvWallManageMode = VCS_TVWALLMANAGE_MANUAL_MODE;
		m_byIsMtInTvWallCanMixing = 0;	
		m_byLocalMuteType  = 0;
		m_byRemoteMuteType = 0; 
        m_bySpeakMode = CONF_SPEAKMODE_NORMAL;
        m_byDualEnable = CONF_DUALSTATUS_DISABLE;
//        m_byDualing = 0;
		memset(m_atNewAddMt, 0, sizeof(m_atNewAddMt));
		RestoreStatus();
	}

	//zjj20091102
	u8 GetTVWallManageMode(){ return m_byTvWallManageMode; }
	void SetTVWallManageMode( u8 byMode )
	{
		if( byMode >= VCS_TVWALLMANAGE_MANUAL_MODE &&
			byMode <= VCS_TVWALLMANAGE_REVIEW_MODE )
		{
			m_byTvWallManageMode = byMode;
		}
	}

	BOOL32 GetMtInTvWallCanMixing(){ return (1 == m_byIsMtInTvWallCanMixing) ;}
	void SetMtInTvWallCanMixing( BOOL32 bCanMixing )
	{
		if( bCanMixing )
		{
			m_byIsMtInTvWallCanMixing = 1;
		}
		else
		{
			m_byIsMtInTvWallCanMixing = 0;
		}
	}

    u8   GetConfSpeakMode()   {   return m_bySpeakMode;   }
    void SetConfSpeakMode(u8 byMode) {    m_bySpeakMode = byMode; }
    BOOL32 IsConfDualEnable()   {   return m_byDualEnable == 1 ? TRUE : FALSE;   }
    void   SetConfDualEnable(BOOL32 bDualing) {   m_byDualEnable = bDualing ? 1 : 0 ; }

//    BOOL32 IsConfDualing()   {   return m_byDualing == 1 ? TRUE : FALSE;   }
//    void   SetConfDualing(BOOL32 bDualing) {   m_byDualing = bDualing ? 1 : 0 ; }

    BOOL32 IsGroupModeLock() const{ return ((m_byMask & emIsGroupLock ) != 0); }
    void   SetGroupModeLock( BOOL32 byIsLock )
    { 
        if( byIsLock )
        {
            m_byMask |= emIsGroupLock;
        }
        else
        {
            m_byMask &= (~emIsGroupLock);
        }		
	}

	void ClearAllNewMt( void )
	{
		memset(m_atNewAddMt, 0, sizeof(m_atNewAddMt));
	}
//	const TMt& GetLstVCMT()           { return m_tLastVcMt; }

	void Print()
	{
 		StaticLog("m_byVCMode:%d\n", m_byVCMode);
 		StaticLog("m_byLocalMuteType:%d\n", m_byLocalMuteType);
 		StaticLog("m_byRemoteMuteType:%d\n", m_byRemoteMuteType);
 		StaticLog("m_tCurVCMT:mcuid－%d mtid－%d\n", m_tCurVCMT.GetMcuId(), m_tCurVCMT.GetMtId() );
 		StaticLog("m_tMtPolling:mcuid-%d mtid－%d\n", m_tMtPolling.GetMcuId(),m_tMtPolling.GetMtId() );
 		StaticLog("m_byPollIntval:%d\n", m_byPollIntval);
 		StaticLog("m_byPollState:%d\n", m_byPollState);
 		StaticLog("m_byTvWallManageMode:%d\n", m_byTvWallManageMode);
 		StaticLog("m_byIsMtInTvWallCanMixing:%d\n", m_byIsMtInTvWallCanMixing);
        StaticLog("m_bySpeakMode:%d\n", m_bySpeakMode);
        StaticLog("m_byDualEnable:%d\n", m_byDualEnable);
//        StaticLog("m_byDualing:%d\n", m_byDualing);
        StaticLog("m_byMask:%d\n", m_byMask);
		StaticLog( "m_atNewAddMt:\n");        

		for (u8 byIdx = 0, byPrintIdx = 0; byIdx < VCS_MAXNEWADDMT_NUM; byIdx++)
		{
			if (!m_atNewAddMt[byIdx].IsNull())
			{
				StaticLog("%d)mtid(%d)\n", byPrintIdx, m_atNewAddMt[byIdx].GetMtId());
				byPrintIdx++;
			}
		}
//		StaticLog( "m_tLastVcMt:mcuid－%d mtid－%d\n", m_tLastVcMt.GetMcuId(), m_tLastVcMt.GetMtId() );
	}

protected:
	u8  m_byVCMode;         // VCS会议当前所处的调度模式(VCS_SINGLE_MODE,VCS_MULTW_MODE,VCS_MULVMP_MODE) 
	TMt m_tCurVCMT;         // VCS当前正在调度的终端
	u8  m_byLocalMuteType;  // 本地静哑音类型
	u8  m_byRemoteMuteType; // 远端静哑音类型

	TMt m_atNewAddMt[VCS_MAXNEWADDMT_NUM]; // VCS临时呼叫终端列表
	s8  m_achOnLineGroup[VCS_MAXGROUPCALL_NUM][MAX_VCSGROUPNAMELEN]; // VCS当前在线组呼名

	TMt m_tMtPolling;    // 当前正在进行主席轮询的终端
	u8  m_byPollIntval;  // 轮询间隔(s)
	u8  m_byPollState;   // 轮询状态

	//zjj20091102 电视墙管理模式
	u8	m_byTvWallManageMode;

	//zjj20091102 电视墙预案中新增一键混音
	//在电视墙中的终端是否可以进混音0-不可以 1-可以
	u8	m_byIsMtInTvWallCanMixing;

	//发言模式：抢答/正常申请(CONF_SPEAKMODE_NORMAL/CONF_SPEAKMODE_ANSWERINSTANTLY)
    u8  m_bySpeakMode;


    u8  m_byMask;	//二值量填充变量,第一位：当前锁定状态；第二位：级联前锁定状态

    //双流是否允许发送：vcs控制
    u8  m_byDualEnable;    
}
PACKED
;


// 单HDU通道相关信息
struct THduModChnlInfo
{
public:
	THduModChnlInfo()
	{
		SetNull();
	}

	void SetNull()
	{
		m_byHduId = 0;
		m_byHduChnlIdx = MAXNUM_HDU_CHANNEL;
		m_byHduChnlMember = MAXNUM_CONF_MT + 1;
		m_byMemberType = TW_MEMBERTYPE_NULL;
	}

	void SetHduId(u8 byHduId)  { m_byHduId = byHduId; }
	u8   GetHduID()    const        { return m_byHduId; }

	void SetHduChnlIdx(u8 byHduChnlIdx)  { m_byHduChnlIdx = byHduChnlIdx; }
	u8   GetHduChnlIdx()    const             { return m_byHduChnlIdx; }

	void SetHduChnlMember(u8 byHduChnlMember)  { m_byHduChnlMember = byHduChnlMember; }
	u8   GetHduChnlMember()      const         { return m_byHduChnlMember; }

	void SetMemberType(u8 byMemberType)  { m_byMemberType = byMemberType; }
	u8   GetMemberType()   const             { return m_byMemberType; }

	// [5/25/2011 liuxu] 添加合法性接口
	BOOL32 IsValid( ) const
	{
		// Hdu的Id判断
		if(HDUID_MIN > m_byHduId || HDUID_MAX < m_byHduId)
		{
			return FALSE;
		}
		
		// 通道编号最大是1
		if ( m_byHduChnlIdx >= MAXNUM_HDU_CHANNEL )
		{
			return FALSE;
		}
		
		return TRUE;
	}

private:
	u8    m_byHduId;             //通道所在设备ID
	u8    m_byHduChnlIdx;        //对应设备的通道号
    u8    m_byHduChnlMember;     //该通道成员索引
    u8    m_byMemberType;        //该通道成员跟随类型,TW_MEMBERTYPE_MCSSPEC,TW_MEMBERTYPE_SPEAKER...
}
PACKED
;

struct TMtListInfo
{
public:
	TMtListInfo()
	{
		memset(this, 0, sizeof(TMtListInfo));
	}

	BOOL SetMtList(u8 byMtNum, const TMtAlias* patMtAlias, const u16* pawMtDialRate)
	{
		if (byMtNum > MAXNUM_CONF_MT ||
			!patMtAlias || !pawMtDialRate)
		{
			StaticLog("[SetMtList]bad input param(byMtNum:%d, patMtAlias:%d, pawMtDialRate:%d)\n",
				      byMtNum, patMtAlias, pawMtDialRate);
			return FALSE;
		}

		m_byMtNum = byMtNum;
		memcpy(m_tMtAliasArray, patMtAlias, byMtNum * sizeof(TMtAlias));
		memcpy(m_awMtDialRate, pawMtDialRate, byMtNum * sizeof(u16));
		return TRUE;
	}

	u8 GetMtNum()
	{
		return m_byMtNum;
	}

	TMtAlias& GetMtAliasByIdx(u8 byIdx)
	{
		return m_tMtAliasArray[byIdx];
	}

	u16 GetMtDialRateByIdx(u8 byIdx)
	{
		return m_awMtDialRate[byIdx];
	}
private:
	TMtAlias   m_tMtAliasArray[MAXNUM_CONF_MT]; // VCS原始的组呼终端列表信息
	u16        m_awMtDialRate[MAXNUM_CONF_MT];    // VCS原始的组呼终端呼叫码率
	u8         m_byMtNum;
	u8         m_byReserve;
}
PACKED
;

// [5/25/2011 liuxu] 判断电视墙成员类型是否是Vcs会议中的类型
inline BOOL IsVcsTvwMemberType( const u8 byType )
{
	if ( byType == TW_MEMBERTYPE_VCSAUTOSPEC 
		|| byType == TW_MEMBERTYPE_VCSSPEC
		|| byType == TW_MEMBERTYPE_TWPOLL)
	{
		return TRUE;
	}

	return FALSE;
}


#endif  /* __VCSSTRUCT_H_ */


