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
#include "eqpcfg.h"
#include "kdvlog.h"
#include "loguserdef.h"
#include "drawbmp.h"

OSPEVENT( EV_VMP_TIMER,                EV_VMP_BGN );                //内部定时器
OSPEVENT( EV_VMP_NEEDIFRAME_TIMER,     EV_VMP_BGN + 1 );            //关键帧定时器
OSPEVENT( EV_VMP_CONNECT_TIMER,        EV_VMP_BGN + 21 );            //连接定时器
OSPEVENT( EV_VMP_CONNECT_TIMERB,       EV_VMP_BGN + 22 );            //连接定时器
OSPEVENT( EV_VMP_REGISTER_TIMER,       EV_VMP_BGN + 23 );            //注册定时器
OSPEVENT( EV_VMP_REGISTER_TIMERB,      EV_VMP_BGN + 24 );            //注册定时器
OSPEVENT( TIME_GET_MSSTATUS,           EV_VMP_BGN + 25 );            //取主备倒换状态
OSPEVENT( TIME_ERRPRINT,               EV_VMP_BGN + 26 );            //错误打印定时器
OSPEVENT( TIME_VMP_FLASHLOG,           EV_VMP_BGN + 27 );            //刷新日志记录定时器

#define VMP_CONNETC_TIMEOUT         (u16)3*1000  //connect time
#define VMP_REGISTER_TIMEOUT        (u16)6*1000  //register time
#define CHECK_IFRAME_INTERVAL       (u16)1000    //关键帧请求间隔时长
#define FIRST_REGACK                (u8)1        //第一次收到注册Ack
#ifdef _8KI_
	#define MAXNUM_VMP_ENCNUM           (u8)6      //8ki画面合成器最大编码通道数
#elif defined _8KH_
	#define MAXNUM_VMP_ENCNUM           (u8)5      //8kh画面合成器最大编码通道数
#else
	#define MAXNUM_VMP_ENCNUM           (u8)4      //8ke画面合成器最大编码通道数
#endif

#define VMP_FLASHLOG_TIMEOUT        (u32)(5*60*1000)    //日志刷新纪录

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

    C8KEVMPParam  m_tParam[MAXNUM_VMP_ENCNUM];			// 合成参数
	TNetAddress   m_tSndAddr[MAXNUM_VMP_ENCNUM];		// 合成后输出地址
    TNetAddress   m_tSndLocalAddr[MAXNUM_VMP_ENCNUM];   // 发送的本地地址
    TMediaEncrypt m_tEncEncrypt[MAXNUM_VMP_ENCNUM];     // 编码加密参数
	u8			  m_abyChanProfileType[MAXNUM_VMP_ENCNUM]; //合成通道的hp/bp属性

	BOOL32        m_bDbVid;				// 当前是否双视频
    BOOL32        m_byNeedPrs;
	

    CConfId       m_cConfId;		// 当前会议号
	
    TVmpStyleCfgInfo  m_tStyleInfo; // 合成风格参数

private:
	//-----------自身状态--------------
	CMulPic      *m_pcHardMulPic;   // 画面复合器的封装
    BOOL32        m_abChnlValid[MAXNUM_SDVMP_MEMBER];	// 记录各通道是否已经添加
	BOOL32        m_abChnlSetBmp[MAXNUM_SDVMP_MEMBER];
	BOOL32        m_abChnlIframeReqed[MAXNUM_SDVMP_MEMBER];	//关键帧请求是否已经发过，发过则按照配置文件定时
    TVmpMbAlias   m_tVmpMbAlias[MAXNUM_SDVMP_MEMBER]; // Vmp成员别名 [7/4/2013 liaokang]

	TPrsTimeSpan  m_tPrsTimeSpan;   // 重传时间参数
    u32           m_dwLastFUPTick;  // 上一次收到关键帧请求的Tick数

	u16			  m_wMTUSize;		// MTU的大小, zgc, 2007-04-02
    TCapSupportEx m_tCapSupportEx;
	CCPParam      m_tCfg;           // 配置参数
	TDebugVal	  m_tDbgVal;
    CDrawBmp      m_cBmpDrawer;    // 适应setlogo新接口，生成bitmap [6/9/2013 liaokang]

private:
	//------------消息响应--------------------
    void DaemonInstanceEntry( CMessage* const pcMsg, CApp* pcApp );
	void InstanceEntry( CMessage * const pcMsg );                 //消息入口
	void Init( CMessage * const pcMsg );                          //硬件初始化
	void MsgRegAckProc( CMessage * const pcMsg );                 //确认
	void MsgRegNackProc( CMessage * const pcMsg );                //拒绝
	void MsgStopVidMixProc( CMessage * const pcMsg );             //停止复合
	void MsgChangeVidMixParamProc( CMessage * const pcMsg );      //改变复合参数
	void MsgGetVidMixParamProc( CMessage * const pcMsg );         //查询复合参数
	void MsgSetBitRate( CMessage * const pcMsg );
    void SendStatusChangeMsg( u8 byOnline, u8 byState); //报告状态

private:
	//-------------过程------------------------
    void ProcConnectTimeOut( void );
	BOOL32 ConnectMcu( u32& dwMcuNode );      //连接MCU
	void Disconnect( CMessage * const pMsg  );                    //断口连接
    void ProcRegisterTimeOut( void );
	void Register( u32 dwMcuNode );                                           //向MCU注册
	BOOL SendMsgToMcu( u16  wEvent, CServMsg* const pcServMsg );  //发送消息给MCU
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

	void SetNeedIFrameTimer4VmpChl(void); //针对每个通道定时请求关键帧
    void MsgTimerNeedIFrameProc( CMessage * const pMsg );
    void MsgFastUpdatePicProc( CMessage * const pMsg );
    void MsgChnnlLogOprCmd( CMessage * const pMsg );
	
	void ProcChangeMemAliasCmd(CMessage * const pMsg);
	
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
    void Trans2EncParam(  C8KEVMPParam * ptVmpParam, TVidEncParam * ptEncparam );
    s8 GetEncChnIdxByParam( TVidEncParam * ptEncParam );
	//zhouyiliang20110331 增加处理停，开单通道处理函数
	BOOL32 SetOutputChnnlActive(u8 byOutChnlIdx,BOOL32 bActive);
	void ProcStartStopChnnlCmd(CMessage * const pcMsg);
	void MsgAddRemoveRcvChnnl( CMessage * const pcMsg);
//	void ProcStartStopAllChnnlCmd(CMessage * const pcMsg);
	//zhouyiliang 20110629 单独修改某一编码通道编码参数
//	void ProcChangeSingleChnlEncParam(CMessage* const pcMsg);

    void SetLogo(u8 byChnNo, BOOL32 bDisplay, s8 *pszBuf = NULL);
        
};

class CVMPCfg
{
public:
    CVMPCfg();
    virtual ~CVMPCfg();
public:
    u32           m_dwMcuNode;      //连接的MCU.A 节点号
    u32           m_dwMcuIId;       //与实例通讯的MCU.A 的全局ID
    BOOL32        m_bEmbed;         //内嵌在A板
    u8            m_byRegAckNum;    //注册成功次数
    T8keEqpCfg       m_tEqpCfg;        //外设基本配置
    BOOL32        m_bInitOk;        //编解码器是否初始化成功
	u32			  m_dwIframeInterval;	//[nizhijun 2011/03/21]VMP关键帧定时请求时间
    
public:
    void FreeStatusData();
};

typedef zTemplate<CVMPInst, 1, CVMPCfg> CVmpApp;
extern CVmpApp g_cVMPApp;


#endif //_VIDEO_MULTIPLEXERINST_H_

