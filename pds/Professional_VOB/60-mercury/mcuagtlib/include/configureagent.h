/*****************************************************************************
   模块名      : Mcu Agent
   文件名      : cfgagent.h
   相关文件    : 
   文件实现功能: 配置文件的读取
   作者        : liuhuiyun
   版本        : V4.0  Copyright( C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人          修改内容
   2005/08/17  1.0         liuhuiyun       创建
   2006/05/20  4.0         liuhuiyun       优化调整
   2006/11/02  4.0         张宝卿          数据读取算法优化
******************************************************************************/
#ifndef _AGENT_CONFIGURE_READ_
#define _AGENT_CONFIGURE_READ_

#include "osp.h"
#include "brdmanager.h"
#include "mcustruct.h"
#include "mcuagtstruct.h"
#include "agentcommon.h"
#include "agtcomm.h"
#include "snmpadp.h"
#include "evagtsvc.h"
#include "mcuver.h"
#include "brdwrapperdef.h"
#include "mcustruct.h"
#include "mcuinnerstruct.h"
#include "commonlib.h"
#include "brdctrl.h"


#ifdef _VXWORKS_
#include "brddrvlib.h"
#endif

// MPC2 支持
#ifdef _LINUX_
    #ifdef _LINUX12_
        #include "brdwrapper.h"
        #include "brdwrapperdef.h"
        #include "nipwrapper.h"
        #include "nipwrapperdef.h"
    #else
        #include "boardwrapper.h"
    #endif
#endif

#ifdef WIN32
#include "stdlib.h"
#include "winbrdwrapper.h"
#pragma  once
#endif


class CSemOpt;
class CAlarmProc;


#define ENTER( hSem) \
	CCfgSemOpt cSemOpt( hSem );

class CCfgAgent{

public:
    CCfgAgent();
    virtual ~CCfgAgent();

public: 

// [0].基本入口函数
    BOOL32 AgentInit( u8 byMcuType );						        // 初始化数据区
    BOOL32 AgentReadConfig( void );							        // 读配置文件
    void   AgentReadDebugInfo( void );                              // 读取Debug文件
    void   AgentDirCreate( const s8* pPathName );			        // 创建目录
    void   SetDefaultConfig( void );							    // 设置成默认配置
    u16    PretreatConfigureFile( s8* achFileName, u8 byMpcType );  // 没有配置文件时生成默认配置文件
    
// [1].具体读配置文件中的某部分    
    BOOL32 AgentGetSystemInfo( const s8* lpszProfileName );         // 读系统信息
    BOOL32 AgentGetLocalInfo( const s8* lpszProfileName );          // 读本地信息
    BOOL32 AgentGetNetworkInfo( const s8* lpszProfileName );        // 读网络信息
    BOOL32 AgentGetTrapRcvTable( const s8* lpszProfileName );       // 读Trap服务器信息
    BOOL32 AgentGetBrdCfgTable( const s8* lpszProfileName );        // 读单板信息
    BOOL32 AgentGetEqpDcs( const s8* lpszProfileName );             // 读数据会议服务器信息
    BOOL32 AgentGetPrsInfo( const s8* lpszProfileName );            // 读Prs信息
    BOOL32 AgentGetNecSync( const s8* lpszProfileName );            // 读网同步信息
    BOOL32 AgentGetMixerTable( const s8* lpszProfileName );         // 读混音器信息
    BOOL32 AgentGetRecorderTable( const s8* lpszProfileName );      // 读录像信息    
    BOOL32 AgentGetTVWallTable( const s8* lpszProfileName );        // 读电视墙信息
    BOOL32 AgentGetBasTable( const s8* lpszProfileName );           // 读Bas信息
    //zw[08/07/2008]
    BOOL32 AgentGetBasHDTable( const s8* lpszProfileName );         // 读BasHD信息
    //4.6版本新加 jlb
    BOOL32 AgentGetHduTable( const s8* lpszProfileName );         // 读Hdu信息
    BOOL32 AgentGetSvmpTable( const s8* lpszProfileName );         // 读Svmp信息
//    BOOL32 AgentGetDvmpTable( const s8* lpszProfileName );         // 读Dvmp信息
    BOOL32 AgentGetMpuBasTable( const s8* lpszProfileName );         // 读MpuBas信息
//    BOOL32 AgentGetEbapTable( const s8* lpszProfileName );         // 读Ebap信息
//    BOOL32 AgentGetEvpuTable( const s8* lpszProfileName );         // 读Evpu信息
    BOOL32 AgentGetHduSchemeTable( const s8* lpszProfileName );     // 读取HDU预案
    BOOL32 AgentGetVrsRecorderTable( const s8* lpszProfileName );	// 读取Vrs新录播信息
    

    BOOL32 AgentGetVMPTable( const s8* lpszProfileName );           // 读Vmp信息
    BOOL32 AgentGetQosInfo( const s8* lpszProfileName );            // 读Qos信息	
    BOOL32 AgentGetVmpAttachTable( const s8* lpszProfileName );     // 读画面合成风格
    BOOL32 AgentGetMpwTable( const s8* lpszProfileName );           // 读多画面电视墙
#ifdef _MINIMCU_
	BOOL32 AgentGetDSCInfo( const s8* lpszProfileName );            // 读8000B下DSC殊配置信息
#endif
	BOOL32 AgentGetVCSSoftName( const s8* lpszProfileName );

	BOOL32 AgentGetGkProxyCfgInfo( const s8* lpszProfileName );		
	BOOL32 AgentGetPrsTimeSpanCfgInfo(const s8* lpszProfileName );
#ifdef _8KI_
	BOOL32 AgentGetNewNetCfgInfo( const s8* lpszProfileName );	//获取8KI网口信息
	void SetNewNetCfgInfoByDefault();
	void SwitchNewNetCfgToMcuEqpCfg( const TNewNetCfg &tNewNetCfg, TMcu8KECfg &tMcuEqpCfg);	//TNewNetCfg转换为tMcuEqpCfg
#endif
// [2].界面配置化
    u16    ReadTrapTable( u8* pbyTrapNum, TTrapInfo* ptTrapTable ); // 整体读TRAP表
    u16    WriteTrapTable( u8 byTrapNum,  TTrapInfo* ptTrapTable ); // 整体写TRAP表
    u16    ReadBrdTable( u8* pbyBrdNum, TBoardInfo* ptBoardTable ); // 整体读单板表
    u16    WriteBrdTable( u8 byBrdNum,  TBoardInfo* ptBoardTable ); // 整体写单板表
    u16    WriteBoardTable( u8 byIndex, TBoardInfo* ptBoardTable, BOOL32 bSemTake = FALSE ); // 写单个单板表项
	u16    OrganizeBoardTable(u8 byIndex,  TBoardInfo* ptBoardTable,   s8* ptInfoTobeWrited, BOOL32 bSemTake = FALSE ); // 单个单板表项
    u16    ReadMixerTable( u8* pbyMixNum, TEqpMixerInfo* ptTable ); // 整体读Mix表
	//标识是否需要更新内存中的端口信息[12/15/2011 chendaiwei]
    u16    WriteMixerTable( u8 byMixNum,  TEqpMixerInfo* ptTable,BOOL32 bIsNeedUpdatePortInTable = TRUE); // 整体写Mix表
    u16    ReadTvTable( u8* pbyTvNum, TEqpTVWallInfo* ptTvTable );  // 整体读TVWall表
    u16    WriteTvTable( u8 byTvNum,  TEqpTVWallInfo* ptTvTable );  // 整体写TVWall表
    u16    ReadRecTable( u8* pbyRecNum, TEqpRecInfo* ptRecTable );  // 整体读Rec表
    u16    WriteRecTable( u8 byRecNum,  TEqpRecInfo* ptRecTable,BOOL32 bIsNeedUpdatePortInTable = TRUE );  // 整体写Rec表
    u16    ReadBasTable( u8* pbyBasNum, TEqpBasInfo* ptBasTable );  // 整体读Bas表
    u16    WriteBasTable( u8 byBasNum,  TEqpBasInfo* ptBasTable,BOOL32 bIsNeedUpdatePortInTable = TRUE );  // 整体写Bas表
    //zw[08/06/2008]    
    u16    ReadBasHDTable( u8* pbyBasNum, TEqpBasHDInfo* ptBasHDTable );  // 整体读BasHD表
    u16    WriteBasHDTable( u8 byBasNum,  TEqpBasHDInfo* ptBasHDTable,BOOL32 bIsNeedUpdatePortInTable = TRUE );  // 整体写BasHD表

