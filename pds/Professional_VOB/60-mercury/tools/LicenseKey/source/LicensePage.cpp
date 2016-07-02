// LicensePage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "usbkeyconst.h"
#include "usbkey.h"
#include "readlicense.h"
#include "LicensePage.h"
#include "licensekeyconst.h"

extern HRESULT hr;
extern IOprKey *pISL;


IMPLEMENT_DYNCREATE(CMcuLicensePage, CPropertyPage)
IMPLEMENT_DYNCREATE(CMtLicensePage, CPropertyPage)
IMPLEMENT_DYNCREATE(CT120LicensePage, CPropertyPage)


/////////////////////////////////////////////////////////////////////////////
// CMcuLicensePage property page

CMcuLicensePage::CMcuLicensePage() : CPropertyPage(CMcuLicensePage::IDD)
{
	//{{AFX_DATA_INIT(CMcuLicensePage)
	m_uAccessNum = 0;
	m_sDeviceID = _T("");
	m_bCheckDate = FALSE;
	//}}AFX_DATA_INIT
}

CMcuLicensePage::~CMcuLicensePage()
{
}

void CMcuLicensePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMcuLicensePage)
	DDX_Control(pDX, IDC_COMBO_MCU_TYPE, m_cboxMcuType);
	DDX_Control(pDX, IDC_DATETIMEPICKER_EXPIREDATE, m_dtcExpireDate);
	DDX_Text(pDX, IDC_EDIT_ACCESSNUM, m_uAccessNum);
	DDX_Text(pDX, IDC_EDIT_DEVICEID, m_sDeviceID);
	DDX_Check(pDX, IDC_CHECK_DATE, m_bCheckDate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMcuLicensePage, CPropertyPage)
	//{{AFX_MSG_MAP(CMcuLicensePage)
	ON_BN_CLICKED(IDC_BTN_GENERATE, OnBtnGenerate)
	ON_BN_CLICKED(IDC_BTN_IMPORT, OnBtnImport)
	ON_BN_CLICKED(IDC_CHECK_DATE, OnCheckDate)
	ON_BN_CLICKED(IDC_BTN_REFRESH, OnBtnRefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMtLicensePage property page

CMtLicensePage::CMtLicensePage() : CPropertyPage(CMtLicensePage::IDD)
{
	//{{AFX_DATA_INIT(CMtLicensePage)
	m_nRadioRunKDMMt = -1;
	m_sDeviceID = _T("");
	//}}AFX_DATA_INIT
}

CMtLicensePage::~CMtLicensePage()
{
}

void CMtLicensePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMtLicensePage)
	DDX_Control(pDX, IDC_COMBO_MTMODAL, m_cboxMtModal);
	DDX_Control(pDX, IDC_COMBO_ACCESSNUM, m_cboxAccessNum);
	DDX_Radio(pDX, IDC_RADIO_RUNKDMMT, m_nRadioRunKDMMt);
	DDX_Text(pDX, IDC_EDIT_DEVICEID, m_sDeviceID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMtLicensePage, CPropertyPage)
	//{{AFX_MSG_MAP(CMtLicensePage)
	ON_BN_CLICKED(IDC_BTN_GENERATE, OnBtnGenerate)
	ON_BN_CLICKED(IDC_BTN_IMPORT, OnBtnImport)
	ON_BN_CLICKED(IDC_BTN_REFRESH, OnBtnRefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CT120LicensePage property page

CT120LicensePage::CT120LicensePage() : CPropertyPage(CT120LicensePage::IDD)
{
	//{{AFX_DATA_INIT(CT120LicensePage)
	m_csDeviceID = _T("");
	//}}AFX_DATA_INIT
}

CT120LicensePage::~CT120LicensePage()
{
}

void CT120LicensePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CT120LicensePage)
	DDX_Control(pDX, IDC_COMBO_ACCESSNUM, m_cboxAccessNum);
	DDX_Text(pDX, IDC_EDIT_DEVICEID, m_csDeviceID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CT120LicensePage, CPropertyPage)
	//{{AFX_MSG_MAP(CT120LicensePage)
	ON_BN_CLICKED(IDC_BTN_GENERATE, OnBtnGenerate)
	ON_BN_CLICKED(IDC_BTN_IMPORT, OnBtnImport)
	ON_BN_CLICKED(IDC_BTN_REFRESH, OnBtnRefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CMcuLicensePage::OnBtnGenerate() 
{
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );
	CString csLicenseContent;
	m_csAuthCode = DEFAULT_MCULICENSE_AUTHCODE;
	
	//判断接入数量合法性
	if ( m_uAccessNum <= 0 || m_uAccessNum > 448)
    {
        AfxMessageBox("错误的接入数，请重新输入!");
        return;
    }
	CString csAccessNum;
	csAccessNum.Format("%d",m_uAccessNum);

	//判断设备ID合法性
	if ( m_sDeviceID.IsEmpty() )
    {
        AfxMessageBox("请输入MCU 设备 ID !");
        return;        
    }
	if( IsDeviceIDInvalid( m_sDeviceID, ':' ) )
	{
		AfxMessageBox("MCU 设备 ID 非法，请重新输入!");
        return; 
	}
	
	//对截止时间进行限制
	CTime ctMaxTime(2037,1,1,0,0,0);
	CTime ctCurTime;
	CString csCurTime;
	m_dtcExpireDate.GetTime( ctCurTime );
	if( ( ctCurTime > ctMaxTime ) || !m_bCheckDate )
	{
		ctCurTime = ctMaxTime;
	}
	csCurTime.Format("%4d-%2d-%2d",ctCurTime.GetYear(),ctCurTime.GetMonth(),ctCurTime.GetDay());

	CString csMcuType;
	if( m_cboxMcuType.GetCurSel() == CB_ERR )
	{
		AfxMessageBox("请选择MCU类型!");
        return; 
	}
    m_cboxMcuType.GetWindowText(csMcuType);

	CFileDialog cFileDlg(FALSE, "key", "kedalicense", OFN_OVERWRITEPROMPT, "License Key文件(.key)|*.key||");
	if(IDOK == cFileDlg.DoModal())
	{
		CString csPath = cFileDlg.GetPathName();
		m_csLicensePath = csPath;
		csLicenseContent += KEY_LICENSE_FILE_PATH;
		csLicenseContent += "=";
		csLicenseContent += csPath;
        //csLicenseContent += USB_PATH;
		csLicenseContent += "&";
	}
	else
	{
		return;
	}

    // guzh [6/13/2007] dll 负责写入本字段
    /*
	csLicenseContent += KEY_LICENSE_KEY;
	csLicenseContent += "=";
	csLicenseContent += m_csLicenseKey;
	csLicenseContent += "&";
    */

	csLicenseContent += KEY_LICENSE_VERSION;
	csLicenseContent += "=";
	csLicenseContent += m_csLicenseVersion;
	csLicenseContent += "&";

	csLicenseContent += KEY_LICENSE_TYPE;
	csLicenseContent += "=";
	csLicenseContent += m_csLicenseType;
	csLicenseContent += "&";
	
	csLicenseContent += KEY_MCU_DEVICE_ID;
	csLicenseContent += "=";
	csLicenseContent += m_sDeviceID;
	csLicenseContent += "&";

	csLicenseContent += KEY_MCU_TYPE;
	csLicenseContent += "=";
	csLicenseContent += csMcuType;
	csLicenseContent += "&";
	
	csLicenseContent += KEY_MCU_ACCESS_NUM;
	csLicenseContent += "=";
	csLicenseContent += csAccessNum;
	csLicenseContent += "&";

	csLicenseContent += KEY_MCU_EXP_DATE;
	csLicenseContent += "=";
	csLicenseContent += csCurTime;

	WriteFile( csLicenseContent, m_csAuthCode, m_csLicenseKey, m_csErrorCode );
}

