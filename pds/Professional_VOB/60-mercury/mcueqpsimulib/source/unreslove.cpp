//In order to resolve libary
#include "osp.h"
#include "boardagent.h"
#include "eqpunit.h"

#ifndef _HD_
u32 m_wConnectedMcuNode = 0;  // =0

u32 BrdGetDstMcuNode( void )
{
	return m_wConnectedMcuNode;
}

u32 BrdGetDstMcuNodeB(void)
{
    return 0;
}

u8 BrdGetTvwallModel(void)
{
    return 3; // “Ù ”∆µ∂º”–
}

void EqpConnectToMcu( u32 dwIp, u16 wPort )
{
	m_wConnectedMcuNode = OspConnectTcpNode( dwIp, wPort );
}
#endif