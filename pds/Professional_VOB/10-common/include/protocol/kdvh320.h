/*****************************************************************************
模块名      : Kdv H320
文件名      : KdvH320.h
相关文件    : 
文件实现功能: h.320协议栈接口头文件
作者        : 秦重军
版本        : V1.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2004/04/06  1.0         秦重军      创建
2004/11/08  1.0         万春雷      框架重构，代码优化整理
******************************************************************************/

#ifndef _KDVH320_H
#define _KDVH320_H

#include "kdvtype.h"
#include "kdvh320def.h"

#ifdef WIN32
#pragma pack(1)
#define H320PACKED 
#else
#define H320PACKED __attribute__((__packed__))	
#endif // WIN32

//协议栈状态，用于自动上报
typedef struct tagKDVH320State
{
	u16 m_wState;		//状态种类	
	u16 m_wLength;		//内容长度
    u8 *m_pbyContent;	//状态内容

}H320PACKED TKDVH320State;

//复用模式
typedef struct tagKDVH320MuxMode
{
	u8  m_byTransRate;
	u8  m_byAudType;
	u8  m_byVidType;
	u8  m_byLSDType;
	u8  m_byHSDType;
	u8  m_byMLPType;
	u8  m_byHMLPType;
	u8  m_byEncrypt;

}H320PACKED TKDVH320MuxMode;

//e1配置信息
typedef struct tagKDVH320E1Info
{
	u32 m_dwTsMask;						//时隙掩码
	u8  m_byChanId;						//E1通道id（对应E1槽号）

}H320PACKED TKDVH320E1Info;

//能力集
typedef struct tagTKdvH320Cap
{
public:
    BOOL32 IsExistCap(u32 dwCap);   
    BOOL32   AddCap(u32 dwCap);

public:
	u32 m_adwCapList[simplecap_max_num];
	u8  m_byActualNum;
    
}H320PACKED TKdvH320Cap;

// xsl [8/8/2005] 增加能力集处理方法
enum KDVH320_CAPTYPE
{
    CAPTYPE_TRANSRATE = 1,
    CAPTYPE_AUDIO,
    CAPTYPE_VIDEO,
    CAPTYPE_LSD,
    CAPTYPE_HSD,
    CAPTYPE_MLP,
    CAPTYPE_HMLP
};

typedef struct tagTKdvH320CapSet
{
public:    
    void ClearAll();

    BOOL32 AddTransCap(u32 dwCap);
    BOOL32 AddVidCap(u32 dwCap, u32 dwMpi);
    BOOL32 AddAudCap(u32 dwCap);
    BOOL32 AddLsdCap(u32 dwCap);
    BOOL32 AddHsdCap(u32 dwCap);
    BOOL32 AddMlpCap(u32 dwCap);
    BOOL32 AddHmlpCap(u32 dwCap);
    
    BOOL32 IsExistCap(u32 dwCap, u8 byCapType);   

public:
	TKdvH320Cap m_tTransRateCap;    //传输能力集
	TKdvH320Cap m_tAudioCap;        //音频能力集
	TKdvH320Cap m_tVideoCap;        //视频能力集
	TKdvH320Cap m_tLSDCap;          //lsd
	TKdvH320Cap m_tHSDCap;          //hsd
	TKdvH320Cap m_tMLPCap;          //mlp
	TKdvH320Cap m_tHMLPCap;         //h-mlp
	
	u8   m_byMBECap;                //是否支持mbe指令的处理
	u8   m_byH224LsdCap;            //H.224 Lsd，用于fecc
	u8   m_byT120Cap;               //是否支持t.120
	
	u8   m_byEncryptCap;            //是否支持Encrypt
	u8   m_byV120LsdCap;            //是否支持V.120Lsd
	u8   m_byV120HsdCap;            //V.120 hsd
	u8   m_byV14LsdCap;             //V.14 lsd
	u8   m_byV14HsdCap;             //V.14 hsd
	u8   m_byH224MlpCap;            //H.224 Mlp
	u8   m_byH224HsdCap;            //H.224 Hsd
	u8   m_byH224SimCap;            //H.224 sim
	u8   m_byNilDataCap;            //Nil-Data
	u8   m_byH224TokenCap;          //H.224 Token

}H320PACKED TKdvH320CapSet;

typedef struct tagH320IIS
{
    u8  m_byIISType;                //define in kdvh320def.h
    s8  m_szAlias[MAXLEN_EPALIAS];  //alias
    
}H320PACKED TH320IIS;

typedef struct tagH320TIL
{
    u8  m_byMcuId;                  //mcu id
    u8  m_abyMtId[MAXNUM_MT];       //mt id array
    u8  m_byMtNum;                  //mt count

}H320PACKED TH320TIL;

typedef struct tagH320Mt
{
    u8  m_byMcuId;                  //mcu id
    u8  m_byMtId;                   //mt id    

}H320PACKED TH320Mt;

typedef struct tagH320MtExt
{
    TH320Mt m_tMt;
    s8      m_szAlias[MAXLEN_EPALIAS];  //alias

}H320PACKED TH320MtExt;

