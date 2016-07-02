/*****************************************************************************
    模块名      : EqpAgt
    文件名      : eqpagtcommbase.h
    相关文件    : 
    文件实现功能: EqpAgt通讯基础
    作者        : liaokang
    版本        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    修改记录:
    日  期      版本        修改人          修改内容
    2012/06/18  4.7         liaokang        创建
******************************************************************************/

#ifndef _EQPAGT_COMMBASE_H_
#define _EQPAGT_COMMBASE_H_

#include "eqpagtutility.h"

struct TEqpAgtCBEntry
{
    // 结点名，根据结点名区分业务处理函数
    u32  dwNodeValue;
    // 回调函数业务处理具体实现
    TEqpAgtCBFunc pfFunc;
};

struct TEqpAgtTrapEntry
{
    // 结点名，根据结点名区分业务处理函数
    u32  dwNodeValue;
    // Trap函数
    TEqpAgtTrapFunc pfFunc;
    // 定时检测Trap的时长（单位：秒）
    u32 dwScanTimeSpan;
    // 支持Trap操作（以便调试看某项打印）
    BOOL32 bEnable;
};

typedef struct EqpAgtCB
{
    u16            m_wEqpAgtCBIdx;
    TEqpAgtCBEntry m_tEqpAgtCBEntry;
    EqpAgtCB      *m_pEqpAgtCBNext;
}EqpAgtCB;

typedef struct EqpAgtTrap
{
    u16              m_wEqpAgtTrapIdx;
    TEqpAgtTrapEntry m_tEqpAgtTrapEntry;
    EqpAgtTrap      *m_pEqpAgtTrapNext;
}EqpAgtTrap;

// Get/Set 链表操作
class CEqpAgtCBList
{
public:
    CEqpAgtCBList();
	~CEqpAgtCBList();

    u16    Size( void ) { return m_wEqpAgtCBSize;}
    BOOL32 GetEqpAgtCBEntry( u16 wEqpAgtCBIdx, TEqpAgtCBEntry* ptEqpAgtCBntry );
    void   PushBack( const TEqpAgtCBEntry& tEqpAgtCBEntry );    
    void   Replace( const TEqpAgtCBEntry& tEqpAgtCBEntry );
private:
    EqpAgtCB *m_pEqpAgtCBHead;
    u16       m_wEqpAgtCBSize;
};

// Trap 链表操作
class CEqpAgtTrapList
{
public:
    CEqpAgtTrapList();
	~CEqpAgtTrapList();

    u16    Size( void ) { return m_wEqpAgtTrapSize;}
    BOOL32 GetEqpAgtTrapEntry( u16 wEqpAgtTrapIdx, TEqpAgtTrapEntry* ptEqpAgtTrapEntry );
    void   PushBack( const TEqpAgtTrapEntry& tEqpAgtTrapEntry );    
    void   Replace( const TEqpAgtTrapEntry& tEqpAgtTrapEntry );
private:
    EqpAgtTrap *m_pEqpAgtTrapHead;
    u16         m_wEqpAgtTrapSize;
};

// 链表操作
extern CEqpAgtCBList   g_cEqpAgtGetList;
extern CEqpAgtCBList   g_cEqpAgtSetList;
extern CEqpAgtTrapList g_cEqpAgtTrapList;

// 回调函数实现
u16 AgentCallBack(u32 dwNodeName, u8 byRWFlag, void * pBuf, u16* pwBufLen);
// 释放信号量
BOOL32 FreeSemHandle( SEMHANDLE &hSem );
// EqpAgt初始化
u16 EqpAgtCommBaseInit( u16 wAidEqpAgtScan, u8 byAppPri, LPCSTR lpszCfgPath, LPCSTR lpszCfgName );
// 退出
void EqpAgtCommBaseQuit(void);

#endif  // _EQPAGT_COMMBASE_H_