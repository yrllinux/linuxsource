/*****************************************************************************
   模块名      : Mcu Agent
   文件名      : Agentinterface.h
   相关文件    : 
   文件实现功能: mcu 配置界面化接口
   作者        : 
   版本        : V4.0  Copyright( C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人        修改内容
   2005/08/17  4.0         liuhuiyun     创建
   2006/11/02  4.0         张宝卿        代码优化
******************************************************************************/
#ifndef _AGENT_INTERFACE_H_
#define _AGENT_INTERFACE_H_
#include "agentcommon.h"
#include "mcuagtstruct.h"
#include "mcuinnerstruct.h"

#ifdef WIN32
#pragma  once
#endif

class CCfgAgent;

BOOL32 McuAgentInit( u8 byMcuType, BOOL32 bEnabled = TRUE );        // 初始化代理
BOOL32 McuAgentQuit( BOOL32 bRebootBrd = TRUE );                    // 退出代理( 参数：是否重启所有单板)

class CAgentInterface
{
public:
    CAgentInterface();
    ~CAgentInterface();
    
public:
    u32    GetDcsIp( void );                                        // 读取DCS IP
    u16    SetDcsIp( u32 dwDcsIp );                                 // 设置DCS IP
    u16    GetQosInfo( TQosInfo*  tQosInfo );                       // 读取QOS信息
    u16    SetQosInfo( TQosInfo tQosInfo );                         // 设置QOS信息
    u16    GetLocalInfo( TLocalInfo*  tLocalInfo );                 // 读取本地信息
    u16    SetLocalInfo( TLocalInfo  tLocalInfo );                  // 设置本地信息
    u16    GetNetWorkInfo( TNetWorkInfo*  tNetWorkInfo );           // 读取网络信息
    u16    SetNetWorkInfo( TNetWorkInfo  tNetWorkInfo );            // 设置网络信息
    u16    GetNetSyncInfo( TNetSyncInfo* ptNetSyncInfo );           // 读取网同步信息
    u16    SetNetSyncInfo( TNetSyncInfo tNetSyncInfo );             // 设置网同步信息
    u16    GetDscInfo( TDSCModuleInfo *ptDSCInfo );                 // 读取8000B DSC板配置信息
	u16    SetDscInfo( TDSCModuleInfo *ptDSCInfo, BOOL32 bToFile ); // 设置8000B DSC板配置信息
	u8	   GetConfigedDscType( void );								// 获得DSC板的类型，如果没有配置过，则返回0, zgc, 2007-03-05

	u16    SetLoginInfo( TLoginInfo *ptLoginInfo );					// 设置登陆信息
	u16	   GetLoginInfo( TLoginInfo *ptLoginInfo );					// 获取登陆信息

	u16	   GetLastDscInfo( TDSCModuleInfo * ptDscInfo );				// 取保存的修改之前的DSC特殊配置信息
	u16	   SetLastDscInfo( TDSCModuleInfo * ptDscInfo );				// 保存修改之前的DSC特殊配置信息

	// 把DSC info设置到MPC板上, zgc, 2007-07-17
	BOOL32 SaveDscLocalInfoToNip( TDSCModuleInfo * ptDscInfo );
	// 设置路由到MINIMCU MPC板上, zgc, 2007-09-21
	BOOL32 SaveRouteToNipByDscInfo( TDSCModuleInfo *ptDscInfo );

	// 获得MCU读配置文件情况, zgc, 2007-07-25
	u8	   GetMcuCfgInfoLevel(void);

    // 新增 处理在线MCS的IP信息  [04/24/2012 liaokang]
    BOOL32 AddRegedMcsIp( u32 dwMcsIpAddr );                        // 添加在线MCS的IP信息
    BOOL32 DeleteRegedMcsIp( u32 dwMcsIpAddr );                     // 删除在线MCS的IP信息

	BOOL32 IsMcuConfiged();											// 判断MCU是否被配置过
	u16    SetIsMcuConfiged(BOOL32 bConfiged);			    		// 设置MCU配置标识

    u16    ReadTrapTable( u8* pbyTrapNum, TTrapInfo* ptTrapTable ); // 读取整体TRAP信息
    u16    WriteTrapTable( u8 byTrapNum, TTrapInfo* ptTrapTable );  // 设置整体TRAP信息
    u16    ReadBrdTable( u8* pbyBrdNum, TBoardInfo* pptBoardTable );// 读取整体单板表信息
    u16    WriteBrdTable( u8 byBrdNum, TBoardInfo* ptBoardTable );  // 设置整体单板表信息
    u16    ReadTvTable( u8* pbyTvNum, TEqpTVWallInfo* ptTvTable );  // 读取整体TVWall表信息
    u16    WriteTvTable( u8 byTvNum, TEqpTVWallInfo* ptTvTable );   // 设置整体TVWall表信息
    u16    ReadRecTable( u8* pbyRecNum, TEqpRecInfo* ptRecTable );  // 读取整体Rec表信息
	// 新增参数 bIsNeedUpdatePortInTable标识是否需要更新内存中的端口信息 [12/15/2011 chendaiwei]
    u16    WriteRecTable( u8 byRecNum, TEqpRecInfo* ptRecTable, BOOL32 bIsNeedUpdatePortInTable = TRUE  );   // 设置整体Rec表信息
    u16    ReadBasTable( u8* pbyBasNum, TEqpBasInfo* ptBasTable );  // 读取整体Bas表信息
	// 新增参数 bIsNeedUpdatePortInTable标识是否需要更新内存中的端口信息 [12/15/2011 chendaiwei]
    u16    WriteBasTable( u8 byBasNum, TEqpBasInfo* ptBasTable, BOOL32 bIsNeedUpdatePortInTable = TRUE  );   // 设置整体Bas表信息
    //zw[08/06/2008]
    u16    ReadBasHDTable( u8* pbyBasNum, TEqpBasHDInfo* ptBasHDTable );  // 读取整体BasHD表信息
	// 新增参数 bIsNeedUpdatePortInTable标识是否需要更新内存中的端口信息 [12/15/2011 chendaiwei]
    u16    WriteBasHDTable( u8 byBasNum, TEqpBasHDInfo* ptBasHDTable, BOOL32 bIsNeedUpdatePortInTable = TRUE  );   // 设置整体BasHD表信息
   
