/*****************************************************************************
   模块名      : MP
   文件名      : mpmanage.h
   相关文件    : 
   文件实现功能: MP与MCU接口定义
   作者        : 胡昌威
   版本        : V0.1  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/07/10  0.1         胡昌威        创建
******************************************************************************/
#ifndef __MPMANAGER_H_
#define __MPMANAGER_H_

#define DEFAULT_MAPPORT 29990 //需要作假的目的ip对应的终端取不到后向通道ip或端口时用的固定端口

// 交换组信息
struct TSwitchGrp
{
private:
	u8  m_bySrcChnl;
	u8  m_byDstMtNum;
	TMt *m_pDstMt;
	
public:
	TSwitchGrp() 
	{
		Init();
	}
	
	void Init()
	{
		m_bySrcChnl = 255;
		m_byDstMtNum = 0;
		m_pDstMt = NULL;
	}
	
	BOOL32 IsNull()
	{
		return m_bySrcChnl == 255;
	}
	
	void SetSrcChnl( u8 bySrcChnl )
	{
		m_bySrcChnl = bySrcChnl;
	}
	u8 GetSrcChnl()
	{
		return m_bySrcChnl;
	}
	
	void SetDstMtNum( u8 byDstMtNum )
	{
		m_byDstMtNum = byDstMtNum;
	}
	u8 GetDstMtNum()
	{
		return m_byDstMtNum;
	}
	
	void SetDstMt( TMt *pDstMt )
	{
		m_pDstMt = pDstMt;
	}
	TMt *GetDstMt()
	{
		return m_pDstMt;
	}
};

//[8/5/2011 liuxu]
// 功能简介 : 由于TSwitchGrp的m_pDstMt是一个指向外部的指针,如果对m_pDstMt做了修改,将会影响外部使用它的地方
// CSwitchGrpAdpt是为解决此问题而设计的. CSwitchGrpAdpt通过将TSwitchGrp的m_pDstMt拷入自己的缓存中,然后替换
// 它们, 从而使操作TSwitchGrp的m_pDstMt的代码不影响它
class CSwitchGrpAdpt
{
public:
	CSwitchGrpAdpt( ) : m_nGrpNum(0), m_ppMtList(NULL){ }
	~CSwitchGrpAdpt( ) { Reset(); }

public:
	void   Reset();
	
	// 用m_ppMtList代替ptSwitchGrp的所有mtlist
	BOOL32 Convert( const s32 nGrpNum, TSwitchGrp* ptSwitchGrp );

private:
	s32			m_nGrpNum;							// TSwitchGrp的数目
	TMt			**m_ppMtList;						// 临时存储TSwitchGrp中mtlist
};



struct TSwInfo
{
	u8  m_byMpId;
	u32 m_dwRcvIp;
	u16 m_wRcvPort;
	u32 m_dwDstIp;
	u16 m_wDstPort;
};

struct TAllMpSwInfo
{
	TSwInfo m_tSwInfo[MAXNUM_DRI];
	u8		m_UsedMPNum;

	TAllMpSwInfo()
	{
		memset(this, 0, sizeof(TAllMpSwInfo));
	}
};

class CMpManager
{
public:
	CMpManager();
	virtual ~CMpManager();

public:	
    //-------------------------Multicast-------------------------------
	//开始组播
	BOOL32 StartMulticast( const TMt & tSrc, u16 wSrcChnnl = 0/*u8 bySrcChnnl = 0*/, u8 byMode = MODE_BOTH, BOOL32 bConf = TRUE );
	//停止组播
	BOOL32 StopMulticast( const TMt & tSrc, u8 bySrcChnnl = 0, u8 byMode = MODE_BOTH, BOOL32 bConf = TRUE );
    
    //-------------------------Distributed Conf Multicast-------------------------------
    //开始分散会议组播
    BOOL32 StartDistrConfCast( const TMt &tSrc, u8 byMode = MODE_BOTH, u8 bySrcChnnl = 0) ;
    //停止分散会议组播
    BOOL32 StopDistrConfCast (const TMt &tSrc, u8 byMode = MODE_BOTH, u8 bySrcChnnl = 0) ;

    //------------------------Switch Bridge----------------------------
	//建立交换桥
	BOOL32 SetSwitchBridge( const TMt & tSrc, u8 bySrcChnnl = 0, u8 byMode = MODE_BOTH, BOOL32 bEqpHDBas = FALSE,u16 wSpyStartPort = SPY_CHANNL_NULL );
	//移除交换桥
	BOOL32 RemoveSwitchBridge( const TMt & tSrc, u8 bySrcChnnl = 0, u8 byMode = MODE_BOTH, BOOL32 bEqpHDBas = FALSE,u16 wSpyStartPort = SPY_CHANNL_NULL );

