// MacSerializer.h: interface for the CMacSerializer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MACSERIALIZER_H__86F8FDAA_7CF7_4939_A5B8_89EA794618C0__INCLUDED_)
#define AFX_MACSERIALIZER_H__86F8FDAA_7CF7_4939_A5B8_89EA794618C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MacAddr.h"

#define MACTAG_START        "MACADDR:"
#define MACTAG_END          "MACADDREND"

#define FILLFILESIZE        (int)(512<<10)

// 负责uboot文件读写
class CMacSerializer  
{
public:
	CMacSerializer();
	virtual ~CMacSerializer();

    BOOL LoadUBootBin(const s8* szFilename);

    BOOL WriteMac2Bin( const s8* szTargetFilename, const CMacAddr &cAddr, BYTE byPOS);

    DWORD GetMacCount()
    {
        return m_dwMacCount;
    }
    BOOL GetMac(DWORD dwIndex, CMacAddr& cRet)
    {
        if (dwIndex == 0 || dwIndex == 1)
        {
            cRet = m_acMac[dwIndex];
            return TRUE;
        }
        else
        {
            cRet.SetNull();
            return FALSE;
        }
    }
    
    void SetFillFile(BOOL bFill)
    {
        m_bFillFile = bFill;
    }
    BOOL IsFillFile()
    {
        return m_bFillFile;
    }


private:
    CMacAddr m_acMac[2];
    DWORD    m_dwMacCount;

    u8*      m_abyFileBuf;
    DWORD    m_dwFileSize;
    DWORD    m_dwWriteStart;
    DWORD    m_dwWriteEnd;

    BOOL     m_bFillFile;
};

extern CMacSerializer g_cSerializer;
#endif // !defined(AFX_MACSERIALIZER_H__86F8FDAA_7CF7_4939_A5B8_89EA794618C0__INCLUDED_)
