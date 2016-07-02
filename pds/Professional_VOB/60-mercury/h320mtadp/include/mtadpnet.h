/*****************************************************************************
模块名      : h320终端适配模块
文件名      : MtAdpNet.h
相关文件    : MtAdpNet.cpp
文件实现功能: 实例模块
作者        : 许世林
版本        : V3.6  Copyright(C) 1997-2003 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2005/06/07  3.6         许世林      创建
******************************************************************************/

#ifndef __MTADPNET_H
#define __MTADPNET_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mtadpcom.h"
#include "kdvmedianet.h"

class CMtAdpInst;
class CMtAdpNet  
{
public:
	CMtAdpNet(CMtAdpInst * const pcIns);
	virtual ~CMtAdpNet();

    //视频接收对象初始化/退出
    BOOL32 VidNetRcvInit();    
    BOOL32 VidNetRcvQuit();

    //音频接收对象初始化/退出
    BOOL32 AudNetRcvInit();
    BOOL32 AudNetRcvQuit();

    //视频发送对象初始化/退出
    BOOL32 VidNetSndInit();
    BOOL32 VidNetSndQuit();

    //音频发送对象初始化/退出
    BOOL32 AudNetSndInit();
    BOOL32 AudNetSndQuit();

    //视频码流发送
    BOOL32 VidNetSnd(u8 *pbyData, u32 dwLen);

    //音频码流发送
    BOOL32 AudNetSnd(u8 *pbyData, u32 dwLen);

    //调试信息打印输出
    void MtAdpLog(u8 byLevel, s8 * pInfo, ...);
  
protected:
    CKdvMediaRcv    * m_pcVidNetRcv;
    CKdvMediaRcv    * m_pcAudNetRcv;
    CKdvMediaSnd    * m_pcVidNetSnd;
    CKdvMediaSnd    * m_pcAudNetSnd;

    CMtAdpInst      * m_pcMtAdpIns;    

    u8               m_byAudSndMediaType;   //音频发送媒体类型
    u8               m_byVidSndMediaType;   //视频发送媒体类型

    u8              * m_byRtpPackBuf;       //rtp打包处理后的视频数据

    SEMHANDLE       m_semVidSnd;            //视频发送信号量
    SEMHANDLE       m_semAudSnd;            //音频发送信号量   
   
};

#endif // __MTADPNET_H
