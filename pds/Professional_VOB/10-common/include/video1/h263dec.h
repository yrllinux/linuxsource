#ifndef _H263DEC_H_
#define _H263DEC_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "algorithmtype.h"
#define DECODE_EXCLUDE_VGA 0x0001
#define DECODE_INCLUDE_VGA 0x0000
#define SF_SQCIF                        1  // 001 
#define SF_QCIF                         2  // 010 
#define SF_CIF                          3  // 011 
#define SF_4CIF                         4  // 100 
#define SF_16CIF                        5  // 101 
#define SF_CUSTOM                       6  // 110 
#ifndef RTPBUFFERSIZE
#define RTPBUFFERSIZE 3076
#endif
#ifdef _LINUX_
	typedef enum
	{
		H263DECDRV_OK,
			H263DECDRV_ERROR,
			H263DECDRV_NOMEM,
			H263DECDRV_BADFMT,
			H263DECDRV_WRONG_VERSION,	
			H263DECDRV_KERNEL_MODULE_PROBLEMS,	
	}H263DecDrvError_t;
	
#define H263DECERROR		H263DECDRV_ERROR
#define H263DECOK		    H263DECDRV_OK
#else
#define H263DecDrvError_t    l32 
#define H263DECOK  0
#define H263DECERROR  -1
#endif
	typedef void *H263DecHandle;
	typedef struct 
	{
		u8	*pu8BitStream;		//待解码的H263一帧图像的码流缓冲区
		u32	u32YUVLen;          //解码后一帧YUV图像的长度，在把解码后的YUV图像输出时用到
		u8	*pu8YUV420;			//解码YUV数据缓冲区，用户在调用解码器前自行分配
		u32	u32BitstreamLen;	//待解码的H263一帧图像的码流长度
		s16  s16PictureType;    //解码帧的图像类型(I帧或者P帧)
		l32  s32Height;         //当前正在解码的图像的高度
		l32  s32Width;          //当前正在解码的图像的宽度
		l32 s32DecodeType;       // 输入 DECODE_EXCLUDE_VGA: 非vga解码  0:vga 解码
		l32 s32PostProcess;      // 1: 后处理 0: 不
	} TH263DecFramePara;
	
	typedef struct 
	{
		s16 s16PictureType; //解码帧的图像类型(I帧或者P帧)
		l32 s32Width;       //当前正在解码的图像的宽度
		l32 s32Height;      //当前正在解码的图像的宽度
		
	}TH263DecFrameHeader;
	
    //H263解码器初始化
    H263DecDrvError_t H263DecoderInit(void **ppvH263DecHandle,TH263DecFramePara *ptH263DecFramePara);
	
    //H263解码一帧图像
    H263DecDrvError_t H263DecodeOneFrame(void *pvH263DecHandle,TH263DecFramePara *ptH263DecFramePara);
		
    //释放H263解码器资源
    H263DecDrvError_t H263DecoderClose(void *pvH263DecHandle);
#ifdef __cplusplus
}
#endif 

#endif
