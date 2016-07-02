/*****************************************************************************
   模块名      : MtAdp
   文件名      : mastruct36.h
   相关文件    : 
   文件实现功能: 3.6版本兼容结构定义 (同步自12月21日的3.6版本)
   作者        : 许世林
   版本        : V4.0  Copyright(C) 2005-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2005/12     4.0         许世林          创建(目前只同步终端状态与会议信息结构)
******************************************************************************/

#ifndef _MASTRUCT36_H
#define _MASTRUCT36_H

#include "kdvtype.h"

#ifdef WIN32
#pragma pack( push )
#pragma pack( 1 )
#define PACKED 
#else
#define PACKED __attribute__((__packed__))	
#endif

//媒体类型定义
#define   MEDIA_TYPE_NULL_VER36                    255  //媒体类型为空

//音频
#define	  MEDIA_TYPE_MP3_VER36	                    96  //mp3 mode 0-4
#define	  MEDIA_TYPE_MP2_VER36	                    98  //mp3 mode 0-4
#define   MEDIA_TYPE_PCMA_VER36						8   //G.711 Alaw  mode 5
#define   MEDIA_TYPE_PCMU_VER36						0   //G.711 ulaw  mode 6
#define   MEDIA_TYPE_G721_VER36						2   //G.721
#define   MEDIA_TYPE_G722_VER36						9   //G.722
#define	  MEDIA_TYPE_G7231_VER36		            4   //G.7231
#define	  MEDIA_TYPE_G728_VER36						15  //G.728
#define	  MEDIA_TYPE_G729_VER36						18  //G.729
#define   MEDIA_TYPE_G7221_VER36					13  //G.7221

//视频
#define   MEDIA_TYPE_MP4_VER36	                    97  //MPEG-4
#define   MEDIA_TYPE_H261_VER36						31  //H.261
#define   MEDIA_TYPE_H262_VER36						33  //H.262 (MPEG-2)
#define   MEDIA_TYPE_H263_VER36						34  //H.263
#define   MEDIA_TYPE_H263PLUS_VER36					101 //H.263+
#define   MEDIA_TYPE_H264_VER36						106 //H.264

//数据
#define   MEDIA_TYPE_H224_VER36						100  //H.224 Payload 暂定100
#define   MEDIA_TYPE_T120_VER36                     200  //T.120媒体类型
#define   MEDIA_TYPE_H239_VER36                     239  //H.239数据类型 
#define   MEDIA_TYPE_MMCU_VER36                     120  //级联数据通道 

//kdv约定的本地发送时使用的活动媒体类型 
#define   ACTIVE_TYPE_PCMA_VER36		            110   //G.711 Alaw  mode 5
#define   ACTIVE_TYPE_PCMU_VER36		            111   //G.711 ulaw  mode 6
#define   ACTIVE_TYPE_G721_VER36		            112   //G.721
#define   ACTIVE_TYPE_G722_VER36		            113   //G.722
#define	  ACTIVE_TYPE_G7231_VER36		            114   //G.7231
#define	  ACTIVE_TYPE_G728_VER36		            115   //G.728
#define	  ACTIVE_TYPE_G729_VER36		            116   //G.729
#define   ACTIVE_TYPE_G7221_VER36                   117   //G.7221
#define   ACTIVE_TYPE_H261_VER36	                118   //H.261
#define   ACTIVE_TYPE_H262_VER36	                119   //H.262 (MPEG-2)
#define   ACTIVE_TYPE_H263_VER36	                120   //H.263

//设备主类定义
#define		TYPE_MCU_VER36							1   //MCU
#define		TYPE_MCUPERI_VER36						2   //MCU外设
#define		TYPE_MT_VER36							3   //终端

//远遥
#define		CAMERA_MAX_NUM_VER36					6	 //最大摄像机数目

//会议id长度
#define     MAXLEN_CONFID_VER36						16

//传送图像声音参数模式
#define		MODE_NONE_VER36			                0	 //音视频都不传
#define		MODE_VIDEO_VER36					    1	 //只传视频
#define		MODE_AUDIO_VER36					    2	 //只传音频
#define		MODE_BOTH_VER36					        3   //音视频都传
#define     MODE_DATA_VER36                         4   //数据 
#define     MODE_SECVIDEO_VER36                     5   //第二路视频

//视频格式定义
#define		VIDEO_FORMAT_SQCIF_VER36			    1   //SQCIF(128*96)
#define		VIDEO_FORMAT_QCIF_VER36				    2   //SCIF(176*144)
#define		VIDEO_FORMAT_CIF_VER36                  3   //CIF(352*288)
#define		VIDEO_FORMAT_4CIF_VER36                 4   //4CIF(704*576)
#define		VIDEO_FORMAT_16CIF_VER36			    5   //16CIF(1408*1152)
#define		VIDEO_FORMAT_AUTO_VER36				    6   //自适应，仅用于MPEG4
#define		VIDEO_FORMAT_2CIF_VER36				    7   //2CIF(352*576)

//视频双流类型定义
#define		VIDEO_DSTREAM_H263PLUS_VER36		    0   //H263＋
#define		VIDEO_DSTREAM_MAIN_VER36			    1   //与主视频格式一致
#define		VIDEO_DSTREAM_H263PLUS_H239_VER36       2   //H263＋的H239
#define		VIDEO_DSTREAM_H263_H239_VER36           3   //H263的H239
#define		VIDEO_DSTREAM_H264_H239_VER36           4   //H264的H239

//会议开放方式定义
#define		CONF_OPENMODE_CLOSED_VER36              0  //不开放,拒绝列表以外的终端
#define		CONF_OPENMODE_NEEDPWD_VER36             1  //根据密码加入
#define		CONF_OPENMODE_OPEN_VER36                2  //完全开放 

//会议加密方式定义
#define     CONF_ENCRYPTMODE_NONE_VER36             0 //不加密
#define     CONF_ENCRYPTMODE_DES_VER36              1 //des加密
#define     CONF_ENCRYPTMODE_AES_VER36              2 //aes加密

//会议码流转发时是否支持归一化整理方式定义, 与丢包重传方式以及加密方式互斥
#define     CONF_UNIFORMMODE_NONE_VER36             0 //不归一化重整
#define     CONF_UNIFORMMODE_VALID_VER36            1 //  归一化重整
#define     SWITCHCHANNEL_UNIFORMMODE_NONE_VER36    0 //不归一化重整
#define     SWITCHCHANNEL_UNIFORMMODE_VALID_VER36   1 //  归一化重整

//数据会议方式定义
#define		CONF_DATAMODE_VAONLY_VER36              0  //不包含数据的视音频会议
#define		CONF_DATAMODE_VAANDDATA_VER36           1  //包含数据的视音频会议
#define		CONF_DATAMODE_DATAONLY_VER36            2  //不包含视音频的数据会议

//会议结束方式定义
#define     CONF_RELEASEMODE_NONE_VER36             0  //不会自动结束
#define     CONF_RELEASEMODE_NOMT_VER36             1  //无终端时自动结束

//会议进行方式
#define     CONF_TAKEMODE_SCHEDULED_VER36           0  //预约会议
#define     CONF_TAKEMODE_ONGOING_VER36             1  //即时会议
#define     CONF_TAKEMODE_TEMPLATE_VER36            2  //会议模板

//会议保护方式
#define		CONF_LOCKMODE_NONE_VER36                0  //未保护
#define		CONF_LOCKMODE_NEEDPWD_VER36             1  //根据密码操作
#define		CONF_LOCKMODE_LOCK_VER36                2  //独享 

//发言人的源的定义
#define		CONF_SPEAKERSRC_SELF_VER36              0  //看自己
#define		CONF_SPEAKERSRC_CHAIR_VER36             1  //看主席
#define		CONF_SPEAKERSRC_LAST_VER36              2  //看上一次发言人

//呼叫终端方式定义
#define		CONF_CALLMODE_NONE_VER36                0  //不呼叫
#define		CONF_CALLMODE_ONCE_VER36                1  //呼叫一次
#define		CONF_CALLMODE_TIMER_VER36               2  //定时呼叫

//定时呼叫终端时间间隔定义
#define		DEFAULT_CONF_CALLINTERVAL_VER36         20 //缺省的定时呼叫间隔（秒）
#define		MIN_CONF_CALLINTERVAL_VER36             10 //最小的定时呼叫间隔（秒）

//定时呼叫终端呼叫次数定义
#define		DEFAULT_CONF_CALLTIMES_VER36            0  //无穷次定时呼叫次数
#define		MIN_CONF_CALLTIMES_VER36                2  //最小的定时呼叫次数

//画面合成方式定义
#define		CONF_VMPMODE_NONE_VER36                 0  //不进行画面合成
#define		CONF_VMPMODE_CTRL_VER36                 1  //会控或主席选择成员合成
#define		CONF_VMPMODE_AUTO_VER36                 2  //MCU自动选择成员合成

//会议录像方式定义 
#define		CONF_RECMODE_NONE_VER36                 0  //不录像
#define		CONF_RECMODE_REC_VER36                  1  //录像
#define		CONF_RECMODE_PAUSE_VER36                2  //暂停

//会议放像方式定义
#define		CONF_PLAYMODE_NONE_VER36                0  //不放像
#define		CONF_PLAYMODE_PLAY_VER36                1  //放像
#define		CONF_PLAYMODE_PAUSE_VER36               2  //暂停
#define		CONF_PLAYMODE_FF_VER36                  3  //快进
#define		CONF_PLAYMODE_FB_VER36                  4  //快退

//会议轮询方式定义
#define		CONF_POLLMODE_NONE_VER36                0  //不轮询广播 
#define		CONF_POLLMODE_VIDEO_VER36               1  //仅图像轮询广播
#define		CONF_POLLMODE_SPEAKER_VER36             2  //轮流发言

//码率适配方式定义
#define		CONF_BASMODE_NONE_VER36                 0  //不进行码率适配 
#define		CONF_BASMODE_RTP_VER36                  1  //RTP头重组适配
#define		CONF_BASMODE_BR_VER36                   2  //码率适配
#define		CONF_BASMODE_MDTP_VER36                 3	//媒体类型适配

//画面合成成员类型定义
#define		VMP_MEMBERTYPE_MCSSPEC_VER36            1  //会控指定 
#define		VMP_MEMBERTYPE_SPEAKER_VER36            2  //发言人跟随
#define		VMP_MEMBERTYPE_CHAIRMAN_VER36           3  //主席跟随
#define		VMP_MEMBERTYPE_POLL_VER36               4  //轮询视频跟随
#define		VMP_MEMBERTYPE_VAC_VER36                5	//语音激励(会控不要用此类型)

//画面合成风格定义
#define		VMP_STYLE_DYNAMIC_VER36                 0   //动态分屏(仅自动合成时有效)
#define		VMP_STYLE_ONE_VER36                     1   //一画面
#define		VMP_STYLE_VTWO_VER36                    2   //两画面：左右分 
#define		VMP_STYLE_HTWO_VER36                    3   //两画面: 一大一小
#define		VMP_STYLE_THREE_VER36                   4   //三画面
#define		VMP_STYLE_FOUR_VER36                    5   //四画面
#define		VMP_STYLE_SIX_VER36                     6   //六画面 
#define		VMP_STYLE_EIGHT_VER36                   7   //八画面
#define		VMP_STYLE_NINE_VER36                    8   //九画面
#define		VMP_STYLE_TEN_VER36                     9   //十画面
#define		VMP_STYLE_THIRTEEN_VER36                10  //十三画面
#define		VMP_STYLE_SIXTEEN_VER36                 11  //十六画面
#define		VMP_STYLE_SPECFOUR_VER36                12  //特殊四画面 
#define		VMP_STYLE_SEVEN_VER36                   13  //七画面

//MCU配置和业务常量定义
#define		MAXLEN_ALIAS_VER36			            128  //MCU或终端假名最多127个字母，63个汉字
#define     VALIDLEN_ALIAS							16   //有效别名它度
#define		MAXLEN_CONFNAME_VER36					64   //会议名最大长度
#define		MAXLEN_PWD_VER36						32   //最大密码长度  
 
#define		MAXLEN_EQP_ALIAS						16   //外设别名最大长度 
#define		MAXLEN_E164_VER36						16   //最大E164号码长度 
#define     MAXLEN_KEY_VER36						16   //最大的加密key长度        

#define     LEN_DES_VER36							8	 //byte
#define     LEN_AES_VER36							16	 //byte
#define     LOCAL_MCUID_VER36						192  //本地MCU ID

//外设常量定义
#define     MAXNUM_MIXER_DEPTH_VER36				10  //混音器最大混音深度
#define     MAXNUM_VMP_MEMBER_VER36					16  //最大画面合成成员数
#define     DEFAULT_MIXER_VOLUME_VER36			    250	//缺省的混音音量

//终端状态
struct _TTERSTATUS_VER36
{
    ///////会议状态///////
    u8		bOnline;					//是否在会议中	1 yes 0 no
    u8		byConfMode;					//会议模式		1 两点 2 多点		
    u8		byPollStat;					//轮询状态		1 开始 2 暂停 3 停止
    u8		byCallMode;					//呼叫方式		1 GK路由 0 直接呼叫
    u8		bRegGkStat;					//GK注册结果	1 成功 0 失败
    u8		byEncryptConf;				//加密状态		1 加密 0 不加密(只是表示本端发送是否加密)
    
    ///////设备状态///////
    u8		byMtBoardType;				//终端板卡类型	0: WIN,1:8010,2:8010a,3:8018,4:IMT,5:8010C
    u8		bMatrixStatus;				//矩阵状态		(1 ok 0 err)
    u8		byCamCtrlSrc;				//当前控制摄像头源类型(0:本地,1:远端)
    u8		byLocalCamCtrlNo;			//当前控制的本地摄像头号(1-6号摄像头)
    u8		byRemoteCamCtrlNo;			//当前控制的远端摄像头号(1-6号摄像头)
    u8		byCamNum;					//摄像头个数	(0~6)
    u8		bCamStatus[CAMERA_MAX_NUM_VER36];	//摄像头状态 (1 ok 0 err)
    u8		byVideoSrc;					//当前本地视频源(0:S端子,1-6:C端子)
    u8		bRemoteCtrl;				//是否允许远遥	1 yes 0 no
    
    //////编解码状态////////
    u8		byAEncFormat;				//音频编码格式(AUDIO_G711A,AUDIO_MP3...)
    u8		byADecFormat;				//音频解码格式(AUDIO_G711A,AUDIO_MP3...)
    
    u8		abyVEncFormat[2];			//视频编码格式(VIDEO_H261,VIDEO_H263...)
    u8		abyVEncRes[2];				//视频编码分辨率(VIDEO_QCIF,VIDEO_CIF...)
    u16		awSendBitRate[2];			//视频编码码率(单位:kbps)
    u16		awSendAVBitRate[2];			//平均编码码率(单位:kbps)
    
    u8		abyVDecFormat[2];			//视频解码格式(VIDEO_H261,VIDEO_H263...)
    u8		abyVDecRes[2];				//视频解码分辨率(VIDEO_QCIF,VIDEO_CIF...)
    u16		awRecvBitRate[2];			//视频解码码率(单位:kbps)
    u16		awRecvAVBitRate[2];			//平均解码码率(单位:kbps)
    
    u8      abyEncryptMode[2];			//编码使用的加密算法(ENCRYPT_AES/ENCRYPT_DES/ENCRYPT_NONE)
    u8		abyDecryptMode[2];			//解码使用的加密算法(ENCRYPT_AES/ENCRYPT_DES/ENCRYPT_NONE)
    
    u8		byEncVol;					//当前编码音量(单位:等级)
    u8		bMute;						//是否哑音			1 yes 0 no
    u8		byDecVol;					//当前解码音量(单位:等级)
    u8		bQuiet;						//是否静音			1 yes 0 no
    u8		bLocalLoop;					//是否自环			1 yes 0 no
    u8		byAudioPower;				//是否在测试音频	1 yes 0 no
    u8		byLocalImageTest;			//是否本地图像测试1 yes 0 no
    u8		byRmtImageTest;				//是否远端图像测试 1 yes 0 no
    u16		wSysSleep;					//待机时间(0xFFFF表示不待机)
    
    //////配置状态///////
    
}PACKED;

//定义会议方式结构(会议中会改变)
struct TConfModeVer36
{
protected:
	u8     m_byTakeMode;        //会议举行方式: 0-预约 1-即时 2-会议模板 
	u8     m_byLockMode;        //会议锁定方式: 0-不锁定,所有会控可见可操作 1-根据密码操作此会议 2-某个会议控制台锁定
    u8     m_byCallMode;        //呼叫终端方式: 0-不呼叫终端，手动呼叫 1-呼叫一次 2-定时呼叫未与会终端
	u8     m_byVACMode;         //语音激励方式: 0-不启用语音激励控制发言(导演控制发言) 1-启用语音激励控制发言
	u8     m_byAudioMixMode;    //混音方式:     0-不混音 1-智能混音
	u8     m_byVMPMode;         //图像复合方式: 0-不图像复合 1-会控或主席控制图像复合 2-自动图像复合(动态分屏与设置成员)
	u8     m_byRecordMode;      //会议录像方式: (BIT:0-6)0-不录像 1-录像 2-录像暂停; BIT7 = 0 实时 1抽帧录像
	u8     m_byPlayMode;        //会议放像方式: 0-不放像 1-放像 2-放像暂停 3-快进 4-快退
	u8     m_byBasMode;         //码率适配方式: BIT0-是否RTP头适配 BIT1-是否码率适配 BIT2-是否类型适配
	u8     m_byPollMode;        //会议轮询方式: 0-不轮询广播 1-仅图像轮询广播 2-轮流发言
	u8     m_byDiscussMode;     //会议讨论模式: 0-非讨论模式(演讲模式) 1-讨论模式
	u8     m_byForceRcvSpeaker; //强制收看发言: 0-不强制收看发言人 1-强制收看发言人
	u8     m_byNoChairMode;     //会议主席模式: 0-不是无主席方式 1-无主席方式
	u8     m_byRegToGK;         //会议注册GK情况: 0-会议未在GK上注册 1-会议在GK上成功注册
	u8     m_byMixSpecMt;		//是否指定终端混音: 0-不是指定终端混音(所有终端参加混音) 1-指定终端混音
	u32    m_dwCallInterval;    //定时呼叫时的呼叫间隔：(单位：秒，最小为5秒)（网络序）
	u32    m_dwCallTimes;       //定时呼叫时的呼叫次数（网络序）
public:
	TConfModeVer36( void ){ memset( this, 0, sizeof(TConfModeVer36) ); }
    void   SetTakeMode(u8 byTakeMode){ m_byTakeMode = byTakeMode;} 
    u8     GetTakeMode( void ) const { return m_byTakeMode; }
    void   SetLockMode(u8   byLockMode){ m_byLockMode = byLockMode;} 
    u8     GetLockMode( void ) const { return m_byLockMode; }
    void   SetCallMode(u8   byCallMode){ m_byCallMode = byCallMode;} 
    u8     GetCallMode( void ) const { return m_byCallMode; }
	void   SetCallInterval(u32 dwCallInterval){ m_dwCallInterval = htonl(dwCallInterval);} 
    u32    GetCallInterval( void ) const { return ntohl(m_dwCallInterval); }
	void   SetCallTimes(u32 dwCallTimes){ m_dwCallTimes = htonl(dwCallTimes);} 
    u32    GetCallTimes( void ) const { return ntohl(m_dwCallTimes); }
    void   SetVACMode(BOOL32 bVACMode){ m_byVACMode = bVACMode;} 
    BOOL32 IsVACMode( void ) const { return m_byVACMode == 0 ? FALSE : TRUE; }
    void   SetAudioMixMode(BOOL32 bAudioMixMode){ m_byAudioMixMode = bAudioMixMode;} 
    BOOL32 IsAudioMixMode( void ) const { return m_byAudioMixMode == 0 ? FALSE : TRUE; }
    void   SetVMPMode(u8   byVMPMode){ m_byVMPMode = byVMPMode;} 
    u8     GetVMPMode( void ) const { return m_byVMPMode; }
	void   SetRegToGK( BOOL32 bRegToGK ){ m_byRegToGK = bRegToGK; }
	BOOL32 IsRegToGK( void ){ return m_byRegToGK == 0 ? FALSE : TRUE; }
    void   SetRecordMode(u8   byRecordMode)
	{   byRecordMode   &= 0x7f ;
		m_byRecordMode &= 0x80 ;
		m_byRecordMode |= byRecordMode;
	} 
    u8     GetRecordMode( void ) const { return m_byRecordMode & 0x7f; }
	
	//判断当前是否在抽帧录像，若是返回TRUE否则返回FALSE
	BOOL32   IsRecSkipFrame(){ return m_byRecordMode & 0x80 ? TRUE : FALSE;}
	//bSkipFrame = TRUE 设置当前为抽帧录像,否则实时录像
	void   SetRecSkipFrame( BOOL32 bSkipFrame )
	{
		if(bSkipFrame)
		{
			m_byRecordMode |=0x80;
		}
		else 
		{
			m_byRecordMode &=0x7f;
		}
	}
    void   SetPlayMode( u8  byPlayMode ){ m_byPlayMode = byPlayMode;} 
    u8     GetPlayMode( void ) const { return m_byPlayMode; }
    void   SetBasMode( u8  byBasMode, BOOL32 bHasBasMode )
	{ 
		if( bHasBasMode )
		{
			m_byBasMode |= (1<<(byBasMode-1)) ;
		}
		else
		{
			m_byBasMode &= ~(1<<(byBasMode-1));
		}
	} 
    BOOL32   GetBasMode( u8  byBasMode ) const 
	{ 
		return m_byBasMode & (1<<(byBasMode-1));
	}
    void   SetPollMode(u8   byPollMode){ m_byPollMode = byPollMode;} 
    u8     GetPollMode( void ) const { return m_byPollMode; }
    void   SetDiscussMode(u8   byDiscussMode){ m_byDiscussMode = byDiscussMode;} 
    BOOL32 IsDiscussMode( void ) const { return m_byDiscussMode == 0 ? FALSE : TRUE; }  
    void   SetForceRcvSpeaker(u8   byForceRcvSpeaker){ m_byForceRcvSpeaker = byForceRcvSpeaker;} 
    BOOL32 IsForceRcvSpeaker( void ) const { return m_byForceRcvSpeaker == 0 ? FALSE : TRUE; }
    void   SetNoChairMode(u8   byNoChairMode){ m_byNoChairMode = byNoChairMode;} 
    BOOL32 IsNoChairMode( void ) const { return m_byNoChairMode == 0 ? FALSE : TRUE; }
    void   SetMixSpecMt( BOOL32 bMixSpecMt ){ m_byMixSpecMt = bMixSpecMt;} 
    BOOL32 IsMixSpecMt( void ) const { return m_byMixSpecMt == 0 ? FALSE : TRUE; }
	void   Print( void )
	{
		StaticLog("\nConfMode:\n" );
		StaticLog("m_byTakeMode: %d\n", m_byTakeMode);
		StaticLog("m_byLockMode: %d\n", m_byLockMode);
		StaticLog("m_byCallMode: %d\n", m_byCallMode);
		StaticLog("m_dwCallInterval: %d\n", m_dwCallInterval);
		StaticLog("m_dwCallTimes: %d\n", m_dwCallTimes);
		StaticLog("m_byVACMode: %d\n", m_byVACMode);
		StaticLog("m_byAudioMixMode: %d\n", m_byAudioMixMode);
		StaticLog("m_byVMPMode: %d\n", m_byVMPMode);
		StaticLog("m_byRecordMode: %d\n", m_byRecordMode);
		StaticLog("m_byPlayMode: %d\n", m_byPlayMode);
		StaticLog("m_byBasMode: %d\n", m_byBasMode);
		StaticLog("m_byPollMode: %d\n", m_byPollMode);
		StaticLog("m_byDiscussMode: %d\n", m_byDiscussMode);
		StaticLog("m_byForceRcvSpeaker: %d\n", m_byForceRcvSpeaker);
		StaticLog("m_byNoChairMode: %d\n", m_byNoChairMode);
		StaticLog("m_byRegToGK: %d\n", m_byRegToGK);
		StaticLog("m_byMixSpecMt: %d\n", m_byMixSpecMt);
	}
}PACKED;

//记录录像或放像进度(单位:秒)
struct  TRecProgVer36
{
protected:
    u32   		m_dwCurProg;	//当前进度
    u32   		m_dwTotalTime;	//总长度，仅在放像时有效
    
public:
    //设置当前进度位置（单位：秒）
    void SetCurProg( u32    dwCurProg )	{ m_dwCurProg = htonl( dwCurProg ); }
    //获取当前进度位置（单位：秒）
    u32    GetCurProg( void ) const	{ return ntohl( m_dwCurProg ); }
    //设置总长度，仅对放像有效（单位：秒）
    void SetTotalTime( u32    dwTotalTime )	{ m_dwTotalTime = htonl( dwTotalTime ); }
    //获取总长度，仅对放像有效（单位：秒）
    u32    GetTotalTime( void ) const	{ return ntohl( m_dwTotalTime ); }

}PACKED;

