// MacAddr.h: interface for the CMacAddr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MACADDR_H__A1C929FD_0442_46CE_8CC7_D748707F103D__INCLUDED_)
#define AFX_MACADDR_H__A1C929FD_0442_46CE_8CC7_D748707F103D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// MAC µÿ÷∑∂®“Â
struct TMacU48
{
public:
    u8 abyVal[6];
public:
    TMacU48()
    {
        SetNull();
    }
    void SetNull()
    {
        memset(this, 0, sizeof(TMacU48));
    }
};

class CMacAddr  
{
public:
	CMacAddr();
    CMacAddr(const s8* szMac);
    CMacAddr(const TMacU48 &u48);
	virtual ~CMacAddr();

    CMacAddr& operator = (const s8* szMac);
    CMacAddr& operator = (const TMacU48 &tU48);

    BOOL operator == (const CMacAddr &cTarget)
    {
        const TMacU48 tAnother = cTarget.GetU48(); 
        return ( memcmp( (void*)&m_u48, &tAnother, sizeof(TMacU48) ) == 0 );
    }

    void operator++(int d);

    BOOL IsInRange( CMacAddr &cMin, CMacAddr &cMax );

    const s8* ToString() const;
    const TMacU48& GetU48() const
    {
        return m_u48;
    }

    void SetNull() { m_u48.SetNull(); }

    BOOL Str2u48(const s8* szMac);
private:
    TMacU48 m_u48;
};

#endif // !defined(AFX_MACADDR_H__A1C929FD_0442_46CE_8CC7_D748707F103D__INCLUDED_)
