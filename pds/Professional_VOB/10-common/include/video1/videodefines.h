/******************************************************************************
模块名	    ： VIDEODEFINES
文件名	    ： VIDEODEFINES.h
相关文件	： 无
文件实现功能： 声明编解码用到的宏定义
---------------------------------------------------------------------------------------
修改记录:
日  期		版本		修改人		走读人    修改内容
01/18/2007  1.0          赵波                  创建
01/18/2008  1.1         王国忠                 增加图标背景图像扫描方式定义
                                               当前支持图像的最大宽度和高度改为1920和1080
                                               增加图像720p和1080p宽高的宏定义
**************************************************************************************/
#ifndef VIDEO_DEFINES

#define VIDEO_DEFINES

#ifdef __cplusplus
extern "C" {
#endif

#define ALIGN_DEFAULT 8 //8字节对齐

//打包方式
//h261:no rtp packet info
#define H261_PACKET_NO_RTPINFO		0

//h261:has rtp packet info
#define H261_PACKET_RTPINFO		    1

//h263:263
#define H263_PACKET_BY_263			0

//h263:263+
#define H263_PACKET_BY_263PLUS		1

//h264:annexb
#define H264_PACKET_BY_ANNEXB		0

//h264:rtp
#define H264_PACKET_BY_RTP			1

//编解码器类型
typedef enum
{
    VCODEC_H261 = 1,  //H261编解码器类型
    VCODEC_H263,      //H263编解码器类型
    VCODEC_H264,      //H264编解码器类型
    VCODEC_MPEG2,     //MPEG2编解码器类型
    VCODEC_MPEG4,     //MPEG4编解码器类型
    VCODEC_VIDUNIT,   //VIDUNIT类型,获取版本信息
    VCODEC_JPEG,      //JPEG编解码器类型
    VCODEC_H264_HP,   //H264HP编解码器类型（三个统一调用）
    VCODEC_PNG,
    VCODEC_GPU_H264,  //H.264 GPU编解码类型
    VCODEC_NONE       //不支持的编解码器类型
}CODECTYPE;

//视频制式
typedef enum
{
    VID_PAL_MODE = 1,     //PAL制式,每秒25帧
    VID_NTSC_MODE,        //NTSC制式,每秒30帧
    VID_VGA_MODE,         //PC桌面采集模式
    VID_720P_MODE,        //720P视频采集模式
    VID_1080P_MODE,       //1080P视频采集模式
    VID_1080I_MODE,       //1080I视频采集模式
    VID_TRANSCODE_MODE,   //转码视频采集模式
    VID_NONE_MODE         //不支持的制式
}VIDEOMODETYPE;

//质量等级宏定义
#define ENC_QUALITY_LEVEL_AUTO        0        //自动选择编码级别
#define ENC_QUALITY_LEVEL_1         (1 << 8)  //最大速度级别编码
#define ENC_QUALITY_LEVEL_2         (1 << 9)  //快速级别编码
#define ENC_QUALITY_LEVEL_3         (1 << 10) //中等速度编码
#define ENC_QUALITY_LEVEL_4         (1 << 11) //慢速级别编码
#define ENC_QUALITY_LEVEL_5         (1 << 12) //中等质量级别编码
#define ENC_QUALITY_LEVEL_6         (1 << 13) //较好质量级别编码
#define ENC_QUALITY_LEVEL_7         (1 << 14) //最好质量级别编码
#define ENC_QUALITY_LEVEL_11         (1 << 16) //HP编码单线程
#define ENC_QUALITY_LEVEL_13         (1 << 18) //HP编码多线程

//质量优先
#define ENC_QUALITY_PRIOR  ENC_QUALITY_LEVEL_4

//速度优先
#define ENC_SPEED_PRIOR  ENC_QUALITY_LEVEL_2

//HP编码单线程
#define ENC_HP_ST ENC_QUALITY_LEVEL_11
//HP编码多线程
#define ENC_HP_MT ENC_QUALITY_LEVEL_13

//解码质量等级
#define DEC_QUALITY_LEVEL_AUTO   0          //自动选择解码级别
#define DEC_SPEED_PRIOR         (1 << 1)    //解码速度优先
#define DEC_QUALITY_PRIOR       (1 << 2)    //解码质量优先

//画面合成最大的图像数
#define MAX_IMAGES_IN_ONE		16

//添加图标和横幅的最大个数
#define MAX_LOGOS_IN_IMAGE		32
#define MAX_BANNERS_IN_IMAGE	32

//移动帧率灵敏度
typedef enum
{
    MOTION_DETECT_SENSITIVE_1 = 1,  //灵敏度级别1，非常不灵敏
    MOTION_DETECT_SENSITIVE_2,      //灵敏度级别2，不灵敏
    MOTION_DETECT_SENSITIVE_3,      //灵敏度级别3，灵敏
    MOTION_DETECT_SENSITIVE_4,      //灵敏度级别4，比较灵敏
    MOTION_DETECT_SENSITIVE_5       //灵敏度级别5，非常灵敏
}MOTIONDETECTSENSITIVELEVEL;

#define MAX_FINE_TUNING_VALUE 10    //移动侦测微调值的最大值

#define MIN_FINE_TUNING_VALUE -10   //移动侦测微调值的最小值

//移动侦测区域最大个数
#define MAX_MOTION_SURVEIL_NUM 16

//自适应台标一次最多的字数
#define MAX_CHAR_IN_ADAPTIVE_LOGO 1000

//图像格式定义
//帧格式
#define FRAME_FORMAT   0x01

//场格式
#define FIELD_FORMAT   0x02

//隔行采集的帧格式格式
#define FRAME_I_FORMAT   0x03

//多路解码状态标志
typedef enum
{
    NEED_DEC = 0x01,  //需要解码
    NO_NEED_DEC,      //不需解码，但BUF保留
    INVALID_DEC       //不解码，BUF不保留
}MULTI_DEC_STATUS_TYPE;

//图标背景图像扫描方式定义
typedef enum
{
    IMAGE_INTERLACE = 0x10, //隔行扫描
    IMAGE_PROGRESIVE        //逐行扫描
}IMAGE_SCAN_TYPE;

//图像的编解码格式(I帧或者P帧)
typedef enum
{
	I_PICTURE = 0x01,  //I帧
	P_PICTURE,         //P帧
	SKIP_PICTURE,      //跳过帧
	DROP_PICTURE,      //丢帧
    B_PICTURE          //B帧
}PICTURETYPE;

//图像缩放类型定义
typedef enum
{
	QUALITY_ON = 0,
	QUALITY_OFF,
    BILINEAR,
	IMAGE_ZOOM_TYPE_MAX
}ZOOMTYPE;

//PN制转换
//输入输出缓存是否为一块
#define SAME_BUFFER        1
#define DIFFERENT_BUFFER   0

//图像的制式
#define KDC_NTSC	78
#define KDC_PAL	    80

//版本信息和错误码信息的长度
#define INFO_BUFFER_LEN 100

//返回成功或者失败的定义
//success
//解码成功并输出
#define  VIDEO_SUCCESS		0
//解码成功不输出
#define  VIDEO_NO_OUT		1
//failure
#define  VIDEO_FAILURE      -1
//错误遮蔽不能处理
#define VID_DEC_EC_UNSUPPORT -2

//图像类型定义
#define  YUV400     400    //Monochrome
#define  YUV444     444    //全采样，即每4个Y，配上4个U，还有4个V
#define  YUV422     422    //UYVY方式 每4个Y，配2个U，2个V, YUV在同一平面中 U0 Y0 V0 Y1 U1 Y2 V1 Y3 …
#define  YUV422P    4221   //YUV Plane方式
#define  YUV420     420    //分三个平面，Y平面，U平面，V平面，其中U平面和V平面均为Y平面的1/4  Y0 Y1 Y2 Y3.. U0 U1.. V0 V1..
#define  YUYV		4201   //每4个Y，配2个U，2个V, YUV在同一平面中,   Y0 U0 Y1 V0 Y2 U1 Y3 V1 …
#define  NV12       42012  //分两个平面，即Y单独平面和UV交织的平面 Y0 Y1...Yn   U0 V0 U1 V1...
#define  GM8180_YUV420     81803
#define  GM8180_YUV420_2D  81802
#define  GM8180_YUV420_3D  81803

//rgb类型定义
typedef enum
{
	RGB1   = 1,
	RGB4   = 4,
    RGB8   = 8,
	RGB565 = 16,
	RGB555 = 15,
	RGB24  = 24,
	RGB32  = 32 //每个数据单元所占用的实际大小
}RGBTYPE_t;

//生成图像的图案类型
typedef enum
{
    GLOBOSITY = 1,  //地球形状
    RAINBOW,        //七彩色,彩虹
    PURE_COLOR,     //纯色
    RAINBOW_HD,         //七彩色,标准颜色彩虹
    PATTERN_NONE    //不支持的图案类型
}PATTERNTYPE;

//滤波器类型
typedef enum
{
	FILTER_VERTICAL = 1,  //垂直滤波方式,用来去除老式电视机的闪烁效应
	FILTER_NONE           //不支持的滤波类型
}FILTERTYPE;

//当前支持图像的最大宽度和高度
#define   MAX_FRAME_WIDTH   1920
#define   MAX_FRAME_HEIGHT  1088

//一帧图像可以添加马赛克的最大值
#define MAX_MOSAIC_IN_ONE_FRAME 32

//图标状态个数
#define LOGO_STATUS_LEN		20

//添加图标最多个数
#ifndef ARCH_ARM_IOS
#define MAX_LOGO_NUM		36
#else
#define MAX_LOGO_NUM		32
#endif

//YUV图像宽高的宏定义
#define YUV_P_D1_WIDTH          720		  // YUV图像P制D1图像的宽度
#define YUV_P_D1_HEIGHT			576		  // YUV图像P制D1图像的高度
#define YUV_P_4CIF_WIDTH		704		  // YUV图像P制4CIF图像的宽度
#define YUV_P_4CIF_HEIGHT		576		  // YUV图像P制4CIF图像的高度
#define YUV_P_2CIF_WIDTH		352		  // YUV图像P制2CIF图像的宽度
#define YUV_P_2CIF_HEIGHT		576		  // YUV图像P制2CIF图像的高度
#define YUV_P_CIF_WIDTH			352		  // YUV图像P制CIF图像的宽度
#define YUV_P_CIF_HEIGHT		288		  // YUV图像P制CIF图像的高度
#define YUV_P_NINETH_D1_WIDTH   240       // YUV图像P制240x192图像的宽度
#define YUV_P_NINETH_D1_HEIGHT  192       // YUV图像P制240x192图像的高度
#define YUV_N_NINETH_D1_WIDTH   240       // YUV图像N制240x160图像的宽度
#define YUV_N_NINETH_D1_HEIGHT  160       // YUV图像N制240x160图像的高度
#define YUV_P_QCIF_WIDTH		176		  // YUV图像P制QCIF图像的宽度
#define YUV_P_QCIF_HEIGHT		144		  // YUV图像P制QCIF图像的高度
#define YUV_P_NINETH_CIF_WIDTH  112	      // YUV图像P制112x96图像的宽度
#define YUV_P_NINETH_CIF_HEIGHT 96		  // YUV图像P制112x96图像的高度

#define YUV_N_D1_WIDTH			720		  // YUV图像N制D1图像的宽度
#define YUV_N_D1_HEIGHT			480		  // YUV图像N制D1图像的高度
#define YUV_N_4CIF_WIDTH		704		  // YUV图像N制4CIF图像的宽度
#define YUV_N_4CIF_HEIGHT		480		  // YUV图像N制4CIF图像的高度
#define YUV_N_2CIF_WIDTH		352		  // YUV图像N制2CIF图像的宽度
#define YUV_N_2CIF_HEIGHT		480		  // YUV图像N制2CIF图像的高度
#define YUV_N_CIF_WIDTH			352		  // YUV图像N制CIF图像的宽度
#define YUV_N_CIF_HEIGHT		240		  // YUV图像N制CIF图像的高度
#define YUV_N_QCIF_WIDTH		176		  // YUV图像N制QCIF图像的宽度
#define YUV_N_QCIF_HEIGHT		120		  // YUV图像N制QCIF图像的高度
#define YUV_N_NINETH_CIF_WIDTH  112	      // YUV图像N制112x80图像的宽度
#define YUV_N_NINETH_CIF_HEIGHT 80		  // YUV图像N制112x80图像的高度

#define YUV_WXGA_WIDTH          1280      // YUV图像WXGA图像的宽度
#define YUV_WXGA_HEIGHT         800       // YUV图像WXGA图像的高度
#define YUV_XGA_WIDTH			1024	  // YUV图像XGA图像的宽度
#define YUV_XGA_HEIGHT			768		  // YUV图像XGA图像的高度
#define YUV_SVGA_WIDTH			800	      // YUV图像SVGA图像的宽度
#define YUV_SVGA_HEIGHT			600		  // YUV图像SVGA图像的高度
#define YUV_VGA_WIDTH			640	      // YUV图像VGA图像的宽度
#define YUV_VGA_HEIGHT			480	      // YUV图像VGA图像的高度
#define YUV_SUBQCIF_WIDTH		128   	  // YUV图像SUBQCIF图像的宽度
#define YUV_SUBQCIF_HEIGHT		96   	  // YUV图像SUBQCIF图像的高度

#define YUV_720P_WIDTH          1280      // YUV图像720P图像的宽度
#define YUV_720P_HEIGHT         720       // YUV图像720P图像的高度
#define YUV_1080P_WIDTH         1920      // YUV图像1080P图像的宽度
#define YUV_1080P_HEIGHT        1080      // YUV图像1080P图像的高度

//声明图像处理类型
typedef enum
{
    IMG_YUV422_TO_YUV420 = 1, //图像YUV422转为图像YUV420
    IMG_YUV420_TO_YUV422,     //图像YUV420转为图像YUV422
    IMG_YUV420_TO_RGB,        //图像YUV420转为RGB图像
    IMG_YUV422_SPLIT,         //图像YUV422拆分为4路YUV420图像
    IMG_ZOOM,                 //图像YUV420的缩小、放大
    IMG_MERGE,                //图像YUV420的画面合成
    IMG_BMPTOYUVALPHA,        //BMP图像转为YUV图像并生成Alpha通道
    IMG_LOGO,                 //加台标
    IMG_BANNER,               //加横幅
    IMG_BANNER_LOGO,          //滚动横幅
    IMG_GENERATE_IMAGE,       //图像生成
    IMG_MOSAIC,               //加马赛克
    IMG_PN_CHANGE,            //图像P制和N制转换
    IMG_ADD_IMAGE,            //画中画
    IMG_YUV422_VGA_ZOOM,      //VGA的YUV422图像缩放
    IMG_ADAPT_16vs9,          //图像适配成16vs9
    IMG_IMGUNIT,              //图像库版本信息类型
    IMG_FILTER_OSD,           //临时增加滤波OSD的类型
    IMG_MULTI_DEC,            //多路解码
    IMG_APT_ZOOM_MOD,         //根据上层设置参数缩放图像并加黑边
    IMG_MERGE_ZOOM,           //图像缩放并画面合成
    IMG_MULTI_DEC_NEW,        //新的多路解码
    IMG_YUV422_ZOOM,          //YUV422图像缩放
    IMG_RESIZER,              //resizer缩放模块
    IMG_MULTI_ENC,            //多路编码
    IMG_RGB_ZOOM,             //RBG565缩放模块
    IMG_RENOVATION,           //图像刷新模块
    IMG_MOTION_DETECT_GM8180, //GM8180芯片运动侦测模块
    IMG_GM8180_TO_YUV420,     //GM8180与YUV420之间的互换
    IMG_COMMON_ADDLOGO,       //GM8180底图上加LOGO
    IMG_GM8180ADDMOSAIC,      //GM8180底图上加MOSAIC
    IMG_DEINTERLACE,          //Deinterlace处理
    IMG_FILTER,               //图像滤波模块
    IMG_DUAL_ENC,             //双流编码模块（同源）
    IMG_SHARP,                //图像锐化模块
    IMG_RGBTOYUV,             //RGB转YUV420模块
    IMG_TEMPORAL_FILTER,      //图像时域滤波模块
	IMG_MOTION_DETECT,        //移动侦测模块
	IMG_MOTION_DETECT_DM365,  //DM365芯片运动侦测模块
    IMG_HQ3DIIR_FILTER,       //图像HQ3dIIR滤波模块
    IMG_POSTDENOISE_HQ3D,     //基于HQ3D算法的图像后处理降噪
    IMG_ROTATE,               //图像旋转(包括镜像)
    IMG_HAZEREMOVE,           //图像去雾模块	
    IMG_PROCESS_NONE          //图像处理不存在的类型
}ImageProcessType;
#define IMG_GM8180ADDLOGO IMG_COMMON_ADDLOGO

#define PIC_MERGE_NUM_MAX 64 //画面合成路数的最大值

//画面合成类型定义
typedef enum
{
    PIC_NO_MERGE = 0,       //无画面合成
    PIC_MERGE_ONE,          //一画面合成
    PIC_MERGE_VTWO,         //大小相等两画面合成
    PIC_MERGE_ITWO,         //一大一小两画面合成
    PIC_MERGE_THREE,        //三画面合成
    PIC_MERGE_FOUR,         //大小相等四画面合成
    PIC_MERGE_SFOUR,        //一大三小四画面合成
    PIC_MERGE_SIX,          //六画面合成
    PIC_MERGE_SEVEN,        //七画面合成
    PIC_MERGE_EIGHT,        //八画面合成
    PIC_MERGE_NINE,         //九画面合成
    PIC_MERGE_TEN,          //十画面合成
    PIC_MERGE_TEN_2_8,      //十画面合成(两大八小)
    PIC_MERGE_TEN_1_9,      //十画面合成(一大九小)
    PIC_MERGE_TWELVE,       //十二画面合成
    PIC_MERGE_THIRTEEN,     //十三画面合成
    PIC_MERGE_FOURTEEN,     //十四画面合成(一大十三小)
    PIC_MERGE_SIXTEEN,      //十六画面合成
    PIC_MERGE_SIXTEEN_1_15, //十六画面合成(一大十五小)
    PIC_MERGE_TWENTYFIVE,   //二十五画面合成
    PIC_MERGE_THIRTYSIX,    //三十六画面合成
    PIC_MERGE_FORTYNINE,    //四十九画面合成
    PIC_MERGE_SIXTYFOUR,    //六十四画面合成
    PIC_MERGE_MAX           //画面合成方式最大值
} PicMergeStyle;

//merge_zoom模块画面合成类型定义
//模块名_画面合成路数_[(位置)等分画面][(位置)大画面_(位置)小画面]
//大小和位置都均分画面如PIC_MERGE_ZOOM_M9，大小均分但位置不均分如PIC_MERGE_ZOOM_M3_T1，大小位置都不均分如PIC_MERGE_ZOOM_M6_2_B4
//缩写含义：T:Top,B:Bottom,L:Left,R:Right
typedef enum
{
    PIC_MERGE_ZOOM_M0 = 0,              //无画面合成
    PIC_MERGE_ZOOM_M1 = 100,            //一画面合成(原PIC_MERGE_ONE)
    PIC_MERGE_ZOOM_M2 = 200,            //两画面合成(原PIC_MERGE_VTWO)
    PIC_MERGE_ZOOM_M2_1_BR1,            //两画面合成(一大一小，小画面在大画面内的右下角，原PIC_MERGE_ITWO)
    PIC_MERGE_ZOOM_M3_T1 = 300,         //三画面合成(大小相等，一上两下，原PIC_MERGE_THREE)
    PIC_MERGE_ZOOM_M3_B1,               //三画面合成(大小相等，两上一下)
    PIC_MERGE_ZOOM_M3_1_B2,             //三画面合成(一大两小，小画面在下)
    PIC_MERGE_ZOOM_M3_1_T2,             //三画面合成(一大两小，小画面在上)
    PIC_MERGE_ZOOM_M3_1_R2,             //三画面合成(一大两小，左大画面，右小画面)
    PIC_MERGE_ZOOM_M4 = 400,            //四画面合成(原PIC_MERGE_FOUR)
    PIC_MERGE_ZOOM_M4_1_R3,             //四画面合成(一大三小，小画面在右，原PIC_MERGE_SFOUR)
    PIC_MERGE_ZOOM_M5_1_R4 = 500,       //五画面合成(一大四小，小画面在右)
    PIC_MERGE_ZOOM_M5_1_D4,             //五画面合成(一大四小，小画面在下)
    PIC_MERGE_ZOOM_M6 = 600,            //六画面合成(大小相等，上三下三)
    PIC_MERGE_ZOOM_M6_1_5,              //六画面合成(一大五小，原PIC_MERGE_SIX)
    PIC_MERGE_ZOOM_M6_2_B4,             //六画面(两大四小，小画面在下)
    PIC_MERGE_ZOOM_M6_1_B5,             //六画面(一大五小，小画面在下)
    PIC_MERGE_ZOOM_M7_3_TL4 = 700,      //七画面合成(三大四小，四个小画面在左上角)
    PIC_MERGE_ZOOM_M7_3_TR4,            //七画面合成(三大四小，四个小画面在右上角)
    PIC_MERGE_ZOOM_M7_3_BL4,            //七画面合成(三大四小，四个小画面在左下角)
    PIC_MERGE_ZOOM_M7_3_BR4,            //七画面合成(三大四小，四个小画面在右下角，原PIC_MERGE_SEVEN)
    PIC_MERGE_ZOOM_M7_3_BLR4,           //七画面合成(三大四小，四个小画面在下面左右两侧)
    PIC_MERGE_ZOOM_M7_3_TLR4,           //七画面合成(三大四小，四个小画面在上面左右两侧)
    PIC_MERGE_ZOOM_M8_1_7 = 800,        //八画面合成(一大七小，原PIC_MERGE_EIGHT)
    PIC_MERGE_ZOOM_M9 = 900,            //九画面合成(原PIC_MERGE_NINE)
    PIC_MERGE_ZOOM_M10_2_R8 = 1000,     //十画面合成(两大八小，八个小画面在右侧，原PIC_MERGE_TEN)
    PIC_MERGE_ZOOM_M10_2_B8,            //十画面合成(两大八小，八个小画面在下面，原PIC_MERGE_TEN_2_8)
    PIC_MERGE_ZOOM_M10_2_T8,            //十画面合成(两大八小，八个小画面在上面)
    PIC_MERGE_ZOOM_M10_2_L8,            //十画面合成(两大八小，八个小画面在左侧)
    PIC_MERGE_ZOOM_M10_2_TB8,           //十画面合成(两个大画面居中，上下各四个小画面）
    PIC_MERGE_ZOOM_M10_1_9,             //十画面合成(一大九小，原PIC_MERGE_TEN_1_9)
    PIC_MERGE_ZOOM_M12_1_11 = 1200,     //十二画面合成(一大十一小，原PIC_MERGE_TWELVE)
    PIC_MERGE_ZOOM_M13_TL1_12 = 1300,   //十三画面合成(大画面在左上角，PIC_MERGE_THIRTEEN)
    PIC_MERGE_ZOOM_M13_TR1_12,          //十三画面合成(大画面在右上角)
    PIC_MERGE_ZOOM_M13_BL1_12,          //十三画面合成(大画面在左下角)
    PIC_MERGE_ZOOM_M13_BR1_12,          //十三画面合成(大画面在右下角)
    PIC_MERGE_ZOOM_M13_1_ROUND12,       //十三画面合成(大画面居中，小画面环绕四周)
    PIC_MERGE_ZOOM_M14_1_13 = 1400,     //十四画面合成(一大十三小，原PIC_MERGE_FOURTEEN)
    PIC_MERGE_ZOOM_M14_TL2_12,          //十四画面合成(两个大画面在左上角)
    PIC_MERGE_ZOOM_M15_T3_12 = 1500,    //十五画面合成(上三下十二)
    PIC_MERGE_ZOOM_M16 = 1600,          //十六画面合成(原PIC_MERGE_SIXTEEN)
    PIC_MERGE_ZOOM_M16_1_15,            //十六画面合成(一大十五小，原PIC_MERGE_SIXTEEN_1_15)
    PIC_MERGE_ZOOM_M20 = 2000,          //二十画面合成
    PIC_MERGE_ZOOM_M25 = 2500,          //二十五画面合成(原PIC_MERGE_TWENTYFIVE)
    PIC_MERGE_ZOOM_M36 = 3600,          //三十六画面合成(原PIC_MERGE_THIRTYSIX)
    PIC_MERGE_ZOOM_M49 = 4900,          //四十九画面合成(原PIC_MERGE_FORTYNINE)
    PIC_MERGE_ZOOM_M64 = 6400           //六十四画面合成(原PIC_MERGE_SIXTYFOUR)
} PicMergeZoomStyle;

//前景图像输入源枚举类型
typedef enum
{
    FG_PIC_INPUT = 1,                           //前景有图像输入
    NO_PIC_INPUT                                //前景无图像输入
} TFgInputSource;
typedef enum
{
    PIC_ZOOM_ONE = 1,       //全屏缩放
    PIC_ZOOM_TWO,           //保持比例拉伸并保持图像完整
    PIC_ZOOM_THREE,         //保持比例拉伸并充满目标区域
    PIC_ZOOM_FOUR,          //保持原有尺寸,大于合成尺寸的图像会居中并做裁边处理
    PIC_ZOOM_FIVE           //保持原有尺寸,大于合成尺寸的图像会按照保持比例拉伸并保持图像完整缩放
} TPicZoomStyle;

typedef enum
{
    MERGE_INBUF_ONE = 1,     //画面合成一个输入Buf
    MERGE_INBUF_MULTI        //画面合成输入多个Buf
} PicMergeInputFlag;
//某路图像是否勾画边框标记
typedef enum
{
    NO_DRAW_FOCUS,   //不勾画边框标记
    DRAW_FOCUS       //勾画边框标记
}TDrawFocusFlag;

//画面合成中前景图像是否画边框
#define PREPROCESS_DRAWSIDES   1

//画面合成中前景图像是否勾画选定边框
#define PREPROCESS_DRAWFOCUS   1

//画面合成中无图像处是否填充背景色
#define PREPROCESS_NO_PIC_FILL_BG  1

//画面合成中无前景图像输入
#define IMG_MERGE_NO_PIC_INPUT  1

//透明系数
#define FULL_TRANSPARENCY  0    //全透明
#define NOT_TRANAPARENCY   255  //不透明

//是否加台标的标志位
#define LOGO_ENABLE 0x01

//是否加横幅的标志位
#define BANNER_ENABLE 0x01

//是否滚动横幅的标志位
#define BANNER_LOGO_ENABLE 0x01

//是否运动侦测的标志位
#define MOTION_SURVEIL_ENABLE  0x01

//初始化时是否分配移动侦测内存标志位
#define MOTION_SURVEIL_GET_MEM 0x02

//解码器检测到数据丢失时，最大返回5个丢失的slice
#define MAX_DAMAGED_SLICE_NUM 5

//是否启用错误遮蔽的标志
typedef enum
{
    EC_OFF,             // 关闭错误遮蔽
    EC_ON               // 开启错误遮蔽
}TDECECFLAG;

// 编码器参数扩展
typedef enum
{
    ENC_EC = 101,        // 编码器的错误恢复策略
    ENC_CHECK_BS = 202,  // 编码器校验码流
    ENC_NUM_TYPE = 303,  // 设置编码器线程数扩展
    ENC_SET_SLICE = 404, // 设置编码器Slice长度扩展
    ENC_SLICE_ALIGN,     // 设置Slice四字节对齐
    ENC_VA_PARAM,        // VA编码参数扩展(仅用于Linux下VA H.264编码)
    ENC_USER_INFO        // 用户自定义信息扩展
}TEncParamReservedType;

// 编码器错误隐藏等级
typedef enum
{
    ENCECLEVELTYPE_OFF,     // 关闭错误隐藏策略
    ENCECLEVELTYPE_LEVEL1,  // 丢包率较低时使用的错误隐藏策略
    ENCECLEVELTYPE_LEVEL2   // 丢包率较高时使用的错误隐藏策略

}TEncECLevelType;

// 解码器错误隐藏等级
#define ISNEEDFREEZE(x) ((x) > 95)

// 编码输入参数扩展
typedef enum
{
    DEC_FEEDBACK = 201, // 解码器反馈
    ENC_VA_INPUT        // VA相关输入信息扩展(仅用于Linux下VA H.264编码)
}TEncInputParam;

typedef enum
{
    ENC_VA_OUTPUT  // VA相关输出信息扩展(仅用于Linux下VA H.264编码)
}TEncOutputParam;

// 解码器参数扩展
typedef enum
{
    DEC_EC = 100,               // 错误遮蔽标记
    DEC_IMAGE_FORMAT,           // 码流图像性质
    DEC_DEINTERLACE,            // 解码后是否做Deinterlace处理扩展标记
    DEC_SET_INPUT_FRAME_TYPE,   // 解码器设置输入图像帧场格式
    DEC_CHECK_BS,               // 解码器校验码流扩展
    DEC_MULTI_THREAD,           // 解码器多线程设置扩展
    DEC_GPU_PARAM,              // 硬件解码参数扩展(Win7)
    DEC_VA_PARAM                // VA硬件解码参数扩展(Linux)
}TDecParamReservedType;

// 解码输出参数扩展
typedef enum
{
    MULTI_DEC_OUT_PUT = 401,    //多路解码输出扩展
    DEC_EC_OUTPUT,              //错误遮蔽输出扩展
    DEC_CHECK_OUTPUT,           //解码码流校验输出信息扩展
    DEC_VA_OUTPUT               // VA硬件解码输出参数扩展(Linux)
}TDecOutReservedType;

typedef enum
{
    DEC_OUT_PRE_FRAME = 301  //解码输出前一帧标记
}TDecInputReservedType;

typedef enum
{
    POST_SPECIAL_RESIZER = 501  //后处理特殊缩放标记
}TPostParamReservedType;

typedef enum
{
    LOGO_SET_ENABLE = 601      //LOGO参数设置使能标记
}TLogoReservedType;

//解码后Deinterlace使能标志
typedef enum
{
    DEINTERLACE_ENABLE = 0x01, //开启deinterlace功能
    DEINTERLACE_DISABLE        //关闭Deinterlace功能
}TDeinterlaceFlag;

//是否第一次进行运动侦测
typedef enum
{
    FIRST_MOTIONSURVEIL,
    NON_FIRST_MOTIONSURVEIL
}MotionsurveilFirstPicFlag;

// deinterlace处理类型
typedef enum
{
    DEINTERLACE_MODE_1 = 0, //全局deinterlace处理
    DEINTERLACE_MODE_2,     //改进的半场deinterlace处理
    DEINTERLACE_MODE_3,     //使用场图低通滤波协助完成Deinterlace
    DEINTERLACE_MODE_4,     //采用时空域进行运动检测和方向滤波进行deinterlace
    DEINTERLACE_END         //不支持的deinteralce类型
}TDeinterlaceMode;

//解码后Deblock处理
typedef enum
{
    DEC_NOSET_DEBLOCK,                  //未设置解码后Deblock
    DEC_SET_DEBLOCK_AGREED_VIDEOUNIT,   //设置解码后Deblcok处理按照videounit默认方式
    DEC_SET_NODEBLOCK,                  //设置解码后不做Deblock处理
    DEC_SET_DEBLOCK,                    //设置解码后做Deblock处理
    DEC_SET_DEBLOCK_END
}TDecDeblockSetMode;

typedef enum
{
	QUERY_GPU_CODEC = 1            // 查询GPU解码是否支持
}TVideoQueryType;

#define H261BLANKFRAME  0x261BF  // 用以标识编码器输入结构体的扩展参数，表示设置261编码器编空帧

//错误码定义
#define ERR_START_VALUE (int)0x80000000         // 错误码开始值，最高位强制为1

//其中大模块定义（占用两个bit位：29、30位）为
#define ERR_VID_MOD (1 << 29)      //videounit模块
#define ERR_IMG_MOD (2 << 29)      //imageunit模块
#define ERR_OTHER_MOD (3 << 29)    //其他模块

//子模块定义（占用7个bit为，从22-28位）为
#define ERR_VID_SELF_MOD (1 << 22)       //videounit模块自身
#define ERR_VID_H261ENC_MOD (2 << 22)    //h261编码器子模块
#define ERR_VID_H261DEC_MOD (3 << 22)    //h261解码器子模块
#define ERR_VID_H263ENC_MOD (4 << 22)    //h263编码器子模块
#define ERR_VID_H263DEC_MOD (5 << 22)    //h263解码器子模块
#define ERR_VID_H264ENC_MOD (6 << 22)    //h264编码器子模块
#define ERR_VID_H264DEC_MOD (7 << 22)    //h264解码器子模块
#define ERR_VID_MPEG2ENC_MOD (8 << 22)   //mpeg2编码器子模块
#define ERR_VID_MPEG2DEC_MOD (9 << 22)   //mpeg2解码器子模块
#define ERR_VID_MPEG4ENC_MOD (10 << 22)  //mpeg4编码器子模块
#define ERR_VID_MPEG4DEC_MOD (11 << 22)  //mpeg4解码器子模块
#define ERR_VID_JPEGENC_MOD  (12 << 22)  //JPEG编码器子模块
#define ERR_VID_JPEGDEC_MOD  (13 << 22)  //JPEG解码器子模块
#define ERR_VID_PNGDEC_MOD  (14 << 22)  //PNG解码器子模块
#define ERR_VID_H264ENC_GPU_MOD  (15 << 22)  //h264硬件编码器子模块
#define ERR_VID_H264DEC_GPU_MOD  (16 << 22)  //h264硬件解码器子模块
#define ERR_VID_QUERY_MOD  (20 << 22)  //VideoQuery子模块

#define ERR_IMG_SELF_MOD (1 << 22)       //imageunit模块自身
#define ERR_IMG_MERGE_MOD (2 << 22)      //合成子模块
#define ERR_IMG_ZOOM_MOD (3 << 22)       //放缩子模块
#define ERR_IMG_COLORSPACE_MOD (4 << 22) //颜色空间转换子模块
#define ERR_IMG_LOGO_MOD (5 << 22)       //加图标、横幅子模块
#define ERR_IMG_FILTER_MOD (6 << 22)     //滤波子模块
#define ERR_IMG_MOTION_MOD (7 << 22)     //运动侦测子模块
#define ERR_IMG_MOSAIC_MOD (8 << 22)     //马赛克子模块
#define ERR_IMG_DEBLOCK_MOD (9 << 22)    //Deblock模块
#define ERR_IMG_PNCHANGE_MOD (10 << 22)	 //PN制转换模块
#define ERR_IMG_DERING_MOD (11 << 22)    //Dering模块
#define ERR_IMG_BMPTOYUV_ALPHA_MOD (12 << 22) //BMP转为YUV并生成Alpha通道模块
#define ERR_IMG_GENERATE_IMAGE_MOD (13 << 22) //生成图像模块
#define ERR_IMG_CROSS_FILTER_MOD (14 << 22)   //图像十字滤波模块
#define ERR_IMG_ADD_IMAGE_MOD (15 << 22)      //画中画子模块
#define ERR_IMG_ADAPT_16vs9_MOD (16 << 22)    //图像转换到16:9格式子模块
#define ERR_IMG_UYVYTO420_ANDTF_MOD (17 << 22)    //图像UYVY转为YUV420并进行时域滤波子模块
#define ERR_IMG_FILTEROSD_MOD (18 << 22)    //OSD菜单滤波子模块H
#define ERR_IMG_APT_ZOOM_MOD (19 << 22)     //根据上层设置参数缩放子模块
#define ERR_IMG_ZOOM_UYVY_MOD (20 << 22)     //UYVY的图像缩放子模块
#define ERR_IMG_MULTIDEC_MOD (21 << 22)     //多解码器子模块
#define ERR_IMG_ENHANCEIMAGE_MOD (22 << 22) //图像增强模块
#define ERR_IMG_MERGE_ZOOM_MOD (23 << 22)   //放缩并合成模块
#define ERR_IMG_MULTIDEC_NEW_MOD (24 << 22) //新版多路解码模块
#define ERR_IMG_TEMPORALFILTER_MOD (25 << 22)   //TemporalFilter模块
#define ERR_IMG_RESIZER_MOD (26 << 22)          //新版通用缩放模块错误码起始位置
#define ERR_IMG_SHARP_MOD (27 << 22)            //图像锐化模块错误码起始位置
#define ERR_IMG_GAUSS_FILTER_MOD (28 << 22)     //高斯滤波模块错误码起始位置
#define ERR_IMG_DEINTERLACE_MOD (29 << 22)      //Deinterlace模块错误码起始位置
#define ERR_IMG_MULTIENC_MOD (30 << 22)         //多路编码模块错误码起始位置
#define ERR_IMG_RGB_ZOOM_MOD (31 << 22)         //RGB565缩放模块错误码起始位置
#define ERR_IMG_RENOVATION_MOD (32 << 22)       //RENOVATION模块错误码起始位置
#define ERR_IMG_MOTION_DETECT_GM8180_MOD (33 << 22) //GM8180运动侦测模块错误码起始位置
#define ERR_IMG_DUALENC_MOD (34 << 22)         //多路编码模块错误码起始位置
#define ERR_IMG_RGBTOYUV_MOD (35 << 22)         //RGB565缩放模块错误码起始位置
#define ERR_IMG_MOTION_DETECT_DM365_MOD (36 << 22) //DM365运动侦测模块错误码起始位置
#define ERR_IMG_HQ3DIIRFILTER_MOD (37 << 22)   //TemporalFilter模块
#define ERR_IMG_DECIIRFILTER_MOD (38 << 22)    //解码图像后处理IIR滤波模块
#define ERR_IMG_ROTATE_MOD (39<<22)            //图像旋转模块
#define ERR_IMG_HAZEREMOVE_MOD (40<<22) //图像去雾模块错误码开始
//子模块定义（占用7个bit为，从22-28位）为
#define ERR_VID_API_MOD (1 << 22)              //视频API接口模块错误码起始
//API子模块定义
#define ERR_VID_API_ENC_MOD (1 << 20)          //视频API编码接口模块错误码起始
#define ERR_VID_API_DEC_MOD (2 << 20)          //视频API解码接口模块错误码起始
#define ERR_VID_API_IMG_MOD (3 << 20)          //视频API图像处理接口模块错误码起始

#define ERR_OTHER_START_VALUE (ERR_START_VALUE | ERR_OTHER_MOD)

enum OTHER_MODULE_ERROR_CODE
{
	ERR_OTHER_ERRORCODE_UNFOUND = ERR_OTHER_START_VALUE + 1,
	ERR_OTHER_ERRORCODE_LENGTH_EXCESS_BUFLEN
};

#define CLIP_POSITIVE(x) ((x) >= 0 ? (x) : 0)
#define CLIP_NEGATIVE(x) ((x) >= 0 ? 0 : (-(x)))
#define ABS(x) ((x) >= 0 ? (x) : (-(x)))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

#define RGB2Y(u8R, u8G, u8B)  	 (((66 * u8R + 129 * u8G + 25 * u8B + 128) >> 8) + 16)
#define RGB2U(u8R, u8G, u8B)     (((-38 * u8R - 74 * u8G + 112 * u8B + 128) >> 8) + 128)
#define RGB2V(u8R, u8G, u8B)	 (((112 * u8R - 94 * u8G - 18 * u8B + 128) >> 8) + 128)

#ifdef __cplusplus
}
#endif

#endif //VIDEO_DEFINES

