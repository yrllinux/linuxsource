/******************************************************************************
模块名	    ： VIDEOUNIT                
文件名	    ： videounit.h
相关文件	： videounit.c等
文件实现功能： 函数参数声明等
---------------------------------------------------------------------------------------
修改记录:
日  期		版本		修改人		走读人    修改内容
12/27/2006  1.0          赵波                  创建
**************************************************************************************/
#ifndef _VIDEOUNIT_H_
#define _VIDEOUNIT_H_

#include "videodefines.h"
#include "img_imagelib.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define VideoCodecHandle  void*

//VideoUnitEnc扩展类型(sunxx,20081212)
typedef enum
{
    ENC_TEST_TYPE = 1,
    ENC_FILTER_TYPE,            //时域滤波扩展类型
    ENC_DEINTERLACE_TYPE        //Deinterlace扩展类型
}TExtendEncType;

//ENC_FILTER_TYPE类型的扩展结构体
typedef struct
{
    TExtendEncType tExtendType; //结构扩展类型
    l32 l32IsDeinterlace;       //是否Deinterlace
    l32 l32IsTemporalFilter;    //是否时域滤波
}TVideoEncFilterParam;

//ENC_DEINTERLACE_TYPE类型的扩展结构体
typedef struct
{
    u32 u32ReservedType;            // 编码参数扩展类型(ENC_DEINTERLACE_TYPE)
    l32 l32EncDeinterlaceEnable;    // 开启Deinterlace功能
    l32 l32DeinterlaceMode;         // DeInterlace处理模式
    l32 l32Interlace;               // Deinterlace处理的帧场格式
    u32 u32Reserved;                // 保留参数
}TVideoEncDeinterlaceParam;

//VideoUnit编码器测试扩展结构体(sunxx,20081212)
typedef struct
{
    TExtendEncType tExtendType; //结构扩展类型
    l32 l32IsDeinterlace;       //是否Deinterlace
    l32 l32IsTemporalFilter;    //是否时域滤波
    u32 u32Reserved;
}TVideoEncTestParam;

//VideoUnit 扩展类型 add by wgz in 08-12-12
typedef enum
{
    DEC_TEST_TYPE = 1,          //videounit测试类型
    DEC_NonH264_DEBLOCK_TYPE    //是否对非264类型启用Deblock处理
}TExtendDecType;

//VideoUnit 解码器测试扩展结构体 add by wgz in 08-12-12
typedef struct
{
    TExtendDecType tExtendType; //解码器测试扩展结构体的类型
    u32 u32Reserved;            //保留参数
}TVideoDecTestParam;

//VideoUnitDec 设置解码后调用Deblock处理 add by zhaobo 2010-8-26
typedef struct
{
    u32 u32ReservedType;    //解码参数扩展类型(DEC_DEBLOCK_TYPE)
    l32 l32SetDeblock;      //设置非264格式码流是否需要进行Deblock处理
    l32 l32DeblockMode;     //暂时未开放。该字段须填为0
    u32 u32Reserved;        //保留参数
}TVideoDecNonH264DeblockParam;

typedef struct
{
    l32 l32StartMB;                     //区域丢失的起始宏块
    l32 l32EndMB;                       //区域丢失的结束宏块
}TSliceRegion;

typedef struct  
{
    l32 l32DropSliceNum;                              //记录丢slice的数目
    TSliceRegion tSliceRegion[MAX_DAMAGED_SLICE_NUM]; //记录区域信息的结构体数组
}TDropSliceInfo;

// 编码器处理解码器返回slice区域结构体
typedef struct
{
    u32 u32EncReservedType; // 编码参数扩展类型(ENC_EC)
    TDropSliceInfo tDSInfo; // 解码器返回的丢失的slice区域
    u32 u32Reserved;
}TDecFeedBack;

// 错误恢复参数结构体
typedef struct
{
    u32 u32EncReservedType; // 编码参数扩展类型(ENC_EC)
    l32 l32ECLevel;         // 错误恢复处理的级别(需要定义宏)
    u32 u32Reserved;        // 保留参数
}TECParam;

// 码流校验参数结构体
typedef struct
{
    u32 u32EncReservedType; // 编码参数扩展类型(ENC_CHECK_BS)
    l32 l32EnableCheckBs;   // 是否开启码流校验(1:打开; 0:关闭)
    u32 u32Reserved;        // 保留参数
}TEncCheckBsParam;

// Slice设置参数结构体
typedef struct
{
    u32 u32EncReservedType; // 编码参数扩展类型(ENC_SET_SLICE)
    l32 l32EncSliceLen;     // 编码Slice长度
    u32 u32Reserved;        // 保留参数
}TEncSetSlice;

