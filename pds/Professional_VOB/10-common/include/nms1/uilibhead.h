/*****************************************************************************
模块名      : 界面控件库
文件名      : UILibHead.h
相关文件    : UITools.cpp
              CheckButton.cpp
              FlatButton.cpp
              SkinButton.cpp
              SkinEdit.cpp
              SkinComboBox.cpp
              SkinHeadCtrl.cpp
              SkinListCtrl.cpp
              SkinIPCtrl.cpp
              SkinMenu.cpp
              SkinScroll.cpp
              SkinSliderCtrl.cpp
              StaticEx.cpp
              SkinFrame.cpp
              FlatDDButton.cpp
              SkinDDListCtrl.cpp
              TextButton30.cpp
              SkinGroup.cpp
              SkinTab.cpp
              SkinPage.cpp
              SkinSheet.cpp
              SkinCheckList.cpp
文件实现功能: 美化windows标准控件
作者        : 朱允荣
版本        : V1.0  Copyright(C) 1997-2004 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2004/03/24  1.0         朱允荣      创建
2004/07/20  1.0         朱允荣      增加拖拉列表，按钮。
2004/07/21  1.0         朱允荣      增加Text30。
2004/07/30  1.0         朱允荣      增加Group。
2004/08/31  1.0         朱允荣      增加Tab,Sheet,Page系列。
2004/09/20  1.0         朱允荣      增加CheckListBox。
2005/03/28  1.0         朱允荣      增加FlatButton图片时的文字显示。
2005/03/28  1.0         朱允荣      增加增加所有控件的静态颜色配置。
******************************************************************************/

#ifndef __UILIBHEAD_H
#define __UILIBHEAD_H

#if _MSC_VER > 1000
#pragma once
#endif

#include <afxtempl.h>
#include <afxole.h>
#include <afxdtctl.h>
#pragma warning(disable:4786)
#include <map>
using namespace std;

//list排序比较
typedef int (CALLBACK *PFNNEWCOMPARE)(LPARAM, LPARAM, LPARAM, LPARAM);


#define USESKINSCROLL

//ToolTip显示时间，毫秒
#define TOOLTIP_SHOWTIME            (int)10000
//从右向左显示的扩展风格  //add by zh.   20130530
#ifndef WS_EX_LAYOUT_RTL
#define WS_EX_LAYOUT_RTL     0x00400000L
#endif

//UI工具类  
class CUITools
{
public:
	CUITools();
	~CUITools();
    enum{STEPBK_VERT = 0, STEPBK_HORZ}; //渐进色的方向， DrawStepBk。
public:
    /*====================================================================
    函数名	：    CreateImage
    功能		：根据BMP资源号得到图片句柄
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：nBitmap 图片资源号
    返回值说明：  图片句柄，不成功为NULL。
    ====================================================================*/
    static HBITMAP CreateImage(UINT nBitmap);
	
	/*====================================================================
    函数名	：    CreateImage
    功能		：从文件读取BMP
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：nBitmap 图片资源号
    返回值说明：  图片句柄，不成功为NULL。
    ====================================================================*/
	static HBITMAP CreateImage( LPCTSTR lpszBitmap );

    /*====================================================================
    函数名	：    DrawStepBk
    功能		：画渐进色
    算法实现	：分三色计算颜色变换的速度，然后计算每次的颜色，为了提高效率
                    每次画两象素的线
    引用全局变量：无
    输入参数说明：pDC 作图的CDC指针
                  crFrom 开始的颜色，最上面（或最左边）的颜色。
                  crTo 结束的颜色，最下面（或最右边）的颜色。
                  rcAll  计算颜色渐变的整个区域范围。
                  rcItem 实际的作图的区域。
                  nStyle 渐变的方向STEPBK_VERT，从上到下，STEPBK_HORZ，从左到右。
    返回值说明：  无
    ====================================================================*/
    static void DrawStepBk(CDC *pDC, COLORREF crFrom, COLORREF crTo, CRect rcAll, CRect rcItem=CRect(0,0,0,0), UINT nStyle=STEPBK_VERT);
protected:
	
private:
};

//一些辅助类

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//    CDropInfo  用于传递拖拉信息的类                                   //
//////////////////////////////////////////////////////////////////////////
class CDropInfo : public CObject
{
public:
	CDropInfo();
	CDropInfo(COleDataObject* pDataObject,DWORD dwKeyState,
		DROPEFFECT dropEffect,DROPEFFECT dropEffectList,CPoint point );
	virtual ~CDropInfo();

public:
	//CDDTarget member functions params
	COleDataObject* m_pDataObject;
	DWORD m_dwKeyState;
	DROPEFFECT m_dropEffect;
	DROPEFFECT m_dropEffectList;
	CPoint m_point;

	DECLARE_DYNAMIC( CDropInfo );
};

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// CDDTarget 继承拖拉目标的类                                              //
// //////////////////////////////////////////////////////////////////////////
class CDDTarget : public COleDropTarget
{
public:
	BOOL MyRegist(CWnd* pWnd);
	CDDTarget();
	virtual ~CDDTarget();
    
protected:
    //处理OnDropEx消息是必须的，否则OnDrop函数不会被执行
    //当然也可以在OnDropEx函数中处理数据
	virtual DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
        DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT OnDropEx(CWnd* pWnd, COleDataObject* pDataObject,
		DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint point);
	virtual void OnDragLeave(CWnd* pWnd);
};



//自画的滚动条。
class CSkinScroll : public CStatic
{
// Construction
public:
	CSkinScroll();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinScroll)
    virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	void Set3DShow(BOOL bShow3D);
    //静态颜色配置  [zhuyr][3/28/2005]
    static void StaticSetChannelBk(COLORREF crBkFrom, COLORREF crBkTo=-1);
    static void StaticSetThumbColor(COLORREF crBk, COLORREF crFrame=-1);
    static void StaticSetArrowColor(COLORREF crFk, COLORREF crBk);

	void ReInit();
	UINT GetOldShowState();
	BOOL UpdateThumbSize();
	void SetShowAllways(BOOL bAllWays = TRUE);
	void UpdateRect();
	void ShowScroll(BOOL bShow = FALSE, BOOL bShowOld = FALSE);
	void SetChannelBk(COLORREF crBkFrom, COLORREF crBkTo=-1, BOOL bRepaint = FALSE);
	void SetThumbColor(COLORREF crBk, COLORREF crFrame=-1, BOOL bRepaint = FALSE);
	void SetArrowColor(COLORREF crFk, COLORREF crBk, BOOL bRepaint = FALSE);
	BOOL InitScrollBar(CWnd* pCtrl, UINT nStyle);
	void UpdateThumbPosition();
	virtual ~CSkinScroll();

	// Generated message map functions
protected:
	BOOL m_bShow3D; 
	BOOL m_bShowAllways;
	BOOL m_bOnce;
	int m_nLineWidth;
	int m_nThumbMetric;
	CRect m_rcFirstArrow;
    CRect m_rcLastArrow;
    CRect m_rcChannel;
    CRect m_rcThumb;
    
  
    BOOL m_bOldShow;
    BOOL m_bMouseDown;
	BOOL m_bLeftUp;
	BOOL m_bRightDown;
    BOOL m_bLeftUpPage;
    BOOL m_bRightDownPage;
	double m_dbThumbInterval;
	UINT m_nItemMetric;
	CWnd* m_pCtrl;
	UINT m_nScrollStyle;

    COLORREF m_crArrowFk;
    COLORREF m_crArrowBk;
    COLORREF m_crThumbBk;
    COLORREF m_crThumbFrame;
    COLORREF m_crChannelBk[2];

    //静态颜色配置  [zhuyr][3/28/2005]
    static COLORREF m_s_crArrowFk;
    static COLORREF m_s_crArrowBk;
    static COLORREF m_s_crThumbBk;
    static COLORREF m_s_crThumbFrame;
    static COLORREF m_s_crChannelBk[2];
    
	int m_nOldMax;
	int m_nPointInThumb;
	int m_nLenth;

	BOOL IsStopPage();
	void CreateArrowRgn(CRgn& rgnFirst, CRgn& rgnLast);
	virtual void DrawArrow(CDC* pDC, int nStyle, CRect rect);
	virtual void DrawThumb();
	void SendScrollMessage(UINT nMessageType, int nTime = 1);
	void LimitThumbPosition();
	//{{AFX_MSG(CSkinScroll)
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//菜单项信息。
class CMenuItem
{
private:
    LPTSTR m_szMenuText;
	LPTSTR m_szHotItem;
public:
	CMenuItem( LPCTSTR strText = NULL, HBITMAP hBitmap = NULL, HICON hIcon = NULL )
	{
        m_szMenuText = NULL;
        SetString( strText );
		m_hBitmap = hBitmap;
		m_hIcon   = hIcon;
        m_szHotItem = NULL;
        m_nFlags = 0;
        m_nID = -1;

        m_nWidth = 0;
        m_nHeight = 20;
	}
    virtual ~CMenuItem();

public:
    CString GetString();
	CString GetHotString();
	void SetHotString( LPCTSTR szString );
	void SetString( LPCTSTR szString );

    UINT        m_nFlags;
    UINT        m_nID;
    HBITMAP     m_hBitmap;
	HICON	    m_hIcon;
	int 	    m_nWidth;		
	int	        m_nHeight;
    CImageList* m_pImaList;
    int         m_nImage;
};


//列表头
class CSkinHeaderCtrl : public CHeaderCtrl
{
// Construction
public:
	CSkinHeaderCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinHeaderCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDS);
// Implementation
public:
	void Set3DShow(BOOL bShow3D);
	void SetDivideColor(COLORREF crDivide, int npenStyle = PS_SOLID, BOOL bRepaint = FALSE);
	void SetFrameColor(COLORREF crFrame, BOOL bRepaint = FALSE);
	CString GetItemText(int nItem) const;
	void SetSortArrow(int nSortCol, BOOL bSortAscending);
	void SetBkColor(COLORREF crBk, BOOL bRepaint = TRUE);
	void SetTextColor(COLORREF crText, BOOL bRepaint = TRUE);
	virtual ~CSkinHeaderCtrl();

    //  [zhuyr][3/29/2005]
    static void staticSetColors(COLORREF crText, COLORREF crBk, COLORREF crFrame = -1);
	// Generated message map functions
protected:
	void DrawArrow(CDC* pDC, CRect rect);
	BOOL m_bSortAscending;
	BOOL m_bShow3D;  
	UINT m_nSortCol;
    COLORREF m_crText;
	COLORREF m_crBk;
    COLORREF m_crFrame;
    CPen     m_penDivide;
    
    //静态配色方案  [zhuyr][3/29/2005]
    static COLORREF m_s_crText;
    static COLORREF m_s_crBk;
    static COLORREF m_s_crFrame;
	//{{AFX_MSG(CSkinHeaderCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
//
//      拖拉消息类
//////////////////////////////////////////////////////////////////////////

class CDragDropMsg
{
public:
    CDragDropMsg();
private:
    static UINT     m_nStartMsgID;
public:
#define MSG_BEGINDRAG       0
#define MSG_DRAGENTER       1
#define MSG_DRAGOVER        2
#define MSG_DROP            3
#define MSG_DROPEX          4
#define MSG_DRAGLEAVE       5
    static void SetStartMsgID(UINT nStartMsgID);
    static UINT GetStartMsgID();
    static BOOL HasSetMsgID();
    static UINT GetDDMsgID(UINT nMsgType);
};


