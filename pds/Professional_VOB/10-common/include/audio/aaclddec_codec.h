/******************************************************************************
模块名      ：AacLd解码器
文件名      : aaclddec_codec.h
相关文件    ：无
文件实现功能：AacLc解码器类型错误定义和对外函数声明
作者        ：
版本        ：
-------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/01/02  1.0         姜  莎      创    建
*******************************************************************************/
#ifndef AACLDDEC_CODEC_H
#define AACLDDEC_CODEC_H

#ifdef __cplusplus
extern "C" {
#endif                                         /* __cplusplus */

#include "audio_codec.h"

enum aacld_dec_errcode 
{
	ERR_AACLDDEC_PPVHANDLE_NULL = ERR_AACLDDEC,  //AAC LD解码器句柄为NULL
	ERR_AACLDDEC_DECTYPE,          //AAC LD解码器类型错误
	ERR_AACLDDEC_SLOWMEM_NULL,     //AAC LD解码器片外内存为空
	ERR_AACLDDEC_NEAACDECOPEN,     //AAC LD解码器打开错误
	ERR_AACLDDEC_HANDLE_NULL,      //AAC LD解码器返回的句柄指针为空
	ERR_AACLDDEC_INOUTBUF_NULL,    //AAC LD解码器输出buffer为空
	ERR_AACLDDEC_HEADER,           //AAC LD解码器头错误
	ERR_AACLDDEC_FRAMEDECODE,      //AAC LD解码器解码器一帧错误
	ERR_AACLDDEC_GETSTATUS_NULL,   //AAC LD解码器状态参数结构体指针为空
	ERR_AACLDDEC_VERSION,          //AAC LD解码器获取版本错误
	ERR_AACLDDEC_CLOSE_NULL,       //AAC LD解码器关闭句柄指针为空
	ERR_AACLDDEC_CLOSE,            //AAC LD解码器关闭错误
	ERR_AACDEC_FORMAT,             //AAC LD解码器格式错误
	ERR_AACLDDEC_QTALBLE,          //AAC LD解码器量化表格大小错误
	ERR_AACLDDEC_FRAME_LEN,        //AAC LD解码器帧长错误
	ERR_AACLDDEC_MP4_STRUCT_NULL,  //AAC LD解码器MP4结构体指针为空
	ERR_AACLDDEC_OBJ_TYPE,         //AAC LD解码器解码器类型错误
	ERR_AACLDDEC_SAMPLE_INDEX,     //AAC LD解码器采样率索引错误
	ERR_AACLDDEC_SAMPLE_FREQ,      //AAC LD解码器采样率错误
	ERR_AACLDDEC_GA_CONFIG,        //AAC LD解码器GA config错误
	ERR_AACLDDEC_EPCONFIG,         //AAC LD解码器EP config错误
	ERR_AACLDDEC_WIN_SEQUENCE,     //AAC LD解码器window sequence错误
	ERR_AACLDDEC_WIN_SHAPE,        //AAC LD解码器window shape错误
	ERR_AACLDDEC_WIN_INFO,         //AAC LD解码器获取窗信息错误
	ERR_AACLDDEC_MAX_SFB,          //AAC LD解码器最大比例因子频带数错误
	ERR_AACLDDEC_PULSE_START_SFB,  //AAC LD解码器pulse escape数据错误
	ERR_AACLDDEC_PCE,              //AAC LD解码器PCE 错误
	ERR_AACLDDEC_BIT_STREAM_LEN,   //AAC LD解码器一帧比特流长度错误
	ERR_AACLDDEC_CHANNEL_CONFIG    //AAC LD解码器声道设置错误
};                                                       
                                                         
/*====================================================================
函数名      : AacLdDecOpen                          
功能        : 初始化解码器参数                                
算法实现    : 无                                                  
引用全局变量: 无                                                      
参数        : ppvHandle: 指向编码器句柄指针的指针[out]
              ptAudDecParam: 编码器初始化参数结构体指针[in]
              pvAudMemParam:  编码器内存管理结构体的指针[in]              
返回值说明  : 成功: AUDIO_SUCCESS, 失败: 返回错误码                   
====================================================================*/
l32 AacLdDecOpen(void **ppvHandle, TAudDecParam *ptAudDecParam, void *pvAudMemParam);

/*====================================================================
函数名      : AacLdDecProcess                          
功能        : 解码一帧                                
算法实现    : 无                                                  
引用全局变量: 无                                                      
参数        : pvHandle: 指向编码器句柄指针[in]
              ptAudDecInput: 指向编码输入结构体指针[in]
              ptAudDecOutput: 指向编码输出结构体指针[out]              
返回值说明  : 成功: AUDIO_SUCCESS, 失败: 返回错误码                   
====================================================================*/
l32 AacLdDecProcess(void *pvHandle, TAudDecInput *ptAudDecInput, TAudDecOutput *ptAudDecOutput);

/*====================================================================                        
函数名      : AacLdDecClose                                                                 
功能        : 解码器关闭                                                                        
算法实现    : 无                                                                              
引用全局变量: 无                                                                              
参数        : pvHandle: 指向编码器句柄指针[in]                                   
返回值说明  : 成功: AUDIO_SUCCESS, 失败: 返回错误码                                           
====================================================================*/                        
l32 AacLdDecClose(void *pvHandle);

/*====================================================================                        
函数名      : AacLdDecGetStatus                                                                 
功能        : 得到解码器状态信息                                                                        
算法实现    : 无                                                                              
引用全局变量: 无                                                                              
参数        : pvHandle: 指向编码器句柄指针[in]
              ptAudDecStatusParam: 指向编码状态参数结构体指针[in/out]                                 
返回值说明  : 成功: AUDIO_SUCCESS, 失败: 返回错误码                                           
====================================================================*/                        
l32 AacLdDecGetStatus(void *pvHandle, TAudDecStatusParam *ptAudDecStatusParam);

/*====================================================================                        
函数名      : AacLdDecGetVersion                                                                 
功能        : 得到解码器版本信息                                                                        
算法实现    : 无                                                                              
引用全局变量: 无                                                                              
参数        : pvVer: 输出缓存[out]
              l32BufLen: 缓存长度[in]
              pl32VerLen: 信息长度[in/out]                            
返回值说明  : 成功: AUDIO_SUCCESS, 失败: 返回错误码                                           
====================================================================*/                        
l32 AacLdDecGetVersion(void *pvVer, l32 l32BufLen, l32 *pl32VerLen);

/*
//解码器原始的接口，不需要码流长度的txt文件
l32 AacLdDecOpen(void *DecHandle, u8 *InBuf, l32 buffersize);
l32 AacLdDecProcess(void *DecHandle, u8 *InBuf, u8 *OutBuf);
void NeAacLdDecClose(void *pvHandle);
*/

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif

