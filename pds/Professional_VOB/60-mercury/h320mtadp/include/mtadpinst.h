/*****************************************************************************
模块名      : h320终端适配模块
文件名      : MtAdpInst.h
相关文件    : MtAdpInst.cpp
文件实现功能: 实例模块
作者        : 许世林
版本        : V3.6  Copyright(C) 1997-2003 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2005/06/07  3.6         许世林      创建
******************************************************************************/

#ifndef __MTADPINST_H
#define __MTADPINST_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mtadpcom.h"

class CMtAdpNet;
class CMtAdpInst : public CInstance 
{
    enum 
    {
        STATE_IDLE = 0,                     //空闲
        STATE_CALLING,                      //终端正在入会
        STATE_NORMAL                        //终端成功入会
    };

public:
	CMtAdpInst();
	virtual ~CMtAdpInst();

//////////////////////////////////////////////////////////////////////////
    //实例入口
    void InstanceEntry(CMessage *const pcMsg);

    //mcu邀请终端入会请求处理函数
    void OnMcuInviteMtReq(const CMessage *pcMsg);    

    //mcu邀请终端入会超时处理函数
    void OnInviteMtTimeout(const CMessage *pcMsg);

    //协议栈状态处理
    void OnH320StackState(const CMessage *pcMsg);

    //协议栈远遥处理
    void OnH320StackFecc(const CMessage *pcMsg);

    //协议栈cmd处理
    void OnH320StackCmd(const CMessage *pcMsg); 

    //收到本端复用模式处理
    void OnLocalMuxMode(const TKDVH320MuxMode *ptMuxMode);

    //mcu 打开逻辑信道操作响应
    void OnMcuMtOpenLogicChanRsp(const CMessage *pcMsg);

    //mcu 打开逻辑信道请求
    void OnMcuMtOpenLogicChanReq(const CMessage *pcMsg);

    //mcu 开启信道处理
    void OnMcuMtChanOnCmd(const CMessage *pcMsg);

    //mcu 停止信道处理
    void OnMcuMtChanOffCmd(const CMessage *pcMsg);

    //mcu 关闭信道处理
    void OnMcuMtCloseChanCmd(const CMessage *pcMsg);

    //flow control
    void OnFlowCtrlCmd(const CMessage *pcMsg);

    //告知选看终端源
    void OnYouAreSeeingNotif(const CMessage *pcMsg);

    void OnGetBitrateInfo(const CMessage *pcMsg);

    //强制关键帧
    void OnMcuMtFastUpdatePicCmd(const CMessage *pcMsg);

    //指定/取消主席
    void OnMcuMtChairNotif(const CMessage *pcMsg);
    
    //指定/取消发言
    void OnMcuMtSpeakerNotif(const CMessage *pcMsg);

    //指定/取消选看
    void OnMcuMtSelNotif(const CMessage *pcMsg);
    //选看响应处理
    void OnMcuMtSelRsp(const CMessage *pcMsg);

    //静音/哑音
    void OnMcuMtMuteDumbCmd(const CMessage *pcMsg);

    //终端列表响应
    void OnMcuMtMtListRsp(const CMessage *pcMsg);

    //终端别名响应
    void OnMcuMtGetMtAliasRsp(const CMessage *pcMsg);

    //终端入会离会通知
    void OnMcuMtOnlineChangeNotif(const CMessage *pcMsg);

    void OnConfInfoNotif(const CMessage *pcMsg);

    //远遥操作
    void OnMcuMtFeccCmd(const CMessage *pcMsg);

    //sel or speaker
    void OnSetInReq(const CMessage *pcMsg);

    //assign tia
    void AssignTia(TMt &tMt);

    u8  GetRemoteMcuId();
    u8  GetLocalMcuId();    
   
    //send iis
    void OnSendCmdIIS();

    //release lsd token
    void OnReleaseLsdToken();

    //测试消息处理
    void OnMcuMtPrivateMsg(const CMessage *pcMsg);

    //交换信息打印
    void OnPrintSwitchInfo(const CMessage *pcMsg);
   
//////////////////////////////////////////////////////////////////////////    
    //Daemon 实例入口
    void DaemonInstanceEntry(CMessage *const pcMsg, CApp *pcApp);

    //上电处理
    void OnDaemonPowerOn();
    //适配模块向mcu注册及相应处理函数
    void OnDaemonRegisterMcuReq(const CMessage *pcMsg, BOOL32 bMcuA);
    void OnDaemonRegisterMcuRsp(const CMessage *pcMsg);

    //断链处理
    void OnDaemonOspDisconnect(const CMessage *pcMsg, CApp *pcApp);

    //请求主备状态回应
    void DaemonProcGetMsStatusRsp(const CMessage *pcMsg, CApp *pcApp);

    //mcu邀请终端Daemon处理
    void OnDaemonMcuInviteMtReq(const CMessage *pcMsg);

    void OnDaemonDelMtCmd(const CMessage *pcMsg, CApp *pcApp);

    //负责mcu到mt部分消息的实例分发
    void OnDaemonMcuMtGeneral(const CMessage *pcMsg, CApp *pcApp);

