/******************************************************************************
模块名      ：AacLc解码器
文件名      : aaclcdec_v200.h
相关文件    ：无
文件实现功能：AacLc解码器类型错误定义和对外函数声明
作者        ：陆志洋
版本        ：V200 
-------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/02/22  V200        陆志洋       创    建
*******************************************************************************/
#ifndef AACLCDEC_V200_H
#define AACLCDEC_V200_H

#ifdef __cplusplus
extern "C" {
#endif                                         /* __cplusplus */

#include "audcodec_common.h"

enum aaclc_dec_errcode 
{
	ERR_AACLCDEC_SLOWMEM_NULL = ERR_AACLCDEC, //解码内存为空
	ERR_AACLCDEC_OBJECT_TYPE,                 //解码类型错误
	ERR_AACLCDEC_FRAMEDEC_NULL,               //解码器解码一帧函数中输入句柄为空
	ERR_AACLCDEC_NO_BITS_READ,                //解码器没有码流可读取
	ERR_AACLCDEC_SLOWMEM_SMALL,               //解码句柄内存不足
	ERR_AACLCDEC_DECTYPE,                     //解码类型错误
	ERR_AACLCDEC_BITSIZE,                     //码流长度错误
	ERR_AACLCDEC_HANDLE_NULL,                 //解码句柄为空错误
	ERR_AACLCDEC_INOUTBUF_NULL,               //解码输入缓冲为空错误
	ERR_AACLCDEC_VERSION,                     //解码版本错误 
	ERR_AACLCDEC_CLOSE,                       //解码释放内存错误
	ERR_AACLCDEC_CLOSE_NULL,                  //解码释放内存为空
	ERR_AACLCDEC_PPVHANDLE_NULL,              //ppvHandle为空
	ERR_AACLCDEC_NEAACDECOPEN,                //NeAACDecOpen错误
	ERR_AACLCDEC_NEAACDECCONFIG,              //NeAACDecSetConfiguration错误
	ERR_AACLCDEC_NEAACDECINIT,                //NeAACDecInit错误
	ERR_AACLCDEC_FRAMEDECODE,                 //aac_frame_decode错误
	ERR_AACLCDEC_ADTSHEAD,                    //ADTS头出错
	ERR_AACLCDEC_GETSTATUS_NULL,              //状态信息句柄为空
	ERR_AACLCDEC_NEAACDECOPEN_FREE,           //NeAACDecOpen释放内存错误
	ERR_AACLCDEC_NEAACDECCONFIG_FREE          //NeAACDecSetConfiguration释放内存错误
};

/*====================================================================
函数名      : AacLcDecOpen                          
功能        : 初始化解码器参数，用在测试中                                 
算法实现    : 无                                                  
引用全局变量: 无                                                      
参数        : ppvHandle            指向编码器句柄指针的指针[out]
              ptAudDecParam        编码器初始化参数结构体指针[in]
              pvAudMemParam        编码器内存管理结构体的指针[in]              
返回值说明  : 成功: AUDIO_SUCCESS
              失败: 返回错误码                   
====================================================================*/
l32 AacLcDecOpen(void **ppvHandle, TAudioDecParam *ptAudioDecParam);

/*====================================================================
函数名      : AacLcDecProcess                          
功能        : 解码一帧，用在测试中                                  
算法实现    : 无                                                  
引用全局变量: 无                                                      
参数        : pvHandle                指向编码器句柄指针[in]
              ptAudDecInput           指向编码输入结构体指针[in]
              ptAudDecOutput          指向编码输出结构体指针[out]              
返回值说明  : 成功: AUDIO_SUCCESS
              失败: 返回错误码                   
====================================================================*/
l32 AacLcDecProcess(void *pvHandle, TAudDecInput *ptAudDecInput, TAudDecOutput *ptAudDecOutput);

/*====================================================================                        
函数名      : AacLcDecClose                                                                 
功能        : 解码器关闭                                                                        
算法实现    : 无                                                                              
引用全局变量: 无                                                                              
参数        : pvHandle          指向编码器句柄指针[in]                                   
返回值说明  : 成功: AUDIO_SUCCESS
              失败: 返回错误码                                           
====================================================================*/                        
l32 AacLcDecClose(void *pvHandle);

/*====================================================================                        
函数名      : AacLcDecGetStatus                                                                 
功能        : 得到解码器状态信息                                                                        
算法实现    : 无                                                                              
引用全局变量: 无                                                                              
参数        : pvHandle                 指向编码器句柄指针[in]
              ptAudDecStatusParam      指向编码状态参数结构体指针[in/out]                                 
返回值说明  : 成功: AUDIO_SUCCESS
              失败: 返回错误码                                           
====================================================================*/                        
l32 AacLcDecGetStatus(void *pvHandle, TAudioDecStatusParam *ptAudioDecStatusParam);

/*====================================================================                        
函数名      : AacLcDecGetVersion                                                                 
功能        : 得到解码器版本信息                                                                        
算法实现    : 无                                                                              
引用全局变量: 无                                                                              
参数        : pvVer            输出缓存[out]
              l32BufLen        缓存长度[in]
              pl32VerLen       信息长度[in/out]                            
返回值说明  : 成功: AUDIO_SUCCESS
              失败: 返回错误码                                           
====================================================================*/                        
l32 AacLcDecGetVersion(void *pvVer, l32 l32BufLen, l32 *pl32VerLen);

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif

