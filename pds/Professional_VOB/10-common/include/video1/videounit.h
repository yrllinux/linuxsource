/******************************************************************************
模块名	    ： VIDEOUNIT                
文件名	    ： videounit.h
相关文件	： videounit.c等
文件实现功能： 函数参数声明等
---------------------------------------------------------------------------------------
修改记录:
日  期		版本		修改人		走读人    修改内容
12/27/2006  1.0          赵波                  创建
**************************************************************************************/
#ifndef _VIDEOUNIT_H_
#define _VIDEOUNIT_H_

#include "algorithmtype.h"
#include "videodefines.h"
#include "imageunit.h"

#define VideoCodecHandle  void*

typedef struct
{
    void *pvDMAHandle;         //DMA句柄
    void *pvInternalMem;       //片内SRAM的起始地址
    l32  l32InternalMemSize;   //片内SRAM的大小
    void *pvExternalMem;       //片外DDR的起始地址
    l32  l32ExternalMemSize;   //片外DDR的大小
} TMemHandle;  //记录空间大小

typedef struct tEncParam
{
	u32		u32EncType;				//编码器类型
	l32		l32EncFrameRate;		//帧率
	l32		l32EncBitRate;			//码率（bps）
	l32		l32EncMinQP;			//最小量化参数
	l32		l32EncMaxQP;			//最大量化参数
	l32		l32EncIFrameInterval;	//I帧间隔（两个I帧间P、B帧数）
	l32		l32EncPacketMode;		//打包模式
	l32		l32EncWidth;			//编码宽度
	l32		l32EncHeight;			//编码高度（宽度=0&高度=0表示自适应，目前仅适用于mpeg4）
	l32		l32EncFrameDropRate;	//允许连续丢帧率（0表示不丢帧，速度优先；非0表示有丢帧，质量优先，必须为10的倍数，以10%为单位）
	u32		u32EncCycles;			//编码一帧可用周期数，目前用于传输编码map的主频(以兆为单位)
	l32		l32EncYUVType;			//编码YUV类型（420、422、444），目前仅支持对420图像编码，此参数没有用到
    l32		l32EncLimitation;		//编码器限制（）
    TMemHandle tMemHandle;          //编码器需要用到的内存空间结构体
	u32		u32EncReserved;			//保留参数
}TEncParam;	//编码初始化参数


typedef struct tEncoderInput
{
	TLogo		*aptLogo[MAX_LOGOS_IN_IMAGE+1];			//图标（以NULL为结束标志）
	TBanner		*aptBanner[MAX_BANNERS_IN_IMAGE+1];		//横幅（以NULL为结束标志）

	TImageInfo	*ptInEncImage;		//输入编码器的图象信息
	u32		u32EncKeyFrame;			//编码帧类型（输入） 1: 强制编I帧  0：编码器自定
	u32	   	u32EncOutputPSNR;		//是否计算PSNR值	 1: 计算  0：不计算
	u8		*pu8EncMotionInfo;		//运动信息（不使用必须置为NULL，44x36块等级0~9）
	u32 	u32EncMotionInfoLen;	//运动信息长度
	l32		l32EncSkipPreprocess;	//前处理开关, 由外部控制是否做前处理
	u32		u32EncReserved;			//保留参数	
}TEncoderInput;		//编码输入参数


typedef struct tEncoderOutput
{
	u8		*pu8EncBitstream;		//输出码流缓冲
	l32		l32EncBitstreamLen;		//输出码流长度
	l32		l32EncIsKeyFrame;		//编码帧类型（输出） 1: 编码输出I帧  0：编码输出P帧
	u32		l32EncWidth;			//实际编码宽度
	u32		l32EncHeight;			//实际编码高度
	l32		l32EncMotionInfoValid;	//运动信息是否有效
	u32		u32EncReserved;			//保留参数	
    //以下为测试接口
    u8      *pu8Pp2encImage;        //前处理压缩至编码器的图像

}TEncoderOutput;		//编码输出参数


typedef struct tDecParam
{
	u32		u32DecType;				 //解码器类型
	l32		l32DecPacketMode;		 //打包模式
	l32		l32DecYUVType;			 //解码输出YUV类型（420、422）
	u32		u32DecCycles;			 //解码一帧可用周期数，目前用于传输解码DSP的主频(以兆为单位)
	l32		l32DecWidth;			 //解码图象宽度(查询状态使用)
	l32		l32DecHeight;			 //解码图象高度(查询状态使用)
    l32     l32DecMaxWidth;          //解码图像的最大宽度
    l32     l32DecMaxHeight;         //解码图像的最大高度
    TMemHandle tMemHandle;           //编码器需要用到的内存空间结构体
	u32		u32DecReserved;			 //保留参数
}TDecParam;		//解码初始化参数


