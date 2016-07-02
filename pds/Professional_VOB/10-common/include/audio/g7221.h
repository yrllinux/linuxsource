/*****************************************************************************
模   块   名: g7221编解码器接口
文   件   名: 
相 关  文 件: 
文件实现功能: 
版        本: V1.0  Copyright(C) 2005-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
修 改 记 录:
日       期      版本        修改人      修改内容
 2006/03/15      1.0         徐  超      创    建
******************************************************************************/

#ifndef G7221_H
#define G7221_H

#ifdef __cplusplus
extern "C" {
#endif

#define G7221_BITRATE_ERR   (-2)
#define G7221_MALLOC_ERR    (-1)
#define G7221_OK            1

#define FRAMESIZE           320             //20ms秒的音频数据

#define DCT_LENGTH          320
#define MAX_BITS_PER_FRAME  640
#define MAX_WORD_PER_FRAME  (MAX_BITS_PER_FRAME/16)



typedef enum SyntaxType
{
	PACKED_FORMAT,  //packed bitstream
	ITU192_FORMAT   //ITU selection test bitstream
}CodeFormat;

//enoder...
typedef struct tG7221Encoder
{
    s16  syntax;
    s16  bit_rate;
    s16  number_of_bits_per_frame;

	s16  as16MLTBuf[FRAMESIZE];
    s16  mlt_coefs[FRAMESIZE];

}TG7221Encoder, *pTG7221Encoder;


//24000或32000
s16 g7221_encinit(TG7221Encoder **ppG7221EncH, s16 s16BitRate);
void g7221_encode(TG7221Encoder *pG7221EncH, s16 *ps16InBuf, s16 *ps16OutBuf);
void g7221_encfree(TG7221Encoder *pG7221EncH);




//decoder...
typedef struct
{
    s16 code_bit_count;      /* bit count of the current word */
    s16 current_word;        /* current word in the bitstream being processed */
    s16 *code_word_ptr;      /* pointer to the bitstream */
    s16 number_of_bits_left; /* number of bits left in the current word */
    s16 next_bit;            /* next bit in the current word */
}Bit_Obj;

typedef struct tG7221Decoder
{
    s16  syntax;
    s16  bit_rate;
    s16  number_of_bits_per_frame;

	//globe var
    s16 frame_error_flag;
    s16 mag_shift;
    s16 old_mag_shift;

	//Random
    s16 seed0;
    s16 seed1;
    s16 seed2;
    s16 seed3;

	//buf
    s16 decoder_mlt_coefs[DCT_LENGTH];
    s16 old_decoder_mlt_coefs[DCT_LENGTH];
    s16 old_samples[DCT_LENGTH>>1];

	//bit object pointer;
    Bit_Obj stBitObj;

}TG7221Decoder, *pTG7221Decoder;

//24000或32000
s16 g7221_decinit(TG7221Decoder **ppG7221DecH, s16 s16BitRate);
void g7221_decode(TG7221Decoder *pG7221DecH, s16 *ps16InBuf, s16 *ps16OutBuf);
void g7221_decfree(TG7221Decoder *pG7221DecH);

#ifdef __cplusplus
}
#endif


#endif //end of G7221_H

