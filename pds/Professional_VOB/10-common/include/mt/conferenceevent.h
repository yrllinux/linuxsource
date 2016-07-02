#ifndef CONFERENCE_EVENT_H
#define CONFERENCE_EVENT_H
#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmConference
{
#endif 
   _ev_segment(conference )//会议消息

/***********************<< 会议链路状态指示 >>**********************	
 *[消息体]			 
 *  TLinkState
 *[消息方向]
 *
 *   MtService <=> 323/sip/320
 *   MtService => UI
 */_event(  ev_CallLinkStateInd   )
   _body ( TLinkState   , 1       )
   _ev_end
 
 /***********************<< 多点会议开始指示 >>**********************	
 *[消息体]			 
 *  无
 *[消息方向]
 *
 *   MtService <=> 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfMccStartInd   )
   _ev_end
 
/***********************<< 输入会议密码请求 >>***********************	
 *[消息体]			 
 *  无
 *[消息方向]
 *
 *   MtService <=> 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfPasswordReq   )
   _ev_end
 

/***********************<< 输入会议密码响应 >>***********************	
 *[消息体]			 
 *  +s8  密码
 *[消息方向]
 *
 *   MtService <=> 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfPasswordRsp   )
   _body (  s8   , REMAIN       )
   _ev_end


/***********************<< 选择会议列表请求 >>***********************	
 *[消息体]			 
 *   + TConfListInfo[]     数组
 *[消息方向]
 *该消息由业务发送给界面来选择预加入的会议列表
 *  
 *   MtService => UI
 */_event(  ev_ConfListChoiceReq   )
   _body ( TConfListInfo   , REMAIN      )
   _ev_end


/***********************<< 选择会议列表响应 >>***********************	
 *[消息体]			 
 *   +TConfId
 *[消息方向]
 *  
 *   MtService => UI
 */_event(  ev_ConfListChoiceRsp   )
   _body ( TConfBaseInfo   , 1           )
   _ev_end



/***********************<< 分配终端编号 >>***********************	
 *[消息体]			 
 *  +TMtId		终端标号
 *[消息方向]
 *
 *   MtService <=> 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfTerLabelInd   ) 
   _body ( TMtId       , 1       )
   _ev_end



/***********************<< 终端加入会议 >>***********************	
 *[消息体]			 
 *  +TMtId		终端标号
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfTerJoinedInd   ) 
   _body ( TMtId   , 1            )
   _ev_end


/***********************<< 终端退出会议 >>***********************	
 *[消息体]			 
 *  +TMtId		终端标号
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfTerLeftInd   ) 
   _body ( TMtId   , 1          )
   _ev_end


/***********************<< 强制终端退出 >>***********************	
 *[消息体]			 
 *  +TMtId		终端标号
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfDropTerCmd   ) 
   _body ( TMtId   , 1          )
   _ev_end


/***********************<< 强制终端退出失败 >>***********************	
 *[消息体]			 
 *  无
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfDropTerRejectInd   )
   _ev_end



/***********************<< 申请发言 >>***********************	
 *[消息体]			 
 *  无
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfApplySpeakReq   )
   _ev_end


/***********************<< 别的终端申请发言 >>***********************	
 *[消息体]			 
 *  TMtId    终端标号
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfApplySpeakInInd   )
   _body ( TMtId   , 1          )
   _ev_end


/***********************<< 发言人位置指示 >>***********************	
 *[消息体]			 
 *  +TMtId		终端标号
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfSpeakerInd   ) 
   _body ( TMtId   , 1          )
   _ev_end


/*********<< 广播终端命令(主席)该命令用于应答 _event(  ev_ConfApplySpeakInInd>>**************	
 *[消息体]			 
 *  +TMtId		终端标号
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfBroadcastCmd   ) 
   _body ( TMtId   , 1          )
   _ev_end


/*********<< 广播终端命令(主席)该请求用于指定某终端为发言人>>**************	
 *[消息体]			 
 *  +TMtId		终端标号
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfBroadcastReq   ) 
   _body ( TMtId   , 1          )
   _ev_end


/*********<< 广播应答 >>**************	
 *[消息体]			 
 *  +BOOL    TRUE=同意 FALSE=拒绝 
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfBroadcastRsp   ) 
   _body ( BOOL   , 1          )
   _ev_end




/*********<< 取消广播命令   >>**************	
 *[消息体]			 
 *  无
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfCancelBroadcastCmd   ) 
   _ev_end



/**********************<< 选看终端命令 >>**************	
 *[消息体]			 
 *  +TMtId		 终端标号
 *  +EmMediaType 选看类型
 *[消息方向]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfSendThisTerCmd   ) 
   _body ( TMtId   , 1          )
   _body ( EmMediaType, 1          )
   _ev_end


 /**********************<< 选看终端请求 >>**************	
 *[消息体]			 
 *  +TMtId		终端标号
  *  +EmMediaType 选看类型
 *[消息方向]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfSendThisTerReq   ) 
   _body ( TMtId   , 1          )
   _body ( EmMediaType, 1          )
   _ev_end


/**********************<< 选看终端应答 >>**************	
 *[消息体]			 
 *  +BOOL     TRUE=选看成功 FALSE=选看失败
 *[消息方向]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfSendThisTerRsp   ) 
   _body ( BOOL   , 1          )
   _ev_end


 /**********************<< 取消选看命令 >>**************	
 *[消息体]			 
 *  无
 *[消息方向]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfCancelSendThisTerCmd   )
   _ev_end


/*********<< 取消选看终端应答 >>**************	
 *[消息体]			 
 *  +BOOL    TRUE=同意 FALSE=拒绝 
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfCancelSendThisTerRsp   )
   _ev_end


/**********************<< 本终端被选看指示 >>**************	
 *[消息体]			 
 *  无
 *[消息方向]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfSeenByOtherInd   )
   _ev_end


/**********************<< 本端被取消选看 >>**************	
 *[消息体]			 
 *  无
 *[消息方向]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfCancelSeenByOtherInd   )
   _ev_end


/**********************<< 正在观看的视频源 >>**************	
 *[消息体]			 
 *  +TMtId		终端标号
 *[消息方向]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfYouAreSeeingInd   ) 
   _body ( TMtId   , 1          )
   _ev_end


/**********************<< 请求终端列表 >>**************	
 *[消息体]			 
 *  无
 *[消息方向]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfTerListReq   ) 
   _ev_end


/**********************<< 终端列表指示 >>**************	
 *[消息体]			 
 *  +TMtInfo[]   终端标号数组
 *[消息方向]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfTerListInd   ) 
   _body ( TMtInfo   , REMAIN     )
   _ev_end


/**********************<< ter请求所有终端名 >>**************	
 *[消息体]			 
 *  无
 *[消息方向]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfTerGetAllTerNameReq   )
   _ev_end


/**********************<< MC回答所有终端名字 >>**************	
 *[消息体]			 
 *  +TMtInfo[] terInfo数组
 *[消息方向]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfTerGetAllTerNameRsp   ) 
   _body ( TMtInfo   , REMAIN          )
   _ev_end


/**********************<< ter请求其他终端名 >>**************	
 *[消息体]			 
 *  TMtId     终端标号
 *[消息方向]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfTerGetTerNameReq   )
   _body ( TMtId   , 1          )
   _ev_end


/**********************<< MC回答其他终端名字 >>**************	
 *[消息体]			 
 *  +TMtInfo[] terInfo数组
 *[消息方向]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfTerGetTerNameRsp   ) 
   _body ( TMtInfo   , REMAIN          )
   _ev_end


/**********************<< MC请求终端名字 >>**************	
 *[消息体]			 
 *  无
 *[消息方向]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfMcGetTerNameReq   ) 
   _ev_end


/**********************<< ter回答终端名字 >>**************	
 *[消息体]			 
 *  +TMtInfo[] terInfo数组
 *[消息方向]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfMcGetTerNameRsp   ) 
   _body ( TMtInfo   , REMAIN          )
   _ev_end



/**********************<< 结束会议 >>**************	
 *[消息体]			 
 *  无
 *[消息方向]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfEndConfCmd   )
   _ev_end


/**********************<< 被广播指示 >>**************	
 *[消息体]			 
 *  无
 *[消息方向]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfSeenByAllInd   )
   _ev_end



/**********************<< 取消广播指示 >>**************	
 *[消息体]			 
 *  
 *[消息方向]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfCancelSeenByAllInd   )
   _ev_end



/**********************<< 申请主席 >>**************	
 *[消息体]			 
 *  无
 *[消息方向]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfMakeMeChairReq   )
   _ev_end


/**********************<< 转移主席令牌 >>**************	
 *[消息体]			 
*  +TMtId
 *[消息方向]
 *
 *   MtService  => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfChairTransferReq  ) 
   _body ( TMtId   , 1          )
   _ev_end


/**********************<< 主席撤销命令>**************	
 *[消息体]			 
 *  无
 *[消息方向]
 *
 *   MtService <=> 323/sip/320
 *   MtService <=> UI
 */_event(  ev_ConfWithdrawChairCmd  )
   _ev_end



/**********************<< 主席撤销指示 >>**************	
 *[消息体]			 
 *  无
 *[消息方向]
 *
 *   MtService <=> 323/sip/320
 *   MtService <=> UI
 */_event(  ev_ConfWithdrawChairInd  )
   _ev_end


/**********************<< 询问当前主席 >>**************	
 *[消息体]			 
 *  无
 *[消息方向]
 *
 *   MtService => 323/sip/320
 *   MtService <= UI
 */_event(  ev_ConfWhoIsChairReq  )
   _ev_end


/**********************<< 询问当前主席应答 >>**************	
 *[消息体]			 
 *  +TMtId
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfWhoIsChairRsp  ) 
   _body ( TMtId   , 1          )
   _ev_end


/**********************<< 本端主席令牌应答 >>**************	
 *[消息体]			 
 *  +BOOL ( TRUE/FALSE )
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfChairTokenRsp  ) 
   _body ( BOOL   , 1          )
   _ev_end


/**********************<< 主席终端指示 >>**************	
 *[消息体]			 
 + TMtId
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfChairInd  ) 
   _body ( TMtId   , 1          )
   _ev_end



/**********************<< 有终端申请主席 >>**************	
 *[消息体]			 
 *  +TMtId
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfApplyChairInInd  ) 
   _body ( TMtId   , 1          )
   _ev_end



/**********************<< 短消息指示 >>**************	
 *[消息体]			 
 *  char[]
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfSMSInd  ) 
   _body ( s8   ,REMAIN    )
   _ev_end


/**********************<< 发送短消息 >>**************	
 *[消息体]			 
 *  char[]
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfSMSCmd  ) 
   _body (  s8   , REMAIN          )
   _ev_end


/**********************<< 请求开始组播 >>**************	
 *[消息体]			 
 *  +TMtId
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStartMultiCastTerReq  )  
   _body ( TMtId   , 1          )
   _ev_end



  
 
 
 

/**********************<< 请求开始组播响应 >>**************	
 *[消息体]			 
 *  + TMtId
 *  + u32( 目标终端IP地址,如果失败ip为0 )
 *  + u16( 音频端口 )
 *  + u16( 视频端口 )
 *  + u8(音频真实载荷值)  
 *  + u8(视频真实载荷值)
 *  + EmEncryptArithmetic(音频加密类型)
 *  + EmEncryptArithmetic(视频加密类型)
 *  + TEncryptKey(音频加密信息)
 *  + u32 音频动态载荷
 *  + TEncryptKey(视频加密信息)
 *  + u32 视频动态载荷

 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(ev_ConfStartMultiCastTerRsp) 
   _body ( TMtId   , 1             )  
   _body ( u32     , 1             ) 
   _body ( u16     , 1             ) 
   _body ( u16     , 1             )
   _body ( u8      , 1             )
   _body ( u8      , 1             )
   _body( EmEncryptArithmetic,  1  )
   _body( EmEncryptArithmetic,  1  )
   _body( u32,  1  )
   _body( TEncryptKey,  1  )
   _body( u32,  1  )
   _body( TEncryptKey,  1  )
   _ev_end



/**********************<< 请求停止组播 >>**************	
 *[消息体]			 
 *  +TMtId
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfStopMultiCastTerReq  ) 
   _body ( TMtId   , 1          )
   _ev_end
   
/***********************<< MTC MT消息同步 >>**********************	
 *[消息体]			 
 *  NULL
 *[消息方向]
  * UI <- MtService
 */_event( ev_UISynchronizeInd )
   _ev_end

/***********************<< MTC MT消息同步 >>**********************	
 *[消息体]			 
 *  NULL
 *[消息方向]
  * mcservice <- h323service
 */_event( ev_ConfInnerMcAdjustVidResCmd )
   _body( u8, 1 )
   _body( u8, 1 )
   _ev_end
   
/***********************<< 输入会议密码响应, DTMF功能 >>***********************	
 *[消息体]			 
 *  +s8  密码
 *[消息方向]
 *
 *   MtService <=> 323/sip/320
 *   MtService => UI
 */_event(  ev_ConfPasswordDtmf   )
   _body (  s8   , REMAIN       )
   _ev_end
   

/***********************<< MC请求对方调整编码帧率命令 >>***********************	
 *[消息体]			 
 *  +u8      1     编码帧率
 *[消息方向]
 *
 *   mcservice => 323/sip/320
 */_event(  ev_ConfInnerMcAdjustFrameRateCmd   )
   _body (  u8      , 1       )
   _ev_end

/***********************<< MC请求对方调整编码帧率指示 >>***********************	
 *[消息体]			 
 *  +u8      1     编码帧率
 *[消息方向]
 *
 *   mcservice <= 323/sip/320
 */_event(  ev_ConfInnerMcAdjustFrameRateInd   )
   _body (  u8      , 1       )
   _ev_end

/***********************<< 当前会议具有FEC能力指示 >>***********************	
 *[消息体]			 
 *  +u8      1     FEC算法类型 0:NONE  1:RAID5  2:RAID6
 *[消息方向]
 *
 *   mcservice <= 323/sip/320
 */_event(  ev_ConfFecCapacityTypeInd   )
   _body (  u8      , 1       )
   _ev_end

   

#ifndef _MakeMtEventDescription_
   _ev_segment_end(conference)
};
#endif
#endif

