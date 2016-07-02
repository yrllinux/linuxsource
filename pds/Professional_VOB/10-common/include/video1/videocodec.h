#ifndef VIDEO_CODEC_INTERFACE_VER1
#define VIDEO_CODEC_INTERFACE_VER1

#include "algorithmtype.h"
#include "videocommondefines.h"
#include "imageprocess.h"

//some limitations
#define MAX_ENCODER_NUM         2
#define MAX_DECODER_NUM         16
#define MAX_IMAGES_IN_ONE		16
#define MAX_LOGOS_IN_IMAGE		32
#define MAX_BANNERS_IN_IMAGE	32
#define LOGO_STATUS_LEN			20

//打包方式
//h261:no rtp packet info
#define H261_PACKET_NO_RTPINFO		0

//h261:has rtp packet info
#define H261_PACKET_RTPINFO		    1

//263:263
#define H263_PACKET_BY_263			0

//263:263+
#define H263_PACKET_BY_263PLUS		1

//264:rtp
#define H264_PACKET_BY_RTP			0

//264:annexb
#define H264_PACKET_BY_ANNEXB		1


//sq 050810 add 画面合成类型定义
typedef enum {
  PIC_NO_MERGE           ,
  PIC_MERGE_ONE		     ,
  PIC_MERGE_VTWO		 ,
  PIC_MERGE_ITWO		 ,
  PIC_MERGE_THREE		 ,
  PIC_MERGE_FOUR		 ,
  PIC_MERGE_SFOUR		 ,
  PIC_MERGE_SIX		     ,
  PIC_MERGE_SEVEN		 ,
  PIC_MERGE_EIGHT		 ,
  PIC_MERGE_NINE		 ,
  PIC_MERGE_TEN	         ,
  PIC_MERGE_THIRTEEN     ,
  PIC_MERGE_SIXTEEN 
} PicMergeStyle;


//编解码器类型
#define VCODEC_H261					0x0001
#define VCODEC_H263					0x0002
#define VCODEC_MPEG2				0x0003
#define VCODEC_MPEG4				0x0004
#define VCODEC_MPEG4_V200			0x0005
#define VCODEC_H264_SIMPLE_1		0x0011
#define VCODEC_H264_SIMPLE_2		0x0012
#define VCODEC_H264_SIMPLE_3		0x0013
#define VCODEC_H264_ADVANCE			0x0021
#define VCODEC_H264_F264			0x0022


//多map编码操作map号
typedef enum {
	SINGLE_MAP_ENC_ONE    ,
	MULTI_MAP_ENC_ONE     ,
	MULTI_MAP_ENC_TWO
} EncMapID;

//编解码外围初始化标记
typedef enum {
    NO_ENC  ,
    ENC_INIT  
} EncInitFlag;

typedef enum {
    NO_DEC  ,
    DEC_INIT  
} DecInitFlag;

//某路图像是否勾画边框标记
typedef enum {
    NO_DRAW_FOCUS ,
    DRAW_FOCUS  
} DrawFocusFlag;

//typedef void * VideoCodecHandle
#define VideoCodecHandle  void*

typedef struct tImage
{
	u8		*pu8YUV;			//图象YUV指针
	l32		l32Width;        	//图象的宽度
	l32		l32Height;       	//图象的高度
	l32		l32ImageType;    	//图象的类型(帧格式或者场格式)
	l32		l32YUVType;      	//图标图象格式(YUV422或者YUV420)
	l32		l32YUVLen;			//YUV长度

    u8      u8DrawFocusFlag;     //画面是否勾画边框的标记
	u8		u8FocuslineR;		//画面边框色R
	u8		u8FocuslineG;		//画面边框色G
	u8		u8FocuslineB;		//画面边框色B
    u8      u8FocusWidth;        //边框统一宽度（暂不可设置）
	u8		u8Pos;				//画面位置编号
	
}TImage;		//图象