    u16    ReadVmpTable( u8* pbyVmpNum, TEqpVMPInfo* ptVmpTable );  // 整体读VMP表
    u16    WriteVmpTable( u8 byVmpNum,  TEqpVMPInfo* ptVmpTable,BOOL32 bIsNeedUpdatePortInTable = TRUE );  // 整体写VMP表
    u16    ReadMpwTable( u8* pbyMpwNum, TEqpMpwInfo* ptMpwTable );  // 整体读MPW表
    u16    WriteMpwTable( u8 byMpwNum,  TEqpMpwInfo* ptMpwTable );  // 整体写MPW表
	//4.6版本 新加外设
	u16    ReadHduTable( u8* pbyHduNum, TEqpHduInfo* ptHduTable );  // 整体读Hdu表
    u16    WriteHduTable( u8 byHduNum,  TEqpHduInfo* ptHduTable );  // 整体写Hdu表
	u16    ReadSvmpTable( u8* pbySvmpNum, TEqpSvmpInfo* ptSvmpTable );  // 整体读Svmp表
    u16    WriteSvmpTable( u8 bySvmpNum,  TEqpSvmpInfo* ptSvmpTable,BOOL32 bIsNeedUpdatePortInTable = TRUE );  // 整体写Svmp表
// 	u16    ReadDvmpTable( u8* pbyDvmpNum, TEqpDvmpBasicInfo* ptDvmpTable );  // 整体读Dvmp表      u16    WriteDvmpTable( u8 byDvmpNum,  TEqpDvmpBasicInfo* ptDvmpTable );  // 整体写Dvmp表
 	u16    ReadMpuBasTable( u8* pbyMpuBasNum, TEqpMpuBasInfo* ptMpuBasTable );  // 整体读MpuBas表
    u16    WriteMpuBasTable( u8 byMpuBasNum,  TEqpMpuBasInfo* ptMpuBasTable,BOOL32 bIsNeedUpdatePortInTable = TRUE );  // 整体写MpuBas表
// 	u16    ReadEbapTable( u8* pbyEbapNum, TEqpEbapInfo* ptEbapTable );  // 整体读Ebap表
//  u16    WriteEbapTable( u8 byEbapNum,  TEqpEbapInfo* ptEbapTable );  // 整体写Ebap表
//	u16    ReadEvpuTable( u8* pbyEvpuNum, TEqpEvpuInfo* ptEvpuTable );  // 整体读Evpu表
//  u16    WriteEvpuTable( u8 byEvpuNum,  TEqpEvpuInfo* ptEvpuTable );  // 整体写Evpu表
    u16    ReadHduSchemeTable( u8* pbyNum, THduStyleInfo* ptCfg );  // 整体读Hdu预案表
    u16    WriteHduSchemeTable( u8 byNum,  THduStyleInfo* ptCfg );  // 整体写Hdu预案表
	u16    ReadVrsRecTable( u8* pbyVrsRecNum, TEqpVrsRecCfgInfo* ptVrsRecTable );  // 整体读Vrs新录播表
	u16    WriteVrsRecTable( u8 byVrsRecNum, TEqpVrsRecCfgInfo* ptVrsRecTable );  // 整体写Vrs新录播表

    u16    ReadVmpAttachTable( u8* pbyNum, TVmpAttachCfg* ptCfg );  // 整体读画面合成风格表
    u16    WriteVmpAttachTable( u8 byNum,  TVmpAttachCfg* ptCfg );  // 整体写画面合成风格表
    u16    ReadPrsTable( u8* pbyPrsNum, TEqpPrsInfo* ptPrsTable );  // 整体读Prs表
    u16    WritePrsTable( u8 byPrsNum,  TEqpPrsInfo* ptPrsTable );  // 整体写Prs表
	BOOL32 RefreshBrdOsType(u8 byBrdId, u8 byOsType);               // 更新某个外设板卡的OSTYPE值

	// lang：读mcu当前编码方式 [pengguofeng 4/16/2013]
	u16    ReadMcuEncodingType(u8 &byEncoding);
	BOOL32 WriteMcuEncodingType(const u8 *pbyEncoding);
	
	// lang [pengguofeng 4/12/2013]
	void SetEncodingType( u8 byEncoding);
	u8	GetEncodingType(void);
	BOOL32	IsUtf8Encoding(void);
	// end [pengguofeng 4/16/2013]

    BOOL32 IsSVmp( u8 byEqpId );
	u16     GetVmpEqpVersion(u8 byEqpId);
	
//    BOOL32 IsDVmp( u8 byEqpId );
//    BOOL32 IsEVpu( u8 byEqpId );
    BOOL32 IsMpuBas( u8 byEqpId );
	u16     GetBasEqpVersion(u8 byEqpId);
	u16		GetHDUEqpVersion(u8 byEqpId);
//    BOOL32 IsEBap( u8 byEqpId );

	BOOL32 GetMpuBasEntry( u8 byEqpId, TEqpMpuBasEntry &tMpuBasEntry);  //获取对应外设id的mpubas信息
	u8     GetIS22BrdIndexByPos(u8 byLayer, u8 bySlot);
	u8	   GetIdlePRSEqpId();	  // 获取当前最小空闲可用的PRS外设ID[9/15/2011 chendaiwei]
	// mcu start optimize, zgc, 2007-02-27
// [3] 配置文件备份的导入导出

	//[3.1] 文件整体复制
	BOOL32 PreReadCfgfile( const s8* lpszCfgfilename );
	BOOL32 CopyCfgSrcfileToDstfile( const s8* lpszSrcfilename, const s8* lpszDstfilename );
	BOOL32 CopyFileBetweenCfgAndBak( const s8* lpszSrcfilename, const s8* lpszDstfilename );
	BOOL32 WriteAllCfgInfoToCfgfile();
	
	//[3.2] 默认配置
	void SetSystemInfoByDefault();         // 系统信息
    void SetLocalInfoByDefault();          // 本地信息
    void SetNetWorkInfoByDefault();        // 网络信息
    void SetTrapRcvTableByDefault();       // Trap服务器信息
    void SetBrdCfgTableByDefault();        // 单板信息
    void SetEqpDcsByDefault();             // 数据会议服务器信息
    void SetPrsInfoByDefault();            // Prs信息
    void SetNecSyncByDefault();            // 网同步信息
    void SetMixerTableByDefault();         // 混音器信息
    void SetRecorderTableByDefault();      // 录像信息    
    void SetTVWallTableByDefault();        // 电视墙信息
    void SetBasTableByDefault();           // Bas信息
    //zw[08/07/2008]
    void SetBasHDTableByDefault();           // BasHD信息
    //4.6版本新加  jlb
	void SetHduTableByDefault();           // Hdu信息
 	void SetSvmpTableByDefault();           // Svmp信息
 	void SetDvmpTableByDefault();           // Dvmp信息
 	void SetMpuBasTableByDefault();           // MpuBas信息
	void SetEbapTableByDefault();           // Ebap信息
	void SetEvpuTableByDefault();           // Evpu信息
    void SetVrsRecorderTableByDefault();    // Vrs新录播信息


    void SetVMPTableByDefault();           // Vmp信息
    void SetQosInfoByDefault();            // Qos信息	
    void SetVmpAttachTableByDefault();     // 画面合成风格
    void SetMpwTableByDefault();           // 多画面电视墙
	void SetEqpExCfgInfoByDefault();      // 外设扩展配置信息
#ifdef _MINIMCU_
	void SetDscInfoByDefault();			   // Dsc信息
#endif

