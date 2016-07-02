#ifndef MT_EXTEND_EVENT_H
#define MT_EXTEND_EVENT_H
#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmMtExtend
{
#endif 
   _ev_segment( extend ) //扩展的外部消息

/***********************<<  增加WiFi网络连接条目配置 >>********************	
 *[消息体]
 * + TWiFiBSSIDCfg , 1 //WiFi网络连接条目配置结构
 *  
 *[消息方向]
 *   UI => mtservice 
 */_event( ev_WiFiCfgAddItemCmd )
   _body( TWiFiBSSIDCfg, 1 )
   _ev_end


/***********************<<  删除WiFi网络连接条目配置 >>********************	
 *[消息体]
 * + TWiFiBSSIDCfg , 1 //WiFi网络连接条目配置结构
 *  
 *[消息方向]
 *   UI => mtservice 
 */_event( ev_WiFiCfgDelItemCmd )
   _body( TWiFiBSSIDCfg, 1 )
   _ev_end
   
/***********************<<  修改WiFi网络连接条目配置 >>********************	
 *[消息体]
 * + u32 , 1 //条目索引 
 * + TWiFiBSSIDCfg , 1 //WiFi网络连接条目配置结构
 *  
 *[消息方向]
 *   UI => mtservice 
 */_event( ev_WiFiCfgModifyItemCmd )
   _body( u32, 1 )
   _body( TWiFiBSSIDCfg, 1 )
   _ev_end

/***********************<< 连接WiFi网络命令 >>********************	
 *[消息体]
 * + TWiFiBSSIDInfo , 1 //WiFi网络连接扫描结构
 *  
 *[消息方向]
 *   UI => mtservice 
 */_event( ev_WiFiConnectCmd )
   _body( TWiFiBSSIDInfo, 1 )
   _ev_end
   
   
/***********************<< 断开WiFi网络命令 >>********************	
 *[消息体]
 *  + 无
 *  
 *[消息方向]
 *   UI  => mtservice 
 */_event( ev_WiFiDisConnectCmd )
   _ev_end
   

/***********************<< WiFi网络连接状态指示 >>********************	
 *[消息体]
 *  + TWifiLinkCfgItem , 1 //WiFi网络连接状态结构
 *  
 *[消息方向]
 *   mtservice  => UI 
 */_event( ev_WiFiLinkStatInd  )
   _body ( TWifiLinkCfgItem, 1 )
   _ev_end


/***********************<< 获取WiFi网络连接状态命令 >>********************	
 *[消息体]
 *  + 无
 *  
 *[消息方向]
 *   UI  => mtservice 
 */_event( ev_WiFiGetLinkStatCmd )
   _ev_end


/***********************<< WiFi网络连接配置指示 >>********************	
 *[消息体]
 *  + TWiFiBSSIDCfg , MT_WIFI_MAX_BSSIDCFG_NUM //WiFi网络连接配置结构
 *  
 *[消息方向]
 *   mtservice  => UI
 */_event( ev_WiFiCfgInd )
   _body ( TWiFiBSSIDCfg, MT_WIFI_MAX_BSSIDCFG_NUM )
   _ev_end


/***********************<< WiFi网络扫描命令 >>********************	
 *[消息体]
 *  + 无
 *  
 *[消息方向]
 *   UI  => mtservice 
 */_event( ev_WiFiScanBSSCmd )
   _ev_end


/***********************<< WiFi网络扫描结果指示 >>********************	
 *[消息体]
 *  + TWiFiBSSIDInfo , MT_WIFI_MAX_BSSIDCFG_NUM //WiFi网络连接扫描结构
 *  
 *[消息方向]
 *   mtservice  => UI 
 */_event( ev_WiFiScanBSSInd )
   _body ( TWiFiBSSIDInfo, MT_WIFI_MAX_BSSIDCFG_NUM )
   _ev_end

   
/***********************<< 防火墙代理域名解析指示 >>**********************	
 *[消息体]	
 *    + u8        0-成功 1-超时 2-未找到 3-失败
 *[消息方向]
 *    UI <= MtService
 */_event(  ev_FwProxyDomainReqInd  )
   _body( u8        ,1 )
   _ev_end

/***********************<< MTC PC 分辨率指示 >>********************	
 *[消息体]
 *  
 *[消息方向]
 *   MT  <=> MTC
 */_event(   ev_MTCVideoResInd )
   _body( EmVideoResolution , 1 )
   _ev_end

/***********************<<  设置码流播放端口    >>********************	
 *[消息体]			 
 *     +EmVideoType first video or second video
 *      u32 : PORT_MASK
 *[消息方向]
 *    MtService = >MtMP
 */_event( ev_SetCodecVideoOutCmd  )
   _body ( EmVideoType   ,1)
   _body ( u32   ,1)
   _ev_end

/***********************<<  设置码流输入端口    >>********************	
 *[消息体]			 
 *     +EmVideoType first video or second video
 *      u32 : PORT_MASK
 *[消息方向]
 *    MtService = >MtMP
 */_event( ev_SetCodecVideoInCmd  )
   _body ( EmVideoType   ,1)
   _body ( u32   ,1)
   _ev_end


/***********************<<  指示当前的矩阵配置    >>********************	
 *[消息体]			 
 *     +EmVideoType first video or second video
 *      u32 : PORT_MASK
 *[消息方向]
 *    MtService = >MtMP
 */_event( ev_CurMatrixInd  )
   _body ( THDAVInnerMatrixScheme   ,1)
   _ev_end


/***********************<<  高清音频端口选择命令    >>********************	
 *[消息体]			 
 *     +THdAudioPort  音频端口
 *[消息方向]
 *    UI/MTC => MtService => MTMP_HD
 */_event( ev_CfgHDAudioPortCmd  )
   _body ( THdAudioPort   ,1)
   _ev_end

/***********************<<  高清音频端口选择指示    >>********************	
 *[消息体]			 
 *     +THdAudioPort  音频端口
 *[消息方向]
 *    UI/MTC <= MtService
 */_event( ev_CfgHDAudioPortInd  )
   _body ( THdAudioPort   ,1)
   _ev_end

/***********************<<  切包传输命令    >>********************	
 *[消息体]			 
 *     +BOOL  是否切包传输
 *[消息方向]
 *    UI/MTC => MtService => MTMP_HD
 */_event( ev_CfgUseSliceCmd  )
   _body ( BOOL   ,1)
   _ev_end

/***********************<<  切包传输指示    >>********************	
 *[消息体]			 
 *     +BOOL  是否切包传输
 *[消息方向]
 *    UI/MTC <= MtService
 */_event( ev_CfgUseSliceInd  )
   _body ( BOOL   ,1)
   _ev_end

/***********************<<  AAC音频配置参数    >>********************	
 *[消息体]			 
 *     +EmCodecComponent	音频类型
 *     +u8		            通道参数（0--7, 其中6表示5.1声道, 7表示7.1声道）
 *     +u32					采样参数（0表示无效）
 *	   +u16					码率（目前未定义）
 *[消息方向]
 *    MtService => MTC/MTMP
 */_event( ev_CodecAudAacParamCmd  )
   _body ( EmCodecComponent		,1 )
   _body ( u8                   ,1 )
   _body ( u32                  ,1 )
   _body ( u16                  ,1 )
   _ev_end

/***********************<<  高清图像参数设置命令  >>********************	
 *[消息体]			 
 *     +TImageAdjustParam	对于高清目前有三个配置
 *[消息方向]
 *    MtService <= MTC/MTMP
 */_event( ev_CfgHDImageParamCmd  )
   _body( TImageAdjustParam     ,REMAIN )
   _ev_end

/***********************<<  高清图像参数设置指示  >>********************	
 *[消息体]			 
 *     +TImageAdjustParam	对于高清目前有三个配置
 *[消息方向]
 *    MtService => MTC/MTMP
 */_event( ev_CfgHDImageParamInd  )
   _body( TImageAdjustParam     ,REMAIN )
   _ev_end

/***********************<<  画面模式参数设置命令  >>********************	
 *[消息体]			 
 *     +TImageAdjustParam	对于高清目前有三个配置
 *[消息方向]
 *    MtService => MTC/MTMP
 */_event( ev_CfgScreenModeCmd  )
   _body( u8     , 1 )
   _ev_end

/***********************<<  高清图像参数设置指示  >>********************	
 *[消息体]			 
 *     +TImageAdjustParam	对于高清目前有三个配置
 *[消息方向]
 *    MtService => MTC/MTMP
 */_event( ev_CfgScreenModeInd  )
   _body( u8     , 1 )
   _ev_end

/***********************<<  高清DVI参数设置命令  >>********************	
 *[消息体]			 
 *     +BOOL  是否启用DVI做输出接口
 *[消息方向]
 *    MtService <= MTC/MTMP
 */_event( ev_CfgHDDVIParamCmd  )
   _body( BOOL     , 1 )
   _ev_end

/***********************<<  高清DVI参数设置指示  >>********************	
 *[消息体]			 
 *     +BOOL  是否启用DVI做输出接口
 *[消息方向]
 *    MtService => MTC/MTMP
 */_event( ev_CfgHDDVIParamInd  )
   _body( BOOL     , 1 )
   _ev_end
   
/***********************<< 获取系统运行时间指示 >>**********************	
 *[消息体]	
 *    + u32 启动时间,单位为秒
 *[消息方向]
 *    UI <= MtService
 */_event(  ev_GetSystemRunTimeInd )
   _body( u32        ,1 )
   _ev_end
   
/***********************<< 获取系统运行时间命令 >>**********************	
 *[消息体]	
 *    + 无
 *[消息方向]
 *    UI <= MtService
 */_event(  ev_GetSystemRunTimeCmd )
   _ev_end
   
/***********************<< 网口模式配置命令 >>************************	
 *[消息体]			 
 *   + u32 网库模式类型 
 *[消息方向]
 *    UI => MtService
 */_event(  ev_CfgEthModeCmd  )
   _body( u32           ,1 )
   _ev_end
   
/***********************<< 网口模式配置指示 >>************************	
 *[消息体]			 
 *   + u32 网库模式类型 
 *[消息方向]
 *    UI => MtService
 */_event(  ev_CfgEthModeInd  )
   _body( u32           ,1 )
   _ev_end
     
  
/***********************<< MC主席选看命令  >>*************************
 *[消息体]
 *  BOOL  +1
 *  TMtId +1
 *[消息方向]
 *  UI => MtService => MC
 */_event( ev_mcViewCmd )
   _body( BOOL,      1 )
   _body( TMtId,     1 )
   _ev_end

/***********************<< MC主席选看指示  >>*************************
 *[消息体]
 *  BOOL  +1
 *  TMtId +1
 *[消息方向]
 *  UI <= MtService <= MC
 */_event( ev_mcViewInd )
   _body( BOOL,      1 )
   _body( TMtId,     1 )
   _ev_end
   
  
   
/***********************<< pcmt设置视频采集参数命令  >>*************************
 *[消息体]
 *  u16     1  视频捕获图像宽度
 *  u16     1  视频捕获图像高度
 *  u8      1  帧率是否小于1
 *  u8      1  视频捕获帧率
 *  u8      1  视频捕获帧格式
 *  u32     2  保留位
 *[消息方向]
 *  MtService <= PCMT
 */_event( ev_PcmtSetCapParamCmd )
   _body( u16,     1 )
   _body( u16,     1 )
   _body( u8,      1 )
   _body( u8,      1 )
   _body( u8,      1 )
   _body( u32,     2 )
   _ev_end
    
  
/*********<< pcmt数据会议请求 >>**************	
 *[消息体]			 
 *
 *[消息方向]
 *
 *   pcmt => mtservice
 *
 */_event(  ev_PcmtDataConfReq   )    
   _ev_end

/*********<< pcmt数据会议应答 >>**************	
 *[消息体]			 
 * BOOL //TRUE,允许;FALSE,不允许
 *[消息方向]
 *
 *   mtservice => pcmt
 *
 */_event( ev_PcmtDataConfRsp   )
   _body ( BOOL,    1           )
   _ev_end
   
/***********************<< 从编码器获取的第一二路帧率上报 >>************************	
 *[消息体]		 
 *  +u16  //第一路视频编码帧率
 *  +u16  //第一路视频解码帧率
 *  +u16  //第二路视频编码帧率
 *  +u16  //第二路视频解码帧率
 *[消息方向]
 *    MtService => UI
 */_event ( ev_VidFramerateInd )
  _body ( u16, 1 )
  _body ( u16, 1 )
  _body ( u16, 1 )
  _body ( u16, 1 )
  _ev_end                           

/***********************<<  按高清分辨率输出指示  >>********************	
 *[消息体]			 
 *     +BOOL  是否启用按高清分辨率输出
 *[消息方向]
 *    MtService => MTC/MTMP
 */_event( ev_CfgHDResOutputInd  )
   _body( u32     , 1 )
   _body( u32     , 1 )//Reversed 1
   _body( u32     , 1 )//Reversed 2  
   _ev_end  

/***********************<<  按高清分辨率输出命令  >>********************	
 *[消息体]			 
 *     +BOOL  是否启用按高清分辨率输出
 *[消息方向]
 *    MtService => MTC/MTMP
 */_event( ev_CfgHDResOutputCmd  )
   _body( u32     , 1 )
   _body( u32     , 1 )//Reversed 1
   _body( u32     , 1 )//Reversed 2  
   _ev_end  


/***********************<<  直接调整图像参数的命令  >>********************	
 *[消息体]			 
 *     +EmCodecComponent  所控制的对象，目前只能是视频主编码器
 *     +u32               操作类型 MT_SCALE_BRIGHTNESS、MT_SCALE_CONTRAST、MT_SCALE_HUE、MT_SCALE_SATURATION           
 *     +u32               操作值
 *[消息方向]
 *    UI => MtService => MTMP
 */_event( ev_AdjustImageParamDirectlyCmd  )
   _body( EmCodecComponent     , 1 )
   _body( u32                  , 1 )
   _body( u32                  , 1 )
   _body( u32                  , 2 ) // Reversed
   _ev_end  
   
/***********************<<  高清PicPlus参数设置命令  >>********************	
 *[消息体]			 
 *     +BOOL  是否启用PicPlus做输出接口
 *[消息方向]
 *    MtService <= MTC/MTMP
 */_event( ev_CfgHDPicPlusParamCmd  )
   _body( BOOL     , 1 )
   _ev_end
   
/***********************<<  高清PicPlus参数设置指示  >>********************	
 *[消息体]			 
 *     +BOOL  是否启用PicPlus做输出接口
 *[消息方向]
 *    MtService => MTC/MTMP
 */_event( ev_CfgHDPicPlusParamInd  )
   _body( BOOL     , 1 )
   _ev_end

/***********************<<  PCMT视频工作模式设置命令  >>********************	
 *[消息体]
 *     +u8  PCMT视频编解码方式  PCMT_VCODEC_SOFTWARE--软件编解码  PCMT_VCODEC_HARDWARE--硬件显卡编解码
 *     +u8  编码/解码 (0:编码  1:解码)   
 *[消息方向]
 *    MtService <= PCMT
 */_event( ev_CfgVideoWorkModeCmd  )
   _body( u8     , 1 )
   _body( u8     , 1 )
   _body( u32    , 1 )
   _body( u32    , 1 )
   _ev_end

/***********************<<  PCMT视频工作模式设置指示  >>********************	
 *[消息体]
 *     +u8  PCMT视频编解码方式  PCMT_VCODEC_SOFTWARE--软件编解码  PCMT_VCODEC_HARDWARE--硬件显卡编解码
 *     +u8  编码/解码 (0:编码  1:解码) 
 *[消息方向]
 *    MtService => PCMT
 */_event( ev_CfgVideoWorkModeInd  )
   _body( u8     , 1 )
   _body( u8     , 1 )
   _body( u32    , 1 )
   _body( u32    , 1 )
   _ev_end

/***********************<<  输出模式帧率设置命令  >>********************	
 *[消息体]			 
 *     +TVideoOutputFrm  输出帧率

 *[消息方向]
 *    MtService <= MTC/MTMP
 */_event( ev_CfgHDVideoFrmCmd  )
   _body( TVideoDisplayFrm     , 1 )
   _ev_end

/***********************<<  输出模式帧率设置提示  >>********************	
 *[消息体]			 
 *     TVideoOutputFrm  输出帧率
 *[消息方向]
 *    MtService <= MTC/MTMP
 */_event( ev_CfgHDVideoFrmInd  )
   _body( TVideoDisplayFrm      , 1 )
   _ev_end
   

/***********************<< 编解码器实际的码率分辨率统计信息指示 >>************************	
 *[消息体]		 
 *  +EmVideoResolution： 分别为pvenc,svenc, pvdec,svdec
 *[消息方向]
 *    MtMP => MtService 
 */_event ( ev_CodecVideoEncDecResInd )
  _body ( EmVideoResolution, 1 )
  _body ( EmVideoResolution, 1 )
  _body ( EmVideoResolution, 1 )
  _body ( EmVideoResolution, 1 )
  _ev_end
  
/***********************<< 终端（仅限主席MT）选看画面合成请求 >>**********************	
 *[消息体]			 
 *  无
 *[消息方向]
 *
 *   MtService => 323/sip/320
 */_event( ev_ConfmtSelectVmpReq  )
   _body ( BOOL           ,1 )
   _ev_end

/***********************<< 终端（仅限主席MT）选看画面合成反馈 >>**********************	
 *[消息体]			 
 *  BOOL
 *[消息方向]
 *
 *   MtService <= 323/sip/320
 */_event( ev_ConfmtSelectVmpRsp  )
   _body ( BOOL           ,1 )
   _ev_end

#ifndef _MakeMtEventDescription_
   _ev_segment_end( extend )
};
#endif

#endif
