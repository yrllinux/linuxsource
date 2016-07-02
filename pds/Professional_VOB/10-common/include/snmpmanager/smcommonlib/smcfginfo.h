/*****************************************************************************
   模块名      : KDVSNMPMANAGER
   文件名      : smcfginfo.h
   相关文件    : smcfginfo.cpp
   文件实现功能: 插件的配置消息体设计
   作者		   : 陆昆朋
   版本        : V5.0  Copyright(C) 2009-2012 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本     修改人      修改内容
   2009/12/31  5.0      陆昆朋      创建
******************************************************************************/
#ifndef KDV_SMCFGINFO_H
#define KDV_SMCFGINFO_H

#include "smmacro.h"

#ifdef WIN32
#pragma pack( push, 1 )
#ifndef PACKED
#define PACKED 
#endif
#endif //WIN32

#ifdef _LINUX_
#ifndef PACKED
#define PACKED  __attribute__((packed))
#endif // PACKED 
#endif //_LINUX_

/*====================================================================
类名  : CCfgInfoBase
功能  : 配置管理相关信息基类
注    :
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
1/11/2010   5.0			陆昆朋			增加
====================================================================*/
class CCfgInfoBase
{
public:

	//构造函数
	CCfgInfoBase();

	//此函数仅仅用于网络接受到buffer后的解析
	CCfgInfoBase(const s8* pszBuffer, u32 dwSize);

	//析构
	virtual ~CCfgInfoBase();
	
public:

	//设备类型获取和设置
	//注：低8位为MainType, 高8位为SubType
	u16 GetDeviceType();
	u8 GetDeviceMainType();
	u8 GetDeviceSubType();
	void SetDeviceType(u16 wDeviceType);

	//别名获取和设置
	const s8* GetAlias();
	void SetAlias(const s8* pszAlias, u32 dwLen);

	//164号的获取和设置
	const s8* Get164Num();
	void Set164Num(const s8* psz164Num, u32 dwLen);

	//设备IP的获取和设置
	u32 GetDeviceIP();
	void SetDeviceIP(u32 dwIpAddr/*主机序*/);

	//GK的获取和设置
	u32 GetGkIP();	
	void SetGkIP(u32 dwIpAddr/*主机序*/);

	//开始接受端口的获取和设置
	u16 GetStartRevPort();
	void SetStartRevPort(u16 wPort/*主机序*/);

	//225245开始端口的获取和设置
	u16 Get225245StartPort();
	void Set225245StartPort(u16 wPort/*主机序*/);

	//子网掩码的获取和设置
	u32 GetIpMask();
	void SetIpMask(u32 dwIpMask/*主机序*/);

	//网关的获取和设置
	u32 GetGateWay();
	void SetGateWay(u32 dwGateWay/*主机序*/);

	//层槽号的获取和设置
	u8 GetLayerID();
	void SetLayerID(u8 byLayerID/*主机序*/);
	u8 GetSlotID();
	void SetSlotID(u8 bySlotID/*主机序*/);

	//网口类型的获取和设置
	u8 GetNetPortKind();
	void SetNetPortKind(u8 byNetPortKind/*主机序*/);

	//此函数仅仅用于网络传输的封装buffer
	virtual u32 GetBuffer(s8 *pszBuffer, u32 dwBufferSize);

	CCfgInfoBase& operator=(const CCfgInfoBase& cCfgInfo);

protected:
	//构造函数直接初始化，禁用
	CCfgInfoBase(const CCfgInfoBase&);
	void Copy(const CCfgInfoBase* pcCfgInfoBase);

protected:
	u16 m_wDeviceType;//网络序，设备类型。 主机序的低8位为MainType, 高8位为SubType
	s8  m_achAlias[SM_MAX_ALIAS_LEN + 4];//设备别名
	s8  m_ach164Num[SM_MAX_E164_LEN + 1];//设备164号
	u32 m_dwDeviceIP; //网络序，设备IP
	u32 m_dwGkIP; //网络序，设备GKIP
	u16 m_wStartRevPort; //网络序，MCU时为设备起始接受端口，MT时为设备UDP端口
	u16 m_w225245StartPort; //网络序，MCU时为设备225245起始端口，MT时为设备TCP端口
	u32 m_dwIpMask;//网络序，掩码
	u32 m_dwGateWay;//网络序，网关
	u8	m_byLayerID;//层号
	u8  m_bySlotID;//槽号
	u8	m_byNetPortKind; //网口类型
	u8  m_byReserved;
	u16 m_wReserved;
}PACKED;


/*====================================================================
类名  : CBoardCfgInfo
功能  : 单板配置的类管理(包括mcu)，继承于CCfgInfoBase
注    :
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
1/11/2010   5.0			陆昆朋			增加
====================================================================*/
class CBoardCfgInfo : public CCfgInfoBase
{
public:
	//构造函数
	CBoardCfgInfo();
	//此函数仅仅用于网络接受到buffer后的解析
	CBoardCfgInfo(const s8* pszBuffer, u32 dwSize);
	virtual ~CBoardCfgInfo();

	//此函数仅仅用于网络传输的封装buffer
	//返回值为真实buffer的大小
	virtual u32 GetBuffer(s8 *pszBuffer, u32 dwBufferSize);

	//以下为操作符等于，内部实现，防止外部调用。
	CBoardCfgInfo& operator=(const CBoardCfgInfo& cCfgInfo);
protected:
	//构造函数直接初始化，禁用
	CBoardCfgInfo(const CBoardCfgInfo&);
	void Copy(const CBoardCfgInfo* pcCfgInfo);

private:
}PACKED;

