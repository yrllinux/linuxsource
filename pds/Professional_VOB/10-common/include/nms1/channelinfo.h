/*=============================================================================
模   块   名: StreamPlayer流媒体播放器
文   件   名: ChannelInfo.h
相 关  文 件: 无
文件实现功能: 为MCU和MT的流媒体功能提供媒体数据写入文件函数
作        者: 王昊
版        本: V4.0  Copyright(C) 2003-2006 KDC, All rights reserved.
-------------------------------------------------------------------------------
修改记录:
日      期  版本    修改人      修改内容
2006/04/19  4.0     王昊        重新整理
=============================================================================*/

#ifndef  __CHANNELINFO_H
#define	 __CHANNELINFO_H

#include <time.h>
#include <stdio.h>
#include "osp.h"


#define MAX_H235KEY_LEN         (s32)255    //  音视频加密密钥最大长度
#define MAX_CHANNELNAME_LEN     (s32)255    //  频道名最大长度
#define MAX_CHANNELPWD_LEN      (s32)32     //  频道密码最大长度


//  加密/解密结构
typedef struct tagChannelEncryptKey
{
    enum EmEncryptMode      //  加密模式
    {
        emEncryptNone   = (u8)0,    //  不加密
        emEncryptDES    = (u8)1,    //  DES加密
        emEncryptAES    = (u8)2     //  AES加密
    };

    u8  m_byMode;                   // 加密模式(emEncryptNone/emEncryptDES/emEncryptAES)
    u8  m_byKeyLen;                 // 密钥长度(以字节为单位)
    u8  m_abyKey[MAX_H235KEY_LEN];  // 密钥内容

    /*=============================================================================
    函 数 名:tagChannelEncryptKey
    功    能:构造函数
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/04/18  4.0     王昊    创建
    =============================================================================*/
    tagChannelEncryptKey()
    { memset( this, 0, sizeof (tagChannelEncryptKey) ); }

    /*=============================================================================
    函 数 名:Print
    功    能:内容打印
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/04/18  4.0     王昊    创建
    =============================================================================*/
    void Print(void) const
    {
        switch ( m_byMode ) //  加密模式
        {
        case emEncryptNone:
            ::OspPrintf( TRUE, FALSE, "EncryptMode = None, " );
            break;
        case emEncryptDES:
            ::OspPrintf( TRUE, FALSE, "EncryptMode = DES, " );
            break;
        case emEncryptAES:
            ::OspPrintf( TRUE, FALSE, "EncryptMode = AES, " );
            break;
        default:
            ::OspPrintf( TRUE, FALSE, "EncryptMode = unknown(%d), ", m_byMode );
            break;
        }

        ::OspPrintf( TRUE, FALSE, "KeyLen = %d\nKey: ", m_byKeyLen );//密钥长度
        for ( s32 nIndex = 0; nIndex < m_byKeyLen; nIndex ++ )      //密钥内容
        {
            ::OspPrintf( TRUE, FALSE, "%.2x ", m_abyKey[nIndex] );
        }
        ::OspPrintf( TRUE, FALSE, "\n" );
    }

} TChannelEncryptKey;


//AAC LC参数
#define CHANNEL_AAC_SAMPLE_FRQ_8                (u8)1
#define CHANNEL_AAC_SAMPLE_FRQ_11               (u8)2
#define CHANNEL_AAC_SAMPLE_FRQ_12               (u8)3
#define CHANNEL_AAC_SAMPLE_FRQ_16               (u8)4
#define CHANNEL_AAC_SAMPLE_FRQ_22               (u8)5
#define CHANNEL_AAC_SAMPLE_FRQ_24               (u8)6
#define CHANNEL_AAC_SAMPLE_FRQ_32               (u8)7
#define CHANNEL_AAC_SAMPLE_FRQ_44               (u8)8
#define CHANNEL_AAC_SAMPLE_FRQ_48               (u8)9
#define CHANNEL_AAC_SAMPLE_FRQ_64               (u8)10
#define CHANNEL_AAC_SAMPLE_FRQ_88               (u8)11
#define CHANNEL_AAC_SAMPLE_FRQ_96               (u8)12

