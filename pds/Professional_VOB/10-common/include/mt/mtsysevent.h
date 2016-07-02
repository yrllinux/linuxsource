#ifndef  MTSYS_EVENT_H
#define  MTSYS_EVENT_H
#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmMtSys
{
#endif

	_ev_segment( mtsys )

/***********************<< 系统初始化消息 >>**********************	
 *[消息体]			 
 *  + u32 参数1
 *  + u32 参数2
 *[消息方向]
 *
 */_event( ev_SysInitCmd )
   _body ( u32    , 1    )
   _body ( u32    , 1    )
   _ev_end

/***********************<< 应用对象初始化完成指示 >>**********************	
 *[消息体]			 
 *  + u8    appid
 *  + u8    instid
 *  + BOOL  TRUE =初始化成功 FALSE=初始化失败
 *[消息方向]
 *
 */_event(  ev_SysInitInd )
   _ev_end


 /***********************<< 应用退出消息 >>********************	
 *[消息体]			 
 * 无消息体
 *[消息方向]
 *
 */_event(  ev_SysExitCmd  )
   _ev_end


/***********************<< 应用退出消息指示 >>********************	
 *[消息体]			 
 * 无消息体
 *[消息方向]
 *
 */_event(  ev_SysExitInd  )
   _ev_end


/***********************<< 系统应用模块开始初始化 >> **************	
 *[消息体]			 
 * 无消息体
 *[消息方向]
 *
 */_event(  ev_SysModuleStartCmd )
   _ev_end


/***********************<< 系统各应用模初始化完毕指示 >> **************	
 *[消息体]			 
 * 无消息体
 *[消息方向]
 *
 */_event(  ev_SysAppInitCompleteInd )
   _ev_end

/***********************<< 终端重启命令 >>********************	
 *[消息体]			 
 * 无消息体
 *[消息方向]
 *
 */_event(  ev_SysRebootCmd  )
   _ev_end
      
/***********************<< PC双流软件登陆指示 >>********************	
 *[消息体]			 
 *   NULL
 *[消息方向]
 *
 */_event(  ev_SysPCLoginInd  )
   _ev_end

/***********************<< PC双流软件退出指示 >>********************	
 *[消息体]			 
 *   NULL
 *[消息方向]
 *
 */_event(  ev_SysPCLogoutInd )
   _ev_end

/***********************<< 终端控制台登陆指示 >>********************	
 *[消息体]			 
 *   NULL
 *[消息方向]
 *
 */_event(  ev_SysMTCLoginInd  )
   _ev_end

/***********************<< 终端控制台登陆指示 >>********************	
 *[消息体]			 
 *   NULL
 *[消息方向]
 *
 */_event(  ev_SysMTCLogoutInd )
   _ev_end

/******************<< 终端控制台键盘内容发送消息  >>**********************	
 *[消息体]			 
 *  BOOL   按键状态 0 key down 1 key up
 *  u8    按键类型 OLD_REMOTE NEW_REMOTE UNKNOW_REMOTE 
 *  u8    键值
 *  u16   按键次数
 *[消息方向]
 *   UTC  => MTUE
 *   MTUE => UI
 */_event( ev_SysMTCKeyboardContent  )
   _body(  BOOL   ,   1        )
   _body(  u8     ,   1        )
   _body(  u8     ,   1        )
   _body(  u16    ,   1        )
   _ev_end

 /***********************<< 通知界面命令>>************************	
 *[消息体]			 
 *    + u8     node_ui/node_mtc
 *
 *[消息方向]
 *   内部
 */_event(  ev_NotifyObserverCmd  )
   _body ( u8    ,1               )
   _ev_end


/***********************<< 启动服务模块>>************************	
 *[消息体]			 
 *    + u8     node_ui/node_mtc
 *
 *[消息方向]
 *   内部
 */_event(  ev_ModuleStartupCmd  )
   _body ( u8    ,1               )
   _ev_end


/***********************<< 短消息翻页定时器消息>>************************	
 *[消息体]			 
 *     
 *
 *[消息方向]
 *   内部
 */_event(  ev_SMSPageTieleInd   )
   _ev_end

/***********************<< 静态文本定时器消息>>************************	
 *[消息体]			 
 *     
 *
 *[消息方向]
 *   内部
 */_event(  ev_SMSStaticTitleInd   )
   _ev_end
   
/***********************<< 申请待机请求     >>**********************	
 *[消息体]			 
 *  无  
 *[消息方向]
 *    MtService = > embed UI
 */_event( ev_ApplySleepCmd  )
   _ev_end

/***********************<< 终端待机命令  >>**********************	
 *[消息体]			 
 *  无
 *[消息方向]
 *   UI = > MtService
 */_event( ev_MtSleepCmd  )
   _ev_end

/***********************<< 终端待机唤醒命令  >>**********************	
 *[消息体]			 
 *  无
 *[消息方向]
 *   UI = > MtService
 */_event( ev_MtWakeupCmd  )
   _ev_end
/***********************<< 待机状态指示  >>**********************	
 *[消息体]			 
 *  BOOL     TRUE =待机 
 *[消息方向]
 *    MtService = > embed UI
 */_event( ev_SleepStateInd  )
   _body(  BOOL   , 1        )
   _ev_end

/******************<< UE一分钟内收到操作操作消息数指示  >>**********************	
 *[消息体]			 
 *  u32   收到消息数
 *[消息方向]
 *   UE => MtService 
 */_event( ev_UserOperationMsgCountInd  )
   _body(  u32   , 1        )
   _ev_end



/******************<< IP冲突指示/用于非T2  >>**********************	
 *[消息体]			 
 *  BOOL   TRUE=冲突
 *[消息方向]
 *   MtService => MTC/MTUI
 */_event( ev_IPConflictInd  )
   _body(  BOOL   , 1        )
   _ev_end

/******************<< IP冲突定时器  >>**********************	
 *[消息体]			 
 *  无
 *[消息方向]
 *   MtService => MTC/MTUI
 */_event( ev_IPConflictTimer )
   _ev_end

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/******************<< IP冲突上报指示/用于T2  >>**********************	
 *[消息体]			 
 *  BOOL   Master是否冲突
 *  u32    Master冲突IP
 *  BOOL   Slave是否冲突
 *  u32    Slave冲突IP
 *[消息方向]
 *   MtService => MTC/MTUI
 */_event( ev_T2IPConflictInd  )
   _body (  BOOL   , 1        )
   _body (  u32    , 1        )
   _body (  BOOL   , 1        )
   _body (  u32    , 1        )
   _ev_end

/******************<< IP冲突指示/用于T2主davinci >>**********************	
 *[消息体]			 
 *  s8   MAC地址
 *  u32  冲突IP
 *[消息方向]
 *   MtService => MtService
 */_event( ev_T2IPConflictMasterInd  )
   _body(  s8     , 6        )
   _body(  u32    , 1        )
   _ev_end

/******************<< IP冲突指示/用于T2从davinci >>**********************	
 *[消息体]			 
 *  s8   MAC地址
 *  u32  冲突IP突
 *[消息方向]
 *   MtService => MtService
 */_event( ev_T2IPConflictSlaveInd  )
   _body(  s8     , 6        )
   _body(  u32    , 1        )
   _ev_end 

/******************<< Master IP冲突定时器  >>**********************	
 *[消息体]			 
 *  无
 *[消息方向]
 *   MtService => MtService
 */_event( ev_T2MasterIPConflictTimer )
   _ev_end

/******************<< Slave IP冲突定时器1  >>**********************	
 *[消息体]			 
 *  无
 *[消息方向]
 *   MtService => MtService
 */_event( ev_T2SlaveIPConflictTimer1 )
   _ev_end

/******************<< Slave IP冲突定时器2  >>**********************	
 *[消息体]			 
 *  无
 *[消息方向]
 *   MtService => MtService
 */_event( ev_T2SlaveIPConflictTimer2 )
   _ev_end

/******************<< Slave IP冲突定时器3  >>**********************	
 *[消息体]			 
 *  无
 *[消息方向]
 *   MtService => MtService
 */_event( ev_T2SlaveIPConflictTimer3 )
   _ev_end
#if 0
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
#endif
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

/******************<< Slave IP冲突定时器7  >>**********************	
 *[消息体]			 
 *  无
 *[消息方向]
 *   MtService => MtService
 */_event( ev_T2SlaveIPConflictTimer7 )
   _ev_end  

/******************<< T2 IP冲突循环检查定时器  >>**********************	
 *[消息体]			 
 *  无
 *[消息方向]
 *   MtService => MtService
 */_event( ev_T2CheckIPConflictTimer )
   _ev_end
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////



/******************<< 以太网口断链检测定时器  >>**********************	
 *[消息体]			 
 *  无
 *[消息方向]
 *   MtService => MtService
 */_event( ev_EthnetStatusTimer )
   _ev_end

/*********************** << CPU idle 和内存查询接口 >>************************
 *[消息体]    
 *            无
 *[消息方向]
 *
 *                UI    =   >     MtService
 */_event(ev_GetSystemStateCmd)
   _ev_end
 /***********************<<CPU idle 和内存查询接口>> ************************
 *[消息体]    
 *            +u8             //当前cpu的空闲百分比
 *            +u32             //系统总的sdram,单位  字节为单位
 *            +u32            //当前空闲的内存大小，字节为单位
 *            +u32            //当前已分配的内存大小，字节为单位
 *[消息方向]
 *
 *                MtService    =   >     UI
 */_event(ev_GetSystemStateInd)
     _body    (u8                          ,  1 )
     _body    (u32                         ,  1 )
     _body    (u32                         ,  1 )
     _body    (u32                         ,  1 )
   _ev_end

/***********************<<终端信息>> ************************
 *[消息体]    
 *            +u8             //硬件版本号
 *            +EmMtOSType     //操作系统类型
 *            +s8             //操作系统版本
 *            +s8             //软件版本号
 *            +s8             //MAC地址
 
 *[消息方向]
 *
 *            MtService    =   >     UI
 */_event(ev_TerminalInformationInd)
     _body    (u8                         ,   1  )
	 _body    (EmMtOSType                 ,   1  )
     _body    (s8                         ,   256 )
     _body    (s8                         ,   256 )
     _body    (s8                         ,   256 )
   _ev_end
   
/**********************<< mt设备能力指示  >>********************	
 *[消息体]
 * +TMtEquipmentCapset  //mt设备能力集
 *[消息方向]
 *    mtservice  = > ui/mtc
 */_event( ev_MtEquipmentCapsetInd )
   _body( TMtEquipmentCapset, 1 )
   _ev_end

/**********************<< 组合键上报  >>********************	
 *[消息体]
 * +TMtEquipmentCapset  //mt设备能力集
 *[消息方向]
 *    mtservice  = > ui/mtc
 */_event( ev_MtCombinationKeyInd )
   _body( u8, 1 )
   _ev_end

/**********************<< 上报动作操作结果 >>********************	
 *[消息体]
  *[消息方向]
 *      = > mtservice
 */_event( ev_HintNotify )
   _body( u16, 1 )
   _ev_end

/***********************<< MTB登录指示 >>********************	
 *[消息体]			 
 *   NULL
 *[消息方向]
 *
 */_event(  ev_SysMTBLoginInd )
   _ev_end


/***********************<< MTB退出指示 >>********************	
 *[消息体]			 
 *   NULL
 *[消息方向]
 *
 */_event(  ev_SysMTBLogoutInd )
   _ev_end

/***********************<< 设置系统时间 >>********************	
 *[消息体]			 
 *   NULL
 *[消息方向]
 *
 */_event(  ev_SysSetTimeCmd )
   _body( TMtKdvTime, 1 )
   _ev_end

/***********************<< 查询系统时间 >>********************	
 *[消息体]			 
 *   NULL
 *[消息方向]
 *
 */_event(  ev_SysTimeCmd )
   _ev_end

/***********************<< 系统时间指示 >>********************	
 *[消息体]			 
 *   NULL
 *[消息方向]
 *
 */_event(  ev_SysTimeInd )
   _body( TMtKdvTime, 1 )
   _ev_end

/***********************<< 添加地址簿条目 >>********************	
 *[消息体]			 
 *   TADDRENTRYINFO
 *[消息方向]
 *
 */_event(  ev_SysAddCallOutEntryCmd )
   _body( TAddrEntry, 1 )
   _ev_end

/***********************<< telnet password indication >>********************	
 *[消息体]			 
 *   password 
 *[消息方向]
 *mt-->mtc
 */
 _event(  ev_SysTelnetPwdInd)
   _body(s8         ,   MT_TELNET_PWD_MAX_LEN )
   _ev_end

/***********************<< change telnet password  req>>********************	
 *[消息体]			 
 *   password
 *[消息方向]
 *mtc-->mt
 */
 _event(  ev_SysChangeTelnetPwdCmd)
   _body(s8         ,   MT_TELNET_PWD_MAX_LEN )
 _ev_end   


#ifndef _MakeMtEventDescription_
   _ev_segment_end(mtsys)
};
#endif
#endif

