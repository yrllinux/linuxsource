/* 
 * Brovic Technologies Co., Ltd.  
 *  Copyright (c)2001-2002 
 *  
 *  http://www.brovic.com 
 */ 
#ifndef _DECORE_H_
#define _DECORE_H_

#include "algorithmtype.h"

#ifdef WIN32
#define STDCALL _stdcall
#else
#define STDCALL
#endif

#ifdef __cplusplus
extern "C" {
#endif 
#define _NEW_DEC_INTERFACE_
#define Mp4vDecHandle void *
// mp4v_dec options
#define DEC_OPT_INIT			0x00004000
#define DEC_OPT_PARSEHEAD		0x00008000
#define DEC_OPT_PARSEFRAME		0x00020000		//mp4v_dec a frame
#define DEC_OPT_RELEASE			0x00040000
/*
#define IMAGE_SIZE_720576 				0
#define IMAGE_SIZE_QUARTER_720576 		1
#define IMAGE_SIZE_NINTH_720576 		2
#define IMAGE_SIZE_SIXTEENTH_720576 	3
*/

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
}mp4decDrvError_t;

#define DEC_OK			    MP4DECDRV_OK
#define	DEC_MEMORY		    MP4DECDRV_NOMEM
#define DEC_NOHEAD          MP4DECDRV_NOHEAD
#define DEC_WRONGVERSION    MP4DECDRV_WRONG_VERSION
#define DEC_BAD_FORMAT      MP4DECDRV_BADFMT

#define DEC_MEM_FAIL		MP4DECDRV_MEMFAIL
#define DEC_INIT_FAIL		MP4DECDRV_INITFAIL
#define DEC_EXIT			MP4DECDRV_EXIT
#define DEC_REINIT          DEC_BAD_FORMAT//for change bitstream
#else
#define mp4decDrvError_t    int 
#define DEC_OK				0
#define DEC_MEMORY			1
#define DEC_BAD_FORMAT	    2
#define DEC_WRONGVERSION    3
#define DEC_NOHEAD          4
#define DEC_REINIT			5

#define DEC_MEM_FAIL		-1
#define DEC_INIT_FAIL		-2
#define DEC_EXIT			-3
#endif

typedef struct Mp4vDecFrame
{
	uint8_t * bmp[3]; // the decoded bitmap 
	uint8_t  *bitstream; // the decoder buffer
	uint32_t length; // the length of the decoder stream
	int render_flag;	// 1: the frame is going to be rendered
	int stride; // decoded bitmap stride
	int width;
	int height;
	int bVolReady; // 1 : vol init ready 0 : no vol head find;
	int ispostprocess;
	void * dec;
	int outputtype;
	int outputwidth;
	int outputheight;
	
	uint8_t  *pu8Output88MV;     //用户在解码器外部分配一块396*4*sizeof(u8)的内存指针作为输入
	                        //输出44*36个8x8块的运动级别，块的位置如下图
	                        //运动级别取值范围0-9，值越大运动越剧烈，如输出为0，表示没有运动
	                        //注意：如不需要查看运动级别，此处必须传入NULL !!!!	

} Mp4vDecFrame;
// the prototype of the mp4v_dec() - main mp4v_dec engine entrance
//
#ifdef DECDLL_EXPORTS
__declspec( dllexport )
#else
#ifdef	DECDLL_IMPORTS
__declspec( dllimport )
#endif
#endif
#ifndef _NEW_DEC_INTERFACE_
int STDCALL mp4v_dec(
			unsigned long dec_opt, // dec_opt - the option for docoding, see below
			Mp4vDecFrame *param);	// param	- the parameter  (it's actually meaning dec_frame);
#else
mp4decDrvError_t Mp4vDecoderInit(Mp4vDecHandle *pHandle,Mp4vDecFrame *dec_frame);
mp4decDrvError_t Mp4vDecoderFrame(Mp4vDecHandle Handle,Mp4vDecFrame *dec_frame);
mp4decDrvError_t Mp4vDecoderFree(Mp4vDecHandle Handle);
void  GetMPEG4DecoderVersion(void *pVer, int iBufLen, int *pVerLen);
#endif
#ifdef __cplusplus
extern "C" {
#endif 
#endif // _DECORE_H_