#define CHANNEL_AAC_CHNL_TYPE_CUST              (u8)11
#define CHANNEL_AAC_CHNL_TYPE_SINGLE            (u8)1
#define CHANNEL_AAC_CHNL_TYPE_DOUBLE            (u8)2
#define CHANNEL_AAC_CHNL_TYPE_3                 (u8)3
#define CHANNEL_AAC_CHNL_TYPE_4                 (u8)4
#define CHANNEL_AAC_CHNL_TYPE_5                 (u8)5
#define CHANNEL_AAC_CHNL_TYPE_5P1               (u8)51
#define CHANNEL_AAC_CHNL_TYPE_7P1               (u8)71

//  加密/解密信息
typedef struct tagChannelEncrypt
{
    u8                  m_byRealAudioPT;    // 音频真实载荷值
    u8                  m_byRealVideoPT;    // 视频真实载荷值
    u8                  m_byAudioEncType;   // 音频动态载荷值(编码格式)
    u8                  m_byVideoEncType;   // 视频动态载荷值(编码格式)
    TChannelEncryptKey  m_tEncryptKey[2];   // 密钥 [0]为视频, [1]为音频
    u8                  m_byAACSamplePerSecond; // AAC采样率
    u8                  m_byAACChannels;        // AAC通道数

    /*=============================================================================
    函 数 名:tagChannelEncrypt
    功    能:构造函数
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/04/18  4.0     王昊    创建
    =============================================================================*/
    tagChannelEncrypt()
    { memset( this, 0, sizeof (tagChannelEncrypt) ); }

    /*=============================================================================
    函 数 名:GetPT
    功    能:获取所有载荷
    参    数:无
    注    意:将四个u8载荷拼接成一个u32载荷
    返 回 值:拼接字段
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/04/18  4.0     王昊    修改
    =============================================================================*/
    u32 GetPT(void) const
    {
        u32 dwPt = ( (u32)m_byRealAudioPT << 24 | (u32)m_byRealVideoPT << 16
                        | (u32)m_byAudioEncType << 8 | (u32)m_byVideoEncType );
        return dwPt;
    }

    /*=============================================================================
    函 数 名:GetKey
    功    能:获取密钥
    参    数:s8* pchBuf                 [out]   密钥内容
             u8 byChannel               [in]    密钥类型 0: 视频; 1: 音频
    注    意:供文件写入和读取使用
    返 回 值:密钥
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/04/18  4.0     王昊    修改
    =============================================================================*/
    s8* GetKey(s8* pchBuf, u8 byChannel) const
    {
        if ( byChannel > 1 || pchBuf == NULL )    //  参数检验
        {
            ::OspPrintf( TRUE, FALSE,
                         "[TChannelEncrypt::GetKey] invalid param.\n" );
            return NULL;
        }

        try
        {
            const TChannelEncryptKey *ptKey = &m_tEncryptKey[byChannel];
            u32 dwKey = 0;
            for ( s32 nIndex = 0; nIndex < ptKey->m_byKeyLen; nIndex += 4 )
            {
                u16 wLow = ((u16)ptKey->m_abyKey[nIndex + 3])
                                | ((u16)ptKey->m_abyKey[nIndex + 2] << 8 );
                u16 wHigh = ((u16)ptKey->m_abyKey[nIndex + 1])
                                | ((u16)ptKey->m_abyKey[nIndex] << 8 );
                dwKey = ((u32)wLow) | ((u32)wHigh << 16 );
                sprintf( pchBuf + nIndex * 2, "%.8x", dwKey );
            }

            return pchBuf;
        }
        catch ( ... )
        {
            ::OspPrintf( TRUE, FALSE,
                         "[TChannelEncrypt::GetKey] not enough memory.\n" );
            return NULL;
        }
    }

    /*=============================================================================
    函 数 名:Print
    功    能:内容打印
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/04/18  4.0     王昊    创建
    =============================================================================*/
    void Print(void) const
    {
        ::OspPrintf( TRUE, FALSE, "Video:\nActivePT = %d, RealPT = %d\n",
                     m_byVideoEncType, m_byRealVideoPT );
        m_tEncryptKey[0].Print();
        ::OspPrintf( TRUE, FALSE, "Audio:\nActivePT = %d, RealPT = %d\n",
                     m_byAudioEncType, m_byRealAudioPT );
        m_tEncryptKey[1].Print();
        // AAC参数
        ::OspPrintf( TRUE, FALSE, "Audio AAC:\nSamplePerSecond = %d, AACChannels = %d\n",
                     m_byAACSamplePerSecond, m_byAACChannels );
    }

} TChannelEncrypt;