// 设置Slice对齐参数结构体
typedef struct
{
    u32 u32EncReservedType;   // 编码参数扩展类型(ENC_SET_SLICE)
    l32 l32EnableSliceAlign;  // 使能slice四字节对齐
    u32 u32Reserved;          // 保留参数
}TEncSliceAlign;

//H.264编码器VA参数扩展(用于Linux平台VA硬件编码器)
typedef struct
{
    u32 u32ReservedType;        // 编码参数扩展类型(ENC_VA_PARAM)
    void *pvDisplay;            // Display句柄(XOpenDisplay()得到)
    void *pvVADisplay;          // VA Display句柄(vaGetDisplay()得到)
    l32 l32EnableInputSurfaceID;// 是否外部直接传入编码图像surfaceid
    u32 u32Reserved;            // 保留参数
}TVAEncParam;

// 设置用户自定义信息参数结构体
typedef struct
{
    u32 u32EncReservedType;   // 编码参数扩展类型(ENC_USER_INFO)
    l32 l32DisableUserInfo;   // 1:关闭用户信息；0：不关闭用户信息
    u32 u32Reserved;          // 保留参数
}TEncSetUserInfo;

// TReservedStructContainer 结构体类型
typedef enum 
{
    ReservedType_StructContainer = 0xfedcba98  // 全局唯一的类型标识符
} TReservedTypeStructContainer;

// 可以容纳多个扩展类型的结构体
typedef struct  
{
    l32 l32ReservedType;        // 扩展类型：ReservedType_StructContainer
    l32 l32ReservedStructNum;   // 扩展结构体的数目
    void *pvReservedStruct;     // 指向扩展结构体的数组
} TReservedStructContainer;

#define MIN_ENC_NUM 1
#define MAX_ENC_NUM 4
//编码器扩展参数
typedef struct
{
    u32  u32EncExtendType;      //扩展类型(ENC_NUM_TYPE)
    u32  u32EncNum;             //开启几个线程编码(支持MIN_ENC_NUM-MAX_ENC_NUM)
    l32 l32EnableBindCPU;       // 使能CPU绑定功能
    l32 al32CPUID[MAX_ENC_NUM]; // 每个线程待绑定的cpu id（cpu id从0开始），在l32EnableBindCPU非0时此字段可以使用
    u32  u32Reserved;           //保留参数
}TEncNumParam;

//编码器初始化结构体
typedef struct
{
    u32	u32EncType;			    //编码器类型
    l32	l32EncPacketMode;		//打包模式
    l32	l32EncWidth;		    //编码宽度
    l32	l32EncHeight;			//编码高度
    l32 l32EncMaxFrameRate;     //最大帧率
    l32 l32EncMinFrameRate;     //最小帧率
    l32 l32EncMaxBitRate;       //最大码率
    l32 l32EncMinBitRate;       //最小码率
    l32	l32EncMaxQP;		    //最大量化参数
    l32	l32EncMinQP;		    //最小量化参数
    l32	l32EncKeyFrameInterval;	//I帧间隔（两个I帧间P、B帧数）
    l32	l32EncQuality;	        //编码图像质量(auto, 1, 2)
    l32 l32EncVideoMode;        //编码图像制式(制式定义:videodefines.h的VIDEOMODETYPE)
    l32 l32EncInterlace;        //编码图像帧场类型
    l32 l32NeedUserInfo;        //是否写入用户信息
    u32	u32Reserved;			//保留参数
}TEncParam;

//编码器统计信息结构体
typedef struct
{
	l32 l32EncFrameRate;      //当前编码帧率
	l32 l32EncSkipFrameRate;  //Skip帧帧率
	l32 l32EncDropFrameRate;  //丢帧帧率
	u32 u32EncTotalFrame;     //总的编码帧数
	u32 u32EncTotalSkipFrame; //总的Skip帧帧数
	u32 u32EncTotalDropFrame; //总的丢帧帧数
	l32 l32EncBitRate;        //当前编码码率
	l32	l32EncQuality;	      //当前编码图像质量(具体等级)
	u32	u32Reserved;		  //保留参数
}TEncStatisParam;

//图像前处理状态结构体
typedef struct  
{
	l32 l32IsLogoBanner;    //是否添加图标
	l32 l32IsMotionSurveil; //是否进行运动侦测
	u32 u32Reserved;        //保留参数
}TPreProcessStatusParam;

