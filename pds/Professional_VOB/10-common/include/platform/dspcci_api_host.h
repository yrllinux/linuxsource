/*****************************************************************************
模块名      : DSPCCI
文件名      : dspcci_api_host.h
相关文件    : 
文件实现功能:  host侧cci通信协议相关的宏和数据结构定义，被上层应用包含，
作者        : 张方明
版本        : V1.0  Copyright(C) 2008-2010KEDACOM, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
10/24/2008  01a         张方明     创建.	
******************************************************************************/

#ifndef __DSPCCI_API_HOST_H
#define __DSPCCI_API_HOST_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdlib.h>
#include "kdvtype.h"
#include "dspcci_common.h"


/* 字节序转换宏定义 */
#ifdef __linux__ 
    #define _LITTLE_ENDIAN              0
    #define _BIG_ENDIAN                 1

    #if __BYTE_ORDER == __LITTLE_ENDIAN
    #   define _BYTE_ORDER     _LITTLE_ENDIAN
    #else
    #   define _BYTE_ORDER     _BIG_ENDIAN
    #endif
#endif

#if _BYTE_ORDER == _BIG_ENDIAN 

#define ltohl(x)	((((x) & 0x000000ffUL) << 24) | \
			 (((x) & 0x0000ff00UL) <<  8) | \
			 (((x) & 0x00ff0000UL) >>  8) | \
			 (((x) & 0xff000000UL) >> 24))

#define htoll(x)	((((x) & 0x000000ffUL) << 24) | \
			 (((x) & 0x0000ff00UL) <<  8) | \
			 (((x) & 0x00ff0000UL) >>  8) | \
			 (((x) & 0xff000000UL) >> 24))

#define ltohs(x)	((((x) & 0x00ff) << 8) | \
			 (((x) & 0xff00) >> 8))

#define htols(x)	((((x) & 0x00ff) << 8) | \
			 (((x) & 0xff00) >> 8))

/* 使用宏来转换当输入的参数为一个表达式(如 *ptr, a+b)时会出现4次计算的操作
   一方面降低代码执行效率，另一方面当指针32位读写外设的内存操作会被依次调用4次，
   若外设在Host4次调用期间修改了内存的值，Host读到的值会错位乱掉，因此改为inline函数
#define	LTOHL(x)	ltohl((u32)(x))
#define	LTOHS(x)	ltohs((u16)(x))
#define	HTOLL(x)	htoll((u32)(x))
#define	HTOLS(x)	htols((u16)(x))
*/

static inline u32 LTOHL(u32 x)
{
    return ltohl(x);
}

static inline u32 LTOHS(u16 x)
{
    return ltohs(x);
}

static inline u32 HTOLL(u32 x)
{
    return htoll(x);
}

static inline u32 HTOLS(u16 x)
{
    return htols(x);
}

#else

#define	LTOHL(x)	(u32)(x)
#define	LTOHS(x)	(u16)(x)
#define	HTOLL(x)	(u32)(x)
#define	HTOLS(x)	(u16)(x)

#endif	/* _BYTE_ORDER==_LITTLE_ENDIAN */

/* DSP类型定义 */
#define DSP_TYPE_MAX_NUM        5
#define DSP_TYPE_DM647_PCI      0x01
#define DSP_TYPE_DM647_HPI      0x02
#define DSP_TYPE_DM6437_PCI     0x03
#define DSP_TYPE_DM6437_HPI     0x04
#define DSP_TYPE_DM6747_SPI     0x05


/* 自检模块宏定义 */
#define DSP_SELFTEST_MEM        0x80000000  /* 进行内存自检 */

typedef void ( *TDspPrtFunc )(const char *pFmtStr, ...);

/* dsp 配置参数结构定义 */
typedef struct{
    u32 dwDspType;                  /* 见DSP类型定义 */
    u32 dwChipId;                   /* 该类型DSP的芯片索引，取值范围: 0~DSPCCI_DSP_MAXNUM-1; */
    s8  *pbyLoadFile;               /* DSP运行的程序文件名 */
    u32 dwDspFreqMHz;               /* DSP运行频率, 0表示使用默认值 */
    u32 dwDDRFreqMHz;               /* DSP内存频率, 0表示使用默认值 */
    u32 dwSelfTest;                 /* 是否进行自检，0=不自检; 可或上相应的宏做相应的模块自检 */
} TDspCfg;


/* 用户传入的通道Buffer参数结构定义 */
typedef struct{
    u32 dwMaxMsgLen;                /* 每条消息的最大长度(按BYTE计)，要求8的整数倍 */
    u32 dwMaxMsgs;                  /* 最多缓存多少条最大的消息长度 */
} TCciBufCfgDesc;

typedef struct{
    u32 dwUpChnls;                  /* 上行通道数 */
    u32 dwDnChnls;                  /* 下行通道数 */
    TCciBufCfgDesc *patUpChnlCfgs;  /* 指向用户分配的TCciBufCfgDesc结构变量数组的首地址 */
    TCciBufCfgDesc *patDnChnlCfgs;  /* 指向用户分配的TCciBufCfgDesc结构变量数组的首地址 */
} TCciParam;


