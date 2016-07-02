// licensedlgDlg.h : header file
//

#if !defined(AFX_LICENSEDLGDLG_H__3D9AE34D_76A3_408E_9121_26E5B7AF94F0__INCLUDED_)
#define AFX_LICENSEDLGDLG_H__3D9AE34D_76A3_408E_9121_26E5B7AF94F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "osp.h"
#include "mcuconst.h"
#include "kdvencrypt.h"

struct TLicenseParam {
public:
	TLicenseParam()
	{
		Clear();
	}

	u16 GetAuthNum( void ) const
	{
		return ntohs( wAuthNum );
	}
	const s8 *GetAuthName( void ) const
	{
		return szAuthName;
	}
    const s8 *GetDeviceID( void ) const
    {
        return szDeviceID;
    }
    TKdvTime GetExpireDate( void ) const
    {
        return tExpireDate;
    }
	
	void Clear( void )
	{
		memset( this, 0, sizeof(TLicenseParam) );
	}

	void SetAuthNum( const u16 wValue )
	{
		wAuthNum = htons( wValue );
	}
	void SetAuthName( const s8 *achName )
	{
        memset(szAuthName, 0, sizeof(szAuthName));
		if( NULL != achName )
		{ 
            strncpy( szAuthName, achName, 255 ); 
        }
	}
    void SetDeviceID( const s8* achDeviceID)
    {
        memset(szDeviceID, 0, sizeof(szDeviceID));
        if ( NULL != szDeviceID )
        {
            strncpy( szDeviceID, achDeviceID, 255 );
        }
    }

    void SetExpireDate( TKdvTime &tDate )
    {
        tExpireDate = tDate;
    }

private:
	u16 wAuthNum;
	s8  szAuthName[256];
    s8  szDeviceID[256];
    TKdvTime tExpireDate;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/////////////////////////////////////////////////////////////////////////////
// CLicensedlgDlg dialog

class CLicensedlgDlg : public CDialog
{
// Construction
public:
	CLicensedlgDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CLicensedlgDlg)
	enum { IDD = IDD_LICENSE };
	CButton	m_ckDate;
	CDateTimeCtrl	m_cExpireDate;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLicensedlgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CLicensedlgDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnGeneratelicense();
	afx_msg void OnBtnImport();
	afx_msg void OnCheckDate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    BOOL GenLicense(const TLicenseParam &tLicense);
    void GenEncodeData(u8 *pOutBuf, u16 wInBufLen, u8 *pInBuf, BOOL32 bDirect);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LICENSEDLGDLG_H__3D9AE34D_76A3_408E_9121_26E5B7AF94F0__INCLUDED_)
