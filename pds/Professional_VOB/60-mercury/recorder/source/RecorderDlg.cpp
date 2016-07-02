/*****************************************************************************
   模块名      : 录像机对话框文件
   文件名      : recorderdlg.cpp
   相关文件    : 
   文件实现功能: 录像机对话框
   作者        : 
   版本        : V4.0  Copyright(C) 2005-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
******************************************************************************/
#include "stdafx.h"
#include "Recorder.h"
#include "RecorderDlg.h"
#include "ConfAgent.h"
#include "evrec.h"
#include "QuitRecDlg.h"
#include "chninst.h"

#pragma once

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define RECORD_HEAD				0x0100
#define PLAY_HEAD				0x0200
#define CONF_HEAD				0x0300
#define CONF_MODIFY_STUB		0x0301
#define CONF_SAVE_STUB			0x0302
#define QUIT_HEAD				0x0400
#define ABOUT_HEAD				0x0500
#define LANG_ENGTOCN			0x0600			// 中英文切换


extern u32     g_adwDevice[MAXNUM_RECORDER_CHNNL+1];		// 记录每个通道的设备号
extern CDBOperate  g_cTDBOperate;							// 数据库类
SEMHANDLE g_hSemQuit;

u16 GetRecChnIndex( u32 dwRecId );

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_cRecVersion;
	CString	m_cVerDeclare;
	//}}AFX_DATA

    BOOL m_bEnglishLang;
public:
    void LangToChange(void);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
    virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_cRecVersion = _T("");
	m_cVerDeclare = _T("");
    m_bEnglishLang = FALSE;
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_STATIC_RECVER, m_cRecVersion);
	DDX_Text(pDX, IDC_STATIC_VERSION_DEC, m_cVerDeclare);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
    
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/*=============================================================================
函 数 名： OnInitDialog
功    能： 初始化对话框
参    数： 
返 回 值： BOOL 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/1/23  4.0			许世林                  创建
=============================================================================*/
BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    
    //enlgish version
    if(m_bEnglishLang)
    {
        m_cRecVersion = "Recorder Version 4.6.1";
        m_cVerDeclare = "Copyright (C) 2001-2010 KDC";
        SetWindowText("About Recorder");
    }
    //chinese version
    else
    {
        m_cRecVersion = "Recorder Version 4.6.1";
        m_cVerDeclare = "Copyright (C) 2001-2010 KDC";   
        SetWindowText("关于 Recorder");
    }
    
    UpdateData(FALSE);

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CRecorderDlg dialog

CRecorderDlg::CRecorderDlg(CWnd* pParent) : CDialog(CRecorderDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRecorderDlg)
	m_nEqpId = 0;
	m_nMcuId = 0;
	m_nMcuPort = 0;
	m_nPlayChnNum = 0;
	m_nRecChnNum = 0;
	m_nRecStartPort = 0;
	m_cDbPassWord = _T("");
	m_cDbAddr = _T("");
	m_cDbUserName = _T("");
	m_cEqpAlias = _T("");
	m_nEqpType = 0;
	m_cMcuAddr = _T("");
	m_cRecAddr = _T("");
	m_cRecMachName = _T("");
	m_cRecSavePath = _T("");
	m_cVodFtpDir = _T("");
	m_cVodFtpPW = _T("");
	m_cVodFtpUserName = _T("");
	m_nMcuIdB = 0;
	m_nMcuPortB = 0;
	m_cMcuAddrB = _T("");
	m_nPlayStartPort = 0;

    m_bOSEn = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRecorderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRecorderDlg)
	DDX_Control(pDX, IDC_STATICSTATUS, m_ctrStatusNote);
	DDX_Text(pDX, IDC_EDIT_EQPID, m_nEqpId);
	DDX_Text(pDX, IDC_EDIT_MCUID, m_nMcuId);
	DDV_MinMaxInt(pDX, m_nMcuId, 0, 65535);
	DDX_Text(pDX, IDC_EDIT_MCUPORT, m_nMcuPort);
	DDX_Text(pDX, IDC_EDIT_PLAYCHNLNUM, m_nPlayChnNum);
	DDX_Text(pDX, IDC_EDIT_RECCHNLNUM, m_nRecChnNum);
	DDX_Text(pDX, IDC_EDIT_RECSTARTPORT, m_nRecStartPort);
	DDX_Text(pDX, IDC_EDIT_DBPASSWORD, m_cDbPassWord);
	DDV_MaxChars(pDX, m_cDbPassWord, 32);
	DDX_Text(pDX, IDC_EDIT_DBADDR, m_cDbAddr);
	DDX_Text(pDX, IDC_EDIT_DBUSERNAME, m_cDbUserName);
	DDV_MaxChars(pDX, m_cDbUserName, 32);
	DDX_Text(pDX, IDC_EDIT_EQPALIAS, m_cEqpAlias);
	DDV_MaxChars(pDX, m_cEqpAlias, 32);
	DDX_Text(pDX, IDC_EDIT_EQPTYPE, m_nEqpType);
	DDX_Text(pDX, IDC_EDIT_MCUADDR, m_cMcuAddr);
	DDV_MaxChars(pDX, m_cMcuAddr, 32);
	DDX_Text(pDX, IDC_EDIT_RECADDR, m_cRecAddr);
	DDV_MaxChars(pDX, m_cRecAddr, 32);
	DDX_Text(pDX, IDC_EDIT_RECMACHNAME, m_cRecMachName);
	DDV_MaxChars(pDX, m_cRecMachName, 32);
	DDX_Text(pDX, IDC_EDIT_RECSAVEPATH, m_cRecSavePath);
	DDV_MaxChars(pDX, m_cRecSavePath, 64);
	DDX_Text(pDX, IDC_EDIT_VODFTPDIR, m_cVodFtpDir);
	DDV_MaxChars(pDX, m_cVodFtpDir, 64);
	DDX_Text(pDX, IDC_EDIT_VODFTPPW, m_cVodFtpPW);
	DDV_MaxChars(pDX, m_cVodFtpPW, 32);
	DDX_Text(pDX, IDC_EDIT_VODFTPUNAME, m_cVodFtpUserName);
	DDX_Text(pDX, IDC_EDIT_MCUIDB, m_nMcuIdB);
	DDV_MinMaxInt(pDX, m_nMcuIdB, 0, 256);
	DDX_Text(pDX, IDC_EDIT_MCUPORTB, m_nMcuPortB);
	DDV_MinMaxInt(pDX, m_nMcuPortB, 0, 65535);
	DDX_Text(pDX, IDC_EDIT_MCUADDRB, m_cMcuAddrB);
	DDX_Text(pDX, IDC_EDIT_STARTPLAYPORT, m_nPlayStartPort);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRecorderDlg, CDialog)
	//{{AFX_MSG_MAP(CRecorderDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_CLICK, IDC_CHNTREE, OnClickChnTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_CHNTREE, OnSelchangedChnTree)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHECK_ISPUBLISH, OnCheckIspublish)
	ON_EN_CHANGE(IDC_EDIT_EQPTYPE, OnChangeEditEqptype)
	ON_EN_CHANGE(IDC_EDIT_MCUID, OnChangeEditMcuid)
	ON_EN_CHANGE(IDC_EDIT_MCUPORT, OnChangeEditMcuport)
	ON_EN_CHANGE(IDC_EDIT_PLAYCHNLNUM, OnChangeEditPlaychnlnum)
	ON_EN_CHANGE(IDC_EDIT_RECCHNLNUM, OnChangeEditRecchnlnum)
	ON_EN_CHANGE(IDC_EDIT_EQPID, OnChangeEditEqpid)
    ON_EN_CHANGE(IDC_EDIT_MCUPORTB, OnChangeEditMcuPortB)
	ON_EN_CHANGE(IDC_EDIT_STARTPLAYPORT, OnChangeEditStartplayport)
	ON_EN_CHANGE(IDC_EDIT_RECSTARTPORT, OnChangeEditRecstartport)
	//}}AFX_MSG_MAP

	ON_MESSAGE (WM_SHOWTASK, OnShowTask)
    ON_MESSAGE( WM_OPENDATABASE, OnOpenDataBase)
    ON_MESSAGE( WM_REC_RESTART, OnRestart)
	ON_MESSAGE( WM_QUITREC, OnRecQuit)
    
//	ON_MESSAGE(WM_RECORD_OUTPUT,    OnOutput)//输出消息
//	ON_MESSAGE(WM_RECORD_SHOWTRC,    OnShowTrc)//输出调试信息消息
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecorderDlg message handlers

BOOL CRecorderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	//ASSERT( IDM_ABOUTBOX == (IDM_ABOUTBOX & 0xFFF0) );
	//ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if( pSysMenu != NULL )
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if( !strAboutMenu.IsEmpty() )
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	// when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
    m_tLangId = GetSystemDefaultLangID();

	// Os不是简体中文版
    if( LANGUAGE_CHINESE != m_tLangId )
    {
        LanguageToEnglish();
        m_bOSEn = TRUE;
        AfxGetMainWnd()->SetWindowText("KDV Recorder Server");
    }

    ::CoInitialize( NULL );

    // 显示任务栏图标
	ChangeTopic(TRUE);
    
    InitTree();
    
    // 初始化界面
    PostMessage( WM_REC_RESTART );

    // 设置刷新界面定时器
    SetTimer(REFRSH_CHNL_TIMER, 1000, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRecorderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
        dlgAbout.m_bEnglishLang = m_bOSEn;
		dlgAbout.DoModal();
	}
	else
	{
		if( nID == SC_MINIMIZE )
		{
            // 最小化则隐藏
			ShowWindow(SW_HIDE);
		}
        else
        {
            CDialog::OnSysCommand(nID, lParam);
        }
		
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRecorderDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRecorderDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/*====================================================================
	函数  : OnRestart
	功能  : 重新启动界面
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CRecorderDlg::OnRestart(WPARAM wParam, LPARAM lParam)
{
    Init();
    // guzh [8/29/2006] 这里不必刷新树。因为底层录像库没有重新启动
    // 通道数也没有变化。上层提示用户自己重启即可
    // 否则新的树控件会导致显示与底层不一致
    //InitTree();

    if( g_tCfg.m_bMediaServer )  // 打开数据库
	{
        SetTimer( OPEN_DATABASE_TIMER, 1, NULL );
	}
    else
	{
		if ( LANGUAGE_CHINESE == m_tLangId )
		{
			m_ctrStatusNote.SetWindowText("    >>>>>>>> 本录像机无发布功能 ! ");
			UpdateData(FALSE);
		}
		else
		{
			m_ctrStatusNote.SetWindowText("    >>>> The recorder NOT support publishing function!");
		}
    }
    
    SetWindowText(g_tCfg.m_szAlias);
    
    m_bSetFlag = FALSE;

    ChangeTopic(FALSE);

    return;
}

/*====================================================================
	函数  : OnRecQuit
	功能  : 退出录像机
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CRecorderDlg::OnRecQuit(WPARAM wParam, LPARAM lParam)
{
	Quit( TRUE );
}

/*====================================================================
	函数  : InitTree
	功能  : 初始化界面树
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CRecorderDlg::InitTree()
{
    // 防止重入，会导致异常
    static long lCalling = FALSE;
	if( ::InterlockedExchange( &lCalling, TRUE ) )
	{
		return;     //该函数已被调用返回
	}

	s32 nChIdx;
	s8 achInfo[32] = {0};
	HTREEITEM hTreeItemRec;
	HTREEITEM hTreeItemPlay;
	HTREEITEM hTreeItemConf;
	HTREEITEM hTreeItemQuit;
	TV_INSERTSTRUCT TreeCtrlItem;
	CTreeCtrl * pTreeCtr = (CTreeCtrl *)GetDlgItem( IDC_CHNTREE );
	ASSERT( pTreeCtr );

    // 先取消选择，否则会导致删除时自动切换语言
	if (!pTreeCtr) return;

    pTreeCtr->SelectItem(NULL);
    pTreeCtr->DeleteAllItems();

	TreeCtrlItem.hParent = TVI_ROOT;
	TreeCtrlItem.hInsertAfter = TVI_LAST;
	TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM;
    if(m_bOSEn)
    {
        TreeCtrlItem.item.pszText = "Record Channels";
    }
    else
	{
        TreeCtrlItem.item.pszText = "录像通道";
    }
	TreeCtrlItem.item.lParam  = RECORD_HEAD;
	hTreeItemRec = pTreeCtr->InsertItem(&TreeCtrlItem);

	for( nChIdx = 1 ;nChIdx <= g_tCfg.m_byRecChnNum ;nChIdx++ )
	{
        if(m_bOSEn)
		{
            sprintf( achInfo ,"Record Channel %d", nChIdx);
        }
        else
		{
            sprintf( achInfo ,"第%d通道",nChIdx);
        }
		TreeCtrlItem.hParent = hTreeItemRec;
		TreeCtrlItem.item.pszText = achInfo;
		TreeCtrlItem.item.lParam  = RECORD_HEAD+nChIdx;
		pTreeCtr->InsertItem(&TreeCtrlItem);
	}

	TreeCtrlItem.hParent = TVI_ROOT;
	TreeCtrlItem.hInsertAfter = TVI_LAST;
	TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM;
    if(m_bOSEn)
	{
        TreeCtrlItem.item.pszText = "Play Channels";
    }
    else
	{
        TreeCtrlItem.item.pszText = "放像通道";
    }
	TreeCtrlItem.item.lParam = PLAY_HEAD;
	hTreeItemPlay = pTreeCtr->InsertItem(&TreeCtrlItem);

	for( nChIdx = (g_tCfg.m_byRecChnNum + 1); nChIdx <= g_tCfg.m_byTotalChnNum; nChIdx++ )
	{
        if(m_bOSEn)
		{
            sprintf( achInfo ,"Play Channel %d",nChIdx - g_tCfg.m_byRecChnNum);
        }
        else
		{
            sprintf( achInfo ,"第%d通道",nChIdx - g_tCfg.m_byRecChnNum);
        }
		TreeCtrlItem.hParent = hTreeItemPlay;
		TreeCtrlItem.item.pszText = achInfo;
		TreeCtrlItem.item.lParam  =  PLAY_HEAD+nChIdx;
		pTreeCtr->InsertItem(&TreeCtrlItem);
	}

/*  
	comment kdv flag .20040209
	TreeCtrlItem.hParent = TVI_ROOT;
	TreeCtrlItem.hInsertAfter = TVI_LAST;
	TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM;
	TreeCtrlItem.item.pszText = " 关于";
	TreeCtrlItem.item.lParam = ABOUT_HEAD;
	hTreeItemAbout = pTreeCtr->InsertItem(&TreeCtrlItem);
*/
	
    TreeCtrlItem.hParent	  = TVI_ROOT;
	TreeCtrlItem.hInsertAfter = TVI_LAST;
	TreeCtrlItem.item.mask	  = TVIF_TEXT | TVIF_PARAM;
    if(m_bOSEn)
	{
        TreeCtrlItem.item.pszText = " Settings";
    }
    else 
	{
        TreeCtrlItem.item.pszText = " 配置参数";
    }
	TreeCtrlItem.item.lParam = CONF_HEAD;
	hTreeItemConf = pTreeCtr->InsertItem(&TreeCtrlItem);
    //InsetItem 修改  add by libo
	TreeCtrlItem.hParent = hTreeItemConf;
    if(m_bOSEn)
	{
        TreeCtrlItem.item.pszText = "Edit";
    }
    else 
	{
        TreeCtrlItem.item.pszText = "修改";
    }
	TreeCtrlItem.item.lParam  =  CONF_MODIFY_STUB;
	pTreeCtr->InsertItem(&TreeCtrlItem);
    //InsetItem 保存  add by libo
	TreeCtrlItem.hParent = hTreeItemConf;
    if(m_bOSEn)
	{
        TreeCtrlItem.item.pszText = "Save";
    }
    else
	{ 
        TreeCtrlItem.item.pszText = "保存";
    }
	TreeCtrlItem.item.lParam  =  CONF_SAVE_STUB;
	pTreeCtr->InsertItem(&TreeCtrlItem);

    // 只有中文版才支持语言切换
    if (m_tLangId == LANGUAGE_CHINESE)
    {
        TreeCtrlItem.hParent	  = TVI_ROOT;
	    TreeCtrlItem.hInsertAfter = TVI_LAST;
	    TreeCtrlItem.item.mask    = TVIF_TEXT | TVIF_PARAM;
        if(m_bOSEn)
	    {
            TreeCtrlItem.item.pszText = " Chinese Version";
        }
        else 
	    {
            TreeCtrlItem.item.pszText = " 英文版";
        }
	    TreeCtrlItem.item.lParam = LANG_ENGTOCN;
	    hTreeItemQuit = pTreeCtr->InsertItem(&TreeCtrlItem);
    }
    else
    {
        // 英文os环境只支持显示英文
    }

	TreeCtrlItem.hParent = TVI_ROOT;
	TreeCtrlItem.hInsertAfter = TVI_LAST;
	TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM;
    if(m_bOSEn)
	{
        TreeCtrlItem.item.pszText = " Exit";
    }
    else 
	{
        TreeCtrlItem.item.pszText = " 退出";
    }
	TreeCtrlItem.item.lParam = QUIT_HEAD;
	hTreeItemQuit = pTreeCtr->InsertItem(&TreeCtrlItem);

	pTreeCtr->Expand(hTreeItemRec,TVE_EXPAND);
	pTreeCtr->Expand(hTreeItemPlay,TVE_EXPAND);
	pTreeCtr->Expand(hTreeItemConf,TVE_EXPAND);
//	pTreeCtr->Expand(hTreeItemAbout,TVE_EXPAND);
	pTreeCtr->Expand(hTreeItemQuit,TVE_EXPAND);

	pTreeCtr->SelectItem( hTreeItemConf);

    ::InterlockedExchange( &lCalling, FALSE );

}

