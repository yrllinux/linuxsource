/* ----------------------------- codec.h ---------------------------- */
#ifndef		__AUDDECODE__
#define		__AUDDECODE__

#define Mp3DecHandle void *
#define	MP3_DEC_HDR_WRONG -1
#define MP3_DEC_OK	   0
#define MP3_DEC_WRONG  -2

typedef 	struct
{
	u32 	* Mp3SteamInBuf;
	s16 		*PlayOutBuf;
	s16 		*PcmOutBuf;
	l32 		iVolume;
}TMp3Dec;

extern l32 	Mp3DecodeInit(Mp3DecHandle  *pHandle);
extern l32 	Mp3DecodeFrame(Mp3DecHandle  handle, TMp3Dec *pAudDec);
extern l32 	Mp3HuffDecodeFrame(Mp3DecHandle  handle, TMp3Dec *pAudDec);//lyg add for MIXER
extern void 	Mp3DecodeExit(Mp3DecHandle  handle);
extern void     GetMp3Version(s8*, l32, l32*);

#endif		/* __AUDDECODE__ */
