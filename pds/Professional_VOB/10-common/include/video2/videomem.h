#ifndef VIDEO_MEMORY_HEAD

#define VIDEO_MEMORY_HEAD

#include "basetype.h"

#define VIDEO_MEMORY_PHYSICAL 0x0001
#define VIDEO_MEMORY_VIRTUAL 0x0002

typedef unsigned long int u32Mem;

l32 VideoMemInitial(void **ppvMemHadle, l32 l32VirtualMemSize, l32 l32PhysicalMemSize);

void *VideoMalloc(void *pvMemHandle, l32 l32MemSize, l32 l32AlignBits, l32 l32MemType);

l32 VideoFree(void *pvMemHandle, void *pvMem);

l32 VideoMemFree(void *pvMemHandle);

l32 VideoMemGetStatis(void *pvMemHandle, 
					   l32 *pl32VirtualMemSize, l32 *pl32UsedVirtualMemSize,
					   l32 *pl32PhysicalMemSize, l32 *pl32UsedPhysicalMemSize);
					   
extern u32Mem MempoolInit();
extern u32Mem MempoolFree(void   *pFreePt);
extern u32Mem MempoolClose();
extern u32Mem MempoolClose();					   
				

#endif