void CMcuLicensePage::OnCheckDate() 
{
	// TODO: Add your control notification handler code here
	m_bCheckDate = !m_bCheckDate;
	m_dtcExpireDate.EnableWindow( m_bCheckDate );
}

void CMtLicensePage::OnBtnGenerate() 
{
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );
	CString csLicenseContent;
	m_csAuthCode = DEFAULT_MCULICENSE_AUTHCODE;
	
    //判断设备ID合法性
    if ( m_sDeviceID.IsEmpty() )
    {
        AfxMessageBox("请输入MT 设备 ID !");
        return;        
    }
    if( IsDeviceIDInvalid( m_sDeviceID, '-' ) )
    {
        AfxMessageBox("MT 设备 ID 非法，请重新输入!");
        return; 
	}

	//接入数量
	CString csAccessNum;
	if( m_cboxAccessNum.GetCurSel() == CB_ERR )
	{
		AfxMessageBox("请选择MC接入数量!");
        return; 
	}
    m_cboxAccessNum.GetWindowText(csAccessNum);	
    
	//是否开启终端监控
	CString csRunKDMMt;
	if( IsDlgButtonChecked(IDC_RADIO_RUNKDMMT) )
	{
		csRunKDMMt = STRING_YES;
	}
	else
	{
		csRunKDMMt = STRING_NO;
	}

    /*
    //是否内置GK
    CString csRunGK;
    if ( IsDlgButtonChecked(IDC_RADIO_RUNGK) )
    {
        csRunGK = STRING_YES;
    }
    else
    {
        csRunGK = STRING_NO;
    }

    //是否内置防火墙代理
    CString csRunProxy;
    if ( IsDlgButtonChecked(IDC_RADIO_RUNPROXY) )
    {
        csRunProxy = STRING_YES;
    }
    else
    {
        csRunProxy = STRING_NO;
    }
    */

	
	//mt类型
	CString csMtModal;
	if( m_cboxMtModal.GetCurSel() == CB_ERR )
	{
		AfxMessageBox("请选择MT类型!");
        return; 
	}
    if (m_cboxMtModal.GetCurSel() == 0)
    {
        csMtModal = "8010A";
    }
    else if (m_cboxMtModal.GetCurSel() == 1)
    {
        csMtModal = "TS6610";	
    }
    //m_cboxMtModal.GetWindowText(csMtModal);	

	CFileDialog cFileDlg(FALSE, "key", "kedalicense", OFN_OVERWRITEPROMPT, "License Key文件(.key)\0*.key\0");
	if(IDOK == cFileDlg.DoModal())
	{
		CString csPath = cFileDlg.GetPathName();
		m_csLicensePath = csPath;
		csLicenseContent += KEY_LICENSE_FILE_PATH;
		csLicenseContent += "=";
		csLicenseContent += csPath;
        //csLicenseContent += USB_PATH;
		csLicenseContent += "&";
	}
	else
	{
		return;
	}
    // guzh [6/13/2007] dll 负责写入本字段
