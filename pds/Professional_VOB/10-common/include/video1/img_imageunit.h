/***************************************************************************************
文件名	    ：img_imageunit.h
相关文件	：img_imageunit.c
文件实现功能：图像处理对外提供界函数接口
作者		：邹文艺
版本		：1.0
-------------------------------------------------------------------------------
日  期		版本		修改人		走读人    修改内容
02/01/2007	1.0         邹文艺                 创建
***************************************************************************************/

#ifndef  _IMG_IMAGEUNIT_H_
#define  _IMG_IMAGEUNIT_H_

#include "img_imagelib.h"

#ifdef __cplusplus
extern "C" {
#endif

/*====================================================================
函数名		: ImageUnitGetVersion
功能		: 图像处理接口版本信息
算法实现	: 无
引用全局变量: 无
参数        : pvVer: 输出缓存[out]
			  l32BufLen: 缓存长度[in]
			  pl32VerLen: 信息长度[in/out]
			  u32ImageProcessType: 图像处理类型[in]
返回值说明  : 成功: VIDEO_SUCCESS, 失败: 返回错误码
====================================================================*/
l32 ImageUnitGetVersion(void *pvVer, l32 l32BufLen, l32 *pl32VerLen, u32 u32ImageProcessType);

/*====================================================================
函数名		: ImageUnitOpen
功能		: 图像处理初始化
算法实现	: 无
引用全局变量: 无
参数        : ppvHandle: 指向图像处理句柄指针[in/out]
              pvImageParam: 指向指定图像处理类型参数设置结构体指针[in]
			  pvEncMemParam: 指向内存参数结构体指针[in/out]
			  u32ImageProcessType: 图像处理类型[in]
返回值说明  : 成功: VIDEO_SUCCESS, 失败: 返回错误码
====================================================================*/
l32 ImageUnitOpen(void **ppvHandle, void *pvImageParam, void *pvMemParam, u32 u32IamgeProcessType);

/*====================================================================
函数名		: ImageUnitProcess
功能		: 图像处理
算法实现	: 无
引用全局变量: 无
参数        : pvHandle: 指向图像处理句柄指针[in]
              pvImageInput: 指向指定图像处理类型输入参数结构体指针[in]
              ptEncOutput: 指向指定图像处理类型输出参数结构体指针[out]
返回值说明  : 成功: VIDEO_SUCCESS, 失败: 返回错误码
====================================================================*/
l32 ImageUnitProcess(void *pvHandle, void *pvImageInput, void *pvImageOutput);

/*====================================================================
函数名		: ImageUnitSetParam
功能		: 改变指定图像处理类型参数设置
算法实现	: 无
引用全局变量: 无
参数        : pvHandle: 指向指定图像处理类型图像处理句柄指针[in/out]
			  pvImageParam: 指向指定图像类型图像处理初始化参数结构体指针[in]
返回值说明  : 成功: VIDEO_SUCCESS, 失败: 返回错误码
====================================================================*/
l32 ImageUnitSetParam(void *pvHandle, void *pvImageParam);

/*====================================================================
函数名		: ImageUnitDebug
功能		: 图像处理调试信息
算法实现	: 无
引用全局变量: 无
参数        : pvHandle:	指向图像处理句柄指针[in]
              pvDebugInfo : 指向调试信息指针[in/out]
返回值说明  : 成功: VIDEO_SUCCESS, 失败: 返回错误码
====================================================================*/
l32 ImageUnitDebug(void *pvHandle, void *pvDebugInfo);

/*====================================================================
函数名		: ImageUnitAnalyzeErrorCode
功能		: 图像处理错误码分析
算法实现	: 无
引用全局变量: 无
参数        : pvHandle: 指向图像处理句柄指针[in]
              l32ErrorCode:	图像处理返回的错误码[in]
			  l32BufLen: 错误码信息的缓冲长度[in]
              pvErrorCodeInfo : 错误码码信息[in/out]
返回值说明  : 成功: VIDEO_SUCCESS, 失败: 返回错误码
====================================================================*/
l32 ImageUnitAnalyzeErrorCode(void *pvHandle, l32 l32ErrorCode, l32 l32BufLen, void *pvErrorCodeInfo);

/*====================================================================
函数名		: ImageUnitClose
功能		: 关闭指定图像类型图像处理句柄
算法实现	: 无
引用全局变量: 无
输入参数说明: pvHandle: 指向指定类型图像处理句柄指针[in]
输出参数说明: 无
返回值说明  : 成功: VIDEO_SUCCESS, 失败: 返回错误码
====================================================================*/
l32 ImageUnitClose(void *pvHandle);

/*====================================================================
函数名		: ImageUnitGetStatus
功能		: 获取指定图像处理类型参数状态
算法实现	: 无
引用全局变量: 无
参数        : pvHandle: 指向指定图像处理类型图像处理句柄指针[in]
			  pvImageParam: 指向指定图像类型图像处理参数状态结构体指针[in/out]
返回值说明  : 成功: VIDEO_SUCCESS, 失败: 返回错误码
====================================================================*/
l32 ImageUnitGetStatus(void *pvHandle, void *pvImageParam);

#ifdef __cplusplus
}
#endif

#endif // _IMG_IMAGEUNIT_H_
