// NmsLog.h: interface for the CNmsLog class.
//
//////////////////////////////////////////////////////////////////////

#ifndef SM_SMLOG_H
#define SM_SMLOG_H

#include "smmacro.h"

//日志类

class CSmLog  
{
public:
	CSmLog(const s8* pchProcName, u8 byLogLevel = APPLICATION);
	CSmLog(const s8* pchInsName, const s8* pchProcName, u8 byLogLevel = APPLICATION);
	virtual ~CSmLog();

	void Log(const s8* pchFormat, ...);
	void Log(BYTE byLevel, const s8* pchFormat, ...);

	void SetDefaultLogLevel(u8 byLogLevel);
protected:
	//函数名
	s8 m_achProcName[SM_MAX_INS_NAME_LEN];
	//类名
	s8 m_achClsName[SM_MAX_PROC_NAME_LEN];

	u8 m_byLogLevel;
	static u8 m_byStaticInd;
	static u8 m_byStaticFlag;
	u8 m_byInd;
	u8 m_byFlag;
private:	
	void Begin();
	void End();
	
	void AddInd();
	void SubInd();
	
	void Log(BYTE byLevel, const s8 *szFormat, va_list argptr);
	CSmLog();
};

#endif //SM_SMLOG_H