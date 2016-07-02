/*
// file:            eleses.h
// description:     header file for C compiler supporting SES of device
//                  for Keil C51, SDCC 8051 C
// version:         2.0
// copyright:       Senselock Data Security Centre
// date:            05/04/2007
*/

#ifndef __ELITE_E_SES_H_INCLUDED__
#define __ELITE_E_SES_H_INCLUDED__

#if (!defined __C51__ && !defined SDCC_mcs51)
#error "this file can't be used on your compiler!"
#endif

#pragma save

#ifdef __C51__
#pragma REGPARMS
#endif

#ifdef SDCC_mcs51
#ifdef SDCC_STACK_AUTO
#error "the ses functions are not reentrant"
#endif
#endif

#ifndef __MEMORY_SPACE_TYPE__
#define __MEMORY_SPACE_TYPE__
#define RAM_EXT     xdata
#define RAM_INT_DE  data
#define RAM_INT_ID  idata
#define ROM         code
#endif

#ifdef  __C51__
#ifndef DEFINE_AT
#define DEFINE_AT(TYPE, NAME, ADDRESS, MEMORY)      TYPE MEMORY NAME _at_ ADDRESS
#endif  //DEFINE_AT
#endif  //__C51__

#ifdef  SDCC_mcs51
#ifndef DEFINE_AT
#define DEFINE_AT(TYPE, NAME, ADDRESS, MEMORY)      MEMORY at ADDRESS TYPE NAME
#endif  //DEFINE_AT
#endif  //SDCC_mcs51

/*
//  const define
*/

// define ses error code
#define SES_SUCCESS                 0x0000          // success
#define SES_ERROR_EEPROM            0x9001          // write eeprom failed
#define SES_ERROR_UNSUPPORT         0x9002          // function not support
#define SES_ERROR_RTC               0x9003          // read clock module error
#define SES_ERROR_RTC_POWER         0x9004          // the clock module has been power down

#define SES_ERROR_MEMORY            0x9201          // memory error
#define SES_ERROR_PARAM             0x9204          // parameter error

// the following error only for write data
#define SES_ERROR_ADDRESS           0x9505          // the address to be written is invalid
#define SES_ERROR_WRITE             0x9508          // the address can't be write
#define SES_ERROR_LENGTH            0x9905          // the data length error

// define DES calculate flag
#define DES_ALGO_MASK               0x01            // bit mask of the des algorithm
#define DES_ALGO_SINGLE             0x00            // single des
#define DES_ALGO_TRIPLE             0x01            // triple des
#define DES_CRYPT_MASK              0x02            // bit mask of the des crypt
#define DES_CRYPT_ENC               0x00            // DES encrypt
#define DES_CRYPT_DEC               0x02            // DES decrypt

// define hash algorithm flag
#define HASH_ALGO_MASK              0x02            // bit mask of the hash algorithm
#define HASH_ALGO_SHA1              0x00            // sha1
#define HASH_ALGO_MD5               0x02            // md5

// define rsa calculate mode
#define RSA_MODE_NORMAL             0x00            // crypt directly
#define RSA_MODE_PKCS               0x01            // pkcs standard

// define timer mode
#define TIMER_MODE_NOCYCLE          0x00            // timer mode no cycle
#define TIMER_MODE_CYCLE            0x01            // timer mode cycle
#define TIMER_MODE_RELOAD           0x02            // timer mode reload

// define hardware info type
#define HW_INFO_DEVICE_SERIAL       0x00            // hardware serial number, 8 bytes
#define HW_INFO_VENDOR_DESC         0x01            // vendor descript, 8 bytes
#define HW_INFO_DEVICE_TYPE         0x02            // device type, 1 byte
#define HW_INFO_DEVICE_VERSION      0x03            // device version, 2 bytes
#define HW_INFO_ONWER_NAME          0x04            // the name of owner of module

// define deivce type bit
#define DEVICE_TYPE_NET             0x01            // net device
#define DEVICE_TYPE_RTC             0x02            // real time device


/*
//  data type define
*/

// UCHAR
#ifndef __UCHAR_DEFINED__
#define __UCHAR_DEFINED__
typedef unsigned char UCHAR;
#endif  //__UCHAR_DEFINED__

// USHORT
#ifndef __USHORT_DEFINED__
#define __USHORT_DEFINED__
typedef unsigned short USHORT;
#endif  //__USHORT_DEFINED__

// ULONG
#ifndef __ULONG_DEFINED__
#define __ULONG_DEFINED__
typedef unsigned long ULONG;
#endif  //__ULONG_DEFINED__

