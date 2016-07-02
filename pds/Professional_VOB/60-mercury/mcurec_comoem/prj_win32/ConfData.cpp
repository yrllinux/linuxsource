// ConfData.cpp: implementation of the CConfData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdio.h"
#include "ConfData.h"
#include "mcu2recitf.h"

CConfData g_cConfData;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfData::CConfData()
{

}

CConfData::~CConfData()
{

}

void CConfData::SetConfig(const TRecCfgInfo *ptCfg)
{
    if (ptCfg)
        memcpy(&m_tCfg, ptCfg, sizeof(TRecCfgInfo));
}

void CConfData::GetConfig(TRecCfgInfo *const ptCfg)
{
    if (ptCfg)
        memcpy(ptCfg, &m_tCfg, sizeof(TRecCfgInfo));
}

#define CHECK_CONFIDX(idx)  \
    if (idx == 0 || idx > MAXNUM_CONFREC)   \
        return FALSE;   \

#define CHECK_STREAMIDX(idx)    \
    if (idx == 0 || idx > MAXNUM_STREAM_PERREC)   \
        return FALSE;   \

BOOL CConfData::SetAt(DWORD dwIdx, const TConfProperty *ptConfProp)
{
    if (dwIdx == 0 || dwIdx > MAXNUM_CONFREC)
        return FALSE;

    if (NULL == ptConfProp)
        return FALSE;

    if ( dwIdx != ptConfProp->GetIndex() )
        return FALSE;

    m_atData[dwIdx-1].m_tConfProp = *ptConfProp;
    return TRUE;
}

BOOL CConfData::SetAt(DWORD dwIdx, const TRecProperty *ptRecProp)
{
    CHECK_CONFIDX(dwIdx);
    
    if (NULL == ptRecProp)
        return FALSE;    
    
    m_atData[dwIdx-1].m_tRecProp = *ptRecProp;
    return TRUE;    
}

BOOL CConfData::SetAt(DWORD dwConfIdx, u8 byStreamIdx, const TStreamProperty *ptStreamProp)
{
    CHECK_CONFIDX(dwConfIdx);

    CHECK_STREAMIDX(byStreamIdx);
    
    if (NULL == ptStreamProp)
        return FALSE;    

    if ( ptStreamProp->GetConfIndex() != dwConfIdx ||
         ptStreamProp->GetStreamIndex() != byStreamIdx )
        return FALSE;

    m_atData[dwConfIdx-1].m_atStream[byStreamIdx-1] = *ptStreamProp;
    return TRUE;
}

BOOL CConfData::Reset(DWORD dwIdx)
{
    CHECK_CONFIDX(dwIdx);

    memset(&m_atData[dwIdx-1], 0, sizeof(TOneRecData));
    return TRUE;
}

BOOL CConfData::GetAt(DWORD dwIdx, TConfProperty *ptConfProp)
{
    CHECK_CONFIDX(dwIdx);

    if (NULL == ptConfProp)
        return FALSE;
    
    DWORD dwMyIdx = m_atData[dwIdx-1].m_tConfProp.GetIndex();
    CHECK_CONFIDX(dwMyIdx);
    
    memcpy(ptConfProp, &m_atData[dwIdx-1].m_tConfProp, sizeof(TConfProperty));
    return TRUE;
}

BOOL CConfData::GetAt(DWORD dwIdx, TRecProperty *ptRecProp)
{
    CHECK_CONFIDX(dwIdx);

    if (NULL == ptRecProp)
        return FALSE;

    DWORD dwMyIdx = m_atData[dwIdx-1].m_tConfProp.GetIndex();
    CHECK_CONFIDX(dwMyIdx);

    memcpy(ptRecProp, &m_atData[dwIdx-1].m_tRecProp, sizeof(TRecProperty));
    return TRUE;
}

BOOL CConfData::GetAt(DWORD dwConfIdx, u8 byStreamIdx, TStreamProperty *ptStreamProp)
{
    CHECK_CONFIDX(dwConfIdx);
    
    CHECK_STREAMIDX(byStreamIdx);
    
    if (NULL == ptStreamProp)
        return FALSE;    

    DWORD dwMyIdx = m_atData[dwConfIdx-1].m_tConfProp.GetIndex();
    CHECK_CONFIDX(dwMyIdx);
    
    memcpy(ptStreamProp, &m_atData[dwConfIdx-1].m_atStream[byStreamIdx-1], sizeof(TStreamProperty));
    return TRUE;
}

void RecComLog( u8 byLvl, s8 * fmt, ... )
{
    s8  achBuf[255];
    va_list argptr;    
    if( byLvl <= g_byComLogLevel )
    {
        s32 nPrefix = sprintf( achBuf, "[RecCom]:" );
        va_start( argptr, fmt );    
        vsprintf( achBuf + nPrefix, fmt, argptr );   
        OspPrintf(TRUE, FALSE, achBuf); 
        va_end(argptr); 
    }
}

API void setreccomlog(u8 byLvl)
{
    g_byComLogLevel = byLvl;
    OspPrintf(TRUE, FALSE, "g_byComLogLevel=%d\n", g_byComLogLevel);
    return ;
}

#undef CHECK_STREAMIDX
#undef CHECK_CONFIDX
