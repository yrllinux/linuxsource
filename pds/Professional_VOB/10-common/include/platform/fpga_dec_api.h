/******************************************************************************
模块名  ： FPGA_DEC_DRV
文件名  ： fpga_dec_api.h
相关文件： fpga_dec_api.c
文件实现功能：
作者    ：张方明
版本    ：1.0.0.0.0
-------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
08/13/2007  1.0         张方明      创建
******************************************************************************/
#ifndef __FPGA_DEC_API_H
#define __FPGA_DEC_API_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "fpga_codec_def.h"


/* 极限值定义 */
#define FPGA_DEC_DEV_MAX_NUM             1              /* 目前一个主机上最大支持1个FPGA解码芯片 */
#define FPGA_DEC_CHNL_MAX_NUM            32             /* 目前每个解码器芯片最大支持32个解码通道 */
#define FPGA_DEC_VP_MAX_NUM              4              /* 目前每个解码器芯片最大支持4个VP视频口 */
#define FPGA_DEC_NALU_MAX_NUM            512            /* 目前1帧数据中最大支持512个NALU单元 */

/* 视频输出接口宏定义 */
#define VIDOUT_HDMI0                    0x00000001
#define VIDOUT_VGA0                     0x00000010
#define VIDOUT_YPbPr0                   0x00000100
#define VIDOUT_YPbPr1                   0x00000200      /* 在TS-7180上该接口和VIDOUT_VGA0冲突，只能2选1 */
#define VIDOUT_SDI0                     0x00001000
#define VIDOUT_C0                       0x00010000
#define VIDOUT_S0                       0x00100000
#define VIDOUT_DVI0                     0x01000000

/* 辅视频输出VP口宏定义 */
#define FPGA_DEC_SUB_NONE                   0      /* 没有辅输出 */
#define FPGA_DEC_SUB_VP0                    1      /* 辅视频输出使用VP0 */
#define FPGA_DEC_SUB_VP1                    2      /* 辅视频输出使用VP1 */
#define FPGA_DEC_SUB_VP2                    3      /* 辅视频输出使用VP2 */
#define FPGA_DEC_SUB_VP3                    4      /* 辅视频输出使用VP3 */

/* FpgaDecChnlCtrl控制命令定义  */
#define FPGA_DEC_CTL_GET_STAT            0              /* 查询解码器统计信息，和TFpgaDecStat配对使用 */

/* 解码SpeedMb定义 */
#define FPGA_DEC_SPEEDMB_CAL_AUTO       0x00010000  /* 由驱动自动计算 */

/* 类型定义 */
typedef void * HFpgaDecEngine;
typedef void * HFpgaDecChnl;

/* 全局参数结构定义 */
typedef struct{
    s8  abyFirmwareName[FPGA_STR_MAX_LEN]; /* FPGA二进制文件的名字，支持绝对路径，
                                              如:"/usr/bin/fpga.bin",不指定的话在当前路径下找 */
    u32 dwFrmBufMaxLen;             /* 一般1帧编出来的数据长度最高为期望码率的1/4，如最高要求
                                       码率为8Mbps,则该值应该设为8/8/4=250KBytes，目前建议512KBytes */
    u32 dwFrmBufCnt;                /* 帧缓冲的个数，至少2个 */
} TFpgaDecGblParam;

/* 解码通道创建的参数结构定义 */
typedef struct{
    u32 dwChnlId;                       /* 解码通道的索引，0~FPGA_DEC_CHNL_MAX_NUM-1 */
    u32 dwVPId;                         /* 占用的VP口，一个chnl最多同时从4个VP口输出。
                                           32位拆分为4个Byte,每个byte对应一个视频输出口:
                                            Bit[ 7: 0]为主视频输出: 范围0~FPGA_DEC_VP_MAX_NUM-1,分别对应VP0/1/2/3；
                                            Bit[15: 8]为辅视频输出1；
                                            Bit[23:16]为辅视频输出2；
                                            Bit[31:24]为辅视频输出3；
                                           3个辅视频输出段采用宏[辅视频输出VP口宏定义]来指示对应的VP号，
                                           FPGA_DEC_SUB_NONE表示对应辅视频输出关闭，
                                           其他几个宏表示该辅视频输出从相应的VP口出 */
} TFpgaDecChnlCrtParam;


