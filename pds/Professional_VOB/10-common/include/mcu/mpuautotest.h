#ifndef MPUHARDWARE_AUTO_TEST_H
#define MPUHARDWARE_AUTO_TEST_H

#include "eventid.h"
#include "kdvtype.h"
#include "osp.h"
#include "mcuconst.h"

#define  SERIAL_FILE_LEN                100
#define  CONNECT_TIMER_LENGTH           (10*1000)
#define  VERSION_LEN                    32

#define MPU_SERVER_APPID            101             // 测试服务器APPID号

//进行测试的模式
#define		TYPE_MPUSVMP				11	 //MPU-VMP
#define		TYPE_MPUBAS					13	 //MPU-BAS

//MPU接受码流起始端口 
#define MPUTEST_RECVBASEPORT	    VMP_EQP_STARTPORT		//(VMP)
#define MPUTEST_BAS_RECVBASEPORT	BAS_EQP_STARTPORT		//(BAS)

//MPU发送到测试服务器用的端口 
#define MPUTEST_SENDSVRPORT			VMP_MCU_STARTPORT + PORTSPAN	//(vmp)该路对应720p			
#define MPUTEST_BAS_SENDSVRPORT		BAS_MCU_STARTPORT				//(BAS)


/*MPU自动检测消息*/ 
//消息体：	  u8 测试的模式（VMP/BAS）+ u8 *31  空	+ 用户信息 + 设备信息
#define C_S_MPULOGIN_REQ                     EV_MPUAUTOTEST_BGN       // 注册请求
#define S_C_MPULOGIN_ACK                     EV_MPUAUTOTEST_BGN + 1       // ACK
#define S_C_MPULOGIN_NACK                    EV_MPUAUTOTEST_BGN + 2      // NACK


#define  C_S_MPUREADY_NOTIF                 EV_MPUAUTOTEST_BGN + 3      //(仅VMP用)通知服务器可以开始发码流

#define  S_C_MPURESTORE_CMD                 EV_MPUAUTOTEST_BGN + 5      //恢复默认值消息
#define  C_S_MPURESTORE_NOTIF               EV_MPUAUTOTEST_BGN + 6      //恢复成功消息

//消息头：channelIdx 
//(对VMP而言，只需填1; 对BAS而言，根据测试项分别填0，1，2，3)
#define  S_C_FASTUPDATEPIC_CMD				EV_MPUAUTOTEST_BGN + 7		  //服务器向MPU请求关键帧

//----------MPU-BAS 新增-----------
//某BAS 通道准备就绪
//消息头： channelIdx ( 0~3, 收到3后表所有通道准备就绪 ) 
#define  C_S_BASCHNL_NOTIF					EV_MPUAUTOTEST_BGN + 8		

//服务器请求开始测试
#define  S_C_STARTBASTEST_REQ				EV_MPUAUTOTEST_BGN + 9
#define  C_S_STARTBASTEST_ACK				EV_MPUAUTOTEST_BGN + 10		//消息头： channel id
#define  C_S_STARTBASTEST_NACK				EV_MPUAUTOTEST_BGN + 11
	 
//改变测试模式（VMP->BAS）
//消息体：u8 byMode： 11 --表转成VMP模式； 13 -- 表转成BAS模式 （见之前的宏定义）
#define  S_C_CHANGETESTMODE_CMD				EV_MPUAUTOTEST_BGN + 12		

#define S_C_TEST_OUS_CMD                    EV_MPUAUTOTEST_BGN + 13

#define S_C_STARTVMPTEST_REQ                EV_MPUAUTOTEST_BGN + 14 //测试服务器开始测试请求
#define C_S_STARTVMPTEST_ACK                EV_MPUAUTOTEST_BGN + 15
#define C_S_STARTVMPTEST_NACK                EV_MPUAUTOTEST_BGN + 16

const s8 achServIpAddr[] = "10.1.1.1";

#endif