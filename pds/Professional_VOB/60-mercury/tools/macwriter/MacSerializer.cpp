// MacSerializer.cpp: implementation of the CMacSerializer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mwt.h"
#include "MacSerializer.h"
#include "SNLogMgr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMacSerializer::CMacSerializer()
{
    m_dwWriteStart = 0;
    m_dwWriteEnd = 0;
    m_dwMacCount = 0;
    m_dwFileSize = 0;
    m_abyFileBuf = NULL;
    
    m_bFillFile = TRUE;
}

CMacSerializer::~CMacSerializer()
{
    if ( m_abyFileBuf != NULL)
    {
        delete []m_abyFileBuf;
    }
}

BOOL CMacSerializer::LoadUBootBin(const s8* szFilename)
{
    if ( m_abyFileBuf != NULL)
    {
        delete []m_abyFileBuf;
        m_abyFileBuf = NULL;
    }

	CFile f;
    BOOL bOpen = f.Open(szFilename, CFile::modeRead);
    if (!bOpen)
    {
        return FALSE;
    }

    m_dwFileSize =f.GetLength();

    m_abyFileBuf = new u8[m_dwFileSize];
    f.ReadHuge((void*)m_abyFileBuf, m_dwFileSize);
    f.Close();    

    m_dwWriteStart = 0;
    m_dwWriteEnd = 0;
    for ( DWORD i = 0; i < m_dwFileSize - 10; i++ )
    {
        if ( memcmp( m_abyFileBuf+i, MACTAG_START, strlen(MACTAG_START) ) == 0 )
        {
            m_dwWriteStart = i + strlen(MACTAG_START);
        }
        else if ( memcmp( m_abyFileBuf+i, MACTAG_END, strlen(MACTAG_END) ) == 0 )
        {
            m_dwWriteEnd = i;
            break;
        }
    }

    m_dwMacCount = 0;
    m_acMac[0].SetNull();
    m_acMac[1].SetNull();
    if (m_dwWriteStart == 0 || m_dwWriteEnd == 0)
    {
        // 没找到
        return FALSE;
    }

    if ( m_dwWriteEnd - m_dwWriteStart != 6 && 
         m_dwWriteEnd - m_dwWriteStart !=  12 )
    {
        // 长度不正确
        return FALSE;
    }

    m_dwMacCount = (m_dwWriteEnd - m_dwWriteStart) / 6;
    TMacU48 tU48;
    for (i = 0; i < m_dwMacCount; i++)
    {
        memcpy( (void*)&tU48, &m_abyFileBuf[m_dwWriteStart+i*6], 6 );
        m_acMac[i] = tU48;
    }
    return TRUE;
    
}

// byPOS 指定了写入的位置
// 如果是0xFF，则自动增量写入，如果是0或者1，则根据用户指定的写入
BOOL CMacSerializer::WriteMac2Bin( const s8* szTargetFilename, const CMacAddr &cAddr, BYTE byPOS)
{
    if ( m_dwMacCount == 0 ||
         m_dwWriteStart == 0 ||
         m_abyFileBuf == NULL )
    {
        return FALSE;
    }

    CMacAddr cTargetAddr = cAddr;

    if ( byPOS == 0xFF )
    {
        // 自动写入
        for (DWORD i = 0; i < m_dwMacCount; i ++)
        {
            TMacU48 tU48 = cTargetAddr.GetU48();
            memcpy( &m_abyFileBuf[ m_dwWriteStart + i * 6 ], 
                    (void*)&tU48,
                    6 );
            cTargetAddr++;
        }
    }
    else if ( byPOS == 0 )
    {
        TMacU48 tU48 = cTargetAddr.GetU48();
        memcpy( &m_abyFileBuf[ m_dwWriteStart ], 
                (void*)&tU48,
                6 );
    }
    else if ( byPOS == 1 && m_dwMacCount == 2)
    {
        TMacU48 tU48 = cTargetAddr.GetU48();
        memcpy( &m_abyFileBuf[ m_dwWriteStart+6 ], 
                (void*)&tU48,
                6 );
    }
    else
    {
        // 非法位置
        return FALSE;
    }
    
	CFile f;
    BOOL bOpen = f.Open(szTargetFilename, CFile::modeWrite | CFile::modeCreate);
    if (!bOpen)
    {
        return FALSE;
    }

    DWORD dwWriteFileSize = m_dwFileSize;
    if (IsFillFile())
    {
        if (m_dwFileSize < FILLFILESIZE)
        {
            dwWriteFileSize = FILLFILESIZE;
        }
        u8 *pbyOutputBuf = new u8[dwWriteFileSize];
        memset( pbyOutputBuf, 0, dwWriteFileSize );
        memcpy( pbyOutputBuf, m_abyFileBuf, m_dwFileSize);
        f.WriteHuge( pbyOutputBuf, dwWriteFileSize );
        delete []pbyOutputBuf;
    }
    else
    {
        f.WriteHuge( m_abyFileBuf, m_dwFileSize );
    }

    
    f.Flush();
    f.Close();

    return TRUE;
}