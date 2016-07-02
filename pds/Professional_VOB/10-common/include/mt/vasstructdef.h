/*****************************************************************************
   模块名      : VAS
   文件名      : VasEventDef.h
   相关文件    : 
   文件实现功能: 结构定义
   作者        : 
   版本        : 
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容

******************************************************************************/
#ifndef _VAS_STRUCT_DEF_H
#define _VAS_STRUCT_DEF_H

#include "eventid.h"

//===========================================
//常量定义
//=============================================
//定义矩阵最大输入通道数
#define MAXNUB_AVCHAN    16
//定义通道名的长度
#define MAXLEN_CHANNAME  16


//===========================================
//结构定义
//=============================================
#ifdef WIN32
#pragma pack( push )
#pragma pack(1)
#define PACKED 
#else
#define PACKED __attribute__((__packed__))	// 取消编译器的优化对齐
#endif // WIN32

typedef struct tagVasSwitcher
{
	u8 m_byMediaType;//媒体种类
	u8 m_byInputChann;//输入通道号
	u8 m_byOutputChann;//输出通道号
}PACKED TVasSwitcher;

//启动时参数配置
typedef struct tagVasStart
{    
	u32  m_dwIP;;//IP地址
	u16 m_wPort;//端口
	u8 m_byInputNumber;//输入源数目
}PACKED TVasStart;

//轮询设置
typedef struct tagVasRolling
{    
	u8 m_byMediaType;//媒体种类
	u8 m_byBeginChann;//开始通道号
	u8 m_byEndChann;//结束通道号
	u8 m_byoutputChann;//输出通道号
	u16 m_dwTime;//间隔时间
	
}PACKED TVasRolling;

//请求配置方案
typedef struct VasConfiguAsk
{    
	u8 m_byMediaType;//媒体种类
	u8 m_byIndex;//方案索引号
	
}PACKED TVasConfiguAsk;

//配置方案
typedef struct tagVasConfigure
{
	u8 m_abyOutputChan[MAXNUB_AVCHAN];//各输出通道所对应的输入通道//如m_abyOutputChan[3]=7 表示 第4输出通道连接到第8输入通道
	u8 m_abyInputChanName[MAXNUB_AVCHAN][MAXLEN_CHANNAME];//输入通道名
	u8 m_abyOutputChanName[MAXNUB_AVCHAN][MAXLEN_CHANNAME];//输出通道名
	
	u8 m_byConfigureID;//配置方案索引号
	u8 m_byMediaType;//媒体种类

}PACKED TVasConfigu,TVasConfig;



#ifdef WIN32
#pragma pack( pop )
#endif // WIN32

#endif // _VAS_STRUCT_DEF_H



