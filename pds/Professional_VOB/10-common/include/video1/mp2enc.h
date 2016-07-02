#ifndef _MP2ENC_H_
#define _MP2ENC_H_

#ifdef __cplusplus
extern "C" {
#endif 
#include "algorithmType.h"
//编码器返回值	
#define MP2ENC_OK 0
#define MP2ENC_ERROR -1	
// chroma_format 
#define CHROMA420 1
#define CHROMA422 2
#define CHROMA444 3

	typedef void * Mp2EncHandle;//编码句柄
	
	//编码器编码参数
	typedef struct TagMp2EncParam
	{
		l32 s32Height;              //图像高度
		l32 s32Width;               // 图像宽度
		l32 s32BitRate;             //编码码率
		l32 s32IFrameInterval;      //I帧间隔
		l32 s32ChromaFormat;        //图像格式CHROMA420/CHROMA422 
		l32 s32MaxQuant;            //最大量化系数1～31
		l32 s32MinQuant;            //最小量化系数1～31
	}TMp2EncParam; 
	
	//编码器输入/输出数据
	typedef struct TagMp2EncFrame
	{
		u8 *   pu8InputBuf;				//输入图像存放地址
		u8 *   pu8OutputBuf;				//编码输出码流存放地址
		l32     s32OutputBitStreamLength;//输出码流长度(byte)
		l32     s32ForceIFrame;			//是否强制I 帧编码; 1:是; 0:否
		l32     s32FrameType;			//输出编码类型 1:I帧; 0:P帧
		l32		s32Width;
		l32		s32Height;
	} TMp2EncFrame;   
	
	//编码初始化
	l32 Mp2EncoderInit(Mp2EncHandle *pHMp2Enc, TMp2EncParam *pMp2EncParam);
    //设置编码参数
	l32 Mp2EncSetParam(Mp2EncHandle hMp2Enc, TMp2EncParam *pMp2EncParam);	
	//编码一帧图像
	l32 Mp2EncodeOneFrame(Mp2EncHandle hMp2Enc, TMp2EncFrame *pMp2EncFrame);
	//释放编码器
	l32 Mp2EncFree(Mp2EncHandle hMp2Enc);
	
	void  GetMpeg2Version(void *pVer, l32 s32BufLen, l32 *ps32VerLen);
	void Mp2GetReconFile(Mp2EncHandle hMp2Enc,s8 *ps8FileName);	
#ifdef __cplusplus
}
#endif 

#endif
