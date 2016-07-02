/*****************************************************************************
模块名      : DSPCCI
文件名      : dspcci_common.h
相关文件    : 
文件实现功能:  host和dsp双方cci通信协议相关的宏和数据结构定义，被两者同时包含，
              注意：包含本头文件前必须包含相应的数据类型头文件，host包含kdvtype.h
              dsp侧包含algorithmtype.h，不可混淆！
作者        : 张方明
版本        : V1.0  Copyright(C) 2008-2010 KEDACOM, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
10/24/2008  01a         张方明     创建.	
******************************************************************************/

#ifndef __DSPCCI_COMMON_H
#define __DSPCCI_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif


/****************************** 模块的版本号命名规定 *************************
总的结构：mn.mm.ii.cc.tttt
     如  Osp 1.1.7.20040318 表示
模块名称Osp
模块1版本
接口1版本
实现7版本
04年3月18号提交

版本修改记录：
----------------------------------------------------------------------------
模块版本：DspCciLib 1.1.1.20081024
增加功能：创建
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DspCciLib 1.1.2.20081225
增加功能：修改dm647的DMA读写函数中死等策略为超时5秒
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DspCciLib 1.1.3.20090109
增加功能：修改dm647的DMA读写函数,使用PRam1,以绕过原先用Pram0和QDMA冲突问题
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DspCciLib 1.1.4.20090115
增加功能：修改dm647的DMA读写函数,改为不触发EDMA中断
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DspCciLib 1.1.5.20090207
增加功能：增加硬件复位DSP芯片功能,从而解决关闭647有时出现PSC超时失败问题
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DspCciLib 1.1.6.20090321
增加功能：修改DM647的PCI通信用的DMA通道，从TC0改为TC1，解决和音频冲突问题。
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DspCciLib 1.1.7.20090413
增加功能：1、提交修改DM647的PCI通信用的DMA通道，从TC0改为TC1，解决和音频冲突问题。
          2、支持从主机侧获得VP口作为输入或输出的配置信息并自动配置VP口
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DspCciLib 1.1.8.20090415
增加功能：1、修改从主机侧获得VP口作为输入或输出的配置信息并自动配置VP口策略；
          2、内核驱动路径修改，需要同步更改一些路径设置
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DspCciLib 1.2.8.20090424
增加功能：1、增加DM647_HPI和DM6437_HPI的驱动；
          2、修改DSPCCI接口，使用全局dwDspId来标识所有的DSP，包括647和6437，
             需要用户绑定物理DSP，通过增加的一个参数来绑定。
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DspCciLib 1.2.9.20090506
增加功能：1、修复DM647_HPI和DM6437_HPI的驱动中CCI统计信息的错误；
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DspCciLib 1.2.10.20090522
增加功能：1、修复DM647驱动中超频参数设置的错误；
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DspCciLib 1.2.11.20090612
增加功能：1、增加DSP的EDMA寄存器打印功能
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DspCciLib 1.3.11.20090622
增加功能：1、增加DSP的内存测试功能接口
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DspCciLib 1.3.12.20090803
增加功能：1、增加防止用户在DSP通信时多次归还消息Buf的校验
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DspCciLib 1.3.13.20090804
增加功能：无
修改缺陷：修复一个查询接收队列消息数和接收消息时判断有无消息的不一致的问题
提交人：张方明
----------------------------------------------------------------------------
模块版本：DspCciLib 1.3.14.20090815
增加功能：无
修改缺陷：修复CCI通信队列被破坏的Bug，改用inline函数替代宏定义来实现字节序转换
提交人：张方明
----------------------------------------------------------------------------
模块版本：DspCciLib 1.3.15.20090818
增加功能：无
修改缺陷：修复CCI通信队列误判读指针不匹配问题
提交人：张方明
----------------------------------------------------------------------------
模块版本：DspCciLib 1.3.16.20091124
增加功能：增加对KDV7810的支持
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DspCciLib 1.3.17.20091126
增加功能：增加对KDV7620的支持；优化了dsp参数配置获取接口
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DspCciLib 1.3.18.20100224
增加功能：增加设置dm647的DDR频率功能，可以设置266(默认)，300和333MHz；
        ! 由于所有的647共用同一个时钟源，因此所有的647芯片必须配置相同的DDR频率
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DspCciLib 1.3.19.20100701
增加功能：每次打开DM647的时候清0一下所有的DDR和L2 Ram，以规避DSP侧某些模块不
          初始化自己的变量导致的一系列奇怪的异常问题。
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DspCciLib 1.3.20.20110412
增加功能：规避dsp收数据发现magic number出错导致业务部工作问题
修改缺陷：无
提交人：hechengjie

****************************************************************************/

