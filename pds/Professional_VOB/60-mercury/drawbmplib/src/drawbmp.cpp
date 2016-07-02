/*****************************************************************************
   模块名      : Kdv paint
   文件名      : kdvpaint.cpp
   相关文件    : kdvpaint.h, kdvpaintconst.h
   文件实现功能: 基本的画图功能
   作者        : liuhuiyun
   版本        : V0.9  Copyright(C) 2001-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2006/03/28  1.0         john          创建
******************************************************************************/
#include "drawbmp.h"

/*=============================================================================
函 数 名：CKdvGDI
功    能：CKdvGDI类的构造函数
注    意：
算法实现：
全局变量：
输入参数：
输出参数：
返 回 值：
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
CKdvGDI::CKdvGDI()
{
	m_bInitGDI = FALSE;
	m_dwBkColor = COLOR_WHITE;
	m_dwTextColor = COLOR_BLACK;
    m_pbyBmpDataBuff = NULL;
	m_byFontLibNum = 0;
	
	// 初始化字库
	memset( m_tKdvFontLib, 0, sizeof( m_tKdvFontLib ) );
  
	// 初始化调色板及其索引表
    memset( m_tPaletteEntry, 0, sizeof( m_tPaletteEntry ) );
	memset( m_abyPaletteIndex, 0, sizeof( m_abyPaletteIndex ) );

	// 初始化画图资源
	memset( &m_tKdvFont, 0, sizeof( m_tKdvFont ) );
	memset( &m_tKdvPoint, 0, sizeof( m_tKdvPoint ) );

    // 初始化画布
	m_bCreatePaint = FALSE;
	memset( &m_tKdvPaint, 0, sizeof( m_tKdvPaint ) );
}

/*=============================================================================
函 数 名：CKdvGDI
功    能：~CKdvGDI类的析构函数
注    意：
算法实现：
全局变量：
输入参数：
输出参数：
返 回 值：
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
/*lint -save -e1551 -e1579*/
CKdvGDI::~CKdvGDI()
{
	QuitGDI();
}
/*lint -restore*/