/* 全局函数声明 */
/*==============================================================================
    函数名      : DspCciOpen
    功能        : 打开指定DSP的CCI通信接口
    输入参数说明: dwDspId:      [I]DSP全局索引，取值范围: 0~DSPCCI_DSP_MAXNUM-1;
                  ptDspCfg:     [I]DSP启动配置信息，可绑定DSP全局索引对应的物理芯片;
                  ptCciParam:   [I]CCI通信接口参数
                  pptObj:       [O]返回给用户的控制句柄
    返回值说明  : 错误返回DSPCCI_FAILURE或错误码；成功返回DSPCCI_SUCCESS
------------------------------------------------------------------------------*/
s32 DspCciOpen(u32 dwDspId, TDspCfg *ptDspCfg , TCciParam *ptCciParam, HDspCciObj *pptObj);

/*==============================================================================
    函数名      : DspCciClose
    功能        : 关闭指定DSP的CCI通信接口
    输入参数说明: ptObj:        [I]CCI通信接口控制句柄
    返回值说明  : 错误返回DSPCCI_FAILURE或错误码；成功返回DSPCCI_SUCCESS
------------------------------------------------------------------------------*/
s32 DspCciClose(HDspCciObj ptObj);

/*==============================================================================
    函数名      : DspCciWriteMsg
    功能        : 通过指定DSP的CCI通信接口的下行通道写一个消息包给DSP
    输入参数说明: ptObj:        [I]CCI通信接口控制句柄
                  dwChnl:       [I]下行通道索引;
                  ptMsgDesc:    [I]消息描述;
    返回值说明  : 错误返回DSPCCI_FAILURE或错误码；成功返回DSPCCI_SUCCESS
------------------------------------------------------------------------------*/
s32 DspCciWriteMsg(HDspCciObj ptObj, u32 dwChnl, TDspCciMsgDesc *ptMsgDesc);

/*==============================================================================
    函数名      : DspCciReadMsg
    功能        : 通过指定DSP的CCI通信接口的上行通道读取一个消息包到用户的Buf，
                  不能和DspCciRecvMsg混用。
    输入参数说明: ptObj:        [I]CCI通信接口控制句柄
                  dwChnl:       [I]上行通道索引;
                  pbyBuf:       [I]用户分配的Buf，用来存放读到的数据;
                  pdwSize:      [IO]用户分配，输入时指定用户的Buf的大小，如果足够大
                                    则写入数据并返回写入的数据长度;否则返回错误
                  nTimeout:     [I]超时值: 0=立即返回, -1=永远等待, >0=等待毫秒数
    返回值说明  : 错误返回DSPCCI_FAILURE或错误码；成功返回DSPCCI_SUCCESS
------------------------------------------------------------------------------*/
s32 DspCciReadMsg(HDspCciObj ptObj, u32 dwChnl, u8 *pbyBuf, u32 *pdwSize, s32 nTimeout);

/*==============================================================================
    函数名      : DspCciRecvMsg
    功能        : 通过指定DSP的CCI通信接口的上行通道接口接收一个消息包，并告知用
                  户消息的地址，用户处理完后调用DspCciRecvMsgDone释放该消息包。
                  不能和DspCciReadMsg混用。
    输入参数说明: ptObj:        [I]CCI通信接口控制句柄
                  dwChnl:       [I]上行通道索引;
                  ptMsgDesc:    [O]用户分配，用来存放接收的数据包的地址长度等信息;
                  nTimeout:     [I]超时值: 0=立即返回, -1=永远等待, >0=等待毫秒数
    返回值说明  : 错误返回DSPCCI_FAILURE或错误码；成功返回DSPCCI_SUCCESS
------------------------------------------------------------------------------*/
s32 DspCciRecvMsg(HDspCciObj ptObj, u32 dwChnl, TDspCciMsgDesc *ptMsgDesc, s32 nTimeout);

/*==============================================================================
    函数名      : DspCciRecvMsgDone
    功能        : 和DspCciRecvMsg配对使用，用户处理完DspCciRecvMsg得到的数据后
                  需调用本函数从接收对列中释放该消息包。
    输入参数说明: ptObj:        [I]CCI通信接口控制句柄
                  dwChnl:       [I]上行通道索引;
                  ptMsgDesc:    [I]需要将DspCciRecvMsg调用时分配的结构变量传入;
    返回值说明  : 错误返回DSPCCI_FAILURE或错误码；成功返回DSPCCI_SUCCESS
------------------------------------------------------------------------------*/
s32 DspCciRecvMsgDone(HDspCciObj ptObj, u32 dwChnl, TDspCciMsgDesc *ptMsgDesc);

/*==============================================================================
    函数名      : DspCciGetDspId
    功能        : 根据CCI通信接口控制句柄查询对应的DSP索引号。
    输入参数说明: ptObj:        [I]CCI通信接口控制句柄
    返回值说明  : 错误返回DSPCCI_FAILURE；成功返回对应的DSP索引号
------------------------------------------------------------------------------*/
s32 DspCciGetDspId(HDspCciObj ptObj);

/*==============================================================================
    函数名      : DspCciGetHeartBeat
    功能        : 查询指定DSP的心跳计数，如果不再累加，说明DSPCCI通信已经挂死。
    输入参数说明: ptObj:        [I]CCI通信接口控制句柄
                  pdwCount:     [I]存放心跳计数的指针;
    返回值说明  : 错误返回DSPCCI_FAILURE或错误码；成功返回DSPCCI_SUCCESS
------------------------------------------------------------------------------*/
s32 DspCciGetHeartBeat(HDspCciObj ptObj, u32 *pdwCount);

/*==============================================================================
    函数名      : DspCciGetErrLog
    功能        : 查询指定DSP的错误日志。
    输入参数说明: ptObj:        [I]CCI通信接口控制句柄
                  dwErrId:      [I]错误日志索引，0~DSPCCI_ERR_LOG_MAX_NUM-1;
                  pdwErrLog:    [I]存放错误日志的指针;
    返回值说明  : 错误返回DSPCCI_FAILURE或错误码；成功返回DSPCCI_SUCCESS
------------------------------------------------------------------------------*/
s32 DspCciGetErrLog(HDspCciObj ptObj, u32 dwErrId, u32 *pdwErrLog);

/*==============================================================================
    函数名      : DspCciClearRxChnl
    功能        : 清空指定的上行通道数据。
    输入参数说明: ptObj:        [I]CCI通信接口控制句柄
                  dwChnl:       [I]上行通道索引;
    返回值说明  : 错误返回DSPCCI_FAILURE或错误码；成功返回DSPCCI_SUCCESS
------------------------------------------------------------------------------*/
s32 DspCciClearUpChnl(HDspCciObj ptObj, u32 dwChnl);

/*==============================================================================
    函数名      : DspCciGetVer
    功能        : 模块版本号查询。
    输入参数说明: pchVer: 给定的存放版本信息的buf指针
                  dwBufLen:给定buf的长度
    返回值说明  : 版本的实际字符串长度。小于0为出错;
                  如果实际字符串长度大于dwBufLen，赋值为0
------------------------------------------------------------------------------*/
s32 DspCciGetVer(s8 *pchVer, u32 dwBufLen);

/*==============================================================================
    函数名      : DspCciRegPrtFunc
    功能        : 注册打印函数，支持用用户的打印函数将dsp侧的打印信息打印出来。
                  如果用户不注册的话驱动使用printf函数打印到串口或网口
    输入参数说明: ptDspPrtFunc: 用户的打印函数指针
    返回值说明  : 错误返回DSPCCI_FAILURE或错误码；成功返回DSPCCI_SUCCESS
------------------------------------------------------------------------------*/
s32 DspCciRegPrtFunc(TDspPrtFunc ptDspPrtFunc);

/*==============================================================================
    函数名      : DspCciGetRxMsgNum
    功能        : 查询接收缓冲Buf中缓存的消息包数。
    输入参数说明: ptObj:        [I]CCI通信接口控制句柄
                  dwChnl:       [I]上行通道索引;
                  pdwMsgNum:    [O]返回给用户的消息包数;
    返回值说明  : 错误返回DSPCCI_FAILURE或错误码；成功返回DSPCCI_SUCCESS
------------------------------------------------------------------------------*/
s32 DspCciGetRxMsgNum(HDspCciObj ptObj, u32 dwChnl, u32 *pdwMsgNum);


/* 以下为调试用函数，由用户注册给OSP */

/*==============================================================================
    函数名      : DspCciPrtEnable
    功能        : 控制是否放开DSP的打印
    输入参数说明: dwDspId:  [I]DSP全局索引，取值范围: 0~DSPCCI_DSP_MAXNUM-1;
                  bEnPrt:   [I]0=禁止打印，1=允许打印 
    返回值说明  : 无
------------------------------------------------------------------------------*/
void DspCciPrtEnable(u32 dwDspId, BOOL32 bEnPrt);

/*==============================================================================
    函数名      : DspCciInfoShow
    功能        : 打印CCI协议中INFO区变量数据和上下行通道控制信息
    输入参数说明: dwDspId:  [I]DSP全局索引，取值范围: 0~DSPCCI_DSP_MAXNUM-1;
    返回值说明  : 无
------------------------------------------------------------------------------*/
void DspCciInfoShow(u32 dwDspId);

/*==============================================================================
    函数名      : DspCciModuleShow
    功能        : 打印HOST侧相关控制结构成员变量以及收发统计计数
    输入参数说明: dwDspId:  [I]DSP全局索引，取值范围: 0~DSPCCI_DSP_MAXNUM-1;
    返回值说明  : 无
------------------------------------------------------------------------------*/
void DspCciModuleShow(u32 dwDspId);


#ifdef __cplusplus 
}
#endif /* __cplusplus */

#endif /* __DSPCCI_API_HOST_H */
