/******************************************************************************
模块名  ： CciLinux
文件名  ： CciLinux.h
相关文件：
文件实现功能：CciLinux模块对外提供的主要头文件
作者    ：王俊华
版本    ：1.0.0.0.0
---------------------------------------------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
06/20/2005  1.0         王俊华      创建
******************************************************************************/
#ifndef _CCILINUX_H
#define _CCILINUX_H

#ifdef __cplusplus 
extern "C" { 
#endif /* __cplusplus */

#include "kdvtype.h"
#include <etilib/etiOsal.h>
#include <etilib/etiErrors.h>
#include <etilib/etiLocal.h>
#include <etilib/etiL2Load.h>
#include <etilib/etiL2PciLoad.h>

/*相关宏定义*/

/*本模块的版本号定义*/
#define CCILINUXVER                     "CciLinux 40.01.00.02.051009"

/*主从机之间通信通道所属的进程号,注意：业务程序必须声明其进程的ID号为该值*/
/*声明方法：在入口函数osalMain()之前：commProcessId_t   commProcessId = HOST_SLAVE_PROCESS_ID;*/
#define HOST_SLAVE_PROCESS_ID           (u32)0

/*上行通道*/
#define UP_CHANNEL                      (u32)0

/*下行通道*/
#define DOWN_CHANNEL                    (u32)1

/*双向通信通道、*/
#define UPDOWN_CHANNEL                  (u32)2                    

/*最多支持的BSP-15数目*/
#define MAX_BSP_NUM                     (u32)6

/*最小的接收缓冲区大小，业务程序在打开通道指定通道的接收缓冲区大小时不能小于该值*/
#define MIN_RCV_BUFFER_SIZE             (u32)6*1024

/*特殊板号定义，如果发送目的板号是该号时，就进行板内数据处理，1.0版本暂不支持*/
#define CCI_LOCAL_BOARD_ID              (u32)(MAX_BSP_NUM)

/*最大上行通道数*/
#define MAX_UP_CHANNEL_NUM              (u32)8
   
/*最大下行通道数*/  
#define MAX_DOWN_CHANNEL_NUM            (u32)256  

/*版本信息的最大长度*/
#define MAX_VERSION_LEN                 (u32)255

/*CCI模块发送给业务程序的消息类型定义*/
/*CCI模块发送给业务程序消息的基址*/
#define CCICOMM_MSG_BASE                (u32)6000

/*发送BSP-15侧心跳的通信通道断链通知消息，消息内容：空*/
#define CCICOMM_HEARTBIT_CONN_LOST      (u32)(CCICOMM_MSG_BASE+1)

/*某个通道断链后，主机侧再次打开该通道时向从机侧发送的消息，从机侧收到该消息后，应该调用相应的接口重新打开该通道
消息内容:结构体TCCIConnLostMsg*/
#define CCICOMM_REOPEN_CHANNEL_REQUEST  (u32)(CCICOMM_MSG_BASE+2) 

/*对端发送过来的数据到达通知消息*/
#define CCICOMM_PEERMSG_ARRIVED         (u32)(CCICOMM_MSG_BASE+3)

/*主机侧关闭所有通信通道通知消息，收到此消息后，从机厕所有通信通道都已经关闭，不能再进行数据通信
消息内容:无*/
#define CCICOMM_HOST_CLOSEALL_NOTIFY    (u32)(CCICOMM_MSG_BASE+4)

/*相关结构体定义*/

/*板级控制接口返回值定义*/
typedef enum {
    CCIBD_OK,                       /* 没有错误，函数执行成功*/
    CCIBD_NO_INIT,                  /*没有调用初始化函数*/
    CCIBD_ERROR,                    /* 操作出现错误.    */
    CCIBD_INVALID_PARM,             /* 不可用的参数.*/
    CCIBD_INSUFFICIENT_MEMORY,      /*内存申请失败*/
    CCIBD_OSAL_ERROR,               /* 执行中OSAL错误.*/
    CCIBD_BSPDRV_ERROR,             /*执行中BSPDRV错误.    */
    CCIBD_INVALID_L2IMAGE,          /* 不可用的L2 image.*/
    CCIBD_COMM_ERROR,               /* 操作中COMM出错.*/
    CCIBD_FILE_ERROR,               /* 文件操作错误. */
    CCIBD_FS_ERROR,                 /* 文件系统出错.       */
    CCIBD_IOMAP_ERROR,              /* IOMAP 出错.       */
    CCIBD_BOARDID_ERROR,            /*板号错误*/
    CCIBD_UIMAGE_ERROR,             /*内核文件错误*/
    CCIBD_ALREADY_OPENED,           /*板子已经打开*/
    CCIBD_NOT_OPEN,                 /*板子没有打开*/
    CCIBD_GETDEFSETUP_ERROR,        /*获取默认的打开BSP-15所需参数出错*/
    CCIBD_LOAD_IMAGE_FAILED,        /*调用低层接口加载内核文件失败*/
    CCIBD_UNLOAD_IMAGE_FAILED,      /*调用低层接口卸载内核文件失败*/
    CCIBD_NOT_SUPPORT,              /*如果输入的扳号是表值内部通信的特殊板号，则返回该值，目前尚不支持*/
}TCCIBDError;

/*CCI通信接口返回值定义*/
typedef enum {
    CCICOMM_OK,			            /*没有错误，函数执行成功*/
    CCICOMM_NOT_INIT,               /*没有调用初始化函数*/
    CCICOMM_CHANNEL_ABORTED,        /*内部使用，外部不可见 */
    CCICOMM_OUT_OF_MEMORY,		    /*内存申请失败*/
    CCICOMM_NO_DRIVER,		        /*COMMOpenChannel函数没有找到可用的驱动*/
    CCICOMM_SIZE_TOO_LARGE,	        /*因消息太大，接收缓存容纳不下*/
    CCICOMM_NO_ATTRIBUTES,		    /*COMMGetChannelAttributes没有找到可用的通道属性*/
    CCICOMM_OSAL_ERROR,		        /*从OSAL层传递的错误*/
    CCICOMM_OSAL_NOT_IMPLEMENTED,   /*从OSAL层传递的错误*/
    CCICOMM_ALREADY_OPENED,         /*通信通道已经打开*/    
    CCICOMM_NOT_OPEN,               /*通信通道没有打开*/    
    CCICOMM_WRONG_CHANNEL_ID,	    /*无效的通道号*/
    CCICOMM_WRONG_BOARD_ID,         /*无效的板号*/
    CCICOMM_PARM_IS_NULL,           /*传入空的指针*/
    CCICOMM_NVALID_PARM,            /* 参数越界*/
    CCICOMM_WAIT_ACK_TIMEOUT,       /*等待对方的应答消息超是*/
    CCICOMM_RCVBUFFER_TOO_SMALL,    /*接收缓冲区的大小小与2K*/
    CCICOMM_NOT_SUPPORT,            /*如果输入的扳号是表值内部通信的特殊板号，则返回该值，目前尚不支持*/
    CCICOMM_CONN_LOST,              /*上层业务成需要发送数据的通道恰好断链了*/
    CCICOMM_CONN_NOTLOST,           /*上层业务程序重新打开通道时，如果该通道没有断链，则返回该错误*/
    CCICOMM_HOST_CLOSEALL,          /*由于主机侧关闭了所有的通道，从机侧再调用接口发送数据则出错*/
}TCCICommError;

/*打开BSP-15时需要传入的参数结构体*/
typedef struct CCIBDSetup {
    u8      *pbyExecFileName;   /* 待加载的BSP-15内核文件*/
    u32     dwFreqCore;         /* BSP-15内核频率（设置0时，默认值）*/
    u32     dwFreqMem;          /* BSP-15内存频率（设置0时，默认值）*/
    u32     dwMemSize;          /* BSP-15内存大小（以字节为单位）（设置0时，默认值）*/
}TCCIBoardSetup;

/*CCI模块发送给业务程序断链通知消息的结构体*/
typedef struct{
    u32     dwBoardID;          /*断链的通道所连接的对端的版号(0~5或CCI_LOCAL_BOARD_ID)*/
    u32     dwChannelID;        /*断链的通道号*/
    u8      byDnOrUp;           /*断链的通道是上行还是下行通道（UP_CHANNEL/DOWN_CHANNEL）*/
}TCCIConnLostMsg;

/*存放主机侧通道端链检测结果的结构体*/
typedef struct{
    u32                 dwConnLostNum;      /*断链的通道通道数(相关的断链信息存储在下边的数组中,有效信息下标范围0~(dwConnLostNum-1))*/
    TCCIConnLostMsg     atCCIConnLostMsg[MAX_UP_CHANNEL_NUM+MAX_DOWN_CHANNEL_NUM];        /*断链的通道信息*/
}TCCIChannelCheck;

/*业务程序注册的接收消息的回调函数类型定义*/
typedef void (*PTCCIMessageCallback)(
    u32     dwBoardID,          /*消息的源板号*/
    u32     dwMsgType,          /*消息类型(见上边的CCI模块发送给业务程序的消息类型定义部分*/
    u8      *pbyMessage,        /*指向消息体的指针*/
    u32     dwSize,             /* 消息的长度*/
    u8      *pbyData            /* 业务程序注册的私有数据*/
);

/*通道属性配置结构体*/
typedef struct {
    u32     dwReceiveCapacity;  /* 单个通道能同时容纳的最多消息数*/
    u32	    dwReceiveBufferSize;/* 单个通道接收缓存大小(以字节为单位，最小MIN_RCV_BUFFER_SIZE)*/
}TCCIChannelConfiguration;

/*打开BSP-15时传入的将要打开通道的信息结构体*/
typedef struct{
    u32     dwDnMsgQs;    /*下行消息队列数 */
    u32     dwUpMsgQs;    /*上行消息队列数 */
    TCCIChannelConfiguration   atCciDnChannelQParam[MAX_DOWN_CHANNEL_NUM];/*下行消息队列参数 */ 
    TCCIChannelConfiguration   atCciUpChannelQParam[MAX_UP_CHANNEL_NUM];/*上行消息队列参数 */            
}TCCIChannelParam;

/*统计信息结构定义*/
typedef struct{
    u32     dwSendCount;        /*总的发包计数*/
    u32     dwSuccSendCount;    /*成功发包计数*/
    u32     dwSuccRecvCount;    /*成功收包计数*/
    u32     dwSendByte;         /*成功发送总字节数*/
    u32     dwRecvByte;         /*成功接收总字节数*/
    u32     dwDownCount;        /*断链次数统计*/
    u32     dwSendLost;         /*因为通道断链而导致发送数据失败的统计*/
}TCCIStatInfo;

/*版本信息结构体*/
typedef struct{
    u8      abyVer[MAX_VERSION_LEN];
}TCCIVersion;

/*函数声明*/


/*================================
函数名：CciLinuxBrdOpen 
功能：加载指定BSP-15的操作系统，使其处于运行状态
算法实现：（可选项）
引用全局变量：
输入参数说明：  dwBoardID(I)  ：BSP-15板号（0～5 或者CCI_LOCAL_BOARD_ID 有效）
                ptSetup(I)：   指向TCCIBoardSetup 结构的变量；
返回值说明： TCCIBDError枚举类型
==================================*/
TCCIBDError CciLinuxBrdOpen (u32  dwBoardID  ,TCCIBoardSetup  *ptSetup);

/*================================
函数名：CciLinuxBrdClose 
功能：关闭板子上指定的BSP-15
算法实现：（可选项）
引用全局变量：
输入参数说明：  dwBoardID(I)  ：BSP-15板号（0～5 或者CCI_LOCAL_BOARD_ID 有效）
返回值说明： TCCIBDError枚举类型
==================================*/
TCCIBDError CciLinuxBrdClose (u32  dwBoardID );

/*================================
函数名：CciLinuxPrintEnable
功能：使能BSP-15侧的打印信息
算法实现：（可选项）
引用全局变量：
输入参数说明：  dwBoardID(I)  ：BSP-15板号（0～5 或者CCI_LOCAL_BOARD_ID 有效）
返回值说明： TCCIBDError枚举类型
==================================*/
TCCIBDError CciLinuxPrintEnable (u32  dwBoardID );

/*================================
函数名：CciLinuxPrintDisable 
功能：关闭BSP-15侧的打印
算法实现：（可选项）
引用全局变量：
输入参数说明：  dwBoardID(I)  ：BSP-15板号（0～5 或者CCI_LOCAL_BOARD_ID 有效）
返回值说明： TCCIBDError枚举类型
==================================*/
TCCIBDError CciLinuxPrintDisable  (u32  dwBoardID);

/*================================
函数名：CciLinuxHostMsgOpen
功能：打开CCI通信通道
算法实现：（可选项）
引用全局变量：
输入参数说明：  dwBoardID (I)：目标板号
	            ptCCIChannelCallBack (I)：注册的接收消息的回调函数
                ptCciChannelParam (I)：  打开通道信息的结构体指针
                pbyData(I):       业务程序注册的私有数据
返回值说明： TCCICommError枚举类型
==================================*/
TCCICommError CciLinuxHostMsgOpen (u32  dwBoardID, PTCCIMessageCallback   ptCCIChannelCallBack, TCCIChannelParam  *ptCciChannelParam, u8* pbyData);

/*================================
函数名：CciLinuxHostMsgClose
功能：关闭与该板号相关的所有CCI通信通道
算法实现：（可选项）
引用全局变量：
输入参数说明：dwBoardID (I)：目标板号
返回值说明： TCCICommError枚举类型
==================================*/
TCCICommError CciLinuxHostMsgClose (u32	dwBoardID);

/*================================
函数名：CciLinuxHostSendMessage
功能：利用指定通道以阻塞方式发送数据
算法实现：（可选项）
引用全局变量：
输入参数说明：  dwBoardID (I)：    目标板号
                dwChanneID (I)： 指定的通道号
                pbyFirstMessage (I)：指向第一个待发送的消息的指针
                dwFirstMsgSize (I)：第一个待发送消息的大小（字节）
                pbySecondMessage (I)：指向第二个待发送的消息的指针
                dwSecondMsgSize (I)：第二个待发送消息的大小（字节）
返回值说明： TCCICommError枚举类型
==================================*/
TCCICommError CciLinuxHostSendMessage 
(
    u32  dwBoardID,
    u32	 dwChanneID,  
    u8  *pbyFirstMessage,
    u32  dwFirstMsgSize, 
    u8  *pbySecondMessage, 
    u32  dwSecondMsgSize
);

/*================================
函数名：CciLinuxHostTrySendMessage
功能：利用指定通道以非阻塞方式发送数据
算法实现：（可选项）
引用全局变量：
输入参数说明：  dwBoardID (I)：    目标板号
                dwChanneID (I)： 指定的通道号
                pbyFirstMessage (I)：指向第一个待发送的消息的指针
                dwFirstMsgSize (I)：第一个待发送消息的大小（字节）
                pbySecondMessage (I)：指向第二个待发送的消息的指针
                dwSecondMsgSize (I)：第二个待发送消息的大小（字节）
                pbSendSuccess (O)：指示发送操作实际是否成功
返回值说明： TCCICommError枚举类型
==================================*/
TCCICommError CciLinuxHostTrySendMessage 
(
    u32  dwBoardID, 
    u32	 dwChanneID,  
    u8  *pbyFirstMessage,
    u32  dwFirstMsgSize ,
    u8  *pbySecondMessage, 
    u32  dwSecondMsgSize,
    BOOL32  *pbSendSuccess
);

/*================================
函数名：CciLinuxHostChannelCheck
功能：主机侧检测通道端链情况的函数
算法实现：（可选项）
引用全局变量：
输入参数说明：  dwBoardID (I)：目标板号
                ptCCIChannelCheck (O):返回的检测结果
返回值说明： TCCICommError枚举类型
==================================*/
TCCICommError  CciLinuxHostChannelCheck (u32 dwBoardID, TCCIChannelCheck *ptCCIChannelCheck);

/*================================
函数名：CciLinuxHostGetMapHeartBeat
功能：获得BSP-15的心跳数
算法实现：（可选项）
引用全局变量：
输入参数说明：  dwBoardID (I)：目标板号
                pdwHeartBeats (O):返回的心跳数
返回值说明： TCCICommError枚举类型
==================================*/
TCCICommError  CciLinuxHostGetMapHeartBeat (u32 dwBoardID, u32  *pdwHeartBeats);

/*================================
函数名：CciLinuxHostGetChannelStateInfo
功能：主机侧获得指定通道统计信息
算法实现：（可选项）
引用全局变量：
输入参数说明：  dwBoardID(I)：目标板号
                byDnOrUp:       上行还是下行通道（UP_CHANNEL/DOWN_CHANNEL）
                dwChanneID(I)：指定的通道,
                ptStateInfo(O)：指向统计信息的结构体指针
返回值说明： TCCICommError枚举类型
==================================*/
TCCICommError CciLinuxHostGetChannelStateInfo(u32   dwBoardID, u8 byDnOrUp, u32	dwChanneID, TCCIStatInfo *ptStateInfo);

/*================================
函数名：CciLinuxHostCCIInfoShow
功能：主机侧板子以及通道信息Dump接口,该接口需要业务程序注册到OSP。
算法实现：（可选项）
引用全局变量：
输入参数说明：  无
返回值说明： 无
==================================*/
void CciLinuxHostCCIInfoShow(void);

/*================================
函数名：CciLinuxHostReOpenUserChannel
功能：主机侧重新打开用户通道
算法实现：（可选项）
引用全局变量：无
输入参数说明：ptCCIConnLostMsg---指向要打开通道相关信息的结构体指针
返回值说明： TRUE--成功
             FALSE-失败
==================================*/
BOOL32 CciLinuxHostReOpenUserChannel(TCCIConnLostMsg* ptCCIConnLostMsg);

/*================================
函数名：CciLinuxSlaveMsgOpen
功能：打开CCI消息通道
算法实现：（可选项）
引用全局变量：
输入参数说明: dwBoardID (I)：目标板号(目标板号即主机的扳号只能为0)
              ptCCIChannelCallBack (I)：注册的接收消息的回调函数
              ptCciChannelParam (O)：从主机侧得到的主机侧打开通道信息的结构体指针
              pbyData(I):       业务程序注册的私有数据
返回值说明： TCCICommError枚举类型
==================================*/
TCCICommError CciLinuxSlaveMsgOpen (u32 dwBoardID, PTCCIMessageCallback   ptCCIChannelCallBack, TCCIChannelParam  *ptCciChannelParam, u8* pbyData);

/*================================
函数名：CciLinuxSlaveSendMessage 
功能：利用指定通道以阻塞方式发送数据
算法实现：（可选项）
引用全局变量：
输入参数说明：  dwBoardID (I)：    目标板号(目标板号即主机的扳号只能为0)
                dwChanneID (I)： 指定的通道号
                pbyFirstMessage (I)：指向第一个待发送的消息的指针
                dwFirstMsgSize (I)：第一个待发送消息的大小（字节）
                pbySecondMessage (I)：指向第二个待发送的消息的指针
                dwSecondMsgSize (I)：第二个待发送消息的大小（字节）
返回值说明： TCCICommError枚举类型
==================================*/
TCCICommError CciLinuxSlaveSendMessage 
(
    u32  dwBoardID, 
    u32	 dwChanneID,    
    u8  *pbyFirstMessage,
    u32  dwFirstMsgSize, 
    u8  *pbySecondMessage, 
    u32  dwSecondMsgSize
);

/*================================
函数名：CciLinuxSlaveTrySendMessage
功能：利用指定通道以非阻塞方式发送数据
算法实现：（可选项）
引用全局变量：
输入参数说明：  dwBoardID (I)：    目标板号(目标板号即主机的扳号只能为0)
                dwChanneID (I)： 指定的通道号
                pbyFirstMessage (I)：指向第一个待发送的消息的指针
                dwFirstMsgSize (I)：第一个待发送消息的大小（字节）
                pbySecondMessage (I)：指向第二个待发送的消息的指针
                dwSecondMsgSize (I)：第二个待发送消息的大小（字节）
                pbSendSuccess (O)：指示发送操作实际是否成功
返回值说明： TCCICommError枚举类型
==================================*/
TCCICommError CciLinuxSlaveTrySendMessage 
(
    u32  dwBoardID, 
    u32	 dwChanneID,  
    u8  *pbyFirstMessage,
    u32  dwFirstMsgSize ,
    u8  *pbySecondMessage, 
    u32  dwSecondMsgSize, 
    BOOL32 *pbSendSuccess
);

/*================================
函数名：CciLinuxSlaveSendMapHeartBeat
功能：从机侧向主机侧发送心跳数
算法实现：（可选项）
引用全局变量：
输入参数说明：  dwBoardID(I)：目标板号(目标板号即主机的扳号只能为0)
                dwHeartBit(I)：心跳数
返回值说明： TCCICommError枚举类型
==================================*/
TCCICommError CciLinuxSlaveSendMapHeartBeat (u32    dwBoardID, u32	 dwHeartBit);

/*================================
函数名：CciLinuxGetChannelStateInfo
功能：从机侧获得指定通道统计信息
算法实现：（可选项）
引用全局变量：
输入参数说明：  dwBoardID(I)：目标板号(目标板号即主机的扳号只能为0)
                byDnOrUp:       上行还是下行通道（UP_CHANNEL/DOWN_CHANNEL)
                dwChanneID(I)：指定的通道,
                ptStateInfo(O)：指向统计信息的结构体指针
返回值说明： TCCICommError枚举类型
==================================*/
TCCICommError CciLinuxSlaveGetChannelStateInfo(u32   dwBoardID, u8 byDnOrUp, u32	dwChanneID, TCCIStatInfo *ptStateInfo);


/*================================
函数名：CciLinuxSlaveReOpenUserChannel
功能：从机机侧重新打开用户通道
算法实现：（可选项）
引用全局变量：无
输入参数说明：ptCCIConnLostMsg---指向要打开通道相关信息的结构体指针
返回值说明： TRUE--成功
             FALSE-失败
==================================*/
BOOL32 CciLinuxSlaveReOpenUserChannel(TCCIConnLostMsg* ptCCIConnLostMsg);

/*================================
函数名：CciLinuxSlaveCCIInfoShow
功能：从机侧板子以及通道信息Dump接口,该接口需要业务程序注册到OSP。
算法实现：（可选项）
引用全局变量：
输入参数说明：  无
返回值说明： 无
==================================*/
void CciLinuxSlaveCCIInfoShow(void);

/*================================
函数名：CciLinuxGetCCIVersion
功能：获得当前CciLinux模块的版本信息
算法实现：（可选项）
引用全局变量：
输入参数说明：  ptCCIVersion(O)：存放版本信息的指针
返回值说明： TCCICommError枚举类型
==================================*/
TCCICommError  CciLinuxGetCCIVersion (TCCIVersion *ptCCIVersion );



#ifdef __cplusplus 
} 
#endif /* __cplusplus */

#endif /*_CCILINUX_H*/