//图像前处理结构体
typedef struct 
{
	l32 l32SrcYUVType;     //输入图像YUV类型
	l32 l32SrcIsInterlace; //输入图像的帧场格式
    l32 l32SrcWidth;       //输入图像的宽度
    l32 l32SrcHeight;      //输入图像的高度
    l32 l32SrcYStride;     //输入图像Y分量的步长
	l32 l32SrcUVStride;    //输入图像UV分量的步长

	l32 l32DstYUVType;     //输出图像YUV类型
	l32 l32DstIsInterlace; //输出图像的帧场格式
    l32 l32DstWidth;       //输出图像的宽度
    l32 l32DstHeight;      //输出图像的高度
    l32 l32DstYStride;     //输出图像Y分量的步长
	l32 l32DstUVStride;    //输出图像UV分量的步长
	l32 l32ImageQuality;   //图像处理质量等级()
	u32 u32Reserved;       //保留参数
}TPreProcessParam;

//运动信息结构体
typedef struct
{
	u8 *pu8MotionInfo;	     //指向保存运动信息的指针
	l32	l32MotionInfoLen;    //保存运动信息缓冲的长度(图像的宽度乘于图像的高度除于8)
	u8 u8Threshold;          //运动侦测灵敏度阈值
    u32 u32Reserved;         //保留参数
}TMotionSurveil;

//编码,图像前处理,运动侦测结构体
typedef struct
{
	TEncParam tEncParam;                    //编码器参数结构体
	TPreProcessParam tPreProcessParam;      //前处理参数结构体
    l32 l32IsLogo;                          //是否加台标标志位(LOGO_ENABLE: 加图标,其他:不加图标)
    TLogoInfo tLogoInfo;                    //加图标参数结构体
    l32 l32IsBanner;                        //是否加横幅标志位(BANNER_ENABLE: 画横幅,其他:不画横幅)
    TBanner tBanner;                        //加横幅参数结构体
    l32 l32IsBannerLogo;                    //是否滚动横幅标志位(BANNER_LOGO_ENABLE: 滚动横幅,其他:不滚动横幅)
    TImageBannerLogoParam tBannerLogoParam; //滚动横幅参数设置结构体
    l32 l32IsMotionSurveil;                 //是否进行运动侦测的标志位(MOTION_SURVEIL_ENABLE: 运动侦测,其他:不运动侦测)
    TMotionSurveil tEncMotionSurveil;       //运动信息参数结构体
	u32 u32Reserved;                        //保留参数
}TVideoUnitEncParam;

//编码器状态结构体
#define TEncStatusParam TEncParam
#define TVideoUnitEncStatusParam TVideoUnitEncParam

typedef struct
{
    u32 u32ReserverdType;   // 编码器输入扩展类型(H261BLANKFRAME)
    l32 l32H261BlankFrame;  // H.261是否要变空帧(1:编空帧,其他:正常编码)
    u32 u32Reserved;        // 保留参数
}TH261EncBlankFrame;

//编码输入结构体
typedef struct
{
    u8 *pu8YUV;             //指向编码器输入图像指针
    u32	u32EncSetKeyFrame;	//设置编码关键帧,1: 当前帧编码I帧,其他: 编码器自定
    u32	u32Reserved;	    //保留参数
}TEncInput;

//H.264编码器输入VA信息扩展(用于Linux平台VA硬件编码器输入surface id)
typedef struct
{
    u32 u32ReservedType;    // 编码器输入扩展类型(ENC_VA_INPUT)
    u32 u32SurfaceID;       // 编码输入surface id
    u32 u32EncBsBufSize;    //外部传入的编码码流Buffer大小
    u32 u32Reserved;        // 保留参数
}TVAEncInput;

//编码输入结构扩展结构体(用于解码器反馈)
typedef struct
{
    u32 u32ReservedType;        // 编码器输入扩展类型(DEC_FEEDBACK)
    TDropSliceInfo tDSInfo;     // 解码器反馈信息
    u32 u32Reserved;            // 保留参数
}TDecFeedBackParam;

//编码输出结构体
typedef struct
{
    u8 *pu8EncBitstream;	//指向输出码流指针
    l32	l32EncBitstreamLen;	//输出码流长度
    l32	l32EncFrameType;	//编码帧类型,I帧,P帧,SKIP帧,DROP帧
    u32	u32Reserved;		//保留参数
}TEncOutput;

typedef struct
{
    u32 u32ReservedType;    // 编码器输出扩展类型(ENC_VA_OUTPUT)
    u32 u32ExtBSBufSize;    // 外部码流buffer的大小，单位为字节
    u32 u32Reserved;        // 保留参数
}TVAEncOutput;

//解码后图像处理结构体
typedef struct
{
	l32 l32OutYUVType;     //输出图像的YUV类型
	l32 l32OutIsInterlace; //输出图像的帧场格式
    l32 l32OutWidth;       //输出图像的宽度
    l32 l32OutHeight;      //输出图像的高度
    l32 l32OutYStride;     //输出图像Y分量的步长
	l32 l32OutUVStride;    //输出图像UV分量的步长
	l32 l32ImageQuality;   //输出图像的质量等级
	u32	u32Reserved;	   //保留参数
}TPostProcessParam;

//图像后处理参数扩展结构体
typedef struct  
{
    //源裁剪相关参数
    u32 u32PostReservedType; //后处理扩展类型(POST_SPECIAL_RESIZER)
    l32 l32SrcTopCutLine;     //源图上面的裁剪的行数，应为2的倍数
	l32 l32SrcBottomCutLine;  //源图下面的裁剪的行数，应为2的倍数
	l32 l32SrcLeftCutPoint;   //源图左边的裁剪的点数，应为2的倍数
	l32 l32SrcRightCutPoint;  //源图右边的裁剪的点数，应为2的倍数
	//目的填充相关参数
	l32 l32DstTopFillLine;    //目的图上面的填充的颜色行数，应为2的倍数
	l32 l32DstBottomFillLine; //目的图下面的填充的颜色行数，应为2的倍数
	l32 l32DstLeftFillPoint;  //目的图左边的填充的颜色点数，应为2的倍数
	l32 l32DstRightFillPoint; //目的图右边的填充的颜色点数，应为2的倍数
	l32 l32DstFillYColor;     //目的图边缘填充颜色Y分量值
	l32 l32DstFillUColor;     //目的图边缘填充颜色U分量值
	l32 l32DstFillVColor;     //目的图边缘填充颜色V分量值，目前黑色为Y:0x01,UV:0x80,绿色是全零
	
    u32 u32Reserved;          //保留参数,请置零
}TPostSpecialResizerParam;

//图像后处理状态结构体
typedef struct  
{
	l32 l32IsLogoBanner;    //是否添加图标
	l32 l32IsMotionSurveil; //是否进行运动侦测
	u32 u32Reserved;        //保留参数
}TPostProcessStatusParam;

//解码器初始化结构体
typedef struct
{
	u32	u32DecType;		  	   //解码器类型
	l32	l32DecPacketMode;	   //码流打包模式
    l32 l32DecMaxWidth;        //解码图像的最大宽度
    l32 l32DecMaxHeight;       //解码图像的最大高度
	u32	u32Reserved;	       //保留参数
}TDecParam;

//解码器初始化扩展结构体(用于错误遮蔽)
typedef struct
{
    u32 u32ReservedType;        // 解码输出扩展类型(DEC_EC)
    l32 l32ErrorConcealFlag;    // 是否打开错误遮蔽开关(EC_ON:打开; EC_OFF:关闭)
    u32 u32Reserved;            // 保留参数
}TDecECParam;
//解码器参数扩展(用于码流校验)
typedef struct
{
    u32 u32ReservedType;        // 解码参数扩展类型(DEC_CHECK_BS)
    l32 l32EnableCheckBs;       // 是否打开解码端码流校验(1:打开; 0:关闭)
    u32 u32Reserved;            // 保留参数
}TDecCheckBsParam;

//解码器参数扩展(用于Win7系统硬件解码器)
typedef struct
{
    u32 u32ReservedType;        // 解码参数扩展类型(DEC_GPU_PARAM)
    l32 l32EnableD3dAlloc;      //是否开启使用D3d显存解码
    l32 l32EnableDecCopy;       //是否开启解码数据输出
    u32 u32Reserved;            // 保留参数
}TGPUDecParam;

//基于VA的GPU解码器参数扩展（用于Linux系统硬件解码器）
typedef struct
{
    u32 u32ReservedType;        // VA解码参数扩展类型(DEC_VA_PARAM)
    void *pvDisplay;            // Display句柄(XOpenDisplay()得到)
    void *pvVADisplay;          // VA Display句柄(vaGetDisplay()得到)
    l32 l32EnableOutSurfaceId;  // 是否输出解码surfaceid
    l32 l32EnableOutYUV;        // 是否输出解码YUV数据
    u32 u32Reserved;
}TVADecParam;

typedef struct
{
    u32 u32ReservedType;        // 解码参数扩展类型(DEC_MULTI_THREAD)
    l32 l32EnableMultiThread;   // 是否打开多线程解码(1:打开; 0:关闭)
    u32 u32Reserved;            // 保留参数
}TDecMultiThreadParam;


//解码器初始化扩展结构体(用于和公司默认的图像格式不同的场景)
typedef struct
{
    u32 u32ReservedType;        // 解码输出扩展类型(DEC_IMAGE_FORMAT)
    l32 l32SrcInterlace;        // 码流的帧场格式
    u32 u32Reserved;            // 保留参数
}TDecSrcImageParam;

