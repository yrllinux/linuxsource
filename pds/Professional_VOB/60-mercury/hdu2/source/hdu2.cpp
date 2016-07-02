/*****************************************************************************
   模块名      : HDU2
   文件名      : hdu2.cpp
   相关文件    : 
   文件实现功能: HDU2业务封装
   作者        : 田志勇
   版本        : V4R7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
  2011/11/28   4.7         田志勇        创建
******************************************************************************/

#include "osp.h"
#include "mcuver.h"
#include "hdu2agent.h"
#include "hdu2app.h"

#ifdef _LINUX_
#include "nipdebug.h"
#endif

#if defined(WIN32) || defined(_WIN32)
#define HDU2_KDVLOG_CONF_FILE	(s8*)"./conf/kdvlog_hdu2.ini"
#else
#define HDU2_KDVLOG_CONF_FILE	(s8*)"/usr/kdvlog/conf/kdvlog.ini"
#endif
/*lint -save -e843*/
/*lint -save -e765*/
static BOOL32 g_bQuit = FALSE;

/*=====================================================================
函数  : hdu2ver
功能  : 查看当前HDU2版本信息
输入  : 无
输出  : 无
返回  : void
注    :
-----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
=====================================================================*/
API void hdu2ver(void)
{
	OspPrintf(TRUE, TRUE, "boardagent version : %s\n", VER_BASICBOARDAGENT);
	OspPrintf(TRUE, TRUE, "      hdu2 version : %s\n", VER_HDU2);
    OspPrintf(TRUE, TRUE, "      compile time : %s:%s\n", __DATE__, __TIME__);
}

/*=====================================================================
函数  : hdu2help
功能  : 查看HDU2外壳帮助命令
输入  : 无
输出  : 无
返回  : void
注    :
-----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
=====================================================================*/
API void hdu2help(void)
{	
	OspPrintf( TRUE, FALSE, "---- All the help selection as follows ----\n");
	OspPrintf( TRUE, FALSE, "\thdu2help : HDU2 help info \n" );
	OspPrintf( TRUE, FALSE, "\thdu2ver  : HDU2 modules version info\n" );
}
#ifdef _LINUX_
/*=====================================================================
函数  : Hdu2APIEnableInLinux
功能  : 注册LINUX下调试函数
输入  : 无
输出  : 无
返回  : void
注    :
-----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
=====================================================================*/
static void Hdu2APIEnableInLinux()
{
	/*lint -save -e611*/
    OspRegCommand("hdu2help",        (void*)hdu2help,         "hdu2 help");
    OspRegCommand("hdu2ver",         (void*)hdu2ver,          "hdu2 ver");
}

/*=====================================================================
函数  : StartCaptureException
功能  : 开启异常捕获
输入  : 无
输出  : 无
返回  : s32
注    :
-----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
=====================================================================*/
static s32 StartCaptureException()
{
    return init_nipdebug(0, NULL, DIR_EXCLOG);
}
/*=====================================================================
函数  : OspQuitFunc
功能  : 程序异常退出处理
输入  : 无
输出  : 无
返回  : void
注    :
-----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
=====================================================================*/
static void OspQuitFunc(int)
{
    g_bQuit = TRUE;
}
#endif
/*=====================================================================
函数  : hdu2start
功能  : HDU2业务入口
输入  : 无
输出  : 无
返回  : void
注    :
-----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
=====================================================================*/
API void hdu2start(void)
{
#ifdef _LINUX_
	s32 nReturn = 0;
	nReturn = system( "cp /usr/bin/dft_16to9.bmp /usr/etc/config/dft_16to9.bmp" );
	if (nReturn != 0)
	{
		LogPrint( LOG_LVL_WARNING, MID_PUB_ALWAYS, 
			"[hdu2start]system(cp /usr/bin/dft_16to9.bmp /usr/etc/config/dft_16to9.bmp) failed!ERRORCODE = [%d]\n", nReturn );
	}
	else
	{
		LogPrint( LOG_LVL_DETAIL, MID_PUB_ALWAYS, 
			"[hdu2start]system(cp /usr/bin/dft_16to9.bmp /usr/etc/config/dft_16to9.bmp) Successed!\n", nReturn );
	}
	nReturn = system( "cp /usr/bin/dft_vga.bmp /usr/etc/config/dft_vga.bmp" );
	if (nReturn != 0)
	{
		LogPrint( LOG_LVL_WARNING, MID_PUB_ALWAYS, 
			"[hdu2start]system(cp /usr/bin/dft_vga.bmp /usr/etc/config/dft_vga.bmp) failed!ERRORCODE = [%d]\n", nReturn );
	}
	else
	{
		LogPrint( LOG_LVL_DETAIL, MID_PUB_ALWAYS, 
			"[hdu2start]system(cp /usr/bin/dft_vga.bmp /usr/etc/config/dft_vga.bmp) Successed!\n", nReturn );
	}

	//zjj20120107升级成功标志提前到注册mcu成功前
	s8 byRet = BrdSetSysRunSuccess();
    if ( byRet == UPDATE_VERSION_ROLLBACK )
    {
        LogPrint(LOG_LVL_WARNING, MID_MCU_EQP,"[hdu2Init] Update failed, rollback version!\n" );
    }
    else if ( byRet == SET_UPDATE_FLAG_FAILED )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_EQP,"[hdu2Init] BrdSetSysRunSuccess() failed,So Reboot!!\n" );
		return;
    }
	else if ( byRet == UPDATE_VERSION_SUCCESS )
	{
        LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP,"[hdu2Init] BrdSetSysRunSuccess() succeed!\n" );
	}    
    else
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP,"[hdu2Init] BrdSetSysRunSuccess(), ret.%d\n",byRet);
	}

