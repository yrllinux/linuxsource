/*****************************************************************************
   模块名      : Board Agent
   文件名      : mediabrdconfig.h
   相关文件    : 
   文件实现功能: 相关配置函数定义
   作者        : jianghy
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/08/25  1.0         jianghy       创建
   2004/12/05  3.5         李 博        新接口的修改
******************************************************************************/
#ifndef MEDIABRDCONFIG_H
#define MEDIABRDCONFIG_H

#define MEMSIZE_1K			(u32)(1024)
#define MEMSIZE_BYTE_1K		(u32)(1*MEMSIZE_1K)
#define MEMSIZE_BYTE_1M		(u32)(MEMSIZE_BYTE_1K*MEMSIZE_1K)

#include "osp.h"
#include "mcuconst.h"
#include "boardconfigbasic.h"
#include "mcuagtstruct.h"

typedef struct
{
	u8		byMAPId;		/*MAP处理器编号*/
	u32		dwMAPCoreSpeed;	/*MAP处理器主频*/
	u32		dwMAPMemSpeed;	/*MAP处理器内存主频*/
	u32		dwMAPMemSize;	/*MAP处理器内存容量*/
	u8		byMAPPort;		/*MAP处理器视频端口*/
}TMAPInfo;

class CMediaBrdConfig:public CBBoardConfig
{
public:
	/*构造函数*/
	CMediaBrdConfig()
	{
		m_bIsRunMixer = FALSE;		//是否运行MIXER
		m_bIsRunTVWall = FALSE;		//是否运行TVWall
		m_bIsRunBas = FALSE;		//是否运行Bas
		m_bIsRunVMP = FALSE;		//是否运行VMP
		m_bIsRunPrs = FALSE;		//是否运行PRS
		m_bIsRunMpw = FALSE;		//是否运行Mpw

		memset(&m_tMixerCfg, 0, sizeof(m_tMixerCfg));
		memset(&m_tTVWallCfg, 0, sizeof(m_tTVWallCfg));
		memset(&m_tBasCfg, 0, sizeof(m_tBasCfg));
		memset(&m_tVMPCfg, 0, sizeof(m_tVMPCfg));
		memset(&m_tPrsCfg, 0, sizeof(m_tPrsCfg));
		memset(&m_tMpwCfg, 0, sizeof(m_tMpwCfg));

	#ifdef IMT
		//要根据本板类型确定是IMT还是APU
		TBrdPosition tBoardPosition;
		BrdQueryPosition( &tBoardPosition );   //此处不用转为TBrdPos，并没有存储转换，jlb_081120

		if( tBoardPosition.byBrdID == BRD_TYPE_IMT/*DSL8000_BRD_IMT*/ )
		{
			m_byMAPCount=3;
			memset(m_tMAPCfg, 0, sizeof(m_tMAPCfg));
			for(u16 wLoop=0; wLoop<m_byMAPCount; wLoop++)
			{
				m_tMAPCfg[wLoop].byMAPId = (u8)wLoop;
				m_tMAPCfg[wLoop].dwMAPCoreSpeed = 392;
				m_tMAPCfg[wLoop].dwMAPMemSpeed = 131;
				m_tMAPCfg[wLoop].dwMAPMemSize = 64*MEMSIZE_BYTE_1M;
			}
		}
		else
		{
			m_byMAPCount=1;
			memset(m_tMAPCfg, 0, sizeof(m_tMAPCfg));
			for(u16 wLoop=0; wLoop<m_byMAPCount; wLoop++)
			{
				m_tMAPCfg[wLoop].byMAPId = (u8)wLoop;
				m_tMAPCfg[wLoop].dwMAPCoreSpeed = 392;
				m_tMAPCfg[wLoop].dwMAPMemSpeed = 131;
				m_tMAPCfg[wLoop].dwMAPMemSize = 64*MEMSIZE_BYTE_1M;
			}
		}
	#endif

	#ifdef MMP
		m_byMAPCount=5;
		memset(m_tMAPCfg, 0, sizeof(m_tMAPCfg));
		for(u16 wLoop=0; wLoop<m_byMAPCount; wLoop++)
		{
			m_tMAPCfg[wLoop].byMAPId = (u8)wLoop;
			m_tMAPCfg[wLoop].dwMAPCoreSpeed = 392;
			m_tMAPCfg[wLoop].dwMAPMemSpeed = 131;
			m_tMAPCfg[wLoop].dwMAPMemSize = 64*MEMSIZE_BYTE_1M;
		}
	#endif
	}

	
	//析构函数
	
	virtual CMediaBrdConfig::~CMediaBrdConfig()
	{
	}

	BOOL32 ReadConfig()
	{
		BOOL32 bResult;

		//单板配置
		bResult = ReadConnectMcuInfo();
		if(bResult == FALSE)
		{
			printf("[ReadConfig] ReadBoardInfo failed !\n");
			return FALSE;
		}

		//MAP配置, 可以没有
		ReadMAPConfig();

		return TRUE;
	}

public:
	u8 GetMcuId()
	{
		return m_byMcuId;
	}
	void SetMcuId(u8 byMcuId)
	{
		m_byMcuId = byMcuId;
	}

	TEapuCfg GetEapuCfg() 
	{  
		return m_tEapuCfg;
	}
	/*====================================================================
	功能：是否运行混音器
	参数：无
	返回值：运行返回TRUE，反之FALSE
	====================================================================*/
	BOOL IsRunMixer()
	{
		return m_bIsRunMixer;
	}

	/*====================================================================
	功能：是否运行BAS
	参数：无
	返回值：运行返回TRUE，反之FALSE
	====================================================================*/
	BOOL32 IsRunBas()
	{
		return m_bIsRunBas;
	}

	/*====================================================================
	功能：是否运行VMP
	参数：无
	返回值：运行返回TRUE，反之FALSE
	====================================================================*/
	BOOL32 IsRunVMP()
	{
		return m_bIsRunVMP;
	}

	/*====================================================================
	功能：是否运行Prs
	参数：无
	返回值：运行返回TRUE，反之FALSE
	====================================================================*/
	BOOL32 IsRunPrs()
	{
		return m_bIsRunPrs;
	}

	/*====================================================================
	功能：是否运行TVWALL
	参数：无
	返回值：运行返回TRUE，反之FALSE
	====================================================================*/
	BOOL32 IsRunTVWall()
	{
		return m_bIsRunTVWall;
	}

	/*====================================================================
	功能：获取Mixer的MAP个数
	参数：u8* pbyIdBuf: 存放MAP编号的数组 
	      u8 byBufLen: 数组长度
	返回值：MAP个数
	====================================================================*/
	u8 GetMixerMAPId( u8* pbyIdBuf, u8 byBufLen )
	{
		u8 byLoop = 0;

		if( !m_bIsRunMixer )
		{
			return FALSE;
		}

		for( byLoop = 0; byLoop < m_tMixerCfg.GetUsedMapNum() && byLoop < byBufLen; byLoop++ )
		{
			pbyIdBuf[byLoop] = m_tMixerCfg.GetUsedMapId(byLoop);
		}

		return byLoop;
	}

	/*====================================================================
	功能：获取TVWall的MAP个数
	参数：u8* pbyIdBuf: 存放MAP编号的数组 
	      u8 byBufLen: 数组长度
	返回值：MAP个数
	====================================================================*/
	u8 GetTVWallMAPId( u8* pbyIdBuf, u8 byBufLen )
	{
		u8 byLoop = 0;

		if( !m_bIsRunTVWall )
		{
			return FALSE;
		}

		for( byLoop = 0; byLoop < m_tTVWallCfg.GetUsedMapNum() && byLoop < byBufLen; byLoop++ )
		{
			pbyIdBuf[byLoop] = m_tTVWallCfg.GetUsedMapId(byLoop);
		}

		return byLoop;
	}

	/*====================================================================
	功能：获取Bas的MAP个数
	参数：u8* pbyIdBuf: 存放MAP编号的数组 
	      u8 byBufLen: 数组长度
	返回值：MAP个数
	====================================================================*/
	u8 GetBasMAPId( u8* pbyIdBuf, u8 byBufLen )
	{
		u8 byLoop = 0;

		if( !m_bIsRunBas )
		{
			return FALSE;
		}

		for( byLoop = 0; byLoop < m_tBasCfg.GetUsedMapNum() && byLoop < byBufLen; byLoop++ )
		{
			pbyIdBuf[byLoop] = m_tBasCfg.GetUsedMapId(byLoop);
		}

		return byLoop;
	}

	/*====================================================================
	功能：获取VMP的MAP个数
	参数：u8* pbyIdBuf: 存放MAP编号的数组 
	      u8 byBufLen: 数组长度
	返回值：MAP个数
	====================================================================*/
	u8 GetVMPMAPId( u8* pbyIdBuf, u8 byBufLen )
	{
		u8 byLoop = 0;

		if( !m_bIsRunVMP )
		{
			return FALSE;
		}

		for( byLoop = 0; byLoop < m_tVMPCfg.GetUsedMapNum() && byLoop < byBufLen; byLoop++ )
		{
			pbyIdBuf[byLoop] = m_tVMPCfg.GetUsedMapId(byLoop);
		}

		return byLoop;
	}

	/*====================================================================
	功能：获取MAP信息
	参数：u8 byId: MAP号
	      u32 *pdwCoreSpeed: MAP主频
		  u32 *pdwMemSpeed: MAP的内存主频
		  u32 *pdwPort: MAP的端口号
	返回值：成功返回TRUE，反之FALSE
	====================================================================*/
	BOOL32 GetMAPInfo(u8 byId, u32 *pdwCoreSpeed, u32 *pdwMemSpeed, u32 *pdwMemSize, u32 *pdwPort)
	{
		if( pdwCoreSpeed == NULL || pdwMemSpeed == NULL || pdwMemSize == NULL || pdwPort == NULL )
		{
			return FALSE;
		}

		if( byId >= m_byMAPCount )
		{
			return FALSE;
		}

		*pdwCoreSpeed = m_tMAPCfg[byId].dwMAPCoreSpeed;
		*pdwMemSpeed = m_tMAPCfg[byId].dwMAPMemSpeed;
		*pdwMemSize = m_tMAPCfg[byId].dwMAPMemSize;
		*pdwPort = m_tMAPCfg[byId].byMAPPort;

		return TRUE;
	}


	/*Mixer的配置信息接口*/
	/*====================================================================
	功能：获取Mixer的TAudioMixerCfg
	参数：TAudioMixerCfg &tAudioMixerCfg: 存放Mixer的TAudioMixerCfg
	返回值：BOOL32
	====================================================================*/
	BOOL32 GetMixerCfg( TAudioMixerCfg &tAudioMixerCfg )
	{
		if( !m_bIsRunMixer )
		{
			return FALSE;
		}

		u8 buf[MAXNUM_MAP];
		s32 nNum = GetMixerMAPId(buf, MAXNUM_MAP);
        tAudioMixerCfg.wMAPCount = (u16)nNum;
        for(s32 nLp=0;nLp<nNum; nLp++)
        {
            tAudioMixerCfg.m_atMap[nLp].byMapId = buf[nLp];
            GetMAPInfo(buf[nLp],
                                     (u32*)&tAudioMixerCfg.m_atMap[nLp].dwCoreSpd,
                                     (u32*)&tAudioMixerCfg.m_atMap[nLp].dwMemSpd,
                                     (u32*)&tAudioMixerCfg.m_atMap[nLp].dwMemSize,
                                     (u32*)&tAudioMixerCfg.m_atMap[nLp].dwPort);
        }
        tAudioMixerCfg.dwConnectIP = GetMpcIpA();
        tAudioMixerCfg.wConnectPort = GetMpcPortA();
        tAudioMixerCfg.dwConnectIpB = GetMpcIpB();
        tAudioMixerCfg.wConnectPortB = GetMpcPortB();
        
        tAudioMixerCfg.byEqpType = GetMixerType();
        tAudioMixerCfg.byEqpId = GetMixerId();
        tAudioMixerCfg.dwLocalIP = GetMixerIpAddr();
        tAudioMixerCfg.wRcvStartPort = GetMixerRecvStartPort();
        tAudioMixerCfg.wMcuId = GetMcuId();
        tAudioMixerCfg.byMaxMixGroupCount = (u8)nNum;
        tAudioMixerCfg.byMaxChannelInGroup = GetMixerMaxChannelInGrp();
        GetMixerAlias(tAudioMixerCfg.achAlias,MAXLEN_ALIAS );
        tAudioMixerCfg.achAlias[MAXLEN_ALIAS] = '\0';
		return TRUE;
	}