/*=============================================================================
函 数 名：InitGDI
功    能：初始化绘图系统
注    意：
算法实现：
全局变量：
输入参数：          
输出参数：
返 回 值：
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
BOOL32 CKdvGDI::InitGDI( void )
{
	if ( m_bInitGDI )  // 防止多次初始化
	{
		return FALSE;
	}

	// 设置系统调色板
    u16 wLp = 0;
	for ( wLp = 0; wLp < PAINTCOLOR; wLp++ )
	{
		m_tPaletteEntry[ wLp ].byRed = PALETTE[ RGB_COLOR_MAKEUP * wLp ];
		m_tPaletteEntry[ wLp ].byGreen = PALETTE[ RGB_COLOR_MAKEUP * wLp + 1 ];
		m_tPaletteEntry[ wLp ].byBlue = PALETTE[ RGB_COLOR_MAKEUP * wLp + 2 ];
		m_tPaletteEntry[ wLp ].byFlags = 0;
	}

	// 初始化调色表顺序表
	for ( wLp = 0; wLp < PAINTCOLOR; wLp++ )
	{
		m_abyPaletteIndex[ wLp ] = (u8)wLp;
	}
	
	// 排序,便于后续查找
	PalSortByShell();
    
    // 创建图形资源
    CreateBitMapRes();

	// 载入字库
	s8 achFontPath[ KDV_MAX_PATH ];
	memset(achFontPath, 0, sizeof(achFontPath));
	if ( !GetFontPath( achFontPath, KDV_MAX_PATH ) )
	{
        paintlog( LOG_LVL_ERROR, "[InitGDI]: Get Font Path Failed !\n");		
		return FALSE;
	}
	
	s8 achFontFullName[ KDV_MAX_PATH ];
	memset(achFontFullName, 0, sizeof(achFontFullName));
	sprintf(achFontFullName, "%s/%s", achFontPath, DEFAULT_FONTLIB_NAME);
	// 加载字库
	if ( !LoadFontLib( achFontFullName ) )
	{
        paintlog( LOG_LVL_ERROR, "[InitGDI]: Load Font Failed !\n");		
		return FALSE;
	}
    paintlog( LOG_LVL_KEYSTATUS, "[InitGDI]: Load Font Success !\n");

	// 初始化画笔,画刷,字体
	memset( &m_tKdvPen, 0, sizeof( m_tKdvPen ) );
	m_tKdvPen.byWidth = 1;
	m_tKdvPen.dwColor = COLOR_BLACK;

	memset( &m_tKdvBrush, 0, sizeof( m_tKdvBrush ) );
	m_tKdvBrush.dwColor = COLOR_WHITE;

	memset( &m_tKdvFont, 0, sizeof( m_tKdvFont ) );
	m_tKdvFont.byType = FONT_KAI;
	m_tKdvFont.bySize = FONT_SIZE_32;

	m_bInitGDI = TRUE;

	return TRUE;
}

/*=============================================================================
函 数 名：QuitGDI
功    能：释放GDI资源
注    意：
算法实现：
全局变量：
输入参数： 

输出参数：
返 回 值：成功返TRUE 失败返回FALSE
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
BOOL32 CKdvGDI::QuitGDI( void )
{
	if ( !m_bInitGDI )
		return FALSE;

	// 释放字库资源
	ReleaseFontLib();
	// 释放图形资源
	ReleaseBitMapRes();

	memset( m_tPaletteEntry, 0, sizeof( m_tPaletteEntry ) );
	memset( m_abyPaletteIndex, 0, sizeof( m_abyPaletteIndex ) );

	m_dwTextColor = 0;

	memset( &m_tKdvPen, 0, sizeof( m_tKdvPen ) );
    memset( &m_tKdvBrush, 0, sizeof( m_tKdvBrush ) );
	memset( &m_tKdvFont, 0, sizeof( m_tKdvFont ) );
	memset( &m_tKdvPoint, 0, sizeof( m_tKdvPoint ) );

	m_bInitGDI = FALSE;

	m_bCreatePaint = FALSE;    
    memset( &m_tKdvPaint, 0, sizeof( m_tKdvPaint ) );

	return TRUE;
}

/*=============================================================================
  函 数 名： GetFontPath
  功    能： 取字库路径
  算法实现： 
  全局变量： 
  参    数：  OUT s8 *pachFontPath
             IN u16 wLen
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CKdvGDI::GetFontPath( OUT s8 *pachFontPath, IN u16 wBufLen  )
{
	if( NULL == pachFontPath )
	{
		return FALSE;
	}
	u16 wPathLen = strlen(FONT_PATH);
	if( wPathLen > wBufLen )
	{
        paintlog( LOG_LVL_KEYSTATUS, "[GetFontPath]: The Buffer for font path is too small!\n");
		return FALSE;
	}

	sprintf(pachFontPath, "%s", FONT_PATH);
	
	return TRUE;
}

/*=============================================================================
函 数 名：GetFontLibIndex
功    能：获取当前字体的在字库中的索引
注    意：
算法实现：
全局变量：
输入参数：
输出参数： byLibIndex   当前字体的在字库中的索引
返 回 值： 成功返回TRUE 失败返回FALSE
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
BOOL32 CKdvGDI::GetFontLibIndex( u8 &byLibIndex )
{
	BOOL32 bFound = FALSE;
	for ( u8 byLp = 0; byLp < m_byFontLibNum; byLp++ )
	{
		if ( m_tKdvFont.bySize == m_tKdvFontLib[ byLp ].byFontSize
			&& m_tKdvFont.byType == m_tKdvFontLib[ byLp ].byFontType )
		{
			bFound = TRUE;
			byLibIndex = byLp;
			break;
		}
	}
	return bFound;
}

/*=============================================================================
函 数 名：PalSortByShell
功    能：对调色板排序
注    意：
算法实现：泡沫排序
全局变量：
输入参数： 
输出参数：
返 回 值：
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
void CKdvGDI::PalSortByShell( void )
{
	u16 wOffSet = PAINTCOLOR;
	BOOL32 bOrder;
	while ( wOffSet > 1 )
	{
		wOffSet = wOffSet / 2;
		do 
		{
			bOrder = TRUE;
			for ( u16 wLp = 0, wLp1 = wOffSet; wLp < ( PAINTCOLOR - wOffSet ); wLp++,wLp1++ )
			{
				u8 byIndex = m_abyPaletteIndex[ wLp ];
				u32 dwPalCorlor1 = RGB( m_tPaletteEntry[ byIndex ].byRed, 
					m_tPaletteEntry[ byIndex ].byGreen,
					m_tPaletteEntry[ byIndex ].byBlue );

				byIndex = m_abyPaletteIndex[ wLp1 ];
				u32 dwPalCorlor2 = RGB( m_tPaletteEntry[ byIndex ].byRed, 
					m_tPaletteEntry[ byIndex ].byGreen,
					m_tPaletteEntry[ byIndex ].byBlue );
				if ( dwPalCorlor1 > dwPalCorlor2 )
				{
					u8 byTemp = m_abyPaletteIndex[ wLp1 ];
					m_abyPaletteIndex[ wLp1 ] = m_abyPaletteIndex[ wLp ];
					m_abyPaletteIndex[ wLp ] = byTemp;
					bOrder = FALSE;
				}

			}
		} while( !bOrder );
	}
	return;
}

/*=============================================================================
函 数 名：GetColorIndex
功    能：根据RGB获取调色板索引
注    意：
算法实现：二分查找
全局变量：
输入参数： dwColorValue RGB值
输出参数：
返 回 值：u8 该颜色在调色半板中的索引
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
u8 CKdvGDI::GetColorIndex( u32 dwColorValue )
{
	u16 wLow = 0;
	u16 wHigh = PAINTCOLOR - 1;
	u16 wMid = 0;
	BOOL32 bFound = FALSE;
	u8 byIndex = 0;
	u32 dwPalColor;

	while ( wLow <= wHigh && !bFound )
	{
		wMid = ( wLow + wHigh ) / 2;
		byIndex = m_abyPaletteIndex[ wMid ];
		dwPalColor = RGB(  m_tPaletteEntry[ byIndex ].byRed, 
			m_tPaletteEntry[ byIndex ].byGreen,
			m_tPaletteEntry[ byIndex ].byBlue ); 

		if ( dwColorValue < dwPalColor )
		{	
			wHigh = wMid - 1;
		}
		else if ( dwColorValue > dwPalColor )
		{	
			wLow = wMid + 1;             
		}
		else
		{	
			bFound = TRUE; 
		}
	}

	//如果没有, 取相似值
	if ( !bFound )
	{
		u16 wMin = 0; // wLow - 1;
		byIndex = m_abyPaletteIndex[ wMin ];
		u32 dwMinColor = RGB(  m_tPaletteEntry[ byIndex ].byRed, 
			m_tPaletteEntry[ byIndex ].byGreen,
			m_tPaletteEntry[ byIndex ].byBlue ); 

		for ( u16 wLp = wMin + 1; wLp <= wHigh + 1; wLp++ )
		{
			byIndex = m_abyPaletteIndex[ wLp ];
			dwPalColor = RGB(  m_tPaletteEntry[ byIndex ].byRed, 
				m_tPaletteEntry[ byIndex ].byGreen,
				m_tPaletteEntry[ byIndex ].byBlue ); 
			if ( abs( dwColorValue - dwPalColor ) < abs( dwColorValue - dwMinColor ) )
			{
				dwMinColor = dwPalColor;  // 找最接近的值
				wMin = wLp;
			}
		}
		wMid = wMin;
	}

	return m_abyPaletteIndex[ wMid ];
}

/*=============================================================================
函 数 名：SetCurrentFont
功    能：设置当前字体属性
注    意：
算法实现：
全局变量：
输入参数： tKdvFont 新的字体
输出参数： 
返 回 值：旧的字体
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
TKDVFONT CKdvGDI::SetCurrentFont( TKDVFONT &tKdvFont )
{
	TKDVFONT tCurKdvFont = *GetCurrentFont();
	memset( &m_tKdvFont, 0, sizeof( m_tKdvFont ) );
	memcpy( &m_tKdvFont, &tKdvFont, sizeof( m_tKdvFont ) );
	return tCurKdvFont;
}

/*=============================================================================
函 数 名：SetTextColor
功    能：设置当前文本颜色
注    意：
算法实现：
全局变量：
输入参数： dwColorValue 新的文本颜色
输出参数： 
返 回 值： 旧的文本颜色
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
u32 CKdvGDI::SetTextColor( u32 dwColorValue )
{
	u32 dwCurTextColor = m_dwTextColor;
	m_dwTextColor = dwColorValue;
	return dwCurTextColor;
}

/*=============================================================================
函 数 名：SetCurrentBrush
功    能：设置当前画刷属性
注    意：
算法实现：
全局变量：
输入参数： tKdvBrush 新的画刷
输出参数： 
返 回 值：旧的画刷
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
TKDVBRUSH CKdvGDI::SetCurrentBrush( TKDVBRUSH &tKdvBrush )
{

	TKDVBRUSH tCurKdvBrush = *GetCurrentBrush();
	memset( &m_tKdvBrush, 0, sizeof( m_tKdvBrush ) );
	memcpy( &m_tKdvBrush, &tKdvBrush, sizeof( m_tKdvBrush ) );

	return tCurKdvBrush;
}

/*=============================================================================
函 数 名：SetCurrentPen
功    能：设置当前画笔属性
注    意：
算法实现：
全局变量：
输入参数： tKdvPen 新的画笔
输出参数： 
返 回 值：旧的画笔
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
TKDVPEN CKdvGDI::SetCurrentPen( TKDVPEN &tKdvPen )
{
	TKDVPEN tCurKdvPen = *GetCurrentPen();
	memset( &m_tKdvPen, 0, sizeof( m_tKdvPen ) );
	memcpy( &m_tKdvPen, &tKdvPen, sizeof( m_tKdvPen ) );
	return tCurKdvPen;
}

/*=============================================================================
  函 数 名： SetCurrentPaint
  功    能： 设置当前画布
  算法实现： 
  全局变量： 
  参    数： TKDVPAINT tPaint
  返 回 值： TKDVPAINT 
=============================================================================*/
TKDVPAINT CKdvGDI::SetCurrentPaint(TKDVPAINT tPaint)
{
	TKDVPAINT tRetValue = m_tKdvPaint;
	memset(&m_tKdvPaint, 0, sizeof(m_tKdvPaint));
	m_tKdvPaint = tPaint;
	return tRetValue;
} 
/*=============================================================================
函 数 名：SetBkColor
功    能：设置当前画布背景色
注    意：
算法实现：
全局变量：
输入参数： dwBkColor 新的背景色
输出参数： 
返 回 值： 旧的背景色
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
u32 CKdvGDI::SetBkColor( u32 dwBkColor )
{
    if ( !m_bInitGDI )
	{
        paintlog( LOG_LVL_ERROR, "[SetBkColor]: Invalid input param!\n");
		return INVALID_COLOR;
	}

	u32 dwOldBkColor = m_dwBkColor; // 保存原有背景色

	u8 byClrIndex = GetColorIndex( dwBkColor );
	u32 dwSize =  m_tKdvPaint.dwWidth * m_tKdvPaint.dwHeight;

	memset( m_pbyBmpDataBuff, byClrIndex, dwSize );  // 设置背景色

	m_dwBkColor = RGB( m_tPaletteEntry[ byClrIndex ].byRed, 
					   m_tPaletteEntry[ byClrIndex ].byGreen,
					   m_tPaletteEntry[ byClrIndex ].byBlue );

	return dwOldBkColor;

}

/*=============================================================================
函 数 名：SetPixel
功    能：画点
注    意：
算法实现：
全局变量：
输入参数： nPosX nPosY   点在画布中的坐标
           dwColorValue  RGB值
输出参数： 
返 回 值： 点的RGB值
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
u32 CKdvGDI::SetPixel( s32 nPosX, s32 nPosY, u32 dwColorValue )
{
	u32 dwRet = INVALID_COLOR;
	// 参数检测
	if ( nPosX >= (s32)m_tKdvPaint.dwWidth || nPosX < 0 )
	{	
		return dwRet;
	}
	if ( nPosY >= (s32)m_tKdvPaint.dwHeight || nPosY < 0 )
	{
		return dwRet;
	}
	// 取颜色对应调色板中的序号
	u8 byClrIndex = GetColorIndex( dwColorValue );

	dwRet = dwColorValue;

	// 默认为8*8位
	u32 dwPos = ( m_tKdvPaint.dwHeight - 1 - nPosY ) * m_tKdvPaint.dwWidth + nPosX ;
	if ( NULL == (m_pbyBmpDataBuff + dwPos) )
	{
		dwRet = INVALID_COLOR;
	}
	else
	{
		m_pbyBmpDataBuff[ dwPos ] = byClrIndex;  // 对应点记录颜色序号
	}

	return dwRet;
}

/*=============================================================================
函 数 名：SetPixel
功    能：画点
注    意：
算法实现：
全局变量：
输入参数： tPoint      该点在画布中的坐标
           dwColor     RGB值
输出参数： 
返 回 值： 点的RGB值
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
u32 CKdvGDI::SetPixel( TPOINT tPoint, u32 dwColor )
{
	return SetPixel( tPoint.nPosX, tPoint.nPosY, dwColor );
}

/*=============================================================================
函 数 名：LineTo
功    能：画线,从当前位置到指定位置
注    意：
算法实现：高速直线绘制算法
全局变量：
输入参数： nPosX  nPosY   线的终点坐标        
输出参数： 
返 回 值：成功返回TRUE 失败返回FALSE
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
BOOL32 CKdvGDI::LineTo( s32 nPosX, s32 nPosY )
{	

	s32 nP, nN, nTmpX, nTmpY, nTn;

	// 是否越界
	if ( nPosX >= (s32)m_tKdvPaint.dwWidth || nPosX < 0 )
	{
		return FALSE;
	}

	if ( nPosY >= (s32)m_tKdvPaint.dwHeight || nPosY < 0 )
	{
		return FALSE;
	}

	// 保存当前起点位置
	TPOINT tCurPoint;
	memcpy( &tCurPoint, &m_tKdvPoint, sizeof( tCurPoint ) );

	if( m_tKdvPoint.nPosY == nPosY )
	{
		if( m_tKdvPoint.nPosX > nPosX ) // 从左向右画线
		{
			nTmpX = nPosX;
			nPosX = m_tKdvPoint.nPosX;
			m_tKdvPoint.nPosX = nTmpX;
		}
		return LevelLine( m_tKdvPoint.nPosX,
			              m_tKdvPoint.nPosY,
			              nPosX - m_tKdvPoint.nPosX + 1 ); // 水平线
	}
	else if( m_tKdvPoint.nPosX == nPosX )
	{
		if( m_tKdvPoint.nPosY > nPosY ) // 从下往上画线
		{
			nTmpY = nPosY;
			nPosY = m_tKdvPoint.nPosY;
			m_tKdvPoint.nPosY = nTmpY;
		}
		return VertLine( m_tKdvPoint.nPosX,
			             m_tKdvPoint.nPosY,
			             nPosY - m_tKdvPoint.nPosY + 1 ); // 垂直线
	}
	else if( abs( nPosY - m_tKdvPoint.nPosY ) <= abs( nPosX - m_tKdvPoint.nPosX ) )  // 与x轴的夹角小于等于45度
	{
		// 保证m_tKdvPoint在指定点的左边
		if( ( nPosY < m_tKdvPoint.nPosY && nPosX < m_tKdvPoint.nPosX ) 
			||( m_tKdvPoint.nPosY <= nPosY && m_tKdvPoint.nPosX > nPosX ) )
		{
			nTmpX = nPosX;
			nTmpY = nPosY;
			nPosX = m_tKdvPoint.nPosX;
			nPosY = m_tKdvPoint.nPosY;
			m_tKdvPoint.nPosX = nTmpX;
			m_tKdvPoint.nPosY = nTmpY;
		}

		if( nPosY >= m_tKdvPoint.nPosY && nPosX >= m_tKdvPoint.nPosX )  // 指定点在原点的右上
		{
			nTmpX = nPosX-m_tKdvPoint.nPosX;
			nTmpY = nPosY-m_tKdvPoint.nPosY;
			nP = 2 * nTmpY;
			nN = 2 * nTmpX - 2 * nTmpY;
			nTn = nTmpX;
			while( m_tKdvPoint.nPosX <= nPosX )
			{
				if( nTn >= 0 )
				{	nTn -= nP; }
				else 
				{
					nTn += nN;
					m_tKdvPoint.nPosY++;
				}

				s32 nX = m_tKdvPoint.nPosX++;
				for ( u8 byLp = 0; byLp < m_tKdvPen.byWidth; byLp++ )
				{
					if ( byLp % 2 == 0 )
					{
						SetPixel( nX + byLp, 
						          m_tKdvPoint.nPosY,
						          m_tKdvPen.dwColor );
					}
					else
					{
						SetPixel( nX - byLp, 
						          m_tKdvPoint.nPosY,
						          m_tKdvPen.dwColor );
					}
				}
			}
		}
		else   // 右下
		{
			nTmpX = nPosX - m_tKdvPoint.nPosX;
			nTmpY = nPosY - m_tKdvPoint.nPosY;
			nP = -2 * nTmpY;
			nN = 2 * nTmpX + 2 * nTmpY;
			nTn = nTmpX;
			while( m_tKdvPoint.nPosX <= nPosX )
			{
				if( nTn >= 0 )
				{
					nTn -= nP;
				}
				else 
				{
					nTn += nN;
					m_tKdvPoint.nPosY--;
				}
				
				s32 nX = m_tKdvPoint.nPosX++;
				for ( u8 byLp = 0; byLp < m_tKdvPen.byWidth; byLp++ )
				{
					if ( byLp % 2 == 0 )
					{	SetPixel( nX + byLp,
								  m_tKdvPoint.nPosY,
								  m_tKdvPen.dwColor );
					}
					else
					{
						SetPixel( nX - byLp,
								  m_tKdvPoint.nPosY,
								  m_tKdvPen.dwColor );
					}
				}
			}
		}
	}
	else
	{
		nTmpX = m_tKdvPoint.nPosX;
		m_tKdvPoint.nPosX = nPosY;
		nPosY = nTmpX;
		nTmpY = m_tKdvPoint.nPosY;
		m_tKdvPoint.nPosY = nPosX;
		nPosX = nTmpY;
		if( ( nPosY < m_tKdvPoint.nPosY && nPosX < m_tKdvPoint.nPosX )
			||( m_tKdvPoint.nPosY <= nPosY && m_tKdvPoint.nPosX > nPosX ) )
		{
			nTmpX = nPosX;
			nTmpY = nPosY;
			nPosX = m_tKdvPoint.nPosX;
			nPosY = m_tKdvPoint.nPosY;
			m_tKdvPoint.nPosX = nTmpX;
			m_tKdvPoint.nPosY = nTmpY;
		}

		if( nPosY >= m_tKdvPoint.nPosY && nPosX >= m_tKdvPoint.nPosX )
		{
			nTmpX = nPosX-m_tKdvPoint.nPosX;
			nTmpY = nPosY-m_tKdvPoint.nPosY;
			nP = 2 * nTmpY;
			nN = 2 * nTmpX - 2 * nTmpY;
			nTn = nTmpX;
			while( m_tKdvPoint.nPosX <= nPosX )
			{
				if( nTn >= 0)
				{	
					nTn -= nP;
				}
				else 
				{ 
					nTn += nN;
					m_tKdvPoint.nPosY++;
				}

				s32 nY = m_tKdvPoint.nPosX++;
				for ( u8 byLp = 0; byLp < m_tKdvPen.byWidth; byLp++ )
				{
					if ( byLp % 2 == 0 )
					{
						SetPixel( m_tKdvPoint.nPosY,
						          nY + byLp,
								  m_tKdvPen.dwColor );
					}
					else
					{
						SetPixel( m_tKdvPoint.nPosY,
								  nY - byLp,
								  m_tKdvPen.dwColor );
					}
				}
			}
		}
		else
		{
			nTmpX = nPosX - m_tKdvPoint.nPosX;
			nTmpY = nPosY - m_tKdvPoint.nPosY;
			nP = -2 * nTmpY;
			nN = 2 * nTmpX + 2 * nTmpY;
			nTn = nTmpX;
			while( m_tKdvPoint.nPosX <= nPosX )
			{
				if( nTn >= 0 )
				{
					nTn -= nP;
				}
				else 
				{
					nTn += nN;
					m_tKdvPoint.nPosY--;
				}

				s32 nY = m_tKdvPoint.nPosX++;
				for ( u8 byLp = 0; byLp < m_tKdvPen.byWidth; byLp++ )
				{
					if ( byLp % 2 == 0 )
						SetPixel( m_tKdvPoint.nPosY,
								  nY + byLp,
								  m_tKdvPen.dwColor );
					else
						SetPixel( m_tKdvPoint.nPosY,
						          nY - byLp,
								  m_tKdvPen.dwColor );

				}
			}
		}
	}

	// 恢复当前起点位置
	memcpy( &m_tKdvPoint, &tCurPoint, sizeof( m_tKdvPoint ) );

	return TRUE;
}

/*=============================================================================
函 数 名：LineTo
功    能：画线,从当前位置到指定位置
注    意：
算法实现：高速直线绘制算法
全局变量：
输入参数：tPoint  线的终点坐标        
输出参数： 
返 回 值：成功返回TRUE 失败返回FALSE
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
BOOL32 CKdvGDI::LineTo( TPOINT tPoint )
{
	return LineTo( tPoint.nPosX, tPoint.nPosY );
}

/*=============================================================================
函 数 名：LevelLine
功    能：画水平线
注    意：
算法实现：
全局变量：
输入参数：nPosX nPosY  终点的坐标
          dwWidth      水平线的宽度
输出参数：
返 回 值： 成功返回TRUE 失败返回FALSE
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
BOOL32 CKdvGDI::LevelLine( int nPosX, int nPosY, u32 dwWidth )
{
	// 参数校验
	if ( nPosY > (int)m_tKdvPaint.dwHeight || nPosY < 0 )
	{
		return FALSE;
	}

	if ( nPosX + dwWidth > m_tKdvPaint.dwWidth + 1 || nPosX < 0 )
	{
		return FALSE;
	}

	u8 byClrIndex = GetColorIndex( m_tKdvPen.dwColor );

	for ( int byLp = 0; byLp < m_tKdvPen.byWidth; byLp++ )
	{
		
		u32 dwPos = ( m_tKdvPaint.dwHeight - 1 - nPosY ) * m_tKdvPaint.dwWidth + nPosX ;
		if ( NULL == m_pbyBmpDataBuff + dwPos )
		{
			continue;
		}

		memset( m_pbyBmpDataBuff + dwPos, byClrIndex, dwWidth );
		nPosY++;
	}
	return TRUE;
}


/*=============================================================================
函 数 名：VertLine
功    能：画垂直线
注    意：
算法实现：
全局变量：
输入参数：nPosX nPosY  终点的坐标
          dwWidth      水平线的宽度
输出参数：
返 回 值： 成功返回TRUE 失败返回FALSE
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
BOOL32 CKdvGDI::VertLine( int nPosX, int nPosY, u32 dwHeight )
{
	// 参数校验
	if ( nPosX > (int)m_tKdvPaint.dwWidth || nPosX < 0 )
	{
		return FALSE;
	}

	if ( nPosY + dwHeight > m_tKdvPaint.dwHeight + 1 || nPosY < 0 )
	{
		return FALSE;
	}

	for ( u8 byOutLp = 0; byOutLp < m_tKdvPen.byWidth; byOutLp++ )
	{
		for ( u8 byInLp = 0; byInLp < dwHeight; byInLp++ )
		{
			u32 dwColorValue = SetPixel( nPosX + byOutLp, nPosY + byInLp, m_tKdvPen.dwColor );
			if ( INVALID_COLOR == dwColorValue  )
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

/*=============================================================================
函 数 名：MoveTo
功    能：改变当前位置
注    意：
算法实现：
全局变量：
输入参数：nPosX nPosY 新的位置的坐标       
输出参数： 
返 回 值：TPOINT  旧的位置的坐标
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
TPOINT CKdvGDI::MoveTo( s32 nPosX, s32 nPosY )
{
	TPOINT tCurPoint;

	memset( &tCurPoint, 0, sizeof( tCurPoint ) );
	memcpy( &tCurPoint, &m_tKdvPoint, sizeof( tCurPoint ) );

	m_tKdvPoint.nPosX = nPosX;
	m_tKdvPoint.nPosY = nPosY;

	return tCurPoint;
}

/*=============================================================================
函 数 名：MoveTo
功    能：改变当前位置
注    意：
算法实现：
全局变量：
输入参数：tPoint 新的位置的坐标         
输出参数： 
返 回 值：TPOINT  旧的位置的坐标
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
TPOINT CKdvGDI::MoveTo( TPOINT tPoint )
{
	return MoveTo( tPoint.nPosX, tPoint.nPosY );
}

/*=============================================================================
函 数 名：DrawRect
功    能：画矩形
注    意：
算法实现：
全局变量：
输入参数：nPosX nPosY 新的位置的坐标
          dwWidth     矩形的宽度
		  dwHeight    矩形的长度
输出参数： 
返 回 值：成功返回TRUE 失败返回FALSE
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
BOOL32 CKdvGDI::DrawRect( s32 nPosX, s32 nPosY, u32 dwWidth, u32 dwHeight )
{
	// 预防越界
	if ( nPosX < 0  || nPosX + dwWidth > m_tKdvPaint.dwWidth )
	{	
		return FALSE;
	}

	if ( nPosY < 0 || nPosY + dwHeight > m_tKdvPaint.dwHeight )
	{
		return FALSE;
	}

	dwWidth -= 1;  //  预留边界
	dwHeight -= 1; // 

	// 保存当前起点位置
	TPOINT tLastPoint = MoveTo( nPosX, nPosY );


	// 绘制矩形

	// 上边
	BOOL32 bRet = LineTo( nPosX + dwWidth, nPosY );
	if ( !bRet )
	{
        paintlog( LOG_LVL_KEYSTATUS, "[DrawRect]: Fail to call LineTo()! Pos: top !\n");
		return FALSE;
	}

	// 左边
	bRet = LineTo( nPosX, nPosY + dwHeight );
	if ( !bRet )
	{
        paintlog( LOG_LVL_KEYSTATUS, "[DrawRect]: Fail to call LineTo()! Pos: left !\n");
		return FALSE;
	}

	// 右边
	MoveTo( nPosX + dwWidth - m_tKdvPen.byWidth + 1, nPosY );
	bRet = LineTo( nPosX + dwWidth - m_tKdvPen.byWidth + 1, 
		nPosY + dwHeight - m_tKdvPen.byWidth + 1 );
	if ( !bRet )
	{
        paintlog( LOG_LVL_KEYSTATUS, "[DrawRect]: Fail to call LineTo()! Pos: right !\n");
		return FALSE;
	}

	// 下边
	MoveTo( nPosX, nPosY + dwHeight - m_tKdvPen.byWidth + 1 );
	bRet = LineTo( nPosX + dwWidth - m_tKdvPen.byWidth + 1, 
		nPosY + dwHeight - m_tKdvPen.byWidth + 1 );
	if ( !bRet )
	{
        paintlog( LOG_LVL_KEYSTATUS, "[DrawRect]: Fail to call LineTo()! Pos: bottom !\n");
		return FALSE;
	}

	// 绘制长和宽接口处
	MoveTo( nPosX + dwWidth - m_tKdvPen.byWidth + 1, 
		nPosY + dwHeight - m_tKdvPen.byWidth + 1 );
	bRet = LineTo( nPosX + dwWidth - m_tKdvPen.byWidth + 1, nPosY + dwHeight );
	if ( !bRet )
	{
        paintlog( LOG_LVL_KEYSTATUS,   "[DrawRect]: Fail to call LineTo()! Pos: interface !\n");
		return FALSE;
	}

	// 恢复当前起点位置
	MoveTo( tLastPoint );

	return TRUE;
}

/*=============================================================================
函 数 名：DrawRect
功    能：画矩形
注    意：
算法实现：
全局变量：
输入参数：tRect 矩形属性
输出参数： 
返 回 值：成功返回TRUE 失败返回FALSE
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
BOOL32 CKdvGDI::DrawRect( TRECT tRect )
{
	INITIALIZED(m_bInitGDI);
	
	// 预防越界(矩形只能在画布范围之内)
	if ( tRect.nPosLeft < 0 || tRect.nPosRight < tRect.nPosLeft )
	{
		return FALSE;
	}

	if ( tRect.nPosTop < 0 || tRect.nPosBottom < tRect.nPosTop )
	{
		return FALSE;
	}

	return DrawRect( tRect.nPosLeft, tRect.nPosTop,
		tRect.nPosRight - tRect.nPosLeft,
		tRect.nPosBottom - tRect.nPosTop );
}

/*=============================================================================
函 数 名：FillRect
功    能：填充矩形
注    意：
算法实现：
全局变量：
输入参数：nPosX nPosY 矩形左上顶点的坐标
          dwWidth     矩形宽度
          dwHeight    矩形长度
输出参数： 
返 回 值：成功返回TRUE 失败返回FALSE
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
BOOL32 CKdvGDI::FillRect( int nPosX, int nPosY, u32 dwWidth, u32 dwHeight )
{
	INITIALIZED(m_bInitGDI);

	// 预防越界(矩形只能在画布范围之内)
	if ( nPosX < 0 || nPosX + dwWidth > m_tKdvPaint.dwWidth )
	{
		return FALSE;
	}

	if ( nPosY < 0  || nPosY + dwHeight > m_tKdvPaint.dwHeight )
	{
		return FALSE;
	}

	// 保存当前起点位置
	TPOINT tLastPoint = MoveTo( nPosX, nPosY );
	TKDVPEN tLastKdvPen  = *GetCurrentPen();

	// 使用画笔填充颜色
	TKDVPEN tKdvPen;
	memset( &tKdvPen, 0, sizeof( tKdvPen ) );
	tKdvPen.byWidth = MAX_BYTE_FILL;
	tKdvPen.dwColor = m_tKdvBrush.dwColor;
	SetCurrentPen( tKdvPen );

	u32 dwCount = dwHeight / MAX_BYTE_FILL;
	for ( u16 wLp = 0; wLp < dwCount; wLp++ )
	{
		// 填充矩形
		MoveTo( nPosX, nPosY + MAX_BYTE_FILL * wLp );
		LineTo( nPosX + dwWidth - 1, nPosY + MAX_BYTE_FILL * wLp );
	}

	// 填充最后一部份
	MoveTo( nPosX, nPosY + MAX_BYTE_FILL * dwCount );
	tKdvPen.byWidth = ( u8 )(dwHeight - dwCount * MAX_BYTE_FILL);
	SetCurrentPen( tKdvPen );
	
	LineTo( nPosX + dwWidth - 1, nPosY + MAX_BYTE_FILL * dwCount );// 填充矩形

	MoveTo( tLastPoint );// 恢复当前起点位置
	
	SetCurrentPen( tLastKdvPen );// 恢复当前画笔

	return TRUE;
}

/*=============================================================================
函 数 名：FillRect
功    能：填充矩形
注    意：
算法实现：
全局变量：
输入参数：tRect 矩形属性
输出参数： 
返 回 值：成功返回TRUE 失败返回FALSE
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
BOOL32 CKdvGDI::FillRect( TRECT tRect )
{
	return FillRect( tRect.nPosLeft, tRect.nPosTop,
		tRect.nPosRight - tRect.nPosLeft,
		tRect.nPosBottom - tRect.nPosTop );
}

/*=============================================================================
函 数 名：DrawTextInRect
功    能：输出字符串到矩形框中
注    意：
算法实现：
全局变量：
输入参数：pchText       要输出的字符串
          dwTextLen     字符串长度
          tRect         矩形属性
输出参数： 
返 回 值：返回实际的文本输出长度
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
u32 CKdvGDI::TextOutInRect( const char* pchText, u32 dwTextLen, TRECT tRect )
{
	u32 dwActTextLen = 0;
	// 获取字体属性
	u8 byLibIndex = 0;
	BOOL32 bRet = GetFontLibIndex( byLibIndex );
	if ( !bRet )
	{
		return dwActTextLen;
	}
	// 取字体信息
	TKDVFONTLIB *ptFontLib = &m_tKdvFontLib[ byLibIndex ];
	
	if ( tRect.nPosBottom - tRect.nPosTop < ptFontLib->byHeight * 8 )
	{
		return dwActTextLen;
	}
	
	// 参数校验
	if ( tRect.nPosLeft < 0 
		|| tRect.nPosTop < 0
		|| ( tRect.nPosRight - tRect.nPosLeft ) > (s32)m_tKdvPaint.dwWidth 
		|| ( tRect.nPosBottom - tRect.nPosTop ) > (s32)m_tKdvPaint.dwHeight )

	{
		return dwActTextLen;
	}

	// 实际可容纳的字符数目
	dwActTextLen = 2 * ( tRect.nPosRight - tRect.nPosLeft ) / ( ptFontLib->byWidth * 8 );

	if ( dwActTextLen > dwTextLen )
	{
		dwActTextLen = dwTextLen;
	}

	// 画字串
	return TextOut( tRect.nPosLeft, tRect.nPosTop, pchText, dwActTextLen );
}

/*=============================================================================
函 数 名：TextOut
功    能：输出文本
注    意：
算法实现：
全局变量：
输入参数：nPosX nPosY   输出文本的起点坐标
          pchText       要输出的字符串
          dwTextLen     字符串长度
输出参数： 
返 回 值：返回实际的文本输出长度
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
u32 CKdvGDI::TextOut( int nPosX, int nPosY, const char* pchText, u32 dwTextLen )
{
	u32 dwActTextLen = 0; // 实际显示文本长度
	// 参数校验
	if ( NULL == pchText )
	{
		return dwActTextLen;
	}

	if ( nPosX >= (int)m_tKdvPaint.dwWidth || nPosX < 0 )
	{
		return dwActTextLen;
	}
	if ( nPosY >= (int)m_tKdvPaint.dwHeight || nPosY < 0 )
	{
		return dwActTextLen;
	}
	
	u8 byCurChar;		// 当前获取的字符
	u8 byCompress;		// 是否压缩

	u8 byArea;		// 区码
	u8 byPosi;		// 位码
	u32 dwFontIndex;	// 字符在字库中的索引

	u8 byLibIndex = 0;
	BOOL bRet = GetFontLibIndex( byLibIndex );
	if ( !bRet )
	{
		return dwActTextLen;
	}

	TKDVFONTLIB *ptFontLib = &m_tKdvFontLib[byLibIndex];
	
	while( dwTextLen > 0 )
	{
		if ( NULL == pchText )
		{
			dwTextLen--;
			continue;
		}

		dwActTextLen++;
		byCurChar = *( ( u8* ) pchText++ );
		if ( byCurChar == ' ' )
		{
			dwTextLen--;
			nPosX += ( 8 * ptFontLib->byWidth  ) >> 1;
			continue;
		}

		if ( byCurChar < 32 )  // 不可显示字符
		{
			dwTextLen--;
			continue;
		}

	    u8* pbyFontData = NULL;
		if ( byCurChar < 161 )
		{
			if ( NULL == ptFontLib->pbyENData )
			{
				nPosX += ( 8 * ptFontLib->byWidth  ) >> 1;
				dwTextLen--;
				continue;
			}

			dwFontIndex = ( byCurChar - 32 ) * ptFontLib->wSize;
			byCompress = 1;

            pbyFontData =  ptFontLib->pbyENData+dwFontIndex;
		}		 
		else  //中文字符
		{
			byArea = byCurChar - 0xA1;   // 获得区位码
			dwTextLen--;
			if ( 0 == dwTextLen )
			{
				break;
			}

			byPosi = *( ( u8* )pchText++ ) - 0xA1;
			byCompress = 0;
			dwActTextLen++;
			dwFontIndex = byArea * 94 + byPosi; //2016E,2007D
			dwFontIndex *= ptFontLib->wSize;

			pbyFontData =  ptFontLib->pbyCNData+dwFontIndex;

		}

		// 横向字库的显示
		s32 nbyte = 0;
		s32 nScaleX = 1;
		s32 nScaleY = 1;
		
		for ( s32 nLp = 0, cx = nPosX, cy = nPosY; 
			nLp < ptFontLib->byHeight * 8; nLp++, cy += nScaleY, cx = nPosX )
		{
			for ( s32 nLp1 = 0; nLp1 < ptFontLib->byWidth; nLp1++, nbyte++ )
			{
				for ( s32 nLp2 = 7; nLp2 >= 0; nLp2 -= ( byCompress + 1 ), cx += nScaleX )
				{
					if ( 1  == ( ( pbyFontData[ nbyte ] >> nLp2 ) & 1 ))
					{
						SetPixel( cx , cy, m_dwTextColor );
					}
				}
			}
		}
		nPosX += (( ptFontLib->byWidth * 8) >> byCompress );
		dwTextLen--;			
	}

	return dwActTextLen;
}

/*=============================================================================
函 数 名：LoadFontLib
功    能：载入字库，将对应的字库数据读入内存
注    意：
算法实现：
全局变量：
输入参数：pchFontPath 文件名
输出参数： 
返 回 值：成功返回TRUE 失败返回FALSE
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
BOOL32 CKdvGDI::LoadFontLib( const char* pchFontLibName )
{	
	if ( NULL == pchFontLibName )
	{
        paintlog( LOG_LVL_ERROR, "[LoadFontLib]: Invalide input param!\n" );
		return FALSE;
	}
	/*lint -save -esym(438, hRead)*/
    // 读字库   (没有对字库文件压缩， 多字库较大时，可考虑压缩)
   	FILE *hRead = fopen( pchFontLibName, "rb" );
	if ( NULL == hRead )
	{	
        paintlog( LOG_LVL_ERROR, "[LoadFontLib]: Fail to open the font lib file: %s!\n", pchFontLibName );
		return FALSE;
	}
    
	// 文件大小
	fseek( hRead, 0, SEEK_END ); 
	s32 nFileLen = ftell(hRead);
	if ( INIT_VALUE == nFileLen )
	{
		fclose( hRead );
		return FALSE;
	}
	/*lint -save -esym(438, pbyDataBuf)*/
	u8 *pbyDataBuf = new u8[ nFileLen + 1 ]; // 压缩库
	if ( NULL == pbyDataBuf )
	{
        paintlog( LOG_LVL_ERROR, "[LoadFontLib]: Open FontLib File Failure!\n");
        fclose( hRead );
		return FALSE;
	}

	memset( pbyDataBuf, 0xFF, nFileLen + 1 );
	fseek( hRead, 0, SEEK_SET );
	fread ( pbyDataBuf, nFileLen, 1, hRead);

	u8* pbyCurPos = pbyDataBuf; // 记录当前位置

	while(1)
	{
		u8 byFontLibType = 0;// 字库类型(1字节)
		memcpy( ( void* )&byFontLibType, ( void * )pbyCurPos, sizeof( byFontLibType ) );
		if( 0xff == byFontLibType )  // 整个字库结束标志
        {
            paintlog( LOG_LVL_DETAIL, "[LoadFontLib]: Read the end of font lib file!\n");
			break;
		}
		
		u8 byFontSize = 0; // 字体大小
		memcpy( ( void* )&byFontSize, ( void * )( pbyCurPos + 1 ), sizeof( byFontSize ) );// 字体大小(1字节)
		
		u8 byFontType = 0;
		memcpy( ( void* )&byFontType, ( void * )( pbyCurPos + 2 ), sizeof( byFontType ) );// 字体类型(1字节)
		
		u32 dwChineseSize = 0;
		memcpy( ( void* )&dwChineseSize, ( void * )(pbyCurPos + 3), sizeof( dwChineseSize ) );// 汉字库大小(4字节)
		
		u32 dwEngSize = 0;
		memcpy( ( void* )&dwEngSize, ( void * )(pbyCurPos + 7), sizeof( dwEngSize ) );// 西文字库大小(4字节)

		TKDVFONTLIB *ptKdvFontLib = &m_tKdvFontLib[m_byFontLibNum];
		m_byFontLibNum++;
		
		switch( byFontLibType )
		{
		case FT_HZK16 :
			ptKdvFontLib->byWidth = 2;
			ptKdvFontLib->byHeight = 2;
			ptKdvFontLib->wSize = 32;  // 2*2 = 4 bytes , 4*8 = 32 bits
			break;
			
		case FT_HZK24 :
			ptKdvFontLib->byWidth = 3;
			ptKdvFontLib->byHeight = 3;
			ptKdvFontLib->wSize = 72;  // 3*3 = 9 bytes, 9*8 = 72 bits
			break;
			
		case FT_HZK32 :
			ptKdvFontLib->byWidth = 4;
			ptKdvFontLib->byHeight = 4;
			ptKdvFontLib->wSize = 128; // 4*4 = 16 bytes, 16*8 = 128 bits
			break;			
			
		default :
			return FALSE;
			
		}

		ptKdvFontLib->byType = byFontLibType;
		ptKdvFontLib->byFontType = byFontType;
		ptKdvFontLib->byFontSize = byFontSize;
		
		// 读汉字库
		if ( NULL != ptKdvFontLib->pbyCNData )
		{
			delete []ptKdvFontLib->pbyCNData;
			ptKdvFontLib->pbyCNData = NULL;
		}
		ptKdvFontLib->pbyCNData = new u8[ dwChineseSize ];

		memcpy( ( void* )ptKdvFontLib->pbyCNData, ( void * )(pbyCurPos + 11), dwChineseSize );
		
		// 读西文字库
		if ( NULL != ptKdvFontLib->pbyENData )
		{
			delete []ptKdvFontLib->pbyENData;
			ptKdvFontLib->pbyENData = NULL;
		}
		ptKdvFontLib->pbyENData = new u8[ dwEngSize ];

		memcpy( ( void* )ptKdvFontLib->pbyENData, 
				( void * )(pbyCurPos + 11 + dwChineseSize), dwEngSize );

		pbyCurPos = pbyCurPos + 11 + dwChineseSize + dwEngSize;  // 移动指针
	}
	
	if ( NULL != pbyDataBuf )
	{
		delete []pbyDataBuf;
		pbyDataBuf = NULL;
	}
	
   	fclose( hRead ); 
	hRead = NULL;
    /*lint -restore*/

	return TRUE;
}

