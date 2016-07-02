/*****************************************************************************
模块名      : h320终端适配模块
文件名      : MtAdpInst.cpp
相关文件    : MtAdpInst.h
文件实现功能: 实例模块
作者        : 许世林
版本        : V3.6  Copyright(C) 1997-2003 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2005/06/07  3.6         许世林      创建
******************************************************************************/

#include "mtadpinst.h"
#include "evmcumt.h"
#include "boardagent.h"
#include "mtadpnet.h"

//lsd通道类型，默认打开6400
u8  g_byLsdType = 1;

//是否发送终端列表，默认发送
u8  g_bySndMtList = 1;

/*=============================================================================
  函 数 名： ProcAudNetData
  功    能： 音频数据回调函数
  算法实现： 调用net库将音频数据发送到mcu
  全局变量： 
  参    数：  u8 *pbyBuf        音频数据指针
             u32 dwLen          音频数据长度
             u32 dwContext      
  返 回 值： void  
=============================================================================*/
void ProcAudData( u8 *pbyBuf, u32 dwLen, u32 dwContext )
{    
    CMtAdpInst *pcIns = (CMtAdpInst *)dwContext;
    if(NULL == pcIns)
    {
        return;
    }
  
    pcIns->m_pcMtAdpNet->AudNetSnd(pbyBuf, dwLen);

    return;
}

/*=============================================================================
  函 数 名： ProcVidNetData
  功    能： 视频数据回调函数
  算法实现： 调用net库将视频数据发送到mcu
  全局变量： 
  参    数：  u8 *pbyBuf        视频数据指针
             u32 dwLen          视频数据长度
             u32 dwContext
  返 回 值： void 
=============================================================================*/
void ProcVidData( u8 *pbyBuf, u32 dwLen, u32 dwContext )
{
    CMtAdpInst *pcIns = (CMtAdpInst *)dwContext;
    if(NULL == pcIns)
    {
        return;
    }    
 
    if(pcIns->m_bBchVidLoopback)
    {
        u16 wRet = pcIns->m_pcH320Stack->SendData(pbyBuf, dwLen, dtype_video);
        if(KDVH320_OK != wRet)
        {
            pcIns->MtAdpLog(LOG_WARNING, "[ProcVidData] send video data failed in loop back. err no :%d\n", wRet);
        }
    }
    else
    {
        pcIns->m_pcMtAdpNet->VidNetSnd(pbyBuf, dwLen);
    }    

    return;
}

/*=============================================================================
  函 数 名： ProcHSDNetData
  功    能： Hsd数据回调函数
  算法实现： 
  全局变量： 
  参    数：  u8 *pbyBuf        Hsd数据指针
             u32 dwLen          Hsd数据长度
             u32 dwContext
  返 回 值： void 
=============================================================================*/
void ProcHSDData( u8 *pbyBuf, u32 dwLen, u32 dwContext )
{
    return;
}

/*=============================================================================
  函 数 名： ProcFECCNetData
  功    能： FECC数据回调函数
  算法实现： 
  全局变量： 
  参    数：  u8 byAction       远遥类型
             u8 byParam1        远遥参数1    
             u8 byParam2        远遥参数2
             u32 dwContext
  返 回 值： void  
=============================================================================*/
void ProcFECCData( u8 byAction, u8 byParam1, u8 byParam2, TH320Mt tDstMt, TH320Mt tSrcMt, u32 dwContext )
{    
    CMtAdpInst *pcIns = (CMtAdpInst *)dwContext;

    CServMsg cServMsg;
    cServMsg.SetMsgBody(&byAction, sizeof(byAction));
    cServMsg.CatMsgBody(&byParam1, sizeof(byParam1));
    cServMsg.CatMsgBody(&byParam2, sizeof(byParam2));
    cServMsg.CatMsgBody((u8*)&tDstMt, sizeof(tDstMt));
//    cServMsg.CatMsgBody((u8*)&tSrcMt, sizeof(tSrcMt));

    OspPost(MAKEIID(AID_MCU_MTADP, pcIns->GetInsID()), 
            EV_H320STACK_FECC_NOTIF,
            cServMsg.GetServMsg(),
            cServMsg.GetServMsgLen());

    return;
}

/*=============================================================================
  函 数 名： ProcCmdNetData
  功    能： C&I 命令回调处理函数
  算法实现： 
  全局变量： 
  参    数：  u16 wCmdId        命令id
             u8 *pbyParam       
             u16 wParamNum
             u32 dwContext
  返 回 值： void  
=============================================================================*/
void ProcCmdData( u16 wCmdId, u8 *pbyParam, u16 wParamNum, u32 dwContext )
{
    CMtAdpInst *pcIns = (CMtAdpInst *)dwContext;
    
    CServMsg cServMsg;
    cServMsg.SetMsgBody((u8 *)&wCmdId, sizeof(wCmdId));
    cServMsg.CatMsgBody(pbyParam, wParamNum);
    
    OspPost(MAKEIID(AID_MCU_MTADP, pcIns->GetInsID()), 
            EV_H320STACK_CMD_NOTIF,
            cServMsg.GetServMsg(),
            cServMsg.GetServMsgLen());

    return;
}

/*=============================================================================
  函 数 名： ProcH320State
  功    能： 协议栈状态回调处理函数
  算法实现： 
  全局变量： 
  参    数：  TKDVH320State tState      协议栈状态
             u32 dwContext
  返 回 值： void  
=============================================================================*/
void ProcH320State( TKDVH320State tState, u32 dwContext )
{
    CMtAdpInst *pcIns = (CMtAdpInst *)dwContext;
    if ( tState.m_pbyContent == NULL && tState.m_wLength != 0 )
    {
        // guzh [9/5/2006] 异常保护
        OspPrintf(TRUE, FALSE, "[DEBUG.0905][ProcH320State]tState.m_wState=%d, m_wLength=%d, m_pbyContent=NULL. IGNORED!%x\n", 
                tState.m_wState,
                tState.m_wLength);
        return;
    }

    CServMsg cServMsg;
    cServMsg.SetMsgBody((u8 *)&tState.m_wState, sizeof(tState.m_wState));  
    cServMsg.CatMsgBody(tState.m_pbyContent, tState.m_wLength);
    
    OspPost(MAKEIID(AID_MCU_MTADP, pcIns->GetInsID()), 
            EV_H320STACK_STATE_NOTIF,
            cServMsg.GetServMsg(),
            cServMsg.GetServMsgLen());

    return;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMtAdpInst::CMtAdpInst()
{
    memset(&m_tMtAlias, 0, sizeof(m_tMtAlias));             //mt alias
    m_byMtId = 0;                                           //mt id
    m_byConfIdx = 0;                                        //conf idx
    m_cConfId.SetNull();                                    //conf id
    
    memset(&m_tLocalMuxMode, 0, sizeof(m_tLocalMuxMode));   //本端复用模式
    memset(&m_tRemoteMuxMode, 0, sizeof(m_tRemoteMuxMode)); //远端复用模式

    memset(&m_tLocalCapSet, 0, sizeof(m_tLocalCapSet));

    m_wVideoRate = 0;                                      //video rate
    m_wConfRate = 0;

    for(s32 i = 0; i < MAXNUM_CHAN; i++)
    {
        m_atChannel[i].Reset();
    }

    //启动协议栈
    m_pcH320Stack = new CKdvH320;    
    m_pcH320Stack->StartUp();

    //初始码流收发对象
    m_pcMtAdpNet = new CMtAdpNet(this);

    m_bBchVidLoopback = FALSE;

    m_tVinMt.SetNull();
    m_bHasLsdToken = FALSE;
    m_bReceivedIIS = FALSE;
    m_wLsdTokenHoldTime = DEF_LSDTOKEN_RELEASE_TIME;
    m_byLocalMcuId = 0;
    m_bRemoteTypeMcu = FALSE;
    m_bMasterMcu = FALSE;   //default slave mcu, 来自于界面配置或配置文件
    m_bChairMt = FALSE;
    m_bAssignMtId = FALSE;
    m_tSpySrcMt.SetNull();
}

CMtAdpInst::~CMtAdpInst()
{
    ClearInst();

    //关闭协议栈
    m_pcH320Stack->Shutdown();

    SAFE_DELETE(m_pcH320Stack);
    SAFE_DELETE(m_pcMtAdpNet);
}

void CMtAdpInst::ClearInst()
{
    if(STATE_IDLE == CurState())
    {
        return;
    }

    NEXTSTATE(STATE_IDLE);    

    // mt drop
    if(!m_bRemoteTypeMcu)
    {
        u8 abyParam[2];
        abyParam[0] = GetRemoteMcuId();
        abyParam[1] = m_byMtId;
        m_pcH320Stack->SendCommand(KDVH320_CI_TID, abyParam, sizeof(abyParam));
        MtAdpLog(LOG_INFO, "mt :%d dropped!\n", m_byMtId);

        OspTaskDelay(500);
    }       
    
    //结束呼叫
    m_pcH320Stack->CallEnd();

    //映射关系
    g_cMtAdpApp.m_abyMt2InsMap[m_byConfIdx][m_byMtId] = 0;

    //终端信息
    memset(&m_tMtAlias, 0, sizeof(m_tMtAlias));
    m_byMtId = 0;
    m_byConfIdx = 0;
    m_cConfId.SetNull();

    //本端复用模式
    memset(&m_tLocalMuxMode, 0, sizeof(m_tLocalMuxMode));
    //远端复用模式
    memset(&m_tRemoteMuxMode, 0, sizeof(m_tRemoteMuxMode));

    //本端能力集
    memset(&m_tLocalCapSet, 0, sizeof(m_tLocalCapSet));
 
    //video rate
    m_wVideoRate = 0;  
    m_wConfRate = 0;

    //释放音视频信道
    for(s32 i = 0; i < MAXNUM_CHAN; i++)
    {
        m_atChannel[i].Reset();
    }

    //释放码流收发资源
    m_pcMtAdpNet->AudNetRcvQuit();
    m_pcMtAdpNet->AudNetSndQuit();
    m_pcMtAdpNet->VidNetRcvQuit();
    m_pcMtAdpNet->VidNetSndQuit();

    m_tVinMt.SetNull();
    m_bHasLsdToken = FALSE;
    m_bReceivedIIS = FALSE;
    m_wLsdTokenHoldTime = DEF_LSDTOKEN_RELEASE_TIME;
    m_bBchVidLoopback = FALSE;
    m_byLocalMcuId = 0;
    m_bRemoteTypeMcu = FALSE;
    m_bMasterMcu = FALSE;   
    m_bChairMt = FALSE;
    m_bAssignMtId = FALSE;
    m_tSpySrcMt.SetNull();
    memset(&m_tSMcuMtList, 0, sizeof(m_tSMcuMtList));

    MtAdpLog(LOG_INFO, "Instance %u cleared.\n", GetInsID());
}

/*=============================================================================
  函 数 名： InstanceEntry
  功    能： 相应终端处理实例
  算法实现： 
  全局变量： 
  参    数： CMessage *const pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::InstanceEntry(CMessage *const pcMsg)
{
    if(NULL == pcMsg)
    {
        MtAdpLog(LOG_CRITICAL, "[InstanceEntry] Msg pointer is Null!\n");
        return;
    }

    MtAdpLog(LOG_DEBUG, "[InstanceEntry] rcv message :%u(%s)\n", pcMsg->event, OspEventDesc(pcMsg->event));
    
    switch(pcMsg->event)
    {
        //mcu 邀请终端入会
    case MCU_MT_INVITEMT_REQ:
        OnMcuInviteMtReq(pcMsg);
        break;

        //删除终端
    case MCU_MT_DELMT_CMD:
        ClearInst();
        break;

        //mcu 打开逻辑信道响应
    case MCU_MT_OPENLOGICCHNNL_ACK:        
    case MCU_MT_OPENLOGICCHNNL_NACK:
        OnMcuMtOpenLogicChanRsp(pcMsg);
        break;

        //mcu 打开逻辑信道请求
    case MCU_MT_OPENLOGICCHNNL_REQ:
        OnMcuMtOpenLogicChanReq(pcMsg);
        break;

        //mcu 开启信道
    case MCU_MT_CHANNELON_CMD:
        OnMcuMtChanOnCmd(pcMsg);
        break;

        //mcu 停止信道
    case MCU_MT_CHANNELOFF_CMD:
        OnMcuMtChanOffCmd(pcMsg);
        break;

        //mcu 关闭信道
    case MCU_MT_CLOSELOGICCHNNL_CMD:
        OnMcuMtCloseChanCmd(pcMsg);
        break;

        //码流控制
    case MCU_MT_FLOWCONTROL_CMD:
        OnFlowCtrlCmd(pcMsg);
        break;

        //通知终端要看的终端
    case MCU_MT_YOUARESEEING_NOTIF:
        OnYouAreSeeingNotif(pcMsg);
        break;

        //强制关键帧
    case MCU_MT_FASTUPDATEPIC_CMD:
        OnMcuMtFastUpdatePicCmd(pcMsg);
        break;

        //指定主席
    case MCU_MT_SPECCHAIRMAN_NOTIF:
        //取消主席
    case MCU_MT_CANCELCHAIRMAN_NOTIF:
        OnMcuMtChairNotif(pcMsg);
        break;

        //指定发言
    case MCU_MT_SPECSPEAKER_NOTIF:      
        //取消发言
    case MCU_MT_CANCELSPEAKER_NOTIF:
        OnMcuMtSpeakerNotif(pcMsg);
        break;

        //指定选看
    case MCU_MT_SEENBYOTHER_NOTIF:   
        //取消选看
    case MCU_MT_CANCELSEENBYOTHER_NOTIF:
        OnMcuMtSelNotif(pcMsg);
        break;

        //静音操作
    case MCU_MT_MTMUTE_CMD:
        //哑音操作
    case MCU_MT_MTDUMB_CMD:
        OnMcuMtMuteDumbCmd(pcMsg);
        break;

        //终端列表响应
    case MCU_MT_JOINEDMTLIST_ACK:
    case MCU_MT_JOINEDMTLIST_NACK:    
        OnMcuMtMtListRsp(pcMsg);
        break;

        //终端入会离会通知
    case MCU_MT_MTJOINED_NOTIF:
    case MCU_MT_MTLEFT_NOTIF:
        OnMcuMtOnlineChangeNotif(pcMsg);
        break;

    case MCU_MT_GETBITRATEINFO_REQ:
        OnGetBitrateInfo(pcMsg);
        break;

        //终端别名响应
    case MCU_MT_GETMTALIAS_NACK:
    case MCU_MT_GETMTALIAS_ACK:	
        OnMcuMtGetMtAliasRsp(pcMsg);
        break;

        //终端申请主席响应
    case MCU_MT_APPLYCHAIRMAN_ACK:
        {
            m_bChairMt = TRUE;
        }        
        break;        
    case MCU_MT_APPLYCHAIRMAN_NACK:
        break;

        //终端取消主席响应
    case MCU_MT_CANCELCHAIRMAN_ACK:
        {
            m_bChairMt = FALSE;
        }
        break;
    case MCU_MT_CANCELCHAIRMAN_NACK:
        break;

        //主席终端指定发言响应
    case MCU_MT_SPECSPEAKER_ACK:        
    case MCU_MT_SPECSPEAKER_NACK:
        break;

        //选看操作
    case MCU_MT_STARTSELMT_ACK:
    case MCU_MT_STARTSELMT_NACK:
        OnMcuMtSelRsp(pcMsg);
        break;

        //会议信息通知
    case MCU_MT_SIMPLECONF_NOTIF:
    case MCU_MT_CONF_NOTIF:
        OnConfInfoNotif(pcMsg);
        break;

        //远遥操作
    case MCU_MT_MTCAMERA_RCENABLE_CMD:
    case MCU_MT_MTCAMERA_CTRL_CMD:
    case MCU_MT_MTCAMERA_CTRL_STOP:
    case MCU_MT_MTCAMERA_SAVETOPOS_CMD:
    case MCU_MT_MTCAMERA_MOVETOPOS_CMD:
    case MCU_MT_SETMTVIDSRC_CMD:
        OnMcuMtFeccCmd(pcMsg);
        break;

        //级联消息
    case MCU_MCU_REGISTER_NOTIF:	    //注册消息
    case MCU_MCU_ROSTER_NOTIF:		    //mcu合并级连式的花名册通知
    case MCU_MCU_MTLIST_REQ:		    //请求对方MCU的终端列表 
    case MCU_MCU_MTLIST_ACK:		
    case MCU_MCU_MTLIST_NACK:		
    case MCU_MCU_VIDEOINFO_REQ:		    //请求对方MCU的视频信息
    case MCU_MCU_VIDEOINFO_ACK:		
    case MCU_MCU_VIDEOINFO_NACK:	
    case MCU_MCU_CONFVIEWCHG_NOTIF:	    //会议视图发生变化通知   
    case MCU_MCU_AUDIOINFO_REQ:		    //请求对方MCU的音频信息
    case MCU_MCU_AUDIOINFO_ACK:		
    case MCU_MCU_AUDIOINFO_NACK:        
    		
    case MCU_MCU_INVITEMT_ACK:		
    case MCU_MCU_INVITEMT_NACK:		
    case MCU_MCU_NEWMT_NOTIF:		        
    case MCU_MCU_REINVITEMT_ACK:    
    case MCU_MCU_REINVITEMT_NACK:       
    case MCU_MCU_DROPMT_ACK:        
    case MCU_MCU_DROPMT_NACK:       
    case MCU_MCU_DROPMT_NOTIF:	
        
    case MCU_MCU_DELMT_ACK:			
    case MCU_MCU_DELMT_NACK:		
    case MCU_MCU_DELMT_NOTIF:      
        
    case MCU_MCU_SETMTCHAN_ACK:    
    case MCU_MCU_SETMTCHAN_NACK:   
    case MCU_MCU_SETMTCHAN_NOTIF:  
        
    case MCU_MCU_CALLALERTING_NOTIF:	//正在呼叫通报
            
    case MCU_MCU_SETIN_ACK:         
    case MCU_MCU_SETIN_NACK:        
        
    case MCU_MCU_SETOUT_REQ:            //指定view输入到终端
    case MCU_MCU_SETOUT_ACK:         
    case MCU_MCU_SETOUT_NACK:        
    case MCU_MCU_SETOUT_NOTIF:       
        
    case MCU_MCU_STARTMIXER_CMD:    
    case MCU_MCU_STARTMIXER_NOTIF:  
    case MCU_MCU_STOPMIXER_CMD:     
    case MCU_MCU_STOPMIXER_NOTIF:   
    case MCU_MCU_GETMIXERPARAM_REQ: 
    case MCU_MCU_GETMIXERPARAM_ACK: 
    case MCU_MCU_GETMIXERPARAM_NACK:
    case MCU_MCU_MIXERPARAM_NOTIF:  
    case MCU_MCU_ADDMIXMEMBER_CMD:  
    case MCU_MCU_REMOVEMIXMEMBER_CMD:
           
    case MCU_MCU_MTSTATUS_CMD:          //请求对方MCU某一终端状态
    case MCU_MCU_MTSTATUS_NOTIF:        //通知对方MCU自己某一所在直连终端状态 
        break;

    case MCU_MCU_SETIN_REQ:             //指定终端图像进入view
        OnSetInReq(pcMsg);
        break;
        
    case MCU_MCU_FECC_CMD:              //远遥
        break;	

        //不支持的级联操作，nack
    case MCU_MCU_INVITEMT_REQ:		    //对方MCU指定本级邀请终端请求
    case MCU_MCU_REINVITEMT_REQ:	    //对方MCU指定本级重邀终端请求
    case MCU_MCU_DROPMT_REQ:            //挂断终端请求
    case MCU_MCU_DELMT_REQ:             //删除终端请求
    case MCU_MCU_SETMTCHAN_REQ:         //请求设置媒体通道开关
        {
            CServMsg cServMsg;
            cServMsg.SetEventId(pcMsg->event+2);
            SendMsg2Mcu(cServMsg);
        }
        break;

        //h320 stack fecc callback notify
    case EV_H320STACK_FECC_NOTIF:
        OnH320StackFecc(pcMsg);
        break;

        //h320 stack cmd callback notify
    case EV_H320STACK_CMD_NOTIF:
        OnH320StackCmd(pcMsg);
        break;

        //h320 stack state callback notify
    case EV_H320STACK_STATE_NOTIF:
        OnH320StackState(pcMsg);
        break;

        //邀请终端超时处理
    case TIMER_INVITEMT:
        {
            MtAdpLog(LOG_INFO, "[InstanceEntry] rcv invite mt timeout msg\n");
            OnInviteMtTimeout(pcMsg);
        }
        break;

        //交换信息打印
    case EV_MTADP_SHOWSWITCH:
        OnPrintSwitchInfo(pcMsg);
        break;

        //no bch video loop back
    case EV_MTADP_VIDLOOPBACK:
        {
            if(NULL != m_pcH320Stack)
            {
                BOOL32 bLoop = (*(pcMsg->content) != 0) ? TRUE : FALSE;
                m_pcH320Stack->SetVidLoopback(bLoop);
            }
        }
        break;

    case EV_MTADP_BCHVIDLOOPBACK://bch video loop back
        {
            m_bBchVidLoopback = (*(pcMsg->content) != 0) ? TRUE : FALSE;
        }
        break;

        //测试消息
    case MCU_H320MTADP_PRIVATE_MSG: 
        OnMcuMtPrivateMsg(pcMsg);
        break;

        //是否lsd远遥令牌
    case TIMER_LSDTOKEN_RELEASE:
        OnReleaseLsdToken();
        break;

    default:
        {
            MtAdpLog(LOG_CRITICAL, "[InstanceEntry] rcv invalid msg :%u\n", pcMsg->event);
        }
        break;
    }
}

/*=============================================================================
  函 数 名： OnMcuInviteMtReq
  功    能： mcu邀请终端入会
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnMcuInviteMtReq(const CMessage *pcMsg)
{
    //状态机校验
    if(STATE_IDLE != CurState())
    {
        MtAdpLog(LOG_CRITICAL, "[OnMcuInviteMtReq] rcv msg<%d> in invalid state<%d>,ignore it\n",
                 pcMsg->event, CurState());
        return;
    }

    //加载配置信息，在此加载无需重启
//#ifdef _VXWORKS_
    LoadConfig();
//#endif //_VXWORKS_
    
    CServMsg cServMsg(pcMsg->content, pcMsg->length);   
    
    //保存终端信息
    TMt *ptMt = (TMt *)cServMsg.GetMsgBody();
    m_byMtId = ptMt->GetMtId();
    m_tMtAlias = *(TMtAlias *)(cServMsg.GetMsgBody() + sizeof(TMt));
    m_byConfIdx = cServMsg.GetConfIdx();
    m_cConfId = cServMsg.GetConfId();

    //终端与实例映射关系
    g_cMtAdpApp.m_abyMt2InsMap[m_byConfIdx][m_byMtId] = (u8)GetInsID();

    //会议码率
    m_wConfRate = ntohs(*(u16 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMtAlias)*2 + 1));
    u32 dwTsMask = 0;
    u32 dwTransCap = 0;
    if(!GetTsMaskAndTransFromBitrate(m_wConfRate, dwTsMask, dwTransCap))
    {
        MtAdpLog(LOG_CRITICAL, "[OnMcuInviteMtReq] invalid conf bitrate<%dk>. nack!\n", m_wConfRate);
        
        u8 byCause = emDisconnectReasonLocal;
        
        cServMsg.SetEventId(cServMsg.GetEventId() + 2);
        cServMsg.SetMsgBody((u8 *)&m_tMtAlias, sizeof(m_tMtAlias));
        cServMsg.CatMsgBody(&byCause, sizeof(byCause));
        SendMsg2Mcu(cServMsg);
        return;
    }

    //会议能力集
    TCapSupport tCap = *(TCapSupport *)(cServMsg.GetMsgBody() + sizeof(TMt) + 
                         sizeof(TMtAlias)*2 + 1 + sizeof(u16));

    // guzh [10/26/2007] 
    // 不处理扩展能力 TCapSupportEx
    u16 wLen = sizeof(TMt) + sizeof(TMtAlias)*2 + 1 + sizeof(u16) + sizeof(TCapSupport);
   
    //本端能力集
    CapSetIn2Out(tCap, dwTransCap, m_tLocalCapSet);
    m_pcH320Stack->SetLocalCapSet(m_tLocalCapSet);
    
    //优先级能力集
    m_pcH320Stack->SetPriCapset(m_tLocalCapSet);

    //注册回调函数
    m_pcH320Stack->RegAudioProcCB(ProcAudData, (u32)this);
    m_pcH320Stack->RegVideoProcCB(ProcVidData, (u32)this);
    m_pcH320Stack->RegFECCProcCB(ProcFECCData, (u32)this);
    m_pcH320Stack->RegStateChangeProcCB(ProcH320State, (u32)this);
    m_pcH320Stack->RegCIProcCB(ProcCmdData, (u32)this);
    m_pcH320Stack->RegHSDProcCB(ProcHSDData, (u32)this);

	//会议别名
	m_tConfName = *(TMtAlias *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMtAlias));

    TKDVH320E1Info tInfo;
    tInfo.m_byChanId = GetInsID()-1;
    tInfo.m_dwTsMask = dwTsMask;

    NEXTSTATE(STATE_CALLING);
    
    //发起呼叫
    u16 wRet = m_pcH320Stack->CallStart(tInfo);
    if(KDVH320_OK != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[OnMcuInviteMtReq] start h320 stack call failed!\n");

        u8 byCause = emDisconnectReasonUnknown;
        
        cServMsg.SetEventId(cServMsg.GetEventId() + 2);
        cServMsg.SetMsgBody((u8 *)&m_tMtAlias, sizeof(m_tMtAlias));
        cServMsg.CatMsgBody(&byCause, sizeof(byCause));
        SendMsg2Mcu(cServMsg);

        ClearInst();        
        return;
    }

    MtAdpLog(LOG_CRITICAL, "[OnMcuInviteMtReq] start h320 stack call success!\n");

    //从mcu默认id，当对端主mcu分配id后再更新
    if(!m_bMasterMcu)
    {
        m_byLocalMcuId = m_byMtId;
    }
}

/*=============================================================================
  函 数 名： OnInviteMtTimeout
  功    能： 邀请终端超时处理
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnInviteMtTimeout(const CMessage *pcMsg)
{
    u8 byCause = emDisconnectReasonUnreachable;

    CServMsg cServMsg;
    cServMsg.SetEventId(MT_MCU_INVITEMT_NACK);
    cServMsg.SetMsgBody((u8 *)&m_tMtAlias, sizeof(m_tMtAlias));
    cServMsg.CatMsgBody(&byCause, sizeof(byCause));
    SendMsg2Mcu(cServMsg);

    ClearInst();
}

/*=============================================================================
  函 数 名： OnH320StackState
  功    能： 协议栈状态处理
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnH320StackState(const CMessage *pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    u16 wState = *(u16 *)cServMsg.GetMsgBody();
    u8 *pbyContent = (cServMsg.GetMsgBody() + sizeof(u16));

    switch(wState)
    {
    case state_remotecapset :
        {
            MtAdpLog( LOG_INFO, "[OnH320StackState]: remote capset notify\n" );              
        }
        break;
        
    case state_localmuxmode :
        {
            MtAdpLog( LOG_INFO, "[OnH320StackState]: local mux mode notify\n" );  

            TKDVH320MuxMode *ptLocalMode = (TKDVH320MuxMode*)pbyContent; 

            MtAdpLog( LOG_INFO, "\t TransRate=%d, AudType=%d, VidType=%d, LSDType=%d, HSDType=%d\n", 
                        ptLocalMode->m_byTransRate, ptLocalMode->m_byAudType, 
                        ptLocalMode->m_byVidType, ptLocalMode->m_byLSDType, 
                        ptLocalMode->m_byHSDType);    
            
            OnLocalMuxMode(ptLocalMode);            
        }
        break;
        
    case state_remotemuxmode :
        {
            MtAdpLog( LOG_INFO, "[OnH320StackState]: remote mux mode notify\n" );
            
            TKDVH320MuxMode *ptRemotMode = (TKDVH320MuxMode*)pbyContent;
            
            MtAdpLog( LOG_INFO, "\t TransRate=%d, AudType=%d, VidType=%d, LSDType=%d, HSDType=%d\n", 
                        ptRemotMode->m_byTransRate, ptRemotMode->m_byAudType, 
                        ptRemotMode->m_byVidType, ptRemotMode->m_byLSDType, 
                        ptRemotMode->m_byHSDType);

            //保存远端复用模式
            m_tRemoteMuxMode = *ptRemotMode;

            if(/*!m_bRemoteTypeMcu || */1 == g_bySndMtList)
            {
                //MCC
                if(KDVH320_OK == m_pcH320Stack->SendCommand(KDVH320_CI_MCC, NULL, 0))
                {
                    MtAdpLog(LOG_INFO, "[OnH320StackState] send KDVH320_CI_MCC to remote success.\n");
                }
                
                //告诉对端本端主从关系, MIM
                if(m_bMasterMcu)
                {
                    if(KDVH320_OK == m_pcH320Stack->SendCommand(KDVH320_CI_MIM, NULL, 0))
                    {
                        MtAdpLog( LOG_INFO, "[OnH320StackState] KDVH320_CI_MIM. local mcu is master. \n" );
                    }                
                }
                
                //MCS            
                if(KDVH320_OK == m_pcH320Stack->SendCommand(KDVH320_CI_MCS, NULL, 0))
                {
                    MtAdpLog( LOG_INFO, "[OnH320StackState] send KDVH320_CI_MCS to remote sucess.\n" );
                }
            }            

            //remote is mt or slave mcu, assign mt id
            if(!m_bRemoteTypeMcu || (m_bRemoteTypeMcu && m_bMasterMcu))
            {
                TMt tMt;
                AssignTia(tMt);
                
                u8 abyParam[2]; 
                abyParam[0] = tMt.GetMcuId();
                abyParam[1] = tMt.GetMtId();

                //请求对端列表
                if(KDVH320_OK == m_pcH320Stack->SendCommand(KDVH320_CI_TCU, NULL, 0))
                {
                    MtAdpLog(LOG_INFO, "[OnH320StackState] send KDVH320_CI_TCU to remote success.\n");
                }   

                //tia
                if(KDVH320_OK == m_pcH320Stack->SendCommand(KDVH320_CI_TIA, abyParam, sizeof(abyParam)))
                {
                    MtAdpLog(LOG_INFO, "[OnH320StackState] send KDVH320_CI_TIA <%d><%d> to remote success.\n",
                        abyParam[0], abyParam[1]);
                    m_bAssignMtId = TRUE;//send tid in mcc
                }                
                 
                //TCS-2 请求对端别名，中兴3000终端收到此消息会重新进行复用模式通知，3000mt主动上报别名，所以暂不请求
                if(!m_bRemoteTypeMcu && !m_bReceivedIIS)
                {
                    if(KDVH320_OK == m_pcH320Stack->SendCommand(KDVH320_CI_TCS_2, NULL, 0))
                    {
                        MtAdpLog(LOG_INFO, "[OnH320StackState] send KDVH320_CI_TCS_2 to remote success.\n");
                    }
                }        
            }
        }
        break;
        
    case state_framealign :
        {
            MtAdpLog( LOG_INFO, "[OnH320StackState]: frame align notify\n" ); 

            if(STATE_CALLING != CurState())
            {
                MtAdpLog( LOG_WARNING, "[OnH320StackState]: instace state<%u> is invalid, ignore it\n",
                         CurState()); 
                return;
            }           
      
            //邀请终端ack
            TMtAlias tMtAddr;
            tMtAddr.m_AliasType = mtAliasTypeTransportAddress;
            tMtAddr.m_tTransportAddr.SetNetSeqIpAddr(g_cMtAdpApp.m_dwLocalIP);

            u8 byType = TYPE_MT;
            u8 byEncrypt = 0;
            cServMsg.SetEventId(MT_MCU_INVITEMT_ACK);
            cServMsg.SetMsgBody((u8 *)&m_tMtAlias, sizeof(m_tMtAlias));
            cServMsg.CatMsgBody((u8 *)&tMtAddr, sizeof(tMtAddr));
            cServMsg.CatMsgBody(&byType, sizeof(byType));
            cServMsg.CatMsgBody(&byEncrypt, sizeof(byEncrypt));
            SendMsg2Mcu(cServMsg);
        }        
        break;
        
    case state_framelost :
        {
            MtAdpLog( LOG_CRITICAL, "[OnH320StackState]: frame lost notify\n" );  

            //向mcu发送断链消息
            u8 byCause = emDisconnectReasonRtd;
            cServMsg.SetEventId(MT_MCU_MTDISCONNECTED_NOTIF);
            cServMsg.SetMsgBody(&byCause, sizeof(byCause));
            SendMsg2Mcu(cServMsg);

            //清空部分实例信息
            NEXTSTATE(STATE_CALLING);               
 
            //本端复用模式
            memset(&m_tLocalMuxMode, 0, sizeof(m_tLocalMuxMode));
            //远端复用模式
            memset(&m_tRemoteMuxMode, 0, sizeof(m_tRemoteMuxMode));         
                      
            //释放音视频信道
            for(s32 i = 0; i < MAXNUM_CHAN; i++)
            {
                m_atChannel[i].Reset();
            }
            
            //释放码流收发资源
            m_pcMtAdpNet->AudNetRcvQuit();
            m_pcMtAdpNet->AudNetSndQuit();
            m_pcMtAdpNet->VidNetRcvQuit();
            m_pcMtAdpNet->VidNetSndQuit();
            
            m_tVinMt.SetNull();
            m_wVideoRate = 0;  
            m_bHasLsdToken = FALSE;
            m_bReceivedIIS = FALSE;
            m_bRemoteTypeMcu = FALSE;            
            m_bChairMt = FALSE;
            m_bAssignMtId = FALSE;
            m_tSpySrcMt.SetNull();
            memset(&m_tSMcuMtList, 0, sizeof(m_tSMcuMtList));
        }              
        break;
        
    case state_abitset :
        MtAdpLog( LOG_INFO, "[OnH320StackState]: A Bit set notify\n" );
        break;
        
    case state_abitclear :
        MtAdpLog( LOG_INFO, "[OnH320StackState]: A Bit clear notify\n" );
        break;	
        
    case state_vidratechange :
        {
            u32 dwVideoRate = (*(u32 *)pbyContent)*8/1000;

            MtAdpLog( LOG_INFO, "[OnH320StackState]: video rate change notify.<%ukbps>\n",
                      dwVideoRate);       
                        
/*            if(m_wVideoRate != dwVideoRate)
            {
                m_wVideoRate = dwVideoRate;

                //若视频发送通道已经打开，发送流控制消息，改变码流接收码率 (目前mcu没有处理)
                if(CHAN_CONNECTED == m_atChannel[VID_SND_INDEX].m_byState)
                {
                    TLogicalChannel tChan = m_atChannel[VID_SND_INDEX].m_tLogicChan;
                    tChan.SetFlowControl(m_wVideoRate);
                    
                    cServMsg.SetEventId(MT_MCU_FLOWCONTROL_CMD);
                    cServMsg.SetMsgBody((u8 *)&tChan);
                    SendMsg2Mcu(cServMsg);
                    
                    MtAdpLog( LOG_INFO, "[OnH320StackState] send flow control msg to mcu.\n");
                }
            }*/            
        }        
        break;   
        
    default:
        MtAdpLog( LOG_CRITICAL, "[OnH320StackState]: received invalid stack state!\n");
        break;
    }
}

/*=============================================================================
  函 数 名： OnH320StackFecc
  功    能： 协议栈远遥处理 (目前点对点模式不支持)
  算法实现： 远遥当然发言人
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnH320StackFecc(const CMessage *pcMsg)
{
    if(STATE_NORMAL != CurState())
    {
        MtAdpLog(LOG_CRITICAL, "[OnH320StackFecc] instace state<%u> is invalid. ignore it\n",
                 CurState());
        return;
    }    

    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    u8 byKind = *(cServMsg.GetMsgBody());
    u8 byAction = *(cServMsg.GetMsgBody() + sizeof(u8));
    TH320Mt *ptDstMt = (TH320Mt *)(cServMsg.GetMsgBody() + 3*sizeof(u8));
    
    if(ptDstMt->m_byMcuId != m_tSpySrcMt.GetMcuId() || ptDstMt->m_byMtId != m_tSpySrcMt.GetMtId())
    {
        MtAdpLog(LOG_CRITICAL, "[OnH320StackFecc] receive other mt<%d,%d> fecc cmd, local mt<%d,%d>\n",
                 ptDstMt->m_byMcuId, ptDstMt->m_byMtId, m_tSpySrcMt.GetMcuId(), m_tSpySrcMt.GetMtId());
        return;
    }
    else
    {
        MtAdpLog(LOG_INFO, "[OnH320StackFecc] fecc param1<%d> param2<%d> dstmt<%d, %d>.\n", 
            byKind, byAction, ptDstMt->m_byMcuId, ptDstMt->m_byMtId);
    }

    u8 byParam = 0;

    //应该为发言人终端
    TMt  tMt;    
    tMt.SetNull();
    
    switch(byKind)
    {
    case feccact_Start:
    case feccact_Continue:
        {
            byParam = FeccActionOut2In(byAction);
            if(0 == byParam)
            {
                MtAdpLog(LOG_CRITICAL, "[OnH320StackFecc] invalid fecc action<%d> from stack\n",
                         byAction);
                return;
            }

            cServMsg.SetEventId(MT_MCU_MTCAMERA_CTRL_CMD);
            cServMsg.SetMsgBody((u8 *)&tMt, sizeof(tMt));
            cServMsg.CatMsgBody(&byParam, sizeof(byParam));
            SendMsg2Mcu(cServMsg);

            MtAdpLog(LOG_INFO, "[OnH320StackFecc] fecc start action<%d>\n", byParam);
        }
        break;   

    case feccact_Stop:
        {
            byParam = FeccActionOut2In(byAction);
            if(0 == byParam)
            {
                MtAdpLog(LOG_CRITICAL, "[OnH320StackFecc] invalid fecc action<%d> from stack\n",
                         byAction);                
                return;
            }
            
            cServMsg.SetEventId(MT_MCU_MTCAMERA_CTRL_STOP);
            cServMsg.SetMsgBody((u8 *)&tMt, sizeof(tMt));
            cServMsg.CatMsgBody(&byParam, sizeof(byParam));
            SendMsg2Mcu(cServMsg);

            MtAdpLog(LOG_INFO, "[OnH320StackFecc] fecc stop action<%d>\n", byParam);
        }
        break;

    case feccact_StoreAsPreset:
        {
            byParam = (byAction>>4) & 0x0f;

            cServMsg.SetEventId(MT_MCU_MTCAMERA_SAVETOPOS_CMD);
            cServMsg.SetMsgBody((u8 *)&tMt, sizeof(tMt));
            cServMsg.CatMsgBody(&byParam, sizeof(byParam));
            SendMsg2Mcu(cServMsg);

            MtAdpLog(LOG_INFO, "[OnH320StackFecc] fecc save to pos<%d> action\n", byParam);
        }
        break;

    case feccact_ActivatePreset:
        {
            byParam = (byAction>>4) & 0x0f;

            cServMsg.SetEventId(MT_MCU_MTCAMERA_MOVETOPOS_CMD);
            cServMsg.SetMsgBody((u8 *)&tMt, sizeof(tMt));
            cServMsg.CatMsgBody(&byParam, sizeof(byParam));
            SendMsg2Mcu(cServMsg);

            MtAdpLog(LOG_INFO, "[OnH320StackFecc] fecc move to pos<%d> action\n", byParam);
        }
        break;

    case feccact_SelectVideoSource:
        {
            byParam = (byAction>>4) & 0x0f;
            
            cServMsg.SetEventId(MT_MCU_SELECTVIDEOSOURCE_CMD);
            cServMsg.SetMsgBody((u8 *)&tMt, sizeof(tMt));
            cServMsg.CatMsgBody(&byParam, sizeof(byParam));
            SendMsg2Mcu(cServMsg);

            MtAdpLog(LOG_INFO, "[OnH320StackFecc] fecc sel video src<%d> action\n", byParam);
        }
        break;

    default:
        MtAdpLog(LOG_CRITICAL, "[OnH320StackFecc] rcv invalid fecc action<%d>\n", byKind);
        break;
    }
}

/*=============================================================================
  函 数 名： OnH320StackCmd
  功    能： 协议栈cmd处理
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnH320StackCmd(const CMessage *pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    u16 wCmdId = *(u16 *)cServMsg.GetMsgBody();
    u8  *pbyParam = (u8 *)(cServMsg.GetMsgBody()+sizeof(u16));
    u16 wParamLen = cServMsg.GetMsgBodyLen()-sizeof(u16);

    switch(wCmdId)
    {
    case KDVH320_CI_CIT:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] Chair Indicate Token \n" );   
        break;

    case KDVH320_CI_CIS:
        OnCmdCIS();        
        break;

    case KDVH320_CI_CCA:    //from mt
        OnCmdCCA();            
        break;
        
    case KDVH320_CI_CCR: //send from mcu
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] Chair Command Release/refuse \n" );            
        break;               

    case KDVH320_CI_CIC:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] Chair-control Indicate Capability \n" );
        break;

    case KDVH320_CI_TCS_2:
        {
            MtAdpLog( LOG_INFO, "[OnH320StackCmd] Terminal Command String: identiy\n" );            
            OnSendCmdIIS();
        }
        break;

        //对端返回的别名
	case KDVH320_CI_IIS:
		OnCmdIIS(pbyParam, wParamLen);                 
		break;

    case KDVH320_CI_TIA:
        OnCmdTIA(pbyParam, wParamLen);
        break;

    case KDVH320_CI_TIS:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] terminal indicate identity \n" );        
        break;

    case KDVH320_CI_TIE:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] Terminal Indicate End_of_Listing \n" );     
        break;

    case KDVH320_CI_TIC:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] Terminal Indicate Capability \n" );     
        break;

    case KDVH320_CI_DCA_L : 
        OnCmdDCAL();       
        break;
        
    case KDVH320_CI_DIT_L :
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] LSD Indicate Token \n" );
        m_bHasLsdToken = TRUE; //获取远遥令牌
        break;
        
    case KDVH320_CI_DCR_L :
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] LSD Command Release/refuse \n" );
        break;
        
    case KDVH320_CI_DIS_L :
        OnCmdDISL();       
        break;
        
    case KDVH320_CI_DCC_L :
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] LSD Command Close \n" );
        break;
        
    case KDVH320_CI_DCA_H :
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] HSD Command Acquire-token \n" );
        break;

    case KDVH320_CI_DIT_H :        
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] HSD Indicate Token \n" );
        break;
        
    case KDVH320_CI_DIS_H :
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] HSD Indicate Stopped-using-token \n" );
        break;
        
    case KDVH320_CI_DCR_H :
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] HSD Command Release/refuse \n" );
        break;

    case KDVH320_CI_DCC_H :
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] HSD Command Close \n" );
        break;

    case h_ci_VidIndicateSuppressed:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd]  h_ci_VidIndicateSuppressed\n" );//图像显示停止命令
        break;

    case h_ci_VidIndicateActive:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] h_ci_VidIndicateActive \n" );
        break;

    case h_ci_VidIndicateActive2:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] h_ci_VidIndicateActive2\n" );
        break;

    case h_ci_VidIndicateActive3:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] h_ci_VidIndicateActive3 \n" );
        break;

    case h_ci_VidIndicateReady:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] h_ci_VidIndicateReady \n" );//预备激活
        break;

    case h_ci_VidCommandFreeze:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] h_ci_VidCommandFreeze \n" );
        break;

    case h_ci_VidCommandUpdate:
        OnCmdVCU();
        break;

    case h_ci_AudIndicateMuted:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] h_ci_AudIndicateMuted \n" );
        break;

    case h_ci_AudIndicateActive:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] h_ci_AudIndicateActive \n" );
        break;

    case h_ci_AudCommandEqualize:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] h_ci_AudCommandZeroDelay \n" );//音频补偿
        break;

    case h_ci_AudCommandZeroDelay:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] h_ci_AudCommandZeroDelay \n" );//音频延迟
        break;

    case KDVH320_CI_MCV:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] Multipoint Command Visualization-forcing \n" );
        break;

    case KDVH320_CI_MIV:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] Multipoint Indication Visualization-forcing \n" );
        break;

    case KDVH320_CI_MVC:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] Multipoint Visualization Capability \n" );
        break;

    case KDVH320_CI_MVA:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] Multipoint Visualization Achieved \n" );
        break;

    case KDVH320_CI_MVR:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] Multipoint Visualization Refused/Revoked \n" );
        break;

    case KDVH320_CI_MCC:
        OnCmdMCC(pbyParam, wParamLen);
        break;

    case KDVH320_CI_MCS:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] Multipoint Command Symmetrical data-transmission \n" );
        break;

    case KDVH320_CI_MCN:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] Multipoint Command Negating MCS \n" );
        break;

    case KDVH320_CI_MIZ:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] Multipoint Indication Zero-communication \n" );
        break;

    case KDVH320_CI_MIS:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] Multipoint Secondary-status \n" );
        break;

    case KDVH320_CI_MIM:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] Multipoint Indicate Master-MCU \n" );
        break;

    case KDVH320_CI_MIL:
        {
            if(wParamLen < 1)
            {
                MtAdpLog(LOG_CRITICAL, "[OnH320StackCmd] KDVH320_CI_MIL, invalid param!\n");
                return;
            }
            else
            {
                MtAdpLog( LOG_INFO, "[OnH320StackCmd] Multipoint Indicate Loop :N<%d>\n", pbyParam[0] );
            }            
        }        
        break;

    case KDVH320_CI_MIH:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] Multipoint Indicate Hierarchy\n" );
        break;
        
    case KDVH320_CI_MIJ:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] Multipoint Indicate Joined_Real_Conference \n" );
        break;

    case KDVH320_CI_TCU:
        OnCmdTCU();
        break;

    case KDVH320_CI_TIX:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] Terminal Indicate additional-channel-X\n" );
        break;

    case KDVH320_CI_TIN:
        OnCmdTIN(pbyParam, wParamLen);
        break;

    case KDVH320_CI_TID:
        OnCmdTID(pbyParam, wParamLen);
        break;

        //mcu seeing mt number
    case KDVH320_CI_VIN:
        OnCmdVIN(pbyParam, wParamLen);
        break;

    case KDVH320_CI_TIL:
        OnCmdTIL(pbyParam, wParamLen);
        break;
        
    case KDVH320_CI_TIP:
        OnCmdTIP(pbyParam, wParamLen);
        break;

        //TIP rsp
    case KDVH320_CI_TCP:
        OnCmdTCP(pbyParam, wParamLen);
        break;
        
        //rsp with TIR
    case KDVH320_CI_TCA:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] Token Command Association \n" );
        break;

    case KDVH320_CI_TIR:    //from mcu notify
        OnCmdTIR(pbyParam, wParamLen);
        break;
            
    case KDVH320_CI_CANCELMIZ:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] Cancel MIZ \n" );
        break;
        
    case KDVH320_CI_CANCELMIS:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] Cancel MIS \n" );
        break;
        
    case KDVH320_CI_CANCELMCV:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] Cancel MCV \n" );
        break;
        
    case KDVH320_CI_CANCELMIV:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] Cancel MIV \n" );
        break;
        
    case KDVH320_CI_LCO:
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] loopback command off \n" );
        break;
        
        //speaker
    case KDVH320_CI_VCB:
        OnCmdVCB(pbyParam, wParamLen);
        break;

    case KDVH320_CI_CancelVCB:
        OnCmdCancelVCB();
        break;
        
        //select
    case KDVH320_CI_VCS:
        OnCmdVCS(pbyParam, wParamLen);
        break;    

    case KDVH320_CI_CancelVCS:
        OnCmdCancelVCS();
        break;

    case KDVH320_CI_TIF://<0><0>-mt self
        {
            MtAdpLog( LOG_INFO, "[OnH320StackCmd] Terminal Indicate Floor-request.\n");

            if(wParamLen < 2)
            {
                MtAdpLog( LOG_CRITICAL, "[OnH320StackCmd] TIF param is invalid, Param Num:%d\n", wParamLen);
                return;
            }
            else
            {
                MtAdpLog(LOG_INFO, "[OnH320StackCmd] TIF <%d><%d>\n", pbyParam[0], pbyParam[1]);
            }
        }        
        break;

    default :
        MtAdpLog( LOG_INFO, "[OnH320StackCmd] receve invalid cmd id :%d \n", wCmdId );
        break;;
    }
}

/*=============================================================================
  函 数 名： OnSendCmdIIS
  功    能： send cmd iis
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnSendCmdIIS()
{
    //将会议名称作为实体别名发给对端            
    TH320IIS tIIS;
    tIIS.m_byIISType = IIS_TYPE_IDENTITY;
    astrncpy(tIIS.m_szAlias, m_tConfName.m_achAlias, MAXLEN_EPALIAS, MAXLEN_ALIAS);
    u16 wRet = m_pcH320Stack->SendCommand(KDVH320_CI_IIS, (u8 *)&tIIS, sizeof(tIIS));
    if(KDVH320_OK != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[OnSendCmdIIS] send cmd KDVH320_CI_IIS failed, ret<%d>\n", wRet);
    }
    else
    {
        MtAdpLog( LOG_INFO, "[OnSendCmdIIS] send KDVH320_CI_IIS to remote :%s\n", tIIS.m_szAlias );
    }        
}

/*=============================================================================
  函 数 名： OnLocalMuxMode
  功    能： 收到本端复用模式处理
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnLocalMuxMode(const TKDVH320MuxMode *ptMuxMode)
{
    CServMsg cServMsg;
    
    //终端成功入会
    if(CurState() == STATE_CALLING)
    {
        NEXTSTATE(STATE_NORMAL);
        
        u8 byMaster = 0;
        cServMsg.SetEventId(MT_MCU_MTJOINCONF_NOTIF);
        cServMsg.SetMsgBody(&byMaster, sizeof(byMaster));
        SendMsg2Mcu(cServMsg);
    }
    
    //是否已经回调过
    if(!IsEqualMuxMode(m_tLocalMuxMode, *ptMuxMode))
    {
        //保存本端复用模式
        m_tLocalMuxMode = *ptMuxMode;
                
        //本端共同能力集通知
        TCapSupport tComCap;
        MuxMode2Cap(m_tLocalMuxMode, m_tLocalCapSet, tComCap);

        TSimCapSet tSimCap = tComCap.GetMainSimCapSet();

        //由于320系统lsd通道不打开时视频码率会比323系统大，造成不能进行选看操作，所以采用323系统的视频码率上报
        m_wVideoRate = m_wConfRate-GetAudioBitrate(tSimCap.GetAudioMediaType()); 
        
        tSimCap.SetVideoMaxBitRate(m_wVideoRate);
        tComCap.SetMainSimCapSet(tSimCap);
                
        cServMsg.SetEventId(MT_MCU_CAPBILITYSET_NOTIF);
        cServMsg.SetMsgBody((u8 *)&tComCap, sizeof(tComCap));
        // guzh [10/26/2007] H320均不考虑扩展能力
        TCapSupportEx tCapEx;
        cServMsg.CatMsgBody((u8*)&tCapEx, sizeof(tCapEx));
        SendMsg2Mcu(cServMsg);   
        
        //向mcu打开逻辑信道请求    

        TMediaEncrypt tEncrypt;
        tEncrypt.SetEncryptMode(CONF_ENCRYPTMODE_NONE);
        
        u16 wPort = AssignSwitchPort();

        //音频发送信道
        u8 byMediaType = tSimCap.GetAudioMediaType();
        if(MEDIA_TYPE_NULL != byMediaType)
        {
            m_atChannel[AUD_SND_INDEX].m_byState = CHAN_OPENING;
            m_atChannel[AUD_SND_INDEX].m_tLogicChan.SetMediaType(MODE_AUDIO);
            m_atChannel[AUD_SND_INDEX].m_tLogicChan.SetChannelType(byMediaType);
            m_atChannel[AUD_SND_INDEX].m_tLogicChan.SetMediaEncrypt(tEncrypt);
            m_atChannel[AUD_SND_INDEX].m_tLogicChan.SetFlowControl(GetAudioBitrate(byMediaType));
            m_atChannel[AUD_SND_INDEX].m_tSndMediaChannel.SetNetSeqIpAddr(g_cMtAdpApp.m_dwLocalIP);
            m_atChannel[AUD_SND_INDEX].m_tSndMediaChannel.SetPort(wPort + AUD_SND_PORT);
            m_atChannel[AUD_SND_INDEX].m_tLogicChan.m_tSndMediaCtrlChannel.SetNetSeqIpAddr(g_cMtAdpApp.m_dwLocalIP);
            m_atChannel[AUD_SND_INDEX].m_tLogicChan.m_tSndMediaCtrlChannel.SetPort(wPort + AUD_SND_CTRLPORT);
            
            cServMsg.SetEventId(MT_MCU_OPENLOGICCHNNL_REQ);
            cServMsg.SetMsgBody((u8 *)&m_atChannel[AUD_SND_INDEX].m_tLogicChan, sizeof(TLogicalChannel));
            SendMsg2Mcu(cServMsg);

            MtAdpLog(LOG_INFO, "[OnLocalMuxMode] mt -> mcu open audio chan bitrate:%d.\n", GetAudioBitrate(byMediaType));
        }
        
        //视频发送信道
        byMediaType = tSimCap.GetVideoMediaType();
        if(MEDIA_TYPE_NULL != byMediaType)
        {
            m_atChannel[VID_SND_INDEX].m_byState = CHAN_OPENING;
            m_atChannel[VID_SND_INDEX].m_tLogicChan.SetMediaType(MODE_VIDEO);
            m_atChannel[VID_SND_INDEX].m_tLogicChan.SetChannelType(byMediaType);
            m_atChannel[VID_SND_INDEX].m_tLogicChan.SetMediaEncrypt(tEncrypt);
            m_atChannel[VID_SND_INDEX].m_tLogicChan.SetFlowControl(m_wVideoRate);
            m_atChannel[VID_SND_INDEX].m_tLogicChan.SetVideoFormat(VIDEO_FORMAT_CIF);
            m_atChannel[VID_SND_INDEX].m_tSndMediaChannel.SetNetSeqIpAddr(g_cMtAdpApp.m_dwLocalIP);
            m_atChannel[VID_SND_INDEX].m_tSndMediaChannel.SetPort(wPort + VID_SND_PORT);
            m_atChannel[VID_SND_INDEX].m_tLogicChan.m_tSndMediaCtrlChannel.SetNetSeqIpAddr(g_cMtAdpApp.m_dwLocalIP);
            m_atChannel[VID_SND_INDEX].m_tLogicChan.m_tSndMediaCtrlChannel.SetPort(wPort + VID_SND_CTRLPORT);
            
            cServMsg.SetEventId(MT_MCU_OPENLOGICCHNNL_REQ);
            cServMsg.SetMsgBody((u8 *)&m_atChannel[VID_SND_INDEX].m_tLogicChan, sizeof(TLogicalChannel));
            SendMsg2Mcu(cServMsg);

            MtAdpLog(LOG_INFO, "[OnLocalMuxMode] mt -> mcu open video chan bitrate:%d.\n", m_wVideoRate);
        }
    }
}

/*=============================================================================
  函 数 名： OnMcuMtOpenLogicChanRsp
  功    能： mcu对mt打开逻辑信道响应
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnMcuMtOpenLogicChanRsp(const CMessage *pcMsg)
{
    if(STATE_NORMAL != CurState())
    {
        MtAdpLog(LOG_CRITICAL, "[OnMcuMtOpenLogicChanRsp] instace state<%u> is invalid. ignore it\n",
                 CurState());    
        return;
    }
    
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    
    TLogicalChannel *ptLogicChan = (TLogicalChannel *)cServMsg.GetMsgBody();
    
    u8 byMediaType = ptLogicChan->GetMediaType();  
    
    //同意
    if(MCU_MT_OPENLOGICCHNNL_ACK == pcMsg->event)
    {
        TChannel *ptChan = NULL;

        u32 dwNetIp = ptLogicChan->m_tRcvMediaChannel.GetNetSeqIpAddr();
        u16 wPort = ptLogicChan->m_tRcvMediaChannel.GetPort();
        u32 dwNetCtrlIp = ptLogicChan->m_tRcvMediaCtrlChannel.GetNetSeqIpAddr();        
        u16 wCtrlPort = ptLogicChan->m_tRcvMediaCtrlChannel.GetPort();        

        if(MODE_AUDIO == byMediaType)
        {
            ptChan = &m_atChannel[AUD_SND_INDEX];
            ptChan->m_byState = CHAN_CONNECTED;
            ptChan->m_tLogicChan.m_tRcvMediaChannel.SetNetSeqIpAddr(dwNetIp);
            ptChan->m_tLogicChan.m_tRcvMediaChannel.SetPort(wPort);
            ptChan->m_tLogicChan.m_tRcvMediaCtrlChannel.SetNetSeqIpAddr(dwNetCtrlIp);
            ptChan->m_tLogicChan.m_tRcvMediaCtrlChannel.SetPort(wCtrlPort);

            //初始码流发送
            if(!m_pcMtAdpNet->AudNetSndInit())
            {
                MtAdpLog(LOG_CRITICAL, "[OnMcuMtOpenLogicChanRsp] AudNetSndInit failed. close chan\n");    
                
                //mt－>mcu, close chan
                u8 byOut = 0;
                cServMsg.SetEventId(MT_MCU_CLOSELOGICCHNNL_NOTIF);
                cServMsg.CatMsgBody(&byOut, sizeof(byOut));
                SendMsg2Mcu(cServMsg);
                
                ptChan->Reset();                
                return;
            }
            
            MtAdpLog(LOG_INFO, "[OnMcuMtOpenLogicChanRsp] mt -> mcu open audio chan success.\n");
        }
        else if(MODE_VIDEO == byMediaType)
        {
            ptChan = &m_atChannel[VID_SND_INDEX];
            ptChan->m_byState = CHAN_CONNECTED;
            ptChan->m_tLogicChan.m_tRcvMediaChannel.SetNetSeqIpAddr(dwNetIp);
            ptChan->m_tLogicChan.m_tRcvMediaChannel.SetPort(wPort);
            ptChan->m_tLogicChan.m_tRcvMediaCtrlChannel.SetNetSeqIpAddr(dwNetCtrlIp);
            ptChan->m_tLogicChan.m_tRcvMediaCtrlChannel.SetPort(wCtrlPort);

            //初始码流发送
            if(!m_pcMtAdpNet->VidNetSndInit())
            {
                MtAdpLog(LOG_CRITICAL, "[OnMcuMtOpenLogicChanRsp] VidNetSndInit failed. close chan\n");            
                
                //mt－>mcu, close chan
                u8 byOut = 0;
                cServMsg.SetEventId(MT_MCU_CLOSELOGICCHNNL_NOTIF);
                cServMsg.CatMsgBody(&byOut, sizeof(byOut));
                SendMsg2Mcu(cServMsg);

                ptChan->Reset();                
                return;
            }

            MtAdpLog(LOG_INFO, "[OnMcuMtOpenLogicChanRsp] mt -> mcu open video chan success.\n");
        }
        else
        {
            MtAdpLog(LOG_CRITICAL, "[OnMcuMtOpenLogicChanRsp] invalid mediatype in openchan ack!\n");
            return;
        }

        MtAdpLog(LOG_INFO, "Mcu Rcv Media IpAddr<%s, %u>\n",
                 StrIPv4(dwNetIp), wPort); 
        MtAdpLog(LOG_INFO, "Mcu Rcv Media CtrlIpAddr<%s, %u>\n",
                 StrIPv4(dwNetCtrlIp), wCtrlPort);
        MtAdpLog(LOG_INFO, "Mt Snd Media IpAddr<%s, %u>\n",
                 StrIPv4(ptChan->m_tSndMediaChannel.GetNetSeqIpAddr()),
                 ptChan->m_tSndMediaChannel.GetPort());
        MtAdpLog(LOG_INFO, "Mt Snd Media CtrlIpAddr<%s, %u>\n",
                 StrIPv4(ptChan->m_tLogicChan.m_tSndMediaCtrlChannel.GetNetSeqIpAddr()),
                 ptChan->m_tLogicChan.m_tSndMediaCtrlChannel.GetPort());
    }
    //拒绝
    else
    {
        MtAdpLog(LOG_INFO, "[OnMcuMtOpenLogicChanRsp] mcu reject mt open logicchan. nack\n");

        if(MODE_AUDIO == byMediaType)
        {
            m_atChannel[AUD_SND_INDEX].Reset();
        }        
        else if(MODE_VIDEO == byMediaType)
        {
            m_atChannel[VID_SND_INDEX].Reset();
        }
        else
        {
            MtAdpLog(LOG_CRITICAL, "[OnMcuMtOpenLogicChanRsp] invalid mediatype in openchan nack!\n");
        }
    }
}

/*=============================================================================
  函 数 名： OnMcuMtOpenLogicChanReq
  功    能： mcu 打开逻辑信道请求处理
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnMcuMtOpenLogicChanReq(const CMessage *pcMsg)
{
    if(STATE_NORMAL != CurState())
    {
        MtAdpLog(LOG_CRITICAL, "[OnMcuMtOpenLogicChanReq] instace state<%u> is invalid. ignore it\n",
                 CurState());
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    if(cServMsg.GetMsgBodyLen() < sizeof(TLogicalChannel))
    {
        MtAdpLog(LOG_CRITICAL, "[OnMcuMtOpenLogicChanReq] msg body is invalid. ignore it\n");
        return;
    }

    TLogicalChannel tLogicChan = *(TLogicalChannel *)cServMsg.GetMsgBody();
    u16 wPort = AssignSwitchPort();

    u8 byMediaType = tLogicChan.GetMediaType();
    u8 byChanType = tLogicChan.GetChannelType();
    if(MODE_AUDIO == byMediaType)
    {
        if(MediaTypeOut2In(m_tLocalMuxMode.m_byAudType) != byChanType)
        {
            MtAdpLog(LOG_CRITICAL, "[OnMcuMtOpenLogicChanReq] Audio ChanType<%d> is invalid. nack\n",
                     byChanType);

            cServMsg.SetEventId(cServMsg.GetEventId() + 2);
            SendMsg2Mcu(cServMsg);
            return;
        }

        tLogicChan.m_tRcvMediaChannel.SetNetSeqIpAddr(g_cMtAdpApp.m_dwLocalIP);
        tLogicChan.m_tRcvMediaChannel.SetPort(wPort + AUD_RCV_PORT);
        tLogicChan.m_tRcvMediaCtrlChannel.SetNetSeqIpAddr(g_cMtAdpApp.m_dwLocalIP);
        tLogicChan.m_tRcvMediaCtrlChannel.SetPort(wPort + AUD_RCV_CTRLPORT);

        //保存信道信息
        m_atChannel[AUD_RCV_INDEX].m_tLogicChan = tLogicChan;
        m_atChannel[AUD_RCV_INDEX].m_byState = CHAN_CONNECTED;

        //初始码流接收
        if(!m_pcMtAdpNet->AudNetRcvInit())
        {
            MtAdpLog(LOG_CRITICAL, "[OnMcuMtOpenLogicChanReq] AudNetRcvInit failed. nack\n");
            
            cServMsg.SetEventId(cServMsg.GetEventId() + 2);
            SendMsg2Mcu(cServMsg);

            m_atChannel[AUD_RCV_INDEX].Reset();

            return;
        }
        
        MtAdpLog(LOG_INFO, "[OnMcuMtOpenLogicChanReq] mcu -> mt open audio chan success.\n");
    }
    else if(MODE_VIDEO == byMediaType)
    {
        if(MediaTypeOut2In(m_tLocalMuxMode.m_byVidType) != byChanType)
        {
            MtAdpLog(LOG_CRITICAL, "[OnMcuMtOpenLogicChanReq] Video ChanType<%d> is invalid. nack\n",
                     byChanType);
            
            cServMsg.SetEventId(cServMsg.GetEventId() + 2);
            SendMsg2Mcu(cServMsg);
            return;
        }

        tLogicChan.m_tRcvMediaChannel.SetNetSeqIpAddr(g_cMtAdpApp.m_dwLocalIP);
        tLogicChan.m_tRcvMediaChannel.SetPort(wPort + VID_RCV_PORT);
        tLogicChan.m_tRcvMediaCtrlChannel.SetNetSeqIpAddr(g_cMtAdpApp.m_dwLocalIP);
        tLogicChan.m_tRcvMediaCtrlChannel.SetPort(wPort + VID_RCV_CTRLPORT);

        //保存信道信息
        m_atChannel[VID_RCV_INDEX].m_tLogicChan = tLogicChan;
        m_atChannel[VID_RCV_INDEX].m_byState = CHAN_CONNECTED;

        //初始码流接收
        if(!m_pcMtAdpNet->VidNetRcvInit())
        {
            MtAdpLog(LOG_CRITICAL, "[OnMcuMtOpenLogicChanReq] VidNetRcvInit failed. nack\n");
            
            cServMsg.SetEventId(cServMsg.GetEventId() + 2);
            SendMsg2Mcu(cServMsg);
            
            m_atChannel[VID_RCV_INDEX].Reset();
            
            return;
        }

        MtAdpLog(LOG_INFO, "[OnMcuMtOpenLogicChanReq] mcu -> mt open video chan success.\n");
    }
    else if(MODE_DATA == byMediaType && MEDIA_TYPE_MMCU == byChanType)
    {
        //打开级联通道                
        cServMsg.SetMsgBody((u8 *)&tLogicChan, sizeof(tLogicChan));
        cServMsg.SetEventId(cServMsg.GetEventId() + 1);
        SendMsg2Mcu(cServMsg);
        MtAdpLog(LOG_INFO, "[OnMcuMtOpenLogicChanReq] mcu -> mt open cascade chan success.\n");            
        
        if(m_bMasterMcu)
        {
            //发送net mt notif，mcu自己
            TMcuMcuMtInfo tMtInfo;
            memset(&tMtInfo, 0, sizeof(tMtInfo));
            
            tMtInfo.m_byAudioIn = m_atChannel[AUD_SND_INDEX].m_tLogicChan.GetChannelType();
            tMtInfo.m_byAudioOut = m_atChannel[AUD_RCV_INDEX].m_tLogicChan.GetChannelType();
            tMtInfo.m_byVideoIn = m_atChannel[VID_SND_INDEX].m_tLogicChan.GetChannelType();
            tMtInfo.m_byVideoOut = m_atChannel[VID_RCV_INDEX].m_tLogicChan.GetChannelType();
            tMtInfo.m_byVideo2In = MEDIA_TYPE_NULL;
            tMtInfo.m_byVideo2Out = MEDIA_TYPE_NULL;
            tMtInfo.m_byMtType = MT_TYPE_MT;
            tMtInfo.m_tMt.SetMt(m_byMtId, 0);
            tMtInfo.m_byIsConnected = 1;
            
            if(m_tLocalMuxMode.m_byLSDType > lsdtype_Begin && m_tLocalMuxMode.m_byLSDType < lsdtype_End)
            {
                tMtInfo.m_byIsFECCEnable = 1;
            }
            
            cServMsg.SetEventId(MCU_MCU_NEWMT_NOTIF);
            cServMsg.SetMsgBody((u8*)&tMtInfo, sizeof(tMtInfo));
            SendMsg2Mcu(cServMsg);  
        }    
        
        return;
    }
    else//h224, t120 not support
    {
        MtAdpLog(LOG_CRITICAL, "[OnMcuMtOpenLogicChanReq] MediaType<%d> ChanType<%d> is invalid. nack\n",
                 byMediaType, byChanType);
        
        cServMsg.SetEventId(cServMsg.GetEventId() + 2);
        SendMsg2Mcu(cServMsg);
        return;
    }

    //ack
    cServMsg.SetMsgBody((u8 *)&tLogicChan, sizeof(tLogicChan));
    cServMsg.SetEventId(cServMsg.GetEventId() + 1);
    SendMsg2Mcu(cServMsg);

    MtAdpLog(LOG_INFO, "Mt Rcv Media IpAddr<%s, %u>\n",
             StrIPv4(tLogicChan.m_tRcvMediaChannel.GetNetSeqIpAddr()), 
             tLogicChan.m_tRcvMediaChannel.GetPort()); 
    MtAdpLog(LOG_INFO, "Mt Rcv Media CtrlIpAddr<%s, %u>\n",
             StrIPv4(tLogicChan.m_tRcvMediaCtrlChannel.GetNetSeqIpAddr()), 
             tLogicChan.m_tRcvMediaCtrlChannel.GetPort());  
    MtAdpLog(LOG_INFO, "Mcu Snd Media CtrlIpAddr<%s, %u>\n",
             StrIPv4(tLogicChan.m_tSndMediaCtrlChannel.GetNetSeqIpAddr()), 
             tLogicChan.m_tSndMediaCtrlChannel.GetPort());    
}

/*=============================================================================
  函 数 名： OnMcuMtChanOnCmd
  功    能： 开启信道处理
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnMcuMtChanOnCmd(const CMessage *pcMsg)
{
    if(STATE_NORMAL != CurState())
    {
        MtAdpLog(LOG_CRITICAL, "[OnMcuMtChanOnCmd] Instance state<%u> is invalid. ignore it\n",
                 CurState());
        return;
    }
}

/*=============================================================================
  函 数 名： OnMcuMtChanOffCmd
  功    能： 停止信道处理
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnMcuMtChanOffCmd(const CMessage *pcMsg)
{
    if(STATE_NORMAL != CurState())
    {
        MtAdpLog(LOG_CRITICAL, "[OnMcuMtChanOffCmd] Instance state<%u> is invalid. ignore it\n",
                 CurState());
        return;
    }
}

/*=============================================================================
  函 数 名： OnMcuMtCloseChanCmd
  功    能： 关闭信道处理
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnMcuMtCloseChanCmd(const CMessage *pcMsg)
{
    if(STATE_NORMAL != CurState())
    {
        MtAdpLog(LOG_CRITICAL, "[OnMcuMtCloseChanCmd] Instance state<%u> is invalid. ignore it\n",
                 CurState());
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    if(cServMsg.GetMsgBodyLen() < sizeof(TLogicalChannel) + 1)
    {
        MtAdpLog(LOG_CRITICAL, "[OnMcuMtCloseChanCmd] msg body is invalid. ignore it\n");
        return;
    }

    TLogicalChannel *ptLogicChan = (TLogicalChannel *)(cServMsg.GetMsgBody());
    
    //信道打开方向（1：mcu->mt，0：mt->mcu）
    u8 byDirect = *(cServMsg.GetMsgBody() + sizeof(TLogicalChannel));

    u8 byChanId = 0;
    u8 byMediaType = ptLogicChan->GetMediaType();
    u8 byChanType = ptLogicChan->GetChannelType();
    
    if(MODE_AUDIO == byMediaType)
    {
        if(MediaTypeOut2In(m_tLocalMuxMode.m_byAudType) != byChanType)
        {
            MtAdpLog(LOG_CRITICAL, "[OnMcuMtCloseChanCmd] ChanType<%d> is invalid.\n", 
                     byChanType);
            return;
        }
        
        if(0 == byDirect)
        {
            byChanId = AUD_SND_INDEX;

            //关闭码流发送
            if(!m_pcMtAdpNet->AudNetSndQuit())
            {
                MtAdpLog(LOG_CRITICAL, "[OnMcuMtCloseChanCmd] AudNetSndQuit failed\n");
            }
        }
        else
        {
            byChanId = AUD_RCV_INDEX;

            //关闭码流接收
            if(!m_pcMtAdpNet->AudNetRcvQuit())
            {
                MtAdpLog(LOG_CRITICAL, "[OnMcuMtCloseChanCmd] AudNetRcvQuit failed\n");
            }
        }
        m_atChannel[byChanId].Reset();        

        MtAdpLog(LOG_INFO, "[OnMcuMtCloseChanCmd] Chan<%d> is closed.\n", byChanId);
    }
    else if(MODE_VIDEO == byMediaType)
    {
        if(MediaTypeOut2In(m_tLocalMuxMode.m_byVidType) != byChanType)
        {
            MtAdpLog(LOG_CRITICAL, "[OnMcuMtCloseChanCmd] ChanType<%d> is invalid.\n", 
                byChanType);
            return;
        }

        if(0 == byDirect)
        {
            byChanId = VID_SND_INDEX;

            //关闭码流发送
            if(!m_pcMtAdpNet->VidNetSndQuit())
            {
                MtAdpLog(LOG_CRITICAL, "[OnMcuMtCloseChanCmd] VidNetSndQuit failed\n");
            }
        }
        else
        {
            byChanId = VID_RCV_INDEX;

            //关闭码流接收
            if(!m_pcMtAdpNet->VidNetRcvQuit())
            {
                MtAdpLog(LOG_CRITICAL, "[OnMcuMtCloseChanCmd] VidNetRcvQuit failed\n");
            }
        }
        m_atChannel[byChanId].Reset();
        
        MtAdpLog(LOG_INFO, "[OnMcuMtCloseChanCmd] Chan<%d> is closed.\n", byChanId);
    }
    else
    {
        MtAdpLog(LOG_CRITICAL, "[OnMcuMtCloseChanCmd] MediaType<%d> is invalid.\n", 
                 byMediaType);
    }
}

/*=============================================================================
  函 数 名： OnMcuMtFastUpdatePicCmd
  功    能： 强制关键帧
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnMcuMtFastUpdatePicCmd(const CMessage *pcMsg)
{
    if(STATE_NORMAL != CurState())
    {
        MtAdpLog(LOG_CRITICAL, "[OnMcuMtFastUpdatePicCmd] Instance state(%u) is invalid. ignore it\n",
                 CurState());
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    u8 byMode = *cServMsg.GetMsgBody();

    if(MODE_VIDEO != byMode)
    {
        MtAdpLog(LOG_CRITICAL, "[OnMcuMtFastUpdatePicCmd] invalid mode<%d>. ignore it\n", byMode);
        return;
    }

    //发送强制关键帧bas码
    u16 wRet = m_pcH320Stack->SendCommand(h_ci_VidCommandUpdate, NULL, 0);
    if(KDVH320_OK != wRet)
    {
        MtAdpLog(LOG_WARNING, "[OnMcuMtFastUpdatePicCmd] send fast update cmd failed. err no<%d>\n", wRet);
    }
}

/*=============================================================================
  函 数 名： OnFlowCtrlCmd
  功    能： 码流控制
  算法实现： 开始/停止码流接收
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnFlowCtrlCmd(const CMessage *pcMsg)
{
    if(STATE_NORMAL != CurState())
    {
        MtAdpLog(LOG_CRITICAL, "[OnFlowCtrlCmd] Instance state(%u) is invalid. ignore it\n",
                 CurState());
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    if(cServMsg.GetMsgBodyLen() < sizeof(TLogicalChannel))
    {
        MtAdpLog(LOG_CRITICAL, "[OnFlowCtrlCmd] msg body is invalid. ignore it\n");
        return;
    }

    TLogicalChannel *ptChan = (TLogicalChannel *)cServMsg.GetMsgBody();

    //start or stop
    BOOL32 bStart = (0 == ptChan->GetFlowControl()) ? FALSE : TRUE;
    u8 byType = ptChan->GetMediaType();
    if(MODE_AUDIO == byType)
    {
        MtAdpLog(LOG_INFO, "[OnFlowCtrlCmd] audio snd start<%d> success.\n", bStart);
    }
    else if(MODE_VIDEO == byType)
    {
        MtAdpLog(LOG_INFO, "[OnFlowCtrlCmd] video snd start<%d> success.\n", bStart);
    }
    else
    {
        MtAdpLog(LOG_CRITICAL, "[OnFlowCtrlCmd] media type<%d> is invalid.\n", byType);
    }
}

/*=============================================================================
  函 数 名： OnYouAreSeeingNotif
  功    能： 通知终端选看的终端源
  算法实现： 开始码流接收
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnYouAreSeeingNotif(const CMessage *pcMsg)
{
    if(STATE_NORMAL != CurState())
    {
        MtAdpLog(LOG_CRITICAL, "[OnYouAreSeeingNotif] Instance state(%u) is invalid. ignore it\n",
                 CurState());
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    if(cServMsg.GetMsgBodyLen() < sizeof(TMt))
    {
        MtAdpLog(LOG_CRITICAL, "[OnYouAreSeeingNotif] msg body is invalid. ignore it\n");
        return;
    }

    TMt *ptMt = (TMt *)cServMsg.GetMsgBody();  
    
    //send TIN
    u8 abyParam[2];
    if(ptMt->GetMcuId() == m_byMtId)
    {
        abyParam[0] = GetRemoteMcuId();
    }
    else
    {
        abyParam[0] = GetLocalMcuId();
    }
    
    abyParam[1] = ptMt->GetMtId();

    if(KDVH320_OK == m_pcH320Stack->SendCommand(KDVH320_CI_VIN, abyParam, sizeof(abyParam)))
    {
        MtAdpLog(LOG_INFO, "[OnYouAreSeeingNotif] send vin<%d><%d> to remote.\n", abyParam[0], abyParam[1]);
    }   
  
    MtAdpLog(LOG_INFO, "[OnYouAreSeeingNotif] mt<%d, %d> start rcv and see mt<%d, %d>.\n",
             g_cMtAdpApp.m_byMcuAId, m_byMtId, ptMt->GetMcuId(), ptMt->GetMtId()); 
}

/*=============================================================================
  函 数 名： OnMcuMtChairNotif
  功    能： 指定/取消主席
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnMcuMtChairNotif(const CMessage *pcMsg)
{
    if(STATE_NORMAL != CurState())
    {
        MtAdpLog(LOG_CRITICAL, "[OnMcuMtChairNotif] Instance state<%u> is invalid. ignore it\n",
                 CurState());
        return;
    }

    //指定主席
/*    if(MCU_MT_SPECCHAIRMAN_NOTIF == pcMsg->event)
    {
        u16 wRet = m_pcH320Stack->SendCommand(KDVH320_CI_CCA, NULL, 0);
        if(KDVH320_OK != wRet)
        {
            MtAdpLog(LOG_WARNING, "[OnMcuMtChairNotif] send chair indicate token failed. err no<%d>\n", wRet);
        }
    }
    //取消主席
    else
    {
        u16 wRet = m_pcH320Stack->SendCommand(KDVH320_CI_CIS, NULL, 0);
        if(KDVH320_OK != wRet)
        {
            MtAdpLog(LOG_WARNING, "[OnMcuMtChairNotif] send chair indicate Stopped-using-token failed. err no<%d>\n", wRet);
        }
    }*/
}

