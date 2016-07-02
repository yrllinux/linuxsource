
#ifndef CALL_EVENT_H
#define CALL_EVENT_H
#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmMtCall
{
#endif  
  
  
    _ev_segment( mtcall  )


/***********************<< 呼叫配置通知 >>************************
 *[消息体]			 
 *  + TCallConfig
 *
 *[消息方向]
 *    MtService => 323/320/sip
 */_event(  ev_CallConfigInd   )
   _body ( s8    ,REMAIN        )
   _ev_end

	
/***********************<< 建立呼叫  >>************************
 *[消息体]			 
 *  + TDialParam
 *
 *[消息方向]
 *    UI        => MtService
 *    MtService => 323/320/sip
 */_event(  ev_MakeCallCmd     )
   _body (  TDialParam    ,1   )
   _ev_end


/***********************<< 呼叫正在处理指示  >>************************
 *[消息体]			 
 *  无
 *
 *[消息方向]
 *
 *    MtService <= 323/320/sip
 */_event(  ev_CallProccedingInd  )
   _ev_end

/***********************<< 建立进入  >>************************
 *[消息体]			 
 *  + TDialParam
 *
 *[消息方向]
 *
 *    MtService <= 323/320/sip
 */_event(  ev_CallIncomingInd   )
   _body ( TDialParam    ,1      )
   _ev_end



/***********************<< 呼叫建立成功  >>************************
 *[消息体]			 
 *  无消息体
 *
 *[消息方向]
 *
 *    MtService <= 323/320/sip
 */_event(  ev_CallConnectedInd   )
   _ev_end


/***********************<< 接受呼叫指示  >>************************
 *[消息体]			 
 *  + BOOL  TRUE=接受呼叫  FALSE=拒绝呼叫  
 *
 *[消息方向]
 *
 *    MtService => 323/320/sip
 */_event(  ev_CallAcceptInd   )
   _body ( BOOL    ,1      )
   _ev_end



/***********************<< 呼叫挂断指示  >>************************
 *[消息体]			 
 *  +u8  byReason	挂断原因
 *
 *[消息方向]
 *
 *    MtService <=> 323/320/sip
 */_event(  ev_CallDisconnectedInd     )
   _body(  EmCallDisconnectReason ,1   )
   _ev_end

 
/***********************<< 挂断呼叫  >>************************
 *[消息体]			 
 *  +u8  byReason	挂断原因
 *
 *[消息方向]
 *
 *    MtService <=> 323/320/sip
 */_event(  ev_CallHangupCmd   )
   _body(  u8   ,  1   )//挂断原因
   _ev_end


 
/***********************<< 存活超时指示  >>************************
 *[消息体]			 
 *  +u32
 *
 *[消息方向]
 *
 *    MtService <=> 323/320/sip
 */_event(  ev_CallKeepLiveTimeOutInd   )
   _body ( u32   ,1                     )
   _ev_end






/***********************<< 模式建立超时  >>************************
 *[消息体]			 
 *  +TLinkState  
 *
 *[消息方向]
 *
 *    MtService <=> 323/320/sip
 */_event(  ev_CallEntryModeInd   )
   _body ( TLinkState   ,1        )
   _ev_end




 /***********************<< 开始双流命令  >>************************
 *[消息体]			 
 *  +EmMtVideoPort  
 *  +BOOL             TRUE=开始 FALSE =停止
 *[消息方向]
 *
 *    MtService <=> 323/320/sip
 */_event(  ev_DualVideoCmd   )
   _body ( EmMtVideoPort   , 1 )//视频端口
   _body ( BOOL            , 1 )//TRUE=开始 FALSE =停止
   _ev_end





/***********************<< 开始双流命令  >>************************
 *[消息体]			 
 *  +EmMtVideoPort  
 *  +BOOL             TRUE=开始 FALSE =停止
 *[消息方向]
 *
 *    MtService <=> 323/320/sip
 */_event(  ev_DualVideoInd   )
   _body ( EmMtVideoPort   , 1 )//视频端口
   _body ( BOOL            , 1 )//TRUE=开始 FALSE =停止
   _ev_end

/***********************<< 请求呼叫统计信息 >>************************	
 *[消息体]		 
 *   无 
 *[消息方向]
 *    UI => MtService
 */_event ( ev_ApplyCallInfoStatisticsCmd )
  _ev_end

/***********************<< 呼叫统计信息指示 >>************************	
 *[消息体]		 
 *  +TCallInfoStatistics  //统计信息
 *[消息方向]
 *    MtService => UI
 */_event ( ev_CallInfoStatisticsInd )
  _body ( TCallInfoStatistics, 1 )
  _ev_end 

/***********************<< 通道状态改变 >>************************	
 *[消息体]		 
 * EmChanType 
 * BOOL       
 *[消息方向]
 *    UI => MtService
 */_event ( ev_ChannelStatusChanged )
   _body ( EmChanType, 1 )
   _body ( BOOL, 1 )
  _ev_end

/***********************<< 会议是否加密指示 >>************************	
 *[消息体]		 
 *  +BOOL  //是否加密
 *[消息方向]
 *    MtService => UI
 */_event ( ev_CallEncryptInd )
  _body ( BOOL, 1 )
  _ev_end 

/***********************<< 接收双流指示  >>************************
 *[消息体]
 *  +BOOL             TRUE=开始 FALSE =停止
 *[消息方向]
 *
 *    MtService <=> 323/320/sip
 */_event(  ev_RecvDualVideoInd   )
   _body ( BOOL            , 1 )//TRUE=开始 FALSE =停止
   _ev_end
   
/**********************<< mt 请求mtc编码统计信息 >>********************	
 *[消息体]
 * 无
 *[消息方向]
 *    Mt  = > MTC
 */_event( ev_MTCEncStatisReq  )
   _ev_end

/**********************<< mtc编码统计信息回馈  >>********************	
 *[消息体]
 * TCodecPackStat  //统计信息
 *[消息方向]
 *    Mt  = > MTC
 */_event( ev_MTCEncStatisAck  )
   _body( TCodecPackStat , 1 )
   _ev_end

 
/***********************<< 组播模式地址指示  >>************************
 *[消息体]
 * +TIPTransAddr     音频接收地址
 * +TIPTransAddr     第一路视频接收地址
 * +TIPTransAddr     第二路视频接收地址
 *[消息方向]
 *
 *    MtService <= 323/320/sip
 */_event(  ev_MulticastModeAddrInd   )
   _body ( TIPTransAddr       , 1 )//音频接收地址
   _body ( TIPTransAddr       , 1 )//第一路视频接收地址
   _body ( TIPTransAddr       , 1 )//第二路视频接收地址
   _ev_end

     
   /***********************<< 向DNS查询域名IP地址  >>************************	
 *[消息体]			 
 *  + u32 DNS Server IP
 *  + u32 dwUserId 
 *  + u16 域名长度 
 *  + s8  域名 
 
 *[消息方向]
 *
 *    MtService => MtH323Service
 */_event( ev_DomainNameReq  )
   _body(  u32_ip    ,1             )
   _body(  u32       ,1             )
   _body(  u16       ,1             )
   _body(  s8        ,REMAIN             )
   _ev_end

    /***********************<< DNS查询域名IP地址响应  >>************************	
 *[消息体]			 
 *  + BOOL   查询结果
    + u32_ip 域名的IP地址
 *
 *[消息方向]
 *
 *    MtH323Service => MtService
 */_event( ev_DomainNameRsp  )
   _body(  BOOL          , 1             )
   _body(  u32_ip        , 1             )
   _ev_end

    /***********************<< 查询域名计时器  >>************************	
 *[消息体]			 
 *  + BOOL   查询结果
    + u32_ip 域名的IP地址
 *
 *[消息方向]
 *
 *    MtH323Service => MtService
 */_event( ev_DomainNameQueryTimeOut  )
   _ev_end
   
 /***********************<< 终端忙且无级联能力时拒绝呼叫指示  >>************************	
 *[消息体]			 
 * + u32_ip     1                        IP地址
   + s8         MT_MAX_H323ALIAS_LEN+1   别名
   + s8         MT_MAX_E164NUM_LEN+1     号码
   + u16        1                        呼叫码率
 *
 *[消息方向]
 *
 *    MtH323Service => MtService => MtUI
 */_event( ev_CallRejectWhenBusyInd  )
   _body( u32_ip      ,1      )
   _body( s8          ,MT_MAX_H323ALIAS_LEN+1)
   _body( s8          ,MT_MAX_E164NUM_LEN+1  )
   _body( u16         ,1      )
   _ev_end
   

/***********************<< 电话状态指示 >********************** 
 *[消息体]    
 *  EmFxoState
 *[消息方向]
 *
 *   MtService => UI
 *   
 */_event(  ev_FxoCallLinkStateInd )
   _body ( EmFxoState, 1       )
   _ev_end

/***********************<< 打电话 >********************** 
 *[消息体]    
 *  s8*  对方号码
 *[消息方向]
 *
 *   MtService <= UI
 *   
 */_event(  ev_FxoMakeCall )
   _body ( s8, REMAIN       )
   _ev_end


/***********************<< 挂断电话 >********************** 
 *[消息体]    
 *  s8*  对方号码
 *[消息方向]
 *
 *   MtService <= UI
 *   
 */_event(  ev_FxoHangup )
   _ev_end


/***********************<< 拒接电话 >********************** 
 *[消息体]    
 *[消息方向]
 *
 *   MtService <= UI
 *   
 */_event(  ev_FxoReject )
   _ev_end
         
   
/***********************<< 电话呼入指示 >********************** 
 *[消息体]    
 *  s8*  对方号码
 *[消息方向]
 *
 *   MtService => UI
 *   
 */_event(  ev_FxoCallIncomingInd )
   _body ( s8, REMAIN       )
   _ev_end
 
  
/***********************<< 接收电话 >********************** 
 *[消息体]    
 *  s8*  对方号码
 *[消息方向]
 *
 *   MtService <= UI
 *   
 */_event( ev_FxoAccept )
   _ev_end

/***********************<< 电话超时 >********************** 
 *[消息体]    
 *  
 *[消息方向]
 *
 *   MtService 
 *   
 */_event( ev_FxoSetupTimerInd )
   _ev_end

   
/***********************<< 对端不再等待被接或者链路超时 >********************** 
 *[消息体]    
 *  
 *[消息方向]
 *
 *   MtService => MTUI
 *   
 */_event( ev_FxoPeerNoLongerWaitingInd )
   _ev_end

   
/***********************<< 电话拨出发生故障 >********************** 
 *[消息体]    
 *  
 *[消息方向]
 *
 *   MtService => MTUI
 *   
 */_event( ev_FxoDialOutErrorInd )
   _ev_end


#ifndef _MakeMtEventDescription_
   _ev_segment_end(mtcall)
};
#endif
#endif

