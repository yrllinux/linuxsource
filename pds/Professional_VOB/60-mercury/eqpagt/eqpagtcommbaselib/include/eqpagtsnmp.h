/*****************************************************************************
    模块名      : EqpAgt
    文件名      : eqpagtsnmp.h
    相关文件    : 
    文件实现功能: Snmp功能调用
    作者        : liaokang
    版本        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    修改记录:
    日  期      版本        修改人        修改内容
    2012/06/18  4.7         liaokang      创建
******************************************************************************/
#ifndef _EQPAGT_SNMP_H_
#define _EQPAGT_SNMP_H_

#include "eqpagtutility.h"

class CEqpAgtSnmp : public CAgentAdapter
{

public:
    CEqpAgtSnmp();
    ~CEqpAgtSnmp();

public :
    u16  EqpAgtSendTrap(CNodes& cNodes);
};

extern CEqpAgtSnmp g_cEqpAgtSnmp;

#endif // _EQPAGT_SNMP_H_

