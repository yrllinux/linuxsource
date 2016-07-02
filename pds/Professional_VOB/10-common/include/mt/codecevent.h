#ifndef CODEC_EVENT_H
#define CODEC_EVENT_H
#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmCodec
{
#endif  
   _ev_segment( codec )//媒体处理消息

/**********************<< PCMT 抓图  >>********************	
 *[消息体]			 
 *     +TPCCapParam
 *[消息方向]
 *    PCMT UI  = > MtMP
 */_event( ev_GrabPicCmd  )
   _body ( EmCodecComponent   ,1) //抓图组件 主编器、第一路解码器、第二路解码器
   _body ( u32                ,1) //CDrawWnd指针
   _ev_end

/***********************<<  设置丢包重传参数    >>********************	
 *[消息体]			 
 *     +TLostPackResend
 *[消息方向]
 *    MtService = >MtMP
 */_event( ev_CodecLostPackResendCmd  )
   _body ( TLostPackResend   ,1)
   _ev_end
/***********************<< 设置解码器接收地址 >>**********************	
 *[消息体]			 
 *  + EmCodecComponent
 *  + TIPTransAddr      RTP接收地址      
 *  + TIPTransAddr      RTCP接收地址
 *  + TIPTransAddr      RTCP发送目的地址
 *  + u32               接收函数回调指针(H320)
 *[消息方向]
 *   MtService ->Codec
 */_event( ev_CodecRecvAddrCmd )
   _body ( EmCodecComponent, 1    )
   _body ( TIPTransAddr    , 1    )
   _body ( TIPTransAddr    , 1    )
   _body ( TIPTransAddr    , 1    )
   _body ( u32             , 1    )
   _ev_end


/***********************<< 设置解码器发送地址 >>**********************	
 *[消息体]			 
 *  + EmCodecComponent
 *  + TIPTransAddr      RTP发送目的地址 
 *  + TIPTransAddr      RTCP发送目的地址
 *  + TIPTransAddr      RTCP接收地址
 *  + u32               发送函数回调指针(H320)
 *[消息方向]
 *   MtService ->Codec
 */_event( ev_CodecSendAddrCmd )
   _body ( EmCodecComponent, 1    )
   _body ( TIPTransAddr    , 1    )
   _body ( TIPTransAddr    , 1    )
   _body ( TIPTransAddr    , 1    )
   _body ( u32             , 1    )
   _ev_end

/***********************<< 选择视频源 >>**********************	
 *[消息体]			 
 *  + EmVideoType ：视频类型，主/幅
 *  + EmMtVideoPort ：视频源类型
 *[消息方向]
 *  MtService  ->Codec
 */_event( ev_VideoSourceSelCmd )
   _body ( EmVideoType    , 1    )
   _body ( EmMtVideoPort    , 1    )
   _ev_end

/***********************<< 选择视频源 >>**********************	
 *[消息体]			 
 *  + EmVideoType ：视频类型，主/幅
 *  + EmMtVideoPort ：视频源类型
 *[消息方向]
 *  MtService  ->UI
 */_event( ev_VideoSourceSelInd )
   _body ( EmVideoType    , 1    )
   _body ( EmMtVideoPort    , 1    )
   _ev_end

/***********************<< 设置视频编码参数 >>**********************	
 *[消息体]			 
 *  + EmCodecComponent  =emPriomVideoEncoder/emSecondVideoEncoder
 *  + TVideoEncodeParameter 
 *[消息方向]
 *  MtService  ->Codec
 */_event( ev_CodecVideoEncodeParamCmd )
   _body ( EmCodecComponent    , 1    )
   _body ( TVideoEncodeParameter    , 1    )
   _ev_end

/***********************<< 设置音频编码参数 >>**********************	
 *[消息体]			 
 *  +EmCodecComponent  =emAudioEncoder
 *  + EmAudioFormat 
 *[消息方向]
 * MtService  ->Codec
 */_event( ev_CodecAudioEncodeParamCmd )
   _body ( EmCodecComponent    , 1    )
   _body ( EmAudioFormat    , 1    )
   _ev_end

/***********************<< 设置视频解码参数 >>**********************	
 *[消息体]			 
 *  +EmCodecComponent  =emPriomVideoDecoder/emSecondVideoDecoder
 *  + EmLostPackageRestore ： 丢包恢复方式
 *  + EmVideoFormat    ：解码格式
 *[消息方向]
 * MtService  ->Codec
 */_event( ev_CodecVideoDecodeParamCmd )
   _body ( EmCodecComponent    , 1    )
   _body ( EmLostPackageRestore    , 1    )
   _body ( EmVideoFormat    , 1    )
   _ev_end

/***********************<< 设置音频解码参数 >>**********************	
 *[消息体]			 
 *  +EmCodecComponent  =emAudioDecoder 
 *  +EmAudioFormat 
 *[消息方向]
 * MtService  ->Codec
 */_event( ev_CodecAudioDecodeParamCmd )
   _body ( EmCodecComponent    , 1    )
   _body ( EmAudioFormat       , 1    ) 
   _ev_end

/***********************<< 开始编解码器工作 >>**********************	
 *[消息体]			 
 *  +EmCodecComponent  
 *[消息方向]
 * MtService  ->Codec
 */_event( ev_CodecStartCmd )
   _body ( EmCodecComponent    , 1    )
   _ev_end

/***********************<< 停止编解码器工作 >>**********************	
 *[消息体]			 
 *  +EmCodecComponent  
 *[消息方向]
 * MtService  ->Codec
 */_event( ev_CodecStopCmd )
   _body ( EmCodecComponent    , 1    )
   _ev_end
   
/***********************<< 开始监控 >>**********************	
 *[消息体]			 
 *  +EmSite   本地/远端
 *  +TIPTransAddr   监控地址
 *[消息方向]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_MonitorStartCmd )
   _body ( EmSite    , 1    )
   _body ( TIPTransAddr    , 1    )
   _ev_end

/***********************<< 停止监控 >>**********************	
 *[消息体]			 
 *  +EmSite   本地/远端
 *[消息方向]
 * UI         ->MtService
 * MtService  ->Codec
 *->Codec
 */_event( ev_MonitorStopCmd )
   _body ( EmSite    , 1    )
   _ev_end

/***********************<< 请求监控图像关键帧 >>************************	
 *[消息体]		 
 *  +EmSite	    本地/远端
 *[消息方向]
 * UI         ->MtService
 * MtService  ->Codec
 */_event ( ev_MonitorVideoFastUpadateCmd )
  _body ( EmSite, 1 )
  _ev_end
    
/***********************<< 视频监控图像参数指示 >>************************	
 *[消息体]		 
 * + u8[2] payload type   第一路视频和音频的载荷类型
 * + u8[2] dynamic ploady 第一路视频和音频的动态载荷类型 
 * + TEncryptKey[2]       第一路视频和音频的密钥
 * + EmSite               本地or远端
 *[消息方向]
 * Codec      -> MtService
 * MtService  -> UI
 */_event ( ev_MonitorVideoParamInd )
  _body ( u8, 2 )
  _body ( u8, 2 )
  _body ( TEncryptKey, 2 )
  _body ( EmSite, 1)
  _ev_end

/***********************<< 开始流媒体功能 >>**********************	
 *[消息体]			 
 *  +TStreamMedia
 *[消息方向]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_CodecStreamMediaCmd      )
   _body ( TStreamMedia    , 1    )
   _ev_end

/***********************<< 开始指定组件流媒体功能 >>**********************	
 *[消息体]		
 *  + EmCodecComponent -组件名
 *  + u32_ip  - 组播地址
 *  + u16     - 端口号
 *  + u8      - TTL值
 *[消息方向]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_CodecStreamMediaStartCmd      )
   _body (  EmCodecComponent ,1)
   _body ( u32_ip    , 1    )
   _body ( u16       , 1    )
   _body ( u8        , 1    )
   _ev_end

/***********************<< 停止流指定组件媒体 >>**********************	
 *[消息体]			 
 *  + EmCodecComponent -组件名
 *[消息方向]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_CodecStreamMediaStopCmd )
  _body (  EmCodecComponent ,1)
  _ev_end

/***********************<< 停止全部组件流媒体 >>**********************	
 *[消息体]			 
 *  + 
 *[消息方向]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_CodecStreamMediaStopAllCmd )
  _ev_end

/***********************<< 流媒体源变化指示 >>**********************	
 *[消息体]			 
 * BOOL  TRUE: 当前看本地 /FALSE :当前看远端
 *[消息方向]
 * MtMP -> MtMP 
 */_event( ev_CodecStreamMediaChangedInd )
   _body ( BOOL , 1 )  
   _ev_end


/***********************<< 设置双流显示模式 >>**********************	
 *[消息体]			 
 *  +EmDualVideoShowMode
 *[消息方向]
 * UI         ->MtService
 */_event( ev_CodecDualShowModeCmd )
   _body ( EmDualVideoShowMode    , 1    )
   _ev_end


/***********************<< 设置显示比例模式 >>**********************	
 *[消息体]			 
 *  +EmDisplayRatio
 *[消息方向]
 * UI         ->MtService
 */_event( ev_CodecDisplayRatioCmd )
   _body ( EmDisplayRatio    , 1    )
   _ev_end
   

/***********************<< 画中画大小画面切换>>**********************	
 *[消息体]			 
 *  无
 *[消息方向]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_SwitchPiPCmd )
   _ev_end

/***********************<< 自环测试 >>**********************	
 *[消息体]			 
 *  +BOOL      TRUE=开始测试  、FALSE=停止测试
 *[消息方向]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_LoopbackTestCmd )
   _body ( BOOL    , 1    )
   _ev_end

/***********************<< 自环测试指示 >>**********************	
 *[消息体]			 
 *  +BOOL      TRUE=开始测试  、FALSE=停止测试
 *[消息方向]
 * MtService  ->UI
 */_event( ev_LoopbackTestInd )
   _body ( BOOL    , 1    )
   _ev_end

/***********************<< 自环生产测试 >>**********************	
 *[消息体]
 *  +EmMediaType      媒体类型
 *  +BOOL             TRUE=开始测试  、FALSE=停止测试
 *[消息方向]
 * MtService  ->Codec
 */_event( ev_AutoTestLoopbackCmd )
   _body ( EmMediaType    , 1    )
   _body ( BOOL           , 1    )
   _ev_end

   
/***********************<< 色带测试 >>**********************	
 *[消息体]			 
 *  +EmSite    本地/远端
 *  +BOOL      TRUE=开始测试  、FALSE=停止测试
 *[消息方向]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_RibbonTestCmd )
   _body ( EmSite    , 1    )
   _body ( BOOL    , 1    )
   _ev_end

/***********************<< 色带测试指示 >>**********************	
 *[消息体]			 
 *  +EmSite    本地/远端
 *  +BOOL      TRUE=开始测试  、FALSE=停止测试
 *[消息方向]
 * MtService  ->UI
 */_event( ev_RibbonTestInd )
   _body ( EmSite    , 1    )
   _body ( BOOL    , 1    )
   _ev_end
/***********************<< 发送静态图片 >>**********************	
 *[消息体]			 
 *  +BOOL      TRUE=开始发送  、FALSE=停止发送
 *[消息方向]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_SendPictureCmd )
   _body ( BOOL    , 1    )
   _ev_end

/***********************<< 发送静态图片指示 >>**********************	
 *[消息体]			 
 *  +BOOL      TRUE=开始发送  、FALSE=停止发送
 *[消息方向]
 * MtService  ->UI
 */_event( ev_SendPictureInd )
   _body ( BOOL    , 1    )
   _ev_end

/***********************<< 快照功能 >>**********************	
 *[消息体]			 
 *  +EmVideoType ：视频类型
 *[消息方向]
  * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_SnapshotCmd )
   _body ( EmVideoType    , 1    )
   _ev_end
   
/***********************<< 快照功能指示 >>**********************	
 *[消息体]			 
 *  TSnapInfo
 *[消息方向]
  * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_SnapshotInd )
   _body ( TSnapInfo    , 1    )
   _ev_end
   
/***********************<< 快照信息请求 >>**********************	
 *[消息体]			 
 *  NULL 
 *[消息方向]
  * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_SnapshotInfoCmd )
   _ev_end
/***********************<< 快照信息请求指示 >>**********************	
 *[消息体]			 
 *  TSnapInfo
 *[消息方向]
  * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_SnapshotInfoInd )
   _body ( TSnapInfo    , 1    )
   _ev_end

 /***********************<< 清空快照命令 >>**********************	
 *[消息体]			 
 *  NULL
 *[消息方向]
  * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_ClearAllSnapshotCmd )
   _ev_end
 
 /***********************<< 清空快照指示 >>**********************	
 *[消息体]			 
 *  BOOL
 *[消息方向]
  * MTC         <- MtService
 * MtService    <- Codec
 */_event( ev_ClearAllSnapshotInd )
    _body ( BOOL    , 1    )
   _ev_end

/***********************<< 设置静音功能 >>**********************	
 *[消息体]			 
 *  +BOOL      TRUE=静音  、FALSE=不静音
 *[消息方向]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_QuietCmd )
   _body ( BOOL    , 1    )
   _ev_end

/***********************<< 设置静音指示 >>**********************	
 *[消息体]			 
 *  +BOOL      TRUE=静音  、FALSE=不静音
 *[消息方向]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_QuietInd    )
   _body ( BOOL    , 1    )
   _ev_end

/***********************<< 设置哑音功能 >>**********************	
 *[消息体]			 
 *  +BOOL      TRUE=静音  、FALSE=不静音
 *[消息方向]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_MuteCmd )
   _body ( BOOL    , 1    )
   _ev_end

/***********************<< 设置哑音指示 >>**********************	
 *[消息体]			 
 *  +BOOL      TRUE=静音  、FALSE=不静音
 *[消息方向]
 * MtService  ->UI
 */_event( ev_MuteInd )
   _body ( BOOL    , 1    )
   _ev_end

/***********************<< 设置输入音量 >>**********************	
 *[消息体]			 
 *  +u8  音量值
 *[消息方向]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_InputVolumeCmd )
   _body ( u8    , 1    )
   _ev_end

/***********************<< 输出声音指示 >>************************	
 *[消息体]			 
 *    +u8 (输出声音值)
 *[消息方向]
 *    MtService => UI
 */_event(  ev_InputVolumeInd  )
   _body ( u8    , 1    )
   _ev_end

/***********************<< 设置输出音量 >>**********************	
 *[消息体]			 
 *  +u8  音量值
 *[消息方向]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_OutputVolumeCmd )
   _body ( u8    , 1    )
   _ev_end

/***********************<< 输出声音指示 >>************************	
 *[消息体]			 
 *    +u8 (输出声音值)
 *[消息方向]
 *    MtService => UI
 */_event(  ev_OutputVolumeInd  )
   _body ( u8    , 1    )
   _ev_end
   
/***********************<< 扬声器输出声音命令 >>************************	
 *[消息体]			 
 *    +EmMtLoudspeakerVal (输出声音值)
 *[消息方向]
 *    MtService => UI
 */_event(  ev_LoudSpeakerVolumeCmd  )
   _body ( EmMtLoudspeakerVal    , 1    )
   _ev_end

/***********************<< 扬声器输出声音指示 >>************************	
 *[消息体]			 
 *    +EmMtLoudspeakerVal (输出声音值)
 *[消息方向]
 *    MtService => UI
 */_event(  ev_LoudSpeakerVolumeInd  )
   _body ( EmMtLoudspeakerVal    , 1    )
   _ev_end

/***********************<< 开启/关闭回声抵消 >>**********************	
 *[消息体]			 
 *  +BOOL  TRUE= 开启 ;FALSE=关闭
 *[消息方向]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_AECCmd )
   _body ( BOOL    , 1    )
   _ev_end

/***********************<< 开启/关闭回声指示 >>**********************	
 *[消息体]			 
 *  +BOOL  TRUE= 开启 ;FALSE=关闭
 *[消息方向]
 * MtService  ->UI
 */_event( ev_AECInd )
   _body ( BOOL    , 1    )
   _ev_end

/***********************<< 开启/关闭自动增益控制 >>**********************	
 *[消息体]			 
 *  +BOOL  TRUE= 开启 ;FALSE=关闭
 *[消息方向]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_AGCCmd )
   _body ( BOOL    , 1    )
   _ev_end

/***********************<< AGC(自动增益控制)指示 >>************************	
 *[消息体]			 
 *    +BOOL TRUE=enable
 *[消息方向]
 *    MtService => UI
 */_event(  ev_AGCInd  )
   _body ( BOOL    , 1    )
   _ev_end
   
/***********************<< 开启/关闭自动噪音抑制 >>**********************	
 *[消息体]			 
 *  +BOOL  TRUE= 开启 ;FALSE=关闭
 *[消息方向]
 * UI         ->MtService
 * MtService  ->Codec
 */_event( ev_AnsCmd )
   _body ( BOOL    , 1    )
   _ev_end

/***********************<< Ans(自动噪音抑制控制)指示 >>************************	
 *[消息体]			 
 *    +BOOL TRUE=enable
 *[消息方向]
 *    MtService => UI
 */_event(  ev_AnsInd  )
   _body ( BOOL    , 1    )
   _ev_end



/***********************<< 设置动态载荷 >>**********************	
 *[消息体]			 
 *  +EmCodecComponent ：编解码器
 *  +u8	：动态载荷值
 *[消息方向]
 * MtService  ->Codec
 */_event( ev_CodecDynamicPayloadCmd )
   _body ( EmCodecComponent    , 1    )
   _body ( u8    , 1    )
   _ev_end
  
 /********************<< 视频码率调整  >>*****************************
 *[消息体]	
 *    + EmCodecComponent
 *    + u16       	码率值 kbps
 *[消息方向]
 *    MtService = > MtMP
*/_event ( ev_CodecSetBitrateCmd )
  _body ( EmCodecComponent , 1 )
  _body ( u16, 1 )
  _ev_end
  
 /********************<< 视频编码帧率调整  >>*****************************
 *[消息体]	
 *    + EmCodecComponent
 *    + u8       	编码帧率值
 *[消息方向]
 *    MtService = > MtMP
*/_event ( ev_CodecSetEncFrameRateCmd )
  _body ( EmCodecComponent , 1 )
  _body ( u8, 1 )
  _ev_end
  
 /********************<< 请求发送关键帧  >>*****************************
 *[消息体]	
 *    + EmCodecComponent  
 *[消息方向]
 *    MtService = > MtMP
*/_event ( ev_CodecVideoFastUpdateCmd  )
  _body ( EmCodecComponent , 1 )
  _ev_end
  
/***********************<< 只解关键帧命令 >>************************	
 *[消息体]			 
 *    +EmVideoType
 *[消息方向]
 * MtService  ->Codec
 */_event ( ev_DecodeIFrameOnlyCmd )
  _body ( EmVideoType, 1 )
  _ev_end
  
 /********************<< 音视频编解码密钥  >>*****************************
 *[消息体]	
 *    + EmCodecComponent
 *    + TEncryptKey
 *[消息方向]
 *    MtService = > MtMP
*/_event ( ev_CodecEncryptKeyCmd )
  _body ( EmCodecComponent , 1 )
  _body ( TEncryptKey, 1 )
  _ev_end
 
/***********************<< 设置窗口句柄 >>************************	
 *[消息体]			 
 *    +u32   预览窗口
 *    +u32   主解码器窗口
 *    +u32   辅解码器窗口
 *[消息方向]
 *    MtService => MtMP
 */_event ( ev_SetWindowHandle )
  _body ( u32, 1 )
  _body ( u32, 1 )
  _body ( u32, 1 )
  _ev_end
  
   
/***********************<< 注册双流盒 >>************************	
 *[消息体]		 
 *    无
 *[消息方向]
 *    MtSevice => MtMP 
 */ _event ( ev_CodecDualVidBoxRegCmd )
  _ev_end

/***********************<< 配置双流盒 >>************************	
 *[消息体]		 
 *    TDualStreamBoxInfo  tDVBInfo;
 *[消息方向]
 *    MtSevice => MtMP 
 */_event ( ev_CodecDualVidBoxCfgCmd )
  _body ( TDualStreamBoxInfo, 1 )
  _ev_end

/***********************<< 配置双流盒指示 >>************************	
 *[消息体]		 
 *    TDualStreamBoxInfo  tDVBInfo;
 *[消息方向]
 *    MtMP => MtService 
 */_event ( ev_CodecDualVidBoxCfgInd )
  _body ( TDualStreamBoxInfo, 1 )
  _ev_end

/***********************<< 注销双流盒 >>************************	
 *[消息体]		 
 *   无 
 *[消息方向]
 *    MtSevice => MtMP 
 */_event ( ev_CodecDualVidBoxUnRegCmd )
  _ev_end
 
/***********************<< 请求编解码器统计信息 >>************************	
 *[消息体]		 
 *   无 
 *[消息方向]
 *    MtSevice => MtMP 
 */_event ( ev_CodecPackStatCmd )
  _ev_end

/***********************<< 编解码器统计信息指示 >>************************	
 *[消息体]		 
 *  +TCodecPackStat[emCodecComponentEnd]  //统计信息结构体
 *[消息方向]
 *    MtMP => MtService 
 */_event ( ev_CodecPackStatInd )
  _body ( TCodecPackStat, emCodecComponentEnd )
  _ev_end  

/***********************<< 双流盒上线指示 >>************************	
 *[消息体]		 
 * 无  
 *[消息方向]
 *    MtMP => MtService 
 */_event ( ev_CodecDualVidBoxOnlineInd )
  _ev_end
  
/***********************<< 双流盒挂断指示 >>************************	
 *[消息体]		 
 * 无  
 *[消息方向]
 *    MtMP => MtService 
 */_event ( ev_CodecDualVidBoxOfflineInd )
  _ev_end
  
/***********************<< 流媒体指示 >>************************	
 *[消息体]		 
 * + u8[3] payload type   第一路第二路视频和音频的载荷类型
 * + u8[3] dynamic ploady 第一路第二路视频和音频的动态载荷类型 
 * + TEncryptKey[3]       第一路第二路视频和音频的密钥
 *[消息方向]
 *    MtMP => MtService 
 */_event ( ev_StreamMediaParamInd )
  _body ( u8, 3 )
  _body ( u8, 3 )
  _body ( TEncryptKey, 3 )
  _ev_end

  
/***********************<< 开始和停止预览 >>************************	
 *[消息体]		 
 * +BOOL  TRUE: 开始预览; FALSE :停止预览
 *[消息方向]
 *    MtMP => MtService 
 */ _event ( ev_CodecPreviewCmd )
  _body ( BOOL, 1  )
  _ev_end

/***********************<< 台标融入码流 >>************************	
 *[消息体]		 
 * +BOOL
 * +TSymboPoint
 * +s8[MT_MAX_NAME_LEN] 
 *[消息方向]
 *    MtMP => MtService 
 */ _event ( ev_CodecSetLogoCmd )
   _body  ( BOOL, 1  )
   _body  ( TSymboPoint ,1 )
   _body  ( s8   ,MT_MAX_NAME_LEN )
   _ev_end


/***********************<< DSP 待机命令 >>************************	
 *[消息体]		 
 * 
 *[消息方向]
 *    MtService = > MtMP
 */ _event ( ev_CodecDspSleepCmd )
    _ev_end

/***********************<< DSP 唤醒命令 >>************************	
 *[消息体]		 
 * 
 *[消息方向]
 *    MtService = > MtMP
 */ _event ( ev_CodecDspWakeupCmd )
    _ev_end


/***********************<< MtMP DSP 状态指示 >>************************	
 *[消息体]		 
 *  + BOOL     TRUE =待机 FALSE =运行
 *[消息方向]
 *     MtMP = > MtService
 */ _event ( ev_CodecDspSleepStateInd )
    _body(   BOOL   , 1          )
    _ev_end


/***********************<< 设置编解码TOS值   >>************************	
 *[消息体]		 
 *  + u8        音频
 *  + u8        视频
 *  + u8        数据
 *[消息方向]
 *     MtMP = > MtService
 */ _event ( ev_CodecTosCmd      )
    _body(   u8     , 1          )//音频
	_body(   u8     , 1          )//视频
	_body(   u8     , 1          )//数据
    _ev_end

/***********************<< 设置单流时Vga播放本地   >>************************	
 *[消息体]		 
 * + BOOL         TRUE=显示本地 FALSE＝显示远端
 *[消息方向]
 *     MtMP = > MtService
 */ _event ( ev_CodecVgaShowLocalCmd      )
    _body(   BOOL     , 1          )
    _ev_end

/***********************<< 设置画中画  >>************************	
 *[消息体]		 
 * + EmPiPMode   
 *[消息方向]
 *      MtService = > MtMP
 */ _event ( ev_CodecSetPIPCmd      )
    _body ( EmPiPMode ,1 )
    _ev_end


/***********************<< 请求音频功率 >>************************	
 *[消息体]			 
 *  无
 *[消息方向]
 *    MtService => MtMP
 */_event(  ev_CodecApplyAudioPowerCmd  )
   _ev_end

/***********************<< 音频功率指示 >>************************	
 *[消息体]			 
 *  u32  输入音频功率
 *  u32  输输出音频功率
 *[消息方向]
 *    MtService <= MtMP
 */_event(  ev_CodecAudioPowerInd  )
   _body ( u32    ,1          )
   _body ( u32    ,1          )
   _ev_end


/***********************<< 视频参数调整  >>************************	
 *[消息体]			 
 *    +EmVideoType
 *    +TImageAdjustParam
 *[消息方向]
 *    MtService => MtMP
 */_event(  ev_CodecImageAdjustParamCmd  )
   _body( EmVideoType       ,1           )
   _body( TImageAdjustParam ,1           )
   _ev_end


/**********************<< Pcmt 选择文件视频源  >>********************	
 *[消息体]			 
 * + u32   //文件名长度
 * + s8    //文件名内容
 *[消息方向]
 *    PCMT UI  = > MtMP
 */_event( ev_CodecPcmtVidSourceCmd  )
   _body ( u32  ,1) //文件名长度
   _body ( s8 , MT_MAX_FULLFILENAME_LEN ) 
   _ev_end
  
/**********************<< Pcmt 选择文件视频源指示  >>********************	
 *[消息体]			 
 * + u32   //文件名长度
 * + s8    //文件名内容
 *[消息方向]
 *    PCMT UI  = > MtMP
 */_event( ev_CodecPcmtVidSourceInd  )
   _body ( u32  ,1) //文件名长度
   _body ( s8 , MT_MAX_FULLFILENAME_LEN ) 
   _ev_end
   
/**********************<< 设置获取音频功率回调指针  >>********************	
 *[消息体]	
 * + EmCodecComponent = emAudioEncoder/emAudioDecoder
 * + u32
 * + u32
 *[消息方向]
 *    PCMT UI  = > MtMP
 */_event( ev_CodecSetAudioPowerCB  )
   _body (EmCodecComponent, 1)
   _body ( u32  ,1) //
   _body ( u32  ,1) //
   _ev_end



/***********************<< 更新流媒体参数  >>************************	
 *[消息体]
 * + BOOL  bForwardLocal //是否转发本地   
 *[消息方向]
 *      MtMP = > MtMP
 */ _event ( ev_CodecUpdateStreamMediaParamCmd      )
    _body ( BOOL ,1 )
    _ev_end

/***********************<< 更新监控参数  >>************************	
 *[消息体]
 * +  EmSite   
 *[消息方向]
 *      MtMP = > MtMP
 */ _event ( ev_CodecUpdateMonitorParamCmd )
    _body ( EmSite ,1 )
    _ev_end

////////////////////////////////////
/***********************<< DVB注册请求 >>********************** 
 *[消息体]
* 无
 *[消息方向]
 *  MtMp  ->DVB
 */_event( ev_CodecDVBRegisterReq )
   _ev_end

/***********************<< DVB注册确认 >>********************** 
 *[消息体]
 * BOOL TRUE/FALSE
 *[消息方向]
 *  MtMp  <-DVB
 */_event( ev_CodecDVBRegisterAck )
   _body( BOOL, 1 )
   _ev_end

/***********************<< DVB注销请求 >>********************** 
 *[消息体]
 * 无
 *[消息方向]
 *  MtMp  ->DVB
 */_event( ev_CodecDVBUnRegisterReq )
   _ev_end

/***********************<< DVB注销确认 >>********************** 
 *[消息体]
 * BOOL  TRUE/FALSE 
 *[消息方向]
 *  MtMp  <-DVB
 */_event( ev_CodecDVBUnRegisterAck )
   _body ( BOOL, 1 )
   _ev_end

/***********************<< 双流盒解码器接收 >>********************** 
 *[消息体]
 *  + EmDVBComponent, 
*   + BOOL TRUE / 开始 FALSE / 停止
*   + TLocalNetParam
 *[消息方向]
 *  MtMp  ->DVB
 */_event( ev_DVBVideoDecoderRcvCmd )
   _body ( EmDVBComponent , 1 )
  _body ( BOOL, 1 )
  _body ( TDVBLocalNetParam, 1 )
   _ev_end

/***********************<< 双流盒解码器解码 >>********************** 
 *[消息体]
 *  + EmDVBComponent, 
*   + BOOL TRUE / 开始 FALSE / 停止
 *[消息方向]
 *  MtMp  ->DVB
 */_event( ev_DVBVideoDecoderDecCmd )
   _body ( EmDVBComponent , 1 )
  _body ( BOOL, 1 )
   _ev_end

/***********************<< 设置双流盒动态载荷 >>********************** 
 *[消息体]
 *  + EmDVBComponent
 *  + u8 ：动态载荷值
 *  + u8 : 载荷类型 
 *[消息方向]
 *  MtMp  ->DVB
 */_event( ev_DVBDynamicPayloadCmd )
   _body ( EmDVBComponent, 1    )
   _body ( u8    , 1    )
   _body ( u8    , 1    )
   _ev_end
   
/***********************<< 双流盒解码器播放 >>********************** 
 *[消息体]
 *  + EmDVBComponent, 
*   + BOOL TRUE / 开始 FALSE / 停止
 *[消息方向]
 *  MtMp  ->DVB
 */_event( ev_DVBVideoDecoderPlyCmd )
   _body ( EmDVBComponent , 1 )
  _body ( BOOL, 1 )
   _ev_end

/***********************<< 设置双流盒密钥 >>********************** 
 *[消息体]
 *  + EmDVBComponent,
 *  + TEncryptKey ：密钥
 *[消息方向]
 *  MtMp  ->DVB
 */_event( ev_DVBEncryptKeyCmd )
   _body ( EmDVBComponent, 1    )
   _body ( TEncryptKey    , 1    )
   _ev_end   

/***********************<< 设置双流盒编码参数 >>********************** 
 *[消息体]
 *  + TDVBVideoEncParam
 *[消息方向]
 *  MtMp  ->DVB
 */_event( ev_DVBVideoEncodeParamCmd )
  _body ( TDVBVideoEncParam    , 1    )
   _ev_end

/***********************<< 双流盒编码器编码 >>********************** 
 *[消息体]
 *  + BOOL = TRUE：开始编码/FALSE：停止编码
 *[消息方向]
 *  MtMp  ->DVB
 */_event( ev_DVBVideoEncodeCmd )
   _body ( BOOL    , 1    )
   _ev_end

/***********************<< 双流盒编码器发送 >>********************** 
 *[消息体]
 *  + BOOL = TRUE：开始发送/FALSE：停止发送
 *[消息方向]
 *  MtMp  ->DVB
 */_event( ev_DVBVideoSendCmd )
   _body ( BOOL    , 1    )  
   _ev_end

/***********************<< 双流盒编码器采集 >>********************** 
 *[消息体]
 *  + BOOL = TRUE：开始采集/FALSE：停止采集
 *[消息方向]
 *  MtMp  ->DVB
 */_event( ev_DVBVideoCapCmd )
   _body ( BOOL    , 1    )
   _ev_end
   
/***********************<< 设置双流盒编码器发送地址 >>********************** 
 *[消息体]
 *  + TDVBNetSndParam //发送地址 
 *[消息方向]
 *  MtMp  ->DVB
 */_event( ev_DVBSetVideoSendAddrCmd )
   _body ( TDVBNetSndParam, 1 )
   _ev_end

/***********************<< 发送关键帧 >>********************** 
 *[消息体]
* 无
 *[消息方向]
 *  MtMp  ->DVB
 */_event( ev_DVBSndKeyFrame)
   _ev_end
   
 
 /***********************<< 开始PC双流命令  >>************************
 *[消息体] 
 *  +EmMtVideoPort 
 *  +BOOL             TRUE=开始 FALSE =停止
 *  +TIPTransAddr   
 *[消息方向]
 *
 *  mtc =>  MtService 
 */_event(  ev_PCDualVideoCmd   )
   _body ( EmMtVideoPort   , 1 )//视频端口
   _body ( BOOL            , 1 )//TRUE=开始 FALSE =停止
   _body ( TIPTransAddr    , 1 ) //编码器本地接收的rtcp地址和端口
   _ev_end

/***********************<< PC双流指示  >>************************
 *[消息体] 
 *  +EmMtVideoPort 
 *  +BOOL             TRUE=开始 FALSE =停止
 *[消息方向]
 *
 *    MtService <=> ui/mtc
 */_event(  ev_PCDualVideoInd   )
   _body ( EmMtVideoPort   , 1 ) //视频端口
   _body ( BOOL            , 1 ) //TRUE=开始 FALSE =停止
   _body ( BOOL            , 1 ) //TRUE=成功 FALSE= 失败
   _ev_end

/***********************<< 设置自动画中画  >>************************
 *[消息体] 
 *  +BOOL             TRUE/ FALSE
 *[消息方向]
 *
 *    MtService => mtmp
 */_event(  ev_CodecAutoPIPCmd   )
   _body ( BOOL            , 1 )//TRUE=自动画中画,FALSE =否
   _ev_end


/***********************<< 设置丢包时降低码率配置  >>************************
 *[消息体] 
 *  +TBitRateAdjust  //
 *[消息方向]
 *
 *    MtService => mtmp
 */_event(  ev_CodecRateAdjustCmd   )
   _body ( TBitRateAdjust      , 1 )//丢包时降低码率配置
   _ev_end
   
/***********************<< 视频源能力指示 >>**********************	
 *[消息体]			 
 * EmVideoType : emPriomVideo/emSecondVideo 
 * EmMtVidPort[MT_MAXNUM_VIDSOURCE] : 所支持的视频源
 *[消息方向]
 * MtMP -> MtService
 * MtService-> MtUI 
 */_event( ev_CodecVidSourceCapInd )
   _body ( EmVideoType , 1 )  
   _body ( EmMtVideoPort,  MT_MAXNUM_VIDSOURCE )
   _ev_end

/***********************<< 丢包降低码率请求  >>************************
 *[消息体]
 *  +u8   //丢包时码率降低的百分比
 *[消息方向]
 *    mtmp->mtservice
 *    
 */_event(  ev_CodecRequireRateAdjustCmd   )
   _body ( u16      , 1 ) //丢包时码率降低的百分比
   _ev_end

/***********************<< 升速的请求  >>************************
 *[消息体]
 *  +u8   //升速的百分比
 *[消息方向]
 *    mtmp->mtservice
 *    
 */_event(  ev_CodecRequireIncreaseRateAdjustCmd   )
   _body ( u16      , 1 ) //要求升速的百分比
   _ev_end

/***********************<< 升速的配置  >>************************
 *[消息体]
 *  +BOOL  //是否启用升速
 *  +u8    //升速比   
 *  +u16   //检测时间隔
 *  +u16   //丢帧数
 *
 *[消息方向]
 *    mtmp->mtservice
 *    
 */_event(  ev_CodecIncreaseRateAdjustCmd   )
   _body ( BOOL       , 1 ) //要求升速的百分比
   _body ( u8, 1 )
   _body ( u16, 1 )
   _body ( u16, 1 )
   _ev_end

   
/**********************<< mt 请求双流盒编码统计信息  >>********************	
 *[消息体]
 * 无
 *[消息方向]
 *    Mtmp  = > dvb
 */_event( ev_DVBEncStatisReq  )
   _ev_end

/**********************<< 双流盒编码统计信息回馈  >>********************	
 *[消息体]
 * TCodecPackStat  //统计信息
 *[消息方向]
 *    Mtmp  = > dvb
 */_event( ev_DVBEncStatisAck  )
   _body( TCodecPackStat , 1 )
   _ev_end
 
	
/**********************<< 设置第二路视频的输出端口(vga/video)  >>********************	
 *[消息体]
 * BOOL bVga ; TRUE:输出到vga/FALSE: 输出到video
 *[消息方向]
 *    mtservice  = > mtmp
 */_event( ev_CodecSetSVOutput  )
   _body( BOOL ,  1 )
   _ev_end
   
/**********************<< mtmp错误码指示  >>********************	
 *[消息体]
 * +u16 //mtmp错误码值
 *[消息方向]
 *    mtmp  = > mtservice
 */_event( ev_CodecErrorCodeInd )
   _body( u16, 1 )
   _ev_end
   
/**********************<< Mic增益控制  >>********************	
 *[消息体]
 * +u8 //mic增益值
 *[消息方向]
 *    ui  = > mtservice
 */_event( ev_MicAdjustCmd )
   _body( u8, 1 )
   _ev_end

/**********************<< Mic增益控制指示  >>********************	
 *[消息体]
 * +u8  //mic增益值
 *[消息方向]
 *    mtservice  = > ui/mtc
 */_event( ev_MicAdjustInd )
   _body( u8, 1 )
   _ev_end
  
/**********************<< Osd重画指示  >>********************	
 *[消息体]
 * 无
 *[消息方向]
 *   mtmp  = > mtservice
 */_event( ev_OsdUpdateInd )
   _ev_end

/***********************<<  调整通道分辨率 >>********************** 
 *[消息体]
//*    +EmCodecComponent
//*    +EmVideoResolution
 *[消息方向]
 *  channelservice  -> mtmp
 */_event( ev_CodecAdjustVideoResCmd )
   _body( EmCodecComponent,  1 )
   _body( EmVideoResolution, 1 )   
   _ev_end

/**********************<< 视频信号输入指示  >>********************	
 *[消息体]
 * BOOL         //第一路视频是否有信号
 * BOOL         //第二路视频是否有信号
 *[消息方向]
 *   mtmp  ->  mtservice
 *   mtservice ->ui /mtc
 */_event( ev_CodecVideoInputInd )
   _body( BOOL , 1 )
   _body( BOOL, 1 )
   _ev_end
   
/***********************<<  设置解码器组播接收地址 >>********************** 
 *[消息体]
//*    +EmCodecComponent, 
//*    +u32  ip
//*    +u16  port
 *[消息方向]
 *  h323service -> mtservice(channelservie)
 */_event( ev_CodecSetMultiCastAddrCmd)
   _body( EmCodecComponent, 1 )
   _body( u32_ip,  1 )  
   _body( u16,  1 ) 
   _ev_end

/**********************<< Ntsc和Pal制式改变指示  >>********************	
 *[消息体]
 * 无
 *[消息方向]
 *   mtmp  = > mtservice
 */_event( ev_NtscPalChangeInd )
   _ev_end

/**********************<< 无视频源时，发送静态图片  >>********************	
 *[消息体]
 * BOOL  //是否无视频源时，发送静态图片
 *[消息方向]
 *   mtmp  = > mtservice
 */_event( ev_CodecNoVideoShowPicCmd )
  _body( BOOL, 1 )
   _ev_end   


/**********************<< 将码率降低一个比率  >>********************	
 *[消息体]
 * u8  //如果为95，则将码率调整到原来的95%
 * u8  //由于ip头的冗余，需要再将编码码率降低一个值
       //默认是90%，即降为原来的90%，网络带宽则设为原来的值,编码码率降
 *[消息方向]
 *   mtservice  = > mtmp
 */_event( ev_CodecBitRateDecsRatioCmd )
  _body( u8, 1 )
  _body( u8, 1 ) 
   _ev_end  
 
/**********************<< 扬声器测试命令  >>********************	
 *[消息体]
 * + EmMtLoudspeakerVal 1
 *[消息方向]
 *   mtui|mtc  => mtservice => mtmp
 */_event( ev_LoudspeakerTestCmd )
   _body( EmMtLoudspeakerVal, 1 )
   _ev_end

/**********************<< 扬声器测试指示  >>********************	
 *[消息体]
 * + EmMtLoudspeakerVal 1
 *[消息方向]
 *   mtui|mtc  <= mtservice
 */_event( ev_LoudspeakerTestInd )
   _body( EmMtLoudspeakerVal, 1 )
   _ev_end
   
/**********************<< 振铃声命令  >>********************	
 *[消息体]
 * + EmMtLoudspeakerVal 1  音量
 * + BOOL               1  开启/关闭
 *[消息方向]
 *   mtui|mtc => mtservice => mtmp_t2
 */_event( ev_PlayRingCmd )
   _body( EmMtLoudspeakerVal, 1 )
   _body( BOOL,               1 )
   _ev_end
   
/**********************<< 播放电话按钮声命令  >>********************	
 *[消息体]
 * + s8                 1  开启/关闭
 *[消息方向]
 *   mtui|mtc => mtservice => mtmp_t2
 */_event( ev_PlayDialRingCmd )
   _body( s8,                 1 )
   _ev_end

/**********************<< 丢包指示  >>********************	
 *[消息体]
 * + BOOL         1
 *[消息方向]
 *   mtui|mtc  <= mtservice
 */_event( ev_PackLostInd )
   _body( BOOL,          1)  
   _ev_end
   

/**********************<< 设置音频解码的缓冲  >>********************	
 *[消息体]
 * + u32,  dwStartRcvBuf
 * + u32,  dwFastRcvBuf
 *[消息方向]
 *   mtui|mtc  <= mtservice
 */_event( ev_CodecAudioDecodeBufCfg )
   _body( u32,     1 )  
   _body( u32,     1 )
   _ev_end
   

/**********************<< 设置mic增益命令  >>********************	
 *[消息体]
 * +u8     mic增益值
 *[消息方向]
 *   mtui|mtc  <= mtservice
 */_event( ev_CodecSetMicCmd )
   _body( u8,     1 )  
   _ev_end

/**********************<< 获取mic增益命令  >>********************	
 *[消息体]

 *[消息方向]
 *   mtservice => codec
 */_event( ev_CodecGetMicCmd )
   _ev_end

/**********************<< 获取mic增益指示  >>********************	
 *[消息体]
 * +u8     mic增益值
 *[消息方向]
 *   mtservice <= codec
 */_event( ev_CodecGetMicInd )
   _body( u8,     1 )  
   _ev_end


/**********************<< OSD是否打开指示  >>********************	
 *[消息体]
 * +BOOL    是否打开
 *[消息方向]
 *   ui => codec
 */_event( ev_CodecOsdOpenInd )
   _body( BOOL,     1 )  
   _ev_end
   
/**********************<< T2CPU异常指示  >>********************	
 *[消息体]
 * +u32    CPU Type
 *[消息方向]
 *   codec => mtservice => ui
 */_event( ev_CodecCpuResetInd )
   _body( u32,     1 )  
   _ev_end

/**********************<< 设置pcmt视频解码器重绘  >>********************	
 *[消息体]
 * +EmCodecComponet 
 *[消息方向]
 *  mtservice => mtmp
 */_event( ev_CodecRedrawLastFrameCmd )
   _body( EmCodecComponent,     1 )  
   _ev_end
   
/**********************<< T2 Osd显示位置指示  >>********************	
 *[消息体]
 * +u32    map Id
 *[消息方向]
 *   codec => mtservice => ui
 */_event( ev_CodecSetOsdMapIdInd )
   _body( u32,     1 )  
   _ev_end


/**********************<< FEC算法设置命令  >>********************	
 *[消息体]
 * +u8    FEC算法类型
 *[消息方向]
 *    mtservice => codec
 */_event( ev_CodecSetFecCapacityTypeCmd )
   _body( u8,     1 )  
   _ev_end


/**********************<< FEC算法设置指示  >>********************	
 *[消息体]
 * +u8    FEC算法类型
 *[消息方向]
 *    mtservice => UI
 */_event( ev_CodecSetFecCapacityTypeInd )
   _body( u8,     1 )  
   _ev_end
   
/**********************<< G7221C字节序翻转  >>********************	
 *[消息体]
 * BOOL bReverse
 *[消息方向]
 *   mtservice => codec
 */_event( ev_CodecSetReverseG7221cCmd )
   _body( BOOL,     1 )  
   _ev_end

  /*********************<< 远端回环测试命令 >>************************
 *[消息体]	
 * + char[]
 *
 *[消息方向]
 *    Service  = > UI
 *
*/_event(  ev_RemoteLoopBackTestCmd )
  _body( EmVideoType,   1 )
  _body( BOOL, 1 )
  _ev_end


  /*********************<< 远端回环测试状态指示  >>************************
 *[消息体]	
 * + char[]
 *
 *[消息方向]
 *    Service  = > UI
 *
*/_event(  ev_RemoteLoopBackTestInd )
  _body( EmVideoType,   1 )
  _body( BOOL, 1 )
  _ev_end

#ifndef _MakeMtEventDescription_
   _ev_segment_end(codec)
};
#endif

#endif
