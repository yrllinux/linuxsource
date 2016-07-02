#ifndef CONFIG_EVENT_H
#define CONFIG_EVENT_H
#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmConfig
{
#endif
     _ev_segment( config)//配置消息
		
/***********************<< 引导指示>>************************	
 *[消息体]			 
 *    + TGuideCfg
 *[消息方向]
 *   MtService => UI
 */_event(  ev_CfgGuidNeedInd  )
   _body( TGuideCfg       ,1  ) 
   _ev_end



/***********************<< 引导指示>>************************	
 *[消息体]			 
 *    + TGuideCfg
 *[消息方向]
 *   MtService => UI
 */_event(  ev_CfgGuidInd  )
   _body( TGuideCfg       ,1  ) 
   _ev_end



/***********************<< 引导配置>>************************	
 *[消息体]			 
 *    + TGuideCfg
 *[消息方向]
 *    UI< = MtService
 */_event(  ev_CfgGuidCmd  )
   _body( TGuideCfg       ,1  ) 
   _ev_end



/***********************<< 语言指示 >>************************	
 *[消息体]			 
 *    + EmLanguage
 *[消息方向]
 *    MtService => UI
 */_event(  ev_CfgLanguageInd  )
   _body( EmLanguage       ,1  ) 
   _ev_end



/***********************<< 语言配置 >>************************	
 *[消息体]			 
 *    + EmLanguage
 *[消息方向]
 *    UI< = MtService
 */_event(  ev_CfgLanguageCmd  )
   _body( EmLanguage       ,1  ) 
   _ev_end



 /**********************<< 终端重置 >>**************	
 *[消息体]			 
 *  
 *  
 *  
 *[消息方向]
 *
 *   
 *   MtService <= UI
 */_event(  ev_CfgResetCmd   )
   _ev_end



/***********************<< 显示配置指示 >>************************	
 *[消息体]			 
 *    + TDisplay (TOsdCfg)
 *[消息方向]
 *    MtService => UI
 */_event(  ev_CfgDisplayInd  )
   _body( TOsdCfg    ,1  ) 
   _ev_end



/***********************<< 显示配置 >>************************	
 *[消息体]			 
 *    + TDisplay (TOsdCfg)
 *[消息方向]
 *    UI< = MtService
 */_event(  ev_CfgDisplayCmd  )
   _body( TOsdCfg    ,1  ) 
   _ev_end


/***********************<< VGA显示配置指示 >>************************	
 *[消息体]			 
 *    + BOOL
 *[消息方向]
 *    MtService => UI
 */_event(  ev_CfgVGAShowLocalInd  )
   _body( BOOL    ,1  ) 
   _ev_end



/***********************<< VGA显示配置 >>************************	
 *[消息体]			 
 *    + BOOL
 *[消息方向]
 *    UI< = MtService
 */_event(  ev_CfgVGAShowLocalCmd  )
   _body( BOOL    ,1  ) 
   _ev_end

/***********************<< 第二路视频输出到VGA指示 >>************************	
 *[消息体]			 
 *    + BOOL
 *[消息方向]
 *    MtService => UI
 */_event(  ev_CfgSecondVideoToVGAInd  )
   _body( BOOL    ,1  ) 
   _ev_end



/***********************<< 第二路视频输出到VGA配置 >>************************	
 *[消息体]			 
 *    + BOOL
 *[消息方向]
 *    UI< = MtService
 */_event(  ev_CfgSecondVideoToVGACmd  )
   _body( BOOL    ,1  ) 
   _ev_end
   
/***********************<< 用户设置配置指示 >>************************	
 *[消息体]			 
 *    + TUserCfg
 *[消息方向]
 *    MtService => UI
 */_event(  ev_CfgUserInd  )
   _body( TUserCfg    ,1  )
   _ev_end


/***********************<< 用户设置配置 >>************************	
 *[消息体]			 
 *    + TUserCfg
 *[消息方向]
 *    UI< = MtService
 */_event(  ev_CfgUserCmd  )
   _body( TUserCfg    ,1  )
   _ev_end



/***********************<< 以太网指示 >>************************	
 *[消息体]			 
 *    + u8 以太网索引 (0-eth0   )
 *    + TEthnetInfo
 *[消息方向]
 *    MtService => UI
 */_event(  ev_CfgEthnetInd  )
   _body( u8          ,1  )
   _body( TEthnetInfo ,1  )
   _ev_end


/***********************<< 以太网配置 >>************************	
 *[消息体]			 
 *    + u8 以太网索引 (0-eth0   )
 *    + TEthnetInfo
 *[消息方向]
 *    UI< = MtService
 */_event(  ev_CfgEthnetCmd  )
   _body( u8          ,1  )
   _body( TEthnetInfo ,1  )
   _ev_end



/***********************<< PPPOE指示 >>************************	
 *[消息体]			 
 *    + TPPPOECfg
 *[消息方向]
 *    MtService => UI
 */_event(  ev_CfgPPPOEInd  )
   _body( TPPPOECfg ,1  )
   _ev_end


/***********************<< PPPOE配置 >>************************	
 *[消息体]			 
 *    + TPPPOECfg
 *[消息方向]
 *    UI< = MtService
 */_event(  ev_CfgPPPOECmd  )
   _body( TPPPOECfg ,1  )
   _ev_end


/***********************<< E1指示 >>************************	
 *[消息体]			 
 *    + TE1Config
 *[消息方向]
 *    MtService => UI
 */_event(  ev_CfgE1Ind  )
   _body( TE1Config ,1  )
   _ev_end


/***********************<< E1配置 >>************************	
 *[消息体]			 
 *    + TE1Config
 *[消息方向]
 *    UI< = MtService
 */_event(  ev_CfgE1Cmd  )
   _body( TE1Config ,1  )
   _ev_end


/***********************<< 路由指示 >>************************	
 *[消息体]			 
 *    + TRouteCfg[MT_MAX_ROUTE_NUM] 
 *[消息方向]
 *    MtService => UI
 */_event(  ev_CfgRouteInd  )
   _body( TRouteCfg ,MT_MAX_ROUTE_NUM  )
   _ev_end



/***********************<< 增加路由 >>************************	
 *[消息体]			 
 *    + TRouteCfg
 *[消息方向]
 *    UI< = MtService
 */_event(  ev_CfgAddRouteCmd  )
   _body( TRouteCfg ,1  )  
   _ev_end



/***********************<< 删除路由 >>************************	
 *[消息体]			 
 *    + TRouteCfg
 *[消息方向]
 *    UI< = MtService
 */_event(  ev_CfgRemoveRouteCmd  )
   _body( TRouteCfg ,1  )
   _ev_end


/***********************<< 防火墙指示 >>************************	
 *[消息体]			 
 *    + TEmbedFwNatProxy
 *[消息方向]
 *    MtService => UI
 */_event(  ev_CfgFireWallInd  )
   _body( TEmbedFwNatProxy ,1  )
   _ev_end


/***********************<< 防火墙配置 >>************************	
 *[消息体]			 
 *    + TEmbedFwNatProxy
 *[消息方向]
 *    UI< = MtService
 */_event(  ev_CfgFireWallCmd  )
   _body( TEmbedFwNatProxy ,1  )
   _ev_end


/***********************<< 码流重传指示 >>************************	
 *[消息体]			 
 *    + TLostPackResend
 *[消息方向]
 *    MtService => UI
 */_event(  ev_CfgStreamRetransInd  )
   _body( TLostPackResend ,1 )
   _ev_end


/***********************<< 码流重传配置 >>************************	
 *[消息体]			 
 *    + TLostPackResend
 *[消息方向]
 *    UI< = MtService
 */_event(  ev_CfgStreamRetransCmd  )
   _body( TLostPackResend ,1 )
   _ev_end



/***********************<< SNMP指示 >>************************	
 *[消息体]			 
 *    + TSNMPCfg
 *[消息方向]
 *    MtService => UI
 */_event(  ev_CfgSNMPInd  )
   _body( TSNMPCfg ,1 )
   _ev_end


/***********************<< SNMP配置 >>************************	
 *[消息体]			 
 *    + TSNMPCfg
 *[消息方向]
 *    UI< = MtService
 */_event(  ev_CfgSNMPCmd  )
   _body( TSNMPCfg ,1 )
   _ev_end


/***********************<< SNTP指示 >>************************	
 *[消息体]			 
 *    + TSNTPCfg
 *[消息方向]
 *    MtService => UI
 */_event(  ev_CfgSNTPInd  )
   _body( u32, 1 )
   _ev_end

/***********************<< SNTP配置 >>************************	
 *[消息体]			 
 *    + TSNTPCfg
 *[消息方向]
 *    UI< = MtService
 */_event(  ev_CfgSNTPCmd  )
   _body( u32, 1 )
   _ev_end

/***********************<< Qos指示 >>************************	
 *[消息体]			 
 *    + TIPQos
 *[消息方向]
 *    MtService => UI
 */_event(  ev_CfgQosInd  )
   _body( TIPQoS ,1 )
   _ev_end


/***********************<< Qos配置 >>************************	
 *[消息体]			 
 *    + TIPQos
 *[消息方向]
 *    UI< = MtService
 */_event(  ev_CfgQosCmd  )
   _body( TIPQoS ,1 )
   _ev_end





/***********************<< H323指示 >>************************	
 *[消息体]			 
 *    + TH323Cfg
 *[消息方向]
 *    MtService => UI
 */_event(  ev_CfgH323SettingInd  )
   _body( TH323Cfg ,1 )
   _ev_end


/***********************<< H323配置 >>************************	
 *[消息体]			 
 *    + TH323Cfg
 *[消息方向]
 *    UI< = MtService
 */_event(  ev_CfgH323SettingCmd  )
   _body( TH323Cfg ,1 )
   _ev_end



/***********************<< 音视频优选协议指示 >>************************	
 *[消息体]			 
 *    + TAVPriorStrategy
 *[消息方向]
 *    MtService => UI
 */_event(  ev_CfgAVPriorStrategyInd  )
   _body( TAVPriorStrategy ,1 )
   _ev_end


/***********************<< 音视频优选协议配置 >>************************	
 *[消息体]			 
 *    + TAVPriorStrategy
 *[消息方向]
 *    UI< = MtService
 */_event(  ev_CfgAVPriorStrategyCmd  )
   _body( TAVPriorStrategy ,1 )
   _ev_end




/***********************<< 视频参数指示 >>************************	
 *[消息体]			 
 *    +EmVideoType
 *    +TVideoEncodeParameter
 *[消息方向]
 *    MtService => UI
 */_event(  ev_CfgVideoParamInd  )
   _body( EmVideoType ,1 )
   _body( TVideoParam ,1 )
   _ev_end


/***********************<< 视频参数配置 >>************************	
 *[消息体]			 
 *    +EmVideoType
 *    +TVideoParam
 *[消息方向]
 *    UI< = MtService
 */_event(  ev_CfgVideoParamCmd  )
   _body( EmVideoType ,1 )
   _body( TVideoParam ,1 )
   _ev_end
   

/***********************<< 用于量化参数的当前视频格式选择配置命令 >>************************	
 *[消息体]			 
 *    u32   +1
 *[消息方向]
 *    UI< = MtService
 */_event(  ev_CfgH264FormatSelCmd  )
   _body( u32          ,1 )
   _ev_end

/***********************<< 用于量化参数的当前视频格式选择配置指示 >>************************	
 *[消息体]			 
 *    u32   +1
 *[消息方向]
 *    UI< = MtService
 */_event(  ev_CfgH264FormatSelInd  )
   _body( u32          ,1 )
   _ev_end


/***********************<< VOD参数指示 >>************************	
 *[消息体]			 
 *    +TVODUserInfo
 *[消息方向]
 *    MtService => UI
 */_event(  ev_CfgVODUserParamInd  )
   _body( TVODUserInfo ,1 )
   _ev_end


/***********************<< VOD参数配置 >>************************	
 *[消息体]			 
 *    +TVODUserInfo
 *[消息方向]
 *    UI< = MtService
 */_event(  ev_CfgVODUserParamCmd  )
   _body( TVODUserInfo ,1 )
   _ev_end

/***********************<< 呼叫码率选择列表指示 >>************************	
 *[消息体]			 
 *    +u32(mask)
 *[消息方向]
 *    MtService => UI
 */_event(  ev_CfgCallBitrateListInd  )
   _body( u32        ,1               )
   _ev_end


/***********************<< 呼叫码率选择列表配置 >>************************	
 *[消息体]			 
 *    +u32(mask)
 *[消息方向]
 *    UI< = MtService
 */_event(  ev_CfgCallBitrateListCmd  )
   _body( u32        ,1               )
   _ev_end



/***********************<< 流媒体指示 >>************************	
 *[消息体]			 
 *    +TStreamMedia
 *[消息方向]
 *    MtService => UI
 */_event(  ev_CfgStreamMediaInd  )
   _body( TStreamMedia ,1         )
   _ev_end


/***********************<< 流媒体配置 >>************************	
 *[消息体]			 
 *    +TStreamMedia
 *[消息方向]
 *    UI< = MtService
 */_event(  ev_CfgStreamMediaCmd  )
   _body( TStreamMedia ,1         )
   _ev_end


 
/***********************<< 摄像头配置修改  >>************************	
 *[消息体]			 
 *    + u8  射像头索引号 (1-6)
 *    + TCameraCfg
 *
 *[消息方向]
 *   UI => MtService
 *
 *   MtService => MtDevice
 */_event(  ev_CameraConfigCmd	  )
   _body( u8           ,1         )
   _body( TCameraCfg   ,1         )
   _ev_end
 

/***********************<< 摄像机指示  >>************************	
 *[消息体]			 
 *    + u8  射像头索引号 (1-6)
 *    + TCameraCfg
 *
 *[消息方向]
 *   UI <= MtService
 */_event(  ev_CameraConfigInd		  )
   _body( u8           ,1         )
   _body( TCameraCfg   ,1         )
   _ev_end
 




/***********************<< 图像参数指示 >>************************	
 *[消息体]			 
 *    +TImageAdjustParam
 *[消息方向]
 *    MtService => UI
 */_event(  ev_ImageAdjustParamInd  )
   _body( TImageAdjustParam ,1      )
   _ev_end


/***********************<< 图像参数配置 >>**********************	
 *[消息体]	
 *    +TImageAdjustParam
 *[消息方向]
 *    UI< = MtService
 */_event(  ev_ImageAdjustParamCmd  )
   _body( TImageAdjustParam ,1      )
   _ev_end


/***********************<< 视频制式指示 >>************************	
 *[消息体]			 
 *    +TVideoStandard
 *[消息方向]
 *    MtService => UI
 */_event(  ev_CfgVideoStandardInd  )
   _body( TVideoStandard ,1         )
   _ev_end


/***********************<< 视频制式配置 >>**********************	
 *[消息体]	
 *    +TVideoStandard
 *[消息方向]
 *    UI< = MtService
 */_event(  ev_CfgVideoStandardCmd  )
   _body( TVideoStandard ,1         )
   _ev_end




/***********************<< 静态NAT映射地址指示 >>************************	
 *[消息体]			 
 *    +TNATMapAddr
 *[消息方向]
 *    MtService => UI
 */_event(  ev_CfgNATMapAddrInd  )
   _body( TNATMapAddr ,1         )
   _ev_end


/***********************<< 静态NAT映射地址配置 >>**********************	
 *[消息体]	
 *    +TNATMapAddr 
 *[消息方向]
 *    UI< = MtService
 */_event(  ev_CfgNATMapAddrCmd  )
   _body( TNATMapAddr ,1         )
   _ev_end



/***********************<< 终端通信起始端口号配置   >>**********************	
 *[消息体]	
 *    +u16  TCP 起始端口号
 *    +u16  UDP 起始端口号
 *[消息方向]
 *    UI => MtService
 */_event(  ev_CfgCommonBasePortCmd  )
   _body (  u16    ,1             ) 
   _body (  u16    ,1             ) 
   _ev_end


/***********************<< 终端通信起始端口号指示 >>**********************	
 *[消息体]	
 *    +u16  TCP 起始端口号
 *    +u16  UDP 起始端口号
 *[消息方向]
 *    UI< = MtService
 */_event(  ev_CfgCommonBasePortInd  )
   _body (  u16    ,1             ) 
   _body (  u16    ,1             ) 
   _ev_end

 
/**********************<< UI请求终端状态 >>**************	
 *[消息体]			 
 *  +TTerStatus
 *  
 *[消息方向]
 *
 *   
 *   MtService <= UI
 */_event(  ev_MtStatusReq      )
   _body( TTerStatus ,1         )
   _ev_end


/**********************<< 本地终端状态指示 >>**************	
 *[消息体]			 
 *  +TTerStatus
 *  
 *[消息方向]
 *
 *   MtService => UI
 */_event(  ev_MtStatusInd   )
   _body( TTerStatus ,1         )
   _ev_end

/***********************<< 设置默认网关 >>**********************	
 *[消息体]			 
 *  + u32_ip    默认网关
  *[消息方向]
 *  UI => MtService
 */_event(  ev_CfgDefaultGatewayCmd )
   _body (  u32_ip    ,1            )
   _ev_end

/***********************<< 默认网管配置指示 >>**********************	
 *[消息体]			 
 *  + u32_ip    默认网关
 *[消息方向]
 *  MtService => UI
 */_event(  ev_CfgDefaultGatewayInd )
   _body (  u32_ip    ,1            )
   _ev_end
   
   /***********************<< 配置DNS Server >>**********************	
 *[消息体]			 
 *  + u32_ip    DNS Server ip
 *[消息方向]
 *  MtService => UI
 */_event(  ev_CfgDNSServerCmd )
   _body (  u32_ip    ,1            )
   _ev_end

     /***********************<< DNS Server指示 >>**********************	
 *[消息体]			 
 *  + u32_ip    DNS Server ip
 *[消息方向]
 *  MtService => UI
 */_event(  ev_CfgDNSServerInd )
   _body (  u32_ip    ,1            )
   _ev_end

/***********************<< 画中画移动命令  >>************************	
 *[消息体]	
 *
 *[消息方向]
 *
 *   UI        => MtService
 *   
 */_event(  ev_MovePiPCmd   )
   _ev_end  

 /***********************<< 系统升级请求 >>********************	
  * NULL
  * MTC => CMiscService
  * CMiscService => MTUI
 */_event(ev_UpdateSystemRQS)
   _body(s8 , 256)
   _ev_end


 /***********************<< 系统升级请求 >>********************	
  * NULL
  * MTC => CMiscService
  * CMiscService => MTUI
 */_event(ev_UpdateSystemFileCmd)
   _ev_end

 /***********************<< 系统升级指示>********************	
  * BOOL  bSuccess
  * MTC => CMiscService
  * CMiscService => MTUI
 */_event(ev_UpdateSystemFileInd)
   _body(BOOL , 1)
   _ev_end
   
/***********************<< 系统升级回复    >>********************	
  * BOOL 同意升级 TRUE , 拒绝升级FALSE
  * CMiscService <= MTUI  
  * MTC <= CMiscService
 */_event(ev_UpdateSystemACK)
  _body(BOOL , 1)
   _ev_end
  
/***********************<< 系统升级回复    >>********************	
  * BOOL 上传成功 TRUE , 上传失败FALSE
  * MTC => CMiscService
  * CMiscService => MTUI  
 */_event(ev_SysfileUploadStatus)
  _body(BOOL , 1)
   _ev_end

 /***********************<< 系统恢复默认值命令 >>********************	
  * NULL
  * CService <= MTUI
 */_event(ev_ResetSystemCmd)
   _ev_end

/***********************<< 系统恢复默认值指示    >>********************	
 */_event(ev_ResetSystemInd)
   _ev_end
  
/***********************<< 系统恢复默认值状态    >>********************	
  * BOOL 成功 TRUE , 失败FALSE
  * CService => MTUI  
 */_event(ev_ResetSystemStatus)
  _body(BOOL , 1)
   _ev_end
/***********************<< 监控使能通知>>************************ 
 *[消息体]    
 *    BOOL
 *
 *[消息方向]
 *    MT <= MTService
 */_event(  ev_MonitorSwitchCapInd  )
  _body ( BOOL, 1    )
   _ev_end
 
/***********************<< 监控参数指示>>************************ 
 *[消息体]    
 *    TMonitorService
 *
 *[消息方向]
 *    MT <= MTService
 */_event(  ev_MonitorSwitchParamInd  )
  _body ( TMonitorService, 1    )
   _ev_end
 
/***********************<< 监控参数设置>>************************ 
 *[消息体]    
 *    TMonitorService
 *
 *[消息方向]
 *    MT <= MTService
 */_event(  ev_MonitorSwitchParamCmd  )
  _body ( TMonitorService, 1    )
   _ev_end

/***********************<< pc双流视频参数指示 >>************************	
 *[消息体]			 
 *    +TVideoParam
 *[消息方向]
 *    MtService => UI/mtc
 */_event(  ev_CfgPCDualVideoParamInd  )
   _body( TVideoParam ,1 )
   _ev_end

/***********************<< pc双流视频参数配置 >>************************	
 *[消息体]			 
 *    +TVideoParam
 *[消息方向]
 *    UI => MtService
 */_event(  ev_CfgPCDualVideoParamCmd  )
   _body( TVideoParam ,1 )
   _ev_end

/***********************<< 8010c显示VGA或者video参数指示>>************************ 
 *[消息体]    
 *    u16
 *
 *[消息方向]
 *    MT <= MTService
 */_event(  ev_8010cShowModelParamInd  )
  _body ( TVgaOutCfg, 1    )
   _ev_end
 
/***********************<< 8010c显示VGA或者video参数设置>>************************ 
 *[消息体]    
 *    u16
 *
 *[消息方向]
 *    MT <= MTService
 */_event(  ev_8010cShowModelParamCmd  )
  _body ( TVgaOutCfg, 1    )
   _ev_end

 
/***********************<< 双流码率比设置>>************************ 
 *[消息体]    
 *    u8
 *
 *[消息方向]
 *    MT <= MTService
 */_event(  ev_CfgDualCodeRationCmd  )
  _body ( TDualRation, 1    )
   _ev_end

/***********************<< 双流码率比指示>>************************ 
 *[消息体]    
 *    u8
 *
 *[消息方向]
 *    MT <= MTService
 */_event(  ev_CfgDualCodeRationInd  )
  _body ( TDualRation, 1    )
   _ev_end
   
/***********************<< pcmt视频源配置 >>************************	
 *[消息体]		 
 *  +TCapType  
 *[消息方向]
 *    pcmt => MtServie
 */_event ( ev_CfgPcmtVidSourceCmd )
  _body ( TCapType, 1 )
  _ev_end

/***********************<< pcmt视频源配置指示 >>************************	
 *[消息体]		 
 *  +TCapType     
 *[消息方向]
 *    MtServie => pcmt
 */_event ( ev_CfgPcmtVidSourceInd )
  _body ( TCapType , 1 )
  _ev_end
  
//////////////////////////////////////////////////////////////////////////
//[xujinxing-2006-10-17]
/***********************<< 内嵌mc配置 >>************************	
 *[消息体]		 
 *  + TInnerMcCfg //mc配置的结构    
 *[消息方向
 *   ui => mtservice => mcservice
 */_event ( ev_CfgInnerMcCmd )
  _body ( TInnerMcCfg , 1 )
  _ev_end
  
//////////////////////////////////////////////////////////////////////////
//[xujinxing-2006-10-17]
/***********************<< 内嵌mc配置指示 >>************************	
 *[消息体]		 
 *  + TInnerMcCfg //mc配置的结构    
 *[消息方向
 *   ui <= mtservice 
 */_event ( ev_CfgInnerMcInd )
  _body ( TInnerMcCfg , 1 )
  _ev_end
  
//////////////////////////////////////////////////////////////////////////
//[fangtao-2007-01-17]
/***********************<< 快捷键配置命令 >>************************	
 *[消息体]		 
 *  + TRapidKey    
 *[消息方向
 *   ui => mtservice 
 */_event ( ev_CfgRapidKeyCmd )
  _body ( TRapidKey , 1 )
  _ev_end

//////////////////////////////////////////////////////////////////////////
//[fangtao-2007-01-17]
/***********************<< 快捷键配置指示 >>************************	
 *[消息体]		 
 *  + TRapidKey    
 *[消息方向
 *   ui <= mtservice 
 */_event ( ev_CfgRapidKeyInd )
  _body ( TRapidKey , 1 )
  _ev_end
  

//////////////////////////////////////////////////////////////////////////
//[fangtao-2007-01-22]
/***********************<< T2主视频配置命令 >>************************	
 *[消息体]		 
 *  + BOOL  TRUE表示输出在Video    
 *[消息方向
 *   ui => mtservice 
 */_event ( ev_CfgT2MainVideoOutCmd )
  _body ( BOOL , 1 )
  _ev_end

//////////////////////////////////////////////////////////////////////////
//[fangtao-2007-01-22]
/***********************<< 快捷键配置指示 >>************************	
 *[消息体]		 
 *  + BOOL  TRUE表示输出在Video   
 *[消息方向
 *   ui <= mtservice 
 */_event ( ev_CfgT2MainVideoOutInd )
  _body ( BOOL , 1 )
  _ev_end

//////////////////////////////////////////////////////////////////////////
//[fangtao-2007-03-12]
/***********************<< 地址簿升级指示 >>************************	
 *[消息体]		 
 *  + s8  100  已经上传好的地址簿文件所在路径   
 *[消息方向
 *   ui <= mtservice 
 */_event ( ev_AddrbookUploadInd )
  _body ( s8 ,    100 )
  _ev_end

/***********************<< License升级指示 >>************************	
 *[消息体]		 
 *  + s8  100  已经上传好的License文件所在路径   
 *[消息方向
 *   ui <= mtservice 
 */_event ( ev_LicenseKeyUploadInd )
  _body ( s8 ,    100 )
  _ev_end


/***********************<< 内置GK配置命令 >>************************	
 *[消息体]		 
 *  + TInnerGK     
 *[消息方向
 *   ui <= mtservice 
 */_event ( ev_CfgInnerGKCmd )
  _body( TInnerGKCfg, 1 )
  _ev_end

/***********************<< 内置GK配置指示 >>************************	
 *[消息体]		 
 *  + TInnerGK     
 *[消息方向
 *   ui <= mtservice 
 */_event ( ev_CfgInnerGKInd )
  _body( TInnerGKCfg, 1 ) 
  _ev_end 

/***********************<< 内置代理配置命令 >>************************	
 *[消息体]		 
 *  + TInnerGK     
 *[消息方向
 *   ui <= mtservice 
 */_event ( ev_CfgInnerProxyCmd )
  _body( TInnerProxyCfg, 1 ) 
  _ev_end

/***********************<< 内置代理配置指示 >>************************	
 *[消息体]		 
 *  + TInnerGK     
 *[消息方向
 *   ui <= mtservice 
 */_event ( ev_CfgInnerProxyInd )
  _body( TInnerProxyCfg, 1 ) 
  _ev_end


/***********************<< 向导配置改动需要重启命令 >>************************	
 *[消息体]		 
   
 *[消息方向
 *   ui => mtservice 
 */_event ( ev_GuideNeedRebootCmd )
  _ev_end

/***********************<< 配置网络WAN/LAN选择命令 >>************************	
 *[消息体]		 
 * + u8   最右三位代表 MT/GK/PXY, 1表示选择LAN
 *[消息方向
 *   ui => mtservice 
 */_event ( ev_CfgSetNetSelectMaskCmd )
  _body( u8          ,1 )
  _ev_end

/***********************<< 配置网络WAN/LAN选择指示 >>************************	
 *[消息体]		 
 * + u8   最右三位代表 MT/GK/PXY, 1表示选择LAN
 *[消息方向
 *   mtservice => ui
 */_event ( ev_CfgSetNetSelectMaskInd )
  _body( u8          ,1 )
  _ev_end


/***********************<< 配置MTU命令 >>************************	
 *[消息体]		 
 * + u32   MTU值
 *[消息方向
 *   ui => mtservice 
 */_event ( ev_CfgSetMTUCmd )
  _body( u32          ,1 )
  _ev_end

/***********************<< 配置MTU指示 >>************************	
 *[消息体]		 
 * + TMtuInfo   MTU值
 *[消息方向
 *   mtservice => ui
 */_event ( ev_CfgSetMTUInd )
  _body( u32          ,1 )
  _ev_end 
  

/***********************<< 配置音质优先命令 >>************************	
 *[消息体]		 
 * + u32   音质优先等级
 *[消息方向
 *   ui => mtservice
 */_event ( ev_CfgSetAudioPrecedenceCmd )
  _body( u32         ,1 )
  _ev_end 

/***********************<< 配置音质优先指示 >>************************	
 *[消息体]		 
 * + u32   音质优先等级
 *[消息方向
 *   mtservice => ui
 */_event ( ev_CfgSetAudioPrecedenceInd )
  _body( u32         ,1 )
  _ev_end 
  

/***********************<< 配置最近呼叫码率命令 >>************************	
 *[消息体]		 
 * + u16   最近呼叫码率
 *[消息方向
 *   ui => mtservice
 */_event ( ev_CfgSetLastCallRateCmd )
  _body( u16         ,1 )
  _ev_end 

/***********************<< 配置最近呼叫码率指示 >>************************	
 *[消息体]		 
 * + u16   最近呼叫码率
 *[消息方向
 *   mtservice => ui
 */_event ( ev_CfgSetLastCallRateInd )
  _body( u16         ,1 )
  _ev_end 


/***********************<< 配置最近呼叫地址记录命令 >>************************	
 *[消息体]		 
 * + s8[MT_MAXNUM_CALLADDR_RECORD][MT_MAX_H323ALIAS_LEN+1]最近呼叫地址记录
 *[消息方向
 *   ui => mtservice
 */_event ( ev_CfgSetCallAddrRecordCmd )
  _body( s8         ,MT_MAXNUM_CALLADDR_RECORD*(MT_MAX_H323ALIAS_LEN+1) )
  _ev_end 

/***********************<< 配置最近呼叫地址记录指示 >>************************	
 *[消息体]		 
 * + s8[MT_MAXNUM_CALLADDR_RECORD][MT_MAX_H323ALIAS_LEN+1]   最近呼叫地址记录
 *[消息方向
 *   mtservice => ui
 */_event ( ev_CfgSetCallAddrRecordInd )
  _body( s8         ,MT_MAXNUM_CALLADDR_RECORD*(MT_MAX_H323ALIAS_LEN+1) )
  _ev_end 


/***********************<< 配置最近电话号码记录命令 >>************************	
 *[消息体]		 
 * + s8[MT_MAXNUM_CALLADDR_RECORD][MT_MAX_H323ALIAS_LEN+1]最近电话号码记录
 *[消息方向
 *   ui => mtservice
 */_event ( ev_CfgSetTeleAddrRecordCmd )
  _body( s8         ,MT_MAXNUM_CALLADDR_RECORD*(MT_MAX_H323ALIAS_LEN+1) )
  _ev_end 

/***********************<< 配置最近电话号码记录指示 >>************************	
 *[消息体]		 
 * + s8[MT_MAXNUM_CALLADDR_RECORD][MT_MAX_H323ALIAS_LEN+1]   最近电话号码记录
 *[消息方向
 *   mtservice => ui
 */_event ( ev_CfgSetTeleAddrRecordInd )
  _body( s8         ,MT_MAXNUM_CALLADDR_RECORD*(MT_MAX_H323ALIAS_LEN+1) )
  _ev_end 
   

/***********************<< 内置代理ip地址配置的指示 >>************************	
 *[消息体]			 
 *    + TPxyIPCfg[MT_MAX_PXYIP_NUM] 
 *[消息方向]
 *    MtService => UI
 */_event(  ev_CfgPxyIPInd  )
   _body( TPxyIPCfg ,MT_MAX_PXYIP_NUM  )
   _ev_end

/***********************<< 增加内置代理的ip地址配置 >>************************	
 *[消息体]			 
 *    + TPxyIPCfg
 *[消息方向]
 *    UI => MtService
 */_event(  ev_CfgAddPxyIPCmd  )
   _body( TPxyIPCfg ,1  )  
   _ev_end

/***********************<< 删除内置代理的ip地址配置 >>************************	
 *[消息体]			 
 *    + TPxyIPCfg
 *[消息方向]
 *    UI => MtService
 */_event(  ev_CfgDeletePxyIPCmd  )
   _body( TPxyIPCfg ,1  )  
   _ev_end

/***********************<< 使内置代理的ip地址配置生效 >>************************	
 *[消息体]			 
 *  //该消息会重启终端，这个配置只有重启才会生效 
 *[消息方向]
 *    UI => MtService
 */_event(  ev_CfgApplyPxyIPCmd  )
   _ev_end

/***********************<< FEC配置命令 >>************************	
 *[消息体]			 
 *   + TFecInfo FEC算法类型 0:无 1:Raid5 2:Raid6
 *[消息方向]
 *    UI => MtService
 */_event(  ev_CfgFecTypeCmd  )
   _body( TFecInfo           ,1 )
   _ev_end

/***********************<< FEC配置指示 >>************************	
 *[消息体]			 
 *   + TFecInfo FEC算法类型 0:无 1:Raid5 2:Raid6
 *[消息方向]
 *    UI => MtService
 */_event(  ev_CfgFecTypeInd  )
   _body( TFecInfo           ,1 )
   _ev_end
   
/***********************<<  配置ldap服务器地址查询 >>********************	
 *[消息体]
 * +s8 , MT_MAX_LDAPSERVER_NAME_LEN  //对端的域名
 *  
 *[消息方向]
 *   mtservice  => mtservice 
 */_event(   ev_LdapServerIPCfgCmd )
   _body( s8 , MT_MAX_LDAPSERVER_NAME_LEN )
   _ev_end

/***********************<<  配置ldap服务器地址查询指示 >>********************	
 *[消息体]
 * +s8 , MT_MAX_LDAPSERVER_NAME_LEN  //对端的域名
 *  
 *[消息方向]
 *   mtservice  => mtservice 
 */_event(   ev_LdapServerIPCfgInd )
   _body( s8 , MT_MAX_LDAPSERVER_NAME_LEN )
   _ev_end
   
/***********************<<  WiFi网络地址配置 >>********************	
 *[消息体]
 * +TWiFiNetCfg , 1 //WiFi网络地址配置结构
 *  
 *[消息方向]
 *   mtservice  => mtservice 
 */_event(   ev_CfgWiFiNetCmd )
   _body( TWiFiNetCfg, 1 )
   _ev_end

/***********************<<  WiFi网络地址指示 >>********************	
 *[消息体]
 * +TWiFiNetCfg , 1 //WiFi网络地址配置结构
 *  
 *[消息方向]
 *   mtservice  => mtservice 
 */_event(   ev_CfgWiFiNetInd ) 
   _body( TWiFiNetCfg, 1 )
   _ev_end
 
   
#ifndef _MakeMtEventDescription_
   _ev_segment_end(config)
};

inline BOOL IsConfigEvent(u16 wEvent)
{
	return (wEvent > get_first_event_val(config)) && (wEvent < get_last_event_val(config));
}

#endif
#endif

