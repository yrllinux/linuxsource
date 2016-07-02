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
#include "bascfg.h"
#include "eqpcfg.h"

OSPEVENT( EV_BAS_TIMER,                EV_BAS_BGN );                //内部定时器
OSPEVENT( EV_BAS_NEEDIFRAME_TIMER,     EV_BAS_BGN + 1 );            //关键帧定时器
OSPEVENT( EV_BAS_CONNECT_TIMER,        EV_BAS_BGN + 2 );            //连接定时器
OSPEVENT( EV_BAS_CONNECT_TIMERB,       EV_BAS_BGN + 3 );            //连接定时器
OSPEVENT( EV_BAS_REGISTER_TIMER,       EV_BAS_BGN + 4 );            //注册定时器
OSPEVENT( EV_BAS_REGISTER_TIMERB,      EV_BAS_BGN + 5 );            //注册定时器
OSPEVENT( TIME_GET_MSSTATUS,           EV_BAS_BGN + 6 );            //取主备倒换状态
OSPEVENT( TIME_ERRPRINT,               EV_BAS_BGN + 7 );            //错误打印定时器
OSPEVENT( TIME_BAS_FLASHLOG,           EV_BAS_BGN + 8 );            //刷新日志记录定时器
OSPEVENT( EV_BAS_SHOW,				   EV_BAS_BGN + 9 );			//显示适配器状态
OSPEVENT( EV_BAS_INI,				   EV_BAS_BGN + 10 );           //bas初始化
OSPEVENT( EV_BAS_QUIT,				   EV_BAS_BGN + 11 );			//停止bas

#define BAS_CONNETC_TIMEOUT         (u16)3*1000  //connect time
#define BAS_REGISTER_TIMEOUT        (u16)6*1000  //register time
#define CHECK_IFRAME_INTERVAL       (u16)1000    //关键帧请求间隔时长
#define FIRST_REGACK                (u8)1        //第一次收到注册Ack
#if defined(_8KH_) || defined(_8KI_)
	#define MAXNUM_BAS_ENCNUM           (u8)5      //BAS最大编码通道数
#else
	#define MAXNUM_BAS_ENCNUM           (u8)4      //BAS最大编码通道数
#endif
#define BAS_FLASHLOG_TIMEOUT        (u32)(5*60*1000)    //日志刷新纪录
#define BAS_REG_NUM					(u8)3        //适配器注册个数

class CBASInst : public CInstance
{
	enum //实例状态
	{
		IDLE   = 0,
		NORMAL = 1,
		RUNNING = 2
	};
public:
	CBASInst();
	~CBASInst();
private:

    C8KEVMPParam  m_tParam[MAXNUM_BAS_ENCNUM];			// 合成参数
	TNetAddress   m_tSndAddr[MAXNUM_BAS_ENCNUM];		// 发送的远端地址
    TNetAddress   m_tSndLocalAddr[MAXNUM_BAS_ENCNUM];   // 发送的本地地址
	TNetAddress   m_tRcvLocalAddr;						// 外设侧接收地址
	TNetAddress   m_tRtcpAddr;							// 远端Rtcp请求地址
	TNetAddress   m_tLocalSndRtcpAddr;					// 本地Snd rtcp地址
    TMediaEncrypt m_tEncEncrypt[MAXNUM_BAS_ENCNUM];     // 编码加密参数
	//BOOL32        m_bDbVid;							// 当前是否双视频
	u8            m_byEncNum;							// 当前编码路数(用于请求关键帧)
    BOOL32        m_byNeedPrs;							// 是否支持丢包重传

    CConfId       m_cConfId;							// 当前会议号
	CCPParam      m_tCfg;								// 配置参数
    TVmpStyleCfgInfo  m_tStyleInfo;						// 合成风格参数
	T8keEqpCfg       m_tEqpCfg;							// 为每个instance分配一个外设信息
    u8            m_byChnType;                          // bas通道类型(注册mcu时通告mcu,区分主流、双流、选看类型通道)
	u32           m_dwMcuIId;							// 与实例通讯的MCU.A 的ID
	BOOL32        m_bInitOk;							// 编码器初始化是否成功
    //songkun,20110622,bas合成Iframe请求调整，增加等待，发送请求后不允许连续再发（等待时间用户配）
    u8			m_byIframeReqCount;
private:
	//-----------自身状态--------------
	CMulPic      *m_pcHardMulPic;						// 画面复合器的封装
    BOOL32        m_abChnlValid[MAXNUM_VMP_MEMBER];		// 记录各通道是否已经添加

