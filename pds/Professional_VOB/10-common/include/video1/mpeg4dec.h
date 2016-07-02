/* 
 * Brovic Technologies Co., Ltd.  
 *  Copyright (c)2001-2002 
 *  
 *  http://www.brovic.com 
 */ 
#ifndef _MPEG4DEC_H_
#define _MPEG4DEC_H_

#include "algorithmtype.h"

#ifdef __cplusplus
extern "C" {
#endif 

// mp4v_dec return values
#ifdef _LINUX_
typedef enum
{
	MP4DECDRV_OK,
	MP4DECDRV_ERROR,
	MP4DECDRV_NOMEM,
	MP4DECDRV_NOHEAD,
	MP4DECDRV_BADFMT,
	MP4DECDRV_MEMFAIL,
	MP4DECDRV_INITFAIL,
	MP4DECDRV_EXIT,
	MP4DECDRV_WRONG_VERSION,
	MP4DECDRV_KERNEL_MODULE_PROBLEMS,	
}TMp4DecDrvError_t;

#define DEC_OK			    MP4DECDRV_OK
#define	DEC_MEMORY		    MP4DECDRV_NOMEM
#define DEC_NOHEAD          MP4DECDRV_NOHEAD
#define DEC_WRONGVERSION    MP4DECDRV_WRONG_VERSION
#define DEC_BAD_FORMAT      MP4DECDRV_BADFMT

#define DEC_MEM_FAIL		MP4DECDRV_MEMFAIL
#define DEC_INIT_FAIL		MP4DECDRV_INITFAIL
#define DEC_EXIT			MP4DECDRV_EXIT
#else

//错误码
enum
{
    DEC_OK,                               //success
    ERROR_INIT_MEM = 0x1000,              //init mem fail
    ERROR_MALLOC_MEM,                     //malloc mem fail
    ERROR_FREE_MEM,                       //free mem fail
    ERROR_ALLOC_DS,                       //alloc ds fail
    ERROR_INVALID_PARAMETER,              //invlid parameter
    ERROR_DEBLOCK,                        //deblock error
    ERROR_DERING,                         //dering error
    ERROR_UNSUPPORT_VOTYPEID = 0x8000,    //video_object_type_indication
    ERROR_UNSUPPORT_SHAPE,                //video_object_layer_shape
    ERROR_UNSUPPORT_OBMC,                 //obmc
    ERROR_UNSUPPORT_SPRITE,               //sprite
    ERROR_UNSUPPORT_COMPLXES,             //complexity_estimation
    ERROR_UNSUPPORT_RES,                  //error_res
    ERROR_UNSUPPORT_DP,                   //data partitioning
    ERROR_UNSUPPORT_RRVOP,                //reduced_resolution_vop
    ERROR_UNSUPPORT_SCALABLE,             //scalability
    ERROR_UNSUPPORT_VSOTYPE,              //visual_object_type
    ERROR_UNSUPPORT_BFRAME,               //unsupport B-frame
    ERROR_UNSUPPORT_SIZE,                 //unsupport size
    ERROR_UNSUPPORT_VOPTYPE,              //unsupport vop type
    ERROR_USERDATA,                       //dec user data error
    ERROR_TIMEBASE,                       //dec time base error
    ERROR_BITSTREAM,                      //error dec bs
    ERROR_DEC_COEFF,                      //dec coeff error
    ERROR_DEC_MB,                         //dec mb error
    ERROR_VLXDEC_VOPHEAD = 0x9000,        //dec vop header error
    ERROR_VLXDEC_MB,                      //vlx dec mb error
    ERROR_VLXDEC_WAIT                     //vlx wait time out
};

#endif

#define GET_DEC_POINT_REF   1
#define GET_DEC_POINT_REC   2
#define GET_DEC_WIDTH       3
#define GET_DEC_HEIGHT      4
#define GET_DEC_EDGE_WIDTH  5

#define TMp4vDecHandle void *

typedef struct TMp4vDecFrame
{
	u8 *pu8YUV;          // 解码输出YUV420图像Buffer
	u8 *pu8BitStream;    // 解码输入码流Buffer
	u32 length;          // 解码码流长度
	l32 l32FrameType;    // 输出解码帧类型 0:I帧; 1:P帧
	l32 l32OutWidth;     // 解码输出图像宽度
	l32 l32OutHeight;    // 解码输出图像高度
	l32 l32PostProcess;  // 是否做后处理 1：是 0：否
	
	u8 *pu8Output88MV; //用户在解码器外部分配一块396*4*sizeof(u8)的内存指针作为输入
	                   //输出44*36个8x8块的运动级别，块的位置如下图
	                   //运动级别取值范围0-9，值越大运动越剧烈，如输出为0，表示没有运动
	                   //注意：如不需要查看运动级别，此处必须传入NULL !!!!	

} TMp4vDecFrame;

//外部接口函数
l32 Mp4vDecoderInit(TMp4vDecHandle *ppvHandle);
l32 Mp4vDecoderFrame(TMp4vDecHandle pvHandle,TMp4vDecFrame *ptDecFrame);
l32 Mp4vDecoderFree(TMp4vDecHandle pvHandle);
l32 MP4vGetDecInfo(void *pvHandle, l32 l32CmdId, u32 *pu32PtrAddr);
void  GetMPEG4DecoderVersion(void *pVer, l32 l32BufLen, l32 *pl32VerLen);

#ifdef __cplusplus
extern "C" {
#endif 
#endif // _DECORE_H_
