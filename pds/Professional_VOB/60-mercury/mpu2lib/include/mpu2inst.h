/*****************************************************************************
   模块名      : mpu2lib
   文件名      : mpu2inst.h
   相关文件    : 
   文件实现功能: mpu2lib应用头文件
   作者        : 倪志俊
   版本        : V4.7  Copyright(C) 2012-2015 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期       版本        修改人      修改内容
   2012/07/26  4.7         倪志俊		创建
******************************************************************************/
#ifndef _MPUINST_H_
#define _MPUINST_H_

#include "mpu2struct.h"
#include "eqpcfg.h"
#include "mpu2utility.h"
#include "mcuinnerstruct.h"

#ifdef _LINUX_
#include "libsswrap.h"
#endif
#include "drawbmp.h"

#define MAXNUM_MPU2BAS_BASIC_CHANNEL         (u8)4
#define MAXNUM_MPU2BAS_EHANCED_CHANNEL		 (u8)7
#define MAXNUM_MPUBAS_CHANNEL        (u8)4
#define MAXNUM_MPUBAS_H_CHANNEL		 (u8)2
#define MAX_VIDEO_FRAME_SIZE         (u32)(512 * 1024)
#define MAX_AUDIO_FRAME_SIZE         (u32)(8 * 1024)
#define FRAME_RATE                   (u8)25

extern s32 g_nmpubaslog;

extern u16 g_wKdvlogMid; //当前打印的kdvlog的模块



/************************************************************************/
/*                                                                      */
/*                          以下为CMpuBasInst                          */
/*                                                                      */
/************************************************************************/

class CMpu2BasInst : public CInstance
{
private:	
	TMpuBasChannelInfo		 m_tChnInfo;			//BAS通道状态信息
	CMpu2BasAdpGroup		 *m_pAdpCroup;			//适配通道应用封装(网络)
    TRtpPkQueue              *m_ptFrmQueue;			//音频缓存包管理
	BOOL32					m_bIsIframeFirstReq;  //[nizhijun 2011/03/24]该参数表示当适配参数发生变化时(开启/改变)，定时1S请求的关键帧请求是否发出给MCU 
    u8						m_byBasIdx;			//属于第几个外设
	u8						m_byChnType;		//mpu2 bas每个通道能力不同，故需要像8KE BAS通道上报一样做处理
	u8						m_byBasChnIdx;		//bas通道号记录，因为存在两个外设的情况，故需要记录该值
protected:
	//daemon状态机
	enum EDaemonInstancStatus
	{
			DAEMON_IDLE ,
			DAEMON_INIT ,
			DAEMON_NORMAL ,
	};
	
	//instance状态机
    enum EInstanceState
    {
        IDLE,
		READY,
		RUNNING,
    };
	
public:
    CMpu2BasInst()
    {	    
        m_pAdpCroup = NULL;
        m_ptFrmQueue = NULL;
		m_bIsIframeFirstReq = FALSE;
    }
    virtual ~CMpu2BasInst()
    {
    }
	
    // ----------- 消息入口 ------------
    void  InstanceEntry( CMessage* const pMsg );
    void  DaemonInstanceEntry( CMessage* const pMsg, CApp* pcApp );
	
    // --------- 消息处理函数 ----------
	void  DaemonProcPowerOn( const CMessage* const pMsg);         //初始化
    void  DaemonProcOspDisconnect( CMessage* const pMsg );			//断链处理
    void  DaemonProcConnectTimeOut( BOOL32 bIsConnectA );							//连接总入口
    void  DaemonProcRegisterTimeOut( CMessage* const pMsg, BOOL32 bIsRegiterA );    //注册总入口    
    void  DaemonProcMcuRegAck(const CMessage* const pMsg);        //处理注册
	void  DaemonProc8KIMcuRegAck(const CMessage* const pMsg);
    void  DaemonProcMcuRegNack(CMessage* const pMsg) const;
	void  DeamonProcGetMsStatusRsp( CMessage* const pMsg ); 
	void  DeamonProcModSendIpAddr( CMessage* const pMsg );
	void  DeamonShowBasDebugInfo();
	
    void  ProcInitBas(CMessage* const pcMsg);
    void  ProcStopBas();
    void  ProcStartAdptReq( CServMsg& );     //开始适配
    void  ProcChangeAdptCmd( CServMsg& );    //调整适配参数
    void  ProcStopAdptReq( CServMsg& );      //停止适配
	void  ProcFastUpdatePicCmd(); //Mcu请求MpuBas编关键帧
    void  ProcTimerNeedIFrame();             //MpuBas请求Mcu发关键帧
	void  ProcModSndAddr();                  //修改网络发送地址

	CMpu2BasAdpGroup* GetBasAdpGroup() const { return m_pAdpCroup; }
	TRtpPkQueue*	  GetRtpPkQueue() const { return m_ptFrmQueue; }

	void  AdjustParamForMpuSimulating(THDAdaptParam *paParam, u8 byOutPutNum);
	
private:
	// ----------- 功能函数 ------------
    BOOL32 ConnectMcu( BOOL32 bIsConnectA, u32& dwMcuNode );            //具体连接函数
	void   Register( u8 byIdx, u32 dwMcuNode );               //向MCU注册
    void   SendMsgToMcu( u16 wEvent, CServMsg& cServMsg );
    void   SendChnNotif();                                                  //发送通道状态给Mcu
    BOOL32 GetBasInitChnlNumAcd2WorkMode(u8 byWorkMode, u8 &byInitChnlNum); //根据bas的启动模式决定初始化多少个(入)通道
	BOOL32 GetBasOutChnlNumAcd2WorkModeAndChnl(u8 byWorkMode, u8 byChnType, u8 &byOutChnlNum);   //根据bas的启动模式和通道号，获得当前通道的输出通道数
	BOOL32 GetFrontOutNumAcd2WorckModeAndChnl(u8 byWorkMode, u8 byChnType, u8 byChnId,u8 &byOutFrontOutNum);   //根据bas的启动模式和通道号，获得之前所有通道的的输出通道数综合
    void   StatusShow( void );
	void   ClearInstStatus( void );                                     //清空本实例保存的状态信息
};

typedef zTemplate< CMpu2BasInst , MAXNUM_MPU2BAS_BASIC_CHANNEL*2, CMpu2BasData > CMpu2BasApp;
extern CMpu2BasApp g_cMpu2BasApp;


 
/************************************************************************/
/*                                                                      */
/*                          以下为CMpu2VmpInst                          */
/*                                                                      */
/************************************************************************/

class CMpu2VmpInst : public CInstance
{
	enum //实例状态
	{
		IDLE   = 0,
		NORMAL = 1,
		RUNNING = 2
	};

	enum eDaemonInstStatus
	{
		DAEMON_IDLE = 0,
		DAEMON_INIT = 1 ,
		DAEMON_NORMAL = 2
	};
	
	enum eOnLineState
	{
		OFFLINE = 0,
		ONLINE = 1
	};
	enum EnumUseState //与mcu侧的usestate值保持一致
	{ 
		USESTATE_IDLE,		//未被占用

		USESTATE_START = 3,      //正进行画面合成
	};
public:
    CMpu2VmpInst() 
    {
        Clear();
	
    }
    virtual ~CMpu2VmpInst() 
    {
      Clear();
    }

private:
	u32           m_dwMcuIId;        // 与实例通讯的MCU.A 的全局ID
    u32           m_dwMcuIIdB;       // 与实例通讯的MCU.B 的全局ID
    u8            m_byRegAckNum;     // 注册成功次数
	u32           m_dwMpcSSrc;       // 业务侧会话校验值
    
    CConfId       m_cConfId;		 // 当前会议号

	BOOL32        m_bCreateHardMulPic; //当前实例创建hardmulpic是否成功

