/*****************************************************************************
   模块名      : raw flash
   文件名      : fpServer.h
   相关文件    : 
   文件实现功能: 
   作者        : 张方明
   版本        : V1.0  Copyright(C) 2003-2004 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2005/4/23    1.0         张方明        创建
******************************************************************************/

#ifndef __INC_FP_SERVER_H
#define __INC_FP_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

/* 头文件 */

/* 宏定义 */
#define FS_SERVER_PORT     2910
#define FS_SERVER_IP       0x01010101   /* "1.1.1.1" */

#define FS_EVENT_MAGIC_NUMBER       0x5cc5c555

#define FS_EVENT_UPDATE_AUXDATA     1
#define FS_EVENT_UPDATE_FPGA        2
#define FS_EVENT_REBOOT             0xf515f717

#define FS_ACT_OPT_MSG_VALID             0xaa5555a5
#define FS_ACT_OPT_MSG_INVALID           0xee5555e5

#define FS_ACT_OPT_FINISH_OK             0x0
#define FS_ACT_OPT_ERR_PARAM_INVALID     0x1
#define FS_ACT_OPT_ERR_NOT_FOUND_FILE    0x2
#define FS_ACT_OPT_ERR_FILE_LEN_ERR      0x3
#define FS_ACT_OPT_ERR_NO_MEMORY         0x4
#define FS_ACT_OPT_ERR_WRT_FAILE         0x5
#define FS_ACT_OPT_ERR_UNKNOWN           -1

#define FS_OPT_FILE_NAME_MAXLEN          32
#define FS_OS_FILE_NAME_MAXLEN           16
#define FS_APP_FILE_NAME_MAXLEN          16

#define FS_OSORAPP_FILE_NAME_USE_DEF     "default"

/* struct */

/* 客户端到服务端的消息结构 */
typedef struct 
{
    int  iMagic;                                 /* 幻数，表示是我的消息,填FS_EVENT_MAGIC_NUMBER */
    int  iEvent;                                 /* 事件，表示不同的操作类型 */
    char acOptFileName[FS_OPT_FILE_NAME_MAXLEN]; /* 操作的文件名，含全路经 */
    char acOsFileName[FS_OS_FILE_NAME_MAXLEN];   /* os操作系统的文件名，default时使用默认值 */
    char acAppFileName[FS_APP_FILE_NAME_MAXLEN]; /* app应用程序的文件名，default时使用默认值 */
}TBootMsgFromClient;

/* 服务端到客户端的消息结构 */
typedef struct 
{
    int  iMagic;           /* 幻数，表示是我的消息 */
    int  iEvent;           /* 事件，表示不同的操作类型 */
    int  iOptRtn;          /* 操作结果 */
}TBootMsgToClient;

#ifdef __cplusplus
}
#endif

#endif /* __INC_FP_SERVER_H */