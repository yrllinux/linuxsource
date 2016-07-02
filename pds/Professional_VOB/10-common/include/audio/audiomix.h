

#define MAX_CHANNEL_MIX 		7
#define	PCM_FRAME_LEN_MIX		1152	


#define MIXINIT_ERR_DEC		-1
#define MIXINIT_ERR_ENC		-2
#define MIXINIT_ERR_MLC		-3
#define MIXINIT_OK	0  

#define MIXPROC_ERR	-4  
#define MIXPROC_OK	0


typedef struct
{
	char * cMp3Buf;
	int iMp3Len;
	short nVolume;
}AudioMixPara;

typedef struct
{
	void *EncodeHandle[MAX_CHANNEL_MIX+1];
	void *DecodeHandle[MAX_CHANNEL_MIX];
	
	short *nMixInBuf[MAX_CHANNEL_MIX];
	short *nMixOutBuf[MAX_CHANNEL_MIX+1];
	
	char * cPlayBuf;
	
}AudioMixStat;

int AudioMixInit(void **HAudioMix, TCodecInitIn *ciiParam);
int AudioMixExit(void *HAudioMix);
int AudioMixProc(void * hMix, AudioMixPara *pInPara[], AudioMixPara *pOutPara[], AudioMixPara *pNModeOut, int iChIndx);
