/*****************************************************************************
   模块名      : draw bmp
   文件名      : drawinterface.h
   相关文件    : 
   文件实现功能: 简易画图基类头文件
   作者        : 
   版本        : V0.9  Copyright(C) 2001-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2006/03/29  4.0         john     创建
******************************************************************************/
#ifndef _DRAW_INTERFACE_H
#define _DRAW_INTERFACE_H

#ifndef  _EXCLUDE_VCCOMMON_H  // 不包Vccommon.h
#define  _EXCLUDE_VCCOMMON_H
#endif

#include "kdvtype.h"
#include "mcuconst.h"
#include "osp.h"

#ifdef WIN32
    #pragma  pack (1)
#endif

#define IN     // 函数参数输入
#define OUT    // 函数参数输出

// 画图返回值
#define  SUCCESS_DRAW   (u8)1    // success
#define  FAIL_DRAW      (u8)0    // fail

//bmp 绘画模式
#define  BMP_8   (u8)1
#define  BMP_24  (u8)2

// 字体对齐方式
// 垂直方向VerticalAlignment
#define VERTICAL_TOP    (u8)0x01  // 垂直方向靠上
#define VERTICAL_CENTER (u8)0x02  // 垂直方向居中
#define VERTICAL_BOTTOM (u8)0x04  // 垂直方向靠下
// 水平方向LevelAlignment
#define LEVEL_RIGHT     (u8)0x10  // 水平方向右对齐
#define LEVEL_CENTER    (u8)0x20  // 水平方向居中
#define LEVEL_LEFT      (u8)0x40  // 水平方向左对齐

// 生成图片大小决定方式
#define AUTO_SIZE_MODE          (u8)0x01        // 生成自适应大小的图片、即根据文本生成合适大小的图片
#define USER_DEFINED_SIZE_MODE  (u8)0x02        // 生成用户自定义大小的图片

// RGB
#undef RGB
#define RGB(r,g,b) ((u32)(((u8)(r)|((u16)((u8)(g))<<8))|(((u32)(u8)(b))<<16)))
// 颜色
const u32 COLOR_BLACK = RGB( 0, 0, 0 );
const u32 COLOR_WHITE = RGB( 255, 255, 255 );
const u32 COLOR_GRAY = RGB( 196, 206, 212 );
const u32 COLOR_MENUHEADER = RGB( 54,40,111 );
const u32 COLOR_MENUBODY = RGB( 124,114,124 )/*RGB( 120,120,120 )*/;
const u32 COLOR_MENUFOOT = RGB(90,90,90 );
const u32 COLOR_ITEMFONT = RGB( 42,31,85 );
const u32 COLOR_ITEMHL = RGB( 85,95,170 );
const u32 COLOR_YELLOW = RGB( 252, 226, 84 );
const u32 COLOR_BLUE = RGB( 44, 94, 180 );
const u32 COLOR_GREEN = RGB( 132, 218, 132 );
const u32 COLOR_CURSOR = RGB( 100, 202, 68 );
const u32 COLOR_RED = RGB( 255, 0, 0 );

// 图片参数
struct TPicParam
{
public:
    /*   默认参数：BMP_8（索引颜色图）  */  
    TPicParam() : m_byPicType( BMP_8 ),
                  m_dwPicHeight(0),
                  m_dwPicWidth(0),
                  m_dwPicSize(0)
    { 
    
    }
    void SetPicParamNull(void) { memset(this, 0, sizeof(TPicParam)); }

    u8  GetPicType( void ) { return m_byPicType; }
    u32 GetPicHeight( void ) { return m_dwPicHeight; }
    u32 GetPicWidth( void )  { return m_dwPicWidth;  }
    u32 GetPicSize( void )   { return m_dwPicSize;  }

    void SetPicType( u8 byPicType )     { m_byPicType = byPicType; }
    void SetPicHeight( u32 dwPicHeight ) { m_dwPicHeight = dwPicHeight; }
    void SetPicWidth( u32 dwPicWidth )   { m_dwPicWidth = dwPicWidth; }
    void SetPicSize( u32 dwPicSize )    { m_dwPicSize = dwPicSize; }
    
protected:
    u8  m_byPicType;        // 图片类型
    u32 m_dwPicHeight;      // 图片高度
    u32 m_dwPicWidth;       // 图片宽度
    u32 m_dwPicSize;        // 图片大小
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// 图片
struct TPic : public TPicParam
{
public:
    TPic() 
    { 
        memset( m_abyPic, 0, sizeof(m_abyPic) );
    }
    void SetPicNull(void) { memset(this, 0, sizeof(TPic)); }

