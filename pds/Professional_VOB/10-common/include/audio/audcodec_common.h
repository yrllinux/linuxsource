/******************************************************************************
Ä£¿éÃû      £º audcodec_common              
ÎÄ¼şÃû      £º audcodec_common.h
Ïà¹ØÎÄ¼ş    £ºÎŞ
ÎÄ¼şÊµÏÖ¹¦ÄÜ£º ±à½âÂëº¯Êı²ÎÊıÉùÃ÷µÈ
---------------------------------------------------------------------------------------
ĞŞ¸Ä¼ÇÂ¼:
ÈÕ  ÆÚ                  °æ±¾            ĞŞ¸ÄÈË     ×ß¶ÁÈË        ĞŞ¸ÄÄÚÈİ
02/22/2008              1.0             Â½Ö¾Ñó                    ´´½¨
**************************************************************************************/
#ifndef _AUDCODEC_COMMON_H_
#define _AUDCODEC_COMMON_H_
#ifdef __cplusplus
extern "C" {
#endif   /* __cplusplus */

#define AUDIO_SUCCESS          0        //³É¹¦·µ»ØÖµ        
#define AUDIO_FAILURE         (-1)      //Ê§°Ü·µ»ØÖµ
#define AUDIO_VER_LEN          128      //°æ±¾ºÅĞèÒªµÄ×îĞ¡ÄÚ´æ¿Õ¼ä´óĞ¡

#define TAudioEncStatusParam TAudioEncParam
#define TAudioDecStatusParam TAudioDecParam

//ÒôÆµ²ÉÑùÂÊÄ£Ê½
typedef enum
{
	SAMPLE_RATE_8K = 8000,         //²ÉÑùÂÊÎª8kHz
	SAMPLE_RATE_16K = 16000,       //²ÉÑùÂÊÎª16kHz
	SAMPLE_RATE_24K = 24000,       //²ÉÑùÂÊÎª24kHz
	SAMPLE_RATE_32K = 32000,       //²ÉÑùÂÊÎª32kHz
	SAMPLE_RATE_48K = 48000        //²ÉÑùÂÊÎª48kHz
}TEnumAudSampleRate;

//ÒôÆµ²ÉÑùÂÊË÷Òı
typedef enum
{
	  SAMPLE_RATE96000_INDEX = 0, //²ÉÑùÂÊÎª96kHzµÄË÷ÒıÊÇ0
		SAMPLE_RATE88200_INDEX,     //²ÉÑùÂÊÎª88.2kHzµÄË÷ÒıÊÇ1
		SAMPLE_RATE64000_INDEX,     //²ÉÑùÂÊÎª64kHzµÄË÷ÒıÊÇ2
		SAMPLE_RATE48000_INDEX,     //²ÉÑùÂÊÎª48kHzµÄË÷ÒıÊÇ3
		SAMPLE_RATE44100_INDEX,     //²ÉÑùÂÊÎª44.1kHzµÄË÷ÒıÊÇ4
		SAMPLE_RATE32000_INDEX,     //²ÉÑùÂÊÎª32kHzµÄË÷ÒıÊÇ5
		SAMPLE_RATE24000_INDEX,     //²ÉÑùÂÊÎª24kHzµÄË÷ÒıÊÇ6  
		SAMPLE_RATE22050_INDEX,     //²ÉÑùÂÊÎª22.05kHzµÄË÷ÒıÊÇ7
		SAMPLE_RATE16000_INDEX,     //²ÉÑùÂÊÎª16kHzµÄË÷ÒıÊÇ8
		SAMPLE_RATE12000_INDEX,     //²ÉÑùÂÊÎª12kHzµÄË÷ÒıÊÇ9
		SAMPLE_RATE11025_INDEX,     //²ÉÑùÂÊÎª11.025kHzµÄË÷ÒıÊÇ10 
		SAMPLE_RATE8000_INDEX      //²ÉÑùÂÊÎª8kHzµÄË÷ÒıÊÇ11
}TEnumAudSampleRateIndex;

//ÉùµÀÄ£Ê½
typedef enum
{
	CH_MONO = 1,                  //µ¥ÉùµÀ
	CH_STEREO                     //Ë«ÉùµÀ
}TEnumAudChannel;

//Ñù±¾Î»¿íÄ£Ê½
typedef enum
{
	BIT_WIDTH_16 = 16,            //Ñù±¾Î»¿íÎª16bit
	BIT_WIDTH_32 = 32             //Ñù±¾Î»¿íÎª32bit
}TEnumAudBitWidth;

//±ÈÌØÂÊÄ£Ê½
typedef enum
{
	AACLC_MO_16K_48K = 48000,//ÃüÃû·½Ê½£ºËã·¨_ÉùµÀ_²ÉÑùÂÊ_±ÈÌØÂÊ
	AACLC_MO_32K_72K = 72000,
	AACLC_MO_48K_96K = 96000,
	AACLC_ST_16K_48K = 48000,
	AACLC_ST_32K_72K = 72000,
	AACLC_ST_48K_96K = 96000,
}TEnumAudBitRate;

//±àÂëÆ÷³õÊ¼»¯²ÎÊı½á¹¹Ìå
typedef struct
{
    l32 l32Channel;     //ÉùµÀÄ£Ê½£¨TEnumAudChannel£©
    l32 l32BitWidth;    //Ñù±¾Î»¿íÄ£Ê½£¨TEnumAudBitWidth£©
    l32 l32SampleRate;  //ÒôÆµ²ÉÑùÂÊÄ£Ê½£¨TEnumAudSampleRate£©
    l32 l32BitRate;     //±ÈÌØÂÊÄ£Ê½(TEnumAudBitRate)
    l32 l32FrameLen;    //±àÂëÒ»Ö¡Ñù±¾Êı
    u32 u32FrameHeadFlag; //Ã¿Ö¡ÂëÁ÷ÊÇ·ñ´øÍ·ĞÅÏ¢(1£ºÓĞ£¬0£º·ñ)
    u32 u32Reserved;    //±£Áô²ÎÊı
}TAudioEncParam;

//±àÂëÊäÈë½á¹¹Ìå
typedef struct
{
    u8 *pu8InBuf;      //Ö¸Ïò±àÂëÆ÷ÊäÈë»º´æÖ¸Õë
    u32 u32InBufSize;  //ÊäÈë»º´æ×Ö½ÚÊı 
    u8 *pu8OutBuf;     //Ö¸Ïò±àÂëÆ÷Êä³ö»º´æÖ¸Õë
    u32 u32OutBufSize; //Êä³ö»º´æ×Ö½ÚÊı  
    u32 u32Reserved;   //±£Áô²ÎÊı
}TAudEncInput;

//±àÂëÊä³ö½á¹¹Ìå
typedef struct
{       
    u32  u32OutBitStreamSize;   //Êä³öÂëÁ÷µÄ×Ö½ÚÊı
    u32  u32StatisCycles;       //±àÂëÒ»Ö¡Ğ§ÂÊÍ³¼Æ
    u32  u32Reserved;           //±£Áô²ÎÊı
}TAudEncOutput;

//½âÂëÆ÷³õÊ¼»¯²ÎÊı½á¹¹Ìå                  
typedef struct                                  
{                                               
    l32 l32BitRate;     //±ÈÌØÂÊÄ£Ê½(TEnumAudBitRate)
    u32 u32FrameHeadFlag; //Ã¿Ö¡ÂëÁ÷ÊÇ·ñ´øÍ·ĞÅÏ¢(1£ºÓĞ£¬0£º·ñ)
    u32 u32Reserved;    //±£Áô²ÎÊı                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
}TAudioDecParam;        

//½âÂëÆ÷ÊäÈë½á¹¹Ìå                                                 
typedef struct                                 
{                                              
    u8 *pu8InBuf;            //Ö¸Ïò½âÂëÆ÷ÊäÈë»º´æÖ¸Õë   
    u32 u32InBufSize;        //ÊäÈë»º´æ×Ö½ÚÊı                                                                                                                                                                                                                                                                                      º´æÖ¸Õë

    u8 *pu8OutBuf;           //Ö¸Ïò½âÂëÆ÷Êä³ö»º´æÖ¸Õë 
    u32 u32OutBufSize;       //Êä³ö»º´æ×Ö½ÚÊı                                                                                                                                                                                                                                                                                           
    u32 u32InBitStreamSize;  //Ã¿Ö¡ÂëÁ÷µÄ×Ö½ÚÊı   
    u32 u32SampelFreIndex;   //Ã¿Ö¡²ÉÑùÂÊË÷Òı
    u32 u32Channels;         //Ã¿Ö¡Í¨µÀÊı                                                                                                                                                                                                                                                                        Ä×Ö½ÚÊı

    u32 u32Reserved;         //±£Áô²ÎÊı                                                                                                                                                                                                                                                                                                
}TAudDecInput;      

//½âÂëÆ÷Êä³ö½á¹¹Ìå
typedef struct
{               
    u32  u32OutFrameLen ; //Ã¿Ö¡Êä³öÑù±¾Êı
    u32  u32StatisCycles; //½âÂëÒ»Ö¡Ğ§ÂÊÍ³¼Æ
    u32  u32Reserved;     //±£Áô²ÎÊı
}TAudDecOutput; 

//´íÎóÂë¶¨Òå
#define ERR_AACLCENC  (15 << 24)    //AACLC±àÂë×ÓÄ£¿é    
#define ERR_AACLCDEC  (16 << 24)    //AACLC½âÂë×ÓÄ£¿é
                                                                                                                    
#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif


