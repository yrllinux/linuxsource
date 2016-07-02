/*=============================================================================
  文 件 名： SimuSnmpLib.cpp
  功    能： 模拟SnmpLib
  算法实现： 
  全局变量： 

=============================================================================*/
// SimuSnmpLib.cpp
#include "testevent.h"
#include "evagtsvc.h"
#include "simuSnmplib.h"

TAgentCallBack g_AgentCallBack;


/*=============================================================================
  函 数 名： Initialize
  功    能： 模拟Snmp初始化
  算法实现： 
  全局变量： 
  参    数： TSnmpAdpParam * ptSnmpAdpParam /* = NULL */
  返 回 值： void
=============================================================================*/
void CAgentAdapter::Initialize(TAgentCallBack tSnmpCallBack, TSnmpAdpParam * ptSnmpAdpParam /* = NULL */)
{
    return;
}

/*=============================================================================
  函 数 名： SetCommunity
  功    能： 设置共同体
  算法实现： 
  全局变量： 
  参    数： s8 * pchCommunity
             u8 byCommunityType
  返 回 值： void 
=============================================================================*/
void CAgentAdapter::SetCommunity(s8 * pchCommunity, u8 byCommunityType)
{
    return;
}

/*=============================================================================
  函 数 名： SetAgentCallBack
  功    能： 设置回调函数
  算法实现： 
  全局变量： 
  参    数： TAgentCallBack tSnmpCallBack
  返 回 值： void 
=============================================================================*/
void CAgentAdapter::SetAgentCallBack(TAgentCallBack tSnmpCallBack)
{
    
    g_AgentCallBack = tSnmpCallBack;
    m_tAgentCallBack = tSnmpCallBack;
    return;
    
}

/*=============================================================================
  函 数 名： SendTrap
  功    能： 发Trap
  算法实现： 
  全局变量： 
  参    数： TTarget tTrapRcvAddr
             CNodes & cNodes
  返 回 值： void 
=============================================================================*/
void CAgentAdapter::SendTrap(TTarget tTrapRcvAddr, CNodes & cNodes)
{
    u32  dwNodes = 0;
    
    u16  wTrapType = GetTrapType(cNodes);
    switch(wTrapType)
    {

    case BOARD_LED_STATUS:
    case BOARD_MODULE_STATUS:
    case SVC_AGT_FILESYSTEM_STATUS:
    case SVC_AGT_MEMORY_STATUS:
        // 告警转发到模拟Nms
        post(MAKEIID(APP_SIMUNMSSSN,1), wTrapType);
        break;

    default:
        break;
    }
}

u16 CAgentAdapter::GetTrapType(CNodes cNodes)
{
    return cNodes.GetTrapType();
}