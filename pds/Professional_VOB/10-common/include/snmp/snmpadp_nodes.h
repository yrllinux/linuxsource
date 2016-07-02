/*****************************************************************************
模块名      : SNMP适配模块
文件名      : snmpadp_nodes.h
相关文件    : snmpadp_nodes.cpp
文件实现功能: 
作者        : 李  博
版本        : V4.0  Copyright(C) 2005-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2005/08/18  4.0         李  博      创建
******************************************************************************/ 
#ifndef _SNMPADP_NODES_H_
#define _SNMPADP_NODES_H_

#if ( _SNMPADP_MANAGER_ || WIN32 )
#include <snmp_pp/snmp_pp.h>
#include <snmp_pp/notifyqueue.h>
#endif

#ifdef _VXWORKS_
#include <snmp.h>
#endif

//#if defined(_LINUX_) 
#if (_SNMPADP_AGENT_ && _LINUX_)
#include "net-snmp-agent-includes.h"
#endif

class CNodeCollection
{
public:
    CNodeCollection();
    virtual ~CNodeCollection();

    u16  AddNodeValue(u32 dwNodeName);

    u16  AddNodeValue(u32 dwNodeName, void * pBuf, u16 wBufLen);

    u16  GetNodeValue(u32 dwNodeName, void * pBuf, u16 wBufLen); //u16 wBufLen参数应该设计成返回参数
    
    u16  GetNodeValue(u32 dwNodeName, void * pBuf, u16 wBufLen, s32 nIndex);

    u16  GetNodeNum(void);

    u16  SetTrapType(u16 wTrapType);
    
    u16  GetTrapType(void);
    
    u16  Clear(void);

#ifdef _SNMPADP_MANAGER_
	void GetPduTimeStamp(TimeTicks &ticks);
#endif

#if( _SNMPADP_MANAGER_ || WIN32 )	
    Pdu & GetPduFromNodes(void);
    u16   SetPduToNodes(Pdu & pdu);
#endif
    
#ifdef _VXWORKS_
    SNMP_PKT_T * GetPduFromNodes(void);
#endif

#if( _LINUX_ && _SNMPADP_AGENT_ )
    netsnmp_variable_list * GetVariableFromNodes(void);
    u16  SetVariableToNodes(netsnmp_variable_list * ptValList);
    void FreeVariableList(void);
#endif 


protected:

#if( _SNMPADP_MANAGER_ || WIN32 )
    Pdu  m_pdu;
#endif

#ifdef _VXWORKS_    
    SNMP_PKT_T m_tPkt;
#endif

#if( _LINUX_ && _SNMPADP_AGENT_ )
    u16  m_wTrapType;
    netsnmp_variable_list * m_ptValList;
#endif
    
};

#endif  //!_SNMPADP_NODES_H_
