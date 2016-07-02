/******************************************************************************
模块名      ：
文件名      : mp3encode.h
相关文件    ：
文件实现功能：mp3 压缩编码主文件接口文件
作者        ：
版本        ：
-------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2007/09/12              陆志洋      添加注释
*******************************************************************************/
#ifndef MP3ENCODE_H
#define MP3ENCODE_H

#define Mp3EncHandle  void *  //mp3编码器句柄指针
#define MP3_STEREO       0    //立体声
#define MP3_JOINT_STEREO 2    //联合立体声
#define MP3_MONO         3    //单声道
#define MP3_ENC_OK       0    //初始化成功
#define MP3_ENC_ERR     -1    //初始化失败

typedef struct
{
    int  iFrequency;   //采样率
    int  iMode;        //声道模式
    int  iBitRate;     //比特率
    int  iEmphasis;    //
    int  fPrivate;   
    int  fCRC;          
    int  fCopyright;  
    int  fOriginal;   
}TCodecInitIn;

int Mp3EncodeInit(Mp3EncHandle  *pHandle, TCodecInitIn *psIn);
unsigned int Mp3EncodeFrame(Mp3EncHandle  Handle,  void *pSamples,  char *pOutput);
unsigned int Mp3HuffEncodeFrame(Mp3EncHandle  Handle,  void *pSamples,  char *pOutput);//lyg add for mixer

//xuchao add 2005-11-03
//unsigned int Mp3EncMDctFrame(void *Handle, void *pSamples, int *pl32XR);                
void  Mp3EncodeExit(Mp3EncHandle  Handle);
void  SetSpeechFlg(unsigned char * cMp3Stream, int iFlg);
void  GetSpeechFlg(unsigned char * cMp3Stream, int *iFlg);
void  SetMp3EncPara(Mp3EncHandle  Handle,  TCodecInitIn *psIn);
void  GetMp3Version(char*, int, int*);

#endif          /* __CODEC__ */
