#ifndef SERIAL_EVENT_H
#define SERIAL_EVENT_H
#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmSerial
{
#endif
  _ev_segment( serial)//串口头消息


/*********************<< 配置串口参数  >>****************************	
 *[消息体]			 
 *    + EmSerialType     232/422/485
 *    + TSerialCfg
 *
 *[消息方向]
 *
 *   UI          => MtService
 *   MtService   => MtDevice
 */_event(  ev_SerialPortConfigCmd  )
   _body ( EmSerialType ,1 )
   _body ( TSerialCfg   ,1 )
   _ev_end
       


/*********************<< 串口参数指示  >>****************************	
 *[消息体]			 
 *    + EmSerialType     232/422/485
 *    + TSerialCfg
 *
 *[消息方向]
 *
 *   MtService   => UI
 */_event(  ev_SerialPortConfigInd  )
   _body ( EmSerialType ,1 )
   _body ( TSerialCfg   ,1 )
   _ev_end


/*********************<< 发送透明串口数据  >>****************************	
 *[消息体]			 
 *    + TTransparentSerial
 *[消息方向]
 *
 *   MtService   => UI
 */_event(  ev_TransparentSerialDataCmd )
   _body ( TTransparentSerial ,1 )
   _ev_end
   

/*********************<< 接收透明串口数据  >>****************************	
 *[消息体]			 
 *    + TTransparentSerial
 *[消息方向]
 *
 *   MtService   => UI
 */_event(  ev_TransparentSerialDataInd)
   _body ( TTransparentSerial ,1 )
   _ev_end
   
/*********************<< 恢复串口默认值  >>****************************	
 *[消息体]			 
 *    + EmSerialType     232/422/485
 *
 *[消息方向]
 *
 *   UI          => MtService
 *   MtService   => MtDevice
 */_event(  ev_SeriesDefaultConfigCmd   )
   _body ( EmSerialType ,1 )
   _ev_end
     
 

/*********************<< 启动Iray  >>****************************	
 *[消息体]			 
 *    + u8 地址 
 *
 *[消息方向]
 *   MtService   => MtDevice
 */_event(  ev_IrayStartCmd   )
   _ev_end
#ifndef _MakeMtEventDescription_
   _ev_segment_end(serial)
};
#endif


#endif


