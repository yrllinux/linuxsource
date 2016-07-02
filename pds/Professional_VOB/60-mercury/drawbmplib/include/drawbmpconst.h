/*****************************************************************************
   模块名      : KDV Paint
   文件名      : KDVPaint.h
   相关文件    : 
   文件实现功能: 简易画图类常量定义文件
   作者        : 
   版本        : V0.9  Copyright(C) 2001-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2006/03/29  4.0         john          创建
******************************************************************************/
#ifndef  _KDVPAINTCONST_H_
#define  _KDVPAINTCONST_H_

#include "kdvtype.h"

#ifdef WIN32
    #pragma  pack (1)
#endif

#define INIT_VALUE     (s32)(-1)
#define INITIALIZED( x )  if( !(x) ) { return FALSE; }

#ifdef WIN32
    #define FONT_PATH	        (s8*)"./res/font"
#else
    #define FONT_PATH	        (s8*)"/usr/bin/font"
#endif
#define  DEFAULT_FONTLIB_NAME   (s8*)"fontlib"  // 默认字库名

// log等级定义
#define	 LOG_LVL_ERROR			(u8)1		    // 程序运行错误(逻辑或业务出错),必须输出
#define	 LOG_LVL_WARNING		(u8)2			// 告警信息, 可能正确, 也可能错误
#define	 LOG_LVL_KEYSTATUS		(u8)3			// 程序运行到一个关键状态时的信息输出
#define	 LOG_LVL_DETAIL			(u8)4			// 普通信息, 最好不要写进log文件中

#define  RGB_COLOR_MAKEUP       (u8)3           // RGB 颜色由3个值组成
#define  MAX_FONTLIB_NUM        (u8)10			// 最大载入字体资源个数

#define  MAX_BYTE_FILL          (u8)255	        // 一次最大填充字节数
#define  INVALID_COLOR          (u32)0xFFFFFFFF // 无效颜色值

// 字库类型
#define  FT_HZK16               (u8)0x01    // 16*16点阵
#define  FT_HZK24               (u8)0x02	// 24*24点阵
#define  FT_HZK32               (u8)0x03	// 32*32点阵

#define  PAINTCOLOR             (u16)256    // 调色板

// 颜色值
const u8 PALETTE[] = 
{
	36,26,36,4,142,84,172,150,76,4,106,20,132,218,100,116,94,36,132,150,172,
	4,86,188,84,158,124,228,210,140,148,178,156,92,86,196,76,150,172,60,34,28,
	108,114,148,68,182,148,52,94,140,52,86,108,132,166,212,220,230,236,132,206,
	244,4,122,4,180,178,180,116,118,196,140,226,252,12,154,228,4,122,172,196,206,
	212,68,42,108,4,62,164,12,90,76,108,186,244,124,166,148,156,158,172,132,150,
	204,148,182,212,164,126,28,68,122,188,28,74,172,108,106,116,132,130,132,252,
	230,140,116,130,188,220,218,204,164,158,204,220,178,44,132,102,132,180,202,212,
	4,138,68,180,242,188,52,66,156,4,78,180,4,114,76,108,206,196,44,178,20,44,86,
	188,60,86,172,196,190,204,52,158,244,4,134,180,28,90,108,100,146,196,228,230,
	228,148,226,140,148,146,140,76,190,36,84,74,44,132,218,220,4,134,28,20,70,148,
	172,182,212,252,246,204,172,162,132,4,110,12,92,86,108,4,106,156,220,206,172,
	164,178,188,100,98,156,92,174,236,140,126,140,68,190,252,52,58,116,4,70,164,
	172,170,156,148,142,196,132,134,172,220,222,228,188,226,236,100,206,252,68,
	102,196,108,162,228,100,202,68,28,162,116,68,146,220,52,46,44,124,114,124,
	108,126,172,36,174,252,212,210,196,4,98,84,140,198,236,108,114,116,140,138,
	140,156,166,196,196,198,220,4,138,244,164,238,172,156,154,140,4,102,204,76,
	194,252,116,206,252,36,38,76,20,150,84,212,170,36,20,102,44,156,182,196,148,
	166,188,148,202,236,4,122,52,188,186,212,156,230,252,20,162,252,12,114,212,
	204,202,212,60,42,132,12,62,164,124,186,220,76,138,220,28,166,108,220,190,
	100,76,62,108,44,182,252,196,250,252,228,218,156,116,102,188,4,122,124,172,
	242,252,108,106,196,84,86,84,76,46,20,156,214,236,92,182,244,252,250,244,
	188,174,124,92,74,108,236,138,4,60,182,52,20,134,212,28,162,204,116,210,
	84,92,198,52,76,66,76,212,198,132,44,38,52,76,158,236,116,150,220,212,174,
	76,244,226,164,148,150,188,124,122,156,108,90,68,156,194,220,172,170,212,36,
	98,92,68,90,132,124,166,220,124,118,108,4,138,212,92,198,252,124,138,204,228,
	226,220,92,106,140,4,122,196,132,218,132,20,78,180,132,218,252,124,126,172,
	60,182,252,156,226,156,164,166,172,132,134,196,172,210,220,228,238,236,100,
	94,196,124,122,140,164,238,252,76,146,196,180,238,212,148,146,164,4,138,52,
	4,102,108,188,182,196,172,174,172,44,94,180,52,54,52,116,110,196,4,130,4,4,
	106,180,116,214,252,148,166,212,12,106,36,156,178,172,76,102,188,12,146,92,
	140,154,180,140,210,236,124,114,188,116,182,236,164,162,164,156,178,220,140,
	138,180,228,226,236,84,134,212,156,154,196,124,170,164,132,154,220,52,54,132,
	108,210,252,180,246,252,100,86,108,204,198,212,36,74,172,220,218,220,60,86,
	188,116,114,116,140,138,156,164,166,196,236,238,236,12,94,84,148,150,148,172,
	186,204,4,114,20,100,206,60,4,106,212,60,46,124,12,66,156,252,254,252,156,230,
	164,196,190,212,28,90,116,4,134,36,4,70,172,68,102,204,140,138,148,204,202,
	220,124,166,228,164,166,180,100,86,116,
	0,0,0,255,255,255,54,40,111,255,0,0,90,90,90,42,31,85,85,95,170,
};

