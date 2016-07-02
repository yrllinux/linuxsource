
#ifndef VIDEO_COMMON_DEFINES

#define VIDEO_COMMON_DEFINES


//图像格式定义

//帧格式
#define FRAMEFORMAT   0x01

//场格式
#define FIELDFORMAT   0x02

//解码图象输出设置
//图象：解码图象不做拉升处理
#define IMAGE_SIZE_NO_CHANGE			0

//图象:720*576
#define IMAGE_SIZE_720576 				1

//图象:1/4 720*576(352*288)
#define IMAGE_SIZE_QUARTER_720576 		2

//图象:1/9 720*576(240*192)
#define IMAGE_SIZE_NINTH_720576 		3

//图象:1/16 720*576(176*144)
#define IMAGE_SIZE_SIXTEENTH_720576 	4

//图象:720*480
#define IMAGE_SIZE_720480 				5

//图象:1/4 720*480(352*240)
#define IMAGE_SIZE_QUARTER_720480 		6

//图象:1/9 720*480(240*160)
#define IMAGE_SIZE_NINTH_720480 		7

//图象:1/16 720*480(176*120)
#define IMAGE_SIZE_SIXTEENTH_720480 	8

//图象:640*480
#define IMAGE_SIZE_640480				9

//图象:800*600
#define IMAGE_SIZE_800600				10

//图象:1024*768
#define IMAGE_SIZE_1024768				11

//图象:96*80
#define IMAGE_SIZE_SIXTEENTH_352288		12

//图象:112*96
#define IMAGE_SIZE_NINTH_352288			13

//图象:用于画面合成前的cif到D1的缩放（画面合成的背景是CIF）
#define IMAGE_SIZE_720576_MERGECIF      14

//图像的编解码格式(I帧或者P帧)
//I帧
#define IPICTURE  0x01

//P帧
#define PPICTURE  0x02

//图像缩放质量定义
#define  QUALITYON     0
#define  QUALITYOFF    1

//返回成功或者失败的定义
#ifdef _LINUX_
typedef enum
{
	VIDEOFAILURE = -1,
	VIDEOSUCCESS,  
    VIDEOSKIP
}imageprocessDrvError_t;
#else

//success
#define  VIDEOSUCCESS		0

//failure
#define  VIDEOFAILURE      -1

//frame not coded
#define	 VIDEOSKIP			1

#endif

//图像类型定义
#define  YUV444     444
#define  YUV422     422
#define  YUV420     420
#define  YUYV		4201		

//rgb类型定义
typedef enum
{
	RGB1   = 1,
	RGB4   = 4,  
    RGB8   = 8,
	RGB565 = 16,
	RGB555 = 15,
	RGB24  = 24,
	RGB32  = 32//每个数据单元所占用的实际大小
}RGBTYPE_t;


//生成测试图像的不同图案
//地球形状
#define  GLOBOSITY   4

//不同间隔黑白条
#define  BLACKWHITEBAR  5

//由白逐渐加到黑
#define   WHITETOBLACK  6

//七彩色,彩虹
#define   RAINBOW   7

//时域滤波时的阈值选择
#define   THRESHIGH   0XFF
#define   THRESLOW    0x11

//当前支持图像的最大宽度和高度
#define   MAXFRAMEWIDTH   1024
#define   MAXFRAMEHEIGHT  768

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


#endif //VIDEO_COMMON_DEFINES