	void SetGkProxyCfgInfoByDefault();
	void SetPrsTimeSpanCfgInfoByDefault();  
//end, zgc, 2007-02-27

// [4].配置信息的读写
    u16 GetEqpRecCfgById( u8 byId, TEqpRecInfo * ptRecCfg );        // 据外设ID取录像机信息
    u16 SetEqpRecCfgById( u8 byId, TEqpRecInfo * ptRecCfg );        // 据外设ID设录像机信息
    u16 GetEqpTVWallCfgById( u8 byId, TEqpTVWallInfo * ptTWCfg );   // 据外设ID取电视墙信息
    u16 SetEqpTVWallCfgById( u8 byId, TEqpTVWallInfo * ptTWCfg );   // 据外设ID设电视墙信息
    u16 GetEqpMixerCfgById( u8 byId, TEqpMixerInfo * ptMixerCfg );  // 据外设ID取混音器信息
    u16 SetEqpMixerCfgById( u8 byId, TEqpMixerInfo tMixerCfg );     // 据外设ID设混音器信息
    u16 GetEqpBasCfgById( u8 byId, TEqpBasInfo * ptBasCfg );        // 据外设ID取Bas信息
    u16 SetEqpBasCfgById( u8 byId, TEqpBasInfo * ptBasCfg );        // 据外设ID设Bas信息

    //zw[08/06/2008] 
    BOOL32 IsEqpBasHD( u8 byId );                                   // 据外设ID判断是否高清bas
	BOOL32 IsEqpBasAud(u8 byEqpId);									//判断BAS是否是音频适配的BAS

    u16 GetEqpBasHDCfgById( u8 byId, TEqpBasHDInfo * ptBasHDCfg );  // 据外设ID取BasHD信息
    u16 SetEqpBasHDCfgById( u8 byId, TEqpBasHDInfo * ptBasHDCfg );  // 据外设ID设BasHD信息
    u16 GetMpuBasCfgById( u8 byId,TEqpMpuBasInfo* ptEqpMpuBasInfo );  // 据外设ID取BasHD信息

    u16 GetEqpVMPCfgById( u8 byId, TEqpVMPInfo * ptVMPCfg );        // 据外设ID取Vmp信息
    u16 SetEqpVMPCfgById( u8 byId, TEqpVMPInfo * ptVMPCfg );        // 据外设ID设Vmp信息
    u16 GetEqpMpwCfgById( u8 byId, TEqpMpwInfo * ptMpwCfg );        // 据外设ID取Mpw信息
    u16 SetEqpMpwCfgById( u8 byId, TEqpMpwInfo * ptMpwCfg );        // 据外设ID设Mpw信息
    u16 GetEqpPrsCfgById( u8 byId, TEqpPrsInfo * ptPrsCfg );        // 据外设ID取PRS信息
    u16 SetEqpPrsCfgById( u8 byId, TEqpPrsInfo * ptPrsCfg );        // 据外设ID设PRS信息

	//4.6版本 新加外设  jlb
    u16 GetEqpHduCfgById( u8 byId, TEqpHduInfo * ptHduCfg );        // 据外设ID取Hdu信息
    u16 SetEqpHduCfgById( u8 byId, TEqpHduInfo * ptHduCfg );        // 据外设ID设Hdu信息
    u16 GetEqpSvmpCfgById( u8 byId, TEqpSvmpInfo * ptSvmpCfg );     // 据外设ID取Svmp信息
    u16 SetEqpSvmpCfgById( u8 byId, TEqpSvmpInfo * ptSvmpCfg );     // 据外设ID设Svmp信息
//  u16 GetEqpDvmpCfgById( u8 byId, TEqpDvmpBasicInfo * ptDvmpCfg );        // 据外设ID取Dvmp信息
//  u16 SetEqpDvmpCfgById( u8 byId, TEqpDvmpBasicInfo * ptDvmpCfg );        // 据外设ID设Dvmp信息
    u16 GetEqpMpuBasCfgById( u8 byId, TEqpMpuBasInfo * ptMpuBasCfg );       // 据外设ID取MpuBas信息
    u16 SetEqpMpuBasCfgById( u8 byId, TEqpMpuBasInfo * ptMpuBasCfg );   // 据外设ID设MpuBas信息
//     u16 GetEqpEbapCfgById( u8 byId, TEqpEbapInfo * ptEbapCfg );        // 据外设ID取Ebap信息
//     u16 SetEqpEbapCfgById( u8 byId, TEqpEbapInfo * ptEbapCfg );        // 据外设ID设Ebap信息
//     u16 GetEqpEvpuCfgById( u8 byId, TEqpEvpuInfo * ptEvpuCfg );        // 据外设ID取Evpu信息
//     u16 SetEqpEvpuCfgById( u8 byId, TEqpEvpuInfo * ptEvpuCfg );        // 据外设ID设Evpu信息

    u16 GetBrdCfgById( u8 byBrdId, TBoardInfo * ptBoardInfo );      // 据外设ID取单板信息
    u16 SetBrdCfgById( u8 byBrdId, TBoardInfo * ptBoardInfo );      // 据外设ID设单板信息

    u16 GetEqpRecCfgByRow( u8 byRow, TEqpRecInfo * ptRecCfg );      // 据配置行号取录像机信息
		//BOOL32 bIsNeedUpdatePortInTable 标识是否需要更新内存表中端口[12/15/2011 chendaiwei]
    u16 SetEqpRecCfgByRow( u8 byRow, TEqpRecInfo * ptRecCfg, BOOL32 bIsNeedUpdatePortInTable = TRUE );      // 据配置行号设录像机信息
    u16 GetEqpTVWallCfgByRow( u8 byRow, TEqpTVWallInfo * ptTWCfg ); // 据配置行号取电视墙信息
    u16 SetEqpTVWallCfgByRow( u8 byRow, TEqpTVWallInfo * ptTWCfg ); // 据配置行号设电视墙信息
    u16 GetEqpMixerCfgByRow( u8 byRow, TEqpMixerInfo * ptMixerCfg );// 据配置行号取混音器信息
	//BOOL32 bIsNeedUpdatePortInTable 标识是否需要更新内存表中端口[12/15/2011 chendaiwei]
    u16 SetEqpMixerCfgByRow( u8 byRow, TEqpMixerInfo tMixerCfg, BOOL32 bIsNeedUpdatePortInTable = TRUE );   // 据配置行号设混音器信息
    u16 GetEqpBasCfgByRow( u8 byRow, TEqpBasInfo * ptBasCfg );      // 据配置行号取Bas信息
	//BOOL32 bIsNeedUpdatePortInTable 标识是否需要更新内存表中端口[12/15/2011 chendaiwei]
    u16 SetEqpBasCfgByRow( u8 byRow, TEqpBasInfo * ptBasCfg,BOOL32 bIsNeedUpdatePortInTable = TRUE );      // 据配置行号设Bas信息

    //zw[08/07/2008]
    u16 GetEqpBasHDCfgByRow( u8 byRow, TEqpBasHDInfo * ptBasHDCfg );// 据配置行号取BasHD信息
	//BOOL32 bIsNeedUpdatePortInTable 标识是否需要更新内存表中端口[12/15/2011 chendaiwei]
    u16 SetEqpBasHDCfgByRow( u8 byRow, TEqpBasHDInfo * ptBasHDCfg,BOOL32 bIsNeedUpdatePortInTable = TRUE );// 据配置行号设BasHD信息