    LPCSTR GetPic( void ) { return m_abyPic; }
    void SetPic( LPCSTR lpszPic )
    { 
        memcpy( m_abyPic, lpszPic, sizeof(m_abyPic) );
        m_abyPic[ sizeof( m_abyPic ) - 1 ] = '\0';
    }
protected:
    s8 m_abyPic[MAX_BMPFILE_SIZE+1];    // 图片BUF
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// 文本参数
struct TTextParam
{
public:
    /*   默认参数：楷体、32号、垂直居中、黑色字体  */    
    TTextParam() : m_byFontType( FONT_KAI ),
                   m_byFontSize( FONT_SIZE_32 ),
                   m_wTextAlignment( VERTICAL_CENTER | LEVEL_CENTER ),
                   m_dwTextColor( COLOR_BLACK )
    { 
        memset( m_achText, 0, sizeof(m_achText) );
    }
    void SetTextParamNull(void) { memset(this, 0, sizeof(TTextParam)); }

    u8  GetFontType( void)       { return m_byFontType; }
    u8  GetFontSize( void )      { return m_byFontSize; }
    u8  GetTextAlignment( void ) { return m_wTextAlignment; }
    u32 GetTextColor( void )     { return m_dwTextColor; }
    LPCSTR GetText( void )       { return m_achText; }
    
    void SetFontType( u8 byFontType )            { m_byFontType = byFontType; }
    void SetFontSize( u8 byFontSize )            { m_byFontSize = byFontSize; }
    void SetTextAlignment( u8 byTextlAlignment ) { m_wTextAlignment = byTextlAlignment; }
    void SetTextColor( u32 dwTextColor )         { m_dwTextColor = dwTextColor; }
    void SetText( LPCSTR lpszText)
    {
        memcpy( m_achText, lpszText, sizeof(m_achText) );
        m_achText[ sizeof( m_achText ) - 1 ] = '\0';
	}

protected:
    u8  m_byFontType;       // 字体类型
    u8  m_byFontSize;       // 字体大小
    u8  m_wTextAlignment;   // 文本对齐方式，调用方式: VerticalAlignment | LevelAlignment（垂直对齐方式 位或 水平对齐方式）
    u32 m_dwTextColor;      // 文本颜色
	s8  m_achText[DEFAULT_TEXT_LEN+1]; // 文本内容
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// 制图参数
/*
通过变量m_byDrawMode判断制图模式：

采用AUTO_SIZE_MODE模式，需设置变量包括：
（TextParam ）m_byFontType,m_byFontSize,m_wTextAlignment,m_dwTextColor,m_achText[DEFAULT_TEXT_LEN+1]T
（TPicParam ）m_byPicType
（TDrawParam）m_dwDrawBkColor

采用USER_DEFINED_SIZE_MODE模式，需设置变量包括：
（TextParam ）m_byFontType,m_byFontSize,m_wTextAlignment,m_dwTextColor,m_achText[DEFAULT_TEXT_LEN+1]
（TPicParam ）m_byPicType,m_wPicHeight（区别部分）,m_wPicWidth（区别部分）
（TDrawParam）m_dwDrawBkColor
*/
struct TDrawParam : public TPicParam, public TTextParam
{
public:
    /*   默认参数：AUTO_SIZE_MODE模式、白色背景  */ 
    TDrawParam() : m_byDrawMode( AUTO_SIZE_MODE ),
                   m_dwDrawBkColor( COLOR_WHITE )
    {

    }
    void SetDrawParamNull(void) { memset(this, 0, sizeof(TDrawParam)); }

    u8  GetDrawMode( void ) { return m_byDrawMode; }
    u32 GetDrawBkColor( void ) { return m_dwDrawBkColor; }    
    
    void SetDrawMode( u8 byDrawMode )     { m_byDrawMode = byDrawMode; }
    void SetDrawBkColor( u32 dwDrawBkColor ) { m_dwDrawBkColor = dwDrawBkColor; }

protected:
    u8  m_byDrawMode;           // 制图模式: AUTO_SIZE_MODE 或者 USER_DEFINED_SIZE_MODE
    u32 m_dwDrawBkColor;        // 制图背景色
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

class CKdvGDI;

class CDrawInterface
{	
public :
	CDrawInterface();
	virtual ~CDrawInterface();

    // 制图
	BOOL32 DrawBmp( OUT TPic& tPic, IN TDrawParam& tDrawParam );

private:
	CKdvGDI *m_pDrawGdi;
};

#endif // _DRAW_INTERFACE_H