/* 解码通道VP口的参数结构定义 */
typedef struct{
    TFpgaVidInfo tVidInfo;              /* 视频信号信息 */
    u32 dwPipId;                        /* 小画面的索引，0~3 */
    u32 dwPipLeft;                      /* 小画面X坐标，必须是16*8=128的整数倍 */
    u32 dwPipTop;                       /* 小画面Y坐标，必须是16的整数倍 */
    u32 dwPipWidth;                     /* 小画面宽度，必须是16的整数倍，填0表示全屏 */
    u32 dwPipHight;                     /* 小画面高度，必须是16的整数倍，填0表示全屏 */

    u32 dwWidthMax;
    u32 dwHightMax;                     /* VP窗口最大宽度和高度，必须是16的整数倍，填0表示按1080p(1920x1088)计算Buf大小
                                           用户所有的解码通道必须使用相同的参数，要修改必须关闭解码器重新打开 */
    int notSetDA;                       /* 0表示在st_vp中设置DA制式，1表示在set_vp中设置DA */
}TFpgaDecVPParam;

/* 解码通道解码参数结构定义 */
typedef struct{
    u32             dwSpeedMb;          /* 解码节奏控制，这只能在解一路码流的情况下控制，当有多路码流时必须设置为0!
                                           0=立即解下一帧，1~65535=用户指定解完当前帧后等待多少宏块时间
                                           后再解下一帧；FPGA_DEC_SPEEDMB_CAL_AUTO=驱动自动计算； */
    TFpgaVidInfo    tDecVidStd;         /* 解码码流视频分辨率 */
}TFpgaDecParam;

/* 解码通道解码统计结构定义 */
typedef struct{
    u32 dwSendFrames;          /* 用户发送帧数计数 */     
    u32 dwDecFrames;           /* 驱动解码帧数计数 */
    u32 dwSendBytes;           /* 发送BYTE总数 */ 
    u32 dwSendKBytes;          /* 发送KBYTE总数，乘上1024加上发送BYTE总数后为总共发送的字节数 */ 
    u32 dwDecBytes;            /* 驱动解码BYTE总数 */
    u32 dwDecKBytes;           /* 驱动解码KBYTE总数，乘上1024加上驱动解码BYTE总数后为总共解码的字节数 */ 
    u32 dwNoBufErrCnt;         /* 没有可用Buf次数 */
    u32 dwErrFrames;           /* 错误帧数 */
    u32 dwVldErrCnt;           /* VLD错误次数 */
    u32 dwNalErrCnt;           /* Nal错误次数 */
}TFpgaDecStat;


/*====================================================================
    函数名      : FpgaDecEngineOpen
    功能        ：FPGA解码器引擎打开。
    输入参数说明：dwDevId: 0~FPGA_DEC_DEV_MAX_NUM-1;
                 ptGblParam: Engine相关的一些全局参数
    返回值说明  ：错误返回NULL；成功返回FPGA Engine控制句柄
---------------------------------------------------------------------*/
HFpgaDecEngine FpgaDecEngineOpen(u32 dwDevId, TFpgaDecGblParam *ptGblParam);

/*====================================================================
    函数名      : FpgaDecEngineClose
    功能        ：FPGA解码器引擎关闭。
    输入参数说明：hEngine: FpgaDecEngineOpen函数返回的句柄;
    返回值说明  ：错误返回FPGA_DEC_FAILURE或错误码；成功返回FPGA_DEC_SUCCESS
---------------------------------------------------------------------*/
s32 FpgaDecEngineClose(HFpgaDecEngine hEngine);

/*====================================================================
    函数名      : FpgaDecChnlCreate
    功能        ：FPGA解码通道创建。
    输入参数说明：hEngine: FpgaDecEngineOpen函数返回的句柄;
                 ptChnlParam: 解码通道创建的参数
    返回值说明  ：错误返回NULL；成功返回FPGA Engine控制句柄
---------------------------------------------------------------------*/
HFpgaDecChnl FpgaDecChnlCreate(HFpgaDecEngine hEngine, TFpgaDecChnlCrtParam *ptChnlParam);

/*====================================================================
    函数名      : FpgaDecChnlDelete
    功能        ：FPGA解码通道删除。
    输入参数说明：hDecChnl: FpgaDecChnlCreate函数返回的句柄;
    返回值说明  ：错误返回FPGA_DEC_FAILURE或错误码；成功返回FPGA_DEC_SUCCESS
---------------------------------------------------------------------*/
s32 FpgaDecChnlDelete(HFpgaDecChnl hDecChnl);

/*====================================================================
    函数名      : FpgaDecChnlVidOutSel
    功能        ：FPGA解码通道主视频输出接口选择,不可动态设置，需要停止解码。
    输入参数说明：hDecChnl: FpgaDecChnlCreate函数返回的句柄;
                 dwVidOutIntf: 参考视频输出接口宏定义
    返回值说明  ：错误返回FPGA_DEC_FAILURE或错误码；成功返回FPGA_DEC_SUCCESS
---------------------------------------------------------------------*/
s32 FpgaDecChnlVidOutSel(HFpgaDecChnl hDecChnl, u32 dwVidOutIntf);

/*====================================================================
    函数名      : FpgaDecChnlVPParamSet
    功能        ：FPGA解码通道VP口播放参数设置,不可动态设置，需要停止解码。
    输入参数说明：hDecChnl: FpgaDecChnlCreate函数返回的句柄;
                 ptVPParam: VP口播放参数
    返回值说明  ：错误返回FPGA_DEC_FAILURE或错误码；成功返回FPGA_DEC_SUCCESS
---------------------------------------------------------------------*/
s32 FpgaDecChnlVPParamSet(HFpgaDecChnl hDecChnl, TFpgaDecVPParam *ptVPParam);

/*====================================================================
    函数名      : FpgaDecChnlDecParamSet
    功能        ：FPGA解码通道解码参数设置,可动态设置，不需要停止解码。
    输入参数说明：hDecChnl: FpgaDecChnlCreate函数返回的句柄;
                 ptDecParam: 解码参数
    返回值说明  ：错误返回FPGA_DEC_FAILURE或错误码；成功返回FPGA_DEC_SUCCESS
---------------------------------------------------------------------*/
s32 FpgaDecChnlDecParamSet(HFpgaDecChnl hDecChnl, TFpgaDecParam *ptDecParam);

/*====================================================================
    函数名      : FpgaDecChnlDecStart
    功能        ：FPGA解码通道解码开始。
    输入参数说明：hDecChnl: FpgaDecChnlCreate函数返回的句柄;
    返回值说明  ：错误返回FPGA_DEC_FAILURE或错误码；成功返回FPGA_DEC_SUCCESS
---------------------------------------------------------------------*/
s32 FpgaDecChnlDecStart(HFpgaDecChnl hDecChnl);

/*====================================================================
    函数名      : FpgaDecChnlDecStop
    功能        ：FPGA解码通道解码停止。
    输入参数说明：hDecChnl: FpgaDecChnlCreate函数返回的句柄;
    返回值说明  ：错误返回FPGA_DEC_FAILURE或错误码；成功返回FPGA_DEC_SUCCESS
---------------------------------------------------------------------*/
s32 FpgaDecChnlDecStop(HFpgaDecChnl hDecChnl);

/*====================================================================
    函数名      : FpgaDecBufGet
    功能        ：FPGA解码Buf申请。
    输入参数说明：hDecChnl: FpgaDecChnlCreate函数返回的句柄;
                 ptBufDesc : 指向存放一个空的帧Buf描述的地址，用户分配并传入指针
    返回值说明  ：错误返回FPGA_DEC_FAILURE或错误码，没有可用Buf时
                    返回FPGA_NO_AVAILABLE_BUF，并且置ptFBuf为NULL
                 成功返回FPGA_DEC_SUCCESS
---------------------------------------------------------------------*/
s32 FpgaDecBufGet(HFpgaDecChnl hDecChnl, TFpgaFrameDesc *ptBufDesc);

/*====================================================================
    函数名      : FpgaDecBufPut
    功能        ：FPGA解码Buf递交。
    输入参数说明：hDecChnl: FpgaDecChnlCreate函数返回的句柄;
                 ptBufDesc : 指向填满帧数据的帧Buf描述的地址，必须是由FpgaDecBufGet获得的
    返回值说明  ：错误返回FPGA_DEC_FAILURE或错误码
                 成功返回FPGA_DEC_SUCCESS
---------------------------------------------------------------------*/
s32 FpgaDecBufPut(HFpgaDecChnl hDecChnl, TFpgaFrameDesc *ptBufDesc);

/*====================================================================
    函数名      : FpgaDecChnlCtrl
    功能        ：FPGA解码通道控制，目前还没有定义，以后需要时可以增加功能。
    输入参数说明：hDecChnl: FpgaDecChnlCreate函数返回的句柄;
                 nCmd: 操作码；pArgs: 参数指针
    返回值说明  ：错误返回FPGA_DEC_FAILURE或错误码；成功返回FPGA_DEC_SUCCESS
---------------------------------------------------------------------*/
s32 FpgaDecChnlCtrl(HFpgaDecChnl hDecChnl, s32 nCmd, void *pArgs);

