
#ifndef AEC_H
#define AEC_H


#define AEC_ERR 1
#define AEC_OK  0


int AecInit(void **hAec);
void EchoCancel(void *hAec, short nFe[], short nNe[], short nOut[]);
void AecFree(void *hAec);
void  GetAecVersion(char *Version, int StrLen, int * StrLenUsed);



#endif

