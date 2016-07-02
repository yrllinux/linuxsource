/*******************************************************************************
 *  模块名   : MT                                                              *
 *  文件名   : mtmacro.h                                                       *
 *  相关文件 :                                                                 *
 *  实现功能 : 终端常用宏定义                                                  *
 *  作者     : 张明义                                                          *
 *  版本     : V3.0  Copyright(c) 1997-2005 KDC, All rights reserved.          *
 *                                                                             *
 *    =======================================================================  *
 *  修改记录:                                                                  *
 *    日  期      版本        修改人      修改内容                             *
 *  2005/8/6  x.0         张明义      创建          *
 *                                                                             *
 *******************************************************************************/

#ifndef _MT_MACRO_H_
#define _MT_MACRO_H_

#include "mtconst.h"
#include <string.h>


#ifdef _MSC_VER
#define inline _inline
#endif

#define MT_OK 0
#define MT_ERR 0xFF

#define IN_PARAM
#define OUT_PARAM
	
#define  _OffSetOf(s,m)   (size_t)&(((s *)0)->m)
#define  _CountOf(array)  (sizeof(array)/sizeof(array[0]))
#define  _SizeOfMember( s ,m ) (size_t)(sizeof((((s *)0)->m)))

#define HEXCAR(  val ) ( ( (u8)(val) >= 0xA ) ? ('A'+ (u8)(val)-0xA) : ('0' + (u8)(val) )  )

// 指针判断
#define MT_ASSERT( p ) if ( p == NULL ) { ::OspPrintf( TRUE, FALSE, "[SYSTEM]Invalid Pointer(%s:(%d))!\n", __FILE__, __LINE__ ); return; }
#define MT_ASSERT_RET( p, ret ) if ( p == NULL ) { ::OspPrintf( TRUE, FALSE, "[SYSTEM]Invalid Pointer(%s:(%d))!\n", __FILE__, __LINE__ ); return ret; }

// 释放内存
#define MT_SAFE_DELETE( p ) if ( p != NULL ) { delete p; p = NULL; }
#define MT_SAFE_DELETE_ARRAY( p ) if ( p != NULL ) { delete []p; p = NULL; }


#define IPFORMAT     "%d.%d.%d.%d"
#define u32ToIP( ip )   (u8)(ntohl(ip)>>24),\
                        (u8)(ntohl(ip)>>16),\
                        (u8)(ntohl(ip)>>8), \
                        (u8)ntohl(ip) 
//判断是否为组播地址
#define IS_MCADDR( ip ) ( (ntohl(ip) &0xE0000000) == 0xE0000000 )

#define SEAL_ARRAY( array ,val) (array[sizeof(array)-1]=val)

//安全复制字符串，旨在防止目标数组溢出。
//dst: 目标字符数组地址
//src: 源字符串指针
#define SAFE_STRCPY( dst, src)\
{\
	strncpy(dst, src, sizeof(dst));\
	dst[sizeof(dst)-1] = '\0';\
}

//////////////////////////////////////////////////////////////////////////


#ifdef _LINUX_

#ifndef min
#define min(a,b) ((a)>(b)?(b):(a))
#endif
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

#endif

#ifdef _LINUX_
#define mtRegCommand( fun ) OspRegCommand (#fun, (void*) fun, "")
#else
#define mtRegCommand( fun ) 
#endif

#ifndef ABS
#define ABS(x) ((x <0) ? -x: x )
#endif

//#ifdef _LINUX_
//	#ifndef _LINUX_CONFIG_DEBUG_
//	#define _LINUX_CONFIG_DEBUG_
//	#endif
// #else
	#ifdef _LINUX_CONFIG_DEBUG_
	#undef _LINUX_CONFIG_DEBUG_
	#endif
// #endif


#define CHECK_POINTER(module, p)  \
{                  \
	if (p == NULL) \
	{              \
		MtLogErr(module, "NULL Pointer, in %s,%d", __FILE__, __LINE__); \
		return MT_ERR; \
	}\
}

#define VOID_CHECK_POINTER(module, p)  \
{                  \
	if (p == NULL) \
	{              \
		MtLogErr(module, "NULL Pointer, in %s,%d", __FILE__, __LINE__); \
		return; \
	}\
}

