/******************************************************************************
文件名	    ：H264Enc.h
相关文件	：H264Enc.c
文件实现功能：由于上层应用层只能为编码一帧提供MAX_FRAME_BUFFER_LEN(128k)的缓存，
              所以如果编码器发现该帧编码的码流超过该长度，编码失败，该危险系数较
              大，对于一个352x288的图像，编码为Ｉ帧可以达到81k !!! 低位低字节
关于共享缓存：由于使用共享内存，其大小不能直接给出，在此仅提供CIF格式下编码器分配
              的内存大小参考值(2500,000Byte)，外部调用者参考如下公式分配共享内存:
			  分配共享内存大小 > 编码图像大小 * 2500000 / CIF !!!
作者		：古继兴
版本		：2004.8.28
-------------------------------------------------------------------------------
日  期		版本		修改人		走读人    修改内容
01/18/2005	1.0         胡  涛      古继兴    添加注释，修改函数及变量名称
******************************************************************************/
#ifndef H264ENC_H
#define H264ENC_H

#include "algorithmtype.h"

#ifdef _LINUX_
#define H264vEncHandle void *
typedef enum
{
	H264ENCDRV_OK,
	H264ENCDRV_ERROR,
	H264ENCDRV_NOMEM,
	H264ENCDRV_BADFMT,
	H264ENCDRV_WRONG_VERSION,	
	H264ENCDRV_KERNEL_MODULE_PROBLEMS,	
}H264encDrvError_t;

#define ENC_FAIL		    -1
#define ENC_OK			    H264ENCDRV_OK
#define	ENC_MEMORY		    H264ENCDRV_NOMEM
#define ENC_BAD_FORMAT		    H264ENCDRV_BADFMT
#else
#define H264encDrvError_t    l32 
#define ENC_FAIL		    -1
#define ENC_OK			    0
#define	ENC_MEMORY		    1
#define ENC_BAD_FORMAT		2
#endif

#define MAX_FRAME_BUFFER_LEN (128*1024)         // 编码后一帧码流最大长度
#ifndef MAX_NALU_NUM    
#define MAX_NALU_NUM  128                       // 最多NALU数目
#define	MAX_NALU_UNIT_SIZE	 (4*MAX_NALU_NUM)   // 最大一个NALU单元长度 (sizeof(int) * MAX_NALU_NUM)
#endif

#define H264SimpleLevel1	1
#define H264SimpleLevel2	2
#define H264SimpleLevel3	3
#define H264AdvanceLevel1	101
//add for f264
#define F264Level1          110

#ifdef __cplusplus
extern "C"
{
#endif 
// 接口结构体定义
	
	typedef struct {
		u8	*pu8YUV420;         //编码YUV数据缓冲区，用户在调用解码器前自行分配
		u32	u32YUVLen;          //原始图像缓冲区长度
		u8	*pu8BitStream;	    //编码帧的输出缓冲区
		u32	u32BitstreamLen;    //编码输出码流长度
		u32	u32ReturnWidth;     //编码返回图像帧宽度
		u32	u32ReturnHeight;    //编码返回图像帧长度
		l32	l32FrameType;       //编码帧类型
		u32 u32EncodeCycles;	    //编码1帧允许使用指令周期数(不使用为0)
		u8  *pu8Output88MV;

        u64 u64FrameTimeStamp;  //帧时间戳
        void* pvNetParam;       //网络状态参数
	} TH264EncFramePara;
    /*-------------------------------------------------------------------------
    frametype 意义说明：
    对Encoder而言：
    作为输入参数：frametype为1表示将当前帧强制编成I帧，frametype为0表示对当前帧
                  编码类型不做强制
    作为输出参数：frametype为1表示当前帧是I帧，frametype为0表示当前帧是P帧
    --------------------------------------------------------------------------*/			
	
	typedef struct {
		l32 l32FrameRate;       // 帧率
		l32 l32BitsRate;    	// 码率
		l32 l32QPMin;           // 最小量化步长(1-51)
		l32 l32QPMax;           // 最大量化步长(1-51)
		l32 l32XDim;		   	// 输入图像宽度[0,720]必须为16的倍数
		l32 l32YDim;		    // 输入图像高度[0,576]必须为16的倍数
		l32 l32PRate;           // I帧间隔
		l32 l32SkipFrames;		// 允许连续发空帧个数
		l32 l32EncMode;		    // 编码方式	 
        l32 l32PicMerge;        // 是否画面合成
		u32 u32Quality;			// 指定使用的264编码器
        l32 l32CoreNum;         //编码用到的内核数
        l32 l32SliceLen;
    } TH264EncParam;
    /*-------------------------------------------------------------------------
	l32EncMode意义说明：
    1:帧编码方式 H264EncFrame.bitstream指向完整的一帧编码后数据
	0:RTP包编码方式（默认）H264EncFrame.bitstream的前MAX_NALU_UNIT_SIZE字节表示
    MAX_NALU_NUM个int32_t类型的数，这些int32_t数的非零值的个数表示每帧实际的Nalu
    个数（即每帧切分的RTP包个数），int32_t数表示该帧中的某个Nalu长度，
	H264EncFrame.bitstream+MAX_NALU_UNIT_SIZE指向完整的一帧编码后数据
    --------------------------------------------------------------------------*/			
        
	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TH264EncFramePara.bitstream缓冲区图例
//  ----------------------------------------------------------------------------------------------------------------------------------------
//  | Nalulen 1 | Nalulen 2 | Nalulen 3 | ..... | Nalulen N |     Nalu 1     |     Nalu 2     |     Nalu 3     | ........ |     Nalu n     |
//  ----------------------------------------------------------------------------------------------------------------------------------------
//  |-- MAX_NALU_UNIT_SIZE = sizeof(int32_t) * MAX_NALU_NUM  -- | ------------------------encoded frame data-----------------------------------|
//  注：
//  1. 其中N为最大的NALU个数MAX_NALU_NUM，  n为一帧编码数据中实际Nalu的个数
//  2. 若nEncMode==1, 则bitstream缓冲区中没有前MAX_NALU_NUM个int32_t类型的数，只有编码后的帧数据
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  

	
// 接口函数定义

    
    //编码器初始化函数
	H264encDrvError_t H264EncoderInit(void **ppvHandle,   TH264EncParam * ptEncParam);
		
    //编码一个空帧函数
	H264encDrvError_t H264EncoderNull (void *pvHandle, TH264EncFramePara* ptFrame); 

    //编码一帧图像函数
	H264encDrvError_t H264EncoderFrame(void *pvHandle, TH264EncFramePara* ptFrame);	
	
    //修改编码器参数函数
	H264encDrvError_t H264SetEncParams(void *pvHandle, TH264EncParam * ptEncParam) ;	
	
    //释放编码器资源函数	
	H264encDrvError_t H264EncoderClose(void *pvHandle);	

    //编码器帮助函数
    void H264EncHelp(void);
	
    //H264编码器版本信息函数
    void  GetH264Version(void *pvVer, l32 l32BufLen, l32 *pl32VerLen);

	//查询编码器状态
	H264encDrvError_t H264EncoderGetStatus(void *pvHandle, TH264EncParam * ptEncParam);
	
//根据图像大小获取公共Buffer大小  返回值单位为kb 输入高/宽应是CIF图像整数倍
	//输入高/宽应为0 将返回-1
	l32 H264EncoderGetBufferSize(l32 l32FrameWidth, l32 l32FrameHeight);
	
	
#ifdef __cplusplus
};
#endif


#endif //end of "#ifndef H264ENC_H"





















