/**************************************************************************
 * 
 * Brovic Technologies Co., Ltd.  
 *  Copyright (c)2001-2002 
 *  
 *  http://www.brovic.com 
 * 
 **************************************************************************/
#ifndef _MP4V_ENC_H
#define _MP4V_ENC_H

#ifdef __cplusplus
extern "C" {
#endif 
#define Mp4vEncHandle void *
#define _NEW_INTERFACE_

#define SOURCE_FORMAT_FRMAERATELOWTHANONE	0x0100  	//编码帧率小于1
#define SOURCE_FORMAT_FRMAERATEPARAM 		0x0080  	//编码帧率是否由外部设定 1:是，0：否
#define SOURCE_FORMAT_AUTODELFRAME 			0x0040		//编码器是否自动丢帧1：是，0：否

//#define SOURCE_FORMAT_SIZEAUTO				0x0010		//编码分辨率可以根据码率大小自动调节
#define SOURCE_FORMAT_MERGEENC 				0x0010      //用于图像合成编码
#define SOURCE_FORMAT_FOURCIF 				0x0008      //固定为FOURCIF编码
#define SOURCE_FORMAT_TWOCIF 				0x0004      //固定为TWOCIF编码
#define SOURCE_FORMAT_CIF 					0x0002		//固定为CIF编码
#define SOURCE_FORMAT_HALFCIF 				0x0001		//固定为HALFCIF编码


#define MPEG4_ENCODE_ONLY_420				1
#define MPEG4_ENCODE_422_420				0

#define MPEG4_ME_FILED 				0x0020		//搜索半场	

#define MPEG4_ENCRYPT				0x1111
#define MPEG4_NO_ENCRYPT			0x0000

//kui
#define QUARTER_PIXEL_SEARCH				0x12345678
//kui end

#ifdef _LINUX_
typedef enum
{
	MP4ENCDRV_OK,
	MP4ENCDRV_ERROR,
	MP4ENCDRV_NOMEM,
	MP4ENCDRV_BADFMT,
	MP4ENCDRV_WRONG_VERSION,	
	MP4ENCDRV_KERNEL_MODULE_PROBLEMS,	
}mp4encDrvError_t;

#define ENC_FAIL		    MP4ENCDRV_ERROR
#define ENC_OK			    MP4ENCDRV_OK
#define	ENC_MEMORY		    MP4ENCDRV_NOMEM
#define ENC_BAD_FORMAT		MP4ENCDRV_BADFMT
#else
#define mp4encDrvError_t    int 
#define ENC_FAIL		    -1
#define ENC_OK			    0
#define	ENC_MEMORY		    1
#define ENC_BAD_FORMAT		2
#endif

typedef struct {                                                                                                                         
	int x_dim;		// the x dimension of the frames to be encoded                              
		// 帧宽度                                
	int y_dim;		// the y dimension of the frames to be encoded  
	    // 帧长度                                
	int framerate;        // the frame rate of the sequence to be encoded, in frames/second  
		//帧率,保留参数                    
	long bitrate;	        // the bitrate of the target encoded stream, in bits/second         
	 	// 波特率                          
	int rc_period;          // the intended rate control averaging period                      
	  	// 保留参数                      
	int rc_reaction_period; // the reaction period for rate control                             
	 	// 保留参数                      
	int rc_reaction_ratio;  // the ratio for down/up rate control                               
	 	// 保留参数                      
	int max_quantizer;      // the upper limit of the quantizer                                 
	 	// 最大量化步长                  
	int min_quantizer;      // the lower limit of the quantizer                                  
		// 最小量化步长                  
	int max_key_interval;   // the maximum interval between key frames                          
	 	// I帧间最大P帧数目              
	int use_bidirect;	// use bidirectional coding                                             
	 	// 保留参数                          
	int deinterlace;	// fast deinterlace                                                      
		// 保留参数                              
	int quality;		// the quality of compression ( 1 - fastest, 5 - best )                 
	 	// 压缩参数 ( 1 - fastest, 5 - best )
	void *handle;		// the empty handle, which will be filled by MP4V_ENC                   
	 	// 内部传递参数                      
	int flags;		//flags for internal control of encoder                                     
		//运动估计控制参数
//added by xmxie 20031216
	int source_format;
		//编码器方式选择:
		//0x0001
		//
	int EncodeOnly420;
		
	int PSNRflag;
		//决定是否计算PSNR值：1：是；0：否
//xmxie added end

//kui
	int iQPixel;		//whether encrypt bitstream or not; MPEG4_ENCRYPT:encrypt, others:no encrypt
//kui end

} Mp4vEncParam;                                                                                 

typedef struct {
	void *image;	// the image frame to be encoded
		// 帧原始图像指针
	void *bitstream;// the buffer for encoded bitstream
		// 指定输出码流的地址
	int length;	// the length of the encoded bitstream
		// 该帧输出码流大小
	int quant;	// quantizer for this frame; only used in VBR modes
		// 指定该帧的量化步长,保留参数
	int frametype;	// force this frame to be I_VOP(1),I_VOP+VOLHEAD(2)
		// 指定编码帧格式,I_VOP(1),I_VOP+VOLHEAD(2)
		
	unsigned char  *pu8Output88MV;     //用户在解码器外部分配一块396*sizeof(u8)的内存指针作为输入
		                        //输出396个运动级别，块的位置如下图
		                        //运动级别取值范围0-9，值越大运动越剧烈，如输出为0，表示没有运动		                        
		                        //注意：如不需要查看运动级别，此处必须传入NULL !!!!
		
} Mp4vEncFrame;

/*
 *  编码完一帧时的输出参数
 */
typedef struct {
	int isKeyFrame;		// the current frame is encoded as a key frame
		// 是否为I帧
	int quantizer;		// the quantizer used to encode the current frame
		// 该帧的量化步长
	int texture_bits;	// the number of bits used for texture coding
		// 保留参数
	int motion_bits;	// the number of bits used for motion vectors
		// 保留参数
	int total_bits;		// the total number of bits used for the current frame
		// 该帧码流长度
	float PSNR;
		
	int width;
		//实际编码宽度
	int height;	
		//实际编码高度
} Mp4vEncResult;

#ifndef _NEW_INTERFACE_
// the prototype of the mp4v_enc() - main encode engine entrance
int mp4v_enc(void* handle,  // handle		- the handle of the calling entity, must be unique
	int enc_opt,	        // enc_opt		- the option for encoding, see below
	void *param1,	        // param1		- the parameter 1 (its actually meaning depends on enc_opt
	void *param2);	        // param2		- the parameter 2 (its actually meaning depends on enc_opt
#else
mp4encDrvError_t Mp4vShowStatis(Mp4vEncHandle Handle);
mp4encDrvError_t Mp4vSetParam(Mp4vEncHandle Handle,Mp4vEncParam* pParam);
mp4encDrvError_t Mp4vEncoderInit(Mp4vEncHandle *pHandle, Mp4vEncParam * pEncParam);
mp4encDrvError_t Mp4vEncoderFrame(Mp4vEncHandle Handle, Mp4vEncFrame* pFrame, Mp4vEncResult* pResult);
mp4encDrvError_t Mp4vEncoderFrame422(Mp4vEncHandle Handle, Mp4vEncFrame* pFrame, Mp4vEncResult* pResult);
mp4encDrvError_t Mp4vEncoderFree(Mp4vEncHandle Handle);
void  GetMPEG4Version(void *pVer, int iBufLen, int *pVerLen);
mp4encDrvError_t Mp4vMergeEncoder(Mp4vEncHandle Handle, Mp4vEncFrame* pFrame, Mp4vEncResult* pResult,                      
					void *input0,void *input1,void *input2,void *input3);
#endif
	
// MP4V_ENC options (the enc_opt parameter of MP4V_ENC())
#define ENC_OPT_INIT    0	// initialize the encoder, return a handle
#define ENC_OPT_RELEASE 1	// release all the resource associated with the handle
#define ENC_OPT_ENCODE	2	// encode a single frame in one-pass mode
#define ENC_OPT_RESET	3	// encode a single frame in one-pass mode

extern int iFavorMv16x16;
extern int iNeighTend16x16;
extern int iNeighTend8x8;
#ifdef _DEBUG
	extern int bDebugShowDetail; //flag to assist debug
#endif//_DEBUG

//#include "config.h"

#ifdef __cplusplus
}
#endif 

#endif