    u16    ReadVmpTable( u8* pbyVmpNum, TEqpVMPInfo* ptVmpTable );  // 读取整体VMP表信息
	// 新增参数 bIsNeedUpdatePortInTable标识是否需要更新内存中的端口信息 [12/15/2011 chendaiwei]
    u16    WriteVmpTable( u8 byVmpNum, TEqpVMPInfo* ptVmpTable, BOOL32 bIsNeedUpdatePortInTable = TRUE   );   // 设置整体VMP表信息
    u16    ReadMpwTable( u8* pbyMpwNum, TEqpMpwInfo* ptMpwTable );  // 读取整体MPW表信息
    u16    WriteMpwTable( u8 byMpwNum, TEqpMpwInfo* ptMpwTable );   // 设置整体MPW表信息
    u16    ReadPrsTable( u8* pbyPrsNum, TEqpPrsInfo* ptPrsTable );  // 读取整体Prs表信息
    u16    WritePrsTable( u8 byPrsNum, TEqpPrsInfo* ptPrsTable );   // 设置整体Prs表信息
    u16    ReadMixerTable( u8* pbyMixNum, TEqpMixerInfo* pptMixerTable );// 读取整体Mix表信息
	// 新增参数 bIsNeedUpdatePortInTable标识是否需要更新内存中的端口信息 [12/15/2011 chendaiwei]
    u16    WriteMixerTable( u8 byMixNum, TEqpMixerInfo* ptMixerTable, BOOL32 bIsNeedUpdatePortInTable = TRUE );  // 设置整体Mix表信息
	//4.6新加外设读写配置表 jlb
    u16    ReadHduTable( u8* pbyHduNum, TEqpHduInfo* ptHduTable );// 读取整体Hdu表信息
    u16    WriteHduTable( u8 byHduNum, TEqpHduInfo* ptHduTable );  // 设置整体Hdu表信息
	u16    ReadSvmpTable( u8* pbySvmpNum, TEqpSvmpInfo* ptSvmpTable );// 读取整体Svmp表信息
	// 新增参数 bIsNeedUpdatePortInTable标识是否需要更新内存中的端口信息 [12/15/2011 chendaiwei]
    u16    WriteSvmpTable( u8 bySvmpNum, TEqpSvmpInfo* ptSvmpTable,BOOL32 bIsNeedUpdatePortInTable = TRUE   );  // 设置整体Svmp表信息
// 	u16    ReadDvmpTable( u8* pbyDvmpNum, TEqpDvmpBasicInfo* ptDvmpTable );// 读取整体Dvmp表信息
//  u16    WriteDvmpTable( u8 byDvmpNum, TEqpDvmpBasicInfo* ptDvmpTable );  // 设置整体Dvmp表信息
	u16    ReadMpuBasTable( u8* pbyMpuBasNum, TEqpMpuBasInfo* ptMpuBasTable );// 读取整体MpuBas表信息
	// 新增参数 bIsNeedUpdatePortInTable标识是否需要更新内存中的端口信息 [12/15/2011 chendaiwei]
    u16    WriteMpuBasTable( u8 byMpuBasNum, TEqpMpuBasInfo* ptMpuBasTable, BOOL32 bIsNeedUpdatePortInTable = TRUE   );  // 设置整体MpuBas表信息
// 	u16    ReadEbapTable( u8* pbyEbapNum, TEqpEbapInfo* ptEbapTable );// 读取整体Ebap表信息
//     u16    WriteEbapTable( u8 byEbapNum, TEqpEbapInfo* ptEbapTable );  // 设置整体Ebap表信息
// 	u16    ReadEvpuTable( u8* pbyEvpuNum, TEqpEvpuInfo* ptEvpuTable );// 读取整体Evpu表信息
//    u16    WriteEvpuTable( u8 byEvpuNum, TEqpEvpuInfo* ptEvpuTable );  // 设置整体Evpu表信息
    u16    ReadVrsRecTable( u8* pbyVrsRecNum, TEqpVrsRecCfgInfo* ptVrsRecTable );  // 读取整体VrsRec表信息
    u16    WriteVrsRecTable( u8 byVrsRecNum, TEqpVrsRecCfgInfo* ptVrsRecTable);   // 设置整体VrsRec表信息
    u16    ReadHduSchemeTable( u8* pbyHduProjectNum, THduStyleInfo* ptHduSchemeTable );  // 读取整体HDU预案设置表信息
    u16    WriteHduSchemeTable( u8 byHduProjectNum, THduStyleInfo* ptHduSchemeTable );   // 设置整体HDU预案设置表信息
    u16    ReadVmpAttachTable( u8* pbyVmpProjectNum, TVmpAttachCfg* ptVmpAttachTable );  // 读取整体VMP风格表信息
    u16    WriteVmpAttachTable( u8 byVmpProjectNum, TVmpAttachCfg* ptVmpAttachTable );   // 设置整体VMP风格表信息
	BOOL32 RefreshBrdOsType(u8 byBrdId, u8 byOsType);//更新某个外设板卡的OSTYPE值
    // 多国语言: [pengguofeng 4/16/2013]
	u16    ReadMcuEncodingType(u8 &byEncoding);
	BOOL32 WriteMcuEncodingType(const u8 &byEncoding);
	u8     GetMcuEncodingType(void);
	void   SetMcuEncodingType(const u8 &byEncoding);
	// end [pengguofeng 4/16/2013]
    
