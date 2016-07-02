#ifndef _PA_TYPE_H_
#define _PA_TYPE_H_
typedef u32 u32_ip;

enum EmBegin
{
	emBegin,
};

enum EmPAMediaType
{
	emPAMediaNone,
		emPAMediaAudio,
		emPAMediaVideo,
		emPAMediaH224,
		emPAMediaT120,
};

enum EmPAProfileMask
{
	emPAProfileBaseline = 64,
	emPAProfileMain = 32,
	emPAProfileExtend = 16,
	emPAProfileHigh = 8,
};

//视频协议类型
enum EmPAVideoFormat
{
    emPAVH261     = 0,
		emPAVH262     = 1,//MPEG2
		emPAVH263     = 2,
		emPAVH263plus = 3,
		emPAVH264     = 4,
		emPAVMPEG4    = 5,
		
		emPAVEnd		
};

//音频协议类型
enum EmPAAudioFormat
{
    emPAAG711a  = 0,
		emPAAG711u  = 1,
		emPAAG722   = 2,
		emPAAG7231  = 3,
		emPAAG728   = 4,
		emPAAG729   = 5,
		emPAAMP3    = 6,
		emPAAG721   = 7,
		emPAAG7221  = 8,
		emPAAG719   = 9,
		emPAAMpegAACLC =10,
		emPAAMpegAACLD = 11,
		
		emPAAEnd	 
};

enum  EmPAPayloadType 
{
	emPAPayloadTypeBegin = 0,
		//音频格式
		emPAAudioTypeBegin,
		emPAG711a,
		emPAG711u,
		emPAG722,
		emPAG7231,
		emPAG728,
		emPAG729,
		emPAMp3,  
		emPAG723,
		emPAG7221,
		emPAMpegaaclc,
		emPAMpegaacld,
		emPAG719,
		emPAAudioTypeEnd = 49,
		
		//视频格式
		emPAVideoTypeBegin = 50,	
		emPAH261,
		emPAH263,
		emPAH263plus,
		emPAH264,
		emPAH262,
		emPAMpeg4,
		emPAVideoTypeEnd = 99,
        
		emPADataTypeBegin = 100,	
		emPAT120,
		emPAH224,
		emPADataTypeEnd = 149,
		
		emPAPayloadTypeEnd = 255
};

//分辨率
enum EmPAResolution
{
	emPAResolutionBegin = 0,
		emPASQCIF,
		emPAQCIF,
		emPACIF,
		emPACIF2,
		emPACIF4,
		emPACIF16,
		
		emPASIF,
		emPASIF4,
		emPAVGA,
		emPASVGA,
		emPAXGA,
		
		emPAWCIF4,  //1024*576
		emPAHD720,  //1280*720
		emPASXGA,   //1280*1024
		emPAUXGA,   //1600*1200
		emPAHD1080, //1920*1080
		emPAAuto,
		emPAResolutionEnd
};

enum EmPAVideoResolution
{
    emPAVResolutionAuto     = 0,   //自动
		emPAVSQCIF              = 1,   //SQCIF88x72
		emPAVQCIF               = 2,   //QCIF176x144
		emPAVCIF                = 3,   //CIF352x288
		emPAV2CIF               = 4,   //2CIF352x576
		emPAV4CIF               = 5,   //4CIF704x576
		emPAV16CIF              = 6,   //16CIF1408x1152
		
		emPAVHD480i720x480      = 7,   //480i720x480
		emPAVHD480p720x480      = 8,   //480p720x480
		emPAVHD576i720x576      = 9,   //576i720x576
		emPAVHD576p720x576      = 10,  //576p720x576
		
		emPAVHD720p1280x720     = 11,  //720p1280x720
		emPAVHD1080i1920x1080   = 12,  //1080i1920x1080
		emPAVHD1080p1920x1080   = 13,  //1080p1920x1080
		
		
		emPAVGA352x240          = 14,  //352x240
		emPAVGA704x480          = 15,  //704x480
		emPAVGA640x480          = 16,  //VGA640x480
		emPAVGA800x600          = 17,  //SVGA800x600
		emPAVGA1024x768         = 18,  //XGA1024x768
		emPAVGA1280x768         = 19,  //WXGA1280x768
		emPAVGA1280x800         = 20,  //WXGA1280x800
		emPAVGA1366x768         = 21,  //WXGA1366x768
		emPAVGA1280x854         = 22,  //WSXGA1280x854
		emPAVGA1440x900         = 23,  //WSXGA1440x900
		emPAVGA1280x1024        = 24,  //SXGA1280x1024
		emPAVGA1680x1050        = 25,  //SXGA+1680x1050
		emPAVGA1600x1200        = 26,  //UXGA1600x1200
		emPAVGA1920x1200        = 27,  //WUXGA1920x1200
		
		emPAVSQCIF112x96        = 28,
		emPAVSQCIF96x80         = 29,
		emPAV320x192            = 30,
		emPAV432x240            = 31,
		emPAV480x272            = 32,
		emPAV640x368            = 33,
		emPAV864x480            = 34,
		emPAV960x544            = 35,
		emPAV1440x816           = 36,
		
		emPAVResEnd = 50,
};

enum EmDualReason
{
	emDualAccept,//接收
	emDualBusy,//忙
	emDualPreemptive,//抢发双流
	emDualPeerReject,//对端拒绝
	emDualPeerStop,//对端停止双流
	emDualNoVideo,//无双流视频源
    emDualLocalStop, //本地停止双流
};

enum EmPAOprType
{
	    PA_OprType_Add,			//增加
		PA_OprType_Del,			//删除
		PA_OprType_Update		//修改
};

// enum EmPACnsCallReason	
// {
// 	    EmPACnsCallReason_success,
// 		EmPACnsCallReason_hungup,			//对端不接受或者主动挂断
// 		EmPACnsCallReason_Unreachable,	//对端不在线，无法呼叫
// 		EmPACnsCallReason_resourcefull,	//对端无可用资源
// 		EmPACnsCallReason_dstnotfind,		//目标不存在
// 		EmPACnsCallReason_Busy,			//对端忙
// 		EmPACnsCallReason_Local,			//本地原因
// 		EmPACnsCallReason_NoConf,			//会议不存在
// 		EmPACnsCallReason_Exist			//对端已在本会议中
// };
enum EmPATPEndpointType
{
	    emPATPEndpointTypeUnknown    = 0,
		emPATPEndpointTypeCNS	     = 0x01,
		emPATPEndpointTypeUMS		 = 0x02,
		emPATPEndpointTypeRegister   = 0x04,

		emPATPEndpointH323MT,
		emPATPEndpointH323MCU,
		emPATPEndpointH323SingleMCU,
		emPATPEndpointH323G400,             

};

enum EmPATpApplySpeakerRet
{
	    emPA_ApplySpeaker_Success,  
		emPA_ApplySpeaker_ConfUnExist,   //会议不存在
		emPA_ApplySpeaker_EpIDError,     //会场不存在
		emPA_ApplySpeaker_EpStateError,  //会场未在线或通道不未建立
};

enum EmPATPDualReason
{
	    emPATpDualAccept,
		emPATpDualBusy,
		emPATpDualPreemptive,
		emPATpDualPeerReject,
		emPATpDualPeerStop,
		emPATpDualNoVideo,
		emPATpDualLocalStop,
		emPATpDualSndFail,            //发送双流失败
};

enum EmPACnsCallReason	
{
		EmPACnsCallReason_success,
		EmPACnsCallReason_hungup,			//本地挂断
		EmPACnsCallReason_Unreachable,	//对端不在线，无法呼叫
		EmPACnsCallReason_resourcefull,	//对端无可用资源
		EmPACnsCallReason_dstnotfind,		//目标不存在
		EmPACnsCallReason_Busy,			//对端忙
		EmPACnsCallReason_Local,			//本地原因
		EmPACnsCallReason_NoConf,			//会议不存在
		EmPACnsCallReason_Exist,			//对端已在本会议中
		EmPACnsCallReason_Rejected,		//对端拒绝
		EmPACnsCallReason_peerhungup,		//对端挂断
		//	EmCnsCallReason_AlreadyDual,    //对端在双流
		EmPACnsCallReason_Abnormal,       //本地掉线挂断
		EmPACnsCallReason_PeerAbnormal,   //对端掉线挂断
		EmPACnsCallReason_unknown,   
};

enum EmPATpMixStatus
{
	emPAAudMixIdle,
		emPAAudMixAssignChan_Fail,
		emPAAudMixAssignChan_Success,
		
		emPAAudMixOpenChan_Fail,		//创建语音激励通道失败
		emPAAudMixOpenChan_Success,   //创建语音激励通道成功
		
		emPAAudMixDisConnect,			//混音板断链
		
		emPAAudOpenChan_Fail,		//打开语音通道失败
		emPAAudOpenChan_Success    //打开语音通道成功
};

enum EmPAAliasType
{
	    emPA_Alias_Unknow,
		emPA_Alias_e164,
		emPA_Alias_h323,
		emPA_Alias_IP,
};

enum EmPADisListOprType
{
	emPA_DisListOpr_Add,
		emPA_DisListOpr_Del,
};

enum EmPADisListOprReslt
{
	emPA_DisListOpr_Sucess,
		emPA_DisListOpr_Fail,
		emPA_DisListOpr_Reject,
		
};


enum EmPAAACSampleFreq                    //sampling frequency
{
	emPAFs96000 = 0,
        emPAFs88200 = 1,
        emPAFs64000 = 2,
        emPAFs48000 = 3,
        emPAFs44100 = 4,
        emPAFs32000 = 5,
        emPAFs24000 = 6,
        emPAFs22050 = 7,
        emPAFs16000 = 8,
        emPAFs12000 = 9,
        emPAFs11025 = 10,
        emPAFs8000  = 11
};

enum EmPAAACChnlCfg                       //channel config
{
	emPAChnlCust   = 0,
        emPAChnl1      = 1,                   //单声道
        emPAChnl2      = 2,                   //双声道
        emPAChnl3      = 3,
        emPAChnl4      = 4,
        emPAChnl5      = 5,
        emPAChnl5dot1  = 6,                   //5.1声道
        emPAChnl7dot1  = 7                    //7.1声道
};

#endif

