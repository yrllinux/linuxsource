/*****************************************************************************
   模块名      : KDV MCU
   文件名      : KdvMcuRec.h
   相关文件    : 
   文件实现功能: KDV Mcu Rec COM 组件封装对外接口提供
   作者        : 
   版本        : V4.0  Copyright(C) 2007-2009 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2007/08/08  4.0         顾振华      创建
******************************************************************************/

#ifndef _KDVMCUREC_INTERFACE_H_
#define _KDVMCUREC_INTERFACE_H_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAXLEN_ALIAS         128           // 会议名称、录像机别名的长度
#define MAXNUM_RECCHNNL      32            // 最大录像通道数
#define MAXNUM_STREAM_PERREC 3             // 每个录像最大的流数量
#define MCU_LISTEN_PORT      60000         // 默认的MCU监听端口


enum emStreamType
{
    emStreamTypeNone    =   0,  /*未知流类型*/
    emStreamTypeAudio   =   1,  /*音频流*/
    emStreamTypeVideo   =   2,  /*视频流*/
    emStreamTypeDStream =   3   /*第二路视频*/
};

enum emPublishMode
{
    emPublishNone           =   0,  /*不发布*/
    emPublishImmediately    =   1,  /*立即发布*/
    emPublishFinished       =   2   /*录完发布*/
};

enum emRecState
{
    emRecStateStop = 0,             /*没有在录像*/
    emRecStateRecording = 1,        /*正在录像*/
    emRecStatePaused = 2            /*录像暂停中*/
};

// 对本静态库操作整体宏定义
enum emRecGenErr
{
    emRecSucceed            =   0,
        
    //emCfgErr: 1-20
    emErrCfgID              =   1,
    emErrCfgAlias           =   2,
    emErrCfgRecIp           =   3,
    emErrCfgRecPort         =   4,
    emErrCfgRecChnNum       =   5,
    emErrCfgPlayChnNum      =   6,
    emErrCfgTotalChnNum     =   7,
    emErrCfgMcuIp           =   8,
    emErrCfgMcuPort         =   9,
    
    //emOprErr: 21-50
    emErrSetNetRcvParam     =   21,
    emErrConfID             =   22,
    emErrStopRec            =   23,
    emErrStopWhileRecready  =   24

    //emOtherException: 51+
};


//录像机需要接收的上层配置信息 len:140
struct TRecCfgInfo
{
    TRecCfgInfo():m_dwRecIpAddr(0),
               m_wRecStartPort(0),
               m_byEqpId(0),
               m_byRecChnNum(MAXNUM_RECCHNNL),
               m_dwMcuIpAddr(0)
    {
        memset( m_achAlias, 0, sizeof(m_achAlias) );
        SetMcuPort(MCU_LISTEN_PORT);
    };
    ~TRecCfgInfo(){};
    
public:
    void SetRecIp(unsigned long dwIpAddr) { m_dwRecIpAddr = htonl(dwIpAddr);    }
    unsigned long  GetRecIp(void) const { return ntohl(m_dwRecIpAddr);    }
    void SetRecAlias(LPCSTR pszAlias)
    {
        if ( NULL != pszAlias )
        {
            unsigned char byStrLen = min(strlen(pszAlias), MAXLEN_ALIAS-1);
            strncpy(m_achAlias, pszAlias, byStrLen);
            m_achAlias[byStrLen] = '\0';
        }
    }
    LPCSTR GetRecAlias(void) const { return m_achAlias;    }
    void SetRecStartPort(unsigned short wPort) { m_wRecStartPort = htons(wPort);    }
    unsigned short  GetRecStartPort(void) const { return ntohs(m_wRecStartPort);    }
    void SetEqpId(unsigned char byEqpId) { m_byEqpId = byEqpId;    }
    unsigned char   GetEqpId(void) const { return m_byEqpId;    }
    void SetRecChnNum(unsigned char byNum) { m_byRecChnNum = byNum;    }
    unsigned char   GetRecChnNum(void) const { return m_byRecChnNum;    }
    void SetMcuIp(unsigned long dwMcuIp) { m_dwMcuIpAddr = htonl(dwMcuIp);    }
    unsigned long  GetMcuIp(void) const  { return ntohl(m_dwMcuIpAddr);    }
    void SetMcuPort(unsigned short wMcuPort) { m_wMcuConnPort = htons(wMcuPort);    }
    unsigned short  GetMcuPort(void) const { return ntohs(m_wMcuConnPort);    } 

protected:
    unsigned long  m_dwRecIpAddr;                  //录像机IP地址
    char           m_achAlias[MAXLEN_ALIAS];       //别名
    unsigned short m_wRecStartPort;                //录像通道起始端口号
    unsigned char  m_byEqpId;                      //外设ID
    unsigned char  m_byRecChnNum;                  //录像通道数