//图标信息的结构体
typedef struct tLogo
{
	u8		*pu8YUV;               	//前景图象数据指针地址(图象格式为YUV422时,分配的空间大小为l32Width*l32Height*2,图象格式为YUV420时,分配的空间大小为l32Width*l32Height*3/2)
	u8		*pu8Alpha;     			//前景图象的alpha通道指针(分配空间的大小和分配图象数据的大小一样)
	u8		*pu8Beta;				//对应背景图象的beta通道指针(分配空间的大小和分配图象数据的大小一样)
	l32		l32Width;              //图标图象的宽度
	l32		l32Height;             //图标图象的高度
	l32		l32YUVType;            //图标图象格式(YUV422或者YUV420)
	l32		l32ImageType;          //前景图象的类型(帧格式或者场格式)
	l32     l32LogoRefPicWidth;    //图标添加参考图像的宽度(用户填写，目前基本使用720)
	l32     l32LogoRefPicHeight;   //图标添加参考图像的高度(用户填写，目前基本使用576)
	l32		l32IsAdaptiveColor;    //是否根据底色改变图标的颜色,1表示需要改变图标的颜色,其他表示不需要改变图标的颜色
	u8	 	u8FgTransparency;       //前景图象透明系数
    //u8      u8BgTransparency;       //背景图象透明系数
	l32 	l32Top;				    //图标图象垂直位置偏移（相对于显示窗口）
	l32		l32Left;        	    //图标图象水平位置偏移（相对于显示窗口）
	s16		s16StepH;				//图标水平运动速度（每帧移动象素数，正向右负向左）
	s16		s16StepV;				//图标垂直运动速度（每帧移动象素数，正向上负向下）
	l32		l32ShowTimes;			//图标显示次数（负：一直显示；0：不显示；正：图标有速度时为循环显示次数，图标无速度时调用次数）
	l32		l32PauseIntervalH;		//图标水平运动暂停间隔（象素数）（正：有暂停； 非正：无暂停）
	l32		l32PauseIntervalV;		//图标垂直运动暂停间隔（象素数）（正：有暂停； 非正：无暂停）
	l32		l32PauseTime;			//图标暂停时间（调用次数）
	l32		l32WindowTop;			//显示窗口垂直位置
	l32		l32WindowLeft;			//显示窗口水平位置
	l32		l32WindowWidth;			//显示窗口宽度
	l32		l32WindowHeight;		//显示窗口高度

	TFontInfo tFontInfo;			//图标中字的结构体
	l32		al32LogoStatus[LOGO_STATUS_LEN];	//图标状态，须上层初始化时清零（内部使用）目前使用情况：l32[LOGO_H_INTER_MOVE_COUNT]:图标水平暂停间连续移动计数
	                                            //                                                      l32[LOGO_H_PAUSE_COUNT]:图标水平暂停计数
	                                            //                                                      l32[LOGO_V_INTER_MOVE_COUNT]:图标垂直暂停间连续移动计数
	                                            //                                                      l32[LOGO_V_PAUSE_COUNT]:图标垂直暂停计数
	                                            //                                                      l32[LOGO_CALL_TIME]:图标总共添加的次数
	                                            //                                                      l32[LOGO_FIRST_ADD_FLAG]:图标首次添加标记0:首次 1:非首次
                                                //                                                      l32[LOGO_CUR_BG_WIDTH]:图标添加基于的背景宽度
                                                //                                                      l32[LOGO_CUR_BG_HEIGHT]:图标添加基于的背景高度
	
}TLogo;			//图标

typedef struct tEncParam
{
	u32		u32EncoderType;				//编码器类型
	l32		l32FrameRate;				//帧率
	l32		l32BitRate;					//码率（bps）
	l32		l32MinQP;					//最小量化参数
	l32		l32MaxQP;					//最大量化参数
	l32		l32IFrameInterval;			//I帧间隔（两个I帧间P、B帧数）
	l32		l32PacketMode;				//打包模式
	l32		l32EncodeWidth;				//编码宽度
	l32		l32EncodeHeight;			//编码高度（宽度=0&高度=0表示自适应，目前仅适用于mpeg4）
	l32		l32FrameDropRate;			//允许连续丢帧率（0表示不丢帧，速度优先；非0表示有丢帧，质量优先，必须为10的倍数，以10%为单位）
	u32		u32EncodeCycles;			//编码一帧可用周期数，目前用于传输编码map的主频(以兆为单位)
	l32		l32EncodeYUVType;			//编码YUV类型（420、422、444），目前仅支持对420图像编码，此参数没有用到
    l32     l32CoreNum;                 //编码用到的内核数（1：单map编码，2：两map编码）
    l32		l32Limitation;				//编码器限制（）
    l32     l32SliceLen;
	
}TEncParam;	//编码初始化参数