void CRecorderDlg::OnClickChnTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint pt;
	BOOL bRet = GetCursorPos(&pt);
	CTreeCtrl* pTreeCtrl = ( CTreeCtrl* )GetDlgItem( IDC_CHNTREE );
	pTreeCtrl->ScreenToClient(&pt);
	
	UINT uFlags;	
	HTREEITEM hItem = pTreeCtrl->HitTest(pt, &uFlags);
	
	if ((hItem != NULL) && (TVHT_ONITEM & uFlags))
	{
		//pTreeCtrl->Select(hItem, TVGN_CARET);
		
		CString cstrItemStr;
		cstrItemStr = pTreeCtrl->GetItemText( hItem );
		
		if( cstrItemStr == " 退出" || cstrItemStr == " Exit")
		{
			Quit();
		}
	}
	
	*pResult = 0;
}


/*=============================================================================
  函 数 名： OnSelchangedChnTree
  功    能： 树操作
  算法实现： 
  全局变量： 
  参    数： NMHDR* pNMHDR
             LRESULT* pResult
  返 回 值： void 
=============================================================================*/
void CRecorderDlg::OnSelchangedChnTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
    static long lCalling = FALSE;
	if( ::InterlockedExchange( &lCalling, TRUE ) )
	{
		return;     //该函数已被调用返回
	}
	
	CWnd* pWnd = GetDlgItem(IDC_INFOCONSOLE);
	s8 achInfo[512];
	
	s8 achStatus[50] = {0};
	CWnd* pWndPrompt = GetDlgItem(IDC_STATICSTATUS);
	
	m_dwShowInfoType = pNMTreeView->itemNew.lParam;
	
    //add by libo
    if (m_dwShowInfoType == CONF_MODIFY_STUB)
    {
        // 进入修改状态
        m_bModifyConfig = TRUE;
    }
    else
    {
		BOOL bSaveIsSuccess = TRUE;
        HideModifyConfig();
        if ((m_dwShowInfoType == CONF_SAVE_STUB) && (m_bModifyConfig == TRUE))
        {
            // 关闭数据库
            BOOL32 bMediaServer = g_tCfg.m_bMediaServer;
			UpdateData( TRUE );
            bSaveIsSuccess = SaveModifyConfig();
			if( m_bModified && bSaveIsSuccess )
            {
                OspPrintf(TRUE, FALSE, "Modified configure\n");
                if( bMediaServer )
                {
                    g_cTDBOperate.CloseDB();
                    g_tCfg.m_bOpenDBSucceed = FALSE;
                }
				
                RestartRecorder(GetSafeHwnd());
                
                // 提示用户关闭录像机已生效
                if (m_bCriticalModify)
                {
                    int bRet = IDNO;
                    if(m_bOSEn)
                    {
                        bRet = AfxMessageBox("The changes you made must restart to take effect. Shutdown recorder server now?",
							MB_YESNO);
                    }
                    else
                    {
                        bRet = AfxMessageBox("您所做的修改必须重新启动录像机后生效，现在就关闭录像机吗？",
							MB_YESNO);
                    }
                    if (IDYES == bRet)
                    {
                        Quit(TRUE);
                        return;
                    }
                }					
			}
			else
			{
                //ShowConfig();
				OspPrintf(TRUE, FALSE, "Not Modified configure\n");
			}
        }
        m_bModifyConfig = FALSE;
		
		if ( (m_dwShowInfoType == CONF_SAVE_STUB) && !bSaveIsSuccess )
		{
			m_bModifyConfig = TRUE;
			// 重新选中“修改”
			CTreeCtrl * pTreeCtr = (CTreeCtrl *)GetDlgItem( IDC_CHNTREE );
			if ( pTreeCtr != NULL )
			{
				HTREEITEM hRootItem = pTreeCtr->GetRootItem();
				HTREEITEM hNextRootItem;
				CString csItemText;
				CString csCmp1;
				CString csCmp2;
				if ( m_bOSEn )
				{
					csCmp1 = " Setting";
					csCmp2 = "Edit";
				}
				else
				{
					csCmp1 = " 配置参数";
					csCmp2 = "修改";
				}				
				while( hRootItem != NULL )
				{
					csItemText = pTreeCtr->GetItemText( hRootItem );
					if (csItemText.Compare(csCmp1)  == 0 )
					{
						HTREEITEM hChildItem = pTreeCtr->GetChildItem( hRootItem );
						HTREEITEM hNextItem;
						while( hChildItem != NULL )
						{
							csItemText = pTreeCtr->GetItemText( hChildItem );
							if ( csItemText.Compare(csCmp2) == 0)
							{
								pTreeCtr->Select( hChildItem, TVGN_CARET|TVGN_DROPHILITE );
								break;
							}
							hNextItem = pTreeCtr->GetNextSiblingItem( hChildItem );
							hChildItem = hNextItem;
						}
						break;
					}
					hNextRootItem = pTreeCtr->GetNextSiblingItem( hRootItem );
					hRootItem = hNextRootItem;
				}
			}
		}
    }
	
    switch( m_dwShowInfoType ) 
	{
    case CONF_HEAD:
    case CONF_MODIFY_STUB:   //add by libo
    case CONF_SAVE_STUB:   //add by libo
        if(m_bOSEn)
		{
            sprintf(achStatus, "  Configuration Parameters");
        }
        else 
		{
            sprintf(achStatus, "  显示配置参数");
        }
		pWndPrompt->SetWindowText(achStatus);
		ShowConfig();
		break;
	case QUIT_HEAD:
        if(m_bOSEn)
		{
            sprintf(achStatus, "  Exit the Recorder");
        }
        else
		{
            sprintf(achStatus, "  退出录像机");
        }
		pWndPrompt->SetWindowText(achStatus);
		
		//		Quit();
		break;
	case RECORD_HEAD:
        if(m_bOSEn)
		{
            sprintf(achStatus, " Record Channel Status");
        }
        else
		{
            sprintf(achStatus, "  录像通道状态");
        }
		pWndPrompt->SetWindowText(achStatus);
        if(m_bOSEn)
		{
            sprintf( achInfo ,"\n\n\tThe sum of record channels: %d\n",g_tCfg.m_byRecChnNum );
        }
        else
		{
            sprintf( achInfo ,"\n\n\t当前录像通道数 %d\n", g_tCfg.m_byRecChnNum );
        }
		pWnd->SetWindowText( achInfo);
		break;
	case PLAY_HEAD:
        if(m_bOSEn)
		{
            sprintf(achStatus, "  Play Channel Status");
        }
        else
		{
            sprintf(achStatus, "  放像通道状态");
        }
		pWndPrompt->SetWindowText(achStatus);
        
        if(m_bOSEn)
		{
            sprintf( achInfo ,"\n\n\tThe sum of play channels: %d\n", g_tCfg.m_byPlayChnNum );
        }
        else 
		{
            sprintf( achInfo ,"\n\n\t当前放像通道数 %d\n", g_tCfg.m_byPlayChnNum);
        }
		pWnd->SetWindowText( achInfo);
		break;
		
    case LANG_ENGTOCN:
        if( LANGUAGE_CHINESE != m_tLangId )
        {
            // do nothing, 英文版中切换为中文版，会出现乱码，暂时屏蔽
        }
        else
		{
            if(m_bOSEn)
            {
                m_bOSEn = FALSE;
                InitTree();
                ChangeMenuText();
                ShowConfig();
            }
            else 
            {
                m_bOSEn = TRUE;
                InitTree();
                ChangeMenuText();
                ShowConfig();
            }
            // guzh [4/24/2007] 清除提示
            GetDlgItem(IDC_STATICSTATUS)->SetWindowText("");
        }
		/*	comment kdv flag . 20040209 by zmy
		case ABOUT_HEAD:
		sprintf( achInfo ,"\n\n\n\n%s\n\n%s\n%s%s\n",
		"  苏州科达通信技术发展有限公司",
		"  录放像服务器",
		"  版本: ",VER_RECORDER);
		pWnd->SetWindowText( achInfo);
		break;
		*/
    default:
		break;
    }
	/*if ( !m_bModified )
    {
	if( IsDlgButtonChecked(IDC_CHECK_ISPUBLISH) )
	{
	ChangeServiceInfo(TRUE);
	}
	else
	{
	ChangeServiceInfo(FALSE);
	}
	}*/
    ::InterlockedExchange(&lCalling, FALSE);
	*pResult = 0;
}

/*=============================================================================
  函 数 名： ChangeTopic
  功    能： 加载托盘
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CRecorderDlg::ChangeTopic(BOOL32 bNew)
{
    if (bNew)
    {
        m_tnidTray.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	    m_tnidTray.hWnd = GetSafeHwnd();
	    m_tnidTray.uID = IDR_MAINFRAME;
	    m_tnidTray.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP ;
	    m_tnidTray.uCallbackMessage = WM_SHOWTASK;     //自定义的消息名称
	    m_tnidTray.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));

        strcpy(m_tnidTray.szTip, g_tCfg.m_szAlias);
    
        /*
        if(FALSE == m_bOSEn)
        {
            strcpy(m_tnidTray.szTip, "KDV 录放像服务器");    //信息提示条
        }
        else
        {
            strcpy(m_tnidTray.szTip, "Server of KDV's recorder and player"); 
        }
        */

        Shell_NotifyIcon(NIM_ADD, &m_tnidTray); 
    }
    else
    {
        strcpy(m_tnidTray.szTip, g_tCfg.m_szAlias);
        Shell_NotifyIcon(NIM_MODIFY, &m_tnidTray); 
    }

    return;
}

void CRecorderDlg::ChangeMenuText(void)
{
    AfxGetMainWnd()->SetWindowText(g_tCfg.m_szAlias);
    /*
    if( FALSE == m_bOSEn )
    {
        if ( 0 != strlen(g_tCfg.m_szAlias) )
        {
            AfxGetMainWnd()->SetWindowText("KDV 录放像服务器");
        }
    }
    else
    {
        if ( 0 != strlen(g_tCfg.m_szAlias) )
        {
            AfxGetMainWnd()->SetWindowText("KDV Record Server");
        }
    }*/
}

/*====================================================================
	函数  : ShowConfig
	功能  : 显示配置信息
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CRecorderDlg::ShowConfig()
{
	s8 achShowStr[1024];
    memset(achShowStr, 0, sizeof(achShowStr));

	static long lCalling = FALSE;

	//CWnd* pWndPrompt = GetDlgItem(IDC_STATICSTATUS);
	CWnd* pWnd = GetDlgItem(IDC_INFOCONSOLE);
	
	if( pWnd == NULL )
	{
		return;
	}
	
	if( ::InterlockedExchange( &lCalling, TRUE ) )
	{
		return;     //该函数已被调用返回
	}

    //add by libo
    if (TRUE == m_bModifyConfig )
    {
        pWnd->MoveWindow(170, 13, 0, 0);
        ::InterlockedExchange( &lCalling, FALSE );
        ShowModifyConfig();
        return;
    }

    if(m_bOSEn)
    {
        GetEnglishConfigureString(achShowStr);
    }
    else
    {
        GetChineseConfigureString(achShowStr);
    }
    
	pWnd->SetWindowText(achShowStr);

    //pWnd = GetDlgItem( IDC_EDIT_RECMACHNAME );
    //pWnd->EnableWindow( FALSE );


//     pWnd = GetDlgItem( IDC_EDIT_STARTPLAYPORT );
//     pWnd->EnableWindow( FALSE );
//     
// 	pWnd = GetDlgItem(IDC_EDIT_DBADDR);
// 	pWnd->SetWindowText("");
// 	pWnd->EnableWindow(FALSE);
// 	
// 	pWnd = GetDlgItem(IDC_EDIT_DBUSERNAME);
// 	pWnd->SetWindowText("");
// 	pWnd->EnableWindow(FALSE);
// 	
// 	pWnd = GetDlgItem(IDC_EDIT_DBPASSWORD);
// 	pWnd->SetWindowText("");
// 	pWnd->EnableWindow(FALSE);
// 	
// 	pWnd = GetDlgItem(IDC_EDIT_VODFTPDIR);
// 	pWnd->SetWindowText("");
// 	pWnd->EnableWindow(FALSE);
// 	
// 	pWnd = GetDlgItem(IDC_EDIT_VODFTPUNAME);
// 	pWnd->SetWindowText("");
// 	pWnd->EnableWindow(FALSE);
// 	
// 	pWnd = GetDlgItem(IDC_EDIT_VODFTPPW);
// 	pWnd->SetWindowText("");
// 	pWnd->EnableWindow(FALSE);
       
    pWnd = NULL;
    
	::InterlockedExchange(&lCalling, FALSE);
	return;

}

void CRecorderDlg::ChangeStringFormat(LPSTR pchDst, LPSTR pchStr, u8 bNum)
{
	s8 achConf[KDV_MAX_PATH];
	memset(achConf, 0, KDV_MAX_PATH);
	
	u8 bIsChar = 0;//奇数个
	u8 bCharNum = 0;//字符串中非汉字字符的个数
	
	if(strlen(pchStr) <= bNum)
	{
		strcat(pchDst, pchStr);
	}
	else
	{
		u8 j, k;
		
		for(k = 0; k < bNum && pchStr[k] != '\0'; ++k)
		{
			if(pchStr[k] > 0)
			{
				bCharNum++;
			}
		}
		
		if(bCharNum % 2 == 0)
		{
			bIsChar = 1;//偶数个
		}
		
		for(j = 0, k = 0; k < KDV_MAX_PATH && pchStr[k] != '\0'; ++k, ++j)
		{
			if(!bIsChar && k == bNum)
			{
				achConf[j] = '\n';
				++j;
			}
            if(bIsChar && k == bNum-1)
			{
				achConf[j] = '\n';
				++j;
			}
			
			achConf[j] = pchStr[k];	
		}
		
		strcat(pchDst, achConf);
	}
	strcat(pchDst, "\n");
	
	return ;
}

void CRecorderDlg::GetChineseConfigureString(char* pchStr)
{

    if(NULL == pchStr)
    {
        return;
    }
    s8 achConf[KDV_MAX_PATH];
    memset(achConf, 0, sizeof(achConf));

    struct in_addr in;
	sprintf(achConf, "\n\t录像机配置信息\n");
	strcat( pchStr, achConf);

	in.s_addr = g_tCfg.m_dwRecIpAddr;
	sprintf(achConf, "\n录像地址     : %s\n", inet_ntoa(in ));
	strcat( pchStr, achConf);

	//sprintf(achConf, "外设别名     : %s\n", g_tCfg.m_szAlias);
	//strcat( pchStr, achConf);

	strcat( pchStr, "外设别名     : ");
	ChangeStringFormat(pchStr, g_tCfg.m_szAlias, 31);

	//sprintf(achConf, "外设类型     : %d\n", g_tCfg.m_byEqpType );
	//strcat( pchStr, achConf);

	sprintf(achConf, "外设ID       : %d\n", g_tCfg.m_byEqpId );
	strcat( pchStr, achConf);

	//sprintf(achConf, "MCU ID       : %d\n", g_tCfg.m_wMcuId );
	//strcat( pchStr, achConf);

	in.s_addr = g_tCfg.m_dwMcuIpAddr;
	sprintf(achConf, "MCU 地址     : %s\n", inet_ntoa(in ));
	strcat( pchStr, achConf);

	sprintf(achConf, "MCU通信端口  : %d\n", g_tCfg.m_wMcuConnPort);
    strcat( pchStr, achConf);
    
	/*
    sprintf(achConf, "Mcu.B ID     : %d\n", g_tCfg.m_wMcuIdB);
    strcat(pchStr, achConf);

    in.s_addr = g_tCfg.m_dwMcuIpAddrB;
    sprintf(achConf, "Mcu.B 地址   : %s\n", inet_ntoa(in));
    strcat(pchStr, achConf);

    sprintf(achConf, "Mcu.B 端口   : %d\n", g_tCfg.m_wMcuConnPortB);
    strcat(pchStr, achConf);
    */
    
    sprintf(achConf, "录像通道数   : %d\n", g_tCfg.m_byRecChnNum );
    strcat(pchStr, achConf);

	sprintf(achConf, "放像通道数   : %d\n", g_tCfg.m_byPlayChnNum );
	strcat(pchStr, achConf);

	sprintf(achConf, "录像起始端口 : %d\n", g_tCfg.m_wRecStartPort);
	strcat(pchStr, achConf);

    sprintf(achConf, "放像起始端口 : %d\n", g_tCfg.m_wPlayStartPort);
	strcat(pchStr, achConf);

	//sprintf(achConf, "录像存储路径 : %s\n", g_tCfg.m_szRecordPath);
    //strcat(pchStr, achConf);

	strcat( pchStr, "录像存储路径 : ");
	ChangeStringFormat(pchStr, g_tCfg.m_szRecordPath, 31);

    //sprintf(achConf, "录像机器名   : %s\n", g_tCfg.m_szMachineName);
	//strcat(pchStr, achConf);

	strcat( pchStr, "录像机器名   : ");
	ChangeStringFormat(pchStr, g_tCfg.m_szMachineName, 31);

    if( g_tCfg.m_bMediaServer )
    {
        sprintf(achConf, "\r\n\r\n");
        strcat(pchStr, achConf);

        sprintf(achConf, "数据服务器 IP: %s\n", g_tCfg.m_achDBHostIP);
        strcat(pchStr, achConf);

        sprintf(achConf, "用户名       : %s\n", g_tCfg.m_achDBUsername);
        strcat(pchStr, achConf);

        u8 achPassWord[64];
        memset(achPassWord, '\0', sizeof(achPassWord));
        memset(achPassWord, '*', strlen(g_tCfg.m_achDBPassword));    // 密码用星号
        sprintf(achConf, "密码         : %s\n", achPassWord);
        strcat(pchStr, achConf);

        sprintf(achConf, "VOD MMS目录  : %s\n", g_tCfg.m_achVodFtpPatch);
        strcat(pchStr, achConf);

        sprintf(achConf, "VOD MMS用户名: %s\n", g_tCfg.m_achFTPUsername);
        strcat(pchStr, achConf);

        memset(achPassWord, '\0', sizeof(achPassWord));
        memset(achPassWord, '*', strlen(g_tCfg.m_achFTPPassword));    // 密码用星号
        sprintf(achConf, "VOD MMS 密码 : %s\n", achPassWord);
        strcat(pchStr, achConf);
    }
	else
	{
		static u32 dwNote = 0;
		dwNote++;
		dwNote &= 0x1;
		sprintf(achConf, "\n\n\n\n >>>>  该录像机无发布功能 !\n");
		strcat(pchStr, achConf);
	}


    if (g_tCfg.m_bCppUnitTest)
	{
		in.s_addr = g_tCfg.m_dwCppUnitIpAddr;
		sprintf(achConf, "\n\nCPPunit 测试地址: %s\n", inet_ntoa(in));
		strcat(pchStr, achConf);

		sprintf(achConf, "CPPunit 测试端口: %d\n", g_tCfg.m_wCppUnitPort);
		strcat(pchStr, achConf);

        if (g_tCfg.m_wTelnetPort != 0)
	    {
		    sprintf(achConf, "Telnet 跟踪端口 : %d\n", g_tCfg.m_wTelnetPort);
		    strcat(pchStr, achConf);
	    }
	}
    return;
}