	//------------------------------Mt----------------------------------
	//将指定终端数据交换到下级终端
// 	BOOL32 StartSwitchToSubMt( const TMt & tSrc,
// 							   u8 bySrcChnnl,
// 							   const TMt & tDst,
// 							   u8 byMode = MODE_BOTH,
// 							   u8 bySwitchMode = SWITCH_MODE_BROADCAST,
// 							   BOOL32 bH239Chnnl = FALSE,
//                                BOOL32 bStopBeforeStart = TRUE,
// 							   BOOL32 bSrcHDBas = FALSE,
// 							   u16 wSpyPort = SPY_CHANNL_NULL);

	//zjj20100201
	//  [11/10/2009 pengjie] 级联多回传支持
	BOOL32 StartSwitchToMmcu( const TMt & tSrc,
		u16 wSrcChnnl,
		const TMt & tDst,
		u8 byMode = MODE_BOTH,
		u8 bySwitchMode = SWITCH_MODE_BROADCAST,
		BOOL32 bH239Chnnl = FALSE,
		BOOL32 bStopBeforeStart = TRUE,
		BOOL32 bSrcHDBas = FALSE,
		u16 wSpyPort = SPY_CHANNL_NULL);

	//停止将数据交换到直连下级级终端
	//void StopSwitchToSubMt( const TMt & tDst, u8 byMode = MODE_BOTH, BOOL32 bH239Chnnl = FALSE, u16 wSpyStartPort = SPY_CHANNL_NULL );
	void StopSwitchToSubMt( u8 byConfIdx, 
							u8 byMtNum,
							const TMt *ptDst, 
							u8 byMode, 
							u16 wSpyPort = SPY_CHANNL_NULL );

	//停止接收终端数据
    // 顾振华 [4/30/2006] 本函数不再使用。改用ds 的RecvOnly交换
	//void StopRecvSubMt( const TMt & tMt, u8 byMode );

    // guzh [3/21/2007] 建立广播交换
    BOOL32 StartSwitchToBrd(const TMt &tSrc, u8 bySrcChnnl, BOOL32 bForce, u16 wSpyStartPort = SPY_CHANNL_NULL,u8 byMpId = 0);
    BOOL32 StartSwitchFromBrd(const TMt &tSrc, u8 bySrcChnnl, u8 byDstMtNum, const TMt* const ptDst, u16 wSpyStartPort = SPY_CHANNL_NULL);
    BOOL32 StartSwitchEqpFromBrd(const TMt &tSrc, u16 bySrcChnnl, u8 byEqpId, u16 wDstChnnl);
    // guzh [3/23/2007] 删除广播源
    BOOL32 StopSwitchToBrd(const TMt &tSrc, u8 bySrcChnnl, u16 wSpyStartPort = SPY_CHANNL_NULL);
    
	
	BOOL32 StartSwitchToDst(TSwitchDstInfo &tSwitchDstInfo, u16 wSpyStartPort = SPY_CHANNL_NULL);
	void   StopSwitchToDst(TSwitchDstInfo &tSwitchDstInfo);	

	void AddSendSelfMutePack(const u8& byMpId,const TSendMutePack& tSendMutePack);
	void DelSendSelfMutePack(const u8 & byMpId,const TMt & tMt);
	//------------------------------Mc---------------------------------
	//将指定终端数据交换到会控
	BOOL32 StartSwitchToMc( const TMt & tSrc, u8 bySrcChnnl, u16 wMcInstId, u8 byDstChnnl = 0, u8 byMode = MODE_BOTH, u16 wSpyPort = SPY_CHANNL_NULL );
	//停止将数据交换到会控
	void StopSwitchToMc( u8 byConfIdx, u16 wMcInstId,  u8 byDstChnnl = 0, u8 byMode = MODE_BOTH );

