#ifndef AUTOUPGRADE_EVENT_H
#define AUTOUPGRADE_EVENT_H

#include "eventcomm.h"

#ifndef _MakeMtEventDescription_

enum EmAutoUpgrade
{
#endif
/***********************************************************/
/*                                                         */
/*                 自动升级消息                            */
/*                                                         */
/***********************************************************/

    _ev_segment( autoupgrade )
 
/***********************<< 连接服务器命令 >>********************	
 *[消息体]			 
 *   +无          //终端信息
 *   +u8  bySrc  //指示当前的升级消息来自mtc,或者osd, xjx_080111
 *        bySrc = 1, 来自osd; bySrc = 2; 来自mtc
 *   
 *[消息方向]
 *   ui  => mtservice 
 */_event(  ev_UpgradeConnectSrvCmd   )
  _body( u8 , 1 )
   _ev_end
   
/***********************<< 取消连接服务器命令 >>********************	
 *[消息体]			 
 *   +无         //终端信息
 *   
 *[消息方向]
 *    ui => mtservice
 */_event(  ev_UpgradeDisconnectSrvCmd   )
 _ev_end

 
/***********************<< 连接服务器状态指示 >>********************	
 *[消息体]			 
 *   +BOOL           //连接状态,TRUE:成功；FALSE:失败
 *   
 *[消息方向]
 *    ui <=  mtservice
 */_event(  ev_UpgradeConnectSrvStatusInd   )
   _body( BOOL, 1 )
   _ev_end

/***********************<< 升级服务器上最新版本信息指示 >>********************	
 *[消息体]			 
 *  TVerInfo   //最新的版本信息结构         
 *   
 *[消息方向]
 *    ui <=  mtservice
 */_event(  ev_UpgradeNewVersionInfoInd   )
   _body( TVerInfo, 1 )
   _ev_end
   

/***********************<< 开始升级命令 >>********************	
 *[消息体]			 
 *           
 *   
 *[消息方向]
 *    ui =>  mtservice
 */_event(  ev_UpgradeStartupgradeCmd   )
   _body( u8, 1 )
   _ev_end


/***********************<< 取消升级命令 >>********************	
 *[消息体]			 
 *           
 *   
 *[消息方向]
 *    ui =>  mtservice
 */_event(  ev_UpgradeCancelupgradeCmd   )
   _ev_end


/***********************<< 文件下载状态指示 >>********************	
 *[消息体]	
 * +BOOL     //TRUE=文件下载成功；FALSE=下载失败  
 * +u16      //总包
 * +u16      //当前包序号
 *[消息方向]
 *    ui =>  mtservice
 */_event(  ev_UpgradeFileDownloadInfoInd   )
  _body( BOOL, 1 )
  _body( u16, 1 )
  _body( u16, 1 )
   _ev_end

  
/***********************<< 文件下载完毕指示 >>********************	
 *[消息体]
 * +无
 *[消息方向]
 *    ui =>  mtservice
 */_event(  ev_UpgradeFileDownloadCompletedInd   ) 
   _ev_end


/***********************<< 更新系统文件状态指示 >>********************	
 *[消息体]
 * +BOOL     //更新文件是否成功
 * +u8       //升级失败的原因
 *[消息方向]
 *    ui =>  mtservice
 */_event(  ev_UpgradeSysFileUpdateStatusInd   ) 
  _body( BOOL, 1 )
  _body( u8, 1 )   
   _ev_end
 
   
/***********************<< 升级服务器地址配置 >>********************	
 *[消息体]
 * +BOOL    //是否为默认地址，还是用户配置的服务器地址
 * +s8      //地址

 *[消息方向]
 *    ui =>  mtservice
 */_event(  ev_UpgradeSrvAddrCfgCmd   )
  _body( BOOL, 1 )
  _body( s8, REMAIN )
  _ev_end
   
/***********************<< 升级服务器地址配置指示 >>********************	
 *[消息体]
 * +BOOL    //是否为默认地址，还是用户配置的服务器地址
 * +s8      //总包

 *[消息方向]
 *    ui =>  mtservice
 */_event(  ev_UpgradeSrvAddrCfgInd   )
  _body( BOOL, 1 )
  _body( s8, REMAIN )
  _ev_end
  
/***********************<<  升级服务器断链，终端请求断点续传  >>******************
   *[消息体]	
   * + BOOL  //是否断点续传	 
   *[消息方向]
   *
   */_event( ev_UpgradeResumeConnectCmd )
   _body( BOOL, 1 )
   _ev_end
   
/***********************<<  升级服务器链路,断链指示  >>******************
   *[消息体]			 
   *[消息方向]
   *
   */_event( ev_UpgradeDisconnectInd )
   _ev_end

/***********************<< 升级服务器上最新版本信息, 列表指示 >>********************	
 *[消息体]			 
 *  TVerInfo   //最新的版本信息        
 *   
 *[消息方向]
 *    ui <=  mtservice
 */_event(  ev_UpgradeNewVersionInfoListInd   )
   _body( u8 , 1 ) 
   _body( TVerInfo, REMAIN )
   _ev_end
   

/***********************<< 断开升级服务器连接的指示 >>********************	
 *[消息体]			 
 *   +无         //该消息用于清除终端的锁界面的消息
 *            
 *[消息方向]
 *    ui/mtc <= mtservice
 */_event(  ev_UpgradeDisconnectSrvInd   )
 _ev_end

/***********************<< osd连接服务器的指示 >>********************	
 *[消息体]			 
 *    //终端信息,该条消息紧跟在ev_UpgradeConnectSrvCmd后，该消息用于
 *  *u8    //1: 升级来者osd; 2: 升级来自mtc
 *   
 *[消息方向]
 *   ui  => mtservice 
 */_event(  ev_UpgradeConnectSrvInd   )
   _body( u8 , 1 )
   _ev_end


#ifndef _MakeMtEventDescription_
   _ev_segment_end( autoupgrade )
};
#endif

#endif