/*****************************************************************************
模块名      : g728编解码器
文件名      : g728.h
相关文件    : 
文件实现功能: g728编解码功能句柄式接口声明
版本        : V1.0  Copyright(C) 2005-2007 KDC, All rights reserved.
作者        : 徐超
-----------------------------------------------------------------------------
修改记录:
日    期      版本        修改人      修改内容
2006/04/30    1.0         徐  超      规范整理
******************************************************************************/
#ifndef G728_H
#define G728_H

#ifdef __CPLUSPLUS
extern "C" {
#endif

// external interface declaration

// macro definitions
#define G728_OK   0
#define G728_ERR -1
#define HG728    void *

int  g728_encinit(HG728* encptr);
int  g728_decinit(HG728* decptr);

int  g728_encode(HG728 pG728,
				 short* EncIn,
				 int inputlengthinbyte,	//需是40的整数倍
				 unsigned char* encbitstream);
int  g728_decode(HG728 pG728,
				 unsigned char* decbitstream,
				 int inputlengthinbyte,	//需是5的整数倍
				 short* DecOut);

void g728_encfree(HG728 encptr);
void g728_decfree(HG728 decptr);
void  GetG728Version(char *Version, int StrLen, int * StrLenUsed);

#ifdef __CPLUSPLUS
}
#endif

#endif /*G728_H*/

