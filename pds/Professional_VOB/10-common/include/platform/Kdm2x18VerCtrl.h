/******************************************************************************
模块名  ：Kdm2x18Os
文件名	：kdm2x18.h
相关文件：无
文件实现功能：KDM2x18 网络升级功能的接口定义
作者	：王俊华
版本	：2.0
-------------------------------------------------------------------------------
修改记录:
日  期		版本		修改人		修改内容
11/17/2004	1.0         王俊华      创建
08/22/2005	2.0         钮月忠      标准化
******************************************************************************/

#ifndef _KDM2X18VERCTRL_H_
#define _KDM2X18VERCTRL_H_ 

#define KDM2X18VERCTRL_LISTEN_PORT        1721          /*监听端口*/
#define KDM2X18VERCTRL_MAX_CONTENT_LEN    4000          /*消息体的最大长度*/
#define KDM2X18VERCTRL_APP_NAME           "kdmvs"       /*应用程序名*/

/*消息结构定义*/
#pragma pack(push, 1)
typedef struct tagMessage
{
	u8 byCode;                                   /*消息码*/
	u8 byReserved1;                              /*保留*/
	u8 byReserved2;                              /*保留*/
	u8 byReserved3;                              /*保留*/
	u32 dwValidData;                             /*消息长度*/
	u8 abyData[KDM2X18VERCTRL_MAX_CONTENT_LEN];  /*消息内容*/
} TMessage, *PTMessage;
#pragma pack(pop)

/*消息码定义*/
#define MESSAGE_BASE                           0                 /*消息码的基值*/

/*开始下载文件消息*/
#define CLIENT_AGENT_DOWN_START_REQ            MESSAGE_BASE+1
#define AGENT_CLIENT_DOWN_START_ACK            MESSAGE_BASE+2
#define AGENT_CLIENT_DOWN_START_NACK           MESSAGE_BASE+3

/*传送文件消息*/
#define CLIENT_AGENT_DOWN_SND_DATA             MESSAGE_BASE+4
#define AGENT_CLIENT_DOWN_RCV_OK               MESSAGE_BASE+5
#define AGENT_CLIENT_DOWN_RCV_ERROR            MESSAGE_BASE+6

/*停止传送文件消息*/
#define CLIENT_AGENT_DOWN_STOP_REQ             MESSAGE_BASE+7
#define AGENT_CLIENT_DOWN_STOP_ACK             MESSAGE_BASE+8
#define AGENT_CLIENT_DOWN_STOP_NACK            MESSAGE_BASE+9

/*传送完一个文件通知消息*/
#define CLIENT_AGENT_DOWN_FINISHONE_NOTIFY     MESSAGE_BASE+10
#define AGENT_CLIENT_DOWN_FINISHONE_ACK        MESSAGE_BASE+11
#define AGENT_CLIENT_DOWN_FINISHONE_NACK       MESSAGE_BASE+12

/*服务端报告烧写文件进度消息*/
#define AGENT_CLIENT_WRITE_PROCESS_NOTIFY      MESSAGE_BASE+13
#define CLIENT_AGENT_WRITE_PROCESS_ACK         MESSAGE_BASE+14
#define CLIENT_AGENT_WRITE_PROCESSE_NACK       MESSAGE_BASE+15

/*服务端烧写完一个文件通知消息*/
#define AGENT_CLIENT_WRITE_FINISH_NOTIFY       MESSAGE_BASE+16
#define CLIENT_AGENT_WRITE_FINISH_ACK          MESSAGE_BASE+17
#define CLIENT_AGENT_WRITE_FINISH_NACK         MESSAGE_BASE+18

/*传送完所有文件通知消息*/
#define CLIENT_AGENT_DOWN_FINISHALL_NOTIFY     MESSAGE_BASE+19
#define AGENT_CLIENT_DOWN_FINISHALL_ACK        MESSAGE_BASE+20
#define AGENT_CLIENT_DOWN_FINISHALL_NACK       MESSAGE_BASE+21

/*服务端烧写完所有文件通知消息*/
#define AGENT_CLIENT_WRITE_FINISHALL_NOTIFY    MESSAGE_BASE+22
#define CLIENT_AGENT_WRITE_FINISHALL_ACK       MESSAGE_BASE+23
#define CLIENT_AGENT_WRITE_FINISHALL_NACK      MESSAGE_BASE+24

/*服务端烧写失败通知*/
#define AGENT_CLIENT_WRITE_FAILED_NOTIFY       MESSAGE_BASE+25

/*客户端下载配置文件请求*/
#define CLIENT_AGENT_DOWN_CONFIG_START_REQ     MESSAGE_BASE+26
#define AGENT_CLIENT_DOWN_CONFIG_START_ACK     MESSAGE_BASE+27
#define AGENT_CLIENT_DOWN_CONFIG_START_NACK    MESSAGE_BASE+28

/*服务端传送配置文件数据消息*/
#define AGENT_CLIENT_DOWN_CONFIG_SND_DATA      MESSAGE_BASE+29
#define CLIENT_AGENT_DOWN_CONFIG_RCV_OK        MESSAGE_BASE+30
#define CLIENT_AGENT_DOWN_CONFIG_RCV_ERROR     MESSAGE_BASE+31

/*服务端传送配置文件完毕消息*/
#define AGENT_CLIENT_DOWN_CONFIG_FINISH_NOTIFY MESSAGE_BASE+32
#define CLIENT_AGENT_DOWN_CONFIG_FINISH_ACK    MESSAGE_BASE+33
#define CLIENT_AGENT_DOWN_CONFIG_FINISH_NACK   MESSAGE_BASE+34

/*服务端命令重启*/
#define CLIENT_AGENT_REBOOT_NOTIFY             MESSAGE_BASE+35

#endif /*_KDM2X18VERCTRL_H_*/
