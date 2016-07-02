/*******************************************************************************
 * 模块名      : TiVidDrv
 * 文件名      : VPort.h
 * 相关文件    : VPort.c
 * 文件实现功能: 视频采集/播放的接口定义
 * 作者        : 钮月忠㊣
 * 版本        : V1.0 Copyright(C) 1995-2006 KEDACOM, All rights reserved.
 * -----------------------------------------------------------------------------
 * 修改记录:
 * 日  期       版本        修改人      修改内容
 * 2006/05/08   1.0a        钮月忠      创建	
*******************************************************************************/
#ifndef VPORT_H
#define VPORT_H 1

enum 
{
    VPORT_OK = 0,
    VPORT_OOM,  /*Out Of Memory*/
    VPORT_BUSY,
    VPORT_CLOSED,
    VPORT_FAILED,
    VPORT_INV_PARAM,
    VPORT_SETUP_FAILED,   /*初始化硬件接口失败*/
    VPORT_ACQUEST_FAILED, /*分配硬件资源失败*/
    VPORT_NO_VALID_FRAME,
    VPORT_NO_VALID_SPACE
    
};

/*视频口序号定义*/
enum
{
    VPORT_PORT_0 = 0,   /*VP0*/
    VPORT_PORT_1,       /*VP1*/
    VPORT_PORT_2,       /*VP2*/
    VPORT_PORT_MAX_NUM
    
};

/*视频图像大小的定义*/
enum
{
    VPORT_FRAME_SPEC_D1 = 0, /*D1*/
    VPORT_FRAME_SPEC_4CIF,   /*4CIF*/
    VPORT_FRAME_SPEC_2CIF,   /*2CIF*/
    VPORT_FRAME_SPEC_CIF,    /* CIF*/
    VPORT_FRAME_SPEC_MAX_NUM
    
};

/*VPControl 中支持的操作命令*/
enum
{
    VPORT_CMD_START = 0,            /*起点视频通道*/
    VPORT_CMD_PAUSE,                /*暂停视频通道*/
    VPORT_CMD_GET_STAT,             /*取得状态统计*/
    VPORT_CMD_RESET_STAT,           /*复位状态统计*/
    VPORT_CMD_GET_READY_BUF_NUM,    /*取得视频管理模块中准备好的缓存数*/
    VPORT_CMD_GET_AVAIL_BUF_NUM,    /*取得视频管理模块中空闲好的缓存数*/
    VPORT_CMD_MAX_NUM
    
};

/*视频 A/D D/A 芯片*/
enum
{
    VPORT_CHIP_ADV7180 = 0,
    VPORT_CHIP_SAA7121,
    VPORT_CHIP_ADV7181,
    VPORT_CHIP_ADV7179,
    VPORT_CHIP_MAX_NUM
    
};

/*
 * 视频帧结构体定义（必须 128 字节对齐 —— L2 CACHE LINE SIZE）
 * 该结构设计为变长结构体。包括了指向一帧图像的 YUV 数据和这些分量的首地址。
 * 与 MediaCtrl 的接口定义为直接传递 YUV 数据缓存的首地址；而驱动需要个分量
 * 的首地址。为了符合两者的需求，需要“ YUV 数据缓存的首地址”与该结构体首地址
 * 之间转换。为了方便这种转换，要求 YUV 数据缓存对齐在 128 字节边界；该结构
 * 对齐在 128 字节边界；从结构头到 abyYUVBuf 的偏移量为 128 字节。
 */
typedef struct
{
    #define TVFRAME_HEAD_SIZE 128
    
    union
    {
        struct 
        {
            u8 *pbyY; /*指向 Y 缓存*/
            u8 *pbyU; /*指向 U 缓存*/
            u8 *pbyV; /*指向 V 缓存*/
            
            /*时间戳：本包完成采集时的时间（系统初始化后的实际运行时间）*/
            /*单位为毫秒（ms）*/
            u64 qwTimeStamp; 
            
        } tHeadStruct;
        
        /*保证头结构的大小为 TVFRAME_HEAD_SIZE 字节*/
        /*在实现中（VCFrameAlloc）对该前提进行运行时检查*/
        u8 _RESERVED[TVFRAME_HEAD_SIZE]; 
        
    } tHeadUnion;

    u8 abyYUVBuf[1]; /*必须保证该成员在 TVFRAME_HEAD_SIZE 字节偏移量处*/
    
} TVFrame, *TVFrame_t;

#define pbyY        tHeadUnion.tHeadStruct.pbyY
#define pbyU        tHeadUnion.tHeadStruct.pbyU
#define pbyV        tHeadUnion.tHeadStruct.pbyV
#define qwTimeStamp tHeadUnion.tHeadStruct.qwTimeStamp

