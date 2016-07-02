/*****************************************************************************
模   块   名: g723encoder
文   件   名: g723encoder.h
相 关  文 件: 
文件实现功能: G723编码器头文件
版        本: V1.0  Copyright(C) 2005-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
修 改 记 录:
日       期      版本        修改人      修改内容
                 1.0         ******      创    建         
 2005/04/28      2.0         徐  超      1.添加了包含控制宏G723ENCODER_H
 2005/09/02      2.1         王水晶      数据类型规范化,for linux testt
******************************************************************************/
#ifndef G723ENCODER_H
#define G723ENCODER_H


#define G723_CODEC_TRUE 0
#define G723_CODEC_FALSE 1

#define G723_53_RATE 0
#define G723_63_RATE 1

#define G723_HF_ON	0
#define G723_HF_OFF 1

l32 G723CodeInit(void **phHandle, l32 iHfFlg);
l32 G723CodeChunk(void *phHandle, s16 *PcmDataBuff, \
				  s8 *G723Stream, l32 iWorkRate );
void G723CodeClose( void *phHandle);
void GetG723Version(char *Version, l32 StrLen, l32 * StrLenUsed);

#endif //end of G723ENCODER_H


