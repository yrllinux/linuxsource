#ifndef _MP2DEC_H_
#define _MP2DEC_H_

#ifdef __cplusplus
extern "C" {
#endif 
#include "algorithmType.h"


#define MP2DEC_OK      0
#define MP2DEC_ERROR   -1

	typedef void *Mp2DecHandle;			//解码器句柄

	typedef struct TagMp2DecFrame
	{	
		u8 *pu8InputBitstream;		//输入码流的地址
		u8 *pu8OutputBuffer;			//输出码流存放的地址
		l32 s32InputBitstreamLength;	//输入解码器码流长度
		l32 s32ChromaFormat;			//输出图像的格式  1：420格式 2：422格式
		l32 s32Height;				//解码输出图像的高度 
		l32 s32Width;				//解码输出图像的宽度
		l32 s32FrameType;			//输出图像格式1:I帧; 0:P帧
		l32 s32PostProcess;			//输入：是否后处理 1：是 0：否
	}TMp2DecFrame;


	l32 Mp2DecoderInit(Mp2DecHandle *pHMp2Dec);//解码器初始化

	l32 Mp2DecodeOneFrame(Mp2DecHandle hMp2Dec,TMp2DecFrame *pMp2DecFrame);//解码一帧

	l32 Mp2DecFree(Mp2DecHandle hMp2Dec);  //释放解码器

#ifdef __cplusplus
}
#endif 

#endif
