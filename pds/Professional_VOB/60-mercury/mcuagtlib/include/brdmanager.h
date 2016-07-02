/*****************************************************************************
   模块名      : Board Manager
   文件名      : brdmanager.h
   相关文件    : 
   文件实现功能: 单板管理基类定义
   作者        : 李屹
   版本        : V4.0  Copyright( C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人        修改内容
   2002/01/25  0.9         李屹          创建
   2002/07/29  1.0         李屹          新方案调整
   2003/11/20  3.0         jianghy       创建
   2004/11/10  3.6         libo          新接口修改
   2004/11/29  3.6         libo          移植到Linux
   2005/08/17  4.0         liuhuiyun     更新
******************************************************************************/
#ifndef BRDMANAGER_H
#define BRDMANAGER_H

#if _MSC_VER > 1000
#pragma once
#endif

#include "mcuagtstruct.h"
#include "agtcomm.h"
#include "mcuconst.h"
#include "evagtsvc.h"
#include "agentcommon.h"
#include "agentmsgcenter.h"

#ifdef WIN32
#pragma pack( push )
#pragma pack(1)
#endif


#ifdef _VXWORKS_
    #define IS_DSL8000_BRD_MPC
    #include "brdDrvLib.h"
    #include "time.h"
    #include "timers.h"
#endif

// 定义单板管理程序监听端口号  (测试用,实际用MCU的交换端口号)
#define BOARD_MANAGER_LISTEN_PORT	8000

#ifdef WIN32
#include "winbrdwrapper.h"
#endif
// BSP中定义,WINDOWS下模拟时使用
//#ifdef WIN32
//struct TBrdPosition
//{
//public:
//    u8 byBrdID;         // 板子ID号 
//    u8 byBrdLayer;      // 板子所在层号
//    u8 byBrdSlot;       // 板子所在槽位号
//
//public:
//    TBrdPosition(){ Clear();}
//    void Clear(void) { memset( this, 0, sizeof(TBrdPosition) );  }
//    void SetBrdId(u8 byID ) { byBrdID = byID;  }
//    u8   GetBrdId(void) const { return byBrdID; }
//    void SetBrdLayer(u8 byLayer ) { byBrdLayer = byLayer; }
//    u8   GetBrdLayer(void) const { return byBrdLayer; }
//    void SetBrdSlot(u8 bySlot) { byBrdSlot = bySlot; }
//    u8   GetBrdSlot(void) const { return byBrdSlot; }
//    BOOL32 IsSameBrd( TBrdPosition &tBrdPosition)
//    {
//        if ( tBrdPosition.GetBrdId() == byBrdID &&
//             tBrdPosition.GetBrdLayer() == byBrdLayer &&
//             tBrdPosition.GetBrdSlot() == byBrdSlot ) 
//        {
//            return TRUE;
//        }
//        return FALSE;
//    }
//    
//};
//#endif

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

// 需要发送给单板的外设信息缓存结构
struct UTEqpConfig
{
public:
    UTEqpConfig()
    {
        memset(this, 0, sizeof(UTEqpConfig));
    }
    u8 byEqpType;
	u8 bySubEqpType;

    // 这里无法使用Union
	TEqpTVWallEntry tTVWallCfg;
	TEqpMixerEntry  tMixerCfg;
	TEqpBasEntry    tBasCfg;
	TEqpVMPEntry tVMPCfg;
    TEqpMPWEntry tMpwCfg;
	TEqpPrsEntry tPrsCfg; 
	//4.6版本 新加   jlb
	TEqpHduEntry tHduCfg;
	TEqpSvmpEntry tSvmpCfg;
	TEqpDvmpBasicEntry tDvmpCfg;
	TEqpMpuBasEntry tMpuBasCfg;
	TEqpEbapEntry tEbapCfg;
	TEqpEvpuEntry tEvpuCfg;
		
};

class CEqpConfigBuf
{
private:
    u8 m_abyBuf[4096];

    u8 m_byUseNetSync;
    u8 m_byNetSyncE1Chnl;

    u8 m_byUseWatchDog;

    u32 m_dwIp;
    
    u8 m_byCfgNum;

    // guzh [1/12/2007] 8000B 支持配置Mp MtAdp Gk 等
    TDSCModuleInfo m_tDscModule;
	
