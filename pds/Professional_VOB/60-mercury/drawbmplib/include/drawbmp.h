/*****************************************************************************
   模块名      : KDV draw bmp
   文件名      : drawbmp.h
   相关文件    : 
   文件实现功能: 简易画图子类头文件
   作者        : 
   版本        : V0.9  Copyright(C) 2001-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2006/03/29  4.0         john     创建
******************************************************************************/
#ifndef _KDVDRAW_DECLARE_H_
#define _KDVDRAW_DECLARE_H_ 

#include "drawbmpconst.h"
#include "drawinterface.h"
#include "kdvsys.h"
#include "kdvlog.h"
#include "loguserdef.h"

class CKdvGDI
{	
public :
	CKdvGDI();
	virtual ~CKdvGDI();

public:
	
	BOOL32 InitGDI( void );// 初始化绘图系统
	BOOL32 QuitGDI( void );// 退出绘图系统

// 新接口 主要操作

    // 判断是否支持当前画图类型
    BOOL32 IsSupportedPicType( IN u8 byPicType );
    // 根据文本内容获取最小的文本框属性
    void   GetMinTextBoxAttrByText(OUT u32& dwHeight, OUT u32& dwWidth, OUT u8& byLineNum, IN LPCSTR lpszText );
    // 创建画布
    BOOL32 CreatePaint( IN TDrawParam& tDrawParam );
    // 判断图片大小是否超过容许范围
    BOOL32 IsVaildPicSize();
    // 画布区域着色（背景色）
    BOOL32 PaintBkColor( IN u32 dwBkColor );
    // 输出文本到画布
    BOOL32 TextOutInPaint( IN LPCSTR lpszText,IN u8 byTextLen, IN u8 byTextAlignment );
    // 获取生成的PIC数据
    BOOL32 GetPic( OUT TPic& tPic );
    // 制图
	BOOL32 DrawBmp( OUT TPic& tPic, IN TDrawParam& tDrawParam );

// 属性操作
	const TKDVFONT* GetCurrentFont( void ) const { return &m_tKdvFont; }// 获取当前字体属性

	u32 SetBkColor( u32 dwBkColor );// 设置当前画布背景色	
	u32 GetBkColor( void ) const { return m_dwBkColor; }// 获取当前画布背景色
	
	u32 SetTextColor( u32 dwTextColor );// 设置当前文本颜色
	u32 GetTextColor( void ) const { return m_dwTextColor; }// 获取当前文本颜色

	TKDVFONT SetCurrentFont( TKDVFONT &tKdvFont );// 设置当前字体属性
    
	TKDVBRUSH SetCurrentBrush( TKDVBRUSH &tKdvBrush );// 设置当前画刷属性		
	const TKDVBRUSH* GetCurrentBrush( void ) const { return &m_tKdvBrush; }// 获取当前画刷属性

	TKDVPEN SetCurrentPen( TKDVPEN &tKdvPen );// 设置当前画笔属性
	const TKDVPEN* GetCurrentPen( void )  const { return &m_tKdvPen; }// 获取当前画笔属性
    
	TKDVPAINT SetCurrentPaint(TKDVPAINT tPaint); // 设置当前画布
	const TKDVPAINT* GetPaintInfo( void ) { return &m_tKdvPaint; }// 获取画布属性

	BOOL32 IsCreate( void ) { return m_bCreatePaint; }	  // 画布是否创建

// 基本图形绘制
	u32 SetPixel( s32 nPosX, s32 nPosY, u32 dwPixelColor );// 画点
	u32 SetPixel( TPOINT tPoint, u32 dwPixelColor );
	
	BOOL32 LineTo( s32 nPosX, s32 nPosY );// 画线
	BOOL32 LineTo( TPOINT tPoint );
    BOOL32 LevelLine( IN s32 nPosX, IN s32 nPosY, IN u32 dwWidth );// 画水平直线	
	BOOL32 VertLine( IN s32 nPosX, IN s32 nPosY, IN u32 dwHeight );// 画垂直直线
		
	TPOINT MoveTo( s32 nPosX, s32 nPosY );// 改变当前位置
	TPOINT MoveTo( TPOINT tPoint );

	BOOL32 DrawRect( s32 nPosX, s32 nPosY, u32 dwWidth, u32 dwHeight );// 画矩形
	BOOL32 DrawRect( TRECT tRect );

	BOOL32 FillRect( s32 nPosX, s32 nPosY, u32 dwWidth, u32 dwHeight ); // 填充指定区域
	BOOL32 FillRect( TRECT tRect );
	
    // 画布
	BOOL32 CreatePaint( u16 wOrignX, u16 wOrignY, u32 dwWidth, u32 dwHeight, u8 byBmpFileStyle = BMP_8 );
	BOOL32 CreatePaint( TKDVPAINT& tKdvPaint, u8 byBmpFileStyle = BMP_8 );
    BOOL32 DeletePaint( void );	

	u32    TextOutInRect( const char* pchText, u32 dwTextLen, TRECT tRect );// 文本输出
	u32    TextOut( IN s32 nPosX, IN s32 nPosY, const char* pchText, u32 dwTextLen ); // 文本输出 
		
	void   PalSortByShell( void );// 对调色板排序
	
	u8     GetColorIndex( IN u32 clrColor );// 根据RGB获取调色板索引

	BOOL32 GetFontPath( OUT s8 *pachFontPath, IN u16 wLen  );// 取字库路径
    BOOL32 LoadFontLib( const char* pchFontPath );// 载入字库资源
	BOOL32 GetFontLibIndex( u8 &byLibIndex ); //  整个字库文件由多个字库组成，取相应的字库序号
	void   ReleaseFontLib( void );// 释放字库资源
    
    // 图形资源
    void   CreateBitMapRes( void );
	void   ReleaseBitMapRes( void );

private :

	BOOL32 m_bInitGDI;	// 整个图形系统初始化标志

	u8	m_byFontLibNum; // 字库数目
	TKDVFONTLIB m_tKdvFontLib[ MAX_FONTLIB_NUM ];// 字库资源

	TPALETTEENTRY m_tPaletteEntry[ PAINTCOLOR ];// 调色板	
	u8  m_abyPaletteIndex[ PAINTCOLOR ];// 顺序调色板索引表

	u32 m_dwBkColor;        // 画布背景色
	u32 m_dwTextColor;      // 当前文本颜色
	
	TKDVFONT   m_tKdvFont;	// 当前字体属性
	TPOINT     m_tKdvPoint;	// 当前起点坐标
	TKDVPEN    m_tKdvPen;	// 当前画笔属性
	TKDVBRUSH  m_tKdvBrush;	// 当前画刷属性
	TKDVPAINT  m_tKdvPaint;	// 当前画布属性

	// 画布创建标志
	BOOL32 m_bCreatePaint;

	// 图形分解为文件头和数据区
	// 图形文件数据区(只含图形数据)
	u8 *m_pbyBmpDataBuff;
	// 整个图形文件数据区(含图形文件头和数据区)
    u8 *m_pbyBmpFileBuff;

// 新增变量
    // 图片类型
    u8 m_byPicType;
};

void paintlog(u8 byLogLvl, s8 * fmt, ...);

#endif  // _KDVDRAW_DECLARE_H_