typedef struct tEncoderInput
{
	TImage		*aptImage[MAX_IMAGES_IN_ONE+1];			//编码图象组（以NULL为结束标志）
	TLogo		*aptLogo[MAX_LOGOS_IN_IMAGE+1];			//图标（以NULL为结束标志）
	TBanner		*aptBanner[MAX_BANNERS_IN_IMAGE+1];		//横幅（以NULL为结束标志）

	u8          u8EncMapNumber;                         //用于编码的map编号
	u8			*pu8Bitstream;							//输出码流缓冲
	l32			l32IsKeyFrame;							//编码帧类型（输入） 1: 强制编I帧  0：编码器自定
	u8			*pu8MotionInfo;							//运动信息（不使用必须置为NULL，44x36块等级0~9）
	l32 	    l32MotionInfoLen;		                //运动信息长度
	l32	   		l32OutputPSNR;							//是否计算PSNR值

	u16			u16MergeStyle;							//画面合成模式
	u8			u8BackgroundR;							//画面合成背景色R
	u8			u8BackgroundG;							//画面合成背景色G
	u8			u8BackgroundB;							//画面合成背景色B
    u8          u8BoundaryR;                            //画面合成各路图像统一边界线颜色R
    u8          u8BoundaryG;                            //画面合成各路图像统一边界线颜色G
    u8          u8BoundaryB;                            //画面合成各路图像统一边界线颜色B
    u8          u8BoundaryWidth;                        //画面合成各路图像统一边界线宽度（暂不可设置）
	l32			l32DisplayImageType;					//合成后可直接显示图象类型（P、N制: P制0， N制1）
	u8			*pu8DisplayImage;						//合成后可直接显示图象（为NULL不输出,非图像合成模式除调试外必须填成NULL，以节省cpu）
	l32			l32SkipPreprocess;						//前处理开关, 由外部控制是否做前处理	
	l32			l32OutPutMode;							//画面合成开关，在264中表现为输出图像是以D1或Cif为底图的图像，其中置为1表示Cif输出
}TEncoderInput;		//编码输入参数


typedef struct tEncoderOutput
{
	l32		l32BitstreamLen;			//输出码流长度
	l32		l32IsKeyFrame;				//编码帧类型（输出） 1: 编码输出I帧  0：编码输出P帧
	l32		l32EncodeWidth;				//实际编码宽度
	l32		l32EncodeHeight;			//实际编码高度
	float	fPSNR;						//PSNR值
	u32		u32ConsumedCycles;			//编码消耗周期数
	l32		l32MotionInfoValid;			//运动信息是否有效
	l32		l32LogoNum;					//图标数(实际显示)
	l32		l32BannerNum;				//横幅数(实际显示)
    //以下为测试接口
    u8      *pu8PpImage;                //前处理后未压缩至编码器的图像
    u8      *pu8Pp2encImage;            //前处理压缩至编码器的图像
    
}TEncoderOutput;		//编码输出参数


typedef struct tDecParam
{
	u32		u32DecoderType;					//解码器类型
	l32		l32PacketMode;					//打包模式
	l32		l32DecodeYUVType;				//解码输出YUV类型（420、422、444） 目前仅支持420的图像输出，如需支持其它格式，需要imageprocess库提供支持
	u32		u32DecodeCycles;				//解码一帧可用周期数，目前用于传输解码map的主频(以兆为单位)
	
	l32		l32Width;						//解码图象宽度(查询状态使用)
	l32		l32Height;						//解码图象高度(查询状态使用)
	
}TDecParam;		//解码初始化参数


