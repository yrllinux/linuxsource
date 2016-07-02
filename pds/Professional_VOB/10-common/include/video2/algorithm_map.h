#ifndef _ALGORITHM_MAP_H_
#define _ALGORITHM_MAP_H_
#include "eti\mm.h"
#include "vcodeccomlib.h"
#include "h264comlib.h"
#include "eti_loopdir.h"

#define MAX_COEFFCOST 256
#define CAVLC_LEVEL_LIMIT 2063
#define Q_BITS          15

static const n64 n64Mask0 = 0x0001000100010001;
static const n64 n64Mask1 = 0xffff0001ffff0001;
static const n64 n64Mask2 = 0xffffffff00010001;        
static const n64 n64Mask3 = 0x0001ffffffff0001;

static const s16 as16QP_REM[52] = 
{
    0,1,2,3,4,5,
    0,1,2,3,4,5,
    0,1,2,3,4,5,
    0,1,2,3,4,5,
    0,1,2,3,4,5,
    0,1,2,3,4,5,
    0,1,2,3,4,5,
    0,1,2,3,4,5,
    0,1,2,3
};

static const s16 as16DEQUANT[6][16] = 
{
    {10,13,10,13,13,16,13,16,10,13,10,13,13,16,13,16},
    {11,14,11,14,14,18,14,18,11,14,11,14,14,18,14,18},
    {13,16,13,16,16,20,16,20,13,16,13,16,16,20,16,20},
    {14,18,14,18,18,23,18,23,14,18,14,18,18,23,18,23},
    {16,20,16,20,20,25,20,25,16,20,16,20,20,25,20,25},
    {18,23,18,23,23,29,23,29,18,23,18,23,23,29,23,29}
};

static const s16 as16QP_PER[52] = 
{
    0,0,0,0,0,0,
    1,1,1,1,1,1,
    2,2,2,2,2,2,
    3,3,3,3,3,3,
    4,4,4,4,4,4,
    5,5,5,5,5,5,
    6,6,6,6,6,6,
    7,7,7,7,7,7,
    8,8,8,8
};

static const s16 as16QP_BITS[9] = 
{
    Q_BITS, Q_BITS + 1, Q_BITS + 2, Q_BITS + 3, Q_BITS + 4, Q_BITS + 5, Q_BITS + 6, Q_BITS + 7, Q_BITS + 8
};

static const s16 as16QUANT[6][16] = 
{
    {13107,8066,13107,8066,8066,5243,8066,5243,13107,8066,13107,8066,8066,5243,8066,5243},
    {11916,7490,11916,7490,7490,4660,7490,4660,11916,7490,11916,7490,7490,4660,7490,4660},
    {10082,6554,10082,6554,6554,4194,6554,4194,10082,6554,10082,6554,6554,4194,6554,4194},
    { 9362,5825, 9362,5825,5825,3647,5825,3647, 9362,5825, 9362,5825,5825,3647,5825,3647},
    { 8192,5243, 8192,5243,5243,3355,5243,3355, 8192,5243, 8192,5243,5243,3355,5243,3355},
    { 7282,4559, 7282,4559,4559,2893,4559,2893, 7282,4559, 7282,4559,4559,2893,4559,2893}
};
// [slice_type][qp_per]
static const l32 al32QP_CONST[3][9] = 
{
    {(1 << Q_BITS)/6,(1 << (Q_BITS+1))/6,(1 << (Q_BITS+2))/6,(1 << (Q_BITS+3))/6,(1 << (Q_BITS+4))/6,
    (1 << (Q_BITS+5))/6,(1 << (Q_BITS+6))/6,(1 << (Q_BITS+7))/6,(1 << (Q_BITS+8))/6},

    {0,0,0,0,0,0,0,0,0},

    {(1 << Q_BITS)/3,(1 << (Q_BITS+1))/3,(1 << (Q_BITS+2))/3,(1 << (Q_BITS+3))/3,(1 << (Q_BITS+4))/3,
    (1 << (Q_BITS+5))/3,(1 << (Q_BITS+6))/3,(1 << (Q_BITS+7))/3,(1 << (Q_BITS+8))/3}
};

