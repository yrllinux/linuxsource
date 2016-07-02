/**
*** Copyright (c) 2002 Brovic Corp., Ltd.
**/

#ifndef BV_DS_AUX_HEADFILE
#define BV_DS_AUX_HEADFILE

#define OPTIMIZE_FOR    100

#ifdef _LINUX_
#include "algorithmtype.h"
#include "ds.h"
#include "ds_desc_api.h"
#define  kprintf  __SYSLOG//zhaobo.050928
#else
#include "algorithmtype.h"
#include <eti/ds.h>
#endif

#define DCACHE_SIZE 0x8000
#define DS_WAIT_TIMEOUT  1000000

//Should be called before a new DsKick!
#define ResetDsPath(path) DsCtrlPath(path, DS_CTRL_RESET, NULL)

//We add this macro to compensate the bugs of DsWaitHalted...
#ifdef _LINUX_
//#define WaitDsHalt(ch) { SCODE s=DsWaitHalted(ch, DS_TIMEOUT_INDEFINITE); }
#define WaitDsHalt(ch) DsWaitHalted(ch, DS_TIMEOUT_INDEFINITE)
#endif

extern DS_PATH_INFO dspath_m2m_0, dspath_m2m_1, dspath_m2m_2, dspath_m2m_3;
extern DS_PATH_INFO dspath_m2m_4, dspath_m2m_5, dspath_m2m_6, dspath_m2m_7;
extern DS_PATH_INFO dspath_m2m_8, dspath_m2m_9, dspath_m2m_10, dspath_m2m_11;
extern DS_PATH_INFO dspath_vlxGBIn, dspath_vlxGBOut;
extern DS_PATH_INFO dspath_gpdpIn, dspath_gpdpOut;
extern UINT64 HalfDataCache[DCACHE_SIZE/16]; //2048
//extern uint8_t *HalfDataCache; //16KByte


//return: TRUE succeeded; FALSE error.
int VideoCodecUtilDSInitial();
BOOL VideoCodecUtilDSInitialized();
int VideoCodecUtilDSRelease();
int VideoCodecUtilDSGetPath();
int VideoCodecUtilDSReturnPath();
void  GetVideoCodecUtilDSVersion(void *pVer, int iBufLen, int *pVerLen);


//return: 0 succeeded; -1 error.
BOOL IsBvDsAuxInitiated();
int BvDsAuxInit();
void BvDsAuxClose();


//return: 0 succeeded; -1 error.
int SetupDsPath(DS_PATH_INFO *pDsPath, BOOL isSrcIO, BOOL isDstIO,
	int32_t bufferSize, int32_t srcSize, int32_t dstSize, int32_t dtsPriority);
	
	
	
////////////////////tmp file////////////////////////////


