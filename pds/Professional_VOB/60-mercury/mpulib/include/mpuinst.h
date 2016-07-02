/*****************************************************************************
   模块名      : mpulib
   文件名      : mpuinst.h
   相关文件    : 
   文件实现功能: mpulib应用头文件
   作者        : 周文
   版本        : V4.6  Copyright(C) 2008-2010 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期       版本        修改人      修改内容
   2009/3/14    4.6         张宝卿      注释
   2009/7/19    4.6         张宝卿      逻辑调整
******************************************************************************/
#ifndef _MPUINST_H_
#define _MPUINST_H_

#include "mpustruct.h"
#include "eqpcfg.h"
#include "mpuutility.h"

#ifdef _LINUX_
#include "libsswrap.h"
#endif


#define MAXNUM_MPUBAS_CHANNEL        (u8)4
#define MAXNUM_MPUBAS_H_CHANNEL		 (u8)2
#define MAX_VIDEO_FRAME_SIZE         (u32)(512 * 1024)
#define MAX_AUDIO_FRAME_SIZE         (u32)(8 * 1024)
#define FRAME_RATE                   (u8)25

extern s32 g_nmpubaslog;

/************************************************************************/
/*                                                                      */
/*                          以下为CMpuBasInst                          */
/*                                                                      */
/************************************************************************/

class CMpuBasInst : public CInstance
{
protected:
    enum ESTATE
    {
        IDLE,
        READY,
        NORMAL,
    };

//临时释放出来，稍后整理
//private:
public:	
	TMpuBasChannelInfo    m_tChnInfo;
	CMpuBasAdpGroup       *m_pAdpCroup;
    TRtpPkQueue             *m_ptFrmQueue;
	//songkun,201106230,bas合成Iframe请求调整，增加等待，发送请求后不允许连续再发（等待时间用户配）
    u8			m_byIframeReqCount;
      
public:
    CMpuBasInst()
    {	    
        m_pAdpCroup = NULL;
        m_ptFrmQueue = NULL;
		m_byIframeReqCount = 0;
    }
    virtual ~CMpuBasInst()
    {
    }

    // ----------- 消息入口 ------------
    void  InstanceEntry( CMessage* const pMsg );
    void  DaemonInstanceEntry( CMessage* const pMsg, CApp* pcApp );

    // --------- 消息处理函数 ----------
	void  DaemonProcPowerOn( const CMessage* const pMsg/*, CApp* pcApp*/ );       //初始化
    void  DaemonProcOspDisconnect( CMessage* const pMsg/*, CApp* pcApp*/ ); //断链处理
    void  DaemonProcConnectTimeOut( BOOL32 bIsConnectA );               //连接总入口
    void  DaemonProcRegisterTimeOut( BOOL32 bIsRegiterA );              //注册总入口    
    void  DaemonProcMcuRegAck(const CMessage* const pMsg/*, CApp* pcApp */);      //处理注册
    void  DaemonProcMcuRegNack(CMessage* const pMsg) const;
	void  DeamonProcGetMsStatusRsp( CMessage* const pMsg ); 
    //void  DaemonProcSetQosCmd( CMessage* const pMsg );                  //设置Qos值
	void  DeamonProcModSendIpAddr( CMessage* const pMsg );
	void  DeamonShowBasDebugInfo();

    void  ProcInitBas();
    void  ProcStopBas();
    void  ProcStartAdptReq( CServMsg& );     //开始适配
    void  ProcChangeAdptCmd( CServMsg& );    //调整适配参数
    void  ProcStopAdptReq( CServMsg& );      //停止适配
	void  ProcFastUpdatePicCmd( /*CServMsg&*/ ); //Mcu请求MpuBas编关键帧
    void  ProcTimerNeedIFrame();             //MpuBas请求Mcu发关键帧
	void  ProcModSndAddr();                  //修改网络发送地址


	// ----------- 功能函数 ------------
    BOOL32 ConnectMcu( BOOL32 bIsConnectA, u32& dwMcuNode );            //具体连接函数
	void   Register( /*BOOL32 bIsRegiterA,*/ u32 dwMcuNode );               //向MCU注册
    void   SendMsgToMcu( u16 wEvent, CServMsg& cServMsg );
    void   SendChnNotif();                                                  //发送通道状态给Mcu
    BOOL32 GetBasInitChnlNumAcd2WorkMode(u8 byWorkMode, u8 &byInitChnlNum); //根据bas的启动模式决定初始化多少个(入)通道
	BOOL32 GetBasOutChnlNumAcd2WorkMode(u8 byWorkMode, u8 &byOutChnlNum);   //根据bas的启动模式决定初始化多少个(出)通道
    void   StatusShow( void );
	void   ClearInstStatus( void );                                     //清空本实例保存的状态信息
};