    unsigned long  m_dwMcuIpAddr;                  //MCU IP地址
    unsigned short m_wMcuConnPort;                 //MCU 通信端口号
    
    unsigned long  m_dwReserved1;
    unsigned long  m_dwReserved2;    
};


//录像机与会议状态: len: 12
struct TRecConfStatus
{
public:
    void SetIndex(unsigned long dwIdx)
    {
        m_dwChnlIndex = dwIdx;
    }
    unsigned long GetIndex() const
    {
        return m_dwChnlIndex;
    }
    void SetRecState(emRecState emState) { m_emRecState = emState; }
    emRecState GetRecState(void) const{ return m_emRecState;    }
    void SetErrCode(unsigned long dwErr) { m_dwErrCode = htonl(dwErr); }
    unsigned long GetErrCode(void) const { return ntohl(m_dwErrCode) ;  }

protected:
    unsigned long   m_dwChnlIndex;                   // 录像通道的索引号，唯一标识一个会议录像（值>0）。如果为0则表示为整个录像机的状态
    emRecState      m_emRecState;                    // 会议录像的状态。emRecState
    unsigned long   m_dwErrCode;                     // 录像机当前的错误码（状态)，错误码由 emRecGenErr 定义
    unsigned long   m_dwReserved;                    // 保留
};


//会议简单属性结构：len:276
struct TConfProperty
{
    TConfProperty(): m_dwChnlIndex(0),
                     m_dwDuration(0),
                     m_bStopMeetingManual(TRUE),
                     m_dwTotalBitRate(0),
                     m_dwReserved(0)
    {
        memset(m_achConfName, 0, sizeof(m_achConfName));
        memset(m_achRecName, 0, sizeof(m_achRecName));
    }
public:
    void SetIndex(unsigned long dwIdx)
    {
        m_dwChnlIndex = dwIdx;
    }
    unsigned long GetIndex() const
    {
        return m_dwChnlIndex;
    }
    void SetDuration(unsigned long dwDur){ m_dwDuration = htonl(dwDur);    }
    unsigned long  GetDuration(void) const { return ntohl(m_dwDuration);    }
    void SetStopManual(BOOL bManual) { m_bStopMeetingManual = bManual;    }
    BOOL IsStopManual(void) const { return m_bStopMeetingManual;    }
    void SetTotalBitRate(unsigned long dwBR) { m_dwTotalBitRate = htonl(dwBR);    }
    unsigned long  GetTotalBitRate(void) const { return ntohl(m_dwTotalBitRate);    }
    void SetConfName(LPCSTR pszAlias)
    {
        if ( NULL != pszAlias )
        {
            unsigned char byStrLen = min(strlen(pszAlias), MAXLEN_ALIAS-1);
            strncpy(m_achConfName, pszAlias, byStrLen);
            m_achConfName[byStrLen] = '\0';
        }
    }
    LPCSTR GetConfName(void) const { return m_achConfName;    }

    void SetRecName(LPCSTR pszAlias)
    {
        if ( NULL != pszAlias )
        {
            unsigned char byStrLen = min(strlen(pszAlias), MAXLEN_ALIAS-1);
            strncpy(m_achRecName, pszAlias, byStrLen);
            m_achRecName[byStrLen] = '\0';
        }
    }
    LPCSTR GetRecName(void) const { return m_achRecName;    }

protected:
    unsigned long  m_dwChnlIndex;                   // 录像通道的索引号，唯一标识一个会议录像
    char	 m_achConfName[MAXLEN_ALIAS];           // 会议名
    char     m_achRecName[MAXLEN_ALIAS];            // 录像名
	unsigned long	 m_dwDuration;                  // 持续时间
	BOOL m_bStopMeetingManual;                      // 是否手动结束
	unsigned long	 m_dwTotalBitRate;              // 总码率
	unsigned long  m_dwReserved;                    // 保留
};

