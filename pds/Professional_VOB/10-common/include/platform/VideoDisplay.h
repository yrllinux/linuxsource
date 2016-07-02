/*******************************************************************************
 * 模块名      : TiVidDrv
 * 文件名      : Viddisplay.h
 * 相关文件    : 
 * 文件实现功能: 视频播放的接口定义（MediaCtrl）
 * 作者        : 钮月忠㊣
 * 版本        : V1.0 Copyright(C) 1995-2006 KEDACOM, All rights reserved.
 * -----------------------------------------------------------------------------
 * 修改记录:
 * 日  期       版本        修改人      修改内容
 * 2006/05/12   1.0a        钮月忠      创建	
*******************************************************************************/
#ifndef VIDDISPLAY_H
#define VIDDISPLAY_H 1

#ifdef __cplusplus
    extern "C" {
#endif

#include "DM642VPort.h"

enum
{
    VIDDIS_OK = 0,
    VIDDIS_INV_PARAM,
    VIDDIS_OPEN_FAILED,
    VIDDIS_IS_CLOSED,
    VIDDIS_VPORT_FAILED,
    VIDDIS_NO_VALID_FRAME
    
};

/*D/A转换芯片名称定义*/
#define VIDDIS_SAA7121    VPORT_CHIP_SAA7121
#define VIDDIS_ADV7179    VPORT_CHIP_ADV7179

typedef struct {
    u8   bOutVGA;            /*不使用（保持兼容）*/
    u8   bOutPAL;            /*PAL 或 NTSC，可动态修改，但要慎重*/
    u8   chipName;           /*视频信号编码芯片代号*/
    u8   chipIicAddr;        /*不使用（保持兼容）*/
    
    l32  maxFrameSize;       /*缓冲区的大小*/
    l32  iWidth;             /*720(pal) 720(ntsc)*/
    l32  iHeight;            /*576(pal) 480(ntsc)*/
    
    u32  iPipUiPixelFormat;  /*不使用（保持兼容）*/
    l32  iPipWidth;          /*不使用（保持兼容）*/
    l32  iPipHeight;         /*不使用（保持兼容）*/
    l32  iPipXPos;           /*不使用（保持兼容）*/
    l32  iPipYPos;           /*不使用（保持兼容）*/
    
    u16  bufNum;             /*缓冲区数量，至少为 2*/
    u16  refreshRate;        /*不使用（保持兼容）*/
    
    u8   bUseOsd;            /*不使用（保持兼容）*/
    u8   bShowPip;           /*不使用（保持兼容）*/
    u16  version;            /*不使用（保持兼容）*/
    
    u8  *pFullScreenArgb;    /*不使用（保持兼容）*/
    u8  *pFullScreenArgbBak; /*不使用（保持兼容）*/
    
}   VDIS_PARAM;

typedef struct {
    u32 dwFrameDropped;
    
    u32 dwDisComplete;
    u32 dwDisUnderrun;
    u32 dwDisCompleteNack;

}   VIDDIS_STATUS;

/*====================================================================
	函数名	    ：ViddisAndOsdInit
	功能        ：视频显示和OSD初始化
	参数说明    ：pVDisParam，初始化参数
	返回值说明  ：ETIDRV_OK，成功
                  小于0，    失败
====================================================================*/
l32 ViddisInit(VDIS_PARAM *pVDisParam);

/*====================================================================
	函数名	    ：ViddisShow
	功能        ：视频显示输出，将pBuf中的内容显示到pictureNo信道画面中
	参数说明    ：int pictureNo，画面号，PICTURE_PIP或PICTURE_FULL
	              u8 *pBuf,   视频输出缓冲指针
                  l32 bShowGray, 是否显示灰屏。1，显示灰屏；0，显示pBuf内容。优先于bLockFrameBef
                  l32 bLockFrameBef ,是否锁定上一幀。1，锁定；0，不锁定
	返回值说明  ：ETIDRV_OK，成功
                  小于0，    失败
====================================================================*/
l32 ViddisShow(l32 pictureNo, u8 *pBuf, l32 bShowGray, l32 bLockFrameBef);

/*====================================================================
	函数名	    ：ViddisMemGet
	功能        ：视频显示空缓冲指针取得，可存放马上要通过函数ViddisShow显示的一幀数据
	参数说明    ：int pictureNo，画面号，PICTURE_PIP或PICTURE_FULL
	              u8 **ppBuf,   得到的视频显示空缓冲指针
	              l32 iTimeOut，    超时时间设置，BUF_NO_WAIT（0）不等待；BUF_INFINITE_WAIT（－1）无限等待；其他值，等待的毫秒数
	返回值说明  ：ETIDRV_OK，成功
                  小于0，    失败
====================================================================*/
l32 ViddisMemGet(l32 pictureNo, u8 **ppBuf, l32 iTimeOut);

/*====================================================================
	函数名	    ：ViddisSetParam
	功能        ：视频显示动态设置参数，1.0版本仅支持用于修改画中画的位置和是否显示画中画
	参数说明    ：pVDisParam，视频显示修改参数指针
	返回值说明  ：ETIDRV_OK，成功
                  小于0，    失败
====================================================================*/
l32 ViddisSetParam(VDIS_PARAM *pVDisParam);  
                                                        
/*====================================================================
	函数名	    ：ViddisAvailBufs
	功能        ：视频显示有效缓冲个数，表示缓冲区中填满的待显示的个数
	参数说明    ：int *bufNum，视频显示有效缓冲个数指针
	返回值说明  ：ETIDRV_OK，成功
                  小于0，    失败
====================================================================*/
l32 ViddisAvailBufs(l32 pictureNo,l32 *bufNum);

/*====================================================================
    函数名	    ：ViddisReadyBufs
    功能        ：视频显示有效缓冲个数，表示缓冲区中填满的待显示的个数
                  如果该函数返回值大于1，上层可以在网络没收到包的时候不锁屏
    参数说明    ：int *bufNum，视频显示有效缓冲个数指针
    返回值说明  ：ETIDRV_OK，成功
                  小于0，    失败
====================================================================*/
l32 ViddisReadyBufs(l32 pictureNo, l32 *bufNum);

/*====================================================================
	函数名	    ：ViddisClose
	功能        ：关闭显示驱动
	参数说明    ：无
    使用全局变量：无
	返回值说明  ：ETIDRV_OK，成功
                  小于0，    失败
====================================================================*/
l32 ViddisClose();

/*====================================================================
	函数名	    ：VidDisGetStatus
	功能        ：取得视频播放驱动的状态统计信息
	参数说明    ：无
	返回值说明  ：ETIDRV_OK，成功
                  小于0，    失败
====================================================================*/
l32 ViddisGetStatus(VIDDIS_STATUS *ptViddisStatus);

/*====================================================================
	函数名	    ：VidDisResetStatus
	功能        ：重置视频播放驱动的状态统计信息
	参数说明    ：无
	返回值说明  ：VIDDIS_OK，       成功
                  其他值，          失败
====================================================================*/
l32 ViddisResetStatus(void);

#ifdef __cplusplus
    }
#endif

#endif /*VIDDISPLAY_H*/