/*
	csLicenseContent += KEY_LICENSE_KEY;
	csLicenseContent += "=";
	csLicenseContent += m_csLicenseKey;
	csLicenseContent += "&";
*/
	csLicenseContent += KEY_LICENSE_VERSION;
	csLicenseContent += "=";
	csLicenseContent += m_csLicenseVersion;
	csLicenseContent += "&";

	csLicenseContent += KEY_LICENSE_TYPE;
	csLicenseContent += "=";
	csLicenseContent += m_csLicenseType;
	csLicenseContent += "&";
	
	csLicenseContent += KEY_MT_MODAL;
	csLicenseContent += "=";
	csLicenseContent += csMtModal;
	csLicenseContent += "&";
	
    csLicenseContent += KEY_MT_DEVICE_ID;
    csLicenseContent += "=";
    csLicenseContent += m_sDeviceID;
	csLicenseContent += "&";

	csLicenseContent += KEY_MT_ACCESS_NUM;
	csLicenseContent += "=";
	csLicenseContent += csAccessNum;
	csLicenseContent += "&";

	csLicenseContent += KEY_RUN_KDMMT;
	csLicenseContent += "=";
	csLicenseContent += csRunKDMMt;
    
/*    
    csLicenseContent += "&";


    csLicenseContent += KEY_MT_RUNGK;
    csLicenseContent += "=";
    csLicenseContent += csRunGK;
    csLicenseContent += "&";

    csLicenseContent += KEY_MT_RUNPROXY;
    csLicenseContent += "=";
    csLicenseContent += csRunProxy;
*/

	WriteFile( csLicenseContent, m_csAuthCode, m_csLicenseKey, m_csErrorCode );
}