/*=============================================================================
函 数 名：ReleaseFontLib
功    能：释放字库资源
注    意：
算法实现：
全局变量：
输入参数：
输出参数： 
返 回 值：
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
void CKdvGDI::ReleaseFontLib( void )
{
	TKDVFONTLIB *ptKdvFontLib = NULL;

	for ( u8 byLp = 0; byLp < m_byFontLibNum; byLp++ )
	{
		ptKdvFontLib = &m_tKdvFontLib[ byLp ];
		if ( ptKdvFontLib->pbyENData != NULL )
		{
			delete []ptKdvFontLib->pbyENData ;
			ptKdvFontLib->pbyENData = NULL;
		}

		if ( ptKdvFontLib->pbyCNData != NULL )
		{
			delete []ptKdvFontLib->pbyCNData;
			ptKdvFontLib->pbyCNData = NULL;
		}

		memset( ptKdvFontLib, 0, sizeof( TKDVFONTLIB ) );
	}

	m_byFontLibNum = 0;

	return;
}

/*=============================================================================
  函 数 名： CreateBitMapRes
  功    能： 创建图形数据缓冲区
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CKdvGDI::CreateBitMapRes( void )
{
    // 存放图片数据区
    m_pbyBmpDataBuff = new u8[MAX_BMPDATA_BUFFER];
    memset( m_pbyBmpDataBuff, 0, MAX_BMPDATA_BUFFER );
    
    // 存放整个图片文件
    m_pbyBmpFileBuff = new u8[MAX_BMPFILE_SIZE];
	memset(m_pbyBmpFileBuff, 0, sizeof(MAX_BMPFILE_SIZE));
}

/*=============================================================================
  函 数 名： ReleaseBitMap
  功    能： 释放图形数据缓冲区
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CKdvGDI::ReleaseBitMapRes( void )
{
	if( NULL != m_pbyBmpDataBuff )
	{
		delete [] m_pbyBmpDataBuff;
		m_pbyBmpDataBuff = NULL;
	}

	if( NULL != m_pbyBmpFileBuff )
	{
		delete []m_pbyBmpFileBuff;
		m_pbyBmpFileBuff = NULL;
	}
	return ;
}
/*=============================================================================
函 数 名：CreatePaint
功    能：创建画布(整个作图区域)
注    意：
算法实现：
全局变量：
输入参数：wOrignX wOrignY 画布原点坐标
          dwWidth   画布宽度
		  dwHeight  画布长度

输出参数：
返 回 值： 成功返回TRUE 失败返回FALSE
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
BOOL32 CKdvGDI::CreatePaint( u16 wOrignX, u16 wOrignY, 
							   u32 dwWidth, u32 dwHeight,
							   u8 byBmpFileStyle )
{
	TKDVPAINT tKdvPaint = { 0 };
	tKdvPaint.wOrginX = wOrignX;
	tKdvPaint.wOrginY = wOrignY;
	tKdvPaint.dwWidth = dwWidth;
	tKdvPaint.dwHeight = dwHeight;

	return CreatePaint( tKdvPaint, byBmpFileStyle );
}

/*=============================================================================
函 数 名：CreatePaint
功    能：创建画布
注    意：
算法实现：
全局变量：
输入参数：tKdvPaint 画布属性
输出参数：
返 回 值： 成功返回TRUE 失败返回FALSE
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
BOOL32 CKdvGDI::CreatePaint( TKDVPAINT& tKdvPaint, u8 byBmpFileStyle )
{
	memset( &m_tKdvPaint, 0, sizeof( m_tKdvPaint ) ); 
	memcpy( &m_tKdvPaint, &tKdvPaint, sizeof( m_tKdvPaint ) );

	return TRUE;
}

/*=============================================================================
函 数 名：DeletePaint
功    能：删除画布
注    意：
算法实现：
全局变量：
输入参数：tKdvPaint 画布属性
输出参数：
返 回 值： 成功返回TRUE 失败返回FALSE
-------------------------------------------------------------------------------
修改纪录：
日      期  版本  修改人  修改内容
=============================================================================*/
BOOL32 CKdvGDI::DeletePaint( void )
{
	if ( !m_bCreatePaint )
	{
		return FALSE;
	}
	memset( &m_tKdvPaint, 0, sizeof( m_tKdvPaint ) );
	m_bCreatePaint = FALSE;
	return TRUE;
}