//  流媒体频道信息
typedef struct tagStreamChannel
{
    s8              m_aszChannelName[MAX_CHANNELNAME_LEN + 1];  //频道名称
    s8              m_aszPassWord[MAX_CHANNELPWD_LEN + 1];      //密码
    u32             m_dwStreamNum;  //只组播主视频时, 为1; 同时组播双流为2
    u32             m_dwIp;         //组播IP(主机序)
    u16             m_wPort[2];     //端口信息(主机序), [0]为主视频, [1]为双流
    TChannelEncrypt m_tEncrypt[2];  //[0]为主视频, [1]为双流

    /*=============================================================================
    函 数 名:tagStreamChannel
    功    能:构造函数
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/04/18  4.0     王昊    创建
    =============================================================================*/
    tagStreamChannel()
    { memset( this, 0, sizeof (tagStreamChannel) ); }

    /*=============================================================================
    函 数 名:Print
    功    能:内容打印
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/04/18  4.0     王昊    创建
    =============================================================================*/
    void Print(void) const
    {
        ::OspPrintf( TRUE, FALSE,
                     "ChannelName = %s, Pwd = %s, IP = %d, StreamNum = %d\n",
                     m_aszChannelName, m_aszPassWord, m_dwIp, m_dwStreamNum );
        ::OspPrintf( TRUE, FALSE, "Main: port = %d\n", m_wPort[0] );
        m_tEncrypt[0].Print();
        ::OspPrintf( TRUE, FALSE, "Dual: port = %d\n", m_wPort[1] );
        m_tEncrypt[1].Print();
    }

} TStreamChannel;

/*=============================================================================
函 数 名:WriteChannel
功    能:向网页文件写入频道信息
参    数:TStreamChannel *ptChannel      [in]    通道指针
         s32 nChannelNum                [in]    通道数
         FILE *pf                       [in]    网页文件(已打开, 且指向下一个写入位置)
注    意:WriteHtml专用函数
返 回 值:成功: TRUE; 失败: FALSE
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2006/04/18  4.0     王昊    修改
=============================================================================*/
inline BOOL32 WriteChannel(TStreamChannel *ptChannel, s32 nChannelNum, FILE *pf)
{
    s8      aszContent[1024]    = {0};  //  文件内容
    s32     nLen                = 0;    //  写入长度

    try
    {
        if ( pf == NULL )   //  文件句柄不正确
        {
            ::OspPrintf( TRUE, FALSE, "[WriteChannel] invalid file.\n" );
            return FALSE;
        }

        if ( nChannelNum == 0 || ptChannel == NULL )    //  不需要写入频道
        {
            return TRUE;
        }

        for ( s32 nIndex = 0; nIndex < nChannelNum; nIndex ++ )
        {
            memset( aszContent, 0, sizeof (aszContent) );
            s8 aszHref[1024] = {0};
            s8 *pszBuf = NULL;
            TStreamChannel *ptCurChannel = &ptChannel[nIndex];  //当前处理通道

            nLen = sprintf( aszHref, "num=%u&",
                            ptChannel[nIndex].m_dwStreamNum );
            if ( nLen == -1 )
            {
                return FALSE;
            }

            pszBuf = aszHref + nLen;
            nLen = sprintf( pszBuf, "ip=%u&pass=%s",
                            ptChannel[nIndex].m_dwIp,
                            ptChannel[nIndex].m_aszPassWord );
            if ( nLen == -1 )
            {
                return FALSE;
            }

            s8 achKeyBuf[MAX_H235KEY_LEN] = {0};
            s8 achKeyBuf2[MAX_H235KEY_LEN] = {0};
            TChannelEncrypt *ptEncrypt = NULL;

            for ( u32 dwNum = 0; dwNum < ptChannel[nIndex].m_dwStreamNum;
                  dwNum ++ )
            {
                ptEncrypt = &ptCurChannel->m_tEncrypt[dwNum];
                memset( achKeyBuf, 0, sizeof (achKeyBuf) );
                memset( achKeyBuf2, 0, sizeof (achKeyBuf2) );
                pszBuf = pszBuf + nLen;
                nLen = sprintf( pszBuf,
"&port=%u&apt=%u&aacfreq=%u&aacchn=%u&vkeylen=%u&vkeymode=%u&vkey=%s&akeylen=%u&akeymode=%u&akey=%s",
                                ptCurChannel->m_wPort[dwNum],
                                ptEncrypt->GetPT(),
                                ptEncrypt->m_byAACSamplePerSecond,
                                ptEncrypt->m_byAACChannels,
                                ptEncrypt->m_tEncryptKey[0].m_byKeyLen,
                                ptEncrypt->m_tEncryptKey[0].m_byMode,
                                ptEncrypt->GetKey( achKeyBuf, 0 ),
                                ptEncrypt->m_tEncryptKey[1].m_byKeyLen,
                                ptEncrypt->m_tEncryptKey[1].m_byMode,
                                ptEncrypt->GetKey( achKeyBuf2, 1 ) );

                if ( nLen == -1 )
                {
                    return FALSE;
                }
            }

            s32 nSize = strlen( aszHref );
            //简易加密
            for ( s32 nPos = 0; nPos < nSize; nPos ++ )
            {
                //过滤不能加密的符号
                switch ( aszHref[nPos] )
                {
                case '=':
                case '&':
                case '<':
                case '>':
                    break;
                default:
                    aszHref[nPos] = aszHref[nPos] + 1;
                    break;
                }
            }

            nLen = sprintf( aszContent,
                    "<p><a href='player.htm?%s' target='target'> %s </a></p>\n",
                            aszHref, ptCurChannel->m_aszChannelName );
            if ( nLen == -1 )
            {
                return FALSE;
            }
            fwrite( aszContent, sizeof (s8), nLen, pf );
        }

        return TRUE;
    }
    catch ( ... )
    {
        ::OspPrintf( TRUE, FALSE, "[WriteChannel] unknown error.\n" );
        return FALSE;
    }
}