void CRecorderDlg::GetEnglishConfigureString(char* pchStr)
{
    if(NULL == pchStr)
    {
        return;
    }
    
    struct in_addr in;
    s8 achConf[KDV_MAX_PATH];
    memset(achConf, 0, sizeof(achConf));

	sprintf(achConf, "\n\tRecorder Settings\n");
	strcat( pchStr, achConf);

	in.s_addr = g_tCfg.m_dwRecIpAddr;
	sprintf(achConf, "Address        : %s\n", inet_ntoa(in ));
	strcat( pchStr, achConf);

	//sprintf(achConf, "Alias          : %s\n",g_tCfg.m_szAlias);
	//strcat( pchStr, achConf);

	strcat( pchStr, "Alias          : ");
	ChangeStringFormat(pchStr, g_tCfg.m_szAlias, 29);

	//sprintf(achConf, "Category       : %d\n",g_tCfg.m_byEqpType );
	//strcat( pchStr, achConf);

	sprintf(achConf, "ID             : %d\n",g_tCfg.m_byEqpId );
	strcat( pchStr, achConf);

	//sprintf(achConf, "MCU ID         : %d\n",g_tCfg.m_wMcuId );
	//strcat( pchStr, achConf);

	in.s_addr = g_tCfg.m_dwMcuIpAddr;
	sprintf(achConf, "MCU Address    : %s\n", inet_ntoa(in ));
	strcat( pchStr, achConf);

	sprintf(achConf, "MCU Port       : %d\n", g_tCfg.m_wMcuConnPort);
    strcat( pchStr, achConf);
    
	/*
    sprintf(achConf, "MCU ID         : %d\n", g_tCfg.m_wMcuIdB);
    strcat(pchStr, achConf);

    in.s_addr = g_tCfg.m_dwMcuIpAddrB;
    sprintf(achConf, "MCU Address    : %s\n", inet_ntoa(in));
    strcat(pchStr, achConf);

    sprintf(achConf, "MCU Port       : %d\n\n", g_tCfg.m_wMcuConnPortB);
    strcat(pchStr, achConf);
    */
    sprintf(achConf, "Record Channels: %d\n", g_tCfg.m_byRecChnNum );
    strcat(pchStr, achConf);
    
	sprintf(achConf, "Play Channels  : %d\n", g_tCfg.m_byPlayChnNum );
	strcat(pchStr, achConf);

	sprintf(achConf, "Recording Port : %d\n", g_tCfg.m_wRecStartPort);
	strcat(pchStr, achConf);

    sprintf(achConf, "Playing Port   : %d\n", g_tCfg.m_wPlayStartPort);
    strcat(pchStr, achConf);

	//sprintf(achConf, "File Location  : %s\n\n", g_tCfg.m_szRecordPath);
    //strcat(pchStr, achConf);

	strcat( pchStr, "File Location  : ");
	ChangeStringFormat(pchStr, g_tCfg.m_szRecordPath, 29);

    //sprintf(achConf, "Computer Name  : %s\n", g_tCfg.m_szMachineName);  
	//strcat(pchStr, achConf);

	strcat( pchStr, "Computer Name  : ");
	ChangeStringFormat(pchStr, g_tCfg.m_szMachineName, 29);

    if( g_tCfg.m_bMediaServer )
    {
        sprintf(achConf, "\r\n\r\nDB Address     : %s\n", g_tCfg.m_achDBHostIP);
        strcat(pchStr, achConf);

        sprintf(achConf, "DB Username    : %s\n", g_tCfg.m_achDBUsername);
        strcat(pchStr, achConf);

        u8 achPassWord[64];
        memset(achPassWord, '\0', sizeof(achPassWord));
        memset(achPassWord, '*', 12);    // 密码用12个星号
        sprintf(achConf, "DB Password    : %s\n", achPassWord);
        strcat(pchStr, achConf);

        sprintf(achConf, "VOD Location   : %s\n", g_tCfg.m_achVodFtpPatch);
        strcat(pchStr, achConf);

        sprintf(achConf, "VOD Username   : %s\n", g_tCfg.m_achFTPUsername);
        strcat(pchStr, achConf);

        memset(achPassWord, '\0', sizeof(achPassWord));
        memset(achPassWord, '*', 12);    // 密码用12个星号
        sprintf(achConf, "VOD Password   : %s\n", achPassWord);
        strcat(pchStr, achConf);
    }
	else
	{
		static u32 dwNote = 0;
		dwNote++;
		dwNote &= 0x1;
		sprintf(achConf, "\n\n\n >>>>  No publishing function!\n");
		strcat(pchStr, achConf);
	}


    if (g_tCfg.m_bCppUnitTest)
	{
		in.s_addr = g_tCfg.m_dwCppUnitIpAddr;
		sprintf(achConf, "\n\nCPPunit Address: %s\n", inet_ntoa(in));
		strcat(pchStr, achConf);

		sprintf(achConf, "CPPunit Port: %d\n", g_tCfg.m_wCppUnitPort);
		strcat(pchStr, achConf);

        if (g_tCfg.m_wTelnetPort != 0)
	    {
		    sprintf(achConf, "Telnet Port: %d\n", g_tCfg.m_wTelnetPort);
		    strcat(pchStr, achConf);
	    }
	}
    return;
}

/*====================================================================
	函数  : HideModifyConfig
	功能  : 隐藏配置信息
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    04/12/31    3.6         libo          create
====================================================================*/
void CRecorderDlg::HideModifyConfig()
{
	s8 achShowStr[1024]={0};
	s8 achConf[64]={0};
	//struct in_addr in; 
	static long lCalling = FALSE;

	CWnd* pWnd = GetDlgItem(IDC_INFOCONSOLE);
	
	if (pWnd == NULL)
	{
		return;
	}
	
	if (::InterlockedExchange(&lCalling, TRUE))
	{
		return;     //该函数已被调用返回
	}

    pWnd->MoveWindow(170, 13, 280, 445);

    //录像地址
    pWnd = GetDlgItem(IDC_EDIT_RECADDR);	
	if( pWnd == NULL )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_RECADDR) error!\n");
	}
    else
    {
        pWnd->EnableWindow(FALSE);
    }

    //外设别名
    pWnd = GetDlgItem(IDC_EDIT_EQPALIAS);	
	if( pWnd == NULL )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_EQPALIAS) error!\n");
	}
    else
    {
        pWnd->EnableWindow(FALSE);
    }

    //外设类型
    pWnd = GetDlgItem(IDC_EDIT_EQPTYPE);	
	if( pWnd == NULL )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_EQPTYPE) error!\n");
	}
    else
    {
        pWnd->EnableWindow(FALSE);
    }

    //外设ID
    pWnd = GetDlgItem(IDC_EDIT_EQPID);	
	if( pWnd == NULL )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_EQPID) error!\n");
	}
    else
    {
        pWnd->EnableWindow(FALSE);
    }

    //MCU ID
    pWnd = GetDlgItem(IDC_EDIT_MCUID);	
	if( pWnd == NULL )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_MCUID) error!\n");
	}
    else
    {
        pWnd->EnableWindow(FALSE);
    }

    //MCU 地址
    pWnd = GetDlgItem(IDC_EDIT_MCUADDR);	
	if( pWnd == NULL )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_MCUADDR) error!\n");
	}
    else
    {
        pWnd->EnableWindow(FALSE);
    }

    //MCU通信端口
    pWnd = GetDlgItem(IDC_EDIT_MCUPORT);	
	if( pWnd == NULL )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_MCUPORT) error!\n");
	}
    else
    {
        pWnd->EnableWindow(FALSE);
    }

	/*
    pWnd = GetDlgItem(IDC_EDIT_MCUPORTB);
    if( NULL != pWnd)
    {
        pWnd->EnableWindow(FALSE);
    }

    pWnd = GetDlgItem(IDC_EDIT_MCUIDB);
    if(NULL != pWnd)
    {
        pWnd->EnableWindow(FALSE);
    }

    pWnd = GetDlgItem(IDC_EDIT_MCUADDRB);
    if(NULL != pWnd)
    {
        pWnd->EnableWindow(FALSE);
    }
	*/

    
    //放像通道数
    pWnd = GetDlgItem(IDC_EDIT_PLAYCHNLNUM);	
	if( pWnd == NULL )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_PLAYCHNLNUM) error!\n");
	}
    else
    {
        pWnd->EnableWindow(FALSE);
    }

    //录像通道数
    pWnd = GetDlgItem(IDC_EDIT_RECCHNLNUM);	
	if( pWnd == NULL )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_RECCHNLNUM) error!\n");
	}
    else
    {
        pWnd->EnableWindow(FALSE);
    }

    //录像起始端口
    pWnd = GetDlgItem(IDC_EDIT_RECSTARTPORT);	
	if( pWnd == NULL )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_RECSTARTPORT) error!\n");
	}
    else
    {
        pWnd->EnableWindow(FALSE);
    }
    // 放像起始端口
    pWnd = GetDlgItem(IDC_EDIT_STARTPLAYPORT);
    if(NULL != pWnd)
    {
        pWnd->EnableWindow(FALSE);
    }

    //录像存储路径
    pWnd = GetDlgItem(IDC_EDIT_RECSAVEPATH);	
	if( pWnd == NULL )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_RECSAVEPATH) error!\n");
	}
    else
    {
        pWnd->EnableWindow(FALSE);
    }

    // 机器名
    pWnd = GetDlgItem(IDC_EDIT_RECMACHNAME);
    if(NULL != pWnd)
    {
        pWnd->EnableWindow(FALSE);
    }

    //是否支持发布功能
    pWnd = GetDlgItem(IDC_CHECK_ISPUBLISH);
	if( pWnd == NULL )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_CHECK_ISPUBLISH) error!\n");
	}
    else
    {
        pWnd->EnableWindow(FALSE);
    }

    //数据库IP地址
    pWnd = GetDlgItem(IDC_EDIT_DBADDR);
	if( pWnd == NULL )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_DBADDR) error!\n");
	}
    else
    {
        pWnd->EnableWindow(FALSE);
    }

    //数据库用户名
    pWnd = GetDlgItem(IDC_EDIT_DBUSERNAME);	
	if( pWnd == NULL )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_DBUSERNAME) error!\n");
	}
    else
    {
        pWnd->EnableWindow(FALSE);
    }

    //数据库用户密码
    pWnd = GetDlgItem(IDC_EDIT_DBPASSWORD);	
	if( pWnd == NULL )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_DBPASSWORD) error!\n");
	}
    else
    {
        pWnd->EnableWindow(FALSE);
    }

    //FTP 路径
    pWnd = GetDlgItem(IDC_EDIT_VODFTPDIR);	
	if( pWnd == NULL )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_VODFTPDIR) error!\n");
	}
    else
    {
        pWnd->EnableWindow(FALSE);
    }

    //FTP用户名
    pWnd = GetDlgItem(IDC_EDIT_VODFTPUNAME);	
	if( pWnd == NULL )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_VODFTPUNAME) error!\n");
	}
    else
    {
        pWnd->EnableWindow(FALSE);
    }

    //FTP密码
    pWnd = GetDlgItem(IDC_EDIT_VODFTPPW);	
	if( pWnd == NULL )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_VODFTPPW) error!\n");
	}
    else
    {
        pWnd->EnableWindow(FALSE);
    }

	::InterlockedExchange(&lCalling, FALSE);
	return;

}

