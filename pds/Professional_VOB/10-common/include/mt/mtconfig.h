/*******************************************************************************
 *  模块名   : 系统配置                                                        *
 *  文件名   : SysConfg.h                                                      *
 *  相关文件 : SysConfg.cpp                                                    *
 *  实现功能 : 系统参数读取、设置                                              *
 *  作者     : 张 飞                                                           *
 *  版本     : V4.0  Copyright(c) 1997-2005 KDC, All rights reserved.          *
 *                                                                             *
 *    =======================================================================  *
 *  修改记录:                                                                  *
 *    日  期      版本        修改人      修改内容                             *
 *  2005/8/5  1.0         张 飞      创建          *
 *                                                                             *
 *******************************************************************************/


#ifndef MTCONFG_H
#define  MTCONFG_H

#include "mtconst.h"
#include "mttype.h"
#include "mtstruct.h"
#include "mtconfigex.h"
#include "addrbook.h"

//	参数配置接口
//	段加载方式表
//本数据结构描述了，
//本文中各种配置项在系统启动时是何加载的，
//当相应的Bit位置1表明是通过文件加载，
//否则表明该字段并未存在于配置文件中而是由系统产生的默认值。
struct tagTConfigKeySrc
{
	u32 Language : 1;
	u32 Ethnet   : 1;
	u32 H323     : 1;
	u32 H320     : 1;
	u32 SIP      : 1;
	u32 Aliase   : 1;
	u32 E164     : 1;
	u32 GK       : 1;
	u32 H239     : 1;
	u32 UserPWD  : 1;
	u32 NetPWD   : 1;
	u32 UserPWDEnable     : 1;
	u32 NetPWDEnable      : 1;
	u32 EncryptArithmetic : 1;
};
typedef struct tagTConfigKeySrc TConfigKeySrc ;





//系统初始化函数
BOOL InitCfg(void);

//退出系统
void QuitCfg(void);

// 以下新加读写配置文件的函数接口, 供mtb界面使用

// 系统初始化函数 
BOOL MtbClientInitCfg(const s8* szFileName, EmMtModel emMtModel);

// 设置内存信息为默认值函数
BOOL MtbClientSetDefaultConfigData(EmMtModel emMtModel);

// 保存内存信息到配置文件函数
BOOL MtbClientSaveConfigData(const s8* szFileName);

// 加载配置文件信息到内存函数
BOOL MtbClientLoadConfigData(const s8* szFileName);

// 读配置文件保存到内存同时发送配置文件更新消息 FIXME: 判断是否需要先进行初始化操作
u8 ReadAndSaveDataToCfgFile(const s8* szFileName);

//获取版本语言信息
BOOL mtGetLanguageInfo(EmLanguage& emLanguage);
//设置版本语言设置
BOOL mtSetLanguageInfo(EmLanguage emLanguage);

//获取用户是否启用用户密码
BOOL mtGetUserPassEnable(BOOL& bEnable);

//设置用户是否启用用户密码
BOOL mtSetUserPassEnable(BOOL bEnable);

//获取用户密码
BOOL mtGetUserPass(s8 achPass[]);

//设置用户密码
BOOL mtSetUserPass(const s8 achPass[]);

//获取用户是否启用网络密码
BOOL mtGetNetPassEnable(BOOL& bEnable);

//设置用户是否启用网络密码
BOOL mtSetNetPassEnable(BOOL bEnable);

//获取用户网络密码
BOOL mtGetNetPass(s8 achPass[]);

//设置用户网络密码
BOOL mtSetNetPass(const s8 achPass[]);

//用来标识该项是否是由文件加载的
BOOL mtGetConfigKeySrc( TConfigKeySrc& tConfigKeySrc);

//	网络配置接口
//IP 服务质量(IP Quality of Service）)
//IP QoS是指IP的服务质量，也是指IP数据流通过网络时的性能。
//它的目的就是向用户提供端到端的服务质量保证。
//目前终端IP QOS提供两种类型的参数配置即 区分服务(DiffServ)、IP优先。
//通过本组API可以对视频会议中的信令、数据、音频、视频设置QoS。
//获取终端Qos信息，值存储在tIPQos结构中
//仅当获取访问信号量超时返回FALSE;
BOOL  mtGetIPQoS (TIPQoS&  tIPQoS );

//设置终端Qos值
//仅当获取访问信号量超时返回FALSE;
BOOL  mtSetIPQos(const TIPQoS&  tIPQoS );

// 获取、是指以太网配置
//获取终端以太网配置，值存储在ptTEthnetInfo结构中
// 仅当获取访问信号量超时返回FALSE;
BOOL mtGetEthnet(u8 byIndex, TEthnetInfo& tTEthnetInfo );

//配置终端以太网配置，值存储在ptTEthnetInfo结构中
//仅当获取访问信号量超时返回FALSE;
//参数说明：
BOOL mtSetEthnet(u8 byIndex, const TEthnetInfo& tTEthnetInfo);


/******* 获取,保存WiFi网络配置 **********/

//获取终端WiFi网络配置，值存储在tWiFiCfg结构中
//仅当获取访问信号量超时返回FALSE;
BOOL mtGetWiFiNetCfgInfo( TWiFiNetCfg& tWiFiCfg );

//保存终端WiFi网络配置
//仅当获取访问信号量超时返回FALSE;
BOOL mtSetWiFiNetCfgInfo( const TWiFiNetCfg& tWiFiCfg );

//保存终端WiFi BSSID配置
//仅当获取访问信号量超时返回FALSE;
BOOL mtSetWiFiBSSIDCfgInfo(const TWiFiBSSIDCfg tWiFiBSSIDCfg[] );

//获取终端WiFi BSSID配置，值存储在TWiFiBSSIDCfg结构中
//仅当获取访问信号量超时返回FALSE;
BOOL mtGetWiFiBSSIDCfgInfo( TWiFiBSSIDCfg tWiFiBSSIDCfg[] );

//E1配置
//获取终端E1配置，值存储在tE1Cfg结构中
//仅当获取访问信号量超时返回FALSE;
BOOL  mtGetE1Cfg(TE1Config& tE1Cfg );

//配置终端E1配置，值存储在tE1Cfg结构中
//仅当获取访问信号量超时返回FALSE;
BOOL  mtSetE1Cfg(const TE1Config& tE1Cfg);
  
