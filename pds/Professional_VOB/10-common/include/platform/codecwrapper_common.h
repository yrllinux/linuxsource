/*****************************************************************************
  模块名      : 编解码适配
  文件名      : codecwrapper_common.h
  相关文件    : 
  文件实现功能: 编解码适配
  作者        : 胡小鹏
  版本        : V1.0  Copyright(C) 1997-2009 KDC, All rights reserved.
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2009/03/16  V1.0        胡小鹏       创建
******************************************************************************/
#ifndef _CODECWRAPPER_COMMON_H_
#define _CODECWRAPPER_COMMON_H_

#include "algorithmtype.h"
#include "kdvdef.h"
#include "kdvtype.h"
#include "codeccommon_win32.h"
#include <memory.h>
#include "stdio.h"

#ifdef _LINUX_
    #define CODECWRAPPER_API

    #ifndef API
        #define API
    #endif
#endif

#ifdef WIN32
    #include "winsock.h"

    #ifndef CODECWRAPPER_API
        #define CODECWRAPPER_API __declspec(dllexport)
    #endif

    #ifndef API
        #define API __declspec(dllexport)
    #endif
#endif

#define QUALITY_PRIOR  0   //质量优先
#define SPEED_PRIOR    1   //速度优先
#define HPSINGLE_PRIO  2   //HP单线程
#define HPMULTE_PRIO   3   //HP多线程

//编码参数
typedef struct
{   
    u32 m_dwMaxKeyFrameInterval;//I帧间最大P帧数目
    u32 m_dwBitRate;            //编码比特率 (Kbit/s)
    u16 m_wEncVideoWidth;       //编码设置图像宽
    u16 m_wEncVideoHeight;      //编码设置图像高
    u8  m_byEncQuality;         //图像质量码率控制参数,质量优先或者速度优QUALITY_PRIOR / SPEED_PRIOR    
    u8  m_byMaxQuant;           //最大量化参数
    u8  m_byMinQuant;           //最小量化参数
    u8  m_byEncType;            //图像编码类型  
	u8  m_byMaxFrameRate;       //最大帧率
	u8  m_byMinFrameRate;		//最小帧率
}TVidEncParam;

typedef struct
{
    u32   dwIp;     //Ip地址(网络序) 
    u16    wPort;    //端口号(网络序) 
}TNetAddress;

typedef struct
{
    u32  m_dwSendBitRate;          //发送码率
    u32  m_byVideoFrameRate;       //编码帧率
    u32  m_dwSendFailCount;        //发送失败次数
    u32  m_dwEncFailCount;         //编码失败次数
}TVidSendStatis;

typedef struct
{
    u32	  m_dwVideoFrameRate;   //视频解码帧率
    u32	  m_dwVideoBitRate;     //视频解码码率   (单位是)bit/s*/
    u32   m_dwVideoRecvFrame;   //收到的视频帧数
    u32   m_dwVideoLoseFrame;   //丢失的视频帧数
    u32   m_dwFullLose;         //满丢帧								  
    u32   m_dwDecdWidth;	    //码流的宽
    u32   m_dwDecdHeight;       //码流的高
    u32   m_dwDecFailCount;     //解码失败次数
    BOOL32 m_bRequestKeyFrame;   //是否请求I帧
	u32   m_dwDecPostWidth;     /*解码后的宽*/
    u32   m_dwDecPostHeight;     /*解码后的高*/
}TVidRecvStatis;

#ifdef __cplusplus
extern "C"
{
#endif

/*************************************************************
函数名    : InitCodecWrapper
功能      ：初始化CodecWrapper, 如不运行此函数CodecWrapper所有功能不能用
参数说明  ：无 

返回值说明：成功返回TRUE，失败返回FALSE
**************************************************************/
BOOL32 CODECWRAPPER_API InitCodecWrapper(void);

BOOL32 CODECWRAPPER_API UninitCodecWrapper(void);

#ifdef __cplusplus
}
#endif
  

#ifdef _LINUX_
void RegsterVidCommands();	
void RegsterAudCommands();	
#endif
//
#endif //end of _CODECWRAPPER_COMMON_H_
