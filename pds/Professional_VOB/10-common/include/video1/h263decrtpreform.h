/******************************************************************************
模块名	    ： H263DecRtpReform                
文件名	    ： H263DecRtpReform.h
相关文件	： 
文件实现功能： 解译H263 RTP 信息
作者		： 胡来胜	
版本		： 1.0
---------------------------------------------------------------------------------------
修改记录:
日  期		版本		修改人		走读人    修改内容
07/08/2005  1.0			胡来胜				  创建
**************************************************************************************/

#ifndef _H263DECRTPREFORM_H_
#define _H263DECRTPREFORM_H_
#ifdef __cplusplus
extern "C" {
#endif


#include "kdvtype.h"


#define  DECRTPERR_OK 0
#define  DECRTPERR_ERROR -300

s32 H263AddRtpHdr(u8 *pu8H320Bitstream, s32 s32BitstreamLen, u8 *au8H323BitstreamBuf,s32 *ps32H323BitstreamLen);

void GetH263DecFormRTPVersion(void *pVer, s32 iBufLen, s32 *pVerLen);
void DumpRTPHead(u8 *pu8RTPData);
#ifdef __cplusplus
};
#endif 

#endif /*_H263DECRTPREFORM_H_*/	