/*=============================================================================
  函 数 名： OnMcuMtSpeakerNotif
  功    能： 指定/取消发言
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnMcuMtSpeakerNotif(const CMessage *pcMsg)
{
    if(STATE_NORMAL != CurState())
    {
        MtAdpLog(LOG_CRITICAL, "[OnMcuMtSpeakerNotif] Instance state<%u> is invalid. ignore it\n",
                 CurState());
        return;
    }
    
    //指定发言(级联时指定mcu发言)
    if(MCU_MT_SPECSPEAKER_NOTIF == pcMsg->event)
    {
    }
    //取消发言
    else
    {        
    }
}

/*=============================================================================
  函 数 名： OnMcuMtSelNotif
  功    能： 指定/取消选看
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnMcuMtSelNotif(const CMessage *pcMsg)
{
    if(STATE_NORMAL != CurState())
    {
        MtAdpLog(LOG_CRITICAL, "[OnMcuMtSelNotif] Instance state<%u> is invalid. ignore it\n",
                 CurState());
        return;
    }
    
    //指定选看
    if(MCU_MT_SEENBYOTHER_NOTIF == pcMsg->event)
    {
    }
    //取消选看
    else
    {
    }
}

/*=============================================================================
  函 数 名： OnMcuMtSelRsp
  功    能： 
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnMcuMtSelRsp(const CMessage *pcMsg)
{
    if(STATE_NORMAL != CurState())
    {
        MtAdpLog(LOG_CRITICAL, "[OnMcuMtSelRsp] Instance state<%u> is invalid. ignore it\n",
                 CurState());
        return;
    }

    //ack
    if(MCU_MT_STARTSELMT_ACK == pcMsg->event)
    {
        u8 abyParam[2];
        abyParam[0] = m_tSpySrcMt.GetMcuId();
        abyParam[1] = m_tSpySrcMt.GetMtId();
        u16 wRet;
        
        if(!m_bMasterMcu&&m_bRemoteTypeMcu)
        {
            wRet = m_pcH320Stack->SendCommand(KDVH320_CI_VCS, abyParam, sizeof(abyParam));
            if(KDVH320_OK == wRet)
            {
                MtAdpLog( LOG_INFO, "[OnMcuMtSelRsp] send cmd KDVH320_CI_VCS to remote success.\n" );
            }
            else
            {
                MtAdpLog( LOG_CRITICAL, "[OnMcuMtSelRsp] send cmd KDVH320_CI_VCS to remote failed. ret:%d\n", wRet);
            }
        }
                
        wRet = m_pcH320Stack->SendCommand(KDVH320_CI_VIN, abyParam, sizeof(abyParam));
        if(KDVH320_OK == wRet)
        {
            MtAdpLog( LOG_INFO, "[OnMcuMtSelRsp] send cmd KDVH320_CI_VIN to remote success. \n");
        }
        else
        {
            MtAdpLog( LOG_CRITICAL, "[OnMcuMtSelRsp] send cmd KDVH320_CI_VIN to remote failed. ret:%d\n", wRet);
        }
    }
    //nack
    else
    {
    }
}

/*=============================================================================
  函 数 名： OnMcuMtMuteDumbCmd
  功    能： 静音/哑音
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnMcuMtMuteDumbCmd(const CMessage *pcMsg)
{
    if(STATE_NORMAL != CurState())
    {
        MtAdpLog(LOG_CRITICAL, "[OnMcuMtMuteDumbCmd] Instance state<%u> is invalid. ignore it\n",
                 CurState());
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    if(cServMsg.GetMsgBodyLen() < sizeof(TMt) + sizeof(u8))
    {
        MtAdpLog(LOG_CRITICAL, "[OnMcuMtMuteDumbCmd] msg body is invalid. ignore it\n");
        return;
    }

    u8 byMode = *(cServMsg.GetMsgBody() + sizeof(TMt));
    BOOL32 bStart = (0 == byMode) ? TRUE : FALSE;
    
    //静音
    if(MCU_MT_MTMUTE_CMD == pcMsg->event)
    {
//        m_pcMtAdpNet->AudNetSndStart(bStart);
    }
    //哑音
    else
    {
//        m_pcMtAdpNet->AudNetSndStart(bStart);
    }
}

/*=============================================================================
  函 数 名： OnMcuMtMtListRsp
  功    能： 终端列表响应
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnMcuMtMtListRsp(const CMessage *pcMsg)
{
    if(STATE_NORMAL != CurState())
    {
        MtAdpLog(LOG_CRITICAL, "[OnMcuMtMtListRsp] Instance state<%u> is invalid. ignore it\n",
                 CurState());
        return;
    }

    if(MCU_MT_JOINEDMTLIST_NACK == pcMsg->event) //nack
    {
        MtAdpLog(LOG_WARNING, "[OnMcuMtMtListRsp] mcu refused mtlist request. Nack.\n");        
    }
    else    //ack or notify
    {
        CServMsg cMsg(pcMsg->content, pcMsg->length);
        u8 byMtNum = cMsg.GetMsgBodyLen() / sizeof(TMt);
        if(byMtNum <= 0 || byMtNum > MAXNUM_CONF_MT)
        {
            MtAdpLog(LOG_CRITICAL, "[OnMcuMtMtListRsp] msg body is invalid, MtNum %u .\n", byMtNum);
            return;
        }
        

        //将终端列表发给对端
        TH320TIL tTIL;
        tTIL.m_byMcuId = GetLocalMcuId();        
        
        u8 byMtIndex = 0;
        TMt *ptMt = NULL;			
        for(s32 i = 0; i < byMtNum; i++)
        {
            ptMt = (TMt*)(cMsg.GetMsgBody() + sizeof(TMt) * i);
            
            //N－1 模式
            if(ptMt->GetMtId() == m_byMtId) 
            {
                continue;
            }

            tTIL.m_abyMtId[byMtIndex] = ptMt->GetMtId();
            byMtIndex++;
        }
        tTIL.m_byMtNum = byMtIndex;

        u16 wRet = m_pcH320Stack->SendCommand(KDVH320_CI_TIL, (u8 *)&tTIL, sizeof(tTIL));
        if(KDVH320_OK != wRet)
        {
            MtAdpLog(LOG_CRITICAL, "[OnMcuMtMtListRsp] send cmd KDVH320_CI_TIL failed. ret<%d>\n", wRet);
        }
        else
        {
            MtAdpLog(LOG_INFO, "[OnMcuMtMtListRsp] mcu send mtlist success. cmd id :%u.\n", pcMsg->event);
            MtAdpLog(LOG_INFO, "[OnMcuMtMtListRsp] mcu id :%d\n", tTIL.m_byMcuId);
            for(s32 i = 0; i < tTIL.m_byMtNum; i++)
            {
                MtAdpLog(LOG_INFO, "[OnMcuMtMtListRsp] mt id :%d\n", tTIL.m_abyMtId[i]);
            }
        }

        //send TIE
        wRet = m_pcH320Stack->SendCommand(KDVH320_CI_TIE, NULL, 0);
        if(KDVH320_OK != wRet)
        {
            MtAdpLog(LOG_CRITICAL, "[OnMcuMtMtListRsp] send cmd KDVH320_CI_TIE failed. ret<%d>\n", wRet);
        }
        else
        {
            MtAdpLog(LOG_INFO, "[OnMcuMtMtListRsp] send cmd KDVH320_CI_TIE success.\n");
        }
    }
}

/*=============================================================================
  函 数 名： OnMcuMtGetMtAliasRsp
  功    能： 终端别名响应
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnMcuMtGetMtAliasRsp(const CMessage *pcMsg)
{
    if(STATE_NORMAL != CurState())
    {
        MtAdpLog(LOG_CRITICAL, "[OnMcuMtGetMtAliasRsp] Instance state<%u> is invalid. ignore it\n",
            CurState());
        return;
    }

    if(MCU_MT_GETMTALIAS_ACK == pcMsg->event)
    {
        CServMsg cMsg( pcMsg->content, pcMsg->length );
        if(cMsg.GetMsgBodyLen() < sizeof(TMt) + sizeof(TMtAlias))
        {
            MtAdpLog(LOG_CRITICAL, "[OnMcuMtGetMtAliasRsp] invalid msg body, len<%d>", cMsg.GetMsgBodyLen());
            return;
        }
        
        //把终端别名发给对端(TIP)
        TMt  *ptTMt = (TMt*)cMsg.GetMsgBody();
        TMtAlias *ptAlias = (TMtAlias*)(cMsg.GetMsgBody() + sizeof(TMt));
        
        TH320MtExt tMtExt;
        tMtExt.m_tMt.m_byMcuId = GetLocalMcuId();
        tMtExt.m_tMt.m_byMtId = ptTMt->GetMtId();
        if(ptAlias->m_achAlias[0])
        {
            astrncpy(tMtExt.m_szAlias, ptAlias->m_achAlias, MAXLEN_EPALIAS, MAXLEN_ALIAS);
        }
        else
        {            
            u32 dwIP = htonl(ptAlias->m_tTransportAddr.GetIpAddr());
            strcpy(tMtExt.m_szAlias, StrIPv4(dwIP));
        }
        
        u16 wRet = m_pcH320Stack->SendCommand(KDVH320_CI_TIP, (u8 *)&tMtExt, sizeof(tMtExt));
        if(KDVH320_OK != wRet)
        {
            MtAdpLog(LOG_CRITICAL, "[OnMcuMtMtListRsp] send cmd KDVH320_CI_TIP failed. ret<%d>\n", wRet);
        }
        else
        {
            MtAdpLog(LOG_INFO, "[OnMcuMtGetMtAliasRsp] mcu response mtalias request. Ack.\n");
        }
    }
    else
    {
        MtAdpLog(LOG_WARNING, "[OnMcuMtGetMtAliasRsp] mcu refused mtalias request. Nack.\n");
    }
}

/*=============================================================================
  函 数 名： OnSetInReq
  功    能： 级联选看或发言
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnSetInReq(const CMessage *pcMsg)
{
    if(STATE_NORMAL != CurState())
    {
        MtAdpLog(LOG_CRITICAL, "[OnSetInReq] Instance state(%u) is invalid. ignore it\n",
                 CurState());
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    if(cServMsg.GetMsgBodyLen() != sizeof(TMcuMcuReq)+sizeof(TSetInParam)+sizeof(u8))
    {
        MtAdpLog(LOG_CRITICAL, "[OnSetInReq] invalid msg body!\n");
        return;
    }

    TSetInParam *ptParam = (TSetInParam *)(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq));
    u8 byMode = *(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq)+sizeof(TSetInParam));    

    if(m_bMasterMcu)
    {
        u16 wRet;
        u8 abyParam[2];
        if(1 == m_byMtId)
        {
            abyParam[0] = DEF_SMCUID;
        }
        else
        {
            abyParam[0] = m_byMtId;
        }        
        abyParam[1] = ptParam->m_tMt.GetMtId();

        if(0 == m_tConfTir.m_tChairTokenMt.m_byMtId)
        {
            MtAdpLog(LOG_CRITICAL, "[OnSetInReq] no chair in conf!\n"); 
        }

        if(SWITCH_MODE_BROADCAST == byMode)//speaker
        {
            if(m_byMtId == ptParam->m_tMt.GetMcuId())
            {
                wRet = m_pcH320Stack->SendCommand(KDVH320_CI_VCB, abyParam, sizeof(abyParam));
                if(KDVH320_OK == wRet)
                {
                    MtAdpLog(LOG_INFO, "[OnSetInReq] send VCB<%d><%d> to remote success\n",
                        abyParam[0], abyParam[1]);

                    m_pcH320Stack->SendCommand(KDVH320_CI_VIN, abyParam, sizeof(abyParam));
                    MtAdpLog(LOG_INFO, "[OnSetInReq] send VIN<%d><%d> to remote", abyParam[0], abyParam[1]);

                    cServMsg.SetEventId(cServMsg.GetEventId()+1);
                    SendMsg2Mcu(cServMsg);
                    return;
                }
                else
                {
                    MtAdpLog(LOG_CRITICAL, "[OnSetInReq] send VCB to remote failed, ret<%d>!\n", wRet);                    
                }
            }
            else
            {
                MtAdpLog(LOG_CRITICAL, "[OnSetInReq] speaker mt<%d><%d> is not mine!\n", 
                    ptParam->m_tMt.GetMcuId(), ptParam->m_tMt.GetMtId());
            }
        }
        else if(SWITCH_MODE_SELECT == byMode)//sel mt
        {
            if(m_byMtId == ptParam->m_tMt.GetMcuId())
            {
                wRet = m_pcH320Stack->SendCommand(KDVH320_CI_VCS, abyParam, sizeof(abyParam));
                if(KDVH320_OK == wRet)
                {
                    MtAdpLog(LOG_INFO, "[OnSetInReq] send VCS<%d><%d> to remote success\n",
                        abyParam[0], abyParam[1]);                      
                 
                    return;
                }
                else
                {
                    MtAdpLog(LOG_CRITICAL, "[OnSetInReq] send VCS to remote failed, ret<%d>!\n", wRet);
                }
            }
            else
            {
                MtAdpLog(LOG_CRITICAL, "[OnSetInReq] select mt<%d><%d> is not mine!\n", 
                    ptParam->m_tMt.GetMcuId(), ptParam->m_tMt.GetMtId());
            }
        }
        else
        {
            MtAdpLog(LOG_CRITICAL, "[OnSetInReq] invalid switch mode :%d!\n", byMode);
        }
    }

    cServMsg.SetEventId(cServMsg.GetEventId()+2);
    SendMsg2Mcu(cServMsg);
    return;
}

/*=============================================================================
  函 数 名： OnMcuMtOnlineChangeNotif
  功    能： 终端入会离会通知, 更新对端终端列表
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnMcuMtOnlineChangeNotif(const CMessage *pcMsg)
{
    if(STATE_NORMAL != CurState())
    {        
        return;
    }

    MtAdpLog(LOG_INFO, "[OnMcuMtOnlineChangeNotif] receive mt online status change notify\n");
    
    if(!m_bRemoteTypeMcu || 1 == g_bySndMtList)
    {
        CServMsg cMsg;
        cMsg.SetEventId(MT_MCU_JOINEDMTLIST_REQ);
        SendMsg2Mcu(cMsg);
    }    
}

/*=============================================================================
函 数 名： OnGetBitrateInfo
功    能： mcu查询码率消息
算法实现： 
全局变量： 
参    数： const CMessage *pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/5/24  4.0			许世林                  创建
=============================================================================*/
void CMtAdpInst::OnGetBitrateInfo(const CMessage *pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    cServMsg.SetEventId(pcMsg->event+1);
    SendMsg2Mcu(cServMsg);

    TMtBitrate tBitrate;
    tBitrate.SetSendBitRate(m_wVideoRate);
    tBitrate.SetRecvBitRate(m_wVideoRate);
    cServMsg.SetEventId(MT_MCU_GETBITRATEINFO_NOTIF);
    cServMsg.SetMsgBody((u8*)&tBitrate, sizeof(tBitrate));
    SendMsg2Mcu(cServMsg);

    return;
}