typedef struct tDecoderInput
{
	u8			*pu8YUV;								//实际输出图象缓冲
	u8			*pu8Bitstream;							//输入码流缓冲
	l32			l32BitstreamLen;						//输入码流长度
	l32			l32OutputType;							//输出图象格式（原始大小，1/4，1/9，1/16,1/1 , 640x480 ,800x600,1024x768）
	TLogo		*aptLogo[MAX_LOGOS_IN_IMAGE+1];			//图标（以NULL为结束标志）
	TBanner		*aptBanner[MAX_BANNERS_IN_IMAGE+1];		//横幅（以NULL为结束标志）
	u8			*pu8MotionInfo;							//运动信息（不使用必须置为NULL）
	l32			l32MotionInfoLen;						//运动信息长度
	l32         l32PostProcess;			                //是否作后处理 1：是 0：否
	l32         l32SizeChangeEdge;                      //图像是否加边框 1：是 0：否 默认为0。（不加边框,目前仅针对1024x768到720x576的图像使用）
	
	
}TDecoderInput;		//解码参数


typedef struct tDecoderOutput
{
	TImage	    tImage;					//解码实际输出图象
	l32			l32DecodeWidth;			//解码图象宽度
	l32			l32DecodeHeight;		//解码图象高度
	l32			l32IsKeyFrame;			//解码帧类型
	u32			u32ConsumedCycles;		//解码消耗周期数
	l32			l32MotionInfoValid;		//运动信息是否有效
	l32			l32LogoNum;				//图标数
	l32			l32BannerNum;			//横幅数
    //以下为测试接口
    u8          *pu8Decded;	            //解码输出的原始图像
    
}TDecoderOutput;			//解码参数


typedef struct
{
	u32		u32EncoderType;	             //编码器类型
    u8      u8InUseFlag;                 //编码器是否在使用中的标志
    VideoCodecHandle pEncoderHandle;     //具体类型编码器
	TImage	tImageModel;                 //前处理背景图像信息
    TImage	tImageProcessed;             //前处理输出至编码器的信息
    u8      u8CurBgR;                    //保存当前前处理背景色
    u8      u8CurBgG;
    u8      u8CurBgB;
    u16     u16CurMergeStyle;            //当前的图像合成类型
    u32     u32CapEncUsableCycles;       //由编码map主频得到的编码一帧可用周期数，采集编码类型
    u32     u32MergeEncUsableCycles;     //由编码map主频得到的编码一帧可用周期数，图像合成编码类型
    u32     u32PreviousEncCycles;        //前一帧编码消耗的指令周期数
    l32		l32CurFrameRate;		     //当前编码器的帧率
    u8      *pu8FrameRateControlTable;   //指向编码帧率控制表
    u16     u16FrameCount;               //编码帧计数，用于帧率控制
    l32		l32SkipPreprocess;			 //前处理开关，用于H263编码VGA按GOB编码，前处理一帧调用一次
}TEncoder;


typedef struct
{
	u32		u32DecoderType;              //解码器类型
    u8      u8InUseFlag;                 //解码器是否在使用中的标志
    VideoCodecHandle pDecoderHandle;     //具体类型解码器
	TImage	tImageDecoded;               //解码输出图像信息
	l32		l32DecodeYUVType;            //解码最后输出的YUV格式
	
}TDecoder;


/*====================================================================
	函数名		：ResetLogoStatus
	功能		：重置图标状态
	算法实现	：无
	引用全局变量：无
    输入参数说明：TLogo *ptLogo（图标）
	输出参数说明：无
	返回值说明  ：VIDEOSUCCESS：成功，VIDEOFAILURE：失败
====================================================================*/
l32 ResetLogoStatus(TLogo *ptLogo);


