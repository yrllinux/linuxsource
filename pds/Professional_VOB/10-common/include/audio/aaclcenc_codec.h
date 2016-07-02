/******************************************************************************
模块名      ：AacLc编码器
文件名      : aaclcenc_codec.h
相关文件    ：无
文件实现功能：AacLc编码器错误类型定义和对外函数声明
作者        ：
版本        ：
-------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2007/10/10  1.0         姚永强      创    建
*******************************************************************************/
#ifndef AACLCENC_CODEC_H
#define AACLCENC_CODEC_H

#ifdef __cplusplus
extern "C" {
#endif                                           /* __cplusplus */

#include "audio_codec.h"

enum aaclc_enc_errcode
{
	ERR_AACLCENC_SLOWMEM_NULL = ERR_AACLCENC,  //内存空间为空错误
	ERR_AACLCENC_SLOWMEM_SMALL,                //内存空间不足
	ERR_AACLCENC_ENCTYPE,                      //编码类型错误
	ERR_AACLCENC_BITWIDTH,                     //位宽错误
	ERR_AACLCENC_FRAMLEN,                      //帧长错误
	ERR_AACLCENC_CHANNEL,                      //声道数目判断
	ERR_AACLCENC_BITRATE,                      //比特率错误
	ERR_AACLCENC_BITSIZE,                      //码流长度错误
	ERR_AACLCENC_INOUTBUF_NULL,                //编码输入输出缓存为空错误        
	ERR_AACLCENC_HANDLE_NULL,                  //编码句柄为空错误  
	ERR_AACLCENC_VERSION,                      //版本错误
	ERR_AACLCENC_CLOSE,                        //编码释放内存错误
	ERR_AACLCENC_PPVHANDLE_NULL,               //句柄为空
	ERR_AACLCENC_INIT,                         //初始化错误
	ERR_AACLCENC_GETSTATUS,                    //得到状态信息错误
	ERR_AACLCENC_CLOSE_NULL,                   //释放的内存为空
	ERR_AACLCENC_INIT_FREE                     //AacEncInit释放的内存错误
};

/*====================================================================
函数名      : AacLcEncOpen                          
功能        : 初始化编码器参数                                  
算法实现    : 无                                                  
引用全局变量: 无                                                      
参数        : ppvHandle: 指向编码器句柄指针的指针[out]
              ptAudEncParam: 编码器初始化参数结构体指针[in]
              pvAudMemParam:  编码器内存管理结构体的指针[in]              
返回值说明  : 成功: AUDIO_SUCCESS, 失败: 返回错误码                   
====================================================================*/
l32 AacLcEncOpen(void **ppvHandle, TAudEncParam *ptAudEncParam, void *pvAudMemParam);

/*====================================================================
函数名      : AacLcEncProcess                          
功能        : 编码一帧                                 
算法实现    : 无                                                  
引用全局变量: 无                                                      
参数        : pvHandle: 指向编码器句柄指针[in]
              ptAudEncInput: 指向编码输入结构体指针[in]
              ptAudEncOutput: 指向编码输出结构体指针[out]              
返回值说明  : 成功: AUDIO_SUCCESS, 失败: 返回错误码                   
====================================================================*/
l32 AacLcEncProcess(void *pvHandle, TAudEncInput *ptAudEncInput, TAudEncOutput *ptAudEncOutput);

/*====================================================================                        
函数名      : AacLcEncClose                                                                 
功能        : 编码器关闭                                                                        
算法实现    : 无                                                                              
引用全局变量: 无                                                                              
参数        : pvHandle: 指向编码器句柄指针[in]                                   
返回值说明  : 成功: AUDIO_SUCCESS, 失败: 返回错误码                                           
====================================================================*/                        
l32 AacLcEncClose(void *pvHandle);

/*====================================================================                        
函数名      : AacLcEncGetStatus                                                                 
功能        : 得到编码器状态信息                                                                        
算法实现    : 无                                                                              
引用全局变量: 无                                                                              
参数        : pvHandle: 指向编码器句柄指针[in]
              ptAudEncStatusParam: 指向编码状态参数结构体指针[in/out]                                 
返回值说明  : 成功: AUDIO_SUCCESS, 失败: 返回错误码                                           
====================================================================*/                        
l32 AacLcEncGetStatus(void *pvHandle, TAudEncStatusParam *ptAudEncStatusParam);

/*====================================================================                        
函数名      : AacLcEncGetVersion                                                                 
功能        : 得到编码器版本信息                                                                        
算法实现    : 无                                                                              
引用全局变量: 无                                                                              
参数        : pvVer: 输出缓存[out]
              l32BufLen: 缓存长度[in]
              pl32VerLen: 信息长度[in/out]                          
返回值说明  : 成功: AUDIO_SUCCESS, 失败: 返回错误码                                           
====================================================================*/                        
l32 AacLcEncGetVersion(void *pvVer, l32 l32BufLen, l32 *pl32VerLen);

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif


