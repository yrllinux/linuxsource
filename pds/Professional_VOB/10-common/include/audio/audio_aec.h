/*****************************************************************************
模块名      : audio_aec
文件名      : audio_aec.h
相关文件    : 
文件实现功能: 单声道和双声道AEC对外结构体声明
              单声道AEC对外函数的声明
版本        : V1.0  Copyright(C) 2005-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日    期      版本        修改人      修改内容
2007/09/21    v100        姜  莎      修改接口
2007/10/19    v100        姜  莎      加入内存管理修改接口
******************************************************************************/
#ifndef AUDIO_AEC_H
#define AUDIO_AEC_H
#ifdef __cplusplus
extern "C" {
#endif   /* __cplusplus */

#define TAecStatusParam  TAecParam

//单声道和双声道AEC对外结构体声明
//初始化参数结构体
typedef struct 
{	
	s16 s16CapSigTimeLen;           //近端播放信号的时长
	s16 s16PlaySigTimeLen;          //远端采集信号的时长
	u32 u32AecType;                 //AEC 类型(TAecType)
	u32 u32Reserved;                //保留参数 
}TAecParam;

//Aec播放端输入数据结构体
typedef struct
{
	s16 *ps16PlayBuf;              //播放端输入缓冲区的指针
	u32 u32PlayBufSize;            //播放端输入缓冲区的字节数
	l32 l32CapBufSamples;          //采集缓冲区的样点数
	l32 l32PlayBufSamples;         //播放缓冲区的样点数
	u32 u32Reserved;               //保留参数
}TAecPlayInput; 

//Aec采集端输入数据结构体
typedef struct
{
	s16 *ps16CapBuf;               //采集端输入缓冲区的指针
	u32 u32CapBufSize;             //采集端输入缓冲区的字节数
	s16 *ps16OutBuf;               //输出端缓冲区的指针
	u32 u32OutBufSize;             //输出端缓冲区的字节数
	l32 l32UseFlag;                //bit2 = AECUseFlag bit1 = SPEUseFlag, bit0 = AGCUseFlag	
	u32 u32Reserved;               //保留参数
}TAecCapInput; 

//单声道AEC的对外接口
/*====================================================================
函   数   名:  AecOpen
功        能： 单声道AEC的初始化（包括8k，16k，32k）
引用全局变量:  无
输入参数说明:  ppvHandle            指向AEC句柄指针的指针 [out]
			   ptAecParam           指向初始化参数结构体的指针[in]
			   ptAecMemParam        指向内存分配结构体的指针[in]
返回值说明  :  成功: AUDIO_SUCCESS, 失败: 返回错误码  
特 殊 说  明:  
======================================================================*/
l32 AecOpen(void **ppvHandle, TAecParam *ptAecParam, void *ptAecMemParam);

/*====================================================================
函   数   名:  AecReset
功        能： 单声道AEC修改编解码类型时重新设置的参数
引用全局变量:  无
输入参数说明:  pvHandle             指向AEC句柄的指针[in]
			   ptAecParam           指向初始化参数结构体的指针[in]
返回值说明  :  成功: AUDIO_SUCCESS, 失败: 返回错误码 
特 殊 说  明:  无
======================================================================*/
l32 AecReset(void *pvHandle, TAecParam *ptAecParam);

/*====================================================================
函   数   名:  AecPlaySigSet  
功        能： 单声道AEC把远端信号放入远端信号的缓存中
引用全局变量:  无
输入参数说明:  pvHandle               指向AEC句柄的指针[in]
			   ptAecPlayInput         指向播放端输入数据结构体指针[in]
返回值说明  :  成功: AUDIO_SUCCESS, 失败: 返回错误码 
特 殊 说  明:  无
======================================================================*/
l32 AecPlaySigSet(void *pvHandle, TAecPlayInput *ptAecPlayInput);

/*====================================================================
函   数   名:  AecProcess
功        能： 单声道AEC完成回声抵消
引用全局变量:  无
输入参数说明:  pvHandle               指向AEC句柄的指针[in]
			   ptAecCapInput          指向采集端输入数据结构体指针[in]
返回值说明  :  成功: AUDIO_SUCCESS, 失败: 返回错误码 
特 殊 说  明:  无
======================================================================*/
l32 AecProcess(void *pvHandle, TAecCapInput *ptAecCapInput);

/*====================================================================                        
函数名      : AecGetStatus                                                                 
功能        : 单声道AEC得到状态信息                                                                        
算法实现    : 无                                                                              
引用全局变量: 无                                                                              
参数        : pvHandle:              指向AEC句柄的指针[in]
              ptAecStatusParam:      指向AEC状态参数结构体指针[in/out]                                 
返回值说明  : 成功: AUDIO_SUCCESS, 失败: 返回错误码                                           
====================================================================*/                        
l32 AecGetStatus(void *pvHandle, TAecStatusParam *ptAecStatusParam);

/*====================================================================
函   数   名:  AecGetVersion
功        能： 单声道AEC获取回声抵消的版本
引用全局变量:  无
输入参数说明:  *pvVer                指向输出版本号缓存的指针[in]
			   l32BufLen             输入的版本号字符串的长度[in]
			   pl32VerLen            实际的版本号字符串的长度[in/out]
			   u32AecType            AEC版本类型[in]
返回值说明  :  成功: AUDIO_SUCCESS, 失败: 返回错误码 
特 殊 说  明:  无
======================================================================*/
l32 AecGetVersion (void *pvVer, l32 l32BufLen, l32 *pl32VerLen, u32 u32AecType);

/*====================================================================
函   数   名:  AecClose
功        能： 单声道AEC关闭
引用全局变量:  无
输入参数说明:  pvHandle             指向AEC句柄的指针[in] 
返回值说明  :  成功: AUDIO_SUCCESS, 失败: 返回错误码 
特 殊 说  明:  
======================================================================*/
l32 AecClose(void *pvHandle);

//立体声SAEC的对外接口
/*====================================================================
函   数   名:  SaecOpen
功        能： 双声道AEC的初始化
引用全局变量:  无
输入参数说明:  ppvHandle            指向AEC句柄指针的指针 [out]
			   ptAecParam           指向初始化参数结构体的指针[in]
			   ptAecMemParam        指向内存分配结构体的指针[in]
返回值说明  :  成功: AUDIO_SUCCESS, 失败: 返回错误码  
特 殊 说  明:  
======================================================================*/
l32 SaecOpen(void **ppvHandle, TAecParam *ptAecParam, void *ptAecMemParam);

/*====================================================================
函   数   名:  SaecReset
功        能： 双声道AEC修改编解码类型时重新设置的参数
引用全局变量:  无
输入参数说明:  pvHandle             指向AEC句柄的指针[in]
			   ptAecParam           指向初始化参数结构体的指针[in]
返回值说明  :  成功: AUDIO_SUCCESS, 失败: 返回错误码 
特 殊 说  明:  无
======================================================================*/
l32 SaecReset(void *pvHandle, TAecParam *ptAecParam);

/*====================================================================
函   数   名:  SaecPlaySigSet  
功        能： 双声道AEC把远端信号放入远端信号的缓存中
引用全局变量:  无
输入参数说明:  pvHandle               指向AEC句柄的指针[in]
			   ptAecPlayInput         指向播放端输入数据结构体指针[in]
返回值说明  :  成功: AUDIO_SUCCESS, 失败: 返回错误码 
特 殊 说  明:  无
======================================================================*/
l32 SaecPlaySigSet(void *pvHandle, TAecPlayInput *ptAecPlayInput);

/*====================================================================
函   数   名:  SaecProcess
功        能： 双声道AEC完成回声抵消
引用全局变量:  无
输入参数说明:  pvHandle               指向AEC句柄的指针[in]
			   ptAecCapInput          指向采集端输入数据结构体指针[in]
返回值说明  :  成功: AUDIO_SUCCESS, 失败: 返回错误码 
特 殊 说  明:  无
======================================================================*/
l32 SaecProcess(void *pvHandle, TAecCapInput *ptAecCapInput);

/*====================================================================                        
函数名      : SaecGetStatus                                                                 
功能        : 双声道AEC得到状态信息                                                                        
算法实现    : 无                                                                              
引用全局变量: 无                                                                              
参数        : pvHandle:              指向AEC句柄的指针[in]
              ptAecStatusParam:      指向AEC状态参数结构体指针[in/out]                                 
返回值说明  : 成功: AUDIO_SUCCESS, 失败: 返回错误码                                           
====================================================================*/                        
l32 SaecGetStatus(void *pvHandle, TAecStatusParam *ptAecStatusParam);

/*====================================================================
函   数   名:  SaecGetVersion
功        能： 双声道AEC获取回声抵消的版本
引用全局变量:  无
输入参数说明:  *pvVer                指向输出版本号缓存的指针[in]
			   l32BufLen             输入的版本号字符串的长度[in]
			   pl32VerLen            实际的版本号字符串的长度[in/out]
			   u32AecType            AEC版本类型[in]
返回值说明  :  成功: AUDIO_SUCCESS, 失败: 返回错误码 
特 殊 说  明:  无
======================================================================*/
l32 SaecGetVersion (void *pvVer, l32 l32BufLen, l32 *pl32VerLen, u32 u32AecType);

/*====================================================================
函   数   名:  SaecClose
功        能： 双声道AEC关闭
引用全局变量:  无
输入参数说明:  pvHandle             指向AEC句柄的指针[in] 
返回值说明  :  成功: AUDIO_SUCCESS, 失败: 返回错误码 
特 殊 说  明:  
======================================================================*/
l32 SaecClose(void *pvHandle);

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif 

