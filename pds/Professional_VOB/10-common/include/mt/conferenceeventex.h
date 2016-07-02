#ifndef CONFERENCEEX_EVENT_H
#define CONFERENCEEX_EVENT_H
#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmConferenceEx
{
#endif 
    _ev_segment(conferenceex )//会议扩展消息

/**********************<< 申请插话 >>**************	
 *[消息体]			 
 *  
 *[消息方向]
 *
 *   UI => MtService  
 */_event(  ev_ConfApplyChiemeReq  )
   _ev_end


 /**********************<< 指定插话 >>**************	
 *[消息体]			 
 *  +TMtId
 *[消息方向]
 *
 *   UI => MtService 
 */_event(  ev_ConfChiemeInReq  )
   _body ( TMtId   , 1          )
   _ev_end


/**********************<< 同意插话(主席) >>**************	
 *[消息体]			 
 *  +TMtId
 *[消息方向]
 *
 *   UI => MtService 
 */_event(  ev_ConfChiemeInCmd  )
   _body ( TMtId   , 1          )
   _ev_end


/**********************<< 终端插话结果指示 >>**************	
 *[消息体]			 
 *  +BOOL    TRUE 正在插话 FALSE失败
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfChimeInStateInd  )  
   _body (  BOOL   , 1          )
   _ev_end


/**********************<< 其他终端申请插话指示 >>**************	
 *[消息体]			 
 *  +TMtId
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfApplyChimeInInd   ) 
   _body ( TMtId   , 1          )
   _ev_end


 /**********************<< 轮询操作 >>**************	
 *[消息体]			 
 * + TMtPollInfo
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfPollCmd  ) 
   _body ( TMtPollInfo   , 1          )
   _ev_end


 /**********************<< 轮询状态指示 >>**************	
 *[消息体]			 
 * + TMtPollInfo
 * 
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfPollStatInd  ) 
   _body ( TMtPollInfo   , 1          )
   _ev_end


/**********************<< 轮询超时 >>**************	
 *[消息体]			 
 * 
 * 
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_PollTimeroutInd  )
   _ev_end


/**********************<< 主席邀请终端chair->MC) >>**************	
 *[消息体]			 
 *  + TTERADDR
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfChairInviteTerReq   ) 
   _body (  TMtAddr   , 1              )
   _ev_end



/**********************<< 主席邀请终端失败(MC->chair) >>**************	
 *[消息体]			 
 *  +BOOL  
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfChairInviteFailInd   ) 
   _body ( BOOL   , 1          )
   _ev_end


 /**********************<< 远端静音指示 >>**************	
 *[消息体]			 
 *  +TMtId
 *  + BOOL     TRUE=静音 /FALSE=
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 */_event(  ev_ConfMakeTerQuietInd   ) 
   _body ( TMtId   , 1          ) 
   _body ( BOOL   , 1          )
   _ev_end


 /**********************<< 远端哑音指示 >>**************	
 *[消息体]			 
 *  +TMtId
 *  + BOOL     TRUE=哑音 /FALSE=
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 */_event(  ev_ConfMakeTerMuteInd   ) 
   _body ( TMtId   , 1          ) 
   _body ( BOOL   , 1          )
   _ev_end


/**********************<< 远端静音命令 >>**************	
 *[消息体]			 
 *  +TMtId
 *  + BOOL     TRUE=静音 /FALSE=
 *[消息方向]
 *
 *   MtService => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfMakeTerQuietCmd   ) 
   _body ( TMtId   , 1          ) 
   _body ( BOOL   , 1          )
   _ev_end


/**********************<< 远端哑音命令 >>**************	
 *[消息体]			 
 *  +TMtId
 *  + BOOL     TRUE=哑音 /FALSE=
 *[消息方向]
 *
 *   MtService => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfMakeTerMuteCmd   ) 
   _body ( TMtId   , 1          ) 
   _body ( BOOL   , 1          )
   _ev_end


/**********************<< 终端请求其他终端状态 >>**************	
 *[消息体]			 
 *  +TMtId
 *  
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfTerGetTerStatusReq   ) 
   _body ( TMtId   , 1          )
   _ev_end


/**********************<< 终端请求其他终端应答 >>**************	
 *[消息体]			 
 *  +TMtId
 *  +TTerStatus
 *  
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfTerGetTerStatusRsp   ) 
   _body ( TMtId   , 1          ) 
   _body ( TTerStatus   , 1          )
   _ev_end


/**********************<< MC请求终端状态 >>**************	
 *[消息体]			 
 *  +TMtId
 *  +TTerStatus
 *  
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfMcGetTerStatusReq   ) 
   _body ( TMtId   , 1          ) 
   _body ( TTerStatus   , 1          )
   _ev_end


/**********************<< MC请求终端状态应答 >>**************	
 *[消息体]			 
 *  +TTerStatus
 *  
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfMcGetTerStatusRsp   ) 
   _body ( TTerStatus   , 1          )
   _ev_end




/**********************<< 会议信息请求 >>**************	
 *[消息体]			 
 *  
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfConfInfoReq   ) 
   _ev_end



/**********************<< 会议信息应答 >>**************	
 *[消息体]			 
 *  TMtConfInfo
 *  
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfConfInfoInd   ) 
   _body ( TMtConfInfo, 1 )
   _body( u8         , 1 )
   _body( TMtVmpItem , REMAIN )
   _ev_end


/**********************<< 会议即将结束 >>**************	
 *[消息体]			 
 *  +u16 minutes  表示还有多久结束
 *  
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfWillEndInd   )
   _body ( u16   ,1            )
   _ev_end


/**********************<< 会议延长请求 >>**************	
 *[消息体]			 
 *  +u16 minutes  
 表示延长多久
 *  
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfProlongReq   ) 
   _body ( u16   , 1          )
   _ev_end



/**********************<< 会议延长请求应答 >>**************	
 *[消息体]			 
 *  +BOOL    TRUE=同意
 *  
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfProlongRsp   ) 
   _body ( BOOL   , 1          )
   _ev_end



/**********************<< 开始语音激励请求 >>**************	
 *[消息体]			 
 *
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStartVACReq   ) 

   _ev_end


/**********************<< 开始语音激励应答 >>**************	
 *[消息体]	
 *  +BOOL    TRUE=同意
 *  
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStartVACRsp   ) 
   _body ( BOOL      , 1          )
   _ev_end


/**********************<< 停止语音激励请求 >>**************	
 *[消息体]	
 *
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStopVACReq   )
   _ev_end


/**********************<< 停止语音激励应答 >>**************	
 *[消息体]	
 *  +BOOL    TRUE=同意
 *
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStopVACRsp   )
   _body ( BOOL      , 1        )
   _ev_end


/**********************<< 开始讨论模式请求 >>**************	
 *[消息体]			 
 *  +u8  讨论深度
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStartDiscussReq   ) 
   _body( u8       ,   1 )
   _ev_end


/**********************<< 开始讨论模式应答 >>**************	
 *[消息体]	
 *  +BOOL    TRUE=同意
 *  
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStartDiscussRsp   ) 
   _body ( BOOL   , 1          )
   _ev_end


/**********************<< 停止讨论模式请求 >>**************	
 *[消息体]			 

 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStopDiscussReq   ) 
   _ev_end


/**********************<< 停止讨论模式应答 >>**************	
 *[消息体]	
 *  +BOOL    TRUE=同意
 *  
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStopDiscussRsp   ) 
   _body ( BOOL   , 1          )
   _ev_end


/**********************<< 开始自动多画面合成请求 >>**************	
 *[消息体]			 
 * +TMtVMPParam

 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStartVMPReq   )
   _body( TMtVMPParam, 1 )
   _body( u8         , 1 )
   _body( TMtVmpItem , REMAIN )
   _ev_end


/**********************<< 开始自动多画面合成应答 >>**************	
 *[消息体]	
 *  +BOOL    TRUE=同意
 *  
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStartVMPRsp   )  
   _body ( BOOL   , 1          )
   _ev_end


/**********************<< 停止自动多画面合成请求 >>**************	
 *[消息体]			 
 * +TMtVMPParam
 *
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStopVMPReq   )
   _body( TMtVMPParam, 1 )
   _body( u8         , 1 )
   _body( TMtVmpItem , REMAIN )
   _ev_end


/**********************<< 停止自动多画面合成应答 >>**************	
 *[消息体]	
 *  +BOOL    TRUE=同意
 *  
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStopVMPRsp   ) 
   _body ( BOOL      , 1          )
   _ev_end


/**********************<< 设置自动多画面合成参数请求 >>**************	
 *[消息体]	
 *  +TMtVMPParam
 *  
 *  
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfSetVMPParamReq   ) 
   _body ( TMtVMPParam   , 1          )
   _body( u8         , 1 )
   _body( TMtVmpItem , REMAIN )
   _ev_end


/**********************<< 设置自动多画面合成参数应答 >>**************	
 *[消息体]	
 *  +BOOL    TRUE=同意
 *  
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfSetVMPParamRsp   ) 
   _body ( BOOL   , 1          )
   _ev_end

/**********************<< 获取自动多画面合成应答参数请求 >>**************	
 *[消息体]	
 * 无
 *  
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfGetVMPParamReq   )
   _ev_end


/**********************<< 获取自动多画面合成应答参数应答 >>**************	
 *[消息体]	
 *  +TMtVMPParam
 *  
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfGetVMPParamRsp   ) 
   _body ( TMtVMPParam   , 1          )
   _body( u8         , 1 )
   _body( TMtVmpItem , REMAIN )
   _ev_end


/**********************<< 设置多画面合成参数请求(华为) >>**************	
 *[消息体]	
 *  +EmVMPStyle
 *  +TMtId  16
 *  
 *  
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfHSetVMPParamReq   ) 
   _body ( EmVMPStyle, 1  )
   _body ( TMtId, 16  )
   _ev_end


/**********************<< 设置多画面合成参数应答(华为) >>**************	
 *[消息体]	
 *  +BOOL    TRUE=同意
 *  
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfHSetVMPParamRsp   ) 
   _body ( BOOL   , 1          )
   _ev_end

/**********************<< 开始强制广播 >>**************	
 *[消息体]	
 *  +无
 *  
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStartForceBroadcastCmd  ) 
   _ev_end

   /**********************<< 停止强制广播 >>**************	
 *[消息体]	
 *  +无
 *  
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStopForceBroadcastCmd   ) 
   _ev_end

   
/**********************<< 简单会议信息指示 >>**************	
 *[消息体]	
 *  +TMtSimpConfInfo
 *  
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_SimpleConfInfoInd   ) 
   _body ( TMtSimpConfInfo   , 1          )
   _ev_end

/*********************<< 视频源名字指示  >>************************
 *[消息体]	
 * + char[]
 *
 *[消息方向]
 *    Service  = > UI
 *
*/_event(  ev_VideoSourceNameInd                    )
  _body(   s8    ,REMAIN                            )
  _ev_end
  

/**********************<< 请求未入会终端列表 >>**************	
 *[消息体]			 
 *  无
 *[消息方向]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfOfflineTerListReq   ) 
   _ev_end


/**********************<< 未入会终端列表指示 >>**************	
 *[消息体]			 
 *  +TMtInfo[]   终端标号数组
 *[消息方向]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfOfflineTerListInd   ) 
   _body ( TMtInfo   , REMAIN     )
   _ev_end

/**********************<< 呼叫离线终端 >>**************	
 *[消息体]			 
 *  +TMtId
 *[消息方向]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfCallOfflineTerCmd   ) 
   _body ( TMtId   , 1     )
   _ev_end

 /*********************<< 点名请求  >>************************
 *[消息体]	
 * + TMtId
 *
 *[消息方向]
 *    Service  = > UI
 *
*/_event(  ev_ConfRollCallReq                    )
  _body(   TMtId    ,1                            )
  _ev_end

 /*********************<< 点名响应  >>************************
 *[消息体]	
 * + TMtId
 *
 *[消息方向]
 *    Service  = > UI
 *
*/_event(  ev_ConfRollCallRsp                    )
  _body(   BOOL    ,1                            )
  _ev_end

  /*********************<< 切换画面风格  >>************************
 *[消息体]	
 * + TMtId
 *
 *[消息方向]
 *    Service  = > UI
 *
*/_event(  ev_ConfSwitchBroadCastStyleCmd              )
  _body(   BOOL    ,1                            )
  _ev_end

  
  /*********************<< 对端会议能力指示  >>************************
 *[消息体]	
 * + TMtId
 *
 *[消息方向]
 *    Service  = > UI
 *
*/_event(  ev_ConfPeerCapInfo                  )
  _body(   TPeerCapabilityInfo    ,1           )
  _ev_end
 
/*********************<< MCU给终端的信息通知  >>************************
 *[消息体]	
 * + TMtId
 *  EmMcuNtfMsgType, 通知的消息类型
 *[消息方向]
 *    h323service  = >  mtservice
 *
*/_event(  ev_ConfMcuInfoNotify                  )
  _body(   EmMcuNtfMsgType   ,  1                )
  _ev_end
 
 /*********************<< 对端是否支持keda私有协议的能力指示  >>************************
 * xjx_070904, 该消息为私有消息，只在mtservice和mth323service之间通信的消息
 * //内部消息易于扩展
 *[消息体]	
 * + BOOL //是否指示丢包重传
 * + BOOL //是否支持音频冗余
 *  
 *[消息方向]
 *    h323service  = >  mtservice
 *
*/_event(  ev_ConfPeerCapPrivateInfo   )
  _ev_end

 /*********************<< 对端不支持keda私有协议的能力,在挂断后的恢复  >>************************
 * xjx_070904, 该消息为私有消息，只在mtservice和mth323service之间通信的消息
 * //内部消息易于扩展
 *[消息体]	
 * + BOOL //是否指示丢包重传
 * + BOOL //是否支持音频冗余
 *  
 *[消息方向]
 *    h323service  = >  mtservice
 *
*/_event(  ev_ConfPeerCapPrivateRecover   )
  _ev_end
  
//xjx_071207
/**********************<< 挂断与会终端 >>**************	
 *[消息体]			 
 *  +TMtId   //与 ev_ConfCallOfflineTerCmd信令，相对应
 *[消息方向]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfDisconnectOnlineTerCmd   ) 
   _body ( TMtId   , 1     )
   _ev_end

/**********************<< 添加会场 >>**************	
 *[消息体]			 
 *  +          //应该跟删除会场是对应的
 *[消息方向]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event( ev_ConfAddRoomReq )
// _body( )
_ev_end

/**********************<< 添加会场响应 >>**************	
 *[消息体]			 
 *  +          //
 *[消息方向]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event( ev_ConfAddRoomRsp )
_ev_end

/**********************<< 当前会议支持主叫呼集 >>**************	
 *[消息体]			 
 *  +     //说明当前是多点会议，mcu是huawei的。 
 *[消息方向]
 *
 *   MtService  <= 323/sip/320
 *   MtService => UI
 */_event( ev_ConfSitecallSupportInd )
_ev_end

/**********************<< 剥夺辅流令牌命令 >>**************	
 *[消息体]			 
 *  +      
 *[消息方向]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event( ev_ConfDepriveTokenReq )
_ev_end

/**********************<< 剥夺辅流令牌的响应 >>**************	
 *[消息体]			 
 *  +      
 *[消息方向]
 *
 *   MtService  <= 323/sip/320
 *   MtService => UI
 */_event( ev_ConfDepriveTokenRsp )
_ev_end


/**********************<< 是否进行自动广播指示命令 >>**************	
 *[消息体]			 
 *  + u8     
 *[消息方向]
 *
 *     UI => MtService
 */_event( ev_ConfHWPollTypeCmd )
 _body( u8    , 1 )
_ev_end



  

#ifndef _MakeMtEventDescription_
   _ev_segment_end(conferenceex)
};
#endif


#endif