// BYTE
#ifndef __BYTE_DEFINED__
#define __BYTE_DEFINED__
typedef unsigned char BYTE;
#endif  //__BYTE_DEFINED__

// WORD
#ifndef __WORD_DEFINED__
#define __WORD_DEFINED__
typedef unsigned short WORD;
#endif  //__WORD_DEFINED__

// DWORD
#ifndef __DWORD_DEFINED__
#define __DWORD_DEFINED__
typedef unsigned long DWORD;
#endif  //__DWORD_DEFINED__

// standard TIME_T, start at 1/1/1970 0:0:0(UTC), 32 bits
#ifndef __TIME_T_DEFINED__
#define __TIME_T_DEFINED__
typedef unsigned long TIME_T;
#endif

// define RTC (Real-Time Clock) format
typedef struct __RTC_TIME_T {
    UCHAR   ucSecond;                               // second (0-59)
    UCHAR   ucMinute;                               // minute (0-59)
    UCHAR   ucHour;                                 // hour (0-23)
    UCHAR   ucDay;                                  // day of month (1-31)
    UCHAR   ucWeek;                                 // day of week (0-6, sunday is 0)
    UCHAR   ucMonth;                                // month (0-11)
    USHORT  usYear;                                 // year (0- 138, 1900 - 2038)
}RTC_TIME_T;

// define digist context for hash calculate
typedef struct __DIGIST_CONTEXT
{
    ULONG   aulState[5];                            // store the result of each block calculate
    ULONG   aulCount[2];                            // store the bit length of the data
    UCHAR   aucBuffer[64];                          // store the remainder data of the last block
}DIGIST_CONTEXT;

// define hash context, do not modify the the value in of the structure
typedef struct __HASH_CONTEXT
{
    UCHAR ucAlgorithm;                              // store the hash algorithm
    DIGIST_CONTEXT DgtCtx[1];                       // digist context
}HASH_CONTEXT;

// define HMAC context, do not modify the the value in of the structure
typedef struct __HMAC_CONTEXT
{
    UCHAR ucAlgorithm;                              // store the hash algorithm
    DIGIST_CONTEXT DgtCtx[2];                       // digist context
}HMAC_CONTEXT;

// define RSA public key struct
typedef struct __RSA_PUB_KEY
{
  UCHAR aucN[128];                // the modulus
  UCHAR aucE[4];                // the exponent
}RSA_PUB_KEY;

// define RSA private key struct
typedef struct __RSA_PRI_KEY
{
  UCHAR aucP[64];               // the larger prime
  UCHAR aucQ[64];               // the smaller prime
  UCHAR aucDp[64];                // (E^-1) mod (P-1)
  UCHAR aucDq[64];                // (E^-1) mod (Q-1)
  UCHAR aucQinv[64];              // (Q^-1) mod P
}RSA_PRI_KEY;


/*
//  macro function definition
*/

// convert data format, little-endian(LE) low byte first, big-endian(BE) high byte first (CC mean C Compiler)
#define _swap_u16(__x__)                _invert(__x__, 2)
#define _swap_u32(__x__)                _invert(__x__, 4)

#ifdef __C51__

#define LE16_TO_CC(__x__)               _swap_u16(__x__)
#define LE32_TO_CC(__x__)               _swap_u32(__x__)

#define CC_TO_LE16(__x__)               _swap_u16(__x__)
#define CC_TO_LE32(__x__)               _swap_u32(__x__)

#define BE16_TO_CC(__x__)               0
#define BE32_TO_CC(__x__)               0

#define CC_TO_BE16(__x__)               0
#define CC_TO_BE32(__x__)               0

#endif

#ifdef SDCC_mcs51

#define LE16_TO_CC(__x__)               0
#define LE32_TO_CC(__x__)               0

#define CC_TO_LE16(__x__)               0
#define CC_TO_LE32(__x__)               0

#define BE16_TO_CC(__x__)               _swap_u16(__x__)
#define BE32_TO_CC(__x__)               _swap_u32(__x__)

#define CC_TO_BE16(__x__)               _swap_u16(__x__)
#define CC_TO_BE32(__x__)               _swap_u32(__x__)

#endif

/*
// define communication buffer
*/

#ifndef pucInBuff
#define pucInBuff                       ((UCHAR xdata *)0xF000)                                         // inputted data
#endif


/*
//  function declaration
*/

