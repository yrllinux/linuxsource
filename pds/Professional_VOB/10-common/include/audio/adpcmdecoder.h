
/*****************************************************************************
模   块   名: IMA ADPCM 解码器
文   件   名: adpcmdecoder.h
相 关  文 件: 
文件实现功能: IMA ADPCM 解码器头文件
版        本: V1.0  Copyright(C) 2005-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
修 改 记 录:
日       期      版本        修改人      修改内容
 2006/06/28      1.0         XXR         创建
******************************************************************************/
#ifndef ADPCMDECODER_H
#define ADPCMDECODER_H

#ifdef __cplusplus
extern "C" {
#endif

//Get Version NO
//void  GetAdpcmDecVersion(u8 *Version, l32 StrLen, l32 *StrLenUsed);

/*=============================================================================
函 数 名: IMAADPCMDecoder
功    能: IMA ADPCM解码器
算法实现: 每次解码一帧数据，数据格式如下:
	byte0~ byte31: 码流头信息.目前的解码器只支持右声道, 有效信息为byte28~byte31
	byte32~byte159: 编码有效数据，共128字节，即256个样点的编码码流，前后4bit倒装
	
全局变量: 无
参    数: pu8InputDataAddr		  [in]输入数据地址
          ps16OutputDataAddr      [out]输出数据地址，共256个s16数据
          
返 回 值: 解码数据长度(字节)  恒等于512(字节)
作    者: XXR
=============================================================================*/
u32 IMAADPCMDecoder(u8 *pu8InputDataAddr, s16 *ps16OutputDataAddr);

#ifdef __cplusplus
}
#endif


#endif //end of ADPCMDECODER_H