    u16    GetEqpRecorderCfg( u8 byRecId, TEqpRecInfo * ptRecCfg ); // 读取单个Rec信息
    u16    SetEqpRecorderCfg( u8 byRecId, TEqpRecInfo * ptRecCfg ); // 设置单个Rec信息
    u16    GetEqpTVWallCfg( u8 byTvId, TEqpTVWallInfo* ptTWCfg );   // 读取单个TVWall信息
    u16    SetEqpTVWallCfg( u8 byTvId, TEqpTVWallInfo* ptTWCfg );   // 设置单个TVWall信息
    u16    GetEqpMixerCfg( u8 byMixId, TEqpMixerInfo* ptMixerCfg ); // 读取单个Mix信息
    u16    SetEqpMixerCfg( u8 byMixId, TEqpMixerInfo tMixerCfg );   // 设置单个Mix信息
    u16    GetEqpBasCfg( u8 byBasId, TEqpBasInfo* ptBasCfg );       // 读取单个Bas信息
    u16    SetEqpBasCfg( u8 byBasId, TEqpBasInfo* ptBasCfg );       // 设置单个Bas信息
	// 4.6 jlb 
	u16    SetEqpHduCfg(u8 byHduId, TEqpHduInfo* ptHduCfg);         //读取单个Hdu信息  
	u16    GetEqpHduCfg(u8 byHduId, TEqpHduInfo* ptHduCfg);         //读取单个Hdu信息  

    //zw[08/07/2008]
    BOOL32 IsEqpBasHD(u8 byBasHDId);                                 //判断是否高清bas
	BOOL32 IsEqpBasAud(u8 byBasId);
    u16    GetEqpBasHDCfg( u8 byBasHDId, TEqpBasHDInfo* ptBasHDCfg );// 读取单个BasHD信息
    u16    SetEqpBasHDCfg( u8 byBasHDId, TEqpBasHDInfo* ptBasHDCfg );// 设置单个BasHD信息
	u16    GetMpuBasBrdInfo(u8 byBasEqpId,TEqpMpuBasInfo* ptEqpMpuBasInfo);
    BOOL32 IsSVmp( u8 byEqpId );
	u16     GetVmpEqpVersion( u8 byEqpId );
    BOOL32 IsDVmp( u8 byEqpId );
    BOOL32 IsEVpu( u8 byEqpId );
    BOOL32 IsMpuBas( u8 byEqpId );
	u16     GetBasEqpVersion(u8 byEqpId);
	u16		GetHDUEqpVersion( u8 byEqpId );
    BOOL32 IsEBap( u8 byEqpId );    
	u16    GetEqpSvmpCfgById( u8 byId, TEqpSvmpInfo * ptSvmpCfg );

    u16    GetEqpVMPCfg( u8 byVmpId, TEqpVMPInfo* ptVMPCfg );       // 读取单个VMP信息
    u16    SetEqpVMPCfg( u8 byVmpId, TEqpVMPInfo* ptVMPCfg );       // 设置单个VMP信息
    u16    GetEqpMpwCfg( u8 byMpwId, TEqpMpwInfo* ptMpwCfg );       // 读取单个MPW信息
    u16    SetEqpMpwCfg( u8 byMpwId, TEqpMpwInfo* ptMpwCfg );       // 设置单个MPW信息
    u16    GetEqpPrsCfg( u8 byPrsId, TEqpPrsInfo& tPrsCfg );        // 读取单个Prs信息
	
	u8     GetEqpSwitchBrdId(u8 byEqpId);                         // 获取指定外设转发板信息
	BOOL32 SetEqpSwitchBrdId(u8 byEqpId, u8 bySwitchBrdId);       // 设置制定外设转发板信息
	u16    GetBrdCfgById(u8 byBrdId, TBoardInfo* ptBoardInfo);    // 获取单板信息

public:
    u16    GetMcuAlias( s8* pszAlias, u8 byLength );        // 取Mcu别名
    u16    GetE164Number( s8* pszE164Num, u8 byLen );       // 取Mcu E164号
    u32    GetMpcIp( void );                                // 取MPC Ip( 网络序)
    u16    SetMpcIp( u32 dwIp, u8 byInterface );            // 设MPC Ip( 网络序)
    u32    GetMaskIp( void );                               // 取子网掩码( 网络序)
    u16    SetMaskIp( u32 dwIp, u8 byInterface );           // 设子网掩码( 网络序)
    u32    GetGateway( void );                              // 取网关( 网络序)
    u16    SetGateway( u32 dwIp );                          // 设网关( 网络序)
	u8     GetInterface( void );                            // 取MPC端口配置
	u16    SetInterface( u8 byInterface );                  // 设MPC端口配置
    u32    GetCastIpAddr( void );                           // 取组播地址
    u16    GetCastPort( void );                             // 取组播端口

    BOOL32 GetIsGKCharge( void );                           // 取GK是否支持
    u32    GetGkIpAddr( void );                             // 取GkIp( 网络序)
    u16    SetGKIpAddr( u32 dwGkIp );                       // 设置Gk Ip( 网络序)
	u8	   GetGkRRQUsePwdFlag ( void ) const;					// 返回GK RRQ用户名
    LPCSTR GetGkRRQPassword ( void );						// 返回GK RRQ密码

    u32    GetRRQMtadpIp( void );                           // 取主协议适配板地址( 网络序)
    void   SetRRQMtadpIp( u32 dwIp );                       // 设置新的主接入地址
    u16    GetRecvStartPort( void );                        // 取Mcu接受起始端口
    u16    Get225245StartPort( void );                      // 取225端口
    u16    Get225245MtNum( void );                          // 取终端数
    u8     GetMpcBoardId( void );                           // 取Mpc板Id
    BOOL32 GetIsUseMpcStack( void );                        // 取是否使用Mpc内置协议
    BOOL32 GetIsUseMpcTranData( void );                     // 取是否用Mpc转发数据(废弃)
    u16    SetMpcEnabled( BOOL32 bEnabled );                // 设Mpc板是否是主板( 业务 1-ACTIVE 0-STANDBY)
    
    u32    GetMpcDIpAddr( void );                           // 取另一块Mpc板Ip( 网络序)
    u16    GetMpcDPort( void );                             // 取另一块Mpc板Port
    u16    GetLocalLayer( u8& byLayer );                    // 本机层号
    u16    GetLocalSlot( u8& bySlot );                      // 本机槽号
    u16    SetSystemTime( time_t tTime );                   // 设置系统时间
    BOOL32 IsHaveOtherMpc( void );                          // 是否配置了两块Mpc板

    u8     GetPeriEqpType( u8 byEqpId );                    // 取外设类型
    u8     GetBrdChoice( u8 byLayer, u8 bySlot, u8 byType );// 取单板网口选择
    u32    GetBrdIpAddr( u8 byBrdId );                      // 取单板Ip(网络序)
    u32    GetBoardIpAddrFromIdx( u8 byBrdId );             // 取单板Ip(网络序)
    u16    RebootBoard( u8 byLayer, u8 bySlot, u8 byType ); // 重启指定单板
    BOOL32 GetCriDriIsCast( u8 byBrdId );                   // 取单板是否组播选择
    u16    GetBoardInfo( u32 dwBrdIp, u8* pbyBrdId, u8* pbyLayer = NULL, u8* pbySlot = NULL ); // 取外设信息
      u16    GetPeriInfo( u8 byEqpId, u32* pdwEqpIp, u8* pbyEqpType );// 取外设信息
	BOOL32 IsDscReged( u8 &byDscType );		// 判断DSC是否注册

    void   SetRunningMsgDst( u16 wRunningEvent, u32 dwNode );   // 设置Mpc运行消息号
    void   SetPowerOffMsgDst( u16 wPowerOffEvent, u32 dwNode ); // 设置Mpc关机消息号
    void   SetRebootMsgDst( u16 wRebootEvent, u32 dwNode );     // 设置Mpc重启消息号
    
    u8     GetRunBrdTypeByIdx( u8 byBrdIdx );                   // 通过板的层槽组合索引查询运行板类型

    u8     GetHWVersion( void );                                // 查询硬件版本号
    u8     GetCpuRate( void );									// 查询Cpu 占有率
	u32	   GetMemoryRate(void);									// 查询Memory 占有率