//定义简化MCU、终端或外设结构 (len:6)
struct TMtVer36
{    
protected:
    u8          m_byMainType;   //设备主类
    u8  		m_bySubType;	//设备子类
    u8  		m_byMcuId;		//MCU号
    u8  		m_byEqpId;	    //设备号	
    u8          m_byConfDriId;  //DRI板号
    u8          m_byConfIdx;    //会议索引号，即会议实例号 1 - MAXNUM_MCU_CONF
    
public:
    //操作所有类型函数
    void   SetType( u8   byType ){ m_byMainType = byType; }         //设置结构主类
    u8     GetType( void ) const { return m_byMainType; }           //得到结构主类
    void   SetMcuId( u8   byMcuId ){ m_byMcuId = byMcuId; }         //设置McuId
    u8     GetMcuId( void ) const { return m_byMcuId; }             //得到McuId
    BOOL32   operator ==( const TMtVer36 & tObj ) const;                  //判断是否相等
    
    //操作MCU函数
    void   SetMcu( u8   byMcuId );                         //设置MCU
    void   SetMcuType( u8  byMcuType ){ m_byMainType = TYPE_MCU_VER36; m_bySubType = byMcuType; }
    u8     GetMcuType( void ) const { return m_bySubType; }
    
    //操作外设函数
    void   SetMcuEqp( u8   byMcuId, u8   byEqpId, u8   byEqpType );            //设置MCU外设
    void   SetEqpType( u8   byEqpType ){ m_byMainType = TYPE_MCUPERI_VER36; m_bySubType = byEqpType; }                                       //设置MCU外设类型
    u8     GetEqpType( void ) const { return m_bySubType; }
    void   SetEqpId( u8   byEqpId ){ m_byMainType = TYPE_MCUPERI_VER36; m_byEqpId = byEqpId; }
    u8     GetEqpId( void ) const { return m_byEqpId; }
    
    //操作终端函数
    void   SetMt( u8   byMcuId, u8   byMtId, u8   m_byDriId = 0, u8   m_byConfIdx = 0);
    void   SetMt( TMtVer36 tMt);
    void   SetMtId( u8   byMtId ){ m_byMainType = TYPE_MT_VER36; m_byEqpId = byMtId; }
    u8     GetMtId( void ) const { return m_byEqpId; }//设置终端
    void   SetMtType( u8   byMtType ) { m_byMainType = TYPE_MT_VER36; m_bySubType = byMtType; }
    u8     GetMtType( void ) const { return m_bySubType; }  	
    
	void   SetDriId(u8   byDriId){ m_byConfDriId = byDriId; }
    u8     GetDriId( void ) const { return m_byConfDriId; }
    void   SetConfIdx(u8   byConfIdx){ m_byConfIdx = byConfIdx; } 
    u8     GetConfIdx( void ) const { return m_byConfIdx; }
    void   SetNull( void ){ m_byMcuId = 0; m_byEqpId = 0; m_byConfDriId = 0; }
    BOOL32 IsNull( void ){ return m_byMcuId == 0 && m_byEqpId == 0; }
    BOOL32 IsLocal() const {	return (m_byMcuId ==LOCAL_MCUID_VER36 );	}

}PACKED;

//终端轮询参数 (len:8)
struct TMtPollParamVer36: public TMtVer36
{
protected:
    u16   m_wKeepTime;        //终端轮询的保留时间 单位:秒(s)
public:
    void  SetKeepTime(u16  wKeepTime){ m_wKeepTime = htons(wKeepTime);} 
    u16   GetKeepTime( void ) const { return ntohs(m_wKeepTime); }
    void  SetTMt( TMtVer36 tMt ){ memcpy( this, &tMt, sizeof(tMt) ); } 
	TMtVer36 GetTMt( void ) const { return *(TMtVer36 *)this; }
}PACKED;

//轮询信息
struct TPollInfoVer36
{
protected:
	u8   m_byMediaMode;          //轮询模式 MODE_VIDEO 图像  MODE_BOTH 语音图像
    u8   m_byPollState;          //轮询状态，参见轮询状态定义
	TMtPollParamVer36 m_tMtPollParam; //当前被轮询广播的终端及其参数
public:
    void   SetMediaMode(u8   byMediaMode){ m_byMediaMode = byMediaMode;} 
    u8     GetMediaMode(){ return m_byMediaMode; }
	void   SetPollState(u8   byPollState){ m_byPollState = byPollState;} 
    u8     GetPollState( void ) const { return m_byPollState; }
    void   SetMtPollParam(TMtPollParamVer36 tMtPollParam){ m_tMtPollParam = tMtPollParam;} 
    TMtPollParamVer36  GetMtPollParam(){ return m_tMtPollParam; }
	void Print( void )
	{
		StaticLog( "\nPollInfo:\n" );
		StaticLog( "m_byMediaMode: %d\n", m_byMediaMode );
		StaticLog( "m_byPollState: %d\n", m_byPollState );
		StaticLog( "CurPollMtId: %d\n", m_tMtPollParam.GetMtId() );
	}
}PACKED;

//定义终端别名枚举类型
typedef enum
{
	mtAliasTypeBegin_ver36 = 0,
	mtAliasTypeTransportAddress_ver36 = 1,  //终端别名类型: 传输地址
	mtAliasTypeE164_ver36,					//终端别名类型: E164号码
	mtAliasTypeH323ID_ver36,				//终端别名类型: H323 ID
	mtAliasTypeH320ID_ver36,				//终端别名类型: H320 ID类型
	mtAliasTypeOthers_ver36					//终端别名类型: 其它
} mtAliasType_ver36;

//定义传输地址结构(len:6)
struct TTransportAddrVer36
{
public:
    u32   		m_dwIpAddr;		//IP地址, 网络序
    u16 		m_wPort;		//端口号, 网络序
public:
    void   SetNetSeqIpAddr(u32    dwIpAddr){ m_dwIpAddr = dwIpAddr; } 
    u32    GetNetSeqIpAddr( void ) const { return m_dwIpAddr; }
    void   SetIpAddr(u32    dwIpAddr){ m_dwIpAddr = htonl(dwIpAddr); } 
    u32    GetIpAddr( void ) const { return ntohl(m_dwIpAddr); }
    void   SetPort(u16  wPort){ m_wPort = htons(wPort); } 
    u16    GetPort( void ) const { return ntohs(m_wPort); }

}PACKED;

//定义终端别名结构(len:135)
struct TMtAliasVer36
{
public:
    u8				    m_AliasType;					//别名类型
    s8			        m_achAlias[MAXLEN_ALIAS_VER36];	//别名字符串
    TTransportAddrVer36	m_tTransportAddr;				//传输地址
public:
	TMtAliasVer36( void ){ memset( this, 0, sizeof(TMtAliasVer36) ); }
	BOOL32 operator ==( const TMtAliasVer36 & tObj ) const;
	BOOL32 IsNull( void ){ if( m_AliasType == 0 )return TRUE; return FALSE; }
	void   SetNull( void ){ memset(this, 0, sizeof(TMtAliasVer36)); }
	void   SetE164Alias( LPCSTR lpzAlias )
	{
		if( lpzAlias != NULL )
		{
			m_AliasType = mtAliasTypeE164_ver36;
			memcpy( m_achAlias, lpzAlias, MAXLEN_ALIAS_VER36 );
			m_achAlias[MAXLEN_ALIAS_VER36-1] = '\0';
		}
	}
	void SetH323Alias( LPCSTR lpzAlias )
	{
		if( lpzAlias != NULL )
		{
			m_AliasType = mtAliasTypeH323ID_ver36;
			memcpy( m_achAlias, lpzAlias, MAXLEN_ALIAS_VER36 );
			m_achAlias[MAXLEN_ALIAS_VER36-1] = '\0';
		}
	}
	void SetH320Alias( u8 byLayer, u8 bySlot, u8 byChannel )
	{
		m_AliasType = mtAliasTypeH320ID_ver36;
		sprintf( m_achAlias, "层%d-槽%d-通道%d%c", byLayer, bySlot, byChannel, 0 );
	}
	BOOL32 GetH320Alias( u8 &byLayer, u8 &bySlot, u8 &byChannel )
	{
		BOOL32 bRet = FALSE;
		byLayer   = 0;
		bySlot    = 0; 
		byChannel = 0; 
		if( mtAliasTypeH320ID_ver36 != m_AliasType )
		{
			return bRet;
		}

		s8 *pachLayerPrefix   = "层";
		s8 *pachSlotPrefix    = "-槽";
		s8 *pachChannelPrefix = "-通道";
		s8 *pachAlias = m_achAlias;

		if( 0 == memcmp( pachAlias, pachLayerPrefix, strlen("层") ) )
		{
			s32  nPos = 0;
			s32  nMaxCHLen = 3; //一字节的最大字串长度
			s8 achLayer[4] = {0};

			//提取层号
			pachAlias += strlen("层");
			for( nPos = 0; nPos < nMaxCHLen; nPos++ )
			{
				if( '-' == pachAlias[nPos] )
				{
					break;
				}
				achLayer[nPos] = pachAlias[nPos];
			}
			if( 0 == nPos || nPos >= nMaxCHLen )
			{
				return bRet;
			}
			achLayer[nPos+1] = '\0';
			byLayer = atoi(achLayer);

			//提取槽号
			pachAlias += nPos;
			if( 0 == memcmp( pachAlias, pachSlotPrefix, strlen("-槽") ) )
			{
				pachAlias += strlen("-槽");
				for( nPos = 0; nPos < nMaxCHLen; nPos++ )
				{
					if( '-' == pachAlias[nPos] )
					{
						break;
					}
					achLayer[nPos] = pachAlias[nPos];
				}
				if( 0 == nPos || nPos >= nMaxCHLen )
				{
					return bRet;
				}
				achLayer[nPos+1] = '\0';
				bySlot = atoi(achLayer);
			
				//提取通道号
				pachAlias += nPos;
				if( 0 == memcmp( pachAlias, pachChannelPrefix, strlen("-通道") ) )
				{
					pachAlias += strlen("-通道");
					memcpy( achLayer, pachAlias, nMaxCHLen );
					achLayer[nMaxCHLen] = '\0';
					byChannel = atoi(achLayer);
					bRet = TRUE;
				}
			}
		}
		return bRet;
	}
	void Print( void )
	{
		if( m_AliasType == 0 )
		{
			StaticLog( "null aliase!" );
		}
		else if( m_AliasType == mtAliasTypeTransportAddress_ver36 )
		{
			StaticLog( "IP:%u.%u.%u.%u, port:%d", 
				     QUADADDR(m_tTransportAddr.GetIpAddr()), m_tTransportAddr.GetPort() );
		}		
		else if( m_AliasType == mtAliasTypeE164_ver36 )
		{
			StaticLog( "E164: %s", m_achAlias ); 
		}
		else if( m_AliasType == mtAliasTypeH323ID_ver36 )
		{
			StaticLog( "H323ID: %s", m_achAlias ); 
		}
		else if( m_AliasType == mtAliasTypeH320ID_ver36 )
		{
			StaticLog( "H320ID: %s", m_achAlias ); 
		}
		else
		{
			StaticLog( "Other type aliase!" ); 
		}
		StaticLog( "\n" );
	}
}PACKED;

//定义混音模式
typedef enum
{
    mcuNoMix_VER36 = 0,
    mcuWholeMix_VER36,
    mcuPartMix_VER36

} emMcuMixMode_VER36;

//定义讨论参数结构
struct TDiscussParamVer36
{
public:
    u8        m_byMixMode;       //混音模式：定制混音、全体混音、不混音
    u8        m_byMemberNum;     //参加讨论的成员数量(混音深度)
    TMtVer36  m_atMtMember[MAXNUM_MIXER_DEPTH_VER36];  //讨论成员
public:
    TDiscussParamVer36( void )
    { 
        memset( this, 0, sizeof(TDiscussParamVer36) );
    }
    BOOL32    IsMtInMember( TMtVer36 tMt )
    {
        u8 byLoop = 0;
        u8 byLoopCtrl = 0;
        while( byLoop < m_byMemberNum && byLoopCtrl < MAXNUM_MIXER_DEPTH_VER36 )
        {
            if( tMt == m_atMtMember[byLoop] )
            {
                return TRUE;
            }
            
            byLoop++;
            byLoopCtrl++;
        }
        
        return FALSE;
    }
}PACKED;

