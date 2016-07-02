/******************************************************************************
模块名  ： TAOS_CODEC_DRV
文件名  ： taos_codec_api.h
相关文件： taos_dev_api.c taos_enc_api.c taos_dec_api.c
文件实现功能：
作者    ：张方明
版本    ：1.0.0.0.0
-------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
12/04/2008  1.0         张方明      创建
******************************************************************************/
#ifndef __TAOS_CODEC_API_H
#define __TAOS_CODEC_API_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "brdwrapper.h"         /* 为了包含视频相关的宏定义和结构定义 */
#include "taos_codec_def.h"


/* 极限值定义 */

/* TaosCtrl 控制命令定义  */
#define TAOS_CTL_GET_ENC_STAT            0x0000  /* 查询TAOS编码模块信息，和TTaosEncStat配对使用 */
#define TAOS_CTL_GET_DEC_STAT            0x0001  /* 查询TAOS解码模块信息，和TTaosDecStat配对使用 */
#define TAOS_CTL_SET_ENC_IIPCFG          0x0100  /* 设置编码器IipCfg寄存器，pArgs指向(u32 *) */
#define TAOS_CTL_SET_OSD_CFG          	 0x0101  /* 设置TAOS的OSD配置，和TTaosOsdCfg配对使用 */
#define TAOS_CTL_SET_VUI_CFG          	 0x0102  /* 设置TAOS的码流VUI信息，和TTaosVuiCfg配对使用 */
#define TAOS_CTL_DRAW_BLOCK              0x0103  /* 编码图像指定位置处画一个灰白色矩形块，和TTaosDrawBlk配对使用 */

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
#define TAOS_ENC_IIP_4x4_DISABLE    0xff0f80fa
#define TAOS_ENC_IIP_PINTRAENABLE   (1 << 0)


/*____________________________公共部分结构定义_______________________*/

/* 类型定义 */
typedef void * HTaosEngine;
typedef void * HTaosEncChnl;
typedef void * HTaosDecChnl;

/* 编码通道回调函数类型定义 */
typedef void (*TTaosEncChnlCallBack)(HTaosEngine hEngine, TTaosFrameDesc *ptFrame, u8 *pbyContext);


/* Buf参数结构定义 */
typedef struct{
    u32     adwVidWidthMax[TAOS_CHNL_MAX_NUM];
    u32     adwVidHightMax[TAOS_CHNL_MAX_NUM];/* 视频最大宽度和高度，用于分配每个通道内存，填0表示按1080p(1920x1088)计算Buf大小
                                               所有的编码和解码通道都使用该参数，要修改必须关闭TAOS重新打开 */

    u32     adwFrmBufLen[TAOS_BUF_POOL_MAX];/* 每组POOL中帧buf长度，多组POOL时要求Pool0~Poolx的长度从小到大排序。
                                               一般1帧编出来的数据长度最高为期望码率的1/4，
                                               如最高要求码率为8Mbps,则该值应该设为8/8/4=250KBytes，目前建议512KBytes */
    u32     adwFrmBufCnt[TAOS_BUF_POOL_MAX];/* 每组POOL中帧buf个数，有效值至少2个, 为0表示POOL结束 */

    u32     dwChnlMaxNum;                   /* 最多创建的通道个数 */
} TTaosBufParam;

/* 全局参数结构定义 */
typedef struct{
    BOOL32  bDoBist;                        /* 是否让TAOS进行内存自检 */
    u32     dwIipCfg;                       /* 0表示使用默认值 非0则用该值设入寄存器，请参考:IipCfg寄存器定义 */
    u32     dwCodecMode;                    /* 编解码工作模式，为0表示AUTO，请参考: TAOS工作模式类型宏定义 */

    TTaosBufParam   *ptEncBufParam;         /* 为NULL表示没有编码应用，驱动不做编码相关初始化和内存分配， */ 
    TTaosBufParam   *ptDecBufParam;         /* 为NULL表示没有解码应用，驱动不做解码相关初始化和内存分配， */ 
} TTaosGblParam;


/* VP口的参数结构定义 */
typedef struct{
    u32     dwVPId;                         /* VP口索引，0~TAOS_VP_MAX_NUM-1 */
    u32     dwIoType;                       /* 输入输出方向，更多参数如: VP口输入输出类型宏定义
                                               当设置为TAOS_VP_DISABLE时其他参数无效;
                                               当设置为TAOS_VP_CLONE时dwInterface和tVidStd无效，
                                                    驱动自动跟随VP0/4,之前必须先配置好VP0/4 */
    u32     dwInterface;                    /* 对应的视频接口名称，如: 视频输入输出接口类型宏定义 */
    TVidStd tVidStd;                        /* 视频信号制式 */

    u32     dwLeftCropPixs;                 /* 源图左边切掉n个象素，整个图像左移 !注意：必须是偶数! */
    u32     dwTopCropLines;                 /* 源图顶部切掉n行，整个图像上移 */
    u32	    dwScaledWidth;                  /* 目前仅输入模式时支持DownScal，其他模式无效，像素为单位 */
    u32	    dwScaledHeight;                 /* 目前仅输入模式时支持DownScal，其他模式无效，行为单位 */

    u32     dwDdrBankSel;                   /* 该VP口数据存放在哪个内存Bank中，如: TAOS内存bank定义
                                               如果没有特殊要求就用 TAOS_DDR_BANK_AUTO由驱动自动分配 */
}TTaosVPParam;

/* 图像窗口结构定义 */
typedef struct{
    u32     dwVPId;                         /* 占用的VP口，需配合相应的硬件设计，0~TAOS_VP_MAX_NUM-1 */
    u32     dwPipId;                        /* 子画面的索引，0~TAOS_PIP_MAX_NUM-1 */
    u32     dwPipLeft;                      /* 子画面X坐标，必须是16*8=128的整数倍 */
    u32     dwPipTop;                       /* 子画面Y坐标，必须是16的整数倍 */
    u32     dwPipWidth;                     /* 子画面宽度，必须是16的整数倍，填0表示全屏 */
    u32     dwPipHight;                     /* 子画面高度，必须是16的整数倍，填0表示全屏 */
} TTaosWinParam;

/* OSD结构定义 */
typedef struct{
    BOOL32  bEncoder;                       /* TRUE＝编码端；FALSE＝解码端 */
    u32     dwChnlId;                       /* 通道的索引，0~TAOS_CHNL_MAX_NUM-1 */
    BOOL32  bOSDEnable;                     /* 0=disable 1=enable */
}TTaosOsdCfg;

/* VUI配置结构定义 */
typedef struct{
    u32     dwChnlId;                       /* 通道的索引，0~TAOS_CHNL_MAX_NUM-1 */
    u32     dwVuiMsk;                       /* VUI掩码，填0时关闭该功能; 非0时开启
                                               如: 码流VUI信息定义 TAOS_VUI_INC_FRAME_RATE */
}TTaosVuiCfg;

/* 编码图像中画灰白色矩形块结构定义 */
typedef struct{
    u32     dwChnlId;                       /* 通道的索引，0~TAOS_CHNL_MAX_NUM-1 */
    u32     dwBlkId;                        /* 矩形块的索引，每个编码通道支持2个矩形块：0~1 */
    u32     dwBlkStartX;                    /* 矩形块左上角X坐标(相对编码图像)，必须是偶数 */
    u32     dwBlkStartY;                    /* 矩形块左上角Y坐标(相对编码图像)，必须是偶数 */
    u32     dwBlkEndX;                      /* 矩形块右下角X坐标(相对编码图像)，必须是偶数 */
    u32     dwBlkEndY;                      /* 矩形块右下角Y坐标(相对编码图像)，必须是偶数 */
}TTaosDrawBlk;

/*____________________________编码部分结构定义_______________________*/


