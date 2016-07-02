#ifndef _COMMONCLASS_H_
#define _COMMONCLASS_H_

#include "osp.h"
#include "mcuconst.h"

// class CSemOpt{
// public:
//     CSemOpt( SEMHANDLE &hSem, u32 dwTime = WAIT_SEM_TIMEOUT )
//     {
//         m_hSem = hSem;
//         if( OspSemTakeByTime( m_hSem, dwTime ) != TRUE )
//         {
//             OspPrintf( TRUE, FALSE, "semTake error accord to handle<0x%x>\n", hSem );
//         }
//     }
//     ~CSemOpt( )
//     {
//         OspSemGive( m_hSem );
//     }
// private:
//     SEMHANDLE m_hSem;
// };

// 日志记录类
#define MAXNUM_PREFIXLEN        (u8)8
#define MAXNUM_LOGRECORD        (u8)50
#define MAXNUM_LOGRECLEN        (u16)512
#define MAXNUM_LOGFILESZIE      (u32)(1024*1024)
class TLogUnit 
{
public:
    TLogUnit()
    {
        memset( m_achLogString, '\0', sizeof(m_achLogString) );
    }
public:
    TKdvTime m_tTime;
    s8  m_achLogString[MAXNUM_LOGRECLEN+1];   // 事件描述	
};


class CLogManager
{
private:
    SEMHANDLE m_hSem;

    s8 m_achLogCurFileName[KDV_MAX_PATH];
    s8 m_achLogLastFileName[KDV_MAX_PATH];
    s8 m_achLogPrefix[MAXNUM_PREFIXLEN+1];
    s8 m_achDirPath[KDV_MAX_PATH+1];        // no last '/' or '\'
    s8 m_achFileTitle[KDV_MAX_PATH+1];      // no file extention
    LinkArray<TLogUnit, MAXNUM_LOGRECORD> m_ltLogTable;
    
public:
    CLogManager()
    {
        memset( m_achLogPrefix, '\0', sizeof(m_achLogPrefix) );
        memset( m_achFileTitle, '\0', sizeof(m_achFileTitle) );
        memset( m_achDirPath, '\0', sizeof(m_achDirPath) );
        memset( m_achLogCurFileName, '\0', sizeof(m_achLogCurFileName) );
        memset( m_achLogLastFileName, '\0', sizeof(m_achLogLastFileName) );
           
        m_ltLogTable.Clear();
        
        if( !OspSemBCreate(&m_hSem) )
        {
            OspSemDelete( m_hSem );
        }
    }
    ~CLogManager()
    {
        OspSemDelete( m_hSem );
    }

    void SetLogPrefix( const s8 * pszLogPrefix );
    void SetDirPath( const s8 * pszDirPath );
    void SetFileTitle( const s8 * pszFileTitle );
	void LogInit( const s8 * pszLogPrefix, const s8 * pszDirPath, const s8 * pszFileTitle );

    void ClearLogTable(void);
    void InsertLogItem( s8 * pszFmt, ... );
    void WriteLogInfo(void);
	void FlashLog(void);
	
};

#endif