/************************************************************************/
/*   菜单                                                               */
/************************************************************************/
class CSkinMenu : public CMenu  
{
    DECLARE_DYNAMIC(CSkinMenu)
public:
	BOOL CreatePopupMenu();
	
	int GetMenuString(UINT nIDItem, CString& rString, UINT nFlag = MF_BYPOSITION);
	int GetMenuString(UINT nIDItem, LPTSTR lpString, UINT nSize, UINT nFlag = MF_BYPOSITION);
  
	void SetMenuString(UINT nIDItem, LPCTSTR lpString, UINT nFlag = MF_BYPOSITION);
	LRESULT OnSkinMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
	void SetBkColor(COLORREF crFrom, COLORREF crTo, COLORREF crHilight = -1);
	void SetTextColor(COLORREF crText, COLORREF crHilight ,COLORREF crDisable = -1);

	BOOL LoadMenu(UINT uMenuID);
	BOOL LoadMenu(LPCTSTR lpszResourceName);
	
	BOOL InsertMenu(UINT nPosition, UINT nFlags, UINT nID, LPCTSTR lpstrText = NULL, CImageList *il=NULL,int xoffset=0);
	
	BOOL AppendMenu(UINT nFlags, UINT nIDNewItem, LPCTSTR lpszNewItem, UINT nIDBitmap=NULL,HICON hIcon=NULL);
	BOOL DeleteMenu(UINT nPosition, UINT nFlags);
	//重载
	BOOL TrackPopupMenu(UINT nFlags, int x, int y,
						CWnd* pWnd, LPCRECT lpRect = 0);

	static BOOL IsMenu(HMENU hMenu);

	CSize GetMenuSize();
	CSkinMenu* FindMenuOption(int nID, int& nLoc);

	CSkinMenu();
	virtual ~CSkinMenu();

	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	virtual BOOL DestroyMenu();

    /*=============================================================================
    函 数 名:SetFont
    功    能:设置菜单文字字体
    参    数:CFont *pcFont              [in]    字体
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/11/08  4.0     王昊    创建
    =============================================================================*/
    inline void SetFont(CFont *pcFont) { m_pcFont = pcFont; }

public:
    enum {TYPE_NORMAL = 0, TYPE_NMC};
    static int m_s_nType;
    static COLORREF m_s_crBk[3]; //0:From; 1:To; 2:Hilight
    static COLORREF m_s_crText[3];//0:Normal; 1:Hilight; 2:Disable
protected:
    void DrawItemText(CDC* pDC, COLORREF crText, CRect rect, CMenuItem* pItem);
	void DrawCheckMark(CDC* pDC,int x,int y,COLORREF color);
	void ApplyRTLStyleToSub();   //将从左右信息应用到下级菜单
protected:
	BOOL m_bLoadMenu;
	COLORREF m_crBk[3];
    COLORREF m_crText[3];

    static CTypedPtrArray<CPtrArray, HMENU>  m_AllSubMenus;
    CTypedPtrArray<CPtrArray,CMenuItem *> m_MenuList;
    CTypedPtrArray<CPtrArray, HMENU> m_SubMenus;
    int			m_curSel;

    int m_nWidth;
    int m_nHeigth;
	BOOL m_bRTLParent;    //记录父窗口的方向
    CFont   *m_pcFont;
};



/*******************************************************************
类名：CFlatButton
功能：平面效果按钮，设定字体颜色，背景图或可设背景色。
*********************************************************************/
class CFlatButton : public CButton
{
    DECLARE_DYNAMIC(CFlatButton)
// Construction
public:
	CFlatButton();

    //按钮风格
    static UINT m_s_nType;
    enum {TYPE_NORMAL = 0, TYPE_NMC};

    //图片上文字位置，下面定义位置与显示位置一致  [zhuyr][3/28/2005]
    enum enTextPos {TEXT_POS_LEFTTOP,     TEXT_POS_TOP,       TEXT_POS_RIGHTTOP,
                    TEXT_POS_LEFT,        TEXT_POS_CENTER,    TEXT_POS_RIGHT,
                    TEXT_POS_LEFTBOTTOM,  TEXT_POS_BOTTOM,    TEXT_POS_RIGHTBOTTOM,
                    TEXT_POS_CUSTOM};
// 接口
public:
    /*====================================================================
    函数名  ：SetBitmaps
    功  能  ：设定按钮的图片
    全局变量：无
    输入参数：nBitmapNormal: 按钮正常状态时，按钮图片的资源号。
              nBitmapOver:   按钮上有鼠标时，按钮图片的资源号。
	          nBitmapDown:   按钮被按下时，按钮图片的资源号。
	          hBitmapDisable:按钮失效时，按钮图片的资源号。  (或为图片句柄)
    返回值  ：无
    ====================================================================*/
	void SetBitmaps(UINT nBitmapNormal, UINT nBitmapOver = 0, UINT nBitmapDown = 0, UINT nBitmapDisable = 0);
	void SetBitmaps(HBITMAP hBitmapNormal, HBITMAP hBitmapOver = NULL, HBITMAP hBitmapDown = NULL, HBITMAP hBitmapDisable = NULL);

	/*====================================================================
    函数名  ：SetNeedRTLBack
    功  能  ：设置是否需要从右向左显示背景 仅在按钮风格为WS_EX_LAYOUT_RTL时有效
    全局变量：无
    输入参数：bNeed 是否需要
    返回值  ：无
    ====================================================================*/
	void SetNeedRTLBack(BOOL bNeed);
    
    /*====================================================================
    函数名  ：SizeToContent
    功  能  ：设定按钮和图片大小的适应关系。
    全局变量：无
    输入参数：bActivate:TRUE时按钮适应图片，FALSE图片适应按钮。
    返回值  ：无
    ====================================================================*/
	void SizeToContent(BOOL bEnable = TRUE);


    /*====================================================================
    函数名  ：SetBkColor
    功  能  ：设定按钮背景的颜色
    全局变量：无
    输入参数：crBkOut	鼠标不在按钮上时，按钮背景的颜色。
	          crBkIn	鼠标在按钮上时，按钮背景的颜色。
    	      crBkDown	按钮被按下时，按钮背景的颜色。
	          crBkDisable	按钮失效时，按钮背景的颜色。
              bRepaint	参数值为TRUE时，按钮马上重画；参数值为FALSE时，按钮在下次刷新时重画。
    返回值  ：无
    ====================================================================*/
   	void SetBkColor(COLORREF crBkOut, COLORREF crBkIn, COLORREF crBkDown, COLORREF crBkDisable = -1, BOOL bRepaint = TRUE);
   	static void staticSetBkColor(COLORREF crBkOut, COLORREF crBkIn, 
        COLORREF crBkDown, COLORREF crBkDisable = -1);

    /*====================================================================
    函数名  ：SetFrameColor
    功  能  ：设定按钮边框的颜色
    全局变量：无
    输入参数：crFrameOut	鼠标不在按钮上时，按钮边框的颜色。
	          crFrameIn	鼠标在按钮上时，按钮边框的颜色。
    	      crFrameDown	按钮被按下时，按钮边框的颜色。
	          crFrameDisable	按钮失效时，按钮边框的颜色。
              bRepaint	参数值为TRUE时，按钮马上重画；参数值为FALSE时，按钮在下次刷新时重画。
    返回值  ：无
    ====================================================================*/   
	void SetFrameColor(COLORREF crFrameOut, COLORREF crFrameIn, COLORREF crFrameDown, COLORREF crFrameDisable, BOOL bRepaint = TRUE);
	static void staticSetFrameColor(COLORREF crFrameOut, COLORREF crFrameIn, 
        COLORREF crFrameDown, COLORREF crFrameDisable);

    /*====================================================================
    函数名  ：SetTextColor
    功  能  ：设定按钮文本的颜色
    全局变量：无
    输入参数：crTextOut	鼠标不在按钮上时，按钮文本的颜色。
	          crTextIn	鼠标在按钮上时，按钮文本的颜色。
    	      crTextDown	按钮被按下时，按钮文本的颜色。
	          crTextDisable	按钮失效时，按钮文本的颜色。
              bRepaint	参数值为TRUE时，按钮马上重画；参数值为FALSE时，按钮在下次刷新时重画。
    返回值  ：无
    ====================================================================*/  
 	void SetTextColor(COLORREF crTextOut, COLORREF crTextIn, COLORREF crTextDown, COLORREF crTextDisable = -1, BOOL bRepaint = TRUE);
 	static void staticSetTextColor(COLORREF crTextOut, COLORREF crTextIn, 
        COLORREF crTextDown, COLORREF crTextDisable = -1);
   
    /*====================================================================
    函数名  ：SetMenuEx
    功  能  ：设定菜单按钮。
    全局变量：无
    输入参数：nMenu: 菜单的资源ID号。
              hParentWnd: 父窗口句柄，接收菜单Commad消息。
              bRepaint	参数值为TRUE时，按钮马上重画；参数值为FALSE时，按钮在下次刷新时重画。
    返回值  ：无
    ====================================================================*/
	void SetMenuEx(UINT nMenu, HWND hParentWnd, BOOL bRepaint = FALSE);

    /*====================================================================
    函数名  ：SetMenuCallBack
    功  能  ：设定菜单回调消息。
    全局变量：无
    输入参数：hWnd:     消息接收者的句柄。
              nMessage: 消息的ID号。
              lParam:	设置的参数，会随消息返回。
    返回值  ：无
    ====================================================================*/
	void SetMenuCallBack(HWND hWnd, UINT nMessage, LPARAM lParam);

    
    /*====================================================================
    函数名  ：EnableTooltipText
    功  能  ：开关Tooltip功能。
    全局变量：无
    输入参数：bActivate:是否启动Tooltip功能。
    返回值  ：无
    ====================================================================*/
	void EnableTooltipText(BOOL bActivate = TRUE);

    /*====================================================================
    函数名  ：SetTooltipText
    功  能  ：设置Tooltip文本
    全局变量：无
    输入参数：lpszText: 文本内容；
              bActivate:是否启动Tooltip功能。
    返回值  ：无
    ====================================================================*/
	void SetTooltipText(UINT nText, BOOL bActivate = TRUE);
	void SetTooltipText(LPCTSTR lpszText, BOOL bActivate = TRUE);

    
    /*====================================================================
    函数名  ：SetPressStyle
    功  能  ：设定按钮下压时，按钮的图标或文字移动的方向和距离。
    全局变量：无
    输入参数：x: 水平方向上的位移。
              y: 竖直方向上的位移。
    返回值  ：无
    ====================================================================*/
	void SetPressStyle(int x, int y);
	void SetPressStyle(CPoint ptPress);

    /*====================================================================
    函数名  ：SetRectRound
    功  能  ：设定按钮的圆角半径。
    全局变量：无
    输入参数：x: 水平方向上的圆角半径。
              y: 竖直方向上的圆角半径。
    返回值  ：无
    ====================================================================*/
	void SetRectRound(int x, int y);
	void SetRectRound(CPoint ptRound);
    
    /*====================================================================
    函数名	：    ShowFocus
    功能		：是否显示焦点框
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：bShow：是否显示的标志。
    返回值说明：  无
    ====================================================================*/
	void ShowFocus(BOOL bShow);

    /*====================================================================
    函数名	：    SetShowText
    功能		：是否在图片按钮上显示按钮标题。
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：bShow：是否显示的标志。
                  TextPos: 文字显示的位置（不要用第一个接口设为TEXT_POS_CUSTOM）。
                  ptPos:  显示文字位置的左上角，在按钮上的位置（TEXT_POS_CUSTOM）。
    返回值说明：  无
    ====================================================================*/
    void SetShowText(BOOL bShow, enTextPos TextPos = TEXT_POS_CENTER);
    void SetShowText(BOOL bShow, CPoint ptPos);

    /*函数名： SetTextPtOff
    功    能：在图片上显示标题，使用默认位置，计算时，四个方向上留出的空间。 
    参    数： CRect rcOff
    返 回 值： 
    说    明： 注意，负数表示向内缩小，正数表示向外扩张*/
    void SetTextPtOff(CRect rcOff);
    static void static_SetTextPtOff(RECT rcOff);
	
	/*====================================================================
    函数名	：    SetMaxLine
    功能		：设置文本按钮的最大显示行数。
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：nMaxLine：NO_LINEFEED		0	不换行
							AUTO_LINEFEED	<0	自动换行
							正整数				自定义最大行数
    返回值说明：  无
    ====================================================================*/
	void SetMaxLine(int nMaxLine);
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlatButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDS);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
    virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	void DrawTextAtPt(CDC *pDC, RECT rect);
	//绘制文本（可自动换行）
	void DrawTextLF( CDC* pDC,const CString& strTitle, CRect rect);
	virtual ~CFlatButton();

	//设置是否需要&的转义字符标志   lbg  2013.06.07
	void SetEscapeSequenceFlag(bool bFlag);
	bool GetEscapeSequenceFlag();
	// Generated message map functions
protected:
	void OnPopupMenu(CRect rect);
	void GetOwnRgn(CRect rect, CRgn& rgn);
	int GetCurState();
	void CancelHover();
	void DrawBitmap(CDC* pDC, RECT rect);
	void FreeResource();
	void DrawTexT(LPDRAWITEMSTRUCT lpDS);
	void InitTooltip();

    enum {STATE_OUT = 0, STATE_IN, STATE_DOWN, STATE_DISABLE, STATE_MAX};
    //四种状态的图片句柄和颜色。 
    HBITMAP  m_hBitmap[STATE_MAX];
    COLORREF m_crFrame[STATE_MAX];
    COLORREF m_crText[STATE_MAX];
    COLORREF m_crBk[STATE_MAX];
    COLORREF m_crRoundBk;
	bool     m_bIsEscapeSequence;
    //静态颜色  [zhuyr][3/29/2005]
    static COLORREF m_s_crFrame[STATE_MAX];
    static COLORREF m_s_crText[STATE_MAX];
    static COLORREF m_s_crBk[STATE_MAX];
    static RECT     m_s_rcTextPtOff;

    BOOL m_bSizeToContent;     //按钮和图片大小适应关系，TRUE时为按钮适应图片。

    COLORREF m_crFocus;  //焦点框的颜色。

    CToolTipCtrl m_ToolTip;

    //按钮状态
    BOOL m_bDisable;
    BOOL m_bDown;
    BOOL m_bFocus;
    BOOL m_bMouseIn;

    BOOL m_bNeedFocus;
	BOOL m_bNeedRTLBack;
    CPoint m_ptRectRound;   //文字按钮的园角的两个方向上的半径。
    CPoint m_ptPressStyle;  //按动按钮时图片、文字移动的距离。
    CRect  m_rcTextPtOff;

    CSize m_sizeButton;
    CSize m_sizeBitmap;

    CBitmap m_bmpMask;

    BOOL m_bNeedFree;   //需要释放图片资源。

    //显示图片按钮标题  [zhuyr][3/28/2005]
    BOOL m_bShowText;
    enTextPos m_enPosType;
    CPoint    m_ptRealTextPos;

    //菜单相关变量。
//    HMENU		m_hMenu;
    CSkinMenu m_Menu;
    HWND m_hParentWndMenu;
	BOOL m_bMenuDisplayed;
	BOOL m_bChecked;

	enum {AUTO_LINEFEED = -1, NO_LINEFEED = 0};
	//最大行数
	int m_nMaxLine;

#pragma pack(1)
	typedef struct tagCallback
	{
		HWND		hWnd;			// Handle to window
		UINT		nMessage;		// Message identifier
		WPARAM		wParam;
		LPARAM		lParam;
	} TCallback;
#pragma pack()
    TCallback m_csCallbacks;
	//{{AFX_MSG(CFlatButton)
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnCancelMode();
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnClicked();
    afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
	//}}AFX_MSG
    afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
};

/*******************************************************************
类名：CFlatDDButton
功能：平面效果按钮子类，增加接收拖拉功能。
*********************************************************************/
class CFlatDDButton : public CFlatButton  
{
public:
    
    /*====================================================================
    函数名	：    SetReceiveMsgWnd
    功能		：设置接收拖拉消息的窗口，默认为其父窗口。
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：
    返回值说明：  无
    ----------------------------------------------------------------------
    修改记录:
    日  期		版本		修改人		走读人    修改内容
    2004/07/20	1.0			朱允荣                  创建
    ====================================================================*/
    void SetReceiveMsgWnd(HWND hRevMsgWnd = NULL);

	void Register();

	CFlatDDButton();
	virtual ~CFlatDDButton();
protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlatDDButton)
    virtual void PreSubclassWindow();
    virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
    virtual BOOL OnDrop(WPARAM pDropInfo, LPARAM lParam);
	virtual DROPEFFECT OnDropEx(WPARAM pDropInfo, LPARAM lParam);
	virtual DROPEFFECT OnDropOver(WPARAM pDropInfo, LPARAM lParam);

	//{{AFX_MSG(CSkinDDListCtrl)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
    CDDTarget   *m_pcDropTarget;      //拖拉对象
    HWND        m_hRevMsgWnd;
};


/*******************************************************************
类名：CCheckButton
功能：Check, radio等的美化
*********************************************************************/
class CCheckButton : public CButton
{
// Construction
public:
	CCheckButton();

// 接口
public:
    /*====================================================================
    函数名  ：SetBitmaps
    功  能  ：设定按钮背景的颜色
    全局变量：无
    输入参数：n(h)BMPCheck:         按钮有效且选中时，按钮Check图片的资源号（句柄）。
              n(h)BMPUncheck:       按钮有效且未选中时，按钮Check图片的资源号（句柄）。
	          n(h)BMPDisableCheck:  按钮失效且选中时，按钮Check图片的资源号（句柄）。
	          n(h)BMPDisableUncheck:按钮失效且未选中时，按钮Check图片的资源号（句柄）。
	          n(h)BMPHotCheck:  按钮在鼠标下且选中时，按钮Check图片的句柄。
	          n(h)BMPHotUncheck:按钮在鼠标下且未选中时，按钮Check图片的句柄。
	          n(h)BMPDownCheck:  鼠标按下且选中时，按钮Check图片的句柄。
	          n(h)BMPDownUncheck:鼠标按下且未选中时，按钮Check图片的句柄。
    返回值  ：无
    ====================================================================*/
	void SetBitmaps(UINT nBMPCheck, UINT nBMPUncheck, 
        UINT nBMPDisableCheck = 0, UINT nBMPDisableUncheck = 0,
        UINT nBMPHotCheck = 0, UINT nBMPHotUncheck = 0,
        UINT nBMPDownCheck = 0, UINT nBMPDownUncheck = 0);
	void SetBitmaps(HBITMAP hBMPCheck, HBITMAP hBMPUncheck, 
        HBITMAP hBMPDisableCheck = NULL, HBITMAP hBMPDisableUncheck = NULL,
        HBITMAP hBMPHotCheck = NULL, HBITMAP hBMPHotUncheck = NULL,
        HBITMAP hBMPDownCheck = NULL, HBITMAP hBMPDownUncheck = NULL);

    /*====================================================================
    函数名  ：SetFrameSize
    功  能  ：设定Check框的大小
    全局变量：无
    输入参数：nSize:Check框的边长（或直径）默认为15。
    返回值  ：无
    ====================================================================*/
	void SetFrameSize(int nSize);
    
    /*====================================================================
    函数名  ：SetFrameColor
    功  能  ：设定Check框的颜色
    全局变量：无
    输入参数：crFrame	按钮有效时，Check框的颜色。
	          crDisable	按钮失效时，Check框的颜色。
              bRepaint	参数值为TRUE时，按钮马上重画；参数值为FALSE时，按钮在下次刷新时重画。
    返回值  ：无
    ====================================================================*/
	void SetFrameColor(COLORREF crFrame, COLORREF crDisable, BOOL bRepaint = FALSE);
    static void staticSetFrameColor(COLORREF crFrame, COLORREF crDisable);
    
    /*====================================================================
    函数名  ：SetTextColor
    功  能  ：设定按钮背景的颜色
    全局变量：无
    输入参数：crText	按钮有效时，按钮文字的颜色。
	          crDisable	按钮失效时，按钮文字的颜色。
              bRepaint	参数值为TRUE时，按钮马上重画；参数值为FALSE时，按钮在下次刷新时重画。
    返回值  ：无
    ====================================================================*/
	void SetTextColor(COLORREF crText, COLORREF crDisable = -1, BOOL bRepaint = FALSE);
	static void staticSetTextColor(COLORREF crText, COLORREF crDisable = -1);

    /*====================================================================
    函数名  ：SetCheckColor
    功  能  ：设定按钮选中标记的颜色（小勾或圆点）
    全局变量：无
    输入参数：crCheck	按钮有效时，选中标记的颜色。
	          crDisable	按钮失效时，选中标记的颜色。
              bRepaint	参数值为TRUE时，按钮马上重画；参数值为FALSE时，按钮在下次刷新时重画。
    返回值  ：无
    ====================================================================*/
	void SetCheckColor(COLORREF crCheck, COLORREF crDisable, BOOL bRepaint = FALSE);
	static void staticSetCheckColor(COLORREF crCheck, COLORREF crDisable);

    /*====================================================================
    函数名  ：SetBkColor
    功  能  ：设定按钮背景的颜色
    全局变量：无
    输入参数：crBk	按钮背景的颜色。
              bRepaint	参数值为TRUE时，按钮马上重画；参数值为FALSE时，按钮在下次刷新时重画。
    返回值  ：无
    ====================================================================*/
	void SetBkColor(COLORREF crBk, BOOL bRepaint = FALSE);
	static void staticSetBkColor(COLORREF crBk);

    /*====================================================================
    函数名  ：SetBk
    功  能  ：画父窗口的背景，比较安全的显示透明效果。
    全局变量：无
    输入参数：CDC* pDC	父窗口画背景的CDC指针。
    返回值  ：无
    ====================================================================*/
	void SetBk(CDC* pDC);
    
    
    /*====================================================================
    函数名  ：SetBkTrans
    功  能  ：设定按钮是否透明。
    全局变量：无
    输入参数：bTrans:TRUE时按钮透明，背景取父窗口，FALSE按钮不透明。
    返回值  ：无
    ====================================================================*/
	void SetBkTrans(BOOL bTrans = TRUE);

    /*====================================================================
    函数名  ：EnableTooltipText
    功  能  ：开关Tooltip功能。
    全局变量：无
    输入参数：bActivate:是否启动Tooltip功能。
    返回值  ：无
    ====================================================================*/
	void EnableTooltipText(BOOL bActivate = TRUE);

    /*====================================================================
    函数名  ：SetTooltipText
    功  能  ：设置Tooltip文本
    全局变量：无
    输入参数：lpszText: 文本内容；
              bActivate:是否启动Tooltip功能。
    返回值  ：无
    ====================================================================*/
	void SetTooltipText(UINT nText, BOOL bActivate = TRUE);
	void SetTooltipText(LPCTSTR lpszText, BOOL bActivate = TRUE);
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
    virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	void FreeResource();
	void SetBkParent(CWnd* pParent);
	void SizeToContent(BOOL bEnable = TRUE);
	void InitTooltip();
	virtual ~CCheckButton();

	// Generated message map functions
protected:
	CSize m_sizeButton;
	CSize m_sizeBitmap;
	void CancelHover();
	CBitmap m_bmpBk;
	CBitmap* m_pbmpOldBk;
	CDC m_dcBk;
	BOOL m_bTrans;
	void PaintBk(CDC* pDC);
	void DrawBitmap(CDC* pDC, CRect rcBit);
	void DrawCheck(CDC* pDC, CRect rect, COLORREF crCheck);
	void DrawTextCheck(CDC* pDC, CRect rcCheckFrame);
	//{{AFX_MSG(CCheckButton)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnClicked();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
    afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
	afx_msg LRESULT OnRadioInfo(WPARAM, LPARAM);
	afx_msg LRESULT OnBMSetCheck(WPARAM, LPARAM);
	afx_msg LRESULT OnBMGetCheck(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
private:
    BOOL m_bMouseIn;
    BOOL m_bDisable;
    BOOL m_bDown;
    BOOL m_bIsFocussed;
	BOOL m_bChecked;
	DWORD m_Style;

    CWnd* m_pParent;
    int m_nFrameSize;
    CToolTipCtrl m_ToolTip;

    enum {STATE_CHECK = 0, STATE_UNCHECK, 
        STATE_DISCHECK, STATE_DISUNCHECK, 
        STATE_HOTCHECK, STATE_HOTUNCHECK,
        STATE_DOWNCHECK, STATE_DOWNUNCHECK,
        STATE_MAX};
    //八种状态的图片句柄。 
    HBITMAP m_hBitmap[STATE_MAX];

    COLORREF m_crFrame[2];
    COLORREF m_crText[2];
    COLORREF m_crCheck[2];
    COLORREF m_crBk;
    
    //静态配色方案  [zhuyr][3/29/2005]
    static COLORREF m_s_crFrame[2];
    static COLORREF m_s_crText[2];
    static COLORREF m_s_crCheck[2];
    static COLORREF m_s_crBk;

    BOOL m_bSizeToContent;     //按钮和图片大小适应关系，TRUE时为按钮适应图片。

    BOOL m_bNeedFree;

    COLORREF m_crFocus;  //焦点框的颜色。
};


/*******************************************************************
类名：CSkinFrame
功能：CSkinListCtrl 的边框
*********************************************************************/
class CSkinFrame : public CStatic
{
// Construction
public:
	CSkinFrame();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinFrame)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetFrameColor(COLORREF crFrame, BOOL bRepaint = FALSE);
    static COLORREF m_s_crFrame;
	virtual ~CSkinFrame();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSkinFrame)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
protected:
	COLORREF m_crFrame;
};

/************************************************************************/
/* 列表控件                                                             */
/************************************************************************/
#define MAX_NUMBER_COLUMN  20
//每一列的比较方式，暂时只有两种（默认都是用string比较），有需求可以继续添加，然后重载回调的比较函数即可
enum{
	RESORT_BY_CSTRING = 0, RESORT_BY_NUMBER
};

class CSkinListCtrl : public CListCtrl
{
    DECLARE_DYNAMIC(CSkinListCtrl)
// Construction
public:
	CSkinListCtrl();
// 接口
public:
	 /*====================================================================
    函数名	：    SetCompareType
    功能		：设置每一列的比较方式。
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：nIndex:列的索引
                  nType:比较的方式。
    返回值说明：  无
    ====================================================================*/
	void SetCompareType(int nIndex, int nType)
	{
		m_mapCmpType.insert(make_pair(nIndex, nType));
	}
    /*====================================================================
    函数名	：    SetTitleBkColor
    功能		：设置标题栏的背景色。
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：crBk:设置的标题栏背景色
                  bRepaint:是否马上重画。
    返回值说明：  无
    ====================================================================*/
	void SetTitleBkColor(COLORREF crBk, BOOL bRepaint = TRUE);
    
    /*====================================================================
    函数名	：    SetTitleTextColor
    功能		：设置标题栏字体的颜色
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：crText:标题栏字体的颜色
                  bRepaint:是否马上重画。
    返回值说明：  无
    ====================================================================*/
	void SetTitleTextColor(COLORREF crText, BOOL bRepaint = TRUE);

    /*====================================================================
    函数名	：    SetTitleFrameColor
    功能		：设置标题栏边框颜色
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：crFrame:设置的标题栏边框色
                  bRepaint:是否马上重画。
    返回值说明：  无
    ====================================================================*/
	void SetTitleFrameColor(COLORREF crFrame, BOOL bRepaint = FALSE);
    
    /*====================================================================
    函数名	：    SetTitleDivideColor
    功能		：设置标题栏分割竖线的颜色和风格
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：crDivide:设置的标题栏分割竖线的颜色
                  npenStyel:竖线的风格：PS_SOLID:实心；PS_DOT：点线。
                  bRepaint:是否马上重画。
    返回值说明：  无
    ====================================================================*/
	void SetTitleDivideColor(COLORREF crFrame, int npenStyle = PS_SOLID, BOOL bRepaint = FALSE);

    /*====================================================================
    函数名	：    SetTitleFont
    功能		：设置标题栏的字体
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：pFont:设置的字体的指针。
    返回值说明：  无
    ====================================================================*/
	void SetTitleFont(CFont* pFont);

    /*====================================================================
    函数名	：    SetTextBkColor
    功能		：设置下面内容的背景颜色。
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：crBk:有效时的背景颜色。
                  crDisable:失效时的背景颜色。
                  bRepaint:是否马上重画。
    返回值说明：  无
    ====================================================================*/
	void SetTextBkColor(COLORREF crBk, COLORREF crDisable=-1, BOOL bRepaint = TRUE);
    
    /*====================================================================
    函数名	：    SetTextColor
    功能		：设置下面内容的文本颜色。
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：crText:有效时的文本颜色。
                  crDisable:失效时的文本颜色。
                  bRepaint:是否马上重画。
    返回值说明：  无
    ====================================================================*/
	void SetTextColor(COLORREF crText, COLORREF crDisable, BOOL bRepaint = TRUE);
    
    /*====================================================================
    函数名	：    SetTextFont
    功能		：设置内容的字体。
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：pFont:字体指针。
    返回值说明：  无
    ====================================================================*/
	void SetTextFont(CFont *pFont);

    /*====================================================================
    函数名	：    EnableHighlight
    功能		：设置光标下的行是否可以自动改变背景色。
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：bHighlight:是否启动该功能。
    返回值说明：  无
    ====================================================================*/
	void EnableHighlight(BOOL bHighlight);

    /*====================================================================
    函数名	：    SetHighlightColor
    功能		：设置高亮显示的文字及背景颜色
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：crText:字体颜色
                  crBk:背景色
                  bRepaint:是否马上重画。
    返回值说明：  无
    ====================================================================*/
	void SetHighlightColor(COLORREF crText=-1, COLORREF crBk=-1, BOOL bRepaint=TRUE);
   
    /*====================================================================
    函数名	：    SetSelectColor
    功能		：设置被选中的行的字体及背景色。
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：crText:字体颜色
                  crBk:背景色
                  bRepaint:是否马上重画。
    返回值说明：  无
    ====================================================================*/
	void SetSelectColor(COLORREF crText=-1, COLORREF crBk=-1, BOOL bRepaint=TRUE);
    
    
    /*====================================================================
    函数名	：    SetFrameColor
    功能		：设置边框的颜色
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：crFrame:边框的颜色。
                  bRepaint:是否马上重画。
                  bShow: 是否显示边框。
    返回值说明：  无
    ====================================================================*/
	void SetFrameColor(COLORREF crFrame, BOOL bRepaint = FALSE, BOOL bShow = TRUE);

    /*====================================================================
    函数名	：    EnableHeadDragDrop
    功能		：是否允许标题拖拽换位
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：bEable:是否允许。
    返回值说明：  无
    ====================================================================*/
	void EnableHeadDragDrop(BOOL bEnable = TRUE);

    /*====================================================================
    函数名	：    EnableSort
    功能		：是否启动排序功能
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：bEnable:是否排序。
    返回值说明：  无
    ====================================================================*/
    void EnableSort(BOOL bEnable = TRUE);
    
    /*====================================================================
    函数名	：    SetCompareFun
    功能		：设置比较回调函数，用于排序。
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：pfnCompare:比较函数名。该函数的声明必须为
                             static int CALLBACK FunName(LPARAM,LPARAM,LPARAM,LPARAM);
                             前两个参数为比较的数据（由SetItemData设置），
                             第三个为设置比较函数时传递的参数（即本函数的第二个参数lparam）
                             第四个为当前排序的列。
                  lparam:可以随回调函数传回的参数。
    返回值说明：  无
    ====================================================================*/
	void SetCompareFun(PFNNEWCOMPARE pfnCompare, LPARAM lparam);

    /*====================================================================
    函数名	：    SetToolTipStyle
    功能		：设置Tooltips提示方式。
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：nStyle: TTS_WHOLEROW表示提示整行内容, TTS_HIDEITME表示只提示不能完全显示的单元格
    返回值说明：  无
    ====================================================================*/
	void SetToolTipStyle(UINT nStyle);

    /*函数名： SetHasFrame
    功    能： 
    参    数： BOOL bFrame：是否有边框
    返 回 值： 
    说    明： */
    void SetShowFrame(BOOL bFrame){m_bShowFrame = bFrame;};

    /*====================================================================
    函数名	：    staticSetBkColor, staticSetTextColor
    功能		：设置静态配色方案。
    算法实现	：（可选项）
    引用全局变量：无
    参    数： COLORREF crBkNormal     ：正常状态颜色
               COLORREF crBkDisable    ：失效状态颜色
               COLORREF crBkSelect     ：选中状态颜色
               COLORREF crBkHighlight  ：高亮状态颜色
    返回值说明：  无
    ====================================================================*/
    static void staticSetBkColor(COLORREF crBkNormal, COLORREF crBkDisable, 
        COLORREF crBkSelect, COLORREF crBkHighlight);
    static void staticSetTextColor(COLORREF crTextNormal, COLORREF crTextDisable, 
        COLORREF crTextSelect, COLORREF crTextHighlight);
// Operations
public:
    void ResortItem(int nItem = -1);
     virtual int OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
    enum{CRNORMAL=0, CRDISABLE, CRSELECT, CRHIGHLIGHT, CRMAX};
    enum{TTS_DISABLE=0, TTS_WHOLEROW=1, TTS_HIDEITME};

#ifdef USESKINSCROLL
	void SetScrollBkColor(COLORREF crFrom, COLORREF crTo=-1, BOOL bRepaint = FALSE);
	void SetScrollArrowColor(COLORREF crBk, COLORREF crArrow=0, COLORREF crFrame = -1, BOOL bRepaint = FALSE);
#endif 
	void SetColumnBkColor(int nColnum,COLORREF crBk, BOOL bRepaint = TRUE);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinListCtrl)
    virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
    virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL ShowWindow(int nShowCmd);
	DWORD GetItemData(int nItem) const;
	BOOL SetItemData(int nItem, DWORD dwData);

    BOOL GetItem( LVITEM* pItem ) const;
    BOOL SetItem(const LVITEM* pItem);
    BOOL SetItem( int nItem, int nSubItem, UINT nMask, LPCTSTR lpszItem, int nImage, UINT nState, UINT nStateMask, LPARAM lParam );

	BOOL DeleteAllItems();
	BOOL DeleteItem(int nItem);
	void ResetIndex();
	int InsertItem( int nItem, LPCTSTR lpszItem, int nImage );
	int InsertItem( int nItem, LPCTSTR lpszItem );
	int InsertItem( UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState, UINT nStateMask, int nImage, LPARAM lParam );
	int InsertItem( const LVITEM* pItem );
	virtual CString GetTooltipText(int nItem, int nSubItem) const;
	void EnableRowSelect(HWND hWnd, int row, BOOL bSelected);
	BOOL IsRowSelect(HWND hWnd, int row);

    //重栽为了添加header的自画风格。
    int InsertColumn(int nCol, const LVCOLUMN* pColumn);
	int InsertColumn(int nCol, LPCTSTR lpszColumnHeading,
		int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1);
	
	virtual ~CSkinListCtrl();

	// Generated message map functions
