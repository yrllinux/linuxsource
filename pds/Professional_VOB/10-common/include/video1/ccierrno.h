#ifndef _CCIERRORNO_H_
#define _CCIERRORNO_H_

#ifdef __cplusplus
extern "C" {
#endif

#define ERR_FLAG_MAX_NUM 16
#define ENCODER_ERROR_ID 1
#define DECODER_ERROR_ID 2

#include "basetype.h"
extern VOLATILE int ErrStatus[16];
void  CciWriteErrNo(int nErrIdx, int  nErrNo);
void  CciReadErrNo(int nErrIdx, int* pnErrno);

#ifdef __cplusplus
}
#endif 

#endif