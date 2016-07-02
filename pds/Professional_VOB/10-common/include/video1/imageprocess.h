/***************************************************************************************
文件名	    ： ImageProcess.h
相关文件	：
文件实现功能：提供图像处理的对外接口.

作者		：邹文艺
版本		：2005.03.10
-------------------------------------------------------------------------------
日  期		版本		修改人		走读人    修改内容
03/10/2005	1.0         邹文艺                 创建
06/20/2005	1.1	 	zhaobo							添加Linux平台下相关内容
***************************************************************************************/
#ifndef  _IMAGEPROCESS_H_
#define  _IMAGEPROCESS_H_

#include "algorithmtype.h"
#include "videocommondefines.h"

#ifdef __cplusplus
extern "C"
{
#endif 

#define LOGOADAPTIVECOLOR 0x01


//图像信息的结构体
typedef struct 
{
	u8   *pu8Y;           //图像的Y分量
	u8   *pu8U;           //图像的U分量
	u8   *pu8V;           //图像的V分量(U、V分量可以不赋值，直接赋以NULL，但注意这两者必须同时填或不填)
	l32  l32Width;        //图像的宽度
	l32  l32Height;       //图像的高度
	l32  l32YStride;      //图像的Y分量步长
	l32  l32UVStride;     //图像的UV分量步长
	l32	 l32ImageType;    //图像的类型(帧格式或者场格式)
	l32  l32YUVType;      //图标图像格式(YUV422或者YUV420)

}TImageInfo;

//图标中字的信息结构体
typedef struct  
{
	l32 l32FontHeight;  //字的高度
	l32 l32WidthBufLen; //记录字的宽度的缓冲大小
	u8 *pu8FontWidth;   //每个字的宽度,从左到右,从上到下的顺序
}TFontInfo;

//图标信息的结构体
typedef struct 
{
	u8   *pu8YUV;               //前景图象数据指针地址(图像格式为YUV422时,
	                            //分配的空间大小为l32Width*l32Height*2,图像格式为YUV420时,分配的空间大小为l32Width*l32Height*3/2)
	u8   *pu8Alpha;             //前景图像的alpha通道指针(分配空间的大小和分配图像数据的大小一样)
	u8   *pu8Beta;              //对应背景图像的beta通道指针(分配空间的大小和分配图像数据的大小一样)
	u8   u8FgTransparency;      //前景图象透明系数
	u8   u8BgTransparency;      //背景图像透明系数
 	l32  l32Width;              //前景图像的宽度
	l32  l32Height;             //前景图像的高度	
	l32  l32YUVType;            //图标图像格式(YUV422或者YUV420)
	l32  l32Top,l32Left;        //前景图象位置偏移
	l32  l32TopCutLine;         //前景图像上边裁去的行数
	l32  l32BottomCutLine;      //前景图像下边裁去的行数
	l32  l32LeftCutLine;        //前景图像左边裁去的列数
	l32  l32RightCutLine;       //前景图像右边裁去的列数
	l32  l32ImageType;          //前景图像的类型(帧格式或者场格式)
	l32  l32LogoRefPicWidth;    //图标生成的参考图象高度
	l32  l32LogoRefPicHeight;   //图标生成的参考图象宽度
    l32  l32IsAdaptiveColor;    //是否根据底色改变图标的颜色,1表示需要改变图标的颜色,其他表示不需要改变图标的颜色
    TFontInfo tFontInfo;        //记录图标中字的信息结构体
} TLogoInfo;

//YUV422转为YUV420并进行时域滤波的参数设置
typedef struct 
{
    l32 l32SrcWidth;    //YUV422图像的宽度
    l32 l32SrcHeight;   //YUV422图像的高度
    l32 l32SrcStride;   //YUV422图像的步长
    l32 l32DstWidth;    //YUV420图像的宽度
    l32 l32DstHeight;   //YUV420图像的高度
    l32 l32DstYStride;  //YUV420Y分量的步长
    l32 l32FilterType;  //滤波的类型，根据不同的编码器选择不同滤波方式，参数设置的数值
} TYUV422ToYUV420Param;

//Map平台、Windows平台和Linux平台下共用的函数声明

/*===========================================================================
函数名	    : GetImageProcessVersion;
功能		：返回版本信息
算法实现	：
参数说明    ：
版本信息的指针 : void *pVer
               l32 l32BufLen  未用
			   l32 *pl32VerLen  版本信息的长度
返回值说明  ：无
其他说明    ：
=============================================================================*/
void  GetImageProcessVersion(void *pVer, l32 l32BufLen, l32 *pl32VerLen);

/*====================================================================
函数名      ：ConvertBMPToYUVAndAlpha
功能        ：将图像RGB值变为YUV值，同时生成前景图像的alpha通道
算法实现    ：(可选项)
引用全局变量：无
输入参数说明：
	          u8  *pu8Bmp         RGB矩阵的指针
              TLogoInfo *ptLogo   包含有前景图像信息的结构的指针
			  u8  u8BackR         背景颜色的R分量
			  u8  u8BackG         背景颜色的G分量
			  u8  u8BackB         背景颜色的B分量
			  u8  u8FgTransparency  前景透明度(255表示不透明,0表示全透明,1~254表示部分透明)
返回值说明  ：成功,VIDEOSUCCESS ;失败，VIDEOFAILURE
特别说明 : 读取BMP图像的数据时, 为了数据的对齐, 要在BMP图像的文件头前面
           空出2个byte; 即puBmp指针指向的前2个byte为空,然后再读取实际的BMP图像
		   的数据
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
03/11/24    1.0         HuangKui		建立
05/04/07    1.1         ZouWenyi      代码规范化
====================================================================*/
l32 ConvertBMPToYUVAndAlpha(u8 *pu8Bmp, TLogoInfo *ptLogo, u8 u8BackR, u8 u8BackG, u8 u8BackB, u8 u8FgTransparency);

/*====================================================================
函数名      ： CalculateSum88
功能    	： 对Y计算8*8块数的和,对UV计算4*4块数的和
算法实现    ：
引用全局变量：无
输入参数说明：
              u8  *pu8YUV : 指向yuv图像数据的指针
			  l32  l32Height : 图像的高度
			  l32  l32Width  : 图像的宽度
			  l32  l32YStride : Y分量的步长
			  l32  l32UVStride : UV分量的步长
			  u16 *pu16Sum : 保存计算的结果
              
返回值说明  ：成功返回VIDEOSUCCESS; 失败返回VIDEOFAILURE
----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  05/03/07             ZouWenyi	        创建
====================================================================*/
l32 CalculateSum88(u8 *pu8YUV, l32 l32Height, l32 l32Width, l32 l32YStride, l32 l32UVStride, u16 *pu16Sum);

/*====================================================================
函数名      ：FrameGenerator420
功能        ：生成一帧图像（格式为YUV420）
算法实现    ：(可选项)
引用全局变量：无 
输入参数说明：TImageInfo *ptImage : 指向生成图像的结构体指针
              l32    l32ImageForm : 生成图像的形状(图案的选择如下：
                   GLOBOSITY表示地球形状，BLACKWHITEBAR表示不同间隔黑白条 ，
                   WHITETOBLACK表示由白逐渐加到黑，RAINBOW表示七彩色)

返回值说明  ：成功返回VIDEOSUCCESS; 失败返回VIDEOFAILURE
特殊说明 : 目前只支持生成720*576的420场格式图像
----------------------------------------------------------------------
修改记录    ：
日  期        版本        修改人        修改内容
05/11/2004    1.0        ZouWenYi         建立    
05/18/2005    1.1        ZouWenyi       代码规范化
====================================================================*/	
l32 FrameGenerator420(TImageInfo *ptImage, l32 l32ImageForm);

/*====================================================================
函数名      :  copy_image
功能	    :  拷贝一帧图像
算法实现    ;  适用DS和CPU同时搬运数据(只提供MAP版本)
引用全局变量： 无
输入参数说明： 	u8 *pu8Dst, 目的地址
				u8 *pu8Src, 源地址
				l32 l32Width, 宽度
				l32 l32Height 高度
返回值说明  ： 无
----------------------------------------------------------------------
  修改记录    ：
  日  期		   版本		修改人		走读人      修改内容
  07/07/2006       1.0      黄奎                    创建 
====================================================================*/
void copy_image(u8 *pu8Dst, u8 *pu8Src, l32 l32Width, l32 l32Height);

/*====================================================================
函数名      :  GenerateYUVtoRGBTables
功能	    :  初始化YUV到RGB装换得色彩表格
算法实现    ;  
引用全局变量： 无
输入参数说明： 无
返回值说明  ： 无
----------------------------------------------------------------------
  修改记录    ：
  日  期		   版本		修改人		走读人      修改内容
  07/10/2006       1.0       赵波                     创建 
====================================================================*/
void GenerateYUVtoRGBTables(void);


#if defined MAP_CODE  //MAP 代码声明
/*====================================================================
函数名      :	D1YUV422FieldPToN_map
功能        :	将4cif的场格式YUV422图像由P制转变为N制
引用全局变量:	无 
算法实现    :	把加的黑边裁去
输入参数说明:	TImageInfo *ptInImage  : 输入图像
				TImageInfo *ptOutImage : 输出图像
返回值说明  :	成功返回 VIDEOSUCCESSFUL，失败返回 VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
06/09/29    1.0         zhaobo			创建
====================================================================*/
l32 D1YUV422FieldPToN_map(TImageInfo *ptInImage, TImageInfo *ptOutImage);

#define  D1YUV422FieldPToN  D1YUV422FieldPToN_map

/*====================================================================
函数名      :	D1YUV422FieldNToP_map
功能        :	将4cif的场格式YUV422图像由N制转变为P制
引用全局变量:	无 
算法实现    :	上下加黑边
输入参数说明:	TImageInfo *ptInImage  : 输入图像
				TImageInfo *ptOutImage : 输出图像
返回值说明  :	成功返回 VIDEOSUCCESSFUL，失败返回 VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
06/09/29    1.0         zhaobo			创建
====================================================================*/
l32 D1YUV422FieldNToP_map(TImageInfo *ptInImage, TImageInfo *ptOutImage);

#define  D1YUV422FieldNToP  D1YUV422FieldNToP_map

/*====================================================================
函数名      :	D1YUV422FieldNToPOneBuffer_map
功能        :	将4cif的场格式YUV422图像由N制转变为P制,操作在同一块buffer
				进行
引用全局变量:	无 
算法实现    :	上下加黑边
输入参数说明:	TImageInfo *ptInImage  : 输入图像
				TImageInfo *ptOutImage : 输出图像
返回值说明  :	成功返回 VIDEOSUCCESSFUL，失败返回 VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
06/10/10    1.0         zhaobo			创建
====================================================================*/
l32 D1YUV422FieldNToPOneBuffer_map(TImageInfo *ptInImage, TImageInfo *ptOutImage);

#define D1YUV422FieldNToPOneBuffer D1YUV422FieldNToPOneBuffer_map

/*====================================================================
函数名      :	D1YUV420FieldPToNOneBuffer_map
功能        :	将4cif的场格式YUV420图像由P制转变为N制, 在一块buffer
				进行
引用全局变量:	无 
算法实现    :	把加的黑边裁去
输入参数说明:	TImageInfo *ptInImage  : 输入图像
				TImageInfo *ptOutImage : 输出图像
返回值说明  :	成功返回 VIDEOSUCCESSFUL，失败返回 VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
06/10/10    1.0         zhaobo			创建
====================================================================*/
l32 D1YUV420FieldPToNOneBuffer_map(TImageInfo *ptInImage, TImageInfo *ptOutImage);

#define D1YUV420FieldPToNOneBuffer D1YUV420FieldPToNOneBuffer_map

/*====================================================================
函数名      :	D1YUV420FieldNToPTwoBuffer_map
功能        :	将4cif的场格式YUV420图像由N制转变为P制
				进行
引用全局变量:	无 
算法实现    :	加黑边
输入参数说明:	TImageInfo *ptInImage  : 输入图像
				TImageInfo *ptOutImage : 输出图像
返回值说明  :	成功返回 VIDEOSUCCESSFUL，失败返回 VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
10/27/06    1.0         zhaobo			创建
====================================================================*/
l32 D1YUV420FieldNToPTwoBuffer_map(TImageInfo *ptInImage, TImageInfo *ptOutImage);

#define D1YUV420FieldNToPTwoBuffer D1YUV420FieldNToPTwoBuffer_map
/*====================================================================
函数名      ：ProduceMosaic
功能        ：对图像加马赛克
引用全局变量：无 
算法实现    ：(可选项)
输入参数说明：  TImageInfo *ptInImage  : 输入图像
				TImageInfo *ptOutImage : 输出图像
                l32 l32Left            : 马赛克区域左坐标
                l32 l32Top             : 马赛克区域上坐标
                l32 l32WindowWidth     : 马赛克区域宽度
                l32 l32WindowHeight    : 马赛克区域高度
                l32 l32Granularity     : 马赛克粒度
返回值说明  ：成功返回 VIDEOSUCCESSFUL，失败返回 VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
03/11/24    1.0         HuangKui		建立
06/06/22    1.5         zhaobo        规范化、修改
06/09/07    2.0         zhaobo        MAP优化
====================================================================*/
l32 ProduceMosaic_map(TImageInfo *ptInImage, TImageInfo *ptOutImage, 
				      l32 l32Left, l32 l32Top, l32 l32WindowWidth, l32 l32WindowHeight, l32 l32Granularity);
#define  ProduceMosaic  ProduceMosaic_map
/*====================================================================
函数名      ：MergePictureInit_map
功能        ：图像合成初始化
引用全局变量：无 
算法实现    ：(可选项)
输入参数说明：void **ppvHandle : 句柄
			  
返回值说明  ：成功返回VIDEOSUCCESS, 失败返回 VIDEOFAILURE
特别说明 :  只支持YUV420的图像合并
----------------------------------------------------------------------
修改记录    ：
日  期      版本         修改人        修改内容
11/24/2003  1.0         HuangKui	     创建
04/26/2005  1.1         ZouWenyi       代码规范化         
08/10/2006  2.0            zb            使用DS  
====================================================================*/
l32 MergePictureInit_map(void **ppvHandle);
#define  MergePictureInit  MergePictureInit_map
/*====================================================================
函数名      ： MergePicture
功能        ：把小图像覆盖到大图像的指定位置
引用全局变量：无 
算法实现    ：(可选项)
输入参数说明：void *pvHandle 句柄
			  TImageInfo *ptBackgroundImage   指向背景图像的指针
              TLogoInfo *ptLogo  指向包含有前景图像信息的结构的指针
			    (小图像的数据放在ptLogo的pu8YUV成员变量,需要设置ptLogo的
				其他成员变量:l32Height(小图像的高度), l32Width(小图像的宽度), 
				l32Left(小图像在大图像中水平方向偏移的大小), l32Top(小图像在
				大图像中垂直方向偏移的大小), l32ImageType(帧格式或者场格式),
				l32TopCutLine(小图像上边裁去的行数),l32BottomCutLine(小图像下
				边裁去的行数), l32LeftCutLine(小图像左边裁去的列数), l32RightCutLine
				(小图像右边裁去的列数))
			  
返回值说明  ：成功返回VIDEOSUCCESS, 失败返回 VIDEOFAILURE
特别说明 :  只支持YUV420的图像合并
----------------------------------------------------------------------
修改记录    ：
日  期      版本         修改人        修改内容
11/24/2003  1.0         HuangKui	     创建
04/26/2005  1.1         ZouWenyi       代码规范化         
08/10/2006  2.0            zb            使用DS  
====================================================================*/
l32 MergePicture_map(void *pvHandle, TImageInfo *ptBackgroundImage, TLogoInfo *ptLogo);

#define  MergePicture  MergePicture_map
/*====================================================================
函数名      ： MergePictureClose
功能        ： 图像合成关闭
引用全局变量：无 
算法实现    ：(可选项)
输入参数说明：void *pvHandle 句柄			  
返回值说明  ：成功返回VIDEOSUCCESS, 失败返回 VIDEOFAILURE
特别说明 :  只支持YUV420的图像合并
----------------------------------------------------------------------
修改记录    ：
日  期      版本         修改人        修改内容
11/24/2003  1.0         HuangKui	     创建
04/26/2005  1.1         ZouWenyi       代码规范化         
08/10/2006  2.0            zb            使用DS  
====================================================================*/
l32 MergePictureClose_map(void *pvHandle);

#define  MergePictureClose  MergePictureClose_map
/*====================================================================
函数名      :	ImageSizeChange_map
功能	    :	采用双线性插值的方法将源图像缩放标图像
算法实现    :	
引用全局变量:	无
输入参数说明:	TImageInfo *ptInImage : 变换前的图像指针
				TImageInfo *ptOutImage : 变换后的图像指针
返回值说明  :	无
----------------------------------------------------------------------
修改记录:    
日  期		   版本		修改人		走读人      修改内容
03/29/2006      1.0      赵波                    创建 
====================================================================*/
l32 ImageSizeChange_map(TImageInfo *ptInImage, TImageInfo *ptOutImage);

#define  ImageSizeChange  ImageSizeChange_map

/*====================================================================
函数名      ： DrawBanner_map
功能        ：在图像中添加横幅(填充矩形块区域为指定的颜色)
算法实现    ：(可选项)
引用全局变量：无 
输入参数说明：TImageInfo  *ptImage   : 指向图像结构体的指针
		      TBanner *ptBanner      : 指向横幅结构的指针

返回值说明  ：成功返回VIDEOSUCCESSFUL, 失败返回 VIDEOFAILURE  
特别说明 : 图像的数据指向结图像构体中的Y分量            
----------------------------------------------------------------------
修改记录    ：
日  期        版本        修改人        修改内容
01/16/2006    1.0        ZouWenyi        创建
====================================================================*/
l32 DrawBanner_map(TImageInfo *ptImage, TBanner *ptBanner);

#define  DrawBanner DrawBanner_map


/*====================================================================
函数名      ：  YUV422ToYUV420AndTemporalFilterInit_map
功能        ：图像YUV422转化为YUV420并进行时域滤波的初始化
算法实现    ：(可选项)
引用全局变量：无 
输入参数说明：void **ppvHandle    : 指向图像前处理句柄的指针
              ptYUV422To420Param  : YUV422转为YUV420图像的参数设置
			               
返回值说明  ：成功返回VIDEOSUCCESSFUL, 错误返回 VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
  日  期	  版 本		 修改人		走读人      修改内容
2006/04/13	   1.0       邹文艺                   创建
====================================================================*/
l32  YUV422ToYUV420AndTemporalFilterInit_map(void **ppvHandle, TYUV422ToYUV420Param *ptYUV422To420Param);


#define  YUV422ToYUV420AndTemporalFilterInit YUV422ToYUV420AndTemporalFilterInit_map

/*====================================================================
函数名      ： YUV422ToYUV420AndTemporalFilter_map
功能        ：图像YUV422转化为YUV420的处理
算法实现    ：(可选项)
引用全局变量：无 
输入参数说明：void *pvHandle   : 指向图像前处理句柄的指针
              u8   *pu8Src     : 指向源图像数据的指针
			  u8   *pu8Dst     : 指向目标图像数据的指针
			  
返回值说明  ：成功返回 VIDEOSUCCESSFUL, 错误返回VIDEOFAILURE
举例或特别说明：只支持分辨率720*576的YUV422转到分辨率720*576,352*576, 352*288的YUV420图像
----------------------------------------------------------------------
修改记录    ：
   日  期	  版 本		 修改人		走读人      修改内容
2006/04/14	   1.0       邹文艺                   创建
====================================================================*/
l32 YUV422ToYUV420AndTemporalFilter_map(void *pvHandle, u8 *pu8Src, u8 *pu8Dst);

#define  YUV422ToYUV420AndTemporalFilter  YUV422ToYUV420AndTemporalFilter_map

/*====================================================================
函数名      ： YUV422ToYUV420AndTemporalFilterClose_map
功能        ：释放前处理的句柄和DS
算法实现    ：(可选项)
引用全局变量：无 
输入参数说明：void *pvHandle  : 图像YUV422转为YUV420的句柄
             
返回值说明  ：成功返回 VIDEOSUCCESS, 错误返回VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
  日  期	  版 本		 修改人		走读人      修改内容
2006/04/13	   1.0       邹文艺                   创建
====================================================================*/	
l32 YUV422ToYUV420AndTemporalFilterClose_map(void *pvHandle);

#define  YUV422ToYUV420AndTemporalFilterClose YUV422ToYUV420AndTemporalFilterClose_map


/*====================================================================
函数名      ： YUV422ToYUV420AndTemporalFilterSetParam_map
功能        ：图像前处理的参数设置
算法实现    ：(可选项)
引用全局变量：无 
输入参数说明：void *pvHandle      : 指向图像前处理句柄的指针
              ptYUV422To420Param  : YUV422转为YUV420图像的参数设置
             
返回值说明  ：成功返回 VIDEOSUCCESS, 错误返回VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
  日  期	  版 本		 修改人		走读人      修改内容
2006/04/13	   1.0       邹文艺                   创建
====================================================================*/
l32 YUV422ToYUV420AndTemporalFilterSetParam_map(void *pvHandle, TYUV422ToYUV420Param *ptYUV422To420Param);

#define  YUV422ToYUV420AndTemporalFilterSetParam YUV422ToYUV420AndTemporalFilterSetParam_map

/*====================================================================
函数名      ： YUV422ToYUV420Init_map
功能        ：图像YUV422转化为YUV420的初始化
算法实现    ：(可选项)
引用全局变量：无 
输入参数说明：void **ppvHandle   : 指向图像前处理句柄的指针
              l32 l32SrcWidth    : 源图像的宽度
			  l32 l32SrcHeight   : 源图像的高度
			  l32 l32SrcStride   : 源图像的步长
			  l32 l32DstWidth    : 目标图像的宽度
			  l32 l32DstHeight   : 目标图像的高度
			  l32 l32DstStride   : 目标图像的步长
             
返回值说明  ：成功返回VIDEOSUCCESS, 错误返回 VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
日  期        版本        修改人        修改内容
2005/04/13    1.0        ZouWenYi        创建
====================================================================*/
l32 YUV422ToYUV420Init_map(void **ppvHandle, l32 l32SrcWidth, l32 l32SrcHeight, l32 l32SrcStride,
				   l32 l32DstWidth, l32 l32DstHeight, l32 l32DstStride);

/*====================================================================
函数名      ： YUV422ToYUV420_map
功能        ：图像YUV422转化为YUV420的处理
算法实现    ：(可选项)
引用全局变量：无 
输入参数说明：void *pvHandle   : 指向图像前处理句柄的指针
              u8   *pu8Src     : 指向源图像数据的指针
			  u8   *pu8Dst     : 指向目标图像数据的指针
			  u16  *pu16Sum8x8 : 指向Y分量8*8块的数据之和,UV分量4*4块数据之和的指针,
			                     当该指针为空时,表示不计算8*8块的数据之和			  
返回值说明  ：成功返回VIDEOSUCCESS, 错误返回 VIDEOFAILURE
举例或特别说明：只支持分辨率720*576的YUV422转到分辨率720*576,352*576, 352*288的YUV420图像
----------------------------------------------------------------------
修改记录    ：
日  期        版本        修改人        修改内容
2005/04/13    1.0        ZouWenYi         创建
====================================================================*/
l32 YUV422ToYUV420_map(void *pvHandle, u8 *pu8Src, u8 *pu8Dst, u16 *pu16Sum8x8);

/*====================================================================
函数名      ： YUV422ToYUV420Close_map
功能        ：释放前处理的句柄和DS
算法实现    ：(可选项)
引用全局变量：无 
输入参数说明：void *pvHandle  : 图像前处理的句柄
返回值说明  ：成功返回VIDEOSUCCESS, 错误返回 VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
日  期        版本        修改人        修改内容
2005/04/11    1.0        ZouWenYi      代码规范化
====================================================================*/	
l32 YUV422ToYUV420Close_map(void *pvHandle);

/*====================================================================
函数名      ： YUV422ToYUV420SetParam_map
功能        ：图像前处理的参数设置
算法实现    ：(可选项)
引用全局变量：无 
输入参数说明：void *pvHandle   : 指向图像前处理句柄的指针
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
2005/04/13    1.0        ZouWenYi        创建
====================================================================*/
l32 YUV422ToYUV420SetParam_map(void *pvHandle, l32 l32SrcWidth, l32 l32SrcHeigt, l32 l32SrcStride,
					   l32 l32DstWidth, l32 l32DstHeight, l32 l32DstStride);

//图像前处理
#define  YUV422ToYUV420Init      YUV422ToYUV420Init_map
#define  YUV422ToYUV420          YUV422ToYUV420_map
#define  YUV422ToYUV420Close     YUV422ToYUV420Close_map
#define  YUV422ToYUV420SetParam  YUV422ToYUV420SetParam_map

/*====================================================================
函数名      ：AddLogo_map
功能        ：将Logo加到图像上
引用全局变量：无 
算法实现    ：(可选项)
输入参数说明：TImageInfo *ptBackgroundImage   指向背景图像的指针
              TLogoInfo *ptLogo  指向包含有前景图像信息的结构的指针

返回值说明  ：成功返回VIDEOSUCCESS; 错误返回 VIDEOFAILURE

特别说明 : 图标的宽度为8的倍数,高度为4的倍数,否则返回错误.
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
03/11/24    1.0         HuangKui		建立
05/04/07    1.1         ZouWenyi       代码规范化及算法的改进
====================================================================*/
l32 AddLogo_map(TImageInfo *ptBackgroundImage, TLogoInfo *ptLogo);

//添加图标
#define  AddLogo_map  AddLogo

/*====================================================================
函数名  ： ImageDering_map
功能	: 对图像进行去振铃效应
算法实现    ：(可选项)
引用全局变量：无
输入参数说明： TImageInfo *ptImage   ：Dering图像指针
               l32         l32Quant  : 量化参数(范围2~62)

返回值说明 ：成功返回VIDEOSUCCESS, 失败返回 VIDEOFAILURE
特殊说明   : 图像的宽度必须为8的倍数   
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
05/14/2005             ZouWenyi    代码规范化整理
====================================================================*/
l32 ImageDering_map(TImageInfo *ptImage, l32 l32Quant);

//Dering
#define  ImageDering  ImageDering_map


/*====================================================================
函数名      ： imagezoom_map
功能        ： 将图像（YUV420格式）缩放和类型转换
算法实现    ：(可选项)
引用全局变量：无 
输入参数说明：TImageInfo *pOld : 变换前的图像指针
              TImageInfo *pNew : 变换后的图像指针
			  u8 u8IamgeQuality : 图像变换质量标志
             
返回值说明  ：成功返回VIDEOSUCCESS, 错误返回 VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
日  期        版本        修改人        修改内容
03/24/2005    1.0         zhaobo        创建
====================================================================*/
l32 ImageZoom_map(TImageInfo *ptOld, TImageInfo *ptNew, u8 u8ImageQuality);

//图像缩放
#define ImageZoom ImageZoom_map


/*====================================================================
函数名      ：ImageTypeChange_map
功能        ：将图像(YUV420格式)场帧类型变换
算法实现    ：(可选项)
引用全局变量：无
输入参数说明：TImageInfo *pOld ：放大前的图像指针
              TImageInfo *pNew ：放大后的图像指针			  
返回值说明  ：无
----------------------------------------------------------------------
修改记录    ：
日  期		   版本		修改人		走读人      修改内容
04/26/2005	   1.0		赵波					创建
====================================================================*/
l32 ImageTypeChange_map(TImageInfo *ptOld, TImageInfo *ptNew);

//图像类型转换
#define ImageTypeChange ImageTypeChange_map

/*====================================================================
函数名      ： YUYVToYUV420_c
功能        ： 图像YUV422转化为YUV420
算法实现    ：(可选项)
引用全局变量：无 
输入参数说明：TImageInfo *pOld ：放大前的图像指针
              TImageInfo *pNew ：放大后的图像指针
             
返回值说明  ：成功返回VIDEOSUCCESS, 错误返回 VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
日  期        版本        修改人        修改内容
2006/12/11    1.0         zhaobo          创建
====================================================================*/
l32 YUYVToYUV420_map(TImageInfo *ptOld, TImageInfo *ptNew);

#define YUYVToYUV420 YUYVToYUV420_map

/*====================================================================
函数名      ： YUV420To422_map
功能        ：图像YUV420转为YUV422
算法实现    ：(可选项)
引用全局变量：无 
输入参数说明：TImageInfo *ptYUV420 : 指向YUV420的图像结构体指针
              TImageInfo *ptYUV422 : 指向YUV422的图像结构体指针(YVU
			  422的图像数据存放在结构体的Y分量里面)
             
返回值说明  ：成功返回 VIDEOSUCCESS, 错误返回VIDEOFAILURE
特别说明 : 图像的宽度为8的倍数,高度为2的倍数 
----------------------------------------------------------------------
修改记录    ：
日  期        版本        修改人        修改内容
05/16/2005    1.0        ZouWenYi      代码规范化
====================================================================*/
l32  YUV420To422_map(TImageInfo *ptYUV420, TImageInfo *ptYUV422);

#define  YUV420To422   YUV420To422_map

/*====================================================================
函数名  ： ImageDeblockInit_map
功能	:  对图像进行去块初始化
算法实现    ：(可选项)
引用全局变量：无
输入参数说明： l32  l32Width  :　图像个宽度(必须为8的倍数)

返回值说明 ：成功返回VIDEOSUCCESSFUL, 失败返回 VIDEOFAILURE
特殊说明   : 图像的宽度必须为8的倍数   
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
09/08/2005             ZouWenyi         创建
====================================================================*/
l32 ImageDeblockInit_map(l32 l32Width);

/*====================================================================
函数名  ： ImageDeblock_map
功能	: 对图像进行去块处理
算法实现    ：(可选项)
引用全局变量：无
输入参数说明： TImageInfo *ptOldImage   ：指向Deblock前的图像指针
               TImageInfo *ptNewImage   : 指向Deblock后的图像指针
               l32  l32Quant     : 量化参数

返回值说明 ：成功返回VIDEOSUCCESS, 失败返回 VIDEOFAILURE
特殊说明   : 图像的宽度必须为8的倍数   
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
05/23/2005             ZouWenyi         创建
====================================================================*/
l32 ImageDeblock_map(TImageInfo *ptOldImage, TImageInfo *ptNewImage, l32 l32Quant);

/*====================================================================
函数名  ： ImageDeblockClose_map
功能	:  关闭图像Deblock
算法实现    ：(可选项)
引用全局变量：无
输入参数说明： 

返回值说明 ：成功返回VIDEOSUCCESSFUL, 失败返回 VIDEOFAILURE
特殊说明   :  
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
09/08/2005             ZouWenyi         创建
====================================================================*/
l32 ImageDeblockClose_map();

//Deblock
#define  ImageDeblockInit   ImageDeblockInit_map
#define  ImageDeblock       ImageDeblock_map
#define  ImageDeblockClose  ImageDeblockClose_map

/*====================================================================
函数名      ： TemporalFilterInit_map
功能    	： 时域滤波初始化函数
算法实现    ：
引用全局变量：无
输入参数说明： void **ppvHandle : 句柄
               l32  l32Width    : 图像的宽度
			   l32  l32Height   : 图像的高度

返回值说明  :  成功返回 VIDEOFAILURE, 失败返回 VIDEOSUCCESS;
特别说明: 图像的宽度为32的倍数,支持最大图像为352*576
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
11/19/2004  1.0        Huangkui         创建
03/07/2005  1.0        ZouWenyi        添加DS
06/01/2005             ZouWenyi	      代码规范化
====================================================================*/
l32 TemporalFilterInit_map(void **ppvHandle, l32 l32Width, l32 l32Height);


/*====================================================================
函数名      ： TemporalFilter
功能    	： 时域滤波
算法实现    ：
引用全局变量：无
输入参数说明： void *pvHandle : 指向时域滤波结构体从指针
               u8 *pu8RawImage          : 指向滤波前图像指针
			   u8 *pu8FilteredImage     : 指向滤波后图像指针
			   u16 *pu16CurSum8x8        : 当前帧每8×8块(Y分量)和4×4块(UV分量)象素点数值之和

返回值说明  ：成功返回VIDEOSUCCESS, 失败返回VIDEOFAILURE;
特别说明: 图像的宽度为32的倍数,支持最大图像为352*576
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
11/19/2004  1.0        Huangkui         创建
03/07/2005  1.0        ZouWenyi        添加DS
06/01/2005             ZouWenyi	      代码规范化
====================================================================*/
l32 TemporalFilter_map(void *pvHandle, u8 *pu8RawImage, u8 *pu8FilteredImage, u16 *pu16CurSum8x8);

/*====================================================================
函数名      ： TemporalFilterClose_map
功能    	： 关闭时域滤波句柄
算法实现    ：
引用全局变量：无
输入参数说明： 
            void *pvHandle  :  指向图像前处理的句柄

返回值说明  ：成功返回VIDEOSUCCESS, 失败返回VIDEOFAILURE;
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
11/19/2004  1.0        Huangkui         创建
03/07/2005  1.0        ZouWenyi        添加DS
06/01/2005             ZouWenyi	      代码规范化
====================================================================*/
l32 TemporalFilterClose_map(void *pvHandle);

/*====================================================================
函数名      ： TemporalFilterSetParam_map
功能    	： 时域滤波的参数改变
算法实现    ：
引用全局变量：无
输入参数说明： void *pvHandle : 句柄
               l32  l32Width    : 图像的宽度
               l32  l32Height   : 图像的高度
			   l32  l32Threshold : 滤波时的阈值选择
			        (THRESHIGH为高阈值, THRESLOW为低阈值)

返回值说明  :  成功返回VIDEOSUCCESS, 失败返回VIDEOFAILURE;
特别说明: 图像的宽度为32的倍数,支持最大图像为352*576
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
11/19/2004  1.0        Huangkui         创建
06/02/2005             ZouWenyi	      代码规范化
====================================================================*/
l32 TemporalFilterSetParam_map(void *pvHandle, l32 l32Width, l32 l32Height, l32  l32Threshold);

/*====================================================================
函数名      ： TemporalFilterSetThreshold_map
功能    	： 时域滤波的参数的阈值改变
算法实现    ：
引用全局变量：无
输入参数说明： void *pvHandle : 句柄
			   l32  l32Threshold : 滤波时的阈值选择
			      (THRESHIGH为高阈值, THRESLOW为低阈值(H264编码器))

返回值说明  : 成功返回 VIDEOSUCCESS, 失败返回VIDEOFAILURE;
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
03/27/2006             ZouWenyi	        创建
====================================================================*/
l32 TemporalFilterSetThreshold_map(void *pvHandle, l32 l32Threshold);

//图像的时域滤波
#define  TemporalFilterInit          TemporalFilterInit_map
#define  TemporalFilter              TemporalFilter_map
#define  TemporalFilterClose         TemporalFilterClose_map
#define  TemporalFilterSetParam      TemporalFilterSetParam_map
#define  TemporalFilterSetThreshold  TemporalFilterSetThreshold_map


/*====================================================================
函数名  ： ImageCrossFilter
功能	: 对图像进行十字滤波
算法实现    ：(可选项)
引用全局变量：无
输入参数说明： TImageInfo  *ptOld  ：指向源图像结构体的指针
               TImageInfo  *ptNew  : 指向滤波后图像结构体的指针

返回值说明 ：成功返回VIDEOSUCCESSFUL, 失败返回 VIDEOFAILURE
特殊说明   : 图像的宽度必须为8的倍数   
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
05/30/2005             ZouWenyi        创建
====================================================================*/
l32 ImageCrossFilter_map(TImageInfo *ptOld, TImageInfo *ptNew);

//图像十字滤波
#define  ImageCrossFilter  ImageCrossFilter_map

/*====================================================================
函数名  ： ZoomImageAndMerge420_map
功能	:  先对图像进行缩小,然后合并到背景图像上,图像的格式为420
算法实现    ：(可选项)
引用全局变量：无
输入参数说明： TImageInfo *ptForegroundImage  :　指向前景图像结构体的指针
               TImageInfo *ptBackgroundImage  :  指向背景图像结构体的指针
			   l32  l32Top      : 前景图像缩小后在背景图像中的垂直方向的位置
			   l32  l32Left     : 前景图像缩小后在背景图像中的水平方向的位置
			   l32  l32ZoomType : 前景图像缩小类型(支持1/4, 1/9, 1/16)

返回值说明 ：成功返回VIDEOSUCCESSFUL, 失败返回 VIDEOFAILURE
特殊说明   : 无   
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
10/11/2005             ZouWenyi         创建
====================================================================*/
l32 ZoomImageAndMerge420_map(TImageInfo *ptForegroundImage, TImageInfo *ptBackgroundImage, l32 l32Top, l32 l32Left, l32 l32ZoomType);

#define  ZoomImageAndMerge420  ZoomImageAndMerge420_map

/*====================================================================
函数名  ： IrregularZoomImageAndMerge420_map
功能	:  先对图像进行缩小,然后合并到背景图像上,图像的格式为420
算法实现    ：(可选项)
引用全局变量：无
输入参数说明： TImageInfo *ptForegroundImage  :　指向前景图像结构体的指针
               TImageInfo *ptBackgroundImage  :  指向背景图像结构体的指针
			   l32  l32Top      : 前景图像缩小后在背景图像中的垂直方向的位置
			   l32  l32Left     : 前景图像缩小后在背景图像中的水平方向的位置
			   l32  l32Width    : 前景图像缩小后在背景图像中的宽度
			   l32 l32Height    : 前景图像缩小后在背景图像中的高度

返回值说明 ：成功返回VIDEOSUCCESSFUL, 失败返回 VIDEOFAILURE
特殊说明   : 无   
----------------------------------------------------------------------
修改记录    ：
日  期      版本    修改人     走读人      修改内容
10/31/2005  1.0    	zhaobo                   创建 
===========================================================================*/
l32 IrregularZoomImageAndMerge420_map(TImageInfo *ptForegroundImage, TImageInfo *ptBackgroundImage, l32 l32Top, l32 l32Left, l32 l32Width, l32 l32Height);

#define  IrregularZoomImageAndMerge420  IrregularZoomImageAndMerge420_map

/*====================================================================
函数名      ： MotionSurveil
功能    	： 图像的运动侦测
算法实现    ：
引用全局变量：无
输入参数说明： TImageInfo  *ptCurImage  : 指向当前帧图像的指针
               TImageInfo  *ptPreImage  : 指向前一帧图像的指针
			   u8 *pu8MotionLevel       : 指向运动检测结果的指针
			                            (对每个bit,0表示静止，1表示运动)
返回值说明  ：成功返回 VIDEOSUCCESSFUL，失败返回 VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
日  期      版本    修改人     走读人      修改内容
10/31/2005  1.0    	zhaobo                   创建 
====================================================================*/
l32 MotionSurveil_map(TImageInfo *ptCurImage, TImageInfo *ptPreImage, u8 *pu8MotionLevel);

#define  MotionSurveil  MotionSurveil_map

#elif  defined  C_CODE
////////////////C语言版的函数声明//////////////////////////////////
/*====================================================================
函数名      :	4CifYUV422FieldPToN
功能        :	将4cif的场格式YUV422图像由P制转变为N制
引用全局变量:	无 
算法实现    :	把加的黑边裁去
输入参数说明:	TImageInfo *ptInImage  : 输入图像
				TImageInfo *ptOutImage : 输出图像
返回值说明  :	成功返回 VIDEOSUCCESSFUL，失败返回 VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
06/09/29    1.0         zhaobo			创建
====================================================================*/
l32 D1YUV422FieldPToN_c(TImageInfo *ptInImage, TImageInfo *ptOutImage);

#define  D1YUV422FieldPToN  D1YUV422FieldPToN_c

/*====================================================================
函数名      :	4CifYUV422FieldNToP_c
功能        :	将4cif的场格式YUV422图像由N制转变为P制
引用全局变量:	无 
算法实现    :	上下加黑边
输入参数说明:	TImageInfo *ptInImage  : 输入图像
				TImageInfo *ptOutImage : 输出图像
返回值说明  :	成功返回 VIDEOSUCCESSFUL，失败返回 VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
06/09/29    1.0         zhaobo			创建
====================================================================*/
l32 D1YUV422FieldNToP_c(TImageInfo *ptInImage, TImageInfo *ptOutImage);

#define  D1YUV422FieldNToP  D1YUV422FieldNToP_c

/*====================================================================
函数名      :	D1YUV422FieldNToP_c
功能        :	将4cif的场格式YUV422图像由N制转变为P制,操作在同一块buffer
				进行
引用全局变量:	无 
算法实现    :	上下加黑边
输入参数说明:	TImageInfo *ptInImage  : 输入图像
				TImageInfo *ptOutImage : 输出图像
返回值说明  :	成功返回 VIDEOSUCCESSFUL，失败返回 VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
06/10/10    1.0         zhaobo			创建
====================================================================*/
l32 D1YUV422FieldNToPOneBuffer_c(TImageInfo *ptInImage, TImageInfo *ptOutImage);

#define  D1YUV422FieldNToPOneBuffer  D1YUV422FieldNToPOneBuffer_c

/*====================================================================
函数名      :	D1YUV420FieldPToNOneBuffer_c
功能        :	将4cif的场格式YUV420图像由P制转变为N制, 在一块buffer
				进行
引用全局变量:	无 
算法实现    :	把加的黑边裁去
输入参数说明:	TImageInfo *ptInImage  : 输入图像
				TImageInfo *ptOutImage : 输出图像
返回值说明  :	成功返回 VIDEOSUCCESSFUL，失败返回 VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
06/10/10    1.0         zhaobo			创建
====================================================================*/
l32 D1YUV420FieldPToNOneBuffer_c(TImageInfo *ptInImage, TImageInfo *ptOutImage);

#define  D1YUV420FieldPToNOneBuffer  D1YUV420FieldPToNOneBuffer_c

/*====================================================================
函数名      :	D1YUV420FieldNToPTwoBuffer_c
功能        :	将4cif的场格式YUV420图像由N制转变为P制
				进行
引用全局变量:	无 
算法实现    :	加黑边
输入参数说明:	TImageInfo *ptInImage  : 输入图像
				TImageInfo *ptOutImage : 输出图像
返回值说明  :	成功返回 VIDEOSUCCESSFUL，失败返回 VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
10/27/06    1.0         zhaobo			创建
====================================================================*/
l32 D1YUV420FieldNToPTwoBuffer_c(TImageInfo *ptInImage, TImageInfo *ptOutImage);

#define  D1YUV420FieldNToPTwoBuffer  D1YUV420FieldNToPTwoBuffer_c
/*====================================================================
函数名      ：ProduceMosaic
功能        ：对图像加马赛克
引用全局变量：无 
算法实现    ：(可选项)
输入参数说明：  TImageInfo *ptInImage  : 输入图像
				TImageInfo *ptOutImage : 输出图像
                l32 l32Left            : 马赛克区域左坐标
                l32 l32Top             : 马赛克区域上坐标
                l32 l32WindowWidth     : 马赛克区域宽度
                l32 l32WindowHeight    : 马赛克区域高度
                l32 l32Granularity     : 马赛克粒度
返回值说明  ：成功返回 VIDEOSUCCESSFUL，失败返回 VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
03/11/24    1.0         HuangKui		建立
06/06/22    1.5         zhaobo        规范化、修改
====================================================================*/
l32 ProduceMosaic_c(TImageInfo *ptInImage, TImageInfo *ptOutImage, 
				      l32 l32Left, l32 l32Top, l32 l32WindowWidth, l32 l32WindowHeight, l32 l32Granularity);
#define  ProduceMosaic  ProduceMosaic_c

/*====================================================================
函数名      ：MergePictureInit
功能        ：图像合成初始化
引用全局变量：无 
算法实现    ：(可选项)
输入参数说明：void **ppvHandle : 句柄
返回值说明  ：成功返回VIDEOSUCCESS, 失败返回 VIDEOFAILURE
特别说明 :  只支持YUV420的图像合并
----------------------------------------------------------------------
修改记录    ：
日  期      版本         修改人        修改内容
11/24/2003  1.0         HuangKui	     创建
04/26/2005  1.1         ZouWenyi       代码规范化         
08/10/2006  2.0            zb         和Map版本一致 
====================================================================*/
l32 MergePictureInit_c(void **ppvHandle);
#define  MergePictureInit  MergePictureInit_c
/*====================================================================
函数名      ： MergePicture
功能        ：把小图像覆盖到大图像的指定位置
引用全局变量：无 
算法实现    ：(可选项)
输入参数说明：void *pvHandle 句柄
			  TImageInfo *ptBackgroundImage   指向背景图像的指针
              TLogoInfo *ptLogo  指向包含有前景图像信息的结构的指针
			    (小图像的数据放在ptLogo的pu8YUV成员变量,需要设置ptLogo的
				其他成员变量:l32Height(小图像的高度), l32Width(小图像的宽度), 
				l32Left(小图像在大图像中水平方向偏移的大小), l32Top(小图像在
				大图像中垂直方向偏移的大小), l32ImageType(帧格式或者场格式),
				l32TopCutLine(小图像上边裁去的行数),l32BottomCutLine(小图像下
				边裁去的行数), l32LeftCutLine(小图像左边裁去的列数), l32RightCutLine
				(小图像右边裁去的列数))
			  
返回值说明  ：成功返回VIDEOSUCCESS, 失败返回 VIDEOFAILURE
特别说明 :  只支持YUV420的图像合并
----------------------------------------------------------------------
修改记录    ：
日  期      版本         修改人        修改内容
11/24/2003  1.0         HuangKui	     创建
04/26/2005  1.1         ZouWenyi       代码规范化         
08/10/2006  2.0            zb         和Map版本一致
====================================================================*/
l32 MergePicture_c(void *pvHandle, TImageInfo *ptBackgroundImage, TLogoInfo *ptLogo);

#define  MergePicture  MergePicture_c
/*====================================================================
函数名      ： MergePictureClose
功能        ： 图像合成关闭
引用全局变量：无 
算法实现    ：(可选项)
输入参数说明：void *pvHandle 句柄			  
返回值说明  ：成功返回VIDEOSUCCESS, 失败返回 VIDEOFAILURE
特别说明 :  只支持YUV420的图像合并
----------------------------------------------------------------------
修改记录    ：
日  期      版本         修改人        修改内容
11/24/2003  1.0         HuangKui	     创建
04/26/2005  1.1         ZouWenyi       代码规范化         
08/10/2006  2.0            zb         和Map版本一致  
====================================================================*/
l32 MergePictureClose_c(void *pvHandle);

#define  MergePictureClose  MergePictureClose_c

/*====================================================================
函数名      :	ImageSizeChange_c
功能	    :	采用双线性插值的方法将源图像缩放标图像
算法实现    :	
引用全局变量:	无
输入参数说明:	TImageInfo *ptInImage : 变换前的图像指针
				TImageInfo *ptOutImage : 变换后的图像指针
返回值说明  :	无
----------------------------------------------------------------------
修改记录:    
日  期		   版本		修改人		走读人      修改内容
03/07/2006      1.0      赵波                    创建 
====================================================================*/
l32 ImageSizeChange_c(TImageInfo *ptInImage, TImageInfo *ptOutImage);

#define  ImageSizeChange  ImageSizeChange_c

/*====================================================================
函数名      :	YUV420toRGB
功能	    :	YUV420格式转化为rgb格式
算法实现    :	
引用全局变量:	无
输入参数说明:	u8 *pu8Dst            : rgb图像的指针
				l32 l32RGBTop         : rgb图像的右边界
				l32 l32RGBLeft        : rgb图像的左边界
				l32 l32RGBWidth       : rgb图像的宽度
				l32 l32RGBHeight      : rgb图像的高度
				(当整幅图像正好是rgb图像时，该参数置0即可，不然设置实际跨度)
				u8 u8RGBType          : rgb图像的类型
				TImageInfo *ptInImage : 输入的YUV420图像
返回值说明  :	无
----------------------------------------------------------------------
修改记录:    
日  期		   版本		修改人		走读人      修改内容
06/29/2006     1.0       赵波                     创建 
====================================================================*/
l32 YUV420toRGB_c(u8 *pu8Dst, l32 l32RGBTop, l32 l32RGBLeft, l32 l32RGBWidth, l32 l32RGBHeight, u8 u8RGBType, TImageInfo *ptInImage);

#define  YUV420toRGB	YUV420toRGB_c

/*====================================================================
函数名      ： DrawBanner_c
功能        ：在图像中添加横幅(填充矩形块区域为指定的颜色)
算法实现    ：(可选项)
引用全局变量：无 
输入参数说明：TImageInfo  *ptImage   : 指向图像结构体的指针
		      TBanner *ptBanner      : 指向横幅结构的指针

返回值说明  ：成功返回VIDEOSUCCESSFUL, 失败返回 VIDEOFAILURE  
特别说明 : 图像的数据指向结图像构体中的Y分量            
----------------------------------------------------------------------
修改记录    ：
日  期        版本        修改人        修改内容
05/18/2005    1.1        ZouWenyi        创建
10/27/2005    1.2        ZouWenyi        修改
====================================================================*/
l32 DrawBanner_c(TImageInfo *ptImage, TBanner *ptBanner);

#define  DrawBanner  DrawBanner_c

/*====================================================================
函数名      ： YUV422ToYUV420AndTemporalFilterInit_c
功能        ： 图像YUV422转化为YUV420的初始化
算法实现    ：(可选项)
引用全局变量：无 
输入参数说明：void **ppvHandle   : 指向图像前处理句柄的指针
              ptYUV422To420Param  : YUV422转为YUV420图像的参数设置
              
返回值说明  ：成功返回VIDEOSUCCESSFUL, 错误返回 VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
日  期        版本        修改人        修改内容
2006/04/21    1.0        ZouWenYi        创建
====================================================================*/
l32  YUV422ToYUV420AndTemporalFilterInit_c(void **ppvHandle, TYUV422ToYUV420Param *ptYUV422To420Param);

#define  YUV422ToYUV420AndTemporalFilterInit YUV422ToYUV420AndTemporalFilterInit_c

/*====================================================================
函数名      ： YUV422ToYUV420AndTemporalFilter_c
功能        ：图像YUV422转化为YUV420的处理并进行时域滤波
算法实现    ：(可选项)
引用全局变量：无 
输入参数说明：void *pvHandle   : 指向图像前处理句柄的指针
              u8   *pu8Src     : 指向源图像数据的指针
			  u8   *pu8Dst     : 指向目标图像数据的指针
								 
返回值说明  ：成功返回VIDEOSUCCESSFUL, 失败返回VIDEOFAILURE;
举例或特别说明：只支持分辨率720*576的YUV422转到分辨率720*576,352*576, 352*288的YUV420图像
                时域滤波只支持352*288和352*576
----------------------------------------------------------------------
修改记录    ：
日  期        版本        修改人        修改内容
2006/04/21    1.0        ZouWenYi         创建
====================================================================*/
l32 YUV422ToYUV420AndTemporalFilter_c(void *pvHandle, u8 *pu8Src, u8 *pu8Dst);

#define  YUV422ToYUV420AndTemporalFilter YUV422ToYUV420AndTemporalFilter_c

/*====================================================================
函数名      ： YUV422ToYUV420AndTemporalFilterSetParam_c
功能        ：图像YUV422转到YUV420的参数设置
算法实现    ：(可选项)
引用全局变量：无 
输入参数说明：void *pvHandle   : 指向图像前处理句柄的指针
              ptYUV422To420Param  : YUV422转为YUV420图像的参数设置
             
返回值说明  ：成功返回 VIDEOSUCCESS, 错误返回VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
日  期        版本        修改人        修改内容
2006/04/21    1.0        ZouWenYi        创建
====================================================================*/
l32 YUV422ToYUV420AndTemporalFilterSetParam_c(void *pvHandle, TYUV422ToYUV420Param *ptYUV422To420Param);

#define  YUV422ToYUV420AndTemporalFilterSetParam  YUV422ToYUV420AndTemporalFilterSetParam_c

/*====================================================================
函数名      ： YUV422ToYUV420AndTemporalFilterClose_c
功能        ：释放前处理的句柄
算法实现    ：(可选项)
引用全局变量：无 
输入参数说明：void *pvHandle  : 图像YUV422转为YUV420的句柄

返回值说明  ：成功返回 VIDEOSUCCESS, 错误返回VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
日  期        版本        修改人        修改内容
2006/04/21    1.0        ZouWenYi         创建
====================================================================*/	
l32 YUV422ToYUV420AndTemporalFilterClose_c(void *pvHandle);

#define  YUV422ToYUV420AndTemporalFilterClose YUV422ToYUV420AndTemporalFilterClose_c

/*====================================================================
函数名      ： TemporalFilterInit_c
功能    	： 时域滤波初始化函数
算法实现    ：
引用全局变量：无
输入参数说明： void **ppvHandle : 句柄
               l32  l32Width    : 图像的宽度
			   l32  l32Height   : 图像的高度
			   u8   *pu8CommonBuf :  公共缓存(分配的大小为(7*352*576*3/2) + (46*352*576/64) + (20<<10); 
			   其中7*352*576*3/2保存前面7帧的数据, (46*352*576/64)保存前面7帧8*8块数据之和和
			   计算8*8块数据之和之差最大绝对值, 最后20<<10是DS使用的公共Buffer,C语言版不需要

返回值说明  :  成功返回 VIDEOSUCCESS, 失败返回 VIDEOFAILURE;
特别说明: 图像的宽度为32的倍数,支持最大图像为352*576
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
03/10/2005              ZouWenyi	       创建
====================================================================*/
l32 TemporalFilterInit_c(void **ppvHandle, l32 l32Width, l32 l32Height);

/*====================================================================
函数名      ： TemporalFilter
功能    	：  时域滤波
算法实现    ：
引用全局变量：无
输入参数说明： TTemporalFilterHandle *ptTemporalFilter : 指向时域滤波结构体从指针
               u8 *pu8RawImage          : 指向滤波前图像指针
			   u8 *pu8FilteredImage     : 指向滤波后图像指针
			   u16 *pu16CurSum8x8        : 当前帧每8×8块(Y分量)和4×4块(UV分量)象素点数值之和

返回值说明  ：成功返回VIDEOSUCCESS, 失败返回 VIDEOFAILURE
特别说明: 图像的宽度为32的倍数,支持最大图像为352*576
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
03/10/2005              ZouWenyi	       创建
====================================================================*/
l32 TemporalFilter_c(void *ptTemporalFilter, u8 *pu8RawImage, u8 *pu8FilteredImage, u16 *pu16CurSum8x8);

/*====================================================================
函数名      ： TemporalFilterClose_c
功能    	： 关闭时域滤波句柄
算法实现    ：
引用全局变量：无
输入参数说明： 
            void *pvHandle  :  指向图像前处理的句柄

返回值说明  ：成功返回 VIDEOSUCCESS, 失败返回VIDEOFAILURE;
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
03/10/2005             ZouWenyi	       创建
====================================================================*/
l32 TemporalFilterClose_c(void *pvHandle);

/*====================================================================
函数名      ： TemporalFilterSetParam_c
功能    	： 时域滤波的参数改变
算法实现    ：
引用全局变量：无
输入参数说明： void *pvHandle : 句柄
               l32  l32Width    : 图像的宽度
			   l32  l32Height   : 图像的高度
			   l32  l32Threshold : 滤波时的阈值选择
			                     (THRESHIGH为高阈值, THRESLOW为低阈值)

返回值说明  :  成功返回 VIDEOSUCCESS, 失败返回VIDEOFAILURE;
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
03/10/2005             ZouWenyi	        创建
====================================================================*/
l32 TemporalFilterSetParam_c(void *pvHandle, l32 l32Width, l32 l32Height, l32 l32Threshold);

/*====================================================================
函数名      ： TemporalFilterSetThreshold_c
功能    	： 时域滤波的参数的阈值改变
算法实现    ：
引用全局变量：无
输入参数说明： void *pvHandle : 句柄
			   l32  l32Threshold : 滤波时的阈值选择
			      (THRESHIGH为高阈值, THRESLOW为低阈值(H264编码器))

返回值说明  : 成功返回 VIDEOSUCCESS, 失败返回VIDEOFAILURE;
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
03/27/2006             ZouWenyi	        创建
====================================================================*/
l32 TemporalFilterSetThreshold_c(void *pvHandle, l32 l32Threshold);

/*====================================================================
函数名      ： imagezoom_c
功能        ： 将图像（YUV420格式）缩放和类型转换
算法实现    ：(可选项)
引用全局变量：无 
输入参数说明：TImageInfo *pOld : 变换前的图像指针
              TImageInfo *pNew : 变换后的图像指针
			  u8 u8IamgeQuality : 图像变换质量标志
             
返回值说明  ：成功返回VIDEOSUCCESS, 错误返回 VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
日  期        版本        修改人        修改内容
03/24/2005    1.0         zhaobo        创建
====================================================================*/
l32 ImageZoom_c(TImageInfo *ptOld, TImageInfo *ptNew, u8 u8ImageQuality);

//图像缩放
#define ImageZoom ImageZoom_c


/*====================================================================
函数名      ：ImageTypeChange_c
功能        ：将图像(YUV420格式)场帧类型变换
算法实现    ：(可选项)
引用全局变量：无
输入参数说明：TImageInfo *pOld ：放大前的图像指针
              TImageInfo *pNew ：放大后的图像指针			  
返回值说明  ：无
----------------------------------------------------------------------
修改记录    ：
日  期		   版本		修改人		走读人      修改内容
04/26/2005	   1.0		赵波					创建
====================================================================*/
l32 ImageTypeChange_c(TImageInfo *ptOld, TImageInfo *ptNew);

//图像类型转换
#define ImageTypeChange ImageTypeChange_c

/*====================================================================
函数名      ： YUYVToYUV420_c
功能        ： 图像YUV422转化为YUV420
算法实现    ：(可选项)
引用全局变量：无 
输入参数说明：TImageInfo *pOld ：放大前的图像指针
              TImageInfo *pNew ：放大后的图像指针
             
返回值说明  ：成功返回VIDEOSUCCESS, 错误返回 VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
日  期        版本        修改人        修改内容
2006/12/11    1.0         zhaobo          创建
====================================================================*/
l32 YUYVToYUV420_c(TImageInfo *ptOld, TImageInfo *ptNew);

#define YUYVToYUV420 YUYVToYUV420_c

/*====================================================================
函数名      ： YUV422ToYUV420Init_c
功能        ： 图像YUV422转化为YUV420的初始化
算法实现    ：(可选项)
引用全局变量：无 
输入参数说明：void **ppvHandle   : 指向图像前处理句柄的指针
              l32 l32SrcWidth    : 源图像的宽度
			  l32 l32SrcHeight   : 源图像的高度
			  l32 l32SrcStride   : 源图像的步长
			  l32 l32DstWidth    : 目标图像的宽度
			  l32 l32DstHeight   : 目标图像的高度
			  l32 l32DstStride   : 目标图像的步长
             
返回值说明  ：成功返回VIDEOSUCCESS, 错误返回 VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
日  期        版本        修改人        修改内容
2005/05/13    1.0        ZouWenYi        创建
====================================================================*/
l32 YUV422ToYUV420Init_c(void **ppvHandle, l32 l32SrcWidth, l32 l32SrcHeight, l32 l32SrcStride,
					   l32 l32DstWidth, l32 l32DstHeight, l32 l32DstStride);


/*====================================================================
函数名      ： YUV422ToYUV420_c
功能        ：图像YUV422转化为YUV420的处理
算法实现    ：(可选项)
引用全局变量：无 
输入参数说明：void *pvHandle   : 指向图像前处理句柄的指针
              u8   *pu8Src     : 指向源图像数据的指针
			  u8   *pu8Dst     : 指向目标图像数据的指针
			  u16  *pu16Sum8x8 : 指向Y分量8*8块的数据之和,UV分量4*4块数据之和的指针,
			                     当该指针为空时,表示不计算8*8块的数据之和			  

返回值说明  ：成功返回VIDEOSUCCESS, 失败返回VIDEOFAILURE;
----------------------------------------------------------------------
修改记录    ：
日  期        版本        修改人        修改内容
2005/05/13    1.0        ZouWenYi         创建
====================================================================*/
l32 YUV422ToYUV420_c(void *pvHandle, u8 *pu8Src, u8 *pu8Dst, u16 *pu16Sum8x8);


/*====================================================================
函数名      ： YUV422ToYUV420SetParam_c
功能        ：图像YUV422转到YUV420的参数设置
算法实现    ：(可选项)
引用全局变量：无 
输入参数说明：void *pvHandle   : 指向图像前处理句柄的指针
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
2005/05/13    1.0        ZouWenYi        创建
====================================================================*/
l32 YUV422ToYUV420SetParam_c(void *pvHandle, l32 l32SrcWidth, l32 l32SrcHeigt, l32 l32SrcStride,
							 l32 l32DstWidth, l32 l32DstHeight, l32 l32DstStride);

/*====================================================================
函数名      ： YUV422ToYUV420Close_c
功能        ：释放前处理的句柄
算法实现    ：(可选项)
引用全局变量：无 
输入参数说明：void *pvHandle  : 图像YUV422转为YUV420的句柄

返回值说明  ：成功返回 VIDEOSUCCESS, 错误返回VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
日  期        版本        修改人        修改内容
2005/05/13    1.0        ZouWenYi         创建
====================================================================*/	
l32 YUV422ToYUV420Close_c(void *pvHandle);
							 
/*====================================================================
函数名      ：AddLogo_c
功能        ：将Logo加到图像上
引用全局变量：无 
算法实现    ：(可选项)
输入参数说明：TImageInfo *ptBackgroundImage   指向背景图像的指针
              TLogoInfo *ptLogo  指向包含有前景图像信息的结构的指针

返回值说明  ：成功返回VIDEOSUCCESS, 失败返回 VIDEOFAILURE

特别说明 : 图标的宽度为8的倍数,高度为4的倍数,否则返回错误.
----------------------------------------------------------------------
修改记录    ：
日  期      版本           修改人        修改内容
04/26/2005    1.1         ZouWenyi         创建
====================================================================*/
l32 AddLogo_c(TImageInfo *ptBackgroundImage, TLogoInfo *ptLogo);

/*====================================================================
函数名  ： ImageDering_c
功能	:  对图像进行去振铃效应
算法实现    ：(可选项)
引用全局变量：无
输入参数说明： TImageInfo *ptImage   ：Dering图像指针
               l32         l32Quant  : 量化参数(范围2~62)

返回值说明 ：成功返回VIDEOSUCCESS, 失败返回 VIDEOFAILURE
特殊说明   : 图像的宽度必须为8的倍数   
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
05/14/2005             ZouWenyi    代码规范化整理
====================================================================*/
l32 ImageDering_c(TImageInfo *ptImage, l32 l32Quant);

/*====================================================================
函数名      ： YUV420To422_c
功能        ：图像YUV420转为YUV422
算法实现    ：(可选项)
引用全局变量：无 
输入参数说明：TImageInfo *ptYUV420 : 指向YUV420的图像结构体指针
              TImageInfo *ptYUV422 : 指向YUV422的图像结构体指针(YVU
                 422的图像数据存放在结构体的Y分量里面, 步长存放在Y分量
				 的步长)

返回值说明  ：成功返回 VIDEOSUCCESS, 错误返回VIDEOFAILURE
特别说明 : 图像的宽度为8的倍数,高度为2的倍数 
----------------------------------------------------------------------
修改记录    ：
日  期        版本        修改人        修改内容
05/16/2005    1.0        ZouWenYi        创建
====================================================================*/
l32  YUV420To422_c(TImageInfo *ptYUV420, TImageInfo *ptYUV422);

/*====================================================================
函数名  ： ImageDeblockInit_c
功能	:  对图像进行去块初始化
算法实现    ：(可选项)
引用全局变量：无
输入参数说明： l32  l32Width  :　图像个宽度(必须为8的倍数)

返回值说明 ：成功返回VIDEOSUCCESSFUL, 失败返回 VIDEOFAILURE
特殊说明   :  
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
09/08/2005             ZouWenyi         创建
====================================================================*/
l32 ImageDeblockInit_c(l32 l32Width);

/*====================================================================
函数名  ： ImageDeblock_c
功能	: 对图像进行去块处理
算法实现    ：(可选项)
引用全局变量：无
输入参数说明： TImageInfo *ptOldImage   ：指向Deblock前的图像指针
               TImageInfo *ptNewImage   : 指向Deblock后的图像指针
               l32  l32Quant  : 量化参数

返回值说明 ：成功返回VIDEOSUCCESS, 失败返回 VIDEOFAILURE
特殊说明   : 图像的宽度必须为8的倍数   
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
05/23/2005             ZouWenyi         创建
====================================================================*/
l32 ImageDeblock_c(TImageInfo *ptOldImage, TImageInfo *ptNewImage, l32 l32Quant);

/*====================================================================
函数名  ： ImageDeblockClose_c
功能	:  关闭图像Deblock
算法实现    ：(可选项)
引用全局变量：无
输入参数说明： 

返回值说明 ：成功返回VIDEOSUCCESSFUL, 失败返回 VIDEOFAILURE
特殊说明   :  
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
09/08/2005             ZouWenyi         创建
====================================================================*/
l32 ImageDeblockClose_c();

/*====================================================================
函数名  ： ZoomImageAndMerge420_c
功能	:  先对图像进行缩小,然后合并到背景图像上,图像的格式为420
算法实现    ：(可选项)
引用全局变量：无
输入参数说明： TImageInfo *ptForegroundImage  :　指向前景图像结构体的指针
               TImageInfo *ptBackgroundImage  :  指向背景图像结构体的指针
			   l32  l32Top      : 前景图像缩小后在背景图像中的垂直方向的位置
			   l32  l32Left     : 前景图像缩小后在背景图像中的水平方向的位置
			   l32  l32ZoomType : 前景图像缩小类型(支持1/4, 1/9, 1/16)

返回值说明 ：成功返回VIDEOSUCCESSFUL, 失败返回 VIDEOFAILURE
特殊说明   : 无   
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
10/11/2005             ZouWenyi         创建
====================================================================*/
l32 ZoomImageAndMerge420_c(TImageInfo *ptForegroundImage, TImageInfo *ptBackgroundImage, l32 l32Top, l32 l32Left, l32 l32ZoomType);

/*====================================================================
函数名  ： IrregularZoomImageAndMerge420_c
功能	:  先对图像进行缩小,然后合并到背景图像上,图像的格式为420
算法实现    ：(可选项)
引用全局变量：无
输入参数说明： TImageInfo *ptForegroundImage  :　指向前景图像结构体的指针
               TImageInfo *ptBackgroundImage  :  指向背景图像结构体的指针
			   l32  l32Top      : 前景图像缩小后在背景图像中的垂直方向的位置
			   l32  l32Left     : 前景图像缩小后在背景图像中的水平方向的位置
			   l32  l32Width    : 前景图像缩小后在背景图像中的宽度
			   l32 l32Height    : 前景图像缩小后在背景图像中的高度
			
返回值说明 ：成功返回VIDEOSUCCESSFUL, 失败返回 VIDEOFAILURE
特殊说明   : 无   
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
03/31/2006              zhaobo          创建
====================================================================*/
l32 IrregularZoomImageAndMerge420_c(TImageInfo *ptForegroundImage, TImageInfo *ptBackgroundImage, l32 l32Top, l32 l32Left, l32  l32Width, l32 l32Height);

/*====================================================================
函数名      ： MotionSurveil
功能    	： 图像的运动侦测
算法实现    ：
引用全局变量：无
输入参数说明： TImageInfo  *ptCurImage  : 指向当前帧图像的指针
               TImageInfo  *ptPreImage  : 指向前一帧图像的指针
			   u8 *pu8MotionLevel       : 指向运动检测结果的指针
			                            (对每个bit,0表示静止，1表示运动)
返回值说明  ：成功返回 VIDEOSUCCESSFUL，失败返回 VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
日  期      版本    修改人     走读人      修改内容
10/31/2005  1.0    	zhaobo                   创建 
====================================================================*/
l32 MotionSurveil_c(TImageInfo *ptCurImage, TImageInfo *ptPreImage, u8 *pu8MotionLevel);

#define  MotionSurveil  MotionSurveil_c

//图像的时域滤波
#define  TemporalFilterSetParam      TemporalFilterSetParam_c
#define  TemporalFilterInit          TemporalFilterInit_c
#define  TemporalFilter              TemporalFilter_c
#define  TemporalFilterClose         TemporalFilterClose_c
#define  TemporalFilterSetThreshold  TemporalFilterSetThreshold_c

//去振铃效应
#define   ImageDering   ImageDering_c

//添加图标
#define   AddLogo   AddLogo_c

//图像YUV422转为YUV420
#define  YUV422ToYUV420Init     YUV422ToYUV420Init_c
#define  YUV422ToYUV420         YUV422ToYUV420_c
#define  YUV422ToYUV420Close    YUV422ToYUV420Close_c
#define  YUV422ToYUV420SetParam YUV422ToYUV420SetParam_c

//图像YUV420转为YUV422
#define  YUV420To422      YUV420To422_c

//图像Deblock
#define  ImageDeblockInit   ImageDeblockInit_c
#define  ImageDeblock       ImageDeblock_c
#define  ImageDeblockClose  ImageDeblockClose_c

//图像缩小和合并
#define  ZoomImageAndMerge420  ZoomImageAndMerge420_c
#define  IrregularZoomImageAndMerge420  IrregularZoomImageAndMerge420_c

#endif

#ifdef __cplusplus
};
#endif 

#endif // _IMAGEPROCESS_H_

