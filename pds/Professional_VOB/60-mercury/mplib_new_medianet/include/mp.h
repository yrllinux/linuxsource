 /*****************************************************************************
   模块名      : MP
   文件名      : mp.h
   相关文件    : 
   文件实现功能: MP的消息接口定义
   作者        : 胡昌威
   版本        : V4.0  Copyright(C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期       版本        修改人          修改内容
   2003/07/10   0.1         胡昌威          创建
   2007/03/21   4.0         张宝卿          广播交换逻辑的增加、代码优化
******************************************************************************/
#ifndef __DMP_H_
#define __DMP_H_

#include "osp.h"
#include "evmp.h"
#include "mcustruct.h"
#include "dataswitch.h"
#include "kdvmedianet.h"

//#ifdef WIN32
//#include "dataswitch36.h"
//#else
//#include "dataswitch.h"
//#endif

#define ENTER( hSem) \
        CSemOpt cSemOpt( hSem );

#define NEW_DS

// 打印级别
#define LOG_CRIT                        (u8)0
#define LOG_ERR                         (u8)1
#define LOG_WARN                        (u8)2
#define LOG_INFO                        (u8)3
#define LOG_DETAIL                      (u8)4
#define LOG_VERBOSE                     (u8)5

#define MAXNUM_HISPACKS                 (u8)100

// 内部切换的源 端口状态 定义
#define PS_IDLE                         (u8)0
#define PS_GETSRC                       (u8)1
#define PS_SWITCHED                     (u8)2
#define PS_UNEXIST                      (u8)0xff

#define POINT_NUM                       (u8)2           // 最大有效源切点数

#define INVALID_IPADDR                  0xffffffff
#define INVALID_SWITCH_CHANNEL          (u8)255         // 无效的交换序号
#define NUM_IPADDR                      (u8)1           // 创建ds时IP数

#define MP_CONNECT_TIMEOUT              (u16)(2*1000)   // 连接超时定时器
#define MP_REGISTER_TMEROUT             (u16)(4*1000)   // 注册超时定时器
#define MP_CHGSRCTOT_CHECK_TIMEOUT      (u16)100        // 源切换检查定时器
#define MP_CHGSRCTOT_TIMEOUT            (u16)3000       // 源切换最大等待时间

#define WAITIFRAME_START_TIMER          (u16)1000       // 等关键帧定时器起始id,跳过osp内部消息id
#define DEFAULT_TIMESTAMP_INTERVAL      (u16)3600
#define SWITCH_SRC_SEQ_INTERVAL         (u8)6   
#define MAX_SEQNUM_INTERVAL             (s32)50
#define MIN_SEQNUM_INTERVAL             (s32)(-50)
#define RTP_FIXEDHEADER_SIZE            (s32)12
#define MAX_TIMESTAMP_INTERVAL          (s32)(MAX_SEQNUM_INTERVAL * DEFAULT_TIMESTAMP_INTERVAL)
#define MIN_TIMESTAMP_INTERVAL          (s32)(MIN_SEQNUM_INTERVAL * DEFAULT_TIMESTAMP_INTERVAL)


//mp内部net终端端口扩展, 跳过mcu当前的udp端口使用，适应总后400个下属点
#define RCV_PORT_OFFSET					(u16)10000
#define SND_PORT_OFFSET					(u16)20000


// 交换类型
#define SWITCH_NONE                     (u8)0
#define SWITCH_ONE2ONE                  (u8)1
#define SWITCH_MUL2ONE                  (u8)2
#define SWITCH_DUMP                     (u8)3

#define NETFRFCSTAT_TIMESPAN            (u32)5          // DataSwitch 网络统计时间间隔
#define NETFRFCSTAT_BUFFER              (u32)120        // 总缓存时间

/* 每个会议支持的广播交换的起始地址，每个会议占用10端口：
   +0广播节点，+2接收源节点一，+4接收源节点二。其中 接收源节点一和接收源节点二在逻
   辑上功能上都没有先后次序，且当前所有的空闲可用端口范围为2000，可支持200个会议 */
#define CONFBRD_STARTPORT               (u16)59000

//关键帧获取 系列宏定义
#define     EX_HEADER_SIZE              4
#define     MIN_PACK_EX_LEN             4


#define MAX_SWITCH_PERPAGE              (u16)100   // 交换表每页打印的表项

