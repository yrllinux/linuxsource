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
#include "videodefines.h"

#ifdef __cplusplus
extern "C"
{
#endif 

#define LOGO_STATUS_LEN		20
#define BANNER_STATUS_LEN   20
#define VCODEC_MPEG4_V200	0x0005

//sq 050810 add 画面合成类型定义
typedef enum {
  PIC_NO_MERGE,
  PIC_MERGE_ONE,
  PIC_MERGE_VTWO,
  PIC_MERGE_ITWO,
  PIC_MERGE_THREE,
  PIC_MERGE_FOUR,
  PIC_MERGE_SFOUR,
  PIC_MERGE_SIX,
  PIC_MERGE_MAX
} PicMergeStyle;

//某路图像是否勾画边框标记
typedef enum {
    NO_DRAW_FOCUS ,
    DRAW_FOCUS  
} DrawFocusFlag;

//前处理中是否画边框
#define PREPROCESS_DRAWSIDES   1  

//前景图像是否勾画选定边框
#define PREPROCESS_DRAWFOCUS   1

//前处理无图像处是否填充背景色
#define PREPROCESS_NO_PIC_FILL_BG  1

//前处理中是否进行时域滤波
#define PREPROCESS_TEMPORAL_FILTER
//前处理时域滤波效率控制阈值
#define TEMPORAL_FILTER_CONTROL  (9)    //取值范围0-10，可以取小数，以主频的X*10%作为编码可用周期数

//前处理中是否进行十字滤波
#define PREPROCESS_CROSS_FILTER   1
//前处理十字滤波效率控制阈值
#define CROSS_FILTER_CONTROL     (7.7)  //取值范围0-10，可以取小数，以主频的X*10%作为编码可用周期数

//编码需达到的帧率
#define FRAMERATE_REQUESTED  25

//编码是否使用帧率控制
#define FRAMERATE_CONTROL

//编码中是否添加横幅图标
#define ENCODE_ADD_BANNER_LOGO

//解码中是否添加横幅图标
#define DECODE_ADD_BANNER_LOGO

//添加图标最多个数
#define MAX_LOGO_NUM		32   

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
	l32  l32Top;                //前景图象位置偏移
	l32	 l32Left;               //前景图象位置偏移
	l32  l32TopCutLine;         //前景图像上边裁去的行数
	l32  l32BottomCutLine;      //前景图像下边裁去的行数
	l32  l32LeftCutLine;        //前景图像左边裁去的列数
	l32  l32RightCutLine;       //前景图像右边裁去的列数
	l32  l32ImageType;          //前景图像的类型(帧格式或者场格式)
	l32  l32LogoRefPicWidth;    //图标生成的参考图象高度
	l32  l32LogoRefPicHeight;   //图标生成的参考图象宽度

} TLogoInfo;

typedef struct tImage
{
	TImageInfo tImageInfo;	    //图象信息
	u8      u8DrawFocusFlag;    //画面是否勾画边框的标记
	u8		u8FocuslineR;		//画面边框色R
	u8		u8FocuslineG;		//画面边框色G
	u8		u8FocuslineB;		//画面边框色B
    u8      u8FocusWidth;       //边框统一宽度（暂不可设置）
	u8		u8Pos;				//画面位置编号
	u32		u32Reserved;		//保留参数
}TImage;		//图象


