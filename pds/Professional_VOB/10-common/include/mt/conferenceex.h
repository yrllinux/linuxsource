#ifndef CONFERENCEEX_EVENT_H
#define CONFERENCEEX_EVENT_H

#include "eventcomm.h"


#ifndef _MakeMtEventDescription_
enum EmConference
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


/**********************<< 停止多画面合成请求 >>**************	
 *[消息体]			 
 * +TMtVMPParam
 *
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStopVMPReq   )
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

     _ev_segment_end(conference)
};

#endif