//流参数
struct TStreamParam
{
public:
    void SetStreamType(emStreamType emType)
    {
        m_emStreamType = emType;
    }
    emStreamType GetStreamType() const
    {
        return m_emStreamType;
    }

    void SetBitrate(unsigned long dwBitrate)
    {
        m_dwBitRate = dwBitrate;
    }

    unsigned long GetBitrate() const
    {
        return m_dwBitRate;
    }

    void SetAudFormat(unsigned short wFormat){ m_tSParam.m_tAudioParam.m_wTwoCC = htons(wFormat);    }
    unsigned long  GetAudFormat(void) const { return ntohs(m_tSParam.m_tAudioParam.m_wTwoCC);    }
    void SetAudSamplesPerSec(unsigned long dwSamples){ m_tSParam.m_tAudioParam.m_dwSamplesPerSec = htonl(dwSamples); }
    unsigned long  GetAudSamplesPerSec(void) const { return ntohl(m_tSParam.m_tAudioParam.m_dwSamplesPerSec);    }
    void SetAudBitsPerSample(unsigned long dwBits){ m_tSParam.m_tAudioParam.m_dwBitsPerSample = htonl(dwBits);    }
    unsigned long  GetAudBitsPerSample(void) const { return ntohl(m_tSParam.m_tAudioParam.m_dwBitsPerSample); }
    void SetAudChnlNum(unsigned char byNum){ m_tSParam.m_tAudioParam.m_byChnnlNum = byNum; }
    unsigned char   GetAudChnlNum(void) const { return  m_tSParam.m_tAudioParam.m_byChnnlNum; }
    void SetAudCodecSpecificDataSize(unsigned short wDataSize) { m_tSParam.m_tAudioParam.m_wCodeSpecificDataSize = wDataSize;    }
    unsigned short   GetAudCodecSpecificDataSize(void) const { return m_tSParam.m_tAudioParam.m_wCodeSpecificDataSize;    }
    void SetAudCodecSpecificData(unsigned char * pbyData)
    {
        m_tSParam.m_tAudioParam.m_pbyCodeSpecificData = pbyData;
    }
    unsigned char *GetAudCodecSpecificData(void) const { return m_tSParam.m_tAudioParam.m_pbyCodeSpecificData;    }
    
    void SetAudBlockAlignment(unsigned short wAlignment){ m_tSParam.m_tAudioParam.m_wBlockAlignment = wAlignment;   }
    unsigned short GetAudBlockAlignment(void) const { return m_tSParam.m_tAudioParam.m_wBlockAlignment;    }
    void SetAudAverageNumOfBytesPerSecond(unsigned long dwNPS) { m_tSParam.m_tAudioParam.m_dwAverageNumOfBytesPerSecond = dwNPS;    }
    unsigned long GetAudAverageNumOfBytesPerSecond(void) const { return m_tSParam.m_tAudioParam.m_dwAverageNumOfBytesPerSecond;    }
    

    void SetVidFormat(unsigned long dwFormat){ m_tSParam.m_tVideoParam.m_dwFourCC = htonl(dwFormat);    }
    unsigned long  GetVidFormat(void) const { return ntohl(m_tSParam.m_tVideoParam.m_dwFourCC);    }
    void SetVidFramesPerSec(unsigned long dwFrames) { m_tSParam.m_tVideoParam.m_dwFramesPerSec = htonl(dwFrames);    }
    unsigned long  GetVidFramesPerSec(void) const { return ntohl(m_tSParam.m_tVideoParam.m_dwFramesPerSec);    }
    void SetVidImageHeight(unsigned long dwHight) { m_tSParam.m_tVideoParam.m_dwImageHeight = htonl(dwHight);    }
    unsigned long  GetVidImageHeight(void) const { return ntohl(m_tSParam.m_tVideoParam.m_dwImageHeight);    }
    void SetVidImageWidth(unsigned long dwWidth) { m_tSParam.m_tVideoParam.m_dwImageWidth = htonl(dwWidth);    }
    unsigned long  GetVidImageWidth(void) const { return ntohl(m_tSParam.m_tVideoParam.m_dwImageWidth);    }
    void SetVidBitCounts(unsigned char byBC) { m_tSParam.m_tVideoParam.m_byBitCounts = byBC;    }
    unsigned char   GetVidBitCounts(void) const { return m_tSParam.m_tVideoParam.m_byBitCounts;    }
    
