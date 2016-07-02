/*****************************************************************************
   模块名      : drawbmp
   文件名      : drawbmp.h
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

#include "kdvtype.h"
#include "osp.h"

#ifdef WIN32
    #pragma  pack (1)
#endif

#define IN     // 函数参数输入
#define OUT    // 函数参数输出

#ifndef min
#define min(a,b) ((a)>(b)?(b):(a))
#endif
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

// 默认文本长度
#define DEF_TEXT_LEN                (u8)128 
// 最大BMP文件大小
#define MAX_BMP_FILESIZE            (u32)800*600  
// 最大BMP数据大小
#define MAX_BMP_DATASIZE            (u32)720*576  

#ifdef WIN32
#define DEF_FONT_PATHNAME	        (s8*)"../res/font/kdvuni.ttf"
#else
#define DEF_FONT_PATHNAME	        (s8*)"/usr/bin/font/kdvuni.ttf"
#endif

//编码方式
#define ENCODE_UTF8	                (u8)1
#define ENCODE_GBK	                (u8)2

//bmp 绘画模式
#define BMP_8                       (u8)1
#define BMP_24                      (u8)2 

// 字体对齐方式
// 垂直方向
#define VERTICAL_TOP                (u16)0x0001  // 垂直方向靠上
#define VERTICAL_CENTER             (u16)0x0002  // 垂直方向居中
#define VERTICAL_BOTTOM             (u16)0x0004  // 垂直方向靠下
// 水平方向
#define LEVEL_RIGHT                 (u16)0x0010  // 水平方向右对齐
#define LEVEL_CENTER                (u16)0x0020  // 水平方向居中
#define LEVEL_LEFT                  (u16)0x0040  // 水平方向左对齐
// 字体排列方式
#define ARRANGE_LEFT_RIGHT          (u16)0x0100  // 从左至右排列
#define ARRANGE_RIGHT_LEFT          (u16)0x0200  // 从右至左排列

// 生成图片大小决定方式
#define AUTO_SIZE_MODE              (u8)0x01        // 生成自适应大小的图片、即根据文本生成合适大小的图片
#define USER_DEFINED_SIZE_MODE      (u8)0x02        // 生成用户自定义大小的图片

// 字体
#define FONT_HEI                    (u8)01 // 黑体
#define FONT_SONG                   (u8)02 // 宋体 (默认)
#define FONT_KAI                    (u8)03 // 楷体

// 字号
#define FONT_SIZE16                 (u8)16	        // 24
#define FONT_SIZE24                 (u8)24	        // 24
#define FONT_SIZE32                 (u8)32	        // 32
#define FONT_SIZE48                 (u8)48	        // 48
#define FONT_SIZE64                 (u8)64	        // 64

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
    /*   默认参数：BMP_24  */  
    TPicParam() : m_byPicType( BMP_24 ),
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

protected:
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

// 最小文本图片参数
struct TMinTextPicParam
{
public:
    TMinTextPicParam()
    { 
        SetMinTextPicParamNull();
    }
    void SetMinTextPicParamNull(void) { memset(this, 0, sizeof(TMinTextPicParam)); }

    u32 GetMinTextPicTop( void ) { return m_dwMinTextPicTop; }
    u32 GetMinTextPicLeft( void )  { return m_dwMinTextPicLeft;  }
    u32 GetMinTextPicHeight( void ) { return m_dwMinTextPicHeight; }
    u32 GetMinTextPicWidth( void )  { return m_dwMinTextPicWidth;  }
    