#endif

    			     
    if (!InitBrdAgent())
    {
        LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS,"[hdu2start]InitBrdAgent Failed!So Reurn!!\n");
        return;
    }
	else
	{
		LogPrint( LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS,"[hdu2start]InitBrdAgent Successed!\n");
	}
	
    THduCfg tHduEqpCfg;
    tHduEqpCfg.dwConnectIP    = g_cBrdAgentApp.GetMpcIpA();
    tHduEqpCfg.wConnectPort   = g_cBrdAgentApp.GetMpcPortA();
	tHduEqpCfg.dwConnectIpB   = g_cBrdAgentApp.GetMpcIpB();
    tHduEqpCfg.wConnectPortB  = g_cBrdAgentApp.GetMpcPortB();
	tHduEqpCfg.SetHduSubType(g_cBrdAgentApp.GetStartMode());//HDU外设子类型与启动模式一一对应
	tHduEqpCfg.byEqpType = EQP_TYPE_HDU;	//HDU2外设类型设为HDU

	/*switch (g_cBrdAgentApp.GetBrdPosition().byBrdID)
	{
	case BRD_TYPE_HDU:
		{
			u8 byStartMode = g_cBrdAgentApp.GetStartMode();
			tHduEqpCfg.byEqpType = EQP_TYPE_HDU;
			if (STARTMODE_HDU_H == byStartMode)
			{
				tHduEqpCfg.byEqpType = EQP_TYPE_HDU_H;
			}	
			if(STARTMODE_HDU_M == byStartMode)
			{
				tHduEqpCfg.byEqpType = EQP_TYPE_HDU;
			}
			break;
		}
	case BRD_TYPE_HDU2:
		tHduEqpCfg.byEqpType = EQP_TYPE_HDU2;
		break;
	case BRD_TYPE_HDU2_L:
		tHduEqpCfg.byEqpType = EQP_TYPE_HDU2_L;
		break;
	default:
		tHduEqpCfg.byEqpType = EQP_TYPE_HDU2;
		break;
	}*/

	THduModePort tModePort;
	for (u8 byChnIdx = 0;byChnIdx < MAXNUM_HDU_CHANNEL; byChnIdx++)
	{
		g_cBrdAgentApp.GetChnCfg(byChnIdx,tModePort);
		tHduEqpCfg.SetHduModePort(byChnIdx,tModePort);
	}
	LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_EQP,"[hdu2start]EQPTYPE(%d) MCUA(%x,%d) MCUB(%x,%d) LocalIp(%x) IsTest(%d)\n",
		tHduEqpCfg.byEqpType,g_cBrdAgentApp.GetMpcIpA(),g_cBrdAgentApp.GetMpcPortA(),g_cBrdAgentApp.GetMpcIpB(),
		g_cBrdAgentApp.GetMpcPortB(),g_cBrdAgentApp.GetBrdIpAddr(),g_cBrdAgentApp.GetIsTest());
    tHduEqpCfg.byEqpId        = g_cBrdAgentApp.GetEqpId();
    tHduEqpCfg.wMcuId         = LOCAL_MCUID;
    tHduEqpCfg.dwLocalIP      = g_cBrdAgentApp.GetBrdIpAddr();
	tHduEqpCfg.SetAutoTest( g_cBrdAgentApp.GetIsTest() );

    if (!hdu2Init(&tHduEqpCfg))
    {
        LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS,"[hdu2start]hdu2Init() failed!\n");
    }
	else
	{
		LogPrint( LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS,"[hdu2start]hdu2Init() successed!\n");
	}
}
/*lint -restore*/
/*=====================================================================
函数  : main
功能  : HDU2程序主入口
输入  : 无
输出  : 无
返回  : int
注    :
-----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
=====================================================================*/
int main()
{
#ifdef _LINUX_
	s32 nReturn = 0;
	nReturn = system( "cp /usr/bin/kdvlog_hdu2.ini /usr/kdvlog/conf/kdvlog.ini" );
	if (nReturn != 0)
	{
		printf(	"[main]system(cp /usr/bin/kdvlog_hdu2.ini %s) failed!ERRORCODE = [%d]\n", HDU2_KDVLOG_CONF_FILE,nReturn );
	}
	else
	{
		printf(	"[main] cp kdvlog.ini success.%s\n",HDU2_KDVLOG_CONF_FILE );
	}
#endif

	Err_t err = LogInit(HDU2_KDVLOG_CONF_FILE);
	if (err != LOG_ERR_NO_ERR)
	{
		printf("[main]LogInit() failed! ERRCODE = [%d]\n",err);
	}
	logenablemod(MID_MCU_EQP);
	//logsetdst(LOG_DST_SERIAL);
#ifdef _LINUX_
	nReturn = 0;
	//设置进程异常捕获回调
    nReturn = StartCaptureException();
	if ( nReturn != 0 )
	{
		LogPrint( LOG_LVL_WARNING, MID_PUB_ALWAYS, "[main]StartCaptureException() failed! ERRCODE = [%d]\n",nReturn);
	}
	else
	{
		LogPrint( LOG_LVL_DETAIL, MID_PUB_ALWAYS, "[main]StartCaptureException() Successed!\n");
	}
    //注册退出清除函数
	if (OspRegQuitFunc( OspQuitFunc ))
	{
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[main]OspRegQuitFunc() Successed!\n");
	}
	else
	{
		LogPrint( LOG_LVL_DETAIL, MID_PUB_ALWAYS, "[main]OspRegQuitFunc() failed!\n");
	}
	Hdu2APIEnableInLinux();
#endif

	hdu2start();

	while(!g_bQuit)
	{
	    OspDelay(1000);
	}

	LogPrint( LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[main]Quit normally!\n");
	return 0;
}
