/******************************************************************************
模块名	：                        
文件名	：g711snr.h
相关文件	：
文件实现功能：declare of g711 function
作者		：王水晶
版本		：1.3
-------------------------------------------------------------------------------
修改记录:
日  期		版本		修改人		修改内容
2005/07/05  1.3         王水晶      add file header
*******************************************************************************/
#ifndef G711_H
#define G711_H

#ifdef __cplusplus
extern "C" {
#endif

u8 Linear2Alaw(s16 nPcmVal);    
s16 Alaw2Linear(u8 ucAval);
u8 Linear2Ulaw(s16 nPcmVal);    
s16 Ulaw2Linear(u8 ucUval);
u8 Alaw2Ulaw(u8 ucAval);
u8 Ulaw2Alaw(u8 ucUval);

//l32 Linear2AlawBlock(s16 *nPcmVal, u8 *uAlawVal, s16 iPcmLen);
l32 Linear2AlawBlock(s16 *nPcmVal, u8 *uAlawVal, s16 Len, s16 bG711SNR_Flag, s16 *ps16G711SNR);
l32 Alaw2LinearBlock(s16 *nPcmVal, u8 *uAlawVal, s16 iAlawLen);
//l32 Linear2UlawBlock(s16 *nPcmVal, u8 *uUlawVal, s16 iPcmLen);
l32 Linear2UlawBlock(s16 *nPcmVal, u8 *uUlawVal, s16 Len, s16 bG711SNR_Flag, s16 *ps16G711SNR);
l32 Ulaw2LinearBlock(s16 *nPcmVal, u8 *uUlawVal, s16 iUlawLen);
l32 Alaw2UlawBlock(u8 ucAval, u8 ucUval, s16 iAlawLen);
l32 Ulaw2AlawBlock(u8 ucAval, u8 ucUval, s16 iUlawLen);
void  GetG711Version(s8 *Version, l32 StrLen, l32 * StrLenUsed);

#ifdef __cplusplus
}
#endif

#endif

