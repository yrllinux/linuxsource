#ifndef _HINT_MSG_HEADER_
#define _HINT_MSG_HEADER_

#define MAX_HINTMSG_LEN    64

#ifndef _MakeHintDescription_

#define _hint_segment_fail_begin emHintMsgBegin = 0,

#define _hint_no( emHintId ) emHintId,
#define _hint_body( language, description )
#define _hint_end

#define _hint_segment_sucess_begin emHintMsgHalf = 3000,

#define _hint_segment_end emHintMsgEnd

enum EmHintMsgNo
{

#else

#define _hint_segment_fail_begin
#define _hint_segment_sucess_begin
#define _hint_segment_end

#endif 

/************************************************************************/
/*                            失败的提示宏                              */
/************************************************************************/
 _hint_segment_fail_begin

/***********************<< 发起双流失败 >>*************************/	
_hint_no( emFailtoSendDual )
_hint_body( emEnglish, "peer not support the dual by current fmt and res" )
_hint_body( emChineseSB, "对端不支持当前格式和分辨率下的双流" )
_hint_end

/***********************<< 接收双流失败 >>*************************/	
_hint_no( emFailtoRecvDual )
_hint_body( emEnglish, "not support peer's dual fmt or res" )
_hint_body( emChineseSB, "能力级不支持对端发起的双流" )
_hint_end

/***********************<< vod登录失败 >>*************************/	
_hint_no( emFailtoVodLogin )
_hint_body( emEnglish, "VOD login failled" )
_hint_body( emChineseSB, "VOD 登录失败" )
_hint_end

/***********************<< DHCP获取地址超时 >>*************************/	
_hint_no( emDHCPTimeOut )
_hint_body( emEnglish, "DHCP get ip time out" )
_hint_body( emChineseSB, "获取DHCP地址超时" )
_hint_end

/***********************<< DHCP地址租约到期 >>*************************/	
_hint_no( emDHCPOverdue )
_hint_body( emEnglish, "DHCP ip overdue" )
_hint_body( emChineseSB, "DHCP地址租约到期" )
_hint_end


/***********************<< PPPOE身份验证错误 >>*************************/	
_hint_no( emPPPoEAuthorError )
_hint_body( emEnglish, "PPPoE authentication error" )
_hint_body( emChineseSB, "PPPOE身份验证错误" )
_hint_end

/***********************<< PPPOE找不到服务商 >>*************************/	
_hint_no( emPPPoEAgentError )
_hint_body( emEnglish, "PPPoE agent not exist" )
_hint_body( emChineseSB, "PPPOE找不到ISP服务商" )
_hint_end

/***********************<< PPPOE拨号超时 >>*************************/	
_hint_no( emPPPoETimeOutError )
_hint_body( emEnglish, "PPPoE dial time out" )
_hint_body( emChineseSB, "PPPOE拨号超时" )
_hint_end

/***********************<< PPPOE断链 >>*************************/	
_hint_no( emPPPoELinkDownError )
_hint_body( emEnglish, "PPPoE link down" )
_hint_body( emChineseSB, "PPPOE断链" )
_hint_end

/***********************<< PPPOE其他错误 >>*************************/	
_hint_no( emPPPoEGeneralError )
_hint_body( emEnglish, "PPPoE General down" )
_hint_body( emChineseSB, "PPPOE其他错误" )
_hint_end

/***********************<< 发起双流失败 >>*************************/	
_hint_no( emFailtoSendDualAsBeject )
_hint_body( emEnglish, "peer reject to receive dual video" )
_hint_body( emChineseSB, "与对端的媒体能力不匹配, 发起双流失败" )
_hint_end

/***********************<< LDAP服务器链接失败 >>*************************/	
_hint_no( emFailtoConnectLdapSvr )
_hint_body( emEnglish, "Failed to connect to LDAP server" )
_hint_body( emChineseSB, "LDAP服务器链接失败或断链" )
_hint_end

/***********************<< LDAP所查条目不存在 >>*************************/	
_hint_no( emLdapEntryNotExist )
_hint_body( emEnglish, "The inquired entry doesn't exist" )
_hint_body( emChineseSB, "所查询的LDAP条目不存在" )
_hint_end

/**************<< 呼叫码率无效导致的MC被动扩展失败 >>**************/	
_hint_no( emCallRateNotValidForMc )
_hint_body( emEnglish, "Call rate isn't valid for Mc" )
_hint_body( emChineseSB, "呼叫码率过高,拒绝MC扩展" )
_hint_end

/***********************<< 1080p/i时发送或接收双流失败提示 >>*************************/	
_hint_no( em1080pDualFail )
_hint_body( emEnglish, "Dual failed when current display 1080p/i!" )
_hint_body( emChineseSB, "当前会议分辨率为1080p/i,双流失败!" )
_hint_end

/***********************<< 720p/i时发送或接收双流失败提示 >>*************************/	
_hint_no( em720pDualFail )
_hint_body( emEnglish, "Dual failed when resolution 720p,>30fps!" )
_hint_body( emChineseSB, "当前会议分辨率为720p超过30帧/秒,双流失败!" )
_hint_end

/***********************<< 1080时只允许发送VGA双流失败提示 >>*************************/	
_hint_no( em1080pNotVGADualFail )
_hint_body( emEnglish, "Only vga or PC dual when priom video 1080p!" )
_hint_body( emChineseSB, "当前会议分辨率为1080p,实时双流失败!" )
_hint_end

/***********************<< 1080源时不能发送实时双流失败提示 >>*************************/	
_hint_no( em1080pSrcRealTimeDualFail )
_hint_body( emEnglish, "Only vga or PC dual when dual video source 1080p/i!" )
_hint_body( emChineseSB, "当前第二路视频源为1080p/i,实时双流失败!" )
_hint_end

/************************************************************************/
/*                            成功的提示宏                              */
/************************************************************************/

