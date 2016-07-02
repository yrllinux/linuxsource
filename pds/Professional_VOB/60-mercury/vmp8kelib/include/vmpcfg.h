/*****************************************************************************
   模块名      : 画面复合器(Video MultiPlexer)
   文件名      : VMPCfg.h
   创建时间    : 2003年 12月 4日
   实现功能    : 画面复合器参数配置
   作者        : zhangsh
   版本        : 
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
******************************************************************************/
#ifndef _VIDEO_MULTIPLEXER_CONFIG_H_
#define _VIDEO_MULTIPLEXER_CONFIG_H_

#include "kdvtype.h"
#include "osp.h"
#include "kdvsys.h"
#include "mcuconst.h"
#include "mcustruct.h"
#include "mmpcommon.h"
#if !defined(_8KH_) && !defined(_8KE_)
#include "multpic_gpu.h"
#else
#include "multpic.h"
#endif



#define	MIN_BITRATE_OUTPUT      (u16)128

#define MAXNUM_VMPDEFINE_PIC   MAXNUM_SDVMP_MEMBER
struct TVmpBitrateDebugVal
{
protected:
    BOOL32          m_bEnableBitrateCheat;         // 是否启用码率输出作弊
    u16             m_wOverDealRate;               // 平滑允许上突的百分比
public:
    TVmpBitrateDebugVal():m_bEnableBitrateCheat(0),
                          m_wOverDealRate(0){}
public:
    void   SetEnableBitrateCheat(BOOL32 bEnable)
    {
        m_bEnableBitrateCheat = bEnable;
    }
    BOOL32 IsEnableBitrateCheat() const
    {
        return m_bEnableBitrateCheat;
    }
    void   SetOverDealRate(u16 wPctRate)
    {
        m_wOverDealRate = wPctRate;
    }
    u16    GetOverDealRate(void) const
    {
        return m_wOverDealRate;
    }
};

// 画面合成成员别名 [7/3/2013 liaokang]
struct TVmpMbAlias
{
public:  
    BOOL32  SetMbAlias( u8 byAliasLen, const s8 *pAlias ) 
    { 
        if( NULL == pAlias )
        {
            return FALSE;
        }
        memset(m_achMbAlias, 0, sizeof(m_achMbAlias));
        memcpy(m_achMbAlias, pAlias, min(byAliasLen, MAXLEN_ALIAS-1));
        m_achMbAlias[MAXLEN_ALIAS-1] = '\0';
        return TRUE; 
    }
    const s8* GetMbAlias(void) { return m_achMbAlias; }
    
protected:
    s8   m_achMbAlias[MAXLEN_ALIAS];
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TVmpMemPicRoute
{
private:
	s8 m_szVmpMemAlias[/*VALIDLEN_ALIAS+1*/MAXLEN_ALIAS]; // 扩容 [7/3/2013 liaokang]
	s8 m_szVmpMemPicRoute[255];
public:
	TVmpMemPicRoute()
	{
		memset(m_szVmpMemAlias, 0, sizeof(m_szVmpMemAlias));
		memset(m_szVmpMemPicRoute, 0, sizeof(m_szVmpMemPicRoute));
	}	
	
	void SetRoute(const s8* pszRoute)
    {
        memset( m_szVmpMemPicRoute, 0, sizeof(m_szVmpMemPicRoute));
        strncpy( m_szVmpMemPicRoute, pszRoute,  sizeof(m_szVmpMemPicRoute));
		m_szVmpMemPicRoute[sizeof(m_szVmpMemPicRoute)-1] = '\0';
    }
	
	const s8* GetRoute() const
    {
        return m_szVmpMemPicRoute;
    }
	
	void SetVmpMemAlias(const s8* pszMemAlias)
    {
        memset( m_szVmpMemAlias, 0, sizeof(m_szVmpMemAlias));
        strncpy( m_szVmpMemAlias, pszMemAlias,  sizeof(m_szVmpMemAlias));
		m_szVmpMemAlias[sizeof(m_szVmpMemAlias)-1] = '\0';
    }
	
	const s8* GetVmpMemAlias() const
    {
        return m_szVmpMemAlias;
    }
};

struct TDebugVal
{
protected:
	u32				m_dwVidResizeMode;	    //模式0:加黑边，1:裁边，2:非等比拉伸，默认0
    u32             m_dwVidEachResizeMode;   // 针对合成风格中各个小图像进行：加 0黑边/ 1裁边/ 2非等比拉升（全屏）
	TVmpMemPicRoute	m_atVmpMemPic[MAXNUM_VMPDEFINE_PIC];
public:
   

