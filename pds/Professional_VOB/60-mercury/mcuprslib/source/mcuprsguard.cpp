/*****************************************************************************
模块名      : mcuprs
文件名      : mcuprsguard.cpp
相关文件    : mcuprsguard.h
文件实现功能: mcuprs守卫应用类定义
作者        : 周广程
版本        : V4.0  Copyright(C) 2001-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/04/07  4.0         周广程        创建
******************************************************************************/
#include "mcuprsguard.h"
//#include "boardagent.h"
//#include "mcuver.h"

CMcuPrsGuardApp g_cMcuPrsGuardApp;
s32  g_nprsguardlog = 0;

BOOL32 CreatPrsDir(const s8* lpszDirPath);
/*=============================================================================
  函 数 名： prsgrdlog
  功    能： 打印
  算法实现： 
  全局变量： 
  参    数： char* fmt
             ...
  返 回 值： void 
=============================================================================*/
/*lint -save -e438 -e530 -e1055*/
void prsgrdlog( s8* pszFmt, ...)
{
    s8 achPrintBuf[255];
    s32  nBufLen = 0;
    va_list argptr;
    if( g_nprsguardlog == 1 )
    {		  
        nBufLen = sprintf( achPrintBuf, "[PrsGuard]: " ); 
        va_start( argptr, pszFmt );
        nBufLen += vsprintf( achPrintBuf + nBufLen, pszFmt, argptr ); 
        va_end(argptr); 
        OspPrintf( TRUE, FALSE, achPrintBuf ); 
    }
}
/*lint -restore*/

/*=============================================================================
函 数 名： InstanceEntry
功    能： 消息入口
算法实现： 
全局变量： 
参    数： CMessage *const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/4/7   4.0		周广程                  创建
=============================================================================*/
void CMcuPrsGuard::InstanceEntry( CMessage *const pcMsg )
{	
	if (pcMsg == NULL)
	{
		OspPrintf( TRUE, FALSE ,"[MCU Prs Guard Recv] NULL message received.\n");
		return ;
	}
	
    prsgrdlog( "Message %u(%s).\n", pcMsg->event ,::OspEventDesc( pcMsg->event ));
	
	switch (pcMsg->event)
    {
	case OSP_POWERON:
		ProcGuardPowerOn(pcMsg);
		break;
	case EV_PRSGUARD_MAINTASKSCAN_TIMER:
		ProcMainTaskScanTimeOut(pcMsg);
		break;
	case EV_MAINTASKSCAN_ACK:
		ProcMainTaskScanAck(pcMsg);
		break;
		// 未定义的消息
	default:
		OspPrintf( TRUE, FALSE, "[PrsGuard]Unexcpet Message %u(%s).\n", pcMsg->event, ::OspEventDesc( pcMsg->event ));
		break;
	}
}