/*====================================================================
	函数  : ShowModifyConfig
	功能  : 隐藏配置信息
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    04/12/31    3.6         libo          create
	05/04/20	3.6			liuhuiyun	  增加状态显示
====================================================================*/
void CRecorderDlg::ShowModifyConfig()
{
	s8 achShowStr[1024]={0};
	s8 achConf[64]={0};
	struct in_addr in; 
	static long lCalling = FALSE;
    CWnd* pWnd = NULL;

	s8 achStatus[50] = {0};
	CWnd* pWndPrompt = GetDlgItem(IDC_STATICSTATUS);

	if( ::InterlockedExchange( &lCalling, TRUE ) )
	{
		return;     // 该函数已被调用返回
	}
    
    if(m_bOSEn)
    {
        sprintf(achStatus, "  Configuration Parameters");
        LanguageToEnglish();
    }
    else{
        sprintf(achStatus, "  设置录像机配置参数");
        LanguageToChinese();
    }    
	pWndPrompt->SetWindowText(achStatus);

    // 录像地址
    pWnd = GetDlgItem(IDC_EDIT_RECADDR);	
	if( pWnd == NULL )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_RECADDR) error!\n");
	}
    else
    {
        pWnd->EnableWindow(TRUE);

        in.s_addr = g_tCfg.m_dwRecIpAddr;
	    sprintf(achConf, "%s", inet_ntoa(in ));
        pWnd->SetWindowText( achConf );
    }

    // 外设别名
    pWnd = GetDlgItem(IDC_EDIT_EQPALIAS);	
	if( pWnd == NULL )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_EQPALIAS) error!\n");
	}
    else
    {
        pWnd->EnableWindow(TRUE);

        sprintf(achConf ,"%s",g_tCfg.m_szAlias);
        pWnd->SetWindowText( achConf );
    }

    // 外设类型
    pWnd = GetDlgItem(IDC_EDIT_EQPTYPE);	
	if( pWnd == NULL )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_EQPTYPE) error!\n");
	}
    else
    {
        pWnd->EnableWindow(TRUE);

        sprintf(achConf, "%d", g_tCfg.m_byEqpType);
        pWnd->SetWindowText(achConf);
    }

    // 外设ID
    pWnd = GetDlgItem(IDC_EDIT_EQPID);	
	if( pWnd == NULL )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_EQPID) error!\n");
	}
    else
    {
        pWnd->EnableWindow(TRUE);

        sprintf(achConf, "%d", g_tCfg.m_byEqpId);
        pWnd->SetWindowText(achConf);
    }

    // MCU ID
    pWnd = GetDlgItem(IDC_EDIT_MCUID);	
	if( pWnd == NULL )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_MCUID) error!\n");
	}
    else
    {
        pWnd->EnableWindow(TRUE);

        u32 dwMcuId = g_tCfg.m_wMcuId;
        pWnd->EnableWindow(TRUE);
        if( 0 == g_tCfg.m_wMcuIdB )
        {
            dwMcuId = LOCAL_MCUID;
        }
        sprintf(achConf, "%d", dwMcuId);
        pWnd->SetWindowText(achConf);
    }

    // MCU 地址
    pWnd = GetDlgItem(IDC_EDIT_MCUADDR);	
	if( pWnd == NULL )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_MCUADDR) error!\n");
	}
    else
    {
        pWnd->EnableWindow(TRUE);

        in.s_addr = g_tCfg.m_dwMcuIpAddr;
	    sprintf(achConf ,"%s", inet_ntoa(in ));
        pWnd->SetWindowText( achConf );
    }


    // MCU通信端口
    pWnd = GetDlgItem(IDC_EDIT_MCUPORT);	
	if( pWnd == NULL )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_MCUPORT) error!\n");
	}
    else
    {
        pWnd->EnableWindow(TRUE);
        u16 wMcuPort = g_tCfg.m_wMcuConnPort;
        if(0 == g_tCfg.m_wMcuConnPort)
        {
            wMcuPort = MCU_LISTEN_PORT;
        }
    	sprintf(achConf, "%d", wMcuPort);
        pWnd->SetWindowText(achConf);
    }
    
	/*
    pWnd = GetDlgItem(IDC_EDIT_MCUIDB); // mcu b's Id
    if(NULL != pWnd)
    {
        u32 dwMcuId = g_tCfg.m_wMcuIdB;
        pWnd->EnableWindow(TRUE);
        if( 0 == g_tCfg.m_wMcuIdB )
        {
            dwMcuId = LOCAL_MCUID;
        }
        sprintf(achConf, "%d", dwMcuId);
        pWnd->SetWindowText(achConf);
    }

    pWnd = GetDlgItem(IDC_EDIT_MCUPORTB); // mcu b's port
    if( NULL != pWnd)
    {
        pWnd->EnableWindow(TRUE);
        u16 wMcuPort = g_tCfg.m_wMcuConnPortB;
        if( 0 == g_tCfg.m_wMcuConnPortB )
        {
            wMcuPort = MCU_LISTEN_PORT;
        }
        sprintf(achConf, "%d", wMcuPort);
        pWnd->SetWindowText(achConf);
    }

    pWnd = GetDlgItem(IDC_EDIT_MCUADDRB); // mcu b's ip
    if(NULL != pWnd)
    {
        pWnd->EnableWindow(TRUE);
        if( 0!= g_tCfg.m_dwMcuIpAddrB )
        {
            in.s_addr = g_tCfg.m_dwMcuIpAddrB;
            sprintf(achConf, "%s", inet_ntoa(in));
        }
        else
        {
            memcpy(achConf, "0", sizeof("0"));
        }
        
        pWnd->SetWindowText(achConf);
    }
	*/

    // 放像通道数
    pWnd = GetDlgItem(IDC_EDIT_PLAYCHNLNUM);	
	if( pWnd == NULL )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_PLAYCHNLNUM) error!\n");
	}
    else
    {
        pWnd->EnableWindow(TRUE);

        sprintf(achConf, "%d", g_tCfg.m_byPlayChnNum);
        pWnd->SetWindowText(achConf);
    }

    // 录像通道数
    pWnd = GetDlgItem(IDC_EDIT_RECCHNLNUM);	
	if( pWnd == NULL )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_RECCHNLNUM) error!\n");
	}
    else
    {
        pWnd->EnableWindow(TRUE);

        sprintf(achConf, "%d", g_tCfg.m_byRecChnNum);
        pWnd->SetWindowText(achConf);
    }

    // 录像起始端口
    pWnd = GetDlgItem(IDC_EDIT_RECSTARTPORT);	
	if( NULL == pWnd )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_RECSTARTPORT) error!\n");
	}
    else
    {
        pWnd->EnableWindow(TRUE);

        sprintf(achConf, "%d", g_tCfg.m_wRecStartPort);
        pWnd->SetWindowText(achConf);
    }

    // 放像起始端口
    pWnd = GetDlgItem(IDC_EDIT_STARTPLAYPORT);
    if(NULL != pWnd)
    {
        pWnd->EnableWindow(TRUE);
        sprintf(achConf, "%d", g_tCfg.m_wPlayStartPort);
        pWnd->SetWindowText(achConf);
    }

    // 录像存储路径
    pWnd = GetDlgItem(IDC_EDIT_RECSAVEPATH);	
	if( NULL == pWnd )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_RECSAVEPATH) error!\n");
	}
    else
    {
        pWnd->EnableWindow(TRUE);

        sprintf(achConf, "%s", g_tCfg.m_szRecordPath);
        pWnd->SetWindowText(achConf);
    }

    // machine name
    pWnd = GetDlgItem(IDC_EDIT_RECMACHNAME);
    if( pWnd == NULL)
    {
        OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_RECMACHNAME) error!\n");
    }
    else
    {
        pWnd->EnableWindow(TRUE);
        sprintf(achConf, "%s", g_tCfg.m_szMachineName);
        pWnd->SetWindowText(achConf);
    }
    
    BOOL32 bIsPubLish = FALSE;
    // 是否支持发布功能
    pWnd = GetDlgItem(IDC_CHECK_ISPUBLISH);
	if( pWnd == NULL )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_CHECK_ISPUBLISH) error!\n");
	}
    else
    {
        pWnd->EnableWindow(TRUE);

        if ( g_tCfg.m_bMediaServer)
        {
            CheckDlgButton(IDC_CHECK_ISPUBLISH, 1);
            bIsPubLish = TRUE;
            ChangeServiceInfo(TRUE);
        }
        else if( m_bSetFlag )
        {
            CheckDlgButton(IDC_CHECK_ISPUBLISH, 1);
            bIsPubLish = TRUE;
            ChangeServiceInfo(TRUE);
        }
        else
        {
            CheckDlgButton(IDC_CHECK_ISPUBLISH, 0);
            ChangeServiceInfo(FALSE);
        }
    }
	::InterlockedExchange( &lCalling, FALSE );
	return;

}