typedef zTemplate< CMpuBasInst , MAXNUM_MPUBAS_CHANNEL, CMpuBasCfg > CMpuBasApp;
extern CMpuBasApp g_cMpuBasApp;


 
/************************************************************************/
/*                                                                      */
/*                          以下为CMpuSVmpInst                          */
/*                                                                      */
/************************************************************************/

class CMpuSVmpInst : public CInstance
{
	enum //实例状态
	{
		IDLE   = 0,
		NORMAL = 1,
		RUNNING = 2
	};

public:
    CMpuSVmpInst() 
    {
        Clear();
    }
    virtual ~CMpuSVmpInst() 
    {
        u8 byLoop = 0;
        for ( ; byLoop < MAXNUM_MPUSVMP_MEMBER; byLoop++ )
        {
            if ( NULL != m_pcMediaRcv[byLoop] )
            {
                delete m_pcMediaRcv[byLoop];
                m_pcMediaRcv[byLoop] = NULL;
            }
        }
        byLoop = 0;
        for ( ; byLoop < MAXNUM_MPUSVMP_CHANNEL; byLoop++ )
        {
            if ( NULL != m_pcMediaSnd[byLoop] )
            {
                delete m_pcMediaSnd[byLoop];
                m_pcMediaSnd[byLoop] = NULL;
            }
        }
    }

private:
    u32           m_dwMcuNode;       // 连接的MCU.A 节点号
    u32           m_dwMcuNodeB;      // 连接的MCU.B 节点号
    u32           m_dwMcuIId;        // 与实例通讯的MCU.A 的全局ID
    u32           m_dwMcuIIdB;       // 与实例通讯的MCU.B 的全局ID
    u8            m_byRegAckNum;     // 注册成功次数
	u32           m_dwMpcSSrc;       // 业务侧会话校验值

    CConfId       m_cConfId;		 // 当前会议号
    u8            m_byChnConfType;   // 服务于高清还是标清会议,可能会用到

    TVmpStyleCfgInfo  m_tStyleInfo;  // 合成风格参数	
    

public:
	CHardMulPic   m_cHardMulPic;   
	CKdvMediaRcv* m_pcMediaRcv[MAXNUM_MPUSVMP_MEMBER];   // 20个接受
	CKdvMediaSnd* m_pcMediaSnd[MAXNUM_MPUSVMP_CHANNEL];  // 最多4路发送 
    
	CKDVNewVMPParam   m_tParam[MAXNUM_MPUSVMP_CHANNEL];	 // 合成参数
    CKDVNewVMPParam   m_tParamPrevious[MAXNUM_MPUSVMP_CHANNEL];  // 上一次合成参数
    u8            m_byHardStyle;     // 画面合成风格（m_tParam[0].m_byVMPStyle是mcu用的不是底层用的） 
	u32           m_dwMcuRcvIp;      // MCU接收地址
    u16           m_wMcuRcvStartPort;// MCU接收起始端口号

	TEqpCfg       m_tCfg;            // 基本配置
    TEqp          m_tEqp;            // 本设备
	TMpuVmpCfg    m_tMpuVmpCfg;      // 高级配置，读取和保存mcueqp.ini.注意是TMpuVmpCfg而不是TMpuDVmpCfg

	BOOL32        m_bAddVmpChannel[MAXNUM_MPUSVMP_MEMBER];// 记录各通道是否已经添加

	BOOL32		  m_bUpdateMediaEncrpyt[MAXNUM_MPUSVMP_CHANNEL];// 记录编码侧加密信息是否已更新

    //songkun,20110622,画面合成Iframe请求调整
    //[nizhijun 2011/03/24]该参数表示当合成参数发生变化时(开启/改变)，定时1S请求的关键帧请求是否发出给MCU
    u8			m_byIframeReqCount[MAXNUM_MPUSVMP_MEMBER];
private:
	//-----------自身状态--------------                 // 画面复合器的封装
	TPrsTimeSpan  m_tPrsTimeSpan;                       // 重传时间参数
	u16			  m_wMTUSize;		                    // MTU的大小
    u32           m_adwLastFUPTick[MAXNUM_MPUSVMP_CHANNEL];   // 上一次收到关键帧请求的Tick数

public:
	//------------消息响应--------------------
    void Clear();
	void InstanceEntry( CMessage * const pcMsg );                    //消息入口
	void Init( CMessage * const pcMsg );                             //硬件初始化
	void MsgRegAckProc( CMessage * const pcMsg );                    //确认
	void MsgRegNackProc( CMessage * const pcMsg );                   //拒绝
	void MsgStartVidMixProc( CMessage * const pcMsg );               //开始复合
	void MsgStopVidMixProc( CMessage * const pcMsg );                //停止复合
	void MsgChangeVidMixParamProc( CMessage * const pcMsg );         //改变复合参数
	void MsgSetBitRate( CMessage * const pcMsg );
	void MsgModSendAddrProc( CMessage* const pMsg );
	