    u16 GetEqpVMPCfgByRow( u8 byRow, TEqpVMPInfo * ptVMPCfg );      // 据配置行号取Vmp信息
	//BOOL32 bIsNeedUpdatePortInTable 标识是否需要更新内存表中端口[12/15/2011 chendaiwei]
    u16 SetEqpVMPCfgByRow( u8 byRow, TEqpVMPInfo * ptVMPCfg, BOOL32 bIsNeedUpdatePortInTable = TRUE  );      // 据配置行号设Vmp信息
    u16 GetEqpMpwCfgByRow( u8 byRow, TEqpMpwInfo * ptMpwCfg );      // 据配置行号取Mpw信息
    u16 SetEqpMpwCfgByRow( u8 byRow, TEqpMpwInfo * ptMpwCfg );      // 据配置行号设Mpw信息
    u16 GetEqpPrsCfgByRow( u8 byRow, TEqpPrsInfo * ptPrsCfg );      // 据配置行号取PRS信息
    u16 SetEqpPrsCfgByRow( u8 byRow, TEqpPrsInfo * ptPrsCfg );      // 据配置行号设PRS信息  

    //4.6版本 新加外设  jlb
    u16 GetEqpHduCfgByRow( u8 byRow, TEqpHduInfo * ptHduCfg );      // 据配置行号取Hdu信息
    u16 SetEqpHduCfgByRow( u8 byRow, TEqpHduInfo * ptHduCfg );      // 据配置行号设Hdu信息
	u16 GetEqpSvmpCfgByRow( u8 byRow, TEqpSvmpInfo * ptSvmpCfg );      // 据配置行号取Svmp信息
	//BOOL32 bIsNeedUpdatePortInTable 标识是否需要更新内存表中端口[12/15/2011 chendaiwei]
    u16 SetEqpSvmpCfgByRow( u8 byRow, TEqpSvmpInfo * ptSvmpCfg,BOOL32 bIsNeedUpdatePortInTable = TRUE  );      // 据配置行号设Svmp信息  
//  u16 GetEqpDvmpCfgByRow( u8 byRow, TEqpDvmpBasicInfo * ptDvmpCfg );      // 据配置行号取Dvmp信息
//  u16 SetEqpDvmpCfgByRow( u8 byRow, TEqpDvmpBasicInfo * ptDvmpCfg );      // 据配置行号设Dvmp信息  
    u16 GetEqpMpuBasCfgByRow( u8 byRow, TEqpMpuBasInfo * ptMpuBasCfg );     // 据配置行号取MpuBas信息
	//BOOL32 bIsNeedUpdatePortInTable 标识是否需要更新内存表中端口[12/15/2011 chendaiwei]
    u16 SetEqpMpuBasCfgByRow( u8 byRow, TEqpMpuBasInfo * ptMpuBasCfg,BOOL32 bIsNeedUpdatePortInTable = TRUE  );     // 据配置行号设MpuBas信息  
//     u16 GetEqpEbapCfgByRow( u8 byRow, TEqpEbapInfo * ptEbapCfg );      // 据配置行号取Ebap信息
//     u16 SetEqpEbapCfgByRow( u8 byRow, TEqpEbapInfo * ptEbapCfg );      // 据配置行号设Ebap信息
//	u16 GetEqpEvpuCfgByRow( u8 byRow, TEqpEvpuInfo * ptEvpuCfg );      // 据配置行号取Evpu信息
//  u16 SetEqpEvpuCfgByRow( u8 byRow, TEqpEvpuInfo * ptEvpuCfg );      // 据配置行号设Evpu信息  
    u16 GetEqpVrsRecCfgByRow( u8 byRow, TEqpVrsRecCfgInfo * ptVrsRecCfg );      // 据配置行号取Vrs新录播信息
    u16 SetEqpVrsRecCfgByRow( u8 byRow, TEqpVrsRecCfgInfo * ptVrsRecCfg );      // 据配置行号设Vrs新录播信息

    
    u16 GetBrdInfoByRow( u8 byRow, TEqpBrdCfgEntry * ptBrdCfgEnt, BOOL32 bSnmp = FALSE ); // 据配置行号取单板信息
    u16 SetBrdInfoByRow( u8 byRow, TEqpBrdCfgEntry * ptBrdCfgEnt ); // 据配置行号设单板信息
    u16 GetBrdInfoById( u8 byBrdId, TEqpBrdCfgEntry * ptBrdCfgEnt );// 据外设ID取单板信息
    u16 SetBrdInfoById( u8 byBrdId, TEqpBrdCfgEntry * ptBrdCfgEnt );// 据外设ID设单板信息
    u16 GetLocalMpcInfo( TEqpBrdCfgEntry * ptBrdCfgEnt);            // 取对端mcu的TEqpBrdCfgEntry信息
	
	u8		GetBrdIdxFrmId(u8 byBrdId);
	u8		GetBrdIdFrmIdx(u8 byBrdIdx);
	u8      GetEqpSwitchBrdId(u8 byEqpId);
	BOOL32  SetEqpSwitchBrdId(u8 byEqpId, u8 bySwitchBrdId);
	
    u16 SetQosInfo( TQosInfo * ptQosInfo ) ;                        // 修改Qos信息
    u16 GetQosInfo( TQosInfo * ptQosInfo );                         // 取Qos信息
    u16 SetLocalInfo( TLocalInfo * ptLocalInfo,u32 dwNodeValue = 0) ;                  // 修改本地信息
    u16 GetLocalInfo( TLocalInfo * ptLocalInfo );                   // 取本地信息
    u16 SetTrapInfo( u8 byRow, TTrapInfo * ptTrapInfo );			// 修改单个Trap服务器信息
    u16 GetTrapInfo( u8 byRow, TTrapInfo * ptTrapInfo );			// 取单个Trap服务器信息
    u16 GetTrapInfo( TTrapInfo* ptTrapInfo );                       // 取整个Trap表信息
    u16 SetNetWorkInfo( TNetWorkInfo * ptNetWorkInfo,u32 dwNodeValue = 0 );             // 修改网络配置信息
    u16 GetNetWorkInfo( TNetWorkInfo * ptNetWorkInfo );             // 取网络配置信息
    u16 SetNetSyncInfo( TNetSyncInfo * ptNetSyncInfo );             // 修改网同步信息
    u16 GetNetSyncInfo( TNetSyncInfo * ptNetSyncInfo );             // 取网同步信息
	u16 SetLoginInfo( TLoginInfo *ptLoginInfo );					// 修改登陆信息
	u16 GetLoginInfo( TLoginInfo *ptLoginInfo );					// 取登陆信息

    u16 GetSystemInfo( TMcuSystem * ptMcuSysInfo );                 // 取系统信息
    u16 SetSystemInfo( TMcuSystem * ptMcuSysInfo );                 // 修改系统信息

    u16 GetMcuPfmInfo( TMcuPfmInfo * ptMcuPfmInfo );                 // 获取mcu Performance Info
    //u16 SetMcuPfmInfo( TMcuPfmInfo * ptMcuPfmInfo );                 // 设置mcu Performance Info
	TMcuPfmInfo* GetMcuPfmInfo();

	u16 GetMcuUpdateInfo( TMcuUpdateInfo * ptMcuUpdateInfo );
	u16 SetMcuUpdateInfo( TMcuUpdateInfo * ptMcuUpdateInfo );
	TMcuUpdateInfo* GetMcuUpdateInfo();

#ifdef _MINIMCU_
    u16 GetDSCInfo( TDSCModuleInfo * ptDscInfo );					// 取8000B的dsc特殊配置信息
	u16 SetDSCInfo( TDSCModuleInfo * ptDscInfo, BOOL32 bToFile );   // 修改8000B的dsc特殊配置信息
	u16 GetLastDscInfo( TDSCModuleInfo * ptDscInfo );				// 取保存的修改之前的DSC特殊配置信息
	u16 SetLastDscInfo( TDSCModuleInfo * ptDscInfo );				// 保存修改之前的DSC特殊配置信息
	// 把DSC info设置到MINIMCU MPC板上, zgc, 2007-07-17
	BOOL32 SaveDscLocalInfoToNip( TDSCModuleInfo * ptDscInfo );	
		// 设置路由到MINIMCU MPC板上, zgc, 2007-09-21
	BOOL32 SaveRouteToNipByDscInfo( TDSCModuleInfo *ptDscInfo );	
    void SetDscInnerIp(u32 dwIp);                                   // 设置dsc板的连接用IP
	u8   GetConfigedDscType( void );								// 获得配置过的Dsc板的类型，如没有配置过，则返回0
	BOOL32 IsDscReged(u8 &byDscType);								// 判断DSC是否注册	
#endif