/* VP 口状态统计*/
typedef struct
{
    u32 dwFrameDropped;         /*丢帧数*/
    
    /*针对采集的统计信息*/
	u32 dwCapComplete;
	u32 dwCapOverrun;           
	u32 dwCapSyncByteErr;
	u32 dwCapShortFieldDetect;
    u32 dwCapLongFieldDetect;
    
    /*针对播放的统计信息*/
    u32 dwDisComplete;
    u32 dwDisUnderrun;
    u32 dwDisCompleteNack;

} TVPStat, *TVPStat_t;

/*视频通道参数*/
typedef struct 
{       
    s32 nFrameNum;  /*用户指定的帧缓存的数量，需不大于 TVPort 中能保存的最大值*/
    s32 nAllocLen;  /*用户指定的一帧（YUV）数据的字节数*/
    s32 nFrameSpec; /*指定视频帧的规格（D1、4CIF、2CIF 和 CIF 等）*/
    
    s32 nChipName; /*A/D D/A 芯片的名字（VPORT_CHIP_ADV7180 ... ）*/
    s32 nPortNum;  /*A/D D/A 芯片上的端口号，目前未使用*/
    s32 nAddrSel;  /*A/D D/A 芯片 I2C 地址选择（0/1）*/
    
    BOOL bPAL;     /*PAL(TRUE); NTSC(FALSE)*/
    
} TVPortParam, *TVPortParam_t;

/*==============================================================================
    函数名      : VCCreate
    功能        ：初始化视频通道，分配帧缓存资源等
    算法实现    ：略
    引用全局变量：g_atVPorts
    输入参数说明：nVPNum(I):    VP 通道号
                  bCapMode(I):  采集/播放模式，采集（TRUE）/播放（FALSE）
                  ptParam(I):   用户指定的参数
    返回值说明  ：成功（VPORT_OK）
                  失败（其他值）
==============================================================================*/
s32 VCCreate(s32 nVPNum, BOOL bCapMode, TVPortParam_t ptParam);

/*==============================================================================
    函数名      : VCDelete
    功能        ：释放视频通道，释放分配的资源，停止采集/播放
    算法实现    ：略
    引用全局变量：g_atVPorts
    输入参数说明：nVPNum(I):    VP 通道号
    返回值说明  ：成功（VPORT_OK）
                  失败（其他值）
==============================================================================*/
s32 VCDelete(s32 nVPNum);

/*==============================================================================
    函数名      : VCControl
    功能        ：设置采集/播放参数
    算法实现    ：略
    引用全局变量：g_atVPorts
    输入参数说明：ptVCHandle(I):    通道句柄
    返回值说明  ：成功（VPORT_OK）
                  失败（其他值）
==============================================================================*/
s32 VCControl(s32 nVPNum, u32 dwCmd, void *pvArgs);

/*==============================================================================
    函数名      : VCAlloc
    功能        ：上层向驱动申请一帧视频缓存的所有权
    算法实现    ：操作缓存管理模块时需要与中断互斥
    引用全局变量：g_atVPorts
    输入参数说明：nVPNum(I):    VP 通道号
                  pptFrame(O):  返回指向帧缓存的指针
    返回值说明  ：成功（VPORT_OK）
                  失败（其他值）
==============================================================================*/
s32 VCAlloc(s32 nVPNum, TVFrame_t *pptFrame);

/*==============================================================================
    函数名      : VCFree
    功能        ：上层向驱动归还一帧视频缓存的所有权
    算法实现    ：操作缓存管理模块时需要与中断互斥
    引用全局变量：g_atVPorts
    输入参数说明：nVPNum(I):    VP 通道号
                  ptFrame(I):   返回指向帧缓存的指针
    返回值说明  ：成功（VPORT_OK）
                  失败（其他值）
==============================================================================*/
s32 VCFree(s32 nVPNum, TVFrame_t ptFrame);

/*==============================================================================
    函数名      : VCExchange
    功能        ：上层与驱动交换一帧视频缓存的所有权
    算法实现    ：如果驱动没有帧可以和应用程序交换，则没有任何改变
    引用全局变量：g_atVPorts
    输入参数说明：nVPNum(I):    VP 通道号
                  pptFrame(O):  返回指向帧缓存的指针
    返回值说明  ：成功（VPORT_OK）
                  失败（其他值）
==============================================================================*/
s32 VCExchange(s32 nVPNum, TVFrame_t *pptFrame);

#endif /*#ifndef VPORT_H*/