/* 编码通道创建的参数结构定义 */
typedef struct{
    u32     dwChnlId;                       /* 编码通道的索引，0~TAOS_CHNL_MAX_NUM-1 */

    TTaosWinParam tWin;                     /* 编码图像窗口信息,tWin.dwVPId可以扩展,见:编码图像窗口信息tWin.dwVPId扩展掩码 */

    u32     dwReqFrameRate;                 /* 编码帧率，或上TAOS_VUI_INC_FRAME_RATE后表示码流sps中包含帧率信息 */
    u32     dwBytesPerSlice;                /* 定义每个slice的字节数，最大65535，默认设成1400, 如果设
                                               为0表示使用单slice方式 */
    TTaosEncChnlCallBack tFrmCallBack;      /* 回调函数指针 */
    u8      *pbyContext;                    /* 用户私有参数，回调函数回调时将该值原样返回给用户 */
} TTaosEncChnlCrtParam;

/* 编码通道编码参数结构定义 */
typedef struct{
    u32     dwChnlId;                       /* 编码通道的索引，0~TAOS_CHNL_MAX_NUM-1 */
    u32     dwGopSize;                      /* 定义每dwGopSize帧中有1个I帧，如10表示I帧9P帧 */
    u32     dwIMaxQP;                       /*  I帧Max QP(0-51) */
    u32     dwIMinQP;                       /*  I帧Min QP(0-51) */
    u32     dwPMaxQP;                       /*  P帧Max QP(0-51) */
    u32     dwPMinQP;                       /*  P帧Min QP(0-51) */
    u32     dwBitRate;                      /*  编码比特率,单位：Kbps */
    u32     dwKBitRatePeak;                 /*  编码比特率极限值,单位：Kbps，
                                              # 最高位置0表示速度优先:
                                                I帧和前后P帧的大小差不多，因此QP值变化太大，导致编I帧
                                                时图像发虚严重，但是其优点是码率非常均匀，适合物理线路带宽严格限制的场合，如E1线路
                                              # 最高位置1表示质量优先:
                                                I帧大小会变大，其QP值和前后P帧变化小，以减弱编I帧时
                                                图像发虚严重的问题，但是码率会略微有些突发，不过总
                                                体不会突很多,比特率极限值可以设置到40M左右,值越大发虚越不明显，
                                                为防止fifo溢出最小QP值不要小于15 */
}TTaosEncParam;

/* 编码通道编码统计结构定义 */
typedef struct{
    u32     dwChnlId;                       /* 编码通道的索引，0~TAOS_CHNL_MAX_NUM-1 */
    u32     dwEncFrames;                    /* TAOS编码帧数计数 */
    u32     dwEncIFrames;                   /* TAOS编码I帧数计数 */
    u32     dwSubmitFrames;                 /* 递交给用户的帧数计数 */     
    u32     dwEncBytes;                     /* TAOS编码BYTE总数 */ 
    u32     dwEncKBytes;                    /* TAOS编码KBYTE总数，乘上1024加上发送TAOS编码BYTE总数后为总共编码的字节数 */ 
    u32     dwSubmitBytes;                  /* 递交给用户的BYTE总数 */
    u32     dwSubmitKBytes;                 /* 递交给用户的KBYTE总数，乘上1024加上递交给用户的BYTE总数后为总共递交的字节数 */ 
    u32     dwNoBufErrCnt;                  /* 没有可用Buf次数 */
    u32     dwErrFrames;                    /* 错误帧数 */ 
    u32     dwLenErrCnt;                    /* 数据长度错误包个数 */
    u32     dwFrameLenMax;                  /* 最大数据帧的长度 */
    u32     dwNALUErrCnt;                   /* NALU内容错误包个数，即头部不为0x00000001 */
}TTaosEncStat;


/*____________________________解码部分结构定义_______________________*/