typedef struct tDecoderInput
{
	u8			*pu8DecBitstream;	//输入码流缓冲
	u32			u32DecBitstreamLen;	//输入码流长度
	TLogo		*aptLogo[MAX_LOGOS_IN_IMAGE+1];			//图标（以NULL为结束标志）
	TBanner		*aptBanner[MAX_BANNERS_IN_IMAGE+1];		//横幅（以NULL为结束标志）
	u8			*pu8DecMotionInfo;	//运动信息（不使用必须置为NULL）
	u32			u32DecMotionInfoLen;//运动信息长度
	u32         u32DecPostProcess;	//是否作后处理 1：是 0：否
	u32			u32DecReserved;		//保留参数	
}TDecoderInput;		//解码输入参数


typedef struct tDecoderOutput
{
	TImageInfo	*ptDecOutImage;		//解码器输出图象的信息
    l32		l32DecWidth;			//解码图象宽度
	l32		l32DecHeight;			//解码图象高度
	l32		l32DecIsKeyFrame;		//解码帧类型
	u32		u32DecConsumedCycles;	//解码消耗周期数
	l32		l32DecMotionInfoValid;	//运动信息是否有效
	u32		u32DecReserved;			//保留参数		
    //以下为测试接口
    u8      *pu8Decoded;	        //解码输出的原始图像
}TDecoderOutput;			//解码输出参数


/*====================================================================
函数名		：	VideoUnitEncoderOpen
功能		：	初始化编码器
算法实现	：	无
引用全局变量：	无
输入参数说明：	ptHandle:	编码器句柄
              	ptEncParam:		编码初始化参数              
输出参数说明：	无
返回值说明  ：	VIDEOSUCCESS：成功，VIDEOFAILURE：失败
举例或特别说明：目前前处理背景仅支持420格式；
                目前输入所有编码器内核的图像统一为420格式；
====================================================================*/
l32 VideoUnitEncoderOpen(VideoCodecHandle *ptHandle, TEncParam *ptEncParam);


/*====================================================================
函数名		：	VideoUnitEncoderProcess
功能		：	编码一帧图象
算法实现	：	width and height is multiple of 8 are supported
引用全局变量：	无
输入参数说明：	ptHandle:		编码器句柄
             	ptEncoderInput:	编码输入参数
             	ptOutputHandle:	编码输出
输出参数说明：	无
返回值说明  ：	VIDEOSUCCESS：成功，VIDEOFAILURE：失败，VIDEOSKIP：跳过
====================================================================*/
l32 VideoUnitEncoderProcess(VideoCodecHandle ptHandle, TEncoderInput *ptEncoderInput, TEncoderOutput *ptEncoderOutput);


/*====================================================================
函数名		：	VideoUnitEncoderClose
功能		：	关闭编码器
算法实现	：	无
引用全局变量：	无
输入参数说明：	ptHandle:	编码器句柄
输出参数说明：	无
返回值说明  ：	VIDEOSUCCESS：成功，VIDEOFAILURE：失败
====================================================================*/
l32 VideoUnitEncoderClose(VideoCodecHandle ptHandle);


/*====================================================================
函数名		：	VideoUnitEncoderSetParam
功能		：	设置编码器参数
算法实现	：	无
引用全局变量：	无
输入参数说明：	ptHandle:	编码器句柄
				ptEncParam:		编码设置参数
输出参数说明：	无
返回值说明  ：	VIDEOSUCCESS：成功，VIDEOFAILURE：失败
举例或特别说明：目前调用该函数可改变的参数不包括编码宽度和高度，如需改变
                编码图像的宽高，须先关闭原编码器，再按新的宽高初始化新的编码器
====================================================================*/
l32 VideoUnitEncoderSetParam(VideoCodecHandle ptHandle, TEncParam *ptEncParam);


/*====================================================================
函数名		：	VideoUnitEncoderGetStatus
功能		：	获取编码器运行状态
算法实现	：	无
引用全局变量：	无
输入参数说明：	ptHandle:	编码器句柄
				ptEncParam:		编码器参数
输出参数说明：	无
返回值说明  ：	VIDEOSUCCESS：成功，VIDEOFAILURE：失败
====================================================================*/
l32 VideoUnitEncoderGetStatus(VideoCodecHandle ptHandle, TEncParam *ptEncParam);


/*====================================================================
函数名		：	VideoUnitEncoderDumpStatus
功能		：	打印编码器信息
算法实现	：	无
引用全局变量：	无
输入参数说明：	ptHandle:	编码器句柄
输出参数说明：	无
返回值说明  ：	VIDEOSUCCESS：成功，VIDEOFAILURE：失败
====================================================================*/
l32 VideoUnitEncoderDumpStatus(VideoCodecHandle ptHandle);

