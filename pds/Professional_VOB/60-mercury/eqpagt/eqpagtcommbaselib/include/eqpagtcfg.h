/*****************************************************************************
    模块名      : EqpAgt
    文件名      : eqpagtcfg.h
    相关文件    : 
    文件实现功能: 配置设备代理信息
    作者        : liaokang
    版本        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    修改记录:
    日  期      版本        修改人          修改内容
    2012/06/18  4.7         liaokang        创建
******************************************************************************/
#ifndef _EQPAGT_CFG_H_
#define _EQPAGT_CFG_H_
#include "eqpagtutility.h"
#include "eqpagtcommon.h"

#define MAXLEN_PORT		         (u8)5                      // 最大端口字符串长度

/*------------------------------------------------------------- 
*EqpAgt信息表常量 
*-------------------------------------------------------------*/
#define SECTION_EqpAgtInfoTable   ( LPCSTR )"EqpAgtInfoTable"// EqpAgt信息表
#define ENTRY_NUM		          ( LPCSTR )"EntryNum"       // 接受Trap的网管服务器数目
#define ENTRY_KEY				  ( LPCSTR )"Entry"          // Entry 字符串
#define FIELD_TrapSrvIpAddr       ( LPCSTR )"TrapSrvIpAddr"  // 接受Trap的网管服务器Ip(网络序)
#define FIELD_ReadCommunity       ( LPCSTR )"ReadCommunity"  // 读共同体
#define FIELD_WriteCommunity      ( LPCSTR )"WriteCommunity" // 写共同体
#define FIELD_GetSetPort          ( LPCSTR )"GetSetPort"     // 读取端口(主机序)
#define FIELD_SendTrapPort        ( LPCSTR )"SendTrapPort"   // 发Trap端口(主机序)

class CEqpAgtCfg
{
public:
    CEqpAgtCfg();
	~CEqpAgtCfg();

    // 基本入口函数
    BOOL32 EqpAgtCfgInit( LPCSTR lpszCfgPath, LPCSTR lpszCfgName );		     // 初始化数据区

    // EqpAgt信息表维护，对于EqpAgt信息是'整体'读/写
    BOOL32 ReadEqpAgtInfoTable( u8* pbyTrapSrvNum, TEqpAgtInfo* ptEqpAgtInfoTable ); // 整体读EqpAgt信息表  
    BOOL32 WriteEqpAgtInfoTable( u8 byTrapSrvNum,  TEqpAgtInfo* ptEqpAgtInfoTable ); // 整体写EqpAgt信息表

    // Trap信息，对于Trap信息是'整体'读/写
    BOOL32 TrapIpsOfEqpAgt2Nms( void *pBuf, u16 *pwBufLen );    // Trap Ip信息 EqpAgt ----> Nms
    BOOL32 TrapIpsOfNms2EqpAgt( void *pBuf, u16 *pwBufLen );    // Trap Ip信息 Nms ----> EqpAgt

    // snmp    
    u8     GetTrapServerNum( void );                            // 取Trap服务器数    
    void   GetTrapTarget( u8 byIdx, TTarget& tTrapTarget );     // 取某一Trap服务器对应的Trap信息结构    
    BOOL32 GetSnmpParam( TSnmpAdpParam& tParam ) const;         // 取SNMP参数    
    BOOL32 HasSnmpNms( void ) const;                            // 取是否配置了网管

protected:
    // 配置文件  
    BOOL32 OrganizeEqpAgtInfoTable( u8 byIdx, TEqpAgtInfo * pEqpAgtInfo, s8* ptInfTobeWrited ); // 单个EqpAgt服务器表项
    BOOL32 ReadTrapTable( LPCSTR lpszCfgFileFullPath );         // 读Trap表
    void   CreateDir( LPCSTR pPathName );			            // 创建目录
    BOOL32 PretreatCfgFile(  LPCSTR lpszCfgPath, LPCSTR lpszCfgName ); // 没有配置文件时生成默认配置文件
private:
    s8          m_achCfgPath[EQPAGT_MAX_PATH_LEN];	            // 配置文件路径
    s8          m_achCfgName[EQPAGT_MAX_NAME_LEN];	            // 配置文件名
    u8		    m_byTrapSrvNum;				                    // Trap服务器数目
    TEqpAgtInfo m_atEqpAgtInfoTable[MAXNUM_EQPAGTINFO_LIST];	// EqpAgt信息表
};


// EqpAgtCfg 生成默认值链表操作
typedef struct EqpAgtCfgDefValue
{
    u16                 m_wIdx;
    TEqpAgtCfgDefValue  m_pfFunc;
    EqpAgtCfgDefValue  *m_pNext;
}EqpAgtCfgDefValue;

class CEqpAgtCfgDefValueList
{
public:
    CEqpAgtCfgDefValueList();
    ~CEqpAgtCfgDefValueList();
    
    u16    Size( void ) { return m_wEqpAgtCfgDefValueSize;}
    BOOL32 GetEqpAgtCfgDefValue( u16 wIdx, TEqpAgtCfgDefValue* pfFunc );
    void   PushBack( TEqpAgtCfgDefValue pfFunc );    
private:
    EqpAgtCfgDefValue *m_pEqpAgtCfgDefValueHead;
    u16                m_wEqpAgtCfgDefValueSize;
};


extern CEqpAgtCfg	g_cEqpAgtCfg;
extern CEqpAgtCfgDefValueList   g_cEqpAgtCfgDefValueList;

#endif  // _EQPAGT_CFG_H_
