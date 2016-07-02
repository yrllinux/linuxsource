 /*****************************************************************************
   模块名      : MP
   文件名      : evmp.h
   相关文件    : 
   文件实现功能: MP与MCU的消息接口定义
   作者        : 胡昌威
   版本        : V0.1  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/07/11  0.1         胡昌威        创建
******************************************************************************/
#ifndef __EVMP_H_
#define __EVMP_H_

#include "osp.h"
#include "eventid.h"

//MP向MCU的注册请求,消息体为TMp
OSPEVENT(MP_MCU_REG_REQ,                    EV_MP_BGN);
//MCU接受MP的注册请求,消息体为TMp
OSPEVENT(MCU_MP_REG_ACK,                    EV_MP_BGN+1);
//MCU拒绝MP的注册请求,消息体无
OSPEVENT(MCU_MP_REG_NACK,                   EV_MP_BGN+2);

//MCU发给MP的增加交换请求,消息体为TSwitchChannel
OSPEVENT(MCU_MP_ADDSWITCH_REQ,              EV_MP_BGN+4);
//MP接受MCU的增加交换请求,消息体为TSwitchChannel
OSPEVENT(MP_MCU_ADDSWITCH_ACK,              EV_MP_BGN+5);
//MP拒绝MCU的增加交换请求,消息体无
OSPEVENT(MP_MCU_ADDSWITCH_NACK,             EV_MP_BGN+6);

//MCU发给MP的移除交换请求,消息体为TSwitchChannel
OSPEVENT(MCU_MP_REMOVESWITCH_REQ,           EV_MP_BGN+8);
//MP接受MCU的移除交换请求,消息体为TSwitchChannel
OSPEVENT(MP_MCU_REMOVESWITCH_ACK,           EV_MP_BGN+9);
//MP拒绝MCU的移除交换请求,消息体无
OSPEVENT(MP_MCU_REMOVESWITCH_NACK,          EV_MP_BGN+10);

//MCU发给MP的得到交换状态请求,消息体为TMp
OSPEVENT(MCU_MP_GETSWITCHSTATUS_REQ,        EV_MP_BGN+12);
//MP发给MCU的得到状态应答,消息体为TSwitchChannel数组
OSPEVENT(MP_MCU_GETSWITCHSTATUS_ACK,        EV_MP_BGN+13);
//MP拒绝应答MCU的得到状态请求,消息体无
OSPEVENT(MP_MCU_GETSWITCHSTATUS_NACK,       EV_MP_BGN+14);

//MP开始连接MCU,消息体无
OSPEVENT(EV_MP_CONNECT_MCU,                 EV_MP_BGN+16);
//MP开始连接MCU,消息体无
OSPEVENT(EV_MP_CONNECT_MCUB,                EV_MP_BGN+17);
//MP开始向MCU注册，消息体无
OSPEVENT(EV_MP_REGISTER_MCU,                EV_MP_BGN+18);
//MP开始向MCU注册，消息体无
OSPEVENT(EV_MP_REGISTER_MCUB,               EV_MP_BGN+19);

//MP会话通知MCU断链
OSPEVENT(MCU_MP_DISCONNECTED_NOTIFY,        EV_MP_BGN+20);

//MCU通知MP会议结束
OSPEVENT(MCU_MP_RELEASECONF_NOTIFY,         EV_MP_BGN+22);

// 会议归一化通知
OSPEVENT(MCU_MP_CONFUNIFORMMODE_NOTIFY,     EV_MP_BGN+23);

// MP 取主备倒换状态
OSPEVENT(MP_MCU_GETMSSTATUS_REQ,            EV_MP_BGN+25);
// Mcu通知Mp主备倒换状态 消息体：TMcuMsStatus
OSPEVENT(MCU_MP_GETMSSTATUS_ACK,            EV_MP_BGN+26);
// 消息体暂无
OSPEVENT(MCU_MP_GETMSSTATUS_NACK,           EV_MP_BGN+27);

//MCU发给MP的增加多点到一点交换请求,消息体为TSwitchChannel + dwMapIpAddr + wMapPort
OSPEVENT(MCU_MP_ADDMULTITOONESWITCH_REQ,    EV_MP_BGN+30);
//MP接受MCU的增加多点到一点交换请求,消息体为TSwitchChannel
OSPEVENT(MP_MCU_ADDMULTITOONESWITCH_ACK,    EV_MP_BGN+31);
//MP拒绝MCU的增加多点到一点交换请求,消息体无
OSPEVENT(MP_MCU_ADDMULTITOONESWITCH_NACK,   EV_MP_BGN+32);