	/*====================================================================
	功能：获取Mixer的ID
	参数：无
	返回值：Mixer的ID，0表示失败
	====================================================================*/
	u8 GetMixerId()
	{
		if( !m_bIsRunMixer )
		{
			return FALSE;
		}

		return m_tMixerCfg.GetEqpId();
	}

	/*====================================================================
	功能：获取Mixer的类型
	参数：无
	返回值：Mixer的类型，0表示失败
	====================================================================*/
	u8 GetMixerType()
	{
		if( !m_bIsRunMixer )
		{
			return FALSE;
		}

		return m_tMixerCfg.GetType();
	}

	/*====================================================================
	功能：获取Mixer的假名
	参数：s8* lpstrBuf, Alias字符串指针，内存需用户申请
          u16  wLen, Alias字符串缓冲区大小，若小于实际长度，返回字符串予以截断
	返回值：成功返回TRUE，反之FALSE
	====================================================================*/
	BOOL32 GetMixerAlias(s8* lpstrBuf, u16  wLen)
	{
		if( !m_bIsRunMixer )
		{
			return FALSE;
		}

		if( lpstrBuf == NULL )
		{
			return FALSE;
		}

		strncpy( lpstrBuf, m_tMixerCfg.GetAlias(), wLen );
		lpstrBuf[wLen-1] = '\0';
		return TRUE;
	}
	
	/*====================================================================
	功能：获取Mixer的IP(网络序)
	参数：无
	返回值：Mixer的IP(网络序)，0表示失败
	====================================================================*/
	u32 GetMixerIpAddr()
	{
		if( !m_bIsRunMixer )
		{
			return FALSE;
		}

		return htonl(m_tMixerCfg.GetIpAddr());
	}
	
	/*====================================================================
	功能：获取Mixer的起始接收端口号
	参数：无
	返回值：起始接收端口号，0表示失败
	====================================================================*/
	u16  GetMixerRecvStartPort()
	{
		if( !m_bIsRunMixer )
		{
			return FALSE;
		}

		return m_tMixerCfg.GetEqpRecvPort();
	}

	/*====================================================================
	功能：获取混音器每个MAP的支持最大混音组数
	参数：无
	返回值：最大混音组数，0表示失败
	====================================================================*/
	u8 GetMixerMaxMixGroupNum()
	{
		if( !m_bIsRunMixer )
		{
			return FALSE;
		}

		return m_tMixerCfg.GetMaxMixGrpNum();
	}
	
	/*====================================================================
	功能：获取每个混音组最大通道数
	参数：无
	返回值：最大通道数，0表示失败
	====================================================================*/
	u8 GetMixerMaxChannelInGrp()
	{
		if( !m_bIsRunMixer )
		{
			return FALSE;
		}

		return m_tMixerCfg.GetMaxChnInGrp();
	}


	/*TVWALL的配置信息接口*/
	/*====================================================================
	功能：获取TVWall的TEqpCfg
	参数：TEqpCfg &tTvWallCfg: 存放TVWall的TEqpCfg
	返回值：BOOL32
	====================================================================*/
	BOOL32 GetTVWallCfg( TEqpCfg &tTvWallCfg )
	{
		if ( !m_bIsRunTVWall )
		{
			return FALSE;
		}

		u8 buf[MAXNUM_MAP];
		s32 nNum = GetTVWallMAPId(buf, MAXNUM_MAP);
        for(s32 nLp=0;nLp<nNum;nLp++)
        {
            tTvWallCfg.m_atMap[nLp].byMapId = buf[nLp];
            GetMAPInfo(buf[nLp],
                       (u32*)&tTvWallCfg.m_atMap[nLp].dwCoreSpd,
                       (u32*)&tTvWallCfg.m_atMap[nLp].dwMemSpd,
                       (u32*)&tTvWallCfg.m_atMap[nLp].dwMemSize,
                       (u32*)&tTvWallCfg.m_atMap[nLp].dwPort);
        }
        tTvWallCfg.wMAPCount     = (u8)nNum;
        tTvWallCfg.dwConnectIP   = GetMpcIpA();
        tTvWallCfg.wConnectPort  = GetMpcPortA();
        tTvWallCfg.dwConnectIpB   = GetMpcIpB();
        tTvWallCfg.wConnectPortB  = GetMpcPortB();

        tTvWallCfg.byEqpType     = GetTWType();
        tTvWallCfg.byEqpId       = GetTWId();
        tTvWallCfg.dwLocalIP     = GetTWIpAddr();
        tTvWallCfg.wRcvStartPort = GetTWRecvStartPort();
        tTvWallCfg.wMcuId        = GetMcuId();
        GetTWAlias(tTvWallCfg.achAlias,MAXLEN_ALIAS );
        tTvWallCfg.achAlias[MAXLEN_ALIAS] = '\0';
		return TRUE;
	}

	/*====================================================================
	功能：获取TVWall的ID
	参数：无
	返回值：TVWall的ID，0表示失败
	====================================================================*/
	u8 GetTWId()
	{
		if( !m_bIsRunTVWall )
		{
			return FALSE;
		}

		return m_tTVWallCfg.GetEqpId();
	}

