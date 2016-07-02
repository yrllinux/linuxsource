/******************************************************************************
模块名	    ：img_bannerlogo_public
文件名	    ：img_bannerlogo_public.h            
相关文件	：
文件实现功能：定义横幅,图标结构体
作者		：zwenyi
版本		：1.0
-------------------------------------------------------------------------------
修改记录:
日  期			版本			修改人		走读人    修改内容
2007/09/14		1.0				zwenyi                  创建
******************************************************************************/
#ifndef _IMG_BANNERLOGO_PUBLIC_H_
#define _IMG_BANNERLOGO_PUBLIC_H_

#include "videodefines.h"

#ifdef __cplusplus
extern "C"
{
#endif

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
	u8 *pu8YUV;                //前景图象数据指针地址(图像格式为YUV422时,
	                           //分配的空间大小为l32Width*l32Height*2,图像格式为YUV420时,分配的空间大小为l32Width*l32Height*3/2)
	u8 *pu8Alpha;              //前景图像的alpha通道指针(分配空间的大小和分配图像数据的大小一样)
	u8 *pu8Beta;               //对应背景图像的beta通道指针(分配空间的大小和分配图像数据的大小一样)
	u8 u8Transparency;         //图标透明系数
	l32 l32Width;              //前景图像的宽度
	l32 l32Height;             //前景图像的高度	
	l32 l32YUVType;            //图标图像格式(YUV422或者YUV420)
	l32 l32Top;                //前景图象位置偏移
	l32	l32Left;               //前景图象位置偏移
	l32 l32TopCutLine;         //前景图像上边裁去的行数
	l32 l32BottomCutLine;      //前景图像下边裁去的行数
	l32 l32LeftCutLine;        //前景图像左边裁去的列数
	l32 l32RightCutLine;       //前景图像右边裁去的列数
	l32 l32IsInterlace;        //前景图像的类型(帧格式或者场格式)
	l32 l32LogoRefPicWidth;    //图标生成的参考图象高度，必须是8的倍数，此值用于计算图标横向缩放比例，如果添加图标不需要缩放，此值等于背景图像宽度
	l32 l32LogoRefPicHeight;   //图标生成的参考图象宽度，必须是4的倍数，此值用于计算图标纵向缩放比例，如果添加图标不需要缩放，此值等于背景图像高度	
	l32 l32IsAdaptiveColor;    //是否根据底色改变图标的颜色,1表示需要改变图标的颜色,其他表示不需要改变图标的颜色
    TFontInfo tFontInfo;       //记录图标中字的信息结构体
} TLogoInfo;

//图标信息的结构体
typedef struct tLogo
{
	u8 *pu8YUV;                //前景图象数据指针地址(图象格式为YUV422时,分配的空间大小为l32Width*l32Height*2,图象格式为YUV420时,分配的空间大小为l32Width*l32Height*3/2)
	u8 *pu8Alpha;     		   //前景图象的alpha通道指针(分配空间的大小和分配图象数据的大小一样)
	u8 *pu8Beta;			   //对应背景图象的beta通道指针(分配空间的大小和分配图象数据的大小一样)
	u32	u32Width;              //图标图象的宽度, 必须保证输入值在乘以背景图像宽度再除以u32LogoRefPicWidth后要是8的倍数
	u32	u32Height;             //图标图象的高度, 必须保证输入值在乘以背景图像高度再除以u32LogoRefPicHeight后要是4的倍数
	l32	l32YUVType;            //图标图象格式(YUV422或者YUV420)
	l32	l32IsInterlace;        //前景图象的类型(帧格式或者场格式)
	u32 u32LogoRefPicWidth;    //图标添加参考图像的宽度，必须是8的倍数，此值用于计算图标横向缩放比例，如果添加图标不需要缩放，此值等于背景图像宽度
	u32 u32LogoRefPicHeight;   //图标添加参考图像的高度，必须是4的倍数，此值用于计算图标纵向缩放比例，如果添加图标不需要缩放，此值等于背景图像高度	
	u8 u8Transparency;         //图标透明系数
	l32 l32Top;				   //图标图象垂直位置偏移（相对于显示窗口）, 必须保证输入值在乘以背景图像高度再除以u32LogoRefPicHeight后要是2的倍数，背景是场图像时为4的倍数
	l32	l32Left;        	   //图标图象水平位置偏移（相对于显示窗口）, 必须保证输入值在乘以背景图像宽度再除以u32LogoRefPicWidth后要是4的倍数
	s16	s16StepH;			   //图标水平运动速度（每帧移动象素数，正向右负向左）, 必须保证输入值在乘以背景图像宽度再除以u32LogoRefPicWidth后要是偶数
	s16	s16StepV;			   //图标垂直运动速度（每帧移动象素数，正向上负向下）, 必须保证输入值在乘以背景图像高度再除以u32LogoRefPicHeight后要是偶数,对于场图像需要是4的倍数
	l32	l32ShowTimes;		   //图标显示次数（负：一直显示；0：不显示；正：图标有速度时为循环显示次数，图标无速度时调用次数）
	l32	l32PauseIntervalH;	   //图标水平运动暂停间隔（象素数）（正：有暂停； 非正：无暂停）
	l32	l32PauseIntervalV;	   //图标垂直运动暂停间隔（象素数）（正：有暂停； 非正：无暂停）
	l32	l32PauseTime;		   //图标暂停时间（调用次数）
	l32	l32WindowTop;		   //显示窗口垂直位置, 必须保证输入值在乘以背景图像高度再除以u32LogoRefPicHeight后要是偶数
	l32	l32WindowLeft;		   //显示窗口水平位置
	l32	l32WindowWidth;	       //显示窗口宽度
	l32	l32WindowHeight;	   //显示窗口高度
	l32 l32IsAdaptiveColor;    //是否根据底色改变图标的颜色,1表示需要改变图标的颜色,其他表示不需要改变图标的颜色
    TFontInfo tFontInfo;       //记录图标中字的信息结构体
	u32	u32Reserved;		   //保留参数
}TLogo;			//图标

