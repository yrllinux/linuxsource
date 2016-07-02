/******************************************************************************
模块名      ： audio_codec              
文件名      ： audio_codec.h
相关文件    ：无
文件实现功能： 编解码函数参数声明等
---------------------------------------------------------------------------------------
修改记录:
日  期                  版本            修改人     走读人        修改内容
09/26/2007              1.0            陆志洋                    创建
11/16/2009              1.1            陆志洋                    增加编解码参数设置函数声明
06/28/2012              1.2            陆志洋                    增加TAudDtxParam和TAudBitRateModeInput声明
11/21/2012              1.3            陆志洋                    添加注释
**************************************************************************************/
#ifndef _AUDIO_CODEC_H_
#define _AUDIO_CODEC_H_
#ifdef __cplusplus
extern "C" {
#endif   /* __cplusplus */

#define TAudEncStatusParam TAudEncParam
#define TAudDecStatusParam TAudDecParam

//编码器初始化参数结构体
typedef struct
{   u32 u32Size ;         //记录结构体空间大小
    u32 u32EncType;       //音频编码CODEC模式（TEnumAudEncCodec）
    l32 l32Channel;       //声道模式（TEnumAudChannel）
    l32 l32BitWidth;      //样本位宽模式（TEnumAudBitWidth）
    l32 l32SampleRate;    //音频采样率模式（TEnumAudSampleRate）
    l32 l32BitRate;       //比特率模式(TEnumAudBitRate)
    l32 l32FrameLen;      //编码一帧样本数(声道数*单声道每帧的样本数)
    u32 u32FrameHeadFlag; //每帧码流是否带头信息(仅AAC_LC使用，1：有，0：否)
    u32 u32Reserved;      //保留参数
}TAudEncParam;

//编码器初始化参数结构体补充参数(仅AMR_NB、AMR_WB使用)                                                
//注: 本结构体使用时,通过编码器初始化参数结构体(TAudEncParam)的保留参数获得其指针
typedef struct                                 
{ 
	u32 u32EncType;         //编码器类型(TEnumAudEncCodec)
	l32 l32DtxFlag;         //是否使能不连续传输模式(1:使能， 0：不使能)
	u32 u32Reserved;        //保留参数                                                                                                                                                                                                                                                                                               
}TAudDtxParam; 

//编码输入结构体
typedef struct
{
    u32 u32Size;        //记录结构体空间大小
    u8 *pu8InBuf;       //指向编码器输入缓存指针
    u32 u32InBufSize;   //输入缓存字节数 
    u8 *pu8OutBuf;      //指向编码器输出缓存指针
    u32 u32OutBufSize;  //输出缓存字节数  
    u32 u32Reserved;    //保留参数
}TAudEncInput;

//编码输入结构体补充参数(仅AMR_NB、AMR_WB、G726使用)                                                
//注: 本结构体使用时,通过编码器输入结构体(TAudEncInput)的保留参数获得其指针
typedef struct                                 
{ 
	u32 u32EncType;         //编码器类型(TEnumAudEncCodec)
	l32 l32BitRateMode;     //当前帧编码比特率模式(TEnumAmrNbBitRateMode或TEnumAmrWbBitRateMode或TEnumG726BitRateMode)
	u32 u32Reserved;        //保留参数                                                                                                                                                                                                                                                                                               
}TAudBitRateModeInput; 

//编码输出结构体
typedef struct
{   
    u32 u32Size ;             //记录结构体空间大小    
    u32 u32OutBitStreamSize;  //输出码流的字节数
    u32 u32StatisCycles;      //编码一帧效率统计
    u32 u32Reserved;          //保留参数
}TAudEncOutput;

//解码器初始化参数结构体                  
typedef struct                                  
{   
    u32 u32Size ;          //记录结构体空间大小                                            
    u32 u32DecType;        //解码器类型(TEnumAudDecCodec)
    l32 l32BitRate;        //比特率模式
    u32 u32FrameHeadFlag;  //每帧码流是否带头信息(仅AAC使用，1：有，0：否)
    u32 u32Reserved;       //保留参数                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
}TAudDecParam;        

//音频丢包隐藏(PLC)初始化参数结构体
//注: 本结构体使用时,通过解码器初始化结构体的保留参数获得其指针
typedef struct                                 
{ 
	u32 u32ReservedType;  //解码参数扩展类型(AUDDEC_PLC_PARAM)
	u32 u32EnablePlcFlag; //PLC功能开关标志(AUDPLC_OFF: 关闭, AUDPLC_ON:打开)
	u32 u32Reserved;      //保留参数                                                                                                                                                                                                                                                                                              
}TAudPlcParam; 

//解码器输入结构体                                                 
typedef struct                                 
{                                              
	u32 u32Size ;            //记录结构体空间大小
	u8 *pu8InBuf;            //指向解码器输入缓存指针   
	u32 u32InBufSize;        //输入缓存字节数                                                                                                                                                                                                                                                                                      捍嬷刚?
	u8 *pu8OutBuf;           //指向解码器输出缓存指针 
	u32 u32OutBufSize;       //输出缓存字节数                                                                                                                                                                                                                                                                                           
	u32 u32InBitStreamSize;  //每帧码流的字节数
	u32 u32SampelFreIndex;   //每帧采样率索引(仅AAC使用)
	u32 u32Channels;         //每帧通道数  
	u32 u32Reserved;         //保留参数(G726格式时用于定义码率模式：TEnumG726BitRateMode)                                                                                                                                                                                                                                                                                                
}TAudDecInput;      

//音频丢包隐藏(PLC)处理输入参数结构体                                                
//注: 本结构体使用时,通过解码器输入结构体(TAudDecInput)的保留参数获得其指针
typedef struct                                 
{ 
	u32 u32ReservedType;    //解码输入扩展类型(AUDDEC_PLC_INPUT)
	u32 u32ErrorFrameFlag;  //丢帧标志(AUDFRAME_GOOD: 正确帧, AUDFRAME_BAD:错误帧)
	u32 u32Reserved;       //保留参数                                                                                                                                                                                                                                                                                               
}TAudPlcInput; 

//解码器输出结构体
typedef struct
{               
    u32  u32Size;         //记录结构体空间大小
    u32  u32OutFrameLen ; //每帧输出样本数
    u32  u32StatisCycles; //解码一帧效率统计
    u32  u32Reserved;     //保留参数
}TAudDecOutput; 
                                                                                                                     
/*====================================================================
函数名      : AudEncOpen                          
功能        : 初始化编码器参数                                  
算法实现    : 无                                                  
引用全局变量: 无                                                      
参数        : ppvHandle:         指向编码器句柄指针的指针[out]
              ptAudEncParam:     编码器初始化参数结构体指针[in]
              pvAudEncMemParam:  编码器内存管理结构体的指针[in]              
返回值说明  : 成功: AUDIO_SUCCESS, 失败: 返回错误码                   
====================================================================*/
l32 AudEncOpen(void **ppvHandle, TAudEncParam *ptAudEncParam, void *ptAudEncMemParam);   

/*====================================================================
函数名      : AudEncProcess                          
功能        : 编码一帧                                 
算法实现    : 无                                                  
引用全局变量: 无                                                      
参数        : pvHandle:       指向编码器句柄指针[in]
              ptAudEncInput:  指向编码输入结构体指针[in]
              ptAudEncOutput: 指向编码输出结构体指针[out]              
返回值说明  : 成功: AUDIO_SUCCESS, 失败: 返回错误码                   
====================================================================*/
l32 AudEncProcess(void *pvHandle, TAudEncInput *ptAudEncInput, TAudEncOutput *ptAudEncOutput);

/*====================================================================                        
函数名      : AudEncGetStatus                                                                 
功能        : 得到编码器状态信息                                                                        
算法实现    : 无                                                                              
引用全局变量: 无                                                                              
参数        : pvHandle:            指向编码器句柄指针[in]
              ptAudEncStatusParam: 指向编码状态参数结构体指针[in/out]                                 
返回值说明  : 成功: AUDIO_SUCCESS, 失败: 返回错误码                                           
====================================================================*/                        
l32 AudEncGetStatus(void *pvHandle, TAudEncStatusParam *ptAudEncStatusParam);

/*====================================================================                        
函数名      : AudEncClose                                                                 
功能        : 编码器关闭                                                                        
算法实现    : 无                                                                              
引用全局变量: 无                                                                              
参数        : pvHandle: 指向编码器句柄指针[in]                                   
返回值说明  : 成功: AUDIO_SUCCESS, 失败: 返回错误码                                           
====================================================================*/                        
l32  AudEncClose(void *pvHandle);

/*====================================================================                        
函数名      : AudEncGetVersion                                                                 
功能        : 得到编码器版本信息                                                                        
算法实现    : 无                                                                              
引用全局变量: 无                                                                              
参数        : pvVer:      输出缓存[out]
              l32BufLen:  缓存长度[in]
              pl32VerLen: 信息长度[in/out]
              u32EncType: 编码器类型[in]                              
返回值说明  : 成功: AUDIO_SUCCESS, 失败: 返回错误码                                           
====================================================================*/                        
l32 AudEncGetVersion(void *pvVer, l32 l32BufLen, l32 *pl32VerLen, u32 u32EncType);

/*====================================================================                        
函数名      : AudEncSetParams                                                                 
功能        : 设置解码器参数                                                                     
算法实现    : 无                                                                              
引用全局变量: 无                                                                              
参数        : pvHandle:         指向编码器句柄指针[in]
              ptAudEncParam:    编码初始化参数结构体指针[in]                          
返回值说明  : 成功: AUDIO_SUCCESS, 失败: 返回错误码                                           
====================================================================*/                        
l32 AudEncSetParams(void *pvHandle, TAudEncParam *ptAudEncParam);

/*====================================================================
函数名      : AudDecOpen                          
功能        : 初始化解码器参数                                  
算法实现    : 无                                                  
引用全局变量: 无                                                      
参数        : ppvHandle:        指向解码器句柄指针的指针[out]
              ptAudDecParam:    解码初始化参数结构体指针[in]
              ptAudDecMemParam: 解码器内存管理结构体的指针          
返回值说明  : 成功: AUDIO_SUCCESS, 失败: 返回错误码                   
====================================================================*/
l32 AudDecOpen (void **ppvHandle, TAudDecParam *ptAudDecParam, void *ptAudDecMemParam);

/*====================================================================                        
函数名      : AudDecProcess                                                                   
功能        : 解码一帧                                                                        
算法实现    : 无                                                                              
引用全局变量: 无                                                                              
参数        : pvHandle:        指向解码器句柄指针[in]                                               
              ptAudDecInput:   指向解码输入结构体指针[in]                                      
              ptAudDecOutput:  指向解码输出结构体指针[out]                                    
返回值说明  : 成功: AUDIO_SUCCESS, 失败: 返回错误码                                           
====================================================================*/                        
l32 AudDecProcess(void *pvHandle, TAudDecInput *ptAudDecInput, TAudDecOutput *ptAudDecOutput);

/*====================================================================                        
函数名      : AudDecGetStatus                                                                 
功能        : 得到解码器状态信息                                                                        
算法实现    : 无                                                                              
引用全局变量: 无                                                                              
参数        : pvHandle:            指向解码器句柄指针[in]
            ptAudDecStatusParam: 指向解码器状态参数结构体指针[in/out]                               
返回值说明  : 成功: AUDIO_SUCCESS, 失败: 返回错误码                                           
====================================================================*/                        
l32 AudDecGetStatus(void *pvHandle, TAudDecStatusParam *ptAudDecStatusParam);

/*====================================================================                        
函数名      : AudDecClose                                                                 
功能        : 解码器关闭                                                                        
算法实现    : 无                                                                              
引用全局变量: 无                                                                              
参数        : ppvHandle: 指向解码器句柄指针[in]                                  
返回值说明  : 成功: AUDIO_SUCCESS, 失败: 返回错误码                                           
====================================================================*/                        
l32 AudDecClose(void *pvHandle);

/*====================================================================                        
函数名      : AudDecGetVersion                                                                 
功能        : 获得解码器版本信息                                                                      
算法实现    : 无                                                                              
引用全局变量: 无                                                                              
参数        : pvVer:      输出缓存[out]
            l32BufLen:  缓存长度[in]
            pl32VerLen: 信息长度[in/out]
            u32DecType: 解码器类型[in]                            
返回值说明  : 成功: AUDIO_SUCCESS, 失败: 返回错误码                                           
====================================================================*/                        
l32 AudDecGetVersion(void *pvVer, l32 l32BufLen, l32 *pl32VerLen, u32 u32DecType);

/*====================================================================                        
函数名      : AudDecSetParams                                                                 
功能        : 设置解码器参数                                                                     
算法实现    : 无                                                                              
引用全局变量: 无                                                                              
参数        : pvHandle:         指向解码器句柄指针[in]
              ptAudDecParam:    解码初始化参数结构体指针[in]                          
返回值说明  : 成功: AUDIO_SUCCESS, 失败: 返回错误码                                           
====================================================================*/                        
l32 AudDecSetParams(void *pvHandle, TAudDecParam *ptAudDecParam);

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif 

