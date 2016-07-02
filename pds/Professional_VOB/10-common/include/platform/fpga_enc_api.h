/******************************************************************************
模块名  ： FPGA_ENC_DRV
文件名  ： fpga_enc_api.h
相关文件： fpga_enc_api.c
文件实现功能：
作者    ：张方明
版本    ：1.0.0.0.0
-------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
08/08/2007  1.0         张方明      创建
03/13/2008  1.1         张方明      增加downscale功能
******************************************************************************/
#ifndef __FPGA_ENC_API_H
#define __FPGA_ENC_API_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "fpga_codec_def.h"


/* 极限值定义 */
#define FPGA_ENC_DEV_MAX_NUM             1              /* 目前一个主机上最大支持1个FPGA编码芯片 */
#define FPGA_ENC_CHNL_MAX_NUM            32             /* 目前每个编码器芯片最大支持32个编码通道 */
#define FPGA_ENC_VP_MAX_NUM              4              /* 目前每个编码器芯片最大支持4个VP视频采集口 */
#define FPGA_ENC_NALU_MAX_NUM            512            /* 目前1帧数据中最大支持512个NALU单元 */

/* 视频输入接口宏定义 */
#define VIDIN_SHUT_DOWN             0x00000000
#define VIDIN_HDMI0                 0x00000001
#define VIDIN_VGA0                  0x00000010
#define VIDIN_YPbPr0                0x00000100
#define VIDIN_YPbPr1                0x00000200  /* 在KDV7810上该接口和VIDIN_VGA0冲突，只能2选1 */
#define VIDIN_SDI0                  0x00001000
#define VIDIN_C0                    0x00010000
#define VIDIN_S0                    0x00100000
#define VIDIN_DVI0                  0x01000000

#define VIDIN_DEC_VP0               0x80000000
#define VIDIN_DEC_VP1               0x80000001
#define VIDIN_DEC_VP2               0x80000002

/* FpgaEncChnlCtrl控制命令定义  */
#define FPGA_ENC_CTL_GET_STAT            0              /* 查询编码器统计信息，和TFpgaEncStat配对使用 */
#define FPGA_ENC_CTL_SET_OSD             1              /* 设置编码器OSD，和TFpgaEncOsdParam配对使用 */
#define FPGA_ENC_CTL_SET_BRIGHTNESS      2              /* 设置亮度，  和TFpgaEncColorParam配对使用 */
#define FPGA_ENC_CTL_SET_CONTRAST        3              /* 设置对比度，和TFpgaEncColorParam配对使用 */
#define FPGA_ENC_CTL_SET_HUE             4              /* 设置色度，  和TFpgaEncColorParam配对使用 */
#define FPGA_ENC_CTL_SET_SATURATION      5              /* 设置饱和度，和TFpgaEncColorParam配对使用 */
#define FPGA_ENC_CTL_SET_VUI_CFG         6              /* 设置码流VUI信息，和TFpgaEncVuiParam配对使用 */
#define FPGA_ENC_CTL_DRAW_BLOCK          7              /* 编码图像指定位置处画一个灰白色矩形块，和TFpgaEncDrawBlk配对使用 */

/* IipCfg寄存器定义
    Motion estimation engine mode configuration
    31:20 - RESERVED
    19 - staticSearchEnable - Enables search of static (absolute (0,0)) motion vector.
    18 - RESERVED
    17 - fpEngine1Enable - Enables full-pel search engine 1.
    16 - fpEngine0Enable - Enables full-pel search engine 0.
    15 - inter16x16Enable - Enables inter 16x16 macro block type.
    14 - inter16x8Enable - Enables inter 16x8 macro block type.
    13 - inter8x16Enable - Enables inter 8x16 macro block type.
    12 - inter8x8Enable - Enables inter 8x8 macro block type.
    11 - inter8x8SubEnable - Enables inter 8x8 sub-macro block type.
    10 - inter8x4SubEnable - Enables inter 8x4 sub-macro block type.
    9 - inter4x8SubEnable - Enables inter 4x8 sub-macro block type.
    8 - inter4x4SubEnable - Enables inter 4x4 sub-macro block type.
    7 - skipEnable - Enables skip macro blocks.
    6 - spQpelEnable - Enables search of quarter-pel locations when doing sub-pel
    search.
    5 - spHpelEnable - Enables search of half-pel locations when doing sub-pel search.
    4 - spFpelEnable - Enables search of full-pel location when doing sub-pel search.
    3 - randomRefresh - If intra refresh is enabled, makes the refresh 'random' instead of
    contiguous.
    2 - intra4Enable - Enables intra4x4 macro-blocks.
    1 - intra16Enable - Enables intra16x16 macro-blocks.
    0 - pIntraEnable - Enables intra macro-blocks in P-frames
 */