void CT120LicensePage::OnBtnGenerate() 
{
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );
		CString csLicenseContent;
	m_csAuthCode = DEFAULT_MCULICENSE_AUTHCODE;
	
	//接入数量
	CString csAccessNum;
	if( m_cboxAccessNum.GetCurSel() == CB_ERR )
	{
		AfxMessageBox("请选择接入数量!");
        return; 
	}
    m_cboxAccessNum.GetWindowText(csAccessNum);	

	//判断设备ID合法性
	if ( m_csDeviceID.IsEmpty() )
    {
        AfxMessageBox("请输入T120设备 ID !");
        return;        
    }
	if( IsDeviceIDInvalid( m_csDeviceID, ':' ) )
	{
		AfxMessageBox("T120设备 ID 非法，请重新输入!");
        return; 
	}
	
	CFileDialog cFileDlg(FALSE, "key", "t120license", OFN_OVERWRITEPROMPT, "License Key文件(.key)\0*.key\0");
	if(IDOK == cFileDlg.DoModal())
	{
		CString csPath = cFileDlg.GetPathName();
		m_csLicensePath = csPath;
		csLicenseContent += KEY_LICENSE_FILE_PATH;
		csLicenseContent += "=";
		csLicenseContent += csPath;
        //csLicenseContent += USB_PATH;
		csLicenseContent += "&";
	}
	else
	{
		return;
	}

    // guzh [6/13/2007] dll 负责写入本字段
    /*
	csLicenseContent += KEY_LICENSE_KEY;
	csLicenseContent += "=";
	csLicenseContent += m_csLicenseKey;
	csLicenseContent += "&";
    */

	csLicenseContent += KEY_LICENSE_VERSION;
	csLicenseContent += "=";
	csLicenseContent += m_csLicenseVersion;
	csLicenseContent += "&";

	csLicenseContent += KEY_LICENSE_TYPE;
	csLicenseContent += "=";
	csLicenseContent += m_csLicenseType;
	csLicenseContent += "&";
	
	csLicenseContent += KEY_T120_ACCESS_NUM;
	csLicenseContent += "=";
	csLicenseContent += csAccessNum;
	csLicenseContent += "&";

	csLicenseContent += KEY_T120_DEVICE_ID;
	csLicenseContent += "=";
	csLicenseContent += m_csDeviceID;

	WriteFile( csLicenseContent, m_csAuthCode, m_csLicenseKey, m_csErrorCode );
}