	/*====================================================================
	功能：获取TVWall的类型
	参数：无
	返回值：TVWall的类型，0表示失败
	====================================================================*/
	u8 GetTWType()
	{
		if( !m_bIsRunTVWall )
		{
			return FALSE;
		}

		return m_tTVWallCfg.GetType();
	}

	/*====================================================================
	功能：获取TVWall的假名
	参数：s8* lpstrBuf, Alias字符串指针，内存需用户申请
          u16  wLen, Alias字符串缓冲区大小，若小于实际长度，返回字符串予以截断
	返回值：成功返回TRUE，反之FALSE
	====================================================================*/
	BOOL32 GetTWAlias(s8* lpstrBuf, u16  wLen)
	{
		if( !m_bIsRunTVWall )
		{
			return FALSE;
		}

		if( lpstrBuf == NULL )
		{
			return FALSE;
		}

		strncpy( lpstrBuf, m_tTVWallCfg.GetAlias(), wLen );
		lpstrBuf[wLen-1] = '\0';
		return TRUE;
	}

	/*====================================================================
	功能：获取TVWall的IP(网络序)
	参数：无
	返回值：TVWall的IP(网络序)，0表示失败
	====================================================================*/
	u32 GetTWIpAddr()
	{
		if( !m_bIsRunTVWall )
		{
			return FALSE;
		}

		return htonl(m_tTVWallCfg.GetIpAddr());
	}

	/*====================================================================
	功能：获取TVWall视频起始接收端口号
	参数：无
	返回值：起始接收端口号，0表示失败
	====================================================================*/
	u16  GetTWRecvStartPort()
	{
		if( !m_bIsRunTVWall )
		{
			return FALSE;
		}

		return m_tTVWallCfg.GetEqpRecvPort();
	}

	/*====================================================================
	功能：获取电视墙的分割方式
	参数：无
	返回值：分割方式，0表示失败
	====================================================================*/
	u8 GetTWDivStyle()
	{
		if( !m_bIsRunTVWall )
		{
			return FALSE;
		}

		// return m_tTVWallCfg.mcueqpTVWallEntDivStyle;
		return 1;
	}

	/*====================================================================
	功能：获取电视墙的分割画面数
	参数：无
	返回值：分割画面数，0表示失败
	====================================================================*/
	u8 GetTWDivNum()
	{
		if( !m_bIsRunTVWall )
		{
			return FALSE;
		}

		// return m_tTVWallCfg.mcueqpTVWallEntDivNum;
		return 1;
	}


	/*BAS的配置信息接口*/
	/*====================================================================
	功能：获取Bas的TEqpCfg
	参数：TEqpCfg &tBasEqpCfg: 存放Bas的TEqpCfg
	返回值：BOOL32
	====================================================================*/
	BOOL32 GetBasCfg( TEqpCfg &tBasEqpCfg )
	{
		if ( !m_bIsRunBas )
		{
			return FALSE;
		}

		u8 buf[MAXNUM_MAP];
		s32 nNum = GetBasMAPId(buf,MAXNUM_MAP);
        for(s32 nLp=0; nLp<MAXNUM_MAP; nLp++)
        {
            tBasEqpCfg.m_atMap[nLp].byMapId = buf[nLp];
            GetMAPInfo(buf[nLp],
                      (u32*)&tBasEqpCfg.m_atMap[nLp].dwCoreSpd,
                      (u32*)&tBasEqpCfg.m_atMap[nLp].dwMemSpd,
                      (u32*)&tBasEqpCfg.m_atMap[nLp].dwMemSize,
                      (u32*)&tBasEqpCfg.m_atMap[nLp].dwPort);
        }
        tBasEqpCfg.dwConnectIP = GetMpcIpA();
        tBasEqpCfg.wConnectPort = GetMpcPortA();
        tBasEqpCfg.dwConnectIpB = GetMpcIpB();
        tBasEqpCfg.wConnectPortB = GetMpcPortB();

        tBasEqpCfg.byEqpType = GetBasType();
        tBasEqpCfg.byEqpId = GetBasId();
        tBasEqpCfg.dwLocalIP = GetBasIpAddr();
        tBasEqpCfg.wRcvStartPort = GetBasRecvStartPort();
        tBasEqpCfg.wMcuId = GetMcuId();
        tBasEqpCfg.wMAPCount = (u8)nNum;
        GetBasAlias(tBasEqpCfg.achAlias,MAXLEN_ALIAS );
        tBasEqpCfg.achAlias[MAXLEN_ALIAS] = '\0';
		return TRUE;
	}

	/*====================================================================
	功能：获取Bas的ID
	参数：无
	返回值：Bas的ID，0表示失败
	====================================================================*/
	u8 GetBasId()
	{
		if( !m_bIsRunBas )
		{
			return FALSE;
		}

		return m_tBasCfg.GetEqpId();
	}

	/*====================================================================
	功能：获取Bas的类型
	参数：无
	返回值：Bas的类型，0表示失败
	====================================================================*/
	u8 GetBasType()
	{
		if( !m_bIsRunBas )
		{
			return FALSE;
		}

		return m_tBasCfg.GetType();
	}

	/*====================================================================
	功能：获取Bas的假名
	参数：s8* lpstrBuf, Alias字符串指针，内存需用户申请
          u16  wLen, Alias字符串缓冲区大小，若小于实际长度，返回字符串予以截断
	返回值：成功返回TRUE，反之FALSE
	====================================================================*/
	BOOL32 GetBasAlias(s8* lpstrBuf, u16  wLen)
	{
		if( !m_bIsRunBas )
		{
			return FALSE;
		}

		if( lpstrBuf == NULL )
		{
			return FALSE;
		}

		strncpy( lpstrBuf, m_tBasCfg.GetAlias(), wLen );
		lpstrBuf[wLen-1] = '\0';
		return TRUE;
	}
	
