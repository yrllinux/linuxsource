// MacAddr.cpp: implementation of the CMacAddr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mwt.h"
#include "MacAddr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMacAddr::CMacAddr()
{

}

CMacAddr::CMacAddr(const TMacU48 &u48)
{
    m_u48 = u48;
}

CMacAddr::CMacAddr(const s8* szMac)
{
    Str2u48(szMac);
}

CMacAddr& CMacAddr::operator = (const s8* szMac)
{
    Str2u48(szMac);
    return *this;
}

CMacAddr& CMacAddr::operator = (const TMacU48 &tU48)
{
    m_u48 = tU48;
    return *this;
}

// 自动增量,后置版本
void CMacAddr::operator++(int d)
{
    DWORD dwTmp;
    for ( DWORD i = 5; i >= 0; i -- )       
    {
        dwTmp = m_u48.abyVal[i];
        dwTmp ++;
        if (dwTmp <= 0xFF)
        {
            m_u48.abyVal[i] = (u8)dwTmp;
            break;
        }
        else
        {
            m_u48.abyVal[i] = 0;
        }
    }
    // 不考虑最后的上溢出
}

// 判断是否在段内
BOOL CMacAddr::IsInRange( CMacAddr &cMin, CMacAddr &cMax )
{
    DWORD dwThis, dwMin, dwMax;
    TMacU48 tMin = cMin.GetU48();
    TMacU48 tMax = cMax.GetU48();

    dwThis = ( m_u48.abyVal[0]  << 8 ) + m_u48.abyVal[1];
    dwMin  = ( tMin.abyVal[0] << 8   ) + tMin.abyVal[1];
    dwMax  = ( tMax.abyVal[0] << 8   ) + tMax.abyVal[1];

    BOOL bNeedLowerPartMax, bNeedLowerPartMin;
    if ( dwThis > dwMax)
    {
        return FALSE;
    }
    else if ( dwThis == dwMax )
    {
        bNeedLowerPartMax = TRUE;
    }
    else
    {
        // 无需比较低位
        bNeedLowerPartMax = FALSE;
    }
    
    if ( dwThis < dwMin )
    {
        return FALSE;
    }
    else if ( dwThis == dwMin )
    {
        bNeedLowerPartMin = TRUE;
    }
    else
    {
        bNeedLowerPartMin = FALSE;
    }

#define COMBINE_U32(x)  \
    (x.abyVal[2]<<24) |    \
    (x.abyVal[3]<<16) |    \
    (x.abyVal[4]<<8 ) |    \
    (x.abyVal[5])

    dwThis = COMBINE_U32(m_u48);
    dwMax  = COMBINE_U32(tMax);
    dwMin  = COMBINE_U32(tMin);

#undef COMBINE_U32
    
    if ( bNeedLowerPartMax )
    {
        if ( dwThis > dwMax )
        {
            return FALSE;
        }
    }
    if ( bNeedLowerPartMin )
    {
        if ( dwThis < dwMin )
        {
            return FALSE;
        }
    }
    return TRUE;

}

BOOL CMacAddr::Str2u48(const s8* szMac)
{
    const s8* achSep = ":-";    // 允许 : - 作为分隔符
    s8 szToken[KDV_MAX_PATH] = {0};
    s8 *pszToken = szToken;
    strncpy( szToken, szMac, KDV_MAX_PATH - 1);
    
    // 处理字符串
    pszToken = strtok( szToken, achSep );
    int nCount= 0;
    int nTmp = 0;
    char ch;
    BOOL bErr = FALSE;
    while( NULL != pszToken)
    {
        if (strlen(pszToken) == 0 || strlen(pszToken) > 2)
        {
            // Error
            break;
        }

        bErr = FALSE;
        nTmp = 0;
        for (DWORD i = 0; i < strlen(pszToken); i ++ )
        {
            ch = pszToken[i];
            if (ch >= '0' && ch <= '9' )
            {
                nTmp = (nTmp << 4) + (ch - '0');
            }
            else if (ch >= 'A' && ch <= 'F' )
            {
                nTmp =  (nTmp << 4) + (ch - 'A') + 10;
            }
            else if (ch >= 'a' && ch <= 'f' )
            {
                nTmp =  (nTmp << 4) + (ch - 'a') + 10;
            }
            else
            {
                bErr = TRUE;
                break;
            }
        }

        if (bErr)
        {
            break;
        }
        if (nTmp < 0  || nTmp > 255)
        {
            break;
        }

        m_u48.abyVal[nCount] = nTmp;
        nCount ++;
        if (nCount == 6)
        {
            break;
        }

        pszToken = strtok( NULL, achSep );
    }

    if ( nCount == 6 )
    {        
        return TRUE;
    }
    else
    {
        m_u48.SetNull();
        return FALSE;
    }                
}

CMacAddr::~CMacAddr()
{
}

const s8* CMacAddr::ToString() const
{
    static s8 szMacStr[KDV_MAX_PATH] = {0};
    CString cstr, cstr2;
    for (DWORD i = 0; i < 6; i++)
    {
        cstr2.Format( "%.2X", m_u48.abyVal[i] );
        cstr += cstr2;
        if (i != 5)
        {
            cstr += ":";
        }
    }
    strncpy( szMacStr, cstr, KDV_MAX_PATH - 1 );
    return szMacStr;
}