/*=============================================================================
函 数 名:WriteHtml
功    能:写网页文件
参    数:s8 *pszFileName                [in]    网页文件名
         TStreamChannel *ptChannel      [in]    通道指针
         s32 nChannelNum                [in]    通道数
注    意:无
返 回 值:成功: TRUE; 失败: FALSE
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2006/04/18  4.0     王昊    修改
=============================================================================*/
inline BOOL32 WriteHtml(s8 *pszFileName, TStreamChannel *ptChannel, s32 nChannelNum)
{
    FILE    *pf                 = NULL; //  文件指针
    s8      aszContent[1024]    = {0};  //  文件内容
    time_t  tTime               = 0;    //  当前时间
    s32     nLen                = 0;    //  写入长度
    BOOL32  bRet                = FALSE;//  返回参数

    try
    {
        pf = fopen( pszFileName, "w" );
        if ( pf == NULL )       //  文件打开失败
        {
            return bRet;
        }

        //写标题
        nLen = sprintf( aszContent, "%s%d%s%s%s%s%s",
                        "<html>\n<head>\n<title>流媒体服务频道刷新区",
                        time( &tTime ), "</title>\n",
        "<meta HTTP-EQUIV=Content-Type CONTENT=text/html; charset=utf-8>\n",
        "<meta NAME=Description CONTENT=2004年于科达上研所负责开发>\n\n",
                        "</head>\n<body bgcolor=#243D53>\n\n",
                        "<span id=linker1  type=hidden>\n" );
        if ( nLen == -1 )
        {
            fclose( pf );
            return bRet;
        }
        fwrite( aszContent, sizeof (s8), nLen, pf );

        bRet = WriteChannel( ptChannel, nChannelNum, pf );
        if ( ! bRet )
        {
            fclose( pf );
            return FALSE;
        }

        //写入页面格式信息2
        memset( aszContent, 0, sizeof (aszContent) );
        nLen = sprintf( aszContent, "%s", "</span>\n</body>\n</html>\r\n" );
        if ( nLen == -1 )
        {
            fclose( pf );
            return FALSE;
        }
        fwrite( aszContent, sizeof (s8), nLen, pf );
        fflush( pf );
        fclose( pf );

        return TRUE;
    }
    catch ( ... )
    {
        ::OspPrintf( TRUE, FALSE, "[WriteHtml] unknown error.\n" );
        return FALSE;
    }
}

#endif  //  __CHANNELINFO_H