// ------------------------------ 新接口 ------------------------------------ //

/*=============================================================================
    函 数 名: IsSupportedPicType
    功    能: 判断是否支持当前画图类型
    注    意: 
    算法实现: 
    全局变量: 
    输入参数: u8 byPicType 图片类型
    输出参数:
    返 回 值: 成功返回TRUE 失败返回FALSE
-------------------------------------------------------------------------------
    修改纪录: 
    日  期      版本        修改人      修改内容
    2012/06/13  4.7         liaokang      创建
=============================================================================*/
BOOL32 CKdvGDI::IsSupportedPicType( IN u8 byPicType )
{
    switch ( byPicType ) 
    {
    case BMP_8:
        paintlog( LOG_LVL_DETAIL, "[DrawBmp]: Cur pic type %d!\n", byPicType ); 
        break;
    case BMP_24:
        paintlog( LOG_LVL_DETAIL, "[DrawBmp]: Cur pic type %d!\n", byPicType ); 
        break;
    default:
        paintlog( LOG_LVL_ERROR, "[DrawBmp]:Do not support cur pic type %d!\n", byPicType );        
        return FALSE;
    }
    return TRUE;
}

/*=============================================================================
    函 数 名: GetMinTextBoxAttrByText
    功    能: 根据文本内容获取最小文本框属性
    注    意: 
    算法实现: 
    全局变量: 
    输入参数: LPCSTR lpszText       文本内容
    输出参数: u32& dwHeight         高度
              u32& dwWidth          宽度
              u8& byLineNum         文本行数
    返 回 值: 成功返回TRUE 失败返回FALSE
-------------------------------------------------------------------------------
    修改纪录: 
    日  期      版本        修改人      修改内容
    2012/06/13  4.7         liaokang      创建
=============================================================================*/
void CKdvGDI::GetMinTextBoxAttrByText( OUT u32& dwHeight, OUT u32& dwWidth, OUT u8& byLineNum, IN LPCSTR lpszText )
{
    u8  byTextLen = strlen( lpszText );
    u8  byLibIndex = 0;
    u8  byCurChar = 0;		// 当前获取的字符
    u32 dwMaxWidth = 0;

    // 获取当前字体的在字库中的索引
    GetFontLibIndex( byLibIndex );
    // 获取当前字体的在字库中的数据
    TKDVFONTLIB *ptFontLib = &m_tKdvFontLib[byLibIndex];

    /* 
        ptFontLib->byWidth * 8 / 2  一个字符的宽度（英文）
        ptFontLib->byWidth * 8      两个字符的宽度（中文）
        ptFontLib->byHeight * 8     一个字符的高度
    */
    if ( byTextLen > 0 )
    {
        // dwHeight应初始为字符高度
        dwHeight = ptFontLib->byHeight * 8;
        byLineNum = 1;
    }
    else
    {
        paintlog( LOG_LVL_ERROR,   "[GetPaintAutoSizeByText]:Text size is zero!\n" ); 
        return;
    }
    
	while( byTextLen > 0 )
	{
        if ( NULL == lpszText )
        {
            byTextLen--;
            continue;
        }

        byCurChar = *( ( u8* ) lpszText++ );

        // 空格
        if ( byCurChar == ' ' )
        {
            dwWidth += ptFontLib->byWidth * 4;  
            byTextLen--;
            continue;
        }
        // 换行符
        if ( byCurChar == '\n' )
        {
            dwHeight += ptFontLib->byHeight * 8;
            byLineNum ++;
            if ( dwMaxWidth < dwWidth )
            {
                dwMaxWidth = dwWidth;
            }            
            dwWidth = 0;
            byTextLen--;
            continue;
        }
        // 其余不可显示字符
        if ( byCurChar < 32 )
        {
            byTextLen--;
            continue;
        }
        // 英文字符
        if ( byCurChar < 161 )
        {
            dwWidth += ptFontLib->byWidth * 4;
            byTextLen--;
            continue;
        }
        //中文字符
        else
        {
            dwWidth += ptFontLib->byWidth * 8;
            lpszText++;
            byTextLen -= 2;
            continue;
        }
    }
    if ( dwMaxWidth > dwWidth )
    {
        dwWidth = dwMaxWidth;
    }
    
    return;
}