static s8 as8RCTest[] = 
{
     3,  0,  2,  3,  1,  2,  1,  6,
     0,  2,  0,  5,  0,  5,  0,  9,
    -1,  1, -1,  5, -1,  4, -1,  8,
    -1,  5, -1,  8, -1,  8, -1, 12,
     4,  0,  2,  2,  1,  2,  1,  5,
     0,  1,  0,  4,  0,  5,  0,  8,
    -1,  1, -1,  4, -1,  4, -1,  7,
    -1,  4, -1,  7, -1,  8, -1, 11,
     5,  0,  2,  2,  1,  1,  1,  5,
     0,  1,  0,  4,  0,  4,  0,  8,
    -1,  1, -1,  4, -1,  3, -1,  7,
    -1,  4, -1,  7, -1,  7, -1, 11,
     6,  0,  2,  1,  1,  1,  1,  4,
     0,  1,  0,  3,  0,  4,  0,  7,
    -1,  0, -1,  3, -1,  3, -1,  6,
    -1,  4, -1,  6, -1,  7, -1, 10,
     7,  0,  2,  1,  1,  1,  1,  4,
     0,  0,  0,  3,  0,  4,  0,  7,
    -1,  0, -1,  3, -1,  3, -1,  6,
    -1,  3, -1,  6, -1,  7, -1, 10,
     8,  0,  2,  1,  1,  0,  1,  4,
     0,  0,  0,  3,  0,  3,  0,  7,
    -1,  0, -1,  3, -1,  2, -1,  6,
    -1,  3, -1,  6, -1,  6, -1, 10,
     9,  0,  2,  0,  1,  0,  1,  3,
     0,  0,  0,  2,  0,  3,  0,  6,
    -1,  0, -1,  2, -1,  2, -1,  5,
    -1,  3, -1,  5, -1,  6, -1,  9,
    10,  0,  2,  0,  1,  0,  1,  3,
     0,  0,  0,  2,  0,  3,  0,  6,
    -1,  0, -1,  2, -1,  2, -1,  5,
    -1,  3, -1,  5, -1,  6, -1,  9,
    11,  0,  2,  0,  1,  0,  1,  3,
     0,  0,  0,  2,  0,  3,  0,  6,
    -1,  0, -1,  2, -1,  2, -1,  5,
    -1,  3, -1,  5, -1,  6, -1,  9,
    12,  0,  2,  0,  1,  0,  1,  3,
     0,  0,  0,  2,  0,  3,  0,  6,
    -1,  0, -1,  2, -1,  2, -1,  5,
    -1,  3, -1,  5, -1,  6, -1,  9,
    13,  0,  2,  0,  1,  0,  1,  3,
     0,  0,  0,  2,  0,  3,  0,  6,
    -1,  0, -1,  2, -1,  2, -1,  5,
    -1,  3, -1,  5, -1,  6, -1,  9,
    14,  0,  2,  0,  1,  0,  1,  3,
     0,  0,  0,  2,  0,  3,  0,  6,
    -1,  0, -1,  2, -1,  2, -1,  5,
    -1,  3, -1,  5, -1,  6, -1,  9,
    15,  0,  2,  0,  1,  0,  1,  3,
     0,  0,  0,  2,  0,  3,  0,  6,
    -1,  0, -1,  2, -1,  2, -1,  5,
    -1,  3, -1,  5, -1,  6, -1,  9,
};

static void INLINE
DCT2x2DC_map(s16 *ps16InBlock)
{
    n64 n64Tem0, n64Out0, n64Out1;
    n64* pn64InBlk = (n64*)ps16InBlock;  
    l32* pl32OutBlk = (l32*)ps16InBlock;
    hmpv_128 plc0;
   
    //将4个s16位数据组成一个64位数据
    n64Tem0 = pn64InBlk[0];

    plc0.lo = n64Tem0; plc0.hi = n64Tem0;
    n64Out0 = hmpv_ssetinprod_p64s32_ps16_sa(n64Mask1, n64Mask0, plc0).rv;                                                                                                 
    n64Out1 = hmpv_ssetinprod_p64s32_ps16_sa(n64Mask3, n64Mask2, plc0).rv;   
    
    pl32OutBlk[0] = hmpv_compress_ps32_td(n64Out0, 0);
    pl32OutBlk[1] = hmpv_compress_ps32_td(n64Out1, 0);
}

