/*******************************************************************************
 * 模块名      : TiVidDrv
 * 文件名      : Vidcapture.h
 * 相关文件    : 
 * 文件实现功能: 视频采集的接口定义（MediaCtrl）
 * 作者        : 钮月忠㊣
 * 版本        : V1.0 Copyright(C) 1995-2006 KEDACOM, All rights reserved.
 * -----------------------------------------------------------------------------
 * 修改记录:
 * 日  期       版本        修改人      修改内容
 * 2006/05/14   1.0a        钮月忠      创建	
*******************************************************************************/
#ifndef VIDCAPTURE_H
#define VIDCAPTURE_H 1

#ifdef __cplusplus
    extern "C" {
#endif

#include "DM642VPort.h"

enum
{
    VIDCAP_OK = 0,
    VIDCAP_INV_PARAM,
    VIDCAP_OPEN_FAILED,
    VIDCAP_IS_CLOSED,
    VIDCAP_VPORT_FAILED,
    VIDCAP_NO_VALID_FRAME
};

/* A/D 转换芯片名称定义*/
#define VIDCAP_ADV7180    VPORT_CHIP_ADV7180
#define VIDCAP_ADV7181    VPORT_CHIP_ADV7181

typedef struct {
    u8   portNo;             /*模拟端口号，仅在初始化使用，可以动态修改，可用VidCapSetParam函数。从0开始*/
    u8   bUsePal;            /*pal或ntsc，可动态修改,但是不推荐，因为实际会重新初始化*/
    u8   chipName;           /*视频信号解码芯片代号，默认VCAP_SAA7114，不能动态修改*/
    u8   chipIicAddr;        /*不使用（保持兼容）*/
    l32  maxFrameSize;       /*一幀缓冲区的大小 一般720×576×2，即ETI_MAX_FRAMESIZE，不能动态修改*/
    l32  iWidth;             /*def 720*/
    l32  iHeight;            /*def 576(pal) 480(ntsc)*/
    
    u16  bufNum;             /*不使用（保持兼容）*/
    u16  bUseIport;          /*不使用（保持兼容）*/
    
} VCAP_PARAM;

typedef struct {
    u32 dwFrameDropped;
    
	u32 dwCapComplete;
	u32 dwCapOverrun;
	u32 dwCapSyncByteErr;
	u32 dwCapShortFieldDetect;
    u32 dwCapLongFieldDetect;

} VIDCAP_STATUS;

typedef struct {
	u64 qwVal;
	
} VIDCAP_TIMESTAMP;

/*====================================================================
	函数名	    ：VidCapInit
	功能        ：视频采集初始化
	参数说明    ：nChanNoIn:        通道号（1～2）
	              ptVcapInitParam:  初始化参数
	返回值说明  ：ETIDRV_OK，成功
                  小于0，    失败
====================================================================*/
l32 VidCapInit(l32 nChanNoIn, VCAP_PARAM *ptVcapInitParam);

/*====================================================================
	函数名      ：VidCapInput
	功能        ：视频采集输入，将采集到的数据放到*ppBuf中
	参数说明    ：chanNo，通道号（1～2）
	              ppBuf，输入数据指针
	              iTimeOut，超时设置，BUF_NO_WAIT（0）不等待；BUF_INFINITE_WAIT（－1）无限等待；其他值，等待的毫秒数
	返回值说明  ：ETIDRV_OK，成功
                  小于0，    失败
====================================================================*/
l32 VidCapInput(l32 nChanNoIn, u8 **ppbyBuf, l32 nTimeOut, VIDCAP_TIMESTAMP *ptTimeStamp);

/*====================================================================
	函数名	    ：VidCapSetParam
	功能        ：视频采集参数动态修改
	参数说明    ：chanNo，通道号，1－2
	              pVcapParam，修改后的参数
	返回值说明  ：ETIDRV_OK，成功
                  小于0，    失败
====================================================================*/
l32 VideoCapSetParam(l32 nChanNoIn, VCAP_PARAM *ptVcapParam);

/*====================================================================
	函数名      ：VidCapAvailBufs
	功能        ：视频采集已经满的缓冲区数量
	参数说明    ：chanNo，通道号，1－2
	              bufNum，缓冲区数量
	返回值说明  ：ETIDRV_OK，成功
                  小于0，    失败
====================================================================*/
l32 VidCapAvailBufs(l32 nChanNoIn, l32 *pnbufNum);

/*====================================================================
	函数名	    ：VidCapClose
    功能        ：视频采集关闭
    参数说明    ：chanNo，通道号，0－1
	返回值说明  ：ETIDRV_OK，成功
                  小于0，    失败
====================================================================*/
l32 VidCapClose(l32 nChanNo);

/*====================================================================
	函数名	    ：VidCapGetStatus
    功能        ：取得视频采集驱动的状态统计信息
    参数说明    ：ptVidCapStatus，
	返回值说明  ：ETIDRV_OK，成功
                  小于0，    失败
====================================================================*/
l32 VidCapGetStatus(VIDCAP_STATUS *ptVidCapStatus);

/*====================================================================
	函数名	    ：VidCapResetStatus
	功能        ：重置视频采集驱动的状态统计信息
	参数说明    ：无
	返回值说明  ：VIDCAP_OK，       成功
                  其他值，          失败
====================================================================*/
l32 VidCapResetStatus(void);

#ifdef __cplusplus
    }
#endif

#endif /*VIDCAPTURE_H*/