    emenCodingForm  m_emMcuEncoding; //[4/9/2013 liaokang] 编码方式

public:
	CHardMulPic   m_cHardMulPic;   
protected:
	CKdvMediaRcv* m_pcMediaRcv[MAXNUM_MPU2VMP_MEMBER];   // 20个接受
	CKdvMediaSnd* m_pcMediaSnd[MAXNUM_MPU2VMP_CHANNEL];  // 最多9路发送 
    
	//模拟MPU板时用的结构
//	CKDVNewVMPParam   m_tMpuParam[MAXNUM_MPUSVMP_CHANNEL];	 // 合成参数
//    CKDVNewVMPParam   m_tMpuParamPrevious[MAXNUM_MPUSVMP_CHANNEL];  // 上一次合成参数
	TVmpStyleCfgInfo  m_tStyleInfo;  // 合成风格参数	
    
	//MPU2 VMP用的结构
	CKDVVMPOutMember   m_tParam[MAXNUM_MPU2VMP_CHANNEL];	 // 合成参数
    CKDVVMPOutMember   m_tParamPrevious[MAXNUM_MPU2VMP_CHANNEL];  //上一次合成参数
	TMPU2CommonAttrb   m_tVmpCommonAttrb;
	TMPU2CommonAttrb   m_tPreviousVmpCommonAttrb;
    TVmpMbAlias        m_tVmpMbAlias[MAXNUM_MPU2VMP_MEMBER]; // Vmp别名扩容 [7/3/2013 liaokang]

    u8            m_byHardStyle;     // 画面合成风格（m_tParam[0].m_byVMPStyle是mcu用的不是底层用的） 
	u32           m_dwMcuRcvIp;      // MCU接收地址
    u16           m_wMcuRcvStartPort;// MCU接收起始端口号

	TEqpBasicCfg       m_tCfg;            // 基本配置
    TEqp          m_tEqp;            // 本设备
	TMpuVmpCfg    m_tMpuVmpCfg;      // 高级配置，读取和保存mcueqp.ini.注意是TMpuVmpCfg而不是TMpuDVmpCfg
	BOOL32        m_bAddVmpChannel[MAXNUM_MPU2VMP_MEMBER];// 记录各通道是否已经添加

	BOOL32		  m_bUpdateMediaEncrpyt[MAXNUM_MPU2VMP_CHANNEL];// 记录编码侧加密信息是否已更新


private:
	//-----------自身状态--------------                 // 画面复合器的封装
	TPrsTimeSpan  m_tPrsTimeSpan;                       // 重传时间参数
	u16			  m_wMTUSize;		                    // MTU的大小
    u32           m_adwLastFUPTick[MAXNUM_MPU2VMP_CHANNEL];   // 上一次收到关键帧请求的Tick数
	u8            m_byMaxVmpOutNum; //vmp最大的后适配个数，（mpu2 9路，模拟mpu时4路/A板3路（A板应该不产了））
	u8            m_byValidVmpOutNum;//vmp当前实际编码的路数
    BOOL32		  m_abChnlIframeReqed[MAXNUM_MPU2VMP_MEMBER];//关键帧请求是否已经发过，发过则按照配置文件定时
	BOOL32        m_bIsSimMpu;   //当前是否是模拟mpu
	BOOL32		  m_bIsDisplayMbAlias; //当前画面合成是否要显示终端别名
	CDrawBmp      m_cBmpDrawer;
	u32			  m_dwInitVmpMode;//初始的模式VMP_BASIC或则VMP_ENHANCED
	BOOL32        m_bFastUpdate; //单独请求关键帧,用于开启带多个下级多回传成员的合成
	
public:
	//------------消息响应--------------------
    void Clear();
	void DaemonInstanceEntry(CMessage *const pcMsg, CApp* pApp );//Daemon消息入口
	void InstanceEntry( CMessage * const pcMsg ); //消息入口
	void DaemonInit(CMessage* const pcMsg);
	void DaemonProcDisconnect( CMessage * const pMsg  );
	void Init( CMessage * const pcMsg );                             //硬件初始化
	void MsgRegAckProc( CMessage * const pcMsg );                    //确认
	void MsgRegNackProc( CMessage * const pcMsg );                   //拒绝

