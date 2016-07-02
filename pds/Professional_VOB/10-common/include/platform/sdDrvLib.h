/*****************************************************************************
模块名      : SD_MODULE
文件名      : sdDrvLib.h
相关文件    : 
文件实现功能:  SD_MODULE 上层接口宏定义和函数的声明.
作者        : 张方明
版本        : V1.0  Copyright(C) 2005-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人     走读人       修改内容
2006/01/04  01a        张方明                  创建	
******************************************************************************/

#ifndef __INC_SdDrvLib_H
#define __INC_SdDrvLib_H

#ifdef __cplusplus 
extern "C" { 
#endif /* __cplusplus */


/* ================================= 宏定义 ================================ */

/* sd驱动模块返回值和错误码描述 */
#define SD_SUCCESS              (INT32)0                    /* 成功 */
#define SD_FAILURE              (INT32)-1                   /* 未知的异常失败 */

#define SD_ERRCODE_BASE         (INT32)1                    /* 错误码基值 */
#define SD_NOT_INIT             (INT32)(SD_ERRCODE_BASE+0)  /* 设备未初始化 */
#define SD_PARAM_ERROR          (INT32)(SD_ERRCODE_BASE+1)  /* 参数非法 */

/* sd范围定义 */
#define SD_CARD_MAX_NUM         (UINT8)1                    /* 最多支持1个sd卡 */
#define SD_STRING_MAX_LEN       (UINT8)16                   /* 字符串最大长度定义 */

#define SD_DOS_OPT_DEFAULT      (INT32)0                    /* 文件系统格式化功能码 */
#define SD_FSCHK_OPT_DEFAULT    (INT32)0                    /* 文件系统检查功能码 */

/* ============================== 结构、类型定义 =========================== */

/* 单个sd卡信息参数结构定义 */
typedef struct{
    BOOL   bExist;                            /* sd卡存在标志 */
    BOOL   bFormated;                         /* sd卡格式化标志 */
    UINT32 dwSizeInMBytes;                    /* sd卡容量，以MByte为单位 */
    char   achModel[SD_STRING_MAX_LEN];       /* sd卡型号 */
    char   achPartionName[SD_STRING_MAX_LEN]; /* 分区设备名 */
} TOneSdCardInfo;

/* 所有sd卡信息参数结构定义 */
typedef struct{
    TOneSdCardInfo tSdInfo[SD_CARD_MAX_NUM];
} TSdCardInfo;



/* ================================= 函数声明 ============================== */

/* 获取指定sd卡的信息 */
INT32 SdApiGetDiskInfo(TSdCardInfo *ptSdInfo);

/* sd卡的格式化 */
INT32 SdApiPartnFormat(char *pPartionName, INT32 nOpt);

/* sd卡分区文件系统检查 */
INT32 SdApiChkdsk(char *pPartionName, INT32 nOpt);

/* sd卡剩余容量查询 */
INT32 SdApiGetPartnFreeSize(char *pPartionName, UINT32 *pdwFreeKbytes);

#ifdef __cplusplus 
} 
#endif /* __cplusplus */


#endif /* __INC_SdDrvLib_H */