/*=============================================================================
    函 数 名: CreatePaint
    功    能: 创建画布
    注    意: 
    算法实现: 
    全局变量: 
    输入参数: TDrawParam& tDrawParam  制图参数
    输出参数:
    返 回 值: 成功返回TRUE 失败返回FALSE
-------------------------------------------------------------------------------
    修改纪录: 
    日  期      版本        修改人      修改内容
    2012/06/13  4.7         liaokang      创建
=============================================================================*/
BOOL32 CKdvGDI::CreatePaint( IN TDrawParam& tDrawParam )
{
    // 判断画布是否已创建
    if ( m_bCreatePaint )
    {
        // 已创建则先删除画布
        DeletePaint();
    }

    u8 byDrawMode = tDrawParam.GetDrawMode();
    if ( AUTO_SIZE_MODE == byDrawMode )
    {
        // 根据文本内容获取自适应画布大小
        u32 dwPaintHeight = 0;
        u32 dwPaintWidth = 0;
        u8  byLineNum = 0;
        GetMinTextBoxAttrByText( dwPaintHeight, dwPaintWidth, byLineNum, tDrawParam.GetText() );
        m_tKdvPaint.wOrginX = 0;
        m_tKdvPaint.wOrginY = 0;
        m_tKdvPaint.dwHeight = dwPaintHeight;
        m_tKdvPaint.dwWidth = dwPaintWidth;
    }
    else
    {
        // 根据传参设置画布大小
        m_tKdvPaint.wOrginX = 0;
        m_tKdvPaint.wOrginY = 0;
        m_tKdvPaint.dwHeight = tDrawParam.GetPicHeight();
        m_tKdvPaint.dwWidth = tDrawParam.GetPicWidth();
    }

    // 判断画布是否符合要求
    if ( m_tKdvPaint.dwHeight <= 0 || m_tKdvPaint.dwWidth <= 0 )
    {
        paintlog( LOG_LVL_ERROR, "[CreatePaint]: Paint's height or width is zero!\n");
        return FALSE;
    }

    // 保证画布宽度为4的整数倍
    m_tKdvPaint.dwWidth = ( m_tKdvPaint.dwWidth % 4 == 0 )
                          ? m_tKdvPaint.dwWidth
                          : ( 4 * ( m_tKdvPaint.dwWidth / 4 + 1 ) );
    m_bCreatePaint = TRUE;
    return TRUE;
}