static void INLINE
Quant2x2DC_map(s16 *ps16InCoeff, u8 u8Quant, l32 l32SliceType)
{
    n64 *pn64Block, n64D0, n64D1, n64Qbitsadd;
    n64u n64uRound64, n64uData, n64uData1;
    u32 u32Coff;
    
    n64uRound64.u32.l0 = n64uRound64.u32.l1 = al32QP_CONST[l32SliceType * 2][as16QP_PER[u8Quant]] << 1; 
    n64Qbitsadd = as16QP_BITS[as16QP_PER[u8Quant]]; 
    n64Qbitsadd++;
    
    pn64Block = (n64 *)(ps16InCoeff);    
    n64uData.n64 = pn64Block[0];

    //求系数
    u32Coff = as16QUANT[as16QP_REM[u8Quant]][0];
    u32Coff = hmpv_bcopyrev_32(u32Coff, 1, 0);        
       
    //组乘
    n64D0 = hmpv_mpyfull_ps16_cl0(n64uData.s32.l0, u32Coff);
    n64D1 = hmpv_mpyfull_ps16_cl1(n64uData.s32.l1, u32Coff);

    //取绝对值	
    n64D0 = hmpv_dif_pu32_ps32_cl0(n64D0, 0);                   
    n64D1 = hmpv_dif_pu32_ps32_cl1(n64D1, 0);                            
                            
    //组加                            
    n64D0 = hmpv_add_pu32_cl0(n64D0, n64uRound64.n64);                   
    n64D1 = hmpv_add_pu32_cl1(n64D1, n64uRound64.n64);                   
         
    n64uData1.s32.l0 = hmpv_compress_ps32_td_sa_cl0(n64D0, n64Qbitsadd);
    n64uData1.s32.l1 = hmpv_compress_ps32_td_sa_cl1(n64D1, n64Qbitsadd);   
    
    //还原符号
    n64D0 = hmpv_appsign_ps16_sa(n64uData.n64, n64uData1.n64);
    
    //写回数据
    pn64Block = (n64 *)(ps16InCoeff);
    pn64Block[0] = n64D0;
}

static void INLINE
iQuant2x2DC_map(s16 *ps16InCoeff, u8 u8Quant)
{
    n64 *pn64Block, n64D0, n64D1;
    n64u n64uRound64, n64uData;
    u32 u32Coff;
    
    n64uRound64.u32.l0 = 1 << as16QP_PER[u8Quant];
    n64uRound64.n64 = hmpv_bcopyrev_64(n64uRound64.n64, 3, 0);
    
    //两个s16组合成一个32位数据
    pn64Block = (n64 *)(ps16InCoeff);
    u32Coff = as16DEQUANT[as16QP_REM[u8Quant]][0];  
    u32Coff = hmpv_bcopyrev_32(u32Coff, 1, 0);

    n64uData.n64 = pn64Block[0];
    //组乘
    n64D0 = hmpv_mpyfull_ps16(n64uData.s32.l0, u32Coff);
    n64D1 = hmpv_mpyfull_ps16(n64uData.s32.l1, u32Coff);
    
    //组乘
    n64D0 = hmpv_mpy_ps32(n64D0, n64uRound64.n64);
    n64D1 = hmpv_mpy_ps32(n64D1, n64uRound64.n64);
    
    n64uData.s32.l0 = hmpv_compress_ps32_td_sa(n64D0, 0);
    n64uData.s32.l1 = hmpv_compress_ps32_td_sa(n64D1, 0);    
    
    //结果写回
    pn64Block = (n64 *)(ps16InCoeff);
    pn64Block[0] = hmpv_rs_ps16_td(n64uData.n64, 1);  
}

static INLINE void 
MemCopy_map(void *pu8Dst, const void *pu8Src, u16 u16Size)
{
    n64 * restrict pn64Src = (n64 *)pu8Src;
    n64 * restrict pn64Dst = (n64 *)pu8Dst;
    l32 l32Row, l32Size = (u16Size >> 3);
    
    unroll_amount(2);
    for(l32Row = 0; l32Row < l32Size; l32Row ++)
    {
        pn64Dst[0] = pn64Src[0];		
        pn64Src ++;
        pn64Dst ++;
    }
}

static INLINE void
MemSet_map(void *pu8Dst, const l32 l32Src, u16 u16Size)
{
    n64 *pn64Dst, n64T;
    l32 l32Row;
    l32 l32Size = u16Size >> 3;
        
    n64T = hmpv_map_64(l32Src, 0);
    pn64Dst = (n64 *)pu8Dst;
    
    unroll_amount(2);
    for(l32Row = 0; l32Row < l32Size; l32Row++)
    {
        pn64Dst[0] = n64T;
        pn64Dst ++;
    }	
}