	u8 GetWatchDogOption();		// 获取WATCH DOG的配置, zgc

	// [1/14/2010 xliang] // 多网卡，路由配置相关
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
#ifdef _8KI_
	u16		SetNewNetCfgInfo(const TNewNetCfg &tNewNetcfgInfo, const s8 bySecEthIndx = -1);	//设置8KI支持3网口网络配置 
	u16		GetNewNetCfgInfo(TNewNetCfg &tNewNetCfgInfo);		//获取8KI支持3网口网络配置 
	u16		WritePxyMultiNetInfo( void );
	void	SwitchMcuEqpCfgToNewNetCfg( TMcu8KECfg &tMcuEqpCfg, TNewNetCfg &tNewNetCfg);
#endif
	u16		SetMcu8KIEqpCfg( TMcu8KECfg * ptMcuEqpCfg ); //设置8KI网络参数 [5/7/2012 chendaiwei]
	u16		SetMcuEqpCfg( TNetAdaptInfoAll * ptNetAdaptInfoAll );		
	u16		GetMcuEqpCfg( TMcu8KECfg * ptMcuEqpCfg );                // 获得MCU设备信息
	
	void	SetRouteToSys( void );								//从配置文件获取路由信息并将路由配到系统中 
	u16		SetRouteCfg(TMultiNetCfgInfo &tMultiNetCfgInfo);	//将路由信息配到系统中
	u16		GetMultiNetCfgInfo(TMultiNetCfgInfo &tMultiNetCfgInfo);
	
	u16		SetMultiManuNetCfg(const TMultiManuNetCfg &tMultiManuNetCfg,
							   const TMultiEthManuNetAccess *tMultiEthManuNetAccess, const u8 byEthNum);
	u16     WriteMultiManuNetAccess(const TMultiManuNetAccess &tMultiManuNetAccess,
									const TMultiEthManuNetAccess *tMultiEthManuNetAccess, const u8 byEthNum);
	u16     GetMultiManuNetAccess(TMultiManuNetAccess &tMultiManuNetAccess,BOOL32 bFromMcuCfgFile = FALSE,
								  TMultiEthManuNetAccess *tMultiEthManuNetAccess = NULL, u8 *byEthNum = NULL);
	//void	Make8KINetCfgEffect( void );
	u16		Save8KENetCfgToSys(TMcu8KECfg * ptMcu8KECfg);
	BOOL32  EthInit( void );

	u16		WriteProxyIp( TNetParam tNtParam );
#endif

#ifdef _LINUX_
	u32 EthIdxSys2Hard( u32 dwSysIdx );
	u32 EthIdxHard2Sys( u32 dwHardIdx );
#endif
#ifdef WIN32
	BOOL32 AdapterDiscript2EthIdx( const s8 * pchAdapterDiscript, u32 &dwEthIdx );
	BOOL32 EthIdx2AdapterDiscript( const u32 dwEthIdx, s8 * pchAdapterDiscript );
#endif


// [5].单板取外设信息（单板管理）
    BOOL32 GetRecCfgToBrd( u8 byId, TEqpRecEntry * ptRecCfg );      // 取录像机
    BOOL32 GetTvCfgToBrd( u8 byId, TEqpTVWallEntry * ptTWCfg );     // 取电视墙信息
    BOOL32 GetMixCfgToBrd( u8 byId, TEqpMixerEntry * ptMixCfg );    // 取混音器信息
    BOOL32 GetBasCfgToBrd( u8 byId, TEqpBasEntry * ptBasCfg );      // 取Bas信息
    BOOL32 GetVmpCfgToBrd( u8 byId, TEqpVMPEntry * ptVMPCfg );      // 取Vmp信息
    BOOL32 GetPrsCfgToBrd( u8 byId, TEqpPrsEntry * ptPrsCfg );      // 取Prs信息
    BOOL32 GetMpwCfgToBrd( u8 byId, TEqpMPWEntry * ptMpwCfg );      // 取Mpw信息

    //4.6版本新加 jlb 
    BOOL32 GetHduCfgToBrd( u8 byId, TEqpHduEntry * ptHduCfg );      // 取Hdu信息
    BOOL32 GetSvmpCfgToBrd( u8 byId, TEqpSvmpEntry * ptSvmpCfg );      // 取Svmp信息
//    BOOL32 GetDvmpCfgToBrd( u8 byId, TEqpDvmpBasicEntry * ptDvmpBasicCfg );      // 取Dvmp信息
    BOOL32 GetMpuBasCfgToBrd( u8 byId, TEqpMpuBasEntry * ptMpuBasCfg );      // 取MpuBas信息
//    BOOL32 GetEbapCfgToBrd( u8 byId, TEqpEbapEntry * ptEbapCfg );      // 取Ebap信息
//    BOOL32 GetEvpuCfgToBrd( u8 byId, TEqpEvpuEntry * ptEvpuCfg );      // 取Evpu信息
        
// [6].其他单个杂项读写

    //[6.1] snmp
    u8     GetTrapServerNum( void );								// 取Trap服务器数
    void   GetTrapTarget( u8 byIndex, TTarget& tTrapTarget );		// 取Trap信息结构
    BOOL32 GetSnmpParam( TSnmpAdpParam& tParam ) const;             // 取SNMP参数
    BOOL32 HasSnmpNms( void ) const;                                // 取是否配置了网管

    //[6.2] dcs
    u32    GetDcsIp( void );										// 取数字会议服务器Ip
    u16    SetDcsIp( u32 dwDcsIp );								    // 设置数字会议服务器Ip

    //[6.3] board
	u32    GetBoardNum( void );
    u8     GetBrdId( TBrdPosition tBrdPositon );                    // 取单板ID
    u32    GetBrdIpAddr( u8 byBrdPosId );							// 由单板Id得到单板Ip
	u32	   GetBrdIpByAgtId( u8 byBrdAgtId );						// 由单板索引得到单板Ip, zgc, 2007-03-22
    u8     GetCriDriIsCast( u8 byBrdId );                           // 由单板ID得到是否组播 (FIXME: byBrdId 特殊标识)
    void   ChangeBoardStatus( TBrdPosition tPos, u8 byStatus );     // 更改单板在线状态
    BOOL32 ChangeBrdRunEqpNum( u8 byBrdId, u8 byEqpId );		    // 修改指定单板运行得外设数
    BOOL32 IsConfigedBrd( TBrdPosition tBrdPositon ); 		        // 是否配置该单板
    u16    GetPriInfo( u8 byEqpId, u32* pdwEqpIp, u8* pbyEqpType ); // 得到外设信息
    u8    GetBoardRow( u8 byLayer, u8 bySlot, u8 byType );        // 取单板行号
    u16    RebootBoard( u8 byLayer, u8 bySlot, u8 byType );         // 重启指定单板
    BOOL32 CheckEqpRow( u8 byType, u8 byRow, BOOL32 bHDFlag = FALSE );// 检测外设行号 zw[08/07/2008]最后一个参数TURE为高清
    LPCSTR GetBrdTypeStr( u8 byType );			                    // 得到类型字符串
    void   GetBrdSlotString( u8 bySlot, u8 byType, s8* pszStr );	// 得到槽字符串
    BOOL32 GetSlotThroughName( const s8* lpszName, u8* pbySlot );	// 从槽字符串得到槽号
    BOOL32 GetTypeThroughName( const s8* lpszName, u8* pbyType );	// 从类型字符串得到类型号
    void   UpdateBrdSoftware( TBrdPosition tBrdPostion, CServMsg &cServMsg ); // 升级单板文件

    //[6.4] mpc
    void   GetMPCInfo( TMPCInfo *ptMPCInfo );                       // 取MPC信息(含主备)
    void   SetMPCInfo( TMPCInfo tMPCInfo );                         // 设MPC信息(含主备)

