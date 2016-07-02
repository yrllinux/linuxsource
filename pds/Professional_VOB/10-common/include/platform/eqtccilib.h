/*****************************************************************************
模块名      : Cci
文件名      : eqtCciLib.h
相关文件    : eqtCciLib.c
文件实现功能:  This file contains some constants and routine declarations used by upper lever.
作者        : 向飞
版本        : V1.0  Copyright(C) 2003-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
13jan2003   01a         向飞        written. 
01mar2003, 01b         张方明      增加接口.	
******************************************************************************/

#ifndef __INCeqtCciLibh
#define __INCeqtCciLibh

#include "eqtbrdlib.h"
/*#include "eti_board.h"*/

#ifndef API

#if defined(__cplusplus) && defined(_MSC_VER)
#define API extern "C" __declspec(dllexport)
#endif /* defined(__cplusplus) && defined(_MSC_VER) */

#if !defined(__cplusplus) && defined(_MSC_VER)
#define API __declspec(dllexport)
#endif /* !defined(__cplusplus) && defined(_MSC_VER) */

#if defined(__cplusplus) && !defined(_MSC_VER)
#define API extern "C"
#endif /* defined(__cplusplus) && defined(_MSC_VER) */

#if !defined(__cplusplus) && !defined(_MSC_VER)
#define API
#endif /* defined(__cplusplus) && defined(_MSC_VER) */

#endif /* API */

/* 变长cci队列设计宏定义,还原时不定义该宏即可 */
#define CCI_USE_DYNAMIC_BUF

#define CCI_VERSION                    "Ver 5.0  build at 2009-09-09"
#define VER_MTCCILib            ( const char * )"HOSTCCILib 40.01.04.06.090909"
#define CCI_VER_FLAG            4

#ifdef  CCI_USE_DYNAMIC_BUF
 #undef  CCI_VER_FLAG
 #define CCI_VER_FLAG 	                5
 #define CCI_UPMSG_MAX_LEN       (512<<10)   /* 最大允许传输的消息长度 */
#endif

/* CCI通信状态相关宏定义 */
#define CCI_SUCCESS                     0    /* CCI通信连接建立成功 */
#define CCI_PEER_CLOSED                 1    /* CCI通信中MAP设备断开 */
#define CCI_NOT_CONNECTED               2    /* CCI通信没有建链 */
#define CCI_SMEM_CORRUPT                3    /* CCI共享内存区被破坏 */
#define CCI_LENGTH_ERROR                4    /* CCI通信信息长度错误 */
#define CCI_QUEUE_FULL                  5    /* CCI消息队列已满 */
#define CCI_MSG_LOST                    6    /* CCI消息丢失 */
#define CCI_FAILURE                    -1    /* CCI操作失败 */


/* CCI通信消息属性的宏定义 */
#define DN_MSGQ_MAX                 1024      /* 最大下行消息队列数 */
#define ERR_FLAG_MAX_NUM            16        /* MAP错误记录的最大个数 */

#define IS_UPMSGQ                   0         /* 上行消息队列编号 */
#define IS_DNMSGQ                   1         /* 下行消息队列编号 */

#define BUF_IS_NORMAL               0         /* 消息队列中可用BUF数未超出设定值 */
#define BUF_WILL_FULL               1         /* 消息队列中可用BUF数超出设定值 */

#define CCI_MSGQ_FREE_RATE          20        /* 空闲消息队列的百分数，低于CCI_MSGQ_FREE_RATE%告警 */

/* 类型定义 */
typedef void * HMsg;    /* handle to CCI host state */
typedef void ( *TMsgCallBack )(HMsg hMsg, BYTE *pbyMsg, WORD32 dwLen,   \
                               BYTE *pbyContext, WORD16 wState);
typedef void ( *TMsgQBufChkCallBack )(BYTE byMapId, BYTE byUpDown,\
                                      BYTE byQueueId, BYTE byStatus); /* added by zfm 2003/7/15 05:42PM */

typedef void ( *TCciMsgPrtFunc )(char *pbyString);

/* 用户传入的消息队列参数结构定义 */
typedef struct{
        WORD32 dwMaxMsgLen;    /*每条消息的最大长度(按BYTE计)，缺省128KB最大长度 */
        WORD32 dwMaxMsgs;      /* 队列中的buffer总数, 缺省为8 */
} TcciMsgQParamDes;