    BOOL32 IsMcuPdtBrdMatch(u8 byPdtType) const;                // 检查业务与MPC板类型是否匹配
	
	void	GetIpFromU32(s8* achDstStr, u32 dwIpAddr);

	// [1/15/2010 xliang] 网卡相关操作
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
#ifdef _8KI_
	u16		SetNewNetCfgInfo(const TNewNetCfg &tNewNetcfgInfo, const s8 bySecEthIndx = -1);	//设置8KI支持3网口网络配置 
	u16		GetNewNetCfgInfo(TNewNetCfg &tNewNetCfgInfo);		//获取8KI支持3网口网络配置 
	u16	    GetMacByEthIdx( const u8 &byEthIdx,s8* achMac,const u8 &byMacLen );
#endif
	u16	  SetGkProxyCfgInfo( TGKProxyCfgInfo &tGKProxyCfgInfo);
	u16   GetGkProxyCfgInfo( TGKProxyCfgInfo &tGKProxyCfgInfo );

	u16		SetSipRegGkInfo( u32 dwGKIp );							//写注册GK到文件
	u16   SetProxyDMZInfo( TProxyDMZInfo &tProxyDMZInfo );
	u16   GetProxyDMZInfo( TProxyDMZInfo &tProxyDMZInfo );
	u16	  GetProxyDMZInfoFromMcuCfgFile( TProxyDMZInfo &tProxyDMZInfo );
	
	u16	  SetPrsTimeSpanCfgInfo( TPrsTimeSpan &tPrsTimeSpan );
	u16	  GetPrsTimeSpanCfgInfo( TPrsTimeSpan &tPrsTimeSpan );	

	u16    GetMcuEqpCfg( TMcu8KECfg * ptMcu8KECfg );                // 获得MCU设备信息
	u16    SetMcuEqpCfg(TNetAdaptInfoAll * ptNetAdaptInfoAll);
	u16	   SetMcu8KIEqpCfg( TMcu8KECfg * ptMcuEqpCfg );   //设置8KI网络配置[5/7/2012 chendaiwei]	

	u16	   GetMultiNetCfgInfo(TMultiNetCfgInfo &tMultiNetCfgInfo);		// 获取路由配置
	u16	   GetMultiManuNetAccess(TMultiManuNetAccess &tMultiManuNetAccess,BOOL32 bFromMcuCfgFile = FALSE, 
								 TMultiEthManuNetAccess *tMultiEthManuNetAccess = NULL, u8 *byEthNum = NULL);	//获取多运营商网络配置
	u16	   WriteMultiManuNetAccess(const TMultiManuNetAccess &tMultiManuNetAccess, 
								   const TMultiEthManuNetAccess *tMultiEthManuNetAccess = NULL, 
								   const u8 byEthNum = 0);
	BOOL32 GetGkProxyCfgInfoFromCfgFile ( TGKProxyCfgInfo &tgkProxyCfgInfo );
	u16    SetRouteCfg(TMultiNetCfgInfo &tMultiNetCfgInfo);				// 配置路由
	u16 SetMultiManuNetCfg(const TMultiManuNetCfg &tMultiManuNetCfg, 
						   const TMultiEthManuNetAccess *tMultiEthManuNetAccess = NULL,
						   const u8 byEthNum = 0);	// 设置多运营商网络配置

//	u16   SaveMcuEqpCfgToSys( TMcu8KECfg * ptMcu8KECfg);			//将配置信息刷新到系统配置中
	u16	  Save8KENetCfgToSys( TMcu8KECfg * ptMcu8KECfg);			//将配置信息刷新到系统配置中
	void  SetRouteToSys(void);
	u8	  SearchPortChoice(u32 dwIpAddr = 0);						//查找当前使用的网口
#endif

#ifdef _LINUX_
	u32 EthIdxSys2Hard( u32 dwSysIdx );
#endif
#ifdef _MINIMCU_
    BOOL32 IsMcu8000BHD() const;
#endif    
	
