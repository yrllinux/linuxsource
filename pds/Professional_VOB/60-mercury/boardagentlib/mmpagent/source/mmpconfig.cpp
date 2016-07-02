/*****************************************************************************
   模块名      : Board Agent
   文件名      : mmpconfig.cpp
   相关文件    : 
   文件实现功能: 单板代理提供给上层应用的接口实现
   作者        : jianghy
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/09/12  1.0         jianghy       创建
   2004/11/11  3.5         李 博         新接口的修改
******************************************************************************/
#include "osp.h"
#include "mmpconfig.h"
#include "mcuconst.h"
#include "mmpagent.h"

/*
*构造函数
*/
CMmpConfig::CMmpConfig()
{
	byBrdId = 0;
	bIsRunMixer = FALSE;    //是否运行MIXER
	bIsRunTVWall = FALSE;	//是否运行TVWall
	bIsRunBas = FALSE;		//是否运行Bas
	bIsRunVMP = FALSE;		//是否运行VMP
	byMcuId = 0;
	dwMpcIpAddr = 0;
	wMpcPort = 0;

    m_wDiscHeartBeatTime = DEF_OSPDISC_HBTIME;
    m_byDiscHeartBeatNum = DEF_OSPDISC_HBNUM;  

	memset(&m_tMixerCfg, 0, sizeof(m_tMixerCfg));
	memset(&m_tTVWallCfg, 0, sizeof(m_tTVWallCfg));
	memset(&m_tBasCfg, 0, sizeof(m_tBasCfg));
	memset(&m_tVMPCfg, 0, sizeof(m_tVMPCfg));
#ifndef WIN32
#ifdef MMP
	byMAPCount=5;
	memset(m_tMAPCfg, 0, sizeof(m_tMAPCfg));
	for(u16 wLoop=0; wLoop<byMAPCount; wLoop++)
	{
		m_tMAPCfg[wLoop].byMAPId = (u8)wLoop;
		m_tMAPCfg[wLoop].dwMAPCoreSpeed = 392;
		m_tMAPCfg[wLoop].dwMAPMemSpeed = 131;
		m_tMAPCfg[wLoop].dwMAPMemSize = 67108864;
	}
#else
	byMAPCount=3;
	memset(m_tMAPCfg, 0, sizeof(m_tMAPCfg));
	for(u16 wLoop=0; wLoop<byMAPCount; wLoop++)
	{
		m_tMAPCfg[wLoop].byMAPId = (u8)wLoop;
		m_tMAPCfg[wLoop].dwMAPCoreSpeed = 392;
		m_tMAPCfg[wLoop].dwMAPMemSpeed = 131;
		m_tMAPCfg[wLoop].dwMAPMemSize = 67108864;
	}
#endif

#else
	byMAPCount=0;
	memset(m_tMAPCfg, 0, sizeof(m_tMAPCfg));
#endif
}

/*
*析构函数
*/
CMmpConfig::~CMmpConfig()
{
}


