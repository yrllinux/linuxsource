#ifndef MATRIX_EVENT_H
#define MATRIX_EVENT_H
#include "eventcomm.h"

#ifndef _MakeMtEventDescription_
enum EmMatrix
{
#endif 
_ev_segment( matrix)//外置矩阵消息


/*********************<< 设置视频源信息命令  >>*********************	
 *[消息体]			 
*TVideoSourceInfo
 *[消息方向]
 *
 *  UI=> MtService => MtDevice
 */_event(  ev_SetVideoSourceInfoCmd  )
   _body (  TVideoSourceInfo ,1            ) 
   _ev_end
/*********************<< 设置视频源信息指示  >>*********************	
 *[消息体]			 
*TVideoSourceInfo
 *[消息方向]
 *
 *  UI<= MtService <= MtDevice
 */_event(  ev_VideoSourceInfoInd  )
	_body (  TVideoSourceInfo ,1            ) 
    _ev_end
   
/*********************<< 获取所有视频源信息命令  >>*********************	
 *[消息体]			 
 *无
 *[消息方向]
 *
 *  UI => MtService => MtDevice
 */_event(  ev_GetAllVideoSourceInfoCmd  )
   _ev_end   
/*********************<< 设置所有视频源信息指示  >>*********************	
 *[消息体]			 
*TVideoSourceInfo
 *[消息方向]
 *
 *  UI<= MtService <= MtDevice
 */_event(  ev_AllVideoSourceInfoInd  )
   _body (  TVideoSourceInfo ,REMAIN     ) 
   _ev_end

/*********************<< 扩展视频源能力指示  >>*********************	
 *[消息体]			 
*u8  byExtendVideoCapacity 
 *[消息方向]
 *
 *  UI<= MtService <= MtDevice
 */_event(  ev_ExtendVideoSourceCapacityInd  )
   _body ( u8 ,1           ) 
   _ev_end   
/*********************<< 获取终端支持外部矩阵配置请求  >>*********************	
 *[消息体]			 
 *  无
 *
 *[消息方向]
 *
 *   MtService =>MtDevice
 */_event(  ev_InnerMatrixGetConfigCmd   )
   _ev_end


/*********************<< 终端外部矩阵配置指示  >>*********************	
 *[消息体]			 
 * + EmSerialType  emCtrlType; //控制类型
 * + TIPTransAddr  tIPAddr;    //串口控制时地址
 *
 *[消息方向]
 *
 *   MtService <= MtDevice
 */_event(  ev_InnerMatrixConfigInd   )
   _body (  EmSerialType  ,1             )
   _body (  TIPTransAddr  ,1             )
   _ev_end


/*********************<< 终端外部矩阵配置  >>*********************	
 *[消息体]			 
 * + EmSerialType  emCtrlType; //控制类型
 * + TIPTransAddr  tIPAddr;    //串口控制时地址
 *
 *[消息方向]
 *
 *   MtService => MtDevice
 */_event(  ev_InnerMatrixConfigCmd   )
   _body (  EmSerialType  ,1             )
   _body (  TIPTransAddr  ,1             )
   _ev_end

 
 /*********************<< 获取终端支持外部矩阵配置请求  >>*********************	
 *[消息体]			 
 *  无
 *
 *[消息方向]
 *
 *   MtService =>MtDevice
 */_event(  ev_ExternalMatrixGetConfigCmd   )
   _ev_end


/*********************<< 终端外部矩阵配置指示  >>*********************	
 *[消息体]			 
 * + EmSerialType  emCtrlType; //控制类型
 * + TIPTransAddr  tIPAddr;    //串口控制时地址
 *
 *[消息方向]
 *
 *   MtService <= MtDevice
 */_event(  ev_ExternalMatrixConfigInd   )
   _body (  EmSerialType  ,1             )
   _body (  TIPTransAddr  ,1             )
   _ev_end


/*********************<< 终端外部矩阵配置  >>*********************	
 *[消息体]			 
 * + EmSerialType  emCtrlType; //控制类型
 * + TIPTransAddr  tIPAddr;    //串口控制时地址
 *
 *[消息方向]
 *
 *   MtService => MtDevice
 */_event(  ev_ExternalMatrixConfigCmd   )
   _body (  EmSerialType  ,1             )
   _body (  TIPTransAddr  ,1             )
   _ev_end


/*********************<< 获取矩阵方案所有内容请求  >>*********************	
 *[消息体]		
 *    
 *    
 *[消息方向]
 *
 *   MtService => MtDevice
 */_event(  ev_InnerMatrixGetAllSchemesCmd  )
   _ev_end