//解码器运行状态结构体
typedef struct
{
	l32 l32DecFrameRate;       //解码帧率
	l32 l32DecBitRate;         //解码码率
	u32 u32DecTotalFrame;      //总的解码帧数
	u32 u32DecTotalErrorFrame; //总的解码失败帧数
    l32 l32DecWidth;           //解码图像的宽度
    l32 l32DecHeight;          //解码图像的高度
	u32	u32Reserved;	       //保留参数
}TDecStatisParam;

//TDecParam结构体中u32Reserved扩展为是否开启Deinterlace后处理。
typedef struct
{
    u32 u32ReservedType;          // 解码参数扩展类型(DEC_DEINTERLACE)
    l32 l32DecDeinterlaceEnable;  // 开启Deinterlace功能
    l32 l32DeinterlaceMode;       // DeInterlace处理模式
    u32 u32Reserved;              // 保留参数
}TDecDeinterlaceParam;

//TDecParam结构体中u32Reserved扩展为是否设置解码器输入图像帧场格式。
typedef struct
{
    u32 u32ReservedType;        // 解码参数扩展类型(DEC_SET_INPUT_FRAME_TYPE)
    l32 l32Width;               // 图像宽度
    l32 l32Height;              // 图像高度
    l32 l32SrcInterlaceType;    // 解码器输入图像的帧场格式
    u32 u32Reserved;            // 保留参数
}TDecSrcFrameTypeParam;

//解码器,图像后处理,运动状态结构体
typedef struct
{
	TDecParam tDecParam;                      //解码器参数结构体
	TPostProcessParam tPostProcessParam;       //后处理参数结构体
    l32 l32DecIsLogo;                          //解码图像是否加图标
    TLogoInfo tDecLogoInfo;                    //加图标参数结构体
    l32 l32DecIsBanner;                        //解码图像是否画横幅
    TBanner tDecBanner;                        //加横幅参数结构体
    l32 l32DecIsBannerLogo;                    //解码图像是否滚动横幅
    TImageBannerLogoParam tDecBannerLogoParam; //滚动横幅参数设置结构体
    l32 l32DecIsMotionSurveil;                 //是否进行运动侦测
	TMotionSurveil tDecMotionSurveil;          //运动信息参数结构体
	u32 u32Reserved;                           //保留参数
}TVideoUnitDecParam;

//解码器解码状态结构体
#define TDecStatusParam TDecParam
#define TVideoUnitDecStatusParam TVideoUnitDecParam

//解码器输入结构体
typedef struct
{
	u8 *pu8DecBitstream;	//输入码流缓冲
	u32 u32DecBitstreamLen;	//一帧码流的有效长度
    l32 l32DecIsOutputYUV;  //解码图像是否输出[0表示输出，非零表示不输出]
	u32 u32Reserved;		//保留参数
}TDecInput;

//解码器输出结构体
typedef struct
{
    u8 *pu8YUV;		     //指向解码器输出图象缓冲的指针
    l32	l32DecWidth;     //解码图象的实际宽度
    l32	l32DecHeight;    //解码图象的实际高度
    l32	l32DecFrameType; //解码帧类型
    l32 l32DecInterlaceType; //解码帧场类型
    u32	u32Reserved;	 //保留参数
}TDecOutput;

//多路解码器输出结构体
typedef struct
{
    u32 u32ReservedType;//解码输出扩展类型(MULTI_DEC_OUT_PUT)
    u8 *pu8OutY;	    //指向解码器输出图象缓冲Y分量的指针
    u8 *pu8OutU;        //指向解码器输出图象缓冲UV分量的指针
    u8 *pu8OutV;        //指向解码器输出图象缓冲UV分量的指针
    l32 l32OutYStride;  //指向解码器输出图象缓冲Y分量Stride 
    l32 l32OutUVStride; //指向解码器输出图象缓冲UV分量Stride
    l32 l32DecWidth;    //解码图象的实际宽度
    l32 l32DecHeight;   //解码图象的实际高度
    l32 l32DecFrameType;//解码帧类型
    u32 u32Reserved;    //保留参数
}TMultiDecOutput;

//错误恢复输出结构体
typedef struct
{
    u32 u32ReservedType;        //解码输出扩展类型(DEC_EC_OUTPUT)
    TDropSliceInfo tDSInfo;     //解码一帧后统计得到的slice丢失情况
    l32 l32ValidMBRatio;        //标识有效宏块的比例，其中-1表示该字段无效
    u32 u32Reserved;            //保留参数
}TECOutput;

