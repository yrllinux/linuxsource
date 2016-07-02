#ifndef MC_EVENT_H
#define MC_EVENT_H

#include "eventcomm.h"


#ifndef _MakeMtEventDescription_
enum EmMCEvent
{
#endif  
	  _ev_segment( MC )
	
/***********************<< 扩展会议请求 >>************************
 *[消息体]
 *  + TDialParam
 *  + s8[MT_MAX_NAME_LEN] 终端别名
 *[消息方向]
 *
 *    MCService <= MtService <= MTUI
 */_event( ev_mcAdHocCmd )
   _body ( TDialParam ,1 )
   _ev_end

/***********************<< 扩展会议应答 >>************************
 *[消息体]
 *  + TMtId
 *  + BOOL                       TRUE=同意 FALSE =拒绝
 *  + EmCallDisconnectReason     拒绝时候的原因
 *[消息方向]
 *
 *    MCService <= MtService <= MTUI
 */_event( ev_mcAcceptInd )
   _body( TMtId                   ,1 )
   _body( BOOL                    ,1 )
   _body( EmCallDisconnectReason  ,1 )
   _ev_end

/***********************<< 扩展会议呼叫进入请求 >>************************
 *[消息体]
 *  + TDialParam
 *[消息方向]
 *
 *    MCService <=> MtService <=> MTUI
 */_event( ev_mcCallIncommingInd )
   _body(  TMtId      ,1 )
   _body ( TDialParam ,1 )
   _ev_end   

/***********************<< 点对点时会议信息 >>************************
 *[消息体]
 *  +s8[MT_MAX_NAME_LEN+1] 对端别名
 *  +EmChanType 通道类型类型
 *[消息方向]
 *
 *    MCService <=> MtService <=> MTUI
 */_event( ev_mcP2PCallInfo )
   _body(  s8      , MT_MAX_NAME_LEN)
   _ev_end   

/**********************<< mc时界面替换画面复合终端命令  >>********************	
 *[消息体]
 * +u8    //画面的位置
 * +TMtId  //要替换成那个终端
 *[消息方向]
 *   ui  ->  mtservice
 */_event( ev_mcUIReplaceMtCmd )
   _body( u8,  1 )
   _body( TMtId,  1 )  
   _ev_end


/**********************<< mc音视频编码参数指示  >>********************	
 *[消息体]
 * +TVideoEncodeParameter    //vmp编码参数
 * +EmAudioFormat            //音频编码参数
 * +u8    //视频动态载荷
 * +u8    //音频动态载荷
 *[消息方向]
 *   mtservice  ->  mcservice
 */_event( ev_mcAVParamInfo )
   _body( TVideoEncodeParameter,  1 )
   _body( EmAudioFormat,  1 )  
   _body( u8,  1 )
   _body( u8, 1 )
   _ev_end 

   
/**********************<< mc能力状态指示  >>********************	
 *[消息体]
 * +BOOL          //mc是否允许扩展会议
 * +BOOL          //mc是否激活
 *[消息方向]
 *   mtservice  ->  UI
 */_event( ev_mcCapStateInd )
   _body( BOOL,  1 )
   _body( BOOL,  1 )  
   _ev_end

/**********************<< mc讨论终端状态指示  >>********************	
 *[消息体]
 * +TMtId[]          //参与画面合成的终端id号, 
                     //如果terId和mcuId都为0则为无效, 表示该位置没有终端.
					 //即该位置黑屏
 *[消息方向]
 *   mtservice  ->  UI
 */_event( ev_mcDiscussMmbInd )
   _body( TMtId,  6 ) 
   _body( EmInnerVMPStyle, 1 )
   _ev_end


/**********************<< mc远遥终端命令  >>********************	
 *[消息体]
 * +TMtId         //要远遥的终端id

 *[消息方向]
 *   UI  ->  mtservice
 */_event( ev_mcFeccTermCmd )
   _body( TMtId,  1 ) 
   _ev_end

/**********************<< mc远遥终端指示  >>******************** 
 *[消息体]
 * +TMtId         //要远遥的终端id
 *[消息方向]
 *   UI  ->  mtservice
 */_event( ev_mcFeccTermInd )
   _body( TMtId,  1 ) 
   _ev_end

/**********************<<  配置mc工作模式命令  >>********************	
 *[消息体]
 * BOOL        //mc是否启用 
 * EmMCMode    //mc工作模式
 *[消息方向]
 *   ui  ->  mtservice
 */_event( ev_mcModeCfgCmd )
   _body( BOOL,  1   ) 
   _body( EmMCMode ,  1 ) 
   _ev_end
  
/**********************<<  配置mc工作模式指示  >>********************	
 *[消息体]
 * BOOL        //mc是否启用 
 * EmMCMode    //mc工作模式
 *[消息方向]
 *   mtservice  ->  ui
 */_event( ev_mcModeCfgInd )
   _body( BOOL,  1   ) 
   _body( EmMCMode ,  1 ) 
   _ev_end
   

/**********************<<  设置画面合成自动增加/或者固定 >>********************	
 *[消息体]
 * +BOOL    //TRUE：画面自动增加；FALSE：画面风格固定
 *[消息方向]
 *   mtservice  ->  mcservice
 */_event( ev_mcVMPCfgCmd )
   _body( BOOL,  1 ) 
   _ev_end 

/**********************<<  设置画面合成自动增加/或者固定指示 >>********************	
 *[消息体]
 * +BOOL    //TRUE：画面自动增加；FALSE：画面风格固定
 *[消息方向]
 *   mtservice  ->  mcservice
 */_event( ev_mcVMPCfgInd )
   _body( BOOL,  1 ) 
   _ev_end 

/**********************<<  设置vmp的风格 >>********************	
 *[消息体]
 * +EmInnerVMPStyle          //画面的风格
 * +TMtId atMTLoc[6]    //画面合成位置上的终端id号
 *[消息方向]
 *   mtservice  ->  mcservice
 */_event( ev_mcSetVMPStyleCmd )
   _body( EmInnerVMPStyle,  1 )
   _body( TMtId, 6 ) 
   _ev_end 

/**********************<<  设置vmp的风格指示 >>********************	
 *[消息体]
 * +EmInnerVMPStyle      emStyle  //画面的风格，主/次
 * +TMtId atMTLoc[6]    //画面合成位置上的终端id号
 *[消息方向]
 *   mtservice  <-  mcservice
 */_event( ev_mcSetVMPStyleInd )
   _body( EmInnerVMPStyle,  1 )
   _body( TMtId, 6 ) 
   _ev_end 

/**********************<< mc终端轮询广播 >>********************	
 *[消息体]
 * +TMtPollInfo
 *[消息方向]
 *   mtservice  ->  mcservice
 */_event( ev_mcPollCmd)
   _body( TMtPollInfo,  1 )
   _ev_end 

/**********************<< mc终端轮询广播指示 >>********************	
 *[消息体]
 * +TMtPollInfo
 *[消息方向]
 *   mcservice  ->  mtservice
 */_event( ev_mcPollInd )
   _body( TMtPollInfo,  1 )
   _ev_end 
   
/**********************<< 添加成员 >>********************	
 *[消息体]
 * +TMtId          //
 *[消息方向]
 *   mcservice  ->  mtservice
 */_event( ev_mcAddEndPointCmd )
   _body( TMtId ,  1 )
   _ev_end
 

/**********************<< 删除成员 >>********************	
 *[消息体]
 * +u32          //CEndPoint的指针值
 *[消息方向]
 *   mcservice  ->  mtservice
 */_event( ev_mcDelEndPointCmd )
   _body( TMtId ,  1 )
   _ev_end 
   
   
/***********************<< 会议链路状态指示 >>**********************	
 *[消息体]			 
 *  TLinkState
 *[消息方向]
 *
 *   MtService <=> 323/sip/320
 *   McService => UI
 */_event(  ev_mcCallLinkStateInd   )
   _body ( TLinkState   , 1       )
   _body ( TMtId        , 1       )
   _ev_end

/***********************<< MC哑音状态指示 >>**********************	
 *[消息体]			 
 *  +TMtId 6
 *  +BOOL  6
 *[消息方向]
 *
 *   McService => MtService
 *   MtService => UI/MTC
 */_event(  ev_mcMakeTerMuteInd   )
   _body ( TMtId   , 6       )
   _body ( BOOL    , 6       )
   _ev_end  

/***********************<< MC会议结束指示 >>**********************	
 *[消息体]			 
 *
 *[消息方向]
 *
 *   McService => MtService
 *   MtService => UI/MTC
 */_event(  ev_mcConfEndInd   )
   _ev_end  
   
/***********************<< MC当前发言人指示 >>**********************	
 *[消息体]			 
 * +TMtId 1
 *[消息方向]
 *
 *   McService => MtService
 *   MtService => UI/MTC
 */_event(  ev_mcCurSpeekerInd   )
   _body ( TMtId   ,  1 )
   _ev_end

/***********************<< MC挂断当前正在呼叫的终端 >>**********************	
 *[消息体]			 
 * 
 *[消息方向]
 *
 *   MtService => McService
 *   UI/MTC => MtService
 */_event(  ev_mcHangUpAddingMtCmd   )
   _ev_end

   
/***********************<< 终端退出时上报自身ID >>**********************	
 *[消息体]			 
 * +TMtId 1
 *[消息方向]
 *
 *   MtService => McService
  */_event(  ev_mcDisconnectedMtInd   )
	_body ( TMtId   ,  1 )
    _ev_end

/***********************<< MC非法呼叫指示 >>**********************	
 *[消息体]			 
 * +u8 1          0: Call self  
                  1: the called ter already in MC 
				  255: Unkown err
 *[消息方向]
 *
 *   MtService => UI/MTC
  */_event(  ev_mcIllegalCallInd   )
	_body ( u8   ,  1 )
    _ev_end

	
/***********************<< 点对点会议时将呼叫码率通知MC >>**********************	
 *[消息体]			 
 * +u32   : 点对点呼叫码率

 *[消息方向]
 *
 *   MtService => mc
  */_event(  ev_mcConfRateInd   )
	_body ( u32   ,  1 )
    _ev_end
   
#ifndef _MakeMtEventDescription_
   _ev_segment_end(MC)

};
#endif//_MakeMtEventDescription_
#endif//MC_EVENT_H

