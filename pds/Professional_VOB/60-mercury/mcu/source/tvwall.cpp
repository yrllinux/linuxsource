/*****************************************************************************
   模块名      : tvwall
   文件名      : tvwall.cpp
   相关文件    : tvwall.h
   文件实现功能: 实现了tvwall(包括hdu等tvwall设备)的基本功能
   作者        : 刘旭
   版本        : V0.9  Copyright(C) 2001-2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2011/06/01  4.6         刘旭	       创建
******************************************************************************/

#include "tvwall.h"

#include "mcuvc.h"
extern  CMcuVcApp	        g_cMcuVcApp;	//MCU业务应用实例
extern CAgentInterface		g_cMcuAgent;   

/*====================================================================
    函数名      ：IsValidHduEqp
    功能        ：判断tEqp是否是合法的Hdu设备
    算法实现    ：根据主类型,辅类型以及EqpId进行判断
    引用全局变量：
    输入参数说明：[in]tEqp
    返回值说明  ：TRUE, 合法; FALSE, 不合法
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	11/05/26    4.6          刘旭          创建
====================================================================*/
BOOL32 IsValidHduEqp(const TEqp& tEqp) 
{
	// 主类型判断
	if (TYPE_MCUPERI != tEqp.GetType())
	{
		return FALSE;
	}

	// Hdu类型判断
	/*if (EQP_TYPE_HDU != tEqp.GetEqpType()
			&& EQP_TYPE_HDU_H != tEqp.GetEqpType() 
			&& EQP_TYPE_HDU_L != tEqp.GetEqpType()
			&& EQP_TYPE_HDU2 != tEqp.GetEqpType()
			&& EQP_TYPE_HDU2_L != tEqp.GetEqpType())
	{
		return FALSE;
	}*/

	// Hdu的Id判断
	if(HDUID_MIN > tEqp.GetEqpId() || HDUID_MAX < tEqp.GetEqpId())
	{
		return FALSE;
	}
	
	return TRUE;
}