/*=============================================================================
    函 数 名: IsVaildPicSize
    功    能: 判断图片大小是否超过容许范围
    注    意: 
    算法实现: 
    全局变量: 
    输入参数: u8 byPicType          当前图片类型
    输出参数:
    返 回 值: 成功返回TRUE 失败返回FALSE
-------------------------------------------------------------------------------
    修改纪录: 
    日  期      版本        修改人      修改内容
    2012/06/13  4.7         liaokang      创建
=============================================================================*/
BOOL32 CKdvGDI::IsVaildPicSize()
{
    u32 dwPaintSize = m_tKdvPaint.dwWidth * m_tKdvPaint.dwHeight;
    u32 dwPicDataSize = 0;
    u32 dwPicSize = 0;
    switch ( m_byPicType ) 
    {
    case BMP_8:
        dwPicDataSize = dwPaintSize;
        dwPicSize = ( u32 )( sizeof( TBITMAPFILEHEADER )
            + sizeof( TBITMAPINFOHEADER )
            + PAINTCOLOR * sizeof( TPALETTEENTRY )
            + dwPicDataSize );
        break;
    case BMP_24:
        dwPicDataSize = dwPaintSize * 3;
        dwPicSize = ( u32 )( sizeof( TBITMAPFILEHEADER )
            + sizeof( TBITMAPINFOHEADER )
            + dwPicDataSize );
        break;
    }
    // 判断数据区是否偏大
    if ( dwPicDataSize > MAX_BMPDATA_BUFFER )
    {
        paintlog( LOG_LVL_ERROR, "[DrawBmp]: Pic data size is too big!\n");
        return FALSE;
	} 
    // 判断图片是否偏大
    if ( dwPicSize > MAX_BMPFILE_SIZE )
    {
        paintlog( LOG_LVL_ERROR, "[DrawBmp]: Pic file size is too big!\n");
        return FALSE;
	}
    return TRUE;
}