 /*********************<< 获取矩阵方案所有内容指示  >>*********************	
 *[消息体]			 
 *    + u8( 方案数 ) + TAVInnerMatrixScheme[ ]
 *[消息方向]
 *
 *   MtService <= MtDevice
 */_event(  ev_InnerMatrixGetAllSchemesInd  )
   _body (  u8  ,1       )
   _body (  TAVInnerMatrixScheme  ,REMAIN      )
   _ev_end


 /*********************<< 获取矩阵方案一个内容请求  >>*********************	
 *[消息体]		
 *    + u8 ( 0 为默认方案 )       方案索引从( 0 - 6 )
 *[消息方向]
 *
 *   MtService => MtDevice
 */_event(  ev_InnerMatrixGetOneSchemeCmd  )
   _body (  u8  ,1       )
   _ev_end


 /*********************<< 获取矩阵方案一个内容指示  >>*********************	
 *[消息体]			 
 *    TAVInnerMatrixScheme
 *[消息方向]
 *
 *   MtService <= MtDevice
 */_event(  ev_InnerMatrixGetOneSchemeInd  )
   _body ( TAVInnerMatrixScheme  ,1       )
   _ev_end


/*********************<< 保存方案  >>*********************	
 *[消息体]			 
 *    +  TAVInnerMatrixScheme
 *	   
 *[消息方向]
 *
 *   MtService => MtDevice
 */_event(  ev_InnerMatrixSaveSchemeCmd   )
   _body ( TAVInnerMatrixScheme  ,1       )
   _ev_end


 /*********************<< 保存方案指示  >>*********************	
 *[消息体]			 
 *    +  TAVInnerMatrixScheme
 *	   
 *[消息方向]
 *
 *   MtService <= MtDevice
 */_event(  ev_InnerMatrixSaveSchemeInd   )
   _body ( TAVInnerMatrixScheme  ,1       )
   _ev_end


/*********************<< 调用指定方案为当前方案  >>*********************	
 *[消息体]			 
 *
 *	  +  u8  方案索引
 *[消息方向]
 *
 *   MtService => MtDevice
 */_event(  ev_InnerMatrixSetCurSchemeCmd   )
   _body ( u8         ,1           )
   _ev_end


 /*********************<< 调用指定方案为当前方案指示  >>*********************	
 *[消息体]			 
 *
 *	  +  u8  方案索引
 *[消息方向]
 *
 *   MtService <= MtDevice
 */_event(  ev_InnerMatrixSetCurSchemeInd   )
   _body ( u8         ,1           )
   _ev_end