#ifdef USESKINSCROLL
    CSkinScroll m_VScroll;
    CSkinScroll m_HScroll;
#endif
protected:
	int m_nCurCmpType;
	map<int, int>m_mapCmpType;    //每一列对应的比较类型（用CSTRING比较，还是INT，还是其他。。。。。）
	void FreeData(int nItem);
	void DrawRTLIcon(NMLVCUSTOMDRAW* pLVCD);
	BOOL m_nODItemChanged;
	BOOL m_bOldHightState;
	BOOL m_bNewFont;
	HCURSOR m_hOldCursor;
	HCURSOR m_hCursor;
	BOOL m_bHighLight;
	CRect m_crClient;
	UINT m_uToolTipStyle;
	DWORD m_wHitMask;
	CBrush m_bkBrush;
	BOOL m_bReport;
	CSkinHeaderCtrl m_Header;
    CSkinFrame m_stFrame;
	
    BOOL m_bLockChange;
    BOOL m_bSortAscending;       //排序方向
	BOOL m_bSort;                //是否排序
    PFNNEWCOMPARE m_pfn;         //自定义排序函数
    LPARAM m_pVoid;              //排序函数带的参数
	int m_nSortColumn;           //当前排序的行号

    BOOL m_bHighlight;           //是否启动高亮功能。
    int m_nHighlighted;          //高亮显示的行号

    COLORREF m_crBk[CRMAX];          //背景色0：普通，1：失效，2：选中，3：高亮
    COLORREF m_crText[CRMAX];        //文本色

	BOOL    m_bShowFrame;           //是否显示边框
    //静态配色方案  [zhuyr][3/29/2005]
    static COLORREF m_s_crBk[CRMAX];  
    static COLORREF m_s_crText[CRMAX];

    int m_nSingleColBk;
    COLORREF m_crSingleBk;
	//{{AFX_MSG(CSkinListCtrl)
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnMove(int x, int y);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPaint();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg BOOL OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnDestroy();
	//}}AFX_MSG
    afx_msg void OnCustomDrawList ( NMHDR* pNMHDR, LRESULT* pResult );

    virtual afx_msg BOOL OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult );
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()

	static int CALLBACK MyCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamData);
	//重载比较函数，通过数字比较大小
	static int CALLBACK MyComparebyNum(LPARAM lParam1, LPARAM lParam2, LPARAM lParamData);
};

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      拖拉列表类                                                      //
//////////////////////////////////////////////////////////////////////////
class CSkinDDListCtrl : public CSkinListCtrl
{
// Construction
public:
	CSkinDDListCtrl();
protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinDDListCtrl)
    virtual void PreSubclassWindow();
    virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
//	virtual CString GetTooltipText(int nItem, int nSubItem);
public:
    void SetReceiveMsgWnd(HWND hRevMsgWnd = NULL);
	virtual ~CSkinDDListCtrl();

    BOOL Register();
protected:
    CDDTarget   *m_pcDropTarget;      //拖拉对象
    HWND        m_hRevMsgWnd;
private:

	// Generated message map functions
protected:
    virtual BOOL OnDrop(WPARAM pDropInfo, LPARAM lParam);
	virtual DROPEFFECT OnDropEx(WPARAM pDropInfo, LPARAM lParam);
	virtual DROPEFFECT OnDropOver(WPARAM pDropInfo, LPARAM lParam);

	//{{AFX_MSG(CSkinDDListCtrl)
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/*******************************************************************
类名：CSkinSlider
功能：背景透明或可设背景色,设滑块图。
*********************************************************************/
class CSkinSlider : public CSliderCtrl
{
// Construction
public:
	CSkinSlider();

// 接口
public:
    /*====================================================================
    函数名	：    SetTickImage
    功能		：设置滑块图片
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：nBitmap:有效时滑块的图片
                  crTans:图片的背景色，可以屏蔽该颜色，透明显示该颜色的区域
                  nDisableBitmap:失效时的滑块图片
                  crDisableTans:失效时的图片的透明色。
    返回值说明：  无
    ====================================================================*/
    void SetTickImage(UINT nBitmap, COLORREF crTans = -1, UINT nDisableBitmap = 0, COLORREF crDisableTans = -1);
	void SetTickImage(HBITMAP hBitmap, COLORREF crTans = -1,HBITMAP hDisableBitmap = NULL, COLORREF crDisableTans = -1);
	
    /*====================================================================
    函数名	：    SetBkImage
    功能		：设置背景图
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：nBitmap:背景图
    返回值说明：  无
    ====================================================================*/
	void SetBkImage(UINT nBitmap);
	void SetBkImage(HBITMAP hBitmap);

    /*====================================================================
    函数名	：    SetBkColor
    功能		：设定控件背景色
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：crBk:背景色
                  bRepaint:是否马上重画。
    返回值说明：  无
    ====================================================================*/
	void SetBkColor(COLORREF crBk, BOOL bRepaint = TRUE);
    static void staticSetBkColor(COLORREF crBk);

    /*====================================================================
    函数名	：    SetBkTransparent
    功能		：是否背景透明
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：bTans:是否背景透明。
    返回值说明：  无
    ====================================================================*/
	void SetBkTransparent(BOOL bTans);
    

    /*=============================================================================
    函 数 名： SetTickColor
    功    能： 
    算法实现： 
    全局变量： 
    参    数： COLORREF crNormal  :正常
               COLORREF crSelect  :选中
               COLORREF crDisable :失效
               BOOL bRepaint      :是否马上重画。
    返 回 值： void 
    =============================================================================*/
	void SetTickColor(COLORREF crNormal, COLORREF crSelect, COLORREF crDisable = -1, BOOL bRepaint = FALSE);
    static void staticSetTickColor(COLORREF crNormal, COLORREF crSelect, COLORREF crDisable);

    void SetAllBkImage(BOOL bAll = TRUE){m_bAllBkImage = bAll;};
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinSlider)
	//}}AFX_VIRTUAL

// Implementation
public:
	int GetState(UINT uState);
	HBITMAP CreateImage(UINT nBitmap);
	HBITMAP CreateMask(HBITMAP hSourceBitmap, DWORD dwWidth, DWORD dwHeight, COLORREF crTransColor);
	virtual ~CSkinSlider();

	// Generated message map functions
protected:
	void DrawBMPChannel(CDC *pDC, LPNMCUSTOMDRAW lpcd, BOOL& bFocus);
	void DrawTrans(CDC* pDC, CRect rect, BOOL& bFocus);

    enum {ISTATENORMAL = 0, ISTATEHOT, ISTATESELECT, ISTATEDISABLE, ISTATEMAX};

	CBrush m_brushBk;
	BOOL m_bTransparent;
    BOOL m_bAllBkImage;
    HDC m_hDCBk;

	HBITMAP m_BitmapBk;
	HBITMAP m_BitmapTrBk;
	HBITMAP m_OldBitmapBk;
    HBITMAP m_BitmapThumb[2];
    HBITMAP m_hThumbMask[2];

    COLORREF m_crBk;
    COLORREF m_crThumb[ISTATEMAX];

    //静态配色方案  [zhuyr][3/29/2005]
    static COLORREF m_s_crBk;
    static COLORREF m_s_crThumb[ISTATEMAX];

    CRect m_rcThumb;
    BOOL m_bHot;

	void DrawThumb(CDC* pDC, LPNMCUSTOMDRAW lpcd);
	void DrawChannel(CDC* pDC, LPNMCUSTOMDRAW lpcd, BOOL& bFocus);
	//{{AFX_MSG(CSkinSlider)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnCustomdraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
    afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};


/*******************************************************************
类名：CSkinEdit
功能：设定字体颜色，背景色和边框色。
*********************************************************************/
class CSkinEdit : public CEdit
{
// Construction
public:
	CSkinEdit();
    enum {TYPE_NORMAL = 0, TYPE_NMC};
    static UINT m_s_nType;

// 接口
public:
    /*====================================================================
    函数名	：   SetBkColor
    功能		：设置编辑框的背景色。
    算法实现	：（可选项）
    引用全局变量：crBk:有效时的背景色。
                  crDisable:失效时的背景色。
                  bRepaint:是否马上重画。
    输入参数说明：无
    返回值说明：  无
    ====================================================================*/
	void SetBkColor(COLORREF crBk, COLORREF crDisable = -1, BOOL bRepaint = FALSE);
    static void staticSetBkColor(COLORREF crBk, COLORREF crDisable);

    /*====================================================================
	函数名	：   SetTextColor
	功能		：设置编辑框文字颜色。
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：crText: 文字的颜色。
                  crDisable:失效时的文字颜色。 
                  bRepaint:是否马上重画。
	返回值说明：无
    ====================================================================*/
	void SetTextColor(COLORREF crText, COLORREF crDisable = -1, BOOL bRepaint = FALSE);
    static void staticSetTextColor(COLORREF crText, COLORREF crDisable);

    /*====================================================================
	函数名	：    SetFrameColor
	功能		：设置编辑框的边框颜色。
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：crFrame:边框的颜色。
                  bRepaint:是否马上重画。
	返回值说明：  无
    ====================================================================*/
	void SetFrameColor(COLORREF crFrame, BOOL bRepaint = FALSE);
    static void staticSetFrameColor(COLORREF crFrame);
    

    /*=============================================================================
    函 数 名： UseCustomMenu
    功    能： 是否使用自定义的右键菜单
    算法实现： 
    全局变量： 
    参    数： BOOL bUseCustomMenu：开关TRUE：使用，FALSE：不使用。
    返 回 值： void 
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2004/8/26   1.0			朱允荣                  创建
    =============================================================================*/
	void UseCustomMenu(BOOL bUseCustomMenu);
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinEdit)
	protected:
    virtual void PreSubclassWindow();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSkinEdit();

	// Generated message map functions
protected:
	CBrush   m_brushBk[2];
    BOOL     m_bCustomMenu;

	COLORREF m_crBk[2];
    COLORREF m_crText[2];
    COLORREF m_crFrame;

    //配色方案  [zhuyr][3/29/2005]
    static COLORREF m_s_crBk[2];
    static COLORREF m_s_crText[2];
    static COLORREF m_s_crFrame;
	//{{AFX_MSG(CSkinEdit)
	afx_msg void OnPaint();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/*******************************************************************
类名：CSkinIPCtrl
功能：设定边框色。
*********************************************************************/
class CSkinIPCtrl : public CIPAddressCtrl
{
// Construction
public:
	CSkinIPCtrl();