 _hint_segment_sucess_begin
/***********************<< 发起双流成功 >>*************************/	
_hint_no( emSuccedetoSendDual )
_hint_body( emEnglish, "succeed to send dual video stream" )
_hint_body( emChineseSB, "发起双流成功" )
_hint_end

/***********************<< 接收双流成功 >>*************************/	
_hint_no( emSuccedetoRecvDual )
_hint_body( emEnglish, "succeed to receive dual video stream" )
_hint_body( emChineseSB, "接收双流成功" )
_hint_end

/***********************<< 告警VGA/Video切换重启 >>*************************/	
_hint_no( emWillSwitchVgaVideoRestart )
_hint_body( emEnglish, "Persist pushing for 10s to switch VGA/Video" )
_hint_body( emChineseSB, "继续按该键十秒后切换VGA/Video!" )
_hint_end

/***********************<< PPPOE拨号成功 >>*************************/	
_hint_no( emPPPoELinkUp )
_hint_body( emEnglish, "PPPoE link up" )
_hint_body( emChineseSB, "PPPOE拨号成功" )
_hint_end

/***********************<< PCMT没有注册GK,没有会议功能 >>*************************/	
_hint_no( emPCMTNoGKNoConf )
_hint_body( emEnglish, "PCMT Unregister GK, can't make/join conference!" )
_hint_body( emChineseSB, "PCMT未注册GK, 不能开会!" )
_hint_end

/***********************<< 提示终端将被MCU轮训到 >>*************************/	
_hint_no( emBePolledNext )
_hint_body( emEnglish, "This Meeting-terminal will be polled by MCU soon!" )
_hint_body( emChineseSB, "本终端即将被MCU轮询到!" )
_hint_end


/***********************<< D1时发送或接收双流失败提示 >>*************************/	
_hint_no( emD1DualFial )
_hint_body( emEnglish, "Dual failed when current display 4CIF!" )
_hint_body( emChineseSB, "当前会议分辨率为4CIF,双流失败!" )
_hint_end




/***********************<< Gk启用但未注册成功，挂断呼叫提示 >>*************************/	
_hint_no( emGkFailForbidCall )
_hint_body( emEnglish, "Forbid call when Gk register failed!" )
_hint_body( emChineseSB, "Gk注册不成功禁止呼叫！" )
_hint_end

_hint_segment_end

#ifndef _MakeHintDescription_
};
#endif 



#undef _hint_no 
#undef _hint_body 
#undef _hint_end 
#undef _hint_segment_fail_begin
#undef _hint_segment_sucess_begin
#undef _hint_segment_end

#endif 


