/*****************************************************************************
   模块名      : Recorder
   文件名      : RecServLog.cpp
   相关文件    : RecApp.cpp, RecServInst.cpp, State.cpp, StateBase.cpp等
   文件实现功能: Recorder Server实现
   作者        : 
   版本        : V1.0  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2010/08/26  1.0         刘 旭        创建
******************************************************************************/

#include "RecServ_Def.h"


static BOOL32 s_bRecServLog = TRUE;							// RecServInst的log开关，默认为开
static u8     s_byLogLevel  = RECSERV_LEVEL_LOG_WARNING;	// 优先级log的值，默认是输出warning及以上


/*=============================================================================
  函 数 名： recservhelp
  功    能： RecServInst打印log的帮助函数
  算法实现： 
  全局变量： 
  参    数： 无
  返 回 值： void 
=============================================================================*/
API void recservhelp()
{
	OspPrintf(TRUE, FALSE, "==========================================================================\n");
	OspPrintf(TRUE, FALSE, "==               Helps of Recorcer Server                               ==\n");
	OspPrintf(TRUE, FALSE, "precservlog()                 : Print log from recserver\n");
	OspPrintf(TRUE, FALSE, "nprecservlog()                : Don't print log from recserver\n");
	OspPrintf(TRUE, FALSE, "setloglevel(u8 byLevel)       : Set the level of levellog。loglevels were defined below\n");
	OspPrintf(TRUE, FALSE, "     loglevel < 4 : only output error message\n");
	OspPrintf(TRUE, FALSE, "     loglevel < 6 : output error and warning message\n");
	OspPrintf(TRUE, FALSE, "     loglevel < 8 : output error, warning and common message\n");
	OspPrintf(TRUE, FALSE, "     loglevel >= 8: output all messages\n");
	OspPrintf(TRUE, FALSE, "recservstatus(u8 byInstId)    : Show the status of RecServInst\n");
	OspPrintf(TRUE, FALSE, "     byInstId = 0 : Show all recservs' status");
	OspPrintf(TRUE, FALSE, "     byInstId < %u: Show recserv[byInstId]'s status", MAXNUM_RECORDER_CHNNL);
	OspPrintf(TRUE, FALSE, "     byInstId > %u: Input Error", MAXNUM_RECORDER_CHNNL);
	OspPrintf(TRUE, FALSE, "==========================================================================\n");
}


/*=============================================================================
  函 数 名： precservlog
  功    能： 打开recserv log的函数
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
API void precservlog()
{
	s_bRecServLog = TRUE;
	OspPrintf(TRUE, FALSE, "Recserv log opend\n");
}


/*=============================================================================
  函 数 名： precservlog
  功    能： 关闭recserv log的函数
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
API void nprecservlog()
{
	s_bRecServLog = FALSE;
	OspPrintf(TRUE, FALSE, "Recserv log closed\n");
}


/*=============================================================================
  函 数 名： setloglevel
  功    能： 设置优先级log的优先级值
  算法实现： 
  全局变量： 
  参    数： 
			 byLevel : 将要设置成的优先级
  返 回 值： void 
=============================================================================*/
API void setloglevel(const u8 byLevel)
{
	if (byLevel <= RECSERV_LEVEL_LOG_BELOW)
	{
		// 参数比最低优先级高，则将优先级设置成参数的值
		s_byLogLevel = byLevel;
	}else
	{
		// 参数比最低优先级还低，则将优先级设置成最低优先级
		s_byLogLevel = RECSERV_LEVEL_LOG_BELOW;
	}

	OspPrintf(TRUE, FALSE, "Log level has been set to %d", s_byLogLevel);
}

/*=============================================================================
  函 数 名： RecServLog
  功    能： RecServLog打印优先级log的函数
  算法实现： 
  全局变量： 
  参    数： 
			 szFormat : 输出字符串
			 其它请参照标准c的printf参数
  返 回 值： void 
=============================================================================*/
void RecServLog( s8* pszFmt, ... )
{
    s8 achPrintBuf[KDV_MAX_PATH];
    s32  nBufLen = 0;
    va_list argptr;
    if( s_bRecServLog )
    {		  
        nBufLen = sprintf( achPrintBuf, "[RecServ]: " ); 
        va_start( argptr, pszFmt );
		vsnprintf(achPrintBuf + nBufLen, KDV_MAX_PATH - nBufLen - 1, pszFmt, argptr );
        //nBufLen += vsprintf( achPrintBuf + nBufLen, pszFmt, argptr ); 
        va_end(argptr); 
        OspPrintf( TRUE, FALSE, achPrintBuf ); 
    }
    return;
}


/*=============================================================================
  函 数 名： RecServLevelLog
  功    能： RecServLevelLog打印优先级log的函数
  算法实现： 
  全局变量： 
  参    数： 
			 byLevel  : log的优先级，请参照log优先级值定义
			 szFormat : 输出字符串
			 其它请参照标准c的printf参数
  返 回 值： void 
=============================================================================*/
void RecServLevelLog( const u8& byLevel, s8* pszFmt, ... )
{
	// 优先级比当前优先级高才输出
	if (byLevel <= s_byLogLevel)
    {
		// log输出开关是打开的
        if( s_bRecServLog )
        {	
			s8 achPrintBuf[KDV_MAX_PATH];
			s32  nBufLen = 0;

			// 设置log信息头
			switch (byLevel)
			{
			case RECSERV_LEVEL_LOG_ERROR:
				nBufLen = sprintf( achPrintBuf, "[Error]" );
				break;
				
			case RECSERV_LEVEL_LOG_WARNING:
				nBufLen = sprintf( achPrintBuf, "[Warning]" );
				break;

			default:
				nBufLen = sprintf( achPrintBuf, "[RecServ]" );
				break;
			}

			// 提取输出字符串
	        va_list argptr;
            va_start( argptr, pszFmt );
			vsnprintf(achPrintBuf + nBufLen, KDV_MAX_PATH - nBufLen - 1, pszFmt, argptr );
            //nBufLen += vsprintf( achPrintBuf + nBufLen, pszFmt, argptr ); 
            va_end(argptr); 

			// 输出log
            OspPrintf( TRUE, FALSE, achPrintBuf ); 
        }
    }
}

API void recservstatus(u8 byInstId = 0)
{
	if (byInstId == 0)
	{
		OspPost(MAKEIID(AID_REC_SERVER, CInstance::DAEMON), EV_RECSERV_SHOW_STATUS);
	}else
	{
		if (byInstId > MAXNUM_RECORDER_CHNNL)
		{
			OspPrintf(TRUE, FALSE, "The parameter Input is invalid, it must be in [0, %u]", MAXNUM_RECORDER_CHNNL);
			return;
		}

		OspPost(MAKEIID(AID_REC_SERVER, byInstId), EV_RECSERV_SHOW_STATUS);
	}	
}