	TPrsTimeSpan  m_tPrsTimeSpan;						// 重传时间参数
    u32           m_dwLastFUPTick;						// 上一次收到关键帧请求的Tick数
	u16			  m_wMTUSize;							// MTU的大小, zgc, 2007-04-02
    TCapSupportEx m_tCapSupportEx;

private:
	//------------daemon实例--------------------
    void DaemonInstanceEntry( CMessage* const pcMsg, CApp* pcApp );
	void DaemonProInit( CMessage * const pcMsg );                 //初始化
	void DaemonProcConnectTimeOut( void );						  //连接
	void DaemonProcRegisterTimeOut( void );						  //注册	
	void DaemonProcDisconnect( CMessage * const pMsg  );          //断链

private:
	//-------------instance处理------------------------
	void InstanceEntry( CMessage * const pcMsg );                 
	void ProBasInit(void);										  //硬件初始化
	void ProcRegAck( CMessage * const pcMsg );                    //注册成功
	void ProcRegNack();                   //注册失败
	void ProcStopAdapt( CMessage * const pcMsg );                 //停止适配
	void ProChangeAdapt( CMessage * const pcMsg );				  //改变适配参数

private:
	//-------------功能函数----------------------------
	BOOL32 ConnectMcu( u32& dwMcuNode );						  //连接MCU   
	void ProRegister();											  //向MCU注册
	BOOL SendMsgToMcu( u16  wEvent, CServMsg* const pcServMsg );  //发送消息给MCU
	void SendChnNotif(void);									  //
	void SendStatusChangeMsg( u8 byOnline, u8 byState);			  //报告状态
	void StatusShow( void );                                      //显示所有的状态和统计信息
	void ChangeCP(CServMsg& cServMsg);                            //改变合成参数
	u32  ConvertVcStyle2HardStyle(  u8 oldstyle  );               //业务到底层的合成模式转换
	void ClearCurrentInst( void );                                //清空当前实例

    void ProcVmpFlashLogTimeOut( void );

	void ParamShow(void);							// 显示业务合成参数, zgc, 2008-03-19

    void MsgUpdataVmpEncryptParamProc( CMessage * const pMsg );

    BOOL32 SetEncryptParam( u8 byChnNo, TMediaEncrypt *ptVideoEncrypt, u8 byMeidaType );
    BOOL32 GetEncryptParam( C8KEVMPParam *ptParam, TMediaEncrypt *ptVideoEncrypt );

    BOOL32 SetDecryptParam( u8 byChnNo, TMediaEncrypt *ptVideoEncrypt, TDoublePayload * ptDoublePayload );
    void MsgTimerNeedIFrameProc( void );
    void MsgFastUpdatePicProc( CMessage * const pMsg );
    void MsgChnnlLogOprCmd( CMessage * const pMsg );
	
    void ProcSetQosInfo( CMessage * const pcMsg );
    void ComplexQos( u8& byValue, u8 byPrior );
    void ProcSetStyleMsg( CMessage * const pcMsg );
    BOOL32 SetVmpAttachStyle( TVmpStyleCfgInfo&  tStyleInfo, C8KEVMPParam & tParam );

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

    BOOL32 IsSameEncParam( TVidEncParam * ptEncParam1, TVidEncParam * ptEncParam2 );
    void Trans2EncParam( const C8KEVMPParam * ptVmpParam, TVidEncParam * ptEncparam, BOOL32 bIsSupportHp = FALSE);
    s8 GetEncChnIdxByParam( TVidEncParam * ptEncParam );
     
	void ConvertRealParam(u8 byOutIdx,THDAdaptParam &tBasParam);
};

class CBASCfg
{
public:
    CBASCfg();
    virtual ~CBASCfg();
public:
    u32           m_dwMcuNode;      //连接的MCU.A 节点号
    u32           m_dwMcuIId;       //与实例通讯的MCU.A 的全局ID
    BOOL32        m_bEmbed;         //内嵌在A板
    u8            m_byRegAckNum;    //注册成功次数
    T8keEqpCfg    m_tEqpCfg;        //外设基本配置
    BOOL32        m_bInitOk;        //编解码器是否初始化成功
	u32			  m_dwIframeInterval;	//[nizhijun 2011/03/21]VMP关键帧定时请求时间
    
public:
    void FreeStatusData();
	void ConvertToRealFR(THDAdaptParam& tAdptParm);
};

typedef zTemplate<CBASInst, BAS_REG_NUM, CBASCfg> CBasApp;
extern CBasApp g_cBasApp;

#endif //_VIDEO_MULTIPLEXERINST_H_

