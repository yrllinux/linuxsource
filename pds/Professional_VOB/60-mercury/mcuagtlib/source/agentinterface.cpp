/*****************************************************************************
   模块名      : Mcu Agent
   文件名      : AgentInterface.cpp
   相关文件    : AgentInterface.h
   文件实现功能: Agent 提供给上层调用得接口类
   作者        : liuhuiyun
   版本        : V4.0  Copyright( C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2005/08/17  1.0         liuhuiyun       创建
******************************************************************************/

#include "agentinterface.h"
#include "configureagent.h"


CAgentInterface::CAgentInterface()
{
	//初始化外设MCU侧起始端口 [12/13/2011 chendaiwei]
	m_wMixerMcuRecvStartPort = MIXER_MCU_STARTPORT;
	m_wRecMcuRecvStartPort = REC_MCU_STARTPORT;
	m_wBasMcuRecvStartPort = BAS_MCU_STARTPORT;
	m_wVmpMcuRecvStartPort = VMP_MCU_STARTPORT;
	
	m_byIsNeedRebootAllMPUBas = 0;
}

CAgentInterface::~CAgentInterface()
{
}

/*=============================================================================
  函 数 名： ReadTrapTable
  功    能： 读Trap表
  算法实现： 
  全局变量： 
  参    数： u8* pbyTrapNum
             TTrapInfo** pptTrapTable
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::ReadTrapTable(u8* pbyTrapNum, TTrapInfo* ptTrapTable )
{
     return g_cCfgParse.ReadTrapTable(pbyTrapNum, ptTrapTable);
}

/*=============================================================================
  函 数 名： WriteTrapTable
  功    能： 写Trap表
  算法实现： 
  全局变量： 
  参    数： u8 byTrapNum
             TTrapInfo** pptTrapTable
  返 回 值： u16 
=============================================================================*/
u16 CAgentInterface::WriteTrapTable(u8 byTrapNum, TTrapInfo* ptTrapTable)
{
    return g_cCfgParse.WriteTrapTable(byTrapNum, ptTrapTable);
}

/*=============================================================================
  函 数 名： ReadBrdTable
  功    能： 读单板表
  算法实现： 
  全局变量： 
  参    数： u8* pbyBrdNum
             TBoardInfo** pptBoardTable
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::ReadBrdTable(u8* pbyBrdNum, TBoardInfo* ptBoardTable)
{
     return g_cCfgParse.ReadBrdTable(pbyBrdNum, ptBoardTable);
}

/*=============================================================================
  函 数 名： WriteBrdTable
  功    能： 写单板表
  算法实现： 
  全局变量： 
  参    数： u8 byBrdNum
             TBoardInfo** pptBoardTable
  返 回 值： u16 
=============================================================================*/
u16 CAgentInterface::WriteBrdTable(u8 byBrdNum, TBoardInfo* ptBoardTable)
{
    return g_cCfgParse.WriteBrdTable(byBrdNum, ptBoardTable);
}

/*=============================================================================
  函 数 名： ReadMixerTable
  功    能： 读混音器表
  算法实现： 
  全局变量： 
  参    数： u8* pbyMixNum
             TEqpMixerInfo** pptMixerTable
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::ReadMixerTable(u8* pbyMixNum, TEqpMixerInfo* ptMixerTable)
{
     return g_cCfgParse.ReadMixerTable(pbyMixNum, ptMixerTable);
}

/*=============================================================================
  函 数 名： WriteMixerTable
  功    能： 写混音器表
  算法实现： 
  全局变量： 
  参    数： u8 byMixNum
             TEqpMixerInfo** pptMixerTable
			 BOOL32 bIsNeedUpdatePortInTable 标识是否需要更新内存中的端口信息
  返 回 值： u16 
=============================================================================*/
u16 CAgentInterface::WriteMixerTable(u8 byMixNum, TEqpMixerInfo* ptMixerTable, BOOL32 bIsNeedUpdatePortInTable)
{
	// 外设在MCU侧的起始端口分配 [12/13/2011 chendaiwei]
	for( u8 byRow = 1; byRow <= byMixNum && ptMixerTable; byRow++ )
    {	
		u8 byMixerType = UNKONW_MIXER;
		if(GetMixerSubTypeByRunBrdId(ptMixerTable[byRow-1].GetRunBrdId(),byMixerType))
		{
			ptMixerTable[byRow-1].SetMcuRecvPort(GetPeriEqpMcuRecvStartPort(EQP_TYPE_MIXER,byMixerType));
		}
	}

    return g_cCfgParse.WriteMixerTable(byMixNum, ptMixerTable,bIsNeedUpdatePortInTable);
}

/*=============================================================================
  函 数 名： ReadTvTable
  功    能： 读电视墙表
  算法实现： 
  全局变量： 
  参    数： u8* pbyTvNum
             TEqpTVWallInfo** pptTvTable
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::ReadTvTable(u8* pbyTvNum, TEqpTVWallInfo* ptTvTable)
{
     return g_cCfgParse.ReadTvTable(pbyTvNum, ptTvTable);
}

/*=============================================================================
  函 数 名： WriteTvTable
  功    能： 写电视墙表
  算法实现： 
  全局变量： 
  参    数： u8 byTvNum
             TEqpTVWallInfo** pptTvTable
  返 回 值： u16 
=============================================================================*/
u16 CAgentInterface::WriteTvTable(u8 byTvNum, TEqpTVWallInfo* ptTvTable)
{
    return g_cCfgParse.WriteTvTable(byTvNum, ptTvTable);
}

/*=============================================================================
  函 数 名： ReadRecTable
  功    能： 读录像机表
  算法实现： 
  全局变量： 
  参    数： u8* pbyRecNum
             TEqpRecInfo** pptRecTable
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::ReadRecTable(u8* pbyRecNum, TEqpRecInfo* ptRecTable)
{
     return g_cCfgParse.ReadRecTable(pbyRecNum, ptRecTable);
}

/*=============================================================================
  函 数 名： WriteRecTable
  功    能： 写录像机表
  算法实现： 
  全局变量： 
  参    数： u8 byRecNum
             TEqpRecInfo** pptRecTable
			 BOOL32 bIsNeedUpdatePortInTable 标识是否需要更新内存中的端口信息
  返 回 值： u16 
=============================================================================*/
u16 CAgentInterface::WriteRecTable(u8 byRecNum, TEqpRecInfo* ptRecTable,BOOL32 bIsNeedUpdatePortInTable)
{
	// 外设在MCU侧的起始端口分配 [12/13/2011 chendaiwei]
	for( u8 byRow = 1; byRow <= byRecNum && ptRecTable; byRow++ )
    {
		ptRecTable[byRow-1].SetMcuRecvPort(GetPeriEqpMcuRecvStartPort(EQP_TYPE_RECORDER));
	}

    return g_cCfgParse.WriteRecTable(byRecNum, ptRecTable,bIsNeedUpdatePortInTable);
}

/*=============================================================================
  函 数 名： ReadVrsRecTable
  功    能： 读Vrs新录播表
  算法实现： 
  全局变量： 
  参    数： u8* pbyVrsRecNum
             TEqpVrsRecInfo** pptVrsRecTable
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::ReadVrsRecTable(u8* pbyVrsRecNum, TEqpVrsRecCfgInfo* ptVrsRecTable)
{
     return g_cCfgParse.ReadVrsRecTable(pbyVrsRecNum, ptVrsRecTable);
}

/*=============================================================================
  函 数 名： WriteVrsRecTable
  功    能： 写Vrs新录播表
  算法实现： 
  全局变量： 
  参    数： u8 byVrsRecNum
             TEqpVrsRecInfo** pptVrsRecTable
  返 回 值： u16 
=============================================================================*/
u16 CAgentInterface::WriteVrsRecTable(u8 byVrsRecNum, TEqpVrsRecCfgInfo* ptVrsRecTable)
{
    return g_cCfgParse.WriteVrsRecTable(byVrsRecNum, ptVrsRecTable);
}

/*=============================================================================
  函 数 名： ReadBasTable
  功    能： 读Bas表
  算法实现： 
  全局变量： 
  参    数： u8* pbyBasNum
             TEqpBasInfo** pptBasTable
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::ReadBasTable(u8* pbyBasNum, TEqpBasInfo* ptBasTable)
{
     return g_cCfgParse.ReadBasTable(pbyBasNum, ptBasTable);
}

/*=============================================================================
  函 数 名： ReadBasHDTable
  功    能： 读BasHD表
  算法实现： 
  全局变量： 
  参    数： u8* pbyBasHDNum
             TEqpBasHDInfo** pptBasHDTable
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::ReadBasHDTable(u8* pbyBasHDNum, TEqpBasHDInfo* ptBasHDTable)
{
     return g_cCfgParse.ReadBasHDTable(pbyBasHDNum, ptBasHDTable);
}

/*=============================================================================
  函 数 名： WriteBasTable
  功    能： 写Bas表
  算法实现： 
  全局变量： 
  参    数： u8 byBasNum
             TEqpBasInfo** pptBasTable
			 BOOL32 bIsNeedUpdatePortInTable 标识是否需要更新内存中的端口信息
  返 回 值： u16 
=============================================================================*/
u16 CAgentInterface::WriteBasTable(u8 byBasNum, TEqpBasInfo* ptBasTable, BOOL32 bIsNeedUpdatePortInTable)
{
	// 外设在MCU侧的起始端口分配 [12/13/2011 chendaiwei]
	for( u8 byRow = 1; byRow <= byBasNum && ptBasTable; byRow++ )
    {
		ptBasTable[byRow-1].SetMcuRecvPort(GetPeriEqpMcuRecvStartPort(EQP_TYPE_BAS));
	}

    return g_cCfgParse.WriteBasTable(byBasNum, ptBasTable,bIsNeedUpdatePortInTable);
}

/*=============================================================================
  函 数 名： WriteBasHDTable
  功    能： 写BasHD表
  算法实现： 
  全局变量： 
  参    数： u8 byBasHDNum
             TEqpBasHDInfo** pptBasHDTable
			 BOOL32 bIsNeedUpdatePortInTable 标识是否需要更新内存中的端口信息
  返 回 值： u16 
=============================================================================*/
u16 CAgentInterface::WriteBasHDTable(u8 byBasHDNum, TEqpBasHDInfo* ptBasHDTable, BOOL32 bIsNeedUpdatePortInTable)
{
	// 外设在MCU侧的起始端口分配 [12/13/2011 chendaiwei]
	for( u8 byRow = 1; byRow <= byBasHDNum && ptBasHDTable; byRow++ )
    {
		ptBasHDTable[byRow-1].SetMcuRecvPort(GetPeriEqpMcuRecvStartPort(EQP_TYPE_BAS));
	}

    return g_cCfgParse.WriteBasHDTable(byBasHDNum, ptBasHDTable, bIsNeedUpdatePortInTable);
}

/*=============================================================================
  函 数 名： ReadVmpTable
  功    能： 读Vmp表
  算法实现： 
  全局变量： 
  参    数：  u8* pbyVmpNum
             TEqpVMPInfo** pptVmpTabl
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::ReadVmpTable( u8* pbyVmpNum, TEqpVMPInfo* ptVmpTable)
{
     return g_cCfgParse.ReadVmpTable(pbyVmpNum, ptVmpTable);
}

/*=============================================================================
  函 数 名： WriteVmpTable
  功    能： 写Vmp表
  算法实现： 
  全局变量： 
  参    数： u8 byVmpNum
             TEqpVMPInfo** pptVmpTabl
			 BOOL32 bIsNeedUpdatePortInTable 标识是否需要更新内存中的端口信息
  返 回 值： u16 
=============================================================================*/
u16 CAgentInterface::WriteVmpTable(u8 byVmpNum, TEqpVMPInfo* ptVmpTable, BOOL32 bIsNeedUpdatePortInTable )
{
	// 外设在MCU侧的起始端口分配 [12/13/2011 chendaiwei]
	for( u8 byRow = 1; byRow <= byVmpNum && ptVmpTable; byRow++ )
    {
		ptVmpTable[byRow-1].SetMcuRecvPort(GetPeriEqpMcuRecvStartPort(EQP_TYPE_VMP));
	}

    return g_cCfgParse.WriteVmpTable(byVmpNum, ptVmpTable,bIsNeedUpdatePortInTable);
}

/*=============================================================================
  函 数 名： ReadMpwTable
  功    能： 读Mpw表
  算法实现： 
  全局变量： 
  参    数： u8* pbyMpwNum
             TEqpMpwInfo* ptMpwTable
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::ReadMpwTable(u8* pbyMpwNum, TEqpMpwInfo* ptMpwTable)
{
     return g_cCfgParse.ReadMpwTable(pbyMpwNum, ptMpwTable);
}

/*=============================================================================
  函 数 名： WriteMpwTable
  功    能： 写Mpw表
  算法实现： 
  全局变量： 
  参    数： u8 byMpwNum
             TEqpMpwInfo* ptMpwTable
  返 回 值： u16 
=============================================================================*/
u16 CAgentInterface::WriteMpwTable(u8 byMpwNum, TEqpMpwInfo* ptMpwTable)
{
    return g_cCfgParse.WriteMpwTable(byMpwNum, ptMpwTable);
}

//4.6新加外设读写配置表 jlb
/*=============================================================================
  函 数 名： ReadHduTable
  功    能： 读取整体Hdu表信息
  算法实现： 
  全局变量： 
  参    数： u8* pbyHduNum, TEqpHduInfo* pptHduTable
  返 回 值： u16 
=============================================================================*/
u16    CAgentInterface::ReadHduTable( u8* pbyHduNum, TEqpHduInfo* ptHduTable )
{
    return g_cCfgParse.ReadHduTable(pbyHduNum, ptHduTable);
}

/*=============================================================================
  函 数 名： WriteHduTable
  功    能： 设置整体Hdu表信息
  算法实现： 
  全局变量： 
  参    数： u8* pbyHduNum, TEqpHduInfo* pptHduTable
  返 回 值： u16 
=============================================================================*/
u16    CAgentInterface::WriteHduTable( u8 byHduNum, TEqpHduInfo* ptHduTable )
{
	    return g_cCfgParse.WriteHduTable(byHduNum, ptHduTable);
}  

/*=============================================================================
  函 数 名： WriteHduTable
  功    能： 读取整体Svmp表信息
  算法实现： 
  全局变量： 
  参    数： u8* pbySvmpNum, TEqpSvmpInfo* pptSvmpTable
  返 回 值： u16 
=============================================================================*/
u16    CAgentInterface::ReadSvmpTable( u8* pbySvmpNum, TEqpSvmpInfo* ptSvmpTable )
{
    return g_cCfgParse.ReadSvmpTable(pbySvmpNum, ptSvmpTable);
}

