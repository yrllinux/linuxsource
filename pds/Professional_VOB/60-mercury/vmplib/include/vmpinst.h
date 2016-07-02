/*****************************************************************************
   模块名      : 画面复合器( Video Multiplexer)
   文件名      : vmpInst.h
   创建时间    : 2003年 12月 4日
   实现功能    : VMPInst.cpp
   作者        : zhangsh
   版本        : 
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   03/12/4	v1.0	       zhangsh	    create
   2006/03/21  4.0         张宝卿      代码优化
******************************************************************************/
#ifndef _VIDEO_MULTIPLEXERINST_H_
#define _VIDEO_MULTIPLEXERINST_H_

#include "kdvtype.h"
#include "osp.h"
#include "evmcueqp.h"
#include "mcustruct.h"
#include "mcuconst.h"
#include "vmpcfg.h"
// #include "drawinterface.h"
#ifdef _LINUX_
#include "libsswrap.h"
#endif

OSPEVENT( EV_VMP_TIMER,                EV_VMP_BGN );                //内部定时器
OSPEVENT( EV_VMP_NEEDIFRAME_TIMER,     EV_VMP_BGN + 1 );            //关键帧定时器
OSPEVENT( EV_VMP_CONNECT_TIMER,        EV_VMP_BGN + 2 );            //连接定时器
OSPEVENT( EV_VMP_CONNECT_TIMERB,       EV_VMP_BGN + 3 );            //连接定时器
OSPEVENT( EV_VMP_REGISTER_TIMER,       EV_VMP_BGN + 4 );            //注册定时器
OSPEVENT( EV_VMP_REGISTER_TIMERB,      EV_VMP_BGN + 5 );            //注册定时器
OSPEVENT( TIME_GET_MSSTATUS,           EV_VMP_BGN + 6 );            //取主备倒换状态

#define VMP_CONNETC_TIMEOUT         (u16)3*1000  //connect time
#define VMP_REGISTER_TIMEOUT        (u16)6*1000  //register time
#define CHECK_IFRAME_INTERVAL       (u16)1000    //关键帧请求间隔时长
#define FIRST_REGACK                (u8)1        //第一次收到注册Ack

class CVMPInst : public CInstance
{
	enum //实例状态
	{
		IDLE   = 0,
		NORMAL = 1,
		RUNNING = 2
	};
public:
	CVMPInst();
	~CVMPInst();
private:
    u8            m_byMaxChannelNum;	
	
    CKDVVMPParam  m_tParam[2];			// 合成参数
	TNetAddress   m_tSndAddr[2];		// 合成后输出地址
	BOOL32        m_bDbVid;				// 是否双视频

    CConfId       m_cConfId;		// 当前会议号
	TVMPCfg       m_tCfg;           // 配置参数
    TVmpStyleCfgInfo  m_tStyleInfo; // 合成风格参数

	// CDrawInterface m_cDrawInterface; // 画图类
private:
	//-----------自身状态--------------
	CHardMulPic   m_cHardMulPic;   // 画面复合器的封装
    BOOL32        m_bAddVmpChannel[MAXNUM_MPUSVMP_MEMBER/*MAXNUM_SDVMP_MEMBER*/];	// 记录各通道是否已经添加

	TPrsTimeSpan  m_tPrsTimeSpan;   // 重传时间参数
    u32           m_dwLastFUPTick;  // 上一次收到关键帧请求的Tick数

	u16			  m_wMTUSize;		// MTU的大小, zgc, 2007-04-02
    TCapSupportEx m_tCapSupportEx;

private:
	//------------消息响应--------------------
	void InstanceEntry( CMessage * const pcMsg );                 //消息入口
	void Init( CMessage * const pcMsg );                          //硬件初始化
	void MsgRegAckProc( CMessage * const pcMsg );                 //确认
	void MsgRegNackProc( CMessage * const pcMsg );                //拒绝
	void MsgStartVidMixProc( CMessage * const pcMsg );            //开始复合
	void MsgStopVidMixProc( CMessage * const pcMsg );             //停止复合
	void MsgChangeVidMixParamProc( CMessage * const pcMsg );      //改变复合参数
	void MsgGetVidMixParamProc( CMessage * const pcMsg );         //查询复合参数
	void MsgSetBitRate( CMessage * const pcMsg );
	void SendStatusChangeMsg( u8 byOnline, u8 byState, u8 byStyle ); //报告状态

