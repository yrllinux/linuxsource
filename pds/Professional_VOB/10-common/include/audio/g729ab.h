/*****************************************************************************
模块名      : g729ab编解码器
文件名      : g729.c
相关文件    : g729ab.h
文件实现功能: g729ab编解码功能句柄式接口实现
版本        : V1.0  Copyright(C) 2005-2007 KDC, All rights reserved.
作者        : 徐超
-----------------------------------------------------------------------------
修改记录:
日    期      版本        修改人      修改内容
2006/02/16    1.0         徐  超      创    建
******************************************************************************/
#ifndef G729AB_H
#define G729AB_H

#ifdef __cplusplus
extern "C" {
#endif

//macro define
#define BUFFERLENGTH ((s16)128)
//1. <ld8a.h>
#define  L_TOTAL      240     /* Total size of speech buffer.               */
#define  L_WINDOW     240     /* Window size in LP analysis.                */
#define  L_NEXT       40      /* Lookahead in LP analysis.                  */
#define  L_FRAME      80      /* Frame size.                                */
#define  L_SUBFR      40      /* Subframe size.                             */
#define  M            10      /* Order of LP filter.                        */
#define  MP1          (M+1)   /* Order of LP filter + 1                     */
#define  PIT_MIN      20      /* Minimum pitch lag.                         */
#define  PIT_MAX      143     /* Maximum pitch lag.                         */
#define  L_INTERPOL   (10+1)  /* Length of filter for interpolation.        */
#define  GAMMA1       24576   /* Bandwitdh factor = 0.75   in Q15           */

#define  PRM_SIZE     11      /* Size of vector of analysis parameters.     */
#define  SERIAL_SIZE  (80+2)  /* bfi+ number of speech bits                 */

#define SHARPMAX  13017   /* Maximum value of pitch sharpening     0.8  Q14 */
#define SHARPMIN  3277    /* Minimum value of pitch sharpening     0.2  Q14 */

/*--------------------------------------------------------------------------*
 *       LSP constant parameters                                            *
 *--------------------------------------------------------------------------*/
#define   NC            5      /*  NC = M/2 */
#define   MA_NP         4      /* MA prediction order for LSP */
#define   MODE          2      /* number of modes for MA prediction */
#define   NC0_B         7      /* number of first stage bits */
#define   NC1_B         5      /* number of second stage bits */
#define   NC0           (1<<NC0_B)
#define   NC1           (1<<NC1_B)

#define   L_LIMIT          40   /* Q13:0.005 */
#define   M_LIMIT       25681   /* Q13:3.135 */

#define   GAP1          10     /* Q13 */
#define   GAP2          5      /* Q13 */
#define   GAP3          321    /* Q13 */
#define GRID_POINTS     50

#define PI04      ((s16)1029)        /* Q13  pi*0.04 */
#define PI92      ((s16)23677)       /* Q13  pi*0.92 */
#define CONST10   ((s16)10*(1<<11))  /* Q11  10.0 */
#define CONST12   ((s16)19661)       /* Q14  1.2 */

/*--------------------------------------------------------------------------*
 *       LTP constant parameters                                            *
 *--------------------------------------------------------------------------*/

#define UP_SAMP         3
#define L_INTER10       10
#define FIR_SIZE_SYN    (UP_SAMP*L_INTER10+1)

/*-------------------------------*
 * gain VQ constants.            *
 *-------------------------------*/

#define NCODE1_B  3                /* number of Codebook-bit */
#define NCODE2_B  4                /* number of Codebook-bit */
#define NCODE1    (1<<NCODE1_B)    /* Codebook 1 size */
#define NCODE2    (1<<NCODE2_B)    /* Codebook 2 size */
#define NCAN1     4                /* Pre-selecting order for #1 */
#define NCAN2     8                /* Pre-selecting order for #2 */
#define INV_COEF  -17103           /* Q19 */

/*-----------------------*
 * Bitstream function    *
 *-----------------------*/
#define BIT_0     (s16)0x007f /* definition of zero-bit in bit-stream      */
#define BIT_1     (s16)0x0081 /* definition of one-bit in bit-stream       */
#define SYNC_WORD (s16)0x6b21 /* definition of frame erasure flag          */
#define SIZE_WORD (s16)80     /* number of speech bits                     */


/*-----------------------------------*
 * Post-filter functions.            *
 *-----------------------------------*/

#define L_H 22     /* size of truncated impulse response of A(z/g1)/A(z/g2) */

#define GAMMAP      16384   /* 0.5               (Q15) */
#define INV_GAMMAP  21845   /* 1/(1+GAMMAP)      (Q15) */
#define GAMMAP_2    10923   /* GAMMAP/(1+GAMMAP) (Q15) */

#define  GAMMA2_PST 18022 /* Formant postfilt factor (numerator)   0.55 Q15 */
#define  GAMMA1_PST 22938 /* Formant postfilt factor (denominator) 0.70 Q15 */

#define  MU       26214   /* Factor for tilt compensation filter   0.8  Q15 */
#define  AGC_FAC  29491   /* Factor for automatic gain control     0.9  Q15 */
#define  AGC_FAC1 (s16)(32767 - AGC_FAC)    /* 1-AGC_FAC in Q15          */


//2.<dtx.h>
/*--------------------------------------------------------------------------*
 * Constants for DTX/CNG                                                    *
 *--------------------------------------------------------------------------*/

/* DTX constants */
#define FLAG_COD        (s16)1
#define FLAG_DEC        (s16)0
#define INIT_SEED       11111
#define FR_SID_MIN      3
#define NB_SUMACF       3
#define NB_CURACF       2
#define NB_GAIN         2
#define FRAC_THRESH1    4855
#define FRAC_THRESH2    3161
#define A_GAIN0         28672

#define SIZ_SUMACF      (NB_SUMACF * MP1)
#define SIZ_ACF         (NB_CURACF * MP1)
#define A_GAIN1         4096    /* 32768L - A_GAIN0 */

#define RATE_8000       80      /* Full rate  (8000 bit/s)       */
#define RATE_SID        15      /* SID                           */
#define RATE_0           0      /* 0 bit/s rate                  */

/* CNG excitation generation constant */
                                        /* alpha = 0.5 */
#define FRAC1           19043           /* (sqrt(40)xalpha/2 - 1) * 32768 */
#define K0              24576           /* (1 - alpha ** 2) in Q15        */
#define G_MAX           5000

//3. vad.h
#define     NP            12                  /* Increased LPC order */
#define     NOISE         0
#define     VOICE         1
#define     INIT_FRAME    32
#define     INIT_COUNT    20
#define     ZC_START      120
#define     ZC_END        200
///////////////////////////////////
// end of macro define
typedef struct s729BEncoder
{
	s8  s8Version[10];
	s16 Frame;
	s16 s16Prm[PRM_SIZE + 1];     // Transmitted parameters  //prm[0] now is frame index      /
	s16 s16Serial[SERIAL_SIZE];   // Output bit stream buffer      /
	
	s16 vad_enable;

	l32   l32Length;         //句柄结构长度
	s16 y2_hi, y2_lo, y1_hi, y1_lo, x0, x1;

	//Speech vector
	s16 old_speech[L_TOTAL];
	s16 *new_speech;
	s16 *speech;
	s16 *p_window;
	
	//Weighted speech vector
	s16 old_wsp[L_FRAME+PIT_MAX];
	s16 *wsp;
	
	//Excitation vector
	s16 old_exc[L_FRAME+PIT_MAX+L_INTERPOL];
	s16 *exc;
	
	//Lsp (Line spectral pairs)
	s16 lsp_old[M];
	//={
	//	30000, 26000, 21000, 15000, 8000, 0, -8000,-15000,-21000,-26000};
	s16 lsp_old_q[M];
	
    //Filter's memory
	s16  mem_w0[M], mem_w[M], mem_zero[M];
	s16  sharp;
	
    //For G.729B
	s16 pastVad;   
	s16 ppastVad;
	s16 seed;
	
	//from vad.c
	s16 MeanLSF[M];
	s16 Min_buffer[16];
	s16 Prev_Min, Next_Min, Min;
	s16 MeanE, MeanSE, MeanSLE, MeanSZC;
	s16 prev_energy;
	s16 vad_prev_energy;  //害死人的同名参数

	s16 count_sil, count_update, count_ext;
	s16 flag, v_flag, less_count;
	
	//from qua_lsp.c
	s16 freq_prev[MA_NP][M];    //Q13:previous LSP vector

	//from taming.c
	l32 L_exc_err[4];
	
	s16 noise_fg[MODE][MA_NP][M];
	
	//from dtx.c
	s16 lspSid_q[M] ;
	s16 pastCoeff[MP1];
	s16 RCoeff[MP1];
	s16 sh_RCoeff;



	s16 Acf[SIZ_ACF];     //dtx.h   
	s16 sh_Acf[NB_CURACF];
	s16 sumAcf[SIZ_SUMACF];
	s16 sh_sumAcf[NB_SUMACF];
	s16 ener[NB_GAIN];
	s16 sh_ener[NB_GAIN];
	s16 fr_cur;
	s16 cur_gain;
	s16 nb_ener;
	s16 sid_gain;
	s16 flag_chang;
	s16 dtx_prev_energy;
	s16 count_fr0;

	s16 past_qua_en[4];	//from file QUA_GAIN.C function qua_gain //Q10

}TG729BENCODER, * pTG729BENCODER;



typedef struct s729BDecoder
{
	s8 version[10];
	l32 length;
	s16  synth_buf[L_FRAME+M];            /* From g729.c Synthesis             */
	s16  *synth;							/* From g729.c Synthesis             */
	s16    parm[PRM_SIZE+5];                /* From g729.c Synthesis parameters + BFI */
	s16  Az_dec[2*MP1];					/* From g729.c Decoded Az for post-filter */
	s16	*ptr_Az;						/* From g729.c Decoded Az for post-filter */
	s16   t2[2]; //* From g729.c
	s16  pst_out[L_FRAME];                /* From g729.c postfilter output          */
	l32 sf_voic;							  /* From g729.c voicing for subframe */

	s16 old_exc[L_FRAME+PIT_MAX+L_INTERPOL]; 
	s16 *exc;
	s16 mem_syn[M];        /* Filter's memory */
	s16 sharp ;            /* pitch sharpening of previous fr */
	s16 old_T0;              /* integer delay of previous frame */
	s16 gain_code;         /* fixed codebook gain */
	s16 gain_pitch ;       /* adaptive codebook gain */
	s16 lsp_old[M];
	
	
	s16 freq_prev[MA_NP][M];    /* previous LSP vector       */ //Q13
	
	s16 prev_ma;                  /* previous MA prediction coef.*/
	
	s16 prev_lsp[M];            /* previous LSP vector         */
	
	s16 mem_zero[M];          /* null memory to compute h_st  */
	s16 mem_stp[M];            /* s.t. postfilter memory       */
	
	s16 res2_buf[PIT_MAX+L_SUBFR];        /* A(gamma2) residual           */
	s16 * res2;
	
	s16 mem_syn_pst[M];

	s16 scal_res2_buf[PIT_MAX+L_SUBFR];
	s16 *scal_res2;double gain_prec;             /* for gain adjustment          */
	
	l32  count_frame_preemphasis;
	l32  count_frame_Syn_filt;
	l32  count_frame_Residu;
	

	s16 y2_hi, y2_lo, y1_hi, y1_lo, x0, x1;
	
	s16 past_qua_en[4];
	
	
	s16 bitconver[SIZE_WORD + 3];
	s16 mem_pre;
	s16 past_gain;
	
	
	s16 vad_enable;
	s8 DataBuffer[BUFFERLENGTH];
	s16 DataLength;
	s16 past_ftyp;
	s16 seed;
	s16 sid_sav, sh_sid_sav;
	s16 seed_fer;		
	s16 noise_fg[MODE][MA_NP][M];
	l32 L_exc_err[4];
	
	s16 cur_gain;
	s16 lspSid[M];
	s16 sid_gain;	

	void * PrivateData;   //reserved.
	
}TG729BDECODER, * pTG729BDECODER;




//external interface declaration

// macro definitions
#define G729_OK   ((s16)0)	               //succeed
#define G729_ERR ((s16)-1)		           //some thing is error now
#define G729_TYPEERR ((s16)-2)             //the type of the parameter that passed into function is error. This maybe caused by confused useing HG729AENC and HG729ADEC.
#define G729_VERERR ((s16)-3)              //版本错误
#define G729_BUFFERFULL ((s16)-4)          //Buffer is full. Should call decoder without any input data;
#define	G729_NOTENOUGHDATA ((s16)-5)       //Buffer is empty. Must send some input data to the decoder.  

typedef enum VAD
{
	G729_VADDISABLE,
	G729_VADENABLE
}enumVAD;

#define HG729BENC void *                  //encoder handle 
#define HG729BDEC void *                  //decoder handle 

void  GetG729Version(u8 *Version, l32 StrLen, l32 * StrLenUsed);

l32  g729_encinit(HG729BENC * poEncptr);

l32  g729_decinit(HG729BDEC * piodecptr);

l32  g729_encode(
				 HG729BENC pG729,  //input: the input encoder
				 s16 * pInData,    // input: encoder data
				 l32 inLength,     //input: encoder data length
				 void * pOutPut,   //output: encoded data
				 l32 * outLength); //output: length in s8 unit.

l32  g729_decode(							//output:
				 HG729BDEC pG729,			//input: the input decoder handle
					s8 * pDecbitstream,	//input: the decoder data
				 l32 inlength,				//input: length of the input length
				 s16* pDecOut,				//output: decoded data
				  l32 * poutLength			//output: length in s8 unit.
				 );

void g729_encfree(HG729BENC encptr);
void g729_decfree(HG729BDEC decptr);


void g729_SetEncAorB(enumVAD vad,HG729BENC  encptr);


#ifdef __cplusplus
}
#endif

#endif //end of G729AB_H


