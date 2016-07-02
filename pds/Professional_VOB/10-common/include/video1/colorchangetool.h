
#ifndef BV_COLORCHANGETOOL_H
#define BV_COLORCHANGETOOL_H

#define MAX_WIDTH_PIXEL 1440
#define PPHandle 	void *
typedef struct PP_Instance_s
{
	uint64_t bufYUV[2][MAX_WIDTH_PIXEL/8];
	
	uint64_t bufYIn[4][MAX_WIDTH_PIXEL/8];
	uint64_t bufYOut[2][MAX_WIDTH_PIXEL/8];
	
	uint64_t bufUIn[4][MAX_WIDTH_PIXEL/16];
	uint64_t bufUOut[2][MAX_WIDTH_PIXEL/16];
	uint64_t bufVIn[4][MAX_WIDTH_PIXEL/16];
	uint64_t bufVOut[2][MAX_WIDTH_PIXEL/16];
	
	
	int32_t src_width, src_height, src_stride;
	int32_t dst_width, dst_height, dst_stride;
	
	DS_PATH_INFO pathIn;
	DS_CHANNEL chInSrc, chInDst;
#ifdef _LINUX_
	VOLATILE DS_DESCRIPTOR *descInSrc, *descInDst;
#else	
	DS_DESCRIPTOR *descInSrc, *descInDst;
#endif

	DS_PATH_INFO pathYOut;
	DS_CHANNEL chYOutSrc, chYOutDst;
#ifdef _LINUX_
	VOLATILE DS_DESCRIPTOR *descYOutSrc, *descYOutDst;
#else	
	DS_DESCRIPTOR *descYOutSrc, *descYOutDst;
#endif	
	
	DS_PATH_INFO pathUOut;
	DS_CHANNEL chUOutSrc, chUOutDst;
#ifdef _LINUX_
	VOLATILE DS_DESCRIPTOR *descUOutSrc, *descUOutDst;
#else	
	DS_DESCRIPTOR *descUOutSrc, *descUOutDst;
#endif
	
	DS_PATH_INFO pathVOut;
	DS_CHANNEL chVOutSrc, chVOutDst;
#ifdef _LINUX_
	VOLATILE DS_DESCRIPTOR *descVOutSrc, *descVOutDst;
#else	
	DS_DESCRIPTOR *descVOutSrc, *descVOutDst;
#endif	
	
} PP_Instance;
/***
*** Now image preprocess only complete 422 to 420 converte
*** and five-point low pass filter
*** we will add some image analizing to it later
***/

void  GetColorChangeToolVersion(void *pVer, int iBufLen, int *pVerLen);


int PreProcessInit(PPHandle *pHandle, int src_width, int src_height, int src_stride, \
		int dst_width, int dst_height, int dst_stride);

int PreProcessFree(PPHandle Handle);

int PreProcessSetParam(PPHandle *pHandle, int src_width, int src_height, int src_stride, \
		int dst_width, int dst_height, int dst_stride);


/* NOTICE:                      */
/* now only support 4cif format */
int PreProcess(PPHandle Handle, uint8_t *src, uint8_t *dst);
int PreProcess_nofilter(PPHandle Handle, UINT8 *src, UINT8 *dst, uint16_t *pSum8x8);

int ChangeYUV(UINT8* pIn,UINT8* pOut);

void EvenToOddFieldCopy(unsigned char *frame_ref[],int width,int height,int stride);//ÆæÅ¼³¡Êý¾Ý¿½±´
void YUV420To422(unsigned char * yuv420[3],unsigned char * yuv422,int stride, int height, int width);
#endif // BV_COLORCHANGETOOL_H