/*====================================================================
    函数名      ：IsValidHduMode
    功能        ：判断Hdu模式是否合法
    算法实现    ：根据主类型,辅类型以及EqpId进行判断
    引用全局变量：
    输入参数说明：[in]tEqp
    返回值说明  ：TRUE, 合法; FALSE, 不合法
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	2013/03/11   4.7.2       陈兵         修改(HDU多画面支持)
====================================================================*/
BOOL32 IsValidHduMode(const u8 byHduChnMode) 
{
	// 四风格判断
	if ( HDUCHN_MODE_FOUR == byHduChnMode)
	{
		return TRUE;
	}
	else if ( HDUCHN_MODE_ONE == byHduChnMode)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*====================================================================
    函数名      IsValidHduChn
    功能        ：判断byHduId是否是合法的Hdu设备的Id
    算法实现    ：
    引用全局变量：
    输入参数说明：[in]byHduId, hdu设备Id
				  [in]byChnnlIdx, hdu通道
    返回值说明  ：TRUE, 合法; FALSE, 不合法
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	11/05/26    4.6          刘旭          创建
====================================================================*/
BOOL32 IsValidHduChn(const u8 byHduId, const u8 byChnnlIdx, const u8 bySubChnId) 
{
	TEqp tHduEqp = g_cMcuVcApp.GetEqp( byHduId );
	if (!IsValidHduEqp(tHduEqp))
	{
		return FALSE;
	}

	//hdu 通道索引校验
	u8 byHduChnNum = 0;
	u8 byHduSubType = 0;
	if(g_cMcuAgent.GetHduSubTypeByEqpId(byHduId,byHduSubType))
	{
		switch(byHduSubType)
		{
		case HDU_SUBTYPE_HDU_M:
		case HDU_SUBTYPE_HDU_L:
		case HDU_SUBTYPE_HDU2:
		case HDU_SUBTYPE_HDU2_L:
		case HDU_SUBTYPE_HDU2_S:
			byHduChnNum = MAXNUM_HDU_CHANNEL;
			break;
		case HDU_SUBTYPE_HDU_H:
			byHduChnNum = MAXNUM_HDU_H_CHANNEL;
			break;
		default:
			byHduChnNum = 0;
			LogPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[GetHduChnNumAcd2Type] unexpected hdu subtype :%d!\n", byHduSubType);
			break;
		}
	}

	// [2013/05/08 chenbing] 
	// 大通道无效的情况下判断子通道是否有效，子通道有效,大通道所在区间正确情况下再解析大通道判断 
	if ( byChnnlIdx >= byHduChnNum )
	{
		if (bySubChnId < HDU_MODEFOUR_MAX_SUBCHNNUM)
		{
			u8 byHduChnId = RESTORECHAN(byChnnlIdx, bySubChnId);
			if ( 0 != byHduChnId && HDU2_PORTSPAN != byHduChnId )
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;
}


/*====================================================================
    函数名      ：IsValidTvwEqp
    功能        ：判断tEqp是否是合法的Tvw设备
    算法实现    ：根据主类型,辅类型以及EqpId进行判断
    引用全局变量：
    输入参数说明：[in]tEqp
    返回值说明  ：TRUE, 合法; FALSE, 不合法
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	11/05/26    4.6          刘旭          创建
====================================================================*/
BOOL32 IsValidTvwEqp(const TEqp& tEqp) 
{
	// Hdu设备也是Tv wall类型
	if (IsValidHduEqp(tEqp))
	{
		return TRUE;
	}

	// 接下来判断是不是普通的Tv Wall

	// 主类型判断
	if (TYPE_MCUPERI != tEqp.GetType())
	{
		return FALSE;
	}

	// Tv Wall类型判断
	if (EQP_TYPE_TVWALL != tEqp.GetEqpType())
	{
		return FALSE;
	}

	// Tv Wall的Id判断
	if(TVWALLID_MIN > tEqp.GetEqpId() || TVWALLID_MAX < tEqp.GetEqpId())
	{
		return FALSE;
	}
	
	return TRUE;
}

/*====================================================================
    函数名      : ClearHduChnnlStatus
    功能        ：清空Hdu外设的某个通道状态的所有信息
    算法实现    ：
    引用全局变量：
    输入参数说明：[in]byEqpId, Hdu设备号
				  [in]byChnnlIdx, Hdu设备通道号, 如果值为0xFF, 则清空该设备所有通道
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	11/05/25    4.6         liuxu           创建
====================================================================*/
void ClearHduChnnlStatus( const u8 byEqpId, const u8 byChnnlIdx /*= 0xFF*/ )
{
	TPeriEqpStatus tStatus;
	if (!g_cMcuVcApp.GetPeriEqpStatus( byEqpId, &tStatus) )
	{
		return;
	}

	if ( byChnnlIdx != 0xFF ) 
	{
		if (!IsValidHduChn(byEqpId, byChnnlIdx))
		{
			return;
		}
		tStatus.m_tStatus.tHdu.atVideoMt[byChnnlIdx].SetNull();
		tStatus.m_tStatus.tHdu.atVideoMt[byChnnlIdx].SetConfIdx(0);
		tStatus.m_tStatus.tHdu.atHduChnStatus[byChnnlIdx].SetNull();
	}
	else
	{
		for (u8 byLoop = 0; byLoop < MAXNUM_HDU_CHANNEL; ++byLoop)
		{
			tStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetNull();
			tStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetConfIdx(0);
			tStatus.m_tStatus.tHdu.atHduChnStatus[byLoop].SetNull();
		}
	}

	g_cMcuVcApp.SetPeriEqpStatus( byEqpId, &tStatus);
}

/*====================================================================
    函数名      : ClearTvwChnnlStatus
    功能        ：清空Tvw外设的某个通道状态的所有信息(包括Hdu)
    算法实现    ：
    引用全局变量：
    输入参数说明：[in]byEqpId, Tvw设备号
				  [in]byChnnlIdx, Tvw设备通道号, 如果值为0xFF, 则清空该设备所有通道
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	11/05/25    4.6         liuxu           创建
====================================================================*/
void ClearTvwChnnlStatus( const u8 byEqpId, const u8 byChnnlIdx /*= 0xFF*/ )
{
	// 如果是HDU, 则清空HDU通道
	if (IsValidHduChn(byEqpId, byChnnlIdx))
	{
		ClearHduChnnlStatus( byEqpId, byChnnlIdx);
		return;
	}

	// 普通电视墙通道
	TPeriEqpStatus tStatus;
	if (!g_cMcuVcApp.GetPeriEqpStatus( byEqpId, &tStatus) )
	{
		return;
	}
	
	if ( byChnnlIdx != 0xFF ) 
	{
		if (!IsValidTvw(byEqpId, byChnnlIdx))
		{
			return;
		}
		
		tStatus.m_tStatus.tTvWall.atVideoMt[byChnnlIdx].byMemberType = 0;
		tStatus.m_tStatus.tTvWall.atVideoMt[byChnnlIdx].SetNull();
		tStatus.m_tStatus.tTvWall.atVideoMt[byChnnlIdx].SetConfIdx( (u8)0 );

	}
	else
	{
		for (u8 byLoop = 0; byLoop < MAXNUM_PERIEQP_CHNNL; ++byLoop)
		{
			tStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = 0;
			tStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetNull();
			tStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx( (u8)0 );
		}
	}
	
	g_cMcuVcApp.SetPeriEqpStatus( byEqpId, &tStatus);
}

/*====================================================================
    函数名      ：IsValidTvw
    功能        ：判断byTvwId和byChnnlIdx是否是合法的Tvw设备的Id和通道
    算法实现    ：
    引用全局变量：
    输入参数说明：[in]byHduId, Tvw设备Id
				  [in]byChnnlIdx, Tvw通道
    返回值说明  ：TRUE, 合法; FALSE, 不合法
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	11/05/26    4.6          刘旭          创建
====================================================================*/
BOOL32 IsValidTvw(const u8 byTvwId, const u8 byChnnlIdx ) 
{
	// Hdu设备也是一种Tv wall
	if (IsValidHduChn(byTvwId, byChnnlIdx))
	{
		return TRUE;
	}else
	{
		TEqp tTvwEqp = g_cMcuVcApp.GetEqp( byTvwId );
		if (!IsValidTvwEqp(tTvwEqp))
		{
			return FALSE;
		}

		if (!IsValidTvwChnnl(byChnnlIdx))
		{
			return FALSE;
		}

		return TRUE;
	}
}

/*====================================================================
    函数名      ：IsValidTvw
    功能        ：判断byTvwId和byChnnlIdx是否是合法的Tvw设备的Id和通道
    算法实现    ：
    引用全局变量：
    输入参数说明：[in]byHduId, Tvw设备Id
				  [in]byChnnlIdx, Tvw通道
    返回值说明  ：TRUE, 合法; FALSE, 不合法
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	11/05/26    4.6          刘旭          创建
====================================================================*/
const char* const GetHduTypeStr( const TEqp& tHdu )
{
	const char* const HDU_STR = "hdu";
	const char* const HDU_L_STR = "hdu_l";
	const char* const HDU_H_STR = "hdu_h";
	const char* const HDU2_STR = "hdu2";
	const char* const HDU2_L_STR = "hdu2_l";
	const char* const HDU2_S_STR = "hdu2_s";
	
	if (!IsValidHduEqp(tHdu))
	{
		return NULL;
	}

	u8 byHduSubType = 0;
	if(g_cMcuAgent.GetHduSubTypeByEqpId(tHdu.GetEqpId(),byHduSubType))
	{
		switch (byHduSubType)
		{
		case HDU_SUBTYPE_HDU_H:
			return HDU_H_STR;
			
		case HDU_SUBTYPE_HDU_L:
			return HDU_L_STR;
			
		case HDU_SUBTYPE_HDU2:
			return HDU2_STR;
			
		case HDU_SUBTYPE_HDU2_L:
			return HDU2_L_STR;

		case HDU_SUBTYPE_HDU2_S:
			return HDU2_S_STR;

		default:
			return HDU_STR;
		}
	}
	else
	{
		return NULL;
	}
}

/*====================================================================
函数名      ：Clear
功能        ：清空Tvwall
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2011/05/25  4.0         刘  旭         创建
====================================================================*/
void CTvwChnnl::Clear()
{
	memset(this, 0, sizeof(CTvwChnnl)); 
	m_tMember.SetNull();
}

/*====================================================================
函数名      ：GetTvwEqp
功能        ：获取通道所在的TEqp对象
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2011/05/25  4.0         刘  旭         创建
====================================================================*/
TEqp CTvwChnnl::GetTvwEqp()
{
	TEqp tRet;
	tRet.SetNull();

	if ( !IsValidTvw( GetEqpId(), GetChnnlIdx() ))
	{
		return tRet;
	}

	return g_cMcuVcApp.GetEqp( GetEqpId() ); 
}

/*====================================================================
函数名      ：operator==
功能        ：复制函数
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2011/05/25  4.0         刘  旭         创建
====================================================================*/
const CTvwChnnl& CTvwChnnl::operator== ( const CTvwChnnl& cOtherTvw )
{
	// 防止自我复制
	if ( this == &cOtherTvw )
	{
		return *this;
	}

	memcpy(this, &cOtherTvw, sizeof(CTvwChnnl));
	return *this;
}

/*====================================================================
函数名      ：operator==
功能        ：复制函数
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2011/05/25  4.0         刘  旭         创建
====================================================================*/
const CConfTvwChnnl& CConfTvwChnnl::operator== ( const CConfTvwChnnl& cOtherTvw)
{
	// 防止自我复制
	if ( this == &cOtherTvw )
	{
		return *this;
	}
	
	memcpy(this, &cOtherTvw, sizeof(CConfTvwChnnl));
	return *this;
}

/************************************************************************/
/* CTvwEqp                                                              */
/************************************************************************/


/*====================================================================
函数名      ：CTvwEqp
功能        ：构造函数
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2011/05/25  4.0         刘  旭         创建
====================================================================*/
CTvwEqp::CTvwEqp()
{
	// 先全部置0
	memset(this, 0, sizeof(CTvwEqp));
	
	// 设置外设属性
	this->SetType( TYPE_MCUPERI );
}

/*====================================================================
函数名      ：GetChnnlNum
功能        ：获取通道数量
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2011/05/25  4.0         刘  旭         创建
====================================================================*/
// u8 CTvwEqp::GetChnnlNum()
// {
// 	return 
// }