	void SendStatusChangeMsg( u8 byOnline, u8 byState/*, u8 byStyle */); //报告状态
    s32  SendMsgToMcu( u16 wEvent, CServMsg& cServMsg );  

public:
	//-------------过程------------------------
    void ProcConnectTimeOut( BOOL32 bIsConnectA );
	BOOL32 ConnectMcu( BOOL32 bIsConnectA, u32& dwMcuNode );         //连接MCU
	void Disconnect( CMessage * const pMsg  );                       //断口连接
    void ProcRegisterTimeOut( BOOL32 bIsRegisterA );
	void Register(/* BOOL32 bIsRegisterA, */u32 dwMcuNode );             //向MCU注册
	void StatusShow( void );                                         //显示所有的状态和统计信息
	BOOL StartNetRcvSnd(u8 byNeedPrs);                               //开始码流收发
    BOOL StopNetRcvSnd( void );                                      //停止码流收发
    BOOL ChangeChnAndNetRcv(/*u8 byNeedPrs,*/ BOOL32 bStyleChged);       //绑定切变码流接收和通道
	BOOL ChangeVMPStyle();                                           //改变合成风格
	u8   ConvertVcStyle2HardStyle( u8 byVcStyle );                   //业务到底层的合成模式转换
    u8   ConvertHardStyle2VcStyle( u8 byHardStyle );                 //底层的合成模式到业务转换
	void ClearCurrentInst( void );                                   //清空当前实例
	void ProcGetMsStatusRsp( CMessage * const pcMsg );               //取主备倒换状态

	void ParamShow( void );							                 //显示业务合成参数
    void MsgUpdataVmpEncryptParamProc( CMessage * const pMsg );
    void SetEncryptParam( u8 byChnNo, TMediaEncrypt *  ptVideoEncrypt, u8 byRealPT );
    void SetDecryptParam( u8 byChnNo, TMediaEncrypt *  ptVideoEncrypt/*, TDoublePayload * ptDoublePayload */);
    void SetPrs( u8 byNeedPrs );
	
    void InitMediaSnd( u8 byChnNo );
    BOOL InitMediaRcv( u8 byChnNo );  
    void StartMediaRcv( u8 byChnNo );
	BOOL SetMediaRcvNetParam( u8 byChnNo );

	
    void MsgTimerNeedIFrameProc( void );
    void MsgFastUpdatePicProc( CMessage * const pMsg );
	
    //FIXME: QOS
    /*
    void ProcSetQosInfo( CMessage * const pcMsg );
    void ComplexQos( u8& byValue, u8 byPrior ); */

    BOOL32 SetVmpAttachStyle( TVmpStyleCfgInfo&  tStyleInfo );
    void GetRGBColor( u32 dwColor, u8& byRColor, u8& byGColor, u8& byBColor );
    u8   VmpGetActivePayload(u8 byRealPayloadType);

    // 平滑发送支持
	//[2010/12/31 zhushz]平滑发送接口封装
//  void SetSmoothSendRule( u32 dwDestIp, u16 wPort, u32 dwRate );
	void SetSmoothSendRule( BOOL32 bSetFlag );
//  void ClearSmoothSendRule( u32 dwDestIp, u16 wPort ); 
    
	void ProcSetFrameRateCmd( CMessage * const pcMsg );
	
	void MsgAddRemoveRcvChnnl(CMessage * const pMsg);	//增加/删除某接收通道// [9/15/2010 xliang] 

	void ShowVMPDebugInfo();							//显示VMPdebug信息 [nizhijun 2010/11/30]


	void ProcChangeMemAliasCmd(CMessage* const pMsg);

protected:
	void Trans2EncParam(u8 byEncChnIdx,CKDVNewVMPParam* ptKdvVmpParam, TVideoEncParam* ptVideEncParam);//将kdvnewvmpparam转换为对应的TVideoEncParam
};

class TMpuSVmpRcvCB
{
public:
    CMpuSVmpInst*  m_pThis;
    u8             m_byChnnlId; 
};

class CMpuSVmpCfg
{
public:
    CMpuSVmpCfg() {}
    virtual ~CMpuSVmpCfg() {}

public:
    u8               m_byWorkMode;               // 避免使用全局变量,在此保存WorkMode
    u8               m_bEmbed;
    TMpuSVmpRcvCB    m_atMpuSVmpRcvCB[MAXNUM_MPUSVMP_MEMBER];  
};

typedef zTemplate< CMpuSVmpInst , 1, CMpuSVmpCfg > CMpuSVmpApp;
extern CMpuSVmpApp g_cMpuSVmpApp;

extern TResBrLimitTable g_tResBrLimitTable; //码率分辨率限制信息表


#endif // !_MPUINST_H_

//END OF FILE