static INLINE l32
Clip3_map(l32 l32A, l32 l32Low, l32 l32High)
{
    l32A = hmpv_slct_32(l32A < l32Low, l32Low, l32A);
    l32A = hmpv_slct_32(l32A > l32High, l32High, l32A);
        
    return l32A;
}

/* get median of three number */
static INLINE l32 
GetMedian_map(l32 l32X, l32 l32Y, l32 l32Z)
{
    l32 l32Min, l32Max;
    
    l32Min = hmpv_slct_32(l32X < l32Y, l32X, l32Y);
    l32Min = hmpv_slct_32(l32Z < l32Min, l32Z, l32Min);
    l32Max = hmpv_slct_32(l32X < l32Y, l32Y, l32X);
    l32Max = hmpv_slct_32(l32Z > l32Max, l32Z, l32Max);

    return l32X + l32Y + l32Z - l32Min - l32Max;
}

static INLINE u32
ArrayNonZeroCount4_map(s16* ps16Data)
{
    n64 *pn64Data, n64A;
    l32 l32NonZero;

    pn64Data = (n64 *)ps16Data;
    n64A = hmpv_gcomp_ps16(pn64Data[0], 0, 5);
    l32NonZero = hmpv_sum2_ps32_ps16(n64A, 0);

    return (u32)(-l32NonZero);
}

static INLINE u32
ArrayNonZeroCount8_map(s16* ps16V)
{
    n64 *pn64Data, n64A, n64B;
    l32 l32NonZero;

    pn64Data = (n64 *)ps16V;

    n64A = hmpv_gcomp_ps16(pn64Data[0], 0, 5);
    n64B = hmpv_gcomp_ps16(pn64Data[1], 0, 5);

    l32NonZero = hmpv_sum2_ps32_ps16(n64A, n64B);

    return (u32)(-l32NonZero);
}

static INLINE u32
ArrayNonZeroCount16_map(s16* ps16V)
{
    n64 *pn64Data, n64A, n64B, n64C, n64D;
    l32 l32NonZero;

    pn64Data = (n64 *)ps16V;

    n64A = hmpv_gcomp_ps16(pn64Data[0], 0, 5);
    n64B = hmpv_gcomp_ps16(pn64Data[1], 0, 5);
    n64C = hmpv_gcomp_ps16(pn64Data[2], 0, 5);
    n64D = hmpv_gcomp_ps16(pn64Data[3], 0, 5);

    l32NonZero = hmpv_sum2_ps32_ps16(n64A, n64B);
    l32NonZero += hmpv_sum2_ps32_ps16(n64C, n64D);

    return (u32)(-l32NonZero);
}

static INLINE void
ScanZigzag16x16_map(s16 ps16Zig[16][4 * 4], s16* ps16Dct)
{
    l32 l32BlkX, l32BlkY;
    s16* restrict ps16Z = ps16Zig[0];
    
    unroll_amount(4);
    for(l32BlkX = 0; l32BlkX < 4; l32BlkX ++)
    {   
        unroll_amount(4);
        for (l32BlkY = 0; l32BlkY < 4; l32BlkY ++)
        {
            ps16Z[0]  = ps16Dct[0];
            ps16Z[1]  = ps16Dct[1];
            ps16Z[2]  = ps16Dct[4];
            ps16Z[3]  = ps16Dct[8];
            ps16Z[4]  = ps16Dct[5];
            ps16Z[5]  = ps16Dct[2];
            ps16Z[6]  = ps16Dct[3];
            ps16Z[7]  = ps16Dct[6];
            ps16Z[8]  = ps16Dct[9];
            ps16Z[9]  = ps16Dct[12];
            ps16Z[10] = ps16Dct[13];
            ps16Z[11] = ps16Dct[10];
            ps16Z[12] = ps16Dct[7];
            ps16Z[13] = ps16Dct[11];
            ps16Z[14] = ps16Dct[14];
            ps16Z[15] = ps16Dct[15];

            ps16Z += 16;
            ps16Dct += 16;
        }
    }
}

