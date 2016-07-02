
#ifndef LDAP_FCNET_EVENT_H
#define LDAP_FCNET_EVENT_H

#include "innereventcomm.h"

#ifndef _MakeMtEventDescription_

enum EmLdapFcnet
{
#endif
/***********************************************************/
/*                                                         */
/*                Ldap和流控的消息                            */
/*                                                         */
/***********************************************************/

    _ev_segment( ldapfcnet )
 
//////////////////////////////////////////////////////////////////////////
//流控相关的消息

/***********************<< 流控模块启用指示 >>********************	
 *[消息体]			 
 * +BOOL          //是否启用了流控模块
 *   
 *[消息方向]
 *   ui  => mtservice 
 */_event(  ev_FcNetInitialInd   )
   _body( BOOL, 1 )
   _ev_end

/***********************<< 检测可用呼叫码率命令 >>********************	
 *[消息体]			 
 * +u32_ip          //目标的ip地址
 * +u32             //检测时使用的初始带宽  
 *
 *[消息方向]
 *   ui  => mtservice 
 */_event(  ev_FcNetDetectBandWidthCmd   )
   _body( u32_ip, 1 )
   _body( u32, 1 )
   _ev_end


/***********************<< 可用呼叫码率检测通知 >>********************	
 *[消息体]			 
 * +u32_ip   //目标地址ip
 * +u32      //当前可用的带宽
 *   
 *[消息方向]
 *   fcnet  => mtservice 
 */_event(  ev_FcNetDetectBandWidthInd   )
   _body( u32_ip, 1 )
   _body(u32 , 1 )
   _ev_end
   
/***********************<< 可用呼叫码率检测通知 >>********************	
 *[消息体]			 
 * +u32_ip   //目标地址ip
 * +u32      //当前可用的带宽
 *   
 *[消息方向]
 *   mtservice  => mtservice
 */_event(  ev_FcNetMTDetectBandWidthInd   )
   _body( u32_ip, 1 )
   _body(u32 , 1 )
   _ev_end


/***********************<<  注册到目标ip的流量检测 >>********************	
 *[消息体]			 
 * +u32_ip   //目标地址ip
 * +u32      //流量的检测时间
 *   
 *[消息方向]
 *   ui  => mtservice 
 */_event(  ev_FcNetRegisterDetectCmd   )
   _body( u32_ip, 1 )
   _body(u32 , 1 )
   _ev_end

/***********************<<  注销到目标ip的流量检测 >>********************	
 *[消息体]			 
 * +u32_ip   //目标地址ip
 * 
 *   
 *[消息方向]
 *   ui  => mtservice 
 */_event(  ev_FcNetUnRegisterDetectCmd   )
   _body( u32_ip, 1 )
   _ev_end


/***********************<< 当前网络带宽通知 >>********************	
 *[消息体]			 
 * +u32_ip   //目标地址ip
 * +u32      //当前可用的带宽
 *   
 *[消息方向]
 *   ui  => mtservice 
 */_event(  ev_FcNetCurBandWidthInd   )
   _body( u32_ip, 1 )
   _body(u32 , 1 )
   _ev_end
   

//////////////////////////////////////////////////////////////////////////
/*
/*              ldap相关的消息
/* */
//////////////////////////////////////////////////////////////////////////

/***********************<< LDAP连接成功指示 >>********************	
 *[消息体]			 
 * +无
 *   
 *[消息方向]
 *   mtservice  => mtservice 
 */_event(   ev_LdapConnectedInd  )
   _ev_end

/***********************<< LDAP断链指示 >>********************	
 *[消息体]			 
 * +无
 *   
 *[消息方向]
 *   mtservice  => mtservice 
 */_event(   ev_LdapDisconnectedInd  )
   _ev_end
   
/***********************<< LDAP多次重连无效指示 >>********************	
 *[消息体]			 
 * +无
 *   
 *[消息方向]
 *   mtservice  => mtservice 
 */_event(   ev_LdapMaxRetryFailedInd  )
   _ev_end

/***********************<< LDAP认证响应 >>********************	
 *[消息体]			 
 * +BOOL  //认证是否通过
 *   
 *[消息方向]
 *   mtservice  => mtservice 
 */_event(   ev_LdapAuthRsp  )
   _body( BOOL, 1 ) 
   _ev_end

/***********************<< LDAP注册响应 >>********************	
 *[消息体]			 
 * +BOOL  //注册是否成功
 *   
 *[消息方向]
 *   mtservice  => mtservice 
 */_event(   ev_LdapRegisterRsp  )
   _body( BOOL, 1 ) 
   _ev_end

/***********************<< LDAP注销响应 >>********************	
 *[消息体]			 
 * +BOOL  //注销是否成功
 *   
 *[消息方向]
 *   mtservice  => mtservice 
 */_event(   ev_LdapUnRegisterRsp  )
   _body( BOOL, 1 ) 
   _ev_end
   
/***********************<<  LDAP查询号码条目信息响应 >>********************	
 *[消息体]			 
 * + BOOL             //查询是否成功
 * +                  //如果成功时，返回查询的终端的信息  
 *[消息方向]
 *   mtservice  => mtservice 
 */_event(   ev_LdapSearchCallnumInfoRsp  )
   _body( u32 , 1 )
   _ev_end

/***********************<<  查询号码对应的条目信息命令 >>********************	
 *[消息体]			 
 * +u8[]             //对端的号码
 *  
 *[消息方向]
 *   mtservice  => mtservice 
 */_event(   ev_LdapSearchCallnumInfoCmd ) 
   _body( u8 , REMAIN  )
   _ev_end

/***********************<<  查询号码对应的条目信息指示 >>********************	
 *[消息体]
 * +BOOL               //号码查询是否成功
 * +u32_ip             //对端的号码, 所对应的ip地址
 *  
 *[消息方向]
 *   mtservice  => mtservice 
 */_event(   ev_LdapSearchCallnumInfoInd )
   _body( BOOL,  1 )
   _body( u32_ip , 1 )
   _ev_end

//////////////////////////////////////////////////////////////////////////
/*
/*              平滑发送相关的消息
/* */
//////////////////////////////////////////////////////////////////////////
/***********************<< 设置平滑发送规则 >>********************	
 *[消息体]
 * +u32_ip             //平滑发送的目的地址
 * +u16 , port
 *  u32 dwRate         //平均发送码率
 *  u32 dwPeak         //峰值的速率
 *[消息方向]
 *   mtservice  => mtservice 
 */_event(   ev_SmoothSendSetRule )
   _body( u32_ip , 1 )
   _body( u16, 1 )
   _body( u32,  1 )
   _body( u32,  1 )
   _ev_end

/***********************<< 取消平滑发送规则 >>********************	
 *[消息体]
 * +u32_ip             //目的地址
 * +u16, port
 *[消息方向]
 *   mtservice  => mtservice 
 */_event(   ev_SmoothSendUnSetRule )
   _body( u32_ip , 1 )
   _body( u16, 1 ) 
   _ev_end

/***********************<< 关闭平滑发送规则 >>********************	
 *[消息体]
 *[消息方向]
 *   mtservice  => mtservice 
 */_event(   ev_SmoothSendClose )
   _ev_end


#ifndef _MakeMtEventDescription_
   _ev_segment_end( ldapfcnet )
};
#endif

#endif