/*====================================================================
    函数名      ：GetBoardId
    功能        ：测获取单板的槽位号
    算法实现    ：
    引用全局变量：
    输入参数说明：无 
    返回值说明  ：ID值，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u8	CMmpConfig::GetBoardId()
{
	return byBrdId;
}

/*====================================================================
    函数名      ：GetConnectMcuId
    功能        ：获取要连接的MCU的ID
    算法实现    ：
    引用全局变量：
    输入参数说明：无 
    返回值说明  ：MCU的ID，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u8 CMmpConfig::GetConnectMcuId()
{
	return byMcuId;
}

/*====================================================================
    函数名      ：GetConnectMcuIpAddr
    功能        ：获取要连接的MCU的IP地址(网络序)
    算法实现    ：
    引用全局变量：
    输入参数说明：无 
    返回值说明  ：MCU的IP地址(网络序)，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u32 CMmpConfig::GetConnectMcuIpAddr()
{
	return htonl(dwMpcIpAddr);
}

/*====================================================================
    函数名      ：GetConnectMcuPort
    功能        ：获取连接的MCU的端口号
    算法实现    ：
    引用全局变量：
    输入参数说明：无 
    返回值说明  ：MCU的端口号，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u16 CMmpConfig::GetConnectMcuPort()
{
	return wMpcPort;
}


/*====================================================================
    函数名      ：IsRunMixer
    功能        ：是否运行混音器
    算法实现    ：
    引用全局变量：
    输入参数说明：无 
    返回值说明  ：运行返回TRUE，反之FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
BOOL CMmpConfig::IsRunMixer()
{
	return bIsRunMixer;
}

/*====================================================================
    函数名      ：IsRunBas
    功能        ：是否运行BAS
    算法实现    ：
    引用全局变量：
    输入参数说明：无 
    返回值说明  ：运行返回TRUE，反之FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
BOOL CMmpConfig::IsRunBas()
{
	return bIsRunBas;
}

/*====================================================================
    函数名      ：IsRunVMP
    功能        ：是否运行VMP
    算法实现    ：
    引用全局变量：
    输入参数说明：无 
    返回值说明  ：运行返回TRUE，反之FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
BOOL CMmpConfig::IsRunVMP()
{
	return bIsRunVMP;
}


/*====================================================================
    函数名      ：IsRunPrs
    功能        ：是否运行prs
    算法实现    ：
    引用全局变量：
    输入参数说明：无 
    返回值说明  ：运行返回TRUE，反之FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
BOOL CMmpConfig::IsRunPrs()
{
	return bIsRunPrs;
}


BOOL32 CMmpConfig::IsRunMpw()
{
    return bIsRunMpw;
}

/*====================================================================
    函数名      ：IsRunTVWALL
    功能        ：是否运行TVWALL
    算法实现    ：
    引用全局变量：
    输入参数说明：无 
    返回值说明  ：运行返回TRUE，反之FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
BOOL CMmpConfig::IsRunTVWall()
{
	return bIsRunTVWall;
}

/*====================================================================
    函数名      ：GetMixerMAPId
    功能        ：获取Mixer的MAP个数
    算法实现    ：
    引用全局变量：
    输入参数说明：u8* pbyIdBuf: 存放MAP编号的数组
                  u8 byBufLen: 数组长度
    返回值说明  ：MAP个数
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u8 CMmpConfig::GetMixerMAPId( u8* pbyIdBuf, u8 byBufLen )
{
	u8 byLoop = 0;

	if( !bIsRunMixer )
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
    函数名      ：GetTVWallMAPId
    功能        ：获取TVWall的MAP个数
    算法实现    ：
    引用全局变量：
    输入参数说明：u8* pbyIdBuf: 存放MAP编号的数组 
                  u8 byBufLen: 数组长度 
    返回值说明  ：MAP个数
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u8 CMmpConfig::GetTVWallMAPId( u8* pbyIdBuf, u8 byBufLen )
{
	u8 byLoop = 0;

	if( !bIsRunTVWall )
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
    函数名      ：GetBasMAPId
    功能        ：获取Bas的MAP个数
    算法实现    ：
    引用全局变量：
    输入参数说明：u8* pbyIdBuf: 存放MAP编号的数组 
                  u8 byBufLen: 数组长度 
    返回值说明  ：MAP个数
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u8 CMmpConfig::GetBasMAPId( u8* pbyIdBuf, u8 byBufLen )
{
	u8 byLoop = 0;

	if( !bIsRunBas )
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
    函数名      ：GetVMPMAPId
    功能        ：获取VMP的MAP个数
    算法实现    ：
    引用全局变量：
    输入参数说明：u8* pbyIdBuf: 存放MAP编号的数组 
                  u8 byBufLen: 数组长度
    返回值说明  ：MAP个数
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u8 CMmpConfig::GetVMPMAPId( u8* pbyIdBuf, u8 byBufLen )
{
	u8 byLoop = 0;

	if( !bIsRunVMP )
	{
		return FALSE;
    }

	for( byLoop = 0; byLoop < m_tVMPCfg.GetUsedMapNum() && byLoop < byBufLen; byLoop++ )
	{
		pbyIdBuf[byLoop] = m_tVMPCfg.GetUsedMapId(byLoop);
	}

	return byLoop;
}

u8  CMmpConfig::GetMpwMAPId( u8* pbyIdBuf, u8 byBufLen )
{
    u8 byLoop = 0;

	if( !bIsRunMpw )
	{
		return FALSE;
    }

	for( byLoop = 0; byLoop < m_tMpwCfg.GetUsedMapNum() && byLoop < byBufLen; byLoop++ )
	{
		pbyIdBuf[byLoop] = m_tMpwCfg.GetUsedMapId(byLoop);
	}

	return byLoop;
}
/*====================================================================
    函数名      ：GetMAPInfo
    功能        ：获取MAP信息
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byId: MAP号
                  u32 *pdwCoreSpeed: MAP主频
                  u32 *pdwMemSpeed: MAP的内存主频
                  u32 *pdwPort: MAP的端口号
    返回值说明  ：成功返回TRUE，反之FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
BOOL CMmpConfig::GetMAPInfo(u8 byId, u32 *pdwCoreSpeed, u32 *pdwMemSpeed,
                            u32 *pdwMemSize, u32 *pdwPort)
{
	if( pdwCoreSpeed == NULL || pdwMemSpeed == NULL || pdwMemSize == NULL || pdwPort == NULL )
	{
		return FALSE;
	}

	if( byId >= byMAPCount )
	{
		return FALSE;
	}

	*pdwCoreSpeed = m_tMAPCfg[byId].dwMAPCoreSpeed;
	*pdwMemSpeed = m_tMAPCfg[byId].dwMAPMemSpeed;
	*pdwMemSize = m_tMAPCfg[byId].dwMAPMemSize;
	*pdwPort = m_tMAPCfg[byId].byMAPPort;

	return TRUE;
}

/*====================================================================
    函数名      ：GetMixerId
    功能        ：获取Mixer的ID
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：Mixer的ID，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u8 CMmpConfig::GetMixerId()
{
	if( !bIsRunMixer )
	{
		return FALSE;
    }

	return m_tMixerCfg.GetEqpId();
}

/*====================================================================
    函数名      ：GetMixerType
    功能        ：获取Mixer的类型
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：Mixer的类型，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u8 CMmpConfig::GetMixerType()
{
	if( !bIsRunMixer )
	{
		return FALSE;
    }

	return m_tMixerCfg.GetType();
}

/*====================================================================
    函数名      ：GetMixerAlias
    功能        ：获取Mixer的假名
    算法实现    ：
    引用全局变量：
    输入参数说明：LPSTR lpstrBuf, Alias字符串指针，内存需用户申请
                  u16 wLen, Alias字符串缓冲区大小，
                  若小于实际长度，返回字符串予以截断
    返回值说明  ：成功返回TRUE，反之FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
BOOL CMmpConfig::GetMixerAlias(s8* lpstrBuf, u16 wLen)
{
	if( !bIsRunMixer )
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
    函数名      ：GetMixerIpAddr
    功能        ：获取Mixer的IP(网络序)
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：Mixer的IP(网络序)，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u32 CMmpConfig::GetMixerIpAddr()
{
	if( !bIsRunMixer )
	{
		return FALSE;
    }

	return htonl(m_tMixerCfg.GetIpAddr());
}

/*====================================================================
    函数名      ：GetMixerRecvStartPort
    功能        ：获取Mixer的起始接收端口号
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：起始接收端口号，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u16 CMmpConfig::GetMixerRecvStartPort()
{
	if( !bIsRunMixer )
	{
		return FALSE;
    }

	return m_tMixerCfg.GetEqpRecvPort();
}


/*====================================================================
    函数名      ：GetMixerMaxMixGroupNum
    功能        ：获取混音器支持最大混音组数
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：最大混音组数，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u8 CMmpConfig::GetMixerMaxMixGroupNum()
{
	if( !bIsRunMixer )
	{
		return FALSE;
    }

	return m_tMixerCfg.GetMaxMixGrpNum();
}

/*====================================================================
    函数名      ：GetMixerMaxChannelInGrp
    功能        ：获取每个混音组最大通道数
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：最大通道数，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u8 CMmpConfig::GetMixerMaxChannelInGrp()
{
	if( !bIsRunMixer )
	{
		return FALSE;
    }

	return m_tMixerCfg.GetMaxChnInGrp();
}


/*====================================================================
    函数名      ：GetTWId
    功能        ：获取TVWall的ID
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：TVWall的ID，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u8 CMmpConfig::GetTWId()
{
	if( !bIsRunTVWall )
	{
		return FALSE;
    }

	return m_tTVWallCfg.GetEqpId();
}

/*====================================================================
    函数名      ：GetTWType
    功能        ：获取TVWall的类型
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：TVWall的类型，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u8 CMmpConfig::GetTWType()
{
	if( !bIsRunTVWall )
	{
		return FALSE;
    }

	return m_tTVWallCfg.GetType();
}


/*====================================================================
    函数名      ：GetTWAlias
    功能        ：获取TVWall的假名
    算法实现    ：
    引用全局变量：
    输入参数说明：LPSTR lpstrBuf, Alias字符串指针，内存需用户申请
                  u16 wLen, Alias字符串缓冲区大小，
                  若小于实际长度，返回字符串予以截断
    返回值说明  ：成功返回TRUE，反之FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
BOOL CMmpConfig::GetTWAlias(s8* lpstrBuf, u16 wLen)
{
	if( !bIsRunTVWall )
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
    函数名      ：GetTWIpAddr
    功能        ：获取TVWall的IP(网络序)
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：TVWall的IP(网络序)，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u32 CMmpConfig::GetTWIpAddr()
{
	if( !bIsRunTVWall )
	{
		return FALSE;
    }

	return htonl(m_tTVWallCfg.GetIpAddr());
}

/*====================================================================
    函数名      ：GetTWRecvStartPort
    功能        ：获取TVWall视频起始接收端口号
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：起始接收端口号，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u16 CMmpConfig::GetTWRecvStartPort()
{
	if( !bIsRunTVWall )
	{
		return FALSE;
    }

	return m_tTVWallCfg.GetEqpRecvPort();
}

/*====================================================================
    函数名      ：GetTWDivStyle
    功能        ：获取电视墙的分割方式
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：分割方式，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u8 CMmpConfig::GetTWDivStyle()
{
	if( !bIsRunTVWall )
	{
		return FALSE;
    }

	// return m_tTVWallCfg.mcueqpTVWallEntDivStyle;
    return 1;
}

/*====================================================================
    函数名      ：GetTWDivNum
    功能        ：获取电视墙的分割画面数
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：分割画面数，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u8 CMmpConfig::GetTWDivNum()
{
	if( !bIsRunTVWall )
	{
		return FALSE;
    }

	// return m_tTVWallCfg.mcueqpTVWallEntDivNum;
    return 1;
}

/*====================================================================
    函数名      ：GetBasId
    功能        ：获取Bas的ID
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：Bas的ID，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u8 CMmpConfig::GetBasId()
{
	if( !bIsRunBas )
	{
		return FALSE;
    }

	return m_tBasCfg.GetEqpId();
}


/*====================================================================
    函数名      ：GetBasType
    功能        ：获取Bas的类型
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：Bas的类型，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u8 CMmpConfig::GetBasType()
{
	if( !bIsRunBas )
	{
		return FALSE;
    }

	return m_tBasCfg.GetType();
}

/*====================================================================
    函数名      ：GetBasAlias
    功能        ：获取Bas的假名
    算法实现    ：
    引用全局变量：
    输入参数说明：LPSTR lpstrBuf, Alias字符串指针，内存需用户申请
                  u16 wLen, Alias字符串缓冲区大小，
                  若小于实际长度，返回字符串予以截断
    返回值说明  ：成功返回TRUE，反之FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
BOOL CMmpConfig::GetBasAlias(s8* lpstrBuf, u16 wLen)
{
	if( !bIsRunBas )
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
    函数名      ：GetBasIpAddr
    功能        ：获取Bas的IP(网络序)
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：Bas的IP(网络序)，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u32 CMmpConfig::GetBasIpAddr()
{
	if( !bIsRunBas )
	{
		return FALSE;
    }

	return htonl(m_tBasCfg.GetIpAddr());
}

/*====================================================================
    函数名      ：GetBasRecvStartPort
    功能        ：获取Bas的起始接收端口号
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：起始接收端口号，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u16 CMmpConfig::GetBasRecvStartPort()
{
	if( !bIsRunBas )
	{
		return FALSE;
    }

	return m_tBasCfg.GetEqpRecvPort();
}


/*====================================================================
    函数名      ：GetBasMaxAdpChannel
    功能        ：获取Bas的每个MAP的最大试配通道数
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：最大试配通道数，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u8 CMmpConfig::GetBasMaxAdpChannel()
{
	if( !bIsRunBas )
	{
		return FALSE;
    }

	return m_tBasCfg.GetAdpChnNum();
}

/*====================================================================
    函数名      ：GetVMPId
    功能        ：获取VMP的ID
    算法实现    ：
    引用全局变量：
    输入参数说明： 
    返回值说明  ：VMP的ID，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u8 CMmpConfig::GetVMPId()
{
	if( !bIsRunVMP )
	{
		return FALSE;
    }

	return m_tVMPCfg.GetEqpId();
}

/*=============================================================================
  函 数 名： GetMpwId
  功    能： get mpw id
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： u8 
=============================================================================*/
u8 CMmpConfig::GetMpwId()
{
    if(!bIsRunMpw)
    {
        return FALSE;
    }
    return m_tMpwCfg.GetEqpId();
}

