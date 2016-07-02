/******************************************************************************
模块名  ：Kdm2x18Os
文件名	：kdm2x18Answer.h
相关文件：无
文件实现功能：KDM2x18 网络搜索应答功能的接口定义
作者	：钮月忠
版本	：1.0
-------------------------------------------------------------------------------
修改记录:
日  期		版本		修改人       走读人		修改内容
08/23/2005	1.0         钮月忠       向  飞     标准化
******************************************************************************/

#ifndef _KDM2X18ANSWER_H_
#define _KDM2X18ANSWER_H_ 

#define KDM2X18ANSWER_BIND_PORT       5512             /*绑定端口*/
#define KDM2X18ANSWER_MSG_INQUIRE     "Who is online?" /*控制台发送的查询消息*/
#define KDM2X18ANSWER_MULTICAST_GROUP "224.16.32.1"    /*组播组地址*/

/*服务端返回的消息结构*/
#pragma pack(push, 4)
typedef struct tagAnswer
{
    u8  byDevId;       /*设备号：KDM2418=2,KDM2518=3*/
    u32 dwIp;          /*本地的IP地址*/ 
    u8  byType;        /*默认 0*/ 
    u8  abyNettype[2]; /*默认 0*/
    u32 dwPppip[2];    /*默认 0*/
    u16 wSrport[2];    /*默认 0*/
    u8  bySinordou;    /*默认 1*/
} TAnswer, *PTAnswer;
#pragma pack(pop)

#endif /*_KDM2X18ANSWER_H_*/