//MCU发给MP的移除多点到一点交换请求,消息体为TSwitchChannel
OSPEVENT(MCU_MP_REMOVEMULTITOONESWITCH_REQ, EV_MP_BGN+35);
//MP接受MCU的移除多点到一点交换请求,消息体为TSwitchChannel
OSPEVENT(MP_MCU_REMOVEMULTITOONESWITCH_ACK, EV_MP_BGN+36);
//MP拒绝MCU的移除多点到一点交换请求,消息体无
OSPEVENT(MP_MCU_REMOVEMULTITOONESWITCH_NACK,EV_MP_BGN+37);

//MCU发给MP的停止多点到一点交换请求,消息体为TSwitchChannel
OSPEVENT(MCU_MP_STOPMULTITOONESWITCH_REQ,   EV_MP_BGN+40);
//MP接受MCU的停止多点到一点交换请求,消息体为TSwitchChannel
OSPEVENT(MP_MCU_STOPMULTITOONESWITCH_ACK,   EV_MP_BGN+41);
//MP拒绝MCU的停止多点到一点交换请求,消息体无
OSPEVENT(MP_MCU_STOPMULTITOONESWITCH_NACK,  EV_MP_BGN+42);

//MCU发给MP的增加只接收交换请求,消息体为TSwitchChannel
OSPEVENT(MCU_MP_ADDRECVONLYSWITCH_REQ,      EV_MP_BGN+45);
//MP接受MCU的增加只接收交换请求,消息体为TSwitchChannel
OSPEVENT(MP_MCU_ADDRECVONLYSWITCH_ACK,      EV_MP_BGN+46);
//MP拒绝MCU的增加只接收交换请求,消息体无
OSPEVENT(MP_MCU_ADDRECVONLYSWITCH_NACK,     EV_MP_BGN+47);

//MCU发给MP的移除只接收交换请求,消息体为TSwitchChannel
OSPEVENT(MCU_MP_REMOVERECVONLYSWITCH_REQ,   EV_MP_BGN+50);
//MP接受MCU的移除只接收交换请求,消息体为TSwitchChannel
OSPEVENT(MP_MCU_REMOVERECVONLYSWITCH_ACK,   EV_MP_BGN+51);
//MP拒绝MCU的移除只接收交换请求,消息体无
OSPEVENT(MP_MCU_REMOVERECVONLYSWITCH_NACK,  EV_MP_BGN+52);

//MCU发给MP的变更接收交换ssrc请求,消息体为TSwitchChannel+byChangeSSRC(u8)
OSPEVENT(MCU_MP_SETRECVSWITCHSSRC_REQ,      EV_MP_BGN+55);
//MP接受MCU的变更接收交换ssrc请求,消息体为TSwitchChannel
OSPEVENT(MP_MCU_SETRECVSWITCHSSRC_ACK,      EV_MP_BGN+56);
//MP拒绝MCU的变更接收交换ssrc请求,消息体无
OSPEVENT(MP_MCU_SETRECVSWITCHSSRC_NACK,     EV_MP_BGN+57);

//MCU发给MP的增加广播源请求,消息体为TSwitchChannel
OSPEVENT(MCU_MP_ADDBRDSRCSWITCH_REQ,        EV_MP_BGN+60);
//MP接受MCU的增加广播源请求,消息体为TSwitchChannel
OSPEVENT(MP_MCU_ADDBRDSRCSWITCH_ACK,        EV_MP_BGN+61);
//MP拒绝MCU的增加广播源请求,消息体无
OSPEVENT(MP_MCU_ADDBRDSRCSWITCH_NACK,       EV_MP_BGN+62);

//MP 某中转广播源 释放后给MCU的通知,消息体为 TMt(释放前其所转发的视频源)
OSPEVENT( MP_MCU_BRDSRCSWITCHCHANGE_NTF,        EV_MP_BGN + 49 );