/* 解码通道创建的参数结构定义 */
typedef struct{
    u32     dwChnlId;                       /* 解码通道的索引，0~TAOS_CHNL_MAX_NUM-1 */

    u32     dwWinNum;                       /* 解码图像输出窗口个数，范围0~TAOS_VP_MAX_NUM*TAOS_PIP_MAX_NUM-1 */
    TTaosWinParam atWin[TAOS_VP_MAX_NUM*TAOS_PIP_MAX_NUM]; /* 解码图像输出窗口信息 */

    u32     dwSpeedMb;                      /* 解码节奏控制，这只能在解一路码流的情况下控制，当有多路码流时必须设置为0!
                                               0=立即解下一帧，1~65535=用户指定解完当前帧后等待多少宏块时间
                                               后再解下一帧；TAOS_SPEEDMB_CAL_AUTO=驱动自动计算； */
}TTaosDecChnlCrtParam;

/* 解码通道解码参数结构定义 */
typedef struct{
    u32     dwChnlId;                       /* 解码通道的索引，0~TAOS_CHNL_MAX_NUM-1 */
    u32     dwSpeedMb;                      /* 解码节奏控制，这只能在解一路码流的情况下控制，当有多路码流时必须设置为0!
                                                0=立即解下一帧，1~65535=用户指定解完当前帧后等待多少宏块时间
                                                后再解下一帧；TAOS_SPEEDMB_CAL_AUTO=驱动自动计算； */
    TVidStd tDecVidStd;                     /* 解码码流视频分辨率 */
}TTaosDecParam;

/* 解码通道解码统计结构定义 */
typedef struct{
    u32     dwChnlId;                       /* 解码通道的索引，0~TAOS_CHNL_MAX_NUM-1 */
    u32     dwSendFrames;                   /* 用户发送帧数计数 */     
    u32     dwDecFrames;                    /* 驱动解码帧数计数 */
    u32     dwSendBytes;                    /* 发送BYTE总数 */ 
    u32     dwSendKBytes;                   /* 发送KBYTE总数，乘上1024加上发送BYTE总数后为总共发送的字节数 */ 
    u32     dwDecBytes;                     /* 驱动解码BYTE总数 */
    u32     dwDecKBytes;                    /* 驱动解码KBYTE总数，乘上1024加上驱动解码BYTE总数后为总共解码的字节数 */ 
    u32     dwNoBufErrCnt;                  /* 没有可用Buf次数 */
    u32     dwErrFrames;                    /* 错误帧数 */
    u32     dwDropFrames;                   /* 驱动解码丢帧计数 */
    u32     dwVldErrCnt;                    /* VLD错误次数 */
    u32     dwNalErrCnt;                    /* Nal错误次数 */
    u32     dwBufedFrames;                  /* 驱动缓冲的待解码帧数 */
}TTaosDecStat;



/*____________________________公共部分函数定义_______________________*/

/*====================================================================
    函数名      : TaosEngineOpen
    功能        ：TAOS设备引擎打开。
    输入参数说明：dwDevId: 0~TAOS_DEV_MAX_NUM-1;
                  ptGblParam: Engine相关的一些全局参数；
    返回值说明  ：错误返回NULL；成功返回设备控制句柄
---------------------------------------------------------------------*/
HTaosEngine TaosEngineOpen(u32 dwDevId, TTaosGblParam *ptGblParam);

/*====================================================================
    函数名      : TaosEngineClose
    功能        ：TAOS设备引擎关闭。
    输入参数说明：hEngine: TaosEngineOpen函数返回的句柄;
    返回值说明  ：错误返回TAOS_FAILURE或错误码；成功返回TAOS_SUCCESS
---------------------------------------------------------------------*/
s32 TaosEngineClose(HTaosEngine hEngine);

/*====================================================================
    函数名      : TaosVPParamSet
    功能        ：TAOS的VP口参数设置,不可动态设置，需要停止绑定通道的
                  编码或解码后才能设置!
    输入参数说明：hEngine: TaosEngineOpen函数返回的句柄;
                  ptVPParam: VP口播放参数
    返回值说明  ：错误返回TAOS_FAILURE或错误码；成功返回TAOS_SUCCESS
---------------------------------------------------------------------*/
s32 TaosVPParamSet(HTaosEngine hEngine, TTaosVPParam *ptVPParam);