	void MsgStartVidMixProc( CMessage * const pcMsg );               //开始复合	
	void MsgChangeVidMixParamProc( CMessage * const pcMsg );         //改变复合参数  
	void MsgStopVidMixProc( CMessage * const pcMsg );                //停止复合

	void MsgSetBitRate( CMessage * const pcMsg );
	void MsgModMcuRcvAddrProc( CMessage* const pMsg );
	
	void SendStatusChangeMsg( u8 byOnline, u8 byState/*, u8 byStyle */); //报告状态
    s32  SendMsgToMcu( u16 wEvent, CServMsg& cServMsg );  

	void MsgAddRemoveRcvChnnl(CMessage * const pMsg);//增加删除某个单独的通道

	void MpuNullParam2ValidParam(CKDVNewVMPParam& tInOutMpuParam,u8 byChnIdx);
	
	void MpuFrameRateCheck(CKDVNewVMPParam& tInOutMpuParam);

public:
	//-------------过程------------------------
	void DaemonProcStatusShow();
	void DaemonProcParamShow();
    void DaemonProcConnectTimeOut( BOOL32 bIsConnectA );
	BOOL32 ConnectMcu( BOOL32 bIsConnectA, u32& dwMcuNode );         //连接MCU
	void Disconnect();                       //断口连接
    void ProcRegisterTimeOut( BOOL32 bIsRegisterA );
	void Register(/* BOOL32 bIsRegisterA, */u32 dwMcuNode );             //向MCU注册
	void StatusShow( void );                                         //显示所有的状态和统计信息
	BOOL StartNetRcvSnd(u8 byNeedPrs);                               //开始码流收发
    BOOL StopNetRcvSnd( void );                                      //停止码流收发
    BOOL ChangeChnAndNetRcv(/*u8 byNeedPrs,*/ BOOL32 bStyleChged);       //绑定切变码流接收和通道
	BOOL ChangeVMPStyle();                                           //改变合成风格
//	u8   ConvertVcStyle2HardStyle( u8 byVcStyle );                   //业务到底层的合成模式转换
    u8   ConvertHardStyle2VcStyle( u8 byHardStyle );                 //底层的合成模式到业务转换
	void ClearCurrentInst( void );                                   //清空当前实例
	void ProcGetMsStatusRsp( CMessage * const pcMsg );               //取主备倒换状态
	void ProcChangeMemAliasCmd(CMessage* const pcMsg);

	void ParamShow( void );							                 //显示业务合成参数
    void MsgUpdateVmpEncryptParamProc( CMessage * const pMsg );
    void SetEncryptParam( u8 byChnNo, TMediaEncrypt *  ptVideoEncrypt, u8 byRealPT );
    void SetDecryptParam( u8 byChnNo, TMediaEncrypt *  ptVideoEncrypt/*, TDoublePayload * ptDoublePayload */);
    void SetPrs( u8 byNeedPrs );
	
    void InitMediaSnd( u8 byChnNo );
    BOOL32 InitMediaRcv( u8 byChnNo );  
    void StartMediaRcv( u8 byChnNo );
	BOOL SetMediaRcvNetParam( u8 byChnNo );

    void MsgTimerNeedIFrameProc(  CMessage * const pMsg );
    void MsgFastUpdatePicProc( CMessage * const pMsg );


    BOOL32 SetVmpAttachStyle( TVmpStyleCfgInfo&  tStyleInfo );
    void GetRGBColor( u32 dwColor, u8& byRColor, u8& byGColor, u8& byBColor );
    u8   VmpGetActivePayload(u8 byRealPayloadType);

    // 平滑发送支持
	//[2010/12/31 zhushz]平滑发送接口封装
	void SetSmoothSendRule( BOOL32 bSetFlag );
    