/*====================================================================
    函数名      ：GetVMPType
    功能        ：获取VMP的类型
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：VMP的类型，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u8 CMmpConfig::GetVMPType()
{
	if( !bIsRunVMP )
	{
		return FALSE;
    }

	return m_tVMPCfg.GetType();
}

/*=============================================================================
  函 数 名： GetMpwType
  功    能： get mpw type
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： u8 
=============================================================================*/
u8 CMmpConfig::GetMpwType()
{
    if( !bIsRunMpw )
	{
		return FALSE;
    }

	return m_tMpwCfg.GetType();
}

/*====================================================================
    函数名      ：GetVMPAlias
    功能        ：获取VMP的假名
    算法实现    ：
    引用全局变量：
    输入参数说明：LPSTR lpstrBuf, Alias字符串指针，内存需用户申请
                  u16 wLen, Alias字符串缓冲区大小，
                  若小于实际长度，返回字符串予以截断
    返回值说明  ：成功返回TRUE，反之FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
BOOL CMmpConfig::GetVMPAlias(s8* lpstrBuf, u16 wLen)
{
	if( !bIsRunVMP )
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

/*=============================================================================
  函 数 名： GetMpwAlias
  功    能： get mpw alias
  算法实现： 
  全局变量： 
  参    数： s8* lpstrBuf
             u16 wLen
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMmpConfig::GetMpwAlias(s8* lpstrBuf, u16 wLen)
{
    if(!bIsRunMpw || NULL == lpstrBuf)
    {
        return FALSE;
    }
    
    strncpy(lpstrBuf, m_tMpwCfg.GetAlias(), wLen);
    lpstrBuf[wLen-1] = '\0';
    return TRUE;
}

/*====================================================================
    函数名      ：GetVMPIpAddr
    功能        ：获取VMP的IP(网络序)
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：VMP的IP(网络序)，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u32 CMmpConfig::GetVMPIpAddr()
{
	if( !bIsRunVMP )
	{
		return FALSE;
    }

	return htonl(m_tVMPCfg.GetIpAddr());
}
	
/*=============================================================================
  函 数 名： GetMpwIpAddr
  功    能： get the mpw ip address
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： u32 
=============================================================================*/
u32 CMmpConfig::GetMpwIpAddr()
{
    if(!bIsRunMpw)
    {
        return FALSE;
    }
    return htonl(m_tMpwCfg.GetIpAddr());
}
/*====================================================================
    函数名      ：GetVMPRecvStartPort
    功能        ：获取VMP的起始接收端口号
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：起始接收端口号，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u16 CMmpConfig::GetVMPRecvStartPort()
{
	if( !bIsRunVMP )
	{
		return FALSE;
    }

	return m_tVMPCfg.GetEqpRecvPort();
}

/*=============================================================================
  函 数 名： GetMpwRecvStartPort
  功    能： mpw start recv port
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： u16 
=============================================================================*/
u16 CMmpConfig::GetMpwRecvStartPort()
{
    if(!bIsRunMpw)
    {
        return FALSE;
    }
    return m_tMpwCfg.GetEqpRecvPort();
}
/*====================================================================
    函数名      ：GetVMPDecodeNumber
    功能        ：获取VMP同时视频解码路数
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：解码路数，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u8  CMmpConfig::GetVMPDecodeNumber()
{
	if( !bIsRunVMP )
	{
		return FALSE;
    }

	return m_tVMPCfg.GetEncodeNum();
}

/*====================================================================
    函数名      ：GetPrsId
    功能        ：获取Prs的ID
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：Prs的ID，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u8 CMmpConfig::GetPrsId()
{
	if( !bIsRunPrs )
	{
		return FALSE;
    }

	return m_tPrsCfg.GetEqpId();
}

/*====================================================================
    函数名      ：GetPrsType
    功能        ：获取Prs的类型
    算法实现    ：
    引用全局变量：
    输入参数说明： 
    返回值说明  ：Prs的类型，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u8 CMmpConfig::GetPrsType()
{
	if( !bIsRunPrs )
	{
		return FALSE;
    }

	return m_tPrsCfg.GetType();
}

/*====================================================================
    函数名      ：GetPrsAlias
    功能        ：获取Prs的假名
    算法实现    ：
    引用全局变量：
    输入参数说明：LPSTR lpstrBuf, Alias字符串指针，内存需用户申请
                  u16 wLen, Alias字符串缓冲区大小，
                  若小于实际长度，返回字符串予以截断
    返回值说明  ：成功返回TRUE，反之FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
BOOL CMmpConfig::GetPrsAlias(s8* lpstrBuf, u16 wLen)
{
	if( !bIsRunPrs )
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
    函数名      ：GetPrsIpAddr
    功能        ：获取Prs的IP(网络序)
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：Prs的IP(网络序)，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u32 CMmpConfig::GetPrsIpAddr()
{
	if( !bIsRunPrs )
	{
		return FALSE;
    }

	return htonl(m_tPrsCfg.GetIpAddr());
}

/*====================================================================
    函数名      ：GetPrsRecvStartPort
    功能        ：获取Prs的起始接收端口号
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：起始接收端口号，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u16 CMmpConfig::GetPrsRecvStartPort()
{
	if( !bIsRunPrs )
	{
		return FALSE;
    }

	return m_tPrsCfg.GetEqpRecvPort();
}

/*====================================================================
    函数名      ：GetPrsRetransPara
    功能        ：获取Prs重传参数
    算法实现    ：
    引用全局变量：
    输入参数说明：u16* pwFistTimeSpan 第一个重传检测点
                  u16 *pwSecondTimeSpan 第二个重传检测点
                  u16* pwThirdTimeSpan 第三个重传检测点
                  u16* pwRejectTimeSpan 过期丢弃的时间跨度 
    返回值说明  ：解码路数，0表示失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
BOOL  CMmpConfig::GetPrsRetransPara(u16* pwFistTimeSpan, u16 *pwSecondTimeSpan, 
									u16* pwThirdTimeSpan, u16* pwRejectTimeSpan )
{
	if( !bIsRunPrs )
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


/*====================================================================
    函数名      ：SetMixerConfig
    功能        ：设置MIXER的配置信息
    算法实现    ：
    引用全局变量：
    输入参数说明：MIXER配置信息结构 
    返回值说明  ：成功返回TRUE，反之FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
BOOL CMmpConfig::SetMixerConfig(TEqpMixerEntry * ptMixerCfg)
{
	if( ptMixerCfg == NULL )
	{
		return FALSE;
    }
	m_tMixerCfg = *ptMixerCfg;
	bIsRunMixer = TRUE;
    byMcuId = ptMixerCfg->GetMcuId();

	return TRUE;
}

/*====================================================================
    函数名      ：GetTVWallMAPId
    功能        ：获取TVWall的MAP个数
    算法实现    ：
    引用全局变量：
    输入参数说明：TVWall配置信息结构
    返回值说明  ：MAP个数
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
BOOL CMmpConfig::SetTVWallConfig(TEqpTVWallEntry *ptTVWallCfg)
{
	if( ptTVWallCfg == NULL )
	{
		return FALSE;
    }
	m_tTVWallCfg = *ptTVWallCfg;
	bIsRunTVWall = TRUE;
    byMcuId = ptTVWallCfg->GetMcuId();

	return TRUE;
}

/*====================================================================
    函数名      ：SetBasConfig
    功能        ：设置BAS的配置信息
    算法实现    ：
    引用全局变量：
    输入参数说明：BAS配置信息结构 
    返回值说明  ：成功返回TRUE，反之FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
BOOL CMmpConfig::SetBasConfig(TEqpBasEntry *ptBasCfg)
{
	if( ptBasCfg == NULL )
	{
		return FALSE;
    }
	m_tBasCfg = *ptBasCfg;
	bIsRunBas= TRUE;
    byMcuId = ptBasCfg->GetMcuId();

	return TRUE;
}

/*====================================================================
    函数名      ：SetPrsConfig
    功能        ：设置prs的配置信息
    算法实现    ：
    引用全局变量：
    输入参数说明：prs配置信息结构
    返回值说明  ：成功返回TRUE，反之FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
BOOL CMmpConfig::SetPrsConfig(TEqpPrsEntry *ptPrsCfg)
{
	if( NULL == ptPrsCfg  )
	{
		return FALSE;
    }
	m_tPrsCfg = *ptPrsCfg;
	bIsRunPrs = TRUE;
    byMcuId = ptPrsCfg->GetMcuId();

	return TRUE;
}


/*====================================================================
    函数名      ：SetVMPConfig
    功能        ：设置VMP的配置信息
    算法实现    ：
    引用全局变量：
    输入参数说明：STRUCT_mcueqpVMPEntry_mem *ptVMPCfg  VMP配置信息结构的指针
    返回值说明  ：成功返回TRUE，反之FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
BOOL CMmpConfig::SetVMPConfig(TEqpVMPEntry *ptVMPCfg)
{
	if( ptVMPCfg == NULL )
	{
		return FALSE;
    }
	m_tVMPCfg = *ptVMPCfg;
	bIsRunVMP = TRUE;
    byMcuId = ptVMPCfg->GetMcuId();

	return TRUE;
}

/*=============================================================================
  函 数 名： SetMpwConfig
  功    能： 设置Mpw的配置信息
  算法实现： 
  全局变量： 
  参    数： TEqpMPWEntry* ptMpwCfg
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMmpConfig::SetMpwConfig(TEqpMPWEntry* ptMpwCfg)
{
    if(NULL == ptMpwCfg)
    {
        return FALSE;
    }
    m_tMpwCfg = *ptMpwCfg;
    bIsRunMpw = TRUE;
    byMcuId = ptMpwCfg->GetMcuId();

    return TRUE;
}

BOOL32 CMmpConfig::SetMpcIpAddrA(u32 dwIpAddr)
{
    dwMpcIpAddr = dwIpAddr;
    return TRUE;
}
/*=============================================================================
  函 数 名： SetMpcPortA
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u16 wPort
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMmpConfig::SetMpcPortA(u16 wPort)
{
    wMpcPort = wPort;
    return TRUE;
}

/*=============================================================================
  函 数 名： GetMpcIpAddrA
  功    能： 
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： u32 
=============================================================================*/
u32 CMmpConfig::GetMpcIpAddrA(void)
{
    return dwMpcIpAddr;
}

