#ifndef VOD_EVENT_H
#define VOD_EVENT_H
#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmVOD
{
#endif 

_ev_segment(vod )//VOD消息

/**********************<< 登陆VODServer请求 >>**************	
 *[消息体]			 
 *  + TVODUserInfo
 *[消息方向]
 *
 *   UI  => MtService => VODServer
 *   
 */_event( ev_VODLoginCmd )
 _body( TVODUserInfo, 1 )
 _ev_end



 /**********************<< 退出VODServer请求 >>**************	
 *[消息体]			 
 *  无
 *[消息方向]
 *
 *   UI  => MtService => VODServer
 *   
 */_event(  ev_VODLogoutCmd  )
   _ev_end




/**********************<< 获取文件列表请求 >>**************	
 *[消息体]			 
 *  + s8[ VOD_MAX_FILE_NAME_LENGTH ] // 目录名 
 * [ Comment ]
 *	// ServiceDir + DirName = > VODServer
 *[消息方向]
 *
 *   UI  => MtService = > VODServer
 *   
 */_event(  ev_VODGetFileListCmd  )
	_body( s8, VOD_MAX_FILE_NAME_LENGTH )
   _ev_end

/**********************<< 获取文件列表指示 >>**************	
 *[消息体]			 
 *  + TVODFile[]
 *[消息方向]
 *
 *   UI  <= MtService <= VODServer
 *   
 */_event(  ev_VODGetFileListInd  )
   _body( TVODFile ,REMAIN )
   _ev_end



/**********************<< 获取文件信息请求 >>**************	
 *[消息体]			 
 *  + s8[ VOD_MAX_FILE_NAME_LENGTH ] // 文件名
 * [ Comment ]
 *	
 *[消息方向]
 *
 *   UI  => MtService => VODServer
 *   
 */_event(  ev_VODGetFileInfoCmd  )
   _body( s8, VOD_MAX_FILE_NAME_LENGTH )
   _ev_end


/**********************<< 获取文件信息指示 >>**************	
 *[消息体]			 
 *  + TVODFile
 *[消息方向]
 *
 *   UI  <= MtService <= VODServer
 *   
 */_event(  ev_VODGetFileInfoInd  )
  _body( TVODFile, 1 )
  _ev_end


/**********************<< 文件播放请求 >>**************	
 *[消息体]			 
 *	u8[VOD_MAX_FILE_NAME_LENGTH] -播放文件名
 *[消息方向]
 *
 *   UI  => MtService => VODServer
 *   
 */_event(  ev_VODPlayFileCmd  )
	_body(  u8, VOD_MAX_FILE_NAME_LENGTH )
   _ev_end

/**********************<< 播放恢复请求 >>**************	
 *[消息体]			 
 *  无
 *[消息方向]
 *
 *   UI  => MtService => VODServer
 *   
 */_event(  ev_VODResumePlayCmd  )
   _ev_end


/**********************<< 播放跳转请求 >>**************	
 *[消息体]			 
 *  u32 / 播放时间起点
 *[消息方向]
 *
 *   UI  => MtService => VODServer
 *   
 */_event(  ev_VODRandomPlayCmd  )
	_body( u32, 1 )
   _ev_end

/**********************<< 播放暂停请求 >>**************	
 *[消息体]			 
 *  无
 *[消息方向]
 *
 *   UI  => MtService => VODServer
 *   
 */_event(  ev_VODPausePlayCmd  )
   _ev_end

/**********************<< 文件停止播放请求 >>**************	
 *[消息体]			 
 *  无
 *[消息方向]
 *
 *   UI  => MtService => VODServer
 *   
 */_event(  ev_VODStopPlayFileCmd  )
   _ev_end

/**********************<< VOD客户端状态通知 >>**************	
 *[消息体]			 
 *  +EmVODState
 *[消息方向]
 *
 *   UI  => MtService => VODServer
 *   
 */_event(  ev_VODStateInd  )
   _body( EmVODState ,1 )
   _ev_end

/**********************<< 播放信息通知指示 >>**************	
 *[消息体]			 
 *  + u32 dwDuration         //播放总时长，毫秒
 *  + u32 dwPlayTime         //已播放时间，毫秒
 *[消息方向]
 *
 *   UI  <= MtService <= VODServer
 *   
 */_event(  ev_VODPlayInfoNotificationInd  )
  _body( u32, 1 )
  _body( u32, 1 )
  _ev_end
 

#ifndef _MakeMtEventDescription_
   _ev_segment_end(vod)
};
#endif  
#endif