//图标信息的结构体
typedef struct tLogo
{
	u8		*pu8YUV;               //前景图象数据指针地址(图象格式为YUV422时,分配的空间大小为l32Width*l32Height*2,图象格式为YUV420时,分配的空间大小为l32Width*l32Height*3/2)
	u8		*pu8Alpha;     		   //前景图象的alpha通道指针(分配空间的大小和分配图象数据的大小一样)
	u8		*pu8Beta;			   //对应背景图象的beta通道指针(分配空间的大小和分配图象数据的大小一样)
	u32		u32Width;              //图标图象的宽度
	u32		u32Height;             //图标图象的高度
	l32		l32YUVType;            //图标图象格式(YUV422或者YUV420)
	l32		l32ImageType;          //前景图象的类型(帧格式或者场格式)
	u32     u32LogoRefPicWidth;    //图标添加参考图像的宽度(用户填写，目前基本使用720)
	u32     u32LogoRefPicHeight;   //图标添加参考图像的高度(用户填写，目前基本使用576)
	u8	 	u8FgTransparency;      //前景图象透明系数
    u8      u8BgTransparency;      //背景图象透明系数
    s16 	s16Reserved;		   //保留参数
	l32 	l32Top;				   //图标图象垂直位置偏移（相对于显示窗口）
	l32		l32Left;        	   //图标图象水平位置偏移（相对于显示窗口）
	s16		s16StepH;			   //图标水平运动速度（每帧移动象素数，正向右负向左）
	s16		s16StepV;			   //图标垂直运动速度（每帧移动象素数，正向上负向下）
	l32		l32ShowTimes;		   //图标显示次数（负：一直显示；0：不显示；正：图标有速度时为循环显示次数，图标无速度时调用次数）
	l32		l32PauseIntervalH;	   //图标水平运动暂停间隔（象素数）（正：有暂停； 非正：无暂停）
	l32		l32PauseIntervalV;	   //图标垂直运动暂停间隔（象素数）（正：有暂停； 非正：无暂停）
	l32		l32PauseTime;		   //图标暂停时间（调用次数）
	l32		l32WindowTop;		   //显示窗口垂直位置
	l32		l32WindowLeft;		   //显示窗口水平位置
	l32		l32WindowWidth;	       //显示窗口宽度
	l32		l32WindowHeight;	   //显示窗口高度
	u32		u32Reserved;			//保留参数
	l32		al32LogoStatus[LOGO_STATUS_LEN];	//图标状态，须上层初始化时清零（内部使用）目前使用情况：
												//l32[LOGO_H_INTER_MOVE_COUNT]:图标水平暂停间连续移动计数
	                                            //l32[LOGO_H_PAUSE_COUNT]:图标水平暂停计数
	                                            //l32[LOGO_V_INTER_MOVE_COUNT]:图标垂直暂停间连续移动计数
	                                            //l32[LOGO_V_PAUSE_COUNT]:图标垂直暂停计数
	                                            //l32[LOGO_CALL_TIME]:图标总共添加的次数
	                                            //l32[LOGO_FIRST_ADD_FLAG]:图标首次添加标记0:首次 1:非首次
                                                //l32[LOGO_CUR_BG_WIDTH]:图标添加基于的背景宽度
                                                //l32[LOGO_CUR_BG_HEIGHT]:图标添加基于的背景高度
}TLogo;			//图标

typedef struct tMerge
{
	TImage  tImageProcessed;    //画面合成背景图象信息
	u16		u16MergeStyle;		//画面合成模式
    u16     u16BoundaryWidth;   //画面合成各路图像统一边界线宽度（暂不可设置）	
	u8		u8BackgroundR;		//画面合成背景色R
	u8		u8BackgroundG;		//画面合成背景色G
	u8		u8BackgroundB;		//画面合成背景色B
    u8      u8BoundaryR;        //画面合成各路图像统一边界线颜色R
    u8      u8BoundaryG;        //画面合成各路图像统一边界线颜色G
    u8      u8BoundaryB;        //画面合成各路图像统一边界线颜色B
	u32		u32Reserved;		//保留参数	
    u8      u8PreprocessPic;    //PREPROCESS_NO_PIC_FILL_BG 前处理无图像处填充背景色
    u8      u8Drawsides;        //PREPROCESS_DRAWSIDES 前处理中画边框
    u8      u8Drawfocus;        //PREPROCESS_DRAWFOCUS 前景图像勾画选定边框
}TMerge;		                //图像合成


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

typedef struct tBanner
{
	l32		l32Width;			//横幅宽度
	l32		l32Height;			//横幅高度
	l32		l32Top;				//横幅左上角垂直坐标
	l32		l32Left;			//横幅左上角水平坐标
	l32     l32BannerRefPicWidth;    //横幅添加参考图像的宽度
	l32     l32BannerRefPicHeight;   //横幅添加参考图像的高度
	l32		l32ShowTimes;		//显示次数（负：一直显示；0：不显示；正：显示次数；和图标关联）
	u8		u8R;				//横幅颜色R
	u8		u8G;				//横幅颜色G
	u8		u8B;				//横幅颜色B
	u8      u8Transparency;     //横幅的透过系数(0表示全透明，255表示不透明，1~254表示部分透明)
	
}TBanner;		//横幅

