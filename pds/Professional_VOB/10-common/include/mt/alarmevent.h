#ifndef ALAEM_EVENT_H
#define ALAEM_EVENT_H

#include "eventcomm.h"


#ifndef _MakeMtEventDescription_
enum EmAlarm
{
#endif  
 _ev_segment(alarm )//告警消息

/***********************<< 初始化告警APP  >>************************	
 *[消息体]			 
 *    TSNMPCfg
 *
 *[消息方向]
 *    MT => AGENT
 */_event(  ev_AgtInitInd  )
   _body( TSNMPCfg, 1 )
   _ev_end


/***********************<< E1线路状态改变 >>************************	
 *[消息体]			 
 *    TAlarmCode
 *
 *[消息方向]
 *    MT => AGENT
 */_event(  ev_AgtE1LineInd  )
   _body( u32, 1 )
   _ev_end


/***********************<< 内存状态改变 >>************************	
 *[消息体]			 
 *    TAlarmCode
 *
 *[消息方向]
 *    MT => AGENT
 */_event(  ev_AgtMemoryInd  )
 _body( u32, 1 )
   _ev_end


/***********************<< 文件系统状态改变 >>************************	
 *[消息体]			 
 *    TAlarmCode
 *
 *[消息方向]
 *    MT => AGENT
 */_event(  ev_AgtFileSystemInd  )
   _ev_end


/***********************<< 任务状态改变 >>************************	
 *[消息体]			 
 *    TAlarmCode
 *
 *[消息方向]
 *    MT => AGENT
 */_event(  ev_AgtTaskInd  )
 _body( u32, 1 )
   _ev_end



/***********************<< MAP状态改变 >>************************	
 *[消息体]			 
 *    TAlarmCode
 *
 *[消息方向]
 *    MT => AGENT
 */_event(  ev_AgtMapInd  )
 _body( u32, 1 )
   _ev_end


 
/***********************<< CCI状态改变 >>************************	
 *[消息体]			 
 *    TAlarmCode
 *
 *[消息方向]
 *    MT => AGENT
 */_event(  ev_AgtCCIInd  )
 _body( u32, 1 )
   _ev_end



/***********************<< 以太网状态 >>************************	
 *[消息体]			 
 *    TAlarmCode
 *
 *[消息方向]
 *    MT => AGENT
 */_event(  ev_AgtEthernetInd  )
 _body( u32, 1 )
   _ev_end

/***********************<< 以太网状态 >>************************	
 *[消息体]			 
 *    BOOL
 *
 *[消息方向]
 *    MT => UI
 */_event(  ev_EthernetStateInd  )
 _body( u32, 1 )
   _ev_end


/***********************<< E1状态 >>************************	
 *[消息体]			 
 *    BOOL
 *
 *[消息方向]
 *    MT => UI
 */_event(  ev_E1StateInd  )
 _body( u32, 1 )
   _ev_end


/***********************<< 媒体流状态 >>************************	
 *[消息体]			 
 *    TAlarmCode
 *
 *[消息方向]
 *    MT => AGENT
 */_event(  ev_AgtMediaStreamInd  )
 _body( u32, 1 )
   _ev_end


/***********************<< 设备连接状态改变 >>************************	
 *[消息体]			 
 *    TAlarmCode
 *
 *[消息方向]
 *    MT => AGENT
 */_event(  ev_AgtEqpConnectInd  )
 _body( u32, 1 )
   _ev_end



/***********************<< 视频矩阵的状态 >>************************	
 *[消息体]			 
 *    TAlarmCode
 *
 *[消息方向]
 *    MT => AGENT
 */_event(  ev_AgtVidoSourceInd  )
 _body( u32, 1 )
   _ev_end



/***********************<< 风扇状态改变 >>************************	
 *[消息体]			 
 *    TAlarmCode
 *
 *[消息方向]
 *    MT => AGENT
 */_event(  ev_AgtFanInd  )
 _body( u32, 1 )
   _ev_end



/***********************<< V35状态改变 >>************************	
 *[消息体]			 
 *    TAlarmCode
 *
 *[消息方向]
 *    MT => AGENT
 */_event(  ev_AgtV35Ind  )
 _body( u32, 1 )
   _ev_end



/***********************<< 面板灯状态通知 >>************************	
 *[消息体]			 
 *    TAlarmCode
 *
 *[消息方向]
 *    MT => AGENT
 */_event(  ev_AgtPaneLEDInd  )
 _body( u32, 1 )
   _ev_end



/***********************<< 线路带宽通知 >>************************	
 *[消息体]			 
 *    U32
 *
 *[消息方向]
 *    MT => MTService
 */_event(  ev_AgtGuardChanBWInd  )
   _ev_end



/***********************<< E1状态自环通知 >>************************	
 *[消息体]			 
 *    TAlarmCode
 *
 *[消息方向]
 *    MT => AGENT
 */_event(  ev_AgtE1LoopInd  )
 _body(BOOL , 1)
 _body( u32, 1 )
 _ev_end


/***********************<< 系统重启后通知 >>************************	
 *[消息体]			 
 *
 *[消息方向]
 *    MT => AGENT
 */_event(  ev_AgtSysRestarInd  )
 _ev_end

#ifndef _MakeMtEventDescription_
   _ev_segment_end(alarm)
};
#endif

#endif

