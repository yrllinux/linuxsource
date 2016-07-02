// parsecfgfile.h: interface for the CParseCfgfile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARSECFGFILE_H__E76898E3_F11F_49D1_A009_08D7A1E5A4CB__INCLUDED_)
#define AFX_PARSECFGFILE_H__E76898E3_F11F_49D1_A009_08D7A1E5A4CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <string>
using namespace std;

typedef struct tagCfgFileData 
{
    string m_strSec, m_strKey, m_strVal;

    tagCfgFileData( string strSec=(""), string strKey=(""), string strVal=("") ):m_strSec(strSec),
        m_strKey(strKey), m_strVal(strVal){}

}TCfgFileData, *PTCfgFileData;

/************************************************************************/
/* 功能：实现内存数据类似配置文件读写的操作                                                                     */
/************************************************************************/
class CParseCfgfile  
{
public:
	bool WriteProfileInt(const char* lpAppName, const char* lpKeyName, int nVal );
	bool WriteProfileString(const char* lpAppName, const char* lpKeyName, const char* lpString );
	int GetProfileInt(const char* lpAppName, const char* lpKeyName, int nDefault );
	int GetProfileString(const char* lpAppName, const char* lpKeyName, const char* lpDefault, char* lpReturnedString,unsigned int nSize);
	void ReadData( const char* pData );
    void Clear(){ m_vctData.clear(); m_strData=(""); }

	CParseCfgfile();
	virtual ~CParseCfgfile();

public:
    vector<TCfgFileData> m_vctData;
    string m_strData;

};

#endif // !defined(AFX_PARSECFGFILE_H__E76898E3_F11F_49D1_A009_08D7A1E5A4CB__INCLUDED_)
