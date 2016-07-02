// MacSNDlg.cpp : implementation file
//

#include "stdafx.h"
#include "mwt.h"
#include "MacSNDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMacSNDlg dialog


CMacSNDlg::CMacSNDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMacSNDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMacSNDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMacSNDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMacSNDlg)
	DDX_Control(pDX, IDC_CB_NUM, m_cbMacNum);
	DDX_Control(pDX, IDC_LS_MACSN, m_lsMacSn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMacSNDlg, CDialog)
	//{{AFX_MSG_MAP(CMacSNDlg)
	ON_BN_CLICKED(IDC_BTN_ADD, OnBtnAdd)
	ON_CBN_SELCHANGE(IDC_CB_NUM, OnSelchangeCbNum)
	ON_NOTIFY(NM_CLICK, IDC_LS_MACSN, OnClickLsMacsn)
	ON_BN_CLICKED(IDC_BTN_EDIT, OnBtnEdit)
	ON_BN_CLICKED(IDC_BTN_DEL, OnBtnDel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMacSNDlg message handlers


BOOL CMacSNDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    // 初始化列表
    m_lsMacSn.InsertColumn( 0,  "序列号",  LVCFMT_CENTER, 160 );
    m_lsMacSn.InsertColumn( 1, "地址个数", LVCFMT_CENTER, 70);
    m_lsMacSn.InsertColumn( 2, "MAC地址0", LVCFMT_CENTER, 140 );
    m_lsMacSn.InsertColumn( 3, "MAC地址1", LVCFMT_CENTER, 140);
    
	m_lsMacSn.SetExtendedStyle(m_lsMacSn.GetExtendedStyle()
					   		   | LVS_EX_FULLROWSELECT );

    m_cbMacNum.SetCurSel( m_dwMacNum - 1 );
    OnSelchangeCbNum();

    // 初始化MAC地址
    CMacAddr cAddr = m_cCurMac;
    SetDlgItemText( IDC_ET_MAC1, cAddr.ToString() );    
    if ( m_dwMacNum == 2 )
    {        
        cAddr ++;
        SetDlgItemText( IDC_ET_MAC2, cAddr.ToString() );
    }

    LoadMacSN();
    
    GetDlgItem(IDC_ET_SN)->SetFocus();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// 从数据结构载入到界面
void CMacSNDlg::LoadMacSN()
{
    vector<SNItem>::const_iterator citr = g_SNLogMsg.m_vtAddedSNItem.begin();
    vector<SNItem>::const_iterator citrEnd = g_SNLogMsg.m_vtAddedSNItem.end();

    m_lsMacSn.DeleteAllItems();

    int i = 0;
    CString cstr;
    CMacAddr cAddr;
    while ( citr != citrEnd )
    {
        int nItem = m_lsMacSn.InsertItem(m_lsMacSn.GetItemCount(), citr->cstrSN );
        m_lsMacSn.SetItemData( nItem, i );
        
        cstr.Format("%u", citr->dwMACCount);            
        m_lsMacSn.SetItemText( nItem, 1, cstr );
        cAddr = citr->atU48[0];
        m_lsMacSn.SetItemText( nItem, 2, cAddr.ToString() );
        if ( citr->dwMACCount == 2)
        {
            cAddr = citr->atU48[1];
            m_lsMacSn.SetItemText( nItem, 3, cAddr.ToString() );
        }

        i++;
        citr++;
    }
}



void CMacSNDlg::OnSelchangeCbNum() 
{
    BOOL bDuoble = ( m_cbMacNum.GetCurSel() == 1 );

    // 第二路永远不能编辑
    GetDlgItem(IDC_ET_MAC2)->EnableWindow( FALSE );

    m_dwMacNum = m_cbMacNum.GetCurSel() + 1;

    if ( m_dwMacNum == 1)
    {
        SetDlgItemText(IDC_ET_MAC2, "");
    }
    else
    {
        CString cstr;
        GetDlgItemText(IDC_ET_MAC1, cstr);
        CMacAddr cAddr;
        if ( cAddr.Str2u48( cstr ) )
        {
            cAddr ++;
            SetDlgItemText(IDC_ET_MAC2, cAddr.ToString());            
        }
    }


}

void CMacSNDlg::OnClickLsMacsn(NMHDR* pNMHDR, LRESULT* pResult) 
{
    POSITION pos = m_lsMacSn.GetFirstSelectedItemPosition();

    if (pos == NULL)
    {
        return;
    }

    int nItem = m_lsMacSn.GetNextSelectedItem(pos);
    DWORD dwIndex = m_lsMacSn.GetItemData( nItem );

    if (dwIndex >= g_SNLogMsg.m_vtAddedSNItem.size())
    {
        return;
    }

    SNItem item = g_SNLogMsg.m_vtAddedSNItem[dwIndex];

    // 刷新界面
    CString cstr;
    SetDlgItemText( IDC_ET_SN, item.cstrSN );
    m_dwMacNum = item.dwMACCount;
    m_cbMacNum.SetCurSel( m_dwMacNum - 1 );
    CMacAddr cAddr;
    cAddr = item.atU48[0];
    SetDlgItemText( IDC_ET_MAC1, cAddr.ToString());
    if ( m_dwMacNum == 2 )
    {
        cAddr = item.atU48[1];
        SetDlgItemText( IDC_ET_MAC2, cAddr.ToString());
    }
    else
    {
        SetDlgItemText( IDC_ET_MAC2, "");
    }
	
	*pResult = 0;
}

// 验证界面用户输入
BOOL CMacSNDlg::ValidateInput(SNItem &item)
{
    CString cstrSN;
    GetDlgItemText( IDC_ET_SN, cstrSN);

    CString cstr;
    CMacAddr cAddr1, cAddr2;
    GetDlgItemText(IDC_ET_MAC1, cstr);
    BOOL bValid1 = cAddr1.Str2u48(cstr);

    // 两个Mac
    BOOL bValid2 = TRUE;
    if ( m_dwMacNum == 2 )
    {
        GetDlgItemText(IDC_ET_MAC2, cstr);
        bValid2 = cAddr2.Str2u48(cstr);        
    }

    if ( cstrSN.IsEmpty() )
    {
        AfxMessageBox("序列号不能为空");
        GetDlgItem(IDC_ET_SN)->SetFocus();
        return FALSE;
    }
	
    if (!bValid1)
    {
        CString cstr2;
        GetDlgItemText(IDC_ET_MAC1, cstr);
        cstr2.Format("MAC 地址 %s 非法!", cstr);
        AfxMessageBox(cstr2);
        GetDlgItem(IDC_ET_MAC1)->SetFocus();
        return FALSE;
    }

    if (!bValid2)
    {
        CString cstr2;
        GetDlgItemText(IDC_ET_MAC2, cstr);
        cstr2.Format("MAC 地址 %s 非法!", cstr);
        AfxMessageBox(cstr2);
        GetDlgItem(IDC_ET_MAC2)->SetFocus();
        return FALSE;
    }

    item.cstrSN = cstrSN;
    item.dwMACCount = m_dwMacNum;
    item.atU48[0] = cAddr1.GetU48();
    if ( m_dwMacNum == 2 )
    {
        item.atU48[1] = cAddr2.GetU48();
    }
    return TRUE;
}


void CMacSNDlg::OnBtnAdd() 
{
    SNItem item;

    if (!ValidateInput(item))
    {
        return;
    }

    if ( g_SNLogMsg.IsSNExistInHistory(item) )
    {
        AfxMessageBox("该序列号在历史记录中已经被使用!");
        GetDlgItem(IDC_ET_SN)->SetFocus();
        return ;
    }

    if ( g_SNLogMsg.IsSNExistInNew(item) )
    {
        AfxMessageBox("该序列号本次已添加!");
        GetDlgItem(IDC_ET_SN)->SetFocus();
        return ;
    }

    g_SNLogMsg.AddNewItem(item);

    LoadMacSN();

    // 自动增量
    if ( m_dwMacNum == 1 )
    {
        m_cCurMac = item.atU48[0];
    }
    else
    {
        m_cCurMac = item.atU48[1];
    }
    m_cCurMac++;

    CMacAddr cAddr = m_cCurMac;

    SetDlgItemText( IDC_ET_MAC1, cAddr.ToString() );
    if ( m_dwMacNum == 2 )
    {
        cAddr ++;
        SetDlgItemText( IDC_ET_MAC2, cAddr.ToString() );
    }

    GetDlgItem(IDC_ET_SN)->SetFocus();
}


void CMacSNDlg::OnBtnEdit() 
{
    POSITION pos = m_lsMacSn.GetFirstSelectedItemPosition();

    if (pos == NULL)
    {
        AfxMessageBox("请先选择需要修改的条目!");
        return;
    }

    int nItem = m_lsMacSn.GetNextSelectedItem(pos);
    DWORD dwIndex = m_lsMacSn.GetItemData( nItem );

    if (dwIndex >= g_SNLogMsg.m_vtAddedSNItem.size())
    {
        return;
    }

    SNItem item;
    if (!ValidateInput(item))
    {
        return;
    }

    g_SNLogMsg.m_vtAddedSNItem[dwIndex] = item;

    LoadMacSN();

    GetDlgItem(IDC_ET_SN)->SetFocus();

    
}

void CMacSNDlg::OnBtnDel() 
{
    POSITION pos = m_lsMacSn.GetFirstSelectedItemPosition();

    if (pos == NULL)
    {
        AfxMessageBox("请先选择需要删除的条目!");
        return;
    }

    int nItem = m_lsMacSn.GetNextSelectedItem(pos);
    DWORD dwIndex = m_lsMacSn.GetItemData( nItem );

    if (dwIndex >= g_SNLogMsg.m_vtAddedSNItem.size())
    {
        return;
    }	

    g_SNLogMsg.m_vtAddedSNItem.erase( &g_SNLogMsg.m_vtAddedSNItem[dwIndex] );

    LoadMacSN();

    GetDlgItem(IDC_ET_SN)->SetFocus();
}


void CMacSNDlg::OnOK() 
{
	CDialog::OnOK();
}