typedef struct{
        WORD32 dwDnMsgQs;    /*下行消息队列数 */
        TcciMsgQParamDes tCciDnMsgQParam[DN_MSGQ_MAX];   /*下行消息队列参数 */
        TcciMsgQParamDes tCciUpMsgQParam;                /*上行消息队列参数 */
} TCciMsgQParam;

/* 用户传入的发送消息参数结构定义 */
typedef struct{
        const BYTE* pbyMsgHead;     /* 用户消息头指针 */
        const BYTE* pbyMsg;         /* 用户消息指针 */
        WORD32 dwMsgHeadLen;        /* 用户消息头长度 */
        WORD32 dwMsgLen;            /* 用户消息长度 */
        WORD32 byMsgType;           /* 消息类型 */
} TcciMsgParam;

/* 全局函数声明 */
API HMsg    EqtMsgOpen ( HBoard hBoard, TCciMsgQParam *ptCciMsgQParam ,  \
                     TMsgCallBack tMsgCallBack, BYTE *pbyContext);    
API SWORD32 EqtMsgSnd(HMsg hMsg, WORD32 dwChannel, TcciMsgParam *pTcciMsgParam);
API SWORD32 EqtMsgClose(HMsg hMsg);
API HBoard  EqtMsgGetBrdHandle(HMsg hMsg);
API SWORD32 EqtMsgGetBrdNo(HMsg hMsg);
API SWORD32 EqtMsgGetMapErrLog(HMsg hMsg, WORD32 dwErrId, WORD32 *pdwMapErrLog);
API SWORD32 EqtMsgGetMapHeartBeat(HMsg hMsg, WORD32 *pdwMapCnt);

API void CciPrintf(char *szFormat,...);
API void EqtMapPrtEnable(BYTE byBrdNo);     /* 开启map消息打印 */
API void EqtMapPrtDisable(BYTE byBrdNo);    /* 关闭map消息打印 */

/* 模拟丢包dwRecvOrSend参数宏定义，dwDropRate以1/10000为单位 */
#define CCI_RECV_DROP                    0    /* 设置接收丢包 */
#define CCI_SEND_DROP                    1    /* 设置发送丢包 */
API SWORD32 EqtMsgSetDropRate( HMsg hMsg, WORD32 dwRecvOrSend, WORD32 dwDropRate );

/* cci的buf将满告警回调注册函数 */
API void EqtMsgQBufChkCallBackReg(TMsgQBufChkCallBack ptMsgQBufChkCallBack);

/* cci的打印回调注册函数,不注册时底层用printf打印 */
API void EqtMsgPrtFuncReg(TCciMsgPrtFunc ptCciMsgPrtFunc);
API void EqtMsgPrt(char *szFormat, ...);

/* cci的buf状态查询函数 */
API SWORD32 EqtCciDnMsgQNumGet(BYTE byBrdNo);   /* 查询当前map[byBrdNo]的下行队列实际使用的队列数 */
API SWORD32 EqtCciMsgQBufAlarmGet(BYTE byBrdNo, BYTE byUpOrDnMsgQ, WORD32 dwMsgQNo);

API void CciInfoShow(SWORD32 byBrdNo);
API void CciStatShow(BYTE byBrdNo);
API SWORD32 EqtCciLoopBack
(
    BYTE byBrdNo, 
    WORD32 wSendTimes,          /* 发送次数 */
    WORD32 dwSendIntval,        /* 发送时间间隔 */
    WORD32 dwPacketsPerIntval    /* 每时间间隔发送包数 */
);
API SWORD32 EqtCciTestTogether
(
    BYTE  byTestBrdNum,
    WORD32 wSendTimes,
    WORD32 dwSendIntval,        /* 发送时间间隔 */
    WORD32 dwPacketsPerIntval    /* 每时间间隔发送包数 */

);

API void EqtCciPrtTest();
API SWORD32 EqtCciHostSendTest(BYTE byBrdNo, WORD32 dwSndTimes);
API SWORD32 EqtCciMapSendTest(BYTE byBrdNo, WORD32 dwSndTimes);
API SWORD32 EqtCciMapPrtTest(BYTE byBrdNo, WORD32 bySndTimes);
API void EqtIncludeTestFun(void);
API void hostcciver(void);

#ifdef __cplusplus 
//} 
#endif /* __cplusplus */

#endif /* __INCeqtCciLibh */
