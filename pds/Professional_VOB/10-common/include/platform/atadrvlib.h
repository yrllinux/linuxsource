/*****************************************************************************
模块名      : ATA_MODULE
文件名      : ataDrvLib.h
相关文件    : 
文件实现功能:  ATA_MODULE 上层接口宏定义和函数的声明.
作者        : 张方明
版本        : V1.0  Copyright(C) 2005-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人     走读人       修改内容
2005/10/22  01a        张方明                  创建	
******************************************************************************/

#ifndef __INC_ataDrvLib_H
#define __INC_ataDrvLib_H

#ifdef __cplusplus 
extern "C" { 
#endif /* __cplusplus */


/* ================================= 宏定义 ================================ */

/* ata驱动模块返回值和错误码描述 */
#define ATA_SUCCESS             (INT32)0                    /* 成功 */
#define ATA_FAILURE             (INT32)-1                   /* 未知的异常失败 */

#define ATA_ERRCODE_BASE        (INT32)1                    /* 错误码基值 */
#define ATA_NOT_INIT            (INT32)(ATA_ERRCODE_BASE+0) /* 设备未初始化 */
#define ATA_PARAM_ERROR         (INT32)(ATA_ERRCODE_BASE+1) /* 参数非法 */

#define	ATA_IN_STANDBY          (UINT8)0x00	            /* 挂起状态 */
#define	ATA_IN_IDLE             (UINT8)0x80	            /* 空闲状态 */
#define	ATA_IN_ACTIVE           (UINT8)0xFF	            /* 活动状态 */

/* ATA范围定义 */
#define ATA_HD_MAX_NUM          (UINT8)4                    /* 最多支持4个硬盘 */
#define ATA_STRING_MAX_LEN      (UINT8)40                   /* 字符串最大长度定义 */

#define ATA_HD_PARTION_MAX_NUM  (UINT8)4                    /* 硬盘最大分区个数 */

#define ATA_DOS_OPT_DEFAULT     (INT32)0                    /* 硬盘分区功能码 */

#define ATA_FSCHK_OPT_DEFAULT   (INT32)0                    /* 硬盘文件系统检查功能码 */

#define ATA_STANDBY_MAX_IDLE_TIME   ((INT32)(18000))        /* 硬盘挂起最大空闲时间值为5小时 */

/* ============================== 结构、类型定义 =========================== */

/* 单个硬盘信息参数结构定义 */
typedef struct{
    BOOL   bHDiskExist;                       /* 硬盘存在标志 */
    char   achHDiskModel[ATA_STRING_MAX_LEN]; /* 硬盘型号 */
    UINT32 dwHDiskMBytes;                     /* 硬盘容量，以MByte为单位 */
}TAtaHDiskInfo;


/* 硬盘分区信息参数结构定义 */
typedef struct{
    BOOL   bFormated;                         /* 分区格式化标志，读取时有效，创建时不用填 */
    UINT32 dwPartionMBytes;                   /* 分区容量，以MByte为单位 */
    char   achPartionName[ATA_STRING_MAX_LEN];/* 分区设备名，读取时有效，创建时不用填 */
}TOnePartionInfo;

typedef struct{
    UINT32 dwPartionNum;                      /* 硬盘分区个数,0~ATA_HD_PARTION_MAX_NUM-1 */
    TOnePartionInfo tPartionInfo[ATA_HD_PARTION_MAX_NUM]; /* 硬盘分区信息 */
}THDiskPartionInfo;


/* ================================= 函数声明 ============================== */

/* 获取指定硬盘的信息 */
INT32 AtaApiGetHDiskInfo(UINT8 byHDiskId, TAtaHDiskInfo *ptAtaHDiskInfo);

/* 硬盘分区 */
INT32 AtaApiFdisk(UINT8 byHDiskId, THDiskPartionInfo *ptPartionInfo);

/* 硬盘分区的格式化 */
INT32 AtaApiPartnFormat(char *pPartionName, INT32 nOpt);

/* 硬盘分区信息查询 */
INT32 AtaApiGetPartnInfo(UINT8 byHDiskId, THDiskPartionInfo *ptPartionInfo);

/* 硬盘分区文件系统检查 */
INT32 AtaApiChkdsk(char *pPartionName, INT32 nOpt);

/* 硬盘分区剩余容量查询 */
INT32 AtaApiGetPartnFreeSize(char *pPartionName, UINT32 *pdwFreeKbytes);

/*====================================================================
/*  功能        ：硬盘自动挂起设置 
    输入参数说明：byHDiskId: 硬盘编号，范围 0~ ATA_HD_MAX_NUM-1
                 nIdleTimeSec：硬盘在没有读写操作到挂起的时间，秒为单位。
                               0为立即休眠，并且休眠激活后将一直处于运行状态
                               最长不能超过5小时(ATA_STANDBY_MAX_IDLE_TIME)
    返回值说明  ：错误返回如ata驱动模块错误码描述错误码；
                 成功, 返回ATA_SUCCESS.
----------------------------------------------------------------------*/
INT32 AtaApiSetStandby(UINT8 byHDiskId, INT32 nIdleTimeSec);

/*====================================================================
    功能        ：查询硬盘当前工作状态。
    输入参数说明：byHDiskId: 硬盘编号，范围 0~ ATA_HD_MAX_NUM-1
                pbyPWRMode: 存放硬盘电源模式的指针，返回类型：
                #define	ATA_IN_STANDBY          (UINT8)0x00 挂起状态
                #define	ATA_IN_IDLE             (UINT8)0x80 空闲状态
                #define	ATA_IN_ACTIVE           (UINT8)0xFF 活动状态
    返回值说明  ：错误返回如ata驱动模块错误码描述错误码；
                 成功, 返回ATA_SUCCESS.
----------------------------------------------------------------------*/
INT32 AtaApiGetPWRMode(UINT8 byHDiskId, UINT8 *pbyPWRMode);

/*====================================================================
    功能        ：备份分区表。
    输入参数说明：device: 要备份的硬盘设备节点号，如"/dev/hda0"    
    返回值说明  ：成功, 返回ATA_SUCCESS.
                  失败，返回ATA_FAILURE
----------------------------------------------------------------------*/
INT32 AtaApiBackupPartitionTable(char *device);

/*====================================================================
    功能        ：还原分区表。
    输入参数说明：device: 要还原的硬盘设备节点号，如"/dev/hda0"    
    返回值说明  ：成功, 返回ATA_SUCCESS.
                  失败，返回ATA_FAILURE
----------------------------------------------------------------------*/
INT32 AtaApiRestorePartitionTable(char *device);

#ifdef __cplusplus 
} 
#endif /* __cplusplus */


#endif /* __INC_ataDrvLib_H */
