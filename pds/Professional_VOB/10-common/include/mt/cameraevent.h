#ifndef CAMERA_EVENT_H
#define CAMERA_EVENT_H
#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmCamera
{
#endif 
     _ev_segment( camera)//摄像头消息
	
/*********************<< 终端摄像头控制源  >>*********************	
 *[消息体]			 
 *    EmSite
 *
 *[消息方向]
 *
 *   MtService <= UI
 */_event(  ev_CameraControlSourceCmd   )
   _body (  EmSite        ,1         ) 
   _ev_end

/*********************<< 终端摄像头控制源指示  >>*********************	
 *[消息体]			 
 *    EmSite
 *
 *[消息方向]
 *
 *   MtService <= UI
 */_event(  ev_CameraControlSourceInd   )
   _body (  EmSite        ,1         ) 
   _ev_end


/*********************<< 终端支持摄像头类型类表  >>*********************	
 *[消息体]			 
 *    + TCameraTypeInfo[]  摄像头类型列表
 *
 *[消息方向]
 *
 *   MtService =>UI
 */_event(  ev_CameraTypeCapsetInd   )
   _body ( TCameraTypeInfo        ,REMAIN ) 
   _ev_end



/***********************<< 摄像头状态指示  >>************************	
 *[消息体]			 
 *    + BOOL[MT_MAX_CAMERA_NUM]   TRUE= 连接成功 FALSE =未连接成功
 *
 *[消息方向]
 *    MtDevice => MtService
 *
 *   MtService => UI
 */_event(  ev_CameraStatusInd     )
   _body ( BOOL        ,MT_MAX_CAMERA_NUM )
   _ev_end



/***********************<< 选择控制射像头命令  >>************************	
 *[消息体]			 
 *    + u8  摄像头索引 1-6
 *
 *[消息方向]
 *
 *   MtService => MtDevice
 */_event(  ev_CameraSelectCmd   )
   _body(   u8         ,       1 )
   _ev_end



 /***********************<< 选择控制射像头命令  >>************************	
 *[消息体]			 
 *    +TMtId ( Source )
 *    +TMtId ( Dest ) 
 *    + u8  摄像头索引 1-6
 *
 *[消息方向]
 *
 *   MtService => MtDevice
 */_event(  ev_FeccCameraSelectCmd   )
   _body ( TMtId        ,1 )
   _body ( TMtId        ,1 )
   _body ( u8           ,1 )
   _ev_end


/***********************<< 摄像头移动命令  >>************************	
 *[消息体]			  
 *    + EmDirection
 *    + EmAction      emStart/emStop
 *
 *[消息方向]
 *
 *   UI        => MtService
 *   Protocol  => MtService
 *   MtService => MtDevice
 */_event(  ev_CameraPantiltCmd	  )
   _body ( EmDirection  ,1 )
   _body ( EmAction     ,1 )
   _ev_end



 /***********************<< 摄像头移动命令  >>************************	
 *[消息体]			 
 *    +TMtId ( Source )
 *    +TMtId ( Dest ) 
 *    + EmDirection
 *    + EmAction      emStart/emStop
 *
 *[消息方向]
 *
 *   UI        => MtService
 *   Protocol  => MtService
 *   MtService => MtDevice
 */_event(  ev_FeccCameraPantiltCmd	  )
   _body ( TMtId        ,1 )
   _body ( TMtId        ,1 )
   _body ( EmDirection  ,1 )
   _body ( EmAction     ,1 )
   _ev_end


/***********************<< 摄像头聚焦命令  >>************************	
 *[消息体]			 
 *    + BOOL      TRUE=近焦 FALSE=远焦
 *   + EmAction      emStart/emStop
 *
 *[消息方向]
 *
 *   UI        => MtService
 *   Protocol  => MtService
 *   MtService => MtDevice
 */_event(  ev_CameraFocusCmd   )
   _body ( BOOL         ,1 )
   _body ( EmAction     ,1 )
   _ev_end
 

  /***********************<< 摄像头聚焦命令  >>************************	
 *[消息体]			 
 *    +TMtId ( Source )
 *    +TMtId ( Dest ) 
 *	  + BOOL      TRUE=近焦 FALSE=远焦
 *   + EmAction      emStart/emStop
 *
 *[消息方向]
 *
 *   UI        => MtService
 *   Protocol  => MtService
 *   MtService => MtDevice
 */_event(  ev_FeccCameraFocusCmd   )
   _body ( TMtId        ,1 )
   _body ( TMtId        ,1 )
   _body ( BOOL         ,1 )
   _body ( EmAction     ,1 )
   _ev_end


/***********************<< 摄像头自动聚焦命令  >>**********************	
 *[消息体]			 
 *    无消息体
 *
 *[消息方向]
 *
 *   UI        => MtService
 *   Protocol  => MtService
 *   MtService => MtDevice
 */_event(  ev_CameraAutoFocusCmd	  	  )
   _ev_end


/***********************<< 摄像头自动聚焦命令  >>**********************	
 *[消息体]			 
 *    +TMtId ( Source )
 *    +TMtId ( Dest ) 
 *
 *[消息方向]
 *
 *   UI        => MtService
 *   Protocol  => MtService
 *   MtService => MtDevice
 */_event(  ev_FeccCameraAutoFocusCmd	  	  )
   _body ( TMtId        ,1 )
   _body ( TMtId        ,1 )
   _ev_end

 /***********************<< 摄像头自动背光补偿  >>************************	
 *[消息体]			 
 *    + BOOL      TRUE=自动背光 FALSE=取消自动背光
  *
 *[消息方向]
 *
 *   UI        => MtService
 *   Protocol  => MtService
 *   MtService => MtDevice
 */_event(  ev_CameraBackLightCmd   )
   _body ( BOOL         ,1 )
   _ev_end
 

/***********************<< 摄像视野命令命令  >>************************	
 *[消息体]			 
 *    + BOOL          TRUE= 放大 /FALSE=缩小
 *    + EmAction      emStart/emStop
 *
 *[消息方向]
 *
 *   UI        => MtService
 *   Protocol  => MtService
 *   MtService => MtDevice
 */_event(  ev_CameraZoomCmd                )
   _body ( BOOL         ,1 )
   _body ( EmAction     ,1 )
   _ev_end


/***********************<< 摄像视野命令命令  >>************************	
 *[消息体]			 
 *    +TMtId ( Source )
 *    +TMtId ( Dest ) 
 *    + BOOL          TRUE= 放大 /FALSE=缩小
 *    + EmAction      emStart/emStop
 *
 *[消息方向]
 *
 *   UI        => MtService
 *   Protocol  => MtService
 *   MtService => MtDevice
 */_event(  ev_FeccCameraZoomCmd                )
   _body ( TMtId        ,1 )
   _body ( TMtId        ,1 )
   _body ( BOOL         ,1 )
   _body ( EmAction     ,1 )
   _ev_end

/***********************<< 摄像亮度调节命令  >>************************	
 *[消息体]			 
 *    + BOOL     TRUE= 加亮  FALSE=减暗
 *    + EmAction      emStart/emStop
 *
 *[消息方向]
 *
 *   UI        => MtService
 *   Protocol  => MtService
 *   MtService => MtDevice
 */_event(  ev_CameraBrightCmd		  )
   _body ( BOOL         ,1 )
   _body ( EmAction     ,1 )
   _ev_end
 

/***********************<< 摄像亮度调节命令  >>************************	
 *[消息体]			 
 *    +TMtId ( Source )
 *    +TMtId ( Dest ) 
 *    + BOOL     TRUE= 加亮  FALSE=减暗
 *    + EmAction      emStart/emStop
 *
 *[消息方向]
 *
 *   UI        => MtService
 *   Protocol  => MtService
 *   MtService => MtDevice
 */_event(  ev_FeccCameraBrightCmd		  )
   _body ( TMtId        ,1 )
   _body ( TMtId        ,1 )
   _body ( BOOL         ,1 )
   _body ( EmAction     ,1 )
   _ev_end

 
 
/***********************<< 预置位命令  >>************************	
 *[消息体]	
 *    + BOOL    TRUE= 存储 FALSE=调用预置位		 
 *    + u8      预置位索引 1-16
 *
 *[消息方向]
 *
 *   UI        => MtService
 *   Protocol  => MtService
 *   MtService => MtDevice
 */_event(  ev_CameraPresetCmd  		  )
   _body ( BOOL         ,1 )
   _body ( u8           ,1 )
   _ev_end

 

/***********************<< 预置位命令  >>************************	
 *[消息体]	
 *    +TMtId ( Source )
 *    +TMtId ( Dest ) 
 *    +BOOL    TRUE= 存储 FALSE=调用预置位		 
 *    + u8      预置位索引 1-16
 *
 *[消息方向]
 *
 *   UI        => MtService
 *   Protocol  => MtService
 *   MtService => MtDevice
 */_event(  ev_FeccCameraPresetCmd  		  )
   _body ( TMtId        ,1 )
   _body ( TMtId        ,1 )
   _body ( BOOL         ,1 )
   _body ( u8           ,1 )
   _ev_end

 

/***********************<< 获取摄像头类型  >>************************	
 *[消息体]	
 *    +TMtId ( Source )
 *    +TMtId ( Dest ) 
 *    +BOOL    TRUE= 存储 FALSE=调用预置位		 
 *    + u8      预置位索引 1-16
 *
 *[消息方向]
 *
 *   UI        => MtService
 *   MtService => MtDevice
 */_event(  ev_CameraGetCapCmd   )
   _body ( TMtId        ,1 )
   _body ( TMtId        ,1 )
   _body ( BOOL         ,1 )
   _body ( u8           ,1 )
   _ev_end


/***********************<< 音频功率请求 >>************************	
 *[消息体]			 
 * 无
 *[消息方向]
 *    MtService <= UI
 */_event(  ev_ApplyAudioPowerCmd  )
   _ev_end

/***********************<< 音频功率指示 >>************************	
 *[消息体]			 
 *  u32  输入音频功率
 *  u32  输输出音频功率
 *[消息方向]
 *    MtService => UI
 */_event(  ev_AudioPowerInd  )
   _body ( u32    ,1          )
   _body ( u32    ,1          )
   _ev_end
#ifndef _MakeMtEventDescription_
   _ev_segment_end(camera)
};
#endif   

#endif