//MCU发给MP的移除广播源请求,消息体为TSwitchChannel
OSPEVENT(MCU_MP_REMOVEBRDSRCSWITCH_REQ,     EV_MP_BGN+65);
//MP接受MCU的移除广播源请求,消息体为TSwitchChannel
OSPEVENT(MP_MCU_REMOVEBRDSRCSWITCH_ACK,     EV_MP_BGN+66);
//MP拒绝MCU的移除广播源请求,消息体无
OSPEVENT(MP_MCU_REMOVEBRDSRCSWITCH_NACK,    EV_MP_BGN+67);

//MCU发给MP的增加广播目标请求,消息体为TSwitchChannel
OSPEVENT(MCU_MP_ADDBRDDSTSWITCH_REQ,        EV_MP_BGN+70);
//MP接受MCU的增加广播源请求,消息体为TSwitchChannel
OSPEVENT(MP_MCU_ADDBRDDSTSWITCH_ACK,        EV_MP_BGN+71);
//MP拒绝MCU的增加广播源请求,消息体无
OSPEVENT(MP_MCU_ADDBRDDSTSWITCH_NACK,       EV_MP_BGN+72);

/*
//MCU发给MP的移除广播源请求,消息体为TSwitchChannel
OSPEVENT(MCU_MP_REMOVEBRDDSTSWITCH_REQ,     EV_MP_BGN+75);
//MP接受MCU的移除广播源请求,消息体为TSwitchChannel
OSPEVENT(MP_MCU_REMOVEBRDDSTSWITCH_ACK,     EV_MP_BGN+76);
//MP拒绝MCU的移除广播源请求,消息体无
OSPEVENT(MP_MCU_REMOVEBRDDSTSWITCH_NACK,    EV_MP_BGN+77);
*/
//mcu获取mp交换码流收包情况。
OSPEVENT(MCU_MP_GETDSINFO_REQ,				EV_MP_BGN+80);
OSPEVENT(MP_MCU_GETDSINFO_ACK,				EV_MP_BGN+81);
OSPEVENT(MP_MCU_GETDSINFO_NACK,				EV_MP_BGN+82);

//给自己发哑音包
OSPEVENT(MCU_MP_ADDSENDSELFMUTEPACK_CMD,	EV_MP_BGN+83);
OSPEVENT(MCU_MP_DELSENDSELFMUTEPACK_CMD,	EV_MP_BGN+84);

// 内部状态统计定时器
OSPEVENT(EV_MP_NETSTAT,                     EV_MP_BGN+90);

// 内部能力监测定时器
OSPEVENT(EV_MP_FLUXOVERRUN,					EV_MP_BGN+91);

//能力超限通知过滤定时器
OSPEVENT(EV_MP_SENDFLUXOVERRUNNOTIFY,	    EV_MP_BGN+92);

// MP当前实际流量通知上报。消息体：u16 当前真实占用的网络带宽
OSPEVENT(MP_MCU_FLUXSTATUS_NOTIFY,			EV_MP_BGN+93);

// 取主备倒换状态定时器
OSPEVENT(EV_MP_GETMSSTATUS_TIEMR,	        EV_MP_BGN+94);
// 交换表分页打印定时器
OSPEVENT(EV_SWITCHTABLE_PRTNEXTPAGE_TEMER,  EV_MP_BGN+97);
// 增加交换表(测试用)
OSPEVENT(EV_MP_ADDTABLE,                    EV_MP_BGN+95);
// 清空交换表(测试用)
OSPEVENT(EV_MP_CLEARTABLE,                  EV_MP_BGN+96);
// 处理缓存表的相关状态
OSPEVENT( EV_MP_DEAL_CALLBACK_NTF,          EV_MP_BGN + 98 );
// 切换视频源超时定时器
OSPEVENT( EV_MP_CHGSRCTOT_TIMER,            EV_MP_BGN + 99 );

//通知Mp打洞消息 消息体:u32(LocalIp) + u16(LocalPort) + u32(RemoteIp) + u16(RemotePort) + u8(HoleType) + u8(PayLoad)
OSPEVENT( MCU_MP_PINHOLE_NOTIFY,			EV_MP_BGN + 100);
//通知Mp补洞消息 消息体: u8(补洞数) + u16(LocalPort) + u32(RemoteIp) + u16(RemotePort) + ...可接多个
OSPEVENT( MCU_MP_STOPPINHOLE_NOTIFY,		EV_MP_BGN + 101);

#endif