	u16   SetVCSSoftName( s8* pachSoftName );
	void  GetVCSSoftName( s8* pachSoftName );

	//  [1/8/2010 pengjie] Modify 外设扩展配置
	u16   SetEqpExCfgInfo( TEqpExCfgInfo &tEqpExCfgInfo );
	u16   GetEqpExCfgInfo( TEqpExCfgInfo &tEqpExCfgInfo );

	//lukp [2/22/2010]添加设置编译时间
	u16   SetMcuCompileTime(s8 *pszCompileTime);

	u16 SetAuthMTNum(u16 wAuthMTNum);
	u16 SetAuthMTTotal(u16 wAuthTotal);
	u16 SetEqpStat(u8 byEqpType, u16 wEqpUsedNum, u16 wEqpTotal);
	// End Modify

	//[2011/02/11 zhushz] mcs修改mcu 网络配置	
	void GetNewNetCfg(TMcuNewNetCfg& tMcuNewNetInfo);
	void SetNewNetCfg(const TMcuNewNetCfg& tMcuNewNetInfo);	
	BOOL32 IsMpcNetCfgBeModifedByMcs(void);
	BOOL32 SetIsNetCfgBeModifed(BOOL32 bNetInfoBeModify);

    //[12/13/2011 chendaiwei]根据vmp外设Id获取所属MPU2板的类型
	u8 GetMPU2TypeByVmpEqpId(u8 byVmpEqpId);

	u8 GetMPU2TypeByEqpId(u8 byEqpId,u8 &byBrdId);
	BOOL32 GetMpu2AnotherEqpId(u8 byEqpId,u8 &byAnotherId);

	BOOL32 GetAPU2BasEqpIdByMixerId(u8 byMixerEqpId,u8 &byBasId);

	// 获取为特定Eqp分配的外设MCU侧接收起始端口 [12/13/2011 chendaiwei]
	u16	GetPeriEqpMcuRecvStartPort(u8 byEqpType, u8 bySubEqpType = 0);
	// 初始化外设MCU侧起始端口值 [12/13/2011 chendaiwei]
	void ClearAllPeriEqpMcuRecvPort(void);
	//zhouyiliang 20110208 设置升级标志位
	void SetMpcUpateStatus(s32 dwStat = 1 );
	
	//获取混音器的类型[2/13/2012 chendaiwei]
	BOOL32 GetMixerSubTypeByEqpId(u8 byEqpId, u8 &byMixertype);
	BOOL32 GetHduSubTypeByEqpId(u8 byEqpId, u8 &byHduSubtype);
	BOOL32 GetMixerSubTypeByRunBrdId(u8 byBrdId, u8 & byMixertype);
	BOOL32 IsNeedRebootAllMpuBas( void );
	void   SetRebootAllMpuBasFlag( BOOL32 bNeedReboot);
	u8 GetBrdIdbyIpAddr(u32 dwIpAddr);
	u8 GetCurEthChoice ( void );

#ifdef _8KH_
	void SetMcuType800L( void );
	void SetMcuType8000Hm(void);
	BOOL32 Is800LMcu( void );
	BOOL32 Is8000HmMcu( void );
#endif

    void AdjustMcuRasPort(u32 wMpcIp, u32 wGkIp);

	void AdjustNewMcuRasPort(u32 dwMcuIp, u32 dwGkIp, u32 dwSipIp);
private:
	// 各外设MCU侧接收起始端口 [12/13/2011 chendaiwei]
	u16 m_wMixerMcuRecvStartPort;
	u16 m_wRecMcuRecvStartPort;
	u16 m_wBasMcuRecvStartPort;
	u16	m_wVmpMcuRecvStartPort;
	u8  m_byIsNeedRebootAllMPUBas; // 是否需要重启所有MPU(bas)单板 [5/3/2012 chendaiwei]
};

API void mcuagtver( void );
API void mcuagenthelp( void );
API u32  BrdGetDstMcuNode( void );
API u32  BrdGetDstMcuNodeB( void );

#endif  // _AGENT_INTERFACE_H_