//路由配置
//获取终端路由配置，值存储在aptTRouteCfg结构数组中，
//仅当获取访问信号量超时返回FALSE;
BOOL  mtGetRoute(TRouteCfg aptTRouteCfg[]);

//配置终端路由配置，值存储在aptTRouteCfg结构中,
//仅当获取访问信号量超时返回FALSE;
BOOL  mtSetRoute(const TRouteCfg aptTRouteCfg[]);

//内置代理的ip地址配置
//获取代理的ip
BOOL mtGetPxyIP( TPxyIPCfg aptTPxyIPCfg[] );
//设置代理的ip
BOOL mtSetPxyIP(const TPxyIPCfg aptTPxyIPCfg[]);



// PPPOE配置
//获取终端PPPOE配置，值存储在tPPPOECfg结构中
//仅当获取访问信号量超时返回FALSE;
BOOL  mtGetPPPOE(TPPPOECfg& tPPPOECfg );

//配置终端PPPOE配置，值存储在tPPPOECfg结构中
//仅当获取访问信号量超时返回FALSE;
BOOL  mtSetPPPOE(const TPPPOECfg &tPPPOECfg );

//串口配置(耿昌明要求加串口默认值)
//根据串口类型获取终端形影串口的配置参数，
//串口的配置参数存储于 tSerialCfg的结构中
//仅当获取访问信号量超时返回FALSE;
BOOL mtGetSerialCfg( EmSerialType ,TSerialCfg& tSerialCfg );

//根据串口类型设置终端串口的配置参数，
//串口的配置参数存储于 tSerialCfg的结构中。
//仅当获取访问信号量超时返回FALSE;
BOOL mtSetSerialCfg( EmSerialType ,const TSerialCfg& tSerialCfg );


//终端信息设置
//获取设置终端别名
//终端别名提供终端一种标识，
//在H323会议系统中也被用做H323Id别名
//获取终端别名，别名存储于 abyAliase 的零结尾符字串。
//仅当获取访问信号量超时返回FALSE;
BOOL mtGetAliase( s8 abyAliase[] );

//设置终端别名，别名存储于 abyAliase 的零结尾符字串。
//仅当获取访问信号量超时返回FALSE;
BOOL mtSetAliase( const s8 abyAliase[] );

// 获取设置终端E164号码
//H323会议系统中别名由 "0123456789#*," 组成
//获取终端E164号码，号码存储在abyE164中
//仅当获取访问信号量超时返回FALSE;
BOOL mtGetE164( s8 abyE164[] );

//设置终端E164号码，号码存储在abyE164中
//仅当获取访问信号量超时返回FALSE;
BOOL mtSetE164( const s8 abyE164[] );

//是否允许远摇
BOOL mtIsRemoteCtrol(BOOL& Enable);
//是否允许远摇
BOOL mtSetRemoteCtrol(BOOL Enable);

//是否允许待机设置
BOOL mtIsSleepEnable(BOOL& Enable);
//是否允许待机设置
BOOL mtSetSleepEnable(BOOL Enable);

//待机时间
BOOL mtGetSleepTimes(u16& Enable);
//待机时间
BOOL mtSetSleepTimes(u16 Enable);

//获取DHCP
BOOL  mtGetDHCP(BOOL& val);
//设置DHCP
BOOL  mtSetDHCP(BOOL val);



//是否启用H323
//该选项指明终端是否启动H323业务，
//在终端中H323、H320、SIP通讯协议必须选用一个
//获取终端是否启动H323业务，是否启动H323业务存储在Enable中
//仅当获取访问信号量超时返回FALSE;
BOOL   mtIsH323Enable(BOOL& Enable );

//设置终端是否启动H323业务，是否启动H323业务存储在Enable中
// 仅当获取访问信号量超时返回FALSE;
BOOL   mtH323Enable(BOOL Enable);

//是否启用H320服务
//该选项指明终端是否启动H320业务，
//在终端中H323、H320、SIP通讯协议必须选用一个
//获取终端是否启动H320业务，是否启动H320业务存储在Enable中
//仅当获取访问信号量超时返回FALSE;
BOOL   mtIsH320Enable(BOOL& Enable);

//设置终端是否启动H320业务，是否启动H320业务存储在Enable中
//仅当获取访问信号量超时返回FALSE;
BOOL   mtH320Enable(BOOL Enable);

// 是否启用SIP服务
//该选项指明终端是否启动SIP业务，
//在终端中H323、H320、SIP通讯协议必须选用一个
//获取终端是否启动SIP业务，是否启动SIP业务存储在Enable中
//仅当获取访问信号量超时返回FALSE;
BOOL   mtIsSIPEnable(BOOL& Enable);

//设置终端是否启动SIP业务，是否启动SIP业务存储在Enable中
// 仅当获取访问信号量超时返回FALSE;
BOOL   mtSIPEnable(BOOL Enable);

//是否启用H239
//该选项指明终端是否启动H239业务
//获取终端是否启动SIP业务，是否启动SIP业务存储在Enable中
//仅当获取访问信号量超时返回FALSE;
BOOL  mtIsUseH239(BOOL& Enable);

//设置终端是否启动SIP业务，是否启动SIP业务存储在Enable中
//仅当获取访问信号量超时返回FALSE;
BOOL  mtSetUseH239(BOOL  Enable);

// 会议加密
//该选项指明终端是否启动会议加密
//获取终端是否启动动会议加密，是否启动会议加密存储在Enable中
//仅当获取访问信号量超时返回FALSE;
BOOL mtIsEncrypt(BOOL& Enable);

//设置终端是否启动会议加密，是否启动会议加密存储在Enable中
//仅当获取访问信号量超时返回FALSE;
BOOL mtSetEncrypt(BOOL  Enable);
//加密算法
//获取终端加密算法，加密算法存储在emModel中
//仅当获取访问信号量超时返回FALSE;
BOOL  mtGetEncryArithmetic(EmEncryptArithmetic& emModel);

//设置终端加密算法，是否启动会议加密算法
//仅当获取访问信号量超时返回FALSE;
BOOL  mtSetEncryArithmetic(EmEncryptArithmetic emModel);


//获取呼叫码率选择
BOOL  mtGetSelectCallRate(u32& dwVal);
//设置呼叫码率选择
BOOL  mtSetSelectCallRate(u32 dwVal);

// 内嵌防火墙代理配置
//启用内嵌防火墙代理
//获取终端防火墙代理参数，防火墙代理参数存储在tNatProxy中
//仅当获取访问信号量超时返回FALSE;
BOOL mtGetEmbedFwNatProxy(TEmbedFwNatProxy& tNatProxy );

//设置防火墙代理参数 
//仅当获取访问信号量超时返回FALSE;
BOOL mtSetEmbedFwNatProxy(const TEmbedFwNatProxy& tNatProxy);

// 静态NAT映射
//获取终端静态NAT地址映射，静态NAT地址映射参数存储在dwAddr中
//仅当获取访问信号量超时返回FALSE;
BOOL mtGetStaticNatAddr(TNATMapAddr&  tAddr);

//设置终端静态NAT地址映射 
//仅当获取访问信号量超时返回FALSE;
BOOL mtSetStaticNatAddr(const TNATMapAddr& tAddr);

//监控业务器换
//获取终端监控业务切换参数，监控业务切换参数存储在tMonitorService中
//仅当获取访问信号量超时返回FALSE;
BOOL mtGetMonitorSerivce(TMonitorService& tMonitorService );

//设置终端监控业务切换参数
//仅当获取访问信号量超时返回FALSE;
BOOL mtSetMonitorService(const TMonitorService& tMonitorService );

// 获取设置GK地址
//获取终端GK地址，GK地址存储在dwGKIP中
//仅当获取访问信号量超时返回FALSE;
BOOL mtGetGKIP(TGKCfg& tGKIP);

//设置终端GK地址
//仅当获取访问信号量超时返回FALSE;
BOOL mtSetGKIP(const TGKCfg& tGKIP );

//设置终端内置GK信息
//仅当获取访问信号量超时返回FALSE;
BOOL mtSetInnerGKInfo(const TInnerGKCfg& tInnerGKCfg);

//获取终端内置GK信息
//仅当获取访问信号量超时返回FALSE;
BOOL mtGetInnerGKInfo(TInnerGKCfg& tInnerGKCfg);

//设置终端内置代理信息
BOOL mtSetInnerProxyInfo( const TInnerProxyCfg& tInnerProxyCfg );
//获取终端内置代理信息
BOOL mtGetInnerProxyInfo( TInnerProxyCfg& tInnerProxyCfg );

//	呼叫设置
//获取、设置应答方式
//获取终端应答模式，应答模式存储在Modle中
//仅当获取访问信号量超时返回FALSE;
BOOL mtGetAnswerMode(EmTripMode& Modle);

//设置终端应答模式
//仅当获取访问信号量超时返回FALSE;
BOOL mtSetAnserMode(EmTripMode Modle) ;

// 是否开启自动呼叫
//获取终端是否启用自动呼叫模式，值存储在bEnable中
//仅当获取访问信号量超时返回FALSE;
BOOL mtIsAutoCallEnable(BOOL& bEnable);

//设置终端是否启用自动呼叫模式
//仅当获取访问信号量超时返回FALSE;
BOOL mtEnableAutoCall(BOOL  bEnable);


// 自动呼叫终端别名
//获取终端自动呼叫的终端别名，值存储在abyAlias中
//仅当获取访问信号量超时返回FALSE;
BOOL  mtGetAutoCallMtAlias(s8 abyAlias[]);

//设置终端自动呼叫的终端别名
//仅当获取访问信号量超时返回FALSE;
BOOL  mtSetAutoCallMtAlias (const s8 abyAlias[] );


// 自动呼叫码率
//获取终端自动呼叫的码率，值存储在wRate中
//仅当获取访问信号量超时返回FALSE;
BOOL mtGetAutoCallBitrate(u16& wRate);

//设置终端自动呼叫的码率
//仅当获取访问信号量超时返回FALSE;
BOOL mtSetAutoCallBitrate(u16 wRate );

// 设置通信起始端口号
//获取终端设置通信UDP起始发送端口号，值存储在wPort中
//仅当获取访问信号量超时返回FALSE;
//BOOL mtGetUDPSndBasePort(u16& wPort );
//设置终端设置通信UDP起始发送端口号
//仅当获取访问信号量超时返回FALSE;
//BOOL mtSetUDPSndBasePort(u16  wPort);
//获取终端设置通信UDP接收起始端口号，值存储在wPort中
//仅当获取访问信号量超时返回FALSE;
//BOOL  mtGetUDPRcvBasePort(u16 & wPort);
//设置终端设置通信UDP接收起始端口号
//仅当获取访问信号量超时返回FALSE;
// BOOL  mtSetUDPRcvBasePort(u16 wPort);

//获取终端设置通信UDP起始端口号，值存储在wPort中
//仅当获取访问信号量超时返回FALSE;
BOOL mtGetUDPBasePort(u16& wPort);
//设置终端设置通信UDP起始端口号
//仅当获取访问信号量超时返回FALSE;
BOOL mtSetUDPBasePort(u16  wPort);

//获取终端设置通信TCP基准端口号，值存储在wPort中
//仅当获取访问信号量超时返回FALSE;
BOOL mtGetTCPBasePort(u16& wPort);

//设置终端设置通信TCP基准端口号
//仅当获取访问信号量超时返回FALSE;
BOOL mtSetTCPBasePort(u16 wPort);

// 链路检测时间
//单位秒
//获取终端链路检测时间，值存储在wTimeOut中
//仅当获取访问信号量超时返回FALSE;
BOOL mtGetLinkKeepLiveTimeOut(u16& wTimeOut);

//设置终端链路检测时间
//仅当获取访问信号量超时返回FALSE;
BOOL mtSetLinkKeepLiveTimeOut(u16 wTimeOut );

// 网络管理配置
// 获取设置告警服务器地址
//获取终端告警服务器的地址，值存储在dwIP中
//仅当获取访问信号量超时返回FALSE;
BOOL mtGetTrapServerIP(u32& dwIP );

//设置终端告警服务器的地址
//仅当获取访问信号量超时返回FALSE;
BOOL mtSetTrapServerIP(u32 dwIP);

// 获取共同体名
//获取终端的共同体名，值存储在abyName中
//仅当获取访问信号量超时返回FALSE;
BOOL mtGetNMPublicName( s8 abyName[]);

//设置终端的共同体名
//仅当获取访问信号量超时返回FALSE;
BOOL mtSetNMPublicName(const  s8 abyName[] );

//	显示设置
// 自动画中画功能配置
//获取终端是否启用自动画中画，值存储在bAuto中
//仅当获取访问信号量超时返回FALSE;
BOOL mtIsAutoPIP(BOOL&  bAuto); 

//设置终端是否启用自动画中画
//仅当获取访问信号量超时返回FALSE;
BOOL mtSetAutoPIP(BOOL  bAuto);

// 会议进行时间显示功能配置
//获取终端是否启用会议进行时间显示功能，值存储在bShow中
//仅当获取访问信号量超时返回FALSE;
BOOL mtIsShowConfLapse(BOOL& bShow);

//设置终端是否启用会议进行时间显示功能
//仅当获取访问信号量超时返回FALSE;
BOOL mtSetShowConfLaps(BOOL  bShow);

// 台标显示方式设置
//这个函数的取值范围是一个三态值，
//再次只是借用了EmTripMode的三态值，但所含意义并不相同
//获取终端台标显示方式，值存储在emMode中
//仅当获取访问信号量超时返回FALSE;
BOOL mtGetSymbolShowMode(EmLabelType& emMode);

//设置终端台标显示方式
//仅当获取访问信号量超时返回FALSE;
BOOL mtSetSymbolShowMode(EmLabelType emMode);

//获取状态显示
BOOL mtIsShowState(BOOL& bShow);
//获取状态显示
BOOL mtSetShowState(BOOL bShow);

//获取状态显示
BOOL mtGetIsVGAShowLocal(BOOL& bShow);
//获取状态显示
BOOL mtSetIsVGAShowLocal(BOOL bShow);

// 台标显示位置
//获取台标左上角得坐标位置注:  16 ≤ X ≤ 600  ; 8 ≤ Y ≤ 500
// 设置台标左上角得坐标位置，值存储在tPos中
BOOL mtGetSymbolXY(TSymboPoint& tPos);

// 设置台标左上角得坐标位置注:  16 ≤ X ≤ 600  ; 8 ≤ Y ≤ 500
BOOL mtSetSymbolXY(const TSymboPoint& tPos);
//设置横幅显示参数
BOOL mtGetBannerInfo(TBannerInfo& tInfo);
//获取横幅显示参数
BOOL mtSetBannerInfo(const TBannerInfo& tInfo);
//双流显示方式设置（没有找到默认值,健名改变，已修改）
//获取终端双流显示方式设置，值存储在emMode中
//仅当获取访问信号量超时返回FALSE;
BOOL  mtGetDualVideoShowMode(EmDualVideoShowMode& emMode);

//设置终端双流显示方式设置
BOOL  mtSetDualVideoShowMode(EmDualVideoShowMode emMode);

//获取显示比例模式
BOOL  mtGetDisplayRatio(EmDisplayRatio& emDR);
//设置显示比例模式
BOOL  mtSetDisplayRatio(EmDisplayRatio emDR);

// 系统时间显示设置（没有找到默认值,健名改变，已修改）
//获取终端系统时间显示设置，值存储在bShow中
//仅当获取访问信号量超时返回FALSE;
BOOL mtIsShowSysTime(BOOL& bShow);

//设置终端系统时间显示设置
//仅当获取访问信号量超时返回FALSE;
BOOL mtShowSysTime(BOOL  bShow);

//获取是否发送静态图片选项
BOOL mtGetSendStaticPic( BOOL &bSend );

//设置是否发送静态图片选项
BOOL mtSetSendStaticPic( BOOL bSend );

//视频设置
// 视频优选策略
//获取终端视频的编码格式，值存储在emFormat中，emType为视频类型
//仅当获取访问信号量超时返回FALSE;
BOOL  mtGetVideoFormatPriorStrategy(EmVideoType emType, EmVideoFormat& emFormat);

//设置终端视频的编码格式，emType为视频类型
//仅当获取访问信号量超时返回FALSE;
BOOL  mtSetVideoFormatPriorStrategy(EmVideoType emType, EmVideoFormat emFormat);

//获取终端视频的图像分辨率，值存储在emFormat中，emType为视频类型
//仅当获取访问信号量超时返回FALSE;
BOOL  mtGetVideoResolvePriorStrategy(EmVideoType emType, EmVideoResolution& emFormat);

//设置终端视频的图像分辨率，emType为视频类型
//仅当获取访问信号量超时返回FALSE;
BOOL  mtSetVideoResolvePriorStrategy(EmVideoType emType, EmVideoResolution emFormat);


// 获取、设置视频制式设置（未完成）
//获取终端输出视频的制式设置，值存储在emStand中，emType为视频类型
//仅当获取访问信号量超时返回FALSE;
BOOL  mtGetOutVideoStandard(EmVideoType emType, EmVideoStandard& emStand );

//设置终端输出视频的制式设置emType为视频类型
//仅当获取访问信号量超时返回FALSE;
BOOL  mtSetOutVideoStandard(EmVideoType emType  ,EmVideoStandard  emStand);

//获取终端输入视频的制式设置，值存储在emStand中，emType为视频类型
//仅当获取访问信号量超时返回FALSE;
BOOL  mtGetInVideoStandard(EmVideoType emType , EmVideoStandard& emStand );

//设置终端输入视频的制式设置emType为视频类型
//仅当获取访问信号量超时返回FALSE;
BOOL  mtSetInVideoStandard(EmVideoType emType  ,EmVideoStandard  emStand);

//输入视频源配置

//获取终端视频源的输入方式，
//emVideoType用来描述视频的类别（第一路、第二路、其他），
//值存储在tVideoSource中，
//只有TvideoSource.emVideoInterface 
//为c端子的时候TvideoSource.byCPortIndex才有意义
//仅当获取访问信号量超时返回FALSE;
BOOL mtGetVideoSource(EmVideoType emVideoType , EmMtVideoPort& tVideoSource);

//设置终端视频源的输入方式，
//emVideoType用来描述视频的类别（第一路、第二路、其他），
//值存储在tVideoSource中
//仅当获取访问信号量超时返回FALSE;
BOOL mtSetVideoSource(EmVideoType emVideoType , EmMtVideoPort  tVideoSource);

