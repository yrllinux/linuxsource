// ConfData.h: interface for the CConfData class.
// 保存所有数据
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFDATA_H__0E0609B9_860C_40CA_A0F9_AE4823567F28__INCLUDED_)
#define AFX_CONFDATA_H__0E0609B9_860C_40CA_A0F9_AE4823567F28__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RecInterface.h"

#define MAXNUM_CONFREC          32


struct TOneRecData
{
    TConfProperty   m_tConfProp;
    TRecProperty    m_tRecProp;
    TStreamProperty m_atStream[MAXNUM_STREAM_PERREC];
};

class CConfData  
{
public:
	CConfData();
	virtual ~CConfData();

public:
    void SetInited(BOOL bInit = TRUE)
    {
        m_bInited = bInit; 
    }
    BOOL IsInited()const
    {
        return m_bInited;
    }

    void SetConfig(const TRecCfgInfo *ptCfg);
    void GetConfig(TRecCfgInfo *const ptCfg);

    // 会议索引号、通道索引号从1开始，流索引号从0开始
    BOOL SetAt(DWORD dwIdx, const TConfProperty *ptConfProp);
    BOOL SetAt(DWORD dwIdx, const TRecProperty *ptRecProp);
    BOOL SetAt(DWORD dwConfIdx, u8 byStreamIdx, const TStreamProperty *ptStreamProp);

    BOOL Reset(DWORD dwIdx);

    BOOL GetAt(DWORD dwIdx, TConfProperty *ptConfProp);
    BOOL GetAt(DWORD dwIdx, TRecProperty *ptRecProp);
    BOOL GetAt(DWORD dwConfIdx, u8 byStreamIdx, TStreamProperty *ptStreamProp);

    void SetLastError(int nErr)
    {
        m_nLastError = nErr;
    }

    int  GetLastError() const
    {
        return m_nLastError;
    }

    BOOL        m_bInited;                  // 是否初始化过
    TRecCfgInfo m_tCfg;                     // 配置参数
    TOneRecData m_atData[MAXNUM_CONFREC];   // 会议数据
    
    int         m_nLastError;
};

extern CConfData g_cConfData;
#endif // !defined(AFX_CONFDATA_H__0E0609B9_860C_40CA_A0F9_AE4823567F28__INCLUDED_)