	/*====================================================================
	功能：获取Bas的IP(网络序)
	参数：无
	返回值：Bas的IP(网络序)，0表示失败
	====================================================================*/
	u32 GetBasIpAddr()
	{
		if( !m_bIsRunBas )
		{
			return FALSE;
		}

		return htonl(m_tBasCfg.GetIpAddr());
	}
	
	/*====================================================================
	功能：获取Bas的起始接收端口号
	参数：无
	返回值：起始接收端口号，0表示失败
	====================================================================*/
	u16  GetBasRecvStartPort()
	{
		if( !m_bIsRunBas )
		{
			return FALSE;
		}

		return m_tBasCfg.GetEqpRecvPort();
	}

	/*====================================================================
	功能：获取Bas的每个MAP的最大试配通道数
	参数：无
	返回值：最大试配通道数，0表示失败
	====================================================================*/
	u8 GetBasMaxAdpChannel()
	{
		if( !m_bIsRunBas )
		{
			return FALSE;
		}

		return m_tBasCfg.GetAdpChnNum();
	}
	
	/*VMP的配置信息接口*/
	/*====================================================================
	功能：获取Vmp的TVmpCfg
	参数：TVmpCfg &tVmpCfg: 存放Vmp的TVmpCfg
	返回值：BOOL32
	====================================================================*/
	BOOL32 GetVmpCfg( TVmpCfg &tVmpCfg )
	{
		if ( !m_bIsRunVMP )
		{
			return FALSE;
		}

		u8 buf[MAXNUM_MAP];
		s32 nNum = GetVMPMAPId(buf, MAXNUM_MAP);
        GetMAPInfo(buf[0],
                   (u32*)&tVmpCfg.m_atMap[0].dwCoreSpd,
                   (u32*)&tVmpCfg.m_atMap[0].dwMemSpd,
                   (u32*)&tVmpCfg.m_atMap[0].dwMemSize,
                   (u32*)&tVmpCfg.m_atMap[0].dwPort);

        tVmpCfg.wMAPCount     = nNum;
        tVmpCfg.dwConnectIP   = GetMpcIpA();
        tVmpCfg.wConnectPort  = GetMpcPortA();
        tVmpCfg.dwConnectIpB  = GetMpcIpB();
        tVmpCfg.wConnectPortB = GetMpcPortB();
        
        tVmpCfg.byEqpType     = GetVMPType();
        tVmpCfg.byEqpId       = GetVMPId();
        tVmpCfg.dwLocalIP     = GetVMPIpAddr();
        tVmpCfg.wRcvStartPort = GetVMPRecvStartPort();
        tVmpCfg.wMcuId        = GetMcuId();
        tVmpCfg.byDbVid       = GetVMPDecodeNumber();
        GetVMPAlias(tVmpCfg.achAlias, MAXLEN_ALIAS);
        tVmpCfg.achAlias[MAXLEN_ALIAS] = '\0';
		return TRUE;
	}
	
	/*====================================================================
	功能：获取VMP的ID
	参数：无
	返回值：VMP的ID，0表示失败
	====================================================================*/
	u8 GetVMPId()
	{
		if( !m_bIsRunVMP )
		{
			return FALSE;
		}

		return m_tVMPCfg.GetEqpId();
	}

	/*====================================================================
	功能：获取VMP的类型
	参数：无
	返回值：VMP的类型，0表示失败
	====================================================================*/
	u8 GetVMPType()
	{
		if( !m_bIsRunVMP )
		{
			return FALSE;
		}

		return m_tVMPCfg.GetType();
	}

	/*====================================================================
	功能：获取VMP的假名
	参数：s8* lpstrBuf, Alias字符串指针，内存需用户申请
          u16  wLen, Alias字符串缓冲区大小，若小于实际长度，返回字符串予以截断
	返回值：成功返回TRUE，反之FALSE
	====================================================================*/
	BOOL32 GetVMPAlias(s8* lpstrBuf, u16  wLen)
	{
		if( !m_bIsRunVMP )
		{
			return FALSE;
		}

		if( lpstrBuf == NULL )
		{
			return FALSE;
		}

		strncpy( lpstrBuf, m_tVMPCfg.GetAlias(), wLen );
		lpstrBuf[wLen-1] = '\0';
		return TRUE;
	}
	
	/*====================================================================
	功能：获取VMP的IP(网络序)
	参数：无
	返回值：VMP的IP(网络序)，0表示失败
	====================================================================*/
	u32 GetVMPIpAddr()
	{
		if( !m_bIsRunVMP )
		{
			return FALSE;
		}

		return htonl(m_tVMPCfg.GetIpAddr());
	}
	
	/*====================================================================
	功能：获取VMP的起始接收端口号
	参数：无
	返回值：起始接收端口号，0表示失败
	====================================================================*/
	u16  GetVMPRecvStartPort()
	{
		if( !m_bIsRunVMP )
		{
			return FALSE;
		}

		return m_tVMPCfg.GetEqpRecvPort();
	}

	/*====================================================================
	功能：获取VMP同时视频解码路数
	参数：无
	返回值：解码路数，0表示失败
	====================================================================*/
	u8  GetVMPDecodeNumber()
	{
		if( !m_bIsRunVMP )
		{
			return FALSE;
		}

		return m_tVMPCfg.GetEncodeNum();
	}