/*====================================================================
类名  : CMtCfgInfo
功能  : 终端配置的类管理，继承于CCfgInfoBase
注    :
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
1/11/2010   5.0			陆昆朋			增加
====================================================================*/
class CMtCfgInfo : public CCfgInfoBase
{
public:
	//构造函数
	CMtCfgInfo();
	//此函数仅仅用于网络接受到buffer后的解析
	CMtCfgInfo(const s8* pszBuffer, u32 dwSize);
	virtual ~CMtCfgInfo();

public:

	//代理IP的获取和设置
	u32 GetAgentIp();
	void SetAgentIp(u32 dwIpAddr);

	//代理端口的获取和设置
	u16 GetAgentPort();	
	void SetAgentPort(u16 wIpPort);

	//网络类型种类的获取和设置
	//0 : 静态IP地址
	//1 : PPPoE
	u16 GetNetKind();	
	void SetNetKind(u16 wNetKind);

	//静态NAT的获取和设置
	u32 GetNatIp();	
	void SetNatIp(u32 dwIpAddr);

	//此函数仅仅用于网络传输的封装buffer
	virtual u32 GetBuffer(s8 *pszBuffer, u32 dwBufferSize);

	CMtCfgInfo& operator=(const CMtCfgInfo& cCfgInfo);
protected:
	//构造函数直接初始化，禁用
	CMtCfgInfo(const CMtCfgInfo&);
	void Copy(const CMtCfgInfo* pcCfgInfo);

private:
	u32 m_dwAgentIp;//网络序，代理IP
	u16 m_wAgentPort;//网络序，代理端口
	u16 m_wNetKind;//网络序，网络类型
	u32 m_dwNatIp;//网络序，静态IP
	
}PACKED;


/*====================================================================
类名  : CBatchCfgInfoBase
功能  : 批量配置基类，公共项
注    :
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
1/11/2010   5.0			陆昆朋			增加
====================================================================*/
class CBatchCfgInfoBase
{
public:
	//构造函数
	CBatchCfgInfoBase();
	//此函数仅仅用于网络接受到buffer后的解析
	CBatchCfgInfoBase(const s8* pszBuffer, u32 dwSize);
	virtual ~CBatchCfgInfoBase();

public:

	//设备类型获取和设置
	//注：低8位为MainType, 高8位为SubType
	u16 GetDeviceType();
	u8 GetDeviceMainType();
	u8 GetDeviceSubType();
	void SetDeviceType(u16 wDeviceType);

	//设备GK的IP
	//得到主机序
	u32 GetGkIP();	
	void SetGkIP(u32 dwIpAddr/*主机序*/);

	//此函数仅仅用于网络传输的封装buffer
	virtual u32 GetBuffer(s8 *pszBuffer, u32 dwBufferSize);

protected:
	//构造函数直接初始化，禁用
	CBatchCfgInfoBase(const CBatchCfgInfoBase&);
	//此函数仅仅用于网络接受到buffer后的解析
	CBatchCfgInfoBase& operator=(const CBatchCfgInfoBase& cCfgInfo);
	void Copy(const CBatchCfgInfoBase* pcCfgInfo);

private:
	u16 m_wDeviceType; //网络序，设备类型
	u32 m_dwGkIP; //网络序，设备GKIP
}PACKED;

/*====================================================================
类名  : CBoardBatchCfgInfo，继承于CBatchCfgInfoBase
功能  : 单板批量配置管理，用于网络传输
注    :
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
1/11/2010   5.0			陆昆朋			增加
====================================================================*/
class CBoardBatchCfgInfo : public CBatchCfgInfoBase
{
public:
	//构造函数
	CBoardBatchCfgInfo();
	//此函数仅仅用于网络接受到buffer后的解析
	CBoardBatchCfgInfo(const s8* pszBuffer, u32 dwSize);
	virtual ~CBoardBatchCfgInfo();

	//层槽号的获取和设置
	u8 GetLayerID();	
	void SetLayerID(u8 byLayerID);
	u8 GetSlotID();	
	void SetSlotID(u8 bySlotID);

	//此函数仅仅用于网络传输的封装buffer
	virtual u32 GetBuffer(s8 *pszBuffer, u32 dwBufferSize);

protected:
	CBoardBatchCfgInfo(const CBoardBatchCfgInfo&);
	CBoardBatchCfgInfo& operator=(const CBoardBatchCfgInfo& cCfgInfo);

	void Copy(const CBoardBatchCfgInfo* pcCfgInfo);
private:
	u8	m_byLayerID;//层号
	u8  m_bySlotID;//槽号
}PACKED;

/*====================================================================
类名  : CMtBatchCfgInfo，继承于CBatchCfgInfoBase
功能  : 单板批量配置管理，用于网络传输
注    :
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
1/11/2010   5.0			陆昆朋			增加
====================================================================*/
class CMtBatchCfgInfo : public CBatchCfgInfoBase
{
public:
	//构造函数
	CMtBatchCfgInfo();
	//此函数仅仅用于网络接受到buffer后的解析
	CMtBatchCfgInfo(const s8* pszBuffer, u32 dwSize);
	virtual ~CMtBatchCfgInfo();

	//此函数仅仅用于网络传输的封装buffer	
	//返回实际填充buffer的长度
	virtual u32 GetBuffer(s8 *pszBuffer, u32 dwBufferSize);


protected:
	CMtBatchCfgInfo(const CMtBatchCfgInfo&);
	CMtBatchCfgInfo& operator=(const CMtBatchCfgInfo& cCfgInfo);

	void Copy(const CMtBatchCfgInfo* pcCfgInfo);
private:
}PACKED;

#ifdef WIN32
#ifdef PACKED
#pragma pack(pop)
#endif
#undef PACKED
#endif //WIN32

#endif
