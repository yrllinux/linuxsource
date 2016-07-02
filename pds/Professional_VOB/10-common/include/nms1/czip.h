// czip.h: interface for the CGZip class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GZIP_H__40AD6D0E_0E52_4E5A_8727_0AFA8CFC98D4__INCLUDED_)
#define AFX_GZIP_H__40AD6D0E_0E52_4E5A_8727_0AFA8CFC98D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "kdvtype.h"
#include <WTYPES.H>

class CGZip
{
public:
	CGZip();
	virtual ~CGZip();

public:
    /*=============================================================================
    函 数 名: Zip
    功    能: 压缩
    参    数: LPCSTR lpszSrcFile        [in]    待压缩文件
              LPCSTR lpszDstFile        [in]    压缩结果文件
    注    意: 无
    返 回 值: 压缩成功: TRUE; 压缩失败: FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2007/06/29  1.0     王昊    创建
    =============================================================================*/
    BOOL32 Zip(LPCSTR lpszSrcFile, LPCSTR lpszDstFile);

    /*=============================================================================
    函 数 名: Unzip
    功    能: 解压
    参    数: LPCSTR lpszSrcFile        [in]    待解压文件
              LPCSTR lpszDstFile        [in]    解压结果文件
    注    意: 无
    返 回 值: 解压成功: TRUE; 解压失败: FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2007/06/29  1.0     王昊    创建
    =============================================================================*/
    BOOL32 Unzip(LPCSTR lpszSrcFile, LPCSTR lpszDstFile);

private:
};

#endif // !defined(AFX_GZIP_H__40AD6D0E_0E52_4E5A_8727_0AFA8CFC98D4__INCLUDED_)