static INLINE void
ScanZigzag8x8_map(s16* ps16Zig, s16* ps16Dct)
{
    l32 l32Blk;
    s16* restrict ps16ZigD = ps16Zig[0];    
    unroll_amount(4);
    for (l32Blk = 0; l32Blk < 4; l32Blk ++)
    {
        ps16ZigD[0]  = ps16Dct[0];
        ps16ZigD[1]  = ps16Dct[1];
        ps16ZigD[2]  = ps16Dct[4];
        ps16ZigD[3]  = ps16Dct[8];
        ps16ZigD[4]  = ps16Dct[5];
        ps16ZigD[5]  = ps16Dct[2];
        ps16ZigD[6]  = ps16Dct[3];
        ps16ZigD[7]  = ps16Dct[6];
        ps16ZigD[8]  = ps16Dct[9];
        ps16ZigD[9]  = ps16Dct[12];
        ps16ZigD[10] = ps16Dct[13];
        ps16ZigD[11] = ps16Dct[10];
        ps16ZigD[12] = ps16Dct[7];
        ps16ZigD[13] = ps16Dct[11];
        ps16ZigD[14] = ps16Dct[14];
        ps16ZigD[15] = ps16Dct[15];

        ps16ZigD += 16;
        ps16Dct += 16;
    }
}

static INLINE void 
ScanZigzag4x4_map(s16* ps16Zig, s16* ps16Dct)
{    
    n64u n64ud0, n64ud1, n64ud2, n64ud3, n64uz0, n64uz1, n64uz2, n64uz3;

    n64* pn64Zig = (n64 *)ps16Zig;
    n64* pn64Dct = (n64 *)ps16Dct;

    n64ud0.n64 = pn64Dct[0];
    n64ud1.n64 = pn64Dct[1];
    n64ud2.n64 = pn64Dct[2];
    n64ud3.n64 = pn64Dct[3];

    n64uz0.s32.l0 = n64ud0.s32.l0;
    n64uz0.s16.s2 = n64ud1.s16.s0;
    n64uz0.s16.s3 = n64ud2.s16.s0;

    n64uz1.s16.s0 = n64ud1.s16.s1;
    n64uz1.s16.s1 = n64ud0.s16.s2;
    n64uz1.s16.s2 = n64ud0.s16.s3;
    n64uz1.s16.s3 = n64ud1.s16.s2;

    n64uz2.s16.s0 = n64ud2.s16.s1;
    n64uz2.s16.s1 = n64ud3.s16.s0;
    n64uz2.s16.s2 = n64ud3.s16.s1;
    n64uz2.s16.s3 = n64ud2.s16.s2;

    n64uz3.s16.s0 = n64ud1.s16.s3;
    n64uz3.s16.s1 = n64ud2.s16.s3;
    n64uz3.s32.l1 = n64ud3.s32.l1;

    pn64Zig[0] = n64uz0.n64;
    pn64Zig[1] = n64uz1.n64;
    pn64Zig[2] = n64uz2.n64;
    pn64Zig[3] = n64uz3.n64;
}

static INLINE void
ScanZigzag2x2_map(s16* ps16Zig, s16* ps16Dct)
{
    n64 *pn64Zig, *pn64Dct;

    pn64Zig = (n64 *)ps16Zig;
    pn64Dct = (n64 *)ps16Dct;

    pn64Zig[0] = pn64Dct[0]; 
}

