/******************************************************************************
模块名        ：jpegdec_v100
文件名        ：jpegdec_v100
相关文件    ：
文件实现功能：接口函数声明
作者        ：金立田
版本        ：1.0
-------------------------------------------------------------------------------
修改记录:
日  期            版本        修改人        走读人    修改内容
2008/01/28        1.0            金立田                  创建
******************************************************************************/
#ifndef _JPEG_DECODER_PUBLIC_H_
#define _JPEG_DECODER_PUBLIC_H_

#include "algorithmtype.h"
#include "vid_videounit.h"
#include "funclist.h"

#ifdef __cplusplus
extern "C"
{
#endif 

#ifndef CHIP_DM64XPLUS
#define RESTRICT 
#else
#define RESTRICT restrict 
#endif		

/* version information: 1.00(JPEG_MAJOR.JPEG_MINOR) */
#define JPEGDEC_MAJOR  ((u32) 1)
#define JPEGDEC_MINOR  ((u32) 00)
#define JPEGDEC_VERSION ((u32)JPEGDEC_MAJOR << 16 |(u32)JPEGDEC_MINOR)

//错误码
enum
{
    ERR_VID_JPEGDEC_START_VALUE = (ERR_START_VALUE | ERR_VID_MOD| ERR_VID_JPEGENC_MOD),    //JPEG解码器错误起始值
    ERR_VID_JPEGDEC_OPEN_INPUT_POINT,      //OPEN输入指针错误
    ERR_VID_JPEGDEC_OPEN_MEM_HANDLE,       //输入内存句柄错误
    ERR_VID_JPEGDEC_MALLOC_DEC_HANDLE,     //解码器句柄分配错误
    ERR_VID_JPEGDEC_MALLOC_MB_BUF,         //解码器分配宏块内存错误
    ERR_VID_JPEGDEC_PROC_INPUT_POINT,      //PROCESS输入指针错误
    ERR_VID_JPEGDEC_SETPARAM_INPUT_POINT,  //SETPARAM输入指针错误
    ERR_VID_JPEGDEC_WIDTH_HEIGHT,          //解码宽高错误
    ERR_VID_JPEGDEC_GET_VERSION,           //获取版本信息错误
    ERR_VID_JPEGDEC_GETSTATUS_INPUT_POINT, //GETSTATUS输入指针错误
    ERR_VID_JPEGDEC_CLOSE_INPUT_POINT,     //CLOSE输入指针错误
    ERR_VID_JPEGDEC_MAX_WIDTH_HEIGHT,      //解码图像大于最大宽高
    ERR_VID_JPEGDEC_UNEXPECTED_BYTE,       //没有找到MARK标志
    ERR_VID_JPEGDEC_HEADER_LASTBLOCK,      //文件头中就包含结束标志
    ERR_VID_JPEGDEC_HEADER_SOF0,           //解码SOF0错误
    ERR_VID_JPEGDEC_HEADER_DRI,            //解码DRI错误
    ERR_VID_JPEGDEC_DQT_ID,            //解码DQT错误
    ERR_VID_JPEGDEC_DHT_TYPE,              //解码DHT错误
    ERR_VID_JPEGDEC_DHT_ID,                //解码DHT时表ID错误
    ERR_VID_JPEGDEC_DHT_LESS_BYTE,         //解码DHT时表时数据不足
    ERR_VID_JPEGDEC_HEADER_SOS,            //解码SOS错误
    ERR_VID_JPEGDEC_HEADER_LESS_BYTE,      //解码时数据不足
    ERR_VID_JPEGDEC_SOF0_HEIGHT,           //解码SOF0时高度为0
    ERR_VID_JPEGDEC_SOF0_COMNUM,           //解码SOF0时分量数错误
    ERR_VID_JPEGDEC_SOF0_COMID,            //解码SOF0时分量ID错误 
    ERR_VID_JPEGDEC_SOF0_FACTOR,           //解码SOF0时分量因子错误
    ERR_VID_JPEGDEC_SOF0_QTID,             //解码SOF0时量化表ID错误
    ERR_VID_JPEGDEC_SCAN_LESS_BYTE         //解码SCANLINE时数据不足 
};

l32 JpegDecoderOpen(VideoCodecHandle *ppvDecoder, TDecParam* ptDecParam, void* pvInputMemHandle);
l32 JpegDecoderProcess(VideoCodecHandle pvHandle, TDecInput *ptDecInput, TDecOutput *ptDecOutput);
l32 JpegDecoderClose(VideoCodecHandle pvHandle);
l32 JpegDecoderSetParams(VideoCodecHandle pvHandle, TDecParam* ptDecParam);
l32 JpegDecoderGetStatus(VideoCodecHandle pvHandle, TDecStatusParam* ptEncStatusParam);
l32 JpegDecoderGetVersion(void *pvVer, l32 l32BufLen, l32 *pl32VerLen);
l32 JpegDecoderDebug(VideoCodecHandle pvHandle, void *pvDecDebug);
l32 JpegDecoderAnalyzeErrorCode(l32 l32ErrorCode, l32 l32BufLen, void* pvErrorCodeInfo);

extern TDecFunction tJpegDec;

#ifdef __cplusplus
};
#endif

#endif
