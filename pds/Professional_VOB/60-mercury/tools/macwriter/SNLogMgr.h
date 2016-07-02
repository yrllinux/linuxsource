// SNLogMgr.h: interface for the CSNLogMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SNLOGMGR_H__45734DF0_40DB_4CB6_81B6_BA320D08245D__INCLUDED_)
#define AFX_SNLOGMGR_H__45734DF0_40DB_4CB6_81B6_BA320D08245D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MacAddr.h"

#define FILENAME_LOG        "mtwlog.txt"

// 每行Log记录的内容
struct SNItem
{
    time_t  tTime;
    CString cstrSN;
    DWORD   dwMACCount;
    TMacU48 atU48[2];
};

class CSNLogMgr  
{
public:
	CSNLogMgr();
	virtual ~CSNLogMgr();

public:
    void SetAutoInc(CString cstrBegin)
    {
        bAutoInc = TRUE;
        cstrAutoSNBase = cstrBegin;
    }

    void SetNoAutoInc()
    {
        bAutoInc = FALSE;
        cstrAutoSNBase.Empty();
    }

    void AddNewItem(SNItem &item)
    {
        m_vtAddedSNItem.push_back(item);
    }

    void AddHistoryItem(SNItem &item)
    {
        m_vtHistorySNItem.push_back(item);
    }

    BOOL IsSNExistInHistory(SNItem &item);
    BOOL IsSNExistInNew(SNItem &item);

    // 最新添加里面查找指定MAC的SN
    BOOL GetMatchedSNbyMacInNew(const CMacAddr &cAddr, SNItem &item);

    // 历史SN存取
    void LoadHistorySNFromFile();
    void SaveHistorySN();


private:
    // 自动增量方式
    BOOL bAutoInc;
    CString cstrAutoSNBase;
    
public:
    // 本次配置的条目
    vector<SNItem> m_vtAddedSNItem;

    // 所有历史条目
    vector<SNItem> m_vtHistorySNItem;

};

extern CSNLogMgr g_SNLogMsg;
#endif // !defined(AFX_SNLOGMGR_H__45734DF0_40DB_4CB6_81B6_BA320D08245D__INCLUDED_)