static INLINE l32 F264CoeffCost16_map(s16* ps16Coeff)
{
    n64 *pn64Coeff;
    n64u n64uD0, n64uD1, n64uD2, n64uD3;
    n64u n64uV0, n64uV1, n64uV2, n64uV3;
    n64 n64K0, n64K1, n64K2, n64K3;
    l32 l32Temp, l32CoeffCost, l32Run;
    
    pn64Coeff = (n64*)ps16Coeff;

    n64uD0.n64 = pn64Coeff[0];
    n64uD1.n64 = pn64Coeff[1];
    n64uD2.n64 = pn64Coeff[2];
    n64uD3.n64 = pn64Coeff[3];

    n64uV0.n64 = hmpv_dif_pu16_ps16(n64uD0.n64, 0x0000000000000000);
    n64uV1.n64 = hmpv_dif_pu16_ps16(n64uD1.n64, 0x0000000000000000);
    n64uV2.n64 = hmpv_dif_pu16_ps16(n64uD2.n64, 0x0000000000000000);
    n64uV3.n64 = hmpv_dif_pu16_ps16(n64uD3.n64, 0x0000000000000000);

    n64K0 = hmpv_band_64(n64uV0.n64, 0xFFFEFFFEFFFEFFFE);
    n64K1 = hmpv_band_64(n64uV1.n64, 0xFFFEFFFEFFFEFFFE);
    n64K2 = hmpv_band_64(n64uV2.n64, 0xFFFEFFFEFFFEFFFE);
    n64K3 = hmpv_band_64(n64uV3.n64, 0xFFFEFFFEFFFEFFFE);		

    if(n64K0 == 0 && n64K1 == 0 && n64K2 == 0 && n64K3 == 0)
    {
        l32Temp = (n64uV0.s16.s0 << 1) | (n64uV0.s16.s1 << 2) | (n64uV0.s16.s2 << 3) | (n64uV0.s16.s3 << 4);
        l32Run = as8RCTest[l32Temp];
        l32CoeffCost = as8RCTest[l32Temp + 1];

        l32Temp = (n64uV1.s16.s0 << 1)| (n64uV1.s16.s1 << 2) | (n64uV1.s16.s2 << 3) | (n64uV1.s16.s3 << 4);
        l32Run = (l32Run + 1) << 5;
        l32Temp = l32Run | l32Temp;
        l32Run = as8RCTest[l32Temp];
        l32CoeffCost += as8RCTest[l32Temp + 1];

        l32Temp = (n64uV2.s16.s0 << 1) | (n64uV2.s16.s1 << 2) | (n64uV2.s16.s2 << 3) | (n64uV2.s16.s3 << 4);
        l32Run = (l32Run + 1) << 5;
        l32Temp = l32Run | l32Temp;
        l32Run = as8RCTest[l32Temp];
        l32CoeffCost += as8RCTest[l32Temp + 1];

        l32Temp = (n64uV3.s16.s0 << 1) | (n64uV3.s16.s1 << 2) | (n64uV3.s16.s2 << 3) | (n64uV3.s16.s3 << 4);
        l32Run = (l32Run + 1) << 5;
        l32Temp = l32Run | l32Temp;
        l32CoeffCost += as8RCTest[l32Temp + 1];
    }
    else
    {
        l32CoeffCost = MAX_COEFFCOST;
    }
    
    return l32CoeffCost;	
}

static INLINE l32 F264CoeffCost15_map(s16* ps16Coeff)
{
    n64 *pn64Coeff;
    n64u n64uD0, n64uD1, n64uD2, n64uD3;
    n64u n64uV0, n64uV1, n64uV2, n64uV3;
    n64 n64K0, n64K1, n64K2, n64K3;
    l32 l32Temp, l32CoeffCost, l32Run;
    
    pn64Coeff = (n64 *)(ps16Coeff - 1);

    n64K0 = pn64Coeff[0];
    n64K1 = pn64Coeff[1];
    n64K2 = pn64Coeff[2];
    n64K3 = pn64Coeff[3];

    n64uD0.n64 = hmpv_alignr_p8(n64K1, n64K0, 2);
    n64uD1.n64 = hmpv_alignr_p8(n64K2, n64K1, 2);
    n64uD2.n64 = hmpv_alignr_p8(n64K3, n64K2, 2);
    
    n64uD3.s16.s0 = ps16Coeff[12];
    n64uD3.s16.s1 = ps16Coeff[13];
    n64uD3.s16.s2 = ps16Coeff[14];
    n64uD3.s16.s3 = 0;

    n64uV0.n64 = hmpv_dif_pu16_ps16(n64uD0.n64, 0x0000000000000000);
    n64uV1.n64 = hmpv_dif_pu16_ps16(n64uD1.n64, 0x0000000000000000);
    n64uV2.n64 = hmpv_dif_pu16_ps16(n64uD2.n64, 0x0000000000000000);
    n64uV3.n64 = hmpv_dif_pu16_ps16(n64uD3.n64, 0x0000000000000000);

    n64K0 = hmpv_band_64(n64uV0.n64, 0xFFFEFFFEFFFEFFFE);
    n64K1 = hmpv_band_64(n64uV1.n64, 0xFFFEFFFEFFFEFFFE);
    n64K2 = hmpv_band_64(n64uV2.n64, 0xFFFEFFFEFFFEFFFE);
    n64K3 = hmpv_band_64(n64uV3.n64, 0xFFFEFFFEFFFEFFFE);
	
    if(n64K0 == 0 && n64K1 == 0 && n64K2 == 0 && n64K3 == 0)
    {
        l32Temp = (n64uV0.s16.s0 << 1) | (n64uV0.s16.s1 << 2) | (n64uV0.s16.s2 << 3) | (n64uV0.s16.s3 << 4);
        l32Run = as8RCTest[l32Temp];
        l32CoeffCost = as8RCTest[l32Temp + 1];

        l32Temp = (n64uV1.s16.s0 << 1) | (n64uV1.s16.s1 << 2) | (n64uV1.s16.s2 << 3) | (n64uV1.s16.s3 << 4);
        l32Run = (l32Run + 1) << 5;
        l32Temp = l32Run | l32Temp;
        l32Run = as8RCTest[l32Temp];
        l32CoeffCost += as8RCTest[l32Temp + 1];

        l32Temp = (n64uV2.s16.s0 << 1) | (n64uV2.s16.s1 << 2) | (n64uV2.s16.s2 << 3) | (n64uV2.s16.s3 << 4);
        l32Run = (l32Run + 1) << 5;
        l32Temp = l32Run | l32Temp;
        l32Run = as8RCTest[l32Temp];
        l32CoeffCost += as8RCTest[l32Temp + 1];

        l32Temp = (n64uV3.s16.s0 << 1) | (n64uV3.s16.s1 << 2) | (n64uV3.s16.s2 << 3) | (n64uV3.s16.s3 << 4);
        l32Run = (l32Run + 1) << 5;
        l32Temp = l32Run | l32Temp;
        l32CoeffCost += as8RCTest[l32Temp + 1];
    }
    else
    {
        l32CoeffCost = MAX_COEFFCOST;
    }
    
    return l32CoeffCost;
}