/*=============================================================================
  函 数 名： WriteSvmpTable
  功    能： 设置整体Svmp表信息
  算法实现： 
  全局变量： 
  参    数： u8 bySvmpNum, TEqpSvmpInfo* ptSvmpTable
			 BOOL32 bIsNeedUpdatePortInTable 标识是否需要更新内存中的端口信息
  返 回 值： u16 
=============================================================================*/
u16    CAgentInterface::WriteSvmpTable( u8 bySvmpNum, TEqpSvmpInfo* ptSvmpTable,BOOL32 bIsNeedUpdatePortInTable)
{
	// 外设在MCU侧的起始端口分配 [12/13/2011 chendaiwei]
	for( u8 byRow = 1; byRow <= bySvmpNum && ptSvmpTable; byRow++ )
    {
		ptSvmpTable[byRow-1].SetMcuRecvPort(GetPeriEqpMcuRecvStartPort(EQP_TYPE_VMP,ptSvmpTable[byRow-1].GetVmpType()));
	}

	return g_cCfgParse.WriteSvmpTable(bySvmpNum, ptSvmpTable,bIsNeedUpdatePortInTable);
}

/*=============================================================================
  函 数 名： ReadDvmpTable
  功    能： 读取整体Dvmp表信息
  算法实现： 
  全局变量： 
  参    数： u8* pbyDvmpNum, TEqpDvmpBasicInfo* pptDvmpTable
  返 回 值： u16 
=============================================================================*/ 
// u16    CAgentInterface::ReadDvmpTable( u8* pbyDvmpNum, TEqpDvmpBasicInfo* ptDvmpTable )
// {
//     return g_cCfgParse.ReadDvmpTable(pbyDvmpNum, ptDvmpTable);
// }

/*=============================================================================
  函 数 名： ReadDvmpTable
  功    能： 设置整体Dvmp表信息
  算法实现： 
  全局变量： 
  参    数： u8 byDvmpNum, TEqpDvmpBasicInfo* ptDvmpTable
  返 回 值： u16 
=============================================================================*/ 
// u16    CAgentInterface::WriteDvmpTable( u8 byDvmpNum, TEqpDvmpBasicInfo* ptDvmpTable )
// {
// 	 return g_cCfgParse.WriteDvmpTable(byDvmpNum, ptDvmpTable);
// }

/*=============================================================================
  函 数 名： ReadMpuBasTable
  功    能： 读取整体MpuBas表信息
  算法实现： 
  全局变量： 
  参    数： u8* pbyMpuBasNum, TEqpSvmpInfo* pptMpuBasTable
  返 回 值： u16 
=============================================================================*/
u16    CAgentInterface::ReadMpuBasTable( u8* pbyMpuBasNum, TEqpMpuBasInfo* ptMpuBasTable )
{
    return g_cCfgParse.ReadMpuBasTable(pbyMpuBasNum, ptMpuBasTable);
}

/*=============================================================================
  函 数 名： WriteMpuBasTable
  功    能： 设置整体MpuBas表信息
  算法实现： 
  全局变量： 
  参    数： u8 byMpuBasNum, TEqpSvmpInfo* ptMpuBasTable
			 BOOL32 bIsNeedUpdatePortInTable 标识是否需要更新内存中的端口信息
  返 回 值： u16 
=============================================================================*/
u16    CAgentInterface::WriteMpuBasTable( u8 byMpuBasNum, TEqpMpuBasInfo* ptMpuBasTable,BOOL32 bIsNeedUpdatePortInTable )
{
	// 外设在MCU侧的起始端口分配 [12/13/2011 chendaiwei]
	for( u8 byRow = 1; byRow <= byMpuBasNum && ptMpuBasTable; byRow++ )
	{
		ptMpuBasTable[byRow-1].SetMcuRecvPort(GetPeriEqpMcuRecvStartPort(EQP_TYPE_BAS,ptMpuBasTable[byRow-1].GetStartMode()));
	}

	return g_cCfgParse.WriteMpuBasTable(byMpuBasNum, ptMpuBasTable,bIsNeedUpdatePortInTable);
}

/*=============================================================================
  函 数 名： ReadEbapTable
  功    能： 读取整体Ebap表信息
  算法实现： 
  全局变量： 
  参    数： u8* pbyEbapNum, TEqpEbapInfo* ptEbapTable
  返 回 值： u16 
=============================================================================*/
// u16    CAgentInterface::ReadEbapTable( u8* pbyEbapNum, TEqpEbapInfo* ptEbapTable )
// {
//     return g_cCfgParse.ReadEbapTable(pbyEbapNum, ptEbapTable);
// }

/*=============================================================================
  函 数 名： WriteEbapTable
  功    能： 设置整体Ebap表信息
  算法实现： 
  全局变量： 
  参    数： u8 byEbapNum, TEqpEbapInfo* ptEbapTable
  返 回 值： u16 
=============================================================================*/
// u16    CAgentInterface::WriteEbapTable( u8 byEbapNum, TEqpEbapInfo* ptEbapTable )
// {
// 	 return g_cCfgParse.WriteEbapTable(byEbapNum, ptEbapTable);
// }


/*=============================================================================
  函 数 名： ReadEvpuTable
  功    能： 读取整体Evpu表信息
  算法实现： 
  全局变量： 
  参    数： u8* pbyEvpuNum, TEqpSvmpInfo* ptEvpuTable
  返 回 值： u16 
=============================================================================*/
// u16    CAgentInterface::ReadEvpuTable( u8* pbyEvpuNum, TEqpEvpuInfo* ptEvpuTable )
// {
//     return g_cCfgParse.ReadEvpuTable(pbyEvpuNum, ptEvpuTable);
// }

/*=============================================================================
  函 数 名： ReadEvpuTable
  功    能： 设置整体Evpu表信息
  算法实现： 
  全局变量： 
  参    数： u8* pbyEvpuNum, TEqpSvmpInfo* ptEvpuTable
  返 回 值： u16 
=============================================================================*/
// u16    CAgentInterface::WriteEvpuTable( u8 byEvpuNum, TEqpEvpuInfo* ptEvpuTable )
// {
// 	 return g_cCfgParse.WriteEvpuTable(byEvpuNum, ptEvpuTable);
// }

/*=============================================================================
  函 数 名： ReadHduSchemeTable
  功    能： 读取整体HDU预案设置表信息
  算法实现： 
  全局变量： 
  参    数： u8* pbyEvpuNum, TEqpSvmpInfo* ptEvpuTable
  返 回 值： u16 
=============================================================================*/
u16    CAgentInterface::ReadHduSchemeTable( u8* pbyHduProjectNum, THduStyleInfo* ptHduSchemeTable )
{
    return g_cCfgParse.ReadHduSchemeTable(pbyHduProjectNum, ptHduSchemeTable);
}

/*=============================================================================
  函 数 名： WriteHduSchemeTable
  功    能： 设置整体HDU预案设置表信息
  算法实现： 
  全局变量： 
  参    数： u8* pbyEvpuNum, TEqpSvmpInfo* ptEvpuTable
  返 回 值： u16 
=============================================================================*/
u16    CAgentInterface::WriteHduSchemeTable( u8 byHduProjectNum, THduStyleInfo* ptHduSchemeTable )
{
	 return g_cCfgParse.WriteHduSchemeTable(byHduProjectNum, ptHduSchemeTable);
} 



/*=============================================================================
  函 数 名： GetEqpRecorderCfg
  功    能： 取录像机配置信息
  算法实现： 
  全局变量： 
  参    数：  u8 byId
             TMcueqpRecorderEntry * ptRecCfg
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::GetEqpRecorderCfg( u8 byId, TEqpRecInfo * ptRecCfg )
{
     return g_cCfgParse.GetEqpRecCfgById( byId, ptRecCfg);
}

/*=============================================================================
  函 数 名： SetEqpRecorderCfg
  功    能： 设置录像机配置信息
  算法实现： 
  全局变量： 
  参    数：  u8 byId
             TmcueqpRecorderEntry * ptRecCfg
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::SetEqpRecorderCfg( u8 byId, TEqpRecInfo * ptRecCfg )
{
     return g_cCfgParse.SetEqpRecCfgById( byId, ptRecCfg);
}

/*=============================================================================
  函 数 名： GetEqpTVWallCfg
  功    能： 取电视墙信息
  算法实现： 
  全局变量： 
  参    数：  u8 byId
             TEqpTVWallInfo * ptTWCfg
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::GetEqpTVWallCfg( u8 byId, TEqpTVWallInfo * ptTWCfg )
{
     return g_cCfgParse.GetEqpTVWallCfgById(byId, ptTWCfg);
}

/*=============================================================================
  函 数 名： SetEqpTVWallCfg
  功    能： 设置电视墙信息
  算法实现： 
  全局变量： 
  参    数：  u8 byId
             TEqpTVWallInfo * ptTWCfg
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::SetEqpTVWallCfg( u8 byId, TEqpTVWallInfo * ptTWCfg )
{
     return g_cCfgParse.SetEqpTVWallCfgById(byId, ptTWCfg);
}

/*=============================================================================
  函 数 名： GetEqpMixerCfg
  功    能： 取混音器信息
  算法实现： 
  全局变量： 
  参    数：  u8 byId
             TEqpMixerInfo * ptMixerCfg
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::GetEqpMixerCfg( u8 byId, TEqpMixerInfo * ptMixerCfg )
{
     return g_cCfgParse.GetEqpMixerCfgById(byId, ptMixerCfg);
}

/*=============================================================================
  函 数 名： SetEqpMixerCfg
  功    能： 设置混音器信息
  算法实现： 
  全局变量： 
  参    数：  u8 byId
             TEqpMixerInfo tMixerCfg
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::SetEqpMixerCfg( u8 byId, TEqpMixerInfo tMixerCfg )
{
     return g_cCfgParse.SetEqpMixerCfgById(byId, tMixerCfg);
}

/*=============================================================================
  函 数 名： GetEqpBasCfg
  功    能： 取Bas信息
  算法实现： 
  全局变量： 
  参    数：  u8 byId
             TEqpBasInfo * ptBasCfg
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::GetEqpBasCfg( u8 byId, TEqpBasInfo * ptBasCfg )
{
     return g_cCfgParse.GetEqpBasCfgById(byId, ptBasCfg);
}

/*=============================================================================
  函 数 名： IsEqpBasHD
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 byId
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CAgentInterface::IsEqpBasHD( u8 byId )
{
     return g_cCfgParse.IsEqpBasHD(byId);
}

BOOL32 CAgentInterface::IsEqpBasAud (u8 byEqpId )
{
	return g_cCfgParse.IsEqpBasAud(byEqpId);
}

/*=============================================================================
  函 数 名： IsSVmp
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 byId
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CAgentInterface::IsSVmp( u8 byEqpId )
{
    return g_cCfgParse.IsSVmp( byEqpId );
}

/*=============================================================================
函 数 名： GetVmpEqpVersion
功    能： 获取当前Vmp的硬件版本
算法实现： 
全局变量： 
参    数： u8 byEqpId
返 回 值： u8 
------------------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
=============================================================================*/
u16 CAgentInterface::GetVmpEqpVersion( u8 byEqpId )
{
	return g_cCfgParse.GetVmpEqpVersion(byEqpId);
}

/*=============================================================================
  函 数 名： IsSVmp
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 byId
  返 回 值： BOOL32 
=============================================================================*/
// BOOL32 CAgentInterface::IsDVmp( u8 byEqpId )
// {
//     return g_cCfgParse.IsDVmp( byEqpId );
// }

/*=============================================================================
  函 数 名： IsSVmp
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 byId
  返 回 值： BOOL32 
=============================================================================*/
// BOOL32 CAgentInterface::IsEVpu( u8 byEqpId )
// {
//     return g_cCfgParse.IsEVpu( byEqpId );
// }

/*=============================================================================
  函 数 名： IsSVmp
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 byId
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CAgentInterface::IsMpuBas( u8 byEqpId )
{
    return g_cCfgParse.IsMpuBas( byEqpId );
}

/*=============================================================================
函 数 名： GetBasEqpVersion
功    能： 获取当前Bas的硬件版本
算法实现： 
全局变量： 
参    数： u8 byEqpId
返 回 值： u8 
------------------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
=============================================================================*/
u16 CAgentInterface::GetBasEqpVersion( u8 byEqpId )
{
	return g_cCfgParse.GetBasEqpVersion(byEqpId);
}

/*=============================================================================
函 数 名： GetHDUEqpVersion
功    能： 获取当前HDU的硬件版本
算法实现： 
全局变量： 
参    数： u8 byEqpId
返 回 值： u8 
------------------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2012/02/20  4.7.2        chendaiwei         创建
=============================================================================*/
u16 CAgentInterface::GetHDUEqpVersion( u8 byEqpId )
{
	return g_cCfgParse.GetHDUEqpVersion(byEqpId);
}

/*=============================================================================
  函 数 名： IsSVmp
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 byId
  返 回 值： BOOL32 
=============================================================================*/
// BOOL32 CAgentInterface::IsEBap( u8 byEqpId )
// {
//     return g_cCfgParse.IsEBap( byEqpId );
// }

/*=============================================================================
  函 数 名： GetEqpSvmpCfgById
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 byId
  返 回 值： BOOL32 
=============================================================================*/
u16    CAgentInterface::GetEqpSvmpCfgById( u8 byId, TEqpSvmpInfo * ptSvmpCfg )
{
    return g_cCfgParse.GetEqpSvmpCfgById( byId, ptSvmpCfg );
}

/*=============================================================================
  函 数 名： GetEqpBasHDCfg
  功    能： 取BasHD信息
  算法实现： 
  全局变量： 
  参    数：  u8 byId
             TEqpBasHDInfo * ptBasHDCfg
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::GetEqpBasHDCfg( u8 byId, TEqpBasHDInfo * ptBasHDCfg )
{
     return g_cCfgParse.GetEqpBasHDCfgById(byId, ptBasHDCfg);
}

/*=============================================================================
  函 数 名： GetMpuBasBrdInfo
  功    能： 取Mpu MPU2 bas 单板信息
  算法实现： 
  全局变量： 
  参    数：  u8 byId
             TEqpMpuBasInfo * ptEqpMpuBasInfo
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::GetMpuBasBrdInfo(u8 byBasEqpId,TEqpMpuBasInfo* ptEqpMpuBasInfo )
{
     return g_cCfgParse.GetMpuBasCfgById(byBasEqpId, ptEqpMpuBasInfo);
}

/*=============================================================================
  函 数 名： SetEqpBasCfg
  功    能： 设置Bas信息
  算法实现： 
  全局变量： 
  参    数：  u8 byId
             TEqpBasInfo * ptBasCfg
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::SetEqpBasCfg( u8 byId, TEqpBasInfo * ptBasCfg )
{
     return g_cCfgParse.SetEqpBasCfgById(byId, ptBasCfg);
}

/*=============================================================================
  函 数 名： SetEqpHduCfg
  功    能： 设置单个Hdu信息
  算法实现： 
  全局变量： 
  参    数：  u8 byId
             TEqpHduInfo* ptHduCfg
  返 回 值： BOOL32 
=============================================================================*/
u16    CAgentInterface::SetEqpHduCfg(u8 byHduId, TEqpHduInfo* ptHduCfg)         
{
	return g_cCfgParse.SetEqpHduCfgById( byHduId, ptHduCfg);
}
/*=============================================================================
  函 数 名： GetEqpHduCfg
  功    能： 读取单个Hdu信息
  算法实现： 
  全局变量： 
  参    数：  u8 byId
             TEqpHduInfo* ptHduCfg
  返 回 值： BOOL32 
=============================================================================*/  
u16    CAgentInterface::GetEqpHduCfg(u8 byHduId, TEqpHduInfo* ptHduCfg)         
{
	return g_cCfgParse.GetEqpHduCfgById(byHduId, ptHduCfg);
}
																				
