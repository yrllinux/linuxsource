#ifndef _MP4V_H_
#define _MP4V_H_

#ifdef __cplusplus
extern "C" {  
#endif 



// API Version: 1.01
#define API_VERSION ((1 << 16) | 1)

#define MPEG4_ENCRYPT				0x1111
#define MPEG4_NO_ENCRYPT			0x0000

// Quant type
#define MP4V_H263QUANT			0
#define MP4V_MPEGQUANT			1

// processors' properties
#define MP4V_CPU_MMX		0x00010000
#define MP4V_CPU_MMXEXT		0x00020000
#define MP4V_CPU_SSE		0x00040000
#define MP4V_CPU_SSE2		0x00080000
#define MP4V_CPU_3DNOW		0x00100000
#define MP4V_CPU_3DNOWEXT	0x00200000

// colorspaces
#define MP4V_CSP_YV12	0
#define MP4V_CSP_YUY2	1
#define MP4V_CSP_UYVY	2
#define MP4V_CSP_I420	3
#define MP4V_CSP_RGB24 	4
#define MP4V_CSP_YVYU	5
#define MP4V_CSP_RGB32 	6
#define MP4V_CSP_RGB555 7
#define MP4V_CSP_RGB565 8
#define MP4V_CSP_NULL 	-1

#define MP4V_CSP_HFLIP	0x40000000	// horizontal flip flag
#define MP4V_CSP_VFLIP	0x80000000	// vertical flip flag

// processing is fine
#define MP4V_OK				0
// error message
#define MP4V_ERR_FAIL		-1
#define	MP4V_ERR_MEMORY		1
#define MP4V_ERR_FORMAT		2
#define MP4V_ERR_DONTDRAW	3

// Rate Control mode
#define CONSTANT_BITRATE				0
#define VBR_SIZE						1
#define VBR_QUALITY						2
#define VBR_QUANTIZER					3

typedef struct 
{
	int cpu_flags;
	char cpu_brand[64];
	int api_version;
} MP4V_INIT_PARAM;

/*************************************************************************
  接口函数实现
int RMP4_codec_initialize(void *handle, int opt, void *param1, void *param2);
	初始化 codec

int RMP4_decoder(void * handle, int opt, void * param1);
	解码接口函数

int RMP4_encoder(void * handle, int opt, void * param1);
    编码接口函数 

**************************************************************************/


int RMP4_codec_initialize(
							  void *handle/*保留参数，设为NULL*/, 
							  int opt/*保留参数，设为NULL*/,
							  void *param1/*初始化参数MP4V_INIT_PARAM的指针*/, 
							  void *param2/*保留参数，设为NULL*/
						  );

int RMP4_decoder(
					  void * handle/*解码器句柄*/,	
					  int opt/*解码器命令参数*/, 
					  void * param1/*解码器数据参数*/
    			 );

int RMP4_encoder(
					 void * handle/*编码器句柄*/,	
					 int opt/*编码器命令参数*/, 
					 void * param/*编码器数据参数*/
				 );


// ==========================================
//	decoder
// ==========================================
typedef struct 
{
	void *handle;
	int width;
	int height;
	int colorspace;
} MP4V_DEC_PARAM;


typedef struct
{
	int colorspace;		// [in]
	int stride;			// [in] width of output frame
	int length;			// [in] bitstream length

	void * bitstream;	// [in] bitstream length
	void * image;		// [in]/[out] image pointer
} MP4V_DEC_FRAME;		// to decode one frame


// decoder options
#define MP4V_DEC_CREATE		0
#define MP4V_DEC_DECODE		1
#define MP4V_DEC_DESTROY	2
#define MP4V_DEC_DECODE_FASTER 3   //用于快速解码，强制解码器不做DEBLOCK

// ==========================================
//	encoder
// ==========================================
typedef struct
{
	void * handle;			// [out] encoder instance handle
	int width;
	int height;

	int quant_type;			// H.263 || MPEG

	int quant;				// [in] use for vbr_quantizer
	int quality;			// [in] use for vbr_quality

	int deinterlacing_enable;
	int bidirectional_encoding;

	int interlaced_coding;
	int top_field_first;
	int alternative_scan;

	int rc_mode;
	double rc_max_delay;	// maximum vbv_delay
	int adapt_quant;

	int fincr, fbase;		// frame increment, fbase. each frame = "fincr/fbase" seconds
	int bitrate;			// the bitrate of the target encoded stream, in bits/second
	int rc_buffersize;		// the rate control buffersize
	int max_quantizer;		// the upper limit of the quantizer
	int min_quantizer;		// the lower limit of the quantizer
	int max_key_interval;	// the maximum interval between key frames
	int encrypt;			// MPEG4_ENCRYPT:yes; others:no
						
} MP4V_ENC_PARAM;


typedef struct
{

	void * image;			// [in] image pointer, where the image is encoded
    int colorspace;			// [in] source colorspace

    int quant;				// [in] preset quantizer, for VBR/quantizer/quality
	void * bitstream;		// [in] bitstream pointer
    int intra;				// [in]	force intra frame 
							
	int length;				// [out] bitstream length (bytes)
	int  IsEncodeOneBlankFrame;

} MP4V_ENC_FRAME;


#define MP4V_ENC_CREATE		0
#define MP4V_ENC_ENCODE		1
#define MP4V_ENC_DESTROY	2


#ifdef __cplusplus

}
#endif 

#endif /* _MP4V_H_ */