typedef struct PreProcessHandletag
{
#ifdef _LINUX_
	VOLATILE DS_DESCRIPTOR InputDescSrcPreY[14];
	VOLATILE DS_DESCRIPTOR InputDescDstPreY[14];
	VOLATILE DS_DESCRIPTOR InputDescSrcPreU[14];
	VOLATILE DS_DESCRIPTOR InputDescDstPreU[14];
	VOLATILE DS_DESCRIPTOR InputDescSrcPreV[14];
	VOLATILE DS_DESCRIPTOR InputDescDstPreV[14];

	VOLATILE DS_DESCRIPTOR InputDescSrcCurY[2];
	VOLATILE DS_DESCRIPTOR InputDescDstCurY[2];
	VOLATILE DS_DESCRIPTOR InputDescSrcCurU[2];
	VOLATILE DS_DESCRIPTOR InputDescDstCurU[2];
	VOLATILE DS_DESCRIPTOR InputDescSrcCurV[2];
	VOLATILE DS_DESCRIPTOR InputDescDstCurV[2];
 
	VOLATILE DS_DESCRIPTOR OutputDescSrcY[2];
	VOLATILE DS_DESCRIPTOR OutputDescDstY[2];
	VOLATILE DS_DESCRIPTOR OutputDescSrcU[2];
	VOLATILE DS_DESCRIPTOR OutputDescDstU[2];
	VOLATILE DS_DESCRIPTOR OutputDescSrcV[2];
	VOLATILE DS_DESCRIPTOR OutputDescDstV[2];

	VOLATILE DS_DESCRIPTOR OutputDescSrcCurY[2];
	VOLATILE DS_DESCRIPTOR OutputDescDstCurY[2];
	VOLATILE DS_DESCRIPTOR OutputDescSrcCurU[2];
	VOLATILE DS_DESCRIPTOR OutputDescDstCurU[2];
	VOLATILE DS_DESCRIPTOR OutputDescSrcCurV[2];
	VOLATILE DS_DESCRIPTOR OutputDescDstCurV[2];
#else
	//图像点滤波
	DS_DESCRIPTOR InputDescSrcPreY[14];
	DS_DESCRIPTOR InputDescDstPreY[14];
	DS_DESCRIPTOR InputDescSrcPreU[14];
	DS_DESCRIPTOR InputDescDstPreU[14];
	DS_DESCRIPTOR InputDescSrcPreV[14];
	DS_DESCRIPTOR InputDescDstPreV[14];

	DS_DESCRIPTOR InputDescSrcCurY[2];
	DS_DESCRIPTOR InputDescDstCurY[2];
	DS_DESCRIPTOR InputDescSrcCurU[2];
	DS_DESCRIPTOR InputDescDstCurU[2];
	DS_DESCRIPTOR InputDescSrcCurV[2];
	DS_DESCRIPTOR InputDescDstCurV[2];
	
	DS_DESCRIPTOR OutputDescSrcY[2];
	DS_DESCRIPTOR OutputDescDstY[2];
	DS_DESCRIPTOR OutputDescSrcU[2];
	DS_DESCRIPTOR OutputDescDstU[2];
	DS_DESCRIPTOR OutputDescSrcV[2];
	DS_DESCRIPTOR OutputDescDstV[2];

	DS_DESCRIPTOR OutputDescSrcCurY[2];
	DS_DESCRIPTOR OutputDescDstCurY[2];
	DS_DESCRIPTOR OutputDescSrcCurU[2];
	DS_DESCRIPTOR OutputDescDstCurU[2];
	DS_DESCRIPTOR OutputDescSrcCurV[2];
	DS_DESCRIPTOR OutputDescDstCurV[2];
	
#endif
	
	uint8_t *pPrevFrame[7];
	uint16_t *pPrevFrameSum8x8[7];
	uint16_t *pMaxDif;
	uint32_t width;
	uint32_t height;
	uint32_t realwidth;
	uint32_t realheight;
	uint32_t format;
	
	u8 *pau8InputBufY[14];
	u8 *pau8InputBufU[14];
	u8 *pau8InputBufV[14];

	u8 *pau8InputBufCurY[2];
	u8 *pau8InputBufCurU[2];
	u8 *pau8InputBufCurV[2];

	u8 *pau8OutputBufY[2];
	u8 *pau8OutputBufU[2];
	u8 *pau8OutputBufV[2];
		
	uint32_t FrameCounter;
	uint32_t isFrameFilter;
	
}PreProcessHandle;

//#define PREPROCESS_WITH_DS	

#define FREE(ptr) { if (ptr) free(ptr); ptr = NULL; }

int PreProcessInitialCIF(void **pHandle, uint32_t width, uint32_t height, uint8_t *pBuffer);

int PreProcessCloseCIF(PreProcessHandle *pHandle);

void PreProcessCIF(PreProcessHandle *pHandle, uint8_t *pRawImage, uint8_t *pFilteredImage, uint16_t *pSum8x8Cur);

//改变设置参数
int PreProcessSetParamCIF(void *pHandle, uint32_t width, uint32_t height);

#ifndef _LINUX_
static INLINE SCODE WaitDsHalt(DS_CHANNEL channel) 
{
	int i = 0;
	SCODE ret;
	while(S_TRUE != (ret = DsCheckHalted(channel)))
	{
		i++;
		if( i > DS_WAIT_TIMEOUT)
		{
			if(S_FALSE == ret)
			{
				kprintf("the channel is running!");
				return S_ERROR;
			}
			else
			{
				kprintf("DS waits for Halt Time Out!\n");
#ifndef _LINUX_				
				kprintf("Error code: %d!\n", GetLastError());
#endif				
				return S_ERROR;				
			}
			
		}
		hmpv_mnop(63);
	}
	return S_OK;
}
#endif
#endif 


