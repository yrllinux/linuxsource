#ifndef SITECALL_EVENT_H
#define SITECALL_EVENT_H
#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EMSitecall
{
#endif 
	   _ev_segment( sitecall )
/***********************<< 初始化主叫呼集命令〉〉************************	
 *[消息体]			 
 *    + THANDLE  句柄
 *
 *[消息方向]
 *
 *   Service => StackOut
 */_event( ev_SiteCallSetGKAddrCmd    )
 	_body(u32_ip ,1)
   _ev_end
/***********************<< 初始化主叫呼集回应〉〉************************	
 *[消息体]			 
 *    + BOOL
 *[消息方向]
 *
 *   Service <= StackOut
 */_event( ev_SiteCallSetGKAddrInd   )
   _body(BOOL , 1)
   _ev_end
/***********************<< 创建主叫呼集命令>>************************	
 *[消息体]			 
 *    + NULL
 *
 *[消息方向]
 *
 *   Service => StackOut
 */_event( ev_SiteCallCreateCmd    )
   _body(TSitecallInformation , 1)
   _ev_end
/***********************<< 创建主叫呼集回应>>************************	
 *[消息体]			 
 *    + BOOL
 *
 *[消息方向]
 *
 *   Service <= StackOut
 */_event( ev_SiteCallCreateInd    )
   _body(u32 , 1)
   _ev_end



_event(  ev_SiteCallResultInd  )
   _body( u32        ,1 )
_ev_end
   
/***********************<< 收到一个主叫呼集>>************************	
 *[消息体]			 
 *    + NULL
 *
 *[消息方向]
 *
 *   Service <= StackIn
 */_event( ev_SiteCallIncoming    )
   _body(BOOL , 1)
   _ev_end

/***********************<< 关闭主叫呼集命令>>************************	
 *[消息体]			 
 *    +NULL
 *
 *[消息方向]
 *
 *   Service => StackOut
 */_event( ev_SiteCallDropCmd    )
   _ev_end
/***********************<< 关闭主叫呼集回应>>************************	
 *[消息体]			 
 *    + NULL
 *
 *[消息方向]
 *
 *   Service <= StackOut
 */_event( ev_SiteCallDropInd    )
   _ev_end
/***********************<< 卸载主叫呼集命令>>************************	
 *[消息体]			 
 *    + NULL
 *
 *[消息方向]
 *
 *   Service => StackOut
 */_event( ev_SiteCallUninstallCmd    )
   _ev_end
/***********************<< 卸载主叫呼集回应>>************************	
 *[消息体]			 
 *    + NULL
 *
 *[消息方向]
 *
 *   Service <= StackOut
 */_event( ev_SiteCallUninstallDropInd    )
   _ev_end
   
/***********************<< 向DNS查询域名IP地址  >>************************	
 *[消息体]			 
 *  + u32 DNS Server IP
 *  + u32 dwUserId 
 *  + u16 域名长度 
 *  + s8  域名 
 *[消息方向]
 *
 *    MtH323Service => StatckIn
 */_event( ev_h323DomainNameReq  )
   _body(  u32_ip    ,1             )
   _body(  u32       ,1             )
   _body(  u16       ,1             )
   _body(  s8        ,REMAIN             )
   _ev_end

    /***********************<< DNS查询域名IP地址响应  >>************************	
 *[消息体]			 
 *  + u8
 *
 *[消息方向]
 *
 *    MtH323Service => StatckIn
 */_event( ev_h323DomainNameRsp  )
   _body(  BOOL          , 1             )
   _body(  u32_ip        , 1             )
   _ev_end
#ifndef _MakeMtEventDescription_
   _ev_segment_end(sitecall)
};
  

inline BOOL IsSitecallEvent(u16 wEvent)
{
	return (wEvent > get_first_event_val(sitecall)) && (wEvent < get_last_event_val(sitecall));
}  
#endif //_MakeMtEventDescription_
#endif //SITECALL_EVENT_H


