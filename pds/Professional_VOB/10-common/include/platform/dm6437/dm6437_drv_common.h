/******************************************************************************
模块名  ： DM6437_DRV
文件名  ： dm6437_drv_common.h
相关文件： 
文件实现功能：
作者    ：张方明
版本    ：1.0.0.0.0
-------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
04/22/2008  1.0         张方明      创建
06/08/2009  1.1         张方明      增加CPU和内存使用率功能函数
******************************************************************************/
#ifndef __DM6437_DRV_COMMON_H
#define __DM6437_DRV_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* runtime include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "brdwrapperdef.h"                /* 使用单板种类ID号宏定义 */
#include "algorithm/algorithmType.h"

/* 板级驱动模块返回值定义  */
#define DM6437_DRV_SUCCESS                0                  /* 成功 */

#define DM6437_DRV_ERR_BASE               0                  /* 错误码基值 */
#define DM6437_DRV_DEV_NOT_EXIST          (DM6437_DRV_SUCCESS+1)  /* 设备不存在 */
#define DM6437_DRV_DEV_NOT_OPEN           (DM6437_DRV_SUCCESS+2)  /* 设备没有打开 */
#define DM6437_DRV_PARAM_ERR              (DM6437_DRV_SUCCESS+3)  /* 参数非法 */
#define DM6437_DRV_LENGTH_ERR             (DM6437_DRV_SUCCESS+4)  /* 信息长度错误 */
#define DM6437_DRV_QUEUE_FULL             (DM6437_DRV_SUCCESS+5)  /* 数据队列已满 */
#define DM6437_DRV_NOT_SUPPORT            (DM6437_DRV_SUCCESS+6)  /* 设备不支持该操作 */
#define DM6437_DRV_OPT_TIMEOUT            (DM6437_DRV_SUCCESS+7)  /* 操作超时 */
#define DM6437_DRV_NO_MEM                 (DM6437_DRV_SUCCESS+8)  /* 没有可用内存 */
#define DM6437_DRV_DEV_BUSY               (DM6437_DRV_SUCCESS+9)  /* 设备忙，已经被打开 */
#define DM6437_DRV_GIO_FAILE              (DM6437_DRV_SUCCESS+10) /* GIO操作失败 */
#define DM6437_DRV_FAILURE                -1                 /* 未知的异常失败 */

/* 设备身份信息
 */
typedef struct{
    u32 dwBrdID;        /* 使用brdwrapperdef.h中单板种类ID号宏定义 */
    u32 dwHwVer;        /* 硬件版本号 */
    u32 dwFpgaVer;      /* EPLD/FPGA/CPLD的程序版本号 */
    u32 dwDspId;        /* 标识当前是哪一块dsp，从0开始编号 */
} TDm6437DevInfo;


/* ---------------------------------------- 视频公共部分 ---------------------------------- */

/* 视频接口宏定义 */
#define VID_INTF_NONE                           0x00000000
#define VID_INTF_HDMI0                          0x00000001
#define VID_INTF_VGA0                           0x00000010
#define VID_INTF_YPbPr0                         0x00000100
#define VID_INTF_YPbPr1                         0x00000200
#define VID_INTF_SDI0                           0x00001000
#define VID_INTF_DVI0                           0x01000000

#define VID_INTF_C0                             0x00010000
#define VID_INTF_C1                             0x00020000
#define VID_INTF_C2                             0x00040000
#define VID_INTF_C3                             0x00080000

#define VID_INTF_S0                             0x00100000
#define VID_INTF_S1                             0x00200000
#define VID_INTF_S2                             0x00400000
#define VID_INTF_S3                             0x00800000

#define VID_INTF_HDMI                           0x0000000f
#define VID_INTF_VGA                            0x000000f0
#define VID_INTF_YPbPr                          0x00000f00
#define VID_INTF_SDI                            0x0000f000
#define VID_INTF_C                              0x000f0000
#define VID_INTF_S                              0x00f00000
#define VID_INIF_DVI                            0x0f000000

/* 视频源信号信息结构定义
    720P60          = 1280*720  Progressive 60fps
    1080i60         = 1920*1080 interlace   30fps(60场)
    1080P30         = 1920*1080 Progressive 30fps
    D1(PAL)         = 720*576   interlace   25fps(50场)
    D1(NTSC)        = 720*480   interlace   30fps(60场)
    VGA60/75/85     = 640*480   Progressive 60/75/85fps
    SVGA60/75/85    = 800*600   Progressive 60/75/85fps
    XGA60/75/85     = 1024*768  Progressive 60/75/85fps
    SXGA60/75/85    = 1280*1024 Progressive 60/75/85fps
 */
typedef struct{
    u16	    wWidth;                     /* 视频宽，像素为单位,为0表示无信号 */
    u16	    wHeight;                    /* 视频高，行为单位,为0表示无信号 */
    BOOL    bProgressive;               /* 逐行或隔行，TRUE=逐行；FALSE=隔行 */
    u32     dwFrameRate;                /* 帧率，逐行时=场频，隔行时=场频/2，即60i=30P,为0表示无信号 */
    u32     dwColorSpace;               /* 详见brdwrapperdef.h: 视频格式掩码定义 */
} TVidInfo;