// system function
void _exit(USHORT usRetCode);                                                                           // exit program
USHORT _get_input_len();                                                                                // get received data length
USHORT _set_output(UCHAR *pucData, USHORT usLen);                                                       // set return data

// data section function
USHORT _write(UCHAR *pucDest, UCHAR *pucData, USHORT usLen);                                            // write data

// DES crypt function, [ucFlag] referent des flag definition
USHORT _des_ecb(UCHAR *pucKey, UCHAR *pucIn, UCHAR *pucOut, USHORT usLen, UCHAR ucFlag);                // DES calculate ECB mode
USHORT _des_cbc(UCHAR *pucKey, UCHAR *pucIn, UCHAR *pucOut, USHORT usLen, UCHAR *pucIV, UCHAR ucFlag);  // DES calculate CBC mode
USHORT _des_mac(UCHAR *pucKey, UCHAR *pucIn, UCHAR *pucOut, USHORT usLen, UCHAR *pucIV, UCHAR ucFlag);  // DES mac, return 8 bytes

// standard hash interface, [ucAlgo] see hash algorithm definition
USHORT _hash_init(HASH_CONTEXT *pCtx, UCHAR ucAlgo);                                                    // sha1 initialize context
USHORT _hash_update(HASH_CONTEXT *pCtx, UCHAR *pucData, USHORT usLen);                                  // sha1 update data
USHORT _hash_final(HASH_CONTEXT *pCtx, UCHAR *pucData);                                                 // sha1 final, getting hash result

// HMAC function, [ucAlgo] see hash mode define
USHORT _hash_mac_init(HMAC_CONTEXT *pCtx, UCHAR *pucKey, USHORT usLen, UCHAR ucAlgo);                   // initial key
USHORT _hash_mac_update(HMAC_CONTEXT *pCtx, UCHAR *pucData, USHORT usLen);                              // update date
USHORT _hash_mac_final(HMAC_CONTEXT *pCtx, UCHAR *pucData);                                             // get result

// RSA calculation function, [ucMode] see rsa mode define
USHORT _rsa_pub_encrypt(RSA_PUB_KEY *pKey, UCHAR *pucIn, UCHAR *pucOut, USHORT usLen, UCHAR ucMode);        // RSA public key encryption
USHORT _rsa_pub_decrypt(RSA_PUB_KEY *pKey, UCHAR *pucIn, UCHAR *pucOut, USHORT *pusLen, UCHAR ucMode);      // RSA public key decryption
USHORT _rsa_pri_encrypt(RSA_PRI_KEY *pKey, UCHAR *pucIn, UCHAR *pucOut, USHORT usLen, UCHAR ucMode);        // RSA private key encryption
USHORT _rsa_pri_decrypt(RSA_PRI_KEY *pKey, UCHAR *pucIn, UCHAR *pucOut, USHORT *pusLen, UCHAR ucMode);      // RSA private key decryption

// timer function, [ucMode] see timer mode define
void _timer_start();                                                                                    // start timer
void _timer_stop();                                                                                     // stop timer
void _timer_init(ULONG ulCount, UCHAR ucMode);                                                          // initialize timer
ULONG _timer_get();                                                                                     // read timer value

// memory operation, used to increase memory operation speed
USHORT _mem_copy(void *pvDest, const void *pvSrc, USHORT usLen);                                        // memcpy
USHORT _mem_move(void *pvDest, const void *pvSrc, USHORT usLen);                                        // memmove
USHORT _mem_set(void *pvDest, UCHAR ucVal, USHORT usLen);                                               // memset
short _mem_cmp(void *pvDest, void *pvSrc, USHORT usLen);                                                // memcmp

// assistant function
USHORT _invert(void *pvData, USHORT usLen);                                                             // convert byte order
USHORT _rand(UCHAR *pucData, USHORT usLen);                                                             // get rand number

// real time function
USHORT _time(TIME_T *ptime);                                                                            // read real time start at 1/1/1970 0:0:0(UTC)
USHORT _mktime(TIME_T *ptime_t, RTC_TIME_T *ptm);                                                       // convert RTC_TIME_T struct to TIME_T (UTC)
USHORT _gmtime(TIME_T *ptime_t, RTC_TIME_T *ptm);                                                       // convert TIME_T to RTC_TIME_T (UTC)

// get device info, [bFlag] see device info flag define
USHORT _get_hw_info(UCHAR *pucData, USHORT usLen, UCHAR ucFlag);                                        // get hardware information

#pragma restore

#endif  //__ELITE_E_SES_H_INCLUDED__
