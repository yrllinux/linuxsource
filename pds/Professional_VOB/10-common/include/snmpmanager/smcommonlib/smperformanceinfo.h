/*****************************************************************************
   模块名      : KDVSNMPMANAGER
   文件名      : smperformanceinfo.h
   相关文件    : smperformanceinfo.cpp
   文件实现功能: 插件的性能消息体设计
   作者		   : 陆昆朋
   版本        : V5.0  Copyright(C) 2009-2012 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本     修改人      修改内容
   2009/12/31  5.0      陆昆朋      创建
******************************************************************************/
#ifndef KDV_SMPERFORMANCEINFO_H
#define KDV_SMPERFORMANCEINFO_H

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

//以下定义性能统计类型

const u8 KDV_STATISTIC_NOTHING = 0; //告警
const u8 KDV_STATISTIC_CONFNUM = KDV_STATISTIC_NOTHING + 1; //mcu会议数
const u8 KDV_STATISTIC_MTNUM = KDV_STATISTIC_NOTHING + 2; //mcu上线终端数
const u8 KDV_STATISTIC_EQPUSERATE = KDV_STATISTIC_NOTHING + 3; //mcu板外设资源使用率
const u8 KDV_STATISTIC_CPURATE = KDV_STATISTIC_NOTHING + 4; //CPU性能统计
const u16 KDV_PFMINFO_BUFFER_MAX = 1000;

//以下结构用于传输，网络序
//基本性能数据类型
typedef struct tagEqpStat
{
	u16 m_wStatEqpType;
	u16 m_wReserved;
	u16 m_wEqpUsed;
	u16 m_wEqpTotal;
}PACKED TEqpStat;

//以下结构用于传输，网络序
//基本性能数据类型
typedef struct tagPerformanceInfo
{
	u32	m_wAudioLoseRate;//低16位为丢包率，高16位为总包数
	u32	m_wVideoLoseRate;//低16位为丢包率，高16位为总包数
	u32	m_dwMemory;//低16位为使用数，高16位为总数
	u16	m_wMtNum;
	u16	m_wConfNum;
	u32 m_dwAuthMtNum; //分为两种，低16位代表已使用MT授权数，高16位总MT授权数
	u16	m_wMachTemperature;
	u16	m_wCpuRate;
	u16	m_wEqpTypeNum;
	u16	m_wReserved;
	TEqpStat m_atEqpStat[SM_STAT_EQP_TYPE_MAX];
}PACKED TPerformanceInfo;

//以下结构用于传输，网络序
//设备性能总信息
typedef struct tagDevPfmInfo
{
	u16 m_wDeviceType;
	s8  m_achAlias[SM_MAX_ALIAS_LEN + 4];//设备别名
	TPerformanceInfo m_tPfmInfo;
}PACKED TDevPfmInfo;

/*====================================================================
类名  : CDevPfmInfoBase
功能  : 获取当前设备性能消息类
注    :
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
1/11/2010   5.0			陆昆朋			增加
====================================================================*/
class CDevPfmInfoBase
{
public:
	//构造函数
	CDevPfmInfoBase();

	//此函数仅仅用于网络接受到buffer后的解析
	CDevPfmInfoBase(const s8* pszBuffer, u32 dwSize);
	virtual ~CDevPfmInfoBase();
	
public:

	//设备类型获取和设置
	//注：低8位为MainType, 高8位为SubType
	u16 GetDeviceType();
	u8 GetDeviceMainType();
	u8 GetDeviceSubType();
	void SetDeviceType(u16 wDeviceType);

	//别名的获取和设置
	const s8* GetAlias();
	void SetAlias(const s8* pszAlias, u32 dwLen);

	//获取性能数据
	//注意参数tPfmInfo为网络序
	const TDevPfmInfo& GetDevPfmInfo();	
	void SetDevPfmInfo(const TDevPfmInfo& tPfmInfo);
	void SetPfmInfobase(const TPerformanceInfo& tPfmInfobase);

	//此函数仅仅用于网络传输的封装buffer
	virtual u32 GetBuffer(s8 *pszBuffer, u32 dwBufferSize);

protected:
	//构造函数直接初始化，禁用
	CDevPfmInfoBase(const CDevPfmInfoBase&);
	//以下为操作符等于，内部实现，防止外部调用。
	CDevPfmInfoBase& operator=(const CDevPfmInfoBase& cPfmInfo);
	void Copy(const CDevPfmInfoBase* pcPfmInfo);

protected:
	TDevPfmInfo m_tDevPfmInfo; //性能相关数据
}PACKED;