/*====================================================================
	函数  : SaveModifyConfig
	功能  : 隐藏配置信息
	输入  : 无
	输出  : 无
	返回  : BOOL
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    04/12/31    3.6         libo          create
	05/04/06	3.6			liuhuiyun	  增加判断是否修改功能
	05/04/12	3.6			liuhuiyun	  修正修改判断
====================================================================*/
BOOL CRecorderDlg::SaveModifyConfig()
{
	s8 achShowStr[1024]={0};
	s8 achConf[64]={0};
	//struct in_addr in; 
	static long lCalling = FALSE;
	CWnd* pWnd = NULL;	
	
	s8 achStatus[50] = {0};
	CWnd* pWndPrompt = GetDlgItem(IDC_STATICSTATUS);
	
	if(m_bOSEn)
	{
		sprintf(achStatus, "  Recorder Settings");
	}
	else
	{
		sprintf(achStatus, "  设置参数");
	}
	pWndPrompt->SetWindowText(achStatus);
	
	pWnd = GetDlgItem( IDC_EDIT_RECMACHNAME );
	pWnd->EnableWindow(FALSE);
	
	if( ::InterlockedExchange( &lCalling, TRUE ) )
	{
		return FALSE;     //该函数已被调用返回
	}
	
	//[2012/7/20 zhangli]端口非法判断 
	if(m_nPlayStartPort < 0 || m_nPlayStartPort > 65535 
		|| m_nRecStartPort < 0 || m_nRecStartPort > 65535 )
	{
		if(m_bOSEn)
		{
			AfxMessageBox("The record and play start port should be between 0 and 65535.");
		}
		else 
		{
			AfxMessageBox("录像和放像起始端口只能在0－－65535之间");
		}
		::InterlockedExchange( &lCalling, FALSE );
		return FALSE;
	}

	//[2012/7/20 zhangli]端口间隔判断
	if ((m_nPlayStartPort > m_nRecStartPort && m_nPlayStartPort < m_nRecChnNum*10 + m_nRecStartPort)
		|| (m_nPlayStartPort < m_nRecStartPort && m_nRecStartPort > m_nPlayChnNum*10 + m_nPlayStartPort))
	{
		if(m_bOSEn)
		{
			AfxMessageBox("The port interval should not be less than channels number * 10.");
		}
		else 
		{
			AfxMessageBox("录放像端口之间间隔必须不小于通道数×10.");
		}
		::InterlockedExchange( &lCalling, FALSE );
		return FALSE;
	}

	// MCU 地址
	pWnd = GetDlgItem(IDC_EDIT_MCUADDR);	
	if( NULL == pWnd )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_MCUADDR) error!\n");
	}
	else
	{
		pWnd->GetWindowText(achConf, 64);
		// 对IP地址进行校验, zgc, 2008-03-27
		if ( !IsValidIpV4(achConf) )
		{
			if (m_bOSEn)
			{
				AfxMessageBox("The Ip of MCU is invalid.\n");
			}
			else
			{
				AfxMessageBox("MCU 的IP地址非法.\n"); // ID 还不支持设置
			}
			::InterlockedExchange( &lCalling, FALSE );
			return FALSE;
		}
		if(g_tCfg.m_dwMcuIpAddr != inet_addr(achConf))
		{
			g_tCfg.m_dwMcuIpAddr = inet_addr(achConf);
			OspPrintf(TRUE, FALSE, "g_tCfg.m_dwMcuIpAddr = 0x%x\n", g_tCfg.m_dwMcuIpAddr);
			m_bModified = TRUE;
		}
	}
	
	// MCU ID 目前不能设置
	pWnd = GetDlgItem(IDC_EDIT_MCUID);	
	if( NULL == pWnd )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_MCUID) error!\n");
	}
	else
	{
		pWnd->GetWindowText(achConf, 64);
		if(g_tCfg.m_wMcuId != (u8)atoi(achConf))
		{
			g_tCfg.m_wMcuId = (u8)atoi(achConf);
			OspPrintf(TRUE, FALSE, "g_tCfg.m_wMcuId = %d\n", g_tCfg.m_wMcuId);
			m_bModified = TRUE;
		}
	}
	
	// MCU.A 通信端口
	pWnd = GetDlgItem(IDC_EDIT_MCUPORT);	
	if( NULL == pWnd )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_MCUPORT) error!\n");
	}
	else
	{
		pWnd->GetWindowText(achConf, 64);
		s32 nMcuNewPort = (s32)atoi(achConf);
		if(g_tCfg.m_wMcuConnPort != (u16)nMcuNewPort)
		{
			if( nMcuNewPort < 0 || nMcuNewPort > 65535 )
			{
				if(m_bOSEn)
				{
					AfxMessageBox("The port number of MCU should be between 0 and 65535.");
				}
				else 
				{
					AfxMessageBox("MCU 端口只能在0－－65535之间");
				}
				::InterlockedExchange( &lCalling, FALSE );
				return FALSE;
			}
			else
			{
				g_tCfg.m_wMcuConnPort = (u16)nMcuNewPort;
				OspPrintf(TRUE, FALSE, "g_tCfg.m_wMcuConnPort = %d\n", g_tCfg.m_wMcuConnPort);
				m_bModified = TRUE;
			}
		}
	}
	
	u32 dwMcuIpA = g_tCfg.m_dwMcuIpAddr;
	u32 dwMcuIpB = g_tCfg.m_dwMcuIpAddrB;
	u16 wMcuAPort = g_tCfg.m_wMcuConnPort;
	u16 wMcuBPort = g_tCfg.m_wMcuConnPortB;
	u16 wMcuAId = g_tCfg.m_wMcuId;
	u16 wMcuBId = g_tCfg.m_wMcuIdB;
	
	if( 0 == dwMcuIpA )
	{
		if (m_bOSEn)
		{
			AfxMessageBox("The Ip of MCU cannot be 0.\n");
		}
		else
		{
			AfxMessageBox("MCU 的IP地址不能为 0.\n"); // ID 还不支持设置
		}
		::InterlockedExchange( &lCalling, FALSE );
		return FALSE;
	}
	
	/*
	if( 0 == dwMcuIpA )
	{
	wMcuAId = 0;
	wMcuAPort = 0;
	}
	
	  //MCU.B ID 固定值，不做设置处理
	  pWnd = GetDlgItem(IDC_EDIT_MCUIDB);
	  if( NULL != pWnd )
	  {
	  pWnd->GetWindowText(achConf, 64);        
	  if(g_tCfg.m_wMcuIdB != (u16)atoi(achConf))
	  {
	  wMcuBId = (u16)atoi(achConf);
	  m_bModified = TRUE;
	  }
	  }
	  
		// MCU.B 通信端口
		pWnd = GetDlgItem(IDC_EDIT_MCUPORTB);
		if( NULL != pWnd )
		{
		pWnd->GetWindowText(achConf, 64);
		s32 nMcuNewPort = (s32)atoi(achConf);
		if(g_tCfg.m_wMcuConnPortB != (u16)nMcuNewPort)
		{
		if( nMcuNewPort < 0 || nMcuNewPort > 65535 )
		{
		if(m_bOSEn)
		{
		AfxMessageBox("The port number of MCU should be between 0 and 65535.");
		}
		else 
		{
		AfxMessageBox("MCU 端口只能在0－－65535之间");
		} 
		m_bModified = FALSE;
		::InterlockedExchange( &lCalling, FALSE );
		return;
		}
		else
		{
		wMcuBPort = (u16)nMcuNewPort;
		m_bModified = TRUE;
		}
		}
		}
		
		  // MCU.B 通信IP
		  pWnd = GetDlgItem(IDC_EDIT_MCUADDRB);
		  if( NULL != pWnd )
		  {
		  pWnd->GetWindowText(achConf, 64);
		  u32 dwIpFromDlg = inet_addr(achConf);
		  if(g_tCfg.m_dwMcuIpAddrB != dwIpFromDlg)
		  {
		  dwMcuIpB = inet_addr(achConf);
		  m_bModified = TRUE;
		  }
		  }
		  
			if( 0 != dwMcuIpB )
			{
			if( 0 == wMcuBId || 0 == wMcuBPort )
			{
			if (m_bOSEn)
			{
			AfxMessageBox("The ID or Port number of MCU cannot be 0.\n");
			}
			else
			{
			AfxMessageBox("MCU.B 的端口号不能为 0.\n"); // ID 还不支持设置
			}
			m_bModified = FALSE;
			::InterlockedExchange( &lCalling, FALSE );
			return;
			}
			}
			
			  if( 0 == dwMcuIpB )
			  {
			  wMcuBId   = 0;
			  wMcuBPort = 0;
			  }
			  
				if( 0 == dwMcuIpA && 0 == dwMcuIpB )
				{
				if (m_bOSEn)
				{
				AfxMessageBox("The IP of MCU_A and MCU_B cannot be all 0.\n");
				}
				else
				{
				AfxMessageBox("MCU.A和MCU.B的IP不能都为 0.\n");
				}
				m_bModified = FALSE;
				::InterlockedExchange( &lCalling, FALSE );
				return;
				}
				
				  if ( dwMcuIpB == dwMcuIpA )
				  {
				  if (m_bOSEn)
				  {
				  AfxMessageBox("The IP of MCU_A and MCU_B cannot be Equal.\n");
				  }
				  else
				  {
				  AfxMessageBox("MCU.A和MCU.B的IP不能相等.\n");
				  }
				  m_bModified = FALSE;
				  ::InterlockedExchange( &lCalling, FALSE );
				  return;
				  }
				  
					if( dwMcuIpA == dwMcuIpB )
					{
					dwMcuIpB = 0;
					wMcuBId  = 0;
					wMcuBPort = 0;
					}
					
					  g_tCfg.m_wMcuIdB = wMcuBId;
					  g_tCfg.m_wMcuConnPortB = wMcuBPort;
					  g_tCfg.m_dwMcuIpAddrB = dwMcuIpB;
*/

	// 录像地址
	pWnd = GetDlgItem(IDC_EDIT_RECADDR);	
	if( NULL == pWnd )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_RECADDR) error!\n");
	}
	else
	{
		pWnd->GetWindowText(achConf, 64);
		// 对IP地址进行校验, zgc, 2008-03-27
		if ( !IsValidIpV4(achConf) )
		{
			if (m_bOSEn)
			{
				AfxMessageBox("The Ip of rec is invalid.\n");
			}
			else
			{
				AfxMessageBox("录像 的IP地址非法.\n"); // ID 还不支持设置
			}
			::InterlockedExchange( &lCalling, FALSE );
			return FALSE;
		}
		// add by liuhuiyun
		if(g_tCfg.m_dwRecIpAddr != inet_addr(achConf))
		{
			g_tCfg.m_dwRecIpAddr = inet_addr(achConf);
			m_bModified = TRUE;
			// VOD部分必须重启后生效
			m_bCriticalModify = TRUE;
		}
	}
	
	// 外设别名
	pWnd = GetDlgItem(IDC_EDIT_EQPALIAS);	
	if( NULL == pWnd )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_EQPALIAS) error!\n");
	}
	else
	{
		pWnd->GetWindowText(achConf, 64);
		if(strcmp(g_tCfg.m_szAlias, achConf))
		{	
			sprintf(g_tCfg.m_szAlias, "%s", achConf);
			m_bModified = TRUE;
			
			SetWindowText( g_tCfg.m_szAlias );
		}
	}
	
	//外设类型---固定值不能修改
	
	// 外设ID
	pWnd = GetDlgItem(IDC_EDIT_EQPID);	
	if( NULL == pWnd )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_EQPID) error!\n");
	}
	else
	{
		pWnd->GetWindowText(achConf, 64);
		
		u8 byNewRecId = (u8)atoi(achConf);
		
		if(g_tCfg.m_byEqpId != byNewRecId)
		{
			if( byNewRecId < 17 || byNewRecId > 32 )
			{
				if(m_bOSEn)
				{
					AfxMessageBox("The Recorder ID can only be a number between 17 and 32");
				}
				else 
				{
					AfxMessageBox("录像机 ID只能在17－－32之间");
				}
				::InterlockedExchange( &lCalling, FALSE );
				return FALSE;
			}
			else
			{
				g_tCfg.m_byEqpId = byNewRecId;
				// guzh [8/29/2006] 重新保存               
				g_cRecApp.m_tEqp.SetMcuEqp( (u8)g_tCfg.m_wMcuId,
					g_tCfg.m_byEqpId,
					g_tCfg.m_byEqpType );
				
				OspPrintf(TRUE, FALSE, "g_tCfg.m_byEqpId = %d\n", g_tCfg.m_byEqpId);
				m_bModified = TRUE;
			}
		}
	}
	
	// 录像通道数
	pWnd = GetDlgItem(IDC_EDIT_RECCHNLNUM);	
	if( NULL == pWnd )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_RECCHNLNUM) error!\n");
	}
	else
	{
		pWnd->GetWindowText(achConf, 64);
		s32 nChnNum = (s32)atoi(achConf);
		if(g_tCfg.m_byRecChnNum != nChnNum)
		{
			if ( nChnNum > 0 && nChnNum < MAXNUM_RECORDER_CHNNL )
			{
				if ( m_nPlayChnNum + m_nRecChnNum <= MAXNUM_RECORDER_CHNNL )
				{
					g_tCfg.m_byRecChnNum = nChnNum;
					OspPrintf(TRUE, FALSE, "g_tCfg.m_byRecChnNum = %d\n", g_tCfg.m_byRecChnNum);
					m_bModified = TRUE;
					
					// 录像通道数必须重启后生效
					m_bCriticalModify = TRUE;
				}
				else
				{
					if(m_bOSEn)
					{
						AfxMessageBox("The record channel number and play channel number should not be over 32.");
					}
					else 
					{
						AfxMessageBox("录像通道和放象通道数的和不能超过最大通道数32");
					}
					::InterlockedExchange( &lCalling, FALSE );
					return FALSE;
				}
			}
			else
			{
				if ( m_bOSEn )
				{
					AfxMessageBox("The record channel number must be over 0 and less than 32");
				}
				else
				{
					AfxMessageBox("录像通道数必须大于0, 小于32");
				}
				::InterlockedExchange( &lCalling, FALSE );
				return FALSE;
			}
		}
	}
	
	// 放像通道数
	pWnd = GetDlgItem(IDC_EDIT_PLAYCHNLNUM);	
	if( NULL == pWnd )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_PLAYCHNLNUM) error!\n");
	}
	else
	{
		pWnd->GetWindowText(achConf, 64);
		s32 nChnNum = (s32)atoi(achConf);
		if(g_tCfg.m_byPlayChnNum != nChnNum)
		{
			if ( nChnNum > 0 && nChnNum < MAXNUM_RECORDER_CHNNL )
			{
				if ( m_nPlayChnNum + m_nRecChnNum <= MAXNUM_RECORDER_CHNNL )
				{
					g_tCfg.m_byPlayChnNum = nChnNum;
					OspPrintf(TRUE, FALSE, "g_tCfg.m_byPlayChnNum = %d\n", g_tCfg.m_byPlayChnNum);
					m_bModified = TRUE;
					
					// 放像通道数必须重启后生效
					m_bCriticalModify = TRUE;
				}
				else
				{
					if(m_bOSEn)
					{
						AfxMessageBox("The record channel number and play channel number should not be over 32.");
					}
					else 
					{
						AfxMessageBox("录像通道和放象通道数的和不能超过最大通道数32");
					}
					::InterlockedExchange( &lCalling, FALSE );
					return FALSE;
				}
			}
			else
			{
				if ( m_bOSEn )
				{
					AfxMessageBox("The record channel number must be over 0 and less than 32");
				}
				else
				{
					AfxMessageBox("录像通道数必须大于0, 小于32");
				}
				::InterlockedExchange( &lCalling, FALSE );
				return FALSE;
			}
		}
	}
	
	
	// 录像起始端口
	pWnd = GetDlgItem(IDC_EDIT_RECSTARTPORT);	
	if( NULL != pWnd )
	{
		pWnd->GetWindowText(achConf, 64);
		s32 nNewRecStartPort = (s32)atoi(achConf);
		if(g_tCfg.m_wRecStartPort != nNewRecStartPort)
		{
			g_tCfg.m_wRecStartPort = (u16)nNewRecStartPort;
			OspPrintf(TRUE, FALSE, "g_tCfg.m_wRecStartPort = %d\n", g_tCfg.m_wRecStartPort);
			m_bModified = TRUE;
		}
	}
	
	// 放像起始端口
	pWnd = GetDlgItem(IDC_EDIT_STARTPLAYPORT);
	if(NULL != pWnd)
	{
		pWnd->GetWindowText(achConf, 64);
		s32 nNewPlayStartPort = (s32)atoi(achConf);
		if(g_tCfg.m_wPlayStartPort != nNewPlayStartPort)
		{
			g_tCfg.m_wPlayStartPort = (u16)nNewPlayStartPort;
			OspPrintf(TRUE, FALSE, "g_tCfg.m_wPlayStartPort = %d\n", g_tCfg.m_wPlayStartPort);
			m_bModified = TRUE;
		}
	}
	
	// 录像存储路径
	pWnd = GetDlgItem(IDC_EDIT_RECSAVEPATH);	
	if( NULL == pWnd )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_RECSAVEPATH) error!\n");
	}
	else
	{        
		pWnd->GetWindowText(achConf, 64);
		if(strcmp(g_tCfg.m_szRecordPath, achConf))
		{
			sprintf(g_tCfg.m_szRecordPath, "%s", achConf);
			m_bModified = TRUE;
			
			// 存储路径必须重启后生效
			m_bCriticalModify = TRUE; 
		}
	}
	
	pWnd = GetDlgItem(IDC_EDIT_RECMACHNAME);  // machine name
	if( NULL == pWnd )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_RECMACHNAME) error!\n");
	}
	else
	{
		pWnd->GetWindowText(achConf, 64);
		if(strcmp(g_tCfg.m_szMachineName, achConf))
		{
			sprintf(g_tCfg.m_szMachineName, "%s", achConf);
			m_bModified = TRUE;
			
			// VOD部分必须重启后生效
			m_bCriticalModify = TRUE;
		}
	}
	
	// 是否支持发布功能
	pWnd = GetDlgItem(IDC_CHECK_ISPUBLISH);
	if( NULL == pWnd )
	{
		OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_CHECK_ISPUBLISH) error!\n");
	}
	else
	{
		u32 dwChecked = IsDlgButtonChecked(IDC_CHECK_ISPUBLISH);
		if((BOOL)dwChecked != g_tCfg.m_bMediaServer)
		{
			m_bModified = TRUE;
			
			// VOD部分必须重启后生效
			m_bCriticalModify = TRUE;
		}
		if (dwChecked == 0)
		{
			g_tCfg.m_bMediaServer = FALSE;
		}
		else
		{
			g_tCfg.m_bMediaServer = TRUE;
		}
		
	}
	
	if (g_tCfg.m_bMediaServer)    //如果不发布则做下面的操作
	{
		
		// 数据库IP地址
		pWnd = GetDlgItem(IDC_EDIT_DBADDR);
		if( NULL == pWnd )
		{
			OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_DBADDR) error!\n");
		}
		else
		{
			pWnd->EnableWindow(TRUE);
			pWnd->GetWindowText(achConf, 64);
			pWnd->EnableWindow(FALSE);
			if ( !IsValidIpV4(achConf) )  		// 对IP地址进行校验
			{
				if (m_bOSEn)
				{
					AfxMessageBox("The Ip of DB  is invalid.\n");
				}
				else
				{
					AfxMessageBox("数据库 的IP地址非法.\n"); // ID 还不支持设置
				}
				::InterlockedExchange( &lCalling, FALSE );
				return FALSE;
			}
			if(g_tCfg.m_dwDBHostIP != inet_addr(achConf))
			{
				g_tCfg.m_dwDBHostIP = inet_addr(achConf);
				OspPrintf(TRUE, FALSE, "g_tCfg.m_dwDBHostIP = 0x%x\n", g_tCfg.m_dwDBHostIP);
				m_bModified = TRUE;
				
				// VOD部分必须重启后生效
				m_bCriticalModify = TRUE;
			}
		}
		
		// 数据库用户名
		pWnd = GetDlgItem(IDC_EDIT_DBUSERNAME);	
		if( NULL == pWnd )
		{
			OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_DBUSERNAME) error!\n");
		}
		else
		{
			pWnd->EnableWindow(TRUE);
			pWnd->GetWindowText(achConf, 64);
			pWnd->EnableWindow(FALSE);
			if(strcmp(g_tCfg.m_achDBUsername, achConf))
			{
				sprintf(g_tCfg.m_achDBUsername, "%s", achConf);
				m_bModified = TRUE;
				
				// VOD部分必须重启后生效
				m_bCriticalModify = TRUE;
			}
		}
		
		// 数据库用户密码
		pWnd = GetDlgItem(IDC_EDIT_DBPASSWORD);	
		if( NULL == pWnd )
		{
			OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_DBPASSWORD) error!\n");
		}
		else
		{
			pWnd->EnableWindow(TRUE);
			pWnd->GetWindowText(achConf, 64);
			pWnd->EnableWindow(FALSE);
			if(strcmp(g_tCfg.m_achDBPassword, achConf))
			{
				sprintf(g_tCfg.m_achDBPassword, "%s", achConf);
				m_bModified = TRUE;
				
				// VOD部分必须重启后生效
				m_bCriticalModify = TRUE;
			}
		}
		
		// FTP 路径
		pWnd = GetDlgItem(IDC_EDIT_VODFTPDIR);	
		if( NULL == pWnd )
		{
			OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_VODFTPDIR) error!\n");
		}
		else
		{
			pWnd->EnableWindow(TRUE);
			pWnd->GetWindowText(achConf, 64);
			pWnd->EnableWindow(FALSE);
			if(strcmp(g_tCfg.m_achVodFtpPatch, achConf))
			{
				sprintf(g_tCfg.m_achVodFtpPatch, "%s", achConf);
				m_bModified = TRUE;
				
				// VOD部分必须重启后生效
				m_bCriticalModify = TRUE;
			}
		}
		
		// FTP用户名
		pWnd = GetDlgItem(IDC_EDIT_VODFTPUNAME);	
		if( NULL == pWnd )
		{
			OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_VODFTPUNAME) error!\n");
		}
		else
		{
			pWnd->EnableWindow(TRUE);
			pWnd->GetWindowText(achConf, 64);
			pWnd->EnableWindow(FALSE);
			if(strcmp(g_tCfg.m_achFTPUsername, achConf))
			{
				sprintf(g_tCfg.m_achFTPUsername, "%s", achConf);
				m_bModified = TRUE;
				
				// VOD部分必须重启后生效
				m_bCriticalModify = TRUE;
			}
		}
		
		// FTP密码
		pWnd = GetDlgItem(IDC_EDIT_VODFTPPW);	
		if( NULL == pWnd )
		{
			OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_VODFTPPW) error!\n");
		}
		else
		{
			pWnd->EnableWindow(TRUE);
			pWnd->GetWindowText(achConf, 64);
			pWnd->EnableWindow(FALSE);
			if(strcmp(g_tCfg.m_achFTPPassword, achConf))
			{
				sprintf(g_tCfg.m_achFTPPassword, "%s", achConf);
				m_bModified = TRUE;
				
				// VOD部分必须重启后生效
				m_bCriticalModify = TRUE;
			}
		}
	}
	if(TRUE == m_bModified )
	{
		//Write Configure Into File
		if( !g_tCfg.WriteConfigIntoFile() )
		{
			OspPrintf(TRUE, FALSE, "WriteConfigIntoFile()--error!\n");
			::InterlockedExchange( &lCalling, FALSE );
			return FALSE;
		}
		else
		{        
			OspPrintf(TRUE, FALSE, "WriteConfigIntoFile()--OK!\n");
		}
	}
	
	::InterlockedExchange( &lCalling, FALSE );
	
	return TRUE;
	
}

/*====================================================================
	函数  : OnShowTrc
	功能  : 显示调试信息
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
/*void CRecorderDlg::OnShowTrc(WPARAM w, LPARAM l) 
{
	ShowTrcInfo();
}*/