/*=============================================================================
  函 数 名： GetEqpVMPCfg
  功    能： 取Vmp信息
  算法实现： 
  全局变量： 
  参    数：  u8 byId
             TEqpVMPInfo * ptVMPCfg
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::GetEqpVMPCfg( u8 byId, TEqpVMPInfo * ptVMPCfg )
{
    if ( IsSVmp(byId) )
    {
        return g_cCfgParse.GetEqpSvmpCfgById( byId, (TEqpSvmpInfo*)(void*)ptVMPCfg );
    }
    else if ( SUCCESS_AGENT != g_cCfgParse.GetEqpVMPCfgById(byId, ptVMPCfg) )
    {
		Agtlog(LOG_INFORM, "[GetEqpVMPCfg] can not find byId(%d) in all VmpTable!\n", byId );
        return ERR_AGENT_EQPNOTEXIST;
    }

    return SUCCESS_AGENT;
}

/*=============================================================================
  函 数 名： SetEqpVMPCfg
  功    能： 设置Vmp信息
  算法实现： 
  全局变量： 
  参    数：  u8 byId
             TEqpVMPInfo * ptVMPCfg
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::SetEqpVMPCfg( u8 byId, TEqpVMPInfo * ptVMPCfg )
{
     return g_cCfgParse.SetEqpVMPCfgById( byId, ptVMPCfg);
}

/*=============================================================================
  函 数 名： GetEqpMpwCfg
  功    能： 取指定Mpw的配置信息
  算法实现： 
  全局变量： 
  参    数： u8 byId
             TEqpMpwInfo* ptMpwCfg
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::GetEqpMpwCfg(u8 byId, TEqpMpwInfo* ptMpwCfg)
{
     return g_cCfgParse.GetEqpMpwCfgById(byId, ptMpwCfg);
}

/*=============================================================================
  函 数 名： SetEqpMpwCfg
  功    能： 设置指定Mpw的配置信息
  算法实现： 
  全局变量： 
  参    数： u8 byId
             TEqpMpwInfo* ptMpwCfg
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::SetEqpMpwCfg(u8 byId, TEqpMpwInfo* ptMpwCfg)
{
     return g_cCfgParse.SetEqpMpwCfgById(byId, ptMpwCfg);
}

/*=============================================================================
函 数 名： GetEqpSwitchBrdId
功    能： 获取指定外设转发板信息
算法实现： 
全局变量： 
参    数： u8 byId
返 回 值： 对应转发板ID
=============================================================================*/
u8 CAgentInterface::GetEqpSwitchBrdId(u8 byEqpId)
{
	return g_cCfgParse.GetEqpSwitchBrdId(byEqpId);
}
/*=============================================================================
函 数 名： GetEqpSwitchBrdId
功    能： 设置制定外设转发板信息
算法实现： 
全局变量： 
参    数： u8 byEqpId
返 回 值： 对应转发板ID
=============================================================================*/
BOOL32 CAgentInterface::SetEqpSwitchBrdId(u8 byEqpId, u8 bySwitchBrdId)
{
	return g_cCfgParse.SetEqpSwitchBrdId(byEqpId, bySwitchBrdId);
}

/*=============================================================================
  函 数 名： GetQosInfo
  功    能： 取Qos信息
  算法实现： 
  全局变量： 
  参    数： TQosInfo*  ptQosInfo
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::GetQosInfo(TQosInfo*  ptQosInfo )
{
     return g_cCfgParse.GetQosInfo(ptQosInfo);
}

/*=============================================================================
  函 数 名： GetLocalInfo
  功    能： 取本地信息
  算法实现： 
  全局变量： 
  参    数： TLocalInfo*  ptLocalInfo
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::GetLocalInfo(TLocalInfo*  ptLocalInfo )
{
	u16 wRet = g_cCfgParse.GetLocalInfo(ptLocalInfo);

#ifdef _8KH_
	if( Is800LMcu() && NULL != ptLocalInfo )
	{
		ptLocalInfo->SetMaxMcsOnGoingConfNum( 4 );
		ptLocalInfo->SetAdminLevel( DEF_ADMINLVEL );
		ptLocalInfo->SetConfNameShowType( 1 );
		ptLocalInfo->SetIsHoldDefaultConf( 1 );
		ptLocalInfo->SetIsSaveBand( 1 );
	}
#endif
    return wRet;
}

/*=============================================================================
  函 数 名： SetLocalInfo
  功    能： 设置本地信息
  算法实现： 
  全局变量： 
  参    数： TLocalInfo  tLocalInfo
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::SetLocalInfo(TLocalInfo  tLocalInfo )
{
#ifdef _8KH_
	if( Is800LMcu() )
	{
		tLocalInfo.SetMaxMcsOnGoingConfNum( 4 );
		tLocalInfo.SetAdminLevel( DEF_ADMINLVEL );
		tLocalInfo.SetConfNameShowType( 1 );
		tLocalInfo.SetIsHoldDefaultConf( 1 );
		tLocalInfo.SetIsSaveBand( 1 );
	}
#endif
	 return g_cCfgParse.SetLocalInfo(&tLocalInfo);
}

/*=============================================================================
  函 数 名： GetNetWorkInfo
  功    能： 取网络信息
  算法实现： 
  全局变量： 
  参    数： TNetWorkInfo*  ptNetWorkInfo
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::GetNetWorkInfo(TNetWorkInfo*  ptNetWorkInfo )
{
     return g_cCfgParse.GetNetWorkInfo(ptNetWorkInfo);
}

/*=============================================================================
  函 数 名： SetNetWorkInfo
  功    能： 设置网络信息
  算法实现： 
  全局变量： 
  参    数： TNetWorkInfo  tNetWorkInfo
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::SetNetWorkInfo(TNetWorkInfo  tNetWorkInfo )
{
     return g_cCfgParse.SetNetWorkInfo(&tNetWorkInfo);
}

/*=============================================================================
函 数 名： AddRegedMcsIp
功    能： 添加在线MCS的IP信息
算法实现： 
全局变量： 
参    数： u32 dwMcsIpAddr 
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2012/4/24   4.7			liaokang                创建
=============================================================================*/
BOOL32 CAgentInterface::AddRegedMcsIp( u32 dwMcsIpAddr )
{
    return g_cCfgParse.AddRegedMcsIp( dwMcsIpAddr );
}

/*=============================================================================
函 数 名： DeleteRegedMcsIp
功    能： 删除在线MCS的IP信息
算法实现： 
全局变量： 
参    数： u32 dwMcsIpAddr 
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2012/4/24   4.7			liaokang                创建
=============================================================================*/
BOOL32 CAgentInterface::DeleteRegedMcsIp( u32 dwMcsIpAddr )
{
    return g_cCfgParse.DeleteRegedMcsIp( dwMcsIpAddr );
}

/*=============================================================================
  函 数 名： IsMcuConfiged
  功    能： 判断MCU的配置标识
  算法实现： 
  全局变量： 无
  参    数： 无
  返 回 值： BOOL32 
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人       修改内容
  2006/12/21  4.0         周广程       创建
=============================================================================*/
BOOL32 CAgentInterface::IsMcuConfiged()
{
	return g_cCfgParse.IsMcuConfiged();
}

/*=============================================================================
  函 数 名： SetIsMcuConfiged
  功    能： 设置MCU的配置标识
  算法实现： 
  全局变量： 
  参    数： u8 byIsMcuConfiged
  返 回 值： u16 
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人       修改内容
  2006/12/21  4.0         周广程       创建
=============================================================================*/
u16 CAgentInterface::SetIsMcuConfiged(BOOL32 bConfiged)
{
	return g_cCfgParse.SetIsMcuConfiged(bConfiged);
}

/*=============================================================================
  函 数 名： GetDcsId
  功    能： 取DcsId
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： u32  
=============================================================================*/
u32   CAgentInterface::GetDcsIp()
{
    return g_cCfgParse.GetDcsIp();
}

/*=============================================================================
  函 数 名： SetDcsId
  功    能： 设置DcsId
  算法实现： 
  全局变量： 
  参    数： u8 byId
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::SetDcsIp(u32 dwDcsIp)
{
     return g_cCfgParse.SetDcsIp(dwDcsIp);
}

/*=============================================================================
  函 数 名： ReadPrsTable
  功    能： 取Prs表信息
  算法实现： 
  全局变量： 
  参    数： u8* pbyPrsNum
             TEqpPrsInfo* ptPrsTable
  返 回 值： u16 
=============================================================================*/
u16 CAgentInterface::ReadPrsTable(u8* pbyPrsNum, TEqpPrsInfo* ptPrsTable)
{
    return g_cCfgParse.ReadPrsTable(pbyPrsNum, ptPrsTable);
}

/*=============================================================================
  函 数 名： GetEqpPrsCfg
  功    能： 取单个Prs信息
  算法实现： 
  全局变量： 
  参    数： u8 byPrsId
             TEqpPrsInfo& tPrsCfg
  返 回 值： u16 
=============================================================================*/
u16 CAgentInterface::GetEqpPrsCfg(u8 byPrsId, TEqpPrsInfo& tPrsCfg)
{
    return g_cCfgParse.GetEqpPrsCfgById(byPrsId, &tPrsCfg);
}

/*=============================================================================
  函 数 名： WritePrsTable
  功    能： 设置Prs信息
  算法实现： 
  全局变量： 
  参    数： u8 byPrsNum
             TEqpPrsInfo* ptPrsTable
  返 回 值： u16 
=============================================================================*/
u16 CAgentInterface::WritePrsTable(u8 byPrsNum, TEqpPrsInfo* ptPrsTable)
{
    return g_cCfgParse.WritePrsTable(byPrsNum, ptPrsTable);
}

/*=============================================================================
  函 数 名： GetNetSyncInfo
  功    能： 取网同步信息
  算法实现： 
  全局变量： 
  参    数： TNetSyncInfo* ptNetSyncInfo
  返 回 值： void 
=============================================================================*/
u16 CAgentInterface::GetNetSyncInfo( TNetSyncInfo* ptNetSyncInfo )
{
     return g_cCfgParse.GetNetSyncInfo(ptNetSyncInfo);
}

/*=============================================================================
  函 数 名： SetNetSyncInfo
  功    能： 设置网同步信息
  算法实现： 
  全局变量： 
  参    数： TNetSyncInfo tNetSyncInfo
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::SetNetSyncInfo(TNetSyncInfo tNetSyncInfo)
{
    return g_cCfgParse.SetNetSyncInfo(&tNetSyncInfo);
}

/*=============================================================================
函 数 名： SetLoginInfo
功    能： 
算法实现： 
全局变量： 
参    数： TLoginInfo *ptLoginInfo
返 回 值： u16 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/10/19   4.0		周广程                  创建
=============================================================================*/
u16 CAgentInterface::SetLoginInfo( TLoginInfo *ptLoginInfo )
{
	return g_cCfgParse.SetLoginInfo( ptLoginInfo );
}

/*=============================================================================
函 数 名： GetLoginInfo
功    能： 
算法实现： 
全局变量： 
参    数： TLoginInfo *ptLoginInfo
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/10/19   4.0		周广程                  创建
=============================================================================*/
u16	CAgentInterface::GetLoginInfo( TLoginInfo *ptLoginInfo )
{
	return g_cCfgParse.GetLoginInfo( ptLoginInfo );
}

/*=============================================================================
  函 数 名： GetVmpAttachCfg
  功    能： 取画面合成附属信息
  算法实现： 
  全局变量： 
  参    数： TVmpAttachCfg* ptVmpAttachCfg
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::ReadVmpAttachTable(u8* pbyVmpProjectNum, TVmpAttachCfg* ptVmpAttachTable)
{
     return g_cCfgParse.ReadVmpAttachTable(pbyVmpProjectNum, ptVmpAttachTable);
}
// 多国语言 [pengguofeng 4/16/2013]
/*=============================================================================
函 数 名： ReadMcuEncodingType
功    能： 获取Mcu的编码方式
算法实现： 
全局变量： 
参    数： u8
返 回 值： u16
=============================================================================*/
u16 CAgentInterface::ReadMcuEncodingType(u8 &byEncoding)
{
	return g_cCfgParse.ReadMcuEncodingType(byEncoding);
}

/*=============================================================================
函 数 名： WriteMcuEncodingType
功    能： 写入Mcu的编码方式，写到文件中
算法实现： 
全局变量： 
参    数： u8
返 回 值： True/False
=============================================================================*/
BOOL32 CAgentInterface::WriteMcuEncodingType(const u8 &byEncoding)
{
	return g_cCfgParse.WriteMcuEncodingType(&byEncoding);
}

/*=============================================================================
函 数 名： GetMcuEncodingType
功    能： 获取Mcu的编码方式
算法实现： 
全局变量： 
参    数： 无
返 回 值： u8
=============================================================================*/
u8 CAgentInterface::GetMcuEncodingType(void)
{
	return g_cCfgParse.GetEncodingType();
}