//码流校验输出结构体
typedef struct
{
    u32 u32ReservedType;        //解码输出扩展类型(DEC_CHECK_OUTPUT)
    l32 l32CheckBsVal;          //校验结果(0:校验失败； 1：校验成功)
    u32 u32Reserved;            //保留参数
}TDecCheckOutput;

//VA解码输出结构体
typedef struct
{
    u32 u32ReservedType;//解码输出扩展类型(DEC_VA_OUTPUT)
    void *pvVADisplay;   //返回解码所使用的VADisplay句柄
    u32 u32SurfaceId;   //解码图像的surfaceid
    u32 u32Reserved;	//保留参数
}TVADecOutput;

//解码器是否输出前一帧图像标记
typedef struct
{
    u32 u32ReservedType;     //解码输出扩展类型(DEC_OUT_PRE_FRAME)
    l32 l32OutPreFrameFlag;  //输出前一帧图像标识(1:输出前一帧；0:不输出前一帧）
    u32 u32Reserved;         //保留参数
}TDecOutPreFlag;

typedef struct
{
    l32 l32GPUCodecSupport;       //是否支持H.264 GPU解码（1-支持；0-不支持）
    l32 l32GPUCodecMinorVersion;  //GPU Codec版本号(minor)
    l32 l32GPUCodecMajorVersion;  //GPU Codec版本号(major)
    u32 u32Reserved;
}TVidGPUCodecInfoOutput;

/*====================================================================
函数名		: VideoUnitEncoderGetVersion
功能		: 编码器接口版本信息
算法实现	: 无
引用全局变量: 无
参数        : pvVer: 输出缓存[out]
			  l32BufLen: 缓存长度[in]
			  pl32VerLen: 信息长度[in/out]
			  u32EncType: 编码器类型[in]
返回值说明  : 成功: VIDEO_SUCCESS, 失败: 返回错误码
====================================================================*/
l32 VideoUnitEncoderGetVersion(void *pvVer, l32 l32BufLen, l32 *pl32VerLen, u32 u32EncType);

/*====================================================================
函数名		: VideoUnitEncoderOpen
功能		: 初始化编码器和前处理
算法实现	: 无
引用全局变量: 无
参数        : ptHandle: 指向编码器句柄指针[in/out]
              ptVideoUnitEncParam: 指向编码初始化参数结构体指针[in]
			  pvEncMemParam: 指向内存参数结构体指针[in]
返回值说明  : 成功: VIDEO_SUCCESS, 失败: 返回错误码
====================================================================*/
l32 VideoUnitEncoderOpen(VideoCodecHandle *ptHandle, TVideoUnitEncParam *ptVideoUnitEncParam, void *pvEncMemParam);

/*====================================================================
函数名		: VideoUnitEncoderProcess
功能		: 编码一帧图象
算法实现	: 无
引用全局变量: 无
参数        : ptHandle: 指向编码器句柄指针[in]
              ptEncInput: 指向编码输入结构体指针[in]
              ptEncOutput: 指向编码输出结构体指针[in/out]
返回值说明  : 成功: VIDEO_SUCCESS, 失败: 返回错误码
====================================================================*/
l32 VideoUnitEncoderProcess(VideoCodecHandle ptHandle, TEncInput *ptEncInput, TEncOutput *ptEncOutput);

/*====================================================================
函数名		: VideoUnitEncoderClose
功能		: 关闭编码器
算法实现	: 无
引用全局变量: 无
输入参数说明: ptHandle: 指向编码器句柄指针[in]
输出参数说明: 无
返回值说明  : 成功: VIDEO_SUCCESS, 失败: 返回错误码
====================================================================*/
l32 VideoUnitEncoderClose(VideoCodecHandle ptHandle);

/*====================================================================
函数名		: VideoUnitEncoderSetParam
功能		: 设置编码器参数
算法实现	: 无
引用全局变量: 无
参数        : ptHandle:	编码器句柄[in/out]
			  ptVideoUnitParam: 编码设置参数[in]
返回值说明  : 成功: VIDEO_SUCCESS, 失败: 返回错误码
举例或特别说明: 目前调用该函数可改变的参数不包括编码宽度和高度，如需改变
                编码图像的宽高，须先关闭原编码器，再按新的宽高初始化新的编码器
====================================================================*/
l32 VideoUnitEncoderSetParam(VideoCodecHandle ptHandle, TVideoUnitEncParam *ptVideoUnitParam);

