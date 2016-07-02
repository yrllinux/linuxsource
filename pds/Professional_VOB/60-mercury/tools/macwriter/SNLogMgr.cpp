// SNLogMgr.cpp: implementation of the CSNLogMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mwt.h"
#include "SNLogMgr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CSNLogMgr g_SNLogMsg;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CSNLogMgr::CSNLogMgr()
{
    bAutoInc = FALSE;
}

CSNLogMgr::~CSNLogMgr()
{

}

// 在历史记录里面查找
BOOL CSNLogMgr::IsSNExistInHistory(SNItem &item)
{
    vector<SNItem>::const_iterator citr = g_SNLogMsg.m_vtHistorySNItem.begin();
    vector<SNItem>::const_iterator citrEnd = g_SNLogMsg.m_vtHistorySNItem.end(); 

    while (citr !=citrEnd)
    {
        if ( citr->cstrSN == item.cstrSN )
        {
            return TRUE;
        }
        citr ++;
    }
    return FALSE;
}

// 最新添加里面查找
BOOL CSNLogMgr::IsSNExistInNew(SNItem &item)
{
    vector<SNItem>::const_iterator citr = g_SNLogMsg.m_vtAddedSNItem.begin();
    vector<SNItem>::const_iterator citrEnd = g_SNLogMsg.m_vtAddedSNItem.end(); 

    while (citr !=citrEnd)
    {
        if ( citr->cstrSN == item.cstrSN )
        {
            return TRUE;
        }
        citr ++;
    }
    return FALSE;
}

// 最新添加里面查找指定MAC的SN
BOOL CSNLogMgr::GetMatchedSNbyMacInNew(const CMacAddr &cAddr, SNItem &item)
{
    vector<SNItem>::const_iterator citr = g_SNLogMsg.m_vtAddedSNItem.begin();
    vector<SNItem>::const_iterator citrEnd = g_SNLogMsg.m_vtAddedSNItem.end(); 

    CMacAddr cAnotherMac;
    while (citr != citrEnd)
    {
        cAnotherMac = citr->atU48[0];
        if ( cAnotherMac == cAddr )
        {
            item = *citr;
            return TRUE;
        }

        cAnotherMac = citr->atU48[1];
        if ( citr->dwMACCount == 2 && cAnotherMac == cAddr )
        {
            item = *citr;
            return TRUE;
        }

        citr ++;
    }
    return FALSE;
}

const s8* GetRunPath ();


void CSNLogMgr::LoadHistorySNFromFile()
{
    g_SNLogMsg.m_vtHistorySNItem.clear();


    CString cstrFilename = GetRunPath();
    cstrFilename += FILENAME_LOG;

    DWORD dwCount = ::GetPrivateProfileInt(_T("General"), _T("Count"), 0, cstrFilename);
    DWORD dwTmp;

    CMacAddr cAddr;
    CString cstrSec;
    SNItem item;
    s8 szTmp[KDV_MAX_PATH+1] = {0};

    for ( DWORD dwIndex = 0; dwIndex < dwCount; dwIndex ++)
    {        
        cstrSec.Format("Entry%u", dwIndex);

        dwTmp = ::GetPrivateProfileInt(cstrSec, _T("Time"), 0, cstrFilename); 
        item.tTime = dwTmp;

        ::GetPrivateProfileString(cstrSec, _T("SN"), "", szTmp, KDV_MAX_PATH, cstrFilename);
        item.cstrSN = szTmp;

        dwTmp = ::GetPrivateProfileInt(cstrSec, _T("MACNum"), 1, cstrFilename); 
        item.dwMACCount = dwTmp;

        ::GetPrivateProfileString(cstrSec, _T("MAC0"), "", szTmp, KDV_MAX_PATH, cstrFilename);
        cAddr = szTmp;
        memcpy( &item.atU48[0], &cAddr.GetU48(), sizeof(TMacU48) );

        if ( item.dwMACCount == 2 )
        {
            ::GetPrivateProfileString(cstrSec, _T("MAC1"), "", szTmp, KDV_MAX_PATH, cstrFilename);
            cAddr = szTmp;
            memcpy( &item.atU48[1], &cAddr.GetU48(), sizeof(TMacU48) );
        }

        g_SNLogMsg.AddHistoryItem( item );

    }
    
}

void CSNLogMgr::SaveHistorySN()
{
    vector<SNItem>::const_iterator citr = g_SNLogMsg.m_vtHistorySNItem.begin();
    vector<SNItem>::const_iterator citrEnd = g_SNLogMsg.m_vtHistorySNItem.end(); 

    CString cstrFilename = GetRunPath();
    cstrFilename += FILENAME_LOG;

    CString cstr, cstrSec;
    cstr.Format("%u", g_SNLogMsg.m_vtHistorySNItem.size());
    DWORD dwIndex = 0;
    CMacAddr cAddr;

    CFile f;
    f.Open( cstrFilename, CFile::modeWrite | CFile::modeCreate );
    f.Close();

    ::WritePrivateProfileString(_T("General"), _T("Count"), cstr, cstrFilename); 
    while ( citr != citrEnd )
    {
        cstrSec.Format("Entry%u", dwIndex);

        cstr.Format("%u", citr->tTime);

        ::WritePrivateProfileString(cstrSec, _T("Time"), cstr, cstrFilename); 

        ::WritePrivateProfileString(cstrSec, _T("SN"), citr->cstrSN, cstrFilename); 

        cstr.Format("%u", citr->dwMACCount);
        ::WritePrivateProfileString(cstrSec, _T("MACNum"), cstr, cstrFilename); 

        cAddr = citr->atU48[0];
        ::WritePrivateProfileString(cstrSec, _T("MAC0"), cAddr.ToString(), cstrFilename); 

        if ( citr->dwMACCount == 2 )
        {
            cAddr = citr->atU48[1];
            ::WritePrivateProfileString(cstrSec, _T("MAC1"), cAddr.ToString(), cstrFilename); 
        }
                
        dwIndex ++;
        citr ++;
    }
}