/*****************************************************************************
模块名      : E1_TRANS_MODULE
文件名      : E1TransLib.h
相关文件    : 
文件实现功能:  E1_TRANS_MODULE 上层接口宏定义和函数的声明.
作者        : 张方明
版本        : V1.0  Copyright(C) 2003-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人     走读人       修改内容
2005/05/25  01a        张方明                  创建	
******************************************************************************/

#ifndef __INC_E1TransLib_H
#define __INC_E1TransLib_H

#ifdef __cplusplus 
extern "C" { 
#endif /* __cplusplus */


/* ================================= 宏定义 ================================ */

/* E1透明传输模块返回值和错误码描述 */
#define E1_TRANS_SUCCESS       (INT32)0                         /* 成功 */

#define E1_TRANS_FAILURE       (INT32)-1                        /* 未知的异常失败 */
#define E1_TRANS_ERRCODE_BASE  (INT32)1                         /* 错误码基值 */
#define E1_TRANS_NOT_INIT      (INT32)(E1_TRANS_ERRCODE_BASE+0) /* 通道未打开 */
#define E1_TRANS_LENGTH_ERROR  (INT32)(E1_TRANS_ERRCODE_BASE+1) /* 数据长度错误 */
#define E1_TRANS_NO_BUF        (INT32)(E1_TRANS_ERRCODE_BASE+2) /* 没有可用的BUF */

/* 参数 wBufLength 范围定义 */
#define E1_TRANS_BUF_MAX_LEN   (UINT32)8000 /* 缓冲区队列中最大BUF的长度,单位字节 */
#define E1_TRANS_BUF_MIN_LEN   (UINT32)800  /* 缓冲区队列中最小BUF的长度,单位字节 */

#define E1_TRANS_CHAN_MAX_NUM  (UINT8)16    /* 最多支持的E1透明传输通道的个数，一
                                               个通道仅对应于一条物理E1线路 */


/* ============================== 结构、类型定义 =========================== */

/* 创建一个通道需要传入的参数结构定义 */
typedef struct{
    UINT32  dwBufNum;           /* 消息队列中buf的总数 */
    UINT16  wBufLength;         /* 消息队列中每个buf的长度,范围如上面的宏定义，且必须是8的整数倍 */
    UINT32  dwTimeSlotMask;     /* 见下面的时晰参数使用说明*/
} TE1TransChanParam;

/********************************************************************************* 
* dwTimeSlotMask 时晰参数使用说明：
*    时晰分配的掩码，由Ts31 Ts30...Ts0构成32位掩码，TSx为1时表示使用该时隙，为0时不使用
* Ts0作为同步信号，用户不可使用，Ts16可作为数据或信令，在本模块中该时隙视用户需求通过设置
* 时隙掩码选择是否使用，时隙分配建议从低到高连续分配时隙，不要间隔，否则可能容易引起通信系
* 统不稳定。
* 例: 使用5个时隙时dwTimeSlotMask = 0x0000003e,此时带宽= 64x5 = 320kbit。 注意:Ts0=0; 
**********************************************************************************/

/* 回调函数类型定义 */
typedef void ( * TE1MsgCallBack )(UINT8 byE1ChanID, UINT32 dwState);

/* 用户传入的发送数据参数结构定义 */
typedef struct{
    UINT8* pbyMsg;                  /* 用户消息指针 */
    UINT32 dwMsgLen;                /* 用户消息长度 */
} TE1TransMsgParam;


/* 通道数据收发统计结构定义 */
typedef struct{
    UINT32 dwRxOnePacketLen;        /* 接收到的单个包的长度 */
    UINT32 dwRxPackets;             /* 从E1线路上收到的包数 */
    UINT32 dwRxErrPackets;          /* 收到错误的包数 */
    UINT32 dwRxDropPackets;         /* 传给上层用户过程中丢掉的包数 */
    UINT32 dwRxSucSndPackets;       /* 成功传给上层用户的包数 */
    UINT32 dwTxSndPackets;          /* 上层要发送的包数 */
    UINT32 dwTxSucSndPackets;       /* 上层成功发送掉的包数 */
    UINT32 dwTxSucSndPacketLen;     /* 上层成功发送掉的总包长，字节为单位 */
}TE1TransChanStat;


/* ================================= 函数声明 ============================== */

/* 获取设备实际支持的E1个数 */
UINT8 E1TransGetMaxE1Num(void);

/* 打开一个E1透明传输通道 */
INT32 E1TransChanOpen
(
    UINT8 byE1ChanID,               /* 打开的通道号,必须小于 E1_TRANS_CHAN_MAX_NUM */
    TE1TransChanParam *ptChanParam, /* 通道的参数设置 */
    TE1MsgCallBack  tE1MsgCallBack  /* 注册的回调函数 */
);

/* 关闭一个E1透明传输通道 */
INT32 E1TransChanClose(UINT8 byE1ChanID);                  

/* 数据发送 */
INT32 E1TransChanMsgSnd(UINT8 byE1ChanID, TE1TransMsgParam *ptMsgParam); 

/* 读取指定通道发送缓冲区队列中待发送的数据包的个数 */
INT32 E1TransTxPacketNumGet(UINT8 byE1ChanID);

/* 增加指定通道共享内存区的头指针,增量为dwOffset字节 */
INT32 E1TransBufQHeadPtrInc(UINT8 byE1ChanID, UINT32 dwOffset);   

/* 拷贝指定通道共享内存区的指定长度的数据到pbyDstBuf指向的内存,返回读出数据的长度 */
UINT32 E1TransBufMsgCopy(UINT8 byE1ChanID, UINT8 *pbyDstBuf, UINT32 dwSize); 

/* 获取指定通道共享内存区待取的数据长度，字节为单位 */
UINT32 E1TransBufMsgLenGet(UINT8 byE1ChanID);    

/* 读取指定通道共享内存区从当前头指针开始偏移量为dwOffset处的一个字节 */
INT32 E1TransBufMsgByteRead(UINT8 byE1ChanID, UINT32 dwOffset, UINT8 *pbyRtnByte);  

/* 将指定通道物理线路自环 */
INT32 E1TransChanLocalLoopSet(UINT8 byE1ChanID, BOOL bIsLoop);

/* 获取指定通道数据收发的统计信息 */
INT32 E1TransChanInfoGet(UINT8 byE1ChanID, TE1TransChanStat *ptInfo);    

#ifdef __cplusplus 
} 
#endif /* __cplusplus */


#endif /* __INC_E1TransLib_H */
