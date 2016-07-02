#ifndef FILE_TRANS_H
#define FILE_TRANS_H
#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmFileTrans
{
#endif
/***********************************************************/
/*                                                         */
/*                 文件传输消息                            */
/*    CMtMsg 消息                                             */
/***********************************************************/

   _ev_segment( file_trans )

/***********************<< MTC请求发送文件 >>********************	
 *[消息体]			 
 *   + TFileTransfer
 *[消息方向]
 *    MTC => UE
 */_event( ev_FTSendReq                     )
   _body ( TFileTransfer    ,1              )
   _ev_end


/***********************<< MTC请求发送文件确认 >>********************	
 *[消息体]			 
 *   + TFileTransfer
 *[消息方向]
 *    MTC => UE
 */_event(  ev_FTSendACK  )
   _body ( TFileTransfer    ,1    )
   _ev_end


 /***********************<< MTC请求发送文件拒绝 >>********************	
 *[消息体]			 
 *   + TFileTransfer
 *[消息方向]
 *    MTC => UE
 */_event(  ev_FTSendNACK  )
   _body ( TFileTransfer    ,1    )
   _ev_end


 /***********************<< MTC开始发送文件 >>********************	
 *[消息体]			 
 *   + TFileTransfer
 *   + 文件内容
 *[消息方向]
 *    MTC => UE
 */_event(  ev_FTSendFileCmd  )
   _body ( TFileTransfer    ,1         )
   _body ( s8               ,REMAIN    )
   _ev_end



 /***********************<< MTC开始发送文件应答 >>********************	
 *[消息体]			 
 *   + TFileTransfer
 *[消息方向]
 *    MTC => UE
 */_event(  ev_FTSendFileRsp  )
   _body ( TFileTransfer    ,1         )
   _ev_end


	
 /***********************<< MTC发送文件完毕通知 >>********************	
 *[消息体]			 
 *   + TFileTransfer
 *[消息方向]
 *    MTC => UE
 */_event(  ev_FTSendCompleteInd  )
   _body ( TFileTransfer    ,1         )
   _ev_end

	
 /***********************<< MTC发送文件完毕报告 >>********************	
 *[消息体]			 
 *   + TFileTransfer
 *[消息方向]
 *    MTC => UE
 */_event(  ev_FTSendCompleteRsp  )
   _body ( TFileTransfer    ,1         )
   _ev_end

	
 /***********************<< MTC请求发送文件 >>********************	
 *[消息体]			 
 *   + TFileTransfer
 *[消息方向]
 *    MTC => UE
 */_event(  ev_FTTimeout  )
   _body ( TFileTransfer    ,1         )
   _ev_end

/**********************<< 设置横幅参数  >>********************	
 *[消息体]	
 *TBannerInfo info //横幅参数
 *[消息方向]
 *    MTC => UE
 */_event( ev_BannerInfoCmd)
   _body ( TBannerInfo ,1)
   _ev_end    
/**********************<< 横幅参数通知  >>********************	
 *[消息体]	
 *TBannerInfo info //横幅参数
 *[消息方向]
 *    MTC => UE
 */_event( ev_BannerInfoInd)
   _body ( TBannerInfo ,1)
   _ev_end       
/**********************<< 台标更新通知  >>********************	
 *[消息体]	
 * NULL
 *[消息方向]
 *    UI <= UE
 */_event( ev_UpdateLabel)
   _ev_end       
/**********************<< 横幅更新通知  >>********************	
 *[消息体]	
 *NULL
 *[消息方向]
 *    UI <= UE
 */_event( ev_UpdateBanner)
   _ev_end     
#ifndef _MakeMtEventDescription_
   _ev_segment_end(file_trans)
};
#endif	

#endif

