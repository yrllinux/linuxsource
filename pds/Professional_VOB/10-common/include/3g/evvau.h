/*****************************************************************************
   模块名      : EVVAU
   文件名      : evvau.h
   相关文件    : 
   文件实现功能: VAU的消息接口定义
   作者        : 胡昌威
   版本        : V1.0  Copyright(C) 2006-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2006/08/21  1.0         胡昌威        创建
******************************************************************************/
#ifndef __EVVAU_H_
#define __EVVAU_H_

#include "osp.h"
#include "eventid.h"

#define ADD_EVENT_DESC(x) OspAddEventDesc(#x, x)
      
//KSS内部消息定义
OSPEVENT( KSS_SETUP_NOTIFY,		    EV_RTSP_BGN + 1 );               //建立Session
OSPEVENT( KSS_PLAY_NOTIFY,		    EV_RTSP_BGN + 2 );               //开始播放
OSPEVENT( KSS_PAUSE_NOTIFY,		    EV_RTSP_BGN + 3 );              //暂停
OSPEVENT( KSS_RECORD_NOTIFY,	    EV_RTSP_BGN + 4 );              //录像
OSPEVENT( KSS_TEARDOWN_NOTIFY,	    EV_RTSP_BGN + 5 );              //拆除Session
OSPEVENT( KSS_DESCRIBE_NOTIFY,	    EV_RTSP_BGN + 6 );              //收到Describe
OSPEVENT( KSS_OPTIONS_NOTIFY,	    EV_RTSP_BGN + 7 );              //收到Options

//OSPEVENT( EV_KSS_CONNECT_VAU,	    EV_RTSP_BGN + 8 );              //连接VAU
//OSPEVENT( EV_KSS_REGISTER_VAU,	    EV_RTSP_BGN + 9 );              //注册VAU

OSPEVENT( EV_KSS_SESSION_CHECK,	    EV_RTSP_BGN + 10 );              //检查会话是否存货

//VAU与KSS之间的消息定义
//KSS发给VAU的注册请求,消息体为结构TKss
OSPEVENT( VAU_KSS_REG_REQ,		    EV_RTSP_BGN + 11 );          
//KSS接受VAU的注册请求,消息体为结构TKss
OSPEVENT( KSS_VAU_REG_ACK,		    EV_RTSP_BGN + 12 ); 
//KSS拒绝VAU的注册请求,消息体为结构TKss
OSPEVENT( KSS_VAU_REG_NACK,		    EV_RTSP_BGN + 13 ); 

//确认是否注册
OSPEVENT( VAU_KSS_REG_CMF,			EV_RTSP_BGN + 14 );
//已经注册
OSPEVENT( KSS_VAU_HAS_REG,			EV_RTSP_BGN + 15 );
//没有注册
OSPEVENT( KSS_VAU_NO_REG,			EV_RTSP_BGN + 16 );

//KSS与VAU之间的消息定义
#define    EV_VAU_RTSP_BGN          (EV_RTSP_BGN+30)                  //起始消息号

//KSS发给VAU的建立会话请求,
//消息体为结构TSetupParam+u32(请求URL的长度)+char *(请求URL)+u32(请求参数的长度)+char *(请求参数)
OSPEVENT( KSS_VAU_SETUP_REQ,		EV_VAU_RTSP_BGN + 1 );          
//VAU接受KSS的建立会话请求
//消息体为结构TSetupParam+u32(请求URL的长度)+char *(请求URL)+u32(请求参数的长度)+char *(请求参数)
OSPEVENT( VAU_KSS_SETUP_ACK,		EV_VAU_RTSP_BGN + 2 ); 
//VAU拒绝KSS的建立会话请求
//消息体为结构TSetupParam+u32(请求URL的长度)+char *(请求URL)+u32(请求参数的长度)+char *(请求参数)
OSPEVENT( VAU_KSS_SETUP_NACK,		EV_VAU_RTSP_BGN + 3 ); 

//KSS发给VAU的开始播放请求
//消息体为double(开始播放时间)
OSPEVENT( KSS_VAU_PLAY_REQ,		    EV_VAU_RTSP_BGN + 4 );          
//VAU接受KSS的开始播放请求
OSPEVENT( VAU_KSS_PLAY_ACK,		    EV_VAU_RTSP_BGN + 5 ); 
//VAU拒绝KSS的开始播放请求
OSPEVENT( VAU_KSS_PLAY_NACK,		EV_VAU_RTSP_BGN + 6 );  

//KSS发给VAU的暂停请求
OSPEVENT( KSS_VAU_PAUSE_REQ,		EV_VAU_RTSP_BGN + 7 );          
//VAU接受KSS的暂停请求
OSPEVENT( VAU_KSS_PAUSE_ACK,		EV_VAU_RTSP_BGN + 8 ); 
//VAU拒绝KSS的暂停请求
OSPEVENT( VAU_KSS_PAUSE_NACK,		EV_VAU_RTSP_BGN + 9 );            

//KSS发给VAU的录像请求
OSPEVENT( KSS_VAU_RECORD_REQ,		EV_VAU_RTSP_BGN + 10 );          
//VAU接受KSS的录像请求
OSPEVENT( VAU_KSS_RECORD_ACK,		EV_VAU_RTSP_BGN + 11 ); 
//VAU拒绝KSS的录像请求
OSPEVENT( VAU_KSS_RECORD_NACK,		EV_VAU_RTSP_BGN + 12 ); 

//KSS发给VAU的拆除会话请求
OSPEVENT( KSS_VAU_TEARDOWN_REQ,		EV_VAU_RTSP_BGN + 13 );          
//VAU接受KSS的拆除会话请求
OSPEVENT( VAU_KSS_TEARDOWN_ACK,		EV_VAU_RTSP_BGN + 14 ); 
//VAU拒绝KSS的拆除会话请求
OSPEVENT( VAU_KSS_TEARDOWN_NACK,	EV_VAU_RTSP_BGN + 15 ); 

//KSS发给VAU的得到描述请求
OSPEVENT( KSS_VAU_DESCRIBE_REQ,		EV_VAU_RTSP_BGN + 16 );          
//VAU接受KSS的得到描述请求
OSPEVENT( VAU_KSS_DESCRIBE_ACK,		EV_VAU_RTSP_BGN + 17 ); 
//VAU拒绝KSS的得到描述请求
OSPEVENT( VAU_KSS_DESCRIBE_NACK,	EV_VAU_RTSP_BGN + 18 );


OSPEVENT( VAU_KSS_ADD_TRANS_ACK,  	EV_VAU_RTSP_BGN + 20 );
OSPEVENT( VAU_KSS_ADD_TRANS_NACK,  	EV_VAU_RTSP_BGN + 21 );  

//VAU内部消息定义
#define    EV_VAU_BGN          EV_VAU_RTSP_BGN+30 

OSPEVENT( EV_VAU_CONNECT_KSS,	    EV_VAU_BGN + 1 );              //连接VAU
OSPEVENT( EV_VAU_REGISTER_KSS,	    EV_VAU_BGN + 2 );              //注册VAU
OSPEVENT( EV_VAU_DISCONNECT_KSS,	EV_VAU_BGN + 3 );              //连接VAU
OSPEVENT( EV_VAU_LOGINOUT_KSS,		EV_VAU_BGN + 4 );              //连接VAU

#endif
