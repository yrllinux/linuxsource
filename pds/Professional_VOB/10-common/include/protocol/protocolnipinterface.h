/*******************************************************************************
 *  模块名   :                                                                 *
 *  文件名   : protocolnipinterface.h                                          *
 *  相关文件 :                                                                 *
 *  实现功能 : 协议组提供的对NIP的封装层，以指针函数的形式提供                 *
 *	引用该库的每条流的APP必须实现下面的函数                                    *
 *  作者     : 翟小刚                                                          *
 *  版本     : V1.0  Copyright(c) 1997-2005 KDC, All rights reserved.          *
 *                                                                             *
 *    =======================================================================  *
 *  修改记录:                                                                  *
 *    日  期      版本        修改人      修改内容                             *
 *  28/5/2010     4.0         翟小刚      创建                                 *
 *                                                                             *
 *******************************************************************************/

#ifndef protocolnipinterface_h__
#define protocolnipinterface_h__

#include "osp.h"

//注册GK需要使用的底层库函数
void InitGKNipFun();

extern s8* ProtocolGKGetConfigPath();

/////GKLIB中的函数, 使用的话，需要连接kdvlib.lib
/*====================================================================
  函 数 名： ProtocolBrdGetEthState
  功    能： 
  算法实现： 
  全局变量： 
  参    数： byIndex: 网口索引， 暂时无意义
			 dwip	: 网口的IP， 暂时无意义
  返 回 值： BOOL 当前的链接状态，此函数目前恒返回TRUE
  --------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2010/5/28     4.0		    翟小刚                   创建
======================================================================*/
extern BOOL32 ProtocolBrdGetEthState(u8 byIndex, u32 dwip);


/*====================================================================
  函 数 名： ProtocolSetSecondIpAddress
  功    能： 设定网口第二个Ip，如果网口没有第一个IP，则会配置成第一个IP
             配置成功后，会进行点灯操作 BrdLedStatusSet( LED_SYS_LINK, BRD_LED_ON );
  算法实现： 
  全局变量： 
  参    数： byIndex:  网口索引
			 dwIp	:  需要配置的IP  
			 dwMask	:  对应的掩码
			 bIsUseSecondIp:  出参， 表示是否设置网口的第二个IP成功，如果不成功则为FALSE
  返 回 值： BOOL 
  --------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2010/5/28     4.0		    翟小刚                   创建
======================================================================*/
extern BOOL32 ProtocolSetSecondIpAddress(u8 byIndex, u32 dwIp, u32 dwMask, BOOL32& bIsUseSecondIp);

/*====================================================================
  函 数 名： ProtocolDeleteSecondIpAddress
  功    能： 删除网口第二个IP，成功之后会进行关灯操作 BrdLedStatusSet( LED_SYS_LINK, BRD_LED_OFF );
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： BOOL 
  --------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2010/5/28     4.0		    翟小刚                   创建
======================================================================*/
extern BOOL32 ProtocolDeleteSecondIpAddress(u8 byIndex, u32 dwIp, u32 dwMask, BOOL32 bIsSecond);



extern void	ProtocolBrdHwReset(void);
extern BOOL32	ProtocolIsMdscORHdsc();
extern u32		ProtocolBrdGetEthParam(u8 byIndex);






//pxyserver中的函数，使用的话需要连接pxyserver

//注册pxy需要使用的底层库函数
void InitPxyNipFun();

/* 单个路由参数结构 */
typedef struct{
    u32 dwDesIpNet;      /* 目的子网，网络字节序	*/
    u32 dwDesIpMask;     /* 掩码，网络字节序	*/
    u32 dwGwIpAdrs;      /* 网关ip地址，网络字节序*/
    u32 dwRoutePri;		/* 路由优先级(1-255)，默认为0*/
}TPxyIpRouteParam;//TBrdIpRouteParam;


extern char* ProtocolPxyGetConfigPath(void);

extern BOOL32 ProtocolIsInSameSubnet(u32 dwSrcIP);
extern int ProtocolPxyAddOneIpRoute(TPxyIpRouteParam *ptBrdIpRouteParam);
extern int ProtocolPxyDelOneIpRoute(TPxyIpRouteParam *ptBrdIpRouteParam);
extern int ProtocolPxySetRunStatusLed();
extern int ProtocolGkSetRunStatusLed();
extern int ProtocolInterworkingSetRunStatusLed();


//add by yj for BrdType [20130227]
typedef enum
{
	nipInter_BrdType_Unknown,
	nipInter_BrdType_WIN32,
	nipInter_BrdType_NOTSINGLEBRD,
	nipInter_BrdType_CRI,
	nipInter_BrdType_CRI2,
	nipInter_BrdType_IS21,
	nipInter_BrdType_IS22
} nipInterBrdType;

/*====================================================================
  函 数 名： ProtocolGetBrdType
  功    能： 通过调用驱动接口获取板卡类型
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： nipInterBrdType 板卡类型
  --------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2013/2/28     4.2		    杨俊                   创建
======================================================================*/
extern nipInterBrdType ProtocolGetBrdType();

/*====================================================================
  函 数 名： ProtocolSingleBrdUpdate
  功    能： 通过调用驱动接口升级单板
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： TRUE-成功，FALSE-失败
  --------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2013/2/28     4.2		    杨俊                   创建
======================================================================*/
extern BOOL32 ProtocolSingleBrdUpdate();
//end [20130227]

#endif//protocolnipinterface_h__


