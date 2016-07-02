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