/*====================================================================
函数名		: 	VideoUnitEncoderGetVersion
功能		：	编码器接口版本信息
算法实现	：	无
引用全局变量：	pvVer:			输出缓存
				l32BufLen:		缓存长度
				pl32VerLen:		信息长度
				u32EncoderType:	编码器类型
输入参数说明：	无
输出参数说明：	无
返回值说明  ：	VIDEOSUCCESS：成功，VIDEOFAILURE：失败　              
====================================================================*/
l32 VideoUnitEncoderGetVersion(void *pvVer, l32 l32BufLen, l32 *pl32VerLen, u32 u32EncoderType);


/*====================================================================
函数名		：	VideoUnitDecoderOpen
功能		：	初始化解码器
算法实现	：	无
引用全局变量：	无
输入参数说明：	ptHandle   :  解码器句柄
              	ptDecParam :  解码初始化参数              
输出参数说明：	无
返回值说明  ：	VIDEOSUCCESS：成功，VIDEOFAILURE：失败
举例或特别说明：目前前处理背景仅支持420格式；
                目前输入所有编码器内核的图像统一为420格式；
====================================================================*/
l32 VideoUnitDecoderOpen(VideoCodecHandle *ptHandle, TDecParam *ptDecParam);


/*====================================================================
函数名		：	VideoUnitDecoderProcess
功能		：	解码一帧图象
算法实现	：	width and height is multiple of 8 are supported
引用全局变量：	无
输入参数说明：	ptHandle       :  解码器存放地址
             	ptDecoderInput :  解码输入参数
             	ptOutputHandle :  解码输出
输出参数说明：	无
返回值说明  ：	VIDEOSUCCESS：成功，VIDEOFAILURE：失败，VIDEOSKIP：跳过
====================================================================*/
l32 VideoUnitDecoderProcess(VideoCodecHandle ptHandle, TDecoderInput *ptDecoderInput, TDecoderOutput *ptDecoderOutput);


/*====================================================================
函数名		：	VideoUnitDecoderClose
功能		：	关闭解码器
算法实现	：	无
引用全局变量：	无
输入参数说明：	ptHandle:	解码器句柄
输出参数说明：	无
返回值说明  ：	VIDEOSUCCESS：成功，VIDEOFAILURE：失败
====================================================================*/
l32 VideoUnitDecoderClose(VideoCodecHandle ptHandle);


/*====================================================================
函数名		：	VideoUnitDecoderSetParam
功能		：	设置解码器参数
算法实现	：	无
引用全局变量：	无
输入参数说明：	ptHandle   :  解码器句柄
				ptDecParam :  解码设置参数
输出参数说明：	无
返回值说明  ：	VIDEOSUCCESS：成功，VIDEOFAILURE：失败
举例或特别说明：目前调用该函数可改变的参数不包括解码宽度和高度，如需改变
                解码图像的宽高，须先关闭原解码器，再按新的宽高初始化新的解码器
====================================================================*/
l32 VideoUnitDecoderSetParam(VideoCodecHandle ptHandle, TDecParam *ptDecParam);


/*====================================================================
函数名		：	VideoUnitDecoderGetStatus
功能		：	获取解码器运行状态
算法实现	：	无
引用全局变量：	无
输入参数说明：	ptHandle:	解码器句柄
				ptDecParam:		解码器参数
输出参数说明：	无
返回值说明  ：	VIDEOSUCCESS：成功，VIDEOFAILURE：失败
====================================================================*/
l32 VideoUnitDecoderGetStatus(VideoCodecHandle ptHandle, TDecParam *ptDecParam);


/*====================================================================
函数名		：	VideoUnitDecoderDumpStatus
功能		：	打印解码器信息
算法实现	：	无
引用全局变量：	无
输入参数说明：	ptHandle:	解码器句柄
输出参数说明：	无
返回值说明  ：	VIDEOSUCCESS：成功，VIDEOFAILURE：失败
====================================================================*/
l32 VideoUnitDecoderDumpStatus(VideoCodecHandle ptHandle);


/*====================================================================
函数名		: 	VideoUnitDecoderGetVersion
功能		：	解码器接口版本信息
算法实现	：	无
引用全局变量：	pvVer:			输出缓存
				l32BufLen:		缓存长度
				pl32VerLen:		信息长度
				u32DecoderType:	解码器类型
输入参数说明：	无
输出参数说明：	无
返回值说明  ：	VIDEOSUCCESS：成功，VIDEOFAILURE：失败              
====================================================================*/
l32 VideoUnitDecoderGetVersion(void *pvVer, l32 l32BufLen, l32 *pl32VerLen, u32 u32DecoderType);

#endif	//_VIDEOUNIT_H_