	//----------------------------PeriEqp----------------------------------	 
	//分配录像机的数据交换IP地址与端口
	BOOL32 GetRecorderSwitchAddr(u8 byRecorderId, u32 &dwSwitchIpAddr,u16 &wSwitchPort); 
	//分配混音器的数据交换IP地址与端口
	BOOL32 GetMixerSwitchAddr(u8 byMixerId, u32 &dwSwitchIpAddr,u16 &wSwitchPort); 
	//分配码率适配器的数据交换IP地址与端口
	BOOL32 GetBasSwitchAddr(u8 byBasId, u32 &dwSwitchIpAddr,u16 &wSwitchPort);
	//分配画面合成器的数据交换IP地址与端口
	BOOL32 GetVmpSwitchAddr(u8 byVmpId, u32 &dwSwitchIpAddr,u16 &wSwitchPort);
    //分配复合电视墙的数据交换IP地址与端口
    BOOL32 GetVmpTwSwitchAddr(u8 byVmpTwId, u32 &dwSwitchIpAddr, u16 &wSwitchPort);
	//分配丢包重传器的数据交换IP地址与端口
	BOOL32 GetPrsSwitchAddr(u8 byPrsId, u32 &dwSwitchIpAddr,u16 &wSwitchPort );

	//将指定终端数据交换到外设
	BOOL32 StartSwitchToPeriEqp( const TMt & tSrc,
								 u16 wSrcChnnl,
								 u8 byEqpId,
								 u16 wDstChnnl = 0, 
								 u8 byMode = MODE_BOTH,
								 u8 bySwitchMode = SWITCH_MODE_BROADCAST,
								 BOOL32 bEqpHDBas = FALSE,
								 BOOL32 bStopBeforeStart = TRUE,
								 u8 byHduSubChnId = 0);
	//停止将数据交换到外设
	void StopSwitchToPeriEqp( u8 byConfIdx, u8 byEqpId, u16 wDstChnnl = 0,  u8 byMode = MODE_BOTH, u8 byHduSubChnId = 0 ); 	


	//屏幕打印所有Mp信息
	void ShowMp();
	//屏幕打印所有交换桥
	void ShowBridge();
	//屏幕打印所有交换信息
	void ShowSwitch( u8 byType );
	//get switchinfo according to DstAddr
	BOOL32 ShowSwitchRouteToDst(u8 byConfIdx, u32 dwDstIp, u16 wDstPort, BOOL32 bDetail = TRUE);
	//屏幕打印所有MtAdp信息
	void ShowMtAdp();
	// 获取指定mp板上的交换信息
	void GetMPSwitchInfo( u8 byMpId, TSwitchTable &tSwitchTable );


	//处理来自Mp的消息
	void ProcMpToMcuMessage(const CMessage * pcMsg);

	//处理来自Mtadp的消息
	void ProcMtAdpToMcuMessage(const CMessage * pcMsg);

	//-----------------------交换相关函数----------------------------
	//开始交换
	BOOL32 StartSwitch(const TMt & tSrcMt, u8 byConfIdx,
                     u32 dwSrcIp, u32 dwDisIp,
                     u32 dwRcvIp, u16 wRcvPort,
                     u32 dwDstIp, u16 wDstPort,
                     u32 dwRcvBindIP = 0,u32 dwSndBindIP = 0,
                     u8 bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_NONE,
                     u8 byUniformPayload = INVALID_PAYLOAD,
                     u8 byIsSwitchRtcp = 1,
                     BOOL32 bStopBeforeStart = TRUE,u8 byDstMtId = 0);
	//停止交换
	BOOL32 StopSwitch( u8 byConfIdx, u32 dwDstIp, u16 wDstPort, u8 byIsSwitchRtcp = 1);
    BOOL32 StartStopSwitch(u8 byConfIdx, u32 dwDstIp, u16 wDstPort, u8 byMpId = 0);

	//开始交换
	BOOL32 StartSwitch(const TMt & tSrcMt, u8 byConfIdx,
                     u32 dwSrcIp, u32 dwDisIp,
                     u32 dwRcvIp, u16 wRcvPort,
                     const TLogicalChannel& tFwdChnnl,
                     u16 wDstChnnl = 0,
                     u8 bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_NONE,
                     u8 byUniformPayload = INVALID_PAYLOAD,
                     u8 byIsSwitchRtcp = 1,
                     BOOL32 bStopBeforeStart = TRUE,u8 byDstMtId = 0);
	//停止交换
	BOOL32 StopSwitch( u8 byConfIdx, const TLogicalChannel& tFwdChnnl, u16 wDstChnnl = 0, 
		               u8 byIsSwitchRtcp = 1, u8 byMulPortSpace = 1);