/*====================================================================
    函数名      : TaosCtrl
    功能        ：TAOS功能控制
    输入参数说明：hDecChnl: TaosChnlCreate函数返回的句柄;
                  nCmd: 操作码；pArgs: 参数指针
    返回值说明  ：错误返回TAOS_FAILURE或错误码；成功返回TAOS_SUCCESS
---------------------------------------------------------------------*/
s32 TaosCtrl(HTaosEngine hEngine, s32 nCmd, void *pArgs);

/*====================================================================
    函数名      : TaosIsDied
    功能        ：查询设备是否异常或死机
    输入参数说明：hEngine: TaosEncEngineOpen函数返回的句柄;
    返回值说明  ：设备异常或死机返回TRUE，否则返回FALSE
--------------------------------------------------------------------*/
BOOL32 TaosIsDied(HTaosEngine hEngine);

/*====================================================================
    函数名      : TaosGetDrvVer
    功能        ：Taos模块版本号查询。
    输入参数说明：pchVer： 给定的存放版本信息的buf指针
                 dwBufLen：给定buf的长度
    返回值说明  ：版本的实际字符串长度。小于0为出错;
                 如果实际字符串长度大于dwBufLen，赋值为0
---------------------------------------------------------------------*/
s32 TaosGetDrvVer(s8 *pchVer, u32 dwBufLen);


/*____________________________编码部分函数定义_______________________*/

/*====================================================================
    函数名      : TaosEncChnlCreate
    功能        ：TAOS编码通道创建。
    输入参数说明：hEngine: TaosEngineOpen函数返回的句柄;
                  ptChnlParam: 编码通道创建的参数
    返回值说明  ：错误返回NULL；成功返回编码通道控制句柄
---------------------------------------------------------------------*/
HTaosEncChnl TaosEncChnlCreate(HTaosEngine hEngine, TTaosEncChnlCrtParam *ptChnlParam);

/*====================================================================
    函数名      : TaosEncChnlDelete
    功能        ：TAOS编码通道删除。
    输入参数说明：hEncChnl: TaosEncChnlCreate函数返回的句柄;
    返回值说明  ：错误返回TAOS_FAILURE或错误码；成功返回TAOS_SUCCESS
---------------------------------------------------------------------*/
s32 TaosEncChnlDelete(HTaosEncChnl hEncChnl);

/*====================================================================
    函数名      : TaosEncChnlParamSet
    功能        ：TAOS编码通道编码参数设置,可动态设置，不需要停止编码。
    输入参数说明：hEncChnl: TaosEncChnlCreate函数返回的句柄;
                  ptEncParam: 编码参数
    返回值说明  ：错误返回TAOS_FAILURE或错误码；成功返回TAOS_SUCCESS
---------------------------------------------------------------------*/
s32 TaosEncChnlParamSet(HTaosEncChnl hEncChnl, TTaosEncParam *ptEncParam);

/*====================================================================
    函数名      : TaosEncChnlStart
    功能        ：TAOS编码通道编码开始。
    输入参数说明：hEncChnl: TaosEncChnlCreate函数返回的句柄;
    返回值说明  ：错误返回TAOS_FAILURE或错误码；成功返回TAOS_SUCCESS
---------------------------------------------------------------------*/
s32 TaosEncChnlStart(HTaosEncChnl hEncChnl);

/*====================================================================
    函数名      : TaosEncChnlStop
    功能        ：TAOS编码通道编码停止。
    输入参数说明：hEncChnl: TaosEncChnlCreate函数返回的句柄;
    返回值说明  ：错误返回TAOS_FAILURE或错误码；成功返回TAOS_SUCCESS
---------------------------------------------------------------------*/
s32 TaosEncChnlStop(HTaosEncChnl hEncChnl);

/*====================================================================
    函数名      : TaosEncChnlForceI
    功能        ：TAOS编码通道强制I帧。
    输入参数说明：hEncChnl: TaosEncChnlCreate函数返回的句柄;
    返回值说明  ：错误返回TAOS_FAILURE或错误码；成功返回TAOS_SUCCESS
---------------------------------------------------------------------*/
s32 TaosEncChnlForceI(HTaosEncChnl hEncChnl);


