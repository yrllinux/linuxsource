/*****************************************************************************
模   块   名: g723decoder
文   件   名: g723decoder.h
相 关  文 件: 
文件实现功能: G723解码器头文件
版        本: V1.0  Copyright(C) 2005-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
修 改 记 录:
日       期      版本        修改人      修改内容
                 1.0         ******      创    建         
 2005/04/28      2.0         徐  超      1.添加了包含控制宏G723DECODER_H
 2005/09/02      2.1         王水晶      数据类型规范化,for linux test
******************************************************************************/
#ifndef G723DECODER_H
#define G723DECODER_H


l32 G723DecodeChunk(void *phHanlde, s16 *PcmDataBuff,\
					s8 *G723Stream, s16 Crc );
l32 G723DecodeInit(void **phHandle);
l32 G723DecodeClose(void *phHandle);
void GetG723Version(char *Version, l32 StrLen, l32 * StrLenUsed);

#endif //end of G723DECODER_H








