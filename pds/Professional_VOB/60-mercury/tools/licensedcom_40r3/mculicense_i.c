/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Fri Oct 20 16:28:15 2006
 */
/* Compiler settings for .\mculicense.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID IID_IOprKey = {0xB2C0C385,0x31E4,0x44B1,{0xBD,0x82,0x95,0xD6,0x42,0xB4,0xDB,0x94}};


const IID LIBID_MCULICENSELib = {0xC56C28B8,0x04D6,0x44F9,{0x8A,0x80,0xFC,0xCB,0xC9,0x3E,0xAA,0x2A}};


const CLSID CLSID_OprKey = {0x6E7700B3,0x802D,0x4354,{0xB9,0x99,0x4B,0x24,0xFF,0xF0,0xE4,0x0F}};


#ifdef __cplusplus
}
#endif

