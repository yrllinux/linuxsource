/******************************************************************************
模块名  ： DHCP-CLIENT
文件名  ： boardsntp.h
相关文件：
文件实现功能：SNTP-CLIENT模块对外提供的宏定义头文件
作者    ：王俊华
版本    ：1.0.0.0.0
---------------------------------------------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
07/14/2006  1.0         王俊华      创建
******************************************************************************/
#ifndef _BOARDSNTP_H
#define _BOARDSNTP_H

#ifdef __cplusplus 
extern "C" { 
#endif /* __cplusplus */


/*宏定义*/

/*请求与SNTP-Servr进行时间同步的时间间隔的最小值*/
#define SNTP_INTERVEL_MIN                       (u32)64
/*请求与SNTP-Servr进行时间同步的时间间隔的最大值*/
#define SNTP_INTERVEL_MAX                       (u32)1024
 

/*SNTP中调用主动向SNTP-Server同步时间时回调函数的消息码基址*/
#define SNTP_MSG_BASE			                (u32)40000
/*向SNTP-Server同步时间校时成功，消息体：无*/
#define SNTP_SYNC_TIME_SUCCESS_MSG				(u32)( SNTP_MSG_BASE+1)
/*向SNTP-Server同步时间校时失败，消息体：无*/
#define SNTP_SYNC_TIME_FAILED_MSG				(u32)( SNTP_MSG_BASE+2)
/*向SNTP-Server同步时间校时超时，消息体：无*/
#define SNTP_SYNC_TIME_TIMEOUT_MSG				(u32)( SNTP_MSG_BASE+3)
/*向SNTP-Server同步时间校时系统级错误，消息体：无*/
#define SNTP_SYNC_TIME_SYSERR_MSG				(u32)( SNTP_MSG_BASE+4)


/*结构体定义*/


/*回调函数类型定义*/

/*================================
函数名：BrdSNTPSyncTimeCallBack
功能：调用主同向SNTP-Server同步系统时间时，通知用户操作结果的回调函数
算法实现：（可选项）
引用全局变量：
输入参数说明：dwMsgID：事件类型，见上面的消息类型定义
		      pMsg：与该事件类型相匹配的消息内容，目前都设置为NULL，以后可以
			  扩展
返回值说明： 暂时都定为0，以后可以扩展
==================================*/
typedef int(*BrdSNTPSyncTimeCallBack)(u32  dwMsgID ,void*  pMsg);


/*函数接口定义*/


/*================================
函数名：BrdSNTPStart
功能：开启SNTP-Client端功能
算法实现：（可选项）
引用全局变量：
输入参数说明：dwServerIp： SNTP-Server的IP地址，网络字节序
		      dwSyncTimeInterval：与SNTP-Server进行时间同步的时间间隔，以秒为单
位，范围：64s~1024s，开启该功能之后，底层会按
照用户设置的时间间隔，每隔一段时间便向
SNTP-Server同步系统时间并设置到本地系统中
返回值说明： OK/ERROR
==================================*/
STATUS BrdSNTPStart(u32  dwServerIp,  u32  dwSyncTimeInterval);

/*================================
函数名：BrdSNTPSyncTime
功能：除了底层定时向SNTP-Server同步时间之外，用户还可以在任何时间调用该接口主动
向SNTP-Server同步时间
算法实现：（可选项）
引用全局变量：
输入参数说明：dwTimeout：较时的超时值，以秒为单位，范围：64s~1024s
              ptCallBack：用户注册的，用以通知用户此次较时操作情况的回调函数
返回值说明： OK/ERROR
==================================*/
STATUS BrdSNTPSyncTime (u32  dwTimeout, BrdSNTPSyncTimeCallBack  ptCallBack);

/*================================
函数名：BrdSNTPStop
功能：关闭SNTP-Client端功能
算法实现：（可选项）
引用全局变量：
输入参数说明：无
返回值说明： OK/ERROR
==================================*/
STATUS BrdSNTPStop (void);





#ifdef __cplusplus 
} 
#endif /* __cplusplus */

#endif /*_BOARDSNTP_H*/