/*=============================================================================
函 数 名： SetMcuEncodingType
功    能： 设置Mcu的编码方式
算法实现： 
全局变量： 
参    数： u8
返 回 值： 无
=============================================================================*/
void CAgentInterface::SetMcuEncodingType(const u8 &byEncoding)
{
	g_cCfgParse.SetEncodingType(byEncoding);
}
// end [pengguofeng 4/16/2013]
/*=============================================================================
  函 数 名： SetVmpAttachCfg
  功    能： 设置画面合成附属信息
  算法实现： 
  全局变量： 
  参    数： TVmpAttachCfg& tVmpAttachCfg
  返 回 值： u8 
=============================================================================*/
u16 CAgentInterface::WriteVmpAttachTable(u8 byVmpProjectNum, TVmpAttachCfg* ptVmpAttachTable)
{
    return g_cCfgParse.WriteVmpAttachTable(byVmpProjectNum, ptVmpAttachTable);
}
/*=============================================================================
  函 数 名： GetPriInfo
  功    能： 取外设信息
  算法实现： 
  全局变量： 
  参    数： u8 byEqpId
             u32* pdwEqpIp:网络序
             u8* pbyEqpType
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::GetPeriInfo(u8 byEqpId, u32* pdwEqpIp, u8* pbyEqpType)
{
    return g_cCfgParse.GetPriInfo(byEqpId, pdwEqpIp, pbyEqpType);
}

/*=============================================================================
  函 数 名： GetMpcIp
  功    能： 取MpcIp(网络序)
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： u32 
=============================================================================*/
u32 CAgentInterface::GetMpcIp()
{
    return g_cCfgParse.GetLocalIp();
}

/*=============================================================================
  函 数 名： SetMpcEnabled 
  功    能： 只提供了设置接口，通知Mcu代理本端MPC是否主用
  算法实现： 
  全局变量： 
  参    数： BOOL32 bEnabled
  返 回 值： void 
=============================================================================*/
u16 CAgentInterface::SetMpcEnabled(BOOL32 bEnabled)
{
    g_cCfgParse.SetMpcActive(bEnabled);
    return SUCCESS_AGENT;
}


/*=============================================================================
  函 数 名： GetMpcDIpAddr
  功    能： 取MpcD的Ip地址(网络序)
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： u32 
=============================================================================*/
u32 CAgentInterface::GetMpcDIpAddr(void)
{
    TMPCInfo tMPCInfo;
    g_cCfgParse.GetMPCInfo( &tMPCInfo );
    return tMPCInfo.GetOtherMpcIp();
}
/*=============================================================================
  函 数 名： GetMpcDPort
  功    能： 取MpcD的Port
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： u16 
=============================================================================*/
u16 CAgentInterface::GetMpcDPort(void)
{
    TMPCInfo tMPCInfo;
    g_cCfgParse.GetMPCInfo( &tMPCInfo );
    return tMPCInfo.GetOtherMpcPort();
}

/*=============================================================================
  函 数 名： IsHaveOtherMpc
  功    能： 当前是否配置了两块Mpc板
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： BOOL32 TRUE：配置了两块，FALSE：没有配置
=============================================================================*/
BOOL32 CAgentInterface::IsHaveOtherMpc(void)
{
    TMPCInfo tMPCInfo;
    g_cCfgParse.GetMPCInfo( &tMPCInfo );
    return tMPCInfo.GetIsHaveOtherMpc();
}

/*=============================================================================
  函 数 名： GetMcuAlias
  功    能： 取Mcu Alias
  算法实现： 
  全局变量： 
  参    数： s8* lpszAlias
             u8 byLength
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::GetMcuAlias(s8* pszAlias, u8 byLength)
{
    if( NULL == pszAlias)
    {
        return ERR_AGENT_GETNODEVALUE;
    }
    byLength = byLength > MAX_ALIAS_LENGTH ? MAX_ALIAS_LENGTH: byLength;

    TLocalInfo  tLocalInfo;
    u16 wRet = g_cCfgParse.GetLocalInfo(&tLocalInfo);
    memcpy( pszAlias, tLocalInfo.GetAlias(), byLength);
	
#ifdef _UTF8
	// 界面允许输入80字节，这里取32个，导致会有不完整的汉字产生 [pengguofeng 11/4/2013]
	CorrectUtf8Str(pszAlias, byLength);
#endif
    return wRet;
}

/*=============================================================================
  函 数 名： GetIsUseMpcTranData
  功    能： 取是否用Mcp转发数据
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： u8  
=============================================================================*/
BOOL32 CAgentInterface::GetIsUseMpcTranData()
{
    TNetWorkInfo  tNetWorkInfo;
    g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
    return(1 == tNetWorkInfo.GetMpcTransData());
}

/*=============================================================================
  函 数 名： GetIsUseMpcStack
  功    能： 取是否用Mcp内置协议栈
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CAgentInterface::GetIsUseMpcStack()
{
    TNetWorkInfo  tNetWorkInfo;
    g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
    return(1 == tNetWorkInfo.GetMpcStack());
}
/*=============================================================================
  函 数 名： GetCastIpAddr
  功    能： 取组播地址(网络序)
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： u32  
=============================================================================*/
u32  CAgentInterface::GetCastIpAddr()
{
    TNetWorkInfo  tNetWorkInfo;
    g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
    return tNetWorkInfo.GetCastIp();
}

/*=============================================================================
  函 数 名： GetCastPort
  功    能： 取组播端口
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： u16  
=============================================================================*/
u16  CAgentInterface::GetCastPort()
{
    TNetWorkInfo  tNetWorkInfo;
    g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
    return tNetWorkInfo.GetCastPort();
}
/*=============================================================================
  函 数 名： GetE164Number
  功    能： 取E164号
  算法实现： 
  全局变量： 
  参    数： s8* lpszE164Number
             u8 byLength
  返 回 值： void 
=============================================================================*/
u16 CAgentInterface::GetE164Number(s8* pszE164Number, u8 byLength)
{
    TLocalInfo  tLocalInfo;
    g_cCfgParse.GetLocalInfo(&tLocalInfo);
    memcpy(pszE164Number, tLocalInfo.GetE164Num(), byLength);
    return SUCCESS_AGENT;
}

/*=============================================================================
  函 数 名： GetGkIpAddr
  功    能： 取Gk地址(网络序)
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： u32 
=============================================================================*/
u32 CAgentInterface::GetGkIpAddr()
{
    TNetWorkInfo  tNetWorkInfo;
    g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
    return htonl(tNetWorkInfo.GetGkIp());
}

/*=============================================================================
  函 数 名： SetGKIpAddr
  功    能： 设置Gk地址
  算法实现： 
  全局变量： 
  参    数： u32 dwGkIp
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::SetGKIpAddr(u32 dwGkIp )
{
    TNetWorkInfo  tNetWorkInfo;
    g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
    tNetWorkInfo.SetGKIp(dwGkIp);
    return g_cCfgParse.SetNetWorkInfo(&tNetWorkInfo);
}

/*=============================================================================
  函 数 名： GetIsGKCharge
  功    能： 取GK是否进行计费
  算法实现： 
  全局变量： 
  参    数： u32 dwGkIp
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CAgentInterface::GetIsGKCharge()
{
    TNetWorkInfo  tNetWorkInfo;
    g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
    return tNetWorkInfo.GetIsGKCharge();
}

/*=============================================================================
函 数 名： GetRRQMtadpIp
功    能： 获取主协议适配地址
算法实现： 
全局变量： 
参    数： 
返 回 值： u32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/3/14  4.0			许世林                  创建
=============================================================================*/
u32 CAgentInterface::GetRRQMtadpIp()
{
    TNetWorkInfo  tNetWorkInfo;
    g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
    return tNetWorkInfo.GetRRQMtadpIp();
}

/*=============================================================================
函 数 名： GetGkRRQUsername
功    能： 获取GK RRQ 用户名
算法实现： 
全局变量： 
参    数： 
返 回 值： LPCSTR 用户名 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/3/19   4.7			陈代伟                 创建
=============================================================================*/
u8 CAgentInterface::GetGkRRQUsePwdFlag() const
{
    TNetWorkInfo  tNetWorkInfo;
    g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
    return tNetWorkInfo.GetGkRRQUsePwdFlag();
}

/*=============================================================================
函 数 名： GetGkRRQPassword
功    能： 获取GK RRQ 密码
算法实现： 
全局变量： 
参    数： 
返 回 值： LPCSTR 密码
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/3/19   4.7.2		陈代伟                 创建
=============================================================================*/
LPCSTR CAgentInterface::GetGkRRQPassword ( void )
{
    TNetWorkInfo  tNetWorkInfo;
    g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
    return tNetWorkInfo.GetGkRRQPassword();
}

/*=============================================================================
函 数 名： SetRRQMtadpIp
功    能： 设置 主协议适配地址
算法实现： 
全局变量： 
参    数： u32 dwIp 主机序
返 回 值： u32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/10/17  4.6			张宝卿                  创建
=============================================================================*/
void CAgentInterface::SetRRQMtadpIp(u32 dwIp)
{
    TNetWorkInfo  tNetWorkInfo;
    g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
    tNetWorkInfo.SetRRQMtadpIp(dwIp);
    g_cCfgParse.SetNetWorkInfo(&tNetWorkInfo);
	
    return;
}


/*=============================================================================
  函 数 名： GetRecvStartPort
  功    能： 取Mcu起始端口
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： u16 
=============================================================================*/
u16 CAgentInterface::GetRecvStartPort()
{
    TNetWorkInfo  tNetWorkInfo;
    g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
    return tNetWorkInfo.GetRecvStartPort();
}

/*=============================================================================
  函 数 名： SetRebootMsgDst
  功    能： 设置重其消息号
  算法实现： 
  全局变量： 
  参    数：  u16 wRebootEvent
              u32 dwNode = 0
  返 回 值：  void 
=============================================================================*/
void CAgentInterface::SetRebootMsgDst( u16 wRebootEvent, u32 dwNode = 0 )
{
	g_cCfgParse.SetRebootMsgDst(wRebootEvent, dwNode );
    return;
}

/*=============================================================================
  函 数 名： SetRunningMsgDst
  功    能： 设置从待机到运行消息号
  算法实现： 
  全局变量： 
  参    数：  u16 wRunningEvent
              u32 dwNode = 0
  返 回 值：  void 
=============================================================================*/
void CAgentInterface::SetRunningMsgDst( u16 wRunningEvent, u32 dwNode = 0 )
{

#ifndef WIN32
	g_cCfgParse.SetRunningMsgDst(wRunningEvent, dwNode );
#endif
    return;
}

/*=============================================================================
  函 数 名： SetPowerOffMsgDst
  功    能： 设置从运行到待机消息号
  算法实现： 
  全局变量： 
  参    数：  u16 wPowerOffEvent
             u32 dwNode = 0
  返 回 值： void 
=============================================================================*/
void CAgentInterface::SetPowerOffMsgDst( u16 wPowerOffEvent, u32 dwNode = 0 )
{

#ifndef WIN32
	g_cCfgParse.SetPowerOffMsgDst(wPowerOffEvent, dwNode );
#endif
    return;
}

/*=============================================================================
  函 数 名： GetPeriEqpType
  功    能： 取外设类型
  算法实现： 
  全局变量： 
  参    数： u8 byEqpId
  返 回 值： u8  
=============================================================================*/
u8  CAgentInterface::GetPeriEqpType(u8 byEqpId)
{
    u8  byEqpType = 0;
    u32 dwEqpIp = 0;
    g_cCfgParse.GetPriInfo(byEqpId, &dwEqpIp, &byEqpType);
    return  byEqpType;
}

/*=============================================================================
  函 数 名： SetSystemTime
  功    能： 设置系统时间
  算法实现： 
  全局变量： 
  参    数： time_t tTime
  返 回 值： u8 
=============================================================================*/
u16 CAgentInterface::SetSystemTime(time_t tTime)
{
    return g_cCfgParse.SetSystemTime(tTime);
}
/*=============================================================================
  函 数 名： GetBrdChoice
  功    能： 取单板网口选择
  算法实现： 
  全局变量： 
  参    数： u8 byLayer
             u8 bySlot
             u8 byType
  返 回 值： u8 
=============================================================================*/
u8 CAgentInterface::GetBrdChoice(u8 byLayer, u8 bySlot, u8 byType)
{
    TBrdPosition tPos;
    tPos.byBrdLayer = byLayer;
    tPos.byBrdSlot  = bySlot;
    tPos.byBrdID    = byType;

    TBoardInfo tBrdInfo;
    g_cCfgParse.GetBrdCfgById( g_cCfgParse.GetBrdId(tPos), &tBrdInfo );
    return tBrdInfo.GetPortChoice();
}

/*=============================================================================
  函 数 名： GetCriDriIsCast
  功    能： 取单板是否组播
  算法实现： 
  全局变量： 
  参    数： u8 byBrdId
  返 回 值： u8 
=============================================================================*/
BOOL32 CAgentInterface::GetCriDriIsCast(u8 byBrdId)
{
    u8 byRet = g_cCfgParse.GetCriDriIsCast(byBrdId);
    return(1 == byRet);
}

/*=============================================================================
  函 数 名： GetBrdIpAddr
  功    能： 取单板Ip(网络序)
  算法实现： 
  全局变量： 
  参    数： u8 byBrdId:单板Id
  返 回 值： u8  
=============================================================================*/
u32   CAgentInterface::GetBrdIpAddr( u8 byBrdId )
{
    /*
    TBoardInfo tBrdInfo;
    g_cCfgParse.GetBrdCfgById( byBrdId, &tBrdInfo );
    return htonl(tBrdInfo.GetBrdIp());*/

    return g_cCfgParse.GetBrdIpAddr( byBrdId );    
}

/*=============================================================================
函 数 名： GetBoardIpAddrFromBrdId
功    能： 取单板Ip(网络序)
算法实现： 
全局变量： 
参    数： u8 byBrdId:单板序号
返 回 值： u32  
=============================================================================*/
u32  CAgentInterface::GetBoardIpAddrFromIdx(u8 byBrdIdx)
{
    TBoardInfo tBrdInfo;
    g_cCfgParse.GetBrdCfgById( byBrdIdx, &tBrdInfo );
    return htonl(tBrdInfo.GetBrdIp());
}

/*=============================================================================
  函 数 名： Get225245StartPort
  功    能： 取225端口
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： u16  
=============================================================================*/
u16  CAgentInterface::Get225245StartPort()
{
    TNetWorkInfo  tNetWorkInfo;
    g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
    return tNetWorkInfo.Get225245StartPort();
}

/*=============================================================================
  函 数 名： Get225245MtNum
  功    能： 取终端数
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： u16  
=============================================================================*/
u16  CAgentInterface::Get225245MtNum()
{
    TNetWorkInfo  tNetWorkInfo;
    g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
    return tNetWorkInfo.Get225245MtNum();
}

/*=============================================================================
  函 数 名： SetQosInfo
  功    能： 设置Qos信息
  算法实现： 
  全局变量： 
  参    数： TQosInfo tQosInfo
  返 回 值： BOOL32 
=============================================================================*/
u16 CAgentInterface::SetQosInfo(TQosInfo tQosInfo )
{
    return g_cCfgParse.SetQosInfo(&tQosInfo);
}