	void	SetVidResizeMode(u32 dwMode)
	{
		m_dwVidResizeMode = htonl(dwMode);
	}
	
	u32		GetVidResizeMode(void)
	{
		return ntohl(m_dwVidResizeMode);
	}

	void	SetVidEachResizeMode(u32 dwMode)
	{
		m_dwVidEachResizeMode = htonl(dwMode);
	}
	
	u32		GetVidEachResizeMode(void)
	{
		return ntohl(m_dwVidEachResizeMode);
	}

	/*=============================================================================
	函 数 名： TableMemoryFree
	功    能： 释放指定表的内存
	算法实现： 
	全局变量： 
	参    数：  void **ppMem
	u32 dwEntryNum
	返 回 值： BOOL32 
	=============================================================================*/
	BOOL32 TableMemoryFree( void **ppMem, u32 dwEntryNum )
	{
		if( NULL == ppMem)
		{
			return FALSE;
		}
		for( u32 dwLoop = 0; dwLoop < dwEntryNum; dwLoop++ )
		{
			if( NULL != ppMem[dwLoop] )
			{
				delete [] (s8*)ppMem[dwLoop];
				ppMem[dwLoop] = NULL;
			}
		}
		delete [] (s8*)ppMem;
		ppMem = NULL;
		return TRUE;
	}

	void ReadDebugValues()
	{
		s8 CFG_FILE[MAX_PATH] = {0};
		s8  SECTION_VMPDEBUG[]    = "EQPDEBUG";
		s8 SECTION_VMPDEFINE_PIC[] = "VMPDEFINEPIC";
		sprintf(CFG_FILE, "%s/mcueqp.ini", DIR_CONFIG);
		s32 nValue;

		// 是否加黑边或裁边,默认是0
		::GetRegKeyInt( CFG_FILE, SECTION_VMPDEBUG, "VidSDMode", 0, &nValue );
		if( nValue != 0 && nValue != 1 && nValue != 2)
		{
			nValue = 0;
		}
		SetVidResizeMode((u32)nValue);
		
		// 针对合成风格中各个小图像进行：加 0黑边/ 1裁边/ 2非等比拉升（全屏）
		::GetRegKeyInt( CFG_FILE, SECTION_VMPDEBUG, "VidEachResizeMode", 2, &nValue );
		if( nValue != 0 && nValue != 1 && nValue != 2)
		{
			nValue = 2;
		}
		SetVidEachResizeMode((u32)nValue);
		s32 nMemEntryNum = 0;
		::GetRegKeyInt( CFG_FILE, SECTION_VMPDEFINE_PIC, "EntryNum", 0, &nMemEntryNum );
		if (0 == nMemEntryNum)
		{
			return;
		}
		
		// alloc memory
		s8** ppszTable = NULL;        
		ppszTable = new s8*[nMemEntryNum];
		if( NULL == ppszTable )
		{
			OspPrintf(1, 0,  "[AgentGetBrdCfgTable] Fail to malloc memory \n");
			return;
		}
		
		u32 dwEntryNum = nMemEntryNum;
		u32 dwLoop = 0;
		for( dwLoop = 0; dwLoop < dwEntryNum; dwLoop++ )
		{
			ppszTable[dwLoop] = new s8[MAX_VALUE_LEN+1];
			if(NULL == ppszTable[dwLoop])
			{
				//释放内存
				TableMemoryFree( (void**)ppszTable, nMemEntryNum );
				return;
			}
		}
		u32 dwReadEntryNum = dwEntryNum;
		GetRegKeyStringTable( CFG_FILE,    
			SECTION_VMPDEFINE_PIC,      
			"fail",     
			ppszTable, 
			&dwReadEntryNum, 
			MAX_VALUE_LEN + 1);
		
		if (dwReadEntryNum != dwEntryNum)
		{
			//释放内存
			TableMemoryFree( (void**)ppszTable, nMemEntryNum );
			return;
		}
		
		s8    achSeps[] = "\t";        // 分隔符
		s8    *pchToken = NULL;
		
		for( dwLoop = 0; dwLoop < dwEntryNum; dwLoop++)
		{
			if (dwLoop + 1 == MAXNUM_VMPDEFINE_PIC)
			{
				break;
			}
			//忽略
			pchToken = strtok( ppszTable[dwLoop], achSeps );
			if (NULL == pchToken)
			{
				continue;
			}
			
			//Vmp成员别名
			pchToken = strtok( NULL, achSeps );
			if (NULL == pchToken)
			{
				continue;
			}
			SetVmpMemAlias(dwLoop, pchToken);
			
			//Vmp成员所要显示的图片
			pchToken = strtok( NULL, achSeps );
			if (NULL == pchToken)
			{
				continue;
			}
			SetRoute(dwLoop, pchToken);
		}
		
		TableMemoryFree( (void**)ppszTable, nMemEntryNum );
		
		return;
	}