//获取终端画面的质量参数，值存在tImageParam
//仅当获取访问信号量超时返回FALSE;
BOOL mtGetImageAdjustParam(EmVideoType emVideoType , TImageAdjustParam& tImageParam);

//设置终端画面的质量参数
//仅当获取访问信号量超时返回FALSE;
BOOL mtSetImageAdjustParam(EmVideoType emVideoType , const TImageAdjustParam& tImageParam);

//流媒体
//设置、获取流媒体配置（新加项，没有默认值，已处理）
//获取终端流媒体参数，值存在tStream
//仅当获取访问信号量超时返回FALSE;
BOOL mtGetStreamMedia(TStreamMedia& tStream );

//设置终端流媒体参数，值存在tStream
//仅当获取访问信号量超时返回FALSE
BOOL mtSetStreamMedia(const TStreamMedia& tStream );

//摄像头配置
//获取、设置摄像头配置(摄像头可能超过一个，已处理)
//获取终端摄像机配置信息，值存在tCfg
//仅当获取访问信号量超时返回FALSE;
BOOL mtGetCameraCfg( u8 byIndex,TCameraCfg& atCfg );

//设置终端摄像机配置信息，值存在tCfg
//仅当获取访问信号量超时返回FALSE;
BOOL mtSetCameraCfg( u8 byIndex,const TCameraCfg& atCfg );


//	音频
// 音频调整参数（新加项，没有默认值，已处理）
//设置输入音量 
//获取终端输入音量的相对值，值存储在wVal中
//仅当获取访问信号量超时返回FALSE;
BOOL  mtGetInputVolume(u8&  wVal);

//设置终端输入音量的相对值
//仅当获取访问信号量超时返回FALSE;
BOOL  mtSetInputVolume(u8   wVal);

// 设置输出音量
//获取终端输出音量的相对值，值存储在wVal中
//仅当获取访问信号量超时返回FALSE;
BOOL  mtGetOutputVolume(u8& wVal);

//设置终端输出音量的相对值
//仅当获取访问信号量超时返回FALSE;
BOOL  mtSetOutputVolume(u8 wVal);

//启用回声抵消（新加项，没有默认值，已处理）
//获取终端是否启用回声抵消，值存储在bEnable中
//仅当获取访问信号量超时返回FALSE;
BOOL  mtIsAECEnable(BOOL& bEnable);

//设置终端是否启用回声抵消
//仅当获取访问信号量超时返回FALSE;
BOOL  mtEnableAEC(BOOL  bEnable);

//自动增益控制（新加项，没有默认值，已处理）
//获取终端是否启用自动增益控制，值存储在bEnable中
//仅当获取访问信号量超时返回FALSE;
BOOL  mtIsAGCEnable(BOOL& bEnable);

//设置终端是否启用自动增益控制
//仅当获取访问信号量超时返回FALSE;
BOOL  mtEnableAGC(BOOL  bEnable);

//
//自动噪音抑制的功能
BOOL mtEnableANS( BOOL bEnable );
BOOL mtIsANSEnable( BOOL &bEnable );


//获取终端音频的编码格式，值存储在emFormat中
//仅当获取访问信号量超时返回FALSE;
BOOL  mtGetAudioFormatPriorStrategy(EmAudioFormat&  emFormat);

//设置终端音频的编码格式
//仅当获取访问信号量超时返回FALSE;
BOOL  mtSetAudioFormatPriorStrategy(EmAudioFormat  emFormat);

//获取视频解码参数
BOOL  mtGetVideoParamter(EmVideoType emType,TVideoParam& atImageInfo);
//设置视频解码参数
BOOL  mtSetVideoParamter(EmVideoType emType,TVideoParam& atImageInfo);
//获取外置矩阵的串口设置类型
BOOL mtGetExtronMatrixComType(EmSerialType& emType);
//设置外置矩阵的串口设置类型
BOOL mtSetExtronMatrixComType(EmSerialType emType);
//获取外置矩阵的串口IP地址
BOOL mtGetExtronMatrixComIP(TIPTransAddr& tIP);
//设置外置矩阵的串口IP地址
BOOL mtSetExtronMatrixComIP(const TIPTransAddr& tIP);

//获取终端控制台发起桌面双流编码参数
BOOL mtGetMtcVideoEncParam( TVideoParam& tParam );

//设置终端控制台发起桌面双流编码参数
BOOL mtSetMtcVideoEncParam( const TVideoParam& tParam );

//获取丢包重传设置
BOOL mtGetLostPackResend(TLostPackResend& tInfo);
//设置丢包重传设置
BOOL mtSetLostPackResend(const TLostPackResend& tInfo);

//获取vod用户设置
BOOL mtGetVODUserInfo(TVODUserInfo& tInfo);
//设置vod用户设置
BOOL mtSetVODUserInfo(const TVODUserInfo& tInfo);


//获取视频源信息
BOOL mtGetVideoSorceInfo(u8 byIndex , s8*const achBuf,u8 byBufSize);
//设置视频源信息
BOOL mtSetVideoSorceInfo(u8 byIndex , const s8*const szStr);

//获取默认视频显示接口
BOOL mtGetDefaultVideoIndex(EmVideoType type , EmMtVideoPort& index);
//设置默认视频显示接口
BOOL mtSetDefaultVideoIndex(EmVideoType type , EmMtVideoPort index);

//8010c显示video还是VGA，0为video,否则为VGA的刷新率
//获取
//BOOL mtGet8010CShowModel(u16& wVal);
BOOL mtGet8010CShowModel(TVgaOutCfg& tVgaOutCfg);
//设置
//BOOL mtSet8010CShowModel(u16 wVal);
BOOL mtSet8010CShowModel(const TVgaOutCfg & tVgaOutCfg);

//设置终端监听端口
BOOL  mtSetMTCListenPort(u16 wPort);
//获取终端监听端口
BOOL  mtGetMTCListenPort(u16& wPort);
//获取双流盒注册信息
BOOL  mtGetDualStreamBoxInfo(TDualStreamBoxInfo& tInfo);
//设置双流盒的注册信息
BOOL  mtSetDualStreamBoxInfo(const TDualStreamBoxInfo& tInfo);
//获取是否使用MC信息
BOOL  mtGetUsedMC(BOOL& bUsed);
//设置是否使用MC信息
BOOL  mtSetUsedMC(BOOL bUsed);

