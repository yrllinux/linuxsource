/*****************************************************************************
   模块名      : Mcu Agent
   文件名      : AgentSnmp.h
   相关文件    : 
   文件实现功能: Snmp类
   作者        : 
   版本        : V4.0  Copyright( C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2005/08/17  4.0         liuhuiyun     创建
******************************************************************************/
#ifndef _AGENT_SNMP_H
#define _AGENT_SNMP_H

#include "snmpadp.h"

class CAgentSnmp : public CAgentAdapter
{

public:
    CAgentSnmp();
    ~CAgentSnmp();

public :
    u16  AgentSendTrap(CNodes & cNodes);
};

#endif // _AGENT_SNMP_H

