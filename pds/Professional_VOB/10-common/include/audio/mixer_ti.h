/******************************************************************
模块名 			：混音器错误码定义文件
文件名 			：mixErr.h
相关文件		：
文件实现功能	：混音器错误码定义
作者			：缪春波	
版本			：0.1
创建日期		：2003/6/10
-------------------------------------------------------------------
修改记录：
日期     	  版本    	 修改人    修改内容
2004/11/19    0.2        徐  超    修改了接口名字MixAud20成MixAud32
******************************************************************/
#ifndef MIXER_TI_H
#define MIXER_TI_H

#define FAILE 1
#define SUCCESS 0

#define MAX_CHANNEL 32


typedef struct
{
    u8    *mPCMValue; //pointer to Audio Buffer
    u32   nVolume;  //Audio Volume
}TAudBuf;

l32 MixAud20(TAudBuf **aptInAudBuf,
			  u32 u32InChList,
              TAudBuf **aptOutAudBuf,
              TAudBuf *ptOutNAudBuf,
               u32 u32InSampleCnt);
             
l32 MixAud32(TAudBuf *InBufArray[],
             u32 InChIndex,
             TAudBuf *OutBufArray[],
             TAudBuf *NoutChannel,
             u32 uiPCMLength);             


#endif