//获取mc运行模式
BOOL  mtGetMCModle(EmMCMode& emModel);
//设置mc运行模式
BOOL  mtSetMCModle(EmMCMode emModel);

//[xujinxing-2006-10-17]
//获取mc是否自动画面合成
BOOL mtGetMcAutoVMP(BOOL &bAutoVMP);
//设置mc是否自动画面合成
BOOL mtSetMcAutoVMP(BOOL bAutoVMP);


//获取双流码率比
BOOL  mtGetDualCodeRation(TDualRation& tRation);
//设置双流码率比
BOOL  mtSetDualCodeRation(TDualRation tRation);

/****add by FangTao****/
//设置Pcmt视频源类型
BOOL mtSetPcmtCapType(EmCapType &emCapType);
BOOL mtSetTelnetPwd(u8 *pPwd, u16 len);
BOOL mtGetTelnetPwd(s8 * pBuf, u8 bufLen);

//获取Pcmt视频源类型
BOOL mtGetPcmtCapType(EmCapType &emCapType);

BOOL mtGetPcmtCapFileName(s8*const achBuf,u8 byBufSize);
BOOL mtSetPcmtCapFileName(const  s8 abyName[]);
//获取第二路视频输出
BOOL  mtGetSecondVideoOutput(BOOL& bIsVGA);
//设置第二路视频输出
BOOL  mtSetSecondVideoOutput(BOOL bIsVGA);
//获取麦克增益
BOOL  mtGetMicVolume(u8& byPlus );
//设置麦克增益
BOOL  mtSetMicVolume(u8 byPlus );
//设置DNS服务器地址
BOOL mtSetDNSSvrAddr(u32_ip dwAddr);
//获取DNS服务器地址
BOOL mtGetDNSSvrAddr(u32_ip& dwAddr);
//设置LDAP服务器域名
BOOL mtSetLDAPSvrName( const s8 achSrvName[] );
//获取LDAP服务器域名
BOOL mtGetLDAPSvrName( s8 achSrvName[] );
//设置网关地址
BOOL mtSetWanGWAddr(u32_ip dwAddr);
//获取网关地址
BOOL mtGetWanGWAddr(u32_ip& dwAddr);
//设置网关地址
BOOL mtSetLanGWAddr(u32_ip dwAddr);
//获取网关地址
BOOL mtGetLanGWAddr(u32_ip& dwAddr);


// 获取快捷键配置
BOOL mtSetRapidKey(TRapidKey tRpdKey);
// 设置快捷键配置
BOOL mtGetRapidKey(TRapidKey &tRpdKey);

// 获取是否禁用打电话配置
BOOL mtSetTelephoneDisable(BOOL bDisable);
// 设置是否禁用打电话配置
BOOL mtGetTelephoneDisable(BOOL& bDisable);

// 获取默认的升级服务器的地址
BOOL mtGetDefaultUpdateSrvAddr( s8 abySrvAddr[]);
// 获取用户设置的升级服务器的地址
BOOL mtGetUserCfgUpdateSrvAddr( s8 abySrvAddr[] );
//设置升级服务器地址
BOOL mtSetUserCfgUpdateSrvAddr(const  s8 abySrvAddr[] );

// 获取T2视频输出配置
BOOL mtGetT2VideoOut(BOOL& bT2VidOutVideo);
// 设置T2视频输出配置
BOOL mtSetT2VideoOut(BOOL bT2VidOutVideo);

// 获取扬声器输出音量
BOOL mtGetLoudspeakerVal(EmMtLoudspeakerVal& emMtLoudspeakerVal);
// 设置扬声器输出音量
BOOL mtSetLoudspeakerVal(EmMtLoudspeakerVal  emMtLoudspeakerVal);

//获取T2以太网内外网位置配置
BOOL mtGetLanEthMask(u8 &byLanEthMask);
//设置T2以太网内外网位置配置
BOOL mtSetLanEthMask(u8 byLanEthMask);

//获取单元最大包数
BOOL mtGetMTU(s32& dwMTU);
//设置单元最大包数
BOOL mtSetMTU(s32 tMTU);

// 获取音频优先设置
BOOL mtGetAudioPrecedence(u32& dwAudioPrecedence);
// 设置音频优先设置
BOOL mtSetAudioPrecedence(u32 dwAudioPrecedence);
// 获取前向纠错算法类型设置
BOOL mtGetFecType( TFecInfo& tInfo );
// 设置前向纠错算法类型设置
BOOL mtSetFecType( TFecInfo tInfo );

//获取最近呼叫码率
BOOL mtGetLastCallRate(u16& wLastCallRate);
//设置最近呼叫码率
BOOL mtSetLastCallRate(u16 wLastCallRate);

//获取呼叫地址记录
BOOL mtGetCallAddrRecord(s8 astrCallAddrRecord[MT_MAXNUM_CALLADDR_RECORD][MT_MAX_H323ALIAS_LEN+1]);
//设置呼叫地址记录
BOOL mtSetCallAddrRecord(s8 astrCallAddrRecord[MT_MAXNUM_CALLADDR_RECORD][MT_MAX_H323ALIAS_LEN+1]);
//获取电话号码记录
BOOL mtGetTeleAddrRecord(s8 astrTeleAddrRecord[MT_MAXNUM_CALLADDR_RECORD][MT_MAX_H323ALIAS_LEN+1]);
//设置电话号码记录
BOOL mtSetTeleAddrRecord(s8 astrTeleAddrRecord[MT_MAXNUM_CALLADDR_RECORD][MT_MAX_H323ALIAS_LEN+1]);

// 获取高清音频端口配置
BOOL mtGetHdAudioPort( THdAudioPort& tInfo );
// 设置高清音频端口配置
BOOL mtSetHdAudioPort( THdAudioPort tInfo );
// 获取是否启用切包传输
BOOL mtGetUseSliceCfg( BOOL& bUseSlice );
// 设置是否切包传输
BOOL mtSetUseSliceCfg( BOOL bUseSlice );

// 获取高清图像参数配置
BOOL mtGetHdImageParam( TImageAdjustParam  atImgParam[] );
// 设置高清图像参数配置
BOOL mtSetHdImageParam( const TImageAdjustParam  atImgParam[] );

