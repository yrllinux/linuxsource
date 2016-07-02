/* ipohEnd.h */

/* Copyright 2001, Kdc  Co., Ltd. */

/*
modification history
--------------------
01a, 01/10/27, created zwj
*/

/*
DESCRIPTION
    Header file of ipoh driver.
    TBD , change the inter device driver and route table
*/

#ifndef __INCmotMccTransh
#define __INCmotMccTransh

#ifdef __cplusplus 
extern "C" { 
#endif /* __cplusplus */


/*#include"motmccHdlc.h" */
#include <stdio.h>
#include <stdlib.h>
#include <vxworks.h>
#include "semLib.h" 
#include "taskLib.h"
#include "msgQLib.h"
#include "iv.h"
/*#include "motMccHdlc.h" */

/* defines */
#define DEBUG


#ifdef DEBUG
    #define DBG_PRINTF(fmt, args...) \
        printf("[Drv debug]: ");\
        printf(fmt, ## args)                     /* 调试信息打印 */
#else
    #define DBG_PRINTF(fmt, args...) \
/*      if (eti_board_debug) printf(fmt, ## args)*/
#endif

#define ERR_PRINTF(fmt, args...) \
    printErr("[Drv ERR]: ");\
    printErr(fmt, ## args)                        /* 错误信息打印 */


#define MSG_PRINTF(fmt, args...) \
        printf(fmt, ## args)                    /* 消息提示打印 */



/* Configuration items */
/* MCC_TRANS通信状态 */
#define MCC_TRANS_SUCCESS          0    /* 操作成功 */
#define MCC_TRANS_NOT_INIT         1    /* 未初始化 */
#define MCC_TRANS_LENGTH_ERROR     2    /* 数据长度错误 */
#define MCC_TRANS_NO_BUF           3    /* 没有可用的BUF */
#define MCC_TRANS_FAILURE         -1    /*  操作失败 */

#define MCC_TRANS_MAX_BUF_LEN   8000    /* 消息队列中最大BUF的长度 */
#define MCC_TRANS_MIN_BUF_LEN   800     /* 消息队列中最小BUF的长度 */

#define MCC_TRANS_E1_TS16_USE     1     /* 使用TS16 */
#define MCC_TRANS_E1_TS16_NOTUSE  0     /* 不使用TS16 */

/* 同步方式 */
/*#define MCC_TRANS_SYNC_NO        0x7000   /* 发送和接收无同步控制 */
/*#define MCC_TRANS_SYNC_SLOT      0x7100   /* 发送和接收使用时晰同步 */
/*#define MCC_TRANS_SYNC_PATTERN8  0x7200  /*  接收采用8位同步字符同步 */
/*#define MCC_TRANS_SYNC_PATTERN16 0x7300  /*  接收采用16位同步字符同步 */

/* 同步方式，单字节比特流顺序和上面相反 */
#define MCC_TRANS_SYNC_NO        0x7400   /* 发送和接收无同步控制 */
#define MCC_TRANS_SYNC_SLOT      0x7500   /* 发送和接收使用时晰同步 */
#define MCC_TRANS_SYNC_PATTERN8  0x7600  /*  接收采用8位同步字符同步 */
#define MCC_TRANS_SYNC_PATTERN16 0x7700  /*  接收采用16位同步字符同步 */

/* Max number of MCCTRANS units */
#define MAX_MCCTRANS_UNITS   128
#define MAX_E1_NUM             8

#define NULL_CHANNEL    65535   // NULL channel
#define ROUTE_TBL_SIZE     MOT_MCC_CHAN_NUM  /* 128 mean a mcc have a 128 channel */    
#define MAX_MCCTRANS_CHAN  MOT_MCC_CHAN_NUM

/* Range of maximum transfer unit   */

#define MCC_MAX_DEVS    256

#define MCC_TRANS_CCS    0         /* TS16用作共路信令，用户可以使用传数据 */
#define MCC_TRANS_CAS    1         /* TS16用作随路信令，用户不可使用传数据 */

#define MCC_TRANS_OPENED        0xabcddcba         /* MCC控制器首次基本初始化完成标志 */
#define UNUSED_CHAN_FLAG        0xff
  
/* Number of net buffers per unit */

typedef void ( * TMccMsgCallBack )( UINT8 byChanID, UINT32 wState);
 
/* typedefs */



 
/********************************************************************************* 
* 时晰分配说明：
*    时晰分配的掩码，由Ts31 Ts30...Ts0构成32位掩码，TSx为1时表示使用该时隙，为0时不使用
* Ts0作为同步信号，用户不可使用，Ts16可作为数据或信令，用户需在创建时给出模式；
* 时晰分配建议从低到高连续分配时隙，不要间隔，否则可能容易引起通信系统不稳定。
* 例: 使用5个时隙时dwTimeSlotMask = 0x0000003e,注意:Ts0=0; 此时带宽= 64x5 = 320kbit
**********************************************************************************/
typedef struct{
    UINT32  dwBufNum;           /* 消息队列中buf的总数 */
    UINT16  wBufLength;         /* 消息队列中每个buf的长度 */
    UINT16  wE1ID;              /* E1的编号（0~7,视模块型号而定） */
    UINT32  dwTimeSlotMask;     /* 见以上的时晰分配说明*/
    UINT16  wTransSyncMode;     /* 同步模式 ，详见同步方式宏定义*/
    UINT16  wTransSyncPattern;  /* 同步字符 */
    UINT32 *pdwBufBaseAddr;     /* 存放返回的共享内存基地址的指针 */
} TMccTransChanParam;

typedef struct{
    UINT8   byTS16RunMode;                                  /* TS16的工作模式 */
    TMccTransChanParam tMccTransChanParam[MCC_MAX_DEVS];    /* 每个通道的参数设置 */
} TMccTransOpenParam;



/* 用户传入的发送消息参数结构定义 */
typedef struct{
        UINT8* pbyMsg;        /* 用户消息指针 */
        UINT16 dwMsgLen;      /* 用户消息长度 */
} TMccTransMsgParam;



/* TCciHostStat 结构定义 */
typedef struct{
    UINT32 dwRxPackets;
    UINT32 dwRxDropPackets;
    UINT32 dwRxErrPackets;
    UINT32 dwRxSucSndPackets;
    UINT32 dwTxSndPackets;
    UINT32 dwTxSucSndPackets;
}TMccTransChanStat;

typedef struct{
    UINT8  byMccTransChanCtrlID;              /* 设备控制编号 */
    UINT8  byMccChan;                         /* 通道设备使用的超信道中mcc channel的编号 */
    UINT32 dwMccTransChanHandle;              /* 通道设备控制句柄 */

    UINT32 dwShMemBaseAddr;
    UINT32 dwHead;              /* 头指针 */
    UINT32 dwTail;              /* 尾指针 */
    
    UINT8 *m_pbyStartPtr;//环形缓冲区基地址
    UINT8 *m_pbyEndPtr; //环形缓冲区尾地址，它指向的空间不可用
    UINT32 m_dwLen;             //环形缓冲区长度

    UINT8 *m_pbyReadPtr;//当前读指针
    UINT8 *m_pbyWritePtr;//当前写指针
	
    UINT32  dwBufNum;           /* 消息队列中buf的总数 */
    UINT16  wBufLength;        /* 消息队列中每个buf的长度 */
    TMccMsgCallBack  tMccMsgCallBack;

    /*TMccTransChanParam tMccTransChanParam;    /* 启动通道设备时上层传入的参数 */
    TMccTransChanStat tMccTransChanStat;      /* 通信的信息统计 */
}TMccTransChanInfo;

typedef struct TMccTransCtlTag
{
    UINT32 dwTimeSlotUsed[MAX_E1_NUM];        /* 总共分配的时晰 */
    UINT32 dwDevChanUsedNum;                  /* 总共使用的设备通道总数 */
    UINT32 dwMccTransSpecHandle;              /* 没有使用的时晰捆绑并初始化后得到的句柄 */
    TMccTransChanInfo tMccTransChanInfo[MCC_MAX_DEVS];
} TMccTransCtl;



typedef struct TMccTransModuleTag
{
    UINT8  byExistE1Num;
    UINT32 dwMccTransOpened;
    UINT32 dwMccTransCtl;
} TMccTransModule; 
 

/* 创建mcc控制器 */
INT32 MccTransOpen
(
    UINT8 byCrtChanNum,                         /* 创建多少个通道 */
    TMccTransOpenParam *ptMccTransOpenParam,    /* 通道的参数设置 */
    TMccMsgCallBack  tMccMsgCallBack            /* 回调函数 */
);

/* 增加指定通道共享内存区的头指针 */
INT32 MccTransBufQHeadPtrInc(UINT8 byDevChanID, UINT32 dwOffset);   

/* 设置ts16的工作模式 */
INT32 MccTransSetTS16Mode(UINT8 byMode);   

/* 关闭mcc控制器 */
INT32 MccTransClose(void);                  
        
/* 获取指定通道共享内存的头指针 */
UINT8* MccTransGetBufQHeadPtr(UINT8 byDevChanID);   

/* 数据发送 */
INT32 MccTransMsgSnd(UINT8 byDevChanID, TMccTransMsgParam *ptMccTransMsgParam); 

/* 拷贝指定通道共享内存区的指定长度的数据到pbyDstBuf指向的内存 */
UINT32 MccTransMsgCopy(UINT8 byDevChanID, UINT8 *pbyDstBuf, UINT32 dwSize); 

/* 获取指定通道共享内存区待取的数据 */
UINT32 MccTransGetMsgLen(UINT8 byDevChanID);    

/* 读取指定通道共享内存区从当前头指针开始偏移量为dwOffset处的一个字节 */
UINT8 MccTransReadOneByte(UINT8 byDevChanID, UINT32 dwOffset);  

/* E1自环 */
#define E1_LOCAL_LOOP_ON    0
#define E1_LOCAL_LOOP_OFF   1
void MccTransE1LoopConf(UINT8 byE1ID, UINT8 byLoopMode);

/* 以下为测试用的函数 */
BOOL MccTransHeaderInc(UINT8 byDevChanID, UINT32 *pdwHeadPtr, UINT32 dwTailPtr);
BOOL IsMccTransBufQEmpty(UINT8 byDevChanID, UINT32 dwHeadPtr, UINT32 dwTailPtr);
BOOL IsMccDevOpened(void);

void MccTransStatShow(UINT8 byDevChanID);
UINT32 MccTransGetTxDataNum(UINT8 byDevChanID);

#ifdef __cplusplus 
} 
#endif /* __cplusplus */



#endif /* __INCmotMccTransh */