	void ProcSetFrameRateCmd( CMessage * const pcMsg );
	


protected:
    //[4/9/2013 liaokang] MCU编码方式    
    void    SetMcuEncoding(emenCodingForm emMcuEncoding);
    emenCodingForm GetMcuEncoding();

	//高级配置相关的常用接口封装
	void ShowVMPDebugInfo();							//显示VMPdebug信息 [nizhijun 2010/11/30]
	BOOL32 IsEnableBitrateCheat()const;
	u16   GetBitrateAfterCheated(u16 wOriginBitrate)
	{
		return m_tMpuVmpCfg.GetDecRateDebug(wOriginBitrate);
	}
	
	u8 GetSubTypeByWorkMode(u8 byWorkMode);
	s32 SetIdleChnShowMode(u8 byShowMode,BOOL32 bStart);
	BOOL32 SetMediaSndNetParam(u8 byChnNo);
	//当前是否模拟mpu
	BOOL32 IsSimulateMpu()const
	{
		return m_bIsSimMpu;
	}
	//设置当前模拟mpu
	void SetIsSimulateMpu(BOOL32 bIsSimMpu)
	{
		m_bIsSimMpu = bIsSimMpu;
	}

	
	//当前画面合成是否需要显示别名
	BOOL32 IsDisplayMbAlias()const
	{
		return m_bIsDisplayMbAlias;
	}
	//设置当前画面合成是否需要显示别名
	void SetIsDisplayMbAlias(BOOL32 const& bIsDisplay)
	{
		m_bIsDisplayMbAlias = bIsDisplay;
	}
	//设置终端别名显示
	void DisplayMbAlias(u8 const byChnnl,const s8* pMemberAlias);
	


	u8 GetEncCifChnNum(); //获取当前编cif码流的通道
	u8 GetEncOtherChnNum();//获取当前编other码流的通道


	void TransFromMpuParam2EncParam(CKDVNewVMPParam* ptKdvVmpParam, TVideoEncParam* ptVideEncParam);

	void TransFromMpu2Param2EncParam(CKDVVMPOutMember* ptKdvVmpPamam,TVideoEncParam* ptVideEncParam);

	s32  SetNoStreamBakByBmpPath(u32 dwBakType,const s8* pBmpPath,u32 dwChnlId = MULPIC_MAX_CHNS);

    // 解析Vmp整体操作消息 [7/3/2013 liaokang]
    void UnpackVmpWholeOperMsg(CServMsg & cServMsg, BOOL32 bStart);

};


class TMpu2VmpRcvCB
{
public:
    CHardMulPic*  m_pHardMulPic;
    u8             m_byChnnlId; 
};

class CMpu2VmpCfg
{
public:
    CMpu2VmpCfg() 
	{
		m_byVmpEqpNum = 0;
		m_dwMcuNode = 0;
		m_dwMcuNodeB = 0;
		m_byWorkMode = 0;
		memset(m_atMpu2VmpRcvCB,0,sizeof(TMpu2VmpRcvCB));
	}
    virtual ~CMpu2VmpCfg() {}

public:
    u8            m_byWorkMode;        // 避免使用全局变量,在此保存WorkMode
	u32           m_dwMcuNode;       // 连接的MCU.A 节点号
    u32           m_dwMcuNodeB;      // 连接的MCU.B 节点号
	u8            m_byVmpEqpNum;     //配置的vmp外设个数
	
    TMpu2VmpRcvCB    m_atMpu2VmpRcvCB[MAXNUM_MPU2_EQPNUM][MAXNUM_MPU2VMP_MEMBER];  
};

typedef zTemplate< CMpu2VmpInst , MAXNUM_MPU2_EQPNUM, CMpu2VmpCfg > CMpu2VmpApp;
extern CMpu2VmpApp g_cMpu2VmpApp;

extern TResBrLimitTable g_tResBrLimitTable;


#endif // !_MPUINST_H_

//END OF FILE