/*====================================================================
函数名		: VideoUnitEncoderGetStatus
功能		: 获取编码器运行状态
算法实现	: 无
引用全局变量: 无
参数        : ptHandle: 编码器句柄[in]
			  ptVidUnitEncStatusParam: 编码状态参数[in/out]
返回值说明  : 成功: VIDEO_SUCCESS, 失败: 返回错误码
====================================================================*/
l32 VideoUnitEncoderGetStatus(VideoCodecHandle ptHandle, TVideoUnitEncStatusParam *ptVidUnitEncStatusParam);

/*====================================================================
函数名		: VideoUnitEncoderGetStatis
功能		: 获取编码器编码统计信息
算法实现	: 无
引用全局变量: 无
参数        : ptHandle: 指向编码器句柄指针[in]
			  ptEncStatisParam: 指向编码器统计信息结构体指针[in/out]
返回值说明  : 成功: VIDEO_SUCCESS, 失败: 返回错误码
====================================================================*/
l32 VideoUnitEncoderGetStatis(VideoCodecHandle ptHandle, TEncStatisParam *ptEncStatisParam);

/*====================================================================
函数名		: VideoUnitEncoderDebug
功能		: 编码器调试信息
算法实现	: 无
引用全局变量: 无
参数        : ptHandle:	指向编码器句柄指针[in]
              pvDebugInfo : 指向调试信息指针[in/out]
返回值说明  : 成功: VIDEO_SUCCESS, 失败: 返回错误码
====================================================================*/
l32 VideoUnitEncoderDebug(VideoCodecHandle ptHandle, void *pvDebugInfo);

/*====================================================================
函数名	    : VideoUnitEncoderAnalyzeErrorCode
功能	    : 编码器错误码分析
算法实现	: 无
引用全局变量: 无
参数        :  ptHandle: 指向编码器句柄指针[in/out]
              l32ErrorCode: 错误码[in]
              l32BufLen: 缓存长度[in]
              pvErrorCodeInfo: 缓存buffer[in/out]
返回值说明  : 成功: VIDEO_SUCCESS, 失败: 返回错误码
====================================================================*/
l32 VideoUnitEncoderAnalyzeErrorCode(VideoCodecHandle ptHandle, l32 l32ErrorCode, l32 l32BufLen, void *pvErrorCodeInfo);

/*====================================================================
函数名	    : VideoUnitEncoderGetMemSize
功能	    : 获取编码需要内存大小
算法实现	: 无
引用全局变量: 无
参数        : ptVideoUnitDecParam: 指向编码初始化参数结构体指针[in]
              pl32EncMemSize: 需要内存大小[in/out]
返回值说明  : 成功: VIDEO_SUCCESS, 失败: 返回错误码
====================================================================*/
l32 VideoUnitEncoderGetMemSize(TVideoUnitEncParam *ptVideoUnitEncParam, l32 *pl32EncMemSize);

/*====================================================================
函数名		: VideoUnitDecoderGetVersion
功能		: 解码器接口版本信息
算法实现	: 无
引用全局变量: 无
参数        : pvVer: 输出缓存[in/out]
			  l32BufLen: 缓存长度[in]
			  pl32VerLen: 信息长度[out]
			  u32DecType: 解码器类型[in]
返回值说明  : 成功: VIDEO_SUCCESS, 失败: 返回错误码
====================================================================*/
l32 VideoUnitDecoderGetVersion(void *pvVer, l32 l32BufLen, l32 *pl32VerLen, u32 u32DecType);

/*====================================================================
函数名		: VideoUnitDecoderOpen
功能		: 初始化解码器
算法实现	: 无
引用全局变量: 无
参数        : ptHandle: 指向解码器句柄指针[in/out]
              ptVideoUnitDecParam: 指向解码初始化参数和解码输出图像结构体指针[in]
			  pvDecMemParam: 指向内存空间参数结构体指针[in]
返回值说明  : 成功: VIDEO_SUCCESS, 失败: 返回错误码
====================================================================*/
l32 VideoUnitDecoderOpen(VideoCodecHandle *ptHandle, TVideoUnitDecParam *ptVideoUnitDecParam, void *pvDecMemParam);

/*====================================================================
函数名		: VideoUnitDecoderProcess
功能		: 解码一帧图象
算法实现	: 无
引用全局变量: 无
参数        : ptHandle: 解码器句柄[in]
              ptDecInput: 指向解码器输入参数结构体指针[in]
              ptDecOutput: 指向解码器输出参数结构体指针[out]
返回值说明  : 成功: VIDEO_SUCCESS, 失败: 返回错误码
====================================================================*/
l32 VideoUnitDecoderProcess(VideoCodecHandle ptHandle, TDecInput *ptDecInput, TDecOutput *ptDecOutput);