/*____________________________解码部分函数定义_______________________*/

/*====================================================================
    函数名      : TaosDecChnlCreate
    功能        ：TAOS解码通道创建。
    输入参数说明：hEngine: TaosEngineOpen函数返回的句柄;
                  ptChnlParam: 解码通道创建的参数
    返回值说明  ：错误返回NULL；成功返回解码通道控制句柄
---------------------------------------------------------------------*/
HTaosDecChnl TaosDecChnlCreate(HTaosEngine hEngine, TTaosDecChnlCrtParam *ptChnlParam);

/*====================================================================
    函数名      : TaosDecChnlDelete
    功能        ：TAOS解码通道删除。
    输入参数说明：hDecChnl: TaosDecChnlCreate函数返回的句柄;
    返回值说明  ：错误返回TAOS_FAILURE或错误码；成功返回TAOS_SUCCESS
---------------------------------------------------------------------*/
s32 TaosDecChnlDelete(HTaosDecChnl hDecChnl);

/*====================================================================
    函数名      : TaosDecChnlParamSet
    功能        ：TAOS解码通道解码参数设置,可动态设置，不需要停止解码。
    输入参数说明：hDecChnl: TaosDecChnlCreate函数返回的句柄;
                  ptDecParam: 解码参数
    返回值说明  ：错误返回TAOS_FAILURE或错误码；成功返回TAOS_SUCCESS
---------------------------------------------------------------------*/
s32 TaosDecChnlParamSet(HTaosDecChnl hDecChnl, TTaosDecParam *ptDecParam);

/*====================================================================
    函数名      : TaosDecChnlStart
    功能        ：TAOS解码通道解码开始。
    输入参数说明：hDecChnl: TaosDecChnlCreate函数返回的句柄;
    返回值说明  ：错误返回TAOS_FAILURE或错误码；成功返回TAOS_SUCCESS
---------------------------------------------------------------------*/
s32 TaosDecChnlStart(HTaosDecChnl hDecChnl);

/*====================================================================
    函数名      : TaosDecChnlStop
    功能        ：TAOS解码通道解码停止。
    输入参数说明：hDecChnl: TaosDecChnlCreate函数返回的句柄;
    返回值说明  ：错误返回TAOS_FAILURE或错误码；成功返回TAOS_SUCCESS
---------------------------------------------------------------------*/
s32 TaosDecChnlStop(HTaosDecChnl hDecChnl);

/*====================================================================
    函数名      : TaosDecChnlBufGet
    功能        ：TAOS解码Buf申请。
    输入参数说明：hDecChnl: TaosDecChnlCreate函数返回的句柄;
                  ptBufDesc : 指向存放一个空的帧Buf描述的地址，用户
                  分配并传入指针。
                  nTimeout: 超时值，0 = no wait, -1 = wait forever, 
                                   >0 = 毫秒
    返回值说明  ：错误返回TAOS_FAILURE或错误码，没有可用Buf时
                 返回 TAOS_NO_AVAILABLE_BUF, 并且清空结构变量
                 成功返回TAOS_SUCCESS
---------------------------------------------------------------------*/
s32 TaosDecChnlBufGet(HTaosDecChnl hDecChnl, TTaosFrameDesc *ptBufDesc, s32 nTimeout);

/*====================================================================
    函数名      : TaosDecChnlBufPut
    功能        ：TAOS解码Buf递交。
    输入参数说明：hDecChnl: TaosChnlCreate函数返回的句柄;
                  ptBufDesc : 指向填满帧数据的帧Buf描述的地址，
                  必须是由TaosDecChnlBufGet获得的。
    返回值说明  ：错误返回TAOS_FAILURE或错误码
                 成功返回TAOS_SUCCESS
---------------------------------------------------------------------*/
s32 TaosDecChnlBufPut(HTaosDecChnl hDecChnl, TTaosFrameDesc *ptBufDesc);


#ifdef __cplusplus 
} 
#endif /* __cplusplus */

#endif /* __TAOS_CODEC_API_H */
