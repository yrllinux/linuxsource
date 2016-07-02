/*****************************************************************************
模   块   名: g7221 Annex c编解码器接口
文   件   名: g7221c.h
相 关  文 件: decode.c,encode.c
文件实现功能: 
版        本: V1.0  Copyright(C) 2005-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
修 改 记 录:
日       期      版本        修改人      修改内容
 2006/05/09      1.0         徐  超      创    建
******************************************************************************/

#ifndef G7221C_H
#define G7221C_H

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************************
 Constant definitions                                                    
*************************************************************************************/
#define G7221C_FRAMESIZE      640
#define G7221_FRAMESIZE       320

#define MAX_SAMPLE_RATE 32000
#define MAX_FRAMESIZE   (MAX_SAMPLE_RATE/50)

#define FRAMESIZE MAX_FRAMESIZE
#define MAX_WORD_PER_FRAME  (MAX_BITS_PER_FRAME/16)

#define G7221C_BITRATE	 24000  //24000 or 32000 or 48000
#define G7221C_BANDWITH  14000

#define G7221_BITRATE	 24000  //24000 or 32000
#define G7221_BANDWITH   7000 

#define MAX_DCT_LENGTH      640
#define MAX_BITS_PER_FRAME  960

#define G7221_BITRATE_ERR   (-2)
#define G7221_MALLOC_ERR    (-1)
#define G7221_OK            1


typedef enum SyntaxType
{
	PACKED_FORMAT,  //packed bitstream
	ITU192_FORMAT   //ITU selection test bitstream
}CodeFormat;


//enoder...
typedef struct 
{
    s16  syntax;
    l32  bit_rate;
    s16  bandwidth;  
    s16  number_of_bits_per_frame; 
    s16  number_of_regions; 
    s16  frame_size;

    s16  history[MAX_FRAMESIZE];
    s16  mlt_coefs[MAX_FRAMESIZE];













} TG7221CEncoder;


void g7221_encfree(void *pvG7221EncH);
s16 g7221_encinit(void **ppvEncH, l32 l32BitRate);
void g7221_encode(void *pvEncH, s16 *ps16InBuf, s16 *ps16OutBuf);

/*
void g7221_encfree(TG7221CEncoder *pG7221EncH);
s16 g7221_encinit(TG7221CEncoder **ppEncH, l32 l32BitRate);
void g7221_encode(TG7221CEncoder *pEncH, s16 *ps16InBuf, s16 *ps16OutBuf);
*/

//decoder...

/***************************************************************************/
/* Type definitions                                                        */
/***************************************************************************/
typedef struct
{
    s16 *code_word_ptr;      /* pointer to the bitstream */
    s16 code_bit_count;      /* bit count of the current word */
    s16 current_word;        /* current word in the bitstream being processed */
    s16 number_of_bits_left; /* number of bits left in the current word */
    s16 next_bit;            /* next bit in the current word */
}Bit_Obj;

typedef struct
{
    s16 seed0;
    s16 seed1;
    s16 seed2;
    s16 seed3;
}Rand_Obj;


/* This object is used to control the command line input */
typedef struct 
{
    s16  syntax;
    l32  bit_rate;
    s16  bandwidth;
    s16  number_of_bits_per_frame;
    s16  number_of_regions;
    s16  frame_size;


    s16 frame_error_flag;
    s16 mag_shift;
    s16 old_mag_shift;

    Rand_Obj randobj;
    Bit_Obj bitobj;


	//buf
    s16 decoder_mlt_coefs[MAX_DCT_LENGTH];
    s16 old_decoder_mlt_coefs[MAX_DCT_LENGTH];
    s16 old_samples[MAX_DCT_LENGTH>>1];










}TG7221CDecoder;


s16 g7221_decinit(void **ppDecH, l32 l32BitRate);
void g7221_decfree(void *pDecH);
void g7221_decode(void *pDecH, s16 *ps16InBuf, s16 *ps16OutBuf);
/*
s16 g7221_decinit(TG7221CDecoder **ppDecH, l32 l32BitRate);
void g7221_decfree(TG7221CDecoder *pDecH);
void g7221_decode(TG7221CDecoder *pDecH, s16 *ps16InBuf, s16 *ps16OutBuf);
*/


#ifdef __cplusplus
}
#endif


#endif //end of G7221C_H