//图标信息使能结构体
typedef struct 
{
    u32 u32LogoReservedType; //TLogo结构体保留字段扩展类型
    l32 l32LogoEnable;       //Logo设置是否使能（用在bannerlogo模块中）
    l32 l32LogoShow;         //Logo是否显示（用在bannerlogo模块中）
    u32 u32Reserved;         //保留参数
}TLogoEnableParam;	

//横幅信息的结构体
typedef struct tBanner
{
	l32		l32Width;			//横幅宽度
	l32		l32Height;			//横幅高度
	l32		l32Top;				//横幅左上角垂直坐标
	l32		l32Left;			//横幅左上角水平坐标
	l32     l32BannerRefPicWidth;    //横幅添加参考图像的宽度
	l32     l32BannerRefPicHeight;   //横幅添加参考图像的高度
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
	l32	 l32IsInterlace;  //图像的类型(帧格式或者场格式)
    l32 l32IsInterleaved;   //逐行扫描还是隔行扫描
	l32  l32YUVType;      //图标图像格式(YUV422或者YUV420)	
}TBackgroundImage;

//图标参数
typedef struct 
{
    TLogoInfo tLogo;           //图标参数
	TBackgroundImage tImage;   //背景图像参数
} TImageLogoParam;

//图标模块状态查询结构体
#define TImageLogoStatusParam TImageLogoParam

//横幅参数
typedef struct 
{
	TBanner tBanner;           //横幅参数
    TBackgroundImage tImage;   //背景图像参数
} TImageBannerParam;

//横幅模块状态查询结构体
#define TImageBannerStatusParam TImageBannerParam

//横幅图标滚动参数
typedef struct
{
	TLogo atLogo[MAX_LOGO_NUM];                 //图标参数结构数组
    TBanner atBanner[MAX_LOGO_NUM];             //横幅参数结构数组
	TBackgroundImage tImage;                    //背景图像参数结构
	l32 l32LogoNum;                             //图标个数
	l32 l32BannerNum;                           //横幅个数
} TImageBannerLogoParam;

//滚动横幅模块状态查询结构体
#define TImageBannerLogoStatusParam TImageBannerLogoParam

typedef enum 
{ 
    ZOOM_BY_REF_IMG = 0,                      //LOGO按参考图像与背景图像相应宽高比缩放
    ZOOM_BY_SELF                              //LOGO自缩放，目标宽高在TLogoZoomInfo中
} ELogoZoomMode; 

typedef struct 
{ 
    l32 l32DstLogoWidth;                      //最终添加Logo宽度 
    l32 l32DstLogoHeight;                     //最终添加Logo高度 
    u32 u32Reserved;                          //保留参数 
} TLogoZoomInfo; 

typedef struct 
{ 
    TLogoInfo tLogo[MAX_LOGO_NUM];               //图标参数结构数组 
    TBackgroundImage tImage;                     //背景图像参数结构 
    l32 l32LogoNum;                              //图标个数 
    ELogoZoomMode eLogoZoomMode;                 //Logo缩放模式(ELogoZoomMode) 
    TLogoZoomInfo atLogoZoomInfo[MAX_LOGO_NUM];  //Logo缩放参数结构数组 
    u32 u32Reserved;                             //保留参数 
} TImageCommonAddLogoParam; 

#define TImageGM8180AddLogoParam TImageCommonAddLogoParam

//图标模块状态查询结构体
#define TImageGM8180AddLogoStatusParam TImageCommonAddLogoParam

//图标输入结构
typedef struct
{
	u8 *pu8YUV;	      //输入输出图像数据
} TImageLogoInput;

//图标输出结构
typedef struct
{
	u32	u32Reserved;  //保留参数
} TImageLogoOutput;

//横幅输入结构
typedef struct
{
	u8 *pu8YUV;	      //输入输出图像数据
} TImageBannerInput;

//横幅输出结构
typedef struct
{
	u32	u32Reserved;  //保留参数
} TImageBannerOutput;

//滚动横幅图标输入结构
typedef struct
{
	u8 *pu8YUV;	      //输入输出图像数据
} TImageBannerLogoInput;

//滚动横幅图标输出结构
typedef struct
{
	u32	u32Reserved;  //保留参数
} TImageBannerLogoOutput;

//GM8180上加多个LOGO的输入结构
typedef struct
{
    u8 *pu8YUV;	      //输入输出图像数据
} TImageCommonAddLogoInput;

#define TImageGM8180AddLogoInput TImageCommonAddLogoInput

//GM8180上加多个LOGO的输出结构
typedef struct
{
    u32	u32Reserved;  //保留参数
} TImageCommonAddLogoOutput;

#define TImageGM8180AddLogoOutput TImageCommonAddLogoOutput

#ifdef __cplusplus
}
#endif

#endif //_IMG_BANNERLOGO_PUBLIC_H_