//背景图像信息的结构体
typedef struct 
{
	l32  l32Width;        //图像的宽度
	l32  l32Height;       //图像的高度
	l32	 l32ImageType;    //图像的类型(帧格式或者场格式)
	l32  l32YUVType;      //图标图像格式(YUV422或者YUV420)	
}TBackgroundImage;

//图标参数
typedef struct 
{
    TLogoInfo tLogo;           //图标参数
	TBackgroundImage tImage;   //背景图像参数
} TImageLogoParam;

//横幅参数
typedef struct 
{
	TBanner tBanner;           //横幅参数
    TBackgroundImage tImage;   //背景图像参数
} TImageBannerParam;
   
//横幅图标滚动参数
typedef struct
{
	TLogo atLogo[MAX_LOGO_NUM];                 //图标参数结构数组
    TBanner atBanner[MAX_LOGO_NUM];             //横幅参数结构数组
	TBackgroundImage tImage;                    //背景图像参数结构
	l32 l32LogoNum;                             //图标个数
	l32 l32BannerNum;                           //横幅个数
} TImageBannerLogoParam;

//图标输入结构
typedef struct
{
	u8 *pu8YUV;	      //输入输出图像数据
} TImageLogoInput;

//横幅输入结构
typedef struct
{
	u8 *pu8YUV;	      //输入输出图像数据
} TImageBannerInput;

//横幅图标输入结构
typedef struct
{
	u8 *pu8YUV;	      //输入输出图像数据
} TImageBannerLogoInput;

//Windows平台下共用的函数声明
#if  defined(C_CODE)

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

//添加图标
#define   AddLogo   AddLogo_c

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
函数名	    ：DrawBannerAndLogo_c
功能	    ：横幅、图标与背景图像的叠加,图标和背景图像可以移动
算法实现    ：无
引用全局变量：无
输入参数说明：aptLogoInput[]     输入的图标信息指针组
              aptBannerInput[]   输入的横幅信息指针组
              ptImage            背景图像信息指针
输出参数说明：ptImage->pu8YUV 叠加完毕的图像指针
返回值说明  ：VIDEOSUCCESS：成功，VIDEOFAILURE：失败
====================================================================*/
l32 DrawBannerAndLogo_c(TLogo *aptLogoInput[], TBanner *aptBannerInput[], TImage *ptImage);

#define DrawBannerAndLogo DrawBannerAndLogo_c

/*====================================================================
函数名	    ：ImageMerge_c
功能	    ：画面合成
算法实现    ：无
引用全局变量：无
输入参数说明：aptImageInput[]   待处理的前景图像组信息
              ptMerge           图像合成信息

返回值说明  ：VIDEOSUCCESS：成功，VIDEOFAILURE：失败
举例或特别说明： TImage *aptImageInput[]   待处理的前景图像组信息中u8Pos须从1开始计数
                 图像合成指定背景信息中l32ImageType要求设定为场格式（FIELDFORMAT）
====================================================================*/
l32 ImageMerge_c(TImage *aptImageInput[], TMerge *ptMerge);

#define ImageMerge ImageMerge_c

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
l32 YUV422ToYUV420Init_c(void **ppvHandle, TYUV422ToYUV420Param *ptYUV422ToYUV420Param);


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
l32 YUV422ToYUV420_c(void *pvHandle, TImageInfo *ptImageSrc, TImageInfo *ptImageDst);


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
l32 YUV422ToYUV420SetParam_c(void *pvHandle, TYUV422ToYUV420Param *ptYUV422ToYUV420Param);

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

//图像YUV420转为YUV422
#define  YUV420To422      YUV420To422_c


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