void CMcuLicensePage::OnBtnImport() 
{
	// TODO: Add your control notification handler code here
	CString csPath;
	CFileDialog cFileDlg(TRUE);
	
	if(IDOK == cFileDlg.DoModal())
	{
		csPath = cFileDlg.GetPathName();
	}
	else
	{
		return;
	}

	unsigned short wRet = 0; //ReadLicenseFile(Path);
	wRet = ReadLicenseFile( (s8*)(LPCSTR)csPath );
	if(SUCCESS_LICENSE != wRet) 
	{
		AfxMessageBox("读取License文件失败");
		return ;
	}

	TKeyValue atKeyValue[50];
	s32 nLL = 50;

	wRet = EnumKeyList(atKeyValue, nLL);
	
	s8 asError[100];
	memset( asError, 0, 100);
	if ( !MatchLicense( (LPCSTR)m_csLicenseKey, (LPCSTR)m_csLicenseVersion, (LPCSTR)m_csLicenseType, asError ) )
	{
		AfxMessageBox(asError);
		return;
	}

	char cGetValue[50];
	CString csGetValue;
	int nValuelen = 50;

	//读设备ID
	nValuelen = 50;
	memset( cGetValue, 0, sizeof(cGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_MCU_DEVICE_ID, (char*)cGetValue, nValuelen) ) )
	{
		AfxMessageBox("读取设备ID失败");
		return;
	}
	m_sDeviceID = cGetValue;
	if ( IsDeviceIDInvalid( m_sDeviceID, ':' ) )
	{
		AfxMessageBox("设备ID错误");
		m_sDeviceID = _T("");
		return;
	}

	//读MCU类型
	nValuelen = 50;
	memset( cGetValue, 0, sizeof(cGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_MCU_TYPE, (char*)cGetValue, nValuelen) ) )
	{
		AfxMessageBox("读取MCU类型失败");
		return;
	}
    if ( strcmp(cGetValue, "8000") == 0 )
    {
        m_cboxMcuType.SetCurSel(0);
    }
    else if ( strcmp(cGetValue, "8000B") == 0 )
    {
        m_cboxMcuType.SetCurSel(1);
    }
    else
    {
        m_cboxMcuType.SetCurSel(2);
    }

	//读MCU接入能力
	nValuelen = 50;
	memset( cGetValue, 0, sizeof(cGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_MCU_ACCESS_NUM, (char*)cGetValue, nValuelen) ) )
	{
		AfxMessageBox("读取MCU接入能力失败");
		return;
	}
	m_uAccessNum = atoi( cGetValue );

	//读失效日期
	nValuelen = 50;
	memset( cGetValue, 0, sizeof(cGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_MCU_EXP_DATE, (char*)cGetValue, nValuelen) ) )
	{
		AfxMessageBox("读取MCU截止日期失败");
		return;
	}
	SYSTEMTIME time;
	memset(&time,0,sizeof(SYSTEMTIME));
	char * token = strtok( cGetValue, "-" );
	time.wYear = atoi( token );
	token = strtok( NULL, "-" );
	time.wMonth = atoi( token );
	token = strtok( NULL, "-" );
	time.wDay = atoi( token );
	
	if( !m_bCheckDate )
	{
		m_bCheckDate = TRUE;
		m_dtcExpireDate.EnableWindow( TRUE );
	}

	UpdateData( FALSE );
	
	m_dtcExpireDate.SetTime( &time );

    // guzh [6/8/2007] 显示序列号
    nValuelen = 50;
    memset( cGetValue, 0, sizeof(cGetValue) );
    if( SUCCESS_LICENSE == GetValue((LPSTR)KEY_LICENSE_KEY, (char*)cGetValue, nValuelen) ) 
    {
        SetDlgItemText(IDC_ST_USRSN, cGetValue);
	}    
}

void CMtLicensePage::OnBtnImport() 
{
	// TODO: Add your control notification handler code here
	CString csPath;
	CFileDialog cFileDlg(TRUE);
	
	if(IDOK == cFileDlg.DoModal())
	{
		csPath = cFileDlg.GetPathName();
	}
	else
	{
		return;
	}

	unsigned short wRet = 0;// ReadLicenseFile(Path);
	wRet = ReadLicenseFile( (s8*)(LPCSTR)csPath );
	if(SUCCESS_LICENSE != wRet) 
	{
		AfxMessageBox("读取License文件失败");
		return ;
	}
	
	s8 asError[100];
	memset( asError, 0, 100);
	if ( !MatchLicense( (LPCSTR)m_csLicenseKey, (LPCSTR)m_csLicenseVersion, (LPCSTR)m_csLicenseType, asError ) )
	{
		AfxMessageBox(asError);
		return;
	}

	int nValuelen = 50;
	char cGetValue[50];
	CString csGetValue;

    //读设备ID
    nValuelen = 50;
    memset( cGetValue, 0, sizeof(cGetValue) );
    if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_MT_DEVICE_ID, (char*)cGetValue, nValuelen) ) )
    {
        AfxMessageBox("读取设备ID失败");
        return;
    }
    m_sDeviceID = cGetValue;
    if ( IsDeviceIDInvalid( m_sDeviceID, '-' ) )
    {
        AfxMessageBox("设备ID错误");
        m_sDeviceID = _T("");
        return;
	}

	//读MC接入能力
	nValuelen = 50;
	memset( cGetValue, 0, sizeof(cGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_MT_ACCESS_NUM, (char*)cGetValue, nValuelen) ) )
	{
		AfxMessageBox("读取MC接入能力失败");
		return;
	}
	m_cboxAccessNum.SelectString( -1, cGetValue );

	//读是否开启终端监控
	nValuelen = 50;
	memset( cGetValue, 0, sizeof(cGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_RUN_KDMMT, (char*)cGetValue, nValuelen) ) )
	{
		AfxMessageBox("读取是否开启终端监控标志失败");
		return;
	}
	CString csRunKDMMt = cGetValue;
    csRunKDMMt.MakeUpper();
	if( csRunKDMMt == STRING_YES )
	{
		m_nRadioRunKDMMt = 0;
	}
	else if( csRunKDMMt == STRING_NO )
	{
		m_nRadioRunKDMMt = 1;
	}
	else
	{
		AfxMessageBox("读取是否开启终端监控标志错误");
		return;
	}