    //分发消息到所有与会终端
    void OnDaemonBroadMsg2AllMt(const CMessage *pcMsg);

//////////////////////////////////////////////////////////////////////////
    //清空实例
    void ClearInst();

    //发消息到mcu
    void SendMsg2Mcu(CServMsg &cServMsg);

    //为此实例对应的终端分配码流收发起始端口
    u16 AssignSwitchPort();    

    //加载配置文件信息
    void LoadConfig();

    //调试信息打印输出
    void MtAdpLog(u8 byLevel, s8 * pInfo, ...);

//////////////////////////////////////////////////////////////////////////
    //H320 cmd dealing
    void OnCmdCIS();
    void OnCmdCCA();
    void OnCmdIIS(u8 *pbyParam, u16 wParamLen);
    void OnCmdTIA(u8 *pbyParam, u16 wParamLen);
    void OnCmdMCC(u8 *pbyParam, u16 wParamLen);
    void OnCmdTIN(u8 *pbyParam, u16 wParamLen);
    void OnCmdTID(u8 *pbyParam, u16 wParamLen);
    void OnCmdTIL(u8 *pbyParam, u16 wParamLen);
    void OnCmdTIP(u8 *pbyParam, u16 wParamLen);
    void OnCmdTCP(u8 *pbyParam, u16 wParamLen);
    void OnCmdTIR(u8 *pbyParam, u16 wParamLen);
    void OnCmdVCB(u8 *pbyParam, u16 wParamLen);
    void OnCmdCancelVCB();
    void OnCmdVCS(u8 *pbyParam, u16 wParamLen);
    void OnCmdCancelVCS();
    void OnCmdDCAL();
    void OnCmdDISL();
    void OnCmdVCU();
    void OnCmdVIN(u8 *pbyParam, u16 wParamLen);
    void OnCmdTCU();

public:

    CKdvH320          * m_pcH320Stack;                  //h320 stack
    CMtAdpNet         * m_pcMtAdpNet;                   //net rcv & snd

    TMtAlias            m_tMtAlias;                     //mt alias
    u8                  m_byMtId;                       //mt id
    u8                  m_byConfIdx;                    //conf idx
    CConfId             m_cConfId;                      //conf id
    TMtAlias            m_tConfName;                    //conf name

    TKDVH320MuxMode     m_tLocalMuxMode;                //本端复用模式
    TKDVH320MuxMode     m_tRemoteMuxMode;               //远端复用模式
    TKdvH320CapSet      m_tLocalCapSet;                 //本端能力集

    u16                 m_wVideoRate;                   //视频码率(kbps)
    u16                 m_wConfRate;                    //时隙掩码

    //AUD_RCV_INDEX-音频收，VID_RCV_INDEX-视频收
    //AUD_SND_INDEX-音频发，VID_SND_INDEX-视频发
    TChannel            m_atChannel[MAXNUM_CHAN];       //逻辑信道

    u8                  m_byLocalMcuId;                 //master mcu 分配给slave mcu的mcu id
    BOOL32              m_bRemoteTypeMcu;               //对端是否mcu(0-mt,1-mcu)
    //master or slave (load from config file) 0-slave, 1-master, only used when remote is mcu
    BOOL32              m_bMasterMcu;
    BOOL32              m_bChairMt;                     //是否主席终端
    BOOL32              m_bAssignMtId;                  //是否已经分配终端id
    TMt                 m_tSpySrcMt;                    //当前回传的终端源
    TMt                 m_tVinMt;                       //vin mt    
    TConfMtInfo         m_tSMcuMtList;                  //对端从mcu上终端列表
    TH320TIR            m_tConfTir;                     //lsd, hsd, chair
    BOOL32              m_bHasLsdToken;                 //是否拥有lsd令牌
    u16                 m_wLsdTokenHoldTime;         //远遥令牌释放超时时间
    BOOL32              m_bReceivedIIS;                 //是否已经收到IIS

    BOOL32              m_bBchVidLoopback;

};

class CMtAdpData
{
public:
    CMtAdpData();
    ~CMtAdpData();

public:
    u32     m_dwMcuANode;                        //mcu A 节点号
    u32     m_dwMcuAIId;                         //mcu A 实例id
    u8      m_byMcuAId;                          //mcu A Id

    // 顾振华 [6/9/2006] 双链
    u32     m_dwMcuBNode;                        //mcu B 节点号
    u32     m_dwMcuBIId;                         //mcu B 实例id
    u8      m_byMcuBId;                          //mcu B Id

    BOOL32  m_bDoubleLink;                       // Mcu Linking mode ( 1-DoubleLink )

    u32     m_dwLocalIP;                        //本地IP地址(net order)
    u8      m_byDriId;                          //Dri板id
    s8      m_achMtAdpAlias[32];                //别名

    //实例号（通道）与终端（confid、mtid）映射关系
    u8      m_abyMt2InsMap[MAXNUM_MCU_CONF+1][MAXNUM_CONF_MT+1];
    
    u8      m_byLogLevel;                       //调试信息打印级别

};

typedef zTemplate< CMtAdpInst, MAXNUM_CHAN, CMtAdpData > CMtAdpApp;

extern  CMtAdpApp   g_cMtAdpApp;

#endif // __MTADPINST_H