// 背景色
typedef struct tagBackBGDColor
{
	u8 RColor;    //R背景
	u8 GColor;    //G背景
	u8 BColor;    //B背景	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
TBackBGDColor;


// 点
typedef struct tagKDVPOINT
{
	int nPosX;
	int nPosY;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
TPOINT,*PTPOINT;

// 矩形
typedef struct tagKDVRECT {
   int nPosLeft;
   int nPosTop;
   int nPosRight;
   int nPosBottom;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
TRECT;


// BMP文件头
typedef struct tagKDVBITMAPFILEHEADER 
{ 
	u16   wType;			// 文件类型,必须是BM
	u32   dwSize;			// 文件大小(字节)
	u16   wReserved1;		// 保留,必须是0
	u16   wReserved2;		// 保留,必须是0
	u32   dwOffBits;		// 偏移量(字节)
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
TBITMAPFILEHEADER, *PTBITMAPFILEHEADER;

// BMP信息头
typedef struct tagKDVBITMAPINFOHEADER
{
	u32  dwSize;			// 结构大小(字节)
	u32  dwWidth;			// 位图宽度(像素)
	u32  dwHeight;			// 位图高度(像素)
	u16  wPlanes;			// 目标设备使用的位面个数,必须是1
	u16  wBitCount;			// 每个像素使用的bit个数
	u32  dwCompression;		// 指定压缩类型
	u32  dwSizeImage;		// 图像大小(字节)
	u32  dwXPelsPerMeter;	// 目标设备上水平分辨率(像素/米)
	u32  dwYPelsPerMeter;	// 目标设备上垂直分辨率(像素/米)
	u32  dwClrUsed;			// 位图实际使用的颜色索引个数
	u32  dwClrImportant;	// 显示位图所需要的颜色索引个数
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
TBITMAPINFOHEADER, *PTBITMAPINFOHEADER;

// BMP256色调色板
typedef struct tagKDVPALETTEENTRY 
{ 
	u8 byRed;			// 红色
	u8 byGreen;		// 绿色
	u8 byBlue;		// 蓝色
	u8 byFlags;		// 使用方式
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
TPALETTEENTRY,*PTPALETTEENTRY;

// 画布
typedef struct tagKDVPAINT
{
	u16	wOrginX;		// 起点坐标X值
	u16	wOrginY;		// 起点坐标Y值
	u32	dwWidth;		// 显示区域宽
	u32	dwHeight;		// 显示区域高
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
TKDVPAINT,*PTKDVPAINT;

// 画笔
typedef struct tagKDVPEN
{
	u8 byWidth;			// 画笔宽度（像素为单位）
	u32 dwColor;			// 画笔颜色（RGB）
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
TKDVPEN,*PTKDVPEN;

// 画刷
typedef struct tagKDVBRUSH
{
	u32 dwColor;	// 画刷颜色
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
TKDVBRUSH,*PTKDVBRUSH;

// 字体
typedef struct tagKDVFONT
{
	u8  bySize;		// 字体大小 ( 像素为单位 )
	u8  byType;		// 字符类型（ 宋体，黑体，楷体）
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
TKDVFONT,*PTKDVFONT;

// 字库资源
typedef struct tagKDVFONTLIB
{
	u8 byWidth;			// 字符宽度和高度(字节为单位)
	u8 byHeight;
	u32 dwFontNum;		// 字库中字符个数
	u16 wSize;			// 每个字符的大小(字节为单位)
	u8 byType;			// 字库类型
	u8 byFontType;		// 字体类型
	u8 byFontSize;		// 字体大小
	u8 *pbyCNData;		// 中文字符数据区
	u8 *pbyENData;		// 西文字符数据区
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
TKDVFONTLIB,*PTKDVFONTLIB;

#endif // _KDVPAINTCONST_H_
