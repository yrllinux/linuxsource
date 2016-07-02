#ifndef _MP4_PUBLIC_H_
#define _MP4_PUBLIC_H_

#include "algorithmtype.h"

/* version information: 1.00(MP4_MAJOR.MP4_MINOR) */
#define MP4_MAJOR  ((u32) 1)
#define MP4_MINOR  ((u32) 00)
#define MP4_VERSION ((u32)MP4_MAJOR << 16 |(u32)MP4_MINOR)

/* main character flags */
#define USE_INTER8x8        (1 << 0)    /* use inter 8x8 block */
#define USE_HALFPEL8x8      (1 << 1)    /* use inter 8x8 halfpel search
                                           (if USE_INTER8x8 do not exist, this flag is not use) */
#define USE_HALFPEL16x16    (1 << 2)    /* use inter 16x16 halfpel search */
#define USE_SEARCH16x16R16  (1 << 3)    /* inter 16x16 search range is 16(default is 8) */
#define USE_SEARCH8x8R16    (1 << 4)    /* inter 8x8 search range is 16(default is 8) */
#define USE_FINEHALFPEL     (1 << 5)    /* search both 16x16 half pel and 8x8 half pel 
                                           (default is search the winner's half pel) */
#define USE_QUARTER         (1 << 6)    /* search both 16x16 half pel and 8x8 quarter pel 
                                           (default is search the winner's quarter pel) */

#define PROFILE_AUTO        0           /* let the encoder to decide encode tools */
#define PROFILE_FASTEST     (1 << 16)   /* just search inter 16x16(default for picture merge board?) */
#define PROFILE_FAST        (1 << 17)   /* search inter 16x16 and perform half pel search(default for D1 encode) */
#define PROFILE_MEDIAM      (1 << 18)   /* search inter 16x16 and inter 8x8 and perform the winner's halfpel search 
                                           (default for 2cif encode) */
#define PROFILE_SLOW        (1 << 19)   /* all encoder tools(default for cif/qcif encoder) */

/* error code(someday should use macro to redefine them) */
/* general error */
#define ERROR_POINTER         0x80010001  /* the pointer is invalid */
#define ERROR_OUTOFMEM        0x80010002  /* run out of memory */
/* initialize error */
#define ERROR_INIT_DIMENSION  0x80020001  /* the width/height is invalid */
#define ERROR_INIT_GOPLEN     0x80020002  /* the gop length is invalid */
#define ERROR_INIT_BITRATE    0x80020003  /* the bitrate is invalid */
#define ERROR_INIT_FRAMERATE  0x80020004  /* the framerate is invalid */
#define ERROR_INIT_QPRANGE    0x80020005  /* the min/max qp is invalid */
/* encode error */
#define ERROR_ENC_LEN         0x80030001  /* encoder encode too long bitstreams(more than 128k) */

#define GET_ENC_POINT_REF       1
#define GET_ENC_POINT_REC       2
#define GET_ENC_WIDTH           3
#define GET_ENC_HEIGHT          4
#define GET_ENC_EDGE_WIDTH      5

#ifdef __cplusplus
extern "C"
{
#endif  
    
    typedef struct
    {
        l32 l32Width;
        l32 l32Height;
        u32 u32CpuFreq;
        l32 l32BNum;
        l32 l32GOPLen;
        l32 l32Bitrate;
        l32 l32Framerate;
        l32 l32InitQP;
        l32 l32MinQP;
        l32 l32MaxQP;
        l32 l32EnableStat;
        l32 l32TopFieldFirst;
        l32 l32DCScaler;
        l32 l32Flags;
        s8* ps8RecName;
    } TMP4Param;
    
    typedef struct {
        u8	*pu8YUV420;         //编码YUV数据缓冲区，用户在调用解码器前自行分配
        u32	u32YUVLen;          //原始图像缓冲区长度
        u8	*pu8BitStream;	    //编码帧的输出缓冲区
        u32	u32BitstreamLen;    //编码输出码流长度
        u32	u32ReturnWidth;     //编码返回图像帧宽度
        u32	u32ReturnHeight;    //编码返回图像帧长度
        l32	l32FrameType;       //编码帧类型
    } TMP4EncFramePara;
    
        
    l32 MP4EncInit(void **ppvHandle, TMP4Param* ptParam);

    l32 MP4EncFrame(void *pvHandle, TMP4EncFramePara *ptEncFramePara);

    void MP4EncClose(void *pvHandle);
    
    l32 MP4EncoderSetEncParams(void* pvHandle, TMP4Param* ptParam);

    void GetMP4EncoderVersion(void *pvVer, l32 l32BufLen, l32 *pl32VerLen);

    l32 MP4EncGetInfo(void *pvHandle, l32 l32CmdId, u32 *pu32PtrAddr);
    
#ifdef __cplusplus
};
#endif

#endif
