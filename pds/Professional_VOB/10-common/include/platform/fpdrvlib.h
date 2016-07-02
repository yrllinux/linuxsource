/*****************************************************************************
模块名      : flashDisk
文件名      : fpDrvLib.h
相关文件    : 
文件实现功能:  .
作者        : 张方明
版本        : V1.0  Copyright(C) 2003-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2005/2/25  01a         张方明      创建	
******************************************************************************/
#ifndef __INC_FP_DRVLIB_H
#define __INC_FP_DRVLIB_H

#ifdef __cplusplus
extern "C" {
#endif

/* 头文件 */

/* 宏定义 */
#define PARTITION_MAX_NUM           8       /* 数据分区最大个数，除分区表区外 */

#define PARTITION_TYPE_NOASSIGN     0       /* 表示为未分配的分区 */
#define PARTITION_TYPE_OS           1       /* 表示为操作系统数据分区，暂不支持 */
#define PARTITION_TYPE_APP          2       /* 表示为应用程序数据分区，暂不支持 */
#define PARTITION_TYPE_AUXDATA      3       /* 表示为操作系统应用程序复合分区 */
#define PARTITION_TYPE_USRDATA      4       /* 表示为用户数据分区 */
#define PARTITION_TYPE_TFFS         5       /* 表示tffs文件系统分区 */

#define PARTITION_TYPE_MAXNUM       6       /* 表示分区类型的总数 */

#define EXECODE_TYPE_NONE           0       /* 特殊可执行程序数据区不使用 */
#define EXECODE_TYPE_FPGA  0xfa002005       /* 特殊可执行程序数据区存放fpga数据 */

/* 每个分区参数结构定义 */
typedef struct{
    UINT32 dwPartitionType;                 /* 分区类型 */
    UINT32 dwPartitionSize;                 /* 该分区容量,字节为单位 */
}TOnePartnParam;

/* 分区参数结构定义 */
typedef struct{
    BOOL  bUseDefaultConf;                  /* 是否使用默认的配置分区,如果为TRUE，使用每个板子的默认配置来分区,在读分区信息时返回0，无效数据 */
    TOnePartnParam tOnePartitionParam[PARTITION_MAX_NUM];    /* 每个分区信息 */
    UINT32 dwRamdiskSize;                   /* RAMDISK 的大小,字节为单位 */
}TFlPartnParam;


/* 函数声明 */
BOOL   BrdRawFlashIsUsed(void);                    /* 查询是否采用rawflash管理 */
UINT32 BrdGetFullRamDiskFileName(char *pInFileName, char *pRtnFileName);/* 转换相对路径的文件名为ramdisk中带绝对路径的文件名 */

STATUS BrdFpUnzipFile(char *pUnzipFileName, char *pOuputFileName);/* 解压指定的文件，并输出到指定的文件 */
BOOL   BrdFpUnzipFileIsExist(char *pUnzipFileName);/* 查询压缩包中指定的压缩文件是否存在 */

STATUS BrdFpPartition(TFlPartnParam *ptParam);     /* 分区 */
STATUS BrdFpGetFPtnInfo(TFlPartnParam *ptParam);   /* 读分区信息 */

INT32  BrdFpGetExecDataInfo(UINT32 *pdwExecAdrs, UINT32 *pdwExeCodeType);         /* 读取特殊可执行程序的类型、地址和大小 */
INT32  BrdFpReadExecData(UINT8 *pbyDesBuf, UINT32 dwLen); /* 读取特殊可执行程序数据 */
STATUS BrdFpWriteExecData(UINT32 dwExeCodeType, UINT8 *pbyData, UINT32 dwLen);  /* 写入特殊可执行程序数据 */
STATUS BrdFpEraseExeCode(void);    /* 擦除ExeCode特殊可执行程序数据 */

INT32  BrdFpGetAuxDataInfo(UINT32 *pdwDataOffset, char *pchOsName, char *pchAppName);
INT32  BrdFpReadAuxData(UINT8 *pbyDesBuf, UINT32 dwLen);/* 读取aux可执行程序数据 */
STATUS BrdFpWriteAuxData(UINT8 *pbyData, UINT32 dwLen, char *pchOsName, char *pchAppName);
STATUS BrdFpEraseAuxData(void);

UINT8  BrdFpGetUsrFpnNum (void);    /* 读取用户数据分区的个数 */
STATUS BrdFpWriteDataToUsrFpn(UINT8 bySection, UINT8 *pbyData, UINT32 dwLen);/* 写指定缓冲区数据到用户数据分区的指定段 */
INT32  BrdFpReadDataFromUsrFpn(UINT8 bySection, UINT8 *pbyDesBuf, UINT32 dwLen);/* 读取指定分区内指定段指定长度的有效数据到缓冲区 */

/* 以下为vx->linux动态升级模块部分的接口 */
#define LINUX_PARTN_PTABLE      0  /* 分区表 */
#define LINUX_PARTN_IOS         1  /* 操作系统数据分区 */
#define LINUX_PARTN_APP         2  /* 应用程序数据分区 */
#define LINUX_PARTN_USRDATA     3  /* 用户数据分区 */
#define LINUX_PARTN_LOGO        4  /* LOGO数据分区 */
#define LINUX_PARTN_JFFS2       5  /* 表示jffs2文件系统分区 */

STATUS  Vx2L_Init(void);
STATUS  Vx2L_ClearFlash(void);
STATUS  Vx2L_WrtFlashPartn(UINT8 byPartnID, char *pbySrcFile, char *pbyLinuxFile);
STATUS  Vx2L_UpdateBoot(char *pbyBootFile);

#ifdef __cplusplus
}
#endif

#endif /* __INC_FP_DRVLIB_H */