//获取画面模式配置
BOOL mtGetScreenModeCfg( u8& byScreenMode);
//设置画面模式配置
BOOL mtSetScreenModeCfg( u8 byScreenMode);

BOOL mtGetDVICfg(BOOL& bDVIEnable);

BOOL mtSetDVICfg( BOOL bDVIEnable );

u32 mtGetHDResCfg( u32& dwHDResEnable );

u32 mtSetHDResCfg( u32 dwHDResEnable );

BOOL mtGetPicPlusCfg( BOOL& bPicPlusEnable );

BOOL mtSetPicPlusCfg( BOOL bPicPlusEnable );

BOOL mtGetAddrEntryInfo( TADDRENTRYINFO& tAddrEntryInfo,  u32 dwIndex );

BOOL mtSetAddrEntryInfo( TADDRENTRYINFO tAddrEntryInfo,  u32 dwIndex );

//获取网口模式
BOOL mtGetEthMode( u32& dwEthMode );
//设置网口模式
BOOL mtSetEthMode( u32 dwEthMode );

// 获取高清输出模式帧率配置
BOOL mtGetVideoDisplayFrm( TVideoDisplayFrm& tInfo );
// 设置高清输出模式帧率配置
BOOL mtSetVideoDisplayFrm( TVideoDisplayFrm tInfo );

//获取PCMT视频编码模式
BOOL mtGetPcmtVEncModeCfg( u8& byPcmtVEncMode);
//设置PCMT视频编码模式
BOOL mtSetPcmtVEncModeCfg( u8 byPcmtVEncMode);

//是否支持多pcmt..
BOOL mtGetMorePcMtEnable(BOOL& bEnable);
//BOOL mtSetMorePcMtEnable(BOOL bEnable);



//--------------------以下为SYSAPI转过来附加的配置项--------------------
//获取附加配置信息类
CConfigEx* GetConfigEx();

//------以下为mtConfig和mtAgent共享的代码不为其他模块所用------
/*-------------------------------------------------------------
   获取分组变量
---------------------------------------------------------------*/
//用户信息描述
typedef struct tagTUserInfo
{
	BOOL                  m_bUserPWDEnable;//是否启用用户密码
	EmLanguage            m_emLanguage;//系统语言设置
	BOOL                  m_bNetPWDEnable; //是否启用网络密码
	s8                    m_achUserPWD[MT_MAX_PASSWORD_LEN];//用户密码
	s8                    m_achNetPWD[MT_MAX_PASSWORD_LEN];//网络密码
	public:
		tagTUserInfo(){memset(this,0,sizeof(struct tagTUserInfo));}
} TUserInfo,*PTUserInfo;

//待机描述
typedef struct tagSleepInfo
{
	BOOL        m_bIsSleepEnable;//是否允许待机
	u16         m_wSleepTimes;//待机时间
public:
	tagSleepInfo(){memset(this,0,sizeof(tagSleepInfo));}
}TSleepInfo,*PTSleepInfo;

//本地终端描述
typedef struct tagTLocalInfo
{
	TSleepInfo  m_tSleepInfo;//待机信息
	BOOL        m_bUsedMC;   //是否使用MC
	EmMCMode    m_emMCModel; //MC运行方式
	BOOL        m_bAutoVMP;  //是否自动增加画面合成数[xujinxing-2006-10-17]
	u32_ip      m_dwDNSSvrIP;//DNS服务器地址
	s8          m_achLDAPSvrName[MT_MAX_LDAPSERVER_NAME_LEN]; //LDAP服务器域名
	BOOL        m_bIsRemoteCtrol;//是否允许远程口之
	BOOL        m_bDHCP;
	u16         m_wMTCPort;  //终端监听端口 
	s8          m_achLocalAliase[MT_MAX_H323ALIAS_LEN + 1];//终端别名
	s8          m_achE164[MT_MAX_E164NUM_LEN + 1];//E164号码
	BOOL        m_bDisableTelephone;  // 是否禁用电话功能

	s8          m_achDefaultServerAddr[MT_MAX_UPDATASERVER_NAME_LEN]; //缺省的升级服务器的域名
	s8          m_achUserCfgServerAddr[MT_MAX_UPDATASERVER_NAME_LEN]; //用户配置的升级服务器域名
	u8          m_byLanEthMask; //网络选择(WAN/LAN)掩码，1为选择LAN，最右三位有效: MT/GK/PXY
	s32         m_dwMTU; //最大单元包数
	u32         m_dwAudioPrecedence; // 音质优先
	u16         m_wLastCallRate; //上一次呼叫码率
	s8          m_astrCallAddrRecord[MT_MAXNUM_CALLADDR_RECORD][MT_MAX_H323ALIAS_LEN+1]; // 呼叫地址记录
	s8          m_astrTeleAddrRecord[MT_MAXNUM_CALLADDR_RECORD][MT_MAX_H323ALIAS_LEN+1]; // 电话号码记录
public:
		tagTLocalInfo(){memset(this,0,sizeof(struct tagTLocalInfo));}
}TLocalInfo,*PTLocalInfo;

//协议描述
typedef struct tagTProtocolInfo
{
	BOOL        m_bH323Enable;//启用H323协议
	BOOL        m_bH320Enable;//启用H320协议
	BOOL        m_bSIPEnable;//启用SIP协议
	BOOL        m_bH239Enable;//启用H239协议
	public:
		tagTProtocolInfo(){memset(this,0,sizeof(struct tagTProtocolInfo));}
}TProtocolInfo,*PTProtocolInfo;

//呼叫描述
typedef struct tagTCallInfo
{
	TDualRation	      m_tDualRation;
	EmTripMode        m_emAnswerMode;//应答模式
	BOOL              m_bIsAutoCallEnable;//是否自动呼叫
	BOOL              m_bEncryptEnable;//会议是否加密
	EmEncryptArithmetic m_emEncryArithmetic;//加密算法
	u32               m_dwSelectCallRate;
	u16               m_wAutoCallBitrate;//自动呼叫码率
	u16               m_wUDPBasePort;//UDP基准端口
	u16               m_wTCPBasePort;//TCP基准端口
	u16               m_wLinkKeepLiveTimeOut;//连路保持时间
	s8                m_abyAutoCallMtAlias[MT_MAX_H323ALIAS_LEN];//自动呼叫对端别名
	public:
		tagTCallInfo(){memset(this,0,sizeof(struct tagTCallInfo));}
}TCallInfo,*PTCallInfo;