#define FPGA_ENC_IIP_4x4_DISABLE    0xff0f80fa
#define FPGA_ENC_IIP_PINTRAENABLE   (1 << 0)

/* 码流VUI信息定义 */
#define FPGA_ENC_VUI_INC_FRAME_RATE 0x80000000  /* 编码码流sps中包含帧率信息 */

/* 类型定义 */
typedef void * HFpgaEncEngine;
typedef void * HFpgaEncChnl;

/* 全局参数结构定义 */
typedef struct{
    s8  abyFirmwareName[FPGA_STR_MAX_LEN]; /* FPGA二进制文件的名字，支持绝对路径，
                                              如:"/usr/bin/fpga.bin",不指定的话在当前路径下找 */
    u32 dwFrmBufMaxLen;             /* 要求4字节对齐！一般1帧编出来的数据长度最高为期望码率的1/4，如最高要求
                                       码率为8Mbps,则该值应该设为8/8/4=250KBytes,目前建议512KBytes */
    u32 dwFrmBufCnt;                /* 帧缓冲的个数，至少2个 */
} TFpgaEncGblParam;


/* 编码通道回调函数类型定义 */
typedef void (*TFpgaEncChnlCallBack)(HFpgaEncEngine hEngine, TFpgaFrameDesc *ptFrame, u8 *pbyContext);

/* 图像窗口结构定义 */
typedef struct{
//    u32     dwVPId;                         /* 占用的VP口，需配合相应的硬件设计，0~FPGA_VP_MAX_NUM-1 */
    u32     dwPipId;                        /* 子画面的索引，0~FPGA_PIP_MAX_NUM-1 */
    u32     dwPipLeft;                      /* 子画面X坐标，必须是16*8=128的整数倍 */
    u32     dwPipTop;                       /* 子画面Y坐标，必须是16的整数倍 */
    u32     dwPipWidth;                     /* 子画面宽度，必须是16的整数倍，填0表示全屏 */
    u32     dwPipHight;                     /* 子画面高度，必须是16的整数倍，填0表示全屏 */
} TFpgaWinParam;

/* 编码通道创建的参数结构定义 */
typedef struct{
    u32 dwChnlId;                       /* 编码通道的索引，0~FPGA_ENC_CHNL_MAX_NUM-1 */
    u32 dwVPId;                         /* 占用的VP口索引，0~FPGA_ENC_VP_MAX_NUM-1 */
    TFpgaEncChnlCallBack tFrmCallBack;  /* 回调函数指针 */
    u8 *pbyContext;                     /* 用户私有参数，回调函数回调时将该值原样返回给用户 */
    u32 dwBytesPerSlice;                /* 定义每个slice的字节数，最大65535，默认设成1400, 如果设为0表示使用单slice方式
                                           对于1080i60需要使用单slice或多slice时设到4096以上才能保证图像不抖 */
    TFpgaWinParam   tWin;                 /* 编码图像窗口信息 */
} TFpgaEncChnlCrtParam;


/* 编码通道VP口的参数结构定义 */
typedef struct{
    TFpgaVidInfo tVidInfo;              /* 物理接口视频信号信息 */
    TFpgaVidInfo tVpVidInfo;            /* 进编码器VP口视频信号信息， 结构变量有一个为0就表示和物理接口视频信号信息相同 */
    /* 注意：由于现在设置AD和接口fpga由用户直接调用，所以现在tVidInfo和tVpVidInfo都设置为进vp口的视频制式 */

    u32          dwLogicVidIntf;        /* 连接编码器VP口的逻辑视频接口，为0表示和物理视频接口一致，其他参考视频输入接口宏定义 */
    u32          dwReqFrameRate;        /* 需要的帧率，或上FPGA_ENC_VUI_INC_FRAME_RATE后表示码流sps中包含帧率信息 */
    u16	         wDownScaledWidth;      /* DownScal后的视频宽，像素为单位,不能大于视频信号源的大小,传入无效值时会自动设成和视频信号源一致 */
    u16	         wDownScaledHeight;     /* DownScal后的视频高，行为单位,不能大于视频信号源的大小,传入无效值时会自动设成和视频信号源一致 */
    u32          dwLipCfg;              /* 0表示使用默认值 非0则用该值设入寄存器 */
}TFpgaEncVPParam;