    void SetMinTextPicTop( u32 dwMinTextPicTop ) { m_dwMinTextPicTop = dwMinTextPicTop; }
    void SetMinTextPicLeft( u32 dwMinTextPicLeft )   { m_dwMinTextPicLeft = dwMinTextPicLeft; }
    void SetMinTextPicHeight( u32 dwMinTextPicHeight ) { m_dwMinTextPicHeight = dwMinTextPicHeight; }
    void SetMinTextPicWidth( u32 dwMinTextPicWidth )   { m_dwMinTextPicWidth = dwMinTextPicWidth; }
    
protected:
    u32 m_dwMinTextPicTop;          // 图片top位置
    u32 m_dwMinTextPicLeft;         // 图片left位置
    u32 m_dwMinTextPicHeight;      // 图片高度
    u32 m_dwMinTextPicWidth;       // 图片宽度
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// 图片
struct TPic : public TPicParam,
              public TMinTextPicParam
{
public:
    TPic() 
    { 
        SetPicNull();
    }
    void SetPicNull(void) { memset(this, 0, sizeof(TPic)); }

    const u8* GetPic( void ) { return m_abyPic; }
    void SetPic( const u8* lpszPic, u32 dwPicSize )
    {
        memset( m_abyPic, 0, sizeof(m_abyPic) );
        SetPicSize( 0 );
        if( NULL == lpszPic || dwPicSize >= sizeof(m_abyPic) )
        {            
            return;
        }

        memcpy( m_abyPic, lpszPic, dwPicSize );
        SetPicSize( dwPicSize );
        m_abyPic[ sizeof( m_abyPic ) - 1 ] = '\0';
    }
protected:
    u8 m_abyPic[MAX_BMP_FILESIZE+1];    // 图片BUF
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// 文本参数
struct TTextParam
{
public: 
    TTextParam() : m_byFontType( FONT_KAI ), // 目前不支持设置
                   m_byFontSize( FONT_SIZE32 ), // 建议使用常规字号
                   m_wTextDistribution( ARRANGE_LEFT_RIGHT | VERTICAL_CENTER | LEVEL_LEFT ),
                   m_dwTextColor( COLOR_WHITE ),
                   m_byTextEncode( ENCODE_UTF8 )   // 默认为UTF8编码
    { 
        memset( m_achText, 0, sizeof(m_achText) );
    }
    void SetTextParamNull(void) { memset(this, 0, sizeof(TTextParam)); }

    u8  GetFontType( void)       { return m_byFontType; }
    u8  GetFontSize( void )      { return m_byFontSize; }
    u16 GetTextDistribution( void ) { return m_wTextDistribution; }
    u32 GetTextColor( void )     { return m_dwTextColor; }
    const s8 * GetText( void )       { return m_achText; }
    
    void SetFontType( u8 byFontType )            { m_byFontType = byFontType; }
    void SetFontSize( u8 byFontSize )            { m_byFontSize = byFontSize; }
    void SetTextDistribution( u16 wTextDistribution ) { m_wTextDistribution = wTextDistribution; }
    void SetTextColor( u32 dwTextColor )         { m_dwTextColor = dwTextColor; }
    void SetText( const s8 * lpszText)
    {
        if( NULL == lpszText )
        {
            return;
        }
        memset( m_achText, 0, sizeof(m_achText) );
        memcpy( m_achText, lpszText, min(sizeof(m_achText), strlen(lpszText)) );
        m_achText[ sizeof( m_achText ) - 1 ] = '\0';
	}

    void SetTextEncode(u8 byTextEncode) { m_byTextEncode = byTextEncode; }
    u8   GetTextEncode() { return m_byTextEncode; }

protected:
    u8  m_byFontType;       // 字体类型
    u8  m_byFontSize;       // 字体大小
    u16 m_wTextDistribution; // 文本分布方式，调用方式: ARRANGE_LEFT_RIGHT | VerticalAlignment | LevelAlignment（从左到右/从右到左排列 位或 垂直对齐方式 位或 水平对齐方式）
    u32 m_dwTextColor;      // 文本颜色
	s8  m_achText[DEF_TEXT_LEN+1]; // 文本内容
    u8 m_byTextEncode;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// 制图参数
/*
通过变量m_byDrawMode判断制图模式：

采用AUTO_SIZE_MODE模式，需设置变量包括：
（TextParam ）m_byFontType,m_byFontSize,m_wTextDistribution,m_dwTextColor,m_achText,m_byTextEncode
（TPicParam ）m_byPicType
（TDrawParam）m_dwDrawBkColor

采用USER_DEFINED_SIZE_MODE模式，需设置变量包括：
（TextParam ）m_byFontType,m_byFontSize,m_wTextDistribution,m_dwTextColor,m_achText,m_byTextEncode
（TPicParam ）m_byPicType,m_wPicHeight（区别部分）,m_wPicWidth（区别部分）
（TDrawParam）m_dwDrawBkColor
*/
struct TDrawParam : public TPicParam, public TTextParam
{
public:
    /*   默认参数：AUTO_SIZE_MODE模式、白色背景  */ 
    TDrawParam() : m_byDrawMode( AUTO_SIZE_MODE ),
                   m_dwDrawBkColor( COLOR_BLACK )
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

class CDrawBmp
{	
public :
	CDrawBmp();
	virtual ~CDrawBmp();

    //目前，启用某个字库  只支持一次启用一个字库
    BOOL32 LoadFont(const s8 * pFontPathName = DEF_FONT_PATHNAME);
    // 制图  success: 1, fail: 0
	BOOL32 DrawBmp( OUT TPic& tPic, IN TDrawParam& tDrawParam );

private:
	CKdvGDI *m_pDrawGdi;
};

API void drawbmphelp();
API void drawbmpver();
API void setdrawbmplog(u8 byLogLvl);

#endif // _DRAW_INTERFACE_H