// DataSwitch 网络统计时间间隔
#define NETFRFCSTAT_TIMESPAN            (u32)5
// 总缓存时间
#define NETFRFCSTAT_BUFFER              (u32)120

// 用于归一化处理
typedef struct
{
    s32  nSeqNumInterval;       //发送包序号的间隔
    s32  nTimeStampInterval;    //发送包时截的间隔

    u16  wModifySeqNum;         //修改的发送包的序号
    u32  dwModifyTimeStamp;     //修改的发送包的时截
    u32  dwModifySSRC;          //修改的发送包的SSRC

    u16  wLastSeqNum;           //最后的发送包的序号
    u32  dwLastTimeStamp;       //最后的发送包的时截
    u32  dwLastSSRC;            //最后的发送包的SSRC

}TSendFilterParam;

typedef struct
{
    u8     byConfNo;            //会议号
    u16    wChannelNo;          //信道号
    u32    dwDstIp;
    u16    wDstPort;
    u32    dwAddTime;           //增加交换的时间
    BOOL32 bRemoved;            //对应的交换是否删除
    BOOL32 bUniform;
    u8     byPayload;

    u16    wSeqNum;
    u32    dwTimeStamp;
    u32    dwSSRC;

    u16    wRcvPort;            /*本字段正常交换情况下都未赋值，目前只针对交换
                                  M->S1/S2->T。 原有标识无法区分，特此标识。*/
}TSendFilterAppData;

//等待关键帧信息
typedef struct 
{
    u32 dwSrcIp;
    u16 wSrcPort;
    u8  byConfNo;               //0-index
    u8  byMtId;   
    BOOL32 bWaitingIFrame;      //是否需要等关键帧
    u32  dwFstTimeStamp;

}TWaitIFrameData;


// 网络流量统计对象
class CNetTrfcStat
{
public:
    CNetTrfcStat();
    ~CNetTrfcStat();

public:
    // 追加最新值
    void AppendLatestStat( s64 lnRecv, s64 lnSend );

    // 打印流量值
    void DumpLatestStat();
    void DumpAllStat();

    // 根据流量统计判断是否超负载
    BOOL32 IsNetTrfcOverrun();
	
	// zgc [2007/01/12] 根据指定时间跨度统计流量是否超负载
	BOOL32 IsFluxOverrun(const u32 nTimeSpan);

    // 取指定时间段内的网络负载
    void  GetAvgNetBurden(u32 nTimeSpan, s32 &nSend, s32 &nRecv);
private:
    void Reset(BOOL32 bRecv = TRUE, BOOL32 bSend = TRUE);

private:
     // 缓存的统计值
     s64 m_alnNetRecvBytes[NETFRFCSTAT_BUFFER/NETFRFCSTAT_TIMESPAN+1];
     s64 m_alnNetSendBytes[NETFRFCSTAT_BUFFER/NETFRFCSTAT_TIMESPAN+1];

public:
    s32  m_nMaxCap; // 本MP的最大转发能力，单位Kbps

};


class CMpInst: public CInstance
{
    enum 
    { 
        STATE_IDLE,
        STATE_NORMAL
    };

public:
    CMpInst();
    virtual ~CMpInst();
    
public:
    void InstanceEntry(CMessage * const pcMsg);							    
    void DaemonInstanceEntry(CMessage * const pcMsg, CApp* pcApp);
    
    void ProcPowerOn();
    void ProcConnectTimeOut(BOOL32 bIsConnectA);
    BOOL32 ConnectToMcu(BOOL32 bIsConnectA, u32& dwMcuNode);	//连接Mcu
    void ProcRegisterTimeOut(BOOL32 bIsRegisterA);
    void RegisterToMcu(BOOL32 bIsRegisterA, u32 dwMcuNode);		//向Mcu注册
    void ProcDisconnect(CMessage * const pcMsg);				//断链处理
    void ProcMcuRegisterRsp(CMessage * const pcMsg);			//Mcu注册回应处理
	void ProcGetMsStatusRsp(CMessage* const pcMsg);             // 取主备倒换状态
    void ProcAddSwitchReq(CMessage * const pcMsg);				//处理Mcu的增加交换请求
    void ProcRemoveSwitchReq(CMessage * const pcMsg);			//处理Mcu的移除交换请求
    void ProcGetStatusReq(CMessage * const pcMsg);				//处理Mcu的得到状态请求
	void ProcReleaseConfNotify(CMessage * const pcMsg);			//处理Mcu的结束会议通知
	
    void ProcAddMulToOneSwitchReq(CMessage * const pcMsg);		//处理Mcu的增加多点到一点交换请求
	void ProcRemoveMulToOneSwitchReq(CMessage * const pcMsg);	//处理Mcu的移除多点到一点交换请求
	void ProcStopMulToOneSwitchReq(CMessage * const pcMsg);		//处理Mcu的停止多点到一点交换请求
	
    void ProcAddRecvOnlySwitchReq(CMessage * const pcMsg);		//处理Mcu的添加只接收交换的请求
	void ProcRemoveRecvOnlySwitchReq(CMessage * const pcMsg);	//处理Mcu的移除只接收交换的请求

    void ProcAddBroadSwitchSrcReq(CMessage * const pcMsg);      // 处理Mcu的添加广播源的请求
    void ProcAddBroadSwitchDstReq(CMessage * const pcMsg);      // 处理Mcu的添加广播目标的请求
    void ProcRemoveBroadSwitchSrcReq(CMessage * const pcMsg);   // 处理Mcu的移除广播源的请求

    void ProcConfUniformModeNotify(CMessage * const pcMsg);
    void DaemonProcWaitingIFrameTimer(CMessage * const pcMsg);

    void ProcAddTable(CMessage* const pcMsg);
    void ProcNetStat(CMessage* const pcMsg);
	void ProcFluxOverRun(CMessage* const pcMsg);
	void ProcSendFluxOverrunNotify(CMessage* const pcMsg);
    void NotifyMcuFluxStatus( void );
    void RestrictFluxOfMp(CMessage* const pcMsg);
    void ProcChangeSrcTimeout( void );
    void ProcCallBackSwitchChgNtf(CMessage* const pcMsg);
    BOOL32 SwitchSrcToT( u8 byConfIdx, u8 byPoint, TSwitchChannel &tSrcSwitch, 
                         u8 bySwitchChnMode, u8 byUniformPayload, BOOL32 bSwitchNow = FALSE );

    void ClearInstStatus(void);
    void ShowSwitch(void);

protected:
    void SendMsgToMcu(u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0);
    void SendMsg2Mcu(u16 wEvent, CServMsg &cServMsg);
};

class CMpData
{
public:
    CMpData();
    virtual ~CMpData();

public:
    BOOL32 Init();
    BOOL32 CreateDS();
    void   DestroyDS();
    void   FreeStatusA(void);
    void   FreeStatusB(void);

    void   MpSendFilterParamInit( void );
    void   MpResetSendFilterParam( u8 byConfNo );
    BOOL32 GetWaitIFrameDataBySrc( u32 dwSrcIp, u16 wSrcPort, TWaitIFrameData *ptWaitData );

    BOOL32 IsPointT( u16 wPort );
    BOOL32 IsPointMt( u16 wPort );
    BOOL32 IsPointSrc( u16 wPort, u8 *pbyConfIdx = NULL, u8 *pbyPointId = NULL );
    BOOL32 IsPortSrcToT( u8 byConfIdx, u16 wRcvPort );
    
    void   SetChgDelay( u8 byConfIdx, u16 wDelayTick );
    u16    GetChgDelay( u8 byConfIdx );
    void   ClearChgDelay( u8 byConfIdx );

    void   SetConfUniMode( u8 byConfIdx, u8 byMode );
    u8     GetConfUniMode( u8 byConfIdx );

    BOOL32 IsPortNeedChkIFrm( u16 wRcvPort );
    BOOL32 IsTInUseStill( u8 byConfIdx );
    
    u8     GetPtIdle( u8 byConfIdx );
    u8     GetPtState( u8 byConfIdx, u8 byPoint, BOOL32 bAnother = FALSE );
    void   SetPtState( u8 byConfIdx, u8 byPoint, u8 byState, BOOL32 bAnother = FALSE );
    u16    GetChanNoByDst( u8 byConfIdx, u32 dwDstIp, u16 wDstPort, u8 byGetType, u16 wRcvPort = 0 );

    BOOL32 GetSwitchChan    ( u8 byConfIdx, u16 wIndex, TSwitchChannel *ptSwitchChannel );
    BOOL32 RemoveSwitchTable( u8 byConfIdx, TSwitchChannel *ptSwitchChannel, u8 byType, BOOL32 bStop = FALSE );
    BOOL32 RemoveSwitchTableSrc( u8 byConfIdx, TSwitchChannel *ptSwitchChannel );
    BOOL32 SearchSwitchTable( u8 byConfIdx, u32 dwDstIp, u16 wDstPort, TSwitchChannel &tSwitchChannel );
    BOOL32 IsSrcSwitchExist( u8 byConfIdx, TSwitchChannel *ptSrcSwitch );
    BOOL32 SearchBridgeSwitch( u8 byConfIdx, u16 wRcvDstPort, TSwitchChannel &tSwitchChannel );

    void   NtfMcuSrcSwitchChg( u8 byConfIdx, TSwitchChannel &tSwitchSrc );
    void   ProceedSrcChange( u8 byConfIdx, u8 byPID, BOOL32 bAnotherPt = TRUE );

    //void   ShowSwitch();
    void   ShowWholeSwitch();
    void   ShowSwitchUniformInfo();
    BOOL32 AddSwitchTable( u8 byConfIdx, TSwitchChannel* ptSwitchChannel, u8 byType );
    void   ClearSwitchTable( BOOL32 bClearAll, u8 byConfIdx = 0 );

    u8     GetMtNumNeedT( u8 byConfIdx );
    void   SetMtNumNeedT( u8 byConfIdx, u8 byMtNum );

    void   SendMsg2Mcu(u16 wEvent, CServMsg &cServMsg);
    
    void   ShowHandle( void );


public:
    s32  m_nInitUdpHandle;

    u32	 m_dwMcuNode;
    u32	 m_dwMcuIId;
    u32  m_dwMcuNodeB;
    u32  m_dwMcuIIdB;
    u32  m_dwMcuIpAddr;
    u16  m_wMcuPort;
    u32  m_dwMcuIpAddrB;
    u16  m_wMcuPortB;
    u16  m_wRecvMtDataPort;
    u16  m_wRecvMtDataPortB;

    BOOL32 m_bEmbedA;
    BOOL32 m_bEmbedB;

    u32  m_dwMpcSSrc;           // guzh [6/12/2007] 业务侧会话校验值    

    u8   m_byAttachMode;        //MP的附属方式 ( 1-MCU,2-混音器,3-码流适配器,255-独立 )
    u8   m_byMpId;              //本Mp编号
    u8   m_byMcuId;             //要连接的Mcu的ID
    u32  m_dwIpAddr;            //Ip地址数组
	s8   m_abyMpAlias[16];      //Mp别名

    u8   m_byRegAckNum;         // 是否收到注册Ack消息
    u8   m_byConnectNum;        // 连接成功的次数    
    BOOL32 m_bIsRestrictFlux;   // 是否限制转发流量
    BOOL32 m_bIsSendOverrunNtf; // 是否允许发送转发能力超限通知
    BOOL32 m_bIsWaitingIFrame;  // 是否等待关键帧

    BOOL32 m_bCancelWaitIFrameNoUni;

    //归一重整参数信息保存表
    TSendFilterParam   (*m_pptFilterParam)[MAX_SWITCH_CHANNEL];
    TSendFilterAppData (*m_pptFilterData)[MAX_SWITCH_CHANNEL];
    TWaitIFrameData    (*m_pptWaitIFrmData)[MAXNUM_CONF_MT+POINT_NUM]; //193和194留给中转交换的归一重整
    TSwitchChannel     (*m_pptSwitchChannel)[MAX_SWITCH_CHANNEL];

private:
    u8        m_abyConfMode[MAX_CONFIDX];       // 会议归一重整模式
    u16       m_awChgDelayTick[MAX_CONFIDX];    // 切换前等待的时长
    u8        m_byMtNumNeedT[MAX_CONFIDX];      // 需要T节点的终端数
    u8        m_abyPtState[MAX_CONFIDX][POINT_NUM];  // 会议码流切换中转点信息
    SEMHANDLE m_hPtInfo;        // 中转节点状态写入信号量
    SEMHANDLE m_hPtTick;        // 会议切换等待超时时间写入信号量
    SEMHANDLE m_hMtNum;         // 转发终端数写入信号量
};