    void SetVidImageSize(unsigned long dwSize) { m_tSParam.m_tVideoParam.m_dwImageSize = dwSize;    }
    unsigned long GetVidImageSize(void) const { return m_tSParam.m_tVideoParam.m_dwImageSize;    }
    void SetVidHorizontalPixelsPerMeter(signed long nHPPM) { m_tSParam.m_tVideoParam.m_nHorizontalPixelsPerMeter = nHPPM;    }
    signed long GetVidHorizontalPixelsPerMeter(void) const { return m_tSParam.m_tVideoParam.m_nHorizontalPixelsPerMeter;    }
    void SetVidVerticalPixelsPerMeter(signed long nVPPM) { m_tSParam.m_tVideoParam.m_nVerticalPixelsPerMeter = nVPPM;   }
    signed long GetVidVerticalPixelsPerMeter(void) const { return m_tSParam.m_tVideoParam.m_nVerticalPixelsPerMeter;    }
    void SetVidColorsUsedCount(unsigned long dwCount) { m_tSParam.m_tVideoParam.m_dwColorsUsedCount = dwCount;    }
    unsigned long GetVidColorsUsedCount(void) const { return m_tSParam.m_tVideoParam.m_dwColorsUsedCount;    }
    void SetVidImportantColorsCount(unsigned long dwCount) { m_tSParam.m_tVideoParam.m_dwImportantColorsCount = dwCount;   }
    unsigned long GetVidImportantColorsCount(void) const { return m_tSParam.m_tVideoParam.m_dwImportantColorsCount;    }

protected:
    emStreamType  m_emStreamType;        // 流类型：参见 emStreamType
    unsigned long m_dwBitRate;           // 本码流比特率，单位：bps
    union UTSParam
    {
        struct TAudioStreamParam
        {        
            unsigned long  m_dwSamplesPerSec;       // 每秒采样数
            unsigned long  m_dwBitsPerSample;       // 每个样本的位数
            unsigned short m_wTwoCC;                // 音频类型
            unsigned char  m_byChnnlNum;            // 声道数
            unsigned short m_wCodeSpecificDataSize; // 音频Codec标识
            unsigned char *m_pbyCodeSpecificData;   // 音频Codec标识
            unsigned short m_wBlockAlignment;       // 块字节对齐数
            unsigned long  m_dwAverageNumOfBytesPerSecond;  // 平均码率 Bytes / Second
        }m_tAudioParam;
        
        struct TVideoStreamParam
        {
            unsigned long  m_dwFourCC;          //视频类型
            unsigned long  m_dwFramesPerSec;    //帧率：the number of frames per second
            unsigned long  m_dwImageWidth;      //图像宽：Specifies the width of the bitmap, in pixels
            unsigned long  m_dwImageHeight;     //图像高：Specifies the height of the bitmap, in pixels
            unsigned char  m_byBitCounts;       //位深：Specifies the number of bits per pixel.
            unsigned long  m_dwImageSize;					// 画面尺寸
            signed long    m_nHorizontalPixelsPerMeter;		// 水平分辨率
            signed long    m_nVerticalPixelsPerMeter;		// 垂直方向分辨率
            unsigned long  m_dwColorsUsedCount;				// 调色板
            unsigned long  m_dwImportantColorsCount;        // 重要颜色数量
            
        }m_tVideoParam;
    }m_tSParam;
};