/*====================================================================
    函数名      : FpgaDbgSetVidOut
    功能        ：调试用，指定输出接口的视频源，可以将输入源直接接给输出接口环回。
    输入参数说明：hEngine: FpgaDecEngineOpen函数返回的句柄;
                 dwVidOutIntf: 视频输出接口；
                 dwFromVP: 为0xffffffff表示从输入接口环回,此时必须填充有效
                           的dwFromVidInIntf参数, 为0~2表示从相应的解码器
                           的VP口获取视频数据,dwFromVidInIntf无效;
                 dwFromVidInIntf: 参考视频输入接口宏定义
    返回值说明  ：错误返回FPGA_FAILURE或错误码；成功返回FPGA_SUCCESS
--------------------------------------------------------------------*/
s32 FpgaDbgSelVidOutSrc(HFpgaDecEngine hEngine, u32 dwVidOutIntf, u32 dwFromVP, u32 dwFromVidInIntf);

/*====================================================================
    函数名      : FpgaDbgSetVidOutStd
    功能        ：调试用，设置指定输出接口的视频制式。
    输入参数说明：hEngine: FpgaDecEngineOpen函数返回的句柄;
                 dwVidOutIntf: 视频输出接口；
                 ptVidInfo: 视频制式;
    返回值说明  ：错误返回FPGA_FAILURE或错误码；成功返回FPGA_SUCCESS
--------------------------------------------------------------------*/
s32 FpgaDbgSetVidOutStd(HFpgaDecEngine hEngine, u32 dwVidOutIntf, TFpgaVidInfo *ptVidInfo);

/*====================================================================
    函数名      : FpgaDbgGetVidOutStd
    功能        ：调试用，获得指定输出接口的视频制式。
    输入参数说明：hEngine: FpgaDecEngineOpen函数返回的句柄;
                 dwVidOutIntf: 视频输出接口；
                 ptVidInfo: 视频制式;
    返回值说明  ：错误返回FPGA_FAILURE或错误码；成功返回FPGA_SUCCESS
--------------------------------------------------------------------*/
s32 FpgaDbgGetVidOutStd(HFpgaDecEngine hEngine, u32 dwVidOutIntf, TFpgaVidInfo *ptVidInfo);

/*====================================================================
    函数名      : FpgaDecIsDied
    功能        ：查询解码器是否异常或死机
    输入参数说明：hEngine: FpgaEncEngineOpen函数返回的句柄;
    返回值说明  ：解码器异常或死机返回TRUE，否则返回FALSE
--------------------------------------------------------------------*/
BOOL32 FpgaDecIsDied(HFpgaDecEngine hEngine);

/*====================================================================
    函数名      : FpgaSetDviMode
    功能        ：设置sii9030工作在DVI还是HDMI mode
    输入参数说明：hEngine: FpgaEncEngineOpen函数返回的句柄;
                  dwVidOutIntf: VIDOUT_DVI0, VIDOUT_HMDI0
    返回值说明  ：正常返回TRUE，否则返回FALSE
--------------------------------------------------------------------*/
s32 FpgaSetDviMode(HFpgaDecEngine hEngine, u32 dwVidOutIntf);

/*====================================================================
    函数名      : FpgaDecPpsSpsSync
    功能        ：FPGA解码码流PPSSPS重新解析，并设置PIP参数。
    输入参数说明：hDecChnl: FpgaDecChnlCreate函数返回的句柄;
    返回值说明  ：错误返回FPGA_FAILURE或错误码；成功返回FPGA_SUCCESS
---------------------------------------------------------------------*/
s32 FpgaDecPpsSpsSync(HFpgaDecChnl hDecChnl);

/*====================================================================
    函数名      : FpgaDecVidOutOffset
    功能        ：设置图象的左右偏移。
    输入参数说明：hEngine: 解码器设备句柄， dwVPId：VP号
                  left：正号为左移left个象素，负号为右移-left个象素
    返回值说明  ：错误返回FPGA_FAILURE或错误码；成功返回FPGA_SUCCESS
---------------------------------------------------------------------*/
s32 FpgaDecVidOutOffset(HFpgaDecEngine hEngine, u32 dwVPId, int left);

#ifdef __cplusplus 
} 
#endif /* __cplusplus */

#endif /* __FPGA_DEC_API_H */
