

// SimuNMS.cpp
#include "SimuNMS.h"

CSimuNmsApp g_cSimuNmsApp;

CSimuNMS::CSimuNMS()
{
    m_dwMcuIns = 0;
    m_dwMcuNode = 0;
    m_dwBrdIns = 0;
    m_dwBrdNode = 0;
    m_dwAgentIns = 0;
    m_dwAgentNode = 0;
}
CSimuNMS::~CSimuNMS()
{
}

/*=============================================================================
  函 数 名： InstanceEntry
  功    能： 模拟Nms入口
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg
  返 回 值： void 
=============================================================================*/
void CSimuNMS::InstanceEntry( CMessage* const pMsg )
{
    CServMsg    cServMsg(pMsg->content, pMsg->length);
    char achAckBuf[512] = {0};

    switch(pMsg->event)
    {

    case SVC_AGT_MEMORY_STATUS:         // 告警
        ProcMemAlarm();
        break;

    case SVC_AGT_FILESYSTEM_STATUS:      //文件系统状态改变
        ProcFileAlarm();
        break;

	case BOARD_MODULE_STATUS: 
        ProcModuleAlarm();
        break;

    case BOARD_LED_STATUS:
        ProcLedAlarm();
        break;

    default:
        break;
        
            
    }

    return;

}

/*=============================================================================
  函 数 名： ProcFileAlarm
  功    能： 处理文件告警
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CSimuNMS::ProcFileAlarm()
{
    post(MAKEIID(APP_SIMUBRDSSN, 1), NMS_REV_FILESYSALARM);
    return;
}

/*=============================================================================
  函 数 名： ProcLedAlarm
  功    能： 处理Led告警
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CSimuNMS::ProcLedAlarm()
{
    post(MAKEIID(APP_SIMUBRDSSN, 1), NMS_REV_LEDALARM);
    return;

}

/*=============================================================================
  函 数 名： ProcMemAlarm
  功    能： 处理内存告警
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CSimuNMS::ProcMemAlarm()
{
    post(MAKEIID(APP_SIMUBRDSSN, 1), NMS_REV_MEMALARM);
    return;
}

/*=============================================================================
  函 数 名： ProcModuleAlarm
  功    能： 处理模块告警
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CSimuNMS::ProcModuleAlarm()
{
    post(MAKEIID(APP_SIMUBRDSSN, 1), NMS_REV_MODULEALARM);
    return;
}