	// GK 注册地址, 给dsc的proxy修改配置文件用, zgc, 2007-08-15
	u32 m_dwGKIp;

	// TLoginInfo, zgc, 2007-10-12
	TLoginInfo m_tLoginInfo;
	
public:
    UTEqpConfig m_utCfg[MAXNUM_BRD_EQP]; 
public:
    CEqpConfigBuf()
    {
        // guzh [1/12/2007] 没有虚函数所以才可以memset
        memset(this, 0, sizeof(CEqpConfigBuf));
    }
    ~CEqpConfigBuf()
    {
    }

	u8 GetEqpCfgNum(void)
	{
		return m_byCfgNum;
	}
	void SetEqpCfgNum(u8 val)
	{
        m_byCfgNum = val;
    }

    void SetUseNetSync(BOOL32 bUse, u8 byE1Chnl = 0xFF)
    {
        m_byUseNetSync = bUse ? 1 : 0;
        if (bUse)
        {
            m_byNetSyncE1Chnl = byE1Chnl;
        }
    }

    void SetUseWatchDog(BOOL32 bUse)
    {
        m_byUseWatchDog = bUse ? 1 : 0;
    }

    void SetIpAddr(u32 dwIp)
    {
        m_dwIp = dwIp;
    }

	void SetGKIp(u32 dwIp)
	{
		m_dwGKIp = htonl(dwIp);
	}

    void AddEqpCfg(TEqpTVWallEntry entry)
    {
        m_utCfg[m_byCfgNum].byEqpType = EQP_TYPE_TVWALL;
        m_utCfg[m_byCfgNum].tTVWallCfg = entry;
        m_byCfgNum ++;
    }
    void AddEqpCfg(TEqpMixerEntry entry)
    {
        m_utCfg[m_byCfgNum].byEqpType = entry.GetType();//tianzhiyong  增加EAPU类型混音器
        m_utCfg[m_byCfgNum].tMixerCfg = entry;
        m_byCfgNum ++;
    }
    void AddEqpCfg(TEqpBasEntry entry)
    {
        m_utCfg[m_byCfgNum].byEqpType = EQP_TYPE_BAS;
        m_utCfg[m_byCfgNum].tBasCfg = entry;
        m_byCfgNum ++;
    }
    void AddEqpCfg(TEqpVMPEntry entry)
    {
        m_utCfg[m_byCfgNum].byEqpType = EQP_TYPE_VMP;        
        m_utCfg[m_byCfgNum].tVMPCfg = entry;
        m_byCfgNum ++;
    }
    void AddEqpCfg(TEqpMPWEntry entry)
    {
        m_utCfg[m_byCfgNum].byEqpType = EQP_TYPE_VMPTW;
        m_utCfg[m_byCfgNum].tMpwCfg = entry;
        m_byCfgNum ++;
    }
    void AddEqpCfg(TEqpPrsEntry entry)
    {
        m_utCfg[m_byCfgNum].byEqpType = EQP_TYPE_PRS;
        m_utCfg[m_byCfgNum].tPrsCfg = entry;
        m_byCfgNum ++;
    }

	//目前只有APU2 bas使用[3/22/2013 chendaiwei]
	void AddEqpCfg(TEqpMpuBasEntry entry)
	{
        m_utCfg[m_byCfgNum].byEqpType = EQP_TYPE_BAS;
        m_utCfg[m_byCfgNum].tMpuBasCfg = entry;
		m_utCfg[m_byCfgNum].bySubEqpType = entry.GetStartMode();
        m_byCfgNum ++;
	}

    //4.6版本 新加外设 jlb
	void AddEqpCfg(TEqpHduEntry entry)
    {
// 		u8 byEqpType = 0;
// 		if (STARTMODE_HDU_H == entry.GetStartMode())
// 		{
// 			byEqpType = EQP_TYPE_HDU_H;
// 		}
// 		else if (STARTMODE_HDU_M == entry.GetStartMode())
// 		{
// 			byEqpType = EQP_TYPE_HDU;
// 		}
// 		else if(STARTMODE_HDU_L == entry.GetStartMode())
// 		{
// 			byEqpType = EQP_TYPE_HDU_L;
// 		}
// 		else if(STARTMODE_HDU2 == entry.GetStartMode())
// 		{
// 			byEqpType = EQP_TYPE_HDU2;
// 		}		
// 		else if(STARTMODE_HDU2_L == entry.GetStartMode())
// 		{
// 			byEqpType = EQP_TYPE_HDU2_L;
// 		}
// 		else
// 		{
// 			OspPrintf(TRUE, FALSE, "[AddEqpCfg][TEqpHduEntry] unexpected hdu type:%d!\n", entry.GetStartMode());
// 			return;
// 		}
		
        m_utCfg[m_byCfgNum].byEqpType = entry.GetType();
        m_utCfg[m_byCfgNum].tHduCfg = entry;
        m_byCfgNum ++;
    }

    void SetDscModuleInfo(const TDSCModuleInfo& tInfo)
    {
        m_tDscModule = tInfo;
    }

	void SetLoginInfo( const TLoginInfo & tLoginInfo )
	{
		memcpy( &m_tLoginInfo, &tLoginInfo, sizeof(m_tLoginInfo) );
	}
    
    u8* GetBuffer(u16 &wBufLen)
    {
        wBufLen = Generate();
        return m_abyBuf;
    }
    
private:
    // 根据所有信息打包到 m_abyBuf，返回包长度
    u16 Generate() ;

};

typedef struct tagRegboard{

    BOOL32   bReg;          // 是否注册
    u32      dwNode;        // 单板的节点号
    u32      dwDstInsId;    // 实例Id
    TBrdPosition tBrdPos;   // 单板位置

    //单板文件状态, 目前只用于LINUX下网管对升级后文件情况的查询，特此约定：
    //m_abyBrdFileStatus[0]->.image, m_abyBrdFileStatus[1]->.linux, 其他暂时未使用，待扩展.
    u8       m_abyBrdFileStatus[MAXNUM_FILE_UPDATE];

}TRegBoard;

class CRegedBoard{
public:
    CRegedBoard();
    virtual ~CRegedBoard();

    BOOL32 IsRegedBoard(TBrdPosition tBrdPos, u32 dwNode = INVALID_NODE);
    u16  AddBoard(TBrdPosition tBrdPos, u32 wInsId, u32 dwNode);
    u16  DelBoard(TBrdPosition tBrdPos);
    void SetBinFileStatus(TBrdPosition tBrdPos, u8 byStatus);
    u8   GetBinFileStatus(TBrdPosition tBrdPos);
    void SetImageFileStatus(TBrdPosition tBrdPos, u8 byStatus);
    u8   GetImageFileStatus(TBrdPosition tBrdPos);
    void SetOSFileStatus(TBrdPosition tBrdPos, u8 byStatus);
    u8   GetOSFileStatus(TBrdPosition tBrdPos);
    void ShowRegedBoard(void);
    void ShowSemHandle(void);

private:
    u16				m_wRegedBrdNum;   // 已经注册单板数目
    TRegBoard		m_tBrdReged[MAX_BOARD_NUM];
	SEMHANDLE		m_hBoardManager;  // 单板管理

    //MPC板文件状态，目前只用于LINUX下网管对升级后文件情况的查询，特此约定：
    //m_abyMpcFileStatus[0]->.image, m_abyMpcFileStatus[1]->.linux, 其他暂时未使用，待扩展.
    u8              m_abyMpcFileStatus[MAXNUM_FILE_UPDATE];
};

// 单板管理类定义
class CBoardManager:public CInstance
{
	enum 
	{ 
		STATE_IDLE,			// 空闲
        STATE_WAITREG,      // 等待注册
		STATE_INIT,			// 初始化
		STATE_NORMAL,		// 工作
	};

public:
	CBoardManager();
	virtual ~CBoardManager();

protected:
	// 实例消息处理入口函数，必须override
	void InstanceEntry(CMessage* const pMsg);
	void ProcBoardRegMsg(CMessage* const pcMsg);            // 单板注册消息	
	void ProcBoardGetConfigMsg(CMessage* const pcMsg);      // 单板取配置消息
	void ProcBoardAlarmSyncAck(CMessage* const pcMsg);      // 告警同步应答消息
	void ProcBoardGetVersionAck(CMessage* const pcMsg);     // 获取版本信息应答
	void ProcBoardGetModuleAck(CMessage* const pcMsg);      // 获取模块信息应答
	void ProcMpcCommand(CMessage* const pcMsg);             // 上层应用到单板的消息
	void ProcBoardAlarmNotify(CMessage* const pcMsg);       // 单板的告警通知消息
	void ProcBoardLedNotify(CMessage* const pcMsg);         // 单板的灯状态通知
    void ProcBoardEthPortNotify(CMessage* const pcMsg );    // 单板的网口状态通知  [2012/05/04 liaokang]
	//void ProcBoardGetStatisticsAck(CMessage* const pcMsg);  // 单板的统计信息应答消息
	void ProcBoardDisconnect( CMessage* const pcMsg );      // 单板断开消息处理
    void ProcBoardUpdateRsp( CMessage* const pcMsg );       // 处理单板升级响应
	void ProcBrdConnectTestOverTimeOut( CMessage* const pcMsg );
	void ProcBoardE1BandWidthNotif( CMessage* const pcMsg );//处理单板E1带宽指示 