typedef zTemplate< CMpInst, 1, CMpData > CMpApp;
extern CMpApp g_cMpApp;


class CSemOpt{
public:
    CSemOpt( SEMHANDLE& hSem )
    {
        m_hSem = hSem;
        if( OspSemTakeByTime( m_hSem, WAIT_SEM_TIMEOUT ) != TRUE )
        {
            OspPrintf( TRUE, FALSE, "[Mp]: semTake error accord to handle<0x%x>\n", hSem );
        }
    }
    ~CSemOpt( )
    {
        OspSemGive( m_hSem );
    }
private:
    SEMHANDLE m_hSem;
};


/*****************************************************************************
   类名        : CMpBitStreamFilter
   实现功能    : 码流过滤检测(功能类，通过静态方法提供接口)
   作者        : 顾振华
   版本        : V4.0  Copyright(C) 2001-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2007/04/20  4.0         顾振华        创建
******************************************************************************/

/*
// RTP 包结构定义
typedef struct
{
    u8	 m_byMark;			//是否帧边界1表示最后一包
    u8   m_byExtence;		//是否有扩展信息
    u8   m_byPadNum;		//可能的padding长度
    u8   m_byPayload;		//载荷
    u32  m_dwSSRC;			//同步源
    u16  m_wSequence;		//序列号
    u32  m_dwTimeStamp;		//时间戳
    u8  *m_pExData;         //扩展数据
    s32  m_nExSize;			//扩展大小：sizeof(u32)的倍数；
    u8  *m_pRealData;		//媒体数据 
    s32  m_nRealSize;		//数据大小 
    s32  m_nPreBufSize;		//m_pRealData前预分配的空间;
}TRtpPack;
*/

//H261头结构，为了便于理解，字段名和标准一致
typedef struct
{
    s32 sBit;
    s32 eBit;
    s32 i;
    s32 v;
    s32 gobN;
    s32 mbaP;
    s32 quant;
    s32 hMvd;
    s32 vMvd;
}TH261Header;

//H264头结构
typedef struct
{
    u16		 m_wWidth;			// 编码帧的宽度
    u16		 m_wHeight;			// 编码帧的高度
    BOOL32	 m_bIsKeyFrame;		// 编码帧是否为关键帧： 1：是关键帧  0：不是关键帧
    u32		 m_dwSPSId;			// SPS的ID
    BOOL32	 m_bIsValidSPS;		// H264的SPS是否有效
    BOOL32	 m_bIsValidPPS;		// H264的PPS是否有效
} TKdvH264Header;

//h264 码流信息分析
typedef struct
{
    u8 *pu8Start;
    u8 *pu8P;
    u8 *pu8End;
    s32 s32Left;        // i_count number of available bits 
    
} TBitStream; 

typedef enum 
{
    P_SLICE = 0,
    B_SLICE,
    I_SLICE,
    SP_SLICE,
    SI_SLICE
} stdh264SliceType;


#define MAXnum_ref_frames_in_pic_order_cnt_cycle  255
#define MAXIMUMVALUEOFcpb_cnt                     32
#define MAXnum_slice_groups_minus1                8
#define MPEG4_FRAMEMODE_POS                       2