/*====================================================================
	函数名		：VideoInitial
	功能		：视频编解码外围初始化
	算法实现	：无
	引用全局变量：无
    输入参数说明：u8 u8EncFlag       是否进行编码器外围初始化（ENC_INIT: 是  NO_ENC: 否）
	              l32 l32EncodeMaxWidth   设备所支持的最大编码宽度
				  l32 l32EncodeMaxHeight  设备所支持的最大编码高度
                  u8 u8DecFlag       是否进行解码器外围初始化（DEC_INIT: 是  NO_DEC: 否）
                  l32 l32DecodeMaxWidth   设备所支持的最大解码宽度
                  l32 l32DecodeMaxHeight  设备所支持的最大解码高度
	输出参数说明：无
	返回值说明  ：VIDEOSUCCESS：成功，VIDEOFAILURE：失败
	举例或特别说明： 目前前处理背景仅支持420格式；
	                 目前输入所有编码器内核的图像统一为420格式，
					 若以后支持其他格式，前处理初始化参数须进行判别，前处理代码内部亦需修改；
====================================================================*/
l32 VideoInitial(u8 u8EncFlag, l32 l32EncodeMaxWidth, l32 l32EncodeMaxHeight, u8 u8DecFlag, l32 l32DecodeMaxWidth, l32 l32DecodeMaxHeight);


/*====================================================================
	函数名		：VideoClose
	功能		：视频编解码外围关闭
	算法实现	：无
	引用全局变量：无
    输入参数说明：u8 u8EncFlag       是否进行编码器外围初始化（ENC_INIT: 是  NO_ENC: 否）
                  u8 u8DecFlag       是否进行解码器外围初始化（DEC_INIT: 是  NO_DEC: 否）
	输出参数说明：无
	返回值说明  ：无
====================================================================*/
void VideoClose(u8 u8EncFlag, u8 u8DecFlag);

/*====================================================================
函数名		: VideoCodecGetVersion
功能		：接口版本信息
算法实现	：（可选项）
引用全局变量：pvVer          版本信息
pl32VerLen     版本信息buffer
输入参数说明：无
输出参数说明：无
返回值说明  ：无　              
====================================================================*/
void VideoCodecGetVersion(void *pvVer, l32 l32BufLen, l32 *pl32VerLen);

/*====================================================================
函数名		: VideoImageProcessGetVersion
功能		：图象处理库版本信息
算法实现	：（可选项）
引用全局变量：pvVer          版本信息
pl32VerLen     版本信息buffer
输入参数说明：无
输出参数说明：无
返回值说明  ：无　              
====================================================================*/
void VideoImageProcessGetVersion(void *pvVer, l32 l32BufLen, l32 *pl32VerLen);

/*====================================================================
	函数名		：VideoEncoderInitial
	功能		：初使化编码器
	算法实现	：无
	引用全局变量：无
    输入参数说明：VideoCodecHandle *ptHandle（编码器存放地址）
                  TEncParam *ptEncParam（编码初始化参数）
	输出参数说明：无
	返回值说明  ：VIDEOSUCCESS：成功，VIDEOFAILURE：失败
	举例或特别说明： 目前前处理背景仅支持420格式；
	                 目前输入所有编码器内核的图像统一为420格式；
====================================================================*/
l32 VideoEncoderInitial(VideoCodecHandle *ptHandle, TEncParam *ptEncParam);


/*====================================================================
	函数名		：VideoEncoderEncode
	功能		：编码一帧图象
	算法实现	：qcif, cif, 2cif, 4cif, vga, svga, xvga are supported
	引用全局变量：无
    输入参数说明：VideoCodecHandle ptHandle（编码器）, TEncoderInput *ptEncoderInput（编码输入）, TEncoderOutput *ptEncoderOutput（编码输出）
	输出参数说明：无
	返回值说明  ：VIDEOSUCCESS：成功，VIDEOFAILURE：失败，VIDEOSKIP：跳过
====================================================================*/
l32 VideoEncoderEncode(VideoCodecHandle ptHandle, TEncoderInput *ptEncoderInput, TEncoderOutput *ptEncoderOutput);


/*====================================================================
函数名		：VideoEncoderEncodeStage0
功能		：编码一帧图象
算法实现	：qcif, cif, 2cif, 4cif, vga, svga, xvga are supported
引用全局变量：无
输入参数说明：VideoCodecHandle ptHandle（编码器）, TEncoderInput *ptEncoderInput（编码输入）, TEncoderOutput *ptEncoderOutput（编码输出）
输出参数说明：无
返回值说明  ：VIDEOSUCCESS：成功，VIDEOFAILURE：失败，VIDEOSKIP：跳过
====================================================================*/
l32 VideoEncoderEncodeStage0(VideoCodecHandle ptHandle, TEncoderInput *ptEncoderInput, TEncoderOutput *ptEncoderOutput);