/*=============================================================================
  函 数 名： OnConfInfoNotif
  功    能： 会议信息通知处理函数
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnConfInfoNotif(const CMessage *pcMsg)
{
    if(STATE_NORMAL != CurState())
    {
        MtAdpLog(LOG_CRITICAL, "[OnConfInfoNotif] Instance state<%u> is invalid. ignore it\n",
                 CurState());
        return;
    }

    TMt tChairMt;
    TMt tSpeakerMt;
    u16 wRet;

    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    if(MCU_MT_SIMPLECONF_NOTIF == cServMsg.GetEventId())
    {
        if(cServMsg.GetMsgBodyLen() != sizeof(TSimConfInfo))
        {
            MtAdpLog(LOG_CRITICAL, "[OnSimConfInfoNotif] msg body is invalid. ignore it\n");
            return;
        }
        
        TSimConfInfo *ptConfInfo = (TSimConfInfo *)cServMsg.GetMsgBody();
        tChairMt = ptConfInfo->m_tChairMan;
        tSpeakerMt = ptConfInfo->m_tSpeaker;
    }
    else
    {
        if(cServMsg.GetMsgBodyLen() != sizeof(TConfInfo))
        {
            MtAdpLog(LOG_CRITICAL, "[OnConfInfoNotif] msg body is invalid. ignore it\n");
            return;
        }
        
        TConfInfo *ptConfInfo = (TConfInfo *)cServMsg.GetMsgBody();

        tChairMt = ptConfInfo->GetChairman(); 
        tSpeakerMt = ptConfInfo->GetSpeaker();
    }      
    
    //for master mcu
    if(m_bMasterMcu)
    {
        if(!tChairMt.IsNull())
        {
            if(tChairMt.GetMcuId() == m_byMtId)
            {
                m_tConfTir.m_tChairTokenMt.m_byMcuId = GetRemoteMcuId();
            }
            else
            {
                m_tConfTir.m_tChairTokenMt.m_byMcuId = GetLocalMcuId();
            }
            m_tConfTir.m_tChairTokenMt.m_byMtId = tChairMt.GetMtId();
            
            wRet = m_pcH320Stack->SendCommand(KDVH320_CI_TIR, (u8*)&m_tConfTir, sizeof(m_tConfTir));
            if(KDVH320_OK != wRet)
            {
                MtAdpLog(LOG_CRITICAL, "[OnConfInfoNotif] send TIR failed. err no<%d>\n", wRet);
            }
            else
            {
                MtAdpLog(LOG_INFO, "[OnConfInfoNotif] send TIR seccess, spec chair<%d><%d>. \n", 
                    m_tConfTir.m_tChairTokenMt.m_byMcuId, m_tConfTir.m_tChairTokenMt.m_byMtId);
            }
        }
        else
        {
            m_tConfTir.m_tChairTokenMt.m_byMcuId = 0;
            m_tConfTir.m_tChairTokenMt.m_byMtId = 0;
            
            wRet = m_pcH320Stack->SendCommand(KDVH320_CI_TIR, (u8*)&m_tConfTir, sizeof(m_tConfTir));
            if(KDVH320_OK != wRet)
            {
                MtAdpLog(LOG_CRITICAL, "[OnConfInfoNotif] send TIR failed. err no<%d>\n", wRet);
            }
            else
            {
                MtAdpLog(LOG_INFO, "[OnConfInfoNotif] send TIR seccess, cancel chair. \n");
            }
        }

        if(!tSpeakerMt.IsNull())
        {
            if(tSpeakerMt.GetMcuId() == m_byMtId)//下级发言在setin里面处理
            {
            }
            else//上级发言
            {
                u8 abyParam[2];
                abyParam[0] = GetLocalMcuId();
                abyParam[1] = tSpeakerMt.GetMtId();
                wRet = m_pcH320Stack->SendCommand(KDVH320_CI_VCB, abyParam, sizeof(abyParam));
                if(KDVH320_OK != wRet)
                {
                    MtAdpLog(LOG_CRITICAL, "[OnConfInfoNotif] send VCB failed. ret<%d>\n", wRet);
                }
                else
                {
                    MtAdpLog(LOG_INFO, "[OnConfInfoNotif] send VCB<%d><%d> seccess\n", abyParam[0], abyParam[1]);
                }
            }
        }
        else
        {
            wRet = m_pcH320Stack->SendCommand(KDVH320_CI_CancelVCB, NULL, 0);
            if(KDVH320_OK != wRet)
            {
                MtAdpLog(LOG_CRITICAL, "[OnConfInfoNotif] send cmd KDVH320_CI_CancelVCB failed, ret:%d\n", wRet);
            }
            else
            {
                MtAdpLog(LOG_INFO, "[OnConfInfoNotif] send cmd KDVH320_CI_CancelVCB success\n");
            }
        }
    }     
    //for slave mcu
    else
    {
        //是否有主席操作    
        if(!m_bChairMt && !tChairMt.IsNull())//new chair
        {
            wRet = m_pcH320Stack->SendCommand(KDVH320_CI_CCA, NULL, 0);
            if(KDVH320_OK != wRet)
            {
                MtAdpLog(LOG_WARNING, "[OnConfInfoNotif] send chair indicate token failed. err no<%d>\n", wRet);
            }
            else
            {
                MtAdpLog(LOG_INFO, "[OnConfInfoNotif] spec new chair seccess, mtid<%d>. \n", tChairMt.GetMtId());
                m_bChairMt = TRUE;
            }
        }
        else if(m_bChairMt && tChairMt.IsNull())//cancel chair
        {      
            wRet = m_pcH320Stack->SendCommand(KDVH320_CI_CIS, NULL, 0);
            if(KDVH320_OK != wRet)
            {
                MtAdpLog(LOG_WARNING, "[OnConfInfoNotif] send chair indicate Stopped-using-token failed. err no<%d>\n", wRet);
            }
            else
            {
                MtAdpLog(LOG_INFO, "[OnConfInfoNotif] cancel chair success. \n");
                m_bChairMt = FALSE;
            }                
        }
        else
        {        
        }
    }
}

/*=============================================================================
  函 数 名： OnMcuMtFeccCmd
  功    能： 远遥操作
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnMcuMtFeccCmd(const CMessage *pcMsg)
{
    if(STATE_NORMAL != CurState())
    {
        MtAdpLog(LOG_CRITICAL, "[OnMcuMtFeccCmd] Instance state<%u> is invalid. ignore it\n",
                 CurState());
        return;
    }

    //是否支持远遥
    if(m_tLocalMuxMode.m_byLSDType <= lsdtype_Begin ||
        m_tLocalMuxMode.m_byLSDType >= lsdtype_End)
    {
        MtAdpLog(LOG_CRITICAL, "[OnMcuMtFeccCmd] not open lsd channel. ignore it\n");
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    if(cServMsg.GetMsgBodyLen() < sizeof(TMt) + sizeof(u8))
    {
        MtAdpLog(LOG_CRITICAL, "[OnMcuMtFeccCmd] msg body is invalid. ignore it\n");
        return;
    }
    
    TMt *ptMt = (TMt *)cServMsg.GetMsgBody();
    u8 byParam = *(cServMsg.GetMsgBody() + sizeof(TMt));    
    u16 wRet = 0;    

    u8 byMcuId, byMtId;    

    //如果本端为从mcu，则远遥上级发言人;本端为主mcu, 远遥下级发言人. 根据VIN获取终端id
    if(m_bRemoteTypeMcu)
    {     
        if (!m_bMasterMcu)
        {
            //是否拥有远遥令牌
            if(!m_bHasLsdToken)
            {
                u8 byLsdRate = 0;
                if(lsdtype_4800 == m_tLocalMuxMode.m_byLSDType)
                {
                    byLsdRate = 3; //from h.243 table.3
                }
                else if(lsdtype_6400 == m_tLocalMuxMode.m_byLSDType)
                {
                    byLsdRate = 4;
                }
                else
                {
                    MtAdpLog(LOG_CRITICAL, "[OnMcuMtFeccCmd] not supported lsd type :%d, ignoring\n", m_tLocalMuxMode.m_byLSDType);
                    return;
                }
                
                wRet = m_pcH320Stack->SendCommand(KDVH320_CI_DCA_L, &byLsdRate, sizeof(byLsdRate));
                if(KDVH320_OK != wRet)
                {
                    MtAdpLog(LOG_WARNING, "[OnMcuMtFeccCmd] send KDVH320_CI_DCA_L failed. err no<%d>\n", wRet);
                }
                else
                {
                    MtAdpLog(LOG_INFO, "[OnMcuMtFeccCmd] send KDVH320_CI_DCA_L to remote success. \n");
                }   
                
                //启动lsd令牌释放定时器
                SetTimer(TIMER_LSDTOKEN_RELEASE, m_wLsdTokenHoldTime*1000);
                return;
            }    
            else
            {
                //启动lsd令牌释放定时器
                SetTimer(TIMER_LSDTOKEN_RELEASE, m_wLsdTokenHoldTime*1000);
            }
        }        
        
        byMcuId = m_tVinMt.GetMcuId();
        byMtId = m_tVinMt.GetMtId();
    }
    //对端为终端的情况
    else
    {
        byMcuId = GetLocalMcuId(); 
        byMtId = ptMt->GetMtId();
    }

    switch(pcMsg->event)
    {
    case MCU_MT_MTCAMERA_RCENABLE_CMD:
        {
            if(0 != byParam)
            {
                wRet = m_pcH320Stack->StartFECC();
                if(KDVH320_OK != wRet)
                {
                    MtAdpLog(LOG_CRITICAL, "[OnMcuMtFeccCmd] start fecc failed. err no<%d>\n", wRet);
                }
            }
            else
            {
                wRet = m_pcH320Stack->StopFECC();
                if(KDVH320_OK != wRet)
                {
                    MtAdpLog(LOG_CRITICAL, "[OnMcuMtFeccCmd] stop fecc failed. err no<%d>\n", wRet);
                }
            }
        }
        break;

    case MCU_MT_MTCAMERA_CTRL_CMD:
        {            
            u8 byAction = FeccActionIn2Out(byParam);
            if(feccreq_Invalid != byAction)
            {
                wRet = m_pcH320Stack->StartCamAction(byMcuId, byMtId, 
                                                   byAction, STARTACTION_TIMEOUT);

                if(KDVH320_OK != wRet)
                {
                    MtAdpLog(LOG_WARNING, "[OnMcuMtFeccCmd] fecc start action failed. err no<%d>\n", wRet);
                }
            }
            else
            {
                MtAdpLog(LOG_WARNING, "[OnMcuMtFeccCmd] invalid fecc start action param<%d>!\n",
                         byParam);
            }
        }
        break;

    case MCU_MT_MTCAMERA_CTRL_STOP:
        {
            u8 byAction = FeccActionIn2Out(byParam);
            if(feccreq_Invalid != byAction)
            {
                wRet = m_pcH320Stack->StopCamAction(byMcuId, byMtId, byAction);                
                if(KDVH320_OK != wRet)
                {
                    MtAdpLog(LOG_WARNING, "[OnMcuMtFeccCmd] fecc Stop action failed. err no<%d>\n", wRet);
                }
            }
            else
            {
                MtAdpLog(LOG_WARNING, "[OnMcuMtFeccCmd] invalid fecc Stop action param<%d>!\n",
                         byParam);
            }
        }
        break;

    case MCU_MT_MTCAMERA_SAVETOPOS_CMD:
        {
            if(byParam < 16)
            {
                wRet = m_pcH320Stack->StorePreset(byMcuId, byMtId, byParam);            
                if(KDVH320_OK != wRet)
                {
                    MtAdpLog(LOG_WARNING, "[OnMcuMtFeccCmd] fecc save to pos failed. err no<%d>\n", wRet);
                }
            }
            else
            {
                MtAdpLog(LOG_WARNING, "[OnMcuMtFeccCmd] invalid pos<%d>. (should < 16)\n", byParam);
            }
        }
        break;

    case MCU_MT_MTCAMERA_MOVETOPOS_CMD:
        {
            if(byParam < 16)
            {
                wRet = m_pcH320Stack->ActivatePreset(byMcuId, byMtId, byParam);            
                if(KDVH320_OK != wRet)
                {
                    MtAdpLog(LOG_WARNING, "[OnMcuMtFeccCmd] fecc move to pos failed. err no<%d>\n", wRet);
                }
            }
            else
            {
                MtAdpLog(LOG_WARNING, "[OnMcuMtFeccCmd] invalid pos<%d>. (should < 16)\n", byParam);
            }
        }
        break;

    case MCU_MT_SETMTVIDSRC_CMD:
        {
            if(byParam < 16)
            {
                wRet = m_pcH320Stack->SelectVideoSource(byMcuId, byMtId, byParam);            
                if(KDVH320_OK != wRet)
                {
                    MtAdpLog(LOG_WARNING, "[OnMcuMtFeccCmd] fecc sel video src failed. err no<%d>\n", wRet);
                }
                else
                {
                    //上报mtstatus
                    TMcu tMcu;
                    tMcu.SetMcu(m_byMtId);
                    TSMcuMtStatus tStatus;
                    tStatus.SetMt(m_byMtId, byMtId);
                    tStatus.SetCurVideo(byParam);
                    tStatus.SetCurAudio(1);
                    tStatus.SetIsEnableFECC(TRUE);
                    
                    cServMsg.SetEventId(MCU_MCU_MTSTATUS_NOTIF);
                    cServMsg.SetMsgBody((u8*)&tMcu, sizeof(tMcu));
                    cServMsg.CatMsgBody((u8*)&tStatus, sizeof(tStatus));
                    SendMsg2Mcu(cServMsg);
                }
            }
            else
            {
                MtAdpLog(LOG_WARNING, "[OnMcuMtFeccCmd] invalid video src<%d>. (should < 16)\n", byParam);
            }
        }
        break;

    default:
        break;
    }
}

/*=============================================================================
  函 数 名： AssignSwitchPort
  功    能： 为此实例对应的终端分配码流收发起始端口
  算法实现： 根据单板起始端口号、实例号和每个终端占用的端口个数分配
  全局变量： 
  参    数： 
  返 回 值： u32    [in] 起始端口
=============================================================================*/
u16 CMtAdpInst::AssignSwitchPort()
{
    return (SWITCH_STARTPORT + (GetInsID()-1)*PORTNUM_PERMT);
}

