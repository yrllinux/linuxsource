/*****************************************************************************
  模块名      : 编解码适配
  文件名      : codecadapter.h
  相关文件    : 
  文件实现功能: 编解码适配
  作者        : 胡小鹏
  版本        : V1.0  Copyright(C) 1997-2009 KDC, All rights reserved.
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2009/03/16  V1.0        胡小鹏       创建
******************************************************************************/
#ifndef _CODECADAPTER_H_
#define _CODECADAPTER_H_

#include "codecwrapper_common.h"

//状态
typedef struct
{
    BOOL32  bIsActive;             //是否开启适配
    TNetAddress tNetRcvAddr;       //各通道的网络接收地址    
    TVidEncParam tVidEncParam;     //图像编码参数
    TNetAddress tNetSndAddr;       //网络发送地址    
}TVidAdapterStatus;

typedef struct
{
    TVidRecvStatis tRecvStatis;
    TVidSendStatis tSenderStatis;
}TVidAdapterStatis;    

class CODECWRAPPER_API CCodecAdapter
{
public:
	CCodecAdapter();
	~CCodecAdapter();	

    /*************************************************************
    函数名    : Start
    功能      ：开始适配
    参数说明  ：tNetSndAddr        [I] 发送目地地址和端口
                tNetSndLocalAddr   [I] 本地地址和端口                 
    
    返回值说明：成功返回TRUE，失败返回FALSE
    **************************************************************/
    BOOL32 Start(TNetAddress tNetSndAddr, TNetAddress tNetSndLocalAddr);

    /*************************************************************
    函数名    : Stop
    功能      ：停止适配
    参数说明  ：无            
    
    返回值说明：成功返回TRUE，失败返回FALSE
    **************************************************************/
    BOOL32 Stop();

    /*************************************************************
    函数名    : SetChannelParam
    功能      ：指定合成输入通道
    参数说明  ：tNetRcvAddr     [I] 指定接收数据的IP地址和端口  
                tRtcpNetRcvAddr [I] Rtcp的IP地址和端口 （指令）

    返回值说明：成功返回TRUE，失败返回FALSE
    **************************************************************/
    BOOL32 SetChannelParam(TNetAddress tNetRcvAddr, TNetAddress tRtcpNetRcvAddr);

    /*************************************************************
    函数名    : SetChannelActive
    功能      ：指定输入通道是否工作
    参数说明  ：byChnNo         [I] 要设定的通道号
                bActive         [I] TRUE为工作，FALSE为不工作

    返回值说明：成功返回TRUE，失败返回FALSE
    **************************************************************/
    BOOL32 SetChannelActive(BOOL32 bActive);       

    /*************************************************************
    函数名    : SetEncParam
    功能      ：设置指定通道编码参数
    参数说明  ：byChnNo              [I]通道号
                tMulPicStatus        [I] 编码参数

    返回值说明：成功返回TRUE，失败返回FALSE   
    **************************************************************/
    BOOL32 SetEncParam(TVidEncParam* ptVideoEncParam);   
   
    /*************************************************************
    函数名    : GetActiveStatus
    功能      ：取启动/停止 状态
    参数说明  ：无
    
    返回值说明：TRUE:为启动  FALSE:为停止
    **************************************************************/
    BOOL32 GetActiveStatus();
    
    /*************************************************************
    函数名    : GetStatus
    功能      ：取状态参数
    参数说明  ：tMulPicStatus        [IO] 状态参数
    
    返回值说明：成功返回TRUE，失败返回FALSE
    **************************************************************/
	BOOL32 GetStatus(TVidAdapterStatus &tVidAdapterStatus);

    /*************************************************************
    函数名    : GetStatis
    功能      ：取统计
    参数说明  ：tMulPicStatus        [IO] 状态参数
    
    返回值说明：成功返回TRUE，失败返回FALSE
    **************************************************************/
	BOOL32 GetStatis(TVidAdapterStatis &tVidAdapterStatis);    

    /*************************************************************
    函数名    : ChangeBitRate
    功能      ：设置编码发送的码率
    参数说明  ：dwBitRate         [I] 码率值，单位 (Kbit/s)
    
    返回值说明：成功返回TRUE，失败返回FALSE
    **************************************************************/
	BOOL32 ChangeEncBitRate(u32 dwBitRate);
    