/*====================================================================
	函数  : ShowRecChnInfo
	功能  : 显示录像通道信息
	输入  : 通道序号
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CRecorderDlg::ShowRecChnInfo(WORD wChn )
{
	TRecorderStatus tRecorderStatus;
    TRecorderStatis tRecorderStatis;
	memset(&tRecorderStatus, 0, sizeof(tRecorderStatus));
	memset(&tRecorderStatis, 0, sizeof(tRecorderStatis));
	//[2013/5/27 zhangli]m_byMediaType不能等于0，因为MEDIA_TYPE_PCMU=0，导致在不录像时每个流统计都显示是“音频”
	for (u8 byLoop = 0; byLoop < MAX_STREAM_NUM; ++byLoop)
	{
		tRecorderStatis.m_atMediaRecStatis[byLoop].m_byMediaType = MEDIA_TYPE_NULL;
	}

	static long lCalling = FALSE;
	s8  achStatus[50] = {0};
    s8 achShowStr[1024]={0};
	s8 achItem[KDV_MAX_PATH+1];
    u16 wRet = 0;

	u32 dwDevice = g_adwDevice[wChn];

	CWnd* pWndPrompt = GetDlgItem(IDC_STATICSTATUS);
	CWnd* pWnd = GetDlgItem(IDC_INFOCONSOLE);
	if( pWnd == NULL )
	{
		return;
	}

	if( ::InterlockedExchange( &lCalling ,TRUE ) )
	{
		return;//该函数已被调用返回
	}

	wRet = RPGetRecorderStatus( dwDevice, &tRecorderStatus);
	
    if( RP_OK == wRet)
    {
        if(REC_STOP != tRecorderStatus.m_tRecStatus.m_tRecState)
		{
			if (RP_OK == RPGetRecorderStatis(dwDevice, &tRecorderStatis))
			{
				tRecorderStatis.m_dwRecTime += g_adwRecDuration[dwDevice];
			}
			else
			{
				if (m_bOSEn)
				{
					strcpy(achShowStr,"Fail to get the recorder statistic!\n" );
					strncpy(achStatus, "Fail to get the recorder statistic!\n", sizeof(achStatus) - 1 );
					pWndPrompt->SetWindowText(achStatus);
				}
				else
				{
					strcpy(achShowStr,"查询录像机统计信息失败!\n" );
					strncpy(achStatus, "查询录像机统计信息失败!\n", sizeof(achStatus) - 1 );
					pWndPrompt->SetWindowText(achStatus);
				}
			}
		}

		if(TRUE == m_bOSEn)
		{
			sprintf(achStatus, "  Record Channel %d \n", wChn);
			pWndPrompt->SetWindowText(achStatus);
			sprintf(achItem ,"\n\n\tRecord Channel %d\n", wChn);
			strcat( achShowStr ,achItem);
			tRecorderStatus.m_tRecParam.m_achRecFileName[MAX_FILE_NAME_LEN]='\0';
			GetEnglishRecorderChannelInfo(achShowStr, tRecorderStatus, tRecorderStatis);
		}
		else
		{
			sprintf(achStatus, "  显示录像通道 %d 状态\n", wChn);
			pWndPrompt->SetWindowText(achStatus);
			sprintf(achItem ,"\n\n\t录像通道 %d\n", wChn);
			strcat( achShowStr ,achItem);
			tRecorderStatus.m_tRecParam.m_achRecFileName[MAX_FILE_NAME_LEN]='\0';
			GetChineseRecorderChannelInfo(achShowStr, tRecorderStatus, tRecorderStatis);
		}
    }
    else
    {
        if(TRUE == m_bOSEn)
		{
			strcpy( achShowStr,"Fail to get the recorder status!\n" );
            sprintf(achStatus, "Fail to get the recorder status!");
            pWndPrompt->SetWindowText(achStatus);
        }
        else
		{
            strcpy(achShowStr,"查询录像机状态失败!\n" );
            strncpy(achStatus, "查询录像机状态失败!\n", sizeof(achStatus) - 1 );
            pWndPrompt->SetWindowText(achStatus);
        }
        
		pWndPrompt->SetWindowText(achStatus);
    }

	pWnd->SetWindowText( achShowStr );
	::InterlockedExchange( &lCalling ,FALSE );
    
    pWnd = GetDlgItem( IDC_EDIT_STARTPLAYPORT );
    pWnd->EnableWindow( FALSE );

	return;
}

/*====================================================================
	函数  : ShowPlayChnInfo
	功能  : 显示放像通道信息
	输入  : 通道序号
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CRecorderDlg::ShowPlayChnInfo( WORD wChn )
{
	TPlayerStatus tPlayerStatus;
    TPlayerStatis tPlayerStatis;

	s8 achShowStr[1024] = {0};
	s8 achItem[KDV_MAX_PATH+1];
	static long lCalling = FALSE;
	s8 achStatus[50] = {0};
    u16 wRet = 0;

    u32 dwDevice = g_adwDevice[wChn];

	CWnd* pWndPrompt = GetDlgItem(IDC_STATICSTATUS);
	CWnd* pWnd = GetDlgItem(IDC_INFOCONSOLE);
	if( pWnd == NULL )
	{
		return;
	}

	if( ::InterlockedExchange( &lCalling ,TRUE ) )
	{
		return;     //该函数已被调用返回
	}

    wRet = RPGetPlayerStatis( dwDevice, &tPlayerStatis );
    if( RP_OK == wRet)
    {
        if( RP_OK == RPGetPlayerStatus( dwDevice, &tPlayerStatus) )
	    {
		    tPlayerStatus.m_tPlayParam.m_achPlayFileName[MAX_FILE_NAME_LEN]='\0';
		    if(TRUE == m_bOSEn)
            {
                sprintf(achStatus, "  Play Channel %d \n", wChn-g_tCfg.m_byRecChnNum);
		        pWndPrompt->SetWindowText(achStatus);

		        sprintf(achItem ,"\n\n\tPlay Channel %d\n",wChn-g_tCfg.m_byRecChnNum );
		        strcat( achShowStr ,achItem);
                GetEnglishPlayChannelInfo(achShowStr, tPlayerStatus, tPlayerStatis);
            }
            else
            {
                sprintf(achStatus, "  显示播放通道 %d 状态\n", wChn-g_tCfg.m_byRecChnNum);
		        pWndPrompt->SetWindowText(achStatus);

		        sprintf(achItem ,"\n\n\t播放通道 %d\n",wChn-g_tCfg.m_byRecChnNum );
		        strcat( achShowStr ,achItem);
                GetChinesePlayChannelInfo(achShowStr, tPlayerStatus, tPlayerStatis);
            }
	    }
	    else
	    {
		    if(TRUE == m_bOSEn)
            {
                strcpy(achShowStr,"Fail to get the player status!\n" );
                strncpy(achStatus, "Fail to get the player statistic!\n", sizeof(achStatus) - 1 );
                pWndPrompt->SetWindowText(achStatus);
            }
            else 
			{
                strcpy(achShowStr,"查询放像机状态失败!\n" );
                strncpy(achStatus, "查询放像机统计信息失败!\n", sizeof(achStatus) - 1 );
                pWndPrompt->SetWindowText(achStatus);
            }
        }
    }
    else
    {
        if(TRUE == m_bOSEn)
		{
            strcpy(achShowStr,"Fail to get the player statistic!\n" );
            strncpy(achStatus, "Fail to get the player statistic!\n", sizeof(achStatus) - 1 );
            pWndPrompt->SetWindowText(achStatus);
        }
        else
		{
            strcpy(achShowStr,"查询放像机统计信息失败!\n" );
            strncpy(achStatus, "查询放像机统计信息失败!\n", sizeof(achStatus) - 1 );
            pWndPrompt->SetWindowText(achStatus);
        }
    }    
	pWnd->SetWindowText( achShowStr );
	::InterlockedExchange( &lCalling ,FALSE );

	return;
}


/*====================================================================
	函数  : Init
	功能  : 初始化成员变量。
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CRecorderDlg::Init()
{
    m_bModifyConfig = FALSE;
	m_dwShowInfoType = 0xffffffff;

	m_bModified = FALSE;
    m_bCriticalModify = FALSE;
    return;
}

void CRecorderDlg::OnTimer(UINT nIDEvent) 
{


    s8 achInfo[64];
    memset(achInfo, '\0', sizeof(achInfo) );

    if( OPEN_DATABASE_TIMER == nIDEvent ) // 打开数据库
    {
        if(m_bOSEn)
		{
            strncpy( achInfo, "Opening database...", sizeof(achInfo) - 1) ;
        }
        else
		{
            strncpy( achInfo, "正在打开数据库...", sizeof(achInfo) - 1) ;
        }
        m_ctrStatusNote.SetWindowText(achInfo);
        UpdateData( FALSE );
        
        PostMessage(WM_OPENDATABASE, 0, 0);
        KillTimer( OPEN_DATABASE_TIMER );
        return;
    }

    // 刷新界面定时器
    if (REFRSH_CHNL_TIMER == nIDEvent)
    {
	    DWORD dwType;
	    u8  byChn;
	    dwType = m_dwShowInfoType;
        byChn  = (u8)(dwType&0xff);
	    dwType &= 0xffffff00;        
	    if( ( dwType == RECORD_HEAD )&&
		    ( byChn  >  0 )&&
		    ( byChn  <= g_tCfg.m_byRecChnNum ))  
	    {
		    ShowRecChnInfo( byChn);

	    }
	    else if( (dwType == PLAY_HEAD )&&
		         ( byChn > g_tCfg.m_byRecChnNum )&&
		         ( byChn <= g_tCfg.m_byTotalChnNum ) )
	    {
		    ShowPlayChnInfo( byChn );
	    }
    }

	CDialog::OnTimer(nIDEvent);
    return;
}

void CRecorderDlg::Quit(BOOL32 bForce)
{
    CQuitRecDlg cQuitRecDlg;
    cQuitRecDlg.m_bEnglishLang = m_bOSEn;

    BOOL32 bRet = IDOK;
    if (!bForce)
    {
        // 不强制则提示用户
        bRet = cQuitRecDlg.DoModal();
    }
    if( IDOK == bRet )
	{		
 		//通知录像机退出
		OspSemBCreate(&g_hSemQuit);
		OspSemTake(g_hSemQuit);
 		::OspPost(MAKEIID(AID_RECORDER, CInstance::DAEMON), EV_REC_QUIT);
	
		OspSemTake(g_hSemQuit);
		if( g_tCfg.m_bMediaServer )
		{
            g_cTDBOperate.CloseDB();
            g_tCfg.m_bOpenDBSucceed = FALSE;
		}
		::RPRelease();

		

		OspSemGive(g_hSemQuit);
		OspSemDelete(g_hSemQuit);
		::OspQuit();
		CDialog::OnOK();
	}

    return;
}


/*====================================================================
	函数  : OnShowTask
	功能  : 
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
LRESULT CRecorderDlg::OnShowTask(WPARAM wParam,LPARAM lParam)
{
	//wParam接收的是图标的ID，而lParam接收的是鼠标的行为
	if (wParam != IDR_MAINFRAME)
		return 1;

	switch(lParam)
	{
	case WM_RBUTTONUP:
		{
		}
		break;
	case WM_LBUTTONDBLCLK:
		{
            //双击左键的处理
			ShowWindow(SW_SHOWNOACTIVATE);
            BringWindowToTop();
		}
		break;
	}

	return S_OK;
} 

/*=============================================================================
  函 数 名： OnOpenDataBase
  功    能： 打开数据库
  算法实现： 
  全局变量： 
  参    数： WPARAM wParam
             LPARAM lParam
  返 回 值： void 
  已知问题： 单线程会导致界面死锁1分钟左右
=============================================================================*/
void CRecorderDlg::OnOpenDataBase(WPARAM wParam, LPARAM lParam)
{
    s8  achInfo[256];
    memset(achInfo, '\0', sizeof(achInfo) );

	u16 wRet = g_cRecApp.OpenDB();

	if( VOD_OK != wRet )
	{
		memset(achInfo, '\0', sizeof(achInfo) );
        if(m_bOSEn)
		{
            sprintf(achInfo, "Can't open database! Error code :%d\nThis recorder does not support publishing function.\n", wRet);
        }
        else 
		{
            sprintf(achInfo, "无法打开 %s 机器上的数据库! Error code :%d\n录像机无发布功能\n",
				    g_tCfg.m_achDBHostIP, wRet);
        }
		AfxMessageBox( achInfo );

        // zbq [07/05/2007] 本问题已解决，用 m_bOpenDBSucceed 作业务逻辑标识。m_bMediaServer 继续保持其原角色，和界面保持一致。
        // FIXME guzh [6/28/2007] 这里不清变量会导致业务程序误认为可以发布
        // zbq [003/10/2007] 该变量只和界面配置保持同步，此处不能置位
        // g_tCfg.m_bMediaServer = FALSE;

        m_bSetFlag = TRUE;
        m_ctrStatusNote.SetWindowText(achInfo);
        UpdateData( FALSE );
        return;
	}
    
    if(m_bOSEn)
	{
        strncpy( achInfo, "Open database successfully!", sizeof("Open database successfully!")) ;
    }
    else 
	{
        strncpy( achInfo, "打开数据库成功", sizeof("打开数据库成功")) ;
    }
    m_ctrStatusNote.SetWindowText(achInfo);
	
    UpdateData( FALSE );
	HideModifyConfig();
    return; 
}

void CRecorderDlg::OnClose() 
{
	Quit();
//	ShowWindow(SW_HIDE);//隐藏主窗口
//	CDialog::OnClose();
}

void CRecorderDlg::OnDestroy() 
{
    ::Shell_NotifyIcon(NIM_DELETE, &m_tnidTray); 

    ::CoUninitialize();
	CDialog::OnDestroy();
	//Quit();
}

BOOL CRecorderDlg::PreTranslateMessage(MSG* pMsg) 
{
	if ( pMsg->message == WM_KEYDOWN )
	{
		if ( ( pMsg->wParam == VK_RETURN ) || ( pMsg->wParam == VK_ESCAPE ) )
		{
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

/*=============================================================================
  函 数 名： ChangeServiceInfo
  功    能： 修改与服务器相关参数
  算法实现： 
  全局变量： 
  参    数： BOOL bIsValid
  返 回 值： void 
=============================================================================*/
void CRecorderDlg::ChangeServiceInfo(BOOL bIsValid)
{
    CWnd* pWnd = NULL;
    s8 achConf[64]={0};
	struct in_addr in;
    
    if(TRUE == bIsValid)
    {
        pWnd = GetDlgItem(IDC_EDIT_DBADDR);
	    if( NULL == pWnd )
	    {
		    OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_DBADDR) error!\n");
	    }
        else
        {
            pWnd->EnableWindow(TRUE);
            if( 0 != g_tCfg.m_dwDBHostIP )
            {
                in.s_addr = g_tCfg.m_dwDBHostIP;
                sprintf(achConf ,"%s", inet_ntoa(in));
            }
            else
            {
                memcpy(achConf, "0", sizeof("0"));
            }
            pWnd->SetWindowText( achConf );
        }

        // 数据库用户名
        pWnd = GetDlgItem(IDC_EDIT_DBUSERNAME);	
	    if( NULL == pWnd )
	    {
		    OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_DBUSERNAME) error!\n");
	    }
        else
        {
            pWnd->EnableWindow(TRUE);
            sprintf(achConf, "%s", g_tCfg.m_achDBUsername);
            pWnd->SetWindowText(achConf);
        }

        // 数据库用户密码
        pWnd = GetDlgItem(IDC_EDIT_DBPASSWORD);	
	    if( NULL == pWnd )
	    {
		    OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_DBPASSWORD) error!\n");
	    }
        else
        {
            pWnd->EnableWindow(TRUE);
            sprintf(achConf, "%s", g_tCfg.m_achDBPassword);
            pWnd->SetWindowText(achConf);
        }

        // FTP 路径
        pWnd = GetDlgItem(IDC_EDIT_VODFTPDIR);	
	    if( NULL == pWnd )
	    {
		    OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_VODFTPDIR) error!\n");
	    }
        else
        {
            pWnd->EnableWindow(TRUE);
            sprintf(achConf, "%s", g_tCfg.m_achVodFtpPatch);
            pWnd->SetWindowText(achConf);
        }

        // FTP用户名
        pWnd = GetDlgItem(IDC_EDIT_VODFTPUNAME);	
	    if( NULL == pWnd )
	    {
		    OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_VODFTPUNAME) error!\n");
	    }
        else
        {
            pWnd->EnableWindow(TRUE);
            sprintf(achConf, "%s", g_tCfg.m_achFTPUsername);
            pWnd->SetWindowText(achConf);
        }

        // FTP密码
        pWnd = GetDlgItem(IDC_EDIT_VODFTPPW);	
	    if( NULL == pWnd )
	    {
		    OspPrintf(TRUE, FALSE, "GetDlgItem(IDC_EDIT_VODFTPPW) error!\n");
	    }
        else
        {
            pWnd->EnableWindow(TRUE);
            sprintf(achConf, "%s", g_tCfg.m_achFTPPassword);
            pWnd->SetWindowText(achConf);
  
        }
    }
    else
    {
        pWnd = GetDlgItem(IDC_EDIT_DBADDR);  // db's ip
        pWnd->EnableWindow(FALSE);

        pWnd = GetDlgItem(IDC_EDIT_DBUSERNAME); // db's user name
        pWnd->EnableWindow(FALSE);

        pWnd = GetDlgItem(IDC_EDIT_DBPASSWORD);// db's password
        pWnd->EnableWindow(FALSE);

        pWnd = GetDlgItem(IDC_EDIT_VODFTPDIR);// ftp;s dir
        pWnd->EnableWindow(FALSE);

        pWnd = GetDlgItem(IDC_EDIT_VODFTPUNAME);// ftp's name
        pWnd->EnableWindow(FALSE);

        pWnd = GetDlgItem(IDC_EDIT_VODFTPPW);// ftp's password
        pWnd->EnableWindow(FALSE);
    }
}

