#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_

#include "kdvtype.h"
#include "kdvdef.h"
#include "osp.h"
#include "eventid.h"

//////////////////////////////////////////////////////////////////////////
//WDCLT 
#define  WD_CLT_APP_ID             (AID_DSC_BGN+2) 
//////
enum TMType
{ 
	em_INVALIDMODULE   = -1,           //未知模块
	em_TS              = 0,            //TS
	em_GK              = 1,            //GK
	em_PROXY           = 2,            //PROXY
    em_MMP             = 3,            //MMP
	em_MCU             = 4,            //MCU
	em_MP              = 5,            //MP
	em_MTADP           = 6,            //MTADP
	em_PRS             = 7,            //PRS
	em_TUI             = 8,            //TUI
	em_DSS             = 9,            //DSS
	em_MPS			   = 10,			  //MPS
	em_END
};

#define  WD_MODULE_NUM             (u16)em_END

enum TMState
{
	em_STOP            = 0,          //停止
    em_RUNNING         = 1           //运行
};
////////////////////////////////////////////
//监护的线程类型
enum eMCUAPPType
{
	em_MCU_INVALIDAPP  = 0,
	em_MCU_GUARD_APP   = 1
};
enum eTsAPPType
{
	em_TS_INVALID_APP  = 0,     
	em_TS_OPER_APP     = 1,
	em_TS_TRVLS_APP    = 2,
	em_TS_CALLS_APP    = 3
};
enum eGKAPPType
{
	em_GK_INVALID_APP  = 0, 
	em_GK_OPER_APP     = 1
};
enum ePXYAPPType
{
	em_PXY_INVALID_APP = 0,
};
enum eMMPAPPType
{
	em_MMP_INVALID_APP = 0,
};

//模块参数
typedef struct tagModuleParam
{
    tagModuleParam() : emType(em_INVALIDMODULE), emState(em_STOP) {}
	
    TMType    emType;     //模块类型
    TMState   emState;    //模块状态
} TModuleParameter, *PTModuleParameter;

typedef struct tagOthersMState
{
	TModuleParameter m_achOtherMdInfo[em_END];
}TOthersMState;

//遵循看门狗服务端的事件范围
OSPEVENT(WD_APP_WDC_REG_REQ,                   (EV_WD8000E_BGN + 40 )); //enum
OSPEVENT(WD_WDC_APP_REG_ACK,                   (EV_WD8000E_BGN + 41 )); //nothing
OSPEVENT(WD_WDC_APP_HB_REQ,                    (EV_WD8000E_BGN + 42 )); //nothing
OSPEVENT(WD_APP_WDC_HB_ACK,                    (EV_WD8000E_BGN + 43 )); //enum
OSPEVENT(WD_MCUGD_WDC_REBOOT_CMD,              (EV_WD8000E_BGN + 44 )); //nothing
OSPEVENT(WD_MCUGD_WDC_GETKEY_REQ,              (EV_WD8000E_BGN + 45 )); //nothing
OSPEVENT(WD_WDC_MCUGD_GETKEY_ACK,              (EV_WD8000E_BGN + 46 )); //TLicenseMsg
OSPEVENT(WD_MCUGD_WDC_GETMDINFO_REQ,           (EV_WD8000E_BGN + 47 )); //nothing
OSPEVENT(WD_WDC_MCUGD_GETMDINFO_ACK,           (EV_WD8000E_BGN + 48 )); //nothing
OSPEVENT(WD_WDC_MCUGD_GETMDINFO_NOTIFY,        (EV_WD8000E_BGN + 49 )); //
OSPEVENT(WD_MCUGD_WDC_UPDATE_CMD,              (EV_WD8000E_BGN + 50 )); //nothing
OSPEVENT(WD_MCUGD_WDC_SHUTDOWN_CMD,            (EV_WD8000E_BGN + 51 )); //nothing
OSPEVENT(WD_MCUGD_WDC_SOFTREBOOT_CMD,          (EV_WD8000E_BGN + 52 )); //nothing

/*=============================================================================
  函 数 名： StartWatchDog
  功    能： 启动看门狗 
  算法实现： 
  全局变量： 
  参    数：  TMType emModuletype 
              BOOL         bIsCarrier
  返 回 值：  BOOL32  看门狗应用是否成功启动
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2009/02/24    1.0         张洪彬      创建
=============================================================================*/
BOOL32 WDStart(TMType emModuletype,BOOL bIsCarrier = FALSE);
/*=============================================================================
  函 数 名： SetModuleState
  功    能： 设置模块状态 
  算法实现： 
  全局变量： 
  参    数：  TMState emState           
  返 回 值：  BOOL32  看门狗应用是否成功启动
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2009/02/24    1.0         张洪彬      创建
=============================================================================*/
void WDSetModuelState(TMState emState);
/*=============================================================================
  函 数 名： SetModuWDStopleState
  功    能： 设置模块状态 
  算法实现： 
  全局变量： 
  参    数：                          
  返 回 值：  
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2009/02/24    1.0         张洪彬      创建
=============================================================================*/
void WDStop();
#endif