/*====================================================================
函数名		：VideoEncoderEncodeStage1
功能		：编码一帧图象
算法实现	：qcif, cif, 2cif, 4cif, vga, svga, xvga are supported
引用全局变量：无
输入参数说明：VideoCodecHandle ptHandle（编码器）, TEncoderInput *ptEncoderInput（编码输入）, TEncoderOutput *ptEncoderOutput（编码输出）
输出参数说明：无
返回值说明  ：VIDEOSUCCESS：成功，VIDEOFAILURE：失败，VIDEOSKIP：跳过
====================================================================*/
l32 VideoEncoderEncodeStage1(VideoCodecHandle ptHandle, TEncoderInput *ptEncoderInput, TEncoderOutput *ptEncoderOutput);


/*====================================================================
	函数名		：VideoEncoderClose
	功能		：关闭编码器
	算法实现	：无
	引用全局变量：无
    输入参数说明：VideoCodecHandle ptHandle（编码器）
	输出参数说明：无
	返回值说明  ：VIDEOSUCCESS：成功，VIDEOFAILURE：失败
====================================================================*/
l32 VideoEncoderClose(VideoCodecHandle ptHandle);


/*====================================================================
	函数名		：VideoEncoderSetParam
	功能		：设置编码器参数
	算法实现	：无
	引用全局变量：无
    输入参数说明：VideoCodecHandle ptHandle（编码器）, TEncParam *ptEncParam（编码设置参数）
	输出参数说明：无
	返回值说明  ：VIDEOSUCCESS：成功，VIDEOFAILURE：失败
	举例或特别说明：目前调用该函数可改变的参数不包括编码宽度和高度，如需改变
	                编码图像的宽高，须先关闭原编码器，再按新的宽高初始化新的编码器
====================================================================*/
l32 VideoEncoderSetParam(VideoCodecHandle ptHandle, TEncParam *ptEncParam);


/*====================================================================
	函数名		：VideoEncoderGetStatus
	功能		：获取编码器参数
	算法实现	：无
	引用全局变量：无
    输入参数说明：VideoCodecHandle ptHandle（编码器）, TEncParam *ptEncParam（编码器参数）
	输出参数说明：无
	返回值说明  ：VIDEOSUCCESS：成功，VIDEOFAILURE：失败
====================================================================*/
l32 VideoEncoderGetStatus(VideoCodecHandle ptHandle, TEncParam *ptEncParam);


/*====================================================================
	函数名		：VideoEncoderGetVersion
	功能		：获取编码器版本
	算法实现	：无
	引用全局变量：无
    输入参数说明：void *pvVer（输出缓存）, l32 l32BufLen（缓存长度）, l32 *pl32VerLen（信息长度）, u32 u32EncoderType（编码器类型）
	输出参数说明：无
	返回值说明  ：VIDEOSUCCESS：成功，VIDEOFAILURE：失败
====================================================================*/
l32 VideoEncoderGetVersion(void *pvVer, l32 l32BufLen, l32 *pl32VerLen, u32 u32EncoderType);


/*====================================================================
	函数名		：VideoEncoderDumpStatus
	功能		：打印编码器信息
	算法实现	：无
	引用全局变量：无
    输入参数说明：VideoCodecHandle ptHandle（编码器）
	输出参数说明：无
	返回值说明  ：VIDEOSUCCESS：成功，VIDEOFAILURE：失败
====================================================================*/
l32 VideoEncoderDumpStatus(VideoCodecHandle ptHandle);


/*====================================================================
	函数名		：VideoDecoderInitial
	功能		：初使化解码器
	算法实现	：无
	引用全局变量：无
    输入参数说明：VideoCodecHandle *ptHandle（解码器存放地址）
                  TDecParam *ptDecParam（解码初始化参数）
	输出参数说明：无
	返回值说明  ：VIDEOSUCCESS：成功，VIDEOFAILURE：失败
	举例或特别说明： 目前所有解码器内核输出的图像统一为420格式，再根据用户在ptDecParam->l32DecodeYUVType
	                 设置的输出格式作相应转化（420->422,444）
====================================================================*/
l32 VideoDecoderInitial(VideoCodecHandle *ptHandle, TDecParam *ptDecParam);