//定义画面合成成员结构 (len:7)
struct TVMPMemberVer36: public TMtVer36
{
protected:
    u8   m_byMemberType;     //画面合成成员类型,参见mcuconst.h中画面合成成员类型定义
public:
    void  SetMemberType(u8   byMemberType){ m_byMemberType = byMemberType;} 
    u8    GetMemberType( void ) const { return m_byMemberType; }
    void  SetMemberTMt( TMtVer36 tMt ){ memcpy( this, &tMt, sizeof(TMtVer36) ); }
    
}PACKED;

//定义画面合成参数结构 (len:121)
struct TVMPParamVer36
{
protected:
	u8      m_byVMPAuto;      //是否是自动画面合成 0-否 1-是
	u8      m_byVMPBrdst;     //合成图像是否向终端广播 0-否 1-是 
	u8      m_byVMPStyle;     //画面合成风格,参见mcuconst.h中画面合成风格定义
	TVMPMemberVer36  m_atVMPMember[MAXNUM_VMP_MEMBER_VER36]; //画面合成成员
public:
    void   SetVMPAuto(u8   byVMPAuto){ m_byVMPAuto = byVMPAuto;} 
    BOOL32 IsVMPAuto( void ) const { return m_byVMPAuto == 0 ? FALSE : TRUE; }
    void   SetVMPBrdst(u8   byVMPBrdst){ m_byVMPBrdst = byVMPBrdst;} 
    BOOL32 IsVMPBrdst( void ) const { return m_byVMPBrdst == 0 ? FALSE : TRUE; }
    void   SetVMPStyle(u8   byVMPStyle){ m_byVMPStyle = byVMPStyle;} 
    u8     GetVMPStyle( void ) const { return m_byVMPStyle; }
    u8     GetMaxMemberNum( void ) const 
	{
		u8   byMaxMemNum = 1;

		switch( m_byVMPStyle ) 
		{
		case VMP_STYLE_ONE_VER36:
			byMaxMemNum = 1;
			break;
		case VMP_STYLE_VTWO_VER36:
		case VMP_STYLE_HTWO_VER36:
			byMaxMemNum = 2;
			break;
		case VMP_STYLE_THREE_VER36:
			byMaxMemNum = 3;
			break;
		case VMP_STYLE_FOUR_VER36:
			byMaxMemNum = 4;
			break;
		case VMP_STYLE_SIX_VER36:
			byMaxMemNum = 6;
			break;
		case VMP_STYLE_EIGHT_VER36:
			byMaxMemNum = 8;
			break;
		case VMP_STYLE_NINE_VER36:
			byMaxMemNum = 9;
			break;
		case VMP_STYLE_TEN_VER36:
			byMaxMemNum = 10;
			break;
		case VMP_STYLE_THIRTEEN_VER36:
			byMaxMemNum = 13;
			break;
		case VMP_STYLE_SIXTEEN_VER36:
			byMaxMemNum = 16;
			break;
		case VMP_STYLE_SPECFOUR_VER36:
			byMaxMemNum = 4;
			break;
		case VMP_STYLE_SEVEN_VER36:
			byMaxMemNum = 7;
			break;
		default:
			byMaxMemNum = 1;
			break;
		}
		return byMaxMemNum;
	}
	void   SetVmpMember( u8   byMemberId, TVMPMemberVer36 tVMPMember )
	{
		if( byMemberId >= MAXNUM_VMP_MEMBER_VER36 )return;
		m_atVMPMember[byMemberId] = tVMPMember;
	}
	void   ClearVmpMember( u8 byMemberId )
	{
		if( byMemberId >= MAXNUM_VMP_MEMBER_VER36 )return;
		m_atVMPMember[byMemberId].SetNull();
	}
    TVMPMemberVer36 *GetVmpMember( u8   byMemberId )
	{
		if( byMemberId >= MAXNUM_VMP_MEMBER_VER36 )return NULL;
		return &m_atVMPMember[byMemberId];
	}
	BOOL32 IsMtInMember( TMtVer36 tMt )
	{
		u8 byLoop = 0;
		u8 byLoopCtrl = 0;
		while( byLoop < GetMaxMemberNum() && byLoopCtrl < MAXNUM_VMP_MEMBER_VER36 )
		{
			if( tMt.GetMtId() == m_atVMPMember[byLoop].GetMtId() && 
				tMt.GetMcuId() == m_atVMPMember[byLoop].GetMcuId() )
			{
				return TRUE;
			}

			byLoop++;
			byLoopCtrl++;
		}

		return FALSE;
	}
	u8 GetChlOfMtInMember( TMtVer36 tMt )
	{
		u8 byLoop = 0;
		while( byLoop < GetMaxMemberNum() )
		{
			if( tMt.GetMtId() == m_atVMPMember[byLoop].GetMtId() )
			{
				return byLoop;
			}

			byLoop++;
		}

		return MAXNUM_VMP_MEMBER_VER36;
	}
	void   Print( void )
	{
		StaticLog( "\nVMPParam:\n" );
		StaticLog( "m_byVMPAuto: %d\n", m_byVMPAuto);
		StaticLog( "m_byVMPBrdst: %d\n", m_byVMPBrdst);
		StaticLog( "m_byVMPStyle: %d\n", m_byVMPStyle);
		for( s32 i=0;i<GetMaxMemberNum();i++ )
		{
			StaticLog( "VMP member %d: MtId-%d, Type-%d\n", 
				i,  m_atVMPMember[i].GetMtId(), m_atVMPMember[i].GetMemberType() );
		}
	}
}PACKED;

//会议号结构 (len:16)
struct CConfIdVer36
{
protected:
	u8  	m_abyConfId[16];

public:
	u8		CConfIdVer36::GetConfId( u8   * pbyConfId, u8   byBufLen ) const; //获得会议号数组
	void	CConfIdVer36::CreateConfId( void );							//创建会议号
	void	GetConfIdString( LPSTR lpszConfId, u8   byBufLen ) const;	//获得会议号字符串
	void	SetConfId( const CConfIdVer36 & cConfId );						//设置会议号
	u8		SetConfId( const u8   * pbyConfId, u8   byBufLen );			//设置会议号数组
	u8		SetConfId( LPCSTR lpszConfId );								//设置会议号数组	
	BOOL32	IsNull( void ) const;										//判断会议号是否为空
	void	SetNull( void );											//设置会议号为空
	BOOL32	operator == ( const CConfIdVer36 & cConfId ) const;			//操作符重载
	void	Print( void )
	{
		s8 achBuf[33];
		achBuf[32] = 0;
		GetConfIdString( achBuf, 33 );
		StaticLog( "%s", achBuf);
	}
}PACKED;

//定义会议状态结构
struct TConfStatusVer36
{
public:
	TMtVer36 			m_tChairman;	    //主席终端，MCU号为0表示无主席
	TMtVer36		    m_tSpeaker;			//发言终端，MCU号为0表示无发言人
	TConfModeVer36		m_tConfMode;        //会议方式
	TRecProgVer36		m_tRecProg;			//当前录像进程,仅录像状态时有效
	TRecProgVer36		m_tPlayProg;	    //当前放像进程,仅放像状态时有效
	TPollInfoVer36		m_tPollInfo;        //会议轮询参数,仅轮询时有较
	TDiscussParamVer36	m_tDiscussParam;    //当前混音参数，仅混音时有效
	TVMPParamVer36		m_tVMPParam;        //当前视频复合参数，仅视频复合时有效
	u8					m_byPrs;            //当前包重传状态
	
public:
    BOOL32   HasChairman( void ) const{ return m_tChairman.GetMtId()==0 ? FALSE : TRUE; }	
	TMtVer36 GetChairman( void ) const{ return m_tChairman; }
	void	 SetNoChairman( void ){ memset( &m_tChairman, 0, sizeof( TMtVer36 ) ); }
	void	 SetChairman( TMtVer36 tChairman ){ m_tChairman = tChairman; }
	BOOL32   HasSpeaker( void ) const{ return m_tSpeaker.GetMtId()==0 ? FALSE : TRUE; }
    TMtVer36 GetSpeaker( void ) const{ return m_tSpeaker; }
	void	 SetNoSpeaker( void ){ memset( &m_tSpeaker, 0, sizeof( TMtVer36 ) ); }
	void	 SetSpeaker( TMtVer36 tSpeaker ){ m_tSpeaker = tSpeaker; }
    void           SetConfMode(TConfModeVer36 tConfMode){ m_tConfMode = tConfMode;} 
    TConfModeVer36 GetConfMode( void ) const { return m_tConfMode; }
    void           SetRecProg(TRecProgVer36 tRecProg){ m_tRecProg = tRecProg;} 
    TRecProgVer36  GetRecProg( void ) const { return m_tRecProg; }
    void           SetPlayProg(TRecProgVer36 tPlayProg){ m_tPlayProg = tPlayProg;} 
    TRecProgVer36  GetPlayProg( void ) const { return m_tPlayProg; }
	void           SetDiscussMember(TDiscussParamVer36 tAudMixParam)
	{ memcpy( m_tDiscussParam.m_atMtMember, tAudMixParam.m_atMtMember, sizeof(tAudMixParam.m_atMtMember) ); } 
    void			   SetDiscussParam(TDiscussParamVer36 tAudMixParam){ m_tDiscussParam = tAudMixParam;} 
    TDiscussParamVer36 GetDiscussParam( void ) const { return m_tDiscussParam; }
    void		   SetVmpParam(TVMPParamVer36 tVMPParam){ m_tVMPParam = tVMPParam;} 
    TVMPParamVer36 GetVmpParam( void ) const { return m_tVMPParam; } 
	//会议进行状态
	BOOL32	IsScheduled( void ) const { return m_tConfMode.GetTakeMode() == CONF_TAKEMODE_SCHEDULED_VER36 ? TRUE : FALSE; }
	void	SetScheduled( void ){ m_tConfMode.SetTakeMode( CONF_TAKEMODE_SCHEDULED_VER36 ); }
	BOOL32	IsOngoing( void ) const { return m_tConfMode.GetTakeMode() == CONF_TAKEMODE_ONGOING_VER36 ? TRUE : FALSE; }
	void	SetOngoing( void ){ m_tConfMode.SetTakeMode( CONF_TAKEMODE_ONGOING_VER36 ); }
	BOOL32	IsTemplate( void ) const { return m_tConfMode.GetTakeMode() == CONF_TAKEMODE_TEMPLATE_VER36 ? TRUE : FALSE; }
	void	SetTemplate( void ){ m_tConfMode.SetTakeMode( CONF_TAKEMODE_TEMPLATE_VER36 ); }
	u8		GetTakeMode( void ){ return m_tConfMode.GetTakeMode();	}
	void	SetTakeMode( u8 byTakeMode ){ m_tConfMode.SetTakeMode( byTakeMode ); }

	//会议保护状态
	u8   GetProtectMode( void ) const { return m_tConfMode.GetLockMode(); }
    void SetProtectMode( u8   byProtectMode ) { m_tConfMode.SetLockMode( byProtectMode ); }
	
	//会议呼叫策略
	u8   GetCallMode( void ) const { return m_tConfMode.GetCallMode(); }
	void SetCallMode( u8   byCallMode ){ m_tConfMode.SetCallMode( byCallMode ); }
	void SetCallInterval(u32 dwCallInterval){ m_tConfMode.SetCallInterval( dwCallInterval ); } 
    u32  GetCallInterval( void ) const { return m_tConfMode.GetCallInterval( ); }
	void SetCallTimes(u32 dwCallTimes){ m_tConfMode.SetCallTimes( dwCallTimes ); } 
    u32  GetCallTimes( void ) const { return m_tConfMode.GetCallTimes( ); }

	//语音激励方式
	BOOL32	IsVACMode( void ) const { return m_tConfMode.IsVACMode(); }
	void	SetVACMode( BOOL32 bVACMode = TRUE ) { m_tConfMode.SetVACMode( bVACMode ); }

	//会议混音状态
	BOOL32	IsNoMixing( void ) const	{ return !m_tConfMode.IsAudioMixMode(); }
	void	SetNoMixing( void )	{ m_tConfMode.SetAudioMixMode( 0 ); }
	BOOL32	IsMixing( void ) const	{ return m_tConfMode.IsAudioMixMode(); }
	void	SetMixing( void )	{ m_tConfMode.SetAudioMixMode( 1 ); }

	//会议讨论方式
	BOOL32	IsDiscussMode( void ) const { return m_tConfMode.IsDiscussMode(); }
	void	SetDiscussMode( BOOL32 bDiscussMode = TRUE ) { m_tConfMode.SetDiscussMode( bDiscussMode ); }

    //画面合成方式
	void	SetVMPMode(u8   byVMPMode){ m_tConfMode.SetVMPMode( byVMPMode ); } 
    u8		GetVMPMode( void ) const { return m_tConfMode.GetVMPMode(); }
	BOOL32  IsBrdstVMP( void ) const { return m_tConfMode.GetVMPMode() != CONF_VMPMODE_NONE_VER36 && m_tVMPParam.IsVMPBrdst(); }
	void	SetVmpBrdst( BOOL32 bBrdst ){ m_tVMPParam.SetVMPBrdst( bBrdst ); }
	u8		GetVmpStyle( void ){ return m_tVMPParam.GetVMPStyle(); }
	void	SetVmpStyle( u8 byVMPStyle ){ m_tVMPParam.SetVMPStyle( byVMPStyle ); }

	//强制广播发言人
	BOOL32	IsMustSeeSpeaker( void ) const { return m_tConfMode.IsForceRcvSpeaker(); }
	void	SetMustSeeSpeaker( BOOL32 bMustSeeSpeaker ) { m_tConfMode.SetForceRcvSpeaker( bMustSeeSpeaker ); }

	//码率适配状态
	BOOL32	IsRtpAdapting( void ) const { return m_tConfMode.GetBasMode( CONF_BASMODE_RTP_VER36 ); }
	BOOL32	IsBrAdapting( void ) const { return m_tConfMode.GetBasMode( CONF_BASMODE_BR_VER36 ); }
	BOOL32	IsMdtpAdapting( void ) const { return m_tConfMode.GetBasMode( CONF_BASMODE_MDTP_VER36 ); }
	void SetAdaptMode( u8 byBasMode, BOOL32 bAdapt ) 
	{ 
		m_tConfMode.SetBasMode( byBasMode, bAdapt );
	}

	//会议主席状态
	BOOL32 IsNoChairMode( void ) const { return m_tConfMode.IsNoChairMode(); }
    void   SetNoChairMode( BOOL32 bNoChairMode ) { m_tConfMode.SetNoChairMode( bNoChairMode ); }

	//轮询方式
	void   SetPollMode(u8   byPollMode){ m_tConfMode.SetPollMode( byPollMode ); } 
    u8     GetPollMode(){ return m_tConfMode.GetPollMode(); }
	
	//轮询参数
    void   SetPollInfo(TPollInfoVer36 tPollInfo){ m_tPollInfo = tPollInfo;} 
    TPollInfoVer36  *GetPollInfo( void ) { return &m_tPollInfo; }
	void   SetPollMedia(u8   byMediaMode){ m_tPollInfo.SetMediaMode( byMediaMode ); } 
    u8     GetPollMedia( void ){ return m_tPollInfo.GetMediaMode(); }
	void   SetPollState(u8   byPollState){ m_tPollInfo.SetPollState( byPollState );} 
    u8     GetPollState( void ) const { return m_tPollInfo.GetPollState(); }
    void   SetMtPollParam(TMtPollParamVer36 tMtPollParam){ m_tPollInfo.SetMtPollParam( tMtPollParam ); } 
    TMtPollParamVer36  GetMtPollParam(){ return m_tPollInfo.GetMtPollParam(); }

	//会议录像状态	
	BOOL32 IsNoRecording( void ) const{ return m_tConfMode.GetRecordMode() == CONF_RECMODE_NONE_VER36 ? TRUE : FALSE; }
	void   SetNoRecording( void ){ m_tConfMode.SetRecordMode( CONF_RECMODE_NONE_VER36 ); }
	BOOL32 IsRecording( void ) const{ return m_tConfMode.GetRecordMode() == CONF_RECMODE_REC_VER36 ? TRUE : FALSE; }
	void   SetRecording( void ){ m_tConfMode.SetRecordMode( CONF_RECMODE_REC_VER36 ); }
	BOOL32 IsRecPause( void ) const{ return m_tConfMode.GetRecordMode() == CONF_RECMODE_PAUSE_VER36 ? TRUE : FALSE; }
	void   SetRecPause( void ){ m_tConfMode.SetRecordMode( CONF_RECMODE_PAUSE_VER36 ); }

	//会议放像状态
	BOOL32 IsNoPlaying( void ) const{ return m_tConfMode.GetPlayMode() == CONF_PLAYMODE_NONE_VER36 ? TRUE : FALSE; }
	void   SetNoPlaying( void ){ m_tConfMode.SetPlayMode( CONF_PLAYMODE_NONE_VER36 ); }
	BOOL32 IsPlaying( void ) const { return m_tConfMode.GetPlayMode() == CONF_PLAYMODE_PLAY_VER36 ? TRUE : FALSE; }
	void   SetPlaying( void ){ m_tConfMode.SetPlayMode( CONF_PLAYMODE_PLAY_VER36 ); }
	BOOL32 IsPlayPause( void )const { return m_tConfMode.GetPlayMode() == CONF_PLAYMODE_PAUSE_VER36 ? TRUE : FALSE; }
	void   SetPlayPause( void ){ m_tConfMode.SetPlayMode( CONF_PLAYMODE_PAUSE_VER36 ); }
	BOOL32 IsPlayFF( void )const { return m_tConfMode.GetPlayMode() == CONF_PLAYMODE_FF_VER36 ? TRUE : FALSE; }
	void   SetPlayFF( void ) { m_tConfMode.SetPlayMode( CONF_PLAYMODE_FF_VER36 ); }
	BOOL32 IsPlayFB( void )const { return m_tConfMode.GetPlayMode() == CONF_PLAYMODE_FB_VER36 ? TRUE : FALSE; }
	void   SetPlayFB( void ){ m_tConfMode.SetPlayMode( CONF_PLAYMODE_FB_VER36 ); }

	//包重传状态
	void   SetPrsing( BOOL32 bPrs ){ m_byPrs = bPrs; }
	BOOL32 IsPrsing( void ){ return m_byPrs; }

	//注册GK情况
	void   SetRegToGK( BOOL32 bRegToGK ){ m_tConfMode.SetRegToGK( bRegToGK ); }
	BOOL32 IsRegToGK( void ){ return m_tConfMode.IsRegToGK(); }

	//混音方式 
    BOOL32 IsMixSpecMt( void ) const { return m_tConfMode.IsMixSpecMt(); }
	void   SetMixSpecMt( BOOL32 bMixSpecMt ){ m_tConfMode.SetMixSpecMt(bMixSpecMt); }

	void Print( void )
	{
		StaticLog( "\nConfStatus:\n" );
		StaticLog( "Chairman: Mcu%dMt%d\n", m_tChairman.GetMcuId(), m_tChairman.GetMtId() );
		StaticLog( "Speaker: Mcu%dMt%d\n", m_tSpeaker.GetMcuId(), m_tSpeaker.GetMtId() );
	}
}PACKED;

//同时能力集结构(len:4)
struct TSimCapSetVer36
{
protected:
	u8  m_byVideoMediaType;
	u8  m_byAudioMediaType;
	u8  m_byDataMediaType1;
	u8  m_byDataMediaType2;
public:
	TSimCapSetVer36( void )
	{
		m_byVideoMediaType = MEDIA_TYPE_NULL_VER36;
		m_byAudioMediaType = MEDIA_TYPE_NULL_VER36;
		m_byDataMediaType1 = MEDIA_TYPE_NULL_VER36;
		m_byDataMediaType2 = MEDIA_TYPE_NULL_VER36;
	}

    void SetVideoMediaType( u8 byVideoMediaType ){ m_byVideoMediaType = byVideoMediaType;} 
    u8   GetVideoMediaType( void ) const { return m_byVideoMediaType; }
    void SetAudioMediaType( u8 byAudioMediaType ){ m_byAudioMediaType = byAudioMediaType;} 
    u8   GetAudioMediaType( void ) const { return m_byAudioMediaType; }
    void SetDataMediaType1( u8 byDataMediaType1 ){ m_byDataMediaType1 = byDataMediaType1;} 
    u8   GetDataMediaType1( void ) const { return m_byDataMediaType1; }
    void SetDataMediaType2( u8 byDataMediaType2 ){ m_byDataMediaType2 = byDataMediaType2;} 
    u8   GetDataMediaType2( void ) const { return m_byDataMediaType2; }

