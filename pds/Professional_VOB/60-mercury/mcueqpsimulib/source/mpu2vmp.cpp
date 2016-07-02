

#include "./dm816x/kdvmulpic_hd.h"

// CHardMulPic::CHardMulPic()
// {
// }
// CHardMulPic::~CHardMulPic()
// {
// }

	s32 CHardMulPic::Create(const TMultiPicCreate &tInit)
	{
		return HARD_MULPIC_OK;
	}


    /*函数名： Destory
      功    能： 
	  返 回 值：s32 
	  说    明： */
    s32 CHardMulPic::Destory()
	{
			return HARD_MULPIC_OK;
	}
    
//     s32 CHardMulPic::SetData(s32 nChnl, const TFrameHeader &tFrame)
// 	{
// 			return HARD_MULPIC_OK;
// 	}
// 	/*设置视频数据回调*/
// 	s32 CHardMulPic::SetVidDataCallback(s32 nChnl, FRAMECALLBACK fVidData, void* pContext)
// 	{
// 			return HARD_MULPIC_OK;
// 	} 
	
	s32 CHardMulPic::AddChannel(s32 nChnNo)
	{
			return HARD_MULPIC_OK;
	}
	s32 CHardMulPic::RemoveChannel(s32 nChnNo)
	{
			return HARD_MULPIC_OK;
	}
	
	s32 CHardMulPic::SetMulPicType(s32 nNewType)
	{
			return HARD_MULPIC_OK;
	}
	s32 CHardMulPic::StartMerge(TVideoEncParam* ptVideoEncParam, s32 nEncNum)
	{
			return HARD_MULPIC_OK;
	}
	s32 CHardMulPic::StartMerge()
	{
		return HARD_MULPIC_OK;
	}
	s32 CHardMulPic::SetVideoEncParam(TVideoEncParam* ptVideoEncParam, s32 nEncNum)
	{
		return HARD_MULPIC_OK;
	}
	s32 CHardMulPic::ChangeVideoEncParam(s32 nIndex, TVideoEncParam* ptVidEncParam)
	{
		return HARD_MULPIC_OK;
	}
// 	s32 CHardMulPic::StopMerge()
// 	{
// 			return HARD_MULPIC_OK;
// 	}

// 	s32 CHardMulPic::ChangeBitRate(s32 nIndex, u16 wBitRate/*kbps*/)
// 	{
// 			return HARD_MULPIC_OK;
// 	}
	s32 CHardMulPic::SetFastUpdata(s32 nIndex)
	{
			return HARD_MULPIC_OK;
	}
	
// 	s32 CHardMulPic::GetStatus(s32 nChnNo, TMulPicStatus &tMulPicStatus)
// 	{
// 			return HARD_MULPIC_OK;
// 	}
// 	s32 CHardMulPic::GetDecStatis(s32 nChnNo, TMulPicChnStatis &tMulPicStatis)
// 	{
// 			return HARD_MULPIC_OK;
// 	}
    s32 CHardMulPic::GetEncStatis(s32 nIndex, TMulPicChnStatis &tMulPicStatis)
	{
			return HARD_MULPIC_OK;
	}

	/*设置画面合成背景颜色tBGDColor,以及边框颜色tSlidelineColor[MULPIC_MAX_CHNS]，
	需要改变边框颜色时，只要改变tSlidelineColor[]对应通道的值即可*/
// 	s32 CHardMulPic::SetBGDAndSidelineColor(TMulPicColor tBGDColor,
//            TMulPicColor atSlidelineColor[MULPIC_MAX_CHNS], BOOL32 bAddSlideLine/* = FALSE*/)
// 	{
// 			return HARD_MULPIC_OK;
// 	}

	//可单独设置vip通道边框的颜色，与SetBGDAndSidelineColor函数可以同时使用
	s32 CHardMulPic::SetVipChnl(TMulPicColor tVipSlideColor,u32 dwChnl)
	{
			return HARD_MULPIC_OK;
	}

	/*开始加入台标*/
	s32 CHardMulPic::StartAddIcon(s32 nChnNo, u8 *pbyBmp, u32 dwBmpLen, u32 dwXPos, u32 dwYPos, u32 dwWidth,
								 u32 dwHeight, TBackBGDColor tBackBGDColor, u32 dwClarity)
	{
			return HARD_MULPIC_OK;
	}

	/* 停止加入台标 */
	s32 CHardMulPic::StopAddIcon(s32 nChnNo)
	{
			return HARD_MULPIC_OK;
	}
	s32 CHardMulPic::ChangeMode(u32 dwMode)
	{
		return HARD_MULPIC_OK;
	}

	s32 CHardMulPic::SetNoStreamBak(u32 dwBakType ,/*见enplayMode定义*/ u8 *pbyBmp /*= NULL*/, u32 dwBmpLen /*= 0*/, u32 dwChnlId /*= MULPIC_MAX_CHNS*/)
	{
		return HARD_MULPIC_OK;
	}

    s32 CHardMulPic::SetEncResizeMode(u32 dwMode)
    {
        return HARD_MULPIC_OK;
    }
// 	void RegsterCommands()
// 	{
// 
// 	}