typedef struct tagSeqParameterSetRBSP
{
    BOOL32   bIsValid;												// indicates the parameter set is valid
    
    u32	profile_idc;										        // u(8)
    //#ifdef G50_SPS
    BOOL32		constrained_set0_flag;								// u(1)
    BOOL32		constrained_set1_flag;								// u(1)
    BOOL32		constrained_set2_flag;								// u(1)
    //#endif
    //uint_8  reserved_zero_5bits; /*equal to 0*/					// u(5)
    u32			level_idc;											// u(8)
    //#ifndef G50_SPS
    //	BOOL32		more_than_one_slice_group_allowed_flag;			// u(1)
    //	BOOL32		arbitrary_slice_order_allowed_flag;				// u(1)
    //	BOOL32		redundant_slices_allowed_flag;					// u(1)
    //#endif
    u32			seq_parameter_set_id;								// ue(v)
    u32			log2_max_frame_num_minus4;							// ue(v)
    u32			pic_order_cnt_type;									// ue(v)
    // if( pic_order_cnt_type == 0 ) 
    u32			log2_max_pic_order_cnt_lsb_minus4;					// ue(v)
    // else if( pic_order_cnt_type == 1 )
    BOOL32		delta_pic_order_always_zero_flag;					// u(1)
    s32			offset_for_non_ref_pic;								// se(v)
    s32			offset_for_top_to_bottom_field;						// se(v)
    u32			num_ref_frames_in_pic_order_cnt_cycle;				// ue(v)
    // for( i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++ )
    s32			offset_for_ref_frame[MAXnum_ref_frames_in_pic_order_cnt_cycle];   // se(v)
    u32			num_ref_frames;										// ue(v)
    BOOL32		gaps_in_frame_num_value_allowed_flag;				// u(1)
    u32			pic_width_in_mbs_minus1;							// ue(v)
    u32			pic_height_in_map_units_minus1;						// ue(v)
    BOOL32		frame_mbs_only_flag;								// u(1)
    // if( !frame_mbs_only_flag ) 
    BOOL32		mb_adaptive_frame_field_flag;						// u(1)
    BOOL32		direct_8x8_inference_flag;							// u(1)
    //#ifdef G50_SPS
    BOOL32		frame_cropping_flag;								// u(1)
    u32			frame_cropping_rect_left_offset;					// ue(v)
    u32			frame_cropping_rect_right_offset;					// ue(v)
    u32			frame_cropping_rect_top_offset;						// ue(v)
    u32			frame_cropping_rect_bottom_offset;					// ue(v)
    //#endif
    BOOL32		vui_parameters_present_flag;						// u(1)
    //not of syntax
    s32			MaxFrameNum;
} TSeqParameterSetRBSP;


typedef struct  tagPicParameterSetRBSP
{
    BOOL32		bIsValid;											// indicates the parameter set is valid
    u32			pic_parameter_set_id;                               // ue(v)
    u32			seq_parameter_set_id;                               // ue(v)
    BOOL32		entropy_coding_mode_flag;                           // u(1)
    // if( pic_order_cnt_type < 2 )  in the sequence parameter set
    BOOL32      pic_order_present_flag;                             // u(1)
    u32			num_slice_groups_minus1;                            // ue(v)
    u32			slice_group_map_type;								// ue(v)
    // if( slice_group_map_type = = 0 )
    u32			run_length_minus1[MAXnum_slice_groups_minus1];  	// ue(v)
    // else if( slice_group_map_type = = 2 )
    u32			top_left[MAXnum_slice_groups_minus1];				// ue(v)
    u32			bottom_right[MAXnum_slice_groups_minus1];			// ue(v)
    // else if( slice_group_map_type = = 3 || 4 || 5
    BOOL32		slice_group_change_direction_flag;				    // u(1)
    u32			slice_group_change_rate_minus1;					    // ue(v)
    // else if( slice_group_map_type = = 6 )
    u32			num_slice_group_map_units_minus1;					// ue(v)
    u32			*slice_group_id;									// complete MBAmap u(v)
    u32			num_ref_idx_l0_active_minus1;                       // ue(v)
    u32			num_ref_idx_l1_active_minus1;                       // ue(v)
    BOOL32		weighted_pred_flag;                                 // u(1)
    BOOL32		weighted_bipred_idc;                                // u(2)
    s32			pic_init_qp_minus26;                                // se(v)
    s32			pic_init_qs_minus26;                                // se(v)
    s32			chroma_qp_index_offset;                             // se(v)
    BOOL32		deblocking_filter_control_present_flag;             // u(1)
    BOOL32		constrained_intra_pred_flag;                        // u(1)
    BOOL32		redundant_pic_cnt_present_flag;                     // u(1)
    BOOL32		vui_pic_parameters_flag;                            // u(1)
    //#ifndef G50_SPS
    //	BOOL32   frame_cropping_flag;                               // u(1)
    //    u32  frame_cropping_rect_left_offset;					    // ue(v)
    //    u32  frame_cropping_rect_right_offset;					// ue(v)
    //    u32  frame_cropping_rect_top_offset;					    // ue(v)
    //    u32  frame_cropping_rect_bottom_offset;				    // ue(v)
    //#endif
} TPicParameterSetRBSP;

typedef struct
{
    u16		first_mb_in_slice;						//ue(v)  slice中第一个MB的地址
    u16		slice_type;								//ue(v)  slice的编码类型
    u16		pic_parameters_set_id;	
} Tstdh264Dec_SliceHeaderData;

