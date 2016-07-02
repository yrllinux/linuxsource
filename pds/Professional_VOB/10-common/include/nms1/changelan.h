/*****************************************************************************
  模块名      : changelan.lib
  文件名      : changelan.h
  相关文件    : changelandef.h
  文件实现功能: 翻译窗口文字、或指定文字
  作者        : 朱允荣
  版本        : V1.0  Copyright(C) 1997-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人                 修改内容
2005/09/09  1.0         朱允荣                 创建
******************************************************************************/
#ifndef _CHANGELAN_050909_H_
#define _CHANGELAN_050909_H_


class CTranslate;
class CChangeLan
{
public:
    CChangeLan();
    ~CChangeLan();
    /*函数名：    Create
    功    能： 
    参    数： LPCTSTR lpszFileName:保存文本信息的文件名
               u32 nMaxNode = 1000:所有的文本条数，用于分配内存。
    返 回 值：  
    说    明： */
    u16     Create(LPCTSTR lpszFileName, u32 nMaxNode = 1000);

    u16     Destroy();

    /*函数名：    SetCurLanguage
    功    能： 翻译并设置当前的语言类型
    参    数： LPCTSTR lpszLangName：新的语言类型。
               HWND hTopWnd = NULL：根据新的语言，重新设定窗口及其子窗口文字
    返 回 值：  
    说    明： */
    u16     SetCurLanguage(LPCTSTR lpszLangName, HWND hTopWnd = NULL);


    /*函数名： TranslateText
    功    能： 翻译指定文字到指定语言类型
    参    数： LPCTSTR lpszOld：待翻译的文字的语言类型
               LPCTSTR lpszNew：翻译目标语言类型
               LPCTSTR lpszText：待翻译的文字
    返 回 值： 翻译后的文字，失败返回NULL
    说    明： */
    LPCTSTR TranslateText(LPCTSTR lpszOld, LPCTSTR lpszNew, LPCTSTR lpszText);
    /*函数名： TranslateText
    功    能： 翻译指定文字到当前语言类型
    参    数： LPCTSTR lpszLangType：待翻译的文字的语言类型
               LPCTSTR lpszText：待翻译的文字
    返 回 值： 翻译后的文字，失败返回NULL
    说    明： */
	LPCTSTR TranslateText(LPCTSTR lpszLangType, LPCTSTR lpszText);

    /*函数名： TranslateFromENG
    功    能： 翻译指定英文文字到当前语言类型
    参    数： LPCTSTR lpszText：待翻译的文字
    返 回 值： 翻译后的文字，失败返回NULL
    说    明： */
    LPCTSTR TranslateFromENG(LPCTSTR lpszText);
    /*函数名： TranslateFromCHN
    功    能： 翻译指定中文文字到当前语言类型
    参    数： LPCTSTR lpszText：待翻译的文字
    返 回 值： 翻译后的文字，失败返回NULL
    说    明： */
    LPCTSTR TranslateFromCHN(LPCTSTR lpszText);

    /*函数名： TurnWindowText
    功    能： 翻译指定窗口及其子窗口文字到指定语言，
    参    数： LPCTSTR lpszOld：翻译前，窗口使用的语言。
               LPCTSTR lpszNew：翻译后，窗口使用的语言。
               HWND hTopWnd：   窗口句柄
    返 回 值： 错误码
    说    明： */
    u16 TurnWindowText(LPCTSTR lpszOld, LPCTSTR lpszNew, HWND hTopWnd);
    /*函数名： TurnWindowText
    功    能： 翻译窗口及其子窗口文字到当前语言，
    参    数： LPCTSTR lpszOld：窗口当前使用的语言
               HWND hTopWnd：   窗口句柄
    返 回 值： 错误码
    说    明： */
	u16 TurnWindowText(LPCTSTR lpszOld, HWND hTopWnd);

    /*函数名： TurnMenuText
    功    能： 翻译菜单中的文字。
    参    数： LPCTSTR lpszOld：翻译前，菜单使用的语言。
               LPCTSTR lpszNew：翻译后，菜单使用的语言。
               HMENU hMenu：    菜单句柄。
    返 回 值：错误码 
    说    明： */
    u16 TurnMenuText(LPCTSTR lpszOld, LPCTSTR lpszNew, HMENU hMenu);

    /*函数名： TurnMenuText
    功    能： 翻译菜单中的文字到当前的语言
    参    数： LPCTSTR lpszOld：翻译前，菜单使用的语言。
               HMENU hMenu
    返 回 值： 错误码
    说    明： */
    u16 TurnMenuText(LPCTSTR lpszOld, HMENU hMenu);
    
    /*函数名： SetNotifyMessage
    功    能： 
    参    数： u32 dwMesssageID：通知消息ID默认为WM_USER+11
               u8 bySendMode：SENDMSG_MODE_ALL全部窗口；
                              SENDMSG_MODE_IGNORE忽略窗口（默认）。
    返 回 值： 
    说    明： */
    u16 SetNotifyMessage(u32 dwMesssageID, u8 bySendMode);

    /*函数名： GetCurrentLan
    功    能： 
    参    数： 
    返 回 值：当前语言 
    说    明： */
    LPCTSTR GetCurrentLan() const;

	static u16 SetIgnoreWnd(HWND hWnd, BOOL32 bIgnore = TRUE);
protected:
    CTranslate*     m_pTran;
};
#endif