typedef struct tagH320TIR
{
    TH320Mt m_tLsdTokenMt;
    TH320Mt m_tHsdTokenMt;
    TH320Mt m_tChairTokenMt;

}H320PACKED TH320TIR;

//协议栈统计信息，用于协议栈调用者查询
typedef struct tagKDVH320Statistics
{
	//capability set
	TKdvH320CapSet  m_tLocalCapSet;
	TKdvH320CapSet  m_tRemoteCapSet;
	TKdvH320CapSet  m_tCommCapSet;
	
	//multiplex&demultiplex mode
	TKDVH320MuxMode m_tLocalMuxMode;
	TKDVH320MuxMode m_tRemoteMuxMode;
	
	u8              m_byConnectted;  //connectted?	
	u8              m_byInConf;      //in conference?
	u32             m_dwMaxVidRate;  //maxium video rate can send
}H320PACKED TKDVH320Statistics;

//Audio/Video... 数据接收后的通知回调
typedef void (*H320DataRcvCB)( u8 *pbyBuff, u32 dwLen, u32 dwContext );

//FECC回调
typedef void (*H320FECCProcCB)( u8 byAction, u8 byParam1, u8 byParam2, TH320Mt tDstMt, TH320Mt tSrcMt, u32 dwContext );

//状态变化回调
typedef void (*H320StateProcCB)( TKDVH320State tState, u32 dwContext );

//指令回调
typedef void (*H320CmdProcCB)( u16 wCmdId, u8 *pbyParam, u16 wParamNum, u32 dwContext );

//参数保存
class CKdvH320Stack;

//协议栈
class CKdvH320
{
public:
	CKdvH320();
	virtual ~CKdvH320();

	//启动协议栈
	u16 StartUp();
	//关闭协议栈
	u16 Shutdown();

	//设置本端能力集
	u16 SetLocalCapSet( TKdvH320CapSet &tLocalCapSet );

    //设置能力集优先级, 按顺序由高到低，没有设置的采用默认优先级
    u16 SetPriCapset(TKdvH320CapSet &tPriCapset);    

	//注册回调函数	
	u16 RegAudioProcCB( H320DataRcvCB lpCB, u32 dwContext ); //Audio	
	u16 RegVideoProcCB( H320DataRcvCB lpCB, u32 dwContext ); //Video	
	u16 RegHSDProcCB( H320DataRcvCB lpCB, u32 dwContext );   //HSD
	u16 RegCIProcCB( H320CmdProcCB lpCB, u32 dwContext );    //C&I	
	u16 RegFECCProcCB( H320FECCProcCB lpCB, u32 dwContext ); //FECC	
	u16 RegStateChangeProcCB( H320StateProcCB lpCB, u32 dwContext ); //State Change
		
	//呼叫开始
	u16 CallStart( TKDVH320E1Info &tE1Info, u8 byWorkMode = callmode_unicast);
	//呼叫结束
	u16 CallEnd();    
	
	//远程摄像头控制	
	u16 StartFECC(); //开启fecc操作	
	u16 StopFECC();  //停止fecc操作	
	u16 StartCamAction( u8 byMcuId, u8 byMtId, u8 byAction, u8 byTimeOut );//摄像头动作开始	
	u16 ContinueCamAction( u8 byMcuId, u8 byMtId, u8 byAction );  //继续指定的摄像头动作	
	u16 StopCamAction( u8 byMcuId, u8 byMtId, u8 byAction );      //停止指定的摄像头动作	
	u16 SelectVideoSource( u8 byMcuId, u8 byMtId, u8 byVidSrcNum,  
                           u8 byMode = vidmode_motion );          //选择视频源	
	u16 StorePreset( u8 byMcuId, u8 byMtId, u8 byPresetNum );     //预存位保存
	u16 ActivatePreset( u8 byMcuId, u8 byMtId, u8 byPresetNum );  //预存位激活
	
	//h.230 C&I 指令发送
	u16 SendCommand( u16 wCmdInd, u8 *pbyParams, u8 byParamNum );
	
	//Audio/Video... 数据发送
	u16 SendData( u8 *pbyDataBuf, u32 dwDataLen, u8 byDataType );

    u16 SetVidLoopback(BOOL32 bLoop);
    u8 GetPeriE1Num();

	//查询协议栈状态
	u16 GetStatistics( TKDVH320Statistics &tStatistics );    
   
private:
	CKdvH320Stack *m_pcH320Stack;
	
};

API void h320stackver();                //ver

//设置打印级别
API void h320stacksetlog(u8 byLevel);

//help
API void h320stackhelp();

API void sendaudiodelaytime(s32 nTime); //send
API void setaudiodelaytime(s32 nTime);  //receive

API void h320ss(u8 byChanId);

#if defined(_VXWORKS_) || defined(_LINUX_)
API void pe1Chaninfo(u8 byChanId);
#endif

#endif ////


//----------------------------------------------------------------------

//end of file