    u32    GetLocalIp();                                            // 取MPC本地地址
    u16    SetLocalIp( u32 dwIp, u8 byInterface );                  // 设MPC本地地址
    u32    GetMpcMaskIp();                                          // 取MPC子网掩码
    u16    SetMpcMaskIp( u32 dwIp, u8 byInterface );                // 设MPC子网掩码
    u32    GetMpcGateway();                                         // 取MPC网关
    u16    SetMpcGateway( u32 dwIp );                               // 设MPC网关
    u8     GetMpcHWVersion();                                       // 取MPC硬件版本号
    u8     GetCpuRate( void );										// 查询Cpu 占有率
	u32	   GetMemoryRate(void);										// 查询Memory 占有率
	void   SetMcuCompileTime(s8 *pszCompileTime);					// 设置mcu编译时间
	s8*	   GetMcuCompileTime(u16 &wLen);							// 获取mcu编译时间
	void   SetTempPortKind(u8 byPortKind);
	u8	   GetTempPortKind();

	u8   GetMac(s8 *pszBuffer);


    void   SetMpcActive( BOOL32 bEnabled );                         // 业务主备板设置(TRUE: ACTIVE, FALSE: STANDBY)
    BOOL32 IsMpcActive( void );                                     // 是否为业务主板(TRUE: ACTIVE, FALSE: STANDBY)
    
#ifdef _MINIMCU_
    BOOL32 IsMcu8000BHD() const;
#endif

    u8     GetMcuLayerCount( void );                                // 取mcu总层数
    BOOL32 GetImageFileName( u8 byType, s8 *pszFileName );          // 得到升级image文件名
	BOOL32 GetBinFileName( u8 byBrdType, s8 *pszFileName );          //得到升级bin文件名
    BOOL32 UpdateMpcSoftware( s8* lpszMsg, u16 wMsgLen, u8 byFileNum, u8 *pbyRet, BOOL32 bNMS = FALSE ); // 升级MPC 文件
    BOOL32 SmUpdateMpcSoftware();

	BOOL32 Update8KXMpcSoftware( const s8* szFileName,TBrdPosition tMpcPosition,BOOL32 &bIsCheckFileError );

//#ifdef _8KE_
//	u8     Search8KEPortChoice();
//#endif
	u8	   SearchPortChoice( u32 dwIpAddr = 0 );	// zgc, 2008-05-14
	BOOL32   SaveMpcInfo( u8 byLocalMpcIdx, TEqpBrdCfgEntry *ptLocalMpc, u8 byOtherMpcIdx = MAX_BOARD_NUM, TEqpBrdCfgEntry *ptOtherMpc = NULL, BOOL32 bSemTake = FALSE ); // zgc, 2008-05-14

	void SetMpcUpateStatus(s32 dwStat = 1);//zhouyiliang 20120208 设置mpc升级标志，如果mcs升级mcu或者网管升级mcu，则将升级标志设为1



    //[6.5] system    
    void   SetSystemState( u8 byState );                            // 系统状态设置    
    u16    SetSystemTime( time_t  tTime );					        // 设置系统时间
    void   SyncSystemTime( const s8* pszTime );	                    // MPC 板时间同步
    u16    SetSystemCfgVer( s8* achVersion );                       // 设置系统配置版本号
    void   SyncBoardTime( u8* pbyBuf );                             // 其他单板的时间同步
    void   SnmpRebootBoard( TBrdPosition tBrdPostion );             // 网管重启单板
    void   BitErrorTest( TBrdPosition tBrdPostion );                // SNMP位错误测试
    void   BoardSelfTest( TBrdPosition tBrdPostion );				// SNMP单板自测

	//支持MCU导航式配置
	u16    SetIsMcuConfiged( BOOL32 bIsConfiged);					// 设置Mcu配置标识
	BOOL32 IsMcuConfiged();											// 判断Mcu配置标识

	// 配置信息的级别
	void	SetMcuCfgInfoLevel( u8 byLevel );						// 设置MCU配置信息级别
	u8		GetMcuCfgInfoLevel();									// 获得MCU配置信息级别

    //[6.6] qos
    u8     GetQosIpType( u8* pchIpType);                 // 将配置文件中的QosIp服务数字字符串转换为对应的Qos数值
    u8     CombineQosValue( u8 byCombineValue, u8 byType );         // 组合Qos值
    BOOL32 IsValidQosIpTypeStr( u8* pchIpTypeStr );                 // 检测Qos字符串是否合法 
    BOOL32 GetQosStrFromNum( u8 byType, u8* pchQosIpType, u8 byLen );// 由数字得到QosIp服务字符串

    //[6.7] VCS
	u16    SetVCSSoftName(const s8* pachSoftName);
	void   GetVCSSoftName(s8* pachSoftName);

	u16    SetEqpExCfgInfo( TEqpExCfgInfo &tEqpExCfgInfo );
	u16    GetEqpExCfgInfo( TEqpExCfgInfo &tEqpExCfgInfo );
	BOOL32 AgentGetEqpExCfgInfo( const s8* lpszProfileName );

	//[6.8] GK/Proxy/Prs --exclude for 8000E
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	u16		SetGkProxyCfgInfo( TGKProxyCfgInfo &tGKProxyCfgInfo);
	u16		GetGkProxyCfgInfo( TGKProxyCfgInfo &tGKProxyCfgInfo );

	u16     SetProxyDMZInfo( TProxyDMZInfo &tProxyDMZInfo );
	u16     GetProxyDMZInfo( TProxyDMZInfo &tProxyDMZInfo );
	u16		GetProxyDMZInfoFromMcuCfgFile( TProxyDMZInfo &tProxyDMZInfo );

	u16		SetPrsTimeSpanCfgInfo( TPrsTimeSpan &tPrsTimeSpan );
	u16		GetPrsTimeSpanCfgInfo( TPrsTimeSpan &tPrsTimeSpan );

	u16		SetSipRegGkInfo(u32 dwGKIp);
#endif	
// [7] 其他辅助功能函数
public:
    BOOL32 CreateSemHandle( SEMHANDLE &hSem );						// 创建信号量
    BOOL32 FreeSemHandle( SEMHANDLE &hSem );						// 释放信号量
	BOOL32 FreeAllSemHandle();										// 释放所有信号量
    void   GetIpFromU32( s8* achDstStr, u32 dwIpAddr );				// 得到IP字符串
    BOOL32 TableMemoryFree( void **ppMem, u32 dwEntryNum );         // 释放表内存
    u8     BrdStatusMcuVc2Agt(u8 byStatusIn);                       // 单板状态值 业务转到代理
    u8     BrdStatusAgt2McuVc(u8 byStatusIn);                       // 单板状态值 代理转到业务
    u8     BrdStatusHW2Agt(u8 byStatusIn);                          // 单板状态值 底层转到代理(只读。所以未实现代理转到底层函数)
    u8     BrdStatusAgt2NMS(u8 byStatusIn);                         // 单板状态值 代理转到NMS(只读)
    u8     PanelLedStatusAgt2NMS( TEqpBrdCfgEntry& tBrdCfg, s8* pszLedStr );   // 灯状态值 代理转到NMS(只读)  [2012/05/09 liaokang]
    u8     EthPortStatusAgt2NMS( TEqpBrdCfgEntry& tBrdCfg, s8* pszEthPortStr );// 网口状态值 代理转到NMS(只读)[2012/05/09 liaokang]
    u8     GetBrdInstIdByPos( const TBrdPosition tBrdPos );         // 取单板对应的实例号
    u8     GetRunBrdTypeByIdx (  u8 byBrdIdx );                     // 通过板的层槽组合索引查询运行板类型
    BOOL32 IsMcuPdtBrdMatch(u8 byPdtType) const;                    // 检查业务与MPC板类型是否匹配
    u8     McuSubTypeAgt2Snmp(u8 byMcuSubType);                     // MCU子类型转换

	u8     GetSnmpBrdTypeFromMcu(u8 byMcuType);							// 获取VC所需的单板类型
    u8     GetMcuBrdTypeFromSnmp(u8 byNmsType);                            // 获取snmp所需的单板类型