    enum {TYPE_NORMAL = 0, TYPE_NMC};
    static UINT m_s_nType;
// Attributes
public:

// Operations
public:
    /*====================================================================
	函数名	：   SetTextColor
	功能		：设置控件文字颜色。
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：crText: 文字的颜色。
                  crDisable:失效时的文字颜色。
                  bRepaint:是否马上重画。
	返回值说明：无
    ====================================================================*/
	void SetTextColor(COLORREF crText, COLORREF crDisable = -1, BOOL bRepaint = FALSE);
    static void staticSetTextColor(COLORREF crText, COLORREF crDisable);

    /*====================================================================
    函数名	：   SetBkColor
    功能		：设置控件的背景色。
    算法实现	：（可选项）
    引用全局变量：crBk:有效时的背景色。
                  crDisable:失效时的背景色。
                  bRepaint:是否马上重画。
    输入参数说明：无
    返回值说明：  无
    ====================================================================*/
	void SetBkColor(COLORREF crBk, COLORREF crDisable, BOOL bRepaint = FALSE);
    static void staticSetBkColor(COLORREF crBk, COLORREF crDisable);

    /*====================================================================
	函数名	：    SetFrameColor
	功能		：设置控件的边框颜色。
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：crFrame:边框的颜色。
                  bRepaint:是否马上重画。
	返回值说明：  无
    ====================================================================*/
	void SetFrameColor(COLORREF crFrame, BOOL bRepaint=FALSE);
	static void staticSetFrameColor(COLORREF crFrame);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinIPCtrl)
	public:
	protected:
    virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSkinIPCtrl();

	// Generated message map functions
protected:
	CBrush m_brushBk[2];

	COLORREF m_crBk[2];
    COLORREF m_crText[2];
    COLORREF m_crFrame;

    //配色方案  [zhuyr][3/29/2005]
    static COLORREF m_s_crBk[2];
    static COLORREF m_s_crText[2];
    static COLORREF m_s_crFrame;
	//{{AFX_MSG(CSkinIPCtrl)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnNcPaint();
    afx_msg void OnPaint();
	afx_msg void OnEnable(BOOL bEnable);
    afx_msg void OnMove(int x, int y);
    afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/*******************************************************************
类名：CStaticEx
功能：设定字体颜色，背景透明或可设背景色。
*********************************************************************/
class CStaticEx : public CStatic
{
// Construction
public:
	CStaticEx();

// Attributes
public:

// 接口
public:
    /*====================================================================
    函数名  ：SetTextColor
    功  能  ：设置文字颜色。
    全局变量：无
    输入参数：crText: 正常时的文字色；
              crDisable:失效时的文字色；
              bRepaint: 是否马上重画。
    返回值  ：无
    ====================================================================*/
	void SetTextColor(COLORREF crText, COLORREF crDisable = -1, BOOL bRepaint = TRUE);
    static void staticSetTextColor(COLORREF crText, COLORREF crDisable);
    
    /*====================================================================
    函数名  ：SetBkTransparent
    功  能  ：设置背景设置是否透明。
    全局变量：无
    输入参数：bTrans: 是否透明。
    返回值  ：无
    ====================================================================*/
	void SetBkTransparent(BOOL bTrans = TRUE);

    /*====================================================================
    函数名  ：SetBkColor
    功  能  ：设置背景颜色，当背景设置为透明时，该函数无效。
    全局变量：无
    输入参数：crBk: 正常时的背景色；
              crDisable:失效时的背景色；
              bRepaint: 是否马上重画。
    返回值  ：无
    ====================================================================*/
	void SetBkColor(COLORREF crBk, COLORREF crDisable = -1, BOOL bRepaint = TRUE);
    static void staticSetBkColor(COLORREF crBk, COLORREF crDisable);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticEx)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
    virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetSingleLine(BOOL bSingleLine = FALSE);
	virtual ~CStaticEx();

	// Generated message map functions
protected:
	void EnableTooltip(BOOL bActive = TRUE);
	void InitToolTip();
	void DrawMultiLineText(CDC *pDC, CRect rect, int nRowDis, UINT nFromat, CString strText);
	CToolTipCtrl m_toolTip;
	BOOL m_bShowToolTips;
    BOOL m_bTrans;
	BOOL m_bSingleLine;

	COLORREF m_crBk[2];
	COLORREF m_crText[2];

    //配色方案  [zhuyr][3/29/2005]
    static COLORREF m_s_crBk[2];
    static COLORREF m_s_crText[2];

    //{{AFX_MSG(CStaticEx)
	afx_msg void OnPaint();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/*******************************************************************
类名：CSkinComboBox
功能：重画ComboBox，可以设定各个部分的颜色。
*********************************************************************/
class CSkinComboBox : public CComboBox
{
// Construction
public:
	CSkinComboBox();
	virtual ~CSkinComboBox();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinComboBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDS);
    virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// 接口
public:
    /*====================================================================
    函数名  ：SetButtonImage
    功  能  ：设置控件下拉按钮图片。
    全局变量：无
    输入参数：nBitmap: 正常时按钮图片资源ID号；
              nDisableBitmap: 失效时的按钮图片资源ID号。(或句柄)
    返回值  ：无
    ====================================================================*/
	void SetButtonImage(UINT nBitmap, UINT nDisableBitmap);
	void SetButtonImage(HBITMAP hBitmap, HBITMAP hDisableBitmap);

    /*====================================================================
    函数名  ：SetButtonColor
    功  能  ：设置控件下拉按钮的颜色，设置了按钮图片时，该函数无效。
    全局变量：无
    输入参数：crFront: 按钮前景三角形的颜色；
              crBk: 按钮的背景色；
              crDisableFt: 失效时的按钮前景色；
              crDisableBk: 失效时的按钮背景色。
    返回值  ：无
    ====================================================================*/
	void SetButtonColor(COLORREF crFront, COLORREF crBk, COLORREF  crDisableFt, COLORREF crDisableBk);
	static void staticSetButtonColor(COLORREF crFront, COLORREF crBk,
        COLORREF  crDisableFt, COLORREF crDisableBk);

    /*====================================================================
    函数名  ：SetFrameColor
    功  能  ：设置控件边框颜色。
    全局变量：无
    输入参数：crFrame: 边框颜色；
              bRepaint: 是否马上重画。
    返回值  ：无
    ====================================================================*/
	void SetFrameColor(COLORREF crFrame, BOOL bRepaint = FALSE);
    static void staticSetFrameColor(COLORREF crFrame);

    /*====================================================================
    函数名  ：SetBkColor
    功  能  ：设置文本背景颜色。
    全局变量：无
    输入参数：crBk: 正常时的文本背景色；
              crHighlight:高亮显示时的文本背景色；
              bRepaint: 是否马上重画。
    返回值  ：无
    ====================================================================*/
	void SetBkColor(COLORREF crBk, COLORREF crHighlight, BOOL bRepaint = FALSE);
    static void staticSetBkColor(COLORREF crBk, COLORREF crHighlight, COLORREF crDisable);

    /*====================================================================
    函数名  ：SetTextColor
    功  能  ：设置文本颜色。
    全局变量：无
    输入参数：crText: 正常时的文本色；
              crHighlight:高亮显示时的背景色；
              bRepaint: 是否马上重画。
    返回值  ：无
    ====================================================================*/
	void SetTextColor(COLORREF crText, COLORREF crHighlight, BOOL bRepaint = FALSE);
    static void staticSetTextColor(COLORREF crText, COLORREF crHighlight);

	//重载该函数以适应win8下下拉框字体变大的问题
	int AddString(LPCTSTR lpszString);

	// Generated message map functions
protected:
	void DrawImageButton(CDC* pDC, CRect rect);

	BOOL m_bDrawList;
	BOOL m_bEnable;
    BOOL m_bNeedFree;
	BOOL m_bDrop;
	BOOL m_bNeedRefresh;

	CSize m_sizeButton;
    HBITMAP m_hBitmap[2];
    CBrush  m_brushBk[2];

	COLORREF m_crFrame;
	COLORREF m_crText[2];
	COLORREF m_crBk[3];
    COLORREF m_crButton[4];

    static COLORREF m_s_crFrame;
    static COLORREF m_s_crText[2];
    static COLORREF m_s_crBk[3];
    static COLORREF m_s_crButton[4];
	//{{AFX_MSG(CSkinComboBox)
	afx_msg void OnPaint();
	afx_msg BOOL OnDropdown();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/*******************************************************************
类名：CBMPDlg
功能：实现设定背景图
*********************************************************************/
class CBMPDlg : public CDialog
{
// 构造函数和接口
public:
    /*====================================================================
    函数名  ：SizeToContent
    功  能  ：改变对话框大小，适应图片。
    ====================================================================*/
	void SizeToContent();
    
    /*====================================================================
    函数名  ：SetBkColor
    功  能  ：设置对话框的背景色，可以设为渐进色。
    全局变量：无
    输入参数：crTop:对话框最上端的颜色。
              crBottom:对话框最下端的颜色。
    返回值  ：无
    ====================================================================*/
	void SetBkColor(COLORREF crTop, COLORREF crBottom = -1, BOOL bRepaint = FALSE);

    /*====================================================================
    函数名  ：SetBkImage
    功  能  ：设置对话框背景图。
    全局变量：无
    输入参数：hBitmap: 背景图的句柄。
              nBitmap:背景图的资源ID。
    返回值  ：无
    ====================================================================*/
	void SetBkImage(HBITMAP hBitmap);
	void SetBkImage(UINT nBitmap);
//接口最后


	CBMPDlg(CWnd* pParent = NULL);   // standard constructor
	CBMPDlg(UINT uResourceID, CWnd* pParent = NULL);
	CBMPDlg(LPCTSTR pszResourceID, CWnd* pParent = NULL);
    ~CBMPDlg();

// Dialog Data
	//{{AFX_DATA(CBMPDlg)
//	enum { IDD = _UNKNOWN_RESOURCE_ID_ };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBMPDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
public:
	static void SetIsRTLStyle(BOOL bRTL);
protected: 
	static BOOL s_bIsRTLStyle;
// Implementation
protected:
	void DrawBk(CDC* pDC);
	void Init();
	void FreeResources();
    CBitmap m_Bitmap;
    HBITMAP m_OldBitmap;
    CDC m_BkDC;
	HBITMAP m_hBitmap;
    DWORD m_dwWidth;
    DWORD m_dwHeight;
    BOOL m_bSizeToContent;

    COLORREF m_crTop;
    COLORREF m_crBottom;

	// Generated message map functions
	//{{AFX_MSG(CBMPDlg)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};






/*******************************************************************
类名：CSkinSpinCtrl
功能：实现设定背景色和前景色
*********************************************************************/
class CSkinSpinCtrl : public CSpinButtonCtrl
{
// Construction
public:
	CSkinSpinCtrl();

// Attributes
public:
    enum {Outside=0, OnRightInside, OnLeftInside};
    enum {NoHot = 0, FirstHot, LastHot};
    enum {STNormal = 0, STPressed, STHilight, STDisable, STAll};
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinSpinCtrl)
	public:
	protected:
	virtual void PreSubclassWindow();
    virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
    
    /*====================================================================
    函数名	：   SetForeColor 
    功能		：设定按钮的前景色（三角）
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：COLORREF crFore:正常状态下的颜色
                  COLORREF crPressed：下压时的颜色
                  COLORREF crHilight：在鼠标下时的颜色
                  BOOL bRepaint：是否马上重画
    返回值说明：  无
    ====================================================================*/
	void SetForeColor(COLORREF crFore, COLORREF crPressed, COLORREF crHilight, BOOL bRepaint = FALSE);
    static void staticSetForeColor(COLORREF crFore, COLORREF crPressed,
        COLORREF crHilight, COLORREF crDisable);
    