	/*MPW的配置信息接口*/
	/*====================================================================
	功能：获取Mpw的TVmpCfg
	参数：TVmpCfg &tMpwCfg: 存放Vmp的TVmpCfg
	返回值：BOOL32
	====================================================================*/
	BOOL32 GetMpwCfg( TVmpCfg &tMpwCfg )
	{
		if ( !m_bIsRunMpw )
		{
			return FALSE;
		}

		u8 buf[MAXNUM_MAP];
		s32 nNum = GetMpwMAPId(buf, MAXNUM_MAP);
        GetMAPInfo(buf[0],
                   (u32*)&tMpwCfg.m_atMap[0].dwCoreSpd,
                   (u32*)&tMpwCfg.m_atMap[0].dwMemSpd,
                   (u32*)&tMpwCfg.m_atMap[0].dwMemSize,
                   (u32*)&tMpwCfg.m_atMap[0].dwPort);

        tMpwCfg.wMAPCount     = nNum;
        tMpwCfg.dwConnectIP   = GetMpcIpA();
        tMpwCfg.wConnectPort  = GetMpcPortA();
        tMpwCfg.dwConnectIpB  = GetMpcIpB();
        tMpwCfg.wConnectPortB = GetMpcPortB();
        
        tMpwCfg.byEqpType     = GetMpwType();
        tMpwCfg.byEqpId       = GetMpwId();
        tMpwCfg.dwLocalIP     = GetMpwIpAddr();
        tMpwCfg.wRcvStartPort = GetMpwRecvStartPort();
        tMpwCfg.wMcuId        = GetMcuId();
        GetMpwAlias(tMpwCfg.achAlias, MAXLEN_ALIAS);
        tMpwCfg.achAlias[MAXLEN_ALIAS] = '\0';
		return TRUE;
	}
	
    u8  GetMpwId()
	{
		if(!m_bIsRunMpw)
		{
			return FALSE;
		}
		return m_tMpwCfg.GetEqpId();
	}

    u8 GetMpwType()
	{
		if( !m_bIsRunMpw )
		{
			return FALSE;
		}

		return m_tMpwCfg.GetType();
	}

    BOOL32 GetMpwAlias(s8* lpstrBuf, u16  wLen)
	{
		if(!m_bIsRunMpw || NULL == lpstrBuf)
		{
			return FALSE;
		}
    
		strncpy(lpstrBuf, m_tMpwCfg.GetAlias(), wLen);
		lpstrBuf[wLen-1] = '\0';
		return TRUE;
	}

    u32 GetMpwIpAddr()
	{
		if(!m_bIsRunMpw)
		{
			return FALSE;
		}
		return htonl(m_tMpwCfg.GetIpAddr());
	}

    u16  GetMpwRecvStartPort()
	{
		if(!m_bIsRunMpw)
		{
			return FALSE;
		}
		return m_tMpwCfg.GetEqpRecvPort();
	}

    BOOL32 IsRunMpw()
	{
		return m_bIsRunMpw;
	}

    u8  GetMpwMAPId( u8* pbyIdBuf, u8 byBufLen )
	{
		u8 byLoop = 0;

		if( !m_bIsRunMpw )
		{
			return FALSE;
		}

		for( byLoop = 0; byLoop < m_tMpwCfg.GetUsedMapNum() && byLoop < byBufLen; byLoop++ )
		{
			pbyIdBuf[byLoop] = m_tMpwCfg.GetUsedMapId(byLoop);
		}

		return byLoop;
	}

    //设置Mpw的配置信息
    BOOL32 SetMpwConfig(TEqpMPWEntry* ptMpwCfg)
	{
		if(NULL == ptMpwCfg)
		{
			return FALSE;
		}

		m_tMpwCfg = *ptMpwCfg;
		m_bIsRunMpw = TRUE;
		m_byMcuId = ptMpwCfg->GetMcuId();

		return TRUE;
	}
	/*Prs的配置信息接口*/
	/*====================================================================
	功能：获取Prs的TPrsCfg
	参数：TPrsCfg &tPrsCfg: 存放Prs的TPrsCfg
	返回值：BOOL32
	====================================================================*/
	BOOL32 GetPrsCfg( TPrsCfg &tPrsCfg )
	{
		if ( !m_bIsRunPrs )
		{
			return FALSE;
		}

		tPrsCfg.wMcuId        = GetMcuId();
        tPrsCfg.byEqpId       = GetPrsId();
        tPrsCfg.byEqpType     = GetPrsType();
        tPrsCfg.dwLocalIP     = GetPrsIpAddr();
        tPrsCfg.wRcvStartPort = GetPrsRecvStartPort();
        tPrsCfg.dwConnectIP   = GetMpcIpA();
        tPrsCfg.wConnectPort  = GetMpcPortA();
        tPrsCfg.dwConnectIpB  = GetMpcIpB();
        tPrsCfg.wConnectPortB = GetMpcPortB();
        
        GetPrsAlias(tPrsCfg.achAlias, MAXLEN_ALIAS);
        tPrsCfg.achAlias[MAXLEN_ALIAS] = '\0';
        GetPrsRetransPara(&tPrsCfg.m_wFirstTimeSpan,
                          &tPrsCfg.m_wSecondTimeSpan,
                          &tPrsCfg.m_wThirdTimeSpan,
                          &tPrsCfg.m_wRejectTimeSpan);
		return TRUE;
	}

	/*====================================================================
	功能：获取Prs的ID
	参数：无
	返回值：Prs的ID，0表示失败
	====================================================================*/
	u8 GetPrsId()
	{
		if( !m_bIsRunPrs )
		{
			return FALSE;
		}

		return m_tPrsCfg.GetEqpId();
	}

	/*====================================================================
	功能：获取Prs的类型
	参数：无
	返回值：Prs的类型，0表示失败
	====================================================================*/
	u8 GetPrsType()
	{
		if( !m_bIsRunPrs )
		{
			return FALSE;
		}

		return m_tPrsCfg.GetType();
	}

	/*====================================================================
	功能：获取Prs的假名
	参数：s8* lpstrBuf, Alias字符串指针，内存需用户申请
          u16  wLen, Alias字符串缓冲区大小，若小于实际长度，返回字符串予以截断
	返回值：成功返回TRUE，反之FALSE
	====================================================================*/
	BOOL32 GetPrsAlias(s8* lpstrBuf, u16  wLen)
	{
		if( !m_bIsRunPrs )
		{
			return FALSE;
		}

		if( lpstrBuf == NULL )
		{
			return FALSE;
		}

		strncpy( lpstrBuf, m_tPrsCfg.GetAlias(), wLen );
		lpstrBuf[wLen-1] = '\0';
		return TRUE;
	}
	
