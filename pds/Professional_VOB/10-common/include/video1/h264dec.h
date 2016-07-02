/***************************************************************************************
文件名	    ：H264Dec.h
相关文件	：H264Dec.c
文件实现功能：由于上层应用层只能为编码一帧提供　MAX_FRAME_BUFFER_LEN　(128k)的缓存，所以
			  如果编码器发现该帧编码的码流超过该长度，编码失败，该危险系数较大，对于一个
			  352x288的图像，编码为Ｉ帧可以达到81k !!! 低位低字节
作者		：古继兴
版本		：2004.8.28
-------------------------------------------------------------------------------
日  期		版本		修改人		走读人    修改内容
01/18/2005	1.0         张智强      古继兴    代码规范化
04/22/2005	1.0         苏强                  代码规范化
***************************************************************************************/
#ifndef H264DEC_H
#define H264DEC_H

#include "algorithmtype.h"

#ifdef _LINUX_
#define H264vDecHandle void *
typedef enum
{
	H264DECDRV_OK,
	H264DECDRV_ERROR,
	H264DECDRV_NOMEM,
	H264DECDRV_NOHEAD,
	H264DECDRV_BADFMT,
	H264DECDRV_MEMFAIL,
	H264DECDRV_INITFAIL,
	H264DECDRV_EXIT,
	H264DECDRV_WRONG_VERSION,
	H264DECDRV_KERNEL_MODULE_PROBLEMS,	
}H264decDrvError_t;

#define	DEC_FAIL	            -1
#define DEC_OK			    H264DECDRV_OK
#define	DEC_MEMORY		    H264DECDRV_NOMEM
#define DEC_NOHEAD                  H264DECDRV_NOHEAD
#define DEC_WRONGVERSION            H264DECDRV_WRONG_VERSION
#define DEC_BAD_FORMAT              H264DECDRV_BADFMT
#define DEC_MEM_FAIL		    H264DECDRV_MEMFAIL
#define DEC_INIT_FAIL		    H264DECDRV_INITFAIL
#define DEC_EXIT		    H264DECDRV_EXIT
#else
#define H264decDrvError_t   l32
#define	DEC_OK		0
#define	DEC_FAIL	-1
#endif //_LINUX_

#define DEC_MAX_FRAME_BUFFER_LEN (128*1024)
#ifndef DEC_MAX_NALU_NUM
#define DEC_MAX_NALU_NUM  128    
#define	DEC_MAX_NALU_UNIT_SIZE	 (4*DEC_MAX_NALU_NUM)
#endif

// interface function declaration
#ifdef __cplusplus
extern "C"
{
#endif 
	
	typedef struct 
	{
		u8	*pu8YUV420;			//解码YUV数据缓冲区，用户在调用解码器前自行分配
		u32	u32YUVLen;          //解码后一帧YUV图像的长度，在把解码后的YUV图像输出时用到
		u8	*pu8BitStream;		//待解码的H264一帧图像的码流缓冲区
		u32	u32BitstreamLen;	//待解码的H264一帧图像的码流长度
		u32	u32ReturnWidth;		//当前正在解码的图像的宽度
		u32	u32ReturnHeight;	//当前正在解码的图像的高度
		u8  *pu8Output88MV;     //用户在解码器外部分配一块396*4*sizeof(u8)的内存指针作为输入
		                        //输出396*4个8x8块的运动级别，块的位置如下图
		                        //运动级别取值范围1-10，值越大运动越剧烈，如输出为0，表示没有运动
		                        //目前不支持4CIF(720 x 576)的运动级别输出		                        
		                        //注意：如不需要查看运动级别，此处必须传入NULL !!!!
	} TH264DecFramePara;
	
	//sq 05.4.26 add 88mv
	/*数组元素指向图像中的396*4个8x8块的位置如图( s16First88BAddr=(l32MBNr/22)*88+(l32MBNr%22)*2 )：

	[0 ][1 ]  [2 ][3 ]  [4 ][5 ]      ......    ......        [42][43]
	[44][45]  [46][47]  [48][49]                              [86][87]
	 ......
	 ......
	[1496][1497]                         ......    ......     [1538][1539]
	[1540][1541]                                              [1582][1583]
    
	*/

	typedef	struct 
	{
		l32 l32Width;		   	//输入图像宽度[0,720]必须为16的倍数 CIF:352 QCIF:176
		l32 l32Height;		    //输入图像高度[0,576]必须为16的倍数 CIF:288	QCIF:144
		l32 l32DecMode;         //待解码的H264的码流格式。 0：rtp格式，1：annexb格式 
	} TH264DecPara;
	
	typedef struct  
	{
		s8 *ps8Version;   //版本信息
		l32 l32Width;	//图像宽度
		l32 l32Height;	//图像高度
	} TH264DecStatus;
	
	typedef enum {
		H264_DECODE_OK,
		H264_DECODE_ERROR	
	}TVideoErrNum;
	
#ifndef CHIP_DM642
    //解码器初始化
	H264decDrvError_t H264DecoderInit(void **ppvHandle, TH264DecPara *ptDecPara);

	//解码一帧图像，输出为352*288
	H264decDrvError_t H264DecodeFrame(void *pvHandle, TH264DecFramePara *ptFrmPara);

	//释放解码器资源
	H264decDrvError_t H264DecoderClose(void *pvHandle);
		
    //获取H264解码器版本信息函数
	void GetH264Version(void *pvVer, l32 l32BufLen, l32 *pl32VerLen);
	
	//主要反馈解码器的版本信息和编译时间
	void H264DecHelp(void);	
#else
	
	//解码器初始化
	TVideoErrNum H264DecoderOpen(void **ppvHandle, TH264DecPara *ptDecPara);
	
	//解码一帧图像，输出为352*288
	TVideoErrNum  H264DecoderDecFrame(void *pvHandle, TH264DecFramePara *ptFrmPara);
	
	//释放解码器资源
	TVideoErrNum  H264DecoderClose(void *pvHandle);
	
    //获取H264解码器版本信息函数
	TVideoErrNum  H264DecoderGetStatus(void *pvHandle, TH264DecStatus *ptH264DecStatus);
	
#endif

#ifdef __cplusplus
};
#endif

#endif