	BOOL32 StartSwitchToAll( const TMt &tSrc,
							const u8 bySwitchGrpNum,
							TSwitchGrp *pSwitchGrp,
							u8 byMode,
							u8 bySwitchMode = SWITCH_MODE_BROADCAST,
							BOOL32 bStopBeforeStart = TRUE,
							u16 wSpyStartPort = SPY_CHANNL_NULL );

	//--------------------多点到一点的交换---------------------------
	//增加交换
	BOOL32 AddMultiToOneSwitch( u8 byConfIdx, u32 dwSrcIp, u32 dwDisIp, u32 dwRcvIp, u16 wRcvPort, 
		                      u32 dwDstIp, u16 wDstPort,u32 dwRcvBindIP = 0, u32 dwSndBindIP = 0, 
							  u32 dwMapIpAddr = 0, u16 wMapPort = 0 );
	//移除交换
	BOOL32 RemoveMultiToOneSwitch( u8 byConfIdx, u32 dwRcvIp, u16 wRcvPort, u32 dwDstIp, u16 wDstPort );

	//zjl20100414 rtcp打包建交换
	BOOL32 AddMultiToOneSwitch(u8 byConfIdx, u8 byDstNum, TSwitchChannelExt *ptSwitchChannelExt);
	//zjl20100414 rtcp打包拆交换
	BOOL32 RemoveMultiToOneSwitch(u8 byConfIdx, u8 byDstNum, TSwitchChannelExt *ptSwitchChannelExt);

    //停止交换
	BOOL32 StopMultiToOneSwitch( u8 byConfIdx, u32 dwDstIp, u16 wDstPort );

	//--------------------只收不发的交换-----------------------------
	//向mp发出ssrc增加交换请求
	BOOL32 AddRecvOnlySwitch( u8 byConfIdx, u32 dwRcvIp, u16 wRcvPort ,u32 dwRtcpBackIp = 0,u16 wRtcpBackPort = 0, BOOL32 bReverseG7221c = FALSE);
	//向mp发出ssrc移除交换请求
    BOOL32 RemoveRecvOnlySwitch( u8 byConfIdx, u32 dwRcvIp, u16 wRcvPort );
	//--------------------起停接收终端数据---------------------------
	//开始接收终端数据
	BOOL32 StartRecvMt( const TMt &tMt, u8 byMode );
	//停止接收终端数据
	BOOL32 StopRecvMt( const TMt &tMt, u8 byMode );
	
	//------------------设置对码流中SSRC作改动-------------------------
    //zbq[07/18/2007]下边涉及的SSRC逻辑已由归一重整统一处理，注释之
	//设置对码流中SSRC作改动
	//BOOL32 ResetRecvMtSSRC( const TMt &tMt, u8 byMode );
	//取消对码流中SSRC作改动
	//BOOL32 RestoreRecvMtSSRC( const TMt &tMt, u8 byMode );
	//向mp发出ssrc变更请求
	//BOOL32 SetRecvMtSSRCValue( u8 byConfIdx, u32 dwRcvIp, u16 wRcvPort, BOOL32 bReset );
	
    //得到交换信息
	BOOL32 GetSwitchInfo( const TMt & tSrc, u32 &dwSwitchIpAddr, u16 &wSwitchPort, u32 &dwSrcIpAddr );
	
	//分配外设接收起始端口和MCU接收起始端口
    BOOL32 GetSwitchInfo( u8 byEqpId, u32 &dwSwitchIpAddr, u16 &wMcuStartPort, u16 &wEqpStartPort );

    void UpdateRRQInfoAfterURQ( u8 byURQConfIdx );


protected:
    
    BOOL32 StartBridgeSwitch(u8 byMode, TMt tSrcMt, u32 dwSrcIp, u32 dwRcvIp, u16 wRcvPort, u32 dwDstIp, BOOL32 bEqpHDBas = FALSE);

	//-----------------------消息处理函数------------------------
	//处理Mp注册消息
	void ProcMpRegReq(const CMessage * pcMsg);	
	//处理Mp断链消息
	void ProcMpDisconnect(const CMessage * pcMsg);
	//处理Mtadp注册消息
	void ProcMtAdpRegReq(const CMessage * pcMsg);	
	//处理Mtadp断链消息
	void ProcMtAdpDisconnect(const CMessage * pcMsg);

};

API void showmp(void);
API void showmtadp(void);
API void showbridge(void);
API void ssw( u8 byType = 0 );
API void sswi(const s8* pchDstIp = NULL, u16 wDstPort = 0, u8 byConfIdx = 0, BOOL32 bDetail = TRUE);

#endif