	BOOL32   operator ==( const TSimCapSetVer36 & tSimCapSet ) const  //判断是否相等(只判断语音图像)
	{
		if( m_byVideoMediaType == tSimCapSet.GetVideoMediaType() && 
			m_byAudioMediaType == tSimCapSet.GetAudioMediaType() && 
			m_byVideoMediaType != MEDIA_TYPE_NULL_VER36 && 
			m_byAudioMediaType != MEDIA_TYPE_NULL_VER36 )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	BOOL32   IsLike(  const TSimCapSetVer36 & tSimCapSet )  const  //判断是否相似(只判断语音图像)
	{
		if( *this == tSimCapSet )
		{
			return TRUE;
		}
		if( ( m_byVideoMediaType == MEDIA_TYPE_NULL_VER36 && m_byAudioMediaType != MEDIA_TYPE_NULL_VER36 && 
			  m_byAudioMediaType == tSimCapSet.GetAudioMediaType() ) || 
			( m_byAudioMediaType == MEDIA_TYPE_NULL_VER36 && m_byVideoMediaType != MEDIA_TYPE_NULL_VER36 && 
			  m_byVideoMediaType == tSimCapSet.GetVideoMediaType() ) || 
			( tSimCapSet.GetAudioMediaType() == MEDIA_TYPE_NULL_VER36 && tSimCapSet.GetVideoMediaType() != MEDIA_TYPE_NULL_VER36 && 
			  tSimCapSet.GetVideoMediaType() == m_byVideoMediaType ) || 
			( tSimCapSet.GetVideoMediaType() == MEDIA_TYPE_NULL_VER36 && tSimCapSet.GetAudioMediaType() != MEDIA_TYPE_NULL_VER36 && 
			  tSimCapSet.GetAudioMediaType() == m_byAudioMediaType ) )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	BOOL32   IsNull( void )
	{
		if(	m_byVideoMediaType == MEDIA_TYPE_NULL_VER36 && m_byAudioMediaType == MEDIA_TYPE_NULL_VER36 )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	void  SetNull( void )
	{
		m_byVideoMediaType = MEDIA_TYPE_NULL_VER36;
		m_byAudioMediaType = MEDIA_TYPE_NULL_VER36;
		m_byDataMediaType1 = MEDIA_TYPE_NULL_VER36;
		m_byDataMediaType2 = MEDIA_TYPE_NULL_VER36;
	}
}PACKED;

//能力集结构(len:27)
struct TCapSupportVer36
{
protected:
	//音频
	u8   m_bySupportG711A;	//是否支持g711alaw
	u8   m_bySupportG711U;	//是否支持g911ulaw
	u8   m_bySupportG722;   //是否支持g722
	u8   m_bySupportG7231;	//是否支持g7231
	u8   m_bySupportG728;	//是否支持g728
	u8   m_bySupportG729;   //是否支持g729
	u8   m_bySupportMP3;	//是否支持mp3
	//视频
	u8   m_bySupportH261;	//是否支持h261
	u8   m_bySupportH262;	//是否支持h262
	u8   m_bySupportH263;	//是否支持h263
	u8   m_bySupportH263Plus;	//是否支持h263+ 双流
	u8   m_bySupportH239;	    //是否支持h239  双流
	u8   m_bySupportH264;	//是否支持h264
	u8   m_bySupportMPEG4;	//是否支持mpeg4
	//数据
	u8   m_bySupportT120;	//是否支持t120
	u8   m_bySupportH224;	//是否支持H224

	u8   m_bySupportMMcu;   //是否支持合并级联

	//同时能力集
	//加密模式CONF_ENCRYPTMODE_NONE_VER36, CONF_ENCRYPTMODE_DES_VER36, CONF_ENCRYPTMODE_AES_VER36
	u8 m_byMainEncryptMode;		  //主的同时能力集加密模式
	u8 m_bySecondEncryptMode;	  //辅的同时能力集加密模式
	TSimCapSetVer36 tMainSimCapSet;    //主的同时能力集
	TSimCapSetVer36 tSecondSimCapSet;  //辅的同时能力集

public:
	//构造函数
	TCapSupportVer36( void ) { memset( this, 0, sizeof( TCapSupportVer36 ) ); tMainSimCapSet.SetNull(); tSecondSimCapSet.SetNull(); }
    void   SetSupportG711A(u8   bySupportG711A){ m_bySupportG711A = bySupportG711A;} 
    u8     GetSupportG711A( void ) const { return m_bySupportG711A; }
    void   SetSupportG711U(u8   bySupportG711U){ m_bySupportG711U = bySupportG711U;} 
    u8     GetSupportG711U( void ) const { return m_bySupportG711U; }
    void   SetSupportG722(u8   bySupportG722){ m_bySupportG722 = bySupportG722;} 
    u8     GetSupportG722( void ) const { return m_bySupportG722; }
    void   SetSupportG7231(u8   bySupportG7231){ m_bySupportG7231 = bySupportG7231;} 
    u8     GetSupportG7231( void ) const { return m_bySupportG7231; }
    void   SetSupportG728(u8   bySupportG728){ m_bySupportG728 = bySupportG728;} 
    u8     GetSupportG728( void ) const { return m_bySupportG728; }
    void   SetSupportG729(u8   bySupportG729){ m_bySupportG729 = bySupportG729;} 
    u8     GetSupportG729( void ) const { return m_bySupportG729; }
    void   SetSupportMP3(u8   bySupportMP3){ m_bySupportMP3 = bySupportMP3;} 
    u8     GetSupportMP3( void ) const { return m_bySupportMP3; } 
    void   SetSupportH261(u8   bySupportH261){ m_bySupportH261 = bySupportH261;} 
    u8     GetSupportH261( void ) const { return m_bySupportH261; }
    void   SetSupportH262(u8   bySupportH262){ m_bySupportH262 = bySupportH262;} 
    u8     GetSupportH262( void ) const { return m_bySupportH262; }
    void   SetSupportH263(u8   bySupportH263){ m_bySupportH263 = bySupportH263;} 
    u8     GetSupportH263( void ) const { return m_bySupportH263; }
    void   SetSupportH263Plus(u8   bySupportH263Plus){ m_bySupportH263Plus = bySupportH263Plus;} 
    u8     GetSupportH263Plus( void ) const { return m_bySupportH263Plus; }
    void   SetSupportH239(u8 bySupportH239){ m_bySupportH239 = bySupportH239;} 
    u8     GetSupportH239( void ) const { return m_bySupportH239; }	
    void   SetSupportH264(u8   bySupportH264){ m_bySupportH264 = bySupportH264;} 
    u8     GetSupportH264( void ) const { return m_bySupportH264; }
    void   SetSupportMPEG4(u8   bySupportMPEG4){ m_bySupportMPEG4 = bySupportMPEG4;} 
    u8     GetSupportMPEG4( void ) const { return m_bySupportMPEG4; }
    void   SetSupportT120(u8   bySupportT120){ m_bySupportT120 = bySupportT120;} 
    u8     GetSupportT120( void ) const { return m_bySupportT120; }
    void   SetSupportH224(u8   bySupportH224){ m_bySupportH224 = bySupportH224;} 
    u8     GetSupportH224( void ) const { return m_bySupportH224; }
	void   SetSupportMMcu( BOOL32 bMMcu) { m_bySupportMMcu = (bMMcu==TRUE?1:0); }
	BOOL32 IsSupportMMcu() const { return (m_bySupportMMcu != 0 ); }

	BOOL32 IsSupportMediaType( u8 byMediaType ); //是否支持某种媒体类型
	void   SetVideoType( u8   byVideoType );//设置视频类型
	void   SetAudioType( u8   byAudioType );//设置音频类型
	u8     GetPriVideoType( void );//得到优先的视频类型
	u8     GetPriAudioType( void );//得到优先的音频类型

	void SetMainEncryptMode(u8 byEncryptMode) {m_byMainEncryptMode = byEncryptMode;}
    u8   GetMainEncryptMode(){return m_byMainEncryptMode;}
	void SetSecondEncryptMode(u8 byEncryptMode) {m_bySecondEncryptMode = byEncryptMode;}
    u8   GetSecondEncryptMode(){return m_bySecondEncryptMode;}
	TSimCapSetVer36 GetMainSimCapSet( void ){ return tMainSimCapSet; }
	TSimCapSetVer36 GetSecondSimCapSet( void ){ return tSecondSimCapSet; }
	void SetMainSimCapSet( TSimCapSetVer36 tSimCapSet )
	{ 
		tMainSimCapSet = tSimCapSet; 
		SetVideoType( tMainSimCapSet.GetVideoMediaType() );
		SetAudioType( tMainSimCapSet.GetAudioMediaType() );
		if( MEDIA_TYPE_T120_VER36 == tMainSimCapSet.GetDataMediaType1() )
		{
			SetSupportT120( TRUE );
		}
		if( MEDIA_TYPE_H224_VER36 == tMainSimCapSet.GetDataMediaType1() )
		{
			SetSupportH224( TRUE );
		}
		if( MEDIA_TYPE_T120_VER36 == tMainSimCapSet.GetDataMediaType2() )
		{
			SetSupportT120( TRUE );
		}
		if( MEDIA_TYPE_H224_VER36 == tMainSimCapSet.GetDataMediaType2() )
		{
			SetSupportH224( TRUE );
		}
	}
	void SetSecondSimCapSet( TSimCapSetVer36 tSimCapSet )
	{ 
		tSecondSimCapSet = tSimCapSet; 
		SetVideoType( tSecondSimCapSet.GetVideoMediaType() );
		SetAudioType( tSecondSimCapSet.GetAudioMediaType() );
		if( MEDIA_TYPE_T120_VER36 == tSecondSimCapSet.GetDataMediaType1() )
		{
			SetSupportT120( TRUE );
		}
		if( MEDIA_TYPE_H224_VER36 == tSecondSimCapSet.GetDataMediaType1() )
		{
			SetSupportH224( TRUE );
		}
		if( MEDIA_TYPE_T120_VER36 == tSecondSimCapSet.GetDataMediaType2() )
		{
			SetSupportT120( TRUE );
		}
		if( MEDIA_TYPE_H224_VER36 == tSecondSimCapSet.GetDataMediaType2() )
		{
			SetSupportH224( TRUE );
		}
	}

	void Clear( void )
	{
		memset( this, 0, sizeof( TCapSupportVer36 ) );
		m_byMainEncryptMode   = CONF_ENCRYPTMODE_NONE_VER36;
		m_bySecondEncryptMode = CONF_ENCRYPTMODE_NONE_VER36;
		tMainSimCapSet.SetNull();
		tSecondSimCapSet.SetNull();
	}
	
	void Print( void )
	{
		StaticLog( "\nvideo support: ");
		if( m_bySupportH261 )StaticLog( "H.261 " );
		if( m_bySupportH262 )StaticLog( "H.262 " );
		if( m_bySupportH263 )StaticLog( "H.263 " );
		if( m_bySupportH264 )StaticLog( "H.264 " );
		if( m_bySupportMPEG4 )StaticLog( "MPEG-4 " );
		StaticLog( "\naudio support: ");
		if( m_bySupportG711A )StaticLog( "G711A " );
		if( m_bySupportG711U )StaticLog( "G711U " );
		if( m_bySupportG722  )StaticLog( "G722 " );
		if( m_bySupportG7231 )StaticLog( "G7231 " );
		if( m_bySupportG728 )StaticLog( "G728 " );
		if( m_bySupportG729 )StaticLog( "G729 " );
		if( m_bySupportMP3  )StaticLog( "MP3 " );
		StaticLog( "\nmain encrypt mode:%d, second encrypt mode:%d", m_byMainEncryptMode, m_bySecondEncryptMode);
		StaticLog( "\nmain simul support:");
		StaticLog( "\n %d ", tMainSimCapSet.GetVideoMediaType() );
		StaticLog( "\n %d ", tMainSimCapSet.GetAudioMediaType() );
		StaticLog( "\nsecond simul support:");
		StaticLog( "\n %d ", tSecondSimCapSet.GetVideoMediaType() );
		StaticLog( "\n %d ", tSecondSimCapSet.GetAudioMediaType() );
	}
}PACKED;

//定义会议属性结构(会议中不会改变)
struct TConfAttrbVer36
{
protected:
    u8     m_byMeetingRoom;     //是否会议室:   0-不是会议室 1-会议室
    u8     m_byOpenMode;        //会议开放方式: 0-不开放,拒绝列表以外的终端 1-根据密码加入 2-完全开放
    u8     m_byEncryptMode;     //会议加密模式: 0-不加密, 1-des加密,2-aes加密
    u8     m_byMulticastMode;   //组播会议方式: 0-不是组播会议 1-组播会议
    u8     m_byDataMode;        //数据会议方式: 0-不包含数据的视音频会议 1-包含数据的视音频会议 2-只有数据的会议
    u8     m_byReleaseMode;     //会议结束方式: 0-不会自动结束 1-无终端时自动结束
    u8     m_byVideoMode;       //会议视频模式: 0-速度优先 1-画质优先
    u8     m_byCascadeMode;     //会议级连方式: 0-不支持合并级联, 1-支持合并级联
    u8     m_byPrsMode;         //丢包重传方式: 0-不重传 1-重
    u8     m_byHasTvWallModule; //电视墙模板:   0-无电视墙模板  1-有电视墙模板
    u8     m_byHasVmpModule;    //画面合成模板: 0-无画面合成模板  1-有画面合成模板
    u8     m_byDiscussConf;     //是否讨论会议: 0-不是讨论会议(演讲会议) 1-讨论会议 (这一字段仅用来控制会议开始后是否启动混音)
    u8     m_byUseAdapter;		//是否启用适配: 0-不启用任何适配 2-按需要进行(码流，码率)适配 
    u8     m_bySpeakerSrc;		//发言人的源:   0-看自己 1-看主席 2-看上一次发言人 
    u8     m_bySpeakerSrcMode;  //发言人的源的方式: 0-无效值 1-仅视频 2-仅音频 3-视音频
    u8     m_byMulcastLowStream;//是否组播低速码流: 0-不是组播低速码流(组播高速码流) 1-组播低速码流
    u8     m_byAllInitDumb;     //终端入会后是否初始哑音 0-不哑音 1-哑音
    u8     m_byUniformMode;     //码流转发时是否支持归一化整理方式: CONF_UNIFORMMODE_NONE-不重整 CONF_UNIFORMMODE_valid-重整，
    //这个处理方式与丢包重传方式互斥
    u8     m_byDStreamType;     //双流格式，目前支持三种类型：VIDEO_DSTREAM_MAIN->与主视频格式一致、
    //VIDEO_DSTREAM_H263PLUS->H263＋、
    //VIDEO_DSTREAM_H239->H239
public:
    TConfAttrbVer36( void )
    { 
        memset( this, 0, sizeof(TConfAttrbVer36) );
        m_byOpenMode = 2;
    }
    void   SetMeetingRoom(BOOL32 bMeetingRoom){ m_byMeetingRoom = bMeetingRoom; } 
    BOOL32 IsMeetingRoom( void ) const { return m_byMeetingRoom == 0 ? FALSE : TRUE; }
    void   SetOpenMode(u8   byOpenMode){ m_byOpenMode = byOpenMode;} 
    u8     GetOpenMode( void ) const { return m_byOpenMode; }
    void   SetEncryptMode(u8   byEncryptMode){ m_byEncryptMode = byEncryptMode;} 
    u8     GetEncryptMode( void ) const { return m_byEncryptMode; }
    void   SetMulticastMode(BOOL32 bMulticastMode){ m_byMulticastMode = bMulticastMode;} 
    BOOL32 IsMulticastMode( void ) const { return m_byMulticastMode == 0 ? FALSE : TRUE; }
    void   SetDataMode(u8   byDataMode){ m_byDataMode = byDataMode;} 
    u8     GetDataMode( void ) const { return m_byDataMode; }
    void   SetReleaseMode(BOOL32 bReleaseMode){ m_byReleaseMode = bReleaseMode;} 
    BOOL32 IsReleaseNoMt( void ) const { return m_byReleaseMode == 0 ? FALSE : TRUE; }
    void   SetQualityPri(BOOL32 bQualityPri){ m_byVideoMode = bQualityPri;} 
    BOOL32 IsQualityPri( void ) const { return m_byVideoMode == 0 ? FALSE : TRUE; }
    void   SetSupportCascade(BOOL32 bCascadeMode){ m_byCascadeMode = (bCascadeMode==TRUE?1:0); }
    BOOL32 IsSupportCascade( void ) const { return (m_byCascadeMode != 0 );}
    void   SetPrsMode( BOOL32 bResendPack ){ m_byPrsMode = bResendPack; }
    BOOL32 IsResendLosePack( void ){ return m_byPrsMode == 0 ? FALSE : TRUE; }
    void   SetHasTvWallModule( BOOL32 bHasTvWallModule ){ m_byHasTvWallModule = bHasTvWallModule; }
    BOOL32 IsHasTvWallModule( void ){ return m_byHasTvWallModule == 0 ? FALSE : TRUE; }
    void   SetHasVmpModule( BOOL32 bHasVmpModule ){ m_byHasVmpModule = bHasVmpModule; }
    BOOL32 IsHasVmpModule( void ){ return m_byHasVmpModule == 0 ? FALSE : TRUE; }
    void   SetDiscussConf( BOOL32 bDiscussConf ){ m_byDiscussConf = bDiscussConf; }
    BOOL32 IsDiscussConf( void ){ return m_byDiscussConf == 0 ? FALSE : TRUE; }
    void   SetUseAdapter( BOOL32 bUseAdapter ){ m_byUseAdapter = bUseAdapter; } 
    BOOL32 IsUseAdapter( void ) const { return m_byUseAdapter == 0 ? FALSE : TRUE; }
    void   SetSpeakerSrc( u8 bySpeakerSrc ){ m_bySpeakerSrc = bySpeakerSrc;} 
    u8     GetSpeakerSrc( void ) const { return m_bySpeakerSrc; }
    void   SetSpeakerSrcMode( u8 bySpeakerSrcMode ){ m_bySpeakerSrcMode = bySpeakerSrcMode;} 
    u8     GetSpeakerSrcMode( void ) const { return m_bySpeakerSrcMode; }
    void   SetMulcastLowStream( BOOL32 bMulcastLowStream ){ m_byMulcastLowStream = bMulcastLowStream;} 
    BOOL32 IsMulcastLowStream( void ) const { return m_byMulcastLowStream == 0 ? FALSE : TRUE; }
    void   SetAllInitDumb( BOOL32 bAllInitDumb ){ m_byAllInitDumb = bAllInitDumb;} 
    BOOL32 IsAllInitDumb( void ) const { return m_byAllInitDumb == 0 ? FALSE : TRUE; }
    void   SetUniformMode( BOOL32 bUniformMode ){ m_byUniformMode = bUniformMode; }
    u8     IsAdjustUniformPack( void ){ return m_byUniformMode; }
    void   SetDStreamType( u8 byDStreamType ){ m_byDStreamType = byDStreamType; }
    u8     GetDStreamType( void ){ return m_byDStreamType; }
    
    void   Print( void )
    {
        StaticLog( "\nConfAttrb:\n" );
        StaticLog( "m_byMeetingRoom: %d\n", m_byMeetingRoom);
        StaticLog( "m_byOpenMode: %d\n", m_byOpenMode);
        StaticLog( "m_byEncryptMode: %d\n", m_byEncryptMode);
        StaticLog( "m_byMulticastMode: %d\n", m_byMulticastMode);
        StaticLog( "m_byDataMode: %d\n", m_byDataMode);
        StaticLog( "m_byReleaseMode: %d\n", m_byReleaseMode);
        StaticLog( "m_byVideoMode: %d\n", m_byVideoMode);
        StaticLog( "m_byCascadeMode: %d\n", m_byCascadeMode);
        StaticLog( "m_byPrsMode: %d\n", m_byPrsMode);
        StaticLog( "m_byHasTvWallModule: %d\n", m_byHasTvWallModule);
        StaticLog( "m_byHasVmpModule: %d\n", m_byHasVmpModule);	
        StaticLog( "m_byDiscussConf: %d\n", m_byDiscussConf);
        StaticLog( "m_byUseAdapter: %d\n", m_byUseAdapter);
        StaticLog( "m_bySpeakerSrc: %d\n", m_bySpeakerSrc);
        StaticLog( "m_bySpeakerSrcMode: %d\n", m_bySpeakerSrcMode);
        StaticLog( "m_byMulcastLowStream: %d\n", m_byMulcastLowStream);
        StaticLog( "m_byAllInitDumb: %d\n", m_byAllInitDumb);
        StaticLog( "m_byUniformMode: %d\n", m_byUniformMode);
        StaticLog( "m_byDStreamType: %d\n", m_byDStreamType);
    }
}PACKED;

//(len:22)
struct TMediaEncryptVer36
{
protected:
    u8  m_byEncryptMode;    //加密模式:CONF_ENCRYPTMODE_NONE_VER36,CONF_ENCRYPTMODE_DES_VER36, CONF_ENCRYPT_AES_VER36
    s32 m_nKeyLen;          //加密key的长度
    u8  m_abyEncKey[MAXLEN_KEY_VER36]; //加密key
    u8  m_byReserve;//保留
public:
    TMediaEncryptVer36()
    {
        Reset();
    }
    void Reset()
    {
        m_byEncryptMode = CONF_ENCRYPTMODE_NONE_VER36;
        m_nKeyLen = 0;
    }
    
    void SetEncryptMode(u8 byEncMode)
    {
        m_byEncryptMode = byEncMode;
    }
    u8  GetEncryptMode()
    {
        return m_byEncryptMode;
    }
    void SetEncryptKey(u8 *pbyKey, s32 nLen)
    {
        m_nKeyLen = (nLen > MAXLEN_KEY_VER36 ? MAXLEN_KEY_VER36 : nLen);
        if(m_nKeyLen > 0)
            memcpy(m_abyEncKey, pbyKey, m_nKeyLen); 
        m_nKeyLen = htonl(m_nKeyLen);
    }
    
    void GetEncryptKey(u8 *pbyKey, s32* pnLen)
    {
        //if(pnLen != NULL)
			*pnLen = ntohl(m_nKeyLen);
        //if(pbyKey != NULL) 
			memcpy(pbyKey, m_abyEncKey, ntohl(m_nKeyLen));
    }
}PACKED;

//时间结构重新定义
struct TKdvTimeVer36
{
protected:
    u16 		m_wYear;		//年
    u8  		m_byMonth;		//月
    u8  		m_byMDay;		//日
    u8  		m_byHour;		//时
    u8  		m_byMinute;		//分
    u8  		m_bySecond;		//秒
    
public:
    void SetTime( const time_t * ptTime );//设置时间
    void GetTime( time_t & tTime ) const;//得到时间结构
    BOOL32 operator == ( const TKdvTimeVer36 & tTime );//重载
    
    u16  GetYear( void ) const	{ return( ntohs( m_wYear ) ); }
    u8   GetMonth( void ) const	{ return( m_byMonth ); }
    u8   GetDay( void ) const	{ return( m_byMDay ); }
    u8   GetHour( void ) const	{ return( m_byHour ); }
    u8   GetMinute( void ) const	{ return( m_byMinute ); }
    u8   GetSecond( void ) const	{ return( m_bySecond ); }
    void SetYear( u16  wYear )	{ m_wYear = htons( wYear ); }
    void SetMonth( u8   byMonth )	{ m_byMonth = byMonth; }
    void SetDay( u8   byDay )	{ m_byMDay = byDay; }
    void SetHour( u8   byHour )	{ m_byHour = byHour; }
    void SetMinute( u8   byMinute )	{ m_byMinute = byMinute; }
    void SetSecond( u8   bySecond )	{ m_bySecond = bySecond; }
    void Print( void )
    {
        StaticLog( "%d-%d-%d %d:%d:%d", ntohs(m_wYear),m_byMonth,m_byMDay,m_byHour,m_byMinute,m_bySecond );
    }
}PACKED;

//定义会议信息结构,该结构定义了会议基本信息和状态 (len:725)
struct TConfInfoVer36
{
protected:
	CConfIdVer36	m_cConfId;			//会议号，全网唯一
	TKdvTimeVer36	m_tStartTime;		//开始时间，控制台填0为立即开始
	u16 			m_wDuration;	    //持续时间(分钟)，0表示不自动停止
    u16				m_wBitRate;         //会议码率(单位:Kbps,1K=1024)
	u16				m_wSecBitRate;      //双速会议的第2码率(单位:Kbps,为0表示是单速会议)
	u16				m_wReserved;        //保留
	u8				m_byVideoFormat;    //视频格式,参见mcuconst.h中视频格式定义
	u8				m_byTalkHoldTime;   //最小发言持续时间(单位:秒)
    TCapSupportVer36 m_tCapSupport;		//会议支持的媒体
	s8      m_achConfPwd[MAXLEN_PWD_VER36+1];		//会议密码
	s8		m_achConfName[MAXLEN_CONFNAME_VER36+1];	//会议名
    s8      m_achConfE164[MAXLEN_E164_VER36+1];		//会议的E164号码
    TMtAliasVer36	m_tChairAlias;      //会议中主席的别名
	TMtAliasVer36	m_tSpeakerAlias;    //会议中发言人的别名
    TConfAttrbVer36 m_tConfAttrb;		//会议属性

	TMediaEncryptVer36  m_tMediaKey;   //第一版本所有密钥一样，不支持更新
		
public:
	TConfStatusVer36	m_tStatus;	//会议状态

public:	
	TConfInfoVer36( void ){ memset( this, 0, sizeof( TConfInfoVer36 ) ); m_tCapSupport.Clear(); }
	CConfIdVer36 GetConfId( void ) const { return m_cConfId; }
	void		 SetConfId( CConfIdVer36 cConfId ){ m_cConfId = cConfId; }
	TKdvTimeVer36 GetKdvStartTime( void ) const { return( m_tStartTime ); }
	void		  SetKdvStartTime( TKdvTimeVer36 tStartTime ){ m_tStartTime = tStartTime; }
    void   SetDuration(u16  wDuration){ m_wDuration = htons(wDuration);} 
    u16    GetDuration( void ) const { return ntohs(m_wDuration); }
    void   SetBitRate(u16  wBitRate){ m_wBitRate = htons(wBitRate);} 
    u16    GetBitRate( void ) const { return ntohs(m_wBitRate); }
    void   SetSecBitRate(u16 wSecBitRate){ m_wSecBitRate = htons(wSecBitRate);} 
    u16    GetSecBitRate( void ) const { return ntohs(m_wSecBitRate); }
    void   SetVideoFormat(u8   byVideoFormat){ m_byVideoFormat = byVideoFormat;} 
    u8     GetVideoFormat( void ) const { return m_byVideoFormat; }
    void   SetTalkHoldTime(u8   byTalkHoldTime){ m_byTalkHoldTime = byTalkHoldTime;} 
    u8     GetTalkHoldTime( void ) const { return m_byTalkHoldTime; }
    void   SetCapSupport(TCapSupportVer36 tCapSupport){ m_tCapSupport = tCapSupport;} 
    TCapSupportVer36 GetCapSupport( void ) const { return m_tCapSupport; }
	void   SetConfPwd( LPCSTR lpszConfPwd );
    LPCSTR GetConfPwd( void ) const { return m_achConfPwd; }
	void   SetConfName( LPCSTR lpszConfName );
    LPCSTR GetConfName( void ) const { return m_achConfName; }
	void   SetConfE164( LPCSTR lpszConfE164 );
	LPCSTR GetConfE164( void ) const { return m_achConfE164; }
    void	       SetChairAlias(TMtAliasVer36 tChairAlias){ m_tChairAlias = tChairAlias;} 
    TMtAliasVer36  GetChairAlias( void ) const { return m_tChairAlias; }
    void		   SetSpeakerAlias(TMtAliasVer36 tSpeakerAlias){ m_tSpeakerAlias = tSpeakerAlias;} 
    TMtAliasVer36  GetSpeakerAlias( void ) const { return m_tSpeakerAlias; }
    void		   SetConfAttrb(TConfAttrbVer36 tConfAttrb){ m_tConfAttrb = tConfAttrb;} 
    TConfAttrbVer36 GetConfAttrb( void ) const { return m_tConfAttrb; }
    void			 SetStatus(TConfStatusVer36 tStatus){ m_tStatus = tStatus;} 
    TConfStatusVer36 GetStatus( void ) const { return m_tStatus; }	

	TMediaEncryptVer36& GetMediaKey(void) { return m_tMediaKey; };
	void SetMediaKey(TMediaEncryptVer36& tMediaEncrypt){ memcpy(&m_tMediaKey, &tMediaEncrypt, sizeof(tMediaEncrypt));}

	void     GetVideoScale( u16 &wVideoWidth, u16 &wVideoHeight );
	BOOL32   HasChairman( void ) const{ if(m_tStatus.m_tChairman.GetMtId()==0)return FALSE; return TRUE; }	
	TMtVer36 GetChairman( void ) const{ return m_tStatus.m_tChairman; }
	void     SetNoChairman( void ){ memset( &m_tStatus.m_tChairman, 0, sizeof( TMtVer36 ) ); }
	void     SetChairman( TMtVer36 tChairman ){ m_tStatus.m_tChairman = tChairman; }
	BOOL32   HasSpeaker( void ) const{ if(m_tStatus.m_tSpeaker.GetMtId()==0)return FALSE; return TRUE; }
    TMtVer36 GetSpeaker( void ) const{ return m_tStatus.m_tSpeaker; }
	void   SetNoSpeaker( void ){ memset( &m_tStatus.m_tSpeaker, 0, sizeof( TMtVer36 ) ); }
	void   SetSpeaker( TMtVer36 tSpeaker ){ m_tStatus.m_tSpeaker = tSpeaker; }
	time_t GetStartTime( void ) const;	
	void   SetStartTime( time_t dwStartTime );

	void Print( void )
	{
		StaticLog( "\nConfInfo:\n" );
		StaticLog( "m_cConfId: " );
		m_cConfId.Print();
		StaticLog( "\nm_tStartTime: " );
        m_tStartTime.Print();
		StaticLog( "\nm_wDuration: %d\n", ntohs(m_wDuration) );
		StaticLog( "m_wBitRate: %d\n", ntohs(m_wBitRate) );
		StaticLog( "m_byVideoFormat: %d\n", m_byVideoFormat );
		StaticLog( "m_byTalkHoldTime: %d\n", m_byTalkHoldTime );
		StaticLog( "m_tCapSupport:" );
	    m_tCapSupport.Print();
		StaticLog( "\nm_achConfPwd: %s\n", m_achConfPwd );
		StaticLog( "m_achConfName: %s\n", m_achConfName );
		StaticLog( "m_achConfE164: %s\n", m_achConfE164 );
		StaticLog( "\n" );
	}
}PACKED;


// ----------   TMtVer36    ---------- 

/*====================================================================
    函数名      ：SetMt
    功能        ：设置终端
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMcuId, MCU号
				  u8 byMtId, 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2002/07/26  1.0         LI Yi         创建
    2003/10/28  3.0         胡昌威        修改
	2005/01/06	4.0			张宝卿		  从3.6移植过来
====================================================================*/
inline void TMtVer36::SetMt( u8 byMcuId, u8 byMtId, u8 byDriId, u8 byConfIdx )
{
	m_byMainType = TYPE_MT;
	m_bySubType = 0;
	m_byMcuId = byMcuId;
	m_byEqpId = byMtId;	
	m_byConfDriId = byDriId;  
    m_byConfIdx = byConfIdx; 
}

/*====================================================================
    函数名      ：SetMt
    功能        ：设置终端
    算法实现    ：
    引用全局变量：
    输入参数说明：TMt tMt 
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2005/01/24  3.6         魏治兵        创建
	2005/01/06	4.0			张宝卿		  从3.6移植过来
====================================================================*/
inline void TMtVer36::SetMt( TMtVer36 tMt )
{
	SetMt(tMt.GetMcuId(), tMt.GetMtId(), tMt.GetDriId(), tMt.GetConfIdx());
}

/*====================================================================
    函数名      ：SetMcu
    功能        ：设置MCU
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMcuId, MCU号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2002/07/26  1.0         LI Yi         创建
    2003/10/28  3.0         胡昌威        修改 
	2005/01/06	4.0			张宝卿		  从3.6移植过来
====================================================================*/
inline void TMtVer36::SetMcu( u8 byMcuId )
{
	m_byMainType = TYPE_MCU;
	m_bySubType = 0;
	m_byMcuId = byMcuId;
	m_byEqpId = 0;	
	m_byConfDriId = 0;  
    m_byConfIdx = 0;    
}

/*====================================================================
    函数名      ：SetMcuEqp
    功能        ：设置MCU外设
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMcuId, MCU号
				  u8 byEqpId, 外设号
				  u8 byEqpType, 外设类型
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2002/07/26  1.0         LI Yi         创建
    2003/06/06  1.0         LI Yi         增加外设类型参数
    2003/10/28  3.0         胡昌威        修改  
	2005/01/06	4.0			张宝卿		  从3.6移植过来
====================================================================*/
inline void TMtVer36::SetMcuEqp( u8 byMcuId, u8 byEqpId, u8 byEqpType )
{
	m_byMainType = TYPE_MCUPERI;
	m_bySubType = byEqpType;
	m_byMcuId = byMcuId;
	m_byEqpId = byEqpId;	
	m_byConfDriId = 0;  
    m_byConfIdx = 0; 
}


// ----------   TConfInfoVer36    ---------- 

/*====================================================================
    函数名      ：SetConfPwd
    功能        ：设置别名
    算法实现    ：
    引用全局变量：
    输入参数说明：LPCSTR lpszConfPwd, 别名
    返回值说明  ：字符串指针
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/26    1.0         LI Yi         创建
	2005/01/06	4.0			张宝卿		  从3.6移植过来
====================================================================*/
inline void TConfInfoVer36::SetConfPwd( LPCSTR lpszConfPwd )
{
	if( lpszConfPwd != NULL )
	{
		strncpy( m_achConfPwd, lpszConfPwd, sizeof( m_achConfPwd ) );
		m_achConfPwd[sizeof( m_achConfPwd ) - 1] = '\0';
	}
	else
	{
		memset( m_achConfPwd, 0, sizeof( m_achConfPwd ) );
	}
}

/*====================================================================
    函数名      ：SetConfName
    功能        ：设置别名
    算法实现    ：
    引用全局变量：
    输入参数说明：LPCSTR lpszConfName, 别名
    返回值说明  ：字符串指针
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/26    1.0         LI Yi         创建
	2005/01/06	4.0			张宝卿		  从3.6移植过来
====================================================================*/
inline void TConfInfoVer36::SetConfName( LPCSTR lpszConfName )
{
	if( lpszConfName != NULL )
	{
		strncpy( m_achConfName, lpszConfName, sizeof( m_achConfName ) );
		m_achConfName[sizeof( m_achConfName ) - 1] = '\0';
	}
	else
	{
		memset( m_achConfName, 0, sizeof( m_achConfName ) );
	}
}

/*====================================================================
    函数名      ：SetConfE164
    功能        ：设置别名
    算法实现    ：
    引用全局变量：
    输入参数说明：LPCSTR lpszConfE164, 别名
    返回值说明  ：字符串指针
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/26    1.0         LI Yi         创建
	2005/01/06	4.0			张宝卿		  从3.6移植过来
====================================================================*/
inline void TConfInfoVer36::SetConfE164( LPCSTR lpszConfE164 )
{
	if( lpszConfE164 != NULL )
	{
		strncpy( m_achConfE164, lpszConfE164, sizeof( m_achConfE164 ) );
		m_achConfE164[sizeof( m_achConfE164 ) - 1] = '\0';
	}
	else
	{
		memset( m_achConfE164, 0, sizeof( m_achConfE164 ) );
	}
}


// ----------   TCapSupportVer36    ---------- 

/*====================================================================
    函数名      ：SetVideoType
    功能        ：设置视频类型
    算法实现    ：
    引用全局变量：
    输入参数说明：u8   byVideoType 视频类型       
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/05    3.0         胡昌威        创建
	2005/01/06	4.0			张宝卿		  从3.6移植过来
====================================================================*/
inline void TCapSupportVer36::SetVideoType( u8   byVideoType )
{
    switch( byVideoType ) 
	{
    case MEDIA_TYPE_H261:
       SetSupportH261( TRUE );
    	break;

    case MEDIA_TYPE_H262:
       SetSupportH262( TRUE );
    	break;

	case MEDIA_TYPE_H263:
       SetSupportH263( TRUE );
		break;

	case MEDIA_TYPE_H263PLUS:
		SetSupportH263Plus( TRUE );
		break;

	case MEDIA_TYPE_H264:
       SetSupportH264( TRUE );
		break;

	case MEDIA_TYPE_MP4:
       SetSupportMPEG4( TRUE );
		break;

    default:
		break;
    }
}

/*====================================================================
    函数名      ：SetAudioType
    功能        ：设置音频类型
    算法实现    ：
    引用全局变量：
    输入参数说明：u8   byAudioType 音频类型            
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/05    3.0         胡昌威        创建 
	2005/01/06	4.0			张宝卿		  从3.6移植过来
====================================================================*/
inline void TCapSupportVer36::SetAudioType( u8   byAudioType )
{
    switch( byAudioType ) 
	{
    case MEDIA_TYPE_PCMA:
       SetSupportG711A( TRUE );
    	break;

    case MEDIA_TYPE_PCMU:
       SetSupportG711U( TRUE );
    	break;

    case MEDIA_TYPE_G722:
       SetSupportG722( TRUE );
    	break;

    case MEDIA_TYPE_G7231:
       SetSupportG7231( TRUE );
    	break;

	case MEDIA_TYPE_G728:
       SetSupportG728( TRUE );
		break;

	case MEDIA_TYPE_G729:
       SetSupportG729( TRUE );
		break;

	case MEDIA_TYPE_MP3:
       SetSupportMP3( TRUE );
		break;

    default:
		break;
    }
}


// ----------   CConfId    ----------

/*====================================================================
    函数名      ：GetConfId
    功能        ：获得会议号数组
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 * pbyConfId, 传入的BUFFER数组指针，用来返回会议号
			      u8 byBufLen, BUFFER大小
    返回值说明  ：实际返回数组大小
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/31    1.0         LI Yi         创建
	2005/01/09	4.0			张宝卿		  从3.6移植过来
====================================================================*/
inline u8 CConfIdVer36::GetConfId( u8 * pbyConfId, u8 byBufLen ) const
{
	u8	byLen = min( sizeof( m_abyConfId ), byBufLen );
	memcpy( pbyConfId, m_abyConfId, byLen );

	return( byLen );
}

/*====================================================================
    函数名      ：GetConfIdString
    功能        ：获得会议号字符串
    算法实现    ：
    引用全局变量：
    输入参数说明：LPSTR lpszConfId, 传入的字符串BUFFER数组指针，用来
	                   返回0结尾会议号字符串
			      u8 byBufLen, BUFFER大小
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/31    1.0         LI Yi         创建
	2005/01/06	4.0			张宝卿		  从3.6移植过来
====================================================================*/
inline void CConfIdVer36::GetConfIdString( LPSTR lpszConfId, u8 byBufLen ) const
{
	u8	byLoop;

	for( byLoop = 0; byLoop < sizeof( m_abyConfId ) && byLoop < ( byBufLen - 1 ) / 2; byLoop++ )
	{
		sprintf( lpszConfId + byLoop * 2, "%.2x", m_abyConfId[byLoop] );
	}
	lpszConfId[byLoop * 2] = '\0';
}

/*====================================================================
    函数名      ：SetConfId
    功能        ：设置会议号数组
    算法实现    ：
    引用全局变量：
    输入参数说明：const u8 * pbyConfId, 传入的数组指针
			      u8 byBufLen, BUFFER大小
    返回值说明  ：实际设置数组大小
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/31    1.0         LI Yi         创建
	2005/01/06	4.0			张宝卿		  从3.6移植过来
====================================================================*/
inline u8 CConfIdVer36::SetConfId( const u8 * pbyConfId, u8 byBufLen )
{
	u8	bySize = min( byBufLen, sizeof( m_abyConfId ) );
	
	memset( m_abyConfId, 0, sizeof( m_abyConfId ) );
	memcpy( m_abyConfId, pbyConfId, bySize );

	return( bySize );
}

	
/*====================================================================
    函数名      ：SetConfId
    功能        ：设置会议号数组
    算法实现    ：
    引用全局变量：
    输入参数说明：LPCSTR lpszConfId, 传入的会议号字符串
    返回值说明  ：实际设置数组大小
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/31    1.0         LI Yi         创建
	2005/01/06	4.0			张宝卿		  从3.6移植过来
====================================================================*/
inline u8 CConfIdVer36::SetConfId( LPCSTR lpszConfId )
{
	LPCSTR	lpszTemp = lpszConfId;
	char	achTemp[3], *lpszStop;
	u8	byCount = 0;

	memset( m_abyConfId, 0, sizeof( m_abyConfId ) );
	while( lpszTemp != NULL && lpszTemp + 1 != NULL && byCount < sizeof( m_abyConfId ) )
	{
		memcpy( achTemp, lpszTemp, 2 );
		achTemp[2] = '\0';
		m_abyConfId[byCount] = ( u8 )strtoul( achTemp, &lpszStop, 16 );
		byCount++;
		lpszTemp += 2;
	}

	return( byCount );
}
#ifdef WIN32
#pragma pack( pop )
#endif

#endif  // _MASTRUCT36_H_
