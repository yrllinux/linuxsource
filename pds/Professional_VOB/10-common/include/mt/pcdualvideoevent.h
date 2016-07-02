
#ifndef PCDUALVIDEO_EVENT_H
#define PCDUALVIDEO_EVENT_H

#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmPCDualVideo
{
#endif
/***********************************************************/
/*                                                         */
/*                 用户管理消息                            */
/*    osp 消息                                             */
/***********************************************************/

    _ev_segment( pcdualvideo )

 
/***********************<< PC双流软件请求登录消息 >>********************	
 *[消息体]			 
 *   + u8[32]         //通信数据结构版本描述
 *   + CLoginRequest
 *[消息方向]
 *    PC双流软件 => UE
 */_event(  ev_PCLoginReq   )
   _ev_end



/***********************<< PC双流软件请求登录确认应答 >>********************	
 *[消息体]			 
 *   无消息体
 *[消息方向]
 *   UE => PC双流软件 
 */_event(  ev_PCLoginAck   )
   _ev_end



 /***********************<< PC双流软件请求登录拒绝应答 >>********************	
 *[消息体]			 
 *   + u8[32]         //通信数据结构版本描述
 *   + u8             // 登陆失败原因   0=用户管理模块 1=已有用户登陆 2＝软件版本布匹配
 *   + u16            // 用户管理模块错误号
 *   + CUserFullInfo  //已登陆用户信息
 *   + u32            //已登陆用户IP
 *[消息方向]
 *   UE => PC双流软件 
 */_event(  ev_PCLoginNack   )
   _ev_end
   

 /***********************<< PC双流软件能否发起双流指示 >>********************	
 *[消息体]			 
 *   + BOOL         //
 *[消息方向]
 *   UE => PC双流软件 
 */_event(  ev_PCCanSendVideoInd   )
   _body( BOOL, 1 )
   _ev_end



/***********************<< 终端状态指示 >>********************	
 *[消息体]			 
 * + EmMtModel      //终端型号
 * + BOOL          //是否在会议中
 * + EmConfMode    //会议类型
 *[消息方向]
 *   MT => PC双流软件 
 */_event(  ev_PCMtStatusInd   )
   _body( EmMtModel, 1 )
   _body( BOOL, 1 )
   _body( EmConfMode, 1)
   _ev_end
   
/***********************<< 终端双流状态指示 >>********************	
 *[消息体]
 * + BOOL         //是否在发送双流
 * + BOOL         //是否在接收双流
 *[消息方向]
 *   MT => PC双流软件 
 */_event(  ev_PCDualVideoStatusInd   )
   _body( BOOL, 1 )
   _body( BOOL, 1 )
   _ev_end



/***********************<< PC双流软件视频分辨率指示 >>********************	
 *[消息体]			 
 * +EmVideoResolution
 *[消息方向]
 *   MT <= PC双流软件 
 */_event(  ev_PCVideoResInd   )
   _body( EmVideoResolution, 1 )
   _ev_end


/***********************<< 终端支持的视频分辨率指示 >>********************	
 *[消息体]
 * +EmVideoResolution
 *[消息方向]
 *   MT => PC双流软件 
 */_event(  ev_PCVideoResMtCapInd )
   _body( EmVideoResolution, 1 )
   _ev_end

 /***********************<< pc双流软件开始/停止双流命令  >>************************
 *[消息体] 
 *  +EmMtVideoPort    //emPC
 *  +BOOL             TRUE=开始 FALSE =停止
 *  +TIPTransAddr   
 *[消息方向]
 *
 *  mtc =>  MtService 
 */_event(  ev_PCSepDualVideoCmd   )
   _body ( EmMtVideoPort   , 1 )//视频端口
   _body ( BOOL            , 1 )//TRUE=开始 FALSE =停止
   _body ( TIPTransAddr    , 1 ) //编码器本地接收的rtcp地址和端口
   _ev_end


#ifndef _MakeMtEventDescription_
   _ev_segment_end( pcdualvideo )
};
#endif

#endif