#ifdef _LINUX_
#define _MT_STRICMP( str1, str2 ) strcasecmp( str1, str2 )
#else
#define _MT_STRICMP( str1, str2 ) stricmp( str1, str2 )
#endif

#define BOOL_DESCRIPTION( val )  ( (val) ? "TRUE" : "FALSE" )

// --------------- 高清图像参数配置相关宏 --------------- 
#define MT_HD_IMAGE_PARAM_COUNT         3      // 配置总数
#define MT_HD_IMAGE_PARAM_CVBS      (u8)0      // CVBS的图像参数配置索引
#define MT_HD_IMAGE_PARAM_YPbPr0    (u8)1      // 分量1的图像参数配置索引
#define MT_HD_IMAGE_PARAM_YPbPr1    (u8)2      // 分量2的图像参数配置索引
#define MT_HD_IMAGE_PARAM_INVALID   (u8)0xFF   // 无效索引
#define GetHdParamImageIndex( emMtVideoPort )\
( emMtVideoPort == emMtC3Vid ? MT_HD_IMAGE_PARAM_YPbPr0 : \
( emMtVideoPort == emMtC4Vid ? MT_HD_IMAGE_PARAM_YPbPr1 : \
( emMtVideoPort == emMtC5Vid ? MT_HD_IMAGE_PARAM_CVBS : MT_HD_IMAGE_PARAM_INVALID ) ) )

#define MT_AACLC_IDLE_CHAN_TYPE     (u8)1


// --------------- 组合键相关宏定义 --------------- 
#define MT_COMBINED_HELP_RED      0   // HELP(3s) + Red
#define MT_COMBINED_HELP_YLW      1   // HELP(3s) + YLW
#define MT_COMBINED_HELP_GRN      2   // HELP(3s) + GRN

// --------------- 网口模式宏定义 --------------- 
#define ETH_MODE_AUTO           (u32)0
#define ETH_MODE_10M_FULL       (u32)1
#define ETH_MODE_10M_HALF       (u32)2
#define ETH_MODE_100M_FULL      (u32)3
#define ETH_MODE_100M_HALF      (u32)4

// --------------- 高清制式宏定义 --------------- 
#define MT_HD_IMAGE_AUTOADJUST           (u32)0
#define MT_HD_IMAGE_ORIGINSIZE           (u32)1

// --------------- 图像参数调整的操作类型 ------------------
#define MT_SCALE_BRIGHTNESS     (u32)0 /* 设置亮度，  C | YPbPr */
#define MT_SCALE_CONTRAST       (u32)1 /* 设置对比度，C | YPbPr */
#define MT_SCALE_HUE            (u32)2 /* 设置色度，  C */
#define MT_SCALE_SATURATION     (u32)3 /* 设置饱和度，C | YPbPr */

// --------------- 远端近端操作类型 --------------- 
#define MT_DIALOG_PA           (u32)0 /*参数调节*/
#define MT_DIALOG_MC           (u32)1 /*移动摄像头*/
#define MT_DIALOG_CP           (u32)2 /*调整摄像头*/

// --------------- PCMT视频编解码方式设置 --------------- 
#define PCMT_VCODEC_SOFTWARE           (u8)0 /*软件编解码*/
#define PCMT_VCODEC_HARDWARE           (u8)1 /*硬件显卡编解码*/
#define PCMT_VENCODE                   (u8)0 /*编码*/
#define PCMT_VDECODE                   (u8)1 /*解码*/

// --------------- 高清1080i--帧率类型 --------------- 
#define MT_HD_1080i_50                   (u32)0
#define MT_HD_1080i_60                   (u32)1
#define MT_HD_1080i_AUTO                 (u32)2

// --------------- 高清1080P--帧率类型 --------------- 
#define MT_HD_1080p_24                   (u32)0
#define MT_HD_1080p_25                   (u32)1
#define MT_HD_1080p_30                   (u32)2
#define MT_HD_1080p_AUTO                 (u32)3
#define MT_HD_1080p_1080i                (u32)4

// --------------- VGA--频率类型 --------------- 
#define MT_HD_VGA_60                     (u32)0
#define MT_HD_VGA_75                     (u32)1

#endif//!_MT_MACRO_H_