#undef F264SAD16x16
#undef F264SAD16x8
#undef F264SAD8x16
#undef F264SAD8x8
#undef Interpolate16x16H
#undef Interpolate16x16V
#undef Interpolate16x16C
#undef Interpolate16x16

#undef PixelAverage16x16
#undef EighthPelMCChroma8x8
#undef EighthPelMCChroma8x4
#undef EighthPelMCChroma4x8
#undef EighthPelMCChroma4x4

#undef DCT4x4DC
#undef DCT2x2DC
#undef AddiDCT16x16
#undef AddiDCT8x8
#undef iDCT4x4DC
#undef iDCT2x2DC

#undef Quant16x16
#undef Quant8x8
#undef Quant4x4DC
#undef Quant2x2DC
#undef iQuant16x16
#undef iQuant8x8
#undef iQuant4x4DC
#undef iQuant2x2DC

#undef DCTandQuant16x16
#undef AddiDCTandiQuant16x16
#undef AddiDCTandiQuant8x8
#undef SubDCT16x16
#undef SubDCT8x8

#undef IntraPredict16x16Horz
#undef IntraPredict16x16Vert
#undef IntraPredict16x16DC
#undef IntraPredict16x16Plane
#undef IntraPredict16x16DCLeft
#undef IntraPredict16x16DCTop 
#undef IntraPredict16x16DC128 

#undef IntraPredict8x8Horz
#undef IntraPredict8x8Vert
#undef IntraPredict8x8DC
#undef IntraPredict8x8Plane
#undef IntraPredict8x8DCLeft
#undef IntraPredict8x8DCTop
#undef IntraPredict8x8DC128

#undef MemCopy16x16
#undef MemCopy16x8
#undef MemCopy8x16
#undef MemCopy8x8
#undef MemCopy
#undef MemSet

#undef Clip3
#undef GetMedian
#undef ArrayNonZeroCount4
#undef ArrayNonZeroCount8
#undef ArrayNonZeroCount16
#undef ScanZigzag16x16
#undef ScanZigzag8x8
#undef ScanZigzag4x4
#undef ScanZigzag2x2
#undef Deviation16x16
#undef F264CoeffCost16
#undef F264CoeffCost15

#define F264SAD16x16            SAD16x16
#define F264SAD16x8             SAD16x8
#define F264SAD8x16             SAD8x16
#define F264SAD8x8              SAD8x8