//图像YUV422转为YUV420
#define  YUV422ToYUV420Init     YUV422ToYUV420Init_c
#define  YUV422ToYUV420         YUV422ToYUV420_c
#define  YUV422ToYUV420Close    YUV422ToYUV420Close_c
#define  YUV422ToYUV420SetParam YUV422ToYUV420SetParam_c

//图像Deblock
#define  ImageDeblockInit   ImageDeblockInit_c
#define  ImageDeblock       ImageDeblock_c
#define  ImageDeblockClose  ImageDeblockClose_c

//图像缩小和合并
#define  ZoomImageAndMerge420  ZoomImageAndMerge420_c
#define  IrregularZoomImageAndMerge420  IrregularZoomImageAndMerge420_c

//DM64PLUS下使用的函数定义
#elif defined(DM64PLUS_CODE) 

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
函数名      ：AddLogo_ti
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
l32 AddLogo_ti(TImageInfo *ptBackgroundImage, TLogoInfo *ptLogo);

//添加图标
#define   AddLogo   AddLogo_ti

/*====================================================================
函数名      ： DrawBanner_ti
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
l32 DrawBanner_ti(TImageInfo *ptImage, TBanner *ptBanner);

#define  DrawBanner  DrawBanner_ti

/*====================================================================
函数名	    ：DrawBannerAndLogo_ti
功能	    ：横幅、图标与背景图像的叠加,图标和背景图像可以移动
算法实现    ：无
引用全局变量：无
输入参数说明：aptLogoInput[]     输入的图标信息指针组
              aptBannerInput[]   输入的横幅信息指针组
              ptImage            背景图像信息指针
输出参数说明：ptImage->pu8YUV 叠加完毕的图像指针
返回值说明  ：VIDEOSUCCESS：成功，VIDEOFAILURE：失败
====================================================================*/
l32 DrawBannerAndLogo_ti(TLogo *aptLogoInput[], TBanner *aptBannerInput[], TImage *ptImage);

#define DrawBannerAndLogo DrawBannerAndLogo_ti

/*====================================================================
函数名	    ：ImageMerge_ti
功能	    ：画面合成
算法实现    ：无
引用全局变量：无
输入参数说明：aptImageInput[]   待处理的前景图像组信息
              ptMerge           图像合成信息

返回值说明  ：VIDEOSUCCESS：成功，VIDEOFAILURE：失败
举例或特别说明： TImage *aptImageInput[]   待处理的前景图像组信息中u8Pos须从1开始计数
                 图像合成指定背景信息中l32ImageType要求设定为场格式（FIELDFORMAT）
====================================================================*/
l32 ImageMerge_ti(TImage *aptImageInput[], TMerge *ptMerge);

#define ImageMerge ImageMerge_ti