	void ProcReconnectBCmd( CMessage * const pcMsg );	//MCU 通知会话 重联MPCB 相应实例, zgc, 2007/04/30
private:
	//-------------过程------------------------
    void ProcConnectTimeOut( BOOL32 bIsConnectA );
	BOOL32 ConnectMcu( BOOL32 bIsConnectA, u32& dwMcuNode );      //连接MCU
	void Disconnect( CMessage * const pMsg  );                    //断口连接
    void ProcRegisterTimeOut( BOOL32 bIsRegisterA );
	void Register( BOOL32 bIsRegisterA, u32 dwMcuNode );                                           //向MCU注册
	BOOL SendMsgToMcu( u16  wEvent, CServMsg* const pcServMsg );  //发送消息给MCU
	void StatusShow( void );                                      //显示所有的状态和统计信息
	BOOL StartHardwareMix( CKDVVMPParam& tParam );                //开始硬件合成
	BOOL ChangeVMPStyle( CKDVVMPParam& tParam );                  //改变合成风格
	BOOL ChangeVMPChannel( CKDVVMPParam& tParam );                //改变逻辑通道
	u8   GetFavirateStyle( u32 totalmap,u8 &channel );            //根据MAP数目来决定合成类型
	u8   ConvertVcStyle2HardStyle(  u8 oldstyle  );               //业务到底层的合成模式转换
	void ClearCurrentInst( void );                                //清空当前实例
	void ProcGetMsStatusRsp( CMessage * const pcMsg );            //取主备倒换状态

	void ParamShow(void);							// 显示业务合成参数, zgc, 2008-03-19
    u8   GetVmpNumOfStyle( u8 byVMPStyle );
    void MsgUpdataVmpEncryptParamProc( CMessage * const pMsg );
    void SetEncryptParam( u8 byChnNo, TMediaEncrypt *  ptVideoEncrypt, u8 byDoublePayload );
    void SetDecryptParam( u8 byChnNo, TMediaEncrypt *  ptVideoEncrypt, TDoublePayload * ptDoublePayload );

    void MsgTimerNeedIFrameProc( void );
    void MsgFastUpdatePicProc( void );
	
    void ProcSetQosInfo( CMessage * const pcMsg );
    void ComplexQos( u8& byValue, u8 byPrior );
    void ProcSetStyleMsg( CMessage * const pcMsg );
    BOOL32 SetVmpAttachStyle( TVmpStyleCfgInfo&  tStyleInfo );
	// 停止MAP的工作, zgc, 2007/04/24
	void ProcStopMapCmd( CMessage * const pcMsg );
    void ProcSetBackBoardOutCmd( CMessage * const pcMsg );

    u8 GetRColor( u32 dwColorValue );
    u8 GetGColor( u32 dwColorValue );
    u8 GetBColor( u32 dwColorValue );
    void GetRGBColor( u32 dwColor, u8& byRColor, u8& byGColor, u8& byBColor );
    
    u16 GetSendRtcpPort(u8 byChanNo);
    u8  VmpGetActivePayload(u8 byRealPayloadType);
    u8 GetVmpChlNumByStyle( u8 byVMPStyle );

    // 平滑发送支持
    void SetSmoothSendRule(u32 dwDestIp, u16 wPort, u32 dwRate);
    void ClearSmoothSendRule(u32 dwDestIp, u16 wPort);
    void StopSmoothSend();
        
};

class CVMPCfg
{
public:
    CVMPCfg();
    virtual ~CVMPCfg();
public:
    u32           m_dwMcuNode;      //连接的MCU.A 节点号
    u32           m_dwMcuNodeB;     //连接的MCU.B 节点号
    u32           m_dwMcuIId;       //与实例通讯的MCU.A 的全局ID
    u32           m_dwMcuIIdB;      //与实例通讯的MCU.B 的全局ID
    BOOL32        m_bEmbed;         //内嵌在A板
    BOOL32        m_bEmbedB;        //内嵌在B板
    u8            m_byRegAckNum;    //注册成功次数

	u32           m_dwMpcSSrc;      // guzh [6/12/2007] 业务侧会话校验值
public:
    void FreeStatusDataA();
    void FreeStatusDataB();
};

typedef zTemplate<CVMPInst, 1, CVMPCfg> CVmpApp;
extern CVmpApp g_cVMPApp;

#endif //_VIDEO_MULTIPLEXERINST_H_