/* 编码通道编码参数结构定义 */
typedef struct{
    u32     dwGopSize;                   /* 定义每dwGopSize帧中有1个I帧，如10表示I帧9P帧 */
    u32     dwIMaxQP;                    /*  I帧Max QP(0-51) */
    u32     dwIMinQP;                    /*  I帧Min QP(0-51) */
    u32     dwPMaxQP;                    /*  P帧Max QP(0-51) */
    u32     dwPMinQP;                    /*  P帧Min QP(0-51) */
    u32     dwBitRate;                   /*  编码比特率,单位：Kbps */
    u32     dwKBitRatePeak;              /*  编码比特率极限值,单位：Kbps，如果低于dwBitRate或大于60Mbps则驱动取默认值30M */
}TFpgaEncParam;

/* 编码通道编码统计结构定义 */
typedef struct{
    u32 dwEncFrames;           /* FPGA编码帧数计数 */
    u32 dwEncIFrames;          /* FPGA编码I帧数计数 */
    u32 dwSubmitFrames;        /* 递交给用户的帧数计数 */     
    u32 dwEncBytes;            /* FPGA编码BYTE总数 */ 
    u32 dwEncKBytes;           /* FPGA编码KBYTE总数，乘上1024加上发送FPGA编码BYTE总数后为总共编码的字节数 */ 
    u32 dwSubmitBytes;         /* 递交给用户的BYTE总数 */
    u32 dwSubmitKBytes;        /* 递交给用户的KBYTE总数，乘上1024加上递交给用户的BYTE总数后为总共递交的字节数 */ 
    u32 dwNoBufErrCnt;         /* 没有可用Buf次数 */
    u32 dwErrFrames;           /* 错误帧数 */ 
    u32 dwLenErrCnt;           /* 数据长度错误包个数 */
    u32 dwNALUErrCnt;          /* NALU内容错误包个数，即头部不为0x00000001 */
}TFpgaEncStat;

/* 编码器OSD结构定义 */
typedef struct{
    BOOL32 bOSDEnable;         /* 0=disable 1=enable */
}TFpgaEncOsdParam;

/* 编码器视频输入接口亮度对比度等参数结构定义 */
typedef struct{
     u32 dwVidInIntf;          /* 参考视频输入接口宏定义 */
     u32 dwVal;                /* 范围00~0x80~0xff */
}TFpgaEncColorParam;


/* VUI配置结构定义 */
typedef struct{
    u32 dwVuiMsk;             /* VUI掩码，填0时关闭该功能; 非0时开启
                                 如: 码流VUI信息定义 FPGA_ENC_VUI_INC_FRAME_RATE */
}TFpgaEncVuiParam;

/* 编码图像中画灰白色矩形块结构定义 */
typedef struct{
    u32    dwBlkId;           /* 矩形块的索引，每个编码通道支持2个矩形块：0~1 */
    u32    dwBlkStartX;       /* 矩形块左上角X坐标(相对编码图像)，必须是偶数 */
    u32    dwBlkStartY;       /* 矩形块左上角Y坐标(相对编码图像)，必须是偶数 */
    u32    dwBlkEndX;         /* 矩形块右下角X坐标(相对编码图像)，必须是偶数 */
    u32    dwBlkEndY;         /* 矩形块右下角Y坐标(相对编码图像)，必须是偶数 */
}TFpgaEncDrawBlk;

/*====================================================================
    函数名      : FpgaEncEngineOpen
    功能        ：FPGA编码器引擎打开。
    输入参数说明：dwDevId: 0~FPGA_ENC_DEV_MAX_NUM-1;
                 ptGblParam: Engine相关的一些全局参数
    返回值说明  ：错误返回NULL；成功返回FPGA Engine控制句柄
---------------------------------------------------------------------*/
HFpgaEncEngine FpgaEncEngineOpen(u32 dwDevId, TFpgaEncGblParam *ptGblParam);

/*====================================================================
    函数名      : FpgaEncEngineClose
    功能        ：FPGA编码器引擎关闭。
    输入参数说明：hEngine: FpgaEncEngineOpen函数返回的句柄;
    返回值说明  ：错误返回FPGA_FAILURE或错误码；成功返回FPGA_SUCCESS
---------------------------------------------------------------------*/
s32 FpgaEncEngineClose(HFpgaEncEngine hEngine);