/*====================================================================
函数名      ： imagezoom_ti
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
l32 ImageZoom_ti(TImageInfo *ptOld, TImageInfo *ptNew, u8 u8ImageQuality);

//图像缩放
#define ImageZoom ImageZoom_ti

/*====================================================================
函数名      ： YUV422ToYUV420Init_ti
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
l32 YUV422ToYUV420Init_ti(void **ppvHandle, TYUV422ToYUV420Param *ptYUV422ToYUV420Param);


/*====================================================================
函数名      ： YUV422ToYUV420_ti
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
l32 YUV422ToYUV420_ti(void *pvHandle, TImageInfo *ptImageSrc, TImageInfo *ptImageDst);


/*====================================================================
函数名      ： YUV422ToYUV420SetParam_ti
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
l32 YUV422ToYUV420SetParam_ti(void *pvHandle, TYUV422ToYUV420Param *ptYUV422ToYUV420Param);

/*====================================================================
函数名      ： YUV422ToYUV420Close_ti
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
l32 YUV422ToYUV420Close_ti(void *pvHandle);

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
l32 D1YUV422FieldPToN_ti(TImageInfo *ptInImage, TImageInfo *ptOutImage);

#define  D1YUV422FieldPToN  D1YUV422FieldPToN_ti

/*====================================================================
函数名      :	4CifYUV422FieldNToP_ti
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
l32 D1YUV422FieldNToP_ti(TImageInfo *ptInImage, TImageInfo *ptOutImage);

#define  D1YUV422FieldNToP  D1YUV422FieldNToP_ti

/*====================================================================
函数名      :	D1YUV422FieldNToP_ti
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
l32 D1YUV422FieldNToPOneBuffer_ti(TImageInfo *ptInImage, TImageInfo *ptOutImage);

#define  D1YUV422FieldNToPOneBuffer  D1YUV422FieldNToPOneBuffer_ti

/*====================================================================
函数名      :	D1YUV420FieldPToNOneBuffer_ti
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
l32 D1YUV420FieldPToNOneBuffer_ti(TImageInfo *ptInImage, TImageInfo *ptOutImage);

#define  D1YUV420FieldPToNOneBuffer  D1YUV420FieldPToNOneBuffer_ti

/*====================================================================
函数名      :	D1YUV420FieldNToPTwoBuffer_ti
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
l32 D1YUV420FieldNToPTwoBuffer_ti(TImageInfo *ptInImage, TImageInfo *ptOutImage);

#define  D1YUV420FieldNToPTwoBuffer  D1YUV420FieldNToPTwoBuffer_ti

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
l32 ProduceMosaic_ti(TImageInfo *ptInImage, TImageInfo *ptOutImage, 
				      l32 l32Left, l32 l32Top, l32 l32WindowWidth, l32 l32WindowHeight, l32 l32Granularity);

#define  ProduceMosaic  ProduceMosaic_ti

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
l32 MergePictureInit_ti(void **ppvHandle);

#define  MergePictureInit  MergePictureInit_ti

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
l32 MergePicture_ti(void *pvHandle, TImageInfo *ptBackgroundImage, TLogoInfo *ptLogo);

#define  MergePicture  MergePicture_ti

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
l32 MergePictureClose_ti(void *pvHandle);

#define  MergePictureClose  MergePictureClose_ti

/*====================================================================
函数名      :	ImageSizeChange_ti
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
l32 ImageSizeChange_ti(TImageInfo *ptInImage, TImageInfo *ptOutImage);

#define  ImageSizeChange  ImageSizeChange_ti

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
l32 YUV420toRGB_ti(u8 *pu8Dst, l32 l32RGBTop, l32 l32RGBLeft, l32 l32RGBWidth, l32 l32RGBHeight, u8 u8RGBType, TImageInfo *ptInImage);

#define  YUV420toRGB	YUV420toRGB_ti

/*====================================================================
函数名      ： YUV422ToYUV420AndTemporalFilterInit_ti
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
l32  YUV422ToYUV420AndTemporalFilterInit_ti(void **ppvHandle, TYUV422ToYUV420Param *ptYUV422To420Param);

#define  YUV422ToYUV420AndTemporalFilterInit YUV422ToYUV420AndTemporalFilterInit_ti

/*====================================================================
函数名      ： YUV422ToYUV420AndTemporalFilter_ti
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
l32 YUV422ToYUV420AndTemporalFilter_ti(void *pvHandle, u8 *pu8Src, u8 *pu8Dst);

#define  YUV422ToYUV420AndTemporalFilter YUV422ToYUV420AndTemporalFilter_ti

/*====================================================================
函数名      ： YUV422ToYUV420AndTemporalFilterSetParam_ti
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
l32 YUV422ToYUV420AndTemporalFilterSetParam_ti(void *pvHandle, TYUV422ToYUV420Param *ptYUV422To420Param);

#define  YUV422ToYUV420AndTemporalFilterSetParam  YUV422ToYUV420AndTemporalFilterSetParam_ti

/*====================================================================
函数名      ： YUV422ToYUV420AndTemporalFilterClose_ti
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
l32 YUV422ToYUV420AndTemporalFilterClose_ti(void *pvHandle);

#define  YUV422ToYUV420AndTemporalFilterClose YUV422ToYUV420AndTemporalFilterClose_ti

/*====================================================================
函数名      ： TemporalFilterInit_ti
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
l32 TemporalFilterInit_ti(void **ppvHandle, l32 l32Width, l32 l32Height);

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
l32 TemporalFilter_ti(void *ptTemporalFilter, u8 *pu8RawImage, u8 *pu8FilteredImage, u16 *pu16CurSum8x8);

/*====================================================================
函数名      ： TemporalFilterClose_ti
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
l32 TemporalFilterClose_ti(void *pvHandle);

/*====================================================================
函数名      ： TemporalFilterSetParam_ti
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
l32 TemporalFilterSetParam_ti(void *pvHandle, l32 l32Width, l32 l32Height, l32 l32Threshold);

/*====================================================================
函数名      ： TemporalFilterSetThreshold_ti
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
l32 TemporalFilterSetThreshold_ti(void *pvHandle, l32 l32Threshold);


/*====================================================================
函数名      ：ImageTypeChange_ti
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
l32 ImageTypeChange_ti(TImageInfo *ptOld, TImageInfo *ptNew);

//图像类型转换
#define ImageTypeChange ImageTypeChange_ti

/*====================================================================
函数名      ： YUYVToYUV420_ti
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
l32 YUYVToYUV420_ti(TImageInfo *ptOld, TImageInfo *ptNew);

#define YUYVToYUV420 YUYVToYUV420_ti

/*====================================================================
函数名  ： ImageDering_ti
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
l32 ImageDering_ti(TImageInfo *ptImage, l32 l32Quant);

/*====================================================================
函数名      ： YUV420To422_ti
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
l32  YUV420To422_ti(TImageInfo *ptYUV420, TImageInfo *ptYUV422);

//图像YUV420转为YUV422
#define  YUV420To422      YUV420To422_ti


/*====================================================================
函数名  ： ImageDeblockInit_ti
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
l32 ImageDeblockInit_ti(l32 l32Width);

/*====================================================================
函数名  ： ImageDeblock_ti
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
l32 ImageDeblock_ti(TImageInfo *ptOldImage, TImageInfo *ptNewImage, l32 l32Quant);

/*====================================================================
函数名  ： ImageDeblockClose_ti
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
l32 ImageDeblockClose_ti();

/*====================================================================
函数名  ： ZoomImageAndMerge420_ti
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
l32 ZoomImageAndMerge420_ti(TImageInfo *ptForegroundImage, TImageInfo *ptBackgroundImage, l32 l32Top, l32 l32Left, l32 l32ZoomType);

/*====================================================================
函数名  ： IrregularZoomImageAndMerge420_ti
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
l32 IrregularZoomImageAndMerge420_ti(TImageInfo *ptForegroundImage, TImageInfo *ptBackgroundImage, l32 l32Top, l32 l32Left, l32  l32Width, l32 l32Height);

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
l32 MotionSurveil_ti(TImageInfo *ptCurImage, TImageInfo *ptPreImage, u8 *pu8MotionLevel);

#define  MotionSurveil  MotionSurveil_ti

//图像的时域滤波
#define  TemporalFilterSetParam      TemporalFilterSetParam_ti
#define  TemporalFilterInit          TemporalFilterInit_ti
#define  TemporalFilter              TemporalFilter_ti
#define  TemporalFilterClose         TemporalFilterClose_ti
#define  TemporalFilterSetThreshold  TemporalFilterSetThreshold_ti

//去振铃效应
#define   ImageDering   ImageDering_ti

//图像YUV422转为YUV420
#define  YUV422ToYUV420Init     YUV422ToYUV420Init_ti
#define  YUV422ToYUV420         YUV422ToYUV420_ti
#define  YUV422ToYUV420Close    YUV422ToYUV420Close_ti
#define  YUV422ToYUV420SetParam YUV422ToYUV420SetParam_ti

//图像Deblock
#define  ImageDeblockInit   ImageDeblockInit_ti
#define  ImageDeblock       ImageDeblock_ti
#define  ImageDeblockClose  ImageDeblockClose_ti

//图像缩小和合并
#define  ZoomImageAndMerge420  ZoomImageAndMerge420_ti
#define  IrregularZoomImageAndMerge420  IrregularZoomImageAndMerge420_ti

#endif

#ifdef __cplusplus
};
#endif 

#endif // _IMAGEPROCESS_H_