/*
    //读是否启用内置GK
    nValuelen = 50;
    memset( cGetValue, 0, sizeof(cGetValue) );
    if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_MT_RUNGK, (char*)cGetValue, nValuelen) ) )
    {
        AfxMessageBox("读取是否启用内置GK标志失败");
        return;
    }
    CString csRunGk = cGetValue;
    csRunGk.MakeUpper();
    if( csRunGk == STRING_YES )
    {
        CheckDlgButton(IDC_RADIO_RUNGK, BST_CHECKED);
    }
    else if( csRunGk == STRING_NO )
    {
        CheckDlgButton(IDC_RADIO_NOTRUNGK, BST_CHECKED);
    }
    else
    {
        AfxMessageBox("读取是否启用内置GK标志错误");
        return;
	}

    //读是否启用内置代理
    nValuelen = 50;
    memset( cGetValue, 0, sizeof(cGetValue) );
    if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_MT_RUNPROXY, (char*)cGetValue, nValuelen) ) )
    {
        AfxMessageBox("读取是否启用内置代理标志失败");
        return;
    }
    CString csRunProxy = cGetValue;
    csRunProxy.MakeUpper();
    if( csRunProxy == STRING_YES )
    {
        CheckDlgButton(IDC_RADIO_RUNPROXY, BST_CHECKED);
    }
    else if( csRunProxy == STRING_NO )
    {
        CheckDlgButton(IDC_RADIO_NOTRUNPROXY, BST_CHECKED);
    }
    else
    {
        AfxMessageBox("读取是否启用内置代理标志错误");
        return;
	}
*/
    
	//读终端型号
	nValuelen = 50;
	memset( cGetValue, 0, sizeof(cGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_MT_MODAL, (char*)cGetValue, nValuelen) ) )
	{
		AfxMessageBox("读取终端型号失败");
		return;
	}
	m_cboxMtModal.SelectString( -1, cGetValue );

	UpdateData( FALSE );
	
    // guzh [6/8/2007] 显示序列号
    nValuelen = 50;
    memset( cGetValue, 0, sizeof(cGetValue) );
    if( SUCCESS_LICENSE == GetValue((LPSTR)KEY_LICENSE_KEY, (char*)cGetValue, nValuelen) ) 
    {
        SetDlgItemText(IDC_ST_USRSN, cGetValue);
	}    
}