	/*====================================================================
	功能：获取Prs的IP(网络序)
	参数：无
	返回值：Prs的IP(网络序)，0表示失败
	====================================================================*/
	u32 GetPrsIpAddr()
	{
		if( !m_bIsRunPrs )
		{
			return FALSE;
		}

		return htonl(m_tPrsCfg.GetIpAddr());
	}
	
	/*====================================================================
	功能：获取Prs的起始接收端口号
	参数：无
	返回值：起始接收端口号，0表示失败
	====================================================================*/
	u16  GetPrsRecvStartPort()
	{
		if( !m_bIsRunPrs )
		{
			return FALSE;
		}

		return m_tPrsCfg.GetEqpRecvPort();
	}

	/*====================================================================
	功能：获取Prs重传参数
	参数：u16* pwFistTimeSpan 第一个重传检测点
	      u16 *pwSecondTimeSpan 第二个重传检测点
		  u16* pwThirdTimeSpan 第三个重传检测点
		  u16* pwRejectTimeSpan 过期丢弃的时间跨度
	返回值：解码路数，0表示失败
	====================================================================*/
	BOOL32  GetPrsRetransPara(u16* pwFistTimeSpan, u16 *pwSecondTimeSpan, 
		u16* pwThirdTimeSpan, u16* pwRejectTimeSpan )
	{
		if( !m_bIsRunPrs )
		{
			return FALSE;
		}

		if( pwFistTimeSpan == NULL || pwSecondTimeSpan == NULL 
			|| pwThirdTimeSpan == NULL || pwRejectTimeSpan == NULL )
		{
			return FALSE;
		}

		*pwFistTimeSpan = m_tPrsCfg.GetFirstTimeSpan();
		*pwSecondTimeSpan = m_tPrsCfg.GetSecondTimeSpan();
		*pwThirdTimeSpan = m_tPrsCfg.GetThirdTimeSpan();
		*pwRejectTimeSpan = m_tPrsCfg.GetRejectTimeSpan();

		return TRUE;
	}
public:
    u8   m_byTvWallModel; // 电视墙模式，0：无，1：音频，2：视频，3：音视频

protected:
	BOOL32 ReadMAPConfig()
	//从配置文件读MAP的配置信息
	{
		char    achProfileName[32];
		s8*   *lpszTable;
		char    chSeps[] = " \t";       /* space or tab as seperators */
		char    *pchToken;
		u32   dwLoop;
		BOOL    bResult = TRUE;
		u32   dwMemEntryNum;
 
		sprintf( achProfileName, "%s/%s", DIR_CONFIG, "mapcfg.ini");
   
		/* get the number of entry */
		bResult = GetRegKeyInt( achProfileName, "MAPTable", STR_ENTRY_NUM, 
					0, (s32*)&dwMemEntryNum );
		if( bResult == FALSE ) 
		{
			printf("[ReadMAPConfig] GetRegKeyInt Ent Num failed !\n");
			return( FALSE );
		}
		if( dwMemEntryNum > 3 ) /*最多3个MAP*/
		{
			printf("[ReadMAPConfig] Ent Num larger than 3, failed !\n");
			return( FALSE );
		}
		m_byMAPCount = (u8)dwMemEntryNum;

		/*alloc memory*/
		lpszTable = (char**)malloc( dwMemEntryNum * sizeof( s8* ) );
		for( dwLoop = 0; dwLoop < dwMemEntryNum; dwLoop++ )
		{
			lpszTable[dwLoop] = (char*)malloc( MAX_VALUE_LEN + 1 );
		}

		/*get the map table*/
		bResult = GetRegKeyStringTable( achProfileName, "MAPTable",
					   "fail", lpszTable, &dwMemEntryNum, MAX_VALUE_LEN + 1 );
		if( bResult == FALSE )
		{
			printf("[ReadMAPConfig] GetRegKeyStringTable failed !\n");
			return( FALSE );
		}
		if( dwMemEntryNum > 5 ) /*最多5个MAP*/
		{
			printf("[ReadMAPConfig] Map Num larger than 5, failed !\n");
			return( FALSE );
		}

		/* analyze entry strings */
		for( dwLoop = 0; dwLoop < dwMemEntryNum; dwLoop++ )
		{
			/* MAP处理器编号 */
			pchToken = strtok( lpszTable[dwLoop], chSeps );
			if( pchToken == NULL )
			{
				printf("[ReadMAPConfig] 1.pchToken == NULL \n");
				bResult = FALSE;
			}
			else
			{
				m_tMAPCfg[dwLoop].byMAPId = atoi( pchToken );
			}

			/* MAP处理器主频 */
			pchToken = strtok( NULL, chSeps );
			if( pchToken == NULL )
			{
				printf("[ReadMAPConfig] 2.pchToken == NULL \n");
				bResult = FALSE;
			}
			else
			{
				m_tMAPCfg[dwLoop].dwMAPCoreSpeed = atoi( pchToken );
			}

			m_tMAPCfg[dwLoop].dwMAPMemSpeed = GetMAPMemSpeed( m_tMAPCfg[dwLoop].dwMAPCoreSpeed );


			/* MAP处理器内存容量 */
			pchToken = strtok( NULL, chSeps );
			if( pchToken == NULL )
			{
				printf("[ReadMAPConfig] 3.pchToken == NULL \n");
				bResult = FALSE;
			}
			else
			{
				m_tMAPCfg[dwLoop].dwMAPMemSize = atoi( pchToken );
			}
			/* MAP处理器视频端口 */
			pchToken = strtok( NULL, chSeps );
			if( pchToken == NULL )
			{
				printf("[ReadMAPConfig] 4.pchToken == NULL \n");
				bResult = FALSE;
			}
			else
			{
				m_tMAPCfg[dwLoop].byMAPPort = atoi( pchToken );
			}
		}
		/* free memory */
		TableMemoryFree( ( void ** )lpszTable, dwMemEntryNum );
    
		return( bResult );
	}

