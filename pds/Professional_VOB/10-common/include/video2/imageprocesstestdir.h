/******************************************************************************
模块名	    ：imageprocesstestdir                       
文件名	    ：imageprocesstestdir.h
相关文件	：
文件实现功能：拼接文件路径
作者		：ZouWenyi
版本		：1.0
-------------------------------------------------------------------------------
修改记录:
日  期		版本		修改人		走读人    修改内容
2005/05/10  1.0        ZouWenyi                 创建
******************************************************************************/
//图像序列进行测试的帧数
#define TESTNUM 1

#ifndef _IMAGEPROCESSTESTDIR_H_
#define _IMAGEPROCESSTESTDIR_H_

static s8 * ImageProcessStrCat( const s8 *ps8FileName )
{
	static const s8  *ps8Resource = "e:\\testvideo\\";
	static s8 ps8String[1024];
	strcpy( ps8String, ps8Resource );
	return strcat( ps8String, ps8FileName );
}

static s8 * ProcessResultStrCat( const s8 *ps8FileName )
{
#if  defined(OPTIMIZE_FOR_EQUATOR) 
    static const s8  *ps8ResourceResult = "e:\\testvideo\\map_result\\";
#elif defined(OPTIMIZE_FOR_MMX)
    static const s8  *ps8ResourceResult = "e:\\testvideo\\mmx_result\\";
#elif defined(OPTIMIZE_FOR_BLACKFIN)
    static const s8  *ps8ResourceResult = "e:\\testvideo\\blackfin_result\\";
#else 
    static const s8  *ps8ResourceResult = "e:\\testvideo\\c_result\\";
#endif	
	static s8 ps8StringResult[1024];
	strcpy( ps8StringResult, ps8ResourceResult );
    strcat( ps8StringResult, ps8FileName );
	return ps8StringResult;
}

#endif // _IMAGEPROCESSTESTDIR_H_