/*=============================================================================
函 数 名： ProcGuardPowerOn
功    能： 启动消息响应函数
算法实现： 
全局变量： 
参    数： CMessage *const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/4/7   4.0		周广程                  创建
=============================================================================*/
void CMcuPrsGuard::ProcGuardPowerOn( CMessage *const pcMsg )
{
	switch( CurState() )
	{
	case STATE_IDLE:
	{
		NEXTSTATE( STATE_NORMAL );

		post( MAKEIID(AID_PRS, 1), EV_MAINTASKSCAN_REQ );
		g_cMcuPrsGuardApp.SetIsRecvScanAck(FALSE);
		SetTimer( EV_PRSGUARD_MAINTASKSCAN_TIMER, PRSGUARD_MAINTASKSCAN_TIMEOUT );

		break;
	}
	default:
		OspPrintf( TRUE, FALSE, "Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

	return;
}

/*=============================================================================
函 数 名： ProcMainTaskScanTimeOut
功    能： 探测主线程定时器超时处理函数
算法实现： 
全局变量： 
参    数： CMessage *const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/4/7   4.0		周广程                  创建
=============================================================================*/
void CMcuPrsGuard::ProcMainTaskScanTimeOut( CMessage *const pcMsg )
{
	SetTimer( EV_PRSGUARD_MAINTASKSCAN_TIMER, PRSGUARD_MAINTASKSCAN_TIMEOUT );
	post( MAKEIID(AID_PRS, 1), EV_MAINTASKSCAN_REQ );

	switch( CurState() )
	{
	case STATE_NORMAL:
	{
		if ( g_cMcuPrsGuardApp.IsRecvScanAck() )
		{		
			g_cMcuPrsGuardApp.SetIsRecvScanAck(FALSE);			
			post( MAKEIID(AID_PRS, 1), EV_MAINTASKSCAN_REQ );		
		}
		else
		{
			u8 byNoScanAckTimes = g_cMcuPrsGuardApp.GetNoScanAckTimes();
			if ( byNoScanAckTimes <= MAXNUM_MAINTASKSCAN_TIMEOUT )
			{
				g_cMcuPrsGuardApp.AddNoScanAckTimes();
				return;
			}			
			
			OspPrintf( TRUE, FALSE, "[PrsGuard] Main task dead or blocked!\n" );
			
			// 记录到文件
			s8 asPrsLogFile[256] = {0};
			sprintf( asPrsLogFile, "%s/%s", DIR_LOG, FILE_PRSLOG );
			FILE *hLogFile = fopen(asPrsLogFile, "r+b");
			
			if ( NULL == hLogFile )
			{
				// 创建log文件
				prsgrdlog("prs.log not exist and create it\n");
				
				BOOL32 bRet = CreatPrsDir( DIR_LOG );
				if ( !bRet )
				{
					prsgrdlog("create dir<%s> failed for %s\n", DIR_LOG);     
					return;
				}
				hLogFile = fopen(asPrsLogFile, "w+b");
				if (NULL == hLogFile)
				{
					prsgrdlog("create prs.log failed for %s\n", strerror(errno));     
					return;
				}
			}
			
			// 写log文件
			s8 achInfo[255] = {0};
			time_t tiCurTime = ::time(NULL);             
			s32 nLen = sprintf(achInfo, "\nSytem time %s\n[PrsGuard] Main task dead or blocked!\n", ctime(&tiCurTime));
			
			fseek(hLogFile, 0, SEEK_END);
			nLen = fwrite(achInfo, nLen, 1, hLogFile);
			fclose(hLogFile);
			if (0 == nLen)
			{
				prsgrdlog("write to prs.log failed\n");
				return;
			}			
		}
		break;
	}
	default:
		OspPrintf( TRUE, FALSE, "Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	
	return;
}

/*=============================================================================
函 数 名： ProcMainTaskScanAck
功    能： 回应处理函数
算法实现： 
全局变量： 
参    数： CMessage *const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/4/7   4.0			周广程                  创建
=============================================================================*/
void CMcuPrsGuard::ProcMainTaskScanAck( CMessage *const pcMsg )
{
	switch( CurState() )
	{
	case STATE_NORMAL:
	{
		g_cMcuPrsGuardApp.ResetNoScanAckTimes();
		g_cMcuPrsGuardApp.SetIsRecvScanAck( TRUE );
		break;
	}
	default:
		OspPrintf( TRUE, FALSE, "Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

//---------------------CMcuPrsGuardData-------------------------//

/*=============================================================================
函 数 名： CMcuPrsGuardData
功    能： 构造函数
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/4/7   4.0		周广程                  创建
=============================================================================*/
CMcuPrsGuardData::CMcuPrsGuardData()
{
	m_byNoScanAckTimes = 0;
	m_byRecvScanAck = 0;
}

/*=============================================================================
函 数 名： GetNoScanAckTimes
功    能： 获得未收到探察回应消息的次数
算法实现： 
全局变量： 
参    数： void
返 回 值： u8 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/4/7   4.0		周广程                  创建
=============================================================================*/
u8 CMcuPrsGuardData::GetNoScanAckTimes(void)
{
	return m_byNoScanAckTimes;
}

/*=============================================================================
函 数 名： AddNoScanAckTimes
功    能： 未收到探察回应消息的次数加1
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/4/7   4.0		周广程                  创建
=============================================================================*/
void CMcuPrsGuardData::AddNoScanAckTimes(void)
{
	if ( MAXNUM_MAINTASKSCAN_TIMEOUT >= m_byNoScanAckTimes )
	{
		m_byNoScanAckTimes++;
	}
}

/*=============================================================================
函 数 名： ResetNoScanAckTimes
功    能： 
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/4/7   4.0		周广程                  创建
=============================================================================*/
void CMcuPrsGuardData::ResetNoScanAckTimes(void)
{
	m_byNoScanAckTimes = 0;
}

/*==========================================================================
函 数 名： IsRecvScanAck
功    能： 
算法实现： 
全局变量： 
参    数： void
返 回 值： BOOL32 
-----------------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/4/7   4.0			周广程                  创建
=============================================================================*/
BOOL32 CMcuPrsGuardData::IsRecvScanAck(void)
{
	return ( 1 == m_byRecvScanAck );
}

/*=============================================================================
函 数 名： SetIsRecvScanAck
功    能： 
算法实现： 
全局变量： 
参    数： BOOL32 bIsRecv
返 回 值： void  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/4/7   4.0			周广程                  创建
=============================================================================*/
void  CMcuPrsGuardData::SetIsRecvScanAck(BOOL32 bIsRecv)
{
	m_byRecvScanAck = bIsRecv ? 1 : 0;
}