void CT120LicensePage::OnBtnImport() 
{
	// TODO: Add your control notification handler code here
	CString csPath;
	CFileDialog cFileDlg(TRUE);
	
	if(IDOK == cFileDlg.DoModal())
	{
		csPath = cFileDlg.GetPathName();
	}
	else
	{
		return;
	}

	unsigned short wRet = 0;// ReadLicenseFile(Path);
	wRet = ReadLicenseFile( (s8*)(LPCSTR)csPath );
	if(SUCCESS_LICENSE != wRet) 
	{
		AfxMessageBox("读取License文件失败");
		return ;
	}
	
	s8 asError[100];
	memset( asError, 0, 100);
	if ( !MatchLicense( (LPCSTR)m_csLicenseKey, (LPCSTR)m_csLicenseVersion, (LPCSTR)m_csLicenseType, asError ) )
	{
		AfxMessageBox(asError);
		return;
	}

	int nValuelen = 50;
	char cGetValue[50];
	CString csGetValue;

	//读终端接入能力
	nValuelen = 50;
	memset( cGetValue, 0, sizeof(cGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_T120_ACCESS_NUM, (char*)cGetValue, nValuelen) ) )
	{
		AfxMessageBox("读取DCMT终端接入能力失败");
		return;
	}
	m_cboxAccessNum.SelectString( -1, cGetValue );

	//读设备ID
	nValuelen = 50;
	memset( cGetValue, 0, sizeof(cGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_T120_DEVICE_ID, (char*)cGetValue, nValuelen) ) )
	{
		AfxMessageBox("读取设备ID失败");
		return;
	}
	m_csDeviceID = cGetValue;
	if ( IsDeviceIDInvalid( m_csDeviceID, ':' ) )
	{
		AfxMessageBox("设备ID错误");
		m_csDeviceID = _T("");
		return;
	}

	UpdateData( FALSE );

    // guzh [6/8/2007] 显示序列号
    nValuelen = 50;
    memset( cGetValue, 0, sizeof(cGetValue) );
    if( SUCCESS_LICENSE == GetValue((LPSTR)KEY_LICENSE_KEY, (char*)cGetValue, nValuelen) ) 
    {
        SetDlgItemText(IDC_ST_USRSN, cGetValue);
	}    
}

BOOL CMcuLicensePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_dtcExpireDate.EnableWindow(FALSE);
	m_csLicenseKey = "TestKey";
	m_csLicenseVersion = "4.0R4";
	m_csLicenseType = "KDVMCU";
    m_cboxMcuType.SetCurSel(0);    

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CMtLicensePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_csLicenseKey = "TestKey";
	m_csLicenseVersion = "4.0R4";
	m_csLicenseType = "KDVMT";
        
    CheckDlgButton(IDC_RADIO_RUNGK, BST_CHECKED);
    CheckDlgButton(IDC_RADIO_RUNPROXY, BST_CHECKED);
    CheckDlgButton(IDC_RADIO_NOTRUNKDMMT, BST_CHECKED);

    m_cboxAccessNum.SetCurSel(1);
    m_cboxMtModal.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CT120LicensePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_csLicenseKey = "TestKey";
	m_csLicenseVersion = "4.0R4";
	m_csLicenseType = "T120";

    m_cboxAccessNum.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMcuLicensePage::OnBtnRefresh() 
{
	// TODO: Add your control notification handler code here
	m_uAccessNum = 0;
	m_sDeviceID = _T("");
	m_bCheckDate = FALSE;
	m_cboxMcuType.SetCurSel( 0 );
	m_dtcExpireDate.SetTime( &CTime::GetCurrentTime() );
	m_dtcExpireDate.EnableWindow( FALSE );
	UpdateData( FALSE );
}

void CMtLicensePage::OnBtnRefresh() 
{
	// TODO: Add your control notification handler code here
    m_sDeviceID = _T("");
	m_nRadioRunKDMMt = 1;
	m_cboxAccessNum.SetCurSel( 0 );
	m_cboxMtModal.SetCurSel( 0 );
	UpdateData( FALSE );

    CheckDlgButton(IDC_RADIO_RUNGK, BST_CHECKED);
    CheckDlgButton(IDC_RADIO_RUNPROXY, BST_CHECKED);
    CheckDlgButton(IDC_RADIO_NOTRUNGK, BST_UNCHECKED);
    CheckDlgButton(IDC_RADIO_NOTRUNPROXY, BST_UNCHECKED);
}

void CT120LicensePage::OnBtnRefresh() 
{
	// TODO: Add your control notification handler code here
	m_cboxAccessNum.SetCurSel( 0 );
	m_csDeviceID.Empty();
	UpdateData( FALSE );
}

BOOL IsDeviceIDInvalid(CString csDeviceID, s8 chSep)
{
	CString csTemp = csDeviceID;
	int strlenth = 	csTemp.GetLength();
	if( strlenth != 17 )
	{
		return TRUE;
	}

	for( int nLoop=0; nLoop<strlenth; nLoop++ )
	{
		if( (nLoop+1)%3 == 0  )
		{
			if( csTemp[nLoop] != chSep )
				return TRUE;
		}
		else if( 
				!(
					( csTemp[nLoop] >= '0' && csTemp[nLoop] <= '9' ) 
					|| ( csTemp[nLoop] >= 'a' && csTemp[nLoop] <= 'f' )
					|| ( csTemp[nLoop] >= 'A' && csTemp[nLoop] <= 'F' )
				 )
			   )
		{
			return TRUE;
		}
	}

	return FALSE;
}

