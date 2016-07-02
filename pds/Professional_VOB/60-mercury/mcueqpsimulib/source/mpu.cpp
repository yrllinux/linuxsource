// #include "kdvtype.h"
// #include "osp.h"
// #include "evmcueqp.h"
// #include "mcustruct.h"
// #include "mcuconst.h"
// #include "codeclib.h"
#include "kdvmulpic_hd.h"

s32 HardMPUInit(u32 nMode)
{
    return 0;
}

s32 HardMPUDestroy()
{
	return 0;
}


s32 CHardMulPic::SetMulPicType(s32 nNewType)
{
    return 0;
}

s32 CHardMulPic::StartMerge(TVideoEncParam* ptVideoEncParam, s32 nEncNum)
{
    return 0;
}

s32 CHardMulPic::AddChannel(s32 nChnNo)
{
    return 0;
}
	
s32 CHardMulPic::RemoveChannel(s32 nChnNo)
{
    return 0;
}

//新的头文件kdvmulpic_hd.h里面没有这个函数
// int CHardMulPic::SetBGDAndSidelineColor(TMulPicColor tBGDColor, TMulPicColor atSlidelineColor[MULPIC_MAX_CHNS], s32 bAddSlideLine)
// {
//     return 0;
// }

s32 CHardMulPic::SetFastUpdata(s32 nIndex)
{
    return 0;
}

s32 CHardMulPic::SetNoStreamBak(u32 BakType, u8 *pbyBmp, u32 dwBmpLen)
{
	return 0;
}

s32 CHardMulPic::SetVipChnl(TMulPicColor tVipSlideColor,u32 dwChnl)
{
	return 0;
}

s32 CHardMulPic::SetEncChnlParam(TVideoEncParam *ptEncParam, s32 nChannel)
{
	return 0;
}