	/*void ProcBoardSoftwareVerNotify(CMessage* const pcMsg); // 单板软件版本号通知 [12/13/2011 chendaiwei]*/

#ifdef _MINIMCU_
	void ProcBoardSetDscInfoRsp( CMessage* const pcMsg );	// 设置DSC info请求的回应处理函数, zgc, 2007-07-17
	void ProcDscStartUpdateSoftwareRsp( CMessage* const pcMsg ); //开始升级请求的回应, zgc, 2007-08-25
	void ProcDscMpcUpdateFileRsp( CMessage* const pcMsg );		 //接收数据包的回应, zgc, 2007-08-25
#endif

	void ProcBoardConfigModifyNotif( CMessage* const pcMsg );	//单板配置修改, zgc, 2007-10-16

	void ProcMpcBrdDisconnectCmd( void );		// MPC命令和单板断链, zgc, 2007-08-28
	
protected:
	BOOL32 PostMsgToBoard( u16 wEvent, u8 * const pbyMsg, u16 wLen );	
	u8   GetWatchDogOption( void );                         // 获取WATCH DOG的配置

	// 用CEqpConfigBuf结构给外设信息打包, 返回FALSE则CEqpConfigBuf中的内容无效, zgc, 2007-10-17
	BOOL32 PackEqpConfig(CEqpConfigBuf &cEqpCfgBuf, TBoardInfo tBrdInfo);

    void GetMpuEBapEVpuEqpInfo( TBoardInfo &tBoardInfo, TMpuEBapEVpuConfigAck &tMpuEBapEVpuConfigAck );

#if defined(_8KI_) && defined(_LINUX_) 	
	void ProcDetectEthTimer(CMessage* const pcMsg);		//检测网口状态
#endif

protected:
	// 以下测试用
	void DaemonInstanceEntry(CMessage* const pMsg, CApp* pcApp);
    void DaemonProcBrdRegMsg(CMessage* const pMsg, CApp* pcApp);
    void DaemonProcDaemonPowerOn(CApp* pApp);
	void DaemonDumpInst(CApp* pApp);
    //void DaemonProcBrdDisconectMsg(CMessage* const pMsg);   // DAEMON 处理单板断链    
	void ProcDaemonGetConfig( void );        // DAEMON的取配置函数(使用同步消息)
	void ProcDaemonManagerCmd( void );      // 管理命令转发

	void ClearInst(void);
protected:
	TBrdPosition	m_tBrdPosition;      //单板位置，包括层、槽、类型
	u32			    m_dwBrdNode;		 //对应单板的节点号
	u32			    m_dwBrdIId;			 //单板的IID
    u8              m_byOsType;          //os类型
};

typedef zTemplate< CBoardManager, MAX_BOARD_NUM, CRegedBoard, 32 > CBrdManagerApp;
extern CBrdManagerApp	g_cBrdManagerApp;	//单板管理应用实例

s32 AgtQueryBrdPosition(TBrdPosition *ptBrdPosition);
s32 AgtGetBrdEthParamAll(u8 byEthId, TBrdEthParamAll *ptBrdEthParamAll);
s32 AgtGetBrdEthParam(u8 byEthId, TBrdEthParam *ptBrdEthParam);
u32 AgtGetMpcGateway( u32 dwIfIndex );




#ifdef WIN32
#pragma pack( pop )
#endif

#endif   /*End BRDMANAGER_H*/

