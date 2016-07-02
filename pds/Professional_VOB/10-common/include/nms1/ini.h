// Ini.h: interface for the CIni class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include <string>
using namespace std;

#include "unicode.h"

enum{
	LOCAL_FORMAT,
	UTF8_FORMAT,
	UTF16_BE_FORMAT,
	UTF16_LE_FORMAT,
	UNKNOWN_FILE,
};

class CIni  
{
public:
	CIni( LPCTSTR pFileName = 0, int nBufLen = 256 );
	CIni( const CIni &cIni );
	virtual ~CIni();

	CIni& operator=(const CIni &cIni);

	bool Exist();
	bool Clear();
	bool GetBool ( LPCTSTR lpAppName, LPCTSTR lpKeyName, bool bDefault = false );
	unsigned int GetInt( LPCTSTR lpAppName, LPCTSTR lpKeyName, unsigned int nDefault = 0 );
	unsigned long GetString( LPCTSTR lpAppName, LPCTSTR lpKeyName, tstring& strValue, LPCTSTR lpDefault = _T("") );
	unsigned long GetString( LPCTSTR lpAppName, LPCTSTR lpKeyName, LPTSTR lpValue, int nSize, LPCTSTR lpDefault = _T("") );
	bool SetBool( LPCTSTR lpAppName, LPCTSTR lpKeyName, bool bValue);
	bool SetInt( LPCTSTR lpAppName, LPCTSTR lpKeyName, int nValue);
	bool SetString( LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpValue );
	bool SetString( LPCTSTR lpAppName, LPCTSTR lpKeyName, const tstring& strValue);
	void SetFilePath( LPCTSTR pFileName );
	bool SetSection( LPCTSTR lpAppName, LPCTSTR lpString );
	void CheckFileCodeHeadInfo();

protected:
	int m_nBufLen;
	tstring m_strFileName;
	LPTSTR m_pcBuf;
	bool ReEncode();
	unsigned char GetCodeType();

};