/*=============================================================================
  函 数 名： GetMpcBoardId
  功    能： 取Mpc板的Id
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： u8 
=============================================================================*/
u8 CAgentInterface::GetMpcBoardId(void)
{
    u8 byMpcId = 0;
    TMPCInfo tMPCInfo;
    g_cCfgParse.GetMPCInfo( &tMPCInfo );

    if(0 == tMPCInfo.GetLocalSlot())
    {
        byMpcId = MCU_BOARD_MPC;
    }
    else if(1 == tMPCInfo.GetLocalSlot())
    {
        byMpcId = MCU_BOARD_MPCD;
    }
    return byMpcId;
}

/*=============================================================================
  函 数 名： GetBoardInfo
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u32 dwBrdIp
             u8* pbyBrdId
             u8* pbyLayer
             u8* pbySlot
   返 回 值： u8 
 =============================================================================*/
u16 CAgentInterface::GetBoardInfo(u32 dwBrdIp, u8* pbyBrdId, u8* pbyLayer, u8* pbySlot)
{
    u16 wRet = ERR_AGENT_GETPERIINFO;
    if( NULL == pbyBrdId )
    {
        Agtlog( LOG_ERROR, "[GetBoardInfo] param err: pbyBrdId.0x%x !\n", pbyBrdId );
        return wRet;
    }
    for(u8 byBrdId = 1; byBrdId <= MAX_BOARD_NUM; byBrdId ++)
    {
        TBoardInfo tBrdInfo;
        if ( g_cCfgParse.GetBrdCfgById( byBrdId, &tBrdInfo ) ) 
        {
            if ( ntohl(dwBrdIp) == tBrdInfo.GetBrdIp() )
            {
                *pbyBrdId = tBrdInfo.GetBrdId();
                if ( NULL != pbyLayer)
                {
                    *pbyLayer = tBrdInfo.GetLayer();
                }
                if ( NULL != pbySlot )
                {
                    *pbySlot = tBrdInfo.GetSlot();
                }
                wRet = SUCCESS_AGENT;
            }
        }
    }
    return wRet;
}

/*=============================================================================
  函 数 名： SetMpcIp
  功    能： 设置Mpc板Ip: 网络序
  算法实现： 
  全局变量： 
  参    数： u32 dwIp
  返 回 值： u8  
=============================================================================*/
u16   CAgentInterface::SetMpcIp(u32 dwIp, u8 byInterface)
{
    return g_cCfgParse.SetLocalIp(dwIp, byInterface);
} 

/*=============================================================================
  函 数 名： GetInterface
  功    能： 取前后网口选择
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： u32 
=============================================================================*/
u8 CAgentInterface::GetInterface(void)
{
    TMPCInfo tMPCInfo;
    g_cCfgParse.GetMPCInfo( &tMPCInfo );
	return tMPCInfo.GetLocalPortChoice();
}

/*=============================================================================
  函 数 名： SetInterface
  功    能： 设置前后网口选择
  算法实现： 
  全局变量： 
  参    数： u8 byInterface
  返 回 值： u16 
=============================================================================*/
u16 CAgentInterface::SetInterface(u8 byInterface)
{
    TMPCInfo tMPCInfo;
    g_cCfgParse.GetMPCInfo( &tMPCInfo );
    tMPCInfo.SetLocalPortChoice( byInterface );
	g_cCfgParse.SetMPCInfo(tMPCInfo);
	return SUCCESS_AGENT;
}
/*=============================================================================
  函 数 名： GetMaskIp
  功    能： 子网掩码(网络序)
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： u32  网络序
=============================================================================*/
u32  CAgentInterface::GetMaskIp()
{
    return g_cCfgParse.GetMpcMaskIp();
}

/*=============================================================================
  函 数 名： SetMaskIp
  功    能： 网络序
  算法实现： 
  全局变量： 
  参    数： u32 dwIp
  返 回 值： u8  
=============================================================================*/
u16   CAgentInterface::SetMaskIp(u32 dwIp, u8 byInterface)
{
    return g_cCfgParse.SetMpcMaskIp(dwIp, byInterface);
}

/*=============================================================================
  函 数 名： GetGateway
  功    能： 取网关Ip,网络序
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： u32  
=============================================================================*/
u32  CAgentInterface::GetGateway()
{
    return g_cCfgParse.GetMpcGateway();
}
/*=============================================================================
  函 数 名： SetGateway
  功    能： 设置网关Ip(网络序)
  算法实现： 
  全局变量： 
  参    数： u32 dwIp
  返 回 值： u8  
=============================================================================*/
u16   CAgentInterface::SetGateway(u32 dwIp)
{
	return g_cCfgParse.SetMpcGateway(dwIp);
}
    
/*=============================================================================
  函 数 名： GetHWVersion
  功    能： 取硬件版本号查询
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： u8  
=============================================================================*/
u8 CAgentInterface::GetHWVersion()
{
    return g_cCfgParse.GetMpcHWVersion();
}

/*=============================================================================
  函 数 名： GetCpuRate
  功    能： 取Cpu 占有率
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： u8  
=============================================================================*/
u8  CAgentInterface::GetCpuRate()
{
    return g_cCfgParse.GetCpuRate();
}

/*=============================================================================
  函 数 名： GetMemoryRate
  功    能： 取Memory占有率
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： u8  
=============================================================================*/
u32  CAgentInterface::GetMemoryRate()
{
    return g_cCfgParse.GetMemoryRate();
}

/*=============================================================================
  函 数 名： RebootBoard
  功    能： 重启指定单板
  算法实现： 
  全局变量： 
  参    数： TBrdPosition tBrdPos
  返 回 值： void 
=============================================================================*/
u16 CAgentInterface::RebootBoard(u8 byLayer, u8 bySlot, u8 byType)
{
    return g_cCfgParse.RebootBoard(byLayer, bySlot, byType);
}


/*=============================================================================
  函 数 名： GetLocalLayer
  功    能： 本机层号
  算法实现： 
  全局变量： 
  参    数： u8& byLayer
  返 回 值： u16 
=============================================================================*/
u16 CAgentInterface::GetLocalLayer(u8& byLayer)
{
    TMPCInfo tMPCInfo;
    g_cCfgParse.GetMPCInfo( &tMPCInfo );
    byLayer = tMPCInfo.GetLocalLayer();
    return SUCCESS_AGENT;
}

/*=============================================================================
  函 数 名： GetLocalSlot
  功    能： 本机槽号
  算法实现： 
  全局变量： 
  参    数： u8& bySlot
  返 回 值： u16 
=============================================================================*/
u16 CAgentInterface::GetLocalSlot(u8& bySlot)
{
    TMPCInfo tMPCInfo;
    g_cCfgParse.GetMPCInfo( &tMPCInfo );
    bySlot = tMPCInfo.GetLocalSlot();
    return SUCCESS_AGENT;
}



#ifdef _LINUX_

u32 CAgentInterface::EthIdxSys2Hard( u32 dwSysIdx )
{
	return g_cCfgParse.EthIdxSys2Hard( dwSysIdx );
}

#endif

#ifdef _MINIMCU_
/*=============================================================================
  函 数 名： Get8000BInfo
  功    能： 获得8000B的DSC特殊配置信息
  算法实现： 
  全局变量： 
  参    数： TDSCInfo *ptMcu8000BInfo
  返 回 值： u16 
=============================================================================*/
u16 CAgentInterface::GetDscInfo( TDSCModuleInfo *ptMcuDscInfo )
{
    return g_cCfgParse.GetDSCInfo( ptMcuDscInfo );
}


/*=============================================================================
函 数 名： GetLastDscInfo
功    能： 
算法实现： 
全局变量： 
参    数： TDSCModuleInfo * ptDscInfo
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/7/2   4.0			周广程                  创建
=============================================================================*/
u16	CAgentInterface::GetLastDscInfo( TDSCModuleInfo * ptDscInfo )
{
	return g_cCfgParse.GetLastDscInfo( ptDscInfo );
}

/*=============================================================================
函 数 名： SetLastDscInfo
功    能： 
算法实现： 
全局变量： 
参    数： TDSCModuleInfo * ptDscInfo
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/7/2   4.0			周广程                  创建
=============================================================================*/
u16	CAgentInterface::SetLastDscInfo( TDSCModuleInfo * ptDscInfo )
{
	return g_cCfgParse.SetLastDscInfo( ptDscInfo );
}

/*=============================================================================
函 数 名： GetConfigedDscType
功    能： 
算法实现： 
全局变量： 
参    数： void
返 回 值： u8(如果配置了DSC，则返回DSC的类型，否则返回0) 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/2/28   4.0			周广程                  创建
2007/3/05	4.0			周广程					修改
=============================================================================*/
u8 CAgentInterface::GetConfigedDscType( void )
{
	return g_cCfgParse.GetConfigedDscType();
}

/*=============================================================================
  函 数 名： SetDscInfo
  功    能： 设置8000B的DSC特殊配置信息
  算法实现： 
  全局变量： 
  参    数： TDSCModuleInfo *ptMcuDscInfo, BOOL32 bToFile
  返 回 值： u16 
=============================================================================*/
u16 CAgentInterface::SetDscInfo( TDSCModuleInfo *ptMcuDscInfo, BOOL32 bToFile )
{
    return g_cCfgParse.SetDSCInfo( ptMcuDscInfo, bToFile);
}

/*=============================================================================
函 数 名： SaveDscLocalInfoToNip
功    能： 
算法实现： 
全局变量： 
参    数： TDSCModuleInfo * ptDscInfo
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/7/17   4.0			周广程                  创建
=============================================================================*/
BOOL32 CAgentInterface::SaveDscLocalInfoToNip( TDSCModuleInfo * ptDscInfo )
{
	return g_cCfgParse.SaveDscLocalInfoToNip( ptDscInfo );
}

/*=============================================================================
函 数 名： SaveRouteToNipByDscInfo
功    能： 
算法实现： 
全局变量： 
参    数： TDSCModuleInfo *ptDscInfo
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/9/21   4.0			周广程                  创建
=============================================================================*/
BOOL32 CAgentInterface::SaveRouteToNipByDscInfo( TDSCModuleInfo *ptDscInfo )
{
	return g_cCfgParse.SaveRouteToNipByDscInfo( ptDscInfo );
}

/*=============================================================================
函 数 名： IsDscReged
功    能： 
算法实现： 
全局变量： 
参    数： 无
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/7/3   4.0			周广程                  创建
=============================================================================*/
BOOL32 CAgentInterface::IsDscReged(u8 &byDscType)
{
	u8 byDscAgtType = BRD_TYPE_UNKNOW;
	BOOL32 bRet = g_cCfgParse.IsDscReged(byDscAgtType);
	// [pengjie 2010/4/26] 接口改变
	//byDscType = g_cCfgParse.GetBrdType4Mcu(byDscAgtType);
	byDscType = g_cCfgParse.GetSnmpBrdTypeFromHW(byDscAgtType);
	return bRet;
}
#endif

/*=============================================================================
  函 数 名： GetMpRunBrdTypeByMpIdx
  功    能： 通过板的层槽组合索引查询运行板类型
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： u8
=============================================================================*/
u8 CAgentInterface::GetRunBrdTypeByIdx( u8 byMpId )
{
    return g_cCfgParse.GetRunBrdTypeByIdx(byMpId);
}

/*=============================================================================
函 数 名： GetMcuCfgInfoLevel
功    能： 
算法实现： 
全局变量： 
参    数： void
返 回 值： u8 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/7/25   4.0			周广程                  创建
=============================================================================*/
u8 CAgentInterface::GetMcuCfgInfoLevel(void)
{
	TMcuSystem tSystem;
	g_cCfgParse.GetSystemInfo( &tSystem );
	return tSystem.GetMcuCfgInfoLevel();
}

/*=============================================================================
  函 数 名： IsMcuPdtBrdMatch
  功    能： 检查业务与MPC板类型是否匹配
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： BOOL32
=============================================================================*/
BOOL32 CAgentInterface::IsMcuPdtBrdMatch(u8 byPdtType) const
{
    return g_cCfgParse.IsMcuPdtBrdMatch(byPdtType);
}

/*=============================================================================
函 数 名： IsMcu8000BHD
功    能： 是否是 MCU-8000B-HD
算法实现： 
全局变量： 
参    数： 
返 回 值： u8 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/03/27  4.5			顾振华                  创建
=============================================================================*/
#ifdef _MINIMCU_
BOOL32 CAgentInterface::IsMcu8000BHD() const
{
    return g_cCfgParse.IsMcu8000BHD();
}
#endif

// VCS
/*=============================================================================
函 数 名： SetVCSSoftName
功    能： 设置VCS软件自定义名称
算法实现： 
全局变量： 
参    数： 
返 回 值： u16 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
09/05/11    4.5			付秀华                  创建
=============================================================================*/
u16 CAgentInterface::SetVCSSoftName( s8* pachSoftName )
{
    return g_cCfgParse.SetVCSSoftName( pachSoftName );
}
/*=============================================================================
函 数 名： GetVCSSoftName
功    能： 获取VCS软件自定义名称
算法实现： 
全局变量： 
参    数： 
返 回 值： void 
  ----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
09/05/11    4.5			付秀华                  创建
=============================================================================*/
void CAgentInterface::GetVCSSoftName( s8* pachSoftName )
{
    g_cCfgParse.GetVCSSoftName( pachSoftName );
}

/*=============================================================================
函 数 名： SetEqpExCfgInfo
功    能： 设置外设扩展配置
算法实现： 
全局变量： 
参    数： IN  TEqpExCfgInfo &tEqpExCfgInfo
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
20100108    4.6			pengjie                create
=============================================================================*/
u16 CAgentInterface::SetEqpExCfgInfo( TEqpExCfgInfo &tEqpExCfgInfo )
{
	u16 wRet = SUCCESS_AGENT;
	g_cCfgParse.SetEqpExCfgInfo( tEqpExCfgInfo );
    return wRet;
}

/*=============================================================================
函 数 名： GetEqpExCfgInfo
功    能： 得到外设扩展配置
算法实现： 
全局变量： 
参    数： OUT  TEqpExCfgInfo &tEqpExCfgInfo
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
20100108    4.6			pengjie                create
=============================================================================*/
u16 CAgentInterface::GetEqpExCfgInfo( TEqpExCfgInfo &tEqpExCfgInfo )
{
	u16 wRet = SUCCESS_AGENT;
	g_cCfgParse.GetEqpExCfgInfo( tEqpExCfgInfo );
    return wRet;
}

u16 CAgentInterface::SetMcuCompileTime(s8 *pszCompileTime)
{
	u16 wRet = SUCCESS_AGENT;
	g_cCfgParse.SetMcuCompileTime( pszCompileTime );
    return wRet;
}