/*====================================================================
	函数名		：VideoDecoderDecode
	功能		：解码一帧图象
	算法实现	：无
	引用全局变量：无
    输入参数说明：VideoCodecHandle ptHandle（解码器）, TDecoderInput *ptDecoderInput（编码输入）, TDecoderOutput *ptDecoderOutput（编码输出）
	输出参数说明：无
	返回值说明  ：VIDEOSUCCESS：成功，VIDEOFAILURE：失败
====================================================================*/
l32 VideoDecoderDecode(VideoCodecHandle ptHandle, TDecoderInput *ptDecoderInput,  TDecoderOutput *ptDecoderOutput);


/*====================================================================
	函数名		：VideoDecoderClose
	功能		：关闭解码器
	算法实现	：无
	引用全局变量：无
    输入参数说明：VideoCodecHandle ptHandle（解码器）
	输出参数说明：无
	返回值说明  ：VIDEOSUCCESS：成功，VIDEOFAILURE：失败
====================================================================*/
l32 VideoDecoderClose(VideoCodecHandle ptHandle);


/*====================================================================
	函数名		：VideoDecoderSetPatam
	功能		：设置解码器参数
	算法实现	：无
	引用全局变量：无
    输入参数说明：VideoCodecHandle ptHandle（解码器）, TDecParam *ptDecParam（解码设置参数）
	输出参数说明：无
	返回值说明  ：VIDEOSUCCESS：成功，VIDEOFAILURE：失败
====================================================================*/
l32 VideoDecoderSetPatam(VideoCodecHandle ptHandle, TDecParam *ptDecParam);


/*====================================================================
	函数名		：VideoDecoderGetStatus
	功能		：获取解码器参数
	算法实现	：无
	引用全局变量：无
    输入参数说明：VideoCodecHandle ptHandle（解码器）, TDecParam *ptDecParam（解码器参数）
	输出参数说明：无
	返回值说明  ：VIDEOSUCCESS：成功，VIDEOFAILURE：失败
====================================================================*/
l32 VideoDecoderGetStatus(VideoCodecHandle ptHandle, TDecParam *ptDecParam);


/*====================================================================
	函数名		：VideoDecoderGetVersion
	功能		：获取解码器版本
	算法实现	：无
	引用全局变量：无
    输入参数说明：void *pvVer（输出缓存）, l32 l32BufLen（缓存长度）, l32 *pl32VerLen（信息长度）, u32 u32EncoderType（编码器类型）
	输出参数说明：无
	返回值说明  ：VIDEOSUCCESS：成功，VIDEOFAILURE：失败
====================================================================*/
l32 VideoDecoderGetVersion(void *pvVer, l32 l32BufLen, l32 *pl32VerLen, u32 u32DecoderType);


/*====================================================================
	函数名		：VideoDecoderDumpStatus
	功能		：打印解码器信息
	算法实现	：无
	引用全局变量：无
    输入参数说明：VideoCodecHandle ptHandle（解码器）
	输出参数说明：无
	返回值说明  ：VIDEOSUCCESS：成功，VIDEOFAILURE：失败
====================================================================*/
l32 VideoDecoderDumpStatus(VideoCodecHandle ptHandle);


/*====================================================================
函数名	    ： LogoDumpStatus
功能	    ： 图标与背景图像的状态信息
算法实现    ： 无
引用全局变量： 无
输入参数说明：TLogo *ptLogo      输入的图标信息指针组
输出参数说明： 返回值为图标相对于背景图像运动的尾部位置
			   正值表示图标朝着图像边缘运动，负值表示图标远离图像边缘运动
			   优先计算图标的水平运动，其次为垂直运动，如为零值表示图标无运动
返回值说明  ：VIDEOSUCCESS：成功，VIDEOFAILURE：失败
====================================================================*/
l32 LogoDumpStatus(TLogo *ptLogo);

#endif	//not VIDEO_CODEC_INTERFACE_VER1