/* 帧数据描述结构定义 */
typedef struct{
    u32	    dwFBufId;                   /* 帧BUF的索引号，驱动内部使用，用户不能修改 */
    u8 *    pbyFBuf;                    /* 帧BUF的指针，指向帧数据Buf；
                                            用户分配BufDesc时如果填NULL的话驱动自动分配1个数据BUF，否则使用用户指定的地址作为视频数据BUF，
                                            !!! 如果是用户分配，
                                                1、用户必须保证Buf的对齐，即起始地址必须是128字节对齐;
                                                计算BUF大小时视频按照公式：round_32(wWidth*bitPerPixel/8)*wHeight，
                                                除了OSD属性层设备bitPerPixel=4外其他全为16；
                                                2、关闭设备时驱动不会释放这些内存 */
    BOOL    bUseCache;                  /* 仅对于用户分配Buf有效，驱动自动分配的为带cache的；
                                           为TRUE表示用户分配的Buf带cache，驱动里面会进行刷cache处理；
                                           为FALSE为不带cache的，驱动不做刷cache处理 */
    u32	    dwTimeStamp;                /* 帧的时间戳，采集时用户可以读取当前帧的时间搓 */
} TFBufDesc;


/* -------------------------------------DSP/BIOS系统相关结构定义------------------------------------- */
/* 内存统计信息
 */
typedef struct{
    u32   dwTotal;              /* original size of segment */
    u32   dwUsed;               /* number of bytes used in segment */
    u32   dwFreeBlkMax;         /* length of largest contiguous block */
} TDm6437MemStat;


/*==============================================================================
    函数名      : Dm6437DrvSetup
    功能        ：Dm6437驱动注册安装，该函数根据指定的板子设备号、硬件版本号和逻辑
                  芯片EPLD/FPGA/CPLD的程序版本号来初始化特定的外设，以方便不同设备
                  和相同设备不同硬件版本之间的兼容。
                      注意：该函速必须在使用音视频及其他驱动之前调用!
    输入参数说明：dwBrdID: 使用brdwrapperdef.h中单板种类ID号宏定义；
                  dwHwVer: 硬件版本号,在HOST侧可以通过相应的函数获取到，如果只有
                  1个硬件版本，可以填0，因为驱动不会判断处理；
                  dwFpgaVer: EPLD/FPGA/CPLD的程序版本号, 有时硬件版本号不改，仅改
                  掉逻辑芯片的程序来实现不同的功能；用户在HOST侧可以通过相应的函数
                  获取到该版本，如果只有1个版本或版本修改不影响驱动资源的改动用户
                  可以填0，因为驱动不会判断处理；
                  dwDspId: 标识当前是哪一块dsp，从0开始编号，对于多个dsp共存的时候
                  需要区别是板子上哪一块dsp芯片，需要区别对待。
    返回值说明  ：错误返回DM6437_DRV_FAILURE或错误码；成功返回DM6437_DRV_SUCCESS
------------------------------------------------------------------------------*/
l32 Dm6437DrvSetup(u32 dwBrdID, u32 dwHwVer, u32 dwFpgaVer, u32 dwDspId);

/*==============================================================================
    函数名      : Dm6437DevInfoGet
    功能        ：查询Dm6437所在设备的身份信息
    输入参数说明：无
    返回值说明  ：错误返回NULL；成功返回存放设备身份信息的指针
------------------------------------------------------------------------------*/
TDm6437DevInfo * Dm6437DevInfoGet(void);

/*==============================================================================
    函数名      : Dm6437GetMemStat
    功能        ：查询DDR2内存
    输入参数说明：nSegId：memory segment identifier
                  ptMemStat: 返回的内存统计信息。
    返回值说明  ：错误返回DM6437_DRV_FAILURE或错误码；成功返回DM6437_DRV_SUCCESS
------------------------------------------------------------------------------*/
l32 Dm6437GetMemStat(l32 nSegId, TDm6437MemStat *ptMemStat);

/*==============================================================================
    函数名      : Dm6437GetCpuLoad
    功能        ：查询DSP的使用率百分比，要使用该功能，需要主工程做如下配置:
                    1、在tcf文件中import相关的load.tci
                    2、在main函数中调用Load_init()函数初始化，函数原型：
                        extern Void Load_init(void);
                      ! 注意cpp文件声明时加上extern "C"。
    输入参数说明：无
    返回值说明  ：错误返回DM6437_DRV_FAILURE；成功返回使用率百分比
------------------------------------------------------------------------------*/
l32 Dm6437GetCpuLoad(void);


#ifdef __cplusplus 
} 
#endif /* __cplusplus */

#endif /* __DM6437_DRV_COMMON_H */