u16 CAgentInterface::SetAuthMTNum(u16 wAuthMTNum)
{
	u16 wRet = SUCCESS_AGENT;
	TMcuPfmInfo* ptMcuPfmInfo = g_cCfgParse.GetMcuPfmInfo();
	if (ptMcuPfmInfo == NULL)
	{
		return ~wRet;
	}
	ptMcuPfmInfo->SetAuthMTNum(wAuthMTNum);
	return wRet;
}

u16 CAgentInterface::SetAuthMTTotal(u16 wAuthTotal)
{
	u16 wRet = SUCCESS_AGENT;
	TMcuPfmInfo* ptMcuPfmInfo = g_cCfgParse.GetMcuPfmInfo();
	if (ptMcuPfmInfo == NULL)
	{
		return ~wRet;
	}
	ptMcuPfmInfo->SetAuthMTTotal(wAuthTotal);
	return wRet;
}

u16 CAgentInterface::SetEqpStat(u8 byEqpType, u16 wEqpUsedNum, u16 wEqpTotal)
{
	u16 wRet = SUCCESS_AGENT;
	TMcuPfmInfo* ptMcuPfmInfo = g_cCfgParse.GetMcuPfmInfo();
	if (ptMcuPfmInfo == NULL)
	{
		return ~wRet;
	}
	ptMcuPfmInfo->SetEqpStat(byEqpType, wEqpUsedNum, wEqpTotal);
	return wRet;
}

void CAgentInterface::GetIpFromU32(s8* achDstStr, u32 dwIpAddr)
{
	g_cCfgParse.GetIpFromU32(achDstStr, dwIpAddr);
}

// 以下8ke 专用
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
#ifdef _8KI_
u16 CAgentInterface::SetNewNetCfgInfo(const TNewNetCfg &tNewNetcfgInfo, const s8 bySecEthIndx/* = -1*/)
{
	return g_cCfgParse.SetNewNetCfgInfo( tNewNetcfgInfo, bySecEthIndx );
}

u16 CAgentInterface::GetNewNetCfgInfo(TNewNetCfg &tNewNetCfgInfo)
{
	return g_cCfgParse.GetNewNetCfgInfo( tNewNetCfgInfo );
}

u16	CAgentInterface::GetMacByEthIdx( const u8 &byEthIdx,s8* achMac,const u8 &byMacLen )
{	
	if( byMacLen < MAX_MACADDR_STR_LEN )
	{
		return ERR_AGENT_VALUEBESET;
	}

	if( byEthIdx >= MAXNUM_ETH_INTERFACE )
	{
		return ERR_AGENT_VALUEBESET; 
	}
#ifdef _LINUX_	
	TEthInfo tEthInfo[MAXNUM_ETH_INTERFACE];
	u8 byEthNum = 0;

	GetEthInfo(tEthInfo, byEthNum);

	memcpy( achMac, tEthInfo[byEthIdx].GetMacAddrStr(), MAX_MACADDR_STR_LEN );


#endif

	return SUCCESS_AGENT;
}

#endif

u16 CAgentInterface::SetSipRegGkInfo( u32 dwGKIp )
{
	return g_cCfgParse.SetSipRegGkInfo( dwGKIp );
}

u16 CAgentInterface::SetProxyDMZInfo( TProxyDMZInfo &tProxyDMZInfo )
{
	u16 wRet = SUCCESS_AGENT;
	g_cCfgParse.SetProxyDMZInfo( tProxyDMZInfo );
    return wRet;
}

u16 CAgentInterface::GetProxyDMZInfo( TProxyDMZInfo &tProxyDMZInfo )
{
	u16 wRet = SUCCESS_AGENT;
	g_cCfgParse.GetProxyDMZInfo( tProxyDMZInfo );
    return wRet;
}

u16 CAgentInterface::GetProxyDMZInfoFromMcuCfgFile( TProxyDMZInfo &tProxyDMZInfo )
{
	u16 wRet = SUCCESS_AGENT;
	g_cCfgParse.GetProxyDMZInfoFromMcuCfgFile( tProxyDMZInfo );
    return wRet;
}

u16 CAgentInterface::SetGkProxyCfgInfo( TGKProxyCfgInfo &tGKProxyCfgInfo )
{
	u16 wRet = SUCCESS_AGENT;
	g_cCfgParse.SetGkProxyCfgInfo( tGKProxyCfgInfo );
    return wRet;
}

u16 CAgentInterface::GetGkProxyCfgInfo( TGKProxyCfgInfo &tGKProxyCfgInfo )
{
	u16 wRet = SUCCESS_AGENT;
	g_cCfgParse.GetGkProxyCfgInfo( tGKProxyCfgInfo );
    return wRet;
}

u16 CAgentInterface::SetPrsTimeSpanCfgInfo( TPrsTimeSpan &tPrsTimeSpan )
{
	u16 wRet = SUCCESS_AGENT;
	g_cCfgParse.SetPrsTimeSpanCfgInfo( tPrsTimeSpan );
    return wRet;
}

u16 CAgentInterface::GetPrsTimeSpanCfgInfo( TPrsTimeSpan &tPrsTimeSpan )
{
	u16 wRet = SUCCESS_AGENT;
	g_cCfgParse.GetPrsTimeSpanCfgInfo( tPrsTimeSpan );
    return wRet;
}

u16 CAgentInterface::GetMcuEqpCfg( TMcu8KECfg * ptMcuEqpCfg )
{
    return g_cCfgParse.GetMcuEqpCfg( ptMcuEqpCfg );
}

/*=============================================================================
  函 数 名： SetMcu8KIEqpCfg
  功    能： 保存8KI网络配置到内存中（多网卡多IP配置 网关 掩码配置及路由设置）
  算法实现： 
  全局变量： 
  参    数： TMcu8KECfg* ptMcu8KECfg:网络配置信息
  返 回 值： u16 
=============================================================================*/
u16 CAgentInterface::SetMcu8KIEqpCfg( TMcu8KECfg * ptMcuEqpCfg )
{
    return g_cCfgParse.SetMcu8KIEqpCfg( ptMcuEqpCfg );
}

u16 CAgentInterface::SetMcuEqpCfg(TNetAdaptInfoAll * ptNetAdaptInfoAll)
{
	return g_cCfgParse.SetMcuEqpCfg(ptNetAdaptInfoAll);
}

u16 CAgentInterface::SetRouteCfg(TMultiNetCfgInfo &tMultiNetCfgInfo)
{
#if defined(_8KE_) || defined(_8KH_)
	return g_cCfgParse.SetRouteCfg(tMultiNetCfgInfo);
#else
	//8000I不提供路由在哪个网口的信息,所以需要重新整理路由在哪个网口(通过和ip配置比较获得)
	TNewNetCfg tNewNetCfg;
	TEthCfg tEthCfg;
	TNetCfg tNetCfg;
	GetNewNetCfgInfo( tNewNetCfg );
	u8 byRouteIdx = 0;
	u8 byRouteEthIdx = 0;
	u8 byRouteEthSubIdx = 0;
	TMultiNetCfgInfo tResultCfgInfo;
	u32 dwRouteIp = 0,dwIp = 0;
	u32 dwRouteMask = 0,dwMask = 0;
	u32 dwRouteGateIp = 0;
	u8 byIpIdx = 0;
	u8 byIsFind = 0;
	u32 adwRouteIp[MAXNUM_ETH_INTERFACE*MCU_MAXNUM_ADAPTER_IP];
	u32 adwRouteMask[MAXNUM_ETH_INTERFACE*MCU_MAXNUM_ADAPTER_IP];
	u32 adwRouteGate[MAXNUM_ETH_INTERFACE*MCU_MAXNUM_ADAPTER_IP];
	u8 byOtherRouteIdx = 0;
	u8 bIsInsert = 0;
	
	for( byRouteIdx = 0;byRouteIdx<MAXNUM_ETH_INTERFACE*MCU_MAXNUM_ADAPTER_IP;++byRouteIdx,++byRouteEthSubIdx )
	{
		byRouteEthIdx    = byRouteIdx / MCU_MAXNUM_ADAPTER_IP;
		byRouteEthSubIdx = byRouteIdx % MCU_MAXNUM_ADAPTER_IP;

		/*if(  0 == byRouteEthSubIdx && byRouteIdx > 0 )
		{
			byIpIdx = 0;
			byRouteEthIdx++;
		}*/

		if( tMultiNetCfgInfo.GetMcuEthCfg(byRouteEthIdx) == NULL )
		{
			continue;
		}

		if( byRouteEthSubIdx >= tMultiNetCfgInfo.GetMcuEthCfg(byRouteEthIdx)->GetIpSecNum() )
		{
			continue;
		}

		dwRouteIp	= tMultiNetCfgInfo.GetMcuEthCfg(byRouteEthIdx)->GetMcuIpAddr(byRouteEthSubIdx);
		dwRouteMask = tMultiNetCfgInfo.GetMcuEthCfg(byRouteEthIdx)->GetMcuSubNetMask(byRouteEthSubIdx);
		dwRouteGateIp = tMultiNetCfgInfo.GetMcuEthCfg(byRouteEthIdx)->GetGWIpAddr(byRouteEthSubIdx);

		if( 0 == dwRouteIp )
		{
			continue;
		}

		byIsFind = 0;
		bIsInsert = 0;
		for( u8 byIdx = 0;byIdx < MAXNUM_ETH_INTERFACE;++byIdx )
		{
			tEthCfg.Clear();
			
			tNewNetCfg.GetEthCfg( byIdx,tEthCfg );
			for( u8 byIpIdx = 0;byIpIdx < MCU_MAXNUM_ADAPTER_IP;++byIpIdx )
			{
				tNetCfg.Clear();
				tEthCfg.GetNetCfg( byIpIdx,tNetCfg );	
				
				dwIp   = tNetCfg.GetIpAddr();
				dwMask = tNetCfg.GetIpMask();

				if( 0 == dwIp )
				{
					continue;
				}
		
				//if( (dwRouteIp & dwRouteMask) == (dwIp & dwMask) )
				if( (dwRouteGateIp & dwMask) == (dwIp & dwMask) )
				{
					if( tResultCfgInfo.GetMcuEthCfg(byIdx) != NULL )
					{
						for( byIpIdx = 0;byIpIdx < MCU_MAXNUM_ADAPTER_IP;++byIpIdx )
						{
							if( tResultCfgInfo.GetMcuEthCfg(byIdx)->GetMcuIpAddr(byIpIdx) == 0 )
							{
								tResultCfgInfo.GetMcuEthCfg(byIdx)->AddIpSection( dwRouteIp,dwRouteMask,dwRouteGateIp);
								//	SetMcuIpAddr( dwRouteIp,byIpIdx );
								//tResultCfgInfo.GetMcuEthCfg(byIdx)->SetMcuSubNetMask( dwRouteMask,byIpIdx );
								//tResultCfgInfo.GetMcuEthCfg(byIdx)->SetGWIpAddr( dwRouteGateIp,byIpIdx );
								bIsInsert = 1;
								break;
							}
						}

					}
					byIsFind = 1;

					break;
				}
			}
			if( 1 == byIsFind )
			{
				break;
			}			
		}
		if( 0 == byIsFind || 0 == bIsInsert )
		{
			adwRouteIp[byOtherRouteIdx]   = dwRouteIp;
			adwRouteMask[byOtherRouteIdx] = dwRouteMask;
			adwRouteGate[byOtherRouteIdx] = dwRouteGateIp;
			++byOtherRouteIdx;
		}
	}

	byRouteEthIdx = 0;
	byIpIdx		  = 0;
	for( byRouteIdx = 0;byRouteIdx<byOtherRouteIdx; )
	{
		for( byIpIdx = 0;byIpIdx < MCU_MAXNUM_ADAPTER_IP;++byIpIdx )
		{		
			if( tResultCfgInfo.GetMcuEthCfg(byRouteEthIdx)->GetMcuIpAddr(byIpIdx) == 0 )
			{				
				tResultCfgInfo.GetMcuEthCfg(byRouteEthIdx)->AddIpSection( adwRouteIp[byRouteIdx],
					adwRouteMask[byRouteIdx],adwRouteGate[byRouteIdx] );
				//	SetMcuIpAddr( adwRouteIp[byRouteIdx],byIpIdx );
				//tResultCfgInfo.GetMcuEthCfg(byRouteEthIdx)->SetMcuSubNetMask( adwRouteMask[byRouteIdx],byIpIdx );
				//tResultCfgInfo.GetMcuEthCfg(byRouteEthIdx)->SetGWIpAddr( adwRouteGate[byRouteIdx],byIpIdx );
				++byRouteIdx;
				//tResultCfgInfo.GetMcuEthCfg(byRouteEthIdx)->ModifyIpSection()
				break;
			}
		}
		if( MCU_MAXNUM_ADAPTER_IP == byIpIdx )
		{
			byRouteEthIdx++;
			if( byRouteEthIdx >= MAXNUM_ETH_INTERFACE )
			{
				break;
			}
		}
	}

	return g_cCfgParse.SetRouteCfg( tResultCfgInfo );
#endif


}


u16 CAgentInterface::SetMultiManuNetCfg( const TMultiManuNetCfg &tMultiManuNetCfg, const TMultiEthManuNetAccess *tMultiEthManuNetAccess, const u8 byEthNum )
{
	return g_cCfgParse.SetMultiManuNetCfg(tMultiManuNetCfg, tMultiEthManuNetAccess, byEthNum);
}

u16 CAgentInterface::GetMultiNetCfgInfo(TMultiNetCfgInfo &tMultiNetCfgInfo)
{
	return g_cCfgParse.GetMultiNetCfgInfo(tMultiNetCfgInfo);
}

u16 CAgentInterface::GetMultiManuNetAccess(TMultiManuNetAccess &tMultiManuNetAccess,BOOL32 bFromMcuCfgFile, TMultiEthManuNetAccess *tMultiEthManuNetAccess, u8 *byEthNum)
{
	return g_cCfgParse.GetMultiManuNetAccess(tMultiManuNetAccess,bFromMcuCfgFile,tMultiEthManuNetAccess, byEthNum);
}

u16 CAgentInterface::WriteMultiManuNetAccess(const TMultiManuNetAccess &tMultiManuNetAccess, const TMultiEthManuNetAccess *tMultiEthManuNetAccess, const u8 byEthNum)
{
	return g_cCfgParse.WriteMultiManuNetAccess(tMultiManuNetAccess, tMultiEthManuNetAccess, byEthNum);
}

BOOL32 CAgentInterface::GetGkProxyCfgInfoFromCfgFile ( TGKProxyCfgInfo &tgkProxyCfgInfo )
{
	s8 achCfgName[256]={0};
	sprintf( achCfgName, "%s/%s", DIR_CONFIG, MCUCFGFILENAME );
	
	BOOL32 bResult = g_cCfgParse.AgentGetGkProxyCfgInfo( achCfgName );
	g_cCfgParse.GetGkProxyCfgInfo(tgkProxyCfgInfo);
	
	return bResult;
}

