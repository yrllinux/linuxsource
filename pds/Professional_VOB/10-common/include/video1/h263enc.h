#ifndef _H263ENC_H_
#define _H263ENC_H_

#define H263_ENCODE_ONLY_420 0x0001
#define H263_ENCODE_PACKET_BY_263plus 0x0080

#ifdef __cplusplus
extern "C" {
#endif 
#include "algorithmtype.h"
//Hu 20050627
#define SF_SQCIF                        1  // 001 
#define SF_QCIF                         2  // 010 
#define SF_CIF                          3  // 011 
#define SF_4CIF                         4  // 100 
#define SF_16CIF                        5  // 101 
#define _1024x768                       9
#define _800x600                        8
#define _640x480                        7
#define SF_CUSTOM                       6  // 110 
#ifndef RTPBUFFERSIZE
#define RTPBUFFERSIZE					3076
#endif	
#ifdef _LINUX_
typedef enum
{
	H263ENCDRV_OK,
		H263ENCDRV_ERROR,
		H263ENCDRV_NOMEM,
		H263ENCDRV_BADFMT,
		H263ENCDRV_WRONG_VERSION,	
		H263ENCDRV_KERNEL_MODULE_PROBLEMS,	
}H263EncDrvError_t;

#define H263ENC_ERROR		H263ENCDRV_ERROR
#define H263ENC_OK		    H263ENCDRV_OK
#else
#define H263EncDrvError_t    l32 
#define H263ENC_OK  0
#define H263ENC_ERROR  -1
#endif
typedef void *H263EncHandle;
typedef struct tagH263EncoderPara
{
	l32 s32FrameRate;				   //帧率
	l32 s32BitsRate;                   //码率
	l32 s32Width;		   			   // 输入图像宽度[0,720]必须为16的倍数
	l32 s32Height;		    		   // 输入图像高度[0,576]必须为16的倍数
	l32 s32MaxQuantizer;     		   //最大量化步长
	l32 s32MinQuantizer;      		   //最小量化步长
	l32 s32IFramePeriod;               //I-帧编码周期，即编码帧出现I－帧的周期
	l32 s32SourceFormat;               // 3 : cif;  7 : _640x480 ;8:_800x600 ;9: _1024x768
	l32 s32VgaEncWay;                  //0 : 按原格式编码 1 : 转换为cif格式编码
	l32  s32RtpWay;                    //rtp打包方式 1为h263+ 方式 0为H263
    l32 l32RTPPacketLen;               //RTP打包的包长，0：为默认值，大于0：指定包长
}TH263EncoderPara;

typedef struct tagH263EncFrame
{
    u8  *pu8InputBuffer;			   //输入编码图像指针
    u8  *pu8OutputBuffer;			   //编码输出码流buffer
    l32  s32OutputLength;			   //输出码流长度
    l32  s32FrameType;				   //输出编码帧类型 I/P
	l32  s32ForceIFrame;               //强制I帧
	l32  s32EncodeOneBlankFrame;	   //编空帧
	l32	 s32Width;					   //输出：编码图像的宽高 
	l32	 s32Height;
}TH263EncFrame; 

H263EncDrvError_t H263EncoderInit(H263EncHandle *phH263Encoder, TH263EncoderPara *ptH263EncPara);  //编码器参数初始化，进行内存分配
H263EncDrvError_t H263EncoderClose(H263EncHandle hH263Encoder);
H263EncDrvError_t H263EncodeOneFrame(H263EncHandle hH263Encoder, TH263EncFrame *ptH263EncFrame);
H263EncDrvError_t H263EncSetParam(H263EncHandle hH263Encoder, TH263EncoderPara *ptH263EncPara);
void H263GetReconFile(H263EncHandle hH263Enc,s8 *ps8ReconFile);
void  GetH263Version(void *pvVer, l32 s32BufLen, l32 *ps32VerLen);
#ifdef __cplusplus
}
#endif 

#endif
