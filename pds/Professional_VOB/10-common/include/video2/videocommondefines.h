
#ifndef VIDEO_COMMON_DEFINES

#define VIDEO_COMMON_DEFINES


//图像格式定义

//帧格式
#define FRAMEFORMAT   0x01

//场格式
#define FIELDFORMAT   0x02

//图像:720*576
#define IMAGE_SIZE_720576 				0

//图像:1/4 720*576(352*288)
#define IMAGE_SIZE_QUARTER_720576 		1

//图像:1/9 720*576(240*192)
#define IMAGE_SIZE_NINTH_720576 		2

//图像:1/16 720*576(176*144)
#define IMAGE_SIZE_SIXTEENTH_720576 	3

//图像的编解码格式(I帧或者P帧)
//I帧
#define IPICTURE  0x01

//P帧
#define PPICTURE  0x02

//图像缩放质量定义
#define  QUALITYON     0
#define  QUALITYOFF    1

//返回成功或者失败的定义
#define  VIDEOSUCCESS   0
#define  VIDEOFAILURE     -1

//图像类型定义
#define  YUV422     422
#define  YUV420     420

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

#endif //VIDEO_COMMON_DEFINES