/* 版本信息 */
#define DSPCCI_MOD_VERSION              ( const char * )"DspCciLib 1.3.20.20110412"

/* 极限定义 */
#define DSPCCI_DSP_MAXNUM               5           /* 最多支持的DSP的个数 */

/* 消息打印宏定义 */
#define DSPCCI_MAX_PRT_MSGS             100         /* 打印信息的最大个数 */
#define DSPCCI_MAX_PRT_MSGLEN           320         /* 打印信息的最大长度 */

/* DSPCCI通信消息属性的宏定义 */
#define DSPCCI_DN_MSGQ_MAX              256         /* 最大下行消息队列数 */
#define DSPCCI_UP_MSGQ_MAX              256         /* 最大上行消息队列数 */
#define DSPCCI_ERR_LOG_MAX_NUM          32          /* DSP错误记录的最大个数 */
#define DSPCCI_MSG_ALIGN_BYTES 	        256         /* n字节对齐，要求是TDspCciMsgHead结构大小的整数倍 */
#define DSPCCI_PROTOCOL_VERSION         1           /* DSPCCI的协议版本号，协议发生改变时加1，HOST和DSP必须保持一致 */

/* 幻数宏定义 */
#define DSPCCI_MAGIC_NUMBER             0xbeafbeaf
#define DSPCCI_START_MAGIC_NUMBER       0xdaaddeed

#define DSPCCI_HEAD_PAD                 0x55555555

/* 消息类型宏定义 */
#define DSPCCI_IS_USR_MSG               0           /* 用户消息 */
#define DSPCCI_IS_HOST_LOOPBACK_MSG     1           /* 主机自环测试消息 */
#define DSPCCI_IS_HOST_SEND_TEST_MSG    2           /* 主机发送测试消息*/
#define DSPCCI_IS_DSP_SEND_TEST_MSG     3           /* DSP发送测试消息 */
#define DSPCCI_IS_DSP_PRINT_MSG         4           /* DSP打印消息 */

/* 返回值宏定义 */
#define DSPCCI_SUCCESS                  0           /* CCI通信连接建立成功 */
#define DSPCCI_PEER_CLOSED              1           /* CCI通信中远端设备断开 */
#define DSPCCI_NOT_CONNECTED            2           /* CCI通信没有建链 */
#define DSPCCI_SMEM_CORRUPT             3           /* CCI共享内存区被破坏 */
#define DSPCCI_LENGTH_ERROR             4           /* CCI通信信息长度错误 */
#define DSPCCI_QUEUE_FULL               5           /* CCI消息队列已满 */
#define DSPCCI_MSG_LOST                 6           /* CCI消息丢失 */
#define DSPCCI_PARAM_ERR                7           /* 参数错误 */
#define DSPCCI_NOT_SUPPORT              8           /* 不支持的操作 */
#define DSPCCI_MULTI_OPEN               9           /* 多次打开设备 */
#define DSPCCI_NOT_OPEN                 10          /* 设备没有打开 */
#define DSPCCI_OPEN_FAIL                11          /* 设备打开失败 */
#define DSPCCI_IOC_FAIL                 12          /* 设备ioctl失败 */
#define DSPCCI_NO_MEM                   13          /* 内存不足 */
#define DSPCCI_TIMEOUT                  14          /* 操作超时 */
#define DSPCCI_QUEUE_EMPTY              15          /* CCI消息队列空 */
#define DSPCCI_FAILURE                  -1          /* CCI操作失败 */

/* CCI通信消息属性的宏定义 */
#define DSPCCI_UPCHNL                   0           /* 上行通道编号 */
#define DSPCCI_DNCHNL                   1           /* 下行通道编号 */

#define DSPCCI_START_SHM_OFFSET         0x20        /* START区相对于RAM_BASE的偏移量，最前面0x20空间保留
                                                       PCI通信时该区0x20大小用于reset_vector，负责引导程序 */

/* 类型定义 */
typedef void * HDspCciObj;                          /* DSP通信对象类型 */


/* START区结构定义 */
typedef struct{
    volatile u32 dwCciStartMarker;           /* start区的幻数，DSP先初始化为0xdaaddeed,之后主机初始化为0xbeafbeaf */
    volatile u32 dwCciHostStartupFlag;       /* 主机启动标志； 1--启动 */
    volatile u32 dwCciInfoAvailableFlag;     /* Info区有效标志；主机初始化为0，由DSP分配完INFO区内存后设置为1 */
    volatile u32 dwCciInfoBaseAddr;          /* info区基地址，主机初始化为0 */
    volatile u32 dwDspCciVer;                /* DSP侧CCI版本号,由DSP侧填写，主机侧校验是否和DSPCCI_PROTOCOL_VERSION匹配 */
    volatile u32 dwHostCciVer;               /* 主机侧CCI版本号,由主机侧填写，map侧校验是否和DSPCCI_PROTOCOL_VERSION匹配 */
    volatile u32 dwBrdID;                    /* 使用brdwrapperdef.h中单板种类ID号宏定义 */
    volatile u32 dwHwVer;                    /* 硬件版本号 */
    volatile u32 dwFpgaVer;                  /* EPLD/FPGA/CPLD的程序版本号 */
    volatile u32 dwDspId;                    /* 标识当前是哪一块dsp，从0开始编号 */
}
TDspCciStartBuf;


