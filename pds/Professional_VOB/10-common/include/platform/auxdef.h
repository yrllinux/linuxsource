/*******************************************************************************
 * 模块名      : TiVidDrv
 * 文件名      : TiVidDrvDef.h
 * 相关文件    : 
 * 文件实现功能: 本模块的公共定义
 * 作者        : 钮月忠㊣
 * 版本        : V1.0 Copyright(C) 1995-2006 KEDACOM, All rights reserved.
 * -----------------------------------------------------------------------------
 * 修改记录:
 * 日  期       版本        修改人      修改内容
 * 2006/04/25   1.0a        钮月忠      创建	
*******************************************************************************/
#ifndef TIVIDDRVDEF_H
#define TIVIDDRVDEF_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "algorithmType.h"

#if !defined(s32)
    #define s32 int
#endif

/*消息打印*/
#define MESSAGE   		printf

/*挂起*/
#define HANG()          do {MESSAGE("hang ... \n"); while (1) {volatile char x = 0;}} while (0)

/*安全地释放内存*/
#define SAFE_FREE(p)    do {if (p) {free(p); p=NULL;}} while (0)

/*断言*/
#define ASSERT(c)       do {assert(c);} while (0)

/*条件检查*/
#define C(c, n)         do {if (!(c)) {nRet=(n); goto finish;}} while (0)

/*错误返回*/
#define R(n)            do {nRet=(n); goto finish;} while (0)

#endif /*#ifndef TIVIDDRVDEF_H*/