/*=============================================================================
  函 数 名： Save8KENetCfgToSys
  功    能： 刷新8KE mcu系统的网络配置,包括多网卡多IP配置 网关 掩码配置及路由设置
  算法实现： 
  全局变量： 
  参    数： TMcu8KECfg* ptMcu8KECfg:网络配置信息
  返 回 值： u16 
=============================================================================*/
u16 CAgentInterface::Save8KENetCfgToSys(TMcu8KECfg * ptMcu8KECfg)
{
	return g_cCfgParse.Save8KENetCfgToSys(ptMcu8KECfg);
}
 
void CAgentInterface::SetRouteToSys(void)
{
	g_cCfgParse.SetRouteToSys();
}
/*=============================================================================
  函 数 名： SearchPortChoice
  功    能： 查找当前使用的网口
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： u8 
=============================================================================*/
u8  CAgentInterface::SearchPortChoice(u32 dwIpAddr /* = 0 */)
{
	return g_cCfgParse.SearchPortChoice(dwIpAddr);
}
#endif
//[2011/02/11 zhushz] mcs修改mcu ip
/*=============================================================================
函 数 名： GetNewNetCfg
功    能： 取得新网络配置
算法实现： 
全局变量： 
参    数： 
返 回 值： void 
=============================================================================*/
void CAgentInterface::GetNewNetCfg(TMcuNewNetCfg& tMcuNewNetInfo)
{
	g_cCfgParse.GetNewMpcNetCfg(tMcuNewNetInfo);
	return;
}
/*=============================================================================
函 数 名： GetNewNetCfg
功    能： 设置新网络配置
算法实现： 
全局变量： 
参    数： 
返 回 值： void 
=============================================================================*/
void CAgentInterface::SetNewNetCfg(const TMcuNewNetCfg& tMcuNewNetInfo)
{
	g_cCfgParse.SetNewMpcNetCfg(tMcuNewNetInfo);
	return;
}
/*=============================================================================
函 数 名： GetNewNetCfg
功    能： 是否配置新网络配置
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CAgentInterface::IsMpcNetCfgBeModifedByMcs()
{
	return g_cCfgParse.IsMpcNetCfgBeModifedByMcs();
}
/*=============================================================================
函 数 名： GetNewNetCfg
功    能： 设置是否配置新网络配置标志
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CAgentInterface::SetIsNetCfgBeModifed(BOOL32 bNetInfoBeModify)
{
	return g_cCfgParse.SetIsNetCfgBeModifed(bNetInfoBeModify);
}

/*====================================================================
    函数名        GetMPU2TypeByVmpEqpId
    功能        ：根据vmp外设Id获取所属MPU2板的类型
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byVmpEqpId vmp外设ID
    返回值说明  ：u8 单板类型.不存在返回BRD_TYPE_UNKNOW.
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	11/12/12    4.7         陈代伟           创建
====================================================================*/
u8 CAgentInterface::GetMPU2TypeByVmpEqpId(u8 byVmpEqpId)
{
	u8 byBrdType = BRD_TYPE_UNKNOW;
	if(byVmpEqpId <VMPID_MIN || byVmpEqpId > VMPID_MAX)
	{
		Agtlog(LOG_ERROR, "[GetMPU2TypeByVmpEqpId] unexpected Eqp ID!\n");
		
		return byBrdType;
	}

	TEqpVMPInfo tVmpInfo;
	if(GetEqpVMPCfg(byVmpEqpId,&tVmpInfo) == SUCCESS_AGENT)
	{
		TBoardInfo tBrdInfo;
		g_cCfgParse.GetBrdCfgById(tVmpInfo.GetRunBrdId(),&tBrdInfo);

		byBrdType = tBrdInfo.GetType();
	}
	else
	{
		Agtlog(LOG_ERROR, "[GetMPU2TypeByVmpEqpId]can not find byId(%d) in all VmpTable!\n", byVmpEqpId );
	}

	return byBrdType;
}

/*====================================================================
    函数名        GetMPU2TypeByVmpEqpId
    功能        ：根据bas外设Id获取所属MPU2板的类型
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byVmpEqpId vmp外设ID
    返回值说明  ：u8 单板类型.不存在返回BRD_TYPE_UNKNOW.
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	2012/11/08   4.7         倪志俊           创建
====================================================================*/
u8 CAgentInterface::GetMPU2TypeByEqpId(u8 byEqpId,u8 &byBrdId)
{
	u8 byBrdType = BRD_TYPE_UNKNOW;
	TBoardInfo tBrdInfo;
	if(byEqpId >= BASID_MIN && byEqpId <= BASID_MAX)
	{	
		TEqpMpuBasInfo tBasInfo;
		if(GetMpuBasBrdInfo(byEqpId,&tBasInfo) == SUCCESS_AGENT)
		{			
			g_cCfgParse.GetBrdCfgById(tBasInfo.GetRunBrdId(),&tBrdInfo);

			byBrdType = tBrdInfo.GetType();
			byBrdId = tBasInfo.GetRunBrdId();
		}
		return byBrdType;
	}

	if(byEqpId >= VMPID_MIN && byEqpId <= VMPID_MAX)
	{	
		TEqpSvmpInfo tVmpInfo;
		if(GetEqpSvmpCfgById(byEqpId,&tVmpInfo) == SUCCESS_AGENT)
		{			
			g_cCfgParse.GetBrdCfgById(tVmpInfo.GetRunBrdId(),&tBrdInfo);
			byBrdType = tBrdInfo.GetType();
			byBrdId = tVmpInfo.GetRunBrdId();
		}
		return byBrdType;
	}	

	Agtlog(LOG_ERROR, "[GetMPU2TypeByBasEqpId] unexpected Eqp ID!\n");
	return byBrdType;
}

/*====================================================================
    函数名        IsLowMpu2EqpId
    功能        ：MPU2 ECARD BASIC模式获得另外一个EQPID
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byVmpEqpId vmp外设ID
    返回值说明  ：u8 单板类型.不存在返回BRD_TYPE_UNKNOW.
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	2012/11/08   4.7         倪志俊           创建
====================================================================*/
BOOL32 CAgentInterface::GetMpu2AnotherEqpId(u8 byEqpId,u8 &byAnotherId)
{
	//校验是否Ecard板
	u8 byBrdId =0;
	if ( BRD_TYPE_MPU2ECARD != GetMPU2TypeByEqpId(byEqpId,byBrdId) )
	{
		return FALSE;
	}

	//获得单板信息
	TBoardInfo tBrdInfo;
    u16 wRet = GetBrdCfgById( byBrdId, &tBrdInfo );

	//根据单板信息判断是否是ECARD板卡配置了两快basic外设

	if ( wRet == SUCCESS_AGENT
		)
	{
		u8 byPeriCount = 0;
		u8 abyPeriId[MAXNUM_BRD_EQP] = {0};
		tBrdInfo.GetPeriId( abyPeriId, &byPeriCount );
		if ( byPeriCount == 2  )
		{
			u8 byMinEqpId = min(abyPeriId[0],abyPeriId[1]);
			if ( byEqpId == byMinEqpId )
			{
				byAnotherId = max(abyPeriId[0],abyPeriId[1]);	//返回高的eqpid	
			}
			else
			{
				byAnotherId =byMinEqpId;
			}
			return TRUE;
		}	
	}

	return FALSE;
}

/*====================================================================
    函数名        GetAPU2BasEqpIdByMixerId
    功能        ：apu2 通过混音器外设id，获得另外一个音频适配器外设EQPID
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMixerEqpId apu2单板上混音器的外设id
    返回值说明  ：u8 &byBasId 同一APU2单板上另外apu2音频适配外设id
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	2013/05/14   4.7         陈代伟           创建
====================================================================*/
BOOL32 CAgentInterface::GetAPU2BasEqpIdByMixerId(u8 byMixerEqpId,u8 &byBasId)
{
	if(byMixerEqpId >= MIXERID_MIN && byMixerEqpId <= MIXERID_MAX)
	{	
		u8 byBrdId =0;
		TEqpMixerInfo tMixerInfo;
		TBoardInfo tBrdInfo;
		if(GetEqpMixerCfg(byMixerEqpId,&tMixerInfo) == SUCCESS_AGENT)
		{			
			g_cCfgParse.GetBrdCfgById(tMixerInfo.GetRunBrdId(),&tBrdInfo);
			
			byBrdId = tMixerInfo.GetRunBrdId();
			
			u8 byPeriCount = 0;
			u8 abyPeriId[MAXNUM_BRD_EQP] = {0};
			tBrdInfo.GetPeriId( abyPeriId, &byPeriCount );
			if(tBrdInfo.GetType() == BRD_TYPE_APU2)
			{
				for( u8 byIdx = 0; byIdx < byPeriCount; byIdx++)
				{
					if(abyPeriId[byIdx] >= BASID_MIN && abyPeriId[byIdx]<= BASID_MAX)
					{
						byBasId = abyPeriId[byIdx];
						
						return TRUE;
					}
				}
			}
		}
	}
	
	return FALSE;
}


/*====================================================================
    函数名        GetPeriEqpMcuRecvStartPort
    功能        ：为特定单板特定外设分配的MCU侧接收起始端口
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byEqpType 外设类型
				  u8 bySubEqpType 外设子类型（比如MPU2板的basic vmp类型）
    返回值说明  ：u16 MCU侧接收起始端口，获取失败返回0
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	11/12/12    4.7         陈代伟           创建
====================================================================*/
u16 CAgentInterface::GetPeriEqpMcuRecvStartPort(u8 byEqpType,u8 bySubEqpType)
{
	u16 wRecvStartPort = 0;

	switch(byEqpType)
	{
	case EQP_TYPE_MIXER:
		if(bySubEqpType == APU_MIXER || bySubEqpType == EAPU_MIXER || bySubEqpType == MIXER_8KH || bySubEqpType == MIXER_8KG )
		{
			wRecvStartPort = m_wMixerMcuRecvStartPort;
			m_wMixerMcuRecvStartPort += MIXER_MCU_PORTSPAN;
		}
		else if (bySubEqpType == APU2_MIXER || bySubEqpType == MIXER_8KI )
		{
			wRecvStartPort = m_wMixerMcuRecvStartPort;
			m_wMixerMcuRecvStartPort += APU2_MIXER_MCU_PORTSPAN;
		}

		break;
	case EQP_TYPE_RECORDER:
		wRecvStartPort = m_wRecMcuRecvStartPort;
		m_wRecMcuRecvStartPort += REC_MCU_PORTSPAN;

		break;

	case EQP_TYPE_BAS:
		wRecvStartPort = m_wBasMcuRecvStartPort;

		if(TYPE_MPU2BAS_BASIC == bySubEqpType )
		{
			m_wBasMcuRecvStartPort += BAS_BASIC_PORTSPAN;
		}
		else if(TYPE_MPU2BAS_ENHANCED == bySubEqpType)
		{
			m_wBasMcuRecvStartPort += BAS_ENHANCED_PORTSPAN;
		}
		else if (TYPE_APU2BAS == bySubEqpType )
		{
			m_wBasMcuRecvStartPort+=BAS_APU2_PORTSPAN;
		}
		else
		{
			m_wBasMcuRecvStartPort += BAS_MCU_PORTSPAN;
		}
		
		break;

	case EQP_TYPE_VMP:
		wRecvStartPort = m_wVmpMcuRecvStartPort;
		if(TYPE_MPU2VMP_BASIC == bySubEqpType || TYPE_MPU2VMP_ENHANCED == bySubEqpType)
		{
			m_wVmpMcuRecvStartPort += VMP_MPU2_PORTSPAN;
		}
		else
		{
			m_wVmpMcuRecvStartPort += VMP_MCU_PORTSPAN;
		}

		break;

	default:
		Agtlog(LOG_WARN, "[GetPeriEqpMcuRecvStartPort] unexpected EqpType!\n" );

		break;
	}

	return wRecvStartPort;
}

/*====================================================================
    函数名        ClearAllPeriEqpMcuRecvPort
    功能		  初始化外设MCU侧起始端口值
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	11/12/12    4.7         陈代伟           创建
====================================================================*/
void CAgentInterface::ClearAllPeriEqpMcuRecvPort( void )
{
	//初始化外设MCU侧起始端口 [12/13/2011 chendaiwei]
	m_wMixerMcuRecvStartPort = MIXER_MCU_STARTPORT;
	m_wRecMcuRecvStartPort = REC_MCU_STARTPORT;
	m_wBasMcuRecvStartPort = BAS_MCU_STARTPORT;
	m_wVmpMcuRecvStartPort = VMP_MCU_STARTPORT;
}


/*=============================================================================
  函 数 名： SetMpcUpateStatus
  功    能： 设置mpc的升级标志位
  算法实现： 
  全局变量： 
  参    数： s32:0表示已经升级，1表示未升级
  返 回 值：  
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	12/02/08    4.7         zhouyiliang           创建
====================================================================*/
void  CAgentInterface::SetMpcUpateStatus(s32 dwStat /*= 1*/ )
{
	 g_cCfgParse.SetMpcUpateStatus(dwStat);
}

/*====================================================================
    函数名        GetMixerSubTypeByEqpId
    功能		  通过外设ID获取混音器的子类型
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	12/02/13    4.7         陈代伟           创建
====================================================================*/
BOOL32 CAgentInterface::GetMixerSubTypeByEqpId(u8 byEqpId, u8 & byMixertype)
{
	if (byEqpId < MIXERID_MIN || byEqpId > MIXERID_MAX)
	{
		return FALSE;
	}
	byMixertype = UNKONW_MIXER;
	BOOL32 bResult = TRUE;

#ifdef _8KE_
	byMixertype = MIXER_8KG;
#elif defined(_8KH_)
	byMixertype = MIXER_8KH;
#elif defined(_8KI_)
	byMixertype = MIXER_8KI;
#elif defined(_MINIMCU_)
	byMixertype = APU_MIXER;
#else
	TEqpMixerInfo atMixInfo;
	if(SUCCESS_AGENT != g_cCfgParse.GetEqpMixerCfgById(byEqpId,&atMixInfo))
	{
		bResult = FALSE;
		Agtlog(LOG_ERROR, "[GetMixerSubTypeByEqpId]Failed to get Mixer Subtype!\n");
	}
	else
	{
		bResult = GetMixerSubTypeByRunBrdId(atMixInfo.GetRunBrdId(),byMixertype);
	}
#endif

	return bResult;
}

