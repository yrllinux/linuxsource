#ifndef CHAN_EVENT_H
#define CHAN_EVENT_H
#include "innereventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmMtChan
{
#endif 
   _ev_segment( chan )//MtService 通道消息

	
/***********************<< 允许打开通道通知 >>************************	
 *[消息体]			 
 *  + EmChanType
 *
 *[消息方向]
 *
 *   StackIn => MtH323Service
 */_event(  ev_ChanCanOpenInd   )
   _body(   EmChanType  ,1      )
   _ev_end


/***********************<< 打开通道命令 >>************************	
 *[消息体]			 
 *  + EmChanType
 *  + TIPTransAddr    打开通道方RTCP接收地址
 *  + EmMtVideoPort   仅第二路视频有效
 *[消息方向]
 *
 *   StackIn <=> MtH323Service
 */_event(  ev_ChanOpenCmd   )
   _body(   EmChanType     ,1      )
   _body(   TIPTransAddr   ,1      )
   _body(   EmMtVideoPort  ,1      )
   _ev_end


/***********************<< 打开通道响应 >>************************	
 *[消息体]			 
 *  + EmChanType
 *    + TIPTransAddr    RTP  地址
 *    + TIPTransAddr    响应通道方RTCP接收地址
 *    + u32             回调函数指针 仅320系统有效
 *[消息方向]
 *
 *   StackIn <=> MtH323Service
 */_event(  ev_ChanOpenRsp   )
   _body(   EmChanType     ,1      )
   _body(   TIPTransAddr   ,1      )
   _body(   TIPTransAddr   ,1      )
   _body(   u32            ,1      )
   _ev_end


 /***********************<< 通道建立成功指示 >>************************	
 *[消息体]			 
 *  + EmChanType
 *    ......视频.........音频..........其他
 *  + TVideoChanParam TAudioChanParam  null
 *[消息方向]
 *
 *   StackIn <=> MtH323Service
 */_event(  ev_ChanConnectedInd   )
   _body(   u8     ,REMAIN        )
   _ev_end


 /***********************<< 关闭通道命令 >>************************	
 *[消息体]			 
 *  + EmChanType
 *[消息方向]
 *
 *   StackIn <=> MtH323Service
 */_event(  ev_ChanCloseCmd   )
   _body(   EmChanType     ,1      )
   _ev_end


/***********************<< 关闭通道指示 >>************************	
 *[消息体]			 
 *  + EmChanType
 *[消息方向]
 *
 *   StackIn <=> MtH323Service
 */_event(  ev_ChanDisconnectedInd   )
   _body(   EmChanType     ,1      )
   _ev_end


/***********************<< 动态载荷通知 >>************************	
 *[消息体]			 
 *  + EmChanType
 *  + u8    dynamic payload
 *[消息方向]
 *
 *   StackIn <=> MtH323Service
 */_event(  ev_ChanDynamicPayloadInd   )
   _body(   EmChanType     ,1      )
   _body(   u8             ,1      )
   _ev_end


/***********************<< 密钥通知 >>************************	
 *[消息体]			 
 *  + EmChanType
 *  + TEncryptKey
 *[消息方向]
 *
 *   StackIn <=> MtH323Service
 */_event(  ev_ChanEncryptKeyInd   )
   _body(   EmChanType     ,1      ) 
   _body(   TEncryptKey    ,1      ) 
   _ev_end



/***********************<< 设置视频通道码率 >>************************	
 *[消息体]			 
 *  + EmChanType
 *  + u16    单位 Kbps
 *[消息方向]
 *
 *   MtService => 323/sip/320
 */_event(  ev_ChanSetBitrateCmd   )
   _body(   EmChanType     ,1      ) 
   _body(   u16            ,1      ) 
   _ev_end


/***********************<< 通道码率指示 >>************************	
 *[消息体]			 
 *  + EmChanType
 *  + u16    单位 Kbps
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 */_event(  ev_ChanBitrateInd   )
   _body(   EmChanType     ,1      ) 
   _body(   u16            ,1      ) 
   _ev_end


/***********************<< 请求关键(I)帧 >>***********************	
 *[消息体]			 
 *  + EmChanType
 *[消息方向]
 *
 *   MtService <=> 323/sip/320
 */_event(  ev_ChanIFrameCmd   )
   _body(   EmChanType     ,1      ) 
   _ev_end


/***********************<< 通道状态指示 >>***********************	
 *[消息体]			 
 *  + EmChanType
 *  + BOOL           TRUE 建立 FALSE关闭
 *[消息方向]
 *
 *   MtService <=> 323/sip/320
 */_event(  ev_ChanStateInd   )
   _body(   EmChanType     ,1      ) 
   _body(   BOOL           ,1      ) 
   _ev_end


/***********************<< 通知通道物理带宽变化指示 >>***********************	
 *[消息体]			 
 *  + EmChanType    无意义
 *  + u16           当前物理带宽
 *[消息方向]
 *
 *   MtService <=> 323/sip/320
 */_event(  ev_ChanBandwidthChangeInd   )
   _body(   EmChanType     ,1      ) 
   _body(   u16            ,1      ) 
   _ev_end

//[xujinxing-2006-06-07]
/***********************<<  调整通道分辨率 >>********************** 
 *[消息体]
//*    +EmChanType
//*    +EmVideoResolution
 *[消息方向]
 *  h323service -> mtservice(channelservie)
 */_event( ev_ChanAdjustVideoResCmd )
   _body( EmChanType, 1 )
   _body( EmVideoResolution, 1 )   
   _ev_end
 

//[xujinxing-2006-08-08]
/***********************<<  设置组播接收地址 >>********************** 
 *[消息体]
//*    +EmChanType
//*    +u32  ip
//*    +u16  port
 *[消息方向]
 *  h323service -> mtservice(channelservie)
 */_event( ev_ChanMultiCastAddrCmd)
   _body( EmChanType, 1 )
   _body( u32_ip,  1 )  
   _body( u16, 1 ) 
   _ev_end
   
//[Fangtao,2006-12-26]
/***********************<<  设置音频输入输出音量命令 >>********************** 
 *[消息体]
//*    +u8  Input volume
//*    +u8  Output volume
 *[消息方向]
 *  h323service -> mtservice(channelservie)
 */_event( ev_ChanSetAudioVolumeCmd)
   _body( u8,  1 )  
   _body( u8,  1 ) 
   _ev_end


/***********************<< 设置视频解码器的动态载荷 >>********************** 
 *[消息体]
//*    +u8  Input volume
//*    +u8  Output volume
 *[消息方向]
 *  h323service -> mtservice(channelservie)
 */_event( ev_ChanDynamicPayloadParamInd )
   _body( EmChanType, 1 )
   _body( EmVideoFormat,  1 )  
   _body( u8,  1 ) 
   _ev_end
   
/***********************<< 设置视频解码器的动态载荷 >>********************** 
 *[消息体]
//*    EmChanType
//*    bool
 *[消息方向]
 *  h323service -> mtservice(channelservie)
 */_event( ev_ChanSetReverseG7221cCmd )
   _body( EmChanType, 1 )
   _body( BOOL,  1 )  
   _ev_end

/***************<< 远端自环命令 >>***********************	
 *[消息体]			 
 *  + EmChanType= emChanTypeEnd 无含义)
   _ev_end仅为消息一致而保留
 *[消息方向]
 *
 *    H239 class  (323 内部使用)
 */_event( ev_chanRemoteLoopCmd )
   _body( EmChanType, 1 )
   _body( BOOL,  1 )  
   _ev_end

  /***********************<< 实际采用的编码参数 >>**********************	
 *[消息体]	 		 
 *  + EmCodecComponent  =e
 *  + TVideoEncodeParameter 
 *[消息方向]
 *  MtService  ->Codec
 */_event( ev_ChanRealVideoEncodeParamInd )
   _body ( EmChanType    , 1    )
   _body ( u32 , 1 )  //宽
   _body ( u32 , 1 ) //高
   _body(  u8 , 1 ) //帧率
   _ev_end
   


#ifndef _MakeMtEventDescription_
   _ev_segment_end(chan)
};
inline BOOL IsMtChanEvent(u16 wEvent)
{
	return (wEvent > get_first_event_val(chan)) && ( wEvent < get_last_event_val(chan));
}
#endif


#endif