//录像属性结构.len：
struct TRecProperty
{
    TRecProperty():m_byPublishMode(emPublishNone),
                   m_byTotalStreamNum(0),
                   m_dwReserved(0)
    {
        memset(&m_atStream, 0, sizeof(m_atStream));
    }
public:
    void SetPublishMode(unsigned char byMode) { m_byPublishMode = byMode;    }
    unsigned char   GetPublishMode(void) const { return m_byPublishMode;    }
    void SetTotalStreamNum(unsigned char byNum) { m_byTotalStreamNum = byNum;    }
    unsigned char   GetTotalStreamNum(void) const { return m_byTotalStreamNum;   }
    void SetStreamParam(unsigned char byStreamIdx, TStreamParam &tStreamParam)
    {
        if ( byStreamIdx > 2 )
        {
            OspPrintf( TRUE, FALSE, "[TRecProperty] SetStreamParam SIdx.%d, ignore\n", byStreamIdx );
            return;
        }
        m_atStream[byStreamIdx] = tStreamParam;
    }
    TStreamParam *GetStreamParam(unsigned char byStreamIdx)
    {
        if ( byStreamIdx > 2 )
        {
            OspPrintf( TRUE, FALSE, "[TRecProperty] GetStreamParam SIdx.%d, ignore\n", byStreamIdx );
            return NULL;
        }        
        return &m_atStream[byStreamIdx];
    }

protected:
    unsigned char  m_byPublishMode;        // 本此所录文件的发布模式：参见 emPublishMode
    unsigned char  m_byTotalStreamNum;     // 录像中包含的流总数
    TStreamParam   m_atStream[MAXNUM_STREAM_PERREC];    //每个流的参数，下标为流号-1。
	unsigned long  m_dwReserved;           // 保留
};

//帧头属性结构.len: 36+xx
struct TFrameHead
{
    TFrameHead():m_pbyDataBuf(NULL),
                 m_dwDataBufLen(0),
                 m_dwTimeStamp(0),
                 m_bKeyFrame(FALSE)
    {
    }
public:
    
    void SetBufHead(unsigned char *pbyBufHead) { m_pbyDataBuf = pbyBufHead;    }
    unsigned char*  GetBufHead(void) const { return m_pbyDataBuf;    }
    void SetBufLen(unsigned long dwLen) { m_dwDataBufLen = htonl(dwLen);    }
    unsigned long  GetBufLen(void) const { return ntohl(m_dwDataBufLen);    }
    void SetTStamp(unsigned long dwTStamp) { m_dwTimeStamp = htonl(dwTStamp);    }
    unsigned long  GetTStamp(void) const { return ntohl(m_dwTimeStamp);    }

    void SetIsKeyFrame(BOOL bKeyFrame) { m_bKeyFrame = bKeyFrame;    }
    BOOL GetIsKeyFrame(void) const { m_bKeyFrame;    }

protected:        
    unsigned char  *m_pbyDataBuf; //帧数据缓冲
    unsigned long m_dwDataBufLen; //帧数据缓冲区大小
    unsigned long m_dwTimeStamp;  //时间戳：Time Stamp
    BOOL          m_bKeyFrame;    //对于视频有效，是否是关键帧
    unsigned long dwReserved;
};

//通道码流结构.len: 46+xx
struct TStreamProperty
{
    TStreamProperty():m_dwConfIndex(0),
                      m_byStreamIdx(0),
                      m_dwReserved(0){}
public:
    void SetConfIndex(unsigned long dwIdx)
    {
        m_dwConfIndex = dwIdx;
    }
    unsigned long GetConfIndex() const
    {
        return m_dwConfIndex;
    }
    void SetStreamIndex(unsigned char byChannel) { m_byStreamIdx = byChannel;    }
    unsigned char   GetStreamIndex(void) const { return m_byStreamIdx;    }
    
    void SetFrameHead(TFrameHead &tFrameHead) { m_tFrameHead = tFrameHead;    }
    TFrameHead * GetFrameHead(void) { return &m_tFrameHead;    }

    void SetNull(){ memset(this, 0, sizeof(TStreamProperty)); }
    
protected:
    unsigned long m_dwConfIndex;          // 本流所在录像通道索引号
    unsigned char  m_byStreamIdx;         // 本流的流号        
    TFrameHead m_tFrameHead;              // 本流帧头属性结构
    unsigned long m_dwReserved;           // 保留
};


#endif // _KDVMCUREC_INTERFACE_H_