    /*************************************************************
    函数名    : SetFastUpdata
    功能      ：设置编码器编关键帧
    参数说明  ：无
    
    返回值说明：成功返回TRUE，失败返回FALSE
    **************************************************************/
    BOOL32 SetFastUpdata();   

    /*************************************************************
    函数名    : GetVidSendStatis
    功能      ：获取通道发送统计信息
    参数说明  ：tKdvDecStatis     [IO] 统计信息
    
    返回值说明：成功返回TRUE，失败返回FALSE  
    **************************************************************/
    BOOL32 GetVidSendStatis(TVidSendStatis &tVidSendStatis);

    /*************************************************************
    函数名    : GetVidRecvStatis
    功能      ：获取通道接收统计信息
    参数说明  ：tKdvDecStatis     [IO] 统计信息
    
    返回值说明：成功返回TRUE，失败返回FALSE
    --------------------------------------------------------------
    修改记录  ：
    日  期      版本        修改人      走读人        修改记录
    2009/03/10  V1.0        胡小鹏                      创建
    **************************************************************/
    BOOL32 GetVidRecvStatis(TVidRecvStatis &tKdvDecStatis);
 
    /*************************************************************
    函数名    : SetNetRecvFeedbackVideoParam
    功能      ：设置所有图像的网络接收重传参数
    参数说明  ：TNetRSParam         [I] 重传参数
                bRepeatSnd          [I] 重传参数
    
    返回值说明：成功返回TRUE，失败返回FALSE
    **************************************************************/
	BOOL32  SetNetRecvFeedbackVideoParam(TNetRSParam tNetRSParam, BOOL32 bRepeatSnd = FALSE);

    /*************************************************************
    函数名    : SetNetSendFeedbackVideoParam
    功能      ：设置所有图像的网络发送重传参数
    参数说明  ：wBufTimeSpan         [I] 重传参数
                bRepeatSnd           [I] 重传参数
    
    返回值说明：成功返回TRUE，失败返回FALSE      
    **************************************************************/
	BOOL32  SetNetSendFeedbackVideoParam(u16 wBufTimeSpan, BOOL32 bRepeatSnd = FALSE);
    
    /*************************************************************
    函数名    : SetVideoActivePT
    功能      ：设置 动态视频载荷的 Playload值
    参数说明  ：byRmtActivePT  [I] 
                byRealPT       [I] 

    返回值说明：成功返回TRUE，失败返回FALSE    
    **************************************************************/
    BOOL32 SetVideoActivePT(u8 byRmtActivePT, u8 byRealPT);

    /*************************************************************
    函数名    : SetDecryptKey
    功能      ：设置 动态视频载荷的 Playload值
    参数说明  ：byRmtActivePT  [I] 
                byRealPT       [I] 

    返回值说明：成功返回TRUE，失败返回FALSE    
    **************************************************************/
    BOOL32 SetDecryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode=DES_ENCRYPT_MODE);

    /*************************************************************
    函数名    : SetDecryptKey
    功能      ：设置加密key字串及加密码流的动态载荷PT值, pszKeyBuf设置为NULL-表示不加密
    参数说明  ：byRmtActivePT  [I] 
                byRealPT       [I] 

    返回值说明：成功返回TRUE，失败返回FALSE    
    **************************************************************/
    BOOL32 SetEncryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode=DES_ENCRYPT_MODE);
    
    //////////////////////////////////////////////////////////////////////////
    //以下是非用户接口

    void* m_pListImg;
    void* m_pListEnc;                   //接收编码列表
    void* m_pListDec;                   //解码发送列表       
    void* m_ptLock;                     //线程锁
    void* m_ptProcessLock;              //线程锁
    void* m_ptCond;           
    void* m_pListVidEncParam;

private:
    BOOL32 SetDecParam();    
    void ConvertEncParam(TVidEncParam* ptVideoEncParam, void* ptVoid);
    BOOL32 Quit();

    BOOL32 m_bActive;    
};

#endif //end of _CODECADAPTER_H_


