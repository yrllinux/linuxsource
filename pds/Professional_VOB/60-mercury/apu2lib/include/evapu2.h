/*****************************************************************************
模块名      : apu2lib
文件名      : evapu2.h
创建时间    : 2012年 02月 14日
实现功能    : apu2内部事件定义
作者        : 周嘉麟
版本        : 1.0
******************************************************************************/

#ifndef _EVENT_APU2_H
#define _EVENT_APU2_H
#include "eventid.h"
#include "evmcueqp.h"
#include "eapuautotest.h"

/***************************混音器内部消息**************************/

//混音器初始化,消息体: TApu2EqpCfg
OSPEVENT(EV_MIXER_INIT,                 EV_MIXER_BGN + 1 ); 
//显示混音器数据
OSPEVENT(EV_MIXER_SHOWMIX,              EV_MIXER_BGN + 2 );
//显示通道信息
OSPEVENT(EV_MIXER_SHOWCHINFO,			EV_MIXER_BGN + 3 );
//显示混音编解码信息
OSPEVENT(EV_MIXER_SHOWSTATE,			EV_MIXER_BGN + 4 );

//适配器初始化,消息体: TApu2EqpCfg
OSPEVENT(EV_BAS_INIT,					EV_BAS_BGN + 1 );
//显示适配器数据
OSPEVENT(EV_BAS_SHOWBAS,				EV_BAS_BGN + 2 );
//显示通道信息
OSPEVENT(EV_BAS_SHOWCHINFO,				EV_BAS_BGN + 3 );

/**************************生产测试内部消息*************************/
//生产测试初始化
OSPEVENT(EV_TEST_INIT,                  EV_MIXER_BGN + 10 ); 
//生产测试点灯(测试6个灯，预留6条消息)
OSPEVENT(TIMER_TEST_LED,                EV_MIXER_BGN + 11 ); 

/*BAS定义*/
#define  MAX_AUDIO_FRAME_SIZE		(u32)8 * 1024			//接收大小
#define  MAXNUM_BASCHN				(u8)1					//BAS抽象成一个通道
#define  MAXNUM_NMODECHN			(u8)4					//N模式通道最大支持
#define  MAXNUM_MIXERCHN			(u8)1					//Apu2抽象成一个通道
#define  MAXVALUE_APU2_VOLUME		(u8)31					//Apu2混音器最大音量值
#define  MAX_AUDIO_FRAME_SIZE		(u32)8 * 1024			//接收大小
#define  APU2_TEST_LEDNUM			(u8)6					//Apu2生产测试点灯数
#define  TEST_LED_TIMER_INTERVAL	(u32)2 * 1000			//点灯等待间隔

/*错误码*/
#define ERR_BAS_NOERROR				( ERR_BAS_BGN + 0 )		//无错误						
#define ERR_BAS_CHNOTREAD           ( ERR_BAS_BGN + 1 )		//当前通道不在就绪状态
#define ERR_BAS_CHRUNING            ( ERR_BAS_BGN + 2 )		//当前通道正在运行 
#define ERR_BAS_CHREADY             ( ERR_BAS_BGN + 3 )		//当前通道正在待命 
#define ERR_BAS_CHIDLE              ( ERR_BAS_BGN + 4 )		//当前通道为空     
#define ERR_BAS_OPMAP               ( ERR_BAS_BGN + 5 )		//调用底层函数失败
#define ERR_BAS_ERRCONFID           ( ERR_BAS_BGN + 6 )		//错误的会议
#define ERR_BAS_NOENOUGHCHLS        ( ERR_BAS_BGN + 7 )		//通道数不够
#define ERR_BAS_GETCFGFAILED		( ERR_BAS_BGN + 8 )		//读取配置失败

#define ERR_MIXER_NONE              ( ERR_MIXER_BGN + 0 )	//无错误 
#define ERR_MIXER_CALLDEVICE        ( ERR_MIXER_BGN + 1 )	//调用底层函数失败
#define ERR_MIXER_MIXING            ( ERR_MIXER_BGN + 2 )	//正在混音状态
#define ERR_MIXER_NOTMIXING         ( ERR_MIXER_BGN + 3 )	//未混音状态
#define ERR_MIXER_ERRCONFID         ( ERR_MIXER_BGN + 4 )	//无效的会议ID
#define ERR_MIXER_CHNINVALID        ( ERR_MIXER_BGN + 5 )	//添加通道失败
#define ERR_MIXER_STATUIDLE         ( ERR_MIXER_BGN + 6 )	//混音器状态空闲

/*操作宏*/
#define SAFE_DELETE(ptr) 	\
{	\
	if(NULL != ptr)	\
	{	\
		delete ptr;	\
		ptr = NULL;	\
	}	\
} 
#define SAFE_DELETE_ARRAY(ptr) 	\
{	\
	if(NULL != ptr)	\
	{	\
		delete []ptr;	\
		ptr = NULL;	\
	}	\
} 

/*安全过滤宏*/
#define PTR_NULL_VOID(param) 	\
{	\
	if ( NULL == param )	\
	{	\
		return;	\
	}	\
}

#define PTR_NULL_RETURN(param, out) 	\
{	\
	if ( NULL == param )	\
	{	\
		return out;	\
	}	\
}

#define PTR_NULL_CONTINUE(param) 	\
{	\
	if ( NULL == param )	\
	{	\
		continue;	\
	}	\
}

/*接口调用失败*/


#endif