//以下结构用于传输，网络序
//历史性能基本存储单元
typedef struct tagPfmInfoBase
{
	u32 m_dwTime; //时间
	u32 m_dwValue; //对应的性能值, 低16位为已使用，高16位为总
}PACKED TPfmInfoBase;


//以下结构用于传输，网络序
typedef struct tagPfmInfoReq
{
	u8 m_byStatisticKind;//统计类型
	u8  m_byEqpType;	//如果是外设使用率统计，需要填写外设类型
	u32 m_dwStartTime;	//开始统计时间
	u32 m_dwEndTime;	//结束统计时间
	u16 m_wHitsRate;	//采样率，确指统计时间内所要的数据量
}PACKED TPfmInfoReq;

/*====================================================================
类名  : CDevPfmInfoBase
功能  : 获取设备历史时间段性能数据请求,用于NP向SM发送请求时
注    :
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
1/11/2010   5.0			陆昆朋			增加
====================================================================*/
class CDevPfmInfoReq
{
public:
	CDevPfmInfoReq();
	
	//此函数仅仅用于网络接受到buffer后的解析
	CDevPfmInfoReq(const s8* pszBuffer, u32 dwSize);
	//无虚函数，如果有申请空间且被继承，请用虚析构函数
	~CDevPfmInfoReq();

	//统计种类的获取及设置
	void SetStatisticKind(u8 byKind);
	u8 GetStatisticKind();

	void SetStatEqpType(u8 byStatEqpType);
	u8 GetStatEqpType();

	//时间区间的获取和设置
	void SetTimeSpan(u32 dwStartTime, u32 dwEndTime);
	u32 GetStartTime();
	u32 GetEndTime();

	//采样率的统计及设置
	void SetHitsRate(u16 wHitsRate);
	u16 GetHitsRate();

	//此函数仅仅用于网络传输的封装buffer
	virtual u32 GetBuffer(s8 *pszBuffer, u32 dwBufferSize);

	const TPfmInfoReq* GetPfmInfoReq();

protected:
	//构造函数直接初始化，禁用
	CDevPfmInfoReq(const CDevPfmInfoReq&);
	//以下为操作符等于，内部实现，防止外部调用。
	CDevPfmInfoReq& operator=(const CDevPfmInfoReq& cPfmInfoReq);
	void Copy(const CDevPfmInfoReq* pcPfmInfoReq);
private:
	//内部封装，存储为网络序
	TPfmInfoReq m_tPfmInfoReq;//获取设备性能的Req信息
};

/*====================================================================
类名  : CDevPfmInfoNotify
功能  : 发送设备历史时间段性能数据,用于SM向NP发送请求时
注    :
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
1/11/2010   5.0			陆昆朋			增加
====================================================================*/
class CDevPfmInfoNotify
{
public:
	//构造函数
	CDevPfmInfoNotify();
	
	//此函数仅仅用于网络接受到buffer后的解析
	CDevPfmInfoNotify(const s8* pszBuffer, u32 dwSize);
	//无虚函数，如果有申请空间且被继承，请用虚析构函数
	~CDevPfmInfoNotify();

public:

	u16 GetPfmInfoNum();

	//增加基本性能数据信息
	BOOL32 Add(const TPfmInfoBase& tPfmInfo);

	//通过索引获取设备信息
	//索引区间 0 - KDV_PFMINFO_BUFFER_MAX(1000) - 1
	const TPfmInfoBase* GetPfmInfo(u32 dwIndex);
	
	//清除buffer信息，m_wNum = 0
	void ClearBuffer();

	//此函数仅仅用于网络传输的封装buffer
	virtual u32 GetBuffer(s8 *pszBuffer, u32 dwBufferSize);

protected:
	//构造函数直接初始化，禁用
	CDevPfmInfoNotify(const CDevPfmInfoNotify&);
	//以下为操作符等于，内部实现，防止外部调用。
	CDevPfmInfoNotify& operator=(const CDevPfmInfoNotify& cPfmInfoNotify);
	void Copy(const CDevPfmInfoNotify* pcPfmInfoNotify);


private:
	u16 m_wNum; //性能个数，取buffer时转换为网络序
	TPfmInfoBase m_atPfmInfo[KDV_PFMINFO_BUFFER_MAX]; //设备基本性能信息buffer
};

#ifdef WIN32
#ifdef PACKED
#pragma pack(pop)
#endif
#undef PACKED
#endif //WIN32

#endif