	u32 GetMAPMemSpeed(u32 dwCoreSpeed)
	{
		u16    wLoop=0;
		static u32 adwCoreSpdToMemSpdTable[][2] = {
			{68,68},  {74,74},  {81,81},  {88,88},  {95,95},  {101,101},{108,108}, {115,115},{122,122},
			{128,128},{135,68}, {149,74}, {162,81}, {176,88}, {189,95}, {203,101}, {216,108},{230,115},
			{243,122},{257,128},{270,135},{284,142},{297,99}, {311,104},{324,108}, {338,113},{351,117},
			{365,122},{378,126},{392,131},{405,135},{419,140},{432,144},{446,149}, {459,153},{473,158},
			{486,162},{500,167},{0,0}
		};

		while( adwCoreSpdToMemSpdTable[wLoop][0] != 0)
		{
			if( adwCoreSpdToMemSpdTable[wLoop][0] == dwCoreSpeed )
			{
				return adwCoreSpdToMemSpdTable[wLoop][1];
			}
			wLoop++;
		}
		return 0;
	}

	void TableMemoryFree( void **ppMem, u32 dwEntryNum )
	{
		u32    dwLoop;

		if( ppMem == NULL )
		{
			return;
		}

		for( dwLoop = 0; dwLoop < dwEntryNum; dwLoop++ )
		{
			if( ppMem[dwLoop] != NULL )
			{
				free( ppMem[dwLoop] );
			}
		}
   
		free( ppMem );
	}

protected:
	//设置MIXER的配置信息
	BOOL32 SetMixerConfig(TEqpMixerEntry *ptMixerCfg)
	{
		if( ptMixerCfg == NULL )
		{
			return FALSE;
		}
		m_tMixerCfg = *ptMixerCfg;
		m_bIsRunMixer = TRUE;
		m_byMcuId = ptMixerCfg->GetMcuId();

		return TRUE;
	}

	//设置TVWALL的配置信息
	BOOL32 SetTVWallConfig(TEqpTVWallEntry *ptTVWallCfg)
	{
		if( ptTVWallCfg == NULL )
		{
			return FALSE;
		}
		m_tTVWallCfg = *ptTVWallCfg;
		m_bIsRunTVWall = TRUE;
		m_byMcuId = ptTVWallCfg->GetMcuId();

		return TRUE;
	}

	//设置BAS的配置信息
	BOOL32 SetBasConfig(TEqpBasEntry *ptBasCfg)
	{
		if( ptBasCfg == NULL )
		{
			return FALSE;
		}
		m_tBasCfg = *ptBasCfg;
		m_bIsRunBas= TRUE;
		m_byMcuId = ptBasCfg->GetMcuId();

		return TRUE;
	}

	//设置VMP的配置信息
	BOOL32 SetVMPConfig(TEqpVMPEntry *ptVMPCfg)
	{
		if( ptVMPCfg == NULL )
		{
			return FALSE;
		}
		m_tVMPCfg = *ptVMPCfg;
		m_bIsRunVMP = TRUE;
		m_byMcuId = ptVMPCfg->GetMcuId();

		return TRUE;
	}

	//设置Prs的配置信息
	BOOL32 SetPrsConfig(TEqpPrsEntry *ptPrsCfg)
	{
		if( ptPrsCfg == NULL )
		{
			return FALSE;
		}
		m_tPrsCfg = *ptPrsCfg;
		m_bIsRunPrs= TRUE;
		m_byMcuId = ptPrsCfg->GetMcuId();

		return TRUE;
	}
	//设置EMIXER的配置信息
	/*
	BOOL32 SetEqpEMixerEntry(u8 byMixIdx,TEqpMixerEntry tEqpMixerEntry)
	{
		m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].byEqpId       = tEqpMixerEntry.GetEqpId();
		m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].byEqpType     = EQP_TYPE_EMIXER;
		m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].wMcuId        = LOCAL_MCUID;
		m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].wRcvStartPort = tEqpMixerEntry.GetEqpRecvPort();
		strcpy(m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].achAlias, tEqpMixerEntry.GetAlias());
		m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].achAlias[MAXLEN_EQP_ALIAS-1] = '\0';
		m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].m_byMixerMemberNum = tEqpMixerEntry.GetMaxChnInGrp();
		m_tEapuCfg.m_tEapuMixerCfg[byMixIdx].m_bIsMultiFormat = FALSE;
		m_bIsRunMixer = TRUE;
		m_byMcuId = tEqpMixerEntry.GetMcuId();

		return TRUE;
	}
	void SetEMixerNum(u8 byMixNum)
	{
		m_tEapuCfg.m_byMixerNum = min(MAXNUM_EAPU_MIXER,byMixNum);
	}*/
protected:
	BOOL32	m_bIsRunMixer;    //是否运行MIXER
	BOOL32	m_bIsRunTVWall;	//是否运行TVWall
	BOOL32	m_bIsRunBas;		//是否运行Bas
	BOOL32	m_bIsRunVMP;		//是否运行VMP
	BOOL32   m_bIsRunPrs;      //是否运行PRS
	BOOL32  m_bIsRunMpw;      //是否运行MPw

	u8		m_byMcuId;		//MCU ID

	TEqpMixerEntry	m_tMixerCfg;	//MIXER的配置信息
	TEqpTVWallEntry	m_tTVWallCfg;	//TVWALL的配置信息
	TEqpBasEntry	m_tBasCfg;		//BAS的配置信息
	TEqpVMPEntry	m_tVMPCfg;		//VMP的配置信息
	TEqpPrsEntry	m_tPrsCfg;		//Prs的配置信息
	TEqpMPWEntry    m_tMpwCfg;
	u8				m_byMAPCount;
	TMAPInfo		m_tMAPCfg[5];	//MAP的配置信息
private:
    TEapuCfg        m_tEapuCfg;     //EAPU类型混音器配置
};

#endif    /* MEDIABRDCONFIG_H */