	u8     GetSnmpBrdTypeFromHW(u8 byHWType);							// 获取VC所需的单板类型
    u8     GetHWBrdTypeFromSnmp(u8 bySnmpType);                            // 获取snmp所需的单板类型

	void   PreTreatMpcNIP(void);									// 检查MPC的NIP是否配置了IP
    void   GetHdBasCfgFromMpuBas(TEqpBasHDInfo &tHdBasInfo,
                                 TEqpMpuBasInfo &tMpuBasInfo);      // 保持对MCU接口的一致性

	// windows下设置取Ip, maskIp, Gateway地址
    BOOL32 RegGetAdpName( s8* lpszAdpName, u16 wLen );
    BOOL32 RegSetIpAdr( s8* lpszAdpName, s8* pIpAddr, u16 wLen );
/*    BOOL32 RegGetIpAdr( s8* lpszAdpName, s8* pIpAddr, u16 wLen );*/
    BOOL32 RegSetNetGate( s8* lpszAdpName, s8* pNetGate, u16 wLen );
    BOOL32 RegGetNetGate( s8* lpszAdpName, s8* pNetGate, u16 wLen );
    BOOL32 RegSetNetMask( s8* lpszAdpName, s8* pNetMask, u16 wLen );
    BOOL32 RegGetNetMask( s8* lpszAdpName, s8* pNetMask, u16 wLen );
    
    void   GetEntryString( s8* achStr, u8 byIdx );					                                    // 得到Entry字符串
    u16    WriteStringToFile( const s8* plszSectionName, const s8* plszKeyName, const s8* pszValue );   // 写字符串到文件
	u16    WriteIntToFile( const s8* plszSectionName, const s8* plszKeyName, const s32 sdwValue );      // 写数字到文件
    u16    WriteTableEntryNum( s8* achFileName, s8* achTableName, s8* achEntryNumStr, u8 byEntryNum );  // 写表项入口索引
    u16    WriteTableEntry( const s8* achFileName, const s8* achTableName, const s8* achEntry, const s8* achConfInfo, BOOL32 bSemTake = TRUE ); // 写表项内容

	// [3/27/2010 xliang] expand interface
	u16		WriteStringToFile( const s8* plszFileName, const s8* plszSectionName, const s8* plszKeyName, const s8* pszValue );
	u16		WriteIntToFile( const s8* plszFileName, const s8* plszSectionName, const s8* plszKeyName, const s32 sdwValue );

	// 对读取配置表函数进行重新封装, zgc, 2007-03-20
	BOOL32 AgtGetRegKeyStringTable( const s8* lpszProfileName,      /* 文件名（含绝对路径） */
		                            const s8* lpszSectionName,      /* Profile中的段名      */ 
		                            const s8* lpszDefault,          /* 失败时返回的默认值   */
		                            s8* *const lpszEntryArray,            /* 返回字符串数组指针   */
		                            u32 *dwEntryNum,                /* 字符串数组数目，成功后返回字符串数组实际数
                                                                       目，如传入的数目不够则多余的记录不予传回 */
		                            u32 dwBufSize                   /* 返回字串的长度，如不够则截断，最
                                                                       后一个字节填上'\0'   */
		                            );

    // 发消息给单板管理
    BOOL32 PostMsgToBrdMgr( const TBrdPosition &tBrdPosition, u16 wEvent, const void* pvContent = NULL, u16 wLen = 0 );

	// 统计MCU8000G的Cpu和内存的占用率 mqs [2010-11-30]
#ifdef _LINUX_
	BOOL32 Get8KECpuInfo( T8KECpuInfo* pt8KECpuInfo );
	BOOL32 Get8KEMemoryInfo( T8KEMemInfo* pt8KEMemInfo);
#endif

public:
    void   ShowLocalInfo( void );                               // 显示本地信息
	void   ShowNetInfo( void );                                 // 显示网络信息
	void   ShowNetsyncInfo( void );                             // 显示网同步信息  
    void   ShowBoardInfo( void );                               // 显示Board信息
    void   ShowMixerInfo( void );                               // 显示Mix信息
    void   ShowBasInfo( void );                                 // 显示Bas信息
    void   ShowRecInfo( void );                                 // 显示Rec信息
	void   ShowTvInfo( void );                                  // 显示TvWall信息
	void   ShowVmpInfo( void );                                 // 显示Vmp信息
	void   ShowMpwInfo( void );                                 // 显示Mpw信息
    void   ShowPrsInfo( void );                                 // 显示Prs信息
    
    //4.6版本新加 jlb 
    void   ShowHduInfo( void );                                 // 显示Hdu信息
    void   ShowSvmpInfo( void );                                // 显示Svmp信息
    void   ShowMpuBasInfo( void );                              // 显示MpuBas信息
    void   ShowMauBasInfo( void );                              // 显示MauBas信息



	void   ShowDcsInfo( void );                                 // 显示Dcs信息
    void   ShowQosInfo( void );                                 // 显示Qos信息
    void   ShowVmpAttachInfo( void );                           // 显示VmpAttach信息
    void   ShowDscInfo( void );                                 // 显示8000B DCS板信息
    void   ShowSemHandleInfo( void );                           // 显示代理信号量句柄信息

public:
    void   SetPowerOffMsgDst( u16 wEvent, u32 dwDstId );        // 设置Mpc待机事件号
    void   SetRebootMsgDst( u16 wEvent, u32 dwDstId );          // 设置Mpc重启事件号
    void   SetRunningMsgDst( u16 wEvent, u32 dwDstId );         // 设置Mpc运行事件号

	//整对mcs还没有对dri16做出调整的情况下，临时修改配置
	void /*CCfgAgent::*/AdjustConfigedBrdDRI16(TBrdPosition tBrdPosition);

	//[2011/02/11 zhushz] mcs修改mcu ip
	void GetNewMpcNetCfg(TMcuNewNetCfg& tMcuNewNetInfo);				//得到mcs设置的新网络配置
	void SetNewMpcNetCfg(const TMcuNewNetCfg& tMcuNewNetInfo);			// 设置mcs设置的新网络配置
	BOOL32 SetIsNetCfgBeModifed(BOOL32 bIsNetCfgModify);				//设置是否mcs设置的新网络配置标志
	BOOL32 IsMpcNetCfgBeModifedByMcs();									//网络配置是否被mcs修改
	void WriteFailedTimeForOpenMcuCfg(s32 nErrno);
	void ShowEqpInfo( void ); //显示外设信息[2/16/2012 chendaiwei]

    // 新增 处理在线MCS的IP信息  [04/24/2012 liaokang]
    BOOL32 AddRegedMcsIp( u32 dwMcsIpAddr );                                    // 添加在线MCS的IP信息
    BOOL32 DeleteRegedMcsIp( u32 dwMcsIpAddr );                                 // 删除在线MCS的IP信息
    BOOL32 GetRecombinedRegedMcsIpInfo( s8 *pchRegedMcsIpInfo, u16 &wBufLen );  // 获取重组的在线MCS的IP信息
	u8 GetBrdIdbyIpAddr(u32 dwIpAddr);
#ifdef _8KH_
	void SetMcuType800L( void );
	void SetMcuType8000Hm(void);
	BOOL32 Is800LMcu( void );
	BOOL32 Is8000HmMcu( void );
#endif	

private:
	s32 MoveToSectionStart( FILE *stream, const s8* lpszSectionName, BOOL32 bCreate );
	s32 MoveToSectionEnd( FILE *stream, const s8* lpszSectionName, BOOL32 bCreate );
	s32	DelSection( s8 *pszfilename, const s8* lpszSectionName );
	
private:

    // 本地信息变量
    TLocalInfo			m_tMcuLocalInfo;                        // 本地信息
    TMcuSystem          m_tMcuSystem;                           // 系统信息
    TMPCInfo            m_tMPCInfo;                             // MPC信息(含主备)    
	TEqpExCfgInfo       m_tEqpExCfgInfo;                       // 外设扩展信息
	