/*====================================================================
函数名		: VideoUnitDecoderClose
功能		: 关闭解码器
算法实现	: 无
引用全局变量: 无
参数        : ptHandle:	解码器句柄[in]
返回值说明  : 成功: VIDEO_SUCCESS, 失败: 返回错误码
====================================================================*/
l32 VideoUnitDecoderClose(VideoCodecHandle ptHandle);

/*====================================================================
函数名		: VideoUnitDecoderSetParam
功能		: 设置解码器参数
算法实现	: 无
引用全局变量: 无
参数        : ptHandle: 指向解码器句柄指针[in/out]
			  ptVideoUnitDecParam: 指向解码器初始化参数结构体指针[in]
返回值说明  : 成功: VIDEO_SUCCESS, 失败: 返回错误码
举例或特别说明: 目前调用该函数可改变的参数不包括解码宽度和高度, 如需改变
                解码图像的宽高, 须先关闭原解码器, 再按新的宽高初始化新的
====================================================================*/
l32 VideoUnitDecoderSetParam(VideoCodecHandle ptHandle, TVideoUnitDecParam *ptVideoUnitDecParam);

/*====================================================================
函数名		: VideoUnitDecoderGetStatus
功能		: 获取解码器运行状态
算法实现	: 无
引用全局变量: 无
参数        : ptHandle:	指向解码器句柄的结构指针[in]
			  ptVideoUnitDecStatusParam: 指向解码状态参数结构体指针[out]
返回值说明  : 成功: VIDEO_SUCCESS, 失败: 返回错误码
====================================================================*/
l32 VideoUnitDecoderGetStatus(VideoCodecHandle ptHandle, TVideoUnitDecStatusParam *ptVideoUnitDecStatusParam);

/*====================================================================
函数名		: VideoUnitDecoderGetStatis
功能		: 获取解码器统计信息状态
算法实现	: 无
引用全局变量: 无
参数        : ptHandle:	指向解码器句柄的结构指针[in]
			  ptDecStatisParam: 指向解码器统计信息结构体指针[out]
返回值说明  : 成功: VIDEO_SUCCESS, 失败: 返回错误码
====================================================================*/
l32 VideoUnitDecoderGetStatis(VideoCodecHandle ptHandle, TDecStatisParam *ptDecStatisParam);

/*====================================================================
函数名		: VideoUnitDecoderDumpStatus
功能		: 解码器调试信息
算法实现	: 无
引用全局变量: 无
参数        : ptHandle:	指向解码器句柄指针[in]
              pvDebugInfo: 指向调试信息指针[in/out]
返回值说明  : 成功: VIDEO_SUCCESS, 失败: 返回错误码
====================================================================*/
l32 VideoUnitDecoderDebug(VideoCodecHandle ptHandle, void *pvDebugInfo);

/*====================================================================
函数名	    : VideoUnitDecoderAnalyzeErrorCode
功能	    : 解码器错误码分析
算法实现	: 无
引用全局变量: 无
参数        : l32ErrorCode:  错误码[in]
              l32BufLen: 缓存长度[in]
              pvErrorCodeInfo: 缓存buffer[in]
返回值说明  : 成功: VIDEO_SUCCESS, 失败: 返回错误码
====================================================================*/
l32 VideoUnitDecoderAnalyzeErrorCode(VideoCodecHandle ptHandle, l32 l32ErrorCode, l32 l32BufLen, void *pvErrorCodeInfo);

/*====================================================================
函数名	    : VideoUnitDecoderGetMemSize
功能	    : 获取解码需要内存大小
算法实现	: 无
引用全局变量: 无
参数        : ptVideoUnitDecParam: 指向解码初始化参数和解码输出图像结构体指针[in]
              pl32DecMemSize: 需要内存大小[in/out]
返回值说明  : 成功: VIDEO_SUCCESS, 失败: 返回错误码
====================================================================*/
l32 VideoUnitDecoderGetMemSize(TVideoUnitDecParam *ptVideoUnitDecParam, l32 *pl32DecMemSize);

/*====================================================================
函数名	    : VideoCodecQuery
功能	    : 获取编解码能力信息
参数        : u32QueryType:         查询类型(QUERY_GPU_CODEC）
              pvVidCodecInfoInput:  获取编解码能力信息输入结构指针[in] 
              pvVidCodecInfoOutput:	获取编解码能力信息输出结构指针[out]
返回值说明  : 成功: VIDEO_SUCCESS, 失败: 返回错误码
====================================================================*/
l32 VideoCodecQuery(u32 u32QueryType, void *pvVidCodecInfoInput, void* pvVidCodecInfoOutput);
#ifdef __cplusplus
}
#endif

#endif	//_VIDEOUNIT_H_
