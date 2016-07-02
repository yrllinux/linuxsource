/*****************************************************************************
   模块名      : KDVSNMPMANAGER
   文件名      : smmockmachineinfo.h
   相关文件    : smmockmachineinfo.cpp
   文件实现功能: 插件的架构图消息体设计
   作者		   : 陆昆朋
   版本        : V5.0  Copyright(C) 2009-2012 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本     修改人      修改内容
   2009/12/31  5.0      陆昆朋      创建
******************************************************************************/
#ifndef KDV_SMMOCKMACHINEINFO_H
#define KDV_SMMOCKMACHINEINFO_H
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


//以下结构用于传输，网络序
//对应的层槽信息，也是相应的板卡信息
typedef struct tagLayerSlotInfo
{
	u32	m_dwBoardIp;		// 板卡的ip地址
	u8	m_byLayerIndex;		// 单板的层索引
	u8	m_bySlotIndex;		// 单板的槽位索引
	u8	m_byBoardType;		// 板卡类型
	u8	m_byLinkStatus;		// 单板的链路状况
	u8  m_byOsType;			// OS类型
	s8	m_szPanelLed[MAX_LED_STATUS_LEN];	//单板的信号灯情况
	s8  m_szSoftwareVersion[MAX_SOFTWARE_VER_LEN];//单板软件版本
}PACKED TLayerSlotInfo;

//以下结构用于传输，网络序
//板卡信息管理的头
typedef struct tagMockMachineInfoHead
{
	u8	m_byLayerAmount;		// 框件的层数，不包括电源
	u8	m_bySlotAmountOfLayer;	// 单层的槽位个数
	u16	m_wBoardNum;			// 构件中单板的数量(实际插入的)
}PACKED TMockMachineInfoHead;

/*====================================================================
类名  : CMockMachineInfo
功能  : 机架图相关信息处理类
注    :
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
1/11/2010   5.0			陆昆朋			增加
====================================================================*/
class CMockMachineInfo
{
public:
	//构造函数
	CMockMachineInfo();

	//此函数仅仅用于网络接受到buffer后的解析
	CMockMachineInfo(const s8* pszBuffer, u32 dwSize);
	virtual ~CMockMachineInfo();
	
public:
	
	//层数的获取和设置
	void SetLayerAmount(u8 byLayerNum);
	u8 GetLayerAmount();

	//每层的槽数的获取和设置
	void SetSlotNumOfLayer(u8 bySlotNum);
	u8 GetSlotAmountOfLayer();

	//获得单板的个数,返回主机序
	u16 GetBoardNum();

	//此函数是检索层槽相等，比较耗时，如果知道dwIndex,请直接用索引获取。
	//byLayerID层区间0 - 3
	//bySlotID 槽区间0 - 15
	//返回存储为网络序
	const TLayerSlotInfo* GetLayerSlotInfo(u8 byLayerID, u8 bySlotID);

	//dwIndex 索引区间0 - 63
	//返回存储为网络序
	const TLayerSlotInfo* GetLayerSlotInfo(u32 dwIndex);

	//tLayerSlotInfo为网络序
	BOOL32 AddBoard(const TLayerSlotInfo& tLayerSlotInfo);

	//清除单板信息m_tMachineInfoHead.m_wBoardNum = 0
	void ClearBoardInfo();

	//此函数仅仅用于网络传输的封装buffer
	virtual u32 GetBuffer(s8 *pszBuffer, u32 dwBufferSize);

protected:
	//构造函数直接初始化，禁用
	CMockMachineInfo(CMockMachineInfo&);
	//以下为操作符等于，内部实现，防止外部调用。
	CMockMachineInfo& operator=(const CMockMachineInfo& cPfmInfo);
	void Copy(const CMockMachineInfo* pcPfmInfo);

protected:
	TMockMachineInfoHead m_tMachineInfoHead; //mcu
	TLayerSlotInfo m_atLayerSlotInfo[SM_MCU_LAYER_SLOT_MAX];
}PACKED;

#ifdef WIN32
#ifdef PACKED
#pragma pack(pop)
#endif
#undef PACKED
#endif //WIN32

#endif
