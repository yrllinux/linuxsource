/******************************************************************************
模块名	：                        
文件名	：g711.h
相关文件	：
文件实现功能：declare of g711 function
作者		：chunbo
版本		：1.0
-------------------------------------------------------------------------------
修改记录:
日  期		版本		修改人		修改内容
06/04/03    1.0         chunbo      add file header
2005/05/09  2.0         徐  超      规范化数据类型
*******************************************************************************/
#ifndef G711_H
#define G711_H
#include "algorithmtype.h"

u8 Linear2Alaw(s16 nPcmVal);    
s16 Alaw2Linear(u8 ucAval);
u8 Linear2Ulaw(s16 nPcmVal);    
s16 Ulaw2Linear(u8 ucUval);
u8 Alaw2Ulaw(u8 ucAval);
u8 Ulaw2Alaw(u8 ucUval);

s32 Linear2AlawBlock(s16 *nPcmVal, u8 *uAlawVal, s16 iPcmLen);
s32 Alaw2LinearBlock(s16 *nPcmVal, u8 *uAlawVal, s16 iAlawLen);
s32 Linear2UlawBlock(s16 *nPcmVal, u8 *uUlawVal, s16 iPcmLen);
s32 Ulaw2LinearBlock(s16 *nPcmVal, u8 *uUlawVal, s16 iUlawLen);
s32 Alaw2UlawBlock(u8 ucAval, u8 ucUval, s16 iAlawLen);
s32 Ulaw2AlawBlock(u8 ucAval, u8 ucUval, s16 iUlawLen);
void  GetG711Version(char *Version, int StrLen, int * StrLenUsed);

#endif