/*=============================================================================
  函 数 名： GetMpcPortA
  功    能： 
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： u16 
=============================================================================*/
u16 CMmpConfig::GetMpcPortA(void)
{
    return wMpcPort;
}
/*=============================================================================
  函 数 名： SetMpcIpAddrB
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u32 dwIpAddr
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMmpConfig::SetMpcIpAddrB(u32 dwIpAddr)
{
    dwMpcIpAddrB = dwIpAddr;
    return TRUE;
}
/*=============================================================================
  函 数 名： SetMpcPortB
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u16 wPort
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMmpConfig::SetMpcPortB(u16 wPort)
{
    wMpcPortB = wPort;
    return TRUE;
}

u32 CMmpConfig::GetMpcIpAddrB(void)
{
    return dwMpcIpAddrB;
}
u16 CMmpConfig::GetMpcPortB(void)
{
    return wMpcPortB;
}
/*====================================================================
    函数名      ：ReadConfig
    功能        ：读MMP的配置信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：成功返回TRUE，反之FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
BOOL CMmpConfig::ReadConfig()
{
	BOOL bResult;

	//单板配置
	bResult = ReadBoardInfo();
	if(bResult == FALSE)
	{
        printf("[ReadConfig] ReadBoardInfo failed !\n");
		return FALSE;
    }

	//MAP配置, 可以没有
	ReadMAPConfig();

	return TRUE;
}


/*====================================================================
    函数名      ：ReadBoardInfo
    功能        ：读MMP的单板配置信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：成功返回TRUE，反之FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
BOOL CMmpConfig::ReadBoardInfo()
{
    s8   achProfileName[32];
    BOOL bResult;
    s8   achDefStr[] = "Error String";
    s8   achReturn[MAX_VALUE_LEN + 1];
    s32  nValue;
    s8   achIpAddr[32];
    memset(achIpAddr, '\0', sizeof(achIpAddr));

	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFG_INI);

    bResult = GetRegKeyString( achProfileName, "BoardSystem", "MpcIpAddr", 
        achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( FALSE == bResult)
	{
		OspPrintf( TRUE, TRUE, "[Mmp] Wrong profile while reading %s!\n", "MpcIpAddr" );
        dwMpcIpAddr = 0;
	}
    else
    {
        dwMpcIpAddr = ntohl( INET_ADDR( achReturn ) );
		if( 0xffffffff == dwMpcIpAddr )
		{
			dwMpcIpAddr = 0;
		}
    }

	bResult = GetRegKeyInt( achProfileName, "BoardSystem", "MpcPort", 0, &nValue );
	if( FALSE == bResult)  
	{
		OspPrintf( TRUE, TRUE, "[Mmp] Wrong profile while reading %s!\n", "MpcPort" );
        nValue = MCU_LISTEN_PORT;
	}
	wMpcPort = ( u16 )nValue;

    memset(achReturn, '\0', sizeof(achReturn));
    bResult = GetRegKeyString(achProfileName, "BoardSystem", "MpcIpAddrB", 
                    achDefStr, achReturn, MAX_VALUE_LEN+1);
    if(FALSE == bResult)
    {
        OspPrintf(TRUE, FALSE, "[Mmp] wrong profile while reading %s\n", "MpcIpAddrB");
        dwMpcIpAddrB = 0;
    }
    else
    {
        dwMpcIpAddrB = ntohl(INET_ADDR(achReturn));
		if( 0xffffffff == dwMpcIpAddrB )
		{
			dwMpcIpAddrB = 0;
		}
    }
    
    nValue = 0;
    bResult = GetRegKeyInt( achProfileName, "BoardSystem", "MpcPortB", 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, TRUE, "[Mmp] Wrong profile while reading %s!\n", "MpcPortB" );
        nValue = MCU_LISTEN_PORT;
	}
	wMpcPortB = ( u16 )nValue;

    if(dwMpcIpAddr == dwMpcIpAddrB)
    {
        dwMpcIpAddrB = 0;
        wMpcPortB = 0;
    }

    bResult = GetRegKeyInt(achProfileName, "IsFront", "Flag", 1, &nValue);
    if(FALSE == bResult)
    {
        OspPrintf( TRUE, TRUE, "[Mmp] Wrong profile while reading %s!\n", "IsFront" );
		nValue = 1;
    }
    m_byChoice = (u8)nValue;   // TRUE: front , FALSE: back

#ifdef WIN32 
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "brdcfgdebug.ini");
	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Layer", 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, TRUE, "[Mmp] Wrong profile while reading %s!\n", "Layer" );
		return( FALSE );
	}
	u8 byBrdLayer = (u8)nValue;

	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Slot", 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, TRUE, "[Mmp] Wrong profile while reading %s!\n", "Slot" );
		return( FALSE );
	}
	byBrdId = (byBrdLayer<<4) | ((u8)nValue+1);

#else
#ifdef MMP
	TBrdPosition tBoardPosition;
	BrdQueryPosition( &tBoardPosition );
	byBrdId = (tBoardPosition.byBrdLayer<<4) | (tBoardPosition.byBrdSlot+1);
#else
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "brdcfgdebug.ini");
	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Layer", 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, TRUE, "[Mmp] Wrong profile while reading %s!\n", "Layer" );
		return( FALSE );
	}
	u8 byBrdLayer = (u8)nValue;

	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Slot", 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, TRUE, "[Mmp] Wrong profile while reading %s!\n", "Slot" );
		return( FALSE );
	}
	byBrdId = (byBrdLayer<<4) | ((u8)nValue+1);
    
#endif
#endif
    
    if(0 == dwMpcIpAddr && 0 == dwMpcIpAddrB)
    {
        OspPrintf(TRUE, FALSE, "[Mmp] The A and B Mpc's Ip are all 0.\n");
        return FALSE;
    }

    //读取osp断链检测参数
    GetRegKeyInt( achProfileName, "BoardSystem", "HeartBeatTime", DEF_OSPDISC_HBTIME, &nValue );
    if (nValue >= 3)
    {
        m_wDiscHeartBeatTime = (u16)nValue;
    }
    
    GetRegKeyInt( achProfileName, "BoardSystem", "HeartBeatNum", DEF_OSPDISC_HBNUM, &nValue );
    if (nValue >= 1)
    {
        m_byDiscHeartBeatNum = (u8)nValue;
    }    

	return( TRUE );
}

/*====================================================================
    函数名      ：GetMAPMemSpeed
    功能        ：通过MAP的CPU主频得到其内存主频
    算法实现    ：
    引用全局变量：
    输入参数说明：DWORD dwCoreSpeed:MAP的CPU主频
    返回值说明  ：MAP的内存主频，0表示没有查到
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
u32 CMmpConfig::GetMAPMemSpeed(u32 dwCoreSpeed)
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


/*====================================================================
    函数名      ：TableMemoryFree
    功能        ：释放掉动态申请（malloc）的表结构内存
    算法实现    ：
    引用全局变量：
    输入参数说明：void **ppMem, 指针数组指针
                  u32 dwEntryNum, 指针数组大小
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
void CMmpConfig::TableMemoryFree( void **ppMem, u32 dwEntryNum )
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

/*====================================================================
    函数名      ：GetTVWallMAPId
    功能        ：读MMP的MAP配置信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：成功返回TRUE，反之FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
BOOL CMmpConfig::ReadMAPConfig()
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
		return( FALSE );
	}
	if( dwMemEntryNum > 5 ) /*最多5个MAP*/
	{
		return( FALSE );
	}
	byMAPCount = (u8)dwMemEntryNum;

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
		return( FALSE );
    }
	if( dwMemEntryNum > 5 ) /*最多5个MAP*/
	{
		return( FALSE );
	}

    /* analyze entry strings */
    for( dwLoop = 0; dwLoop < dwMemEntryNum; dwLoop++ )
    {
        /* MAP处理器编号 */
        pchToken = strtok( lpszTable[dwLoop], chSeps );
        if( pchToken == NULL )
        {
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