/*=============================================================================
    函 数 名: PaintBkColor
    功    能: 画布区域着色（背景色）
    注    意: 
    算法实现: 
    全局变量: 
    输入参数: u32 dwBkColor   背景色
    输出参数: 
    返 回 值: 成功返回TRUE 失败返回FALSE
-------------------------------------------------------------------------------
    修改纪录: 
    日  期      版本        修改人      修改内容
    2012/06/13  4.7         liaokang      创建
=============================================================================*/
BOOL32 CKdvGDI::PaintBkColor(IN u32 dwBkColor )
{
    //////////////////////////////////////////////////////////////////////////
    // 根据不同类型图片着色 后续修改

    u32 dwPaintSize = m_tKdvPaint.dwWidth * m_tKdvPaint.dwHeight;
    u8 byClrIndex = GetColorIndex( dwBkColor );
	memset( m_pbyBmpDataBuff, byClrIndex, dwPaintSize );  // 设置背景色
    return TRUE;
}

/*=============================================================================
    函 数 名: TextOutInPaint
    功    能: 输出文本到画布
    注    意: 
    算法实现: 
    全局变量: 
    输入参数: LPCSTR lpszText    文本
              u8 byTextLen       文本长度
              u8 byTextAlignment 文本排列方式
    输出参数: 
    返 回 值: 成功返回TRUE 失败返回FALSE
-------------------------------------------------------------------------------
    修改纪录: 
    日  期      版本        修改人      修改内容
    2012/06/13  4.7         liaokang      创建
=============================================================================*/
BOOL32 CKdvGDI::TextOutInPaint( IN LPCSTR lpszText,IN u8 byTextLen, IN u8 byTextAlignment )
{
    s8  achText[DEFAULT_TEXT_LEN+1];        // 文本内容
    memset(achText, 0, sizeof(achText));
    s8  achLineText[DEFAULT_TEXT_LEN+1];    // 每行文本内容
    memset(achLineText, 0, sizeof(achLineText));
    s8 *pchText = achText;                  // 文本内容指针
    u8  LastCharTotalNum = 0;               // 上一次已记录的字符总数
    u8  CurCharTotalNum = 0;                // 当前已记录的字符总数
    u8  byPrintTextLen = 0;                 // 当前可打印的字符总数
    u8  byLibIndex = 0;                     // 字库索引
    u8  byCurChar = 0;		                // 当前获取的字符
    s32 nStartPosX = 0;                     // 文本输出至画布的起始位置X
    s32 nStartPosY = 0;                     // 文本输出至画布的起始位置Y

    // 记录文本内容
    memcpy( achText, lpszText, byTextLen ); 

    // 对文本长度判断
    if (  0 == byTextLen )
    {
        paintlog( LOG_LVL_ERROR,   "[TextOutInPaint]: Text size is zero!\n" ); 
        return FALSE;
    } 

    // 根据文本内容获取最小的文本框大小
    u32 dwMinTextBoxHeight = 0;
    u32 dwMinTextBoxWidth = 0;
    u8  byLineNum = 1;
    GetMinTextBoxAttrByText( dwMinTextBoxHeight, dwMinTextBoxWidth, byLineNum, achText );
    if( dwMinTextBoxHeight > m_tKdvPaint.dwHeight || dwMinTextBoxWidth > m_tKdvPaint.dwWidth )
    {
        paintlog( LOG_LVL_ERROR, "[TextOutInPaint]: The paint can't accommodate %d characters !\n", byTextLen ); 
        return FALSE;
    }
    
    // 获取当前字体的在字库中的索引
    GetFontLibIndex( byLibIndex );
    // 获取当前字体的在字库中的数据
    TKDVFONTLIB *ptFontLib = &m_tKdvFontLib[byLibIndex];

    // 根据垂直方向文本排列方式得出垂直方向起始位置
    switch ( byTextAlignment & 0x0F )
    {
        case VERTICAL_TOP:
            //垂直方向靠上
            nStartPosY = 0;
            break;
        case VERTICAL_CENTER:
            // 垂直方向居中
            nStartPosY = ( m_tKdvPaint.dwHeight - ptFontLib->byFontSize * byLineNum ) / 2;
            break;
        case VERTICAL_BOTTOM:
            // 垂直方向靠下
            nStartPosY = m_tKdvPaint.dwHeight - ptFontLib->byFontSize * byLineNum;
            break;
        default:
            paintlog( LOG_LVL_ERROR, "[TextOutInPaint]: Text vertical alignment param is wrong!\n"); 
            return FALSE; 
    } 

    while( byTextLen > 0 )
    {
        if ( NULL == pchText )
        {
            paintlog( LOG_LVL_ERROR, "[TextOutInPaint]: Text vertical alignment param is wrong!\n"); 
            return FALSE; 
        }
        
        byCurChar = *( ( u8* ) pchText++ );
        
        // 空格
        if ( byCurChar == ' ' )
        {
            byTextLen--;
            CurCharTotalNum++;
            byPrintTextLen++;
            continue;
        }
        // 换行符
        if ( byCurChar == '\n' )
        { 
            // 根据水平方向文本排列方式得出水平方向起始位置
            switch ( byTextAlignment & 0xF0 )
            {
                case LEVEL_LEFT:
                    // 水平方向左对齐
                    nStartPosX = 0;
                    break;
                case LEVEL_CENTER:
                    // 水平方向居中
                    nStartPosX = ( m_tKdvPaint.dwWidth - byPrintTextLen * ptFontLib->byFontSize / 2 ) / 2 ; // 第一个2表示英文字符，所以减半
                    break;
                case LEVEL_RIGHT:
                    // 水平方向右对齐
                    nStartPosX = m_tKdvPaint.dwWidth - ( byPrintTextLen * ptFontLib->byFontSize / 2);
                    break;
                default:
                    paintlog( LOG_LVL_ERROR, "[TextOutInPaint]: Text level alignment param is wrong!\n"); 
                    return FALSE; 
            }
            byTextLen--;
            CurCharTotalNum++;
            memcpy(achLineText, achText + LastCharTotalNum, CurCharTotalNum - LastCharTotalNum );
            TextOut( nStartPosX, nStartPosY, achLineText, CurCharTotalNum - LastCharTotalNum );
            memset(achLineText, 0, sizeof(achLineText));
            nStartPosY += ptFontLib->byFontSize;
            LastCharTotalNum = CurCharTotalNum;
            byPrintTextLen = 0;
            continue;
        }
        // 其余不可显示字符
        if ( byCurChar < 32 )
        {
            byTextLen--;
            CurCharTotalNum++;
            continue;
        }
        // 英文字符
        if ( byCurChar < 161 )
        {
            byTextLen--;
            CurCharTotalNum++;
            byPrintTextLen++;
            continue;
        }
        //中文字符
        else
        {
            pchText++;
            byTextLen -= 2;
            CurCharTotalNum += 2;
            byPrintTextLen += 2;
            continue;
        }
    }

    if ( 0 != byPrintTextLen )
    {
        // 根据水平方向文本排列方式得出水平方向起始位置
        switch ( byTextAlignment & 0xF0 )
        {
            case LEVEL_LEFT:
                // 水平方向左对齐
                nStartPosX = 0;
                break;
            case LEVEL_CENTER:
                // 水平方向居中
                nStartPosX = ( m_tKdvPaint.dwWidth - byPrintTextLen * ptFontLib->byFontSize / 2 ) / 2 ; // 第一个2表示英文字符，所以减半
                break;
            case LEVEL_RIGHT:
                // 水平方向右对齐
                nStartPosX = m_tKdvPaint.dwWidth - ( byPrintTextLen * ptFontLib->byFontSize / 2);
                break;
            default:
                paintlog( LOG_LVL_ERROR, "[TextOutInPaint]: Text level alignment param is wrong!\n"); 
                return FALSE; 
        }
        memcpy(achLineText, achText + LastCharTotalNum, CurCharTotalNum - LastCharTotalNum );
        TextOut( nStartPosX, nStartPosY, achLineText, CurCharTotalNum - LastCharTotalNum );
    }

    return TRUE;
}