    /*====================================================================
    函数名	：   SetBkColor 
    功能		：设定按钮的背景色
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：COLORREF crBk:正常状态下的颜色
                  COLORREF crPressed：下压时的颜色
                  COLORREF crHilight：在鼠标下时的颜色
                  BOOL bRepaint：是否马上重画
    返回值说明：  无
    ====================================================================*/
	void SetBkColor(COLORREF crBk, COLORREF crPressed, COLORREF crHilight = -1, BOOL bRepaint = FALSE);
    static void staticSetBkColor(COLORREF crBk, COLORREF crPressed,
        COLORREF crHilight, COLORREF crDisable);

    /*函数名： SetDisableColor
    功    能： 设置失效时的颜色。
    参    数： COLORREF crFore
               COLORREF crBk
    返 回 值： 
    说    明： */
    void SetDisableColor(COLORREF crFore, COLORREF crBk);
	
    virtual ~CSkinSpinCtrl();
protected:
    CRect m_rcFirst;
    CRect m_rcLast;
    CRect m_rcPressed;
    UINT m_nSpinAlign;
	BOOL m_bVertical;
    BOOL m_bPressed;
    
    int  m_nMax;
    int  m_nMin;
    int  m_nPos;

	COLORREF m_crFore[STAll];
    COLORREF m_crBk[STAll];

    //配色方案  [zhuyr][3/29/2005]
    static COLORREF m_s_crFore[STAll];
    static COLORREF m_s_crBk[STAll];

	// Generated message map functions
protected:
	int m_nHotPos;
	void CreateArrowRgn(CRgn &rgnFirst, CRgn &rgnLast);
	//{{AFX_MSG(CSkinSpinCtrl)
	afx_msg void OnPaint();
	afx_msg BOOL OnDeltapos(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};


class CSkinGroup : public CButton
{
// Construction
public:
	CSkinGroup();
    enum {COLTYPE_TEXT = 1, COLTYPE_LINE = 2, COLTYPE_ALL = 3};

// 接口
public:
    /*====================================================================
    函数名	：    SetLMetric
    功能		：设置文字到左边框的距离
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：nMetric：距离
    返回值说明：  无
    ----------------------------------------------------------------------
    修改记录:
    日  期		版本		修改人		走读人    修改内容
    2004/07/30	1.0			朱允荣                  创建
    ====================================================================*/
	void SetLMetric(UINT nMetric = 7);

    /*====================================================================
    函数名	：    SetGroupColor
    功能		：设置控件颜色
    算法实现	：（可选项）
    引用全局变量：无
    输入参数说明：crNormal: 正常时显示的颜色, -1时保持原来的颜色。
                  crDisable:失效时显示的颜色, -1时保持原来的颜色。
                  bRepaint:是否马上重画控件。
    返回值说明：  无
    ----------------------------------------------------------------------
    修改记录:
    日  期		版本		修改人		走读人    修改内容
    2004/07/30	1.0			朱允荣                  创建
    ====================================================================*/
	void SetGroupColor(COLORREF crNormal, COLORREF crDisable=-1,
        BOOL bRepaint = FALSE, int nType = COLTYPE_ALL);
    static void staticSetGroupColor(COLORREF crNormal, COLORREF crDisable,
        int nType = COLTYPE_ALL);

    /*=============================================================================
    函 数 名:SetWindowText
    功    能:设置标题文字
    参    数:LPCTSTR lpszString         [in]    标题文字
    注    意:专门用于处理重影问题
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/12/26  4.0     王昊    创建
    =============================================================================*/
    void SetWindowText(LPCTSTR lpszString);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinGroup)
	public:
	protected:
        virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSkinGroup();

	// Generated message map functions
protected:
    COLORREF m_crText[2];
    COLORREF m_crLine[2];
    CPen     m_penLine[2];
    UINT     m_nLMetric;
    CString  m_strTitle;

    static COLORREF m_s_crText[2];
    static COLORREF m_s_crLine[2];
	//{{AFX_MSG(CSkinGroup)
	afx_msg void OnPaint();
	afx_msg void OnEnable(BOOL bEnable);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

class CTextButton30 : public CButton
{
public:
    CTextButton30();
    virtual ~CTextButton30();

    /*====================================================================
    函数名  ：EnableTooltipText
    功  能  ：开关Tooltip功能。
    全局变量：无
    输入参数：bActivate:是否启动Tooltip功能。
    返回值  ：无
    ====================================================================*/
	void EnableTooltipText(BOOL bActivate = TRUE);

	void InitTooltip();

    /*====================================================================
    函数名  ：SetTooltipText
    功  能  ：设置Tooltip文本
    全局变量：无
    输入参数：lpszText: 文本内容；
              bActivate:是否启动Tooltip功能。
    返回值  ：无
    ====================================================================*/
	void SetTooltipText(LPCTSTR lpszText, BOOL bActivate = TRUE);

public:
    //按钮颜色，0正常，1失效， over，click反白。
	COLORREF m_crBk[2];
	COLORREF m_crFrame[2];
    COLORREF m_crText;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextButton30)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDS);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
    virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

protected:

    BOOL m_bMouseIn;
    BOOL m_bDown;
    BOOL m_bDisable;
    BOOL m_bFocus;

    CToolTipCtrl m_ToolTip;

    //{{AFX_MSG(CTextButton30)
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnCancelMode();
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
    LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
    void CancelHover();
};



class CSkinTab : public CTabCtrl
{
// Construction
public:
	CSkinTab();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinTab)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:

    /*=============================================================================
    函 数 名： SetColors
    功    能： 
    算法实现： 
    全局变量： 
    参    数： COLORREF crFrame  :边框颜色
               COLORREF crHBk    :高亮页背景色
               COLORREF crLBk    :普通页背景色
               COLORREF crText   :字体颜色
    返 回 值： void 
    说明：     使用图片时才会用crText值来绘制字体，否则使用框架crFrame的颜色
    =============================================================================*/
	void SetColors(COLORREF crFrame, COLORREF crHBk, COLORREF crLBk, COLORREF crText = RGB(80, 114, 170));
    void SetBKColor(COLORREF crBk);
    static void staticSetColors(COLORREF crFrame, COLORREF crHBk, COLORREF crLBk, COLORREF crText = RGB(80, 114, 170));
    static void staticSetBKColor(COLORREF crBk);

	void SetType(UINT nType);
	void SetMask(UINT nHLeft, UINT nHRight, UINT nLLeft, UINT nLRight);

    void SetSigleLine(BOOL bSigleLine){m_bSigleLine = bSigleLine;};
    
	void DrawSimpleBorder(CDC* pDC, CRect& rect, BOOL bSel);
	void DrawImageBorder(CDC* pDC, CRect rect, BOOL bSel);
	virtual ~CSkinTab();

    enum {TYPE_ROUNTRECT = 0, TYPE_POLYGON, TYPE_TRAPEZIA, TYPE_BITMAP};

	// Generated message map functions
protected:
	void GetRgn(CRect& rect, CRgn& rgn, BOOL bSel, int& nWidth);
	void DrawItem(LPDRAWITEMSTRUCT lpdis);
	//{{AFX_MSG(CSkinTab)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

    COLORREF m_crLBk;
    COLORREF m_crHBk;
    COLORREF m_crFrame;
    COLORREF m_crTableBK;
    COLORREF m_crText;  // 使用图片时才会用该值来绘制字体，否则使用框架的颜色

    BOOL     m_bSigleLine;
    UINT     m_nType;
    UINT     m_nWidth;
    UINT     m_nImageWidth[4];

    CRect    m_rcClient;

    HBITMAP  m_hBitmap[4];
    CSkinSpinCtrl  m_Spin;

    static COLORREF m_s_crLBk;
    static COLORREF m_s_crHBk;
    static COLORREF m_s_crFrame;
    static COLORREF m_s_crTableBK;
    static COLORREF m_s_crText; // 使用图片时才会用该值来绘制字体，否则使用框架的颜色
};


class CSkinSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CSkinSheet)
// Construction
public:
	CSkinSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CSkinSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
    static COLORREF m_s_crBk;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinSheet)
	public:
	virtual BOOL OnInitDialog();
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
    void SetCreate(BOOL bCreate){m_bCreate = bCreate;};
	void MovePage(CPropertyPage* pPage);
	void FreshButton();
	CSkinTab* GetSkinTab();
	virtual ~CSkinSheet();
    enum{HMARGIN = 6, VMARGIN = 3};

	// Generated message map functions
	CTextButton30        m_btnCancel;
protected:
	void Init();
	//{{AFX_MSG(CSkinSheet)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
    CSkinTab *m_pTab;
	CTextButton30        m_btnFinish;
	CTextButton30        m_btnOK;
	CTextButton30        m_btnApply;
    CTextButton30        m_btnHelp;
	CTextButton30        m_btnBack;
	CTextButton30        m_btnNext;

    COLORREF             m_crBk;
    LONG                 m_nCapHeight;
    BOOL                 m_bWizard;
    CRect                m_rcCaption;

    BOOL                 m_bCreate;
};


class CSkinPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CSkinPage)

// Construction
public:
	void SetTitle( LPCTSTR strTitle );
    void SetColorBk(COLORREF crBk){m_crBk = crBk;};
	CSkinPage(UINT nIDTemplate = 0);
	~CSkinPage();

// Dialog Data
	//{{AFX_DATA(CSkinPage)
//	enum { IDD = _UNKNOWN_RESOURCE_ID_ };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
    
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSkinPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:
    COLORREF m_crBk;
    CString  m_strTitle;
protected:
	// Generated message map functions
	//{{AFX_MSG(CSkinPage)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


class CSkinCheckList : public CCheckListBox
{
// Construction
public:
	CSkinCheckList();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinCheckList)
	public:
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL OnChildNotify(UINT, WPARAM, LPARAM, LRESULT*);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
    //接口
	void SetTextColor(COLORREF crNormal, COLORREF crDisable, COLORREF crHilight);
	void SetBkColor(COLORREF crNormal, COLORREF crDisable, COLORREF crHilight);
	static void staticSetTextColor(COLORREF crNormal, COLORREF crDisable, COLORREF crHilight);
	static void staticSetBkColor(COLORREF crNormal, COLORREF crDisable, COLORREF crHilight);

    void SetFrameColor(COLORREF crFrame){m_stFrame.SetFrameColor(crFrame);};
    
    void SetCheckBMP(UINT nCheck);
	virtual ~CSkinCheckList();
	// Generated message map functions