class CMpBitStreamFilter
{
public:
    // 关键帧检测
    static BOOL32 IsKeyFrame( u8 byPayLoad, u8 *pBuf, s32 nSize );

private:    
    // Toolkits and utilities    
    // u32网络序主机序转化
    static void ConvertN2H(u8 *pBuf, s32 nStartIndex, s32 nSize);
    
    // u32获取指定bits range
    static u32  GetBitField(u32 dwValue, s32 nStartBit, s32 nBits);

    static BOOL32 IsH261KeyFrame( u8 *pBuf, s32 nSize );
    static BOOL32 IsH263KeyFrame( u8 *pBuf, s32 nSize );
    static BOOL32 IsH264KeyFrame( u8 *pBuf, s32 nSize );
    static BOOL32 IsMpeg4KeyFrame( const TRtpPack *ptRtpPack );   // MPEG4 & H262
    
private:
    static void stdh264_bs_init( TBitStream *s, void *p_data, s32 i_data );
    static s32 stdh264_bs_pos( TBitStream *s );
    static s32 stdh264_bs_eof( TBitStream *s );
    static u32 stdh264_bs_read( TBitStream *s, s32 i_count );
    static u32 stdh264_bs_read1( TBitStream *s );
    static u32 stdh264_bs_show( TBitStream *s, s32 i_count );
    static void stdh264_bs_skip( TBitStream *s, s32 i_count );
    static s32 stdh264_bs_read_ue( TBitStream *s );
    static s32 stdh264_bs_read_se( TBitStream *s );
    static s32 stdh264_bs_read_te( TBitStream *s, s32 x );

    static s32 stdh264_FirstPartOfSliceHeader(TBitStream *s, Tstdh264Dec_SliceHeaderData *dec_slice_header);

    static BOOL32 DecodeH264SPS( TBitStream *s, TSeqParameterSetRBSP *sps, 
                                 TKdvH264Header *pStdH264Header );
    
    static BOOL32 DecodeH264PPS( TBitStream *s, TPicParameterSetRBSP *pps, 
							     TKdvH264Header *pStdH264Header );
};


//是否冗余 全局记录类
struct TPackBuf
{
public:
    TPackBuf();
    void AddPack(u32 dwSSRC, u16 wSequence);
    BOOL IsPackExist(u32 dwSSRC, u16 wSequence);
    void Clear();
private:
    u32 m_dwSSRC[MAXNUM_HISPACKS];
    u16 m_wSequence[MAXNUM_HISPACKS];
};


class CPackDefer
{

};

#define	MAXNUM_NET_SW					(u8)10
#define	MAX_FRAME_SIZE					(u32)512 * 1024
#define NET_SW_NETBAND					(u32)8 * 1024 * 1024


//Medianet收发中继类
class CNetSwitch
{
public:
	CKdvMediaRcv m_cMediaRcv;
	CKdvMediaSnd m_cMediaSnd;
	TSwitchChannel m_tSWChn;
	u8 m_bySndPT;

public:
	void Init(TSwitchChannel tSwitchChn, u8 bySndPT);

	void Quit();

	BOOL32 IsNull();

	void SetNull();
};

//最大支持十组中继收发
class CNetSWGrp
{
public:
	//CNetSWGrp::CNetSWGrp();

	u8 GetIdleNetSW();

	void InitNetSW(u8 byIdx, TSwitchChannel tSwitchChn, u8 bySndPT);

	void QuitNetSW(u8 byIdx);

	void QuitNetSW(TSwitchChannel tSwitchChn, u8 bySndPT);

public:
	CNetSwitch m_atNetSW[MAXNUM_NET_SW];

};



// 全局接口
s8*   strofip(u32 dwIp);
void  strofip(u32 dwIp, s8* StrIp);
void  MpAPIEnableInLinux( void );
void  MpLog( u8 byLvl, s8 * pszFmt, ... );

void   MpSendFilterCallBackProc( u32 dwRecvIP, u16 wRecvPort,
                                 u32 dwSrcIP,  u16 wSrcPort,
                                 TNetSndMember * ptSends,      // 转发目标列表
                                 u16 * pwSendNum,              // 转发目标个数
                                 u8  * pUdpData, u32 dwUdpLen );

#endif  // !__DMP_H_

// END OF FILE
