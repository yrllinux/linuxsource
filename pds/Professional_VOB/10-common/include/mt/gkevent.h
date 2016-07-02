#ifndef GK_EVENT_H
#define GK_EVENT_H
#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmGK
{
#endif 

  _ev_segment( gk  )

/***********************<< 注测GK结果  >>************************
 *[消息体]			 
 *  + BOOL  TRUE = 注册成功 FALSE失败
 *
 *[消息方向]
 *
 *    MtService => UI
 *
 */_event(  ev_GKRegistResultInd   )
   _body ( BOOL            , 1     )//TRUE = 注册成功 FALSE失败
   _ev_end


/***********************<< 注册GK  >>************************
 *[消息体]			 
 *  + u32      GK IP地址
 *  + u32      注册本端IP
 *  + TMtAddr  E164
 *  + TMtAddr  H323id
 *[消息方向]
 *    UI        => MtService 
 *    MtService => H323
 *
 */_event(  ev_GKRegistReq   )
   _body(  u32      ,  1     )//GK IP地址
   _body(  u32      ,  1     )//注册本端IP
   _body(  TMtAddr  ,  1     )//E164
   _body(  TMtAddr  ,  1     )//H323id
   _ev_end


/***********************<< 注册GK  >>************************
 *[消息体]			 
 *  无
 *[消息方向]
 *
 *    UI        => MtService 
 *    MtService => H323
 *
 */_event(  ev_GKUnregistReq   )
   _ev_end


#ifndef _MakeMtEventDescription_
   _ev_segment_end(gk)
};
#endif
#endif

