/***************************************************************************************
文件名	    ： TIimageprocess.h
相关文件	：
文件实现功能：提供图像处理的对外接口.

作者		：邹文艺
版本		：4.0
-------------------------------------------------------------------------------
日  期		版本		修改人		走读人    修改内容
04/29/2006	4.0         邹文艺                 创建
***************************************************************************************/
#ifndef  _TIIMAGEPROCESS_H_
#define  _TIIMAGEPROCESS_H_

#include "algorithmtype.h"
#include "videocommondefines.h"

#ifdef __cplusplus
extern "C"
{
#endif 
	

/*====================================================================
函数名      ： YUV422PlaneToYUV420AndTFInit_c
功能        ： 图像YUV422转化为YUV420并进行时域滤波的初始化
算法实现    ：(可选项)
引用全局变量：无 
输入参数说明：void **ppvHandle   : 指向图像前处理并进行时域滤波的句柄的指针
              l32 l32SrcWidth    : 源图像的宽度
			  l32 l32SrcHeight   : 源图像的高度
			  l32 l32SrcStride   : 源图像的步长
			  l32 l32DstWidth    : 目标图像的宽度
			  l32 l32DstHeight   : 目标图像的高度
			  l32 l32DstStride   : 目标图像的步长
             
返回值说明  ：成功返回VIDEOSUCCESSFUL, 错误返回 VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
日  期        版本        修改人        修改内容
2006/04/29    1.0        ZouWenYi        创建
====================================================================*/
l32 YUV422PlaneToYUV420AndTFInit_c(void **ppvHandle, l32 l32SrcWidth, l32 l32SrcHeight, l32 l32SrcStride,
					   l32 l32DstWidth, l32 l32DstHeight, l32 l32DstStride);



/*====================================================================
函数名      ： YUV422PlaneToYUV420AndTF_c
功能        ：图像YUV422转化为YUV420并进行时域滤波的处理
算法实现    ：(可选项)
引用全局变量：无 
输入参数说明：void *pvHandle   : 指向图像前处理句柄的指针
              u8   *pu8Src     : 指向源图像数据的指针
			  u8   *pu8Dst     : 指向目标图像数据的指针

返回值说明  ：成功返回VIDEOSUCCESSFUL, 失败返回VIDEOFAILURE;
举例或特别说明：只支持分辨率720*576的YUV422转到分辨率720*576,352*576, 352*288的YUV420图像
----------------------------------------------------------------------
修改记录    ：
日  期        版本        修改人        修改内容
2006/04/29    1.0        ZouWenYi         创建
====================================================================*/
l32 YUV422PlaneToYUV420AndTF_c(void *pvHandle, u8 *pu8Src, u8 *pu8Dst);

/*====================================================================
函数名      ： YUV422PlaneToYUV420andTFClose_c
功能        ：释放前处理的句柄
算法实现    ：(可选项)
引用全局变量：无 
输入参数说明：void *pvHandle  : 图像YUV422转为YUV420并进行时域滤波的句柄

返回值说明  ：成功返回 VIDEOSUCCESS, 错误返回VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
日  期        版本        修改人        修改内容
2006/04/29    1.0        ZouWenYi         创建
====================================================================*/	
l32 YUV422PlaneToYUV420andTFClose_c(void *pvHandle);

/*====================================================================
函数名      ： YUV422PlaneToYUV420SetParamAndTF_c
功能        ：图像YUV422转到YUV420并时域滤波的参数设置
算法实现    ：(可选项)
引用全局变量：无 
输入参数说明：void *pvHandle   : 指向图像前处理并时域滤波的句柄的指针
              l32 l32SrcWidth    : 源图像的宽度
			  l32 l32SrcHeight   : 源图像的高度
			  l32 l32SrcStride   : 源图像的步长
			  l32 l32DstWidth    : 目标图像的宽度
			  l32 l32DstHeight   : 目标图像的高度
			  l32 l32DstStride   : 目标图像的步长
             
返回值说明  ：成功返回 VIDEOSUCCESS, 错误返回VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
日  期        版本        修改人        修改内容
2006/04/29    1.0        ZouWenYi        创建
====================================================================*/
l32 YUV422PlaneToYUV420SetParamAndTF_c(void *pvHandle, l32 l32SrcWidth, l32 l32SrcHeight, l32 l32SrcStride,
					   l32 l32DstWidth, l32 l32DstHeight, l32 l32DstStride);

#define  YUV422PlaneToYUV420AndTFInit      YUV422PlaneToYUV420AndTFInit_c
#define  YUV422PlaneToYUV420AndTF          YUV422PlaneToYUV420AndTF_c
#define  YUV422PlaneToYUV420andTFClose     YUV422PlaneToYUV420andTFClose_c
#define  YUV422PlaneToYUV420SetParamAndTF  YUV422PlaneToYUV420SetParamAndTF_c


#ifdef __cplusplus
};
#endif 

#endif // _TIIMAGEPROCESS_H_


