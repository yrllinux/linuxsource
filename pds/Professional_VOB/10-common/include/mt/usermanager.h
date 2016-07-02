
#ifndef USERMANAGER_EVENT_H
#define USERMANAGER_EVENT_H

#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmUserManager
{
#endif
/***********************************************************/
/*                                                         */
/*                 用户管理消息                            */
/*    osp 消息                                             */
/***********************************************************/
    _ev_segment( user_manager )
#if 0
/***********************<< 客户端请求登录消息 >>********************	
 *[消息体]			 
 *   + u8[32]         //通信数据结构版本描述
 *   + CLoginRequest
 *[消息方向]
 *    MTC => UE
 */_event(  ev_UMLoginReq   )
   _ev_end



/***********************<< 客户端请求登录确认应答 >>********************	
 *[消息体]			 
 *   无消息体
 *[消息方向]
 *   UE <= MTC
 */_event(  ev_UMLoginAck   )
   _ev_end



 /***********************<< 客户端请求登录拒绝应答 >>********************	
 *[消息体]
 *   + CUserFullInfo  //已登陆用户信息
 *   + u8[32]         //通信数据结构版本描述
 *   + u8             // 登陆失败原因   0=用户管理模块 1=已有用户登陆 2＝软件版本布匹配
 *   + u16            // 用户管理模块错误号
 *   + u32            //已登陆用户IP
 *[消息方向]
 *   UE => MTC
 */_event(  ev_UMLoginNack   )
   _ev_end
#endif
/******************<< Slave IP冲突定时器4  >>**********************	
 *[消息体]			 
 *  无
 *[消息方向]
 *   MtService => MtService
 */_event( ev_T2SlaveIPConflictTimer4 )
   _ev_end

/******************<< Slave IP冲突定时器5  >>**********************	
 *[消息体]			 
 *  无
 *[消息方向]
 *   MtService => MtService
 */_event( ev_T2SlaveIPConflictTimer5 )
   _ev_end

/******************<< Slave IP冲突定时器6  >>**********************	
 *[消息体]			 
 *  无
 *[消息方向]
 *   MtService => MtService
 */_event( ev_T2SlaveIPConflictTimer6 )
   _ev_end

/************************<< 获取所有用户列表请求  >>********************
 *[消息体]
 * +CUserFullInfo[] 用户信息列表
 *[消息方向]
 *  UE <= MTC
 */_event(  ev_UMGetAllUserListReq  )
   _ev_end


 
 /**********************<< 获取所有用户信息列表确认应答  >>*************
  *[消息体]
  * +CUserFullInfo[] 用户信息列表
  *[消息方向]
  * UE => MTC
  */_event(  ev_UMGetAllUserListACK  )
   _ev_end


 
 /***********************<< 获取用所有户信息列表拒绝应答 >>************
  *[消息体]
 *   + u8[32]         //通信数据结构版本描述
 *   + u8             // 登陆失败原因   0=用户管理模块 1=已有用户登陆 2＝软件版本布匹配
 *   + u16            // 用户管理模块错误号
 *   + CUserFullInfo  //已登陆用户信息
 *   + u32            //已登陆用户IP
 */_event(  ev_UMGetAllUserListNACK  )
   _ev_end


 
  
/***********************<< 客户端添加用户请求 >>********************	
 *[消息体]			 
 *   +CUserFullInfo
 *[消息方向]
 *   UE => MTC
 */_event(  ev_UMAddUserReq  )
   _ev_end



/***********************<< 客户端添加用户确认求 >>********************	
 *[消息体]			 
 *  无消息体
 *[消息方向]
 *   UE => MTC
 */_event(  ev_UMAddUserAck  )
   _ev_end



/***********************<< 客户端添加用户拒绝 >>********************	
 *[消息体]			 
 *  + u16 错误号
 *[消息方向]
 *   UE => MTC
 */_event(  ev_UMAddUserNack  )
   _ev_end



/***********************<< 客户端删除用户请求 >>********************	
 *[消息体]			 
 *   +CUserFullInfo
 *[消息方向]
 *   UE => MTC
 */_event(  ev_UMRemoveUserReq  )
   _ev_end



/***********************<< 客户端删除用户确认 >>********************	
 *[消息体]			 
 *  无消息体
 *[消息方向]
 *   UE => MTC
 */_event(  ev_UMRemoveUserAck  )
   _ev_end


/**********************<< 客户端删除用户拒绝 >>*********************
 *[消息体]			 
 *  + u16 错误号
 *[消息方向]
 *   UE => MTC
 */_event(  ev_UMRemoveUserNack  )
   _ev_end


 
/**********************<< 客户端更改用户密码请求>>*******************
 *[消息体]			 
 *   +CUserFullInfo
 *[消息方向]
 *   UE => MTC
 */_event(  ev_UMChangePWDReq  )
   _ev_end



/***********************<< 客户端更改用户密码确认 >>********************	
 *[消息体]			 
 *  无消息体
 *[消息方向]
 *   UE => MTC
 */_event(  ev_UMChangePWDACK  )
   _ev_end


/**********************<< 客户端更改用户密码拒绝 >>*********************
 *[消息体]			 
 *  + u16 错误号
 *[消息方向]
 *   UE => MTC
 */_event(  ev_UMChangePWDNACK  )
   _ev_end


/**********************<< 客户端更改用户密码拒绝 >>*********************
 *[消息体]			 
 *  +CUserFullInfo[16] 用户信息列表
 *[消息方向]
 *   UE => MTC
 */_event(  ev_UMNotifyMTC  )
   _ev_end

/**********************<< 系统启动未完成mtc挂断消息 >>*********************
 *[消息体]			 
 *  +CUserFullInfo[16] 用户信息列表
 *[消息方向]
 *   UE => MTC
 */_event(  ev_UMMtSysHasNotInited  )
   _ev_end

#ifndef _MakeMtEventDescription_
   _ev_segment_end(user_manager)
};
#endif

#endif



