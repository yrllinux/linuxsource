/*****************************************************************************
模   块   名: audio_common
文   件   名: audio_common.h
相 关  文 件: 
文件实现功能: 版本信息及内存信息结构体定义
版        本: V100  Copyright(C) 2005-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
修 改 记 录:
日       期      版本        修改人      修改内容
2009/03/26       v100        陆志洋      创    建
2013/04/10       v101        陆志洋      增加内存信息结构体
******************************************************************************/
#ifndef _AUDIO_COMMON_H_
#define _AUDIO_COMMON_H_
#ifdef __cplusplus
extern "C" {
#endif   /* __cplusplus */

//版本信息输入结构
typedef struct
{
    void* pvVer;         //版本信息缓存     
    l32 l32BufLen;       //版本信息缓存空间大小
    u32 u32CodecType;    //编解码器或aec类型
    u32 u32Reserve;      
}TAudVerInfoInput;

//版本信息输出结构
typedef struct
{               
    l32 l32VerLen;      //输出版本信息所占字节数
    u32 u32Reserve;    
}TAudVerInfoOutput;

//内存信息输入结构体
typedef struct
{
    void* pvDMAHandle;              //DMA通道句柄
    void* pvFastMem;                //片内快速内存句柄
    void* pvSlowMem;                //片外慢速内存句柄
    void* pvSlowScratchMem;         //片外Scratch内存句柄
    l32 l32FastMemSize;             //片内快速内存总大小
    l32 l32SlowMemSize;             //片外慢速内存总大小
} TAudInputMemParam;

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif 



