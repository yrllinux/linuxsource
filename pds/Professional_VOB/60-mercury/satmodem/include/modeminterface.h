
#ifndef _MDCOM_INTERFACE_H_
#define _MDCOM_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "kdvtype.h"

//LOG_LVL
#define LVL_ERR							0
#define LVL_WARN						1
#define LVL_INFO						2
#define LVL_DETAIL						3
#define LVL_VERBOSE						4

#define MCMD_OK                 0               //成功
#define MCMD_FAIL               -1              //失败
#define MCMD_UNSURE             -2              //正在执行中

/* 此处的定义必须与命令表中的位置对应 */
#define  MDCOM_SET_MOD_FREQ             0       //设置发送频点
#define  MDCOM_SET_MOD_OUTPUT           1       //设置发送是否开始
#define  MDCOM_SET_MOD_BITRATE          2       //设置发送码率
#define  MDCOM_SET_MOD_CLKSRC           3       //设置发送时钟源

#define  MDCOM_SET_DEM_FREQ             4       //设置接收频点
#define  MDCOM_SET_DEM_BITRATE          5       //设置接收码率
#define  MDCOM_SET_DEM_CLKSRC           6       //设置接收时钟源


#define  MDCOM_SET_MAX                  7       //有效命令个数
#define  MDCOM_PIPEBUF_SIZE             1024    //管道缓冲中可以容纳消息的个

/* 查询命令均是对应的SET命令或上0x100 */
#define  MDCOM_QUERY_MOD_BITRATE                \
            MDCOM_SET_MOD_BITRATE  | 0x100 
#define  MDCOM_QUERY_MOD_FREQ                   \
            MDCOM_SET_MOD_FREQ     | 0x100      
#define  MDCOM_QUERY_MOD_OUTPUT                 \
            MDCOM_SET_MOD_OUTPUT   | 0x100 
#define  MDCOM_QUERY_MOD_CLKSRC                 \
            MDCOM_SET_MOD_CLKSRC   | 0x100 

#define  MDCOM_QUERY_DEM_BITRATE                \
            MDCOM_SET_DEM_BITRATE  | 0x100 
#define  MDCOM_QUERY_DEM_CLKSRC                 \
            MDCOM_SET_DEM_CLKSRC   | 0x100 
#define  MDCOM_QUERY_DEM_FREQ                   \
            MDCOM_SET_DEM_FREQ     | 0x100 

typedef u16 cmd_t;

typedef struct {
	cmd_t cmd;                      /* 命令号 */
	u32 ip; 
    u16 port;                       /*目的IP, Port*/
	u32 val;                        /* 要设置的值 */
} MdcomCmd;


int     CommandModem(const MdcomCmd *cmd, u8 trynum);     //向MODEM发送命令
int     CheckCmdStatus(MdcomCmd *cmd);                    //查询命令执行状态
int 	MdcomOutput(u8 byLvl, const char* fmt, ...);

#ifdef __cplusplus
}
#endif

#if defined(_MSC_VER)       // for Microsoft c++
#define API extern "C"  __declspec(dllexport)
#else                       // for gcc
#define API extern "C"
#endif

API int		MdcomInit		(void);                       //模块初始化
API void	MdcomTerm		(void);
API void	MdcomReport		(void); 
API void	MdcomMsgLog		(void);
API int		MdcomCommand	(const char* cmd, const char* ip, u16 port, u32 value); 
API int		MdcomCheck		(const char* cmd, const char* ip, u16 port);


#endif

