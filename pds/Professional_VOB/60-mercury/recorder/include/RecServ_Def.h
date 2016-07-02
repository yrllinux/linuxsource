/*****************************************************************************
   模块名      : Recorder
   文件名      : RecServ_Def.h
   相关文件    : 
   文件实现功能: Recorder　Server相关内容定义
   作者        : 
   版本        : V1.0  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2010/08/26  1.0         刘 旭         创建
******************************************************************************/

#ifndef RECSERVDEF_H
#define RECSERVDEF_H

#include "mcustruct.h"
#include "evmcueqp.h"
#include "evrec.h"
#include "RecError.h"

#ifdef WIN32
#pragma comment( lib, "ws2_32.lib" ) 
#pragma pack( push )
#pragma pack( 1 )
#define window( x )	x
#else
#include <netinet/in.h>
#define window( x )
#endif


// 任务返回值定义
typedef u32 RET_TYPE;				// 任务返回值类型定义
const RET_TYPE RET_PASSED = 0;		// 没有接受此任务,pass掉
const RET_TYPE RET_DONE   = 1;		// 接受此任务并处理完成
const RET_TYPE RET_FAILED = 2;		// 接受此任务,但处理失败


// RecServer的AppId。MT通过此AppId与服务器建立连接
#define     AID_REC_SERVER                AID_MCU_BGN + 49    // APP ID of Rec Server

// 错误号ID，录像机服务器通知MT服务器忙
#define ERR_REC_SERVER_BUSY					ERR_REC_BGN + 30			  //输入文件名过长
// 错误号ID，终端请求非法
#define ERR_REC_SERVER_INVALID				ERR_REC_BGN + 31			  //终端请求非法
// 错误号ID，匿名异常
#define ERR_REC_SERVER_INVALID_PARAM		ERR_REC_BGN + 32			  //参数错误
// 错误号ID，匿名异常
#define ERR_REC_SERVER_ERR					ERR_REC_BGN + 33			  //匿名异常

//终端向录像机服务器发出注册请求，MT->REC SERVER，消息体为TMtRegReq结构
OSPEVENT( MT_RECSERV_REG_REQ,			    EV_MCUREC_BGN  + 109);
//录像机服务器接受应答，REC SERVER->MT，消息体为消息体为TMtRegAck
OSPEVENT( RECSERV_MT_REG_ACK,				EV_MCUREC_BGN + 110 );
//录像机服务器拒绝应答，REC SERVER->MT，无消息体
OSPEVENT( RECSERV_MT_REG_NACK,				EV_MCUREC_BGN + 111 );

//[暂时不用]终端向录像机服务器请求注销，MT->REC SERVER，消息体无
OSPEVENT( MT_RECSERV_UNREG_CMD,			    EV_MCUREC_BGN  + 112);
//[暂时不用]录像机服务器主动断开与终端的连接
OSPEVENT( RECSERV_MT_EXIT_CMD,				EV_MCUREC_BGN + 115 );

//MT指定时间内没响应, 主动断开
OSPEVENT( RECSERV_REC_MT_DISC,				EV_MCUREC_BGN + 116 );
//与终端之间失去连接
OSPEVENT( EV_MT_DISCONNECTED,		    	EV_MCUREC_BGN + 117 );

//打印录像机服务器的状态信息
OSPEVENT( EV_RECSERV_SHOW_STATUS,	    	EV_MCUREC_BGN + 131 );

// 定义回复客户端的结构体
typedef TPeriEqpRegReq TMtRegAck;
// 定义客户端请求注册的结构体
typedef TPeriEqpRegAck TMtRegReq;


// 定义节点信息
typedef struct tagNodeInfo
{
    u32 m_dwNodeId;									// 节点号
    u32 m_dwAppId;									// Instance号，由MAKEID(appid，instanceid)生成
}TNodeInfo, *PTNodeInfo;


// 终端请求注册的信息
struct TMTRegInfo
{
    TMtRegReq m_tMtRegReq;
    s8        m_aliase[MAXLEN_RECORD_NAME + 1];
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// 录像文件文件名的最大长度（绝对路径）
#define MAX_REC_FILE_NAME 128
// 终端别名的最大长度
#define MAX_H323ALIASNAME_LEN 32

// 定义终端的默认别名
#define MT_DEFAULT_NAME  "localMt"

// 终端请求录像机服务器开始录像的msg中content的内容
typedef struct tagTRecStartParam
{
public:
    //TEqp			TEqp;
    // 新加的
    TRecRtcpBack    tRecRtcpBack;						// 重传信息
    // 新加的
    u8              byNeedPrs;							//是否重传
    TRecStartPara	tRecStartPara;
    TMediaEncrypt	tPriVideoEncrypt;
    TMediaEncrypt	tSecVideoEncrypt;
    TMediaEncrypt	tAudVideoEncrypt;
    TDoublePayload	tPriPayload;
    TDoublePayload	tSecPayload;
    TDoublePayload	tAudPayload;
    s8				achFileName[MAX_REC_FILE_NAME+1];
    TCapSupportEx	tCapSupportEx;
  	s8				achAliase[MAX_H323ALIASNAME_LEN+1];
public:
    tagTRecStartParam()
    {
        memset( this->achFileName, 0, MAX_REC_FILE_NAME+1 );
        memset( this->achAliase, 0, MAX_H323ALIASNAME_LEN+1 );
    };
}TRecStartParam
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// 定义Nack时通知终端的错误体结构
typedef struct tagErrInfo
{
    u16 m_wErrCode;									// 错误号
}TErrorInfo
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// Recserv模块的log输出函数
extern void RecServLog( s8* pszFmt, ... );

// Recserv模块的优先级log输出函数
extern void RecServLevelLog( const u8& byLevel, s8* pszFmt, ... );

// log优先级定义。值越小，优先级越高
const u8 RECSERV_LEVEL_LOG_ERROR    = 0;			// 错误信息，错误发生时输出
const u8 RECSERV_LEVEL_LOG_WARNING  = 4;			// 警告信息，警告发生时输出
const u8 RECSERV_LEVEL_LOG_COMMON   = 6;			// 普通信息，输出一般的状态信息
const u8 RECSERV_LEVEL_LOG_BELOW    = 8;			// 低级信息，为方便调试，输出一些非常低级的消息，如msg内容等


#endif