protected:
	//{{AFX_MSG(CSkinCheckList)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnPaint();
    afx_msg LRESULT OnLBGetItemData(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnLBSetItemCheck(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
    // 预准备自绘
	void PreDrawItemSelf(LPDRAWITEMSTRUCT lpDIS);
    void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	DECLARE_MESSAGE_MAP()

protected:
    COLORREF m_crBk[3];
    COLORREF m_crText[3];
    CSkinFrame  m_stFrame;
    CSkinScroll m_VScroll;

    static COLORREF m_s_crBk[3];
    static COLORREF m_s_crText[3];
};


//////////////////////////////////////////////////////////////////////////
//CColourPopup 颜色选择控件使用的弹出选择窗口
//////////////////////////////////////////////////////////////////////////

// CColourPopup messages
#define CPN_SELCHANGE        WM_USER + 1001        // Colour Picker Selection change
#define CPN_DROPDOWN         WM_USER + 1002        // Colour Picker drop down
#define CPN_CLOSEUP          WM_USER + 1003        // Colour Picker close up
#define CPN_SELENDOK         WM_USER + 1004        // Colour Picker end OK
#define CPN_SELENDCANCEL     WM_USER + 1005        // Colour Picker end (cancelled)
#define CSM_GETCOLOR         WM_USER + 1010        // Colour Select get colour
#define CSM_SETCOLOR         WM_USER + 1011        // Colour Select set colour

// forward declaration
class CColourPicker;

// To hold the colours and their names
typedef struct {
    COLORREF crColour;
    TCHAR    *szName;
    TCHAR    *szEncname;
} ColourTableEntry;

/////////////////////////////////////////////////////////////////////////////
// CColourPopup window

class CColourPopup : public CWnd
{
// Construction
public:
    CColourPopup();
    CColourPopup(CPoint p, COLORREF crColour, COLORREF crDefault, CWnd* pParentWnd,
                 LPCTSTR szDefaultText = NULL, LPCTSTR szCustomText = NULL);
    void Initialise();

// Attributes
public:
    static BOOL m_s_bUserChinese;

// Operations
public:
    BOOL Create(CPoint p, COLORREF crColour, CWnd* pParentWnd, 
                LPCTSTR szDefaultText = NULL, LPCTSTR szCustomText = NULL);

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CColourPopup)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
    virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CColourPopup();

protected:
    BOOL GetCellRect(int nIndex, const LPRECT& rect);
    void FindCellFromColour(COLORREF crColour);
    void SetWindowSize();
    void CreateToolTips();
    void ChangeSelection(int nIndex);
    void EndSelection(int nMessage);
    void DrawCell(CDC* pDC, int nIndex);

    COLORREF GetColour(int nIndex)              { return m_crColours[nIndex].crColour; }
    LPCTSTR GetColourName(int nIndex)           { return m_s_bUserChinese ?
        m_crColours[nIndex].szName : m_crColours[nIndex].szEncname; }
    int  GetIndex(int row, int col) const;
    int  GetRow(int nIndex) const;
    int  GetColumn(int nIndex) const;

// protected attributes
protected:
    static ColourTableEntry m_crColours[];
    int            m_nNumColours;
    int            m_nNumColumns, m_nNumRows;
    int            m_nBoxSize, m_nMargin;
    int            m_nCurrentSel;
    int            m_nChosenColourSel;
    CString        m_strDefaultText;
    CString        m_strCustomText;
    CRect          m_CustomTextRect, m_DefaultTextRect, m_WindowRect;
    CFont          m_Font;
    CPalette       m_Palette;
    COLORREF       m_crInitialColour, m_crColour, m_crDefault;
    CToolTipCtrl   m_ToolTip;
    CWnd*          m_pParent;

    BOOL           m_bChildWindowVisible;

    //custom draw  [zhuyr][10/9/2005]
    COLORREF       m_crFrame;
    COLORREF       m_crBackground;
    COLORREF       m_crFont;
    COLORREF       m_crHilightBk;
    COLORREF       m_crCustomBk;

    // Generated message map functions
protected:
    //{{AFX_MSG(CColourPopup)
    afx_msg void OnNcDestroy();
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnPaint();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg BOOL OnQueryNewPalette();
    afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
#if _MSC_VER > 1200
	afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
#else
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
#endif
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
//Office风格颜色选择控件
//////////////////////////////////////////////////////////////////////////

void AFXAPI DDX_ColourPicker(CDataExchange *pDX, int nIDC, COLORREF& crColour);
class CColorSelCtrl : public CButton
{
// Construction
public:
	CColorSelCtrl();

// Attributes
public:

// Operations
public:
    COLORREF GetColor() const;
    void     SetColor(COLORREF coColor);

    COLORREF GetDefaultColor() const;
    void     SetDefaultColor(COLORREF coColor);

    void     SetCustomText(LPCTSTR lpText);
    void     SetDefaultText(LPCTSTR lpText);

    void     SetTrackSelection(BOOL bTrack);
    BOOL     GetTrackSelection(void) const;

    void     SetTextMode(BOOL bUseDefault);
    void     SetBtnImage(UINT nNoraml, UINT nHilight,
        UINT nDown, UINT nDisable);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorSelCtrl)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorSelCtrl();
	__declspec(property(get=GetColor,put=SetColor))						COLORREF	Color;
	__declspec(property(get=GetDefaultColor,put=SetDefaultColor))		COLORREF	DefaultColor;
	__declspec(property(get=GetTrackSelection,put=SetTrackSelection))	BOOL		TrackSelection;
	__declspec(property(put=SetCustomText))								LPCTSTR		CustomText;
	__declspec(property(put=SetDefaultText))							LPCTSTR		DefaultText;
	// Generated message map functions
protected:
    static void DrawArrow(CDC* pDC, 
						  RECT* pRect, 
						  int iDirection = 0,
						  COLORREF clrArrow = RGB(0,0,0));
    static HBITMAP CreateImage(UINT nBitmap);
    void CancelHover();
	//{{AFX_MSG(CColorSelCtrl)
	afx_msg void OnClicked();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg LONG OnSelEndOK(UINT lParam, LONG wParam);
    afx_msg LONG OnSelEndCancel(UINT lParam, LONG wParam);
    afx_msg LONG OnSelChange(UINT lParam, LONG wParam);
    LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
    enum {STATE_NORMAL = 0, STATE_HILIGHT, STATE_DOWN, STATE_DISABLE, STATE_MAX};
    COLORREF m_crColor;
	COLORREF m_crDefault;
	CString  m_strDefaultText;
	CString  m_strCustomText;
	BOOL	 m_bPopupActive;
	BOOL	 m_bTrackSelection;
    BOOL     m_bDefautText;
    BOOL     m_bMouseIn;
    HBITMAP  m_hBitmap[STATE_MAX];

    COLORREF m_crFrame;
    COLORREF m_crBackgroud;
};

class CSkinDateTime : public CDateTimeCtrl
{
// Construction
public:
	CSkinDateTime();

// Attributes
public:
    void SetFrameColor(COLORREF crFrame);
    void SetBackColor(COLORREF crBK, COLORREF crDisable);
    void SetTextColor(COLORREF crText, COLORREF crDisable);

    static void static_SetFrameColor(COLORREF crFrame);
    static void static_SetBackColor(COLORREF crBK, COLORREF crDisable);
    static void static_SetTextColor(COLORREF crText, COLORREF crDisable);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinDateTime)
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSkinDateTime();

protected:  
    CSkinSpinCtrl m_Spin;

	CBrush   m_brushBk[2];

	COLORREF m_crBk[2];
    COLORREF m_crText[2];
    COLORREF m_crFrame;
    
    //配色方案  [zhuyr][3/29/2005]
    static COLORREF m_s_crBk[2];
    static COLORREF m_s_crText[2];
    static COLORREF m_s_crFrame;
	// Generated message map functions
protected:
	//{{AFX_MSG(CSkinDateTime)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/*=============================================================================
类    名: CCalcLen
功    能: 计算ucs2转成utf8需要的char长度
使    用: 请继承本类

-------------------------------------------------------------------------------
修改纪录:
日      期	版本	修改人	修改内容
2013/01/30	5.0	    沈  钦	创建
=============================================================================*/
//////////////////////////////////////////////////////////////////////////
class CCalcLen
{
public:
	CCalcLen(int limitlen) : m_nLimitLen(limitlen)
	{
	}
	CCalcLen()
	{
		m_nLimitLen = -1;
	}
	/***
	*设置编辑框字符串最大长度
	* @param limitlen [in]  长度
	***/
	virtual void SetLimitLen(int limitlen)
	{
		m_nLimitLen = limitlen;
	}

	/******
	* 根据限制的最大长度获取字符串
	* @param strSrcText [in]  要截取的字符串
	* @param nSrcPos [in]  截取前光标位置
	* @param strDstText [out] 截取后的字符串
	* @param nDstPos [out] 截取后光标位置
	* @return 是否超过限制
	******/
	BOOL LimitString(CString strSrcText, int nSrcPos, CString &strDstText, int &nDstPos);

	/***
	*设置字符串的编码方式
	* @param dwCodePage [in]  字符编码
	**/
	static void SetCodePage( int dwCodePage ){ s_dwCodePage = dwCodePage; }

private:

	/******
	* 截取字符串
	* @param strSrcText [in]  要截取的字符串
	* @param nSrcPos [in]  截取前光标位置
	* @param strDstText [out] 截取后的字符串
	* @param nDstPos [out] 截取后光标位置
	******/
	int LimitStr(CString strSrcText, int nSrcPos, CString &strDstText, int &nDstPos);
	/******
	* 从字符串尾部开始截取字符串
	* @param strSrcText [in]  要截取的字符串
	* @param nSrcPos [in]  截取前光标位置
	* @param strDstText [out] 截取后的字符串
	* @param nDstPos [out] 截取后光标位置
	******/
	int LimitStrFromTail(CString strSrcText, int nSrcPos, CString &strDstText, int &nDstPos);
	/******
	* 从字符串中间部分开始截取字符串
	* @param strSrcText [in]  要截取的字符串
	* @param nSrcPos [in]  截取前光标位置
	* @param strDstText [out] 截取后的字符串
	* @param nDstPos [out] 截取后光标位置
	******/
	int LimitStrFromMid(CString strSrcText, int nSrcPos, CString &strDstText, int &nDstPos);
	/***
	*得到字符串在给定编码格式下的长度
	* @param strSrcText [in]  字符串
	* @return 字符串在给定编码格式下长度
	**/
	int GetLen(CString strText);

protected:
	int m_nLimitLen; // 限制的字符串长度
	static int s_dwCodePage;
};

// 自定义消息
#define   WM_PASTE_LIMIT			WM_USER + 2000

class CLimitEdit : public CSkinEdit, public CCalcLen
{
// Construction
public:
	CLimitEdit();
// Attributes
public:
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLimitEdit)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLimitEdit();
	void LimitText(int nChars = 0)	{
		CCalcLen::SetLimitLen(nChars);
	}
	void SetLimitText(UINT nMax){
		CCalcLen::SetLimitLen(nMax);
	}
	// 设置Windows窗口content
	void SetWindowText(LPCTSTR lpszString);
	// 限制窗口显示内容
	void LimitTextContent();
	// Generated message map functions
protected:
	//{{AFX_MSG(CLimitEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//afx_msg void OnPaste(void);
	//}}AFX_MSG
	afx_msg LRESULT OnPasteLimit(WPARAM wp, LPARAM lp);
	DECLARE_MESSAGE_MAP()

	CString m_strCurText;
	BOOL  m_bIsMaxLen;

private:
	enum emKeyMessage	{
		emCtrlV = 1, // 快捷键Ctrl + V
			emPaste, // 粘贴
			emOther // 其他
	};
	
	BOOL m_bFlag; // 标识项，标识当前项是否为快捷键	
	emKeyMessage  m_emKey; // 状态标识

	BOOL m_bUseToolTip;
	CToolTipCtrl m_cToolTip;
};


class CFilterEdit :public CLimitEdit
{
public:
	CFilterEdit();
	// 要过滤的字符串数组，/0结尾
	void SetFilterChars( LPCTSTR lpszFilterChar );
protected:
//{{AFX_MSG(CFilterCharEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	CString m_strFilterChars;
};

#endif