 /*********************<< 获取当前方案请求  >>*********************	
 *[消息体]			 
 *    
 *[消息方向]
 *
 *   MtService => MtDevice
 */_event(  ev_InnerMatrixCurSchemeCmd   )
   _ev_end


/*********************<< 获取当前方案请求指示  >>*********************	
 *[消息体]			 
 *    +  u8  方案索引
 *[消息方向]
 *
 *   MtService <= MtDevice
 */_event(  ev_InnerMatrixCurSchemeInd   )
   _body ( u8         ,1           )
   _ev_end


/*********************<< 更换外部矩阵脚本文件  >>*********************	
 *[消息体]			 
 *    +  u8  FileName[ 100 ]
 *[消息方向]
 *
 *   MtService => MtDevice
 */_event(  ev_ExternalMatrixChangeCommandCmd   )
    _body ( u8         ,REMAIN                  )
   _ev_end


/*********************<< 外部矩阵切换连接  >>*********************	
 *[消息体]			 
 *    +  u8  ( 输入端口号)
 *[消息方向]
 *
 *   MtService => MtDevice
 */_event(  ev_ExternalMatrixConnectionCmd   )
   _body ( u8         ,1           )
   _ev_end

/*********************<< 外部矩阵切换连接指示  >>*********************	
 *[消息体]			 
 *    +  u8  ( 输入端口号)
 *[消息方向]
 *
 *   UI / MTC / MCU <=MtService
 */_event(  ev_ExternalMatrixConnectionInd   )
   _body ( u8         ,1           )
   _ev_end


/*********************<< 获取外部矩阵切换连接请求  >>*********************	
 *[消息体]			 
 *    无
 *[消息方向]
 *
 *   MtService => MtDevice
 */_event(  ev_ExternalMatrixGetConnectionCmd   )
   _ev_end

/*********************<< 获取外部矩阵切换连接指示  >>*********************	
 *[消息体]			 
 *    +  u8  ( 输入端口号)
 *[消息方向]
 *
 *   MtService <= MtDevice
 */_event(  ev_ExternalMatrixGetConnectionInd   )
   _body ( u8         ,1           )
   _ev_end


/*********************<< 获取外部矩阵信息请求  >>*********************	
 *[消息体]			 
 *    无
 *[消息方向]
 *
 *   MtService => MtDevice
 */_event(  ev_ExternalMatrixInfoCmd   )
   _ev_end

/*********************<< 获取外部矩阵信息指示  >>*********************	
 *[消息体]			 
 *    TExternalMatrixInfo
 *[消息方向]
 *
 *   MtService <= MtDevice
 */_event(  ev_ExternalMatrixInfoInd   )
   _body (  TExternalMatrixInfo         ,1            ) 
   _ev_end

/*********************<< 外部矩阵端口名保存  >>*********************	
 *[消息体]			 
 *    +u8 ( 0 - 64 / 0输出名，其余输入名)
 *    + s8 [ MT_MAX_AVMATRIX_MODEL_NAME + 1 ]
 * 
 *[消息方向]
 *
 *   MtService => MtDevice
 */_event(  ev_ExternalMatrixSavePortCmd   )
   _body (  u8         ,REMAIN            ) 
   _ev_end

/*********************<< 外部矩阵端口名保存指示  >>*********************	
 *[消息体]			 
 *    +u8 ( 0 - 64 / 0输出名，其余输入名)
 *    + s8 [ MT_MAX_AVMATRIX_MODEL_NAME + 1 ]
 *[消息方向]
 *
 *   UI / MTC / MCU <=MtService
 */_event(  ev_ExternalMatrixSavePortInd   )
   _body (  u8         ,REMAIN            ) 
   _ev_end

/*********************<< 获取外部矩阵端口名  >>*********************	
 *[消息体]			 
 *    无
 *[消息方向]
 *
 *   MtService => MtDevice
 */_event(  ev_ExternalMatrixGetPortCmd   )
   _ev_end

/*********************<< 获取外部矩阵端口名指示  >>*********************	
 *[消息体]			 
 *    s8 [ ( MT_MAX_PORTNAME_LEN + 1 ) * ( 64 + 1 ) ]
 * [ 0 ]: Outport Name / [ 1] to [ 64 ] Inport Name 
 *[消息方向]
 *
 *   MtService <= MtDevice
 */_event(  ev_ExternalMatrixGetPortInd   )
   _body (  u8         ,REMAIN            ) 
   _ev_end
   
//xjx_071221, 高清终端下的视频矩阵消息
 
/*********************<< HD终端,获取矩阵方案所有内容指示  >>*********************	
 *[消息体]			 
 *    + u8( 方案数 ) + THDAVInnerMatrixScheme[ ]
 *[消息方向]
 *
 *   MtService <= MtDevice
 */_event(  ev_HDInnerMatrixGetAllSchemesInd  )
   _body (  u8  ,1       )
   _body (  THDAVInnerMatrixScheme  ,REMAIN      )
   _ev_end

/*********************<< HD终端,获取矩阵方案一个内容指示  >>*********************	
 *[消息体]			 
 *    THDAVInnerMatrixScheme
 *[消息方向]
 *
 *   MtService <= MtDevice
 */_event(  ev_HDInnerMatrixGetOneSchemeInd  )
   _body ( THDAVInnerMatrixScheme  ,1       )
   _ev_end


/*********************<< HD终端,保存方案  >>*********************	
 *[消息体]			 
 *    +  THDAVInnerMatrixScheme
 *	   
 *[消息方向]
 *
 *   MtService => MtDevice
 */_event(  ev_HDInnerMatrixSaveSchemeCmd   )
   _body ( THDAVInnerMatrixScheme  ,1       )
   _ev_end


 /*********************<< HD终端,保存方案指示  >>*********************	
 *[消息体]			 
 *    +  THDAVInnerMatrixScheme
 *	   
 *[消息方向]
 *
 *   MtService <= MtDevice
 */_event(  ev_HDInnerMatrixSaveSchemeInd   )
   _body ( THDAVInnerMatrixScheme  ,1       )
   _ev_end


#ifndef _MakeMtEventDescription_
   _ev_segment_end(matrix)
};
#endif  

#endif