//告警描述
typedef struct tagTTrapServerInfo
{
	u32               m_dwTrapServerIP;//告警服务器地质
	s8                m_abyNMPublicName[MT_MAX_COMM_LEN];//共同体名
	public:
		tagTTrapServerInfo(){memset(this,0,sizeof( struct tagTTrapServerInfo));}
} TTrapServerInfo,*PTrapServerInfo;
//显示信息
typedef struct tagTDisplayInfo
{
    //[xujinxing]
	TVgaOutCfg        m_tVgaOutCfg; //8010c输出到vga或者video,及刷新率
	BOOL              m_bIsAutoPIP;//是否自动画中画
	BOOL              m_bIsShowConfLapse;//是否显示台标
	BOOL              m_bIsShowSysTime;//是否显示系统时间
	BOOL              m_bIsShowState;//是否显示状态信息
	BOOL              m_bVGAShowLocal;//显示本地画面还是远端画面
	BOOL              m_bSendStaticPic;//发送静态图片
	BOOL              m_bSecondvideoToVGA;//设置第二路视频输出到VGA
	//u16               m_w8010cShowModel;//8010c显示video还是VGA，0为video,否则为VGA的刷新率 
	EmMtVideoPort     m_abyDefaultVideoIndex[2];
	EmLabelType        m_emSymbolShowMode;//台表现是类型
	TSymboPoint       m_tSymbolXY;//台标显示位置
	EmDualVideoShowMode  m_emDualVideoShowMode;//双流显示模式
	BOOL              m_bT2MainVidOutVideo; //T2版本视频输出是否为Video
	EmDisplayRatio    m_emDisplayRatio;//T2版本显示比例4:3 16:9
	public:
		tagTDisplayInfo(){memset(this,0,sizeof(struct tagTDisplayInfo));}
}TDisplayInfo,*PTDisplayInfo;


//视频信息
typedef struct tagTVideoInfo
{
	TImageAdjustParam    m_tImageAdjustParam;//视频图像调整参数
	EmVideoFormat        m_emVideoFormatPriorStrategy;//视频编码优先策略
	EmVideoResolution    m_emVideoResolvePriorStrategy;//视频分辨率优先策略
	EmVideoStandard      m_emOutVideoStandard;//视频输出标准
	EmVideoStandard      m_emInVideoStandard;//视频输入标准
	EmMtVideoPort         m_emVideoSource;//视频源
	public:
		tagTVideoInfo(){memset(this,0,sizeof(struct tagTVideoInfo));}
}TVideoInfo,*PTVideoInfo;
//音频信息
typedef struct tagTAudioInfo
{
	BOOL                 m_bIsAECEnable;//是否启用回声抵消
	BOOL                 m_bIsAGCEnable;//是否启用自动增益
	BOOL                 m_bIsANSEnable; //是否启动自动噪音抑制
	EmAudioFormat        m_emAudioFormatPriorStrategy;//音频编码优先策略
	u8                   m_byInputVolume;//输入音量（编码音量）
	u8                   m_byOutputVolume;//输出音量（解码音量）
	u8                   m_byMicVolume;
	EmMtLoudspeakerVal   m_emLoudspeakerVal; //扬声器音量值（T2版本）

	public:
		tagTAudioInfo(){memset(this,0,sizeof(struct tagTAudioInfo));}
}TAudioInfo,*PTAudioInfo;

typedef struct tagTExtronMatrixComInfo
{
	//当串口类型为tcpip时，一下字段有用
	//串口服务器IP ,当该值为0时表示使用本地串口
	//串口服务器端口号   	
	TIPTransAddr m_tIP;
    EmSerialType m_emSerialType;
}TExtronMatrixComInfo,*PTExtronMatrixComInfo;

//pcmt设置
typedef struct tagTPCMTInfo
{
	//单独用于特殊版本的创建，只能手动修改配置文件，不提供程序修改
	BOOL	m_bMorePcMtEnable;
	//编码方式：软编码，硬编码
	u8		m_byPcmtVEncMode;
}TPCMTInfo,*PTPCMTInfo;

BOOL  GetUserInfo(TUserInfo&   tUserInfo);
BOOL  GetIPQos(TIPQoS&      tIPQoS);
BOOL  GetEthnetInfo(TEthnetInfo atTEthnetInfo[MT_MAX_ETHNET_NUM]);
BOOL  GetE1Info(TE1Config&  tE1Cfg);
BOOL  GetRouteInfo(TRouteCfg   atTRouteCfg[MT_MAX_ROUTE_NUM]);
BOOL  GetPPPOEInfo(TPPPOECfg&   tPPPOECfg);
BOOL  GetSerialInco(TSerialCfg  atSerialCfg[MT_MAX_SERIAL_NUM]);
BOOL  GetLocalInfo(TLocalInfo&  tLocalInfo);
BOOL  GetProtocolInfo(TProtocolInfo&     tProtocolInfo);
BOOL  GetFirwareInfo(TEmbedFwNatProxy&  tEmbedNatProxy);
BOOL  GetNatMapAddrInfo(TNATMapAddr&  tInlineStaticNatAddr);
BOOL  GetMonitorInfo(TMonitorService&   tMonitorSerivce);
BOOL  GetGKInfo(TGKCfg&   tGKIP);
BOOL  GetCallInfo(TCallInfo&  tCallConfig);
BOOL  GetTrapInfo(TTrapServerInfo&   tTrapInfo);
BOOL  GetDisplayInfo(TDisplayInfo&      tDisplayInfo);
BOOL  GetVideoInfo(TVideoInfo      atVideoConfig[MT_MAX_VIDEO_NUM]);
BOOL  GetAudioInfo(TAudioInfo&        tAudioConfig);
BOOL  GetStreamInfo(TStreamMedia&      tStreamMedia);
BOOL  GetCameraInfo(TCameraCfg  atCameraCfg[MT_MAX_CAMERA_NUM]);

API void  mtconfighelp();
API void  CfgAllData(void);
API void  ResetDallData(void);

#endif