/*====================================================================
    函数名      : FpgaEncChnlCreate
    功能        ：FPGA编码通道创建。
    输入参数说明：hEngine: FpgaEncEngineOpen函数返回的句柄;
                 ptChnlParam: 编码通道创建的参数
    返回值说明  ：错误返回NULL；成功返回FPGA编码通道控制句柄
---------------------------------------------------------------------*/
HFpgaEncChnl FpgaEncChnlCreate(HFpgaEncEngine hEngine, TFpgaEncChnlCrtParam *ptChnlParam);

/*====================================================================
    函数名      : FpgaEncChnlDelete
    功能        ：FPGA编码通道删除。
    输入参数说明：hEncChnl: FpgaEncChnlCreate函数返回的句柄;
    返回值说明  ：错误返回FPGA_FAILURE或错误码；成功返回FPGA_SUCCESS
---------------------------------------------------------------------*/
s32 FpgaEncChnlDelete(HFpgaEncChnl hEncChnl);

/*====================================================================
    函数名      : FpgaEncChnlVidSrcSel
    功能        ：FPGA编码通道视频源选择,不可动态设置，需要停止解码。
    输入参数说明：hEncChnl: FpgaEncChnlCreate函数返回的句柄;
                 dwVidInIntf: 参考视频输入接口宏定义
    返回值说明  ：错误返回FPGA_FAILURE或错误码；成功返回FPGA_SUCCESS
---------------------------------------------------------------------*/
s32 FpgaEncChnlVidInSel(HFpgaEncChnl hEncChnl, u32 dwVidInIntf);

/*====================================================================
    函数名      : FpgaEncChnlVidStdGet
    功能        ：通过hEncChnl查询指定视频接口的信号信息。
    输入参数说明：hEncChnl: FpgaEncChnlCreate函数返回的句柄;
                 dwVidInIntf: 参考视频输入接口宏定义
                 ptVidInfo: 返回的视频信息
    返回值说明  ：错误返回FPGA_FAILURE或错误码；成功返回FPGA_SUCCESS
---------------------------------------------------------------------*/
s32 FpgaEncChnlVidStdGet(HFpgaEncChnl hEncChnl, u32 dwVidInIntf, TFpgaVidInfo *ptVidInfo);

/*====================================================================
    函数名      : FpgaEncChnlVPParamSet
    功能        ：FPGA编码通道VP口采集参数设置，视频源信号发生改变后需要
                 用户在先停止编码，使用新的视频信号参数来重新配置VP口采集
                 参数，之后再开始编码。
    输入参数说明：hEncChnl: FpgaEncChnlCreate函数返回的句柄;
                 ptVPParam: VP口采集参数
    返回值说明  ：错误返回FPGA_FAILURE或错误码；成功返回FPGA_SUCCESS
---------------------------------------------------------------------*/
s32 FpgaEncChnlVPParamSet(HFpgaEncChnl hEncChnl, TFpgaEncVPParam *ptVPParam);

/*====================================================================
    函数名      : FpgaEncChnlEncParamSet
    功能        ：FPGA编码通道编码参数设置,可动态设置，不需要停止编码。
    输入参数说明：hEncChnl: FpgaEncChnlCreate函数返回的句柄;
                 ptEncParam: 编码参数
    返回值说明  ：错误返回FPGA_FAILURE或错误码；成功返回FPGA_SUCCESS
---------------------------------------------------------------------*/
s32 FpgaEncChnlEncParamSet(HFpgaEncChnl hEncChnl, TFpgaEncParam *ptEncParam);

/*====================================================================
    函数名      : FpgaEncChnlEncStart
    功能        ：FPGA编码通道编码开始。
    输入参数说明：hEncChnl: FpgaEncChnlCreate函数返回的句柄;
    返回值说明  ：错误返回FPGA_FAILURE或错误码；成功返回FPGA_SUCCESS
---------------------------------------------------------------------*/
s32 FpgaEncChnlEncStart(HFpgaEncChnl hEncChnl);

/*====================================================================
    函数名      : FpgaEncChnlEncStop
    功能        ：FPGA编码通道编码停止。
    输入参数说明：hEncChnl: FpgaEncChnlCreate函数返回的句柄;
    返回值说明  ：错误返回FPGA_FAILURE或错误码；成功返回FPGA_SUCCESS
---------------------------------------------------------------------*/
s32 FpgaEncChnlEncStop(HFpgaEncChnl hEncChnl);