/* 通道控制信息结构定义 */
typedef struct{
    volatile u32 dwBufBase;                  /* 内存池基地址, DSPCCI_MSG_ALIGN_BYTES对齐 */
    volatile u32 dwMaxMsgLen;                /* 消息的最大长度(按BYTE计)，要求8的整数倍 */
    volatile u32 dwMaxMsgs;                  /* 最多缓存的消息个数 */
    volatile u32 dwBufSize;                  /* 内存池大小,为(dwMaxMsgLen*dwMaxMsgs)作DSPCCI_MSG_ALIGN_BYTES对齐 */
    volatile u32 dwReadPtr;                  /* 内存池读指针,为相对内存池基地址的偏移量 */
    volatile u32 dwWritePtr;                 /* 内存池写指针,为相对内存池基地址的偏移量 */
    volatile u32 dwWriteNum;                 /* 已经写入的数据包个数 */
    volatile u32 dwReadNum;                  /* 已经读出的数据包个数 */
    volatile u32 dwNextRcvMsgPtr;            /* 保存下一个接收消息的地址 */
    volatile u32 dwDspRxTxMsgs;              /* DSP发包/收包计数 */
    volatile u32 dwDspRxErrOrTxOkMsgs;       /* DSP成功发包/接收错包计数 */
    volatile u32 dwDspRxTxKBytes;            /* DSP发送/接收数据量KByte */
    volatile u32 dwDspRxTxBytes;             /* DSP发送/接收数据量Byte */
}
TDspCciChnlInfo;


/* INFO区结构定义 */
typedef struct{
    volatile u32 dwCciInfoMarker;            /* INFO区的幻数,有效值为0xbeafbeaf */
    volatile u32 dwHostRdyFlag;              /* 主机就绪标志：1--请求分配通道控制区；3--请求分配通道内存池；7--通知dsp通信创建ok */
    volatile u32 dwDspRdyFlag;               /* DSP就绪标志：1--已分配通道控制区；3--已分配通道内存池； */
    volatile u32 dwDspHeartBeat;             /* DSP心跳：由DSP驱动不断累加，主机侧检测 */
    volatile u32 dwUpChnlNum;                /* 上行通道个数 */
    volatile u32 dwDnChnlNum;                /* 下行通道个数  */
    volatile u32 dwUpChnlInfoBase;           /* 上行通道管理区基地址 */
    volatile u32 dwDnChnlInfoBase;           /* 下行通道管理区基地址  */

    volatile u32 dwDspPrtEn;                 /* 允许打印标志  */
    volatile TDspCciChnlInfo tPrtChnlInfo;   /* 打印通道控制信息 */

    volatile u32 dwDspErrLog[DSPCCI_ERR_LOG_MAX_NUM];/* DSP错误纪录，由DSP出错后填写 */
}
TDspCciInfoBuf;


/* 消息头部结构定义 */
typedef struct{
    volatile u32 dwDebug[4];
    volatile u32 dwMsgMarker;                /* 消息头幻数,有效值为0xbeafbeaf */
    volatile u32 dwMsgType;                  /* 消息类型：参考: 消息类型宏定义 */
    volatile u32 dwMsgLen;                   /* 消息体长度 */
    volatile u32 dwNextMsgAddr;              /* 下一个消息的偏移地址(相对dwBufBase)，DSPCCI_MSG_ALIGN_NUMBER字节对齐 */
}
TDspCciMsgHead;

/* 消息描述结构定义 */
typedef struct{
    u8* pbyMsg1;                    /* 用户消息1指针 */
    u8* pbyMsg2;                    /* 用户消息2指针，接收时该段无效 */
    u32 dwMsg1Len;                  /* 用户消息1长度 */
    u32 dwMsg2Len;                  /* 用户消息2长度，接收时该段无效 */
    u32 dwMsgType;                  /* 消息类型，见: 消息类型宏定义 */
} TDspCciMsgDesc;

#ifdef __cplusplus
}
#endif

#endif /* __DSPCCI_COMMON_H */