	TGKProxyCfgInfo		m_tGkProxyCfgInfo;						//gk/代理配置// [3/16/2010 xliang]
	TProxyDMZInfo       m_tProxyDMZInfo;						//DMZ相关配置信息
	TPrsTimeSpan		m_tPrsTimeSpan;
    
    TMcu8KECfg          m_tMcuEqpCfg;							//包含MPC多网卡，路由配置// [1/14/2010 xliang] 

    // 设备组的变量 
    u32					m_dwBasEntryNum;                        // Bas数目

    //zw[08/06/2008]
    u32                 m_dwBasHDEntryNum;                      // BasHD数目 

    u32					m_dwMixEntryNum;                        // Mix数目 
    u32                 m_dwMpwEntryNum;                        // Mpw数目
    u32                 m_dwPrsEntryNum;                        // Prs数目
    u32					m_dwVMPEntryNum;                        // Vmp数目
    u32					m_dwRecEntryNum;                        // Rec数目
    u32					m_dwTVWallEntryNum;                     // TvWall数目
    u32					m_dwBrdCfgEntryNum;                     // 单板数目
    
    //4.6新加外设 jlb
    u32                 m_dwHduEntryNum;                        // Hdu数目
    u32                 m_dwEbapEntryNum;                       // Ebap数目
    u32                 m_dwEvpuEntryNum;                       // Evpu数目
	u32                 m_dwHduSchemeNum;                       // Hdu预案数目
	u32                 m_dwSvmpEntryNum;                       // Svmp数目
	u32                 m_dwDvmpEntryNum;                       // MAXNUM = 8  8*2
    u32                 m_dwMpuBasEntryNum;                     //  MpuBas数目
	u32					m_dwVrsRecEntryNum;						// VRS新录播数目

    u32                 m_dwVmpAttachNum;					    // VMP风格数目
    TEqpBasEntry		m_atBasTable[MAX_PRIEQP_NUM];	        // Bas表
    
    //zw[08/07/2008]
    TEqpBasHDEntry		m_atBasHDTable[MAX_PRIEQP_NUM];	        // BasHD表

    TEqpMixerEntry	    m_atMixTable[MAX_PRIEQP_NUM];	        // Mix表
    TEqpMPWEntry        m_atMpwTable[MAX_PRIEQP_NUM];           // Mpw表
    TEqpPrsEntry        m_atPrsTable[MAX_PRIEQP_NUM];		    // Prs表
    TEqpVMPEntry		m_atVMPTable[MAX_PRIEQP_NUM];	        // Vmp表
    TEqpRecEntry		m_atRecTable[MAX_PRIEQP_NUM];	        // Rec表
    TEqpTVWallEntry		m_atTVWallTable[MAX_PRIEQP_NUM];        // TvWall表
    TEqpBrdCfgEntry	    m_atBrdCfgTable[MAX_BOARD_NUM];		    // 单板表
    TVmpAttachCfg       m_atVmpAttachCfg[MAX_VMPSTYLE_NUM];	    // VMP风格表
	//4.6新加外设  jlb
	TEqpHduEntry        m_atHduTable[MAX_PERIHDU_NUM];          // Hdu表
 	TEqpSvmpEntry       m_atSvmpTable[MAX_PRIEQP_NUM];          // Svmp表
//	TEqpDvmpBasicEntry  m_atDvmpBasicTable[MAX_PRIEQP_NUM];     // BasicDvmp表
	TEqpMpuBasEntry     m_atMpuBasTable[MAX_PRIEQP_NUM];        // MpuBas表
//	TEqpEbapEntry       m_atEbapTable[MAX_PRIEQP_NUM];          // Ebap表
//	TEqpEvpuEntry       m_atEvpuTable[MAX_PRIEQP_NUM];          // Evpu表
	THduStyleInfo       m_atHduSchemeTable[MAX_HDUSTYLE_NUM];   // HDU预案表
	TEqpVrsRecEntry		m_atVrsRecTable[MAX_PRIEQP_NUM];		// VRS新录播表

    TEqpDCSEntry		m_tDCS;						            // 数字会议服务器   
        
    // 网络组的变量  
    u32					m_dwTrapRcvEntryNum;				    // Trap服务器数目
    TTrapInfo			m_atTrapRcvTable[MAXNUM_TRAP_LIST];	    // Trap服务器表
    TNetWorkInfo		m_tMcuNetwork;						    // 网络配置信息
    TEqpNetSync			m_tNetSync;							    // 网同步   
    TQosInfo            m_tMcuQosInfo;						    // Qos 信息
    TDSCModuleInfo      m_tMcuDscInfo;                          // MCU8000B特殊DSC配置信息
	TDSCModuleInfo		m_tLastMcuDscInfo;						// MCU8000B修改前的特殊DSC配置信息
	
	// VCS
	s8                  m_achVCSSoftName[MAX_VCSSOFTNAME_LEN];     // 软件名称

    // MCS   新增 在线MCS的IP信息  [04/24/2012 liaokang] 
    TRegedMcsIpInfo     m_tRegedMcsIpInfo[MAXNUM_MCU_MC];       // 在线MCS的IP信息

    // 其他标识变量
    u16                 m_wPowerOffEvent;                       // 待机事件号
    u32                 m_dwPowerOffDstId;                      // 待机事件接受对象
    u16                 m_wRunningEvent;                        // 运行事件号
    u32                 m_dwRunningDstId;                       // 运行事件接受对象
    u16                 m_wRebootEvent;                         // 重启事件号
    u32                 m_dwRebootDstId;                        // 重启事件接受对象
	s8                  m_achCfgName[256];					    // 配置文件名字
    SEMHANDLE           m_hMcuAllConfig;                        // 配置信息写入信号量保护
	s8					m_achCfgBakName[256];					// 配置文件备份名字
	s8					m_achCompileTime[MAX_ALARM_TIME_LEN + 1];	//mcu编译时间
	u8					m_byTempPortKind;
	TMcuPfmInfo			m_tMcuPfmInfo;							// Mcu Performance Info
	TMcuUpdateInfo		m_tMcuUpdateInfo;						// Mcu Update Info
	T8KECpuInfo         m_t8KECpuInfo;                          // mqs [2010-11-30]
	T8KEMemInfo         m_t8KEMemInfo;                          // mqs [2010-11-30]
	//[2011/02/11 zhushz] mcs修改mcu ip
	TMcuNewNetCfg		m_tMcuNewNetCfg;						//mcs设置新的网络配置
	BOOL32				m_bIsNetCfgBeModifedByMcs;				//mcs设置新的网络配置标志
	u8					m_byIsMcu800L;							//是否是800L MCU
	u8					m_byIsMcu8000H_M;						//是否是8000H-M MCU
	u8					m_byEncoding;							//编码方式：0表示GBK,1表示UTF8，其他未使用
#ifdef _8KI_
	TNewNetCfg			m_tNewNetCfg;
#endif
};

class CCfgSemOpt{
public:
	CCfgSemOpt( SEMHANDLE& hSem )
	{
		m_hSem = hSem;
		if( OspSemTakeByTime( m_hSem, WAIT_SEM_TIMEOUT ) != TRUE )
		{
			OspPrintf( TRUE, FALSE, "[Agent] semTake error accord to handle<0x%x>\n", hSem );
		}
	}
	~CCfgSemOpt( )
	{
		OspSemGive( m_hSem );
	}
private:
	SEMHANDLE m_hSem;
};

void Agtlog( u8 byPrintLvl, s8 * pszFmt, ... );
void SendMsgToMcuCfg( u16 wEvent, const u8 * pcMsg, u16 wLen );
const s8* GetBoardIDStr();
const s8* GetMcuSoftVersion();

extern CCfgAgent		g_cCfgParse;
extern CBrdManagerApp	g_cBrdManagerApp;
extern CAlarmProc		g_cAlarmProc;

extern TMcuAgentDebugInfo g_tAgentDebugInfo;

#endif  // _AGENT_CONFIGURE_READ_
