/******************************************************************************
模块名	    ：H.261编解码器
相关文件	：所有的源文件
文件实现功能：H.261编解码器头文件，是提供给用户使用的，内容包括调用H.261编解码库
              所用到的各种宏、常量定义以及接口函数的原型声明。
---------------------------------------------------------------------------------------------------------------------
修改记录:
  日  期	  版本		修改人		修改内容
2003.5.30	  1.0        刘建         新建
2003.9.1      2.0        周恒         重建
******************************************************************************/

#ifndef _H261_CORE_LIB_H_
#define _H261_CORE_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif 

#include "algorithmtype.h"
#include "videocommondefines.h"

#define OPTIMIZE_FOR_INTEL      0
#define OPTIMIZE_FOR_EQUATOR    100

//返回值定义 
#define ERR_OK          0
#define ERR_ERROR       -1
#define ERR_NOMEM       -2
#define ERR_BADPARAM    -3

//图象格式定义
#define FF_CIF      1
#define FF_QCIF     2

//H.261标准规定一帧码流不超过256kbit,即32K byte,一个RTP包1k byte,故最多32包	
#define MAX_PACKET_NUM     32   

typedef void * HH261CODEC;
typedef int RESULT ;

//编码器编码参数
typedef struct {
    l32 FrameFormat;
    l32 BitRate;
    l32 IFramePeriod;
    l32 IntraFrame;
    l32 MinQUANT;
    l32 MaxQUANT;
    l32 HasRTPPacketInfo;
	l32 l32FrameRate;
}TH261EncParam;     

//编码器输入/输出数据
typedef struct {
    u8 *   InputBuf;
    u8 *   OutputBuf;
    l32         OutputBitStreamLength;
    BOOL        IsIntraFrame;
    BOOL        IsEncodeOneBlankFrame;
} TH261EncBuffer;   

//解码器输入/输出数据
typedef struct {
    u8 *   InputBuf;
    u8 *   OutputBuf;
    l32    InputBitStreamLength;
    l32    OutputFrameFormat;
    BOOL   IsIntraFrame;
    BOOL   IsDoPostProcess;   //是否做后处理
	l32	   OutputWidth;	      //实际输出图像宽度（输出参数）
	l32	   OutputHeight;	  //实际输出图像高度（输出参数）	
} TH261DecBuffer;

//RTP打包信息
typedef  struct H261RTP_HEADER_s
{
   u32      H261Header;	   //packet h261 header
   l32      PacketStart;   //packet start pos(in byte,begins with 0)
   l32      PacketLength;  //packet length(byte)
}TH261RTPPacketInfo;


//返回H261编解码的版本信息
void  GetH261Version(void *pVer, l32 iBufLen, l32 *pVerLen);

//H261编码器的初始化
l32  H261EncInit(HH261CODEC * pHandle,TH261EncParam * pEncParam);

//H261编码一帧图像
l32  H261EncEncodeOneFrame(HH261CODEC Handle,TH261EncBuffer * pBuffer);

//编码器的关闭
void  H261EncFree(HH261CODEC Handle);

l32  H261EncGetParam(HH261CODEC Handle,TH261EncParam * pEncParam);

l32  H261EncSetParam(HH261CODEC Handle,TH261EncParam * pEncParam);


//解码函数的初始化
l32  H261DecInit(HH261CODEC * pHandle);

//解码一帧图像
l32  H261DecDecodeOneFrame (HH261CODEC Handle,TH261DecBuffer * pBuffer);

//解码器的关闭
void    H261DecFree(HH261CODEC Handle);


#ifdef __cplusplus
};
#endif 

#endif //_H261_CORE_LIB_H_