/*=============================================================================
  函 数 名： AssignTia
  功    能： assign tia
  算法实现： 
  全局变量： 
  参    数： TMt tMt
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::AssignTia(TMt &tMt)
{
    //mt
    if(!m_bRemoteTypeMcu)
    {
        if(m_bMasterMcu)
        {
            tMt.SetMcuId(1);
        }     
        else
        {
            tMt.SetMcuId(m_byLocalMcuId);
        }
        tMt.SetMtId(m_byMtId);
    }
    //slave mcu
    else
    {
        if(1 != m_byMtId)
        {
            tMt.SetMt(m_byMtId, 0);   
        }
        else
        {
            tMt.SetMt(DEF_SMCUID, 0);//temp
        }
    }
}

/*=============================================================================
  函 数 名： GetRemoteMcuId
  功    能： get remote ep mcu id
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： u8  
=============================================================================*/
u8  CMtAdpInst::GetRemoteMcuId()
{
    u8 byMcuId = 0;

    //mt
    if(!m_bRemoteTypeMcu)
    {
        if(m_bMasterMcu)
        {
            byMcuId = 1;
        }
        else
        {
            byMcuId = m_byLocalMcuId;
        }
    }
    else
    {
        if(m_bMasterMcu)
        {
            if(1 != m_byMtId)
            {
                byMcuId = m_byMtId;
            }
            else
            {
                byMcuId = DEF_SMCUID;
            }
        }
        else
        {
            byMcuId = m_byLocalMcuId;
        }
    } 
    
    return byMcuId;
}

/*=============================================================================
  函 数 名： GetLocalMcuId
  功    能： 
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： u8  
=============================================================================*/
u8  CMtAdpInst::GetLocalMcuId()
{
    u8 byMcuId = 0;
    
    if(m_bMasterMcu)
    {
        byMcuId = 1;
    }
    else
    {
        byMcuId = m_byLocalMcuId;
    }
    
    return byMcuId;
}


