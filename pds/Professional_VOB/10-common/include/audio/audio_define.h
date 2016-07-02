/******************************************************************************
模块名      :  AUDIO_DEFINE               
文件名      :  AUDIO_DEFINE.h
相关文件    :  无
文件实现功能:  音频对外常量、宏定义
---------------------------------------------------------------------------------------
修改记录:
日  期          版本            修改人          走读人    修改内容
09/26/2007      1.0             陆志洋                    创建
09/27/2007      1.1             姜  莎                    添加AEC相关宏和枚举类型
10/30/2007      1.2             徐  超                    添加混音器相关宏和枚举类型
11/16/2007      1.3             徐  超                    修改了g7221c内存大小宏定义
                                                          及编解码器最大内存大小宏定义
06/28/2012      1.4             陆志洋                    增加AMR_NB相关定义
11/21/2012      1.5             陆志洋                    增加AMR_WB相关定义
12/20/2012      1.6             陆志洋                    增加G726相关定义
05/06/2013      1.7             徐  超                    增加MP2相关定义
09/03/2013      1.8             徐  超                    增加MAEC相关定义
**************************************************************************************/
#ifndef _AUDIO_DEFINE_H_
#define _AUDIO_DEFINE_H_
#ifdef __cplusplus
extern "C" {
#endif   /* __cplusplus */

#define AUDIO_SUCCESS          0      //成功返回值        
#define AUDIO_FAILURE          (-1)   //失败返回值

/*************common 内存空间大小************/
#define AUDIO_VER_LEN          128    //版本号需要的最小内存空间大小

/***********codec 内存空间大小***************/
#define AUDENC_MAX_MEM_HANDLE  92504   //音频编码器句柄需要的最大内存空间大小 
#define AUDDEC_MAX_MEM_HANDLE  69944   //音频解码器句柄需要的最大内存空间大小
#define AUDMIX_MAX_MEM_SIZE    18696   //音频混音器需要的内存空间大小

/*************audio_process 内存空间大小*********/
#define AGC_MEM_HANDLE  66144 + 256                           //增益控制模块需要的内存空间大小
#define RESAMPLE_MEM_HANDLE    25344                          //采样率变换需要的内存空间大小
#define AUDPROC_MAX_MEM_SIZE   (25528 + RESAMPLE_MEM_HANDLE)  //音频处理需要的内存空间大小
#define AUDPROC_AGC_MAX_MEM_SIZE   (25528 + RESAMPLE_MEM_HANDLE + AGC_MEM_HANDLE)  //音频处理增加增益控制模块需要的内存空间大小

/*************aec 内存空间大小*******************/
#define AEC8K_SLOWMEM_SIZE           58488   //8k AEC v100片外内存大小(Byte) 
#define AEC8K_FASTMEM_SIZE           0       //8k AEC v100片内内存大小(Byte)

#define AEC16K_SLOWMEM_SIZE          120500  //16k AEC v100片外内存大小(Byte)
#define AEC16K_FASTMEM_SIZE          0       //16k AEC v100片内内存大小(Byte)

#define AEC32K_SLOWMEM_SIZE          206076  //32k AEC v100片外内存大小(Byte)
#define AEC32K_FASTMEM_SIZE          0       //32k AEC v100片内内存大小(Byte)

#define AEC32K_V200_SLOWMEM_SIZE     275824  //32k AEC v200片外内存大小(Byte)
#define AEC32K_V200_FASTMEM_SIZE     0       //32k AEC v200片内内存大小(Byte)

#define SAEC32K_V100_SLOWMEM_SIZE    690424  //32k SAEC(立体声AEC)v100片外内存大小(Byte)
#define SAEC32K_V100_FASTMEM_SIZE    0       //32k SAEC(立体声AEC)v100片内内存大小(Byte)

#define SAEC32K_V200_SLOWMEM_SIZE    400428  //32k SAEC(立体声AEC)v200片外内存大小(Byte)
#define SAEC32K_V200_FASTMEM_SIZE    0       //32k SAEC(立体声AEC)v200片内内存大小(Byte)
  
#define MAEC_SLOWMEM_SIZE            1797120 //32k 多声道片外内存大小(Byte)  

/*************语音增强SPE内存空间大小*********/
#define SPE_MAX_MEM_HANDLE           104400   //语音增强句柄需要的最大内存空间大小

/*************equalizer内存空间大小*********/
#define EQ_MAX_MEM_HANDLE           14832     //数字均衡句柄需要的最大内存空间大小

/*************AFC内存空间大小*********/
#define AFC_MAX_MEM_HANDLE          69632    //啸叫抑制句柄需要的最大内存空间大小

/*************mixer*****************/
#define AUDMIX_MAX_CHANNEL           32      //最大混音路数
#define AUDMIX_MAX_SAMPLE_COUNT      2304    //混音器每路每帧输入的最大样本数
                                             //注意: 16位混音每个样本2个字节, 32位混音每个样本4个字节

//混音模式定义
typedef enum
{
    AUD_MIX16_MODE,                //16位混音模式
    AUD_MIX32_MODE,                //32位混音模式
    AUD_MIX16_N_MODE               //简化16位混音模式(仅有N模式输出)
}TEnumAudMixMode;

/***********codec***************/
//音频编码CODEC模式
typedef  enum
{
	AUDENC_G711A = 0,              //g.711的A律编码器
	AUDENC_G711U,                  //g.711的u律编码器
    AUDENC_G722,                   //g.722编码器
    AUDENC_G7231,                  //g.7231编码器
	AUDENC_G728,                   //g.728编码器
	AUDENC_G729,                   //g.729编码器
	AUDENC_ADPCM,                  //adpcm编码器
	AUDENC_G7221C,                 //g.7221c编码器
	AUDENC_MP3,                    //MP3编码器
	AUDENC_HUFFMAN,                //Huffman编码器(属于MP3中子模块)
	AUDENC_AACLC,                  //AAC低复杂度编码器
	AUDENC_AACLD,                  //AAC低延时编码器
	AUDENC_EAACPLUS,               //eAACPlus编码器
	AUDENC_G719,                   //g.719编码器
    AUDENC_AMR_NB,                 //AMR窄带编码器
	AUDENC_AMR_WB,				   //AMR_WB编码器
	AUDENC_G726,                   //g.726编码器(输入pcm格式码流)
	AUDENC_G726a,                  //g.726的A律编码器(输入a律格式码流)
	AUDENC_G726u,                  //g.726的u律编码器(输入u律格式码流)
	AUDENC_MP2,                    //MP2编码器
	AUDENC_NONE
}TEnumAudEncCodec; 

//音频解码CODEC模式
typedef  enum
{
	AUDDEC_G711A = 0,              //g.711的A律解码器
	AUDDEC_G711U,                  //g.711的u律解码器
    AUDDEC_G722,                   //g722解码器
    AUDDEC_G7231,                  //g7231解码器
	AUDDEC_G728,                   //g.728解码器
	AUDDEC_G729,                   //g.729解码器
	AUDDEC_ADPCM,                  //adpcm解码器
	AUDDEC_G7221C,                 //g.7221c解码器
	AUDDEC_MP3,                    //MP3解码器
	AUDDEC_HUFFMAN,                //Huffman解码器(属于MP3中子模块)
	AUDDEC_AACLC,                  //AAC低复杂度解码器
	AUDDEC_AACLD,                  //AAC低延时解码器
	AUDDEC_EAACPLUS,               //eAACPlus解码器
	AUDDEC_G719,                   //g.719解码器
	AUDDEC_AMR_NB,                 //AMR窄带解码器
	AUDDEC_AMR_WB,                 //AMR窄带解码器
	AUDDEC_G726,                   //g.726解码器(输出pcm格式码流)
	AUDDEC_G726a,                  //g.726的A律解码器(输出a律格式码流)
	AUDDEC_G726u,                  //g.726的u律解码器(输出u律格式码流)
	AUDDEC_MP2,                    //MP2解码器
	AUDDEC_NONE
}TEnumAudDecCodec;

//音频解码参数扩展类型
typedef enum
{
    AUDDEC_PLC_PARAM = 100         // 解码参数扩展类型
}TEnumAudDecParamReservedType;

//音频解码输入扩展类型
typedef enum
{
	AUDDEC_PLC_INPUT = 200         // 解码输入扩展类型
}TEnumAudDecInputReservedType;

//是否启用丢包隐藏的标志
typedef enum
{
    AUDPLC_OFF = 0,                // 关闭丢包隐藏
    AUDPLC_ON                      // 开启丢包隐藏
}TEnumAudPLCSwitchFLAG;

//音频帧判断标志
typedef enum
{
    AUDFRAME_GOOD = 0,            // 正确帧
    AUDFRAME_BAD                  // 错误帧或丢失帧
}TEnumAudPLCFrameFLAG;

//音频采样率模式
typedef enum
{
	SAMPLE_RATE_8K = 8000,         //采样率为8kHz
	SAMPLE_RATE_16K = 16000,       //采样率为16kHz
	SAMPLE_RATE_24K = 24000,       //采样率为24kHz
	SAMPLE_RATE_32K = 32000,       //采样率为32kHz
	SAMPLE_RATE_44K = 44100,       //采样率为44.1kHz
	SAMPLE_RATE_48K = 48000        //采样率为48kHz
}TEnumAudSampleRate;

//音频采样率索引
typedef enum
{
    SAMPLE_RATE96000_INDEX = 0,    //采样率为96kHz的索引是0
    SAMPLE_RATE88200_INDEX,        //采样率为88.2kHz的索引是1
    SAMPLE_RATE64000_INDEX,        //采样率为64kHz的索引是2
    SAMPLE_RATE48000_INDEX,        //采样率为48kHz的索引是3
    SAMPLE_RATE44100_INDEX,        //采样率为44.1kHz的索引是4
    SAMPLE_RATE32000_INDEX,        //采样率为32kHz的索引是5
    SAMPLE_RATE24000_INDEX,        //采样率为24kHz的索引是6  
    SAMPLE_RATE22050_INDEX,        //采样率为22.05kHz的索引是7
    SAMPLE_RATE16000_INDEX,        //采样率为16kHz的索引是8
    SAMPLE_RATE12000_INDEX,        //采样率为12kHz的索引是9
    SAMPLE_RATE11025_INDEX,        //采样率为11.025kHz的索引是10 
    SAMPLE_RATE8000_INDEX          //采样率为8kHz的索引是11
}TEnumAudSampleRateIndex;

//样本位宽模式
typedef enum
{
	BIT_WIDTH_16 = 16,             //样本位宽为16bit
	BIT_WIDTH_32 = 32              //样本位宽为32bit
}TEnumAudBitWidth;                 
                                   
//声道模式                         
typedef enum                       
{                                  
	CH_MONO = 1,                   //单声道
	CH_STEREO                      //双声道
}TEnumAudChannel;

//编码比特率
typedef enum
{
	G7221C_MO_32K_24K = 24000,     //命名方式：算法_声道_采样率_比特率
	G7221C_MO_32K_32K = 32000,
	G7221C_MO_32K_48K = 48000,
	G719_MO_48K_64K = 64000,
	G722_MO_16K_64K = 64000,
	G7231_MO_8K_53K = 53000,
    G7231_MO_8K_63K = 63000,
	AACLC_MO_16K_48K = 48000,
	AACLC_MO_32K_72K = 72000,
	AACLC_MO_48K_128K = 128000,
	AACLC_ST_16K_48K = 48000,
	AACLC_ST_32K_72K = 72000,
	AACLC_ST_48K_128K = 128000,
	AACLD_ST_24K_64K = 64000,
	AACLD_ST_32K_96K = 96000,
	AACLD_ST_48K_128K = 128000,
	AACLD_MO_24K_32K = 32000,
	AACLD_MO_32K_48K = 48000,
	AACLD_MO_48K_64K = 64000,
	MP2_32K = 32000,
	MP2_64K = 64000,
	MP2_128K = 128000
}TEnumAudBitRate;

//AMR NB比特率模式
typedef enum
{ 
	MR475 = 0,
	MR515,            
	MR59,
	MR67,
	MR74,
	MR795,
	MR102,
	MR122
}TEnumAmrNbBitRateMode;

//AMR WB比特率模式
typedef enum
{ 
    MR66 = 0,
    MR885,            
    MR1265,
    MR1425,
    MR1585,
    MR1825,
    MR1985,
    MR2305,
    MR2385
}TEnumAmrWbBitRateMode;

//G726比特率模式
typedef enum
{ 
    G726_40K = 5,
    G726_32k = 4,
    G726_24k = 3,
    G726_16k = 2 
}TEnumG726BitRateMode;

/*************AGC*******************/

/*************aec*******************/
//AEC类型
typedef  enum
{
	AEC_TYPE_8K = 8,               //单声道AEC，8k采样率
	AEC_TYPE_16K = 16,             //单声道AEC，16k采样率
	AEC_TYPE_32K = 32,             //单声道AEC，32k采样率
	AEC_MIX_TYPE_32K = 33,         //用于高清64x平台，单双声道AEC混合使用时，单声道32k AEC的标志
	                               //与AEC_TYPE_32K不同之处在于api的封装使用的片外内存大小不同
	SAEC_TYPE_32K = 64             //双声道AEC，32k采样率
}TAecType;

//错误码定义
//其中大模块定义（占用4个bit位：28、29、30、 31位）为
#define ERR_AUDIOENC_MOD     (1 << 29)     //音频编码模块
#define ERR_AUDIODEC_MOD     (2 << 29)     //音频解码模块
#define ERR_AUDPROCESS_MOD   (3 << 29)     //音频其它模块

//子模块定义（占用8个bit为，从20-27位)为
#define ERR_G711ENC          (1 << 20)     //G711编码子模块   
#define ERR_G711DEC          (2 << 20)     //G711解码子模块  

#define ERR_G722ENC          (3 << 20)     //G722编码子模块
#define ERR_G722DEC          (4 << 20)     //G722 解码子模块

#define ERR_G7221CENC        (5 << 20)     //G7221C编码子模块
#define ERR_G7221CDEC        (6 << 20)     //G7221C解码子模块

#define ERR_G728ENC          (7 << 20)     //G728编码子模块
#define ERR_G728DEC          (8 << 20)     //G728解码子模块

#define ERR_G729ENC          (9 << 20)     //G729编码子模块
#define ERR_G729DEC          (10 << 20)    //G729解码子模块

#define ERR_MP3ENC           (11 << 20)    //MP3编码子模块
#define ERR_MP3DEC           (12 << 20)    //MP3解码子模块

#define ERR_ADPCMENC         (13 << 20)    //ADPCM编码子模块
#define ERR_ADPCMDEC         (14 << 20)    //ADPCM解码子模块

#define ERR_AACLCENC         (15 << 20)    //AACLC编码子模块    
#define ERR_AACLCDEC         (16 << 20)    //AACLC解码子模块

#define ERR_AACLDENC         (17 << 20)    //AACLD编码子模块    
#define ERR_AACLDDEC         (18 << 20)    //AACLD解码子模块

#define ERR_G7231ENC         (19 << 20)    //G7231编码子模块
#define ERR_G7231DEC         (20 << 20)    //G7231解码子模块

#define ERR_EAACPLUSENC      (21 << 20)    //eAACPlus编码子模块    
#define ERR_EAACPLUSDEC      (22 << 20)    //eAACPlus解码子模块

#define ERR_G719ENC          (23 << 20)    //G719编码子模块    
#define ERR_G719DEC          (24 << 20)    //G719解码子模块

#define ERR_AMR_NB_ENC       (25 << 20)    //AMR_NB编码子模块    
#define ERR_AMR_NB_DEC       (26 << 20)    //AMR_NB解码子模块

#define ERR_AMR_WB_ENC       (27 << 20)    //AMR_WB编码子模块
#define ERR_AMR_WB_DEC       (28 << 20)    //AMR_WB解码子模块

#define ERR_G726ENC          (29 << 20)    //G726编码子模块
#define ERR_G726DEC          (30 << 20)    //G726解码子模块

#define ERR_MP2ENC           (31 << 20)    //MP2编码子模块
#define ERR_MP2DEC           (32 << 20)    //MP2 解码子模块

#define ERR_MAEC32K          (127 << 20)   //MAEC32K子模块  
#define ERR_AEC8K            (128 << 20)   //AEC8K子模块  
#define ERR_AEC16K           (129 << 20)   //AEC16K子模块  
#define ERR_AEC32K           (130 << 20)   //AEC32K子模块  

#define ERR_DTMF             (131 << 20)   //DTMF子模块            
#define ERR_MIXER            (132 << 20)   //混音子模块
#define ERR_SPLIT            (133 << 20)   //SPLIT子模块
#define ERR_RESAMPLE         (134 << 20)   //采样率变换模块 
#define ERR_SAEC32K          (135 << 20)   //SAEC32K子模块 
#define ERR_SPE              (136 << 20)   //语音增强模块 
#define ERR_EQ               (137 << 20)   //数字均衡模块 
#define ERR_AGC              (138 << 20)   //自动增益模块 
#define ERR_AFC              (139 << 20)   //啸叫控制模块
#define ERR_MDD32K           (140 << 20)   //MDD延时检测模块 

#ifdef __cplusplus
}
#endif  /* __cplusplus */           
#endif

