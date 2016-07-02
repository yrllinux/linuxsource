/*****************************************************************************
   模块名      : VAS
   文件名      : VasEventDef.h
   相关文件    : 
   文件实现功能: vas代理和界面间消息定义
   作者        : 
   版本        : 
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容

******************************************************************************/
#ifndef _VAS_EVENT_DEF_H
#define _VAS_EVENT_DEF_H

#include "eventid.h"



//=============================================
//常量定义
//==============================================
const u8 AID_VASLIB = 110 ; //	lib的instance号
const u8 AID_VASINTERFACE =111;// 界面的instance号

const u8 VAS_LIB_VIDEO = 0;
const u8 VAS_LIB_AUDIO = 1;
const u8 VAS_LIB_BOTH = 2;

const u8 OUTPUT_LIB_ON = 1;
const u8 OUTPUT_LIB_OFF = 0;



const u8 EVENT_CONNECT_NOTIFY_OK =1;//回调函数通知
const u8 EVENT_CONNECT_NOTIFY_FAIL =2;
const u8 EVENT_GET_CONFIG_NOTIFY =3;


//定义矩阵最大输入通道数
#define MAX_MATRIX_INPUTCHANN	16
//定义矩阵最大输入通道数（lib）
#define MAXNUB_AVCHAN    16
//定义通道的名字（lib）
#define MAXLEN_CHANNAME  16


#define		VAS_LOG_ERROR		0
//==========================================
//消息定义
//==========================================

//启动消息UI->LIB, 消息体为TVasStart
OSPEVENT(EV_VAS_LIB_START_CMD,								EV_MT_BGN + 1000);

//启动结果LIB->UI，u8为失败原因。如果为u8为0，说明成功
OSPEVENT(EV_VAS_LIB_START_IND,								EV_MT_BGN + 1009);

//轮询指示,LIB->UI，u8为当前轮训到的channel号,将这个号码在图形界面上显示出来
OSPEVENT(EV_VAS_LIB_ROLLING_IND,								EV_MT_BGN + 1001);

//轮询,UI->LIB，消息体为TVasRolling
OSPEVENT(EV_VAS_LIB_ROLLING_CMD,								EV_MT_BGN + 1002);

//实施该项配置,UI->LIB，消息体为TVasConfigu
OSPEVENT(EV_VAS_LIB_APPLY_CMD,								EV_MT_BGN + 1003);

//取消论询，UI->LIB
OSPEVENT(EV_VAS_LIB_CANCEL_ROLLING_CMD,								EV_MT_BGN + 1004);

//请求配置文件UI->LIB,消息体为TVasConfiguAsk
OSPEVENT(EV_VAS_LIB_ASK_CONFIGUE_REQ,								EV_MT_BGN + 1005 );

//得到配置文件LIB->UI，消息体为TVasConfigu结构
OSPEVENT(EV_VAS_LIB_GET_CONFIGUE_RSP,								EV_MT_BGN + 1006 );

//保存当前配置UI->LIB,消息体为TVasConfigu结构
OSPEVENT(EV_VAS_LIB_SAVE_CONFIGUE_CMD,								EV_MT_BGN + 1007);

//进行VAS交换，UI->LIB, 消息体为TVasSwitcher
OSPEVENT(EV_VAS_LIB_SWITCH_CMD,								EV_MT_BGN + 1008);

//EV_MT_BGN + 1009 已经被使用

////////////////////////////////////////
//保存界面窗口句柄, 消息体为HWND
OSPEVENT(EV_VAS_LIB_SET_HWND_CMD,								EV_MT_BGN + 1010);

//保存回调函数, 消息体为TVasCallBack
OSPEVENT(EV_VAS_LIB_CALLBACK_CMD,								EV_MT_BGN + 1011);

#endif // _VAS_EVENT_DEF_H