/*====================================================================
    函数名      : FpgaEncChnlForceI
    功能        ：FPGA编码通道强制I帧。
    输入参数说明：hEncChnl: FpgaEncChnlCreate函数返回的句柄;
    返回值说明  ：错误返回FPGA_FAILURE或错误码；成功返回FPGA_SUCCESS
---------------------------------------------------------------------*/
s32 FpgaEncChnlForceI(HFpgaEncChnl hEncChnl);

/*====================================================================
    函数名      : FpgaEncChnlCtrl
    功能        ：FPGA编码通道控制，目前还没有定义，以后需要时可以增加功能。
    输入参数说明：hEncChnl: FpgaEncChnlCreate函数返回的句柄;
                 nCmd: 操作码；pArgs: 参数指针
    返回值说明  ：错误返回FPGA_FAILURE或错误码；成功返回FPGA_SUCCESS
---------------------------------------------------------------------*/
s32 FpgaEncChnlCtrl(HFpgaEncChnl hEncChnl, s32 nCmd, void *pArgs);

/*====================================================================
    函数名      : FpgaDbgGetVidInStd
    功能        ：根据hEngine查询指定视频接口的信号信息。
    输入参数说明：hEngine: FpgaEncEngineOpen函数返回的句柄;
                 dwVidInIntf: 参考视频输入接口宏定义
                 ptVidInfo: 返回的视频信息
    返回值说明  ：错误返回FPGA_FAILURE或错误码；成功返回FPGA_SUCCESS
--------------------------------------------------------------------*/
s32 FpgaDbgGetVidInStd(HFpgaEncEngine hEngine, u32 dwVidInIntf, TFpgaVidInfo *ptVidInfo);

/*====================================================================
    函数名      : FpgaDbgSetVidInStd
    功能        ：调试用，强行指定输入接口的视频制式。
    输入参数说明：hEngine: FpgaEncEngineOpen函数返回的句柄;
                 dwVidInIntf: 视频输入接口；
                 ptVidInfo: 视频制式;
    返回值说明  ：错误返回FPGA_FAILURE或错误码；成功返回FPGA_SUCCESS
--------------------------------------------------------------------*/
s32 FpgaDbgSetVidInStd(HFpgaEncEngine hEngine, u32 dwVidInIntf, TFpgaVidInfo *ptVidInfo);

/*====================================================================
    函数名      : FpgaCodecGetVer
    功能        ：codec模块版本号查询。
    输入参数说明：pchVer： 给定的存放版本信息的buf指针
                 dwBufLen：给定buf的长度
    返回值说明  ：版本的实际字符串长度。小于0为出错;
                 如果实际字符串长度大于dwBufLen，赋值为0
---------------------------------------------------------------------*/
s32 FpgaCodecGetVer(s8 *pchVer, u32 dwBufLen);

/*====================================================================
    函数名      : FpgaEncIsDied
    功能        ：查询编码器是否异常或死机
    输入参数说明：hEngine: FpgaEncEngineOpen函数返回的句柄;
    返回值说明  ：编码器异常或死机返回TRUE，否则返回FALSE
--------------------------------------------------------------------*/
BOOL32 FpgaEncIsDied(HFpgaEncEngine hEngine);

/*====================================================================
    函数名      : FpgaDbgSetRange
    功能        ：设置运动矢量
    输入参数说明：输入u32值直接写入register;
    返回值说明  ：编码器异常或死机返回TRUE，否则返回FALSE
--------------------------------------------------------------------*/
s32 FpgaDbgSetRange(HFpgaEncEngine hEngine, u32 range);

/*====================================================================
    函数名      : FpgaDbgSelVidInSrc
    功能        ：指定输入接口的视频源。
    输入参数说明：hEngine: FpgaEncEngineOpen函数返回的句柄;
                 dwToVP: 视频输出接口；
                 dwFromVP: 为0xffffffff表示从物理输入口输入,此时必须填充有效
                           的dwVidInIntf参数, 为0~2表示从相应的解码器
                           的VP口获取视频数据,dwVidInIntf无效;
                 dwVidInIntf: 物理输入接口宏定义
    返回值说明  ：错误返回FPGA_FAILURE或错误码；成功返回FPGA_SUCCESS
--------------------------------------------------------------------*/
s32 FpgaDbgSelVidInSrc(HFpgaEncEngine hEngine, u32 dwToVP, u32 dwFromVP, u32 dwVidInIntf);

#ifdef __cplusplus 
} 
#endif /* __cplusplus */

#endif /* __FPGA_ENC_API_H */
