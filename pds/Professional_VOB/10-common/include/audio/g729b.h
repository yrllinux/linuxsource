/******************************************************************************
模块名	： G729a编解码器结构定义                       
文件名	：SelfTypedefa.h
相关文件	：
文件实现功能：
    作者	：
版本		：
---------------------------------------------------------------------------------------------------------------------
修改记录:
日  期		版本		修改人		走读人    修改内容
2004-03-01  v1.0		chukaiyan 
2004-04-01	v2.0		chukaiyan	           fixpoint	
2004-04-19  V2.1        chukaiyan              g729a					------------
2004-05-10  V2.2        chukaiyan              supports VAD
2005/04/22  V2.3        徐     超              G729A_H修改成G729B_H
                                               使用头文件"algorithmtype.h"
******************************************************************************/


/*******************************************************************************
* FILE G729.H 
* VERSION 0.1
* HISTORY: 
*		2004-03-01
*       Decoder is added to this file.
* SZKDSH company.
* All rights reserved. (R) 
* 2004-03-10
* Author chukaiyan 
* email: chukaiyan@kdshc.com.cn
*********************************************************************************/

#ifndef G729B_H
#define G729B_H


#ifdef __cplusplus
extern "C" {
#endif

#ifndef LD8A_H

#define L_FRAME         ((s16)80)    /* LPC update frame size                     */

#define SIZE_WORD       ((s16)80)  /* size of bitstream frame */

#define SERIAL_SIZE     ((s16) 82)

#endif
// external interface declaration

// macro definitions
#define G729_OK   ((s16)0)		//succeed
#define G729_ERR ((s16)-1)		//some thing is error now
#define G729_TYPEERR ((s16)-2) //the type of the parameter that passed into function is error. This maybe caused by confused useing HG729AENC and HG729ADEC.
#define G729_VERERR ((s16)-3) //版本错误
#define G729_BUFFERFULL ((s16)-4) //Buffer is full. Should call decoder without any input data;
#define	G729_NOTENOUGHDATA ((s16)-5) //Buffer is empty. Must send some input data to the decoder.  

typedef enum VAD
{
	G729_VADDISABLE,
	G729_VADENABLE
	
}enumVAD;



#define HG729BENC void * //encoder handle 
#define HG729BDEC void * //decoder handle 


l32  g729_encinit(HG729BENC * poEncptr);

l32  g729_decinit(HG729BDEC * piodecptr);

l32  g729_encode(					//output:
				 HG729BENC pG729, //input: the input encoder
				 s16 * pInData, // input: encoder data
				 l32 inLength, //input: encoder data length
				 void * pOutPut, //output: encoded data
				 l32 * outLength); //output: length in char unit.

l32  g729_decode(									//output:
				 HG729BDEC pG729,					//input: the input decoder handle
					u8 * pDecbitstream,	//input: the decoder data
				 l32 inlength,					//input: length of the input length
				 s16* pDecOut,					//output: decoded data
				  l32 * poutLength			//output: length in char unit.
				 );

void g729_encfree(HG729BENC encptr);
void g729_decfree(HG729BDEC decptr);


void g729_SetEncAorB(enumVAD vad,HG729BENC  encptr);


#ifdef __cplusplus
}
#endif

#endif 
/*G729B_H*/