/*=============================================================================
    函 数 名: GetPic
    功    能: 获取生成的PIC数据
    注    意: 
    算法实现: 
    全局变量: 
    输入参数: u8 byPicType  Pic类型
    输出参数: TPic& tPic    Pic
    返 回 值: 成功返回TRUE 失败返回FALSE
-------------------------------------------------------------------------------
    修改纪录: 
    日  期      版本        修改人      修改内容
    2012/06/13  4.7         liaokang      创建
=============================================================================*/
BOOL32 CKdvGDI::GetPic( OUT TPic& tPic )
{
	if ( NULL == &tPic )
	{
        paintlog( LOG_LVL_ERROR, "[GetPic]: Invalid input param!\n");
        return FALSE;
	}

    if ( NULL == m_pbyBmpDataBuff )
    {
        paintlog( LOG_LVL_ERROR, "[GetPic]: Bmpdata point is null!\n");
        return FALSE;
    }
    
    if ( NULL == m_pbyBmpFileBuff )
    {
        // 存放整个图片文件
        m_pbyBmpFileBuff = new u8[MAX_BMPFILE_SIZE];
        memset(m_pbyBmpFileBuff, 0, sizeof(MAX_BMPFILE_SIZE));
    }

    u32 dwPaintSize = m_tKdvPaint.dwWidth * m_tKdvPaint.dwHeight;
    u32 dwPicSize = 0;
    u32 dwOffBits = 0;
    switch ( m_byPicType ) 
    {
    case BMP_8:
        {
            dwPicSize = ( u32 )( sizeof( TBITMAPFILEHEADER )
                + sizeof( TBITMAPINFOHEADER )
                + PAINTCOLOR * sizeof( TPALETTEENTRY )
                + dwPaintSize );
            dwOffBits = ( u32 ) ( sizeof( TBITMAPFILEHEADER )
                + sizeof( TBITMAPINFOHEADER )
                + PAINTCOLOR * sizeof( TPALETTEENTRY ) );
            
            // 调色板( BMP图片中,RGB存放次序:BGR )
            TPALETTEENTRY tPalEntry[ PAINTCOLOR ] = { 0 };
            for ( u16 wLp = 0; wLp < PAINTCOLOR; wLp++ )
            {
                tPalEntry[ wLp ].byRed = m_tPaletteEntry[ wLp ].byBlue;
                tPalEntry[ wLp ].byGreen = m_tPaletteEntry[ wLp ].byGreen;
                tPalEntry[ wLp ].byBlue = m_tPaletteEntry[ wLp ].byRed;
                tPalEntry[ wLp ].byFlags = m_tPaletteEntry[ wLp ].byFlags;
            }
            memcpy( m_pbyBmpFileBuff + sizeof( TBITMAPFILEHEADER ) + sizeof( TBITMAPINFOHEADER ),
                tPalEntry, sizeof( tPalEntry ) );
        }
        break;
    case BMP_24:
        {
            dwPicSize = ( u32 )( sizeof( TBITMAPFILEHEADER )
                + sizeof( TBITMAPINFOHEADER )
                + dwPaintSize * 3);
            dwOffBits = ( u32 ) ( sizeof( TBITMAPFILEHEADER )
            + sizeof( TBITMAPINFOHEADER ) );
        }
        break;
    }

	// 文件头
	TBITMAPFILEHEADER tBitMapFileHeader;
    memset( &tBitMapFileHeader, 0, sizeof( tBitMapFileHeader ) );
	tBitMapFileHeader.wType = (u16)0x4D42;  // 文件头中类型值固定为"BM", 注意高低字节
	tBitMapFileHeader.dwSize = dwPicSize;
    tBitMapFileHeader.wReserved1 = 0;
    tBitMapFileHeader.wReserved2 = 0;
	tBitMapFileHeader.dwOffBits = dwOffBits;
	memcpy( m_pbyBmpFileBuff, &tBitMapFileHeader, sizeof( tBitMapFileHeader ) );

	// 信息头
	TBITMAPINFOHEADER tBitMapInfoHeader;
    memset( &tBitMapInfoHeader, 0, sizeof( tBitMapInfoHeader ) );
    tBitMapInfoHeader.dwSize = sizeof( TBITMAPINFOHEADER );
    tBitMapInfoHeader.dwWidth = m_tKdvPaint.dwWidth;
    tBitMapInfoHeader.dwHeight = m_tKdvPaint.dwHeight;
    tBitMapInfoHeader.wPlanes = 1;
    tBitMapInfoHeader.wBitCount = (m_byPicType == BMP_8)?8:24;
    tBitMapInfoHeader.dwCompression = 0L;
    tBitMapInfoHeader.dwSizeImage = (m_byPicType == BMP_8)?dwPaintSize:3*dwPaintSize;
    tBitMapInfoHeader.dwXPelsPerMeter = 0;
    tBitMapInfoHeader.dwYPelsPerMeter = 0;
    tBitMapInfoHeader.dwClrUsed = 0;
    tBitMapInfoHeader.dwClrImportant = 0;
	memcpy( m_pbyBmpFileBuff + sizeof( TBITMAPFILEHEADER ), &tBitMapInfoHeader, sizeof( tBitMapInfoHeader ) );
	
	// 数据区
	memcpy( m_pbyBmpFileBuff + tBitMapFileHeader.dwOffBits, m_pbyBmpDataBuff, tBitMapInfoHeader.dwSizeImage );

	// 返回数据
    tPic.SetPicNull();
    tPic.SetPicType( m_byPicType );
    tPic.SetPicHeight( tBitMapInfoHeader.dwHeight );
    tPic.SetPicWidth( tBitMapInfoHeader.dwWidth );
    tPic.SetPicSize( tBitMapFileHeader.dwSize );
    tPic.SetPic( (s8 *)m_pbyBmpFileBuff );

	return TRUE;
}

/*=============================================================================
    函 数 名: DrawBmp
    功    能: 制Bmp图
    算法实现:  
    全局变量:  
    参    数: OUT TPic& tPic 图片
              IN TDrawParam& tDrawParam 制图参数
    返 回 值: 成功返回SUCCESS_DRAW 失败返回FAIL_DRAW
-------------------------------------------------------------------------------
    修改纪录: 
    日  期      版本        修改人      修改内容
    2012/06/13  4.7         liaokang      创建
=============================================================================*/
BOOL32 CKdvGDI::DrawBmp( OUT TPic& tPic, IN TDrawParam& tDrawParam )
{
    // 置空
    // 防止前面调用接口成功，后面调用接口不成功但没有判断时反复生成同一图片
    tPic.SetPicNull();

    // 防止没有初始化
    if ( FALSE == m_bInitGDI )
    {
        paintlog( LOG_LVL_ERROR, "[DrawBmp]:Initialization is not completed, please initialize!\n");
        return FAIL_DRAW;
	}
    
    BOOL32 bRet = FALSE;
    // 记录当前图片类型
    m_byPicType = tDrawParam.GetPicType();
    // 判断是否支持当前画图类型
    bRet = IsSupportedPicType( m_byPicType );
    if ( !bRet )
    {
        paintlog( LOG_LVL_ERROR,   "[DrawBmp]:Do not support cur pic type %d!\n", m_byPicType );        
        return FAIL_DRAW;
    } 

    // 记录当前字体
    TKDVFONT tFont;
    tFont.byType = tDrawParam.GetFontType();
    tFont.bySize = tDrawParam.GetFontSize();
	SetCurrentFont( tFont );

    // 创建画布
    bRet = CreatePaint( tDrawParam );
    if ( !bRet )
    {
        paintlog( LOG_LVL_ERROR, "[DrawBmp]: Fail to creat paint!\n" );        
        return FAIL_DRAW;
    }
    bRet = IsVaildPicSize();
    if ( !bRet )
    {
        paintlog( LOG_LVL_ERROR, "[DrawBmp]: Pic size is invalid!\n" );        
        return FAIL_DRAW;
    }

    // 画布区域着色（背景色）
    PaintBkColor( tDrawParam.GetDrawBkColor() );
    // 文本颜色
	SetTextColor( tDrawParam.GetTextColor() );

    // 输出字符串
    s8 achText[DEFAULT_TEXT_LEN+1];
    memset(achText, 0, sizeof(achText));
 	memcpy(achText, tDrawParam.GetText(), sizeof(achText));
    bRet = TextOutInPaint( achText, strlen(achText), tDrawParam.GetTextAlignment() );
    if ( !bRet )
    {
        paintlog( LOG_LVL_ERROR, "[DrawBmp]:Fail to text out in paint!\n" );        
        return FAIL_DRAW;
    }

	// 得到数据区
 	bRet = GetPic( tPic );
    if ( !bRet )
    {
        paintlog( LOG_LVL_ERROR, "[DrawBmp]:Fail to get pic!\n" );        
        return FAIL_DRAW;
    }
	
	// 删除画布
	bRet = DeletePaint();
    if ( !bRet )
    {
        paintlog( LOG_LVL_ERROR, "[DrawBmp]:Fail to delete paint!\n" );        
        return FAIL_DRAW;
    }

	return SUCCESS_DRAW;
}

/*=============================================================================
函 数 名： paintlog
功    能： 打印函数
算法实现： 
全局变量： 
参    数： u8 byLogLvl
s8 * fmt
...
返 回 值： void 
=============================================================================*/
/*lint -save -e530 */
/*lint -save -esym(526, __builtin_va_start)*/
/*lint -save -esym(628, __builtin_va_start)*/
void paintlog( u8 byLevel, s8* pszFmt, ... )
{
    s8 achPrintBuf[1024]= {0};
    va_list argptr;
    u16 wBufLen = sprintf(achPrintBuf, "[drawbmp]");
    va_start( argptr, pszFmt );
    vsprintf(achPrintBuf + wBufLen, pszFmt, argptr );
    va_end(argptr);
    LogPrint( byLevel, MID_VMPEQP_DRAWBMP, achPrintBuf );
    return;
}
/*lint -restore*/