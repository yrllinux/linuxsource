/* ----------------------------- codec.h ---------------------------- */
/******************************************************************************
模块名  ：
文件名  : codec.h
相关文件    ：
文件实现功能：mp3 压缩编码主文件接口文件
作者        ：
版本        ：
-------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容

*******************************************************************************/
#ifndef		__CODEC__
#define		__CODEC__

#define Mp3EncHandle  void *
#define MP3_STEREO	0
#define	MP3_JOINT_STEREO 2
#define MP3_MONO	3
#define MP3_ENC_OK 0
#define MP3_ENC_ERR -1


typedef		struct
{
	l32			iFrequency;   
	l32			iMode;      
	l32			iBitRate;    
	l32			iEmphasis;    
	l32			fPrivate;   
	l32			fCRC;		
	l32			fCopyright;  
	l32			fOriginal;   
}TCodecInitIn;


l32		Mp3EncodeInit(Mp3EncHandle  *pHandle,TCodecInitIn *psIn);
u32	Mp3EncodeFrame(Mp3EncHandle  Handle,  void *pSamples,  s8 *pOutput,
							   BOOL bMp3SNR_Flag, s16 *ps16Mp3SNR);

u32	Mp3HuffEncodeFrame(Mp3EncHandle  Handle,  void *pSamples,  s8 *pOutput);//lyg add for mixer

//xuchao add 2005-11-03
u32	Mp3EncMDctFrame
(
	void					*Handle,
	void					*pSamples,
	l32                     *pl32XR
);

void		Mp3EncodeExit(Mp3EncHandle  Handle);
void		SetSpeechFlg(u8 * cMp3Stream, l32 iFlg);
void		GetSpeechFlg(u8 * cMp3Stream, l32 *iFlg);
void		SetMp3EncPara(Mp3EncHandle  Handle,  TCodecInitIn *psIn);
void   		GetMp3Version(s8*, l32, l32*);

#endif		/* __CODEC__ */