void WriteFile( CString csLicenseContent, CString csAuthCode, CString csLicenseKey, CString & csErrorCode)
{
	BSTR bstrAuthCode = csAuthCode.AllocSysString();
	BSTR bstrLicenseKey = csLicenseKey.AllocSysString();
	BSTR bstrLicenseContent = csLicenseContent.AllocSysString();
	BSTR bstrRetId = 0;
	BSTR bsDescription = 0;
	
	BOOL32 bWriteFileFail = FALSE;
	CString csMsg;
	if(NULL != pISL) 
	{
		hr = pISL->WriteKey(bstrAuthCode, bstrLicenseKey, bstrLicenseContent, &bstrRetId);
		if( hr == S_FALSE ) 
		{
			bWriteFileFail = TRUE;
		}

		CString csRet = bstrRetId;
		unsigned short wRet = atoi((LPCSTR)csRet);
		hr = pISL->GetErrorDescription(wRet, &bsDescription);
		if( hr == S_FALSE ) 
		{
			csMsg = bWriteFileFail ? "写License文件失败!读错误描述文件失败!" : "操作成功!读错误描述文件失败!";
		}
		else
		{
			csErrorCode = bsDescription;
			csMsg = bWriteFileFail ? ( csErrorCode + '!' + "写License文件失败!" ) : ( csErrorCode + '!' );
		}
		AfxMessageBox( csMsg );
	}

    SysFreeString(bstrAuthCode);
    SysFreeString(bstrLicenseKey);
    SysFreeString(bstrLicenseContent);
    
	return;
}

BOOL MatchLicense( const s8 *pcLicenseKey, const s8 *pcLicenseVersion, const s8 *pcLicenseType, s8 *pcError)
{
	char cGetValue[50];
	unsigned short wRet = 0;
	//读License Key
	int nValuelen = 50;
	memset( cGetValue, 0, sizeof(cGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue((s8*)KEY_LICENSE_KEY, (char*)cGetValue, nValuelen) ) )
	{
		//printf("读取LicenseKey值失败");
		memcpy( pcError, "读取LicenseKey值失败", sizeof("读取LicenseKey值失败") );
		return FALSE;
	}
    // guzh [6/21/2007]  无需检验
    /*
	if( memcmp( pcLicenseKey, cGetValue, strlen( pcLicenseKey ) ) )
	{
		//printf("LicenseKey值不匹配");
		memcpy( pcError, "LicenseKey值不匹配", sizeof("LicenseKey值不匹配") );
		return FALSE;
	}
    */

	//读许可证版本号
	nValuelen = 50;
	memset( cGetValue, 0, sizeof(cGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_LICENSE_VERSION, (char*)cGetValue, nValuelen) ) )
	{
		//printf("读取许可证版本号失败");
		memcpy( pcError, "读取许可证版本号失败", sizeof("读取许可证版本号失败") );
		return FALSE;
	}
	if( memcmp( pcLicenseVersion, cGetValue, strlen( pcLicenseVersion ) ) )
	{
		//printf("许可证版本号不匹配");
		memcpy( pcError, "许可证版本号不匹配", sizeof("许可证版本号不匹配") );
		return FALSE;
	}

	//读许可证类型
	nValuelen = 50;
	memset( cGetValue, 0, sizeof(cGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_LICENSE_TYPE, (char*)cGetValue, nValuelen) ) )
	{
		//printf("读取许可证类型失败");
		memcpy( pcError, "读取许可证类型失败", sizeof("读取许可证类型失败") );
		return FALSE;
	}
	if( memcmp( pcLicenseType, cGetValue, strlen( pcLicenseType ) ) )
	{
		//printf("许可证类型不匹配");
		memcpy( pcError, "许可证类型不匹配", sizeof("许可证类型不匹配") );
		return FALSE;
	}

	return TRUE;
}