/*====================================================================
    函数名        GetHduSubTypeByEqpId
    功能		  通过外设ID获取HDU的子类型
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	12/02/13    4.7         陈代伟           创建
====================================================================*/
BOOL32 CAgentInterface::GetHduSubTypeByEqpId(u8 byEqpId, u8 & byHduSubtype)
{
	if (byEqpId < HDUID_MIN || byEqpId > HDUID_MAX)
	{
		return FALSE;
	}
	byHduSubtype = 0;
	BOOL32 bResult = TRUE;

	TEqpHduInfo atHduInfo;
	if(SUCCESS_AGENT != g_cCfgParse.GetEqpHduCfgById(byEqpId,&atHduInfo))
	{
		bResult = FALSE;
		Agtlog(LOG_ERROR, "[GetHduSubTypeByEqpId]Failed to get HDU Subtype!\n");
	}
	else
	{
		byHduSubtype = atHduInfo.GetStartMode();//START_MODE与HDU外设子类型一一对应，不需再做转换
	}

	return bResult;
}

/*====================================================================
    函数名        GetMixerSubTypeByRunBrdId
    功能		  通过单板ID获取单板上的混音器的子类型
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	12/02/13    4.7         陈代伟           创建
====================================================================*/
/*lint -save -esym(715,byBrdId)*/
BOOL32 CAgentInterface::GetMixerSubTypeByRunBrdId(u8 byBrdId, u8 & byMixertype)
{
	byMixertype = UNKONW_MIXER;
	BOOL32 bResult = TRUE;

#ifdef _8KE_
	byMixertype = MIXER_8KG;
#elif defined(_8KH_)
	byMixertype = MIXER_8KH;
#elif defined(_8KI_)
	byMixertype = MIXER_8KI;
#else

	TEqpBrdCfgEntry tBrdCfg;
	g_cCfgParse.GetBrdInfoById(byBrdId,&tBrdCfg);
	
	switch(tBrdCfg.GetType())
	{
	case BRD_TYPE_APU2:
		byMixertype = APU2_MIXER;
		break;

	case BRD_TYPE_APU:
		byMixertype = APU_MIXER;
		break;

	case BRD_TYPE_EAPU:
		byMixertype = EAPU_MIXER;
		break;

	default:
		byMixertype = UNKONW_MIXER;
		bResult = FALSE;
		break;
	}
#endif

	return bResult;
}

/*====================================================================
    函数名        IsNeedRebootAllMpuBas
    功能		  是否需要重启所有的MPU（BAS）单板
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：BOOL32 TRUE表示需要重启，FALS表示不需要
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	12/02/13    4.7         陈代伟           创建
====================================================================*/
BOOL32 CAgentInterface::IsNeedRebootAllMpuBas( void )
{
	return (1 == m_byIsNeedRebootAllMPUBas);
}

/*====================================================================
    函数名        SetRebootAllMpuBasFlag
    功能		  设置重启所有MPU（BAs）标志位
    算法实现    ：
    引用全局变量：
    输入参数说明：BOOL32 bNeedReboot True表示需要重启，False表示不需要重启
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	12/02/13    4.7         陈代伟           创建
====================================================================*/
void CAgentInterface::SetRebootAllMpuBasFlag( BOOL32 bNeedReboot)
{
	if(bNeedReboot)
	{
		m_byIsNeedRebootAllMPUBas = 1;
	}
	else
	{
		m_byIsNeedRebootAllMPUBas = 0;
	}
}

/*====================================================================
    函数名        GetBrdIdbyIpAddr
    功能		  通过IP获取单板索引
    算法实现    ：
    引用全局变量：
    输入参数说明：u32 IP 主机序
    返回值说明  ：u8 单板ID
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	12/02/13    4.7         陈代伟           创建
====================================================================*/
u8 CAgentInterface::GetBrdIdbyIpAddr(u32 dwIpAddr)
{
	return g_cCfgParse.GetBrdIdbyIpAddr(dwIpAddr);
}

/*====================================================================
    函数名        GetCurEthChoice
    功能		  获取当前启用网口
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：u8 网口idx
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	12/02/13    4.7         陈代伟           创建
====================================================================*/
u8 CAgentInterface::GetCurEthChoice ( void )
{
	TMPCInfo tMpcInfo;
	g_cCfgParse.GetMPCInfo(&tMpcInfo);

	return tMpcInfo.GetLocalPortChoice();
}


#ifdef _8KH_
/*====================================================================
    函数名        SetMcuType800L
    功能		  标记本MCU为800L类型（方便和8000I区分）  
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	12/06/18    4.7         陈代伟           创建
====================================================================*/
void CAgentInterface::SetMcuType800L( void )
{
	g_cCfgParse.SetMcuType800L();
}

/*====================================================================
    函数名        SetMcuType8000Hm
    功能		  标记本MCU为8000H-M类型  
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	12/06/18    4.7         陈代伟           创建
====================================================================*/
void CAgentInterface::SetMcuType8000Hm( void )
{
	g_cCfgParse.SetMcuType8000Hm();
}

/*====================================================================
    函数名        Is800LMcu
    功能		  判断是否是800L型号的MCU  
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：BOOL32 是返回TRUE 否则返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	12/06/18    4.7         陈代伟           创建
====================================================================*/
BOOL32 CAgentInterface::Is800LMcu( void )
{
	return g_cCfgParse.Is800LMcu();
}

/*====================================================================
    函数名        Is8000HMMcu
    功能		  判断是否是8000H-M型号的MCU  
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：BOOL32 是返回TRUE 否则返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	12/06/18    4.7         陈代伟           创建
====================================================================*/
BOOL32 CAgentInterface::Is8000HmMcu( void )
{
	return g_cCfgParse.Is8000HmMcu();
}

#endif //defined 8KH

/*====================================================================
    函数名        AdjustMcuRasGKPort
    功能		  调整MCU的RasPort（分MpcIP和GKIP相等或者不相等
				  两种情况） 以及设置GK和GKC通信的默认端口为60001
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	12/06/18    4.7         陈代伟           创建
====================================================================*/
void CAgentInterface::AdjustMcuRasPort(u32 dwMpcIp, u32 dwGkIp)
{
	LogPrint(LOG_LVL_DETAIL,MID_MCU_CFG,"[AdjustMcuRasPort] Gk IP: 0x%x,Mcu IP: 0x%x\n",dwGkIp,dwMpcIp);

	s8 achFName[KDV_MAX_PATH] = {0};
	sprintf(achFName, "%s/%s", DIR_CONFIG, FILE_MTADPDEBUG_INI);
	
	FILE *hFile = fopen(achFName,"a");
	{
		if( NULL != hFile )
		{
			fclose(hFile);
		}
		else
		{
			LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustMcuRasPort] open mtadpdebug.ini failed! could not set correct Ras Port.\n");
		}
	}
	
	s32 dwRasPort = 0;
	s32 dwq931Port = 0;

	//内置GK IP与MCU IP相同的情况下，未避免IP冲突，将mcu内置接入的RASport
	//和Q931 port置为1919和1920[6/19/2012 chendaiwei]
	if( dwMpcIp == dwGkIp )
	{
		dwRasPort = 1919;
		dwq931Port = 1920;
	}
	else
	{
		dwRasPort = 1719;
		dwq931Port = 1720;
	}
	
	BOOL32 bResult = ::SetRegKeyInt( achFName,"calldata", "rasport",dwRasPort);
	if(!bResult)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustMcuRasPort] SetRegKeyInt [RasPort:%d] failed.\n",dwRasPort);	
	}
	
	bResult = ::SetRegKeyInt( achFName, "calldata", "q931port", dwq931Port);
	if(!bResult)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustMcuRasPort] SetRegKeyInt [q931port:%d] failed.\n",dwq931Port);
	}

	memset(achFName,0,sizeof(achFName));
	sprintf(achFName, "%s/%s", DIR_CONFIG, GKCFG_INI_FILENAME);
	FILE *hGkFile = fopen(achFName,"a");
	{
		if( NULL != hGkFile )
		{
			fclose(hGkFile);
		}
		else
		{
			LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustMcuRasPort] open gkconfig.ini failed! could not set Gk Port 60001.\n");
		}
	}

	bResult = ::SetRegKeyInt( achFName,"PortInfo", "PortAddTimes",10);
	if(!bResult)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustMcuRasPort] SetRegKeyInt [PortAddTimes:10] failed.\n");	
	}

	bResult = ::SetRegKeyInt( achFName,"PortInfo", "Port_GK",60001);
	if(!bResult)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustMcuRasPort] SetRegKeyInt [Port_GK:60001] failed.\n");	
	}

	return;
}

/*====================================================================
    函数名        AdjustNewMcuRasPort
    功能		  调整MCU的RasPort（分MpcIP和GKIP、sipip相等或者不相等
				  两种情况） 以及设置GK和GKC通信的默认端口为60001
    算法实现    ：
    引用全局变量：
    输入参数说明：u32 dwMcuIp, u32 dwGKIp, u32 dwSipIp
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	20130929    8KI         闫日亮           创建
====================================================================*/
void CAgentInterface::AdjustNewMcuRasPort(u32 dwMpcIp, u32 dwGKIp, u32 dwSipIp)
{
	s8 achFName[KDV_MAX_PATH] = {0};
	sprintf(achFName, "%s/%s", DIR_CONFIG, FILE_MTADPDEBUG_INI);
	
	FILE *hFile = fopen(achFName,"a");
	{
		if( NULL != hFile )
		{
			fclose(hFile);
		}
		else
		{
			LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustNewMcuRasPort] open mtadpdebug.ini failed! could not set correct Ras Port.\n");
		}
	}
	
	s32 dwRasPort = 1719;
	s32 dwq931Port = 1720;

	//内置GK IP与MCU IP相同或Sip Ip与MCU IP相同的情况下，为避免IP冲突，将mcu内置接入的RASport
	//和Q931 port置为1919和1920
	if( dwMpcIp == dwGKIp || dwMpcIp == dwSipIp)
	{
		dwRasPort = 1919;
		dwq931Port = 1920;
	}
	
	BOOL32 bResult = ::SetRegKeyInt( achFName,"calldata", "rasport",dwRasPort);
	if(!bResult)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustNewMcuRasPort] SetRegKeyInt [RasPort:%d] failed.\n",dwRasPort);	
	}
	
	bResult = ::SetRegKeyInt( achFName, "calldata", "q931port", dwq931Port);
	if(!bResult)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustNewMcuRasPort] SetRegKeyInt [q931port:%d] failed.\n",dwq931Port);
	}

	s32 dwSipPort = 1720;
	//内置GK IP与Sip Ip相同的情况下，为避免IP冲突，将Sip port置为1825
	if(dwGKIp == dwSipIp)
	{
		dwSipPort = 1825;
	}

	#ifdef WIN32
	#define DIR_CONFIG_NEW               ( LPCSTR )"./conf"	
	#else
	#define DIR_CONFIG_NEW               ( LPCSTR )"/usr/conf"	
	#endif

	memset(achFName,0,sizeof(achFName));
	sprintf(achFName, "%s/%s", DIR_CONFIG_NEW, "iwCfg.ini");
	
	FILE *hOpen = NULL;
    if(NULL == (hOpen = fopen(achFName, "r")))	// no exist
	{	
		g_cCfgParse.AgentDirCreate(DIR_CONFIG_NEW);
		hOpen = fopen(achFName, "w");			// create configure file
	}

	if(NULL != hOpen)
	{
		fclose(hOpen);
		hOpen = NULL;
	}

	bResult = ::SetRegKeyInt( achFName, "InterworkingSrvInfo", "H225Port", dwSipPort);
	if(!bResult)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustNewMcuRasPort] SetRegKeyInt [Sipport:%d] failed.\n",dwSipPort);
	}

	//写SipIp地址
	s8	achSipIpAddr[32] = {0};
	GetIpFromU32(achSipIpAddr, htonl(dwSipIp));
	bResult = ::SetRegKeyString( achFName, "InterworkingSrvInfo", "InterworkingIP", achSipIpAddr);
	if(!bResult)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustNewMcuRasPort] SetRegKeyString [InterworkingIP:%s] failed.\n",achSipIpAddr);
	}

	bResult = ::SetRegKeyString( achFName, "SipRegSrvInfo", "IpAddr", achSipIpAddr);
	if(!bResult)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustNewMcuRasPort] SetRegKeyString [IpAddr:%s] failed.\n",achSipIpAddr);
	}

	memset(achFName,0,sizeof(achFName));
	sprintf(achFName, "%s/%s", DIR_CONFIG_NEW, "regprefix.ini");

    if(NULL == (hOpen = fopen(achFName, "r")))	// no exist
	{	
		g_cCfgParse.AgentDirCreate(DIR_CONFIG_NEW);
		hOpen = fopen(achFName, "w");			// create configure file
	}
	
	if(NULL != hOpen)
	{
		fclose(hOpen);
		hOpen = NULL;
	}

	bResult = ::SetRegKeyString( achFName, "RegserverIP", "IP", achSipIpAddr);
	if(!bResult)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustNewMcuRasPort] SetRegKeyString [IP:%s] failed.\n",achSipIpAddr);
	}

	memset(achFName,0,sizeof(achFName));
	sprintf(achFName, "%s/%s", DIR_CONFIG, GKCFG_INI_FILENAME);
	FILE *hGkFile = fopen(achFName,"a");
	{
		if( NULL != hGkFile )
		{
			fclose(hGkFile);
		}
		else
		{
			LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustNewMcuRasPort] open gkconfig.ini failed! could not set Gk Port 60001.\n");
		}
	}

	bResult = ::SetRegKeyInt( achFName,"PortInfo", "PortAddTimes",10);
	if(!bResult)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustNewMcuRasPort] SetRegKeyInt [PortAddTimes:10] failed.\n");	
	}

	bResult = ::SetRegKeyInt( achFName,"PortInfo", "Port_GK",60001);
	if(!bResult)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"[AdjustNewMcuRasPort] SetRegKeyInt [Port_GK:60001] failed.\n");	
	}

	return;
}
/*====================================================================
    函数名        RefreshBrdOsType
    功能		  更新某个外设板卡的OSTYPE值
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：u8 byBrdId, u8 byOsType
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	12/08/23    4.7         田志勇          创建
====================================================================*/
BOOL32 CAgentInterface::RefreshBrdOsType(u8 byBrdId, u8 byOsType)
{
	return g_cCfgParse.RefreshBrdOsType(byBrdId, byOsType);
}

/*====================================================================
    函数名        GetBrdCfgById
    功能		  获取单板信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	12/10/23    4.7         chendaiwei    创建
====================================================================*/
u16 CAgentInterface::GetBrdCfgById(u8 byBrdId, TBoardInfo* ptBoardInfo)
{
	return g_cCfgParse.GetBrdCfgById(byBrdId,ptBoardInfo);
}
// END OF FILE