/*=============================================================================
  函 数 名： OnCheckIspublish
  功    能： 检测是否作为发布服务器
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CRecorderDlg::OnCheckIspublish() 
{
	// TODO: Add your control notification handler code here
	//m_bModified = TRUE;
    BOOL bRet = IsDlgButtonChecked(IDC_CHECK_ISPUBLISH);
    if(TRUE == bRet)
    {
        ChangeServiceInfo(TRUE);
    }
    else
    {
        ChangeServiceInfo(FALSE);
    }
	return;
}

void CRecorderDlg::OnChangeEditEqptype() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CRecorderDlg::OnChangeEditMcuid() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here    
    /*
	UpdateData(TRUE);    
    if( m_nMcuId > 300 || m_nMcuId < 0)
    {
        if(m_bOSEn)
        {
            AfxMessageBox("The MCU ID only be a number between 0 and 254");
        }
        else 
        {
            AfxMessageBox("MCU ID 只能在0－－254之间");
        }
    }
    */
    return;
}

void CRecorderDlg::OnChangeEditMcuport() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	//UpdateData(TRUE);
    /*
    if( m_nMcuPort < 0 || m_nMcuPort > 65535 )
    {
        if(m_bOSEn)
        {
            AfxMessageBox("The port number of MCU should be between 0 and 65535");
        }
        else 
        {
            AfxMessageBox("MCU 端口只能在0－－65535之间");
        }
    }
    */
    return;
}

void CRecorderDlg::OnChangeEditPlaychnlnum() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
//	UpdateData(TRUE);
//    if( m_nPlayChnNum < 0 || m_nPlayChnNum > MAXNUM_RECORDER_CHNNL )
//    {
//        if(m_bOSEn)
//        {
//            AfxMessageBox("The play channels should be between 0 and 32, and the sum of play and record channels cannot exceed 32.");
//        }
//        else 
//        {
//            AfxMessageBox("放像通道数只能在0－－32之间，且放像通道与录像通道之和不能超过32");
//        }
//    }
    return;
}

void CRecorderDlg::OnChangeEditRecchnlnum() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
//	UpdateData(TRUE);
//    if( m_nRecChnNum < 0 || m_nRecChnNum > MAXNUM_RECORDER_CHNNL )
//    {
//        if(m_bOSEn)
//        {
//            AfxMessageBox("The record channels should be between 0 and 32, and the sum of play and record channels cannot exceed 32.");
//        }
//        else 
//        {
//            AfxMessageBox("录像通道数只能在0－－32之间，且放像通道与录像通道之和不能超过32");
//        }
//    }
    return;
}

void CRecorderDlg::OnChangeEditEqpid() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
    //UpdateData(TRUE);
    /*
    if( m_nEqpId < 17 || m_nEqpId > 32 )
    {
        if(m_bOSEn)
		{
            AfxMessageBox("The recorder ID should be between 17 and 32");
        }
        else 
		{
            AfxMessageBox("录像机 ID 只能在17－－32之间");
        }
    }
    */
    return;
}

void CRecorderDlg::OnChangeEditMcuPortB()
{
    //UpdateData(TRUE);
    /*
    if(m_nMcuPortB < 0 || m_nMcuPortB > 65535 )
    {
        if(m_bOSEn)
		{
            AfxMessageBox("The port number of MCU should be between 0 and 65535");
        }
        else
		{
            AfxMessageBox("MCU 端口只能在0－－65535之间");
        }    
    }
    */
    return;
}

void CRecorderDlg::OnChangeEditStartplayport() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
//	UpdateData(TRUE);
//    if(abs(m_nPlayStartPort - m_nRecStartPort) < 100)
//    {
//        if(m_bOSEn)
//		{
//            AfxMessageBox("The port interval should be larger than 100.");
//        }
//        else 
//		{
//            AfxMessageBox("推荐录放像端口之间间隔在100以上");
//        }
//    }
    return;
}

void CRecorderDlg::OnChangeEditRecstartport() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
//	UpdateData(TRUE);
//    if(abs(m_nPlayStartPort - m_nRecStartPort) < 100)
//    {
//        if(m_bOSEn)
//		{
//            AfxMessageBox("The port interval should be larger than 100.");
//        }
//        else 
//		{
//            AfxMessageBox("推荐录放像端口之间间隔在100以上");
//        }
//    }
    return;
}

/*=============================================================================
  函 数 名： LanguageToEnglish
  功    能： 转换文本
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CRecorderDlg::LanguageToEnglish(void)
{
    CWnd* pWnd = GetDlgItem(IDC_STATIC_TOPIC);
    pWnd->SetWindowText("Recorder Settings");

    pWnd = GetDlgItem(IDC_STATIC_RECADDR);
    pWnd->SetWindowText("    Address        :");

    pWnd = GetDlgItem(IDC_STATIC_EQPALIAS);
    pWnd->SetWindowText("    Alias          :");

    pWnd = GetDlgItem(IDC_STATIC_EQPTYPE);
    pWnd->SetWindowText("    Category       :");

    pWnd = GetDlgItem(IDC_STATIC_EQPID);
    pWnd->SetWindowText("    ID             :");

    pWnd = GetDlgItem(IDC_STATIC_MCUAID);
    pWnd->SetWindowText("    MCU ID         :");

    pWnd = GetDlgItem(IDC_STATIC_MCUAADDR);
    pWnd->SetWindowText("    MCU Address    :");

    pWnd = GetDlgItem(IDC_STATIC_MCUAPORT);
    pWnd->SetWindowText("    MCU Port       :");

	/*
    pWnd = GetDlgItem(IDC_STATIC_MCUBID);
    pWnd->SetWindowText("MCU_B ID:");

    pWnd = GetDlgItem(IDC_STATIC_MCUBADDR);
    pWnd->SetWindowText("MCU_B Address:");

    pWnd = GetDlgItem(IDC_STATIC_MCUBPORT);
    pWnd->SetWindowText("MCU_B Port:");
	*/

    pWnd = GetDlgItem(IDC_STATIC_RECCHANNELS);
    pWnd->SetWindowText("    Record Channels:");
    
    pWnd = GetDlgItem(IDC_STATIC_PLAYCHANNELS);
    pWnd->SetWindowText("    Play Channels  :");

    pWnd = GetDlgItem(IDC_STATIC_RECSTARTPORT);
    pWnd->SetWindowText("    Recording Port :");

    pWnd = GetDlgItem(IDC_STATIC_PLAYSTARTPORT);
    pWnd->SetWindowText("    Playing Port   :");

    pWnd = GetDlgItem(IDC_STATIC_DIROFFILES);
    pWnd->SetWindowText("    File Location  :");

    pWnd = GetDlgItem(IDC_MACHINENAME);
    pWnd->SetWindowText("    Computer Name  :");

   	pWnd = GetDlgItem(IDC_CHECK_ISPUBLISH);
	pWnd->SetWindowText("Publish");
	
	pWnd = GetDlgItem(IDC_STATIC_DBADDRESS);
	pWnd->SetWindowText("    DB Address     :");
	
	pWnd = GetDlgItem(IDC_STATIC_DBUSERNAME);
	pWnd->SetWindowText("    DB Username    :");
	
	pWnd = GetDlgItem(IDC_STATIC_PWOFDB);
	pWnd->SetWindowText("    DB Password    :");
	
	pWnd = GetDlgItem(IDC_STATIC_DIROFVOD);
	pWnd->SetWindowText("    VOD Location   :");
	
	pWnd = GetDlgItem(IDC_STATIC_USERNAMEOFVOD);
	pWnd->SetWindowText("    VOD Username   :");
	
	pWnd = GetDlgItem(IDC_STATIC_PWOFVOD);
	pWnd->SetWindowText("    VOD Password   :");

    
    return;
}

void CRecorderDlg::LanguageToChinese(void)
{
    CWnd* pWnd = GetDlgItem(IDC_STATIC_TOPIC);
    pWnd->SetWindowText("录像机配置信息");

    pWnd = GetDlgItem(IDC_STATIC_RECADDR);
    pWnd->SetWindowText("      录像地址     :");

    pWnd = GetDlgItem(IDC_STATIC_EQPALIAS);
    pWnd->SetWindowText("      外设别名     :");

    pWnd = GetDlgItem(IDC_STATIC_EQPTYPE);
    pWnd->SetWindowText("      外设类型     :");

    pWnd = GetDlgItem(IDC_STATIC_EQPID);
    pWnd->SetWindowText("      外设ID       :");

    pWnd = GetDlgItem(IDC_STATIC_MCUAID);
    pWnd->SetWindowText("      MCU ID       :");

    pWnd = GetDlgItem(IDC_STATIC_MCUAADDR);
    pWnd->SetWindowText("      MCU 地址     :");

    pWnd = GetDlgItem(IDC_STATIC_MCUAPORT);
    pWnd->SetWindowText("      MCU通信端口  :");

	/*
    pWnd = GetDlgItem(IDC_STATIC_MCUBID);
    pWnd->SetWindowText("      MCU.B ID     :");

    pWnd = GetDlgItem(IDC_STATIC_MCUBADDR);
    pWnd->SetWindowText("      MCU.B 地址   :");

    pWnd = GetDlgItem(IDC_STATIC_MCUBPORT);
    pWnd->SetWindowText("      MCU.B通信端口:");
	*/

    pWnd = GetDlgItem(IDC_STATIC_RECCHANNELS);
    pWnd->SetWindowText("      录像通道数   :");
    
    pWnd = GetDlgItem(IDC_STATIC_PLAYCHANNELS);
    pWnd->SetWindowText("      放像通道数   :");

    pWnd = GetDlgItem(IDC_STATIC_RECSTARTPORT);
    pWnd->SetWindowText("      录像起始端口 :");

    pWnd = GetDlgItem(IDC_STATIC_PLAYSTARTPORT);
    pWnd->SetWindowText("      放像起始端口 :");

    pWnd = GetDlgItem(IDC_STATIC_DIROFFILES);
    pWnd->SetWindowText("      录像存储路径 :");

    pWnd = GetDlgItem(IDC_MACHINENAME);
    pWnd->SetWindowText("      计算机机器名 :");

	pWnd = GetDlgItem(IDC_CHECK_ISPUBLISH);
	pWnd->SetWindowText(" 支持发布");
	
	pWnd = GetDlgItem(IDC_STATIC_DBADDRESS);
	pWnd->SetWindowText("      数据库地址   :");
	
	pWnd = GetDlgItem(IDC_STATIC_DBUSERNAME);
	pWnd->SetWindowText("      数据库用户名 :");
	
	pWnd = GetDlgItem(IDC_STATIC_PWOFDB);
	pWnd->SetWindowText("      数据库密码   :");
	
	pWnd = GetDlgItem(IDC_STATIC_DIROFVOD);
	pWnd->SetWindowText("      VOD MMS目录  :");
	
	pWnd = GetDlgItem(IDC_STATIC_USERNAMEOFVOD);
	pWnd->SetWindowText("      VOD MMS用户名:");
	
	pWnd = GetDlgItem(IDC_STATIC_PWOFVOD);
	pWnd->SetWindowText("      VOD MMS密码  :");

    
    return;
}

void CRecorderDlg::GetChinesePlayChannelInfo(char* pchChlStr, TPlayerStatus& tPlayerStatus, TPlayerStatis& tPlayerStatis)
{
	s8 achItem[KDV_MAX_PATH+1];
    memset(achItem, 0, sizeof(achItem));
    
	sprintf(achItem ,"\n播放记录 : %s\n", tPlayerStatus.m_tPlayParam.m_achPlayFileName );
	strcat( pchChlStr ,achItem);

	sprintf(achItem, "已播放    :%d(秒)\n",tPlayerStatis.m_dwPlyTime );
	strcat( pchChlStr ,achItem);

	sprintf(achItem,"记录总长度:%d(秒)\n",tPlayerStatis.m_dwTotalPlyTime);
	strcat( pchChlStr ,achItem);

    strcat( pchChlStr ,"\n\t第一路视频\n");

	sprintf(achItem,"视频包数  :%d\n",tPlayerStatis.m_atMediaPlyStatis[1].m_dwNetFrmNum );
	strcat( pchChlStr ,achItem);

    strcat( pchChlStr ,"\n\t第二路视频\n");

    sprintf(achItem,"视频包数  :%d\n", tPlayerStatis.m_atMediaPlyStatis[2].m_dwNetFrmNum );
	strcat( pchChlStr ,achItem);

	sprintf(achItem,"\n语音包数  :%d\n\n", tPlayerStatis.m_atMediaPlyStatis[0].m_dwNetFrmNum );
	strcat( pchChlStr ,achItem);

	switch( tPlayerStatus.m_tPlyStatus ) 
	{
	case PLAYER_PLAY_START:
		strncpy( achItem ,"当前状态  : 播放\n" ,sizeof(achItem));
		achItem[KDV_MAX_PATH] ='\0';
		break;
	case PLAYER_PLAY_PAUSE:
		strncpy( achItem ,"当前状态  : 暂停\n" ,sizeof(achItem));
		achItem[KDV_MAX_PATH] ='\0';
		break;
	case PLAYER_PLAY_STOP:
		strncpy( achItem ,"当前状态  : 停止\n" ,sizeof(achItem));
		achItem[KDV_MAX_PATH] ='\0';
		break;
	case PLAYER_PLAY_QUICK:
		strncpy( achItem ,"当前状态  : 快进\n" ,sizeof(achItem));
		achItem[KDV_MAX_PATH] ='\0';
		break;
	case PLAYER_PLAY_SLOW:
		strncpy( achItem ,"当前状态  : 快退\n" ,sizeof(achItem));
		achItem[KDV_MAX_PATH] ='\0';
		break;
	default:
		strncpy( achItem ,"当前状态  : 未知\n" ,sizeof(achItem));
		achItem[KDV_MAX_PATH] ='\0';
		break;
	}
	strcat( pchChlStr ,achItem);
    return;
}

void CRecorderDlg::GetEnglishPlayChannelInfo(char* pchChlStr, TPlayerStatus& tPlayerStatus, TPlayerStatis& tPlayerStatis )
{
    s8 achItem[KDV_MAX_PATH+1];
    memset(achItem, 0, sizeof(achItem));
    
	sprintf(achItem ,"\nFilename: %s\n", tPlayerStatus.m_tPlayParam.m_achPlayFileName );
	strcat( pchChlStr ,achItem);

	sprintf(achItem,"Elapsed: %d(s)\n",tPlayerStatis.m_dwPlyTime );
	strcat( pchChlStr ,achItem);

	sprintf(achItem,"Total: %d(s)\n",tPlayerStatis.m_dwTotalPlyTime);
	strcat( pchChlStr ,achItem);

    strcat( pchChlStr ,"\n\tThe Primary Stream\n");

	sprintf(achItem,"Video Packets: %d\n",tPlayerStatis.m_atMediaPlyStatis[1].m_dwNetFrmNum );
	strcat( pchChlStr ,achItem);

    strcat( pchChlStr ,"\n\tThe Secondary Stream\n");

    sprintf(achItem,"Video Packets: %d\n", tPlayerStatis.m_atMediaPlyStatis[2].m_dwNetFrmNum );
	strcat( pchChlStr ,achItem);

	sprintf(achItem,"\nAudio Packets: %d\n\n", tPlayerStatis.m_atMediaPlyStatis[0].m_dwNetFrmNum );
	strcat( pchChlStr ,achItem);

	switch( tPlayerStatus.m_tPlyStatus ) 
	{
	case PLAYER_PLAY_START:
		strncpy( achItem ,"Current Status: Playing\n" ,sizeof(achItem));
		achItem[KDV_MAX_PATH] ='\0';
		break;
	case PLAYER_PLAY_PAUSE:
		strncpy( achItem ,"Current Status: Pause\n" ,sizeof(achItem));
		achItem[KDV_MAX_PATH] ='\0';
		break;
	case PLAYER_PLAY_STOP:
		strncpy( achItem ,"Current Status: Stop\n" ,sizeof(achItem));
		achItem[KDV_MAX_PATH] ='\0';
		break;
	case PLAYER_PLAY_QUICK:
		strncpy( achItem ,"Current Status: FF\n" ,sizeof(achItem));
		achItem[KDV_MAX_PATH] ='\0';
		break;
	case PLAYER_PLAY_SLOW:
		strncpy( achItem ,"Current Status: Rew\n" ,sizeof(achItem));
		achItem[KDV_MAX_PATH] ='\0';
		break;
	default:
		strncpy( achItem ,"Current Status: Unknow\n" ,sizeof(achItem));
		achItem[KDV_MAX_PATH] ='\0';
		break;
	}
	strcat( pchChlStr ,achItem);
    return;
}

