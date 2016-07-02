#ifndef _G722_H
#define _G722_H

#define MODE_64KBPS 3
#define MODE_56KBPS 2
#define MODE_48KBPS 1
#define DECG722_SUCCEED  0
#define DECG722_FAILED_DATATOOLONG   1

// external interface declaration
void g722_encinit(void** encptr);
void g722_decinit(void** decptr);

void g722_encode( void* pG722, 
				 s16* rawdata,
				 l32 inbytenum,  // PCM data input in byte per call
				 u8* bitstream, 
				 l32 mode );
l32 g722_decode( void* pG722, 
				 u8* bitstream,
				 l32 inbytenum,  // input bitstream length in byte per call
				 s16* decdata, 
				 l32 mode );

void g722_encfree(void* encptr);
void g722_decfree(void* decptr);

//2006-12-29 xc add
void g722_encinit_TI(void *pVoid);
void g722_decinit_TI(void *pVoid);
void GetG722Version(s8 *Version, l32 StrLen, l32 * StrLenUsed);

#endif