/*=============================================================================
  函 数 名： LoadConfig
  功    能： 加载配置文件信息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::LoadConfig()
{
    MtAdpLog(LOG_INFO, "[LoadConfig] loading mtadp config info......\n");
    
    s8 szPath[KDV_MAX_PATH] = {0};  
    sprintf(szPath, "%s/mtadpcfg.ini", DIR_CONFIG);

    
    s8 szSection[256];
    sprintf(szSection, "chan%d", GetInsID()-1);

    s32 nValue = 0;
    if(GetRegKeyInt(szPath, szSection, "IsMasterMcu", 0, &nValue))
    {
        //master or slave
        m_bMasterMcu = (nValue == 0) ? FALSE:TRUE;
        MtAdpLog(LOG_INFO, "[LoadConfig] IsMasterMcu :%d\n", m_bMasterMcu);
    }
    
    if(GetRegKeyInt(szPath, szSection, "AudioRcvDelayTime", 0, &nValue))
    {
        //set audio rcv delay time
        setaudiodelaytime(nValue);

        MtAdpLog(LOG_INFO, "[LoadConfig] AudioRcvDelayTime :%d\n", nValue);
    }

    if(GetRegKeyInt(szPath, szSection, "AudioSndDelayTime", 0, &nValue))
    {
        //set audio snd delay time
        sendaudiodelaytime(nValue);

        MtAdpLog(LOG_INFO, "[LoadConfig] AudioSndDelayTime :%d\n", nValue);
    }

    //lsd type: 0-close, 1-6400, 2-4800
    if(GetRegKeyInt(szPath, szSection, "LsdType", 1, &nValue))
    {
        if(0 == nValue || 1== nValue || 2 == nValue)
        {
            g_byLsdType = nValue;
        }        

        MtAdpLog(LOG_INFO, "[LoadConfig] LsdType :%d\n", g_byLsdType);
    }
    
    if(GetRegKeyInt(szPath, szSection, "IsSndMtList", 1, &nValue))
    {
        if(m_bMasterMcu) //只能在作从mcu时不发列表
        {
            g_bySndMtList = 1;
            MtAdpLog(LOG_INFO, "[LoadConfig] no mtlist only in slave mcu mode!!!\n");
        }
        else
        {
            g_bySndMtList = (nValue == 0) ? 0:1;
        }            
                
        MtAdpLog(LOG_INFO, "[LoadConfig] IsSndMtList :%d\n", g_bySndMtList);
    }

    if(GetRegKeyInt(szPath, szSection, "LsdTokenHoldTime", DEF_LSDTOKEN_RELEASE_TIME, &nValue))
    {
        m_wLsdTokenHoldTime = nValue;

        MtAdpLog(LOG_INFO, "[LoadConfig] LsdTokenHoldTime :%d\n", m_wLsdTokenHoldTime);
    }
}

/*=============================================================================
  函 数 名： OnCmdCIS
  功    能： 
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnCmdCIS()
{
    CServMsg cServMsg;
    MtAdpLog(LOG_INFO, "[OnCmdCIS] Chair Indicate Stopped-using-token\n");

    if(m_bChairMt)
    {
        u16 wRet = m_pcH320Stack->SendCommand(KDVH320_CI_CCR, NULL, 0);
        if(KDVH320_OK != wRet)
        {
            MtAdpLog(LOG_CRITICAL, "[OnCmdCIS] send cmd KDVH320_CI_CCR failed, ret<%d>\n", wRet);
        }
        else
        {
            MtAdpLog( LOG_INFO, "[OnCmdCIS] send KDVH320_CI_CCR to remote.\n");
        }
        
        //send to mcu cancel chair request
        cServMsg.SetEventId(MT_MCU_CANCELCHAIRMAN_REQ);
        SendMsg2Mcu(cServMsg);
        
        //cancel speaker
        cServMsg.SetEventId(MT_MCU_CANCELSPEAKER_CMD);
        SendMsg2Mcu(cServMsg);
    }
}

/*=============================================================================
  函 数 名： OncmdCCA
  功    能： 
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnCmdCCA()
{
    CServMsg cServMsg;
    MtAdpLog( LOG_INFO, "[OnCmdCCA] Chair Command Acquire \n" );
    u16 wRet;
    
    if(!m_bChairMt)
    {
        //master mcu
        if(m_bMasterMcu && m_bRemoteTypeMcu)
        {
            wRet = m_pcH320Stack->SendCommand(KDVH320_CI_CCR, NULL, 0);
            if(KDVH320_OK != wRet)
            {
                MtAdpLog(LOG_CRITICAL, "[OnCmdCCA] send cmd KDVH320_CI_CCR failed, ret<%d>\n", wRet);
            }
            else
            {
                MtAdpLog( LOG_INFO, "[OnCmdCCA] send KDVH320_CI_CCR to remote.\n");
            }         
        }        
        else //mt
        {
            wRet = m_pcH320Stack->SendCommand(KDVH320_CI_CIT, NULL, 0);
            if(KDVH320_OK != wRet)
            {
                MtAdpLog(LOG_CRITICAL, "[OnCmdCCA] send cmd KDVH320_CI_CIT failed, ret<%d>\n", wRet);
            }
            else
            {
                MtAdpLog( LOG_INFO, "[OnCmdCCA] send KDVH320_CI_CIT to remote.\n");
            }                
            
            //send to mcu chair request
            cServMsg.SetEventId(MT_MCU_APPLYCHAIRMAN_REQ);
            SendMsg2Mcu(cServMsg);
        }        
    }
    else
    {
        MtAdpLog( LOG_CRITICAL, "[OnCmdCCA] already has chair in conf.\n");
    }
}

/*=============================================================================
  函 数 名： OnCmdIIS
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 *pbyParam
             u16 wParamLen
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnCmdIIS(u8 *pbyParam, u16 wParamLen)
{    
    if(wParamLen != sizeof(TH320IIS))
    {
        MtAdpLog(LOG_CRITICAL, "[OnCmdIIS] KDVH320_CI_IIS , invalid param!\n");
        return;
    }
    else
    {
        TH320IIS *ptIIS = (TH320IIS *)pbyParam;	
        MtAdpLog( LOG_INFO, "[OnCmdIIS] Information Indicate Sting: %s\n", ptIIS->m_szAlias);	
        m_bReceivedIIS = TRUE;
        
        //将对端别名上报mcu
        TMtAlias tMtAlias;
        tMtAlias.m_AliasType = mtAliasTypeH320Alias;
        astrncpy(tMtAlias.m_achAlias, ptIIS->m_szAlias, sizeof(tMtAlias.m_achAlias), sizeof(ptIIS->m_szAlias));
        
        CServMsg cServMsg;
        cServMsg.SetMsgBody((u8*)&tMtAlias, sizeof(tMtAlias));
        cServMsg.SetEventId(MT_MCU_MTALIAS_NOTIF);
        SendMsg2Mcu(cServMsg);
    }       
}

/*=============================================================================
  函 数 名： OnCmdTIA
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 *pbyParam
             u16 wParamLen
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnCmdTIA(u8 *pbyParam, u16 wParamLen)
{
    CServMsg cServMsg;

    if(wParamLen < 2)
    {
        MtAdpLog(LOG_CRITICAL, "[OnCmdTIA] KDVH320_CI_TIA, invalid param!\n");
        return;
    }
    else
    {
        MtAdpLog( LOG_INFO, "[OnCmdTIA] Terminal Indicate Assignment :<%d><%d>\n" ,
            pbyParam[0], pbyParam[1]);  
        
        //only first time update mtlist
        if(m_bRemoteTypeMcu && m_byLocalMcuId != pbyParam[0])
        {
            if(1 == g_bySndMtList)
            {
                //更新对端列表 
                cServMsg.SetEventId(MT_MCU_JOINEDMTLIST_REQ);
                SendMsg2Mcu(cServMsg);   
            }                    
        }
        
        //对端是mcu且本端是从mcu, 保存主mcu为本端mcu分配的id
        if(m_bRemoteTypeMcu && !m_bMasterMcu)
        {
            m_byLocalMcuId = pbyParam[0]; 
            
            //需要重新为原来终端分配id...
        }    
        
        //请求对端列表
//        if(KDVH320_OK == m_pcH320Stack->SendCommand(KDVH320_CI_TCU, NULL, 0))
//        {
//            MtAdpLog(LOG_INFO, "[OnCmdTIA] send KDVH320_CI_TCU to remote success.\n");
//        }
    }         
}

/*=============================================================================
  函 数 名： OnCmdMCC
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 *pbyParam
             u16 wParamLen
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnCmdMCC(u8 *pbyParam, u16 wParamLen)
{
    MtAdpLog( LOG_INFO, "[OnCmdMCC] Multipoint Command Conference. remote is mcu \n" );
      
    //若已经分配过mtid，发送tid
    if(!m_bRemoteTypeMcu && m_bAssignMtId)
    {
        m_bAssignMtId = FALSE;
        
        u8 abyParam[2];
        abyParam[0] = 1;
        abyParam[1] = m_byMtId;
        if(KDVH320_OK == m_pcH320Stack->SendCommand(KDVH320_CI_TID, abyParam, sizeof(abyParam)))
        {
            MtAdpLog(LOG_INFO, "[OnCmdMCC] send KDVH320_CI_TID, mt<%d>.\n", m_byMtId);
        }        
    }
    
    if(!m_bRemoteTypeMcu)
    {
        //上报对端类型
        u8 byType = TYPE_MCU;
        CServMsg cServMsg;
        cServMsg.SetEventId(MT_MCU_MTTYPE_NOTIF);
        cServMsg.SetMsgBody(&byType, sizeof(byType));
        SendMsg2Mcu(cServMsg);   
        
        //请求对端列表
        if(KDVH320_OK == m_pcH320Stack->SendCommand(KDVH320_CI_TCU, NULL, 0))
        {
            MtAdpLog(LOG_INFO, "[OnCmdMCC] send KDVH320_CI_TCU to remote success.\n");
        }     
        
        //iis
        OnSendCmdIIS();
    }

    //remote is mcu
    m_bRemoteTypeMcu = TRUE;    
}

/*=============================================================================
  函 数 名： OnCmdTIN
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 *pbyParam
             u16 wParamLen
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnCmdTIN(u8 *pbyParam, u16 wParamLen)
{
    MtAdpLog( LOG_INFO, "[OnCmdTIN] Terminal Indicate Number \n" );
    
    if(wParamLen < 2)
    {
        MtAdpLog( LOG_CRITICAL, "[OnCmdTIN] TIN param is invalid, param num:%d\n", wParamLen);
        return;
    }
    else
    {
        MtAdpLog( LOG_INFO, "[OnCmdTIN] TIN:<%d><%d>\n", pbyParam[0], pbyParam[1]);
    }      
    
    if(m_bMasterMcu)
    {
        if(!m_tSMcuMtList.MtInConf(pbyParam[1]))
        {
            //请求终端别名，在TIP里上报mcu有新终端入会
            if(KDVH320_OK == m_pcH320Stack->SendCommand(KDVH320_CI_TCP, pbyParam, (u8)wParamLen))
            {
                MtAdpLog( LOG_INFO, "[OnCmdTIN] send cmd TCP success, mtid:%d\n", pbyParam[1]);
            }

            m_tSMcuMtList.AddMt(pbyParam[1]);
        }        
    }    
}

/*=============================================================================
  函 数 名： OnCmdTID
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 *pbyParam
             u16 wParamLen
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnCmdTID(u8 *pbyParam, u16 wParamLen)
{
    MtAdpLog( LOG_INFO, "[OnCmdTID] Terminal Indicate Dropped \n" );
    
    if(wParamLen < 2)
    {
        MtAdpLog( LOG_CRITICAL, "[OnCmdTID] TID param is invalid, param num:%d\n", wParamLen);
        return;
    }
    else
    {
        MtAdpLog( LOG_INFO, "[OnCmdTID] TID:<%d><%d>\n", pbyParam[0], pbyParam[1]);
    }      
    
    if(m_bMasterMcu)
    {
        if(m_tSMcuMtList.MtInConf(pbyParam[1]))
        {
            //上报mcu有终端离会
            TMt tMt;
            tMt.SetMt(m_byMtId, pbyParam[1]);
            CServMsg cServMsg;
            cServMsg.SetEventId(MCU_MCU_DELMT_NOTIF);
            cServMsg.SetMsgBody((u8*)&tMt, sizeof(tMt));
            SendMsg2Mcu(cServMsg);

            m_tSMcuMtList.RemoveMt(pbyParam[1]);
        }        
    }    
}

/*=============================================================================
  函 数 名： OnCmdTIL
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 *pbyParam
             u16 wParamLen
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnCmdTIL(u8 *pbyParam, u16 wParamLen)
{
    MtAdpLog( LOG_INFO, "[OnCmdTIL] Terminal Indicate List \n" );
    
    if(wParamLen != sizeof(TH320TIL))
    {
        MtAdpLog( LOG_CRITICAL, "[OnCmdTIL] TIL param is invalid, Param Num:%d\n", wParamLen);
        return;
    }
    else
    {
        TH320TIL *ptTIL = (TH320TIL *)pbyParam;
        MtAdpLog( LOG_INFO, "[OnCmdTIL] mcu id :%d\n", ptTIL->m_byMcuId);
        for(s32 i = 0; i < ptTIL->m_byMtNum; i++)
        {
            MtAdpLog( LOG_INFO, "[OnCmdTIL] mt id :%d\n", ptTIL->m_abyMtId[i]);
        }

        //若本端为主mcu，需要保存从mcu终端列表
        if(m_bMasterMcu)
        {
            u8 abyParam[2];
            abyParam[0] = ptTIL->m_byMcuId;
            m_tSMcuMtList.SetMcuId(m_byMtId);
            for(s32 nIndex = 0; nIndex < ptTIL->m_byMtNum; nIndex++)
            {
                u8 byMtId = ptTIL->m_abyMtId[nIndex];
                if(!m_tSMcuMtList.MtInConf(byMtId))
                {
                    m_tSMcuMtList.AddMt(byMtId);

                    //请求终端别名
                    abyParam[1] = byMtId;
                    m_pcH320Stack->SendCommand(KDVH320_CI_TCP, abyParam, sizeof(abyParam));
                    MtAdpLog( LOG_INFO, "[OnCmdTIL] send TCP mt id :%d\n", byMtId);
                }                
            }

            //update mt list
            CServMsg cServMsg;
            cServMsg.SetEventId(MCU_MCU_ROSTER_NOTIF);
            cServMsg.SetMsgBody((u8*)&m_tSMcuMtList, sizeof(m_tSMcuMtList));
            SendMsg2Mcu(cServMsg);
        }
    }    
}

/*=============================================================================
  函 数 名： OnCmdTIP
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 *pbyParam
             u16 wParamLen
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnCmdTIP(u8 *pbyParam, u16 wParamLen)
{
    MtAdpLog( LOG_INFO, "[OnCmdTIP] Terminal Indicate Personal-identifer \n" );
    
    if(wParamLen != sizeof(TH320MtExt))
    {
        MtAdpLog( LOG_CRITICAL, "[OnCmdTIP] TIP param is invalid, Param Num:%d\n", wParamLen);
        return;
    }
    else
    {
        TH320MtExt *ptMtExt = (TH320MtExt *)pbyParam;
        MtAdpLog( LOG_INFO, "[OnCmdTIP] Terminal Indicate Personal-identifer \n" );
        MtAdpLog( LOG_INFO, "[OnCmdTIP] TIP:<%d><%d> alias:%s\n", ptMtExt->m_tMt.m_byMcuId, 
            ptMtExt->m_tMt.m_byMtId, ptMtExt->m_szAlias);   
        
        //上报mcu new mt join  
        if(m_bMasterMcu)    
        {
            if(ptMtExt->m_tMt.m_byMtId <= MAXNUM_CONF_MT)
            {                        
                if(!m_tSMcuMtList.MtJoinedConf(ptMtExt->m_tMt.m_byMtId))
                {
                    TMcuMcuMtInfo tMtInfo;
                    memset(&tMtInfo, 0, sizeof(tMtInfo));

                    tMtInfo.m_byAudioIn = m_atChannel[AUD_SND_INDEX].m_tLogicChan.GetChannelType();
                    tMtInfo.m_byAudioOut = m_atChannel[AUD_RCV_INDEX].m_tLogicChan.GetChannelType();
                    tMtInfo.m_byVideoIn = m_atChannel[VID_SND_INDEX].m_tLogicChan.GetChannelType();
                    tMtInfo.m_byVideoOut = m_atChannel[VID_RCV_INDEX].m_tLogicChan.GetChannelType();
                    tMtInfo.m_byVideo2In = MEDIA_TYPE_NULL;
                    tMtInfo.m_byVideo2Out = MEDIA_TYPE_NULL;
                    tMtInfo.m_byMtType = MT_TYPE_MT;
                    tMtInfo.m_tMt.SetMt(m_byMtId, ptMtExt->m_tMt.m_byMtId);
                    tMtInfo.m_byIsConnected = 1;
                    
                    if(m_tLocalMuxMode.m_byLSDType > lsdtype_Begin && m_tLocalMuxMode.m_byLSDType < lsdtype_End)
                    {
                        tMtInfo.m_byIsFECCEnable = 1;
                    }
                    astrncpy(tMtInfo.m_szMtName, ptMtExt->m_szAlias, 
                        sizeof(tMtInfo.m_szMtName), sizeof(ptMtExt->m_szAlias)); 
                    
                    CServMsg cServMsg;                    
                    cServMsg.SetEventId(MCU_MCU_NEWMT_NOTIF);
                    cServMsg.SetMsgBody((u8*)&tMtInfo, sizeof(tMtInfo));                
                    SendMsg2Mcu(cServMsg);

                    //上报mtstatus
                    TMcu tMcu;
                    tMcu.SetMcu(m_byMtId);
                    TSMcuMtStatus tStatus;
                    tStatus.SetMt(m_byMtId, ptMtExt->m_tMt.m_byMtId);
                    tStatus.SetCurVideo(1);
                    tStatus.SetCurAudio(1);
                    tStatus.SetIsEnableFECC(tMtInfo.m_byIsFECCEnable);
                    
                    cServMsg.SetEventId(MCU_MCU_MTSTATUS_NOTIF);
                    cServMsg.SetMsgBody((u8*)&tMcu, sizeof(tMcu));
                    cServMsg.CatMsgBody((u8*)&tStatus, sizeof(tStatus));
                    SendMsg2Mcu(cServMsg);

                    m_tSMcuMtList.AddJoinedMt(ptMtExt->m_tMt.m_byMtId);
                }                
            }     
            else
            {
                MtAdpLog( LOG_CRITICAL, "[OnCmdTIP] invalid mt id\n");
            }
        }
    }  
}

/*=============================================================================
  函 数 名： OnCmdTCP
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 *pbyParam
             u16 wParamLen
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnCmdTCP(u8 *pbyParam, u16 wParamLen)
{
    CServMsg cServMsg;
    MtAdpLog( LOG_INFO, "[OnCmdTCP] Terminal Command Personal-identifer \n" );
    
    if(wParamLen < 2)
    {
        MtAdpLog( LOG_CRITICAL, "[OnCmdTCP] TCP param is invalid, Param Num:%d\n", wParamLen);
        return;
    }
    else
    {
        MtAdpLog( LOG_INFO, "[OnCmdTCP] TCP:<%d><%d>\n", pbyParam[0], pbyParam[1]);
    }      
    
    //send msg to mcu request mt alias, send to remote in ack
    TMt tMt;
    tMt.SetMcuId(LOCAL_MCUID);
    tMt.SetMtId(pbyParam[1]);
    cServMsg.SetEventId(MT_MCU_GETMTALIAS_REQ);
    cServMsg.SetMsgBody((u8 *)&tMt, sizeof(tMt));
    SendMsg2Mcu(cServMsg);
}

/*=============================================================================
  函 数 名： OnCmdTIR
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 *pbyParam
             u16 wParamLen
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnCmdTIR(u8 *pbyParam, u16 wParamLen)
{
    CServMsg cServMsg;
    MtAdpLog( LOG_INFO, "[OnCmdTIR] Token Indicate Response \n" );
    
    TH320TIR *ptTIR = (TH320TIR *)pbyParam;
    if(wParamLen != sizeof(TH320TIR))
    {
        MtAdpLog( LOG_CRITICAL, "[OnCmdTIR] TIR param is invalid, Param Num:%d\n", wParamLen);
        return;
    }
    else
    {                
        MtAdpLog( LOG_INFO, "[OnCmdTIR] lsd<%d><%d>, hsd<%d><%d>, chair<%d><%d>\n",
            ptTIR->m_tLsdTokenMt.m_byMcuId, ptTIR->m_tLsdTokenMt.m_byMtId,
            ptTIR->m_tHsdTokenMt.m_byMcuId, ptTIR->m_tHsdTokenMt.m_byMtId,
            ptTIR->m_tChairTokenMt.m_byMcuId, ptTIR->m_tChairTokenMt.m_byMtId);
    }  
    
    //lsd token
    //hsd token
    //chair token - 目前仅处理主席通知
    if(m_bChairMt &&
        0 == ptTIR->m_tChairTokenMt.m_byMcuId && 0 == ptTIR->m_tChairTokenMt.m_byMtId)//cancel chair
    {
        cServMsg.SetEventId(MT_MCU_CANCELCHAIRMAN_REQ);
        SendMsg2Mcu(cServMsg);
    }
    else if(!m_bChairMt &&
        0 != ptTIR->m_tChairTokenMt.m_byMcuId && 0 != ptTIR->m_tChairTokenMt.m_byMtId)//spec chair
    {
        cServMsg.SetEventId(MT_MCU_APPLYCHAIRMAN_REQ);
        SendMsg2Mcu(cServMsg);
    }
    else
    {
    }
}

/*=============================================================================
  函 数 名： OnCmdVCB
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 *pbyParam
             u16 wParamLen
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnCmdVCB(u8 *pbyParam, u16 wParamLen)
{
    CServMsg cServMsg;
    MtAdpLog( LOG_INFO, "[OnCmdVCB] Video Command Broadcasting\n");
    
    if(wParamLen < 2)
    {
        MtAdpLog( LOG_CRITICAL, "[OnCmdVCB] VCB param is invalid, Param Num:%d\n", wParamLen);
        return;
    }
    else
    {
        MtAdpLog(LOG_INFO, "[OnCmdVCB] VCB <%d><%d>\n", pbyParam[0], pbyParam[1]);
    }
    
    //spec speaker
    TMt tMt;
    tMt.SetMcuId(LOCAL_MCUID);
    if(pbyParam[0] == GetRemoteMcuId()) 
    {              
        if(m_bMasterMcu)
        {
            tMt.SetMcuId(m_byMtId);
        }
        tMt.SetMtId(pbyParam[1]);

        //save it for fecc filter
        m_tSpySrcMt.SetMt(pbyParam[0], pbyParam[1]);
    }
    else //master speaking
    {
        tMt.SetMtId(m_byMtId);
    }
    cServMsg.SetEventId(MT_MCU_SPECSPEAKER_REQ);
    cServMsg.SetMsgBody((u8 *)&tMt, sizeof(tMt));
    SendMsg2Mcu(cServMsg);    

    return;
}

/*=============================================================================
  函 数 名： OnCmdCancelVCB
  功    能： 
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnCmdCancelVCB()
{
    CServMsg cServMsg;
    MtAdpLog( LOG_INFO, "[OnCmdCancelVCB] Cancel Video Command Broadcasting\n");
    
    //若对端为主mcu，本端为从mcu，不响应
    if (m_bRemoteTypeMcu && !m_bMasterMcu)
    {
        return;
    }

    //cancel speaker
    cServMsg.SetEventId(MT_MCU_CANCELSPEAKER_CMD);
    SendMsg2Mcu(cServMsg);

    return;
}

/*=============================================================================
  函 数 名： OnCmdVCS
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 *pbyParam
             u16 wParamLen
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnCmdVCS(u8 *pbyParam, u16 wParamLen)
{
    CServMsg cServMsg;
    MtAdpLog( LOG_INFO, "[OnCmdVCS] Video Command Select\n");
    
    if(wParamLen < 2)
    {
        MtAdpLog( LOG_CRITICAL, "[OnCmdVCS] VCS param is invalid, Param Num:%d\n", wParamLen);
        return;
    }
    else
    {
        MtAdpLog(LOG_INFO, "[OnCmdVCS] VCS <%d><%d>\n", pbyParam[0], pbyParam[1]);
    }    
        
    //start select      
    if((!m_bMasterMcu&&m_bRemoteTypeMcu) || !m_bRemoteTypeMcu)
    {
        TMt tDstMt, tSrcMt;
        tDstMt.SetMt(LOCAL_MCUID, m_byMtId);    
        tSrcMt.SetMt(LOCAL_MCUID, pbyParam[1]);
        
        TSwitchInfo tInfo;
        tInfo.SetMode(MODE_BOTH);
        tInfo.SetDstMt(tDstMt);
        tInfo.SetSrcMt(tSrcMt);
        
        cServMsg.SetEventId(MT_MCU_STARTSELMT_REQ);
        cServMsg.SetMsgBody((u8 *)&tInfo, sizeof(tInfo));
        SendMsg2Mcu(cServMsg);
        
        //save it for ack or fecc filter
        m_tSpySrcMt.SetMt(pbyParam[0], pbyParam[1]);
    }    

    return;
}

/*=============================================================================
  函 数 名： OnCmdCancelVCS
  功    能： 
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnCmdCancelVCS()
{
    CServMsg cServMsg;
    MtAdpLog( LOG_INFO, "[OnCmdCancelVCS] Cancel Video Command Select\n");
    
    //cancel speaker
    TMt tMt;
    tMt.SetMt(LOCAL_MCUID, m_byMtId);
    cServMsg.SetEventId(MT_MCU_STOPSELMT_CMD);
    cServMsg.SetMsgBody((u8 *)&tMt, sizeof(tMt));
    SendMsg2Mcu(cServMsg);
}

/*=============================================================================
  函 数 名： OnCmdDCAL
  功    能： 
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnCmdDCAL()
{
    MtAdpLog( LOG_INFO, "[OnCmdDCAL] LSD Command Acquire-token \n" );
    u16 wRet;
    
//    if(m_bMasterMcu && m_bRemoteTypeMcu)
//    {
//        wRet = m_pcH320Stack->SendCommand(KDVH320_CI_DCR_L, NULL, 0);
//        if(KDVH320_OK != wRet)
//        {
//            MtAdpLog(LOG_CRITICAL, "[OnCmdDCAL] send cmd KDVH320_CI_DCR_L failed, ret<%d>\n", wRet);
//        }
//        else
//        {
//            MtAdpLog( LOG_INFO, "[OnCmdDCAL] send KDVH320_CI_DCR_L to remote.\n");
//        }    
//    }
//    else
//    {

        wRet = m_pcH320Stack->SendCommand(KDVH320_CI_DIT_L, NULL, 0);
        if(KDVH320_OK != wRet)
        {
            MtAdpLog(LOG_CRITICAL, "[OnCmdDCAL] send cmd KDVH320_CI_DIT_L failed, ret<%d>\n", wRet);
        }
        else
        {
            MtAdpLog( LOG_INFO, "[OnCmdDCAL] send KDVH320_CI_DIT_L to remote.\n");
        }    
//    }        
}

/*=============================================================================
  函 数 名： OnCmdDISL
  功    能： 
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnCmdDISL()
{
    MtAdpLog( LOG_INFO, "[OnCmdDISL] LSD Indicate Stopped-using-token \n" );
    
    u16 wRet = m_pcH320Stack->SendCommand(KDVH320_CI_DCR_L, NULL, 0);
    if(KDVH320_OK != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[OnCmdDISL] send cmd KDVH320_CI_DCR_L failed, ret<%d>\n", wRet);
    }
    else
    {
        MtAdpLog( LOG_INFO, "[OnCmdDISL] send KDVH320_CI_DCR_L to remote.\n");
    }    
}

/*=============================================================================
  函 数 名： OnCmdVCU
  功    能： 
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnCmdVCU()
{
    CServMsg cServMsg;
    MtAdpLog( LOG_INFO, "[OnCmdVCU] h_ci_VidCommandUpdate \n" );
    u8 byMode = MODE_VIDEO;
    cServMsg.SetEventId(MT_MCU_FASTUPDATEPIC_CMD);
    cServMsg.SetMsgBody(&byMode, sizeof(byMode));
    SendMsg2Mcu(cServMsg);
}

/*=============================================================================
  函 数 名： OnCmdVIN
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 *pbyParam
             u16 wParamLen
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnCmdVIN(u8 *pbyParam, u16 wParamLen)
{
    MtAdpLog( LOG_INFO, "[OnCmdVIN] Video Indicate Number \n" );
    
    if(wParamLen < 2)
    {
        MtAdpLog( LOG_CRITICAL, "[OnCmdVIN] VIN param is invalid, param num:%d\n", wParamLen);
        return;
    }
    else
    {
        MtAdpLog( LOG_INFO, "[OnCmdVIN] VIN:<%d><%d>\n", pbyParam[0], pbyParam[1]);

        m_tVinMt.SetMt(pbyParam[0], pbyParam[1]);        

        if(m_bMasterMcu)
        {
            if(pbyParam[1] != m_tVinMt.GetMtId())
            {                
                TMt tSelMt;
                tSelMt.SetMt(m_byMtId, m_tVinMt.GetMtId());
                
                TCConfViewInfo tView;
                memset(&tView, 0, sizeof(tView));
                tView.m_atViewInfo[0].m_nViewId = OspTickGet();
                tView.m_atViewInfo[0].m_byRes = VIDEO_FORMAT_CIF;
                tView.m_atViewInfo[0].m_byCurGeoIndex = 0;
                tView.m_atViewInfo[0].m_byGeoCount = 1;
                tView.m_atViewInfo[0].m_abyGeoInfo[0] = 1;
                tView.m_atViewInfo[0].m_byMtCount =  1;
                tView.m_atViewInfo[0].m_atMts[0] = tSelMt;

                tView.m_atViewInfo[0].m_byVSchemeCount = 1;
                tView.m_atViewInfo[0].m_atVSchemes[0].m_byMediaType = m_atChannel[VID_SND_INDEX].m_tLogicChan.GetChannelType();
                tView.m_atViewInfo[0].m_atVSchemes[0].m_dwMaxRate = m_wVideoRate;
                tView.m_atViewInfo[0].m_atVSchemes[0].m_dwMinRate = m_wVideoRate;
                tView.m_atViewInfo[0].m_atVSchemes[0].m_bCanUpdateRate = FALSE;
                tView.m_atViewInfo[0].m_atVSchemes[0].m_nFrameRate = 25;
                tView.m_atViewInfo[0].m_atVSchemes[0].m_nVideoSchemeID = OspTickGet()+10;

                CServMsg cServMsg;
                cServMsg.SetEventId(MCU_MCU_VIDEOINFO_ACK);
                cServMsg.SetMsgBody((u8*)&tView, sizeof(tView));
                SendMsg2Mcu(cServMsg);
            }        
        }        
    }      
}

/*=============================================================================
  函 数 名： OnCmdTCU
  功    能： 
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnCmdTCU()
{
    CServMsg cServMsg;
    MtAdpLog( LOG_INFO, "[OnCmdTCU] Terminal Command Update \n" );
    
    if(!m_bRemoteTypeMcu || 1 == g_bySndMtList)
    {
        //request mt list, send TIL in ack    
        cServMsg.SetEventId(MT_MCU_JOINEDMTLIST_REQ);
        SendMsg2Mcu(cServMsg);
    }    
}

/*=============================================================================
  函 数 名： OnReleaseLsdToken
  功    能： 释放远遥令牌
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnReleaseLsdToken()
{
    if(m_bHasLsdToken)
    {
        u16 wRet = m_pcH320Stack->SendCommand(KDVH320_CI_DIS_L, NULL, 0);
        if(KDVH320_OK == wRet)
        {
            MtAdpLog(LOG_INFO, "[OnReleaseLsdToken] send cmd KDVH320_CI_DIS_L success\n");    
            m_bHasLsdToken = FALSE;
        }        
        else
        {
            MtAdpLog(LOG_WARNING, "[OnReleaseLsdToken] send cmd KDVH320_CI_DIS_L failed, ret:%d\n", wRet);    
            SetTimer(TIMER_LSDTOKEN_RELEASE, m_wLsdTokenHoldTime*1000);
        }
    }
}

/*=============================================================================
  函 数 名： OnMcuMtPrivateMsg
  功    能： 测试消息，用于mcu查询适配状态
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnMcuMtPrivateMsg(const CMessage *pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    if(cServMsg.GetMsgBodyLen() < sizeof(u8)*2)
    {
        MtAdpLog(LOG_CRITICAL, "[OnMcuMtPrivateMsg] msg body is invalid!\n");
        return;
    }

    u8 byState = 0;

    u8 byMsgType = *(cServMsg.GetMsgBody());
    u8 byMode = *(cServMsg.GetMsgBody() + 1);
    switch(byMsgType)
    {
    case CHAN_OPENCLOSE_STATE:
        {
            if(cServMsg.GetMsgBodyLen() < sizeof(u8)*3)
            {
                MtAdpLog(LOG_CRITICAL, "[OnMcuMtPrivateMsg] no direct info in msg body!\n");
                return;
            }

            u8 byDirect = *(cServMsg.GetMsgBody() + 2);
            if(MODE_AUDIO == byMode)
            {
                //mt->mcu
                if(0 == byDirect)
                {
                    if(CHAN_CONNECTED == m_atChannel[AUD_SND_INDEX].m_byState)
                    {
                        byState = 1;
                    }
                }
                //mcu ->mt
                else
                {
                    if(CHAN_CONNECTED == m_atChannel[AUD_RCV_INDEX].m_byState)
                    {
                        byState = 1;
                    }
                }
            }
            else if(MODE_VIDEO == byMode)
            {
                //mt->mcu
                if(0 == byDirect)
                {
                    if(CHAN_CONNECTED == m_atChannel[VID_SND_INDEX].m_byState)
                    {
                        byState = 1;
                    }
                }
                //mcu ->mt
                else
                {
                    if(CHAN_CONNECTED == m_atChannel[VID_RCV_INDEX].m_byState)
                    {
                        byState = 1;
                    }
                }
            }
            else if(MODE_BOTH == byMode)
            {
                //mt->mcu
                if(0 == byDirect)
                {
                    if(CHAN_CONNECTED == m_atChannel[VID_SND_INDEX].m_byState &&
                        CHAN_CONNECTED == m_atChannel[AUD_SND_INDEX].m_byState)
                    {
                        byState = 1;
                    }
                }
                //mcu ->mt
                else
                {
                    if(CHAN_CONNECTED == m_atChannel[VID_RCV_INDEX].m_byState &&
                        CHAN_CONNECTED == m_atChannel[AUD_RCV_INDEX].m_byState)
                    {
                        byState = 1;
                    }
                }
            }
            else
            {
                MtAdpLog(LOG_CRITICAL, "[OnMcuMtPrivateMsg] msg mode<%d> is invalid!\n", byMode);
                return;
            }
        }
        break;

    default:
        {
            MtAdpLog(LOG_CRITICAL, "[OnMcuMtPrivateMsg] msg type<%d> is invalid!\n", byMsgType);
            return;
        }        
        break;
    }

    cServMsg.SetEventId(H320MTADP_MCU_PRIVATE_MSG);
    cServMsg.SetMsgBody(&byMsgType, sizeof(u8));
    cServMsg.CatMsgBody(&byState, sizeof(u8));
    SendMsg2Mcu(cServMsg);

    MtAdpLog(LOG_INFO, "[OnMcuMtPrivateMsg] send private msg to mcu success.\n");
}

/*=============================================================================
  函 数 名： OnPrintSwitchInfo
  功    能： 交换信息打印
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnPrintSwitchInfo(const CMessage *pcMsg)
{
    TChannel *ptChan = NULL;
    s8 szIP1[16], szIP2[16];

    ptChan = &m_atChannel[AUD_SND_INDEX];
    if(CHAN_CONNECTED == ptChan->m_byState)
    {            
        strcpy(szIP1, StrIPv4(ptChan->m_tSndMediaChannel.GetNetSeqIpAddr()));
        strcpy(szIP2, StrIPv4(ptChan->m_tLogicChan.m_tRcvMediaChannel.GetNetSeqIpAddr()));
        MtAdpLog(LOG_CRITICAL, "audio snd   %s:%u -> %s:%u\n", 
                 szIP1,
                 ptChan->m_tSndMediaChannel.GetPort(),
                 szIP2,
                 ptChan->m_tLogicChan.m_tRcvMediaChannel.GetPort());
    }
  
    ptChan = &m_atChannel[VID_SND_INDEX];
    if(CHAN_CONNECTED == ptChan->m_byState)
    {
        strcpy(szIP1, StrIPv4(ptChan->m_tSndMediaChannel.GetNetSeqIpAddr()));
        strcpy(szIP2, StrIPv4(ptChan->m_tLogicChan.m_tRcvMediaChannel.GetNetSeqIpAddr()));
        MtAdpLog(LOG_CRITICAL, "video snd   %s:%u -> %s:%u\n", 
                szIP1,
                ptChan->m_tSndMediaChannel.GetPort(),
                szIP2,
                ptChan->m_tLogicChan.m_tRcvMediaChannel.GetPort());
    }
   
    ptChan = &m_atChannel[AUD_RCV_INDEX];
    if(CHAN_CONNECTED == ptChan->m_byState)
    {
        strcpy(szIP1, StrIPv4(ptChan->m_tLogicChan.m_tRcvMediaChannel.GetNetSeqIpAddr()));
        strcpy(szIP2, StrIPv4(ptChan->m_tLogicChan.m_tSndMediaCtrlChannel.GetNetSeqIpAddr()));
        MtAdpLog(LOG_CRITICAL, "audio rcv   %s:%u <- %s:%u\n", 
                szIP1,
                ptChan->m_tLogicChan.m_tRcvMediaChannel.GetPort(),
                szIP2,
                ptChan->m_tSndMediaChannel.GetPort());
    }

    ptChan = &m_atChannel[VID_RCV_INDEX];
    if(CHAN_CONNECTED == ptChan->m_byState)
    {
        strcpy(szIP1, StrIPv4(ptChan->m_tLogicChan.m_tRcvMediaChannel.GetNetSeqIpAddr()));
        strcpy(szIP2, StrIPv4(ptChan->m_tLogicChan.m_tSndMediaCtrlChannel.GetNetSeqIpAddr()));
        MtAdpLog(LOG_CRITICAL, "video rcv   %s:%u <- %s:%u\n", 
                 szIP1,
                 ptChan->m_tLogicChan.m_tRcvMediaChannel.GetPort(),
                 szIP2,
                 ptChan->m_tSndMediaChannel.GetPort());
    }
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMtAdpData::CMtAdpData():m_dwMcuANode(INVALID_NODE), 
                         m_dwMcuAIId(0), 
                         m_byMcuAId(0),                       
                         m_dwMcuBNode(INVALID_NODE), 
                         m_dwMcuBIId(0), 
                         m_byMcuBId(0),           
                         m_bDoubleLink(FALSE),
                         m_dwLocalIP(0),
                         m_byDriId(0), 
                         m_byLogLevel(LOG_CRITICAL)
{
    memset(m_achMtAdpAlias, 0, sizeof(m_achMtAdpAlias));
    memset(m_abyMt2InsMap, 0, sizeof(m_abyMt2InsMap));
}

CMtAdpData::~CMtAdpData()
{
    
}