#define Interpolate16x16H       Intp6TapHalfPelH16x16
#define Interpolate16x16V       Intp6TapHalfPelV16x16
#define Interpolate16x16C       Intp6TapHalfPelHV16x16
#define Interpolate16x16        Intp6TapHalfPelALL16x16

#define MemCopy16x16            MemCopy16x16_map
#define MemCopy16x8             MemCopy16x8_map
#define MemCopy8x16             MemCopy8x16_map
#define MemCopy8x8              MemCopy8x8_map
#define PixelAverage16x16       PixAvg16x16

#define EighthPelMCChroma8x8    McUVEighthPix8x8
#define EighthPelMCChroma8x4    McUVEighthPix8x4
#define EighthPelMCChroma4x8    McUVEighthPix4x8
#define EighthPelMCChroma4x4    McUVEighthPix4x4

#define SubDCT8x8               H264SubDCT8x8
#define DCTandQuant16x16        H264DctandQuant16x16
#define AddiDCT8x8              H264AddiDCT8x8
#define AddiDCTandiQuant8x8     H264AddiDCTiQuant8x8
#define AddiDCTandiQuant16x16   H264AddiDCTiQuant16x16
#define DCT2x2DC                DCT2x2DC_map
#define Quant2x2DC              Quant2x2DC_map
#define iDCT2x2DC               DCT2x2DC
#define iQuant2x2DC             iQuant2x2DC_map
#define DCT4x4DC                Hadamard4x4
#define AddiDCT16x16            H264AddiDCT16x16
#define iDCT4x4DC               IHadamard4x4
#define SubDCT16x16             H264SubDCT16x16

#define Quant8x8(ps16Dct, u8Quant, u32IsIntra)    H264Quant8x8(ps16Dct, ps16Dct, u8Quant, 2 * u32IsIntra)
#define iQuant8x8(ps16Dct, u8Quant)               H264Dequant8x8(ps16Dct, ps16Dct, u8Quant)
#define Quant16x16(ps16Dct, u32Quant, u32IsIntra) H264Quant16x16_map(ps16Dct, ps16Dct, (u8)u32Quant, u32IsIntra ? 2 : 0)
#define iQuant16x16(pDct, u8Quant)                H264Dequant16x16_map(pDct, pDct, u8Quant)
#define Quant4x4DC(ps16DctDC, u32QP)              H264DCQuant4x4(ps16DctDC, ps16DctDC, u32QP, 2)
#define iQuant4x4DC(ps16DctDC, u32QP)             H264DCDequant4x4(ps16DctDC, ps16DctDC, u32QP)

#define IntraPredict16x16Horz   H264IntraPredict16x16Horz
#define IntraPredict16x16Vert   H264IntraPredict16x16Vert
#define IntraPredict16x16DC     H264IntraPredict16x16DC
#define IntraPredict16x16Plane  H264IntraPredict16x16Plane
#define IntraPredict16x16DCLeft H264IntraPredict16x16DCLeft
#define IntraPredict16x16DCTop  H264IntraPredict16x16DCTop
#define IntraPredict16x16DC128  H264IntraPredict16x16DC128

#define IntraPredict8x8Horz     H264IntraPredict8x8Horz
#define IntraPredict8x8Vert     H264IntraPredict8x8Vert
#define IntraPredict8x8DC       H264IntraPredict8x8DC
#define IntraPredict8x8Plane    H264IntraPredict8x8Plane
#define IntraPredict8x8DCLeft   H264IntraPredict8x8DCLeft
#define IntraPredict8x8DCTop    H264IntraPredict8x8DCTop
#define IntraPredict8x8DC128    H264IntraPredict8x8DC128

#define MemCopy                 MemCopy_map
#define MemSet                  MemSet_map
#define Clip3                   Clip3_map
#define GetMedian               GetMedian_map
#define ArrayNonZeroCount4      ArrayNonZeroCount4_map
#define ArrayNonZeroCount8      ArrayNonZeroCount8_map
#define ArrayNonZeroCount16     ArrayNonZeroCount16_map
#define ScanZigzag16x16         ScanZigzag16x16_map
#define ScanZigzag8x8           ScanZigzag8x8_map
#define ScanZigzag4x4           ScanZigzag4x4_map
#define ScanZigzag2x2           ScanZigzag2x2_map
#define Deviation16x16          Deviation16x16_map
#define F264CoeffCost16         F264CoeffCost16_map
#define F264CoeffCost15         F264CoeffCost15_map

#endif
