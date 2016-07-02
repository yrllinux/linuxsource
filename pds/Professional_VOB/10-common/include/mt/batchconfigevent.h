
#ifndef BATCHCONFIG_EVENT_H
#define BATCHCONFIG_EVENT_H

#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmBatchConfig
{
#endif
/***********************************************************/
/*                                                         */
/*               配置文件批处理                            */
/*                                                         */
/***********************************************************/

    _ev_segment( batchconfig )


//mod by xujinxing,2007-01-06
//osp消息，不用加消息体
/***********************<< MTB请求登陆MT >>********************	
 *[消息体]			 
 *   NULL 
 *[消息方向]
 *    MTB => UE
 */_event(  ev_MTBLoginReq  )
   _ev_end 


/***********************<< MT确认MTB登陆请求消息 >>********************	
 *[消息体]			 
 *   NULL
 *[消息方向]
 *    UE => MTB
 */_event(  ev_MTBLoginAck  )
   _ev_end 
      
   
/***********************<< MT拒绝MTB登陆请求消息 >>********************	
 *[消息体]			 
 *   NULL 
 *[消息方向]
 *     UE => MTB
 */_event(  ev_MTBLoginNack )
   _ev_end 
  

/***********************<< MTB更新MT配置文件请求 >>********************	
 *[消息体]			 
 *   + TfileTransfer  
 *   + 配置文件内容
 *[消息方向]
 *     MTB =>MTService
 */_event(  ev_MTBUpdateCfgFileReq           )
   _body (  TFileTransfer         ,  1       ) 
   _body (  s8                    ,  REMAIN  )
   _ev_end    


/***********************<< MT确认MTB更新MT配置文件请求 >>********************	
 *[消息体]			 
 *   + TfileTransfer  
 *[消息方向]
 *     MTService => MTB
 */_event(  ev_MTBUpdateCfgFileAck      )
   _body (  TFileTransfer         ,  1  ) 
   _ev_end    
  
 
/***********************<< MT拒绝MTB更新MT配置文件请求 >>********************	
 *[消息体]			 
 *   + TfileTransfer  
 *[消息方向]
 *     MTService => MTB
 */_event(  ev_MTBUpdateCfgFileNack     )
   _body (  TFileTransfer         ,  1  ) 
   _ev_end     


/***********************<< MTB下载MT配置文件请求 >>********************	
 *[消息体]			 
 * +TfileTransfer //文件传输头信息
 *[消息方向]
 *     MTB => MTService
 */_event(  ev_MTBDownloadCfgFileReq      )
   _body (  TFileTransfer           ,  1  )
   _ev_end


/***********************<< MT确认MTB下载配置文件请求 >>********************	
 *[消息体]			 
 * +TfileTransfer //文件传输头信息
 *[消息方向]
 *     MTService => MTB
 */_event(  ev_MTBDownloadCfgFileAck      )
   _body (  TFileTransfer           ,  1  )
   _ev_end


/***********************<< MT拒绝MTB下载配置文件请求 >>********************	
 *[消息体]			 
 * +TfileTransfer //文件传输头信息
 *[消息方向]
 *     MTService => MTB
 */_event(  ev_MTBDownloadCfgFileNack     )
   _body (  TFileTransfer           ,  1  )
   _ev_end
   

/***********************<< MT向MTB传送配置文件 >>********************	
 *[消息体]			 
 * + TfileTransfer //文件传输头信息
 * + 配置文件内容
 *[消息方向]
 *     MTService => MTB
 *
 */_event(  ev_MTBDownloadCfgFileNotify        )
   _body (  TFileTransfer           ,  1       )
   _body (  s8                      ,  REMAIN  )
   _ev_end

/***********************************************************/
/*                                                         */
/*               终端硬件自动化测试消息                    */
/*                                                         */
/***********************************************************/

/***********************<< Mt Auto Test UI请求登陆MT >>********************	
 *[消息体]			 
 *   NULL 
 *[消息方向]
 *    Mt Auto Test UI => UE
 */_event(  ev_MTAutoTestLoginReq  )
   _ev_end 

/***********************<< 确认Mt Auto Test UI登陆请求消息 >>********************	
 *[消息体]			 
 *   NULL 
 *[消息方向]
 *    UE => Mt Auto Test UI
 */_event(  ev_MTAutoTestLoginAck  )
   _ev_end

/***********************<< 拒绝Mt Auto Test UI登陆请求消息 >>********************	
 *[消息体]			 
 *   NULL 
 *[消息方向]
 *    UE => Mt Auto Test UI
 */_event(  ev_MTAutoTestLoginNack  )
   _ev_end

/***********************<< Mt Auto Test UI测试命令 >>********************	
 *[消息体]			 
 *   TMtAutoTestMsg  //自动化测试头信息
 *[消息方向]
 *    Mt Auto Test UI => UE
 */_event(  ev_MTAutoTestCmd     )
   _ev_end

/***********************<< Mt Auto Test UI 测试结果通知 >>********************	
 *[消息体]			 
 *   TMtAutoTestMsg //自动化测试头信息
 *[消息方向]
 *    UE => Mt Auto Test UI
 */_event(  ev_MTAutoTestNotify  )
   _ev_end

/***********************<< Mt Auto Test UI 设置默认值请求 >>********************	
 *[消息体]			 
 *   无
 *[消息方向]
 *    Mt Auto Test UI => UE
 */_event(  ev_MTAutoTestSetDefaultCmd  )
   _ev_end

/***********************<< 恢复默认值结果通知 >>********************	
 *[消息体]			 
 *   无
 *[消息方向]
 *    UE => Mt Auto Test UI
 */_event(  ev_MTAutoTestSetDefaultNotify  )
   _ev_end

/***********************<< 环回测试消除OSD菜单消息 >>********************	
 *[消息体]			 
 *   无
 *[消息方向]
 *    UE => UE
 */_event(  ev_MTAutoTestCloseAllWindowCmd  )
   _ev_end

/***********************<< 文件加载消息 >>********************	
 *[消息体]			 
 *   无
 *[消息方向]
 *    UE => UE
 */_event(  ev_MTAutoTestLoadFileCmd  )
   _ev_end   

/***********************<< 文件加载成功消息 >>********************	
 *[消息体]			 
 *   无
 *[消息方向]
 *    UE => UE
 */_event(  ev_MTAutoTestLoadFileInd  )
   _ev_end   

#ifndef _MakeMtEventDescription_
   _ev_segment_end( batchconfig )
};
#endif

#endif