	void SetRoute(u8 byIndex, const s8* pszRoute)
    {
		if (byIndex >= MAXNUM_VMPDEFINE_PIC || NULL == pszRoute)
		{
			return;
		}
        m_atVmpMemPic[byIndex].SetRoute(pszRoute);
    }
	
	const s8* GetVmpMemRoute(u8 byIndex) const
    {
        return m_atVmpMemPic[byIndex].GetRoute();
    }
	
	void SetVmpMemAlias(u8 byIndex, const s8* pszMemAlias)
    {
		if (byIndex >= MAXNUM_VMPDEFINE_PIC || NULL == pszMemAlias)
		{
			return;
		}
		m_atVmpMemPic[byIndex].SetVmpMemAlias(pszMemAlias);
    }
	
	const s8* GetVmpMemAlias(u8 byIndex) const
    {
        return m_atVmpMemPic[byIndex].GetVmpMemAlias();
    }

	void Print()
	{
		OspPrintf(TRUE,FALSE,"GetVidResizeMode: %d\n",GetVidResizeMode());
		OspPrintf(TRUE,FALSE,"GetVidEachResizeMode: %d\n",GetVidEachResizeMode());
		OspPrintf(TRUE,  FALSE, "VmpPic as follows:\n");
		for (u8 byIndex = 0; byIndex < MAXNUM_VMPDEFINE_PIC; byIndex++)
		{
			OspPrintf(TRUE, FALSE, "Index.%d -> Alias.%s -> Route.%s\n", 
				byIndex,
				m_atVmpMemPic[byIndex].GetVmpMemAlias(),
				m_atVmpMemPic[byIndex].GetRoute());
		}
	}

};

class CCPParam
{      
public:
    //保存当前工作状态
    C8KEVMPParam m_tStatus;

    //TVmpBitrateDebugVal   m_tDebugVal;                // 将码率作弊值
public:
    CCPParam();
	~CCPParam();
    void Clear();
	
	void GetDefaultParam(u8 byEncType,TVidEncParam& TEncParam);
};

#endif //_BITRATE_ADAPTER_SERVER_CONFIG_H_