void CRecorderDlg::GetChineseRecorderChannelInfo(char* pchChlStr, TRecorderStatus& tRecorderStatus, TRecorderStatis& tRecorderStatis)
{
	s8 achItem[KDV_MAX_PATH+1];
    memset(achItem, 0, sizeof(achItem));

    sprintf(achItem ,"\n录像名 : %s\n", tRecorderStatus.m_tRecParam.m_achRecFileName );
	strcat( pchChlStr ,achItem);

	sprintf(achItem,"已录制    :%d(秒)\n",tRecorderStatis.m_dwRecTime );
	strcat( pchChlStr ,achItem);

	u32 dwLostFrm = 0;
	u8 byLoop = 0;

	u8 byMediaType = 255;
	for (byLoop = 0; byLoop < MAX_STREAM_NUM; ++byLoop)
	{
		byMediaType = tRecorderStatis.m_atMediaRecStatis[byLoop].m_byMediaType;
		if (byMediaType == MEDIA_TYPE_MP3 || byMediaType == MEDIA_TYPE_G7221C 
			|| byMediaType == MEDIA_TYPE_G719 || byMediaType == MEDIA_TYPE_PCMA
			|| byMediaType == MEDIA_TYPE_PCMU || byMediaType == MEDIA_TYPE_G721
			|| byMediaType == MEDIA_TYPE_G722 || byMediaType == MEDIA_TYPE_G7231
			|| byMediaType == MEDIA_TYPE_ADPCM || byMediaType == MEDIA_TYPE_G728
			|| byMediaType == MEDIA_TYPE_G729 || byMediaType == MEDIA_TYPE_G7221
			|| byMediaType == MEDIA_TYPE_AACLC || byMediaType == MEDIA_TYPE_AACLD)
		{
			sprintf(achItem, "\n第(%d)路码流(音频)\n", byLoop+1);
		}
		else if (byMediaType == MEDIA_TYPE_MP4 || byMediaType == MEDIA_TYPE_H261 
			|| byMediaType == MEDIA_TYPE_H262 || byMediaType == MEDIA_TYPE_H263
			|| byMediaType == MEDIA_TYPE_H263PLUS || byMediaType == MEDIA_TYPE_H264
			|| byMediaType == MEDIA_TYPE_H264_ForHuawei || byMediaType == MEDIA_TYPE_FEC)
		{
			sprintf(achItem, "\n第(%d)路码流(视频)\n", byLoop+1);
		}
		else
		{
			sprintf(achItem, "\n第(%d)路码流\n", byLoop+1);
		}

		strcat(pchChlStr, achItem);

		sprintf(achItem,"\t收包  :%d 帧\n", tRecorderStatis.m_atMediaRecStatis[byLoop].m_dwNetFrmNum);
		strcat( pchChlStr ,achItem);

		dwLostFrm = tRecorderStatis.m_atMediaRecStatis[byLoop].m_dwBufFullLostFrmNum;
		dwLostFrm += tRecorderStatis.m_atMediaRecStatis[byLoop].m_dwErrFrmLostFrmNum;
		
		sprintf(achItem,"\t丢包  :%d 帧\n", dwLostFrm );
		strcat( pchChlStr ,achItem);
	}

// 	strcat( pchChlStr ,"\n第一路视频\n");
// 
// 	sprintf(achItem,"\t收包  :%d 帧\n",
// 		tRecorderStatis.m_atMediaRecStatis[1].m_dwNetFrmNum);
// 	strcat( pchChlStr ,achItem);
// 
//     u32 dwLostFrm = tRecorderStatis.m_atMediaRecStatis[1].m_dwBufFullLostFrmNum;
//     dwLostFrm += tRecorderStatis.m_atMediaRecStatis[1].m_dwErrFrmLostFrmNum;
// 
// 	sprintf(achItem,"\t丢包  :%d 帧\n",
// 		dwLostFrm );
// 	strcat( pchChlStr ,achItem);
// 
//     strcat( pchChlStr, "\n第二路视频\n");
//     
//     sprintf(achItem,"\t收包  :%d 帧\n",
// 		tRecorderStatis.m_atMediaRecStatis[2].m_dwNetFrmNum);
// 	strcat( pchChlStr ,achItem);
// 
//     dwLostFrm = tRecorderStatis.m_atMediaRecStatis[2].m_dwBufFullLostFrmNum;
//     dwLostFrm += tRecorderStatis.m_atMediaRecStatis[2].m_dwErrFrmLostFrmNum;
// 
// 	sprintf(achItem,"\t丢包  :%d 帧\n",
// 		dwLostFrm );
// 	strcat( pchChlStr ,achItem);
// 
// 	strcat( pchChlStr ,"\n音频\n");
// 
// 	sprintf(achItem,"\t收包  :%d 帧\n",
// 		tRecorderStatis.m_atMediaRecStatis[0].m_dwNetFrmNum);
// 	strcat( pchChlStr ,achItem);
// 
//     dwLostFrm = tRecorderStatis.m_atMediaRecStatis[0].m_dwBufFullLostFrmNum;
//     dwLostFrm += tRecorderStatis.m_atMediaRecStatis[0].m_dwErrFrmLostFrmNum;
// 
// 	sprintf(achItem,"\t丢包  :%d 帧\n\n",
// 		dwLostFrm);
// 	strcat( pchChlStr ,achItem);

	strcat( pchChlStr, "\n当前状态  : ");
	switch( tRecorderStatus.m_tRecStatus.m_tRecState  ) 
	{
	case REC_RUNNING:
		strncpy( achItem, "录像\n" ,sizeof(achItem));
		break;
	case REC_PAUSE:
		strncpy( achItem ,"暂停\n" ,sizeof(achItem));
		break;
	case REC_STOP:
		strncpy( achItem ,"停止\n" ,sizeof(achItem));
		break;
	default:
		strncpy( achItem ,"未知\n" ,sizeof(achItem));
		break;
	}
	achItem[KDV_MAX_PATH] ='\0';
	strcat( pchChlStr ,achItem);
}

void CRecorderDlg::GetEnglishRecorderChannelInfo(char* pchChlStr, TRecorderStatus& tRecorderStatus, TRecorderStatis& tRecorderStatis)
{
	
    s8 achItem[KDV_MAX_PATH+1];
    memset(achItem, 0, sizeof(achItem));

    sprintf(achItem ,"\nFilename: %s\n", tRecorderStatus.m_tRecParam.m_achRecFileName );
	strcat( pchChlStr ,achItem);

	sprintf(achItem,"Time: %d(s)\n",tRecorderStatis.m_dwRecTime );
	strcat( pchChlStr ,achItem);

	u32 dwLostFrm = 0;
	u8 byLoop = 0;
	
	u8 byMediaType = 255;
	for (byLoop = 0; byLoop < MAX_STREAM_NUM; ++byLoop)
	{
		byMediaType = tRecorderStatis.m_atMediaRecStatis[byLoop].m_byMediaType;
		if (byMediaType == MEDIA_TYPE_MP3 || byMediaType == MEDIA_TYPE_G7221C 
			|| byMediaType == MEDIA_TYPE_G719 || byMediaType == MEDIA_TYPE_PCMA
			|| byMediaType == MEDIA_TYPE_PCMU || byMediaType == MEDIA_TYPE_G721
			|| byMediaType == MEDIA_TYPE_G722 || byMediaType == MEDIA_TYPE_G7231
			|| byMediaType == MEDIA_TYPE_ADPCM || byMediaType == MEDIA_TYPE_G728
			|| byMediaType == MEDIA_TYPE_G729 || byMediaType == MEDIA_TYPE_G7221
			|| byMediaType == MEDIA_TYPE_AACLC || byMediaType == MEDIA_TYPE_AACLD)
		{
			sprintf(achItem, "\nStream Number (%d)(Audio)\n", byLoop+1);
		}
		else if (byMediaType == MEDIA_TYPE_MP4 || byMediaType == MEDIA_TYPE_H261 
			|| byMediaType == MEDIA_TYPE_H262 || byMediaType == MEDIA_TYPE_H263
			|| byMediaType == MEDIA_TYPE_H263PLUS || byMediaType == MEDIA_TYPE_H264
			|| byMediaType == MEDIA_TYPE_H264_ForHuawei || byMediaType == MEDIA_TYPE_FEC)
		{
			sprintf(achItem, "\nStream Number (%d)(Video)\n", byLoop+1);
		}
		else
		{
			sprintf(achItem, "\nStream Number (%d)\n", byLoop+1);
		}
		
		strcat(pchChlStr, achItem);
		
		sprintf(achItem,"\tRx Packets  :%d frames\n", tRecorderStatis.m_atMediaRecStatis[byLoop].m_dwNetFrmNum);
		strcat( pchChlStr ,achItem);
		
		dwLostFrm = tRecorderStatis.m_atMediaRecStatis[byLoop].m_dwBufFullLostFrmNum;
		dwLostFrm += tRecorderStatis.m_atMediaRecStatis[byLoop].m_dwErrFrmLostFrmNum;
		
		sprintf(achItem,"\tLost Packets  :%d frames\n", dwLostFrm );
		strcat( pchChlStr ,achItem);
	}

// 	strcat( pchChlStr ,"\nThe Primary Stream\n");
// 
// 	sprintf(achItem,"\tRx Packets: %d Frames\n",
// 		tRecorderStatis.m_atMediaRecStatis[1].m_dwNetFrmNum);
// 	strcat( pchChlStr ,achItem);
// 
//     u32 dwLostFrm = tRecorderStatis.m_atMediaRecStatis[1].m_dwBufFullLostFrmNum;
//     dwLostFrm += tRecorderStatis.m_atMediaRecStatis[1].m_dwErrFrmLostFrmNum;
// 
// 	sprintf(achItem,"\tLost Packets: %d Frames\n",
// 		dwLostFrm );
// 	strcat( pchChlStr ,achItem);
// 
//     strcat( pchChlStr, "\nThe Secondary Stream\n");
//     
//     sprintf(achItem,"\tRx Packets: %d Frames\n",
// 		tRecorderStatis.m_atMediaRecStatis[2].m_dwNetFrmNum);
// 	strcat( pchChlStr ,achItem);
// 
//     dwLostFrm = tRecorderStatis.m_atMediaRecStatis[2].m_dwBufFullLostFrmNum;
//     dwLostFrm += tRecorderStatis.m_atMediaRecStatis[2].m_dwErrFrmLostFrmNum;
// 
// 	sprintf(achItem,"\tLost Packets: %d Frames\n",
// 		dwLostFrm );
// 	strcat( pchChlStr ,achItem);
// 
// 	strcat( pchChlStr ,"\nAudio\n");
// 
// 	sprintf(achItem,"\tRx Packets: %d Frames\n",
// 		tRecorderStatis.m_atMediaRecStatis[0].m_dwNetFrmNum);
// 	strcat( pchChlStr ,achItem);
// 
//     dwLostFrm = tRecorderStatis.m_atMediaRecStatis[0].m_dwBufFullLostFrmNum;
//     dwLostFrm += tRecorderStatis.m_atMediaRecStatis[0].m_dwErrFrmLostFrmNum;
// 
// 	sprintf(achItem,"\tLost Packets: %d Frames\n\n",
// 		dwLostFrm);
// 	strcat( pchChlStr ,achItem);

	strcat( pchChlStr, "\nCurrent Status: ");
	switch( tRecorderStatus.m_tRecStatus.m_tRecState  ) 
	{
	case REC_RUNNING:
		strncpy( achItem ,"Recordering\n" ,sizeof(achItem));
		break;
	case REC_PAUSE:
		strncpy( achItem ,"Pause\n" ,sizeof(achItem));
		break;
	case REC_STOP:
		strncpy( achItem ,"Stop\n" ,sizeof(achItem));
		break;
	default:
		strncpy( achItem ,"Unknow\n" ,sizeof(achItem));	
		break;
	}
	achItem[KDV_MAX_PATH] ='\0';
	strcat( pchChlStr ,achItem);
}

/*====================================================================
    函数名      ：OnOutput
    功能        ：输出录像机信息
    输入说明    ：无
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
====================================================================*/
/*void CRecorderDlg::OnOutput(WPARAM w, LPARAM l) 
{
	s32 nFillPos;
	s8* lpszInfo = (s8*)w;
	if( m_nTrcInfoSize < MAXLINE_RUNINFO )
	{
		m_nTrcInfoSize++;
		nFillPos = (m_nTrcInfoBeginPos + m_nTrcInfoSize)%MAXLINE_RUNINFO;
	}
	else
	{
		nFillPos = m_nTrcInfoBeginPos;
		m_nTrcInfoBeginPos = (m_nTrcInfoBeginPos+1)%MAXLINE_RUNINFO;
	}

	memcpy( m_tTrcInfo[nFillPos].achInfo ,lpszInfo ,KDV_MAX_PATH);
	m_tTrcInfo[nFillPos].achInfo[KDV_MAX_PATH-1] ='\0';
	if( m_dwShowInfoType == TRC_HEAD)
		PostMessage( WM_RECORD_SHOWTRC ,0 ,0);
}
*/

/*=============================================================================
函 数 名： IsValidIpV4
功    能： 
算法实现： 
全局变量： 
参    数： char* psIPStr
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/3/27   4.0			周广程                  创建
=============================================================================*/
BOOL32 CRecorderDlg::IsValidIpV4( char * psIPStr)
{
    if ( psIPStr == NULL ) 
    {		
        return FALSE;
    }
	
    u8 byDot    = 0;                      //  已处理的字符串里的点号数
    u16 wData   = 0;                     //  已处理的字符串里的数值
    u8 byPos	= 0;					  //  两点之间的间隔
    u8 byLen    = _tcslen( psIPStr );     //  字符串长度
	
    //  字符串最少7位最长15位, 且不能以'0'开头
    if ( byLen < 7 || byLen > 15 ) 
    {
        // 2004/05/19 李洪强 *.*.*.*和***.***.***.***		
        return FALSE;
    }
	
    if ( psIPStr[0] == '0'
		|| ( psIPStr[0] == '2' && psIPStr[1] == '5'
		&& psIPStr[2] == '5' ) )
    {
        return FALSE;
    }
	
    if ( psIPStr[0] == '.' || psIPStr[byLen - 1] == '.' )
    {		
        return FALSE;
    }
	
	u8 byCount = 0;
    for ( byCount = 0; byCount < byLen; ++byCount )
    {
        if ( psIPStr[byCount] >= '0' && psIPStr[byCount] <= '9' )//遇到数字
        {
            wData = 10 * wData + psIPStr[byCount] - '0';     //计算数值
			++byPos;
        }
        else if ( psIPStr[byCount] == '.' )     //  遇到点号
        {
            byDot++;
            if ( byDot > 3 )  //  点号数大于3
            {				
				return FALSE;
            }
			else if ( byPos > 3 )
			{				
				return FALSE;
			}
			else if ( wData > 255 ) //数值大于255
			{		
				return FALSE;
			}
            else        //  已处理的字符串合法
            {
                wData = 0;
				byPos = 0;
            }
        }
        else            //  遇到非法字符
        {			
			return FALSE;
        }
    }
	
    if ( byDot != 3 )
    {		
		return FALSE;
    }
    
    // 顾振华@2006.03.17 检查最后一个数字是否合法
    if ( wData > 255 ) //数值大于255
	{		
		return FALSE;
	}    
	
    return TRUE;
}